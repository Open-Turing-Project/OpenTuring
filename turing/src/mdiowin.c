/************************************************************************/
/* mdiowin.c								*/
/*									*/
/* This contains the platform dependent routines that relate to generic	*/
/* run windows (that is both text and graphics run windows).		*/
/*									*/
/* miowindow - Handles platform independent routines that relate to 	*/
/*             generic run windows.					*/
/* miowintext - Handles platform independent routines that relate to 	*/
/*              text windows.						*/
/* miowingraph - Handles platform independent routines that relate to	*/
/*               graphics windows.					*/
/* mdiowin - Handles platform dependent routines that relate to 	*/
/*           generic run windows.					*/
/* mdiowintext - Handles platform dependent routines that relate to	*/
/*               text windows.						*/
/* mdiowingraph - Handles platform dependent routines that relate to	*/
/*                graphics windows.					*/
/************************************************************************/

/****************/
/* Self include */
/****************/
#include "mdiowin.h"

/******************/
/* Other includes */
/******************/
#include "mio.h"
#include "mdio.h"
#include "miowintext.h"
#include "miowingraph.h"
#include "mdiowintext.h"
#include "mdiowingraph.h"

#include "miowindow.h"

#include "miosprite.h"

#include "miocommands.h"
#include "miodialogs.h"

#include "edint.h"
//#include "edfail.h"
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

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
#define NUM_SPEC_KEYS	22
#define FUNC_KEY_BASE	187
#define ADD_FOR_SHIFT	212-187
#define ADD_FOR_CTRL	222-187
#define ADD_FOR_ALT	232-187
#define KEYCODE_MINUS	189
#define KEYCODE_EQUALS	187

#define WM_SETTURINGRUNWINDOWPOS    (WM_USER + 20)

/********************/
/* Global variables */
/********************/

/*********/
/* Types */
/*********/

/**********************/
/* External variables */
/**********************/
extern void EdLog_Log();

/********************/
/* Static constants */
/********************/
static int stWindowsKeycode [NUM_SPEC_KEYS] =
{
    VK_UP, VK_DOWN, VK_LEFT, VK_RIGHT, VK_INSERT, 
    VK_DELETE, VK_HOME, VK_END, VK_PRIOR, VK_NEXT,
    VK_F1, VK_F2, VK_F3, VK_F4, VK_F5, VK_F6, 
    VK_F7, VK_F8, VK_F9, VK_F10, VK_F11, VK_F12
};

static UCHAR stTuringKeycode [NUM_SPEC_KEYS] =
{
    (UCHAR) 200, (UCHAR) 208, (UCHAR) 203, (UCHAR) 205, (UCHAR) 210, 
    (UCHAR) 211, (UCHAR) 199, (UCHAR) 207, (UCHAR) 201, (UCHAR) 209,
    (UCHAR) 187, (UCHAR) 188, (UCHAR) 189, (UCHAR) 190, (UCHAR) 191, 
    (UCHAR) 192, (UCHAR) 193, (UCHAR) 194, (UCHAR) 195, (UCHAR) 196,
    (UCHAR) 133, (UCHAR) 134
};
    
/********************/
/* Static variables */
/********************/
static int			stWheelMouseDeltaPerLine;

/******************************/
/* Static callback procedures */
/******************************/

/*********************/
/* Static procedures */
/*********************/
static void	MyConvertMYRECTToRECT (MYRECT *pmMyRect, RECT *pmRect);
static void	MyConvertRECTToMYRECT (RECT *pmMyRect, MYRECT *pmRect);


/***********************/
/* External procedures */
/***********************/
/************************************************************************/
/* MDIOWin_Init								*/
/* 									*/
/* Initialize the modules under EdWin and register the new class	*/
/************************************************************************/
void	MDIOWin_Init (void)
{
    MDIOWinText_Init ();
    MDIOWinGraph_Init ();

    stWheelMouseDeltaPerLine = EdInt_GetMouseWheelSettings ();
} // MDIOWin_Init


/************************************************************************/
/* MDIOWin_Finalize							*/
/************************************************************************/
void	MDIOWin_Finalize (void)
{
    MDIOWinText_Finalize ();
    MDIOWinGraph_Finalize ();
} // MDIOWin_Finalize


/************************************************************************/
/* MDIOWin_AdjustContextualMenu						*/      
/************************************************************************/
void	MDIOWin_AdjustContextualMenu (HMENU pmMenu)
{
    // If we're finished execution, grey out the Resume button.
    if (MIO_finished)
    {
	EnableMenuItem (pmMenu, MIO_COMMAND_PAUSE_RESUME, 
			MF_BYCOMMAND | MF_GRAYED);
    }
    else
    {
	EnableMenuItem (pmMenu, MIO_COMMAND_PAUSE_RESUME, 
			MF_BYCOMMAND | MF_ENABLED);
    }
} // MDIOWin_AdjustContextualMenu


/************************************************************************/
/* MDIOWin_CalculateWindowRect						*/
/************************************************************************/
void	MDIOWin_CalculateWindowRect (MYRECT *pmRect)
{
    RECT	myRect;
    
    MyConvertMYRECTToRECT (pmRect, &myRect);
    AdjustWindowRect (&myRect, WS_OVERLAPPEDWINDOW, FALSE);
    MyConvertRECTToMYRECT (&myRect, pmRect);
} // MDIOWin_CalculateWindowRect

					
/************************************************************************/
/* MDIOWin_CaretHide							*/
/************************************************************************/
void	MDIOWin_CaretHide (WIND pmInnerWindow)
{
    MIOWin_AssertInnerWindow (pmInnerWindow);
    
    HideCaret ((HWND) pmInnerWindow);
} // MDIOWin_CaretHide


/************************************************************************/
/* MDIOWin_CaretMove							*/
/************************************************************************/
void	MDIOWin_CaretMove (WIND pmInnerWindow, int pmX, int pmY)
{
    MIOWin_AssertInnerWindow (pmInnerWindow);
    
    if (pmInnerWindow == MIO_caretOwner)
    {
	SetCaretPos (pmX, pmY);
    }
} // MDIOWin_CaretMove


/************************************************************************/
/* MDIOWin_CaretShow							*/
/************************************************************************/
void	MDIOWin_CaretShow (WIND pmInnerWindow)
{
    MIOWin_AssertInnerWindow (pmInnerWindow);
    
    ShowCaret ((HWND) pmInnerWindow);
} // MDIOWin_CaretShow


/************************************************************************/
/* MDIOWin_CheckForCloseDialog						*/
/************************************************************************/
BOOL	MDIOWin_CheckForCloseDialog (HWND pmWindow)
{
    MIOWinInfoPtr	myInfo = MIOWin_GetInfo (pmWindow);
    int			myResult;
    
    // Display the dialog box and get a response
    myResult = EdInt_Message2 (pmWindow, MB_YESNO | MB_ICONWARNING, 
    			       IDS_MIO_STOP_PROGRAM_TITLE, IDS_MIO_STOP_PROGRAM, 
    			       MIO_programName);

    // Press Cancel, then don't close
    if (myResult == IDNO)
    {
	return FALSE;
    }

    return TRUE;
} // MDIOWin_CheckForCloseDialog


/************************************************************************/
/* MDIOWin_CloseWindow							*/
/************************************************************************/
void	MDIOWin_CloseWindow (WIND pmWindow)
{
    MIOWin_AssertOuterWindow (pmWindow);
    
    SendMessage (pmWindow, WM_CLOSE, 0, 0);
} // MDIOWin_CloseWindow


/************************************************************************/
/* MDIOWin_CopyPaletteFromOldToNewWindow				*/
/************************************************************************/
void	MDIOWin_CopyPaletteFromOldToNewWindow (WIND pmOldWindow, 
					       WIND pmNewWindow)
{
    MIOWinInfoPtr	myOldInfo = MIOWin_GetInfo (pmOldWindow);
    MIOWinInfoPtr	myNewInfo = MIOWin_GetInfo (pmNewWindow);

    if (!myOldInfo -> usingDefaultPalette)
    {
	// Deselect the palette from the old device contexts
    	SelectPalette (myOldInfo -> deviceContext, NULL, FALSE);
    	SelectPalette (myOldInfo -> offscreenDeviceContext, NULL, FALSE);
	// Select the palette into the new device context
	SelectPalette ((HDC) myNewInfo -> deviceContext, myOldInfo -> palette,
		       FALSE);
	SelectPalette ((HDC) myNewInfo -> offscreenDeviceContext, 
		       myOldInfo -> palette, FALSE);
	// Move the palette over
	myNewInfo -> palette = myOldInfo -> palette;
	myOldInfo -> palette = NULL;
	myOldInfo -> usingDefaultPalette = TRUE;
	myNewInfo -> usingDefaultPalette = FALSE;
    }
} // MDIOWin_CopyPaletteFromOldToNewWindow


/************************************************************************/
/* MDIOWin_CreateDestroyCaret						*/
/************************************************************************/
void	MDIOWin_CreateDestroyCaret (WIND pmInnerWindow, BOOL pmCreate)
{
    MIOWinInfoPtr	myInfo = MIOWin_GetInfo (pmInnerWindow);
    int			myCaretWidth;

    if (MDIOWin_GetWindowType (pmInnerWindow) == WINDOW_TYPE_MIO_GRAPHICS_INNER)
    {
	myCaretWidth = myInfo -> fontCharWidth;
    }
    else
    {
	if (MIOWin_GetProperties () -> caretWidth == 3)
        {
    	    myCaretWidth = myInfo -> fontCharWidth;
	}
	else
	{
	    myCaretWidth = MIOWin_GetProperties () -> caretWidth;
	}
    }

    if (pmCreate)
    {
	CreateCaret (pmInnerWindow, NULL, myCaretWidth, 
		     myInfo -> fontCharHeight);
	MIO_caretOwner = pmInnerWindow;

	// Display the caret (if appropriate), in the new location    
	MIOWin_CaretDisplay (pmInnerWindow);
    }
    else
    {
	MIOWin_CaretHide (pmInnerWindow);
	MIO_caretOwner = NULL;
	DestroyCaret ();
    }
} // MDIOWin_CreateDestroyCaret


/************************************************************************/
/* MDIOWin_DestroyWindow						*/
/************************************************************************/
void	MDIOWin_DestroyWindow (WIND pmWindow)
{
    MIOWin_AssertOuterWindow (pmWindow);

    // Hide the window before destroying it.
    MDIOWin_HideWindow (pmWindow);
        
    DestroyWindow (pmWindow);
} // MDIOWin_DestroyWindow


/************************************************************************/
/* MDIOWin_Dispose							*/
/************************************************************************/
void	MDIOWin_Dispose (MIOWinInfoPtr pmInfo)
{
    // Get rid of the cached pen, if necessary
    if (pmInfo -> cachedPenColour != NO_COLOUR)
    {
    	HPEN	myPen;
    	
    	myPen = SelectPen (pmInfo -> deviceContext, NULL);
	DeletePen (myPen);
    }

    // Get rid of cached brush, if necessary
    if (pmInfo -> cachedBrushColour != NO_COLOUR)
    {
    	HBRUSH	myBrush;
    	
    	myBrush = SelectBrush (pmInfo -> deviceContext, NULL);
	DeleteBrush (myBrush);
    }

    // Delete the palette, if necessary
    if (!pmInfo -> usingDefaultPalette)
    {
    	SelectPalette (pmInfo -> deviceContext, NULL, FALSE);
    	SelectPalette (pmInfo -> offscreenDeviceContext, NULL, FALSE);
    	DeletePalette (pmInfo -> palette);
    }

    // Delete the clipping region, if necessary
    if (pmInfo -> clippingRegion != NULL)
    {
    	SelectClipRgn (pmInfo -> deviceContext, NULL); 
    	SelectClipRgn (pmInfo -> offscreenDeviceContext, NULL); 
    	DeleteObject (pmInfo -> clippingRegion);
    }

    // Delete the text, if necessary
    if (pmInfo -> text != NULL)
    {
    	MDIOWinText_DisposeTextRecord (pmInfo -> outerWindow);
    }
    
    // Release the device context
    if (pmInfo -> deviceContext != NULL)
    {
    	ReleaseDC (pmInfo -> innerWindow, pmInfo -> deviceContext);
    }
    
    // Release the offscreen device context
    if (pmInfo -> offscreenDeviceContext != NULL)
    {
    	DeleteDC (pmInfo -> offscreenDeviceContext);
    }
    
    // Release the offscreen bitmap
    if (pmInfo -> offscreenBitMap != NULL)
    {
    	DeleteObject (pmInfo -> offscreenBitMap);
    }

    // Release the offscreen device context
    if (pmInfo -> spriteDeviceContext != NULL)
    {
    	DeleteDC (pmInfo -> spriteDeviceContext);
    }
    
    // Release the offscreen bitmap
    if (pmInfo -> spriteBitMap != NULL)
    {
    	DeleteObject (pmInfo -> spriteBitMap);
    }

    // Mark all the sprites as not using this window
    MIOSprite_RemoveSpritesFromWindow (pmInfo);
} // MDIOWin_Dispose


/************************************************************************/
/* MDIOWin_GetActiveWindow						*/
/************************************************************************/
WIND	MDIOWin_GetActiveWindow (void)
{
    return GetActiveWindow ();
} // MDIOWin_GetActiveWindow


/************************************************************************/
/* MDIOWin_GetClipboardBuffer						*/
/*									*/
/* The contents of the clipboard is returned in an allocated buffer.	*/
/* It is the responsibility of the caller to free the buffer when no	*/
/* longer needed.							*/
/************************************************************************/
char	*MDIOWin_GetClipboardBuffer (WIND pmWindow)
{
    HGLOBAL		myGlobalHandle;
    int			myBufferLen;
    char		*myBuffer;
    char		*myGlobalPtr;

    MIOWin_AssertOuterWindow (pmWindow);
    
    OpenClipboard ((HWND) pmWindow);
    myGlobalHandle = GetClipboardData (CF_TEXT);
    if (myGlobalHandle == NULL)
    {
    	MDIO_Beep ();
    	CloseClipboard ();
    	return NULL;
    }
    myBufferLen = GlobalSize (myGlobalHandle);
    myBuffer = malloc (myBufferLen);
    if (myBuffer == NULL)
    {
    	MDIO_Beep ();
    	CloseClipboard ();
    	return NULL;
    }
    myGlobalPtr = GlobalLock (myGlobalHandle);
    strcpy (myBuffer, myGlobalPtr);
    GlobalUnlock (myGlobalHandle);
    CloseClipboard ();
    
    return myBuffer;
} // MDIOWin_GetClipboardBuffer


/************************************************************************/
/* MDIOWin_GetDoubleClickTime						*/
/************************************************************************/
UINT	MDIOWin_GetDoubleClickTime (void)
{
    return GetDoubleClickTime ();
} // MDIOWin_GetDoubleClickTime


/************************************************************************/
/* MDIOWin_GetHScrollBarWidth						*/
/************************************************************************/
int	MDIOWin_GetHScrollBarWidth (void)
{
    return GetSystemMetrics (SM_CYHSCROLL);
} // MDIOWin_GetHScrollBarWidth


/************************************************************************/
/* MDIOWin_GetMessageTime						*/
/************************************************************************/
long	MDIOWin_GetMessageTime (void)
{
    return GetMessageTime ();
} // MDIOWin_GetMessageTime


/************************************************************************/
/* MDIOWin_GetParent							*/
/************************************************************************/
WIND	MDIOWin_GetParent (WIND pmWindow)
{
    return GetParent ((HWND) pmWindow);
} // MDIOWin_GetParent


/************************************************************************/
/* MDIOWin_GetTitleBarHeight						*/
/************************************************************************/
int	MDIOWin_GetTitleBarHeight (void)
{
    return GetSystemMetrics (SM_CYCAPTION) + 4;
} // MDIOWin_GetTitleBarHeight


/************************************************************************/
/* MDIOWin_GetVScrollBarWidth						*/
/************************************************************************/
int	MDIOWin_GetVScrollBarWidth (void)
{
    return GetSystemMetrics (SM_CXVSCROLL);
} // MDIOWin_GetVScrollBarWidth


/************************************************************************/
/* MDIOWin_GetWheelMouseDeltaPerLine					*/
/************************************************************************/
int	MDIOWin_GetWheelMouseDeltaPerLine (void)
{
    return stWheelMouseDeltaPerLine;
} // MDIOWin_GetWheelMouseDeltaPerLine


/************************************************************************/
/* MDIOWin_GetWindowRect						*/
/************************************************************************/
void	MDIOWin_GetWindowRect (WIND pmWindow, MYRECT *pmRect)
{
    RECT    myRect;

    GetWindowRect (pmWindow, &myRect);
    MyConvertRECTToMYRECT (&myRect, pmRect);
} // MDIOWin_GetWindowRect


/************************************************************************/
/* MDIOWin_GetWindowTitle						*/
/************************************************************************/
void	MDIOWin_GetWindowTitle (WIND pmWindow, char *pmTitle)
{
    GetWindowText (pmWindow, pmTitle, 255);
} // MDIOWin_GetWindowTitle


/************************************************************************/
/* MDIOWin_GetWindowType						*/
/*									*/
/* Return the type of the window.  Should be one of:			*/
/*	WINDOW_TYPE_CONSOLE_TEXT					*/
/*	WINDOW_TYPE_CONSOLE_GRAPHICS					*/
/*	WINDOW_TYPE_CONSOLE_TEXT_PARENT					*/
/*	WINDOW_TYPE_CONSOLE_GRAPHICS_PARENT				*/
/************************************************************************/
int	MDIOWin_GetWindowType (WIND pmWindow)
{
    return GetWindowLong ((HWND) pmWindow, WINDOW_MEM_WINDOW_TYPE);
} // MDIOWin_GetWindowType


/************************************************************************/
/* MDIOWin_GetWindowInfo						*/
/*									*/
/* Return the information field about the window.  Assumes that the	*/
/* window type is one of the four mentioned in MDIOWin_GetWindowType.*/
/************************************************************************/
void	*MDIOWin_GetWindowInfo (WIND pmWindow)
{
    void	*myResult;
    
    myResult = (void *) GetWindowLong ((HWND) pmWindow, WINDOW_MEM_WINDOW_INFO);
    if (myResult == NULL)
    {
    	EdInt_HaltEnvironment (IDS_MIO_GETWINDOWLONGFAIL, __FILE__, __LINE__,
	    		       GetLastError (), WINDOW_MEM_WINDOW_INFO);
    }
    
    return myResult;
} // MDIOWin_GetWindowInfo


/************************************************************************/
/* MDIOWin_HandleControlKey						*/
/************************************************************************/
BOOL	MDIOWin_HandleControlKey (WIND pmOuterWindow, int pmKeyCode)
{
    BOOL	myShiftState = ((GetKeyState (VK_SHIFT) & 0x8000) != 0);
    BOOL	myCtrlState = ((GetKeyState (VK_CONTROL) & 0x8000) != 0);
    BOOL	myIsTextWindow = MIOWin_IsTextWindow (pmOuterWindow);

    if (!myShiftState && !myCtrlState && (pmKeyCode == VK_F1))
    {
	SendMessage (pmOuterWindow, WM_COMMAND, MIO_COMMAND_RERUN, 0);
    	return TRUE;
    }

    if (!myCtrlState || (pmKeyCode == VK_CONTROL))
    {
    	return FALSE;
    }

    // Check for Control keys
    if (pmKeyCode == 'N')
    {
	SendMessage (pmOuterWindow, WM_COMMAND, MIO_COMMAND_NEW_EDITOR_WIN, 0);
    	return TRUE;
    }
    else if (pmKeyCode == 'O')
    {
	SendMessage (pmOuterWindow, WM_COMMAND, MIO_COMMAND_OPEN_EDITOR_WIN, 0);
    	return TRUE;
    }
    else if (pmKeyCode == 'W')
    {
	SendMessage (pmOuterWindow, WM_COMMAND, MIO_COMMAND_CLOSE, 0);
    	return TRUE;
    }
    else if (pmKeyCode == 'Q')
    {
	SendMessage (pmOuterWindow, WM_COMMAND, MIO_COMMAND_EXIT, 0);
    	return TRUE;
    }
    else if (pmKeyCode == 'R')
    {
	SendMessage (pmOuterWindow, WM_COMMAND, MIO_COMMAND_RERUN, 0);
    	return TRUE;
    }
    else if (pmKeyCode == 'P')
    {
	SendMessage (pmOuterWindow, WM_COMMAND, MIO_COMMAND_PRINT_FILE, 0);
    	return TRUE;
    }
    else if (pmKeyCode == 'S')
    {
	SendMessage (pmOuterWindow, WM_COMMAND, MIO_COMMAND_SAVEAS_FILE, 0);
    	return TRUE;
    }
    else if ((pmKeyCode == 'A') && myIsTextWindow)
    {
	SendMessage (pmOuterWindow, WM_COMMAND, MIO_COMMAND_SELECT_ALL, 0);
    	return TRUE;
    }
    else if ((pmKeyCode == 'C') && myIsTextWindow)
    {
	SendMessage (pmOuterWindow, WM_COMMAND, MIO_COMMAND_COPY, 0);
    	return TRUE;
    }
    
    return FALSE;
} // MDIOWin_HandleControlKey


/************************************************************************/
/* MDIOWin_HideWindow							*/
/************************************************************************/
void	MDIOWin_HideWindow (WIND pmWindow)
{
    MIOWin_AssertOuterWindow (pmWindow);
    
    if (IsWindowVisible ((HWND) pmWindow))
    {
	EdInt_NotifyRunWindowClosed (pmWindow);
	ShowWindow ((HWND) pmWindow, SW_HIDE);
    }
} // MDIOWin_HideWindow


/************************************************************************/
/* MDIOWin_IsVisible							*/
/************************************************************************/
BOOL	MDIOWin_IsVisible (WIND pmWindow)
{
    return IsWindowVisible (pmWindow);
} // MDIOWin_IsVisible


/************************************************************************/
/* MDIOWin_ResizeInnerWindow						*/
/************************************************************************/
void	MDIOWin_ResizeInnerWindow (WIND pmInnerWindow)
{
    RECT		myWindowRect;
    MIOWinInfoPtr	myInfo;

    // The first resizing comes before there's a client area, so
    // we put this check here to ignore it
    if (GetClientRect ((HWND) pmInnerWindow, &myWindowRect) == 0)
    {
    	return;
    }
    
    // If the window hasn't really been resized, return immediately 
    if ((myWindowRect.bottom == 0) && (myWindowRect.right == 0) &&
        (myWindowRect.top == 0) && (myWindowRect.left == 0))
    {
    	return;
    }
    
    // Get the window information
    myInfo = MIOWin_GetInfo (pmInnerWindow);
    
    // Set the text rectangle
    myInfo -> textRect.top = myWindowRect.top + MARGIN;
    myInfo -> textRect.left = myWindowRect.left + MARGIN;
    myInfo -> textRect.bottom = myWindowRect.bottom;
    myInfo -> textRect.right = myWindowRect.right;

    // Set the window width and height        
    myInfo -> windowWidth = (myInfo -> textRect.right - 
        myInfo -> textRect.left) / myInfo -> fontCharWidth;
    myInfo -> windowHeight = (myInfo -> textRect.bottom - 
        myInfo -> textRect.top) / myInfo -> fontCharHeight;
    
    if (MIOWin_IsTextWindow (pmInnerWindow))
    {
    	MDIOWinText_SynchScrollBarsToText (pmInnerWindow);
    }
    else
    {
    	MDIOWinGraph_SynchScrollBarsToGraphics (pmInnerWindow);
    }

    MIOWin_CaretDisplay (pmInnerWindow);
} // MDIOWin_ResizeInnerWindow


/************************************************************************/
/* MDIOWin_ResizeRunWindow						*/
/************************************************************************/
void	MDIOWin_ResizeRunWindow (WIND pmWindow)
{
    RECT		myWindowRect;
    MIOWinInfoPtr	myInfo;
    int			myWindowWidth;
    int			myTopFrameBorder;
    HDWP		myDeferWindowPosHandle;

    MIOWin_AssertOuterWindow (pmWindow);
    
    // The first resizing comes before there's a client area, so
    // we put this check here to ignore it
    if (GetClientRect (pmWindow, &myWindowRect) == 0)
    {
    	return;
    }
    
    // Get the window infor    
    myInfo = MIOWin_GetInfo (pmWindow);
    
    // If the window hasn't really been resized, return immediately 
    if ((myWindowRect.bottom == myInfo -> oldWindowRect.bottom) && 
        (myWindowRect.right == myInfo -> oldWindowRect.right) &&
        (myWindowRect.top == myInfo -> oldWindowRect.top) &&
        (myWindowRect.left == myInfo -> oldWindowRect.left))
    {
    	return;
    }
    
    // If we've been iconisized, ignore the resizing
    if (IsIconic (pmWindow))
    {
    	return;
    }

    // Save the old window size    
    myInfo -> oldWindowRect.top = myWindowRect.top;
    myInfo -> oldWindowRect.left = myWindowRect.left;
    myInfo -> oldWindowRect.bottom = myWindowRect.bottom;
    myInfo -> oldWindowRect.right = myWindowRect.right;
    
    myWindowWidth = myWindowRect.right - myWindowRect.left;
    
    //
    // Resize the child windows
    //
    myDeferWindowPosHandle = BeginDeferWindowPos (5);
    if (myDeferWindowPosHandle == NULL)
    {
        EdInt_HaltEnvironment (IDS_MIO_NO_MEM_FOR_RESIZE, __FILE__, __LINE__, 
		               GetLastError ());
    }

    //
    // Resize the button bar, if it exists
    //
    if (myInfo -> buttonBarWindow != NULL)
    {
    	myTopFrameBorder = MDIOWinTop_GetWindowHeight ();
    	DeferWindowPos (myDeferWindowPosHandle, myInfo -> buttonBarWindow, 
    	    HWND_TOP, 0, 0, myWindowRect.right, myTopFrameBorder, 
    	    SWP_NOACTIVATE);
    }
    else
    {
    	myTopFrameBorder = 0;
    }
    
    //
    // Resize the text display window
    //
    DeferWindowPos (myDeferWindowPosHandle, 
    	myInfo -> innerWindow,  HWND_TOP, 0, myTopFrameBorder, 
    	myWindowRect.right, myWindowRect.bottom - myTopFrameBorder, 
    	SWP_NOACTIVATE);
        
    if (!EndDeferWindowPos (myDeferWindowPosHandle))
    {
        EdInt_HaltEnvironment (IDS_MIO_NO_RESIZE, __FILE__, __LINE__, 
        		       GetLastError ());
    }
} // MDIOWin_ResizeRunWindow


/************************************************************************/
/* MDIOWin_SetActiveWindow						*/
/************************************************************************/
void	MDIOWin_SetActiveWindow (WIND pmWindow)
{
    SetActiveWindow ((HWND) pmWindow);
} // MDIOWin_SetActiveWindow


/************************************************************************/
/* MDIOWin_SetDisplayFonts						*/
/************************************************************************/
BOOL	MDIOWin_SetDisplayFonts (WIND pmWindow, const char *pmFontName,
				 int pmFontSize, int *pmFontHeight,
				 int *pmFontWidth, int *pmFontAscent, 
				 int *pmFontDescent, int *pmFontIntLeading,
				 FONT *pmFont, COLOR pmDesiredColour, 
				 COLOR *pmColour)
{
    HINSTANCE	myApplicationInstance;
    HDC		myDisplayContext;
    TEXTMETRIC	myTextMetrics;
    
    MIOWin_AssertOuterWindow (pmWindow);
    
    // Get the font name
    myApplicationInstance = (HINSTANCE) GetWindowLong ((HWND) pmWindow, 
        GWL_HINSTANCE);

    // Delete any previously existing font
    if (*pmFont != NULL)
    {
    	DeleteObject (pmFont);
    }
    
    // Create the new font
    *pmFont = EdInt_CreateFont (pmWindow, pmFontName, pmFontSize, 
			        NO_BOLD, NO_ITALIC);
    if (*pmFont == NULL)
    {
    	EdInt_FailWarn (IDS_MIO_CREATEFONTFAIL, __FILE__, __LINE__, 
    			GetLastError ());
    	return FALSE;
    }

    myDisplayContext = GetDC (pmWindow);
    SelectFont (myDisplayContext, *pmFont);
    GetTextMetrics (myDisplayContext, &myTextMetrics);
    *pmFontHeight = myTextMetrics.tmHeight;
    *pmFontWidth = myTextMetrics.tmAveCharWidth;
    *pmFontAscent = myTextMetrics.tmAscent;
    *pmFontDescent = myTextMetrics.tmDescent;
    *pmFontIntLeading = myTextMetrics.tmInternalLeading;
        
    // Initialize the selection colour		     
    *pmColour = (COLOR) GetNearestColor (myDisplayContext, 
			        (COLORREF) pmDesiredColour);

    ReleaseDC (pmWindow, myDisplayContext);
    
    return TRUE;
} // MDIOWin_SetDisplayFonts


/************************************************************************/
/* MDIOWin_SetWindowInfo						*/
/************************************************************************/
void	MDIOWin_SetWindowInfo (WIND pmWindow, void *pmInfoPtr)
{
    SetWindowLong (pmWindow, WINDOW_MEM_WINDOW_INFO, (long) pmInfoPtr);
} // MDIOWin_SetWindowInfo


/************************************************************************/
/* MDIOWin_SetWindowInitialization					*/
/************************************************************************/
void	MDIOWin_SetWindowInitialization (WIND pmWindow, BOOL pmInitialized)
{
    SetWindowLong (pmWindow, WINDOW_MEM_WINDOW_INITIALIZED, 
    		   (long) pmInitialized);
} // MDIOWin_SetWindowInitialization


/************************************************************************/
/* MDIOWin_SetWindowInitialization					*/
/************************************************************************/
void	MDIOWin_SetWindowPosition (WIND pmWindow, int pmXPos, int pmYPos,
				      int pmWidth, int pmHeight)
{
    if (pmWidth == 0)
    {
    	SetWindowPos (pmWindow, HWND_TOP, pmXPos, pmYPos, 0, 0, SWP_NOSIZE);
    }
    else
    {
	int x = 0;
	x = IsWindow (pmWindow);
    	SetWindowPos (pmWindow, HWND_TOP, pmXPos, pmYPos, pmWidth, pmHeight, 0);
    }
} // MDIOWin_SetWindowInitialization


/************************************************************************/
/* MDIOWin_SetWindowTitle						*/
/************************************************************************/
void	MDIOWin_SetWindowTitle (WIND pmWindow, const char *pmTitle)
{
    MIOWin_AssertOuterWindow (pmWindow);
    
    SetWindowText ((HWND) pmWindow, pmTitle);
} // MDIOWin_SetWindowTitle


/************************************************************************/
/* MDIOWin_SetWindowType						*/
/************************************************************************/
void	MDIOWin_SetWindowType (WIND pmWindow, int pmWindowType)
{
    SetWindowLong (pmWindow, WINDOW_MEM_WINDOW_TYPE, (long) pmWindowType);
} // MDIOWin_SetWindowType


/************************************************************************/
/* MDIOWin_ShowWindow							*/
/************************************************************************/
void	MDIOWin_ShowWindow (WIND pmWindow)
{
    MIOWin_AssertOuterWindow (pmWindow);
    
    if (!IsWindowVisible ((HWND) pmWindow))
    {
	ShowWindow ((HWND) pmWindow, SW_SHOWNORMAL);
	EdInt_NotifyRunWindowAdded (pmWindow, "");
    }
} // MDIOWin_ShowWindow


/************************************************************************/
/* MDIOWin_TranslateAltKeystroke					*/
/************************************************************************/
UCHAR	MDIOWin_TranslateAltKeystroke (int pmChar)
{
    if (pmChar == '1') return 248;
    if (pmChar == '2') return 249;
    if (pmChar == '3') return 250;
    if (pmChar == '4') return 251;
    if (pmChar == '5') return 252;
    if (pmChar == '6') return 253;
    if (pmChar == '7') return 254;
    if (pmChar == '8') return 255;
    if (pmChar == '9') return 242;
    if (pmChar == '0') return 129;
    if (pmChar == '-') return 130;
    if (pmChar == '=') return 131;
    if (pmChar == 'a') return 158;
    if (pmChar == 'b') return 176;
    if (pmChar == 'c') return 174;
    if (pmChar == 'd') return 160;
    if (pmChar == 'e') return 146;
    if (pmChar == 'f') return 161;
    if (pmChar == 'g') return 162;
    if (pmChar == 'h') return 163;
    if (pmChar == 'i') return 151;
    if (pmChar == 'j') return 164;
    if (pmChar == 'k') return 165;
    if (pmChar == 'l') return 166;
    if (pmChar == 'm') return 178;
    if (pmChar == 'n') return 177;
    if (pmChar == 'o') return 152;
    if (pmChar == 'p') return 153;
    if (pmChar == 'q') return 144;
    if (pmChar == 'r') return 147;
    if (pmChar == 's') return 159;
    if (pmChar == 't') return 148;
    if (pmChar == 'u') return 150;
    if (pmChar == 'v') return 175;
    if (pmChar == 'w') return 145;
    if (pmChar == 'x') return 173;
    if (pmChar == 'y') return 149;
    if (pmChar == 'z') return 172;
    return 0;
}


/************************************************************************/
/* MDIOWin_TranslateSpecialKeystroke					*/
/************************************************************************/
UCHAR	MDIOWin_TranslateSpecialKeystroke (int pmWindowsKeycode)
{
    UCHAR	myTuringKeycode;
    BOOL	myShiftState = ((GetKeyState (VK_SHIFT) & 0x8000) != 0);
    BOOL	myCtrlState = ((GetKeyState (VK_CONTROL) & 0x8000) != 0);
    BOOL	myAltState = ((GetKeyState (VK_MENU) & 0x8000) != 0);
    int		cnt;

    myTuringKeycode = 0;

    // Cover Up Arrow, Down Arrow, Left Arrow, Right Arrow, Insert,
    // Delete, Home, End, PgUp, PgDn, F1-F10, Shift+F1-F10, Ctrl+F1-F10, 
    // Alt+F1-F10

    for (cnt = 0 ; cnt < NUM_SPEC_KEYS ; cnt++)
    {
	if (pmWindowsKeycode == stWindowsKeycode [cnt])
	{
	    myTuringKeycode = stTuringKeycode [cnt];
	    break;
	}
    }

    if (myShiftState)
    {
	if (pmWindowsKeycode == VK_SHIFT) return 0;
	if ((FUNC_KEY_BASE <= myTuringKeycode) &&
	    (myTuringKeycode <= FUNC_KEY_BASE + 10))
	{
	    myTuringKeycode += ADD_FOR_SHIFT;
	}
	else if (pmWindowsKeycode == VK_F11) myTuringKeycode = 135;
	else if (pmWindowsKeycode == VK_F12) myTuringKeycode = 136;
	else if (pmWindowsKeycode == VK_TAB) myTuringKeycode = 143;
    }
    else if (myCtrlState)
    {
	if (pmWindowsKeycode == VK_CONTROL) return 0;
	if ((FUNC_KEY_BASE <= myTuringKeycode) &&
	    (myTuringKeycode <= FUNC_KEY_BASE + 10))
	{
	    myTuringKeycode += ADD_FOR_CTRL;
	}
	else if (pmWindowsKeycode == VK_PRIOR) myTuringKeycode = 132;
	else if (pmWindowsKeycode == VK_F11) myTuringKeycode = 137;
	else if (pmWindowsKeycode == VK_F12) myTuringKeycode = 138;
	else if (pmWindowsKeycode == VK_UP) myTuringKeycode = 141;
	else if (pmWindowsKeycode == VK_DOWN) myTuringKeycode = 145;
	else if (pmWindowsKeycode == VK_INSERT) myTuringKeycode = 146;
	else if (pmWindowsKeycode == VK_DELETE) myTuringKeycode = 147;
	else if (pmWindowsKeycode == VK_LEFT) myTuringKeycode = 243;
	else if (pmWindowsKeycode == VK_RIGHT) myTuringKeycode = 244;
	else if (pmWindowsKeycode == VK_END) myTuringKeycode = 245;
	else if (pmWindowsKeycode == VK_NEXT) myTuringKeycode = 246;
	else if (pmWindowsKeycode == VK_HOME) myTuringKeycode = 247;
    }
    else if (myAltState)
    {
	if (pmWindowsKeycode == VK_MENU) return 0;
	if ((FUNC_KEY_BASE <= myTuringKeycode) &&
	    (myTuringKeycode <= FUNC_KEY_BASE + 10))
	{
	    myTuringKeycode += ADD_FOR_ALT;
	}
	else if (pmWindowsKeycode == VK_F11) myTuringKeycode = 139;
	else if (pmWindowsKeycode == VK_F12) myTuringKeycode = 140;
    }

    return myTuringKeycode;
} // MDIOWin_TranslateSpecialKeystroke


/************************************************************************/
/* MDIOWin_Update							*/
/************************************************************************/
void	MDIOWin_Update (WIND pmWindow)
{
    RedrawWindow ((HWND) pmWindow, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
} // MDIOWin_Update


/************************************************************************/
/* MDIOWin_UpdateArea							*/
/************************************************************************/
void	MDIOWin_UpdateArea (WIND pmWindow, int x1, int y1, int x2, int y2)
{
    RECT	myRect;
    
    myRect.top = min (y1, y2) + MARGIN;
    myRect.bottom = max (y1, y2) + MARGIN + 1;
    myRect.left = min (x1, x2) + MARGIN;
    myRect.right = max (x1, x2) + MARGIN + 1;
	    
    RedrawWindow ((HWND) pmWindow, &myRect, NULL, 
    	RDW_INVALIDATE | RDW_UPDATENOW);
} // MDIOWin_UpdateArea


/************************************************************************/
/* MDIOWin_WindowProcedure						*/
/************************************************************************/
long	MDIOWin_WindowProcedure (HWND pmWindow, UINT pmMessage, 
				 WPARAM pmWParam, LPARAM pmLParam,
				 BOOL pmIsGraphicsWindow)
{
    WindowAttribPtr	myWindowAttribs;
    MIOWinInfoPtr	myInfo;
    static BOOL		myStAppActive;
    
    // Log the windows messages
    if (MIOWin_GetProperties () -> logWindowMessages)
    {
//	EdLog_PrintWindowsMessage ("Edit", pmWindow, pmMessage, 
//	    pmWParam, pmLParam);
    }
    
    switch (pmMessage)
    {
    	case WM_CREATE:
	    if (MIOWin_GetProperties () -> logWindowMessages)
	    {
//	    	EdLog_Log ("Edit Window = %x  (Starting Creation)", 
//	    	    pmWindow);
	    }
	    myWindowAttribs = (WindowAttribPtr)
	    	(((LPCREATESTRUCT) pmLParam) -> lpCreateParams);

	    return MIOWin_InitializeWindow (pmWindow, myWindowAttribs);

        case WM_CLOSE:
            // This gets called when the program is finished and the user
            // hits the button bar close button or the program is not finished
	    // and the user presses the close box.
            if (pmWParam || MIOWin_CheckForClose (pmWindow))
            {
	    	MDIOWin_HideWindow (pmWindow);
            	break;
            }
            return 0;

	case WM_DESTROY:
	    myInfo = MIOWin_GetInfo (pmWindow);
	    // Hide the window before removing it
	    MDIOWin_HideWindow (pmWindow);
	    // Remove the window to the list of MIO windows
	    MIO_RemoveFromRunWindowList (pmWindow);
	    // If we're still running, stop execution if the window has not
	    // been flagged as okay to close.
	    if (!MIO_finished && !myInfo -> okayToClose)
	    {
		EdInt_StopExecution ();
		if (!MIO_paused)
		{
		    // TW: Close all open run windows
		}
	    }
	    
	    // Tell the debugger that the window is closed
	    if (myInfo -> turingWindowID != WINDOW_CHANGED_NOT_CLOSED)
	    {
	    	MIOWindow_RegisterClose (myInfo -> turingWindowID);
	    }
	    
	    // Get rid of allocated resources and memory
	    MIOWin_Dispose (pmWindow);
	    break;
	    	    
	case WM_SIZE:
	    MDIOWin_ResizeRunWindow (pmWindow);
	    return 0;
	    
        case WM_COMMAND:
            if (MIOWin_CommandHandler (pmWindow, LOWORD (pmWParam)))
            {
                return 0;
            }
            EdInt_HaltEnvironment (IDS_MIO_COMMAND_NOT_PROCESSED, __FILE__, 
            	__LINE__, 0, LOWORD (pmWParam));
	    break; // Never reaches here            	

	case WM_WINDOWPOSCHANGING:
	    // If the window who's position is changing is the topmost topmost 
	    // window, then don't do anything special.  If the window who's 
	    // position is changing is "a" topmost window, but not the
	    // topmost topmost window, then set all the topmost windows
	    // above it and SetForegroundWindow the topmost topmost window.
	    // If the window who's position is changing is not a topmost window,
	    // then set all the topmost windows and this window and then
	    // SetForegroundWindow the topmost window.  In all cases, 
	    // unminimize the topmost window if necessary.
	    {
	    	MIOWinInfoPtr	myInfo;
		WINDOWPOS	*myWindowPos;
	    	
		myWindowPos = (WINDOWPOS *) pmLParam;
		if ((myWindowPos -> flags & SWP_NOACTIVATE) == 0)
		{
		    myInfo = MIO_GetTopMostWindow ();
		    if ((myInfo != NULL) && (myInfo -> outerWindow != pmWindow))
		    {
			PostMessage (pmWindow, WM_SETTURINGRUNWINDOWPOS, 0, (LPARAM) pmWindow);
			myWindowPos -> flags |= SWP_NOZORDER | SWP_NOACTIVATE;
		    }
		}
//		EdLog_Log ("WINDOWPOS hwnd=%x hwnInsertAfter=%x flags=%d", 
//		    x ->hwnd, x->hwndInsertAfter, x->flags);
	    }
	    break;

	case WM_SETTURINGRUNWINDOWPOS:
	    {
		HWND	myPrevWindow;

		myInfo = MIO_ListTopMostWindows (TRUE);
		if ((myInfo != NULL) && (myInfo -> outerWindow != pmWindow))
		{
		    if (IsIconic (myInfo -> outerWindow))
		    {
			OpenIcon (myInfo -> outerWindow);
		    }
		    SetForegroundWindow (myInfo -> outerWindow);

		    // Now position all the other windows behind them
		    
		    myPrevWindow = myInfo -> outerWindow;
		    while (TRUE)
		    {
			myInfo = MIO_ListTopMostWindows (FALSE);
			if ((myInfo == NULL) || 
			    (myInfo -> outerWindow == pmWindow))
			{
			    break;
			}
			SetWindowPos (myInfo -> outerWindow, myPrevWindow, 
				      0, 0, 0, 0,
				      SWP_NOSIZE | SWP_NOMOVE | 
				      SWP_NOACTIVATE | SWP_NOSENDCHANGING);
			myPrevWindow = myInfo -> outerWindow;
		    } // while
		    if (myInfo == NULL)
		    {
			SetWindowPos (pmWindow, myPrevWindow,  0, 0, 0, 0,
				      SWP_NOSIZE | SWP_NOMOVE | 
				      SWP_NOACTIVATE | SWP_NOSENDCHANGING);
		    }
		}
	    }
	    break;

        case WM_ACTIVATE:
            if (LOWORD (pmWParam) != WA_INACTIVE)
            {
            	SetFocus (MIOWin_GetInfo (pmWindow) -> innerWindow);
            }
	    if (!MIOWin_GetInfo (pmWindow) -> displayOnScreen)
	    {
		MDIOWin_Update (MIOWin_GetInfo (pmWindow) -> innerWindow);
	    }
	    // Clear all the key presses
	    ZeroMemory (MIOWin_GetInfo (pmWindow) -> pressedKeys, 
			256 * sizeof (BYTE));
            return 0;
            
        case WM_SETFOCUS:
            SetFocus (MIOWin_GetInfo (pmWindow) -> innerWindow);
            return 0;	

	case WM_GETMINMAXINFO:
	    if (GetWindowLong (pmWindow, WINDOW_MEM_WINDOW_INITIALIZED))
	    {
	    	MINMAXINFO	*myMinMaxInfo = (MINMAXINFO *) pmLParam;
	    	MIOWinInfoPtr	myInfo;
	    	
		myInfo = MIOWin_GetInfo (pmWindow);

	    	myMinMaxInfo -> ptMinTrackSize.x = myInfo -> minWindowWidth;
	    	myMinMaxInfo -> ptMinTrackSize.y = myInfo -> minWindowHeight;
		if (pmIsGraphicsWindow)
		{
	    	    myMinMaxInfo -> ptMaxTrackSize.x = myInfo -> maxWindowWidth;
	    	    myMinMaxInfo -> ptMaxTrackSize.y = myInfo -> maxWindowHeight;
	    	    myMinMaxInfo -> ptMaxPosition.x = myInfo -> initWindowLeft;
	    	    myMinMaxInfo -> ptMaxPosition.y = myInfo -> initWindowTop;
	    	    myMinMaxInfo -> ptMaxSize.x = myInfo -> maxWindowWidth;
	    	    myMinMaxInfo -> ptMaxSize.y = myInfo -> maxWindowHeight;
		}
	    	  
	    	return 0;
	    }
	    break;
        
        case WM_QUERYNEWPALETTE:
            if (pmIsGraphicsWindow)
            {
            	MIOWinInfoPtr	myInfo = MIOWin_GetInfo (pmWindow);
            	
            	SelectPalette (myInfo -> deviceContext, myInfo -> palette,
			       FALSE);
            	SelectPalette (myInfo -> offscreenDeviceContext, 
            		       myInfo -> palette, FALSE);
		if (RealizePalette (myInfo -> deviceContext) > 0)
		{
		    RealizePalette (myInfo -> offscreenDeviceContext);
		    InvalidateRect (pmWindow, NULL, TRUE);
		}
		return TRUE;
	    }
	    break;
	    
	case WM_PALETTECHANGED:
	    if ((pmWindow != (HWND) pmWParam) && pmIsGraphicsWindow)
	    {
            	MIOWinInfoPtr	myInfo = MIOWin_GetInfo (pmWindow);
            	
            	SelectPalette (myInfo -> deviceContext, myInfo -> palette,
			       FALSE);
            	SelectPalette (myInfo -> offscreenDeviceContext, 
            		       myInfo -> palette, FALSE);
		RealizePalette (myInfo -> deviceContext);			       
		RealizePalette (myInfo -> offscreenDeviceContext);
		UpdateColors (myInfo -> deviceContext);
	    }
	    break;
	    
        case WM_SETTINGCHANGE:
	    // Set wheel mouse settings
	    stWheelMouseDeltaPerLine = EdInt_GetMouseWheelSettings ();
            return 0;
    }
    
    return DefWindowProc (pmWindow, pmMessage, pmWParam, pmLParam);
} // MDIOWin_WindowProcedure


/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/
/************************************************************************/
/* MyConvertMYRECTToRECT						*/
/************************************************************************/
static void	MyConvertMYRECTToRECT (MYRECT *pmMyRect, RECT *pmRect)
{
    pmRect -> top = pmMyRect -> top;
    pmRect -> bottom = pmMyRect -> bottom;
    pmRect -> left = pmMyRect -> left;
    pmRect -> right = pmMyRect -> right;
} // MyConvertMYRECTToRECT


/************************************************************************/
/* MyConvertRECTToMYRECT						*/
/************************************************************************/
static void	MyConvertRECTToMYRECT (RECT *pmRect, MYRECT *pmMyRect)
{
    pmMyRect -> top = pmRect -> top;
    pmMyRect -> bottom = pmRect -> bottom;
    pmMyRect -> left = pmRect -> left;
    pmMyRect -> right = pmRect -> right;
} // MyConvertRECTToMYRECT




