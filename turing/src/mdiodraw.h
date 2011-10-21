/**************/
/* mdiodraw.h */
/**************/

#ifndef _MDIODRAW_H_
#define _MDIODRAW_H_

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
#define SOLID_PEN	0

/*********/
/* Types */
/*********/

/**********************/
/* External variables */
/**********************/

/***********************/
/* External procedures */
/***********************/
extern void	MDIODraw_Arc (OOTint pmX, OOTint pmY, OOTint pmXRadius, 
			      OOTint pmYRadius, OOTint pmInitAngle,
			      OOTint pmFinalAngle, OOTint pmStyle, 
			      OOTint pmWidth, OOTint pmClr, BOOL pmFill);
extern void	MDIODraw_Box (OOTint pmX1, OOTint pmY1, OOTint pmX2, 
			      OOTint pmY2, OOTint pmStyle, OOTint pmWidth, 
			      OOTint pmClr, BOOL pmFill);
extern void	MDIODraw_Dot (OOTint pmX1, OOTint pmY1, OOTint pmClr);
extern void	MDIODraw_Fill (OOTint pmX, OOTint pmY, OOTint pmFillClr, 
			       OOTint pmBorderClr);
extern void	MDIODraw_Line (OOTint pmX1, OOTint pmY1, OOTint pmX2, 
			       OOTint pmY2, OOTint pmStyle, 
			       OOTint pmWidth, OOTint pmClr);
extern void	MDIODraw_Oval (OOTint pmX, OOTint pmY, OOTint pmXRadius, 
			       OOTint pmYRadius, OOTint pmStyle, OOTint pmWidth, 
			       OOTint pmClr, BOOL pmFill);
extern void	MDIODraw_Polygon (OOTint *pmXArray, OOTint *pmYArray, 
				  OOTint pmNumPoints, OOTint pmStyle, 
				  OOTint pmWidth, OOTint pmClr, BOOL pmFill,
				  BOOL pmFlipY);
extern void	MDIODraw_Text (OOTstring pmString, OOTint pmX, OOTint pmY,
			       FONT pmFont, OOTint pmClr);

#endif // #ifndef _MDIODRAW_H_
