/***********/
/* edlog.h */
/***********/

#ifndef _EDLOG_H_
#define _EDLOG_H_

/*******************/
/* System includes */
/*******************/

/******************/
/* Other includes */
/******************/

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/

/*********/
/* Types */
/*********/
typedef struct
{
    int	dummy;
} EdLog_Properties;


/**********************/
/* External variables */
/**********************/

/***********************/
/* External procedures */
/***********************/
// Init, Finalize subprograms
extern BOOL	EdLog_Init (void);
extern void	EdLog_Finalize (void);
// Properties subprograms
extern void	EdLog_PropertiesSet (EdLog_Properties pmProperties);
extern void	EdLog_PropertiesImplementChanges (void);
// Other subprograms
extern HANDLE	EdLog_GetLogFileHandle (void);
extern void	EdLog_Log (char *pmFormatString, ...);
extern void	EdLog_LogFont (const char *label, HDC hdc);
extern void	EdLog_LogFontEnumerate (HDC hdc);
extern void	EdLog_SetWriteToLog (void);

// EdLog_Fail - This is when an internal problem has occurred in Turing,
// but you don't want to halt execution immediately
extern void	EdLog_Fail (int pmReason, char *pmFile, 
			    int pmLine, int pmErrorCode, ...);
// EdLog_FatalFail - This is when an fatal internal problem has occurred in
// Turing, and you need to quit immediately.  It should save all open
// dirty files
extern void	EdLog_FatalFail (int pmReason, char *pmFile, 
			         int pmLine, int pmErrorCode, ...);
// EdLog_PrintWindowMessage - List the window messages sent to a window			         
extern void	EdLog_PrintWindowsMessage (const char *pmTitle, HWND pmWindow, 
					   int pmMessage, WPARAM pmWParam,
					   LPARAM pmLParam);
// EdLog_WritePanicLog - Write an entry into the panic log			         
extern void	EdLog_WritePanicLog  (char *pmFormatString, ...);

#endif // #ifndef _EDLOG_H_
