/************/
/* miowin.h */
/************/

#ifndef _MIOWIN_H_
#define _MIOWIN_H_

/*******************/
/* System includes */
/*******************/

/******************/
/* Other includes */
/******************/
#include "miotypes.h"

#include "mdiowintop.h"

/**********/
/* Macros */
/**********/
#define INC(x)			(((x) + 1) % INPUT_BUFFER_SIZE)
#define MIN(x,y)		((x) < (y) ? (x) : (y))
#define MAX(x,y)		((x) > (y) ? (x) : (y))

/*************/
/* Constants */
/*************/
// Used by MIOWindow_AddText
#define MARK_END_OF_OUTPUT_POSITION	1
#define DONT_MARK			2

#define PROPERTY_LENGTH		     1024

// The size of the margins around the text/graphics in a window
#define MARGIN				2

//
// Use for WindowAttrib
//
#define INITIAL_SETUP		TRUE
#define SETSCREEN_SETUP		FALSE

// For unset values
#define UNSET_VALUE	    -9999
// For sizes
#define MAX_SIZE	    -9998
// mode
#define TEXT_MODE		1
#define GRAPHICS_MODE		2
// visibility
#define VIS_POPUP		4
#define VIS_VISIBLE		5
#define VIS_INVISIBLE		6
// xPosition
#define LEFT_ALIGN		-9911
#define CENTRE_ALIGN		-9912
#define RIGHT_ALIGN		-9913
// yPosition
#define TOP_ALIGN		-9914
#define MIDDLE_ALIGN		-9915
#define TRUE_MIDDLE_ALIGN	-9916
#define BOTTOM_ALIGN		-9917
// graphicsMode
#define MCGA_MODE		21
#define VGA_MODE		22
#define SVGA1_MODE		23
#define SVGA2_MODE		24
#define SVGA3_MODE		25
#define SVGA4_MODE		26
#define SVGA5_MODE		27
#define SVGA6_MODE		28
// 
#define NO_COLOUR		0xFFFF

// Used by MOUSEPOINT
#define LEFT_BUTTON		1
#define RIGHT_BUTTON		2
#define MIDDLE_BUTTON		4

#define BUTTON_UP		1
#define BUTTON_DOWN		2
#define BUTTON_UP_OR_DOWN	3

/*********/
/* Types */
/*********/
typedef struct
{
    BOOL	fullScreen;
    int		textRows;
    int		textCols;

    int		textFontSize;
    char	textFontName [PROPERTY_LENGTH];
    int		caretWidth;

    COLOR	desiredSelectionColour;

    // Debugging
    BOOL	logging;
    BOOL	logWindowMessages;
} MIOWinOnly_Properties, *MIOWinOnly_PropertiesPtr;


typedef struct
{
    MDIOWinTop_Properties	topProperties;
    MIOWinOnly_Properties	myProperties;
} MIOWin_Properties;


typedef struct
{
    int 	mode;
    int		rows, columns;
    int 	xSize, ySize;
    int		graphicsMode;
    int 	visibility;
    int		xor;
    int		displayOnScreen;
    int		cursor;
    int		echo;
    int		buttonBar;
    int		msdosCharSet;
    char	title [256];
    int		xPosition, yPosition;
    int		topMost;
} WindowAttrib, *WindowAttribPtr;


/**********************/
/* External variables */
/**********************/

/***********************/
/* External procedures */
/***********************/
extern void	MIOWin_Init (BOOL pmCenterOutputWindow, BOOL pmStopUserClose);
extern void	MIOWin_Finalize (void);
extern void	MIOWin_PropertiesSet (MIOWin_Properties pmProperties);
extern void	MIOWin_PropertiesImplementChanges (WIND pmWindow);

extern void	MIOWin_AddText (WIND pmWindow, char *pmBuffer, int pmFlag);
extern void	MIOWin_AssertInnerWindow (WIND pmWindow);
extern void	MIOWin_AssertOuterWindow (WIND pmWindow);
extern void	MIOWin_AssertWindowType (WIND pmWindow, int pmType);
extern void	MIOWin_CalculateInnerWindowSize (
		    WindowAttribPtr pmWindowAttribs,
		    int *pmMinWindowWidth, int *pmMinWindowHeight,
		    int *pmWidth, int *pmHeight, 
		    int *pmGraphicsRunWindowWidth, 
		    int *pmGraphicsRunWindowHeight,
		    int *pmMaxWindowWidth, int *pmMaxWindowHeight);
extern void	MIOWin_CalculateWindowDisplacement (WIND pmWindow,
				       	       WindowAttribPtr pmWindowAttribs);
extern void	MIOWin_CalculateWindowPosition (WIND pmWindow, int pmXPos, 
						int pmYPos, int *pmWindowLeft, 
						int *pmWindowTop);
extern void	MIOWin_CalculateWindowSize (WIND pmWindow,
				            WindowAttribPtr pmWindowAttribs);
extern void	MIOWin_CaretDisplay (WIND pmInnerOrOuterWindow);
extern void	MIOWin_CaretHide (WIND pmInnerOrOuterWindow);
extern void	MIOWin_CaretMove (WIND pmInnerOrOuterWindow);
extern BOOL	MIOWin_CheckForClose (WIND pmWindow);
extern void	MIOWin_CloseWindow (WIND pmWindow);
extern BOOL	MIOWin_CommandHandler (WIND pmWindow, int pmCommand);
extern BOOL	MIOWin_ConvertSetscreenAttribs (const char *pmAttributeString, 
						WindowAttribPtr pmAttribs,
					        BOOL pmInitialSetup);
extern WIND	MIOWin_CreateWindow (const char *pmWindowAttribString);
extern WIND	MIOWin_CreateWindowFromAttribs (WindowAttribPtr pmWindowAttribs);
extern void	MIOWin_Dispose (WIND pmWindow);
extern char	MIOWin_Getch (WIND pmWindow);
extern int	MIOWin_GetCharacter (WIND pmWindow, BOOL *pmEchoEOF);
extern MIOWinInfoPtr	MIOWin_GetInfo (WIND pmWindow);
extern MIOWinOnly_PropertiesPtr  MIOWin_GetProperties (void);
extern int	MIOWin_GetWindowType (WIND pmWindow);
extern BOOL	MIOWin_Hasch (WIND pmWindow);
extern int	MIOWin_InitializeWindow (WIND pmWindow, 
				    WindowAttribPtr pmWindowAttribs);
extern void	MIOWin_InitRun (void);
extern BOOL	MIOWin_IsInputAvailable (WIND pmWindow, int pmKind, 
					    int pmCount);
extern BOOL	MIOWin_IsTextWindow (WIND pmInnerOrOuterWindow);
extern void	MIOWin_KeystrokeHandler (WIND pmWindow, UCHAR pmKeystroke);
extern void	MIOWin_OutputText (WIND pmWindow, char *pmBuffer);
extern void	MIOWin_SaveWindowToFile (WIND pmWindow, const char *pmPathName);
extern void	MIOWin_SetRunWindowTitle (WIND pmWindow);
extern void	MIOWin_UngetCharacter (OOTint pmChar, WIND pmWindow);

#endif // #ifndef _MIOWIN_H_
