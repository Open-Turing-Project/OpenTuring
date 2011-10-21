/************************************************************************/
/* miofile.c								*/
/*									*/
/* There is a bit of a kludge here in that the Turing file manager has	*/
/* a parallel set of code to control the current directory, load files,	*/
/* etc.  In general, anything going through MIO will use the routines	*/
/* found here to calculate the path and then send the machine-dependent	*/
/* path to the Turing file manager.  However, the exception occurs when	*/
/* compiling files.  Any references during compilation are handled	*/
/* entirely by the Turing file manager.  The only real difference at	*/
/* the moment is that the MIOFile path handler recognizes that doing a	*/
/* ".." in the root directory is an error.  It also handles things like	*/
/* not providing a drive letter in a path and not providing a path,	*/
/* just a drive letter.							*/
/************************************************************************/

/*******************/
/* System includes */
/*******************/
#include <string.h>
#include <ctype.h>

/****************/
/* Self include */
/****************/
#include "miofile.h"

/******************/
/* Other includes */
/******************/
#include "miotypes.h"

#include "mdiofile.h"

#include "miodir.h"

#include "mio.h"

#include "mioerr.h"

#include "edint.h"

#include "miodialogs.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
#define PATH_SEP	'/'
#define PATH_SEP_STR	"/"

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
static int	stOS;
static char	stOOTDir [1024], stHomeDir [1024], stExecutionDir [1024];

/******************************/
/* Static callback procedures */
/******************************/

/*********************/
/* Static procedures */
/*********************/
static BOOL	MyConvertToTuringPath (const char *pmMDPathName, 
				       char *pmTuringPath, int *pmPathType);
static BOOL	MyIsElement (const char *pmPathName);

/***********************/
/* External procedures */
/***********************/
/************************************************************************/
/* MIOFile_Copy								*/
/************************************************************************/
void	MIOFile_Copy (const OOTstring pmSrcPath, 
		      const OOTstring pmDestPath)
{
    char	mySrcPath [1024];
    char	myDestPath [1024];

    // Convert to full path name
    if (!MIOFile_ConvertPath (pmSrcPath, NULL, mySrcPath, NO_TRAILING_SLASH))
    {
    	// if MIOFile_ConvertPath returned false, then the error message was 
	// set to indicate why.
	return;
    }

    // Check that file to be copied exists
    if (!MDIOFile_Exists (mySrcPath, FILE_ONLY))
    {
    	// if MDIOFile_Exists returned false, then the error message was set
	// to indicate why.
	return;
    }

    // Convert to full path name
    if (!MIOFile_ConvertPath (pmDestPath, NULL, myDestPath, 
			      ALLOW_TRAILING_SLASH))
    {
    	// if MIOFile_ConvertPath returned false, then the error message was 
	// set to indicate why.
	return;
    }

    if (MIODir_Exists (myDestPath))
    {
	// Add the file name component from pmSrcPath to myDestPath
	char	myPath [1024], myFileName [1024];

	MIOFile_SplitPath (myPath, myFileName, myDestPath);
	MIOFile_CreatePath (myDestPath, myDestPath, myFileName);
    }
    else
    {
	// MIODir_Exists may have set the error code.
	SET_ERRNO (E_NO_ERROR);
    }

    MDIOFile_Copy (mySrcPath, myDestPath);
} // MIOFile_Copy


/************************************************************************/
/* MIOFile_CreatePath							*/
/*									*/
/* This routine takes a base path and a relative path (in UNIX, Windows	*/
/* or Macintosh format and returns the Turing path.			*/
/*									*/
/* Path formats:							*/
/*  Turing - Relative: "a/b/c"						*/
/*	     Absolute: "/a/b/c" or "x:/a/b/c" or "volume:/a/b/c" or	*/
/*		       "//server/a/b/c"					*/
/*		("/a/b/c" alone on Windows or Mac assumes default vol)	*/
/*									*/
/*  UNIX -      Relative: "a/b/c/../d"					*/
/*              Absolute: "/a/b/c/../d" 				*/
/*  Windows -   Relative: "a\b\c\..\d" or "\\server\a\b\c\..\d"		*/
/*		Absolute: "x:\a\b\..\d"					*/
/*		Absolute (*): "\a\b\..\d"				*/
/*  Macintosh - Relative: ":a:b:c::d"					*/
/*		Absolute: "volume:a:b:c::d"				*/
/************************************************************************/
void	MIOFile_CreatePath (OOTstring pmPathName, 
			    const OOTstring pmBasePath,
			    const OOTstring pmRelativePath)
{
/*
    char    myBasePath [1024], myRelativePath [1024];
    char    *myPtr;
    // First, convert all the backslashes to forward slashes.
    strcpy (myBasePath, pmBasePath);
    while ((myPtr = strchr (myBasePath, '\\')) != NULL)
    {
	*myPtr = PATH_SEP;
    }
    // If it's a Macintosh style path, then 
    if ((strchr (myBasePath, PATH_SEP) == NULL) && 
	(strchr (myBasePath, ':') != NULL))
    {
	if (myBasePath [0] == ':')
	{
*/
} // MIOFile_CreatePath


/************************************************************************/
/* MIOFile_Delete							*/
/************************************************************************/
void	MIOFile_Delete (OOTstring pmPathName)
{
    char	myPathName [4096];

    // Convert to full path name
    if (!MIOFile_ConvertPath (pmPathName, NULL, myPathName, NO_TRAILING_SLASH))
    {
    	// if MIOFile_ConvertPath returned false, then the error message was 
	// set to indicate why.
	return;
    }

    // Check that file to be copied exists
    if (MDIOFile_Exists (myPathName, FILE_ONLY))
    {
	MDIOFile_Delete (myPathName);
    }
    else
    {
    	// if MDIOFile_Exists returned false, then the error message was set
	// to indicate why.
	return;
    }
} // MIOFile_Delete


/************************************************************************/
/* MIOFile_DiskFree							*/
/************************************************************************/
OOTint	MIOFile_DiskFree (OOTstring pmPathName)
{
    char	myPathName [4096];

    // Convert to full path name
    if (!MIOFile_ConvertPath (pmPathName, NULL, myPathName, NO_TRAILING_SLASH))
    {
    	// if MIOFile_ConvertPath returned false, then the error message was 
	// set to indicate why.
	return -1;
    }

    return MDIOFile_DiskFree (myPathName);
} // MIOFile_DiskFree


/************************************************************************/
/* MIOFile_Exists							*/
/************************************************************************/
BOOL	MIOFile_Exists (char *pmPathName)
{
    char	myPathName [1024];

    // Convert to full path name
    if (!MIOFile_ConvertPath (pmPathName, NULL, myPathName, NO_TRAILING_SLASH))
    {
    	// if MIOFile_ConvertPath returned false, then the error message was 
	// set to indicate why.
	return FALSE;
    }

    return MDIOFile_Exists (myPathName, FILE_ONLY);
} // MIOFile_Exists


/************************************************************************/
/* MIOFile_FullPath    							*/
/************************************************************************/
void	MIOFile_FullPath (OOTstring pmNewPath, const OOTstring pmOldPath)
{
    char	myPathName [4096];

    // Convert to full path name
    if (!MIOFile_ConvertPath (pmOldPath, myPathName, NULL, NO_TRAILING_SLASH))
    {
    	// if MIOFile_ConvertPath returned false, then the error message was 
	// set to indicate why.
	pmNewPath [0] = 0;
	return;
    }

    strncpy (pmNewPath, myPathName, 255);
    pmNewPath [255] = 0;
} // MIOFile_FullPath


/************************************************************************/
/* MIOFile_Parent    							*/
/************************************************************************/
void	MIOFile_Parent (OOTstring pmNewPath, const OOTstring pmOldPath)
{
    char	myPath [4096];
    char	*myRoot, *myPtr;
    
    // Convert to full path name
    if (!MIOFile_ConvertPath (pmOldPath, myPath, NULL, NO_TRAILING_SLASH))
    {
    	// if MIOFile_ConvertPath returned false, then the error message was 
	// set to indicate why.
	pmNewPath [0] = 0;
	return;
    }

    // return if root directory
    if (MIOFile_GetRootDir (myPath, &myRoot))
    {
    	strcpy (pmNewPath, pmOldPath);
    	SET_ERRNO (E_FILE_CANNOT_GET_PARENT_OF_ROOT);
    	return;
    }

    // At this point, we are guaranteed that the path does not end in a /
    // Back up until we reach a slash and discard it, or hit the root.
    myPtr = &myPath [strlen (myPath) - 1];
    while (*myPtr != '/')
    {
    	myPtr--;
    }
    if (myPtr < myRoot)
    {
    	myPtr++;
    }
    strncpy (pmNewPath, myPath, myPtr - myPath);
    pmNewPath [myPtr - myPath] = 0;
} // MIOFile_Parent


/************************************************************************/
/* MIOFile_Rename							*/
/************************************************************************/
void	MIOFile_Rename (OOTstring pmPathName, OOTstring pmNewPathName)
{
    char	myPathName [1024];
    char	myNewPathName [1024];

    // Convert to full path name
    if (!MIOFile_ConvertPath (pmPathName, NULL, myPathName, NO_TRAILING_SLASH))
    {
    	// if MIOFile_ConvertPath returned false, then the error message was 
	// set to indicate why.
	return;
    }

    // Check that file to be copied exists
    if (!MDIOFile_Exists (myPathName, FILE_OR_DIR))
    {
    	// if MDIOFile_Exists returned false, then the error message was set
	// to indicate why.
	return;
    }

    // Convert to full path name
    if (!MIOFile_ConvertPath (pmNewPathName, NULL, myNewPathName, 
			      NO_TRAILING_SLASH))
    {
    	// if MIOFile_ConvertPath returned false, then the error message was 
	// set to indicate why.
	return;
    }

    if (MDIOFile_Exists (myNewPathName, FILE_ONLY))
    {
	SET_ERRMSG (E_FSYS_FILE_EXISTS, "A data file '%s' already exists",
		    myNewPathName);
    }
    else if (MIODir_Exists (myNewPathName))
    {
	SET_ERRMSG (E_FSYS_FILE_EXISTS, "A directory '%s' already exists",
		    myNewPathName);
    }
    else
    {
	// MIODir_Exists may have set the error code.
	SET_ERRNO (E_NO_ERROR);
 	MDIOFile_Rename (myPathName, myNewPathName);
    }
} // MIOFile_Rename


/************************************************************************/
/* MIOFile_SplitPath							*/
/************************************************************************/
void	MIOFile_SplitPath (OOTstring pmPath, OOTstring pmFileName,
			   const OOTstring pathName)
{
} // MIOFile_SplitPath

#include <windows.h>
/************************************************************************/
/* MIOFile_Status							*/
/************************************************************************/
void	MIOFile_Status (OOTstring pmPathName, OOTint *pmFileSize,
		        OOTint *pmAttrib, OOTint *pmFileTime)
{
    char	myPathName [1024];

    // Convert to full path name
    if (!MIOFile_ConvertPath (pmPathName, NULL, myPathName, NO_TRAILING_SLASH))
    {
    	// if MIOFile_ConvertPath returned false, then the error message was 
	// set to indicate why.
	return;
    }

    MDIOFile_Status (myPathName, pmFileSize, pmAttrib, pmFileTime);
} // MIOFile_Status


/***************************************/
/* External procedures for MIO library */
/***************************************/
/************************************************************************/
/* MIOFile_Init								*/
/************************************************************************/
void	MIOFile_Init (int pmOS, const char *pmOOTDir, const char *pmHomeDir)
{
    int		myPathType;
    
    stOS = pmOS;
    
    if (!MyConvertToTuringPath (pmOOTDir, stOOTDir, &myPathType))
    {
    	EdInt_HaltEnvironment (IDS_MIO_FILEFAIL_1, __FILE__, __LINE__, 0, 101,
    			       pmOOTDir, stOOTDir, myPathType);
    }
    if ((myPathType & ABSOLUTE_PATH) == 0)
    {
    	EdInt_HaltEnvironment (IDS_MIO_FILEFAIL_1, __FILE__, __LINE__, 0, 102,
    			       pmOOTDir, stOOTDir, myPathType);
    }
    if (stOOTDir [strlen (stOOTDir) - 1] != PATH_SEP)
    {
        strcat (stOOTDir, PATH_SEP_STR);
    }

    if (!MyConvertToTuringPath (pmHomeDir, stHomeDir, &myPathType))
    {
    	EdInt_HaltEnvironment (IDS_MIO_FILEFAIL_1, __FILE__, __LINE__, 0, 103,
    			       pmHomeDir, stHomeDir, myPathType);
    }
    if ((myPathType & ABSOLUTE_PATH) == 0)
    {
    	EdInt_HaltEnvironment (IDS_MIO_FILEFAIL_1, __FILE__, __LINE__, 0, 104,
    			       pmHomeDir, stHomeDir, myPathType);
    }
    if (stHomeDir [strlen (stHomeDir) - 1] != PATH_SEP)
    {
        strcat (stHomeDir, PATH_SEP_STR);
    }
} // MIOFile_Init


/************************************************************************/
/* MIOFile_InitRun							*/
/************************************************************************/
void	MIOFile_InitRun (const char *pmExecutionDir)
{
} // MIOFile_InitRun


/************************************************************************/
/* MIOFile_ConvertPath							*/
/*									*/
/* Converts the path in pmUserPath to an absolute path name.  It sets	*/
/* both the Turing path (pmAbsolutePath) and Machine Dependent path	*/
/* (pmMDAbsolutePath).  Note that either can be NULL, in which case	*/
/* they are not set.  Note, this will require rewriting on the 		*/
/* Macintosh.								*/
/************************************************************************/
BOOL	MIOFile_ConvertPath (const char *pmMDUserPath, char *pmAbsolutePath, 
			     char *pmMDAbsolutePath, BOOL pmAllowTrailingSlash)
{
    char	myTuringUserPath [4096];
    int		myPathType;
    char	myPath [4096], myAbsolutePath [4096];
    char	*myPtr, *myStart, *myDestPtr;

    // Note that a Turing Path is UNIX like
    if (!MyConvertToTuringPath (pmMDUserPath, myTuringUserPath, &myPathType))
    {
	return FALSE;
    }

    if ((stOS != OS_WINDOWS) && (myPathType & WINDOWS_TYPE_PATH))
    {
	SET_ERRMSG (E_FSYS_MALFORMED_PATHNAME, 
		    "Attempt to use a Windows style pathname (%s) on a "
		    "non-Windows system", pmMDUserPath);
	return FALSE;		    
    }
    if ((stOS != OS_MACOS) && (myPathType & MACOS_TYPE_PATH))
    {
	SET_ERRMSG (E_FSYS_MALFORMED_PATHNAME, 
		    "Attempt to use a Macintosh style pathname (%s) on a "
		    "non-Macintosh system", pmMDUserPath);
	return FALSE;		    
    }

    //
    // There are a number of different file types possible at this point,
    // each with different flags:
    //
    // RELATIVE_PATH 			"a/b/c"
    // ABSOLUTE_PATH			"%oot/a/b/c"
    // ABSOLUTE_NO_VOL_PATH	   	"/a/b/c"  (Win/Mac assume default vol)
    // ABSOLUTE_PATH [WINDOWS]		"x:/a/b/c"
    // ABSOLUTE_PATH [WINDOWS]		"//server/a/b/c"
    // ABSOLUTE_PATH [MACOS]		"volume:/a/b/c"
    // ABSOLUTE_NO_DIR_PATH [WINDOWS]  	"x:" (Win assume default directory)
    //
    
    if (myPathType & RELATIVE_PATH)
    {
    	//
    	// RELATIVE_PATH 	"a/b/c"
    	//
    	
    	strcpy (myPath, stExecutionDir);
    	// Add the relative directory
    	strcat (myPath, myTuringUserPath);
    }
    else
    {
    	if ((strncmp (myTuringUserPath, "%oot", 4) == 0) ||
    	    (strncmp (myTuringUserPath, "%home", 5) == 0))
	{
	    //
	    // ABSOLUTE_PATH	"%oot/a/b/c"
	    //
    	    if (strcmp (myTuringUserPath, "%oot") == 0)
    	    {
    	    	strcpy (myPath, stOOTDir);
    	    }
    	    else if (strcmp (myTuringUserPath, "%home") == 0)
    	    {
    	    	strcpy (myPath, stHomeDir);
    	    }
    	    else if (strncmp (myTuringUserPath, "%oot/", 5) == 0)
    	    {
    	    	strcpy (myPath, stOOTDir);
    	    	strcat (myPath, &myTuringUserPath [5]);
    	    }
    	    else if (strncmp (myTuringUserPath, "%home/", 6) == 0)
    	    {
    	    	strcpy (myPath, stHomeDir);
    	    	strcat (myPath, &myTuringUserPath [6]);
    	    }
    	    else
    	    {
	    	EdInt_HaltEnvironment (IDS_MIO_FILEFAIL, __FILE__, __LINE__, 
	    			       0, 401);
    	    }
	}
	else if (myPathType & ABSOLUTE_NO_VOL_PATH)
	{
	    //
	    // ABSOLUTE_NO_VOL_PATH	   	"/a/b/c"  (Win/Mac assume default vol)
	    //
	    if (stOS == OS_UNIXLIKE)
	    {
	    	EdInt_HaltEnvironment (IDS_MIO_FILEFAIL, __FILE__, __LINE__, 
	    			       0, 402);
	    }
	    
	    // Copy the volume name from the execution directory
	    strcpy (myPath, stExecutionDir);
	    myPtr = myPath;
	    while ((*myPtr != ':') && (*myPtr != 0))
	    {
	    	myPtr++;
	    }
	    if (*myPtr == 0)
	    {
	    	EdInt_HaltEnvironment (IDS_MIO_FILEFAIL, __FILE__, __LINE__, 
	    			       0, 403);
	    }
	    
	    // Copy the user path after the execution directory volume
	    myPtr++;
	    strcpy (myPtr, myTuringUserPath);
	}
	else if (myPathType & ABSOLUTE_NO_DIR_PATH)
	{
	    //
	    // ABSOLUTE_NO_DIR_PATH [WINDOWS]  	"x:" (Win assume default directory)
	    //
	    if (stOS != OS_WINDOWS)
	    {
	    	EdInt_HaltEnvironment (IDS_MIO_FILEFAIL, __FILE__, __LINE__, 
	    			       0, 404);
	    }
	    	
	    if (!MDIOFile_GetDefaultPathForWindowsVolume (myTuringUserPath [0],
	    						  myPath))
	    {
		// Error message is set by MDIOFile_GetDefault...
		return FALSE;
	    }
	}
	else
	{
	    //
	    // We don't actually have to do any additional work for the 
	    // following.
	    //
	    // ABSOLUTE_PATH [WINDOWS]		"x:/a/b/c"
	    // ABSOLUTE_PATH [WINDOWS]		"//server/a/b/c"
	    // ABSOLUTE_PATH [MACOS]		"volume:/a/b/c"
	    //
	    strcpy (myPath, myTuringUserPath);
	}
    }

    //    
    // myPath is now in one of the following forms:
    //		/a/b/c"  (UNIX only)			[/]
    // 		"x:/a/b/c" (WINDOWS only)		[x:/]
    // 		"//server/share/a/b/c" (WINDOWS only)	[//server/share/]
    // 		"volume:/a/b/c" (MACOS only)		[volume:/]
    //
    // First, mark where the start of the path is
    //
    if ((myPath [0] == PATH_SEP) && (myPath [1] == PATH_SEP))
    {
    	myPtr = &myPath [2];	// myPtr now points at beginning of server name
    	// Skip over server name
    	while ((*myPtr != PATH_SEP) && (*myPtr != 0))
    	{
    	    myPtr++;
    	}
    	if (*myPtr == 0)
    	{
    	    EdInt_HaltEnvironment (IDS_MIO_FILEFAIL, __FILE__, __LINE__, 
    	    			   0, 405);
    	}
    	// Skip over share name
	myPtr++;	// myPtr now points at beginning of share name
    	while ((*myPtr != PATH_SEP) && (*myPtr != 0))
    	{
    	    myPtr++;
    	}
    }
    else if (myPath [0] == PATH_SEP)
    {
    	myPtr = myPath;
    }
    else
    {
    	myPtr = myPath;
    	// Skip over volume name
    	while ((*myPtr != ':') && (*myPtr != 0))
    	{
    	    myPtr++;
    	}
    	if (*myPtr == 0)
    	{
    	    EdInt_HaltEnvironment (IDS_MIO_FILEFAIL, __FILE__, __LINE__, 
    	    			   0, 406);
    	}
    	myPtr++;
    }

    if (*myPtr == 0)
    {
	EdInt_HaltEnvironment (IDS_MIO_FILEFAIL, __FILE__, __LINE__, 
	    0, 407);
    }

    myPtr++;

    // myPtr is now pointing at ^ [//server/share/^ or d:/^]

    strncpy (myAbsolutePath, myPath, myPtr - myPath);
    myStart = &myAbsolutePath [myPtr - myPath];
    myDestPtr = myStart;
    
    // Now factor out the ".."'s
    while (*myPtr != 0)
    {
    	if ((strncmp (myPtr, "../", 3) == 0) || (strcmp (myPtr, "..") == 0))
    	{
	    //
	    // Handle ".."
	    //
	    
	    // Remove directory and last /
    	    if (myDestPtr == myStart)
    	    {
	    	SET_ERRMSG (E_FSYS_MALFORMED_PATHNAME, 
		    "Attempt to move up ('..') in the root directory ('%s')",
		    pmMDUserPath);
		return FALSE;			
    	    }
    	    else
    	    {
		myDestPtr--;
    	    	do
    	    	{
    	    	    myDestPtr--;
    	    	} while (*myDestPtr != PATH_SEP);
    	    	myDestPtr++;
    	    }
    	    
	    // Skip over '..'
    	    if (strncmp (myPtr, "../", 3) == 0)
    	    {
    	    	myPtr += 3;
    	    }
    	    else
    	    {
    	    	// Special case: if ".." at the end of the line, we don't want
    	    	// to end with a /, *unless* we're at the root.  *sigh*.
    	    	myPtr += 2;
    	    	if (myDestPtr != myStart)
    	    	{
    	    	    myDestPtr--;
    	    	}
    	    }
    	    
    	}
    	else if ((strncmp (myPtr, "./", 2) == 0) || (strcmp (myPtr, ".") == 0))
    	{
	    //
	    // Handle "."
	    //
	    
    	    if (strncmp (myPtr, "./", 2) == 0)
    	    {
    	    	myPtr += 2;
    	    }
    	    else
    	    {
    	    	// Special case: if "." at the end of the line, we don't want
    	    	// to end with a /, *unless* we're at the root.  *sigh*.
    	    	myPtr += 1;
    	    	if (myDestPtr != myStart)
    	    	{
    	    	    myDestPtr--;
    	    	}
    	    }
	}
	else
	{
	    //
	    // Handle path element
	    //
	    if (*myPtr == PATH_SEP)
	    {
	    	SET_ERRMSG (E_FSYS_MALFORMED_PATHNAME, 
			    "'//' not allowed in path name (%s)", myAbsolutePath);
		return FALSE;			
	    }
	    while ((*myPtr != PATH_SEP) && (*myPtr != 0))
	    {
	    	*myDestPtr++ = *myPtr++;
	    }
	}
	if (*myPtr == PATH_SEP)
	{
	    *myDestPtr++ = *myPtr++;
	}
    } // while (*myPtr != 0)

    *myDestPtr = 0;
   
    // Remove any trailing "/" unless it's the root directory slash
    myDestPtr--;
    if ((*myDestPtr == PATH_SEP) && (myDestPtr > myStart))
    {
	if (pmAllowTrailingSlash)
	{
	    *myDestPtr = 0;
	}
	else
	{
	    SET_ERRMSG (E_FSYS_MALFORMED_PATHNAME, 
			"No trailing slash allowed in a file path name", 
			myAbsolutePath);
	    return FALSE;			
	}
    }

    //
    // At this point we have a Turing style absolute path in myAbsolutePath
    //
   
    // Copy to pmAbsolutePath if necessary
    if (pmAbsolutePath != NULL)
    {
    	strcpy (pmAbsolutePath, myAbsolutePath);
    }
    
    // If we don't need an MD absolute path, exit.
    if (pmMDAbsolutePath == NULL)
    {
    	return TRUE;
    }
    
    if (stOS == OS_UNIXLIKE)
    {
    	strcpy (pmMDAbsolutePath, myAbsolutePath);
    }
    else if (stOS == OS_WINDOWS)
    {
    	strcpy (pmMDAbsolutePath, myAbsolutePath);
    	myPtr = pmMDAbsolutePath;
    	while (*myPtr != 0)
    	{
    	    if (*myPtr == PATH_SEP)
    	    {
    	    	*myPtr = '\\';
    	    }
    	    myPtr++;
    	}
    }
    else if (stOS == OS_MACOS)
    {
    	// TW Work to do here
    }
    else
    {
    	EdInt_HaltEnvironment (IDS_MIO_FILEFAIL, __FILE__, __LINE__, 0, 301);
    }
    
    return TRUE;
} // MIOFile_ConvertPath


/************************************************************************/
/* MIOFile_GetExecutionDirectory					*/
/************************************************************************/
void	MIOFile_GetExecutionDirectory (char *pmAbsolutePath)
{
    int		myLen;

    strcpy (pmAbsolutePath, stExecutionDir);

    // Remove the trailing slash
    myLen = strlen (pmAbsolutePath);
    if (pmAbsolutePath [myLen - 1] == PATH_SEP)
    {
	if ((myLen != 1) && (pmAbsolutePath [myLen - 2] != ':'))
	{
	    pmAbsolutePath [myLen - 1] = 0;
	}
    }
} // MIOFile_GetExecutionDirectory


/************************************************************************/
/* MIOFile_GetRootDir							*/
/*									*/
/* This takes an absolute Turing or MD path and returns TRUE if it is	*/
/* a legal root directory, and a pointer to the location where the	*/
/* root directory ends.	Note, the pointer can be NULL			*/
/*  For example:							*/
/*	\\server\share\	    TRUE					*/
/*	x:\		    TRUE					*/
/*      /		    TRUE					*/
/*      \\server\share\xyz  FALSE   points to 'x'			*/
/*	x:/xyz		    FALSE   points to 'x'			*/
/*	/xyz/abc	    FALSE   points to 'x'			*/
/************************************************************************/
BOOL	MIOFile_GetRootDir (const char *pmPath, const char **pmRootChar)
{
    const char	*myPtr;
    
    if (((pmPath [0] == '\\') && (pmPath [1] == '\\')) ||
	((pmPath [0] == '/') && (pmPath [1] == '/')))
    {
	char	mySep = pmPath [0];

    	myPtr = &pmPath [2];
    	while ((*myPtr != mySep) && (*myPtr != 0))	// Skip over server
	{
    	    myPtr++;
	}
	myPtr++;					// Skip over sep
    	while ((*myPtr != mySep) && (*myPtr != 0))	// Skip over share
    	{
    	    myPtr++;
    	}
	myPtr++;					// Skip over sep
	if (pmRootChar != NULL)
	{
	    *pmRootChar = myPtr;
	}
	return (*myPtr == 0);
    }
    else if (pmPath [0] == '/')
    {
	if (pmRootChar != NULL)
	{
	    *pmRootChar = &pmPath [1];
	}
	return (pmPath [1] == 0);
    }
    else
    {
	// Format is X:\ or X:/
	if (pmRootChar != NULL)
	{
	    *pmRootChar = &pmPath [3];
	}
	return (pmPath [3] == 0);
    }
} // MIOFile_GetRootDir


/************************************************************************/
/* MIOFile_SetCurrentDirectory						*/
/*									*/
/* This tells the MIOFile module what the current execution directory	*/
/* is supposed to be.  Note that the pmAbsolutePath should already have	*/
/* been successfully converted to a Turing path before this function	*/
/* is called.								*/
/************************************************************************/
void	MIOFile_SetExecutionDirectory (const char *pmExecutionDir)
{
    int		myPathType;
    
    if (!MyConvertToTuringPath (pmExecutionDir, stExecutionDir, &myPathType))
    {
    	EdInt_HaltEnvironment (IDS_MIO_FILEFAIL, __FILE__, __LINE__, 0, 201);
    }
    if ((myPathType & ABSOLUTE_PATH) == 0)
    {
    	EdInt_HaltEnvironment (IDS_MIO_FILEFAIL, __FILE__, __LINE__, 0, 202);
    }
    if (stExecutionDir [strlen (stExecutionDir) - 1] != PATH_SEP)
    {
        strcat (stExecutionDir, PATH_SEP_STR);
    }
} // MIOFile_SetCurrentDirectory


/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/
/************************************************************************/
/* MIOFile_ConvertToTuringPath						*/
/*									*/
/* This routine takes a path returns the Turing version of the path.	*/
/*									*/
/* Path formats:							*/
/*  Turing -	Relative: (1)  "a/b/c"					*/
/*		Absolute: (2)  "%oot/a/b/c"				*/
/*			  (3)  "/a/b/c"  (Win/Mac assume default vol)	*/
/*			  (4)  "x:/a/b/c"				*/
/*		          (5)  "//server/a/b/c"				*/
/*			  (6)  "volume:/a/b/c"				*/
/*			  (7)  "x:" (Win assume default directory)	*/
/*  Macintosh - Relative: (8)  ":a:b:c::d"				*/
/*			  (9)  "::a:b"					*/
/*		Absolute: (10)  "volume:a:b:c::d"			*/
/*			  (11) "%oot:a:b:c::d"				*/
/*  UNIX -      Relative: (12) "a/b/c/../d" (same as (1))		*/
/*              Absolute: (13) "%oot/a/b/c/../d" (same as (2))		*/
/*			  (14) "/a/b/c/../d" (same as (3))		*/
/*  Windows -   Relative: (15) "a\b\c\..\d"				*/
/*		Absolute: (16) "%oot\a\b\c\..\d"			*/
/*			  (17) "\a\b\..\d" (Win/Mac assume default vol)	*/
/*			  (18) "x:\a\b\..\d"				*/
/*			  (19) "\\server\a\b\c\..\d"			*/
/*									*/
/* To avoid confusion, OOT does not allow either a '%', ':', '/' or '\'	*/
/* in any directory or file name.  In comments below, each directory	*/
/* or file name is referred to as an element.				*/
/************************************************************************/
static BOOL	MyConvertToTuringPath (const char *pmMDPathName, 
				       char *pmTuringPath, int *pmPathType)
{
    char    myPath [1024];
    char    ch;
    char    *mySrcPtr, *myDestPtr;
    int	    myNumElements = 0;
    BOOL    myIsServerStylePath = FALSE;

    // Initialize type of path
    *pmPathType = 0;
    
    // First, convert all the backslashes to forward slashes 
    // (maps path types 15-19 to 1 - 5)
    strcpy (myPath, pmMDPathName);
    while ((myDestPtr = strchr (myPath, '\\')) != NULL)
    {
	*myDestPtr = PATH_SEP;
    }

    mySrcPtr = myPath;
    myDestPtr = pmTuringPath; 
    // Okay, let's determine what type of file we're dealing with here.
    // It can start with 
    // (1)  "elementEOS", "element/"
    // (2)  "%ootEOS", "%homeEOS", "%oot/", "%home/" 
    // (3)  "/elementEOS", "/element{/:}"
    // (4)  "x:/"
    // (5)  "//element/"
    // (6)  "volume:/"
    // (7)  "x:EOS"
    // (8)  ":element{/:}"
    // (9)  "::"
    // (10) "volume:"
    // (11) "%oot:"

    // (2a) "%ootEOS", "%homeEOS"
    if ((strcmp (myPath, "%oot") == 0) || (strcmp (myPath, "%home") == 0))
    {
	strcpy (pmTuringPath, myPath);
	
	// Set type of path
	*pmPathType = ABSOLUTE_PATH;

	return TRUE;
    }
    // (7) "x:EOS"
    else if (isalpha (myPath [0]) && (myPath [1] == ':') && (myPath [2] == 0))
    {
	strcpy (pmTuringPath, myPath);
	strcat (pmTuringPath, PATH_SEP_STR);
	
	// Set type of path
	*pmPathType = ABSOLUTE_NO_DIR_PATH | WINDOWS_TYPE_PATH;

	return TRUE;
    }
    // (2b) "%oot/", "%home/" 
    else if ((strncmp (myPath, "%oot/", 5) == 0) || 
	     (strncmp (myPath, "%home/", 6) == 0))

    {
	while (*mySrcPtr != PATH_SEP)
	{
	    *myDestPtr++ = *mySrcPtr++;
	}
	*myDestPtr++ = *mySrcPtr++;
	
	// Set type of path
	*pmPathType = ABSOLUTE_PATH;
    }
    // (11) "%oot:"
    else if ((strncmp (myPath, "%oot:", 5) == 0) || 
	     (strncmp (myPath, "%home:", 6) == 0))
    {
	while (*mySrcPtr != ':')
	{
	    *myDestPtr++ = *mySrcPtr++;
	}
	*myDestPtr++ = PATH_SEP;
	mySrcPtr++;
	
	// Set type of path
	*pmPathType = ABSOLUTE_PATH;
    }
    // (4) "x:/"
    else if (isalpha (*mySrcPtr) && (myPath [1] == ':') && (myPath [2] == PATH_SEP))
    {
	int i;

	for (i = 0 ; i < 3 ; i++)
	{
	     *myDestPtr++ = *mySrcPtr++;
	}
	
	// Set type of path
	*pmPathType = ABSOLUTE_PATH | WINDOWS_TYPE_PATH;
    }
    // (5) "//element/"
    else if ((myPath [0] == PATH_SEP) && (myPath [1] == PATH_SEP) && 
	     MyIsElement (&myPath [2]))
    {
	*myDestPtr++ = *mySrcPtr++;
	*myDestPtr++ = *mySrcPtr++;
	ch = *mySrcPtr;
	while ((ch != PATH_SEP) && (ch != ':') && (ch != '%') && (ch != 0))
	{
	    *myDestPtr++ = *mySrcPtr++;
	    ch = *mySrcPtr;
	}
	if (ch != PATH_SEP)
	{
	    SET_ERRMSG (E_FSYS_MALFORMED_PATHNAME, 
			"Bad UNC format path ('%s'). Must be '//server/share'",
			pmMDPathName);
	    pmTuringPath [0] = 0;
	    return FALSE;
	}

	*myDestPtr++ = *mySrcPtr++;
		
	// Set type of path
	*pmPathType = ABSOLUTE_PATH | WINDOWS_TYPE_PATH;

	myIsServerStylePath = TRUE;
    }
    // (3) "/elementEOS", "/element{/:}"
    else if ((myPath [0] == PATH_SEP) && MyIsElement (&myPath [1]))
    {
	*myDestPtr++ = *mySrcPtr++;
	ch = *mySrcPtr;
	while ((ch != PATH_SEP) && (ch != ':') && (ch != '%') && (ch != 0))
	{
	    *myDestPtr++ = *mySrcPtr++;
	    ch = *mySrcPtr;
	}
	if (ch == '%')
	{
	    SET_ERRMSG (E_FSYS_BAD_CHAR_IN_PATHNAME, 
			"Illegal character in '%s'.  %% can only appear "
			"in path name as part of '%oot' or '%home'",
			pmMDPathName);
	    pmTuringPath [0] = 0;
	    return FALSE;
	}
	else if (ch == 0)
	{
	    *myDestPtr++ = 0;
	    return TRUE;
	}
	else if (ch == PATH_SEP)
	{
	    *myDestPtr++ = *mySrcPtr++;
	}
	else // ch == ':'
	{
	    *myDestPtr++ = PATH_SEP;
	    mySrcPtr++;
	    while (*mySrcPtr == ':')
	    {
		*myDestPtr++ = '.';
		*myDestPtr++ = '.';
		*myDestPtr++ = PATH_SEP;
		mySrcPtr++;
	    }
	}
		
	// Set type of path
	if (stOS == OS_UNIXLIKE)
	{
	    *pmPathType = ABSOLUTE_PATH;
	}
	else
	{
	    *pmPathType = ABSOLUTE_NO_VOL_PATH;
	}
    }
    // (9) "::"
    else if ((myPath [0] == ':') && (myPath [1] == ':'))
    {
	mySrcPtr++;
	while (*mySrcPtr == ':')
	{
	    *myDestPtr++ = '.';
	    *myDestPtr++ = '.';
	    *myDestPtr++ = PATH_SEP;
	    mySrcPtr++;
	}
		
	// Set type of path
	*pmPathType = RELATIVE_PATH;
    }
    // (8) ":element{/:}"
    else if ((myPath [0] == ':') && MyIsElement (&myPath [1]))
    {
	// This is a Macintosh-style relative path
	mySrcPtr++;
	ch = *mySrcPtr;
	while ((ch != PATH_SEP) && (ch != ':') && (ch != '%') && (ch != 0))
	{
	    *myDestPtr++ = *mySrcPtr++;
	    ch = *mySrcPtr;
	}
	if (ch == '%')
	{
	    SET_ERRMSG (E_FSYS_BAD_CHAR_IN_PATHNAME, 
			"Illegal character in '%s'.  %% can only appear "
			"in path name as part of '%oot' or '%home'",
			pmMDPathName);
	    pmTuringPath [0] = 0;
	    return FALSE;
	}
	else if (ch == 0)
	{
	    *myDestPtr++ = 0;
	    return TRUE;
	}
	else if (ch == PATH_SEP)
	{
	    *myDestPtr++ = *mySrcPtr++;
	}
	else // ch == ':'
	{
	    *myDestPtr++ = PATH_SEP;
	    mySrcPtr++;
	    while (*mySrcPtr == ':')
	    {
		*myDestPtr++ = '.';
		*myDestPtr++ = '.';
		*myDestPtr++ = PATH_SEP;
		mySrcPtr++;
	    }
	}
	
	// Set type of path
	*pmPathType = RELATIVE_PATH;
    }
    // (1) "elementEOS", "element/", (6) "volume:/", (10) "volume:"
    else if (MyIsElement (myPath))
    {
	ch = *mySrcPtr;
	while ((ch != PATH_SEP) && (ch != ':') && (ch != '%') && (ch != 0))
	{
	    *myDestPtr++ = *mySrcPtr++;
	    ch = *mySrcPtr;
	}
	if (ch == '%')
	{
	    SET_ERRMSG (E_FSYS_BAD_CHAR_IN_PATHNAME, 
			"Illegal character in '%s'.  %% can only appear "
			"in path name as part of '%oot' or '%home'",
			pmMDPathName);
	    pmTuringPath [0] = 0;
	    return FALSE;
	}
	// (1a)
	else if (ch == 0)
	{
	    *myDestPtr++ = *mySrcPtr++;
	
	    // Set type of path
	    *pmPathType = RELATIVE_PATH;

	    return TRUE;
	}
	// (1b)
	else if (ch == PATH_SEP)
	{
	    *myDestPtr++ = *mySrcPtr++;

	    // Set type of path
	    *pmPathType = RELATIVE_PATH;
	}
	// (6) or (10)
	else if (ch == ':')
	{
	    *myDestPtr++ = *mySrcPtr++;
	    ch = *mySrcPtr;

	    // (6)
	    if (ch == PATH_SEP)
	    {
		*myDestPtr++ = *mySrcPtr++;
	    }
	    // (10)
	    else if (MyIsElement (mySrcPtr))
	    {
		*myDestPtr++ = PATH_SEP;
	    }
	    else
	    {
		SET_ERRMSG (E_FSYS_MALFORMED_PATHNAME, 
			    "Malformed pathname in '%s'.  Macintosh path name "
			    "format is 'volume:folder:folder:file'", 
			    pmMDPathName);
		pmTuringPath [0] = 0;
		return FALSE;
	    }
	
	    // Set type of path
	    *pmPathType = ABSOLUTE_PATH | MACOS_TYPE_PATH;
	}
    }
    else
    {
	SET_ERRMSG (E_FSYS_MALFORMED_PATHNAME, "Malformed pathname in '%s'", 
		    pmMDPathName);
	pmTuringPath [0] = 0;
	return FALSE;
    }

    // We've now converted the first element.  We should now point at 
    // the next element or the end of file.
    while (*mySrcPtr != 0)
    {
	if (!MyIsElement (mySrcPtr))
	{
	    SET_ERRMSG (E_FSYS_MALFORMED_PATHNAME, "Malformed pathname in '%s'",
		        pmMDPathName);
	    pmTuringPath [0] = 0;
	    return FALSE;
	}
	ch = *mySrcPtr;
	while ((ch != PATH_SEP) && (ch != ':') && (ch != '%') && (ch != 0))
	{
	    *myDestPtr++ = *mySrcPtr++;
	    ch = *mySrcPtr;
	}
	if (ch == 0)
	{
	    // The format of //server/share should always have a terminating /
	    if ((myIsServerStylePath) && (myNumElements == 0))
	    {
		*myDestPtr++ = '/';
	    }

	    *myDestPtr++ = *mySrcPtr++;
	    return TRUE;
	}
	else if (ch == PATH_SEP)
	{
	    *myDestPtr++ = *mySrcPtr++;
	    myNumElements++;
	}
	else if (ch == '%')
	{
	    SET_ERRMSG (E_FSYS_BAD_CHAR_IN_PATHNAME, 
			"Illegal character in '%s'.  %% can only appear "
			"in path name as part of '%oot' or '%home'",
			pmMDPathName);
	    pmTuringPath [0] = 0;
	    return FALSE;
	}
	else if (ch == ':')
	{
	    // Colon has special meaning on Macintosh
	    *myDestPtr++ = PATH_SEP;
	    mySrcPtr++;
	    while (*mySrcPtr == ':')
	    {
		*myDestPtr++ = '.';
		*myDestPtr++ = '.';
		*myDestPtr++ = PATH_SEP;
		mySrcPtr++;
	    }
	}
    }

    *myDestPtr = 0;
    return TRUE;
} // MyConvertToTuringPath


/************************************************************************/
/* MyIsElement								*/
/************************************************************************/
static BOOL	MyIsElement (const char *pmPathName)
{
    char    ch = pmPathName [0];

    if (isalnum (ch) || (ch == '$') || (ch == '\'') || (ch == '-') ||
	(ch == '_') || (ch == '@') || (ch == '{') || (ch == '}') ||
	(ch == '~') || (ch == '`') || (ch == '!') || (ch == '#') ||
	(ch == '(') || (ch == ')') || (ch == '.'))
    {
	return TRUE;
    }

    return FALSE;
} // MyIsElement


