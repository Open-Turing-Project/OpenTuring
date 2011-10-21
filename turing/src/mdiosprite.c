/****************/
/* mdiosprite.c */
/****************/

/*******************/
/* System includes */
/*******************/

/****************/
/* Self include */
/****************/
#include "mdiosprite.h"

/******************/
/* Other includes */
/******************/
#include "mio.h"

#include "miorgb.h"

#include "mdiopic.h"

#include <windows.h>
#include <windowsx.h>

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/

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

/******************************/
/* Static callback procedures */
/******************************/

/*********************/
/* Static procedures */
/*********************/


/***********************/
/* External procedures */
/***********************/
extern BOOL	MDIOPic_TransparentBlt (HDC pmDestDC, int pmDestX, int pmDestY,
			          int pmWidth, int pmHeight, HDC pmSourceDC,
			          int pmSourceX, int pmSourceY, 
			          COLORREF pmTransColor, HBITMAP *pmMask);

/************************************************************************/
/* MDIOSprite_InitializeSpriteContext					*/
/*									*/
/* Create the sprite context.						*/
/************************************************************************/
void	MDIOSprite_InitializeSpriteContext (MIOWinInfoPtr pmInfoPtr)
{
    pmInfoPtr -> spriteDeviceContext = CreateCompatibleDC (
					pmInfoPtr -> deviceContext);
    pmInfoPtr -> spriteBitMap = CreateCompatibleBitmap (
    					pmInfoPtr -> deviceContext,
    					pmInfoPtr -> width, 
					pmInfoPtr -> height);
    SelectObject ((HDC) pmInfoPtr -> spriteDeviceContext, 
    		  (HBITMAP) pmInfoPtr -> spriteBitMap);
    
    // Set up the palette
    SelectPalette ((HDC) pmInfoPtr -> spriteDeviceContext, 
		   pmInfoPtr -> palette, FALSE);
    RealizePalette ((HDC) pmInfoPtr -> spriteDeviceContext);
} // MDIOSprite_InitializeSpriteContext


/************************************************************************/
/* MDIOSprite_UpdateWindow						*/
/*									*/
/* Update the dirty rectangles in a window.  Only called if there is at	*/
/* least one dirty rectangle.						*/
/************************************************************************/
void	MDIOSprite_UpdateWindow (MIOWinInfoPtr pmInfoPtr, 
				 BOOL pmCopyToOffscreen)
{
    MYRECTLIST  *myDirtyRect, *myNextDirtyRect;
    MYRECT	myRect;
    SpriteInfo	*mySpritePtr;
    BOOL	mySpriteNotDrawn, myBackgroundNotDrawn;
    int		myWidth, myHeight;

//int gdiRes, userRes, gdiRes1, userRes1;
//gdiRes = GetGuiResources (GetCurrentProcess(), GR_GDIOBJECTS);
//userRes = GetGuiResources (GetCurrentProcess(), GR_USEROBJECTS);
    myDirtyRect = pmInfoPtr -> dirtyRectHead;

    // Set up the window.
    // TW

    // Now, if there are no items that are behind the offscreen window (i.e.
    // all sprites have positive height), then simply do a copy of the 
    // offscreen window.  However, if there are, then we start by filling
    // the entire area with the transparent colour of the offscreen window, 
    // then drawing the non-postive height sprites in MERGE mode, then drawing
    // the offscreen window in MERGE mode, then drawing the rest of the 
    // sprites in MERGE mode.
    while (myDirtyRect != NULL)
    {
	myRect = myDirtyRect -> rect;

	// Convert from Turing coordinates to Windows coordinates
	myWidth = myRect.right - myRect.left;
	myHeight = myRect.bottom - myRect.top;

	// Now draw each sprite 
	mySpritePtr = (SpriteInfo *) (pmInfoPtr -> spriteQueueHead);
	mySpriteNotDrawn = TRUE;
	myBackgroundNotDrawn = TRUE;

	while (mySpritePtr != NULL)
	{
	    // If the sprite is not visible or does not intersect with the
	    // dirty rectangle, ignore it.
	    if ((!mySpritePtr -> visible) ||
		!MIO_RectanglesIntersect (&myDirtyRect -> rect, 
					  &mySpritePtr -> spriteRect))
	    {
		// Sprite not visible or doesn't intersect, don't draw it!
		mySpritePtr = mySpritePtr -> next;
		continue;
	    }

	    // If we haven't drawn a sprite before we either have to draw the
	    // offscreen window (if the first sprite's height is positive, 
	    // or just fill the sprite space with the transparent colour).
	    if (mySpriteNotDrawn)
	    {
		if (mySpritePtr -> spriteHeight >= 0)
		{
		    // The lowest sprite is above the background.  Draw the
		    // background using COPY
		    BitBlt (pmInfoPtr -> spriteDeviceContext, 
			    myRect.left, myRect.top, myWidth, myHeight, 
			    pmInfoPtr -> offscreenDeviceContext, 
			    myRect.left, myRect.top, SRCCOPY);
		    myBackgroundNotDrawn = FALSE;
		}
		else
		{
		    // Fill the background with the offscreen window's
		    // transparent color
		    HPEN 	myNewPen, myOldPen;
		    HBRUSH	myNewBrush, myOldBrush;
		    COLORREF	myTransparentColour;

		    myTransparentColour = 
			(COLORREF) MIORGB_ConvertColour (
					pmInfoPtr -> transparentColour);
		    myNewPen = CreatePen (PS_SOLID, 1, myTransparentColour);
		    myNewBrush = CreateSolidBrush (myTransparentColour);
		    myOldPen = SelectPen (pmInfoPtr -> spriteDeviceContext, 
					  myNewPen);
		    myOldBrush = SelectBrush (pmInfoPtr -> spriteDeviceContext, 
					      myNewBrush);
		    Rectangle (pmInfoPtr -> spriteDeviceContext, myRect.left, 
			       myRect.top, myRect.right + 1, myRect.bottom + 1);
		    SelectBrush (pmInfoPtr -> spriteDeviceContext, myOldBrush);
		    DeleteBrush (myNewBrush);
    		    SelectPen (pmInfoPtr -> spriteDeviceContext, myOldPen);
		    DeletePen (myNewPen);
		}
		mySpriteNotDrawn = FALSE;
	    }
	    else
	    {
		// If we haven't drawn the background before, and this sprite
		// is above the background (height >= 0), then its time to
		// draw the background in MERGE mode
		if (myBackgroundNotDrawn && (mySpritePtr -> spriteHeight >= 0))
		{
		    MDIOPic_SpriteDraw (pmInfoPtr, NULL, myRect.left, myRect.top, 
					myWidth, myHeight, 
					pmInfoPtr -> transparentColour);
		    
		    myBackgroundNotDrawn = FALSE;
		}
	    }

	    // Draw Sprite
	    MDIOPic_SpriteDraw (pmInfoPtr, mySpritePtr -> picMDPicInfo, 
				mySpritePtr -> x, mySpritePtr -> y,
				mySpritePtr -> picWidth, 
				mySpritePtr -> picHeight,
				mySpritePtr -> picTransparentColour);

	    mySpritePtr = mySpritePtr -> next;
	}
	// If we still haven't drawn the background after having drawn all the
	// sprites, then it is time to draw the background.  If we've drawn
	// any sprites above, then the background has already been drawn, and
	// we draw in MERGE mode.  If we haven't drawn a sprite, then we draw
	// the background in copy mode.
	if (myBackgroundNotDrawn)
	{
	    if (mySpriteNotDrawn)
	    {
		BitBlt (pmInfoPtr -> spriteDeviceContext, 
			myRect.left, myRect.top, myWidth, myHeight, 
			pmInfoPtr -> offscreenDeviceContext, 
			myRect.left, myRect.top, SRCCOPY);
	    }
	    else
	    {
		MDIOPic_SpriteDraw (pmInfoPtr, NULL, myRect.left, myRect.top, 
				    myWidth, myHeight, 
				    pmInfoPtr -> transparentColour);
	    }
	    myBackgroundNotDrawn = FALSE;
	}

	// Finally, draw the completed sprite surface to the on-screen window.
	BitBlt (pmInfoPtr -> deviceContext, myRect.left, myRect.top, 
		myWidth, myHeight, pmInfoPtr -> spriteDeviceContext, 
		myRect.left, myRect.top, SRCCOPY);

	// Okay, if pmCopyToOffscreen is true, then copy all of this to the
	// offscreen bitmap as well.  This is used when Turing is finished and
	// the sprites are about to be deallocated.
	if (pmCopyToOffscreen)
	{
	    BitBlt (pmInfoPtr -> offscreenDeviceContext, myRect.left, 
		    myRect.top, myWidth, myHeight, 
		    pmInfoPtr -> spriteDeviceContext, 
		    myRect.left, myRect.top, SRCCOPY);
	}

	// Free the current dirty rect and get the next dirty rectangle.
	myNextDirtyRect = myDirtyRect -> next;
	free (myDirtyRect);
	myDirtyRect = myNextDirtyRect;
    } // while

    pmInfoPtr -> dirtyRectHead = NULL;
//gdiRes1 = GetGuiResources (GetCurrentProcess(), GR_GDIOBJECTS);
//userRes1 = GetGuiResources (GetCurrentProcess(), GR_USEROBJECTS);
} // MDIOSprite_UpdateWindow

/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/


