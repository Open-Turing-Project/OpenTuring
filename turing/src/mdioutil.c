/************************************************************************/
/* mdioutil.c								*/
/*									*/
/* This module contains extremely platform dependent routines.  Even	*/
/* the ".h" file is platform dependent.					*/
/************************************************************************/

/*******************/
/* System includes */
/*******************/

/****************/
/* Self include */
/****************/
#include "mdioutil.h"

/******************/
/* Other includes */
/******************/

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
/* MDIOUtil_ConvertBitMapToDIB						*/
/*									*/
/* This function can be used in one of four ways.			*/
/* (1) If pmBitMapInfo is NULL, then only pmBitMapInfoSize and		*/
/*     pmBytesSize are assigned values.					*/
/* (2) If pmBitMapInfo is not NULL and pmAlloc is FALSE, then the	*/
/*     bit map header and image bytes are saved in the areas pointed to	*/
/*     by the appropriate parameters.					*/
/* (3) If pmBitMapInfo is not NULL and pmAlloc is TRUE, then 		*/
/*     bit map header and image bytes are saved in the areas allocated	*/
/*     to them by the routine and passed back in the appropriate	*/
/*     parameters.							*/
/* (4) If pmBitMapInfo is not NULL and pmBits is NULL and pmAlloc is 	*/
/*     FALSE, then bit map header and image bytes are saved in the 	*/
/*     areas allocated to pmBitMapInfo, one followed by the other	*/
/*  									*/
/* if pmWidth is 0, then the entire bitmap is converted into a DIB.	*/
/* If pmBitMap is NULL, use the bitmap selected in pmDeviceContext.	*/
/************************************************************************/
BOOL	MDIOUtil_ConvertBitMapToDIB (HDC pmDeviceContext, HBITMAP pmBitMap,
	    HPALETTE pmPalette, int pmX, int pmY, int pmWidth, int pmHeight,
	    BOOL pmAlloc, BITMAPINFO **pmBitMapInfo,
	    int *pmBitMapInfoSize, BYTE **pmBits, int *pmBitsSize)
{
    HBITMAP		myBitMap, myOldBitMap, myOriginalBitMap, mySmallBitMap;
    BITMAP		myActualBitMap;
    int			myBitsPerPixel;
    int			myNumColours, myBytesPerLine, myNumScanLines;
    BITMAPINFOHEADER	*myBitMapInfoHeader;
    BITMAPINFO		*myBitMapInfo;
    HDC			myTempDC;
    HPALETTE		myOldPalette;
    BYTE		*myBits;
    
    //
    // Create the device context from which we will extract the bits.
    //
    myTempDC = CreateCompatibleDC (pmDeviceContext);
    if (myTempDC == NULL) 
    {
	return FALSE;
    }

    // Set the palette of the new device context
    myOldPalette = SelectPalette (myTempDC, pmPalette, FALSE);
    RealizePalette (myTempDC);

    //
    // We are not copying the entire bitmap.  This means we have to create a
    // second bitmap which will contain the bits to be copied.
    //
    if (pmWidth != 0)
    {
	myBitMap = CreateCompatibleBitmap (pmDeviceContext, pmWidth, pmHeight);
	if (myBitMap == NULL) 
	{
    	    SelectPalette (myTempDC, myOldPalette, FALSE);
    	    DeleteDC (myTempDC);
	    
	    return FALSE;
	}

	// Select the newly created bitmap into the newly created DC
	myOldBitMap = SelectBitmap (myTempDC, myBitMap);

	if (pmBitMap != NULL)
	{
	    myOriginalBitMap = SelectBitmap (pmDeviceContext, pmBitMap);
        }

	// Blit the backing store bitmap into the newly created bitmap
	BitBlt (myTempDC, 0, 0, pmWidth, pmWidth, pmDeviceContext, pmX, pmY,
		SRCCOPY);

	if (pmBitMap != NULL)
	{
	    SelectBitmap (pmDeviceContext, myOriginalBitMap);
        }

	// The bitmap with the pixels must not be selected into the DC
	// at this point if we're doing a GetDIBits (see MS doc).
	SelectBitmap (myTempDC, myOldBitMap);
	
	// At this point, pmDeviceContext and pmBitMap are no longer needed
	// and are in original state.
	
	// myTempDC has it's original bit map loaded along with the passed
	// in palette.  myBitMap refers to the created bit map and it is not
	// selected into a DC.
    }
    else
    {
    	//
    	// We are copying the entire bitmap.  
    	//
    	
	// We can use the original device context, since we're going to
	// use the entire screen.
	// Since pmBitMap is selected into pmDeviceContext, and this isn't
	// allowed during a GetDIBits.
	if (pmBitMap != NULL)
	{
	    myBitMap = pmBitMap;
        }
	else
	{
	    mySmallBitMap = CreateCompatibleBitmap (pmDeviceContext, 1, 1);
	    myBitMap = SelectBitmap (pmDeviceContext, mySmallBitMap);
	}
	
	// At this point, pmDeviceContext is being used and has a small bit
	// map in it.  To clean up, it will be necessary to put the original
	// bit map back (myBitMap) and delete the small bit map.
    }

    //
    // Get information about the bitmap in order to create a BITMAPHEADERINFO
    //

    // Fill in BITMAP structure, return NULL if it didn't work
    if (!GetObject (myBitMap, sizeof (myActualBitMap), (LPSTR) &myActualBitMap))
    {
	// Clean up
        if (pmWidth != 0)
	{
    	    DeleteBitmap (myBitMap);
	}
	else if (pmBitMap == NULL)
	{
    	    // Restore the original bit map to the pmDeviceContext
    	    SelectBitmap (pmDeviceContext, myBitMap);
    	    DeleteBitmap (mySmallBitMap);
	}
    	SelectPalette (myTempDC, myOldPalette, FALSE);
    	DeleteDC (myTempDC);

	return FALSE;
    }

    // Calculate bits per pixel
    myBitsPerPixel = myActualBitMap.bmPlanes * myActualBitMap.bmBitsPixel;

    // Make sure bits per pixel is valid, force over 8 to 24 bits
    if (myBitsPerPixel <= 1) myBitsPerPixel = 1;
    else if (myBitsPerPixel <= 4) myBitsPerPixel = 4;
    else if (myBitsPerPixel <= 8) myBitsPerPixel = 8;
    else myBitsPerPixel = 24;

    // Calculate the size of the BITMAPINFO
    if (myBitsPerPixel <= 8)
    {
    	myNumColours = 1 << myBitsPerPixel;
    }
    else
    {
    	myNumColours = 0;
    }
    *pmBitMapInfoSize = sizeof (BITMAPINFO) + 
    			(myNumColours - 1) * sizeof(RGBQUAD);
    myBitMapInfoHeader = (BITMAPINFOHEADER *) malloc (*pmBitMapInfoSize);
    
    ZeroMemory (myBitMapInfoHeader, sizeof (BITMAPINFOHEADER));
    myBitMapInfoHeader -> biSize = sizeof (BITMAPINFOHEADER);
    myBitMapInfoHeader -> biWidth = myActualBitMap.bmWidth;
    myBitMapInfoHeader -> biHeight = myActualBitMap.bmHeight;
    myBitMapInfoHeader -> biPlanes = 1;
    myBitMapInfoHeader -> biBitCount = myBitsPerPixel;
    myBitMapInfoHeader -> biCompression = BI_RGB;

    //
    // Okay, now get the BITMAPINFOHEADER information about the new bitmap
    //
    myNumScanLines = GetDIBits (myTempDC, myBitMap, 0, 
				myBitMapInfoHeader -> biHeight, NULL, 
		     	        (BITMAPINFO *) myBitMapInfoHeader, 
		     	        DIB_RGB_COLORS);
    if (myNumScanLines == 0)
    {
	// Clean up
        if (pmWidth != 0)
	{
    	    DeleteBitmap (myBitMap);
	}
	else if (pmBitMap == NULL)
	{
    	    // Restore the original bit map to the pmDeviceContext
    	    SelectBitmap (pmDeviceContext, myBitMap);
    	    DeleteBitmap (mySmallBitMap);
	}
    	SelectPalette (myTempDC, myOldPalette, FALSE);
    	DeleteDC (myTempDC);
	
    	return FALSE;
    }

    // Calculate the size of the bitmap
    if (myBitMapInfoHeader -> biSizeImage == 0)
    {
	int	myBitsPerLine;

	myBitsPerLine = (myBitMapInfoHeader -> biWidth * myBitsPerPixel);
	// Align myBytesPerLine to DWORD boundary
	myBytesPerLine = ((myBitsPerLine + 31) & ~31) >> 3;
	*pmBitsSize = myBytesPerLine * myBitMapInfoHeader -> biHeight;
    }
    else
    {
	*pmBitsSize = myBitMapInfoHeader -> biSizeImage;
    }

    //
    // If this is case (1), return now
    //
    if (pmBitMapInfo == NULL)
    {
        if (pmWidth != 0)
	{
    	    DeleteBitmap (myBitMap);
	}
	else if (pmBitMap == NULL)
	{
    	    // Restore the original bit map to the pmDeviceContext
    	    SelectBitmap (pmDeviceContext, myBitMap);
    	    DeleteBitmap (mySmallBitMap);
	}
    	SelectPalette (myTempDC, myOldPalette, FALSE);
    	DeleteDC (myTempDC);
    
    	return TRUE;
    }
    
    //
    // Allocate the appropriate memory, if necessary
    //
    if (pmAlloc)
    {
    	*pmBitMapInfo = (BITMAPINFO *) malloc (*pmBitMapInfoSize);
    	if (*pmBitMapInfo == NULL)
    	{
    	    return FALSE;
    	}
    	*pmBits = (BYTE *) malloc (*pmBitsSize);
    	if (*pmBits == NULL)
    	{
    	    return FALSE;
    	}
    }


    // Okay, now copy the bits to the structure.
    if (pmBits == NULL)
    {
	//
        // The bloody call returns NOTHING (no error, NOTHING), if myBits
	// isn't 16-byte aligned!!!!  So we get the first 8-byte aligned
	// address after the bitmap.  This occurred in 32-bit Win2K
	//
    	myBits = sizeof (int) + (char *) *pmBitMapInfo + *pmBitMapInfoSize;
	// Align to next 16-byte boundary
	myBits = (char *) ((int) (myBits + 0xF) & ~0xF);

    	// The first entry is actually the offset to the bits
    	** (int **) pmBitMapInfo = (char *) myBits - (char *) *pmBitMapInfo;
    	myBitMapInfo = (BITMAPINFO *) ((char *) *pmBitMapInfo + sizeof (int));
    }
    else
    {
    	myBits = *pmBits;
    	myBitMapInfo = *pmBitMapInfo;
    }

    // Copy all the information from the myBitMapInfoHeader structure into
    // the pmBitMapInfo.
    memcpy (myBitMapInfo, myBitMapInfoHeader, sizeof (BITMAPINFOHEADER));
        
    myNumScanLines = GetDIBits (myTempDC, myBitMap, 0, 
				myBitMapInfoHeader -> biHeight, myBits, 
    			        myBitMapInfo, DIB_RGB_COLORS);
    if (myNumScanLines == 0)
    {
    	return FALSE;
    }

    // Clean up
    if (pmWidth != 0)
    {
    	DeleteBitmap (myBitMap);
    }
    else if (pmBitMap == NULL)
    {
    	// Restore the original bit map to the pmDeviceContext
    	SelectBitmap (pmDeviceContext, myBitMap);
    	DeleteBitmap (mySmallBitMap);
    }
    SelectPalette (myTempDC, myOldPalette, FALSE);
    DeleteDC (myTempDC);
    
    return TRUE;
} // MDIOUtil_ConvertBitMapTODIB


/************************************************************************/
/* OkMsgBox								*/
/*									*/
/* Here because it is used in fileman.c!				*/
/************************************************************************/
void	OkMsgBox (const char *pmMessage)
{
    MessageBox (GetDesktopWindow(), pmMessage, "Turing Debug", MB_OK);
} // OkMsgBox


/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/


