/***********/
/* jscon.h */
/***********/

#ifndef _JSCON_H_
#define _JSCON_H_

/*******************/
/* System includes */
/*******************/

/******************/
/* Other includes */
/******************/
#include "jstop.h"
#include "edtext.h"
//#include "edglob.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
// Parameters for JSCon_ProgramRunning
#define RUNNING			1
#define HALTED			2
#define FINISHED		3
#define WAITING_FOR_INPUT	4
#define PAUSED			5

// User messages for console windows
#define WM_ADD_TEXT		(WM_USER + 0)
#define WM_DELETE_TEXT		(WM_USER + 1)
#define WM_SET_RUN_STATUS	(WM_USER + 2)
#define WM_SHOW_CURSOR		(WM_USER + 3)
#define WM_HIDE_CURSOR		(WM_USER + 4)
#define WM_PROCESS_INPUT	(WM_USER + 5)

// Used by WM_ADD_TEXT
#define NOT_AT_BEGINNING_OF_LINE	1
#define BEGINNING_OF_LINE		2
#define FREE_BUFFER			3

#define PROPERTY_LENGTH			1024

/*********/
/* Types */
/*********/
typedef struct
{
    BOOL	fullScreen;
    int		textRows;
    int		textCols;

    // Console text display properties
    int		textFontSize;
    char	textFontName [PROPERTY_LENGTH];
    int		caretWidth;

    // Debugging
    BOOL	logging;
    BOOL	logWindowMessages;
} JSConOnly_Properties;

typedef struct
{
    JSTop_Properties		topProperties;
    EdText_Properties		textProperties;
    JSConOnly_Properties	myProperties;
} JSCon_Properties;

/**********************/
/* External variables */
/**********************/

/***********************/
/* External procedures */
/***********************/
// Init, Finalize subprograms
extern void	JSCon_Init (void);
extern void	JSCon_Finalize (void);
// Properties subprograms
extern void	JSCon_PropertiesSet (JSCon_Properties pmProperties);
extern void	JSCon_PropertiesImplementChanges (HWND pmEditWindow);
// Create, Dispose subprograms
extern HWND	JSCon_Create (const char *pmClassName);

// Other subprograms
extern void	JSCon_GetLineOfInput (HWND pmConsoleWindow, char *pmBuffer, 
				      int pmBufferSize);
extern void	JSCon_SetRunState (HWND pmConsoleWindow, int pmRunning);
#endif // #ifndef _JSCON_H_
