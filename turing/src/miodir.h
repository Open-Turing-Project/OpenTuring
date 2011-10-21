/************/
/* miodir.h */
/************/

#ifndef _MIODIR_H_
#define _MIODIR_H_

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

/**********************/
/* External variables */
/**********************/

/*********************************************/
/* External procedures for Turing predefines */
/*********************************************/
extern BOOL	MIODir_Exists (OOTstring pmPathName);
extern OOTint 	MIODir_Open (OOTstring pmPathName, SRCPOS *pmSrcPos);
extern void 	MIODir_Close (OOTint pmDirectoryID);
extern void 	MIODir_Get (OOTint pmDirectoryID, OOTstring pmFileName);
extern void 	MIODir_GetLong (OOTint pmDirectoryID, OOTstring pmFileName, 
			        OOTint *pmFileSize, OOTint *pmAttrib, 
			        OOTint *pmFileTime);
extern void 	MIODir_Create (OOTstring pmPathName);
extern void 	MIODir_Delete (OOTstring pmPathName);
extern void 	MIODir_Change (OOTstring pmPathName);
extern void 	MIODir_Current (OOTstring pmPathName);

/***************************************/
/* External procedures for MIO library */
/***************************************/

#endif  // #ifndef _MIODIR_H_