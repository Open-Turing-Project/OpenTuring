/*************/
/* mdiorgb.h */
/*************/

#ifndef _MDIORGB_H_
#define _MDIORGB_H_

/*******************/
/* System includes */
/*******************/

/******************/
/* Other includes */
/******************/
#include "miotypes.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/

/*********/
/* Types */
/*********/

/**********************/
/* External variables */
/**********************/

/***********************/
/* External procedures */
/***********************/
extern int	MDIORGB_AddColour (OOTreal pmRed, OOTreal pmGreen, 
				   OOTreal pmBlue);
extern COLOR	MDIORGB_ConvertColour (OOTint pmClr);
extern PALETTE	MDIORGB_CreateNewPalette (PALETTE pmDefaultPalette);
extern void	MDIORGB_GetColour (COLOR pmColour, OOTreal *pmRed, 
				   OOTreal *pmGreen, OOTreal *pmBlue);
extern int	MDIORGB_GetMatchingColourInPalette (MIOWinInfoPtr pmInfo, 
						    OOTreal pmRed, 
						    OOTreal pmGreen, 
						    OOTreal pmBlue);
extern int	MDIORGB_GetNearestColourInPalette (MIOWinInfoPtr pmInfo, 
						   OOTreal pmRed, 
						   OOTreal pmGreen, 
						   OOTreal pmBlue);
extern int	MDIORGB_GetScreenDepth (void);
extern BOOL	MDIORGB_SetColour (OOTint pmClrNum, OOTreal pmRed, 
				   OOTreal pmGreen, OOTreal pmBlue);
extern void	MDIORGB_UseNewPalette (MIOWinInfoPtr pmInfo);
extern int	MDIORGB_WhatDotColor (MIOWinInfoPtr pmInfo, int pmX, int pmY);

#endif // #ifndef _MDIORGB_H_
