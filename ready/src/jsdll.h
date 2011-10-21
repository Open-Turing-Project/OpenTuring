/***********/
/* jsdll.h */
/***********/

#ifndef _JSDLL_H_
#define _JSDLL_H_

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
#define EXPORT __declspec (dllexport)

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
EXPORT char * CALLBACK DLL_GetWrittenLine (void);
EXPORT void CALLBACK DLL_SetLineToRead (char *pmLine);
EXPORT void CALLBACK DLL_SetLogging (BOOL pmLogging, HANDLE pmLogFileHandle);

#endif // #ifndef _JSDLL_H_
