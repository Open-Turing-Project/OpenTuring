/************************************************************************/
/* miofile.h								*/
/************************************************************************/

#ifndef _MIOFILE_H_
#define _MIOFILE_H_

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
// MIOFile_ConvertPath
#define ALLOW_TRAILING_SLASH	TRUE
#define NO_TRAILING_SLASH	FALSE

// OOT attribute bits
#define OOT_ATTR_DIR		0x1
#define OOT_ATTR_READ		0x2
#define OOT_ATTR_WRITE		0x4
#define OOT_ATTR_EXECUTE	0x8
#define OOT_ATTR_HIDDEN		0x10
#define OOT_ATTR_SYSTEM		0x20
#define OOT_ATTR_VOLUME		0x40
#define OOT_ATTR_ARCHIVE	0x80
#define OOT_ATTR_OWNER_READ	0x100
#define OOT_ATTR_OWNER_WRITE	0x200
#define OOT_ATTR_OWNER_EXECUTE	0x400
#define OOT_ATTR_GROUP_READ	0x800
#define OOT_ATTR_GROUP_WRITE	0x1000
#define OOT_ATTR_GROUP_EXECUTE	0x2000
#define OOT_ATTR_OTHER_READ	0x4000
#define OOT_ATTR_OTHER_WRITE	0x8000
#define OOT_ATTR_OTHER_EXECUTE	0x10000

// Path types
#define GENERIC_TYPE_PATH	 1
#define WINDOWS_TYPE_PATH	 2
#define MACOS_TYPE_PATH		 4
#define ABSOLUTE_PATH		 8
#define ABSOLUTE_NO_VOL_PATH	16
#define ABSOLUTE_NO_DIR_PATH	32
#define RELATIVE_PATH		64

/*********/
/* Types */
/*********/

/**********************/
/* External variables */
/**********************/

/***********************/
/* External procedures */
/***********************/
extern void	MIOFile_Copy (const OOTstring pmDestFile, 
			      const OOTstring pmSrcFile);
extern void	MIOFile_CreatePath (OOTstring pmPathName, 
				    const OOTstring pmPath,
				    const OOTstring pmName);
extern void	MIOFile_Delete (OOTstring pmPathName);
extern OOTint	MIOFile_DiskFree (OOTstring pmPathName);
extern void	MIOFile_FullPath (OOTstring pmNewPath, 
				  const OOTstring pmOldPath);
extern BOOL	MIOFile_Exists (char *pmPathName);
extern void	MIOFile_Parent (OOTstring pmNewPath, const OOTstring pmOldPath);
extern void	MIOFile_Status (OOTstring pmPathname, OOTint *pmFileSize,
			        OOTint *pmAttrib, OOTint *pmFileTime);
extern void	MIOFile_Rename (OOTstring pmPathname, OOTstring pmNewFileName);
extern void	MIOFile_SplitPath (OOTstring pmPath, OOTstring pmFileName,
				   const OOTstring pathName);

/***************************************/
/* External procedures for MIO library */
/***************************************/
extern void	MIOFile_Init (int pmOS, const char *pmOOTDir, 
			      const char *pmHomeDir);
extern BOOL	MIOFile_ConvertPath (const char *pmMDUserPath, 
				     char *pmAbsolutePath, 
				     char *pmMDAbsolutePath,
				     BOOL pmAllowTrailingSlash);
extern void	MIOFile_GetExecutionDirectory (char *pmAbsolutePath);
extern BOOL	MIOFile_GetRootDir (const char *pmPath, 
				    const char **pmRootChar);				     
extern void	MIOFile_SetExecutionDirectory (const char *pmAbsolutePath);

#endif // #ifndef _MIOFILE_H_
