/********/
/* ed.h */
/********/

#ifndef _ED_H_
#define _ED_H_

/*******************/
/* System includes */
/*******************/
#include <windows.h>

/******************/
/* Other includes */
/******************/
#include "eddde.h"
#include "edfail.h"
#include "edfile.h"
#include "edgui.h"
#include "edlog.h"
#include "edsplash.h"
#include "edwin.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
// Window types
#define NO_WINDOW		0
#define EDIT_WINDOW		1
#define SPLASH_WINDOW		2
#define CONSOLE_WINDOW		3
#define TURING_RUN_WINDOW	4
#define FIND_REPLACE_DIALOG	5
#define DEBUGGER_DIALOG		6

#define LAST_WINDOW_TYPE	7

// Return values from Ed_GetExampleDirectories
#define EXAMPLES_NOT_FOUND	(Examples *) 1
#define EXAMPLES_BAD_FORMAT	(Examples *) 2
#define EXAMPLES_EMPTY		(Examples *) 3
#define EXAMPLES_OUT_OF_MEMORY	(Examples *) 4

/*********/
/* Types */
/*********/
typedef struct
{
    EdLog_Properties	logProperties;
    EdWin_Properties	winProperties;
} Ed_Properties;

typedef struct Example
{
    char		*label;
    char		*directoryName;
    struct Example	*next;
} Example;

typedef struct Examples
{
    int		numExamples;
    Example	*head;	    
} Examples;

/**********************/
/* External variables */
/**********************/

/***********************/
/* External procedures */
/***********************/
extern void	Ed_PropertiesSet (Ed_Properties pmProperties);
extern void	Ed_PropertiesImplementChanges (void);

extern void	Ed_AddToWindowList (HWND pmWindow, int pmWindowType);
extern BOOL	Ed_CatchExceptions (void);
extern BOOL	Ed_CheckWindowPreemption (void);
extern void	Ed_ClearBreakpoints (void);
extern void	Ed_CreateMemoryLogFile (void);
extern void	Ed_CreateRecentMenu (void);
extern void	Ed_DumpMemory (void);
extern void	Ed_EnableAllWindows (BOOL pmEnable, HWND pmWindow);
extern void	Ed_EnumerateFiles (
			void (*enumProc) (char *, HWND, HWND, void *, void *),
			void *pmUserParam);
extern void	Ed_EnumerateWindows (void (*enumProc) (HWND, void *), 
				     void *pmUserParam);
extern void	Ed_ExitApplication (HWND pmWindow, BOOL pmImmediate);
extern Examples	*Ed_GetExampleDirectories (HWND pmWindow);
extern HWND	Ed_GetLastActiveEditWindow (void);
extern void	Ed_ProcessWaitingEvents (BOOL pmWaitForEvent);
extern void	Ed_PutWindowAboveAllOtherEdWindows (HWND pmWindow);
extern void	Ed_RedrawWindows (void);
extern BOOL	Ed_RemoveFromWindowList (HWND pmWindow);
extern void	Ed_SendMessageEnumerator (HWND pmWindow, void *pmMessage);
extern void	Ed_SendMessageToAllWindows (BOOL pmUseNotify, int pmMessage,
					    WPARAM pmWParam, LPARAM pmLParam);
extern void	Ed_SendStatusMessageEnumerator (HWND pmWindow, void *pmMessage);
extern void	Ed_SendStatusMessageToAllWindows (char *pmMessage);
extern void	Ed_SetActiveWindow (HWND pmWindow, int pmWindowType);
extern void	Ed_SetModalDialog (HWND pmWindow);
extern void	Ed_ToggleBreakpointVisibility (void);
extern void	Ed_ToggleDebuggerControlsVisibility (void);

#endif // #ifndef _ED_H_
