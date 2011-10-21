/************************************************************************/
/* mdiodir.c 								*/
/************************************************************************/

/*******************/
/* System includes */
/*******************/
#include <windows.h>
#include <windowsx.h>

/****************/
/* Self include */
/****************/
#include "mdiodir.h"

/******************/
/* Other includes */
/******************/
#include "mio.h"

#include "mdio.h"

#include "mioerr.h"

#include "miofile.h"

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
typedef struct MDDirInfo
{
    HANDLE		handle;
    BOOL		dataAvailable;
    WIN32_FIND_DATA	data;
} MDDirInfo;

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
/* MDIODir_Create							*/
/************************************************************************/
void	MDIODir_Create (const char *pmDirectoryPath)
{
    UINT	myPrevErrorMode;
    BOOL	myResult;
    
    myPrevErrorMode = SetErrorMode (SEM_FAILCRITICALERRORS);
    myResult = CreateDirectory (pmDirectoryPath, NULL);
    SetErrorMode (myPrevErrorMode);
    
    if (!myResult)
    {
	MDIO_ProcessMSWindowsError (GetLastError ());
    }
} // MDIODir_Create


/************************************************************************/
/* MDIODir_Delete							*/
/************************************************************************/
void	MDIODir_Delete (const char *pmDirectoryPath)
{
    UINT	myPrevErrorMode;
    BOOL	myResult;
    
    myPrevErrorMode = SetErrorMode (SEM_FAILCRITICALERRORS);
    myResult = RemoveDirectory (pmDirectoryPath);
    SetErrorMode (myPrevErrorMode);

    if (!myResult)
    {
	MDIO_ProcessMSWindowsError (GetLastError ());
    }
} // MDIODir_Delete


/************************************************************************/
/* MDIODir_Exists							*/
/************************************************************************/
BOOL	MDIODir_Exists (char *pmDirectoryPath)
{
    UINT	myPrevErrorMode;
    DWORD	myWindowsAttributes;
    
    // Set the error mode so that missing disks fail normally
    // Load the first file in the directory
    myPrevErrorMode = SetErrorMode (SEM_FAILCRITICALERRORS);
    myWindowsAttributes = GetFileAttributes (pmDirectoryPath);
    SetErrorMode (myPrevErrorMode);
    
    if (myWindowsAttributes == (DWORD) -1)
    {
    	DWORD	myError = GetLastError ();
    	
    	if (myError == ERROR_FILE_NOT_FOUND)
    	{
	    SET_ERRMSG (E_FSYS_FILE_NOT_FOUND, "'%s' not found", 
			pmDirectoryPath);
    	}
    	else if (myError == ERROR_PATH_NOT_FOUND)
    	{
    	    SET_ERRMSG (E_FSYS_PATH_NOT_FOUND, 
    	 	        "Path not found (couldn't find the directory "
    	 	        "that '%s' is in)", pmDirectoryPath);
    	}
    	else
    	{
	    MDIO_ProcessMSWindowsError (myError);
	}
    	return FALSE;
    }
    
    if ((myWindowsAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
    {
	SET_ERRMSG (E_DIR_IS_A_FILE, "'%s' is a data file", pmDirectoryPath);
	return FALSE;
    }

    return TRUE;
} // MDIODir_Exists


/************************************************************************/
/* MDIODir_FreeDir							*/
/************************************************************************/
void	MDIODir_FreeDir (DIR pmMDDirInfo)
{
    MDDirInfo	*myInfo = (MDDirInfo *) pmMDDirInfo;

    if (myInfo -> handle != (HANDLE) 1)
    {
	FindClose (myInfo -> handle);
    }

    free (myInfo);
} // MDIODir_FreeDir


/************************************************************************/
/* MDIODir_Get								*/
/************************************************************************/
BOOL	MDIODir_Get (DIR pmMDDirInfo, OOTstring pmFileName,
		     int *pmFileSize, int *pmAttrib, int *pmFileTime)
{
    MDDirInfo	*myInfo = (MDDirInfo *) pmMDDirInfo;
    UINT	myPrevErrorMode;
    DWORD	myResult;
    DWORD	myWindowsAttributes;
    int		myOOTAttributes = 0;
    LONGLONG	myLongTime;

    if (myInfo -> dataAvailable)
    {
    	myInfo -> dataAvailable = FALSE;
    }
    else
    {
    	myPrevErrorMode = SetErrorMode (SEM_FAILCRITICALERRORS);
    	myResult = FindNextFile (myInfo -> handle, &myInfo -> data);
    	SetErrorMode (myPrevErrorMode);
    	
    	if (!myResult)
    	{
	    DWORD   myError = GetLastError ();

	    *pmFileName = 0;
	    *pmFileSize = -1;
	    *pmAttrib = -1;
	    *pmFileTime = -1;
	    
	    if (myError != ERROR_NO_MORE_FILES)
	    {
		MDIO_ProcessMSWindowsError (myError);
	    }
	    return FALSE;
	}
    }

    // Get the file name
    strcpy_s (pmFileName, 255, myInfo -> data.cFileName);

    // Get the file size
    *pmFileSize = myInfo -> data.nFileSizeLow;

    // Get the file attributes
    myWindowsAttributes = myInfo -> data.dwFileAttributes;
    myOOTAttributes |= OOT_ATTR_READ;
    if ((myWindowsAttributes & FILE_ATTRIBUTE_READONLY) == 0)
    {
        myOOTAttributes |= OOT_ATTR_WRITE;
    }
    if (myWindowsAttributes & FILE_ATTRIBUTE_HIDDEN)
    {
        myOOTAttributes |= OOT_ATTR_HIDDEN;
    }
    if (myWindowsAttributes & FILE_ATTRIBUTE_SYSTEM)
    {
        myOOTAttributes |= OOT_ATTR_SYSTEM;
    }
    if (myWindowsAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        myOOTAttributes |= OOT_ATTR_DIR;
    }
    if (myWindowsAttributes & FILE_ATTRIBUTE_ARCHIVE)
    {
        myOOTAttributes |= OOT_ATTR_ARCHIVE;
    }
    
    if ((myWindowsAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
    {
    	if ((_stricmp (pmFileName, ".exe") == 0) ||
    	    (_stricmp (pmFileName, ".com") == 0) ||
    	    (_stricmp (pmFileName, ".bat") == 0) ||
   	    (_stricmp (pmFileName, ".cmd") == 0))
	{
            myOOTAttributes |= OOT_ATTR_EXECUTE;
        }
    }
    *pmAttrib = myOOTAttributes;

    // Get the file time
    myLongTime = ((LONGLONG) myInfo -> data.ftLastWriteTime.dwHighDateTime) << 32;
    myLongTime += myInfo -> data.ftLastWriteTime.dwLowDateTime;
    myLongTime /= 10000000L;	// Convert to seconds
    myLongTime -= 11644473600;	// Convert from Jan 1, 1601 to Jan 1, 1970
    *pmFileTime = (unsigned long) myLongTime;

    return TRUE;
} // MDIODir_Get


/************************************************************************/
/* MDIODir_OpenDir							*/
/************************************************************************/
BOOL	MDIODir_OpenDir (char *pmDirectoryPath, DIR *pmMDDirInfo, 
			 BOOL *pmIsEmpty)
{
    MDDirInfo	*myInfo;
    UINT	myPrevErrorMode;
    HANDLE	myHandle;

    myInfo = (MDDirInfo *) malloc (sizeof (MDDirInfo));
    if (myInfo == NULL)
    {
	SET_ERRNO (E_DIR_CANT_ALLOC_MEM_FOR_DIR);
	return FALSE;
    }

    // If the file to be opened is a directory name, then we need to add the
    // "\*".  Otherwise we will get information about the directory, not
    // its contents!
    if (MDIODir_Exists (pmDirectoryPath))
    {
	strcat (pmDirectoryPath, "\\*");
    }
    else
    {
	// This is actually okay.  It may be that there is already a wild
	// card specified which is interpreted as a file name.
	SET_ERRNO (0);
    }

    // Set the error mode so that missing disks fail normally
    // Load the first file in the directory
    myPrevErrorMode = SetErrorMode (SEM_FAILCRITICALERRORS);
    myHandle = FindFirstFile (pmDirectoryPath, &myInfo -> data);
    SetErrorMode (myPrevErrorMode);
    
    if (myHandle == (HANDLE) INVALID_HANDLE_VALUE)
    {
    	// If the path is okay, but there are no file, don't give an error.
    	// Instead set emptyDirectory flag.
    	DWORD	myError = GetLastError ();
    	
    	if (myError != ERROR_FILE_NOT_FOUND)
    	{
	    MDIO_ProcessMSWindowsError (myError);
	    free (myInfo);
	    return FALSE;
	}

	myInfo -> handle = (HANDLE) 1;
	*pmIsEmpty = TRUE;
    }
    else
    {
	myInfo -> handle = myHandle;
	*pmIsEmpty = FALSE;
    }
    
    *pmMDDirInfo = myInfo;

    myInfo -> dataAvailable = TRUE;
    
    return TRUE;
} // MDIODir_OpenDir
