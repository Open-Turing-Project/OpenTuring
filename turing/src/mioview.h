/*************/
/* mioview.h */
/*************/

#ifndef _MIOVIEW_H_
#define _MIOVIEW_H_

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
extern void	MIOView_ClipAdd (OOTint pmX1, OOTint pmY1, OOTint pmX2, 
				 OOTint pmY2);
extern void	MIOView_ClipAddOval (OOTint pmX, OOTint pmY, OOTint pmXR, 
				     OOTint pmYR);
extern void	MIOView_ClipOff (void);
extern void	MIOView_ClipSet (OOTint pmX1, OOTint pmY1, OOTint pmX2, 
				 OOTint pmY2);
extern int	MIOView_GetMaxx (void);
extern int	MIOView_GetMaxy (void);
extern void	MIOView_MouseHide (void);
extern void	MIOView_MouseShow (void);
extern void	MIOView_Scroll (OOTint pmDX, OOTint pmDY, BOOL pmClear);
extern void	MIOView_SetMousePosition (OOTint pmX, OOTint pmY);
extern int	MIOView_WhatDotColour (OOTint pmX, OOTint pmY);

#endif // #ifndef _MIOVIEW_H_
