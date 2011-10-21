/*************/
/* miotext.c */
/*************/

/*******************/
/* System includes */
/*******************/

/****************/
/* Self include */
/****************/
#include "miotext.h"

/******************/
/* Other includes */
/******************/
#include "mio.h"
#include "miowin.h"

#include "mdio.h"
#include "miodraw.h"

#include "mioerr.h"

// Test to make certain we're not accidentally including <windows.h> which
// might allow for windows contamination of platform independent code.
#ifdef _INC_WINDOWS
xxx
#endif

/**********/
/* Macros */
/**********/

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


/***********************/
/* External procedures */
/***********************/
/************************************************************************/
/* MIOText_Cls								*/
/************************************************************************/
void	MIOText_Cls (void)
{
    MIOWinInfoPtr	myInfo = MIOWin_GetInfo (MIO_selectedRunWindow);
    BOOL		myOldXOR;
    
    MIO_CheckOuputIsToWindow ("Text.Cls");
    MIO_CheckOuputWindowIsInGraphicsMode ("Text.Cls");
    MIO_MakePopupWindowVisible ();
    
    // Implemented simply as a drawfillbox of the entire screen in the
    // text background color
    myOldXOR = myInfo -> xor;
    myInfo -> xor = FALSE;
    MIODraw_Box (0, 0, myInfo -> width - 1, myInfo -> height - 1, NO_STYLE,
    		 NORMAL_WIDTH, myInfo -> textBGColour, FILL);
    myInfo -> xor = myOldXOR;

    // Set the cursor positon to upper-left corner
    myInfo -> actualCol = 0;
    myInfo -> actualRow = 0;
    myInfo -> row = 0;
    myInfo -> col = 0;
    myInfo -> startInputRow = 0;
    myInfo -> startInputCol = 0;
    
    // Display the caret, if appropriate
    MIOWin_CaretDisplay (MIO_selectedRunWindow);
} // MIOText_Cls


/************************************************************************/
/* MIOText_GetCurrentCol						*/
/************************************************************************/
int	MIOText_GetCurrentCol (void)
{
    MIO_CheckOuputIsToWindow ("Text.WhatCol");
    MIO_CheckOuputWindowIsInGraphicsMode ("Text.WhatCol");

    return MIOWin_GetInfo (MIO_selectedRunWindow) -> actualCol + 1;
} // MIOText_GetCurrentCol


/************************************************************************/
/* MIOText_GetCurrentColour						*/
/************************************************************************/
int	MIOText_GetCurrentColour (void)
{
    MIO_CheckOuputIsToWindow ("Text.WhatColor");
    MIO_CheckOuputWindowIsInGraphicsMode ("Text.WhatColor");

    return MIOWin_GetInfo (MIO_selectedRunWindow) -> textColour;
} // MIOText_GetCurrentColour


/************************************************************************/
/* MIOText_GetCurrentColourBack						*/
/************************************************************************/
int	MIOText_GetCurrentColourBack (void)
{
    MIO_CheckOuputIsToWindow ("Text.WhatColorBack");
    MIO_CheckOuputWindowIsInGraphicsMode ("Text.WhatColorBack");

    return MIOWin_GetInfo (MIO_selectedRunWindow) -> textBGColour;
} // MIOText_GetCurrentColourBack


/************************************************************************/
/* MIOText_GetCurrentRow						*/
/************************************************************************/
int	MIOText_GetCurrentRow (void)
{
    MIO_CheckOuputIsToWindow ("Text.WhatRow");
    MIO_CheckOuputWindowIsInGraphicsMode ("Text.WhatRow");

    return MIOWin_GetInfo (MIO_selectedRunWindow) -> actualRow + 1;
} // MIOText_GetCurrentRow


/************************************************************************/
/* MIOText_GetMaxCol							*/
/************************************************************************/
int	MIOText_GetMaxCol (void)
{
    MIO_CheckOuputIsToWindow ("maxcol");
    MIO_CheckOuputWindowIsInGraphicsMode ("maxcol");

    return MIOWin_GetInfo (MIO_selectedRunWindow) -> maxCol;
} // MIOText_GetMaxCol


/************************************************************************/
/* MIOText_GetMaxRow							*/
/************************************************************************/
int	MIOText_GetMaxRow (void)
{
    MIO_CheckOuputIsToWindow ("maxrow");
    MIO_CheckOuputWindowIsInGraphicsMode ("maxrow");

    return MIOWin_GetInfo (MIO_selectedRunWindow) -> maxRow;
} // MIOText_GetMaxRow


/************************************************************************/
/* MIOText_Locate							*/
/************************************************************************/
void	MIOText_Locate (OOTint pmRow, OOTint pmCol)
{
    MIOWinInfoPtr	myInfo = MIOWin_GetInfo (MIO_selectedRunWindow);
    char		myMessage [1024];

    MIO_CheckOuputIsToWindow ("Text.Locate");
    MIO_CheckOuputWindowIsInGraphicsMode ("Text.Locate");

    if (pmRow < 1) 
    {
	MDIO_sprintf (myMessage, "Row of %d is less than 1", pmRow);
	ABORT_WITH_ERRMSG (E_TEXT_ROW_TOO_SMALL, myMessage);
    }
    if (pmRow > myInfo -> maxRow) 
    {
	MDIO_sprintf (myMessage, "Row of %d is greater than maxrow (%d)", 
		      pmRow, myInfo -> maxRow);
	ABORT_WITH_ERRMSG (E_TEXT_ROW_TOO_LARGE, myMessage);
    }
    if (pmCol < 1) 
    {
	MDIO_sprintf (myMessage, "Column of %d is less than 1", pmCol);
	ABORT_WITH_ERRMSG (E_TEXT_COL_TOO_SMALL, myMessage);
    }
    if (pmCol > myInfo -> maxCol)
    {
	MDIO_sprintf (myMessage, "Column of %d is greater than maxcol (%d)", 
		      pmCol, myInfo -> maxCol);
	ABORT_WITH_ERRMSG (E_TEXT_COL_TOO_LARGE, myMessage);
    }
    
    myInfo -> row = pmRow - 1;
    myInfo -> col = pmCol - 1;
    myInfo -> actualRow = myInfo -> row;
    myInfo -> actualCol = myInfo -> col;
    
    // Display the caret, if appropriate
    MIOWin_CaretDisplay (MIO_selectedRunWindow);
} // MIOText_Locate


/************************************************************************/
/* MIOText_LocateXY							*/
/************************************************************************/
void	MIOText_LocateXY (OOTint pmX, OOTint pmY)
{
    MIOWinInfoPtr	myInfo = MIOWin_GetInfo (MIO_selectedRunWindow);
    int			myRow, myCol;
    
    MIO_CheckOuputIsToWindow ("Text.LocateXY");
    MIO_CheckOuputWindowIsInGraphicsMode ("Text.LocateXY");
 
    myRow = (myInfo -> height - pmY - 1) / myInfo -> fontCharHeight + 1;
    myCol = pmX / myInfo -> fontCharWidth + 1;
    
    MIOText_Locate (myRow, myCol);
} // MIOText_LocateXY


/************************************************************************/
/* MIOText_SetBackgroundColour						*/
/************************************************************************/
void	MIOText_SetBackgroundColour (OOTint pmClr)
{
    MIOWinInfoPtr	myInfo = MIOWin_GetInfo (MIO_selectedRunWindow);
    
    MIO_CheckOuputIsToWindow ("Text.ColorBack");
    MIO_CheckOuputWindowIsInGraphicsMode ("Text.ColorBack");
    MIO_CheckColourRange (pmClr);        

    myInfo -> textBGColour = pmClr;
} // MIOText_SetBackgroundColour


/************************************************************************/
/* MIOText_SetColour							*/
/************************************************************************/
void	MIOText_SetColour (OOTint pmClr)
{
    MIOWinInfoPtr	myInfo = MIOWin_GetInfo (MIO_selectedRunWindow);
    
    MIO_CheckOuputIsToWindow ("Text.Color");
    MIO_CheckOuputWindowIsInGraphicsMode ("Text.Color");
    MIO_CheckColourRange (pmClr);        

    myInfo -> textColour = pmClr;
} // MIOText_SetColour


/************************************************************************/
/* MIOText_WhatChar							*/
/************************************************************************/
void	MIOText_WhatChar (void)
{
    ABORT_WITH_ERRNO (E_TEXT_WHATTEXTCHAR_DEAD);
} // MIOText_WhatChar


/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/


