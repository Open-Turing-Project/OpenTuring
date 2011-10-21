/************************************************************************/
/* jstop.c								*/
/*									*/
/* JSTop uses the following routines in other modules:			*/
/*	EdFail - various						*/
/*	EdGUI - EdGUI_CreateFont					*/
/************************************************************************/

/*******************/
/* System includes */
/*******************/
#include <windows.h>
#include <windowsx.h>
#include <crtdbg.h>

/****************/
/* Self include */
/****************/
#include "jstop.h"

/******************/
/* Other includes */
/******************/
#include "jsmain.h"

#include "edglob.h"
#include "edgui.h"

#include "jscommands.h"
#include "jsdialogs.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
// Windows class name for a button
#define BUTTON_CLASS_NAME		"button"

// The short and long buttons (6 each)
#define NUM_BUTTONS			12
#define NUM_AREAS			 1

#define FRAME_GAP			 4
#define CTRL_GAP			 4

#define BUTTON_HEIGHT_MULTIPLIER_SMALL	1.8
#define BUTTON_HEIGHT_MULTIPLIER_MID	1.5
#define BUTTON_HEIGHT_MULTIPLIER_LARGE	1.2


/********************/
/* Global variables */
/********************/

/*********/
/* Types */
/*********/
typedef struct
{
    // The elements in the window
    HWND	button [NUM_BUTTONS];
    HWND	raisedArea;
    BOOL	drawLine;
} ButtonBarWindowInfo, *ButtonBarWindowInfoPtr;

typedef struct
{
    int	buttonNameResource;
    int buttonCommand;
} ButtonInfo;

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
static JSTop_Properties	stProperties;
static BOOL		stPropertiesChanged;

//
static HBRUSH	stBackgroundBrush;

// Used for the status box
static HFONT	stButtonFont;
static int	stButtonFontCharWidth, stButtonFontCharHeight;

// Used for sizing the button bars
static int	stLongWidth, stShortWidth, stTwoLongWidth;
static int	stTwoShortWidth, stOneLongWidth;

static int	stShortAreaXPos, stShortAreaWidth;
static int	stLongAreaXPos, stLongAreaWidth;
static int	stButtonXPos [NUM_BUTTONS], stButtonWidth [NUM_BUTTONS];

// The string resource names of the buttons in order
static ButtonInfo	stButtonInfo [NUM_BUTTONS] = 
{
    {IDS_PAUSE_BUTTON, COMMAND_PAUSE_RESUME},
    {IDS_CLOSE_BUTTON, COMMAND_TERMINATE},
    {IDS_PRINT_BUTTON, COMMAND_PRINT_FILE},
    {IDS_SAVE_BUTTON, COMMAND_SAVEAS_FILE},
    {IDS_COPY_BUTTON, COMMAND_COPY},
    {IDS_PASTE_BUTTON, COMMAND_PASTE},
    
    {IDS_PAUSE_BUTTON_LONG, COMMAND_PAUSE_RESUME},
    {IDS_CLOSE_BUTTON_LONG, COMMAND_TERMINATE},
    {IDS_PRINT_BUTTON_LONG, COMMAND_PRINT_FILE},
    {IDS_SAVE_BUTTON_LONG, COMMAND_SAVEAS_FILE},
    {IDS_COPY_BUTTON_LONG, COMMAND_COPY},
    {IDS_PASTE_BUTTON_LONG, COMMAND_PASTE}
};
	
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
static int			MyCreateButtonBar (HWND pmButtonBarWindow);
static ButtonBarWindowInfoPtr	MyCreateButtonBarWindowInfo (void);
static void			MyDispose (HWND pmButtonBarWindow);
static void			MyDrawShadedFrame (HDC pmDeviceContext, 
						   RECT *pmRectPtr, 
		    				   BOOL pmDepressed);
static ButtonBarWindowInfoPtr	MyGetButtonBarWindowInfo (
						HWND pmButtonBarWindow);
static int			MyGetButtonSize (HWND pmButtonBarWindow, 
						 ButtonBarWindowInfoPtr pmInfo, 
						 int pmButtonFrom, 
						 int pmButtonTo, int *pmXPos);
static void			MyInitializeButtonBarSizes (
						HWND pmButtonBarWindow, 
						ButtonBarWindowInfoPtr pmInfo);
static void			MyResizeButtonBar (HWND pmButtonBarWindow);
static BOOL			MySetDialogFont (HWND pmButtonBarWindow);


/***********************/
/* External procedures */
/***********************/
/*************************************************************************/
/* JSTop_Init								 */
/*************************************************************************/
void	JSTop_Init (void)
{
    char	myButtonBarWindowClassName [256];
//    char	myRaisedFrameWindowClassName [256];
    int		myResult;
    WNDCLASSEX	myWindowClass; // , myRaisedFrameClass;

    // Get the background brush
    stBackgroundBrush = CreateSolidBrush (GetSysColor (COLOR_BTNFACE));
    if (stBackgroundBrush == NULL)	  
    {
    	JS_ErrorMessage (IDS_CREATEBRUSHFAIL, __FILE__, __LINE__, 
    		GetLastError ());
    }
    
    // Get the class name
    EdGUI_LoadString (IDS_JSTOP_WINDOW_NAME, myButtonBarWindowClassName, 
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
    	JS_ErrorMessage (IDS_REGISTERCLASSFAIL, __FILE__, __LINE__, 
    			 GetLastError ());
    }
} // JSTop_Init


/*************************************************************************/
/* JSTop_Finalize							 */
/*************************************************************************/
void	JSTop_Finalize (void)
{
} // JSTop_Finalize


/************************************************************************/
/* JSTop_PropertiesSet							*/
/************************************************************************/
void	JSTop_PropertiesSet (JSTop_Properties pmProperties)
{
    stProperties = pmProperties;
    stPropertiesChanged = TRUE;
} // JSTop_PropertiesSet

	
/************************************************************************/
/* JSTop_PropertiesImplementChanges					*/
/************************************************************************/
void	JSTop_PropertiesImplementChanges (HWND pmButtonBarWindow)
{
    ButtonBarWindowInfoPtr	myInfo;
    int				cnt;
    	
    if (stPropertiesChanged)
    {
    	MySetDialogFont (pmButtonBarWindow);
        stPropertiesChanged = FALSE;
    }
    	
    // Set the font of the buttons
    myInfo = MyGetButtonBarWindowInfo (pmButtonBarWindow);

    for (cnt = 0 ; cnt < NUM_BUTTONS ; cnt++)
    {
        SendMessage (myInfo -> button [cnt], WM_SETFONT, 
            (WPARAM) stButtonFont, 0);
    }	
    	     
} // JSTop_PropertiesImplementChanges


/*************************************************************************/
/* JSTop_Create								 */
/*************************************************************************/
HWND	JSTop_Create (HWND pmEditWindow)
{
    HINSTANCE	myApplicationInstance;
    char	myButtonBarWindowClassName [256];
    HWND	myButtonBarWindow;
    
    // Get the program instance
    myApplicationInstance = 
        (HINSTANCE) GetWindowLong (pmEditWindow, GWL_HINSTANCE);
        
    // Load the class name (we've already loaded it once, so no error checking)    
    EdGUI_LoadString (IDS_JSTOP_WINDOW_NAME, myButtonBarWindowClassName, 
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

    return myButtonBarWindow;
} // JSTop_Create


/*************************************************************************/
/* JSTop_GetWindowHeight						 */
/*************************************************************************/
int	JSTop_GetWindowHeight (void)
{
    double  myMultiplier;

    if (stProperties.dialogFontSize < 12)
	myMultiplier = BUTTON_HEIGHT_MULTIPLIER_SMALL;
    else if (stProperties.dialogFontSize < 18)
	myMultiplier = BUTTON_HEIGHT_MULTIPLIER_MID;
    else
	myMultiplier = BUTTON_HEIGHT_MULTIPLIER_LARGE;

    return (int) (stButtonFontCharHeight * myMultiplier) + 
	2 * CTRL_GAP + 2 * FRAME_GAP;
} // JSTop_GetWindowHeight


/*************************************************************************/
/* JSTop_DisablePauseButton						 */
/*************************************************************************/
void	JSTop_DisablePauseButton (HWND pmButtonBarWindow)
{
    ButtonBarWindowInfoPtr	myInfo;
    	
    myInfo = MyGetButtonBarWindowInfo (pmButtonBarWindow);
    
    EnableWindow (myInfo -> button [0], FALSE);
    EnableWindow (myInfo -> button [6], FALSE);
} // JSTop_DisablePauseButton


/*************************************************************************/
/* JSTop_SwitchPauseToResume						 */
/*************************************************************************/
void	JSTop_SwitchPauseToResume (HWND pmButtonBarWindow)
{
    ButtonBarWindowInfoPtr	myInfo;
    char			myButtonName [256];
    	
    myInfo = MyGetButtonBarWindowInfo (pmButtonBarWindow);

    EdGUI_LoadString (IDS_RESUME_BUTTON, myButtonName, sizeof (myButtonName));
    SetWindowText (myInfo -> button [0], myButtonName);
    
    EdGUI_LoadString (IDS_RESUME_BUTTON_LONG, myButtonName, 
    		      sizeof (myButtonName));
    SetWindowText (myInfo -> button [6], myButtonName);
} // JSTop_SwitchPauseToResume


/*************************************************************************/
/* JSTop_SwitchResumeToPause						 */
/*************************************************************************/
void	JSTop_SwitchResumeToPause (HWND pmButtonBarWindow)
{
    ButtonBarWindowInfoPtr	myInfo;
    char			myButtonName [256];
    	
    myInfo = MyGetButtonBarWindowInfo (pmButtonBarWindow);

    EdGUI_LoadString (IDS_PAUSE_BUTTON, myButtonName, sizeof (myButtonName));
    SetWindowText (myInfo -> button [0], myButtonName);
    
    EdGUI_LoadString (IDS_PAUSE_BUTTON_LONG, myButtonName, 
    		      sizeof (myButtonName));
    SetWindowText (myInfo -> button [6], myButtonName);
} // JSTop_SwitchResumeToPause


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
    HDC			myDeviceContext;
    PAINTSTRUCT		myPaintStruct;
    RECT		myWindowRect;
    
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
    	    myDeviceContext = BeginPaint (pmButtonBarWindow, &myPaintStruct);
    	    GetClientRect (pmButtonBarWindow, &myWindowRect);
    	    MoveToEx (myDeviceContext, 0, myWindowRect.bottom - 1, NULL);
    	    LineTo (myDeviceContext, myWindowRect.right, 
    	        myWindowRect.bottom - 1);
    	    EndPaint (pmButtonBarWindow, &myPaintStruct);
    	    return 0;
	    
	case WM_COMMAND:
	    // Pass the command to the parent
	    SetFocus (GetParent (pmButtonBarWindow));
	    return SendMessage (GetParent (pmButtonBarWindow), WM_COMMAND,
	    			pmWParam, pmLParam);

	case WM_SYSCOLORCHANGE:
	    //
	    // Set the new background colours
	    //
	    
	    // Delete the old background brush
	    if (stBackgroundBrush != NULL)
	    {
	    	DeleteObject (stBackgroundBrush);
	    }
	    
	    // Create the new background brush
	    stBackgroundBrush = CreateSolidBrush (GetSysColor (COLOR_BTNFACE));
	    
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
/* MyCreateButtonBar							*/
/************************************************************************/
static int	MyCreateButtonBar (HWND pmButtonBarWindow)
{
    HINSTANCE			myApplicationInstance;
    ButtonBarWindowInfoPtr	myInfo;
    char			myButtonName [256];
    char			myRaisedFrameWindowClassName [256];
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
    JSTop_PropertiesImplementChanges (pmButtonBarWindow);
   	    
    EdGUI_LoadString (IDS_RAISED_FRAME_WINDOW_NAME, 
        myRaisedFrameWindowClassName, sizeof (myRaisedFrameWindowClassName));
		
    // Create all the raised areas
    myInfo -> raisedArea = CreateWindow (myRaisedFrameWindowClassName,
    	NULL, WS_CHILD | WS_DISABLED, 0, 0, 0, 0,
    	pmButtonBarWindow, NULL, myApplicationInstance, NULL);
 
    // Create all the buttons
    for (cnt = 0 ; cnt < NUM_BUTTONS ; cnt++)
    {
        // Get the button name
        EdGUI_LoadString (stButtonInfo [cnt].buttonNameResource,
			  myButtonName, sizeof (myButtonName));
        myInfo -> button [cnt] = CreateWindow (BUTTON_CLASS_NAME,
            myButtonName, WS_CHILD | BS_PUSHBUTTON, 0, 0, 0, 0, 
            pmButtonBarWindow, (HMENU) stButtonInfo [cnt].buttonCommand, 
            myApplicationInstance, NULL);
        SendMessage (myInfo -> button [cnt], WM_SETFONT, 
            (WPARAM) stButtonFont, 0);
    }		     
    
    // Initialize the button bar sizes
    if (!stButtonBarButtonSizesInitialized)
    {
    	MyInitializeButtonBarSizes (pmButtonBarWindow, myInfo);
    	stButtonBarButtonSizesInitialized = TRUE;
    }

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
    	JS_ErrorMessage (IDS_BADWINDOWTYPE, __FILE__, __LINE__, 0,
    		         myWindowType, WINDOW_TYPE_BEGINNER_TOP);
    }
    	
    myInfo = (ButtonBarWindowInfoPtr) GetWindowLong (pmButtonBarWindow, 
        WINDOW_MEM_WINDOW_INFO);
    if (myInfo == NULL)
    {
    	JS_ErrorMessage (IDS_GETWINDOWLONGFAIL, __FILE__, __LINE__,
    		         GetLastError (), WINDOW_MEM_WINDOW_INFO);
    }
       
    return myInfo;
} // MyGetButtonBarWindowInfo


/************************************************************************/
/* MyGetButtonSize							*/
/************************************************************************/
static int	MyGetButtonSize (HWND pmButtonBarWindow, 
				 ButtonBarWindowInfoPtr pmInfo, 
				 int pmButtonFrom, int pmButtonTo, int *pmXPos)
{
    int		myMaxSize = 0;
    HDC		myDeviceContext;
    char	myButtonLabel [256];
    SIZE	myExtent;
    int		cnt;
    
    myDeviceContext = GetDC (pmInfo -> button [pmButtonFrom]);
    SelectFont (myDeviceContext, stButtonFont);
    
    for (cnt = pmButtonFrom ; cnt <= pmButtonTo ; cnt++)
    {
    	SendMessage (pmInfo -> button [cnt], WM_GETTEXT, 
    	    sizeof (myButtonLabel), (LPARAM) myButtonLabel);
    	    
	GetTextExtentPoint32 (myDeviceContext, myButtonLabel, 
	    strlen (myButtonLabel), &myExtent);
	if (myExtent.cx > myMaxSize)
	{
	    myMaxSize = myExtent.cx;
	}
    }

    ReleaseDC (pmInfo -> button [pmButtonFrom], myDeviceContext);

    myMaxSize += 2 * stButtonFontCharWidth;
    
    for (cnt = pmButtonFrom ; cnt <= pmButtonTo ; cnt++)
    {
    	*pmXPos += CTRL_GAP;
    	stButtonXPos [cnt] = *pmXPos;
        *pmXPos += myMaxSize;
        stButtonWidth [cnt] = myMaxSize;
    }
    
    *pmXPos += CTRL_GAP;
    
    return myMaxSize;
} // MyGetButtonSize

/************************************************************************/
/* MyInitializeButtonBarSizes						*/
/************************************************************************/
static void	MyInitializeButtonBarSizes (HWND pmButtonBarWindow, 
					    ButtonBarWindowInfoPtr pmInfo)
{
    int	myXPos;
    int	myShortWidth, myLongWidth;
    
    myXPos = FRAME_GAP;
    stShortAreaXPos = FRAME_GAP;
    myShortWidth = MyGetButtonSize (pmButtonBarWindow, pmInfo, 0, 5, &myXPos);
    stShortAreaWidth = myXPos - stShortAreaXPos;
        
    myXPos = FRAME_GAP;
    stLongAreaXPos = FRAME_GAP;
    myLongWidth = MyGetButtonSize (pmButtonBarWindow, pmInfo, 6, 11, &myXPos);
    stLongAreaWidth = myXPos - stLongAreaXPos;
    
    stLongWidth = 2 * FRAME_GAP + 6 * CTRL_GAP + 5 * myShortWidth;
    stShortWidth = 2 * FRAME_GAP + 6 * CTRL_GAP + 5 * myLongWidth;
} // MyInitializeButtonBarSizes
					    

/************************************************************************/
/* MyResizeButtonBar							*/
/************************************************************************/
static void	MyResizeButtonBar (HWND pmButtonBarWindow)
{
    RECT			myWindowRect;
    int				myWindowWidth;
    ButtonBarWindowInfoPtr	myInfo;
    int				myFrameTop, myFrameHeight;
    int				myButtonTop, myButtonHeight;
    int				myFromButton, myToButton, myToArea;
    BOOL			myLong;
    int				cnt;
    double			myMultiplier;
        
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
	    
    myWindowWidth = myWindowRect.right - myWindowRect.left;

    myFrameTop = FRAME_GAP;
    myButtonTop = FRAME_GAP + CTRL_GAP;

    if (stProperties.dialogFontSize < 12)
	myMultiplier = BUTTON_HEIGHT_MULTIPLIER_SMALL;
    else if (stProperties.dialogFontSize < 18)
	myMultiplier = BUTTON_HEIGHT_MULTIPLIER_MID;
    else
	myMultiplier = BUTTON_HEIGHT_MULTIPLIER_LARGE;

    myButtonHeight = (int) (stButtonFontCharHeight * myMultiplier);	    
    myFrameHeight = myButtonHeight + 2 * CTRL_GAP;
    
    // Set the posiiton of the three components
    myInfo = MyGetButtonBarWindowInfo (pmButtonBarWindow);

    if (myWindowWidth >= stLongWidth)
    {
    	myFromButton = 6;
    	myToButton = 11;
    	myToArea = 2;
    	myLong = TRUE;
    }
    else 
    {
    	myFromButton = 0;
    	myToButton = 5;
    	myToArea = 2;
    	myLong = FALSE;
    }
    
    for (cnt = 0 ; cnt < myFromButton ; cnt++)
    {
    	ShowWindow (myInfo -> button [cnt], SW_HIDE);
    }
    
    for (cnt = myFromButton ; cnt <= myToButton ; cnt++)
    {
	SetWindowPos (myInfo -> button [cnt], NULL, stButtonXPos [cnt], 
	    myButtonTop, stButtonWidth [cnt], myButtonHeight,
	    SWP_NOZORDER);
    	ShowWindow (myInfo -> button [cnt], TRUE);
    }    	
    
    for (cnt = myToButton + 1 ; cnt < NUM_BUTTONS ; cnt++)
    {
    	ShowWindow (myInfo -> button [cnt], SW_HIDE);
    }
    
    if (myLong)
    {
	SetWindowPos (myInfo -> raisedArea, NULL, stLongAreaXPos, myFrameTop,
	    stLongAreaWidth, myFrameHeight, SWP_NOZORDER);
    }
    else
    {
	SetWindowPos (myInfo -> raisedArea, NULL, stShortAreaXPos, myFrameTop,
	    stShortAreaWidth, myFrameHeight, SWP_NOZORDER);
    }
    	
    ShowWindow (myInfo -> raisedArea, TRUE);
    
    RedrawWindow (pmButtonBarWindow, NULL, NULL, RDW_INVALIDATE);
} // MyResizeButtonBar


/************************************************************************/
/* MySetDialogFont							*/
/************************************************************************/
static BOOL	MySetDialogFont (HWND pmButtonBarWindow)
{
    char	myFontName [256];
    HDC		myDisplayContext;
    TEXTMETRIC	myTextMetrics;
    
    // Get the font name
    EdGUI_LoadString (IDS_DIALOG_FONT_NAME, myFontName, sizeof (myFontName));

    // Delete any previously existing font
    if (stButtonFont != NULL)
    {
    	DeleteObject (stButtonFont);
    }
    
    // Create the new font
    stButtonFont = EdGUI_CreateFont (pmButtonBarWindow, NULL, myFontName,
        stProperties.dialogFontSize, NO_BOLD, NO_ITALIC);
    if (stButtonFont == NULL)
    {
    	JS_ErrorMessage (IDS_CREATEFONTFAIL, __FILE__, __LINE__, 
    			 GetLastError ());
    	return FALSE;
    }

    myDisplayContext = GetDC (pmButtonBarWindow);
    SelectFont (myDisplayContext, stButtonFont);
    GetTextMetrics (myDisplayContext, &myTextMetrics);
    stButtonFontCharHeight = myTextMetrics.tmHeight;
    stButtonFontCharWidth = myTextMetrics.tmAveCharWidth;
    ReleaseDC (pmButtonBarWindow, myDisplayContext);
    
    return TRUE;
} // MySetDialogFont


