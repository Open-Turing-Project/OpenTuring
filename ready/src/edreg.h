/***********/
/* edreg.h */
/***********/

#ifndef _EDREG_H_
#define _EDREG_H_

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
#define ASSOC_WITH_READY		(char *) 1

/*********/
/* Types */
/*********/

/**********************/
/* External variables */
/**********************/

/***********************/
/* External procedures */
/***********************/
extern BOOL	EdReg_ClearTree (HWND pmWindow, const char *pmKeyName);
extern char	*EdReg_GetCurrentFileAssoc (HWND pmWindow, BOOL pmDebug);
extern BOOL	EdReg_GetKey (HWND pmWindow, const char *pmKey, 
			      const char *pmValueName, char *pmValue);
extern char	*EdReg_GetPreviousFileAssoc (HWND pmWindow, BOOL pmDebug);
extern BOOL	EdReg_KeyExists (HWND pmWindow, char *pmKey);
extern void	EdReg_ResetFileAssoc (HWND pmWindow);
extern void	EdReg_SetFileAssoc (HWND pmWindow);

#endif // #ifndef _EDREG_H_
