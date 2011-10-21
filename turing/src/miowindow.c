/****************/
/* miowindow.c */
/****************/

/*******************/
/* System includes */
/*******************/
#include <string.h>

/****************/
/* Self include */
/****************/
#include "miowindow.h"

/******************/
/* Other includes */
/******************/
#include "miowin.h"
#include "mio.h"
#include "mdio.h"
#include "mdiowin.h"

#include "mioerr.h"

#include "edint.h"

// Test to make certain we're not accidentally including <windows.h> which
// might allow for windows contamination of platform independent code.
#ifdef _INC_WINDOWS
xxx
#endif

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
#define WINDOW_CLOSED			(MIOWinInfoPtr) 1

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
static MIOWinInfoPtr	stStreams [40];
static char		stDescriptionOfLastWindowOpened [1024];

/******************************/
/* Static callback procedures */
/******************************/

/*********************/
/* Static procedures */
/*********************/
static MIOWinInfoPtr	MyGetWindowInfo (int pmWindowID);

/***********************/
/* External procedures */
/***********************/
/************************************************************************/
/* MIOWindow_GetActive							*/
/************************************************************************/
OOTint	MIOWindow_GetActive (void)
{
    WIND		myActiveWindow;
    int			myWindowType;
    MIOWinInfoPtr	myInfo;
    
    myActiveWindow = MDIOWin_GetActiveWindow ();
    
    myWindowType = MIOWin_GetWindowType (myActiveWindow);
    if ((myWindowType == WINDOW_TYPE_MIO_TEXT) || 
	(myWindowType == WINDOW_TYPE_MIO_GRAPHICS) ||
	(myWindowType == WINDOW_TYPE_MIO_DIALOG))
    {
	myInfo = MIOWin_GetInfo (myActiveWindow);
	return myInfo -> turingWindowID;
    }

    // Not a Turing Run Window

    SET_ERRNO (E_WINDOW_NOT_RUN_WINDOW);
    return -5;
} // MIOWindow_GetActive


/************************************************************************/
/* MIOWindow_GetKeyDown							*/
/************************************************************************/
void	MIOWindow_GetKeyDown (BOOL *pmArray)
{
    if (MIO_selectedRunWindowInfo -> visibility == VIS_POPUP)
    {
    	MDIOWin_ShowWindow (MIO_selectedRunWindow);
    	MIO_selectedRunWindowInfo -> visibility = VIS_VISIBLE;
    }
    else if (MIO_selectedRunWindowInfo -> visibility == VIS_INVISIBLE)
    {
    	// TW Error should be set correctly.
    	ABORT_WITH_ERRMSG (1, 
    		"Keyboard input from invisible window is not allowed");
    }

    memcpy (pmArray, MIO_selectedRunWindowInfo -> pressedKeys, 256);
    pmArray [0] = 0;
    
    // If we're reading keys this way, we're not doing regular input.  Flush the
    // keyboard buffer to prevent keyboard overflow.
    MIO_selectedRunWindowInfo -> kbdBufferTail = 
    		MIO_selectedRunWindowInfo -> kbdBufferHead;
} // MIOWindow_GetKeyDown


/************************************************************************/
/* MIOWindow_GetPosition						*/
/************************************************************************/
void	MIOWindow_GetPosition (OOTint pmWindowID, OOTint *pmX, OOTint *pmY)
{
    MIOWinInfoPtr	myInfo;
    MYRECT		myWindowRect, myScreenRect;
    
//    myInfo = (MIOWinInfoPtr) MIO_StreamGet (pmWindowID, WINDOW_STREAM);
    myInfo = MyGetWindowInfo (pmWindowID);
    
    MDIOWin_GetWindowRect (myInfo -> outerWindow, &myWindowRect);
    myScreenRect = EdInt_GetWorkArea (); 		   	

    *pmX = myWindowRect.left;
    *pmY = myScreenRect.bottom - myWindowRect.bottom;
} // MIOWindow_GetPosition


/************************************************************************/
/* MIOWindow_GetSelect							*/
/************************************************************************/
OOTint	MIOWindow_GetSelect (void)
{
    return MIO_selectedRunWindowInfo -> turingWindowID;
} // MIOWindow_GetSelect


/************************************************************************/
/* MIOWindow_Hide							*/
/************************************************************************/
void	MIOWindow_Hide (OOTint pmWindowID)
{
    MIOWinInfoPtr	myInfo;
    
//    myInfo = (MIOWinInfoPtr) MIO_StreamGet (pmWindowID, WINDOW_STREAM);
    myInfo = MyGetWindowInfo (pmWindowID);

    myInfo -> visibility = VIS_INVISIBLE;
    
    MDIOWin_HideWindow (myInfo -> outerWindow);
} // MIOWindow_Hide


/************************************************************************/
/* MIOWindow_IsVisible							*/
/************************************************************************/
BOOL	MIOWindow_IsVisible (OOTint pmWindowID)
{
    MIOWinInfoPtr	myInfo = MyGetWindowInfo (pmWindowID);

    return (myInfo -> visibility == VIS_VISIBLE);
} // MIOWindow_IsVisible


/************************************************************************/
/* MIOWindow_RegisterClose						*/
/************************************************************************/
void	MIOWindow_RegisterClose (OOTint pmWindowID)
{
    // Tell the debugger it is no longer in use.
    // TW: Actually the debugger already deallocated the stream when
    // it closed the stream in mio.c
    EdInt_NotifyDebuggerObjectDeallocated (pmWindowID);

    stStreams [pmWindowID + 3] = WINDOW_CLOSED;
} // MIOWindow_RegisterClose


/************************************************************************/
/* MIOWindow_RegisterOpen						*/
/*									*/
/* This is called only just after opening a window when 		*/
/* MIO_selectedRunWindowInfo refers to the just opened window.		*/
/************************************************************************/
void	MIOWindow_RegisterOpen (OOTint pmWindowID, SRCPOS *pmSrcPos)
{
    EdInt_NotifyDebuggerObjectAllocated (WINDOW_STREAM, pmWindowID, pmSrcPos,
	    stDescriptionOfLastWindowOpened);

    stStreams [pmWindowID + 3] = MIO_selectedRunWindowInfo;	    
    MIO_selectedRunWindowInfo -> turingWindowID = pmWindowID;
} // MIOWindow_RegisterOpen


/************************************************************************/
/* MIOWindow_RegisterWindow						*/
/*									*/
/* This is called by MIO to register stdin/stdout/stderr windows.	*/
/************************************************************************/
void	MIOWindow_RegisterWindow (OOTint pmWindowID, MIOWinInfoPtr pmInfo)
{
    SRCPOS	mySrcPos;
    
    mySrcPos.fileNo = 0;
    mySrcPos.lineNo = 0;

    EdInt_NotifyDebuggerObjectAllocated (WINDOW_STREAM, pmWindowID, &mySrcPos,
	    stDescriptionOfLastWindowOpened);

    stStreams [pmWindowID + 3] = pmInfo;	    
    pmInfo -> turingWindowID = pmWindowID;
} // MIOWindow_RegisterWindow


/************************************************************************/
/* MIOWindow_SetActive							*/
/************************************************************************/
void	MIOWindow_SetActive (OOTint pmWindowID)
{
    MIOWinInfoPtr	myInfo;
    WIND		myWindow = MyGetWindowInfo (pmWindowID) -> outerWindow;

    // If we've got a top-most window, activate it instead of whatever window
    // the program asked us to activate.
    myInfo = MIO_GetTopMostWindow ();

    if (myInfo != NULL)
    {
	myWindow = myInfo -> outerWindow;
    }

    MDIOWin_SetActiveWindow (myWindow);
} // MIOWindow_SetActive


/************************************************************************/
/* MIOWindow_SetDescription						*/
/*									*/
/* This routine exists to provide a way for the MIO module (which	*/
/* actually opens windows, to provide a description to be used by	*/
/* MIOWindow_RegisterOpen so that the debugger will have a description	*/
/* that it can display.							*/
/************************************************************************/
void	MIOWindow_SetDescription (const char *pmDescription)
{
    strcpy (stDescriptionOfLastWindowOpened, pmDescription);
} // MIOWindow_SetDescription


/************************************************************************/
/* MIOWindow_SetPosition						*/
/*									*/
/* Realizing that people confuse centre and middle, We allow the	*/
/* following combinations:						*/
/*	right-top,right-middle,right-truemiddle,right-bottom		*/
/*	centre-top,centre-middle,centre-truemiddle,centre-bottom	*/
/*	left-top,left-middle,left-truemiddle,left-bottom		*/
/*									*/
/* It also allows the coordinates to be flip-flopped.			*/
/*	top-right, middle-right, truemiddle-right, bottom-right		*/
/*	top-centre, middle-centre, truemiddle-centre, bottom-centre	*/
/*	top-left, middle-left, truemiddle-left, bottom-left		*/
/*									*/
/* It also allows confusion of centre and middle.			*/
/*	right-centre,middle-top,middle-middle,middle-truemiddle,	*/
/*	middle-bottom,middle-centre,left-centre				*/
/*	centre-right,top-middle,middle-truemiddle,middle-middle		*/
/*	middle-bottom,middle-centre,left-centre				*/
/************************************************************************/
void	MIOWindow_SetPosition (OOTint pmWindowID, OOTint pmX, OOTint pmY)
{
    MIOWinInfoPtr	myInfo;
    int			myWindowLeft, myWindowTop;
    
//    myInfo = (MIOWinInfoPtr) MIO_StreamGet (pmWindowID, WINDOW_STREAM);
    myInfo = MyGetWindowInfo (pmWindowID);

    //               y
    //       L  C  R  T  M  TM  B
    //   L   x  5  x ok ok  ok ok
    //   C   2  4  2 ok ok  ok ok
    // x R   x  5  x ok ok  ok ok
    //   T   1  1  1  x  3   x  x
    //   M   1  1  1  6  4   6  6
    //   TM  1  1  1  x  3   x  x
    //   B   1  1  1  x  3   x  x
    //
    // (1) If somebody goofed and placed a vertical alignment first and a
    // horizontal alignment second, switch them.
    if (((pmX == TOP_ALIGN) || (pmX == MIDDLE_ALIGN) || 
	 (pmX == TRUE_MIDDLE_ALIGN) || (pmX == BOTTOM_ALIGN)) && 
	((pmY == LEFT_ALIGN) || (pmY == CENTRE_ALIGN) || (pmY == RIGHT_ALIGN)))
    {
	int mySwap = pmX;
	pmX = pmY;
	pmY = mySwap;
    }
    // (2) If somebody switched alignment and confused CENTRE with MIDDLE
    else if ((pmX == CENTRE_ALIGN) && 
	     ((pmY == LEFT_ALIGN) || (pmY == RIGHT_ALIGN)))
    {
	pmX = pmY;
	pmY = MIDDLE_ALIGN;
    }
    // (3) If somebody switched alignment and confused MIDDLE with CENTRE
    else if (((pmX == TOP_ALIGN) || (pmX == TRUE_MIDDLE_ALIGN) || 
	      (pmX == BOTTOM_ALIGN)) && 
	     (pmY == MIDDLE_ALIGN))
    {
 	pmX = CENTRE_ALIGN;
	pmY = pmX;
    }
    // (4) If somebody used MIDDLE or CENTRE twice
    else if (((pmX == MIDDLE_ALIGN) && (pmY == MIDDLE_ALIGN)) ||
	     ((pmX == CENTRE_ALIGN) && (pmY == CENTRE_ALIGN)))
    {
 	pmX = CENTRE_ALIGN;
	pmY = MIDDLE_ALIGN;
    }
    // (5) If somebody switched alignment and confused CENTRE with MIDDLE
    else if (((pmX == LEFT_ALIGN) || (pmX == RIGHT_ALIGN)) &&
	     (pmY == CENTRE_ALIGN))
    {
	pmY = MIDDLE_ALIGN;
    }
    // (6) If somebody switched alignment and confused CENTRE with MIDDLE
    else if ((pmX == MIDDLE_ALIGN) &&
	     ((pmY == TOP_ALIGN) || (pmY == TRUE_MIDDLE_ALIGN) || 
	      (pmY == BOTTOM_ALIGN)))
    {
	pmX = CENTRE_ALIGN;
    }

    //
    // If after all of that, there's still a problem, give an error message.
    //
    if ((pmX == TOP_ALIGN) || (pmX == MIDDLE_ALIGN) || 
	(pmX == TRUE_MIDDLE_ALIGN) || (pmX == BOTTOM_ALIGN))
    {
	SET_ERRMSG (E_WINDOW_SET_BAD_OPTION, 
			 "Bad specification of window x position");
	return;
    }
    if ((pmY == LEFT_ALIGN) || (pmY == CENTRE_ALIGN) || (pmY == RIGHT_ALIGN))
    {
	SET_ERRMSG (E_WINDOW_SET_BAD_OPTION, 
			 "Bad specification of window y position");
	return;
    }

    // 
    // Set the new window position
    //
    myInfo -> initXPos = pmX;
    myInfo -> initYPos = pmY;

    MIOWin_CalculateWindowPosition (myInfo -> outerWindow, pmX, pmY,
				    &myWindowLeft, &myWindowTop);
    MDIOWin_SetWindowPosition (myInfo -> outerWindow, myWindowLeft, 
        		       myWindowTop, 0, 0);
} // MIOWindow_SetPosition


/************************************************************************/
/* MIOWindow_SetScreen							*/
/*									*/
/* Possible options to change:						*/
/*	mode - TEXT_MODE, GRAPHICS_MODE					*/
/*      rows, columns - size of screen in characters			*/
/*      xSize, ySize - size of screen in pixels				*/
/*	graphicsMode - ignored for now (TW)				*/
/*	visibility - VIS_VISIBLE, VIS_INVISIBLE, VIS_POPUP		*/
/*	xor, cursor, echo, msdosCharSet - window attributes		*/
/*	buttonBar - has a button bar on it?				*/
/*	title - window title?						*/
/*	xPosition - Win location - Number or {LEFT/CENTRE/RIGHT}_ALIGN 	*/
/*	yPosition - Win location - Number or 				*/
/*				{TOP/MIDDLE/TRUEMIDDLE/BUTTON}_ALIGN 	*/
/*									*/
/* If the window changes mode, size or buttonBar, then it must be	*/
/* recreated.								*/
/************************************************************************/
void	MIOWindow_SetScreen (OOTint pmWindowID, OOTstring pmSetString)
{
    WindowAttrib	myAttribs;
    BOOL		myResult;
    MIOWinInfoPtr	myInfo, myNewInfo;
    int			myWidth, myHeight, myWindowHasButtonBar;
    MYRECT		myWindowRect;
    WIND		myOldWindow, myNewWindow;
    
    if (pmWindowID == CURRENTLY_SELECTED_WINDOW)
    {
    	pmWindowID = MIO_selectedRunWindowInfo -> turingWindowID;
    }
    
    myResult = MIOWin_ConvertSetscreenAttribs (pmSetString, &myAttribs, 
    					       SETSCREEN_SETUP);
    if (!myResult)
    {
    	return;
    }

    //
    // Convert each item in turn.  Some items require the window to be 
    // recreated.
    //
//    myInfo = (MIOWinInfoPtr) MIO_StreamGet (pmWindowID, WINDOW_STREAM);
    myInfo = MyGetWindowInfo (pmWindowID);

    //
    // Decide if the window must be recreated.
    //
    
    // Calculate the window's size
    // If the window does have a button bar, the attribute must be set.
    if (myInfo -> buttonBarWindow == NULL)
    {
    	myWindowHasButtonBar = 0;
    }
    else
    {
    	myWindowHasButtonBar = 1;
    }
    	
    if (myAttribs.buttonBar == UNSET_VALUE)
    {
    	myAttribs.buttonBar = myWindowHasButtonBar;
    }
    
    MIOWin_CalculateInnerWindowSize (&myAttribs, NULL, NULL, &myWidth, 
    	&myHeight, NULL, NULL, NULL, NULL);

    // Do the comparison.  Recreate the window if the mode changed, the window
    // size changed or the button bar status changed.
    // Logic: Same window if button bar is same and (window type is unset or
    // (window type is same and width/height are same)).
    if ((myWindowHasButtonBar == myAttribs.buttonBar) &&
	((myAttribs.mode == UNSET_VALUE) ||
         ((myAttribs.mode == myInfo -> windowKind) && 
          (myInfo -> width == myWidth) &&
          (myInfo -> height == myHeight))))
    {
	//
	// We're keeping the same window.  Change the various attributes
	// that need changing in the actual myInfo (because we're keeping it).
	//

	// visibility    
	if (myAttribs.visibility == VIS_INVISIBLE)
	{
	    myInfo -> visibility = VIS_INVISIBLE;
	    
	    MDIOWin_HideWindow (myInfo -> outerWindow);
	}
	else if (myAttribs.visibility == VIS_VISIBLE)
	{
	    myInfo -> visibility = VIS_VISIBLE;
	    
	    MDIOWin_ShowWindow (myInfo -> outerWindow);
	}
	else if (myAttribs.visibility == VIS_POPUP)
	{
	    myInfo -> visibility = VIS_POPUP;
	    
	    MDIOWin_HideWindow (myInfo -> outerWindow);
	}
	
	// xor
	if (myAttribs.xor != UNSET_VALUE)
	{
	    myInfo -> xor = myAttribs.xor;
	}
	
	// displayOnScreen (offscreenonly)
	if (myAttribs.displayOnScreen != UNSET_VALUE)
	{
	    myInfo -> displayOnScreen = myAttribs.displayOnScreen;
	    // If we've just turned off "offscreenonly", then do an update.
	    if (myAttribs.displayOnScreen)
	    {
		MIOWindow_Update (pmWindowID);
	    }
	}
	
	// cursor
	if (myAttribs.cursor != UNSET_VALUE)
	{
	    myInfo -> cursor = myAttribs.cursor;
	    // This displays the cursor if appropriate (that is, if this is the
	    // owner window, and it is accepting input, and cursor is on for this
	    // window.
	    MIOWin_CaretDisplay (myInfo -> innerWindow);
	}
	
	// echo
	if (myAttribs.echo != UNSET_VALUE)
	{
	    myInfo -> echo = myAttribs.echo;
	}
	
	// msdosCharSet
	if (myAttribs.msdosCharSet != UNSET_VALUE)
	{
	    myInfo -> msdosCharSet = myAttribs.msdosCharSet;
	}

	// top most
	if (myAttribs.topMost != UNSET_VALUE)
	{
	    myInfo -> zOrder = myAttribs.topMost;
	}

	// title
	if (myAttribs.title [0] != 0)
	{
	    strcpy (myInfo -> title, myAttribs.title);
	    MDIOWin_SetWindowTitle (myInfo -> outerWindow, myAttribs.title);
	}
	
	// position
	if (myAttribs.xPosition != UNSET_VALUE)
	{
	    int	myWindowLeft, myWindowTop;
	    
	    MIOWin_CalculateWindowPosition (myInfo -> outerWindow, 
		myAttribs.xPosition, myAttribs.yPosition,
		&myWindowLeft, &myWindowTop);
	    MDIOWin_SetWindowPosition (myInfo -> outerWindow, myWindowLeft, 
		myWindowTop, 0, 0);
	}

	return;
    }

    EdInt_AddFailMessage ("View/Window.Set called (%s)", pmSetString);

    //
    // Recreate the window.  Set any unset attributes to be the same as
    // in the original window.
    //
    if (myAttribs.mode == UNSET_VALUE) 
    {
	if (myAttribs.rows == UNSET_VALUE)
	{
	    myAttribs.rows = myInfo -> initRows;
	}
	if (myAttribs.columns == UNSET_VALUE)
	{
	    myAttribs.columns = myInfo -> initColumns;
	}
	if (myAttribs.xSize == UNSET_VALUE)
	{
	    myAttribs.xSize = myInfo -> initXSize;
	}
	if (myAttribs.ySize == UNSET_VALUE)
	{
	    myAttribs.ySize = myInfo -> initYSize;
	}
	myAttribs.mode = myInfo -> windowKind;
    }
    
    //
    // If we've manually moved the window from its original location, then
    // keep the new location, otherwise use the original location
    //
    MDIOWin_GetWindowRect (myInfo -> outerWindow, &myWindowRect);
    if ((myWindowRect.left == myInfo -> initWindowLeft) && 
	(myWindowRect.top == myInfo -> initWindowTop))
    {
	if (myAttribs.xPosition == UNSET_VALUE)
	{
	    myAttribs.xPosition = myInfo -> initXPos;
	}
	if (myAttribs.yPosition == UNSET_VALUE)
	{
	    myAttribs.yPosition = myInfo -> initYPos;
	}
    }
    else
    {
	myAttribs.xPosition = myWindowRect.left;
	myAttribs.yPosition = myWindowRect.top;
    }
    
    if (myAttribs.visibility == UNSET_VALUE)
    {
	myAttribs.visibility = myInfo -> visibility;
    }
    if (myAttribs.xor == UNSET_VALUE)
    {
	myAttribs.xor = myInfo -> xor;
    }
    if (myAttribs.displayOnScreen == UNSET_VALUE)
    {
	myAttribs.displayOnScreen = myInfo -> displayOnScreen;
    }
    if (myAttribs.cursor == UNSET_VALUE)
    {
	myAttribs.cursor = myInfo -> cursor;
    }
    if (myAttribs.echo == UNSET_VALUE)
    {
	myAttribs.echo = myInfo -> echo;
    }
    if (myAttribs.buttonBar == UNSET_VALUE)
    {
	if (myInfo -> buttonBarWindow == NULL)
	{
	    myAttribs.buttonBar = 0;
	}
	else
	{
	    myAttribs.buttonBar = 1;
	}
    }
    if (myAttribs.msdosCharSet == UNSET_VALUE)
    {
	myAttribs.msdosCharSet = myInfo -> msdosCharSet;
    }
    if (myAttribs.topMost == UNSET_VALUE)
    {
	myAttribs.topMost = myInfo -> zOrder;
    }
    if (myAttribs.title [0] == 0)
    {
	strcpy (myAttribs.title, myInfo -> title);
    }
    
    // Perform the substitution of the old window for the new
    myOldWindow = myInfo -> outerWindow;
    MDIOWin_HideWindow (myOldWindow);
    myNewWindow = MIOWin_CreateWindowFromAttribs (&myAttribs);
    myNewInfo = MIOWin_GetInfo (myNewWindow);
    
    // If the old window used a special palette, transfer it
    // to the new window.
    MDIOWin_CopyPaletteFromOldToNewWindow (myOldWindow, myNewWindow);

    // Substitute the new window for the old one in the list of streams
    // and with the currently selected window, and the run window list
    stStreams [pmWindowID + 3] = MIOWin_GetInfo (myNewWindow);
    myNewInfo -> turingWindowID = myInfo -> turingWindowID;
    myNewInfo -> turingMIOFilePtr = myInfo -> turingMIOFilePtr;

    MIO_SubstituteRunWindow (myOldWindow, myNewWindow, pmSetString);
    myInfo -> okayToClose = TRUE;
    myInfo -> turingWindowID = WINDOW_CHANGED_NOT_CLOSED;
    MIOWin_CloseWindow (myOldWindow);
} // MIOWindow_SetScreen


/************************************************************************/
/* MIOWindow_SetTransparentColour					*/
/*									*/
/* Set the colour that will be transparent in picUnderMerge mode in	*/
/* Pic.Draw operations.							*/
/************************************************************************/
void	MIOWindow_SetTransparentColour (OOTint pmWindowID, OOTint pmColourNum)
{
    MIOWinInfoPtr	myInfo;
    
    myInfo = MyGetWindowInfo (pmWindowID);
    MIO_CheckColourRange (pmColourNum);        

    myInfo -> transparentColour = pmColourNum;
} // MIOWindow_SetTransparentColour


/************************************************************************/
/* MIOWindow_Show							*/
/************************************************************************/
void	MIOWindow_Show (OOTint pmWindowID)
{
    MIOWinInfoPtr	myInfo;
    
//    myInfo = (MIOWinInfoPtr) MIO_StreamGet (pmWindowID, WINDOW_STREAM);
    myInfo = MyGetWindowInfo (pmWindowID);

    myInfo -> visibility = VIS_VISIBLE;
    
    MDIOWin_ShowWindow (myInfo -> outerWindow);
} // MIOWindow_Show


/************************************************************************/
/* MIOWindow_Update							*/
/************************************************************************/
void MIOWindow_Update (OOTint pmWindowID)
{
    MIOWinInfoPtr	myInfo;
    
    myInfo = MyGetWindowInfo (pmWindowID);

    MDIOWin_Update (myInfo -> innerWindow);
} // MIOWindow_Update


/************************************************************************/
/* MIOWindow_UpdateArea							*/
/************************************************************************/
void MIOWindow_UpdateArea (OOTint pmWindowID, OOTint x1, OOTint y1, 
		           OOTint x2, OOTint y2)
{
    MIOWinInfoPtr	myInfo;
    
    myInfo = MyGetWindowInfo (pmWindowID);

    MDIOWin_UpdateArea (myInfo -> innerWindow, x1, myInfo -> height - y1 - 1, 
    		x2, myInfo -> height - y2 - 1);
} // MIOWindow_UpdateArea


/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/
/************************************************************************/
/* MyGetWindowInfo							*/
/************************************************************************/
static MIOWinInfoPtr	MyGetWindowInfo (int pmWindowID)
{
    char    myMessage [1024];

    if (pmWindowID == CURRENTLY_SELECTED_WINDOW)
    {
    	pmWindowID = MIO_selectedRunWindowInfo -> turingWindowID;
    }

    if ((pmWindowID < -2) || (pmWindowID > 20))
    {
	MDIO_sprintf (myMessage, "%d is not a legal window id", pmWindowID);
	ABORT_WITH_ERRMSG (E_WINDOW_NOT_AN_ID, myMessage);
    }
    if (stStreams [pmWindowID + 3] == NULL)
    {
	MDIO_sprintf (myMessage, "Window with id of %d is not been opened", 
		      pmWindowID);
	ABORT_WITH_ERRMSG (E_WINDOW_NEVER_OPENED, myMessage);
    }
    if (stStreams [pmWindowID + 3] == WINDOW_CLOSED)
    {
	MDIO_sprintf (myMessage, "Window with id of %d is closed", 
		      pmWindowID);
	ABORT_WITH_ERRMSG (E_WINDOW_CLOSED, myMessage);
    }
    return stStreams [pmWindowID + 3];
} // MyGetWindowInfo

