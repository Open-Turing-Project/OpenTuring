/**************/
/* mdiodraw.c */
/**************/

/*******************/
/* System includes */
/*******************/
#include <windows.h>
#include <windowsx.h>
#include <math.h>

/****************/
/* Self include */
/****************/
#include "mdiodraw.h"

/******************/
/* Other includes */
/******************/
#include "mio.h"
#include "miowin.h"
#include "miorgb.h"
#include "mioerr.h"

/**********/
/* Macros */
/**********/
#define PI		(3.14159265)
#define RAD(a)		(((double) a) * PI / 180.0)

#define ROUND(x)	((unsigned int) ((x)+0.5))

/*************/
/* Constants */
/*************/
#define MAX_POLY_POINTS   		500

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
static void	MyGetArcPoints (OOTint pmX, OOTint pmY, OOTint pmAngle, 
				OOTint pmXRadius, OOTint pmYRadius,
				short *pmXPos, short *pmYPos);
static void	MySetRect (RECT *pmRect, int pmX1, int pmY1, int pmX2, 
			   int pmY2);
static void	MySetRunWindowBrush (OOTint pmClr);
static void	MySetRunWindowPen (int pmStyle, int pmWidth, OOTint pmClr);


/***********************/
/* External procedures */
/***********************/
/************************************************************************/
/* MDIODraw_Arc								*/
/************************************************************************/
void	MDIODraw_Arc (OOTint pmX, OOTint pmY, OOTint pmXRadius, 
		      OOTint pmYRadius, OOTint pmInitAngle,
		      OOTint pmFinalAngle, OOTint pmStyle, OOTint pmWidth,
		      OOTint pmClr, BOOL pmFill)
{
    MIOWinInfoPtr	myInfo = MIOWin_GetInfo (MIO_selectedRunWindow);
    short		myXStart, myYStart, myXEnd, myYEnd;
    RECT		myRect;
    
    MySetRunWindowPen (pmStyle, pmWidth, pmClr);

    if (myInfo -> xor)
    {
	SetROP2 (myInfo -> deviceContext, R2_XORPEN);
	SetROP2 (myInfo -> offscreenDeviceContext, R2_XORPEN);
    }

    MyGetArcPoints (pmX, pmY, pmInitAngle, pmXRadius, pmYRadius, &myXStart, 
    		    &myYStart);
    MyGetArcPoints (pmX, pmY, pmFinalAngle, pmXRadius, pmYRadius, &myXEnd, 
    		    &myYEnd);

    MySetRect (&myRect, pmX - pmXRadius, pmY - pmYRadius, pmX + pmXRadius, 
    	       pmY + pmYRadius);

    if (pmFill) 
    {
	MySetRunWindowBrush (pmClr);

	if (myInfo -> displayOnScreen)
	{
    	    Pie (myInfo -> deviceContext, myRect.left, myRect.top, 
		 myRect.right + 1, myRect.bottom + 1, myXStart, myYStart, 
		 myXEnd, myYEnd);
	}
    	Pie (myInfo -> offscreenDeviceContext, myRect.left, myRect.top, 
    	     myRect.right + 1, myRect.bottom + 1, myXStart, myYStart, 
    	     myXEnd, myYEnd);
    }
    else 
    {
	if (myInfo -> displayOnScreen)
	{
	    Arc (myInfo -> deviceContext, myRect.left, myRect.top, 
		 myRect.right + 1, myRect.bottom + 1, myXStart, myYStart, 
		 myXEnd, myYEnd);
	    SetPixel (myInfo -> deviceContext, myXEnd, myYEnd, 
		(COLORREF) MIORGB_ConvertColour (pmClr + 1));
	}
	Arc (myInfo -> offscreenDeviceContext, myRect.left, myRect.top, 
    	     myRect.right + 1, myRect.bottom + 1, myXStart, myYStart, 
    	     myXEnd, myYEnd);
        SetPixel (myInfo -> offscreenDeviceContext, myXEnd, myYEnd, 
    	      (COLORREF) MIORGB_ConvertColour (pmClr + 1));
    }

    if (myInfo -> xor)
    {
	SetROP2 (myInfo -> deviceContext, R2_COPYPEN);
	SetROP2 (myInfo -> offscreenDeviceContext, R2_COPYPEN);
    }
} // MDIODraw_Arc


/************************************************************************/
/* MDIODraw_Box								*/
/************************************************************************/
void	MDIODraw_Box (OOTint pmX1, OOTint pmY1, OOTint pmX2, OOTint pmY2, 
		      OOTint pmStyle, OOTint pmWidth, OOTint pmClr, BOOL pmFill)
{
    MIOWinInfoPtr	myInfo = MIOWin_GetInfo (MIO_selectedRunWindow);
    RECT		myRect;
    
    MySetRunWindowPen (pmStyle, pmWidth, pmClr);
    MySetRect (&myRect, pmX1, pmY1, pmX2, pmY2);
    
    if (myInfo -> xor)
    {
	SetROP2 (myInfo -> deviceContext, R2_XORPEN);
	SetROP2 (myInfo -> offscreenDeviceContext, R2_XORPEN);
    }

    if (pmFill) 
    {
	MySetRunWindowBrush (pmClr);
	if (myInfo -> displayOnScreen)
	{
	    Rectangle (myInfo -> deviceContext, myRect.left, myRect.top, 
    		       myRect.right + 1, myRect.bottom + 1);
	}
	Rectangle (myInfo -> offscreenDeviceContext, myRect.left, myRect.top, 
		   myRect.right + 1, myRect.bottom + 1);
    }
    else 
    {
        POINT	myPoint [5];

	myPoint [0].x = myRect.left;
	myPoint [0].y = myRect.top;
	myPoint [1].x = myRect.right;
	myPoint [1].y = myRect.top;
	myPoint [2].x = myRect.right;
	myPoint [2].y = myRect.bottom;
	myPoint [3].x = myRect.left;
	myPoint [3].y = myRect.bottom;
	myPoint [4].x = myRect.left;
	myPoint [4].y = myRect.top;

	if (myInfo -> displayOnScreen)
	{
	    Polyline (myInfo -> deviceContext, myPoint, 5);
	}
	Polyline (myInfo -> offscreenDeviceContext, myPoint, 5);
    }

    if (myInfo -> xor)
    {
	SetROP2 (myInfo -> deviceContext, R2_COPYPEN);
	SetROP2 (myInfo -> offscreenDeviceContext, R2_COPYPEN);
    }
} // MDIODraw_Box		      


/************************************************************************/
/* MDIODraw_Dot								*/
/************************************************************************/
void	MDIODraw_Dot (OOTint pmX, OOTint pmY, OOTint pmClr)
{
    MIOWinInfoPtr	myInfo = MIOWin_GetInfo (MIO_selectedRunWindow);
    
    if (myInfo -> xor)
    {
	SetROP2 (myInfo -> deviceContext, R2_XORPEN);
	SetROP2 (myInfo -> offscreenDeviceContext, R2_XORPEN);
    }

    if (myInfo -> displayOnScreen)
    {
	SetPixel (myInfo -> deviceContext, pmX, pmY, 
    	          (COLORREF) MIORGB_ConvertColour (pmClr)); 
    }
    SetPixel (myInfo -> offscreenDeviceContext, pmX, pmY, 
    	      (COLORREF) MIORGB_ConvertColour (pmClr));

    if (myInfo -> xor)
    {
	SetROP2 (myInfo -> deviceContext, R2_COPYPEN);
	SetROP2 (myInfo -> offscreenDeviceContext, R2_COPYPEN);
    }
} // MDIODraw_Dot


/************************************************************************/
/* MDIODraw_Fill							*/
/************************************************************************/
void	MDIODraw_Fill (OOTint pmX, OOTint pmY, OOTint pmFillClr, 
		       OOTint pmBorderClr)
{
    MIOWinInfoPtr	myInfo = MIOWin_GetInfo (MIO_selectedRunWindow);
    HBRUSH		myCurrentBrush, myNewBrush;

    if (myInfo -> xor)
    {
	SetROP2 (myInfo -> deviceContext, R2_XORPEN);
	SetROP2 (myInfo -> offscreenDeviceContext, R2_XORPEN);
    }

    // Create the brush for the flood fill
    myNewBrush = CreateSolidBrush ((COLORREF) MIORGB_ConvertColour (pmFillClr));

    // Set the brush inthe device contexts
    if (myInfo -> displayOnScreen)
    {
	myCurrentBrush = SelectBrush (myInfo -> deviceContext, myNewBrush);
	FloodFill (myInfo -> deviceContext, pmX, pmY,
    	           (COLORREF) MIORGB_ConvertColour (pmBorderClr));
	SelectBrush (myInfo -> deviceContext, myCurrentBrush);
    }

    myCurrentBrush = SelectBrush (myInfo -> offscreenDeviceContext, myNewBrush);
    FloodFill (myInfo -> offscreenDeviceContext, pmX, pmY, 
    	       (COLORREF) MIORGB_ConvertColour (pmBorderClr));
    SelectBrush (myInfo -> offscreenDeviceContext, myCurrentBrush);

    DeleteBrush (myNewBrush);

    if (myInfo -> xor)
    {
	SetROP2 (myInfo -> deviceContext, R2_COPYPEN);
	SetROP2 (myInfo -> offscreenDeviceContext, R2_COPYPEN);
    }
} // MDIODraw_Fill


/************************************************************************/
/* MDIODraw_Line							*/
/************************************************************************/
void	MDIODraw_Line (OOTint pmX1, OOTint pmY1, OOTint pmX2, OOTint pmY2, 
		       OOTint pmStyle, OOTint pmWidth, OOTint pmClr)
{
    MIOWinInfoPtr	myInfo = MIOWin_GetInfo (MIO_selectedRunWindow);

    MySetRunWindowPen (pmStyle, pmWidth, pmClr);
    
    if (myInfo -> xor)
    {
	SetROP2 (myInfo -> deviceContext, R2_XORPEN);
	SetROP2 (myInfo -> offscreenDeviceContext, R2_XORPEN);
    }

    if (myInfo -> displayOnScreen)
    {
	SetBkMode (myInfo -> deviceContext, TRANSPARENT);
	MoveToEx (myInfo -> deviceContext, pmX1, pmY1, NULL);
	LineTo (myInfo -> deviceContext, pmX2, pmY2);
	SetPixel (myInfo -> deviceContext, pmX2, pmY2, 
	          (COLORREF) MIORGB_ConvertColour (pmClr)); 
	SetBkMode (myInfo -> deviceContext, OPAQUE);
    }
    
    SetBkMode (myInfo -> offscreenDeviceContext, TRANSPARENT);
    MoveToEx (myInfo -> offscreenDeviceContext, pmX1, pmY1, NULL);
    LineTo (myInfo -> offscreenDeviceContext, pmX2, pmY2);
    SetPixel (myInfo -> offscreenDeviceContext, pmX2, pmY2, 
    	      (COLORREF) MIORGB_ConvertColour (pmClr));
    SetBkMode (myInfo -> offscreenDeviceContext, OPAQUE);

    if (myInfo -> xor)
    {
	SetROP2 (myInfo -> deviceContext, R2_COPYPEN);
	SetROP2 (myInfo -> offscreenDeviceContext, R2_COPYPEN);
    }
} // MDIODraw_Line

/************************************************************************/
/* MDIODraw_Oval							*/
/************************************************************************/
void	MDIODraw_Oval (OOTint pmX, OOTint pmY, OOTint pmXRadius, 
		       OOTint pmYRadius, OOTint pmStyle, OOTint pmWidth,
		       OOTint pmClr, BOOL pmFill)
{
    MIOWinInfoPtr	myInfo = MIOWin_GetInfo (MIO_selectedRunWindow);
    RECT		myRect;
    
    MySetRunWindowPen (pmStyle, pmWidth, pmClr);

    MySetRect (&myRect, pmX - pmXRadius, pmY - pmYRadius, pmX + pmXRadius, 
    	       pmY + pmYRadius);

    if (myInfo -> xor)
    {
	SetROP2 (myInfo -> deviceContext, R2_XORPEN);
	SetROP2 (myInfo -> offscreenDeviceContext, R2_XORPEN);
    }

    if (pmFill) 
    {
	MySetRunWindowBrush (pmClr);

	if (myInfo -> displayOnScreen)
	{
    	    Ellipse (myInfo -> deviceContext, myRect.left, myRect.top, 
    		     myRect.right + 1, myRect.bottom + 1);
	}
    	Ellipse (myInfo -> offscreenDeviceContext, myRect.left, myRect.top, 
    	         myRect.right + 1, myRect.bottom + 1);
    }
    else 
    {
	if (myInfo -> displayOnScreen)
	{
    	    Arc (myInfo -> deviceContext, myRect.left, myRect.top, 
		 myRect.right + 1, myRect.bottom + 1, pmX + pmXRadius, pmY, 
		 pmX + pmXRadius, pmY);
	}
	Arc (myInfo -> offscreenDeviceContext, myRect.left, myRect.top, 
    	     myRect.right + 1, myRect.bottom + 1, pmX + pmXRadius, pmY, 
    	     pmX + pmXRadius, pmY);
    }

    if (myInfo -> xor)
    {
	SetROP2 (myInfo -> deviceContext, R2_COPYPEN);
	SetROP2 (myInfo -> offscreenDeviceContext, R2_COPYPEN);
    }
} // MDIODraw_Oval


/************************************************************************/
/* MDIODraw_Polygon							*/
/************************************************************************/
void	MDIODraw_Polygon (OOTint *pmXArray, OOTint *pmYArray, 
			  OOTint pmNumPoints, OOTint pmStyle, OOTint pmWidth,
			  OOTint pmClr, BOOL pmFill, BOOL pmFlipY)
{
    MIOWinInfoPtr	myInfo = MIOWin_GetInfo (MIO_selectedRunWindow);
    POINT		myPoints [MAX_POLY_POINTS];
    int			myHeight = myInfo -> height;
    int			cnt;
        
    if (pmNumPoints > MAX_POLY_POINTS)
    {
	ABORT_WITH_ERRNO (E_DRAW_TOO_COMPLICATED);
	return;
    }
    
    MySetRunWindowPen (pmStyle, pmWidth, pmClr);

    if (myInfo -> xor)
    {
	SetROP2 (myInfo -> deviceContext, R2_XORPEN);
	SetROP2 (myInfo -> offscreenDeviceContext, R2_XORPEN);
    }

    if (pmFlipY)
    {
    	for (cnt = 0; cnt < pmNumPoints; cnt++) 
    	{
	    myPoints [cnt].x = pmXArray [cnt];
	    myPoints [cnt].y = myHeight - pmYArray [cnt] - 1;
    	}
    }
    else
    {
    	for (cnt = 0; cnt < pmNumPoints; cnt++) 
    	{
	    myPoints [cnt].x = pmXArray [cnt];
	    myPoints [cnt].y = pmYArray [cnt];
    	}
    }
    myPoints [pmNumPoints].x = myPoints [0].x;
    myPoints [pmNumPoints].y = myPoints [0].y;

    if (pmFill) 
    {
	MySetRunWindowBrush (pmClr);

	if (myInfo -> displayOnScreen)
	{
    	    Polygon (myInfo -> deviceContext, myPoints, pmNumPoints);
	}
    	Polygon (myInfo -> offscreenDeviceContext, myPoints, pmNumPoints);
    }
    else 
    {
	if (myInfo -> displayOnScreen)
	{
    	    Polyline (myInfo -> deviceContext, myPoints, pmNumPoints + 1);
	}
    	Polyline (myInfo -> offscreenDeviceContext, myPoints, pmNumPoints + 1);
    }

    if (myInfo -> xor)
    {
	SetROP2 (myInfo -> deviceContext, R2_COPYPEN);
	SetROP2 (myInfo -> offscreenDeviceContext, R2_COPYPEN);
    }
} // MDIODraw_Polygon


/************************************************************************/
/* MDIODraw_Text							*/
/************************************************************************/
void	MDIODraw_Text (OOTstring pmString, OOTint pmX, OOTint pmY,
		       FONT pmFont, OOTint pmClr)
{
    MIOWinInfoPtr	myInfo = MIOWin_GetInfo (MIO_selectedRunWindow);
    int			myLen = strlen (pmString);
    unsigned short	myUnicodeString [1000];
    
    // Set the font, foreground and background colour for the offscreen
    SelectFont (myInfo -> offscreenDeviceContext, (HFONT) pmFont);
    SetTextColor (myInfo -> offscreenDeviceContext, 
    		  (COLORREF) MIORGB_ConvertColour (pmClr));

    if (myInfo -> xor)
    {
	SetROP2 (myInfo -> deviceContext, R2_XORPEN);
	SetROP2 (myInfo -> offscreenDeviceContext, R2_XORPEN);
    }

    // Draw it offscreen    		
    SetBkMode(myInfo -> offscreenDeviceContext, TRANSPARENT);
    if (myInfo -> msdosCharSet)
    {
	MultiByteToWideChar (CP_OEMCP, 0, pmString, myLen, myUnicodeString, 
			     1000);
	ExtTextOutW (myInfo -> offscreenDeviceContext, pmX, pmY, 0, NULL, 
		     myUnicodeString, myLen, NULL);
    }
    else
    {
	ExtTextOut (myInfo -> offscreenDeviceContext, pmX, pmY, 0, NULL, pmString, 
    		    myLen, NULL);
    }
    SetBkMode(myInfo -> offscreenDeviceContext, OPAQUE);
	        
    if (myInfo -> displayOnScreen)
    {
	// Set the font, foreground and background colour for the window
	SelectFont (myInfo -> deviceContext, (HFONT) pmFont);
	SetTextColor (myInfo -> deviceContext, 
	    (COLORREF) MIORGB_ConvertColour (pmClr));
	
	// Draw it to the window    		
	HideCaret (myInfo -> innerWindow);
	SetBkMode(myInfo -> deviceContext, TRANSPARENT);
	if (myInfo -> msdosCharSet)
	{
	    ExtTextOutW (myInfo -> deviceContext, pmX, pmY, 0, NULL, 
			myUnicodeString, myLen, NULL);
	}
	else
	{
	    ExtTextOut (myInfo -> deviceContext, pmX, pmY, 0, NULL, pmString, 
			 myLen, NULL);
	}
	SetBkMode(myInfo -> deviceContext, OPAQUE);
	ShowCaret (myInfo -> innerWindow);
    }

    if (myInfo -> xor)
    {
	SetROP2 (myInfo -> deviceContext, R2_COPYPEN);
	SetROP2 (myInfo -> offscreenDeviceContext, R2_COPYPEN);
    }
} // MDIODraw_Text

		       
/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/
/************************************************************************/
/* MyGetArcPoints							*/
/*									*/
/* Calculate the x and y coordinates of the point on an ellipse,	*/
/* where the line extending from the origin at 'pmAngle' degrees	*/
/* counterclockwise from the +ve x-axis, intercepts the ellipse.	*/
/************************************************************************/
static void	MyGetArcPoints (OOTint pmX, OOTint pmY, OOTint pmAngle, 
				OOTint pmXRadius, OOTint pmYRadius,
				short *pmXPos, short *pmYPos)
{
    double	myXPoint, myYPoint;

    myXPoint = ((double) pmXRadius) * cos (RAD(pmAngle));
    myYPoint = ((double) pmYRadius) * sin (RAD(pmAngle));

    *pmXPos = ROUND(pmX + myXPoint); // offset by the center and round to short
    *pmYPos = ROUND(pmY - myYPoint);
}


/************************************************************************/
/* MySetRect								*/
/************************************************************************/
static void	MySetRect (RECT *pmRect, int pmX1, int pmY1, int pmX2, int pmY2)
{
    pmRect -> left = min (pmX1, pmX2);
    pmRect -> right = max (pmX1, pmX2);
    pmRect -> top = min (pmY1, pmY2);
    pmRect -> bottom = max (pmY1, pmY2);
} // MySetRect


/************************************************************************/
/* MySetRunWindowBrush							*/
/*									*/
/* For efficiency sake, we cache the current brush.  We only create a 	*/
/* new brush when needed.						*/
/************************************************************************/
static void	MySetRunWindowBrush (OOTint pmClr)
{
    MIOWinInfoPtr	myInfo = MIO_selectedRunWindowInfo;
    HBRUSH 		myNewBrush, myOldBrush;
           
    if (myInfo -> cachedBrushColour == pmClr)
    {
    	return;
    }
    
    // We must update the brush

    // Create the new pen and select it into the device contexts
    myNewBrush = CreateSolidBrush ((COLORREF) MIORGB_ConvertColour (pmClr));
    myOldBrush = SelectBrush (myInfo -> deviceContext, myNewBrush);
    SelectBrush (myInfo -> offscreenDeviceContext, myNewBrush);

    // Get rid of old pen, if necessary
    if (myInfo -> cachedBrushColour != NO_COLOUR)
    {
	DeleteBrush (myOldBrush);
    }
    
    myInfo -> cachedBrushColour = pmClr;
} // MySetRunWindowBrush


/************************************************************************/
/* MySetRunWindowPen							*/
/*									*/
/* For efficiency sake, we cache the current pen.  We only create a new	*/
/* pen when needed.							*/
/************************************************************************/
static void	MySetRunWindowPen (int pmStyle, int pmWidth, OOTint pmClr)
{
    MIOWinInfoPtr	myInfo = MIOWin_GetInfo (MIO_selectedRunWindow);
    int			myPenStyle;
    HPEN 		myNewPen, myOldPen;
    static int		myStWindowsLineStyles [] = {PS_SOLID, PS_DASH, PS_DOT,
    						    PS_DASHDOT, PS_DASHDOTDOT};
           
    if ((myInfo -> cachedPenColour == pmClr) &&
        (myInfo -> cachedPenWidth == pmWidth) &&
        (myInfo -> cachedPenStyle == pmStyle))
    {
    	return;
    }
    
    // We must update the pen

    // Convert the style number into a pen style
    myPenStyle = myStWindowsLineStyles [pmStyle];

    // Create the new pen and select it into the device contexts
    myNewPen = CreatePen (myPenStyle, pmWidth, 
    			  (COLORREF) MIORGB_ConvertColour (pmClr));
    myOldPen = SelectPen (myInfo -> deviceContext, myNewPen);
    SelectPen (myInfo -> offscreenDeviceContext, myNewPen);

    // Get rid of old pen, if necessary
    if (myInfo -> cachedPenColour != NO_COLOUR)
    {
	DeletePen (myOldPen);
    }
    
    myInfo -> cachedPenColour = pmClr;
    myInfo -> cachedPenWidth = pmWidth;
    myInfo -> cachedPenStyle = pmStyle;
} // MySetRunWindowPen
