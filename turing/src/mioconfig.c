/***************/
/* mioconfig.c */
/***************/

/*******************/
/* System includes */
/*******************/

/****************/
/* Self include */
/****************/
#include "mioconfig.h"

/******************/
/* Other includes */
/******************/
#include "mdioconfig.h"

#include "mio.h"

#include "mioerr.h"

#include "edint.h"

#include "miorgb.h"

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
/* MIOConfig_Display							*/
/************************************************************************/
OOTint	MIOConfig_Display (OOTint pmDisplayCode)
{
    MYRECT  myRect;

    switch (pmDisplayCode)
    {
	case CD_SCREEN_HEIGHT:
	    myRect = EdInt_GetWorkArea ();
	    return myRect.bottom - myRect.top;

	case CD_SCREEN_WIDTH:
	    myRect = EdInt_GetWorkArea ();
	    return myRect.right - myRect.left;

	case CD_MAX_NUM_COLOURS:
	    if (MIORGB_GetScreenDepth () >= 32)
		return ((unsigned int) (1 << 31) - 1);
	    else
	        return (1 << MIORGB_GetScreenDepth ());
    } // switch

    // Error
    ABORT_WITH_ERRNO (E_CONFIG_UNKNOWN_DISPLAY_CODE);

    return -1;	// Never reach here
} // MIOConfig_Display


/************************************************************************/
/* MIOConfig_Lang							*/
/************************************************************************/
OOTint	MIOConfig_Lang (OOTint pmLangCode)
{
    switch (pmLangCode)
    {
	case CL_RELEASE:
	    return 405; // TW - This should be placed somewhere. release_code;

	case CL_LANGUAGE_VERSION:
	    return 400; // TW - This should be placed somewhere. lang_code;

	case CL_MAX_NUM_DIR_STREAMS:
	    return 100;  // TW - This should be a call to MIODir

	case CL_MAX_NUM_STREAMS:
	    return 20;

	case CL_MAX_RUN_TIME_ARGS:
	    return 20;
    } // switch

    // Error
    ABORT_WITH_ERRNO (E_CONFIG_UNKNOWN_LANG_CODE);

    return -1;	// Never reach here
} // MIOConfig_Lang


/************************************************************************/
/* MIOConfig_Machine							*/
/************************************************************************/
OOTint	MIOConfig_Machine (OOTint pmMachineCode)
{
    switch (pmMachineCode)
    {
	case CM_PROCESSOR:
	    return MDIOConfig_GetProcessor ();
	    
	case CM_FPU:
	    return TRUE;  // TW Replace with better

        case CM_OS:
	    return MDIOConfig_GetOS ();
    }

    // Error
    ABORT_WITH_ERRNO (E_CONFIG_UNKNOWN_MACHINE_CODE);

    return -1;	// Never reach here
} // MIOConfig_Machine


/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/


