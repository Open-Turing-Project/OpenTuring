/***********/
/* edwin.h */
/***********/

#ifndef _EDWIN_H_
#define _EDWIN_H_

/*******************/
/* System includes */
/*******************/

/******************/
/* Other includes */
/******************/
#include "edbbar.h"
#include "eddisp.h"
#include "edmenu.h"
#include "edrec.h"
#include "edstat.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
// User messages for console windows
#define WM_SET_BUTTONS			(WM_USER + 0)
#define WM_SET_STATUS			(WM_USER + 1)
#define WM_PRINTOUTPUTJOB		(WM_USER + 2)
#define WM_CHANGEDEBUGGERCONTROL	(WM_USER + 3)
#define WM_SETTRACESPEED		(WM_USER + 4)
#define WM_SETTRACECHECKBOX		(WM_USER + 5)
#define WM_SETSHOWVARSCHECKBOX		(WM_USER + 6)
#define WM_SETTRACESPEEDSLIDER		(WM_USER + 7)
#define WM_DEBUGGER_OUTPUT		(WM_USER + 8)

// Types of windows
#define BEGINNER_WINDOW			 1
#define SINGLE_WINDOW			 2
#define MULTI_WINDOW			 3

// Flags for passing commands
#define SENT_FROM_EDWIN			 1

// Constants for EdWin_Open
#define PURPOSE_LOAD_FILE		 1
#define PURPOSE_DISPLAY_ERROR		 2

// Flags for WM_SET_BUTTONS message
#define SET_BUTTON_NOT_RUNNING		41
#define SET_BUTTON_IS_EXECUTING		42
#define SET_BUTTON_PAUSED		43
#define SET_BUTTON_WAITING		44

/*********/
/* Types */
/*********/
typedef struct
{
    BOOL	beginnerMode;
    BOOL	oneWindowMode;
    BOOL	beginnerFullScreen;
    BOOL	advancedFullScreen;
    int		textRows;
    int		textCols;
    int		tabFontSize;
    int		tabMargin;
    
    // Debugging
    BOOL	logging;
    BOOL	logWindowMessages;
} EdWinOnly_Properties;

typedef struct
{
    EdDisp_Properties		dispProperties;
    EdMenu_Properties		menuProperties;
    EdRec_Properties		recProperties;
    EdBBar_Properties		topbProperties;
    EdStat_Properties		statProperties;

    EdWinOnly_Properties	myProperties;
} EdWin_Properties;

/**********************/
/* External variables */
/**********************/

/***********************/
/* External procedures */
/***********************/
// Init, Finalize subprograms
extern BOOL	EdWin_Init (void);
extern void	EdWin_Finalize (void);
// Properties subprograms
extern void	EdWin_PropertiesSet (EdWin_Properties pmProperties);
extern void	EdWin_PropertiesImplementChanges (HWND pmEditWindow);
// Create, Dispose subprograms
extern HWND	EdWin_Create (const char *pmPathName, const char *pmExampleName,
			      int pmNewSkeleton, BOOL pmBeginnerMode, 
			      BOOL pmOneWindowMode);
// Other subprograms
extern void	EdWin_ActivateTextDisplay (HWND pmEditWindow,
					   HWND pmTextDisplayWindow);
extern void	EdWin_AddFile (HWND pmEditWindow, const char *pmPathName,
			       const char *pmExampleName, int pmNewSkeleton);
extern void	EdWin_CreateRecentMenu (HWND pmEditWindow);
extern void	EdWin_EnumerateFiles (HWND pmEditWindow, 
			void (*enumProc) (char *, HWND, HWND, void *, void *),
			void *pmUserParam);
extern HWND	EdWin_GetActiveDisplayWindow (HWND pmEditWindow);			
extern void	EdWin_MarkMenuCreate (HWND pmEditWindow);
extern BOOL	EdWin_MatchesPathName (HWND pmEditWindow, 
				       const char *pmPathName);
extern void	EdWin_OpenExample (HWND pmEditWindow, int pmLineNum, 
				   const char *pmLParam);
extern BOOL	EdWin_OpenFile (HWND pmEditWindow, const char *pmPathName,
				const char *pmExampleName, int pmPurpose);
extern void	EdWin_ShowCursorPosition (HWND pmWindow); 
extern void	EdWin_ShowStatus (HWND pmWindow, const char *pmMessage, ...);
extern void	EdWin_ShowStatus1 (HWND pmWindow, int pmMessageRsrc, ...);
extern void	EdWin_ShowStatusBold (HWND pmWindow, 
				      const char *pmMessage, ...);
extern void	EdWin_ShowStatusBold1 (HWND pmWindow, 
				       int pmMessageRsrc, ...);
extern void	EdWin_SetWindowTitle (HWND pmWindow);

#endif // #ifndef _EDWIN_H_
