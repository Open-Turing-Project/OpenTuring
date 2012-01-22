/***********/
/* edrun.c */
/***********/

/*******************/
/* System includes */
/*******************/
#include "stdio.h"
/****************/
/* Self include */
/****************/
#include "edrun.h"

/******************/
/* Other includes */
/******************/
#include "ed.h"
#include "eddisp.h"
#include "ederr.h"
#include "edgui.h"
#include "edprop.h"

#include "language.h"

#include "edint.h"

#include "mio.h"

#include "eddialogs.h"

#include "tprolog.h"

#include "hashmap.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
#define	INTERP_NUM			0
#define GRAPHICS_NUM			1
#define WINTURING_NUM			2
#define WINDOWS_NUM			3
#define OPENTURING_NUM			4

#define NUM_DEFAULT_PREDEFS		5

#define INTERP_STRING			"_INTERP_"
#define GRAPHICS_STRING			"_GRAPHICS_"
#define WINTURING_STRING		"_WINOOT_"
#define WINDOWS_STRING			"_WINDOWS_"
#define OPENTURING_STRING		"_OPENTURING_"

#define FLOATING_MAIN			0
#define NULL_OOT_VALUE			0

#define PREPARING_EXECUTION		1
#define STARTING_EXECUTION		2
#define FINISHED_EXECUTION		3

#define FORCE_DISPLAY			77
#define DISPLAY_IF_VISIBLE		78

#define UNIT_BREAK			-999

// Just a random number used to identify this timer.
#define TURING_TIMER			512

#define MINIMUM_DELAY			  1
#define MSECS_BETWEEN_EVENT_CHECK	100

#define COPY_BUFFER_SIZE		32768

#define SYSEXIT_ERROR_STRING		"SysExit"

/********************/
/* Global variables */
/********************/

/*********/
/* Types */
/*********/
typedef struct SrcPosition		SrcPosition;
typedef struct Language_ErrMsgs		*TuringErrorPtr;
typedef struct Language_RunStatus	RunStatus;
typedef struct Breakpoint
{
    FileNoType	fileNo;
    int		lineNo;
} Breakpoint;


typedef struct MatchWindow
{
    WORD	fileNo;
    const char	*pathName;
    TextPtr	textPtr;
    HWND	textWindow;
} MatchWindow;

typedef struct RunArgs
{
    char	commandLineArguments [4096];
    OOTargs	ootArgs;
    int		numArgs;
    int		inputRedirection;
    int		outputRedirection;
    FilePath	inputFile, outputFile;
} RunArgs;
    
/**********************/
/* External variables */
/**********************/
BOOL	 gFileManagerIgnoreFileOperation = FALSE;

/********************/
/* Static constants */
/********************/

/********************/
/* Static variables */
/********************/
static FilePath		stPredefineListPathName;

static TuringErrorPtr	stFirstErrorPtr;

// Redirection Arguments
static RunArgs		stRunArgs;

// The file number of the fixed main program
static FileNoType	stMainProgramFileNo;
// The file number of the last program compiled
static FileNoType	stSourceFileNo;
// The display name of the program the user compiled/executed
static FilePath		stSourceFileName;
// The path name of the program the user compiled/executed
static FilePath		stSourcePathName;

static RunStatus	stRunStatus;
// Are we currently running a Turing program (paused or otherwise)
static BOOL		stTuringProgramRunning = FALSE;
static BOOL		stTuringProgramHalting = FALSE;
static BOOL		stRerunningTuringProgram = FALSE;
static BOOL		stQuittingEnvironment = FALSE;
// Set if we are currently waiting for the user to give a command to continue
// execution (i.e. Resume, Step, etc.).
static BOOL		stTuringProgramPaused = FALSE;
static BOOL		stTuringProgramWaitingForEvent = FALSE;
static int		stDelayWait = 0;

// Flags used for stepping and tracing
static BOOL		stTracing = FALSE;
// This flag is set true when a step command has been given.
static int		stStepping = NO_STEP;
static BOOL		stBreakpointsVisible = FALSE;
static FileNoType	stFileNoLastExecuted;
static int		stTraceSpeed = 33, stTraceDelayTimer;

static UINT		stMinimumPeriod, stMaximumPeriod, stMinimumEventPeriod;

// Boolean flags indicating whether the program is running/paused
// TW
static char		*stFileBeingCompiled;
static HWND		stCompilingWindow;
static HWND		stExecutingWindow;
static BOOL		stAllRunWindowsClosed = FALSE;


/******************************/
/* Static callback procedures */
/******************************/
static void CALLBACK 	MyExecutionTimeoutTimerProcedure (
				UINT pmID, UINT pmMsg, DWORD pmWindow,
				DWORD pmDummy1, DWORD pmDummy2);
static UINT CALLBACK 	MyRunWithArgsDialogProcedure (HWND pmWindow, 
						      UINT pmMessage, 
    						      WPARAM pmWParam, 
    						      LPARAM pmLParam);
static void CALLBACK 	MyTimerProcedure (UINT pmID, UINT pmMsg, DWORD pmUser,
					  DWORD pmDummy1, DWORD pmDummy2);

/*********************/
/* Static procedures */
/*********************/
static void	MyExecuteQuantum (OOTint *pmNumErrors, HWND pmWindow);
static int	MyGetMaxStackSize (void);
static void	MyMatchPathName (char *pmPathName, HWND pmEditWindow, 
		    		 HWND pmTextDisplayWindow, TextPtr pmTextPtr, 
				 void *pmMatch);
static void	MyRunTestSuiteProgram (const char *pmTestDirectory, 
				       const char *pmOutputDirectory,
				       const char *pmTestFileName,
				       int pmLineNo, char *pmIndexPath);
static void	MySetPreprocessorSymbols (void);
static void	MySetTuringPointers (const char *pmPathName, HWND pmEditWindow,
				 HWND pmTextDisplayWindow, TextPtr pmTextPtr,
				 void *pmDummy);
static void	MyStatusExecuting (const char *pmClassName, int pmStage);
static int	MyWriteByteCode (FileNoType pmProgramFileNo, long pmTuringFileDesciptor, 
			 BOOL pmCloseWindowsOnTerminate, 
			 BOOL pmDisplayRunWithArgs, BOOL pmCenterOutputWindow, 
			 BOOL pmStopUserClose);

/***********************/
/* External procedures */
/***********************/
extern void	TL (void);
// Needed because language.h didn't anticipate the need to initialize
// execution without a recompilation.
extern void	Language_ErrorModule_Initialize (void);
extern void	Language_DebugModule_Initialize (void);
extern void	Language_Execute_RecoverAllMemory (void);

extern int	MIOTime_GetTicks (void);
extern char	Language_Execute_externalExecutionTimeoutFlag;
extern TLint1	Language_Execute_fileMap[];

/************************************************************************/
/* EdRun_Init								*/
/************************************************************************/
BOOL	EdRun_Init (void)
{
    FilePath	myOOTPathName, myHomePathName;
    TIMECAPS	myTimeCaps;
        
    //
    // Initialize Turing
    //
    TL ();
    FileManager ();
    Language ();
    
    MySetPreprocessorSymbols ();
    
    // Check to make certain predef list exists    
    EdFile_GetApplicationDirectory (stPredefineListPathName);
    strcat (stPredefineListPathName, "Support\\Predefs\\Predefs.lst");
    if (!EdFile_FileExists (stPredefineListPathName))
    {
    	EdFail_Warn (IDS_CANTFINDPREDEFS, __FILE__, __LINE__, GetLastError (),
    		     stPredefineListPathName);
    	return FALSE;
    }
    
    // Set default include directory
    EdFile_GetApplicationDirectory (myOOTPathName);
    FileManager_SetDefaultInclude (myOOTPathName);
    
    // Set the home directory
    strcpy (myHomePathName, EdProp_GetStartupDirectory ());
    FileManager_SetHomeDir (myHomePathName);

    //
    // Initialize Run with Argument Structure
    //
    stRunArgs.commandLineArguments [0] = 0;
    stRunArgs.numArgs = 0;
    stRunArgs.inputRedirection = ARGS_IN_KEY;
    stRunArgs.outputRedirection = ARGS_OUT_SCREEN;
    stRunArgs.inputFile [0] = 0;
    stRunArgs.outputFile [0] = 0;
    
    //
    // Initialize the main program
    //
    stMainProgramFileNo = FLOATING_MAIN;
    
    // Initialize the MIO module
    MIO_Initialize (gProgram.applicationInstance, OS_WINDOWS, myOOTPathName,
    		    myHomePathName, FALSE, FALSE, SYSEXIT_ERROR_STRING);

    //
    // Initialize Windows high resolution timer
    //
    if (timeGetDevCaps (&myTimeCaps, sizeof(TIMECAPS)) != TIMERR_NOERROR) 
    {
    	EdFail_Warn (IDS_CANTFINDHIRESTIMER, __FILE__, __LINE__, 
    		     GetLastError ());
    	return FALSE;
    }
    stMinimumPeriod = myTimeCaps.wPeriodMin;
    stMinimumEventPeriod = max (MSECS_BETWEEN_EVENT_CHECK / 4, 
    				stMinimumPeriod);
    stMaximumPeriod = myTimeCaps.wPeriodMax;
    
    timeBeginPeriod (stMinimumPeriod);
    
    return TRUE;
} // EdRun_Init


/************************************************************************/
/* EdRun_Finalize							*/
/************************************************************************/
void	EdRun_Finalize (void)
{
    MIO_Finalize ();
    
    if (stFileBeingCompiled != NULL)
    {
    	free (stFileBeingCompiled);
    }
    
    timeEndPeriod (stMinimumPeriod);
} // EdRun_Finalize


/************************************************************************/
/* EdRun_PropertiesSet							*/
/************************************************************************/
void	EdRun_PropertiesSet (void)
{
} // EdRun_PropertiesSet


/************************************************************************/
/* EdRun_PropertiesImplementChanges					*/
/************************************************************************/
void	EdRun_PropertiesImplementChanges (void)
{
} // EdRun_PropertiesImplementChanges


/************************************************************************/
/* EdRun_CloseAllRunWindows						*/
/************************************************************************/
void	EdRun_CloseAllRunWindows (HWND pmEditWindow)
{
    MIO_CloseAllRunWindows ();
    EdWin_ShowStatus (pmEditWindow, "All Run Windows Closed");

} // EdRun_CloseAllRunWindows


/************************************************************************/
/* EdRun_Compile							*/
/************************************************************************/
BOOL	EdRun_Compile (HWND pmTextDisplayWindow, const char *dummy, 
		       BOOL pmCompileOnly)
{
    TuringErrorPtr	myError;   
	OOTint myErrors;
    int			myMessages, myWarnings, myNumFiles;
    FilePath		mySourceDirectory;

    //
    // Check that the window hasn't been closed since we've tried to run
    // with it.
    //
    if (!IsWindow (pmTextDisplayWindow))
    {
    	// TW Put a real error message here.
    	MessageBeep (MB_OK);
    	return TRUE;
    }
    
    //
    // If we get the message to compile this program while we're still running,
    // we want to halt the program and then run again.  There are three
    // possibilities:
    //
    // (1) There is a fixed main window - if running, halt the program and
    //     run the fixed main window
    // (2) The current program is a run window - if running, halt the program
    //     and run the previously run program again
    // (3) The current program is a program window - if running halt the 
    //     program and run the program in the run window
    //
    if (stMainProgramFileNo == FLOATING_MAIN)
    {
    	if (pmTextDisplayWindow != NULL)
    	{
    	    // The user pressed F1 when a run window was displayed.  Use the
    	    // last program compiled.
    	    stSourceFileNo = EdDisp_GetTuringFileNo (pmTextDisplayWindow);
    	    EdDisp_GetPathForDisplay (pmTextDisplayWindow, USE_FILENAME, 
	    	    			      DONT_SHOW_DIRTY_MARK, stSourceFileName);
    	    if (EdDisp_GetPath (pmTextDisplayWindow) == NULL)
    	    {
    	    	// We need to make this a full path name
    	    	EdFile_GetActiveDirectory (stSourcePathName);
    	    	if (!EdGlob_EndsWith (stSourcePathName, "\\"))
    	    	{
    	    	    strcat (stSourcePathName, "\\");
    	    	}
    	    	strcat (stSourcePathName, stSourceFileName);
    	    }
    	    else
    	    {
    	    	strcpy (stSourcePathName, EdDisp_GetPath (pmTextDisplayWindow));
    	    }
    	}
    	else
    	{
    	    FileManager_FileName (stSourceFileNo, stSourcePathName);
    	    strcpy (stSourceFileName, EdFile_GetFileName (stSourcePathName));
    	}
    }
    else
    {
    	stSourceFileNo = stMainProgramFileNo;
    	FileManager_FileName (stSourceFileNo, stSourcePathName);
    	strcpy (stSourceFileName, EdFile_GetFileName (stSourcePathName));
    }
    strncpy (stRunArgs.ootArgs [0], stSourcePathName, 255);    	    			     
    
    // Set the base source directory to be the directory 
    // in which the source file was located.
    EdFile_GetDirectoryFromPath (stSourcePathName, mySourceDirectory);
    FileManager_ChangeDirectory ((OOTstring) mySourceDirectory);
    
    //
    // Now, for every open file, set the OOT file manager pointer to point to
    // it and mark it modified
    //
    
    Ed_EnumerateFiles (MySetTuringPointers, NULL);

    stCompilingWindow = pmTextDisplayWindow;
    
    // Clear all previous errors
    EdErr_ClearErrors ();

	// dump bytecode if debugging (Shift + Ctrl at launch)
	Language_Debugging(debugModeEnabled);

    // Free any previously allocated memory.  This could take a while if 
    // someone allocated a huge amount
    EdRun_InitializeForRunWithoutCompile (pmTextDisplayWindow);
    
    // Close any run windows and free any allocated objects in debugger		      
    MIO_Init_Free ();
    
    // Compile the program
    EdWin_ShowStatus (pmTextDisplayWindow, "Initializing Compiler");
    Language_CompileProgram ("", stSourceFileNo, &myError, &myErrors);

    myMessages = myWarnings = myNumFiles = 0;
    
    while (myError != NULL)
    {
    	WORD		myErrorTuringFileNo;
    	FilePath	myErrorPathName;
    	SrcPosition	*mySrc;
    	
    	myErrorTuringFileNo = myError -> srcPos.fileNo;
    	FileManager_FileName (myErrorTuringFileNo, myErrorPathName);
    	mySrc = &(myError -> srcPos);
        if (EdErr_AddError (myErrorPathName, myErrorTuringFileNo,
        		    mySrc -> lineNo, mySrc -> linePos + 1, 
        		    mySrc -> lineNo, mySrc -> linePos + mySrc -> tokLen, 
		      	    myError -> text))
	{
	    myNumFiles++;
	}
	myMessages++;
	myError = myError -> next;
    }

    myWarnings = myMessages - myErrors;
        
    return EdErr_DisplayNumberOfErrorsDialog (pmTextDisplayWindow, 
    					      stSourceFileName, myErrors,
    				              myWarnings, myNumFiles,
    				              pmCompileOnly);
} // EdRun_Compile

/************************************************************************/
/* EdRun_CreateByteCodeFile							*/
/* Creates a bytecode file to be read by the prolog with the -file option. */
/* Returns an error code */
/************************************************************************/
int	EdRun_CreateByteCodeFile (FilePath pmProgramPath,FilePath pmOutputPath)
{
    long		myTuringFileDesciptor;
	FileNoType	myProgramFileNumber;

	TextHandleType	myDummyTuringTextHandle;
    SizePtrType		myDummyTuringSizePtr;
    ResultCodeType	myResult;

    OOTint			myStatus = -1;
	FilePath mySourceDirectory;

	TuringErrorPtr	myError;   
	OOTint myErrors;

	// Make certain the test file exists
    if (!EdFile_FileExists (pmProgramPath))
    {
	// Test file not found
	EdGUI_Message1 (NULL, MB_ICONEXCLAMATION, 
	    IDS_TEST_SUITE_ERROR_TITLE, 
	    IDS_TEST_SUITE_FILE_DOESNT_EXIST, 0,
		"<none>", pmProgramPath);
	return 1;
    }

	FileManager_OpenNamedHandle (pmProgramPath, &myProgramFileNumber, 
    				     &myDummyTuringTextHandle, &myDummyTuringSizePtr,
    				     &myResult);
    
    // Set the base source directory to be the directory 
    // in which the source file was located.
    EdFile_GetDirectoryFromPath (pmProgramPath, mySourceDirectory);
    FileManager_ChangeDirectory ((OOTstring) mySourceDirectory);

    // Compile the program
    Language_CompileProgram ("", myProgramFileNumber, &myError, &myErrors);

	// Open the executable file
    TL_TLI_TLIOF (16, pmOutputPath, &myTuringFileDesciptor);
    
    if (myError != NULL)
    {
		int		myMessages = 0;
		char	msgBuffer[1024];
		
		// error header
		TL_TLI_TLIWR (OBJECT_FILE_ERROR_HEADER, sizeof (OBJECT_FILE_ERROR_HEADER), &myStatus,
    		  myTuringFileDesciptor);
		if (myStatus != 0)
		{
    		return 1;
		}

		while (myError != NULL)
		{
			WORD	myErrorTuringFileNo;
			FilePath	myErrorPathName;
			SrcPosition	*mySrc;
	    
			myErrorTuringFileNo = myError -> srcPos.fileNo;
			FileManager_FileName (myErrorTuringFileNo, myErrorPathName);
			mySrc = &(myError -> srcPos);
	    
			if (mySrc -> tokLen > 0)
			{
				sprintf (msgBuffer,"\r\nLine %d [%d - %d] of (%s): %s",
					mySrc -> lineNo, mySrc -> linePos + 1,
					mySrc -> linePos + 1 + mySrc -> tokLen, 
					myErrorPathName, myError -> text);
			}
			else
			{
				sprintf (msgBuffer, 
					"\r\nLine %d [%d] of (%s): %s",
					mySrc -> lineNo, mySrc -> linePos + 1,
					myErrorPathName, myError -> text);
			}

			TL_TLI_TLIWR (msgBuffer, strlen(msgBuffer), &myStatus,
    		  myTuringFileDesciptor);
			if (myStatus != 0)
			{
    			return 1;
			}

			myError = myError -> next;
			myMessages++;
		}

		// close the file before returning
		TL_TLI_TLICL (myTuringFileDesciptor);

		return myMessages;
	}

    
    if (myTuringFileDesciptor <= 0)
    {
    	EdGUI_Message1 (NULL, 0, IDS_CREATE_EXE_FAILED_TITLE,
    	    	        IDS_CANT_CREATE_EXE, pmProgramPath);
        return 1;
    }

    myStatus = MyWriteByteCode(myProgramFileNumber,myTuringFileDesciptor,FALSE,FALSE,FALSE,FALSE);

	if (myStatus != 0)
    {
    	EdGUI_Message1 (NULL, 0, IDS_CREATE_EXE_FAILED_TITLE,
    	    	        IDS_UNABLE_TO_WRITE_EXE, pmProgramPath, 4);
        return 1;
    }
    
    TL_TLI_TLICL (myTuringFileDesciptor);

	return 0; // no errors
} // EdRun_CreateByteCodeFile


/************************************************************************/
/* EdRun_CreateEXE							*/
/************************************************************************/
void	EdRun_CreateEXE (HWND pmTextDisplayWindow, FilePath pmPathName, 
			 BOOL pmCloseWindowsOnTerminate, 
			 BOOL pmDisplayRunWithArgs, BOOL pmCenterOutputWindow, 
			 BOOL pmStopUserClose)
{
    long		myTuringFileDesciptor;
    int			myStatus = -1;
    char		myBuffer [COPY_BUFFER_SIZE];
    int			myBytesRead, myBytesWritten;
    HANDLE		myPrologHandle, myEXEFileHandle;
    FilePath		myPrologPath;
    int			myNoErrors;
    ErrorBuffer		myErrorBuffer;
    // Used for setting the offset
    int			myPrologSize = 0;
    char		myChar;
    int			myOffsetInOffset = 0;
    char		*myOffsetString = OFFSET_STRING;
    int			myStartOfOffset, myPositionInFile = 0;
    
    // Make certain that this program will compile
    myNoErrors = EdRun_Compile (pmTextDisplayWindow, NULL, TRUE);
    if (!myNoErrors)
    {
        EdWin_ShowStatus (pmTextDisplayWindow, "Create EXE cancelled");
        return;
    }

    // Open the executable file
    TL_TLI_TLIOF (16, pmPathName, &myTuringFileDesciptor);
    
    if (myTuringFileDesciptor <= 0)
    {
    	EdGUI_Message1 (pmTextDisplayWindow, 0, IDS_CREATE_EXE_FAILED_TITLE,
    	    	        IDS_CANT_CREATE_EXE, pmPathName);
        EdWin_ShowStatus (pmTextDisplayWindow, "Create EXE cancelled");
        return;
    }
      
    //  	
    // Open the prolog
    //
    EdFile_ConstructPath (myPrologPath, IDS_READY_BIN, IDS_TURING_EXE_PROLOG);
    
    myPrologHandle = CreateFile (myPrologPath, GENERIC_READ, FILE_SHARE_READ, 
        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
    	NULL);
    if (myPrologHandle == (HANDLE) INVALID_HANDLE_VALUE)
    {
	int	myErrorCode = GetLastError ();
	
    	if (myErrorCode == ERROR_FILE_NOT_FOUND)
    	{
    	    EdGUI_Message1 (pmTextDisplayWindow, 0, IDS_CREATE_EXE_FAILED_TITLE,
    	    		    IDS_CANT_FIND_TPROLOG, myPrologPath);
    	}
    	else
    	{
    	    EdGUI_Message1 (pmTextDisplayWindow, 0, IDS_CREATE_EXE_FAILED_TITLE,
    	                    IDS_FILE_OPEN_FAILED_MESSAGE, myPrologPath, 
    	            	    EdFail_GetErrorMessage (myErrorBuffer), 
    	            	    myErrorCode);
    	}
    	TL_TLI_TLICL (myTuringFileDesciptor);
    	return;
    }

    //    
    // Copy prolog to the user file
    //
    EdWin_ShowStatus (pmTextDisplayWindow, "Copying EXE prolog to %s", 
		      EdFile_GetFileName (pmPathName));
    while (TRUE)
    {
    	// Read from the prolog file
        if (ReadFile (myPrologHandle, myBuffer, COPY_BUFFER_SIZE, 
        	      &myBytesRead, NULL) == 0)
        {
    	    EdGUI_Message1 (pmTextDisplayWindow, 0, IDS_CREATE_EXE_FAILED_TITLE,
    	        IDS_FILE_READ_FAILED, myPrologPath, 
    	        EdFail_GetErrorMessage (myErrorBuffer), GetLastError ());
	    CloseHandle (myPrologHandle);
	    return;
        }
        
        if (myBytesRead == 0) break;
        myPrologSize += myBytesRead;
        
        // Write to the user executable
    	TL_TLI_TLIWR (myBuffer, myBytesRead, &myStatus, myTuringFileDesciptor);
    	if (myStatus != 0)
    	{
    	    EdGUI_Message1 (pmTextDisplayWindow, 0, IDS_CREATE_EXE_FAILED_TITLE,
    	    	            IDS_UNABLE_TO_WRITE_EXE, pmPathName, 4);
            EdWin_ShowStatus (pmTextDisplayWindow, "Create EXE cancelled");
            return;
        }
    }

    CloseHandle (myPrologHandle);

    myStatus = MyWriteByteCode(stSourceFileNo,myTuringFileDesciptor,pmCloseWindowsOnTerminate,pmDisplayRunWithArgs,pmCenterOutputWindow,pmStopUserClose);
	if (myStatus != 0)
    {
    	EdGUI_Message1 (pmTextDisplayWindow, 0, IDS_CREATE_EXE_FAILED_TITLE,
    	    	        IDS_UNABLE_TO_WRITE_EXE, pmPathName, 4);
        EdWin_ShowStatus (pmTextDisplayWindow, "Create EXE cancelled");
        return;
    }
    
    TL_TLI_TLICL (myTuringFileDesciptor);
    
    //
    // Now open the file produced and set the offset
    //
    EdWin_ShowStatus (pmTextDisplayWindow, "Setting file size offset in %s", 
		      EdFile_GetFileName (pmPathName));

    myEXEFileHandle = CreateFile (pmPathName, GENERIC_READ | GENERIC_WRITE, 0,
        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
    	NULL);
    if (myEXEFileHandle == (HANDLE) INVALID_HANDLE_VALUE)
    {
    	EdGUI_Message1 (pmTextDisplayWindow, 0, IDS_CREATE_EXE_FAILED_TITLE,
    	                IDS_CANT_REOPEN_EXE, myPrologPath, 
    	            	EdFail_GetErrorMessage (myErrorBuffer), 
    	            	GetLastError ());
        EdWin_ShowStatus (pmTextDisplayWindow, "Create EXE cancelled");
    	return;
    }

    //    
    // Copy prolog to the user file
    //
    while (TRUE)
    {
    	// Read from the EXE file
        if (ReadFile (myEXEFileHandle, &myChar, 1, &myBytesRead, NULL) == 0)
        {
    	    EdGUI_Message1 (pmTextDisplayWindow, 0, IDS_CREATE_EXE_FAILED_TITLE,
    	        IDS_FILE_READ_FAILED, pmPathName, 
    	        EdFail_GetErrorMessage (myErrorBuffer), GetLastError ());
	    CloseHandle (myEXEFileHandle);
            EdWin_ShowStatus (pmTextDisplayWindow, "Create EXE cancelled");
	    return;
        }
        
        if (myBytesRead == 0)
        {
    	    EdGUI_Message1 (pmTextDisplayWindow, 0, IDS_CREATE_EXE_FAILED_TITLE,
    			    IDS_CANT_FIND_OFFSET_MARKER, pmPathName);
	    CloseHandle (myEXEFileHandle);
            EdWin_ShowStatus (pmTextDisplayWindow, "Create EXE cancelled");
	    return;
        }
        
	if (myChar == myOffsetString [myOffsetInOffset])
	{
	    if (myOffsetInOffset == 0)
	    {
	    	myStartOfOffset = myPositionInFile;
	    }
	    myOffsetInOffset++;
	    if ((unsigned) myOffsetInOffset == strlen (myOffsetString))
	    {
	    	DWORD	myResult;
	    	
	        myResult = SetFilePointer (myEXEFileHandle, myStartOfOffset, 
	        			   NULL, FILE_BEGIN);
		if (myResult == INVALID_SET_FILE_POINTER)
		{
		    // TW Error message
		    CloseHandle (myEXEFileHandle);
	            EdWin_ShowStatus (pmTextDisplayWindow, "Create EXE cancelled");
		    return;
		}	        			   
	        myResult = WriteFile (myEXEFileHandle, &myPrologSize, 
	        		      sizeof (int), &myBytesWritten, NULL);
	    	if (!myResult || ((int) myBytesWritten != sizeof (int)))
    		{
    		    // TW Error message
		    CloseHandle (myEXEFileHandle);
	            EdWin_ShowStatus (pmTextDisplayWindow, "Create EXE cancelled");
		    return;
		}
	    	break;
	    }
	} 
	else
	{
	    myOffsetInOffset = 0;
	}

        myPositionInFile++;
    } // while (TRUE)
    
    CloseHandle (myEXEFileHandle);
    EdWin_ShowStatus (pmTextDisplayWindow, "%s created", 
    		      EdFile_GetFileName (pmPathName));
} // EdRun_CreateEXE


/************************************************************************/
/* EdRun_DebuggerOutput							*/
/*									*/
/* NOTE: This routine operates in the main thread, and is called by the	*/
/* Windows dispatcher.							*/
/************************************************************************/
void	EdRun_DebuggerOutput (int pmPurpose, char *pmOutputBuffer)
{
    // This is only used by the Java version
} // EdRun_DebuggerOutput


/************************************************************************/
/* EdRun_Execute							*/
/*									*/
/* Executes a Turing program.  Return TRUE if program is to be rerun.	*/
/************************************************************************/
BOOL	EdRun_Execute (HWND pmWindow, const char *pmDummy1,
		       int pmDummy2, BOOL pmWithArgs, BOOL pmStartWithStep)
{
    SYSTEMTIME		mySystemTime;
    FILETIME		myFileTime;
    OOTint		myNumErrors;
    FilePath		myExecutionDirectory;
    int			myFontSize;
    char		*myInputFile, *myOutputFile;
    RunArgs		myRunArgs;
    MMRESULT		myTimer;

	// PROFILING - work in progress
	// Needs to be exposed through an API (in turing)
	// hashmap to store timing data for profiling
	// only allocated if profiling is enabled (by checking trace execution)
	/*map_t myProfileMap = NULL; 
	LARGE_INTEGER myTickFreq,myTime1,myTime2;
	double myProfElapsed;*/
    
    stExecutingWindow = EdGUI_GetTopWindow (pmWindow);
    stAllRunWindowsClosed = TRUE;

    // Clear all previous errors (warnings)
    EdErr_ClearErrors ();

    //
    // Get run arguments if necessary
    //
    if (pmWithArgs)
    {
    	if (!DialogBoxParam (gProgram.applicationInstance, 
                MAKEINTRESOURCE (TURING_RUN_WITH_ARGS_DIALOG),
	        pmWindow, MyRunWithArgsDialogProcedure, (LPARAM) &stRunArgs))
    	{
	    return FALSE;
    	}
	myRunArgs = stRunArgs;
    }
    else
    {
    	myRunArgs.commandLineArguments [0] = 0;
        strncpy (myRunArgs.ootArgs [0], stRunArgs.ootArgs [0], 255);    	    			     
	myRunArgs.numArgs = 0;
    	myRunArgs.inputRedirection = ARGS_IN_KEY;
    	myRunArgs.outputRedirection = ARGS_OUT_SCREEN;
    	myRunArgs.inputFile [0] = 0;
    	myRunArgs.outputFile [0] = 0;
    }
    
    //
    // Display status message using the file name that was compiled
    //
    MyStatusExecuting (stSourceFileName, PREPARING_EXECUTION);

    
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

    Language_SetupExecution (MyGetMaxStackSize (), "", "", myRunArgs.ootArgs,
    			     myRunArgs.numArgs);
    
    EdRun_SetStream0Destination ();
    EdRun_SendInfoToStderr (gProperties.errorsToStderr, 
			    gProperties.infoToStderr);

    if (myRunArgs.inputRedirection == ARGS_IN_KEY)
    {
	myInputFile = NULL;
    }
    else
    {
	myInputFile = myRunArgs.inputFile;
    }
    if (myRunArgs.outputRedirection == ARGS_OUT_SCREEN)
    {
	myOutputFile = NULL;
    }
    else
    {
	myOutputFile = myRunArgs.outputFile;
    }

    // Set the base source directory and the execution directory to be the
    // directory in which the source file was located.
    EdFile_GetDirectoryFromPath (stSourcePathName, myExecutionDirectory);
    
    myFontSize = gProperties.runConsoleFontSize;
    if (gProperties.runUseSmallFont)
    {
	RECT	myRect = EdGUI_GetWorkArea ();

	if ((myRect.right - myRect.left <= 640) ||
	    (myRect.bottom - myRect.top <= 480))
	{
	    myFontSize = 9;
	}
    }

    if (!MIO_Init_Run (stSourceFileName, myInputFile, 
    		      myRunArgs.inputRedirection == ARGS_IN_FILE_ECHO,
    		      myOutputFile,
    		      ((myRunArgs.outputRedirection == ARGS_OUT_FILE_SCREEN) ||
    		       (myRunArgs.outputRedirection == ARGS_OUT_PRINTER_SCREEN)),
    		      ((myRunArgs.outputRedirection == ARGS_OUT_PRINTER) ||
    		       (myRunArgs.outputRedirection == ARGS_OUT_PRINTER_SCREEN)),
    		      myExecutionDirectory,		// Execution directory
    		      gProperties.useGraphicsMode, 	// Graphics Mode
    		      gProperties.runConsoleFontName, 	// Run window font name
    		      myFontSize, 			// Run window font size
    		      0, // Run window font width
    		      0, // Run window font options
    		      0, // Run window dimensions
    		      0, 0, // Run window width and height
    		      gProperties.runConsoleTextRows, 	// Run window rows
    		      gProperties.runConsoleTextCols, 	// Run window columns
    		      gProperties.runConsoleFullScreen,
		      (COLOR) RGB (0, 0, 132),
		      !gProperties.prohibitSysExec,	// Allow/Forbid Sys.Exec
		      !gProperties.noSound,		// Allow/Forbid Music
		      gProperties.parallelIOPort,	// Parallel I/O Port
		      FALSE))				// Not a test program
    {
    	return FALSE;
    }
    
    // Status message    
    MyStatusExecuting (stSourceFileName, STARTING_EXECUTION);
    Ed_SendMessageToAllWindows (FALSE, WM_SET_BUTTONS, SET_BUTTON_IS_EXECUTING, 
    				0);

    stTuringProgramRunning = TRUE;
    stTuringProgramPaused = FALSE;
    stTuringProgramHalting = FALSE;
    stQuittingEnvironment = FALSE;
    stRerunningTuringProgram = FALSE;

    if (pmStartWithStep)
    {
		stStepping = STEP;
    	Language_SetStep (Language_Step_StepIntoCall);
    }
    else
    {
    	stStepping = NO_STEP;
    }
    
    do
    {
    	// Execute a quantum if
    	// (1) We've received a step instruction
    	// (2) The program is not paused and we're not tracing or
    	//     The program is not paused, we're tracing and trace speed is 0 or
    	//     The program is not paused, we're tracing and we've passed the trace delay
    	if ((stStepping != NO_STEP) || 
	    (!stTuringProgramPaused && 
	     (!stTracing  || (stTraceSpeed == 0) ||
	      (EdGUI_GetTicks () >= stTraceDelayTimer))))
		{
			/*if(stTracing) { // profiling, work in progress
				if(myProfileMap == NULL) {
					myProfileMap = hashmap_new();
					QueryPerformanceFrequency(&myTickFreq);
					if(myProfileMap == NULL || myTickFreq.QuadPart == 0) { // still null? We have a problem
						MessageBox(NULL,"Can't start profiling.","ERROR",MB_OK|MB_ICONEXCLAMATION);
						return FALSE;
					}
				}
				QueryPerformanceCounter(&myTime1); // start the clock
			}*/

    	    MyExecuteQuantum (&myNumErrors, pmWindow);

			// If we're using sprites, then it may be time to update the screen.
			MIO_UpdateSpritesIfNecessary ();

    	    if (gProperties.executionDelay > 0)
    	    {
    	    	Sleep (gProperties.executionDelay);
    	    }

			/*if(stTracing) { // profiling, work in progress
				double **myResult;
				double myCurVal = 0.0;
				char *myKey;
				char *myFilename;
				int myOk;

				QueryPerformanceCounter(&myTime2); // stop the clock
				myProfElapsed = (myTime2.QuadPart - myTime1.QuadPart) * 1000.0 / myTickFreq.QuadPart;


				FileManager_FileName (stRunStatus.srcPos.fileNo, myFilename);
				sprintf(myKey,"%s - %i",myFilename,stRunStatus.srcPos.lineNo);

				myOk = hashmap_get(myProfileMap,myKey,(any_t*)myResult);
				if(myOk == MAP_OK) myCurVal = **myResult; // this should point to a double if it worked

				//hashmap_put(myProfileMap,myKey,
			}*/
        }

	// We are waiting for a certain amount of time to pass.  We do this by
	// setting a timer and then waiting for the next event to pass.
	if (stDelayWait > 0)
	{
	    int	myTimerResolution;
	    
    	    if (!IsWindow (pmWindow))
	    {
		pmWindow = Ed_GetLastActiveEditWindow ();
	    }
	    
	    myTimerResolution = max (stMinimumPeriod, (UINT) stDelayWait / 10);	
	    myTimer = timeSetEvent ((UINT) stDelayWait, myTimerResolution, 
	    			    MyTimerProcedure, (DWORD) pmWindow, 
	    			    TIME_ONESHOT | TIME_CALLBACK_FUNCTION);
	    if (myTimer == 0)
	    {	
	    	stDelayWait = 0;    		      	
    	    	stTuringProgramWaitingForEvent = FALSE;
    	    }
	}
		
	// If we are paused, waiting for keyboard or mouse input, or waiting 
	// for a delay, don't return until we have processed at least 
	// one Windows event.
	Ed_ProcessWaitingEvents (stTuringProgramPaused || 
				 stTuringProgramWaitingForEvent);
	if (stDelayWait > 0)
	{
	    if (!IsWindow (pmWindow))
	    {
	    	timeKillEvent (myTimer);
	    }
	}
    } while ((stRunStatus.state != Finished) && (!stTuringProgramHalting));

    //
    // At this point, the program has either finished executing or the been
    // told to halt (permanently) either by the user closing a run window or
    // pressing stop, etc.
    // 
    // At this point, we've been processing any number of OS messages.
    // This means that the system could have closed any open windows
    // that we refer to.  Check carefully before using them.
    //

    // If the original window is no longer open, use the top editor window.
    if (!IsWindow (pmWindow))
    {
	pmWindow = Ed_GetLastActiveEditWindow ();
    }

    // If we were paused
    Ed_SendMessageToAllWindows (FALSE, WM_SET_BUTTONS, SET_BUTTON_NOT_RUNNING, 
    				0);

    // Status message    
    MyStatusExecuting (stSourceFileName, FINISHED_EXECUTION);

    MIO_Finalize_Run ();
	
    // Close all files opened by the running program
    Language_EndExecution ();
	
    stTuringProgramRunning = FALSE;
    
    if (stTuringProgramHalting && 
	(stRerunningTuringProgram || stQuittingEnvironment))
    {
	if (stQuittingEnvironment)
	{
	    // Free any previously allocated memory.  This could take a while if 
	    // someone allocated a huge amount
	    EdRun_InitializeForRunWithoutCompile (pmWindow);
    
	    // Close any run windows and free any allocated objects in debugger		      
	    MIO_Init_Free ();
    
	    Language_Reset ();
		
	    PostQuitMessage (0);
	}

	return stRerunningTuringProgram;
    }

    // If the user halted the program, we manufacture an error message
    if (stRunStatus.state != Finished)
    {
    	myNumErrors = 1;
    	
    	stFirstErrorPtr = (TuringErrorPtr) malloc (sizeof (
    						struct Language_ErrMsgs));
    	
    	strcpy (stFirstErrorPtr -> text, "Program interrupted");
    	stFirstErrorPtr -> next = NULL;
    	
	if (stRunStatus.srcPos.fileNo == 0)
	{
	    struct Language_ThreadQueues	*myThreadQueues;

	    Language_DumpThreadList (&myThreadQueues);
	    stFirstErrorPtr->srcPos = 
	    			myThreadQueues->threadList->traceback->srcPos;
	}
	else
	{
	    stFirstErrorPtr->srcPos = stRunStatus.srcPos;
	}
	
	// Set the Run Window title
	// TW
    }
    else
    {
	if (myNumErrors > 0)
	{
	    // We had a real run-time error.
	    MessageBeep (MB_ICONEXCLAMATION);
	}

	// Set the Run Window title
	// TW
    }
    
    // If the run window is a text window, make it selectable at this point
    // TW

    if ((myNumErrors >= 1) && 
	(strncmp (stFirstErrorPtr -> text, SYSEXIT_ERROR_STRING, 
		  strlen (SYSEXIT_ERROR_STRING)) != 0))
    {
    	WORD		myErrorTuringFileNo;
    	FilePath	myErrorPathName;
    	SrcPosition	*mySrc;
    	
    	myErrorTuringFileNo = stFirstErrorPtr -> srcPos.fileNo;
    	FileManager_FileName (myErrorTuringFileNo, myErrorPathName);
    	mySrc = &(stFirstErrorPtr -> srcPos);
        EdErr_AddError (myErrorPathName, myErrorTuringFileNo,
        		mySrc -> lineNo, mySrc -> linePos + 1, mySrc -> lineNo, 
		      	mySrc -> linePos + mySrc -> tokLen, 
		      	stFirstErrorPtr -> text);

	EdErr_FindFirstError (pmWindow, NO_MESSAGE_IF_NO_ERROR,
			      ACTIVATE_SOURCE_WINDOW);
    }

    return stRerunningTuringProgram;
} // EdRun_Execute


/************************************************************************/
/* EdRun_FixMain							*/
/************************************************************************/
void	EdRun_FixMain (HWND pmTextDisplayWindow)
{
    // Remove any references to an outstanding main program
    if (stMainProgramFileNo != FLOATING_MAIN)
    {
    	FileManager_DeleteReference (stMainProgramFileNo);
    	FileManager_FlushUnreferencedFiles ();
    }
    
    // Set the new main program's file number
    stMainProgramFileNo = EdDisp_GetTuringFileNo (pmTextDisplayWindow);

    // Debug messages
    EdFail_AddMessage (NORMAL, "Fix Main - Window: %x - Fileno: %d", 
    		       pmTextDisplayWindow, stMainProgramFileNo);
        
    // Add a file manager reference to stop it being cleared out 
    // by the file manager.
    FileManager_AddReference (stMainProgramFileNo);
} // EdRun_FixMain


/************************************************************************/
/* EdRun_FixMainPathName						*/
/************************************************************************/
void	EdRun_FixMainPathName (const char *pmPathName)
{
    // We don't need to do anything here.  Turing File Manager automatically
    // handles this for us.
} // EdRun_FixMainPathName


/************************************************************************/
/* EdRun_FloatMain							*/
/************************************************************************/
void	EdRun_FloatMain (void)
{
    // Remove any references to an outstanding main program
    if (stMainProgramFileNo != FLOATING_MAIN)
    {
    	FileManager_DeleteReference (stMainProgramFileNo);
    	FileManager_FlushUnreferencedFiles ();
    }
    
    // Debug messages
    EdFail_AddMessage (NORMAL, "Float Main - Was Fileno: %d", 
    		       stMainProgramFileNo);
        
    stMainProgramFileNo = FLOATING_MAIN;
} // EdRun_FloatMain


/************************************************************************/
/* EdRun_GetMainFileNo							*/
/************************************************************************/
unsigned short	EdRun_GetMainFileNo (void)
{
    return stMainProgramFileNo;
} // EdRun_GetMainFileNo


/************************************************************************/
/* EdRun_GetMainPathName						*/
/************************************************************************/
const char	*EdRun_GetMainPathName (void)
{
    static char myFileName [256];
    
    if (stMainProgramFileNo == FLOATING_MAIN)
    {
        return NULL;
    }
    
    FileManager_FileName (stMainProgramFileNo, myFileName);
    
    if (myFileName [0] == 0)
    {
    	return NULL;
    }
    
    return myFileName;
} // EdRun_GetMainPathName


/************************************************************************/
/* EdRun_GetMainProgramType						*/
/************************************************************************/
int	EdRun_GetMainProgramType (void)
{
    return APPLICATION;
} // EdRun_GetMainProgramType


/************************************************************************/
/* EdRun_GetMainWindow							*/
/************************************************************************/
HWND	EdRun_GetMainWindow (void)
{
    return NULL;
} // EdRun_GetMainWindow


/************************************************************************/
/* EdRun_GetPseudoPath							*/
/************************************************************************/
BOOL	EdRun_GetPseudoPath (HWND pmWindow, char *pmText, int pmTextLen, 
			     char *pmPseudoPath)
{
    return FALSE;
} // EdRun_GetPseudoPath


/************************************************************************/
/* EdRun_GotoLineInDocument						*/
/************************************************************************/
void	EdRun_GotoLineInDocument (unsigned short pmFileNo, 
				  unsigned short pmLineNo, BOOL forceFlags)
{
    // TW
} // EdRun_GotoLineInDocument


/************************************************************************/
/* EdRun_InitializeForRunWithoutCompile					*/
/************************************************************************/
void	EdRun_InitializeForRunWithoutCompile (HWND pmWindow)
{
    EdFail_AddMessage (NORMAL, "Turing Language Freeing Memory");
    if (pmWindow != NULL)
    {
	EdWin_ShowStatus (pmWindow, "Freeing memory used by Turing program");
    }
    Language_ErrorModule_Initialize ();
    Language_DebugModule_Initialize ();
    Language_Execute_RecoverAllMemory ();
} // EdRun_InitializeForRunWithoutCompile


/************************************************************************/
/* EdRun_IsAnyRunWindowVisible						*/
/************************************************************************/
BOOL	EdRun_IsAnyRunWindowVisible (void)
{
    return MIO_IsAnyRunWindowVisible ();
} // EdRun_IsAnyRunWindowVisible


/************************************************************************/
/* EdRun_IsProgramPaused						*/
/************************************************************************/
BOOL	EdRun_IsProgramPaused (void)
{
    return stTuringProgramPaused;
} // EdRun_IsProgramPaused


/************************************************************************/
/* EdRun_IsProgramRunning						*/
/************************************************************************/
BOOL	EdRun_IsProgramRunning (void)
{
    return stTuringProgramRunning;
} // EdRun_IsProgramRunning


/************************************************************************/
/* EdRun_IsProgramRunningAnApplet					*/
/************************************************************************/
BOOL	EdRun_IsProgramRunningAnApplet (void)
{
    return FALSE;
} // EdRun_IsProgramRunningAnApplet


/************************************************************************/
/* EdRun_IsMainPathName							*/
/************************************************************************/
BOOL	EdRun_IsMainPathName (const char *pmPathName)
{
    return FALSE;
} // EdRun_IsMainPathName


/************************************************************************/
/* EdRun_IsTracing							*/
/************************************************************************/
BOOL	EdRun_IsTracing (void)
{
    return stTracing;
} // EdRun_IsTracing


/************************************************************************/
/* EdRun_KillRunningProgram						*/
/************************************************************************/
void	EdRun_KillRunningProgram (void)
{
    EdFail_AddMessage (NORMAL, "Kill Running Program");
    stTuringProgramHalting = TRUE;

    // Clear all previous errors
    EdErr_ClearErrors ();
} // EdRun_KillRunningProgram


/************************************************************************/
/* EdRun_KillRunningProgramAndQuit					*/
/*									*/
/* Returns: TRUE if the executor will post the quit message.		*/
/*          FALSE if the caller is responsible for posting the message.	*/
/*									*/
/* This notifies the caller whether it should post the quit message, or	*/
/* whether that will be done in the executor.  If a program is		*/
/* currently running, the the exector will post the quit message once	*/
/* it has stopped execution, closed its run windows, etc.  Otherwise	*/
/* it's the responsibility of the caller to post the quit message.	*/		
/************************************************************************/
BOOL	EdRun_KillRunningProgramAndQuit (HWND pmWindow)
{
    EdFail_AddMessage (NORMAL, "Kill Running Program and Quit");
    stTuringProgramHalting = TRUE;
    stQuittingEnvironment = TRUE;
    stRerunningTuringProgram = FALSE; // Just in case...

    // Clear all previous errors
    EdErr_ClearErrors ();

    return stTuringProgramRunning;
} // EdRun_KillRunningProgramAndQuit


/************************************************************************/
/* EdRun_KillRunningProgramAndRerun					*/
/*									*/
/* This routine may be called when we are still executing a program or	*/
/* after execution has finished.  If it is called after execution is	*/
/* finished, then we run the program that we last compiled.		*/
/************************************************************************/
void	EdRun_KillRunningProgramAndRerun (void)
{
    if (stTuringProgramRunning)
    {
    	stTuringProgramHalting = TRUE;
    	stRerunningTuringProgram = TRUE;
    }
    else
    {
    	// Run the same program as last time.  The other two arguments
    	// are actually ignored.  They will be set to the same value as
    	// last time.
    	EdDisp_Run (SAME_WINDOW_AS_LAST_TIME, TRUE, FALSE, RUN_CONTINUOUSLY);
    }
} // EdRun_KillRunningProgramAndRerun


/************************************************************************/
/* EdRun_LoadBreakpoints						*/
/*									*/
/* When a file is loaded, check the list of errors to see if there are	*/
/* any break points referring to this file.  If there are, add them 	*/
/* to the text.								*/
/************************************************************************/
void	EdRun_LoadBreakpoints (HWND pmTextDisplayWindow)
{
/*    TextPtr	myTextPtr = EdDisp_GetTextPtr (pmTextDisplayWindow);
    int		myTuringFileNo = EdDisp_GetTuringFileNo (pmTextDisplayWindow);
    int		cnt;
    
    for (cnt = 0 ; cnt < stNumBreakpoints ; cnt++)
    {
    	if (stBreakpoints [cnt].fileNo = myTuringFileNo)
    	{
    	    EdText_ToggleBreakpoint (myTextPtr, stBreakpoints [cnt].lineNo + 1);
    	}
    }
*/    
} // EdRun_LoadBreakpoints


/************************************************************************/
/* EdRun_MatchPath							*/
/************************************************************************/
TextPtr	EdRun_MatchPath (const char *pmPathName)
{
    MatchWindow	myMatchWindow;
    
    myMatchWindow.fileNo = 0;
    myMatchWindow.pathName = pmPathName;
    myMatchWindow.textPtr = NULL;
    
    Ed_EnumerateFiles (MyMatchPathName, &myMatchWindow);

    return myMatchWindow.textPtr;    
} // EdRun_MatchPath


/************************************************************************/
/* EdRun_MatchTuringFileNo						*/
/************************************************************************/
HWND	EdRun_MatchTuringFileNo (WORD pmTuringFileNo)
{
    MatchWindow	myMatchWindow;
    
    myMatchWindow.fileNo = pmTuringFileNo;
    myMatchWindow.pathName = NULL;
    myMatchWindow.textWindow = NULL;
    
    Ed_EnumerateFiles (MyMatchPathName, &myMatchWindow);

    return myMatchWindow.textWindow;    
} // EdRun_MatchTuringFileNo


/************************************************************************/
/* EdRun_NotifyAllRunWindowsClosed					*/
/*									*/
/* If we're in beginner mode and we've closed the run window, enlarge	*/
/* the editor window.							*/
/************************************************************************/
void	EdRun_NotifyAllRunWindowsClosed (void)
{
    if (!stAllRunWindowsClosed)
    {
	stAllRunWindowsClosed = TRUE;
	if (gProperties.beginnerMode && IsWindow (stExecutingWindow) && 
	    IsIconic (stExecutingWindow))
	{
	    EdDisp_SetDisplayResizeMessage (FALSE);
	    ShowWindow (stExecutingWindow, SW_RESTORE);
	    EdDisp_SetDisplayResizeMessage (TRUE);
	}
    }
} // EdRun_NotifyAllRunWindowsClosed


/************************************************************************/
/* EdRun_NotifyFileClosed						*/
/************************************************************************/
void	EdRun_NotifyFileClosed (HWND pmTextDisplayWindow)
{
    FileNoType	myTuringFileNo;
    TextPtr	myTextPtr;
    int		myTextSize;
    char	*myPtr;
    
    myTuringFileNo = EdDisp_GetTuringFileNo (pmTextDisplayWindow);
    
    if (myTuringFileNo != NULL_OOT_VALUE)
    {
    	//
    	// First, we copy the text buffer to another location.  We then
    	// set the file manager to look at it and free up the text handle.
    	// Then we call FileManager_CloseFileHandle in order to release the
    	// information if necessary.
    	//
    	myTextPtr = EdDisp_GetTextPtr (pmTextDisplayWindow);
    	myTextSize = myTextPtr -> textLen + 1;
    	myPtr = malloc (myTextSize);
    	if (myPtr == NULL)
    	{
    	    // If we don't have enough memory to reload the program, then
    	    // we reset the compiler, which dumps everything out of the cache.
    	    Language_Reset ();
    	}
    	else
    	{
    	    memcpy (myPtr, myTextPtr -> text, myTextSize);
    	    *(EdDisp_GetTuringSizePtr (pmTextDisplayWindow)) = myTextSize;
    	    *(EdDisp_GetTuringTextHandle (pmTextDisplayWindow)) = myPtr;
    	    myPtr [myTextPtr -> textLen] = 0;
    	}
    	
    	FileManager_CloseFileHandle (myTuringFileNo);
    	
    	// Just in case this gets called twice, let's set the file number to 0.
    	EdDisp_SetTuringValues (pmTextDisplayWindow, NULL_OOT_VALUE, 
    			    	NULL, NULL, NOTIFY_CLOSED);
    }
} // EdRun_NotifyFileClosed


/************************************************************************/
/* EdRun_NotifyFileOpened						*/
/************************************************************************/
void	EdRun_NotifyFileOpened (HWND pmTextDisplayWindow)
{
    char		*myPathName;
    FileNoType		myTuringFileNo;
    TextHandleType	myTuringTextHandle;
    SizePtrType		myTuringSizePtr;
    ResultCodeType	myResult;
        
    //
    // The call to OpenNamedHandle is bracketed by the setting of 
    // gFileManagerIgnoreFileOperation to true in order to prevent
    // the actual loading of the file.  When gFileMan... is set to
    // true, instead of loading a file, the file manager sets the
    // size to zero and the buffer pointer to (char *) 1.
    //
    myPathName = EdDisp_GetPath (pmTextDisplayWindow);
    
    gFileManagerIgnoreFileOperation = TRUE;
    if (myPathName == NULL)
    {
    	FileManager_OpenUnnamedHandle (&myTuringFileNo, &myTuringTextHandle,
    				       &myTuringSizePtr);
    }
    else
    {
    	FileManager_OpenNamedHandle (myPathName, &myTuringFileNo, 
    				     &myTuringTextHandle, &myTuringSizePtr,
    				     &myResult);
    }
    gFileManagerIgnoreFileOperation = FALSE;

    //     
    // If we've previously loaded this file, (i.e. it's been used in a 
    // compilation) then free it up, since we're going to use our own buffer
    // before we next run.  Note that if the file wasn't loaded, then the
    // text handle returned will be equal to (char *) 1.
    // 
    if (*myTuringTextHandle != (char *) 1)
    {
    	free (*myTuringTextHandle);
    	*myTuringTextHandle = (char *) 1;
    	*myTuringSizePtr = 0;
    }
    
    EdDisp_SetTuringValues (pmTextDisplayWindow, myTuringFileNo, 
    			    myTuringTextHandle, myTuringSizePtr, NOTIFY_OPENED);
} // EdRun_NotifyFileOpened


/************************************************************************/
/* EdRun_NotifyFileSaved						*/
/************************************************************************/
void	EdRun_NotifyFileSaved (HWND pmTextDisplayWindow)
{
    FileNoType		myOldTuringFileNo, myTuringFileNo;
    TextHandleType	myTuringTextHandle;
    SizePtrType		myTuringSizePtr;
    ResultCodeType	myResult;
    
    
    myTuringFileNo = EdDisp_GetTuringFileNo (pmTextDisplayWindow);
    myOldTuringFileNo = myTuringFileNo;

    gFileManagerIgnoreFileOperation = TRUE;
    FileManager_WriteFile (EdDisp_GetPath (pmTextDisplayWindow), 
    			   &myTuringFileNo, &myTuringTextHandle, 
    			   &myTuringSizePtr, &myResult);
    gFileManagerIgnoreFileOperation = FALSE;

    if (myTuringFileNo != myOldTuringFileNo)
    {
	EdDisp_SetTuringValues (pmTextDisplayWindow, myTuringFileNo, 
    				myTuringTextHandle, myTuringSizePtr, 
				NOTIFY_SAVED);
    }
} // EdRun_NotifyFileSaved


/************************************************************************/
/* EdRun_NotifyRunWindowOpened						*/
/*									*/
/* If we're in beginner mode and we've opened our first run window, 	*/
/* minimize the editor window.						*/
/************************************************************************/
void	EdRun_NotifyRunWindowOpened (HWND pmRunWindow)
{
    if (stAllRunWindowsClosed)
    {
	stAllRunWindowsClosed = FALSE;
	if (gProperties.beginnerMode && IsWindow (stExecutingWindow))
	{
	    CloseWindow (stExecutingWindow);
	    BringWindowToTop (pmRunWindow);
	}
    }
} // EdRun_NotifyRunWindowOpened


/************************************************************************/
/* EdRun_PauseResumeProgram						*/
/************************************************************************/
void	EdRun_PauseResumeProgram (BOOL pmActivateSourceWindow, int pmReason)
{
    if (!stTuringProgramRunning)
    {
    	// TW Assert Fail!
    	return;
    	
    }
    
    // Clear all previous errors
    EdErr_ClearErrors ();

    if (stTuringProgramPaused)
    {
    	// We're paused, so let's resume
    	stTuringProgramPaused = FALSE;

	if (pmReason == PAUSING)
	{
    	    Ed_SendStatusMessageToAllWindows ("Executing: Resumed");
	}
	else if (pmReason == STEP)
	{
    	    Ed_SendStatusMessageToAllWindows ("Executing: Stepping Into");
    	}
    	else if (pmReason == STEP_OVER)
	{
    	    Ed_SendStatusMessageToAllWindows ("Executing: Stepping Over");
    	}
    	else if (pmReason == STEP_RETURN)
	{
    	    Ed_SendStatusMessageToAllWindows ("Executing: Until Return from Call");
    	}
    	else
    	{
    	    EdFail_Fatal (IDS_BAD_REASON_FOR_PAUSE, __FILE__, __LINE__, 0, 
    		          pmReason, 1, stTuringProgramRunning, stTracing, 
			  stStepping, stRunStatus.state);
	}

	// Change the resume button back to pause.
    	Ed_SendMessageToAllWindows (FALSE, WM_SET_BUTTONS, 
    				    SET_BUTTON_IS_EXECUTING, 0);

	// Notify the MIO module that the program is no longer paused.  Put
	// the Run windows on top if we are no longer pausing.  If we are
	// just stepping, then don't bother.
	MIO_NotifyTuringProgramResumed (pmReason == PAUSING);
    }
    else
    {
    	WORD		myErrorTuringFileNo;
    	FilePath	myErrorPathName;
    	SrcPosition	mySrcPos;
    	char		*myPauseReason;
    	
    	// Let's pause the program
    	stTuringProgramPaused = TRUE;

	// Let's hilight the line that we are currently executing
	if (stRunStatus.srcPos.fileNo == 0)
	{
	    struct Language_ThreadQueues	*myThreadQueues;

	    Language_DumpThreadList (&myThreadQueues);
	    mySrcPos = myThreadQueues->threadList->traceback->srcPos;
	}
	else
	{
	    mySrcPos = stRunStatus.srcPos;
	}

	myErrorTuringFileNo = mySrcPos.fileNo;
    	FileManager_FileName (myErrorTuringFileNo, myErrorPathName);
    	if (pmReason == PAUSING)
    	{
	    myPauseReason = "Paused at this line";
	    Ed_SendStatusMessageToAllWindows ("Paused: Pause Selected");
	}
	else if (pmReason == HIT_BREAK_POINT)
	{
	    myPauseReason = "Reached breakpoint";
	    Ed_SendStatusMessageToAllWindows ("Paused: Breakpoint Reached");
	}
	else if (pmReason == HIT_UNIT_BREAK_POINT)
	{
	    myPauseReason = "Reached unit breakpoint";
	    Ed_SendStatusMessageToAllWindows (
	    				"Paused: Unit Breakpoint Reached");
	}
	else if (pmReason == HIT_BREAK_STATEMENT)
	{
	    myPauseReason = "Reached break statement";
	    Ed_SendStatusMessageToAllWindows (
	    				"Paused: \"break\" Statement Executed");
	}
	else if (pmReason == STEP)
	{
	    myPauseReason = "Paused after Step Into";
	    Ed_SendStatusMessageToAllWindows ("Paused: After Step Into");
    	}
    	else if (pmReason == STEP_OVER)
	{
	    myPauseReason = "Paused after Step Over";
	    Ed_SendStatusMessageToAllWindows ("Paused: After Step Into");
    	}
    	else if (pmReason == STEP_RETURN)
	{
	    myPauseReason = "Paused after Step Return";
	    Ed_SendStatusMessageToAllWindows ("Paused: After Step Return");
    	}
	else
	{
    	    EdFail_Fatal (IDS_BAD_REASON_FOR_PAUSE, __FILE__, __LINE__, 0, 
    		          pmReason, 2, stTuringProgramRunning, stTracing, 
			  stStepping, stRunStatus.state);
	}
        EdErr_AddError (myErrorPathName, myErrorTuringFileNo, mySrcPos.lineNo, 
        		1, mySrcPos.lineNo, 0, myPauseReason);

	EdErr_FindFirstError (stCompilingWindow, NO_MESSAGE_IF_NO_ERROR,
			      pmActivateSourceWindow);

    	Ed_SendMessageToAllWindows (FALSE, WM_SET_BUTTONS, 
    				    SET_BUTTON_PAUSED, 0);

    	// Notify the MIO module that the program is paused.
	MIO_NotifyTuringProgramPaused ();
    }
} // EdRun_PauseResumeProgram


/************************************************************************/
/* EdRun_ResetCompiler							*/
/************************************************************************/
void	EdRun_ResetCompiler ()
{
    Language_Reset ();
    MySetPreprocessorSymbols ();
} // EdRun_ResetCompiler


/************************************************************************/
/* EdRun_SetStep							*/
/************************************************************************/
void	EdRun_SetStep (int pmStep)
{
    if (pmStep == TRACE_ON)
    {
    	if (!stTuringProgramPaused)
    	{
    	    Ed_SendStatusMessageToAllWindows ("Executing: with Trace");
    	}
    	stStepping = NO_STEP;
    	return;
    }
    
    // If we're already in a step, then ignore
    if (stStepping != NO_STEP)
    {
    	return;
    }
    
    stStepping = pmStep;
    
    // If we are paused, then unpause in order to prepare to continue execution
    if (stTuringProgramPaused)
    {
    	EdRun_PauseResumeProgram (ACTIVATE_SOURCE_WINDOW, pmStep);
    }
    
    switch (pmStep)
    {
    	case STEP:
    	    if (stTuringProgramRunning)
    	    {
    	    	Language_SetStep (Language_Step_StepIntoCall);
    	    }
    	    else
    	    {
    	    	// The progam is not running, so run it and execute 1 statement.
    	    	stTuringProgramPaused = TRUE;
    	    	// TW Do Run
    	    }
    	    break;
    	case STEP_OVER:
    	    Language_SetStep (Language_Step_NextOverCall);
    	    break;
    	case STEP_RETURN:
    	    Language_SetStep (Language_Step_ReturnFromCall);
    	    break;
    } // switch
} // EdRun_SetStep


/************************************************************************/
/* EdRun_SendInfoToStderr						*/
/************************************************************************/
void	EdRun_SendInfoToStderr (BOOL pmLibErrors, BOOL pmLibInfo)
{
    MIO_SendInfoToStderr (pmLibErrors, pmLibInfo);
} // EdRun_SendInfoToStderr


/************************************************************************/
/* EdRun_SetStream0Destination						*/
/************************************************************************/
void	EdRun_SetStream0Destination (void)
{
    if (gProperties.stream0ToStderr)
    {
	Language_Execute_fileMap [2] = 0;
    }
    else
    {
	Language_Execute_fileMap [2] = -1;
    }
} // EdRun_SetStream0Destination


/************************************************************************/
/* EdRun_SetTraceSpeed							*/
/************************************************************************/
void	EdRun_SetTraceSpeed (int pmTraceSpeed)
{
    stTraceSpeed = pmTraceSpeed;
} // EdRun_SetTraceSpeed


/************************************************************************/
/* EdRun_StatusCompiling						*/
/************************************************************************/
void	EdRun_StatusCompiling (const char *pmClassName)
{
    char    myStatusMessage [1024];
	
    if (pmClassName != NULL)
    {
    	if (stFileBeingCompiled != NULL)
    	{
    	    free (stFileBeingCompiled);
    	}
    	
    	stFileBeingCompiled = malloc (strlen (pmClassName) + 1);
    	strcpy (stFileBeingCompiled, pmClassName);
    } 
    else if (stFileBeingCompiled == NULL)
    {
    	stFileBeingCompiled = malloc (strlen ("Program") + 1);
    	strcpy (stFileBeingCompiled, "Program");
    }

    wsprintf (myStatusMessage, "Compiling %s", stFileBeingCompiled);
    EdWin_ShowStatus (stCompilingWindow, myStatusMessage);
} // EdRun_StatusCompiling


/************************************************************************/
/* EdRun_TestSuite							*/
/*									*/
/* Read and execute programs found in "_index.dat".  The file has the	*/
/* following format:							*/
/*  #execdir <directory name>	This is the directory containing input	*/
/*				and output files.			*/
/*  Each following line is in the format of:				*/
/*  {D,B,I} path							*/
/*     D represents a directory, read pmFileName in directory <path>	*/
/*       and continue recursively.					*/
/*     B represents batch mode. <path> represents a file name. Run	*/
/*       program stored in <path>.  If errors are found, save in	*/
/*	 <base path>.err, if input is needed, then read from		*/
/*	 <base path>.in, standard output is stored in <base path>.txt	*/
/*	 for text mode or <base path>.bmp for graphics output.  All	*/
/*	 files read are found in <path> directory.  All files written	*/
/*	 are saved in the execution directories.			*/
/*     I represents interactive. Any lines starting in '!' are shown in	*/
/*	 a message box (presumably instructions on how to run the	*/
/*	 program).  All input comes form the user.  When the program	*/
/*	 finishes execution, a dialog box appears with a text area that	*/
/*	 asks if execution was successful and saves the result in a	*/
/*	 <base path>.txt.						*/
/************************************************************************/
void	EdRun_TestSuite (const char *pmDirectoryName, const char *pmOutputDir)
{
    FilePath		myIndexPath, myPath, myOutputDir;
    int			myLineNo = 0;
    char		*myBuffer;
    int			myFileSize;
    char		*myLine;
    
    // If test suite diretcory doesn't exist, abort with error
    if (!EdFile_DirectoryExists (pmDirectoryName))
    {
        EdGUI_Message1 (NULL, MB_ICONEXCLAMATION, IDS_TEST_SUITE_ERROR_TITLE, 
	    IDS_TEST_SUITE_DIRECTORY_DOESNT_EXIST, pmDirectoryName);
	return;
    }

    // myIndexPath contains the path to the index file
    strcpy (myIndexPath, pmDirectoryName);
    if (pmDirectoryName [strlen (pmDirectoryName) - 1] != '\\')
    {
	strcat (myIndexPath, "\\");
    }
    strcat (myIndexPath, "_index.txt");

    // Read file into myBuffer.  Abort if fail.
    myBuffer = EdFile_ReadFileIntoBuffer (NULL, myIndexPath, 
    					  &myFileSize, ERROR_IF_NOT_FOUND);
    if (myBuffer == NULL)
    {
	// Error message about _index.dat non-existence displayed by 
	// EdFile_ReadFileIntoBuffer.  
	
        EdGUI_Message1 (NULL, MB_ICONEXCLAMATION, IDS_TEST_SUITE_ERROR_TITLE, 
	    IDS_TEST_SUITE_ABORT, myIndexPath);
	return;
    }

    if (pmOutputDir != NULL)
    {
	strcpy (myOutputDir, pmOutputDir);
    }
    else
    {
	myOutputDir [0] = 0;
    }

    // The files are stored in order from most recent to least    
    EdFile_ReadLineFromBuffer (myBuffer, myFileSize, READ_FIRST);

    // Read the last active directory
    myLine = EdFile_ReadLineFromBuffer (myBuffer, 0, READ_NEXT);
    while (myLine != NULL)
    {
	myLineNo++;
	EdGlob_Trim (myLine);
	if (strncmp ("#execdir ", myLine, 9) == 0)
	{
	    if (myOutputDir [0] == 0)
	    {
		EdFile_CombinePath (pmDirectoryName, &myLine [9],
				    myOutputDir);
	    }
	    else
	    {
	        EdGUI_Message1 (NULL, MB_ICONEXCLAMATION, 
		    IDS_TEST_SUITE_ERROR_TITLE, 
		    IDS_TEST_SUITE_EXECDIR_ALREADY_SPECIFIED, myLineNo, 
		    myIndexPath);
		return;
	    }
	}
	else if ((myLine [0] == '#') || (myLine [0] == 0))
	{
	    continue;
	}
	else if ((toupper (myLine [0]) == 'D') && (myLine [1] == ' '))
	{
	    FilePath	myNewOutputDir;

	    if (myOutputDir [0] != 0)
	    {
		strcpy (myNewOutputDir, myOutputDir);
		if (myNewOutputDir [strlen (myNewOutputDir) - 1] != '\\')
		{
		    strcat (myNewOutputDir, "\\");
		}
		strcat (myNewOutputDir, &myLine [2]);
	    }

	    strcpy (myPath, pmDirectoryName);
	    if (pmDirectoryName [strlen (pmDirectoryName) - 1] != '\\')
	    {
		strcat (myPath, "\\");
	    }
	    strcat (myPath, &myLine [2]);
	    
	    if (!EdFile_DirectoryExists (myPath))
	    {
	        EdGUI_Message1 (NULL, MB_ICONEXCLAMATION, 
		    IDS_TEST_SUITE_ERROR_TITLE, 
		    IDS_TEST_SUITE_DIRECTORY_DOESNT_EXIST_1, myLineNo,
		    myIndexPath, myPath);
		return;
	    }
	    else
	    {
	    	if (myOutputDir [0] == 0)
	    	{
		    EdRun_TestSuite (myPath, NULL);
		}
		else
		{
		    EdRun_TestSuite (myPath, myNewOutputDir);
		}
	    }
	}
	else if ((toupper (myLine [0]) == 'B') && (myLine [1] == ' '))
	{
	    MyRunTestSuiteProgram (pmDirectoryName, myOutputDir,
				   &myLine [2], myLineNo, myIndexPath); 
	}
	else if ((toupper (myLine [0]) == 'I') && (myLine [1] == ' '))
	{
	}
	else
	{
	    // TW Error
	}

	// Read the next line
	myLine = EdFile_ReadLineFromBuffer (myBuffer, 0, READ_NEXT);
    } // while

    free (myBuffer);
} // EdRun_TestSuite

/************************************************************************/
/* EdRun_RunProgramNoEditor						*/
/*									*/
/* Run a program not from the editor. Used for command line execution.					*/
/************************************************************************/
void	EdRun_RunProgramNoEditor (const char *pmTestDirectory, const char *pmTestFileName)
{
    FilePath		myTestPathName, mySourceDirectory;
    HWND		myEditWindow, myTextDisplayWindow;
    TuringErrorPtr	myError;    
    int			myErrors;

	printf("running program %s\n",pmTestFileName);

    // Create the test file path
    strcpy (myTestPathName, pmTestFileName);
 

    // Make certain the test file exists
    if (!EdFile_FileExists (myTestPathName))
    {
	// Test file not found
	EdGUI_Message1 (NULL, MB_ICONEXCLAMATION, 
	    IDS_TEST_SUITE_ERROR_TITLE, 
	    IDS_TEST_SUITE_FILE_DOESNT_EXIST, 0,
		"<none>", myTestPathName);
	return;
    }
    
    //
    // Open the file in the editor
    // Compile the file
    // Write extant errors to path.err
    // If no errors, execute program
    //    During execution, if reading from stdin, redirect from stdin.
    //    At end of execution, write output to path.out or path.bmp
    // Close the editor file.
    //
    
    // Open the file in the editor.
    myEditWindow = EdWin_CreateShow (myTestPathName, NULL, 0, FALSE, FALSE,FALSE);
    if (myEditWindow == NULL)
    {
	EdGUI_Message1 (NULL, 0, IDS_TEST_SUITE_ERROR_TITLE,
	    IDS_TEST_SUITE_UNABLE_TO_OPEN_WINDOW, 0,
	    "<none>", myTestPathName);
	return;
    }
    myTextDisplayWindow = EdWin_GetActiveDisplayWindow (myEditWindow);
    
    //
    // Compile the file
    //
    
    stSourceFileNo = EdDisp_GetTuringFileNo (myTextDisplayWindow);
    strncpy (stRunArgs.ootArgs [0], pmTestFileName, 255);    	    			     
    
    // Set the base source directory to be the directory 
    // in which the source file was located.
    EdFile_GetDirectoryFromPath (pmTestFileName, mySourceDirectory);
    FileManager_ChangeDirectory ((OOTstring) mySourceDirectory);
    
    // Now, for every open file, set the OOT file manager pointer to 
    // point to it and mark it modified
    Ed_EnumerateFiles (MySetTuringPointers, NULL);
    
    stCompilingWindow = myTextDisplayWindow;
    
    // Clear all previous errors
    EdErr_ClearErrors ();
    
    // Free any previously allocated memory.  This could take a while if 
    // someone allocated a huge amount
    Language_ErrorModule_Initialize ();
    Language_DebugModule_Initialize ();
    Language_Execute_RecoverAllMemory ();
    
    // Close any run windows and free any allocated objects in debugger		      
    MIO_Init_Free ();
    
    // Compile the program
    EdWin_ShowStatus (myTextDisplayWindow, "Initializing Compiler");
    Language_CompileProgram ("", stSourceFileNo, &myError, &myErrors);
    
    //
    // Write extant errors to stdout
    //
    
    if (myError != NULL)
    {
	int		myMessages = 0;
	printf("Syntax Errors:\n");
	while (myError != NULL)
	{
	    WORD	myErrorTuringFileNo;
	    FilePath	myErrorPathName;
	    SrcPosition	*mySrc;
	    
	    myErrorTuringFileNo = myError -> srcPos.fileNo;
	    FileManager_FileName (myErrorTuringFileNo, myErrorPathName);
	    mySrc = &(myError -> srcPos);
	    
	    if (mySrc -> tokLen > 0)
	    {
		printf ("Line %d [%d - %d] of %s: %s\n",
		    mySrc -> lineNo, mySrc -> linePos + 1,
		    mySrc -> linePos + 1 + mySrc -> tokLen, 
		    EdFile_GetFileName (myErrorPathName), myError -> text);
	    }
	    else
	    {
		printf ( 
		    "Line %d [%d] of %s: %s\n",
		    mySrc -> lineNo, mySrc -> linePos + 1,
		    EdFile_GetFileName (myErrorPathName), myError -> text);
	    }
	    myError = myError -> next;
	    myMessages++;
	} // while (myError != NULL)

	// Add a message about the number of errors and warnings
	if (myErrors > 1)
	{
	    if (myMessages - myErrors > 1)
	    {
		printf ( "%d Errors and %d Warnings\n",
		    myErrors, myMessages - myErrors);
	    }
	    else if (myMessages - myErrors == 1)
	    {
		printf ( "%d Errors and 1 Warning\n",
		    myErrors);
	    }
	    else
	    {
		printf ( "%d Errors\n", myErrors);
	    }
	}
	else if (myErrors == 1)
	{
	    if (myMessages - myErrors > 1)
	    {
		printf ( "1 Error and %d Warnings\n",
		    myMessages - myErrors);
	    }
	    else if (myMessages - myErrors == 1)
	    {
		printf ( "1 Error and 1 Warning\n");
	    }
	    else
	    {
		printf ( "1 Error\n");
	    }
	}
	else
	{
	    if (myMessages - myErrors > 1)
	    {
		printf ( "%d Warnings\n",
		    myMessages - myErrors);
	    }
	    else if (myMessages - myErrors == 1)
	    {
		printf ( "1 Warning\n");
	    }
	}
    } // if (myError != NULL)
    
    if (myErrors == 0)
    {
	//
	// If no errors, execute program
	//    During execution, if reading from stdin, redirect from stdin.
	//    At end of execution, write output to path.out or path.bmp
	//
	
	OOTint		myNumErrors;
	FilePath	myExecutionDirectory;
	int		myFontSize;
	OOTargs		myOOTArgs;
	
	stExecutingWindow = myTextDisplayWindow;
	stAllRunWindowsClosed = TRUE;
	
	// Clear all previous errors (warnings)
	EdErr_ClearErrors ();
	
	strncpy (myOOTArgs [0], stRunArgs.ootArgs [0], 255);    	    			     
	
	Language_SetupExecution (MyGetMaxStackSize (), "", "", myOOTArgs, 0);

	// Set the base source directory and the execution directory to be the
	// directory in which the source file was located.
	EdFile_GetDirectoryFromPath (pmTestFileName, myExecutionDirectory);
	
	myFontSize = gProperties.runConsoleFontSize;
	if (gProperties.runUseSmallFont)
	{
	    RECT	myRect = EdGUI_GetWorkArea ();
	    
	    if ((myRect.right - myRect.left <= 640) ||
		(myRect.bottom - myRect.top <= 480))
	    {
		myFontSize = 9;
	    }
	}
	
	if (!MIO_Init_Run (pmTestFileName, 
		      NULL, 
		      FALSE, 
		      NULL, 
		      FALSE, FALSE,
		      myExecutionDirectory,		// Execution directory
		      FALSE, 				// Graphics Mode
		      gProperties.runConsoleFontName, 	// Run window font name
		      myFontSize, 			// Run window font size
		      0, // Run window font width
		      0, // Run window font options
		      0, // Run window dimensions
		      0, 0, // Run window width and height
		      25, 				// Run window rows
		      80, 				// Run window columns
		      FALSE,
		      (COLOR) RGB (0, 0, 132),
		      TRUE,				// Allow Sys.Exec
		      TRUE,				// Allow Music
		      0,				// PP I/O Port
		      FALSE))				// A test program
	{
	    return;
	}
	
	// Status message    
	stTuringProgramRunning = TRUE;
	stTuringProgramPaused = FALSE;
	stTuringProgramHalting = FALSE;
	stQuittingEnvironment = FALSE;
	stRerunningTuringProgram = FALSE;
	
	stStepping = NO_STEP;
	
	do
	{
	    MyExecuteQuantum (&myNumErrors, NULL);
	    
	    Ed_ProcessWaitingEvents (stTuringProgramWaitingForEvent);
	} while (stRunStatus.state != Finished);
	
	//
	// At this point, the program has either finished executing or the been
	// told to halt (permanently) either by the user closing a run window or
	// pressing stop, etc.
	// 
	// At this point, we've been processing any number of OS messages.
	// This means that the system could have closed any open windows
	// that we refer to.  Check carefully before using them.
	//
	MIO_Finalize_Run ();
	
	// Close all files opened by the running program
	Language_EndExecution ();
	
	stTuringProgramRunning = FALSE;
	
	if (myNumErrors >= 1)
	{
	    WORD	myErrorTuringFileNo;
	    FilePath	myErrorPathName;
	    SrcPosition	*mySrc;
	    
		printf("Runtime Errors:\n");

	    myErrorTuringFileNo = stFirstErrorPtr -> srcPos.fileNo;
	    FileManager_FileName (myErrorTuringFileNo, myErrorPathName);
	    mySrc = &(stFirstErrorPtr -> srcPos);
		
	    if (mySrc -> tokLen > 0)
	    {
		printf ( 
		    "Run Time Error: Line %d [%d - %d] of %s: %s\n",
		    mySrc -> lineNo, mySrc -> linePos + 1,
		    mySrc -> linePos + 1 + mySrc -> tokLen, 
		    EdFile_GetFileName (myErrorPathName), stFirstErrorPtr -> text);
	    }
	    else
	    {
		printf ( 
		    "Run Time Error: Line %d [%d] of %s: %s\n",
		    mySrc -> lineNo, mySrc -> linePos + 1,
		    EdFile_GetFileName (myErrorPathName), stFirstErrorPtr -> text);
	    }
	} // if (myNumErrors >= 1)
    } // Finished execution
    //
    // Close the editor file.
    //
    SendMessage (myEditWindow, WM_CLOSE, 0, 0);

    // Process events to allow windows to close, etc.
    Ed_ProcessWaitingEvents (FALSE);
} // EdRun_RunProgramNoEditor


/************************************************************************/
/* EdRun_ToggleBreakpointVisibility					*/
/*									*/
/* Add a breakpoint if one doesn't already exist on this line, 		*/
/* otherwise remove it.							*/
/************************************************************************/
void	EdRun_ToggleBreakpointVisibility (void)
{
    stBreakpointsVisible = !stBreakpointsVisible;
} // EdRun_ToggleBreakpointVisibility


/************************************************************************/
/* EdRun_ToggleTracing							*/
/************************************************************************/
void	EdRun_ToggleTracing (void)
{
    stTracing = !stTracing;
    if (stTracing)
    {
    	EdRun_SetStep (TRACE_ON);
    }
} // EdRun_ToggleTracing


/****************************/
/* Turing module procedures */
/****************************/
/************************************************************************/
/* AbortCheck								*/
/*									*/
/* Checks if compilation was aborted.  Should theoretically process any	*/
/* waiting messages, but compilation is so quick nowadays that it's not	*/
/* worth the hassle.							*/
/************************************************************************/
OOTboolean	AbortCheck (char *pmString)
{
    return FALSE;
} // AbortCheck


/************************************************************************/
/* FeedBack								*/
/*									*/
/* This routine is called from within the Turing language module to	*/
/* return messages to the user.						*/
/************************************************************************/
void	FeedBack (char *pmString)
{
    if (stCompilingWindow == NULL)
    {
    	HWND	myWindow = Ed_GetLastActiveEditWindow ();
    	
    	if (myWindow != NULL)
    	{
    	    EdWin_ShowStatus (myWindow, pmString);
	}
    }
    else
    {
    	EdWin_ShowStatus (stCompilingWindow, pmString);
    } 
} // FeedBack


/************************************************************************/
/* ErrorFeedBack							*/
/*									*/
/* This routine is called from within the Turing language module to	*/
/* return messages to the user.						*/
/************************************************************************/
void	ErrorFeedBack (char *pmString)
{
    if (stCompilingWindow == NULL)
    {
    	HWND	myWindow = Ed_GetLastActiveEditWindow ();
    	
    	if (myWindow != NULL)
    	{
    	    EdWin_ShowStatus (myWindow, pmString);
	}
    }
    else
    {
    	EdWin_ShowStatus (stCompilingWindow, pmString);
    } 
} // ErrorFeedBack


/************************************************************************/
/* Panic								*/
/*									*/
/* Badness!  The system has a problem it can't resolve!			*/
/************************************************************************/
void	Panic (char *pmString)
{
    EdFail_Fatal (IDS_RUNTIMEPANIC, __FILE__, __LINE__, 0, pmString);
} // Panic


/************************************************************************/
/* Warning								*/
/*									*/
/* This routine is called from within the Turing language module to	*/
/* warn the user about an anamolous situation.				*/
/************************************************************************/
void	Warning (char *pmString)
{
    EdGUI_Message ("Turing Run Time Warning", pmString);   
} // Warning


/************************************************************************/
/* WinAssert								*/
/*									*/
/* Badness!  The system has a problem it can't resolve!			*/
/************************************************************************/
void	WinAssert (int pmAssertionNumber, BOOL pmAssertionCondition, 
		   char *pmFile, int pmLine)
{
    if (!pmAssertionCondition) 
    {
    	EdFail_Fatal (IDS_RUNTIMEASSERTFAIL, pmFile, pmLine, 0, 
    		      pmAssertionNumber);
    }
} // WinAssert


/******************************/
/* Static callback procedures */
/******************************/
/************************************************************************/
/* MyExecutionTimeoutTimerProcedure					*/
/************************************************************************/
static void CALLBACK 	MyExecutionTimeoutTimerProcedure (
				UINT pmID, UINT pmMsg, DWORD pmWindow,
				DWORD pmDummy1, DWORD pmDummy2)
{
    Language_Execute_externalExecutionTimeoutFlag = 1;    
} // MyExecutionTimeoutTimerProcedure


/************************************************************************/
/* MyRunWithArgsDialogProcedure						*/
/************************************************************************/
static UINT CALLBACK 	MyRunWithArgsDialogProcedure (HWND pmDialog, 
						      UINT pmMessage, 
    						      WPARAM pmWParam, 
    						      LPARAM pmLParam)
{
    FilePath		myFileName;
    HWND		myItem;
    char		*myPtr;
    char		myQuote;
    int			myArgument;
    int			myArgPos;
    int			myStartPos, myEndPos;
    static RunArgs	*myStRunArgs;
    
    switch (pmMessage)
    {
    	case WM_INITDIALOG:
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
    	    return TRUE;
    	case WM_COMMAND:
    	    switch (LOWORD (pmWParam))
    	    {
    	    	case IDOK:
    	    	    //
    	    	    // Copy the contents of the controls to the run args struct
    	    	    //    
		    myItem = GetDlgItem (pmDialog, ARGS_INPUT_FILE_NAME);
    	    	    GetDlgItemText (pmDialog, ARGS_COMMAND_LINE_ARGS, 
    	    	        myStRunArgs -> commandLineArguments, 
    	    	        sizeof (myStRunArgs -> commandLineArguments));
    	    	        
    	    	    // Convert the run time arguments into individual arguments
    	    	    myArgument = 0;
    	    	    myArgPos = 0;
    	    	    myPtr = myStRunArgs -> commandLineArguments;
    	    	    while (TRUE)
    	    	    {
    	    	    	// Skip over whitespace
    	    	    	while ((*myPtr == ' ') || (*myPtr == '\t'))
    	    	    	{
    	    	    	    myPtr++;
    	    	    	}
    	    	    	
    	    	    	if (*myPtr == 0) break;
    	    	    	
    	    	    	myStartPos = myPtr - 
    	    	    	    myStRunArgs -> commandLineArguments;
    	    	    
    	    	    	myArgument++;
			myArgPos = 0;

    	    	        // If we start with a quote, convert until the end quote
    	    	        if ((*myPtr == '"') || (*myPtr == '\''))
    	    	        {
    	    	            myQuote = *myPtr;
    	    	            myPtr++;
    	    	            while ((*myPtr != myQuote) && (*myPtr != 0))
    	    	            {
    	    	            	myStRunArgs -> ootArgs [myArgument][myArgPos++] 
    	    	            					= *myPtr++;
				if (myArgPos == 255)
				{				    
				    EdGUI_Message1 (pmDialog, 
				    	MB_ICONEXCLAMATION, 
			    		IDS_BAD_CMD_LINE_ARG_TITLE, 
			    		IDS_ARG_TOO_LONG);
			    	    myEndPos = myPtr - 
			    	        myStRunArgs -> commandLineArguments;
				    SetFocus (myItem);
		    	    	    SendMessage (myItem, EM_SETSEL, 
		    	    	    	(WPARAM) myStartPos, (LPARAM) myEndPos);
		    	    	    return FALSE;
				}    	    	            					
    	    	            }
    	    	            if (*myPtr == 0)
    	    	            {
				EdGUI_Message1 (pmDialog, MB_ICONEXCLAMATION, 
			    	    IDS_BAD_CMD_LINE_ARG_TITLE, 
			    	    IDS_UNTERMINATED_QUOTE);
			    	myEndPos = myPtr - 
			    	        myStRunArgs -> commandLineArguments;
				SetFocus (myItem);
		    	    	SendMessage (myItem, EM_SETSEL, 
		    	    	    (WPARAM) myStartPos, (LPARAM) myEndPos);
		    	    	return FALSE;
    	    	            }
    	    	            myPtr++;
    	    	        }
    	    	        else
    	    	        {
    	    	    	    while ((*myPtr != ' ') && (*myPtr != '\t') && 
    	    	    	    	   (*myPtr != 0))
    	    	    	    {
    	    	            	myStRunArgs -> ootArgs [myArgument][myArgPos++] 
    	    	            					= *myPtr++;
				if (myArgPos == 255)
				{
				    EdGUI_Message1 (pmDialog, 
				    	MB_ICONEXCLAMATION, 
			    		IDS_BAD_CMD_LINE_ARG_TITLE, 
			    		IDS_ARG_TOO_LONG);
			    	    myEndPos = myPtr - 
			    	        myStRunArgs -> commandLineArguments;
				    SetFocus (myItem);
		    	    	    SendMessage (myItem, EM_SETSEL, 
		    	    	    	(WPARAM) myStartPos, (LPARAM) myEndPos);
		    	    	    return FALSE;
				}    	    	            					
    	    	    	    } 
    	    	    	}
    	    	    } // while (TRUE)
    	    	    
    	    	    myStRunArgs -> numArgs = myArgument;
    	    	    
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
    	    	    EndDialog (pmDialog, TRUE);
    	    	    return TRUE;
    	    	case IDCANCEL:
    	    	    EndDialog (pmDialog, FALSE);
    	    	    return TRUE;
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

    						      
/************************************************************************/
/* MyTimerProcedure							*/
/************************************************************************/
static void CALLBACK 	MyTimerProcedure (UINT pmID, UINT pmMsg, DWORD pmWindow,
					  DWORD pmDummy1, DWORD pmDummy2)
{
    PostMessage ((HWND) pmWindow, WM_TIMER, TURING_TIMER, (LPARAM) NULL);
} // MyTimerProcedure

// Writes bytecode to a file. Used for EXE creation and writing bytecode files
static int	MyWriteByteCode (FileNoType pmProgramFileNo, long pmTuringFileDesciptor, 
			 BOOL pmCloseWindowsOnTerminate, 
			 BOOL pmDisplayRunWithArgs, BOOL pmCenterOutputWindow, 
			 BOOL pmStopUserClose)
{
	OOTint			myStatus = -1;
	TuringErrorPtr	myError;
	int			myErrors;
	//
    // First write the header.
    //
    TL_TLI_TLIWR (OBJECT_FILE_HEADER, sizeof (OBJECT_FILE_HEADER), &myStatus,
    		  pmTuringFileDesciptor);
    if (myStatus != 0)
    {
    	return 1;
    }

    //
    // Then write TProlog specific preferences      
    //
    TL_TLI_TLIWR (&pmCloseWindowsOnTerminate, sizeof (BOOL), &myStatus,
    		  pmTuringFileDesciptor);
    if (myStatus != 0)
    {
    	return 1;
    }
    TL_TLI_TLIWR (&pmDisplayRunWithArgs, sizeof (BOOL), &myStatus,
    		  pmTuringFileDesciptor);
    if (myStatus != 0)
    {
    	return 1;
    }
    TL_TLI_TLIWR (&pmCenterOutputWindow, sizeof (BOOL), &myStatus,
    		  pmTuringFileDesciptor);
    if (myStatus != 0)
    {
    	return 1;
    }
    TL_TLI_TLIWR (&pmStopUserClose, sizeof (BOOL), &myStatus,
    		  pmTuringFileDesciptor);
    if (myStatus != 0)
    {
    	return 1;
    }
    
    // 
    // Then write the environment preferences
    //
    TL_TLI_TLIWR (&gProperties, sizeof (Properties), &myStatus,
    		  pmTuringFileDesciptor);
    if (myStatus != 0)
    {
    	return 1;
    }
    
    //
    // Then write another header (just to make certain we're synced)
    //
    TL_TLI_TLIWR (OBJECT_FILE_HEADER, sizeof (OBJECT_FILE_HEADER), &myStatus,
    		  pmTuringFileDesciptor);
    if (myStatus != 0)
    {
        return 1;
    }

    // 
    // Then write the object files
    //
    EdRun_ResetCompiler ();
    Language_WriteObjectFile ("", pmProgramFileNo, &myError, &myErrors,
    	pmTuringFileDesciptor);
    EdRun_ResetCompiler ();

	return 0;
}

/*********************/
/* Static procedures */
/*********************/
/************************************************************************/
/*	static void	MyExecuteQuantum (OOTint *pmNumErrors)		*/
/*									*/
/*    Going into the execution loop, we could be doing one of several	*/
/* things:								*/
/*	1) Just plain executing						*/
/*	2) Doing a trace execution					*/
/*	3) Debugging							*/
/*	4) Doing a step of some sort					*/
/************************************************************************/
static void	MyExecuteQuantum (OOTint *pmNumErrors, HWND pmWindow)
{
    int			myReasonForBreak = NO_BREAK;
    BOOL		myFound;
    MMRESULT		myTimer;
    static BOOL		stEventWaitingMessageDisplayed = FALSE;

    // If we're tracing or if there are break points set and we're not doing
    // a step, then step into a call.  If we are doing a step, the 
    // Language_SetStep has aleady been called appropriately and we're going
    // to ignore the trace and possible breakpoints.  
    //
    // TW - If we were a little more sophisticated, we would always step into
    // a call if tracing or a breakpoint was set, we would just know when to 
    // stop to emulate a step over or a step return (perhaps by doing a 
    // temporary breakpoint?)
    if ((stTracing || (stBreakpointsVisible)) && (stStepping == NO_STEP))
    {
	Language_SetStep (Language_Step_StepIntoCall);
    }

    // The following lines are used to set a timer so that Turing will
    // resurface to check for events at least every 100 milliseconds.
    Language_Execute_externalExecutionTimeoutFlag = 0;
    myTimer = timeSetEvent (MSECS_BETWEEN_EVENT_CHECK, stMinimumEventPeriod, 
	    	  	    MyExecutionTimeoutTimerProcedure, 0, 
	    	  	    TIME_ONESHOT | TIME_CALLBACK_FUNCTION);
    
    // Execute a quantum of code
    Language_ExecuteProgram (&stRunStatus, &stFirstErrorPtr, pmNumErrors);

    // If the timer is still going, kill it.
    timeKillEvent (myTimer);
    
    // There should be no way the turing program is paused since we've just
    // executed a line and haven't checked for events yet.  This just 
    // guarantees it
    if (stTuringProgramPaused)
    {
    	EdFail_Fatal (IDS_PAUSE_FAILURE, __FILE__, __LINE__, 0, 
    		      stTuringProgramRunning, stTracing, stStepping, 
    		      stBreakpointsVisible, stRunStatus.state);
    }

    // After the execution, stRunStatus.state is one of:
    //       	Finished - 	Program finished execution
    //		Hiccup -	Program came up to handle events
    //		EventWait -	All threads are waiting for an event
    //		Stopped -	Program hit a break statement
    //		NextLine -	Return from a step command
    //		ContextSwap -	Return from a step command (Switching threads)
    //		KeyboardEventWait - All threads are waiting for keyboard input
    //		MouseEventWait - All threads are waiting for mouse input
    //		DelayEventWait - All threads are waiting until time t

    stTuringProgramWaitingForEvent = 
    				((stRunStatus.state == KeyboardEventWait) ||
    				 (stRunStatus.state == MouseEventWait) ||
    				 (stRunStatus.state == DelayEventWait));
    if (stRunStatus.state == DelayEventWait)
    {
    	stDelayWait = stRunStatus.moreStateInfo - MIOTime_GetTicks () - 
    		      MINIMUM_DELAY - stMinimumPeriod;
    	if (stDelayWait <= 0) 
    	{
    	    stTuringProgramWaitingForEvent = FALSE; 
    	    stDelayWait = 0;
    	}
    }
    else
    {
    	stDelayWait = 0;
    }
    
    //
    // Put up event waiting message in the debugger controller, if appropriate
    //
    if (stRunStatus.state == KeyboardEventWait)
    {
	if (!stEventWaitingMessageDisplayed)
	{
	    stEventWaitingMessageDisplayed = TRUE;
	    Ed_SendStatusMessageToAllWindows (
	    				"Waiting: waiting for keyboard input");
	}   
    }
    else if (stRunStatus.state == MouseEventWait)
    {
	if (!stEventWaitingMessageDisplayed)
	{
	    stEventWaitingMessageDisplayed = TRUE;
	    Ed_SendStatusMessageToAllWindows (
	    				"Waiting: waiting for mouse click");
	}   
    }
    else
    {
	if (stEventWaitingMessageDisplayed)
	{
	    stEventWaitingMessageDisplayed = FALSE;

	    if (stTracing)
	    {
	    	Ed_SendStatusMessageToAllWindows ("Executing: with Trace");
	    }
	    else
	    {
	    	Ed_SendStatusMessageToAllWindows ("Executing");
	    }
	}   
    }

    //
    // Here we check for reasons to stop.  At this point, stTuringProgramPaused 
    // would only be true if the user pressed the Pause button, in which case,
    // even if there was another reason for pausing (for example, hitting a 
    // break statement or a break point) we ignore the other reason.
    //
    
    // Check if we've hit a 'break' keyword
    if (stRunStatus.state == Stopped)
    {
	myReasonForBreak = HIT_BREAK_STATEMENT;
    }
	
    // Check if we've hit a breakpoint
    if (stBreakpointsVisible && (stRunStatus.srcPos.fileNo != 0))
    {
	FileNoType	myFile = stRunStatus.srcPos.fileNo;
	int		myLine = stRunStatus.srcPos.lineNo - 1;
    	HWND		myTextDisplayWindow = EdRun_MatchTuringFileNo (myFile);

    	if (myTextDisplayWindow != NULL)
    	{
	    //
	    // Go through all the breakpoints in this window.  If there's a 
	    // unit breakpoint (indicated by the line number being
	    // UNIT_BREAK) and we've just entered the unit, then break.  
	    // Otherwise break if the line number we are on matches a break
	    // line number in the window.
	    //
    	    TextPtr		myTextPtr;
	    BreakLinePtr	myBreak;

	    myTextPtr = EdDisp_GetTextPtr (myTextDisplayWindow);
	    myBreak = myTextPtr -> breakLine;
       	    if ((myBreak != NULL) && (myBreak -> line == UNIT_BREAK) &&
		(myFile != stFileNoLastExecuted))
       	    {
	        myReasonForBreak = HIT_UNIT_BREAK_POINT;
	    }
	    else
	    {
	    	while (myBreak != NULL)
	    	{
	    	    if (myBreak -> line >= myLine)
	    	    {
	    	        if (myBreak -> line == myLine)
	    	    	{
			    myReasonForBreak = HIT_BREAK_POINT;
			}
	    	    	break;
		    }
	    	    myBreak = myBreak -> next;
	    	} // while
	    } // else - if not a unit break
        } // if matching window found
    } // if (stBreakpointsVisible)
	
    // This means that if a b.p. is set on a unit while we're in the unit,
    // it won't cause a halt until execution has left and then re-entered 
    // the unit.
    stFileNoLastExecuted = stRunStatus.srcPos.fileNo;

    // If we've hit a breakpoint, then we pause at that location
    if (myReasonForBreak != NO_BREAK)
    {
	// Pause the program with the appropriate error message
	EdRun_PauseResumeProgram (ACTIVATE_SOURCE_WINDOW, myReasonForBreak);

	// We've finished the step, now turn it off.
	stStepping = NO_STEP;
	    
	// If we're hitting breakpoints, then make the 
	// debugger controller visible
	if (!EdBBar_AreDebuggerControlsVisible ())
	{
	    Ed_ToggleDebuggerControlsVisibility ();
	}
	
	return;
    }
    
    //
    // At this point, if the user paused the program or we hit a break
    // statement or a break point we are paused and stTuringProgramPaused 
    // is true
    //

    // If we're either stepping or tracing and the state is either a
    // context swap or next line, then we want to hilight the line.
    if ((stTracing || (stStepping != NO_STEP)) && 
    	(stRunStatus.srcPos.lineNo > 0) &&
    	((stRunStatus.state == ContextSwap) || (stRunStatus.state == NextLine)))
    {
	if (stStepping != NO_STEP)
	{
	    Language_SetStep (Language_Step_None);
	    
	    // Pause the program with the appropriate error message
	    EdRun_PauseResumeProgram (ACTIVATE_SOURCE_WINDOW, stStepping);
	    			      
	    stStepping = NO_STEP;
	}
	else
	{
	    char	myPathName [1024];
	    
	    if (!IsWindow (pmWindow))
	    {
		pmWindow = Ed_GetLastActiveEditWindow ();
	    }

    	    FileManager_FileName (stRunStatus.srcPos.fileNo, myPathName);
	    myFound = EdErr_HilightLine (pmWindow, myPathName, 
	    		  stRunStatus.srcPos.fileNo,
	    		  stRunStatus.srcPos.lineNo, TRUE, NULL);
	    
	    if (!myFound)
	    {	    		  
	        // Could not open a file by that name.  Display the error 
	        // message in the current window instead.
    	    	if ((strncmp (myPathName, "unnamed", 7) != 0) &&
		    (strcmp (myPathName, "<No file>") != 0))
	    	{
		    EdWin_ShowStatus (EdGUI_GetTopWindow (pmWindow),
		        "Executing: Line %d of \"%s\"", 
		        stRunStatus.srcPos.lineNo + 1, myPathName);
	    	}
	    	else
	    	{
		    EdWin_ShowStatus (
		        EdGUI_GetTopWindow (pmWindow),
		            "Executing: Line %d of Unnamed File", 
		            stRunStatus.srcPos.lineNo + 1);
	        }
	    }
	}
	    
	// If we're tracing, delay an appropriate amount
	if (stTracing)
	{
	    stTraceDelayTimer = EdGUI_GetTicks ();
	    stTraceDelayTimer += stTraceSpeed * 16;
	    if (stTraceSpeed > 33)
	    {
	        stTraceDelayTimer += (stTraceSpeed - 33) * 16;
	    }
	    if (stTraceSpeed > 66)
	    {
		stTraceDelayTimer += (stTraceSpeed - 66) * 16;
	    }
	} // if (stTracing)
    } // if
    
//    Dbg_UpdateVariables (&stRunStatus);
} // MyExecuteQuantum


/************************************************************************/
/* MyGetMaxStackSize							*/
/************************************************************************/
static int	MyGetMaxStackSize (void)
{
    #define MIN_STACK_SIZE_IN_KB	64
    #define MAX_STACK_SIZE_IN_KB	8192
    
    if (gProperties.turingStackSizeInKB > 0)
    {
    	return max (MIN_STACK_SIZE_IN_KB, gProperties.turingStackSizeInKB);
    }
    else
    {
    	MEMORYSTATUS	myBuffer;
    	
    	GlobalMemoryStatus (&myBuffer);
    	
    	return max (MIN_STACK_SIZE_IN_KB, 
    		    min (MAX_STACK_SIZE_IN_KB,
    			 myBuffer.dwAvailVirtual / (2 * 1024)));
    }    			       
} // MyGetMaxStackSize


/************************************************************************/
/* MyGetRunArgs								*/
/************************************************************************/
static BOOL	MyGetRunArgs (HWND pmWindow, const char *pmPathName, 
			      int pmProgramKind, int *pmAppletWidth,
			      int *pmAppletHeight, char *pmAppletParams)
{
    HINSTANCE	myApplicationInstance;

    // Get the program instance
    myApplicationInstance = 
        (HINSTANCE) GetWindowLong (pmWindow, GWL_HINSTANCE);
    
    if (!DialogBoxParam (myApplicationInstance, 
            MAKEINTRESOURCE (TURING_RUN_WITH_ARGS_DIALOG),
	    pmWindow, MyRunWithArgsDialogProcedure, (LPARAM) &stRunArgs))
    {
	return FALSE;
    }
    
    return TRUE;
} // MyGetRunArgs

    
/************************************************************************/
/* MyHilightLine							*/
/************************************************************************/
static void	MyHilightLine (const char *pmMessage, SRCPOS *pmSrcPos)
{
    FileNoType	myErrorTuringFileNo;
    FilePath	myErrorPathName;
    SrcPosition	mySrcPos;
    
    if (pmSrcPos -> fileNo == 0)
    {
	struct Language_ThreadQueues	*myThreadQueues;

	Language_DumpThreadList (&myThreadQueues);
	mySrcPos = myThreadQueues->threadList->traceback->srcPos;
    }
    else
    {
	mySrcPos = *pmSrcPos;
    }

    myErrorTuringFileNo = mySrcPos.fileNo;
    FileManager_FileName (myErrorTuringFileNo, myErrorPathName);
    EdErr_ClearErrors ();
    EdErr_AddError (myErrorPathName, myErrorTuringFileNo,
        	    mySrcPos.lineNo, 1, mySrcPos.lineNo, 0, 
		    "Paused at this line");
    EdErr_FindFirstError (stCompilingWindow, NO_MESSAGE_IF_NO_ERROR,
			  TRUE);
} // MyHilightLine


/************************************************************************/
/* MyMatchPathName							*/
/************************************************************************/
static void	MyMatchPathName (char *pmPathName, HWND pmEditWindow, 
		    		 HWND pmTextDisplayWindow, TextPtr pmTextPtr, 
				 void *pmMatch)
{
    unsigned int	myErrorFileNo, myPathFileNo;
    const char		*myErrorPathName, *myPathName;    
    
    myErrorFileNo = ((MatchWindow *) pmMatch) -> fileNo;
    if (myErrorFileNo != 0)
    {
    	myPathFileNo = EdDisp_GetTuringFileNo (pmTextDisplayWindow);

    	if (myErrorFileNo == myPathFileNo)
    	{
    	    ((MatchWindow *) pmMatch) -> textPtr = pmTextPtr;
    	    ((MatchWindow *) pmMatch) -> textWindow = pmTextDisplayWindow;
    	}
    }
    else
    {
    	myErrorPathName = ((MatchWindow *) pmMatch) -> pathName;
    	myPathName = EdDisp_GetPath (pmTextDisplayWindow);

	if ((myPathName != NULL) &&
    	    (_stricmp (myErrorPathName, myPathName) == 0))
    	{
    	    ((MatchWindow *) pmMatch) -> textPtr = pmTextPtr;
    	    ((MatchWindow *) pmMatch) -> textWindow = pmTextDisplayWindow;
	}
    }    	    
} // MyMatchPathName


/************************************************************************/
/* MyRunTestSuiteProgram						*/
/*									*/
/* Set up the predefined symbols					*/
/************************************************************************/
static void	MyRunTestSuiteProgram (const char *pmTestDirectory, 
				       const char *pmOutputDirectory,
				       const char *pmTestFileName,
				       int pmLineNo, char *pmIndexPath)
{
    FilePath		myTestPathName, mySourceDirectory, myErrorPath;
    HWND		myEditWindow, myTextDisplayWindow;
    TuringErrorPtr	myError;    
    FileEntryPtr	myErrorFileEntry;
    const char		*myPtr;
    int			myErrors;

    // Create the test file path
    strcpy (myTestPathName, pmTestDirectory);
    if (myTestPathName [strlen (myTestPathName) - 1] != '\\')
    {
	strcat (myTestPathName, "\\");
    }
    strcat (myTestPathName, pmTestFileName);
    
    // Create the error file path
    strcpy (myErrorPath, pmOutputDirectory);
    if (myErrorPath [strlen (myErrorPath) - 1] != '\\')
    {
	strcat (myErrorPath, "\\");
    }
    strcat (myErrorPath, pmTestFileName);
    myPtr = EdFile_GetFileSuffix (myErrorPath);
    myErrorPath [myPtr - myErrorPath] = 0;
    strcat (myErrorPath, ".err");

    // Make certain the test file exists
    if (!EdFile_FileExists (myTestPathName))
    {
	// Test file not found
	EdGUI_Message1 (NULL, MB_ICONEXCLAMATION, 
	    IDS_TEST_SUITE_ERROR_TITLE, 
	    IDS_TEST_SUITE_FILE_DOESNT_EXIST, pmLineNo,
	    pmIndexPath, myTestPathName);
	return;
    }
    
    // Make certain output directory is specified
    if (pmOutputDirectory [0] == 0)
    {
	EdGUI_Message1 (NULL, MB_ICONEXCLAMATION, 
	    IDS_TEST_SUITE_ERROR_TITLE, 
	    IDS_TEST_SUITE_NO_OUTPUT_DIR, pmLineNo,
	    pmIndexPath, myTestPathName);
	return;
    }
    
    // Create the output directories as necessary
    if (!EdFile_CreateDirectoriesIfNecessary (pmOutputDirectory))
    {
	EdGUI_Message1 (NULL, MB_ICONEXCLAMATION, 
	    IDS_TEST_SUITE_ERROR_TITLE, 
	    IDS_TEST_SUITE_NO_CREATE_DIR, pmLineNo,
	    pmIndexPath, pmOutputDirectory);
	return;
    }
    
    //
    // Open the file in the editor
    // Compile the file
    // Write extant errors to path.err
    // If no errors, execute program
    //    During execution, if reading from stdin, redirect from stdin.
    //    At end of execution, write output to path.out or path.bmp
    // Close the editor file.
    //
    
    // Open the file in the editor.
    myEditWindow = EdWin_Create (myTestPathName, NULL, 0, FALSE, FALSE);
    if (myEditWindow == NULL)
    {
	EdGUI_Message1 (NULL, 0, IDS_TEST_SUITE_ERROR_TITLE,
	    IDS_TEST_SUITE_UNABLE_TO_OPEN_WINDOW, pmLineNo,
	    pmIndexPath, myTestPathName);
	return;
    }
    myTextDisplayWindow = EdWin_GetActiveDisplayWindow (myEditWindow);
    
    //
    // Compile the file
    //
    
    stSourceFileNo = EdDisp_GetTuringFileNo (myTextDisplayWindow);
    EdDisp_GetPathForDisplay (myTextDisplayWindow, USE_FILENAME, 
	DONT_SHOW_DIRTY_MARK, stSourceFileName);
    if (EdDisp_GetPath (myTextDisplayWindow) == NULL)
    {
	// We need to make this a full path name
	EdFile_GetActiveDirectory (stSourcePathName);
	if (!EdGlob_EndsWith (stSourcePathName, "\\"))
	{
	    strcat (stSourcePathName, "\\");
	}
	strcat (stSourcePathName, stSourceFileName);
    }
    else
    {
	strcpy (stSourcePathName, EdDisp_GetPath (myTextDisplayWindow));
    }
    
    strncpy (stRunArgs.ootArgs [0], stSourcePathName, 255);    	    			     
    
    // Set the base source directory to be the directory 
    // in which the source file was located.
    EdFile_GetDirectoryFromPath (stSourcePathName, mySourceDirectory);
    FileManager_ChangeDirectory ((OOTstring) mySourceDirectory);
    
    // Now, for every open file, set the OOT file manager pointer to 
    // point to it and mark it modified
    Ed_EnumerateFiles (MySetTuringPointers, NULL);
    
    stCompilingWindow = myTextDisplayWindow;
    
    // Clear all previous errors
    EdErr_ClearErrors ();
    
    // Free any previously allocated memory.  This could take a while if 
    // someone allocated a huge amount
    EdRun_InitializeForRunWithoutCompile (myTextDisplayWindow);
    
    // Close any run windows and free any allocated objects in debugger		      
    MIO_Init_Free ();
    
    // Compile the program
    EdWin_ShowStatus (myTextDisplayWindow, "Initializing Compiler");
    Language_CompileProgram ("", stSourceFileNo, &myError, &myErrors);
    
    //
    // Write extant errors to path.err
    //
    
    if (myError != NULL)
    {
	int		myErrorCode;
	int		myMessages = 0;
	
	myErrorFileEntry = EdFile_OpenFileForWrite (myErrorPath,
	    &myErrorCode, BUFFER_OUTPUT, 
	    CREATE_NEW_FILE);
	
	if (myErrorFileEntry == NULL)
	{
	    EdGUI_Message1 (NULL, 0, IDS_TEST_SUITE_ERROR_TITLE,
		IDS_TEST_SUITE_UNABLE_TO_OPEN_ERR_FILE, pmLineNo,
		pmIndexPath,  myErrorPath, myErrorCode);
	    return;
	}
	
	while (myError != NULL)
	{
	    WORD	myErrorTuringFileNo;
	    FilePath	myErrorPathName;
	    SrcPosition	*mySrc;
	    
	    myErrorTuringFileNo = myError -> srcPos.fileNo;
	    FileManager_FileName (myErrorTuringFileNo, myErrorPathName);
	    mySrc = &(myError -> srcPos);
	    
	    if (mySrc -> tokLen > 0)
	    {
		EdFile_Writeln (myErrorFileEntry, 
		    "Line %d [%d - %d] of %s: %s",
		    mySrc -> lineNo, mySrc -> linePos + 1,
		    mySrc -> linePos + 1 + mySrc -> tokLen, 
		    EdFile_GetFileName (myErrorPathName), myError -> text);
	    }
	    else
	    {
		EdFile_Writeln (myErrorFileEntry, 
		    "Line %d [%d] of %s: %s",
		    mySrc -> lineNo, mySrc -> linePos + 1,
		    EdFile_GetFileName (myErrorPathName), myError -> text);
	    }
	    myError = myError -> next;
	    myMessages++;
	} // while (myError != NULL)
	
	EdFile_WriteNL (myErrorFileEntry);

	// Add a message about the number of errors and warnings
	if (myErrors > 1)
	{
	    if (myMessages - myErrors > 1)
	    {
		EdFile_Writeln (myErrorFileEntry, "%d Errors and %d Warnings",
		    myErrors, myMessages - myErrors);
	    }
	    else if (myMessages - myErrors == 1)
	    {
		EdFile_Writeln (myErrorFileEntry, "%d Errors and 1 Warning",
		    myErrors);
	    }
	    else
	    {
		EdFile_Writeln (myErrorFileEntry, "%d Errors", myErrors);
	    }
	}
	else if (myErrors == 1)
	{
	    if (myMessages - myErrors > 1)
	    {
		EdFile_Writeln (myErrorFileEntry, "1 Error and %d Warnings",
		    myMessages - myErrors);
	    }
	    else if (myMessages - myErrors == 1)
	    {
		EdFile_Writeln (myErrorFileEntry, "1 Error and 1 Warning");
	    }
	    else
	    {
		EdFile_Writeln (myErrorFileEntry, "1 Error");
	    }
	}
	else
	{
	    if (myMessages - myErrors > 1)
	    {
		EdFile_Writeln (myErrorFileEntry, "%d Warnings",
		    myMessages - myErrors);
	    }
	    else if (myMessages - myErrors == 1)
	    {
		EdFile_Writeln (myErrorFileEntry, "1 Warning");
	    }
	}

	EdFile_CloseFile (myErrorFileEntry);
    } // if (myError != NULL)
    
    if (myErrors == 0)
    {
	//
	// If no errors, execute program
	//    During execution, if reading from stdin, redirect from stdin.
	//    At end of execution, write output to path.out or path.bmp
	//
	
	OOTint		myNumErrors;
	FilePath	myExecutionDirectory;
	int		myFontSize;
	OOTargs		myOOTArgs;
	
	stExecutingWindow = myTextDisplayWindow;
	stAllRunWindowsClosed = TRUE;
	
	// Clear all previous errors (warnings)
	EdErr_ClearErrors ();
	
	strncpy (myOOTArgs [0], stRunArgs.ootArgs [0], 255);    	    			     
	
	Language_SetupExecution (MyGetMaxStackSize (), "", "", myOOTArgs, 0);

	// Set the base source directory and the execution directory to be the
	// directory in which the source file was located.
	EdFile_GetDirectoryFromPath (stSourcePathName, myExecutionDirectory);
	
	myFontSize = gProperties.runConsoleFontSize;
	if (gProperties.runUseSmallFont)
	{
	    RECT	myRect = EdGUI_GetWorkArea ();
	    
	    if ((myRect.right - myRect.left <= 640) ||
		(myRect.bottom - myRect.top <= 480))
	    {
		myFontSize = 9;
	    }
	}
	
	if (!MIO_Init_Run (stSourceFileName, 
		      pmTestDirectory, 
		      FALSE, 
		      pmOutputDirectory, 
		      FALSE, FALSE,
		      myExecutionDirectory,		// Execution directory
		      FALSE, 				// Graphics Mode
		      gProperties.runConsoleFontName, 	// Run window font name
		      myFontSize, 			// Run window font size
		      0, // Run window font width
		      0, // Run window font options
		      0, // Run window dimensions
		      0, 0, // Run window width and height
		      25, 				// Run window rows
		      80, 				// Run window columns
		      FALSE,
		      (COLOR) RGB (0, 0, 132),
		      TRUE,				// Allow Sys.Exec
		      TRUE,				// Allow Music
		      0,				// PP I/O Port
		      TRUE))				// A test program
	{
	    return;
	}
	
	// Status message    
	stTuringProgramRunning = TRUE;
	stTuringProgramPaused = FALSE;
	stTuringProgramHalting = FALSE;
	stQuittingEnvironment = FALSE;
	stRerunningTuringProgram = FALSE;
	
	stStepping = NO_STEP;
	
	do
	{
	    MyExecuteQuantum (&myNumErrors, NULL);
	    
	    Ed_ProcessWaitingEvents (stTuringProgramWaitingForEvent);
	} while (stRunStatus.state != Finished);
	
	//
	// At this point, the program has either finished executing or the been
	// told to halt (permanently) either by the user closing a run window or
	// pressing stop, etc.
	// 
	// At this point, we've been processing any number of OS messages.
	// This means that the system could have closed any open windows
	// that we refer to.  Check carefully before using them.
	//
	
	MIO_Finalize_Run ();
	
	// Close all files opened by the running program
	Language_EndExecution ();
	
	stTuringProgramRunning = FALSE;
	
	if (myNumErrors >= 1)
	{
	    int		myErrorCode;
	    WORD	myErrorTuringFileNo;
	    FilePath	myErrorPathName;
	    SrcPosition	*mySrc;
	    
	    myErrorFileEntry = EdFile_OpenFileForWrite (myErrorPath,
		&myErrorCode, BUFFER_OUTPUT, APPEND_TO_FILE);
	    
	    if (myErrorFileEntry == NULL)
	    {
		EdGUI_Message1 (NULL, 0, IDS_TEST_SUITE_ERROR_TITLE,
		    IDS_TEST_SUITE_UNABLE_TO_OPEN_ERR_FILE, pmLineNo,
		    pmIndexPath,  myErrorPath, myErrorCode);
		return;
	    }
	    
	    myErrorTuringFileNo = stFirstErrorPtr -> srcPos.fileNo;
	    FileManager_FileName (myErrorTuringFileNo, myErrorPathName);
	    mySrc = &(stFirstErrorPtr -> srcPos);
	    if (mySrc -> tokLen > 0)
	    {
		EdFile_Writeln (myErrorFileEntry, 
		    "Run Time Error:\n    Line %d [%d - %d] of %s: %s",
		    mySrc -> lineNo, mySrc -> linePos + 1,
		    mySrc -> linePos + 1 + mySrc -> tokLen, 
		    EdFile_GetFileName (myErrorPathName), myError -> text);
	    }
	    else
	    {
		EdFile_Writeln (myErrorFileEntry, 
		    "Run Time Error:\n    Line %d [%d] of %s: %s",
		    mySrc -> lineNo, mySrc -> linePos + 1,
		    EdFile_GetFileName (myErrorPathName), myError -> text);
	    }
	    	    
	    EdFile_CloseFile (myErrorFileEntry);
	} // if (myNumErrors >= 1)
    } // Finished execution
    
    //
    // Close the editor file.
    //
    SendMessage (myEditWindow, WM_CLOSE, 0, 0);

    // Process events to allow windows to close, etc.
    Ed_ProcessWaitingEvents (FALSE);
} // MyRunTestSuiteProgram


/************************************************************************/
/* MySetTuringPointers							*/
/*									*/
/* Set up the predefined symbols					*/
/************************************************************************/
static void	MySetPreprocessorSymbols (void)
{
    OOTargs	myPredefSymbols;
    
    strcpy (myPredefSymbols [INTERP_NUM], INTERP_STRING);
    strcpy (myPredefSymbols [GRAPHICS_NUM], GRAPHICS_STRING);
    strcpy (myPredefSymbols [WINTURING_NUM], WINTURING_STRING);
    strcpy (myPredefSymbols [WINDOWS_NUM], WINDOWS_STRING);
	strcpy (myPredefSymbols [OPENTURING_NUM], OPENTURING_STRING);
    Language_EnterPreprocSymbols (myPredefSymbols, NUM_DEFAULT_PREDEFS);
} // MySetPreprocessorSymbols


/************************************************************************/
/* MySetTuringPointers							*/
/************************************************************************/
static void	MySetTuringPointers (const char *pmPathName, HWND pmEditWindow,
				 HWND pmTextDisplayWindow, TextPtr pmTextPtr,
				 void *pmDummy)
{
    // First, add 0 to end of file (there's always allocated room
    pmTextPtr -> text [pmTextPtr -> textLen] = 0;
    
    // Now set the turing pointers to the file size and text handle
    *EdDisp_GetTuringSizePtr (pmTextDisplayWindow) = pmTextPtr -> textLen + 1;
    *EdDisp_GetTuringTextHandle (pmTextDisplayWindow) = pmTextPtr -> text;
    if (pmTextPtr -> isChangedSinceLastRun)
    {
    	FileManager_SetModified (EdDisp_GetTuringFileNo (pmTextDisplayWindow));
    	pmTextPtr -> isChangedSinceLastRun = FALSE;
    }
} // MySetTuringPointers


/************************************************************************/
/* MyStatusExecuting							*/
/************************************************************************/
static void	MyStatusExecuting (const char *pmClassName, int pmStage)
{
    char		myStatusMessage [1024];
    static unsigned int	myStFileBeingCompiledSize = 0;

    if (pmClassName == NULL)
    {
	pmClassName = "Program";
    }

    if (strlen (pmClassName) > myStFileBeingCompiledSize)
    {
	if (stFileBeingCompiled != NULL)
	{
	    free (stFileBeingCompiled);
	}
	
	stFileBeingCompiled = malloc (strlen (pmClassName) + 1);
	myStFileBeingCompiledSize = strlen (pmClassName);
    }
    strcpy (stFileBeingCompiled, pmClassName);
    
    if (pmStage == PREPARING_EXECUTION)
    {
    	wsprintf (myStatusMessage, "Preparing %s for execution", 
    		  stFileBeingCompiled);
    }
    else if (pmStage == STARTING_EXECUTION)
    {
    	wsprintf (myStatusMessage, "Executing %s", stFileBeingCompiled);
    }
    else
    {
    	wsprintf (myStatusMessage, "%s finished execution", 
    		  stFileBeingCompiled);
    }
    // It's possible that the compiling window could be closed by this
    // point, so check the validity of the window.
    if (IsWindow (stCompilingWindow))
    {
	EdWin_ShowStatus (stCompilingWindow, myStatusMessage);
    }
} // MyStatusExecuting


