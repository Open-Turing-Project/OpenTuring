/************************************************************************/
/* ed.c									*/
/*									*/
/* This module is the front end to the Turing environment for Windows. 	*/
/************************************************************************/

/*******************/
/* System includes */
/*******************/
#include <windows.h>
#include <commctrl.h>
#include <crtdbg.h>
#include <stdio.h>
//#include <signal.h>

/****************/
/* Self include */
/****************/
#include "ed.h"

/******************/
/* Other includes */
/******************/
#include "eddde.h"
#include "edfail.h"
#include "edfalse.h"
#include "edprog.h"
#include "edprop.h"
#include "edrun.h"
#include "edsplash.h"
#include "edwin.h"

#include "dbgalloc.h"

#include "eddialogs.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
// This should be set to the number of different window types + 1
#define NUM_ACCELERATORS	(LAST_WINDOW_TYPE + 1)

// Command line options
#define OPTION_ADMIN			"-admin"
#define OPTION_LOG_STR			"-log"
#define OPTION_WRITE_ALL_PREFS_STR	"-writepref"
#define OPTION_NO_INITIAL_WINDOW	"-nowindow"
#define OPTION_DISPLAY_HELP_STR		"-?"
#define OPTION_TESTSUITE		"-testsuite"
#define OPTION_AUTORUN			"-run"
#define OPTION_COMPILEFILE			"-compile"

// Argumens for MyProcessCommandLineArguments
#define PROCESS_OPTIONS			TRUE
#define IGNORE_OPTIONS			FALSE

/*********/
/* Types */
/*********/
typedef struct WindowList
{
    HWND		window;
    int			windowKind;
    // activeIndex is a counter that is set to the current index whenever a
    // window is activated.  The WINDOW_TYPE_EDIT with the highest value
    // is the last WINDOW_TYPE_EDIT to have been active.
    int			activeIndex;
    struct WindowList	*next;
} WindowList, *WindowListPtr;

typedef struct WindowMessage
{
    BOOL	useNotify;
    int		message;
    WPARAM	wParam;
    LPARAM	lParam;
} WindowMessage;

/********************/
/* Static constants */
/********************/

/********************/
/* Static variables */
/********************/
// Properties of the status window
static BOOL		stPropertiesChanged;

// The accelerators
static HACCEL		stAccelerator [NUM_ACCELERATORS];

// The active window.  Send accelerator events to this window.
static HWND		stActiveWindow = NULL;
static HWND		stActiveModelessDialog = NULL;
static int		stLastActiveCounter = 1;
static int		stActiveWindowKind;

// There are times when only a specific window is allowed to be active.
// (Such as when a window has a dialog in front of it.)  This is all
// needed because Windows doesn't implement task modal dialogs correctly
// and messes up short cuts when there's a window modal dialog around.
static HWND		stModalDialog = NULL;

//
static WindowListPtr	stWindowListHeader = NULL;
static BOOL		stAlreadyExiting = FALSE;

// Logging command line option set
static BOOL		stLogging = FALSE;

// Don't open an initial window (used when DDE commands coming)
static BOOL		stNoWindow = FALSE;

// Admin command line option set
static BOOL		stWantToBecomeAdmin = FALSE;

// Run file on startup
static BOOL		stAutoRunPrograms = FALSE;
static FilePath		stAutoRunFile;

// Compile file on startup
static BOOL		stCompilePrograms = FALSE;
static FilePath		stCompileFile;

// Test suite command line option set
static BOOL		stRunningTestSuite = FALSE;
static FilePath		stTestSuiteFile;

// Mutex indicating Ready is running
static HANDLE		stReadyRunningMutex;

static HANDLE		stMemoryLogHandle = NULL;

#ifdef _MEMORY_CHECK
// Memory state holder
static _CrtMemState	stCRTMemState;	
#endif // #ifdef _MEMORY_CHECK

/******************************/
/* Static callback procedures */
/******************************/
static BOOL CALLBACK 	MyCheckControlProcedure (HWND pmDialog, UINT pmMessage, 
						 WPARAM pmWParam, 
						 LPARAM pmLParam);

/*********************/
/* Static procedures */
/*********************/
static BOOL	MyInitModules (void);
static BOOL	MyInit (void);
static BOOL	MyCheckCommonControlVersion (void);
static BOOL	MyConfirmExit (HWND pmWindow);
static void	MyFreeCommandLineArguments (int pmNumArgs, char *pmArgs[]);
static void	MyGetCommandLineArguments (char *pmCmdLine, int *pmNumArgs, 
					   int *pmNumOptionArgs, 
					   char ***pmArgs);
static BOOL	MyProcessCommandLineArguments (int pmNumArgs, char *pmArgs[],
					       BOOL pmProcessOptions);
static DWORD	MyProcessEvents (void);
static void	MyProcessMessage (MSG *pmMessagePtr);
static BOOL	MySetForegroundWindow (HWND pmWindow);
static void	MySignalHandler (int pmSignalNo);

/***********************/
/* External procedures */
/***********************/
extern char *tmpnam (char *string);

/********************************************************/
/* WinMain						*/
/*							*/
/* Called by the system when the program is first run.	*/
/*********************************** *********************/
int WINAPI	WinMain (HINSTANCE pmApplicationInstance, 
			 HINSTANCE pmPrevInstance,
    			 PSTR pmCmdLine, int pmCmdShow)
{
    BOOL	myAllowMoreThanOneRun = FALSE;
    int		myNumArgs, myNumOptionArgs;
    char	**myArgs;
    DWORD	myReturnValue;
    HWND	myWindow;
    int		cnt;
	

    // If the program is started up with the Shift and Ctrl keys both pressed,
    // then start the logging.
    if (((GetKeyState (VK_SHIFT) & 0x8000) != 0) &&
        ((GetKeyState (VK_CONTROL) & 0x8000) != 0))
    {
    	stLogging = TRUE;
		debugModeEnabled = TRUE;
    } else {
		stLogging = FALSE;
		debugModeEnabled = FALSE;
	}

    // If the program is started up with the Alt and Ctrl keys both pressed,
    // then start the allow more than one copy of Turing.  Mostly used to
    // debug network applications
    if (((GetKeyState (VK_MENU) & 0x8000) != 0) &&
        ((GetKeyState (VK_CONTROL) & 0x8000) != 0))
    {
    	myAllowMoreThanOneRun = TRUE;
    }        

    // Many routines use these, so we initialize it immediately.
    gProgram.applicationInstance = pmApplicationInstance;
    strcpy (gProgram.environmentName, "Environment");    
    
    InitCommonControls ();

    // Initialize the installation dependent stuff.  EdProg_Init should be 
    // called before just about anything else.
    if (!EdProg_Init ())
    {
    	EdGUI_Message1 (NULL, 0, IDS_INIT_FAILURE_TITLE,
    	    IDS_UNABLE_TO_OPEN_WINDOW);
    	return 0;
    }

    // Split the command line arguments into an array
    MyGetCommandLineArguments (pmCmdLine, &myNumArgs, &myNumOptionArgs, 
        &myArgs);

    //
    // Code to prevent more than one instance of Ready running at a time
    //
    if (myAllowMoreThanOneRun)
    {
    	char	*myTempName;
    	
    	if ((myTempName = tmpnam (NULL)) != NULL)
    	{
    	    stReadyRunningMutex = CreateMutex (NULL, TRUE, myTempName);
    	}
    	else
    	{
    	    stReadyRunningMutex = CreateMutex (NULL, TRUE, "TuringTemp");
    	}
    }
    else if (gProgram.isTuring)
    {
    	stReadyRunningMutex = CreateMutex (NULL, TRUE, "TuringRunningMutex");
    }
    else if (gProgram.isJava)
    {
    	stReadyRunningMutex = CreateMutex (NULL, TRUE, "ReadyRunningMutex");
    }
    if ((stReadyRunningMutex != NULL) && 
        (GetLastError () == ERROR_ALREADY_EXISTS))
    {
    	char	myEditWindowClassName [1024];
    	HWND	myForegroundWindow, myChildWindow;
	BOOL	myWindowDisplayed = FALSE;

	// Check to see if there are any files being opened by the commands
	// line call.  If there are, pass them via DDE to the currently
	// open Ready
	
    	// Process the command line arguments (most imporantly, turn on logging)
    	if (MyProcessCommandLineArguments (myNumArgs, myArgs, IGNORE_OPTIONS)) 
    	{
    	    EdDDE_TellReadyOpen (myNumArgs, myArgs);
    	}

    	EdGUI_LoadString (IDS_EDIT_MDI_FRAME_WINDOW_NAME,
	    myEditWindowClassName, sizeof (myEditWindowClassName));
	myWindow = FindWindow (myEditWindowClassName, NULL);
	if (myWindow == NULL)
	{
    	    EdGUI_LoadString (IDS_EDIT_WINDOW_NAME,
	        myEditWindowClassName, sizeof (myEditWindowClassName));
	    myWindow = FindWindow (myEditWindowClassName, NULL);
	}
	
	if (myWindow != NULL)
	{
   	    myChildWindow = GetLastActivePopup (myWindow);
	    myWindowDisplayed = MySetForegroundWindow (myWindow);
   	    if (myWindow != myChildWindow)
   	    {
	        myWindowDisplayed = MySetForegroundWindow (myChildWindow);
 	    }
	}
	
	myForegroundWindow = GetForegroundWindow ();
	if ((myForegroundWindow != myWindow) &&
	    (myForegroundWindow != myChildWindow))
	{
	    if (gProgram.isTuring)
	    {
   		EdGUI_Message1 (NULL, MB_OK, IDS_TURING_ALREADY_OPEN_TITLE,
    	    	    IDS_TURING_ALREADY_OPEN_MESSAGE);
	    }
	    else if (gProgram.isJava)
	    {
   		EdGUI_Message1 (NULL, MB_OK, IDS_READY_ALREADY_OPEN_TITLE,
    	    	    IDS_READY_ALREADY_OPEN_MESSAGE);
	    }
	}
	return 0;
    }
    
    //
    // Check Common Control Version Number
    //
    if (!MyCheckCommonControlVersion ())
    {
    	return 0;
    }
        
    // Process the command line arguments (most imporantly, turn on logging)
    if (!MyProcessCommandLineArguments (myNumArgs, myArgs, PROCESS_OPTIONS)) 
    { 
    	return 0; 
    }

    if (!MyInitModules ())
    {
    	EdGUI_Message1 (NULL, 0, IDS_INIT_FAILURE_TITLE,
    	    IDS_INIT_FAILURE_MESSAGE);
    	return 0;
    }

    if (stRunningTestSuite)
    {
    	FilePath	myCurrentDirectory, myTestSuiteFile;
    	
    	EdFile_GetCurrentDirectory (myCurrentDirectory);
    	EdFile_CombinePath (myCurrentDirectory, stTestSuiteFile, 
    			    myTestSuiteFile);
	EdRun_TestSuite (myTestSuiteFile, NULL);
	return 0;
    }
	if (stAutoRunPrograms)
    {
    	FilePath	myCurrentDirectory;
    	EdFile_GetCurrentDirectory (myCurrentDirectory);
		EdRun_RunProgramNoEditor (myCurrentDirectory,stAutoRunFile);
		return 0;
    }
	if (stCompilePrograms)
	{
		int myStatus;
		FilePath	myBytecodeFile;
		const char	*myPtr;
	
    	strcpy (myBytecodeFile, stCompileFile);
    	myPtr = EdFile_GetFileSuffix (myBytecodeFile);
    	strcpy (&myBytecodeFile [myPtr - myBytecodeFile], ".tbc");

		myStatus = EdRun_CreateByteCodeFile (stCompileFile,myBytecodeFile);
		return 0;
	}

    if (myNumArgs == myNumOptionArgs)
    {
    	// If we're opening up a window via DDE and we're in beginner mode,
    	// the system expects to see the initial window already created.
    	// Otherwise we don't need to create the initial window.
    	if (!stNoWindow || gProperties.beginnerMode)
    	{
    	    myWindow = EdWin_Create (NULL, NULL, 0, gProperties.beginnerMode, 
    	    			     gProperties.oneWindowMode); 
    	}
    } 
    else 
    { 
    	if (gProperties.beginnerMode)
    	{
    	    // Attempting to open more than one file is not allowed
    	    // in beginner mode
    	    if (myNumArgs - myNumOptionArgs > 1)
    	    {
    	    	EdGUI_Message1 (NULL, 0, IDS_CMD_LINE_FAILURE_TITLE,
	            IDS_CMD_LINE_FAILURE_MESSAGE);
    	    }
    	    
    	    // Open the first file.
    	    for (cnt = 0 ; cnt < myNumArgs ; cnt++)
    	    {
    	    	if (myArgs [cnt] [0] != 0)
    	    	{
    	    	    FilePath	myPathName;
    	    	    char	*myDummy;
    	    	    
    	    	    GetFullPathName (myArgs [cnt], PATHNAME_LENGTH, myPathName,
    	    	    	&myDummy);
	    	    myWindow = EdWin_Create (myPathName, NULL, 0, 
	    	    	gProperties.beginnerMode, gProperties.oneWindowMode);
		    if (myWindow == NULL)
		    {
		    	EdGUI_Message1 (NULL, 0, IDS_INIT_FAILURE_TITLE,
		    	    IDS_UNABLE_TO_OPEN_WINDOW);
			return 0;
		    }
	    	    break;
	    	}
	    }
    	}  // if (gProperties.beginnerMode)
    	else
    	{
    	    if (gProperties.oneWindowMode)
    	    {
    	    	// In one window mode, we create only one window for the first
    	    	// file and then call EdWin_AddFile for the rest
    	    	int	myCounter = 0;
    	    	while (myCounter < myNumArgs)
    	    	{
    	    	    if (myArgs [myCounter] [0] != 0)
    	    	    {
    	    	    	FilePath	myPathName;
    	    	    	char		*myDummy;
    	    	    
    	    	    	GetFullPathName (myArgs [myCounter], PATHNAME_LENGTH, 
    	    	    	    myPathName, &myDummy);
			myWindow = EdWin_Create (myPathName, NULL, 0, 
			    	        	 gProperties.beginnerMode, 
			    	        	 gProperties.oneWindowMode);
			if (myWindow == NULL)
			{
		    	    EdGUI_Message1 (NULL, 0, IDS_INIT_FAILURE_TITLE,
		    	        IDS_UNABLE_TO_OPEN_WINDOW);
			    return 0;
			}
	    	        myCounter++;
	    	        break;
	    	    }
	    	} // while
	    	
	    	// Now open the rest of the files into the same window
	    	for (cnt = myCounter ; cnt < myNumArgs ; cnt++)
	    	{
    	    	    if (myArgs [cnt] [0] != 0)
    	    	    {
	    	        EdWin_AddFile (myWindow, myArgs [cnt], NULL, 0);
			if (myWindow == NULL)
			{
		    	    EdGUI_Message1 (NULL, 0, IDS_INIT_FAILURE_TITLE,
		    	        IDS_UNABLE_TO_OPEN_WINDOW);
			    return 0;
			}
	    	    }
	    	} // for
    	    } // if (gProperties.oneWindowMode)
    	    else
    	    {
    	    	// Open each file into a new window
    	        for (cnt = 0 ; cnt < myNumArgs ; cnt++)
    	        {
    	    	    if (myArgs [cnt] [0] != 0)
    	    	    {
    	    	    	FilePath	myPathName;
    	    	    	char		*myDummy;
    	    	    
    	    	    	GetFullPathName (myArgs [cnt], PATHNAME_LENGTH, 
    	    	    	    myPathName, &myDummy);
	    	        myWindow = EdWin_Create (myPathName, NULL, 0, 
	    	        			 gProperties.beginnerMode, 
				    	         gProperties.oneWindowMode);
			if (myWindow == NULL)
			{
		    	    EdGUI_Message1 (NULL, 0, IDS_INIT_FAILURE_TITLE,
		    	        IDS_UNABLE_TO_OPEN_WINDOW);
			    return 0;
			}
	    	    } // if
	    	} // for
	    } // if (!gProperties.oneWindowMode)
    	} // if (!gProperties.beginnerMode)
    } // if (myNumArgs != myNumOptionArgs)

    if (myWindow == NULL)
    {
    	EdGUI_Message1 (NULL, 0, IDS_INIT_FAILURE_TITLE,
    	    IDS_UNABLE_TO_OPEN_WINDOW);
    	return 0;
    }

    if (!gProperties.skipSplashScreen)
    {    
    	// Start the splash screen
    	EdSplash_Create (FALSE);
    }
    
    MyFreeCommandLineArguments (myNumArgs, myArgs);
 
#ifdef _MEMORY_CHECK    
    // Set the memory state after everything is (presumably initialized)
    if (gProperties.debug)
    {
        _CrtMemCheckpoint (&stCRTMemState);
    }
#endif // #ifdef _MEMORY_CHECK
    
/*
__try
{
*/    	
    // Main body of the program.  Process all events here.
    myReturnValue = MyProcessEvents ();
/*    	
}
__except (EdFail_CrashHandler (GetExceptionInformation()))
{
}
*/    

    // Release mutex to allow another copy of Ready to exist
    ReleaseMutex (stReadyRunningMutex);
    
    // Quitting procedures here     
    EdRec_WriteRecentFiles ();
    EdDDE_Finalize ();
    EdWin_Finalize ();
    EdFile_Finalize ();
    EdFail_Finalize ();

    // This returns the exception filter to the default handler
    if (gProperties.catchExceptions)
    {
    	SetUnhandledExceptionFilter ((LPTOP_LEVEL_EXCEPTION_FILTER) NULL);
    }

#ifdef _MEMORY_CHECK    
    if (gProperties.debug)
    {
	Ed_CreateMemoryLogFile ();

        // Debugging information about memory leaks
        _CrtDumpMemoryLeaks ();
    }
#endif // #ifdef _MEMORY_CHECK

    if (stMemoryLogHandle != NULL)
    {
    	BOOL	myDebugEmpty;
    	
    	myDebugEmpty = (GetFileSize (stMemoryLogHandle, NULL) == 0);

	CloseHandle (stMemoryLogHandle);
    
    	// Delete the file if it's empty.
    	if (myDebugEmpty)
    	{
    	    FilePath	myPathName;
    	
    	    EdFile_ConstructPath (myPathName, FROM_STARTUP, 
    	    			  IDS_MEMORY_LEAK_FILE);
    	
    	    if (!DeleteFile (myPathName))
    	    {
	    	// Write to log
	    	EdLog_Log ("Unable to delete empty debug file.  Error = %d", 
	    	           GetLastError ());
    	    }
        }
    }

    return myReturnValue; 
} // WinMain


/************************************************************************/
/* Ed_PropertiesSet							*/
/************************************************************************/
void	Ed_PropertiesSet (Ed_Properties pmProperties)
{
    // EdGlob has no initialization sequence
    EdDDE_PropertiesSet ();
    EdFile_PropertiesSet ();
    EdGUI_PropertiesSet ();
    EdLog_PropertiesSet (pmProperties.logProperties);
    EdRun_PropertiesSet ();
    EdSplash_PropertiesSet ();
    EdWin_PropertiesSet (pmProperties.winProperties);
    
    stPropertiesChanged = TRUE;
} // Ed_PropertiesSet


/************************************************************************/
/* Ed_PropertiesImplementChanges					*/
/************************************************************************/
void	Ed_PropertiesImplementChanges ()
{
    WindowListPtr	myWindowListEntry;
    
    if (stPropertiesChanged)
    {
    	// EdGlob has no initialization sequence
    	EdDDE_PropertiesImplementChanges ();
    	EdFile_PropertiesImplementChanges ();
    	EdGUI_PropertiesImplementChanges ();
    	EdLog_PropertiesImplementChanges ();
    	EdRun_PropertiesImplementChanges ();
    	EdSplash_PropertiesImplementChanges ();
    	
	myWindowListEntry = stWindowListHeader;
	while (myWindowListEntry != NULL)
	{
	    switch (myWindowListEntry -> windowKind)
	    {
	    	case WINDOW_TYPE_EDIT:
	    	    EdWin_PropertiesImplementChanges (
	    	    	myWindowListEntry -> window);
	    	    EdWin_SetWindowTitle (myWindowListEntry -> window);
	    	    break;
//	    	case WINDOW_TYPE_CONSOLE:
//	    	    EdCon_PropertiesImplementChanges (
//	    	    	myWindowListEntry -> window);
//	    	    break;
	    	default:
	    	    EdFail_Fatal (IDS_UNEXPECTED_WINDOW_KIND, __FILE__, 
	    	    	__LINE__, 0, myWindowListEntry -> windowKind);
	    }
	    myWindowListEntry = myWindowListEntry -> next;
	}
    
        stPropertiesChanged = FALSE;
    }
} // Ed_PropertiesImplementChanges


/************************************************************************/
/* Ed_AddToWindowList							*/
/************************************************************************/
void	Ed_AddToWindowList (HWND pmWindow, int pmWindowKind)
{
    WindowListPtr	myWindowListEntry;
    
    myWindowListEntry = (WindowListPtr) malloc (sizeof (WindowList));
    myWindowListEntry -> window = pmWindow;
    myWindowListEntry -> windowKind = pmWindowKind;
    myWindowListEntry -> next = stWindowListHeader;
    
    stWindowListHeader = myWindowListEntry;
} // Ed_AddToWindowList


/************************************************************************/
/* Ed_CatchExceptions							*/
/************************************************************************/
BOOL	Ed_CatchExceptions (void)
{
    return gProperties.catchExceptions;
} // Ed_CatchExceptions


/************************************************************************/
/* Ed_CheckWindowPreemption						*/
/************************************************************************/
BOOL	Ed_CheckWindowPreemption (void)
{
    if ((stModalDialog != NULL) && IsWindow (stModalDialog))
    {
	SetForegroundWindow (stModalDialog);

	return TRUE;
    }

    return FALSE;
} // Ed_CheckWindowPreemption


/************************************************************************/
/* Ed_ClearBreakpoints							*/
/************************************************************************/
void	Ed_ClearBreakpoints (void)
{
    Ed_EnumerateFiles (EdDisp_ClearBreakpoints, NULL);
} // Ed_ClearBreakpoints


/************************************************************************/
/* Ed_CreateMemoryLogFile						*/
/************************************************************************/
void	Ed_CreateMemoryLogFile (void)
{
    // Has memory log file been created
    static BOOL		myStMemoryLogFileCreated = FALSE;

    if (!myStMemoryLogFileCreated)
    {
    	FilePath	myPathName;
    	
    	EdFile_ConstructPath (myPathName, FROM_STARTUP, IDS_MEMORY_LEAK_FILE);
    	
    	stMemoryLogHandle = CreateFile (myPathName, GENERIC_WRITE, 0, NULL,
    	    CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    	_CrtSetReportMode (_CRT_WARN, _CRTDBG_MODE_FILE);
    	_CrtSetReportFile (_CRT_WARN, stMemoryLogHandle);
    	_CrtSetReportMode (_CRT_ERROR, _CRTDBG_MODE_FILE);
    	_CrtSetReportFile (_CRT_ERROR, stMemoryLogHandle);
    	_CrtSetReportMode (_CRT_ASSERT, _CRTDBG_MODE_FILE);
    	_CrtSetReportFile (_CRT_ASSERT, stMemoryLogHandle);
    	
    	myStMemoryLogFileCreated = TRUE;
    }
} // Ed_CreateMemoryLogFile


/************************************************************************/
/* Ed_CreateRecentMenu							*/
/************************************************************************/
void	Ed_CreateRecentMenu (void)
{
    WindowListPtr	myWindowListEntry;
    
    myWindowListEntry = stWindowListHeader;

    // For each window, create the menu.
    while (myWindowListEntry != NULL)
    {
    	if (myWindowListEntry -> windowKind == WINDOW_TYPE_EDIT)
    	{
	    EdWin_CreateRecentMenu (myWindowListEntry -> window);
	}
    	myWindowListEntry = myWindowListEntry -> next;
    } // while (for each window)
} // Ed_CreateRecentMenu


/************************************************************************/
/* Ed_DumpMemory							*/
/************************************************************************/
void	Ed_DumpMemory (void)
{
#ifdef _MEMORY_CHECK
    Ed_CreateMemoryLogFile ();
    _CrtMemDumpAllObjectsSince (&stCRTMemState);
#endif // #ifdef _MEMORY_CHECK
} // Ed_DumpMemory


/************************************************************************/
/* Ed_EnableAllWindows							*/
/************************************************************************/
void	Ed_EnableAllWindows (BOOL pmEnable, HWND pmWindow)
{
    HWND		myWindow;
    WindowListPtr	myWindowListEntry;
    
    // Get the top-level window from the pmWindow parameter
    myWindow = pmWindow;
    while (GetParent (myWindow) != NULL)
    {
    	myWindow = GetParent (myWindow);
    }

    stModalDialog = NULL;

    myWindowListEntry = stWindowListHeader;

    while (myWindowListEntry != NULL)
    {
	if (myWindowListEntry -> window != myWindow)
	{
	    EnableWindow (myWindowListEntry -> window, pmEnable);
	}
    	myWindowListEntry = myWindowListEntry -> next;
    }
} // Ed_EnableAllWindows


/************************************************************************/
/* Ed_EnumerateFiles							*/
/************************************************************************/
void	Ed_EnumerateFiles (void (*enumProc) (char *, HWND, HWND, void *, void *),
			   void *pmUserParam)
{
    // Note this must be reworked to handle one window mode
    WindowListPtr	myWindowListEntry;
            
    myWindowListEntry = stWindowListHeader;

    while (myWindowListEntry != NULL)
    {
	if (myWindowListEntry -> windowKind == WINDOW_TYPE_EDIT)
	{
	    EdWin_EnumerateFiles (myWindowListEntry -> window, enumProc, 
	    			  pmUserParam);
	}
    	myWindowListEntry = myWindowListEntry -> next;
    }
} // Ed_EnumerateFiles


/************************************************************************/
/* Ed_EnumerateWindows							*/
/************************************************************************/
void	Ed_EnumerateWindows (void (*enumProc) (HWND, void *), void *pmUserParam)
{
    // Note this must be reworked to handle one window mode
    WindowListPtr	myWindowListEntry;
            
    myWindowListEntry = stWindowListHeader;

    while (myWindowListEntry != NULL)
    {
	if (myWindowListEntry -> windowKind == WINDOW_TYPE_EDIT)
	{
	    (*enumProc) (myWindowListEntry -> window, pmUserParam);
	}
    	myWindowListEntry = myWindowListEntry -> next;
    }
} // Ed_EnumerateWindows


/************************************************************************/
/* Ed_ExitApplication							*/
/************************************************************************/
void	Ed_ExitApplication (HWND pmWindow, BOOL pmImmediate)
{
    WindowListPtr	myWindowListEntry, myNextWindowListEntry;

    EdWin_ShowStatus (pmWindow, "Exiting %s", gProgram.environmentName);

    // Confirm Exit        
    if (!pmImmediate && (!MyConfirmExit (pmWindow)))
    {
    	EdWin_ShowStatus (pmWindow, "Exit Cancelled");
    	return;
    }

    stAlreadyExiting = TRUE;

    // Kill any running program
    EdRun_KillRunningProgramAndQuit (pmWindow);
    Sleep (40); // Give enough time for the system to kill the process
    
    // Close all open windows.  If any are left open, then we don't quit
    myWindowListEntry = stWindowListHeader;

    while (myWindowListEntry != NULL)
    {
    	myNextWindowListEntry = myWindowListEntry -> next;
    	
    	SendMessage (myWindowListEntry -> window, WM_CLOSE, 0, 0);
	
    	myWindowListEntry = myNextWindowListEntry;
    }

    stAlreadyExiting = FALSE;
    
    // The window may already be gone by this point.  Make certain it's
    // still a legit window before displaying the status message
    if (IsWindow (pmWindow))
    {
	EdWin_ShowStatus (pmWindow, "Exit Cancelled");
    }
} // Ed_ExitApplication


/************************************************************************/
/* Ed_GetExampleDirectories						*/
/*									*/
/* This function reads the "examples.txt" file located in the 		*/
/* <Ready Dir>\"Examples" folder and returns the list of labels along	*/
/* with the directory to copy.  Note it caches the list, so multiple	*/
/* calls result in the file being read once.				*/
/************************************************************************/
Examples	*Ed_GetExampleDirectories (HWND pmWindow)
{
    FilePath		myExamplesPathName;
    char		*myLine, *myPtr;
    char		*myBuffer;
    int			myLineNumber = 0;
    DWORD		myFileSize;
    Example		*myExample, *myLastExample = NULL;
    static Examples	*stExamples = NULL;
    static BOOL		stExamplesInitialized = FALSE;

    if (stExamplesInitialized)
    {
	return stExamples;
    }
    stExamplesInitialized = TRUE;

    EdFile_ConstructPath (myExamplesPathName, IDS_EXAMPLES_DIR, 
        IDS_EXAMPLES_FILE);
        
    // Read file into buffer
    myBuffer = EdFile_ReadFileIntoBuffer (NULL, myExamplesPathName, 
    					  &myFileSize, OK_IF_NOT_FOUND);
    if (myBuffer == NULL)
    {
    	stExamples = EXAMPLES_NOT_FOUND;
    	return stExamples;
    }
    
    EdFile_ReadLineFromBuffer (myBuffer, myFileSize, READ_FIRST);
    while (TRUE)
    {
    	// Read a line from the file
    	myLine = EdFile_ReadLineFromBuffer (myBuffer, 0, READ_NEXT);
    	if (myLine == NULL)
    	{
    	    break;
    	}
    	myLineNumber++;

	// Trim whitespace
	EdGlob_Trim (myLine);
    	myPtr = myLine;
	
    	// Check for comment or blank line
    	if ((*myPtr == '#') || (*myPtr == ';') || (*myPtr == 0))
    	{
    	    continue;
    	}
    	
	myPtr = strchr (myLine, '\t');
	if (myPtr == NULL)
	{
	    // Error - Missing tab
	    EdGUI_Message1 (pmWindow, 0, IDS_BAD_EXAMPLES_TITLE, 
	    		    IDS_MISSING_TAB, myLineNumber, myExamplesPathName);
	    stExamples = EXAMPLES_BAD_FORMAT;
	    break;
	}
	*myPtr++ = 0;
	EdGlob_Trim (myLine);
	EdGlob_Trim (myPtr);
	if (strchr (myPtr, '\t') != NULL)
	{
	    // Error - Too many tabs
	    EdGUI_Message1 (pmWindow, 0, IDS_BAD_EXAMPLES_TITLE, 
	    		    IDS_MISSING_TAB, myLineNumber, myExamplesPathName);
	    stExamples = EXAMPLES_BAD_FORMAT;
	    break;
	}

	// Initialize stExamples, if necessary
	if (stExamples == NULL)
	{
	    stExamples = (Examples *) malloc (sizeof (Examples));
	    if (stExamples == NULL)
	    {
	        EdGUI_Message1 (pmWindow, 0, IDS_BAD_EXAMPLES_TITLE, 
	    		    	IDS_OUTOFMEMORY);
	    	stExamples = EXAMPLES_OUT_OF_MEMORY;
	    	break;
	    }
	    stExamples -> numExamples = 0;
	    stExamples -> head = NULL;
	}

	// Create and fill the Example buffer
	myExample = (Example *) malloc (sizeof (Example));
	if (myExample == NULL)
	{
	    EdGUI_Message1 (pmWindow, 0, IDS_BAD_EXAMPLES_TITLE, 
	    	    	    IDS_OUTOFMEMORY);
	    stExamples = EXAMPLES_OUT_OF_MEMORY;
	    break;
	}
	myExample -> label = (char *) malloc (strlen (myLine) + 1);
	strcpy (myExample -> label, myLine);
	myExample -> directoryName = (char *) malloc (strlen (myPtr) + 1);
	strcpy (myExample -> directoryName, myPtr);
	myExample -> next = NULL;

	// Add to the end of the list
	if (myLastExample == NULL)
	{
	    stExamples -> head = myExample;
	}
	else
	{
	    myLastExample -> next = myExample;
	}
	myLastExample = myExample;

	// Increment the number of examples
	stExamples -> numExamples++;
    } // while

    free (myBuffer); 

    return stExamples;
} // Ed_GetExampleDirectories


/************************************************************************/
/* Ed_GetLastActiveEditWindow						*/
/************************************************************************/
HWND	Ed_GetLastActiveEditWindow (void)
{
    WindowListPtr	myWindowListEntry;
    HWND		myWindow = NULL;
    int			myHighestValue = -1;
    	
    // Find the file in the list
    myWindowListEntry = stWindowListHeader;
    while (myWindowListEntry != NULL)
    {
    	if ((myWindowListEntry -> windowKind == WINDOW_TYPE_EDIT) &&
    	    (myWindowListEntry -> activeIndex > myHighestValue))
    	{
    	    myWindow = myWindowListEntry -> window;
    	    myHighestValue = myWindowListEntry -> activeIndex;
    	}
    	myWindowListEntry = myWindowListEntry -> next;
    }
    
    return myWindow;    	
} // Ed_GetActiveWindow

extern void TL_TLI_TLISS ();extern void TL_TLI_TLIPS ();
extern MDIOTime_GetTicks ();
/************************************************************************/
/* Ed_ProcessWaitingEvents						*/
/************************************************************************/
void	Ed_ProcessWaitingEvents (BOOL pmWaitForEvent)
{
    MSG		myMessage;
    int		myResult;
    static BOOL	myStProcessingMessages = FALSE;
//static int c = 0;        
//char myMsg [256];
    if (!myStProcessingMessages)
    {
    	myStProcessingMessages = TRUE;
    	
//TL_TLI_TLISS (0, (short) 2);
//wsprintf (myMsg, "%d-%d\n", MDIOTime_GetTicks (), c++);
//TL_TLI_TLIPS (0, myMsg, (short) 0);
	if (pmWaitForEvent)
	{
	    // Process at least one event, even if you have to wait for it
    	    myResult = GetMessage (&myMessage, NULL, 0, 0);
    	    if (myResult)
    	    {
		MyProcessMessage (&myMessage);
	    }
//TL_TLI_TLISS (0, (short) 2);
//wsprintf (myMsg, "*%d-%d\n", MDIOTime_GetTicks (), c++);
//TL_TLI_TLIPS (0, myMsg, (short) 0);
	}

	// Process any waiting messages and return	
    	while (PeekMessage (&myMessage, NULL, 0, 0, PM_REMOVE))
    	{
//char myMsg [256];
//wsprintf (myMsg, "%d H:%x M:%d", c, (int) myMessage.hwnd, myMessage.message);
//c++;
//TL_TLI_TLISS (0, (short) 2);
//TL_TLI_TLIPS (0, myMsg, (short) 0);
    	    MyProcessMessage (&myMessage);
//TL_TLI_TLISS (0, (short) 2);
//TL_TLI_TLIPS (0, ".\n", (short) 0);
    	}
    	
    	myStProcessingMessages = FALSE;
    }
    else
    {
	DebugBreak ();
    }
} // Ed_ProcessWaitingEvents


/************************************************************************/
/* Ed_PutWindowAboveAllOtherEdWindows					*/
/*									*/
/* This goes through the window list and places all the MIO windows on	*/
/* the top, preserving the Z order relative to themselves.		*/
/************************************************************************/
void	Ed_PutWindowAboveAllOtherEdWindows (HWND pmWindow)
{
    // Go through the list of windows, putting all the MIO windows
    // on the top.
    HWND    myLastWindow = NULL;
    HWND    myWindow = GetTopWindow (GetDesktopWindow ());
    
    while (myWindow != NULL)
    {
	int myWindowType = GetWindowLong (myWindow, WINDOW_MEM_WINDOW_TYPE);

        if (myWindowType == WINDOW_TYPE_EDIT)
	{
	    if (myLastWindow == NULL)
	    {
		SetForegroundWindow (pmWindow);
	    }
	    else
	    {
		SetWindowPos (pmWindow, myLastWindow, 0, 0, 0, 0,
			      SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
	    }
	    return;
	}
	myLastWindow = myWindow;
	myWindow = GetNextWindow (myWindow, GW_HWNDNEXT);
    } // while
} // Ed_PutWindowAboveAllOtherEdWindows


/************************************************************************/
/* Ed_RedrawWindows							*/
/************************************************************************/
void	Ed_RedrawWindows (void)
{
    WindowListPtr	myWindowListEntry;
        
    myWindowListEntry = stWindowListHeader;

    while (myWindowListEntry != NULL)
    {
    	RedrawWindow (myWindowListEntry -> window, NULL, NULL, 
    	    RDW_FRAME | RDW_INVALIDATE);
    	myWindowListEntry = myWindowListEntry -> next;
    }
} // Ed_RedrawWindows


/************************************************************************/
/* Ed_RemoveFromWindowList						*/
/*									*/
/* If this is the last window to be closed, ask if the user wants to	*/
/* quit.  If they don't, then return false, otherwise start quitting	*/
/* the program.								*/
/************************************************************************/
BOOL	Ed_RemoveFromWindowList (HWND pmWindow)
{
    WindowListPtr	myWindowListEntry, myPrevWindowListEntry;
        
    // The files are stored in the linked list from most recent to least.
    
    // Check if the file is already on the list
    myWindowListEntry = stWindowListHeader;
    myPrevWindowListEntry = NULL;
    while (myWindowListEntry != NULL)
    {
    	if (myWindowListEntry -> window == pmWindow)
    	{
    	    break;
    	}
    	myPrevWindowListEntry = myWindowListEntry;
    	myWindowListEntry = myWindowListEntry -> next;
    }
    
    if (myWindowListEntry == NULL)
    {
    	EdFail_Fatal (IDS_WINDOW_NOT_FOUND, __FILE__, __LINE__, 0);
    }
    
    // The file is in the list.  If it is the last editor
    // window, then ask the user if they want to quit.
    if (myWindowListEntry -> windowKind == WINDOW_TYPE_EDIT)
    {
    	WindowListPtr	myTempWindowListEntry;
	int		myNumEditWindows = 0;
	    	
    	myTempWindowListEntry = stWindowListHeader;
    	while (myTempWindowListEntry != NULL)
    	{
    	    if (myTempWindowListEntry -> windowKind == WINDOW_TYPE_EDIT)
	    {
	    	myNumEditWindows++;
	    }
    	    myTempWindowListEntry = myTempWindowListEntry -> next;
    	}
    	
    	if ((myNumEditWindows == 1) && !stRunningTestSuite && !stAutoRunPrograms)
    	{
    	    if (!MyConfirmExit (pmWindow))
    	    {
		return FALSE;
	    }
	    
	    // Kill any running program
	    if (!EdRun_KillRunningProgramAndQuit (pmWindow))
	    {
		// Quit the program
		PostQuitMessage (0);
	    }
    	}
    }
    
    	
    if (myPrevWindowListEntry == NULL)
    {
    	stWindowListHeader = myWindowListEntry -> next;
    }    	    	
    else
    {
    	myPrevWindowListEntry -> next = myWindowListEntry -> next;
    }
    
    // If there are no non-editor windows to be opened, ask if user if they
    // want to quit
    // Free the entry
    free (myWindowListEntry);
    	    
    return TRUE;
} // Ed_RemoveFromWindowList


/************************************************************************/
/* Ed_SendMessageEnumerator						*/
/************************************************************************/
void	Ed_SendMessageEnumerator (HWND pmWindow, void *pmMessage)
{
    WindowMessage	*myMessage = (WindowMessage *) pmMessage;
    
    if (myMessage -> useNotify)
    {
    	SendNotifyMessage (pmWindow, myMessage -> message, 
	    		   (WPARAM) myMessage -> wParam, 
	    		   (LPARAM) myMessage -> lParam);
    }
    else
    {
    	SendMessage (pmWindow, myMessage -> message, 
    		     (WPARAM) myMessage -> wParam, 
    		     (LPARAM) myMessage -> lParam);
    }
} // Ed_SendMessageEnumerator

    
/************************************************************************/
/* Ed_SendMessageToAllWindows						*/
/************************************************************************/
void	Ed_SendMessageToAllWindows (BOOL pmUseNotify, int pmMessage,
				    WPARAM pmWParam, LPARAM pmLParam)
{
    WindowMessage	myMessage;
    
    myMessage.useNotify = pmUseNotify;
    myMessage.message = pmMessage;
    myMessage.wParam = pmWParam;
    myMessage.lParam = pmLParam;
    
    Ed_EnumerateWindows (Ed_SendMessageEnumerator, &myMessage);
} // Ed_SendMessageToAllWindows

				    
/************************************************************************/
/* Ed_SendStatusMessageEnumerator					*/
/************************************************************************/
void	Ed_SendStatusMessageEnumerator (HWND pmWindow, void *pmMessage)
{
    SendNotifyMessage (pmWindow, WM_SETSTATUSMESSAGE, 
	    	       (WPARAM) EdGlob_MallocString (pmMessage), 0);
} // Ed_SendStatusMessageEnumerator

    
/************************************************************************/
/* Ed_SendStatusMessageToAllWindows					*/
/************************************************************************/
void	Ed_SendStatusMessageToAllWindows (char *pmMessage)
{
    Ed_EnumerateWindows (Ed_SendStatusMessageEnumerator, pmMessage);
} // Ed_SendStatusMessageToAllWindows

				    
/************************************************************************/
/* Ed_SetActiveWindow							*/
/************************************************************************/
void	Ed_SetActiveWindow (HWND pmWindow, int pmWindowKind)
{
    stActiveWindow = pmWindow;
    stActiveModelessDialog = NULL;
    
    if (pmWindow == NULL)
    {
    	stActiveWindowKind = NO_WINDOW;
    	return;
    }
    
    stActiveWindowKind = pmWindowKind;
    
    // If this is an edit window, go through the list and set the active
    // counter of the window to the current value
    if (stActiveWindowKind == EDIT_WINDOW)
    {
    	WindowListPtr	myWindowListEntry;
    	
    	// Find the file in the list
    	myWindowListEntry = stWindowListHeader;
    	while (myWindowListEntry != NULL)
    	{
    	    if (myWindowListEntry -> window == pmWindow)
    	    {
    	        break;
    	    }
    	    myWindowListEntry = myWindowListEntry -> next;
        }
    
    	if (myWindowListEntry == NULL)
    	{
    	    EdFail_Fatal (IDS_WINDOW_NOT_FOUND, __FILE__, __LINE__, 0);
        }
    	
    	myWindowListEntry -> activeIndex = stLastActiveCounter;
    	stLastActiveCounter++;
    } // if (stActiveWindowKind == EDIT_WINDOW)
    
    // If this is a modeless dialog, save the current dialog
    if (stActiveWindowKind == FIND_REPLACE_DIALOG)
    {
    	stActiveModelessDialog = pmWindow;
    }
} // Ed_SetActiveWindow


/************************************************************************/
/* Ed_SetModalDialog							*/
/************************************************************************/
void	Ed_SetModalDialog (HWND pmWindow)
{
    if (pmWindow == NULL)
    {
	stModalDialog = NULL;
    }
    else if (stModalDialog == NULL)
    {
	stModalDialog = pmWindow;
    }
} // Ed_SetModalDialog


/************************************************************************/
/* Ed_ToggleBreakpointVisibility					*/
/************************************************************************/
void	Ed_ToggleBreakpointVisibility (void)
{
    EdRun_ToggleBreakpointVisibility ();
    EdDisp_SetBreakpointVisible (TOGGLE_BREAKPOINTS);
    
    Ed_EnumerateFiles (EdDisp_ResizeWindow, NULL);
} // Ed_ToggleBreakpointVisibility


/************************************************************************/
/* Ed_ToggleDebuggerControlsVisibility					*/
/************************************************************************/
void	Ed_ToggleDebuggerControlsVisibility (void)
{
    EdBBar_SetDebuggerVisible (TOGGLE_DEBUGGER_CONTROLS);
    
    Ed_SendMessageToAllWindows (FALSE, WM_CHANGEDEBUGGERCONTROL, 0, 0);
} // Ed_ToggleDebuggerControlsVisibility


/******************************/
/* Static callback procedures */
/******************************/
/***********************************************************************/
/* MyCheckControlProcedure				               */
/***********************************************************************/
static BOOL CALLBACK 	MyCheckControlProcedure (HWND pmDialog, UINT pmMessage, 
						 WPARAM pmWParam, 
						 LPARAM pmLParam)
{
    switch (pmMessage)
    {
        case WM_INITDIALOG:
	    EdGUI_CentreDialogBox (pmDialog);
    	    return TRUE;
    	case WM_COMMAND:
    	    switch (LOWORD (pmWParam))
    	    {
        	case IDOK:
    		    EndDialog (pmDialog, TRUE);
   		    return TRUE;
    		case IDCANCEL:
   		    EndDialog (pmDialog, FALSE);
   		    return TRUE;
	    } // switch
    } // switch
    return FALSE;
} // MyCheckControlProcedure

    			
/*********************/
/* Static procedures */
/*********************/
/************************************************************************/
/* MyInitModules							*/
/* 									*/
/* Initialize all the modules used by this program.			*/
/************************************************************************/
static BOOL	MyInitModules (void)
{
    // This must be called first, because all other init methods may call
    // EdFail methods that depends on the initialization.
    if (!EdFail_Init ())
    {
    	return FALSE;
    }

    // EdProp_Init will call Ed_PropertiesSet that is needed for most other 
    // modules to initialize properly
    if (!EdProp_Init (stWantToBecomeAdmin, stLogging))
    {
    	return FALSE;
    }

    // Logging may now be enabled
    if (gProperties.logging | gProperties.logWindowMessages)
    {
        EdLog_SetWriteToLog ();
    }

    // Now initialize this module
    if (!MyInit ())
    {
    	return FALSE;
    }

    // Initialize the DDE routines
    if (!EdDDE_Init ())
    {
    	return FALSE;
    }
    
    // Initialize the File routines
    gProgram.globalWindow = EdFalse_InitWindow ();
    if (gProgram.globalWindow == NULL)
    {
    	return FALSE;
    }
    
    // Initialize the File routines
    if (!EdFile_Init ())
    {
    	return FALSE;
    }
    
    // Initialize the GUI routines
    if (!EdGUI_Init ())
    {
    	return FALSE;
    }
    
    // Initialize the logging routines
    if (!EdLog_Init ())
    {
    	return FALSE;
    }
    
    // Initialize the Splash screen routines
    if (!EdSplash_Init ())
    {
    	return FALSE;
    }
    
    // EdWin_Init must be called after EdGUI_Init
    if (!EdWin_Init ()) 
    {
    	return FALSE;
    }
    
    // EdGlob has no initialization sequence
    
    if (!DbgAlloc_Init ())
    {
    	return FALSE;
    }
    
    return TRUE;
} // MyInitModules


/************************************************************************/
/* MyInit								*/
/* 									*/
/* Load the accelerators						*/
/************************************************************************/
static BOOL	MyInit (void)
{
    // Save the application instance
    stAccelerator [NO_WINDOW] = NULL;    
    stAccelerator [EDIT_WINDOW] = LoadAccelerators (
    	gProgram.applicationInstance, 
        MAKEINTRESOURCE (EDIT_WINDOW_ACCELERATOR));
    stAccelerator [SPLASH_WINDOW] = NULL;    
    stAccelerator [CONSOLE_WINDOW] = LoadAccelerators (
    	gProgram.applicationInstance, 
        MAKEINTRESOURCE (CONSOLE_WINDOW_ACCELERATOR));
    stAccelerator [TURING_RUN_WINDOW] = LoadAccelerators (
    	gProgram.applicationInstance, 
        MAKEINTRESOURCE (TURING_RUN_WINDOW_ACCELERATOR));
    stAccelerator [FIND_REPLACE_DIALOG] = LoadAccelerators (
        gProgram.applicationInstance, 
        MAKEINTRESOURCE (FIND_REPLACE_DIALOG_ACCELERATOR));
        
    if (stAccelerator [EDIT_WINDOW] == NULL)
    {
    	EdFail_Warn (IDS_LOADACCELERATORSFAIL, __FILE__, __LINE__,
    	    GetLastError (), EDIT_WINDOW);
    	return FALSE;
    }
    if (stAccelerator [CONSOLE_WINDOW] == NULL)
    {
    	EdFail_Warn (IDS_LOADACCELERATORSFAIL, __FILE__, __LINE__,
    	    GetLastError (), CONSOLE_WINDOW);
    	return FALSE;
    }
    if (stAccelerator [TURING_RUN_WINDOW] == NULL)
    {
    	EdFail_Warn (IDS_LOADACCELERATORSFAIL, __FILE__, __LINE__,
    	    GetLastError (), TURING_RUN_WINDOW);
    	return FALSE;
    }
    if (stAccelerator [FIND_REPLACE_DIALOG] == NULL)
    {
    	EdFail_Warn (IDS_LOADACCELERATORSFAIL, __FILE__, __LINE__,
    	    GetLastError (), FIND_REPLACE_DIALOG);
    	return FALSE;
    }
    
    // This code handles things like GPFs and so on.
    if (gProperties.catchExceptions)
    {
    	SetUnhandledExceptionFilter (
    		(LPTOP_LEVEL_EXCEPTION_FILTER) EdFail_CrashHandler);
    }

    return TRUE;
} // MyInit


/************************************************************************/
/* MyCheckCommonControlVersion						*/
/************************************************************************/
static BOOL	MyCheckCommonControlVersion (void)
{
    int	myVersion;
    
    myVersion = EdGUI_GetCommonControlVersionNumber ();
    
    if (myVersion == 0)
    {
        return DialogBoxParam (gProgram.applicationInstance, 
        		       MAKEINTRESOURCE (UNKNOWN_COMCTL_VERSION),
			       NULL, MyCheckControlProcedure, 0);
    }
    else if (myVersion < 470)
    {
        return DialogBoxParam (gProgram.applicationInstance, 
        		       MAKEINTRESOURCE (BAD_COMCTL_VERSION),
			       NULL, MyCheckControlProcedure, 0);
    }
    
    return TRUE;
} // MyCheckCommonControlVersion


/************************************************************************/
/* MyConfirmExit							*/
/************************************************************************/
static BOOL	MyConfirmExit (HWND pmWindow)
{
    int		myResult;
    
    if (stAlreadyExiting || !gProperties.confirmQuit)
        return TRUE;
    
    myResult = EdGUI_Message1 (pmWindow, 
    	MB_OKCANCEL | MB_ICONWARNING, IDS_APPLICATION_NAME, 
    	IDS_CONFIRM_QUIT_MESSAGE, gProgram.environmentName);

    if (myResult == IDCANCEL)
    {
	return FALSE;
    }
    
    return TRUE;
} // MyConfirmExit


/************************************************************************/
/* MyFreeCommandLineArguments						*/
/************************************************************************/
static void	MyFreeCommandLineArguments (int pmNumArgs, char *pmArgs[])
{
    int 	cnt;
    
    for (cnt = 0 ; cnt < pmNumArgs ; cnt++)
        free (pmArgs [cnt]);
    if (pmArgs != NULL)
	free (pmArgs);
} // MyFreeCommandLineArguments

    
/************************************************************************/
/* MyGetCommandLineArguments						*/
/************************************************************************/
static void	MyGetCommandLineArguments (char *pmCmdLine, int *pmNumArgs, 
					   int *pmNumOptionArgs, 
					   char **pmArgs[])
{
    char	*myPtr = pmCmdLine, *myWordPtr;
    int		myNumArgs = 0;
    int		myNumOptionArgs = 0;
    
    while (*myPtr != 0)
    {
    	// Skip whitespace
    	while ((*myPtr == ' ') || (*myPtr == '\t'))
    	    myPtr++;
    	   
    	if (*myPtr == 0)
    	    break;

	// Add one to the number of arguments
	myNumArgs++;

	if (*myPtr == '"')
	{
	    myPtr++;

    	    // Skip non-whitespace
    	    if ((*myPtr == '-') || (*myPtr == '/'))
    		myNumOptionArgs++;

    	    while ((*myPtr != '"') && (*myPtr != 0))
    		myPtr++;

	    if (*myPtr == '"')
	    {
		myPtr++;
	    }
	}
	else
	{
    	    // Skip non-whitespace
    	    if ((*myPtr == '-') || (*myPtr == '/'))
    		myNumOptionArgs++;
    	    
    	    while ((*myPtr != ' ') && (*myPtr != '\t') && (*myPtr != 0))
    		myPtr++;
	}
    }

    if (myNumArgs > 0)
    {
        *pmArgs = (char **) malloc (myNumArgs * sizeof (char *));
        
	myPtr = pmCmdLine;
	myNumArgs = 0;
	
        while (*myPtr != 0)
        {
    	    // Skip whitespace
    	    while ((*myPtr == ' ') || (*myPtr == '\t'))
    	        myPtr++;
    	   
    	    if (*myPtr == 0)
    	        break;

    	    // Skip non-whitespace
	    if (*myPtr == '"')
	    {
		myPtr++;
    		myWordPtr = myPtr;
    	        while ((*myWordPtr != '"') && (*myWordPtr != 0))
	        {
    		    myWordPtr++;
		}
	    }
	    else
	    {
    		myWordPtr = myPtr;
    	        while ((*myWordPtr != ' ') && (*myWordPtr != '\t') && 
    		        (*myWordPtr != 0))
		{
    		    myWordPtr++;
		}
	    }

	    // If this is a relative path, add the startup directory
	    if ((*myPtr == '-') || (*myPtr == '/') || (*myPtr == '\\') ||
		(*(myPtr + 1) == ':'))
	    {
    		(*pmArgs) [myNumArgs] = (char *) malloc (myWordPtr - myPtr + 1);
    	    
    		// Copy argument and add trailing space
    	        strncpy ((*pmArgs) [myNumArgs], myPtr, myWordPtr - myPtr);
    		((*pmArgs) [myNumArgs]) [myWordPtr - myPtr] = 0;
	    }
	    else
	    {
		FilePath    myCurrentDir;

		EdFile_GetCurrentDirectory (myCurrentDir);
		if (myCurrentDir [strlen (myCurrentDir) - 1] != '\\')
		{
		    strcat (myCurrentDir, "\\");
		}
    		(*pmArgs) [myNumArgs] = (char *) malloc (myWordPtr - myPtr + 
					    strlen (myCurrentDir) + 1);
    	    
    		// Copy argument and add trailing space
    	        strcpy ((*pmArgs) [myNumArgs], myCurrentDir);
    	        strncat ((*pmArgs) [myNumArgs], myPtr, myWordPtr - myPtr);
    		((*pmArgs) [myNumArgs]) [strlen (myCurrentDir) + 
					 myWordPtr - myPtr] = 0;
	    }    

	    // Add one to the number of arguments
	    myNumArgs++;

	    myPtr = myWordPtr;
	    if (*myPtr == '"')
	    {
		myPtr++;
	    }
	}
    }
    else
    {
    	*pmArgs = NULL;
    }
    
    *pmNumArgs = myNumArgs;
    *pmNumOptionArgs = myNumOptionArgs;
} // MyGetCommandLineArguments


/************************************************************************/
/* MyProcessCommandLineArguments					*/
/************************************************************************/
static BOOL	MyProcessCommandLineArguments (int pmNumArgs, char *pmArgs[],
					       BOOL pmProcessOptions)
{
    char	myArgument [1024];
    int		cnt;
    
    for (cnt = 0 ; cnt < pmNumArgs ; cnt++)
    {
    	strcpy (myArgument, pmArgs [cnt]);
    	
    	// Convert to upper case and set '/' switch to '-'
    	EdGlob_ConvertToLowerCase (myArgument);
    	if (myArgument [0] == '/')
    	{
    	    myArgument [0] = '-';
    	}
    			    
    	if (myArgument [0] == '-')
    	{
    	    if (strcmp (myArgument, OPTION_ADMIN) == 0)
    	    {
    	    	if (pmProcessOptions)
    	    	{
    	    	    stWantToBecomeAdmin = TRUE;
    	    	}
    	    	pmArgs [cnt] [0] = 0;
    	    }
			else if (strcmp (myArgument, OPTION_COMPILEFILE) == 0)
    	    {
    	    	if (pmProcessOptions)
    	    	{
					stCompilePrograms = TRUE;
    	    	}
    	    	pmArgs [cnt] [0] = 0;
				cnt++;
				if (cnt == pmNumArgs)
				{
						EdGUI_Message1 (NULL, 0, IDS_CMD_LINE_ERR_TITLE,
							IDS_NO_TESTSUITE_FILE);
    	    				return FALSE;
				}
				strcpy (stCompileFile, pmArgs [cnt]);
    	    			pmArgs [cnt] [0] = 0;
			}
			else if (strcmp (myArgument, OPTION_AUTORUN) == 0)
    	    {
    	    	if (pmProcessOptions)
    	    	{
    	    	    stAutoRunPrograms = TRUE;
    	    	}
    	    	pmArgs [cnt] [0] = 0;
				cnt++;
				if (cnt == pmNumArgs)
				{
						EdGUI_Message1 (NULL, 0, IDS_CMD_LINE_ERR_TITLE,
							IDS_NO_TESTSUITE_FILE);
    	    				return FALSE;
				}
				strcpy (stAutoRunFile, pmArgs [cnt]);
    	    			pmArgs [cnt] [0] = 0;
			}
    	    else if (strcmp (myArgument, OPTION_LOG_STR) == 0)
    	    {
    	    	if (pmProcessOptions)
    	    	{
    	    	    stLogging = TRUE;
    	    	}
    	    	pmArgs [cnt] [0] = 0;
    	    }
    	    else if (strcmp (myArgument, OPTION_WRITE_ALL_PREFS_STR) == 0)
    	    {
    	    	if (pmProcessOptions)
    	    	{
    	    	    EdProp_SetWriteAllPrefs ();
    	    	}
    	    	pmArgs [cnt] [0] = 0;
    	    }
    	    else if (strcmp (myArgument, OPTION_NO_INITIAL_WINDOW) == 0)
    	    {
    	    	if (pmProcessOptions)
    	    	{
    	    	    stNoWindow = TRUE;
    	    	}
    	    	pmArgs [cnt] [0] = 0;
    	    }
	    else if (strcmp (myArgument, OPTION_TESTSUITE) == 0)
	    {
    	    	if (pmProcessOptions)
    	    	{
    	    	    stRunningTestSuite = TRUE;
    	    	}
    	    	pmArgs [cnt] [0] = 0;
		cnt++;
		if (cnt == pmNumArgs)
		{
	            EdGUI_Message1 (NULL, 0, IDS_CMD_LINE_ERR_TITLE,
	                IDS_NO_TESTSUITE_FILE);
    	    	    return FALSE;
		}
		strcpy (stTestSuiteFile, pmArgs [cnt]);
    	    	pmArgs [cnt] [0] = 0;
	    }
    	    else if (strcmp (myArgument, OPTION_DISPLAY_HELP_STR) == 0)
    	    {
    	    	FilePath	myApplicationPath;
    	    	char		*myApplicationName;
    	    	char		*myDot;
    	    	// Get the name of the application. Remove the path and the
    	    	// extension.
    	    	GetModuleFileName (NULL, myApplicationPath, PATHNAME_LENGTH);
		myApplicationName = strrchr (myApplicationPath, '\\');
		if (myApplicationName == NULL)
		{
		    myApplicationName = myApplicationPath;
		}
		else
		{
		    // Skip over the backslash
		    myApplicationName++;
		}
		myDot = strrchr (myApplicationName, '.');
		if (_stricmp (myDot, ".exe") == 0)
		{
		    *myDot = 0;
		}
		
    	    	// Display command line options
	        EdGUI_Message1 (NULL, 0, IDS_CMD_LINE_HELP_TITLE,
	            IDS_CMD_LINE_HELP_MESSAGE, myApplicationName,
	            OPTION_LOG_STR, OPTION_WRITE_ALL_PREFS_STR, 
				OPTION_DISPLAY_HELP_STR,OPTION_AUTORUN,OPTION_COMPILEFILE);
    	    	pmArgs [cnt] [0] = 0;
    	    	return FALSE;
    	    }
    	    else
    	    {
    	    	// Unrecognized option
	        EdGUI_Message1 (NULL, 0, IDS_CMD_LINE_ERR_TITLE,
	            IDS_UNKNOWN_CMD_LINE_OPTION, pmArgs [cnt], OPTION_LOG_STR, 
	            OPTION_WRITE_ALL_PREFS_STR, OPTION_DISPLAY_HELP_STR,
				OPTION_AUTORUN,OPTION_COMPILEFILE);
    	    	pmArgs [cnt] [0] = 0;
    	    	return FALSE;
    	    }
    	} // if option
    } // for
    
    return TRUE;
} // MyProcessCommandLineArguments


/************************************************************************/
/* MyProcessEvents							*/
/************************************************************************/
static DWORD	MyProcessEvents (void)
{
    MSG		myMessage;
    int		myResult;
    
    while (TRUE)
    {
    	myResult = GetMessage (&myMessage, NULL, 0, 0);
    	if (!myResult) break;

	MyProcessMessage (&myMessage);
    }
    return myMessage.wParam;
} // MyProcessEvents


/************************************************************************/
/* MyProcessMessage							*/
/************************************************************************/
static void	MyProcessMessage (MSG *pmMessagePtr)
{
/*
    	// Log the windows messages
    	if (TRUE)
    	{
	    EdLog_PrintWindowsMessage ("Processing", pmMessagePtr -> hwnd, 
	    	pmMessagePtr -> message, pmMessagePtr -> wParam, 
	    	pmMessagePtr -> lParam);
    	}
    	
*/

    if ((stActiveWindow == NULL) ||
    	(!TranslateAccelerator (stActiveWindow, 
    	 	stAccelerator [stActiveWindowKind], pmMessagePtr)))
    {    	    	
    	if ((stActiveModelessDialog == NULL) || 
    	    (!IsDialogMessage (stActiveModelessDialog, pmMessagePtr)))
    	{
            TranslateMessage (pmMessagePtr);
       	    DispatchMessage (pmMessagePtr);
       	}
    }
} // MyProcessMessage


/***********************************************************************/
/* MySetForegroundWindow				               */
/***********************************************************************/
static BOOL	MySetForegroundWindow (HWND pmWindow)
{
    DWORD	myThreadID1, myThreadID2;
    BOOL	myResult;
    
    if (pmWindow == GetForegroundWindow ())
    {
    	return TRUE;
    }
    
    //
    // First need to get the thread responsible for this window,
    // and the thread for the foreground window.
    //
      
    myThreadID1 = GetWindowThreadProcessId (GetForegroundWindow (), NULL);
    myThreadID2 = GetWindowThreadProcessId (pmWindow, NULL);
    
    //
    // By sharing input state, threads share their concept of the active window.
    //
    if (myThreadID1 != myThreadID2)
    {
        AttachThreadInput (myThreadID1, myThreadID2, TRUE);
        myResult = SetForegroundWindow (pmWindow);
	Sleep (100);
        AttachThreadInput (myThreadID1, myThreadID2, FALSE);
    }
    else
    {         
        myResult = SetForegroundWindow (pmWindow);
	Sleep (100);
    }
    
    if (IsIconic (pmWindow))
    {
    	ShowWindow (pmWindow, SW_RESTORE);
    }
    else
    {
    	ShowWindow (pmWindow, SW_SHOW);
    }

    return myResult;
} // MySetForegroundWindow
