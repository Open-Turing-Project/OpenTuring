/****************/
/* mdioconfig.c */
/****************/

/*******************/
/* System includes */
/*******************/
#include <windows.h>

/****************/
/* Self include */
/****************/
#include "mdioconfig.h"

/******************/
/* Other includes */
/******************/

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
#define MY_ERROR_WRONG_OS   0x20000000

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
/* MDIOConfig_GetOS							*/
/************************************************************************/
int	MDIOConfig_GetOS (void)
{
    OSVERSIONINFO	myOSVersionInfo;

    ZeroMemory (&myOSVersionInfo, sizeof (myOSVersionInfo));
    myOSVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx (&myOSVersionInfo);

    if (myOSVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
    {
	if (myOSVersionInfo.dwMinorVersion == 0)
	{
	    return 95;	    // Windows 95
	}
	else if (myOSVersionInfo.dwMinorVersion == 10)
	{
	    return 98;	    // Windows 98
	}
	else if (myOSVersionInfo.dwMinorVersion == 90)
	{
	    return 99;	    // Windows Me
	}
    }
    else
    {
	if (myOSVersionInfo.dwMajorVersion == 4)
	{
	    return 40;	    // Windows NT 4.0
	}
	else if (myOSVersionInfo.dwMinorVersion == 0)
	{
	    return 2000;    // Windows 2000
	}
	else if (myOSVersionInfo.dwMinorVersion == 1)
	{
	    return 2001;    // Windows 2001
	}
    }

    return 0;
} // MDIOConfig_GetOS


/************************************************************************/
/* MDIOConfig_GetProcessor						*/
/************************************************************************/
int	MDIOConfig_GetProcessor (void)
{
    SYSTEM_INFO		mySystemInfo;
    OSVERSIONINFO	myOSVersionInfo;

    ZeroMemory (&myOSVersionInfo, sizeof (myOSVersionInfo));
    myOSVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx (&myOSVersionInfo);

    GetSystemInfo (&mySystemInfo);

    // If we're running Win95/98/Me, use dwProcessorType
    if (myOSVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
    {
	if (mySystemInfo.dwProcessorType == PROCESSOR_INTEL_386)
	{
	    return 386;
	}
	else if (mySystemInfo.dwProcessorType == PROCESSOR_INTEL_486)
	{
	    return 486;
	}
	else if (mySystemInfo.dwProcessorType == PROCESSOR_INTEL_PENTIUM)
	{
	    return 586;
	}
    }
    else
    {
	if (mySystemInfo.wProcessorLevel == 3)
	{
	    return 386;
	}
	else if (mySystemInfo.wProcessorLevel == 4)
	{
	    return 486;
	}
	else if (mySystemInfo.wProcessorLevel == 5)
	{
	    return 586;
	}
	else if (mySystemInfo.wProcessorLevel == 6)
	{
	    return 686;
	}
    }

    return 0;
} // MDIOConfig_GetProcessor


/************************************************************************/
/* MDIOConfig_IsCoProcessorPresent							*/
/************************************************************************/
BOOL	MDIOConfig_IsCoProcessorPresent (void)
{
    HKEY	myKey;
    SYSTEM_INFO mySystemInfo;

    // return FALSE if we are not running under Windows NT
    // this should be expanded to cover alternative Win32 platforms
    if (!(GetVersion() & 0x7FFFFFFF))
    {
        SetLastError (MY_ERROR_WRONG_OS);
        return (FALSE);
    }

    // we return TRUE if we're not running on x86
    // other CPUs have built in floating-point, with no registry entry

    GetSystemInfo (&mySystemInfo);

    if ((mySystemInfo.dwProcessorType != PROCESSOR_INTEL_386) &&
        (mySystemInfo.dwProcessorType != PROCESSOR_INTEL_486) &&
        (mySystemInfo.dwProcessorType != PROCESSOR_INTEL_PENTIUM))
    {
        return TRUE;
    }

    if (RegOpenKeyEx (HKEY_LOCAL_MACHINE,
		"HARDWARE\\DESCRIPTION\\System\\FloatingPointProcessor",
		0, KEY_EXECUTE, &myKey) != ERROR_SUCCESS)
    {
        // GetLastError() will indicate ERROR_RESOURCE_DATA_NOT_FOUND
        // if we can't find the key.  This indicates no coprocessor
	// present
        return(FALSE);
    }

    RegCloseKey (myKey);

    return(TRUE);
} // MDIOConfig_IsCoProcessorPresent


/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/


