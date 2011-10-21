/**************/
/* miolexer.h */
/**************/

#ifndef _MIOLEXER_H_
#define _MIOLEXER_H_

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
typedef struct TOKEN_RECORD 
{
    OOTint4	lineNo;
    OOTint4	linePos;

    OOTint4	tokNum;
    char	token [STRLEN];

    /* Filled only if evaluate was requested */

    OOTint4	intVal;
    OOTreal8	realVal;
    char	stringVal [STRLEN];

    OOTboolean  error;
} TOKEN_RECORD;

/**********************/
/* External variables */
/**********************/

/*********************************************/
/* External procedures for Turing predefines */
/*********************************************/
extern UINT	MIOLexer_Begin (const char *pmSrcFile, const char *pmTableFile, 
			        OOTint pmFlags, SRCPOS *pmSrcPos);
extern void	MIOLexer_End (OOTint pmLexID);
extern void	MIOLexer_Scan (OOTint pmLexId, TOKEN_RECORD *pmTokRecord);

/***************************************/
/* External procedures for MIO library */
/***************************************/
extern void	MIOLexer_Init (void);
extern void	MIOLexer_Finalize_Run (void);

#endif // #ifndef _MIOLEXER_H_
