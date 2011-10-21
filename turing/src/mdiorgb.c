/*************/
/* mdiorgb.c */
/*************/

/*******************/
/* System includes */
/*******************/
#include <windows.h>

/****************/
/* Self include */
/****************/
#include "mdiorgb.h"

/******************/
/* Other includes */
/******************/
#include "mio.h"

#include "mdiowingraph.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
#define PALETTE_GROW_SIZE	256

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
static PBITMAPINFO	MyCreateBitmapInfo (HBITMAP pmBitMap);
static COLORREF	MyGetColourFromOOTRGB (OOTreal pmRed, OOTreal pmGreen, 
				       OOTreal pmBlue);


/***********************/
/* External procedures */
/***********************/
/************************************************************************/
/* MDIORGB_AddColour							*/
/*									*/
/* At this point, we are assured that the window is not using the	*/
/* default colour palette.						*/
/************************************************************************/
int	MDIORGB_AddColour (OOTreal pmRed, OOTreal pmGreen, OOTreal pmBlue)
{
    MIOWinInfoPtr	myInfo = MIO_selectedRunWindowInfo;
    int			myColourNumber;

    if (myInfo -> numPaletteSlots == myInfo -> numColours)
    {
    	// We need to expand the palette
    	myInfo -> numPaletteSlots += PALETTE_GROW_SIZE;
        if (!ResizePalette (myInfo -> palette, myInfo -> numPaletteSlots))
        {
            return -1;
        }
    }
    
    myColourNumber = myInfo -> numColours;
    if (MDIORGB_SetColour (myColourNumber, pmRed, pmGreen, pmBlue))
    {
    	myInfo -> numColours++;
    	return (myColourNumber);
    }
    else
    {
    	return -1;
    }    		           
} // MDIORGB_AddColour


/************************************************************************/
/* MDIORGB_ConvertColour						*/
/************************************************************************/
COLOR	MDIORGB_ConvertColour (OOTint pmClrNum)
{
    COLORREF	myBaseColour, myDesiredColour;
    BYTE	myRed, myGreen, myBlue;
    
    if (!MIO_selectedRunWindowInfo -> xor)
    {
    	return (COLOR) PALETTEINDEX (pmClrNum);
    }
    
    // Gets ugly - Handle XOR
    
    // The main problem is that Windows can draw in XOR mode, but it uses the
    // pixel values.  This means that if we want XOR to work with a particular
    // colour value (i.e. drawing on top of colour x comes out in the original
    // colour, then one a direct colour display we need to XOR the RGB of the 
    // colour we want to draw with the RGB of colour x, and then draw the line
    // in XOR mode in the resulting colour.
    
    // With a CLUT display, it's even more complicated.  We need to find out
    // the index to the actual display CLUT used by colour x. We then XOR it 
    // with the index of the colour we're drawing in. We then get an index 
    // number to the actual display CLUT that we want to use. We then have to 
    // figure out what RGB or palette index will correspond to that index in
    // the display CLUT.
    
    // For direct colour
    myBaseColour = GetNearestColor (
    				MIO_selectedRunWindowInfo -> deviceContext,
    				(COLORREF) PALETTEINDEX (0));
    myDesiredColour = GetNearestColor (
    				MIO_selectedRunWindowInfo -> deviceContext,
    				(COLORREF) PALETTEINDEX (pmClrNum));
    myRed = GetRValue (myBaseColour) ^ GetRValue (myDesiredColour);   				
    myGreen = GetGValue (myBaseColour) ^ GetGValue (myDesiredColour);   				
    myBlue = GetBValue (myBaseColour) ^ GetBValue (myDesiredColour);   				
    
    return (COLOR) RGB (myRed, myGreen, myBlue); 
} // MDIORGB_ConvertColour


/************************************************************************/
/* MDIORGB_CreateNewPalette						*/
/*									*/
/* Create a new palette that is identical to the default palette.	*/
/************************************************************************/
PALETTE	MDIORGB_CreateNewPalette (PALETTE pmDefaultPalette)
{
    HPALETTE	myPalette;
    WORD	myNumDefaultPaletteEntries;
    LOGPALETTE	*myNewLogPalette;

    GetObject (pmDefaultPalette, sizeof (WORD), &myNumDefaultPaletteEntries);
    myNewLogPalette = malloc (sizeof (LOGPALETTE) + 
			  sizeof(PALETTEENTRY) * myNumDefaultPaletteEntries);
    // TW Check for NULL
    myNewLogPalette -> palVersion = 0x300;
    myNewLogPalette -> palNumEntries = myNumDefaultPaletteEntries;
    GetPaletteEntries (pmDefaultPalette, 0, myNumDefaultPaletteEntries,
    		       &(myNewLogPalette -> palPalEntry [0]));
    		       
    myPalette = CreatePalette (myNewLogPalette);
    // TW Check for successful creation
    
    free (myNewLogPalette);
    
    return myPalette;
} // MDIORGB_CreateNewPalette


/************************************************************************/
/* MDIORGB_GetColour							*/
/*									*/
/* The number specified here is an Windows color.  This is likely to be	*/
/* actually a reference to a palette, rather than a colour specifiying	*/
/* the RGB (in Windows, a COLORREF can be either an index to a palette	*/
/* or RGB values.							*/
/************************************************************************/
void	MDIORGB_GetColour (COLOR pmColour, OOTreal *pmRed, OOTreal *pmGreen, 
			   OOTreal *pmBlue)
{
    COLORREF	myColourDisplayed;
    
    myColourDisplayed = GetNearestColor (
    				MIO_selectedRunWindowInfo -> deviceContext,
    				(COLORREF) pmColour);

    *pmRed = (double) GetRValue (myColourDisplayed) / 255;   				
    *pmGreen = (double) GetGValue (myColourDisplayed) / 255;   				
    *pmBlue = (double) GetBValue (myColourDisplayed) / 255;   				
} // MDIORGB_GetColour

			   
/************************************************************************/
/* MDIORGB_GetMatchingColourInPalette					*/
/*									*/
/* Return the index of the palette entry that exactly matches the color	*/
/* passed in.  If no colour exactly matches, then return -1.		*/
/************************************************************************/
int	MDIORGB_GetMatchingColourInPalette (MIOWinInfoPtr pmInfo, OOTreal pmRed, 
					    OOTreal pmGreen, OOTreal pmBlue)
{
    int			myRed, myBlue, myGreen;
    COLORREF		myColour;
    PALETTEENTRY	myPaletteEntry;
    int			myClosestColour;
    
    
    myRed = (int) (pmRed * 255 + 0.5);
    myGreen = (int) (pmGreen * 255 + 0.5);
    myBlue = (int) (pmBlue * 255 + 0.5);
    
    myColour = RGB (myRed, myGreen, myBlue);
    myClosestColour = GetNearestPaletteIndex (pmInfo -> palette, myColour);
    if (GetPaletteEntries (pmInfo -> palette, myClosestColour, 1, 
    			   &myPaletteEntry) == 0)
    {
    	return -1;
    }
    
    if ((myRed == myPaletteEntry.peRed) &&
    	(myGreen == myPaletteEntry.peGreen) &&
    	(myBlue == myPaletteEntry.peBlue))
    {
    	return myClosestColour;
    }
    
    return -1;
} // MDIORGB_GetNearestColourInPalette


/************************************************************************/
/* MDIORGB_GetNearestColourInPalette					*/
/************************************************************************/
int	MDIORGB_GetNearestColourInPalette (MIOWinInfoPtr pmInfo, OOTreal pmRed, 
					   OOTreal pmGreen, OOTreal pmBlue)
{
    COLORREF	myColour = MyGetColourFromOOTRGB (pmRed, pmGreen, pmBlue);
    
    return GetNearestPaletteIndex (pmInfo -> palette, myColour);
} // MDIORGB_GetNearestColourInPalette


/************************************************************************/
/* MDIORGB_GetScreenDepth						*/
/************************************************************************/
int	MDIORGB_GetScreenDepth (void)
{
    HDC	myDC;
    int	myBitsPerPixel;

    myDC = GetDC (NULL);
    myBitsPerPixel = GetDeviceCaps (myDC, BITSPIXEL);
    ReleaseDC (NULL, myDC);

    return myBitsPerPixel;
} // MDIORGB_GetScreenDepth


/************************************************************************/
/* MDIORGB_SetColour							*/
/*									*/
/* At this point, we are assured that the window is not using the	*/
/* default colour palette.						*/
/*									*/
/* We're going to require extensive reworking in order get RGB.SetColor	*/
/* to actually remap colours that are on the screen.  For the moment,   */
/* any new drawing done in the changed colour appears in the new colour.*/
/************************************************************************/
BOOL	MDIORGB_SetColour (OOTint pmClrNum, OOTreal pmRed, OOTreal pmGreen, 
			   OOTreal pmBlue)
{
    PALETTEENTRY 	myPaletteEntry;
    MIOWinInfoPtr	myInfo = MIO_selectedRunWindowInfo;
 
/*   
    BITMAPINFO		myBitMapInfo;
    PBITMAPINFOHEADER	myBitMapInfoHeader;
    void		*myBits;

    ZeroMemory (&myBitMapInfo, sizeof(myBitMapInfo));
    myBitMapInfo.bmiHeader.biSize = sizeof (myBitMapInfo.bmiHeader);
    GetDIBits (myInfo -> offscreenDeviceContext, myInfo -> offscreenBitMap,
	0, 0, NULL, &myBitMapInfo, DIB_PAL_COLORS);
    myBitMapInfoHeader = (BITMAPINFOHEADER *) malloc (sizeof (BITMAPINFO) +
						      sizeof (RGBQUAD) * 256);
    // Duplicate the bitmapinfoheader information into the new struct.
    memcpy(myBitMapInfoHeader , &myBitMapInfo, sizeof(myBitMapInfo.bmiHeader));
    myBits = malloc (myBitMapInfoHeader -> biSizeImage);
    GetDIBits (myInfo -> offscreenDeviceContext, myInfo -> offscreenBitMap,
	0, 1, myBits, &myBitMapInfo, DIB_PAL_COLORS);

/*
1. To fill in a BITMAPINFOHEADER struct. Call Thus:

  BITMAPINFO bmi;

  Here GetDIBits will use the inititlized bmiSize member to determine
  which structure it should be writing to. It will fill in the
  BITMAPINFOHEADER struct with the size and characteristics of the
  bitmap.
    myBitMapInfo = MyCreateBitmapInfo (myInfo -> offscreenBitMap);
    if (myBitMapInfo == NULL)
    {
    	// Error Message
    	return FALSE;
    }
    
    myBitMapInfoHeader = (PBITMAPINFOHEADER) myBitMapInfo;
    myBits = malloc (myBitMapInfoHeader -> biSizeImage);
    if (myBits == NULL)
    {
    	// Error Message
    	return FALSE;
    }

    // Retrieve the color table (RGBQUAD array) and the bits 
    // (array of palette indices) from the DIB. 
//    if (!GetDIBits (myInfo -> deviceContext, myInfo -> offscreenBitMap, 0, 
//    		    (WORD) myBitMapInfoHeader -> biHeight, myBits, myBitMapInfo, 
//		    DIB_RGB_COLORS)) 
//    {
//    	// Error Message
//    	return FALSE;
//    }
*/
/*****/    

    // Now set the new palette entry
    myPaletteEntry.peRed = (int) (pmRed * 255 + 0.5);
    myPaletteEntry.peGreen = (int) (pmGreen * 255 + 0.5);
    myPaletteEntry.peBlue  = (int) (pmBlue * 255 + 0.5);
    myPaletteEntry.peFlags = 0;

    if (SetPaletteEntries (myInfo -> palette, pmClrNum, 1, 
    		           &myPaletteEntry) == 0)
    {
    	return FALSE;
    }    		           

    MDIORGB_UseNewPalette (myInfo);
    RealizePalette (myInfo -> deviceContext);
    RealizePalette (myInfo -> offscreenDeviceContext);
    
    return TRUE;
} // MDIORGB_SetColour


/************************************************************************/
/* MDIORGB_UseNewPalette						*/
/************************************************************************/
void	MDIORGB_UseNewPalette (MIOWinInfoPtr pmInfo)
{
    SelectPalette ((HDC) pmInfo -> deviceContext, pmInfo -> palette, FALSE);
    SelectPalette ((HDC) pmInfo -> offscreenDeviceContext, pmInfo -> palette, 
    		   FALSE);
} // MDIORGB_UseNewPalette


/************************************************************************/
/* MDIORGB_WhatDotColor							*/
/************************************************************************/
int	MDIORGB_WhatDotColor (MIOWinInfoPtr pmInfo, int pmX, int pmY)
{
    COLORREF	myColour = GetPixel ((HDC) pmInfo -> offscreenDeviceContext, 
    				     pmX, pmY);
    
    return GetNearestPaletteIndex (pmInfo -> palette, myColour);
} // MDIORGB_WhatDotColor


/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/
/************************************************************************/
/* MyCreateBitmapInfo							*/
/************************************************************************/
static PBITMAPINFO	MyCreateBitmapInfo (HBITMAP pmBitMap)
{ 
    BITMAP	myBitMap;
    PBITMAPINFO myBitMapInfo; 
    WORD	myClrBits; 

    // Retrieve the bitmap's color format, width, and height. 
    if (!GetObject (pmBitMap, sizeof(BITMAP), (LPSTR) &myBitMap)) 
    {
    	// TW Handle Error
    	return NULL;
    }

    // Convert the color format to a count of bits. 
    myClrBits = (WORD) (myBitMap.bmPlanes * myBitMap.bmBitsPixel); 
    if (myClrBits == 1) 
        myClrBits = 1; 
    else if (myClrBits <= 4) 
        myClrBits = 4; 
    else if (myClrBits <= 8) 
        myClrBits = 8; 
    else if (myClrBits <= 16) 
        myClrBits = 16; 
    else if (myClrBits <= 24) 
        myClrBits = 24; 
    else 
    	myClrBits = 32; 

    // Allocate memory for the BITMAPINFO structure. (This structure 
    // contains a BITMAPINFOHEADER structure and an array of RGBQUAD 
    // data structures.) 
    if (myClrBits != 24) 
    {
        myBitMapInfo = (PBITMAPINFO) malloc (sizeof(BITMAPINFOHEADER) + 
			                    sizeof(RGBQUAD) * (1 << myClrBits)); 
    }
    else 
    {
     	// There is no RGBQUAD array for the 24-bit-per-pixel format. 
        myBitMapInfo = (PBITMAPINFO) malloc (sizeof(BITMAPINFOHEADER)); 
    }        
    if (myBitMapInfo == NULL)
    {
    	return NULL;
    }
    
    // Initialize the fields in the BITMAPINFO structure. 
    myBitMapInfo -> bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
    myBitMapInfo -> bmiHeader.biWidth = myBitMap.bmWidth; 
    myBitMapInfo -> bmiHeader.biHeight = myBitMap.bmHeight; 
    myBitMapInfo -> bmiHeader.biPlanes = myBitMap.bmPlanes; 
    myBitMapInfo -> bmiHeader.biBitCount = myBitMap.bmBitsPixel; 
    if (myClrBits < 24) 
        myBitMapInfo -> bmiHeader.biClrUsed = (1 << myClrBits); 

    // If the bitmap is not compressed, set the BI_RGB flag. 
    myBitMapInfo -> bmiHeader.biCompression = BI_RGB; 

    // Compute the number of bytes in the array of color 
    // indices and store the result in biSizeImage. 
    // For Windows NT/2000, the width must be DWORD aligned unless 
    // the bitmap is RLE compressed. This example shows this. 
    // For Windows 95/98, the width must be WORD aligned unless the 
    // bitmap is RLE compressed.
    myBitMapInfo -> bmiHeader.biSizeImage = 
    	((myBitMapInfo->bmiHeader.biWidth * myClrBits + 31) & ~31) / 8
                                  * myBitMapInfo->bmiHeader.biHeight; 
    // Set biClrImportant to 0, indicating that all of the 
    // device colors are important. 
    myBitMapInfo->bmiHeader.biClrImportant = 0; 
    
    return myBitMapInfo; 
} // MyCreateBitmapInfo


/************************************************************************/
/* MyGetColourFromOOTRGB						*/
/************************************************************************/
static COLORREF	MyGetColourFromOOTRGB (OOTreal pmRed, OOTreal pmGreen, 
				       OOTreal pmBlue)
{
    int		myRed, myBlue, myGreen;
    
    myRed = (int) (pmRed * 255 + 0.5);
    myGreen = (int) (pmGreen * 255 + 0.5);
    myBlue = (int) (pmBlue * 255 + 0.5);
    
    return RGB (myRed, myGreen, myBlue);
} // MyGetColourFromOOTRGB

