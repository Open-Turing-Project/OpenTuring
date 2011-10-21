/****************/
/* miowintext.c */
/****************/

/*******************/
/* System includes */
/*******************/

/****************/
/* Self include */
/****************/
#include "miowintext.h"

/******************/
/* Other includes */
/******************/
#include "mio.h"
#include "mdio.h"

#include "miodialogs.h"

#include "miowin.h"
#include "mdiowin.h"
#include "mdiowintext.h"

#include "edint.h"
//#include "edgui.h"

// Test to make certain we're not accidentally including <windows.h> which
// might allow for windows contamination of platform independent code.
#ifdef _INC_WINDOWS
xxx
#endif

#include "edtext.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
// Scrolling instructions for MyScrollToSelection
#define SCROLL_NORMAL		104

/********************/
/* Global variables */
/********************/

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

/******************************/
/* Static callback procedures */
/******************************/

/*********************/
/* Static procedures */
/*********************/
static TextPtr	MyGetTextPtr (WIND pmInnerOrOuterWindow);
static BOOL	MyMouseInTextRect (WIND pmInnerOrOuterWindow, int pmX, int pmY);
static void	MyMouseExtendSelectionToLine (WIND pmInnerWindow);
static void	MyMouseExtendSelectionToWord (WIND pmInnerWindow);
static void	MyScrollToSelection (WIND pmInnerOrOuterWindow, 
				     int pmSkipDistance, BOOL pmRedraw,
				     BOOL pmScrollToEnd);


/***********************/
/* External procedures */
/***********************/
/************************************************************************/
/* MIOWinText_Init							*/
/************************************************************************/
void	MIOWinText_Init (void)
{
} // MIOWinText_Init


/************************************************************************/
/* MIOWinText_Finalize							*/
/************************************************************************/
void	MIOWinText_Finalize (void)
{
} // MIOWinText_Finalize


/************************************************************************/
/* MIOWinText_PropertiesSet						*/
/************************************************************************/
void	MIOWinText_PropertiesSet (void)
{
    EdText_Properties	myTextProperties;

    myTextProperties.myProperties.numSpacesPerTab = 8;
    myTextProperties.myProperties.indentSize = 4;
    myTextProperties.myProperties.verifyLineDB = TRUE;
    
    EdText_PropertiesSet (myTextProperties);
} // MIOWinText_PropertiesSet

			  
/************************************************************************/
/* MIOWinText_PropertiesImplementChanges				*/
/************************************************************************/
void	MIOWinText_PropertiesImplementChanges (void)
{
} // MIOWinText_PropertiesImplementChanges


/************************************************************************/
/* MIOWinText_AddText							*/
/*									*/
/* If pmFlag is MARK_END_OF_OUTPUT_POSITION, the current caret position */
/* after the output is marked to indicate the beginning of a line of	*/
/* input.  This allows the program to easily delete a line containing	*/
/* tabs, etc in response to the user pressing backspace.		*/
/************************************************************************/
void	MIOWinText_AddText (WIND pmWindow, const char *pmText, int pmFlag)
{
    MIOWinInfoPtr	myInfo;
    TextPtr		myTextPtr;
    int 		myOrigSelFrom, myOrigSelTo;
    int			myTextPos;
    BOOL		myAtEnd;
    int			myAction;
    int			myLine, myCol;
    
    MIOWin_AssertWindowType (pmWindow, WINDOW_TYPE_MIO_TEXT);
    
    myInfo = MIOWin_GetInfo (pmWindow);
    myTextPtr = myInfo -> text;
    myTextPos = MIOWinText_GetCurrentTextPos (pmWindow);

    if ((myInfo -> topLine <= myTextPtr -> numLines) &&
    	(myTextPtr -> numLines <= myInfo -> topLine + myInfo -> windowHeight))
    {
    	myAtEnd = TRUE;
    }
    else
    {
    	myAtEnd = FALSE;
    }        
    
    myOrigSelFrom = myTextPtr -> selFrom;
    myOrigSelTo = myTextPtr -> selTo;
    
    myTextPtr -> selFrom = myTextPos;
    myTextPtr -> selTo = myTextPos;
    myTextPtr -> locationOK = FALSE;
    
    // Insert the text at that point
    myAction = EdText_InsertText (myTextPtr, pmText, strlen (pmText),
        			  FALSE, NULL);
    
    myTextPtr -> selFrom = myOrigSelFrom;
    myTextPtr -> selTo = myOrigSelTo;
    myTextPtr -> locationOK = FALSE;
    
    if (myAction & (KEY_LONGEST_LINE_CHANGED | KEY_NUM_LINES_CHANGED))
    {
    	MDIOWinText_SynchScrollBarsToText (pmWindow);
    }

    if (myAction & KEY_REDRAW_FROM_SELECTION)
    {
        MIOWin_CaretDisplay (pmWindow);
    	MDIOWinText_DisplayText (pmWindow, FALSE, ALL_LINES, ALL_LINES);
    }
    else if (myAction & KEY_UPDATE_LINE)
    {
        MIOWin_CaretDisplay (pmWindow);
	MIOWinText_GetCurrentLineCol (pmWindow, &myLine, &myCol);
    	MDIOWinText_DisplayText (pmWindow, FALSE, myLine, myLine);
    }

    if (myAtEnd)
    {    
    	MyScrollToSelection (pmWindow, SCROLL_NORMAL, FALSE, TRUE);
    }

    // We check to see if we interrupted some input.
    if (pmFlag == MARK_END_OF_OUTPUT_POSITION)
    {
    	myInfo -> startInputRow = MIOWinText_GetCurrentTextPos (pmWindow);
    }           
} // MIOWinText_AddText


/************************************************************************/
/* MIOWinText_ButtonPressHandler					*/
/************************************************************************/
BOOL	MIOWinText_ButtonPressHandler (WIND pmInnerWindow, int pmX, int pmY, 
				       BOOL pmShift)
{
    MIOWinInfoPtr	myInfo;

    MIOWin_AssertWindowType (pmInnerWindow, WINDOW_TYPE_MIO_TEXT_INNER);
    
    // Returns true if mouse down in text region
    if (!MIOWinText_MouseDownHandler (pmInnerWindow, pmX, pmY, pmShift, FALSE))
    {
	return FALSE;
    }

    myInfo = MIOWin_GetInfo (pmInnerWindow);

    MDIOWinText_CaptureMouse (pmInnerWindow);
    MDIOWinText_SetScrollTimer (pmInnerWindow);

    myInfo -> verticalTicks = 0;
    myInfo -> horizontalTicks = 0;
    myInfo -> selectingWithMouse = TRUE;

    if (((UINT) (MDIOWin_GetMessageTime () - myInfo -> lastClickTime) >
		 MDIOWin_GetDoubleClickTime ()) || 
		 !myInfo -> clickOnSameChar)
    {
	myInfo -> numClicks = 1;
    }
    else
    {
        myInfo -> numClicks++;
    }
    myInfo -> lastClickTime = GetMessageTime ();

    if (myInfo -> numClicks == 2)
    {
        MyMouseExtendSelectionToWord (pmInnerWindow);
    }
    else if (myInfo -> numClicks == 3)
    {
        MyMouseExtendSelectionToLine (pmInnerWindow);
    }
    else if (myInfo -> numClicks == 4)
    {
        myInfo -> numClicks = 1;
    }

    return TRUE;
} // MIOWinText_ButtonPressHandler


/************************************************************************/
/* MIOWinText_Copy							*/
/************************************************************************/
void	MIOWinText_Copy (WIND pmWindow)
{
    TextPtr	myTextPtr;
    int		mySelectionLen;
    int		myFromLine, myFromCol, myToLine, myToCol;
    char	*mySrc, *myEnd;
    
    MIOWin_AssertWindowType (pmWindow, WINDOW_TYPE_MIO_TEXT);
    
    myTextPtr = MyGetTextPtr (pmWindow);
    
    // We need to convert the \n's to \r\n's in the clipboard.
    mySelectionLen = abs (myTextPtr -> selTo - myTextPtr -> selFrom);
    
    if (mySelectionLen == 0)
    {
    	MDIO_Beep ();
    	return;
    }

    EdText_GetSelectionLinesCols (myTextPtr, &myFromLine, &myFromCol,
    				  &myToLine, &myToCol);
    
    //    				 
    // Copy the text to a global handle
    //
    
    mySrc = myTextPtr -> text + min (myTextPtr -> selFrom, myTextPtr -> selTo);
    myEnd = myTextPtr -> text + max (myTextPtr -> selFrom, myTextPtr -> selTo);

    MDIOWinText_CopyToClipboard (pmWindow, 
			         mySelectionLen + myToLine - myFromLine + 1, 
			         mySrc, myEnd);
} // MIOWinText_Copy


/************************************************************************/
/* MIOWinText_CreateWindow						*/
/************************************************************************/
WIND	MIOWinText_CreateWindow (WindowAttribPtr pmWindowAttribs)
{
    return MDIOWinText_CreateWindow (pmWindowAttribs);
} // MIOWinText_CreateWindow

			      
/************************************************************************/
/* MIOWinText_DeleteText						*/
/************************************************************************/
void	MIOWinText_DeleteText (WIND pmWindow, int pmLocation)
{
    MIOWinInfoPtr	myInfo;
    TextPtr		myTextPtr;
    int 		myOrigSelFrom, myOrigSelTo;
    int			myTextPos;
    BOOL		myAtEnd;
    int			myAction;
   
    MIOWin_AssertWindowType (pmWindow, WINDOW_TYPE_MIO_TEXT);
    
    myInfo = MIOWin_GetInfo (pmWindow);
    myTextPtr = myInfo -> text;
    myTextPos = MIOWinText_GetCurrentTextPos (pmWindow);

    if ((myInfo -> topLine <= myTextPtr -> numLines) &&
    	(myTextPtr -> numLines < myInfo -> topLine + myInfo -> windowHeight))
    {
    	myAtEnd = TRUE;
    }
    else
    {
    	myAtEnd = FALSE;
    }        
    
    myOrigSelFrom = myTextPtr -> selFrom;
    myOrigSelTo = myTextPtr -> selTo;
    
    myTextPtr -> selFrom = pmLocation;
    myTextPtr -> selTo = myTextPos;
    myTextPtr -> locationOK = FALSE;
    
    myAction = EdText_SelectionDelete (myTextPtr);
    
    myTextPtr -> selFrom = myOrigSelFrom;
    myTextPtr -> selTo = myOrigSelTo;
    myTextPtr -> locationOK = FALSE;
    
    if (myAction & (KEY_LONGEST_LINE_CHANGED | KEY_NUM_LINES_CHANGED))
    {
    	MDIOWinText_SynchScrollBarsToText (pmWindow);
    }

    if (myAction & KEY_REDRAW_FROM_SELECTION)
    {
        MIOWin_CaretDisplay (pmWindow);
    	MDIOWinText_DisplayText (pmWindow, FALSE, ALL_LINES, ALL_LINES);
    }
    else if (myAction & KEY_UPDATE_LINE)
    {
    	int	myLine, myCol;
    	
        MIOWin_CaretDisplay (pmWindow);
        MIOWinText_GetCurrentLineCol (pmWindow, &myLine, &myCol);
    	MDIOWinText_DisplayText (pmWindow, FALSE, myLine, myLine);
    }

    if (myAtEnd)
    {    
    	MyScrollToSelection (pmWindow, SCROLL_NORMAL, FALSE, TRUE);
    }
} // MIOWinText_DeleteText


/************************************************************************/
/* MIOWinText_GetCurrentLineCol						*/
/************************************************************************/
void	MIOWinText_GetCurrentLineCol (WIND pmWindow, int *pmLine, int *pmCol)
{
    TextPtr	myTextPtr = MyGetTextPtr (pmWindow);
    	
    MIOWin_AssertWindowType (pmWindow, WINDOW_TYPE_MIO_TEXT);
    
    if (myTextPtr -> newLineAdded)
    {
	*pmLine = myTextPtr -> numLines - 1;
	*pmCol = myTextPtr -> lineStarts [*pmLine + 1] -
	         myTextPtr -> lineStarts [*pmLine] - 1;
    }
    else
    {
	*pmLine = myTextPtr -> numLines;
	*pmCol = 0;
    }
} // MIOWinText_GetCurrentLineCol


/************************************************************************/
/* MIOWinText_GetCurrentTextPos						*/
/************************************************************************/
int	MIOWinText_GetCurrentTextPos (WIND pmWindow)
{
    TextPtr	myTextPtr = MyGetTextPtr (pmWindow);
    	
    MIOWin_AssertWindowType (pmWindow, WINDOW_TYPE_MIO_TEXT);
    
    if (myTextPtr -> newLineAdded)
    {
    	return myTextPtr -> textLen - 1;
    }
    else
    {
    	return myTextPtr -> textLen;
    }
} // MIOWinText_GetCurrentTextPos


/************************************************************************/
/* MIOWinText_IsAtBeginningOfLine					*/
/************************************************************************/
BOOL	MIOWinText_IsAtBeginningOfLine (WIND pmWindow)
{
    TextPtr	myTextPtr = MyGetTextPtr (pmWindow);
    int		myCurrentPos;
	
    MIOWin_AssertWindowType (pmWindow, WINDOW_TYPE_MIO_TEXT);
    
    myCurrentPos = MIOWinText_GetCurrentTextPos (pmWindow);
    return ((myCurrentPos == 0) ||
 	    (myTextPtr -> text [myCurrentPos - 1] == '\n'));
} // MIOWinText_IsAtBeginningOfLine


/************************************************************************/
/* MIOWinText_MouseDownHandler						*/
/************************************************************************/
BOOL	MIOWinText_MouseDownHandler (WIND pmInnerWindow, int pmX, int pmY, 
				  BOOL pmShift, BOOL pmAllowOutOfBounds)
{
    MIOWinInfoPtr	myInfo;
    MYRECT		myRect;
    
    MIOWin_AssertWindowType (pmInnerWindow, WINDOW_TYPE_MIO_TEXT_INNER);
    
    myInfo = MIOWin_GetInfo (pmInnerWindow);
    myRect = myInfo -> textRect;
    
    // Adjust the mouse position so that if it is in the margins, we move it
    // into the text rectangle
    if ((0 <= pmX) && (pmX < myRect.left))
    {
    	pmX = myRect.left;
    }
    if ((0 <= pmY) && (pmY < myRect.top))
    {
    	pmY = myRect.top;
    }
    
    if (MyMouseInTextRect (pmInnerWindow, pmX, pmY) || pmAllowOutOfBounds)
    {
    	int 	myLine;
    	int 	myCol;
    	BOOL	myRedraw;

	pmX = min (max (pmX, myRect.left), myRect.right);
	pmY = min (max (pmY, myRect.top), myRect.bottom);
    	myLine = (pmY - myRect.top) / myInfo -> fontCharHeight;
    	myCol = (pmX - myRect.left + (myInfo -> fontCharWidth / 2)) /
    	    myInfo -> fontCharWidth;
	myCol = min (myCol, myInfo -> windowWidth - 1);
	myLine = min (myLine, myInfo -> windowHeight - 1);
	
	// Check to see if we clicked in the same place as last time
	if ((myInfo -> lastMouseCol == myCol) && 
	    (myInfo -> lastMouseLine == myLine))
	{
	    myInfo -> clickOnSameChar = TRUE;
	}
	else
	{
	    myInfo -> clickOnSameChar = FALSE;
	}
	myInfo -> lastMouseCol = myCol;
	myInfo -> lastMouseLine = myLine;
	
	if (pmShift)
	{
	    int	myStatus;
	    
	    myStatus = EdText_SelectExtendSelection (myInfo -> text, 
    	        myLine + myInfo -> topLine, myCol + myInfo -> leftCol);
      	    
    	    if (myStatus == HIDE_SELECTION)
    	    {
    	        MIOWin_CaretDisplay (pmInnerWindow);
    	    }
    	    else if (myStatus == CREATE_SELECTION)
    	    {
    	    	MIOWin_CaretHide (pmInnerWindow);
    	    }
    	    MyScrollToSelection (pmInnerWindow, SCROLL_NORMAL, 
    	        myStatus != NO_REDRAW, FALSE);
	}
	else
	{
     	    if (EdText_SetSelection (myInfo -> text, 
    	        myLine + myInfo -> topLine, myCol + myInfo -> leftCol))
      	    {
   	        myRedraw = TRUE;
    	        MIOWin_CaretDisplay (pmInnerWindow);
      	    }
    	    MIOWin_CaretMove (pmInnerWindow);
    	    MyScrollToSelection (pmInnerWindow, SCROLL_NORMAL, myRedraw,
    	    			 FALSE);
        }
        
    	return TRUE;
    }
    else
    {
    	return FALSE;
    }
} // MIOWinText_MouseDownHandler


/************************************************************************/
/* MIOWinText_PrintWindow						*/
/************************************************************************/
void	MIOWinText_PrintWindow (WIND pmWindow)
{
    TextPtr	myTextPtr = MyGetTextPtr (pmWindow);

    MIOWin_AssertWindowType (pmWindow, WINDOW_TYPE_MIO_TEXT);
    
    if (!EdText_IsEmpty (myTextPtr))
    {
    	EdInt_SubmitPrintJob (pmWindow, myTextPtr -> text, 
    	    		      myTextPtr -> textLen,
			      MIOWin_GetInfo (pmWindow) -> title);
    }	            
    else
    {
    	EdInt_Message1 (pmWindow, 0, IDS_MIO_EMPTY_FILE_TITLE,
    	    	        IDS_MIO_NOTHING_TO_PRINT);
    }
} // MIOWinText_PrintWindow


/************************************************************************/
/* MIOWinText_SaveWindowToFile						*/
/************************************************************************/
void	MIOWinText_SaveWindowToFile (WIND pmWindow, const char *pmPathName)
{
    TextPtr	myTextPtr = MyGetTextPtr (pmWindow);

    MIOWin_AssertWindowType (pmWindow, WINDOW_TYPE_MIO_TEXT);
    
    if ((!EdText_IsEmpty (myTextPtr)) || (pmPathName != NULL))
    {
    	MDIOWinText_SaveWindowToFile (pmWindow, pmPathName); 
    }
    else
    {
    	EdInt_Message1 (pmWindow, 0, IDS_MIO_EMPTY_FILE_TITLE,
    	    	        IDS_MIO_NOTHING_TO_SAVE);
    }
} // MIOWinText_SaveWindowToFile


/************************************************************************/
/* MIOWinText_SelectAll							*/
/************************************************************************/
void	MIOWinText_SelectAll (WIND pmWindow)
{
    int		myStatus;

    MIOWin_AssertWindowType (pmWindow, WINDOW_TYPE_MIO_TEXT);
    
    myStatus = EdText_SelectAll (MyGetTextPtr (pmWindow));
    if (myStatus == HIDE_SELECTION)
    {
        MIOWin_CaretDisplay (pmWindow);
    }
    else if (myStatus == CREATE_SELECTION)
    {
    	MIOWin_CaretHide (pmWindow);
    }
    MyScrollToSelection (pmWindow, SCROLL_NORMAL, 
        myStatus != NO_REDRAW, FALSE);
} // MIOWinText_SelectAll


/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/
/************************************************************************/
/* MyGetTextPtr								*/
/************************************************************************/
static TextPtr	MyGetTextPtr (WIND pmInnerOrOuterWindow)
{
    return MIOWin_GetInfo (pmInnerOrOuterWindow) -> text;
} // MyGetTextPtr


/************************************************************************/
/* MyMouseInTextRect							*/
/************************************************************************/
static BOOL	MyMouseInTextRect (WIND pmInnerOrOuterWindow, int pmX, int pmY)
{
    MYRECT	myRect;
    
    myRect = MIOWin_GetInfo (pmInnerOrOuterWindow) -> textRect;
    
    return ((myRect.left <= pmX) && (pmX <= myRect.right) &&
        (myRect.top <= pmY) && (pmY <= myRect.bottom));
} // MyMouseInTextRect


/************************************************************************/
/* MyMouseExtendSelectionToLine						*/
/************************************************************************/
static void	MyMouseExtendSelectionToLine (WIND pmInnerWindow)
{
    int	myStatus;
	    
    MIOWin_AssertWindowType (pmInnerWindow, WINDOW_TYPE_MIO_TEXT_INNER);
    
    myStatus = EdText_SelectLine (MyGetTextPtr (pmInnerWindow));
      	    
    if (myStatus == HIDE_SELECTION)
    {
        MIOWin_CaretDisplay (pmInnerWindow);
    }
    else if (myStatus == CREATE_SELECTION)
    {
    	MIOWin_CaretHide (pmInnerWindow);
    }
    MyScrollToSelection (pmInnerWindow, SCROLL_NORMAL, 
        myStatus != NO_REDRAW, FALSE);
} // MyMouseExtendSelectionToLine


/************************************************************************/
/* MyMouseExtendSelectionToWord						*/
/************************************************************************/
static void	MyMouseExtendSelectionToWord (WIND pmInnerWindow)
{
    int	myStatus;
	    
    MIOWin_AssertWindowType (pmInnerWindow, WINDOW_TYPE_MIO_TEXT_INNER);
    
    myStatus = EdText_SelectWord (MyGetTextPtr (pmInnerWindow));
      	    
    if (myStatus == HIDE_SELECTION)
    {
        MIOWin_CaretDisplay (pmInnerWindow);
    }
    else if (myStatus == CREATE_SELECTION)
    {
    	MIOWin_CaretHide (pmInnerWindow);
    }
    MyScrollToSelection (pmInnerWindow, SCROLL_NORMAL, 
        myStatus != NO_REDRAW, FALSE);
} // MyMouseExtendSelectionToWord


/************************************************************************/
/* MyScrollToSelection							*/
/************************************************************************/
static void	MyScrollToSelection (WIND pmInnerOrOuterWindow, 
				     int pmSkipDistance, BOOL pmRedraw,
				     BOOL pmScrollToEnd)
// One DEFAULT, DOWN_PAGE, UP_PAGE, SCROLL_TOP_OF_SCREEN, SCROLL_BOTTOM_OF_SCREEN,
{
    MIOWinInfoPtr	myInfo;
    TextPtr		myTextPtr;
    HWND		myWindow;
    int			myLine, myCol;
    int			myFirstVisibleLine, myLastVisibleLine;
    int			myFirstVisibleCol,myLastVisibleCol;
    int			myNewTopLine, myNewLeftCol;
    int			myHeight, myWidth;
    int			myMaxLine, myMaxCol;
    int			myPageDistance;
    
    myInfo = MIOWin_GetInfo (pmInnerOrOuterWindow);
    myTextPtr = (TextPtr) myInfo -> text;
    myWindow = (HWND) myInfo -> outerWindow;
    myNewTopLine = myInfo -> topLine;
    myNewLeftCol = myInfo -> leftCol;
    myHeight = myInfo -> windowHeight;
    myWidth = myInfo -> windowWidth;
    myMaxLine = max (0, myTextPtr -> numLines - (myHeight - 2));
    myMaxCol = max (0, myTextPtr -> longestLineLen - (myWidth - 2));
    myPageDistance = myHeight - 2;

    if (pmScrollToEnd)
    {
	MIOWinText_GetCurrentLineCol (myWindow, &myLine, &myCol);
    }
    else
    {
    	EdText_GetSelectionLocation (myTextPtr, &myLine, &myCol);
    }
    
    myFirstVisibleLine = myInfo -> topLine;
    myLastVisibleLine = myFirstVisibleLine + myHeight - 1;
    myFirstVisibleCol = myInfo -> leftCol;
    myLastVisibleCol = myFirstVisibleCol + myInfo -> windowWidth - 1;
    
    if ((myFirstVisibleLine <= myLine) && (myLine <= myLastVisibleLine) &&
        (myFirstVisibleCol <= myCol) && (myCol <= myLastVisibleCol))
    {
    	// The selection is currently visible
    	if (pmRedraw)
    	{
    	    MDIOWinText_DisplayText (pmInnerOrOuterWindow, FALSE, 
    	    			     ALL_LINES, ALL_LINES);
    	}
    	return;
    }

    // Handle vertical displacement first    
    if ((myFirstVisibleLine <= myLine) && (myLine <= myLastVisibleLine))
    {
    	// Do nothing, and skip all the other checks
    }
    else
    {
    	// If within three lines of the top of the screen, place the 
    	// selection at the top of the screen
    	if ((myFirstVisibleLine - 3 <= myLine) && (myLine < myFirstVisibleLine))
    	{
    	    // Place selection at top of screen
    	    myNewTopLine = myLine;
    	}
    	// If within three lines of the bottom, place the selection at 
    	// the bottom of the screen
    	else if ((myLastVisibleLine < myLine) && 
    	    (myLine <= myLastVisibleLine + 3))
    	{
    	    // Place selection one line from bottom of screen
    	    myNewTopLine = myLine - myHeight + 1;
	}
	else
	{
	    // Place the selection approximately 1/3 from the top of
	    // the screen
	    myNewTopLine = myLine - myHeight / 3;
	}
    }

    // Handle horizontal displacement second    
    if ((myFirstVisibleCol <= myCol) && (myCol <= myLastVisibleCol))
    {
    	// Do nothing, and skip all the other checks
    }
    // If we're within a screen's width of the left side, place it on the
    // left side
    else if (myCol < myWidth)
    {
    	myNewLeftCol = 0;
    }
    else
    {
    	// Otherwise, place the selection in the middle of the screen
    	myNewLeftCol = myCol - myWidth / 2;
    }
    

    // Make certain we're within bounds
    myNewTopLine = max (0, min (myMaxLine, myNewTopLine));
    myNewLeftCol = max (0, min (myMaxCol, myNewLeftCol));
    
    if (pmRedraw || (myNewTopLine != myInfo -> topLine) || 
        (myNewLeftCol != myInfo -> leftCol))
    {
    	myInfo -> topLine = myNewTopLine;
    	myInfo -> leftCol = myNewLeftCol;
    	MIOWin_CaretMove (pmInnerOrOuterWindow);
	MDIOWinText_SynchScrollBarsToText (pmInnerOrOuterWindow);
    	MDIOWinText_DisplayText (pmInnerOrOuterWindow, FALSE, 
    			         ALL_LINES, ALL_LINES);
    }
} // MyScrollToSelection
