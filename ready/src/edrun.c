/***********/
/* edrun.c */
/***********/

/*******************/
/* System includes */
/*******************/
#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <time.h>
#include <crtdbg.h>

/****************/
/* Self include */
/****************/
#include "edrun.h"

/******************/
/* Other includes */
/******************/
#include "ed.h"
#include "eddisp.h"
#include "edfail.h"
#include "edfile.h"
#include "edglob.h"
#include "edgui.h"
#include "edjdb.h"
#include "edlog.h"
#include "edprog.h"
#include "edprop.h"
#include "edtext.h"
#include "edwin.h"

#include "dbgctrl.h"

#include "jssignals.h"

#include "edcommands.h"
#include "eddialogs.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
#define PREPARING_EXECUTION	TRUE
#define STARTING_EXECUTION	FALSE

// These are constants for EdRun_DebuggerOutput and must
// correspond with those defined in ReadyDebug.java
#define ENV_MESSAGEBOX 			-41
#define ENV_PAUSE_AT_LOC		-42
#define ENV_PAUSE_NO_LOC		-43

/********************/
/* Global variables */
/********************/

/*********/
/* Types */
/*********/
typedef struct FileWindow
{
    const char	*path;
    char	*pseudoPath;
    HWND	editWindow;
    HWND	textDisplayWindow;
    TextPtr	textPtr;
    struct FileWindow *next;
} FileWindow, *FileWindowPtr;
    

typedef struct ThreadData
{
    HWND	consoleWindow;
    char	*className;
} ThreadData;

typedef struct AppletSize
{
    int		x, y;
} AppletSize;

typedef struct RunArgs
{
    BOOL	isApplet;
    char	commandLineArguments [4096];
    int		inputRedirection;
    int		outputRedirection;
    FilePath	inputFile, outputFile;
    int		appletWidth, appletHeight;
    char	appletParameterPairs [4096];
} RunArgs;

typedef struct ClassRead
{
    char		*javaBackslashPath;
    char		*classBackslashPath;
    char		*fullClassName;
    struct ClassRead	*next;
} ClassRead;

/**********************/
/* External variables */
/**********************/

/********************/
/* Static constants */
/********************/

/********************/
/* Static variables */
/********************/
// Properties of the File module
//static BOOL		stPropertiesChanged;

// I/O Handles used to communicate with the Java Supervisor
static HANDLE		stToJavaSupWriteDup, stFromJavaSupReadDup;
static HANDLE		stToJavaSupRead, stFromJavaSupWrite;

// Window containing main program
static HWND		stMainTextDisplayWindow;

// Path name of main program
static char		*stMainPathName;

// The original handle for stderr
static HANDLE		stOriginalStderrHandle;

// The handles for the pipe replacing stderr
static HANDLE		stReadPipeHandle, stWritePipeHandle;

// Redirection Arguments
static RunArgs		stRunArgs;

// Default Class Path
static char		stJARFiles [4096];

// Handle for the Java Program Runner Process
static HANDLE		stJavaRunnerProcessHandle;

// Handles for signals from Java Program Runner to Ready
static HANDLE		stNotifyReadyProgramPaused, stNotifyReadyProgramResumed;
static HANDLE		stNotifyReadyProgramFinished;
static HANDLE		stNotifyReadyProgramHalted;
static HANDLE		stNotifyReadySendingOutput;
static HANDLE		stNotifyReadySendingPrintout;
static HANDLE		stNotifyReadySendingException;
static HANDLE		stNotifyReadySendingFailMessage;
static HANDLE		stNotifyReadyReadyMustTerminate;

// Handles for signals from Ready to Java Program Runner    
static HANDLE		stNotifyJavaRunnerPause, stNotifyJavaRunnerResume;
static HANDLE		stNotifyJavaRunnerTerminate;

// Handle for signal to indicate Java process dead
static HANDLE		stJavaSignalHandlerKnowsProcessDead;

// Boolean flags indicating whether the program is running/paused
static BOOL		stJavaProgramRunning;
static BOOL		stJavaProgramPaused;
static BOOL		stJavaProgramRunningIsApplet;

// Handle to this process
static HANDLE		stReadyProcess;

// Handle to the log file
static HANDLE		stLogFileHandleDup;

// 
static BOOL		stRestartingJavaProgramRunner = FALSE;

// Other properties
static FileWindowPtr	stFileWindowHead, stFileWindowPtr;
static const char	*stFileWindowSearchDir;
// The window of the file containing the first error
static HWND		stErrorEditWindow, stErrorTextDisplayWindow;
static CRITICAL_SECTION	stJavaProcessCS;
static char		*stClassBeingCompiled;
static HWND		stCompilingWindow;
static char		*stHTMLBuffer;
// StringLink List holding all the files that the program read.
static StringLink	stJavaFilesReadDuringCompilation;
// The time that all files in the editor are supposed ot have
static time_t		stEditorFileTime;
// The package created
static char		stPackageName [1024];
// The code base
static FilePath		stCodeBase;
// Are we using a user-installed JVM
static BOOL		stUsingNewJVM;
// Whether we are currently tracing execution
static BOOL		stTracing = FALSE;
// Handles to register if a change takes place in a directory
static HANDLE		stClassDirChanged [2];
// The number of warnings and errors that occurred in compilation
static int		stErrors, stWarnings, stNumFiles;
// A handle to the list of classes that were read during compilation
static ClassRead	*stClassesReadHeader = NULL;

/******************************/
/* Static callback procedures */
/******************************/
static BOOL CALLBACK 	MyGetAppletSizeDialogProcedure (HWND pmDialog, 
							UINT pmMessage, 
    					       		WPARAM pmWParam, 
    					       		LPARAM pmLParam);
static UINT CALLBACK 	MyRunWithArgsDialogProcedure (HWND pmWindow, 
						      UINT pmMessage, 
    						      WPARAM pmWParam, 
    						      LPARAM pmLParam);

/*********************/
/* Static procedures */
/*********************/
static void		MyAddFileToList (const char *pmPathName, 
					  HWND pmEditWindow,
					  HWND pmTextDisplayWindow, 
					  TextPtr pmTextPtr, void *pmDummy);
static int		MyCheckForChangesInClassDirs_T (void);
static void		MyClearFilesList (void);
static void		MyConvertSlashToBackslash (char *pmString);
static void		MyConvertSlashToDot (char *pmString);
static void		MyCreateClassPath (char *pmClassPath);
static BOOL		MyCreateHTMLFile (HWND pmWindow, 
					  const char *pmHTMLPathName, 
					  const char *pmJavaFileName,
					  int *pmAppletWidth, 
					  int *pmAppletHeight,
					  char *pmAppletParams);
static void		MyFreeClassesReadList ();
static BOOL		MyGetDataFromHTMLFile (HWND pmWindow, 
					       const char *pmPathName, 
					       int *pmAppletWidth, 
					       int *pmAppletHeight,
					       char *pmAppletParams, 
					       BOOL pmShowError);
static int		MyGetIndentSize (char *pmPtr, char *pmStartPtr);
static void		MyGetJavaLibPath (char *pmJavaLibPath);
static int		MyGetJavaSupSignals_T (LPVOID pmWindow);
static BOOL		MyGetRunArgs (HWND pmWindow, const char *pmPathName, 
			      int pmProgramKind, int *pmAppletWidth,
			      int *pmAppletHeight, char *pmAppletParams);
static void		MyInterpretException_T (char *pmExceptionText);
static void		MyListJARFiles (void);
static char		*MyReadLine (char **pmPtr);
static BOOL		MyReadFromPipe (HANDLE pmReadPipeHandle, 
					char **pmBuffer);
static BOOL		MyReadWordFromString (char **pmPtr, char *pmWord);
static BOOL		MyRewriteHTMLFile (HWND pmWindow, 
					   const char *pmHTMLPathName, 
					   const char *pmJavaFileName,
					   int pmAppletWidth, 
					   int pmAppletHeight,
					   char *pmAppletParams);
static void		MySkipWhiteSpace (char **pmPtr);
static void		MyStatusExecuting (const char *pmClassName, 
					   BOOL pmPreparing);

/***********************/
/* External procedures */
/***********************/
extern void	EdJikes_Compile (int pmNumArgs, char *pmArguments[]);

/************************************************************************/
/* EdRun_Init								*/
/************************************************************************/
BOOL	EdRun_Init (void)
{
    int			myCFileDescriptor;
    FILE		*myFileHandle;
    FilePath		myPathName;
    HANDLE		myFromJavaSupRead, myToJavaSupWrite, myThread;
    SECURITY_ATTRIBUTES	myAttribs;
    DWORD		myThreadID;
    
    if ((gProgram.operatingSystem != WIN_95) && 
    	(gProgram.operatingSystem != WIN_95_OSR2))
    {
    	if (!EdJdb_Init (stCodeBase))
    	{
    	    return FALSE;
    	}
    }

    stJavaProgramRunning = FALSE;
    stJavaProgramPaused = FALSE;
    stJavaProgramRunningIsApplet = FALSE;
        
    stMainTextDisplayWindow = NULL;
    stMainPathName = NULL;

    stReadyProcess = OpenProcess (PROCESS_ALL_ACCESS, TRUE, 
    				  GetCurrentProcessId ());
    				  
    //
    // Initialize Run with Argument Structure
    //
    stRunArgs.isApplet = FALSE;
    stRunArgs.commandLineArguments [0] = 0;
    stRunArgs.inputRedirection = ARGS_IN_KEY;
    stRunArgs.outputRedirection = ARGS_OUT_SCREEN;
    stRunArgs.inputFile [0] = 0;
    stRunArgs.outputFile [0] = 0;
    stRunArgs.appletWidth = 200;
    stRunArgs.appletHeight = 200;
    stRunArgs.appletParameterPairs [0] = 0;

    //
    // Redirect standard output to a pipe.  All compiler error messages
    // will be sent to the pipe instead of standard output.
    //
    CreatePipe (&stReadPipeHandle, &stWritePipeHandle, NULL, 32768);
    stOriginalStderrHandle = GetStdHandle (STD_ERROR_HANDLE);
    SetStdHandle (STD_ERROR_HANDLE, stWritePipeHandle);
    myCFileDescriptor = _open_osfhandle ((int) stWritePipeHandle, _O_TEXT);
    myFileHandle = _fdopen (myCFileDescriptor, "w");
    *stderr = *myFileHandle;
    setvbuf (stderr, NULL, _IONBF, 0);

    //
    // Create all the signals going to and from the Java Program Runner    
    //
    stNotifyReadyProgramPaused = CreateEvent (NULL, TRUE, FALSE, 
    				   READY_NOTIFY_JAVA_PROGRAM_PAUSED);
    stNotifyReadyProgramResumed = CreateEvent (NULL, TRUE, FALSE, 
    				    READY_NOTIFY_JAVA_PROGRAM_RESUMED);
    stNotifyReadyProgramFinished = CreateEvent (NULL, TRUE, FALSE, 
    				     READY_NOTIFY_JAVA_PROGRAM_FINISHED);
    stNotifyReadyProgramHalted = CreateEvent (NULL, TRUE, FALSE, 
    				   READY_NOTIFY_JAVA_PROGRAM_HALTED);
    stNotifyReadySendingOutput = CreateEvent (NULL, TRUE, FALSE, 
    				READY_NOTIFY_SEND_OUTPUT);
    stNotifyReadySendingPrintout = CreateEvent (NULL, TRUE, FALSE, 
    				 READY_NOTIFY_PRINT_OUTPUT);
    stNotifyReadySendingException = CreateEvent (NULL, TRUE, FALSE, 
    				 READY_NOTIFY_EXCEPTION_OUTPUT);
    stNotifyReadySendingFailMessage = CreateEvent (NULL, TRUE, FALSE, 
    				 READY_NOTIFY_FAIL_MESSAGE);
    stNotifyReadyReadyMustTerminate = CreateEvent (NULL, TRUE, FALSE, 
    				      READY_NOTIFY_TERMINATE);

    stNotifyJavaRunnerPause = CreateEvent (NULL, TRUE, FALSE, 
    				   READY_MAIN_SAYS_PAUSE);
    stNotifyJavaRunnerResume = CreateEvent (NULL, TRUE, FALSE, 
    				   READY_MAIN_SAYS_RESUME);
    stNotifyJavaRunnerTerminate = CreateEvent (NULL, TRUE, FALSE, 
    				   READY_MAIN_SAYS_TERMINATE);

    stJavaSignalHandlerKnowsProcessDead = CreateEvent (NULL, TRUE, FALSE, NULL);

    InitializeCriticalSection (&stJavaProcessCS);
    
    // Initialize some static variables
    stFileWindowHead = NULL;
    stJavaFilesReadDuringCompilation = NULL;
    
    //
    // Read in the HTML file
    //
    EdFile_ConstructPath (myPathName, IDS_INI_DIR, IDS_HTML_BOILERPLATE);
        
    if (!EdFile_FileExists (myPathName))
    {
    	EdFail_Warn (IDS_HTMLBOILERMISSINGFAIL, __FILE__, __LINE__, 0,
    		     myPathName);
    	return FALSE;
    }
        
    // Read file into buffer
    stHTMLBuffer = EdFile_ReadFileIntoBuffer (NULL, myPathName, NULL, 
    				              OK_IF_NOT_FOUND);
    if (stHTMLBuffer == NULL)
    {
    	EdFail_Warn (IDS_HTMLBOILERREADFAIL, __FILE__, __LINE__, 0,
    		     myPathName);
    	return FALSE;
    }

    ZeroMemory (&myAttribs, sizeof (myAttribs));
    myAttribs.nLength = sizeof (myAttribs);
    myAttribs.bInheritHandle = TRUE;

    //
    // Create "To JavaSup" Pipe
    //
    if (!CreatePipe (&stToJavaSupRead, &myToJavaSupWrite, &myAttribs, 0))
    {
    	EdFail_Warn (IDS_CREATEPIPEFAIL, __FILE__, __LINE__, GetLastError ());
    	return FALSE;
    }
    
    if (!DuplicateHandle (GetCurrentProcess (), myToJavaSupWrite, 
    			  GetCurrentProcess (), &stToJavaSupWriteDup,
    			  0, FALSE, DUPLICATE_SAME_ACCESS))
    {
    	EdFail_Warn (IDS_DUPLICATEPIPEFAIL, __FILE__, __LINE__, 
    		     GetLastError ());
    	return FALSE;
    }

    // This leaves us with myToJavaSupRead (inheritable) and closed in the
    // next statement and stToJavaSupWriteDup (non-inheritable).  I'm not 
    // certain why it is important that we use only the non-inheritable 
    // version from here on in.  However, we at least do not attempt to 
    // pass it to another process.  We do pass the other side of the pipe,
    // stToJavaSupRead, to the JavaSup process.
    CloseHandle (myToJavaSupWrite);
    
    //
    // Create "From JavaSup" Pipe
    //
    if (!CreatePipe (&myFromJavaSupRead, &stFromJavaSupWrite, &myAttribs, 0))
    {
    	EdFail_Warn (IDS_CREATEPIPEFAIL, __FILE__, __LINE__, GetLastError ());
    	return FALSE;
    }
    
    if (!DuplicateHandle (GetCurrentProcess (), myFromJavaSupRead, 
    			  GetCurrentProcess (), &stFromJavaSupReadDup,
    			  0, FALSE, DUPLICATE_SAME_ACCESS))
    {
    	EdFail_Warn (IDS_DUPLICATEPIPEFAIL, __FILE__, __LINE__, 
    		     GetLastError ());
    	return FALSE;
    }

    // This leaves us with myFromJavaSupRead (inheritable) and closed in the
    // next statement and stFromJavaSupReadDup (non-inheritable).  I'm not 
    // certain why it is important that we use only the non-inheritable 
    // version from here on in.  However, we at least do not attempt to 
    // pass it to another process.  We do pass the other side of the pipe,
    // stFromJavaSupWrite, to the JavaSup process.
    CloseHandle (myFromJavaSupRead);
    
    if (gProperties.logging)
    {
    	HANDLE	myLogFileHandle = EdLog_GetLogFileHandle ();
    	
    	if (!DuplicateHandle (GetCurrentProcess (), myLogFileHandle, 
    			      GetCurrentProcess (), &stLogFileHandleDup,
    			      0, TRUE, DUPLICATE_SAME_ACCESS))
    	{
    	    EdFail_Warn (IDS_DUPLICATEPIPEFAIL, __FILE__, __LINE__, 
    		         GetLastError ());
    	    return FALSE;
    	}
    }
    
    // This leaves us with myLogFileHandle (non-inheritable) and
    // stLogFileHandleDup (inheritable).  We pass stLogFileHandleDup to
    // the JavaSup process.

    //
    // Create handles to check if there has been a change in the contents
    // of the class directories
    //
    EdFile_ConstructPath (myPathName, IDS_READY_CLASSES_DIR, DIR_ONLY_NO_SLASH);
    stClassDirChanged [0]  = FindFirstChangeNotification (myPathName, FALSE,
        FILE_NOTIFY_CHANGE_FILE_NAME);
    if (stClassDirChanged [0]  == NULL)
    {
    	EdFail_Warn (IDS_FINDFIRSTCHANGEFAIL, __FILE__, __LINE__, 
    		     GetLastError (), myPathName);
    	return FALSE;
    }

    EdFile_ConstructPath (myPathName, IDS_USER_CLASSES_DIR, DIR_ONLY_NO_SLASH);
    stClassDirChanged [1] = FindFirstChangeNotification (myPathName, FALSE,
        FILE_NOTIFY_CHANGE_FILE_NAME);
    if (stClassDirChanged [1]  == NULL)
    {
    	EdFail_Warn (IDS_FINDFIRSTCHANGEFAIL, __FILE__, __LINE__, 
    		     GetLastError (), myPathName);
    	return FALSE;
    }

    //
    // Get the list of JAR files
    //
    MyListJARFiles ();

    //
    // Start a thread that will check for changes to the directories.  When a
    // change occurs, send a message that recalculates the list of JAR files.
    //
    myThread = CreateThread (NULL, 0, 
        (LPTHREAD_START_ROUTINE) MyCheckForChangesInClassDirs_T, NULL, 0, 
        &myThreadID);
    if (myThread == 0)
    {
    	EdFail_Warn (IDS_CREATETHREADFAIL, __FILE__, __LINE__, GetLastError ());
    	return FALSE;
    }
    
    return TRUE;
} // EdRun_Init


/************************************************************************/
/* EdRun_Finalize							*/
/************************************************************************/
void	EdRun_Finalize (void)
{
    DeleteCriticalSection (&stJavaProcessCS);
    
    if (stMainPathName != NULL)
    {
    	free (stMainPathName);
    	stMainPathName = NULL;
    }

    if (stClassBeingCompiled != NULL)
    {
    	free (stClassBeingCompiled);
    }
    
    MyClearFilesList ();
    
    if (stHTMLBuffer != NULL)
    {
    	free (stHTMLBuffer);
    }
    
    CloseHandle (stToJavaSupWriteDup);
    CloseHandle (stFromJavaSupReadDup);
    CloseHandle (stToJavaSupRead);
    CloseHandle (stFromJavaSupWrite);

    //
    // Close all signals
    //
    CloseHandle (stNotifyReadyProgramPaused);
    CloseHandle (stNotifyReadyProgramResumed);
    CloseHandle (stNotifyReadyProgramFinished);
    CloseHandle (stNotifyReadyProgramHalted);
    CloseHandle (stNotifyReadySendingOutput);
    CloseHandle (stNotifyReadySendingPrintout);
    CloseHandle (stNotifyReadySendingException);
    CloseHandle (stNotifyReadySendingFailMessage);
    CloseHandle (stNotifyReadyReadyMustTerminate);
    
    CloseHandle (stNotifyJavaRunnerPause);
    CloseHandle (stNotifyJavaRunnerResume);
    CloseHandle (stNotifyJavaRunnerTerminate);
} // EdRun_Finalize


/************************************************************************/
/* EdRun_PropertiesSet							*/
/************************************************************************/
void	EdRun_PropertiesSet (void)
{
    EdRun_PropertiesImplementChanges ();
} // EdRun_PropertiesSet


/************************************************************************/
/* EdRun_PropertiesImplementChanges					*/
/************************************************************************/
void	EdRun_PropertiesImplementChanges (void)
{
} // EdRun_PropertiesImplementChanges


/************************************************************************/
/* EdRun_CloseAllRunWindows						*/
/*									*/
/* Not called by Java version of the editor.  Placeholder for link.	*/
/************************************************************************/
void	EdRun_CloseAllRunWindows (HWND pmEditWindow)
{
} // EdRun_CloseAllRunWindows


/************************************************************************/
/* EdRun_Compile							*/
/************************************************************************/
BOOL	EdRun_Compile (HWND pmWindow, const char *pmPathName, 
		       BOOL pmCompileOnly)
{
    char	*myClassName;
    FilePath	myPathName, myOriginalDirectory;
    char	*myArgs[10];
    char	myBootClassPath [4096], myClassPath [4096];
    char	myPackageName [1024];
    UINT	myPackageNameLen;
    char	*myBuffer;
    BOOL	myReadSuccess;
    BOOL	myResult;
    UINT	cnt;

    stCompilingWindow = pmWindow;
    myClassName = malloc (strlen (EdFile_GetFileName (pmPathName)) + 1);
    // TW Check for NULL
    strcpy (myClassName, EdFile_GetFileName (pmPathName));
    myClassName [EdFile_GetFileSuffix (myClassName) - myClassName] = 0;
    EdRun_StatusCompiling (myClassName);
        
    // Clear all previous errors
    EdErr_ClearErrors ();

    // We need to create pseudo-file names for all the classes in the 
    // editor that don't have associated files but are still Java classes.
    // This enables us to type in a program and run it without it ever
    // having been saved.
    
    MyClearFilesList ();
    Ed_EnumerateFiles (MyAddFileToList, NULL);
    
    // Determine the package of the class to be compiled
    {
    	char	*myTextBuffer;
    	int	myTextSize;
    	char	myDummyName [1024];
    	
    	if (EdRun_GetOpenFile (pmPathName, &myTextBuffer, &myTextSize))
    	{
    	    EdJava_GetPackageAndClass (myTextBuffer, myTextSize,
                stPackageName, myDummyName);
        }
        else
        {
            myTextBuffer = EdFile_ReadFileIntoBuffer (pmWindow, pmPathName, 
						      &myTextSize, TRUE);
    	    EdJava_GetPackageAndClass (myTextBuffer, myTextSize,
                stPackageName, myDummyName);
            free (myTextBuffer);
        }
    
        strcpy (myPackageName, stPackageName);
        myPackageNameLen = strlen (myPackageName);  
        for (cnt = 0 ; cnt < myPackageNameLen ; cnt++)
        {
            if (myPackageName [cnt] == '.')
            {
            	myPackageName [cnt] = '\\';
            }
        } // for

	strcat (myPackageName, "\\");
	myPackageNameLen++;
    } // end block
    
    
    // Set the directory to the program's directory.  If the class is in a
    // package, set it up a level.  i.e. compiling class MyClass in 
    // package C.D saved in A\B\C\D\MyClass.java is compiled in directory
    // A\B
    EdFile_GetCurrentDirectory (myOriginalDirectory);
    EdFile_GetDirectoryFromPath (pmPathName, stCodeBase);
    if ((myPackageNameLen > 0) && (strlen (stCodeBase) > myPackageNameLen) &&
	(stricmp (&stCodeBase [strlen (stCodeBase) - myPackageNameLen], 
		  myPackageName) == 0))
    {		  
	stCodeBase [strlen (stCodeBase) - myPackageNameLen] = 0;
    }
    EdFile_SetCurrentDirectory (stCodeBase);

    // Set up the boothclasspath (Jikes needs to know where Java class lib is
    MyGetJavaLibPath (myBootClassPath);
    
    // Set up the classpath.  
    MyCreateClassPath (myClassPath);
    
    // Set up the editor file time
    stEditorFileTime = time (NULL);
    
    // Set up the command line arguments
    // myArgs is not const char *, so we need to recopy pmPathName
    strcpy (myPathName, pmPathName);  	
    
    myArgs [0] = "Ready";
    myArgs [1] = "-bootclasspath";	// Set up bootclasspath
    myArgs [2] = myBootClassPath;
    myArgs [3] = "-classpath";		// Set up classpath
    myArgs [4] = myClassPath;
    myArgs [5] = "-source";
    if (gProperties.oldJavaCompile)
    {    
    	// Use Java 1.3
    	myArgs [6] = "1.3";
    }
    else
    {
    	// Use Java 1.4
    	myArgs [6] = "1.4";
    }
    myArgs [7] = myPathName;
    myArgs [8] = "+F";			// Full dependency check excluding ZIP

    stErrors = 0;
    stWarnings = 0;
    stNumFiles = 0;

    // Compile the program
    EdJikes_Compile (9, myArgs);
 
    // If Jikes sent messages to standard output or stderr, they can be
    // ignored.  However, send them to the logger, just in case.
    myReadSuccess = MyReadFromPipe (stReadPipeHandle, &myBuffer);
    
    if ((myReadSuccess) && (myBuffer != NULL))
    {
    	char	*myStart, *myEnd = myBuffer;
    	
    	while (TRUE)
    	{
    	    // Skip newlines
    	    while ((*myEnd == '\r') || (*myEnd == '\n') || 
    	           (((unsigned char) *myEnd) < 128))
    	    {
    	    	myEnd++;
    	    }
    	    if (*myEnd == 0) break;
    	    
    	    myStart = myEnd;
    	    while ((*myEnd != 0) && (*myEnd != '\r') && (*myEnd != '\n') &&
    	    	   (((unsigned char) *myEnd) < 128) && (myEnd - myStart < 450))
    	    {
    	    	myEnd++;
    	    }
    	    if (myStart != myEnd)
    	    {
    	    	char myChar = *myEnd;
    	    	*myEnd = 0;
    	 	EdFail_AddMessage (NORMAL, "Jikes: %s", myStart);
    	 	*myEnd = myChar;
    	    }
	} // while (true)    	     
    } // if MyReadFromPipe

    // Set the diretcory back to its original value
    EdFile_SetCurrentDirectory (myOriginalDirectory);    

    myResult = EdErr_DisplayNumberOfErrorsDialog (pmWindow, myClassName, 
    						  stErrors, stWarnings, 
    						  stNumFiles, pmCompileOnly);
    free (myClassName);
    	
    return myResult;
} // EdRun_Compile


/************************************************************************/
/* EdRun_CreateEXE							*/
/************************************************************************/
void	EdRun_CreateEXE (HWND pmTextDisplayWindow, FilePath pmPathName, 
			 BOOL pmCloseWindowsOnTerminate, 
			 BOOL pmDisplayRunWithArgs, BOOL pmCenterOutputWindow, 
			 BOOL pmStopUserClose)
{
    // So far, this does nothing.
} // EdRun_CreateEXE
			 

/************************************************************************/
/* EdRun_DebuggerOutput							*/
/*									*/
/* NOTE: This routine operates in the main thread, and is called by the	*/
/* Windows dispatcher.							*/
/************************************************************************/
void	EdRun_DebuggerOutput (int pmPurpose, char *pmOutputBuffer)
{
    if (pmPurpose == ENV_MESSAGEBOX)
    {
	EdGUI_Message ("Output from Debugger JVM", pmOutputBuffer);
    }
    else if (pmPurpose == ENV_PAUSE_AT_LOC)
    {
	// ENV_PAUSE_AT_LOC = Put an error message saying that we paused at 
	//		      this line.
	// pmOutputBuffer = classname.java:lineno or
	//                  dir\dir\dir\classname.java:lineno
	//
    	// We want to open the file (if possible), indicating that we've
    	// paused on a certain line.  We start with a class name and line
    	// number.  To find the associated files, we the file name to each
    	// directory in the class path and see if we've loaded that file in
    	// the compiler.  If we have, then we add an error to the file.  This
    	// slightly more complicated because a class may not actually have
    	// a file name attached, in which case we use the pseudo-file name
    	// found in the list started with stFileWindowHead.

	char		*myPtr = strchr (pmOutputBuffer, ':');
	int		myLine;
	ClassRead	*myClassReadPtr = stClassesReadHeader;
	char		*myPath = NULL;

	*myPtr++ = 0;
	myLine = atoi (myPtr);
	// pmOutputBuffer now points to "classname.java" or "dir\dir\dir\classname.java"
	// Convert it slashes to . and remove ".java".  Then compare it to the files
	// found in the ClassRead structure
	MyConvertSlashToDot (pmOutputBuffer);
	if (strcmp (&pmOutputBuffer [strlen (pmOutputBuffer) - 5], ".java") == 0)
	{
	    pmOutputBuffer [strlen (pmOutputBuffer) - 5] = 0;
	}

	while (myClassReadPtr != NULL)
	{
	    if (strcmp (pmOutputBuffer, myClassReadPtr -> fullClassName) == 0)
	    {
		myPath = myClassReadPtr -> javaBackslashPath;
		break;
	    }
	    myClassReadPtr = myClassReadPtr -> next;
	}

	if (myPath != NULL)
	{
	    EdErr_AddError (myPath, 0, myLine, 1, myLine, 0, "Paused at this line");

	    EdErr_FindFirstError (NULL, FALSE, TRUE);
	}
    }
    else if (pmPurpose == ENV_PAUSE_NO_LOC)
    {
    	Ed_SendStatusMessageToAllWindows ("Program paused");
    } 
    
    // We need to use GlobalFree because this memory block was allocated in
    // using GlobalAlloc by a DLL (which I guess has its own heap).
    GlobalFree (pmOutputBuffer);
} // EdRun_DebuggerOutput


/************************************************************************/
/* EdRun_Execute							*/
/*									*/
/* Executes a Java application or applet.  Execution is performed by 	*/
/* "javasup.exe" located in the "Support\bin" directory.  Pipes are	*/
/* opened ot talk with javasup.exe. The pipe to javasup.exe sends all	*/
/* the command line options necessary for execution.  The pipe from 	*/
/* javasup.exe is used for output to be sent to the printer or to be	*/
/* used in submissions.							*/
/************************************************************************/
BOOL	EdRun_Execute (HWND pmWindow, const char *pmPathName,
		       int pmProgramKind, BOOL pmWithArgs, BOOL pmStartWithStep)
{
    int			myAppletWidth, myAppletHeight;
    int			myOriginalAppletWidth, myOriginalAppletHeight;
    char		myAppletParams [4096], myOriginalAppletParams [4096];
    HANDLE		myThread;
    DWORD		myThreadID;
    char		myArgs [256];
    FilePath		myJavaSupPath;
    int			myResult;
    STARTUPINFO		myStartupInfo;
    FilePath		myDocumentBase;
    PROCESS_INFORMATION	myProcessInfo;
    FilePath		myJavaDLL;
    char		myClassPath [4096];
    char		myClassName [1024];
    char		myCommandLine [4096];
    DWORD		myBytesWritten;
    ThreadData		*myThreadData;
    SYSTEMTIME		mySystemTime;
    FILETIME		myFileTime;
    
    //
    // Get the class name
    //        
    strcpy (myClassName, EdFile_GetFileName (pmPathName));
    myClassName [EdFile_GetFileSuffix (myClassName) - myClassName] = 0;

    //
    // Display status message
    //
    MyStatusExecuting (myClassName, PREPARING_EXECUTION);
    
    //
    // Kill any running program, if necessary
    //
    stRestartingJavaProgramRunner = TRUE;
    EdRun_KillRunningProgram ();
    stRestartingJavaProgramRunner = FALSE;

    //
    // If it's an applet, get the information from the HTML file
    //
    if (pmProgramKind == APPLET)
    {
    	myAppletWidth = -1;
    	myAppletParams [0] = 0;
    	MyGetDataFromHTMLFile (pmWindow, pmPathName, &myAppletWidth, 
    			       &myAppletHeight, myAppletParams, TRUE);
	myOriginalAppletWidth = myAppletWidth;
	myOriginalAppletHeight = myAppletHeight;
	strcpy (myOriginalAppletParams, myAppletParams);
    } // if (pmProgramKind == APPLET)

    //
    // Get run arguments if necessary
    //
    if (pmWithArgs)
    {
    	stRunArgs.isApplet = (pmProgramKind == APPLET);
    	if (!MyGetRunArgs (pmWindow, pmPathName, pmProgramKind, &myAppletWidth,
    			   &myAppletHeight, myAppletParams))
    	{
    	    return FALSE;
    	}
    }

    //
    // Create the HTML file, if necessary
    //
    if (pmProgramKind == APPLET)
    {
    	FilePath	myHTMLName;

    	strcpy (myHTMLName, pmPathName);
    	
    	// Eliminate the ".java"
    	if (EdGlob_EndsWith (myHTMLName, JAVA_SUFFIX))
    	{
    	    myHTMLName [strlen (myHTMLName) - 5] = 0;
    	}
    	
    	strcat (myHTMLName, ".html");
    	
    	if (myOriginalAppletWidth == -1)
    	{
    	    // Create the HTML File
    	    if (!MyCreateHTMLFile (pmWindow, myHTMLName, 
    	    			   EdFile_GetFileName (pmPathName),
    	    			   &myAppletWidth, &myAppletHeight,
    	    			   myAppletParams))
    	    {
    	    	return FALSE;
    	    }
    	}
    	else if ((myOriginalAppletWidth != myAppletWidth) ||
    		 (myOriginalAppletHeight != myAppletHeight) ||
    		 (strcmp (myOriginalAppletParams, myAppletParams) != 0))
    	{
    	    // Rewrite the HTML File with new width, height & parameters
	    if (!MyRewriteHTMLFile (pmWindow, myHTMLName, 
    	    			    EdFile_GetFileName (pmPathName),
    	    			    myAppletWidth, myAppletHeight,
    	    			    myAppletParams))
	    {
	    	EdGUI_Message ("Problem Rewriting HTML file", 
	    			"Unable to rewrite the HTML file.  The new width, height and parameters will be used\nfor this run, but will not be saved.");
	    }	    	    	    
    	}
    } // if (pmProgramKind == APPLET)
        	
    //
    // Reset all the signals
    //
    ResetEvent (stNotifyReadyProgramPaused);
    ResetEvent (stNotifyReadyProgramResumed);
    ResetEvent (stNotifyReadyProgramFinished);
    ResetEvent (stNotifyReadyProgramHalted);
    ResetEvent (stNotifyReadySendingOutput);
    ResetEvent (stNotifyReadySendingPrintout);
    ResetEvent (stNotifyReadySendingException);
    ResetEvent (stNotifyReadySendingFailMessage);
    ResetEvent (stNotifyReadyReadyMustTerminate);
    
    ResetEvent (stNotifyJavaRunnerPause);
    ResetEvent (stNotifyJavaRunnerResume);
    ResetEvent (stNotifyJavaRunnerTerminate);

    //
    // Check for a timeout in the evaluation version.  Actually the
    // check occurs for all versions, but only the _EVAL_ version can
    // ever time out.
    //
    GetLocalTime (&mySystemTime);
    SystemTimeToFileTime (&mySystemTime, &myFileTime);
    
    if (CompareFileTime (&myFileTime, &gProgram.expiryDate) > 0)
    {
        // Put up an dialog box
    	EdGUI_Message1 (pmWindow, 0, IDS_DEMO_EXPIRED_TITLE,
    	        	IDS_DEMO_EXPIRED, gProgram.expiryDateString);
        return FALSE;
    }
        
    //
    // Create the Process
    //
    EdFile_ConstructPath (myJavaSupPath, IDS_READY_BIN, 
    	IDS_READY_PROGRAM_RUNNER);
    wsprintf (myArgs, "\"%s\" -pipe %d %d %d %d", myJavaSupPath, 
    	stToJavaSupRead, stFromJavaSupWrite, stReadyProcess, 
    	EdGUI_GetTopWindow (pmWindow));
    	
    ZeroMemory (&myStartupInfo, sizeof (myStartupInfo));
    myStartupInfo.cb = sizeof (myStartupInfo);

    if (gProperties.logging)
    {
        EdLog_Log ("[00] Java Prog Runner to have startup directory of \"%s\"", 
            stCodeBase);
    }    

    // We assume the stPackageName and stCodeBase, both set in EdRun_Compile
    // are accurate.  i.e. we never run a program we didn't compile
    myResult = CreateProcess (myJavaSupPath, myArgs, NULL, NULL, TRUE, 
        CREATE_NEW_PROCESS_GROUP | DETACHED_PROCESS, NULL, 
        stCodeBase, &myStartupInfo, &myProcessInfo);
    if (myResult == 0)
    {
    	ErrorBuffer	myErrorBuffer;
    	
    	EdGUI_Message (
    	    "Java Program Runner Creation Error",
    	    "Unable to start Java Program Runner located at\n\"%s\"."
    	    "\n\n%s\n[Error code: %d]",
    	    myJavaSupPath, EdFail_GetErrorMessage (myErrorBuffer), 
    	    GetLastError ());
    	EdLog_Log ("CreateProcess for Java Prog Runner failed with error = %d",
	    GetLastError ());
    	return FALSE;
    }
    stJavaRunnerProcessHandle = myProcessInfo.hProcess;
	
    //
    // Create thread to get messages back from JavaSup
    //
    myThreadData = (ThreadData *) malloc (sizeof (ThreadData));
    // TW Check for NULL
    myThreadData -> consoleWindow = pmWindow;
    myThreadData -> className = EdGlob_MallocString (myClassName);
    
    myThread = CreateThread (NULL, 0, 
        (LPTHREAD_START_ROUTINE) MyGetJavaSupSignals_T, myThreadData, 0, 
        &myThreadID);
    if (myThread == 0)
    {
    	ErrorBuffer	myErrorBuffer;
    	
    	EdGUI_Message (
    	    "Thread Creation Error",
    	    "Unable to create thread to control Java "
    	    "Program Runner.\n\n%s\n[Error code: %d]",
    	    EdFail_GetErrorMessage (myErrorBuffer), GetLastError ());
    	EdLog_Log ("CreateThread for Java Proc Monitor failed with error = %d",
	    GetLastError ());
	EdRun_KillRunningProgram ();
	return FALSE;
    }    		       

    //
    // Display status message
    //
    MyStatusExecuting (myClassName, STARTING_EXECUTION);
    
    //
    // Set the button bar correctly
    //
    Ed_SendMessageToAllWindows (FALSE, WM_SET_BUTTONS, SET_BUTTON_IS_EXECUTING, 
    				0);
    
    // 
    // Set the flags
    //
    stJavaProgramRunning = TRUE;
    stJavaProgramPaused = FALSE;
    stJavaProgramRunningIsApplet = (pmProgramKind != APPLICATION);
    
    //
    // Send the command line arguments to JavaSup
    //
    
    // Create the Java DLL Path
    EdRun_GetJavaDLLPath (myJavaDLL);
    
    // Create the class path
    MyCreateClassPath (myClassPath);
    
    // Create the document directory
    EdFile_GetDirectoryFromPath (pmPathName, myDocumentBase);
    
    // Eliminate last "\" from name
    myDocumentBase [strlen (myDocumentBase) - 1] = 0;
    
    // Create the class name to run (in pkg/pkg/class format)
    if (stPackageName [0] != 0)
    {
    	strcpy (myClassName, stPackageName);
        while (strchr (myClassName, '.') != NULL)
        {
            *strchr (myClassName, '.') = '/';
        }
    	strcat (myClassName, "/");
        strcat (myClassName, EdFile_GetFileName (pmPathName));
    }
    else
    {    	
    	strcpy (myClassName, EdFile_GetFileName (pmPathName));
    }
    myClassName [EdFile_GetFileSuffix (myClassName) - myClassName] = 0;

    // Create properties string
    //	  BOOL	fullScreen;
    //    int	textRows;
    //    int	textCols;
    //    int	textFontSize;
    //    char	textFontName [PROPERTY_LENGTH];
    //    int	caretWidth;
    //    BOOL	logging;
    //    int   logFileHandle;
    //    BOOL	logWindowMessages;
    //    int 	dialogFontSize;
    //    int	numSpacesPerTab;
    //    BOOL	verifyLineDB;
    
    wsprintf (myCommandLine , 
        "-java \"%s\" -cp \"%s\" -class %s -props %c %d %d %d \"%s\" %d %c %d %c %d %d %d %c %c",
        myJavaDLL, myClassPath, myClassName, 
//        ((stUsingNewJVM || 
//         (gProperties.useNewJVM && gProperties.alternateJavaDLL [0] != 0)) ? 
//            "-newjvm" : ""), 
        (gProperties.runConsoleFullScreen ? 'T' : 'F'), 
        gProperties.runConsoleTextRows, gProperties.runConsoleTextCols,
        gProperties.runConsoleFontSize, gProperties.runConsoleFontName,
	gProperties.editCaretWidth, 
        (gProperties.logging ? 'T' : 'F'), 
        stLogFileHandleDup,
        (gProperties.logWindowMessages ? 'T' : 'F'), 
	8, 
	gProperties.editNumSpacesPerTab, 
	gProperties.memoryForJVMInMB,
	((gProperties.JVMType == JRE_131) ? 'T' : 'F'),
        (gProperties.debug ? 'T' : 'F'));

    // Send Applet info
    if (pmProgramKind == APPLET)
    {
    	char	myAppletArguments [4096];

	// Note that the following will not work for applets in packages.	    	
    	wsprintf (myAppletArguments, " -applet \"%s\" \"%s\" %d %d %c%s%c", 
    		  stCodeBase, myDocumentBase, myAppletWidth, myAppletHeight, 
    		  255, myAppletParams, 255);
    	strcat (myCommandLine, myAppletArguments);
    } 

    // Set up debugger if necessary
    if (gProperties.startJavaRunnerInDebugger)
    {
    	strcat (myCommandLine, " -startindebugger");
    }
      
    // Specify input redirection
    if (pmWithArgs && (stRunArgs.inputRedirection == ARGS_IN_FILE))
    {
    	strcat (myCommandLine, " -i \"");
    	strcat (myCommandLine, stRunArgs.inputFile);
    	strcat (myCommandLine, "\"");
    }
    else if (pmWithArgs && (stRunArgs.inputRedirection == ARGS_IN_FILE_ECHO))
    {
    	strcat (myCommandLine, " -ii \"");
    	strcat (myCommandLine, stRunArgs.inputFile);
    	strcat (myCommandLine, "\"");
    }
    
    // Specify output redirection
    if (pmWithArgs && (stRunArgs.outputRedirection == ARGS_OUT_FILE))
    {
    	strcat (myCommandLine, " -o \"");
    	strcat (myCommandLine, stRunArgs.outputFile);
    	strcat (myCommandLine, "\"");
    }
    else if (pmWithArgs && (stRunArgs.outputRedirection == ARGS_OUT_FILE_SCREEN))
    {
    	strcat (myCommandLine, " -oo \"");
    	strcat (myCommandLine, stRunArgs.outputFile);
    	strcat (myCommandLine, "\"");
    }
    else if (pmWithArgs && (stRunArgs.outputRedirection == ARGS_OUT_PRINTER))
    {
    	strcat (myCommandLine, " -p");
    }
    else if (pmWithArgs && 
    	     (stRunArgs.outputRedirection == ARGS_OUT_PRINTER_SCREEN))
    {
    	strcat (myCommandLine, " -pp");
    }

    // If we're logging, then output the line to the log file
    if (gProperties.logging)
    {
    	EdLog_Log ("[01] Sent to Java Supervisor: \"%s\"", myCommandLine);
    }    

    // Debug messages
    EdFail_AddMessage (NORMAL, "Command to Java Supervisor - \"%s\"", 
    		       myCommandLine);
        
    // Write the command line to the Java supervisor
    if ((!WriteFile (stToJavaSupWriteDup, myCommandLine, 
    	             strlen (myCommandLine), &myBytesWritten, NULL)) || 
    	(myBytesWritten != strlen (myCommandLine)))
    {
    	ErrorBuffer	myErrorBuffer;
    	
    	EdGUI_Message (
    	    "Java Program Runner Communication Error",
    	    "Unable to send message (1) to Java Program Runner."
    	    "\n\n%s\n[Error code: %d]",
    	    EdFail_GetErrorMessage (myErrorBuffer), 
    	    GetLastError ());
    	EdLog_Log ("WriteFile (1) to Java Prog Runner failed with error = %d",
	    GetLastError ());

	EdRun_KillRunningProgram ();
    	return FALSE;
    }	

    // Write command line arguments, if any
    if (stRunArgs.commandLineArguments [0] != 0)
    {
    	char	myCommandLineArguments [4096];
    	
    	// The "!@!@" is a end of arguments flag
    	wsprintf (myCommandLineArguments, " -args %s !@!@", 
    	    stRunArgs.commandLineArguments);
    	    
	// If we're logging, then output the line to the log file
	if (gProperties.logging)
	{
	    EdLog_Log ("[02] Sent to Java Supervisor: \"%s\"", 
	    	       myCommandLineArguments);
	}    

    	if ((!WriteFile (stToJavaSupWriteDup, myCommandLineArguments, 
    	             strlen (myCommandLineArguments), &myBytesWritten, NULL)) || 
    	    (myBytesWritten != strlen (myCommandLineArguments)))
    	{
    	    ErrorBuffer	myErrorBuffer;
    	
    	    EdGUI_Message (
    	    	"Java Program Runner Communication Error",
    	    	"Unable to send message (2) to Java Program Runner."
    	    	"\n\n%s\n[Error code: %d]",
    	    	EdFail_GetErrorMessage (myErrorBuffer), 
    	    	GetLastError ());
    	    EdLog_Log ("WriteFile (2) to Java Prog Runner failed with error = %d",
	    	GetLastError ());

	    EdRun_KillRunningProgram ();
    	    return FALSE;
    	}
    }	
    
    // If we're logging, then output the line to the log file
    if (gProperties.logging)
    {
    	EdLog_Log ("[03] Sent to Java Supervisor: \"<null>\"");
    }    

    if ((!WriteFile (stToJavaSupWriteDup, "\0", 1, &myBytesWritten, NULL)) || 
    	(myBytesWritten != 1))
    {
    	ErrorBuffer	myErrorBuffer;
    	
    	EdGUI_Message (
    	    "Java Program Runner Communication Error",
    	    "Unable to send message (3) to Java Program Runner."
    	    "\n\n%s\n[Error code: %d]",
    	    EdFail_GetErrorMessage (myErrorBuffer), 
    	    GetLastError ());
    	EdLog_Log ("WriteFile (3) to Java Prog Runner failed with error = %d",
	    GetLastError ());

	EdRun_KillRunningProgram ();
    	return FALSE;
    }
    
    return TRUE;
} // EdRun_Execute


/************************************************************************/
/* EdRun_FixMain							*/
/************************************************************************/
void	EdRun_FixMain (HWND pmTextDisplayWindow)
{
    stMainTextDisplayWindow = pmTextDisplayWindow;
    if (stMainPathName != NULL)
    {
    	free (stMainPathName);
    	stMainPathName = NULL;
    }
} // EdRun_FixMain


/************************************************************************/
/* EdRun_FixMainPathName						*/
/************************************************************************/
void	EdRun_FixMainPathName (const char *pmPathName)
{
    EdRun_FixMain (NULL);
    stMainPathName = malloc (strlen (pmPathName) + 1);
    // TW Check for NULL
    strcpy (stMainPathName, pmPathName);
} // EdRun_FixMainPathName


/************************************************************************/
/* EdRun_FloatMain							*/
/************************************************************************/
void	EdRun_FloatMain (void)
{
    stMainTextDisplayWindow = NULL;
} // EdRun_FloatMain


/************************************************************************/
/* EdRun_GetJavaDLLPath							*/
/************************************************************************/
void	EdRun_GetJavaDLLPath (char *pmJavaDLLPath)
{
    // This is temporary - Does not handle user installed JRE/JDK's
    if (gProperties.JVMType == JRE_131)
    {
        EdFile_ConstructPath (pmJavaDLLPath, IDS_JAVA_BIN_131, IDS_JAVA_DLL);
    }
    else if (gProperties.JVMType == JRE_BUILT_IN)
    {
        EdFile_ConstructPath (pmJavaDLLPath, IDS_JAVA_BIN_BUILT_IN, 
        		      IDS_JAVA_DLL);
    }
    else
    {
    	EdFail_Fatal (IDS_BADJVMTYPE, __FILE__, __LINE__, 0,
    		      gProperties.JVMType);
    }
} // EdRun_GetJavaDLLPath


/************************************************************************/
/* EdRun_GetFileStat							*/
/*									*/
/* This is called by Jikes.						*/
/************************************************************************/
BOOL	EdRun_GetFileStat (char *pmPathName, struct stat *pmStat)
{
    int		myResult;
//    StringLink	myLink;
//    char	*myPtr;
        
    if (EdRun_MatchPath (pmPathName) == NULL)
    {
    	myResult = stat (pmPathName, pmStat);
    }
    else
    {
    	ZeroMemory (pmStat, sizeof (struct _stat));
    	pmStat -> st_mtime = stEditorFileTime;
    	myResult = 0;
    }
    
    return myResult;
} // EdRun_GetFileStat


/************************************************************************/
/* EdRun_GetJavaFileInitialize						*/
/*									*/
/* This function, along with EdRun_GetJavaFileInitialize, returns all	*/
/* the files that are currently open in the editor that would be	*/
/* considered to be in the specified directory.				*/
/************************************************************************/
void	EdRun_GetJavaFileInitialize (const char *pmDirectory)
{
    stFileWindowPtr = stFileWindowHead;
    stFileWindowSearchDir = pmDirectory;
} // EdRun_GetJavaFileInitialize


/************************************************************************/
/* EdRun_GetJavaFile							*/
/*									*/
/* See EdRun_GetJavaFileInitialize comment.				*/
/************************************************************************/
const char	*EdRun_GetJavaFile (void)
{
    const char    *myFileName;

    while (stFileWindowPtr != NULL)
    {
    	if (stFileWindowPtr -> pseudoPath != NULL)
    	{
    	    FilePath	myDirectory;
    	    UINT	cnt;
    	    
	    EdFile_GetDirectoryFromPath (stFileWindowPtr -> pseudoPath,
	    				 myDirectory);
	    for (cnt = 0 ; cnt < strlen (myDirectory) ; cnt++)
	    {
		if (myDirectory [cnt] == '\\')
		{
		    myDirectory [cnt] = '/';
		}
	    }
	    strcat (myDirectory, "*");
    	    if (strcmp (stFileWindowSearchDir, myDirectory) == 0)
    	    {
    	    	break;
    	    }
    	}
    	
    	stFileWindowPtr = stFileWindowPtr -> next;
    }

    if (stFileWindowPtr == NULL)
    {
    	return NULL;
    }
    
    myFileName = EdFile_GetFileName (stFileWindowPtr -> pseudoPath);
    	
    stFileWindowPtr = stFileWindowPtr -> next;

    return myFileName;
} // EdRun_GetJavaFile


/************************************************************************/
/* EdRun_GetMainFileNo							*/
/************************************************************************/
unsigned short	EdRun_GetMainFileNo (void)
{
    return 0;
} // EdRun_GetMainFileNo


/************************************************************************/
/* EdRun_GetMainPathName						*/
/************************************************************************/
const char	*EdRun_GetMainPathName (void)
{
    return stMainPathName;
} // EdRun_GetMainPathName


/************************************************************************/
/* EdRun_GetMainProgramType						*/
/************************************************************************/
int		EdRun_GetMainProgramType (void)
{
    // TW
    return APPLICATION;
} // EdRun_GetMainProgramType


/************************************************************************/
/* EdRun_GetMainWindow							*/
/************************************************************************/
HWND	EdRun_GetMainWindow (void)
{
    return stMainTextDisplayWindow;
} // EdRun_GetMainWindow


/************************************************************************/
/* EdRun_GetOpenFile							*/
/*									*/
/* This is called by Jikes.						*/
/************************************************************************/
BOOL	EdRun_GetOpenFile (const char *pmPathName, const char **pmBuffer, 
			   unsigned int *pmSize)
{
    TextPtr	myTextPtr;
    StringLink	myLink;
    myTextPtr = EdRun_MatchPath (pmPathName);

    if (myTextPtr == NULL)
    {
    	return FALSE;
    }

    *pmBuffer = myTextPtr -> text;
    *pmSize = myTextPtr -> textLen;

    //
    // Add this file to the list of files loaded.    
    //
    if (EdGlob_EndsWith (pmPathName, JAVA_SUFFIX))
    {
    	myLink = stJavaFilesReadDuringCompilation;
    	while (myLink != NULL)
    	{
    	    if (strcmp (myLink -> string, pmPathName) == 0)
    	    {
    	    	break;
    	    }
    	    myLink = myLink -> next;
    	}
    	if (myLink == NULL)
    	{
    	    myLink = EdGlob_CreateStringLink (pmPathName);
    	    // TW Check myLink == NULL
    	    myLink -> next = stJavaFilesReadDuringCompilation;
    	    stJavaFilesReadDuringCompilation = myLink;
    	}
    }
    
    return TRUE;
} // EdRun_GetOpenFile


/************************************************************************/
/* EdRun_NotifyClassRead						*/
/*									*/
/* This creates a linked list of the of the class files and where they	*/
/* were read from.							*/
/************************************************************************/
void	EdRun_NotifyClassRead (const char *pmJavaFile, 
			       const char *pmClassFile,
			       const char *pmFullClassName)
{
    ClassRead	*myPtr = calloc (1, sizeof (ClassRead));
    ClassRead	*myPrev = NULL;
    ClassRead	*myCurrent = stClassesReadHeader;

    // If the ".java" file is null and the other two aren't, there's a good 
    // chance that there is an entry here for the ".java" and ".class" file.  
    // If that's the case, then fill in the class name and refile 
    // appropriately.
    if ((pmJavaFile == NULL) && (pmClassFile != NULL) && 
	(pmFullClassName != NULL))
    {
	char	*myClassBackslashPath = EdGlob_MallocString (pmClassFile);
	
	MyConvertSlashToBackslash (myClassBackslashPath);

	while (myCurrent != NULL)
	{
	    if ((myCurrent -> classBackslashPath != NULL) &&
		(strcmp (myCurrent -> classBackslashPath, 
			 myClassBackslashPath) == 0))
	    {
		// Remove myCurrent from it's current position
		if (myPrev == NULL)
		{
		    stClassesReadHeader = myCurrent -> next;
		}
		else
		{
		    myPrev -> next = myCurrent -> next;
		}
		free (myPtr);
		myPtr = myCurrent;
		break;
	    }
	    myPrev = myCurrent;
	    myCurrent = myCurrent -> next;
	} // while

	free (myClassBackslashPath);
    } // if

    if ((pmJavaFile != NULL) && (myPtr -> javaBackslashPath == NULL))
    {
	myPtr -> javaBackslashPath = EdGlob_MallocString (pmJavaFile);
	MyConvertSlashToBackslash (myPtr -> javaBackslashPath);
    }
    if ((pmClassFile != NULL) && (myPtr -> classBackslashPath == NULL))
    {
	myPtr -> classBackslashPath = EdGlob_MallocString (pmClassFile);
	MyConvertSlashToBackslash (myPtr -> classBackslashPath);
    }
    if ((pmFullClassName != NULL) && (myPtr -> fullClassName == NULL))
    {
	myPtr -> fullClassName = EdGlob_MallocString (pmFullClassName);
	MyConvertSlashToDot (myPtr -> fullClassName);
    }

    // Insert the new class in order
    myCurrent = stClassesReadHeader;
    myPrev = NULL;
    while (myCurrent != NULL)
    {
	int myComp;
	
	if (myCurrent -> fullClassName == NULL)
	    if (myPtr -> fullClassName == NULL)
		myComp = strcmp (myCurrent -> javaBackslashPath, 
				 myPtr -> javaBackslashPath);
	    else
		myComp = -1;
	else
	    if (myPtr -> fullClassName == NULL)
		myComp = 1;
	    else
		myComp = strcmp (myCurrent -> fullClassName, 
				 myPtr -> fullClassName);

	if (myComp == 0)
	{
	    if (myPtr -> javaBackslashPath) free (myPtr -> javaBackslashPath);
	    if (myPtr -> classBackslashPath) free (myPtr -> classBackslashPath);
	    if (myPtr -> fullClassName) free (myPtr -> fullClassName);
	    free (myPtr);
	    return;
	}
	if (myComp > 0)
	{
	    break;
	}
	myPrev = myCurrent;
	myCurrent = myCurrent -> next;
    } // while

    myPtr -> next = myCurrent;
    if (myPrev == NULL)
    {
	stClassesReadHeader = myPtr;
    }
    else
    {
	myPrev -> next = myPtr;
    }
} // EdRun_NotifyClassRead


/************************************************************************/
/* EdRun_NotifyFileClosed						*/
/************************************************************************/
void	EdRun_NotifyFileClosed (HWND pmTextDisplayWindow)
{
} // EdRun_NotifyFileClosed


/************************************************************************/
/* EdRun_NotifyFileOpened						*/
/************************************************************************/
void	EdRun_NotifyFileOpened (HWND pmTextDisplayWindow)
{
} // EdRun_NotifyFileOpened


/************************************************************************/
/* EdRun_NotifyFileSaved						*/
/************************************************************************/
void	EdRun_NotifyFileSaved (HWND pmTextDisplayWindow)
{
} // EdRun_NotifyFileSaved


/************************************************************************/
/* EdRun_GetPseudoPath							*/
/************************************************************************/
BOOL	EdRun_GetPseudoPath (HWND pmWindow, char *pmText, int pmTextLen, 
			     char *pmPseudoPath)
{
    char	myPackageName [1024], myClassName [1024];
    FilePath	myActiveDirectory;
    UINT	cnt;

    if ((pmPseudoPath [0] != 0)	&& !EdGlob_EndsWith (pmPseudoPath, JAVA_SUFFIX))
    {
    	// If it has a file name and it doesn't end with ".java", don't
    	// try and process it.
    	return FALSE;
    }
    
    EdJava_GetPackageAndClass (pmText, pmTextLen, myPackageName, myClassName);
    if (myClassName [0] == 0)
    {
	return FALSE;
    }

    if (pmPseudoPath [0] != 0)
    {
    	// Let's compare the class name with the name the file is saved
    	// under and see if they are different.
    	char	*myPtr = &pmPseudoPath [strlen (pmPseudoPath) - 1];
    	
    	while (*myPtr != '\\')
    	{
    	    myPtr--;
    	}
	myPtr++;
    	// Make the "." into end of string
    	pmPseudoPath [strlen (pmPseudoPath) - 5] = 0;	
    	if (strcmp (myClassName, myPtr) == 0)
	{
	    return FALSE;
	}
	else
    	{
    	    // The file name differs from the class name.  Give the user the 
    	    // choice of using the class name.  Note that the error message
    	    // takes into account that the path has already had the ".java"
    	    // eliminated.
    	    int	myResult;

    	    myResult = EdGUI_Message1 (pmWindow, MB_YESNO, 
    	    			       IDS_ILLEGAL_FILE_NAME_TITLE, 
    	        		       IDS_USE_CLASS_NAME, myClassName, 
    	        		       EdFile_GetFileName (pmPseudoPath));
	    if (myResult == IDNO)
	    {
	    	return FALSE;
	    }

	    strcpy (myPtr, myClassName);
	    strcat (myPtr, JAVA_SUFFIX);
	}
	
	
	return TRUE;
    }
    
    // The pseudo path name will be:
    //    the active directory +
    //    the package name (with "." turned to "\") +
    //    "\" +
    //    class name +
    //    ".java"
    EdFile_GetActiveDirectory (myActiveDirectory);
	
    strcpy (pmPseudoPath, myActiveDirectory);
    if (myActiveDirectory [strlen (myActiveDirectory) - 1] != '\\')
    {
    	strcat (pmPseudoPath, "\\");
    }
    if (myPackageName [0] != 0)
    {
	for (cnt = 0 ; cnt < strlen (myPackageName) ; cnt++)
        {
    	    if (myPackageName [cnt] == '.')
    	    {
		myPackageName [cnt] = '\\';
	    }
	}
	strcat (pmPseudoPath, myPackageName);
	strcat (pmPseudoPath, "\\");
    }
    strcat (pmPseudoPath, myClassName);
    strcat (pmPseudoPath, ".java");
    
    return TRUE;
} // EdRun_GetPseudoPath


/************************************************************************/
/* EdRun_GotoLineInDocument						*/
/************************************************************************/
void	EdRun_GotoLineInDocument (unsigned short pmFileNo, 
				  unsigned short pmLineNo, BOOL forceFlags)
{
} // EdRun_GotoLineInDocument


/************************************************************************/
/* EdRun_InitializeForRunWithoutCompile					*/
/*									*/
/* Not called by Java version of the editor.  Placeholder for link.	*/
/************************************************************************/
void	EdRun_InitializeForRunWithoutCompile (HWND pmWindow)
{
} // EdRun_InitializeForRunWithoutCompile


/************************************************************************/
/* EdRun_IsAnyRunWindowVisible						*/
/*									*/
/* Not called by Java version of the editor.  Placeholder for link.	*/
/************************************************************************/
BOOL	EdRun_IsAnyRunWindowVisible (void)
{
    return FALSE;
} // EdRun_IsAnyRunWindowVisible


/************************************************************************/
/* EdRun_IsProgramPaused						*/
/************************************************************************/
BOOL	EdRun_IsProgramPaused (void)
{
    return stJavaProgramPaused;
} // EdRun_IsProgramPaused


/************************************************************************/
/* EdRun_IsProgramRunning						*/
/************************************************************************/
BOOL	EdRun_IsProgramRunning (void)
{
    return stJavaProgramRunning;
} // EdRun_IsProgramRunning


/************************************************************************/
/* EdRun_IsProgramRunningAnApplet					*/
/************************************************************************/
BOOL	EdRun_IsProgramRunningAnApplet (void)
{
    return stJavaProgramRunningIsApplet;
} // EdRun_IsProgramRunningAnApplet


/************************************************************************/
/* EdRun_IsMainPathName							*/
/************************************************************************/
BOOL	EdRun_IsMainPathName (const char *pmPathName)
{
    if (stMainPathName == NULL)
    {
    	return FALSE;
    }
    else
    {
    	return (stricmp (stMainPathName, pmPathName) == 0);
    }
} // EdRun_IsMainPathName


/************************************************************************/
/* EdRun_IsTracing							*/
/************************************************************************/
BOOL	EdRun_IsTracing (void)
{
    return FALSE;
} // EdRun_IsTracing


/************************************************************************/
/* EdRun_KillRunningProgram						*/
/*									*/
/* This process exits once the death of the thread has been noted by	*/
/* the myStWatchThread.							*/
/************************************************************************/
void	EdRun_KillRunningProgram (void)
{
    int	myResult;
    
    EnterCriticalSection (&stJavaProcessCS);
    if (stJavaRunnerProcessHandle != NULL)
    {
	// Reset the signal that the signal handler sends when 
	// the process quits
    	ResetEvent (stJavaSignalHandlerKnowsProcessDead);

	// Detach the debugger, if necessary
	EdJdb_DetachFromVM ();

   	// Attempt to terminate it by sending it a signal to quit
    	SetEvent (stNotifyJavaRunnerTerminate);

    	// Give it a fifth of a second to quit.
  	myResult = WaitForSingleObject (stJavaRunnerProcessHandle, 200);
  	
  	if (myResult == WAIT_TIMEOUT)
  	{
    	    // It still hasn't, terminate it using TerminateProcess.
    	    if (!TerminateProcess (stJavaRunnerProcessHandle, 0))
    	    {
    	    	ErrorBuffer	myErrorBuffer;
    	    	
	    	EdGUI_Message (
	    	    "Java Program Runner Termination Error",
	    	    "Unable to terminate Java Program Runner."
	    	    "\n\n%s\n[Error code: %d]",
	    	    EdFail_GetErrorMessage (myErrorBuffer), 
	    	    GetLastError ());
	    	EdLog_Log ("TerminateProcess failed with error = %d",
		    	   GetLastError ());
    	    }
    	}

	// The process is dead, Jim...
	stJavaRunnerProcessHandle = NULL;
        LeaveCriticalSection (&stJavaProcessCS);

    	// Wait for up to five seconds for an indication that the main signal
    	// handler has processed the process death signal
    	myResult = WaitForSingleObject (stJavaSignalHandlerKnowsProcessDead, 
    					30000);
    }
    else
    {
	LeaveCriticalSection (&stJavaProcessCS);
    }
} // EdRun_KillRunningProgram


/************************************************************************/
/* EdRun_KillRunningProgramAndQuit					*/
/*									*/
/* Not called by Java version of the editor.  Placeholder for link.	*/
/************************************************************************/
BOOL	EdRun_KillRunningProgramAndQuit (HWND pmWindow)
{
    EdRun_KillRunningProgram ();
    
    return FALSE;
} // EdRun_KillRunningProgramAndQuit


/************************************************************************/
/* EdRun_KillRunningProgramAndRerun					*/
/*									*/
/* Not called by Java version of the editor.  Placeholder for link.	*/
/************************************************************************/
void	EdRun_KillRunningProgramAndRerun (void)
{
} // EdRun_KillRunningProgramAndRerun


/************************************************************************/
/* EdRun_MatchPath							*/
/************************************************************************/
TextPtr	EdRun_MatchPath (const char *pmPathName)
{
    FileWindowPtr	myPtr;
    
    myPtr = stFileWindowHead;
    while (myPtr != NULL)
    {
    	//
    	// Check for match with path
    	//
    	if (myPtr -> path != NULL)
    	{
    	    const char	*myPath1, *myPath2;
    	    BOOL	myMatch = TRUE;
    	    
    	    myPath1 = myPtr -> path;
    	    myPath2 = pmPathName;

	    while ((*myPath1 != 0) && (*myPath2 != 0))
	    {
	    	if ((tolower (*myPath1) != tolower (*myPath2)) &&
	    	    (!((*myPath1 == '/') && (*myPath2 == '\\'))) &&
	    	    (!((*myPath1 == '\\') && (*myPath2 == '/'))))
		{
		    myMatch = FALSE;
		    break;
		}	    	    
		myPath1++;
		myPath2++;
	    }
	    
	    if (myMatch && (*myPath1 == *myPath2))
	    {
    	    	return myPtr -> textPtr;
    	    }
    	}

    	//
    	// Check for match with pseudo path
    	//
    	if (myPtr -> pseudoPath != NULL)
    	{
    	    const char	*myPath1, *myPath2;
    	    BOOL	myMatch = TRUE;
    	    
    	    myPath1 = myPtr -> pseudoPath;
    	    myPath2 = pmPathName;

	    while ((*myPath1 != 0) && (*myPath2 != 0))
	    {
	    	if ((tolower (*myPath1) != tolower (*myPath2)) &&
	    	    (!((*myPath1 == '/') && (*myPath2 == '\\'))) &&
	    	    (!((*myPath1 == '\\') && (*myPath2 == '/'))))
		{
		    myMatch = FALSE;
		    break;
		}	    	    
		myPath1++;
		myPath2++;
	    }
	    
	    if (myMatch && (*myPath1 == *myPath2))
	    {
    	    	return myPtr -> textPtr;
    	    }
    	}

    	myPtr = myPtr -> next;
    }
    
    return NULL;
} // EdRun_MatchPath


/************************************************************************/
/* EdRun_MatchTuringFileNo						*/
/*									*/
/* Not called by Java version of the editor.  Placeholder for link.	*/
/************************************************************************/
HWND	EdRun_MatchTuringFileNo (WORD pmTuringFileNo)
{
    return NULL;
} // EdRun_MatchTuringFileNo


/************************************************************************/
/* EdRun_PauseResumeProgram						*/
/************************************************************************/
void	EdRun_PauseResumeProgram (BOOL pmActivateSourceWindow, int pmReason)
{
    // Clear all previous errors
    EdErr_ClearErrors ();

    if (stJavaProgramPaused)
    {
    	// Attempt to terminate it by sending it a signal to quit
    	SetEvent (stNotifyJavaRunnerResume);
	if (gProperties.JVMType != JRE_131)
	{
	    EdJdb_Resume ();
	}
	stJavaProgramPaused = FALSE;
	Ed_SendMessageToAllWindows (TRUE, WM_SET_BUTTONS, 
	        		    SET_BUTTON_IS_EXECUTING, 0);
	Ed_SendStatusMessageToAllWindows ("Execution Resumed");
    }
    else
    {
    	// Attempt to terminate it by sending it a signal to quit
    	SetEvent (stNotifyJavaRunnerPause);
	if (gProperties.JVMType != JRE_131)
	{
	    EdJdb_Pause ();
	}
	stJavaProgramPaused = TRUE;
	Ed_SendMessageToAllWindows (TRUE, WM_SET_BUTTONS, 
	        		    SET_BUTTON_PAUSED, 0);
	Ed_SendStatusMessageToAllWindows ("Execution Paused");
    }
} // EdRun_PauseResumeProgram


/************************************************************************/
/* EdRun_ProcessJikesErrorMsg						*/
/************************************************************************/
void	EdRun_ProcessJikesErrorMsg (const char *pmPathName, 
			    int pmLeftLineNo, int pmLeftCol, int pmRightLineNo,
			    int pmRightCol, int pmIsWarning, 
			    const char *pmErrorMessage)
{
    if ((pmLeftLineNo == 0) && (pmLeftCol == 0) && (pmRightLineNo == 0) && 
    	(pmRightCol == 0))
    {
    	EdGUI_Message1 (Ed_GetLastActiveEditWindow (), 0, 
    			IDS_COMPILATION_ERROR_TITLE, IDS_USE_STRING, 
    			pmErrorMessage);
    }
    else
    {                
       	if (EdErr_AddError (pmPathName, 0, pmLeftLineNo, pmLeftCol, 
       			    pmRightLineNo, pmRightCol, pmErrorMessage))
	{
	    stNumFiles++;
	}	            			
    }
                
    if (pmIsWarning)
    {
       	stWarnings++;
    }
    else
    {
       	stErrors++;
    }
} // EdRun_ProcessJikesErrorMsg

			    
/************************************************************************/
/* EdRun_ResetCompiler							*/
/*									*/
/* Not called by Java version of the editor.  Placeholder for link.	*/
/************************************************************************/
void	EdRun_ResetCompiler (HWND pmEditWindow)
{
    // This function is purposely empty
} // EdRun_ResetCompiler


/************************************************************************/
/* EdRun_SetBreakpoint							*/
/************************************************************************/
void	EdRun_SetBreakpoint (HWND pmTextDisplayWindow, int pmLine)
{
    // TW To be filled in
} // EdRun_SetBreakpoint


/************************************************************************/
/* EdRun_SetStep							*/
/************************************************************************/
void	EdRun_SetStep (int pmStep)
{
    // TW To be filled in
} // EdRun_SetStep


/************************************************************************/
/* EdRun_SetTraceSpeed							*/
/************************************************************************/
void	EdRun_SetTraceSpeed (int pmTraceSpeed)
{
    // TW To be filled in
} // EdRun_SetTraceSpeed


/************************************************************************/
/* EdRun_StatusCompiling						*/
/************************************************************************/
void	EdRun_StatusCompiling (const char *pmClassName)
{
    char    myStatusMessage [1024];
	
    if (pmClassName != NULL)
    {
    	if (stClassBeingCompiled != NULL)
    	{
    	    free (stClassBeingCompiled);
    	}
    	
    	stClassBeingCompiled = malloc (strlen (pmClassName) + 1);
    	strcpy (stClassBeingCompiled, pmClassName);
    } 
    else if (stClassBeingCompiled == NULL)
    {
    	stClassBeingCompiled = malloc (strlen ("Program") + 1);
    	strcpy (stClassBeingCompiled, "Program");
    }

    wsprintf (myStatusMessage, "Compiling %s", stClassBeingCompiled);
    EdWin_ShowStatus (stCompilingWindow, myStatusMessage);
} // EdRun_StatusCompiling


/************************************************************************/
/* EdRun_TestSuite							*/
/************************************************************************/
void	EdRun_TestSuite (const char *pmDirectoryName, const char *pmOutputDir)
{
    // Used only by Turing (at this time).
} // EdRun_TestSuite


/************************************************************************/
/* EdRun_StatusFinishedReading						*/
/************************************************************************/
void	EdRun_StatusFinishedReading (void)
{
    if (IsWindow (stCompilingWindow))
    {
    	EdRun_StatusCompiling (NULL);
    }
} // EdRun_StatusFinishedReading


/************************************************************************/
/* EdRun_StatusReading							*/
/************************************************************************/
void	EdRun_StatusReading (const char *pmPathName)
{
    if (IsWindow (stCompilingWindow))
    {
    	const char	*myFileName;
    	char		myStatusMessage [1024];
    	
    	myFileName = EdFile_GetFileNameForwardSlash (pmPathName);
    	wsprintf (myStatusMessage, "Reading %s", myFileName);
	EdWin_ShowStatus (stCompilingWindow, myStatusMessage);    
    }
} // EdRun_StatusReading


/************************************************************************/
/* EdRun_ToggleBreakpointVisibility					*/
/************************************************************************/
void	EdRun_ToggleBreakpointVisibility (void)
{
    // TW To be filled in
} // EdRun_ToggleBreakpointVisibility


/************************************************************************/
/* EdRun_ToggleTracing							*/
/************************************************************************/
void	EdRun_ToggleTracing (void)
{
    stTracing = !stTracing;
    if (stJavaProgramRunning) 
    {
	if (stTracing)
	{
	    EdJdb_StartTracing ();
	}
	else
	{
	    EdJdb_StopTracing ();
	}
    }
} // EdRun_ToggleTracing


/******************************/
/* Static callback procedures */
/******************************/
/************************************************************************/
/* MyGetAppletSizeDialogProcedure					*/
/************************************************************************/
static BOOL CALLBACK 	MyGetAppletSizeDialogProcedure (HWND pmDialog, 
							UINT pmMessage, 
    					       		WPARAM pmWParam, 
    					       		LPARAM pmLParam)
{
    char		myNumber [128], myNumber1 [128];
    HWND		myOK;
    static AppletSize	*myStAppletSize;
    
    switch (pmMessage)
    {
    	case WM_INITDIALOG:
	    Ed_SetModalDialog (pmDialog); 
    	    EdGUI_CentreDialogBox (pmDialog);
	    myStAppletSize = (AppletSize *) pmLParam;
	    wsprintf (myNumber, "%d", myStAppletSize -> x);
	    SetDlgItemText (pmDialog, XSIZE_EDITBOX, myNumber);
	    wsprintf (myNumber, "%d", myStAppletSize -> y);
	    SetDlgItemText (pmDialog, YSIZE_EDITBOX, myNumber);
    	    return TRUE;
    	case WM_COMMAND:
    	    switch (LOWORD (pmWParam))
    	    {
    	    	case IDOK:
    	    	    GetDlgItemText (pmDialog, XSIZE_EDITBOX, myNumber, 
    	    	        sizeof (myNumber));
    	    	    myStAppletSize -> x = atoi (myNumber);
    	    	    GetDlgItemText (pmDialog, YSIZE_EDITBOX, myNumber, 
    	    	        sizeof (myNumber));
    	    	    myStAppletSize -> y = atoi (myNumber);
    	    	    EndDialog (pmDialog, TRUE);
    	    	    return TRUE;
    	    	case IDCANCEL:
    	    	    EndDialog (pmDialog, FALSE);
    	    	    return TRUE;
    	    	case XSIZE_EDITBOX:
    	    	case YSIZE_EDITBOX:
    	    	    if (HIWORD (pmWParam) == EN_UPDATE)
    	    	    {
    	    	    	EdGUI_DlgNumbersOnly (pmDialog, pmWParam);
    	    	    	GetDlgItemText (pmDialog, XSIZE_EDITBOX, myNumber, 
    	    	    	    sizeof (myNumber));
    	    	    	GetDlgItemText (pmDialog, YSIZE_EDITBOX, myNumber1, 
    	    	    	    sizeof (myNumber1));
    	    	    	myOK = GetDlgItem (pmDialog, IDOK);
     	    	    	
    	    	    	if ((myNumber [0] == 0) || (myNumber1 [0] == 0))
    	    	    	    EnableWindow (myOK, FALSE);
    	    	    	else
    	    	    	    EnableWindow (myOK, TRUE);
    	    	    }
    	    	    break;
    	    } // switch
    	    break;
    } // switch
    return FALSE;
} // MyGetAppletSizeDialogProcedure


/************************************************************************/
/* MyRunWithArgsDialogProcedure						*/
/************************************************************************/
static UINT CALLBACK 	MyRunWithArgsDialogProcedure (HWND pmDialog, 
						      UINT pmMessage, 
    						      WPARAM pmWParam, 
    						      LPARAM pmLParam)
{
    char		myNumber [128], myNumber1 [128];
    FilePath		myFileName;
    HWND		myItem;
    char		myAppletParams [4096], *myParamPtr;
    int			myNumberOfQuotes;
    int			myPos;
    static RunArgs	*myStRunArgs;
    
    switch (pmMessage)
    {
    	case WM_INITDIALOG:
	    Ed_SetModalDialog (pmDialog); 
    	    EdGUI_CentreDialogBox (pmDialog);
	    myStRunArgs = (RunArgs *) pmLParam;
	    // Set the command line arguments
	    SetDlgItemText (pmDialog, ARGS_COMMAND_LINE_ARGS, 
	        myStRunArgs -> commandLineArguments);
	    // Select the appropriate input redirection checkbox
	    myItem = GetDlgItem (pmDialog, ARGS_INPUT_FILE_NAME);
            CheckDlgButton (pmDialog, ARGS_IN_KEY, BST_UNCHECKED);
            CheckDlgButton (pmDialog, ARGS_IN_FILE, BST_UNCHECKED);
            CheckDlgButton (pmDialog, ARGS_IN_FILE_ECHO, BST_UNCHECKED);
            if (myStRunArgs -> inputRedirection == ARGS_IN_KEY)
            {
            	CheckDlgButton (pmDialog, ARGS_IN_KEY, BST_CHECKED);
            	ShowWindow (myItem, SW_HIDE);
            }
            else if (myStRunArgs -> inputRedirection == ARGS_IN_FILE)
            {
            	CheckDlgButton (pmDialog, ARGS_IN_FILE, BST_CHECKED);
            	wsprintf (myFileName, "File: %s", 
            	    EdFile_GetFileName (myStRunArgs -> inputFile));
	    	SetDlgItemText (pmDialog, ARGS_INPUT_FILE_NAME, myFileName);
            	ShowWindow (myItem, SW_SHOWNORMAL);
            }
            else
            {
            	CheckDlgButton (pmDialog, ARGS_IN_FILE_ECHO, BST_CHECKED);
            	wsprintf (myFileName, "File: %s", 
            	    EdFile_GetFileName (myStRunArgs -> inputFile));
	    	SetDlgItemText (pmDialog, ARGS_INPUT_FILE_NAME, myFileName);
            	ShowWindow (myItem, SW_SHOWNORMAL);
            }
	    // Select the appropriate output redirection checkbox
	    myItem = GetDlgItem (pmDialog, ARGS_OUTPUT_FILE_NAME);
            CheckDlgButton (pmDialog, ARGS_OUT_SCREEN, BST_UNCHECKED);
            CheckDlgButton (pmDialog, ARGS_OUT_FILE, BST_UNCHECKED);
            CheckDlgButton (pmDialog, ARGS_OUT_FILE_SCREEN, BST_UNCHECKED);
            CheckDlgButton (pmDialog, ARGS_OUT_PRINTER, BST_UNCHECKED);
            CheckDlgButton (pmDialog, ARGS_OUT_PRINTER_SCREEN, BST_UNCHECKED);
            if (myStRunArgs -> outputRedirection == ARGS_OUT_SCREEN)
            {
            	CheckDlgButton (pmDialog, ARGS_OUT_SCREEN, BST_CHECKED);
            	ShowWindow (myItem, SW_HIDE);
            }
            else if (myStRunArgs -> outputRedirection == ARGS_OUT_FILE)
            {
            	CheckDlgButton (pmDialog, ARGS_OUT_FILE, BST_CHECKED);
            	wsprintf (myFileName, "File: %s", 
            	    EdFile_GetFileName (myStRunArgs -> outputFile));
	    	SetDlgItemText (pmDialog, ARGS_OUTPUT_FILE_NAME, myFileName);
            	ShowWindow (myItem, SW_SHOWNORMAL);
            }
            else if (myStRunArgs -> outputRedirection == ARGS_OUT_FILE_SCREEN)
            {
            	CheckDlgButton (pmDialog, ARGS_OUT_FILE_SCREEN, BST_CHECKED);
            	wsprintf (myFileName, "File: %s", 
            	    EdFile_GetFileName (myStRunArgs -> outputFile));
	    	SetDlgItemText (pmDialog, ARGS_OUTPUT_FILE_NAME, myFileName);
            	ShowWindow (myItem, SW_SHOWNORMAL);
            }
            else if (myStRunArgs -> outputRedirection == ARGS_OUT_PRINTER)
            {
            	CheckDlgButton (pmDialog, ARGS_OUT_PRINTER, BST_CHECKED);
            	ShowWindow (myItem, SW_HIDE);
            }
            else
            {
            	CheckDlgButton (pmDialog, ARGS_OUT_PRINTER_SCREEN, BST_CHECKED);
            	ShowWindow (myItem, SW_HIDE);
            }
            // Set the applet width and height fields
	    wsprintf (myNumber, "%d", myStRunArgs -> appletWidth);
	    SetDlgItemText (pmDialog, ARGS_APPLET_WIDTH, myNumber);
	    wsprintf (myNumber, "%d", myStRunArgs -> appletHeight);
	    SetDlgItemText (pmDialog, ARGS_APPLET_HEIGHT, myNumber);
            // Set the applet parameter pairs
	    SetDlgItemText (pmDialog, ARGS_APPLET_PARAMS, 
	        myStRunArgs -> appletParameterPairs);
	        
	    // Now, if an application, grey out the applet only elements and
	    // vice-versa
	    if (myStRunArgs -> isApplet)
	    {
	    	// Disable the command line arguments
	        myItem = GetDlgItem (pmDialog, ARGS_COMMAND_LINE_ARGS);
	        EnableWindow (myItem, FALSE);
	    }
	    else	        
	    {
	    	// Disable a bunch of stuff
	        myItem = GetDlgItem (pmDialog, ARGS_APPLET_WIDTH);
	        EnableWindow (myItem, FALSE);
	        myItem = GetDlgItem (pmDialog, ARGS_APPLET_HEIGHT);
	        EnableWindow (myItem, FALSE);
	        myItem = GetDlgItem (pmDialog, ARGS_APPLET_PARAMS);
	        EnableWindow (myItem, FALSE);
	    }
    	    return TRUE;
    	case WM_COMMAND:
    	    switch (LOWORD (pmWParam))
    	    {
    	    	case IDOK:
    	    	    //
    	    	    // Check that the double quotes are paired in the command 
    	    	    // line arguments
    	    	    //
    	    	    
    	    	    // These aren't myAppletParams, we just use the variable
    	    	    // temporarily to save on stack space.
    	    	    GetDlgItemText (pmDialog, ARGS_COMMAND_LINE_ARGS, 
    	    	        myAppletParams, sizeof (myAppletParams));
    	    	    myParamPtr = myAppletParams;
    	    	    myNumberOfQuotes = 0;
    	    	    while (*myParamPtr != 0)
    	    	    {
    	    	    	if (*myParamPtr++ == '"')
    	    	    	   myNumberOfQuotes = (myNumberOfQuotes + 1) % 2;
		    }
		    if (myNumberOfQuotes != 0)
		    {
		        myItem = GetDlgItem (pmDialog, ARGS_COMMAND_LINE_ARGS);
			SetFocus (myItem);
		    	SendMessage (myItem, EM_SETSEL, 0, (LPARAM) -1);
			EdGUI_Message1 (pmDialog, MB_ICONEXCLAMATION, 
			    IDS_BAD_CMD_LINE_ARG_TITLE, IDS_NO_MATCHING_QUOTE);
	    	    	return FALSE;
		    }
		    	    	    
    	    	    //
    	    	    // Check the applet parameters are paired
    	    	    //
		    myItem = GetDlgItem (pmDialog, ARGS_APPLET_PARAMS);
    	    	    GetDlgItemText (pmDialog, ARGS_APPLET_PARAMS, 
    	    	        myAppletParams, sizeof (myAppletParams));
    	    	        
		    // Check out all the parameters
		    myParamPtr = myAppletParams;
		    while (TRUE)
		    {
		    	// Skip the whitespace
		    	while ((*myParamPtr == ' ') || (*myParamPtr == '\t') ||
		    	       (*myParamPtr == '\r') || (*myParamPtr == '\n'))
			{
			    myParamPtr++;
			}
			if (*myParamPtr == 0) break;

			// Make certain the user hasn't put a " in the value
			if (*myParamPtr == '"')
			{
			    EdGUI_Message1 (pmDialog, MB_ICONEXCLAMATION, 
			    	IDS_BAD_PARAMS_TITLE, IDS_QUOTE_IN_PARAM_NAME);
			    myPos = myParamPtr - myAppletParams;
			    SetFocus (myItem);
		    	    SendMessage (myItem, EM_SETSEL, (WPARAM) myPos, 
		    	    		 (LPARAM) myPos + 1);
	    	    	    return FALSE;
			}
			while ((*myParamPtr != '=') && (*myParamPtr != ' ') &&	
		    	       (*myParamPtr != '\r') && (*myParamPtr != '\n') &&
		    	       (*myParamPtr != '\t') && (*myParamPtr != 0))
			{		    	       
			    myParamPtr++;
			}
			if ((*myParamPtr == ' ') || (*myParamPtr == '\t'))
			{
			    EdGUI_Message1 (pmDialog, MB_ICONEXCLAMATION, 
			    	IDS_BAD_PARAMS_TITLE, IDS_SPACE_IN_PARAM_NAME);
			    myPos = myParamPtr - myAppletParams;
			    SetFocus (myItem);
		    	    SendMessage (myItem, EM_SETSEL, (WPARAM) myPos, 
		    	    		 (LPARAM) myPos + 1);
	    	    	    return FALSE;
			}
			if ((*myParamPtr == '\r') || (*myParamPtr == '\n') ||
			    (*myParamPtr == 0))
			{
			    EdGUI_Message1 (pmDialog, MB_ICONEXCLAMATION, 
			    	IDS_BAD_PARAMS_TITLE, IDS_NO_EQUAL_SIGN);
			    myPos = myParamPtr - myAppletParams;
			    SetFocus (myItem);
		    	    SendMessage (myItem, EM_SETSEL, (WPARAM) myPos, 
		    	    		 (LPARAM) myPos + 1);
	    	    	    return FALSE;
			}
			myParamPtr++;
			if (*myParamPtr == '"')
			{
			    EdGUI_Message1 (pmDialog, MB_ICONEXCLAMATION, 
			    	IDS_BAD_PARAMS_TITLE, IDS_NO_QUOTE_IN_VALUE);
			    myPos = myParamPtr - myAppletParams;
			    SetFocus (myItem);
		    	    SendMessage (myItem, EM_SETSEL, (WPARAM) myPos, 
		    	    		 (LPARAM) myPos + 1);
	    	    	    return FALSE;
			}
			while ((*myParamPtr != '\r') && (*myParamPtr != '\n') &&
		    	       (*myParamPtr != 0))
			{		    	       
			    myParamPtr++;
			}
			while ((*myParamPtr == '\r') || (*myParamPtr == '\n'))
			{		    	       
			    myParamPtr++;
			}
		    } // while			
    	    	        
    	    	    //
    	    	    // Copy the contents of the controls to the run args struct
    	    	    //    
    	    	    GetDlgItemText (pmDialog, ARGS_COMMAND_LINE_ARGS, 
    	    	        myStRunArgs -> commandLineArguments, 
    	    	        sizeof (myStRunArgs -> commandLineArguments));
    	    	    if (IsDlgButtonChecked (pmDialog, ARGS_IN_KEY) == 
    	    	    		BST_CHECKED)
    	    	    {
    	    	    	myStRunArgs -> inputRedirection = ARGS_IN_KEY;
    	    	    }
    	    	    else if (IsDlgButtonChecked (pmDialog, ARGS_IN_FILE) == 
    	    	    		BST_CHECKED)
    	    	    {
    	    	    	myStRunArgs -> inputRedirection = ARGS_IN_FILE;
    	    	    }
    	    	    else
    	    	    {
    	    	    	myStRunArgs -> inputRedirection = ARGS_IN_FILE_ECHO;
    	    	    }
    	    	    GetDlgItemText (pmDialog, ARGS_APPLET_WIDTH, myNumber, 
    	    	        sizeof (myNumber));
    	    	    myStRunArgs -> appletWidth = atoi (myNumber);
    	    	    GetDlgItemText (pmDialog, ARGS_APPLET_HEIGHT, myNumber, 
    	    	        sizeof (myNumber));
    	    	    myStRunArgs -> appletHeight = atoi (myNumber);
    	    	    if (IsDlgButtonChecked (pmDialog, ARGS_OUT_SCREEN) == 
    	    	    		BST_CHECKED)
    	    	    {
    	    	    	myStRunArgs -> outputRedirection = ARGS_OUT_SCREEN;
    	    	    }
    	    	    else if (IsDlgButtonChecked (pmDialog, ARGS_OUT_FILE) == 
    	    	    		BST_CHECKED)
    	    	    {
    	    	    	myStRunArgs -> outputRedirection = ARGS_OUT_FILE;
    	    	    }
    	    	    else if (IsDlgButtonChecked (pmDialog, 
    	    	    		ARGS_OUT_FILE_SCREEN) == BST_CHECKED)
    	    	    {
    	    	    	myStRunArgs -> outputRedirection = ARGS_OUT_FILE_SCREEN;
    	    	    }
    	    	    else if (IsDlgButtonChecked (pmDialog, ARGS_OUT_PRINTER) == 
    	    	    		BST_CHECKED)
    	    	    {
    	    	    	myStRunArgs -> outputRedirection = ARGS_OUT_PRINTER;
    	    	    }
    	    	    else
    	    	    {
    	    	    	myStRunArgs -> outputRedirection = 
    	    	    		ARGS_OUT_PRINTER_SCREEN;
    	    	    }
    	    	    GetDlgItemText (pmDialog, ARGS_APPLET_PARAMS, 
    	    	        myStRunArgs -> appletParameterPairs, 
    	    	        sizeof (myStRunArgs -> appletParameterPairs));
    	    	    EndDialog (pmDialog, TRUE);
    	    	    return TRUE;
    	    	case IDCANCEL:
    	    	    EndDialog (pmDialog, FALSE);
    	    	    return TRUE;
    	    	case ARGS_APPLET_WIDTH:
    	    	case ARGS_APPLET_HEIGHT:
    	    	    if (HIWORD (pmWParam) == EN_UPDATE)
    	    	    {
    	    	    	HWND	myOK;
    	    	    	
    	    	    	EdGUI_DlgNumbersOnly (pmDialog, pmWParam);
    	    	    	GetDlgItemText (pmDialog, ARGS_APPLET_WIDTH, myNumber, 
    	    	    	    sizeof (myNumber));
    	    	    	GetDlgItemText (pmDialog, ARGS_APPLET_HEIGHT, myNumber1, 
    	    	    	    sizeof (myNumber1));
    	    	    	myOK = GetDlgItem (pmDialog, IDOK);
     	    	    	
    	    	    	if ((myNumber [0] == 0) || (myNumber1 [0] == 0))
    	    	    	    EnableWindow (myOK, FALSE);
    	    	    	else
    	    	    	    EnableWindow (myOK, TRUE);
    	    	    }
    	    	    break;
		case ARGS_IN_KEY:
		    myItem = GetDlgItem (pmDialog, ARGS_INPUT_FILE_NAME);
            	    ShowWindow (myItem, SW_HIDE);
            	    break;
		case ARGS_IN_FILE:
		case ARGS_IN_FILE_ECHO:
		    myItem = GetDlgItem (pmDialog, ARGS_INPUT_FILE_NAME);
		    strcpy (myFileName, myStRunArgs -> inputFile);
		    if (!EdGUI_GetRedirectInputFile (pmDialog, myFileName))
		    {
		        CheckDlgButton (pmDialog, ARGS_IN_KEY, BST_CHECKED);
            		CheckDlgButton (pmDialog, ARGS_IN_FILE, BST_UNCHECKED);
            		CheckDlgButton (pmDialog, ARGS_IN_FILE_ECHO, 
            		    BST_UNCHECKED);
            	        ShowWindow (myItem, SW_HIDE);
		    	break;
		    }
		    strcpy (myStRunArgs -> inputFile, myFileName);
            	    wsprintf (myFileName, "File: %s", 
            	    	EdFile_GetFileName (myStRunArgs -> inputFile));
	    	    SetWindowText (myItem, myFileName);
            	    ShowWindow (myItem, SW_SHOWNORMAL);
            	    break;    
		case ARGS_OUT_SCREEN:
		case ARGS_OUT_PRINTER:
		case ARGS_OUT_PRINTER_SCREEN:
		    myItem = GetDlgItem (pmDialog, ARGS_OUTPUT_FILE_NAME);
            	    ShowWindow (myItem, SW_HIDE);
            	    break;
		case ARGS_OUT_FILE:
		case ARGS_OUT_FILE_SCREEN:
		    myItem = GetDlgItem (pmDialog, ARGS_OUTPUT_FILE_NAME);
		    strcpy (myFileName, myStRunArgs -> outputFile);
		    if (!EdGUI_GetRedirectOutputFile (pmDialog, myFileName))
		    {
            		CheckDlgButton (pmDialog, ARGS_OUT_SCREEN, 
            		    BST_CHECKED);
            		CheckDlgButton (pmDialog, ARGS_OUT_FILE, 
            		    BST_UNCHECKED);
            		CheckDlgButton (pmDialog, ARGS_OUT_FILE_SCREEN, 
            		    BST_UNCHECKED);
            		CheckDlgButton (pmDialog, ARGS_OUT_PRINTER, 
            		    BST_UNCHECKED);
            		CheckDlgButton (pmDialog, ARGS_OUT_PRINTER_SCREEN, 
            		    BST_UNCHECKED);
            	        ShowWindow (myItem, SW_HIDE);
		    	break;
		    }
		    strcpy (myStRunArgs -> outputFile, myFileName);
            	    wsprintf (myFileName, "File: %s", 
            	    	EdFile_GetFileName (myStRunArgs -> outputFile));
	    	    SetWindowText (myItem, myFileName);
            	    ShowWindow (myItem, SW_SHOWNORMAL);
            	    break;    
    	    } // switch
    	    break;
    } // switch
    return FALSE;
} // MyRunWithArgsDialogProcedure

    						      
/*********************/
/* Static procedures */
/*********************/
/************************************************************************/
/* MyAddFileToList							*/
/************************************************************************/
static void	MyAddFileToList (const char *pmPathName, HWND pmEditWindow,
				 HWND pmTextDisplayWindow, TextPtr pmTextPtr,
				 void *pmDummy)
{
    FileWindowPtr		myPtr;
    
    myPtr = (FileWindowPtr) malloc (sizeof (FileWindow));
    if (myPtr == NULL)
    {
    	// TW No mem
    	return;
    }

    // If there's a pseudo path, then use it, otherwise use the real path.
    if (EdDisp_GetPseudoPath (pmTextDisplayWindow) != NULL)
    {
	myPtr -> path = NULL;
	myPtr -> pseudoPath = EdDisp_GetPseudoPath (pmTextDisplayWindow);
    }
    else
    {
    	myPtr -> path = pmPathName;
    	myPtr -> pseudoPath = NULL;
    }
    myPtr -> editWindow = pmEditWindow;
    myPtr -> textDisplayWindow = pmTextDisplayWindow;
    myPtr -> textPtr = pmTextPtr;
    myPtr -> next = stFileWindowHead;
    stFileWindowHead = myPtr;
} // MyAddFileToList

				  
/************************************************************************/
/* MyCheckForChangesInClassDirs_T					*/
/*									*/
/* This thread sits waiting for files to be added or removed from the	*/
/* "Ready Classes" and "User Classes" directories.  When this occurs,	*/
/* it recalculates the list of JAR files.  Note that we're a little	*/
/* careless here, as there's the possibility of running a Java program	*/
/* at *exactly* the same time as we're updating the list of JAR files.  */
/* To do this properly, we should probably use a semaphore or the like.	*/
/* In this case, I consider the risk to be small enough not to warrant	*/
/* a fix.								*/
/************************************************************************/
static int	MyCheckForChangesInClassDirs_T (void)
{
    DWORD	myWaitStatus;
    
    while (TRUE) 
    { 
    	// Wait for notification.
        myWaitStatus = WaitForMultipleObjects (2, stClassDirChanged, FALSE, 
        				       INFINITE); 
	switch (myWaitStatus)
    	{ 
            case WAIT_OBJECT_0 + 0: 
                FindNextChangeNotification (stClassDirChanged [0]);
                break; 
 
            case WAIT_OBJECT_0 + 1: 
                FindNextChangeNotification (stClassDirChanged [1]);
                break;
        }
        
        MyListJARFiles ();
    }
} // MyCheckForChangesInClassDirs_T


/************************************************************************/
/* MyClearFilesList							*/
/*									*/
/* Clear the list of windows and associated files as well as the list	*/
/* of file dependencies.						*/
/************************************************************************/
static void	MyClearFilesList (void)
{
    FileWindowPtr	myPtr, myNext;
        
    myPtr = stFileWindowHead;
    while (myPtr != NULL)
    {
    	myNext = myPtr -> next;
    	free (myPtr);
    	myPtr = myNext;
    }
    
    stFileWindowHead = NULL;
    
    // Clear the list of file dependencies
    EdGlob_FreeStringLinkList (&stJavaFilesReadDuringCompilation);
} // MyClearFilesList


/************************************************************************/
/* MyConvertSlashToBackslash						*/
/************************************************************************/
static void	MyConvertSlashToBackslash (char *pmString)
{
    char    *myPtr = pmString;

    while (*myPtr != 0)
    {
	if (*myPtr == '/')
	{
	    *myPtr = '\\';
	}
	myPtr++;
    } // while
} // MyConvertSlashToBackslash


/************************************************************************/
/* MyConvertSlashToDot							*/
/************************************************************************/
static void	MyConvertSlashToDot (char *pmString)
{
    char    *myPtr = pmString;

    while (*myPtr != 0)
    {
	if ((*myPtr == '\\') || (*myPtr == '/'))
	{
	    *myPtr = '.';
	}
	myPtr++;
    } // while
} // MyConvertSlashToDot


/************************************************************************/
/* MyCreateClassPath							*/
/*									*/
/* Set up the classpath.  						*/
/*	The classpath consists of:					*/
/*		1) The current directory				*/
/*		2) [Ready]\support\ready classes			*/
/*		3) [Ready]\support\classes				*/
/*		4) Any additional class path elements			*/
/*		5) ".jar" files found in [Ready]\support\ready classes	*/
/*		6) ".jar" files found in [Ready]\support\user classes	*/
/************************************************************************/
static void	MyCreateClassPath (char *pmClassPath)
{
    FilePath	myPathName;
    
    // 1) The current directory
    strcpy (pmClassPath, ".;");
    
    // 2) [Ready]\support\ready classes
    EdFile_ConstructPath (myPathName, IDS_READY_CLASSES_DIR, DIR_ONLY_NO_SLASH);
    strcat (pmClassPath, myPathName);
    strcat (pmClassPath, ";");
        
    // 3) [Ready]\support\user classes	
    EdFile_ConstructPath (myPathName, IDS_USER_CLASSES_DIR, DIR_ONLY_NO_SLASH);
    strcat (pmClassPath, myPathName);
    strcat (pmClassPath, ";");
    
    // 4) Any additional class path elements
    if (gProperties.additionalClassPath [0] != 0)
    {
        strcat (pmClassPath, gProperties.additionalClassPath);
        if (pmClassPath [strlen (pmClassPath) - 1] != ';')
        {
	    strcat (pmClassPath, ";");
	}
    }
    
    // 5) ".jar" files found in [Ready]\support\ready classes
    // 6) ".jar" files found in [Ready]\support\user classes
    strcat (pmClassPath, stJARFiles);
} // MyCreateClassPath


/************************************************************************/
/* MyCreateHTMLFile							*/
/************************************************************************/
static BOOL	MyCreateHTMLFile (HWND pmWindow, const char *pmHTMLPathName, 
				  const char *pmJavaFileName,
				  int *pmAppletWidth, int *pmAppletHeight,
				  char *pmAppletParams)
{
    char		myClassName [256];
    HINSTANCE		myApplicationInstance;
    FileEntryPtr	myHTMLFile;
    char		myGenericClassName [80];
    char		myXSizeHolder [80], myYSizeHolder [80];
    char		myParamHolder [80];
    char		*myPtr, *myStart, myChar = 0;
    int			myErrorCode;
    AppletSize		myAppletSize;
    
    strcpy (myClassName, pmJavaFileName);
    if (EdGlob_EndsWith (myClassName, JAVA_SUFFIX))
    {
    	myClassName [strlen (myClassName) - 5] = 0;
    }
    
    // Get the program instance
    myApplicationInstance = 
        (HINSTANCE) GetWindowLong (pmWindow, GWL_HINSTANCE);
    
    if (*pmAppletWidth == -1)
    {
	BOOL	myResult;

    	myAppletSize.x = gProperties.runAppletWidth;
    	myAppletSize.y = gProperties.runAppletHeight;    

	Ed_EnableAllWindows (FALSE, pmWindow);
    	myResult = DialogBoxParam (myApplicationInstance, 
            MAKEINTRESOURCE (GET_APPLET_SIZE_DIALOG),
	    pmWindow, MyGetAppletSizeDialogProcedure, (LPARAM) &myAppletSize);
	Ed_EnableAllWindows (TRUE, pmWindow);

	if (!myResult)
    	{
	    return FALSE;
	}
	*pmAppletWidth = myAppletSize.x;
	*pmAppletHeight = myAppletSize.y;
    }
    else
    {
    	myAppletSize.x = *pmAppletWidth;
    	myAppletSize.y = *pmAppletHeight;    
    }

    
    myHTMLFile = EdFile_OpenFileForWrite (pmHTMLPathName, &myErrorCode,
    					  FALSE, FALSE);
    if (myHTMLFile == NULL)
    {
    	// Write to log
    	EdGUI_Message ("HTML Write failure", 
    	    "Unable to open HTML file (\"%s\") for writing.\n"
    		"Error = %d", pmHTMLPathName, myErrorCode);
    	return FALSE;
    }
    
    //
    // Go through entire file, converting ClassName to the name of the class.
    //
    myPtr = stHTMLBuffer;
    myStart = stHTMLBuffer;
    EdGUI_LoadString (IDS_CLASSNAMEHOLDER, myGenericClassName,
    		      sizeof (myGenericClassName));
    EdGUI_LoadString (IDS_APPLETWIDTHHOLDER, myXSizeHolder, 
    		      sizeof (myXSizeHolder));
    EdGUI_LoadString (IDS_APPLETHEIGHTHOLDER, myYSizeHolder, 
    		      sizeof (myYSizeHolder));
    EdGUI_LoadString (IDS_APPLETPARAMETERSHOLDER, myParamHolder, 
    		      sizeof (myParamHolder));
    while (*myPtr != 0)
    {
    	if (strnicmp (myPtr, myGenericClassName, strlen (myGenericClassName)) 
    									== 0)
    	{
    	    if (myStart != myPtr)
    	    {
    	    	myChar = *myPtr;
    	    	*myPtr = 0;
    	    	EdFile_Write (myHTMLFile, myStart);
    	    	*myPtr = myChar;
    	    }

	    EdFile_Write (myHTMLFile, myClassName);
	    myPtr += strlen (myGenericClassName);
	    myStart = myPtr;
    	}
    	else if (strnicmp (myPtr, myXSizeHolder, strlen (myXSizeHolder)) == 0)
    	{
    	    char	myNumber [64];
    	    
    	    if (myStart != myPtr)
    	    {
    	    	myChar = *myPtr;
    	    	*myPtr = 0;
    	    	EdFile_Write (myHTMLFile, myStart);
    	    	*myPtr = myChar;
    	    }

    	    wsprintf (myNumber, "%d", myAppletSize.x);
	    EdFile_Write (myHTMLFile, myNumber);
	    myPtr += strlen (myXSizeHolder);
	    myStart = myPtr;
    	}
    	else if (strnicmp (myPtr, myYSizeHolder, strlen (myYSizeHolder)) == 0)
    	{
    	    char	myNumber [64];
    	    
    	    if (myStart != myPtr)
    	    {
    	    	myChar = *myPtr;
    	    	*myPtr = 0;
    	    	EdFile_Write (myHTMLFile, myStart);
    	    	*myPtr = myChar;
    	    }

    	    wsprintf (myNumber, "%d", myAppletSize.y);
	    EdFile_Write (myHTMLFile, myNumber);
	    myPtr += strlen (myYSizeHolder);
	    myStart = myPtr;
	}
    	else if (strnicmp (myPtr, myParamHolder, strlen (myParamHolder)) == 0)
    	{
    	    char	*myPtr1;
    	    int		myIndentSize;
    	    BOOL	myFirstLine = TRUE, myLastParam = FALSE;
    	    char	*myParamPtr = pmAppletParams;
	    char	myIndent [256];
    	    char	myChar;
    	    int		cnt;
    	    
    	    //
    	    // In order to avoid writing a blank line, we only write up to
    	    // the new line.  If there are no parameters to write, we then
    	    // skip to the end of the line with the [param] marker.
    	    //
    	    myPtr1 = myPtr;
    	    while ((myPtr1 > myStart) && (*(myPtr1 - 1) == ' '))
    	    {
    	    	myPtr1--;
    	    }
    	    
    	    if (myStart != myPtr1)
    	    {
    	    	myChar = *myPtr1;
    	    	*myPtr1 = 0;
    	    	EdFile_Write (myHTMLFile, myStart);
    	    	*myPtr1 = myChar;
    	    	myStart = myPtr1;
    	    }

	    // Determine the indentation of the Param Holder and create a
	    // string with that many spaces
	    myIndentSize = MyGetIndentSize (myPtr, stHTMLBuffer);
	    for (cnt = 0 ; cnt < myIndentSize ; cnt++)
	    {
	    	myIndent [cnt] = ' ';
	    }
	    myIndent [myIndentSize] = 0;
	    
	    // Write out all the params
	    while (TRUE)
	    {
	    	while ((*myParamPtr == ' ') || (*myParamPtr == '\t') ||
	    	       (*myParamPtr == '\r') || (*myParamPtr == '\n'))
		{
		    myParamPtr++;
		}
		if (*myParamPtr == 0) break;
		
		if (!myFirstLine)
		{
		    // Write a newline and indent
		    EdFile_WriteNL (myHTMLFile);
		}
		else
		{
		    myFirstLine = FALSE;
		}
		
		EdFile_Write (myHTMLFile, myIndent);

	    	EdFile_Write (myHTMLFile, "<param name=\"");
	    	myStart = myParamPtr;
	    	while (*myParamPtr != '=') myParamPtr++;
	    	*myParamPtr = 0;
	    	EdFile_Write (myHTMLFile, myStart);
	    	*myParamPtr++ = '=';
	    	EdFile_Write (myHTMLFile, "\" value=\"");
	    	myStart = myParamPtr;
	    	while ((*myParamPtr != '\r') && (*myParamPtr != '\n') &&
	    	       (*myParamPtr != 0))
		{
		    myParamPtr++;
		}
		myChar = *myParamPtr;
		*myParamPtr = 0;
	    	EdFile_Write (myHTMLFile, myStart);
	    	*myParamPtr = myChar;
	    	EdFile_Write (myHTMLFile, "\">");
	    	if (myChar == 0) break;
	    } // while
	    
	    myPtr += strlen (myParamHolder);
	    
	    // Skip the rest of the line if there wasn't any params
	    if (myFirstLine)
	    {
	    	while (*myPtr == ' ')
	    	{
	    	    myPtr++;
	    	}
	    	if (*myPtr == '\r')
	    	{
	    	    myPtr++;
	    	}
	    	if (*myPtr == '\n')
	    	{
	    	    myPtr++;
	    	}
	    }
	    myStart = myPtr;
	}
	else
    	{
	    myPtr++;
	}
    }

    if (myStart != myPtr)
    {
    	EdFile_Write (myHTMLFile, myStart);
    }

    EdFile_CloseFile (myHTMLFile);
    
    return TRUE;
} // MyCreateHTMLFile


/************************************************************************/
/* MyFreeClassesReadList						*/
/************************************************************************/
static void	MyFreeClassesReadList ()
{
    ClassRead	*myNext, *myPtr = stClassesReadHeader;

    stClassesReadHeader = NULL;

    while (myPtr != NULL)
    {
	if (myPtr -> javaBackslashPath) free (myPtr -> javaBackslashPath);
	if (myPtr -> classBackslashPath) free (myPtr -> classBackslashPath);
	if (myPtr -> fullClassName) free (myPtr -> fullClassName);
	myNext = myPtr -> next;
	free (myPtr);
	myPtr = myNext;
    }
} // MyFreeClassesReadList


/************************************************************************/
/* MyGetDataFromHTMLFile						*/
/************************************************************************/
static BOOL	MyGetDataFromHTMLFile (HWND pmWindow, const char *pmPathName, 
				       int *pmAppletWidth, int *pmAppletHeight,
				       char *pmAppletParams, BOOL pmShowError)
{
    FilePath	myHTMLName;
    char	myClassName [1024];
    char	*myHTMLBuffer;
    char	*myPtr;
    char	myWord [1024];
    BOOL	myMatch = FALSE;
    int		myWidth, myHeight;
    
    strcpy (myHTMLName, pmPathName);
    	
    // Eliminate the ".java"
    if (EdGlob_EndsWith (myHTMLName, JAVA_SUFFIX))
    {
        myHTMLName [strlen (myHTMLName) - 5] = 0;
    }
    strcat (myHTMLName, ".html");
    	
    if (!EdFile_FileExists (myHTMLName))
    {
	return FALSE;
    }

    // Create class name
    strcpy (myClassName, EdFile_GetFileName (pmPathName));
    if (EdGlob_EndsWith (myClassName, JAVA_SUFFIX))
    {
        myClassName [strlen (myClassName) - 5] = 0;
    }
    strcat (myClassName, ".class");

    // Read file into buffer
    myHTMLBuffer = EdFile_ReadFileIntoBuffer (NULL, myHTMLName, NULL, 
    				              OK_IF_NOT_FOUND);
    if (myHTMLBuffer == NULL)
    {
    	if (pmShowError)
    	{
    	    EdGUI_Message1 (pmWindow, 0, IDS_HTMLREADFAIL_TITLE, 
    			    IDS_HTMLREADFAIL_MESSAGE, myHTMLName);
	}    			   
    	return FALSE;
    }
    
    myPtr = myHTMLBuffer;
    while (TRUE)
    {
    	// Search for an angle bracket
    	myPtr = strchr (myPtr, '<');
    	if (myPtr == NULL)
    	{
    	    if (pmShowError)
    	    {
    	    	EdGUI_Message1 (pmWindow, 0, IDS_HTMLNOAPPLET_TITLE, 
    			        IDS_HTMLNOAPPLET_MESSAGE, myHTMLName);
	    }
	    free (myHTMLBuffer);    			        
    	    return FALSE;
    	}
    	myPtr++;
    	
	if (!MyReadWordFromString (&myPtr, myWord))
	{
	    continue;
	}
	
	// See if the tag is applet or object
	if (stricmp (myWord, "applet") != 0)
	{
	    continue;
	}
	
	// We've read "<applet".  Now look for CODE=, WIDTH= and HEIGHT=
	myWidth = -1;
	myHeight = -1;
	
	while (TRUE)
	{
	    // Read attribute name
	    if (!MyReadWordFromString (&myPtr, myWord))
	    {
	    	myMatch = FALSE;
	    	break;
	    }
	    if (strcmp (myWord, ">") == 0)
	    {
	    	if ((myWidth == -1) || (myHeight == -1))
	    	{
	    	    myMatch = FALSE;
	    	}
	    	break;
	    }
	    if (strcmp (myWord, "=") == 0)
	    {
	        if (!MyReadWordFromString (&myPtr, myWord))
	        {
	            myMatch = FALSE;
	            break;
	        }
	        continue;
	    }
	    if (stricmp (myWord, "code") == 0)
	    {
	    	if (!MyReadWordFromString (&myPtr, myWord))
	    	{
	    	    myMatch = FALSE;
	    	    break;
	    	}
	    	if (strcmp (myWord, "=") != 0)
	    	{
	    	    break;
	    	}
	    	// Read the code
	    	if (!MyReadWordFromString (&myPtr, myWord))
	    	{
	    	    myMatch = FALSE;
	    	    break;
	    	}
	    	
	    	// Don't worry about the path.  It should end with 
	    	// "classname.class"
	    	if ((strlen (myWord) >= strlen (myClassName)) &&
	    	    (strcmp (&myWord [strlen (myWord) - strlen (myClassName)], 
	    	     myClassName) != 0))
	    	{
	    	    myMatch = FALSE;
	    	    break;
	    	}
	    	myMatch = TRUE;
	    }
	    if (stricmp (myWord, "width") == 0)
	    {
	    	if (!MyReadWordFromString (&myPtr, myWord))
	    	{
	    	    myMatch = FALSE;
	    	    break;
	    	}
	    		
	    	if (strcmp (myWord, "=") != 0)
	    	{
	    	    myMatch = FALSE;
	    	    break;
	    	}
	    	// Read the width
	    	if (!MyReadWordFromString (&myPtr, myWord))
	    	{
	    	    myMatch = FALSE;
	    	    break;
	    	}
	    	if (!EdGlob_ConvertStringToNum (myWord, &myWidth))
	    	{
	    	    myMatch = FALSE;
	    	    break;
	    	}
	    }
	    if (stricmp (myWord, "height") == 0)
	    {
	    	if (!MyReadWordFromString (&myPtr, myWord))
	    	{
	    	    myMatch = FALSE;
	    	    break;
	    	}
	    	if (strcmp (myWord, "=") != 0)
	    	{
	    	    myMatch = FALSE;
	    	    break;
	    	}
	    	// Read the height
	    	if (!MyReadWordFromString (&myPtr, myWord))
	    	{
	    	    myMatch = FALSE;
	    	    break;
	    	}
	    	if (!EdGlob_ConvertStringToNum (myWord, &myHeight))
	    	{
	    	    myMatch = FALSE;
	    	    break;
	    	}
	    }
	} // while (reading applet internals)
	
	if (myMatch)
	{
	    *pmAppletWidth = myWidth;
	    *pmAppletHeight = myHeight;
	    pmAppletParams [0] = 0;
	    
	    // Let's read the parameters, if any
	    while (TRUE)
	    {
	    	myPtr = strchr (myPtr, '<');
	    	if (myPtr == NULL)
	    	{
	    	    break;
		}
		myPtr++;
		if (!MyReadWordFromString (&myPtr, myWord))
		{
		    break;
		}
		if (stricmp (myWord, "/applet") == 0)
		{
		    break;
		}
		else if (stricmp (myWord, "param") == 0)
		{
		    char	myName [1024], myValue [1024];
		    
		    myName [0] = 0;
		    myValue [0] = 0;
		    
		    while (TRUE)
		    {
		        // Read attribute name
	    	        if (!MyReadWordFromString (&myPtr, myWord))
	    	        {
	    	            break;
	    	        }
		        if (strcmp (myWord, ">") == 0)
	    		{
		    	    break;
		        }
		        if (strcmp (myWord, "=") == 0)
		        {
		            if (!MyReadWordFromString (&myPtr, myWord))
		            {
		            	break;
		            }
		            continue;
		        }
		        if (stricmp (myWord, "name") == 0)
		        {
		    	    if (!MyReadWordFromString (&myPtr, myWord))
		    	    {
		    	    	break;
		    	    }
		    	    if (strcmp (myWord, "=") != 0)
		    	    {
		    	        break;
		    	    }
		    	    // Read the name
		    	    if (!MyReadWordFromString (&myPtr, myName))
		    	    {
		    	    	break;
		    	    }
		        }
		        if (stricmp (myWord, "value") == 0)
		        {
		    	    if (!MyReadWordFromString (&myPtr, myWord))
		    	    {
		    	    	break;
		    	    }
		    	    if (strcmp (myWord, "=") != 0)
		    	    {
		    	        break;
		    	    }
		    	    // Read the value
		    	    if (!MyReadWordFromString (&myPtr, myValue))
		    	    {
		    	    	break;
		    	    }
		        }
		    } // while (reading param internals)
		    if ((myName [0] != 0) && (myValue [0] != 0))
		    {
		    	strcat (pmAppletParams, myName);
		    	strcat (pmAppletParams, "=");
		    	strcat (pmAppletParams, myValue);
		    	strcat (pmAppletParams, "\r\n");
		    }
		} // if we found the <param> tag
	    } // while (TRUE)
	    free (myHTMLBuffer);    			        
	    return TRUE;
	} // if we read the <applet> tag
    } // while
} // MyGetDataFromHTMLFile


/************************************************************************/
/* MyGetIndentSize							*/
/************************************************************************/
static int	MyGetIndentSize (char *pmPtr, char *pmStartPtr)
{
    char    *myPtr = pmPtr - 1, *myStartOfBlank;
    int	    myCol = 0, myStartOfBlankCol;

    while ((myPtr >= pmStartPtr) && ((*myPtr == ' ') || (*myPtr == '\t')))
    {
    	myPtr--;
    }
    myStartOfBlank = myPtr + 1;
    while ((myPtr >= pmStartPtr) && ((*myPtr != '\r') && (*myPtr != '\n')))
    {
    	myPtr--;
    }
    myPtr++;

    // First determine the column of the myStartOfBlank position
    while (myPtr < myStartOfBlank)
    {
	if (*myPtr != '\t')
	    myCol++;
	else
	    myCol += (gProperties.editNumSpacesPerTab - 
	        myCol % gProperties.editNumSpacesPerTab);
	myPtr++;
    }
    myStartOfBlankCol = myCol;
    while (myPtr < pmPtr)
    {
	if (*myPtr != '\t')
	    myCol++;
	else
	    myCol += (gProperties.editNumSpacesPerTab - 
	        myCol % gProperties.editNumSpacesPerTab);
	myPtr++;
    }

    return myCol - myStartOfBlankCol;
} // MyGetIndentSize


/************************************************************************/
/* MyGetJavaLibPath							*/
/************************************************************************/
static void	MyGetJavaLibPath (char *pmJavaLibPath)
{
    // This is temporary - Does not handle user installed JRE/JDK's
    if (gProperties.JVMType == JRE_131)
    {
        EdFile_ConstructPath (pmJavaLibPath, IDS_JAVA_LIB_131, IDS_JAVA_RT_JAR);
    }
    else if (gProperties.JVMType == JRE_BUILT_IN)
    {
        EdFile_ConstructPath (pmJavaLibPath, IDS_JAVA_LIB_BUILT_IN, 
        		      IDS_JAVA_RT_JAR);
    }
    else
    {
    	EdFail_Fatal (IDS_BADJVMTYPE, __FILE__, __LINE__, 0,
    		      gProperties.JVMType);
    }
} // MyGetJavaLibPath


/************************************************************************/
/* MyGetJavaSupSignals_T						*/
/************************************************************************/
static int	MyGetJavaSupSignals_T (LPVOID pmThreadData)
{
    HANDLE	myHandles [10];
    int		myResult;
    HWND	myConsoleWindow, myEditorWindow;
    char	myClassName [1024];
    	
    myConsoleWindow = ((ThreadData *) pmThreadData) -> consoleWindow;
    myEditorWindow = EdGUI_GetTopWindow (myConsoleWindow);
    strcpy (myClassName, ((ThreadData *) pmThreadData) -> className);
    free (((ThreadData *) pmThreadData) -> className);
    free (pmThreadData);
    
    myHandles [0] = stNotifyReadyProgramPaused;
    myHandles [1] = stNotifyReadyProgramResumed;
    myHandles [2] = stNotifyReadyProgramFinished;
    myHandles [3] = stNotifyReadyProgramHalted;
    myHandles [4] = stNotifyReadyReadyMustTerminate;
    myHandles [5] = stNotifyReadySendingOutput;
    myHandles [6] = stNotifyReadySendingPrintout;
    myHandles [7] = stNotifyReadySendingException;
    myHandles [8] = stNotifyReadySendingFailMessage;
    myHandles [9] = stJavaRunnerProcessHandle;
    
    while (TRUE)
    {
    	myResult = WaitForMultipleObjects (10, myHandles, FALSE, INFINITE);

	if (myResult == (WAIT_OBJECT_0 + 0))
    	{
	    // Program paused
	    ResetEvent (stNotifyReadyProgramPaused);
	    SendNotifyMessage (Ed_GetLastActiveEditWindow (), WM_COMMAND, 
		(WPARAM) COMMAND_PAUSERESUME, (LPARAM) NULL);
	}
    	else if (myResult == (WAIT_OBJECT_0 + 1))
    	{
	    // Program resumed
	    ResetEvent (stNotifyReadyProgramResumed);
	    SendNotifyMessage (Ed_GetLastActiveEditWindow (), WM_COMMAND, 
		(WPARAM) COMMAND_PAUSERESUME, (LPARAM) NULL);
	}
    	else if (myResult == (WAIT_OBJECT_0 + 2))
    	{
    	    // Program finished
	    ResetEvent (stNotifyReadyProgramFinished);
	    if (stJavaProgramRunning)
	    {
	    	stJavaProgramRunning = FALSE;
	    	stJavaProgramPaused = FALSE;
	    	stJavaProgramRunningIsApplet = FALSE;
		EdJdb_DetachFromVM ();
	    	if (!stRestartingJavaProgramRunner)
	    	{
	    	    Ed_SendMessageToAllWindows (TRUE, WM_SET_BUTTONS, 
	    				        SET_BUTTON_NOT_RUNNING, 0);
	            Ed_SendStatusMessageToAllWindows ("Execution Finished");
		}	    	        
	    }
	}
    	else if (myResult == (WAIT_OBJECT_0 + 3))
    	{
    	    // Program halted
	    ResetEvent (stNotifyReadyProgramHalted);
	    if (stJavaProgramRunning)
	    {
	    	stJavaProgramRunning = FALSE;
	    	stJavaProgramPaused = FALSE;
	    	stJavaProgramRunningIsApplet = FALSE;
		EdJdb_DetachFromVM ();
	    	if (!stRestartingJavaProgramRunner)
	    	{
	    	    Ed_SendMessageToAllWindows (TRUE, WM_SET_BUTTONS, 
	    				        SET_BUTTON_NOT_RUNNING, 0);
	            Ed_SendStatusMessageToAllWindows ("Execution Halted");
		}	    	        
	    }
	}
    	else if (myResult == (WAIT_OBJECT_0 + 4))
    	{
    	    // Quit Ready requested
	    ResetEvent (stNotifyReadyReadyMustTerminate);
	    stJavaProgramRunning = FALSE;
	    stJavaProgramPaused = FALSE;
	    stJavaProgramRunningIsApplet = FALSE;
	    EdJdb_DetachFromVM ();
	    if (IsWindow (myEditorWindow) && !stRestartingJavaProgramRunner)
	    {
	    	SendNotifyMessage (myEditorWindow, WM_COMMAND, 
	    	    (WPARAM) COMMAND_EXIT, (LPARAM) NULL);
	    }
	}
    	else if (myResult == (WAIT_OBJECT_0 + 5))
    	{
#ifdef JUNK    	    
    	    // Here is the output of the program
	    ResetEvent (stNotifyReadySendingOutput);

    	    printf ("Output of program\n");
    	    printf ("-----------------\n");

	    do
	    {
	    	if (!ReadFile (stFromJavaSupReadDup, myBuffer, 255, 
	    		       &myBytesRead, NULL))
 	    	{
 	            printf ("Error reading program input\n");
	            ExitProcess (0);
 	    	}
 	    	
 	    	printf ("%s", myBuffer);
 	    } while (myBuffer [myBytesRead - 1] != 0);
#endif  	    
	}
    	else if (myResult == (WAIT_OBJECT_0 + 6))
	{
    	    // Please print this output
	    int			myTextLen;
	    int			mySubmitterWindow;
	    int			myBytesRead;
	    HWND		myCurrentEditorWindow;
	    OutputPrintJob	*myJob;
	    	    	
	    ResetEvent (stNotifyReadySendingPrintout);
	    
	    myJob = (OutputPrintJob *) malloc (sizeof (OutputPrintJob));
	    myJob -> className = EdGlob_MallocString (myClassName);
	    
	    if (!ReadFile (stFromJavaSupReadDup, &mySubmitterWindow, 4, 
	    		   &myBytesRead, NULL) || (myBytesRead != 4))
 	    {
 	    	// TW Error Message and abort
 	    	continue;
 	    }
	    // If we're logging, then output the line to the log file
	    if (gProperties.logging)
	    {
	    	EdLog_Log ("[10] Rcvd from Java Supervisor: %d", mySubmitterWindow);
	    }    
 	    
	    if (!ReadFile (stFromJavaSupReadDup, &myTextLen, 4, 
	    		   &myBytesRead, NULL) || (myBytesRead != 4))
 	    {
 	    	// TW Error Message and abort
 	    	continue;
 	    }
	    // If we're logging, then output the line to the log file
	    if (gProperties.logging)
	    {
	    	EdLog_Log ("[11] Rcvd from Java Supervisor: %d", myTextLen);
	    }    
 	    
 	    myJob -> programOutput = malloc (myTextLen + 1);
	    if (!ReadFile (stFromJavaSupReadDup, myJob -> programOutput, 
	    		   myTextLen, &myBytesRead, NULL) || 
	        (myBytesRead != myTextLen))
 	    {
 	    	// TW Error Message and abort
 	    	continue;
 	    }
 	    myJob -> programOutput [myTextLen] = 0;
	    // If we're logging, then output the line to the log file
	    if (gProperties.logging)
	    {
	    	EdLog_Log ("[12] Rcvd from Java Supervisor: \"%s\"", myJob -> programOutput);
	    }    
 	    myJob -> outputLen = myTextLen;
	    myJob -> submitterWindow = (HWND) mySubmitterWindow;
	    
	    myCurrentEditorWindow = Ed_GetLastActiveEditWindow ();	    		   
	    SendNotifyMessage (myCurrentEditorWindow, WM_PRINTOUTPUTJOB, 
	    	    (WPARAM) myJob, (LPARAM) NULL);
	}
    	else if (myResult == (WAIT_OBJECT_0 + 7))
	{
	    // Sending an exception
	    int		myTextLen;
	    int		myBytesRead;
	    char	*myExceptionText;

	    ResetEvent (stNotifyReadySendingException);
	    if (!ReadFile (stFromJavaSupReadDup, &myTextLen, 4, 
	    		   &myBytesRead, NULL) || (myBytesRead != 4))
 	    {
 	    	// TW Error Message and abort
 	    	continue;
 	    }
	    // If we're logging, then output the line to the log file
	    if (gProperties.logging)
	    {
	    	EdLog_Log ("[13] Rcvd from Java Supervisor: %d", myTextLen);
	    }    
 	    
 	    myExceptionText = malloc (myTextLen + 1);
	    if (!ReadFile (stFromJavaSupReadDup, myExceptionText, 
	    		   myTextLen, &myBytesRead, NULL) || 
	        (myBytesRead != myTextLen))
 	    {
 	    	// TW Error Message and abort
 	    	continue;
 	    }
 	    myExceptionText [myTextLen] = 0;
	    if (gProperties.logging)
	    {
	    	EdLog_Log ("[14] Rcvd from Java Supervisor: \"%s\"", myExceptionText);
	    }    
 	    
 	    MyInterpretException_T (myExceptionText);
 	    
 	    free (myExceptionText);
	}
    	else if (myResult == (WAIT_OBJECT_0 + 8))
	{
	    // Send to the EdFail_AddMessage
	    int		myTextLen;
	    int		myBytesRead;
	    char	*myMessageText;

	    ResetEvent (stNotifyReadySendingFailMessage);
	    
	    // Read the message length
	    if (!ReadFile (stFromJavaSupReadDup, &myTextLen, 4, 
	    		   &myBytesRead, NULL) || (myBytesRead != 4))
 	    {
 	    	// TW Error Message and abort
 	    	continue;
 	    }
 	    // Read the message
 	    myMessageText = malloc (myTextLen + 1);
	    if (!ReadFile (stFromJavaSupReadDup, myMessageText, 
	    		   myTextLen, &myBytesRead, NULL) || 
	        (myBytesRead != myTextLen))
 	    {
 	    	// TW Error Message and abort
 	    	continue;
 	    }
 	    myMessageText [myTextLen] = 0;
	    EdFail_AddMessage (NORMAL, myMessageText);
	}
    	else if (myResult == (WAIT_OBJECT_0 + 9))
	{
	    // The Java Program Runner is dead
	    if (stJavaProgramRunning)
	    {
	    	stJavaProgramRunning = FALSE;
	    	stJavaProgramPaused = FALSE;
	    	stJavaProgramRunningIsApplet = FALSE;
		EdJdb_DetachFromVM ();
	    	if (!stRestartingJavaProgramRunner)
	    	{
	    	    Ed_SendMessageToAllWindows (TRUE, WM_SET_BUTTONS, 
	    				        SET_BUTTON_NOT_RUNNING, 0);
	            Ed_SendStatusMessageToAllWindows ("Execution Finished");
		}	    	        
	    }
	    break;
	}
    	else if (myResult == WAIT_FAILED)
    	{
    	    break;
    	}
    } // while (TRUE)
    
    EnterCriticalSection (&stJavaProcessCS);
    stJavaRunnerProcessHandle = NULL;
    LeaveCriticalSection (&stJavaProcessCS);
    SetEvent (stJavaSignalHandlerKnowsProcessDead);
    
    return 0;
} // MyGetJavaSupSignals_T


/************************************************************************/
/* MyInterpretException_T						*/
/*									*/
/* This takes all the output that was sent to stderr and tries to	*/
/* convert it into a run-time error.  The expected format is:		*/
/* 		Error message						*/
/*			at [Java class].[method] ([File]:[Line Number])	*/
/*			at ..						*/
/*			at ..						*/
/************************************************************************/
static void	MyInterpretException_T (char *pmExceptionText)
{
    char	*myPtr, *myReadPtr, *myLine, *myNextLine, *myErrorLocation;
    char	*myErrorMessage;
    StringLink	myLocationListHead;
    StringLink	myReverseLocationListHead, myCurrentReverseLocationListPtr;
    char	*myFileNamePtr, *myLineNumberPtr;
    StringLink	myLink, myPrevLink, myFileDependencyLink, myLocationLink;
    BOOL	myErrorLocationFound;
    char	myErrorBuffer [8192];  
    int		myDepth, cnt;
      
    myReadPtr = pmExceptionText;
    myLine = MyReadLine (&myReadPtr);
    while (myLine != NULL)
    {
    	myErrorLocation = myReadPtr;
    	myNextLine = MyReadLine (&myReadPtr);
    	if ((myNextLine != NULL) && (strncmp (myNextLine, "\tat ", 4) == 0))
    	{
    	    break;
    	}
    	myLine = myNextLine;
    	myNextLine = NULL;
    }
    
    // There was no message with format:
    if (myNextLine == NULL)
    {
    	return;
    }

    //
    // Process the error message.  Assume the following format:
    // 		Error message
    //		<TAB>at [Java class].[method] ([File]:[Line Number])
    //		..
    //  
    myErrorMessage = myLine;
        
    myLocationListHead = NULL;
    myReverseLocationListHead = NULL;
    myCurrentReverseLocationListPtr = NULL;
    
    // Process all the locations
    myDepth = 0;
    while ((myNextLine != NULL) && (myDepth < 30))
    {
    	StringLink	myNewErrorLine;
    	
    	if (strncmp (myNextLine, "\tat ", 4) == 0)
    	{
    	    // Add it to the forward list
    	    myNewErrorLine = EdGlob_CreateStringLink (&myNextLine [4]);
    	    myPtr = strchr (myNewErrorLine -> string, '(');
    	    if (myPtr != NULL)
    	    {
    	    	*myPtr = 0;
    	    	myPtr++;
    	    	myPtr = strchr (myPtr, ':');
    	    	if (myPtr != NULL)
    	    	{
    	    	    *myPtr = 0;
    	    	    myPtr++;
    	    	    myPtr = strchr (myPtr, ')');
    	    	    if (myPtr != NULL)
    	    	    {
    	    	    	*myPtr = 0;
    	    	    }
		    else
		    {
			// TW Remove from list
		    }
    	    	}
		else
		{
		    // TW Remove from list
		}
    	    }
	    else
	    {
		// TW Remove from list
	    }
    	    
    	    myNewErrorLine -> next = myLocationListHead;
    	    myLocationListHead = myNewErrorLine;
    	    
    	    // Add it to the backward list
    	    myNewErrorLine = EdGlob_CreateStringLink (&myNextLine [4]);
    	    if (myCurrentReverseLocationListPtr == NULL)
    	    {
    	    	myReverseLocationListHead = myNewErrorLine;
    	    }
    	    else
    	    {
    	    	myCurrentReverseLocationListPtr -> next = myNewErrorLine;
    	    }
    	    myCurrentReverseLocationListPtr = myNewErrorLine;
    	}
    	myNextLine = MyReadLine (&myReadPtr);

	// We don't want to go 300 levels deep if there's infinite recursion.
	// This will stop at 30 levels.
	myDepth++;
    }
    
    // We now have a list of all the locations:
    //     myReverseLocationListHead lists the error location from the 
    //		lowest level (where error occurred) to the highest level (main)
    //	   myLocationListHead lists the error location from the highest
    //		level (main) to the lowest level (where error occurred).
    // now go through the list until
    // we reach a location of code that we compiled.
    myErrorLocationFound = FALSE;
    
    myLink = myReverseLocationListHead;
    myPrevLink = NULL;

    while (myLink != NULL)
    {
    	// The line looks like: hsa.Console.<init>(Console.java:135)
    	// When it's finished it looks like
    	//    hsa.Console.<init><NULL>Console.java<NULL>135<NULL>
    	myFileNamePtr = strchr (myLink -> string, '(');
    	if (myFileNamePtr == NULL)
    	{
	    myPrevLink = myLink;
	    myLink = myLink -> next;
    	    continue;
    	}
    	*myFileNamePtr = 0;
    	
    	// Point to the beginning of the file name
    	myFileNamePtr++;
    	myLineNumberPtr = strchr (myFileNamePtr, ':');
    	if (myLineNumberPtr == NULL)
    	{
	    myPrevLink = myLink;
	    myLink = myLink -> next;
    	    continue;
    	}
    	*myLineNumberPtr = 0;
    	// Point to the beginning of the line number
    	myLineNumberPtr++;
    	myPtr = strchr (myLineNumberPtr, ')');
    	if (myPtr == NULL)
    	{
	    myPrevLink = myLink;
	    myLink = myLink -> next;
    	    continue;
    	}
    	*myPtr = 0;
    	
    	myFileDependencyLink = stJavaFilesReadDuringCompilation;
    	while (myFileDependencyLink != NULL)
    	{
    	    if (strcmp (
    	    	EdFile_GetFileName (myFileDependencyLink -> string), 
    	    	myFileNamePtr) == 0)
    	    {
    	    	break;
    	    }
    	    myFileDependencyLink = myFileDependencyLink -> next;
    	} // while (myFileDependencyLink != NULL)
    	
    	// File found in the list of opened files.  Place the error message
    	if (myFileDependencyLink != NULL)
    	{
    	    // Add an error message
    	    
    	    // if this is the first error message, display the full error 
    	    // message
    	    if (!myErrorLocationFound)
    	    {
	    	if (strcmp (myErrorMessage, 
	    	    	    "java.lang.NullPointerException") == 0)
	    	{
	    	    myErrorMessage = 
	    	       "Attempt to use a null pointer (java.lang.NullPointerException)";
	    	}
	    	else if (strcmp (myErrorMessage, 
	    	    	    "java.lang.ArithmeticException: / by zero") == 0)
	    	{
	    	    myErrorMessage = 
	    	       "Division by zero (java.lang.ArithmeticException)";
	    	}

	   	strcpy (myErrorBuffer, myErrorMessage);
	   	
	   	myErrorLocationFound = TRUE;
    	    }
    	    else
    	    {
    	    	strcpy (myErrorBuffer, "Error occurred");
    	    }

    	    if (myPrevLink != NULL)
    	    {
    	    	// If there's a method before this, indicate the name of the
    	    	// method that the exception occurred in
    	    	char *myMethodName;
    	    	
    	    	if (strncmp (myPrevLink -> string, "java.", 5) == 0)
    	    	{
    	    	    myMethodName = strrchr (myPrevLink -> string, '.') + 1;
    	    	}
    	    	else
    	    	{
    	    	    myMethodName = myPrevLink -> string;
    	    	}
    	    	strcat (myErrorBuffer, " while calling ");
    	    	strcat (myErrorBuffer, myMethodName);
	    } // if (myPrevLink != NULL)
	    
	    // Now add the rest frame stack with a "-->" at the current location
	    // if there is more than one element in the stack.
	    myLocationLink = myLocationListHead;
	    myDepth = 0;
	    while (myLocationLink != NULL)
	    {
	    	char *myLocationFileName, *myLocationLineNumber;
	    	
	    	// If the stack is only one deep, don't bother adding the
	    	// stack trace.
	    	if ((myLocationLink == myLocationListHead) && 
	    	    (myLocationLink -> next == NULL))
	    	{
	    	    break;
	    	}
	    	
	    	// Determine if this is the current line
	    	myLocationFileName = &myLocationLink -> string [
	    				strlen (myLocationLink -> string) + 1];
		myLocationLineNumber = &myLocationFileName [
	    				strlen (myLocationFileName) + 1];
		if ((strcmp (myFileNamePtr, myLocationFileName) == 0) &&
		    (strcmp (myLineNumberPtr, myLocationLineNumber) == 0))
		{
	            strcat (myErrorBuffer, "\r\n-->");
	        }
	        else
		{
	            strcat (myErrorBuffer, "\r\n    ");
	        }
	        
	    	for (cnt = 0 ; cnt < myDepth ; cnt++)
	    	{
	    	    strcat (myErrorBuffer, "  ");
	    	}
	    	myDepth++;
	    	strcat (myErrorBuffer, myLocationLink -> string);
		myLocationLink = myLocationLink -> next;
	    }

	    EdErr_AddError (myFileDependencyLink -> string, 0, 
	    		    atoi (myLineNumberPtr), 0, atoi (myLineNumberPtr), 
	    		    0, myErrorBuffer);
    	} // if (myFileDependencyLink != NULL)

	myPrevLink = myLink;
	myLink = myLink -> next;
    } // while (myLink != NULL)
    
    SendNotifyMessage (Ed_GetLastActiveEditWindow (), WM_COMMAND, 
    		       (WPARAM) COMMAND_FIND_NEXT_ERROR, (LPARAM) NULL);
    		       
    // Free the two lists of error locations
    EdGlob_FreeStringLinkList (&myLocationListHead);
    EdGlob_FreeStringLinkList (&myReverseLocationListHead);
} // MyInterpretException_T


/************************************************************************/
/* MyGetRunArgs								*/
/************************************************************************/
static BOOL	MyGetRunArgs (HWND pmWindow, const char *pmPathName, 
			      int pmProgramKind, int *pmAppletWidth,
			      int *pmAppletHeight, char *pmAppletParams)
{
    HINSTANCE	myApplicationInstance;
    BOOL	myResult;

    // Get the program instance
    myApplicationInstance = 
        (HINSTANCE) GetWindowLong (pmWindow, GWL_HINSTANCE);
    
    if ((pmProgramKind == APPLET) && (*pmAppletWidth != -1))
    {
	stRunArgs.appletWidth = *pmAppletWidth;
	stRunArgs.appletHeight = *pmAppletHeight;
	strcpy (stRunArgs.appletParameterPairs, pmAppletParams);
    }
    else
    {
    	stRunArgs.appletWidth = gProperties.runAppletWidth;
    	stRunArgs.appletHeight = gProperties.runAppletHeight;
    	stRunArgs.appletParameterPairs [0] = 0;
    }

    Ed_EnableAllWindows (FALSE, pmWindow);
    myResult = DialogBoxParam (myApplicationInstance, 
            MAKEINTRESOURCE (JAVA_RUN_WITH_ARGS_DIALOG),
	    pmWindow, MyRunWithArgsDialogProcedure, (LPARAM) &stRunArgs);
    Ed_EnableAllWindows (TRUE, pmWindow);

    if (!myResult)
    {
	return FALSE;
    }
    
    if (pmProgramKind == APPLET)
    {
    	*pmAppletWidth = stRunArgs.appletWidth;
    	*pmAppletHeight = stRunArgs.appletHeight;
    	strcpy (pmAppletParams, stRunArgs.appletParameterPairs);
    }
    return TRUE;
} // MyGetRunArgs

    
/************************************************************************/
/* MyListJARFiles							*/
/************************************************************************/
static void	MyListJARFiles (void)
{
    FilePath		myPathName;
    HANDLE		myHandle;
    WIN32_FIND_DATA	myFindFileData;
    
    stJARFiles [0] = 0;
    
    // Now, add any ".jar" files that appear in the Ready Classes directory
    EdFile_ConstructPath (myPathName, IDS_READY_CLASSES_DIR, NO_FILE_NAME);
    strcat (myPathName, "*.jar");
    myHandle = FindFirstFile (myPathName, &myFindFileData);
    if (myHandle != (HANDLE) INVALID_HANDLE_VALUE)
    {
    	do
    	{
    	    // Add the file name to the class path
            EdFile_ConstructPath (myPathName, IDS_READY_CLASSES_DIR, 
        		      	  NO_FILE_NAME);
            strcat (myPathName, myFindFileData.cFileName);
    	    strcat (stJARFiles, myPathName);
    	    strcat (stJARFiles, ";");
    	} while (FindNextFile (myHandle, &myFindFileData));
    	FindClose (myHandle);
    }    	
    
    // Now, add any ".jar" files that appear in the Ready Classes directory
    EdFile_ConstructPath (myPathName, IDS_USER_CLASSES_DIR, NO_FILE_NAME);
    strcat (myPathName, "*.jar");
    myHandle = FindFirstFile (myPathName, &myFindFileData);
    if (myHandle != (HANDLE) INVALID_HANDLE_VALUE)
    {
    	do
    	{
    	    // Add the file name to the class path
            EdFile_ConstructPath (myPathName, IDS_USER_CLASSES_DIR, 
        		      	  NO_FILE_NAME);
            strcat (myPathName, myFindFileData.cFileName);
    	    strcat (stJARFiles, myPathName);
    	    strcat (stJARFiles, ";");
    	} while (FindNextFile (myHandle, &myFindFileData));
    	FindClose (myHandle);
    }    	
} // MyListJARFiles


/************************************************************************/
/* MyReadLine								*/
/************************************************************************/
static char	*MyReadLine (char **pmPtr)
{
    char	*myResult;
    
    if (**pmPtr == 0)
    {
	return NULL;
    }

    myResult = *pmPtr;
    
    while (**pmPtr != 0)
    {
    	if (**pmPtr == '\r')
    	{
	    **pmPtr = 0;
    	    *pmPtr = *pmPtr + 1;
	    if (**pmPtr == '\n')
	    {
    	        *pmPtr = *pmPtr + 1;
    	    }
    	    break;
	}
	else if (**pmPtr == '\r')
	{
	    **pmPtr = 0;
    	    *pmPtr = *pmPtr + 1;
    	    break;
	}
    	else
    	{
    	    *pmPtr = *pmPtr + 1;
    	}
    }	
    
    return myResult;
} // MyReadLine


/************************************************************************/
/* MyReadFromPipe							*/
/************************************************************************/
static BOOL	MyReadFromPipe (HANDLE pmReadPipeHandle, char **pmBuffer)
{
    int		myBytesToRead, myBytesRead;
    
    // Determine if there is anything to read in the pipe
    if (PeekNamedPipe (pmReadPipeHandle, NULL, 0, NULL, &myBytesToRead, NULL))
    {
    	if (myBytesToRead != 0)
    	{
    	    *pmBuffer = malloc (myBytesToRead + 2);
    	    if(*pmBuffer == NULL)
    	    {
    	    	// TW Not enough memory
    	    	return FALSE;
    	    }
    	    
    	    if (ReadFile (pmReadPipeHandle, *pmBuffer, myBytesToRead, 
    		      &myBytesRead, NULL))
	    {
	    	if (myBytesToRead != myBytesRead)
	    	{
	    	    EdFail_Warn (IDS_READFROMPIPESHORT, __FILE__, __LINE__,
	    	    	0, myBytesRead, myBytesToRead);
		    free (*pmBuffer);	    	    	
	    	    return FALSE;
	    	}

	    	// Add the end of line marker
	    	(*pmBuffer) [myBytesRead] = 0;
	    }
	    // If we're logging, then output the line to the log file
	    if (gProperties.logging)
	    {
	    	EdLog_Log ("[15] Rcvd from Java Supervisor: \"%s\"", *pmBuffer);
	    }    
	}
	else
	{
	    *pmBuffer = NULL;
	}
	return TRUE;
    }
    else
    {
    	EdFail_Warn (IDS_PEEKFROMPIPEFAIL, __FILE__, __LINE__, GetLastError ());
    	return FALSE;
    }
} // MyReadFromPipe


/************************************************************************/
/* MyReadWordFromString							*/
/************************************************************************/
static BOOL	MyReadWordFromString (char **pmPtr, char *pmWord)
{
    char	*myWordPtr;
    char	myChar;
        
    // Skip pver whitespace
    MySkipWhiteSpace (pmPtr);
    
    // If there's no more text, return NULL
    if (**pmPtr == 0)
    {
    	pmWord [0] = 0;
    	return FALSE;
    }

    if (**pmPtr == '=')
    {
    	strcpy (pmWord, "=");
    	(*pmPtr)++;    
    	return TRUE;
    }
    
    if (**pmPtr == '>')
    {
    	strcpy (pmWord, ">");
    	(*pmPtr)++;
    	return TRUE;
    }
    
    if (**pmPtr == '<')
    {
    	strcpy (pmWord, "<");
    	(*pmPtr)++;
    	return TRUE;
    }
    
    if (**pmPtr == '"')
    {
	myWordPtr = pmWord;
	(*pmPtr)++;
	while ((**pmPtr != '"') && (**pmPtr != '\r') && (**pmPtr != '\n'))
	{
	    *myWordPtr++ = *(*pmPtr)++;
	}
	(*pmPtr)++;
	*myWordPtr = 0;
	return TRUE;
    }

    myWordPtr = pmWord;
    myChar = **pmPtr;
    while ((myChar != ' ') && (myChar != '\t') && (myChar != '\n') &&
    	   (myChar != '\n')  && (myChar != '<') && (myChar != '>') &&
    	   (myChar != '"') && (myChar != '='))
    {
	*myWordPtr++ = *(*pmPtr)++;
	myChar = **pmPtr;
    }
    *myWordPtr = 0;
    
    return TRUE;
} // MyReadWordFromString


/************************************************************************/
/* MyRewriteHTMLFile							*/
/************************************************************************/
static BOOL	MyRewriteHTMLFile (HWND pmWindow, const char *pmHTMLPathName, 
				  const char *pmJavaFileName,
				  int pmAppletWidth, int pmAppletHeight,
				  char *pmAppletParams)
{
    char	myClassName [1024];
    char	*myHTMLBuffer;
    char	*myPtr, *myAppletPtr;
    char	myWord [1024];
    BOOL	myMatch = FALSE;
    int		myWidth, myHeight;
    char	*myWidthPtr, *myHeightPtr;
    int		myAppletIndent = 0, myParamIndent = -1;
    
    if (!EdFile_FileExists (pmHTMLPathName))
    {
	return FALSE;
    }

    // Create class name
    strcpy (myClassName, EdFile_GetFileName (pmJavaFileName));
    if (EdGlob_EndsWith (myClassName, JAVA_SUFFIX))
    {
        myClassName [strlen (myClassName) - 5] = 0;
    }
    strcat (myClassName, ".class");

    // Read file into buffer
    myHTMLBuffer = EdFile_ReadFileIntoBuffer (NULL, pmHTMLPathName, NULL, 
    				              OK_IF_NOT_FOUND);
    if (myHTMLBuffer == NULL)
    {
    	EdGUI_Message1 (pmWindow, 0, IDS_HTMLREADFAIL_TITLE, 
    			IDS_HTMLREADFAIL_MESSAGE, pmHTMLPathName);
    	return FALSE;
    }
    
    myPtr = myHTMLBuffer;
    while (TRUE)
    {
    	// Search for an angle bracket
    	myPtr = strchr (myPtr, '<');
    	if (myPtr == NULL)
    	{
    	    EdGUI_Message1 (pmWindow, 0, IDS_HTMLNOAPPLET_TITLE, 
    			    IDS_HTMLNOAPPLET_MESSAGE, pmHTMLPathName);
    	    return FALSE;
    	}
    	myAppletPtr = myPtr;
    	myPtr++;
    	
	if (!MyReadWordFromString (&myPtr, myWord))
	{
	    continue;
	}
	
	// See if the tag is applet or object
	if (stricmp (myWord, "applet") != 0)
	{
	    continue;
	}
	
	// We've read "<applet".  Get the identation
	myAppletIndent = MyGetIndentSize (myAppletPtr, myHTMLBuffer);
	
	// Now look for CODE=, WIDTH= and HEIGHT=
	myWidth = -1;
	myHeight = -1;
	
	while (TRUE)
	{
	    // Read attribute name
	    if (!MyReadWordFromString (&myPtr, myWord))
	    {
	    	myMatch = FALSE;
	    	break;
	    }
	    if (strcmp (myWord, ">") == 0)
	    {
	    	if ((myWidth == -1) || (myHeight == -1))
	    	{
	    	    myMatch = FALSE;
	    	}
	    	break;
	    }
	    if (strcmp (myWord, "=") == 0)
	    {
	        if (!MyReadWordFromString (&myPtr, myWord))
	    	{
	    	    myMatch = FALSE;
	    	    break;
	    	}
	        continue;
	    }
	    if (stricmp (myWord, "code") == 0)
	    {
	        if (!MyReadWordFromString (&myPtr, myWord))
	    	{
	    	    myMatch = FALSE;
	    	    break;
	    	}
	    	if (strcmp (myWord, "=") != 0)
	    	{
	    	    break;
	    	}
	    	// Read the code
	        if (!MyReadWordFromString (&myPtr, myWord))
	    	{
	    	    myMatch = FALSE;
	    	    break;
	    	}
	    	// Don't worry about the path.  It should end with 
	    	// "classname.class"
	    	if ((strlen (myWord) >= strlen (myClassName)) &&
	    	    (strcmp (&myWord [strlen (myWord) - strlen (myClassName)], 
	    	     myClassName) != 0))
	    	{
	    	    myMatch = FALSE;
	    	    break;
	    	}
	    	myMatch = TRUE;
	    }
	    if (stricmp (myWord, "width") == 0)
	    {
	        if (!MyReadWordFromString (&myPtr, myWord))
	    	{
	    	    myMatch = FALSE;
	    	    break;
	    	}
	    	if (strcmp (myWord, "=") != 0)
	    	{
	    	    myMatch = FALSE;
	    	    break;
	    	}
	    	// Read the width
	    	myWidthPtr = myPtr;
	        if (!MyReadWordFromString (&myPtr, myWord))
	    	{
	    	    myMatch = FALSE;
	    	    break;
	    	}
	    	if (!EdGlob_ConvertStringToNum (myWord, &myWidth))
	    	{
	    	    myMatch = FALSE;
	    	    break;
	    	}
	    }
	    if (stricmp (myWord, "height") == 0)
	    {
	        if (!MyReadWordFromString (&myPtr, myWord))
	    	{
	    	    myMatch = FALSE;
	    	    break;
	    	}
	    	if (strcmp (myWord, "=") != 0)
	    	{
	    	    myMatch = FALSE;
	    	    break;
	    	}
	    	// Read the height
	    	myHeightPtr = myPtr;
	        if (!MyReadWordFromString (&myPtr, myWord))
	    	{
	    	    myMatch = FALSE;
	    	    break;
	    	}
	    	if (!EdGlob_ConvertStringToNum (myWord, &myHeight))
	    	{
	    	    myMatch = FALSE;
	    	    break;
	    	}
	    }
	} // while (reading applet internals)
	
	if (myMatch)
	{
	    FileEntryPtr	myHTMLFile;
	    int			myErrorCode;
	    char		*myStart, *myParamStart, *myTagPtr, myChar = 0;
    	    char		myNumber [64];
	    int			cnt;
	    char		myIndent [256];

	    myHTMLFile = EdFile_OpenFileForWrite (pmHTMLPathName, &myErrorCode,
	    					  FALSE, FALSE);
	    if (myHTMLFile == NULL)
	    {
	    	// Write to log
	    	EdGUI_Message ("HTML Write failure", 
    		    "Unable to open HTML file (\"%s\") for writing.\n"
    			"Error = %d", pmHTMLPathName, myErrorCode);
	    	return FALSE;
	    }

	    //
	    // Write the file, substituting the new width and height
	    //    
	    myStart = myHTMLBuffer;
	    if (myWidthPtr < myHeightPtr)
	    {
	    	myChar = *myWidthPtr;
	    	*myWidthPtr = 0;
	    	EdFile_Write (myHTMLFile, myStart);
	    	*myWidthPtr = myChar;
	    	MyReadWordFromString (&myWidthPtr, myWord);
	    	myStart = myWidthPtr;
    	        wsprintf (myNumber, "\"%d\"", pmAppletWidth);
	        EdFile_Write (myHTMLFile, myNumber);
	    }
	    
	    // Rewrite the height
	    myChar = *myHeightPtr;
	    *myHeightPtr = 0;
	    EdFile_Write (myHTMLFile, myStart);
	    *myHeightPtr = myChar;
	    MyReadWordFromString (&myHeightPtr, myWord);
	    myStart = myHeightPtr;
    	    wsprintf (myNumber, "\"%d\"", pmAppletHeight);
	    EdFile_Write (myHTMLFile, myNumber);

	    // Rewrite the width (if after the height)
	    if (myWidthPtr > myHeightPtr)
	    {
	    	myChar = *myWidthPtr;
	    	*myWidthPtr = 0;
	    	EdFile_Write (myHTMLFile, myStart);
	    	*myWidthPtr = myChar;
	    	MyReadWordFromString (&myWidthPtr, myWord);
	    	myStart = myWidthPtr;
    	        wsprintf (myNumber, "\"%d\"", pmAppletWidth);
	        EdFile_Write (myHTMLFile, myNumber);
	    }
	    
	    // Write the HTML to the end of the applet tag
	    myChar = *myPtr;
	    *myPtr = 0;
	    EdFile_Write (myHTMLFile, myStart);
	    *myPtr = myChar;
	    myStart = myPtr;
	    
	    // 
	    // Write the new parameters
	    //
	    
	    // First, let's read the contents of the applet, skipping over
	    // the <param> tags.  We do, however, use the first <param>
	    // tag to determine what the indentation should be.
	    while (TRUE)
	    {
	    	myPtr = strchr (myPtr, '<');
	    	if (myPtr == NULL)
	    	{
	    	    break;
		}
		myTagPtr = myPtr;
	    	myPtr++;
		if (!MyReadWordFromString (&myPtr, myWord))
		{
		    break;
		}
		if (stricmp (myWord, "/applet") == 0)
		{
		    break;
		}
		else if (stricmp (myWord, "param") == 0)
		{
		    //
		    // If this is the first param statement, get the indent
		    //
		    if (myParamIndent == -1)
		    {
			myParamIndent = MyGetIndentSize (myTagPtr, 
							 myHTMLBuffer);
		    }
		    
		    do
		    {
		    	MyReadWordFromString (&myPtr, myWord);
		    } while ((strcmp (myWord, ">") != 0) && (myWord [0] != 0));
		    myStart = myPtr;
		}
		else // Rewrite any other item
		{
		    do
		    {
		    	MyReadWordFromString (&myPtr, myWord);
		    } while ((strcmp (myWord, ">") != 0) && (myWord [0] != 0));
		    
		    myChar = *myPtr;
		    *myPtr = 0;
		    EdFile_Write (myHTMLFile, myStart);
		    *myPtr = myChar;
		    myStart = myPtr;
		}		    
	    } // while (TRUE)

	    // We should now be at the ">" if the </applet> tag or the EOF
	    // We have written any non-param tags and myTagPtr should 
	    // point to the beginning of the </applet> tag.

	    // Write to the beginning of the </applet> line (or non-whitespace
	    // in front of the applet line)
	    myPtr = myTagPtr - 1;
	    while ((*myPtr == ' ') || (*myPtr == '\t'))
	    {
		myPtr--;
	    }
	    myPtr++;
	    myChar = *myPtr;
	    *myPtr = 0;
	    EdFile_Write (myHTMLFile, myStart);
	    *myPtr = myChar;
	    myStart = myPtr;

	    // myStart is now at the beginning of the line containing </applet>
	    if ((*(myPtr - 1) != '\r') && (*(myPtr - 1) != '\n'))
	    {
		EdFile_WriteNL (myHTMLFile);
	    }

	    //
	    // Write the new parameters
	    //
	    if (myParamIndent == -1)
	    {
		myParamIndent = myAppletIndent + 4;
	    }
	    for (cnt = 0 ; cnt < myParamIndent ; cnt++)
	    {
	    	myIndent [cnt] = ' ';
	    }
	    myIndent [myParamIndent] = 0;
	    
	    // Write out all the params
	    while (TRUE)
	    {
	    	while ((*pmAppletParams == ' ') || (*pmAppletParams == '\t') ||
	    	       (*pmAppletParams == '\r') || (*pmAppletParams == '\n'))
		{
		    pmAppletParams++;
		}
		if (*pmAppletParams == 0) break;
		
		EdFile_Write (myHTMLFile, myIndent);
	    	EdFile_Write (myHTMLFile, "<param name=\"");
	    	myParamStart = pmAppletParams;
	    	while (*pmAppletParams != '=') pmAppletParams++;
	    	*pmAppletParams = 0;
	    	EdFile_Write (myHTMLFile, myParamStart);
	    	*pmAppletParams++ = '=';
	    	EdFile_Write (myHTMLFile, "\" value=\"");
	    	myParamStart = pmAppletParams;
	    	while ((*pmAppletParams != '\r') && (*pmAppletParams != '\n') &&
	    	       (*pmAppletParams != 0))
		{
		    pmAppletParams++;
		}
		myChar = *pmAppletParams;
		*pmAppletParams = 0;
	    	EdFile_Write (myHTMLFile, myParamStart);
	    	*pmAppletParams = myChar;
	    	EdFile_Writeln (myHTMLFile, "\">");
	    	if (myChar == 0) break;
	    } // while

	    //
	    // Write the rest of the HTML file
	
	    //
	    EdFile_Write (myHTMLFile, myStart);
	    EdFile_CloseFile (myHTMLFile);
	    
	    return TRUE;
	} // if we rewrote the correct applet tag
    } // while	    
    
    return FALSE;
} // MyRewriteHTMLFile


/************************************************************************/
/* MySkipWhiteSpace							*/
/************************************************************************/
static void	MySkipWhiteSpace (char **pmPtr)
{
    char	myChar = **pmPtr;
    
    while ((myChar == ' ') || (myChar == '\t') || (myChar == '\r') || 
    	   (myChar == '\n'))
    {
	(*pmPtr)++;
    	myChar = **pmPtr;
    }
} // MySkipWhiteSpace


/************************************************************************/
/* MyStatusExecuting							*/
/************************************************************************/
static void	MyStatusExecuting (const char *pmClassName, BOOL pmPreparing)
{
    char	myStatusMessage [1024];
    
    if (pmClassName != NULL)
    {
    	if (stClassBeingCompiled != NULL)
    	{
    	    free (stClassBeingCompiled);
    	}
    	
    	stClassBeingCompiled = malloc (strlen (pmClassName) + 1);
    	strcpy (stClassBeingCompiled, pmClassName);
    }
    else if (stClassBeingCompiled == NULL)
    {
    	stClassBeingCompiled = malloc (strlen ("Program") + 1);
    	strcpy (stClassBeingCompiled, "Program");
    }
    
    if (pmPreparing)
    {
    	wsprintf (myStatusMessage, "Preparing %s for execution", 
    		  stClassBeingCompiled);
    }
    else
    {
    	wsprintf (myStatusMessage, "Executing %s", stClassBeingCompiled);
    }
    EdWin_ShowStatus (stCompilingWindow, myStatusMessage);
} // MyStatusExecuting


