/************************************************************************/
/* edint.c								*/
/*									*/
/* This module provides the interface for the run time system back to	*/
/* the editor.  Rather than calling the editor directly, the Turing	*/
/* run time system calls routines in this module which redirect them	*/
/* to the editor.							*/
/************************************************************************/

/*******************/
/* System includes */
/*******************/

/****************/
/* Self include */
/****************/
#include "edint.h"

/******************/
/* Other includes */
/******************/
#include "ed.h"
//#include "eddbg.h"
#include "edfail.h"
#include "edgui.h"
#include "edrun.h"

#include "dbgalloc.h"

#include "edcommands.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/

/********************/
/* Global variables */
/********************/
/*char		*EdInt_fontName;
int		EdInt_fontSize, EdInt_fontWidth, EdInt_fontOptions;
int		EdInt_windowDimension;
int		EdInt_windowWidth, EdInt_windowHeight;
int		EdInt_windowRows, EdInt_windowColumns;
WIND		hCaretOwner; // Temporary!
*/

/*********/
/* Types */
/*********/

/**********************/
/* External variables */
/**********************/

/********************/
/* Static constants */
/********************/

/********************/
/* Static variables */
/********************/
static int	stNumRunWindowsVisible = 0;

/******************************/
/* Static callback procedures */
/******************************/

/*********************/
/* Static procedures */
/*********************/


/****************************************/
/* External procedures called by Turing */
/****************************************/
/*************************************/
/* External procedures called by MIO */
/*************************************/
/************************************************************************/
/* EdInt_Init_Run							*/
/************************************************************************/
void	EdInt_Init_Run (void)
{
    stNumRunWindowsVisible = 0;
} // EdInt_Init_Run


/************************************************************************/
/* EdInt_AddFailMessage							*/
/************************************************************************/
void	EdInt_AddFailMessage (const char *pmMessage, ...)
{
#ifndef TPROLOG
    char	myMessage [4096];
    va_list 	myArgList;
    
    va_start (myArgList, pmMessage);
    wvsprintf (myMessage, pmMessage, myArgList);
    EdFail_AddMessage (NORMAL, myMessage);
    va_end (myArgList);
#endif // #ifndef TPROLOG
} // EdInt_AddFailMessage


/************************************************************************/
/* EdInt_CreateFont							*/
/************************************************************************/
FONT	EdInt_CreateFont (WIND pmWindow, const char *pmFontName,
			  int pmFontSize, BOOL pmBold, BOOL pmItalic)
{
    return EdGUI_CreateFont (pmWindow, NULL, pmFontName, pmFontSize, 
        		     pmBold, pmItalic);
} // EdInt_CreateFont

			  
/************************************************************************/
/* EdInt_Exit								*/
/************************************************************************/
void	EdInt_Exit (void)
{
#ifdef TPROLOG
    EdRun_KillRunningProgramAndQuit (Ed_GetLastActiveEditWindow ());
#else    
    EdFail_AddMessage (NORMAL, "Exit Turing from Run Window");
    Ed_ExitApplication (Ed_GetLastActiveEditWindow (), FALSE);
#endif // #else - #ifdef TPROLOG
} // EdInt_Exit


/************************************************************************/
/* EdInt_ExitImmediate							*/
/************************************************************************/
void	EdInt_ExitImmediate (void)
{
#ifdef TPROLOG
    EdRun_KillRunningProgramAndQuit (Ed_GetLastActiveEditWindow ());
#else    
    Ed_ExitApplication (Ed_GetLastActiveEditWindow (), TRUE);
#endif // #else - #ifdef TPROLOG
} // EdInt_ExitImmediate


/************************************************************************/
/* EdInt_FailWarn							*/
/************************************************************************/
void	EdInt_FailWarn (int pmReason, char *pmFile, int pmLine, int pmErrorCode)
{
    EdFail_Warn (pmReason, pmFile, pmLine, pmErrorCode);
} // EdInt_FailWarn


/************************************************************************/
/* EdInt_GetMouseWheelMessage						*/
/************************************************************************/
int	EdInt_GetMouseWheelMessage (void)
{
    return EdGUI_GetMouseWheelMessage ();
} // EdInt_GetMouseWheelMessage


/************************************************************************/
/* EdInt_GetMouseWheelSettings						*/
/************************************************************************/
int	EdInt_GetMouseWheelSettings (void)
{
    return EdGUI_GetMouseWheelSettings ();
} // EdInt_GetMouseWheelSettings


/************************************************************************/
/* EdInt_GetSaveFile							*/
/************************************************************************/
BOOL	EdInt_GetSaveFile (WIND pmWindow, char *pmPathName)
{
    return EdGUI_GetSaveFile ((HWND) pmWindow, pmPathName, FALSE);
} // EdInt_GetSaveFile


/************************************************************************/
/* EdInt_GetTopWindow							*/
/************************************************************************/
WIND	EdInt_GetTopWindow (WIND pmWindow)
{
    return EdGUI_GetTopWindow ((HWND) pmWindow);
} // EdInt_GetTopWindow


/************************************************************************/
/* EdInt_GetWorkArea							*/
/************************************************************************/
MYRECT	EdInt_GetWorkArea (void)
{
    RECT	myRect;
    MYRECT	myResultRect;
    
    myRect = EdGUI_GetWorkArea ();
    
    myResultRect.top = myRect.top;
    myResultRect.left = myRect.left;
    myResultRect.bottom = myRect.bottom;
    myResultRect.right = myRect.right;
    
    return myResultRect;
} // EdInt_GetWorkArea


/************************************************************************/
/* EdInt_HaltEnvironment						*/
/************************************************************************/
void	EdInt_HaltEnvironment (int pmReason, char *pmFile, int pmLine, 
		               int pmErrorCode, ...)
{
    va_list 	myArgList;

    // Output the error message to dialog box and file    
    va_start (myArgList, pmErrorCode);
    EdFail_Fatal1 (pmReason, pmFile, pmLine, pmErrorCode, myArgList);
    va_end (myArgList);
} // EdInt_HaltEnvironment

		               
/************************************************************************/
/* EdInt_IsExecuting							*/
/************************************************************************/
BOOL	EdInt_IsExecuting (void)
{
    return EdRun_IsProgramRunning () || EdRun_IsProgramPaused ();
} // EdInt_IsExecuting


/************************************************************************/
/* EdInt_LoadString							*/
/************************************************************************/
void	EdInt_LoadString (int pmStringResource, char *pmString, 
			  int pmStringSize)
{
    EdGUI_LoadString (pmStringResource, pmString, pmStringSize);
} // EdInt_LoadString


/************************************************************************/
/* EdInt_LogFont							*/
/************************************************************************/
void	EdInt_LogFont (const char *pmString, HDC pmDeviceContext)
{
} // EdInt_LogFont


/************************************************************************/
/* EdInt_Message							*/
/************************************************************************/
void	EdInt_Message (WIND pmWindow, int pmTitleRsrc, int pmMessageRsrc)
{
    EdGUI_Message1 (pmWindow, 0, pmTitleRsrc, pmMessageRsrc);
} // EdInt_Message


/************************************************************************/
/* EdInt_Message1							*/
/************************************************************************/
void	EdInt_Message1 (WIND pmWindow, int pmMessageBoxParams, 
			int pmTitleStringResource, int pmMessageStringResource)
{
    EdGUI_Message1 (pmWindow, pmMessageBoxParams, pmTitleStringResource,
    		    pmMessageStringResource);
} // EdInt_Message1

			
/************************************************************************/
/* EdInt_Message2							*/
/************************************************************************/
int	EdInt_Message2 (WIND pmWindow, int pmMessageBoxParams, 
			int pmTitleStringResource, int pmMessageStringResource,
			const char *pmArg)
{
    return EdGUI_Message1 (pmWindow, pmMessageBoxParams, pmTitleStringResource,
    		           pmMessageStringResource, pmArg);
} // EdInt_Message2

			
/************************************************************************/
/* EdInt_NewEditorWindow						*/
/************************************************************************/
void	EdInt_NewEditorWindow (void)
{
#ifndef TPROLOG
    EdFail_AddMessage (NORMAL, "New Editor Window from Run Window");
    SendMessage (Ed_GetLastActiveEditWindow (), WM_COMMAND, 
    		 (WPARAM) COMMAND_NEW_UNNAMED, 0);
#endif // #ifndef TPROLOG
} // EdInt_NewEditorWindow


/************************************************************************/
/* EdInt_NotifyDebuggerInitRun						*/
/************************************************************************/
void	EdInt_NotifyDebuggerInitRun (void)
{
#ifndef TPROLOG
    EdFail_AddMessage (NORMAL, "Debugger Notify Init Run");
    DbgAlloc_Init_Run ();
#endif // #ifndef TPROLOG
} // EdInt_NotifyDebuggerInitRun


/************************************************************************/
/* EdInt_NotifyDebuggerFinalizeRun					*/
/************************************************************************/
void	EdInt_NotifyDebuggerFinalizeRun (void)
{
#ifndef TPROLOG
    EdFail_AddMessage (NORMAL, "Debugger Notify Finalize Run");
    DbgAlloc_Finalize_Run ();
#endif // #ifndef TPROLOG
} // EdInt_NotifyDebuggerFinalizeRun


/************************************************************************/
/* EdInt_NotifyDebuggerObjectAllocated					*/
/************************************************************************/
void	EdInt_NotifyDebuggerObjectAllocated (int pmObjectType, int pmID, 
					     SRCPOS *pmSrcPos, 
					     const char *pmDescription)
{
#ifndef TPROLOG
    EdFail_AddMessage (NORMAL, "Object Allocated %d %d", pmObjectType, pmID);
    DbgAlloc_Allocate (pmObjectType, pmID, pmSrcPos, pmDescription);
#endif // #ifndef TPROLOG
} // EdInt_NotifyDebuggerObjectAllocated


/************************************************************************/
/* EdInt_NotifyDebuggerObjectDeallocated				*/
/************************************************************************/
void	EdInt_NotifyDebuggerObjectDeallocated (int pmID)
{
#ifndef TPROLOG
    EdFail_AddMessage (NORMAL, "Object Freed %d", pmID);
    DbgAlloc_Free (pmID);
#endif // #ifndef TPROLOG
} // EdInt_NotifyDebuggerObjectDeallocated


/************************************************************************/
/* EdInt_NotifyDebuggerAboutError					*/
/************************************************************************/
void	EdInt_NotifyDebuggerAboutError (OOTint pmErrorNo, OOTstring pmErrMsg,
					SRCPOS *pmSrcPos)
{
} // EdInt_NotifyDebuggerAboutError


/************************************************************************/
/* EdInt_NotifyRunWindowActivated					*/
/************************************************************************/
void	EdInt_NotifyRunWindowActivated (HWND pmWindow)
{
} // EdInt_NotifyRunWindowActivated


/************************************************************************/
/* EdInt_NotifyRunWindowAdded						*/
/************************************************************************/
void	EdInt_NotifyRunWindowAdded (HWND pmWindow, char *pmName)
{
#ifndef TPROLOG
    EdFail_AddMessage (NORMAL, "Run Window Added %x %s", pmWindow, pmName);
#endif // #ifndef TPROLOG
    stNumRunWindowsVisible++;
    EdRun_NotifyRunWindowOpened (pmWindow);
} // EdInt_NotifyRunWindowAdded


/************************************************************************/
/* EdInt_NotifyRunWindowClosed						*/
/************************************************************************/
void	EdInt_NotifyRunWindowClosed (HWND pmWindow)
{
    stNumRunWindowsVisible--;
    if (stNumRunWindowsVisible == 0)
    {
#ifndef TPROLOG
    	EdFail_AddMessage (NORMAL, "Notify All Run Windows Closed");
#endif // #ifndef TPROLOG
    	EdRun_NotifyAllRunWindowsClosed ();
    }
} // EdInt_NotifyRunWindowClosed


/************************************************************************/
/* EdInt_NotifyRunWindowRenamed						*/
/************************************************************************/
void	EdInt_NotifyRunWindowRenamed (HWND pmWindow, char *pmName)
{
} // EdInt_NotifyRunWindowRenamed


/************************************************************************/
/* EdInt_OpenEditorFile							*/
/************************************************************************/
void	EdInt_OpenEditorFile (void)
{
#ifndef TPROLOG
    EdFail_AddMessage (NORMAL, "Open File in Editor from Run Window");
    SendMessage (Ed_GetLastActiveEditWindow (), WM_COMMAND, 
    		 (WPARAM) COMMAND_OPEN_FILE, 0);
#endif // #ifndef TPROLOG
} // EdInt_OpenEditorFile


/************************************************************************/
/* EdInt_ProcessOutstandingMessages					*/
/************************************************************************/
void	EdInt_ProcessOutstandingMessages (void)
{
    // TW
} // EdInt_ProcessOutstandingMessages


/************************************************************************/
/* EdInt_PauseExecution							*/
/************************************************************************/
void	EdInt_PauseExecution (void)
{
#ifndef TPROLOG
    EdFail_AddMessage (NORMAL, "Pause Execution from Run Window");
#endif // #ifndef TPROLOG
    EdRun_PauseResumeProgram (FALSE, PAUSING);
} // EdInt_PauseExecution


/************************************************************************/
/* EdInt_PrintGraphics							*/
/************************************************************************/
void	EdInt_PrintGraphics (WIND pmWindow, DEVCONTEXT pmSource, 
			     int pmWidth, int pmHeight,
			     PALETTE pmPalette, int pmNormalWidth,
			     BOOL pmDisplayPrintDialog, BOOL pmDrawBorder, 
			     BOOL pmScaleToFit, BOOL pmDrawHeader, 
			     char *pmTitle)
{
    EdPrint_PrintGraphics (pmWindow, pmSource, pmWidth, pmHeight, pmPalette,
			   pmNormalWidth, pmDisplayPrintDialog, pmDrawBorder, 
			   pmScaleToFit, pmDrawHeader, pmTitle);
} // EdInt_PrintGraphics


/************************************************************************/
/* EdInt_RerunProgram							*/
/************************************************************************/
void	EdInt_RerunProgram (void)
{
#ifndef TPROLOG
    EdFail_AddMessage (NORMAL, "Rerun Program from Run Window");
    EdRun_KillRunningProgramAndRerun ();
#endif // #ifndef TPROLOG
} // EdInt_RerunProgram


/************************************************************************/
/* EdInt_ResumeExecution						*/
/************************************************************************/
void	EdInt_ResumeExecution (void)
{
#ifndef TPROLOG
    EdFail_AddMessage (NORMAL, "Resume Execution from Run Window");
#endif // #ifndef TPROLOG
    EdRun_PauseResumeProgram (FALSE, PAUSING);
} // EdInt_ResumeExecution


/************************************************************************/
/* EdInt_SetDebuggingState						*/
/************************************************************************/
void	EdInt_SetDebuggingState (BOOL pmState)
{
    // TW
} // EdInt_SetDebuggingState


/************************************************************************/
/* EdInt_StopExecution							*/
/************************************************************************/
void	EdInt_StopExecution (void)
{
#ifndef TPROLOG
    EdFail_AddMessage (NORMAL, "Stop Execution from Run Window");
#endif // #ifndef TPROLOG
    EdRun_KillRunningProgram ();
} // EdInt_StopExecution


/************************************************************************/
/* EdInt_SubmitPrintJob							*/
/************************************************************************/
void	EdInt_SubmitPrintJob (HWND pmWindow, char *pmText, int pmTextLen,
			      char *pmProgramName)
{
    EdPrint_PrintOutput (Ed_GetLastActiveEditWindow (), pmText, pmTextLen, 
    			 pmProgramName);
    // TW Get program name being run    			 
} // EdInt_SubmitPrintJob


/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/


