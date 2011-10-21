/****************/
/* miowintext.h */
/****************/

#ifndef _MIOWINTEXT_H_
#define _MIOWINTEXT_H_

/*******************/
/* System includes */
/*******************/

/******************/
/* Other includes */
/******************/
#include "miotypes.h"
#include "miowin.h"

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
extern void	MIOWinText_Init (void);
extern void	MIOWinText_Finalize (void);
extern void	MIOWinText_PropertiesSet (void);
extern void	MIOWinText_PropertiesImplementChanges (void);
extern void	MIOWinText_AddText (WIND pmWindow, const char *pmBuffer, 
				    int pmFlag);
extern BOOL	MIOWinText_ButtonPressHandler (WIND pmWindow, int pmX, int pmY, 
					       BOOL pmShift);
extern void	MIOWinText_Copy (WIND pmWindow);
extern WIND	MIOWinText_CreateWindow (WindowAttribPtr pmWindowAttribs);
extern void	MIOWinText_DeleteText (WIND pmWindow, int pmLocation); 
extern void	MIOWinText_GetCurrentLineCol (WIND pmWindow, int *pmLine, 
					      int *pmCol);
extern int	MIOWinText_GetCurrentTextPos (WIND pmWindow);
extern BOOL	MIOWinText_IsAtBeginningOfLine (WIND pmWindow);
extern BOOL	MIOWinText_MouseDownHandler (WIND pmWindow, int pmX, int pmY, 
					     BOOL pmShift, 
					     BOOL pmAllowOutOfBounds);
extern void	MIOWinText_PrintWindow (WIND pmWindow);
extern void	MIOWinText_SaveWindowToFile (WIND pmWindow, 
					     const char *pmPathName);
extern void	MIOWinText_SelectAll (WIND pmWindow);

#endif // #ifndef _MIOWINTEXT_H_
