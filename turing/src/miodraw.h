/*************/
/* miodraw.h */
/*************/

#ifndef _MIODRAW_H_
#define _MIODRAW_H_

/*******************/
/* System includes */
/*******************/

/******************/
/* Other includes */
/******************/
#include "miotypes.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
#define FILL		1
#define NOFILL  	0

#define NO_STYLE	0
#define NORMAL_WIDTH	1

/*********/
/* Types */
/*********/

/**********************/
/* External variables */
/**********************/

/***********************/
/* External procedures */
/***********************/
extern void	MIODraw_Arc (OOTint pmX, OOTint pmY, OOTint pmXRadius, 
			     OOTint pmYRadius, OOTint pmInitAngle,
			     OOTint pmFinalAngle, OOTint pmStyle, 
			     OOTint pmWidth, OOTint pmClr, BOOL pmFill);
extern void	MIODraw_Box (OOTint pmX1, OOTint pmY1, OOTint pmX2, OOTint pmY2, 
			     OOTint pmStyle, OOTint pmWidth, 
			     OOTint pmClr, BOOL pmFill);
extern void	MIODraw_Cls (void);
extern void	MIODraw_Dot (OOTint pmX1, OOTint pmY1, OOTint pmClr);
extern void	MIODraw_Fill (OOTint pmX, OOTint pmY, OOTint pmFillClr, 
			      OOTint pmBorderClr);
extern void	MIODraw_Line (OOTint pmX1, OOTint pmY1, OOTint pmX2, 
			      OOTint pmY2, OOTint pmStyle, OOTint pmWidth, 
			      OOTint pmClr);
extern void	MIODraw_MapleLeaf (OOTint pmX1, OOTint pmY1, OOTint pmX2, 
				   OOTint pmY2, OOTint pmStyle, OOTint pmWidth, 
				   OOTint pmClr, BOOL pmFill);
extern void	MIODraw_Oval (OOTint pmX, OOTint pmY, OOTint pmXRadius, 
			      OOTint pmYRadius, OOTint pmStyle, OOTint pmWidth, 
			      OOTint pmClr, BOOL pmFill);
extern void	MIODraw_Polygon (OOTint *pmXArray, OOTint *pmYArray, 
				 OOTint pmNumPoints, OOTint pmStyle, 
				 OOTint pmWidth, OOTint pmClr, BOOL pmFill);
extern void	MIODraw_Star (OOTint pmX1, OOTint pmY1, OOTint pmX2, 
			      OOTint pmY2, OOTint pmStyle, OOTint pmWidth, 
			      OOTint pmClr, BOOL pmFill);
extern void	MIODraw_Text (OOTstring pmString, OOTint pmX, OOTint pmY,
			      OOTint pmFontID, OOTint pmClr);

#endif // #ifndef _MIODRAW_H_
