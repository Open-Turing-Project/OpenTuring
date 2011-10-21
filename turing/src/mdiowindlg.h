/****************/
/* mdiowindlg.h */
/****************/

#ifndef _MDIOWINDLG_H_
#define _MDIOWINDLG_H_

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
// Parameters for MIOWinDlg_ProgramRunning
#define RUNNING			1
#define HALTED			2
#define FINISHED		3
#define WAITING_FOR_INPUT	4
#define PAUSED			5

// Argument for MyDisplayText
#define ALL_LINES		-99

/*********/
/* Types */
/*********/


/**********************/
/* External variables */
/**********************/

/***********************/
/* External procedures */
/***********************/
// Init, Finalize subprograms
extern void	MDIOWinDlg_Init (void);
extern void	MDIOWinDlg_Finalize (void);
// Properties subprograms
//extern void	MDIOWinDlg_PropertiesSet (MIOWinDlg_Properties pmProperties);
//extern void	MDIOWinDlg_PropertiesImplementChanges (HWND pmEditWindow);

extern void	MDIOWinDlg_CaptureMouse (WIND pmWindow);
extern void	MDIOWinDlg_CopyToClipboard (WIND pmWindow, int pmLen, 
					     char *pmSrc, char *pmEnd);
extern void	MDIOWinDlg_CloseWindow (WIND pmWindow);				 
extern WIND	MDIOWinDlg_CreateInnerWindow (WIND pmWindow);
extern WIND	MDIOWinDlg_CreateWindow (WindowAttribPtr pmWindowAttribs);
extern BOOL	MDIOWinDlg_InitializeWindow (WIND pmWindow);
extern void	MDIOWinDlg_SetScrollTimer (WIND pmWindow);

#endif // #ifndef _MDIOWINDLG_H_
