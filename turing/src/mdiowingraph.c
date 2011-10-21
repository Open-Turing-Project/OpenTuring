/******************/
/* mdiowingraph.c */
/******************/

/****************/
/* Self include */
/****************/
#include "mdiowingraph.h"

/******************/
/* Other includes */
/******************/
#include "mio.h"

#include "miodialogs.h"

#include "miowin.h"
#include "mdiowin.h"

#include "miosprite.h"

#include "edint.h"

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
#include <winuser.h>
//#include <crtdbg.h>

/**********/
/* Macros */
/**********/
#define MIN(x,y)		((x) < (y) ? (x) : (y))
#define MAX(x,y)		((x) > (y) ? (x) : (y))

/*************/
/* Constants */
/*************/
// The foreground/background colour indices
#define FG_COLOUR		 7
#define BG_COLOUR		 0

// The overlap between scrolling the graphics window by a page
#define PIXEL_OVERLAP		20

// Used for by the mouse EdInt_GetMouseWheelSettings
#ifndef WM_MOUSEWHEEL
// Message ID for IntelliMouse wheel
#define WM_MOUSEWHEEL WM_MOUSELAST+1 
#endif // !WM_MOUSEWHEEL

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
static int stTuringVirtualKeycode [256] =
{
      0,   0,   0,   0,   0,   0,   0,   0,   8,   9,   0,   0, 183,  10,   0,   0, 
    180, 181, 182,   0,   0,   0,   0,   0,   0,   0,   0,  27,   0,   0,   0,   0,
     32, 201, 209, 207, 199, 203, 200, 205, 208,   0,   0,   0,   0, 210, 211,   0,
     48,  49,  50,  51,  52,  53,  54,  55,  56,  57,   0,   0,   0,   0,   0,   0, 
      0,  97,  98,  99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
    112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122,   0,   0,   0,   0,   0,
     48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  42,  43,   0,  45,  46,  47,
    187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 133, 134,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  59,  61,  44,  45,  46,  47,
     96,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  91,  92,  93,  39,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
} ;


/********************/
/* Static variables */
/********************/
// Used by the wheel mouse
static int	stWheelMouseDeltaPerLine;
static UINT	stMouseWheelMessage;
static HPALETTE	stDefaultPalette;
static int	stDefaultPaletteSize;
// The contextual menu
static HMENU	stContextualMenu;

/******************************/
/* Static callback procedures */
/******************************/
static LRESULT CALLBACK MyGraphicsInnerWindowProcedure (
						HWND pmGraphicsDisplayWindow, 
						UINT pmMessage, 
    						WPARAM pmWParam, 
    						LPARAM pmLParam);
static LRESULT CALLBACK MyGraphicsWindowProcedure (
					      HWND pmWindow, UINT pmMessage, 
					      WPARAM pmWParam, 
					      LPARAM pmLParam);


/*********************/
/* Static procedures */
/*********************/
static PBITMAPINFO	MyCreateBitmapInfo (HBITMAP pmBitMap);
static BOOL		MyCreateDefaultPalette (void);
static void		MyDeleteText (HWND pmRunWindow, int pmRow, int pmCol);
static void		MyDisplayGraphics (HWND pmRunGraphicsWindow, 
					   BOOL pmPaintUpdate);
static int		MyInitializeGraphicsInnerWindow (HWND pmGraphicsDisplayWindow);
static int		MyInitializeGraphicsWindow (HWND pmRunWindow, 
					   WindowAttribPtr pmWindowAttribs);
static BOOL		MySaveGraphicsToFile (HWND pmWindow, 
					      const char *pmPathName);
static void		MyScrollHandlerHorz (HWND pmRunGraphicsWindow,
					     int pmPart, int pmValue);
static void		MyScrollHandlerVert (HWND pmRunGraphicsWindow,
					     int pmPart, int pmValue);
static void		MySetClipRegion (MIOWinInfoPtr myInfo);
static void		MySetOnscreenClipRegion (MIOWinInfoPtr myInfo);
static void		MySetRect (RECT *pmRect, OOTint pmX1, OOTint pmY1, 
				   OOTint pmX2, OOTint pmY2);


/***********************/
/* External procedures */
/***********************/
/************************************************************************/
/* MDIOWinGraph_Init							*/
/* 									*/
/* Initialize the modules under EdWin and register the new class	*/
/************************************************************************/
BOOL	MDIOWinGraph_Init ()
{
    char	myGraphicsRunWindowClassName [256];
    char	myGraphicsChildRunWindowClassName [256];
    int		myResult;
    WNDCLASSEX	myWindowClass;

    //
    // Initialize the palette
    //
    if (!MyCreateDefaultPalette ())
    {
    	return FALSE;
    }

    // Initialize the wheel mouse
    stWheelMouseDeltaPerLine = EdInt_GetMouseWheelSettings ();
    stMouseWheelMessage = EdInt_GetMouseWheelMessage ();
    
    // Get the class name
    EdInt_LoadString (IDS_MIOGRAPHICS_WINDOW_NAME, myGraphicsRunWindowClassName, 
	sizeof (myGraphicsRunWindowClassName));

    //
    // Register the console window class
    //
    myWindowClass.cbSize = 	  sizeof (myWindowClass);
    // Set window class to redraw when window size changes
    myWindowClass.style = 	  CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    // Procedure to be called with messages for this window class
    myWindowClass.lpfnWndProc =   MyGraphicsWindowProcedure;
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
    myWindowClass.lpszClassName = myGraphicsRunWindowClassName; 
    // Set the icon for this class.
    myWindowClass.hIconSm = 	  LoadIcon (MIO_applicationInstance, 
    					MAKEINTRESOURCE (MIO_RUN_SMALL_ICON));
    
    myResult = RegisterClassEx (&myWindowClass);
    if (myResult == 0)
    {
    	EdInt_FailWarn (IDS_MIO_REGISTERCLASSFAIL, __FILE__, __LINE__, 
    			GetLastError ());
	return FALSE;    			  
    }
    
    // Get the class name
    EdInt_LoadString (IDS_MIOGRAPHICS_INNER_WINDOW_NAME, 
    	myGraphicsChildRunWindowClassName, 
    	sizeof (myGraphicsChildRunWindowClassName));
    
    //
    // Register the text display window class
    //
    myWindowClass.cbSize = 	  sizeof (myWindowClass);
    // Set window class to redraw when window size changes
    myWindowClass.style = 	  CS_HREDRAW | CS_VREDRAW;
    // Procedure to be called with messages for this window class
    myWindowClass.lpfnWndProc =   MyGraphicsInnerWindowProcedure;
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
    myWindowClass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH);
    // Set the menu for this window class
    myWindowClass.lpszMenuName =  NULL;
    // Name of the window class
    myWindowClass.lpszClassName = myGraphicsChildRunWindowClassName; 
    // Set the icon for this class.
    myWindowClass.hIconSm = 	  NULL;
    
    myResult = RegisterClassEx (&myWindowClass);
    if (myResult == 0)
    {
    	EdInt_FailWarn (IDS_MIO_REGISTERCLASSFAIL, __FILE__, __LINE__, 
    		        GetLastError ());
	return FALSE;    			  
    }

    // Load the contextual menu
    stContextualMenu = LoadMenu (MIO_applicationInstance, 
		MAKEINTRESOURCE (MIO_GRAPHICS_CONTEXTUAL_MENU));
    stContextualMenu = GetSubMenu (stContextualMenu, 0);

    return TRUE;
} // MDIOWinGraph_Init


/************************************************************************/
/* MDIOWinGraph_Finalize						*/
/************************************************************************/
void	MDIOWinGraph_Finalize (void)
{
    // This finalization *must* take place in either MIOText_Finalize or
    // MDIOWinGraph_Finalize.  In this case, it takes place in MIOText_Finalize.   
    // MDIOWinTop_Finalize ();
} // MDIOWinGraph_Finalize


/************************************************************************/
/* MDIOWinGraph_ClearText						*/
/************************************************************************/
void	MDIOWinGraph_ClearText (WIND pmWindow, int pmRow, int pmFromCol, 
				int pmToCol)
{
    MIOWinInfoPtr	myInfo = MIOWin_GetInfo (pmWindow);
    RECT		myRect;
    HBRUSH		myBGColourBrush;
    
    myRect.top = pmRow * myInfo -> fontCharHeight;
    myRect.bottom = myRect.top + myInfo -> fontCharHeight;
    myRect.left = pmFromCol * myInfo -> fontCharWidth;
    myRect.right = pmToCol * myInfo -> fontCharWidth;
    
    // Set the font, foreground and background colour for the offscreen
    myBGColourBrush = CreateSolidBrush (PALETTEINDEX (myInfo -> textBGColour));

    // Draw it offscreen    		
    FillRect (myInfo -> offscreenDeviceContext, &myRect, myBGColourBrush);
	        
    // Draw it to the window
    if (myInfo -> displayOnScreen)
    {
	HideCaret (myInfo -> innerWindow);
	FillRect (myInfo -> deviceContext, &myRect, myBGColourBrush);
	ShowCaret (myInfo -> innerWindow);
    }
    
    DeleteObject (myBGColourBrush);
} // MDIOWinGraph_ClearText


/************************************************************************/
/* MDIOWinGraph_ClearToEOL						*/
/************************************************************************/
void	MDIOWinGraph_ClearToEOL (WIND pmWindow, int pmRow, int pmCol)
{
    MIOWinInfoPtr	myInfo = MIOWin_GetInfo (pmWindow);
    RECT		myRect;
    HBRUSH		myBGColourBrush;
    
    myRect.top = pmRow * myInfo -> fontCharHeight;
    myRect.bottom = myRect.top + myInfo -> fontCharHeight;
    myRect.left = pmCol * myInfo -> fontCharWidth;
    myRect.right = myInfo -> width;
    
    // Set the font, foreground and background colour for the offscreen
    myBGColourBrush = CreateSolidBrush (PALETTEINDEX (myInfo -> textBGColour));

    // Draw it offscreen    		
    FillRect (myInfo -> offscreenDeviceContext, &myRect, myBGColourBrush);
	        
    // Draw it to the window
    if (myInfo -> displayOnScreen)
    {
	HideCaret (myInfo -> innerWindow);
	FillRect (myInfo -> deviceContext, &myRect, myBGColourBrush);
	ShowCaret (myInfo -> innerWindow);
    }

    DeleteObject (myBGColourBrush);
} // MDIOWinGraph_ClearToEOL


/************************************************************************/
/* MDIOWinGraph_ClipAdd							*/
/************************************************************************/
void	MDIOWinGraph_ClipAdd (WIND pmWindow, OOTint pmX1, OOTint pmY1,
			      OOTint pmX2, OOTint pmY2)
{
    MIOWinInfoPtr	myInfo = MIOWin_GetInfo (pmWindow);
    HRGN 		myNewRegion;
    RECT		myRect;

    if (myInfo -> clippingRegion == NULL)
    {
    	MDIOWinGraph_ClipSet (pmWindow, pmX1, pmY1, pmX2, pmY2);
    	return;
    }
        
    MySetRect (&myRect, pmX1, pmY1, pmX2, pmY2);
    
    myNewRegion = CreateRectRgnIndirect (&myRect);

    CombineRgn (myInfo -> clippingRegion, myInfo -> clippingRegion, myNewRegion,
    		RGN_OR);
    DeleteObject (myNewRegion);

    MySetClipRegion (myInfo);
} // MDIOWinGraph_ClipAdd


/************************************************************************/
/* MDIOWinGraph_ClipAddOval						*/
/************************************************************************/
void	MDIOWinGraph_ClipAddOval (WIND pmWindow, OOTint pmX, OOTint pmY,
			          OOTint pmXR, OOTint pmYR)
{
    MIOWinInfoPtr	myInfo = MIOWin_GetInfo (pmWindow);
    HRGN 		myNewRegion;

    myNewRegion = CreateEllipticRgn (pmX - pmXR, pmY - pmYR, pmX + pmXR,
    					 pmY + pmYR);

    if (myInfo -> clippingRegion == NULL)
    {
    	myInfo -> clippingRegion = myNewRegion;
    
    	MySetClipRegion (myInfo);
    	
    	return;
    }
        
    CombineRgn (myInfo -> clippingRegion, myInfo -> clippingRegion, myNewRegion,
    		RGN_OR);
    DeleteObject (myNewRegion);

    MySetClipRegion (myInfo);
} // MDIOWinGraph_ClipAddOval


/************************************************************************/
/* MDIOWinGraph_ClipClear						*/
/************************************************************************/
void	MDIOWinGraph_ClipClear (WIND pmWindow)
{
    MIOWinInfoPtr	myInfo = MIOWin_GetInfo (pmWindow);
    
    DeleteObject (myInfo -> clippingRegion);
    myInfo -> clippingRegion = NULL;
    
    MySetClipRegion (myInfo);
} // MDIOWinGraph_ClipClear


/************************************************************************/
/* MDIOWinGraph_ClipSet							*/
/************************************************************************/
void	MDIOWinGraph_ClipSet (WIND pmWindow, OOTint pmX1, OOTint pmY1,
			      OOTint pmX2, OOTint pmY2)
{
    MIOWinInfoPtr	myInfo = MIOWin_GetInfo (pmWindow);
    RECT		myRect;
    
    MySetRect (&myRect, pmX1, pmY1, pmX2, pmY2);
    
    if (myInfo -> clippingRegion != NULL) 
    {
    	DeleteObject (myInfo -> clippingRegion);
    }
    myInfo -> clippingRegion = CreateRectRgnIndirect (&myRect);
    
    MySetClipRegion (myInfo);
} // MDIOWinGraph_ClipSet


/************************************************************************/
/* MDIOWinGraph_CreateInnerWindow					*/
/************************************************************************/
WIND	MDIOWinGraph_CreateInnerWindow (WIND pmWindow)
{
    HINSTANCE		myApplicationInstance;
    char		myClassName [256];
    HWND		myInnerWindow;
    
    // Get the program instance
    myApplicationInstance = 
        (HINSTANCE) GetWindowLong (pmWindow, GWL_HINSTANCE);
        
    // Create the console text window
    EdInt_LoadString (IDS_MIOGRAPHICS_INNER_WINDOW_NAME, myClassName,
    	sizeof (myClassName));

    myInnerWindow = CreateWindow (
    	myClassName, 			// Window class
        NULL,	 			// Window title
        WS_CHILDWINDOW | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL,	// Window style
    	0, 0, 0, 0, 			// Initial location & size
    	pmWindow, 			// Parent window handle
    	NULL, 				// Window menu handle
    	myApplicationInstance,	 	// Program instance handle
    	NULL);				// Creation parameters
    
    if (MIOWin_GetProperties () -> logWindowMessages)
    {
//    	EdLog_Log ("Text Display Window = %x  (Finished Creation)", 
//    	    myInnerWindow);
    }

    return myInnerWindow;
} // MDIOWinGraph_CreateInnerWindow


/************************************************************************/
/* MDIOWinGraph_CreateWindow						*/
/************************************************************************/
WIND	MDIOWinGraph_CreateWindow (WindowAttribPtr pmWindowAttribs)
{
    char		myRunWindowClassName [256];
    HWND		myRunWindow;
    MIOWinInfoPtr	myInfo;
    HPEN		myPen;
    HBRUSH		myBrush;
        
    // Load the class name (we've already loaded it once, so no error checking)    
    EdInt_LoadString (IDS_MIOGRAPHICS_WINDOW_NAME,
	myRunWindowClassName, sizeof (myRunWindowClassName));

    // Create the editor window    
    myRunWindow = CreateWindow (
    	myRunWindowClassName, 			// Window class
        "Run Windows",		 		// Window title
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

    //
    // Clear the screen in colour 0
    //
    myInfo = MIOWin_GetInfo (myRunWindow);
    myPen = CreatePen (0, 1, PALETTEINDEX (0));
    myBrush = CreateSolidBrush (PALETTEINDEX (0));
    SelectBrush (myInfo -> offscreenDeviceContext, myPen);
    SelectBrush (myInfo -> offscreenDeviceContext, myBrush);
    Rectangle (myInfo -> offscreenDeviceContext, 0, 0, myInfo -> width,
	       myInfo -> height);
    DeleteObject (myPen);
    DeleteObject (myBrush);
    
    return myRunWindow;
} // MDIOWinGraph_CreateWindow


/************************************************************************/
/* MDIOWinGraph_GetMousePos					      	*/
/************************************************************************/
void	MDIOWinGraph_GetMousePos (WIND pmWindow, OOTint *pmX, OOTint *pmY, 
				  OOTint *pmButtonUpDown)
{
    MIOWinInfoPtr	myInfo = MIOWin_GetInfo (pmWindow);
    POINT		myPoint;
    
    GetCursorPos (&myPoint);
    
    ScreenToClient (myInfo -> innerWindow, &myPoint);
    
    *pmX = myPoint.x - MARGIN + myInfo -> leftX;
    *pmY = myInfo -> height - (myPoint.y - MARGIN + myInfo -> topY) - 1;
    *pmButtonUpDown =
        ((GetAsyncKeyState (VK_LBUTTON) & 0x8000) ? LEFT_BUTTON : 0) +
	((GetAsyncKeyState (VK_MBUTTON) & 0x8000) ? MIDDLE_BUTTON : 0) +
	((GetAsyncKeyState (VK_RBUTTON) & 0x8000) ? RIGHT_BUTTON : 0);
} // MDIOWinGraph_GetMousePos

				  
/************************************************************************/
/* MDIOWinGraph_GetDefaultPalette				      	*/
/************************************************************************/
PALETTE	MDIOWinGraph_GetDefaultPalette (void)
{
    return stDefaultPalette;
} // MDIOWinGraph_GetDefaultPalette


/************************************************************************/
/* MDIOWinGraph_InitializeWindow				      	*/
/************************************************************************/
BOOL	MDIOWinGraph_InitializeWindow (WIND pmWindow)
{
    MIOWinInfoPtr	myInfo = MIOWin_GetInfo (pmWindow);
    
    // Initialize the run window graphics
    myInfo -> deviceContext = GetDC (myInfo -> innerWindow);
    myInfo -> offscreenDeviceContext = 
    				CreateCompatibleDC (myInfo -> deviceContext);
    myInfo -> offscreenBitMap = CreateCompatibleBitmap (
    					myInfo -> deviceContext,
    					myInfo -> width, myInfo -> height);
    SelectObject ((HDC) myInfo -> offscreenDeviceContext, 
    		  (HBITMAP) myInfo -> offscreenBitMap);
    
    // Set up the palette
    SelectPalette ((HDC) myInfo -> deviceContext, stDefaultPalette, FALSE);
    SelectPalette ((HDC) myInfo -> offscreenDeviceContext, stDefaultPalette, 
    		   FALSE);
    RealizePalette ((HDC) myInfo -> deviceContext);
    RealizePalette ((HDC) myInfo -> offscreenDeviceContext);

    myInfo -> usingDefaultPalette = TRUE;
    myInfo -> palette = stDefaultPalette;
    myInfo -> numColours = stDefaultPaletteSize;
        
    myInfo -> textColour = FG_COLOUR;
    myInfo -> textBGColour = BG_COLOUR;

    myInfo -> cachedPenColour = NO_COLOUR;
    myInfo -> cachedBrushColour = NO_COLOUR;

    myInfo -> clearToEndOfLine = TRUE;

    SetViewportOrgEx (myInfo -> deviceContext, MARGIN, MARGIN, NULL);
    MySetOnscreenClipRegion (myInfo);    		      
    
    return TRUE;
} // MDIOWinGraph_InitializeWindow


/************************************************************************/
/* MDIOWinGraph_PutChar						      	*/
/************************************************************************/
void	MDIOWinGraph_PutChar (WIND pmWindow, char pmChar, int pmRow, int pmCol)
{
    MIOWinInfoPtr	myInfo = MIOWin_GetInfo (pmWindow);
    RECT		myRect;
    unsigned short	myUnicodeChar;
    
    myRect.top = pmRow * myInfo -> fontCharHeight;
    myRect.bottom = myRect.top + myInfo -> fontCharHeight;
    myRect.left = pmCol * myInfo -> fontCharWidth;
    myRect.right = myRect.left + myInfo -> fontCharWidth;
    
    // Set the font, foreground and background colour for the offscreen
    SelectFont (myInfo -> offscreenDeviceContext, myInfo -> font);
    SetTextColor (myInfo -> offscreenDeviceContext, 
    		  PALETTEINDEX (myInfo -> textColour));
    SetBkMode(myInfo -> offscreenDeviceContext, OPAQUE);
    SetBkColor (myInfo -> offscreenDeviceContext, 
    		PALETTEINDEX (myInfo -> textBGColour));

    // Draw it offscreen    		
    if (myInfo -> msdosCharSet)
    {
	MultiByteToWideChar (CP_OEMCP, 0, &pmChar, 1, &myUnicodeChar, 2);
	ExtTextOutW (myInfo -> offscreenDeviceContext, myRect.left, myRect.top,
		     ETO_CLIPPED | ETO_OPAQUE, &myRect, &myUnicodeChar, 1, NULL);
    }
    else
    {
	ExtTextOut (myInfo -> offscreenDeviceContext, myRect.left, myRect.top,
		    ETO_CLIPPED | ETO_OPAQUE, &myRect, &pmChar, 1, NULL);
    }
	        
    // Set the font, foreground and background colour for the window
    if (myInfo -> displayOnScreen)
    {
	SelectFont (myInfo -> deviceContext, myInfo -> font);
	SetTextColor (myInfo -> deviceContext, 
		      PALETTEINDEX (myInfo -> textColour));
	SetBkMode(myInfo -> deviceContext, OPAQUE);
	SetBkColor (myInfo -> deviceContext, 
		    PALETTEINDEX (myInfo -> textBGColour));
	
	// Draw it to the window    		
	HideCaret (myInfo -> innerWindow);
	if (myInfo -> msdosCharSet)
	{
	    ExtTextOutW (myInfo -> deviceContext, myRect.left, myRect.top,
			 ETO_CLIPPED | ETO_OPAQUE, &myRect, &myUnicodeChar, 
			 1, NULL);
	}
	else
	{
	    ExtTextOut (myInfo -> deviceContext, myRect.left, myRect.top,
			ETO_CLIPPED | ETO_OPAQUE, &myRect, &pmChar, 1, NULL);
	}
	ShowCaret (myInfo -> innerWindow);
    }
} // MDIOWinGraph_PutChar


/************************************************************************/
/* MDIOWinGraph_PutString					      	*/
/************************************************************************/
void	MDIOWinGraph_PutString (WIND pmWindow, const char *pmString,
				int pmLen, int pmRow, int pmCol)
{
    MIOWinInfoPtr	myInfo = MIOWin_GetInfo (pmWindow);
    RECT		myRect;
    unsigned short	myUnicodeString [1000];

    myRect.top = pmRow * myInfo -> fontCharHeight;
    myRect.bottom = myRect.top + myInfo -> fontCharHeight;
    myRect.left = pmCol * myInfo -> fontCharWidth;
    myRect.right = myRect.left + pmLen * myInfo -> fontCharWidth;
    
    // Set the font, foreground and background colour for the offscreen
    SelectFont (myInfo -> offscreenDeviceContext, myInfo -> font);
    SetTextColor (myInfo -> offscreenDeviceContext, 
    		  PALETTEINDEX (myInfo -> textColour));
    SetBkColor (myInfo -> offscreenDeviceContext, 
    		PALETTEINDEX (myInfo -> textBGColour));

    // Draw it offscreen
    if (myInfo -> msdosCharSet)
    {
	MultiByteToWideChar (CP_OEMCP, 0, pmString, pmLen, myUnicodeString, 
			     1000);
	ExtTextOutW (myInfo -> offscreenDeviceContext, myRect.left, myRect.top,
		     ETO_CLIPPED | ETO_OPAQUE, &myRect, myUnicodeString, pmLen,
		     NULL);
    }
    else
    {
	ExtTextOut (myInfo -> offscreenDeviceContext, myRect.left, myRect.top,
		    ETO_CLIPPED | ETO_OPAQUE, &myRect, pmString, pmLen, NULL);
    }

    if (myInfo -> displayOnScreen)
    {
	// Set the font, foreground and background colour for the window
	SelectFont (myInfo -> deviceContext, myInfo -> font);
	SetTextColor (myInfo -> deviceContext, 
		      PALETTEINDEX (myInfo -> textColour));
	SetBkColor (myInfo -> deviceContext, 
		    PALETTEINDEX (myInfo -> textBGColour));
	
	// Draw it to the window    		
	HideCaret (myInfo -> innerWindow);
	if (myInfo -> msdosCharSet)
	{
	    ExtTextOutW (myInfo -> deviceContext, myRect.left, myRect.top,
			 ETO_CLIPPED | ETO_OPAQUE, &myRect, myUnicodeString, 
			 pmLen, NULL);
	}
	else
	{
	    ExtTextOut (myInfo -> deviceContext, myRect.left, myRect.top,
			ETO_CLIPPED | ETO_OPAQUE, &myRect, pmString, pmLen, 
			NULL);
	}
	ShowCaret (myInfo -> innerWindow);
    }
} // MDIOWinGraph_PutString

				
/************************************************************************/
/* MDIOWinGraph_SaveWindowToFile				      	*/
/************************************************************************/
void	MDIOWinGraph_SaveWindowToFile (WIND pmWindow, const char *pmPathName)
{
    BOOL		myNewPathName;
    char		myPathName [4096];
    MIOWinInfoPtr	myInfo;
    
    MIOWin_AssertWindowType (pmWindow, WINDOW_TYPE_MIO_GRAPHICS);
    
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
	strcat (myPathName, " Output.bmp");
	
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
		strcat (myPathName, ".bmp");
	    }
	    else
	    {
		strcat (myPathName, ".BMP");
	    }
	}    
    }

    // Save the text to the file.  If it fails, return immediately
    MySaveGraphicsToFile (pmWindow, myPathName);
} // MDIOWinGraph_SaveWindowToFile


/************************************************************************/
/* MDIOWinGraph_ScrollUpALine					      	*/
/************************************************************************/
void	MDIOWinGraph_ScrollUpALine (WIND pmWindow)
{
    MIOWinInfoPtr	myInfo = MIOWin_GetInfo (pmWindow);
    RECT		myRect;
    HBRUSH		myBGColourBrush;

    //
    // Scroll the offscreen bitmap
    //
    ScrollDC (myInfo -> offscreenDeviceContext, 0, -myInfo -> fontCharHeight,
    	      NULL, NULL, NULL, NULL);
    	      
    myRect.top = (myInfo -> maxRow - 1) * myInfo -> fontCharHeight;
    myRect.bottom = myRect.top + myInfo -> fontCharHeight;
    myRect.left = 0;
    myRect.right = myInfo -> width;
    
    // Set the font, foreground and background colour for the offscreen
    myBGColourBrush = CreateSolidBrush (PALETTEINDEX (myInfo -> textBGColour));

    // Draw it offscreen    		
    FillRect (myInfo -> offscreenDeviceContext, &myRect, myBGColourBrush);
	        
    DeleteObject (myBGColourBrush);
    
    //
    // Now copy the offscreen bitmap to the window
    //
    if (myInfo -> displayOnScreen)
    {
    	MyDisplayGraphics (pmWindow, FALSE);
    }
    
    // This sleep command allows scrolling not to lock the mouse up
    // under some Windows machines.  Otherwise ScrollDC and MyDisplayGraphics
    // cause the mouse cursor to go near catatonic.
    Sleep (10);
} // MDIOWinGraph_ScrollUpALine


/************************************************************************/
/* MDIOWinGraph_SynchScrollBarsToGraphics			      	*/
/*						      			*/
/* Set the scroll bars to correspond with the current top-most line and	*/
/* left-most column.  Also change the size of the scroll bars to 	*/
/* reflect the number of lines and the longest line.			*/
/************************************************************************/
void	MDIOWinGraph_SynchScrollBarsToGraphics (HWND pmWindow)
{
    MIOWinInfoPtr	myInfo;
    HWND		myInnerWindow;
    RECT		myWindowRect, myClientRect;
    SCROLLINFO		myScrollInfo;
    int			myGraphicsRunWindowWidth, myGraphicsRunWindowHeight;
    int			myMaxLeftX, myMaxTopY;
        
    myInfo = MIOWin_GetInfo (pmWindow);
    myInnerWindow = myInfo -> innerWindow;
    
    if (!GetWindowRect (myInnerWindow, &myWindowRect))
    {
    	return;
    }
    if (!GetClientRect (myInnerWindow, &myClientRect))
    {
    	return;
    }
    
    myGraphicsRunWindowWidth = myWindowRect.right - myWindowRect.left;
    myGraphicsRunWindowHeight = myWindowRect.bottom - myWindowRect.top;
    
    // If either scroll bar is needed, both are made visible.
    if ((myGraphicsRunWindowWidth == myInfo -> graphicsRunWindowWidth) &&
    	(myGraphicsRunWindowHeight == myInfo -> graphicsRunWindowHeight))
    {
	myScrollInfo.cbSize = sizeof (SCROLLINFO);
        myScrollInfo.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
    	myScrollInfo.nMin = 0;
    	myScrollInfo.nMax = 0;
    	myScrollInfo.nPage = 0;
    	myScrollInfo.nPos = 0;

	// Hide the horizontal scroll bar    
    	if (myInfo -> leftX != 0)
    	{
    	    myInfo -> leftX = 0;
    	    InvalidateRect (myInnerWindow, NULL, FALSE);
	}    	    
    	SetScrollInfo (myInnerWindow, SB_HORZ, &myScrollInfo, TRUE);
    	
    	// Hide the vertical scroll bar
    	if (myInfo -> topY != 0)
    	{
    	    myInfo -> topY = 0;
    	    InvalidateRect (myInnerWindow, NULL, FALSE);
	}    	    
	SetViewportOrgEx (myInfo -> deviceContext, MARGIN, MARGIN, NULL);
	MySetOnscreenClipRegion (myInfo);	    		      
    	SetScrollInfo (myInnerWindow, SB_VERT, &myScrollInfo, TRUE);
    }
    else
    {
        // Do horizontal scroll bar first
    	myScrollInfo.cbSize = sizeof (SCROLLINFO);
    	myScrollInfo.fMask = SIF_DISABLENOSCROLL | SIF_PAGE | SIF_POS | SIF_RANGE;
    	myScrollInfo.nMin = 0;
    	myScrollInfo.nMax = myInfo -> graphicsRunWindowWidth - 1;
    	myScrollInfo.nPage = myClientRect.right - myClientRect.left;
    	myScrollInfo.nPos = myInfo -> leftX;
    	SetScrollInfo (myInnerWindow, SB_HORZ, &myScrollInfo, TRUE);

    	myMaxLeftX = myInfo -> graphicsRunWindowWidth - myScrollInfo.nPage;
    	if (myInfo -> leftX > myMaxLeftX)
    	{
    	    myInfo -> leftX = myMaxLeftX;
	    SetViewportOrgEx (myInfo -> deviceContext, MARGIN - myInfo -> leftX, 
	    		      MARGIN - myInfo -> topY, NULL);
	    SetScrollPos (myInnerWindow, SB_HORZ, myMaxLeftX, TRUE);
    	    InvalidateRect (myInnerWindow, NULL, FALSE);
    	}
    
    	// Do vertical scroll bar
    	myScrollInfo.cbSize = sizeof (SCROLLINFO);
    	myScrollInfo.nMax = myInfo -> graphicsRunWindowHeight - 1;
    	myScrollInfo.nPage = myClientRect.bottom - myClientRect.top;
    	myScrollInfo.nPos = myInfo -> topY;
    	SetScrollInfo (myInnerWindow, SB_VERT, &myScrollInfo, TRUE);

    	myMaxTopY = myInfo -> graphicsRunWindowHeight - myScrollInfo.nPage;
    	if (myInfo -> topY > myMaxTopY)
    	{
    	    myInfo -> topY = myMaxTopY;
	    SetViewportOrgEx (myInfo -> deviceContext, MARGIN - myInfo -> leftX, 
	    		      MARGIN - myInfo -> topY, NULL);
	    SetScrollPos (myInnerWindow, SB_VERT, myMaxTopY, TRUE);
    	    InvalidateRect (myInnerWindow, NULL, FALSE);
    	}

	MySetOnscreenClipRegion (myInfo);	    		      
    }

    SetWindowPos (myInnerWindow, 0, 0, 0, 0, 0,
		  SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED); 
} // MDIOWinGraph_SynchScrollBarsToGraphics


/******************************/
/* Static callback procedures */
/******************************/
/************************************************************************/
/* MyGraphicsInnerWindowProcedure					*/
/************************************************************************/
static LRESULT CALLBACK MyGraphicsInnerWindowProcedure (
						  HWND pmInnerWindow, 
						  UINT pmMessage, 
    						  WPARAM pmWParam, 
    						  LPARAM pmLParam)
{
    MIOWinInfoPtr	myInfo;
    UCHAR		mySpecialChar;

    // Log the windows messages
    if (MIOWin_GetProperties () -> logWindowMessages)
    {
//	EdLog_PrintWindowsMessage ("RunText", pmRunTextWindow, 
//	    pmMessage, pmWParam, pmLParam);
    }
    
    switch (pmMessage)
    {    
    	case WM_CREATE:
	    if (MIOWin_GetProperties () -> logWindowMessages)
	    {
//	    	EdLog_Log ("Run Text Window = %x  (Starting Creation)", 
//	    	    pmRunTextWindow);
	    }

	    return MyInitializeGraphicsInnerWindow (pmInnerWindow);

        case WM_PAINT:
            MyDisplayGraphics (pmInnerWindow, TRUE);
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
	    MIOWin_GetInfo (pmInnerWindow) -> pressedKeys [
				    stTuringVirtualKeycode [pmWParam]] = 1;
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

	case WM_KEYUP:
	case WM_SYSKEYUP:
	    MIOWin_GetInfo (pmInnerWindow) -> pressedKeys [
				    stTuringVirtualKeycode [pmWParam]] = 0;
	    return 0;

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
/*
	case WM_MOUSEMOVE:
	    myInfo = MIOWin_GetInfo (pmInnerWindow);
	    if (myInfo -> selectingWithMouse)
	    {
	    	if (MyMouseDownHandler (pmInnerWindow, 
	    	    GET_X_LPARAM (pmLParam), GET_Y_LPARAM (pmLParam), 
	    	    TRUE, FALSE))
                {
            	    myInfo -> verticalTicks = 0;
            	    myInfo -> horizontalTicks = 0;
                }
	    }
            return 0;
*/
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
	    // Add to the mouse buffer if appropriate
	    if (!MIO_paused & !MIO_finished)
	    {
		MOUSEPOINT  *myPoint;

	    	myInfo = MIOWin_GetInfo (pmInnerWindow);

		myPoint = &myInfo -> mouseBuffer [myInfo -> mouseBufferHead];
		myInfo -> mouseBufferHead = INC(myInfo -> mouseBufferHead);
		myPoint -> x = GET_X_LPARAM (pmLParam) - MARGIN + 
			       myInfo -> leftX;
		myPoint -> y = myInfo -> height - 
		    (GET_Y_LPARAM (pmLParam) - MARGIN + myInfo -> topY) - 1;
		if ((pmMessage == WM_LBUTTONDOWN) || 
		    (pmMessage == WM_LBUTTONUP))
		{
		    myPoint -> button = LEFT_BUTTON;
		}
		else if ((pmMessage == WM_RBUTTONDOWN) || 
		         (pmMessage == WM_RBUTTONUP))
		{
		    myPoint -> button = RIGHT_BUTTON;
		}
		else
		{
		    myPoint -> button = MIDDLE_BUTTON;
		}
		if ((pmMessage == WM_LBUTTONDOWN) || 
		    (pmMessage == WM_RBUTTONDOWN) ||
		    (pmMessage == WM_MBUTTONDOWN))
		{
		    myPoint -> buttonDirection = BUTTON_DOWN;
		}
		else
		{
		    myPoint -> buttonDirection = BUTTON_UP;
		}
		return 0;
	    }
            break;
            	    
/*	case WM_MOUSEWHEEL:
	    if (stWheelMouseDeltaPerLine != 0)
	    {
	    	myInfo = MIOWin_GetInfo (pmInnerWindow);
	    	
	        myInfo -> accumulateDelta += HIWORD (pmWParam);
	        while (myInfo -> accumulateDelta >= stWheelMouseDeltaPerLine)
	    	{
	    	    MyScrollHandlerVert (pmInnerWindow, SB_LINEUP, 0);
	    	    myInfo -> accumulateDelta -= stWheelMouseDeltaPerLine;
	    	}
	    	while (myInfo -> accumulateDelta <= -stWheelMouseDeltaPerLine)
	    	{
	    	    MyScrollHandlerVert (pmInnerWindow, SB_LINEDOWN, 0);
	    	    myInfo -> accumulateDelta += stWheelMouseDeltaPerLine;
	    	}
	    	return 0;
	    }
	    break;
*/	    
	//
	// Focus handling
	//
        case WM_SETFOCUS:
	    MDIOWin_CreateDestroyCaret (pmInnerWindow, CREATE_CARET);
	    MyDisplayGraphics (pmInnerWindow, FALSE);
            return 0;

	case WM_KILLFOCUS:
	    MDIOWin_CreateDestroyCaret (pmInnerWindow, DESTROY_CARET);
            MyDisplayGraphics (pmInnerWindow, TRUE);
	    return 0;
/*	    
	 default:
	    if ((pmMessage == stMouseWheelMessage) && 
	    	(stWheelMouseDeltaPerLine != 0))
	    {
	    	myInfo = MIOWin_GetInfo (pmInnerWindow);
	    	
	        myInfo -> accumulateDelta += HIWORD (pmWParam);
	        while (myInfo -> accumulateDelta >= stWheelMouseDeltaPerLine)
	    	{
	    	    MyScrollHandlerVert (pmInnerWindow, SB_LINEUP, 0);
	    	    myInfo -> accumulateDelta -= stWheelMouseDeltaPerLine;
	    	}
	    	while (myInfo -> accumulateDelta <= -stWheelMouseDeltaPerLine)
	    	{
	    	    MyScrollHandlerVert (pmInnerWindow, SB_LINEDOWN, 0);
	    	    myInfo -> accumulateDelta += stWheelMouseDeltaPerLine;
	    	}
	    	return 0;
            }
*/            
    }
    
    return DefWindowProc (pmInnerWindow, pmMessage, pmWParam, 
        		  pmLParam);
} // MyGraphicsInnerWindowProcedure

    						  
/************************************************************************/
/* MyGraphicsWindowProcedure						*/
/************************************************************************/
static LRESULT CALLBACK MyGraphicsWindowProcedure (HWND pmWindow, 
						   UINT pmMessage, 
						   WPARAM pmWParam, 
						   LPARAM pmLParam)
{
    // Last parameter is pmIsGraphicsWindow = TRUE
    return MDIOWin_WindowProcedure (pmWindow, pmMessage, pmWParam, 
				    pmLParam, TRUE);
} // MyGraphicsWindowProcedure
						 

/*********************/
/* Static procedures */
/*********************/
/************************************************************************/
/* MyCreateDefaultPalette						*/
/************************************************************************/
static BOOL	MyCreateDefaultPalette (void)
{
    HRSRC	myResourceHandle;
    HGLOBAL	myResourceData;
    LOGPALETTE	*myDefaultLogPalette;
    WORD	*myPtr;
    int		myNumEntries;
    int		myEntry;
    
    myResourceHandle = FindResource (GetModuleHandle (NULL), 
    				     MAKEINTRESOURCE (DEFAULT_PALETTE_RSRC), 
    				     RT_RCDATA);
    if (myResourceHandle == NULL)
    {    				    
    	EdInt_FailWarn (IDS_MIO_FINDRESOURCEFAIL, __FILE__, __LINE__, 
    			GetLastError ());
    	return FALSE;
    }

    myResourceData = LoadResource (GetModuleHandle (NULL), myResourceHandle);
    if (myResourceData == NULL)
    {
    	EdInt_FailWarn (IDS_MIO_LOADRESOURCEFAIL, __FILE__, __LINE__, 
    			GetLastError ());
    	return FALSE;
    }
    
    myPtr = (WORD *) myResourceData;
    myNumEntries = *myPtr++;
    myDefaultLogPalette = malloc (sizeof (LOGPALETTE) + 
				sizeof(PALETTEENTRY) * myNumEntries);
    // TW Check for NULL
    myDefaultLogPalette -> palVersion = 0x300;
    myDefaultLogPalette -> palNumEntries = myNumEntries;
    for (myEntry = 0 ; myEntry < myNumEntries ; myEntry++)
    {
	myDefaultLogPalette -> palPalEntry [myEntry].peRed = (BYTE) *myPtr++;
	myDefaultLogPalette -> palPalEntry [myEntry].peGreen = (BYTE) *myPtr++;
	myDefaultLogPalette -> palPalEntry [myEntry].peBlue = (BYTE) *myPtr++;
	myDefaultLogPalette -> palPalEntry [myEntry].peFlags = 0;
    }
    stDefaultPalette = CreatePalette (myDefaultLogPalette);
    stDefaultPaletteSize = myNumEntries;
    
    free (myDefaultLogPalette);
    
    return TRUE;
} // MyCreateDefaultPalette


/************************************************************************/
/* MyCreateBitmapInfo							*/
/************************************************************************/
static PBITMAPINFO	MyCreateBitmapInfo (HBITMAP pmBitMap)
{ 
    BITMAP	myBitMap;
    PBITMAPINFO myBitMapInfo; 
    WORD	myClrBits; 

    // Retrieve the bitmap's color format, width, and height. 
    if (!GetObject (pmBitMap, sizeof(BITMAP), (LPSTR) &myBitMap)) 
    {
    	// TW Handle Error
    	return NULL;
    }

    // Convert the color format to a count of bits. 
    myClrBits = (WORD) (myBitMap.bmPlanes * myBitMap.bmBitsPixel); 
    if (myClrBits == 1) 
        myClrBits = 1; 
    else if (myClrBits <= 4) 
        myClrBits = 4; 
    else if (myClrBits <= 8) 
        myClrBits = 8; 
    else if (myClrBits <= 16) 
        myClrBits = 16; 
    else if (myClrBits <= 24) 
        myClrBits = 24; 
    else 
    	myClrBits = 32; 

    // Allocate memory for the BITMAPINFO structure. (This structure 
    // contains a BITMAPINFOHEADER structure and an array of RGBQUAD 
    // data structures.) 
    if (myClrBits < 24) 
    {
        myBitMapInfo = (PBITMAPINFO) malloc (sizeof(BITMAPINFOHEADER) + 
			                    sizeof(RGBQUAD) * (1 << myClrBits)); 
    }
    else 
    {
     	// There is no RGBQUAD array for the 24 or 32 bit-per-pixel format. 
        myBitMapInfo = (PBITMAPINFO) malloc (sizeof(BITMAPINFOHEADER)); 
    }        
    if (myBitMapInfo == NULL)
    {
    	return NULL;
    }
    
    // Initialize the fields in the BITMAPINFO structure. 
    myBitMapInfo -> bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
    myBitMapInfo -> bmiHeader.biWidth = myBitMap.bmWidth; 
    myBitMapInfo -> bmiHeader.biHeight = myBitMap.bmHeight; 
    myBitMapInfo -> bmiHeader.biPlanes = myBitMap.bmPlanes; 
    myBitMapInfo -> bmiHeader.biBitCount = myBitMap.bmBitsPixel; 
    if (myClrBits < 24) 
        myBitMapInfo -> bmiHeader.biClrUsed = (1 << myClrBits); 
    else
        myBitMapInfo -> bmiHeader.biClrUsed = 0;

    // If the bitmap is not compressed, set the BI_RGB flag. 
    myBitMapInfo -> bmiHeader.biCompression = BI_RGB; 

    // Compute the number of bytes in the array of color 
    // indices and store the result in biSizeImage. 
    // For Windows NT/2000, the width must be DWORD aligned unless 
    // the bitmap is RLE compressed. This example shows this. 
    // For Windows 95/98, the width must be WORD aligned unless the 
    // bitmap is RLE compressed.
    myBitMapInfo -> bmiHeader.biSizeImage = 
    	((myBitMapInfo->bmiHeader.biWidth * myClrBits + 31) & ~31) / 8
                                  * myBitMapInfo->bmiHeader.biHeight; 
    // Set biClrImportant to 0, indicating that all of the 
    // device colors are important. 
    myBitMapInfo->bmiHeader.biClrImportant = 0; 
    
    return myBitMapInfo; 
} // MyCreateBitmapInfo


/************************************************************************/
/* MyDisplayGraphics							*/
/************************************************************************/
static void	MyDisplayGraphics (HWND pmWindow, BOOL pmPaintUpdate)
{
    MIOWinInfoPtr	myInfo;
    HWND		myInnerWindow;
    HDC			myDeviceContext;
    HPEN		myWhitePen;
    int			i;
    PAINTSTRUCT		myPaintStruct;
    HRGN		myClipRegion = CreateRectRgn (0, 0, 0, 0);


    // Get the window info    
    myInfo = MIOWin_GetInfo (pmWindow);
    myInnerWindow = myInfo -> innerWindow;

    // Prepare for painting   
    if (pmPaintUpdate)
    {
        myDeviceContext = BeginPaint (myInnerWindow, &myPaintStruct);
    }        
    else
    {
        myDeviceContext = GetDC (myInnerWindow);
    }
    
    HideCaret (myInnerWindow);

    // Temporarily clear clipping
    GetClipRgn (myDeviceContext, myClipRegion);
    SelectClipRgn (myDeviceContext, NULL);

    // ClearWindow(rw, rw->scrnColour, FALSE);
    // SelectPalette (myDeviceContext, myInfo -> palette, FALSE);
    // SelectPalette (myInfo -> offscreenDeviceContext, myInfo -> palette, FALSE);
    // RealizePalette (myDeviceContext);
    // RealizePalette (myInfo -> offscreenDeviceContext);


    // Redraw sprites if necessary
    if (myInfo -> spriteQueueHead == NULL)
    {
	// Blit the memory copy onto screen
	BitBlt (myDeviceContext, MARGIN - myInfo -> leftX, MARGIN - myInfo -> topY, 
	    myInfo -> width, myInfo -> height, 
	    myInfo -> offscreenDeviceContext, 0, 0, SRCCOPY);
    }
    else
    {
	// This also redraws the screen.
	MIOSprite_Dirty (myInfo, 0, 0, myInfo -> width, myInfo -> height);
	MIOSprite_UpdateIfNecessary (FALSE, TRUE);
    }

    // Clear the margins
    myWhitePen = GetStockObject (WHITE_PEN);
    SelectObject (myDeviceContext, myWhitePen);
    for (i = 0 ; i < MARGIN ; i++)
    {
    	MoveToEx (myDeviceContext, i - myInfo -> leftX, i - myInfo -> topY,
    		  NULL);
	LineTo (myDeviceContext, 
		i - myInfo -> leftX, 
		myInfo -> graphicsRunWindowHeight - 1 - i - myInfo -> topY);
	LineTo (myDeviceContext, 	  
		myInfo -> graphicsRunWindowWidth - 1 - i - myInfo -> leftX,
		myInfo -> graphicsRunWindowHeight - 1 - i - myInfo -> topY);
	LineTo (myDeviceContext, 	  
		myInfo -> graphicsRunWindowWidth - 1 - i - myInfo -> leftX, 
		-myInfo -> topY + i);
	LineTo (myDeviceContext, i - myInfo -> leftX, i - myInfo -> topY);
    }    	    	
    	        	
    // Restore clipping regions
    SelectClipRgn (myDeviceContext, myClipRegion);
    DeleteObject (myClipRegion);
       
    ShowCaret (myInnerWindow);
    
    if (pmPaintUpdate)
    {
    	EndPaint (myInnerWindow, &myPaintStruct);
    }
    else
    {
    	ReleaseDC (myInnerWindow, myDeviceContext);
    }
} // MyDisplayGraphics


/************************************************************************/
/* MyInitializeGraphicsInnerWindow					*/
/************************************************************************/
static int	MyInitializeGraphicsInnerWindow (HWND pmInnerWindow)
{
    //
    // Initialize the extra window memory
    //
	
    // Set the window type            
    SetWindowLong (pmInnerWindow, WINDOW_MEM_WINDOW_TYPE, 
        (long) WINDOW_TYPE_MIO_GRAPHICS_INNER);

    // Set the window initialized                
    SetWindowLong (pmInnerWindow, WINDOW_MEM_WINDOW_INITIALIZED, 
        (long) TRUE);
            
    return 0;
} // MyInitializeGraphicsInnerWindow

				    
/************************************************************************/
/* MySaveGraphicsToFile							*/
/************************************************************************/
static BOOL	MySaveGraphicsToFile (HWND pmWindow, const char *pmPathName)
{
    MIOWinInfoPtr	myInfo = MIOWin_GetInfo (pmWindow);
    PBITMAPINFO		myBitMapInfo;
    PBITMAPINFOHEADER	myBitMapInfoHeader;
    void		*myBits;
    HANDLE		myFile;
    BITMAPFILEHEADER	myFileHeader;	// Bitmap file-header 
    DWORD		myBytesWritten;
    
    myBitMapInfo = MyCreateBitmapInfo (myInfo -> offscreenBitMap);
    if (myBitMapInfo == NULL)
    {
    	// Error Message
    	return FALSE;
    }
    
    myBitMapInfoHeader = (PBITMAPINFOHEADER) myBitMapInfo;
    myBits = malloc (myBitMapInfoHeader -> biSizeImage);
    if (myBits == NULL)
    {
    	// Error Message
    	return FALSE;
    }

    // Retrieve the color table (RGBQUAD array) and the bits 
    // (array of palette indices) from the DIB. 
    if (!GetDIBits (myInfo -> deviceContext, myInfo -> offscreenBitMap, 0, 
    		    (WORD) myBitMapInfoHeader -> biHeight, myBits, myBitMapInfo, 
		    DIB_RGB_COLORS)) 
    {
    	// Error Message
    	return FALSE;
    }

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
    
    myFileHeader.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M" 
    // Compute the size of the entire file. 
    myFileHeader.bfSize = (DWORD) (sizeof (BITMAPFILEHEADER) +
                 myBitMapInfoHeader -> biSize + 
                 myBitMapInfoHeader -> biClrUsed * sizeof(RGBQUAD) + 
                 myBitMapInfoHeader -> biSizeImage); 
    myFileHeader.bfReserved1 = 0; 
    myFileHeader.bfReserved2 = 0; 

    // Compute the offset to the array of color indices. 
    myFileHeader.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) + 
                    myBitMapInfoHeader -> biSize + 
                    myBitMapInfoHeader -> biClrUsed * sizeof (RGBQUAD); 

    // Copy the BITMAPFILEHEADER into the .BMP file. 
    if (!WriteFile (myFile, (LPVOID) &myFileHeader, sizeof (BITMAPFILEHEADER), 
		    &myBytesWritten,  NULL)) 
    {
    	// TW Error
    	return FALSE;
    }

    // Copy the BITMAPINFOHEADER and RGBQUAD array into the file. 
    if (!WriteFile (myFile, (LPVOID) myBitMapInfoHeader, 
    		    sizeof (BITMAPINFOHEADER) 
                  	+ myBitMapInfoHeader -> biClrUsed * sizeof (RGBQUAD), 
                    &myBytesWritten, NULL))
    {
    	// TW Error
    	return FALSE;
    }

    // Copy the array of  indices into the .BMP file. 
    if (!WriteFile (myFile, myBits, myBitMapInfoHeader -> biSizeImage, 
                    &myBytesWritten, NULL))
    {
    	// TW Error
    	return FALSE;
    }

    // Close the .BMP file. 
    if (!CloseHandle (myFile)) 
    {
    	// TW Error
    	return FALSE;
    }

    // Free memory. 
    free (myBits);
    
    return TRUE;
} // MySaveGraphicsToFile


/************************************************************************/
/* MyScrollHandlerHorz							*/
/************************************************************************/
static void	MyScrollHandlerHorz (HWND pmRunGraphicsWindow, int pmPart, 
				     int pmValue)
{
    RECT		myRect;
    MIOWinInfoPtr	myInfo;
    int			myWidth;
    int			myMaxLeftX;
    int 		myNewLeftX;
        
    if (!GetClientRect (pmRunGraphicsWindow, &myRect))
    {
    	return;
    }
    
    myInfo = MIOWin_GetInfo (pmRunGraphicsWindow);
    myWidth = myRect.right - myRect.left;
    myMaxLeftX = max (0, myInfo -> graphicsRunWindowWidth - myWidth);
    
    switch (pmPart)
    {
    	case SB_PAGERIGHT:
    	    myNewLeftX = myInfo -> leftX + (myWidth - PIXEL_OVERLAP);
    	    break;
    	case SB_LINERIGHT:
    	    myNewLeftX = myInfo -> leftX + 1;
    	    break;
    	case SB_PAGELEFT:
    	    myNewLeftX = myInfo -> leftX - (myWidth - PIXEL_OVERLAP);
    	    break;
    	case SB_LINELEFT:
    	    myNewLeftX = myInfo -> leftX - 1;
    	    break;
    	case SB_LEFT:
    	    myNewLeftX = 0;
    	    break;
    	case SB_RIGHT:
    	    myNewLeftX = myMaxLeftX;
    	    break;
    	case SB_THUMBPOSITION:
	    myNewLeftX = pmValue;    	    
    	    break;
    	case SB_THUMBTRACK:
    	    myNewLeftX = pmValue;
    	    break;
    	default:
    	    return;
    } // switch
    
    myNewLeftX = max (0, min (myNewLeftX, myMaxLeftX));
    
    if (myNewLeftX == myInfo -> leftX)
        return;
    
    myInfo -> leftX = myNewLeftX;
    SetViewportOrgEx (myInfo -> deviceContext, MARGIN - myInfo -> leftX, 
    		      MARGIN - myInfo -> topY, NULL);
    MySetOnscreenClipRegion (myInfo);    		      
    SetScrollPos (pmRunGraphicsWindow, SB_HORZ, myNewLeftX, TRUE);
    InvalidateRect (pmRunGraphicsWindow, NULL, FALSE);
    MIOWin_CaretMove (pmRunGraphicsWindow);    
//    MyDisplayGraphics (pmRunGraphicsWindow, FALSE);
} // MyScrollHandlerHorz


/************************************************************************/
/* MyScrollHandlerVert							*/
/************************************************************************/
static void	MyScrollHandlerVert (HWND pmRunGraphicsWindow, int pmPart, 
				     int pmValue)
{
    RECT		myRect;
    MIOWinInfoPtr	myInfo;
    int			myHeight;
    int			myMaxTopY;
    int 		myNewTopY;
        
    if (!GetClientRect (pmRunGraphicsWindow, &myRect))
    {
    	return;
    }
    
    myInfo = MIOWin_GetInfo (pmRunGraphicsWindow);
    myHeight = myRect.bottom - myRect.top;
    myMaxTopY = max (0, myInfo -> graphicsRunWindowHeight - myHeight);
    
    switch (pmPart)
    {
    	case SB_PAGEDOWN:
    	    myNewTopY = myInfo -> topY + (myHeight - PIXEL_OVERLAP);
    	    break;
    	case SB_LINEDOWN:
    	    myNewTopY = myInfo -> topY + 1;
    	    break;
    	case SB_PAGEUP:
    	    myNewTopY = myInfo -> topY - (myHeight - PIXEL_OVERLAP);
    	    break;
    	case SB_LINEUP:
    	    myNewTopY = myInfo -> topY - 1;
    	    break;
    	case SB_TOP:
    	    myNewTopY = 0;
    	    break;
    	case SB_BOTTOM:
    	    myNewTopY = myMaxTopY;
    	    break;
    	case SB_THUMBPOSITION:
	    myNewTopY = pmValue;    	    
    	    break;
    	case SB_THUMBTRACK:
    	    myNewTopY = pmValue;
    	    break;
    	default:
    	    return;
    } // switch
        
    myNewTopY = max (0, min (myNewTopY, myMaxTopY));
    
    if (myNewTopY == myInfo -> topY)
        return;
    
    myInfo -> topY = myNewTopY;
    SetViewportOrgEx (myInfo -> deviceContext, MARGIN - myInfo -> leftX, 
    		      MARGIN - myInfo -> topY, NULL);
    MySetOnscreenClipRegion (myInfo);    		      
    SetScrollPos (pmRunGraphicsWindow, SB_VERT, myNewTopY, TRUE);
    InvalidateRect (pmRunGraphicsWindow, NULL, FALSE);
    MIOWin_CaretMove (pmRunGraphicsWindow);    
//    MyDisplayGraphics (pmRunGraphicsWindow, FALSE);
} // MyScrollHandlerVert


/************************************************************************/
/* MySetClipRegion							*/
/************************************************************************/
static void	MySetClipRegion (MIOWinInfoPtr myInfo)
{
    MySetOnscreenClipRegion (myInfo);
    
    SelectClipRgn (myInfo -> offscreenDeviceContext, myInfo -> clippingRegion);
} // MySetClipRegion


/************************************************************************/
/* MySetOnscreenClipRegion						*/
/*									*/
/* This routine assumes that the origin has already been properly set.	*/
/************************************************************************/
static void	MySetOnscreenClipRegion (MIOWinInfoPtr myInfo)
{
    // Given the user-defined Turing clipping region, set the 
    // real clipping region.

    // Set the clipping region for the onscreen window.
    SelectClipRgn (myInfo -> deviceContext, myInfo -> clippingRegion);
    // Offset it with the current margins, etc.
    OffsetClipRgn (myInfo -> deviceContext, MARGIN - myInfo -> leftX,
    		   MARGIN - myInfo -> topY);
    // Combine it with the visible window, without the margins
    IntersectClipRect (myInfo -> deviceContext, 
    		       max (myInfo -> leftX - MARGIN, 0),
    		       max (myInfo -> topY - MARGIN, 0),
    		       myInfo -> width, myInfo -> height);
} // MySetOnscreenClipRegion


/************************************************************************/
/* MySetRect								*/
/************************************************************************/
static void	MySetRect (RECT *pmRect, OOTint pmX1, OOTint pmY1, 
			   OOTint pmX2, OOTint pmY2)
{
    MIOWinInfoPtr	myInfo = MIOWin_GetInfo (MIO_selectedRunWindow);

    pmRect -> left = MIN (pmX1, pmX2);
    pmRect -> right = MAX (pmX1, pmX2);
    pmRect -> top = MIN (pmY1, pmY2);
    pmRect -> bottom = MAX (pmY1, pmY2);
} // MySetRect

