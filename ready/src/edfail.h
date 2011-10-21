/************/
/* edfail.h */
/************/

#ifndef _EDFAIL_H_
#define _EDFAIL_H_

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
#define ERROR_BUFFER_SIZE	2048
#define NORMAL			0
#define MOUSE_MESSAGE		1
#define LETTER_MESSAGE		2
#define EXTRA_MESSAGE		3

/*********/
/* Types */
/*********/
typedef char	ErrorBuffer [ERROR_BUFFER_SIZE];

/**********************/
/* External variables */
/**********************/

/***********************/
/* External procedures */
/***********************/
// Init, Finalize subprograms
extern BOOL	EdFail_Init (void);
extern void	EdFail_Finalize (void);
// Other subprograms
extern void	EdFail_AddMessage (int pmMsgType, const char *pmMessage, ...);
extern long	EdFail_CrashHandler (
				struct _EXCEPTION_POINTERS *pmExceptionInfo);
extern void	EdFail_Fatal (int pmReason, char *pmFile, int pmLine, 
			      int pmErrorCode, ...);
extern void	EdFail_Fatal1 (int pmReason, char *pmFile, int pmLine, 
			       int pmErrorCode, va_list pmArgs);
extern char	*EdFail_GetErrorMessage (ErrorBuffer pmBuffer);
extern char	*EdFail_GetErrorMessage1 (ErrorBuffer pmBuffer, 
					  int pmErrorCode);
extern void	EdFail_Warn (int pmReason, char *pmFile, int pmLine, 
			     int pmErrorCode, ...);
extern void	EdFail_WriteToLog (char *pmString1, char *pmString2, ...);			     

#endif // #ifndef _EDFAIL_H_
