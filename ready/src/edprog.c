/************/
/* edprog.c */
/************/

/*******************/
/* System includes */
/*******************/

/****************/
/* Self include */
/****************/
#include "edprog.h"

/******************/
/* Other includes */
/******************/
#include "edfail.h"
#include "edgui.h"

#include "eddialogs.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/

/********************/
/* Global variables */
/********************/
// The globals are constants for the entire program and include 
// registration information, etc.
Globals		gProgram;
// The user preferences.
Properties	gProperties;

/*********/
/* Types */
/*********/
typedef struct ExpiryDate
{
    WORD	year;
    WORD	month;
    WORD	day;
} ExpiryDate;

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
/* EdProg_Init								*/
/*									*/
/* Initializes the EdProg module.  Fills in all the globals.		*/
/************************************************************************/
extern BOOL	EdProg_Init (void)
{
    FilePath		myLangDLLPath;
    HMODULE		myLangLibrary;
    ExpiryDate		myExpiryDate;
    SYSTEMTIME		mySystemTime;
    FilePath		myApplicationName;
    DWORD		myDummy;
    DWORD		myVersionSize;
    char		*myVersionData, *myVersionInfo;
    UINT		myVersionInfoSize;
    OSVERSIONINFO	myOSVersionInfo;
    
    EdFile_ConstructPath (myLangDLLPath, IDS_READY_BIN, IDS_LANG_DLL);
    
    myLangLibrary = LoadLibrary (myLangDLLPath);
    if (myLangLibrary == NULL)
    {
    	EdFail_Warn (IDS_LOADLIBRARYFAIL, __FILE__, __LINE__, GetLastError (),
    		     myLangDLLPath);
    	return FALSE;
    }

    if (!EdGUI_LoadResource (myLangLibrary, LANGUAGE_RSRC, &gProgram.language, 
    			     sizeof (int)))
        return FALSE;
    if (gProgram.language == LANGUAGE_KIND_TURING)
    {
    	EdGUI_LoadString (IDS_TURING_ENVIRONMENT_NAME,
	    gProgram.environmentName, sizeof (gProgram.environmentName));
	gProgram.isTuring = TRUE;	    
    }
    else if (gProgram.language == LANGUAGE_KIND_JAVA)
    {
    	EdGUI_LoadString (IDS_JAVA_ENVIRONMENT_NAME,
	    gProgram.environmentName, sizeof (gProgram.environmentName));
	gProgram.isJava = TRUE;	    
    }
    else
    {
    	EdFail_Warn (IDS_UNKNOWNLANGUAGE, __FILE__, __LINE__, 
    		     gProgram.language);
    }
    	        
    if (!EdGUI_LoadResource (myLangLibrary, INSTALL_KIND_RSRC, 
    			     &gProgram.installKind, sizeof (int)))
        return FALSE;
    if (!EdGUI_LoadResource (myLangLibrary, MINI_VERSION_RSRC, 
    			     &gProgram.miniVersion, sizeof (int)))
        return FALSE;
    if (!EdGUI_LoadResource (myLangLibrary, RESTRICTED_VERSION_RSRC, 
    			     &gProgram.restrictedVersion, sizeof (int)))
        return FALSE;
    if (!EdGUI_LoadResource (myLangLibrary, ASSISTED_RSRC, 
    			     &gProgram.assistedByIBM, sizeof (int)))
        return FALSE;

    //
    // Set the expiry date (set to 2200/1/1 if not evaluation version)
    //
    ZeroMemory (&mySystemTime, sizeof (SYSTEMTIME));
    if ((gProgram.installKind == INSTALL_KIND_EVAL) ||
	(gProgram.installKind == INSTALL_KIND_BETA))
    {        
    	if (!EdGUI_LoadResource (myLangLibrary, EXPIRY_DATE_RSRC, &myExpiryDate, 
    				 sizeof (ExpiryDate)))
            return FALSE;    
            
    	mySystemTime.wYear = (WORD) (myExpiryDate.year - DATE_CYPHER);
    	mySystemTime.wMonth = (WORD) (myExpiryDate.month - DATE_CYPHER);
    	mySystemTime.wDay = (WORD) (myExpiryDate.day - DATE_CYPHER);
    	mySystemTime.wHour = 23;
    	mySystemTime.wMinute = 59;
    	mySystemTime.wSecond = 59;
    }
    else
    {
    	mySystemTime.wYear = 2100;
    	mySystemTime.wMonth = 1;
    	mySystemTime.wDay = 1;
    }
    GetDateFormat (LOCALE_USER_DEFAULT, 0, &mySystemTime, "MMM d yyyy", 
        gProgram.expiryDateString, sizeof (gProgram.expiryDateString));

    if (!SystemTimeToFileTime (&mySystemTime, &gProgram.expiryDate))
    {
    	EdFail_Warn (IDS_SYSTEMTIMETOFILETIMEFAIL, __FILE__, __LINE__, 
    		     GetLastError ());
    	return FALSE;
    }
    
    if (mySystemTime.wYear == 2100)
    {
    	gProgram.expiryDateString [0] = 0;
    }

    //
    // Get the current version
    //
    
    strcpy (gProgram.versionNumber, "Unknown");
    
    // Get the applicaton name
    if (GetModuleFileName (NULL, myApplicationName, PATHNAME_LENGTH) != 0)
    {
        myVersionSize = GetFileVersionInfoSize (myApplicationName, &myDummy);
        if (myVersionSize != 0)
    	{
    
	    myVersionData = malloc (myVersionSize);
	    if (myVersionData != NULL)
	    {
	        if (GetFileVersionInfo (myApplicationName, myDummy, 
	        		        myVersionSize, myVersionData))
    		{
    	
		    if (VerQueryValue (myVersionData, 
		        "\\StringFileInfo\\04090000\\ProductVersion", 
		        &myVersionInfo, &myVersionInfoSize))
		    {
		    	strncpy (gProgram.versionNumber, myVersionInfo, 
		    		 myVersionInfoSize);
		    	gProgram.versionNumber [myVersionInfoSize] = 0;
		    }
    		}
	    	free (myVersionData);
    	    }
        }
    }

    FreeLibrary (myLangLibrary);

    // Set the OS version
    ZeroMemory (&myOSVersionInfo, sizeof (myOSVersionInfo));
    myOSVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx (&myOSVersionInfo);

    gProgram.operatingSystem = UNKNOWN_OS;
    gProgram.servicePack [0] = 0;

    if ((myOSVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) &&
	(myOSVersionInfo.dwMajorVersion == 4))
    {
        if (myOSVersionInfo.dwMinorVersion == 0)
        {
            if (myOSVersionInfo.szCSDVersion[1] == 'C')
	    {
		gProgram.operatingSystem = WIN_95_OSR2;
	    }
	    else
	    {
		gProgram.operatingSystem = WIN_95;
	    }
        } 
        else if (myOSVersionInfo.dwMinorVersion == 10)
        {
            if (myOSVersionInfo.szCSDVersion[1] == 'A')
	    {
		gProgram.operatingSystem = WIN_98_SE;
	    }
	    else
	    {
		gProgram.operatingSystem = WIN_98;
	    }
        } 
        else if (myOSVersionInfo.dwMinorVersion == 90)
        {
	    gProgram.operatingSystem = WIN_ME;
        } 
    }
    else if (myOSVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
    {
        if (myOSVersionInfo.dwMajorVersion == 4)
	{
	    gProgram.operatingSystem = WIN_NT;
	}
        else if (myOSVersionInfo.dwMajorVersion == 5)
	{
	    if (myOSVersionInfo.dwMinorVersion == 0)
	    {
		gProgram.operatingSystem = WIN_2000;
	    }
	    else
	    {
		gProgram.operatingSystem = WIN_XP;
	    }
	}
	else if (myOSVersionInfo.dwMajorVersion > 5)
	{
	    gProgram.operatingSystem = WIN_NEWER;
	}

	// Get service pack name
	strncpy (gProgram.servicePack, myOSVersionInfo.szCSDVersion, 130);
	gProgram.servicePack [130] = 0;
    }
    
    gProgram.globalsInitialized = TRUE;
    
    return TRUE;
} // EdProg_Init

/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/