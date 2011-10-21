/***********/
/* ederr.h */
/***********/

#ifndef _EDERR_H_
#define _EDERR_H_

/*******************/
/* System includes */
/*******************/
#include <windows.h>

/******************/
/* Other includes */
/******************/

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
#define ACTIVATE_SOURCE_WINDOW	TRUE

/*********/
/* Types */
/*********/
typedef struct Error
{
    char	*pathName;	// Path where the error occurred
    WORD	turingFileNo;	// If the file was unnamed when compiled, store
    				// NULL for pathName and set turingFileNo
    int		line;		// Line to be highlighted
    int		tokenPos;	// Position of token to be "super" highlighted.
    				// tokenPos set to NO_HIGHLIGHT if nothing
    				// "super" highlighted
    int		tokenLen;	// Length of token to be "super" highlighted
    char	*errorMessage;  // Error message to display
    struct Error  *next;	// Next line to be highlighted
} Error, *ErrorPtr;


/**********************/
/* External variables */
/**********************/

/***********************/
/* External procedures */
/***********************/
// Init, Finalize subprograms
extern BOOL	EdErr_Init (void);
extern void	EdErr_Finalize (void);
// Other subprograms
extern BOOL     EdErr_AddError (const char *pmPathName, WORD pmTuringFileNo,
				int pmLineFrom, int pmColFrom, 
			 	int pmLineTo, int pmColTo, 
			 	const char *pmErrorMessage);
extern void	EdErr_ClearErrors (void);
extern void	EdErr_DeleteError (ErrorPtr pmError);
extern BOOL	EdErr_DisplayNumberOfErrorsDialog (HWND pmTextDisplayWindow,
						   const char *pmCompiledPathName, 
						   int pmNumErrors, 
						   int pmNumWarnings, 
						   int pmNumFiles,
						   BOOL pmCompileOnly);
extern void	EdErr_FindFirstError (HWND pmTextDisplayWindow, 
				      BOOL pmMessageIfNoError,
				      BOOL pmActivateSourceWindow);
extern void	EdErr_FindNextError (HWND pmTextDisplayWindow,
				     BOOL pmMessageIfNoError,
				     BOOL pmActivateSourceWindow);
extern BOOL	EdErr_HasErrors (void);
extern BOOL	EdErr_HilightLine (HWND pmTextDisplayWindow, 
				   const char *pmPathName,
				   WORD pmTuringFileNo, int pmLine, 
				   BOOL pmActivateSourceWindow,
				   const char *pmMessage);
extern void	EdErr_LoadErrors (HWND pmTextDisplayWindow);

#endif // #ifndef _EDERR_H_
