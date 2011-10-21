/************************************************************************/
/* mdiopic.c 								*/
/*									*/
/* Pic's created using Pic.New and imported using Pic.FileNew use the	*/
/* DDB (HBITMAP) format.  They are already set to work with the screen	*/
/* device context.  The TakePic/DrawPic stuff uses a DIB because you	*/
/* determine exactly how large a DIB is and save it in an array.  This	*/
/* also means that if people save a takepic array it will be usable on 	*/
/* a different system.							*/
/*									*/
/* A PIC is actually a reference to a two element array.  The first	*/
/* element points to the DDB (HBITMAP) of the picture, the second	*/
/* starts as NULL and points (if necessary) to the mask created when	*/
/* the picture is displayed using picMerge.				*/
/************************************************************************/

/*******************/
/* System includes */
/*******************/
#include <windows.h>
#include <windowsx.h>
#include <math.h>

/****************/
/* Self include */
/****************/
#include "mdiopic.h"

#include "miowin.h"

#include "mio.h"

#include "mdio.h"

#include "edint.h"

#include "mioerr.h"

#include "miorgb.h"

#include "mdioutil.h"

/******************/
/* Other includes */
/******************/

/**********/
/* Macros */
/**********/
#define PI		(3.14159265)
#define RAD(a)		(((double) a) * PI / 180.0)

/*************/
/* Constants */
/*************/
#define B16_5_5_5	1
#define B16_5_6_5	2
#define B24		3
#define B32		4

/********************/
/* Global variables */
/********************/

/*********/
/* Types */
/*********/
typedef struct MDPicInfo
{
    HBITMAP	bmp;
    HBITMAP	mask;
    int		refCount;
} MDPicInfo, *MDPicInfoPtr;

/**********************/
/* External variables */
/**********************/
PIC	MDIOPic_MallocMDInfo (HBITMAP pmBMP1, HBITMAP pmBMP2);

/********************/
/* Static constants */
/********************/

/********************/
/* Static variables */
/********************/
static BOOL	stHasTransparentBlt = FALSE;

/******************************/
/* Static callback procedures */
/******************************/

/*********************/
/* Static procedures */
/*********************/
static void	MyBlend (HBITMAP pmBitMap1, int pmX1, int pmY1, 
			 HBITMAP pmBitMap2, int pmX2, int pmY2, 
			 int pmWidth, int pmHeight, int pmPercent,
			 HDC pmDstDC, HBITMAP pmDstBitMap, 
			 int pmDstX, int pmDstY,
			 int pmWindowWidth, int pmWindowHeight,
			 HDC pmWindowDeviceContext,
			 BOOL pmDisplayOnScreen,
			 HPALETTE pmPalette);
static void	MyBlur (HBITMAP pmBitMap, int pmX, int pmY, 
			int pmWidth, int pmHeight, int pmIterations,
			HDC pmDstDC, HBITMAP pmDstBitMap, 
			int pmDstX, int pmDstY,
			int pmWindowWidth, int pmWindowHeight,
			HDC pmWindowDeviceContext,
			BOOL pmDisplayOnScreen);
static HBITMAP	MyCreateMask (HDC pmDestDC, HDC pmSrcDC, 
			      int pmWidth, int pmHeight,
			      COLORREF pmTransColor);
static void	MyGetColourMasks (BITMAPINFO *pmBMI, 
		    int *pmRedMask, int *pmGreenMask, int *pmBlueMask,
		    int *pmRedShift, int *pmGreenShift, int *pmBlueShift);
static BOOL	MyTransparentBlt (HDC pmDestDC, int pmDestX, int pmDestY,
			          int pmWidth, int pmHeight, HDC pmSourceDC,
			          int pmSourceX, int pmSourceY, 
			          COLORREF pmTransColor, HBITMAP pmMask);
static void	MyGetSinCos (int pmAngle, double *pmSine, double *pmCosine);

/***********************/
/* External procedures */
/***********************/
/************************************************************************/
/* MDIOPic_Init								*/
/************************************************************************/
void	MDIOPic_Init (void)
{
    // Set to true if Win98 or better or Win2K or better
    OSVERSIONINFO	myOSVersionInfo;

    ZeroMemory (&myOSVersionInfo, sizeof (myOSVersionInfo));
    myOSVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx (&myOSVersionInfo);

    if ((myOSVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) &&
        (myOSVersionInfo.dwMinorVersion >= 10))
    {
    	stHasTransparentBlt = TRUE;
    }
    else if ((myOSVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT) &&
             (myOSVersionInfo.dwMajorVersion >= 5))
    {
    	stHasTransparentBlt = TRUE;
    }
    else
    {
    	stHasTransparentBlt = FALSE;
    }
} // MDIOPic_Init


/************************************************************************/
/* MDIOPic_CreatePicByBlend						*/
/************************************************************************/
BOOL	MDIOPic_CreatePicByBlend (PIC pmPic1, PIC pmPic2, 
				  int pmWidth, int pmHeight, 
				  int pmPercent, PIC *pmResultPic)
{
    MIOWinInfoPtr	myInfo = MIO_selectedRunWindowInfo;
    MDPicInfoPtr	*myPicInfo = (MDPicInfoPtr *) pmResultPic;
    MDPicInfoPtr	myPicInfo1 = (MDPicInfoPtr) pmPic1;
    MDPicInfoPtr	myPicInfo2 = (MDPicInfoPtr) pmPic2;
    HBITMAP		myBitMap, myOldBitMap;
    HDC			myDC;
    HPALETTE		myOldPalette;

    myBitMap = CreateCompatibleBitmap (myInfo -> deviceContext, 
	pmWidth, pmHeight);
    if (myBitMap == NULL) 
    {
	MIO_MoreInfo ("MDIOPic_CreatePicByBlend: Problem creating bitmap (%d)",
		      GetLastError ());
	SET_ERRNO (E_PIC_CANT_ALLOC_MEM_FOR_PIC);
        return FALSE;
    }

    myDC = CreateCompatibleDC (myInfo -> deviceContext);
    if (myDC == NULL) 
    {
	MIO_MoreInfo ("GetImage: Problem creating memory context (%d)",
		      GetLastError ());
    	SET_ERRNO (E_PIC_CANT_ALLOC_MEM_FOR_PIC);
	DeleteBitmap (myBitMap);
        return FALSE;
    }

    // Set the new palette
    myOldPalette = SelectPalette (myDC, myInfo -> palette, FALSE);
    RealizePalette (myDC);

    // Select the newly created bitmap into the DC
    myOldBitMap = SelectBitmap (myDC, myBitMap);

    // Create the new image by blending the other two images
    MyBlend (myPicInfo1 -> bmp, 0, 0, myPicInfo2 -> bmp, 0, 0, 
	     pmWidth, pmHeight, pmPercent,
	     myDC, myBitMap, 0, 0,
	     999999, 999999, myInfo -> deviceContext, FALSE,
	     myInfo -> palette);

    // Assign the BitMap to the PIC
    *myPicInfo = (MDPicInfoPtr) MDIOPic_MallocMDInfo (myBitMap, NULL);
    
    // Clean up
    SelectBitmap (myDC, myOldBitMap);
    SelectPalette (myDC, myOldPalette, FALSE);
    DeleteDC (myDC);

    return TRUE;
} // MDIOPic_CreatePicByBlend


/************************************************************************/
/* MDIOPic_CreatePicByBlur						*/
/************************************************************************/
BOOL	MDIOPic_CreatePicByBlur (PIC pmPic, int pmWidth, int pmHeight, 
				 int pmIterations, PIC *pmResultPic)
{
    MIOWinInfoPtr	myInfo = MIO_selectedRunWindowInfo;
    MDPicInfoPtr	*myResultPicInfo = (MDPicInfoPtr *) pmResultPic;
    MDPicInfoPtr	myPicInfo = (MDPicInfoPtr) pmPic;
    HBITMAP		myBitMap, myOldBitMap;
    HDC			myDC;
    HPALETTE		myOldPalette;

    myBitMap = CreateCompatibleBitmap (myInfo -> deviceContext, 
	pmWidth, pmHeight);
    if (myBitMap == NULL) 
    {
	MIO_MoreInfo ("MDIOPic_CreatePicByBlend: Problem creating bitmap (%d)",
		      GetLastError ());
	SET_ERRNO (E_PIC_CANT_ALLOC_MEM_FOR_PIC);
        return FALSE;
    }

    myDC = CreateCompatibleDC (myInfo -> deviceContext);
    if (myDC == NULL) 
    {
	MIO_MoreInfo ("GetImage: Problem creating memory context (%d)",
		      GetLastError ());
    	SET_ERRNO (E_PIC_CANT_ALLOC_MEM_FOR_PIC);
	DeleteBitmap (myBitMap);
        return FALSE;
    }

    // Set the new palette
    myOldPalette = SelectPalette (myDC, myInfo -> palette, FALSE);
    RealizePalette (myDC);

    // Select the newly created bitmap into the DC
    myOldBitMap = SelectBitmap (myDC, myBitMap);

    // Create the new image by blending the other two images
    MyBlur (myPicInfo -> bmp, 0, 0, pmWidth, pmHeight, pmIterations,
	    myDC, myBitMap, 0, 0, 999999, 999999, myInfo -> deviceContext, 
	    FALSE);

    // Assign the BitMap to the PIC
    *myResultPicInfo = (MDPicInfoPtr) MDIOPic_MallocMDInfo (myBitMap, NULL);
    
    // Clean up
    SelectBitmap (myDC, myOldBitMap);
    SelectPalette (myDC, myOldPalette, FALSE);
    DeleteDC (myDC);

    return TRUE;
} // MDIOPic_CreatePicByBlur


/************************************************************************/
/* MDIOPic_CreatePicByDraw						*/
/*									*/
/* This is used to create a PIC (usually used temporarily by		*/
/* MIOPic_DrawSpecial) that is equivalent to the what you would get	*/
/* with a Pic.Draw command.  This allows one to do (for example), a	*/
/*	Pic.DrawSpecial (pic, 100, 200, picMerge, picBlend, 0)		*/
/************************************************************************/
BOOL	MDIOPic_CreatePicByDraw (PIC pmPic, int pmX, int pmY, 
				 int pmWidth, int pmHeight, 
				 int pmTransparentColour, int pmMode,
				 PIC *pmResultPic)
{
    MIOWinInfoPtr	myInfo = MIO_selectedRunWindowInfo;
    MDPicInfoPtr	myPicInfo = (MDPicInfoPtr) pmPic;
    HBITMAP		myNewBitMap, myOldBitMap;
    HBITMAP		myResultBitMap, myOldResultBitMap;
    HDC			myNewDC, myPicDC, myResultDC;
    HPALETTE		myOldPalette, myOldNewPalette, myOldResultPalette;
    HBRUSH		myNewBrush, myOldBrush;
    COLORREF		myTransparentColour;
    int			myNewX = pmX, myNewY = pmY;
    int			myNewWidth = pmWidth, myNewHeight = pmHeight;
    int			mySrcX = 0, mySrcY = 0;
    BOOL		myOffEdge = FALSE;

    myResultBitMap = CreateCompatibleBitmap (myInfo -> deviceContext, 
					     pmWidth, pmHeight);
    if (myResultBitMap == NULL) 
    {
	MIO_MoreInfo ("Draw: Problem creating bitmap (%d)",
		      GetLastError ());
	SET_ERRNO (E_PIC_CANT_ALLOC_MEM_FOR_PIC);
        return FALSE;
    }
    
    myPicDC = CreateCompatibleDC (myInfo -> deviceContext);
    if (myPicDC == NULL) 
    {
	MIO_MoreInfo ("Draw: Problem creating memory context (%d)",
		      GetLastError ());
    	SET_ERRNO (E_PIC_CANT_ALLOC_MEM_FOR_PIC);
    	return FALSE;
    }

    myResultDC = CreateCompatibleDC (myInfo -> deviceContext);
    if (myResultDC == NULL) 
    {
	MIO_MoreInfo ("Draw: Problem creating memory context (%d)",
		      GetLastError ());
    	SET_ERRNO (E_PIC_CANT_ALLOC_MEM_FOR_PIC);
    	return FALSE;
    }

    // Set the new palette
    myOldPalette = SelectPalette (myPicDC, myInfo -> palette, FALSE);
    RealizePalette (myPicDC);

    // Select the newly created bitmap into the temp DC
    myOldBitMap = SelectBitmap (myPicDC, myPicInfo -> bmp);

    // Set the new palette
    myOldResultPalette = SelectPalette (myResultDC, myInfo -> palette, FALSE);
    RealizePalette (myResultDC);

    // Select the newly created bitmap into the temp DC
    myOldResultBitMap = SelectBitmap (myResultDC, myResultBitMap);

    // Adjust coordinates to fit on the output window
    if (pmX < 0)
    {
//        myNewX = 0;
//        mySrcX = -pmX; // Remember: pmX is negative
//	myNewWidth += pmX;
	myOffEdge = TRUE;
    }
    if (pmY < 0)
    {
//        myNewY = 0;
//        mySrcY = -pmY; // Remember: pmX is negative
//	myNewHeight += pmY;
	myOffEdge = TRUE;
    }
    if (myNewWidth > myInfo -> width - myNewX)
    {
//	myNewWidth = max (myInfo -> width - myNewX, 0);
	myOffEdge = TRUE;
    }
    if (myNewHeight > myInfo -> height - myNewY)
    {
//	myNewHeight = max (myInfo -> height - myNewY, 0);
	myOffEdge = TRUE;
    }

    // If some part of this bitmap is off the edge, then blank the
    // bitmap with white.

    if (myOffEdge)
    {
	BitBlt (myResultDC, 0, 0, pmWidth, pmHeight, myPicDC, 
	        0, 0, WHITENESS);
    }

    switch (pmMode)
    {
    	case MODE_COPY:
	    // Blit the picture onto the backing store
	    BitBlt (myResultDC, mySrcX, mySrcY, 
		    myNewWidth, myNewHeight, myPicDC, mySrcX, mySrcY, 
		    SRCCOPY);
	    break;
	    
	case MODE_XOR:
	    // XOR is slightly more difficult because colour 0 in the DC 
	    // palette does not necessarily correspond to colour 0 in the

	    // system palette (in fact it usually doesn't).  So we need to
	    // do an XOR not just between the source and destination, but also
	    // a brush set to DC colour number 0.
	    
	    // Set the brushes to color 0
	    myNewBrush = CreateSolidBrush ((COLORREF) MIORGB_ConvertColour (0));
	    myOldBrush = SelectBrush (myInfo -> deviceContext, myNewBrush);
	    SelectBrush (myInfo -> offscreenDeviceContext, myNewBrush);
	    
	    // Blit the picture onto the backing store
	    BitBlt (myResultDC, mySrcX, mySrcY, 
		    myNewWidth, myNewHeight, myPicDC, mySrcX, mySrcY,
		    0x960169);

	    // Set the original brushes back
	    SelectBrush (myInfo -> deviceContext, myOldBrush);
	    SelectBrush (myInfo -> offscreenDeviceContext, myOldBrush);
	    DeleteBrush (myNewBrush);
	    break;
	    
	case MODE_MERGE:
	    myTransparentColour = 
		(COLORREF) MIORGB_ConvertColour (pmTransparentColour);

	    // Blit the background onto the result
	    BitBlt (myResultDC, mySrcX, mySrcY, myNewWidth, myNewHeight, 
		    myInfo -> offscreenDeviceContext, myNewX, myNewY, SRCCOPY);

	    // Create mask of picture, if necessary
	    if (myPicInfo -> mask == NULL)
	    {
		myPicInfo -> mask = MyCreateMask (myResultDC, myPicDC, 
						  pmWidth, pmHeight,
						  myTransparentColour);
	    }

	    // Merge the picture onto the result
	    MyTransparentBlt (myResultDC, 
			      mySrcX, mySrcY, myNewWidth, myNewHeight, 
			      myPicDC, mySrcX, mySrcY, 
			      myTransparentColour, myPicInfo -> mask);
	    break;
	        	    	        
	case MODE_UNDER_MERGE:
	    // If there's a back surface to be restored, restore it
	    // now (mainly used for drawing multiple frames)
	    BitBlt (myResultDC, mySrcX, mySrcY, 
		    myNewWidth, myNewHeight, myPicDC, mySrcX, mySrcY, 
		    SRCCOPY);
	    myNewBitMap = CreateCompatibleBitmap (myInfo -> deviceContext, 
	    					  myNewWidth, myNewHeight);
	    if (myNewBitMap == NULL) 
	    {
		MIO_MoreInfo ("Draw: Problem creating bit map (%d)",
			      GetLastError ());
		SET_ERRNO (E_PIC_FAILURE);
	        return FALSE;
	    }
	    myNewDC = CreateCompatibleDC (myInfo -> deviceContext);
	    if (myNewDC == NULL) 
	    {
		MIO_MoreInfo ("Draw: Problem [2] creating memory context (%d)", 
			      GetLastError ());
	    	SET_ERRNO (E_PIC_FAILURE);
	    	return FALSE;
	    }

	    // Set the new palette
	    myOldNewPalette = SelectPalette (myNewDC, myInfo -> palette, FALSE);
	    RealizePalette (myNewDC);

	    // Select the newly created bitmap into the DC
	    myOldBitMap = SelectBitmap (myNewDC, myNewBitMap);

	    // Copy the current screen to the new bitmap
	    BitBlt (myNewDC, 0, 0, myNewWidth, myNewHeight, 
	    	    myInfo -> offscreenDeviceContext, myNewX, myNewY, SRCCOPY);
	    	    
	    // Then, copy our picture (without the picture's transparent colour) 
	    // onto the new bitmap
	    myTransparentColour = 
		(COLORREF) MIORGB_ConvertColour (pmTransparentColour);

	    // Create mask of picture, if necessary
	    if (myPicInfo -> mask == NULL)
	    {
		myPicInfo -> mask = MyCreateMask (myNewDC, myPicDC, 
						  pmWidth, pmHeight,
						  myTransparentColour);
	    }

	    MyTransparentBlt (myNewDC, 0, 0, myNewWidth, myNewHeight, 
			      myPicDC, mySrcX, mySrcY, 
			      myTransparentColour, myPicInfo -> mask);
	    
	    // Finally, copy the current screen (without the screen's 
	    // transparent colour) onto the new bitmap
	    myTransparentColour = 
		(COLORREF) MIORGB_ConvertColour (myInfo -> transparentColour);

	    MyTransparentBlt (myNewDC, 0, 0, myNewWidth, myNewHeight, 
	    		      myInfo -> offscreenDeviceContext, 
			      myNewX, myNewY, myTransparentColour, NULL);
	    	    	      
	    // Blit the picture onto the backing store
	    BitBlt (myResultDC, 0, 0,
		    myNewWidth, myNewHeight, myNewDC, 0, 0, SRCCOPY);
	    	    
    	    SelectPalette (myNewDC, myOldNewPalette, FALSE);
	    SelectBitmap (myNewDC, myNewBitMap);
	    DeleteDC (myNewDC);	    	    
	    DeleteBitmap (myNewBitMap);
    } // switch

    // Clean up
    SelectBitmap (myPicDC, myOldBitMap);
    SelectPalette (myPicDC, myOldPalette, FALSE);
    DeleteDC (myPicDC);

    SelectBitmap (myResultDC, myOldResultBitMap);
    SelectPalette (myResultDC, myOldResultPalette, FALSE);
    DeleteDC (myResultDC);

    // Assign the BitMap to the PIC
    *pmResultPic = (MDPicInfoPtr) MDIOPic_MallocMDInfo (myResultBitMap, NULL);

    return TRUE;
} // MDIOPic_CreatePicByDraw


/************************************************************************/
/* MDIOPic_DecrementUse							*/
/************************************************************************/
void	MDIOPic_DecrementUse (PIC pmPic)
{
    MDPicInfoPtr	myPicInfo = (MDPicInfoPtr) pmPic;

    myPicInfo -> refCount--;

    if (myPicInfo -> refCount == 0)
    {
	MDIOPic_FreePic (pmPic);
    }
} // MDIOPic_DecrementUse


/************************************************************************/
/* MDIOPic_DefaultFormat						*/
/************************************************************************/
int	MDIOPic_DefaultFormat (void)
{
    return BMP;
} // MDIOPic_DefaultFormat


/************************************************************************/
/* MDIOPic_Draw								*/
/************************************************************************/
void	MDIOPic_Draw (PIC pmPic, PIC pmBackPic, int pmX, int pmY, int pmWidth, 
		      int pmHeight, int pmTransparentColour, int pmMode)
{
    MIOWinInfoPtr	myInfo = MIO_selectedRunWindowInfo;
    MDPicInfoPtr	myPicInfo = (MDPicInfoPtr) pmPic;
    MDPicInfoPtr	myBackPicInfo = (MDPicInfoPtr) pmBackPic;
    HBITMAP		myNewBitMap, myOldBitMap;
    HDC			myNewDC, myPicDC;
    HPALETTE		myOldPalette, myOldNewPalette;
    HBRUSH		myNewBrush, myOldBrush;
    COLORREF		myTransparentColour;
    // myNew{X,Y} specifies where on the screen to draw the picture
    int			myNewX = pmX, myNewY = pmY;
    // myNew{Width,Height} specifies the width of the picture to be drawn
    int			myNewWidth = pmWidth, myNewHeight = pmHeight;
    // mySrc{X,Y} specifies the point in the picture to draw from
    int			mySrcX = 0, mySrcY = 0;
    
    // Instrumenting for memory and resource usage
//    MDIO_OutputSystemInfo ("Before Draw");
            		   
    myPicDC = CreateCompatibleDC (myInfo -> deviceContext);
    if (myPicDC == NULL) 
    {
	MIO_MoreInfo ("Draw: Problem creating memory context (%d)",
		      GetLastError ());
    	SET_ERRNO (E_PIC_CANT_ALLOC_MEM_FOR_PIC);
    	return;
    }

    // Set the new palette
    myOldPalette = SelectPalette (myPicDC, myInfo -> palette, FALSE);
    RealizePalette (myPicDC);

    // Select the newly created bitmap into the DC
    myOldBitMap = SelectBitmap (myPicDC, myPicInfo -> bmp);

    // Adjust coordinates to fit on the output window
    if (pmX < 0)
    {
        myNewX = 0;
        mySrcX = -pmX; // Remember: pmX is negative
	myNewWidth += pmX;
    }
    if (pmY < 0)
    {
        myNewY = 0;
        mySrcY = -pmY; // Remember: pmX is negative
	myNewHeight += pmY;
    }
    myNewWidth = min (myInfo -> width - myNewX, myNewWidth);
    myNewHeight = min (myInfo -> height - myNewY, myNewHeight);

    switch (pmMode)
    {
    	case MODE_COPY:
	    // Blit the picture onto the screen
	    if (myInfo -> displayOnScreen)
	    {
		BitBlt (myInfo -> deviceContext, myNewX, myNewY, 
		        myNewWidth, myNewHeight, myPicDC, mySrcX, mySrcY,
			SRCCOPY);
	    }
	    // Blit the picture onto the backing store
	    BitBlt (myInfo -> offscreenDeviceContext, myNewX, myNewY, 
		    myNewWidth, myNewHeight, myPicDC, mySrcX, mySrcY, 
		    SRCCOPY);
	    break;
	    
	case MODE_XOR:
	    // XOR is slightly more difficult because colour 0 in the DC 
	    // palette does not necessarily correspond to colour 0 in the
	    // system palette (in fact it usually doesn't).  So we need to
	    // do an XOR not just between the source and destination, but also
	    // a brush set to DC colour number 0.
	    
	    // Set the brushes to color 0
	    myNewBrush = CreateSolidBrush ((COLORREF) MIORGB_ConvertColour (0));
	    myOldBrush = SelectBrush (myInfo -> deviceContext, myNewBrush);
	    SelectBrush (myInfo -> offscreenDeviceContext, myNewBrush);
	    
	    // Blit the picture onto the screen (ROP 0x96 = D ^ P ^ S)
	    if (myInfo -> displayOnScreen)
	    {
		BitBlt (myInfo -> deviceContext, myNewX, myNewY, 
		        myNewWidth, myNewHeight, myPicDC, mySrcX, mySrcY,
			0x960169);
	    }
	    // Blit the picture onto the backing store
	    BitBlt (myInfo -> offscreenDeviceContext, myNewX, myNewY, 
		    myNewWidth, myNewHeight, myPicDC, mySrcX, mySrcY,
		    0x960169);

	    // Set the original brushes back
	    SelectBrush (myInfo -> deviceContext, myOldBrush);
	    SelectBrush (myInfo -> offscreenDeviceContext, myOldBrush);
	    DeleteBrush (myNewBrush);
	    break;
	    
	case MODE_MERGE:
	    myTransparentColour = 
		(COLORREF) MIORGB_ConvertColour (pmTransparentColour);

	    // If there's a back surface to be restored, restore it
	    // now (mainly used for drawing multiple frames)
	    if (myBackPicInfo != NULL)
	    {
	        // Select the newly created bitmap into the DC
	        SelectBitmap (myPicDC, myBackPicInfo -> bmp);
		BitBlt (myInfo -> offscreenDeviceContext, myNewX, myNewY, 
		        myNewWidth, myNewHeight, myPicDC, mySrcX, mySrcY, 
			SRCCOPY);
		SelectBitmap (myPicDC, myPicInfo -> bmp);
	    }

	    // Create mask of picture, if necessary
	    if (myPicInfo -> mask == NULL)
	    {
		myPicInfo -> mask = MyCreateMask (
		    myInfo -> offscreenDeviceContext, myPicDC, 
		    pmWidth, pmHeight, myTransparentColour);
	    }

	    // Blit the picture onto the backing store
	    MyTransparentBlt (myInfo -> offscreenDeviceContext, 
			      myNewX, myNewY, myNewWidth, myNewHeight, 
			      myPicDC, mySrcX, mySrcY, 
			      myTransparentColour, myPicInfo -> mask);
	    if (myInfo -> displayOnScreen)
	    {
		// Blit the picture fromthe backing store onto the screen
		BitBlt (myInfo -> deviceContext, myNewX, myNewY,
		        myNewWidth, myNewHeight, 
			myInfo -> offscreenDeviceContext, myNewX, myNewY,
    	    		SRCCOPY);
	    }
	    break;
	        	    	        
	case MODE_UNDER_MERGE:
	    // If there's a back surface to be restored, restore it
	    // now (mainly used for drawing multiple frames)
	    if (myBackPicInfo != NULL)
	    {
	        // Select the newly created bitmap into the DC
	        SelectBitmap (myPicDC, myBackPicInfo -> bmp);
		BitBlt (myInfo -> offscreenDeviceContext, myNewX, myNewY, 
		        myNewWidth, myNewHeight, myPicDC, mySrcX, mySrcY, 
			SRCCOPY);
		SelectBitmap (myPicDC, myPicInfo -> bmp);
	    }

	    myNewBitMap = CreateCompatibleBitmap (myInfo -> deviceContext, 
	    					  myNewWidth, myNewHeight);
	    if (myNewBitMap == NULL) 
	    {
		MIO_MoreInfo ("Draw: Problem creating bit map (%d)",
			      GetLastError ());
		SET_ERRNO (E_PIC_FAILURE);
	        return;
	    }
	    myNewDC = CreateCompatibleDC (myInfo -> deviceContext);
	    if (myNewDC == NULL) 
	    {
		MIO_MoreInfo ("Draw: Problem [2] creating memory context (%d)", 
			      GetLastError ());
	    	SET_ERRNO (E_PIC_FAILURE);
	    	return;
	    }

	    // Set the new palette
	    myOldNewPalette = SelectPalette (myNewDC, myInfo -> palette, FALSE);
	    RealizePalette (myNewDC);

	    // Select the newly created bitmap into the DC
	    myOldBitMap = SelectBitmap (myNewDC, myNewBitMap);

	    // Copy the current screen to the new bitmap
	    BitBlt (myNewDC, 0, 0, myNewWidth, myNewHeight, 
	    	    myInfo -> offscreenDeviceContext, myNewX, myNewY, SRCCOPY);
	    	    
	    // Then, copy our picture (without the picture's transparent colour) 
	    // onto the new bitmap
	    myTransparentColour = 
		(COLORREF) MIORGB_ConvertColour (pmTransparentColour);
		
	    // Create mask of picture, if necessary
	    if (myPicInfo -> mask == NULL)
	    {
		myPicInfo -> mask = MyCreateMask (
		    myNewDC, myPicDC, 
		    pmWidth, pmHeight, myTransparentColour);
	    }

	    MyTransparentBlt (myNewDC, 0, 0, myNewWidth, myNewHeight, 
			      myPicDC, mySrcX, mySrcY, 
			      myTransparentColour, myPicInfo -> mask);
	    
	    // Finally, copy the current screen (without the screen's 
	    // transparent colour) onto the new bitmap
	    myTransparentColour = 
		(COLORREF) MIORGB_ConvertColour (myInfo -> transparentColour);

	    MyTransparentBlt (myNewDC, 0, 0, myNewWidth, myNewHeight, 
	    		      myInfo -> offscreenDeviceContext, 
			      myNewX, myNewY, myTransparentColour, NULL);
	    	    	      
	    // Now copy the new picture onto the screen
	    if (myInfo -> displayOnScreen)
	    {
		BitBlt (myInfo -> deviceContext, myNewX, myNewY, myNewWidth, 
	    		myNewHeight, myNewDC, 0, 0, SRCCOPY);
	    }
	    // Blit the picture onto the backing store
	    BitBlt (myInfo -> offscreenDeviceContext, myNewX, myNewY, 
		    myNewWidth, myNewHeight, myNewDC, 0, 0, SRCCOPY);
	    	    
    	    SelectPalette (myNewDC, myOldNewPalette, FALSE);
	    SelectBitmap (myNewDC, myNewBitMap);
	    DeleteDC (myNewDC);	    	    
	    DeleteBitmap (myNewBitMap);
    } // switch

    // Clean up
    SelectBitmap (myPicDC, myOldBitMap);
    SelectPalette (myPicDC, myOldPalette, FALSE);
    DeleteDC (myPicDC);

    // Instrumenting for memory and resource usage
//    MDIO_OutputSystemInfo ("After Draw");
} // MDIOPic_Draw


/************************************************************************/
/* MDIOPic_DrawPic							*/
/************************************************************************/
void	MDIOPic_DrawPic (PICBUFFER pmPic, int pmX, int pmY, int pmWidth, 
			 int pmHeight, int pmTransparentColour, int pmMode)
{
    MIOWinInfoPtr	myInfo = MIO_selectedRunWindowInfo;
    BITMAPINFO		*myBitMapInfo;
    void		*myBits;
    HBITMAP		myBitMap;
    
    myBitMapInfo = (BITMAPINFO *) ((char *) pmPic + sizeof (int));
    myBits = (void *) ((char *) pmPic + * (int *) pmPic);
    myBitMap = CreateDIBitmap (myInfo -> deviceContext, 
    			       (BITMAPINFOHEADER *) myBitMapInfo, CBM_INIT,
			       myBits, myBitMapInfo, DIB_RGB_COLORS);

    MDIOPic_Draw (myBitMap, NULL, pmX, pmY, pmWidth, pmHeight, 
		  pmTransparentColour, pmMode);
    
    DeleteBitmap (myBitMap);
} // MDIOPic_DrawPic


/************************************************************************/
/* MDIOPic_DrawTransitionBlend						*/
/************************************************************************/
void	MDIOPic_DrawTransitionBlend (PIC pmPic, PIC pmBackPic, int pmX, int pmY, 
				     int pmWidth, int pmHeight, int pmPercent)
{
    MIOWinInfoPtr	myInfo = MIO_selectedRunWindowInfo;
    MDPicInfoPtr	myPicInfo = (MDPicInfoPtr) pmPic;
    MDPicInfoPtr	myBackPicInfo = (MDPicInfoPtr) pmBackPic;

    // If pmBackPic is NULL, then the blend is with the run window's offscreen
    // bitmap.  Otherwise it is with the the picture passed in.  In both cases,
    // the destination is the run window's offscreen bitmap.
    if (pmBackPic == NULL)
    {
	// We're blending with the background.
	// The second BMP is the run window's offscreen bitmap
	MyBlend (myPicInfo -> bmp, 0, 0,
	    myInfo -> offscreenBitMap, pmX, pmY, pmWidth, pmHeight, pmPercent,
	    myInfo -> offscreenDeviceContext, myInfo -> offscreenBitMap,
	    pmX, pmY, myInfo -> width, myInfo -> height, 
	    myInfo -> deviceContext, myInfo -> displayOnScreen , 
	    myInfo -> palette);
	return;
    }
    else
    {
	MyBlend (myPicInfo -> bmp, 0, 0,
		 myBackPicInfo -> bmp, 0, 0, pmWidth, pmHeight, pmPercent,
		 myInfo -> offscreenDeviceContext, myInfo -> offscreenBitMap,
		 pmX, pmY, myInfo -> width, myInfo -> height, 
		 myInfo -> deviceContext, myInfo -> displayOnScreen , 
		 myInfo -> palette);
    }
} // MDIOPic_DrawTransitionBlend


/************************************************************************/
/* MDIOPic_DrawTransitionLeftRight					*/
/*									*/
/* This controls wipes, slides and grows that go left to right or right	*/
/* to left.  If pmMoveRight, the image moves from left to right,	*/
/* otherwise it moves from right to left.  If pmMode is MODE_WIPE,	*/
/* then the image moves in over top of the background.  If pmMode is	*/
/* MODE_SLIDE, the background appears to slide out as the image slides	*/
/* in.  If pmMode is MODE_GROW, the image appear to grow from the right	*/
/* or left as the background appears to shrink towards the other side.	*/
/*									*/
/* Note:								*/
/*   pmCurrentPos go from 0 to the width of the picture ( + bar	width	*/
/*   if bar drawn).							*/
/************************************************************************/
void	MDIOPic_DrawTransitionLeftRight (PIC pmPic, PIC pmBackPic, 
	    int pmX, int pmY, int pmWidth, int pmHeight, 
	    int pmLastPos, int pmCurrentPos, BOOL pmUseBar, 
	    BOOL pmMoveRight, int pmMode)
{
    MIOWinInfoPtr	myInfo = MIO_selectedRunWindowInfo;
    MDPicInfoPtr	myPicInfo = (MDPicInfoPtr) pmPic;
    MDPicInfoPtr	myBackPicInfo = (MDPicInfoPtr) pmBackPic;
    HBITMAP		myOldBitMap;
    HDC			myTempDC;
    HPALETTE		myOldPalette;
    // myBarDstX: The left side location of the screen where the bar goes
    int			myBarDstX;
    // myBarWidth: The width of the bar
    int			myBarWidth;
    // myPicSrc{X,Y}: The picture co-ordinates of the upper-left corner of the 
    //		      picture section to be copied
    int			myPicSrcX = 0, myPicSrcY = 0;
    // myPicDst{X,Y}: The screen co-ordinates of the upper-left corner where
    //                the picture will be copied.  Note myPicDstY is also used
    //		      for the background pic section and the bar.
    int			myPicDstX = pmX, myPicDstY = pmY;
    // myPic{Width,Height}: The width and height of the picture section to 
    //                      be copied.  Note myPicHeight is also used for the
    //			    background pic section and the bar.
    int			myPicWidth, myPicHeight = pmHeight;
    // myPicGrowX: The x co-ordinate to draw the picture from (in grow mode)
    int			myPicGrowX = 0;
    // myPicGrowWidth: The width to draw the picture from (in grow mode)
    int			myPicGrowWidth = pmWidth;
    // myBGSrcX: The left side location background pic section to be copied
    int			myBGSrcX = 0;
    // myBGDstX: The left side location of the screen into which the background
    //           pic section will be copied
    int			myBGDstX = pmX;
    // myBGWidth: The width of the background pic section to be copied
    int			myBGWidth;
    // myBGGrowX: 
    int			myBGGrowX = 0;
    // myBGGrowWidth: 
    int			myBGGrowWidth = pmWidth;
    // myTotalDstX: The left side location of all the parts to be copied
    int			myTotalDstX = pmX;
    // myTotalWidth: The width of all the parts to be copied to the screen
    int			myTotalWidth = pmWidth;
    int			myOrigPicWidth, myOrigBGWidth;

    // Adjust the coordinates to draw only the new region
    if (pmUseBar)
    {
	myBarWidth = min (pmCurrentPos, 
			  WIPE_BAR_WIDTH - max (0, pmCurrentPos - pmWidth));
	myPicWidth = max (pmCurrentPos - WIPE_BAR_WIDTH, 0);
	myBGWidth = max (pmWidth - pmCurrentPos - WIPE_BAR_WIDTH, 0);
	if ((pmMode == MODE_SLIDE) || (pmMode == MODE_GROW))
	{
	    if (pmMoveRight)
	    {
		myPicSrcX += pmWidth - max (pmCurrentPos - WIPE_BAR_WIDTH, 0);
		myBGDstX += pmCurrentPos;
	    }
	    else
	    {
		myPicDstX += pmWidth - max (pmCurrentPos - WIPE_BAR_WIDTH, 0);
		myBGSrcX += pmCurrentPos;
	    }
	}
	else if (pmMode == MODE_WIPE)
	{
	    if (pmMoveRight)
	    {
		// We need only draw the newly revealed part
		myPicSrcX += max (pmLastPos - WIPE_BAR_WIDTH, 0);
		myPicDstX += max (pmLastPos - WIPE_BAR_WIDTH, 0);
	    }
	    else
	    {
		myPicSrcX += pmWidth - max (pmCurrentPos - WIPE_BAR_WIDTH, 0);
		myPicDstX += pmWidth - max (pmCurrentPos - WIPE_BAR_WIDTH, 0);
	    }
	    myPicWidth -= max (pmLastPos - WIPE_BAR_WIDTH, 0);
	}
    }
    else
    {
	myBarWidth = 0;
	myPicWidth = pmCurrentPos;
	myBGWidth = pmWidth - pmCurrentPos;
	if ((pmMode == MODE_SLIDE) || (pmMode == MODE_GROW))
	{
	    if (pmMoveRight)
	    {
		myPicSrcX += pmWidth - pmCurrentPos;
		myBGDstX += pmCurrentPos;
	    }
	    else
	    {
		myPicDstX += pmWidth - pmCurrentPos;
		myBGSrcX += pmCurrentPos;
	    }
	}
	else if (pmMode == MODE_WIPE)
	{
	    if (pmMoveRight)
	    {
		// We need only draw the newly revealed part
		myPicSrcX += pmLastPos;
		myPicDstX += pmLastPos;
	    }
	    else
	    {
		myPicSrcX += pmWidth - pmCurrentPos;
		myPicDstX += pmWidth - pmCurrentPos;
	    }
	    myPicWidth -= pmLastPos;
	}
    }

    myOrigPicWidth = myPicWidth;
    myOrigBGWidth = myBGWidth;

    //
    // Adjust coordinates to fit on the output window
    //

    // Left side of picture section off the left of output window?
    if (myPicDstX < 0)
    {
        myPicSrcX -= myPicDstX; // Remember: myPicDstX is negative
	myPicWidth += myPicDstX;
	if ((pmMode == MODE_GROW) && (myOrigPicWidth > 0))
	{
	    // If we're going to cut out some of the drawn image on the screen,
	    // then we have to decide which part of the image we're not going
	    // to draw from.
	    myPicGrowX = -myPicDstX * pmWidth / myOrigPicWidth;
	    myPicGrowWidth = myPicWidth * pmWidth / myOrigPicWidth;
	}
        myPicDstX = 0;
    }
    // Right side of picture off the right of output window?
    if (myPicWidth > myInfo -> width - myPicDstX)
    {
	myPicWidth = max (myInfo -> width - myPicDstX, 0);
	if ((pmMode == MODE_GROW) && (myOrigPicWidth > 0))
	{
	    // If we're going to cut out some of the drawn image on the screen,
	    // then we have to decide which part of the image we're not going
	    // to draw from.
	    myPicGrowWidth = myPicWidth * pmWidth / myOrigPicWidth;
	}
    }

    // Left side of background drawing section off the left of output window?
    if (myBGDstX < 0)
    {
        myBGSrcX -= myBGDstX; // Remember: myBGDstX is negative
	myBGWidth += myBGDstX;
	if ((pmMode == MODE_GROW) && (myOrigBGWidth > 0))
	{
	    // If we're going to cut out some of the drawn image on the screen,
	    // then we have to decide which part of the image we're not going
	    // to draw from.
	    myBGGrowX = -myBGDstX * pmWidth / myOrigBGWidth;
	    myBGGrowWidth = myBGWidth * pmWidth / myOrigBGWidth;
	}
        myBGDstX = 0;
    }
    // Right side of background drawing section off the right of output window?
    if (myBGWidth > myInfo -> width - myBGDstX)
    {
	myBGWidth = max (myInfo -> width - myBGDstX, 0);
	if ((pmMode == MODE_GROW) && (myOrigBGWidth > 0))
	{
	    // If we're going to cut out some of the drawn image on the screen,
	    // then we have to decide which part of the image we're not going
	    // to draw from.
	    myBGGrowWidth = myBGWidth * pmWidth / myOrigBGWidth;
	}
    }

    // Left side of drawing area off the left of the output window?
    if (myTotalDstX < 0)
    {
	myTotalWidth += myTotalDstX;
        myTotalDstX = 0;
    }
    // Right side of drawing area off the right of the output window?
    if (myTotalWidth > myInfo -> width - myTotalDstX)
    {
	myTotalWidth = max (myInfo -> width - myTotalDstX, 0);
    }

    // Top of picture off the top of the output window?
    if (myPicDstY < 0)
    {
        myPicSrcY -= myPicDstY; // Remember: myPicDstY is negative
	myPicHeight += myPicDstY;
        myPicDstY = 0;
    }
    // Bottom of picture off the bottom of the output window?
    if (myPicHeight > myInfo -> height - myPicDstY)
    {
	myPicHeight = max (myInfo -> height - myPicDstY, 0);
    }

    //
    // Calculate where the bar should go and how wide it should be.
    //
    if (myBarWidth > 0)
    {
	if (pmMoveRight)
	{
	    myBarDstX = myPicDstX + myPicWidth;
	}
	else
	{
	    myBarDstX = myPicDstX - myBarWidth;
	}
	// Left side of bar off the left side of the output window?
	if (myBarDstX < 0)
	{
	    myBarWidth += myBarDstX;
	    myBarDstX = 0;
	}
	// Right side of bar off the right of the output window?
	if (myBarWidth > myInfo -> width - myBarDstX)
	{
	    myBarWidth = max (myInfo -> width - myBarDstX, 0);
	}
    }

    //
    // Set up the device context
    //
    myTempDC = CreateCompatibleDC (myInfo -> deviceContext);
    if (myTempDC == NULL) 
    {
	MIO_MoreInfo ("Draw: Problem creating memory context (%d)",
		      GetLastError ());
    	SET_ERRNO (E_PIC_CANT_ALLOC_MEM_FOR_PIC);
    	return;
    }

    // Set the new palette
    myOldPalette = SelectPalette (myTempDC, myInfo -> palette, FALSE);
    RealizePalette (myTempDC);

    //
    // Copy the picture (scaled if in GROW_MODE)
    //

    // Select the picture into the DC
    myOldBitMap = SelectBitmap (myTempDC, myPicInfo -> bmp);

    if ((myPicWidth > 0) && (myPicHeight > 0))
    {
	if (pmMode == MODE_GROW)
	{
	    // Stretch the new picture onto the backing store
	    StretchBlt (myInfo -> offscreenDeviceContext, myPicDstX, myPicDstY,
			myPicWidth, myPicHeight, myTempDC, 
			myPicGrowX, myPicSrcY,
			myPicGrowWidth, myPicHeight, SRCCOPY);
	}
	else
	{
	    // Blit the new picture onto the backing store
	    BitBlt (myInfo -> offscreenDeviceContext, myPicDstX, myPicDstY, 
		    myPicWidth, myPicHeight, myTempDC, myPicSrcX, myPicSrcY, 
		    SRCCOPY);
	}
    }

    //
    // Copy the background section (if in SLIDE_MODE or scaled in GROW_MODE)
    //

    if ((pmMode == MODE_SLIDE) || (pmMode == MODE_GROW))
    {
	// Select the background bitmap into the DC
	SelectBitmap (myTempDC, myBackPicInfo -> bmp);
	
	if ((myBGWidth > 0) && (myPicHeight > 0))
	{
	    if (pmMode == MODE_SLIDE)
	    {
		// Blit the background that is still showing onto the backing 
		// store
		BitBlt (myInfo -> offscreenDeviceContext, myBGDstX, myPicDstY, 
		        myBGWidth, myPicHeight, myTempDC, myBGSrcX, myPicSrcY,
			SRCCOPY);
	    }
	    else
	    {
		// Stretch the new bg onto the backing store
		StretchBlt (myInfo -> offscreenDeviceContext, 
			    myBGDstX, myPicDstY,
			    myBGWidth, myPicHeight, myTempDC, 
			    myBGGrowX, myPicSrcY, 
			    myBGGrowWidth, myPicHeight, SRCCOPY);
	    }
	}
    }

    //
    // Copy the bar
    //
    if ((myBarWidth > 0) && (myPicHeight > 0))
    {
	BitBlt (myInfo -> offscreenDeviceContext, myBarDstX, myPicDstY, 
	    myBarWidth, myPicHeight, myTempDC, myPicSrcX, myPicSrcY, BLACKNESS);
    }

    //
    // Copy the entire picture from offscreen bitmap to screen
    //
    if ((myInfo -> displayOnScreen) && (myPicHeight > 0) &&
	(myTotalWidth > 0))
    {
	// Blit the picture fromthe backing store onto the screen
	BitBlt (myInfo -> deviceContext, myTotalDstX, myPicDstY,
	    myTotalWidth, myPicHeight, 
	    myInfo -> offscreenDeviceContext, myTotalDstX, myPicDstY, SRCCOPY);
    }

    // Clean up
    SelectBitmap (myTempDC, myOldBitMap);
    SelectPalette (myTempDC, myOldPalette, FALSE);
    DeleteDC (myTempDC);
} // MDIOPic_DrawTransitionLeftRight


/************************************************************************/
/* MDIOPic_DrawTransitionTopBottom					*/
/*									*/
/* This controls wipes, slides and grows that go top to bottom or 	*/
/* bottom to top.  If pmMoveRight, the image moves from top to bottom,	*/
/* otherwise it moves from bottom to top.  If pmMode is MODE_WIPE,	*/
/* then the image moves in over top of the background.  If pmMode is	*/
/* MODE_SLIDE, the background appears to slide out as the image slides	*/
/* in.  If pmMode is MODE_GROW, the image appear to grow from the 	*/
/* bottom or top as the background appears to shrink towards the other	*/
/* side.								*/
/*									*/
/* Note:								*/
/*   pmCurrentPos go from 0 to the width of the picture ( + bar	width	*/
/*   if bar drawn).							*/
/************************************************************************/
void	MDIOPic_DrawTransitionTopBottom (PIC pmPic, PIC pmBackPic, 
	    int pmX, int pmY, int pmWidth, int pmHeight, 
	    int pmLastPos, int pmCurrentPos, BOOL pmUseBar, 
	    BOOL pmMoveDown, int pmMode)
{
    MIOWinInfoPtr	myInfo = MIO_selectedRunWindowInfo;
    MDPicInfoPtr	myPicInfo = (MDPicInfoPtr) pmPic;
    MDPicInfoPtr	myBackPicInfo = (MDPicInfoPtr) pmBackPic;
    HBITMAP		myOldBitMap;
    HDC			myTempDC;
    HPALETTE		myOldPalette;
    // myBarDstY: The left side location of the screen where the bar goes
    int			myBarDstY;
    // myBarHeight: The height of the bar
    int			myBarHeight;
    // myPicSrc{X,Y}: The picture co-ordinates of the upper-left corner of the 
    //		      picture section to be copied
    int			myPicSrcX = 0, myPicSrcY = 0;
    // myPicDst{X,Y}: The screen co-ordinates of the upper-left corner where
    //                the picture will be copied.  Note myPicDstY is also used
    //		      for the background pic section and the bar.
    int			myPicDstX = pmX, myPicDstY = pmY;
    // myPic{Width,Height}: The width and height of the picture section to 
    //                      be copied.  Note myPicHeight is also used for the
    //			    background pic section and the bar.
    int			myPicWidth = pmWidth, myPicHeight;
    // myPicGrowY: The y co-ordinate to draw the picture from (in grow mode)
    int			myPicGrowY = 0;
    // myPicGrowHeight: The height to draw the picture from (in grow mode)
    int			myPicGrowHeight = pmHeight;
    // myBGSrcY: The left side location background pic section to be copied
    int			myBGSrcY = 0;
    // myBGDstY: The left side location of the screen into which the background
    //           pic section will be copied
    int			myBGDstY = pmY;
    // myBGHeight: The width of the background pic section to be copied
    int			myBGHeight;
    // myBGGrowY: 
    int			myBGGrowY = 0;
    // myBGGrowHeight: 
    int			myBGGrowHeight = pmHeight;
    // myTotalDstY: The left side location of all the parts to be copied
    int			myTotalDstY = pmY;
    // myTotalHeight: The height of all the parts to be copied to the screen
    int			myTotalHeight = pmHeight;
    int			myOrigPicHeight, myOrigBGHeight;

    // Adjust the coordinates to draw only the new region
    if (pmUseBar)
    {
	myBarHeight = min (pmCurrentPos, 
			  WIPE_BAR_WIDTH - max (0, pmCurrentPos - pmHeight));
	myPicHeight = max (pmCurrentPos - WIPE_BAR_WIDTH, 0);
	myBGHeight = max (pmHeight - pmCurrentPos - WIPE_BAR_WIDTH, 0);
	if ((pmMode == MODE_SLIDE) || (pmMode == MODE_GROW))
	{
	    if (pmMoveDown)
	    {
		myPicSrcY += pmHeight - max (pmCurrentPos - WIPE_BAR_WIDTH, 0);
		myBGDstY += pmCurrentPos;
	    }
	    else
	    {
		myPicDstY += pmHeight - max (pmCurrentPos - WIPE_BAR_WIDTH, 0);
		myBGSrcY += pmCurrentPos;
	    }
	}
	else if (pmMode == MODE_WIPE)
	{
	    if (pmMoveDown)
	    {
		// We need only draw the newly revealed part
		myPicSrcY += max (pmLastPos - WIPE_BAR_WIDTH, 0);
		myPicDstY += max (pmLastPos - WIPE_BAR_WIDTH, 0);
	    }
	    else
	    {
		myPicSrcY += pmHeight - max (pmCurrentPos - WIPE_BAR_WIDTH, 0);
		myPicDstY += pmHeight - max (pmCurrentPos - WIPE_BAR_WIDTH, 0);
	    }
	    myPicHeight -= max (pmLastPos - WIPE_BAR_WIDTH, 0);
	}
    }
    else
    {
	myBarHeight = 0;
	myPicHeight = pmCurrentPos;
	myBGHeight = pmHeight - pmCurrentPos;
	if ((pmMode == MODE_SLIDE) || (pmMode == MODE_GROW))
	{
	    if (pmMoveDown)
	    {
		myPicSrcY += pmHeight - pmCurrentPos;
		myBGDstY += pmCurrentPos;
	    }
	    else
	    {
		myPicDstY += pmHeight - pmCurrentPos;
		myBGSrcY += pmCurrentPos;
	    }
	}
	else if (pmMode == MODE_WIPE)
	{
	    if (pmMoveDown)
	    {
		// We need only draw the newly revealed part
		myPicSrcY += pmLastPos;
		myPicDstY += pmLastPos;
	    }
	    else
	    {
		myPicSrcY += pmHeight - pmCurrentPos;
		myPicDstY += pmHeight - pmCurrentPos;
	    }
	    myPicHeight -= pmLastPos;
	}
    }

    myOrigPicHeight = myPicHeight;
    myOrigBGHeight = myBGHeight;

    //
    // Adjust coordinates to fit on the output window
    //

    // Top of picture section off the top of output window?
    if (myPicDstY < 0)
    {
        myPicSrcY -= myPicDstY; // Remember: myPicDstY is negative
	myPicHeight += myPicDstY;
	if ((pmMode == MODE_GROW) && (myOrigPicHeight > 0))
	{
	    // If we're going to cut out some of the drawn image on the screen,
	    // then we have to decide which part of the image we're not going
	    // to draw from.
	    myPicGrowY = -myPicDstY * pmHeight / myOrigPicHeight;
	    myPicGrowHeight = myPicHeight * pmHeight / myOrigPicHeight;
	}
        myPicDstY = 0;
    }
    // Top side of picture off the top of output window?
    if (myPicHeight > myInfo -> height - myPicDstY)
    {
	myPicHeight = max (myInfo -> height - myPicDstY, 0);
	if ((pmMode == MODE_GROW) && (myOrigPicHeight > 0))
	{
	    // If we're going to cut out some of the drawn image on the screen,
	    // then we have to decide which part of the image we're not going
	    // to draw from.
	    myPicGrowHeight = myPicHeight * pmHeight / myOrigPicHeight;
	}
    }

    // Top of background drawing section off the top of output window?
    if (myBGDstY < 0)
    {
        myBGSrcY -= myBGDstY; // Remember: myBGDstY is negative
	myBGHeight += myBGDstY;
	if ((pmMode == MODE_GROW) && (myOrigBGHeight > 0))
	{
	    // If we're going to cut out some of the drawn image on the screen,
	    // then we have to decide which part of the image we're not going
	    // to draw from.
	    myBGGrowY = -myBGDstY * pmHeight / myOrigBGHeight;
	    myBGGrowHeight = myBGHeight * pmHeight / myOrigBGHeight;
	}
        myBGDstY = 0;
    }
    // Bottom side of background drawing section off the bottom of output window?
    if (myBGHeight > myInfo -> height - myBGDstY)
    {
	myBGHeight = max (myInfo -> height - myBGDstY, 0);
	if ((pmMode == MODE_GROW) && (myOrigBGHeight > 0))
	{
	    // If we're going to cut out some of the drawn image on the screen,
	    // then we have to decide which part of the image we're not going
	    // to draw from.
	    myBGGrowHeight = myBGHeight * pmHeight / myOrigBGHeight;
	}
    }

    // Top of drawing area off the top of the output window?
    if (myTotalDstY < 0)
    {
	myTotalHeight += myTotalDstY;
        myTotalDstY = 0;
    }
    // Bottom of drawing area off the bottom of the output window?
    if (myTotalHeight > myInfo -> height - myTotalDstY)
    {
	myTotalHeight = max (myInfo -> height - myTotalDstY, 0);
    }

    // Left side of picture off the left side of the output window?
    if (myPicDstX < 0)
    {
        myPicSrcX -= myPicDstX; // Remember: myPicDstX is negative
	myPicWidth += myPicDstX;
        myPicDstX = 0;
    }
    // Right side of picture off the right side of the output window?
    if (myPicWidth > myInfo -> width - myPicDstX)
    {
	myPicWidth = max (myInfo -> width - myPicDstX, 0);
    }

    //
    // Calculate where the bar should go and how wide it should be.
    //
    if (myBarHeight > 0)
    {
	if (pmMoveDown)
	{
	    myBarDstY = myPicDstY + myPicHeight;
	}
	else
	{
	    myBarDstY = myPicDstY - myBarHeight;
	}
	// Top of bar off the top of output window
	if (myBarDstY < 0)
	{
	    myBarHeight += myBarDstY;
	    myBarDstY = 0;
	}
	// Bottom of bar off the bottom of output window?
	if (myBarHeight > myInfo -> height - myBarDstY)
	{
	    myBarHeight = max (myInfo -> height - myBarDstY, 0);
	}
    }

    //
    // Set up the device context
    //
    myTempDC = CreateCompatibleDC (myInfo -> deviceContext);
    if (myTempDC == NULL) 
    {
	MIO_MoreInfo ("Draw: Problem creating memory context (%d)",
		      GetLastError ());
    	SET_ERRNO (E_PIC_CANT_ALLOC_MEM_FOR_PIC);
    	return;
    }

    // Set the new palette
    myOldPalette = SelectPalette (myTempDC, myInfo -> palette, FALSE);
    RealizePalette (myTempDC);

    //
    // Copy the picture
    //

    // Select the picture into the DC
    myOldBitMap = SelectBitmap (myTempDC, myPicInfo -> bmp);

    if ((myPicHeight > 0) && (myPicWidth > 0))
    {
	if (pmMode == MODE_GROW)
	{
	    // Stretch the new picture onto the backing store
	    StretchBlt (myInfo -> offscreenDeviceContext, myPicDstX, myPicDstY,
			myPicWidth, myPicHeight, myTempDC, 
			myPicSrcX, myPicGrowY, 
			myPicWidth, myPicGrowHeight, SRCCOPY);
	}
	else
	{
	    // Blit the new picture onto the backing store
	    BitBlt (myInfo -> offscreenDeviceContext, myPicDstX, myPicDstY, 
		    myPicWidth, myPicHeight, myTempDC, myPicSrcX, myPicSrcY, 
		    SRCCOPY);
	}
    }

    //
    // Copy the background section
    //

    if ((pmMode == MODE_SLIDE) || (pmMode == MODE_GROW))
    {
	// Select the background bitmap into the DC
	SelectBitmap (myTempDC, myBackPicInfo -> bmp);
	
	if ((myBGHeight > 0) && (myPicWidth > 0))
	{
	    if (pmMode == MODE_SLIDE)
	    {
		// Blit the background that is still showing onto the backing 
		// store
		BitBlt (myInfo -> offscreenDeviceContext, myPicDstX, myBGDstY, 
		        myPicWidth, myBGHeight, myTempDC, myPicSrcX, myBGSrcY,
			SRCCOPY);
	    }
	    else
	    {
		// Stretch the new bg onto the backing store
		StretchBlt (myInfo -> offscreenDeviceContext, 
			    myPicDstX, myBGDstY,
			    myPicWidth, myBGHeight, myTempDC, 
			    myPicSrcX, myBGGrowY, 
			    myPicWidth, myBGGrowHeight, SRCCOPY);
	    }
	}
    }

    //
    // Copy the bar
    //
    if ((myBarHeight > 0) && (myPicWidth > 0))
    {
	BitBlt (myInfo -> offscreenDeviceContext, myPicDstX, myBarDstY, 
	    myPicWidth, myBarHeight, myTempDC, myPicSrcX, myPicSrcY, BLACKNESS);
    }

    //
    // Copy the entire picture from offscreen bitmap to screen
    //
    if ((myInfo -> displayOnScreen) && (myPicWidth > 0) &&
	(myTotalHeight > 0))
    {
	// Blit the picture fromthe backing store onto the screen
	BitBlt (myInfo -> deviceContext, myPicDstX, myTotalDstY,
	    myPicWidth, myTotalHeight, 
	    myInfo -> offscreenDeviceContext, myPicDstX, myTotalDstY, SRCCOPY);
    }

    // Clean up
    SelectBitmap (myTempDC, myOldBitMap);
    SelectPalette (myTempDC, myOldPalette, FALSE);
    DeleteDC (myTempDC);
} // MDIOPic_DrawTransitionTopBottom


/************************************************************************/
/* MDIOPic_DrawTransitionCentreToEdge					*/
/*									*/
/* Draw the window from inside out.  pmCurrentPos ranges from 0 to the	*/
/* (maximum of width/height + 1) / 2.					*/
/************************************************************************/
void	MDIOPic_DrawTransitionCentreToEdge (PIC pmPic, PIC pmBackPic, 
	    int pmX, int pmY, int pmWidth, int pmHeight, 
	    int pmCurrentPos, BOOL pmUseBar, BOOL pmMode)
{
    MIOWinInfoPtr	myInfo = MIO_selectedRunWindowInfo;
    MDPicInfoPtr	myPicInfo = (MDPicInfoPtr) pmPic;
    MDPicInfoPtr	myBackPicInfo = (MDPicInfoPtr) pmBackPic;
    HBITMAP		myOldBitMap;
    HDC			myTempDC;
    HPALETTE		myOldPalette;
    // myBarWidth: The width of the bar
    int			myBarLeftWidth, myBarRightWidth;
    int			myBarTopHeight, myBarBottomHeight;
    // myPicSrc{X,Y}: The picture co-ordinates of the upper-left corner of the 
    //		      picture section to be copied
    int			myPicSrcX, myPicSrcY;
    // myPicDst{X,Y}: The screen co-ordinates of the upper-left corner where
    //                the picture will be copied
    int			myPicDstX, myPicDstY;
    // myPic{Width,Height}: The width and height of the picture section to 
    //                      be copied
    int			myPicWidth, myPicHeight;
    int			myOrigPicWidth, myOrigPicHeight;
    // myTotalDst{X,Y}: The screen co-ordinates of all the parts to be copied
    int			myTotalDstX, myTotalDstY;
    // myTotal{Width,Height}: The width and height of all the parts to be 
    //			      copied to the screen
    int			myTotalWidth, myTotalHeight;
    int			myGrowX = 0, myGrowY = 0;
    int			myGrowWidth = pmWidth, myGrowHeight = pmHeight;
    // 
    // Calculate where to draw the picture
    //
    if (pmUseBar)
    {
	pmCurrentPos = max (pmCurrentPos - WIPE_BAR_WIDTH, 0);
    }

    // Adjust elements
    if (pmWidth > pmHeight)
    {
	myPicWidth = min (pmWidth, pmCurrentPos * 2);
	myPicHeight = min (pmHeight, (pmCurrentPos * 2) * pmHeight / pmWidth);
    }
    else
    {
	myPicWidth = min (pmHeight, pmCurrentPos * 2);
	myPicHeight = min (pmWidth, (pmCurrentPos * 2) * pmWidth / pmHeight);
    }
    myOrigPicWidth = myPicWidth;
    myOrigPicHeight = myPicHeight;
    myPicSrcX = (pmWidth - myPicWidth) / 2;
    myPicSrcY = (pmHeight - myPicHeight) / 2;
    myPicDstX = pmX + myPicSrcX;
    myPicDstY = pmY + myPicSrcY;

    //
    // Calculate the bar widths so they don't extend out of the drawing area
    //
    if (pmUseBar)
    {
	myBarLeftWidth = min (WIPE_BAR_WIDTH, myPicSrcX);
	myBarRightWidth = min (WIPE_BAR_WIDTH, pmWidth - myPicSrcX - myPicWidth);
	myBarTopHeight = min (WIPE_BAR_WIDTH, myPicSrcY);
	myBarBottomHeight = min (WIPE_BAR_WIDTH, pmHeight - myPicSrcY - myPicHeight);
    }
    else
    {
	myBarLeftWidth = 0;
	myBarRightWidth = 0;
	myBarTopHeight = 0;
	myBarBottomHeight = 0;
    }

    //
    // Calculate the total drawing area
    //

    myTotalDstX = myPicDstX - myBarLeftWidth;
    myTotalDstY = myPicDstY - myBarTopHeight;
    myTotalWidth = myBarLeftWidth + myPicWidth + myBarRightWidth;
    myTotalHeight = myBarTopHeight + myPicHeight + myBarBottomHeight;

    //
    // Adjust coordinates to fit on the output window
    //

    // Left side of picture section off the left of output window?
    if (myPicDstX < 0)
    {
        myPicSrcX -= myPicDstX; // Remember: myPicDstX is negative
	myPicWidth += myPicDstX;
	if ((pmMode == MODE_GROW) && (myOrigPicWidth > 0))
	{
	    // If we're going to cut out some of the drawn image on the screen,
	    // then we have to decide which part of the image we're not going
	    // to draw from.
	    myGrowX = -myPicDstX * pmWidth / myOrigPicWidth;
	    myGrowWidth = myPicWidth * pmWidth / myOrigPicWidth;
	}
        myPicDstX = 0;
    }
    // Right side of picture off the right of output window?
    if (myPicWidth > myInfo -> width - myPicDstX)
    {
	myPicWidth = max (myInfo -> width - myPicDstX, 0);
	if ((pmMode == MODE_GROW) && (myOrigPicWidth > 0))
	{
	    // If we're going to cut out some of the drawn image on the screen,
	    // then we have to decide which part of the image we're not going
	    // to draw from.
	    myGrowWidth = myPicWidth * pmWidth / myOrigPicWidth;
	}
    }

    // Top of picture off the top of the output window?
    if (myPicDstY < 0)
    {
        myPicSrcY -= myPicDstY; // Remember: myPicDstY is negative
	myPicHeight += myPicDstY;
	if ((pmMode == MODE_GROW) && (myOrigPicHeight > 0))
	{
	    // If we're going to cut out some of the drawn image on the screen,
	    // then we have to decide which part of the image we're not going
	    // to draw from.
	    myGrowY = -myPicDstY * pmHeight / myOrigPicHeight;
	    myGrowHeight = myPicHeight * pmHeight / myOrigPicHeight;
	}
        myPicDstY = 0;
    }
    // Bottom of picture off the bottom of the output window?
    if (myPicHeight > myInfo -> height - myPicDstY)
    {
	myPicHeight = max (myInfo -> height - myPicDstY, 0);
	if ((pmMode == MODE_GROW) && (myOrigPicHeight > 0))
	{
	    // If we're going to cut out some of the drawn image on the screen,
	    // then we have to decide which part of the image we're not going
	    // to draw from.
	    myGrowHeight = myPicHeight * pmHeight / myOrigPicHeight;
	}
    }

    // Left side of drawing area off the left of the output window?
    if (myTotalDstX < 0)
    {
	myTotalWidth += myTotalDstX;
        myTotalDstX = 0;
    }
    // Right side of drawing area off the right of the output window?
    if (myTotalWidth > myInfo -> width - myTotalDstX)
    {
	myTotalWidth = max (myInfo -> width - myTotalDstX, 0);
    }

    // Top side of drawing area off the top of the output window?
    if (myTotalDstY < 0)
    {
	myTotalHeight += myTotalDstY;
        myTotalDstY = 0;
    }
    // Bottom side of drawing area off the bottom of the output window?
    if (myTotalHeight > myInfo -> height - myTotalDstY)
    {
	myTotalHeight = max (myInfo -> height - myTotalDstY, 0);
    }

    //
    // Set up the device context
    //
    myTempDC = CreateCompatibleDC (myInfo -> deviceContext);
    if (myTempDC == NULL) 
    {
	MIO_MoreInfo ("Draw: Problem creating memory context (%d)",
		      GetLastError ());
    	SET_ERRNO (E_PIC_CANT_ALLOC_MEM_FOR_PIC);
    	return;
    }

    // Set the new palette
    myOldPalette = SelectPalette (myTempDC, myInfo -> palette, FALSE);
    RealizePalette (myTempDC);

    //
    // Copy the picture
    //

    // Select the picture into the DC
    myOldBitMap = SelectBitmap (myTempDC, myPicInfo -> bmp);

    if ((myPicWidth > 0) && (myPicHeight > 0))
    {
	if (pmMode == MODE_WIPE)
	{
	    // Blit the new picture onto the backing store
	    BitBlt (myInfo -> offscreenDeviceContext, myPicDstX, myPicDstY, 
		myPicWidth, myPicHeight, myTempDC, myPicSrcX, myPicSrcY, 
		SRCCOPY);
	}
	else
	{
	    StretchBlt (myInfo -> offscreenDeviceContext, myPicDstX, myPicDstY, 
		myPicWidth, myPicHeight, myTempDC, myGrowX, myGrowY, 
		myGrowWidth, myGrowHeight, SRCCOPY);
	}
    }

    //
    // Copy the bars
    //
    if (myBarLeftWidth > 0)
    {
	BitBlt (myInfo -> offscreenDeviceContext, 
	    myPicDstX - myBarLeftWidth, myPicDstY - myBarTopHeight,
	    myBarLeftWidth, myPicHeight + myBarTopHeight + myBarBottomHeight, 
	    myTempDC, myPicSrcX, myPicSrcY, BLACKNESS);
    }
    if (myBarRightWidth > 0)
    {
	BitBlt (myInfo -> offscreenDeviceContext, 
	    myPicDstX + myPicWidth, myPicDstY - myBarTopHeight,
	    myBarLeftWidth, myPicHeight + myBarTopHeight + myBarBottomHeight, 
	    myTempDC, myPicSrcX, myPicSrcY, BLACKNESS);
    }
    if (myBarTopHeight > 0)
    {
	BitBlt (myInfo -> offscreenDeviceContext, 
	    myPicDstX - myBarLeftWidth, myPicDstY - myBarTopHeight,
	    myPicWidth + myBarLeftWidth + myBarRightWidth, myBarTopHeight, 
	    myTempDC, myPicSrcX, myPicSrcY, BLACKNESS);
    }
    if (myBarBottomHeight > 0)
    {
	BitBlt (myInfo -> offscreenDeviceContext, 
	    myPicDstX - myBarLeftWidth, myPicDstY + myPicHeight,
	    myPicWidth + myBarLeftWidth + myBarRightWidth, myBarBottomHeight, 
	    myTempDC, myPicSrcX, myPicSrcY, BLACKNESS);
    }

    //
    // Copy the entire picture from offscreen bitmap to screen
    //
    if ((myInfo -> displayOnScreen) && (myTotalHeight > 0) &&
	(myTotalWidth > 0))
    {
	// Blit the picture fromthe backing store onto the screen
	BitBlt (myInfo -> deviceContext, myTotalDstX, myTotalDstY,
	    myTotalWidth, myTotalHeight, myInfo -> offscreenDeviceContext,
	    myTotalDstX, myTotalDstY, SRCCOPY);
    }

    // Clean up
    SelectBitmap (myTempDC, myOldBitMap);
    SelectPalette (myTempDC, myOldPalette, FALSE);
    DeleteDC (myTempDC);
} // MDIOPic_DrawTransitionCentreToEdge


/************************************************************************/
/* MDIOPic_DrawTransitionCornerToCorner					*/
/*									*/
/* Draw the window from one corner outward.				*/
/************************************************************************/
void	MDIOPic_DrawTransitionCornerToCorner (PIC pmPic, PIC pmBackPic, 
	    int pmX, int pmY, int pmWidth, int pmHeight, 
	    int pmCurrentPos, BOOL pmUseBar, int pmCorner,
	    int pmMode)
{
    MIOWinInfoPtr	myInfo = MIO_selectedRunWindowInfo;
    MDPicInfoPtr	myPicInfo = (MDPicInfoPtr) pmPic;
    MDPicInfoPtr	myBackPicInfo = (MDPicInfoPtr) pmBackPic;
    HBITMAP		myOldBitMap;
    HDC			myTempDC;
    HPALETTE		myOldPalette;
    // myBarWidth: The width of the bar
    int			myBarLeftWidth, myBarRightWidth;
    int			myBarTopHeight, myBarBottomHeight;
    // myPicSrc{X,Y}: The picture co-ordinates of the upper-left corner of the 
    //		      picture section to be copied
    int			myPicSrcX, myPicSrcY;
    // myPicDst{X,Y}: The screen co-ordinates of the upper-left corner where
    //                the picture will be copied
    int			myPicDstX, myPicDstY;
    // myPic{Width,Height}: The width and height of the picture section to 
    //                      be copied
    int			myPicWidth, myPicHeight;
    int			myOrigPicWidth, myOrigPicHeight;
    // myTotalDst{X,Y}: The screen co-ordinates of all the parts to be copied
    int			myTotalDstX, myTotalDstY;
    // myTotal{Width,Height}: The width and height of all the parts to be 
    //			      copied to the screen
    int			myTotalWidth, myTotalHeight;
    int			myGrowX = 0, myGrowY = 0;
    int			myGrowWidth = pmWidth, myGrowHeight = pmHeight;
    // 
    // Calculate where to draw the picture
    //
    if (pmUseBar)
    {
	pmCurrentPos = max (pmCurrentPos - WIPE_BAR_WIDTH, 0);
    }

    // Adjust elements
    if (pmWidth > pmHeight)
    {
	myPicWidth = min (pmWidth, pmCurrentPos * 2);
	myPicHeight = min (pmHeight, (pmCurrentPos * 2) * pmHeight / pmWidth);
    }
    else
    {
	myPicWidth = min (pmHeight, pmCurrentPos * 2);
	myPicHeight = min (pmWidth, (pmCurrentPos * 2) * pmWidth / pmHeight);
    }
    myOrigPicWidth = myPicWidth;
    myOrigPicHeight = myPicHeight;

    if (pmCorner == FROM_UPPER_LEFT)
    {
	myPicSrcX = 0;
	myPicSrcY = 0;
	myPicDstX = pmX;
	myPicDstY = pmY;
    }
    else if (pmCorner == FROM_UPPER_RIGHT)
    {
	myPicSrcX = pmWidth - myPicWidth;
	myPicSrcY = 0;
	myPicDstX = pmX + myPicSrcX;
	myPicDstY = pmY;
    }
    else if (pmCorner == FROM_LOWER_LEFT)
    {
	myPicSrcX = 0;
	myPicSrcY = pmHeight - myPicHeight;
	myPicDstX = pmX;
	myPicDstY = pmY + myPicSrcY;
    }
    else if (pmCorner == FROM_LOWER_RIGHT)
    {
	myPicSrcX = pmWidth - myPicWidth;
	myPicSrcY = pmHeight - myPicHeight;
	myPicDstX = pmX + myPicSrcX;
	myPicDstY = pmY + myPicSrcY;
    }

    //
    // Calculate the bar widths so they don't extend out of the drawing area
    //
    if (pmUseBar)
    {
	myBarLeftWidth = min (WIPE_BAR_WIDTH, myPicSrcX);
	myBarRightWidth = min (WIPE_BAR_WIDTH, pmWidth - myPicSrcX - myPicWidth);
	myBarTopHeight = min (WIPE_BAR_WIDTH, myPicSrcY);
	myBarBottomHeight = min (WIPE_BAR_WIDTH, pmHeight - myPicSrcY - myPicHeight);
    }
    else
    {
	myBarLeftWidth = 0;
	myBarRightWidth = 0;
	myBarTopHeight = 0;
	myBarBottomHeight = 0;
    }

    //
    // Calculate the total drawing area
    //

    myTotalDstX = myPicDstX - myBarLeftWidth;
    myTotalDstY = myPicDstY - myBarTopHeight;
    myTotalWidth = myBarLeftWidth + myPicWidth + myBarRightWidth;
    myTotalHeight = myBarTopHeight + myPicHeight + myBarBottomHeight;

    //
    // Adjust coordinates to fit on the output window
    //

    // Left side of picture section off the left of output window?
    if (myPicDstX < 0)
    {
        myPicSrcX -= myPicDstX; // Remember: myPicDstX is negative
	myPicWidth += myPicDstX;
	if ((pmMode == MODE_GROW) && (myOrigPicWidth > 0))
	{
	    // If we're going to cut out some of the drawn image on the screen,
	    // then we have to decide which part of the image we're not going
	    // to draw from.
	    myGrowX = -myPicDstX * pmWidth / myOrigPicWidth;
	    myGrowWidth = myPicWidth * pmWidth / myOrigPicWidth;
	}
        myPicDstX = 0;
    }
    // Right side of picture off the right of output window?
    if (myPicWidth > myInfo -> width - myPicDstX)
    {
	myPicWidth = max (myInfo -> width - myPicDstX, 0);
	if ((pmMode == MODE_GROW) && (myOrigPicWidth > 0))
	{
	    // If we're going to cut out some of the drawn image on the screen,
	    // then we have to decide which part of the image we're not going
	    // to draw from.
	    myGrowWidth = myPicWidth * pmWidth / myOrigPicWidth;
	}
    }

    // Top of picture off the top of the output window?
    if (myPicDstY < 0)
    {
        myPicSrcY -= myPicDstY; // Remember: myPicDstY is negative
	myPicHeight += myPicDstY;
	if ((pmMode == MODE_GROW) && (myOrigPicHeight > 0))
	{
	    // If we're going to cut out some of the drawn image on the screen,
	    // then we have to decide which part of the image we're not going
	    // to draw from.
	    myGrowY = -myPicDstY * pmHeight / myOrigPicHeight;
	    myGrowHeight = myPicHeight * pmHeight / myOrigPicHeight;
	}
        myPicDstY = 0;
    }
    // Bottom of picture off the bottom of the output window?
    if (myPicHeight > myInfo -> height - myPicDstY)
    {
	myPicHeight = max (myInfo -> height - myPicDstY, 0);
	if ((pmMode == MODE_GROW) && (myOrigPicHeight > 0))
	{
	    // If we're going to cut out some of the drawn image on the screen,
	    // then we have to decide which part of the image we're not going
	    // to draw from.
	    myGrowHeight = myPicHeight * pmHeight / myOrigPicHeight;
	}
    }

    // Left side of drawing area off the left of the output window?
    if (myTotalDstX < 0)
    {
	myTotalWidth += myTotalDstX;
        myTotalDstX = 0;
    }
    // Right side of drawing area off the right of the output window?
    if (myTotalWidth > myInfo -> width - myTotalDstX)
    {
	myTotalWidth = max (myInfo -> width - myTotalDstX, 0);
    }

    // Top side of drawing area off the top of the output window?
    if (myTotalDstY < 0)
    {
	myTotalHeight += myTotalDstY;
        myTotalDstY = 0;
    }
    // Bottom side of drawing area off the bottom of the output window?
    if (myTotalHeight > myInfo -> height - myTotalDstY)
    {
	myTotalHeight = max (myInfo -> height - myTotalDstY, 0);
    }

    //
    // Set up the device context
    //
    myTempDC = CreateCompatibleDC (myInfo -> deviceContext);
    if (myTempDC == NULL) 
    {
	MIO_MoreInfo ("Draw: Problem creating memory context (%d)",
		      GetLastError ());
    	SET_ERRNO (E_PIC_CANT_ALLOC_MEM_FOR_PIC);
    	return;
    }

    // Set the new palette
    myOldPalette = SelectPalette (myTempDC, myInfo -> palette, FALSE);
    RealizePalette (myTempDC);

    //
    // Copy the picture
    //

    // Select the picture into the DC
    myOldBitMap = SelectBitmap (myTempDC, myPicInfo -> bmp);

    if ((myPicWidth > 0) && (myPicHeight > 0))
    {
	if (pmMode == MODE_WIPE)
	{
	    // Blit the new picture onto the backing store
	    BitBlt (myInfo -> offscreenDeviceContext, myPicDstX, myPicDstY, 
		myPicWidth, myPicHeight, myTempDC, myPicSrcX, myPicSrcY, 
		SRCCOPY);
	}
	else
	{
	    StretchBlt (myInfo -> offscreenDeviceContext, myPicDstX, myPicDstY, 
		myPicWidth, myPicHeight, myTempDC, myGrowX, myGrowY, 
		myGrowWidth, myGrowHeight, SRCCOPY);
	}
    }

    //
    // Copy the bars
    //
    if (myBarLeftWidth > 0)
    {
	BitBlt (myInfo -> offscreenDeviceContext, 
	    myPicDstX - myBarLeftWidth, myPicDstY - myBarTopHeight,
	    myBarLeftWidth, myPicHeight + myBarTopHeight + myBarBottomHeight, 
	    myTempDC, myPicSrcX, myPicSrcY, BLACKNESS);
    }
    if (myBarRightWidth > 0)
    {
	BitBlt (myInfo -> offscreenDeviceContext, 
	    myPicDstX + myPicWidth, myPicDstY - myBarTopHeight,
	    myBarRightWidth, myPicHeight + myBarTopHeight + myBarBottomHeight, 
	    myTempDC, myPicSrcX, myPicSrcY, BLACKNESS);
    }
    if (myBarTopHeight > 0)
    {
	BitBlt (myInfo -> offscreenDeviceContext, 
	    myPicDstX - myBarLeftWidth, myPicDstY - myBarTopHeight,
	    myPicWidth + myBarLeftWidth + myBarRightWidth, myBarTopHeight, 
	    myTempDC, myPicSrcX, myPicSrcY, BLACKNESS);
    }
    if (myBarBottomHeight > 0)
    {
	BitBlt (myInfo -> offscreenDeviceContext, 
	    myPicDstX - myBarLeftWidth, myPicDstY + myPicHeight,
	    myPicWidth + myBarLeftWidth + myBarRightWidth, myBarBottomHeight, 
	    myTempDC, myPicSrcX, myPicSrcY, BLACKNESS);
    }

    //
    // Copy the entire picture from offscreen bitmap to screen
    //
    if ((myInfo -> displayOnScreen) && (myTotalHeight > 0) &&
	(myTotalWidth > 0))
    {
	// Blit the picture fromthe backing store onto the screen
	BitBlt (myInfo -> deviceContext, myTotalDstX, myTotalDstY,
	    myTotalWidth, myTotalHeight, myInfo -> offscreenDeviceContext,
	    myTotalDstX, myTotalDstY, SRCCOPY);
    }

    // Clean up
    SelectBitmap (myTempDC, myOldBitMap);
    SelectPalette (myTempDC, myOldPalette, FALSE);
    DeleteDC (myTempDC);
} // MDIOPic_DrawTransitionCornerToCorner


/************************************************************************/
/* MDIOPic_FreePic							*/
/************************************************************************/
void	MDIOPic_FreePic (PIC pmPic)
{
    MDPicInfoPtr	myPicInfo = (MDPicInfoPtr) pmPic;

    DeleteBitmap (myPicInfo -> bmp);
    myPicInfo -> bmp = NULL;
    if (myPicInfo -> mask != NULL)
    {
	DeleteBitmap (myPicInfo -> mask);
	myPicInfo -> mask = NULL;
    }
    free (myPicInfo);
} // MDIOPic_FreePic


/************************************************************************/
/* MDIOPic_GetExtraInfo							*/
/************************************************************************/
int	MDIOPic_GetExtraInfo (void)
{
    return 0;
} // MDIOPic_GetExtraInfo


/************************************************************************/
/* MDIOPic_GetImage							*/
/************************************************************************/
BOOL	MDIOPic_GetImage (OOTint pmX1, OOTint pmY1, OOTint pmX2, 
			  OOTint pmY2, PIC *pmPic)
{
    MIOWinInfoPtr	myInfo = MIO_selectedRunWindowInfo;
    MDPicInfoPtr	*myPicInfo = (MDPicInfoPtr *) pmPic;
    int			myWidth, myHeight;
    int			mySrcX, mySrcY;
    int			myDstX = 0, myDstY = 0;
    HBITMAP		myBitMap, myOldBitMap;
    HDC			myTempDC;
    HPALETTE		myOldPalette;


    if (pmX1 <= pmX2)
    {
	myWidth = pmX2 - pmX1 + 1;
	mySrcX = pmX1;
    }
    else
    {
	myWidth = pmX1 - pmX2 + 1;
	mySrcX = pmX2;
    }
    if (pmY1 <= pmY2)
    {
	myHeight = pmY2 - pmY1 + 1;
	mySrcY = pmY1;
    }
    else
    {
	myHeight = pmY1 - pmY2 + 1;
	mySrcY = pmY2;
    }
    
    myBitMap = CreateCompatibleBitmap (myInfo -> deviceContext, myWidth, 
    				       myHeight);
    if (myBitMap == NULL) 
    {
	MIO_MoreInfo ("GetImage: Problem creating bitmap (%d)",
		      GetLastError ());
	SET_ERRNO (E_PIC_CANT_ALLOC_MEM_FOR_PIC);
        return FALSE;
    }
    
    myTempDC = CreateCompatibleDC (myInfo -> deviceContext);
    if (myTempDC == NULL) 
    {
	MIO_MoreInfo ("GetImage: Problem creating memory context (%d)",
		      GetLastError ());
    	SET_ERRNO (E_PIC_CANT_ALLOC_MEM_FOR_PIC);
	DeleteBitmap (myBitMap);
        return FALSE;
    }

    // Set the new palette
    myOldPalette = SelectPalette (myTempDC, myInfo -> palette, FALSE);
    RealizePalette (myTempDC);

    // Select the newly created bitmap into the DC
    myOldBitMap = SelectBitmap (myTempDC, myBitMap);

    // If any part of the picture goes off the edge, then colour the
    // entire picture white first, then draw on top of it.
    if ((mySrcX < 0) || (mySrcY < 0) || (mySrcX + myWidth > myInfo -> width) ||
	(mySrcY + myHeight > myInfo -> height))
    {
	// Set the entire bitmap to white
	BitBlt (myTempDC, 0, 0, myWidth, myHeight, myInfo -> offscreenDeviceContext,
		pmX1, pmY1, WHITENESS);

	if (mySrcX < 0)
	{
	    myDstX -= mySrcX;
	    myWidth += mySrcX;
	    mySrcX = 0;
	}
	if (mySrcY < 0)
	{
	    myDstY -= mySrcY;
	    myHeight += mySrcY;
	    mySrcY = 0;
	}
	if (mySrcX + myWidth > myInfo -> width)
	{
	    myWidth = myInfo -> width - mySrcX;
	}
	if (mySrcY + myHeight > myInfo -> height)
	{
	    myHeight = myInfo -> height - mySrcY;
	}
    }

    // Blit the backing store bitmap into the newly created bitmap
    BitBlt (myTempDC, myDstX, myDstY, myWidth, myHeight, myInfo -> offscreenDeviceContext,
	    mySrcX, mySrcY, SRCCOPY);

    // Assign the BitMap to the PIC
    *myPicInfo = (MDPicInfoPtr) MDIOPic_MallocMDInfo (myBitMap, NULL);
    
    // Clean up
    SelectBitmap (myTempDC, myOldBitMap);
    SelectPalette (myTempDC, myOldPalette, FALSE);
    DeleteDC (myTempDC);

    return TRUE;
} // MDIOPic_GetImage


/************************************************************************/
/* MDIOPic_GetMaskFromMDInfo						*/
/************************************************************************/
void	*MDIOPic_GetMaskFromMDInfo (PIC pmPic)
{
    MDPicInfoPtr	myPicInfo = (MDPicInfoPtr) pmPic;

    return &myPicInfo -> mask;
} // MDIOPic_GetMaskFromMDInfo


/************************************************************************/
/* MDIOPic_GetMemSize							*/
/************************************************************************/
int	MDIOPic_GetMemSize (PIC pmPic)
{
    BITMAP		myBitMap;
    MDPicInfoPtr	myPicInfo = (MDPicInfoPtr) pmPic;
    int			mySize;

    GetObject (myPicInfo -> bmp, sizeof (BITMAP), &myBitMap);
    mySize = 1 + ((myBitMap.bmHeight * myBitMap.bmWidthBytes) >> 10);

    if (myPicInfo -> mask != NULL)
    {
	GetObject (myPicInfo -> mask, sizeof (BITMAP), &myBitMap);
	mySize += 1 + ((myBitMap.bmHeight * myBitMap.bmWidthBytes) >> 10);
    }

    return mySize;
} // MDIOPic_GetMemSize


/************************************************************************/
/* MDIOPic_IncrementUse							*/
/************************************************************************/
void	MDIOPic_IncrementUse (PIC pmPic)
{
    MDPicInfoPtr	myPicInfo = (MDPicInfoPtr) pmPic;

    myPicInfo -> refCount++;
} // MDIOPic_IncrementUse


/************************************************************************/
/* MDIOPic_IsSupportedFormat						*/
/************************************************************************/
BOOL	MDIOPic_IsSupportedFormat (int pmFormat)
{
    return ((pmFormat == BMP) || (pmFormat == JPG) || (pmFormat == GIF));
} // MDIOPic_IsSupportedFormat


/************************************************************************/
/* MDIOPic_LoadBMP							*/
/************************************************************************/
BOOL	MDIOPic_LoadBMP (const char *pmPathName, int *pmWidth,
			 int *pmHeight, PIC *pmPic)
{
    MDPicInfoPtr	*myPicInfo = (MDPicInfoPtr *) pmPic;
    HANDLE		myFile;
    BITMAPFILEHEADER	myBitMapFileHeader;
    DWORD		myBytesRead, myInfoSize, myBitsSize;
    BITMAPINFO		*myBitMapInfo;
    BYTE		*myBits;
    BOOL		mySuccess;
    HBITMAP		myBitMap;
                
    myFile = CreateFile (pmPathName, GENERIC_READ, FILE_SHARE_READ, NULL,
    			 OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (myFile == INVALID_HANDLE_VALUE)
    {
    	DWORD	myError = GetLastError ();
    	
    	if (myError == ERROR_FILE_NOT_FOUND)
    	{
	    SET_ERRMSG (E_FSYS_FILE_NOT_FOUND, "File '%s' not found", 
			pmPathName);
    	}
    	else if (myError == ERROR_PATH_NOT_FOUND)
    	{
    	    SET_ERRMSG (E_FSYS_PATH_NOT_FOUND, 
    	 	        "Path not found (couldn't find the directory "
    	 	        "that file '%s' is in)", pmPathName);
    	}
    	else
    	{
	    MIO_MoreInfo ("LoadImage: Problem opening graphics file '%s' (%d)",
		          pmPathName, GetLastError ());
	    SET_ERRNO (E_PIC_IO_ERROR);
	}		          
    	return FALSE;
    }
    
    //
    // Read the BITMAPFILEHEADER
    //
    mySuccess = ReadFile (myFile, &myBitMapFileHeader, 
    			  sizeof (BITMAPFILEHEADER), &myBytesRead, NULL);
    if (!mySuccess)
    {
	MIO_MoreInfo ("LoadImage: Problem reading graphics file '%s' (%d)",
		      pmPathName, GetLastError ());
	SET_ERRNO (E_PIC_IO_ERROR);
    	CloseHandle (myFile);
    	return FALSE;
    }
    
    if (myBytesRead != sizeof (BITMAPFILEHEADER))
    {
	MIO_MoreInfo ("LoadImage: Read failure: Only read %d of %d bytes",
		      myBytesRead, sizeof (BITMAPFILEHEADER));
	SET_ERRNO (E_PIC_IO_ERROR);
    	CloseHandle (myFile);
    	return FALSE;
    }
    
    if (myBitMapFileHeader.bfType != * (WORD *) "BM")
    {
	MIO_MoreInfo ("LoadImage: Read failure: '%s' did not have a BMP header",
		      pmPathName);
    	SET_ERRNO (E_PIC_NOT_PIC_FILE);
    	CloseHandle (myFile);
    	return FALSE;
    }

    //
    // Read the BITMAPINFO (includes BITMAPINFOHEADER & Color table
    //
    
    // Allocate memory for the information structure and read it in
    myInfoSize = myBitMapFileHeader.bfOffBits - sizeof (BITMAPFILEHEADER);
    
    myBitMapInfo = (BITMAPINFO *) malloc (myInfoSize);
    if (myBitMapInfo == NULL)
    {
	MIO_MoreInfo ("LoadImage: Unable to allocate bit map info of %d bytes",
		      myInfoSize);
	SET_ERRNO (E_PIC_CANT_ALLOC_MEM_FOR_PIC);
    	return FALSE;
    }

    mySuccess = ReadFile (myFile, myBitMapInfo, myInfoSize, &myBytesRead, NULL);
    
    if (!mySuccess)
    {
	MIO_MoreInfo ("LoadImage: Problem [2] reading graphics file '%s' (%d)",
		      pmPathName, GetLastError ());
	SET_ERRNO (E_PIC_IO_ERROR);
    	free (myBitMapInfo);
    	CloseHandle (myFile);
    	return FALSE;
    }
    
    if (myBytesRead != myInfoSize)
    {
	MIO_MoreInfo ("LoadImage: Read failure [2]: Only read %d of %d bytes",
		      myBytesRead, myInfoSize);
	SET_ERRNO (E_PIC_IO_ERROR);
    	free (myBitMapInfo);
    	CloseHandle (myFile);
    	return FALSE;
    }

    // 
    // Read in the image bits
    //
    myBitsSize = myBitMapFileHeader.bfSize - myBitMapFileHeader.bfOffBits;
    
    // Note that we must use GlobalAlloc rather than malloc because under
    // unspecified cases, CreateDIBitmap will fail (!) with an error code
    // of zero.  This is documented on Usenet, but not in MS documentation.
    myBits = (BYTE *) GlobalAlloc (GMEM_FIXED, myBitsSize);
    if (myBits == NULL)
    {
	MIO_MoreInfo ("LoadImage: Unable to allocate bit map of %d bytes",
		      myBitsSize);
	SET_ERRNO (E_PIC_CANT_ALLOC_MEM_FOR_PIC);
    	free (myBitMapInfo);
    	CloseHandle (myFile);
    	return FALSE;
    }

    mySuccess = ReadFile (myFile, myBits, myBitsSize, &myBytesRead, NULL);
    
    if (!mySuccess)
    {
	MIO_MoreInfo ("LoadImage: Problem [3] reading graphics file '%s' (%d)",
		      pmPathName, GetLastError ());
	SET_ERRNO (E_PIC_IO_ERROR);
    	GlobalFree (myBits);
    	free (myBitMapInfo);
    	CloseHandle (myFile);
    	return FALSE;
    }
    
    if (myBytesRead != myBitsSize)
    {
	MIO_MoreInfo ("LoadImage: Read failure [3]: Only read %d of %d bytes",
		      myBytesRead, myBitsSize);
	SET_ERRNO (E_PIC_IO_ERROR);
    	GlobalFree (myBits);
    	free (myBitMapInfo);
    	CloseHandle (myFile);
    	return FALSE;
    }

    CloseHandle (myFile);
    
    //
    // Convert the data to a DDB
    //
    myBitMap = CreateDIBitmap (MIO_selectedRunWindowInfo -> offscreenDeviceContext,
    			       &myBitMapInfo -> bmiHeader, CBM_INIT, myBits, 
    			       myBitMapInfo, DIB_RGB_COLORS);
    if (myBitMap == NULL)
    {
	MIO_MoreInfo ("LoadImage: Unable to create device dep bitmap (%d)",
		      GetLastError ());
	SET_ERRNO (E_PIC_IO_ERROR);
    	GlobalFree (myBits);
    	free (myBitMapInfo);
    	return FALSE;
    }
    
    if (myBitMapInfo -> bmiHeader.biSize == 12)
    {
    	*pmWidth = ((BITMAPCOREHEADER *) myBitMapInfo) -> bcWidth;
    	*pmHeight = ((BITMAPCOREHEADER *) myBitMapInfo) -> bcHeight;
    }
    else
    {
    	*pmWidth = myBitMapInfo -> bmiHeader.biWidth;
    	*pmHeight = myBitMapInfo -> bmiHeader.biHeight;
    }
    
    GlobalFree (myBits);
    free (myBitMapInfo);
    
    *myPicInfo = (MDPicInfoPtr) MDIOPic_MallocMDInfo (myBitMap, NULL);
    
    return TRUE;
} // MDIOPic_LoadBMP

			   
/************************************************************************/
/* MDIOPic_MallocMDInfo							*/
/************************************************************************/
PIC	MDIOPic_MallocMDInfo (HBITMAP pmBMP1, HBITMAP pmBMP2)
{
    MDPicInfoPtr    myPtr;

    myPtr = (MDPicInfoPtr) malloc (sizeof (MDPicInfo));
    myPtr -> bmp = pmBMP1;
    myPtr -> mask = pmBMP2;
    myPtr -> refCount = 1;

    return myPtr;
} // MDIOPic_MallocMDInfo


/************************************************************************/
/* MDIOPic_Rotate							*/
/*									*/
/* Note, the axis coordinates are in terms of Turing coords		*/
/************************************************************************/
BOOL	MDIOPic_Rotate (PIC pmSrcPic, PIC *pmDestPic, int pmSrcWidth, 
			int pmSrcHeight, int *pmDestWidth, int *pmDestHeight,
			int pmAngle, int pmXAxis, int pmYAxis, int pmBGColorNum)
{
    MDPicInfoPtr	mySrcPicInfo = (MDPicInfoPtr) pmSrcPic;
    MDPicInfoPtr	*myDestPicInfo = (MDPicInfoPtr *) pmDestPic;
    double		x [4], y [4];
    double		sine, cosine;
    double		myMinX = 9999, myMinY = 9999;
    double		myMaxX = -9999, myMaxY = -9999;
    int			myDestWidth = 1, myDestHeight = 1;
    MIOWinInfoPtr	myInfo = MIO_selectedRunWindowInfo;
    BITMAPINFO		*mySrcBitMapInfo, *myDestBitMapInfo;
    HBITMAP		myDestBitMap;
    BYTE		*mySrcBits, *myDestBits;
    int			mySrcBitMapInfoSize, mySrcBitsSize;
    int			myDestBitsSize;
    int			myBPP, myNumColours, mySrcRowBytes;
    int			myDestRowBytes;
    PALETTEENTRY	myPaletteEntry;
    COLORREF		myBGColour;
    int			myX, myY;
    BYTE		*mySrcRowPtr, *myDestRowPtr;
    double		myNewY, mySourceXFull, mySourceYFull;
    int			mySourceX, mySourceY;
    int			cnt;

    //
    // Determine the four corners of the rotation so that we can figure out
    // the size of the new picture.
    //    
    MyGetSinCos (pmAngle, &sine, &cosine);
    
    x [0] = x [3] = 0;
    x [1] = x [2] = pmSrcWidth - 1;
    y [0] = y [1] = 0;
    y [2] = y [3] = pmSrcHeight - 1;
    
    if (pmXAxis == -1)
    {
	// Rotate on (0, 0) but keep everything in picture
	for (cnt = 0 ; cnt < 4 ; cnt++)
	{
	    double	myNewX, myNewY;
	    myNewX = x [cnt] * cosine - y [cnt] * sine;
	    myNewY = y [cnt] * cosine + x [cnt] * sine;
	    myMinX = min (myMinX, myNewX);
	    myMinY = min (myMinY, myNewY);
	    myMaxX = max (myMaxX, myNewX);
	    myMaxY = max (myMaxY, myNewY);
	}
	myDestWidth = (int) (myMaxX - myMinX + 1);
	myDestHeight = (int) (myMaxY - myMinY + 1);
    }   
    else
    {
	myMinX = 0;
	myMinY = 0;

	for (cnt = 0 ; cnt < 4 ; cnt++)
	{
	    double	myNewX, myNewY;
	    myNewX = (x [cnt] - pmXAxis) * cosine - 
		     (y [cnt] - pmYAxis) * sine + pmXAxis;
	    myNewY = (y [cnt] - pmYAxis) * cosine + 
		     (x [cnt] - pmXAxis) * sine + pmYAxis;
	    myDestWidth = max (myDestWidth, (int) (myNewX + 1));
	    myDestHeight = max (myDestHeight, (int) (myNewY + 1));
	}
    } // if 

    //
    // Rotate the bits
    //
    
    // Convert the Bitmap to a DIB and extract information
    if (!MDIOUtil_ConvertBitMapToDIB (
    		   myInfo -> offscreenDeviceContext, mySrcPicInfo -> bmp, 
    		   myInfo -> palette, 0, 0, 0, 0, TRUE, &mySrcBitMapInfo, 
    		   &mySrcBitMapInfoSize, &mySrcBits, &mySrcBitsSize))
    {
    	return FALSE;
    }    		   
    myBPP = mySrcBitMapInfo -> bmiHeader.biBitCount;
    myNumColours = mySrcBitMapInfo -> bmiHeader.biClrUsed ? 
    			mySrcBitMapInfo -> bmiHeader.biClrUsed : 1 << myBPP;
    mySrcRowBytes = (((pmSrcWidth * myBPP) + 31) & ~31) >> 3;
    
    // Create the destination DIB
    myDestRowBytes = (((myDestWidth * myBPP) + 31) & ~31) >> 3;
    myDestBitsSize = myDestRowBytes * myDestHeight;
    myDestBitMapInfo = (BITMAPINFO *) malloc (mySrcBitMapInfoSize);
    // Note that we must use GlobalAlloc rather than malloc because under
    // unspecified cases, CreateDIBitmap will fail (!) with an error code
    // of zero.  This is documented on Usenet, but not in MS documentation.
    myDestBits = (BYTE *) GlobalAlloc (GMEM_FIXED, myDestBitsSize);
    memcpy (myDestBitMapInfo, mySrcBitMapInfo, mySrcBitMapInfoSize);
    myDestBitMapInfo -> bmiHeader.biWidth = myDestWidth;
    myDestBitMapInfo -> bmiHeader.biHeight = myDestHeight;
    myDestBitMapInfo -> bmiHeader.biSizeImage = myDestBitsSize;

    // Set the bitmap to the background colour
    GetPaletteEntries ((HPALETTE) myInfo -> palette, pmBGColorNum, 1,
		       &myPaletteEntry);
    myBGColour = RGB (myPaletteEntry.peRed, myPaletteEntry.peGreen, 
		      myPaletteEntry.peBlue);
    switch (myBPP)
    {
    	case 1:
    	    // Background colour is black unless bg colour is white
    	    if (myBGColour == RGB (255, 255, 255))
    	    {
    	    	memset (myDestBits, 0xff, myDestBitsSize);
    	    }
    	    else
    	    {
    	    	memset (myDestBits, 0, myDestBitsSize);
    	    }
    	    break;
	case 4:
	case 8:
	    pmBGColorNum = GetNearestColor (myInfo -> deviceContext, myBGColour);
	    if (myBPP == 4)
	    {
	    	pmBGColorNum = pmBGColorNum | (pmBGColorNum << 4);
	    }
	    memset (myDestBits, pmBGColorNum, myDestBitsSize);
    	    break;
	case 24:
	    // For 24 bit colour, the drawing routine will draw the background.
	    memset (myDestBits, 0, myDestBitsSize);
    	    break;
    	default:
	    MIO_MoreInfo ("Rotate: Cannot rotate bitmap with this depth (%d)",
		          myBPP);
	    SET_ERRMSG (E_PIC_CANT_ROTATE_THIS_DEPTH, 
	    		"Turing cannot rotate %d-bit bitmaps", myBPP);
            return FALSE;
    } // switch

    //
    // Now do the actual rotating - a pixel at a time
    // Computing the destination point for each source point
    // will leave a few pixels that do not get covered
    // So we use a reverse transform - e.i. compute the source point
    // for each destination point
    //
    if (pmXAxis == -1)
    {
	pmXAxis = 0;
	pmYAxis = 0;
    }

    myNewY = (myDestHeight - 1 - pmYAxis) + myMinY;
    myDestRowPtr = myDestBits + 
	    myDestRowBytes * (myDestHeight - 1);

    for (myY = 0; myY < myDestHeight; myY++)
    {
	mySourceXFull = (myMinX - pmXAxis) * cosine + myNewY * sine + pmXAxis + 0.5;
	mySourceYFull = (pmSrcHeight - 1) - 
		    (myNewY * cosine - (myMinX - pmXAxis) * sine + pmYAxis) + 0.5;
	
	for (myX = 0; myX < myDestWidth; myX++)
	{
	    mySourceX = (int) mySourceXFull;
	    mySourceY = (int) mySourceYFull;

	    if (((0 <= mySourceXFull) && (mySourceX < pmSrcWidth)) &&
	        ((0 <= mySourceYFull) && (mySourceY < pmSrcHeight)))
	    {	        
		BYTE	myMask, myPixel;
		DWORD	myDWPixel;

		mySrcRowPtr = mySrcBits + 
			   mySrcRowBytes * (pmSrcHeight - 1 - mySourceY);
			   
		// Set the destination pixel
		switch (myBPP)
		{
		    case 1: // Monochrome
			myMask = *(mySrcRowPtr + (mySourceX >> 3)) & 
			         (0x80 >> (mySourceX % 8));
			//Adjust mask for destination bitmap
			myMask = myMask ? (0x80 >> (myX % 8)) : 0;
			*(myDestRowPtr + (myX >> 3)) &= ~(0x80 >> (myX % 8));
			*(myDestRowPtr + (myX >> 3)) |= myMask;
			break;
		    case 4:
			myMask = *(mySrcRowPtr + (mySourceX >> 1)) & 
			         ((mySourceX & 1) ? 0x0f : 0xf0);
			// Adjust mask for destination bitmap
			if ((mySourceX & 1) != (myX & 1))
			{
			    myMask = (myMask & 0xf0) ? (myMask >> 4) : (myMask << 4);
			}
			*(myDestRowPtr + (myX >> 1)) &= ~((myX & 1) ? 0x0f : 0xf0);
			*(myDestRowPtr + (myX >> 1)) |= myMask;
			break;
		    case 8:
			myPixel = *(mySrcRowPtr + mySourceX);
			*(myDestRowPtr + myX) = myPixel;
			break;
		    case 24:
			myDWPixel = * (DWORD *) (mySrcRowPtr + mySourceX * 3) 
				    & 0xffffff;
			* (DWORD *) (myDestRowPtr + myX * 3) |= myDWPixel;
		} // switch
	    } // if (point in bounds)
	    else 
	    {
		// Draw the background color. The background color
		// has already been drawn for 8 bits per pixel and less
		if (myBPP == 24)
		{
		    * (DWORD *) (myDestRowPtr + myX * 3) |= myBGColour;
		}
	    }

	    mySourceXFull += cosine;
	    mySourceYFull += sine;
	} // for myX

	myNewY--;
	myDestRowPtr -= myDestRowBytes;
    } // for myY

    //
    // Convert the data to a DDB
    //
    myDestBitMap = CreateDIBitmap (
		    MIO_selectedRunWindowInfo -> offscreenDeviceContext,
    		    &myDestBitMapInfo -> bmiHeader, CBM_INIT, myDestBits,
    		    myDestBitMapInfo, DIB_RGB_COLORS);
    if (myDestBitMap == NULL)
    {
	MIO_MoreInfo ("Rotate: Unable to create device dep bitmap (%d)",
		      GetLastError ());
	SET_ERRNO (E_PIC_IO_ERROR);
    	GlobalFree (myDestBits);
    	free (myDestBitMapInfo);
    	return FALSE;
    }

    *myDestPicInfo = (MDPicInfoPtr) MDIOPic_MallocMDInfo (myDestBitMap, NULL);
        
    GlobalFree (myDestBits);
    free (myDestBitMapInfo);
    
    *pmDestWidth = myDestWidth;
    *pmDestHeight = myDestHeight;
        		   
    return TRUE;
} // MDIOPic_Rotate

			
/************************************************************************/
/* MDIOPic_SaveImage							*/
/************************************************************************/
BOOL	MDIOPic_SaveImage (const char *pmPathName, PIC pmPic)
{
    MIOWinInfoPtr	myInfo = MIO_selectedRunWindowInfo;
    MDPicInfoPtr	myPicInfo = (MDPicInfoPtr) pmPic;
    BOOL		myResult;
    BITMAPINFO		*myBitMapInfo;
    BITMAPFILEHEADER	myBitMapFileHeader;
    BYTE		*myBits;
    int			myBitMapInfoSize, myBitsSize;
    DWORD		myBytesWritten;
    HANDLE		myFile;
    BOOL		mySuccess;
    
    myResult = MDIOUtil_ConvertBitMapToDIB (
    		   myInfo -> offscreenDeviceContext, myPicInfo -> bmp, 
    		   myInfo -> palette, 0, 0, 0, 0, TRUE, &myBitMapInfo, 
    		   &myBitMapInfoSize, &myBits, &myBitsSize);

    if (!myResult)
    {
	SET_ERRNO (E_PIC_IO_ERROR);
    	return FALSE;
    }
    
    //
    // We've got all the data, now write the file!
    //
    
    // Fill in a file header

    myBitMapFileHeader.bfType = * (WORD *) "BM";
    myBitMapFileHeader.bfSize = sizeof(BITMAPFILEHEADER) + myBitMapInfoSize +
    				myBitsSize;
    myBitMapFileHeader.bfReserved1 = 0;
    myBitMapFileHeader.bfReserved2 = 0;
    myBitMapFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + myBitMapInfoSize;

    // Save file header, bitmap header, and image data

    myFile = CreateFile (pmPathName, GENERIC_WRITE, 0, NULL,
    			 CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (myFile == INVALID_HANDLE_VALUE)
    {
    	DWORD	myError = GetLastError ();
    	
    	if (myError == ERROR_FILE_NOT_FOUND)
    	{
	    SET_ERRMSG (E_FSYS_FILE_NOT_FOUND, "File '%s' not found", 
			pmPathName);
    	}
    	else if (myError == ERROR_PATH_NOT_FOUND)
    	{
    	    SET_ERRMSG (E_FSYS_PATH_NOT_FOUND, 
    	 	        "Path not found (couldn't find the directory "
    	 	        "that file '%s' is in)", pmPathName);
    	}
    	else
    	{
	    MIO_MoreInfo ("SaveImage: Problem opening graphics file '%s' (%d)",
		          pmPathName, GetLastError ());
	    SET_ERRNO (E_PIC_IO_ERROR);
	}		          
    	return FALSE;
    }
    
    //
    // Write the BITMAPFILEHEADER
    //
    mySuccess = WriteFile (myFile, &myBitMapFileHeader, 
    			   sizeof (BITMAPFILEHEADER), &myBytesWritten, NULL);
    if (!mySuccess)
    {
	MIO_MoreInfo ("SaveImage: Problem writing graphics file '%s' (%d)",
		      pmPathName, GetLastError ());
	SET_ERRNO (E_PIC_IO_ERROR);
    	CloseHandle (myFile);
    	return FALSE;
    }
    
    if (myBytesWritten != sizeof (BITMAPFILEHEADER))
    {
	MIO_MoreInfo ("SaveImage: Write failure: Only wrote %d of %d bytes",
		      myBytesWritten, sizeof (BITMAPFILEHEADER));
	SET_ERRNO (E_PIC_IO_ERROR);
    	CloseHandle (myFile);
    	return FALSE;
    }
    
    //
    // Write the BITMAPINFO (includes BITMAPINFOHEADER & Color table)
    //
    
    mySuccess = WriteFile (myFile, myBitMapInfo, myBitMapInfoSize, 
    			   &myBytesWritten, NULL);
    
    if (!mySuccess)
    {
	MIO_MoreInfo ("SaveImage: Problem [2] writing graphics file '%s' (%d)",
		      pmPathName, GetLastError ());
	SET_ERRNO (E_PIC_IO_ERROR);
    	CloseHandle (myFile);
    	return FALSE;
    }
    
    if ((int) myBytesWritten != myBitMapInfoSize)
    {
	MIO_MoreInfo ("SaveImage: Write failure [2]: Only wrote %d of %d bytes",
		      myBytesWritten, myBitMapInfoSize);
	SET_ERRNO (E_PIC_IO_ERROR);
    	CloseHandle (myFile);
    	return FALSE;
    }

    // 
    // Write the image bits
    //
    mySuccess = WriteFile (myFile, myBits, myBitsSize, &myBytesWritten, NULL);
    
    if (!mySuccess)
    {
	MIO_MoreInfo ("SaveImage: Problem [3] writing graphics file '%s' (%d)",
		      pmPathName, GetLastError ());
	SET_ERRNO (E_PIC_IO_ERROR);
    	CloseHandle (myFile);
    	return FALSE;
    }
    
    if ((int) myBytesWritten != myBitsSize)
    {
	MIO_MoreInfo ("SaveImage: Read failure [3]: Only wrote %d of %d bytes",
		      myBytesWritten, myBitsSize);
	SET_ERRNO (E_PIC_IO_ERROR);
    	CloseHandle (myFile);
    	return FALSE;
    }

    CloseHandle (myFile);
    
    return TRUE;
} // MDIOPic_SaveImage


/************************************************************************/
/* MDIOPic_Scale							*/
/************************************************************************/
BOOL	MDIOPic_Scale (PIC pmSrcPic, PIC *pmDestPic, 
		       int pmSrcWidth, int pmSrcHeight, 
		       int pmDestWidth, int pmDestHeight)
{
    MDPicInfoPtr	mySrcPicInfo = (MDPicInfoPtr) pmSrcPic;
    MDPicInfoPtr	*myDestPicInfo = (MDPicInfoPtr *) pmDestPic;
    MIOWinInfoPtr	myInfo = MIO_selectedRunWindowInfo;
    HBITMAP		myNewBitMap, mySrcOldBitMap, myDestOldBitMap;
    HDC			mySrcDC, myDestDC;
    HPALETTE		mySrcOldPalette, myDestOldPalette;
    int			myDestX = 0, myDestY = 0;
    
    //
    // Create and prepare the new bitmap
    //
    myNewBitMap = CreateCompatibleBitmap (myInfo -> deviceContext, 
    					  abs (pmDestWidth), 
    					  abs (pmDestHeight));
    if (myNewBitMap == NULL) 
    {
	MIO_MoreInfo ("GetImage: Problem creating bitmap (%d)",
		      GetLastError ());
	SET_ERRNO (E_PIC_CANT_ALLOC_MEM_FOR_PIC);
        return FALSE;
    }
    
    myDestDC = CreateCompatibleDC (myInfo -> deviceContext);
    if (myDestDC == NULL) 
    {
	MIO_MoreInfo ("GetImage: Problem creating memory context (%d)",
		      GetLastError ());
    	SET_ERRNO (E_PIC_CANT_ALLOC_MEM_FOR_PIC);
	DeleteBitmap (myNewBitMap);
        return FALSE;
    }

    // Set the new palette
    myDestOldPalette = SelectPalette (myDestDC, myInfo -> palette, FALSE);
    RealizePalette (myDestDC);

    // Select the newly created bitmap into the DC
    myDestOldBitMap = SelectBitmap (myDestDC, myNewBitMap);

    //
    // Prepare the source bitmap
    //
    mySrcDC = CreateCompatibleDC (myInfo -> deviceContext);
    if (mySrcDC == NULL) 
    {
	MIO_MoreInfo ("Draw: Problem creating memory context (%d)",
		      GetLastError ());
    	SET_ERRNO (E_PIC_CANT_ALLOC_MEM_FOR_PIC);
    	return FALSE;
    }

    // Set the new palette
    mySrcOldPalette = SelectPalette (mySrcDC, myInfo -> palette, FALSE);
    RealizePalette (mySrcDC);

    // Select the newly created bitmap into the DC
    mySrcOldBitMap = SelectBitmap (mySrcDC, mySrcPicInfo -> bmp);

    //
    // Copy the bits
    //
    if (pmDestWidth < 0) 
    {
    	myDestX = -pmDestWidth - 1;
    }
    if (pmDestHeight < 0) 
    {
    	myDestY = -pmDestHeight - 1;
    }
    
    StretchBlt (myDestDC, myDestX, myDestY, pmDestWidth, pmDestHeight, mySrcDC,
    		0, 0, pmSrcWidth, pmSrcHeight, SRCCOPY);

    // The bitmap with the pixels must not be selected into the DC
    // at this point if we're doing a GetDIBits (see MS doc).
    SelectBitmap (mySrcDC, mySrcOldBitMap);
    SelectBitmap (myDestDC, myDestOldBitMap);

    // Assign the BitMap to the PIC
    *myDestPicInfo = (MDPicInfoPtr) MDIOPic_MallocMDInfo (myNewBitMap, NULL);
    
    // Clean up
    SelectPalette (mySrcDC, mySrcOldPalette, FALSE);
    SelectPalette (myDestDC, myDestOldPalette, FALSE);
    DeleteDC (mySrcDC);
    DeleteDC (myDestDC);

    return TRUE;
} // MDIOPic_Scale


/************************************************************************/
/* MDIOPic_SizeImage							*/
/*									*/
/* Returns the necessary size of the pic buffer in bytes	    
/************************************************************************/
int	MDIOPic_SizePic (OOTint pmX1, OOTint pmY1, OOTint pmX2, OOTint pmY2)
{
    int		myBitMapInfoSize, myBitsSize;
    BOOL	myResult;
    
//    MDIO_OutputSystemInfo ("Before SizePic");

    myResult = MDIOUtil_ConvertBitMapToDIB (
    		   MIO_selectedRunWindowInfo -> offscreenDeviceContext, NULL,
		   MIO_selectedRunWindowInfo -> palette,
    		   pmX1, pmY1, pmX2 - pmX1 + 1, pmY2 - pmY1 + 1,
    		   FALSE, NULL, &myBitMapInfoSize, NULL, &myBitsSize);

//    MDIO_OutputSystemInfo ("After SizePic");

    // Add sizeof (int) to allow for offset in first byte
    // Add 16 to allow for 16-byte alignment.
    return myBitMapInfoSize + myBitsSize + sizeof (int) + 16;    	
} // MDIOPic_SizePic


/************************************************************************/
/* MDIOPic_SpriteDraw							*/
/************************************************************************/
void	MDIOPic_SpriteDraw (MIOWinInfoPtr pmInfo, PIC pmPic, int pmX, int pmY, 
			    int pmWidth, int pmHeight, int pmTransparentColour)
{
    MDPicInfoPtr	myPicInfo = (MDPicInfoPtr) pmPic;
    HBITMAP		myOldBitMap, myMask;
    HDC			myTempDC;
    HPALETTE		myOldPalette;
    COLORREF		myTransparentColour;
    int			myNewX = pmX, myNewY = pmY;
    int			myNewWidth = pmWidth, myNewHeight = pmHeight;
    int			mySrcX = 0, mySrcY = 0;
    
    if (myPicInfo == NULL)
    {
	myTempDC = pmInfo -> offscreenDeviceContext;
	mySrcX = pmX;
	mySrcY = pmY;
    }
    else
    {
	myTempDC = CreateCompatibleDC (pmInfo -> deviceContext);
	if (myTempDC == NULL) 
	{
	    MIO_MoreInfo ("Draw: Problem creating memory context (%d)",
			  GetLastError ());
    	    SET_ERRNO (E_PIC_CANT_ALLOC_MEM_FOR_PIC);
	    return;
	}
	
	// Set the new palette
	myOldPalette = SelectPalette (myTempDC, pmInfo -> palette, FALSE);
	RealizePalette (myTempDC);
	
	// Select the newly created bitmap into the DC
	myOldBitMap = SelectBitmap (myTempDC, myPicInfo -> bmp);
    }

    // Adjust coordinates to fit on the output window
    if (pmX < 0)
    {
        myNewX = 0;
        mySrcX -= pmX;
	myNewWidth += pmX;
    }
    if (pmY < 0)
    {
        myNewY = 0;
        mySrcY -= pmY;
	myNewHeight += pmY;
    }
    myNewWidth = min (pmInfo -> width, myNewWidth);
    myNewHeight = min (pmInfo -> height, myNewHeight);

    myTransparentColour = 
	(COLORREF) MIORGB_ConvertColour (pmTransparentColour);
    
    // Create mask of picture, if necessary
    if (myPicInfo == NULL)
    {
	myMask = NULL;
    }
    else 
    {
	if (myPicInfo -> mask == NULL)
	{
	    myPicInfo -> mask = MyCreateMask (pmInfo -> spriteDeviceContext,
					      myTempDC, pmWidth, pmHeight,
					      myTransparentColour);
	}
	myMask = myPicInfo -> mask;
    }

    // Blit the picture onto the backing store
    MyTransparentBlt (pmInfo -> spriteDeviceContext, 
	myNewX, myNewY, myNewWidth, myNewHeight, 
	myTempDC, mySrcX, mySrcY, 
	myTransparentColour, myMask);

    // Clean up
    if (myPicInfo != NULL)
    {
	SelectBitmap (myTempDC, myOldBitMap);
	SelectPalette (myTempDC, myOldPalette, FALSE);
	DeleteDC (myTempDC);
    }
} // MDIOPic_SpriteDraw


/************************************************************************/
/* MDIOPic_TakePic							*/
/************************************************************************/
BOOL	MDIOPic_TakePic (OOTint pmX1, OOTint pmY1, OOTint pmX2, 
			 OOTint pmY2, PICBUFFER pmBuffer)
{
    int		myBitMapInfoSize, myBitsSize;
    BOOL	myResult;

//    MDIO_OutputSystemInfo ("Before TakePic");

    myResult = MDIOUtil_ConvertBitMapToDIB (
    		    MIO_selectedRunWindowInfo -> offscreenDeviceContext, NULL,
		    MIO_selectedRunWindowInfo -> palette,
    		    pmX1, pmY1, pmX2 - pmX1 + 1, pmY2 - pmY1 + 1, FALSE, 
    		    (BITMAPINFO **) &pmBuffer, &myBitMapInfoSize, NULL, 
		    &myBitsSize);

//    MDIO_OutputSystemInfo ("After TakePic");

    return myResult;
} // MDIOPic_TakePic


/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/
/************************************************************************/
/* MyBlend								*/
/*									*/
/* Blend bitmaps pmBitMap1 and pmBitMap2 together and write the results	*/
/* to bitmap pmDstBitMap.  pmBitMap1 and pmBitMap2 must have the same	*/
/* dimensions.								*/	
/* This routine is used for the						*/
/* 1) Pic.Blend function (in which case the three BitMaps are all	*/
/*    pictures.  pmX1 = pmY1 = pmX2 = pmY2 = pmDstX = pmDstY = 0.	*/
/* 2) Pic.DrawSpecial with blend transition, in which case pmBitMap2	*/
/*    and pmDstBitMap will actually be the run window's offscren bitmap	*/
/*    and pmX2, and pmDstX will contain the coordinates of the part of	*/
/*    the screen being written to.  pmX1 = pmY1 = 0.  pmX2 = pmDstX.	*/
/*    pmY2 = pmDstY							*/
/* 3) Pic.DrawSpecial with the fadein transition, in which case		*/
/*    pmBitMap1 and pmBitMap2 are both pictures, and pmDstBitMap is the	*/
/*    run window offscreen bitmap.  pmX1 = pmY1 = pmX2 = pmY2 = 0.	*/
/*									*/
/* pmPercent - How much to weigh pmBitMap1 (0 - 100)			*/
/* pmWidth, pmHeight - The size of the area being blended.		*/
/* pmWindowWidth, pmWindowHeight - The run window's width and height.	*/
/* pmWindowDeviceContext - The run window's device context.		*/
/* pmDisplayOnScreen - Should we copy the result to the onscreen window	*/
/* pmPalette - The run window's palette.				*/
/************************************************************************/
static void	MyBlend (HBITMAP pmBitMap1, int pmX1, int pmY1, 
			 HBITMAP pmBitMap2, int pmX2, int pmY2, 
			 int pmWidth, int pmHeight, int pmPercent,
			 HDC pmDstDC, HBITMAP pmDstBitMap, 
			 int pmDstX, int pmDstY,
			 int pmWindowWidth, int pmWindowHeight,
			 HDC pmWindowDeviceContext,
			 BOOL pmDisplayOnScreen,
			 HPALETTE pmPalette)
{
    BITMAPINFO		*myBitMap1Info, *myBitMap2Info, *myDstBitMapInfo;
    HDC			myDC1, myDC2;
    unsigned char	myConv1 [256], myConv2 [256];
    HPALETTE		myOldPalette1, myOldPalette2;
    HBITMAP		myOldBitMap1, myOldBitMap2;
    COLORREF		myColour1, myColour2, myDstColour;
    int			myBytesPerLine, myDstBytesPerLine, myBytesPerPixel;
    char		*myOrigBits1, *myBits1;
    char		*myOrigBits2 = NULL, *myBits2 = NULL;
    char		*myOrigDstBits, *myDstBits;
    int			myRedMask, myGreenMask, myBlueMask;
    int			myRedShift, myGreenShift, myBlueShift;
    DWORD		myPixel1, myPixel2;
    char		*myPixel1Ptr, *myPixel2Ptr, *myDstPixelPtr;
    BYTE		myRed1, myGreen1, myBlue1;
    BYTE		myRed2, myGreen2, myBlue2;
    int			myScanY, myCutOffBottom = 0;
    int			x, y, cnt;

    // Adjust the location coordinates if we're being asked to draw outside
    // the edges of the window.
    if (pmDstX < 0)
    {
	pmX1 -= pmDstX;
	pmX2 -= pmDstX;
	pmWidth += pmDstX;
	pmDstX -= pmDstX;
    }
    if (pmDstY < 0)
    {
	pmY1 -= pmDstY;
	pmY2 -= pmDstY;
	pmHeight += pmDstY;
	pmDstY -= pmDstY;
    }
    if (pmWidth > pmWindowWidth - pmDstX)
    {
	pmWidth = pmWindowWidth - pmDstX;
    }
    if (pmHeight > pmWindowHeight - pmDstY)
    {
	myCutOffBottom = pmHeight - (pmWindowHeight - pmDstY);
	pmHeight = pmWindowHeight - pmDstY;
    }
    
    // The coordinates and sizes now all fit in the window.    

    //
    // Get information about the bitmap
    //

    // Allocate a DC to use
    myDC1 = CreateCompatibleDC (pmWindowDeviceContext);
    if (myDC1 == NULL) 
    {
	MIO_MoreInfo ("Draw: Problem creating memory context (%d)",
		      GetLastError ());
    	SET_ERRNO (E_PIC_CANT_ALLOC_MEM_FOR_PIC);
    	return;
    }

    // Allocate the extra space for the BI_BITFIELDS color map
    myBitMap1Info = (BITMAPINFO *) malloc (sizeof(BITMAPINFO) + 
	sizeof(RGBQUAD) * 2);
    ZeroMemory (myBitMap1Info, sizeof (BITMAPINFO));
    myBitMap1Info -> bmiHeader.biSize = sizeof (myBitMap1Info -> bmiHeader);
    GetDIBits (myDC1, pmBitMap1, 0, 0, NULL, myBitMap1Info, DIB_RGB_COLORS);
    
    myBitMap2Info = (BITMAPINFO *) malloc (sizeof(BITMAPINFO) + 
	sizeof(RGBQUAD) * 2);
    ZeroMemory (myBitMap2Info, sizeof (BITMAPINFO));
    myBitMap2Info -> bmiHeader.biSize = sizeof (myBitMap2Info -> bmiHeader);
    GetDIBits (myDC1, pmBitMap2, 0, 0, NULL, myBitMap2Info, DIB_RGB_COLORS);
    
    myDstBitMapInfo = (BITMAPINFO *) malloc (sizeof(BITMAPINFO) + 
	sizeof(RGBQUAD) * 2);
    ZeroMemory (myDstBitMapInfo, sizeof (BITMAPINFO));
    myDstBitMapInfo -> bmiHeader.biSize = sizeof (myDstBitMapInfo -> bmiHeader);
    GetDIBits (myDC1, pmDstBitMap, 0, 0, NULL, myDstBitMapInfo, DIB_RGB_COLORS);
    
    // Set the conversion tables
    for (cnt = 0 ; cnt < 256 ; cnt++)
    {
	myConv1 [cnt] = (unsigned char) ((float) cnt * pmPercent / 100 + 0.5);
	myConv2 [cnt] = cnt - myConv1 [cnt];
    } // for
        
    if ((myBitMap1Info -> bmiHeader.biBitCount <= 8) ||
	(myDstBitMapInfo -> bmiHeader.biBitCount != 
	    myBitMap1Info -> bmiHeader.biBitCount))
    {
	//
	// We're dealing with 1, 2, 4, or 8 bits
	//
	myOldPalette1 = SelectPalette (myDC1, pmPalette, FALSE);
	RealizePalette (myDC1);
	myOldBitMap1 = SelectBitmap (myDC1, pmBitMap1);

	// If we're using a separate background picture, then we'll
	// need a second DC
	if (pmBitMap2 != pmDstBitMap)
	{
	    myDC2 = CreateCompatibleDC (pmWindowDeviceContext);
	    if (myDC2 == NULL) 
	    {
		MIO_MoreInfo ("Draw: Problem creating memory context (%d)",
		    GetLastError ());
		SET_ERRNO (E_PIC_CANT_ALLOC_MEM_FOR_PIC);
		return;
	    }
	    
	    // Set the new palette
	    myOldPalette2 = SelectPalette (myDC2, pmPalette, FALSE);
	    RealizePalette (myDC2);
	    
	    // Select the newly created bitmap into the DC
	    myOldBitMap2 = SelectBitmap (myDC2, pmBitMap2);
	}
	else
	{
	    // We'll be taking the background from the main offscreen bitmap.
	    myDC2 = pmDstDC;
	}

	for (y = 0 ; y < pmHeight ; y++)
	{
	    for (x = 0 ; x < pmWidth ; x++)
	    {
		myColour1 = GetPixel (myDC1, x + pmX1, y + pmX1);
		myColour2 = GetPixel (myDC2, x + pmX2, y + pmY2);
		myDstColour = RGB (myConv1 [GetRValue (myColour1)] + 
		    myConv2 [GetRValue (myColour2)],
		    myConv1 [GetGValue (myColour1)] + 
		    myConv2 [GetGValue (myColour2)],
		    myConv1 [GetBValue (myColour1)] + 
		    myConv2 [GetBValue (myColour2)]);
		SetPixel (pmDstDC, x + pmDstX, y + pmDstY, myDstColour);
	    } // for
	} // for

	// Clean up
	SelectBitmap (myDC1, myOldBitMap1);
	SelectPalette (myDC1, myOldPalette1, FALSE);
	
	if (pmBitMap1 != pmBitMap2)
	{
	    // If necessary, clean up the second DC for a separate bg image.
	    SelectBitmap (myDC2, myOldBitMap2);
	    SelectPalette (myDC2, myOldPalette2, FALSE);
	}
    }
    else
    {
	//
	// We're dealing with a non-palette system (16, 24, or 32 bits)
	//

	// If the height is positive, then this is a "bottom up" bitmap.  Scan
	// line 0 is at the *bottom*.  However, the 
	if (myDstBitMapInfo -> bmiHeader.biHeight > 0)
	{
	    myScanY = myDstBitMapInfo -> bmiHeader.biHeight - pmHeight - pmDstY;
	}
	else
	{
	    myScanY = pmDstY;
	}

	// Find out how many bytes in a line and then dword align the result.
	myBytesPerLine = (myBitMap1Info -> bmiHeader.biWidth * 
	    myBitMap1Info -> bmiHeader.biBitCount) / 8;
	myBytesPerLine = (myBytesPerLine + 3) & ~3;
	myBytesPerPixel = myBitMap1Info -> bmiHeader.biBitCount / 8;

	myDstBytesPerLine = (myDstBitMapInfo -> bmiHeader.biWidth * 
	    myDstBitMapInfo -> bmiHeader.biBitCount) / 8;
	myDstBytesPerLine = (myDstBytesPerLine + 3) & ~3;
	
	// Allocate space and fetch the bits from the source bitmap
	myOrigBits1 = malloc (myBytesPerLine * pmHeight + 16);
	myBits1 = (char *) ((int) (myOrigBits1 + 0xF) & ~0xF);
	GetDIBits (myDC1, pmBitMap1, myCutOffBottom, pmHeight, myBits1, 
	    myBitMap1Info, DIB_RGB_COLORS);

	if (pmBitMap2 != pmDstBitMap)
	{
	    // Allocate space and fetch the bits from the source bitmap
	    myOrigBits2 = malloc (myBytesPerLine * pmHeight + 16);
	    myBits2 = (char *) ((int) (myOrigBits2 + 0xF) & ~0xF);
	    GetDIBits (myDC1, pmBitMap2, myCutOffBottom, pmHeight, myBits2, 
		myBitMap2Info, DIB_RGB_COLORS);
	}

	// Allocate space and fetch the bits from the offscreen bitmap
	myOrigDstBits = malloc (myDstBytesPerLine * pmHeight + 16);
	myDstBits = (char *) ((int) (myOrigDstBits + 0xF) & ~0xF);
	// According to the MS documentation for GetDIBits, the HBITMAP must 
	// not be selected into *any* device context.  It was selected into 
	// myInfo -> offscreenDeviceContext, so we Select a different bitmap 
	// into it, and then select the real bitmap back into it (after we've
	// done the SetDIBits).
	SelectBitmap (pmDstDC, NULL);
	GetDIBits (pmDstDC, pmDstBitMap, myScanY, pmHeight, 
	    myDstBits, myDstBitMapInfo, DIB_RGB_COLORS);

	// Get the colour masks that depend on the colour depth and other
	// factors.
	MyGetColourMasks (myBitMap1Info, &myRedMask, &myGreenMask, &myBlueMask,
			  &myRedShift, &myGreenShift, &myBlueShift);

	for (y = 0 ; y < pmHeight ; y++)
	{
	    myPixel1Ptr = (char *) (y * myBytesPerLine + myBits1 + pmX1 * myBytesPerPixel);
	    if (myBits2 != NULL)
	    {
		myPixel2Ptr = (char *) (y * myBytesPerLine + myBits2 + pmX2 * myBytesPerPixel);
	    }
	    else
	    {
		// The second bitmap and the destination bitmap are the same
		myPixel2Ptr = (char *) (y * myDstBytesPerLine + myDstBits + pmDstX * myBytesPerPixel);
	    }
	    myDstPixelPtr = (char *) (y * myDstBytesPerLine + myDstBits + pmDstX * myBytesPerPixel);
    
	    for (x = 0 ; x < pmWidth ; x++)
	    {
		myPixel1 = * (DWORD *) myPixel1Ptr;
		myPixel2 = * (DWORD *) myPixel2Ptr;

		myRed1 = (BYTE) (myPixel1 >> myRedShift) & myRedMask;
		myGreen1 = (BYTE) (myPixel1 >> myGreenShift) & myGreenMask;
		myBlue1 = (BYTE) (myPixel1 >> myBlueShift) & myBlueMask;
		myRed2 = (BYTE) (myPixel2 >> myRedShift) & myRedMask;
		myGreen2 = (BYTE) (myPixel2 >> myGreenShift) & myGreenMask;
		myBlue2 = (BYTE) (myPixel2 >> myBlueShift) & myBlueMask;
		* (DWORD *) myDstPixelPtr = 
		    ((myConv1 [myRed1] + myConv2 [myRed2]) << myRedShift) +
		    ((myConv1 [myGreen1] + myConv2 [myGreen2]) << myGreenShift) +
		    ((myConv1 [myBlue1] + myConv2 [myBlue2]) << myBlueShift);
		myPixel1Ptr += myBytesPerPixel;
		myPixel2Ptr += myBytesPerPixel;
		myDstPixelPtr += myBytesPerPixel;
	    } // for
	} // for

	SetDIBits (pmDstDC, pmDstBitMap, myScanY, pmHeight, 
	    myDstBits, myDstBitMapInfo, DIB_RGB_COLORS);

	// Now select the offscreen bitmap back into the offscreen context
	SelectBitmap (pmDstDC, pmDstBitMap);

	free (myOrigBits1);
	if (myOrigBits2 != NULL)
	{
	    free (myOrigBits2);
	}
	free (myOrigDstBits);
    }

    if (pmDisplayOnScreen)
    {
	// Blit the picture fromthe backing store onto the screen
	BitBlt (pmWindowDeviceContext, pmDstX, pmDstY,
	    pmWidth, pmHeight, pmDstDC, pmDstX, pmDstY, SRCCOPY);
    }

    // Clean up
    DeleteDC (myDC1);

    if (pmBitMap1 != pmBitMap2)
    {
	// If necessary, clean up the second DC for a separate bg image.
	DeleteDC (myDC2);
    }

    free (myBitMap1Info);
    free (myBitMap2Info);
    free (myDstBitMapInfo);
} // MyBlend


/************************************************************************/
/* MyBlur								*/
/*									*/
/* Blend bitmaps pmBitMap1 and pmBitMap2 together and write the results	*/
/* to bitmap pmDstBitMap.  pmBitMap1 and pmBitMap2 must have the same	*/
/* dimensions.								*/	
/* This routine is used for the						*/
/* 1) Pic.Blend function (in which case the three BitMaps are all	*/
/*    pictures.  pmX1 = pmY1 = pmX2 = pmY2 = pmDstX = pmDstY = 0.	*/
/* 2) Pic.DrawSpecial with blend transition, in which case pmBitMap2	*/
/*    and pmDstBitMap will actually be the run window's offscren bitmap	*/
/*    and pmX2, and pmDstX will contain the coordinates of the part of	*/
/*    the screen being written to.  pmX1 = pmY1 = 0.  pmX2 = pmDstX.	*/
/*    pmY2 = pmDstY							*/
/* 3) Pic.DrawSpecial with the fadein transition, in which case		*/
/*    pmBitMap1 and pmBitMap2 are both pictures, and pmDstBitMap is the	*/
/*    run window offscreen bitmap.  pmX1 = pmY1 = pmX2 = pmY2 = 0.	*/
/*									*/
/* pmPercent - How much to weigh pmBitMap1 (0 - 100)			*/
/* pmWidth, pmHeight - The size of the area being blended.		*/
/* pmWindowWidth, pmWindowHeight - The run window's width and height.	*/
/* pmWindowDeviceContext - The run window's device context.		*/
/* pmDisplayOnScreen - Should we copy the result to the onscreen window	*/
/* pmPalette - The run window's palette.				*/
/************************************************************************/
static void	MyBlur (HBITMAP pmBitMap, int pmX, int pmY, 
			int pmWidth, int pmHeight, int pmIterations,
			HDC pmDstDC, HBITMAP pmDstBitMap, 
			int pmDstX, int pmDstY,
			int pmWindowWidth, int pmWindowHeight,
			HDC pmWindowDeviceContext,
			BOOL pmDisplayOnScreen)
{
    BITMAPINFO	*myBitMapInfo, *myDstBitMapInfo;
    HDC		myDC;
    int		myBytesPerLine, myDstBytesPerLine, myBytesPerPixel;
    char	*myOrigBits, *myBits;
    char	*myOrigDstBits1, *myDstBits1;
    char	*myOrigDstBits2, *myDstBits2;
    int		myRedMask, myGreenMask, myBlueMask;
    int		myRedShift, myGreenShift, myBlueShift;
    DWORD	myPixel;
    DWORD	myRedPrevLinePrev, myRedPrevLineCurr = 0, myRedPrevLineNext;
    DWORD	myRedCurrLinePrev, myRedCurrLineCurr = 0, myRedCurrLineNext;
    DWORD	myRedNextLinePrev, myRedNextLineCurr = 0, myRedNextLineNext;
    DWORD	myGreenPrevLinePrev, myGreenPrevLineCurr = 0, myGreenPrevLineNext;
    DWORD	myGreenCurrLinePrev, myGreenCurrLineCurr = 0, myGreenCurrLineNext;
    DWORD	myGreenNextLinePrev, myGreenNextLineCurr = 0, myGreenNextLineNext;
    DWORD	myBluePrevLinePrev, myBluePrevLineCurr = 0, myBluePrevLineNext;
    DWORD	myBlueCurrLinePrev, myBlueCurrLineCurr = 0, myBlueCurrLineNext;
    DWORD	myBlueNextLinePrev, myBlueNextLineCurr = 0, myBlueNextLineNext;
    char	*myPixelPrevLinePtr, *myPixelCurrLinePtr, *myPixelNextLinePtr;
    char	*myDstPixelPtr;
    int		myRedTotal, myGreenTotal, myBlueTotal, myTotal;
    int		myScanY;
    int		x, y, cnt;

    // Adjust the location coordinates if we're being asked to draw outside
    // the edges of the window.
    if (pmDstX < 0)
    {
	pmX -= pmDstX;
	pmDstX -= pmDstX;
	pmWidth += pmDstX;
    }
    if (pmDstY < 0)
    {
	pmY -= pmDstY;
	pmDstY -= pmDstY;
	pmHeight += pmDstY;
    }
    if (pmWidth > pmWindowWidth - pmDstX)
    {
	pmWidth = pmWindowWidth - pmDstX;
    }
    if (pmHeight > pmWindowHeight - pmDstY)
    {
	pmHeight = pmWindowHeight - pmDstY;
    }
    
    // The coordinates and sizes now all fit in the window.    

    //
    // Get information about the bitmap
    //

    // Allocate a DC to use
    myDC = CreateCompatibleDC (pmWindowDeviceContext);
    if (myDC == NULL) 
    {
	MIO_MoreInfo ("Blur: Problem creating memory context (%d)",
		      GetLastError ());
    	SET_ERRNO (E_PIC_CANT_ALLOC_MEM_FOR_PIC);
    	return;
    }

    // Allocate the extra space for the BI_BITFIELDS color map
    myBitMapInfo = (BITMAPINFO *) malloc (sizeof(BITMAPINFO) + 
	sizeof(RGBQUAD) * 2);
    ZeroMemory (myBitMapInfo, sizeof (BITMAPINFO));
    myBitMapInfo -> bmiHeader.biSize = sizeof (myBitMapInfo -> bmiHeader);
    GetDIBits (myDC, pmBitMap, 0, 0, NULL, myBitMapInfo, DIB_RGB_COLORS);
    
    myDstBitMapInfo = (BITMAPINFO *) malloc (sizeof(BITMAPINFO) + 
	sizeof(RGBQUAD) * 2);
    ZeroMemory (myDstBitMapInfo, sizeof (BITMAPINFO));
    myDstBitMapInfo -> bmiHeader.biSize = sizeof (myDstBitMapInfo -> bmiHeader);
    GetDIBits (myDC, pmDstBitMap, 0, 0, NULL, myDstBitMapInfo, DIB_RGB_COLORS);
    
    if (myBitMapInfo -> bmiHeader.biBitCount <= 8)
    {
	// TW - Error too few bits!
    }
    else
    {
	//
	// We're dealing with a non-palette system (16, 24, or 32 bits)
	//

	// If the height is positive, then this is a "bottom up" bitmap.  Scan
	// line 0 is at the *bottom*.  However, the 
	if (myDstBitMapInfo -> bmiHeader.biHeight > 0)
	{
	    myScanY = myDstBitMapInfo -> bmiHeader.biHeight - pmHeight - pmDstY;
	}
	else
	{
	    myScanY = pmDstY;
	}

	// Find out how many bytes in a line and then dword align the result.
	myBytesPerLine = (myBitMapInfo -> bmiHeader.biWidth * 
	    myBitMapInfo -> bmiHeader.biBitCount) / 8;
	myBytesPerLine = (myBytesPerLine + 3) & ~3;
	myBytesPerPixel = myBitMapInfo -> bmiHeader.biBitCount / 8;

	myDstBytesPerLine = (myDstBitMapInfo -> bmiHeader.biWidth * 
	    myDstBitMapInfo -> bmiHeader.biBitCount) / 8;
	myDstBytesPerLine = (myDstBytesPerLine + 3) & ~3;
	
	// Allocate space and fetch the bits from the source bitmap
	myOrigBits = malloc (myBytesPerLine * pmHeight + 16);
	if (myOrigBits == NULL)
	{
	    MIO_MoreInfo ("Blur: Not enough memory to allocate offscreen "
			  "bitmap (%d bytes) (%d)", 
			  myBytesPerLine * pmHeight + 16, GetLastError ());
    	    SET_ERRNO (E_PIC_CANT_ALLOC_MEM_FOR_PIC);
	    free (myBitMapInfo);
	    free (myDstBitMapInfo);
	    DeleteDC (myDC);
    	    return;
	}
	myBits = (char *) ((int) (myOrigBits + 0xF) & ~0xF);
	GetDIBits (myDC, pmBitMap, pmY, pmHeight, myBits, 
	    myBitMapInfo, DIB_RGB_COLORS);

	// Allocate space and fetch the bits from the offscreen bitmap
	myOrigDstBits1 = malloc (myDstBytesPerLine * pmHeight + 16);
	if (myOrigDstBits1 == NULL)
	{
	    MIO_MoreInfo ("Blur: Not enough memory to allocate offscreen "
			  "bitmap (%d bytes) (%d)", 
			  myDstBytesPerLine * pmHeight + 16, GetLastError ());
    	    SET_ERRNO (E_PIC_CANT_ALLOC_MEM_FOR_PIC);
	    free (myOrigBits);
	    free (myBitMapInfo);
	    free (myDstBitMapInfo);
	    DeleteDC (myDC);
	    return;
	}
	myDstBits1 = (char *) ((int) (myOrigDstBits1 + 0xF) & ~0xF);
	// According to the MS documentation for GetDIBits, the HBITMAP must 
	// not be selected into *any* device context.  It was selected into 
	// myInfo -> offscreenDeviceContext, so we Select a different bitmap 
	// into it, and then select the real bitmap back into it (after we've
	// done the SetDIBits).
	SelectBitmap (pmDstDC, NULL);
	GetDIBits (pmDstDC, pmDstBitMap, myScanY, pmHeight, 
	    myDstBits1, myDstBitMapInfo, DIB_RGB_COLORS);

	// Allocate space and fetch the bits for the destination bitmap
	if (pmIterations > 1)
	{
	    myOrigDstBits2 = malloc (myDstBytesPerLine * pmHeight + 16);
	    if (myOrigDstBits2 == NULL)
	    {
		MIO_MoreInfo ("Blur: Not enough memory to allocate offscreen "
		    "bitmap (%d bytes) (%d)", 
		    myDstBytesPerLine * pmHeight + 16, GetLastError ());
		SET_ERRNO (E_PIC_CANT_ALLOC_MEM_FOR_PIC);
		free (myOrigBits);
		free (myOrigDstBits1);
		free (myBitMapInfo);
		free (myDstBitMapInfo);
		DeleteDC (myDC);
		return;
	    }
	    myDstBits2 = (char *) ((int) (myOrigDstBits2 + 0xF) & ~0xF);
	    memcpy (myDstBits2, myDstBits1, myDstBytesPerLine * pmHeight);
	}

	// Get the colour masks that depend on the colour depth and other
	// factors.
	MyGetColourMasks (myBitMapInfo, &myRedMask, &myGreenMask, &myBlueMask,
			  &myRedShift, &myGreenShift, &myBlueShift);

	for (cnt = 0 ; cnt < pmIterations ; cnt++)
	{
	    for (y = 0 ; y < pmHeight ; y++)
	    {
		if (y > 0)
		{
		    if (cnt == 0)
		    {
			myPixelPrevLinePtr = (char *) ((y - 1) * myBytesPerLine + myBits);
		    }
		    else
		    {
			if ((cnt % 2) == 1)
			{
			    myPixelPrevLinePtr = (char *) ((y - 1) * myDstBytesPerLine + myDstBits1 + pmDstX * myBytesPerPixel);
			}
			else
			{
			    myPixelPrevLinePtr = (char *) ((y - 1) * myDstBytesPerLine + myDstBits2 + pmDstX * myBytesPerPixel);
			}
		    }
		    myPixel = * (DWORD *) myPixelPrevLinePtr;
		    myRedPrevLineNext = (BYTE) (myPixel >> myRedShift) & myRedMask;
		    myGreenPrevLineNext = (BYTE) (myPixel >> myGreenShift) & myGreenMask;
		    myBluePrevLineNext = (BYTE) (myPixel >> myBlueShift) & myBlueMask;
		    myPixelPrevLinePtr += myBytesPerPixel;
		}
		
		if (cnt == 0)
		{
		    myPixelCurrLinePtr = (char *) (y * myBytesPerLine + myBits);
		}
		else
		{
		    if ((cnt % 2) == 1)
		    {
			myPixelCurrLinePtr = (char *) (y * myDstBytesPerLine + myDstBits1 + pmDstX * myBytesPerPixel);
		    }
		    else
		    {
			myPixelCurrLinePtr = (char *) (y * myDstBytesPerLine + myDstBits2 + pmDstX * myBytesPerPixel);
		    }
		}
		myPixel = * (DWORD *) myPixelCurrLinePtr;
		myRedCurrLineNext = (BYTE) (myPixel >> myRedShift) & myRedMask;
		myGreenCurrLineNext = (BYTE) (myPixel >> myGreenShift) & myGreenMask;
		myBlueCurrLineNext = (BYTE) (myPixel >> myBlueShift) & myBlueMask;
		myPixelCurrLinePtr += myBytesPerPixel;
		
		if (y < pmHeight - 1)
		{
		    if (cnt == 0)
		    {
			myPixelNextLinePtr = (char *) ((y + 1) * myBytesPerLine + myBits);
		    }
		    else
		    {
			if ((cnt % 2) == 1)
			{
			    myPixelNextLinePtr = (char *) ((y + 1) * myDstBytesPerLine + myDstBits1 + pmDstX * myBytesPerPixel);
			}
			else
			{
			    myPixelNextLinePtr = (char *) ((y + 1) * myDstBytesPerLine + myDstBits2 + pmDstX * myBytesPerPixel);
			}
		    }
		    myPixel = * (DWORD *) myPixelNextLinePtr;
		    myRedNextLineNext = (BYTE) (myPixel >> myRedShift) & myRedMask;
		    myGreenNextLineNext = (BYTE) (myPixel >> myGreenShift) & myGreenMask;
		    myBlueNextLineNext = (BYTE) (myPixel >> myBlueShift) & myBlueMask;
		    myPixelNextLinePtr += myBytesPerPixel;
		}
		
		if ((cnt % 2) == 1)
		{
		    myDstPixelPtr = (char *) (y * myDstBytesPerLine + myDstBits2 + pmDstX * myBytesPerPixel);
		}
		else
		{
		    myDstPixelPtr = (char *) (y * myDstBytesPerLine + myDstBits1 + pmDstX * myBytesPerPixel);
		}
		
		for (x = 0 ; x < pmWidth ; x++)
		{
		    //
		    // Set the red, green, blue values for the 
		    // nine pixels around (x, y)
		    //
		    myRedPrevLinePrev = myRedPrevLineCurr;
		    myGreenPrevLinePrev = myGreenPrevLineCurr;
		    myBluePrevLinePrev = myBluePrevLineCurr;
		    myRedPrevLineCurr = myRedPrevLineNext;
		    myGreenPrevLineCurr = myGreenPrevLineNext;
		    myBluePrevLineCurr = myBluePrevLineNext;
		    myRedCurrLinePrev = myRedCurrLineCurr;
		    myGreenCurrLinePrev = myGreenCurrLineCurr;
		    myBlueCurrLinePrev = myBlueCurrLineCurr;
		    myRedCurrLineCurr = myRedCurrLineNext;
		    myGreenCurrLineCurr = myGreenCurrLineNext;
		    myBlueCurrLineCurr = myBlueCurrLineNext;
		    myRedNextLinePrev = myRedNextLineCurr;
		    myGreenNextLinePrev = myGreenNextLineCurr;
		    myBlueNextLinePrev = myBlueNextLineCurr;
		    myRedNextLineCurr = myRedNextLineNext;
		    myGreenNextLineCurr = myGreenNextLineNext;
		    myBlueNextLineCurr = myBlueNextLineNext;
		    
		    myRedTotal = 0;
		    myGreenTotal = 0;
		    myBlueTotal = 0;
		    myTotal = 0;
		    
		    if (x < pmWidth - 1)
		    {
			if (y > 0)
			{
			    myPixel = * (DWORD *) myPixelPrevLinePtr;
			    myRedPrevLineNext = (BYTE) (myPixel >> myRedShift) & myRedMask;
			    myGreenPrevLineNext = (BYTE) (myPixel >> myGreenShift) & myGreenMask;
			    myBluePrevLineNext = (BYTE) (myPixel >> myBlueShift) & myBlueMask;
			}
			myPixel = * (DWORD *) myPixelCurrLinePtr;
			myRedCurrLineNext = (BYTE) (myPixel >> myRedShift) & myRedMask;
			myGreenCurrLineNext = (BYTE) (myPixel >> myGreenShift) & myGreenMask;
			myBlueCurrLineNext = (BYTE) (myPixel >> myBlueShift) & myBlueMask;
			if (y < pmHeight - 1)
			{
			    myPixel = * (DWORD *) myPixelNextLinePtr;
			    myRedNextLineNext = (BYTE) (myPixel >> myRedShift) & myRedMask;
			    myGreenNextLineNext = (BYTE) (myPixel >> myGreenShift) & myGreenMask;
			    myBlueNextLineNext = (BYTE) (myPixel >> myBlueShift) & myBlueMask;
			}
		    }
		    
		    //
		    // Sum up the red, green, and blue values for the 
		    // nine pixels around (x, y)
		    //
		    if (y > 0)
		    {
			// Upper-left: 1
			if (x > 0)
			{
			    myRedTotal += myRedPrevLinePrev;
			    myGreenTotal += myGreenPrevLinePrev;
			    myBlueTotal += myBluePrevLinePrev;
			    myTotal += 1;
			}
			// Upper-centre: 2
			myRedTotal += myRedPrevLineCurr << 1;
			myGreenTotal += myGreenPrevLineCurr << 1;
			myBlueTotal += myBluePrevLineCurr << 1;
			myTotal += 2;
			// Upper-left: 1
			if (x < pmWidth - 1)
			{
			    myRedTotal += myRedPrevLineNext;
			    myGreenTotal += myGreenPrevLineNext;
			    myBlueTotal += myBluePrevLineNext;
			    myTotal += 1;
			}
		    }
		    
		    // Middle-left: 2
		    if (x > 0)
		    {
			myRedTotal += myRedCurrLinePrev << 1;
			myGreenTotal += myGreenCurrLinePrev << 1;
			myBlueTotal += myBlueCurrLinePrev << 1;
			myTotal += 2;
		    }
		    // Upper-middle: 8
		    myRedTotal += myRedCurrLineCurr << 3;
		    myGreenTotal += myGreenCurrLineCurr << 3;
		    myBlueTotal += myBlueCurrLineCurr << 3;
		    myTotal += 8;
		    // Upper-left: 1
		    if (x < pmWidth - 1)
		    {
			myRedTotal += myRedCurrLineNext << 1;
			myGreenTotal += myGreenCurrLineNext << 1;
			myBlueTotal += myBlueCurrLineNext << 1;
			myTotal += 2;
		    }
		    
		    if (y < pmHeight - 1)
		    {
			// Bottom-left: 1
			if (x > 0)
			{
			    myRedTotal += myRedNextLinePrev;
			    myGreenTotal += myGreenNextLinePrev;
			    myBlueTotal += myBlueNextLinePrev;
			    myTotal += 1;
			}
			// Bottom-centre: 2
			myRedTotal += myRedNextLineCurr << 1;
			myGreenTotal += myGreenNextLineCurr << 1;
			myBlueTotal += myBlueNextLineCurr << 1;
			myTotal += 2;
			// Bottom-left: 1
			if (x < pmWidth - 1)
			{
			    myRedTotal += myRedNextLineNext;
			    myGreenTotal += myGreenNextLineNext;
			    myBlueTotal += myBlueNextLineNext;
			    myTotal += 1;
			}
		    }
		
		    //
		    // Set the destination pixel
		    //
		    * (DWORD *) myDstPixelPtr = 
			((myRedTotal / myTotal) << myRedShift) +
			((myGreenTotal / myTotal) << myGreenShift) +
			((myBlueTotal / myTotal) << myBlueShift);
		    
		    myPixelPrevLinePtr += myBytesPerPixel;
		    myPixelCurrLinePtr += myBytesPerPixel;
		    myPixelNextLinePtr += myBytesPerPixel;
		    myDstPixelPtr += myBytesPerPixel;
		} // for (x = 0 ; x < pmWidth ; x++)
	    } // for (y = 0 ; y < pmHeight ; y++)
	} // for (cnt = 0 ; cnt < pmIterations ; cnt++)

	if ((pmIterations % 2) == 0)
	{
	    SetDIBits (pmDstDC, pmDstBitMap, myScanY, pmHeight, 
		myDstBits2, myDstBitMapInfo, DIB_RGB_COLORS);
	}
	else
	{
	    SetDIBits (pmDstDC, pmDstBitMap, myScanY, pmHeight, 
		myDstBits1, myDstBitMapInfo, DIB_RGB_COLORS);
	}

	// Now select the offscreen bitmap back into the offscreen context
	SelectBitmap (pmDstDC, pmDstBitMap);

	free (myOrigBits);
	free (myOrigDstBits1);
	if (pmIterations > 1)
	{
	    free (myOrigDstBits2);
	}
    }

    if (pmDisplayOnScreen)
    {
	// Blit the picture fromthe backing store onto the screen
	BitBlt (pmWindowDeviceContext, pmDstX, pmDstY,
	    pmWidth, pmHeight, pmDstDC, pmDstX, pmDstY, SRCCOPY);
    }

    // Clean up
    free (myBitMapInfo);
    free (myDstBitMapInfo);
    DeleteDC (myDC);
} // MyBlur


/************************************************************************/
/* MyCreateMask								*/
/************************************************************************/
static HBITMAP	MyCreateMask (HDC pmDestDC, HDC pmSourceDC, 
			      int pmWidth, int pmHeight,
			      COLORREF pmTransColor)
{
    HDC 	myMaskDC;
    HBITMAP 	myMaskBMP;
    HBITMAP 	myOldMaskBMP;
    HDC 	myTempDC;
    HBITMAP 	myTempBMP;
    HBITMAP 	myOldTempBMP;
    COLORREF	myOldColorSrc, myOldColorDst;

    //-- The Background colors of Source and Destination DCs must
    //   be the transparancy color in order to create a mask.
    myOldColorSrc = SetBkColor (pmSourceDC, pmTransColor);
    myOldColorDst = SetBkColor (pmDestDC, pmTransColor);

    // The mask DC must be compatible with the destination dc,
    // but the mask has to be created as a monochrome bitmap.
    // For this reason, we create a compatible dc and bitblt
    // the mono mask into it.

    // Create the Mask DC, and a compatible bitmap to go in it.
    myMaskDC = CreateCompatibleDC (pmDestDC);

    myMaskBMP = CreateCompatibleBitmap (pmDestDC, pmWidth, pmHeight);

    // Move the Mask bitmap into the Mask DC
    myOldMaskBMP = SelectObject (myMaskDC, myMaskBMP);

    // Create a monochrome bitmap that will be the actual mask bitmap.
    myTempBMP = CreateBitmap (pmWidth, pmHeight, 1, 1, 0);

    // Create a temporary DC, and put the mono bitmap into it
    myTempDC = CreateCompatibleDC (pmDestDC);
    myOldTempBMP = SelectObject (myTempDC, myTempBMP);
    
    // BitBlt the Source image into the mono dc to create a mono mask.
    if (BitBlt (myTempDC, 0, 0, pmWidth, pmHeight, pmSourceDC, 
		0, 0, SRCCOPY)) 
    {
	// Copy the mono mask into our Mask DC
	if (!BitBlt (myMaskDC, 0, 0, pmWidth, pmHeight, myTempDC, 0, 0, 
	    SRCCOPY))
	{
	    return NULL;
	}
    }
    else 
    {
	return NULL;
    }
    
    // Clean up temp DC and bitmap
    myTempBMP = SelectObject (myTempDC, myOldTempBMP);
    DeleteObject (myTempBMP);
    DeleteDC (myTempDC);

    // Clean up mask DC and bitmap
    SelectObject (myMaskDC, myOldMaskBMP);
    DeleteDC (myMaskDC);

    // Restore old background values
    SetBkColor (pmSourceDC, myOldColorSrc);
    SetBkColor (pmDestDC, myOldColorDst);

    return myMaskBMP;
} // MyCreateMask


/************************************************************************/
/* MyGetColourMasks							*/
/************************************************************************/
static void	MyGetColourMasks (BITMAPINFO *pmBMI, 
		    int *pmRedMask, int *pmGreenMask, int *pmBlueMask,
		    int *pmRedShift, int *pmGreenShift, int *pmBlueShift)
{
    int	myBitFormat;

    // Possible bit patterns:
    //   16, 5-5-5 (BI_RGB or BI_BITFIELDS and appropriate masks)
    //   16, 5-6-5 (BI_BITFIELDS and appropriate masks)
    //   24, 8-8-8
    //   32, 8-8-8-0
    if (pmBMI -> bmiHeader.biBitCount == 16)
    {
	if (pmBMI -> bmiHeader.biCompression == BI_RGB)
	{
	    myBitFormat = B16_5_5_5;
	}
	else if (pmBMI -> bmiHeader.biCompression == BI_BITFIELDS)
	{
	    if (* (DWORD *) &pmBMI -> bmiColors [1] == 0x03e0)
	    {
		myBitFormat = B16_5_5_5;
	    }
	    else if (* (DWORD *) &pmBMI -> bmiColors [1] == 0x07e0)
	    {
		myBitFormat = B16_5_6_5;
	    }
	}
    }
    else if (pmBMI -> bmiHeader.biBitCount == 24)
    {
	myBitFormat = B24;
    }
    else if (pmBMI -> bmiHeader.biBitCount == 32)
    {
	myBitFormat = B32;
    }
    
    // Set mask and shifts
    switch (myBitFormat)
    {
	case B16_5_5_5:
	    *pmRedMask = 0x1f;
	    *pmGreenMask = 0x1f;
	    *pmBlueMask = 0x1f;
	    *pmRedShift = 10;
	    *pmGreenShift = 5;
	    *pmBlueShift = 0;
	    break;
	case B16_5_6_5:
	    *pmRedMask = 0x1f;
	    *pmGreenMask = 0x3f;
	    *pmBlueMask = 0x1f;
	    *pmRedShift = 11;
	    *pmGreenShift = 5;
	    *pmBlueShift = 0;
	    break;
	case B24:
	    *pmRedMask = 0xff;
	    *pmGreenMask = 0xff;
	    *pmBlueMask = 0xff;
	    *pmRedShift = 16;
	    *pmGreenShift = 8;
	    *pmBlueShift = 0;
	    break;
	case B32:
	    *pmRedMask = 0xff;
	    *pmGreenMask = 0xff;
	    *pmBlueMask = 0xff;
	    *pmRedShift = 16;
	    *pmGreenShift = 8;
	    *pmBlueShift = 0;
	    break;
	default:
	    // TW - Error
	    break;
    } // switch
} // MyGetColourMasks


/************************************************************************/
/* MyGetSinCos								*/
/*									*/
/* We use a special routine in order to get exact values for right	*/
/* angles.								*/
/************************************************************************/
static void	MyGetSinCos (int pmAngle, double *pmSine, double *pmCosine)
{
    *pmSine = sin (RAD (pmAngle));
    *pmCosine = cos (RAD (pmAngle));

    if (fabs (*pmSine) < 1e-6) *pmSine = 0;
    if (fabs (*pmSine - 1) < 1e-6) *pmSine = 1;
    if (fabs (*pmSine + 1) < 1e-6) *pmSine = -1;
    if (fabs (*pmSine - 0.5) < 1e-6) *pmSine = 0.5;
    if (fabs (*pmSine + 0.5) < 1e-6) *pmSine = -0.5;

    if (fabs (*pmCosine) < 1e-6) *pmCosine = 0;
    if (fabs (*pmCosine - 1) < 1e-6) *pmCosine = 1;
    if (fabs (*pmCosine + 1) < 1e-6) *pmCosine = -1;
    if (fabs (*pmCosine - 0.5) < 1e-6) *pmCosine = 0.5;
    if (fabs (*pmCosine + 0.5) < 1e-6) *pmCosine = -0.5;
} // MyGetSinCos


/************************************************************************/
/* MyTransparentBlt							*/
/************************************************************************/
static BOOL	MyTransparentBlt (HDC pmDestDC, int pmDestX, int pmDestY,
			          int pmWidth, int pmHeight, HDC pmSourceDC,
			          int pmSourceX, int pmSourceY, 
			          COLORREF pmTransColor, HBITMAP pmMask)
{
    COLORREF	myOldColorSrc, myOldColorDst;
    HDC 	myMaskDC;
    HBITMAP 	myMaskBmp;
    HBITMAP 	myOldMaskBmp;
    int		myMaskSourceX = pmSourceX, myMaskSourceY = pmSourceY;

    //-- The Background colors of Source and Destination DCs must
    //   be the transparancy color in order to create a mask.
    myOldColorSrc = SetBkColor (pmSourceDC, pmTransColor);
    myOldColorDst = SetBkColor (pmDestDC, pmTransColor);

    //-- The mask DC must be compatible with the destination dc,
    //   but the mask has to be created as a monochrome bitmap.
    //   For this reason, we create a compatible dc and bitblt
    //   the mono mask into it.

    //-- Create the Mask DC, and a compatible bitmap to go in it.
    myMaskDC = CreateCompatibleDC (pmDestDC);

    if (pmMask == NULL)
    {
	HBITMAP 	myTempBmp;
	HDC 		myTempDC;
	HBITMAP 	myOldTempBmp;

	myMaskBmp = CreateCompatibleBitmap (pmDestDC, pmWidth, pmHeight);
	//-- Move the Mask bitmap into the Mask DC
	myOldMaskBmp = SelectObject (myMaskDC, myMaskBmp);
	
	//-- Create a monochrome bitmap that will be the actual mask bitmap.
	myTempBmp = CreateBitmap (pmWidth, pmHeight, 1, 1, 0);
	//-- Create a temporary DC, and put the mono bitmap into it
	myTempDC = CreateCompatibleDC (pmDestDC);
	myOldTempBmp = SelectObject (myTempDC, myTempBmp);
	
	// BitBlt the Source image into the mono dc to create a mono mask.
	if (BitBlt (myTempDC, 0, 0, pmWidth, pmHeight, pmSourceDC, 
		    pmSourceX, pmSourceY, SRCCOPY)) 
	{
	    // Copy the mono mask into our Mask DC
	    if (!BitBlt (myMaskDC, 0, 0, pmWidth, pmHeight, myTempDC, 0, 0, 
			 SRCCOPY))
	    {
		return FALSE;
	    }
	}
	else 
	{
	    return FALSE;
	}
	
	// Clean up temp DC and bitmap
	myTempBmp = SelectObject (myTempDC, myOldTempBmp);
	DeleteObject (myTempBmp);
	DeleteDC (myTempDC);

	// If we created a temporary mask, then it is set only to the 
	// size we'll actually be drawing.
	myMaskSourceX = 0;
	myMaskSourceY = 0;
    }
    else
    {
	myOldMaskBmp = SelectObject (myMaskDC, pmMask);
    }

    // Copy the source to the destination with XOR
    if (BitBlt (pmDestDC, pmDestX, pmDestY, pmWidth, pmHeight, pmSourceDC, 
    		pmSourceX, pmSourceY, SRCINVERT)) 
    {
        //-- Copy the Mask to the destination with AND
        if (BitBlt (pmDestDC, pmDestX, pmDestY, pmWidth, pmHeight, myMaskDC, 
        	    myMaskSourceX, myMaskSourceY, SRCAND)) 
        {
            //-- Again, copy the source to the destination with XOR
            if (!BitBlt (pmDestDC, pmDestX, pmDestY, pmWidth, pmHeight, 
            		 pmSourceDC, pmSourceX, pmSourceY, SRCINVERT))
            {
		return FALSE;
	    }
            //-- If all these worked, we were successful.
        }
	else
	{
	    return FALSE;
	}
    }
    else
    {
	return FALSE;
    }

    // Clean up mask DC and bitmap
    SelectObject (myMaskDC, myOldMaskBmp);
    DeleteDC (myMaskDC);
    if (pmMask == NULL)
    {
	DeleteObject (myMaskBmp);
    }

    // Restore old background values
    SetBkColor (pmSourceDC, myOldColorSrc);
    SetBkColor (pmDestDC, myOldColorDst);
    
    return TRUE;
} // MyTransparentBlt


