/***********/
/* eddde.c */
/***********/

/*******************/
/* System includes */
/*******************/
#include <windows.h>
#include <windowsx.h>
#include <crtdbg.h>

/****************/
/* Self include */
/****************/
#include "eddde.h"

/******************/
/* Other includes */
/******************/
#include "edfail.h"
#include "edgui.h"
#include "edwin.h"

#include "eddialogs.h"

/**********/
/* Macros */
/**********/
#define OPEN_COMMAND		"open("
#define PRINT_COMMAND		"print("

/*************/
/* Constants */
/*************/
static char	stJavaAppName[] = "ReadyToProgram";
static char	stTuringAppName[] = "Turing";
static char	stTopicName[] = "System";
static HSZ	stAppNameHandle;
static HSZ	stTopicNameHandle;

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
// Properties of the status window
static EdDDE_Properties	stProperties;
static BOOL		stPropertiesChanged;

// The DDE Instance
static DWORD	stDDEInstance = 0;

/******************************/
/* Static callback procedures */
/******************************/
static HDDEDATA	CALLBACK MyDDECallback (UINT pmTransactionType, UINT pmFormat, 
			       HCONV pmConversion, HSZ pmString1, HSZ pmString2,
			       HDDEDATA pmData, DWORD pmData1, DWORD pmData2);
static HDDEDATA	CALLBACK MyDDEEmptyCallback (UINT pmTransactionType, 
				UINT pmFormat, HCONV pmConversion, 
				HSZ pmString1, HSZ pmString2,
			        HDDEDATA pmData, DWORD pmData1, DWORD pmData2);

/*********************/
/* Static procedures */
/*********************/
static void	MyExecuteDDECommand (char *pmCommand);

/***********************/
/* External procedures */
/***********************/
/************************************************************************/
/* EdDDE_Init								*/
/************************************************************************/
BOOL	EdDDE_Init (void)
{
    int		myResult;
    char	*myAppName;
    
    if (gProgram.isJava)
        myAppName = stJavaAppName;
    else if (gProgram.isTuring)
        myAppName = stTuringAppName;
        
    myResult = DdeInitialize (&stDDEInstance, (PFNCALLBACK) &MyDDECallback,
    	CBF_FAIL_ADVISES | CBF_FAIL_POKES | CBF_FAIL_REQUESTS | 
    	CBF_SKIP_REGISTRATIONS | CBF_SKIP_UNREGISTRATIONS, 0);
    if (myResult != DMLERR_NO_ERROR)
    {
    	EdFail_Warn (IDS_DDEINITIALIZEFAIL, __FILE__, __LINE__,
    	    myResult);
        return FALSE;
    }
    
    stAppNameHandle = DdeCreateStringHandle (stDDEInstance, myAppName, 0);
    if (stAppNameHandle == NULL)
    {
    	EdFail_Warn (IDS_DDECREATESTRINGHANDLEFAIL, __FILE__, __LINE__,
    	    DdeGetLastError (stDDEInstance));
        return FALSE;
    }

    stTopicNameHandle = DdeCreateStringHandle (stDDEInstance, stTopicName, 0);
    if (stTopicNameHandle == NULL)
    {
    	EdFail_Warn (IDS_DDECREATESTRINGHANDLEFAIL, __FILE__, __LINE__,
    	    DdeGetLastError (stDDEInstance));
        return FALSE;
    }

    if (!DdeNameService (stDDEInstance, stAppNameHandle, (HSZ) 0, 
        DNS_REGISTER))
    {
    	EdFail_Warn (IDS_DDENAMESERVICEFAIL, __FILE__, __LINE__,
    	    DdeGetLastError (stDDEInstance));
        return FALSE;
    }
    
    return TRUE;
} // EdDDE_Init


/************************************************************************/
/* EdDDE_Finalize							*/
/************************************************************************/
void	EdDDE_Finalize (void)
{
    DdeNameService (stDDEInstance, (HSZ) 0, (HSZ) 0, 0);
    DdeFreeStringHandle (stDDEInstance, stAppNameHandle);
    DdeFreeStringHandle (stDDEInstance, stTopicNameHandle);
    DdeUninitialize (stDDEInstance);
} // EdDDE_Finalize


/************************************************************************/
/* EdDDE_PropertiesSet							*/
/************************************************************************/
void	EdDDE_PropertiesSet (void)
{
    EdDDE_PropertiesImplementChanges ();
} // EdDDE_PropertiesSet

	
/************************************************************************/
/* EdDDE_PropertiesImplementChanges					*/
/************************************************************************/
void	EdDDE_PropertiesImplementChanges (void)
{
    // Do nothing, for the moment	
} // EdDDE_PropertiesImplementChanges


/************************************************************************/
/* EdDDE_TellReadyOpen							*/
/************************************************************************/
BOOL	EdDDE_TellReadyOpen (int pmNumFiles, char *pmFiles[])
{
    DWORD	myDDEInstance = 0;
    HCONV	myDDEConversation;
    HSZ		myAppNameHandle;
    HSZ		myTopicNameHandle;
    HDDEDATA	myDDEResult;
    int		myResult;
    char	*myAppName;
    int		cnt;
    
    
    if (gProgram.isJava)
        myAppName = stJavaAppName;
    else if (gProgram.isTuring)
        myAppName = stTuringAppName;

    myResult = DdeInitialize (&myDDEInstance, (PFNCALLBACK) &MyDDEEmptyCallback,
    	CBF_FAIL_ALLSVRXACTIONS, 0);
//    	CBF_FAIL_CONNECTIONS | CBF_FAIL_EXECUTES, 0);
    if (myResult != DMLERR_NO_ERROR)
    {
    	EdFail_Warn (IDS_DDEINITIALIZEFAIL, __FILE__, __LINE__,
    	    myResult);
        return FALSE;
    }
    
    myAppNameHandle = DdeCreateStringHandle (myDDEInstance, myAppName, 0);
    if (myAppNameHandle == NULL)
    {
    	EdFail_Warn (IDS_DDECREATESTRINGHANDLEFAIL, __FILE__, __LINE__,
    	    DdeGetLastError (myDDEInstance));
        return FALSE;
    }

    myTopicNameHandle = DdeCreateStringHandle (myDDEInstance, stTopicName, 0);
    if (myTopicNameHandle == NULL)
    {
    	EdFail_Warn (IDS_DDECREATESTRINGHANDLEFAIL, __FILE__, __LINE__,
    	    DdeGetLastError (myDDEInstance));
        return FALSE;
    }

    myDDEConversation = DdeConnect (myDDEInstance, myAppNameHandle, 
    				 myTopicNameHandle, NULL);
 
    if (myDDEConversation == NULL) 
    {
    	EdGUI_Message1 (NULL, 0, IDS_DDE_TROUBLE_TITLE, 
	    	        IDS_DDE_TROUBLE_CONNECT, 
	    	        DdeGetLastError (myDDEInstance));
        return FALSE; 
    } 
    
    for (cnt = 0 ; cnt < pmNumFiles ; cnt++)
    {
    	if (pmFiles [cnt] [0] != 0)
    	{
    	    char	myString [4096];
    	
    	    wsprintf (myString, "open(\"%s\")", pmFiles [cnt]);
    	
	    myDDEResult = DdeClientTransaction (myString, strlen (myString) + 1,
		    			        myDDEConversation, NULL, CF_TEXT, 
		    			        XTYP_EXECUTE, 3000, NULL);
	    if (myDDEResult == NULL)
	    {
    	    	EdGUI_Message1 (NULL, 0, IDS_DDE_TROUBLE_TITLE, 
	    	                IDS_DDE_TROUBLE_TRANS, 
	    	                DdeGetLastError (myDDEInstance));
	        return FALSE;
	    }
	}		    
    }	    

    if (!DdeDisconnect (myDDEConversation))
    {
    	EdGUI_Message1 (NULL, 0, IDS_DDE_TROUBLE_TITLE, 
	    	        IDS_DDE_TROUBLE_DISCONNECT, 
	    	        DdeGetLastError (myDDEInstance));
    	return FALSE;
    }
    
    DdeFreeStringHandle (myDDEInstance, myAppNameHandle);
    DdeFreeStringHandle (myDDEInstance, myTopicNameHandle);
    
    DdeUninitialize (myDDEInstance);

    return TRUE;
} // EdDDE_TellReadyOpen


/******************************/
/* Static callback procedures */
/******************************/

/************************************************************************/
/* MyDDECallback							*/
/************************************************************************/
static HDDEDATA	CALLBACK MyDDECallback (UINT pmTransactionType, UINT pmFormat, 
			       HCONV pmConversion, HSZ pmString1, HSZ pmString2,
			       HDDEDATA pmData, DWORD pmData1, DWORD pmData2)
{
    char	myCommandBuffer [4096];
    char	*myPtr, *myStart;
    BOOL	myInParens = FALSE, myInQuotes = FALSE;

    switch (pmTransactionType)
    {
    	case XTYP_CONNECT:

    	    if ((DdeCmpStringHandles (pmString1, stTopicNameHandle) == 0) &&
    	        (DdeCmpStringHandles (pmString2, stAppNameHandle) == 0))
	    {    	    
    	    	return (HDDEDATA) TRUE;
    	    }

    	    return FALSE;
    	    
    	case XTYP_EXECUTE:
    	    // Make certain application name matches
    	    if (DdeCmpStringHandles (pmString1, stTopicNameHandle) != 0)
    	    {
    	    	return FALSE;
    	    }
    	    
    	    DdeGetData (pmData, myCommandBuffer, sizeof (myCommandBuffer), 0);
    	    DdeFreeDataHandle (pmData);

	    //
	    // The command is a series of [Open(file)][Open(file)]...
	    //
	    myPtr = myCommandBuffer;
	    while (*myPtr != 0)
	    {
		// Skip to the first open bracket in command line
	    	while ((*myPtr != '[') && (*myPtr != 0))
	    	{
	    	   myPtr++;
	    	}
	    	if (*myPtr == 0)
	    	{
	    	    break;
	    	}
	    	myPtr++;
	    	myStart = myPtr;
	    	while (*myPtr != 0)
	    	{
		    // If we reach a close bracket and we're not in 
		    // a parentheses or quotes, then exit
		    if ((*myPtr == ']') && !myInParens && !myInQuotes)
		    {
			break;
		    }
		    // If we reach an open parenthesis and we're not in
		    // quotes, then up the paren count
		    if ((*myPtr == '(') && !myInQuotes)
		    {
			myInParens++;
		    }
		    // If we reach a close parenthesis and we're not in
		    // quotes, then lower the paren count
		    if ((*myPtr == ')') && !myInQuotes)
		    {
			myInParens--;
		    }
		    // If we reach a quote, then toggle the in quotes flag
		    if (*myPtr == '"')
		    {
			myInQuotes = !myInQuotes;
		    }

	    	    myPtr++;
	    	}
	    	if (*myPtr == 0)
	    	{
	    	    // Improperly terminated command
	    	    EdGUI_Message1 (NULL, 0, IDS_BAD_DDE_COMMAND_TITLE, 
	    	        IDS_BAD_DDE_COMMAND, myStart);
	    	}
	    	*myPtr = 0;
	    	myPtr++;
	    	MyExecuteDDECommand (myStart);
	    }
	    
    	    return (HDDEDATA) DDE_FACK;
    } // switch
    
    return NULL;
} // MyDDECallback


/************************************************************************/
/* MyDDECallback							*/
/************************************************************************/
static HDDEDATA	CALLBACK MyDDEEmptyCallback (UINT pmTransactionType, 
				UINT pmFormat, HCONV pmConversion, 
				HSZ pmString1, HSZ pmString2,
			        HDDEDATA pmData, DWORD pmData1, DWORD pmData2)
{
    return NULL;
} // MyDDEEmptyCallback


/*********************/
/* Static procedures */
/*********************/
/************************************************************************/
/* MyExecuteDDECommand							*/
/************************************************************************/
static void	MyExecuteDDECommand (char *pmCommand)
{
    if ((_strnicmp (pmCommand, OPEN_COMMAND, strlen (OPEN_COMMAND)) == 0) &&
	(pmCommand [strlen (OPEN_COMMAND)] == '"'))  		 
    {
    	char	*myPath;
    	
    	// Remove the terminating quote
    	while (pmCommand [strlen (pmCommand) - 1] != '"')
    	{
	    pmCommand [strlen (pmCommand) - 1] = 0;
	}
	pmCommand [strlen (pmCommand) - 1] = 0;
	myPath = &pmCommand [strlen (OPEN_COMMAND) + 1];
	EdWin_OpenFile (NULL, myPath, NULL, PURPOSE_LOAD_FILE);
    }
    else if ((_strnicmp (pmCommand, PRINT_COMMAND, strlen (PRINT_COMMAND)) == 0) 
    		&& (pmCommand [strlen (PRINT_COMMAND)] == '"'))  		 
    {
    	char	*myPath;
    	
    	// Remove the terminating quote
    	while (pmCommand [strlen (pmCommand) - 1] != '"')
    	{
	    pmCommand [strlen (pmCommand) - 1] = 0;
	}
	pmCommand [strlen (pmCommand) - 1] = 0;
	myPath = &pmCommand [strlen (OPEN_COMMAND) + 1];
	// TW Print file
	EdWin_OpenFile (NULL, myPath, NULL, PURPOSE_LOAD_FILE);
    }
    else
    {
    	EdGUI_Message1 (NULL, 0, IDS_BAD_DDE_COMMAND_TITLE, 
    	    IDS_BAD_DDE_COMMAND, pmCommand);
    }
} // MyExecuteDDECommand