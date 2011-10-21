/*****************/
/* miowingraph.c */
/*****************/

/*******************/
/* System includes */
/*******************/

/****************/
/* Self include */
/****************/
#include "miowingraph.h"

/******************/
/* Other includes */
/******************/
#include "mio.h"
#include "mdio.h"

#include "miowin.h"
#include "mdiowin.h"
#include "mdiowingraph.h"

#include "edint.h"

// Test to make certain we're not accidentally including <windows.h> which
// might allow for windows contamination of platform independent code.
#ifdef _INC_WINDOWS
xxx
#endif

/**********/
/* Macros */
/**********/
#define TAB_SIZE		8

/*************/
/* Constants */
/*************/

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
static void	MyAddCharacter (WIND pmWindow, char pmChar);


/***********************/
/* External procedures */
/***********************/
/************************************************************************/
/* MIOWinGraph_Init							*/
/************************************************************************/
void	MIOWinGraph_Init (void)
{
} // MIOWinGraph_Init


/************************************************************************/
/* MIOWinGraph_Finalize							*/
/************************************************************************/
void	MIOWinGraph_Finalize (void)
{
} // MIOWinGraph_Finalize


/************************************************************************/
/* MIOWinGraph_AddText							*/
/************************************************************************/
void	MIOWinGraph_AddText (WIND pmWindow, const char *pmBuffer, int pmFlag)
{
    MIOWinInfoPtr	myInfo = MIOWin_GetInfo (pmWindow);
    const char		*myStart = pmBuffer;
    char 		myDestBuffer [256];
    const char		*mySrc;
    char		*myDest;
    
    MIOWin_AssertWindowType (pmWindow, WINDOW_TYPE_MIO_GRAPHICS);

    for (;;)
    {
    	mySrc = myStart;
    	myDest = myDestBuffer;

	while ((*mySrc != '\b') && (*mySrc != '\t') && (*mySrc != '\n') && 
	       (*mySrc != 0) && 
	       ((mySrc - myStart) < (myInfo -> maxCol - myInfo -> col)))
	{
	    *myDest++ = *mySrc++;
	}
	*myDest = 0;

	/* Draw the string */
	if ((mySrc - myStart) > 0)
	{
	    MDIOWinGraph_PutString (pmWindow, myDestBuffer, (mySrc - myStart),
	    			    myInfo -> row, myInfo -> col);

	    myInfo -> col += mySrc - myStart;
	    myInfo -> actualCol = myInfo -> col;
	}

	if (*mySrc == 0)
	{
	    break;
	}

	// Handle special characters, including characters that make go
	// to the next line
	MyAddCharacter (pmWindow, *mySrc);
	
	myStart = mySrc + 1;
    } // for
    
    // We check to see if we interrupted some input.
    if (pmFlag == MARK_END_OF_OUTPUT_POSITION)
    {
    	myInfo -> startInputRow = myInfo -> actualRow;
    	myInfo -> startInputCol = myInfo -> actualCol;
    }           
    
    // Set cursor position
    MIOWin_CaretMove (pmWindow);
} // MIOWinGraph_AddText

				     
/************************************************************************/
/* MIOWinGraph_CreateWindow						*/
/************************************************************************/
WIND	MIOWinGraph_CreateWindow (WindowAttribPtr pmWindowAttribs)
{
    return MDIOWinGraph_CreateWindow (pmWindowAttribs);
} // MIOWinGraph_CreateWindow

			      	  
/************************************************************************/
/* MIOWinGraph_DeleteText						*/
/************************************************************************/
void	MIOWinGraph_DeleteText (WIND pmWindow, int pmRow, int pmCol)
{
    MIOWinInfoPtr	myInfo = MIOWin_GetInfo (pmWindow);
    int			cnt;
        
    // Draw blank spaces from (pmRow, pmCol) (inclusive) to
    // (actualRow, actualCol) (exclusive).
    if (myInfo -> actualRow == pmRow)
    {
    	MDIOWinGraph_ClearText (pmWindow, pmRow, pmCol, myInfo -> actualCol);
    }
    else
    {
	MDIOWinGraph_ClearToEOL (pmWindow, pmRow, pmCol);
	for (cnt = pmRow + 1 ; cnt < myInfo -> actualRow ; cnt++)
	{
	    MDIOWinGraph_ClearToEOL (pmWindow, cnt, 0);
	}
	MDIOWinGraph_ClearText (pmWindow, pmRow, 0, myInfo -> actualCol);
    }
    
    myInfo -> row = pmRow;
    myInfo -> col = pmCol;
    myInfo -> actualRow = pmRow;
    myInfo -> actualCol = pmCol;
    
    // Move the caret (if appropriate) to the new location.
    MIOWin_CaretDisplay (pmWindow);
} // MIOWinGraph_DeleteText


/************************************************************************/
/* MIOWinGraph_PrintWindow						*/
/************************************************************************/
void	MIOWinGraph_PrintWindow (WIND pmWindow)
{
    MIOWinInfoPtr   myInfo = MIOWin_GetInfo (pmWindow);
    PALETTE	    myPalette;

    MIOWin_AssertWindowType (pmWindow, WINDOW_TYPE_MIO_GRAPHICS);
    
    myPalette = myInfo -> palette;

    EdInt_PrintGraphics (pmWindow, myInfo -> offscreenBitMap,
        myInfo -> width, myInfo -> height, myPalette, 
	80 * myInfo -> fontCharWidth, TRUE, TRUE, TRUE, TRUE, 
        myInfo -> title);
} // MIOWinGraph_PrintWindow


/************************************************************************/
/* MIOWinGraph_SaveWindowToFile						*/
/************************************************************************/
void	MIOWinGraph_SaveWindowToFile (WIND pmWindow, const char *pmPathName)
{
    MIOWin_AssertWindowType (pmWindow, WINDOW_TYPE_MIO_GRAPHICS);

    MDIOWinGraph_SaveWindowToFile (pmWindow, pmPathName);
} // MIOWinGraph_SaveWindowToFile


/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/
/************************************************************************/
/* MyAddCharacter							*/
/************************************************************************/
static void	MyAddCharacter (WIND pmWindow, char pmChar)
{
    MIOWinInfoPtr	myInfo = MIOWin_GetInfo (pmWindow);
    int			myNumSpaces;
    int			cnt;
    
    switch (pmChar)
    {
	case '\t':
	    myNumSpaces = TAB_SIZE - (myInfo -> col % TAB_SIZE) + 1;

	    for (cnt = 0 ; cnt < myNumSpaces ; cnt++)
	    {
		MyAddCharacter (pmWindow, ' ');
	    }
	    return;
	case '\b':
	    //
	    // Erase the character before the cursor
	    //
	    if (myInfo -> col > 0) 
	    {
	    	myInfo -> col--;
	    }
 	    else
    	    {
		if (myInfo -> row > 0)
		{
		    myInfo -> row--;
		    myInfo -> col = myInfo -> maxCol - 1;
		}
	    }
	    myInfo -> actualRow = myInfo -> row;
	    myInfo -> actualCol = myInfo -> col;

	    MDIOWinGraph_PutChar (pmWindow, ' ', myInfo -> row, myInfo -> col);

	    if ((myInfo -> col == 0) && 
	    	(myInfo -> row != myInfo -> startInputRow))
	    {
		myInfo -> col = myInfo -> maxCol;
		myInfo -> row--;
	    }
	    break;
	case '\n':
	    if (myInfo -> col <= myInfo -> maxCol - 1)
	    {
		if (myInfo -> clearToEndOfLine)
		{
		    MDIOWinGraph_ClearToEOL (pmWindow, myInfo -> row, 
					     myInfo -> col);
		}
	    }
	    if (myInfo -> row < myInfo -> maxRow - 1)
	    {
		myInfo -> col = 0;
		myInfo -> row++;
		myInfo -> actualRow = myInfo -> row;
		myInfo -> actualCol = myInfo -> col;
	    }
	    else
	    {
		MDIOWinGraph_ScrollUpALine (pmWindow);

		myInfo -> startInputRow--;
		myInfo -> col = 0;
		myInfo -> actualCol = myInfo -> col;
	    }
	    break;
	default:
	    if (myInfo -> col < myInfo -> maxCol)
	    {
		MDIOWinGraph_PutChar (pmWindow, pmChar, myInfo -> row, 
				      myInfo -> col);

		if (myInfo -> col < myInfo -> maxCol - 1)
		{
		    myInfo -> col++;
		    myInfo -> actualCol = myInfo -> col;
		}
		else
		{
		    if (myInfo -> row < myInfo -> maxRow - 1)
		    {
			myInfo -> col++;
			myInfo -> actualCol = 0;
			myInfo -> actualRow++;
		    }
		    else
		    {
			myInfo -> col++;
		    }
		}
	    }
	    else 
	    {
		if (myInfo -> row < myInfo -> maxRow - 1)
		{
		    myInfo -> row++;
		}
		else
		{
		    MDIOWinGraph_ScrollUpALine (pmWindow);
		    myInfo -> startInputRow--;
		}
		MDIOWinGraph_PutChar (pmWindow, pmChar, myInfo -> row, 0);
		myInfo -> col = 1;
		myInfo -> actualRow = myInfo -> row;
		myInfo -> actualCol = myInfo -> col;
	    }
	    break;
    } // switch
    
    // Set cursor position
    MIOWin_CaretMove (pmWindow);
} // MyAddCharacter



