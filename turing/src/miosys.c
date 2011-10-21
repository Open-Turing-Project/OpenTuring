/************/
/* miosys.c */
/************/

/*******************/
/* System includes */
/*******************/
#include <string.h>

/****************/
/* Self include */
/****************/
#include "miosys.h"

/******************/
/* Other includes */
/******************/
#include "miotypes.h"

#include "mdiosys.h"

#include "miofile.h"

#include "mio.h"

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
static BOOL	stAllowSysExec;
static char	*stSysExitString;

/******************************/
/* Static callback procedures */
/******************************/

/*********************/
/* Static procedures */
/*********************/
static BOOL	MyExtractPath (char *pmString, char *pmElements[]);


/***********************/
/* External procedures */
/***********************/
/************************************************************************/
/* MIOSys_Init								*/
/************************************************************************/
void	MIOSys_Init (const char *pmSysExitString)
{
    stSysExitString = MIO_MallocString (pmSysExitString);
} // MIOSys_SetSysExitString


/************************************************************************/
/* MIOSys_Init_Run							*/
/************************************************************************/
void	MIOSys_Init_Run (BOOL pmAllowSysExec)
{
    stAllowSysExec = pmAllowSysExec;
} // MIOSys_Init_Run


/************************************************************************/
/* MIOSys_Exec								*/
/************************************************************************/
int	MIOSys_Exec (const char *pmCommandStr)
{
    char	myDirectory [4096];
    char	myCommandStr [256];
    char	*myElements [2];

    if (!MIOFile_ConvertPath (".", NULL, myDirectory, ALLOW_TRAILING_SLASH))
    {
    	// if MIOFile_ConvertPath returned false, then the error message was 
    	// set to indicate why.
    	return 0;
    }
    
    strcpy (myCommandStr, pmCommandStr);
    if (!MyExtractPath (myCommandStr, myElements))
    {
    	// if MyExtractPath returned false, then the error message was 
    	// set to indicate why.
    	return 0;
    }

    if (!stAllowSysExec)
    {
	SET_ERRNO (E_SYS_SYSEXEC_NOT_ALLOWED);
	return 0;
    }

    return MDIOSys_Exec (myElements [0], myElements [1], myDirectory);
} // MIOSys_Exec


/************************************************************************/
/* MIOSys_Exit								*/
/************************************************************************/
void	MIOSys_Exit (void)
{
    MIOError_Abort (0, stSysExitString);
} // MIOSys_Exit


/************************************************************************/
/* MIOSys_GetComputerName						*/
/************************************************************************/
void	MIOSys_GetComputerName (char *pmComputerName)
{
    MDIOSys_GetComputerName (pmComputerName);
} // MIOSys_GetComputerName


/************************************************************************/
/* MIOSys_GetEnv							*/
/************************************************************************/
void	MIOSys_GetEnv (const char *pmSymbolStr, char *pmResultStr)
{
    MDIOSys_GetEnv (pmSymbolStr, pmResultStr);
} // MIOSys_GetEnv


/************************************************************************/
/* MIOSys_GetPID							*/
/************************************************************************/
int	MIOSys_GetPID (void)
{
    return MDIOSys_GetPID ();
} // MIOSys_GetPID


/************************************************************************/
/* MIOSys_GetUserName							*/
/************************************************************************/
void	MIOSys_GetUserName (char *pmUserName)
{
    MDIOSys_GetUserName (pmUserName);
} // MIOSys_GetUserName


/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/
/************************************************************************/
/* MyExtractPath							*/
/************************************************************************/
static BOOL	MyExtractPath (char *pmString, char *pmElements[])
{
    int		myArgument = 0;
    char	*myPtr = pmString, myQuote;
    
    // Skip over whitespace
    while ((*myPtr == ' ') || (*myPtr == '\t'))
    {
    	myPtr++;
    }
    	
    if (*myPtr == 0)
    {
    	// TW Error - Nothing in string
    	return FALSE;
    }

    // If we start with a quote, convert until the end quote
    if ((*myPtr == '"') || (*myPtr == '\''))
    {
        myQuote = *myPtr;
        myPtr++;
        pmElements [0] = myPtr;
        while ((*myPtr != myQuote) && (*myPtr != 0))
        {
            myPtr++;
        }
        if (*myPtr == 0)
        {
            // TW Error - Unterminated quote
            return FALSE;
        }
        *myPtr = 0;
        myPtr++;
    }
    else
    {
        pmElements [0] = myPtr;
    	while ((*myPtr != ' ') && (*myPtr != '\t') && (*myPtr != 0))
    	{
            myPtr++;
    	}
    	if (*myPtr != 0)
    	{ 
    	    *myPtr = 0;
    	    myPtr++;
    	}
    }
    
    // Skip over whitespace
    while ((*myPtr == ' ') || (*myPtr == '\t'))
    {
    	myPtr++;
    }

    if (*myPtr == 0)
    {
	pmElements [1] = NULL;
    }
    else
    {
    	pmElements [1] = myPtr;
    }
    
    return TRUE;
} // MyExtractPath





