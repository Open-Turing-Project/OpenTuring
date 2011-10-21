/*************/
/* mdiodlg.h */
/*************/

#ifndef _MDIODLG_H_
#define _MDIODLG_H_

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
extern void	MDIODlg_Alert (const char *pmTitle, const char *pmMsg);
//extern void	MDIODlg_Create (const char *pmDirectoryPath);
//extern void	MDIODir_Delete (const char *pmDirectoryPath);
//extern BOOL	MDIODir_Exists (char *pmDirectoryPath);
//extern void	MDIODir_FreeDir (DIR pmMDDirInfo);
//extern BOOL	MDIODir_Get (DIR pmMDDirInfo, OOTstring pmFileName,
//			     int *pmFileSize, int *pmAttrib, int *pmFileTime);
//extern BOOL	MDIODir_OpenDir (char *pmDirectoryPath, DIR *pmMDDirInfo, 
//				 BOOL *pmIsEmpty);

#endif // #ifndef _MDIODLG_H_
