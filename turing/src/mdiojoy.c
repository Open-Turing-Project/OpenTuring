/*************/
/* mdiojoy.c */
/*************/

/*******************/
/* System includes */
/*******************/
#include <windows.h>

/****************/
/* Self include */
/****************/
#include "mdiojoy.h"

/******************/
/* Other includes */
/******************/
#include "mio.h"
#include "mioerr.h"

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
/* MDIOJoy_Caps								*/
/************************************************************************/
BOOL	MDIOJoy_Caps (int pmJoystickNumber, DWORD *pmCaps, DWORD *pmMaxNumAxes, 
		      DWORD *pmNumAxes, DWORD *pmMaxNumButtons, 
		      DWORD *pmNumButtons, DWORD *pmMinX, DWORD *pmMaxX,
		      DWORD *pmMinY, DWORD *pmMaxY, DWORD *pmMinZ, 
		      DWORD *pmMaxZ, DWORD *pmMinR, DWORD *pmMaxR,
		      DWORD *pmMinU, DWORD *pmMaxU, DWORD *pmMinV, 
		      DWORD *pmMaxV)
{
    int		myNumSupportedJoysticks, myJoystickID;
    JOYINFO	myJoyInfo;
    JOYCAPS	myJoyCaps;
    MMRESULT	myResult;

    myNumSupportedJoysticks = joyGetNumDevs ();
    if (myNumSupportedJoysticks == 0)
    {
	SET_ERRNO (E_JOY_NO_DRIVER);
	return FALSE;
    }

    if (pmJoystickNumber == 0)
    {
	myJoystickID = JOYSTICKID1;
    }
    else if (pmJoystickNumber == 1)
    {
	myJoystickID = JOYSTICKID2;
    }
    else
    {
	SET_ERRNO (E_JOY_BAD_ID);
	return FALSE;    
    }

    myResult = joyGetPos (myJoystickID, &myJoyInfo);

    switch (myResult)
    {
        case MMSYSERR_NODRIVER:
	    SET_ERRNO (E_JOY_NO_DRIVER);
	    return FALSE;

	case MMSYSERR_INVALPARAM:
	case JOYERR_PARMS:
	    SET_ERRNO (E_JOY_BAD_ID);
	    return FALSE;

	case JOYERR_UNPLUGGED:
	    SET_ERRNO (E_JOY_UNPLUGGED);
	    return FALSE;
    }

    myResult = joyGetDevCaps (myJoystickID, &myJoyCaps, sizeof(JOYCAPS));

    switch (myResult)
    {
        case MMSYSERR_NODRIVER:
	    SET_ERRNO (E_JOY_NO_DRIVER);
	    return FALSE;

	case MMSYSERR_INVALPARAM:
	case JOYERR_PARMS:
	    SET_ERRNO (E_JOY_BAD_ID);
	    return FALSE;
    }

    *pmCaps = myJoyCaps.wCaps;
    *pmMaxNumAxes = myJoyCaps.wMaxAxes;
    *pmNumAxes = myJoyCaps.wNumAxes;
    *pmMaxNumButtons = myJoyCaps.wMaxButtons;
    *pmNumButtons = myJoyCaps.wNumButtons;
    *pmMinX = myJoyCaps.wXmin;
    *pmMaxX = myJoyCaps.wXmax;
    *pmMinY = myJoyCaps.wYmin;
    *pmMaxY = myJoyCaps.wYmax;
    *pmMinZ = myJoyCaps.wZmin;
    *pmMaxZ = myJoyCaps.wZmax;
    *pmMinR = myJoyCaps.wRmin;
    *pmMaxR = myJoyCaps.wRmax;
    *pmMinU = myJoyCaps.wUmin;
    *pmMaxU = myJoyCaps.wUmax;
    *pmMinV = myJoyCaps.wVmin;
    *pmMaxV = myJoyCaps.wVmax;

    return TRUE;
} // MDIOJoy_Caps
    
    
/************************************************************************/
/* MDIOJoy_Read								*/
/************************************************************************/
BOOL	MDIOJoy_Read (int pmJoystickNumber, DWORD *pmXPos, DWORD *pmYPos, 
		      DWORD *pmZPos, DWORD *pmRPos, DWORD *pmUPos, 
		      DWORD *pmVPos, DWORD *pmButtons, DWORD *pmButtonNumber, 
		      DWORD *pmPOV)
{
    JOYINFOEX	myJoyInfo;
    MMRESULT	myResult;

    myJoyInfo.dwSize = sizeof(JOYINFOEX);
    myJoyInfo.dwFlags = JOY_RETURNALL;

    myResult = joyGetPosEx (pmJoystickNumber, &myJoyInfo);

    switch(myResult)
    {
	case MMSYSERR_NODRIVER:
	    SET_ERRNO (E_JOY_NO_DRIVER);
	    return FALSE;

	case MMSYSERR_INVALPARAM:
	case JOYERR_PARMS:
	    SET_ERRNO (E_JOY_BAD_ID);
	    return FALSE;

	case JOYERR_UNPLUGGED:
	    SET_ERRNO (E_JOY_UNPLUGGED);
	    return FALSE;
    }

    *pmXPos = myJoyInfo.dwXpos;
    *pmYPos = myJoyInfo.dwYpos;
    *pmZPos = myJoyInfo.dwZpos;
    *pmRPos = myJoyInfo.dwRpos;
    *pmUPos = myJoyInfo.dwUpos;
    *pmVPos = myJoyInfo.dwVpos;
    *pmButtons = myJoyInfo.dwButtons;
    *pmButtonNumber = myJoyInfo.dwButtonNumber;
    *pmPOV = myJoyInfo.dwPOV; 

    return TRUE;
} // MDIOJoy_Read



/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/


