/************/
/* jsmain.c */
/************/

/*******************/
/* System includes */
/*******************/
#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>

/****************/
/* Self include */
/****************/
#include "jsmain.h"

/******************/
/* Other includes */
/******************/
#include "jscon.h"
#include "jsdll.h"
#include "jni.h"

#include "jsdialogs.h"
#include "jssignals.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
#define PATHNAME_LENGTH			1024
#define JSDLL_FILE_NAME			"jsdll.dll"
#define MAIN_METHOD_NAME		"main"
#define CONSTRUCTOR_METHOD_NAME		"<init>"
#define READY_SETUP_CLASS_NAME		"ready/ReadySetUp"
#define READY_SETUP_METHOD_NAME		"initialize"
#define READY_RUNAPPLET_METHOD_NAME	"runApplet"
#define PAUSE_JAVA_METHOD_NAME		"pauseJavaThreads"
#define RESUME_JAVA_METHOD_NAME		"resumeJavaThreads"
#define PRINTPROP_CLASS_NAME		"DoPrintProp"
#define PRINTPROP_DOIT_METHOD_NAME	"doIt"
#define HSA_CONSOLE_CLASS_NAME		"hsa/Console"
#define MAIN_RETURNED_METHOD_NAME	"mainReturned"

#define READ_LINE_FROM_KBD	WAIT_OBJECT_0
#define WRITE_LINE_TO_SCRN	(WAIT_OBJECT_0 + 1)

#define PAUSE_JAVA_PROGRAM	WAIT_OBJECT_0
#define RESUME_JAVA_PROGRAM	(WAIT_OBJECT_0 + 1)
#define TERMINATE_JAVA_SUP	(WAIT_OBJECT_0 + 2)
#define READY_TERMINATED	(WAIT_OBJECT_0 + 3)
#define SEND_EDFAIL_MESSAGE	(WAIT_OBJECT_0 + 9)

#define KEYBOARD_BUFFER_SIZE	1024

#define OPEN_FOR_READ		TRUE
#define OPEN_FOR_WRITE		FALSE

#define SCREEN_AND_FILE		1
#define SCREEN_ONLY		2
#define FILE_ONLY		3

// VM options
#define CLASSPATH_VM_OPTION		"-Djava.class.path="
#define LIBRARY_PATH_VM_OPTION		"-Djava.library.path="
#define ENABLE_ASSERTIONS_VM_OPTION	"-enableassertions"
#define DEBUG_VM_OPTION			"-Xdebug"
#define CONNECTION_OPTION_VM_OPTION	"-Xrunjdwp:transport=dt_shmem,address=readyshmem,server=y,suspend=n";
#define MAX_JVM_SIZE_VM_OPTION		"-Xmx%dm"
#define NO_JIT_12			"-Djava.compiler=NONE"

/********************/
/* Global variables */
/********************/

/*********/
/* Types */
/*********/
typedef jint (JNICALL *JNI_CreateJavaVMCall) (JavaVM **pvm, void **penv, void *args);
typedef jint (JNICALL *JNI_GetDefaultJavaVMInitArgsCall) (void *args);

/**********************/
/* External variables */
/**********************/
Globals	gProgram;

/********************/
/* Static constants */
/********************/

/********************/
/* Static variables */
/********************/
static char		*stCommandLineOptions;

static char		*stJVMDLLPath = NULL;
static char		*stClassPath = NULL;
static char		*stClassToRunName = NULL;

static BOOL		stInputFromFile = FALSE;
static BOOL		stEchoInput = FALSE;
static char		*stInputPathName = NULL;
static FILE		*stInputFile = NULL;
static BOOL		stOutputToFile = FALSE;
static BOOL		stScreenAndFile = FALSE;
static BOOL		stOutputFileIsPrinter = FALSE;
static char		*stOutputPathName = NULL;
static FILE		*stOutputFile = NULL;
// Is this an applet?
static BOOL		stIsApplet;
// Applet code base
static char		*stCodeBase = NULL;
// Applet document base
static char		*stDocumentBase = NULL;
// Applet dimensions
static int		stAppletWidth, stAppletHeight;
// Applet parameters
static char		*stAppletParams [200];
// The number of applet parameters
static int		stNumAppletParams = 0;
// The JNI Environment
static JNIEnv		*stJNIEnv;
// The Java VM
static JavaVM		*stJVM;
// The Console Window
static HWND		stConsoleWindow;
// The JavaSup application directory
static char		stApplicationDirectory [PATHNAME_LENGTH];
// Flag to indicate console visible
static BOOL		stConsoleVisible = FALSE;
// Flag to indicate the program has been paused
static BOOL		stJavaProgramPaused = FALSE;
// Flag to indicate the program has stopped execution
static BOOL		stJavaProgramFinished = FALSE;
// Flag to indicate the program was stopped by the user
static BOOL		stJavaProgramHalted = FALSE;
// Handles from and to the main program
static HANDLE		stFromMainPipe, stToMainPipe;
// Flag to indicate whether we're waiting for input
static BOOL		stWaitingForInput = FALSE;
// The properties need to run the standard input/output window
static JSCon_Properties	stProperties;
// Signal to indicate that the Ready Console Window is opened
static HANDLE		stConsoleWindowOpened;
// Signal indicating we've read an EOF, and indicator if we've echoed it
static BOOL		stEOFInStdin = FALSE;
static BOOL		stEOFEchoed = FALSE;
// String names for status
static char		*stWindowTitles [5] = 
	{"Unknown State", "Running", "Paused", "Waiting for Input", "Finished"};
// The command line arguments to the Java program
static char		*stCommandLineArgs [200];
// The number of command line arguments to the Java program
static int		stNumCmdLineArgs = 0;
// The process handle for the main Ready program
static HANDLE		stReadyProcess;
// The window handle for the main Ready window
static HWND		stReadyWindow;
// The buffer containing output to go to the printer
static char		*stPrinterOutput;
// The size of the buffer
static int		stPrinterOutputBufferSize;
// The length of the output going to the printer
static int		stPrinterOutputLen;
// The flag indicating we should apture the exception
static BOOL	    	stInterceptException = FALSE;
// The buffer containing text of the exception
static char		*stExceptionOutput;
// The size of the buffer
static int		stExceptionOutputBufferSize;
// The length of the exception text
static int		stExceptionOutputLen;
// The flag indicating if we're debugging the JVM for Ready
static BOOL		stDebuggingReady = FALSE;
// The flag to indicate we're logging 
static BOOL		stLogging = FALSE;
// The HANDLE for the log file
static HANDLE		stLogFileHandle = NULL;
// The flag indicating that we're too break into the debugger before executing
// the program.
static BOOL		stStartInDebugger = FALSE;
// The maximum size of the JVM in MB
static int		stMaxJVMSize = 0;
// Whether we're running JRE 1.3
static BOOL		stRunningJRE13 = FALSE;

/******************************/
/* Static callback procedures */
/******************************/

/*********************/
/* Static procedures */
/*********************/
static void		MyChangeWindowTitle (int pmStatus);
static void		MyDestroyJavaVM (void);
static void		*MyFindLibraryAddress (char *pmDLLPath, 
					       char *pmProcName);
static void		MyGetApplicationDirectory (void);
static void		MyGetJavaException (char *pmMessage);
static void		MyInvokeAppletRunner (void);
static void		MyInvokeMain (void);
static int		MyInvokePauseJava (LPVOID pmDummy);
static void		MyInvokePrintPropDoIt (void);
static void		MyInvokeReadySetup (void);
static int		MyInvokeResumeJava (LPVOID pmDummy);
static char		*MyMallocWordFromString (char **pmPtr);
static int		MyMessageBox (HWND pmWindow, int pmMessageBoxParams, 
				      int pmTitleStringResource, 
				      int pmMessageStringResource, ...);
static void		MyOpenReadyConsole (void);
static int		MyOpenReadyConsoleThread (LPVOID pmConsoleWindow);
static FILE		*MyOpenFile (char *pmPathName, BOOL pmOpenForRead);
static jint JNICALL	MyPrintVMMessages (FILE *pmFilePointer, 
					   const char *pmFormat, 
					   va_list pmArgs);
static void		MyReadCommandLine (char *pmCommandLine);
static char		*MyReadFromPipe (void);
static char		*MyReadWordFromString (char **pmPtr);
static void		MySendException (void);
static void		MyStartJavaVM (void);
static int		MyWaitForReadWrite (LPVOID pmConsoleWindow);
static int		MyWaitForSignalFromMain (LPVOID pmConsoleWindow);
static void		MyWriteLine (int pmToWhere, char *pmString);

/***********************/
/* External procedures */
/***********************/

/************************************************************************/
/* WinMain								*/
/*									*/
/* Called by the system when the program is first run.			*/
/************************************************************************/
int WINAPI	WinMain (HINSTANCE pmApplicationInstance, 
			 HINSTANCE pmPrevInstance,
    			 PSTR pmCmdLine, int pmCmdShow)
{
//    char	myPathBuffer [PATHNAME_LENGTH];
    char	myDLLPathName [PATHNAME_LENGTH];
    HANDLE	myWaitForReadWriteThread;
    HANDLE	myWaitForSignalFromMain;
    DWORD	myThreadID;
    // Event to notify main program of exection finished
    HANDLE	myNotifyMainExecutionFinishedEvent;

    gProgram.applicationInstance = pmApplicationInstance;
    strcpy (gProgram.environmentName, "\"Ready\"");

//    DebugBreak ();
    
    // 
    stCommandLineOptions = malloc (strlen (pmCmdLine) + 1);
    if (stCommandLineOptions == NULL)
    {
    	JS_ErrorMessage (IDS_OUT_OF_MEM_FOR_STRING, pmCmdLine);
    }
    strcpy (stCommandLineOptions, pmCmdLine);   
    
    // Read in the command line
    MyReadCommandLine (pmCmdLine);

    JS_SendFailMessage (stCommandLineOptions);
        
    // Start the Java supervisor log
    JS_WriteLog ("[100] From JavaSup: Logging started");
  
    {
    	char	myCurrentDirectory [4096];
    	
    	if (GetCurrentDirectory (4095, myCurrentDirectory) == 0)
    	{
	    JS_WriteLog ("[101] From JavaSup: No current directory. (%d)",
	    	GetLastError ());
    	}
    	else
    	{
	    JS_WriteLog ("[101] From JavaSup: Current directory = \"%s\"",
	        myCurrentDirectory);
    	}
    }    	
    
    // The following must be true
    if (stJVMDLLPath == NULL)
    {
    	JS_ErrorMessage (IDS_NO_JAVA_DLL_SUPPLIED);
    }
    if (stClassPath == NULL)
    {
    	JS_ErrorMessage (IDS_NO_CLASSPATH_SUPPLIED);
    }
    if (stClassToRunName == NULL)
    {
    	JS_ErrorMessage (IDS_NO_CLASSNAME_SUPPLIED);
    }

    // Break to debugger if option selected
    if (stStartInDebugger)
    {
    	DebugBreak ();
    }
    
    // Load the DLL
    MyGetApplicationDirectory ();
    strcpy (myDLLPathName, stApplicationDirectory);
    strcat (myDLLPathName, JSDLL_FILE_NAME);
    if (!LoadLibrary (myDLLPathName))
    {
	int	myErrorCode = GetLastError ();
	char	*myErrorMessage;
	
	FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM | 
	    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
    	    NULL, myErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
    	    (LPTSTR) &myErrorMessage, 0, NULL);
    	    
    	JS_ErrorMessage (IDS_CANT_LOAD_LIB, myDLLPathName, myErrorMessage, 
    	    myErrorCode);
	LocalFree (myErrorMessage);
    }

    // Notify the DLL about logging
    DLL_SetLogging (stLogging, stLogFileHandle);
    
    // More logging messages
    JS_WriteLog ("[102] From JavaSup: DLL Loaded");

    // Create the thread that checks for read/writes
    myWaitForReadWriteThread = CreateThread (NULL, 0, 
        (LPTHREAD_START_ROUTINE) MyWaitForReadWrite, NULL, 0, &myThreadID);
    if (myWaitForReadWriteThread == NULL)
    {
	JS_ErrorMessage (IDS_UNABLE_TO_CREATE_THREAD);
    }    		       
    CloseHandle (myWaitForReadWriteThread);
    
    // More logging messages
    JS_WriteLog ("[103] From JavaSup: Read/Write thread created");
    
    // Create the thread that waits for halt, terminate signals
    myWaitForSignalFromMain  = CreateThread (NULL, 0, 
        (LPTHREAD_START_ROUTINE) MyWaitForSignalFromMain, NULL, 0, &myThreadID);
    if (myWaitForSignalFromMain == NULL)
    {
	JS_ErrorMessage (IDS_UNABLE_TO_CREATE_THREAD);
    }    		       
    CloseHandle (myWaitForSignalFromMain);

    // More logging messages
    JS_WriteLog ("[104] From JavaSup: Signal thread created");
    
    // Start the Java program, invoke the setup and then invoke the user prog
    MyStartJavaVM ();
  
    // More logging messages
    JS_WriteLog ("[108] From JavaSup: VM Started");
    
    MyInvokeReadySetup ();
    
    // More logging messages
    JS_WriteLog ("[109] From JavaSup: Ready Setup Invoked");
    
    if (stIsApplet)
    {
    	// More logging messages
    	JS_WriteLog ("[110] From JavaSup: About to invoke applet runner");
    
    	MyInvokeAppletRunner ();
    }
    else
    {
    	// More logging messages
    	JS_WriteLog ("[111] From JavaSup: About to invoke main");
    
    	MyInvokeMain ();
    }

    // More logging messages
    JS_WriteLog ("[112] From JavaSup: Finished execution");
    
    // Wait until all non-daemon threads are dead
    MyDestroyJavaVM ();
    
    // More logging messages
    JS_WriteLog ("[113] From JavaSup: JavaVM Destroyed (Finished = %d)", 
    		 stJavaProgramFinished);
    
    if (!stJavaProgramFinished)
    {
    	stJavaProgramFinished = TRUE;
    	
    	MyChangeWindowTitle (STATUS_FINISHED);
    	
    	// Notify the main program that we're finished.
    	myNotifyMainExecutionFinishedEvent = OpenEvent (EVENT_ALL_ACCESS, FALSE, 
				        READY_NOTIFY_JAVA_PROGRAM_FINISHED);
	if (myNotifyMainExecutionFinishedEvent != NULL)
	{
    	    SetEvent (myNotifyMainExecutionFinishedEvent);
    	    CloseHandle (myNotifyMainExecutionFinishedEvent);
	}

    	// More logging messages
    	JS_WriteLog ("[114] From JavaSup: Main notified");
    	
	// Close the open input and output files after giving time 
	// to get all the left over I/O out of the system
	Sleep (200); 
    	if (stInputFile != NULL)
    	{
	    fclose (stInputFile);
	    stInputFile = NULL;
    	}
    	if (stOutputFile != NULL)
    	{
	    fclose (stOutputFile);
	    stOutputFile = NULL;
    	}

	// If there was an exception, send it now
	if (stInterceptException)
	{
	    stInterceptException = FALSE;
	    MySendException ();

    	    // More logging messages
    	    JS_WriteLog ("[115] From JavaSup: Exception sent");
	}
	    	
    	// If there's output for the printer, send it now.
    	if (stOutputFileIsPrinter)
    	{
    	    stOutputFileIsPrinter = FALSE;
	    JS_SubmitPrintJob (NULL, stPrinterOutput, stPrinterOutputLen);
	    free (stPrinterOutput);

    	    // More logging messages
    	    JS_WriteLog ("[116] From JavaSup: Print job sent");
    	}
    }

    // More logging messages
    JS_WriteLog ("[117] From JavaSup: Console visibility = %d", 
    		 stConsoleVisible);
    	
    // Java has finished.  If we don't have a Console visible, 
    // terminate the process.
    if (!stConsoleVisible)
    {
    	// Eliminate the other processes.
    	JS_TerminateProcess ();

    	// More logging messages
    	JS_WriteLog ("[118] From JavaSup: Processes terminated");
    }

    // This stops the program from ending when the main thread returns
    ExitThread (0);
} // WinMain


/************************************************************************/
/* JS_ErrorMessage							*/
/************************************************************************/
void	JS_ErrorMessage (int pmMessageStringResource, ...)
{
    int		myResult;
    char 	myDialogTitle [1024], myDialogMessage [1024];
    va_list 	myArgList;
    char	*myFormattedMessage;
    
    // Get the window title
    JS_LoadString (IDS_ERROR_BOX_TITLE, myDialogTitle, sizeof (myDialogTitle));
    		
    // Get the window format string    
    JS_LoadString (pmMessageStringResource, myDialogMessage, 
    		   sizeof (myDialogMessage));
    
    // Process the additional arguments for use in FormatMessage
    va_start (myArgList, pmMessageStringResource);
    
    // Format the message
    myResult = FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_STRING, myDialogMessage, 0, 
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &myFormattedMessage, 0, &myArgList);
    if (myResult == 0)	  
    {
    	char	line [256];
    	
    	wsprintf (line, 
    	    "Unable to format error message\n\n    %s\n\nError number=%d",
    	    myDialogMessage, GetLastError ());
    	MessageBox (NULL, line, "Internal Java Supervisor Error", 0);
	JS_TerminateProcess ();
    }    

    // More logging messages
    JS_WriteLog ("[120] From JavaSup: Error occurred: \"%s\"", 
    		 myFormattedMessage);
    	
    // Close the variable args    
    va_end (myArgList);

    MessageBox (NULL, myFormattedMessage, myDialogTitle, 
        MB_OK | MB_ICONINFORMATION);
    
    // Free the buffer allocated by FormatMessage
    LocalFree (myFormattedMessage);
    
    JS_TerminateProcess ();
} // JS_ErrorMessage


/************************************************************************/
/* JS_LoadString							*/
/************************************************************************/
void	JS_LoadString (int pmStringResource, char *pmString, int pmStringSize)
{
    int	myResult;
    
    myResult = LoadString (gProgram.applicationInstance, pmStringResource,
			   pmString, pmStringSize);
    if (myResult == 0)	  
    {
    	char	line [256];
    	
    	wsprintf (line, "Unable to load string resource #%d",
    	    pmStringResource);
    	MessageBox (NULL, line, "Internal Java Supervisor Error", 0);
	JS_TerminateProcess ();
    }
} // JS_LoadString

			  
/************************************************************************/
/* JS_PauseJavaProgram							*/
/************************************************************************/
void	JS_PauseJavaProgram (void)
{
    // Event to notify main program of exection halted
    HANDLE	myNotifyMainExecutionPausedEvent;
    
    // Notify the main program 
    myNotifyMainExecutionPausedEvent = OpenEvent (EVENT_ALL_ACCESS, FALSE, 
	READY_NOTIFY_JAVA_PROGRAM_PAUSED);
    if (myNotifyMainExecutionPausedEvent != NULL)
    {
	SetEvent (myNotifyMainExecutionPausedEvent);
	CloseHandle (myNotifyMainExecutionPausedEvent);
    }
} // JS_PauseJavaProgram


/************************************************************************/
/* JS_ResumeJavaProgram							*/
/************************************************************************/
void	JS_ResumeJavaProgram (void)
{
    // Event to notify main program of exection halted
    HANDLE	myNotifyMainExecutionResumedEvent;
    
    // Notify the main program 
    myNotifyMainExecutionResumedEvent = OpenEvent (EVENT_ALL_ACCESS, FALSE, 
	READY_NOTIFY_JAVA_PROGRAM_RESUMED);
    if (myNotifyMainExecutionResumedEvent != NULL)
    {
	SetEvent (myNotifyMainExecutionResumedEvent);
	CloseHandle (myNotifyMainExecutionResumedEvent);
    }
} // JS_ResumeJavaProgram


/************************************************************************/
/* JS_SendFailMessage							*/
/*									*/
/* Note, this is used to send messages to EdFail_AddMessage, which is a	*/
/* series of messages that are saved and output if/when the program	*/
/* crashes.								*/
/************************************************************************/
void	JS_SendFailMessage (char *pmMessage, ...)
{
    HANDLE	myNotifyMainFailMessage;
    int		myBytesWritten, myLen;
    va_list 	myArgList;
    char	myFailMessage [4096];
        	    	
    // First, notify main that we're going to send a bunch of
    // stuff down the pipe.
    myNotifyMainFailMessage = OpenEvent (EVENT_ALL_ACCESS, FALSE, 
				   READY_NOTIFY_FAIL_MESSAGE);
    if (myNotifyMainFailMessage == NULL)
    {
	JS_ErrorMessage (IDS_UNABLE_TO_OPEN_FAIL_MESSAGE_SIGNAL);
    }

    SetEvent (myNotifyMainFailMessage);
    CloseHandle (myNotifyMainFailMessage);

    strcpy (myFailMessage, "JavaSup: ");
    
    va_start (myArgList, pmMessage);
    wvsprintf (&myFailMessage [strlen (myFailMessage)], pmMessage, myArgList);
    va_end (myArgList);

    // Send the number of characters
    myLen = strlen (myFailMessage);
    if ((!WriteFile (stToMainPipe, &myLen, 4, &myBytesWritten, NULL)) || 
        (myBytesWritten != 4))
    {
	JS_ErrorMessage (IDS_UNABLE_TO_WRITE_FAIL_MESSAGE, GetLastError ());
    }
    
    // Now send the contents of the window
    if ((!WriteFile (stToMainPipe, myFailMessage, myLen, 
          &myBytesWritten, NULL)) || (myBytesWritten != myLen))
    {
	JS_ErrorMessage (IDS_UNABLE_TO_WRITE_FAIL_MESSAGE, GetLastError ());
    }
} // JS_SendFailMessage


/************************************************************************/
/* JS_SignalMainToTerminate						*/
/************************************************************************/
void	JS_SignalMainToTerminate (void)
{
    HANDLE	myNotifyMainTerminateEvent;
    
    myNotifyMainTerminateEvent = OpenEvent (EVENT_ALL_ACCESS, FALSE, 
				            READY_NOTIFY_TERMINATE);
    if (myNotifyMainTerminateEvent != NULL)
    {
    	SetEvent (myNotifyMainTerminateEvent);
    	CloseHandle (myNotifyMainTerminateEvent);
    }
} // JS_SignalMainToTerminate


/************************************************************************/
/* JS_SubmitPrintJob							*/
/************************************************************************/
void	JS_SubmitPrintJob (HWND pmWindow, char *pmText, int pmLen)
{
    HANDLE	myNotifyMainPrint;
    int		myBytesWritten;
        	    	
    // First, notify main that we're going to send a bunch of
    // stuff down the pipe.
    myNotifyMainPrint = OpenEvent (EVENT_ALL_ACCESS, FALSE, 
				   READY_NOTIFY_PRINT_OUTPUT);
    if (myNotifyMainPrint == NULL)
    {
	JS_ErrorMessage (IDS_UNABLE_TO_OPEN_PRINT_SIGNAL);
    }

    SetEvent (myNotifyMainPrint);
    CloseHandle (myNotifyMainPrint);

    // Send the window being printed    
    if ((!WriteFile (stToMainPipe, &pmWindow, 4, &myBytesWritten, NULL)) || 
    	(myBytesWritten != 4))
    {
	JS_ErrorMessage (IDS_UNABLE_TO_WRITE_PRINT_JOB, GetLastError ());
    }
    
    // Send the number of characters    
    if ((!WriteFile (stToMainPipe, &pmLen, 4, &myBytesWritten, NULL)) || 
    	(myBytesWritten != 4))
    {
	JS_ErrorMessage (IDS_UNABLE_TO_WRITE_PRINT_JOB, GetLastError ());
    }
    
    // Now send the contents of the window
    if ((!WriteFile (stToMainPipe, pmText, pmLen, 
        &myBytesWritten, NULL)) || (myBytesWritten != pmLen))
    {
	JS_ErrorMessage (IDS_UNABLE_TO_WRITE_PRINT_JOB, GetLastError ());
    }

    // Set the main program's window in front
    SetForegroundWindow (stReadyWindow);
} // JS_SubmitPrintJob


/************************************************************************/
/* JS_TerminateProcess							*/
/************************************************************************/
void	JS_TerminateProcess (void)
{
    HANDLE	myNotifyMainExecutionHaltedEvent;

    // Close open files    
    if (stInputFile != NULL)
    {
	fclose (stInputFile);
    }
    if (stOutputFile != NULL)
    {
	fclose (stOutputFile);
    }
    
    // If there was an exception, send it now
    if (stInterceptException)
    {
	stInterceptException = FALSE;
	MySendException ();
    }
	    	
    // If there's output for the printer, send it now.
    if (stOutputFileIsPrinter)
    {
        stOutputFileIsPrinter = FALSE;
	JS_SubmitPrintJob (NULL, stPrinterOutput, stPrinterOutputLen);
	free (stPrinterOutput);
    }

    // Notify main program
    myNotifyMainExecutionHaltedEvent = OpenEvent (EVENT_ALL_ACCESS, FALSE, 
				            READY_NOTIFY_JAVA_PROGRAM_HALTED);
    if (myNotifyMainExecutionHaltedEvent != NULL)
    {
    	SetEvent (myNotifyMainExecutionHaltedEvent);
    	CloseHandle (myNotifyMainExecutionHaltedEvent);
    }
    
    // Quit the program
    ExitProcess (0);
} // JS_TerminateProcess


/************************************************************************/
/* JS_WriteLog								*/
/************************************************************************/
void	JS_WriteLog (char *pmFormatString, ...)
{
    va_list	myArgList;
    char	myFullMessage [2048];
    DWORD	myNumBytesWritten;
    
    if (stLogging)
    {
        va_start (myArgList, pmFormatString);
        wvsprintf (myFullMessage, pmFormatString, myArgList);
        va_end (myArgList);
        
    	if (!WriteFile (stLogFileHandle, myFullMessage, strlen (myFullMessage), 
    		        &myNumBytesWritten, NULL))
    	{
    	    // ERROR! Write failed!
    	    JS_ErrorMessage (IDS_LOG_FILE_WRITE_FAILED, GetLastError ());
    	    return;
    	}
    	if (myNumBytesWritten != strlen (myFullMessage))
    	{
    	    // ERROR! Write short!
    	    JS_ErrorMessage (IDS_LOG_FILE_WRITE_FAILED1, myNumBytesWritten, 
    	    		     strlen (myFullMessage));
    	    return;
    	}
    	if (!WriteFile (stLogFileHandle, "\r\n", 2, &myNumBytesWritten, NULL))
    	{
    	    // ERROR! Write failed!
    	    JS_ErrorMessage (IDS_LOG_FILE_WRITE_FAILED, GetLastError ());
    	    return;
    	}
    	if (myNumBytesWritten != 2)
    	{
    	    // ERROR! Write short!
    	    JS_ErrorMessage (IDS_LOG_FILE_WRITE_FAILED1, myNumBytesWritten, 2);
    	    return;
    	}
        if (!FlushFileBuffers (stLogFileHandle))
    	{
    	    // ERROR! Flush failed!
    	    JS_ErrorMessage (IDS_LOG_FILE_FLUSH_FAILED, GetLastError ());
    	    return;
    	}
    }
} // JS_WriteLog


/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/

/************************************************************************/
/* MyChangeWindowTitle							*/
/************************************************************************/
static void	MyChangeWindowTitle (int pmStatus)
{
    char	myConsoleName [1024];
    
    if (stConsoleVisible)
    {   
    	wsprintf (myConsoleName, 
            "%s - %s - Standard Input/Output - Ready to Program", 
            stClassToRunName, stWindowTitles [pmStatus]);
    	SendNotifyMessage (stConsoleWindow, WM_SET_RUN_STATUS, 
    		           (WPARAM) pmStatus, (LPARAM) NULL);
    }
} // MyChangeWindowTitle


/************************************************************************/
/* MyDestroyJavaVM							*/
/*									*/
/* Actually, it doesn't destroy the JVM.  But it does wait until all	*/
/* the threads are dead.						*/
/************************************************************************/
static void	MyDestroyJavaVM (void)
{
    (*stJVM) -> DestroyJavaVM (stJVM);
} // MyDestroyJavaVM


/************************************************************************/
/* MyFindLibraryAddress							*/
/************************************************************************/
static void	*MyFindLibraryAddress (char *pmDLLPath, char *pmProcName)
{
    HINSTANCE	myLibrary;
    void	*myProcAddress;
    
    myLibrary = LoadLibrary (pmDLLPath);
    if (myLibrary == NULL)
    {
	int	myErrorCode = GetLastError ();
	char	*myErrorMessage;
	
	FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM | 
	    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
    	    NULL, myErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
    	    (LPTSTR) &myErrorMessage, 0, NULL);
    	    
    	JS_ErrorMessage (IDS_UNABLE_TO_LOAD_LIBRARY, pmDLLPath, myErrorMessage,
    	    myErrorCode);
    	LocalFree (myErrorMessage);
    }

    myProcAddress = GetProcAddress (myLibrary, pmProcName);
    if (myProcAddress == NULL)
    {
    	JS_ErrorMessage (IDS_UNABLE_TO_FIND_PROC, pmProcName, pmDLLPath);
    }
    
    return myProcAddress;
} // MyFindLibraryAddress


/************************************************************************/
/* MyGetApplicationDirectory						*/
/************************************************************************/
static void	MyGetApplicationDirectory (void)
{
    char	*myPtr;
    
    GetModuleFileName (NULL, stApplicationDirectory, PATHNAME_LENGTH);
    myPtr = strrchr (stApplicationDirectory, '\\');
    if (myPtr == NULL)
    {
    	JS_ErrorMessage (IDS_UNEXPECTED_PATH, stApplicationDirectory);
    }
    myPtr++;
    *myPtr = 0;
} // MyGetApplicationDirectory


/************************************************************************/
/* MyGetJavaException							*/
/************************************************************************/
static void	MyGetJavaException (char *pmMessage)
{
    if ((*stJNIEnv)->ExceptionOccurred (stJNIEnv)) 
    {
    	jthrowable	myException;
    	jclass		myClass;
    	jmethodID	myMethod;
    	jstring		myJavaString;
    	const char	*myString;
    	
	myException = (*stJNIEnv)->ExceptionOccurred (stJNIEnv); 
	if (myException == NULL) 
	{ 
	    strcpy (pmMessage, "JNI Error - cannot get exception object");
	    return; 
	}

	(*stJNIEnv)->ExceptionDescribe (stJNIEnv);

	myClass = (*stJNIEnv)->FindClass (stJNIEnv, "java/lang/Throwable"); 
	if (myClass == NULL) 
	{ 
	    strcpy (pmMessage, 
	    	    "JNI Error - cannot find class 'java/lang/Throwable'"); 
	    return; 
	}
	
	myMethod = (*stJNIEnv)->GetMethodID (stJNIEnv, myClass, "toString", 
					     "()Ljava/lang/String;"); 
	if (myMethod == NULL) 
	{ 
	    strcpy (pmMessage, 
	    	    "JNI Error - cannot get method ID for 'toString'"); 
	    return; 
	}

	myJavaString = (jstring) (*stJNIEnv)->CallObjectMethod (stJNIEnv, 
							myException, myMethod); 
	if (myJavaString == NULL) 
	{ 
	    strcpy (pmMessage, 
	    	    "JNI Error - cannot get method ID for 'toString'"); 
	    return; 
	}
	
	myString = (*stJNIEnv)->GetStringUTFChars (stJNIEnv, myJavaString,
						   NULL); 
	if (myString == NULL) 
	{ 
	    strcpy (pmMessage, "JNI Error - 'GetStringUTFChars' returned NULL"); 
	    return; 
	}
	
	strcpy (pmMessage, myString);
	(*stJNIEnv)->ReleaseStringUTFChars (stJNIEnv, myJavaString, myString);
    }
    else
    {
    	strcpy (pmMessage, "No exception");
    }
} // MyGetJavaException


/************************************************************************/
/* MyInvokeAppletRunner							*/
/************************************************************************/
static void	MyInvokeAppletRunner (void)
{
    jclass		myClass;
    jmethodID		myMethodID;
    jobject		myApplet;
    jclass		myStringClass;
    jobjectArray	myArgs = NULL;
    jstring		myCodeBaseJavaString, myDocumentBaseJavaString;
    char		myException [2048];
    int			cnt;

    //    
    // Instantiate an Applet of the appropriate class    
    //
    // Find the class
    myClass = (*stJNIEnv) -> FindClass (stJNIEnv, stClassToRunName);
    if (myClass == NULL)
    {
    	// This means an exception was thrown.
    	MyGetJavaException (myException);
    	JS_WriteLog ("[EXC 01]: %s", myException);
    		
    	JS_ErrorMessage (IDS_UNABLE_TO_FIND_CLASS, stClassToRunName);
    }
    
    // Find the constructor
    myMethodID = (*stJNIEnv) -> GetMethodID (stJNIEnv, myClass, 
    					     CONSTRUCTOR_METHOD_NAME, "()V");
    if (myMethodID == NULL)
    {
    	// This means an exception was thrown.
    	MyGetJavaException (myException);
    	JS_WriteLog ("[EXC 02]: %s", myException);
    		
    	JS_ErrorMessage (IDS_UNABLE_TO_FIND_METHOD, stClassToRunName, 
    			 CONSTRUCTOR_METHOD_NAME);
    }

    // Instantiate the applet
    myApplet = (*stJNIEnv) -> NewObject (stJNIEnv, myClass, myMethodID);

    // Handle any exceptions that occur    					 
    if ((*stJNIEnv) -> ExceptionOccurred (stJNIEnv) != NULL)
    {
    	Sleep (200); // Give a little time for rest of text to flush
    	stInterceptException = TRUE;
    	(*stJNIEnv) -> ExceptionDescribe (stJNIEnv);
    	return;
    }

    //
    // Find the runApplet method of the ReadySetUp class
    //
    // Find the ready.ReadySetUp class
    myClass = (*stJNIEnv) -> FindClass (stJNIEnv, READY_SETUP_CLASS_NAME);
    if (myClass == NULL)
    {
    	// This means an exception was thrown.
    	MyGetJavaException (myException);
    	JS_WriteLog ("[EXC 03]: %s", myException);
    		
    	JS_ErrorMessage (IDS_UNABLE_TO_FIND_CLASS, READY_SETUP_CLASS_NAME);
    }
    
    // Find the runApplet method
    myMethodID = (*stJNIEnv) -> GetStaticMethodID (stJNIEnv, myClass, 
    				READY_RUNAPPLET_METHOD_NAME,
    				"(Ljava/applet/Applet;II[Ljava/lang/String;"
    				"Ljava/lang/String;Ljava/lang/String;)V");
    if (myMethodID == NULL)
    {
    	// This means an exception was thrown.
    	MyGetJavaException (myException);
    	JS_WriteLog ("[EXC 04]: %s", myException);
    		
    	JS_ErrorMessage (IDS_UNABLE_TO_FIND_METHOD, READY_SETUP_CLASS_NAME,
    			 READY_RUNAPPLET_METHOD_NAME);
    }

    // Create the array of strings for the parameter method
    myStringClass = (*stJNIEnv) -> FindClass (stJNIEnv, "java/lang/String");
    if (myStringClass == NULL)
    {
    	// This means an exception was thrown.
    	MyGetJavaException (myException);
    	JS_WriteLog ("[EXC 05]: %s", myException);
    		
    	JS_ErrorMessage (IDS_UNABLE_TO_FIND_STRING_CLASS);
    }
    myArgs = (*stJNIEnv) -> NewObjectArray (stJNIEnv, stNumAppletParams,
    					myStringClass, NULL);
    if (myArgs == NULL)
    {
    	// This means an exception was thrown.
    	MyGetJavaException (myException);
    	JS_WriteLog ("[EXC 06]: %s", myException);
    		
    	JS_ErrorMessage (IDS_UNABLE_TO_CREATE_ARGS_ARRAY, stNumAppletParams);
    }
    
    for (cnt = 0 ; cnt < stNumAppletParams ; cnt++)
    {
	jstring	myJavaString = (*stJNIEnv) -> NewStringUTF (stJNIEnv, 
	    					stAppletParams [cnt]);
	if (myJavaString == NULL)
	{
    	    // This means an exception was thrown.
    	    MyGetJavaException (myException);
    	    JS_WriteLog ("[EXC 07]: %s", myException);
    		
    	    JS_ErrorMessage (IDS_UNABLE_TO_CREATE_ARG_STRING, cnt + 1);
	}	    					
	(*stJNIEnv) -> SetObjectArrayElement (stJNIEnv, myArgs, cnt, 
	    				      myJavaString);
    }

    // Create the codeBase and documentBase strings
    myCodeBaseJavaString = (*stJNIEnv) -> NewStringUTF (stJNIEnv, stCodeBase);
    myDocumentBaseJavaString = (*stJNIEnv) -> NewStringUTF (stJNIEnv, 
    							    stDocumentBase);

    // Call the runApplet method
    (*stJNIEnv) -> CallStaticVoidMethod (stJNIEnv, myClass, myMethodID, 
    					 myApplet, stAppletWidth, 
    					 stAppletHeight, myArgs, 
    					 myCodeBaseJavaString, 
    					 myDocumentBaseJavaString);
    					 
    // Handle any exceptions that occur    					 
    if ((*stJNIEnv) -> ExceptionOccurred (stJNIEnv) != NULL)
    {
    	Sleep (200); // Give a little time for rest of text to flush
    	stInterceptException = TRUE;
    	(*stJNIEnv) -> ExceptionDescribe (stJNIEnv);
    	return;
    }
} // MyInvokeAppletRunner


/************************************************************************/
/* MyInvokeMain								*/
/************************************************************************/
static void	MyInvokeMain (void)
{
    jclass		myClass;
    jmethodID		myMethodID;
    jclass		myStringClass;
    jobjectArray	myArgs = NULL;
    char		myException [2048], myCurrentDirectory [1025];
    int			cnt;

    //
    // Find the main method of the class to be run
    //
    myClass = (*stJNIEnv) -> FindClass (stJNIEnv, stClassToRunName);
    if (myClass == NULL)
    {
    	// This means an exception was thrown.
    	MyGetJavaException (myException);
    	JS_WriteLog ("[EXC 10]: %s", myException);
    	JS_SendFailMessage ("Unable to open \"%s\": %s", stClassToRunName,
    			    myException);
	GetCurrentDirectory (1024, myCurrentDirectory);
    	JS_SendFailMessage ("Current directory: \"%s\"", myCurrentDirectory);
    	JS_ErrorMessage (IDS_UNABLE_TO_FIND_CLASS, stClassToRunName);
    }
    
    myMethodID = (*stJNIEnv) -> GetStaticMethodID (stJNIEnv, myClass, 
    						   MAIN_METHOD_NAME,
    						   "([Ljava/lang/String;)V");
    if (myMethodID == NULL)
    {
    	// This means an exception was thrown.
    	MyGetJavaException (myException);
    	JS_WriteLog ("[EXC 11]: %s", myException);
    		
    	JS_ErrorMessage (IDS_UNABLE_TO_FIND_METHOD, stClassToRunName, 
    			 MAIN_METHOD_NAME);
    }

    // 
    // Create the array of java strings for the command line arguments
    //
    myStringClass = (*stJNIEnv) -> FindClass (stJNIEnv, "java/lang/String");
    if (myStringClass == NULL)
    {
    	// This means an exception was thrown.
    	MyGetJavaException (myException);
    	JS_WriteLog ("[EXC 12]: %s", myException);
    		
    	JS_ErrorMessage (IDS_UNABLE_TO_FIND_STRING_CLASS);
    }
    myArgs = (*stJNIEnv) -> NewObjectArray (stJNIEnv, stNumCmdLineArgs,
    					myStringClass, NULL);
    if (myArgs == NULL)
    {
    	// This means an exception was thrown.
    	MyGetJavaException (myException);
    	JS_WriteLog ("[EXC 13]: %s", myException);
    		
    	JS_ErrorMessage (IDS_UNABLE_TO_CREATE_ARGS_ARRAY, stNumCmdLineArgs);
    }
    
    for (cnt = 0 ; cnt < stNumCmdLineArgs ; cnt++)
    {
	jstring	myJavaString = (*stJNIEnv) -> NewStringUTF (stJNIEnv, 
	    					stCommandLineArgs [cnt]);
	if (myJavaString == NULL)
	{
    	    // This means an exception was thrown.
    	    MyGetJavaException (myException);
    	    JS_WriteLog ("[EXC 14]: %s", myException);
    		
    	    JS_ErrorMessage (IDS_UNABLE_TO_CREATE_ARG_STRING, cnt + 1);
	}	    					
	(*stJNIEnv) -> SetObjectArrayElement (stJNIEnv, myArgs, cnt, 
	    				      myJavaString);
    }

    //
    // Call the main method and pass back any exception that occurred
    //
    (*stJNIEnv) -> CallStaticVoidMethod (stJNIEnv, myClass, myMethodID, myArgs);
    if ((*stJNIEnv) -> ExceptionOccurred (stJNIEnv) != NULL)
    {
    	Sleep (200); // Give a little time for rest of text to flush
    	stInterceptException = TRUE;
    	(*stJNIEnv) -> ExceptionDescribe (stJNIEnv);
    }
    
    //
    // Notify the Console method that the Main method has finished
    //
    myClass = (*stJNIEnv) -> FindClass (stJNIEnv, HSA_CONSOLE_CLASS_NAME);
    if (myClass != NULL)
    {
        myMethodID = (*stJNIEnv) -> GetStaticMethodID (stJNIEnv, myClass, 
    					MAIN_RETURNED_METHOD_NAME, "()V");
    	if (myMethodID != NULL)
    	{
	    (*stJNIEnv) -> CallStaticVoidMethod (stJNIEnv, myClass, myMethodID, 
	    					 NULL);
    	}
    }
} // MyInvokeMain


/************************************************************************/
/* MyInvokePauseJava							*/
/*									*/
/* This subprogram is meant to be invoked as a separate thread.	 It is	*/
/* only used with Java 1.3						*/
/************************************************************************/
static int	MyInvokePauseJava (LPVOID pmDummy)
{
    jint	myResult;
    JNIEnv	*myJNIEnv;
    jclass	myClass;
    jmethodID	myMethodID;
    char	myException [2048];
    
    // Attach the thread to the JVM.
    myResult = (*stJVM) -> AttachCurrentThread (stJVM, 
						(void **) &myJNIEnv, NULL); 


    // Finds the java.lang.System class	     
    myClass = (*myJNIEnv) -> FindClass (myJNIEnv, READY_SETUP_CLASS_NAME);
    if (myClass == NULL)
    {
    	// This means an exception was thrown.
    	MyGetJavaException (myException);
    	JS_WriteLog ("[EXC 20]: %s", myException);
    		
    	JS_ErrorMessage (IDS_UNABLE_TO_FIND_CLASS, READY_SETUP_CLASS_NAME);
    }

    // Find the java.lang.System.exit method    
    myMethodID = (*myJNIEnv) -> GetStaticMethodID (myJNIEnv, myClass, 
    						   PAUSE_JAVA_METHOD_NAME, 
    						   "()V");
    if (myMethodID == NULL)
    {
    	// This means an exception was thrown.
    	MyGetJavaException (myException);
    	JS_WriteLog ("[EXC 21]: %s", myException);
    		
    	JS_ErrorMessage (IDS_UNABLE_TO_FIND_METHOD, READY_SETUP_CLASS_NAME, 
    			 PAUSE_JAVA_METHOD_NAME);
    }

    // Call the pauseJavaThreads method.  This suspends all the threads 
    // associated with the JVM
    (*myJNIEnv) -> CallStaticVoidMethod (myJNIEnv, myClass, myMethodID, 0);
    if ((*myJNIEnv) -> ExceptionOccurred (myJNIEnv) != NULL)
    {
    	(*myJNIEnv) -> ExceptionDescribe (myJNIEnv);
    }

    (*stJVM) -> DetachCurrentThread (stJVM);
    
    return 0;
} // MyInvokePauseJava


/************************************************************************/
/* MyInvokeReadySetup							*/
/************************************************************************/
static void	MyInvokeReadySetup (void)
{
    jclass		myClass;
    jmethodID		myMethodID;
    char		myException [2048];
    
    myClass = (*stJNIEnv) -> FindClass (stJNIEnv, READY_SETUP_CLASS_NAME);
    if (myClass == NULL)
    {
    	if (stLogging)
    	{
    	    // This means an exception was thrown.
    	    MyGetJavaException (myException);
    	    JS_WriteLog ("[EXC 30]: %s", myException);
    		
    	    // Okay, at least try outputting the properties.
    	    MyInvokePrintPropDoIt ();
    	}
    	JS_ErrorMessage (IDS_UNABLE_TO_FIND_CLASS, READY_SETUP_CLASS_NAME);
    }
    
    myMethodID = (*stJNIEnv) -> GetStaticMethodID (stJNIEnv, myClass, 
    						   READY_SETUP_METHOD_NAME,
    						   "()V");
    if (myMethodID == NULL)
    {
    	// This means an exception was thrown.
    	MyGetJavaException (myException);
    	JS_WriteLog ("[EXC 31]: %s", myException);
    		
    	JS_ErrorMessage (IDS_UNABLE_TO_FIND_METHOD, READY_SETUP_CLASS_NAME,
    			 READY_SETUP_METHOD_NAME);
    }

    (*stJNIEnv) -> CallStaticVoidMethod (stJNIEnv, myClass, myMethodID, NULL);
    if ((*stJNIEnv) -> ExceptionOccurred (stJNIEnv) != NULL)
    {
    	(*stJNIEnv) -> ExceptionDescribe (stJNIEnv);
    }
} // MyInvokeReadySetup


/************************************************************************/
/* MyInvokePrintPropDoIt						*/
/************************************************************************/
static void	MyInvokePrintPropDoIt (void)
{
    jclass		myClass;
    jmethodID		myMethodID;
    char		myException [1024];
    
    myClass = (*stJNIEnv) -> FindClass (stJNIEnv, PRINTPROP_CLASS_NAME);
    if (myClass == NULL)
    {
    	// This means an exception was thrown.
    	MyGetJavaException (myException);
    	JS_WriteLog ("[EXC 40]: %s", myException);
    	
    	JS_ErrorMessage (IDS_UNABLE_TO_FIND_CLASS, PRINTPROP_CLASS_NAME);
    }
    
    myMethodID = (*stJNIEnv) -> GetStaticMethodID (stJNIEnv, myClass, 
    						   PRINTPROP_DOIT_METHOD_NAME,
    						   "()V");
    if (myMethodID == NULL)
    {
    	// This means an exception was thrown.
    	MyGetJavaException (myException);
    	JS_WriteLog ("[EXC 41]: %s", myException);
    	
    	JS_ErrorMessage (IDS_UNABLE_TO_FIND_METHOD, PRINTPROP_CLASS_NAME,
    			 PRINTPROP_DOIT_METHOD_NAME);
    }

    (*stJNIEnv) -> CallStaticVoidMethod (stJNIEnv, myClass, myMethodID, NULL);
    if ((*stJNIEnv) -> ExceptionOccurred (stJNIEnv) != NULL)
    {
    	(*stJNIEnv) -> ExceptionDescribe (stJNIEnv);
    }
} // MyInvokePrintPropDoIt


/************************************************************************/
/* MyInvokeResumeJava							*/
/*									*/
/* This subprogram is meant to be invoked as a separate thread.	 It is	*/
/* only used with Java 1.3						*/
/************************************************************************/
static int	MyInvokeResumeJava (LPVOID pmDummy)
{
    jint	myResult;
    JNIEnv	*myJNIEnv;
    jclass	myClass;
    jmethodID	myMethodID;
    char	myException [2048];
    
    // Attach the thread to the JVM.
    myResult = (*stJVM) -> AttachCurrentThread (stJVM, 
						(void **) &myJNIEnv, NULL); 


    // Finds the java.lang.System class	     
    myClass = (*myJNIEnv) -> FindClass (myJNIEnv, READY_SETUP_CLASS_NAME);
    if (myClass == NULL)
    {
    	// This means an exception was thrown.
    	MyGetJavaException (myException);
    	JS_WriteLog ("[EXC 50]: %s", myException);
    	
    	JS_ErrorMessage (IDS_UNABLE_TO_FIND_CLASS, READY_SETUP_CLASS_NAME);
    }

    // Find the java.lang.System.exit method    
    myMethodID = (*myJNIEnv) -> GetStaticMethodID (myJNIEnv, myClass, 
    						   RESUME_JAVA_METHOD_NAME, 
    						   "()V");
    if (myMethodID == NULL)
    {
    	// This means an exception was thrown.
    	MyGetJavaException (myException);
    	JS_WriteLog ("[EXC 51]: %s", myException);
    	
    	JS_ErrorMessage (IDS_UNABLE_TO_FIND_METHOD, READY_SETUP_CLASS_NAME, 
    			 RESUME_JAVA_METHOD_NAME);
    }

    // Call the pauseJavaThreads method.  This suspends all the threads 
    // associated with the JVM
    (*myJNIEnv) -> CallStaticVoidMethod (myJNIEnv, myClass, myMethodID, 0);
    if ((*myJNIEnv) -> ExceptionOccurred (myJNIEnv) != NULL)
    {
    	(*myJNIEnv) -> ExceptionDescribe (myJNIEnv);
    }

    (*stJVM) -> DetachCurrentThread (stJVM);
    
    return 0;
} // MyInvokeResumeJava


/************************************************************************/
/* MyMallocWordFromString						*/
/************************************************************************/
static char	*MyMallocWordFromString (char **pmPtr)
{
    char	*myWord, *myText, *myPtr;
    
    myWord = MyReadWordFromString (pmPtr);
	    
    if (myWord == NULL)
    {
    	JS_ErrorMessage (IDS_MALFORMED_OPTION, stCommandLineOptions);
    }
    	    
    myText = malloc (strlen (myWord) + 1);
    if (myText == NULL)
    {
    	JS_ErrorMessage (IDS_OUT_OF_MEM_FOR_STRING, myWord);
    }
    
    strcpy (myText, myWord);
    
    // Convert the single quotes back to double quotes.
    myPtr = myText;
    while (*myPtr != 0)
    {
    	if (*myPtr == '\'')
    	{
    	    *myPtr = '"';
    	}
	myPtr++;
    }
    
    return myText;
} // MyMallocWordFromString


/************************************************************************/
/* MyMessageBox								*/
/************************************************************************/
static int	MyMessageBox (HWND pmWindow, int pmMessageBoxParams, 
			      int pmTitleStringResource, 
			      int pmMessageStringResource, ...)
{
    int		myResult;
    char 	myDialogTitle [1024], myDialogMessage [1024];
    va_list 	myArgList;
    char	*myFormattedMessage;
    
    // Get the window title
    JS_LoadString (pmTitleStringResource, myDialogTitle, 
    		   sizeof (myDialogTitle));
    		
    // Get the window format string    
    JS_LoadString (pmMessageStringResource, myDialogMessage, 
    	 	   sizeof (myDialogMessage));
    
    // Process the additional arguments for use in FormatMessage
    va_start (myArgList, pmMessageStringResource);
    
    // Format the message
    myResult = FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_STRING, myDialogMessage, 0, 
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &myFormattedMessage, 0, &myArgList);
    if (myResult == 0)	  
    {
    	char	line [256];
    	
    	wsprintf (line, "Unable to format message\n\n    %s\n\nError number=%d",
    	    myDialogMessage, GetLastError ());
    	MessageBox (NULL, line, "Internal Java Supervisor Error", 0);
	JS_TerminateProcess ();
    }    

    // Close the variable args    
    va_end (myArgList);

    // Set some default parameters
    if (pmMessageBoxParams == 0)
    {
    	pmMessageBoxParams = MB_OK | MB_ICONINFORMATION;
    }
    
    MessageBox (NULL, myFormattedMessage, myDialogTitle, pmMessageBoxParams);
    
    // Free the buffer allocated by FormatMessage
    LocalFree (myFormattedMessage);
    
    return myResult;
} // MyMessageBox


/************************************************************************/
/* MyOpenFile								*/
/************************************************************************/
static FILE	*MyOpenFile (char *pmPathName, BOOL pmOpenForRead)
{
    int		myOpenFor, myFileShare, myCreationDisposition;
    char	*myCFileOpenFor;
    HANDLE	myFile;
    int		myCFileDescriptor;
    FILE	*myCFileHandle;
    
    if (pmOpenForRead)
    {
    	myOpenFor = GENERIC_READ;
    	myFileShare = FILE_SHARE_READ;
    	myCreationDisposition = OPEN_EXISTING;
    	myCFileOpenFor = "r";
    }
    else
    {
    	myOpenFor = GENERIC_WRITE;
    	myFileShare = 0;
    	myCreationDisposition = CREATE_ALWAYS;
    	myCFileOpenFor = "w";
    }
    
    // Open the file
    myFile = CreateFile (pmPathName, myOpenFor, myFileShare, NULL,
    	myCreationDisposition, 
    	FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (myFile == (HANDLE) INVALID_HANDLE_VALUE)
    {
	int	myErrorCode = GetLastError ();
	char	*myErrorMessage;
	
	FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM | 
	    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
    	    NULL, myErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
    	    (LPTSTR) &myErrorMessage, 0, NULL);
    	    
	if (pmOpenForRead)
	{
	    MyMessageBox (NULL, 0, IDS_CANT_OPEN_FILE, 
	        IDS_CANT_OPEN_FILE_FOR_READ, pmPathName, myErrorMessage, 
	        myErrorCode);
	}
	else
	{
	    MyMessageBox (NULL, 0, IDS_CANT_OPEN_FILE, 
	        IDS_CANT_OPEN_FILE_FOR_WRITE, pmPathName, 
	        myErrorMessage, myErrorCode);
	}
	LocalFree (myErrorMessage);
	JS_TerminateProcess ();
    }

    // Convert the Windows HANDLE into a C (FILE *)    
    myCFileDescriptor = _open_osfhandle ((int) myFile, _O_BINARY);
    myCFileHandle = _fdopen (myCFileDescriptor, myCFileOpenFor);

    return myCFileHandle;
} // MyOpenFile


/************************************************************************/
/* MyOpenReadyConsole							*/
/*									*/
/* This procedure spawns a thread that opens the standard input/output	*/
/* window and processes its events.					*/
/************************************************************************/
static void	MyOpenReadyConsole (void)
{
    HANDLE	myOpenReadyConsoleThread;
    DWORD	myThreadID;

    JSCon_Init ();
        
    stConsoleWindowOpened = CreateEvent (NULL, TRUE, FALSE, NULL);
    
    myOpenReadyConsoleThread = CreateThread (NULL, 0, 
        (LPTHREAD_START_ROUTINE) MyOpenReadyConsoleThread, NULL, 0, 
        &myThreadID);
    if (myOpenReadyConsoleThread == NULL)
    {
	JS_ErrorMessage (IDS_UNABLE_TO_CREATE_THREAD);
    }    		       
    CloseHandle (myOpenReadyConsoleThread);

    // Wait until the window is opened before returning
    WaitForSingleObject (stConsoleWindowOpened, INFINITE);
    CloseHandle (stConsoleWindowOpened);
} // MyOpenReadyConsole


/************************************************************************/
/* MyOpenReadyConsoleThread						*/
/*									*/
/* This procedure must be called in its own thread because it doesn't	*/
/* return.  Instead it stays processing events until shutdown.		*/
/************************************************************************/
static int	MyOpenReadyConsoleThread (LPVOID pmConsoleWindow)
{
    MSG		myMessage;
    HACCEL	myConsoleAccelerator;

    // Load accelerators
    myConsoleAccelerator = LoadAccelerators (gProgram.applicationInstance, 
        MAKEINTRESOURCE (CONSOLE_WINDOW_ACCELERATOR));

    // Create window
    stConsoleWindow = JSCon_Create (stClassToRunName);
    
    // Signal the main thread that the window is now open
    SetEvent (stConsoleWindowOpened);

    // Dispatch messages
    while (GetMessage (&myMessage, NULL, 0, 0))
    {
    	if (!TranslateAccelerator (stConsoleWindow, myConsoleAccelerator, 
    				   &myMessage))
	{    	    	
            TranslateMessage (&myMessage);
       	    DispatchMessage (&myMessage);
       	}
    }
    
    return 0;
} // MyOpenReadyConsoleThread


/************************************************************************/
/* MyPrintVMMessages							*/
/************************************************************************/
static jint JNICALL	MyPrintVMMessages (FILE *pmFilePointer, 
					   const char *pmFormat, 
					   va_list pmArgs)
{
    char myLine [1024];
    _vsnprintf (myLine, sizeof (myLine), pmFormat, pmArgs);
    MessageBox (NULL, myLine, "JVM Message", 0);
    
    return 0;
} // MyPrintVMMessages

					   
/************************************************************************/
/* MyReadCommandLine							*/
/************************************************************************/
static void	MyReadCommandLine (char *pmCommandLine)
{
    char	*myPtr = pmCommandLine;
    char	*myWord;
    char	*myBuffer = NULL;

// printf ("Command Line = \"%s\"\n", pmCommandLine); // XYZ
    
    myWord = MyReadWordFromString (&myPtr);
    if (strcmp (myWord, "-pipe") == 0)
    {
    	myWord = MyReadWordFromString (&myPtr);
    	stFromMainPipe = (HANDLE) atoi (myWord);
   	
    	myWord = MyReadWordFromString (&myPtr);
    	stToMainPipe = (HANDLE) atoi (myWord);

    	myWord = MyReadWordFromString (&myPtr);
    	stReadyProcess = (HANDLE) atoi (myWord);

    	myWord = MyReadWordFromString (&myPtr);
    	stReadyWindow = (HWND) atoi (myWord);

    	// Read in the command line, then start reading options from command 
    	// line
    	myBuffer = MyReadFromPipe ();
// printf ("RadFromPipe = \"%s\"\n", myBuffer); // XYZ
    	myPtr = myBuffer;
    	myWord = MyReadWordFromString (&myPtr);
    }
    
    while (myWord != NULL)
    {
    	if ((myWord [0] != '-') && (myWord [0] != '/'))
    	{
    	    JS_ErrorMessage (IDS_MALFORMED_OPTION, stCommandLineOptions);
    	}
    
    	myWord++;
    	
    	if (strcmp (myWord, "java") == 0)
    	{
    	    stJVMDLLPath = MyMallocWordFromString (&myPtr);
    	}
    	else if (strcmp (myWord, "cp") == 0)
    	{
    	    stClassPath = MyMallocWordFromString (&myPtr);
    	}
    	else if (strcmp (myWord, "class") == 0)
    	{
    	    stClassToRunName = MyMallocWordFromString (&myPtr);
    	}
#ifdef NO_LONGER_NEEDED_AS_READY_SETS_APPROPRIATE_DIR    
    	else if (strcmp (myWord, "dir") == 0)
    	{
    	    stDirectoryToStart = MyMallocWordFromString (&myPtr);
    	}
#endif    	
    	else if (strcmp (myWord, "debugready") == 0)
    	{
    	    stDebuggingReady = TRUE;
    	}
    	else if (strcmp (myWord, "startindebugger") == 0)
    	{
    	    stStartInDebugger = TRUE;	
    	}
    	else if (strcmp (myWord, "applet") == 0)
    	{
    	    char	*myNumPtr, *myParamPtr;
    	    
    	    stIsApplet = TRUE;
    	    stCodeBase = MyMallocWordFromString (&myPtr);
    	    stDocumentBase = MyMallocWordFromString (&myPtr);
    	    myNumPtr = MyReadWordFromString (&myPtr);
    	    stAppletWidth = atoi (myNumPtr);
    	    myNumPtr = MyReadWordFromString (&myPtr);
    	    stAppletHeight = atoi (myNumPtr);
    	    myParamPtr = MyMallocWordFromString (&myPtr);

    	    while (TRUE)
    	    {
    	    	while ((*myParamPtr == ' ') || (*myParamPtr == '\t') ||
    	    	       (*myParamPtr == '\n') || (*myParamPtr == '\r'))
    	    	{
    	    	    myParamPtr++;
    	    	}
    	    	if (*myParamPtr == 0) break;
    	        stAppletParams [stNumAppletParams++] = myParamPtr;
    	    	while ((*myParamPtr != '\n') && (*myParamPtr != '\r') &&
    	    	       (*myParamPtr != 0))
    	    	{
    	    	    myParamPtr++;
    	    	}
    	    	if (*myParamPtr == 0) break;
    	    	*myParamPtr++ = 0;
    	    }
    	}
    	else if (strcmp (myWord, "i") == 0)
    	{
    	    stInputFromFile = TRUE;
    	    stInputPathName = MyMallocWordFromString (&myPtr);
    	    stInputFile = MyOpenFile (stInputPathName, OPEN_FOR_READ);
    	}
    	else if (strcmp (myWord, "ii") == 0)
    	{
    	    stInputFromFile = TRUE;
    	    stEchoInput = TRUE;
    	    stInputPathName = MyMallocWordFromString (&myPtr);
    	    stInputFile = MyOpenFile (stInputPathName, OPEN_FOR_READ);
    	}
    	else if (strcmp (myWord, "o") == 0)
    	{
    	    stOutputToFile = TRUE;
    	    stOutputPathName = MyMallocWordFromString (&myPtr);
    	    stOutputFile = MyOpenFile (stOutputPathName, OPEN_FOR_WRITE);
    	}
    	else if (strcmp (myWord, "oo") == 0)
    	{
    	    stOutputToFile = TRUE;
    	    stScreenAndFile = TRUE;
    	    stOutputPathName = MyMallocWordFromString (&myPtr);
    	    stOutputFile = MyOpenFile (stOutputPathName, OPEN_FOR_WRITE);
    	}
    	else if (strcmp (myWord, "p") == 0)
    	{
    	    stOutputToFile = TRUE;
    	    stOutputFileIsPrinter = TRUE;
    	    stPrinterOutput = malloc (10000);
    	    stPrinterOutputBufferSize = 10000;
    	    stPrinterOutputLen = 0;
    	}
    	else if (strcmp (myWord, "pp") == 0)
    	{
    	    stOutputToFile = TRUE;
    	    stScreenAndFile = TRUE;
    	    stOutputFileIsPrinter = TRUE;
    	    stPrinterOutput = malloc (10000);
    	    stPrinterOutputBufferSize = 10000;
    	    stPrinterOutputLen = 0;
    	}
    	else if (strcmp (myWord, "args") == 0)
    	{
    	    while (TRUE)
    	    {
    	    	stCommandLineArgs [stNumCmdLineArgs] = 
    	            MyMallocWordFromString (&myPtr);
    	        if (strcmp (stCommandLineArgs [stNumCmdLineArgs], "!@!@") == 0)
		{
		    break;
		}
	        stNumCmdLineArgs++;
	    }	        							
    	}
    	else if (strcmp (myWord, "props") == 0)
    	{
    	    //
	    // Read the following properties
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
	    //    int   memoryForJVMInMB;
	    //    BOOL	runningJRE13;
	    //    BOOL	verifyLineDB;
	    //
	    
	    char	*myString;
	    int		myNum;
	    
	    //	  BOOL	fullScreen;
	    myString = MyReadWordFromString (&myPtr);
	    if ((stricmp (myString, "Y") == 0) ||
	        (stricmp (myString, "T") == 0))
	    {
	    	stProperties.myProperties.fullScreen = TRUE;
	    }
	    else if ((stricmp (myString, "N") == 0) ||
	             (stricmp (myString, "F") == 0))
	    {
	    	stProperties.myProperties.fullScreen = FALSE;
	    }
	    else
	    {
	    	JS_ErrorMessage (IDS_BAD_FULLSCREEN_OPTION, myString);
	    }
	    
	    //    int	textRows;
	    myNum = atoi (MyReadWordFromString (&myPtr));
	    if (myNum == 0)
	    {
	    	JS_ErrorMessage (IDS_BAD_TEXTROWS_OPTION, myString);
	    }
	    else
	    {
	    	stProperties.myProperties.textRows = myNum;
	    }
	    
	    //    int	textCols;
	    myNum = atoi (MyReadWordFromString (&myPtr));
	    if (myNum == 0)
	    {
	    	JS_ErrorMessage (IDS_BAD_TEXTCOLS_OPTION, myString);
	    }
	    else
	    {
	    	stProperties.myProperties.textCols = myNum;
	    }
	    
	    //    int	textFontSize;
	    myNum = atoi (MyReadWordFromString (&myPtr));
	    if (myNum == 0)
	    {
	    	JS_ErrorMessage (IDS_BAD_TEXTFONTSIZE_OPTION, myString);
	    }
	    else
	    {
	    	stProperties.myProperties.textFontSize = myNum;
	    }
	    
	    //    char	textFontName [PROPERTY_LENGTH];
	    strcpy (stProperties.myProperties.textFontName,
	        MyReadWordFromString (&myPtr));

	    //    int	caretWidth;
	    myNum = atoi (MyReadWordFromString (&myPtr));
	    if (myNum == 0)
	    {
	    	JS_ErrorMessage (IDS_BAD_CARETWIDTH_OPTION, myString);
	    }
	    else
	    {
	    	stProperties.myProperties.caretWidth = myNum;
	    }
	    
	    //    BOOL	logging;
	    myString = MyReadWordFromString (&myPtr);
	    if ((stricmp (myString, "Y") == 0) ||
	        (stricmp (myString, "T") == 0))
	    {
	    	stLogging = TRUE;
	    }
	    else if ((stricmp (myString, "N") == 0) ||
	             (stricmp (myString, "F") == 0))
	    {
	    	stLogging = FALSE;
	    }
	    else
	    {
	    	JS_ErrorMessage (IDS_BAD_LOGGING_OPTION, myString);
	    }
	    
	    //    int   logFileHandle;
    	    stLogFileHandle = (HANDLE) atoi (MyReadWordFromString (&myPtr));
   	
	    //    BOOL	logWindowMessages;
	    myString = MyReadWordFromString (&myPtr);
	    if ((stricmp (myString, "Y") == 0) ||
	        (stricmp (myString, "T") == 0))
	    {
	    	stProperties.myProperties.logWindowMessages = TRUE;
	    }
	    else if ((stricmp (myString, "N") == 0) ||
	             (stricmp (myString, "F") == 0))
	    {
	    	stProperties.myProperties.logWindowMessages = FALSE;
	    }
	    else
	    {
	    	JS_ErrorMessage (IDS_BAD_LOGGINGWINDOWS_OPTION, myString);
	    }
	    
	    //    int 	dialogFontSize;
	    myNum = atoi (MyReadWordFromString (&myPtr));
	    if (myNum == 0)
	    {
	    	JS_ErrorMessage (IDS_BAD_DIALOGFONTSIZE_OPTION, myString);
	    }
	    else
	    {
	    	stProperties.topProperties.dialogFontSize = myNum;
	    }
	    
	    //    int	numSpacesPerTab;
	    myNum = atoi (MyReadWordFromString (&myPtr));
	    if (myNum == 0)
	    {
	    	JS_ErrorMessage (IDS_BAD_SPACESPERTAB_OPTION, myString);
	    }
	    else
	    {
	    	stProperties.textProperties.myProperties.numSpacesPerTab = myNum;
	    }
	    
	    //    int   memoryForJVMInMB;
	    stMaxJVMSize = atoi (MyReadWordFromString (&myPtr));
	    
	    //    BOOL	runningJRE13;
	    myString = MyReadWordFromString (&myPtr);
	    if ((stricmp (myString, "Y") == 0) ||
	        (stricmp (myString, "T") == 0))
	    {
	    	stRunningJRE13 = TRUE;
	    }
	    else if ((stricmp (myString, "N") == 0) ||
	             (stricmp (myString, "F") == 0))
	    {
	    	stRunningJRE13 = FALSE;
	    }
	    else
	    {
	    	JS_ErrorMessage (IDS_BAD_RUN13_OPTION, myString);
	    }

	    //    BOOL	verifyLineDB;
	    myString = MyReadWordFromString (&myPtr);
	    if ((stricmp (myString, "Y") == 0) ||
	        (stricmp (myString, "T") == 0))
	    {
	    	stProperties.textProperties.myProperties.verifyLineDB = TRUE;
	    }
	    else if ((stricmp (myString, "N") == 0) ||
	             (stricmp (myString, "F") == 0))
	    {
	    	stProperties.textProperties.myProperties.verifyLineDB = FALSE;
	    }
	    else
	    {
	    	JS_ErrorMessage (IDS_BAD_VERIFYDB_OPTION, myString);
	    }
	    JSCon_PropertiesSet (stProperties);
    	}
    	else
    	{
    	    JS_ErrorMessage (IDS_UNKNOWN_OPTION, myWord);
    	}

    	myWord = MyReadWordFromString (&myPtr);
    } // while
} // MyReadCommandLine


/************************************************************************/
/* MyReadFromPipe							*/
/************************************************************************/
static char	*MyReadFromPipe (void)
{
    char	*myBuffer;
    int		mySize = 0;
    int		myBytesRead;
    
    myBuffer = malloc (256);
    if (myBuffer == NULL)
    {
    	// TW Problem
    }
    
    while (TRUE)
    {
    	if (!ReadFile (stFromMainPipe, &myBuffer [mySize], 255, 
    		       &myBytesRead, NULL))
	{
	    // TW Problem
	}
	
	mySize += myBytesRead;
	
	if (myBuffer [mySize - 1] == 0)
	{
	    break;
	}
	
	myBuffer = realloc (myBuffer, mySize + 256);
	if (myBuffer == NULL)
	{
	    // TW Problem
	}
    }
    
    myBuffer [mySize] = 0;
    
    return myBuffer;
} // MyReadFromPipe


/************************************************************************/
/* MyReadWordFromString							*/
/************************************************************************/
static char	*MyReadWordFromString (char **pmPtr)
{
    char	*myResult;
    
    // Skip pver whitespace
    while (**pmPtr == ' ')
    {
    	(*pmPtr)++;
    }
    
    // If there's no more text, return NULL
    if (**pmPtr == 0)
    {
    	return NULL;
    }
    
    myResult = *pmPtr;

    // If we're at a ", read until the close quote
    if ((**pmPtr == '"') || (**pmPtr == (char) 0xFF))
    {
    	char	myEndChar = **pmPtr;
    	
    	(*pmPtr)++;
    	myResult = *pmPtr;
    	
    	// Skip until we reach close " or end of string
    	while ((**pmPtr != myEndChar) && (**pmPtr != 0))
    	{
    	    (*pmPtr)++;
    	}
    	
    	if (**pmPtr == 0)
    	{
    	    JS_ErrorMessage (IDS_MALFORMED_OPTION, stCommandLineOptions);
    	    return NULL;
	}
	
    	**pmPtr = 0;
    	(*pmPtr)++;
    }
    else
    {
    	while ((**pmPtr != ' ') && (**pmPtr != 0))
    	{
    	    (*pmPtr)++;
    	}
    
    	if (**pmPtr != 0)
    	{
    	    **pmPtr = 0;
    	    (*pmPtr)++;
    	}
    } // else - if
    
    return myResult;
} // MyReadWordFromString


/************************************************************************/
/* MySendException							*/
/************************************************************************/
static void	MySendException (void)
{
    HANDLE	myNotifyMainException;
    int		myBytesWritten;
        	    	
    // First, notify main that we're going to send a bunch of
    // stuff down the pipe.
    myNotifyMainException = OpenEvent (EVENT_ALL_ACCESS, FALSE, 
				   READY_NOTIFY_EXCEPTION_OUTPUT);
    if (myNotifyMainException == NULL)
    {
	JS_ErrorMessage (IDS_UNABLE_TO_OPEN_EXCEPTION_SIGNAL);
    }

    SetEvent (myNotifyMainException);
    CloseHandle (myNotifyMainException);

    // Send the number of characters    
    if ((!WriteFile (stToMainPipe, &stExceptionOutputLen, 4, 
         &myBytesWritten, NULL)) || (myBytesWritten != 4))
    {
	JS_ErrorMessage (IDS_UNABLE_TO_WRITE_EXCEPTION, GetLastError ());
    }
    
    // Now send the contents of the window
    if ((!WriteFile (stToMainPipe, stExceptionOutput, stExceptionOutputLen, 
        &myBytesWritten, NULL)) || (myBytesWritten != stExceptionOutputLen))
    {
	JS_ErrorMessage (IDS_UNABLE_TO_WRITE_EXCEPTION, GetLastError ());
    }
    
    free (stExceptionOutput);
} // MySendException

	
/************************************************************************/
/* MyStartJavaVM							*/
/************************************************************************/
static void	MyStartJavaVM (void)
{
    jint			myResult;
    int				myNumOptions = 0;
    JNI_CreateJavaVMCall	myJNI_CreateJavaVM;
    JavaVMInitArgs		myArguments;
    JavaVMOption		myOptions [10];
    int				cnt;
    
    //
    // Set the options for the JVM
    //
    
    //
    // Set the classpath option [-Djava.class.path=<classpath>]
    //
    myOptions [myNumOptions].optionString = 
    	malloc (strlen (CLASSPATH_VM_OPTION) + strlen (stClassPath) + 1);
    strcpy (myOptions [myNumOptions].optionString, CLASSPATH_VM_OPTION);
    strcat (myOptions [myNumOptions].optionString, stClassPath);
    myNumOptions++;
	
    //
    // Set the DLL library path option [-Djava.library.path=<librarypath>]
    //
    // Note that stApplicationDirectory always has a trailing '\'. 
    // Remove the trailing '\' if the directory is not 'X:\'
    //
    myOptions [myNumOptions].optionString = 
    	malloc (strlen (LIBRARY_PATH_VM_OPTION) + 
    	strlen (stApplicationDirectory) + 1);
    strcpy (myOptions [myNumOptions].optionString, LIBRARY_PATH_VM_OPTION);
    strcat (myOptions [myNumOptions].optionString, stApplicationDirectory);
    if (strlen (stApplicationDirectory) > 3)
    {
    	int	myLastChar = strlen (myOptions [myNumOptions].optionString) - 1;
    	myOptions [myNumOptions].optionString [myLastChar] = 0;
    }
    myNumOptions++;

    if (stRunningJRE13)
    {
	myOptions [myNumOptions].optionString = NO_JIT_12;
	myNumOptions++;
    }
    else
    {
	//
	// Set the enable assertions option	
	//
	myOptions [myNumOptions].optionString = ENABLE_ASSERTIONS_VM_OPTION;
	myNumOptions++;
	
	//
	// Set the debugging option
	//    
	myOptions [myNumOptions].optionString = DEBUG_VM_OPTION;
	myNumOptions++;
	
	//
	// Set the connection options.  Used to allow debugger JVM to attach
	// to this one.
	//
	myOptions [myNumOptions].optionString = CONNECTION_OPTION_VM_OPTION;
	myNumOptions++;
    }

    //
    // If necessary, set the maximum JVM size option
    if (stMaxJVMSize > 0)
    {
	myOptions [myNumOptions].optionString = malloc (20);
	wsprintf (myOptions [myNumOptions].optionString, 
	    MAX_JVM_SIZE_VM_OPTION, stMaxJVMSize);
	myNumOptions++;
    }	    

    // Specify the options for the execution
    for (cnt = 0 ; cnt < myNumOptions ; cnt++)
    {
    	JS_SendFailMessage ("JVM Option: \"%s\"", myOptions [cnt].optionString);
    }
    
    if (stRunningJRE13)
    {
	myArguments.version = JNI_VERSION_1_2;
    }
    else
    {
	myArguments.version = JNI_VERSION_1_4;
    }
    myArguments.options = myOptions;
    myArguments.nOptions = myNumOptions;
    myArguments.ignoreUnrecognized = JNI_TRUE;

    // Get the address of JNI_CreateJavaVM
    myJNI_CreateJavaVM = 
	(JNI_CreateJavaVMCall) MyFindLibraryAddress (stJVMDLLPath, 
	    						 "JNI_CreateJavaVM");
	    						 
    // Create the Java VM
    myResult = (myJNI_CreateJavaVM) (&stJVM, (void **) &stJNIEnv, 
				     &myArguments);
    
    if (myResult < 0)
    {
    	JS_ErrorMessage (IDS_JVM_CREATION_FAILED);
    }
    
    // Log the options
    JS_WriteLog ("[105] Options to VM");
    for (cnt = 0 ; cnt < myNumOptions ; cnt++)
    {
    	JS_WriteLog ("[106] %s", myOptions [cnt].optionString);
    }
} // MyStartJavaVM


/************************************************************************/
/* MyWaitForReadWrite							*/
/************************************************************************/
static int	MyWaitForReadWrite (LPVOID pmConsoleWindow)
{
    // Events to synchronize reading of input
    // Event is set when the Java program needs a line of input from the user
    HANDLE	myNeedALineEvent = NULL;
    // Event is set when the Java program has a line of output to be displayed
    HANDLE	myHaveALineEvent = NULL;
    char	myBuffer [KEYBOARD_BUFFER_SIZE];
    HANDLE	myHandles [2];
    BOOL	myShowConsole;
    int		myResult;

    // Create the signals
    myNeedALineEvent = CreateEvent (NULL, TRUE, FALSE, "JavaSupNeedALine");
    myHaveALineEvent = CreateEvent (NULL, TRUE, FALSE, "JavaSupHaveALine");

    myHandles [0] = myNeedALineEvent;
    myHandles [1] = myHaveALineEvent;
    
    while (TRUE)
    {
    	myResult = WaitForMultipleObjects (2, myHandles, FALSE, INFINITE);
    	
    	myShowConsole = (!stConsoleVisible) &&
    	    (((myResult == READ_LINE_FROM_KBD) && 
    	     (!stInputFromFile || 
    	      (stInputFromFile && stEchoInput &&
    	       (!stOutputToFile || stScreenAndFile)))) ||
    	     ((myResult == WRITE_LINE_TO_SCRN) && 
    	      (!stOutputToFile || stScreenAndFile)));

	// Open up the input/output window    	       
    	if (myShowConsole)
    	{
	    MyOpenReadyConsole ();
	    stConsoleVisible = TRUE;
    	}
    	
    	if (myResult == READ_LINE_FROM_KBD)
    	{
    	    // Set the read a line event to unsignaled
    	    ResetEvent (myNeedALineEvent);
    	    
	    if (stInputFromFile)
	    {
	    	//
	    	// Read input from a file
	    	//
	    	
	    	// Check for EOF, echoing it if we haven't before
	    	if (stEOFInStdin || feof (stInputFile))
	    	{
	    	    // We reached EOF, return an empty string which signals EOF
	    	    myBuffer [0] = 0;
	            
	            stEOFInStdin = TRUE;
	            
	            if (stEchoInput && !stEOFEchoed)
	            {
			stEOFEchoed = TRUE;
	            	MyWriteLine (SCREEN_AND_FILE, "^Z\n");
	            }
	    	}
	    	else
	    	{	
	    	    // Read a line from the file
    	    	    fgets (myBuffer, KEYBOARD_BUFFER_SIZE - 1, stInputFile);
    	    	    
    	    	    // An error occurred, create an exception
    	    	    if (ferror (stInputFile))
    	    	    {
    	    	    	jclass myException;
    	    	    	
    	    	    	myException = (*stJNIEnv) -> FindClass (stJNIEnv, 
    	    	    					"java/io/IOException");
    	    	    	if (myException != NULL)
    	    	    	{
    	    	    	    (*stJNIEnv) -> ThrowNew (stJNIEnv, myException, 
    	    	    	    			     _sys_errlist [errno]);
			    (*stJNIEnv) -> DeleteLocalRef (stJNIEnv, 
			    				   myException);
    	    	    	}
    	    	    	myBuffer [0] = 0;
    	    	    }
    	    	    else
    	    	    {	
    	    	    	// Add a newline if necessary and echo it to the 
    	    	    	// screen and file if input echoed
    	    	    	if (myBuffer [strlen (myBuffer) - 1] != '\n')
    	    	    	{
    	    	    	    strcat (myBuffer, "\n");
    	    	    	}
	            	if (stEchoInput)
	            	{
	            	    MyWriteLine (SCREEN_AND_FILE, myBuffer);
	            	}
	            }
	        }
	    }
	    else 
    	    {
    	    	//
    	    	// Read input from the keyboard
    	    	//
    	    	
    	    	MyChangeWindowTitle (STATUS_WAITING_FOR_INP);
    	    	stWaitingForInput = TRUE;

		//
		// Read input from standard input/output window
		//
		JSCon_GetLineOfInput (stConsoleWindow, myBuffer, 
		  		      KEYBOARD_BUFFER_SIZE - 1);
	
		if (!stJavaProgramPaused)
		{
		    MyChangeWindowTitle (STATUS_RUNNING);
		}
		
    	    	stWaitingForInput = FALSE;
    	    	if ((strlen (myBuffer) > 0) &&
    	    	    (myBuffer [strlen (myBuffer) - 1] != '\n'))
    	    	{
    	    	    strcat (myBuffer, "\n");
    	    	}
    	    	if (stEchoInput)
	        {
	            MyWriteLine (FILE_ONLY, myBuffer);
	        }
	    }
	    DLL_SetLineToRead (myBuffer);    	
	}
	else if (myResult == WRITE_LINE_TO_SCRN)
	{
	    char	*myLine;
    	    // Set the have a line event to unsignaled
    	    ResetEvent (myHaveALineEvent);
    	    
	    myLine = DLL_GetWrittenLine ();
	    
	    MyWriteLine (SCREEN_AND_FILE, myLine);
	    if (GlobalFree (myLine) != NULL)
	    {
	    	JS_ErrorMessage (IDS_CANT_FREE_MEMORY);
	    }
	}
    }
    
    return 0;
} // MyWaitForReadWrite


/************************************************************************/
/* MyWaitForSignalFromMain						*/
/************************************************************************/
static int	MyWaitForSignalFromMain (LPVOID pmConsoleWindow)
{
    // Events to synchronize reading of input
    // Event is set when the Ready wants to pause Java program
    HANDLE	myPauseJavaProgramEvent = NULL;
    // Event is set when the Ready wants to resume Java program
    HANDLE	myResumeJavaProgramEvent = NULL;
    // Event is set when the Ready wants to terminate Java Supervisor
    HANDLE	myTerminateJavaSupEvent = NULL;
    HANDLE	myHandles [4];
    int		myResult;

    // Create the signals
    myPauseJavaProgramEvent = OpenEvent (EVENT_ALL_ACCESS, FALSE, 
				         READY_MAIN_SAYS_PAUSE);
    myResumeJavaProgramEvent = OpenEvent (EVENT_ALL_ACCESS, FALSE, 
				          READY_MAIN_SAYS_RESUME);
    myTerminateJavaSupEvent = OpenEvent (EVENT_ALL_ACCESS, FALSE, 
				         READY_MAIN_SAYS_TERMINATE);

    if ((myPauseJavaProgramEvent == NULL) ||
	(myResumeJavaProgramEvent == NULL) ||
	(myTerminateJavaSupEvent == NULL))
    {
	return (0);
    }

    myHandles [0] = myPauseJavaProgramEvent;
    myHandles [1] = myResumeJavaProgramEvent;
    myHandles [2] = myTerminateJavaSupEvent;
    myHandles [3] = stReadyProcess;
    
    while (TRUE)
    {
    	myResult = WaitForMultipleObjects (4, myHandles, FALSE, INFINITE);

    	if (myResult == PAUSE_JAVA_PROGRAM)
    	{
    	    ResetEvent (myPauseJavaProgramEvent);

	    //    	    
    	    // Set the window header and flags.
    	    //
    	    
	    // If we are already paused, then return immediately
	    if (!stJavaProgramPaused && !stJavaProgramFinished)
	    {
		// If we are in Java 1.4, then the program has been paused by
		// calling the debugger in ready.exe.  In Java 1.3, we must
		// pause the program ourselves.
		if (stRunningJRE13)
		{
		    HANDLE	myInvokePauseJavaThread;
		    DWORD	myThreadID;
		    
		    // Starts a thread that invokes the pauseJavaThreads method
		    myInvokePauseJavaThread = CreateThread (NULL, 0, 
			(LPTHREAD_START_ROUTINE) MyInvokePauseJava, NULL, 0, &myThreadID);
		    if (myInvokePauseJavaThread == NULL)
		    {
			JS_ErrorMessage (IDS_UNABLE_TO_CREATE_THREAD);
		    }    		       
		    
		    // Wait while System.exit is called and the thread deleted
		    WaitForSingleObject (myInvokePauseJavaThread, INFINITE);
		    
		    // Close the handle
		    CloseHandle (myInvokePauseJavaThread);
		}

	    	stJavaProgramPaused = TRUE;
	    	
	    	// Change the Window title to "Paused" if applicable
	    	MyChangeWindowTitle (STATUS_PAUSED);
	    }
    	}
    	else if (myResult == RESUME_JAVA_PROGRAM)
    	{
    	    ResetEvent (myResumeJavaProgramEvent);

	    //    	    
    	    // Set the window header and flags.
    	    //
    	    
	    // If we are not paused, then return immediately
	    if (stJavaProgramPaused && !stJavaProgramFinished)
	    {
		// If we are in Java 1.4, then the program has been paused by
		// calling the debugger in ready.exe.  In Java 1.3, we must
		// pause the program ourselves.
		if (stRunningJRE13)
		{
		    HANDLE	myInvokeResumeJavaThread;
		    DWORD	myThreadID;
		    
		    // Starts a thread that invokes the resumeJavaThreads method
		    myInvokeResumeJavaThread = CreateThread (NULL, 0, 
			(LPTHREAD_START_ROUTINE) MyInvokeResumeJava, NULL, 0, &myThreadID);
		    if (myInvokeResumeJavaThread == NULL)
		    {
			JS_ErrorMessage (IDS_UNABLE_TO_CREATE_THREAD);
		    }    		       
		    
		    // Wait while System.exit is called and the thread deleted
		    WaitForSingleObject (myInvokeResumeJavaThread, INFINITE);
		    
		    // Close the handle
		    CloseHandle (myInvokeResumeJavaThread);
		}

	    	stJavaProgramPaused = FALSE;
    
	    	// Change the Window title to "Running" if applicable
	    	if (stWaitingForInput)
	    	{
	    	    MyChangeWindowTitle (STATUS_WAITING_FOR_INP);
	    	}
	    	else
	    	{
	    	    MyChangeWindowTitle (STATUS_RUNNING);
	    	}
	    }
    	}
    	else if (myResult == TERMINATE_JAVA_SUP)
    	{
	    ResetEvent (myTerminateJavaSupEvent);    	    
    	    JS_TerminateProcess ();
    	}
    	else if (myResult == READY_TERMINATED)
    	{
    	    JS_TerminateProcess ();
    	}
    } // while
} // MyWaitForSignalFromMain


/************************************************************************/
/* MyWriteLine								*/
/************************************************************************/
static void	MyWriteLine (int pmToWhere, char *pmString)
{
    // If we're intercepting the text of an exception, add it to the buffer.
    if (stInterceptException)
    {
    	// Add it to the exception text
    	int	myLength = strlen (pmString);
    	    
    	if (myLength + stExceptionOutputLen > stExceptionOutputBufferSize)
    	{
    	    stExceptionOutputBufferSize = 
    	    				myLength + stExceptionOutputLen + 10000;
    	    stExceptionOutput = realloc (stExceptionOutput, 
    	    				   stExceptionOutputBufferSize);
	}
	strcpy (&stExceptionOutput [stExceptionOutputLen], pmString);
	stExceptionOutputLen += myLength;
    }
    
    if (((pmToWhere == SCREEN_AND_FILE) || (pmToWhere == SCREEN_ONLY)) &&
    	(!stOutputToFile || stScreenAndFile))
    {
	// Write string to Ready stdin/out window
	char	*myPassedString;
	    
    	myPassedString = malloc (strlen (pmString) + 1);
    	// TW Check for NULL
    	strcpy (myPassedString, pmString);
    	    
    	SendMessage (stConsoleWindow, WM_ADD_TEXT, 
    	 	     (WPARAM) myPassedString, (LPARAM) FREE_BUFFER);
    }

    if (((pmToWhere == SCREEN_AND_FILE) || (pmToWhere == FILE_ONLY)) &&
        stOutputToFile)
    {
    	// Write to the file
    	if (stOutputFileIsPrinter)
    	{
    	    // Add it to the output going to the printer
    	    int	myLength = strlen (pmString);
    	    
    	    if (myLength + stPrinterOutputLen > stPrinterOutputBufferSize)
    	    {
    	    	stPrinterOutputBufferSize = 
    	    				myLength + stPrinterOutputLen + 10000;
    	    	stPrinterOutput = realloc (stPrinterOutput, 
    	    				   stPrinterOutputBufferSize);
	    }
	    strcpy (&stPrinterOutput [stPrinterOutputLen], pmString);
	    stPrinterOutputLen += myLength;
	}
	else
	{
    	    fputs (pmString, stOutputFile);
    	    fflush (stOutputFile);
    	}
    }
} // MyWriteLine

