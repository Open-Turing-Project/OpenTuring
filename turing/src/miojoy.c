/************/
/* miojoy.c */
/************/

/*******************/
/* System includes */
/*******************/
#include <memory.h>

/****************/
/* Self include */
/****************/
#include "miojoy.h"

/******************/
/* Other includes */
/******************/
#include "mdiojoy.h"

#include "miotypes.h"

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
/* MIOJoy_Caps								*/
/************************************************************************/
void	MIOJoy_Caps (int pmJoystickNumber, JOYCAPSRECORD *pmJoyCapsRecord)
{
    DWORD	myCaps, myMaxNumAxes, myNumAxes, myMaxNumButtons, myNumButtons;
    DWORD	myMinX, myMaxX, myMinY, myMaxY, myMinZ, myMaxZ;
    DWORD	myMinR, myMaxR, myMinU, myMaxU, myMinV, myMaxV;

    memset (pmJoyCapsRecord, 0, sizeof (JOYCAPSRECORD));

    if (!MDIOJoy_Caps (pmJoystickNumber, &myCaps, &myMaxNumAxes, &myNumAxes, 
		       &myMaxNumButtons, &myNumButtons, &myMinX, &myMaxX,
		       &myMinY, &myMaxY, &myMinZ, &myMaxZ, &myMinR, &myMaxR,
		       &myMinU, &myMaxU, &myMinV, &myMaxV))
    {
	return;
    }

    pmJoyCapsRecord -> caps = myCaps;
    pmJoyCapsRecord -> maxNumAxes = myMaxNumAxes;
    pmJoyCapsRecord -> numAxes = myNumAxes;
    pmJoyCapsRecord -> maxNumButtons = myMaxNumButtons;
    pmJoyCapsRecord -> numButtons = myNumButtons;
    pmJoyCapsRecord -> minX = myMinX;
    pmJoyCapsRecord -> maxX = myMaxX;
    pmJoyCapsRecord -> minY = myMinY;
    pmJoyCapsRecord -> maxY = myMaxY;
    pmJoyCapsRecord -> minZ = myMinZ;
    pmJoyCapsRecord -> maxZ = myMaxZ;
    pmJoyCapsRecord -> minR = myMinR;
    pmJoyCapsRecord -> maxR = myMaxR;
    pmJoyCapsRecord -> minU = myMinU;
    pmJoyCapsRecord -> maxU = myMaxU;
    pmJoyCapsRecord -> minV = myMinV;
    pmJoyCapsRecord -> maxV = myMaxV;
} // MIOJoy_Caps


/************************************************************************/
/* MIOJoy_Read								*/
/************************************************************************/
void	MIOJoy_Read (int pmJoystickNumber, JOYPOSRECORD *pmJoyPosRecord)
{
    DWORD	myXPos, myYPos, myZPos, myRPos, myUPos, myVPos;
    DWORD	myButtons, myButtonNumber, myPOV;

    memset (pmJoyPosRecord, 0, sizeof(JOYPOSRECORD));

    if (!MDIOJoy_Read (pmJoystickNumber, &myXPos, &myYPos, &myZPos, 
		       &myRPos, &myUPos, &myVPos, &myButtons, &myButtonNumber,
		       &myPOV))
    {
	return;
    }

    pmJoyPosRecord -> xpos = myXPos;
    pmJoyPosRecord -> ypos = myYPos;
    pmJoyPosRecord -> zpos = myZPos;
    pmJoyPosRecord -> rpos = myRPos;
    pmJoyPosRecord -> upos = myUPos;
    pmJoyPosRecord -> vpos = myVPos;
    pmJoyPosRecord -> buttons = myButtons;
    pmJoyPosRecord -> buttonNumber = myButtonNumber;
    pmJoyPosRecord -> POV = myPOV; 
} // MIOJoy_Read


/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/


