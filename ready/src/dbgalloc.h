/**************/
/* dbgalloc.h */
/**************/

#ifndef _DBGALLOC_H_
#define _DBGALLOC_H_

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
typedef void	*MYSRCPOS;

/**********************/
/* External variables */
/**********************/

/***********************/
/* External procedures */
/***********************/
extern BOOL	DbgAlloc_Init (void);
extern void	DbgAlloc_Allocate (int pmObjectType, int pmID, 
				   MYSRCPOS pmSrcPos, 
				   const char *pmDescription);
extern void	DbgAlloc_Finalize_Run (void);
extern void	DbgAlloc_Free (int pmID);
extern void	DbgAlloc_Init_Run (void);
extern BOOL	DbgAlloc_IsWindowVisible (void);
extern void	DbgAlloc_ToggleWindowVisibility (void);

#endif // #ifndef _DBGALLOC_H_
