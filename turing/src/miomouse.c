/**************/
/* miomouse.c */
/**************/

/*******************/
/* System includes */
/*******************/
#include <string.h>

/****************/
/* Self include */
/****************/
#include "miomouse.h"

/******************/
/* Other includes */
/******************/
#include "mio.h"
#include "mioerr.h"

#include "mdiowingraph.h"

#include "language.h"

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
static int  stMultiButtonMode;

/******************************/
/* Static callback procedures */
/******************************/

/*********************/
/* Static procedures */
/*********************/
static int	MyConvertButtonString (OOTstring pmString);

/***********************/
/* External procedures */
/***********************/
extern void	Language_Execute_EventQueueInsert (int pmStream, 
					struct EventDescriptor *pmEvent);
					
/************************************************************************/
/* MIOMouse_InitRun							*/
/************************************************************************/
void	MIOMouse_Init_Run (void)
{
    stMultiButtonMode = FALSE;
} // MIOMouse_Init_Run


/************************************************************************/
/* MIOMouse_ButtonChoose						*/
/************************************************************************/
void	MIOMouse_ButtonChoose (OOTstring pmString)
{
    if ((_stricmp (pmString, "singlebutton") == 0) ||
	(_stricmp (pmString, "onebutton") == 0))
    {
	stMultiButtonMode = FALSE;
    }
    else if (_stricmp (pmString, "multibutton") == 0)
    {
	stMultiButtonMode = TRUE;
    }
    else 
    {
	ABORT_WITH_ERRNO (E_MOUSE_BAD_BUTTONCHOOSE_STR);
    }
} // MIOMouse_ButtonChoose


/************************************************************************/
/* MIOMouse_ButtonMoved							*/
/************************************************************************/
int	MIOMouse_ButtonMoved (OOTstring pmString)
{
    MIO_CheckOuputIsToWindow ("Mouse.ButtonMoved");
    MIO_CheckOuputWindowIsInGraphicsMode ("Mouse.ButtonMoved");
    MIO_MakePopupWindowVisible ();

    return MIOMouse_ButtonMoved1 (MyConvertButtonString (pmString));
} // MIOMouse_ButtonMoved


/************************************************************************/
/* MIOMouse_ButtonMoved							*/
/************************************************************************/
int	MIOMouse_ButtonMoved1 (int pmDirection)
{
    MIOWinInfoPtr	myInfo = MIOWin_GetInfo (MIO_selectedRunWindow);
    int			myIndex = myInfo -> mouseBufferTail;
    MOUSEPOINT		*myPtr;

    while (myIndex != myInfo -> mouseBufferHead)
    {
	myPtr = &myInfo -> mouseBuffer [myIndex];
	if (myPtr -> buttonDirection & pmDirection)
	{
	    return TRUE;
	}
	myIndex = INC(myIndex);
    }

    return FALSE;
} // MIOMouse_ButtonMoved1


/************************************************************************/
/* MIOMouse_ButtonWait							*/
/************************************************************************/
void	MIOMouse_ButtonWait (OOTstring pmString, OOTint *pmX, OOTint *pmY, 
			     OOTint *pmButtonNumber, OOTint *pmButtonUpDown)
{
    int			myMotionType = MyConvertButtonString (pmString);
    MIOWinInfoPtr	myInfo = MIOWin_GetInfo (MIO_selectedRunWindow);
    MOUSEPOINT		*myPtr;
    EventDescriptor	myEvent;
    
    MIO_CheckOuputIsToWindow ("Mouse.ButtonWait");
    MIO_CheckOuputWindowIsInGraphicsMode ("Mouse.ButtonWait");
    MIO_MakePopupWindowVisible ();

    if (MIOMouse_ButtonMoved1 (myMotionType))
    {
    	while (myInfo -> mouseBufferTail != myInfo -> mouseBufferHead)
    	{
	    myPtr = &myInfo -> mouseBuffer [myInfo -> mouseBufferTail];
	    myInfo -> mouseBufferTail = INC(myInfo -> mouseBufferTail);
	    if (myPtr -> buttonDirection & myMotionType)
	    {
	    	*pmX = myPtr -> x;
	    	*pmY = myPtr -> y;
	    	*pmButtonNumber = 1;
	    	if (stMultiButtonMode)
	    	{
	    	    if (myPtr -> button == LEFT_BUTTON)
		    	*pmButtonNumber = 1;
		    else if (myPtr -> button == MIDDLE_BUTTON)
		    	*pmButtonNumber = 2;
		    else if (myPtr -> button == RIGHT_BUTTON)
		    	*pmButtonNumber = 3;
	    	}
	    	if (myPtr -> buttonDirection = BUTTON_UP)
	            *pmButtonUpDown = 1;
	    	else
	    	    *pmButtonUpDown = 0;
	    	
	    	return;
	    } // if (myPtr -> buttonDirection & myMotionType)    	
        } // while

	// TW - Internal Error!    	
    }

    // There is no mouse event in the queue.  Wait for the event
    myEvent.mode = EventMode_MouseButton;
    myEvent.count = myMotionType;
    Language_Execute_EventQueueInsert ((int) NULL, &myEvent);
} // MIOMouse_ButtonWait


/************************************************************************/
/* MIOMouse_Hide							*/
/************************************************************************/
void	MIOMouse_Hide (void)
{
    MIO_CheckOuputIsToWindow ("Mouse.ButtonMoved");
    MIO_CheckOuputWindowIsInGraphicsMode ("Mouse.ButtonMoved");
    MIO_MakePopupWindowVisible ();

    // TW Fill in
} // MIOMouse_Hide


/************************************************************************/
/* MIOMouse_SetPosition							*/
/************************************************************************/
void	MIOMouse_SetPosition (OOTint pmX, OOTint pmY)
{
    MIO_CheckOuputIsToWindow ("Mouse.SetPosition");
    MIO_CheckOuputWindowIsInGraphicsMode ("Mouse.SetPosition");
    MIO_MakePopupWindowVisible ();

    // TW Fill in
} // MIOMouse_SetPosition


/************************************************************************/
/* MIOMouse_Show							*/
/************************************************************************/
void	MIOMouse_Show (void)
{
    MIO_CheckOuputIsToWindow ("Mouse.Show");
    MIO_CheckOuputWindowIsInGraphicsMode ("Mouse.Show");
    MIO_MakePopupWindowVisible ();

    // TW Fill in
} // MIOMouse_Show


/************************************************************************/
/* MIOMouse_Where							*/
/************************************************************************/
void	MIOMouse_Where (OOTint *pmX, OOTint *pmY, OOTint *pmButtonUpDown)
{
    int	myButtonUpDown;
    
    MIO_CheckOuputIsToWindow ("Mouse.Where");
    MIO_CheckOuputWindowIsInGraphicsMode ("Mouse.Where");
    MIO_MakePopupWindowVisible ();

    MDIOWinGraph_GetMousePos (MIO_selectedRunWindow, pmX, pmY, &myButtonUpDown);

    // Convert the internal representation of the button to the Turing one.
    *pmButtonUpDown = 0;
    if (stMultiButtonMode)
    {
	if (myButtonUpDown & LEFT_BUTTON)
	    *pmButtonUpDown += 1;
	else if (myButtonUpDown & MIDDLE_BUTTON)
	    *pmButtonUpDown += 10;
	else if (myButtonUpDown & RIGHT_BUTTON)
	    *pmButtonUpDown += 100;
    }
    else
    {
    	if (myButtonUpDown != 0) 
    	    *pmButtonUpDown = 1;
    }
} // MIOMouse_Where


/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/
static int   MyConvertButtonString (OOTstring pmString)
{
    if (_stricmp ((char *) pmString, "up") == 0)
    {
	return BUTTON_UP;
    }
    else if (_stricmp ((char *) pmString, "down") == 0)
    {
	return BUTTON_DOWN;
    }
    else if ((_stricmp ((char *) pmString, "updown") == 0) ||
	     (_stricmp ((char *) pmString, "downup") == 0))
    {
	return BUTTON_UP_OR_DOWN;
    }
    
    ABORT_WITH_ERRNO (E_MOUSE_BAD_BUTTONMOTION_STR);
    
    return 0;
} // MyConvertButtonString