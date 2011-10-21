/***********/
/* edreg.c */
/***********/

/*******************/
/* System includes */
/*******************/

/****************/
/* Self include */
/****************/
#include "edreg.h"

/******************/
/* Other includes */
/******************/
#include "edfail.h"
#include "edfile.h"
#include "edglob.h"
#include "edgui.h"
#include "edprog.h"

#include "eddialogs.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
// Used to set file associations
#define READY_JAVA_FILE_ASSOC		"Ready.java"
#define READY_CLASS_FILE_ASSOC		"Ready.class"
#define TURING_T_FILE_ASSOC		"Turing.t"
#define TURING_TU_FILE_ASSOC		"Turing.tu"
#define TURING_TI_FILE_ASSOC		"Turing.ti"
#define TURING_TUR_FILE_ASSOC		"Turing.tur"
#define TURING_DEM_FILE_ASSOC		"Turing.dem"
#define JAVA_FILE_SUFFIX		".java"
#define CLASS_FILE_SUFFIX		".class"
#define TURING_FILE_SUFFIX		".t"
#define TURING_UNIT_FILE_SUFFIX		".tu"
#define TURING_INCLUDE_FILE_SUFFIX	".ti"
#define TURING_LONG_FILE_SUFFIX		".tur"
#define TURING_DEMO_FILE_SUFFIX		".dem"
#define PREVIOUS_FILE_ASSOC		"PreviousFileAssoc"
#define DDE_OPEN_STRING			"[Open(\"%1\")]"
#define DDE_PRINT_STRING		"[Print(\"%1\")]"
#define DDE_TOPIC			"system"
#define DDE_READY_NAME			"ReadyToProgram"
#define DDE_TURING_NAME			"Turing"
#define ALWAYS_SHOW_EXT			"AlwaysShowExt"
#define MAX_KEY_LENGTH			1024

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
static BOOL	MyCreateAssocEntry (HWND pmWindow, const char *pmAssocName, 
			const char *pmFileSuffix, int pmFileDescriptionRsrc,
			int pmIconNumber, int pmCommandNameRsrc,
			const char *pmDDEApplicationName);
static BOOL	MyCreateRegEntry (HWND pmWindow, const char *pmKeyName, 
				  const char *pmKeyValue);
static char	*MyGetFileAssocDescription (HWND pmWindow, const char *pmKey,
					    BOOL pmDebug);
static void	MyGetNameAndVersionNumber (char *pmPath, char **pmProductName,
					   char **pmProductVersion);
static BOOL	MyResetAssocEntry (HWND pmWindow, const char *pmAssocName, 
				   const char *pmFileSuffix);

/***********************/
/* External procedures */
/***********************/
/************************************************************************/
/* EdReg_ClearTree							*/
/************************************************************************/
BOOL	EdReg_ClearTree (HWND pmWindow, const char *pmKeyName)
{
    LONG 	myResult;
    ErrorBuffer	myErrorBuffer;
    HKEY	myKeyHandle;
    char	mySubKey [MAX_KEY_LENGTH];
    char	myNewKey [MAX_KEY_LENGTH];
    DWORD	mySubKeyLength;
        
    if ((pmKeyName == NULL) || (pmKeyName [0] == 0))
    {
    	EdGUI_Message1 (pmWindow, 0, IDS_REG_CLEAR_ERROR_TITLE, 
    			IDS_REG_BAD_NAME);
    	return FALSE;
    }
    
    // Open the key
    myResult = RegOpenKeyEx (HKEY_CLASSES_ROOT, pmKeyName, 0, 
    			     KEY_ENUMERATE_SUB_KEYS | DELETE, &myKeyHandle);
    if (myResult != ERROR_SUCCESS)
    {
    	if (myResult != ERROR_FILE_NOT_FOUND)
    	{
    	    EdGUI_Message1 (pmWindow, 0, IDS_REG_CLEAR_ERROR_TITLE, 
    	    	IDS_REG_BAD_OPEN, pmKeyName, 
    	    	EdFail_GetErrorMessage1 (myErrorBuffer, myResult), myResult);
    	}
       	return FALSE;
    }
        
    while (TRUE)
    {
    	mySubKeyLength = MAX_KEY_LENGTH;
    	myResult = RegEnumKeyEx (myKeyHandle, 0, mySubKey, &mySubKeyLength,
    				 NULL, NULL, NULL, NULL);
	// No more keys.  Delete the top item.
        if (myResult == ERROR_NO_MORE_ITEMS)
    	{
    	    myResult = RegDeleteKey (HKEY_CLASSES_ROOT, pmKeyName);
	    if (myResult != ERROR_SUCCESS)
	    {
	    	EdGUI_Message1 (pmWindow, 0, IDS_REG_CLEAR_ERROR_TITLE, 
	    	    IDS_REG_BAD_DELETE, pmKeyName, 
	    	    EdFail_GetErrorMessage1 (myErrorBuffer, myResult), 
	    	    myResult);
	       	return FALSE;
	    }
    	    break;
    	}
    	
    	// Check for other errors wrt to enumeration
    	if (myResult != ERROR_SUCCESS)
    	{
    	    EdGUI_Message1 (pmWindow, 0, IDS_REG_CLEAR_ERROR_TITLE, 
    	        IDS_REG_BAD_ENUM, pmKeyName, 
    	        EdFail_GetErrorMessage1 (myErrorBuffer, myResult), myResult);
       	    return FALSE;
    	}

	// Recursively delete subkeys
	wsprintf (myNewKey, "%s\\%s", pmKeyName, mySubKey);
    	if (!EdReg_ClearTree (pmWindow, myNewKey))
    	{
    	    return FALSE;
    	}
    } // while (TRUE)
    
    myResult = RegCloseKey (myKeyHandle);
    if (myResult != ERROR_SUCCESS)
    {
    	EdGUI_Message1 (pmWindow, 0, IDS_REG_CLEAR_ERROR_TITLE, 
    	    IDS_REG_BAD_CLOSE, pmKeyName, 
    	    EdFail_GetErrorMessage1 (myErrorBuffer, myResult), myResult);
       	return FALSE;
    }
    
    return TRUE;
} // EdReg_ClearTree


/************************************************************************/
/* EdReg_GetCurrentFileAssoc						*/
/************************************************************************/
char	*EdReg_GetCurrentFileAssoc (HWND pmWindow, BOOL pmDebug)
{
    char	*myFileSuffix;
    char	myAssoc [1024];
    
    if (pmDebug)
    {
    	EdFail_WriteToLog ("File Assoc Debug 1", "Display File Association");
    }
    
    if (gProgram.language == LANGUAGE_KIND_TURING)
    {
    	myFileSuffix = TURING_FILE_SUFFIX;
    }
    else if (gProgram.language == LANGUAGE_KIND_JAVA)
    {
    	myFileSuffix = JAVA_FILE_SUFFIX;
    }
    else
    {
    	return NULL;
    }
    
    if (pmDebug)
    {
    	EdFail_WriteToLog ("File Assoc Debug 2", myFileSuffix);
    }
    
    //
    // Get the value of the ".java" or ".t" key from registry (= myAssoc)
    //
    if (!EdReg_GetKey (pmWindow, myFileSuffix, "", myAssoc))
    {
    	return NULL;
    }
    if (myAssoc [0] == 0)
    {
    	return NULL;
    }

    if (pmDebug)
    {
    	EdFail_WriteToLog ("File Assoc Debug 3", myAssoc);
    }
    
    //
    // Check to see if we're already associated with Ready
    //
    if (((gProgram.language == LANGUAGE_KIND_TURING) && 
         (strcmp (myAssoc, TURING_T_FILE_ASSOC) == 0)) ||
        ((gProgram.language == LANGUAGE_KIND_JAVA) && 
         (strcmp (myAssoc, READY_JAVA_FILE_ASSOC) == 0)))
    {
    	if (pmDebug)
    	{
    	    EdFail_WriteToLog ("File Assoc Debug 4", "Associated with Ready");
    	}
    	
    	return ASSOC_WITH_READY;
    }
    
    return MyGetFileAssocDescription (pmWindow, myAssoc, pmDebug);
} // EdReg_GetCurrentFileAssoc


/************************************************************************/
/* EdReg_GetKey								*/
/*									*/
/* Sets pmValue to the value associated with the default subkey for the	*/
/* key passed in.  Assumes pmValue is a buffer of at least length 1024.	*/
/*									*/
/* if the key is not found, then EdReg_GetKey sets the value to the	*/
/* empty string and returns TRUE.					*/
/************************************************************************/
BOOL	EdReg_GetKey (HWND pmWindow, const char *pmKeyName, 
		      const char *pmValueName, char *pmValue)
{
    LONG 	myResult;
    DWORD	myValueType;
    DWORD	mySize;
    HKEY	myKeyHandle;
    ErrorBuffer	myErrorBuffer;
        
    myResult = RegOpenKeyEx (HKEY_CLASSES_ROOT, pmKeyName, 0, KEY_READ, 
    			     &myKeyHandle);
    if (myResult != ERROR_SUCCESS)
    {
    	if (myResult != ERROR_FILE_NOT_FOUND)
    	{
    	    EdGUI_Message1 (pmWindow, 0, IDS_REG_READ_ERROR_TITLE, 
    	    	IDS_REG_BAD_OPEN, pmKeyName, 
    	    	EdFail_GetErrorMessage1 (myErrorBuffer, myResult), myResult);
    	}
       	return FALSE;
    }
        
    mySize = 1024;
    myResult = RegQueryValueEx (myKeyHandle, pmValueName, 0, &myValueType, 
    				(BYTE *) pmValue, &mySize);
    if (myResult != ERROR_SUCCESS)
    {
    	if (myResult == ERROR_FILE_NOT_FOUND)
    	{
    	    pmValue [0] = 0;
    	}
    	else
    	{
    	    EdGUI_Message1 (pmWindow, 0, IDS_REG_READ_ERROR_TITLE, 
    	        IDS_REG_BAD_QUERY, pmKeyName, 
    	        EdFail_GetErrorMessage1 (myErrorBuffer, myResult), myResult);
       	    return FALSE;
    	}
    }
    
    myResult = RegCloseKey (myKeyHandle);
    if (myResult != ERROR_SUCCESS)
    {
    	EdGUI_Message1 (pmWindow, 0, IDS_REG_CLEAR_ERROR_TITLE, 
    	    IDS_REG_BAD_CLOSE, pmKeyName, 
    	    EdFail_GetErrorMessage1 (myErrorBuffer, myResult), myResult);
       	return FALSE;
    }
    
    return TRUE;
} // EdReg_GetKey


/************************************************************************/
/* EdReg_GetPreviousFileAssoc						*/
/*									*/
/* When we change a file association to Ready, we store all the		*/
/* previous file associations under the PREVIOUS_FILE_ASSOC key		*/
/* with the new Ready key.  (i.e. Previous association with .java	*/
/* is stored in Ready.java\\PreviousFileAssociation.			*/
/************************************************************************/
char	*EdReg_GetPreviousFileAssoc (HWND pmWindow, BOOL pmDebug)
{
    char	myFileAssocKey [1024], myAssoc [1024];
    
    if (pmDebug)
    {
    	EdFail_WriteToLog ("File Prev Assoc Debug 1", "Display File Association");
    }
    
    //
    // Check to see if we're already associated with Ready
    //
    if (gProgram.language == LANGUAGE_KIND_TURING)
    {
    	strcpy (myFileAssocKey, TURING_T_FILE_ASSOC);
    }
    else if (gProgram.language == LANGUAGE_KIND_JAVA)
    {
    	strcpy (myFileAssocKey, READY_JAVA_FILE_ASSOC);
    }
    else
    {
    	return NULL;
    }
    
    if (pmDebug)
    {
    	EdFail_WriteToLog ("File Prev Assoc Debug 2", myFileAssocKey);
    }
    
    //
    // If there's a key there, get the value
    //
    if (!EdReg_GetKey (pmWindow, myFileAssocKey, PREVIOUS_FILE_ASSOC, myAssoc))
    {
    	return NULL;
    }
    if (myAssoc [0] == 0)
    {
    	return NULL;
    }

    if (pmDebug)
    {
    	EdFail_WriteToLog ("File Prev Assoc Debug 3", myAssoc);
    }
    
    return MyGetFileAssocDescription (pmWindow, myAssoc, pmDebug);
} // EdReg_GetPreviousFileAssoc


/************************************************************************/
/* EdReg_KeyExists							*/
/************************************************************************/
BOOL	EdReg_KeyExists (HWND pmWindow, char *pmKeyName)
{
    LONG 	myResult;
    HKEY	myKeyHandle;
    ErrorBuffer	myErrorBuffer;
        
    myResult = RegOpenKeyEx (HKEY_CLASSES_ROOT, pmKeyName, 0, KEY_READ, 
    			     &myKeyHandle);
    if (myResult != ERROR_SUCCESS)
    {
    	if (myResult != ERROR_FILE_NOT_FOUND)
    	{
    	    EdGUI_Message1 (pmWindow, 0, IDS_REG_READ_ERROR_TITLE, 
    	        IDS_REG_BAD_OPEN, pmKeyName, 
    	        EdFail_GetErrorMessage1 (myErrorBuffer, myResult), myResult);
    	}
    	return FALSE;
    }
        
    myResult = RegCloseKey (myKeyHandle);
    if (myResult != ERROR_SUCCESS)
    {
    	EdGUI_Message1 (pmWindow, 0, IDS_REG_READ_ERROR_TITLE, 
    	    IDS_REG_BAD_CLOSE, pmKeyName, 
    	    EdFail_GetErrorMessage1 (myErrorBuffer, myResult), myResult);
       	return FALSE;
    }
    
    return TRUE;
} // EdReg_KeyExists


/************************************************************************/
/* EdReg_ResetFileAssoc							*/
/*									*/
/* Set the file associations to our application.			*/
/************************************************************************/
void	EdReg_ResetFileAssoc (HWND pmWindow)
{
    if (gProgram.language == LANGUAGE_KIND_TURING)
    {
    	if (!MyResetAssocEntry (pmWindow, TURING_T_FILE_ASSOC, 
    				TURING_FILE_SUFFIX))
    	{
	    return;
	}	    		
    	if (!MyResetAssocEntry (pmWindow, TURING_TU_FILE_ASSOC, 
    				TURING_UNIT_FILE_SUFFIX))
    	{
	    return;
	}	    		
    	if (!MyResetAssocEntry (pmWindow, TURING_TI_FILE_ASSOC, 
    				TURING_INCLUDE_FILE_SUFFIX))
    	{
	    return;
	}	    		
    	if (!MyResetAssocEntry (pmWindow, TURING_TUR_FILE_ASSOC, 
    				TURING_LONG_FILE_SUFFIX))
    	{
	    return;
	}	    		
    	if (!MyResetAssocEntry (pmWindow, TURING_DEM_FILE_ASSOC, 
    				TURING_LONG_FILE_SUFFIX))
    	{
	    return;
	}	    		
    }
    else if (gProgram.language == LANGUAGE_KIND_JAVA)
    {
    	if (!MyResetAssocEntry (pmWindow, READY_CLASS_FILE_ASSOC, 
    				CLASS_FILE_SUFFIX))
    	{
	    return;
	}	    		
    	if (!MyResetAssocEntry (pmWindow, READY_JAVA_FILE_ASSOC, 
    				JAVA_FILE_SUFFIX))
    	{
	    return;
	}	    		
    }
} // EdReg_ResetFileAssoc


/************************************************************************/
/* EdReg_SetFileAssoc							*/
/*									*/
/* Set the file associations to our application.			*/
/************************************************************************/
void	EdReg_SetFileAssoc (HWND pmWindow)
{
    if (gProgram.language == LANGUAGE_KIND_TURING)
    {
    	if (!MyCreateAssocEntry (pmWindow, 
    		TURING_T_FILE_ASSOC, TURING_FILE_SUFFIX,
    		IDS_TURING_DESCRIPTION, TURING_ICON,
	    	IDS_TURING_COMMAND_NAME, DDE_TURING_NAME))
	{
	    return;
	}	    		
	if (!MyCreateAssocEntry (pmWindow, 
    		TURING_TU_FILE_ASSOC, TURING_UNIT_FILE_SUFFIX,
	    	IDS_TURING_UNIT_DESCRIPTION, TURING_UNIT_ICON,
	    	IDS_TURING_COMMAND_NAME, DDE_TURING_NAME))
	{
	    return;
	}	    		
    	if (!MyCreateAssocEntry (pmWindow, 
    		TURING_TI_FILE_ASSOC, TURING_INCLUDE_FILE_SUFFIX,
    		IDS_TURING_INCLUDE_DESCRIPTION, TURING_INCLUDE_ICON,
    		IDS_TURING_COMMAND_NAME, DDE_TURING_NAME))
	{
	    return;
	}	    		
    	if (!MyCreateAssocEntry (pmWindow, 
    		TURING_TUR_FILE_ASSOC, TURING_LONG_FILE_SUFFIX,
    		IDS_TURING_DESCRIPTION, TURING_ICON,
    		IDS_TURING_COMMAND_NAME, DDE_TURING_NAME))
	{
	    return;
	}	    		
    	if (!MyCreateAssocEntry (pmWindow, 
    		TURING_DEM_FILE_ASSOC, TURING_LONG_FILE_SUFFIX,
    		IDS_TURING_UNIT_DESCRIPTION, TURING_DEMO_ICON,
    		IDS_TURING_COMMAND_NAME, DDE_TURING_NAME))
	{
	    return;
	}	    		
    }
    else if (gProgram.language == LANGUAGE_KIND_JAVA)
    {
    	if (!MyCreateAssocEntry (pmWindow, 
    		READY_CLASS_FILE_ASSOC, CLASS_FILE_SUFFIX,
    		IDS_CLASS_DESCRIPTION, CLASS_ICON,
	    	IDS_JAVA_COMMAND_NAME, DDE_READY_NAME))
	{
	    return;
	}	    		
	if (!MyCreateAssocEntry (pmWindow, 
    		READY_JAVA_FILE_ASSOC, JAVA_FILE_SUFFIX,
	    	IDS_JAVA_DESCRIPTION, JAVA_ICON,
	    	IDS_JAVA_COMMAND_NAME, DDE_READY_NAME))
	{
	    return;
	}	    		
    }
} // EdReg_SetFileAssoc


/******************************/
/* Static callback procedures */
/******************************/
/*********************/
/* Static procedures */
/*********************/
/************************************************************************/
/* MyCreateAssocEntry							*/
/************************************************************************/
static BOOL	MyCreateAssocEntry (HWND pmWindow, const char *pmAssocName, 
			const char *pmFileSuffix, int pmFileDescriptionRsrc,
			int pmIconNumber, int pmCommandNameRsrc,
			const char *pmDDEApplicationName)
{
    char	myCurrentAssociation [1024];
    HKEY	myKeyHandle;
    char	myString [1024];
    char	myKeyName [1024];
    FilePath	myApplicationPath;
    char	myIconNumber [256];
    ErrorBuffer	myErrorBuffer;
    int		myResult;
    
    //
    // Get the association for the file suffix
    //
    if (!EdReg_GetKey (pmWindow, pmFileSuffix, "", myCurrentAssociation))
    {
    	myCurrentAssociation [0] = 0;
    }
    if (_stricmp (myCurrentAssociation, pmAssocName) == 0)
    {
    	myCurrentAssociation [0] = 0;
    }
    
    //
    // If there is current association, then check to see if there's already 
    // a previous registration, which we want to keep.  This means if we
    // set the file association to Ready when it's already set to Ready, 
    // we don't lose the previous file association
    //
    if (myCurrentAssociation [0] == 0)
    {
    	if (!EdReg_GetKey (pmWindow, pmAssocName, PREVIOUS_FILE_ASSOC,
    			   myCurrentAssociation))
	{
    	    myCurrentAssociation [0] = 0;
	}
    }
    
    //
    // Clear the previous tree
    //
    EdReg_ClearTree (pmWindow, pmAssocName);

    //
    // Create Ready.java and set to "Java Source File"
    //
    EdGUI_LoadString (pmFileDescriptionRsrc, myString, sizeof (myString));
    if (!MyCreateRegEntry (pmWindow, pmAssocName, myString))
    {
    	return FALSE;
    }

    //
    // Create Ready.java\DefaultIcon and set to "[..]\apps\Ready\Ready.exe,-6"
    //
    wsprintf (myKeyName, "%s\\DefaultIcon", pmAssocName);
    GetModuleFileName (NULL, myApplicationPath, PATHNAME_LENGTH);
    wsprintf (myIconNumber, ",-%d", pmIconNumber);
    strcat (myApplicationPath, myIconNumber);
    if (!MyCreateRegEntry (pmWindow, myKeyName, myApplicationPath))
    {
    	return FALSE;
    }

    //
    // Create Ready.java\shell
    //
    wsprintf (myKeyName, "%s\\shell", pmAssocName);
    if (!MyCreateRegEntry (pmWindow, myKeyName, NULL))
    {
    	return FALSE;
    }
    
    //
    // Create Ready.java\shell\open and set to "&Open with Ready to Program"
    //
    wsprintf (myKeyName, "%s\\shell\\open", pmAssocName);
    EdGUI_LoadString (pmCommandNameRsrc, myString, sizeof (myString));
    if (!MyCreateRegEntry (pmWindow, myKeyName, myString))
    {
    	return FALSE;
    }
    
    //
    // Was: Create Ready.java\shell\open\command and set to ""[..]\apps\Ready\Ready.exe" "%1""
    // Now: Create Ready.java\shell\open\command and set to ""[..]\apps\Ready\Ready.exe" -nowindow"
    //
    wsprintf (myKeyName, "%s\\shell\\open\\command", pmAssocName);
    myApplicationPath [0] = '"';    	
    GetModuleFileName (NULL, &myApplicationPath [1], PATHNAME_LENGTH);
//    strcat (myApplicationPath, "\" \"%1\"");
    strcat (myApplicationPath, "\" -nowindow");
    if (!MyCreateRegEntry (pmWindow, myKeyName, myApplicationPath))
    {
    	return FALSE;
    }

    //
    // Create Ready.java\shell\open\ddeexec and set to "[Open("%1")]"
    //
    wsprintf (myKeyName, "%s\\shell\\open\\ddeexec", pmAssocName);
    if (!MyCreateRegEntry (pmWindow, myKeyName, DDE_OPEN_STRING))
    {
    	return FALSE;
    }

    //
    // Create Ready.java\shell\open\ddeexec\application to "ReadyToProgram"
    //
    wsprintf (myKeyName, "%s\\shell\\open\\ddeexec\\application", pmAssocName);
    if (!MyCreateRegEntry (pmWindow, myKeyName, pmDDEApplicationName))
    {
    	return FALSE;
    }

    //
    // Create Ready.java\shell\open\ddeexec\application to "system"
    //
    wsprintf (myKeyName, "%s\\shell\\open\\ddeexec\\topic", pmAssocName);
    if (!MyCreateRegEntry (pmWindow, myKeyName, DDE_TOPIC))
    {
    	return FALSE;
    }

    //    
    // Add in some extra values for the main key
    //
    // Open key
    //
    myResult = RegOpenKeyEx (HKEY_CLASSES_ROOT, pmAssocName, 0, KEY_ALL_ACCESS,
    			     &myKeyHandle);
    if (myResult != ERROR_SUCCESS)
    {
    	EdGUI_Message1 (pmWindow, 0, IDS_REG_CREATE_ERROR_TITLE, 
    	    IDS_REG_BAD_CREATE, pmAssocName, 
    	    EdFail_GetErrorMessage1 (myErrorBuffer, myResult), myResult);
    	return FALSE;
    }

    //
    // Write always display extension
    //
    myResult = RegSetValueEx (myKeyHandle, ALWAYS_SHOW_EXT, 0, REG_SZ, "", 0);
    if (myResult != ERROR_SUCCESS)
    {
    	EdGUI_Message1 (pmWindow, 0, IDS_REG_CREATE_ERROR_TITLE, 
    	    IDS_REG_BAD_SET_VALUE, pmAssocName, 
    	    EdFail_GetErrorMessage1 (myErrorBuffer, myResult), myResult);
    	return FALSE;
    }
    
    //
    // Write previous association
    //
    if (myCurrentAssociation [0] != 0)
    {
    	myResult = RegSetValueEx (myKeyHandle, PREVIOUS_FILE_ASSOC, 0, REG_SZ, 
    			myCurrentAssociation, strlen (myCurrentAssociation));
    	if (myResult != ERROR_SUCCESS)
    	{
    	    EdGUI_Message1 (pmWindow, 0, IDS_REG_CREATE_ERROR_TITLE, 
    	        IDS_REG_BAD_SET_VALUE, pmAssocName, 
    	        EdFail_GetErrorMessage1 (myErrorBuffer, myResult), myResult);
    	    return FALSE;
    	}
    }
    
    //
    // Close key
    //
    myResult = RegCloseKey (myKeyHandle);    				   
    if (myResult != ERROR_SUCCESS)
    {
    	EdGUI_Message1 (pmWindow, 0, IDS_REG_CREATE_ERROR_TITLE, 
    	    IDS_REG_BAD_CLOSE, pmAssocName, 
    	    EdFail_GetErrorMessage1 (myErrorBuffer, myResult), myResult);
    	return FALSE;
    }

    //
    // Create link from file suffix (Set .java to Ready.java)
    //
    if (!MyCreateRegEntry (pmWindow, pmFileSuffix, pmAssocName))
    {
    	return FALSE;
    }
        
    return TRUE;
} // MyCreateAssocEntry


/************************************************************************/
/* MyCreateRegEntry							*/
/************************************************************************/
static BOOL	MyCreateRegEntry (HWND pmWindow, const char *pmKeyName, 
				  const char *pmKeyValue)
{
    HKEY	myKeyHandle;
    DWORD	myDisposition;
    ErrorBuffer	myErrorBuffer;
    int		myResult;
        
    //
    // Create key
    //
    myResult = RegCreateKeyEx (HKEY_CLASSES_ROOT, pmKeyName, 0, "", 
    			       REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
    			       NULL, &myKeyHandle, &myDisposition);
    if (myResult != ERROR_SUCCESS)
    {
    	EdGUI_Message1 (pmWindow, 0, IDS_REG_CREATE_ERROR_TITLE, 
    	    IDS_REG_BAD_CREATE, pmKeyName, 
    	    EdFail_GetErrorMessage1 (myErrorBuffer, myResult), myResult);
    	return FALSE;
    }

    //
    // If there's a value, set it.
    //
    if (pmKeyValue != NULL)
    {
        myResult = RegSetValueEx (myKeyHandle, NULL, 0, REG_SZ, 
    			          pmKeyValue, strlen (pmKeyValue));
        if (myResult != ERROR_SUCCESS)
    	{
    	    EdGUI_Message1 (pmWindow, 0, IDS_REG_CREATE_ERROR_TITLE, 
    	        IDS_REG_BAD_SET_VALUE, pmKeyName, 
    	        EdFail_GetErrorMessage1 (myErrorBuffer, myResult), myResult);
    	    return FALSE;
    	}
    }

    //
    // Close key
    //
    myResult = RegCloseKey (myKeyHandle);    				   
    if (myResult != ERROR_SUCCESS)
    {
    	EdGUI_Message1 (pmWindow, 0, IDS_REG_CREATE_ERROR_TITLE, 
    	    IDS_REG_BAD_CLOSE, pmKeyName, 
    	    EdFail_GetErrorMessage1 (myErrorBuffer, myResult), myResult);
    	return FALSE;
    }

    return TRUE;
} // MyCreateRegEntry


/************************************************************************/
/* MyGetFileAssocDescription						*/
/************************************************************************/
static char	*MyGetFileAssocDescription (HWND pmWindow, const char *pmKey,
					    BOOL pmDebug)
{
    char	myAssoc [4096];
    char	myVerb [1024], myOpenAssoc [1024], myCommand [1024];
    char	*myName, *myVersion;
    FilePath	myPath;
    int		myResult;
        
    //
    // Read the value of the myAssoc\\shell key from registry.
    //
    wsprintf (myAssoc, "%s\\shell", pmKey);

    if (pmDebug)
    {
    	EdFail_WriteToLog ("File Assoc Debug 5", myAssoc);
    }

    if (!EdReg_GetKey (pmWindow, myAssoc, "", myVerb))
    {
    	return NULL;
    }
    
    if (pmDebug)
    {
    	EdFail_WriteToLog ("File Assoc Debug 6", myVerb);
    }

    //
    // If shell has a value, use the default verb, otherwise use open    
    //
    if (myVerb [0] == 0)
    {
        wsprintf (myOpenAssoc, "%s\\open\\command", myAssoc);
    
    	if (pmDebug)
    	{
    	    EdFail_WriteToLog ("File Assoc Debug 7", myOpenAssoc);
    	}

    	if (!EdReg_KeyExists (pmWindow, myOpenAssoc))
    	{
    	    DWORD	mySize;
    	    FILETIME	myFTtime;
	    HKEY	myKeyHandle;

    	    //
    	    // Look for first verb
    	    //
	    RegOpenKeyEx (HKEY_CLASSES_ROOT, myAssoc, 0, KEY_READ, &myKeyHandle);
    
	    mySize = sizeof (myVerb);
    	    myResult = RegEnumKeyEx (myKeyHandle, 0, myVerb, &mySize, NULL, 
    	    			     NULL, NULL, &myFTtime);
    	    
    	    if (myResult != ERROR_SUCCESS)
    	    {
    	    	return NULL;
    	    }
    
	    RegCloseKey (myKeyHandle);

	    wsprintf (myOpenAssoc, "%s\\%s\\command", myAssoc, myVerb);
            
    	    if (pmDebug)
    	    {
    	    	EdFail_WriteToLog ("File Assoc Debug 7a", myOpenAssoc);
    	    }
    	}

    	if (pmDebug)
    	{
    	    EdFail_WriteToLog ("File Assoc Debug 7b", myOpenAssoc);
    	}
    }
    else
    {
	wsprintf (myOpenAssoc, "%s\\%s\\command", myAssoc, myVerb);

    	if (pmDebug)
    	{
    	    EdFail_WriteToLog ("File Assoc Debug 8", myOpenAssoc);
    	}
    } // if
    
    if (!EdReg_GetKey (pmWindow, myOpenAssoc, "", myCommand))
    {
    	return NULL;
    }
    
    if (pmDebug)
    {
    	EdFail_WriteToLog ("File Assoc Debug 9", myCommand);
    }

    if (myCommand [0] == 0)
    {
    	return NULL;
    }
    if (myCommand [0] == '"')
    {
    	char	*mySrc = &myCommand [1];
    	char	*myDest = myPath;
    	
    	while (*mySrc != '"')
    	{
    	    *myDest++ = *mySrc++;
    	}
    	*myDest = 0;
    }
    else
    {
    	char	*mySrc = myCommand;
    	char	*myDest = myPath;
    	
    	while (*mySrc != ' ')
    	{
    	    *myDest++ = *mySrc++;
    	}
    	*myDest = 0;
    }

    if (pmDebug)
    {
    	EdFail_WriteToLog ("File Assoc Debug 10", myPath);
    }
    
    MyGetNameAndVersionNumber (myPath, &myName, &myVersion);

    if (pmDebug)
    {
    	EdFail_WriteToLog ("File Assoc Debug 11", myName);
    }
    
    if (pmDebug)
    {
    	EdFail_WriteToLog ("File Assoc Debug 12", myVersion);
    }
    
    if (myName == NULL)
    {
	if (strrchr (myPath, '\\') != NULL)
	{
	    if (EdGlob_EndsWith (myPath, ".exe") || 
	        EdGlob_EndsWith (myPath, ".EXE"))
	    {
	    	myPath [strlen (myPath) - 4] = 0;
	    }

	    return EdGlob_MallocString (strrchr (myPath, '\\') + 1);
	}
	else
	{
	    return EdGlob_MallocString (myPath);
	}
    }
    else
    {
    	if (myVersion != NULL)
    	{
    	    char	*myStr = malloc (strlen (myName) + 
    	    				 strlen (myVersion) + 3);
    	    wsprintf (myStr, "%s v%s", myName, myVersion);
    	    free (myName);
    	    free (myVersion);
    	    
    	    if (pmDebug)
    	    {
    		EdFail_WriteToLog ("File Assoc Debug 13", myStr);
	    }
    
    	    return myStr;
    	}
	else
	{
	    return (myName);
	}
    }
} // MyGetFileAssocDescription


/************************************************************************/
/* MyGetNameAndVersionNumber						*/
/************************************************************************/
static void	MyGetNameAndVersionNumber (char *pmPath, char **pmProductName,
					   char **pmProductVersion)
{
    DWORD	myDummy;
    DWORD	myVersionSize;
    char	*myVersionData;
    char	*myProductName, *myProductVersion;
    UINT	myProductNameSize, myProductVersionSize;
    
    myVersionSize = GetFileVersionInfoSize (pmPath, &myDummy);
    if (myVersionSize == 0)
    {
	*pmProductName = NULL;
    	return;
    }
    
    myVersionData = malloc (myVersionSize);
    if (myVersionData == NULL)
    {
	*pmProductName = NULL;
    	return;
    }
    
    if (!GetFileVersionInfo (pmPath, myDummy, myVersionSize, 
        myVersionData))
    {
	*pmProductName = NULL;
    	free (myVersionData);
    	return;
    }
    	
    if (!VerQueryValue (myVersionData, 
        "\\StringFileInfo\\04090000\\FileDescription", &myProductName, 
        &myProductNameSize))
    {
    	if (!VerQueryValue (myVersionData, 
            "\\StringFileInfo\\040904e4\\FileDescription", &myProductName, 
            &myProductNameSize))
	{            
	    if (!VerQueryValue (myVersionData, 
	        "\\StringFileInfo\\040904b0\\FileDescription", &myProductName, 
	        &myProductNameSize))
	    {
		*pmProductName = NULL;
    	        free (myVersionData);
    	        return;
    	    }
    	}
    }

    *pmProductName = malloc (myProductNameSize + 1);
    if (*pmProductName == NULL)
    {
    	free (myVersionData);
    	return;
    }

    strncpy (*pmProductName, myProductName, myProductNameSize);
        
    if (!VerQueryValue (myVersionData, 
        "\\StringFileInfo\\04090000\\FileVersion", &myProductVersion, 
        &myProductVersionSize))
    {
    	if (!VerQueryValue (myVersionData, 
            "\\StringFileInfo\\040904e4\\FileVersion", &myProductVersion, 
            &myProductVersionSize))
        {
    	    if (!VerQueryValue (myVersionData, 
            	"\\StringFileInfo\\040904b0\\FileVersion", &myProductVersion, 
            	&myProductVersionSize))
            {
    	    	free (myVersionData);
    	    	return;
    	    }
    	}
    }

    *pmProductVersion = malloc (myProductVersionSize + 1);
    if (*pmProductVersion == NULL)
    {
    	free (myVersionData);
    	return;
    }
    
    strncpy (*pmProductVersion, myProductVersion, myProductVersionSize);

    free (myVersionData);
} // MyGetNameAndVersionNumber



/************************************************************************/
/* MyResetFileAssociation						*/
/************************************************************************/
static BOOL	MyResetAssocEntry (HWND pmWindow, const char *pmAssocName, 
				   const char *pmFileSuffix)
{
    char	myPrevAssociation [1024];
    
    //
    // Get the previous association for the file suffix
    //
    if ((!EdReg_GetKey (pmWindow, pmAssocName, PREVIOUS_FILE_ASSOC, 
    		        myPrevAssociation)) ||
        (myPrevAssociation [0] == 0))
    {
    	// If file suffix is associated with us and there are no previous
    	// file associations, then delete it.
    	if (EdReg_GetKey (pmWindow, pmFileSuffix, "", myPrevAssociation))
    	{
	    if (strcmp (myPrevAssociation, pmAssocName) == 0)
	    {
		EdReg_ClearTree (pmWindow, pmFileSuffix);
	    }
	}

    	// Clear the Ready tree
    	EdReg_ClearTree (pmWindow, pmAssocName);
    	
    	return TRUE;
    }
    	
    //
    // Create link from file suffix (Set .java to Ready.java)
    //
    if (!MyCreateRegEntry (pmWindow, pmFileSuffix, myPrevAssociation))
    {
    	return FALSE;
    }
        
    //
    // Clear the previous tree
    //
    EdReg_ClearTree (pmWindow, pmAssocName);
    
    return TRUE;
} // MyResetFileAssociation

