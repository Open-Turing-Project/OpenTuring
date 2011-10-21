/*************/
/* miotext.h */
/*************/

#ifndef _MIOTEXT_H_
#define _MIOTEXT_H_

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
extern void	MIOText_Cls (void);
extern int	MIOText_GetCurrentCol (void);
extern int	MIOText_GetCurrentColour (void);
extern int	MIOText_GetCurrentColourBack (void);
extern int	MIOText_GetCurrentRow (void);
extern int	MIOText_GetMaxCol (void);
extern int	MIOText_GetMaxRow (void);
extern void	MIOText_Locate (OOTint pmRow, OOTint pmCol);
extern void	MIOText_LocateXY (OOTint pmX, OOTint pmY);
extern void	MIOText_SetBackgroundColour (OOTint pmClr);
extern void	MIOText_SetColour (OOTint pmClr);
extern void	MIOText_WhatChar (void);

#endif // #ifndef _MIOTEXT_H_
