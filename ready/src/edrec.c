/***********/
/* edrec.c */
/***********/

/*******************/
/* System includes */
/*******************/
#include <windows.h>
#include <crtdbg.h>

/****************/
/* Self include */
/****************/
#include "edrec.h"

/******************/
/* Other includes */
/******************/
#include "ed.h"
#include "edfile.h"
#include "edlog.h"
#include "edprop.h"

#include "edcommands.h"
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

/*********/
/* Types */
/*********/
typedef struct RecentFiles
{
    char		*pathName;
    struct RecentFiles	*next;
} RecentFiles, *RecentFilesPtr;


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
static EdRec_Properties	stProperties;
static BOOL		stPropertiesChanged;

// The head of the recent files linked list
static RecentFilesPtr	stRecentFilesHeader;

/******************************/
/* Static callback procedures */
/******************************/

/*********************/
/* Static procedures */
/*********************/
static void	MyAddFile (char *pmPathName, BOOL pmAddToTop);
static void	MyDeleteExcessFiles (void);
static void	MyReadRecentFiles (void);
static BOOL	MyReadWindowPosition (char *myPtr, int *x, int *y, int *width,
				      int *height);

/***********************/
/* External procedures */
/***********************/
/************************************************************************/
/* EdRec_Init								*/
/************************************************************************/
BOOL	EdRec_Init (void)
{
    stRecentFilesHeader = NULL;

    EdFile_SetActiveDirectory (EdProp_GetStartupDirectory (), DIRECTORY_PATH);
    
    if (!stProperties.noRecentFiles)
    {
    	// Read the recent files from the current directory
    	MyReadRecentFiles ();
    }
    
    return TRUE;
} // EdRec_Init


/************************************************************************/
/* EdRec_Finalize							*/
/************************************************************************/
void	EdRec_Finalize (void)
{
    RecentFilesPtr	myRecentFile, myRecentFileToBeDeleted;
    
    // Free up the memory blocks
    myRecentFile = stRecentFilesHeader;
    while (myRecentFile != NULL)
    {			    	
    	myRecentFileToBeDeleted = myRecentFile;
    	myRecentFile = myRecentFile -> next;
    	
    	free (myRecentFileToBeDeleted -> pathName);
    	free (myRecentFileToBeDeleted);
    } // while (for each recent file)
} // EdRec_Finalize


/************************************************************************/
/* EdRec_PropertiesSet							*/
/************************************************************************/
void	EdRec_PropertiesSet (EdRec_Properties pmProperties)
{
    stProperties = pmProperties;
    stPropertiesChanged = TRUE;
} // EdRec_PropertiesSet

	
/************************************************************************/
/* EdRec_PropertiesImplementChanges					*/
/************************************************************************/
void	EdRec_PropertiesImplementChanges (void)
{
    if (stPropertiesChanged)
    {
    	MyDeleteExcessFiles ();
        stPropertiesChanged = FALSE;
    }
} // EdRec_PropertiesImplementChanges


/************************************************************************/
/* EdRec_AddFile							*/
/************************************************************************/
void	EdRec_AddFile (const char *pmPathName, BOOL pmAddToTop)
{
    FilePath	myPathName;
    
    // Remove any trailing period
    strcpy (myPathName, pmPathName);
    if (myPathName [strlen (myPathName) - 1] == '.')
    {
    	myPathName [strlen (myPathName) - 1] = 0;
    }
    
    MyAddFile (myPathName, pmAddToTop);

    // Update all the "Recent Files" menus
    Ed_CreateRecentMenu ();
} // EdRec_AddFile


/************************************************************************/
/* EdRec_CreateRecentMenu						*/
/************************************************************************/
void	EdRec_CreateRecentMenu (HMENU pmRecentMenu)
{
    RecentFilesPtr	myRecentFile;
    int			myNumItems;
    int			myCommand;
    int			cnt;
    
    // Delete the current contents of the menu
    myNumItems = GetMenuItemCount (pmRecentMenu);
    for (cnt = 0 ; cnt < myNumItems ; cnt++)
    {
    	RemoveMenu (pmRecentMenu, 0, MF_BYPOSITION);
    }

    myCommand = COMMAND_RECENT_BASE;
	
    myRecentFile = stRecentFilesHeader;
    while (myRecentFile != NULL)
    {			    	
	// Add the item to the end
    	if (stProperties.useFullPathNames)
    	{
            AppendMenu (pmRecentMenu, MF_STRING, myCommand, 
            		myRecentFile -> pathName);
    	}
    	else
    	{
            AppendMenu (pmRecentMenu, MF_STRING, myCommand, 
	                EdFile_GetFileName (myRecentFile -> pathName));
    	}
    	myCommand++;
    	myRecentFile = myRecentFile -> next;
    } // while (for each recent file)
} // EdRec_CreateRecentMenu


/************************************************************************/
/* EdRec_GetPathName							*/
/************************************************************************/
const char	*EdRec_GetPathName (int pmNumber)
{
    RecentFilesPtr	myRecentFile;
    int			cnt;
            
    myRecentFile = stRecentFilesHeader;
    
    for (cnt = 0 ; cnt < pmNumber ; cnt++)
    {
    	if (myRecentFile == NULL)
    	{
    	    break;
    	}
    	myRecentFile = myRecentFile -> next;
    }
    
    if (myRecentFile == NULL)
    {
    	EdFail_Warn (IDS_RECENT_FILE_NOT_FOUND, __FILE__, __LINE__, 0, 
    	    pmNumber);
    }
    
    return myRecentFile -> pathName;
} // EdRec_GetPathName


/************************************************************************/
/* EdRec_RemoveFile							*/
/************************************************************************/
void	EdRec_RemoveFile (char *pmPathName)
{
    RecentFilesPtr	myRecentFile, myPrevRecentFile;
        
    // The files are stored in the linked list from most recent to least.
    
    // Check if the file is already on the list
    myRecentFile = stRecentFilesHeader;
    myPrevRecentFile = NULL;
    while (myRecentFile != NULL)
    {
    	if (strcmp (myRecentFile -> pathName, pmPathName) == 0)
    	{
    	    // The file is in the list.  Remove it
    	    if (myPrevRecentFile == NULL)
    	    {
    	    	stRecentFilesHeader = myRecentFile -> next;
    	    }
    	    else
    	    {
    	    	myPrevRecentFile -> next = myRecentFile -> next;
    	    }
    	    free (myRecentFile -> pathName);
    	    free (myRecentFile);
    	    
    	    // Update all the "Recent Files" menus
   	    Ed_CreateRecentMenu ();
   	    
    	    return;
    	}
    	myPrevRecentFile = myRecentFile;
    	myRecentFile = myRecentFile -> next;
    }
} // EdRec_RemoveFile


/************************************************************************/
/* EdRec_WriteRecentFiles						*/
/************************************************************************/
extern void	EdRec_WriteRecentFiles (void)
{
    FilePath		myRecentPathName;
    FileEntryPtr	myFile;
    int			myErrorCode;
    RecentFilesPtr	myRecentFile;
    	    
    if (stProperties.noRecentFiles)
    {
    	return;
    }

    if (stProperties.useMyDocuments)
    {
        EdFile_ConstructPath (myRecentPathName, FROM_APPDATA, RECENT_FILES);    
    }
    else
    {
        EdFile_ConstructPath (myRecentPathName, FROM_STARTUP, RECENT_FILES);    
    }
        
    // Try opening the recent files file in the startup directory
    myFile = EdFile_OpenFileForWrite (myRecentPathName, &myErrorCode, 
        BUFFER_OUTPUT, CREATE_NEW_FILE);
    if (myFile == NULL)
    {
    	// Write to log
    	EdLog_Log ("Unable to open recent files (\"%s\") for writing.  "
    		"Error = %d", myRecentPathName, myErrorCode);
    	return;
    }

    // Write to log
    EdLog_Log ("Writing recent files file to \"%s\"", myRecentPathName);

    // Write the last active directory
    EdFile_GetActiveDirectory (myRecentPathName);
    EdFile_Writeln (myFile, myRecentPathName);

    // Write all the recent files out to the file    
    myRecentFile = stRecentFilesHeader;
    while (myRecentFile != NULL)
    {			    	
    	EdFile_Writeln (myFile, myRecentFile -> pathName);
    	myRecentFile = myRecentFile -> next;
    } // while (for each recent file)
    EdFile_CloseFile (myFile);
} // EdRec_WriteRecentFiles


/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/
/************************************************************************/
/* MyAddFile								*/
/************************************************************************/
static void	MyAddFile (char *pmPathName, BOOL pmAddToTop)
{
    RecentFilesPtr	myRecentFile, myPrevRecentFile;
        
    // The files are stored in the linked list from most recent to least.
    
    // Check if the file is already on the list
    myRecentFile = stRecentFilesHeader;
    myPrevRecentFile = NULL;
    while (myRecentFile != NULL)
    {
    	if (strcmp (myRecentFile -> pathName, pmPathName) == 0)
    	{
    	    // The file is already in the list.  Rearrange the list to
    	    // place the file at the top,  Return if already there
    	    if (myPrevRecentFile == NULL)
    	    {
    	    	return;
    	    }
    	    myPrevRecentFile -> next = myRecentFile -> next;
    	    myRecentFile -> next = stRecentFilesHeader;
    	    stRecentFilesHeader = myRecentFile;
    	    
    	    // Update all the "Recent Files" menus
    	    Ed_CreateRecentMenu ();
    	    
    	    return;
    	}
    	myPrevRecentFile = myRecentFile;
    	myRecentFile = myRecentFile -> next;
    }
    
    // Create record for this file
    myRecentFile = (RecentFilesPtr) malloc (sizeof (RecentFiles));
    myRecentFile -> pathName = (char *) malloc (strlen (pmPathName) + 1);
    strcpy (myRecentFile -> pathName, pmPathName);
    
    if (pmAddToTop || (stRecentFilesHeader == NULL))
    {
    	// Add the file to the beginning of the list
    	myRecentFile -> next = stRecentFilesHeader;
    	stRecentFilesHeader = myRecentFile;
    }
    else
    {
    	// Add the file to the end of the list
    	RecentFilesPtr	myPtr;
    	
    	myRecentFile -> next = NULL;
    	myPtr = stRecentFilesHeader;
    	while (myPtr -> next != NULL)
    	{
    	    myPtr = myPtr -> next;
    	}
    	myPtr -> next = myRecentFile;
    }

    // Delete the end of the list, if necessary   
    MyDeleteExcessFiles ();
} // MyAddFile


/************************************************************************/
/* MyDeleteExcessFiles							*/
/************************************************************************/
static void	MyDeleteExcessFiles (void)
{
    RecentFilesPtr	myRecentFile, myRecentFileToBeDeleted;
    int			cnt;

    // Delete the end of the list, if necessary   
    myRecentFile = stRecentFilesHeader;  
    for (cnt = 0 ; (myRecentFile != NULL) && 
            (cnt < stProperties.numFilesToDisplay) ; cnt++)
    {
    	myRecentFile = myRecentFile -> next;
    }

    if (myRecentFile != NULL)
    {
    	// Cut the link to the rest of the recent files
    	myRecentFileToBeDeleted = myRecentFile -> next;
    	myRecentFile -> next = NULL;
    
    	// Free the rest of the recent files
    	while (myRecentFileToBeDeleted != NULL)
    	{
    	    free (myRecentFileToBeDeleted -> pathName);
    	    myRecentFile = myRecentFileToBeDeleted -> next;
    	    free (myRecentFileToBeDeleted);
    	    myRecentFileToBeDeleted = myRecentFile;
    	}
    }
} // MyDeleteExcessFiles


/************************************************************************/
/* MyReadRecentFiles							*/
/************************************************************************/
static void	MyReadRecentFiles (void)
{
    FilePath	myRecentPathName;
    char	*myLine;
    char	*myBuffer;
    DWORD	myFileSize;
    char	*myPtr;
    char	myDriveChar;
    	

    if (stProperties.useMyDocuments)
    {
        EdFile_ConstructPath (myRecentPathName, FROM_APPDATA, RECENT_FILES);    
    }
    else
    {
        EdFile_ConstructPath (myRecentPathName, FROM_STARTUP, RECENT_FILES);    
    }
    
    if (!EdFile_FileExists (myRecentPathName))
    {
    	return;
    }
    
    // Read file into buffer
    myBuffer = EdFile_ReadFileIntoBuffer (NULL, myRecentPathName, &myFileSize, 
    				          OK_IF_NOT_FOUND);
    if (myBuffer == NULL)
    {
    	return;
    }

    // The files are stored in order from most recent to least    
    EdFile_ReadLineFromBuffer (myBuffer, myFileSize, READ_FIRST);

    // Read the last active directory
    myLine = EdFile_ReadLineFromBuffer (myBuffer, 0, READ_NEXT);
    if (myLine != NULL)
    {
    	if (stProperties.useLastDirectory)
    	{
    	    myPtr = myLine;
    	    while (*myPtr == ' ')
    	    {
    	    	myPtr++;
    	    }
    	
    	    myDriveChar = tolower (*myPtr);
    	    if ((('a' <= myDriveChar) && (myDriveChar <= 'z') && 
    	         (myPtr [1] == ':') && (myPtr [2] == '\\')) ||
    	        ((*myPtr == '\\') && (myPtr [1] == '\\')))
    	    {
	    	// Add a character (because EdFile_SetActiveDirectory 
	    	// expects a file, not a directory
    	    	EdFile_SetActiveDirectory (myPtr, DIRECTORY_PATH);
    	    }
    	}
    	else
    	{
    	    EdFile_SetActiveDirectory (EdProp_GetStartupDirectory (), 
    	    	DIRECTORY_PATH);
    	}

        // Read a line from the file
	myLine = EdFile_ReadLineFromBuffer (myBuffer, 0, READ_NEXT);
    }

    while (myLine != NULL)
    {
    	myPtr = myLine;
    	while (*myPtr == ' ')
    	{
    	    myPtr++;
    	}
    	
    	// See if this refers to the Alloc position
    	if (strncmp (myPtr, "#alloc ", 7) == 0)
    	{
    	    int	x, y, width, height;
    	    
    	    myPtr += 7;
    	    
    	    // Window position: x, y, width, height
    	    if (MyReadWindowPosition (myPtr, &x, &y, &width, &height))
    	    {
    	    }
	
	}
	else
	{
    	    // Verify that pathname either starts with x:\ or \\ characters
    	    myDriveChar = tolower (*myPtr);
    	    if ((('a' <= myDriveChar) && (myDriveChar <= 'z') && 
    	         (myPtr [1] == ':') && (myPtr [2] == '\\')) ||
    	        ((*myPtr == '\\') && (myPtr [1] == '\\')))
    	    {
    	    	EdRec_AddFile (myLine, ADD_TO_BOTTOM);
    	    }
    	}
    	myLine = EdFile_ReadLineFromBuffer (myBuffer, 0, READ_NEXT);
    }
    
    free (myBuffer);
} // MyReadRecentFiles


/************************************************************************/
/* MyReadWindowPosition							*/
/************************************************************************/
static BOOL	MyReadWindowPosition (char *pmPtr, int *pmX, int *pmY, 
				      int *pmWidth, int *pmHeight)
{
    char	*myStart;
    
    // x
    myStart = pmPtr;
    while (isdigit (*pmPtr))
    {
    	pmPtr++;
    }
    *pmPtr++ = 0;
    if (!isdigit (*myStart))
    {
    	return FALSE;
    }
    *pmX = atol (myStart);
    
    // y
    myStart = pmPtr;
    while (isdigit (*pmPtr))
    {
    	pmPtr++;
    }
    *pmPtr++ = 0;
    if (!isdigit (*myStart))
    {
    	return FALSE;
    }
    *pmY = atol (myStart);
    
    // width
    myStart = pmPtr;
    while (isdigit (*pmPtr))
    {
    	pmPtr++;
    }
    *pmPtr++ = 0;
    if (!isdigit (*myStart))
    {
    	return FALSE;
    }
    *pmWidth = atol (myStart);
    
    // height
    myStart = pmPtr;
    while (isdigit (*pmPtr))
    {
    	pmPtr++;
    }
    *pmPtr++ = 0;
    if (!isdigit (*myStart))
    {
    	return FALSE;
    }
    *pmWidth = atol (myStart);
    
    return TRUE;
} // MyReadWindowPosition

				      
