/************/
/* miosys.h */
/************/

#ifndef _MIOSYS_H_
#define _MIOSYS_H_

/*******************/
/* System includes */
/*******************/

/******************/
/* Other includes */
/******************/
#include "miotypes.h"

#include "mio.h"
#include "mioerr.h"

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
extern void	MIOSys_Init (const char *pmSysExitString);
extern void	MIOSys_Init_Run (BOOL pmAllowSysExec);

extern int	MIOSys_Exec (const char *pmCommandStr);
extern void	MIOSys_Exit (void);
extern void	MIOSys_GetComputerName (char *pmComputerName);
extern void	MIOSys_GetEnv (const char *pmSymbolStr, char *pmResultStr);
extern int	MIOSys_GetPID (void);
extern void	MIOSys_GetUserName (char *pmUserName);
extern void	MIOSys_SetSysExitString (char *pmSysExitString);

#endif // #ifndef _MIOSYS_H_
