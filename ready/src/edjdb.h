/***********/
/* edjdb.h */
/***********/

#ifndef _EDJDB_H_
#define _EDJDB_H_

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
extern BOOL	EdJdb_Init (char *pmCodeBase);
extern void	EdJdb_DetachFromVM (void);
extern void	EdJdb_DebuggerOutput_T (int pmReason, const char *pmOutput);
extern void	EdJdb_Pause (void);
extern void	EdJdb_Resume (void);
extern void	EdJdb_StartTracing (void);
extern void	EdJdb_StopTracing (void);

#endif // #ifndef _EDJDB_H_
