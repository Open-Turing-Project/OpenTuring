/****************/
/* mdiowintop.h */
/****************/

#ifndef _MDIOWINTOP_H_
#define _MDIOWINTOP_H_

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
#define TOP_TEXT_WINDOW		TRUE
#define TOP_GRAPHICS_WINDOW	FALSE

/*********/
/* Types */
/*********/
typedef struct
{
    int 	dialogFontSize;
} MDIOWinTop_Properties;

/**********************/
/* External variables */
/**********************/

/***********************/
/* External procedures */
/***********************/
// Init, Finalize subprograms
extern void	MDIOWinTop_Init (BOOL pmStopUserClose);
extern void	MDIOWinTop_Finalize (void);
// Properties subprograms
extern void	MDIOWinTop_PropertiesSet (MDIOWinTop_Properties pmProperties);
extern void	MDIOWinTop_PropertiesImplementChanges (WIND pmButtonBarWindow);
// Create, Dispose subprograms
extern WIND	MDIOWinTop_Create (WIND pmEditWindow, BOOL pmIsTextWindow);
// Other subprograms
extern int	MDIOWinTop_GetWindowHeight (void);
extern void	MDIOWinTop_DisablePauseButton (WIND pmButtonBarWindow);
extern void	MDIOWinTop_SwitchPauseToResume (WIND pmButtonBarWindow);
extern void	MDIOWinTop_SwitchResumeToPause (WIND pmButtonBarWindow);
extern void	MDIOWinTop_SwitchStopToClose (WIND pmButtonBarWindow);

#endif // #ifndef _MIOWINTOP_H_
