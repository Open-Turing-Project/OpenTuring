/************************************************************************/
/* mdiofile.c								*/
/************************************************************************/

/*******************/
/* System includes */
/*******************/
#include <windows.h>
#include <string.h>

/****************/
/* Self include */
/****************/
#include "mdiofile.h"

#include "mioerr.h"

#include "mio.h"

#include "mdio.h"

#include "miofile.h"

#include "edint.h"

#include "miodialogs.h"

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
/* MDIOFile_ConvertFilePathToTuringPath					*/
/*									*/
/* Simply convert the forward slashes to backslashes.			*/
/************************************************************************/
/*
void	MDIOFile_ConvertFilePathToTuringPath (const char *pmMDPathName, 
					      char *pmTuringPath)
{
    char	*myPtr = pmTuringPath;
    
    strcpy (pmTuringPath, pmMDPathName);
    while (*myPtr != 0)
    {
    	if (*myPtr == '\\')
    	{
    	    *myPtr = '/';
    	}
    	myPtr++;
    }
} // MDIOFile_ConvertFilePathToTuringPath
*/
					      
/************************************************************************/
/* MDIOFile_Copy							*/
/************************************************************************/
void	MDIOFile_Copy (const char *pmSrcPath, const char *pmDestPath)
{
    UINT	myPrevErrorMode;

    myPrevErrorMode = SetErrorMode (SEM_FAILCRITICALERRORS);

    if (!CopyFile (pmSrcPath, pmDestPath, FALSE))
    {
	MDIO_ProcessMSWindowsError (GetLastError ());
    }

    SetErrorMode (myPrevErrorMode);
} // MDIOFile_Copy


/************************************************************************/
/* MDIOFile_Delete							*/
/************************************************************************/
void	MDIOFile_Delete (const char *pmPathName)
{
    UINT	myPrevErrorMode;

    myPrevErrorMode = SetErrorMode (SEM_FAILCRITICALERRORS);

    if (!DeleteFile (pmPathName))
    {
	MDIO_ProcessMSWindowsError (GetLastError ());
    }

    SetErrorMode (myPrevErrorMode);
} // MDIOFile_Delete


/************************************************************************/
/* MDIOFile_DiskFree							*/
/************************************************************************/
int	MDIOFile_DiskFree (const char *pmPathName)
{
    char	myRootDir [1024];
    FARPROC	myGetDiskFreeSpaceExProc;
    BOOL	myResult;
    const char	*myPtr;
    UINT	myPrevErrorMode;

    myGetDiskFreeSpaceExProc = GetProcAddress (GetModuleHandle("kernel32.dll"),
					       "GetDiskFreeSpaceExA");
    if (myGetDiskFreeSpaceExProc == NULL)
    {
	DWORD	mySectPerClust, myBytesPerSect, myFreeClusters, myTotalClusters;

	// If GetDiskFreeSpaceEx is not available, we must get the root
	// directory of the current drive and pass it to GetDiskFreeSpace.
	MIOFile_GetRootDir (pmPathName, &myPtr);
	strncpy (myRootDir, pmPathName, myPtr - pmPathName);
	myRootDir [myPtr - pmPathName] = 0;

	// Windows 95 cannot look up disk free on a network share.
	if ((myRootDir [0] == '\\') && (myRootDir [1] == '\\'))
	{
	    OSVERSIONINFO	myOSVersionInfo;

	    ZeroMemory (&myOSVersionInfo, sizeof (myOSVersionInfo));
	    myOSVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	    GetVersionEx (&myOSVersionInfo);
	    
	    // You cannot use CreateFile on a directory under Win 95/98/Me
	    if ((myOSVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) &&
		(myOSVersionInfo.dwMajorVersion == 4) &&
		(myOSVersionInfo.dwMinorVersion == 0))
	    {
	        return -1;
	    }
	}

    	myPrevErrorMode = SetErrorMode (SEM_FAILCRITICALERRORS);
	myResult = GetDiskFreeSpace (myRootDir, &mySectPerClust, 
				     &myBytesPerSect, &myFreeClusters, 
				     &myTotalClusters);
    	SetErrorMode (myPrevErrorMode);
	if (myResult)
	{
	    return (mySectPerClust * myBytesPerSect * myFreeClusters);
	}
    }
    else
    {
	ULARGE_INTEGER	myFreeBytesToCaller, myTotalBytes, myFreeBytes;
	ULONGLONG	myLongFree;

    	myPrevErrorMode = SetErrorMode (SEM_FAILCRITICALERRORS);
        myResult = myGetDiskFreeSpaceExProc (pmPathName,  &myFreeBytesToCaller,
					     &myTotalBytes, &myFreeBytes);
    	SetErrorMode (myPrevErrorMode);
	if (myResult)
	{
	    myLongFree = myFreeBytesToCaller.QuadPart;
	    if (myLongFree > (ULONGLONG) 0x7FFFFFFF)
	    {
		return (0x7FFFFFFF);
	    }
	    else
	    {
		return ((int) myLongFree);
	    }
	}
    }

    // An error occurred - process it.
    MDIO_ProcessMSWindowsError (GetLastError ());
    return -1;
} // MDIOFile_DiskFree


/************************************************************************/
/* MDIOFile_Exists							*/
/************************************************************************/
BOOL	MDIOFile_Exists (const char *pmPathName, BOOL pmDataFileOnly)
{
    UINT	myPrevErrorMode;
    DWORD	myWindowsAttributes;

    // Set the error mode so that missing disks fail normally
    // Load the first file in the directory
    myPrevErrorMode = SetErrorMode (SEM_FAILCRITICALERRORS);
    myWindowsAttributes = GetFileAttributes (pmPathName);
    SetErrorMode (myPrevErrorMode);
    
    if (myWindowsAttributes == (DWORD) -1)
    {
    	DWORD	myError = GetLastError ();
    	
    	if (myError == ERROR_FILE_NOT_FOUND)
    	{
	    SET_ERRMSG (E_FSYS_FILE_NOT_FOUND, "'%s' not found", 
			pmPathName);
    	}
    	else if (myError == ERROR_PATH_NOT_FOUND)
    	{
    	    SET_ERRMSG (E_FSYS_PATH_NOT_FOUND, 
    	 	        "Path not found (couldn't find the directory "
    	 	        "that '%s' is in)", pmPathName);
    	}
    	else
    	{
	    MDIO_ProcessMSWindowsError (myError);
	}
    	return FALSE;
    }
    
    if (pmDataFileOnly && 
	((myWindowsAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0))
    {
	SET_ERRMSG (E_DIR_IS_A_FILE, "'%s' is a directory", pmPathName);
	return FALSE;
    }

    return TRUE;
} // MDIOFile_Exists


/************************************************************************/
/* MDIOFile_GetDefaultPathForWindowsVolume				*/
/************************************************************************/
BOOL	MDIOFile_GetDefaultPathForWindowsVolume (char pmDriveLetter, 
						 char *pmPath)
{
    char	myDrive [4], *myPathPart;
    UINT	myPrevErrorMode;
    int		myReturnCode;
        
    myDrive [0] = pmDriveLetter;
    myDrive [1] = ':';
    myDrive [2] = '\\';
    myDrive [3] = '\0';

    myPrevErrorMode = SetErrorMode (SEM_FAILCRITICALERRORS);
    myReturnCode = GetDriveType (myDrive);
    SetErrorMode (myPrevErrorMode);
    
    if (myReturnCode == DRIVE_UNKNOWN)
    {
	SET_ERRMSG (E_FSYS_DISK_DRIVE_INVALID, "'%c' is not a valid drive", 
		    pmDriveLetter);
    	return FALSE;
    }
    if (myReturnCode == DRIVE_NO_ROOT_DIR)
    {
	SET_ERRMSG (E_FSYS_DISK_DRIVE_INVALID, "No disk in '%c'", 
		    pmDriveLetter);
    	return FALSE;
    }
    
    myDrive [2] = '.';
    
    myReturnCode = GetFullPathName (myDrive, 1023, pmPath, &myPathPart);
    
    if (myReturnCode == 0)
    {
	SET_ERRMSG (E_FSYS_DISK_DRIVE_INVALID, 
		    "Unable to get default directory of '%c'", 
		    pmDriveLetter);
    	return FALSE;
    }
    
    return TRUE;
} // MDIOFile_GetDefaultPathForWindowsVolume

						 
/************************************************************************/
/* MDIOFile_Rename							*/
/************************************************************************/
void	MDIOFile_Rename (const char *pmPathName, const char *pmNewPathName)
{
    UINT	myPrevErrorMode;

    myPrevErrorMode = SetErrorMode (SEM_FAILCRITICALERRORS);

    // These are complete paths.  Make certain they're on the same
    // volume.  Otherwise MoveFile fails.
    if ((pmPathName [0] == pmNewPathName [0]) && 
	(pmPathName [1] == ':') && (pmNewPathName [1] == ':'))
    {
	if (!MoveFile (pmPathName, pmNewPathName))
	{
	    MDIO_ProcessMSWindowsError (GetLastError ());
	}
    }
    else
    {
	// Instead, copy the file and then delete the original.
	if (!CopyFile (pmPathName, pmNewPathName, TRUE))
	{
	    MDIO_ProcessMSWindowsError (GetLastError ());
	    return;
	}
	if (!DeleteFile (pmPathName))
	{
	    MDIO_ProcessMSWindowsError (GetLastError ());
	}
    }

    SetErrorMode (myPrevErrorMode);
} // MDIOFile_Rename


/************************************************************************/
/* MDIOFile_Status							*/
/************************************************************************/
void	MDIOFile_Status (const char *pmPathName, int *pmFileSize,
		         int *pmAttrib, int *pmFileTime)
{
    UINT		myPrevErrorMode;
    DWORD		myWindowsAttributes;
    OSVERSIONINFO	myOSVersionInfo;
    int			myOOTAttributes;
    HANDLE		myHandle;
    FILETIME		myCreationTime, myLastAccessTime, myLastWriteTime;
    DWORD		mySize, myHighSize;
    LONGLONG		myLongTime;
    int			myTime;
    char		*myPtr;
    
    // Set default values
    *pmFileSize = -1;
    *pmAttrib = -1;
    *pmFileTime = -1;

    // Set the error mode so that missing disks fail normally
    // Load the first file in the directory
    myPrevErrorMode = SetErrorMode (SEM_FAILCRITICALERRORS);
    myWindowsAttributes = GetFileAttributes (pmPathName);
    SetErrorMode (myPrevErrorMode);
        	
    if (myWindowsAttributes == (DWORD) -1)
    {
    	DWORD	myError = GetLastError ();
    	
    	if (myError == ERROR_FILE_NOT_FOUND)
    	{
	    SET_ERRMSG (E_FSYS_FILE_NOT_FOUND, "'%s' not found", 
			pmPathName);
    	}
    	else if (myError == ERROR_PATH_NOT_FOUND)
    	{
    	    SET_ERRMSG (E_FSYS_PATH_NOT_FOUND, 
    	 	        "Path not found (couldn't find the directory "
    	 	        "that '%s' is in)", pmPathName);
    	}
    	else
    	{
	    MDIO_ProcessMSWindowsError (myError);
	}

    	return;
    }

    // Get the file attributes
    myOOTAttributes = OOT_ATTR_READ | OOT_ATTR_OWNER_READ |
    		      OOT_ATTR_GROUP_READ | OOT_ATTR_OTHER_READ;
    if ((myWindowsAttributes & FILE_ATTRIBUTE_READONLY) == 0)
    {
        myOOTAttributes |= OOT_ATTR_WRITE | OOT_ATTR_OWNER_WRITE |
        		   OOT_ATTR_GROUP_WRITE | OOT_ATTR_OTHER_WRITE;
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
    
    // Set the OS version
    ZeroMemory (&myOSVersionInfo, sizeof (myOSVersionInfo));
    myOSVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx (&myOSVersionInfo);
    
    // You cannot use CreateFile on a directory under Win 95/98/Me
    if ((myWindowsAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
    	(myOSVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS))
    {
	WIN32_FIND_DATA	myFindData;

	myPrevErrorMode = SetErrorMode (SEM_FAILCRITICALERRORS);
	myHandle = FindFirstFile (pmPathName, &myFindData);
	SetErrorMode (myPrevErrorMode);

	// Unfortunately, you cannot use FindFirstFile on a root directory
	// under Win95/98/Me.  (I don't think you can actually get the file
	// date of the root directory.)
	if (myHandle == (HANDLE) INVALID_HANDLE_VALUE)
	{
	    MDIO_ProcessMSWindowsError (GetLastError ());
	    return;
	}
	
	// Get the file time
	// Turing times use Unix style time.
	myLongTime = ((LONGLONG) myFindData.ftLastWriteTime.dwHighDateTime) << 32;
	myLongTime += myFindData.ftLastWriteTime.dwLowDateTime;
	myLongTime /= 10000000L;	// Convert to seconds
	myLongTime -= 11644473600;	// Convert from Jan 1, 1601 to Jan 1, 1970
	*pmFileTime = (unsigned long) myLongTime;
	
	*pmAttrib = myOOTAttributes;
	*pmFileSize = 0;
	return;
    }

    myPtr = strrchr (pmPathName, '.');
    if ((myPtr != NULL) &&
        ((_stricmp (myPtr, ".exe") == 0) ||
    	 (_stricmp (myPtr, ".com") == 0) ||
    	 (_stricmp (myPtr, ".bat") == 0) ||
   	 (_stricmp (myPtr, ".cmd") == 0)))
    {
        myOOTAttributes |= OOT_ATTR_EXECUTE | OOT_ATTR_OWNER_EXECUTE |
        		   OOT_ATTR_GROUP_EXECUTE | OOT_ATTR_OTHER_EXECUTE;
    }

    // Get the file time and size
    myPrevErrorMode = SetErrorMode (SEM_FAILCRITICALERRORS);

    if (myWindowsAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
	myHandle = CreateFile (pmPathName, FILE_LIST_DIRECTORY, 
	    FILE_SHARE_READ | FILE_SHARE_DELETE, NULL,
	    OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
    }
    else
    {
	myHandle = CreateFile (pmPathName, GENERIC_READ, 
	    FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
	    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    }

    SetErrorMode (myPrevErrorMode);

    // If call failed, then return FALSE
    if (myHandle == (HANDLE) INVALID_HANDLE_VALUE)
    {
	MDIO_ProcessMSWindowsError (GetLastError ());
    	return;
    }
    
    // Get the file time
    myPrevErrorMode = SetErrorMode (SEM_FAILCRITICALERRORS);
    if (!GetFileTime (myHandle, &myCreationTime, &myLastAccessTime,
    		      &myLastWriteTime))
    {
    	CloseHandle (myHandle);
    	SetErrorMode (myPrevErrorMode);
	MDIO_ProcessMSWindowsError (GetLastError ());
    	return;
    }

    // The file time is in local time for Windows 95, and in UTC for Win NT

    // Turing times use Unix style time.
    myLongTime = ((LONGLONG) myLastWriteTime.dwHighDateTime) << 32;
    myLongTime += myLastWriteTime.dwLowDateTime;
    myLongTime /= 10000000L;	// Convert to seconds
    myLongTime -= 11644473600;	// Convert from Jan 1, 1601 to Jan 1, 1970
    myTime = (unsigned long) myLongTime;

    if (myWindowsAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
    	// Directory doesn't have a file size.
	CloseHandle (myHandle);
    	SetErrorMode (myPrevErrorMode);
	*pmAttrib = myOOTAttributes;
	*pmFileSize = 0;
	*pmFileTime = myTime;
	return;
    }

    // Get the file size
    mySize = GetFileSize (myHandle, &myHighSize);
    if (mySize == -1)
    {
    	CloseHandle (myHandle);
    	SetErrorMode (myPrevErrorMode);
	MDIO_ProcessMSWindowsError (GetLastError ());
    	return;
    }
    
    CloseHandle (myHandle);
    SetErrorMode (myPrevErrorMode);
    
    *pmAttrib = myOOTAttributes;;
    *pmFileSize = mySize;
    *pmFileTime = myTime;
} // MDIOFile_Status


/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/
