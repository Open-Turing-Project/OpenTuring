/*****************/
/* miowingraph.h */
/*****************/

#ifndef _MIOWINGRAPH_H_
#define _MIOWINGRAPH_H_

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
extern void	MIOWinGraph_Init (void);
extern void	MIOWinGraph_Finalize (void);
extern void	MIOWinGraph_AddText (WIND pmWindow, const char *pmBuffer, 
				     int pmFlag);
extern WIND	MIOWinGraph_CreateWindow (WindowAttribPtr pmWindowAttribs);
extern void	MIOWinGraph_DeleteText (WIND pmWindow, int pmRow, int pmCol);
extern void	MIOWinGraph_PrintWindow (WIND pmWindow);
extern void	MIOWinGraph_SaveWindowToFile (WIND pmWindow, 
					      const char *pmPathName);

#endif // #ifndef _MIOWINGRAPH_H_
