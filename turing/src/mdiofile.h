/************************************************************************/
/* mdiofile.h								*/
/************************************************************************/

#ifndef _MDIOFILE_H_
#define _MDIOFILE_H_

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
#define FILE_ONLY   TRUE
#define FILE_OR_DIR FALSE

/*********/
/* Types */
/*********/

/**********************/
/* External variables */
/**********************/

/***********************/
/* External procedures */
/***********************/
//extern void	MDIOFile_ConvertFilePathToTuringPath (const char *pmMDPathName, 
//						      char *pmTuringPath);
extern void	MDIOFile_Copy (const char *pmSrcPath, const char *pmDestPath);
extern void	MDIOFile_Delete (const char *pmPathName);
extern int	MDIOFile_DiskFree (const char *pmPathName);
extern BOOL	MDIOFile_Exists (const char *pmPathName, BOOL pmDataFileOnly);
extern BOOL	MDIOFile_GetDefaultPathForWindowsVolume (char pmDriveLetter,
	    					         char *pmPath);
extern void	MDIOFile_Rename (const char *pmPathName, 
				 const char *pmNewPathName);
extern void	MDIOFile_Status (const char *pmPathname, int *pmFileSize,
			         int *pmAttrib, int *pmFileTime);

#endif // #ifndef _MDIOFILE_H_
