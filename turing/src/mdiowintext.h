/*****************/
/* mdiowintext.h */
/*****************/

#ifndef _MDIOWINTEXT_H_
#define _MDIOWINTEXT_H_

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
// Parameters for MDIOWinText_ProgramRunning
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
extern void	MDIOWinText_Init (void);
extern void	MDIOWinText_Finalize (void);
// Properties subprograms
//extern void	MDIOWinText_PropertiesSet (MDIOWinText_Properties pmProperties);
//extern void	MDIOWinText_PropertiesImplementChanges (HWND pmEditWindow);

extern void	MDIOWinText_CaptureMouse (WIND pmWindow);
extern void	MDIOWinText_CopyToClipboard (WIND pmWindow, int pmLen, 
					     char *pmSrc, char *pmEnd);
extern void	MDIOWinText_CloseWindow (WIND pmWindow);				 
extern WIND	MDIOWinText_CreateInnerWindow (WIND pmWindow);
extern WIND	MDIOWinText_CreateWindow (WindowAttribPtr pmWindowAttribs);
extern void	MDIOWinText_DisplayText (WIND pmInnerOrOuterWindow, 
				         BOOL pmPaintUpdate, 
				         int pmFromLine, int pmToLine);
extern void	MDIOWinText_DisposeTextRecord (WIND pmWindow);
extern BOOL	MDIOWinText_InitializeWindow (WIND pmWindow);
extern void	MDIOWinText_SaveWindowToFile (WIND pmWindow, 
					      const char *pmPathName);
extern void	MDIOWinText_SetScrollTimer (WIND pmWindow);
extern void	MDIOWinText_SynchScrollBarsToText (WIND pmWindow);

#endif // #ifndef _MDIOWINTEXT_H_
