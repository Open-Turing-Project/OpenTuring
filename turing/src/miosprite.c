/***************/
/* miosprite.c */
/***************/

/*******************/
/* System includes */
/*******************/
#include <malloc.h>

/****************/
/* Self include */
/****************/
#include "miosprite.h"

/******************/
/* Other includes */
/******************/
#include "mio.h"
#include "miopic.h"
#include "miotime.h"
#include "mioerr.h"
#include "mdio.h"
#include "mdiopic.h"

#include "mdiosprite.h"

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
#define FOREGROUND_HEIGHT		  0
#define DEFAULT_SPRITE_INTERVAL		 30

/********************/
/* Global variables */
/********************/

/*********/
/* Types */
/*********/
typedef struct WindowQueue
{
    MIOWinInfoPtr	windowInfoPtr;
    struct WindowQueue	*next;
} WindowQueue;

/**********************/
/* External variables */
/**********************/

/********************/
/* Static constants */
/********************/

/********************/
/* Static variables */
/********************/
static BOOL		stSpritesInUse;
static int		stSpriteInterval;
static int		stSpriteLastTime;
static WindowQueue	*stSpriteWindowQueueHead;

/******************************/
/* Static callback procedures */
/******************************/

/*********************/
/* Static procedures */
/*********************/
static void	MyAddDirtyRect (MIOWinInfoPtr pmInfoPtr, MYRECT pmRect);
static void	MyAddToQueue (MIOWinInfoPtr pmInfoPtr, 
			      SpriteInfo *pmSpriteInfo);
static void	MyFreeSprite (SpriteInfo *pmSpriteInfo);
static MYRECT	MyGetSpriteRect (SpriteInfo *pmSpriteInfo);
static void	MyRemoveFromQueue (MIOWinInfoPtr pmInfoPtr, 
				   SpriteInfo *pmSpriteInfo);


/*********************************************/
/* External procedures for Turing predefines */
/*********************************************/
/************************************************************************/
/* MIOSprite_Init_Run							*/
/************************************************************************/
void	MIOSprite_Init_Run (void)
{
    stSpritesInUse = FALSE;
    stSpriteInterval = DEFAULT_SPRITE_INTERVAL;
    stSpriteLastTime = MIOTime_GetTicks ();
    stSpriteWindowQueueHead = NULL;
} // MIOSprite_Init_Run


/************************************************************************/
/* MIOSprite_Animate							*/
/************************************************************************/
void	MIOSprite_Animate (OOTint pmSpriteID, OOTint pmPicID, 
			   OOTint pmX, OOTint pmY, OOTint pmCentered)
{
    SpriteInfo		*mySpriteInfo;
    MIOWinInfoPtr	myInfo;

    mySpriteInfo = (SpriteInfo *) MIO_IDGet (pmSpriteID, SPRITE_ID);
    myInfo = MIOWin_GetInfo (mySpriteInfo -> runWindow);

    if (mySpriteInfo -> visible) 
    {
	MyAddDirtyRect (myInfo, mySpriteInfo -> spriteRect);
    }

    MDIOPic_DecrementUse (mySpriteInfo -> picMDPicInfo);
    MIOPic_GetInfoForSprite (pmPicID, &(mySpriteInfo -> picWidth),
			     &(mySpriteInfo -> picHeight),
			     &(mySpriteInfo -> picTransparentColour),
			     &(mySpriteInfo -> picMDPicInfo));
    MDIOPic_IncrementUse (mySpriteInfo -> picMDPicInfo);
    
    if (pmCentered) 
    {
	pmX -= mySpriteInfo -> picWidth / 2;
	pmY -= mySpriteInfo -> picHeight / 2;
    }
    mySpriteInfo -> x = pmX;
    mySpriteInfo -> y =  myInfo -> height - pmY - mySpriteInfo -> picHeight;
    mySpriteInfo -> spriteRect = MyGetSpriteRect (mySpriteInfo);
    mySpriteInfo -> centered = pmCentered;
    
    if (mySpriteInfo -> visible) 
    {
	MyAddDirtyRect (myInfo, mySpriteInfo -> spriteRect);
    }
} // MIOSprite_Animate


/************************************************************************/
/* MIOSprite_ChangePic							*/
/************************************************************************/
void	MIOSprite_ChangePic (OOTint pmSpriteID, OOTint pmPicID)
{
    SpriteInfo		*mySpriteInfo;
    MIOWinInfoPtr	myInfo;
    int			myOldWidth, myOldHeight;

    mySpriteInfo = (SpriteInfo *) MIO_IDGet (pmSpriteID, SPRITE_ID);
    myOldWidth = mySpriteInfo -> picWidth;
    myOldHeight = mySpriteInfo -> picHeight;
    myInfo = MIOWin_GetInfo (mySpriteInfo -> runWindow);

    if (mySpriteInfo -> visible) 
    {
	MyAddDirtyRect (myInfo, mySpriteInfo -> spriteRect);
    }

    MDIOPic_DecrementUse (mySpriteInfo -> picMDPicInfo);

    if (mySpriteInfo -> centered)
    {
	MIOPic_GetInfoForSprite (pmPicID, &(mySpriteInfo -> picWidth),
				 &(mySpriteInfo -> picHeight),
				 &(mySpriteInfo -> picTransparentColour),
				 &(mySpriteInfo -> picMDPicInfo));
	mySpriteInfo -> x += (myOldWidth - mySpriteInfo -> picWidth) / 2;
	mySpriteInfo -> y += (myOldHeight - mySpriteInfo -> picHeight) / 2;
    }
    else
    {
	MIOPic_GetInfoForSprite (pmPicID, &(mySpriteInfo -> picWidth),
				 &(mySpriteInfo -> picHeight),
				 &(mySpriteInfo -> picTransparentColour),
				 &(mySpriteInfo -> picMDPicInfo));
	mySpriteInfo -> y += (myOldHeight - mySpriteInfo -> picHeight);
    }
    mySpriteInfo -> spriteRect = MyGetSpriteRect (mySpriteInfo);
    
    MDIOPic_IncrementUse (mySpriteInfo -> picMDPicInfo);

    if (mySpriteInfo -> visible) 
    {
	MyAddDirtyRect (myInfo, mySpriteInfo -> spriteRect);
    }
} // MIOSprite_ChangePic


/************************************************************************/
/* MIOSprite_Dirty							*/
/************************************************************************/
void	MIOSprite_Dirty (MIOWinInfoPtr pmInfoPtr, 
			 int pmX1, int pmY1, int pmX2, int pmY2)
{
    MYRECT  myRect;

    if (stSpritesInUse)
    {
	myRect.left = MIN (pmX1, pmX2);
	myRect.right = MAX (pmX1, pmX2);
	myRect.top = MIN (pmY1, pmY2);
	myRect.bottom = MAX (pmY1, pmY2);
	
	MyAddDirtyRect (pmInfoPtr, myRect);
    }
} // MIOSprite_Dirty


/************************************************************************/
/* MIOSprite_Free							*/
/************************************************************************/
void	MIOSprite_Free (OOTint pmSpriteID)
{
    SpriteInfo		*mySpriteInfo;
    MIOWinInfoPtr	myInfo;

    mySpriteInfo = (SpriteInfo *) MIO_IDGet (pmSpriteID, SPRITE_ID);
    if (mySpriteInfo -> runWindow != NULL)
    {
	myInfo = MIOWin_GetInfo (mySpriteInfo -> runWindow);

	if (mySpriteInfo -> visible) 
	{
	    MyAddDirtyRect (myInfo, mySpriteInfo -> spriteRect);
	}

	MyRemoveFromQueue (myInfo, mySpriteInfo);
    }

    MDIOPic_DecrementUse (mySpriteInfo -> picMDPicInfo);

    MyFreeSprite (mySpriteInfo);
    MIO_IDRemove (pmSpriteID, SPRITE_ID);
} // MIOSprite_Free


/************************************************************************/
/* MIOSprite_Hide							*/
/************************************************************************/
void	MIOSprite_Hide (OOTint pmSpriteID)
{
    SpriteInfo		*mySpriteInfo;
    MIOWinInfoPtr	myInfo;

    mySpriteInfo = (SpriteInfo *) MIO_IDGet (pmSpriteID, SPRITE_ID);
    myInfo = MIOWin_GetInfo (mySpriteInfo -> runWindow);

    if (mySpriteInfo -> visible) 
    {
	mySpriteInfo -> visible = FALSE;
	MyAddDirtyRect (myInfo, mySpriteInfo -> spriteRect);
    }
} // MIOSprite_Hide


/************************************************************************/
/* MIOSprite_New							*/
/************************************************************************/
OOTint	MIOSprite_New (OOTint pmPicID, SRCPOS *pmSrcPos)
{
    SpriteInfo		*mySpriteInfo;
    char		myAllocMessage [4096];
    static int		myStSpriteCount = 1;
    
    MIO_CheckOuputIsToWindow ("Sprite.New");
    MIO_CheckOuputWindowIsInGraphicsMode ("Sprite.New");
    MIO_MakePopupWindowVisible ();
    
    mySpriteInfo = (SpriteInfo *) malloc (sizeof (SpriteInfo));
    if (mySpriteInfo == NULL)
    {
	SET_ERRNO (E_PIC_CANT_ALLOC_MEM_FOR_PIC);
	return 0;
    }

//    mySpriteInfo -> picID = pmPicID;
    MIOPic_GetInfoForSprite (pmPicID, &(mySpriteInfo -> picWidth),
			     &(mySpriteInfo -> picHeight),
			     &(mySpriteInfo -> picTransparentColour),
			     &(mySpriteInfo -> picMDPicInfo));
    mySpriteInfo -> x = MIO_selectedRunWindowInfo -> width / 2;
    mySpriteInfo -> y = MIO_selectedRunWindowInfo -> height / 2;
    mySpriteInfo -> spriteRect = MyGetSpriteRect (mySpriteInfo);
    mySpriteInfo -> spriteHeight = FOREGROUND_HEIGHT + myStSpriteCount;
    mySpriteInfo -> centered = FALSE;
    mySpriteInfo -> visible = FALSE;
    myStSpriteCount++;

    MDIO_sprintf (myAllocMessage, "Allocated from picture %d", pmPicID);
    mySpriteInfo -> spriteID = MIO_IDAdd (SPRITE_ID, mySpriteInfo, pmSrcPos, 
					  myAllocMessage, NULL);

    // Couldn't allocate an identifier.  Return the default font ID.
    if (mySpriteInfo -> spriteID == 0)
    {
	MyFreeSprite (mySpriteInfo);
	return 0;
    }

    MIOPic_IncremementSpriteUse (pmPicID);

    MyAddToQueue (MIO_selectedRunWindowInfo, mySpriteInfo);

    return mySpriteInfo -> spriteID;
} // MIOSprite_New


/************************************************************************/
/* MIOSprite_RemoveSpritesFromWindow					*/
/************************************************************************/
void	MIOSprite_RemoveSpritesFromWindow (MIOWinInfoPtr pmInfo)
{
    SpriteInfo	*mySpriteInfo = (SpriteInfo *) (pmInfo -> spriteQueueHead);
    int		mySpriteID;

    while (mySpriteInfo != NULL)
    {
	mySpriteID = mySpriteInfo -> spriteID;
	mySpriteInfo = mySpriteInfo -> next;
	MIOSprite_Free (mySpriteID);
    }
} // MIOSprite_RemoveSpritesFromWindow


/************************************************************************/
/* MIOSprite_SetFrameRate						*/
/************************************************************************/
void	MIOSprite_SetFrameRate (int pmFrameRate)
{
    stSpriteInterval = 1000 / pmFrameRate;
} // MIOSprite_SetFrameRate


/************************************************************************/
/* MIOSprite_SetHeight							*/
/************************************************************************/
void	MIOSprite_SetHeight (OOTint pmSpriteID, OOTint pmHeight)
{
    SpriteInfo		*mySpriteInfo;
    MIOWinInfoPtr	myInfo;

    mySpriteInfo = (SpriteInfo *) MIO_IDGet (pmSpriteID, SPRITE_ID);
    myInfo = MIOWin_GetInfo (mySpriteInfo -> runWindow);

    MyRemoveFromQueue (myInfo, mySpriteInfo);

    mySpriteInfo -> spriteHeight = pmHeight;

    MyAddToQueue (myInfo, mySpriteInfo);
    
    if (mySpriteInfo -> visible) 
    {
	// If visible, a redisplay is in order
	MyAddDirtyRect (myInfo, mySpriteInfo -> spriteRect);
    }
} // MIOSprite_SetHeight


/************************************************************************/
/* MIOSprite_SetPosition						*/
/************************************************************************/
void	MIOSprite_SetPosition (OOTint pmSpriteID, OOTint pmX, OOTint pmY,
			       OOTint pmCentered)
{
    SpriteInfo		*mySpriteInfo;
    MIOWinInfoPtr	myInfo;

    mySpriteInfo = (SpriteInfo *) MIO_IDGet (pmSpriteID, SPRITE_ID);
    myInfo = MIOWin_GetInfo (mySpriteInfo -> runWindow);

    if (pmCentered) 
    {
	pmX -= mySpriteInfo -> picWidth / 2;
	pmY -= mySpriteInfo -> picHeight / 2;
    }
    mySpriteInfo -> centered = pmCentered;
    
    if ((mySpriteInfo -> x == pmX) && (mySpriteInfo -> y == pmY))
    {
	return;
    }
    
    if (mySpriteInfo -> visible) 
    {
	MyAddDirtyRect (myInfo, mySpriteInfo -> spriteRect);
    }

    mySpriteInfo -> x = pmX;
    mySpriteInfo -> y =  myInfo -> height - pmY - mySpriteInfo -> picHeight;
    mySpriteInfo -> spriteRect = MyGetSpriteRect (mySpriteInfo);

    if (mySpriteInfo -> visible) 
    {
	MyAddDirtyRect (myInfo, mySpriteInfo -> spriteRect);
    }
} // MIOSprite_SetPosition


/************************************************************************/
/* MIOSprite_Show							*/
/************************************************************************/
void	MIOSprite_Show (OOTint pmSpriteID)
{
    SpriteInfo		*mySpriteInfo;
    MIOWinInfoPtr	myInfo;

    mySpriteInfo = (SpriteInfo *) MIO_IDGet (pmSpriteID, SPRITE_ID);
    myInfo = MIOWin_GetInfo (mySpriteInfo -> runWindow);

    if (!mySpriteInfo -> visible) 
    {
	if (!stSpritesInUse)
	{
	    stSpritesInUse = TRUE;		// Start updating sprites
	    MDIOSprite_InitializeSpriteContext (myInfo);
	}
	mySpriteInfo -> visible = TRUE;
	MyAddDirtyRect (myInfo, mySpriteInfo -> spriteRect);
    }
} // MIOSprite_Show


/************************************************************************/
/* MIOSprite_UpdateIfNecessary						*/
/*									*/
/* Update the sprites if any have changed position.			*/
/************************************************************************/
void	MIOSprite_UpdateIfNecessary (BOOL pmCopyToOffscreen,
				     BOOL pmUpdateNow)
{
    // Go through the windows and update any that have dirty windows.
    WindowQueue	*myWindowQueuePtr;

    if (pmUpdateNow || 
	    (stSpritesInUse &&
		(MIOTime_GetTicks () > stSpriteLastTime + stSpriteInterval)))
    {

	myWindowQueuePtr = stSpriteWindowQueueHead;
	while (myWindowQueuePtr != NULL)
	{
	    if (myWindowQueuePtr -> windowInfoPtr -> dirtyRectHead != NULL)
	    {
		// Update window
		MDIOSprite_UpdateWindow (myWindowQueuePtr -> windowInfoPtr,
					 pmCopyToOffscreen);
	    }
	    myWindowQueuePtr = myWindowQueuePtr -> next;
	}
	stSpriteLastTime = MIOTime_GetTicks ();
    }
} // MIOSprite_UpdateIfNecessary


/***************************************/
/* External procedures for MIO library */
/***************************************/


/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/
/************************************************************************/
/* MyAddDirtyRect							*/
/*									*/
/* Add to the linked list of dirty rectangles.  The catch is that if	*/
/* there is an intersection with one or more already dirty rectangle, 	*/
/* then replace those rectangles with a new rectangle consisting of the	*/
/* intersection of all of these rectangles.				*/
/************************************************************************/
static void	MyAddDirtyRect (MIOWinInfoPtr pmInfoPtr, MYRECT pmRect)
{
    MYRECTLIST  *myDirtyRect, *myLastDirtyRect, *myNewRect;
    MYRECT	myWindowRect;

    myWindowRect.left = 0;
    myWindowRect.top = 0;
    myWindowRect.right = pmInfoPtr -> width - 1;
    myWindowRect.bottom = pmInfoPtr -> height - 1;
    
    if (!MIO_RectanglesSetIntersect (&pmRect, &myWindowRect))
    {
	return;
    }

    myDirtyRect = pmInfoPtr -> dirtyRectHead;
    while (myDirtyRect != NULL)
    {
	if (MIO_RectanglesIntersect (&myDirtyRect -> rect, &pmRect))
	{
	    // There is an intersection.  Replace the new rectangle with
	    // a union of the two and remove this rectangle from the
	    // dirty list.  Then start at the beginniong
	    MIO_RectanglesUnion (&pmRect, &myDirtyRect -> rect);
	    if (pmInfoPtr -> dirtyRectHead == myDirtyRect)
	    {
		pmInfoPtr -> dirtyRectHead = myDirtyRect -> next;
	    }
	    else
	    {
		myLastDirtyRect -> next = myDirtyRect -> next;
	    }
	    free (myDirtyRect);
	    myLastDirtyRect = myDirtyRect;
	    myDirtyRect = pmInfoPtr -> dirtyRectHead;
	}
	else
	{
	    myLastDirtyRect = myDirtyRect;
	    myDirtyRect = myDirtyRect -> next;
	}
    }

    // Add the new rect to the beginning of the list
    myNewRect = (MYRECTLIST *) malloc (sizeof (MYRECTLIST));
    myNewRect -> rect = pmRect;
    myNewRect -> next = pmInfoPtr -> dirtyRectHead;
    pmInfoPtr -> dirtyRectHead = myNewRect;
} // MyAddDirtyRect


/************************************************************************/
/* MyAddToQueue								*/
/*									*/
/* Add to the queue from lowest height to highest.			*/
/************************************************************************/
static void	MyAddToQueue (MIOWinInfoPtr pmInfoPtr, 
			      SpriteInfo *pmSpriteInfo)
{
    SpriteInfo	*myPtr, *myLastPtr;

    pmSpriteInfo -> runWindow = MIO_selectedRunWindow;

    myPtr = (SpriteInfo *) (pmInfoPtr -> spriteQueueHead);

    // If there were no sprites in the queue, (and there never has been) 
    // add this to the list of windows with sprites in it.
    if (myPtr == NULL)
    {
	WindowQueue	*myWQ = (WindowQueue *) malloc (sizeof (WindowQueue));
	myWQ -> windowInfoPtr = pmInfoPtr;
	myWQ -> next = stSpriteWindowQueueHead;
	stSpriteWindowQueueHead = myWQ;

	pmInfoPtr -> hasSprites = TRUE;
    }

    myLastPtr = NULL;

    while (myPtr != NULL)
    {
	if (pmSpriteInfo -> spriteHeight < myPtr -> spriteHeight)
	{
	    if (myLastPtr == NULL)
	    {
		// At head of queue
		pmInfoPtr -> spriteQueueHead = pmSpriteInfo;
	    }
	    else
	    {
		myLastPtr -> next = pmSpriteInfo;
	    }
	    pmSpriteInfo -> prev = myLastPtr;
	    pmSpriteInfo -> next = myPtr;
	    if (myPtr != NULL)
	    {
		myPtr -> prev = pmSpriteInfo;
	    }

	    return;
	}
	myLastPtr = myPtr;
	myPtr = myPtr -> next;
    }

    if (myLastPtr == NULL)
    {
	// At head of queue
	pmInfoPtr -> spriteQueueHead = pmSpriteInfo;
    }
    else
    {
	myLastPtr -> next = pmSpriteInfo;
    }
    pmSpriteInfo -> prev = myLastPtr;
    pmSpriteInfo -> next = NULL;
} // MyAddToQueue


/************************************************************************/
/* MyFreeSprite								*/
/************************************************************************/
static void	MyFreeSprite (SpriteInfo *pmSpriteInfo)
{
//    MDIOPic_FreePic (pmPicInfo -> mdPicInfo);
    free (pmSpriteInfo);
} // MyFreeSprite


/************************************************************************/
/* MyGetSpriteRect							*/
/************************************************************************/
static MYRECT	MyGetSpriteRect (SpriteInfo *pmSpriteInfo)
{
    MYRECT  myRect;

    myRect.left = pmSpriteInfo -> x;
    myRect.top = pmSpriteInfo -> y;
    myRect.right = myRect.left + pmSpriteInfo -> picWidth;
    myRect.bottom = myRect.top + pmSpriteInfo -> picHeight;

    return myRect;
} // MyGetSpriteRect


/************************************************************************/
/* MyRemoveFromQueue							*/
/*									*/
/* Add to the queue from lowest height to highest.			*/
/************************************************************************/
static void	MyRemoveFromQueue (MIOWinInfoPtr pmInfoPtr, 
				   SpriteInfo *pmSpriteInfo)
{
    pmSpriteInfo -> runWindow = NULL;

    if (pmSpriteInfo -> prev == NULL)
    {
	// At beginning of queue
	pmInfoPtr -> spriteQueueHead = pmSpriteInfo -> next;

	// If there are now no sprites in the queue, remove the window from
	// list of windows with sprites in it.
	if (pmInfoPtr -> spriteQueueHead == NULL)
	{
	    WindowQueue	*myWQ = stSpriteWindowQueueHead;
	    WindowQueue *myPrev = NULL;
	    while (myWQ -> windowInfoPtr != pmInfoPtr)
	    {
		myPrev = myWQ;
		myWQ = myWQ -> next;
	    }
	    if (myPrev == NULL)
	    {
		stSpriteWindowQueueHead = myWQ -> next;
	    }
	    else
	    {
		myPrev -> next = myWQ -> next;
	    }
	    free (myWQ);
	}
    }
    else
    {
	pmSpriteInfo -> prev -> next = pmSpriteInfo -> next;
    }
    if (pmSpriteInfo -> next != NULL)
    {
	pmSpriteInfo -> next -> prev = pmSpriteInfo -> prev;
    }
} // MyRemoveFromQueue
