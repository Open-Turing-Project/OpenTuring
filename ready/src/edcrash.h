/*************/
/* edcrash.h */
/*************/

#ifndef _EDCRASH_H_
#define _EDCRASH_H_

/*******************/
/* System includes */
/*******************/
#include <windows.h>
#include <imagehlp.h>
#include <tlhelp32.h>

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
BOOL	EdCrash_Init (void);
void	EdCrash_Finalize (void);
void	EdCrash_GetExceptionName (char *pmBuffer, DWORD pmException);
void	EdCrash_RegisterDump (char *pmBuffer, CONTEXT *pmContext);
void	EdCrash_StackDump (char *pmBuffer, int pmSize, CONTEXT *pmContext);

#endif // #ifndef _EDCRASH_H_
