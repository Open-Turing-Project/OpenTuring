/*********/
/* mio.h */
/*********/

#ifndef _MIO_H_
#define _MIO_H_

/*******************/
/* System includes */
/*******************/

/******************/
/* Other includes */
/******************/
#include "miotypes.h"

#include "language.h"
#include "levent.h"

#include "miowin.h"

// Made reference to by run-time error macros
#include "mioerror.h"
//#include "edint.h"

/**********/
/* Macros */
/**********/
#define ABORT_WITH_ERRMSG	MIOError_AbortWithErrMsg
#define ABORT_WITH_ERRNO	MIOError_AbortWithErrNo
#define SET_ERRMSG		MIOError_SetErrMsg
#define SET_ERRNO		MIOError_SetErrNo

/*************/
/* Constants */
/*************/
#define WINDOW_TYPE_BEGINNER_TOP		104
#define WINDOW_TYPE_MIO_TEXT			108
#define WINDOW_TYPE_MIO_TEXT_INNER		109
#define WINDOW_TYPE_MIO_GRAPHICS		110
#define WINDOW_TYPE_MIO_GRAPHICS_INNER		111
#define WINDOW_TYPE_MIO_DIALOG			112

// Values for MIO_Initialize
#define OS_WINDOWS			1
#define OS_MACOS			2
#define OS_UNIXLIKE			3

// Values for pmWhen in Fseek
#define SEEK_BEGIN			0
#define SEEK_CURRENT			1
#define SEEK_END			2

// Values for pmStreamType 
#define FILE_STREAM			10
#define WINDOW_STREAM			11
#define NET_STREAM			12
#define UNKNOWN_STREAM			13

// Values for pmIDType 
#define FONT_ID				20
#define DIR_ID				21
#define PIC_ID				22
#define SPRITE_ID			23
#define LEXER_ID			24
#define UNKNOWN_ID			25
#define ZERO_ID				26

/*********/
/* Types */
/*********/
// MIOFILE can represent (1) a file - filePtr is a HANDLE, (2) a 
// graphics window - windowPtr is HWND, (3) a text window - windowPtr is
// a HWND, (4) a socket - filePtr is a ???, (5) a printer file - filePtr
// is a MIOPRINTER, (6) a combination of (1),(4),(5) and (2),(3) in which 
// case output is going to a file and window simultaneously.
typedef struct MIOFILE 
{
    void	*filePtr;	// Set to the file pointer
    int		fileType;	// The type represented by filePtr
    void	*windowPtr;	// The window
    int		windowType;	// The type represented by windowPtr
    BOOL	isWindowFile;	// Is this a window masquerading as a file
} MIOFILE;


/**********************/
/* External variables */
/**********************/
extern INSTANCE		MIO_applicationInstance;
extern WIND		MIO_selectedRunWindow;
extern MIOWinInfoPtr	MIO_selectedRunWindowInfo;
extern int		MIO_parallelIOPort;
extern BOOL		MIO_paused, MIO_finished;
extern WIND		MIO_caretOwner;
extern char		*MIO_programName;

/***********************/
/* External procedures */
/***********************/
extern void	MIO_Initialize (			    // Called by edrun
				INSTANCE pmApplicationInstance, int pmOS,
				const char *pmOOTDir, const char *pmHomeDir,
			        BOOL pmCenterOutputWindow, 
			        BOOL pmStopUserClose,
				const char *pmSysExitString);	    
extern void	MIO_Finalize (void);			    // Called by edrun
extern void	MIO_Init_Free (void);			    // Called by edrun
extern BOOL	MIO_Init_Run (const char *pmProgramName,    // Called by edrun
		      const char *pmInputPath, BOOL pmEchoInput,
		      const char *pmOutputPath, BOOL pmOutputToScreenAndFile,
		      BOOL pmOutputToPrinter, const char *pmExecutionDirectory,
		      BOOL pmDefaultTextMode, const char *pmFontName, 
		      int pmFontSize, int pmFontWidth, 
		      int pmFontOptions, int pmWindowDimension, 
		      int pmWindowWidth, int pmWindowHeight, 
		      int pmWindowRows, int pmWindowColumns,
		      BOOL pmFullScreen, COLOR pmSelectionColour,
		      BOOL pmAllowSysExec, BOOL pmAllowSound,
		      int pmParallelIOPort, BOOL pmTestSuiteProgram);
extern void	MIO_Finalize_Run (void);		    // Called by edrun
extern void	MIO_AddToRunWindowList (WIND pmWindow);
extern void	MIO_CheckColourRange (OOTint pmClr);
extern void	MIO_CheckInputIsFromKeyboard (const char *pmRoutineName);
extern void	MIO_CheckLineStyleRange (OOTint pmStyle);
extern void	MIO_CheckOuputIsToWindow (const char *pmRoutineName);
extern void	MIO_CheckOuputWindowIsInGraphicsMode (
						const char *pmRoutineName);
extern void	MIO_CloseAllRunWindows (void);		    // Called by edrun
extern void	MIO_DebugOut (const char *pmMessage, ...);
extern void	MIO_ErrorInfo (const char *pmMessage, ...);
extern MIOWinInfoPtr	MIO_GetTopMostWindow (void);
extern int	MIO_IDAdd (int pmIDType, void *pmInfo, SRCPOS *pmSrcPos,
			   const char *pmDescription, void *pmCompareInfo);
extern int	MIO_IDCompare (int pmIDType, void *pmCompareInfo, 
			       int pmCompareInfoSize);
extern void	MIO_IDDecrement (int pmSlotNumber);
extern void	MIO_IDFree (int pmSlotNumber);
extern void	*MIO_IDGet (int pmIDNumber, int pmIDType);
extern int	MIO_IDGetCount (int pmSlotNumber, int pmIDType);
extern void	MIO_IDIncrement (int pmSlotNumber);
extern void	MIO_IDRemove (int pmIDNumber, int pmIDType);
extern BOOL	MIO_IsAnyRunWindowVisible (void);	    // Called by edrun
extern MIOWinInfoPtr	MIO_ListTopMostWindows (BOOL pmStart);
extern void	MIO_MakePopupWindowVisible (void);
extern char	*MIO_MallocString (const char *pmString);
extern void	MIO_MoreInfo (const char *pmMessage, ...);
extern void	MIO_NotifyTuringProgramFinished (void);	    // Called by edrun
extern void	MIO_NotifyTuringProgramPaused (void);	    // Called by edrun
extern void	MIO_NotifyTuringProgramResumed (	    // Called by edrun
						BOOL pmPutMIOWindowsOnTop);
extern BOOL	MIO_RectanglesIntersect (MYRECT *pmRect1, MYRECT *pmRect2);
extern BOOL	MIO_RectanglesSetIntersect (MYRECT *pmRect1, MYRECT *pmRect2);
extern void	MIO_RectanglesUnion (MYRECT *pmRect1, MYRECT *pmRect2);
extern void	MIO_RemoveFromRunWindowList (WIND pmWindow);
extern void	MIO_SendInfoToStderr (BOOL pmLibErrors,	    // Called by edrun
				      BOOL pmLibInfo);
extern void	MIO_SetNextWindowActive (WIND pmWindow);
extern void	MIO_UpdateSpritesIfNecessary (void);	    // Called by edrun

#ifdef NOT_YET_NEEDED
extern int	MIO_StreamAdd (int pmStreamType, void *pmInfo);
extern void	*MIO_StreamGet (int pmStreamID, int pmStreamType);
extern void	MIO_StreamRemove (int pmStreamID, int pmStreamType);
#endif // #ifdef NOT_YET_NEEDED
extern void	MIO_SubstituteRunWindow (WIND pmOldWindow, WIND pmNewWindow,
					 const char *pmDescription);

// Called by executor
extern void		MIO (void);
extern void		MIO_Init (void);
extern void		MIO_DrawPic (OOTint pmX, OOTint pmY, char *pmBuffer, 
				     OOTint pmMode);
extern char		MIO_Getch (void);
extern void		MIO_GetEvent (TW_OOT_EVENT *pmEventPtr);
extern OOTboolean	MIO_Hasch (void);
extern OOTboolean	MIO_HasEvent (MIOFILE *pmMIOFile, 
				      EventDescriptor *pmEvent);
extern void		MIO_RegisterClose (OOTint pmID);
extern void		MIO_RegisterOpen (OOTint pmID, OOTint pmMode);
extern OOTboolean	MIO_SetActive (MIOFILE *pmMIOFile);

#endif // #ifndef _MIO_H_
