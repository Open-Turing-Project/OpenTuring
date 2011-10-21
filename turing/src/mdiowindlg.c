/*****************/
/* MDIOWinDlg.c */
/*****************/

/****************/
/* Self include */
/****************/
#include "mdiowindlg.h"

/******************/
/* Other includes */
/******************/
#include "mio.h"

#include "miodialogs.h"

#include "miowin.h"
#include "mdiowin.h"
#include "miowindlg.h"

#include "levent.h"

#include "edint.h"

#include "miotypes.h"

// Test to make certain we're not accidentally including <windows.h> which
// might allow for windows contamination of platform independent code.
#ifdef _INC_WINDOWS
xxx
#endif

#include "edtext.h"

/*******************/
/* System includes */
/*******************/
#include <windows.h>
#include <windowsx.h>
#include <crtdbg.h>

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
// The number of text display windows allowed in single window mode
#define MAX_WINDOWS		30

// Used for scrolling with the mouse
#define SCROLL_TIMER		1
#define HORIZ_TICKS_PER_SCROLL	4

// Used for the 1 second delay before halting when a run-time error occurs
#define HALT_TIMER		2

// Used in MDIOWinDlg_DisplayText
#define NO_SEL_ON_THIS_LINE	    -1
#define TO_END_OF_LINE	 	999999
#define IN_SELECTION		     1
#define IN_HIGHLIGHT		     2
#define IN_NORMAL		     3

// Some colours                        
#define COLOUR_WHITE		RGB (255, 255, 255)
#define COLOUR_BLACK		RGB (0, 0, 0)
#define COLOUR_DEEPBLUE		RGB (0, 0, 132)

// The colour the selection will appear in
#define SELECTION_COLOUR	COLOUR_DEEPBLUE

// Used for by the mouse EdInt_GetMouseWheelSettings
#ifndef WM_MOUSEWHEEL
// Message ID for IntelliMouse wheel
#define WM_MOUSEWHEEL WM_MOUSELAST+1 
#endif // !WM_MOUSEWHEEL

#define NEW_LINE	"\r\n"

/********************/
/* Global variables */
/********************/

/*********/
/* Types */
/*********/

/**********************/
/* External variables */
/**********************/

/********************/
/* Static constants */
/********************/

/********************/
/* Static variables */
/********************/
//
static HBRUSH	stBlackBrush;

// Used by the wheel mouse
static UINT	stMouseWheelMessage;

// The contextual menu
static HMENU	stContextualMenu;


/******************************/
/* Static callback procedures */
/******************************/
static LRESULT CALLBACK MyTextInnerWindowProcedure (HWND pmInnerWindow, 
						    UINT pmMessage, 
    						    WPARAM pmWParam, 
    						    LPARAM pmLParam);
static LRESULT CALLBACK MyDlgWindowProcedure (HWND pmWindow, UINT pmMessage, 
					      WPARAM pmWParam, 
					      LPARAM pmLParam);

/*********************/
/* Static procedures */
/*********************/
static int		MyInitializeTextInnerWindow (HWND pmInnerWindow);
static int		MyInitializeTextWindow (HWND pmWindow, 
				    WindowAttribPtr pmWindowAttrib);
static HWND		MyGetInnerWindow (HWND pmWindow);
static BOOL		MySaveTextToFile (char *pmPathName, TextPtr pmText);

/***********************/
/* External procedures */
/***********************/
/************************************************************************/
/* MDIOWinDlg_Init							*/
/* 									*/
/* Initialize the modules under EdWin and register the new class	*/
/************************************************************************/
void	MDIOWinDlg_Init ()
{
    char	myRunWindowClassName [256];
    char	myRunTextWindowClassName [256];
    int		myResult;
    WNDCLASSEX	myWindowClass;

    //
    // Initialize the MDIOText module
    //

    // Create Brush
    stBlackBrush = CreateSolidBrush (COLOUR_BLACK);
    if (stBlackBrush == NULL)	  
    {
    	EdInt_FailWarn (IDS_MIO_CREATEBRUSHFAIL, __FILE__, __LINE__, 
    			GetLastError ());
    }
    
    // Initialize the wheel mouse
    stMouseWheelMessage = EdInt_GetMouseWheelMessage ();
    
    // Get the class name
    EdInt_LoadString (IDS_MIOTEXT_WINDOW_NAME, myRunWindowClassName, 
	sizeof (myRunWindowClassName));

    //
    // Register the console window class
    //
    myWindowClass.cbSize = 	  sizeof (myWindowClass);
    // Set window class to redraw when window size changes
    myWindowClass.style = 	  CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    // Procedure to be called with messages for this window class
    myWindowClass.lpfnWndProc =   MyDlgWindowProcedure;
    // The extra space in class struct
    myWindowClass.cbClsExtra = 	  0;
    // The extra space in window struct for the pointer to text data
    // and editor window
    myWindowClass.cbWndExtra = 	  WINDOW_EXTRA_MEMORY;
    // The application's handle
    myWindowClass.hInstance = 	  MIO_applicationInstance;
    // Set the icon for this window class
    myWindowClass.hIcon = 	  LoadIcon (MIO_applicationInstance, 
    					  MAKEINTRESOURCE (MIO_RUN_ICON));
    // Set the cursor for this window class
    myWindowClass.hCursor = 	  LoadCursor (NULL, IDC_ARROW);
    // Set the background colour for this window
    myWindowClass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH);
    // Set the menu for this window class
    myWindowClass.lpszMenuName =  NULL;
    // Name of the window class
    myWindowClass.lpszClassName = myRunWindowClassName; 
    // Set the icon for this class.
    myWindowClass.hIconSm = 	  LoadIcon (MIO_applicationInstance, 
    					  MAKEINTRESOURCE (MIO_RUN_SMALL_ICON));
    
    myResult = RegisterClassEx (&myWindowClass);
    if (myResult == 0)
    {
    	EdInt_FailWarn (IDS_MIO_REGISTERCLASSFAIL, __FILE__, __LINE__, 
    			GetLastError ());
    }
    
    // Get the class name
    EdInt_LoadString (IDS_MIOTEXT_INNER_WINDOW_NAME, myRunTextWindowClassName, 
	sizeof (myRunTextWindowClassName));
    
    //
    // Register the text display window class
    //
    myWindowClass.cbSize = 	  sizeof (myWindowClass);
    // Set window class to redraw when window size changes
    myWindowClass.style = 	  CS_HREDRAW | CS_VREDRAW;
    // Procedure to be called with messages for this window class
    myWindowClass.lpfnWndProc =   MyTextInnerWindowProcedure;
    // The extra space in class struct
    myWindowClass.cbClsExtra = 	  0;
    // The extra space in window struct for the pointer to text data
    // and editor window
    myWindowClass.cbWndExtra = 	  WINDOW_EXTRA_MEMORY;
    // The application's handle
    myWindowClass.hInstance = 	  MIO_applicationInstance;
    // Set the icon for this window class
    myWindowClass.hIcon = 	  NULL;
    // Set the cursor for this window class
    myWindowClass.hCursor = 	  LoadCursor (NULL, IDC_IBEAM);
    // Set the background colour for this window
    myWindowClass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH);
    // Set the menu for this window class
    myWindowClass.lpszMenuName =  NULL;
    // Name of the window class
    myWindowClass.lpszClassName = myRunTextWindowClassName; 
    // Set the icon for this class.
    myWindowClass.hIconSm = 	  NULL;
    
    myResult = RegisterClassEx (&myWindowClass);
    if (myResult == 0)
    {
    	EdInt_FailWarn (IDS_MIO_REGISTERCLASSFAIL, __FILE__, __LINE__, 
    		        GetLastError ());
    }

    // Load the contextual menu
    stContextualMenu = LoadMenu (MIO_applicationInstance, 
		MAKEINTRESOURCE (MIO_TEXT_CONTEXTUAL_MENU));
    stContextualMenu = GetSubMenu (stContextualMenu, 0);
} // MDIOWinDlg_Init


/************************************************************************/
/* MDIOWinDlg_Finalize							*/
/************************************************************************/
void	MDIOWinDlg_Finalize (void)
{
} // MDIOWinDlg_Finalize


/************************************************************************/
/* MDIOWinDlg_CaptureMouse						*/
/************************************************************************/
void	MDIOWinDlg_CaptureMouse (WIND pmWindow)
{
    SetCapture ((HWND) pmWindow);
} // MDIOWinDlg_CaptureMouse


/******************************************************************************/
/* MDIOWinDlg_CopyToClipboard						      */
/******************************************************************************/
void	MDIOWinDlg_CopyToClipboard (WIND pmWindow, int pmLen, char *pmSrc, 
				     char *pmEnd)
{
    HGLOBAL	myGlobalHandle;
    char	*myGlobalPtr, *myDest;

    // Allocate the global memory and lock it down into a pointer
    myGlobalHandle = GlobalAlloc (GHND | GMEM_SHARE, pmLen);
    if (myGlobalHandle == NULL)
    {
    	EdInt_Message1 (pmWindow, 0, IDS_MIO_COPY_FAILED_TITLE,
    	    IDS_MIO_COPY_FAILED_MESSAGE);
    	return;
    }
    myGlobalPtr = GlobalLock (myGlobalHandle);
    
    // Copy all the selection, turning LF to CR LF
    myDest = myGlobalPtr;
    while (pmSrc < pmEnd)
    {
    	if (*pmSrc == '\n')
    	{
    	    *myDest++ = '\r';
    	}
    	*myDest++ = *pmSrc++;
    } // while

    // Unlock the handle
    GlobalUnlock (myGlobalHandle);

    //
    // Copy the data into the clipboard    
    //
    OpenClipboard ((HWND) pmWindow);
    EmptyClipboard ();
    SetClipboardData (CF_TEXT, myGlobalHandle);
    CloseClipboard ();
} // MDIOWinDlg_CopyToClipboard


/******************************************************************************/
/* MDIOWinDlg_CreateInnerWindow					      */
/******************************************************************************/
WIND	MDIOWinDlg_CreateInnerWindow (WIND pmWindow)
{
    HINSTANCE		myApplicationInstance;
    char		myClassName [256];
    HWND		myRunTextWindow;
    
    // Get the program instance
    myApplicationInstance = 
        (HINSTANCE) GetWindowLong (pmWindow, GWL_HINSTANCE);
        
    // Create the console text window
    EdInt_LoadString (IDS_MIOTEXT_INNER_WINDOW_NAME, myClassName,
    	sizeof (myClassName));

    myRunTextWindow = CreateWindow (
    	myClassName, 			// Window class
        NULL,	 			// Window title
        WS_CHILDWINDOW | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL, // Window style
    	0, 0, 0, 0, 			// Initial location & size
    	pmWindow, 			// Parent window handle
    	NULL, 				// Window menu handle
    	myApplicationInstance,	 	// Program instance handle
    	NULL);				// Creation parameters
    
    if (MIOWin_GetProperties () -> logWindowMessages)
    {
//    	EdLog_Log ("Text Display Window = %x  (Finished Creation)", 
//    	    myRunTextWindow);
    }

    return myRunTextWindow;
} // MDIOWinDlg_CreateInnerWindow


/************************************************************************/
/* MDIOWinDlg_CreateWindow						*/
/************************************************************************/
WIND	MDIOWinDlg_CreateWindow (WindowAttribPtr pmWindowAttribs)
{
    char			myRunWindowClassName [256];
    HWND			myRunWindow;
        
    // Load the class name (we've already loaded it once, so no error checking)    
    EdInt_LoadString (IDS_MIOTEXT_WINDOW_NAME,
	myRunWindowClassName, sizeof (myRunWindowClassName));

    // Create the editor window    
    myRunWindow = CreateWindow (
    	myRunWindowClassName, 		// Window class
        "Run Window", 				// Window title
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, 	// Window style
    	CW_USEDEFAULT, 				// Initial x location
    	CW_USEDEFAULT, 				// Initial y location
    	CW_USEDEFAULT, 				// Initial x size
    	CW_USEDEFAULT, 				// Initial y size
    	HWND_DESKTOP, 				// Parent window handle
    	NULL, 					// Window menu handle
    	MIO_applicationInstance,		// Program instance handle
    	pmWindowAttribs);			// Creation parameters
    
    if (MIOWin_GetProperties () -> logWindowMessages)
    {
    	// EdLog_Log ("Run Window = %x  (Finished Creation)", myRunWindow);
    }

    // If we failed to create the window, return immediately
    if (myRunWindow == NULL)
    {
	EdInt_HaltEnvironment (IDS_MIO_CREATEWINDOWFAIL, __FILE__, __LINE__, 
		               GetLastError ());
    }
    
    
    MIOWin_SetRunWindowTitle (myRunWindow);

    return myRunWindow;
} // MDIOWinDlg_CreateWindow

    			      
/************************************************************************/
/* MDIOWinDlg_InitializeWindow						*/
/************************************************************************/
BOOL	MDIOWinDlg_InitializeWindow (WIND pmWindow)
{
    MIOWinInfoPtr	myInfo = MIOWin_GetInfo (pmWindow);
    
    // Add the edit text
    myInfo -> text = EdText_Create ();
    if (myInfo -> text == NULL)
        return FALSE;

    EdText_InitializeTextRecord (myInfo -> text);  
    
    return TRUE;
} // MDIOWinDlg_InitializeWindow


/******************************/
/* Static callback procedures */
/******************************/
/************************************************************************/
/* MyTextInnerWindowProcedure						*/
/*									*/
/* Callback procedure for the main editor window.			*/
/************************************************************************/
static LRESULT CALLBACK MyTextInnerWindowProcedure (HWND pmInnerWindow, 
						    UINT pmMessage, 
    						    WPARAM pmWParam, 
    						    LPARAM pmLParam)
{
    MIOWinInfoPtr	myInfo;
    UCHAR		mySpecialChar;
    
    // Log the windows messages
    if (MIOWin_GetProperties () -> logWindowMessages)
    {
//	EdLog_PrintWindowsMessage ("RunText", pmInnerWindow, 
//	    pmMessage, pmWParam, pmLParam);
    }
    
    switch (pmMessage)
    {    
    	case WM_CREATE:
	    if (MIOWin_GetProperties () -> logWindowMessages)
	    {
//	    	EdLog_Log ("Run Text Window = %x  (Starting Creation)", 
//	    	    pmInnerWindow);
	    }

	    return MyInitializeTextInnerWindow (pmInnerWindow);

        case WM_PAINT:
//            MDIOWinDlg_DisplayText (pmInnerWindow, TRUE, ALL_LINES, ALL_LINES);
            return 0;
        
	case WM_SIZE:
	    MDIOWin_ResizeInnerWindow (pmInnerWindow);
	    return 0;
	    
	//
	// Keystroke handling
	//
	case WM_CHAR:
	    // If not paused, pass it to the Turing keystroke handler
	    if (!MIO_paused && !MIO_finished)
	    {
		myInfo = MIOWin_GetInfo (pmInnerWindow);
		// Convert ENTER key code from 13 to 10
		if (pmWParam == 13)
		{
		    pmWParam = 10;
		}
		MIOWin_KeystrokeHandler (myInfo -> outerWindow, (UCHAR) pmWParam);
		return 0;
	    }
	    break;

	case WM_SYSCHAR:
	    if (!MIO_paused && !MIO_finished)
	    {
		// This translates all the ALT+0-9,-,=,A-Z keys
		mySpecialChar = MDIOWin_TranslateAltKeystroke (pmWParam);
		if (mySpecialChar != 0)
		{
		    myInfo = MIOWin_GetInfo (pmInnerWindow);
	    
		    MIOWin_KeystrokeHandler (myInfo -> outerWindow, mySpecialChar);

		    return 0;
		}
	    }
	    else
	    {
	        if (pmWParam == 'x')
		{
    		    EdInt_ExitImmediate ();
    		    return 0;
    		}
	    }
	    break;

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	    if (!MIO_paused && !MIO_finished)
	    {
	    	// Check for Control+C/Break
	    	if (((GetKeyState (VK_CONTROL) & 0x8000) != 0) &&
	    	    (pmWParam == 'c') || (pmWParam == VK_CANCEL))
	    	{
	    	    EdInt_PauseExecution ();
		}

		// Function and arrow keys won't be passed on to WM_CHAR, so
		// they must be processed here.  Normal keys will be passed to
		// WM_CHAR, so they must *not* be processed here.
		mySpecialChar = MDIOWin_TranslateSpecialKeystroke (pmWParam);
		if (mySpecialChar != 0)
		{
		    myInfo = MIOWin_GetInfo (pmInnerWindow);
	    
		    MIOWin_KeystrokeHandler (
		    	MIOWin_GetInfo (pmInnerWindow) -> outerWindow, 
		    	mySpecialChar);

		    return 0;
		}
	    }
	    // If paused or finished, check if it's a menu shortcut
	    else if (MDIOWin_HandleControlKey (
	    		 MIOWin_GetInfo (pmInnerWindow) -> outerWindow, 
	    		 pmWParam))
	    {
	    	return 0;
	    }
	    break;

	//
	// Contextual Menu Handling
	//
        case WM_CONTEXTMENU:
	    if (MIO_paused || MIO_finished)
	    {
		int myX = GET_X_LPARAM (pmLParam);
		int myY = GET_Y_LPARAM (pmLParam);
		MDIOWin_AdjustContextualMenu (stContextualMenu);
		TrackPopupMenu (stContextualMenu, 0, myX, myY, 
			    0, MIOWin_GetInfo (pmInnerWindow) -> outerWindow, NULL);

		return 0;
	    }
	    break;
    }
    
    return DefWindowProc (pmInnerWindow, pmMessage, pmWParam, 
        		  pmLParam);
} // MyTextInnerWindowProcedure


/************************************************************************/
/* MyDlgWindowProcedure							*/
/*									*/
/* Callback procedure for the main editor window.			*/
/************************************************************************/
static LRESULT CALLBACK MyDlgWindowProcedure (HWND pmWindow, UINT pmMessage, 
    					       WPARAM pmWParam, LPARAM pmLParam)
{
    // Last parameter is pmIsGraphicsWindow = FALSE
    return MDIOWin_WindowProcedure (pmWindow, pmMessage, pmWParam, 
				    pmLParam, FALSE);
} // MyDlgWindowProcedure


/*********************/
/* Static procedures */
/*********************/
/************************************************************************/
/* MyInitializeTextInnerWindow						*/
/************************************************************************/
static int	MyInitializeTextInnerWindow (HWND pmInnerWindow)
{
    // Set the window type            
    SetWindowLong (pmInnerWindow, WINDOW_MEM_WINDOW_TYPE, 
        (long) WINDOW_TYPE_MIO_TEXT_INNER);

    // Set the window initialized                
    SetWindowLong (pmInnerWindow, WINDOW_MEM_WINDOW_INITIALIZED, 
        (long) TRUE);
            
    return 0;
} // MyInitializeTextInnerWindow

				    
/************************************************************************/
/* MyGetInnerWindow							*/
/************************************************************************/
static HWND	MyGetInnerWindow (HWND pmWindow)
{
    MIOWinInfoPtr	myInfo;
    
    // Get the window information
    myInfo = MIOWin_GetInfo (pmWindow);
    
    return myInfo -> innerWindow;
} // MyGetInnerWindow


