/************************************************************************/
/* dbgctrl.c								*/
/*									*/
/* Behaviour for the Debugger Controller.				*/
/*									*/
/* Program States: Not running, Executing, Paused, Waiting		*/
/*									*/
/* Run -  	 Always active.  					*/
/*		 Reads "Run" when not running				*/
/*		 Reads "Stop" when executing, paused or waiting		*/
/* Pause - 	 Active when executing, paused or waiting		*/
/*		 Disabled when not running				*/
/*		 Reads "Pause" when not running, executing, waiting	*/
/*		 Reads "Resume" when paused				*/
/* Step	-	 Active when not running, paused or waiting		*/
/*		 Disabled when executing				*/
/* Step Over -	 Active when paused					*/
/*		 Disabled when not running, executing or waiting	*/
/* Step Return - Active when paused					*/
/*		 Disabled when not running, executing or waiting	*/
/************************************************************************/

/*******************/
/* System includes */
/*******************/
#include <windows.h>
#include <commctrl.h>

/****************/
/* Self include */
/****************/
#include "dbgctrl.h"

/******************/
/* Other includes */
/******************/
#include "ed.h"
#include "edfail.h"
#include "edgui.h"
#include "edprog.h"
#include "edrun.h"

#include "eddialogs.h"
#include "edcommands.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
// Windows class name for a button
#define BUTTON_CLASS_NAME	"button"
#define STATIC_CLASS_NAME	"static"

#define NUM_ITEMS			11
#define NUM_BUTTONS			 5
#define FIRST_LABEL			 5
#define NUM_LABELS			 3
#define FIRST_CHECKBOX			 8
#define NUM_CHECKBOXES			 2

#define RUN_BUTTON			 0
#define PAUSE_BUTTON			 1
#define STEP_BUTTON			 2
#define STEP_OVER_BUTTON		 3
#define STEP_RETURN_BUTTON		 4
#define TRACKER			        10

#define DIALOG_CONTROLLER_WIDTH	       432
#define DIALOG_CONTROLLER_HEIGHT        87
		
/********************/
/* Global variables */
/********************/

/*********/
/* Types */
/*********/
typedef struct
{
    int	nameResource;
    int	alternateNameResource;
    int command, alternateButtonCommand;
    int x, y, width, height;
} ItemInfo;

/**********************/
/* External variables */
/**********************/

/********************/
/* Static constants */
/********************/
static ItemInfo	stItemInfo [NUM_ITEMS] = 
{
    {IDS_RUN_BUTTON, IDS_STOP_BUTTON, COMMAND_RUN, COMMAND_STOP, 11, 11, 75, 23},
    {IDS_PAUSE_BUTTON, IDS_RESUME_BUTTON, COMMAND_PAUSERESUME, 0, 95, 11, 75, 23},
    {IDS_STEP_BUTTON, 0, COMMAND_DEBUG_STEP, 0, 179, 11, 75, 23},
    {IDS_STEP_OVER_BUTTON, 0, COMMAND_DEBUG_STEP_OVER, 0, 263, 11, 75, 23},
    {IDS_STEP_RETURN_BUTTON, 0, COMMAND_DEBUG_STEP_RETURN, 0, 347, 11, 75, 23},
    
    {IDS_SLOW_LABEL, 0, 0, 0, 102, 59, 23, 12},
    {IDS_FAST_LABEL, 0, 0, 0, 293, 59, 22, 12},
    {IDS_TRACE_SPEED_LABEL, 0, 0, 0, 177, 41, 65, 12},
    
    {IDS_TRACE_CHECKBOX, 0, COMMAND_DEBUG_TRACE, 0, 11, 53, 70, 24},
    {IDS_SHOW_VARS_CHECKBOX, 0, COMMAND_DEBUG_SHOWVARS, 0, 347, 53, 100, 24},
    
    {0, 0, 0, 0, 125, 55, 168, 30}
};

/********************/
/* Static variables */
/********************/
HWND	stDebuggerControllerWindow = NULL;
BOOL	stDebuggerControllerVisible = FALSE;
HWND	stItems [NUM_ITEMS] = {NULL, NULL, NULL, NULL, NULL, NULL,
			       NULL, NULL, NULL, NULL, NULL};

/******************************/
/* Static callback procedures */
/******************************/
static LRESULT CALLBACK MyDebugControllerWindowProcedure (HWND pmController, 
							  UINT pmMessage, 
	    						  WPARAM pmWParam, 
	    						  LPARAM pmLParam);

/*********************/
/* Static procedures */
/*********************/
static void	MyCreateDebuggerController (HWND pmController);


/***********************/
/* External procedures */
/***********************/
/************************************************************************/
/* DbgCtrl_Init								*/
/************************************************************************/
BOOL	DbgCtrl_Init (void)
{
    // Create the Window
    char	myWindowClassName [256];
    WNDCLASSEX	myWindowClass;
    int		myResult;
    
    // Get the class name
    EdGUI_LoadString (IDS_DEBUG_CTRL_WINDOW_NAME, 
	myWindowClassName, sizeof (myWindowClassName));
    
    //
    // Register the debugger controller window class
    //
    myWindowClass.cbSize = sizeof (myWindowClass);
    // Set window class to redraw when window size changes
    myWindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    // Procedure to be called with messages for this window class
    myWindowClass.lpfnWndProc = MyDebugControllerWindowProcedure;
    // The extra space in class struct
    myWindowClass.cbClsExtra = 0;
    // The extra space in window struct for the pointer to text data
    // and editor window
    myWindowClass.cbWndExtra = 0;
    // The application's handle
    myWindowClass.hInstance = gProgram.applicationInstance;
    // Set the icon for this window class
    myWindowClass.hIcon = LoadIcon (gProgram.applicationInstance, 
	MAKEINTRESOURCE (APP_ICON));
    // Set the cursor for this window class
    myWindowClass.hCursor = LoadCursor (NULL, IDC_ARROW);
    // Set the background colour for this window
    myWindowClass.hbrBackground = GetSysColorBrush (COLOR_BTNFACE);
    // Set the menu for this window class
    myWindowClass.lpszMenuName = NULL;
    // Name of the window class
    myWindowClass.lpszClassName = myWindowClassName; 
    // Set the icon for this class.
    myWindowClass.hIconSm = LoadIcon (gProgram.applicationInstance, 
	MAKEINTRESOURCE (DEBUG_SMALL_ICON));
    
    myResult = RegisterClassEx (&myWindowClass);
    if (myResult == 0)
    {
	EdFail_Warn (IDS_REGISTERCLASSFAIL, __FILE__, __LINE__, 
	    GetLastError ());
	return FALSE;
    }
    
    // Create the debugger controller window    
    stDebuggerControllerWindow = CreateWindowEx (
	WS_EX_DLGMODALFRAME | WS_EX_TOPMOST,	// Extended styles
	myWindowClassName, 			// Window class
	"Debugger Controller",			// Window title
	WS_MINIMIZEBOX | WS_SYSMENU | WS_CLIPCHILDREN, 	// Window style
	CW_USEDEFAULT, 				// Initial x location
	CW_USEDEFAULT, 				// Initial y location
	CW_USEDEFAULT, 				// Initial x size
	CW_USEDEFAULT, 				// Initial y size
	HWND_DESKTOP, 				// Parent window handle
	NULL, 					// Window menu handle
	gProgram.applicationInstance,		// Program instance handle
	NULL);					// Creation parameters

    if (stDebuggerControllerWindow == NULL)
    {
	return FALSE;
    }

    return TRUE;
} // DbgCtrl_Init


/************************************************************************/
/* DbgCtrl_IsControllerVisible						*/
/************************************************************************/
BOOL	DbgCtrl_IsControllerVisible (void)
{
    return stDebuggerControllerVisible;
} // DbgCtrl_IsControllerVisible


/************************************************************************/
/* DbgCtrl_SetButtons							*/
/*									*/
/* Run -  	 Always active.  					*/
/*		 Reads "Run" when not running				*/
/*		 Reads "Stop" when executing, paused or waiting		*/
/* Pause - 	 Active when executing, paused or waiting		*/
/*		 Disabled when not running				*/
/*		 Reads "Pause" when not running, executing, waiting	*/
/*		 Reads "Resume" when paused				*/
/* Step	-	 Active when not running, paused or waiting		*/
/*		 Disabled when executing				*/
/* Step Over -	 Active when paused					*/
/*		 Disabled when not running, executing or waiting	*/
/* Step Return - Active when paused					*/
/*		 Disabled when not running, executing or waiting	*/
/************************************************************************/
void	DbgCtrl_SetButtons (int pmStatus)
{
    int		myRunButtonNameResource;
    int		myRunButtonCommand;
    int		myPauseButtonNameResource;
    BOOL	myPauseButtonEnable;
    BOOL	myStepButtonEnable;
    BOOL	myOtherStopButtonsEnable;
    char	myLabel [256];
    
    if (pmStatus == SET_BUTTON_NOT_RUNNING)
    {
    	// Set Run button to Run
    	myRunButtonNameResource = stItemInfo [RUN_BUTTON].nameResource;
	myRunButtonCommand = stItemInfo [RUN_BUTTON].command;
    	// Set Pause button to Pause
	myPauseButtonNameResource = stItemInfo [PAUSE_BUTTON].nameResource;
    	// Disable Pause button
    	myPauseButtonEnable = FALSE;
    	// Enable Step button
    	myStepButtonEnable = TRUE;
    	// Disable Step Over and Step Return buttons
    	myOtherStopButtonsEnable = FALSE;
    }
    else if (pmStatus == SET_BUTTON_IS_EXECUTING)
    {
    	// Set Run button to Stop
    	myRunButtonNameResource = stItemInfo [RUN_BUTTON].alternateNameResource;
	myRunButtonCommand = stItemInfo [RUN_BUTTON].alternateButtonCommand;	    
    	// Set Pause button to Pause
	myPauseButtonNameResource = stItemInfo [PAUSE_BUTTON].nameResource;
    	// Enable Pause button
    	myPauseButtonEnable = TRUE;
    	// Disable Step button
    	myStepButtonEnable = FALSE;
    	// Disable Step Over and Step Return buttons
    	myOtherStopButtonsEnable = FALSE;
    }
    else if (pmStatus == SET_BUTTON_PAUSED)
    {
    	// Set Run button to Stop
    	myRunButtonNameResource = stItemInfo [RUN_BUTTON].alternateNameResource;
	myRunButtonCommand = stItemInfo [RUN_BUTTON].alternateButtonCommand;	    
    	// Set Pause button to Resume
	myPauseButtonNameResource = 
			stItemInfo [PAUSE_BUTTON].alternateNameResource;
    	// Enable Pause button
    	myPauseButtonEnable = TRUE;
    	// Enable Step button
    	myStepButtonEnable = TRUE;
    	// Enable Step Over and Step Return buttons
    	myOtherStopButtonsEnable = TRUE;
    }
    else if (pmStatus == SET_BUTTON_WAITING)
    {
    	// Set Run button to Stop
    	myRunButtonNameResource = stItemInfo [RUN_BUTTON].alternateNameResource;
	myRunButtonCommand = stItemInfo [RUN_BUTTON].alternateButtonCommand;	    
    	// Set Pause button to Pause
	myPauseButtonNameResource = stItemInfo [PAUSE_BUTTON].nameResource;
    	// Enable Pause button
    	myPauseButtonEnable = TRUE;
    	// Enable Step button
    	myStepButtonEnable = TRUE;
    	// Disable Step Over and Step Return buttons
    	myOtherStopButtonsEnable = FALSE;
    }
    else
    {
    	// TW Assertion Failure
    }

    // Modify the Run button
    EdGUI_LoadString (myRunButtonNameResource, myLabel, sizeof (myLabel));
    SetWindowText (stItems [RUN_BUTTON], myLabel);
    RedrawWindow (stItems [RUN_BUTTON], NULL, NULL, RDW_UPDATENOW);
    RedrawWindow (stItems [RUN_BUTTON], NULL, NULL, RDW_INVALIDATE);
    SetWindowLong (stItems [RUN_BUTTON], GWL_ID, myRunButtonCommand);
    // Modify the Pause button
    EdGUI_LoadString (myPauseButtonNameResource, myLabel, sizeof (myLabel));
    SetWindowText (stItems [PAUSE_BUTTON], myLabel);
    RedrawWindow (stItems [PAUSE_BUTTON], NULL, NULL, RDW_UPDATENOW);
    RedrawWindow (stItems [PAUSE_BUTTON], NULL, NULL, RDW_INVALIDATE);
    // Set Pause button enable
    EnableWindow (stItems [PAUSE_BUTTON], myPauseButtonEnable);
    // Set Step button enable
    EnableWindow (stItems [STEP_BUTTON], myStepButtonEnable);
    // Set Step Over/Step Return button enable
    EnableWindow (stItems [STEP_OVER_BUTTON], myOtherStopButtonsEnable);
    EnableWindow (stItems [STEP_RETURN_BUTTON], myOtherStopButtonsEnable);
} // DbgCtrl_SetButtons


/************************************************************************/
/* DbgCtrl_SetStatus							*/
/************************************************************************/
void	DbgCtrl_SetStatus (const char *pmMessage, ...)
{
    va_list 	myArgList;
    char	myFullMessage [1024];
    
    va_start (myArgList, pmMessage);
    wvsprintf (myFullMessage, pmMessage, myArgList);
    va_end (myArgList);
    
    // TW
} // DbgCtrl_SetStatus


/************************************************************************/
/* DbgCtrl_ToggleControllerVisibility					*/
/************************************************************************/
void	DbgCtrl_ToggleControllerVisibility (void)
{
    if (stDebuggerControllerWindow == NULL)
    {
    }

    if (stDebuggerControllerVisible)
    {
        ShowWindow (stDebuggerControllerWindow, SW_HIDE);
    }
    else
    {
        ShowWindow (stDebuggerControllerWindow, SW_SHOWNORMAL);
    }

    stDebuggerControllerVisible = !stDebuggerControllerVisible;
} // DbgCtrl_ToggleControllerVisibility


/******************************/
/* Static callback procedures */
/******************************/
/************************************************************************/
/* MyDebugControllerWindowProcedure					*/
/*									*/
/* Callback procedure for the debugger controller window.		*/
/************************************************************************/
static LRESULT CALLBACK MyDebugControllerWindowProcedure (HWND pmController, 
							  UINT pmMessage, 
	    						  WPARAM pmWParam, 
	    						  LPARAM pmLParam)
{
    int	myPos;
    
    switch (pmMessage)
    {
    	case WM_CREATE:
    	    MyCreateDebuggerController (pmController);
    	    return 0;
    	    
        case WM_CLOSE:
            // Since the window must be visible to close it, it will hide 
            // the window.
            DbgCtrl_ToggleControllerVisibility ();
            return 0;

	case WM_COMMAND:
	    switch (LOWORD (pmWParam))
	    {
		case COMMAND_RUN:
		case COMMAND_STOP:
		case COMMAND_PAUSERESUME:
		    SendMessage (Ed_GetLastActiveEditWindow (),
				 WM_COMMAND, pmWParam, 0);
		    return 0;
		case COMMAND_DEBUG_STEP:
		    EdRun_SetStep (STEP);
		    break;
		case COMMAND_DEBUG_STEP_OVER:
		    EdRun_SetStep (STEP_OVER);
		    break;
		case COMMAND_DEBUG_STEP_RETURN:
		    EdRun_SetStep (STEP_RETURN);
		    break;
		case COMMAND_DEBUG_TRACE:
		    EdRun_ToggleTracing ();
		    break;
		case COMMAND_DEBUG_SHOWVARS:
		    // TW
            	    EdFail_Fatal (IDS_COMMAND_NOT_PROCESSED, __FILE__, 
            	        __LINE__, 0, LOWORD (pmWParam));
		    return 0;
		default:
		    // TW
            	    EdFail_Fatal (IDS_COMMAND_NOT_PROCESSED, __FILE__, 
            	        __LINE__, 0, LOWORD (pmWParam));
		    return 0;
	    }

	case WM_HSCROLL:
	    switch (LOWORD (pmWParam))
	    {
	    	case TB_THUMBPOSITION:
	    	case TB_THUMBTRACK:
	    	    myPos = HIWORD (pmWParam);
	    	    break;
	    	default:
	    	    myPos = SendMessage (stItems [TRACKER], TBM_GETPOS, 0, 0);
	    	    break;
	    }
	    EdRun_SetTraceSpeed (myPos);
	    return TRUE;
/*	    	    
	case WM_DESTROY:
	    break;

	case WM_SIZE:
	    return 0;
	    
        case WM_COMMAND:
            if (MyCommandHandler (pmEditWindow, LOWORD (pmWParam)))
            {
                return 0;
            }
            if (pmLParam == SENT_FROM_EDDISP)
            {
            }
	    return SendMessage (MyGetTextDisplayWindow (pmEditWindow), 
	    			WM_COMMAND, pmWParam, 
	    			(LPARAM) (SENT_FROM_EDWIN));


        case WM_ACTIVATE:
            if (LOWORD (pmWParam) == WA_INACTIVE)
            {
            	Ed_SetActiveWindow (NULL, NO_WINDOW);
            }
            else
            {
            	Ed_SetActiveWindow (pmEditWindow, EDIT_WINDOW);
            	SetFocus (MyGetTextDisplayWindow (pmEditWindow));
            }
            return 0;
            
        case WM_SETFOCUS:
	    if (!Ed_CheckWindowPreemption ())
	    {
		SetFocus (MyGetTextDisplayWindow (pmEditWindow));
	    }
            return 0;	

	case WM_GETMINMAXINFO:
	    if (GetWindowLong (pmEditWindow, WINDOW_MEM_WINDOW_INITIALIZED))
	    {
	    	MINMAXINFO	*myMinMaxInfo = (MINMAXINFO *) pmLParam;
	    	long		myWindowWidth, myWindowHeight;

		// Calculate the rectangle size
		MyGetWindowSize (pmEditWindow, MIN_ROWS, MIN_COLS, 
		    &myWindowWidth, &myWindowHeight);

		// Place the rectangle	    	
	    	myMinMaxInfo -> ptMinTrackSize.x = myWindowWidth;
	    	myMinMaxInfo -> ptMinTrackSize.y = myWindowHeight;
	    	  
	    	return 0;
	    }
	    break;
*/
    } // switch
    
    return DefWindowProc (pmController, pmMessage, pmWParam, pmLParam);
} // MyDebugControllerWindowProcedure


/*********************/
/* Static procedures */
/*********************/
static void	MyCreateDebuggerController (HWND pmController)
{
    char	myLabel [256];
    int		myWindowWidth, myWindowHeight, myScreenWidth, myScreenHeight;
    RECT	myWindowRect, myScreenRect;
    HFONT	myDialogFont;
    int		cnt;
   
    myDialogFont = (HFONT) GetStockObject (DEFAULT_GUI_FONT);
    
    // Create and place all the buttons
    for (cnt = 0 ; cnt < NUM_BUTTONS ; cnt++)
    {
    	EdGUI_LoadString (stItemInfo [cnt].nameResource,
			  myLabel, sizeof (myLabel));		  
	stItems [cnt] = CreateWindow (BUTTON_CLASS_NAME, myLabel,
        			      WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 
        			      stItemInfo [cnt].x, 
        			      stItemInfo [cnt].y, 
        			      stItemInfo [cnt].width, 
        			      stItemInfo [cnt].height, 
            			      pmController, 
            			      (HMENU) stItemInfo [cnt].command, 
            			      gProgram.applicationInstance, NULL);
        SendMessage (stItems [cnt], WM_SETFONT, (WPARAM) myDialogFont, 0);
    }
    
    // Create and place all the labels
    for (cnt = FIRST_LABEL ; cnt < FIRST_LABEL + NUM_LABELS ; cnt++)
    {
    	EdGUI_LoadString (stItemInfo [cnt].nameResource,
			  myLabel, sizeof (myLabel));
	stItems [cnt] = CreateWindow (STATIC_CLASS_NAME, myLabel, 
		      		      WS_CHILD | WS_VISIBLE | SS_LEFT, 
        	      		      stItemInfo [cnt].x, stItemInfo [cnt].y, 
        	      		      stItemInfo [cnt].width, 
        	      		      stItemInfo [cnt].height, 
            	      		      pmController, NULL, 
            	      		      gProgram.applicationInstance, NULL);
        SendMessage (stItems [cnt], WM_SETFONT, (WPARAM) myDialogFont, 0);
    }

    // Create and place the check boxes
    for (cnt = FIRST_CHECKBOX ; cnt < FIRST_CHECKBOX + NUM_CHECKBOXES; cnt++)
    {
    	EdGUI_LoadString (stItemInfo [cnt].nameResource,
			  myLabel, sizeof (myLabel));
    	stItems [cnt] = CreateWindow (BUTTON_CLASS_NAME, myLabel, 
        	      		      WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX |
        	      		      BS_MULTILINE, 
        	      		      stItemInfo [cnt].x, stItemInfo [cnt].y, 
        	      		      stItemInfo [cnt].width, 
        	      		      stItemInfo [cnt].height, 
		      		      pmController, 
		      		      (HMENU) stItemInfo [cnt].command, 
		      		      gProgram.applicationInstance, NULL);
        SendMessage (stItems [cnt], WM_SETFONT, (WPARAM) myDialogFont, 0);
    }
        
    // Create and place the track       
    stItems [TRACKER] =  CreateWindowEx (0, TRACKBAR_CLASS, "", 
    					 WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS, 
        	      		      	 stItemInfo [cnt].x, stItemInfo [cnt].y, 
        	      		      	 stItemInfo [cnt].width, 
        	      		      	 stItemInfo [cnt].height, 
		      		      	 pmController, 
		      		         (HMENU) stItemInfo [cnt].command, 
		      		         gProgram.applicationInstance, NULL);

    SendMessage (stItems [TRACKER], TBM_SETRANGE, (WPARAM) TRUE,
    		 (LPARAM) MAKELONG (0, 100));
    SendMessage (stItems [TRACKER], TBM_SETPAGESIZE, 0, (LPARAM) 25);
    SendMessage (stItems [TRACKER], TBM_SETPOS, (WPARAM) TRUE, (LPARAM) 33);
    SendMessage (stItems [TRACKER], TBM_SETTICFREQ, (WPARAM) 25, (LPARAM) 0);
 /*    
    CONTROL         "Tracing",IDC_CHECK1,"Button",BS_AUTOCHECKBOX | 
                    WS_TABSTOP,7,35,40,10
    CONTROL         "Show Vars",IDC_CHECK2,"Button",BS_AUTOCHECKBOX | 
                    WS_TABSTOP,231,35,50,10
    CONTROL         "Slider1",IDC_SLIDER1,"msctls_trackbar32",TBS_BOTH | 
                    TBS_NOTICKS | WS_TABSTOP,83,33,112,15
*/
   
    // Make the window the appropriate size
    myWindowRect.top = 0;
    myWindowRect.left = 0;
    myWindowRect.bottom = DIALOG_CONTROLLER_HEIGHT;
    myWindowRect.right = DIALOG_CONTROLLER_WIDTH;

    AdjustWindowRect (&myWindowRect, WS_CAPTION, FALSE);

    myWindowWidth = myWindowRect.right - myWindowRect.left;
    myWindowHeight = myWindowRect.bottom - myWindowRect.top;
    
    // Locate the window to the middle bottom of the screen
    myScreenRect = EdGUI_GetWorkArea (); 		   	
    myScreenWidth = myScreenRect.right - myScreenRect.left;
    myScreenHeight = myScreenRect.bottom - myScreenRect.top;
    SetWindowPos (pmController, NULL, 
	          (myScreenWidth - myWindowWidth) / 2 + myScreenRect.left,
	          myScreenRect.bottom - myWindowHeight,
	          myWindowWidth, myWindowHeight, SWP_NOZORDER);          
} // MyCreateDebuggerController
