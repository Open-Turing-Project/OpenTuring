/************/
/* miorgb.h */
/************/

#ifndef _MIORGB_H_
#define _MIORGB_H_

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

/*********************************************/
/* External procedures for Turing predefines */
/*********************************************/
extern int	MIORGB_AddColour (OOTreal pmRed, OOTreal pmGreen, 
				  OOTreal pmBlue);
extern void	MIORGB_GetColour (OOTint pmColourNum, OOTreal *pmRed, 
				  OOTreal *pmGreen, OOTreal *pmBlue);
extern int	MIORGB_GetMaxColour (void);
extern void	MIORGB_SetColour (OOTint pmColourNum, OOTreal pmRed, 
				  OOTreal pmGreen, OOTreal pmBlue);

/***************************************/
/* External procedures for MIO library */
/***************************************/
extern COLOR	MIORGB_ConvertColour (OOTint pmClr);
extern int	MIORGB_GetScreenDepth (void);
extern int	MIORGB_WhatDotColor (MIOWinInfoPtr pmInfo, int pmX, int pmY);

#endif // #ifndef _MIORGB_H_
