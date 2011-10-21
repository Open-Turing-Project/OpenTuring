/*************/
/* mdiowin.h */
/*************/

#ifndef _MDIOWIN_H_
#define _MDIOWIN_H_

/*******************/
/* System includes */
/*******************/

/******************/
/* Other includes */
/******************/
#include "miotypes.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
// MDIOWin_CreateDestroyCaret
#define CREATE_CARET		    TRUE
#define DESTROY_CARET		    FALSE

// All Turing editor windows define 12 bytes of extra data as follows
#define	WINDOW_MEM_WINDOW_TYPE		 0
#define	WINDOW_MEM_WINDOW_INITIALIZED	 4
#define WINDOW_MEM_WINDOW_INFO		 8

#define WINDOW_EXTRA_MEMORY		12

// Constant to prevent deallocation of a window that is being replaced
// rather than closed.
#define WINDOW_CHANGED_NOT_CLOSED	-999

/*********/
/* Types */
/*********/

/**********************/
/* External variables */
/**********************/

/***********************/
/* External procedures */
/***********************/
extern void	MDIOWin_Init (void);
extern void	MDIOWin_Finalize (void);
extern void	MDIOWin_AdjustContextualMenu (MENU pmMenu);
extern void	MDIOWin_CalculateWindowRect (MYRECT *pmRect);
extern void	MDIOWin_CaretHide (WIND pmInnerWindow);
extern void	MDIOWin_CaretMove (WIND pmInnerWindow, int pmX, int pmY);
extern void	MDIOWin_CaretShow (WIND pmInnerWindow);
extern BOOL	MDIOWin_CheckForCloseDialog (WIND pmWindow);
extern void	MDIOWin_CloseWindow (WIND pmWindow);
extern void	MDIOWin_CopyPaletteFromOldToNewWindow (WIND pmOldWindow, 
						       WIND pmNewWindow);
extern void	MDIOWin_CreateDestroyCaret (WIND pmInnerWindow,
					    BOOL pmCreate);
extern void	MDIOWin_DestroyWindow (WIND pmWindow);
extern void	MDIOWin_Dispose (MIOWinInfoPtr pmInfo);
extern WIND	MDIOWin_GetActiveWindow (void);
extern char	*MDIOWin_GetClipboardBuffer (WIND pmWindow);
extern UINT	MDIOWin_GetDoubleClickTime (void);
extern int	MDIOWin_GetHScrollBarWidth (void);
extern long	MDIOWin_GetMessageTime (void);
extern WIND	MDIOWin_GetParent (WIND pmWindow);
extern int	MDIOWin_GetTitleBarHeight (void);
extern int	MDIOWin_GetVScrollBarWidth (void);
extern int	MDIOWin_GetWheelMouseDeltaPerLine (void);
extern void	MDIOWin_GetWindowRect (WIND pmWindow, MYRECT *pmRect);
extern void	MDIOWin_GetWindowTitle (WIND pmWindow, char *pmTitle);
extern int	MDIOWin_GetWindowType (WIND pmWindow);
extern void	*MDIOWin_GetWindowInfo (WIND pmWindow);
extern BOOL	MDIOWin_HandleControlKey (WIND pmWindow, int pmKeyCode);
extern void	MDIOWin_HideWindow (WIND pmWindow);
extern BOOL	MDIOWin_IsVisible (WIND pmWindow);
extern void	MDIOWin_ResizeInnerWindow (WIND pmInnerWindow);
extern void	MDIOWin_ResizeRunWindow (WIND pmWindow);
extern void	MDIOWin_SetActiveWindow (WIND pmWindow);
extern BOOL	MDIOWin_SetDisplayFonts (WIND pmWindow, 
			const char *pmFontName, int pmFontSize, 
			int *pmFontHeight, int *pmFontWidth, 
			int *pmFontAscent, int *pmFontDescent, 
			int *pmFontIntLeading, FONT *pmFont, 
			COLOR pmDesiredColour, COLOR *pmColour);
extern void	MDIOWin_SetWindowInfo (WIND pmWindow, void *pmInfoPtr);
extern void	MDIOWin_SetWindowInitialization (WIND pmWindow, 
						 BOOL pmInitialized);
extern void	MDIOWin_SetWindowPosition (WIND pmWindow, int pmXPos, 
					   int pmYPos, int pmWidth, 
					   int pmHeight);
extern void	MDIOWin_SetWindowTitle (WIND pmWindow, const char *pmTitle);
extern void	MDIOWin_SetWindowType (WIND pmWindow, int pmWindowType);
extern void	MDIOWin_ShowWindow (WIND pmWindow);
extern UCHAR	MDIOWin_TranslateAltKeystroke (int pmChar);
extern UCHAR	MDIOWin_TranslateSpecialKeystroke (int pmWindowsKeycode);
extern void	MDIOWin_Update (WIND pmWindow);
extern void	MDIOWin_UpdateArea (WIND pmWindow, int x1, int y1, 
				    int x2, int y2);
extern long	MDIOWin_WindowProcedure (WIND pmWindow, UINT pmMessage, 
					 WPARAM pmWParam, LPARAM pmLParam,
					 BOOL pmIsGraphicsWindow);

#endif // #ifndef _MDIOWIN_H_
