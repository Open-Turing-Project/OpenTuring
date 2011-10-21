/*************/
/* mdiojoy.h */
/*************/

#ifndef _MDIOJOY_H_
#define _MDIOJOY_H_

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

/*********/
/* Types */
/*********/

/**********************/
/* External variables */
/**********************/

/***********************/
/* External procedures */
/***********************/
extern BOOL	MDIOJoy_Caps (int pmJoystickNumber, DWORD *pmCaps, 
			      DWORD *pmMaxNumAxes, DWORD *pmNumAxes, 
			      DWORD *pmMaxNumButtons, DWORD *pmNumButtons, 
			      DWORD *pmMinX, DWORD *pmMaxX,
			      DWORD *pmMinY, DWORD *pmMaxY, 
			      DWORD *pmMinZ, DWORD *pmMaxZ, 
			      DWORD *pmMinR, DWORD *pmMaxR,
			      DWORD *pmMinU, DWORD *pmMaxU, 
			      DWORD *pmMinV, DWORD *pmMaxV);
extern BOOL	MDIOJoy_Read (int pmJoystickNumber, DWORD *pmXPos, 
			      DWORD *pmYPos, DWORD *pmZPos, DWORD *pmRPos, 
			      DWORD *pmUPos, DWORD *pmVPos, DWORD *pmButtons, 
			      DWORD *pmButtonNumber, DWORD *pmPOV);

#endif // #ifndef _MDIOJOY_H_
