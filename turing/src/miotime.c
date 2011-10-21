/************************************************************************/
/* miotime.c								*/
/*									*/
/* OOT has two time formats:						*/
/*									*/
/* ootTime     - seconds since Jan 1, 1970  00:00:00 GMT		*/
/* ootTimeStr  - "<d|' '>d mmm yy hh:mm:ss"				*/
/************************************************************************/

/*******************/
/* System includes */
/*******************/
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

/****************/
/* Self include */
/****************/
#include "miotime.h"

/******************/
/* Other includes */
/******************/
#include "mio.h"

#include "mdiotime.h"

#include "mioerr.h"

#include "edint.h"

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
#define DATE_STR_LEN		9
#define TIME_STR_LEN		8
#define DATETIME_STR_LEN	DATE_STR_LEN + TIME_STR_LEN + 1
#define SPACE			' '

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
static char	*stMonths[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
	  			  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

/********************/
/* Static variables */
/********************/
static long		stStartTick;

/******************************/
/* Static callback procedures */
/******************************/

/*********************/
/* Static procedures */
/*********************/
static int	MyConvertDateStrToSeconds (char *pmDateString);
static int	MyConvertTimeStrToSeconds (char *pmTimeString);


/*********************************************/
/* External procedures for Turing predefines */
/*********************************************/
/************************************************************************/
/* MIOTime_Date								*/
/************************************************************************/
void	MIOTime_Date (OOTstring pmOOTimeStr)
{
    MIOTime_SecDate (pmOOTimeStr, MIOTime_Sec ());
} // MIOTime_Date


/************************************************************************/
/* MIOTime_DateSec							*/
/*									*/
/* Parse a string, converting it into OOT time (perhaps relative)	*/
/* String can be in one of three forms:					*/
/*									*/
/* "dd mmm yy"		  - returns ootTime based on midnight		*/
/* "hh:mm:ss"		  - returns seconds since midnight		*/
/* "dd mmm yy hh:mm:ss"   - returns ootTime				*/
/************************************************************************/
OOTint	MIOTime_DateSec (OOTstring pmDateTimeStr)
{
    int myOOTTime;

    /* Ignore white space at beginning */

    if (*pmDateTimeStr == 0) 
    {
	SET_ERRNO(E_TIME_INVALID_TIME_STR);
	return -1;
    }

    if (pmDateTimeStr [2] == ':') 
    {
	/* Only time (possibly) */
	myOOTTime = MyConvertTimeStrToSeconds (pmDateTimeStr);

	if (myOOTTime == -1) 
	{
	    SET_ERRNO(E_TIME_INVALID_TIME_STR);
	}
    }
    else 
    {
        char *myTimeStr = pmDateTimeStr;

	myOOTTime = MyConvertDateStrToSeconds (myTimeStr);

	if (myOOTTime == -1) 
	{
	    SET_ERRNO(E_TIME_INVALID_TIME_STR);
	    return -1;
	}

	myTimeStr += DATE_STR_LEN;

	if (*myTimeStr) 
	{
	    int myTimeSecs = MyConvertTimeStrToSeconds (myTimeStr + 1);

	    if (myTimeSecs == -1) 
	    {
	        SET_ERRNO(E_TIME_INVALID_TIME_STR);
	        return -1;
	    }
	    myOOTTime += myTimeSecs;
	}
    }
    return myOOTTime;
} // MIOTime_DateSec


/************************************************************************/
/* MIOTime_Elapsed							*/
/************************************************************************/
OOTint	MIOTime_Elapsed (void)
{
    return MDIOTime_GetTicks () - stStartTick;
} // MIOTime_Elapsed


/************************************************************************/
/* MIOTime_ElapsedCPU							*/
/************************************************************************/
OOTint	MIOTime_ElapsedCPU (void)
{
    return MDIOTime_GetTicks () - stStartTick;
} // MIOTime_ElapsedCPU


/************************************************************************/
/* MIOTime_PartsSec							*/
/************************************************************************/
OOTint	MIOTime_PartsSec (OOTint pmYear, OOTint pmMon, OOTint pmDay, 
		          OOTint pmHour, OOTint pmMin, OOTint pmSec)
{
    struct tm	myTimeRecord;

    myTimeRecord.tm_mday = pmDay;
    myTimeRecord.tm_mon = pmMon - 1;
    myTimeRecord.tm_year = pmYear - 1900;

    myTimeRecord.tm_hour = pmHour;
    myTimeRecord.tm_min = pmMin;
    myTimeRecord.tm_sec = pmSec;

    myTimeRecord.tm_isdst = -1;

    return (OOTint) mktime (&myTimeRecord);
} // MIOTime_PartsSec


/************************************************************************/
/* MIOTime_Sec								*/
/************************************************************************/
OOTint	MIOTime_Sec (void)
{
    return (OOTint) time (NULL);
} // MIOTime_Sec


/************************************************************************/
/* MIOTime_SecDate							*/
/************************************************************************/
void	MIOTime_SecDate (OOTstring pmOOTimeStr, OOTint pmOOTTime)
{
    if (pmOOTTime == -1)
    {
	pmOOTimeStr [0] = 0;
	SET_ERRNO (E_TIME_SECONDS_OUT_OF_RANGE);
    }
    else
    {
	struct tm *myTimeRec = localtime ((time_t *) &pmOOTTime);
	
	strftime (pmOOTimeStr, DATETIME_STR_LEN + 1, "%d %b %y %H:%M:%S", myTimeRec);
	
	if (pmOOTimeStr[0] == '0')
	{
	    pmOOTimeStr[0] = SPACE;
	}
    }
} // MIOTime_SecDate


/************************************************************************/
/* MIOTime_SecParts							*/
/************************************************************************/
void	MIOTime_SecParts (OOTint pmOOTTime, OOTint *pmYear, OOTint *pmMon, 
			  OOTint *pmDay, OOTint *pmDOW, OOTint *pmHour, 
		          OOTint *pmMin, OOTint *pmSec)
{
    struct tm	*myTimeRecord = localtime ((time_t *) &pmOOTTime);

    *pmYear = myTimeRecord -> tm_year + 1900;
    *pmMon = myTimeRecord -> tm_mon + 1;
    *pmDay = myTimeRecord -> tm_mday;

    *pmDOW = myTimeRecord -> tm_wday + 1;

    *pmHour = myTimeRecord -> tm_hour;
    *pmMin = myTimeRecord -> tm_min;
    *pmSec = myTimeRecord -> tm_sec;
} // MIOTime_SecParts


/************************************************************************/
/* MIOTime_SecStr							*/
/*									*/
/* An ootTime to general string conversion facility using the		*/
/* ANSI/POSIX format directives.					*/
/************************************************************************/
void	MIOTime_SecStr (OOTstring pmString, OOTint pmOOTTime, 
		        OOTstring pmFormatStr)
{
    struct tm	*myTimeRecord = localtime ((time_t *) &pmOOTTime);

    strftime (pmString, STRLEN, pmFormatStr, myTimeRecord);
} // MIOTime_SecStr


/***************************************/
/* External procedures for MIO library */
/***************************************/
/************************************************************************/
/* MIOTime_Init								*/
/************************************************************************/
void	MIOTime_Init (void)
{
    MDIOTime_Init ();
} // MIOTime_Reset


/************************************************************************/
/* MIOTime_Init_Run							*/
/************************************************************************/
void	MIOTime_Init_Run (void)
{
    stStartTick = MDIOTime_GetTicks ();
} // MIOTime_Init_Run


/************************************************************************/
/* MIOTime_GetTicks							*/
/************************************************************************/
int	MIOTime_GetTicks (void)
{
    return MDIOTime_GetTicks ();
} // MIOTime_GetTicks


/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/
/************************************************************************/
/* MyConvertTimeStrToSeconds						*/
/************************************************************************/
static int	MyConvertDateStrToSeconds (char *pmDateString)
{
    struct tm	myTimeRecord;
    char 	myDateString [DATE_STR_LEN + 1];
    int		cnt;

    if ((strlen (pmDateString) >= DATE_STR_LEN) && 
    	(pmDateString [2] == SPACE) && (pmDateString [6] == SPACE)) 
    {
	strncpy (myDateString, pmDateString, DATE_STR_LEN);
	myDateString [DATE_STR_LEN] = 0;
    }
    else
    {
	return -1;
    }
    
    if (!((isdigit (myDateString [0]) || (myDateString [0] == SPACE)) && 
    	  isdigit (myDateString [1]) && isdigit (myDateString [7]) && 
    	  isdigit (myDateString [8])))
    {
	return -1;
    }

    myTimeRecord.tm_hour = 0;
    myTimeRecord.tm_min  = 0;
    myTimeRecord.tm_sec  = 0;
    myTimeRecord.tm_isdst = -1;

    // Convert the month
    for (cnt = 0; cnt < 12; cnt++) 
    {
	char myString [5];

	myTimeRecord.tm_mon = cnt;

	/* Test against english */
	if (strncmp (myDateString + 3, stMonths [cnt], 3) == 0)
	{
	    break;
	}
	strftime (myString, sizeof (myString), "%h", &myTimeRecord);

	/* Test against locale's month abbreviation */
	if (strncmp (myDateString + 3, myString, 3) == 0)
	{
	    break;
	}
    }

    if (cnt == 12) 
    {
	return -1;
    }

    myTimeRecord.tm_mday = 
    	atoi ((myDateString[0] == SPACE) ? myDateString + 1 : myDateString);
    myTimeRecord.tm_year = atoi (myDateString + 7);

    if (myTimeRecord.tm_year < 70)
    {
	myTimeRecord.tm_year += 100;
    }

    return (int) mktime (&myTimeRecord);
} // MyConvertDateStrToSeconds


/************************************************************************/
/* MyConvertTimeStrToSeconds						*/
/************************************************************************/
static int	MyConvertTimeStrToSeconds (char *pmTimeString)
{
    if ((pmTimeString [2] != ':') || (pmTimeString[5] != ':') || 
        (strlen (pmTimeString) != TIME_STR_LEN))
    {
	return -1;
    }

    if (!(isdigit (pmTimeString [0]) && isdigit (pmTimeString [1]) && 
    	  isdigit (pmTimeString [3]) && isdigit (pmTimeString [4]) && 
    	  isdigit (pmTimeString [6]) && isdigit (pmTimeString [7])))
    {
	return -1;
    }

    return atoi (pmTimeString) * 3600 + atoi (pmTimeString + 3) * 60 + 
    	   atoi (pmTimeString + 6);
} // MyConvertTimeStrToSeconds


