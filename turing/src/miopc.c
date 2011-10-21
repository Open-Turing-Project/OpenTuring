/***********/
/* miopc.c */
/***********/

/*******************/
/* System includes */
/*******************/

/****************/
/* Self include */
/****************/
#include "miopc.h"

/******************/
/* Other includes */
/******************/
#include "mdiopc.h"

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
/* MIOPC_InPort								*/
/************************************************************************/
int	MIOPC_InPort (int pmPortAddress)
{
    // Read the port value
    return MDIOPC_InPort (pmPortAddress);
} // MIOPC_InPort


/************************************************************************/
/* MIOPC_OutPort							*/
/************************************************************************/
void	MIOPC_OutPort (int pmPortAddress, int pmValue)
{
    // Check for valid value
    if ((pmValue < 0) || (pmValue > 255))
    {
    	// TW
    }

    // Set the port value
    MDIOPC_OutPort (pmPortAddress, (unsigned char) pmValue);
} // MIOPC_OutPort


/************************************************************************/
/* MIOPC_ParallelGet							*/
/************************************************************************/
int	MIOPC_ParallelGet (int pmPort)
{
    // Check for valid parallel port number
    if ((pmPort < 1) || (pmPort > 4))
    {
    	// TW
    }

    // Read the parallel port value
    return MDIOPC_ParallelGet (pmPort);
} // MIOPC_ParallelGet


/************************************************************************/
/* MIOPC_ParallelPut							*/
/************************************************************************/
void	MIOPC_ParallelPut (int pmPort, int pmValue)
{
    // Check for valid parallel port number
    if ((pmPort < 1) || (pmPort > 4))
    {
    	// TW
    }

    // Check for valid value
    if ((pmValue < 0) || (pmValue > 255))
    {
    	// TW
    }

    // Set the parallel port value
    MDIOPC_ParallelPut (pmPort, (unsigned char) pmValue);
} // MIOPC_ParallelPut


/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/



