/************************************************************************/
/* mioerror.h								*/
/************************************************************************/

#ifndef _MIOERROR_H_
#define _MIOERROR_H_

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

/***********************/
/* External procedures */
/***********************/
extern void	MIOError_Init (void);
extern void	MIOError_Abort (OOTint pmErrNo, OOTstring pmMessage);
extern void	MIOError_Halt (OOTstring pmMessage);
extern OOTint	MIOError_Last (void);
extern void	MIOError_LastMsg (OOTstring pmMessage);
extern void 	MIOError_LastStr (OOTstring pmString);
extern void	MIOError_Msg (OOTstring pmMessage, OOTint pmErrNo);
extern void	MIOError_Set (OOTint pmErrNo, OOTstring pmMessage);
extern void	MIOError_Str (OOTstring pmMessage, OOTint pmErrNo);

/***************************************/
/* External procedures for MIO library */
/***************************************/
extern void	MIOError_AbortWithErrNo (int pmErrNo);
extern void	MIOError_AbortWithErrMsg (int pmErrNo, char *pmErrMsg, ...);
extern void	MIOError_SetErrNo (int pmErrNo);
extern void	MIOError_SetErrMsg (int pmErrNo, char *pmErrMsg, ...);

#endif // #ifndef _MIOERROR_H_
