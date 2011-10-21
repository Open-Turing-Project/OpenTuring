/**************/
/* mdiotime.c */
/**************/

/*******************/
/* System includes */
/*******************/
#include <windows.h>

/****************/
/* Self include */
/****************/
#include "mdiotime.h"

/******************/
/* Other includes */
/******************/

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
static LARGE_INTEGER	stCounterFrequency;
static LARGE_INTEGER	stStartCount;
static BOOL		stUseTicks;

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
/* MDIOTime_Init							*/
/************************************************************************/
void	MDIOTime_Init (void)
{
    if (QueryPerformanceCounter (&stStartCount)) 
    {
    	stUseTicks = FALSE;

	QueryPerformanceFrequency (&stCounterFrequency);
    }
    else
    {
    	stUseTicks = TRUE;
    }
} // MDIOTime_Init


/************************************************************************/
/* MDIOTime_GetTicks							*/
/************************************************************************/
long	MDIOTime_GetTicks (void)
{
    if (stUseTicks)
    {
    	return timeGetTime ();
    }
    else
    {
    	LARGE_INTEGER	myLargeInteger;
	__int64		myDifference;
	
    	QueryPerformanceCounter (&myLargeInteger);
    	myDifference = ((__int64) myLargeInteger.QuadPart) - 
    		       ((__int64) stStartCount.QuadPart);
    	return (long) (myDifference * 1000 / 
    		       (__int64) stCounterFrequency.QuadPart);
    }
} // MDIOTime_GetTicks


/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/


