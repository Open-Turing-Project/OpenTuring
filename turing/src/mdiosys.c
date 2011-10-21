/*************/
/* mdiosys.c */
/************/

/*******************/
/* System includes */
/*******************/
#include <windows.h>

/****************/
/* Self include */
/****************/
#include "mdiosys.h"

/******************/
/* Other includes */
/******************/
#include "mio.h"
#include "mdio.h"
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
/* MDIOSys_Exec								*/
/************************************************************************/
int	MDIOSys_Exec (const char *pmCommandStr, const char *pmParameterStr,
		      const char *pmWorkingDirectoryStr)
{
    int	myResult;
    
    myResult = (int) ShellExecute (NULL, "open", pmCommandStr, pmParameterStr,
				   pmWorkingDirectoryStr, SW_SHOWNORMAL);
				   
    if (myResult >= 32)
    {				   
    	return 1;
    }
    else
    {
	if ((myResult == 0) ||
	    (myResult == SE_ERR_OOM))
	{
	    SET_ERRNO (E_SYS_NOT_ENOUGH_SYSTEM_MEMORY);
	}
	else if ((myResult == ERROR_FILE_NOT_FOUND) ||
	         (myResult == SE_ERR_FNF))
	{
	    SET_ERRMSG (E_FSYS_FILE_NOT_FOUND, "File '%s' not found", 
			pmCommandStr);
	}
	else if ((myResult == ERROR_PATH_NOT_FOUND) || 
		 (myResult == SE_ERR_PNF))
	{
    	    SET_ERRMSG (E_FSYS_PATH_NOT_FOUND, 
    	 	        "Path not found (couldn't find the directory "
    	 	        "that file '%s' is in)", pmCommandStr);
	}
	else if (myResult == ERROR_BAD_FORMAT)
	{
	    SET_ERRNO (E_SYS_BAD_EXE_FILE);
	}
	else if (myResult == SE_ERR_ACCESSDENIED)
	{
	    SET_ERRNO (E_SYS_ACCESS_DENIED);
	}
	else if (myResult == SE_ERR_ASSOCINCOMPLETE)
	{
	    SET_ERRNO (E_SYS_BAD_FILE_ASSOC);
	}
	else if (myResult == SE_ERR_NOASSOC)
	{
	    SET_ERRNO (E_SYS_NO_FILE_ASSOC);
	}
	else if (myResult == SE_ERR_SHARE)
	{
	    SET_ERRNO (E_SYS_SHARING_VIOLATION);
	}
	else
	{
	    SET_ERRMSG (E_SYS_UNKNOWN_ERROR,
			"Unknown system error.  Windows shell error = %d",
			myResult);
	}
    	return 0;
    }
} // MDIOSys_Exec


/************************************************************************/
/* MDIOSys_GetComputerName						*/
/************************************************************************/
void	MDIOSys_GetComputerName (char *pmComputerName)
{
    int	mySize = 255;
    
    if (!GetComputerName (pmComputerName, &mySize))
    {
    	strcpy (pmComputerName, "No Name");
    }
} // MDIOSys_GetComputerName


/************************************************************************/
/* MDIOSys_GetEnv							*/
/************************************************************************/
void	MDIOSys_GetEnv (const char *pmSymbolStr, char *pmResultStr)
{
    char	myEnvironmentVariable [500];
    char	myMessage [1024];
    int		myResult;
    
    wsprintf (myEnvironmentVariable, "%%%s%%", pmSymbolStr);
    myResult = ExpandEnvironmentStrings (myEnvironmentVariable, 
					 pmResultStr, 255);
    
    // If the environment variable is not found, then myEnvironmentVariable
    // is left unchanged.  In this case, we want it to return an empty string.
    if (strcmp (pmResultStr, myEnvironmentVariable) == 0)
    {
    	pmResultStr [0] = 0;
	MDIO_sprintf (myMessage, "Environment variable \"%s\" not found", 
		      pmSymbolStr);
	SET_ERRMSG (E_SYS_ENV_VAR_NOT_FOUND, myMessage);
    }

    // If the environment variable doesn't fit into the Turing string, set the
    // string to empty string at set an error message.
    if (myResult > 255)
    {
    	pmResultStr [0] = 0;
	MDIO_sprintf (myMessage, 
		      "Environment variable \"%s\" too long (%d characters)", 
		      pmSymbolStr, myResult);
	SET_ERRMSG (E_SYS_ENV_VAR_TOO_LONG, myMessage);
    }
} // MDIOSys_GetEnv


/************************************************************************/
/* MDIOSys_GetPID							*/
/************************************************************************/
int	MDIOSys_GetPID (void)
{
    return GetCurrentProcessId ();
} // MDIOSys_GetPID


/************************************************************************/
/* MDIOSys_GetUserName							*/
/************************************************************************/
void	MDIOSys_GetUserName (char *pmUserName)
{
    int	mySize = 255;
    
    if (!GetUserName (pmUserName, &mySize))
    {
    	strcpy (pmUserName, "Unknown");
    }
} // MDIOSys_GetUserName


/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/
