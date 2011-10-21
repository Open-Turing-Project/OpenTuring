/*************/
/* dbgmain.h */
/*************/

#ifndef _DBGMAIN_H_
#define _DBGMAIN_H_

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
extern void	DbgMain_LocateWindow (HWND pmWindow, int pmWindowWidth, 
				      int pmWindowHeight);
#endif // #ifndef _DBGMAIN_H_
