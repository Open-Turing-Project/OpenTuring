/***********/
/* edrun.h */
/***********/

#ifndef _EDRUN_H_
#define _EDRUN_H_

/*******************/
/* System includes */
/*******************/
#include <windows.h>
#include <sys/types.h>
#include <sys/stat.h>

/******************/
/* Other includes */
/******************/
#include "edfile.h"
#include "edtext.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
// Parameters to EdRun_Execute
#define NO_PROGRAM		0
#define APPLICATION		1
#define APPLET			2

// Parameter to EdRun_SetStep

// Parameters to EdRun_FindNextError
#define MESSAGE_IF_NO_ERROR	TRUE
#define NO_MESSAGE_IF_NO_ERROR	FALSE

// Parameter to EdRun_PauseResumeProgram and EdRun_SetStep
#define NO_STEP			54
#define PAUSING			55
#define NO_BREAK		56
#define HIT_BREAK_POINT		57
#define HIT_UNIT_BREAK_POINT	58
#define HIT_BREAK_STATEMENT	59
#define STEP			60
#define STEP_OVER		61
#define STEP_RETURN		62
#define TRACE_ON		63


/*********/
/* Types */
/*********/
typedef struct OutputPrintJob
{
    char	*className;
    char	*programOutput;
    int		outputLen;
    HWND	submitterWindow;
} OutputPrintJob;

/**********************/
/* External variables */
/**********************/

/***********************/
/* External procedures */
/***********************/
// Init, Finalize subprograms
extern BOOL		EdRun_Init (void);
extern void		EdRun_Finalize (void);
// Properties subprograms
extern void		EdRun_PropertiesSet (void);
extern void		EdRun_PropertiesImplementChanges (void);
// Other subprograms
extern void		EdRun_CloseAllRunWindows (HWND pmEditWindow);
extern BOOL		EdRun_Compile (HWND pmWindow, const char *pmPathName, 
		       		       BOOL pmCompileOnly);
extern int	EdRun_CreateByteCodeFile (FilePath pmProgramPath,FilePath pmOutputPath);
extern void		EdRun_CreateEXE (HWND pmTextDisplayWindow, 
					 FilePath pmPathName, 
			 		 BOOL pmCloseWindowsOnTerminate,
			 		 BOOL pmDisplayRunWithArgs, 
			 		 BOOL pmCenterOutputWindow, 
			 		 BOOL pmStopUserClose);
extern void		EdRun_DebuggerOutput (int pmReason, char *pmOutput);
extern void		EdRun_DeleteError (ErrorPtr pmError);
extern BOOL		EdRun_Execute (HWND pmWindow, const char *pmPathName,
				       int pmProgramKind, BOOL pmWithArgs,
				       BOOL pmStartWithStep);
extern void		EdRun_FindNextError (HWND pmTextDisplayWindow,
					     BOOL pmMessageIfNoError);
extern void		EdRun_FixMain (HWND pmTextDisplayWindow);
extern void		EdRun_FixMainPathName (const char *pmPathName);
extern void		EdRun_FloatMain (void);
extern void		EdRun_GetJavaDLLPath (char *pmJavaDLLPath);
extern unsigned short	EdRun_GetMainFileNo (void);
extern const char	*EdRun_GetMainPathName (void);
extern int		EdRun_GetMainProgramType (void);
extern HWND		EdRun_GetMainWindow (void);
extern BOOL		EdRun_GetPseudoPath (HWND pmWindow, char *pmText, 
					     int pmTextLen, char *pmPseudoPath);
extern void		EdRun_GotoLineInDocument (unsigned short pmFileNo, 
						  unsigned short pmLineNo,
						  BOOL forceFlags);
extern BOOL		EdRun_HasErrors (void);
extern void		EdRun_InitializeForRunWithoutCompile (HWND pmWindow);
extern BOOL		EdRun_IsAnyRunWindowVisible (void);
extern BOOL		EdRun_IsProgramPaused (void);
extern BOOL		EdRun_IsProgramRunning (void);
extern BOOL		EdRun_IsProgramRunningAnApplet (void);
extern BOOL		EdRun_IsMainPathName (const char *pmPathName);
extern BOOL		EdRun_IsTracing (void);
extern void		EdRun_KillRunningProgram (void);
extern BOOL		EdRun_KillRunningProgramAndQuit (HWND pmWindow);
extern void		EdRun_KillRunningProgramAndRerun (void);
extern TextPtr		EdRun_MatchPath (const char *pmPathName);
extern HWND		EdRun_MatchTuringFileNo (WORD pmTuringFileNo);
extern void		EdRun_NotifyAllRunWindowsClosed (void);
extern void		EdRun_NotifyFileClosed (HWND pmTextDisplayWindow);
extern void		EdRun_NotifyFileOpened (HWND pmTextDisplayWindow);
extern void		EdRun_NotifyFileSaved (HWND pmTextDisplayWindow);
extern void		EdRun_NotifyRunWindowOpened (HWND pmRunWindow);
extern void		EdRun_PauseResumeProgram (BOOL pmActivateSourceWindow,
						  int pmReason);
extern void		EdRun_ResetCompiler ();
extern void		EdRun_SendInfoToStderr (BOOL pmLibErrors, 
						BOOL pmLibInfo);
extern void		EdRun_SetStream0Destination (void);
extern void		EdRun_SetStep (int pmStep);
extern void		EdRun_SetTraceSpeed (int pmTraceSpeed);
extern void		EdRun_TestSuite (const char *pmFileName, 
					 const char *pmOutputDir);
extern void	EdRun_RunProgramNoEditor (const char *pmTestDirectory, const char *pmTestFileName);
extern void		EdRun_ToggleBreakpointVisibility (void);
extern void		EdRun_ToggleTracing (void);

// Called by Jikes Compiler
extern BOOL		EdRun_GetFileStat (char *pmPathName, 
					   struct stat *pmStat);
extern const char	*EdRun_GetJavaFile (void);
extern void		EdRun_GetJavaFileInitialize (const char *pmDirectory);
extern BOOL		EdRun_GetOpenFile (const char *pmPathName, 
					   const char **pmBuffer, 
					   unsigned int *pmSize);
extern void		EdRun_NotifyClassRead (const char *pmJavaFile, 
					       const char *pmClassFile,
					       const char *pmFullClassName);
extern void		EdRun_ProcessJikesErrorMsg (const char *pmPathName, 
			    int pmLeftLineNo, int pmLeftCol, int pmRightLineNo,
			    int pmRightCol, int pmSeverity, 
			    const char *pmMessage);
extern void		EdRun_StatusCompiling (const char *pmClassName);
extern void		EdRun_StatusFinishedReading (void);
extern void		EdRun_StatusReading (const char *pmPathName);

#endif // #ifndef _EDRUN_H_
