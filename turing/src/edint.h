/***********/
/* edint.h */
/***********/

#ifndef _EDINT_H_
#define _EDINT_H_

/*******************/
/* System includes */
/*******************/
//#include <windows.h>

/******************/
/* Other includes */
/******************/
#include "miotypes.h"
#include "language.h"

//#include "eddbg.h"

#include "miowin.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
// Parameters for EdInt_CreateFont
#define NO_BOLD		FALSE
#define NO_ITALIC 	FALSE

/*********/
/* Types */
/*********/

/**********************/
/* External variables */
/**********************/

/***********************/
/* External procedures */
/***********************/
//
// Routines called by MIO
//
extern void	EdInt_Init_Run (void);
extern void	EdInt_AddFailMessage (const char *pmMessage, ...);
extern FONT	EdInt_CreateFont (WIND pmWindow, const char *pmFontName,
				  int pmFontSize, BOOL pmBold, BOOL pmItalic);
extern void	EdInt_Exit (void);
extern void	EdInt_ExitImmediate (void);
extern void	EdInt_FailWarn (int pmReason, char *pmFile, int pmLine, 
			        int pmErrorCode);
extern int	EdInt_GetMouseWheelMessage (void);
extern int	EdInt_GetMouseWheelSettings (void);
extern BOOL	EdInt_GetSaveFile (WIND pmWindow, char *pmPathName);
extern WIND	EdInt_GetTopWindow (WIND pmWindow);
extern MYRECT	EdInt_GetWorkArea (void);			        
extern void	EdInt_HaltEnvironment (int pmReason, char *pmFile, int pmLine, 
			               int pmErrorCode, ...);
extern BOOL	EdInt_IsExecuting (void);
extern void	EdInt_LoadString (int pmStringResource, char *pmString, 
				  int pmStringSize);
extern void	EdInt_LogFont (const char *pmString, 
			       DEVCONTEXT pmDeviceContext);
extern void	EdInt_Message (WIND pmWindow, int pmTitleRsrc, 
			       int pmMessageRsrc);
extern void	EdInt_Message1 (WIND pmWindow, int pmMessageBoxParams, 
				int pmTitleStringResource, 
			        int pmMessageStringResource);
extern int	EdInt_Message2 (WIND pmWindow, int pmMessageBoxParams, 
				int pmTitleStringResource, 
			        int pmMessageStringResource, const char *pmArg);
extern void	EdInt_NewEditorWindow (void);
extern void	EdInt_NotifyAllRunWindowsClosed (void);
extern void	EdInt_NotifyDebuggerAboutError (OOTint pmErrorNo, 
						OOTstring pmErrMsg,
						SRCPOS *pmSrcPos);
extern void	EdInt_NotifyDebuggerInitRun (void);
extern void	EdInt_NotifyDebuggerFinalizeRun (void);
extern void	EdInt_NotifyDebuggerObjectAllocated (int pmObjectType,
						     int pmID, SRCPOS *pmSrcPos,
						     const char *pmDescription);
extern void	EdInt_NotifyDebuggerObjectDeallocated (int pmID);
extern void	EdInt_NotifyRunWindowAdded (WIND pmWindow, char *pmName);
extern void	EdInt_NotifyRunWindowClosed (WIND pmWindow);
extern void	EdInt_NotifyRunWindowRenamed (WIND pmWindow, char *pmName);
extern void	EdInt_OpenEditorFile (void);			        
extern void	EdInt_PauseExecution (void);
extern void	EdInt_PrintGraphics (WIND pmWindow, DEVCONTEXT pmSource, 
				     int pmWidth, int pmHeight,
				     PALETTE pmPalette, int pmNormalWidth,
				     BOOL pmDisplayPrintDialog, 
				     BOOL pmDrawBorder, BOOL pmScaleToFit,
				     BOOL pmDrawHeader, char *pmTitle);
extern void	EdInt_ProcessOutstandingMessages (void);
extern void	EdInt_RerunProgram (void);
extern void	EdInt_ResumeExecution (void);
extern void	EdInt_SetDebuggingState (BOOL pmState);
extern void	EdInt_StopExecution (void);
extern void	EdInt_SubmitPrintJob (WIND pmWindow, char *pmText,
				      int pmTextLen, char *pmProgramName);

#endif // #ifndef _EDINT_H_
