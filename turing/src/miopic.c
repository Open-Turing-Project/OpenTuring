/************/
/* miopic.c */
/************/

/*******************/
/* System includes */
/*******************/
#include <malloc.h>
#include <string.h>
#include <math.h>
#include <crtdbg.h>

/****************/
/* Self include */
/****************/
#include "miopic.h"

/******************/
/* Other includes */
/******************/
#include "mdiopic.h"
#include "mdiopicjpeg.h"
#include "mdiopicgif.h"

#include "mioerr.h"

#include "mio.h"

#include "mdio.h"

#include "miofile.h"
#include "miotime.h"

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
// These constants must match what is in the Pic.tu predefs file.
#define WIPE_LEFT_TO_RIGHT			 1
#define WIPE_RIGHT_TO_LEFT			 2
#define WIPE_TOP_TO_BOTTOM			 3
#define WIPE_BOTTOM_TO_TOP			 4
#define WIPE_CENTRE_TO_EDGE			 5
#define WIPE_EDGE_TO_CENTRE			 6
#define WIPE_UPPER_LEFT_TO_LOWER_RIGHT		 7
#define WIPE_LOWER_LEFT_TO_UPPER_RIGHT		 8
#define WIPE_LOWER_RIGHT_TO_UPPER_LEFT		 9
#define WIPE_UPPER_RIGHT_TO_LOWER_LEFT		10

#define WIPE_LEFT_TO_RIGHT_NO_BAR		21
#define WIPE_RIGHT_TO_LEFT_NO_BAR		22
#define WIPE_TOP_TO_BOTTOM_NO_BAR		23
#define WIPE_BOTTOM_TO_TOP_NO_BAR		24
#define WIPE_CENTRE_TO_EDGE_NO_BAR		25
#define WIPE_EDGE_TO_CENTRE_NO_BAR		26
#define WIPE_UPPER_LEFT_TO_LOWER_RIGHT_NO_BAR	27
#define WIPE_LOWER_LEFT_TO_UPPER_RIGHT_NO_BAR	28
#define WIPE_LOWER_RIGHT_TO_UPPER_LEFT_NO_BAR	29
#define WIPE_UPPER_RIGHT_TO_LOWER_LEFT_NO_BAR	30

#define GROW_LEFT_TO_RIGHT			41
#define GROW_RIGHT_TO_LEFT			42
#define GROW_TOP_TO_BOTTOM			43
#define GROW_BOTTOM_TO_TOP			44
#define GROW_CENTRE_TO_EDGE			45
#define GROW_UPPER_LEFT_TO_LOWER_RIGHT		47
#define GROW_LOWER_LEFT_TO_UPPER_RIGHT		48
#define GROW_LOWER_RIGHT_TO_UPPER_LEFT		49
#define GROW_UPPER_RIGHT_TO_LOWER_LEFT		50

#define GROW_LEFT_TO_RIGHT_NO_BAR		61
#define GROW_RIGHT_TO_LEFT_NO_BAR		62
#define GROW_TOP_TO_BOTTOM_NO_BAR		63
#define GROW_BOTTOM_TO_TOP_NO_BAR		64
#define GROW_CENTRE_TO_EDGE_NO_BAR		65
#define GROW_UPPER_LEFT_TO_LOWER_RIGHT_NO_BAR	67
#define GROW_LOWER_LEFT_TO_UPPER_RIGHT_NO_BAR	68
#define GROW_LOWER_RIGHT_TO_UPPER_LEFT_NO_BAR	69
#define GROW_UPPER_RIGHT_TO_LOWER_LEFT_NO_BAR	70

#define SLIDE_LEFT_TO_RIGHT			81
#define SLIDE_RIGHT_TO_LEFT			82
#define SLIDE_TOP_TO_BOTTOM			83
#define SLIDE_BOTTOM_TO_TOP			84

#define SLIDE_LEFT_TO_RIGHT_NO_BAR		91
#define SLIDE_RIGHT_TO_LEFT_NO_BAR		92
#define SLIDE_TOP_TO_BOTTOM_NO_BAR		93
#define SLIDE_BOTTOM_TO_TOP_NO_BAR		94

#define FADE_IN					99

#define BLEND				       100

/********************/
/* Global variables */
/********************/

/*********/
/* Types */
/*********/
typedef struct PicInfo
{
    int		height, width, version, size;
    int		extraMDInfo;
    int		transparentColour;
    PIC		mdPicInfo;
} PicInfo;

typedef struct DrawFramesEventData
{
    PicInfo	**picInfos;
    int		x, y;
    int		mode;
    int		currentFrame;
    int		maxFrames;
    int		delay;
    PIC		mdBackPicInfo;
    BOOL	eraseAfter;
} DrawFramesEventData;

typedef struct DrawSpecEventData
{
    PicInfo	*picInfo;
    PIC		mdPicInfo;
    PIC		mdBackPicInfo;
    int		x, y;
    int		mode;
    int		startTime;
    int		currentPos;
    int		maxPos;
    int		transition;
    int		duration;
    BOOL	hasBar;
    int		transitionType;
    int		direction;
} DrawSpecEventData;


/**********************/
/* External variables */
/**********************/

/********************/
/* Static constants */
/********************/
#define PIC_CONSTANT	    0xABCD
#define TAKEPIC_CONSTANT    0xABCF

/********************/
/* Static variables */
/********************/

/******************************/
/* Static callback procedures */
/******************************/

/*********************/
/* Static procedures */
/*********************/
static void	MyAdjustCoords (MIOWinInfoPtr pmInfo, OOTint *pmX1, 
				OOTint *pmY1, OOTint *pmX2, OOTint *pmY2);
static void	MyCheckMode (int pmMode);
static void	MyFreePic (PicInfo *pmPicInfo);
static int	MyGetFormat (const char *pmPathName);

/*********************************************/
/* External procedures for Turing predefines */
/*********************************************/
/************************************************************************/
/* MIOPic_Blend								*/
/************************************************************************/
OOTint	MIOPic_Blend (OOTint pmPicID1, OOTint pmPicID2, int pmPercent,
		      SRCPOS *pmSrcPos)
{
    PicInfo		*myPicInfo1, *myPicInfo2, *myResultPicInfo;
    char		myAllocMessage [4096];
    int			myPicID;

    MIO_CheckOuputIsToWindow ("Pic.Blend");
    MIO_CheckOuputWindowIsInGraphicsMode ("Pic.Blend");
    MIO_MakePopupWindowVisible ();

    myPicInfo1 = (PicInfo *) MIO_IDGet (pmPicID1, PIC_ID);
    myPicInfo2 = (PicInfo *) MIO_IDGet (pmPicID2, PIC_ID);

    if ((myPicInfo1 -> width != myPicInfo2 -> width) ||
	(myPicInfo1 -> height != myPicInfo2 -> height))
    {
	// TW - Error pictures not same size!
	return 0;
    }

    if ((pmPercent < 0) || (100 < pmPercent))
    {
	// TW - Error percent not from 0 to 100
	return 0;
    }

    myResultPicInfo = (PicInfo *) malloc (sizeof (PicInfo));
    if (myResultPicInfo == NULL)
    {
	SET_ERRNO (E_PIC_CANT_ALLOC_MEM_FOR_PIC);
	return 0;
    }

    if (!MDIOPic_CreatePicByBlend (myPicInfo1 -> mdPicInfo, 
		myPicInfo2 -> mdPicInfo, 
		myPicInfo1 -> width, myPicInfo1 -> height, 
		pmPercent, &myResultPicInfo -> mdPicInfo))
    {
    	// Error set in MDIOPic_GetImage
	return 0;
    }
    
    myResultPicInfo -> width = myPicInfo1 -> width;
    myResultPicInfo -> height = myPicInfo1 -> height;
    myResultPicInfo -> version = PIC_CONSTANT;
    myResultPicInfo -> extraMDInfo = MDIOPic_GetExtraInfo ();
    myResultPicInfo -> size = 0;
    myResultPicInfo -> transparentColour = 0;

    MDIO_sprintf (myAllocMessage, 
		  "[%dx%d] [%dKB] Blended from picture IDs %d and %d", 
		  myResultPicInfo -> width, myResultPicInfo -> height, 
		  MDIOPic_GetMemSize (myResultPicInfo -> mdPicInfo),
		  pmPicID1, pmPicID2);
    myPicID = MIO_IDAdd (PIC_ID, myResultPicInfo, pmSrcPos, myAllocMessage, 
	NULL);

    // Couldn't allocate an identifier.  Return the default font ID.
    if (myPicID == 0)
    {
	MyFreePic (myResultPicInfo);
	return 0;
    }
        	    
    return myPicID;
} // MIOPic_Blend


/************************************************************************/
/* MIOPic_Blur								*/
/************************************************************************/
OOTint	MIOPic_Blur (OOTint pmPicID, int pmIterations, SRCPOS *pmSrcPos)
{
    PicInfo		*myPicInfo, *myResultPicInfo;
    char		myAllocMessage [4096];
    int			myPicID;

    MIO_CheckOuputIsToWindow ("Pic.Blur");
    MIO_CheckOuputWindowIsInGraphicsMode ("Pic.Blur");
    MIO_MakePopupWindowVisible ();

    myPicInfo = (PicInfo *) MIO_IDGet (pmPicID, PIC_ID);

    myResultPicInfo = (PicInfo *) malloc (sizeof (PicInfo));
    if (myResultPicInfo == NULL)
    {
	SET_ERRNO (E_PIC_CANT_ALLOC_MEM_FOR_PIC);
	return 0;
    }

    if (!MDIOPic_CreatePicByBlur (myPicInfo -> mdPicInfo, 
		myPicInfo -> width, myPicInfo -> height, pmIterations,
		&myResultPicInfo -> mdPicInfo))
    {
    	// Error set in MDIOPic_GetImage
	return 0;
    }
    
    myResultPicInfo -> width = myPicInfo -> width;
    myResultPicInfo -> height = myPicInfo -> height;
    myResultPicInfo -> version = PIC_CONSTANT;
    myResultPicInfo -> extraMDInfo = MDIOPic_GetExtraInfo ();
    myResultPicInfo -> size = 0;
    myResultPicInfo -> transparentColour = 0;

    MDIO_sprintf (myAllocMessage, 
		  "[%dx%d] [%dKB] Blur of picture ID %d", 
		  myResultPicInfo -> width, myResultPicInfo -> height, 
		  MDIOPic_GetMemSize (myResultPicInfo -> mdPicInfo),
		  pmPicID);
    myPicID = MIO_IDAdd (PIC_ID, myResultPicInfo, pmSrcPos, myAllocMessage, 
	NULL);

    // Couldn't allocate an identifier.  Return the default font ID.
    if (myPicID == 0)
    {
	MyFreePic (myResultPicInfo);
	return 0;
    }

    return myPicID;
} // MIOPic_Blur


/************************************************************************/
/* MIOPic_DecremementSpriteUse						*/
/*									*/
/* This ensures that we don't free pictures that are being used by	*/
/* sprites.								*/
/************************************************************************/
void	MIOPic_DecremementSpriteUse (int pmPicID)
{
    PicInfo	*myPicInfo;

    myPicInfo = MIO_IDGet (pmPicID, PIC_ID);

    MDIOPic_DecrementUse (myPicInfo -> mdPicInfo);
} // MIOPic_DecremementSpriteUse


/************************************************************************/
/* MIOPic_Draw								*/
/************************************************************************/
void	MIOPic_Draw (OOTint pmPicID, OOTint pmX, OOTint pmY, OOTint pmMode)
{
    int		myHeight = MIO_selectedRunWindowInfo -> height;
    PicInfo	*myPicInfo;
    int		myY;
        
    MIO_CheckOuputIsToWindow ("Pic.Draw");
    MIO_CheckOuputWindowIsInGraphicsMode ("Pic.Draw");
    MIO_MakePopupWindowVisible ();
    MyCheckMode (pmMode);
    
    myPicInfo = (PicInfo *) MIO_IDGet (pmPicID, PIC_ID);
    
    myY = myHeight - pmY - myPicInfo -> height;
    
    if (((0 <= pmX + myPicInfo -> width) &&
	 (pmX < MIO_selectedRunWindowInfo -> width)) &&
	((0 <= myY + myPicInfo -> height) &&
	 (myY < MIO_selectedRunWindowInfo -> height)))
    {
        MDIOPic_Draw (myPicInfo -> mdPicInfo, NULL, (int) pmX, myY, 
		      myPicInfo -> width, myPicInfo -> height, 
		      myPicInfo -> transparentColour, (int) pmMode);
    }
} // MIOPic_Draw


/************************************************************************/
/* MIOPic_DrawPic							*/
/************************************************************************/
void	MIOPic_DrawPic (OOTint pmX, OOTint pmY, PICBUFFER pmBuffer, 
			OOTint pmMode)
{
    int		myHeight = MIO_selectedRunWindowInfo -> height;
    PicInfo	*myPicInfo;
    int		myY;
        
    MIO_CheckOuputIsToWindow ("drawpic");
    MIO_CheckOuputWindowIsInGraphicsMode ("drawpic");
    MIO_MakePopupWindowVisible ();
    MyCheckMode (pmMode);
    
    myPicInfo = (PicInfo *) pmBuffer;
    
    myY = myHeight - pmY - myPicInfo -> height;
    
    MDIOPic_DrawPic (&myPicInfo -> mdPicInfo, (int) pmX, myY, myPicInfo -> width,
    		     myPicInfo -> height, myPicInfo -> transparentColour, 
		     (int) pmMode);
} // MIOPic_DrawPic


/************************************************************************/
/* MIOPic_DrawFrames							*/
/*									*/
/* The system will continuously call MIOPic_EventCheckDrawFrames until	*/
/* it returns true.							*/
/************************************************************************/
BOOL	MIOPic_DrawFrames (struct EventDescriptor *pmEvent,
			   OOTint *pmPicIDs, OOTint pmX, OOTint pmY, OOTint pmMode, 
			   OOTint pmNumFrames, OOTint pmArraySize, 
			   OOTint pmDelayBetweenFrames, OOTboolean pmEraseAfter)
{
    DrawFramesEventData	*myEventData;
    char		myMessage [1024];
    int			myHeight = MIO_selectedRunWindowInfo -> height;
    PicInfo		*myPicInfo;
    int			myBGX1, myBGX2, myBGY1, myBGY2, myY;
    PIC			myMDBackPicInfo = NULL;
    int			cnt;

    if (pmNumFrames > pmArraySize)
    {
	MDIO_sprintf (myMessage, 
	    "Array size of %d is smaller than the number of frames (%d)",
	    pmArraySize, pmNumFrames);
    	ABORT_WITH_ERRMSG (E_PIC_BAD_ARRAY_SIZE, myMessage);
	return FALSE;
    }

    MIO_CheckOuputIsToWindow ("Pic.DrawFrames");
    MIO_CheckOuputWindowIsInGraphicsMode ("Pic.DrawFrames");
    MIO_MakePopupWindowVisible ();
    MyCheckMode (pmMode);
    
    myPicInfo = (PicInfo *) MIO_IDGet (pmPicIDs [0], PIC_ID);

    // We need to check that all the other picIDs are correct, because if we
    // try to cause an abort while Turing is in the event check, Turing exits
    // immediately (we abort the executor, not the Turing program!)  So, we
    // check all the picIDs here rather than later.
    
    myY = myHeight - pmY - myPicInfo -> height;
    
    // Take a picture of the background, then draw the first frame.
    myBGX1 = pmX;
    myBGX2 = pmX + myPicInfo -> width - 1;
    myBGY1 = pmY;
    myBGY2 = pmY + myPicInfo -> height - 1;

    if ((pmMode == MODE_MERGE) || (pmMode == MODE_UNDER_MERGE) || pmEraseAfter)
    {
	MyAdjustCoords (MIO_selectedRunWindowInfo, &myBGX1, &myBGY1, &myBGX2, &myBGY2);

	if (!MDIOPic_GetImage (myBGX1, myBGY1, myBGX2, myBGY2, &myMDBackPicInfo))
	{
	    // Error set in MDIOPic_GetImage
	    return FALSE;
	}
    }

    myEventData = (DrawFramesEventData *) malloc (sizeof (DrawFramesEventData));
    myEventData -> picInfos = malloc (sizeof (PicInfo *) * pmNumFrames);
    for (cnt = 0 ; cnt < pmNumFrames ; cnt++)
    {
	myEventData -> picInfos [cnt] = (PicInfo *) MIO_IDGet (pmPicIDs [cnt], PIC_ID);
	// Make certain the pictures are not freed up while we're in background!
	MDIOPic_IncrementUse (myEventData -> picInfos [cnt] -> mdPicInfo);
    }
    myEventData -> x = pmX;
    myEventData -> y = myY;
    myEventData -> mode = pmMode;
    myEventData -> currentFrame = 0;
    myEventData -> maxFrames = pmNumFrames;
    myEventData -> delay = pmDelayBetweenFrames;
    myEventData -> mdBackPicInfo = myMDBackPicInfo;
    myEventData -> eraseAfter = pmEraseAfter;

    if (((0 <= pmX + myPicInfo -> width) &&
	 (pmX < MIO_selectedRunWindowInfo -> width)) &&
	((0 <= myY + myPicInfo -> height) &&
	 (myY < MIO_selectedRunWindowInfo -> height)))
    {
        MDIOPic_Draw (myPicInfo -> mdPicInfo, NULL, (int) pmX, myY, 
		      myPicInfo -> width, myPicInfo -> height, 
		      myPicInfo -> transparentColour, (int) pmMode);
    	MDIOPic_DecrementUse (myEventData -> picInfos [0] -> mdPicInfo);

	pmEvent -> mode = EventMode_PicDrawFrames;
	pmEvent -> count = MIOTime_GetTicks () + pmDelayBetweenFrames;
	pmEvent -> data = myEventData;

	return TRUE;
    }

    return FALSE;
} // MIOPic_DrawFrames

/************************************************************************/
/* MIOPic_DrawSpecial							*/
/*									*/
/* Implements special effects.						*/
/************************************************************************/
BOOL	MIOPic_DrawSpecial (struct EventDescriptor *pmEvent,
			    OOTint pmPicID, OOTint pmX, OOTint pmY, 
			    OOTint pmMode, OOTint pmTransition,
			    OOTint pmDuration)
{
    DrawSpecEventData	*myEventData;
    char		myMessage [1024];
    int			myHeight = MIO_selectedRunWindowInfo -> height;
    PicInfo		*myPicInfo;
    int			myBGX1, myBGX2, myBGY1, myBGY2;
    int			myY;
    PIC			myMDPicInfo = NULL;
    PIC			myMDBackPicInfo = NULL;
    PIC			myMDTempPicInfo = NULL;
    int			myPercent;

    MIO_CheckOuputIsToWindow ("Pic.DrawSpecial");
    MIO_CheckOuputWindowIsInGraphicsMode ("Pic.DrawSpecial");
    MIO_MakePopupWindowVisible ();
    MyCheckMode (pmMode);
    
    myPicInfo = (PicInfo *) MIO_IDGet (pmPicID, PIC_ID);
    
    myY = myHeight - pmY - myPicInfo -> height;

    if (pmMode == MODE_COPY)
    {
	// Make certain the picture is not freed up while we're in background!
	myMDPicInfo = myPicInfo -> mdPicInfo;

	MDIOPic_IncrementUse (myMDPicInfo);
    }
    else
    {
	// Ignoring return code for now
	if (!MDIOPic_CreatePicByDraw (myPicInfo -> mdPicInfo, pmX, myY, 
				      myPicInfo -> width, myPicInfo -> height,
				      myPicInfo ->transparentColour, pmMode,
				      &myMDPicInfo))
	{
	    return FALSE;
	}
    }

    if ((BLEND <= pmTransition) && (pmTransition <= BLEND + 100))
    {
	// if we're doing a blend, then we don't need to go the
	// whole event checking route.  It's all done as one shot.
	if (pmTransition == BLEND)
	{
	    myPercent = 70;
	}
	else
	{
	    myPercent = pmTransition - BLEND;
	}
	MDIOPic_DrawTransitionBlend (
		myMDPicInfo, NULL, pmX, myY,
		myPicInfo -> width, myPicInfo -> height,
		myPercent);
	MDIOPic_DecrementUse (myMDPicInfo);
	return FALSE;
    }

    if (((SLIDE_LEFT_TO_RIGHT <= pmTransition) &&
	 (pmTransition <= SLIDE_BOTTOM_TO_TOP_NO_BAR)) ||
	((GROW_LEFT_TO_RIGHT <= pmTransition) &&
	 (pmTransition <= GROW_BOTTOM_TO_TOP)) ||
	((GROW_LEFT_TO_RIGHT_NO_BAR <= pmTransition) &&
	 (pmTransition <= GROW_BOTTOM_TO_TOP_NO_BAR)) ||
	 (pmTransition == FADE_IN))
    {
	// Take a picture of the background, if necessary
	myBGX1 = pmX;
	myBGX2 = pmX + myPicInfo -> width - 1;
	myBGY1 = myY;
	myBGY2 = myY + myPicInfo -> height - 1;
	
	if (!MDIOPic_GetImage (myBGX1, myBGY1, myBGX2, myBGY2, &myMDBackPicInfo))
	{
	    // Error set in MDIOPic_GetImage
	    return FALSE;
	}
    }

    myEventData = (DrawSpecEventData *) malloc (sizeof (DrawSpecEventData));
    myEventData -> picInfo = myPicInfo;
    myEventData -> mdPicInfo = myMDPicInfo;
    myEventData -> mdBackPicInfo = myMDBackPicInfo;
    myEventData -> x = pmX;
    myEventData -> y = myY;
    myEventData -> mode = pmMode;
    myEventData -> startTime = MIOTime_GetTicks ();
    myEventData -> currentPos = 0;
    myEventData -> transition = pmTransition;
    myEventData -> duration = pmDuration;

    if (((WIPE_LEFT_TO_RIGHT_NO_BAR <= pmTransition) && 
	 (pmTransition <= WIPE_UPPER_RIGHT_TO_LOWER_LEFT_NO_BAR)) ||
	((GROW_LEFT_TO_RIGHT_NO_BAR <= pmTransition) && 
	 (pmTransition <= GROW_UPPER_RIGHT_TO_LOWER_LEFT_NO_BAR)) ||
	((SLIDE_LEFT_TO_RIGHT_NO_BAR <= pmTransition) && 
	 (pmTransition <= SLIDE_BOTTOM_TO_TOP_NO_BAR)))
    {
	myEventData -> hasBar = FALSE;
    }
    else
    {
	myEventData -> hasBar = TRUE;
    }

    if ((WIPE_LEFT_TO_RIGHT <= pmTransition) && 
	(pmTransition <= WIPE_UPPER_RIGHT_TO_LOWER_LEFT_NO_BAR))
    {
	myEventData -> transitionType = MODE_WIPE;
    }
    else if ((GROW_LEFT_TO_RIGHT <= pmTransition) && 
	     (pmTransition <= GROW_UPPER_RIGHT_TO_LOWER_LEFT_NO_BAR))
    {
	myEventData -> transitionType = MODE_GROW;
    }
    else if ((SLIDE_LEFT_TO_RIGHT <= pmTransition) && 
	     (pmTransition <= SLIDE_BOTTOM_TO_TOP_NO_BAR))
    {
	myEventData -> transitionType = MODE_SLIDE;
    }

    switch (pmTransition)
    {
	case WIPE_LEFT_TO_RIGHT:
	case SLIDE_LEFT_TO_RIGHT:
	case GROW_LEFT_TO_RIGHT:
	case WIPE_LEFT_TO_RIGHT_NO_BAR:
	case SLIDE_LEFT_TO_RIGHT_NO_BAR:
	case GROW_LEFT_TO_RIGHT_NO_BAR:
	    myEventData -> direction = MOVE_RIGHT;
	    break;
	case WIPE_RIGHT_TO_LEFT:
	case SLIDE_RIGHT_TO_LEFT:
	case GROW_RIGHT_TO_LEFT:
	case WIPE_RIGHT_TO_LEFT_NO_BAR:
	case SLIDE_RIGHT_TO_LEFT_NO_BAR:
	case GROW_RIGHT_TO_LEFT_NO_BAR:
	    myEventData -> direction = MOVE_LEFT;
	    break;
	case WIPE_TOP_TO_BOTTOM:
	case SLIDE_TOP_TO_BOTTOM:
	case GROW_TOP_TO_BOTTOM:
	case WIPE_TOP_TO_BOTTOM_NO_BAR:
	case SLIDE_TOP_TO_BOTTOM_NO_BAR:
	case GROW_TOP_TO_BOTTOM_NO_BAR:
	    myEventData -> direction = MOVE_DOWN;
	    break;
	case WIPE_BOTTOM_TO_TOP:
	case SLIDE_BOTTOM_TO_TOP:
	case GROW_BOTTOM_TO_TOP:
	case WIPE_BOTTOM_TO_TOP_NO_BAR:
	case SLIDE_BOTTOM_TO_TOP_NO_BAR:
	case GROW_BOTTOM_TO_TOP_NO_BAR:
	    myEventData -> direction = MOVE_UP;
	    break;
	case WIPE_UPPER_LEFT_TO_LOWER_RIGHT:
	case GROW_UPPER_LEFT_TO_LOWER_RIGHT:
	case WIPE_UPPER_LEFT_TO_LOWER_RIGHT_NO_BAR:
	case GROW_UPPER_LEFT_TO_LOWER_RIGHT_NO_BAR:
	    myEventData -> direction = FROM_UPPER_LEFT;
	    break;
	case WIPE_UPPER_RIGHT_TO_LOWER_LEFT:
	case GROW_UPPER_RIGHT_TO_LOWER_LEFT:
	case WIPE_UPPER_RIGHT_TO_LOWER_LEFT_NO_BAR:
	case GROW_UPPER_RIGHT_TO_LOWER_LEFT_NO_BAR:
	    myEventData -> direction = FROM_UPPER_RIGHT;
	    break;
	case WIPE_LOWER_LEFT_TO_UPPER_RIGHT:
	case GROW_LOWER_LEFT_TO_UPPER_RIGHT:
	case WIPE_LOWER_LEFT_TO_UPPER_RIGHT_NO_BAR:
	case GROW_LOWER_LEFT_TO_UPPER_RIGHT_NO_BAR:
	    myEventData -> direction = FROM_LOWER_LEFT;
	    break;
	case WIPE_LOWER_RIGHT_TO_UPPER_LEFT:
	case GROW_LOWER_RIGHT_TO_UPPER_LEFT:
	case WIPE_LOWER_RIGHT_TO_UPPER_LEFT_NO_BAR:
	case GROW_LOWER_RIGHT_TO_UPPER_LEFT_NO_BAR:
	    myEventData -> direction = FROM_LOWER_RIGHT;
	    break;
	case WIPE_CENTRE_TO_EDGE:
	case WIPE_EDGE_TO_CENTRE:
	case GROW_CENTRE_TO_EDGE:
	case WIPE_CENTRE_TO_EDGE_NO_BAR:
	case WIPE_EDGE_TO_CENTRE_NO_BAR:
	case GROW_CENTRE_TO_EDGE_NO_BAR:
	case FADE_IN:
	    break;
	default:
	    // Error: unrcognized transition
	    MDIO_sprintf (myMessage, 
		"Unknown transition %d passed to Pic.DrawSpecial",
		pmTransition);
    	    ABORT_WITH_ERRMSG (E_PIC_UNKNOWN_TRANSITION, myMessage);
	    return FALSE;
    } // switch

    // The maxPos depends on the transition
    switch (pmTransition)
    {
	case WIPE_LEFT_TO_RIGHT:
	case WIPE_RIGHT_TO_LEFT:
	case SLIDE_LEFT_TO_RIGHT:
	case SLIDE_RIGHT_TO_LEFT:
	case GROW_LEFT_TO_RIGHT:
	case GROW_RIGHT_TO_LEFT:
	    // maxPos = width + black tranition bar of WIPE_BAR_WIDTH
	    myEventData -> maxPos = myPicInfo -> width + WIPE_BAR_WIDTH;
	    break;
	case WIPE_LEFT_TO_RIGHT_NO_BAR:
	case WIPE_RIGHT_TO_LEFT_NO_BAR:
	case SLIDE_LEFT_TO_RIGHT_NO_BAR:
	case SLIDE_RIGHT_TO_LEFT_NO_BAR:
	case GROW_LEFT_TO_RIGHT_NO_BAR:
	case GROW_RIGHT_TO_LEFT_NO_BAR:
	    // maxPos = width
	    myEventData -> maxPos = myPicInfo -> width;
	    break;
	case WIPE_TOP_TO_BOTTOM:
	case WIPE_BOTTOM_TO_TOP:
	case SLIDE_TOP_TO_BOTTOM:
	case SLIDE_BOTTOM_TO_TOP:
	case GROW_TOP_TO_BOTTOM:
	case GROW_BOTTOM_TO_TOP:
	    // maxPos = height + black tranition bar of WIPE_BAR_WIDTH
	    myEventData -> maxPos = myPicInfo -> height + WIPE_BAR_WIDTH;
	    break;
	case WIPE_TOP_TO_BOTTOM_NO_BAR:
	case WIPE_BOTTOM_TO_TOP_NO_BAR:
	case SLIDE_TOP_TO_BOTTOM_NO_BAR:
	case SLIDE_BOTTOM_TO_TOP_NO_BAR:
	case GROW_TOP_TO_BOTTOM_NO_BAR:
	case GROW_BOTTOM_TO_TOP_NO_BAR:
	    // maxPos = height
	    myEventData -> maxPos = myPicInfo -> height;
	    break;
	case WIPE_CENTRE_TO_EDGE:
	case WIPE_EDGE_TO_CENTRE:
	case WIPE_UPPER_LEFT_TO_LOWER_RIGHT:
	case WIPE_LOWER_LEFT_TO_UPPER_RIGHT:
	case WIPE_LOWER_RIGHT_TO_UPPER_LEFT:
	case WIPE_UPPER_RIGHT_TO_LOWER_LEFT:
	case GROW_CENTRE_TO_EDGE:
	case GROW_UPPER_LEFT_TO_LOWER_RIGHT:
	case GROW_LOWER_LEFT_TO_UPPER_RIGHT:
	case GROW_LOWER_RIGHT_TO_UPPER_LEFT:
	case GROW_UPPER_RIGHT_TO_LOWER_LEFT:
	    // maxPos = max (height, width)
	    myEventData -> maxPos = 
		MAX (myPicInfo -> height + 1, myPicInfo -> width + 1) / 2 +
		WIPE_BAR_WIDTH;
	    break;
	case WIPE_CENTRE_TO_EDGE_NO_BAR:
	case WIPE_EDGE_TO_CENTRE_NO_BAR:
	case WIPE_UPPER_LEFT_TO_LOWER_RIGHT_NO_BAR:
	case WIPE_LOWER_LEFT_TO_UPPER_RIGHT_NO_BAR:
	case WIPE_LOWER_RIGHT_TO_UPPER_LEFT_NO_BAR:
	case WIPE_UPPER_RIGHT_TO_LOWER_LEFT_NO_BAR:
	case GROW_CENTRE_TO_EDGE_NO_BAR:
	case GROW_UPPER_LEFT_TO_LOWER_RIGHT_NO_BAR:
	case GROW_LOWER_LEFT_TO_UPPER_RIGHT_NO_BAR:
	case GROW_LOWER_RIGHT_TO_UPPER_LEFT_NO_BAR:
	case GROW_UPPER_RIGHT_TO_LOWER_LEFT_NO_BAR:
	    // maxPos = max (height, width)
	    myEventData -> maxPos = 
		MAX (myPicInfo -> height + 1, myPicInfo -> width + 1) / 2;
	    break;
	case FADE_IN:
	    // maxPos = height
	    myEventData -> maxPos = 100;
	    break;
	default:
	    // Error: unrcognized transition
	    MDIO_sprintf (myMessage, 
		"Unknown transition %d passed to Pic.DrawSpecial",
		pmTransition);
    	    ABORT_WITH_ERRMSG (E_PIC_UNKNOWN_TRANSITION, myMessage);
	    return FALSE;
    }

    if (((0 <= pmX + myPicInfo -> width) &&
	 (pmX < MIO_selectedRunWindowInfo -> width)) &&
	((0 <= myY + myPicInfo -> height) &&
	 (myY < MIO_selectedRunWindowInfo -> height)))
    {
	pmEvent -> mode = EventMode_PicDrawSpecial;
	pmEvent -> count = 0;
	pmEvent -> data = myEventData;

	return TRUE;
    }

    return FALSE;
} // MIOPic_DrawSpecial


/************************************************************************/
/* MIOPic_EventCheckDrawFrames						*/
/************************************************************************/
BOOL	MIOPic_EventCheckDrawFrames (struct EventDescriptor *pmEvent)
{
    DrawFramesEventData	*myEventData;
    PicInfo		*myPicInfo;
    
    if (pmEvent -> count > MIOTime_GetTicks ())
    {
	return FALSE;
    }

    myEventData = (DrawFramesEventData *) (pmEvent -> data);

    myEventData -> currentFrame++;
    if (myEventData -> currentFrame >= myEventData -> maxFrames)
    {
	// We've reached the last frame
	// Redraw the background, if asked for
	if (myEventData -> eraseAfter)
	{
	    myPicInfo = myEventData -> picInfos [0];
    
	    MDIOPic_Draw (myEventData -> mdBackPicInfo, NULL, 
			  myEventData -> x, myEventData -> y, 
			  myPicInfo -> width, myPicInfo -> height, 
			  0, MODE_COPY);
	}

	// Free up data structures allocated in MIOPic_DrawFrames
	if (myEventData -> mdBackPicInfo != NULL)
	{
	    MDIOPic_DecrementUse (myEventData -> mdBackPicInfo);
	}
	free (myEventData);
	pmEvent -> data = NULL;
	return TRUE;
    }

    myPicInfo = myEventData -> picInfos [myEventData -> currentFrame];
    
    MDIOPic_Draw (myPicInfo -> mdPicInfo, myEventData -> mdBackPicInfo, 
		  myEventData -> x, myEventData -> y, 
		  myPicInfo -> width, myPicInfo -> height, 
		  myPicInfo -> transparentColour, myEventData -> mode);

    // Free up the picture used in the frame now that we're done with it
    MDIOPic_DecrementUse (myEventData -> picInfos [myEventData -> currentFrame] -> mdPicInfo);

    pmEvent -> count += myEventData -> delay;

    return FALSE;
} // MIOPic_EventCheckDrawFrames


/************************************************************************/
/* MIOPic_EventCheckDrawSpecial						*/
/************************************************************************/
BOOL	MIOPic_EventCheckDrawSpecial (struct EventDescriptor *pmEvent)
{
    DrawSpecEventData	*myEventData;
    PicInfo		*myPicInfo;
    int			myPos;

    myEventData = (DrawSpecEventData *) (pmEvent -> data);

    myPos = (MIOTime_GetTicks () - myEventData -> startTime) *
	myEventData -> maxPos / myEventData -> duration;
    if (myPos == myEventData -> currentPos)
    {
	// Not enough time has passed to make a change
	return FALSE;
    }

    myPos = MIN (myPos, myEventData -> maxPos);
    myPicInfo = myEventData -> picInfo;

    switch (myEventData -> transition)
    {
	case WIPE_LEFT_TO_RIGHT:
	case WIPE_LEFT_TO_RIGHT_NO_BAR:
	case WIPE_RIGHT_TO_LEFT:
	case WIPE_RIGHT_TO_LEFT_NO_BAR:
	case GROW_LEFT_TO_RIGHT:
	case GROW_LEFT_TO_RIGHT_NO_BAR:
	case GROW_RIGHT_TO_LEFT:
	case GROW_RIGHT_TO_LEFT_NO_BAR:
	case SLIDE_LEFT_TO_RIGHT:
	case SLIDE_LEFT_TO_RIGHT_NO_BAR:
	case SLIDE_RIGHT_TO_LEFT:
	case SLIDE_RIGHT_TO_LEFT_NO_BAR:
	    MDIOPic_DrawTransitionLeftRight (
		myEventData -> mdPicInfo, 
		myEventData -> mdBackPicInfo, 
	        myEventData -> x, myEventData -> y, 
		myPicInfo -> width, myPicInfo -> height, 
		myEventData -> currentPos, myPos, 
		myEventData -> hasBar, myEventData -> direction,
		myEventData -> transitionType);
	    break;
	case WIPE_TOP_TO_BOTTOM:
	case WIPE_TOP_TO_BOTTOM_NO_BAR:
	case WIPE_BOTTOM_TO_TOP:
	case WIPE_BOTTOM_TO_TOP_NO_BAR:
	case GROW_TOP_TO_BOTTOM:
	case GROW_TOP_TO_BOTTOM_NO_BAR:
	case GROW_BOTTOM_TO_TOP:
	case GROW_BOTTOM_TO_TOP_NO_BAR:
	case SLIDE_TOP_TO_BOTTOM:
	case SLIDE_TOP_TO_BOTTOM_NO_BAR:
	case SLIDE_BOTTOM_TO_TOP:
	case SLIDE_BOTTOM_TO_TOP_NO_BAR:
	    MDIOPic_DrawTransitionTopBottom (
		myEventData -> mdPicInfo, 
		myEventData -> mdBackPicInfo, 
	        myEventData -> x, myEventData -> y, 
		myPicInfo -> width, myPicInfo -> height, 
		myEventData -> currentPos, myPos, 
		myEventData -> hasBar, myEventData -> direction,
		myEventData -> transitionType);
	    break;
	case WIPE_CENTRE_TO_EDGE:
	case WIPE_CENTRE_TO_EDGE_NO_BAR:
	case GROW_CENTRE_TO_EDGE:
	case GROW_CENTRE_TO_EDGE_NO_BAR:
	    MDIOPic_DrawTransitionCentreToEdge (
		myEventData -> mdPicInfo, 
		myEventData -> mdBackPicInfo, 
	        myEventData -> x, myEventData -> y, 
		myPicInfo -> width, myPicInfo -> height, 
		myPos, myEventData -> hasBar, 
		myEventData -> transitionType);
	    break;
	case WIPE_EDGE_TO_CENTRE:
	case WIPE_EDGE_TO_CENTRE_NO_BAR:
	    // TW
	    break;
	case WIPE_UPPER_LEFT_TO_LOWER_RIGHT:
	case WIPE_UPPER_LEFT_TO_LOWER_RIGHT_NO_BAR:
	case WIPE_LOWER_LEFT_TO_UPPER_RIGHT:
	case WIPE_LOWER_LEFT_TO_UPPER_RIGHT_NO_BAR:
	case WIPE_LOWER_RIGHT_TO_UPPER_LEFT:
	case WIPE_LOWER_RIGHT_TO_UPPER_LEFT_NO_BAR:
	case WIPE_UPPER_RIGHT_TO_LOWER_LEFT:
	case WIPE_UPPER_RIGHT_TO_LOWER_LEFT_NO_BAR:
	case GROW_UPPER_LEFT_TO_LOWER_RIGHT:
	case GROW_UPPER_LEFT_TO_LOWER_RIGHT_NO_BAR:
	case GROW_LOWER_LEFT_TO_UPPER_RIGHT:
	case GROW_LOWER_LEFT_TO_UPPER_RIGHT_NO_BAR:
	case GROW_LOWER_RIGHT_TO_UPPER_LEFT:
	case GROW_LOWER_RIGHT_TO_UPPER_LEFT_NO_BAR:
	case GROW_UPPER_RIGHT_TO_LOWER_LEFT:
	case GROW_UPPER_RIGHT_TO_LOWER_LEFT_NO_BAR:
	    MDIOPic_DrawTransitionCornerToCorner (
		myEventData -> mdPicInfo, 
		myEventData -> mdBackPicInfo, 
	        myEventData -> x, myEventData -> y, 
		myPicInfo -> width, myPicInfo -> height, 
		myPos, myEventData -> hasBar, myEventData -> direction,
		myEventData -> transitionType);
	    break;
	case FADE_IN:
	    MDIOPic_DrawTransitionBlend (
		myEventData -> mdPicInfo, 
		myEventData -> mdBackPicInfo, 
	        myEventData -> x, myEventData -> y, 
		myPicInfo -> width, myPicInfo -> height, 
		myPos);
	    break;
    }
    myEventData -> currentPos = myPos;

    if (myEventData -> currentPos == myEventData -> maxPos)
    {
	// Free up data structures allocated in MIOPic_DrawSpecial
	MDIOPic_DecrementUse (myEventData -> mdPicInfo);
	if (myEventData -> mdBackPicInfo != NULL)
	{
	    MDIOPic_DecrementUse (myEventData -> mdBackPicInfo);
	}
	free (myEventData);
	pmEvent -> data = NULL;
	return TRUE;
    }

    return FALSE;
} // MIOPic_EventCheckDrawSpecial


/************************************************************************/
/* MIOPic_FileNew							*/
/************************************************************************/
OOTint	MIOPic_FileNew (const OOTstring pmPathName, SRCPOS *pmSrcPos)
{
    char    	myFullPathName [1024], myAllocMessage [4096];
    int	    	myFormat;
    PicInfo	*myPicInfo;
    int		myPicID;
    BOOL	myResult;
        
    MIO_CheckOuputIsToWindow ("Pic.FileNew");
    MIO_CheckOuputWindowIsInGraphicsMode ("Pic.FileNew");
    
    if (!MIOFile_ConvertPath (pmPathName, NULL, myFullPathName, 
    			      NO_TRAILING_SLASH))
    {
	return 0;
    }
    myFormat = MyGetFormat (myFullPathName);

    // This won't occur unless I eventually add code to allow MIO aborts
    // to become merely error settings.
    if (myFormat == UNKNOWN_FORMAT)
    {
    	return 0;
    }
    
    myPicInfo = (PicInfo *) malloc (sizeof (PicInfo));
    if (myPicInfo == NULL)
    {
	SET_ERRNO (E_PIC_CANT_ALLOC_MEM_FOR_PIC);
	return 0;
    }

    myPicInfo -> transparentColour = 0;

    if (myFormat == BMP)
    {
        myResult = MDIOPic_LoadBMP (myFullPathName, &myPicInfo -> width, 
    			            &myPicInfo -> height, 
    			            &myPicInfo -> mdPicInfo);
    }
    else if (myFormat == GIF)
    {
        myResult = MDIOPic_LoadGIF (myFullPathName, &myPicInfo -> width, 
    			            &myPicInfo -> height, 
				    &myPicInfo -> transparentColour,
    			            &myPicInfo -> mdPicInfo);
    }
    else if (myFormat == JPG)
    {
        myResult = MDIOPic_LoadJPEG (myFullPathName, &myPicInfo -> width, 
    			             &myPicInfo -> height, 
    			             &myPicInfo -> mdPicInfo);
    }
    
    if (!myResult)
    {
    	// Error set in MDIOPic_Loadxxx
    	return 0;
    }

    myPicInfo -> version = PIC_CONSTANT;
    myPicInfo -> extraMDInfo = MDIOPic_GetExtraInfo ();
    myPicInfo -> size = 0;

    MDIO_sprintf (myAllocMessage, "[%dx%d] [%dKB] Loaded from \"%s\"", 
		  myPicInfo -> width, myPicInfo -> height, 
		  MDIOPic_GetMemSize (myPicInfo -> mdPicInfo),
		  myFullPathName);
    myPicID = MIO_IDAdd (PIC_ID, myPicInfo, pmSrcPos, myAllocMessage, NULL);

    // Couldn't allocate an identifier.  Return the default font ID.
    if (myPicID == 0)
    {
	MyFreePic (myPicInfo);
	return 0;
    }
        	    
    return myPicID;
} // MIOPic_FileNew


/************************************************************************/
/* MIOPic_FileNewFrames							*/
/************************************************************************/
void	MIOPic_FileNewFrames (OOTint *pmPicIDs, OOTint pmArraySize,
			      const OOTstring pmPathName, OOTint *pmDelayTime, 
			      SRCPOS *pmSrcPos)
{
    char    	myFullPathName [1024], myAllocMessage [4096];
    char	myMessage [1024];
    int	    	myFormat;
    int		myNumFrames;
    PicInfo	*myPicInfo;
    int		myWidth, myHeight, myTransparentColour;
    PIC		*myMDPicInfos = NULL;
    BOOL	myResult;
    int		cnt;
    
    MIO_CheckOuputIsToWindow ("Pic.FileNewFrames");
    MIO_CheckOuputWindowIsInGraphicsMode ("Pic.FileNewFrames");
    
    if (!MIOFile_ConvertPath (pmPathName, NULL, myFullPathName, 
    			      NO_TRAILING_SLASH))
    {
	return;
    }
    myFormat = MyGetFormat (myFullPathName);

    // This won't occur unless I eventually add code to allow MIO aborts
    // to become merely error settings.
    if (myFormat == UNKNOWN_FORMAT)
    {
    	return;
    }

    if ((myFormat == BMP) || (myFormat == JPG))
    {
	if (pmArraySize < 1)
	{
	    MDIO_sprintf (myMessage, 
		"Array size of %d is smaller than the number of frames (1) in '%s'",
		pmArraySize, myFullPathName);
    	    ABORT_WITH_ERRMSG (E_PIC_BAD_ARRAY_SIZE, myMessage);
	    return;
	}

	pmPicIDs [0] = MIOPic_FileNew (pmPathName, pmSrcPos);
	return;
    }

    myNumFrames = MIOPic_Frames (pmPathName);

    if (pmArraySize < myNumFrames)
    {
	MDIO_sprintf (myMessage, 
	    "Array size of %d is smaller than the number of frames (%d) in '%s'",
	    pmArraySize, myNumFrames, myFullPathName);
	ABORT_WITH_ERRMSG (E_PIC_BAD_ARRAY_SIZE, myMessage);
	return;
    }

    // Allocate the arrays for the widths, heights, transparent colours
    // and machine dependent info
    myMDPicInfos = (PIC) malloc (sizeof (PIC) * myNumFrames);
    if (myMDPicInfos == NULL)
    {
	SET_ERRNO (E_PIC_CANT_ALLOC_MEM_FOR_PIC);
	return;
    }

    myTransparentColour = 0;

    myResult = MDIOPic_LoadGIFFrames (myFullPathName, &myWidth, &myHeight,
				      &myTransparentColour, myMDPicInfos,
				      myNumFrames, (int *) pmDelayTime);
    if (!myResult)
    {
    	// Error set in MDIOPic_Loadxxx
    	return;
    }

    for (cnt = 0 ; cnt < myNumFrames ; cnt++)
    {
	myPicInfo = (PicInfo *) malloc (sizeof (PicInfo));
	myPicInfo -> width = myWidth;
	myPicInfo -> height = myHeight;
	myPicInfo -> transparentColour = myTransparentColour;
	myPicInfo -> mdPicInfo = myMDPicInfos [cnt];
	myPicInfo -> version = PIC_CONSTANT;
	myPicInfo -> extraMDInfo = MDIOPic_GetExtraInfo ();
	myPicInfo -> size = 0;
	MDIO_sprintf (myAllocMessage, "[%dx%d] [%dKB] Loaded from \"%s\"", 
		  myPicInfo -> width, myPicInfo -> height, 
		  MDIOPic_GetMemSize (myPicInfo -> mdPicInfo),
		  myFullPathName);
	pmPicIDs [cnt] = MIO_IDAdd (PIC_ID, myPicInfo, pmSrcPos, myAllocMessage, NULL);
    
	// Couldn't allocate an identifier.  Return the default font ID.
	if (pmPicIDs [cnt] == 0)
	{
	    MyFreePic (myPicInfo);
	}
    }
} // MIOPic_FileNewFrames


/************************************************************************/
/* MIOPic_FileSave							*/
/************************************************************************/
void	MIOPic_FileSave (OOTint pmPicID, const OOTstring pmPathName)
{
    char    	myFullPathName [1024];
    int	    	myFormat;
    PicInfo	*myPicInfo;
        
    MIO_CheckOuputIsToWindow ("Pic.FileSave");
    MIO_CheckOuputWindowIsInGraphicsMode ("Pic.FileSave");
    
    if (!MIOFile_ConvertPath (pmPathName, NULL, myFullPathName, 
    			      NO_TRAILING_SLASH))
    {
	return;
    }
    myFormat = MyGetFormat (myFullPathName);

    // This won't occur unless I eventually add code to allow MIO aborts
    // to become merely error settings.
    if (myFormat == UNKNOWN_FORMAT)
    {
    	return;
    }
    
    myPicInfo = (PicInfo *) MIO_IDGet (pmPicID, PIC_ID);
    
    MDIOPic_SaveImage (myFullPathName, myPicInfo -> mdPicInfo);
} // MIOPic_FileSave


/************************************************************************/
/* MIOPic_Flip								*/
/************************************************************************/
OOTint	MIOPic_Flip (OOTint pmSrcPicID, BOOL pmVertical, SRCPOS *pmSrcPos)
{
    PicInfo	*mySrcPicInfo, *myDestPicInfo;
    int		myNewWidth, myNewHeight;
    char	myAllocMessage [4096];
    int		myDestPicID;
        
    MIO_CheckOuputIsToWindow ("Pic.Flip{Vertical,Horizontal}");
    MIO_CheckOuputWindowIsInGraphicsMode ("Pic.Flip{Vertical,Horizontal}");
    MIO_MakePopupWindowVisible ();
    
    mySrcPicInfo = (PicInfo *) MIO_IDGet (pmSrcPicID, PIC_ID);
    
    myDestPicInfo = (PicInfo *) malloc (sizeof (PicInfo));
    if (myDestPicInfo == NULL)
    {
	SET_ERRNO (E_PIC_CANT_ALLOC_MEM_FOR_PIC);
	return 0;
    }

    if (pmVertical) 
    {
    	myNewWidth = mySrcPicInfo -> width;
    	myNewHeight = -mySrcPicInfo -> height;
    }
    else
    {
    	myNewWidth = -mySrcPicInfo -> width;
    	myNewHeight = mySrcPicInfo -> height;
    }
    
    if (!MDIOPic_Scale (mySrcPicInfo -> mdPicInfo, &myDestPicInfo -> mdPicInfo,
    		        mySrcPicInfo -> width, mySrcPicInfo -> height,
    		        myNewWidth, myNewHeight))
    {
    	// Error set in MDIOPic_Scale
    	return 0;
    }

    myDestPicInfo -> width = mySrcPicInfo -> width;
    myDestPicInfo -> height = mySrcPicInfo -> height;
    myDestPicInfo -> version = PIC_CONSTANT;
    myDestPicInfo -> extraMDInfo = MDIOPic_GetExtraInfo ();
    myDestPicInfo -> size = 0;
    myDestPicInfo -> transparentColour = mySrcPicInfo -> transparentColour;

    if (pmVertical)
    {
	MDIO_sprintf (myAllocMessage, 
		      "[%dx%d] [%dKB] Vertically flipped from ID %d", 
		      myDestPicInfo -> width, myDestPicInfo -> height, 
		      MDIOPic_GetMemSize (myDestPicInfo -> mdPicInfo),
		      pmSrcPicID);
    }
    else
    {
	MDIO_sprintf (myAllocMessage, 
		      "[%dx%d] [%dKB] Horizontally flipped from ID %d", 
		      myDestPicInfo -> width, myDestPicInfo -> height, 
		      MDIOPic_GetMemSize (myDestPicInfo -> mdPicInfo),
		      pmSrcPicID);
    }
    myDestPicID = MIO_IDAdd (PIC_ID, myDestPicInfo, pmSrcPos, myAllocMessage, 
			     NULL);

    // Couldn't allocate an identifier.  Return the default font ID.
    if (myDestPicID == 0)
    {
	MyFreePic (myDestPicInfo);
	return 0;
    }
        	    
    return myDestPicID;
} // MIOPic_Flip


/************************************************************************/
/* MIOPic_Frames							*/
/************************************************************************/
int	MIOPic_Frames (const OOTstring pmPathName)
{
    char    	myFullPathName [1024];
    int	    	myFormat;

    MIO_CheckOuputIsToWindow ("Pic.Frames");
    MIO_CheckOuputWindowIsInGraphicsMode ("Pic.Frames");
    
    if (!MIOFile_ConvertPath (pmPathName, NULL, myFullPathName, 
    			      NO_TRAILING_SLASH))
    {
	return 0;
    }

    myFormat = MyGetFormat (myFullPathName);

    // This won't occur unless I eventually add code to allow MIO aborts
    // to become merely error settings.
    if (myFormat == UNKNOWN_FORMAT)
    {
	return 0;
    }
    
    if ((myFormat == BMP) || (myFormat == JPG))
    {
	return 1;
    }
    else if (myFormat == GIF)
    {
	return MDIOPic_GetGIFFrames (myFullPathName);
    }
    else
    {
	return 0;
    }
} // MIOPic_Frames


/************************************************************************/
/* MIOPic_Free								*/
/************************************************************************/
void	MIOPic_Free (OOTint pmPicID)
{
    PicInfo	*myPicInfo;

    myPicInfo = MIO_IDGet (pmPicID, PIC_ID);

    MyFreePic (myPicInfo);
    MIO_IDRemove (pmPicID, PIC_ID);
} // MIOPic_Free


/************************************************************************/
/* MIOPic_GetDimensions							*/
/************************************************************************/
void	MIOPic_GetDimensions (OOTint pmPicID, OOTint *pmWidth, OOTint *pmHeight)
{
    PicInfo	*myPicInfo;

    myPicInfo = MIO_IDGet (pmPicID, PIC_ID);
    *pmWidth = myPicInfo -> width;
    *pmHeight = myPicInfo -> height;
} // MIOPic_GetDimensions


/************************************************************************/
/* MIOPic_GetInfoForSprite						*/
/************************************************************************/
void	MIOPic_GetInfoForSprite (OOTint pmPicID, OOTint *pmWidth, 
				 OOTint *pmHeight, int *pmTransparentColour,
				 PIC *pmMDPicInfo)
{
    PicInfo	*myPicInfo;

    myPicInfo = MIO_IDGet (pmPicID, PIC_ID);
    *pmWidth = myPicInfo -> width;
    *pmHeight = myPicInfo -> height;
    *pmTransparentColour = myPicInfo -> transparentColour;
    *pmMDPicInfo = myPicInfo -> mdPicInfo;
} // MIOPic_GetInfoForSprite


/************************************************************************/
/* MIOPic_IncremementSpriteUse						*/
/*									*/
/* This ensures that we don't free pictures that are being used by	*/
/* sprites.								*/
/************************************************************************/
void	MIOPic_IncremementSpriteUse (int pmPicID)
{
    PicInfo	*myPicInfo;

    myPicInfo = MIO_IDGet (pmPicID, PIC_ID);

    MDIOPic_IncrementUse (myPicInfo -> mdPicInfo);
} // MIOPic_IncremementSpriteUse


/************************************************************************/
/* MIOPic_New								*/
/************************************************************************/
OOTint	MIOPic_New (OOTint pmX1, OOTint pmY1, OOTint pmX2, OOTint pmY2, 
		    SRCPOS *pmSrcPos)
{
    PicInfo		*myPicInfo;
    char		myAllocMessage [4096];
    int			myPicID;
        
    MIO_CheckOuputIsToWindow ("Pic.New");
    MIO_CheckOuputWindowIsInGraphicsMode ("Pic.New");
    MIO_MakePopupWindowVisible ();
    
    MyAdjustCoords (MIO_selectedRunWindowInfo, &pmX1, &pmY1, &pmX2, &pmY2);
    
    myPicInfo = (PicInfo *) malloc (sizeof (PicInfo));
    if (myPicInfo == NULL)
    {
	SET_ERRNO (E_PIC_CANT_ALLOC_MEM_FOR_PIC);
	return 0;
    }

    if (!MDIOPic_GetImage (pmX1, pmY1, pmX2, pmY2, &myPicInfo -> mdPicInfo))
    {
    	// Error set in MDIOPic_GetImage
    	return 0;
    }

    myPicInfo -> width = pmX2 - pmX1 + 1;
    myPicInfo -> height = pmY2 - pmY1 + 1;
    myPicInfo -> version = PIC_CONSTANT;
    myPicInfo -> extraMDInfo = MDIOPic_GetExtraInfo ();
    myPicInfo -> size = 0;
    myPicInfo -> transparentColour = 0;

    MDIO_sprintf (myAllocMessage, 
		  "[%dx%d] [%dKB] From screen location (%d,%d) - (%d,%d)", 
		  myPicInfo -> width, myPicInfo -> height, 
		  MDIOPic_GetMemSize (myPicInfo -> mdPicInfo),
		  pmX1, pmY1, pmX2, pmY2);
    myPicID = MIO_IDAdd (PIC_ID, myPicInfo, pmSrcPos, myAllocMessage, NULL);

    // Couldn't allocate an identifier.  Return the default font ID.
    if (myPicID == 0)
    {
	MyFreePic (myPicInfo);
	return 0;
    }
        	    
    return myPicID;
} // MIOPic_New


/************************************************************************/
/* MIOPic_Rotate							*/
/************************************************************************/
OOTint	MIOPic_Rotate (OOTint pmSrcPicID, OOTint pmAngle, OOTint pmXAxis,
		       OOTint pmYAxis, SRCPOS *pmSrcPos)
{
    PicInfo	*mySrcPicInfo, *myDestPicInfo;
    char	myAllocMessage [4096];
    int		myDestPicID;
        
    MIO_CheckOuputIsToWindow ("Pic.Rotate");
    MIO_CheckOuputWindowIsInGraphicsMode ("Pic.Rotate");
    MIO_MakePopupWindowVisible ();
    
    mySrcPicInfo = (PicInfo *) MIO_IDGet (pmSrcPicID, PIC_ID);
    
    myDestPicInfo = (PicInfo *) malloc (sizeof (PicInfo));
    if (myDestPicInfo == NULL)
    {
	SET_ERRNO (E_PIC_CANT_ALLOC_MEM_FOR_PIC);
	return 0;
    }

    if (!MDIOPic_Rotate (mySrcPicInfo -> mdPicInfo, 
    			 &myDestPicInfo -> mdPicInfo,
    		       	 mySrcPicInfo -> width, mySrcPicInfo -> height,
    		       	 &myDestPicInfo -> width, &myDestPicInfo -> height,
    		       	 pmAngle, pmXAxis, pmYAxis, 
    		       	 mySrcPicInfo -> transparentColour))
    {    		       	 
    	return 0;
    }
    
    myDestPicInfo -> version = PIC_CONSTANT;
    myDestPicInfo -> extraMDInfo = MDIOPic_GetExtraInfo ();
    myDestPicInfo -> size = 0;
    myDestPicInfo -> transparentColour = mySrcPicInfo -> transparentColour;

    MDIO_sprintf (myAllocMessage, 
		  "[%dx%d] [%dKB] Rotated %d degrees from ID %d", 
		  myDestPicInfo -> width, myDestPicInfo -> height, 
		  MDIOPic_GetMemSize (myDestPicInfo -> mdPicInfo),
		  pmAngle, pmSrcPicID);
    myDestPicID = MIO_IDAdd (PIC_ID, myDestPicInfo, pmSrcPos, myAllocMessage, 
			     NULL);

    // Couldn't allocate an identifier.  Return the default font ID.
    if (myDestPicID == 0)
    {
	MyFreePic (myDestPicInfo);
	return 0;
    }
        	    
    return myDestPicID;
} // MIOPic_Rotate


/************************************************************************/
/* MIOPic_Scale								*/
/************************************************************************/
OOTint	MIOPic_Scale (OOTint pmSrcPicID, OOTint pmNewWidth, OOTint pmNewHeight, 
		      SRCPOS *pmSrcPos)
{
    PicInfo		*mySrcPicInfo, *myDestPicInfo;
    char		myAllocMessage [4096];
    int			myDestPicID;
        
    MIO_CheckOuputIsToWindow ("Pic.Scale");
    MIO_CheckOuputWindowIsInGraphicsMode ("Pic.Scale");
    MIO_MakePopupWindowVisible ();
    
    if ((pmNewWidth == 0) || (pmNewHeight == 0))
    {
    	SET_ERRNO (E_PIC_SCALE_TOO_SMALL);
    	return 0;
    }
    if ((pmNewWidth > 4096) || (pmNewHeight > 4096))
    {
    	SET_ERRNO (E_PIC_SCALE_TOO_LARGE);
    	return 0;
    }
    mySrcPicInfo = (PicInfo *) MIO_IDGet (pmSrcPicID, PIC_ID);
    
    myDestPicInfo = (PicInfo *) malloc (sizeof (PicInfo));
    if (myDestPicInfo == NULL)
    {
	SET_ERRNO (E_PIC_CANT_ALLOC_MEM_FOR_PIC);
	return 0;
    }

    if (!MDIOPic_Scale (mySrcPicInfo -> mdPicInfo, 
    			&myDestPicInfo -> mdPicInfo,
    		       	mySrcPicInfo -> width, mySrcPicInfo -> height,
    		       	pmNewWidth, pmNewHeight))
    {
    	// Error set in MDIOPic_Flip
    	return 0;
    }

    myDestPicInfo -> width = abs (pmNewWidth);
    myDestPicInfo -> height = abs (pmNewHeight);
    myDestPicInfo -> version = PIC_CONSTANT;
    myDestPicInfo -> extraMDInfo = MDIOPic_GetExtraInfo ();
    myDestPicInfo -> size = 0;
    myDestPicInfo -> transparentColour = mySrcPicInfo -> transparentColour;

    MDIO_sprintf (myAllocMessage, 
		  "[%dx%d] [%dKB] Scaled from ID %d", 
		  myDestPicInfo -> width, myDestPicInfo -> height, 
		  MDIOPic_GetMemSize (myDestPicInfo -> mdPicInfo),
		  pmSrcPicID);
    myDestPicID = MIO_IDAdd (PIC_ID, myDestPicInfo, pmSrcPos, myAllocMessage, 
			     NULL);

    // Couldn't allocate an identifier.  Return the default font ID.
    if (myDestPicID == 0)
    {
	MyFreePic (myDestPicInfo);
	return 0;
    }
        	    
    return myDestPicID;
} // MIOPic_Scale


/************************************************************************/
/* MIOPic_SetTransparentColour						*/
/*									*/
/* Set the colour that will be transparent in picMerge mode in Pic.Draw	*/
/* operations.								*/
/************************************************************************/
void	MIOPic_SetTransparentColour (OOTint pmPicID, OOTint pmColourNum)
{
    PicInfo	*myPicInfo;
        
    myPicInfo = (PicInfo *) MIO_IDGet (pmPicID, PIC_ID);
    MIO_CheckColourRange (pmColourNum);        

    myPicInfo -> transparentColour = pmColourNum;
} // MIOPic_SetTransparentColour


/************************************************************************/
/* MIOPic_SizePic							*/
/*									*/
/* Return the memory for the picture (in bytes)				*/
/************************************************************************/
int	MIOPic_SizePic (OOTint pmX1, OOTint pmY1, OOTint pmX2, OOTint pmY2)
{
    MIO_CheckOuputIsToWindow ("sizepic");
    MIO_CheckOuputWindowIsInGraphicsMode ("sizepic");
    
    MyAdjustCoords (MIO_selectedRunWindowInfo, &pmX1, &pmY1, &pmX2, &pmY2);
    
    return (sizeof (PicInfo) + MDIOPic_SizePic (pmX1, pmY1, pmX2, pmY2));
} // MIOPic_SizePic


/************************************************************************/
/* MIOPic_TakePic							*/
/*									*/
/* The TakePic buffer looks like a PicBufferInfo + machine dep buffer	*/
/* + a check byte							*/
/************************************************************************/
void	MIOPic_TakePic (OOTint pmX1, OOTint pmY1, OOTint pmX2, OOTint pmY2, 
			PICBUFFER pmBuffer)
{
    PicInfo	*myPicInfo = (PicInfo *) pmBuffer;
        
    MIO_CheckOuputIsToWindow ("takepic");
    MIO_CheckOuputWindowIsInGraphicsMode ("takepic");
    
    MyAdjustCoords (MIO_selectedRunWindowInfo, &pmX1, &pmY1, &pmX2, &pmY2);
    
    myPicInfo = (PicInfo *) pmBuffer;

    if (!MDIOPic_TakePic (pmX1, pmY1, pmX2, pmY2, &myPicInfo -> mdPicInfo))
    {
    	// There's no provision for failure in a takepic
	ABORT_WITH_ERRNO (E_PIC_FAILURE);
    	return;
    }

    myPicInfo -> width = pmX2 - pmX1 + 1;
    myPicInfo -> height = pmY2 - pmY1 + 1;
    myPicInfo -> version = TAKEPIC_CONSTANT;
    myPicInfo -> extraMDInfo = MDIOPic_GetExtraInfo ();
    myPicInfo -> size = 0;
    myPicInfo -> transparentColour = 0;
} // MIOPic_TakePic


/***************************************/
/* External procedures for MIO library */
/***************************************/
/************************************************************************/
/* MIOPic_Init								*/
/************************************************************************/
void	MIOPic_Init (void)
{
    MDIOPic_Init ();
} // MIOPic_Init


/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/
/************************************************************************/
/* MyAdjustCoords							*/
/************************************************************************/
static void	MyAdjustCoords (MIOWinInfoPtr pmInfo, OOTint *pmX1, 
				OOTint *pmY1, OOTint *pmX2, OOTint *pmY2)
{				
    int	mySwap;
        
    // Make certain pmX1 < pmX2
    if (*pmX2 < *pmX1)
    {
	mySwap = *pmX2;
	*pmX2 = *pmX1;
	*pmX1 = mySwap;
    }
    
    // Adjust pmY1 to computer coords and check pmY1 < pmY2
    if (*pmY1 < *pmY2)
    {
	mySwap = pmInfo -> height - 1 - *pmY2;
	*pmY2 = pmInfo -> height - 1 - *pmY1;
	*pmY1 = mySwap;
    }
    else
    {
	*pmY1 = pmInfo -> height - 1 - *pmY1;
	*pmY2 = pmInfo -> height - 1 - *pmY2;
    }
} // MyAdjustCoords


/************************************************************************/
/* MyCheckMode								*/
/************************************************************************/
static void	MyCheckMode (int pmMode)
{
    if ((pmMode < MODE_COPY) || (MODE_XOR_MERGE < pmMode))
    {
    	ABORT_WITH_ERRNO (E_PIC_MODE_OUT_OF_BOUNDS);
    }
} // MyCheckMode


/************************************************************************/
/* MyFreePic								*/
/************************************************************************/
static void	MyFreePic (PicInfo *pmPicInfo)
{
    MDIOPic_DecrementUse (pmPicInfo -> mdPicInfo);
    free (pmPicInfo);
} // MyFreePic


/************************************************************************/
/* MyGetFormat								*/
/************************************************************************/
static int	MyGetFormat (const char *pmPathName)
{
    const char	*myPtr = strrchr (pmPathName, '.');
    int		myFormat;
        
    if (myPtr == NULL)
    {
    	myFormat = MDIOPic_DefaultFormat ();
    	return myFormat;
    }
    
    myPtr++;
    
    if (_stricmp (myPtr, "BMP") == 0)
    {
    	myFormat = BMP;
    }
    else if (_stricmp (myPtr, "GIF") == 0)
    {
    	myFormat = GIF;
    }
    else if (_stricmp (myPtr, "JPG") == 0)
    {
    	myFormat = JPG;
    }
    else if (_stricmp (myPtr, "JPE") == 0)
    {
    	myFormat = JPG;
    }
    else if (_stricmp (myPtr, "JPEG") == 0)
    {
    	myFormat = JPG;
    }
    else if (_stricmp (myPtr, "PNG") == 0)
    {
    	myFormat = PNG;
    }
    else if (_stricmp (myPtr, "PCX") == 0)
    {
    	myFormat = PCX;
    }
    else if (_stricmp (myPtr, "PICT") == 0)
    {
    	myFormat = PCT;
    }
    else if (_stricmp (myPtr, "PCT") == 0)
    {
    	myFormat = PCT;
    }
    else
    {
    	char	myMessage [1024];
    	
    	MDIO_sprintf (myMessage, "Unknown graphics file extension '%s'", myPtr);
    		    
    	ABORT_WITH_ERRMSG (E_PIC_UNKNOWN_FILE_FORMAT, myMessage);
    	
	return UNKNOWN_FORMAT;
    }    	

    if (!MDIOPic_IsSupportedFormat (myFormat))
    {
    	char	myMessage [1024], *myFormatString;
    	
    	switch (myFormat)
    	{
    	    case BMP:
    	        myFormatString = "BMP";
    	        break;
    	    case GIF:
    	        myFormatString = "GIF";
    	        break;
    	    case JPG:
    	        myFormatString = "JPEG";
    	        break;
    	    case PNG:
    	        myFormatString = "PNG";
    	        break;
    	    case PCX:
    	        myFormatString = "PCX";
    	        break;
    	    case PCT:
    	        myFormatString = "Macintosh PICT";
    	        break;
    	} // switch
    	
    	MDIO_sprintf (myMessage, 
    		      "%s graphics files are not supported at this time",
    		      myFormatString);
    			
	ABORT_WITH_ERRMSG (E_PIC_UNSUPPORTED_FILE_FORMAT, myMessage);
	
	return UNKNOWN_FORMAT;
    }
    
    return myFormat;
} // MyGetFormat



