/************************************************************************/
/* miowin.c								*/
/*									*/
/* This contains the platform dependent routines that relate to generic	*/
/* run windows (that is both text and graphics run windows).		*/
/*									*/
/* miowindow - Handles platform independent routines that relate to 	*/
/*             generic run windows.					*/
/* miowintext - Handles platform independent routines that relate to 	*/
/*              text windows.						*/
/* miowingraph - Handles platform independent routines that relate to	*/
/*               graphics windows.					*/
/* mdiowin - Handles platform dependent routines that relate to 	*/
/*           generic run windows.					*/
/* mdiowintext - Handles platform dependent routines that relate to	*/
/*               text windows.						*/
/* mdiowingraph - Handles platform dependent routines that relate to	*/
/*                graphics windows.					*/
/************************************************************************/

/*******************/
/* System includes */
/*******************/
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/****************/
/* Self include */
/****************/
#include "miowin.h"

/******************/
/* Other includes */
/******************/
#include "mioerr.h"

#include "mio.h"
#include "mdio.h"

#include "miodialogs.h"

#include "mdiowin.h"
#include "miowintext.h"
#include "mdiowintext.h"
#include "miowingraph.h"
#include "mdiowingraph.h"
#include "miowindlg.h"
#include "mdiowindlg.h"

#include "levent.h"

#include "miocommands.h"

#include "edint.h"

#include "miotypes.h"

// Test to make certain we're not accidentally including <windows.h> which
// might allow for windows contamination of platform independent code.
#ifdef _INC_WINDOWS
xxx
#endif

/**********/
/* Macros */
/**********/
#define INC2(x)			(((x) + 2) % INPUT_BUFFER_SIZE)
//#define INC3(x)			(((x) + 3) % INPUT_BUFFER_SIZE)
#define DEC(x)			(((x) + INPUT_BUFFER_SIZE - 1) % INPUT_BUFFER_SIZE)

/*************/
/* Constants */
/*************/
// Keyboard characters
#define CTRL_D			 4
#define BACKSPACE		 8
#define TAB			 9
#define NEWLINE			10
#define RETURN			13
#define CTRL_U			21
#define CTRL_Z			26

#define MARGIN			 2    

// Minimum size of the window in rows and columns
#define MIN_TEXT_ROWS		 3
#define MIN_TEXT_COLS		20
#define MIN_GRAPHICS_ROWS	 2
#define MIN_GRAPHICS_COLS	10

// Maximum size of the drawing surface of a graphics window
#define MAX_WIDTH_IN_PIXELS	4096
#define MAX_HEIGHT_IN_PIXELS	4096

#define TITLE_HEIGHT		 MDIOWin_GetTitleBarHeight ()
#define MIN_HORZ_ON_SCREEN	 100

/********************/
/* Global variables */
/********************/

/*********/
/* Types */
/*********/

/**********************/
/* External variables */
/**********************/
extern void EdLog_Log();

/********************/
/* Static constants */
/********************/
char *stBadSizeSpecForText = "Bad specification of text window size";
char *stBadSizeSpecForScreen = "Bad specification of screen window size";
char *stBadSizeSpecForGraphics = "Bad specification of graphics window size";
char *stBadLocSpecForPosition = "Bad specification of window position";

/********************/
/* Static variables */
/********************/
// Properties of the text display window
static MIOWinOnly_Properties	stProperties;
static BOOL			stPropertiesChanged;

// Used to display the text
static int			stTextFontCharWidth, stTextFontCharHeight;
static int			stTextFontCharAscent, stTextFontCharDescent;
static int			stTextFontCharIntLeading;

static FONT			stNormalFont;

static COLOR			stSelectionColour;

static int			stLeftDisplacement, stTopDisplacement;

static BOOL			stCenterOutputWindow, stStopUserClose;

static int			stLastNonZeroZOrder = 1;

/******************************/
/* Static callback procedures */
/******************************/

/*********************/
/* Static procedures */
/*********************/
static void		MyAddChar (WIND pmWindow, char pmChar, int pmFlag);
static void		MyDeletePreviousChar (WIND pmWindow);
static void		MyDeleteText (WIND pmWindow, int pmRow, int pmCol);
static void		MyGetCurrentLineCol (WIND pmWindow, int *pmLine, 
					     int *pmCol);
static int		MyGetHDisplacement (WIND pmWindow);
static const char	*MyGetLine (const char *pmPtr, char *pmLine);
static const char	*MyGetPosition (const char *pmPtr, int *pmXPos, 
					int *pmYPos);
static const char	*MyGetSeparator (const char *pmPtr);
static const char	*MyGetValue (const char *pmPtr, int *pmValue);
static int		MyGetVDisplacement (WIND pmWindow);
static const char	*MyGetWord (const char *pmPtr, char *pmWord);
static BOOL		MyIsAtBeginningOfLine (WIND pmWindow);
static BOOL		MyIsCurrentPositionLastOutput (WIND pmWindow);
static BOOL		MyIsInputAvailable (WIND pmWindow, int pmKind, 
					    int pmCount);
static BOOL		MyIsNewLineInBuffer (WIND pmWindow);
static void		MyPaste (WIND pmWindow);
static BOOL		MyProcessFromKeyboardToLineBuffer (WIND pmWindow);
static const char	*MySkipWhiteSpace (const char *pmPtr);

/***********************/
/* External procedures */
/***********************/
/************************************************************************/
/* MIOWin_Init								*/
/* 									*/
/* Initialize the modules under EdWin and register the new class	*/
/************************************************************************/
void	MIOWin_Init (BOOL pmCenterOutputWindow, BOOL pmStopUserClose)
{
    MDIOWin_Init ();
    MIOWinText_Init ();
    MIOWinGraph_Init ();
    MIOWinDlg_Init ();
    
    MDIOWinTop_Init (pmStopUserClose);
    
    stCenterOutputWindow = pmCenterOutputWindow;
    stStopUserClose = pmStopUserClose;
} // MIOWin_Init


/************************************************************************/
/* MIOWin_Finalize							*/
/************************************************************************/
void	MIOWin_Finalize (void)
{
    MDIOWin_Finalize ();
    MIOWinText_Finalize ();
    MIOWinGraph_Finalize ();
    MIOWinDlg_Finalize ();

    MDIOWinTop_Finalize ();
} // MIOWin_Finalize


/************************************************************************/
/* MIOWin_PropertiesSet							*/
/************************************************************************/
void	MIOWin_PropertiesSet (MIOWin_Properties pmProperties)
{
    MDIOWinTop_PropertiesSet (pmProperties.topProperties);
    MIOWinText_PropertiesSet ();
    
    stProperties = pmProperties.myProperties;
    stPropertiesChanged = TRUE;
} // MIOWin_PropertiesSet

			  
/************************************************************************/
/* MIOWin_PropertiesImplementChanges					*/
/************************************************************************/
void	MIOWin_PropertiesImplementChanges (WIND pmWindow)
{
    if (stPropertiesChanged)
    {
    	// Make changes for the module
	MDIOWin_SetDisplayFonts (pmWindow, 
	    stProperties.textFontName, stProperties.textFontSize,
	    &stTextFontCharHeight, &stTextFontCharWidth,
	    &stTextFontCharAscent, &stTextFontCharDescent, 
	    &stTextFontCharIntLeading, &stNormalFont, 
	    stProperties.desiredSelectionColour, &stSelectionColour);
    	stPropertiesChanged = FALSE;
    }
} // MIOWin_PropertiesImplementChanges


/************************************************************************/
/* MIOWin_AddText							*/
/************************************************************************/
void	MIOWin_AddText (WIND pmWindow, char *pmBuffer, int pmFlag)
{
    MIOWin_AssertOuterWindow (pmWindow);
        
    if (MIOWin_GetInfo (pmWindow) -> visibility == VIS_POPUP)
    {
    	MDIOWin_ShowWindow (pmWindow);
    	MIOWin_GetInfo (pmWindow) -> visibility = VIS_VISIBLE;
    }
    
    if (MIOWin_IsTextWindow (pmWindow))
    {
	MIOWinText_AddText (pmWindow, pmBuffer, pmFlag);
    }
    else
    {
    	MIOWinGraph_AddText (pmWindow, pmBuffer, pmFlag);
    }    
} // MIOWin_AddText


/************************************************************************/
/* MIOWin_AssertInnerWindow						*/
/************************************************************************/
void	MIOWin_AssertInnerWindow (WIND pmWindow)
{
    int	myWindowType = MDIOWin_GetWindowType (pmWindow);
    if ((myWindowType != WINDOW_TYPE_MIO_TEXT_INNER) && 
        (myWindowType != WINDOW_TYPE_MIO_GRAPHICS_INNER))
    {
    	EdInt_HaltEnvironment (IDS_MIO_BADWINDOWTYPE, __FILE__, __LINE__, 0,
    			       myWindowType, WINDOW_TYPE_MIO_TEXT_INNER);
    }
} // MIOWin_AssertOuterWindow


/************************************************************************/
/* MIOWin_AssertOuterWindow						*/
/************************************************************************/
void	MIOWin_AssertOuterWindow (WIND pmWindow)
{
    int	myWindowType = MDIOWin_GetWindowType (pmWindow);
    if ((myWindowType != WINDOW_TYPE_MIO_TEXT) && 
        (myWindowType != WINDOW_TYPE_MIO_GRAPHICS) &&
	(myWindowType != WINDOW_TYPE_MIO_DIALOG))
    {
    	EdInt_HaltEnvironment (IDS_MIO_BADWINDOWTYPE, __FILE__, __LINE__, 0,
    			       myWindowType, WINDOW_TYPE_MIO_TEXT);
    }
} // MIOWin_AssertOuterWindow


/************************************************************************/
/* MIOWin_AssertWindowType						*/
/************************************************************************/
void	MIOWin_AssertWindowType (WIND pmWindow, int pmType)
{
    if (MDIOWin_GetWindowType (pmWindow) != pmType)
    {
    	EdInt_HaltEnvironment (IDS_MIO_BADWINDOWTYPE, __FILE__, __LINE__, 0,
    			       MDIOWin_GetWindowType (pmWindow), pmType);
    }
} // MIOWin_AssertWindowType


/************************************************************************/
/* MIOWin_ConvertSetscreenAttribs					*/
/************************************************************************/
BOOL	MIOWin_ConvertSetscreenAttribs (const char *pmAttributeString, 
				        WindowAttribPtr pmAttribs,
				        BOOL pmInitialSetup)
{
    const char	*myPtr;
    char	myWord [256];

    //
    // Initialize the attributes
    //
    pmAttribs -> mode = UNSET_VALUE;
    pmAttribs -> rows = UNSET_VALUE;
    pmAttribs -> columns = UNSET_VALUE;
    pmAttribs -> xSize = UNSET_VALUE;
    pmAttribs -> ySize = UNSET_VALUE;
    pmAttribs -> graphicsMode = UNSET_VALUE;
    
    if (pmInitialSetup)
    {
    	pmAttribs -> visibility = VIS_VISIBLE;
    	pmAttribs -> xor = FALSE;
    	pmAttribs -> displayOnScreen = TRUE;
    	pmAttribs -> cursor = TRUE;
    	pmAttribs -> echo = TRUE;
    	pmAttribs -> buttonBar = FALSE;
    	pmAttribs -> msdosCharSet = FALSE;
	pmAttribs -> topMost = 0;
    }
    else
    {
    	pmAttribs -> visibility = UNSET_VALUE;
    	pmAttribs -> xor = UNSET_VALUE;
    	pmAttribs -> displayOnScreen = UNSET_VALUE;
    	pmAttribs -> cursor = UNSET_VALUE;
    	pmAttribs -> echo = UNSET_VALUE;
    	pmAttribs -> buttonBar = UNSET_VALUE;
    	pmAttribs -> msdosCharSet = UNSET_VALUE;
	pmAttribs -> topMost = UNSET_VALUE;
    }
    pmAttribs -> title [0] = 0;
    pmAttribs -> xPosition = UNSET_VALUE;
    pmAttribs -> yPosition = UNSET_VALUE;

    //
    // Loop through the string
    //
    
    // Must be one of:	text, screen, graphics, text:, screen:, graphics:, 
    //			visible, invisible, popup, noxor, xor, nooffscreenonly,
    //			offscreenonly, nocursor, 
    //			cursor, noecho, echo, buttonbar, nobuttonbar,
    //			title:, position:
    myPtr = MySkipWhiteSpace (pmAttributeString);

    while (*myPtr != 0)
    {
	// Read an option
   	myPtr = MyGetWord (myPtr, myWord);
    	if (myPtr == NULL)
	{
	    SET_ERRNO(E_WINDOW_SET_BAD_OPTION);
	    return FALSE;
	}

	// If no more options, quit loop
	if (myWord [0] == 0)
	{
	    break;
	}

    	if (strcmp (myWord, "text") == 0)
    	{
	    pmAttribs -> mode = TEXT_MODE;
	    pmAttribs -> rows = UNSET_VALUE;
	    pmAttribs -> columns = UNSET_VALUE;
	    pmAttribs -> xSize = UNSET_VALUE;
	    pmAttribs -> ySize = UNSET_VALUE;
	}
    	else if (strcmp (myWord, "screen") == 0)
    	{
	    pmAttribs -> mode = GRAPHICS_MODE;
	    pmAttribs -> rows = UNSET_VALUE;
	    pmAttribs -> columns = UNSET_VALUE;
	    pmAttribs -> xSize = UNSET_VALUE;
	    pmAttribs -> ySize = UNSET_VALUE;
	}
    	else if (strcmp (myWord, "graphics") == 0)
    	{
	    pmAttribs -> mode = GRAPHICS_MODE;
	    pmAttribs -> rows = UNSET_VALUE;
	    pmAttribs -> columns = UNSET_VALUE;
	    pmAttribs -> xSize = UNSET_VALUE;
	    pmAttribs -> ySize = UNSET_VALUE;
	}
    	else if (strcmp (myWord, "text:") == 0)
    	{
	    pmAttribs -> mode = TEXT_MODE;
	    pmAttribs -> xSize = UNSET_VALUE;
	    pmAttribs -> ySize = UNSET_VALUE;
	    // Read rows
    	    myPtr = MyGetValue (myPtr, &pmAttribs -> rows);
	    if (myPtr == NULL)
	    {
		SET_ERRMSG(E_WINDOW_SET_BAD_OPTION, stBadSizeSpecForText);
		return FALSE;
	    }
	    // Read ";"
	    myPtr = MyGetSeparator (myPtr);
	    if (myPtr == NULL)
	    {
		SET_ERRMSG(E_WINDOW_SET_BAD_OPTION, stBadSizeSpecForText);
		return FALSE;
	    }
	    // Read columns
    	    myPtr = MyGetValue (myPtr, &pmAttribs -> columns);
	    if (myPtr == NULL)
	    {
		SET_ERRMSG(E_WINDOW_SET_BAD_OPTION, stBadSizeSpecForText);
		return FALSE;
	    }
	}
    	else if (strcmp (myWord, "screen:") == 0)
    	{
	    pmAttribs -> mode = GRAPHICS_MODE;
	    pmAttribs -> xSize = UNSET_VALUE;
	    pmAttribs -> ySize = UNSET_VALUE;
	    // Read rows
    	    myPtr = MyGetValue (myPtr, &pmAttribs -> rows);
	    if (myPtr == NULL)
	    {
		SET_ERRMSG(E_WINDOW_SET_BAD_OPTION, 
							stBadSizeSpecForScreen);
		return FALSE;
	    }
	    // Read ";"
	    myPtr = MyGetSeparator (myPtr);
	    if (myPtr == NULL)
	    {
		SET_ERRMSG(E_WINDOW_SET_BAD_OPTION, 
							stBadSizeSpecForScreen);
		return FALSE;
	    }
	    // Read columns
    	    myPtr = MyGetValue (myPtr, &pmAttribs -> columns);
	    if (myPtr == NULL)
	    {
		SET_ERRMSG(E_WINDOW_SET_BAD_OPTION, 
							stBadSizeSpecForScreen);
		return FALSE;
	    }
	}
    	else if (strcmp (myWord, "graphics:") == 0)
    	{
	    pmAttribs -> mode = GRAPHICS_MODE;
	    pmAttribs -> rows = UNSET_VALUE;
	    pmAttribs -> columns = UNSET_VALUE;
	    // Read x size
    	    myPtr = MyGetValue (myPtr, &pmAttribs -> xSize);
	    if (myPtr == NULL)
	    {
		SET_ERRMSG(E_WINDOW_SET_BAD_OPTION, 
						stBadSizeSpecForGraphics);
		return FALSE;
	    }
	    // Read ";"
	    myPtr = MyGetSeparator (myPtr);
	    if (myPtr == NULL)
	    {
		SET_ERRMSG(E_WINDOW_SET_BAD_OPTION, 
						stBadSizeSpecForGraphics);
		return FALSE;
	    }
	    // Read y size
    	    myPtr = MyGetValue (myPtr, &pmAttribs -> ySize);
	    if (myPtr == NULL)
	    {
		SET_ERRMSG(E_WINDOW_SET_BAD_OPTION, 
						stBadSizeSpecForGraphics);
		return FALSE;
	    }
	}
    	else if (strcmp (myWord, "visible") == 0)
    	{
	    pmAttribs -> visibility = VIS_VISIBLE;
	}
    	else if (strcmp (myWord, "invisible") == 0)
    	{
	    pmAttribs -> visibility = VIS_INVISIBLE;
	}
    	else if (strcmp (myWord, "popup") == 0)
    	{
	    pmAttribs -> visibility = VIS_POPUP;
	}
    	else if (strcmp (myWord, "noxor") == 0)
    	{
	    pmAttribs -> xor = 0;
	}
    	else if (strcmp (myWord, "xor") == 0)
    	{
	    pmAttribs -> xor = 1;
	}
    	else if (strcmp (myWord, "nooffscreenonly") == 0)
    	{
	    pmAttribs -> displayOnScreen = 1;
	}
    	else if (strcmp (myWord, "offscreenonly") == 0)
    	{
	    pmAttribs -> displayOnScreen = 0;
	}
    	else if (strcmp (myWord, "nocursor") == 0)
    	{
	    pmAttribs -> cursor = 0;
	}
    	else if (strcmp (myWord, "cursor") == 0)
    	{
	    pmAttribs -> cursor = 1;
	}
    	else if (strcmp (myWord, "noecho") == 0)
    	{
	    pmAttribs -> echo = 0;
	}
    	else if (strcmp (myWord, "echo") == 0)
    	{
	    pmAttribs -> echo = 1;
	}
    	else if (strcmp (myWord, "nobuttonbar") == 0)
    	{
	    pmAttribs -> buttonBar = 0;
	}
    	else if (strcmp (myWord, "buttonbar") == 0)
    	{
	    pmAttribs -> buttonBar = 1;
	}
    	else if (strcmp (myWord, "msdos") == 0)
    	{
	    pmAttribs -> msdosCharSet = 1;
	}
    	else if (strcmp (myWord, "nomsdos") == 0)
    	{
	    pmAttribs -> msdosCharSet = 0;
	}
    	else if (strcmp (myWord, "topmost") == 0)
    	{
	    pmAttribs -> topMost = stLastNonZeroZOrder++;
	}
    	else if (strcmp (myWord, "notopmost") == 0)
    	{
	    pmAttribs -> topMost = 0;
	}
    	else if (strcmp (myWord, "title:") == 0)
    	{
	    myPtr = MyGetLine (myPtr, pmAttribs -> title);
	    if (myPtr == NULL)
	    {
	    	pmAttribs -> title [0] = 0;
	    }	    	
	}
    	else if (strcmp (myWord, "position:") == 0)
    	{
	    // Read x position
	    myPtr = MyGetPosition (myPtr, &pmAttribs -> xPosition, 
				   &pmAttribs -> yPosition);
	    if (myPtr == NULL)
	    {
		SET_ERRMSG(E_WINDOW_SET_BAD_OPTION, 
						stBadLocSpecForPosition);
		return FALSE;
	    }
	    // Read ";"
	    myPtr = MyGetSeparator (myPtr);
	    if (myPtr == NULL)
	    {
		SET_ERRMSG(E_WINDOW_SET_BAD_OPTION, 
						stBadLocSpecForPosition);
		return FALSE;
	    }
	    // Read y position
	    myPtr = MyGetPosition (myPtr, &pmAttribs -> xPosition, 
				   &pmAttribs -> yPosition);
	    if (myPtr == NULL)
	    {
		SET_ERRMSG(E_WINDOW_SET_BAD_OPTION, 
						stBadLocSpecForPosition);
		return FALSE;
	    }
	    
	    // Make certain both x and y position are set
	    if ((pmAttribs -> xPosition == UNSET_VALUE) ||
	    	(pmAttribs -> yPosition == UNSET_VALUE))
	    {
		SET_ERRMSG(E_WINDOW_SET_BAD_OPTION, 
						stBadLocSpecForPosition);
		return FALSE;
	    }
	}
	else
	{
	    char    myMessage [1024];

	    strcpy (myMessage, "Unknown set window option \"");
	    strcat (myMessage, myWord);
	    strcat (myMessage, "\"");
	    SET_ERRMSG(E_WINDOW_SET_BAD_OPTION, myMessage);
	    return FALSE;
	}

	// Read "," between elements
	if (*myPtr != 0) myPtr++;		// Skip separator

	myPtr = MySkipWhiteSpace (myPtr);
    } // while
    return TRUE;
} // MIO_ConvertSetscreenAttribs

				  
/************************************************************************/
/* MIOWin_CalculateInnerWindowSize					*/
/*									*/
/* Sets the appropriate fields in RunWindowInfo to account for a window	*/
/* of the specified size.						*/
/* The size can be specified as either					*/
/* 	(pmRows, pmCols) - The window is so many rows and columns	*/
/*      (pmRows = 0, pmCols = 0) (pmWidth, pmHeight) - The window is so	*/
/*           many pixels in size.					*/
/*      (A value of -1 represents the maximum size for the window).	*/
/************************************************************************/
void	MIOWin_CalculateInnerWindowSize (WindowAttribPtr pmWindowAttribs,
		int *pmMinWindowWidth, int *pmMinWindowHeight,
		int *pmWidth, int *pmHeight, 
		int *pmGraphicsRunWindowWidth, int *pmGraphicsRunWindowHeight,
		int *pmMaxWindowWidth, int *pmMaxWindowHeight)
{
    int			myVMarginSize, myHMarginSize;
    int			myMinRows, myMinCols;
    MYRECT		myRect;
    int			myMinWidth, myMinHeight;
    MYRECT		myScreenRect;
    int			myMaxWidth, myMaxHeight;

    //
    // Calculate margins
    //
    if (pmWindowAttribs -> mode == TEXT_MODE)
    {
    	// Margins only on left side, scroll bars on right
        myHMarginSize = MARGIN + MDIOWin_GetVScrollBarWidth ();		
    	// Margins only on top side, scroll bars on bottom
        myVMarginSize = MARGIN + MDIOWin_GetHScrollBarWidth ();		
        myMinRows = MIN_TEXT_ROWS;
        myMinCols = MIN_TEXT_COLS;
    }
    else
    {
        myHMarginSize = 2 * MARGIN;	// Margins only on left & right side
        myVMarginSize = 2 * MARGIN;	// Margins only on top & bottom side
        myMinRows = MIN_GRAPHICS_ROWS;
        myMinCols = MIN_GRAPHICS_COLS;
    }

    if (pmWindowAttribs -> buttonBar == 1)
    {
        myVMarginSize += MDIOWinTop_GetWindowHeight ();
    }
    
    //
    // Calculate the size of the minimum outer window size
    //
     
    // Calculate the minimum width/height of the internal window
    myMinWidth = stTextFontCharWidth * myMinCols + myHMarginSize;
    myMinHeight = stTextFontCharHeight * myMinRows + myVMarginSize;

    // Calculate the minimum size of an outer window containing the inner window
    myRect.left = 0;
    myRect.top = 0;
    myRect.right = myMinWidth;
    myRect.bottom = myMinHeight;
    MDIOWin_CalculateWindowRect (&myRect);
    
    // Fill in the minimum width and height of the outer run window
    if (pmMinWindowWidth != NULL)
    {
    	*pmMinWindowWidth = myRect.right - myRect.left;
    	*pmMinWindowHeight = myRect.bottom - myRect.top;
    }
    
    //
    // Calculate the maximum inner width/height that fits on the screen.
    //
    myRect.left = 0;
    myRect.top = 0;
    myRect.right = 100;
    myRect.bottom = 100;
    MDIOWin_CalculateWindowRect (&myRect);

    myScreenRect = EdInt_GetWorkArea (); 		   	
    myMaxWidth = (myScreenRect.right - myScreenRect.left) - 
    		 (myRect.right - myRect.left - 100) - myHMarginSize;
    myMaxHeight = (myScreenRect.bottom - myScreenRect.top) - 
    		 (myRect.bottom - myRect.top - 100) - myVMarginSize;

    //
    // Calculate the specified width/height for the inner window
    //
    myRect.left = 0;
    myRect.top = 0;

    // Use rows/column values, if set
    if (pmWindowAttribs -> rows != UNSET_VALUE)
    {
    	// Set the screen size based on the rows and columns
    	if (pmWindowAttribs -> columns == MAX_SIZE)
    	{
    	    myRect.right = myMaxWidth;
    	}
    	else
    	{
            myRect.right = stTextFontCharWidth * pmWindowAttribs -> columns;
    	}
    	
    	if (pmWindowAttribs -> rows == MAX_SIZE)
    	{
    	    myRect.bottom = myMaxHeight;
    	}
    	else
    	{
    	    myRect.bottom = stTextFontCharHeight * pmWindowAttribs -> rows;
	}    		
    }
    else if (pmWindowAttribs -> xSize != UNSET_VALUE)
    {
    	// Set the screen size based on the pixel dimension specified
    	if (pmWindowAttribs -> xSize == MAX_SIZE)
    	{
    	    myRect.right = myMaxWidth;
    	}
    	else
    	{
            myRect.right = pmWindowAttribs -> xSize;
    	}
    	
    	if (pmWindowAttribs -> ySize == MAX_SIZE)
    	{
    	    myRect.bottom = myMaxHeight;
    	}
    	else
    	{
    	    myRect.bottom = pmWindowAttribs -> ySize;
	}    		
    }
    else
    {
        myRect.right = stTextFontCharWidth * stProperties.textCols;
    	myRect.bottom = stTextFontCharHeight * stProperties.textRows;
    }
    
    myRect.right = MIN (MAX (myRect.right, myMinWidth), MAX_WIDTH_IN_PIXELS);
    myRect.bottom = MIN (MAX (myRect.bottom, myMinHeight), MAX_HEIGHT_IN_PIXELS);

    // Fill in the width and height of the graphics area
    if (pmWidth != NULL)
    {
    	*pmWidth = myRect.right;
    	*pmHeight = myRect.bottom;
    }
    
    myRect.right += myHMarginSize;
    myRect.bottom += myVMarginSize;

    // Fill in the width and hight of the inner window
    if (pmGraphicsRunWindowWidth != NULL)
    {
    	*pmGraphicsRunWindowWidth = myRect.right;
    	*pmGraphicsRunWindowHeight = myRect.bottom;
    	if (pmWindowAttribs -> buttonBar == 1)
    	{
            *pmGraphicsRunWindowHeight -= MDIOWinTop_GetWindowHeight ();
    	}
    }
    
    MDIOWin_CalculateWindowRect (&myRect);
    
    // Fill in the width and height of the run window
    if (pmMaxWindowWidth != NULL)
    {
    	*pmMaxWindowWidth = MIN (myRect.right - myRect.left,
	    myScreenRect.right - myScreenRect.left);
    	*pmMaxWindowHeight = MIN (myRect.bottom - myRect.top,
	    myScreenRect.bottom - myScreenRect.top);
    }
} // MIOWin_CalculateInnerWindowSize


/************************************************************************/
/* MIOWin_CalculateWindowDisplacement					*/
/************************************************************************/
void	MIOWin_CalculateWindowDisplacement (WIND pmWindow,
				       	    WindowAttribPtr pmWindowAttribs)
{
    MIOWinInfoPtr	myInfo;

    MIOWin_AssertOuterWindow (pmWindow);
    
    myInfo = MIOWin_GetInfo (pmWindow);
    
    MIOWin_CalculateWindowPosition (pmWindow, pmWindowAttribs -> xPosition,
    	pmWindowAttribs -> yPosition, &myInfo -> initWindowLeft,
    	&myInfo -> initWindowTop);
} // MIOWin_CalculateWindowDisplacement


/************************************************************************/
/* MIOWin_CalculateWindowPosition					*/
/************************************************************************/
void	MIOWin_CalculateWindowPosition (WIND pmWindow, int pmXPos, int pmYPos,
					int *pmWindowLeft, int *pmWindowTop)
{
    MIOWinInfoPtr	myInfo;
    MYRECT		myScreenRect;
    int			myScreenWidth, myScreenHeight;
    int			myLeft, myTop;
        
    MIOWin_AssertOuterWindow (pmWindow);
    
    myInfo = MIOWin_GetInfo (pmWindow);
    
    // Calculate the top, left corner of the window
    myScreenRect = EdInt_GetWorkArea (); 		   	
    myScreenWidth = myScreenRect.right - myScreenRect.left;
    myScreenHeight = myScreenRect.bottom - myScreenRect.top;
    
    if (pmXPos == UNSET_VALUE)
    {
    	if (stCenterOutputWindow)
    	{
    	    // Place the window in the upper centre of the screen, displacing 
    	    // each one down by DISPLACEMENT_VALUE.  Don't let any part of the 
    	    // window go off the screen.
    	    myLeft = (myScreenWidth - myInfo -> maxWindowWidth) / 2 + 
    		     myScreenRect.left;
    	    if (myLeft < myScreenRect.left)
    	    {
    	    	myLeft = 0;
    	    }
    	    if (stTopDisplacement == -1)
    	    {
	    	myTop = (myScreenHeight - myInfo -> maxWindowHeight) / 3 + 
    		        myScreenRect.top;
		stTopDisplacement = myTop;
	    }
	    else
	    {
	    	myTop = stTopDisplacement;
	    }
    	    if (myTop + myInfo -> maxWindowHeight > myScreenRect.bottom)
    	    {
    	    	stTopDisplacement = myScreenRect.top;
    	    	myTop = myScreenRect.top;
    	    }
    	    else
    	    {
    	    	stTopDisplacement += MDIOWin_GetTitleBarHeight ();
    	    }
	}
	else
	{
    	    // Place the window in the upper right corner, displacing each one
    	    // down and left by DISPLACEMENT_VALUE.  Don't let any part of the 
    	    // window go off the screen.
    	    myLeft = myScreenRect.right - myInfo -> maxWindowWidth - 
    		     stLeftDisplacement;
    	    if (myLeft < myScreenRect.left)
    	    {
    	    	stLeftDisplacement = 0;
    	    	myLeft = myScreenRect.right - myInfo -> maxWindowWidth;
    	    }
    	    else
    	    {
    	    	stLeftDisplacement += MDIOWin_GetTitleBarHeight ();
    	    }
    	    myTop = myScreenRect.top + stTopDisplacement;
    	    if (myTop + myInfo -> maxWindowHeight > myScreenRect.bottom)
    	    {
    	    	stTopDisplacement = 0;
    	    	myTop = myScreenRect.top;
    	    }
    	    else
    	    {
    	    	stTopDisplacement += MDIOWin_GetTitleBarHeight ();
    	    }
    	}
    	*pmWindowLeft = myLeft;
    	*pmWindowTop = myTop;
    	return;
    }
    
    if (pmXPos == LEFT_ALIGN)
    {
    	myLeft = myScreenRect.left;
    }
    else if ((pmXPos == CENTRE_ALIGN) || (pmXPos == MIDDLE_ALIGN))
    {
    	myLeft = (myScreenWidth - myInfo -> maxWindowWidth) / 2 + 
    		 myScreenRect.left;
    }
    else if (pmXPos == RIGHT_ALIGN)
    {
    	myLeft = myScreenRect.right - myInfo -> maxWindowWidth;
    }
    // Make certain it's on the screen
    else if (pmXPos + myInfo -> maxWindowWidth < MIN_HORZ_ON_SCREEN)
    {    	     
    	myLeft = myScreenRect.left + MIN_HORZ_ON_SCREEN - 
		 myInfo -> maxWindowWidth;
    }
    // Make certain it's on the screen
    else if (pmXPos + myScreenRect.left > 
	     myScreenRect.right - MIN_HORZ_ON_SCREEN)
    {    	     
    	myLeft = myScreenRect.right - MIN_HORZ_ON_SCREEN;
    }
    else
    {
    	myLeft = pmXPos + myScreenRect.left;
    }	
    
    if (pmYPos == TOP_ALIGN)
    {
    	myTop = myScreenRect.top;
    }
    else if (pmYPos == MIDDLE_ALIGN)
    {
    	myTop = (myScreenHeight - myInfo -> maxWindowHeight) / 3 + 
    		 myScreenRect.top;
    }
    else if ((pmYPos == TRUE_MIDDLE_ALIGN) || (pmYPos == CENTRE_ALIGN))
    {
    	myTop = (myScreenHeight - myInfo -> maxWindowHeight) / 2 + 
    		 myScreenRect.top;
    }
    else if (pmYPos == BOTTOM_ALIGN)
    {
    	myTop = myScreenRect.bottom - myInfo -> maxWindowHeight;
    }
    else
    {
	//
	// This is a numeric value that represents the bottom left 
	// corner of the window on a screen with a bottom left origin!
	//

	// Transform pmYPos into the actual location on the screen
	pmYPos = (myScreenRect.bottom - myScreenRect.top) - pmYPos + 
		 myScreenRect.top - myInfo -> maxWindowHeight;

	// Make certain it's on the screen
        if (pmYPos - TITLE_HEIGHT > myScreenRect.bottom)
	{    	     
    	    myTop = myScreenRect.bottom - TITLE_HEIGHT;
	}
	// Make certain it's on the screen
	else if (pmYPos < myScreenRect.top)
	{    	     
    	    myTop = myScreenRect.top;
	}
	else
	{
	    myTop = pmYPos;
	}
    }

    // Set the initial placement of the window
    *pmWindowLeft = myLeft;
    *pmWindowTop = myTop;
} // MIOWin_CalculateWindowPosition


/************************************************************************/
/* MIOWin_CalculateWindowSize						*/
/*									*/
/* Sets the appropriate fields in RunWindowInfo to account for a window	*/
/* of the specified size.						*/
/* The size can be specified as either					*/
/* 	(pmRows, pmCols) - The window is so many rows and columns	*/
/*      (pmRows = 0, pmCols = 0) (pmWidth, pmHeight) - The window is so	*/
/*           many pixels in size.					*/
/*      (A value of -1 represents the maximum size for the window).	*/
/************************************************************************/
void	MIOWin_CalculateWindowSize (WIND pmWindow,
				    WindowAttribPtr pmWindowAttribs)
{
    MIOWinInfoPtr	myInfo;
    
    MIOWin_AssertOuterWindow (pmWindow);
    
    myInfo = MIOWin_GetInfo (pmWindow);

    MIOWin_CalculateInnerWindowSize (pmWindowAttribs,
    	&myInfo -> minWindowWidth, &myInfo -> minWindowHeight,
    	&myInfo -> width, &myInfo -> height,
    	&myInfo -> graphicsRunWindowWidth, &myInfo -> graphicsRunWindowHeight,
    	&myInfo -> maxWindowWidth, &myInfo -> maxWindowHeight);
    	
    // Fill in the maximum number of rows and columns
    myInfo -> maxRow = myInfo -> height / stTextFontCharHeight;
    myInfo -> maxCol = myInfo -> width / stTextFontCharWidth;
} // MIOWin_CalculateWindowSize


/************************************************************************/
/* MIOWin_CaretDisplay							*/
/*									*/
/* This routine has no effect unless it is passed the window that	*/
/* currently owns the caret.  Then it will place the caret correctly if	*/
/* the window is accepting input.  If the caret was already visible, it	*/
/* will move the caret to the appropriate location.			*/
/************************************************************************/
void	MIOWin_CaretDisplay (WIND pmInnerOrOuterWindow)
{
    MIOWinInfoPtr	myInfo = MIOWin_GetInfo (pmInnerOrOuterWindow);
    	
    if (myInfo -> innerWindow != MIO_caretOwner) return;

    if (myInfo -> acceptingInput && myInfo -> cursor && !MIO_paused && 
	!MIO_finished)
    {
        int	myLine, myCol, myX, myY;

	// Get the position of the text input position (at end or one
	// from the end if a new line has been added)
	MyGetCurrentLineCol (myInfo -> outerWindow, &myLine, &myCol);
	
        myX = myCol * stTextFontCharWidth - 
              MyGetHDisplacement (pmInnerOrOuterWindow) + 
              myInfo -> textRect.left;
        myY = myLine * stTextFontCharHeight -
              MyGetVDisplacement (pmInnerOrOuterWindow) + 
              myInfo -> textRect.top;
	if ((myInfo -> windowKind == TEXT_MODE) &&
	    (stProperties.caretWidth == 2))
	{
	    myX--;
	}
        MDIOWin_CaretMove (myInfo -> innerWindow, myX, myY);
        if (!myInfo -> caretShowing)
        {
            myInfo -> caretShowing = TRUE;
	    MDIOWin_CaretShow (myInfo -> innerWindow);
        }
    }
    else
    {
    	if (myInfo -> caretShowing)
    	{
            myInfo -> caretShowing = FALSE;
    	    MDIOWin_CaretHide (myInfo -> innerWindow);
    	}
    }
} // MIOWin_CaretDisplay


/************************************************************************/
/* MIOWin_CaretHide							*/
/************************************************************************/
void	MIOWin_CaretHide (WIND pmInnerOrOuterWindow)
{
    MIOWinInfoPtr	myInfo;
            
    // Get the text display window information
    myInfo = MIOWin_GetInfo (pmInnerOrOuterWindow);
    	
    if (myInfo -> innerWindow != MIO_caretOwner) return;

    if (myInfo -> caretShowing)
    {
        myInfo -> caretShowing = FALSE;
    	MDIOWin_CaretHide (myInfo -> innerWindow);
    }
} // MIOWin_CaretHide


/************************************************************************/
/* MIOWin_CaretMove							*/
/************************************************************************/
void	MIOWin_CaretMove (WIND pmInnerOrOuterWindow)
{
    MIOWinInfoPtr	myInfo;
    int			myLine, myCol, myX, myY;
            
    // Create the ddit window information
    myInfo = MIOWin_GetInfo (pmInnerOrOuterWindow);
            	
    // Get the position of the text input position (at end or one
    // from the end if a new line has been added)
    MyGetCurrentLineCol (myInfo -> outerWindow, &myLine, &myCol);

    myX = myCol * stTextFontCharWidth - 
          MyGetHDisplacement (pmInnerOrOuterWindow) + myInfo -> textRect.left;
    myY = myLine * stTextFontCharHeight -
          MyGetVDisplacement (pmInnerOrOuterWindow) + myInfo -> textRect.top;
    if ((myInfo -> windowKind == TEXT_MODE) &&
	(stProperties.caretWidth == 2))
    {
        myX--;
    }
    MDIOWin_CaretMove (myInfo -> innerWindow, myX, myY);
} // MIOWin_CaretMove


/************************************************************************/
/* MIOWin_CheckForClose							*/
/************************************************************************/
BOOL	MIOWin_CheckForClose (WIND pmWindow)
{
    MIOWinInfoPtr	myInfo;
            
    // Check if a program is running and if it is, give a message
    myInfo = MIOWin_GetInfo (pmWindow);
    
    if (stStopUserClose)
    {
        if (MIO_finished || myInfo -> okayToClose)
        {
    	    return TRUE;
        }
        MDIO_Beep ();
        return FALSE;
    }
    	
    if (MIO_paused || MIO_finished || myInfo -> okayToClose)
    {
    	return TRUE;
    }

    return MDIOWin_CheckForCloseDialog (pmWindow);
} // MIOWin_CheckForClose


/************************************************************************/
/* MIOWin_CloseWindow							*/
/************************************************************************/
void	MIOWin_CloseWindow (WIND pmWindow)
{
    MIOWin_AssertOuterWindow (pmWindow);
    
    // If the current window is currently being used for output, then the
    // last activated window should now become the current window.
    if (pmWindow == MIO_selectedRunWindow)
    {
    	MIO_SetNextWindowActive (pmWindow);
    }
    
    MDIOWin_DestroyWindow (pmWindow);
} // MIOWin_CloseWindow


/************************************************************************/
/* MIOWin_CommandHandler						*/
/************************************************************************/
BOOL	MIOWin_CommandHandler (WIND pmWindow, int pmCommand)
{
    MIOWinInfoPtr	myInfo;
    
    MIOWin_AssertOuterWindow (pmWindow);
    
    myInfo = MIOWin_GetInfo (pmWindow);
    
    switch (pmCommand)
    {
	case MIO_COMMAND_PAUSE_RESUME:
	    EdInt_AddFailMessage ("CMD: MIO_COMMAND_PAUSE_RESUME");
	    if (MIO_paused)
	    {
	    	EdInt_ResumeExecution ();
	    }
	    else
	    {
	    	EdInt_PauseExecution ();
	    }
	    break;
	case MIO_COMMAND_RERUN:
	    EdInt_AddFailMessage ("CMD: MIO_COMMAND_RERUN");
            EdInt_RerunProgram ();
	    break;
	case MIO_COMMAND_TERMINATE:
	    // Called when the program is executing and the user clicks
	    // "Stop" on the button bar.
	    EdInt_AddFailMessage ("CMD: MIO_COMMAND_TERMINATE");
	    EdInt_StopExecution ();
	    break;
	case MIO_COMMAND_PRINT_FILE:
	    EdInt_AddFailMessage ("CMD: MIO_COMMAND_PRINT_FILE");
	    if (MIOWin_IsTextWindow (pmWindow))
	    {
		MIOWinText_PrintWindow (pmWindow);
	    }
	    else
	    {
	    	MIOWinGraph_PrintWindow (pmWindow);
	    }
	    break;
	case MIO_COMMAND_SAVEAS_FILE:
	    EdInt_AddFailMessage ("CMD: MIO_COMMAND_SAVEAS_FILE");
	    MIOWin_SaveWindowToFile (pmWindow, NULL);
	    break;
	case MIO_COMMAND_COPY:
	    EdInt_AddFailMessage ("CMD: MIO_COMMAND_COPY");
	    if (MIOWin_IsTextWindow (pmWindow))
	    {
		MIOWinText_Copy (pmWindow);
	    }
	    else
	    {
		return FALSE;
	    }
	    break;
	case MIO_COMMAND_PASTE:
	    EdInt_AddFailMessage ("CMD: MIO_COMMAND_PASTE");
	    MyPaste (pmWindow);
	    break;
	case MIO_COMMAND_SELECT_ALL:
	    EdInt_AddFailMessage ("CMD: MIO_COMMAND_SELECT_ALL");
	    if (MIOWin_IsTextWindow (pmWindow))
	    {
	        MIOWinText_SelectAll (pmWindow);
	    }
	    else
	    {
		return FALSE;
	    }
	    break;
	case MIO_COMMAND_CLOSE:
	    EdInt_AddFailMessage ("CMD: MIO_COMMAND_CLOSE");
	    MIOWin_CloseWindow (pmWindow);
	    break;	    
	case MIO_COMMAND_EXIT:
	    EdInt_AddFailMessage ("CMD: MIO_COMMAND_EXIT");
	    EdInt_Exit ();
    	    break;
	case MIO_COMMAND_NEW_EDITOR_WIN:
	    EdInt_AddFailMessage ("CMD: MIO_COMMAND_NEW_EDITOR_WIN");
	    EdInt_NewEditorWindow ();
	    break;
	case MIO_COMMAND_OPEN_EDITOR_WIN:
	    EdInt_AddFailMessage ("CMD: MIO_COMMAND_OPEN_EDITOR_WIN");
	    EdInt_OpenEditorFile ();
	    break;
	case MIO_COMMAND_CLOSE_ALL:
	    EdInt_AddFailMessage ("CMD: MIO_COMMAND_CLOSE_ALL");
	    MIO_CloseAllRunWindows ();
	    break;
    	default:
    	    return FALSE;
    }
    
    return TRUE;
} // MIOWin_CommandHandler


/************************************************************************/
/* MIOWin_CreateWindow							*/
/************************************************************************/
WIND	MIOWin_CreateWindow (const char *pmAttributeString)
{
    WindowAttrib	myWindowAttribs;
    
    MIOWin_ConvertSetscreenAttribs (pmAttributeString, &myWindowAttribs,
    				    INITIAL_SETUP);
    return MIOWin_CreateWindowFromAttribs (&myWindowAttribs);
} // MIOWin_CreateWindow


/************************************************************************/
/* MIOWin_CreateWindowFromAttribs					*/
/************************************************************************/
WIND	MIOWin_CreateWindowFromAttribs (WindowAttribPtr pmWindowAttribs)
{
    WIND		myWindow;
    
    if (pmWindowAttribs -> title [0] == 0)
    {
    	EdInt_AddFailMessage ("MIOWin_CreateWindowFromAttribs (No Title)");
    	strcpy (pmWindowAttribs -> title, MIO_programName);
    	strcat (pmWindowAttribs -> title, " Run Window");
    }
    else
    {
    	EdInt_AddFailMessage ("MIOWin_CreateWindowFromAttribs (%s)", 
    	    pmWindowAttribs -> title);
    }

    if (pmWindowAttribs -> mode == TEXT_MODE)
    {
    	myWindow = MIOWinText_CreateWindow (pmWindowAttribs);
    }
    else
    {
    	myWindow = MIOWinGraph_CreateWindow (pmWindowAttribs);
    }
    
    // TW Check for NULL
    
    if (pmWindowAttribs -> visibility == VIS_VISIBLE)
    {
    	MDIOWin_ShowWindow (myWindow);
    }
    
    return myWindow;
} // MIOWin_CreateWindowFromAttribs


/************************************************************************/
/* MIOWin_Dispose							*/
/************************************************************************/
void	MIOWin_Dispose (WIND pmWindow)
{
    MIOWinInfoPtr	myInfo = MIOWin_GetInfo (pmWindow);
    MYRECTLIST		*myPtr, *myLastPtr;

    MDIOWin_Dispose (myInfo);
    
    MDIOWin_SetWindowInfo (pmWindow, 0);
    
    // Free the dirty rect list, if there is one.
    myPtr = myInfo -> dirtyRectHead;
    while (myPtr != NULL)
    {
	myLastPtr = myPtr;
	myPtr = myPtr -> next;
	free (myLastPtr);
    }

    // Free the actual data structure
    free (myInfo);
} // MIOWin_Dispose


/************************************************************************/
/* MIOWin_Getch								*/
/************************************************************************/
char	MIOWin_Getch (WIND pmWindow)
{
    MIOWinInfoPtr	myInfo;
    char		myChar;
    
    MIOWin_AssertOuterWindow (pmWindow);
    
    myInfo = MIOWin_GetInfo (pmWindow);
    
    if (myInfo -> visibility == VIS_POPUP)
    {
    	MDIOWin_ShowWindow (pmWindow);
    	MIOWin_GetInfo (pmWindow) -> visibility = VIS_VISIBLE;
    }
    else if (myInfo -> visibility == VIS_INVISIBLE)
    {
    	// TW Error should be set correctly.
    	ABORT_WITH_ERRMSG (1, 
    		"Keyboard input from invisible window is not allowed");
    }

    myChar = myInfo -> kbdBuffer [myInfo -> kbdBufferTail];
    myInfo -> kbdBufferTail = INC (myInfo -> kbdBufferTail);
    
    // Commented out because we decided not to echo getch input
    // if (myInfo -> echo)
    // {
    //	  MyAddChar (pmWindow, myChar, MARK_END_OF_OUTPUT_POSITION);
    // }
    
    return myChar;
} // MIOWin_Getch


/************************************************************************/
/* MIOWin_GetCharacter							*/
/************************************************************************/
int	MIOWin_GetCharacter (WIND pmWindow, BOOL *pmEchoEOF)
{
    MIOWinInfoPtr	myInfo;
    char		myResult;

    MIOWin_AssertOuterWindow (pmWindow);
    
    if (pmEchoEOF)
    {    
    	*pmEchoEOF = FALSE;
    }
    
    myInfo = MIOWin_GetInfo (pmWindow);

    // If there's a character in the ungot position
    if (myInfo -> ungetChar != EOF)
    {
	myResult = myInfo -> ungetChar;
	myInfo -> ungetChar = EOF;
	return myResult;
    }

    if (myInfo -> eofAlreadyEntered)
    {
	// TW: Assertion Failure
	return EOF;
    }

    // If there's a character in the line buffer to be sent
    if (myInfo -> lineBufferHead != myInfo -> lineBufferTail)
    {
	myResult = myInfo -> lineBuffer [myInfo -> lineBufferTail];
	myInfo -> lineBufferTail = INC(myInfo -> lineBufferTail);
	return myResult;
    }
    
    /* If an EOF is "on deck" ready to be sent */
    if (myInfo -> eofOnDeck)
    {
	// Echoing takes place in MIO_Fgetc
	myInfo -> eofAlreadyEntered = TRUE;
    	if (pmEchoEOF)
    	{    
	    *pmEchoEOF = TRUE;
	}
	return EOF;
    }
    
    return EOF;
} // MIOWin_GetCharacter


/************************************************************************/
/* MIOWin_GetInfo							*/
/************************************************************************/
MIOWinInfoPtr	MIOWin_GetInfo (WIND pmInnerOrOuterWindow)
{
    int			myWindowType;
    MIOWinInfoPtr	myInfo;
    
    myWindowType = MDIOWin_GetWindowType (pmInnerOrOuterWindow);

    if ((myWindowType == WINDOW_TYPE_MIO_TEXT_INNER) ||
        (myWindowType == WINDOW_TYPE_MIO_GRAPHICS_INNER))
    {
    	pmInnerOrOuterWindow = MDIOWin_GetParent (pmInnerOrOuterWindow);
    	myWindowType = MDIOWin_GetWindowType (pmInnerOrOuterWindow);
    }
    
    if ((myWindowType != WINDOW_TYPE_MIO_TEXT) &&
        (myWindowType != WINDOW_TYPE_MIO_GRAPHICS) &&
	(myWindowType != WINDOW_TYPE_MIO_DIALOG))
    {
    	EdInt_HaltEnvironment (IDS_MIO_BADWINDOWTYPE, __FILE__, __LINE__, 0,
    			       myWindowType, WINDOW_TYPE_MIO_TEXT);
    }
    	
    myInfo = (MIOWinInfoPtr) MDIOWin_GetWindowInfo (pmInnerOrOuterWindow);
       
    return myInfo;
} // MIOWin_GetInfo


/************************************************************************/
/* MIOWin_GetProperties							*/
/************************************************************************/
MIOWinOnly_PropertiesPtr	MIOWin_GetProperties (void)
{
    return &stProperties;
} // MIOWin_GetProperties


/************************************************************************/
/* MIOWin_GetWindowType							*/
/************************************************************************/
int	MIOWin_GetWindowType (WIND pmWindow)
{
    return MDIOWin_GetWindowType (pmWindow);
} // MIOWin_GetWindowType


/************************************************************************/
/* MIOWin_Hasch								*/
/************************************************************************/
BOOL	MIOWin_Hasch (WIND pmWindow)
{
    MIOWinInfoPtr	myInfo;
    
    MIOWin_AssertOuterWindow (pmWindow);
    
    myInfo = MIOWin_GetInfo (pmWindow);
    
    if (myInfo -> visibility == VIS_POPUP)
    {
    	MDIOWin_ShowWindow (pmWindow);
    	MIOWin_GetInfo (pmWindow) -> visibility = VIS_VISIBLE;
    }
    else if (myInfo -> visibility == VIS_INVISIBLE)
    {
    	// TW Error should be set correctly.
    	ABORT_WITH_ERRMSG (1, 
    		"Keyboard input from invisible window is not allowed");
    }

    return (myInfo -> kbdBufferHead != myInfo -> kbdBufferTail);
} // MIOWin_Hasch


/************************************************************************/
/* MIOWin_InitializeWindow						*/
/************************************************************************/
int	MIOWin_InitializeWindow (WIND pmWindow, 
				 WindowAttribPtr pmWindowAttribs)
{
    MIOWinInfoPtr	myInfo;
    BOOL		myResult;
    
    //
    // Initialize the extra window memory
    //
	
    // Set the window type
    if (pmWindowAttribs -> mode == TEXT_MODE)
    {
        MDIOWin_SetWindowType (pmWindow, WINDOW_TYPE_MIO_TEXT);
    }
    else
    {
        MDIOWin_SetWindowType (pmWindow, WINDOW_TYPE_MIO_GRAPHICS);
    }
    
    // Set the window initialized                
    MDIOWin_SetWindowInitialization (pmWindow, FALSE);
            
    // If the font properties have not already been determined, 
    // determine them now (we needed a window to do so).  This must be
    // called *after* the top widow and status window created and
    // before the window info.
    MIOWin_PropertiesImplementChanges (pmWindow);
        	    
    // Create the edit window information
    // Allocate the structure for the information about the edit window
    myInfo = (MIOWinInfoPtr) malloc (sizeof (MIOWindowInfo));
    
    if (myInfo == NULL)
    {
        EdInt_AddFailMessage ("MIOWin_InitializeWindow: No room for info");

	return -1;
    }

    memset (myInfo, 0, sizeof (MIOWindowInfo));
    
    myInfo -> windowKind = pmWindowAttribs -> mode;
    
    myInfo -> initXPos = pmWindowAttribs -> xPosition;
    myInfo -> initYPos = pmWindowAttribs -> yPosition;
    myInfo -> initRows = pmWindowAttribs -> rows;
    myInfo -> initColumns = pmWindowAttribs -> columns;
    myInfo -> initXSize = pmWindowAttribs -> xSize;
    myInfo -> initYSize = pmWindowAttribs -> ySize;
    
    myInfo -> fontCharWidth = stTextFontCharWidth;
    myInfo -> fontCharHeight = stTextFontCharHeight;
    myInfo -> fontCharAscent = stTextFontCharAscent;
    myInfo -> fontCharDescent = stTextFontCharDescent;
    myInfo -> fontCharIntLeading = stTextFontCharIntLeading;
    myInfo -> font = stNormalFont;
    myInfo -> selectionColour = stSelectionColour;

    myInfo -> lineBufferLastNewLine = -1;
    myInfo -> ungetChar = EOF;
        
    myInfo -> textRect.top = -1000;
    myInfo -> textRect.bottom = -1000;
    myInfo -> textRect.left = -1000;
    myInfo -> textRect.right = -1000;

	
    myInfo -> acceptingInput = FALSE;
    myInfo -> waitingForInputMessageDisplayed = FALSE;

    myInfo -> cursor = pmWindowAttribs -> cursor;
    myInfo -> echo = pmWindowAttribs -> echo;
    myInfo -> visibility = pmWindowAttribs -> visibility;
    myInfo -> displayOnScreen = pmWindowAttribs -> displayOnScreen;
    myInfo ->msdosCharSet = pmWindowAttribs -> msdosCharSet;
    myInfo -> zOrder = pmWindowAttribs -> topMost;

    MDIOWin_SetWindowInfo (pmWindow, myInfo);
    
    //
    // Add the elements to the window
    //

    // Add the button bar window
    EdInt_AddFailMessage ("MIOWin_InitializeWindow: Create button bar");
    if (pmWindowAttribs -> buttonBar == 1)
    {
	myInfo -> buttonBarWindow = MDIOWinTop_Create (pmWindow, 
    				(pmWindowAttribs -> mode == TEXT_MODE));
    
	if (myInfo -> buttonBarWindow == NULL)
	{
            EdInt_AddFailMessage ("MIOWin_InitializeWindow: No button bar");
            
    	    return -1;
    	}
    }

    // Specify the outer window
    myInfo -> outerWindow = pmWindow;
    
    // Initialize info fields
    strcpy (myInfo -> title, pmWindowAttribs -> title);
    MDIOWin_SetWindowTitle (pmWindow, myInfo -> title);
//EdLog_Log ("Run Window created: hwnd=%x title='%s'", pmWindow, myInfo -> title);

    // Set the window to initialized
    MDIOWin_SetWindowInitialization (pmWindow, TRUE);
    
    // Calculate the window size
    MIOWin_CalculateWindowSize (pmWindow, pmWindowAttribs);

    if (pmWindowAttribs -> mode == TEXT_MODE)
    {
    	// Add the console text window
    	EdInt_AddFailMessage ("MIOWin_InitializeWindow: Create inner text win");
    	myInfo -> innerWindow = MDIOWinText_CreateInnerWindow (pmWindow);
	myResult = MDIOWinText_InitializeWindow (pmWindow);
    }
    else
    {
    	// Add the console text window
    	EdInt_AddFailMessage ("MIOWin_InitializeWindow: Create inner grph win");
    	myInfo -> innerWindow = MDIOWinGraph_CreateInnerWindow (pmWindow);
	myResult = MDIOWinGraph_InitializeWindow (pmWindow);
    }
    
    if (myInfo -> innerWindow == NULL)
    {
        EdInt_AddFailMessage ("MIOWin_InitializeWindow: No inner window");
            
    	return -1;
    }

    if (!myResult)
    {
        EdInt_AddFailMessage (
            "MIOWin_InitializeWindow: MDIOWinGraph_InitializeWindow failed");
            
    	return -1;
    }

    // Calculate the top, left corner of the window
    MIOWin_CalculateWindowDisplacement (pmWindow, pmWindowAttribs);

    // Set the window to the correct position
    MDIOWin_SetWindowPosition (pmWindow, myInfo -> initWindowLeft, 
        myInfo -> initWindowTop, myInfo -> maxWindowWidth, 
        myInfo -> maxWindowHeight);
/*
    // Hide the scroll bars
    if (pmWindowAttribs -> mode == TEXT_MODE)
    {
    	MDIOWinText_SynchScrollBarsToText (myInfo -> innerWindow);
    }
    else
    {
    	MDIOWinGraph_SynchScrollBarsToGraphics (myInfo -> innerWindow);
    }
*/    							
    
    // Add the window to the list of MIO windows
    MIO_AddToRunWindowList (pmWindow);

    // Get a Turing window ID for this window
//    myInfo -> turingWindowID = MIO_StreamAdd (WINDOW_STREAM, myInfo);

    EdInt_AddFailMessage ("MIOWin_InitializeWindow: Window initialized");
    
    return 0;
} // MIOWin_InitializeWindow


/************************************************************************/
/* MIOWin_InitRun							*/
/************************************************************************/
void	MIOWin_InitRun (void)
{
    stLeftDisplacement = MDIOWin_GetTitleBarHeight ();
    if (stCenterOutputWindow)
    {
    	stTopDisplacement = -1;
    }
    else
    {
    	stTopDisplacement = MDIOWin_GetTitleBarHeight ();
    }
    
} // MIOWin_InitRun


/************************************************************************/
/* MIOWin_IsInputAvailable						*/
/*									*/
/* When the system wants input from the window, it will repeatedly call	*/
/* this routine until it returns TRUE.  While this is returning false,	*/
/* the cursor in the window should be activated and each time this is	*/
/* called, we should process any waiting keyboard input.		*/
/************************************************************************/
BOOL	MIOWin_IsInputAvailable (WIND pmWindow, int pmKind, int pmCount)
{
    MIOWinInfoPtr	myInfo;
    BOOL		myResult;
    
    MIOWin_AssertOuterWindow (pmWindow);
    
    myInfo = MIOWin_GetInfo (pmWindow);
    
    if (myInfo -> visibility == VIS_POPUP)
    {
    	MDIOWin_ShowWindow (pmWindow);
    	MIOWin_GetInfo (pmWindow) -> visibility = VIS_VISIBLE;
    }
    else if (myInfo -> visibility == VIS_INVISIBLE)
    {
    	// TW Error should be set correctly.
    	ABORT_WITH_ERRMSG (1, 
    			"Keyboard input from invisible window is not allowed");
    }

    myResult = MyIsInputAvailable (pmWindow, pmKind, pmCount);
    
    if (!myResult && !myInfo -> waitingForInputMessageDisplayed)
    {
	myInfo -> acceptingInput = TRUE;  
	myInfo -> waitingForInputMessageDisplayed = TRUE;
	myInfo -> outputSinceLastInput = FALSE;
	myInfo -> outputSinceLastInputChar = TRUE;
	MIOWin_CaretDisplay (pmWindow);
	MIOWin_SetRunWindowTitle (pmWindow);
    }
    else if (myResult && myInfo -> waitingForInputMessageDisplayed)
    {
	myInfo -> acceptingInput = FALSE;  // Should already be false.(TW No More)
	myInfo -> waitingForInputMessageDisplayed = FALSE;
	MIOWin_CaretDisplay (pmWindow); // Will turn it off
	MIOWin_SetRunWindowTitle (pmWindow);
    }

    return myResult;
} // MIOWin_IsInputAvailable


/************************************************************************/
/* MIOWin_IsTextWindow							*/
/*									*/
/* Returns TRUE if the window is either a text window or the text child	*/
/* window.								*/
/************************************************************************/
BOOL	MIOWin_IsTextWindow (WIND pmInnerOrOuterWindow)
{
    int myWindowType = MDIOWin_GetWindowType (pmInnerOrOuterWindow);

    return (myWindowType == WINDOW_TYPE_MIO_TEXT) ||
	   (myWindowType == WINDOW_TYPE_MIO_TEXT_INNER);
} // MIOWin_IsTextWindow


/************************************************************************/
/* MIOWin_KeystrokeHandler						*/
/************************************************************************/
void	MIOWin_KeystrokeHandler (WIND pmWindow, UCHAR pmKeystroke)
{
    MIOWinInfoPtr	myInfo;

    MIOWin_AssertOuterWindow (pmWindow);
    
    // Get the window info    
    myInfo = MIOWin_GetInfo (pmWindow);

    // Beep if input is coming from a file, the program has finished running 
    // or the keyboard buffer is full
    if (myInfo -> inputFromFile || MIO_paused || MIO_finished ||
        (INC (myInfo -> kbdBufferHead) == myInfo -> kbdBufferTail))
    {
    	MDIO_Beep ();
    	return;
    }

    // Add to the keyboard buffer
    myInfo -> kbdBuffer [myInfo -> kbdBufferHead] = pmKeystroke;
    myInfo -> kbdBufferHead = INC (myInfo -> kbdBufferHead);
} // MIOWin_KeystrokeHandler

				    
/************************************************************************/
/* MIOWin_OutputText							*/
/************************************************************************/
void	MIOWin_OutputText (WIND pmWindow, char *pmBuffer)
{
    MIOWinInfoPtr	myInfo = MIOWin_GetInfo (pmWindow);
    
    myInfo -> outputSinceLastInput = TRUE;
    
    MIOWin_AddText (pmWindow, pmBuffer, MARK_END_OF_OUTPUT_POSITION);
} // MIOWin_OutputText


/************************************************************************/
/* MIOWin_SaveWindowToFile						*/
/************************************************************************/
void	MIOWin_SaveWindowToFile (WIND pmWindow, const char *pmPathName)
{
    if (MIOWin_IsTextWindow (pmWindow))
    {
	MIOWinText_SaveWindowToFile (pmWindow, pmPathName);
    }
    else
    {
	MIOWinGraph_SaveWindowToFile (pmWindow, pmPathName);
    }
} // MIOWin_SaveWindowToFile


/************************************************************************/
/* MIOWin_SetRunWindowTitle						*/
/************************************************************************/
void	MIOWin_SetRunWindowTitle (WIND pmWindow)
{
    MIOWinInfoPtr	myInfo;
    char		myWindowTitle [1024];
  
    MIOWin_AssertOuterWindow (pmWindow);
    
    myInfo = MIOWin_GetInfo (pmWindow);

    strcpy (myWindowTitle, myInfo -> title);
    if (MIO_finished)
    {
	strcat (myWindowTitle, " - Finished");
    }
    else if (MIO_paused)
    {
	strcat (myWindowTitle, " - Paused");
    }
    else if (myInfo -> acceptingInput)
    {
    	strcat (myWindowTitle, " - Waiting for Input");
    }
    else if (myInfo -> waitingForMouseClick > 0)
    {
    	strcat (myWindowTitle, " - Waiting for Mouse Click");
    }

    MDIOWin_SetWindowTitle (pmWindow, myWindowTitle);
} // MIOWin_SetRunWindowTitle


/************************************************************************/
/* MIOWin_UngetCharacter						*/
/************************************************************************/
void	MIOWin_UngetCharacter (OOTint pmChar, WIND pmWindow)
{
    MIOWinInfoPtr	myInfo;
    
    MIOWin_AssertOuterWindow (pmWindow);
    
    myInfo = MIOWin_GetInfo (pmWindow);
    
    myInfo -> ungetChar = pmChar;
} // MIOWin_UngetCharacter


/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/
/************************************************************************/
/* MyAddChar								*/
/************************************************************************/
static void	MyAddChar (WIND pmWindow, char pmChar, int pmFlag)
{
    char    myBuffer [2];

    MIOWin_AssertOuterWindow (pmWindow);
    
    myBuffer [0] = pmChar;
    myBuffer [1] = 0;

    MIOWin_AddText (pmWindow, myBuffer, pmFlag);
} // MyAddChar


/************************************************************************/
/* MyDeletePreviousChar							*/
/************************************************************************/
static void	MyDeletePreviousChar (WIND pmWindow)
{
    MIOWinInfoPtr	myInfo = MIOWin_GetInfo (pmWindow);

    MIOWin_AssertOuterWindow (pmWindow);
    
    if (MIOWin_IsTextWindow (pmWindow))
    {
	int	myCurrentPos = MIOWinText_GetCurrentTextPos (pmWindow);

	MIOWinText_DeleteText (pmWindow, myCurrentPos - 1);
    }
    else
    {
	if (MyIsAtBeginningOfLine (pmWindow))
	{
	    if (myInfo -> actualRow != 0)
	    {
		MIOWinGraph_DeleteText (pmWindow, myInfo -> actualRow - 1, 
			   	        myInfo -> maxCol);
	    }
	}
	else
	{
	    MIOWinGraph_DeleteText (pmWindow, myInfo -> actualRow, 
				    myInfo -> actualCol - 1);
	}
    }
} // MyDeletePreviousChar


/************************************************************************/
/* MyDeleteText								*/
/************************************************************************/
static void	MyDeleteText (WIND pmWindow, int pmRow, int pmCol)
{
    MIOWin_AssertOuterWindow (pmWindow);
    
    if (MIOWin_IsTextWindow (pmWindow))
    {
	MIOWinText_DeleteText (pmWindow, pmRow);
    }
    else
    {
    	MIOWinGraph_DeleteText (pmWindow, pmRow, pmCol);
    }
} // MyDeleteText


/************************************************************************/
/* MyGetCurrentLineCol							*/
/************************************************************************/
static void	MyGetCurrentLineCol (WIND pmWindow, int *pmLine, int *pmCol)
{
    MIOWin_AssertOuterWindow (pmWindow);
    
    if (MIOWin_IsTextWindow (pmWindow))
    {
	MIOWinText_GetCurrentLineCol (pmWindow, pmLine, pmCol);
    }
    else
    {
    	MIOWinInfoPtr	myInfo = MIOWin_GetInfo (pmWindow);
    	
    	*pmLine = myInfo -> actualRow;
    	*pmCol = myInfo -> actualCol;
    }
} // MyGetCurrentLineCol


/************************************************************************/
/* MyGetHDisplacement							*/
/************************************************************************/
static int	MyGetHDisplacement (WIND pmWindow)
{
    MIOWinInfoPtr	myInfo = MIOWin_GetInfo (pmWindow);
    
    if (MIOWin_IsTextWindow (pmWindow))
    	return myInfo -> leftCol * myInfo -> fontCharWidth;
    else
        return myInfo -> leftX;
} // MyGetHDisplacement


/************************************************************************/
/* MyGetLine								*/
/************************************************************************/
static const char	*MyGetLine (const char *pmPtr, char *pmLine)
{
    if ((pmPtr == NULL) || (*pmPtr == 0))
    {
	return NULL;
    }

    // Skip white space
    while (*pmPtr == ' ') pmPtr++;
    
    // If a character, read a string
    if ((*pmPtr == ',') || (*pmPtr == ';') || (*pmPtr == 0))
    {
	return NULL;
    }

    while ((*pmPtr != ',') && (*pmPtr != ';') && (*pmPtr != 0))
    {
    	*pmLine++ = *pmPtr++;
    }

    *pmLine = 0;
    	
    return pmPtr;
} // MyGetLine


/************************************************************************/
/* MyGetPosition							*/
/************************************************************************/
static const char	*MyGetPosition (const char *pmPtr, int *pmXPos, 
					int *pmYPos)
{
    BOOL    myIsNegative = FALSE;
    int	    myValue;
    
    if ((pmPtr == NULL) || (*pmPtr == 0))
    {
	return NULL;
    }

    // Skip white space
    while (*pmPtr == ' ') pmPtr++;
    
    // If a character, read a string
    if ((_strnicmp (pmPtr, "left", 4) == 0) && 
	((*(pmPtr + 4) == ',') || (*(pmPtr + 4) == ';') ||
	 (*(pmPtr + 4) == 0)))
    {
	*pmXPos = LEFT_ALIGN;
	return pmPtr + 4;
    }
    else if ((_strnicmp (pmPtr, "centre", 6) == 0) && 
	((*(pmPtr + 6) == ',') || (*(pmPtr + 6) == ';') ||
	 (*(pmPtr + 6) == 0)))
    {
	if (*pmXPos == CENTRE_ALIGN)
	{
	    // Person entered "position:centre,centre"
	    *pmYPos = MIDDLE_ALIGN;
	}
	else
	{
	    *pmXPos = CENTRE_ALIGN;
	}
	return pmPtr + 6;
    }
    else if ((_strnicmp (pmPtr, "center", 6) == 0) && 
	((*(pmPtr + 6) == ',') || (*(pmPtr + 6) == ';') ||
	 (*(pmPtr + 6) == 0)))
    {
	if (*pmXPos == CENTRE_ALIGN)
	{
	    // Person entered "position:centre,centre"
	    *pmYPos = MIDDLE_ALIGN;
	}
	else
	{
	    *pmXPos = CENTRE_ALIGN;
	}
	return pmPtr + 6;
    }
    else if ((_strnicmp (pmPtr, "right", 5) == 0) && 
	((*(pmPtr + 5) == ',') || (*(pmPtr + 5) == ';') ||
	 (*(pmPtr + 5) == 0)))
    {
	*pmXPos = RIGHT_ALIGN;
	return pmPtr + 5;
    }
    else if ((_strnicmp (pmPtr, "top", 3) == 0) && 
	((*(pmPtr + 3) == ',') || (*(pmPtr + 3) == ';') ||
	 (*(pmPtr + 3) == 0)))
    {
	*pmYPos = TOP_ALIGN;
	return pmPtr + 3;
    }
    else if ((_strnicmp (pmPtr, "middle", 6) == 0) && 
	((*(pmPtr + 6) == ',') || (*(pmPtr + 6) == ';') ||
	 (*(pmPtr + 6) == 0)))
    {
	if (*pmYPos == MIDDLE_ALIGN)
	{
	    // Person entered "position:middle,middle"
	    *pmXPos = CENTRE_ALIGN;
	}
	else
	{
	    *pmYPos = MIDDLE_ALIGN;
	}
	return pmPtr + 6;
    }
    else if ((_strnicmp (pmPtr, "truemiddle", 10) == 0) && 
	((*(pmPtr + 10) == ',') || (*(pmPtr + 10) == ';') ||
	 (*(pmPtr + 10) == 0)))
    {
	*pmYPos = TRUE_MIDDLE_ALIGN;
	return pmPtr + 10;
    }
    else if ((_strnicmp (pmPtr, "bottom", 6) == 0) && 
	((*(pmPtr + 6) == ',') || (*(pmPtr + 6) == ';') ||
	 (*(pmPtr + 6) == 0)))
    {
	*pmYPos = BOTTOM_ALIGN;
	return pmPtr + 6;
    }

    // Check for negative value
    if (*pmPtr == '-')
    {
	myIsNegative = TRUE;
	pmPtr++;
    }

    // If not a number, return null
    if (!isdigit (*pmPtr))
    {
	return NULL;
    }

    myValue = 0;

    do
    {
    	myValue = myValue * 10 + (*pmPtr++ - '0');
    } while (isdigit (*pmPtr));

    if (myIsNegative)
    {
	myValue *= -1;
    }

    if (*pmXPos == UNSET_VALUE)
    {
    	*pmXPos = myValue;
    }
    else
    {
    	*pmYPos = myValue;
    }
    
    if ((*pmPtr != ',') && (*pmPtr != ';') && (*pmPtr != 0))
    {
	return NULL;
    }

    return pmPtr;
} // MyGetPosition


/************************************************************************/
/* MyGetSeparator							*/
/************************************************************************/
static const char	*MyGetSeparator (const char *pmPtr)
{
    if ((pmPtr == NULL) || (*pmPtr == 0))
    {
	return NULL;
    }

    // Skip white space
    while (*pmPtr == ' ') pmPtr++;
    
    if ((*pmPtr != ',') && (*pmPtr != ';') && (*pmPtr != 0))
    {
	return NULL;
    }

    return pmPtr + 1;
} // MyGetSeparator


/************************************************************************/
/* MyGetValue								*/
/************************************************************************/
static const char	*MyGetValue (const char *pmPtr, int *pmValue)
{
    if ((pmPtr == NULL) || (*pmPtr == 0))
    {
	return NULL;
    }

    // Skip white space
    while (*pmPtr == ' ') pmPtr++;
    
    // If a character, read a string
    if ((_strnicmp (pmPtr, "max", 3) == 0) && 
	((*(pmPtr + 3) == ',') || (*(pmPtr + 3) == ';') ||
	 (*(pmPtr + 3) == 0)))
    {
	*pmValue = MAX_SIZE;
	return pmPtr + 3;
    }

    // If not a number, return null
    if (!isdigit (*pmPtr))
    {
	return NULL;
    }

    *pmValue = 0;

    do
    {
    	*pmValue = *pmValue * 10 + (*pmPtr++ - '0');
    } while (isdigit (*pmPtr));

    if ((*pmPtr != ',') && (*pmPtr != ';') && (*pmPtr != 0))
    {
	return NULL;
    }

    return pmPtr;
} // MyGetValue


/************************************************************************/
/* MyGetVDisplacement							*/
/************************************************************************/
static int	MyGetVDisplacement (WIND pmWindow)
{
    MIOWinInfoPtr	myInfo = MIOWin_GetInfo (pmWindow);
    
    if (MIOWin_IsTextWindow (pmWindow))
    	return myInfo -> topLine * myInfo -> fontCharHeight;
    else
        return myInfo -> topY;
} // MyGetVDisplacement


/************************************************************************/
/* MyGetWord								*/
/************************************************************************/
static const char	*MyGetWord (const char *pmPtr, char *pmWord)
{
    if (pmPtr == NULL)
    {
	return NULL;
    }

    // Skip white space
    while (*pmPtr == ' ') pmPtr++;

    // If at end of string, return word as empty string    
    if (*pmPtr == 0)
    {
    	*pmWord = 0;
    	return pmPtr;
    }
    
    // If a character, read a string
    if (!isalpha (*pmPtr))
    {
	return NULL;
    }

    do
    {
    	*pmWord++ = tolower (*pmPtr++);
    } while (isalpha (*pmPtr));
    if (*pmPtr == ':')
    {
        *pmWord++ = *pmPtr++;
    }
    else if ((*pmPtr != ',') && (*pmPtr != ';') && (*pmPtr != 0))
    {
	return NULL;
    }

    *pmWord = 0;
    	
    return pmPtr;
} // MyGetWord


/************************************************************************/
/* MyIsAtBeginningOfLine						*/
/************************************************************************/
static BOOL	MyIsAtBeginningOfLine (WIND pmWindow)
{
    MIOWinInfoPtr	myInfo = MIOWin_GetInfo (pmWindow);
    
    MIOWin_AssertOuterWindow (pmWindow);
    
    if (MIOWin_IsTextWindow (pmWindow))
    {
	return MIOWinText_IsAtBeginningOfLine (pmWindow);
    } 	     
    else
    {
    	return (myInfo -> actualCol == 0);
    }	       
} // MyIsAtBeginningOfLine


/************************************************************************/
/* MyIsCurrentPositionLastOutput					*/
/************************************************************************/
static BOOL	MyIsCurrentPositionLastOutput (WIND pmWindow)
{
    MIOWinInfoPtr	myInfo = MIOWin_GetInfo (pmWindow);
    
    MIOWin_AssertOuterWindow (pmWindow);
    
    if (MIOWin_IsTextWindow (pmWindow))
    {
	int	myCurrentPos = MIOWinText_GetCurrentTextPos (pmWindow);
	
	return (myInfo -> startInputRow == myCurrentPos);
    }
    else
    {
    	return ((myInfo -> startInputRow == myInfo -> actualRow) &&
	        (myInfo -> startInputCol == myInfo -> actualCol));
    }	       
} // MyIsCurrentPositionLastOutput

/************************************************************************/
/* MyIsInputAvailable							*/
/*									*/
/* When the system wants input from the window, it will repeatedly call	*/
/* this routine until it returns TRUE.  While this is returning false,	*/
/* the cursor in the window should be activated and each time this is	*/
/* called, we should process any waiting keyboard input.		*/
/************************************************************************/
static BOOL	MyIsInputAvailable (WIND pmWindow, int pmKind, int pmCount)
{
    MIOWinInfoPtr	myInfo;
    char		myChar;
    int			cnt;
    
    MIOWin_AssertOuterWindow (pmWindow);
    
    myInfo = MIOWin_GetInfo (pmWindow);
    
    switch (pmKind)
    {
	case EventMode_InputLine:
	    // If we've already sent an EOF, then allow the read
	    if (myInfo -> eofOnDeck || myInfo -> eofAlreadyEntered)
	    {
		return TRUE;
	    }
	    
	    // If the ungot character is a newline, then allow the read
	    if (myInfo -> ungetChar == '\n')
	    {
		return TRUE;
	    }
	    
	    // If there is a NL in the line buffer, then allow read
	    if (MyIsNewLineInBuffer (pmWindow))
	    {
		return TRUE;
	    }
	    
	    // The call to MyProcessFromKeyboardToLineBuffer reads a maximum 
	    // of one line of input (and returns TRUE if it did).
       	    return (MyProcessFromKeyboardToLineBuffer (pmWindow));
       	    
	case EventMode_InputCount:
	    // If we've already sent an EOF, then allow the read
	    if (myInfo -> eofOnDeck || myInfo -> eofAlreadyEntered)
	    {
		return TRUE;
	    }

	    // If there's a line in the line buffer, see if there are enough
	    // characters in it.
	    if (MyIsNewLineInBuffer (pmWindow))
	    {
		int	myCharCount = 0;
		
		if (myInfo -> ungetChar != EOF) myCharCount++;
		cnt = myInfo -> lineBufferTail;
		while (myInfo -> lineBufferLastNewLine != cnt)
		{
		    myCharCount++;
		    cnt = INC(cnt);
		}
		if (myCharCount >= pmCount)
		{
		    return TRUE;
		}
	    }
	    
	    // Not enough characters were read in, read more from the keyboard
	    // Each call to MyProcessFromKeyboardToLineBuffer reads a maximum 
	    // of one line of input (and returns TRUE if it did).
       	    while (MyProcessFromKeyboardToLineBuffer (pmWindow))
       	    {
		int	myCharCount = 0;
		cnt = myInfo -> lineBufferTail;
		while (myInfo -> lineBufferLastNewLine != cnt)
		{
		    myCharCount++;
		    cnt = INC(cnt);
		}
		if (myCharCount >= pmCount)
		{
		    return TRUE;
		}
	    }
	    
	    return FALSE;

	case EventMode_InputToken:
	    // If we've already sent an EOF, then allow the read
	    if (myInfo -> eofOnDeck || myInfo -> eofAlreadyEntered)
	    {
		return TRUE;
	    }

	    // If the ungot character is a non-whitespace char, 
	    // then allow the read
	    if (myInfo -> ungetChar != EOF)
	    {
		myChar = myInfo -> ungetChar;
		if ((myChar != ' ') && (myChar != '\t') && (myChar != '\n'))
		{
		    return TRUE;
		}
	    }
		
	    if (MyIsNewLineInBuffer (pmWindow))
	    {
		// TW: At some point, it may become necessary to	
		// actually read chars off the queue.  Otherwise it is	
		// possible that the queue could fill up with white	
		// space chars until there's no space to put a new char
		cnt = myInfo -> lineBufferTail;
		while (myInfo -> lineBufferLastNewLine != cnt)
		{
		    myChar = myInfo -> lineBuffer [cnt];
		    if ((myChar != ' ') && (myChar != '\t') && (myChar != '\n'))
		    {
			return TRUE;
		    }
		    cnt = INC(cnt);
		} // while
	    }

	    // Each call to MyProcessFromKeyboardToLineBuffer reads a maximum 
	    // of one line of input (and returns TRUE if it did)).
	    while (MyProcessFromKeyboardToLineBuffer (pmWindow))
	    {
		cnt = myInfo -> lineBufferTail;
		while (myInfo -> lineBufferLastNewLine != cnt)
		{
		    myChar = myInfo -> lineBuffer [cnt];
		    if ((myChar != ' ') && (myChar != '\t') && (myChar != '\n'))
		    {
			return TRUE;
		    }
		    cnt = INC(cnt);
		} // while
	    }
	    
	    return FALSE;

	case EventMode_InputUnblocked:
	    return (myInfo -> kbdBufferHead != myInfo -> kbdBufferTail);
    } // switch
    
    return FALSE;
} // MyIsInputAvailable


/************************************************************************/
/* MyIsNewLineInBuffer							*/
/************************************************************************/
static BOOL	MyIsNewLineInBuffer (WIND pmWindow)
{
    MIOWinInfoPtr	myInfo;
    int			cnt;
    
    myInfo = MIOWin_GetInfo ((WIND) pmWindow);
    
    cnt = myInfo -> lineBufferTail;

    while (cnt != myInfo -> lineBufferHead)
    {
	if (myInfo -> lineBuffer [cnt] == '\n')
	{
	    return TRUE;
	}
	cnt = INC(cnt);
    }
    return FALSE;
} // MyIsNewLineInBuffer


/************************************************************************/
/* MyPaste								*/
/************************************************************************/
static void	MyPaste (WIND pmWindow)
{
    MIOWinInfoPtr	myInfo;
    char		*myBuffer;
    int			myBufferLen;
    int			cnt;
            
    MIOWin_AssertOuterWindow (pmWindow);
    
    // Get the window info    
    myInfo = MIOWin_GetInfo (pmWindow);

    // Beep if input is coming from a file, the program has finished running
    // or the keyboard buffer is full
    if (myInfo -> inputFromFile || MIO_paused || MIO_finished ||
        (INC (myInfo -> kbdBufferHead) == myInfo -> kbdBufferTail))
    {
    	MDIO_Beep ();
    	return;
    }

    myBuffer = MDIOWin_GetClipboardBuffer (pmWindow);
    myBufferLen = strlen (myBuffer);
    
    // Add to the keyboard buffer
    for (cnt = 0 ; cnt < myBufferLen ; cnt++)
    {
    	if (INC (myInfo -> kbdBufferHead) == myInfo -> kbdBufferTail)
    	{
    	    MDIO_Beep ();
    	    break;
    	}
    	
    	// Convert '\r' or '\r\n' to '\n'
    	if (myBuffer [cnt] == '\r')
    	{
    	    myInfo -> kbdBuffer [myInfo -> kbdBufferHead] = RETURN;
    	    if ((cnt < myBufferLen - 1) &&
    	        (myBuffer [cnt + 1] == '\n'))
    	    {
    		cnt++;
    	    }
    	}
    	else if (myBuffer [cnt] == '\n')
    	{
    	    myInfo -> kbdBuffer [myInfo -> kbdBufferHead] = RETURN;
    	}
    	else
    	{
    	    myInfo -> kbdBuffer [myInfo -> kbdBufferHead] = myBuffer [cnt];
    	}
    	myInfo -> kbdBufferHead = INC (myInfo -> kbdBufferHead);
    }
    
    free (myBuffer);
} // MyPaste


/************************************************************************/
/* MyProcessFromKeyboardToLineBuffer					*/
/************************************************************************/
static BOOL	MyProcessFromKeyboardToLineBuffer (WIND pmWindow)
{
    MIOWinInfoPtr	myInfo;
    char		myChar, myCharToErase;
    BOOL		myFoundNewLine;
    int			cnt;
    	    	
    MIOWin_AssertOuterWindow (pmWindow);
    
    // Get the window info    
    myInfo = MIOWin_GetInfo (pmWindow);

    myFoundNewLine = FALSE;

    while (myInfo -> kbdBufferHead != myInfo -> kbdBufferTail)
    {
    	myChar = myInfo -> kbdBuffer [myInfo -> kbdBufferTail];
    	myInfo -> kbdBufferTail = INC (myInfo -> kbdBufferTail);
    	
	//
	// Handle a Carriage Return
	//
	if ((myChar == NEWLINE) || (myChar == RETURN))
	{
	    myFoundNewLine = TRUE;
	    break;
	}

	//
	// Handle a ^D or ^Z
	//
	if ((myChar == CTRL_D) || (myChar == CTRL_Z))
	{
	    myInfo -> eofOnDeck = TRUE;
	    if (myInfo -> lineBufferHead == 
	    				INC(myInfo -> lineBufferLastNewLine))
	    {
		return TRUE;
	    }
	
	    myFoundNewLine = TRUE;
	    break;
	}

	//
	// Handle a Backspace
	//
	if (myChar == BACKSPACE)
	{	
	    if (myInfo -> lineBufferHead == 
	    	INC(myInfo -> lineBufferLastNewLine))
	    {
	    	// If there's no characters to be backspaces over, then beep
	    	MDIO_Beep ();
	    }
	    else
	    {
		myInfo -> lineBufferHead = DEC(myInfo -> lineBufferHead);
	    	myCharToErase = myInfo -> lineBuffer [myInfo -> lineBufferHead];
		    	
	    	// If we delete a non-tab, see if the previous char is part
	    	// of the input and if it is not, repeat the line buffer on
	    	// a new line.  If it is a tab, then erase the entire line if
	    	// possible and redraw it.  If not, then erase the part 
	    	// available and redraw the line on a new line
	    	//
	    	// Examples:  Uppercase output, lowercase keystrokes
	    	//    ABCabc	       => ABCab^
	    	//    abcABCdefDEF     => abcABCdefDEF
	    	//                        abcde^
	    	//    abcABCdef        => abcABCde^
	    	//    ABCab\t	       -> ABCab^
	    	//    abcABCdefDEFgh\t => abcABCdefDEF
	    	//		          abcdefgh^

		// Skip deleting stuff on the screen if "echo" is off
	    	if (!myInfo -> echo) continue;

	    	if (myCharToErase != TAB)
	    	{
		    // If we've moved the cursor or added output since the
		    // last input char, we don't want to erase a different
		    // character, so we redraw the line.
		    if (MyIsCurrentPositionLastOutput (pmWindow))
		    {
		    	// The previous character is from output, redraw
		    	// the rest of the line (on a new line if necessary)
		    	if (!MyIsAtBeginningOfLine (pmWindow));
		 	{
		 	    // Add a newline
			    MyAddChar (pmWindow, '\n', 
			    	       MARK_END_OF_OUTPUT_POSITION);
		 	}
		 	
		 	// Add the rest of the line buffer
			cnt = myInfo -> lineBufferTail;
			while (myInfo -> lineBufferHead != cnt)
		 	{
			    MyAddChar (pmWindow, myInfo -> lineBuffer [cnt],
		    	    	       DONT_MARK);
			    cnt = INC(cnt);
			}
		    }
		    else
		    {
		    	// Remove the last character
		    	MyDeletePreviousChar (pmWindow);
		    }	    	    			
		} // if (myCharToErase != TAB)
		else
		{
		    // Erase from start of input
		    MyDeleteText (pmWindow, myInfo -> startInputRow, 
		   		  myInfo -> startInputCol);
		    
		    // If that wasn't all the input, then add a newline
		    // if necessary	    
		    if (myInfo -> outputSinceLastInput)
		    {
		    	if (!MyIsAtBeginningOfLine (pmWindow))
		 	{
		 	    // Add a newline
			    MyAddChar (pmWindow, '\n', 
			    	       MARK_END_OF_OUTPUT_POSITION);
		 	}
		    }

		    // Add the rest of the line buffer
		    cnt = myInfo -> lineBufferTail;
		    while (myInfo -> lineBufferHead != cnt)
		    {
			MyAddChar (pmWindow, myInfo -> lineBuffer [cnt],
		    	    	   DONT_MARK);
			cnt = INC(cnt);
		    } // while (myInfo -> lineBufferHead != cnt)
		} // else (myCharToErase == TAB)
	    } // else char available to erase
	}  // if (myChar == BACKSPACE)
	else if (myChar == CTRL_U)
	{
	    if (myInfo -> lineBufferHead == 
	    	INC(myInfo -> lineBufferLastNewLine))
	    {
	    	// If there's no characters to be erase, then beep
	    	MDIO_Beep ();
	    }
	    else
	    {
	    	// Wipe out text in the buffer
		myInfo -> lineBufferHead = INC(myInfo -> lineBufferLastNewLine);
	    				
		// Skip deleting stuff on the screen if "echo" is off
	    	if (!myInfo -> echo) continue;

		// Erase from start of input
		MyDeleteText (pmWindow, myInfo -> startInputRow, 
		              myInfo -> startInputCol);
		    
		// If that wasn't all the input, then add a newline
		// if necessary	    
		if (myInfo -> outputSinceLastInput)
		{
		    if (!MyIsAtBeginningOfLine (pmWindow))
		    {
		 	// Add a newline
			MyAddChar (pmWindow, '\n', MARK_END_OF_OUTPUT_POSITION);
		    }
		}
	    } // else char available to erase
	} // if CTRL_U
	else
	{
	    // This is a real character, enter it onto the queue
	    // TW Actually, check for ASCII char
	    
	    // Always make certain there's enough space in the
	    // queue for a newline
	    if ((INC(myInfo -> lineBufferHead) != myInfo -> lineBufferTail) &&
		(INC2(myInfo -> lineBufferHead) != myInfo -> lineBufferTail))
	    {
		myInfo -> lineBuffer [myInfo -> lineBufferHead] = 
							(unsigned char) myChar;
		myInfo -> lineBufferHead = INC(myInfo -> lineBufferHead);
	    	
		// Skip deleting stuff on the screen if "echo" is off
	    	if (!myInfo -> echo) continue;

	    	// Echo the character onto the screen
		if (myInfo -> lineBufferHead == 
					INC(myInfo -> lineBufferLastNewLine))
					
	    	{
	    	    MyAddChar (pmWindow, myChar, MARK_END_OF_OUTPUT_POSITION);
		}
		else
		{
	    	    MyAddChar (pmWindow, myChar, DONT_MARK);
		}		    	
	    }
	    else
	    {
	    	MDIO_Beep ();
	    }
	} // else
    } // while (myInfo -> kbdBufferHead != myInfo -> kbdBufferTail)
    
    if (myFoundNewLine)
    {
    	if (myInfo -> echo)
    	{
	    myInfo -> clearToEndOfLine = FALSE;
	    MyAddChar (pmWindow, '\n', MARK_END_OF_OUTPUT_POSITION);
	    myInfo -> clearToEndOfLine = TRUE;
	}
	myInfo -> lineBuffer [myInfo -> lineBufferHead] = '\n';
	cnt = INC(myInfo -> lineBufferLastNewLine);
	myInfo -> lineBufferLastNewLine = myInfo -> lineBufferHead;
	myInfo -> lineBufferHead = INC(myInfo -> lineBufferHead);

	// TW - Why is this here? (I put it here - TW) Document it! I can't!
	// I forgot why it's here!  Commenting out until I know why it's 
	// necessary.
//	myInfo -> acceptingInput = FALSE;

/* TW - If this is stdin, then echo to file, if called for
	if (echoToFile)
	{
	    while (w->lineBufferHead != cnt)
	    {
		MIO_Fputc (w->lineBuffer [cnt], 
			   MakeFileFromFile (mioStdinEcho->filePtr, 
					     mioStdinEcho->isPrinter));
		cnt = INC(cnt);
	    }
	}
*/	
    }
    
    return myFoundNewLine;
} // MyProcessFromKeyboardToLineBuffer


/************************************************************************/
/* MySkipWhiteSpace							*/
/************************************************************************/
static const char	*MySkipWhiteSpace (const char *pmPtr)
{
    while (*pmPtr == ' ') pmPtr++;

    return pmPtr;
} // MySkipWhiteSpace

