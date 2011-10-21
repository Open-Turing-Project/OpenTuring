/*****************/
/* mdiowintext.c */
/*****************/

/****************/
/* Self include */
/****************/
#include "mdiowintext.h"

/******************/
/* Other includes */
/******************/
#include "mio.h"

#include "miodialogs.h"

#include "miowin.h"
#include "mdiowin.h"
#include "miowintext.h"

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

// Used in MDIOWinText_DisplayText
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
static LRESULT CALLBACK MyTextWindowProcedure (HWND pmWindow, UINT pmMessage, 
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
static void		MyScrollHandlerHorz (HWND pmInnerWindow, 
					     int pmPart, int pmValue);
static void		MyScrollHandlerVert (HWND pmInnerWindow, 
					     int pmPart, int pmValue);

/***********************/
/* External procedures */
/***********************/
/************************************************************************/
/* MDIOWinText_Init							*/
/* 									*/
/* Initialize the modules under EdWin and register the new class	*/
/************************************************************************/
void	MDIOWinText_Init ()
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
    myWindowClass.lpfnWndProc =   MyTextWindowProcedure;
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
} // MDIOWinText_Init


/************************************************************************/
/* MDIOWinText_Finalize							*/
/************************************************************************/
void	MDIOWinText_Finalize (void)
{
} // MDIOWinText_Finalize


/************************************************************************/
/* MDIOWinText_CaptureMouse						*/
/************************************************************************/
void	MDIOWinText_CaptureMouse (WIND pmWindow)
{
    SetCapture ((HWND) pmWindow);
} // MDIOWinText_CaptureMouse


/******************************************************************************/
/* MDIOWinText_CopyToClipboard						      */
/******************************************************************************/
void	MDIOWinText_CopyToClipboard (WIND pmWindow, int pmLen, char *pmSrc, 
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
} // MDIOWinText_CopyToClipboard


/******************************************************************************/
/* MDIOWinText_CreateInnerWindow					      */
/******************************************************************************/
WIND	MDIOWinText_CreateInnerWindow (WIND pmWindow)
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
} // MDIOWinText_CreateInnerWindow


/************************************************************************/
/* MDIOWinText_CreateWindow						*/
/************************************************************************/
WIND	MDIOWinText_CreateWindow (WindowAttribPtr pmWindowAttribs)
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
} // MDIOWinText_CreateWindow

    			      
/******************************************************************************/
/* MDIOWinText_DisplayText						      */
/* 									      */
/* The text should be displayed in the specified colours.  The special line   */
/* should display with normal coloured text and a light grey background.  The */
/* special token should display in dark grey with white text.  The selection  */
/* appears above all others and appears in dark blue with inverted text.      */
/******************************************************************************/
void	MDIOWinText_DisplayText (HWND pmInnerOrOuterWindow, BOOL pmPaintUpdate, 
			      int pmFromLine, int pmToLine)
{
    MIOWinInfoPtr	myInfo;
    HWND		myInnerWindow;
    HDC			myDisplayContext;
    RECT		myLineRect, mySectionRect;
    TextPtr		myTextPtr;
    HBRUSH		mySelectionBrush;
    HBRUSH		myBackgroundBrush;
    int			myWidth, myHeight;
    int			myNumVisibleLines;
    int			myLastVisibleLine;
    int			myFirstLine, myLastLine;
    int			myLineNumber;
    int			mySelFromLine, mySelFromCol;
    int			mySelToLine, mySelToCol;
    BOOL		myEraseAtEnd = FALSE;
    BOOL		myEraseLastLine = FALSE;
    BOOL		myWindowActive;
    HRGN		mySelectionRegion = NULL;
    PAINTSTRUCT		myPaintStruct;
    
    // Get the window info    
    myInfo = MIOWin_GetInfo (pmInnerOrOuterWindow);
    myInnerWindow = myInfo -> innerWindow;

    // Create the appropriate brushes
    mySelectionBrush = CreateSolidBrush ((COLORREF) myInfo -> selectionColour);
    myBackgroundBrush = (HBRUSH) GetStockObject (WHITE_BRUSH);
    
    // Determine if the selection is drawn or framed
    {
	HWND	myForegroundWindow, myRunWindow, myNextWindow;
	
    	myForegroundWindow = GetForegroundWindow ();
    	myRunWindow = EdInt_GetTopWindow (myInnerWindow);
    	myNextWindow = EdInt_GetTopWindow (
    			GetNextWindow (myForegroundWindow, GW_HWNDNEXT));

    	if (myForegroundWindow == myRunWindow)
    	{
    	    myWindowActive = TRUE;
        }
        else
        {
    	    myWindowActive = FALSE;
        }
    }

    // Calculate the 
    myWidth = myInfo -> textRect.right - myInfo -> textRect.left;
    myHeight = myInfo -> textRect.bottom - myInfo -> textRect.top;
    
    // Translate it appropriately for the offscreen bitmap
    myLineRect.top = myInfo -> textRect.top;
    myLineRect.left = myInfo -> textRect.left;
    myLineRect.right = myInfo -> textRect.right;
    myLineRect.bottom = myLineRect.top + myInfo -> fontCharHeight;
    
    myNumVisibleLines = (myHeight + (myInfo -> fontCharHeight - 1)) / 
    				myInfo -> fontCharHeight;

    myTextPtr = (TextPtr) myInfo -> text;
    
    if (pmFromLine == ALL_LINES)
    {
    	pmFromLine = 0;
    	pmToLine = myTextPtr -> numLines;
    	myEraseAtEnd = TRUE;
    }
    else if (pmFromLine == myTextPtr -> numLines)
    {
    	myEraseLastLine = TRUE;
    }
    
    myLastVisibleLine = min (myInfo -> topLine + myNumVisibleLines, 
    	myTextPtr -> numLines);

    /***********************************************************************/    
    /* Draw a line of text in the correct colours.  The highlight line     */
    /* will be properly highlighted, but the hiliteToken and the selection */
    /* won't be drawn by this command.					   */
    /***********************************************************************/    
    myFirstLine = max (myInfo -> topLine, pmFromLine);
    myLastLine = min (myLastVisibleLine - 1, pmToLine);
    myLineRect.top += (myFirstLine - myInfo -> topLine) * 
    		      myInfo -> fontCharHeight;
    myLineRect.bottom += (myFirstLine - myInfo -> topLine) * 
		         myInfo -> fontCharHeight;
    
    // Get the selection's line and column
    EdText_GetSelectionLinesCols (myTextPtr, &mySelFromLine, &mySelFromCol, 
    	&mySelToLine, &mySelToCol);

    // Prepare for painting   
    if (pmPaintUpdate)
    {
        myDisplayContext = BeginPaint (myInnerWindow, &myPaintStruct);
    }        
    else
    {
        myDisplayContext = GetDC (myInnerWindow);
    }        
    HideCaret (myInnerWindow);

    // Set the font for the window    
    SelectFont (myDisplayContext, myInfo -> font);
    
    //
    // Loops once for each line to be drawn
    //
    for (myLineNumber = myFirstLine ; myLineNumber <= myLastLine ; 
         myLineNumber++)
    {
    	int	myStatus, myStartPos, myEndPos;
    	char	*myLine;
    	int	myLength;
    	int	mySelStart, mySelEnd;

	mySectionRect = myLineRect;
	
	//
	// Get the beginning and ending parts of the selection on this line
	//
	if ((mySelFromLine > myLineNumber) || 
	    (myTextPtr -> selFrom == myTextPtr -> selTo))
	{
	    // Selection starts after current line
	    mySelStart = NO_SEL_ON_THIS_LINE;
	}
	else if (mySelFromLine == myLineNumber)
	{
	    // Selection starts on current line
	    mySelStart = mySelFromCol;
	}
	else if (mySelFromLine < myLineNumber) 
	{
	    if (myLineNumber <= mySelToLine)
	    {
	    	// Selection starts before current line and ends on or
	    	// after current line
	    	mySelStart = 0;
	    }
	    else
	    {
	    	// Selection starts before current line and ends before
	    	// current line
	    	mySelStart = NO_SEL_ON_THIS_LINE;
	    }
	}
	if (myTextPtr -> selFrom == myTextPtr -> selTo)
	{
	    // Selection starts after current line
	    mySelEnd = NO_SEL_ON_THIS_LINE;
	}
	else if (myLineNumber < mySelToLine)
	{
	    if (mySelFromLine <= myLineNumber)
	    {
	    	// Selection ends after current line and starts on or
	    	// before current line
	    	mySelEnd = TO_END_OF_LINE;
	    }
	    else
	    {
	    	// Selection ends after current line and ends after 
	    	// current line
	    	mySelEnd = NO_SEL_ON_THIS_LINE;
	    }
	}
	else if (mySelToLine == myLineNumber)
	{
	    // Selection ends on current line
	    mySelEnd = mySelToCol;
	}
	else if (mySelToLine < myLineNumber) 
	{
	    // Selection ends before current line
	    mySelEnd = NO_SEL_ON_THIS_LINE;
	}
	
    	myStartPos = myInfo -> leftCol;
    	
    	myLength = myTextPtr -> lineStarts [myLineNumber + 1] - 
    	    myTextPtr -> lineStarts [myLineNumber] - 1;
    	    
    	myLine = &myTextPtr -> text [
    	    myTextPtr -> lineStarts [myLineNumber] + myStartPos];
    	
    	while (myStartPos < myLength)
    	{
	    //
	    // Determine what to draw and for how long
	    //
	    
    	    if (myWindowActive && (mySelStart != NO_SEL_ON_THIS_LINE) &&
    	        ((mySelStart <= myStartPos) && (myStartPos < mySelEnd)))
    	    {
    	    	myStatus = IN_SELECTION;
    	    	myEndPos = min (mySelEnd, myLength);
    	    }
    	    else
    	    {
    	    	myStatus = IN_NORMAL;
    	    	myEndPos = TO_END_OF_LINE;
    	        if (myWindowActive && 
    	            (mySelStart != NO_SEL_ON_THIS_LINE) &&
    	            (myStartPos < mySelStart))
    	        {
    	            myEndPos = mySelStart;
    	        }
    	    	myEndPos = min (myEndPos, myLength);
    	    }
    	    
    	    if (myStatus == IN_NORMAL)
    	    {
		SetTextColor (myDisplayContext, COLOUR_BLACK);
		SetBkColor (myDisplayContext, COLOUR_WHITE);
	    }
	    else
	    {
		SetTextColor (myDisplayContext, COLOUR_WHITE);
		SetBkColor (myDisplayContext, 
			    (COLORREF) myInfo -> selectionColour);
	    }
	    mySectionRect.right = mySectionRect.left + 
	        (myEndPos - myStartPos) * myInfo -> fontCharWidth;
	    ExtTextOut (myDisplayContext, mySectionRect.left, 
	        mySectionRect.top, ETO_CLIPPED | ETO_OPAQUE, &mySectionRect, 
    	        myLine, (myEndPos - myStartPos), NULL);
    	    mySectionRect.left = mySectionRect.right;
    	    myLine += myEndPos - myStartPos;
	    SetTextColor (myDisplayContext, COLOUR_BLACK);
    	    
    	    myStartPos = myEndPos;
    	} // while

	//
	// Draw the rest of the line
	//
	mySectionRect.right = myInfo -> textRect.right;
    	if (myWindowActive && (mySelEnd == TO_END_OF_LINE))
    	{
    	    FillRect (myDisplayContext, &mySectionRect, mySelectionBrush);
    	}
    	else
    	{
    	    FillRect (myDisplayContext, &mySectionRect, myBackgroundBrush);
    	}

	myLineRect.top += myInfo -> fontCharHeight;
	myLineRect.bottom += myInfo -> fontCharHeight;
    } // for 
    
    DeleteBrush (mySelectionBrush);
	
    // If there's a non-active selection, draw the frame
    if (!myWindowActive && (myTextPtr -> selFrom != myTextPtr -> selTo))
    {
    	RECT	myRect;
    	
    	if (mySelFromLine == mySelToLine)
    	{
    	    myRect.left = (mySelFromCol - myInfo -> leftCol) * 
    	        myInfo -> fontCharWidth + myInfo -> textRect.left;
    	    myRect.right = (mySelToCol - myInfo -> leftCol) * 
    	        myInfo -> fontCharWidth + myInfo -> textRect.left;
    	    myRect.top = (mySelFromLine - myInfo -> topLine) *
    	        myInfo -> fontCharHeight + myInfo -> textRect.top;
    	    myRect.bottom = myRect.top + myInfo -> fontCharHeight;
    	    FrameRect (myDisplayContext, &myRect, stBlackBrush);
    	}
    	else
    	{
    	    HRGN	mySelectionRegion, myTempRegion;
    	    
    	    // The top part of the selection
    	    myRect.left = (mySelFromCol - myInfo -> leftCol) * 
    	        myInfo -> fontCharWidth + myInfo -> textRect.left;
    	    myRect.right = myInfo -> textRect.right;
    	    myRect.top = (mySelFromLine - myInfo -> topLine) *
    	        myInfo -> fontCharHeight + myInfo -> textRect.top;
    	    myRect.bottom = myRect.top + myInfo -> fontCharHeight;
            mySelectionRegion = CreateRectRgnIndirect (&myRect);
	
	    // The middle section of the selection    	    
    	    myRect.left = myInfo -> textRect.left;
    	    myRect.right = myInfo -> textRect.right;
    	    myRect.top = ((mySelFromLine + 1) - myInfo -> topLine) *
    	        myInfo -> fontCharHeight + myInfo -> textRect.top;
    	    myRect.bottom = (mySelToLine - myInfo -> topLine) *
    	        myInfo -> fontCharHeight + myInfo -> textRect.top;
    	        
    	    myTempRegion = CreateRectRgnIndirect (&myRect);
    	    CombineRgn (mySelectionRegion, mySelectionRegion, myTempRegion, 
    	        RGN_OR);
    	   
    	    // The bottom line of the selection
    	    myRect.left = myInfo -> textRect.left;
    	    myRect.right = (mySelToCol - myInfo -> leftCol) * 
    	        myInfo -> fontCharWidth + myInfo -> textRect.left;
    	    myRect.top = (mySelToLine - myInfo -> topLine) *
    	        myInfo -> fontCharHeight + myInfo -> textRect.top;
    	    myRect.bottom = myRect.top + myInfo -> fontCharHeight;

    	    myTempRegion = CreateRectRgnIndirect (&myRect);
    	    CombineRgn (mySelectionRegion, mySelectionRegion, myTempRegion, 
    	        RGN_OR);
	
    	    FrameRgn (myDisplayContext, mySelectionRegion, stBlackBrush, 1, 1);
    	} // if
    	
  	// Set the drawing colours back to normal
	SetTextColor (myDisplayContext, COLOUR_BLACK);
	SetBkColor (myDisplayContext, COLOUR_WHITE);
    } // if non-active selection
    
    if (myEraseAtEnd)
    {
 	// Clear the rest of the window (TW)
    	if (myLineRect.top < myHeight)
    	{
	    myLineRect.bottom = myHeight;
	    FillRect (myDisplayContext, &myLineRect, 
	    	(HBRUSH) GetStockObject (WHITE_BRUSH));
    	}
    }
    else if (myEraseLastLine)
    {
    	// Needed when we are the last character in a line at the end of
    	// the file
	FillRect (myDisplayContext, &myLineRect, 
	    (HBRUSH) GetStockObject (WHITE_BRUSH));
    }
    
    // Clear the margins
    {
    	RECT	myRect;
    	
    	myRect.left = 0;
    	myRect.right = myInfo -> textRect.right;
    	myRect.top = 0;
    	myRect.bottom = 2;
	FillRect (myDisplayContext, &myRect, 
	    (HBRUSH) GetStockObject (WHITE_BRUSH));
	    
    	myRect.left = 0;
    	myRect.right = 2;
    	myRect.top = 0;
    	myRect.bottom = myInfo -> textRect.bottom;
	FillRect (myDisplayContext, &myRect, 
	    (HBRUSH) GetStockObject (WHITE_BRUSH));
    }
    	    
    ShowCaret (myInnerWindow);
    if (pmPaintUpdate)
    {
    	EndPaint (myInnerWindow, &myPaintStruct);
    }
    else
    {
    	ReleaseDC (myInnerWindow, myDisplayContext);
    }
} // MDIOWinText_DisplayText


/************************************************************************/
/* MDIOWinText_DisposeTextRecord					*/
/************************************************************************/
void	MDIOWinText_DisposeTextRecord (WIND pmWindow)
{
    if (MIOWin_GetInfo (pmWindow) -> text != NULL)
    {
    	EdText_Dispose (MIOWin_GetInfo (pmWindow) -> text);
    }
} // MDIOWinText_DisposeTextRecord


/************************************************************************/
/* MDIOWinText_SaveWindowToFile						*/
/************************************************************************/
void	MDIOWinText_SaveWindowToFile (WIND pmWindow, const char *pmPathName)
{
    BOOL		myNewPathName;
    char		myPathName [4096];
    MIOWinInfoPtr	myInfo;
    
    MIOWin_AssertWindowType (pmWindow, WINDOW_TYPE_MIO_TEXT);
    
    // Get the window information
    myInfo = MIOWin_GetInfo (pmWindow);

    if (pmPathName != NULL)
    {
	strcpy (myPathName, pmPathName);
    }
    else
    {
	myNewPathName = FALSE;
	
	strcpy (myPathName, MIO_programName);
	strcat (myPathName, " Output.txt");
	
	if (!EdInt_GetSaveFile ((HWND) pmWindow, myPathName))
	{
	    return;
	}
	
	if (strchr (myPathName, '.') == NULL)
	{
	    BOOL    myLowerCase = FALSE;
	    int	cnt;
	    
	    for (cnt = 0 ; cnt < (int) strlen (myPathName) ; cnt++)
	    {
		if (('a' <= myPathName [cnt]) && (myPathName [cnt] <= 'z'))
		{
		    myLowerCase = TRUE;
		    break;
		}
	    }
	    if (myLowerCase)
	    {
		strcat (myPathName, ".txt");
	    }
	    else
	    {
		strcat (myPathName, ".TXT");
	    }
	}    
    }

    // Save the text to the file.  If it fails, return immediately
    MySaveTextToFile (myPathName, myInfo -> text);
} // MDIOWinText_SaveWindowToFile


/************************************************************************/
/* MDIOWinText_InitializeWindow						*/
/************************************************************************/
BOOL	MDIOWinText_InitializeWindow (WIND pmWindow)
{
    MIOWinInfoPtr	myInfo = MIOWin_GetInfo (pmWindow);
    
    // Add the edit text
    myInfo -> text = EdText_Create ();
    if (myInfo -> text == NULL)
        return FALSE;

    EdText_InitializeTextRecord (myInfo -> text);  
    
    return TRUE;
} // MDIOWinText_InitializeWindow


/************************************************************************/
/* MDIOWinText_SetScrollTimer						*/
/************************************************************************/
void	MDIOWinText_SetScrollTimer (WIND pmWindow)
{
    MIOWin_AssertWindowType (pmWindow, WINDOW_TYPE_MIO_TEXT_INNER);
    
    SetTimer ((HWND) pmWindow, SCROLL_TIMER, 1, NULL);
} // MDIOWinText_SetScrollTimer


/************************************************************************/
/* MDIOWinText_SynchScrollBarsToText			      		*/
/*						      			*/
/* Set the scroll bars to correspond with the current top-most line and	*/
/* left-most column.  Also change the size of the scroll bars to 	*/
/* reflect the number of lines and the longest line.			*/
/************************************************************************/
void	MDIOWinText_SynchScrollBarsToText (HWND pmInnerOrOuterWindow)
{
    MIOWinInfoPtr	myInfo;
    HWND		myInnerWindow;
    TextPtr		myText;
    SCROLLINFO		myScrollInfo;
    int			myMaxLine, myMaxCol;

    // Get the window information
    myInfo = MIOWin_GetInfo (pmInnerOrOuterWindow);
    myInnerWindow = myInfo -> innerWindow;
    
    myText = myInfo -> text;
    
    // Check to see if we've increased the size of the window
    // and we move the window down.
    myMaxLine = max (0, myText -> numLines - (myInfo -> windowHeight - 2));
    myInfo -> topLine = max (0, min (myInfo -> topLine, myMaxLine));
    myMaxCol = max (0, myText -> longestLineLen - (myInfo -> windowWidth - 2));
    myInfo -> leftCol = max (0, min (myInfo -> leftCol, myMaxCol));

    myScrollInfo.cbSize = sizeof (SCROLLINFO);
    myScrollInfo.fMask = SIF_DISABLENOSCROLL | SIF_PAGE | SIF_POS | SIF_RANGE;
    myScrollInfo.nMin = 0;
    myScrollInfo.nMax = myText -> numLines + 1;
    myScrollInfo.nPage = myInfo -> windowHeight;
    myScrollInfo.nPos = myInfo -> topLine;
    
    SetScrollInfo (myInnerWindow, SB_VERT, &myScrollInfo, TRUE);

    myScrollInfo.cbSize = sizeof (SCROLLINFO);
    myScrollInfo.fMask = SIF_DISABLENOSCROLL | SIF_PAGE | SIF_POS | SIF_RANGE;
    myScrollInfo.nMin = 0;
    myScrollInfo.nMax = myText -> longestLineLen + 1;
    myScrollInfo.nPage = myInfo -> windowWidth;
    myScrollInfo.nPos = myInfo -> leftCol;
    
    SetScrollInfo (myInnerWindow, SB_HORZ, &myScrollInfo, TRUE);
} // MDIOWinText_SynchScrollBarsToText


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
            MDIOWinText_DisplayText (pmInnerWindow, TRUE, ALL_LINES, ALL_LINES);
            return 0;
        
	case WM_SIZE:
	    MDIOWin_ResizeInnerWindow (pmInnerWindow);
	    return 0;
	    
	//
	// Scroll bar handling
	//
	case WM_HSCROLL:
	    MyScrollHandlerHorz (pmInnerWindow, LOWORD (pmWParam), 
	    	HIWORD (pmWParam));
	    return 0;
	    
	case WM_VSCROLL:
	    MyScrollHandlerVert (pmInnerWindow, LOWORD (pmWParam), 
	        HIWORD (pmWParam));
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

	//
	// Mouse handling
	//
	case WM_MOUSEMOVE:
	    myInfo = MIOWin_GetInfo (pmInnerWindow);
	    if (myInfo -> selectingWithMouse)
	    {
	    	if (MIOWinText_MouseDownHandler (pmInnerWindow, 
	    	        GET_X_LPARAM (pmLParam), GET_Y_LPARAM (pmLParam), 
	    	        TRUE, FALSE))
                {
            	    myInfo -> verticalTicks = 0;
            	    myInfo -> horizontalTicks = 0;
                }
	    }
            return 0;

        case WM_LBUTTONDOWN:
            // Returns true if mouse down in text region
            if (MIOWinText_ButtonPressHandler (pmInnerWindow, 
                    GET_X_LPARAM (pmLParam), GET_Y_LPARAM (pmLParam), 
                    pmWParam & MK_SHIFT))
            {
            	return 0;
            }
            break;
                    
        case WM_LBUTTONUP:
	    myInfo = MIOWin_GetInfo (pmInnerWindow);
    
            if (myInfo -> selectingWithMouse)
            {
            	MIOWinText_MouseDownHandler (pmInnerWindow, 
            	    GET_X_LPARAM (pmLParam), GET_Y_LPARAM (pmLParam), 
            	    TRUE, FALSE);
            	ReleaseCapture ();
            	KillTimer (pmInnerWindow, SCROLL_TIMER);
            	myInfo -> selectingWithMouse = FALSE;
            	return 0;
            }
            	    
	case WM_TIMER:
	    if (pmWParam == SCROLL_TIMER)
	    {
	    	// If the mouse is currently in the window (vertically), 
	    	// reset the timer to 0. Otherwise compare the accumulated
	    	// time to the distance from the top or bottom of the text
	    	// window and scroll if enough time has passed.  Scroll faster
	    	// if the mouse is farther away from the window.
	    	POINT	myPoint;
	    	MYRECT	myRect;
	    	
	    	myInfo = MIOWin_GetInfo (pmInnerWindow);
	    	
                myRect = myInfo -> textRect;
                
	    	myInfo -> verticalTicks++;
	    	myInfo -> horizontalTicks++;
	    	
	    	GetCursorPos (&myPoint);
	    	ScreenToClient (pmInnerWindow, &myPoint);
	    	
	    	if (myPoint.x < myRect.left)
	    	{
	    	    while (myInfo -> horizontalTicks > HORIZ_TICKS_PER_SCROLL)
	    	    {
	    	    	MyScrollHandlerHorz (pmInnerWindow, 
	    	    	    SB_LINELEFT, 0);
	    	    	myInfo -> horizontalTicks -= HORIZ_TICKS_PER_SCROLL;
	    	    }
	    	    MIOWinText_MouseDownHandler (pmInnerWindow, myPoint.x, 
	    	    	myPoint.y, TRUE, TRUE);
	    	}
	    	else if (myPoint.x > myRect.right)
	    	{
	    	    while (myInfo -> horizontalTicks > HORIZ_TICKS_PER_SCROLL)
	    	    {
	    	    	MyScrollHandlerHorz (pmInnerWindow, 
	    	    	    SB_LINERIGHT, 0);
	    	    	myInfo -> horizontalTicks -= HORIZ_TICKS_PER_SCROLL;
	    	    }
	    	    MIOWinText_MouseDownHandler (pmInnerWindow, myPoint.x, 
	    	        myPoint.y, TRUE, TRUE);
	    	}
	    	else
	    	{
	    	    myInfo -> horizontalTicks = 0;
	    	}
	    	
	    	if (myPoint.y < myRect.top)
	    	{
	    	    int ticks = max (1, 6 - ((myRect.top - myPoint.y) / 
	    	        myInfo -> fontCharHeight));
	    	    while (myInfo -> verticalTicks > ticks)
	    	    {
	    	    	MyScrollHandlerVert (pmInnerWindow, SB_LINEUP, 0);
	    	    	myInfo -> verticalTicks -= ticks;
	    	    }
	    	    MIOWinText_MouseDownHandler (pmInnerWindow, myPoint.x, 
	    	        myPoint.y, TRUE, TRUE);
	    	}
	    	else if (myPoint.y > myRect.bottom)
	    	{
	    	    int ticks = max (1, 6 - ((myPoint.y - myRect.bottom) / 
	    	        myInfo -> fontCharHeight));
	    	    while (myInfo -> verticalTicks > ticks)
	    	    {
	    	    	MyScrollHandlerVert (pmInnerWindow, 
	    	    	    SB_LINEDOWN, 0);
	    	    	myInfo -> verticalTicks -= ticks;
	    	    }
	    	    MIOWinText_MouseDownHandler (pmInnerWindow, myPoint.x,
	    	        myPoint.y, TRUE, TRUE);
	    	}
	    	else
	    	{
	    	    myInfo -> verticalTicks = 0;
	    	}
	    }
	    return 0;
	    
	case WM_MOUSEWHEEL:
	    if (MDIOWin_GetWheelMouseDeltaPerLine () != 0)
	    {
	    	myInfo = MIOWin_GetInfo (pmInnerWindow);
	    	
	        myInfo -> accumulateDelta += (short) HIWORD (pmWParam);
	        while (myInfo -> accumulateDelta >= MDIOWin_GetWheelMouseDeltaPerLine ())
	    	{
	    	    MyScrollHandlerVert (pmInnerWindow, SB_LINEUP, 0);
	    	    myInfo -> accumulateDelta -= MDIOWin_GetWheelMouseDeltaPerLine ();
	    	}
	    	while (myInfo -> accumulateDelta <= -MDIOWin_GetWheelMouseDeltaPerLine ())
	    	{
	    	    MyScrollHandlerVert (pmInnerWindow, SB_LINEDOWN, 0);
	    	    myInfo -> accumulateDelta += MDIOWin_GetWheelMouseDeltaPerLine ();
	    	}
	    	return 0;
	    }
	    break;
	    
	//
	// Focus handling
	//
        case WM_SETFOCUS:
	    MDIOWin_CreateDestroyCaret (pmInnerWindow, CREATE_CARET);
	    MDIOWinText_DisplayText (pmInnerWindow, FALSE, ALL_LINES, 
				     ALL_LINES);
            return 0;

	case WM_KILLFOCUS:
	    MDIOWin_CreateDestroyCaret (pmInnerWindow, DESTROY_CARET);
            MDIOWinText_DisplayText (pmInnerWindow, FALSE, ALL_LINES, 
            			     ALL_LINES);
	    return 0;
	    
	 default:
	    if ((pmMessage == stMouseWheelMessage) && 
	    	(MDIOWin_GetWheelMouseDeltaPerLine () != 0))
	    {
	    	myInfo = MIOWin_GetInfo (pmInnerWindow);
	    	
	        myInfo -> accumulateDelta += (short) HIWORD (pmWParam);
	        while (myInfo -> accumulateDelta >= MDIOWin_GetWheelMouseDeltaPerLine ())
	    	{
	    	    MyScrollHandlerVert (pmInnerWindow, SB_LINEUP, 0);
	    	    myInfo -> accumulateDelta -= MDIOWin_GetWheelMouseDeltaPerLine ();
	    	}
	    	while (myInfo -> accumulateDelta <= -MDIOWin_GetWheelMouseDeltaPerLine ())
	    	{
	    	    MyScrollHandlerVert (pmInnerWindow, SB_LINEDOWN, 0);
	    	    myInfo -> accumulateDelta += MDIOWin_GetWheelMouseDeltaPerLine ();
	    	}
	    	return 0;
            }
    }
    
    return DefWindowProc (pmInnerWindow, pmMessage, pmWParam, 
        		  pmLParam);
} // MyTextInnerWindowProcedure


/************************************************************************/
/* MyTextWindowProcedure						*/
/*									*/
/* Callback procedure for the main editor window.			*/
/************************************************************************/
static LRESULT CALLBACK MyTextWindowProcedure (HWND pmWindow, UINT pmMessage, 
    					       WPARAM pmWParam, LPARAM pmLParam)
{
    // Last parameter is pmIsGraphicsWindow = FALSE
    return MDIOWin_WindowProcedure (pmWindow, pmMessage, pmWParam, 
				    pmLParam, FALSE);
} // MyTextWindowProcedure


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


/************************************************************************/
/* MySaveTextToFile							*/
/************************************************************************/
static BOOL	MySaveTextToFile (char *pmPathName, TextPtr pmText)
{
//    ErrorBuffer	myErrorBuffer;
    HANDLE	myFile;
    int		myPos, myEOLPos;
    DWORD	myNumBytesWritten;
    
    // Open the file
    myFile = CreateFile (pmPathName, GENERIC_WRITE, 0, NULL,
    	CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (myFile == (HANDLE) INVALID_HANDLE_VALUE)
    {
/*    	EdGUI_Message (
    	    "File Open Error",
    	    "Unable to open file \"%s\".\n\n%s\n[Error code: %d]",
    	    pmPathName, EdFail_GetErrorMessage (myErrorBuffer), 
    	    GetLastError ());
*/
	return FALSE;
    }

    // Write the file
    myPos = 0;
    while (myPos < pmText -> textLen)
    {
	// Write out the rest of the line
	myEOLPos = myPos;
    	while ((myEOLPos < pmText -> textLen) && 
    	       (pmText -> text [myEOLPos] != '\n'))
    	{
    	    myEOLPos++;
    	}
    	if (myEOLPos != myPos)
    	{
    	    // Write out the rest of the line
    	    if (!WriteFile (myFile, &pmText -> text [myPos], myEOLPos - myPos,
    	        &myNumBytesWritten, NULL))
    	    {
    	    	// TW
    	    }
    	    if (myNumBytesWritten != (DWORD) (myEOLPos - myPos))
    	    {
    	    	// TW
	    }
    	    myPos = myEOLPos;
	}
	
	// Write out the newlines
    	while ((myPos < pmText -> textLen) && (pmText -> text [myPos] == '\n'))
    	{
	    myPos++;
    	    if (!WriteFile (myFile, NEW_LINE, strlen (NEW_LINE), 
    	        	    &myNumBytesWritten, NULL))
    	    {
    	    	// TW
    	    }
    	    if (myNumBytesWritten != strlen (NEW_LINE))
    	    {
    	    	// TW
	    }
	}
    } // while (myPos < pmText -> textLen)

    // Write the file    
    if (CloseHandle (myFile) == 0)
    {
	// TW
	return FALSE;
    }
    
    return TRUE;
} // MySaveTextToFile


/************************************************************************/
/* MyScrollHandlerHorz							*/
/************************************************************************/
static void	MyScrollHandlerHorz (HWND pmInnerWindow, int pmPart, 
				     int pmValue)
{
    MIOWinInfoPtr	myInfo;
    TextPtr		myTextPtr;
    int			myWidth;
    int			myMaxCol;
    int 		myNewLeftCol;
        
    MIOWin_AssertWindowType (pmInnerWindow, WINDOW_TYPE_MIO_TEXT_INNER);
    
    myInfo = MIOWin_GetInfo (pmInnerWindow);
    myTextPtr = (TextPtr) myInfo -> text;
    myWidth = myInfo -> windowWidth;
    myMaxCol = max (0, myTextPtr -> longestLineLen - (myWidth - 2));
    
    switch (pmPart)
    {
    	case SB_PAGERIGHT:
    	    myNewLeftCol = myInfo -> leftCol + (myWidth - 2);
    	    break;
    	case SB_LINERIGHT:
    	    myNewLeftCol = myInfo -> leftCol + 1;
    	    break;
    	case SB_PAGELEFT:
    	    myNewLeftCol = myInfo -> leftCol - (myWidth - 2);
    	    break;
    	case SB_LINELEFT:
    	    myNewLeftCol = myInfo -> leftCol - 1;
    	    break;
    	case SB_LEFT:
    	    myNewLeftCol = 0;
    	    break;
    	case SB_RIGHT:
    	    myNewLeftCol = myMaxCol;
    	    break;
    	case SB_THUMBPOSITION:
	    myNewLeftCol = pmValue;    	    
    	    break;
    	case SB_THUMBTRACK:
    	    myNewLeftCol = pmValue;
    	    break;
    	default:
    	    return;
    } // switch
    
    myNewLeftCol = max (0, min (myNewLeftCol, myMaxCol));
    
    if (myNewLeftCol == myInfo -> leftCol)
        return;
    
    myInfo -> leftCol = myNewLeftCol;
    SetScrollPos (pmInnerWindow, SB_HORZ, myNewLeftCol, TRUE);
    MIOWin_CaretMove (pmInnerWindow);    
    MDIOWinText_DisplayText (pmInnerWindow, FALSE, ALL_LINES, ALL_LINES);
} // MyScrollHandlerHorz


/************************************************************************/
/* MyScrollHandlerVert							*/
/************************************************************************/
static void	MyScrollHandlerVert (HWND pmInnerWindow, int pmPart, 
				     int pmValue)
{
    MIOWinInfoPtr	myInfo;
    TextPtr		myTextPtr;
    int			myHeight;
    int			myMaxLine;
    int 		myNewTopLine;
        
    MIOWin_AssertWindowType (pmInnerWindow, WINDOW_TYPE_MIO_TEXT_INNER);
    
    myInfo = MIOWin_GetInfo (pmInnerWindow);
    myTextPtr = (TextPtr) myInfo -> text;
    myHeight = myInfo -> windowHeight;
    myMaxLine = max (0, myTextPtr -> numLines - (myHeight - 2));
        
    switch (pmPart)
    {
    	case SB_PAGEDOWN:
    	    myNewTopLine = myInfo -> topLine + (myHeight - 2);
    	    break;
    	case SB_LINEDOWN:
    	    myNewTopLine = myInfo -> topLine + 1;
    	    break;
    	case SB_PAGEUP:
    	    myNewTopLine = myInfo -> topLine - (myHeight - 2);
    	    break;
    	case SB_LINEUP:
    	    myNewTopLine = myInfo -> topLine - 1;
    	    break;
    	case SB_TOP:
    	    myNewTopLine = 0;
    	    break;
    	case SB_BOTTOM:
    	    myNewTopLine = myMaxLine;
    	    break;
    	case SB_THUMBPOSITION:
	    myNewTopLine = pmValue;    	    
    	    break;
    	case SB_THUMBTRACK:
    	    myNewTopLine = pmValue;
    	    break;
    	default:
    	    return;
    } // switch
    
    myNewTopLine = max (0, min (myNewTopLine, myMaxLine));
    
    if (myNewTopLine == myInfo -> topLine)
        return;
    
    myInfo -> topLine = myNewTopLine;
    SetScrollPos (pmInnerWindow, SB_VERT, myNewTopLine, TRUE);
    MIOWin_CaretMove (pmInnerWindow);    
    MDIOWinText_DisplayText (pmInnerWindow, FALSE, ALL_LINES, ALL_LINES);
} // MyScrollHandlerVert




