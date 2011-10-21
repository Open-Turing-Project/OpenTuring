/************/
/* edjdll.h */
/************/

#ifndef _EDJDLL_H_
#define _EDJDLL_H_

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
EXPORT void __cdecl DLL_Initialize (void (*pmNotifyDebuggerDisconnect) (void),
				    void (*pmOutputFromDebugger) (int pmReason, 
					       const char *pmOutputBuffer));
EXPORT void __cdecl DLL_SetLogging (BOOL pmLogging, HANDLE pmLogFileHandle);

#endif // #ifndef _EDJDLL_H_
