/**********/
/* mdio.h */
/**********/

#ifndef _MDIO_H_
#define _MDIO_H_

/*******************/
/* System includes */
/*******************/
#include <stdarg.h>

/******************/
/* Other includes */
/******************/
#include "miotypes.h"

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
extern void	MDIO_Init (void);
extern void	MDIO_Beep (void);
extern BOOL	MDIO_DiskFile_Close (void *pmFilePtr);
extern int	MDIO_DiskFile_Flush (void *pmFilePtr);
extern int	MDIO_DiskFile_Getc (void *pmFilePtr);
extern void	*MDIO_DiskFile_Open (const char *pmPathName, char *pmMode);
extern int	MDIO_DiskFile_Putc (OOTint pmChar, void *pmFilePtr);
extern int	MDIO_DiskFile_Puts (char *pmString, void *pmFilePtr);
extern int	MDIO_DiskFile_Read (BYTE *pmBuffer, int pmSize, 
				    void *pmFilePtr);
extern char	*MDIO_DiskFile_ReadLine (char *pmBuffer, int pmSize, 
					 void *pmFilePtr);
extern void	MDIO_DiskFile_Seek (int pmLocation, void *pmFilePtr,
				    int pmWhence);
extern int	MDIO_DiskFile_Tell (void *pmFilePtr);
extern void	MDIO_DiskFile_Ungetc (OOTint pmChar, void *pmFilePtr);
extern int	MDIO_DiskFile_Write (void *pmBuffer, int pmSize, 
				     void *pmFilePtr);
extern void	MDIO_GetSystemInfo (int *pmResourceCount, DWORD *pmMemUsed);
extern void	MDIO_MemoryLeakTest (void);
extern void	MDIO_OutputSystemInfo (const char *pmMessage);
extern void	MDIO_ProcessMSWindowsError (DWORD pmMSWindowsErrorCode);
extern void	MDIO_PutMIOWindowsOnTop (void);
extern void	MDIO_sprintf (char *pmOutputStr, const char *pmFormat, ...);
extern void	MDIO_vsprintf (char *pmOutputStr, const char *pmFormat, 
			       va_list pmArgList);

#endif // #ifndef _MDIO_H_
