/*************/
/* mdiosys.h */
/*************/

#ifndef _MDIOSYS_H_
#define _MDIOSYS_H_

/*******************/
/* System includes */
/*******************/

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
extern void	MDIOSys_GetComputerName (char *pmComputerName);
extern int	MDIOSys_Exec (const char *pmCommandStr, 
			      const char *pmParameterStr,
			      const char *pmWorkingDirectoryStr);
extern void	MDIOSys_GetEnv (const char *pmSymbolStr, char *pmResultStr);
extern int	MDIOSys_GetPID (void);
extern void	MDIOSys_GetUserName (char *pmUserName);

#endif // #ifndef _MDIOSYS_H_
