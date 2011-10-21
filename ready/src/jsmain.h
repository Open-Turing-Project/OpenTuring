/************/
/* jsmain.h */
/************/

#ifndef _JSMAIN_H_
#define _JSMAIN_H_

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
// Window title status messages
#define STATUS_RUNNING		1
#define	STATUS_PAUSED		2
#define STATUS_WAITING_FOR_INP	3
#define STATUS_FINISHED		4

// All Turing editor windows define 12 bytes of extra data as follows
#define	WINDOW_MEM_WINDOW_TYPE		 0
#define	WINDOW_MEM_WINDOW_INITIALIZED	 4
#define WINDOW_MEM_WINDOW_INFO		 8

#define WINDOW_EXTRA_MEMORY		12

#define WINDOW_TYPE_BEGINNER_TOP	104
#define WINDOW_TYPE_CONSOLE		108
#define WINDOW_TYPE_CONSOLE_TEXT	109
/*
// These are all the type of windows defined in the Turing editor
#define	WINDOW_TYPE_EDIT		101
#define WINDOW_TYPE_STATUS		102
#define WINDOW_TYPE_TEXT_DISPLAY	103
#define WINDOW_TYPE_SINGLE_WIN_TOP	105
#define WINDOW_TYPE_MULTI_WIN_TOP	106
#define WINDOW_TYPE_SPLASH		107
#define WINDOW_TYPE_CONSOLE		108
#define WINDOW_TYPE_CONSOLE_TEXT	109

// All string formatted properties in Turing have a maximum length
#define PROPERTY_LENGTH			1024
#define PROP_STRING_SIZE		 128
#define PROP_LONG_STRING_SIZE		1024
*/
/*********/
/* Types */
/*********/
typedef struct Globals
{
    char	environmentName [80];
    HINSTANCE	applicationInstance;
} Globals;

/*********/
/* Types */
/*********/

/**********************/
/* External variables */
/**********************/
extern Globals	gProgram;

/***********************/
/* External procedures */
/***********************/
extern void	JS_LoadString (int pmStringResource, char *pmString, 
			       int pmStringSize);
extern void	JS_ErrorMessage (int pmMessageStringResource, ...);
extern void	JS_PauseJavaProgram (void);
extern void	JS_ResumeJavaProgram (void);
extern void	JS_SendFailMessage (char *pmMessage, ...);
extern void	JS_SignalMainToTerminate (void);
extern void	JS_SubmitPrintJob (HWND pmWindow, char *pmText, int pmLen);
extern void	JS_TerminateProcess (void);
extern void	JS_WriteLog (char *pmFormatString, ...);

#endif // #ifndef _JSMAIN_H_
