/*************/
/* mioview.c */
/*************/

/*******************/
/* System includes */
/*******************/

/****************/
/* Self include */
/****************/
#include "mioview.h"

/******************/
/* Other includes */
/******************/
#include "mio.h"
#include "miowin.h"

#include "miorgb.h"

#include "mdiowingraph.h"

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

/******************************/
/* Static callback procedures */
/******************************/

/*********************/
/* Static procedures */
/*********************/


/***********************/
/* External procedures */
/***********************/
/************************************************************************/
/* MIOView_ClipAdd							*/
/************************************************************************/
void	MIOView_ClipAdd (OOTint pmX1, OOTint pmY1, OOTint pmX2, OOTint pmY2)
{
    MIOWinInfoPtr	myInfo = MIOWin_GetInfo (MIO_selectedRunWindow);

    MDIOWinGraph_ClipAdd (MIO_selectedRunWindow, pmX1, 
			  myInfo -> height - pmY1 - 1, pmX2, 
			  myInfo -> height - pmY2 - 1);
} // MIOView_ClipAdd


/************************************************************************/
/* MIOView_ClipAddOval							*/
/************************************************************************/
void	MIOView_ClipAddOval (OOTint pmX, OOTint pmY, OOTint pmXR, OOTint pmYR)
{
    MIOWinInfoPtr	myInfo = MIOWin_GetInfo (MIO_selectedRunWindow);

    MDIOWinGraph_ClipAddOval (MIO_selectedRunWindow, pmX, 
			      myInfo -> height - pmY - 1, pmXR, pmYR);
} // MIOView_ClipAddOval


/************************************************************************/
/* MIOView_ClipOff							*/
/************************************************************************/
void	MIOView_ClipOff (void)
{
    MDIOWinGraph_ClipClear (MIO_selectedRunWindow);
} // MIOView_ClipOff


/************************************************************************/
/* MIOView_ClipSet							*/
/************************************************************************/
void	MIOView_ClipSet (OOTint pmX1, OOTint pmY1, OOTint pmX2, OOTint pmY2)
{
    MIOWinInfoPtr	myInfo = MIOWin_GetInfo (MIO_selectedRunWindow);

    MDIOWinGraph_ClipSet (MIO_selectedRunWindow, pmX1, 
			  myInfo -> height - pmY1 - 1, pmX2, 
			  myInfo -> height - pmY2 - 1);
} // MIOView_ClipSet


/************************************************************************/
/* MIOView_GetMaxx							*/
/************************************************************************/
int	MIOView_GetMaxx (void)
{
    return MIOWin_GetInfo (MIO_selectedRunWindow) -> width - 1;
} // MIOView_GetMaxx


/************************************************************************/
/* MIOView_GetMaxy							*/
/************************************************************************/
int	MIOView_GetMaxy (void)
{
    return MIOWin_GetInfo (MIO_selectedRunWindow) -> height - 1;
} // MIOView_GetMaxy


/************************************************************************/
/* MIOView_MouseHide							*/
/************************************************************************/
void	MIOView_MouseHide (void)
{
    // TW = To be filled in
} // MIOView_MouseHide


/************************************************************************/
/* MIOView_MouseShow							*/
/************************************************************************/
void	MIOView_MouseShow (void)
{
    // TW = To be filled in
} // MIOView_MouseShow


/************************************************************************/
/* MIOView_Scroll							*/
/************************************************************************/
void	MIOView_Scroll (OOTint pmDX, OOTint pmDY, BOOL pmClear)
{
    // TW = To be filled in
} // MIOView_Scroll


/************************************************************************/
/* MIOView_SetMousePosition						*/
/************************************************************************/
void	MIOView_SetMousePosition (OOTint pmX, OOTint pmY)
{
    // TW = To be filled in
} // MIOView_SetMousePosition


/************************************************************************/
/* MIOView_WhatDotColour						*/
/************************************************************************/
int	MIOView_WhatDotColour (OOTint pmX, OOTint pmY)
{
    MIOWinInfoPtr	myInfo = MIOWin_GetInfo (MIO_selectedRunWindow);
    
    return MIORGB_WhatDotColor (myInfo, pmX, myInfo -> height - pmY - 1);
} // MIOView_WhatDotColour


/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/
					


