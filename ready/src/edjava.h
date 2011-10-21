/************/
/* edjava.h */
/************/

#ifndef _EDJAVA_H_
#define _EDJAVA_H_

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
extern BOOL	EdJava_Init (void);
extern void	EdJava_Finalize (void);
// Properties subprograms
extern void	EdJava_PropertiesSet (void);
extern void	EdJava_PropertiesImplementChanges (void);
// Other subprograms
extern void	EdJava_GetPackageAndClass (char *pmText, int pmTextLen,
		    			   char *pmPackage, char *pmClass);
extern void	EdJava_GetProgramType (char *pmText, int pmTextLen,
			    	       BOOL *pmIsApplication, BOOL *pmIsApplet);
extern void	EdJava_Indent (HWND pmWindowForStatus, char *pmText,
			       int pmTextLen, char **pmNewBuffer, 
			       int *pmNewBufferLen, int *pmNewTextLen);
extern BOOL	EdJava_IsIdentFollow (char ch);
extern BOOL	EdJava_IsIdentStart (char ch);
extern BOOL	EdJava_IsInJavaLang (const char *pmClassName);
extern BOOL	EdJava_IsInOtherJavaPackage (const char *pmClassName, 
					     char *pmPackage);
extern BOOL	EdJava_IsUpperCase (char ch);
extern MarkLinePtr	EdJava_Mark (char *pmText, int pmTextLen);

#endif // #ifndef _EDJAVA_H_
