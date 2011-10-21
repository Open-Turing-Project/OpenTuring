/************/
/* edprop.h */
/************/

#ifndef _EDPROP_H_
#define _EDPROP_H_

/*******************/
/* System includes */
/*******************/

/******************/
/* Other includes */
/******************/
#include "edfile.h"

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
extern BOOL		EdProp_Init (BOOL pmAdmin, BOOL pmLogging);
extern void		EdProp_AddFileToRecent (char *pmPathName);
extern void		EdProp_ChangePreferences (HWND pmWindow);
extern const char	*EdProp_GetApplicationDirectory (void);
extern const char	*EdProp_GetStartupDirectory (void);
extern void		EdProp_SetWriteAllPrefs (void);
extern BOOL		EdProp_WritePrefs (void);
extern void		EdProp_WriteRecentFiles (void);

#endif // #ifndef _EDPROP_H_
