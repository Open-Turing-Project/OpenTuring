/*************/
/* tprolog.c */
/*************/

/*******************/
/* System includes */
/*******************/
#include <windows.h>
#include "stdio.h"

/****************/
/* Self include */
/****************/
#include "tprolog.h"

/******************/
/* Other includes */
/******************/
#include "tprologdialogs.h"

#include "edprog.h"

#include "edfail.h"
#include "edfile.h"
#include "edgui.h"
#include "edprint.h"

#include "language.h"

#include "mio.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/

/********************/
/* Global variables */
/********************/
// The globals are constants for the entire program and include 
// registration information, etc.
Globals		gProgram;
// The user preferences.
Properties	gProperties;
BOOL	 	gFileManagerIgnoreFileOperation = FALSE;

/*********/
/* Types */
/*********/
typedef struct SrcPosition		SrcPosition;
typedef struct Language_ErrMsgs		*TuringErrorPtr;
typedef struct Language_RunStatus	RunStatus;
typedef struct RunArgs
{
    char	commandLineArguments [4096];
    OOTargs	ootArgs;
    int		numArgs;
    int		inputRedirection;
    int		outputRedirection;
    FilePath	inputFile, outputFile;
} RunArgs;
typedef struct RunSettings
{
    int		status;
    BOOL	closeWindowsOnTerminate;
    BOOL	displayRunWithArgs;
    BOOL	centerOutputWindow;
    BOOL	stopUserClose;
} RunSettings;

/**********************/
/* External variables */
/**********************/
extern void	TL (void);
extern void 	Language_SetupExecutionFromObjectFile (OOTint objectFileStream, 
		    unsigned char debug, unsigned long maxStackSize, 
		    TLstring inName, TLstring outName, OOTargs args, 
		    OOTint numArgs);
extern void	Language_GetFileName (unsigned short fileNo, TLstring str);
extern int	MIOTime_GetTicks (void);

/********************/
/* Static constants */
/********************/
// Just a random number used to identify this timer.
#define TURING_TIMER			512

#define MINIMUM_DELAY			  1
#define MSECS_BETWEEN_EVENT_CHECK	100
#define DUMMY_WINDOW			"DummyWindow"

#define SYSEXIT_ERROR_STRING		"SysExit"

/********************/
/* Static variables */
/********************/
static FilePath		stStartupDirectory;
static FilePath		stApplicationPath, stApplicationDirectory;
static FilePath		stApplicationName;
static TuringErrorPtr	stErrorPtr;
// Redirection Arguments
static RunArgs		stRunArgs;
static BOOL		stTuringProgramPaused = FALSE;
static BOOL		stTuringProgramRunning = TRUE;
static BOOL		stTuringProgramHalting = FALSE;
static BOOL		stQuittingEnvironment = FALSE;
static BOOL		stTuringProgramWaitingForEvent = FALSE;
static BOOL		stAllRunWindowsClosed = TRUE;
static RunStatus	stRunStatus;
static char		*stSizeMarker = OFFSET_STRING;
static UINT		stMinimumPeriod, stMaximumPeriod, stMinimumEventPeriod;
static HWND		stDummyWindow;

/******************************/
/* Static callback procedures */
/******************************/
static LRESULT CALLBACK	MyRaisedFrameWindowProcedure (HWND pmWindow, 
						      UINT pmMessage, 
						      WPARAM pmWParam, 
						      LPARAM pmLParam);
static UINT CALLBACK 	MyRunWithArgsDialogProcedure (HWND pmWindow, 
						      UINT pmMessage, 
    						      WPARAM pmWParam, 
    						      LPARAM pmLParam);
static void CALLBACK 	MyTimerProcedure (UINT pmID, UINT pmMsg, DWORD pmUser,
					  DWORD pmDummy1, DWORD pmDummy2);

/*********************/
/* Static procedures */
/*********************/
static int	MyGetMaxStackSize (void);
static BOOL	MyInitializeGlobals (HINSTANCE pmApplicationInstance);
static BOOL	MyInitializeWindowClass (void);
static BOOL MyInitializeRunFromByteCode(RunSettings *runSettings, RunArgs *runArgs, char *filePath, char *fileName, BOOL useSeparateFile);
static BOOL MyInitializeRunFromFile(RunSettings *runSettings, RunArgs *runArgs, char *filePath, char *fileName, BOOL useSeparateFile);
static void	MyProcessWaitingEvents (BOOL pmGetAtLeastOneEvent);
static int	MyGetDirectoryFromPath (const char *pmPath, 
						     char *pmDirectory);
static void MyGetFilePathFromCmdLine(const char *cmdLine, unsigned int start, char *outFileName, char *outFilePath);


/***********************/
/* External procedures */
/***********************/
/********************************************************/
/* WinMain						*/
/*							*/
/* Called by the system when the program is first run.	*/
/********************************************************/
int WINAPI	WinMain (HINSTANCE pmApplicationInstance, 
			 HINSTANCE pmPrevInstance,
    			 PSTR pmCmdLine, int pmCmdShow)
{
    BOOL	myUseSeparateFile = FALSE;
	BOOL	myCompileFile = FALSE;
    FilePath	myFileName, myFilePath;
	int myStatus;
    RunArgs	myRunArgs;
	RunSettings myRunSettings;
    char	*myInputFile, *myOutputFile;
    int		myFontSize;
    OOTint	myNumErrors;
    int		myDelayWait;
    BOOL	myTuringProgramWaitingForEvent;
    TIMECAPS	myTimeCaps;
    MMRESULT	myTimer;
    HWND	myDummyWindow;

	FilePath myCurrentDirectory;



    //
    // Initialize Turing modules
    //
    EdGUI_Init ();
    EdPrint_Init ();
    
    //
    // Initialize Turing
    //
    TL ();
    FileManager ();
    Language ();

    //
    // Initialize windows needed by prolog
    //
    if (!MyInitializeWindowClass ())
    {
    	return 0;
    }
    
    // 
    // Create the dummy window
    //
    myDummyWindow = CreateWindow (
    	DUMMY_WINDOW, 		// Window class
        NULL,	 		// Window title
        0,			// Window style
    	CW_USEDEFAULT, 		// Initial x location
    	CW_USEDEFAULT, 		// Initial y location
    	CW_USEDEFAULT, 		// Initial x size
    	CW_USEDEFAULT, 		// Initial y size
    	HWND_DESKTOP, 		// Parent window handle
    	NULL, 			// Window menu handle
    	pmApplicationInstance,	// Program instance handle
    	NULL);			// Creation parameters

    //
    // Initialize the globals
    // 
    if (!MyInitializeGlobals (pmApplicationInstance))
    {
    	return 0;
    }
    
    //
    // If the command line is "-file <filename>", then read from the file
    // rather than reading from whatever is appended to the file.
    //
    strcpy (myFileName, stApplicationName);
    strcpy (myFilePath, stApplicationPath);
	strcpy (myCurrentDirectory, stStartupDirectory);
    if (pmCmdLine [0] != 0)
    {
    	if (strncmp (pmCmdLine, "-file ", 6) == 0)
    	{
    	    myUseSeparateFile = TRUE;
			MyGetFilePathFromCmdLine(pmCmdLine,6,myFileName,myFilePath);

			// resource gets must be based from the bytecode file
			myStatus = MyGetDirectoryFromPath (myFilePath, myCurrentDirectory);
			if (myStatus != 0)
			{
    			EdGUI_Message1 (NULL, 0, IDS_TPROLOG_APPLICATION_NAME,
    	    					IDS_TPROLOG_CANT_OPEN_OBJECT_FILE, myFileName);
				return 0;
			}
    	} else if (strncmp (pmCmdLine, "-run ", 5) == 0)
    	{
    	    myCompileFile = TRUE;
			MyGetFilePathFromCmdLine(pmCmdLine,5,myFileName,myFilePath);

			// resource gets must be based from the bytecode file
			myStatus = MyGetDirectoryFromPath (myFilePath, myCurrentDirectory);
			if (myStatus != 0)
			{
    			EdGUI_Message1 (NULL, 0, IDS_TPROLOG_APPLICATION_NAME,
    	    					IDS_TPROLOG_CANT_OPEN_OBJECT_FILE, myFileName);
				return 0;
			}
    	} else if (strncmp (pmCmdLine, "-Debug", 6) == 0)
		{
			DebugBreak ();
		}
    }

    
    FileManager_SetHomeDir (stStartupDirectory);

	if(myCompileFile) {
		if(!MyInitializeRunFromFile(&myRunSettings,&myRunArgs,myFilePath,myFileName,myUseSeparateFile)) {
			return FALSE; // initialize failed
		}
	} else {
		if(!MyInitializeRunFromByteCode(&myRunSettings,&myRunArgs,myFilePath,myFileName,myUseSeparateFile)) {
			return FALSE; // initialize failed
		}
	}

    // Initialize the MIO module
    MIO_Initialize (gProgram.applicationInstance, OS_WINDOWS, 
    		    stApplicationDirectory, stStartupDirectory,
    		    myRunSettings.centerOutputWindow, myRunSettings.stopUserClose,
		    SYSEXIT_ERROR_STRING);
    		    
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

    // Get rid of the ".exe" at the end of a file name
    if (strcmp (&myFileName [strlen (myFileName) - 4], ".exe") == 0)
    {
    	myFileName [strlen (myFileName) - 4] = 0;
    }

    //
    // Initialize Windows high resolution timer
    //
    if (timeGetDevCaps (&myTimeCaps, sizeof(TIMECAPS)) == TIMERR_NOERROR) 
    {
	stMinimumPeriod = myTimeCaps.wPeriodMin;
	stMinimumEventPeriod = max (MSECS_BETWEEN_EVENT_CHECK / 10, 
    				    stMinimumPeriod);
	stMaximumPeriod = myTimeCaps.wPeriodMax;

	timeBeginPeriod (stMinimumPeriod);
    }
    else
    {
	stMinimumPeriod = -1;
    }
    
    
    // 
    // Initialize MIO
    //
    if (!MIO_Init_Run (myFileName, myInputFile, 
    		      myRunArgs.inputRedirection == ARGS_IN_FILE_ECHO,
    		      myOutputFile,
    		      ((myRunArgs.outputRedirection == ARGS_OUT_FILE_SCREEN) ||
    		       (myRunArgs.outputRedirection == ARGS_OUT_PRINTER_SCREEN)),
    		      ((myRunArgs.outputRedirection == ARGS_OUT_PRINTER) ||
    		       (myRunArgs.outputRedirection == ARGS_OUT_PRINTER_SCREEN)),
    		      myCurrentDirectory,		// Execution directory
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
		      gProperties.parallelIOPort,	// Set PP I/O Port
		      FALSE)) 				// Not a Test Suit Prog
    {
    	return FALSE;
    }
    
    // Status message    
    stTuringProgramRunning = TRUE;
    stTuringProgramPaused = FALSE;
    stTuringProgramHalting = FALSE;
    stQuittingEnvironment = FALSE;

    do
    {
    	if (!stTuringProgramPaused)
	{
            Language_ExecuteProgram (&stRunStatus, &stErrorPtr, &myNumErrors);

	    // If we're using sprites, then it may be time to update the screen.
	    MIO_UpdateSpritesIfNecessary ();

    	    if (gProperties.executionDelay > 0)
    	    {
    	    	Sleep (gProperties.executionDelay);
    	    }
        }

	myTuringProgramWaitingForEvent = FALSE; 
	myDelayWait = 0;

	// If we're delaying for a number of reasons (waiting for keyboard
	// input, in a delay, etc.
	if (stRunStatus.state == DelayEventWait)
	{
	    myDelayWait = stRunStatus.moreStateInfo - MIOTime_GetTicks () - 
		MINIMUM_DELAY - stMinimumPeriod;
	    if (myDelayWait <= 0) 
	    {
		myDelayWait = 0;
	    }
	}

	// We are waiting for a certain amount of time to pass.  We do this by
	// setting a timer and then waiting for the next event to pass.
	if (myDelayWait > 0)
	{
	    int	myTimerResolution;
	    
	    myTimerResolution = max (stMinimumPeriod, (UINT) myDelayWait / 10);	
	    myTimer = timeSetEvent ((UINT) myDelayWait, myTimerResolution, 
	    			    MyTimerProcedure, (DWORD) myDummyWindow, 
	    			    TIME_ONESHOT | TIME_CALLBACK_FUNCTION);
	    if (myTimer == 0)
	    {	
	    	myDelayWait = 0;    		      	
    	    }
	    else
	    {
		myTuringProgramWaitingForEvent = TRUE;
	    }
	}
		
	// If we are paused, waiting for keyboard or mouse input, or waiting 
	// for a delay, don't return until we have processed at least 
	// one Windows event.
	MyProcessWaitingEvents (myTuringProgramWaitingForEvent);

	if (myDelayWait > 0)
	{
	    timeKillEvent (myTimer);
	}
    } while ((stRunStatus.state != Finished) && (!stTuringProgramHalting));

    //
    // At this point, the program has either finished executing or the been
    // told to halt (permanently) either by the user closing a run window or
    // pressing stop, etc.
    // 

    // If the original window is no longer open, use the top editor window.
    MIO_Finalize_Run ();
	
    // Close all files opened by the running program
    Language_EndExecution ();
	
    stTuringProgramRunning = FALSE;
	  
    if (stTuringProgramHalting && stQuittingEnvironment)
    {
    	return 0;
    }

    if ((myNumErrors >= 1) &&
	(strncmp (stErrorPtr -> text, SYSEXIT_ERROR_STRING, 
		  strlen (SYSEXIT_ERROR_STRING)) != 0))
    {
    	FilePath	myErrorPathName;

	Language_GetFileName (stErrorPtr -> srcPos.fileNo, myErrorPathName);
    	EdGUI_Message ("Run Time Error", 
    	    "An run-time error has occurred in this program.\n\n"
    	    "Line %d of %s: %s", stErrorPtr -> srcPos.lineNo, 
    	    myErrorPathName, stErrorPtr -> text);
    }

    DestroyWindow (myDummyWindow);

    // If we are to continue after program is finished, then continue
    if (!myRunSettings.closeWindowsOnTerminate)
    {
    	int	myResult = TRUE;
    	MSG	myMessage;

	while (!stAllRunWindowsClosed)
	{    	
    	    if (!GetMessage (&myMessage, NULL, 0, 0))
    	    {
    	    	break;
    	    }
            TranslateMessage (&myMessage);
       	    DispatchMessage (&myMessage);
       	}
    }

    return 0;
} // WinMain


/****************************************************/
/* Emulated routines from other parts of the editor */
/****************************************************/
/************************************************************************/
/* Ed_GetLastActiveEditWindow						*/
/************************************************************************/
HWND	Ed_GetLastActiveEditWindow (void)
{
    return NULL;
} // Ed_GetLastActiveEditWindow


/************************************************************************/
/* Ed_SetModalDialog							*/
/************************************************************************/
void	Ed_SetModalDialog (HWND pmWindow)
{
    // Do nothing
} // Ed_SetModalDialog


/************************************************************************/
/* EdProp_GetStartupDirectory						*/
/************************************************************************/
const char	*EdProp_GetStartupDirectory (void)
{
    return stStartupDirectory;
} // EdProp_GetStartupDirectory


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
/* EdRun_NotifyAllRunWindowsClosed					*/
/*									*/
/* If we're in beginner mode and we've closed the run window, enlarge	*/
/* the editor window.							*/
/************************************************************************/
void	EdRun_NotifyAllRunWindowsClosed (void)
{
    stAllRunWindowsClosed = TRUE;
} // EdRun_NotifyAllRunWindowsClosed


/************************************************************************/
/* EdRun_NotifyRunWindowOpened						*/
/*									*/
/* If we're in beginner mode and we've opened our first run window, 	*/
/* minimize the editor window.						*/
/************************************************************************/
void	EdRun_NotifyRunWindowOpened (HWND pmRunWindow)
{
    stAllRunWindowsClosed = FALSE;
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
    
    if (stTuringProgramPaused)
    {
    	// We're paused, so let's resume
    	stTuringProgramPaused = FALSE;

	// Notify the MIO module that the program is no longer paused.  Put
	// the Run windows on top if we are no longer pausing.  If we are
	// just stepping, then don't bother.
	MIO_NotifyTuringProgramResumed (TRUE);
    }
    else
    {
    	// Let's pause the program
    	stTuringProgramPaused = TRUE;

    	// Notify the MIO module that the program is paused.
	MIO_NotifyTuringProgramPaused ();
    }
} // EdRun_PauseResumeProgram


/************************************************************************/
/* EdRun_KillRunningProgram						*/
/************************************************************************/
void	EdRun_KillRunningProgram (void)
{
    stTuringProgramHalting = TRUE;
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
BOOL	EdRun_KillRunningProgramAndQuit (void)
{
    stTuringProgramHalting = TRUE;
    stQuittingEnvironment = TRUE;

    return stTuringProgramRunning;
} // EdRun_KillRunningProgramAndQuit


/******************************/
/* Static callback procedures */
/******************************/
/************************************************************************/
/* MyDummyWindowProcedure						*/
/*									*/
/* Callback routine for dummy window.		 			*/
/************************************************************************/
static LRESULT CALLBACK	MyDummyWindowProcedure (HWND pmWindow, 
						UINT pmMessage, 
						WPARAM pmWParam, 
						LPARAM pmLParam)
{
    return DefWindowProc (pmWindow, pmMessage, pmWParam, pmLParam);
} // MyDummyWindowProcedure


/************************************************************************/
/* MyRaisedFrameWindowProcedure						*/
/*									*/
/* Callback routine from the raised frame window. 			*/
/************************************************************************/
static LRESULT CALLBACK	MyRaisedFrameWindowProcedure (HWND pmWindow, 
						      UINT pmMessage, 
						      WPARAM pmWParam, 
						      LPARAM pmLParam)
{
    HDC		myDeviceContext;
    PAINTSTRUCT	myPaintStruct;
    RECT	myWindowRect;
    
    if (pmMessage == WM_PAINT)
    {
    	myDeviceContext = BeginPaint (pmWindow, &myPaintStruct);
    	SetMapMode (myDeviceContext, MM_TEXT);
    	GetClientRect (pmWindow, &myWindowRect);
    	DrawEdge (myDeviceContext, &myWindowRect, EDGE_RAISED, BF_RECT);
    	EndPaint (pmWindow, &myPaintStruct);
    	return 0;
    }
        	
    return DefWindowProc (pmWindow, pmMessage, pmWParam, pmLParam);
} // MyRaisedFrameWindowProcedure


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
			    		IDS_TPROLOG_BAD_CMD_LINE_ARG_TITLE, 
			    		IDS_TPROLOG_ARG_TOO_LONG);
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
			    	    IDS_TPROLOG_BAD_CMD_LINE_ARG_TITLE, 
			    	    IDS_TPROLOG_UNTERMINATED_QUOTE);
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
			    		IDS_TPROLOG_BAD_CMD_LINE_ARG_TITLE, 
			    		IDS_TPROLOG_ARG_TOO_LONG);
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


/*********************/
/* Static procedures */
/*********************/
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
/* MyInitializeGlobals							*/
/************************************************************************/
static BOOL	MyInitializeGlobals (HINSTANCE pmApplicationInstance)
{
    char	*myPtr;
    SYSTEMTIME	mySystemTime;
    DWORD	myDummy;
    DWORD	myVersionSize;
    char	*myVersionData, *myVersionInfo;
    UINT	myVersionInfoSize;

    if (GetModuleFileName (NULL, stApplicationPath, PATHNAME_LENGTH) == 0)
    {
    	EdFail_Warn (IDS_TPROLOG_GETMODULEFILENAMEFAIL, __FILE__, __LINE__, 
    		     GetLastError ());
    	return FALSE;
    }

    strcpy (stApplicationDirectory, stApplicationPath);
    myPtr = strrchr (stApplicationDirectory, '\\');
    *myPtr = 0;
    strcpy (stApplicationName, myPtr + 1);

    // If we're at a top level directory, then we need to restore the 
    // last backslash.
    if (stApplicationDirectory [2] == 0)
    {
	stApplicationDirectory [2] = '\\';
	stApplicationDirectory [3] = 0;
    }
    
    // Initialize the current (startup) directory
    EdFile_GetCurrentDirectory (stStartupDirectory);

    gProgram.applicationInstance = pmApplicationInstance;
    gProgram.globalsInitialized = TRUE;
    gProgram.isTuring = TRUE;
    gProgram.isJava = FALSE;
    gProgram.isCpp = FALSE;
    
    gProgram.language = LANGUAGE_KIND_TURING;
    EdGUI_LoadString (IDS_TPROLOG_TURING_PROLOG_NAME,
	gProgram.environmentName, sizeof (gProgram.environmentName));
    gProgram.installKind = INSTALL_KIND_OPEN;
    gProgram.miniVersion = FALSE;
    gProgram.restrictedVersion = FALSE;
    gProgram.assistedByIBM = FALSE;
    gProgram.expiryDateString [0] = 0;
    ZeroMemory (&mySystemTime, sizeof (SYSTEMTIME));
    mySystemTime.wYear = 2100;
    mySystemTime.wMonth = 1;
    mySystemTime.wDay = 1;
    if (!SystemTimeToFileTime (&mySystemTime, &gProgram.expiryDate))
    {
    	EdFail_Warn (IDS_TPROLOG_SYSTEMTIMETOFILETIMEFAIL, __FILE__, __LINE__, 
    		     GetLastError ());
    	return FALSE;
    }
    gProgram.licensedTo [0] = 0;
    
    strcpy (gProgram.versionNumber, "Unknown");
    
    // Get the applicaton name
    myVersionSize = GetFileVersionInfoSize (stApplicationPath, &myDummy);
    if (myVersionSize != 0)
    {
	myVersionData = malloc (myVersionSize);
	if (myVersionData != NULL)
	{
	    if (GetFileVersionInfo (stApplicationPath, myDummy, 
	       		            myVersionSize, myVersionData))
    	    {
		if (VerQueryValue (myVersionData, 
		        "\\StringFileInfo\\04090000\\ProductVersion", 
		        &myVersionInfo, &myVersionInfoSize))
		{
		    strncpy (gProgram.versionNumber, myVersionInfo, 
		    	     myVersionInfoSize);
		    gProgram.versionNumber [myVersionInfoSize] = 0;
		}
    	    }
	    free (myVersionData);
    	}
    }
    
    return TRUE;
} // MyInitializeGlobals

static void MyGetFilePathFromCmdLine(const char *cmdLine, unsigned int start, char *outFileName, char *outFilePath)
{
	if (strrchr (cmdLine, '\\') == NULL)
    {
    	strcpy (outFileName, &cmdLine [start]);
    }
    else
    {
    	strcpy (outFileName, strrchr (cmdLine, '\\') + 1);
    }

	// is the path quoted? Then skip the quotes.
	if (cmdLine [start] == '"') {
		// on the file name
		outFileName[strlen(outFileName) - 1] = 0;
		strcpy (outFilePath, &cmdLine [start + 1]);
		// make sure to remove closing quote on path too
		outFilePath[strlen(outFilePath) - 1] = 0;
	} else {
    	strcpy (outFilePath, &cmdLine [start]);
	}

	// if the file name ends with .tbc, replace it with the original
	if(strcmp(&outFileName[strlen(outFileName) - 4],".tbc") == 0) {
		// .tbc minus 2 chars = .t
		outFileName[strlen(outFileName) - 2] = 0;
	}
}

/************************************************************************/
/* MyGetDirectoryFromPath						*/
/************************************************************************/
static int	MyGetDirectoryFromPath (const char *pmPath, char *pmDirectory)
{
    char	*myPtr;
    
    strcpy (pmDirectory, pmPath);
    myPtr = strrchr (pmDirectory, '\\');
    if (myPtr == NULL)
    {
    	// ERROR! No backslash in pathname!
    	return 1;
    }
    myPtr++;
    *myPtr = 0;
	return 0;
} // EdFile_GetDirectoryFromPath

/************************************************************************/
/* MyInitializeWindowClass						*/
/************************************************************************/
static BOOL	MyInitializeWindowClass (void)
{
    char	myRaisedFrameWindowClassName [256];
    int		myResult;
    WNDCLASSEX	myRaisedFrameClass, myDummyClass;

    // Get the class name
    EdGUI_LoadString (IDS_MIO_RAISED_FRAME_WINDOW_NAME, 
        myRaisedFrameWindowClassName, 
	sizeof (myRaisedFrameWindowClassName));
		     
    /************************************/
    /* Register the raised window class */
    /************************************/
    myRaisedFrameClass.cbSize =        sizeof (myRaisedFrameClass);
    // Set window class to redraw when window size changes
    myRaisedFrameClass.style =	       CS_HREDRAW | CS_VREDRAW;
    // Procedure to be called with messages for this window class
    myRaisedFrameClass.lpfnWndProc =   MyRaisedFrameWindowProcedure;
    // The extra space in class struct
    myRaisedFrameClass.cbClsExtra =    0;
    // The extra space in window struct
    myRaisedFrameClass.cbWndExtra =    0;
    // The application's handle
    myRaisedFrameClass.hInstance =     gProgram.applicationInstance;
    // Set the icon for this window class
    myRaisedFrameClass.hIcon =	       NULL;
    // Set the cursor for this window class
    myRaisedFrameClass.hCursor =       LoadCursor (NULL, IDC_ARROW);
    // Set the background colour for this window
    myRaisedFrameClass.hbrBackground = GetSysColorBrush (COLOR_BTNFACE);
    // Set the menu for this window class
    myRaisedFrameClass.lpszMenuName =  NULL;
    // Name of the window class
    myRaisedFrameClass.lpszClassName = myRaisedFrameWindowClassName; 
    // Set the icon for this class.
    myRaisedFrameClass.hIconSm =       NULL;
    
    myResult = RegisterClassEx (&myRaisedFrameClass);
    if (myResult == 0)
    {
    	EdFail_Warn (IDS_TPROLOG_REGISTERCLASSFAIL, __FILE__, __LINE__, 
    		     GetLastError ());
    	return FALSE;
    }
    
    /************************************/
    /* Register the raised window class */
    /************************************/
    myDummyClass.cbSize =        sizeof (myDummyClass);
    // Set window class to redraw when window size changes
    myDummyClass.style =	 CS_HREDRAW | CS_VREDRAW;
    // Procedure to be called with messages for this window class
    myDummyClass.lpfnWndProc =   MyDummyWindowProcedure;
    // The extra space in class struct
    myDummyClass.cbClsExtra =    0;
    // The extra space in window struct
    myDummyClass.cbWndExtra =    0;
    // The application's handle
    myDummyClass.hInstance =     gProgram.applicationInstance;
    // Set the icon for this window class
    myDummyClass.hIcon =	 NULL;
    // Set the cursor for this window class
    myDummyClass.hCursor =       NULL;
    // Set the background colour for this window
    myDummyClass.hbrBackground = NULL;
    // Set the menu for this window class
    myDummyClass.lpszMenuName =  NULL;
    // Name of the window class
    myDummyClass.lpszClassName = DUMMY_WINDOW; 
    // Set the icon for this class.
    myDummyClass.hIconSm =       NULL;
    
    myResult = RegisterClassEx (&myDummyClass);
    if (myResult == 0)
    {
    	EdFail_Warn (IDS_TPROLOG_REGISTERCLASSFAIL, __FILE__, __LINE__, 
    		     GetLastError ());
    	return FALSE;
    }
    
    return TRUE;
} // MyInitializeWindowClass

static BOOL MyInitializeRunFromByteCode(RunSettings *runSettings, RunArgs *runArgs, char *filePath, char *fileName, BOOL useSeparateFile)
{
	long	myTuringFileDesciptor;
    int		myStatus;
    char	myObjectFileHeader [6];
	//
    // Open the executable file
    //
    TL_TLI_TLIOF (9, filePath, &myTuringFileDesciptor);
    
    if (myTuringFileDesciptor <= 0)
    {
    	EdGUI_Message1 (NULL, 0, IDS_TPROLOG_APPLICATION_NAME,
    	    	        IDS_TPROLOG_CANT_OPEN_OBJECT_FILE, filePath);
        return 0;
    }
    
    //
    // If we're reading from the executable, skip the header
    //
    if (!useSeparateFile)
    {
    	if (strcmp (stSizeMarker, OFFSET_STRING) == 0)
    	{
    	    EdGUI_Message1 (NULL, 0, IDS_TPROLOG_APPLICATION_NAME,
    	    		    IDS_TPROLOG_OFFSET_NOT_SET);
    	    return 0;
    	}
    	TL_TLI_TLISK (* (int *) stSizeMarker, myTuringFileDesciptor);
    } 

    //
    // First read the header.
    //
    TL_TLI_TLIRE (myObjectFileHeader, sizeof (OBJECT_FILE_HEADER), &myStatus,
    		  myTuringFileDesciptor);
    if (myStatus != 0)
    {
    	EdGUI_Message1 (NULL, 0, IDS_TPROLOG_FILE_READ_FAILED_TITLE,
    	    	        IDS_TPROLOG_UNABLE_TO_READ_EXE, fileName, 1);
        return 0;
    }
    myObjectFileHeader [sizeof (OBJECT_FILE_HEADER)-1] = 0;

	if (strcmp (myObjectFileHeader, OBJECT_FILE_ERROR_HEADER) == 0) {
		EdGUI_Message1 (NULL, 0, IDS_TPROLOG_FILE_READ_FAILED_TITLE,
    	    	        IDS_TPROLOG_COMPILE_FAIL, fileName);
        return 0;
	} else if (strcmp (myObjectFileHeader, OBJECT_FILE_HEADER) != 0)
    {
    	EdGUI_Message1 (NULL, 0, IDS_TPROLOG_FILE_READ_FAILED_TITLE,
    	    	        IDS_TPROLOG_BAD_HEADER_MATCH, fileName, 1);
        return 0;
    }
        	
    //
    // Then read TProlog specific preferences      
    //
    TL_TLI_TLIRE (&runSettings->closeWindowsOnTerminate, sizeof (BOOL), &myStatus,
    		  myTuringFileDesciptor);
    if (myStatus != 0)
    {
    	EdGUI_Message1 (NULL, 0, IDS_TPROLOG_FILE_READ_FAILED_TITLE,
    	    	        IDS_TPROLOG_UNABLE_TO_READ_EXE, fileName, 2);
        return 0;
    }
    TL_TLI_TLIRE (&runSettings->displayRunWithArgs, sizeof (BOOL), &myStatus,
    		  myTuringFileDesciptor);
    if (myStatus != 0)
    {
    	EdGUI_Message1 (NULL, 0, IDS_TPROLOG_FILE_READ_FAILED_TITLE,
    	    	        IDS_TPROLOG_UNABLE_TO_READ_EXE, fileName, 3);
        return 0;
    }
    TL_TLI_TLIRE (&runSettings->centerOutputWindow, sizeof (BOOL), &myStatus,
    		  myTuringFileDesciptor);
    if (myStatus != 0)
    {
    	EdGUI_Message1 (NULL, 0, IDS_TPROLOG_FILE_READ_FAILED_TITLE,
    	    	        IDS_TPROLOG_UNABLE_TO_READ_EXE, fileName, 4);
        return 0;
    }
    TL_TLI_TLIRE (&runSettings->stopUserClose, sizeof (BOOL), &myStatus,
    		  myTuringFileDesciptor);
    if (myStatus != 0)
    {
    	EdGUI_Message1 (NULL, 0, IDS_TPROLOG_FILE_READ_FAILED_TITLE,
    	    	        IDS_TPROLOG_UNABLE_TO_READ_EXE, fileName, 5);
        return 0;
    }
    
    // 
    // Then read the environment preferences
    //
    TL_TLI_TLIRE (&gProperties, sizeof (Properties), &myStatus,
    		  myTuringFileDesciptor);
    if (myStatus != 0)
    {
    	EdGUI_Message1 (NULL, 0, IDS_TPROLOG_FILE_READ_FAILED_TITLE,
    	    	        IDS_TPROLOG_UNABLE_TO_READ_EXE, fileName, 6);
        return 0;
    }
    
    //
    // Then read another header (just to make certain we're synced)
    //
    TL_TLI_TLIRE (myObjectFileHeader, sizeof (OBJECT_FILE_HEADER), &myStatus,
    		  myTuringFileDesciptor);
    if (myStatus != 0)
    {
    	EdGUI_Message1 (NULL, 0, IDS_TPROLOG_FILE_READ_FAILED_TITLE,
    	    	        IDS_TPROLOG_UNABLE_TO_READ_EXE, fileName, 7);
        return 0;
    }
    myObjectFileHeader [sizeof (OBJECT_FILE_HEADER)-1] = 0;
    if (strcmp (myObjectFileHeader, OBJECT_FILE_HEADER) != 0)
    {
    	EdGUI_Message1 (NULL, 0, IDS_TPROLOG_FILE_READ_FAILED_TITLE,
    	    	        IDS_TPROLOG_BAD_HEADER_MATCH, fileName, 2);
        return 0;
    }

	    //
    // Get run arguments if necessary
    //
    runArgs->commandLineArguments [0] = 0;
    strncpy (runArgs->ootArgs [0], stRunArgs.ootArgs [0], 255);    	    			     
    runArgs->numArgs = 0;
    runArgs->inputRedirection = ARGS_IN_KEY;
    runArgs->outputRedirection = ARGS_OUT_SCREEN;
    runArgs->inputFile [0] = 0;
    runArgs->outputFile [0] = 0;

    if (runSettings->displayRunWithArgs)
    {
    	if (!DialogBoxParam (gProgram.applicationInstance, 
                MAKEINTRESOURCE (TPROLOG_RUN_WITH_ARGS_DIALOG),
	        NULL, MyRunWithArgsDialogProcedure, (LPARAM) &stRunArgs))
    	{
	    return FALSE;
    	}
	*runArgs = stRunArgs;
    }
    
    // 
    // Then read the object files
    //
    Language_SetupExecutionFromObjectFile (myTuringFileDesciptor, 0, 
    	MyGetMaxStackSize (), "", "", runArgs->ootArgs, runArgs->numArgs);
   
    TL_TLI_TLICL (myTuringFileDesciptor);
	return TRUE;
}

static BOOL MyInitializeRunFromFile(RunSettings *runSettings, RunArgs *runArgs, char *filePath, char *fileName, BOOL useSeparateFile)
{
	TextHandleType	myTuringTextHandle;
    SizePtrType		myTuringSizePtr;
	FileNoType		myTuringFileNo;
    ResultCodeType	myResult;
	TuringErrorPtr	myError;    
    int			myErrors;
        	
	runSettings->closeWindowsOnTerminate = FALSE;
	runSettings->displayRunWithArgs = FALSE;
	runSettings->centerOutputWindow = FALSE;
	runSettings->stopUserClose = FALSE;

	gProperties.runUseSmallFont = FALSE;
    gProperties.useGraphicsMode = TRUE; 	// Graphics Mode
	strcpy(gProperties.runConsoleFontName,"Courier New");
	gProperties.runConsoleFontSize = 10;
    gProperties.runConsoleTextRows = 25; 	// Run window rows
    gProperties.runConsoleTextCols = 80; 	// Run window columns
    gProperties.runConsoleFullScreen = FALSE;
    gProperties.prohibitSysExec = FALSE;	// Allow/Forbid Sys.Exec
    gProperties.noSound = FALSE;		// Allow/Forbid Music
    gProperties.parallelIOPort = FALSE;	// Set PP I/O Port
	gProperties.executionDelay = 0;
	gProperties.turingStackSizeInKB = 0;

	//
    // Get run arguments if necessary
    //
    runArgs->commandLineArguments [0] = 0;
    strncpy (runArgs->ootArgs [0], stRunArgs.ootArgs [0], 255);    	    			     
    runArgs->numArgs = 0;
    runArgs->inputRedirection = ARGS_IN_KEY;
    runArgs->outputRedirection = ARGS_OUT_SCREEN;
    runArgs->inputFile [0] = 0;
    runArgs->outputFile [0] = 0;
    
    // 
    // Then read the file
    //
	FileManager_OpenNamedHandle (filePath, &myTuringFileNo, &myTuringTextHandle, &myTuringSizePtr,&myResult);
	if(myResult != 0)
	{
		EdGUI_Message1 (NULL, 0, IDS_TPROLOG_FILE_READ_FAILED_TITLE,
    	    	        IDS_TPROLOG_UNABLE_TO_READ_EXE, fileName, 1);
		return FALSE;
	}
	Language_CompileProgram ("", myTuringFileNo, &myError, &myErrors);
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
		return FALSE;
    } // if (myError != NULL)
	// Set up for running
	Language_SetupExecution (MyGetMaxStackSize (), "", "", runArgs->ootArgs, runArgs->numArgs);
	return TRUE;
}

/************************************************************************/
/* MyProcessWaitingEvents						*/
/************************************************************************/
static void	MyProcessWaitingEvents (BOOL pmGetAtLeastOneEvent)
{
    MSG		myMessage;
    int		myResult;

    if (pmGetAtLeastOneEvent)
    {
	// Process at least one event, even if you have to wait for it
	myResult = GetMessage (&myMessage, NULL, 0, 0);
	if (myResult)
	{
	    TranslateMessage (&myMessage);
	    DispatchMessage (&myMessage);
	}
    }

    // Process any waiting messages and return	
    while (PeekMessage (&myMessage, NULL, 0, 0, PM_REMOVE))
    {
	TranslateMessage (&myMessage);
	DispatchMessage (&myMessage);
    }
} // MyProcessWaitingEvents


