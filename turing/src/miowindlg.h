/***************/
/* miowindlg.h */
/***************/

#ifndef _MIOWINDLG_H_
#define _MIOWINDLG_H_

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
extern void	MIOWinDlg_Init (void);
extern void	MIOWinDlg_Finalize (void);
extern void	MIOWinDlg_PropertiesSet (void);
extern void	MIOWinDlg_PropertiesImplementChanges (void);
extern BOOL	MIOWinDlg_ButtonPressHandler (WIND pmWindow, int pmX, int pmY, 
					       BOOL pmShift);
extern WIND	MIOWinDlg_CreateWindow (WindowAttribPtr pmWindowAttribs);
extern BOOL	MIOWinDlg_MouseDownHandler (WIND pmWindow, int pmX, int pmY, 
					     BOOL pmShift, 
					     BOOL pmAllowOutOfBounds);

#endif // #ifndef _MIOWINDLG_H_
