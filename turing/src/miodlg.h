/************/
/* miodlg.h */
/************/

#ifndef _MIODLG_H_
#define _MIODLG_H_

/*******************/
/* System includes */
/*******************/

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

/***********************/
/* External procedures */
/***********************/

/***********************/
/* External procedures */
/***********************/
extern void	MIODlg_Alert (const char *pmTitle, const char *pmMsg);
//extern void	MIODlg_Create (const char *pmDirectoryPath);
//extern void	MIODir_Delete (const char *pmDirectoryPath);
//extern BOOL	MIODir_Exists (char *pmDirectoryPath);
//extern void	MIODir_FreeDir (DIR pmMDDirInfo);
//extern BOOL	MIODir_Get (DIR pmMDDirInfo, OOTstring pmFileName,
//			     int *pmFileSize, int *pmAttrib, int *pmFileTime);
//extern BOOL	MIODir_OpenDir (char *pmDirectoryPath, DIR *pmMDDirInfo, 
//				 BOOL *pmIsEmpty);

#endif // #ifndef _MIODLG_H_
