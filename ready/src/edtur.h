/***********/
/* edtur.h */
/***********/

#ifndef _EDTUR_H_
#define _EDTUR_H_

/*******************/
/* System includes */
/*******************/
#include <windows.h>

/******************/
/* Other includes */
/******************/
#include "edtext.h"

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
// Init, Finalize subprograms
extern BOOL		EdTur_Init (void);
extern void		EdTur_Finalize (void);
// Properties subprograms
extern void		EdTur_PropertiesSet (void);
extern void		EdTur_PropertiesImplementChanges (void);
// Other subprograms
extern void		EdTur_GetProgramName (char *pmText, int pmTextLen,
			    		      char *pmProgramName);
extern void		EdTur_Indent (HWND pmWindowForStatus, char *pmText,
				      int pmTextLen, char **pmNewBuffer, 
				      int *pmNewBufferLen, int *pmNewTextLen);
extern void		EdTur_KeywordDisplayPage (HWND pmEditWindow, 
						  TextPtr pmTextPtr);
extern void		EdTur_KeywordLookup (HWND pmEditWindow, 
					     TextPtr pmTextPtr);
extern MarkLinePtr	EdTur_Mark (char *pmText, int pmTextLen);

#endif // #ifndef _EDTUR_H_
