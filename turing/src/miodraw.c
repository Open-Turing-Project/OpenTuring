/*************/
/* miodraw.c */
/*************/

/*******************/
/* System includes */
/*******************/
#include <math.h>

/****************/
/* Self include */
/****************/
#include "miodraw.h"

/******************/
/* Other includes */
/******************/
#include "mio.h"

#include "miowin.h"

#include "mdiodraw.h"

#include "miofont.h"

#include "miosprite.h"

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
#define NUM_POINTS_IN_MAPLELEAF	 	 26
#define NUM_POINTS_IN_STAR		 11

#define FLIP_Y_COORD			 1
#define Y_ALREADY_FLIPPED		 0

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
/* MIODraw_Arc								*/
/************************************************************************/
void	MIODraw_Arc (OOTint pmX, OOTint pmY, OOTint pmXRadius, OOTint pmYRadius, 
		     OOTint pmInitAngle, OOTint pmFinalAngle, OOTint pmStyle, 
		     OOTint pmWidth, OOTint pmClr, BOOL pmFill)
{
    int	myHeight = MIO_selectedRunWindowInfo -> height;

    if (pmInitAngle == pmFinalAngle) 
    {
    	return;
    }
    
    MIO_CheckOuputIsToWindow ("Draw.[Fill]Arc");
    MIO_CheckOuputWindowIsInGraphicsMode ("Draw.[Fill]Arc");
    MIO_MakePopupWindowVisible ();
    MIO_CheckColourRange (pmClr);        
    MIO_CheckLineStyleRange (pmStyle);

    pmY = myHeight - pmY - 1;
    MDIODraw_Arc (pmX, pmY, pmXRadius, pmYRadius, pmInitAngle, 
    		  pmFinalAngle, pmStyle, pmWidth, pmClr, pmFill);

    if (MIO_selectedRunWindowInfo -> spriteQueueHead != NULL)
    {
	MIOSprite_Dirty (MIO_selectedRunWindowInfo, pmX - pmXRadius, 
	    pmY - pmYRadius, pmX + pmXRadius, pmY + pmYRadius);
    }
} // MIODraw_Arc
		     

/************************************************************************/
/* MIODraw_Box								*/
/************************************************************************/
void	MIODraw_Box (OOTint pmX1, OOTint pmY1, OOTint pmX2, OOTint pmY2, 
		     OOTint pmStyle, OOTint pmWidth, OOTint pmClr, BOOL pmFill)
{
    int	myHeight = MIO_selectedRunWindowInfo -> height;
    
    MIO_CheckOuputIsToWindow ("Draw.[Fill]Box");
    MIO_CheckOuputWindowIsInGraphicsMode ("Draw.[Fill]Box");
    MIO_MakePopupWindowVisible ();
    MIO_CheckColourRange (pmClr);        
    MIO_CheckLineStyleRange (pmStyle);

    pmY1 = myHeight - pmY1 - 1;
    pmY2 = myHeight - pmY2 - 1;
    MDIODraw_Box (pmX1, pmY1, pmX2, pmY2, pmStyle, pmWidth, pmClr, pmFill);

    if (MIO_selectedRunWindowInfo -> spriteQueueHead != NULL)
    {
	MIOSprite_Dirty (MIO_selectedRunWindowInfo, pmX1, pmY1, pmX2, pmY2);
    }
} // MIODraw_Box
		     

/************************************************************************/
/* MIODraw_Cls								*/
/************************************************************************/
void	MIODraw_Cls (void)
{
    MIOWinInfoPtr	myInfo = MIO_selectedRunWindowInfo;
    int			myHeight = myInfo -> height - 1;
    int			myWidth = myInfo -> width - 1;
    BOOL		myOldXOR;

    MIO_CheckOuputIsToWindow ("Draw.Cls");
    MIO_CheckOuputWindowIsInGraphicsMode ("Draw.Cls");
    MIO_MakePopupWindowVisible ();
    
    myOldXOR = myInfo -> xor;
    myInfo -> xor = FALSE;
    MDIODraw_Box (0, 0, myWidth, myHeight, NO_STYLE, NORMAL_WIDTH, 0, FILL);
    myInfo -> xor = myOldXOR;

    // Set the cursor positon to upper-left corner
    myInfo -> actualCol = 0;
    myInfo -> actualRow = 0;
    myInfo -> row = 0;
    myInfo -> col = 0;
    myInfo -> startInputRow = 0;
    myInfo -> startInputCol = 0;
    
    if (MIO_selectedRunWindowInfo -> spriteQueueHead != NULL)
    {
	MIOSprite_Dirty (MIO_selectedRunWindowInfo, 0, 0, myWidth, myHeight);
    }

    // Display the caret, if appropriate
    MIOWin_CaretDisplay (MIO_selectedRunWindow);
} // MIODraw_Cls


/************************************************************************/
/* MIODraw_Dot								*/
/************************************************************************/
void	MIODraw_Dot (OOTint pmX, OOTint pmY, OOTint pmClr)
{
    int	myHeight = MIO_selectedRunWindowInfo -> height;
    
    MIO_CheckOuputIsToWindow ("Draw.Dot");
    MIO_CheckOuputWindowIsInGraphicsMode ("Draw.Dot");
    MIO_MakePopupWindowVisible ();
    MIO_CheckColourRange (pmClr);        

    pmY = myHeight - pmY - 1;
    MDIODraw_Dot (pmX, pmY, pmClr);

    if (MIO_selectedRunWindowInfo -> spriteQueueHead != NULL)
    {
	MIOSprite_Dirty (MIO_selectedRunWindowInfo, pmX, pmY, pmX + 1, pmY + 1);
    }
} // MIODraw_Dot
		     

/************************************************************************/
/* MIODraw_Fill								*/
/************************************************************************/
void	MIODraw_Fill (OOTint pmX, OOTint pmY, OOTint pmFillClr, 
		      OOTint pmBorderClr)
{
    int	myHeight = MIO_selectedRunWindowInfo-> height;
    
    MIO_CheckOuputIsToWindow ("Draw.Fill");
    MIO_CheckOuputWindowIsInGraphicsMode ("Draw.Fill");
    MIO_MakePopupWindowVisible ();
    MIO_CheckColourRange (pmFillClr);        
    MIO_CheckColourRange (pmBorderClr);        

    MDIODraw_Fill (pmX, myHeight - pmY - 1, pmFillClr, pmBorderClr);

    if (MIO_selectedRunWindowInfo -> spriteQueueHead != NULL)
    {
	MIOSprite_Dirty (MIO_selectedRunWindowInfo, 0, 0, 
	    MIO_selectedRunWindowInfo -> width - 1,
	    MIO_selectedRunWindowInfo -> height - 1);
    }
} // MIODraw_Fill

		      
/************************************************************************/
/* MIODraw_Line								*/
/************************************************************************/
void	MIODraw_Line (OOTint pmX1, OOTint pmY1, OOTint pmX2, OOTint pmY2, 
		      OOTint pmStyle, OOTint pmWidth, OOTint pmClr)
{
    int	myHeight = MIO_selectedRunWindowInfo -> height;
    
    MIO_CheckOuputIsToWindow ("Draw.[Thick/Dashed]Line");
    MIO_CheckOuputWindowIsInGraphicsMode ("Draw.[Thick/Dashed]Line");
    MIO_MakePopupWindowVisible ();
    MIO_CheckColourRange (pmClr);
    MIO_CheckLineStyleRange (pmStyle);

    pmY1 = myHeight - pmY1 - 1;
    pmY2 = myHeight - pmY2 - 1;
    MDIODraw_Line (pmX1, pmY1, pmX2, pmY2, pmStyle, pmWidth, pmClr);

    if (MIO_selectedRunWindowInfo -> spriteQueueHead != NULL)
    {
	MIOSprite_Dirty (MIO_selectedRunWindowInfo, pmX1, pmY1, pmX2, pmY2);
    }
} // MIODraw_Line
		     

/************************************************************************/
/* MIODraw_MapleLeaf							*/
/************************************************************************/
void	MIODraw_MapleLeaf (OOTint pmX1, OOTint pmY1, OOTint pmX2, OOTint pmY2, 
		           OOTint pmStyle, OOTint pmWidth, OOTint pmClr, 
		           BOOL pmFill)
{
    OOTint	myXPoint [NUM_POINTS_IN_MAPLELEAF];
    OOTint	myYPoint [NUM_POINTS_IN_MAPLELEAF];
    double 	myRX, myRY, myXC, myYC;
    int		myHeight = MIO_selectedRunWindowInfo -> height;

    MIO_CheckOuputIsToWindow ("Draw.[Fill]MapleLeaf");
    MIO_CheckOuputWindowIsInGraphicsMode ("Draw.[Fill]MapleLeaf");
    MIO_MakePopupWindowVisible ();
    MIO_CheckColourRange (pmClr);        
    MIO_CheckLineStyleRange (pmStyle);

    // Fill the POINTS array with vertices of the maple leaf

    myRX  = (double) abs (pmX2 - pmX1);
    myRY  = (double) (pmY2 - pmY1);
    myXC  = (double) (pmX2 + pmX1) / 2.0;
    myYC  = (double) (myHeight - pmY1 - 1);

    myXPoint [0] = (OOTint) (myXC + myRX * 0.021423);	// .02 * r */
    myYPoint [0] = (OOTint) (myYC - myRY * 0.215686);	// .2 * r */
    myXPoint [1] = (OOTint) (myXC + myRX * 0.270780);	// .33 * r * .766044 */
    myYPoint [1] = (OOTint) (myYC - myRY * 0.203804);	// .33 * r * .642788 */
    myXPoint [2] = (OOTint) (myXC + myRX * 0.271820);	// .28 * r * .906308 */
    myYPoint [2] = (OOTint) (myYC - myRY * 0.295752);	// .28 * r * .422618 */
    myXPoint [3] = (OOTint) (myXC + myRX * 0.482015);	// .45 * r */
    myYPoint [3] = (OOTint) (myYC - myRY * 0.411765);
    myXPoint [4] = (OOTint) (myXC + myRX * 0.443046);	// .42 * r * .984808 */
    myYPoint [4] = (OOTint) (myYC - myRY * 0.483267);	// .42 * r * .173648 */
    myXPoint [5] = (OOTint) (myXC + myRX * 0.500000);	// .5 * r * .93358 */
    myYPoint [5] = (OOTint) (myYC - myRY * 0.587435);	// .5 * r * .358368 */
    myXPoint [6] = (OOTint) (myXC + myRX * 0.363353);	// .4 * r * .848048 */
    myYPoint [6] = (OOTint) (myYC - myRY * 0.619576);	// .4 * r * .529919 */
    myXPoint [7] = (OOTint) (myXC + myRX * 0.342287);	// .43 * r * .743145 */
    myYPoint [7] = (OOTint) (myYC - myRY * 0.693849);	// .43 * r * .669131 */
    myXPoint [8] = (OOTint) (myXC + myRX * 0.153596);	// .25 * r * .573576 */
    myYPoint [8] = (OOTint) (myYC - myRY * 0.612537);	// .25 * r * .819152 */
    myXPoint [9] = (OOTint) (myXC + myRX * 0.201601);	// .55 * r * .342202 */
    myYPoint [9] = (OOTint) (myYC - myRY * 0.918462);	// .55 * r * .939693 */
    myXPoint [10] = (OOTint) (myXC + myRX * 0.093001);	// .5 * r * .173648 */
    myYPoint [10] = (OOTint) (myYC - myRY * 0.894514);	// .5 * r * .984808 */
    myXPoint [11] = (OOTint) myXC;
    myYPoint [11] = (OOTint) (myYC - myRY);			// .6 * r */
    myXPoint [12] = (OOTint) (myXC - myRX * 0.093001);	// .5 * r * .173648 */
    myYPoint [12] = myYPoint [10];
    myXPoint [13] = (OOTint) (myXC - myRX * 0.201601);	// .55 * r * .342202 */
    myYPoint [13] = myYPoint [9];
    myXPoint [14] = (OOTint) (myXC - myRX * 0.153596);	// .25 * r * .573576 */
    myYPoint [14] = myYPoint [8];
    myXPoint [15] = (OOTint) (myXC - myRX * 0.342287);	// .43 * r * .743145 */
    myYPoint [15] = myYPoint [7];
    myXPoint [16] = (OOTint) (myXC - myRX * 0.363353);	// .4 * r * .848048 */
    myYPoint [16] = myYPoint [6];
    myXPoint [17] = (OOTint) (myXC - myRX * 0.500000);	// .5 * r * .93358 */
    myYPoint [17] = myYPoint [5];
    myXPoint [18] = (OOTint) (myXC - myRX * 0.443046);	// .42 * r * .984808 */
    myYPoint [18] = myYPoint [4];
    myXPoint [19] = (OOTint) (myXC - myRX * 0.482015);	// .45 * r */
    myYPoint [19] = myYPoint [3];
    myXPoint [20] = (OOTint) (myXC - myRX * 0.271820);	// .28 * r * .906308 */
    myYPoint [20] = myYPoint [2];
    myXPoint [21] = (OOTint) (myXC - myRX * .2707796);	// .33 * r * .766044 */
    myYPoint [21] = myYPoint [1];
    myXPoint [22] = (OOTint) (myXC - myRX * 0.021423);	// .02 * r */
    myYPoint [22] = myYPoint [0];
    myXPoint [23] = myXPoint [22];
    myYPoint [23] = (OOTint) myYC;			// .42 * r */
    myXPoint [24] = myXPoint [0];
    myYPoint [24] = myYPoint [23];
    myXPoint [25] = myXPoint [0];
    myYPoint [25] = myYPoint [0];

    MDIODraw_Polygon (myXPoint, myYPoint, NUM_POINTS_IN_MAPLELEAF, pmStyle, 
    		      pmWidth, pmClr, pmFill, Y_ALREADY_FLIPPED);

    if (MIO_selectedRunWindowInfo -> spriteQueueHead != NULL)
    {
	MIOSprite_Dirty (MIO_selectedRunWindowInfo, pmX1, myHeight - pmY1 - 1, 
			 pmX2, myHeight - pmY1 - 1);
    }
} // MIODraw_MapleLeaf


/************************************************************************/
/* MIODraw_Oval								*/
/************************************************************************/
void	MIODraw_Oval (OOTint pmX, OOTint pmY, OOTint pmXRadius, 
		      OOTint pmYRadius, OOTint pmStyle, OOTint pmWidth, 
		      OOTint pmClr, BOOL pmFill)
{
    int	myHeight = MIO_selectedRunWindowInfo -> height;
    
    MIO_CheckOuputIsToWindow ("Draw.[Fill]Oval");
    MIO_CheckOuputWindowIsInGraphicsMode ("Draw.[Fill]Oval");
    MIO_MakePopupWindowVisible ();
    MIO_CheckColourRange (pmClr);        
    MIO_CheckLineStyleRange (pmStyle);

    pmY = myHeight - pmY - 1;
    MDIODraw_Oval (pmX, pmY, pmXRadius, pmYRadius, pmStyle, 
    		   pmWidth, pmClr, pmFill);

    if (MIO_selectedRunWindowInfo -> spriteQueueHead != NULL)
    {
	MIOSprite_Dirty (MIO_selectedRunWindowInfo, pmX - pmXRadius, 
	    pmY - pmYRadius, pmX + pmXRadius, pmY + pmYRadius);
    }
} // MIODraw_Oval
		     

/************************************************************************/
/* MIODraw_Polygon							*/
/************************************************************************/
void	MIODraw_Polygon (OOTint *pmXArray, OOTint *pmYArray, 
			 OOTint pmNumPoints, OOTint pmStyle, OOTint pmWidth, 
			 OOTint pmClr, BOOL pmFill)
{
    MIO_CheckOuputIsToWindow ("Draw.[Fill]Polygon");
    MIO_CheckOuputWindowIsInGraphicsMode ("Draw.[Fill]Polygon");
    MIO_MakePopupWindowVisible ();
    MIO_CheckColourRange (pmClr);        
    MIO_CheckLineStyleRange (pmStyle);

    MDIODraw_Polygon (pmXArray, pmYArray, pmNumPoints, pmStyle, pmWidth, pmClr, 
    		      pmFill, FLIP_Y_COORD);

    if (MIO_selectedRunWindowInfo -> spriteQueueHead != NULL)
    {
	MIOSprite_Dirty (MIO_selectedRunWindowInfo, 0, 0, 
	    MIO_selectedRunWindowInfo -> width - 1,
	    MIO_selectedRunWindowInfo -> height - 1);
    }
} // MIODraw_Polygon
		     

/************************************************************************/
/* MIODraw_Star								*/
/************************************************************************/
void	MIODraw_Star (OOTint pmX1, OOTint pmY1, OOTint pmX2, OOTint pmY2, 
		      OOTint pmStyle, OOTint pmWidth, OOTint pmClr, BOOL pmFill)
{
    OOTint	myXPoint [NUM_POINTS_IN_STAR], myYPoint [NUM_POINTS_IN_STAR];
    double 	myRX, myRY, myXC, myYC;
    int		myHeight = MIO_selectedRunWindowInfo -> height;
    
    MIO_CheckOuputIsToWindow ("Draw.[Fill]MapleLeaf");
    MIO_CheckOuputWindowIsInGraphicsMode ("Draw.[Fill]MapleLeaf");
    MIO_MakePopupWindowVisible ();
    MIO_CheckColourRange (pmClr);        
    MIO_CheckLineStyleRange (pmStyle);

    // Fill the POINTS array with vertices of the star

    myRX  = (double) abs (pmX2 - pmX1);
    myRY  = (double) (pmY2 - pmY1);
    myXC  = (double) (pmX2 + pmX1) / 2.0;
    myYC  = (double) (myHeight - pmY1 - 1);

    myXPoint [0] = (OOTint) myXC;
    myYPoint [0] = (OOTint) (myYC - myRY);
    myXPoint [1] = (OOTint) (myXC + myRX * 0.118034);
    myYPoint [1] = (OOTint) (myYC - myRY * 0.618560);
    myXPoint [2] = (OOTint) (myXC + myRX * 0.500000);
    myYPoint [2] = myYPoint [1];
    myXPoint [3] = (OOTint) (myXC + myRX * 0.190983);
    myYPoint [3] = (OOTint) (myYC - myRY * 0.381759);
    myXPoint [4] = (OOTint) (myXC + myRX * 0.309017);
    myYPoint [4] = (OOTint) myYC;
    myXPoint [5] = (OOTint) myXC;
    myYPoint [5] = (OOTint) (myYC - myRY * 0.236068);
    myXPoint [6] = (OOTint) (myXC - myRX * 0.309017);
    myYPoint [6] = myYPoint [4];
    myXPoint [7] = (OOTint) (myXC - myRX * 0.190983);
    myYPoint [7] = myYPoint [3];
    myXPoint [8] = (OOTint) (myXC - myRX * 0.500000);
    myYPoint [8] = myYPoint [2];
    myXPoint [9] = (OOTint) (myXC - myRX * 0.118034);
    myYPoint [9] = myYPoint [1];
    myXPoint [10] = myXPoint [0];
    myYPoint [10] = myYPoint [0];

    MDIODraw_Polygon (myXPoint, myYPoint, NUM_POINTS_IN_STAR, pmStyle, pmWidth, 
    		      pmClr, pmFill, Y_ALREADY_FLIPPED);

    if (MIO_selectedRunWindowInfo -> spriteQueueHead != NULL)
    {
	MIOSprite_Dirty (MIO_selectedRunWindowInfo, pmX1, myHeight - pmY1 - 1, 
			 pmX2, myHeight - pmY1 - 1);
    }
} // MIODraw_Star


/************************************************************************/
/* MIODraw_Text								*/
/************************************************************************/
void	MIODraw_Text (OOTstring pmString, OOTint pmX, OOTint pmY,
		      OOTint pmFontID, OOTint pmClr)
{
    int		myHeight = MIO_selectedRunWindowInfo -> height;
    FONT	myFont;
    int		myFontHeight;
    int		myY;
        
    MIO_CheckOuputIsToWindow ("Draw.Text/Font.Draw");
    MIO_CheckOuputWindowIsInGraphicsMode ("Draw.Text/Font.Draw");
    MIO_MakePopupWindowVisible ();
    MIO_CheckColourRange (pmClr);
    
    if (pmFontID == DEFAULT_FONT_ID)
    {
    	myFont = MIO_selectedRunWindowInfo -> font;
    	myFontHeight = MIO_selectedRunWindowInfo -> fontCharAscent;
    }
    else
    {
    	myFont = MIOFont_GetFont (pmFontID);
    	myFontHeight = MIOFont_GetFontAscent (pmFontID);
    }
    
    myY = myHeight - pmY - myFontHeight;
    
    MDIODraw_Text (pmString, pmX, myY, myFont, pmClr);

    if (MIO_selectedRunWindowInfo -> spriteQueueHead != NULL)
    {
	MIOSprite_Dirty (MIO_selectedRunWindowInfo, 0, 0, 
	    MIO_selectedRunWindowInfo -> width - 1,
	    MIO_selectedRunWindowInfo -> height - 1);
    }
} // MIODraw_Text


/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/

