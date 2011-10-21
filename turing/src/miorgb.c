/************/
/* miorgb.c */
/************/

/*******************/
/* System includes */
/*******************/

/****************/
/* Self include */
/****************/
#include "miorgb.h"

/******************/
/* Other includes */
/******************/
#include "mdiorgb.h"

#include "mio.h"

#include "edint.h"

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
static BOOL	MyCreateNewPaletteIfNecessary (MIOWinInfoPtr pmInfo);


/*********************************************/
/* External procedures for Turing predefines */
/*********************************************/
/************************************************************************/
/* MIORGB_AddColour							*/
/************************************************************************/
int	MIORGB_AddColour (OOTreal pmRed, OOTreal pmGreen, OOTreal pmBlue)
{
    MIOWinInfoPtr	myInfo = MIO_selectedRunWindowInfo;
    int			myNewColour;
    
    MIO_CheckOuputIsToWindow ("RGB.AddColor/AddColour");
    MIO_CheckOuputWindowIsInGraphicsMode ("RGB.AddColor/AddColour");

    // 
    // Make certain the palette will hold the extra colour
    //
    if (myInfo -> numColours == 1024)
    {
	SET_ERRNO (E_RGB_TABLE_FULL);
	return MDIORGB_GetNearestColourInPalette (myInfo, pmRed, pmGreen, 
						  pmBlue);
    }

    //
    // Check all colour components are in range
    //
    if (pmRed < 0.0 || pmRed > 1.0) 
    {
	SET_ERRNO (E_RGB_RED_OUT_OF_RANGE);
	return 0;
    }

    if (pmGreen < 0.0 || pmGreen > 1.0) 
    {
	SET_ERRNO (E_RGB_GREEN_OUT_OF_RANGE);
	return 0;
    }

    if (pmBlue < 0.0 || pmBlue > 1.0) 
    {
	SET_ERRNO (E_RGB_BLUE_OUT_OF_RANGE);
	return 0;
    }
    
    // Create a palette for this window, if it does not already have one.
    if (!MyCreateNewPaletteIfNecessary (myInfo))
    {
	return MDIORGB_GetNearestColourInPalette (myInfo, pmRed, 
						  pmGreen, pmBlue);
    }
    
    // Check to see if we already have a match and return it if we do.
    myNewColour = MDIORGB_GetMatchingColourInPalette (myInfo, pmRed, pmGreen, 
    						      pmBlue);
    if (myNewColour != -1)
    {
    	return myNewColour;
    }    						          						      
	    
    myNewColour = MDIORGB_AddColour (pmRed, pmGreen, pmBlue);
    
    if (myNewColour == -1)
    {
    	SET_ERRNO (E_RGB_FAILURE);
	return MDIORGB_GetNearestColourInPalette (myInfo, pmRed, pmGreen, 
						  pmBlue);
    }
    
    return myNewColour;    	
} // MIORGB_AddColour


/************************************************************************/
/* MIORGB_GetColour							*/
/************************************************************************/
void	MIORGB_GetColour (OOTint pmColourNum, OOTreal *pmRed, 
			  OOTreal *pmGreen, OOTreal *pmBlue)
{
    MIO_CheckOuputIsToWindow ("RGB.GetColor/GetColour");
    MIO_CheckOuputWindowIsInGraphicsMode ("RGB.GetColor/GetColour");

    //
    // This routine should return the actual colour being displayed rather 
    // than the colour in the palette.
    //
    MDIORGB_GetColour (MDIORGB_ConvertColour (pmColourNum), pmRed, pmGreen, 
    		       pmBlue);
} // MIORGB_GetColour


/************************************************************************/
/* MIORGB_GetMaxColour							*/
/************************************************************************/
int	MIORGB_GetMaxColour (void)
{
    MIO_CheckOuputIsToWindow ("maxcolor/maxcolour");
    MIO_CheckOuputWindowIsInGraphicsMode ("maxcolor/maxcolour");
    
    return MIO_selectedRunWindowInfo -> numColours - 1;
} // MIORGB_GetMaxColour


/************************************************************************/
/* MIORGB_SetColour							*/
/************************************************************************/
void	MIORGB_SetColour (OOTint pmColourNum, OOTreal pmRed, OOTreal pmGreen, 
			  OOTreal pmBlue)
{
    MIO_CheckOuputIsToWindow ("RGB.SetColor/SetColour");
    MIO_CheckOuputWindowIsInGraphicsMode ("RGB.SetColor/SetColour");

    if ((pmColourNum < 0) || 
    	(pmColourNum >= MIO_selectedRunWindowInfo -> numColours))
    {
    	SET_ERRNO (E_RGB_BAD_NUM);
    	return;
    }    	

    // Create a palette for this window, if it does not already have one.
    if (!MyCreateNewPaletteIfNecessary (MIO_selectedRunWindowInfo))
    {
	return;
    }
    
    if (!MDIORGB_SetColour (pmColourNum, pmRed, pmGreen, pmBlue))
    {
    	SET_ERRNO (E_RGB_FAILURE);
    }
} // MIORGB_SetColour


/***************************************/
/* External procedures for MIO library */
/***************************************/
/************************************************************************/
/* MIORGB_ConvertColour							*/
/************************************************************************/
COLOR	MIORGB_ConvertColour (OOTint pmClr)
{
    return MDIORGB_ConvertColour (pmClr);
} // MIORGB_ConvertColour


/************************************************************************/
/* MIORGB_GetScreenDepth						*/
/************************************************************************/
int	MIORGB_GetScreenDepth (void)
{
    return MDIORGB_GetScreenDepth ();
} // MIORGB_GetScreenDepth


/************************************************************************/
/* MIORGB_WhatDotColor							*/
/************************************************************************/
int	MIORGB_WhatDotColor (MIOWinInfoPtr pmInfo, int pmX, int pmY)
{
    return MDIORGB_WhatDotColor (pmInfo, pmX, pmY);
} // MIORGB_WhatDotColor


/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/
/************************************************************************/
/* MyCreateNewPaletteIfNecessary					*/
/************************************************************************/
static BOOL	MyCreateNewPaletteIfNecessary (MIOWinInfoPtr pmInfo)
{
    // Create a palette for this window, if it does not already have one.
    if (pmInfo -> usingDefaultPalette)
    {
	PALETTE	myNewPalette = MDIORGB_CreateNewPalette (pmInfo -> palette);

	if (myNewPalette == NULL)
	{
	    SET_ERRNO (E_RGB_FAILURE);
	    return FALSE;
	}
    	pmInfo -> palette = myNewPalette;
    	pmInfo -> usingDefaultPalette = FALSE;
    	pmInfo -> numPaletteSlots = pmInfo -> numColours;
    	MDIORGB_UseNewPalette (pmInfo);
    }
    
    return TRUE;
} // MyCreateNewPaletteIfNecessary

