/************/
/* miodir.c */
/************/

/*******************/
/* System includes */
/*******************/
#include <string.h>
#include <malloc.h>

/****************/
/* Self include */
/****************/
#include "miodir.h"

/******************/
/* Other includes */
/******************/
#include "mdiodir.h"

#include "mioerr.h"

#include "mio.h"

#include "mdio.h"

#include "mioerror.h"
#include "miofile.h"
#include "mdiofile.h"

// Test to make certain we're not accidentally including <windows.h> which
// might allow for windows contamination of platform independent code.
#ifdef _INC_WINDOWS
xxx
#endif

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
// Under Windows, opening up the directory autotmatically does a ready of
// the first file in the directory list.  The "data" field stores that
// data until the first read from the directory.  The "dataAvailable" 
// field indicates whether there's something in the data field (which
// is true only between the opening of the directory and the first read.
typedef struct DirInfo
{
    BOOL	reachedEOF;
    BOOL	emptyDirectory;
    DIR		mdDirInfo;
} DirInfo;

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
static void	MyFreeDir (DirInfo *pmDirInfo);

/***********************/
/* External procedures */
/***********************/
/************************************************************************/
/* MIODir_Create							*/
/************************************************************************/
void 	MIODir_Change (OOTstring pmPathName)
{
    char	myAbsolutePath [1024], myMDAbsolutePath [1024];

    // Convert to full path name
    if (!MIOFile_ConvertPath (pmPathName, myAbsolutePath, myMDAbsolutePath,
			      ALLOW_TRAILING_SLASH))
    {
    	// if MIOFile_ConvertPath returned false, then the error message was 
    	// set to indicate why.
    	return;
    }

    if (MDIODir_Exists (myMDAbsolutePath))
    {
	FileManager_ChangeExecDirectory (myMDAbsolutePath);
	MIOFile_SetExecutionDirectory (myAbsolutePath);
    }
    else
    {
    	// if MDIODir_Exists returned false, then the error message was set
	// to indicate why.
	return;
    }
} // MIODir_Change


/************************************************************************/
/* MIODir_Close								*/
/************************************************************************/
void	MIODir_Close (OOTint pmDirectoryID)
{
    DirInfo	*myDirInfo;

    myDirInfo = MIO_IDGet (pmDirectoryID, DIR_ID);
    MyFreeDir (myDirInfo);
    MIO_IDRemove (pmDirectoryID, DIR_ID);
} // MIODir_Close


/************************************************************************/
/* MIODir_Create							*/
/************************************************************************/
void	MIODir_Create (OOTstring pmPathName)
{
    char	myMDAbsolutePath [1024];

    // Convert to full path name
    if (!MIOFile_ConvertPath (pmPathName, NULL, myMDAbsolutePath,
			      ALLOW_TRAILING_SLASH))
    {
    	// if MIOFile_ConvertPath returned false, then the error message was 
    	// set to indicate why.
    	return;
    }

    if (MDIODir_Exists (myMDAbsolutePath))
    {
    	SET_ERRMSG (E_FSYS_DIR_EXISTS, "The directory '%s' already exists",
		    myMDAbsolutePath);
    }
    else if (MIOError_Last () == E_DIR_IS_A_FILE)
    {
    	SET_ERRMSG (E_FSYS_FILE_EXISTS, 
		    "A data file located at '%s' already exists",
		    myMDAbsolutePath);
    }
    else
    {
	// Eliminate any error messages set by MDIODir_Exists
	SET_ERRNO (E_NO_ERROR);
	MDIODir_Create (myMDAbsolutePath);
	return;
    }

} // MIODir_Create


/************************************************************************/
/* MIODir_Current							*/
/************************************************************************/
void 	MIODir_Current (OOTstring pmPathName)
{
    MIOFile_GetExecutionDirectory (pmPathName);
} // MIODir_Current


/************************************************************************/
/* MIODir_Delete							*/
/************************************************************************/
void	MIODir_Delete (OOTstring pmPathName)
{
    char	myMDAbsolutePath [1024];

    // Convert to full path name
    if (!MIOFile_ConvertPath (pmPathName, NULL, myMDAbsolutePath,
			      ALLOW_TRAILING_SLASH))
    {
    	// if MIOFile_ConvertPath returned false, then the error message was 
    	// set to indicate why.
    	return;
    }

    if (MDIODir_Exists (myMDAbsolutePath))
    {
	MDIODir_Delete (myMDAbsolutePath);
    }
    else
    {
    	// if MDIODir_Exists returned false, then the error message was set
	// to indicate why.
	if (MIOError_Last () == E_DIR_IS_A_FILE)
	{
	    SET_ERRMSG (E_DIR_NOT_A_DIRECTORY,
    	    	"\"%s\" is a file and cannot be deleted with Dir.Delete",
    	    	myMDAbsolutePath);
	}
	return;
    }
} // MIODir_Delete


/************************************************************************/
/* MIODir_Exists							*/
/************************************************************************/
BOOL	MIODir_Exists (OOTstring pmPathName)
{
    char	myMDAbsolutePath [1024];

    // Convert to full path name
    if (!MIOFile_ConvertPath (pmPathName, NULL, myMDAbsolutePath,
			      ALLOW_TRAILING_SLASH))
    {
    	// if MIOFile_ConvertPath returned false, then the error message was 
    	// set to indicate why.
    	return FALSE;
    }

    return MDIODir_Exists (myMDAbsolutePath);
} // MIODir_Exists


/************************************************************************/
/* MIODir_Get								*/
/************************************************************************/
void 	MIODir_Get (OOTint pmDirectoryID, OOTstring pmFileName)
{
    OOTint  myFileSize, myAttrib, myFileTime;

    MIODir_GetLong (pmDirectoryID, pmFileName, &myFileSize, &myAttrib,
		    &myFileTime);
} // MIODir_Get


/************************************************************************/
/* MIODir_GetLong							*/
/************************************************************************/
void 	MIODir_GetLong (OOTint pmDirectoryID, OOTstring pmFileName, 
		        OOTint *pmFileSize, OOTint *pmAttrib, 
		        OOTint *pmFileTime)
{
    DirInfo	*myDirInfo;

    myDirInfo = MIO_IDGet (pmDirectoryID, DIR_ID);

    if (myDirInfo -> reachedEOF)
    {
    	// Already reached the end of directory
    	*pmFileName = 0;
	*pmFileSize = -1;
	*pmAttrib = -1;
	*pmFileTime = -1;
	SET_ERRNO (E_DIR_READ_PAST_EOF);
    	return;
    }

    if (myDirInfo -> emptyDirectory)
    {
    	*pmFileName = 0;
    	*pmFileSize = -1;
    	*pmAttrib = -1;
    	*pmFileTime = -1;
    	myDirInfo -> reachedEOF = TRUE;
    	myDirInfo -> emptyDirectory = FALSE;
    	return;
    }

    if (!MDIODir_Get (myDirInfo -> mdDirInfo, pmFileName, 
		      pmFileSize, pmAttrib, pmFileTime))
    {
    	*pmFileName = 0;
    	*pmFileSize = -1;
    	*pmAttrib = -1;
    	*pmFileTime = -1;
    	myDirInfo -> reachedEOF = TRUE;
    	return;
    }
} // MIODir_GetLong


/************************************************************************/
/* MIODir_Open								*/
/************************************************************************/
OOTint	MIODir_Open (OOTstring pmPathName, SRCPOS *pmSrcPos)
{
    char	myMDAbsolutePath [1024];
    DirInfo	*myDirInfo;
    int		myDirID;
        
    // Convert to a full path name

    // Convert to full path name
    if (!MIOFile_ConvertPath (pmPathName, NULL, myMDAbsolutePath,
			      ALLOW_TRAILING_SLASH))
    {
    	// if MIOFile_ConvertPath returned false, then the error message was 
    	// set to indicate why.
    	return 0;
    }
    
    myDirInfo = (DirInfo *) malloc (sizeof (DirInfo));
    if (myDirInfo == NULL)
    {
	SET_ERRNO (E_DIR_CANT_ALLOC_MEM_FOR_DIR);
	return 0;
    }

    if (!MDIODir_OpenDir (myMDAbsolutePath, &myDirInfo -> mdDirInfo, 
			  &myDirInfo -> emptyDirectory))
    {
    	// Error set in MDIODir_OpenDir
	free (myDirInfo);
    	return 0;
    }

    myDirInfo -> reachedEOF = FALSE;

    myDirID = MIO_IDAdd (DIR_ID, myDirInfo, pmSrcPos, myMDAbsolutePath, NULL);

    // Couldn't allocate an identifier.  Return the default font ID.
    if (myDirID == 0)
    {
	MyFreeDir (myDirInfo);
	return 0;
    }
        	    
    return myDirID;
} // MIODir_Open


/*********************/
/* Static procedures */
/*********************/
/************************************************************************/
/* MyFreeDir								*/
/************************************************************************/
static void	MyFreeDir (DirInfo *pmDirInfo)
{
    MDIODir_FreeDir (pmDirInfo -> mdDirInfo);
    free (pmDirInfo);
} // MyFreeDir
