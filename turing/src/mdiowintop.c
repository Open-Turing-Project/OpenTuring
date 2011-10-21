/************************************************************************/
/* mdiowindowtop.c							*/
/*									*/
/* MDIOWindowTop uses the following routines in other modules:		*/
/*	EdFail - various						*/
/*	EdGUI - EdGUI_CreateFont					*/
/************************************************************************/

/****************/
/* Self include */
/****************/
#include "mdiowintop.h"

/******************/
/* Other includes */
/******************/
#include "mio.h"

#include "miocommands.h"
#include "miodialogs.h"

#include "mdiowin.h"

#include "edint.h"

//#include "edglob.h"
//#include "edgui.h"

// Test to make certain we're not accidentally including <windows.h> which
// might allow for windows contamination of platform independent code.
#ifdef _INC_WINDOWS
xxx
#endif

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
// Windows class name for a button
#define BUTTON_CLASS_NAME		"button"

// The short and long buttons (6 each)
#define NUM_BUTTONS			 6
#define NUM_AREAS			 1

#define PAUSE_BUTTON			 0
#define STOP_BUTTON			 1

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
    BOOL	isTextWindowTop;
    int		numButtons;
    HWND	button [NUM_BUTTONS];
    HWND	raisedArea;
    BOOL	drawLine;
    
    // Used for sizing the button bars
    int		areaXPos, areaWidth;
    int		buttonXPos [NUM_BUTTONS], buttonWidth [NUM_BUTTONS];
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
static MDIOWinTop_Properties	stProperties;
static BOOL			stPropertiesChanged;

//
static HBRUSH	stBackgroundBrush;

// Used for the status box
static HFONT	stButtonFont;
static int	stButtonFontCharWidth, stButtonFontCharHeight;

static int	stStopUserClose;

// The string resource names of the buttons in order
static ButtonInfo	stButtonInfo [NUM_BUTTONS] = 
{
    {IDS_MIO_PAUSE_BUTTON, MIO_COMMAND_PAUSE_RESUME},
    {IDS_MIO_STOP_BUTTON, MIO_COMMAND_TERMINATE},
    {IDS_MIO_PRINT_BUTTON, MIO_COMMAND_PRINT_FILE},
    {IDS_MIO_SAVE_BUTTON, MIO_COMMAND_SAVEAS_FILE},
    {IDS_MIO_COPY_BUTTON, MIO_COMMAND_COPY},
    {IDS_MIO_PASTE_BUTTON, MIO_COMMAND_PASTE},
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
static int			MyCreateButtonBar (HWND pmButtonBarWindow,
						   BOOL pmIsTextWindow);
static ButtonBarWindowInfoPtr	MyCreateButtonBarWindowInfo (void);
static void			MyDispose (HWND pmButtonBarWindow);
static void			MyDrawShadedFrame (HDC pmDeviceContext, 
						   RECT *pmRectPtr, 
		    				   BOOL pmDepressed);
static ButtonBarWindowInfoPtr	MyGetButtonBarWindowInfo (
						HWND pmButtonBarWindow);
static int			MyGetButtonSize (HWND pmButtonBarWindow, 
						 ButtonBarWindowInfoPtr pmInfo, 
						 int pmNumButton, int *pmXPos);
static void			MyInitializeButtonBarSizes (
						HWND pmButtonBarWindow, 
						ButtonBarWindowInfoPtr pmInfo);
static void			MyResizeButtonBar (HWND pmButtonBarWindow);
static BOOL			MySetDialogFont (HWND pmButtonBarWindow);


/***********************/
/* External procedures */
/***********************/
/*************************************************************************/
/* MDIOWinTop_Init							 */
/*************************************************************************/
void	MDIOWinTop_Init (BOOL pmStopUserClose)
{
    char	myButtonBarWindowClassName [256];
    int		myResult;
    WNDCLASSEX	myWindowClass;

    stStopUserClose = pmStopUserClose;
    
    // Get the background brush
    stBackgroundBrush = CreateSolidBrush (GetSysColor (COLOR_BTNFACE));
    if (stBackgroundBrush == NULL)	  
    {
    	EdInt_HaltEnvironment (IDS_MIO_CREATEBRUSHFAIL, __FILE__, __LINE__, 
		    	       GetLastError ());
    }
    
    // Get the class name
    EdInt_LoadString (IDS_MIOWINDOWTOP_WINDOW_NAME, myButtonBarWindowClassName, 
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
    myWindowClass.hInstance = 	  MIO_applicationInstance;
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
	EdInt_HaltEnvironment (IDS_MIO_REGISTERCLASSFAIL, __FILE__, __LINE__, 
			       GetLastError ());
    }
} // MDIOWinTop_Init


/*************************************************************************/
/* MDIOWinTop_Finalize							 */
/*************************************************************************/
void	MDIOWinTop_Finalize (void)
{
} // MDIOWinTop_Finalize


/************************************************************************/
/* MDIOWinTop_PropertiesSet						*/
/************************************************************************/
void	MDIOWinTop_PropertiesSet (MDIOWinTop_Properties pmProperties)
{
    stProperties = pmProperties;
    stPropertiesChanged = TRUE;
} // MDIOWinTop_PropertiesSet

	
/************************************************************************/
/* MDIOWinTop_PropertiesImplementChanges				*/
/************************************************************************/
void	MDIOWinTop_PropertiesImplementChanges (WIND pmButtonBarWindow)
{
    ButtonBarWindowInfoPtr	myInfo;
    int				cnt;
    	
    if (stPropertiesChanged)
    {
    	MySetDialogFont ((HWND) pmButtonBarWindow);
        stPropertiesChanged = FALSE;
    }
    	
    // Set the font of the buttons
    myInfo = MyGetButtonBarWindowInfo ((HWND) pmButtonBarWindow);

    for (cnt = 0 ; cnt < myInfo -> numButtons ; cnt++)
    {
        SendMessage (myInfo -> button [cnt], WM_SETFONT, 
            (WPARAM) stButtonFont, 0);
    }	
    	     
} // MDIOWinTop_PropertiesImplementChanges


/*************************************************************************/
/* MDIOWinTop_Create							 */
/*************************************************************************/
WIND	MDIOWinTop_Create (WIND pmEditWindow, BOOL pmIsTextWindow)
{
    HINSTANCE	myApplicationInstance;
    char	myButtonBarWindowClassName [256];
    HWND	myButtonBarWindow;
    
    // Get the program instance
    myApplicationInstance = 
        (HINSTANCE) GetWindowLong ((HWND) pmEditWindow, GWL_HINSTANCE);
        
    // Load the class name (we've already loaded it once, so no error checking)    
    EdInt_LoadString (IDS_MIOWINDOWTOP_WINDOW_NAME, myButtonBarWindowClassName, 
	sizeof (myButtonBarWindowClassName));

    // Create the button bar window
    myButtonBarWindow = CreateWindow (
    	myButtonBarWindowClassName, 		// Window class
        NULL,	 				// Window title
        WS_CHILDWINDOW | WS_VISIBLE,	 	// Window style
    	0, 0, 0, 0, 				// Initial location & size
    	(HWND) pmEditWindow, 			// Parent window handle
    	NULL, 					// Window menu handle
    	myApplicationInstance,	 		// Program instance handle
    	(void *) pmIsTextWindow);		// Creation parameters

    return myButtonBarWindow;
} // MDIOWinTop_Create


/*************************************************************************/
/* MDIOWinTop_GetWindowHeight						 */
/*************************************************************************/
int	MDIOWinTop_GetWindowHeight (void)
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
} // MDIOWinTop_GetWindowHeight


/*************************************************************************/
/* MDIOWinTop_DisablePauseButton					 */
/*************************************************************************/
void	MDIOWinTop_DisablePauseButton (WIND pmButtonBarWindow)
{
    ButtonBarWindowInfoPtr	myInfo;
    char			myButtonName [256];
    
    // If user close is 
    if (stStopUserClose) return;

    myInfo = MyGetButtonBarWindowInfo ((HWND) pmButtonBarWindow);
    
    // Switch back to pause and disable.
    EdInt_LoadString (IDS_MIO_PAUSE_BUTTON, myButtonName, 
		      sizeof (myButtonName));
    SetWindowText (myInfo -> button [PAUSE_BUTTON], myButtonName);
    EnableWindow (myInfo -> button [PAUSE_BUTTON], FALSE);

    // When finished, disable Paste
    EnableWindow (myInfo -> button [myInfo -> numButtons - 1], FALSE);
} // MDIOWinTop_DisablePauseButton


/*************************************************************************/
/* MDIOWinTop_SwitchPauseToResume					 */
/*************************************************************************/
void	MDIOWinTop_SwitchPauseToResume (WIND pmButtonBarWindow)
{
    ButtonBarWindowInfoPtr	myInfo;
    char			myButtonName [256];
    	
    // If user close is 
    if (stStopUserClose) return;

    myInfo = MyGetButtonBarWindowInfo ((HWND) pmButtonBarWindow);

    EdInt_LoadString (IDS_MIO_RESUME_BUTTON, myButtonName, 
		      sizeof (myButtonName));
    SetWindowText (myInfo -> button [PAUSE_BUTTON], myButtonName);

    // When paused, disable Paste
    EnableWindow (myInfo -> button [myInfo -> numButtons - 1], FALSE);
} // MDIOWinTop_SwitchPauseToResume


/************************************************************************/
/* MDIOWinTop_SwitchResumeToPause					*/
/************************************************************************/
void	MDIOWinTop_SwitchResumeToPause (WIND pmButtonBarWindow)
{
    ButtonBarWindowInfoPtr	myInfo;
    char			myButtonName [256];
    	
    // If user close is 
    if (stStopUserClose) return;

    myInfo = MyGetButtonBarWindowInfo ((HWND) pmButtonBarWindow);

    EdInt_LoadString (IDS_MIO_PAUSE_BUTTON, myButtonName, 
		      sizeof (myButtonName));
    SetWindowText (myInfo -> button [PAUSE_BUTTON], myButtonName);

    // When resumed, disable Paste
    EnableWindow (myInfo -> button [myInfo -> numButtons - 1], TRUE);
} // MDIOWinTop_SwitchResumeToPause


/*************************************************************************/
/* MDIOWinTop_SwitchStopToClose						 */
/*************************************************************************/
void	MDIOWinTop_SwitchStopToClose (WIND pmButtonBarWindow)
{
    ButtonBarWindowInfoPtr	myInfo;
    char			myButtonName [256];
    	
    // If user close is 
    if (stStopUserClose) return;

    myInfo = MyGetButtonBarWindowInfo ((HWND) pmButtonBarWindow);

    EdInt_LoadString (IDS_MIO_CLOSE_BUTTON, myButtonName, 
		      sizeof (myButtonName));
    SetWindowText (myInfo -> button [STOP_BUTTON], myButtonName);
} // MDIOWinTop_SwitchStopToClose


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
    BOOL		myIsTextWindow;
    HDC			myDeviceContext;
    PAINTSTRUCT		myPaintStruct;
    RECT		myWindowRect;
    
    switch (pmMessage)
    {
    	case WM_CREATE:
	    myIsTextWindow = (BOOL) 
	    			(((LPCREATESTRUCT) pmLParam) -> lpCreateParams);
    	    return MyCreateButtonBar (pmButtonBarWindow, myIsTextWindow);
    	    
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
	    if ((pmWParam == MIO_COMMAND_TERMINATE) && MIO_finished)
	    {
		return SendMessage (GetParent (pmButtonBarWindow), WM_COMMAND,
	    			    MIO_COMMAND_CLOSE, pmLParam);
	    }
	    else
	    {
		return SendMessage (GetParent (pmButtonBarWindow), WM_COMMAND,
	    			    pmWParam, pmLParam);
	    }

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
static int	MyCreateButtonBar (HWND pmButtonBarWindow, BOOL pmIsTextWindow)
{
    HINSTANCE			myApplicationInstance;
    ButtonBarWindowInfoPtr	myInfo;
    char			myButtonName [256];
    char			myRaisedFrameWindowClassName [256];
    int				cnt;
    int				myButtonNumber = 0;
    
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
        EdInt_AddFailMessage ("MDIOWinTop_MyCreateButtonBar: No info");
        
	return -1;
    }
    SetWindowLong (pmButtonBarWindow, WINDOW_MEM_WINDOW_INFO, (long) myInfo);
   	    
    //
    // Add the elements to the window
    //
    myInfo -> isTextWindowTop = pmIsTextWindow;

    MDIOWinTop_PropertiesImplementChanges (pmButtonBarWindow);
   	    
    EdInt_LoadString (IDS_MIO_RAISED_FRAME_WINDOW_NAME, 
        myRaisedFrameWindowClassName, sizeof (myRaisedFrameWindowClassName));
		
    // Create all the raised areas
    myInfo -> raisedArea = CreateWindow (myRaisedFrameWindowClassName,
    	NULL, WS_CHILD | WS_DISABLED, 0, 0, 0, 0,
    	pmButtonBarWindow, NULL, myApplicationInstance, NULL);
 
    // Create all the buttons
    for (cnt = 0 ; cnt < NUM_BUTTONS ; cnt++)
    {
    	if (stStopUserClose && (cnt < 2))
    	{
    	    continue;
    	}
    	if (!pmIsTextWindow && 
    	    (stButtonInfo [cnt].buttonCommand == MIO_COMMAND_COPY))
    	{
    	    continue;
    	}
    	    
        // Get the button name
        EdInt_LoadString (stButtonInfo [cnt].buttonNameResource,
			  myButtonName, sizeof (myButtonName));
        myInfo -> button [myButtonNumber] = CreateWindow (BUTTON_CLASS_NAME,
                myButtonName, WS_CHILD | BS_PUSHBUTTON, 0, 0, 0, 0, 
                pmButtonBarWindow, (HMENU) stButtonInfo [cnt].buttonCommand, 
                myApplicationInstance, NULL);
        SendMessage (myInfo -> button [myButtonNumber], WM_SETFONT, 
            	     (WPARAM) stButtonFont, 0);
        myButtonNumber++;
    } // for
    
    if (pmIsTextWindow)
    {
    	myInfo -> numButtons = 6;
    }
    else
    {
    	myInfo -> numButtons = 5;
    }
    if (stStopUserClose)
    {
    	myInfo -> numButtons -= 2;
    }
    
    // Initialize the button bar sizes
    MyInitializeButtonBarSizes (pmButtonBarWindow, myInfo);

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
	EdInt_HaltEnvironment (IDS_MIO_BADWINDOWTYPE, __FILE__, __LINE__, 0,
			       myWindowType, WINDOW_TYPE_BEGINNER_TOP);
    }
    	
    myInfo = (ButtonBarWindowInfoPtr) GetWindowLong (pmButtonBarWindow, 
        WINDOW_MEM_WINDOW_INFO);
    if (myInfo == NULL)
    {
	EdInt_HaltEnvironment (IDS_MIO_GETWINDOWLONGFAIL, __FILE__, __LINE__,
	    		       GetLastError (), WINDOW_MEM_WINDOW_INFO);
    }
       
    return myInfo;
} // MyGetButtonBarWindowInfo


/************************************************************************/
/* MyGetButtonSize							*/
/************************************************************************/
static int	MyGetButtonSize (HWND pmButtonBarWindow, 
				 ButtonBarWindowInfoPtr pmInfo, 
				 int pmNumButtons, int *pmXPos)
{
    int		myMaxSize = 0;
    HDC		myDeviceContext;
    char	myButtonLabel [256];
    SIZE	myExtent;
    int		cnt;
    
    myDeviceContext = GetDC (pmInfo -> button [0]);
    SelectFont (myDeviceContext, stButtonFont);
    
    for (cnt = 0 ; cnt < pmNumButtons ; cnt++)
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

    // Also check for size of Resume
    EdInt_LoadString (IDS_MIO_RESUME_BUTTON, myButtonLabel, 
		      sizeof (myButtonLabel));
    GetTextExtentPoint32 (myDeviceContext, myButtonLabel, 
			  strlen (myButtonLabel), &myExtent);
    if (myExtent.cx > myMaxSize)
    {
	myMaxSize = myExtent.cx;
    }

    ReleaseDC (pmInfo -> button [0], myDeviceContext);

    myMaxSize += 2 * stButtonFontCharWidth;
    
    for (cnt = 0 ; cnt < pmNumButtons ; cnt++)
    {
    	*pmXPos += CTRL_GAP;
    	pmInfo -> buttonXPos [cnt] = *pmXPos;
        *pmXPos += myMaxSize;
        pmInfo -> buttonWidth [cnt] = myMaxSize;
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
    
    myXPos = FRAME_GAP;
    pmInfo -> areaXPos = FRAME_GAP;
    MyGetButtonSize (pmButtonBarWindow, pmInfo, pmInfo -> numButtons, &myXPos);
    pmInfo -> areaWidth = myXPos - pmInfo -> areaXPos;
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
    
    // Set the position of the three components
    myInfo = MyGetButtonBarWindowInfo (pmButtonBarWindow);

    for (cnt = 0 ; cnt < myInfo -> numButtons ; cnt++)
    {
	SetWindowPos (myInfo -> button [cnt], NULL, myInfo -> buttonXPos [cnt], 
	    myButtonTop, myInfo -> buttonWidth [cnt], myButtonHeight,
	    SWP_NOZORDER);
    	ShowWindow (myInfo -> button [cnt], TRUE);
    }    	
    
    SetWindowPos (myInfo -> raisedArea, NULL, myInfo -> areaXPos, 
	    myFrameTop, myInfo -> areaWidth, myFrameHeight, SWP_NOZORDER);
    	
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
    EdInt_LoadString (IDS_MIO_DIALOG_FONT_NAME, myFontName, sizeof (myFontName));

    // Delete any previously existing font
    if (stButtonFont != NULL)
    {
    	DeleteObject (stButtonFont);
    }
    
    // Create the new font
    stButtonFont = EdInt_CreateFont (pmButtonBarWindow, myFontName,
        stProperties.dialogFontSize, NO_BOLD, NO_ITALIC);
    if (stButtonFont == NULL)
    {
	EdInt_HaltEnvironment (IDS_MIO_CREATEFONTFAIL, __FILE__, __LINE__, 
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


