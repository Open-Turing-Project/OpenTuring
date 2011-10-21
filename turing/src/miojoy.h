/************/
/* miojoy.h */
/************/

#ifndef _MIOJOY_H_
#define _MIOJOY_H_

/*******************/
/* System includes */
/*******************/

/******************/
/* Other includes */
/******************/
#include "turing.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/

/*********/
/* Types */
/*********/
typedef struct JOYPOSRECORD 
{
    OOTnat	xpos;
    OOTnat	ypos;
    OOTnat	zpos;
    OOTnat	rpos;
    OOTnat	upos;
    OOTnat	vpos;
    OOTnat	buttons;
    OOTnat	buttonNumber;
    OOTnat	POV;
} JOYPOSRECORD;

typedef struct JOYCAPSRECORD 
{
    OOTnat	caps;
    OOTnat	maxNumAxes;
    OOTnat	numAxes;
    OOTnat	maxNumButtons;
    OOTnat	numButtons;
    OOTnat	minX;
    OOTnat	maxX;
    OOTnat	minY;
    OOTnat	maxY;
    OOTnat	minZ;
    OOTnat	maxZ;
    OOTnat	minR;
    OOTnat	maxR;
    OOTnat	minU;
    OOTnat	maxU;
    OOTnat	minV;
    OOTnat	maxV;
} JOYCAPSRECORD;

/**********************/
/* External variables */
/**********************/

/***********************/
/* External procedures */
/***********************/
extern void	MIOJoy_Read (int pmJoystickNumber, 
			     JOYPOSRECORD *pmJoyPosRecord);
extern void	MIOJoy_Caps (int pmJoystickNumber, 
			     JOYCAPSRECORD *pmJoyCapsRecord);

#endif // #ifndef _MIOJOY_H_
