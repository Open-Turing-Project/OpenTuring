/*************/
/* mdiodir.h */
/*************/

#ifndef _MDIODIR_H_
#define _MDIODIR_H_

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
extern void	MDIODir_Create (const char *pmDirectoryPath);
extern void	MDIODir_Delete (const char *pmDirectoryPath);
extern BOOL	MDIODir_Exists (char *pmDirectoryPath);
extern void	MDIODir_FreeDir (DIR pmMDDirInfo);
extern BOOL	MDIODir_Get (DIR pmMDDirInfo, OOTstring pmFileName,
			     int *pmFileSize, int *pmAttrib, int *pmFileTime);
extern BOOL	MDIODir_OpenDir (char *pmDirectoryPath, DIR *pmMDDirInfo, 
				 BOOL *pmIsEmpty);

#endif // #ifndef _MDIODIR_H_
