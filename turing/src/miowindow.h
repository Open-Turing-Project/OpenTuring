/***************/
/* miowindow.h */
/***************/

#ifndef _MIOWINDOW_H_
#define _MIOWINDOW_H_

/*******************/
/* System includes */
/*******************/

/******************/
/* Other includes */
/******************/
#include "miowin.h"

#include "language.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
#define CURRENTLY_SELECTED_WINDOW	-99

/*********/
/* Types */
/*********/

/**********************/
/* External variables */
/**********************/

/***********************/
/* External procedures */
/***********************/
extern OOTint	MIOWindow_GetActive (void);
extern void	MIOWindow_GetKeyDown (BOOL *pmArray);
extern void	MIOWindow_GetPosition (OOTint pmWindowID, OOTint *pmX, 
				       OOTint *pmY);
extern OOTint	MIOWindow_GetSelect (void);
extern void	MIOWindow_Hide (OOTint pmWindowID);
extern BOOL	MIOWindow_IsVisible (OOTint pmWindowID);
extern void	MIOWindow_RegisterClose (OOTint pmWindowID);
extern void	MIOWindow_RegisterOpen (OOTint pmWindowID, SRCPOS *pmSrcPos);
extern void	MIOWindow_RegisterWindow (OOTint pmWindowID, 
					  MIOWinInfoPtr pmInfo);
extern void	MIOWindow_SetActive (OOTint pmWindowID);
extern void	MIOWindow_SetDescription (const char *pmDescription);
extern void	MIOWindow_SetPosition (OOTint pmWindowID, OOTint pmX, 
				       OOTint pmY);
extern void	MIOWindow_SetScreen (OOTint pmWindowID, OOTstring pmSetString);
extern void	MIOWindow_SetTransparentColour (OOTint pmWindowID, 
						OOTint pmColourNum);
extern void	MIOWindow_Show (OOTint pmWindowID);
extern void	MIOWindow_Update(OOTint pmWindowID);
extern void	MIOWindow_UpdateArea (OOTint pmWindowID, OOTint x1, OOTint y1, 
		           	      OOTint x2, OOTint y2);

#endif // #ifndef _MIOWINDOW_H_
