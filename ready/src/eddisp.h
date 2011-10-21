/************/
/* eddisp.h */
/************/

#ifndef _EDDISP_H_
#define _EDDISP_H_

/*******************/
/* System includes */
/*******************/
#include <windows.h>

/******************/
/* Other includes */
/******************/
#include "edfile.h"
#include "edglob.h"
#include "edprint.h"
#include "edprog.h"
#include "edsearch.h"
#include "edtext.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
// Flags for passing commands
#define SENT_FROM_EDDISP	2

// Paramters for EdDisp_GetPathForDisplay
#define USE_DEFAULT		TRUE
#define USE_FILENAME		FALSE
#define DONT_SHOW_DIRTY_MARK	TRUE
#define SHOW_DIRTY_MARK		FALSE

// Value returned by EdDisp_IsNearBrace
#define NO_BRACE_FOUND		-1

// Value returned from EdDisp_EmergencySave
#define NOT_SAVED		1
#define SAVED			2
#define SAVE_FAILED		3

// Parameters for EdDisp_Run
#define COMPILE_AND_EXECUTE	TRUE
#define COMPILE_ONLY		FALSE
#define SAME_WINDOW_AS_LAST_TIME    ((HWND) -1)
#define RUN_ARGS_DIALOG		TRUE
#define NO_RUN_ARGS_DIALOG	FALSE
#define START_WITH_STEP		TRUE
#define RUN_CONTINUOUSLY	FALSE

// Parameters for EdDisp_SetTuringValues
#define NOTIFY_SAVED		51
#define NOTIFY_OPENED		52
#define NOTIFY_CLOSED		53
#define NOTIFY_NEVER		54

// Parameters for EdDisp_SetBreakpointVisible
#define SHOW_BREAKPOINTS	35
#define HIDE_BREAKPOINTS	36
#define TOGGLE_BREAKPOINTS	37

// Other
#define JAVA_SUFFIX		".java"
#define TURING_SUFFIX_LOWER	".t"
#define TURING_SUFFIX_UPPER	".T"

// Window message to set status
#define WM_SETSTATUSMESSAGE	(WM_USER + 10)


/*********/
/* Types */
/*********/
typedef struct
{
    BOOL	displayFullPathNames;
    BOOL	syntaxColour;
    BOOL	addSuffix;
    int		textFontSize;
    char	textFontName [PROPERTY_LENGTH];
    int		caretWidth;
    BOOL	findUsesSelection;
        
    // Debugging
    BOOL	logging;
    BOOL	debugWindowAreas;
    BOOL	logWindowMessages;
} EdDispOnly_Properties;

typedef struct
{
    EdPrint_Properties		printProperties;
    EdSearch_Properties		searchProperties;
    EdText_Properties		textProperties;    

    EdDispOnly_Properties	myProperties;
} EdDisp_Properties;


/**********************/
/* External variables */
/**********************/

/***********************/
/* External procedures */
/***********************/
// Init, Finalize subprograms
extern BOOL	EdDisp_Init (void);
extern void	EdDisp_Finalize (void);
// Properties subprograms
extern void	EdDisp_PropertiesSet (EdDisp_Properties pmProperties);
extern void	EdDisp_PropertiesImplementChanges (HWND pmTextDisplayWindow);
// Create, Dispose subprograms
extern HWND	EdDisp_Create (HWND pmEditWindow, BOOL pmBeginnerMode, 
			       BOOL pmOneWindowMode);
extern void	EdDisp_Create2 (HWND pmTextDisplayWindow, 
				const char *pmPathName,
				const char *pmExampleName,
				int pmNewSkeleton);
// Other subprograms
extern BOOL	EdDisp_AreBreakpointsVisible (void);
extern BOOL	EdDisp_CheckForClose (HWND pmTextDisplayWindow);
extern void	EdDisp_ClearBreakpoints (char *pmDummy1, HWND pmDummy2, 
				    	 HWND pmTextDisplayWindow, 
				    	 TextPtr pmTextPtr, void *pmDummy3);
extern void	EdDisp_ClearErrors (char *pmDummy1, HWND pmDummy2, 
				    HWND pmTextDisplayWindow, 
				    TextPtr pmTextPtr, void *pmDummy3);
extern void	EdDisp_CreateMarkMenu (HWND pmTextDisplayWindow, HMENU pmMenu);
extern void	EdDisp_SetDisplayResizeMessage (BOOL pmDisplayResizeMessage);
extern int	EdDisp_EmergencySave (HWND pmTextDisplayWindow, 
				      TextPtr pmTextPtr,
				      FilePath myOriginalName, 
			      	      FilePath myEmergencyName);
extern void	EdDisp_EnumerateFile (HWND pmEditWindow, 
			HWND pmTextDisplayWindow, 
			void (*enumProc) (char *, HWND, HWND, void *, void *),
			void *pmUserParam);
extern char	*EdDisp_GetMainFile (FilePath pmPathDisplayBuffer);
extern char	*EdDisp_GetPathForDisplay (HWND pmTextDisplayWindow, 
					   BOOL pmDefaultPathSize, 
					   BOOL pmNoDirtyMark,
					   FilePath pmPathForDisplayBuffer);
extern char	*EdDisp_GetPath (HWND pmTextDisplayWindow);
extern char	*EdDisp_GetPseudoPath (HWND pmTextDisplayWindow);
extern int	EdDisp_GetRedoStringResource (HWND pmTextDisplayWindow);
extern void	EdDisp_GetSelectionLocation (HWND pmTextDisplayWindow, 
					     int *pmLine, int *pmCol, 
					     int *pmNumLines);
extern TextPtr	EdDisp_GetTextPtr (HWND pmTextDisplayWindow);
extern unsigned short EdDisp_GetTuringFileNo (HWND pmTextDisplayWindow);
extern int	*EdDisp_GetTuringSizePtr (HWND pmTextDisplayWindow);
extern char	**EdDisp_GetTuringTextHandle (HWND pmTextDisplayWindow);
extern int	EdDisp_GetUndoStringResource (HWND pmTextDisplayWindow);
extern int	EdDisp_GetUnnamedNumber (HWND pmTextDisplayWindow);					     
extern void	EdDisp_GetWindowSize (HWND pmTextDisplayWindow, int pmNumRows, 
				      int pmNumCols, long *pmPixelWidth, 
				      long *pmPixelHeight);
extern BOOL	EdDisp_GetWindowWithMatchingPathName (const char *pmPathName, 
					      HWND *pmEditWindow,
					      HWND *pmTextDisplayWindow);
extern BOOL	EdDisp_GetWindowWithMatchingTuringFileNo (
				WORD pmTuringFileNo, HWND *pmEditWindow,
				HWND *pmTextDisplayWindow);
extern void	EdDisp_GotoError (HWND pmTextDisplayWindow, int pmLine,
				  int pmTokenPos, int pmTokenLen, 
				  const char *pmErrorMessage);					      
extern void	EdDisp_HandleClose (HWND pmTextDisplayWindow);	      
extern BOOL	EdDisp_HasErrors (HWND pmTextDisplayWindow);
extern void	EdDisp_HilightLine (HWND pmTextDisplayWindow, int pmLine);
extern BOOL	EdDisp_IsAssociated (HWND pmTextDisplayWindow);
extern BOOL	EdDisp_IsDirtyFile (HWND pmTextDisplayWindow);
extern BOOL	EdDisp_IsEmptyFile (HWND pmTextDisplayWindow);
extern BOOL	EdDisp_IsKnownLanguage (HWND pmTextDisplayWindow);
extern int	EdDisp_IsNearBrace (HWND pmTextDisplayWindow);
extern BOOL	EdDisp_IsSelectionActive (HWND pmTextDisplayWindow);
extern BOOL	EdDisp_MatchesPathName (HWND pmTextDisplayWindow, 
					const char *pmPathName);
extern void	EdDisp_MouseWheelSettingsInit (void);
extern BOOL	EdDisp_OpenOver (HWND pmTextDisplayWindow, 
				 const char *pmPathName, int pmPurpose);
extern void	EdDisp_ResizeWindow (char *pmPathName, 
		    HWND pmEditWindow, HWND pmTextDisplayWindow, 
		    TextPtr pmTextPtr, void *pmEnumerateInfoPtr);
extern void	EdDisp_Run (HWND pmTextDisplayWindow, BOOL pmExecute, 
			    BOOL pmWithArgs, BOOL pmStartWithStep);
extern void	EdDisp_SetBreakpointVisible (int pmVisible);
extern BOOL	EdDisp_SetFindSelectionMenuItem (HWND pmTextDisplayWindow, 
					 char *pmFindSelectionMenuItemString);
extern void	EdDisp_SetTuringValues (HWND pmTextDisplayWindow, 
					WORD pmTuringFileNo, 
					char **pmTuringTextHandle, 
    			    		int *pmTuringSizePtr, int pmWhere);
					 
#endif // #ifndef _EDDISP_H_
