/************************************************************************/
/* edstat.c								*/
/*									*/
/* EdStat uses the following routines in other modules:			*/
/*	EdFail - various						*/
/*	EdGUI - EdGUI_CreateFont					*/
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
#include "edstat.h"

/******************/
/* Other includes */
/******************/
#include "edfail.h"
#include "edglob.h"
#include "edgui.h"
#include "edprog.h"

#include "eddialogs.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
// Windows class name for the edit box
#define EDITBOX_CLASS_NAME		"edit"

// Arguments to MySetStatus
#define STATUS_BOLD			TRUE
#define STATUS_NOT_BOLD			FALSE

// Size of font in status bar
#define STATUS_BAR_FONT_SIZE		 8

// Spacing constants for status bar
// Size of margin from top of window to control (in pixels)
#define TOP_MARGIN			 2
#define STATUS_SIZE_IN_CHARS		40
#define UP_DOWN_LEFT_MARGIN		 3
#define UP_DOWN_WIDTH			20
#define UP_DOWN_RIGHT_MARGIN		20
#define LARGE_POSITION_SIZE_IN_CHARS	30
#define SMALL_POSITION_SIZE_IN_CHARS	23

// Timer ID
#define STATUS_TIMER			  2
#define SCROLL_DELAY_IN_MILLISEC	333

// IDs for the window components
#define ID_STATUS_BOX			10001
#define ID_UPDOWN			10002
#define ID_POSITION_BOX			10003

//
#define STATUS_HEIGHT_MULTIPLIER_SMALL	1.5
#define STATUS_HEIGHT_MULTIPLIER_MID	1.35
#define STATUS_HEIGHT_MULTIPLIER_LARGE	1.25

// Time in milliseconds to indicate practically forever
#define FOREVER				32767

/********************/
/* Global variables */
/********************/

/*********/
/* Types */
/*********/
typedef struct
{
    // The elements in the window
    HWND	statusBox;
    HWND	upDown;
    HWND	positionBox;
    HWND	toolTip;

    // The message currently displayed in the status window
    char	*statusMessage;
    int		statusMessageSize;

    // The line in the status box currently being displayed.
    int 	currentStatusLine;

    // Whether the text is displayed in bold
    BOOL	isBold;
    
    // Display the position info in short form
    BOOL	displayShortPositionInfo;

    // Whether to allow status bar scrolling
    BOOL	allowScrolling;

    // What was the last position info and how was it displayed
    int		lastLine;
    int 	lastCol;
    int		lastNumLines;
    BOOL	lastShort;
} StatusWindowInfo, *StatusWindowInfoPtr;

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
static EdStat_Properties	stProperties;
static BOOL			stPropertiesChanged;

// Colours for drawing background, etc.
static COLORREF	stBackgroundColour;
static HBRUSH	stBackgroundBrush;

// Used for the status box
static HFONT	stStatusFont, stBoldStatusFont;
static int	stStatusFontCharWidth, stStatusFontCharHeight;

// The display string for the position box
static char	stShortPositionString [80], stLongPositionString [80];

// The atom associated with the status window
static DWORD	stStatusWindowAtom;

// Hook for Tool tip relayer
static HHOOK	stToolTipRelayerHook = NULL;

/******************************/
/* Static callback procedures */
/******************************/
static LRESULT CALLBACK MyStatusWindowProcedure (HWND pmStatusWindow, 
					         UINT pmMessage, 
					         WPARAM pmWParam, 
					         LPARAM pmLParam);
static LRESULT CALLBACK MyToolTipRelayer (int pmNCode, WPARAM pmWParam, 
					  LPARAM pmLParam);

/*********************/
/* Static procedures */
/*********************/
static int			MyCreateStatusWindow (HWND pmStatusWindow);
static StatusWindowInfoPtr	MyCreateStatusWindowInfo (void);
static void			MyDispose (HWND pmStatusWindow);
static StatusWindowInfoPtr	MyGetStatusWindowInfo (HWND pmStatusWindow);
static void			MyResizeStatusWindow (HWND pmStatusWindow);
static BOOL			MySetDialogFonts (HWND pmStatusWindow);
static void			MySetStatus (HWND pmStatusWindow, 
					     const char *pmMessage, 
					     BOOL pmBold);
static void			MySetStatusUpDown (HWND pmStatusWindow);

/***********************/
/* External procedures */
/***********************/
/*************************************************************************/
/* EdStat_Init								 */
/*************************************************************************/
BOOL	EdStat_Init (void)
{
    char	myStatusWindowClassName [256];
    WNDCLASSEX	myWindowClass;
        
    // Get the background colour (can't fail unless index is invalid)
    stBackgroundColour = GetSysColor (COLOR_BTNFACE);
    
    // Get the background brush
    stBackgroundBrush = CreateSolidBrush (stBackgroundColour);
    if (stBackgroundBrush == NULL)	  
    {
    	EdFail_Warn (IDS_CREATEBRUSHFAIL, __FILE__,  __LINE__, GetLastError ());
    	return FALSE;
    }
    
    // Get the class name
    EdGUI_LoadString (IDS_STATUS_WINDOW_NAME, myStatusWindowClassName, 
	sizeof (myStatusWindowClassName));
		     
    // Get the short position string
    EdGUI_LoadString (IDS_SHORT_POSITION_STRING, stShortPositionString, 
        sizeof (stShortPositionString));

    // Get the long position string
    EdGUI_LoadString (IDS_LONG_POSITION_STRING, stLongPositionString, 
        sizeof (stLongPositionString));
		     
    //
    // Register the status window class
    //
    myWindowClass.cbSize = 	  sizeof (myWindowClass);
    // Set window class to redraw when window size changes
    myWindowClass.style = 	  CS_HREDRAW | CS_VREDRAW;
    // Procedure to be called with messages for this window class
    myWindowClass.lpfnWndProc =   MyStatusWindowProcedure;
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
    myWindowClass.lpszClassName = myStatusWindowClassName; 
    // Set the icon for this class.
    myWindowClass.hIconSm = 	  NULL;
    
    stStatusWindowAtom = RegisterClassEx (&myWindowClass);
    if (stStatusWindowAtom == 0)
    {
    	EdFail_Warn (IDS_REGISTERCLASSFAIL, __FILE__, __LINE__, 
    		     GetLastError ());
    	return FALSE;
    }
    
    return TRUE;
} // EdStat_Init


/*************************************************************************/
/* EdStat_Finalize							 */
/*************************************************************************/
void	EdStat_Finalize (void)
{
} // EdStat_Finalize


/************************************************************************/
/* EdStat_PropertiesSet							*/
/************************************************************************/
void	EdStat_PropertiesSet (EdStat_Properties pmProperties)
{
    stProperties = pmProperties;
    stPropertiesChanged = TRUE;
} // EdStat_PropertiesSet

	
/************************************************************************/
/* EdStat_PropertiesImplementChanges					*/
/************************************************************************/
void	EdStat_PropertiesImplementChanges (HWND pmStatusWindow)
{
    StatusWindowInfoPtr	myInfo;

    if (stPropertiesChanged)
    {
    	MySetDialogFonts (pmStatusWindow);
    	stPropertiesChanged = FALSE;
    }

    // Set the font in the status box and position box
    myInfo = MyGetStatusWindowInfo (pmStatusWindow);
    SendMessage (myInfo -> positionBox, WM_SETFONT, (WPARAM) stStatusFont, 0);
    if (myInfo -> isBold)
    {
    	SendMessage (myInfo -> statusBox, WM_SETFONT, 
    	    (WPARAM) stBoldStatusFont, 0);
    }
    else
    {
    	SendMessage (myInfo -> statusBox, WM_SETFONT, 
    	    (WPARAM) stStatusFont, 0);
    }
} // EdStat_PropertiesImplementChanges


/*************************************************************************/
/* EdStat_Create							 */
/*************************************************************************/
HWND	EdStat_Create (HWND pmEditWindow)
{
    HINSTANCE	myApplicationInstance;
    char	myStatusWindowClassName [256];
    HWND	myStatusWindow;
    
    // Get the program instance
    myApplicationInstance = 
        (HINSTANCE) GetWindowLong (pmEditWindow, GWL_HINSTANCE);
        
    // Load the class name (we've already loaded it once, so no error checking)    
    EdGUI_LoadString (IDS_STATUS_WINDOW_NAME, myStatusWindowClassName, 
        sizeof (myStatusWindowClassName));

    // Create the editor window
    myStatusWindow = CreateWindow (
    	myStatusWindowClassName, 		// Window class
        NULL,	 				// Window title
        WS_CHILDWINDOW | WS_VISIBLE,	 	// Window style
    	0, 0, 0, 0, 				// Initial location & size
    	pmEditWindow, 				// Parent window handle
    	NULL, 					// Window menu handle
    	myApplicationInstance,	 		// Program instance handle
    	NULL);					// Creation parameters

    return myStatusWindow;
} // EdStat_Create


/************************************************************************/
/* EdStat_GetWindowHeight						*/
/************************************************************************/
int	EdStat_GetWindowHeight (void)
{
    double  myMultiplier;

    if (STATUS_BAR_FONT_SIZE < 12)
	myMultiplier = STATUS_HEIGHT_MULTIPLIER_SMALL;
    else if (STATUS_BAR_FONT_SIZE < 18)
	myMultiplier = STATUS_HEIGHT_MULTIPLIER_MID;
    else
	myMultiplier = STATUS_HEIGHT_MULTIPLIER_LARGE;

    return (int) (stStatusFontCharHeight * myMultiplier) + TOP_MARGIN;
} // EdStat_GetWindowHeight


/************************************************************************/
/* EdStat_Set								*/
/*									*/
/* Display a string in the information panel at the bottom of the	*/
/* window.								*/
/************************************************************************/
void	EdStat_Set (HWND pmStatusWindow, const char *pmMessage, 
		    va_list pmArgList)
{
    if (pmArgList == NULL)
    {
    	MySetStatus (pmStatusWindow, pmMessage, STATUS_NOT_BOLD);
    }
    else
    {
    	char 	myFullMessage [4096];
    
    	wvsprintf (myFullMessage, pmMessage, pmArgList);
    	va_end (pmArgList);
    
    	MySetStatus (pmStatusWindow, myFullMessage, STATUS_NOT_BOLD);
    }
} // EdStat_Set


/*************************************************************************/
/* EdStat_SetBold							 */
/*									 */
/* Display a string in the information panel at the bottom of the window */
/*************************************************************************/
void	EdStat_SetBold (HWND pmStatusWindow, const char *pmMessage, 
			va_list pmArgList)
{
    if (pmArgList == NULL)
    {
    	MySetStatus (pmStatusWindow, pmMessage, STATUS_BOLD);
    }
    else
    {
    	char 	myFullMessage [4096];
    
    	wvsprintf (myFullMessage, pmMessage, pmArgList);
    	va_end (pmArgList);
    
    	MySetStatus (pmStatusWindow, myFullMessage, STATUS_BOLD);
    }
} // EdStat_SetBold


/************************************************************************/
/* EdStat_SetPositionInfo						*/
/*									*/
/* Set the cursor position panel at the bottom of the window.		*/
/************************************************************************/
void	EdStat_SetPositionInfo (HWND pmStatusWindow, int pmLine, int pmCol,
				int pmNumLines)
{
    StatusWindowInfoPtr	myInfo;
    char		myStr [80];

    if (pmStatusWindow == NULL)
    {
    	return;
    }
    myInfo = MyGetStatusWindowInfo (pmStatusWindow);
    
    if ((pmLine == myInfo -> lastLine) && (pmCol == myInfo -> lastCol) && 
        (pmNumLines == myInfo -> lastNumLines) && 
        (myInfo -> displayShortPositionInfo == myInfo -> lastShort))
    {
    	return;
    }
    
    myInfo -> lastLine = pmLine;
    myInfo -> lastCol = pmCol;
    myInfo -> lastNumLines = pmCol;
    myInfo -> lastShort = myInfo -> displayShortPositionInfo;
    
    if (myInfo -> displayShortPositionInfo)
    {
    	wsprintf (myStr, stShortPositionString, pmLine + 1, pmNumLines, 
    	    pmCol + 1);
    }
    else
    {
    	wsprintf (myStr, stLongPositionString, pmLine + 1, pmNumLines, 
    	    pmCol + 1);
    }
    
    SendMessage (myInfo -> positionBox, SB_SETTEXT, 0, (LPARAM) myStr);
} // EdStat_SetPositionInfo


/******************************/
/* Static callback procedures */
/******************************/
/************************************************************************/
/* MyStatusWindowProcedure						*/
/*						 		        */
/* Callback procedure for the status window.				*/
/************************************************************************/
static LRESULT CALLBACK MyStatusWindowProcedure (HWND pmStatusWindow, 
					         UINT pmMessage, 
					         WPARAM pmWParam, 
					         LPARAM pmLParam)
{
    HDC			myDeviceContext;
    PAINTSTRUCT		myPaintStruct;
    RECT		myWindowRect;
    MSG			myMessage;

    switch (pmMessage)
    {
    	case WM_CREATE:
	    return MyCreateStatusWindow (pmStatusWindow);

	case WM_DESTROY:
	    MyDispose (pmStatusWindow);
	    break;
	    	    
	case WM_SIZE:
	    MyResizeStatusWindow (pmStatusWindow);
	    return 0;

	case WM_SETCURSOR:
	    return 1;
	    
    	case WM_PAINT:
    	    if (!stProperties.oneWindowMode)
    	    {
    	    	myDeviceContext = BeginPaint (pmStatusWindow, &myPaintStruct);
    	    	GetClientRect (pmStatusWindow, &myWindowRect);
    	    	MoveToEx (myDeviceContext, 0, 0, NULL);
    	    	LineTo (myDeviceContext, myWindowRect.right, 0);
    	    	EndPaint (pmStatusWindow, &myPaintStruct);
     	    	return 0;
   	    }
	    break;
    	
        case WM_COMMAND:
            if (LOWORD (pmWParam) == ID_STATUS_BOX)
            {
                SetFocus (GetParent (pmStatusWindow));
	        return 0;
            }
            break;

	case WM_SYSCOLORCHANGE:
	    //
	    // Set the new background colours
	    //
	    
	    // Set new colours
	    stBackgroundColour = GetSysColor (COLOR_BTNFACE);
	    
	    // Delete the old background brush
	    if (stBackgroundBrush != NULL)
	    {
	    	DeleteObject (stBackgroundBrush);
	    }
	    
	    // Get the background brush
	    stBackgroundBrush = CreateSolidBrush (stBackgroundColour);
	    
	    // Set the window to be redrawn
	    InvalidateRect (pmStatusWindow, NULL, TRUE);
	    return 0;
	    
    	case WM_CTLCOLOREDIT:
    	case WM_CTLCOLORSTATIC:
    	    // We handle these messages in order to obtain a grey brackground
    	    // for the status box (which is an edit window)
     	    SetBkColor ((HDC) pmWParam, stBackgroundColour);
   	    return (DWORD) stBackgroundBrush;
    	    
	case WM_VSCROLL:
	    // We handle these messages for the up-down control that appears
	    // when the message is longre than fits in the status box
	    if (LOWORD (pmWParam) == SB_THUMBPOSITION)
	    {
	    	StatusWindowInfoPtr	myInfo;
	    	int			myNewLine;
	    	
	    	myInfo = MyGetStatusWindowInfo (pmStatusWindow);
	    	myNewLine = HIWORD (pmWParam);
	    	
		// TW
//	    	while (myInfo -> currentStatusLine != myNewLine)
//	    	{
		    if (myNewLine > myInfo -> currentStatusLine)
		    {	    	    
			SendMessage (myInfo -> statusBox, EM_LINESCROLL, 0, -1);
			myInfo -> currentStatusLine++;
		    }
		    else if (myNewLine < myInfo -> currentStatusLine)
		    {
			SendMessage (myInfo -> statusBox, EM_LINESCROLL, 0, 1);
			myInfo -> currentStatusLine--;
		    }
//		} // while
	    } // if
	    return 0;

	case WM_TIMER:
	    if (pmWParam == STATUS_TIMER)
	    {
	    	KillTimer (pmStatusWindow, STATUS_TIMER);
		MyGetStatusWindowInfo (pmStatusWindow) -> allowScrolling = TRUE;
	    }
	    return TRUE;
	    
	// Pass messages on to the tooltip control
        case WM_MOUSEMOVE: 
        case WM_LBUTTONDOWN: 
        case WM_LBUTTONUP: 
        case WM_RBUTTONDOWN: 
        case WM_RBUTTONUP: 
            myMessage.lParam = pmLParam; 
            myMessage.wParam = pmWParam;
            myMessage.message = pmMessage; 
            myMessage.hwnd = pmStatusWindow;
            SendMessage (MyGetStatusWindowInfo (pmStatusWindow) -> toolTip, 
		TTM_RELAYEVENT, 0, (LPARAM) (LPMSG) &myMessage); 
	    break;

	// Handle notification from the tooltip control requesting the
	// the message to display
	case WM_NOTIFY:
	    if ((((LPNMHDR) pmLParam)->code) == TTN_NEEDTEXT) 
	    {
		// Currently we don't need the control ID, but it's here for
		// future reference
		// MyControlID = 
		//	    GetDlgCtrlID ((HWND) ((LPNMHDR) pmLParam)->idFrom); 
		((LPTOOLTIPTEXT) pmLParam) -> lpszText = 
		    MyGetStatusWindowInfo (pmStatusWindow) -> statusMessage;
		return 0;
	    }
	    else if ((((LPNMHDR) pmLParam)->code) == UDN_DELTAPOS) 
	    {
		StatusWindowInfoPtr myInfo;
		BOOL		    myAllowScrolling;
		NM_UPDOWN	    *myUpDown;

		myInfo = MyGetStatusWindowInfo (pmStatusWindow);
		myAllowScrolling = myInfo -> allowScrolling;
		myUpDown = (LPNM_UPDOWN) pmLParam;

		if (myAllowScrolling)
		{
		    myInfo -> allowScrolling = FALSE;
		    SetTimer (pmStatusWindow, STATUS_TIMER, 
			SCROLL_DELAY_IN_MILLISEC, NULL);
		}
    
		return !myAllowScrolling;
	    }
	    break;
    } // switch

    return DefWindowProc (pmStatusWindow, pmMessage, pmWParam, pmLParam);
} // MyStatusWindowProcedure


/*********************/
/* Static procedures */
/*********************/
/************************************************************************/
/* MyCreateStatusWindow							*/
/************************************************************************/
static int	MyCreateStatusWindow (HWND pmStatusWindow)
{
    HINSTANCE		myApplicationInstance;
    TOOLINFO		myToolInfo;
    StatusWindowInfoPtr	myInfo;
    //
    // Initialize the extra window memory
    //
	
    // Set the window type            
    SetWindowLong (pmStatusWindow, WINDOW_MEM_WINDOW_TYPE, 
        (long) WINDOW_TYPE_STATUS);

    // Set the window initialized                
    SetWindowLong (pmStatusWindow, WINDOW_MEM_WINDOW_INITIALIZED, (long) FALSE);
            
    // Create the status window information
    myInfo = MyCreateStatusWindowInfo ();
    if (myInfo == NULL)
    {
    	return -1;
    }
    SetWindowLong (pmStatusWindow, WINDOW_MEM_WINDOW_INFO, (long) myInfo);
    	    
    //
    // Add the elements to the window
    //
    myApplicationInstance = (HINSTANCE) GetWindowLong (pmStatusWindow, 
	GWL_HINSTANCE);

    // Add the status box (an edit window)
    myInfo -> statusBox = CreateWindowEx (WS_EX_CLIENTEDGE, 
    	EDITBOX_CLASS_NAME, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | 
    	ES_READONLY | ES_MULTILINE,
    	0, 0, 0, 0, pmStatusWindow, (HMENU) ID_STATUS_BOX,
    	myApplicationInstance, NULL);
    if (myInfo -> statusBox == NULL)
    {
	EdFail_Warn (IDS_CREATEWINDOWFAIL, __FILE__, __LINE__, GetLastError ());
    	return -1;
    }

    // Add the up down control    	
    myInfo -> upDown = CreateUpDownControl (WS_CHILD, 0, 0, 0, 0,
        pmStatusWindow, ID_UPDOWN, (HINSTANCE) 
        myApplicationInstance, NULL, 0, 0, 0);
    if (myInfo -> statusBox == NULL)
    {
    	EdFail_Warn (IDS_CREATEWINDOWFAIL, __FILE__, __LINE__, GetLastError ());
    	return -1;
    }
    	    
    // Add the position box (a status control)
    myInfo -> positionBox = CreateStatusWindow (
        WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP | CCS_NORESIZE, 
        "", pmStatusWindow, ID_POSITION_BOX);
    if (myInfo -> statusBox == NULL)
    {
  	EdFail_Warn (IDS_CREATEWINDOWFAIL, __FILE__, __LINE__, GetLastError ());
	return -1;
    }
    	 
    myInfo -> toolTip = CreateWindow (TOOLTIPS_CLASS, NULL, TTS_ALWAYSTIP, 
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
        pmStatusWindow, (HMENU) NULL, myApplicationInstance, NULL); 
    if (myInfo -> toolTip == NULL)
    {
  	EdFail_Warn (IDS_CREATEWINDOWFAIL, __FILE__, __LINE__, GetLastError ());
	return -1;
    }
 
    // Add the status box to the tool tip window tools
    myToolInfo.cbSize = sizeof(TOOLINFO); 
    myToolInfo.uFlags = TTF_IDISHWND; 
    myToolInfo.hwnd = pmStatusWindow; 
    myToolInfo.uId = (UINT) myInfo -> statusBox; 
    myToolInfo.hinst = 0; 
    myToolInfo.lpszText = LPSTR_TEXTCALLBACK; 
    if (!SendMessage (myInfo -> toolTip, TTM_ADDTOOL, 0, (LPARAM) &myToolInfo))
    {
	// TW Not precisely the right error message
	EdFail_Warn (IDS_CREATEWINDOWFAIL, __FILE__, __LINE__, GetLastError ());
	return -1;
    }

    // If the font properties have not already been determined, 
    // determine them now (we needed a window to do so)
    EdStat_PropertiesImplementChanges (pmStatusWindow);
    	    
    // Set the window to initialized
    SetWindowLong (pmStatusWindow, WINDOW_MEM_WINDOW_INITIALIZED, (long) TRUE);
     
    if (stToolTipRelayerHook == NULL)
    {
	stToolTipRelayerHook = SetWindowsHookEx (WH_GETMESSAGE, 
	    MyToolTipRelayer, NULL, GetCurrentThreadId ());
    }

    return 0;
} // MyCreateStatusWindow


/************************************************************************/
/* MyCreateStatusWindowInfo						*/
/************************************************************************/
static StatusWindowInfoPtr	MyCreateStatusWindowInfo (void)
{
    StatusWindowInfoPtr	myInfo;
    
    // Allocate the structure for the information about the status window
    myInfo = (StatusWindowInfoPtr) malloc (sizeof (StatusWindowInfo));
    
    if (myInfo != NULL)
    {
    	ZeroMemory (myInfo, sizeof (StatusWindowInfo));
    	
    	myInfo -> lastLine = -1;
    }

    return myInfo;  
} // MyCreateStatusWindowInfo


/************************************************************************/
/* MyDispose							 	*/
/************************************************************************/
static void	MyDispose (HWND pmStatusWindow)
{
    StatusWindowInfoPtr	myInfo;
    
    myInfo = MyGetStatusWindowInfo (pmStatusWindow);
    if (myInfo -> statusMessage != NULL)
    {
	free (myInfo -> statusMessage);
    }

    free (myInfo);
} // MyDispose


/************************************************************************/
/* MyGetStatusWindowInfo						*/
/************************************************************************/
static StatusWindowInfoPtr	MyGetStatusWindowInfo (HWND pmStatusWindow)
{
    int			myWindowType;
    StatusWindowInfoPtr myInfo;
    
    myWindowType = GetWindowLong (pmStatusWindow, WINDOW_MEM_WINDOW_TYPE);
    if (myWindowType != WINDOW_TYPE_STATUS)
    {
    	EdFail_Fatal (IDS_BADWINDOWTYPE, __FILE__, __LINE__, 0,
    		      myWindowType, WINDOW_TYPE_STATUS);
    }
    	
    myInfo = (StatusWindowInfoPtr) GetWindowLong (pmStatusWindow, 
        WINDOW_MEM_WINDOW_INFO);
    if (myInfo == NULL)
    {
    	EdFail_Fatal (IDS_GETWINDOWLONGFAIL, __FILE__, __LINE__,
    		      GetLastError (), WINDOW_MEM_WINDOW_INFO);
    }
       
    return myInfo;
} // MyGetStatusWindowInfo


/************************************************************************/
/* MyRedrawPositionInfo							*/
/*									*/
/* Set the cursor position panel at the bottom of the window.		*/
/************************************************************************/
static void	MyRedrawPositionInfo (HWND pmStatusWindow)
{
    StatusWindowInfoPtr	myInfo;
    char		myStr [80];

    myInfo = MyGetStatusWindowInfo (pmStatusWindow);
    
    if (myInfo -> displayShortPositionInfo == myInfo -> lastShort)
    {
    	return;
    }
    
    myInfo -> lastShort = myInfo -> displayShortPositionInfo;

    if (myInfo -> displayShortPositionInfo)
    {
    	wsprintf (myStr, stShortPositionString, myInfo -> lastLine + 1, 
    	    myInfo -> lastNumLines, myInfo -> lastCol + 1);
    }
    else
    {
    	wsprintf (myStr, stLongPositionString, myInfo -> lastLine + 1, 
    	    myInfo -> lastNumLines, myInfo -> lastCol + 1);
    }
    
    SendMessage (myInfo -> positionBox, SB_SETTEXT, 0, (LPARAM) myStr);
} // MyRedrawPositionInfo


/************************************************************************/
/* MyResizeStatusWindow							*/
/************************************************************************/
static void	MyResizeStatusWindow (HWND pmStatusWindow)
{
    RECT		myWindowRect;
    int			myWindowWidth;
    StatusWindowInfoPtr	myInfo;
    int			myControlHeight;
    int			myPositionBoxWidth;
    int			myUpDownLeft;
    HDWP		myDeferWindowPosHandle;
    
    // The first resizing comes before there's a client area, so
    // we put this check here to ignore it
    if (GetClientRect (pmStatusWindow, &myWindowRect) == 0)
    {
    	return;
    }

    if (!GetWindowLong (pmStatusWindow, WINDOW_MEM_WINDOW_INITIALIZED))
    {
   	return;
    }
	    
    myWindowWidth = myWindowRect.right - myWindowRect.left;
	    
    // Set the position of the three components
    myInfo = MyGetStatusWindowInfo (pmStatusWindow);
    	    
    myControlHeight = EdStat_GetWindowHeight () - TOP_MARGIN;

    // We want space for 40 characters in the status box + UP_DOWN_WIDTH 
    // pixels for the updown control + UP_DOWN_LEFT_MARGIN + 
    // UP_DOWN_RIGHT_MARGIN pixels for the margin on either side + 
    // 30 characters in the position box.  If there isn't enough space,
    // then reserve only 23 characters for the position box,
    if (myWindowRect.right > 
    	(STATUS_SIZE_IN_CHARS * stStatusFontCharWidth + 
            UP_DOWN_WIDTH + UP_DOWN_LEFT_MARGIN + UP_DOWN_RIGHT_MARGIN +
            LARGE_POSITION_SIZE_IN_CHARS * stStatusFontCharWidth))
    {
    	// There's enough room
    	myPositionBoxWidth = LARGE_POSITION_SIZE_IN_CHARS * 
    	    stStatusFontCharWidth;
    	myInfo -> displayShortPositionInfo = FALSE;
    }
    else
    {
    	// There isn't enough room, use the smaller position size
    	myPositionBoxWidth = SMALL_POSITION_SIZE_IN_CHARS * 
    	    stStatusFontCharWidth;
    	myInfo -> displayShortPositionInfo = TRUE;
    }
    
    //
    // Resize the child windows
    //
    myDeferWindowPosHandle = BeginDeferWindowPos (3);
    if (myDeferWindowPosHandle == NULL)
    {
        EdFail_Fatal (IDS_NO_MEM_FOR_RESIZE, __FILE__, __LINE__, 
            GetLastError ());
    }

    // Set the position of the Status Box
    myUpDownLeft = myWindowWidth - myPositionBoxWidth - UP_DOWN_WIDTH - 
        UP_DOWN_RIGHT_MARGIN;
    DeferWindowPos (myDeferWindowPosHandle, myInfo -> statusBox, NULL, 0, 2,
        myUpDownLeft - UP_DOWN_LEFT_MARGIN, myControlHeight, SWP_NOZORDER);
    
    // Set the maximum width of the tooltip        
    SendMessage (myInfo -> toolTip, TTM_SETMAXTIPWIDTH, TRUE, 
        myUpDownLeft - UP_DOWN_LEFT_MARGIN);
        
    // Set the delay times
    SendMessage (myInfo -> toolTip, TTM_SETDELAYTIME, TTDT_INITIAL, 
        MAKELONG(100,0));
    SendMessage (myInfo -> toolTip, TTM_SETDELAYTIME, TTDT_AUTOPOP, 
        MAKELONG(FOREVER,0));
	
    // Set the position of the Up Down Control
    DeferWindowPos (myDeferWindowPosHandle, myInfo -> upDown, NULL, 
        myUpDownLeft, 2, UP_DOWN_WIDTH, myControlHeight, SWP_NOZORDER);

    // Set the position of the Position Box
    DeferWindowPos (myDeferWindowPosHandle, myInfo -> positionBox, NULL, 
        myWindowWidth - myPositionBoxWidth, 1,
        myPositionBoxWidth, myControlHeight, SWP_NOZORDER);

    // Set the split status bar
    {
    	int	myWidths [2];
    	myWidths [0] = myPositionBoxWidth - myControlHeight;
    	myWidths [1] = -1;
    	SendMessage (myInfo -> positionBox, SB_SETPARTS, (WPARAM) 2,
    	    (LPARAM) myWidths);
    	SendMessage (myInfo -> positionBox, SB_SETTEXT, 
    	    (WPARAM) 1 | SBT_NOBORDERS, (LPARAM) "");
    }
    	
    if (!EndDeferWindowPos (myDeferWindowPosHandle))
    {
        EdFail_Fatal (IDS_NO_RESIZE, __FILE__, __LINE__, GetLastError ());
    }
	    
    // Redraw the status and position info, if necessary.
    MySetStatusUpDown (pmStatusWindow);
    MyRedrawPositionInfo (pmStatusWindow);
} // MyResizeStatusWindow

/************************************************************************/
/* MySetDialogFonts							*/
/************************************************************************/
static BOOL	MySetDialogFonts (HWND pmStatusWindow)
{
    char	myFontName [256];
    HDC		myDisplayContext;
    TEXTMETRIC	myTextMetrics;
    
    // Get the font name
    EdGUI_LoadString (IDS_DIALOG_FONT_NAME, myFontName, sizeof (myFontName));

    // Delete any previously existing font
    if (stStatusFont != NULL)
    {
    	DeleteObject (stStatusFont);
    }
    
    // Create the new font
    stStatusFont = EdGUI_CreateFont (pmStatusWindow, NULL, myFontName,
        STATUS_BAR_FONT_SIZE, NO_BOLD, NO_ITALIC);
    if (stStatusFont == NULL)
    {
    	EdFail_Warn (IDS_CREATEFONTFAIL, __FILE__, __LINE__, 
    			  GetLastError ());
    	return FALSE;
    }

    // Delete any previously existing font
    if (stBoldStatusFont != NULL)
    {
    	DeleteObject (stBoldStatusFont);
    }
    
    // Create the new font
    stBoldStatusFont =  EdGUI_CreateFont (pmStatusWindow, NULL, myFontName,
        STATUS_BAR_FONT_SIZE, BOLD, NO_ITALIC);
    if (stBoldStatusFont == NULL)
    {
    	EdFail_Warn (IDS_CREATEFONTFAIL, __FILE__, __LINE__, 
    			  GetLastError ());
    	return FALSE;
    }

    myDisplayContext = GetDC (pmStatusWindow);
    SelectFont (myDisplayContext, stStatusFont);
    GetTextMetrics (myDisplayContext, &myTextMetrics);
    stStatusFontCharHeight = myTextMetrics.tmHeight;
    stStatusFontCharWidth = myTextMetrics.tmAveCharWidth;
    ReleaseDC (pmStatusWindow, myDisplayContext);
    
    return TRUE;
} // MySetDialogFonts


/*************************************************************************/
/* MySetStatus								 */
/*									 */
/* Display a string in the information panel at the bottom of the window */
/*************************************************************************/
static void	MySetStatus (HWND pmStatusWindow, const char *pmMessage, 
			     BOOL pmBold)
{
    HWND		myStatusBox;
    StatusWindowInfoPtr	myInfo;
    char		*myEmpty = "";
//    static unsigned int	myStatusMessageSize = 0;

    myInfo = MyGetStatusWindowInfo (pmStatusWindow);
    myStatusBox = myInfo -> statusBox;
    
    if ((myInfo -> statusMessage == NULL) ||
	(strcmp (myInfo -> statusMessage, pmMessage) != 0))
    {    
    	if (pmBold)
    	{
    	    SendMessage (myStatusBox, WM_SETTEXT, 0, (LPARAM) myEmpty);
    	    SendMessage (myStatusBox, WM_SETFONT, (WPARAM) stBoldStatusFont, 0);
    	    myInfo -> isBold = TRUE;
    	}
    	else
    	{
    	    SendMessage (myStatusBox, WM_SETTEXT, 0, (LPARAM) myEmpty);
    	    SendMessage (myStatusBox, WM_SETFONT, (WPARAM) stStatusFont, 0);
    	    myInfo -> isBold = FALSE;
    	}
    	
    	SendMessage (myStatusBox, WM_SETTEXT, 0, (LPARAM) pmMessage);
    	MySetStatusUpDown (pmStatusWindow);
 
	if ((int) strlen (pmMessage) > myInfo -> statusMessageSize)
	{
	    if (myInfo -> statusMessage != NULL)
	    {
		free (myInfo -> statusMessage);
	    }
	    
	    myInfo -> statusMessage = malloc (strlen (pmMessage) + 1);
	    myInfo -> statusMessageSize = strlen (pmMessage);
	}
	if (myInfo -> statusMessage != NULL)
	{
	    strcpy (myInfo -> statusMessage, pmMessage);
	}
	
	// Set tooltip not to appear on empty status box
	if (pmMessage [0] == 0)
	{
	    SendMessage (myInfo -> toolTip, TTM_SETDELAYTIME, TTDT_INITIAL, 
	        MAKELONG(FOREVER,0));
	}
	else
	{
	    SendMessage (myInfo -> toolTip, TTM_SETDELAYTIME, TTDT_INITIAL, 
	        MAKELONG(100,0));
	}
    }

    myInfo -> allowScrolling = TRUE;
    
    RedrawWindow (myStatusBox, NULL, NULL, RDW_UPDATENOW);
} // MySetStatus


/************************************************************************/
/* MySetStatusUpDown							*/
/*									*/
/* Set the cursor position panel at the bottom of the window.		*/
/************************************************************************/
static void	MySetStatusUpDown (HWND pmStatusWindow)
{
    StatusWindowInfoPtr	myInfo;
    HWND		myStatusBox, myUpDown;
    int			myLineCount;
    
    myInfo = MyGetStatusWindowInfo (pmStatusWindow);
    myStatusBox = myInfo -> statusBox;
    myUpDown = myInfo -> upDown;
    
    myLineCount = SendMessage (myStatusBox, EM_GETLINECOUNT, 0, 0);
    if (myLineCount > 1)
    {
    	ShowWindow (myUpDown, SW_SHOW);
    	SendMessage (myUpDown, UDM_SETRANGE, 0, 
    	        (LPARAM) MAKELONG(myLineCount, 1)); 
    	SendMessage (myUpDown, UDM_SETPOS, 0, (LPARAM) MAKELONG(myLineCount, 0));
    	myInfo -> currentStatusLine = myLineCount;
    }
    else
    {
    	ShowWindow (myUpDown, SW_HIDE);
    }
    RedrawWindow (myUpDown, NULL, NULL, RDW_UPDATENOW);
} // MySetStatusUpDown


/************************************************************************/
/* MyToolTipRelayer							*/
/************************************************************************/
static LRESULT CALLBACK MyToolTipRelayer (int pmNCode, WPARAM pmWParam, 
					  LPARAM pmLParam) 
{ 
    MSG	    *myMessage;
    HWND    myWindow;
    DWORD   myWindowAtom = 0;

    myMessage = (MSG *) pmLParam;

    myWindow = myMessage -> hwnd;

    if (myWindow != NULL)
    {
	if (GetParent (myWindow) != NULL)
	{
	    myWindowAtom = GetClassLong (GetParent (myWindow), GCW_ATOM);
	}
	
	if ((pmNCode >= 0) && (myWindowAtom == stStatusWindowAtom))
	{
	    MSG	myRelayedMessage;
	    HWND	myStatusWindow = GetParent (myWindow);
	    
	    switch (myMessage -> message)
	    {
	    case WM_MOUSEMOVE: 
	    case WM_LBUTTONDOWN: 
	    case WM_LBUTTONUP: 
	    case WM_RBUTTONDOWN: 
	    case WM_RBUTTONUP: 
                myRelayedMessage.lParam = myMessage -> lParam; 
		myRelayedMessage.wParam = myMessage -> wParam;
		myRelayedMessage.message = myMessage -> message; 
		myRelayedMessage.hwnd = myMessage -> hwnd;
		SendMessage (MyGetStatusWindowInfo (myStatusWindow) -> toolTip,
		    TTM_RELAYEVENT, 0, (LPARAM) (LPMSG) &myRelayedMessage); 
		break;
	    }
	} 
    }

    return CallNextHookEx (stToolTipRelayerHook, pmNCode, pmWParam, pmLParam); 
} // MyToolTipRelayer
 