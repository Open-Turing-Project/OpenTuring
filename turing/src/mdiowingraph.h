/******************/
/* mdiowingraph.h */
/******************/

#ifndef _MDIOWINGRAPH_H_
#define _MDIOWINGRAPH_H_

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
#define PROPERTY_LENGTH			1024

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
extern BOOL	MDIOWinGraph_Init (void);
extern void	MDIOWinGraph_Finalize (void);

// Other subprograms
extern void	MDIOWinGraph_ClearText (WIND pmWindow, int pmRow, int pmFromCol,
					int pmToCol);
extern void	MDIOWinGraph_ClearToEOL (WIND pmWindow, int pmRow, int pmCol);
extern void	MDIOWinGraph_ClipAdd (WIND pmWindow, OOTint pmX1, OOTint pmY1,
				      OOTint pmX2, OOTint pmY2);
extern void	MDIOWinGraph_ClipAddOval (WIND pmWindow, OOTint pmX, OOTint pmY,
				          OOTint pmXR, OOTint pmYR);
extern void	MDIOWinGraph_ClipClear (WIND pmWindow);
extern void	MDIOWinGraph_ClipSet (WIND pmWindow, OOTint pmX1, OOTint pmY1,
				      OOTint pmX2, OOTint pmY2);
extern WIND	MDIOWinGraph_CreateInnerWindow (WIND pmWindow);
extern WIND	MDIOWinGraph_CreateWindow (WindowAttribPtr pmWindowAttribs);
extern PALETTE	MDIOWinGraph_GetDefaultPalette (void);
extern void	MDIOWinGraph_GetMousePos (WIND pmWindow, OOTint *pmX, 
					  OOTint *pmY, OOTint *pmButtonUpDown);
extern BOOL	MDIOWinGraph_InitializeWindow (WIND pmWindow);
extern void	MDIOWinGraph_PutChar (WIND pmWindow, char pmChar,
				      int pmRow, int pmCol);
extern void	MDIOWinGraph_PutString (WIND pmWindow, const char *pmString,
					int pmLen, int pmRow, int pmCol);
extern void	MDIOWinGraph_SaveWindowToFile (WIND pmWindow,
					       const char *pmPathName);
extern void	MDIOWinGraph_ScrollUpALine (WIND pmWindow);
extern void	MDIOWinGraph_SynchScrollBarsToGraphics (WIND pmWindow);
//extern void	MIOGraphics_AddText (HWND pmConsoleWindow, char *pmText, 
//				 int pmFlag);
//extern void	MIOGraphics_CloseWindow (HWND pmConsoleWindow);				 
//extern int	MIOGraphics_GetCharacter (HWND pmConsoleWindow, BOOL *pmEchoEOF);				 
//extern void	MIOGraphics_GetLineOfInput (HWND pmConsoleWindow, char *pmBuffer, 
//				        int pmBufferSize);
//extern BOOL	MIOGraphics_IsInputAvailable (HWND pmConsoleWindow, int pmKind,
//					  int pmCount);        
//extern void	MIOGraphics_UngetCharacter (OOTint pmChar, HWND pmConsoleWindow);

#endif // #ifndef _MDIOWINGRAPH_H_
