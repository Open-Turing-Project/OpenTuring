/**************/
/* edsplash.h */
/**************/

#ifndef _EDSPLASH_H_
#define _EDSPLASH_H_

/*******************/
/* System includes */
/*******************/
#include <windows.h>

/******************/
/* Other includes */
/******************/

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
extern BOOL	EdSplash_Init (void);
extern void	EdSplash_Finalize (void);
extern void	EdSplash_PropertiesSet (void);
extern void	EdSplash_PropertiesImplementChanges (void);
extern HWND	EdSplash_Create (BOOL pmAbout);

#endif // #ifndef _EDSPLASH_H_
