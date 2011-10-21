/************************************************************************/
/* edbbar.c								*/
/*									*/
/* EdBBar uses the following routines in other modules:			*/
/*	EdFail - various						*/
/*	EdGUI - EdGUI_CreateFont, EdGUI_LoadResource			*/
/************************************************************************/

/*******************/
/* System includes */
/*******************/
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <crtdbg.h>

/****************/
/* Self include */
/****************/
#include "edbbar.h"

/******************/
/* Other includes */
/******************/
#include "edfail.h"
#include "edglob.h"
#include "edgui.h"
#include "edprog.h"
#include "edwin.h"

#include "edcommands.h"
#include "eddialogs.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
// This is not normally defined unless _WIN32_IE has been defined to be
// 0x400 or higher (i.e. we assume IE 4.0 has been installed.)  In our case,
// we don't depend upon it, but we do take advantage of it if it is available.
#ifndef NM_RELEASEDCAPTURE
#define NM_RELEASEDCAPTURE      (NM_FIRST-16)
#endif

// Windows class name for a button
#define BUTTON_CLASS_NAME		"button"
#define STATIC_CLASS_NAME		"static"

// The number and type of controls
#define NUM_CONTROLS			20
#define NUM_FRAMES			 8

#define NUM_BUTTONS			15
#define FIRST_CHECKBOX			15
#define NUM_CHECKBOXES			 2
#define FIRST_LABEL			17
#define NUM_LABELS			 2

// Control numbers
#define RUN_BUTTON			 0
#define PAUSE_BUTTON			 1
#define OPEN_BUTTON			 2
#define SAVE_BUTTON			 3
#define INDENT_BUTTON			 4
#define PRINT_BUTTON		 	 5
#define CUT_BUTTON			 6
#define COPY_BUTTON			 7
#define PASTE_BUTTON			 8
#define FIND_BUTTON			 9
#define FIND_NEXT_BUTTON		10
#define REPLACE_BUTTON			11
#define STEP_BUTTON			12
#define STEP_OVER_BUTTON		13
#define STEP_RETURN_BUTTON		14

#define TRACE_CHECKBOX			15
#define SHOW_VARS_CHECKBOX		16
#define FAST_LABEL			17
#define SLOW_LABEL			18
#define TRACKER			        19

#define RUN_PAUSE_FRAME			 0
#define OPEN_SAVE_FRAME			 1
#define INDENT_PRINT_FRAME		 2
#define CUT_COPY_PASTE_FRAME		 3
#define FIND_REPLACE_FRAME		 4
#define DEBUGGER_FRAME1			 5
#define DEBUGGER_FRAME2			 6
#define DEBUGGER_FRAME3			 7

/********************/
/* Global variables */
/********************/

/*********/
/* Types */
/*********/
typedef struct WordRect
{
    WORD	x;
    WORD	y;
    WORD	width;
    WORD	height;
} WordRect;

typedef struct ControlData
{
    WORD	nameResource;
    WORD	alternateNameResource;
    WORD	command, alternateCommand;
    WORD	x, y, width, height;
} ControlData;

typedef struct FrameData
{
    WORD	x, y, width, height;
} FrameData;

typedef struct ButtonBarData
{
    WORD	height;
    ControlData	controlData [NUM_CONTROLS];
    FrameData	frameData [NUM_FRAMES];
} ButtonBarData;

typedef struct
{
    // The elements in the window
    HWND	control [NUM_CONTROLS];
    HWND	raisedArea [NUM_FRAMES];
} ButtonBarWindowInfo, *ButtonBarWindowInfoPtr;

/**********************/
/* External variables */
/**********************/

/********************/
/* Static constants */
/********************/

/********************/
/* Static variables */
/********************/
// Properties of the status window
static EdBBar_Properties	stProperties;
static BOOL			stPropertiesChanged;

//
static HBRUSH		stBackgroundBrush;
static HFONT		stButtonFont;

// Data about position of controls
static ButtonBarData	stButtonBarData;

static BOOL		stDebuggerControlsVisible = FALSE;

// The last status of a button bar
static int		stLastBBarStatus = SET_BUTTON_NOT_RUNNING;
    	
/******************************/
/* Static callback procedures */
/******************************/
static LRESULT CALLBACK MyButtonBarWindowProcedure (HWND pmButtonBarWindow, 
					            UINT pmMessage, 
					            WPARAM pmWParam, 
					            LPARAM pmLParam);
static LRESULT CALLBACK	MyRaisedFrameWindowProcedure (HWND pmWindow, 
						      UINT pmMessage, 
						      WPARAM pmWParam, 
						      LPARAM pmLParam);

/*********************/
/* Static procedures */
/*********************/
static void			MyConvertToPixelLocs (WordRect *pmRect, 
						      int x, int y, 
						      int width, int height);
static int			MyCreateButtonBar (HWND pmButtonBarWindow);
static ButtonBarWindowInfoPtr	MyCreateButtonBarWindowInfo (void);
static void			MyDispose (HWND pmButtonBarWindow);
static void			MyDrawShadedFrame (HDC pmDeviceContext, 
						   RECT *pmRectPtr, 
		    				   BOOL pmDepressed);
static ButtonBarWindowInfoPtr	MyGetButtonBarWindowInfo (
						HWND pmButtonBarWindow);
static void			MyResizeButtonBar (HWND pmButtonBarWindow);


/***********************/
/* External procedures */
/***********************/
/*************************************************************************/
/* EdStat_Init								 */
/*************************************************************************/
BOOL	EdBBar_Init (void)
{
    char	myButtonBarWindowClassName [256];
    char	myRaisedFrameWindowClassName [256];
    int		myResult;
    WNDCLASSEX	myWindowClass, myRaisedFrameClass;

    // Get the background brush
    stBackgroundBrush = GetSysColorBrush (COLOR_BTNFACE);
    if (stBackgroundBrush == NULL)	  
    {
    	EdFail_Warn (IDS_CREATEBRUSHFAIL, __FILE__, __LINE__, GetLastError ());
    	return FALSE;
    }

    stButtonFont = (HFONT) GetStockObject (DEFAULT_GUI_FONT);

    // Get the class name
    EdGUI_LoadString (IDS_BBAR_WINDOW_NAME, myButtonBarWindowClassName, 
	sizeof (myButtonBarWindowClassName));
		     
    //
    // Register the status window class
    //
    myWindowClass.cbSize = 	  sizeof (myWindowClass);
    // Set window class to redraw when window size changes
    myWindowClass.style = 	  CS_HREDRAW | CS_VREDRAW;
    // Procedure to be called with messages for this window class
    myWindowClass.lpfnWndProc =   MyButtonBarWindowProcedure;
    // The extra space in class struct
    myWindowClass.cbClsExtra = 	  0;
    // The extra space in window struct for the pointer to text data
    // and editor window
    myWindowClass.cbWndExtra = 	  WINDOW_EXTRA_MEMORY;
    // The application's handle
    myWindowClass.hInstance = 	  gProgram.applicationInstance;
    // Set the icon for this window class
    myWindowClass.hIcon = 	  NULL;
    // Set the cursor for this window class
    myWindowClass.hCursor = 	  LoadCursor (NULL, IDC_ARROW);
    // Set the background colour for this window
    myWindowClass.hbrBackground = stBackgroundBrush;
    // Set the menu for this window class
    myWindowClass.lpszMenuName =  NULL;
    // Name of the window class
    myWindowClass.lpszClassName = myButtonBarWindowClassName; 
    // Set the icon for this class.
    myWindowClass.hIconSm = 	  NULL;
    
    myResult = RegisterClassEx (&myWindowClass);
    if (myResult == 0)
    {
    	EdFail_Warn (IDS_REGISTERCLASSFAIL, __FILE__, __LINE__, 
    			  GetLastError ());
    	return FALSE;
    }
    
    // Get the class name
    EdGUI_LoadString (IDS_RAISED_FRAME_WINDOW_NAME, myRaisedFrameWindowClassName, 
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
    myRaisedFrameClass.hbrBackground = stBackgroundBrush;
    // Set the menu for this window class
    myRaisedFrameClass.lpszMenuName =  NULL;
    // Name of the window class
    myRaisedFrameClass.lpszClassName = myRaisedFrameWindowClassName; 
    // Set the icon for this class.
    myRaisedFrameClass.hIconSm =       NULL;
    
    myResult = RegisterClassEx (&myRaisedFrameClass);
    if (myResult == 0)
    {
    	EdFail_Warn (IDS_REGISTERCLASSFAIL, __FILE__, __LINE__, 
    			  GetLastError ());
    	return FALSE;
    }

    // Load the resource containing all the button positions    
    if (!EdGUI_LoadResource (NULL, TOP_BUTTON_DATA, &stButtonBarData, 
    			     sizeof (ButtonBarData)))
    {
    	return FALSE;
    }    			     
    return TRUE;
} // EdBBar_Init


/*************************************************************************/
/* EdBBar_Finalize							 */
/*************************************************************************/
void	EdBBar_Finalize (void)
{
} // EdBBar_Finalize


/************************************************************************/
/* EdBBar_PropertiesSet							*/
/************************************************************************/
void	EdBBar_PropertiesSet (EdBBar_Properties pmProperties)
{
    stProperties = pmProperties;
    stPropertiesChanged = TRUE;
} // EdBBar_PropertiesSet

	
/************************************************************************/
/* EdBBar_PropertiesImplementChanges					*/
/************************************************************************/
void	EdBBar_PropertiesImplementChanges (HWND pmButtonBarWindow)
{
    if (stPropertiesChanged)
    {
        stPropertiesChanged = FALSE;
    }
} // EdBBar_PropertiesImplementChanges


/*************************************************************************/
/* EdBBar_Create							 */
/*************************************************************************/
HWND	EdBBar_Create (HWND pmEditWindow)
{
    HINSTANCE	myApplicationInstance;
    char	myButtonBarWindowClassName [256];
    HWND	myButtonBarWindow;
    
    // Get the program instance
    myApplicationInstance = 
        (HINSTANCE) GetWindowLong (pmEditWindow, GWL_HINSTANCE);
        
    // Load the class name (we've already loaded it once, so no error checking)    
    EdGUI_LoadString (IDS_BBAR_WINDOW_NAME, myButtonBarWindowClassName, 
	sizeof (myButtonBarWindowClassName));

    // Create the button bar window
    myButtonBarWindow = CreateWindow (
    	myButtonBarWindowClassName, 		// Window class
        NULL,	 				// Window title
        WS_CHILDWINDOW | WS_VISIBLE,	 	// Window style
    	0, 0, 0, 0, 				// Initial location & size
    	pmEditWindow, 				// Parent window handle
    	NULL, 					// Window menu handle
    	myApplicationInstance,	 		// Program instance handle
    	NULL);					// Creation parameters

    // Set the new windows buttons to the same state as the other windows
    EdBBar_SetButtons (myButtonBarWindow, stLastBBarStatus);

    return myButtonBarWindow;
} // EdBBar_Create


/************************************************************************/
/* EdBBar_AreDebuggerControlsVisible					*/
/************************************************************************/
BOOL	EdBBar_AreDebuggerControlsVisible (void)
{
    return stDebuggerControlsVisible;
} // EdBBar_AreDebuggerControlsVisible


/*************************************************************************/
/* EdBBar_GetWindowHeight						 */
/*************************************************************************/
int	EdBBar_GetWindowHeight (void)
{
    long    myDialogSizes = GetDialogBaseUnits ();

    return MulDiv (stButtonBarData.height, HIWORD (myDialogSizes), 8);
} // EdBBar_GetWindowHeight


/*************************************************************************/
/* EdBBar_SetButtons							 */
/*************************************************************************/
void	EdBBar_SetButtons (HWND pmButtonBarWindow, int pmStatus)
{
    ButtonBarWindowInfoPtr	myInfo;
    int				myRunButtonNameResource;
    int				myRunButtonCommand;
    int				myPauseButtonNameResource;
    BOOL			myPauseButtonEnable;
    BOOL			myStepButtonEnable;
    BOOL			myOtherStopButtonsEnable;
    char			myLabel [256];
    
    myInfo = MyGetButtonBarWindowInfo (pmButtonBarWindow);
    
    stLastBBarStatus = pmStatus;

    if (pmStatus == SET_BUTTON_NOT_RUNNING)
    {
    	// Set Run button to Run
    	myRunButtonNameResource = 
    		stButtonBarData.controlData [RUN_BUTTON].nameResource;
	myRunButtonCommand = 
		stButtonBarData.controlData [RUN_BUTTON].command;
    	// Set Pause button to Pause
	myPauseButtonNameResource = 
		stButtonBarData.controlData [PAUSE_BUTTON].nameResource;
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
    	myRunButtonNameResource = 
    		stButtonBarData.controlData [RUN_BUTTON].alternateNameResource;
	myRunButtonCommand = 
		stButtonBarData.controlData [RUN_BUTTON].alternateCommand;	    
    	// Set Pause button to Pause
	myPauseButtonNameResource = 
		stButtonBarData.controlData [PAUSE_BUTTON].nameResource;
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
    	myRunButtonNameResource = 
    		stButtonBarData.controlData [RUN_BUTTON].alternateNameResource;
	myRunButtonCommand = 
		stButtonBarData.controlData [RUN_BUTTON].alternateCommand;	    
    	// Set Pause button to Resume
	myPauseButtonNameResource = 
		stButtonBarData.controlData [PAUSE_BUTTON].alternateNameResource;
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
    	myRunButtonNameResource = 
    		stButtonBarData.controlData [RUN_BUTTON].alternateNameResource;
	myRunButtonCommand = 
		stButtonBarData.controlData [RUN_BUTTON].alternateCommand;	    
    	// Set Pause button to Pause
	myPauseButtonNameResource = 
		stButtonBarData.controlData [PAUSE_BUTTON].nameResource;
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
    SetWindowText (myInfo -> control [RUN_BUTTON], myLabel);
    RedrawWindow (myInfo -> control [RUN_BUTTON], NULL, NULL, RDW_UPDATENOW);
    RedrawWindow (myInfo -> control [RUN_BUTTON], NULL, NULL, RDW_INVALIDATE);
    SetWindowLong (myInfo -> control [RUN_BUTTON], GWL_ID, myRunButtonCommand);
    // Modify the Pause button
    EdGUI_LoadString (myPauseButtonNameResource, myLabel, sizeof (myLabel));
    SetWindowText (myInfo -> control [PAUSE_BUTTON], myLabel);
    RedrawWindow (myInfo -> control [PAUSE_BUTTON], NULL, NULL, RDW_UPDATENOW);
    RedrawWindow (myInfo -> control [PAUSE_BUTTON], NULL, NULL, RDW_INVALIDATE);
    // Set Pause button enable
    EnableWindow (myInfo -> control [PAUSE_BUTTON], myPauseButtonEnable);
    // Set Step button enable
    EnableWindow (myInfo -> control [STEP_BUTTON], myStepButtonEnable);
    // Set Step Over/Step Return button enable
    EnableWindow (myInfo -> control [STEP_OVER_BUTTON], 
    		  myOtherStopButtonsEnable);
    EnableWindow (myInfo -> control [STEP_RETURN_BUTTON], 
    		  myOtherStopButtonsEnable);
} // EdBBar_SetButtons


/************************************************************************/
/* EdBBar_SetDebuggerVisible						*/
/*									*/
/* After this is called, all the window *must* be resized.		*/
/************************************************************************/
void	EdBBar_SetDebuggerVisible (int pmVisible)
{
    if (pmVisible == SHOW_DEBUGGER_CONTROLS)
    {
    	stDebuggerControlsVisible = TRUE;
    }
    else if (pmVisible == HIDE_DEBUGGER_CONTROLS)
    {
    	stDebuggerControlsVisible = FALSE;
    }
    else if (pmVisible == TOGGLE_DEBUGGER_CONTROLS)
    {
    	stDebuggerControlsVisible = !stDebuggerControlsVisible;
    }
    else
    {
    	// TW Assert Failure
    }
} // EdBBar_SetDebuggerVisible


/******************************/
/* Static callback procedures */
/******************************/
/************************************************************************/
/* MyButtonBarWindowProcedure						*/
/*						 		        */
/* Callback procedure for the button bar window.			*/
/************************************************************************/
static LRESULT CALLBACK MyButtonBarWindowProcedure (HWND pmButtonBarWindow, 
					            UINT pmMessage, 
					            WPARAM pmWParam, 
					            LPARAM pmLParam)
{
    HDC				myDeviceContext;
    PAINTSTRUCT			myPaintStruct;
    RECT			myWindowRect;
    ButtonBarWindowInfoPtr	myInfo;
    int				myPos;
        
    switch (pmMessage)
    {
    	case WM_CREATE:
    	    return MyCreateButtonBar (pmButtonBarWindow);
    	    
	case WM_DESTROY:
	    MyDispose (pmButtonBarWindow);
	    break;
	    	    
	case WM_SIZE:
	    MyResizeButtonBar (pmButtonBarWindow);
	    return 0;
	    
    	case WM_PAINT:
    	    if (!stProperties.oneWindowMode)
    	    {
    	    	myDeviceContext = BeginPaint (pmButtonBarWindow, 
    	    	    &myPaintStruct);
    	    	GetClientRect (pmButtonBarWindow, &myWindowRect);
    	    	MoveToEx (myDeviceContext, 0, myWindowRect.bottom - 1, NULL);
    	    	LineTo (myDeviceContext, myWindowRect.right, 
    	    	    myWindowRect.bottom - 1);
    	    	EndPaint (pmButtonBarWindow, &myPaintStruct);
    	    	return 0;
    	    }
	    break;
	    
	case WM_COMMAND:
	    // Pass the command to the parent
	    SetFocus (GetParent (pmButtonBarWindow));
	    return SendMessage (GetParent (pmButtonBarWindow), WM_COMMAND,
	    			pmWParam, pmLParam);

	case WM_HSCROLL:
	    switch (LOWORD (pmWParam))
	    {
	    	case TB_THUMBPOSITION:
	    	case TB_THUMBTRACK:
	    	    myPos = HIWORD (pmWParam);
	    	    break;
	    	default:
	    	    myInfo = MyGetButtonBarWindowInfo (pmButtonBarWindow);
	    	    myPos = SendMessage (myInfo -> control [TRACKER], 
	    	    			 TBM_GETPOS, 0, 0);
	    	    break;
	    }
	    return SendMessage (GetParent (pmButtonBarWindow), WM_SETTRACESPEED,
	    			myPos, 0);

	case WM_NOTIFY:
	    // When user finished with the slider, transfer control back to 
	    // the main window.
    	    if ((((NMHDR FAR *) pmLParam) -> code) == NM_RELEASEDCAPTURE)
    	    {
	    	SetFocus (GetParent (pmButtonBarWindow));
	    }
	    return 0;

	case WM_SETTRACECHECKBOX:
	    myInfo = MyGetButtonBarWindowInfo (pmButtonBarWindow);
	    SendMessage (myInfo -> control [TRACE_CHECKBOX], BM_SETCHECK,
	    		 pmWParam ? BST_CHECKED : BST_UNCHECKED, 0);
	    return 0;

        case WM_SETSHOWVARSCHECKBOX:
	    myInfo = MyGetButtonBarWindowInfo (pmButtonBarWindow);
	    SendMessage (myInfo -> control [SHOW_VARS_CHECKBOX], BM_SETSTATE,
	    		 pmWParam, 0);
	    return 0;

        case WM_SETTRACESPEEDSLIDER:
	    myInfo = MyGetButtonBarWindowInfo (pmButtonBarWindow);
	    SendMessage (myInfo -> control [TRACKER], TBM_SETPOS,
	    		 TRUE, pmWParam);
	    return 0;
	    	    
	case WM_SYSCOLORCHANGE:
	    //
	    // Set the new background colours
	    //
	    
	    // Create the new background brush
	    stBackgroundBrush = GetSysColorBrush (COLOR_BTNFACE);
	    
	    // Set the window to be redrawn
	    InvalidateRect (pmButtonBarWindow, NULL, TRUE);
	    return 0;
    } // switch

    return DefWindowProc (pmButtonBarWindow, pmMessage, pmWParam, pmLParam);
} // MyButtonBarWindowProcedure


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


/*********************/
/* Static procedures */
/*********************/
/************************************************************************/
/* MyConvertToPixelLocs							*/
/************************************************************************/
static void	MyConvertToPixelLocs (WordRect *pmRect, int x, int y, 
				      int width, int height)
{
    long    myDialogSizes = GetDialogBaseUnits ();

    pmRect -> x = MulDiv (x, LOWORD (myDialogSizes), 4);
    pmRect -> y = MulDiv (y, HIWORD (myDialogSizes), 8);
    pmRect -> width = MulDiv (width, LOWORD (myDialogSizes), 4);
    pmRect -> height = MulDiv (height, HIWORD (myDialogSizes), 8);
} // MyConvertToPixelLocs


/************************************************************************/
/* MyCreateButtonBar							*/
/************************************************************************/
static int	MyCreateButtonBar (HWND pmButtonBarWindow)
{
    HINSTANCE			myApplicationInstance;
    ButtonBarWindowInfoPtr	myInfo;
    char			myButtonName [256], myLabel [256];
    char			myRaisedFrameWindowClassName [256];
    WordRect			myRect;
    int				cnt;
    static BOOL			stButtonBarButtonSizesInitialized = FALSE;
    
    // Get the program instance
    myApplicationInstance = 
        (HINSTANCE) GetWindowLong (pmButtonBarWindow, GWL_HINSTANCE);
        
    //
    // Initialize the extra window memory
    //

    // Set the window type            
    SetWindowLong (pmButtonBarWindow, WINDOW_MEM_WINDOW_TYPE, 
        (long) WINDOW_TYPE_BEGINNER_TOP);

    // Set the window initialized                
    SetWindowLong (pmButtonBarWindow, WINDOW_MEM_WINDOW_INITIALIZED, 
        (long) FALSE);
           
    // Create the status window information
    myInfo = MyCreateButtonBarWindowInfo ();
    if (myInfo == NULL)
    {
	return -1;
    }
    SetWindowLong (pmButtonBarWindow, WINDOW_MEM_WINDOW_INFO, (long) myInfo);
   	    
    //
    // Add the elements to the window
    //
    EdBBar_PropertiesImplementChanges (pmButtonBarWindow);
   	    
    EdGUI_LoadString (IDS_RAISED_FRAME_WINDOW_NAME, 
        myRaisedFrameWindowClassName, sizeof (myRaisedFrameWindowClassName));
		
    // Create all the raised areas
    for (cnt = 0 ; cnt < NUM_FRAMES ; cnt++)
    {
	MyConvertToPixelLocs (&myRect, stButtonBarData.frameData [cnt].x, 
	    stButtonBarData.frameData [cnt].y,
	    stButtonBarData.frameData [cnt].width, 
	    stButtonBarData.frameData [cnt].height);

    	myInfo -> raisedArea [cnt] = CreateWindow (myRaisedFrameWindowClassName,
    	    NULL, WS_CHILD | WS_DISABLED, myRect.x, myRect.y, myRect.width,
    	    myRect.height, pmButtonBarWindow, NULL, myApplicationInstance, NULL);
    }
 
    // Create all the buttons
    for (cnt = 0 ; cnt < NUM_BUTTONS ; cnt++)
    {
        // Get the button name
        EdGUI_LoadString (stButtonBarData.controlData [cnt].nameResource,
			  myButtonName, sizeof (myButtonName));

	MyConvertToPixelLocs (&myRect, stButtonBarData.controlData [cnt].x, 
	    stButtonBarData.controlData [cnt].y,
	    stButtonBarData.controlData [cnt].width, 
	    stButtonBarData.controlData [cnt].height);

        myInfo -> control [cnt] = CreateWindow (BUTTON_CLASS_NAME,
            myButtonName, WS_CHILD | BS_PUSHBUTTON, 
            myRect.x, myRect.y, myRect.width, myRect.height, pmButtonBarWindow,
            (HMENU) stButtonBarData.controlData [cnt].command, 
            myApplicationInstance, NULL);
        SendMessage (myInfo -> control [cnt], WM_SETFONT, 
            	     (WPARAM) stButtonFont, 0);
    }		     
    
    // Create and place the check boxes
    for (cnt = FIRST_CHECKBOX ; cnt < FIRST_CHECKBOX + NUM_CHECKBOXES; cnt++)
    {
    	EdGUI_LoadString (stButtonBarData.controlData [cnt].nameResource,
			  myLabel, sizeof (myLabel));

	MyConvertToPixelLocs (&myRect, stButtonBarData.controlData [cnt].x, 
	    stButtonBarData.controlData [cnt].y,
	    stButtonBarData.controlData [cnt].width, 
	    stButtonBarData.controlData [cnt].height);

    	myInfo -> control [cnt] = CreateWindow (BUTTON_CLASS_NAME, myLabel, 
            WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_MULTILINE, 
            myRect.x, myRect.y, myRect.width, myRect.height, pmButtonBarWindow,
            (HMENU) stButtonBarData.controlData [cnt].command, 
            myApplicationInstance, NULL);
        SendMessage (myInfo -> control [cnt], WM_SETFONT, 
        	     (WPARAM) stButtonFont, 0);
    }
        
    // Create and place all the labels
    for (cnt = FIRST_LABEL ; cnt < FIRST_LABEL + NUM_LABELS ; cnt++)
    {
    	WORD	myFlag;
    	
    	EdGUI_LoadString (stButtonBarData.controlData [cnt].nameResource,
			  myLabel, sizeof (myLabel));
	if (cnt == FIRST_LABEL) myFlag = SS_RIGHT;
	else myFlag = SS_LEFT;			  

	MyConvertToPixelLocs (&myRect, stButtonBarData.controlData [cnt].x, 
	    stButtonBarData.controlData [cnt].y,
	    stButtonBarData.controlData [cnt].width, 
	    stButtonBarData.controlData [cnt].height);

	myInfo -> control [cnt] = CreateWindow (STATIC_CLASS_NAME, 
	    myLabel, WS_CHILD | WS_VISIBLE | myFlag, 
            myRect.x, myRect.y, myRect.width, myRect.height,
            pmButtonBarWindow, NULL, gProgram.applicationInstance, NULL);
        SendMessage (myInfo -> control [cnt], WM_SETFONT, 
        	     (WPARAM) stButtonFont, 0);
    }

    // Create and place the track       

    MyConvertToPixelLocs (&myRect, stButtonBarData.controlData [TRACKER].x,
	stButtonBarData.controlData [TRACKER].y,
	stButtonBarData.controlData [TRACKER].width, 
	stButtonBarData.controlData [TRACKER].height);

    myInfo -> control [TRACKER] = CreateWindowEx (0, TRACKBAR_CLASS, "", 
    	WS_CHILD | WS_VISIBLE, 
        myRect.x, myRect.y, myRect.width, myRect.height, pmButtonBarWindow,
        (HMENU) stButtonBarData.controlData [TRACKER].command, 
        myApplicationInstance, NULL);
    SendMessage (myInfo -> control [TRACKER], TBM_SETRANGE, (WPARAM) TRUE,
    		 (LPARAM) MAKELONG (0, 100));
    SendMessage (myInfo -> control [TRACKER], TBM_SETPAGESIZE, 0, 
    		 (LPARAM) 25);
    SendMessage (myInfo -> control [TRACKER], TBM_SETPOS, (WPARAM) TRUE, 
    		 (LPARAM) 33);
    SendMessage (myInfo -> control [TRACKER], TBM_SETBUDDY, (WPARAM) FALSE, 
    		 (LPARAM) myInfo -> control [SLOW_LABEL]);
    SendMessage (myInfo -> control [TRACKER], TBM_SETBUDDY, (WPARAM) TRUE, 
    		 (LPARAM) myInfo -> control [FAST_LABEL]);
    		

    // Set the window to initialized
    SetWindowLong (pmButtonBarWindow, WINDOW_MEM_WINDOW_INITIALIZED, (long) TRUE);

    return 0;
} // MyCreateButtonBar


/************************************************************************/
/* MyCreateButtonBarWindowInfo						*/
/************************************************************************/
static ButtonBarWindowInfoPtr	MyCreateButtonBarWindowInfo (void)
{
    ButtonBarWindowInfoPtr	myInfo;
    
    // Allocate the structure for the information about the status window
    myInfo = (ButtonBarWindowInfoPtr) malloc (sizeof (ButtonBarWindowInfo));
    
    if (myInfo != NULL)
    {
    	ZeroMemory (myInfo, sizeof (ButtonBarWindowInfo));
    }

    return myInfo;  
} // MyCreateButtonBarWindowInfo


/*************************************************************************/
/* MyDispose								 */
/*************************************************************************/
static void	MyDispose (HWND pmButtonBarWindow)
{
    ButtonBarWindowInfoPtr	myInfo;
    
    myInfo = MyGetButtonBarWindowInfo (pmButtonBarWindow);
    free (myInfo);
} // MyDispose


/************************************************************************/
/* MyGetButtonBarWindowInfo						*/
/************************************************************************/
static ButtonBarWindowInfoPtr	MyGetButtonBarWindowInfo (
						HWND pmButtonBarWindow)
{
    int				myWindowType;
    ButtonBarWindowInfoPtr	myInfo;
    
    myWindowType = GetWindowLong (pmButtonBarWindow, WINDOW_MEM_WINDOW_TYPE);
    if (myWindowType != WINDOW_TYPE_BEGINNER_TOP)
    {
    	EdFail_Fatal (IDS_BADWINDOWTYPE, __FILE__, __LINE__, 0,
    		      myWindowType, WINDOW_TYPE_BEGINNER_TOP);
    }
    	
    myInfo = (ButtonBarWindowInfoPtr) GetWindowLong (pmButtonBarWindow, 
        WINDOW_MEM_WINDOW_INFO);
    if (myInfo == NULL)
    {
    	EdFail_Fatal (IDS_GETWINDOWLONGFAIL, __FILE__, __LINE__,
    		      GetLastError (), WINDOW_MEM_WINDOW_INFO);
    }
       
    return myInfo;
} // MyGetButtonBarWindowInfo


/************************************************************************/
/* MyResizeButtonBar							*/
/************************************************************************/
static void	MyResizeButtonBar (HWND pmButtonBarWindow)
{
    FrameData			*myFrameData;
    RECT			myWindowRect;
    int				myWindowWidth;
    ButtonBarWindowInfoPtr	myInfo;
    int				myToButton, myToArea;
    long			myDialogSizes;
    int				cnt;
        
    // The first resizing comes before there's a client area, so
    // we put this check here to ignore it
    if (GetClientRect (pmButtonBarWindow, &myWindowRect) == 0)
    {
    	return;
    }

    if (!GetWindowLong (pmButtonBarWindow, WINDOW_MEM_WINDOW_INITIALIZED))
    {
    	return;
    }

    // We have the window width in pixels.  Now convert it to 
    // dialog box units
    myWindowWidth = myWindowRect.right - myWindowRect.left;
    myDialogSizes = GetDialogBaseUnits ();
    myWindowWidth = MulDiv (myWindowWidth, 4, LOWORD (myDialogSizes));

    // Set the posiiton of the three components
    myInfo = MyGetButtonBarWindowInfo (pmButtonBarWindow);

    myFrameData = &stButtonBarData.frameData [0];
    if (stDebuggerControlsVisible)
    {
    	myToButton = PAUSE_BUTTON;
    	myToArea = RUN_PAUSE_FRAME;
    }
    else if (myWindowWidth >= myFrameData [FIND_REPLACE_FRAME].x +
    			 myFrameData [FIND_REPLACE_FRAME].width + 2)
    {
    	myToButton = REPLACE_BUTTON;
    	myToArea = FIND_REPLACE_FRAME;
    }
    else if (myWindowWidth >= myFrameData [CUT_COPY_PASTE_FRAME].x +
    			      myFrameData [CUT_COPY_PASTE_FRAME].width + 2)
    {
    	myToButton = PASTE_BUTTON;
    	myToArea = CUT_COPY_PASTE_FRAME;
    }
    else if (myWindowWidth >= myFrameData [INDENT_PRINT_FRAME].x +
    			      myFrameData [INDENT_PRINT_FRAME].width + 2)
    {
    	myToButton = PRINT_BUTTON;
    	myToArea = INDENT_PRINT_FRAME;
    }
    else if (myWindowWidth >= myFrameData [OPEN_SAVE_FRAME].x +
    			      myFrameData [OPEN_SAVE_FRAME].width + 2)
    {
    	myToButton = SAVE_BUTTON;
    	myToArea = OPEN_SAVE_FRAME;
    }
    else if (myWindowWidth >= myFrameData [RUN_PAUSE_FRAME].x +
    			      myFrameData [RUN_PAUSE_FRAME].width + 2)
    {
    	myToButton = PAUSE_BUTTON;
    	myToArea = RUN_PAUSE_FRAME;
    }
    else
    {
    	myToButton = -1;
    	myToArea = -1;
    }
    
    // Show and hide the appropriate regular buttons
    for (cnt = RUN_BUTTON ; cnt <= myToButton ; cnt++)
    {
    	ShowWindow (myInfo -> control [cnt], SW_SHOW);
    }    	
    for (cnt = myToButton + 1 ; cnt <= REPLACE_BUTTON ; cnt++)
    {
    	ShowWindow (myInfo -> control [cnt], SW_HIDE);
    }

    // Show and hide the appropriate regular frames    
    for (cnt = RUN_PAUSE_FRAME ; cnt <= myToArea ; cnt++)
    {
    	ShowWindow (myInfo -> raisedArea [cnt], SW_SHOW);
    }    	
    for (cnt = myToArea + 1 ; cnt <= FIND_REPLACE_FRAME ; cnt++)
    {
   	ShowWindow (myInfo -> raisedArea [cnt], SW_HIDE);
    }

    if (stDebuggerControlsVisible)
    {
    	if (myWindowWidth >= myFrameData [DEBUGGER_FRAME3].x +
	    		     myFrameData [DEBUGGER_FRAME3].width + 2)
    	{
    	    myToButton = TRACKER;
    	    myToArea = DEBUGGER_FRAME3;
    	}
        else if (myWindowWidth >= myFrameData [DEBUGGER_FRAME2].x +
    			          myFrameData [DEBUGGER_FRAME2].width + 2)
    	{
    	    myToButton = SHOW_VARS_CHECKBOX;
    	    myToArea = DEBUGGER_FRAME2;
    	}
    	else if (myWindowWidth >= myFrameData [DEBUGGER_FRAME1].x +
    			          myFrameData [DEBUGGER_FRAME1].width + 2)
    	{
    	    myToButton = STEP_RETURN_BUTTON;
    	    myToArea = DEBUGGER_FRAME1;
    	}
    	else
    	{
    	    myToButton = -1;
    	    myToArea = -1;
    	}
    	
    	// Show and hide the appropriate debugger controls
    	for (cnt = STEP_BUTTON ; cnt <= myToButton ; cnt++)
    	{
    	    ShowWindow (myInfo -> control [cnt], SW_SHOW);
    	}
    	for (cnt = myToButton + 1 ; cnt <= TRACKER ; cnt++)
    	{
    	    ShowWindow (myInfo -> control [cnt], SW_HIDE);
    	}
    	
    	// Show and hide the appropriate regular frames    
    	for (cnt = DEBUGGER_FRAME1 ; cnt <= myToArea ; cnt++)
    	{
    	    ShowWindow (myInfo -> raisedArea [cnt], SW_SHOW);
    	}    	
    	for (cnt = myToArea + 1 ; cnt <= DEBUGGER_FRAME3 ; cnt++)
    	{
   	    ShowWindow (myInfo -> raisedArea [cnt], SW_HIDE);
    	}
    	
    	// For the moment, disable the Show Vars check box
    	if (myToButton >= SHOW_VARS_CHECKBOX)
    	{
    	    EnableWindow (myInfo -> control [SHOW_VARS_CHECKBOX], FALSE);
    	}
    }
    else
    {
    	for (cnt = STEP_BUTTON ; cnt <= TRACKER ; cnt++)
    	{
    	    ShowWindow (myInfo -> control [cnt], SW_HIDE);
    	}
	ShowWindow (myInfo -> raisedArea [DEBUGGER_FRAME1], SW_HIDE);
	ShowWindow (myInfo -> raisedArea [DEBUGGER_FRAME2], SW_HIDE);
	ShowWindow (myInfo -> raisedArea [DEBUGGER_FRAME3], SW_HIDE);
    }
    
    RedrawWindow (pmButtonBarWindow, NULL, NULL, RDW_INVALIDATE);
} // MyResizeButtonBar


