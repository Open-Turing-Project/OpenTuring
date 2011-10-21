/****************/
/* mdiosprite.h */
/****************/

#ifndef _MDIOSPRITE_H_
#define _MDIOSPRITE_H_

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
typedef struct SpriteInfo
{
    int			spriteID;
    WIND		runWindow;
    int			x, y;			// In PC coordinates
    int			picWidth, picHeight;	// From Pic
    int			picTransparentColour;	// From Pic
    PIC			picMDPicInfo;		// From Pic
    MYRECT		spriteRect;		// In PC coordinates
    int			spriteHeight;
    BOOL		visible;
    BOOL		centered;
    struct SpriteInfo	*next;
    struct SpriteInfo	*prev;
} SpriteInfo;


/**********************/
/* External variables */
/**********************/

/***********************/
/* External procedures */
/***********************/
extern void	MDIOSprite_InitializeSpriteContext (MIOWinInfoPtr pmInfoPtr);
extern void	MDIOSprite_UpdateWindow (MIOWinInfoPtr pmInfo, 
					 BOOL pmCopyToOffscreen);

#endif // #ifndef _MDIOSPRITE_H_
