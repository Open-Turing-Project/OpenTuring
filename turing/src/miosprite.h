/***************/
/* miosprite.h */
/***************/

#ifndef _MIOSPRITE_H_
#define _MIOSPRITE_H_

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
extern void	MIOSprite_Init_Run (void);

extern void	MIOSprite_Animate (OOTint pmSpriteID, OOTint pmPicID, 
				   OOTint pmX, OOTint pmY,
				   OOTint pmCentered);
extern void	MIOSprite_ChangePic (OOTint pmSpriteID, OOTint pmPicID);
extern void	MIOSprite_Free (OOTint pmSpriteID);
extern void	MIOSprite_Hide (OOTint pmSpriteID);
extern OOTint	MIOSprite_New (OOTint pmPicID, SRCPOS *pmSrcPos);
extern void	MIOSprite_RemoveSpritesFromWindow (MIOWinInfoPtr pmInfo);
extern void	MIOSprite_SetFrameRate (int pmFrameRate);
extern void	MIOSprite_SetHeight (OOTint pmPicID, OOTint pmHeight);
extern void	MIOSprite_SetPosition (OOTint pmPicID, OOTint pmX, OOTint pmY,
				       OOTint pmCentered);
extern void	MIOSprite_Show (OOTint pmSpriteID);
extern void	MIOSprite_UpdateIfNecessary (BOOL pmCopyToOffscreen,
					     BOOL pmUpdateNow);

/***************************************/
/* External procedures for MIO library */
/***************************************/
extern void	MIOSprite_Dirty (MIOWinInfoPtr pmInfoPtr, 
				 int pmX1, int pmY1, int pmX2, int pmY2);

#endif // #ifndef _MIOSPRITE_H_
