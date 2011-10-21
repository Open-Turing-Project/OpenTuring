/************/
/* edfile.c */
/************/

/*******************/
/* System includes */
/*******************/
#include <windows.h>
#include <crtdbg.h>
#include <shlobj.h>

/****************/
/* Self include */
/****************/
#include "edfile.h"

/******************/
/* Other includes */
/******************/
#include "edfail.h"
#include "edglob.h"
#include "edgui.h"
#include "edprog.h"
#include "edprop.h"

#include "eddialogs.h"

/**********/
/* Macros */
/**********/
#define NEW_LINE	"\r\n"
#define BACKUP_SUFFIX	".bak"

#define RTF_TAB		"\\tab "
#define RTF_OPEN_BRACE	"\\{"
#define RTF_CLOSE_BRACE	"\\}"
#define RTF_BACKSLASH	"\\\\"
#define RTF_M_DASH	"\\endash "
#define RTF_N_DASH	"-"
#define RTF_NEWLINE	"\\par \r\n"
#define RTF_ITALIC_TAG	"{\\i "
#define RTF_BOLD_TAG	"{\\b "
#define RTF_END_TAG	"}"

// Remove when using new Platform SDK
#ifndef CSIDL_FLAG_CREATE
#define CSIDL_FLAG_CREATE	0x8000
#endif // #ifndef CSIDL_FLAG_CREATE

/*************/
/* Constants */
/*************/

/********************/
/* Global variables */
/********************/

/*********/
/* Types */
/*********/
typedef struct ReadBufferData
{
    char	*buffer;
    char	*bufPtr;
    char	*end;
    char	*charPtr;
    char	ch;
    struct ReadBufferData *next;
} ReadBufferData, *ReadBufferDataPtr;

/**********************/
/* External variables */
/**********************/

/********************/
/* Static constants */
/********************/
static char	*stRTFHeader [] = 
    {
    	"{\\rtf1\\mac\\deff2 \r\n",
     	// RTF Font table
     	"{\\fonttbl{\\f16\\fmodern Courier;}{\\f20\\froman Times;}}\r\n",
     	// RTF Colour table
     	"{\\colortbl\\red0\\green0\\blue0;}\r\n",
     	// RTF Style sheet table
     	"{\\stylesheet\r\n",
     	// Style - Normal
     	"{\\f20 \\sbasedon222\\snext0 Normal;}\r\n",
     	// Style - program,p,program-body,pb
     	"{\\s1\\sl-240",
     	"\\tx360\\tx720\\tx1080\\tx1440\\tx1800\\tx2160",
     	"\\tx2520\\tx2880\\tx3240\\tx3600\\tx3960\\tx4320",
     	"\\tx4680\\tx5040\\tx5400\\tx5760\\tx6120\\tx6480 ",
     	"\\f16\\fs20 \\sbasedon222\\snext1 ",
     	"program;}}\r\n",
     	// More header stuff
     	"\\margl1440\\margr1440\\ftnbj\\fracwidth ",
     	"\\sectd \\sbknone\\linemod0\\linex0\\cols1\\endnhere \r\n",
     	"\\pard\\plain \\s1\\sl-240\\tx360\\tx720\\tx1080\\tx1440",
     	"\\tx1800\\tx2160\\tx2520\\tx2880\\tx3240\\tx3600\\tx3960",
     	"\\tx4320\\tx4680\\tx5040\\tx5400\\tx5760\\tx6120\\tx6480 ",
     	"\\f16\\fs20 \r\n",
     	""
     };
static char	*stRTFTrailer = "}\r\n";
    	
/********************/
/* Static variables */
/********************/
// Other
static FilePath			stActiveDirectory;
static ReadBufferDataPtr	stReadBufferDataHead = NULL;

/******************************/
/* Static callback procedures */
/******************************/

/*********************/
/* Static procedures */
/*********************/
static void	MyConvertSlashToBackslash (char *pmPathName);
static char	*MyGetBackupPathName (char *pmPathName, 
				      char *pmPathWithBakExtension,
				      char *pmExtension);
static BOOL	MyWriteFile (HWND pmWindow, char *pmPathName, HANDLE pmFile, 
			     char *pmString, int *pmBytesSaved); 
static void	MyWriteFileEntry (FileEntryPtr pmFile, const char *pmString);

/***********************/
/* External procedures */
/***********************/
#ifndef TPROLOG
/************************************************************************/
/* EdFile_Init								*/
/************************************************************************/
BOOL	EdFile_Init (void)
{
    return TRUE;
} // EdFile_Init


/************************************************************************/
/* EdFile_Finalize							*/
/************************************************************************/
void	EdFile_Finalize (void)
{
    // Free all the items in the ReadBuffer list
    ReadBufferDataPtr	myPtr, myLastPtr;
    
    myLastPtr = stReadBufferDataHead;
    while (myLastPtr != NULL)
    {
	myPtr = myLastPtr -> next;
	free (myLastPtr);
	myLastPtr = myPtr;
    }
} // EdFile_Finalize


/************************************************************************/
/* EdFile_PropertiesSet							*/
/************************************************************************/
void	EdFile_PropertiesSet (void)
{
    EdFile_PropertiesImplementChanges ();
} // EdFile_PropertiesSet

	
/************************************************************************/
/* EdFile_PropertiesImplementChanges					*/
/************************************************************************/
void	EdFile_PropertiesImplementChanges (void)
{
} // EdFile_PropertiesImplementChanges
#endif // #ifndef TPROLOG


/************************************************************************/
/* EdFile_CloseFile							*/
/************************************************************************/
void	EdFile_CloseFile (FileEntryPtr pmFile)
{
    if (!CloseHandle (pmFile -> handle))
    {
    	int		myErrorCode = GetLastError ();
    	ErrorBuffer	myErrorBuffer;
    	
    	EdGUI_Message1 (NULL, 0, IDS_FILE_WRITE_FAILED_TITLE, 
    	    IDS_CANT_CLOSE_FILE, pmFile -> pathName, 
    	    EdFail_GetErrorMessage (myErrorBuffer), myErrorCode);
    }

    free (pmFile -> pathName);
    free (pmFile);
} // EdFile_CloseFile


#ifndef TPROLOG
/************************************************************************/
/* EdFile_CombinePath							*/
/*									*/
/* Combines pmBasePath with pmNewPath to produce pmAbsolutePath.	*/
/*									*/
/* Acceptable combinations:						*/
/*	pmBasePath	    pmNewPath	    pmAbsolutePath		*/
/* 1.	  any		    x:\a\b\c	      x:\a\b\c			*/
/* 2.	  any		    \\s\a\b\c	      \\s\a\b\c			*/
/* 3.    x:\a\b\c	    \d\e\f	      x:\d\e\f			*/
/* 4.    \\s\a\b\c          \d\e\f            \\s\a\b\c			*/
/* 5.    x:\a\b\c	    d\e\f	     x:\a\b\c\d\e\f		*/
/* 6.    \\s\a\b\c          d\e\f            \\s\a\b\c\d\e\f		*/
/************************************************************************/
BOOL	EdFile_CombinePath (const char *pmBasePath, const char *pmNewPath,
			    char *pmAbsolutePath)
{
    BOOL	myNewPathIsAbsolute = FALSE;

    // pmNewPath is absolute if starts with "X:\", "X:/" or "\\"or "//"
    if (isalpha (pmNewPath [0]) && (pmNewPath [1] == ':')  &&
	((pmNewPath [2] == '\\') || (pmNewPath [2] == '/')))
    {
	// 1.	  any		    x:\a\b\c	      x:\a\b\c
	strcpy (pmAbsolutePath, pmNewPath);
	MyConvertSlashToBackslash (pmAbsolutePath);
	return TRUE;
    }
    if (((pmNewPath [0] == '\\') && (pmNewPath [1] == '\\')) ||
	((pmNewPath [0] == '/') && (pmNewPath [1] == '/')))
    {
	// 2.	  any		    \\s\a\b\c	      \\s\a\b\c
	strcpy (pmAbsolutePath, pmNewPath);
	MyConvertSlashToBackslash (pmAbsolutePath);
	return TRUE;
    }

    if (pmBasePath == NULL)
    {
	return FALSE;
    }

    strcpy (pmAbsolutePath, pmBasePath);
    MyConvertSlashToBackslash (pmAbsolutePath);

    if ((pmNewPath [0] == '\\') || (pmNewPath [0] == '/'))
    {
	// 3.    x:\a\b\c	    \d\e\f	      x:\d\e\f
	// 4.    \\s\a\b\c          \d\e\f            \\s\a\b\c
	if (isalpha (pmBasePath [0]))
	{
	    // x:
	    pmAbsolutePath [2] = 0;
	}
	else
	{
	    // \\s
	    char	*myPtr = &pmAbsolutePath [2];

	    while (*myPtr != '\\')
	    {
		myPtr++;
	    }
	    *myPtr = 0;
	}
	strcat (pmAbsolutePath, pmNewPath);
	MyConvertSlashToBackslash (pmAbsolutePath);
	return TRUE;
    }

    if (pmAbsolutePath [strlen (pmAbsolutePath) - 1] != '\\')
    {
	strcat (pmAbsolutePath, "\\");
    }

    strcat (pmAbsolutePath, pmNewPath);
    MyConvertSlashToBackslash (pmAbsolutePath);
    return TRUE;
} // EdFile_CombinePath


/************************************************************************/
/* EdFile_ConstructPath							*/
/*									*/
/* If pmDirectoryStringResource is					*/
/*	FROM_STARTUP - Then file name is off of the startup directory	*/
/*	FROM_APPDATA - Then file name is off of the app data directory	*/
/*	other - Then directory name and file name is off application 	*/
/*		directory						*/
/************************************************************************/
void	EdFile_ConstructPath (char *pmPathName, int pmDirectoryStringResource,
			      int pmFileStringResource)
{
    char	myString [256];
    
    if (pmDirectoryStringResource == FROM_STARTUP)
    {
    	strcpy (pmPathName, EdProp_GetStartupDirectory ());
    }
    else if (pmDirectoryStringResource == FROM_APPDATA)
    {
    	int	myResult = EdFile_GetWindowsFolder (CSIDL_APPDATA, pmPathName,
    						    TRUE);
	// If there was a failure, use the startup directory.
	if (myResult != 0)
	{
    	    strcpy (pmPathName, EdProp_GetStartupDirectory ());
	}
    }
    else
    {
    	EdFile_GetApplicationDirectory (pmPathName);
    	EdGUI_LoadString (pmDirectoryStringResource, myString, 
    	    sizeof (myString));
    	strcat (pmPathName, myString);
    }
    
    if ((pmFileStringResource != NO_FILE_NAME) &&
        (pmFileStringResource != DIR_ONLY_NO_SLASH))
    {
    	if (pmFileStringResource == ADMIN_PREF)
    	{
    	    if (gProgram.isJava) 
    	        pmFileStringResource = IDS_JAVA_ADMIN_PREF;
    	    else if (gProgram.isTuring) 
    	        pmFileStringResource = IDS_TURING_ADMIN_PREF;
	}
	else if (pmFileStringResource == USER_PREF)
    	{
    	    if (gProgram.isJava) 
    	        pmFileStringResource = IDS_JAVA_USER_PREF;
    	    else if (gProgram.isTuring) 
    	        pmFileStringResource = IDS_TURING_USER_PREF;
	}
	else if (pmFileStringResource == RECENT_FILES)
    	{
    	    if (gProgram.isJava) 
    	        pmFileStringResource = IDS_JAVA_RECENT_FILES;
    	    else if (gProgram.isTuring) 
    	        pmFileStringResource = IDS_TURING_RECENT_FILES;
	}
	else if (pmFileStringResource == KEYWORD_LOOKUP)
    	{
    	    if (gProgram.isJava) 
    	        pmFileStringResource = IDS_JAVA_KEYWORD_LOOKUP;
    	    else if (gProgram.isTuring) 
    	        pmFileStringResource = IDS_TURING_KEYWORD_LOOKUP;
	}
	else if (pmFileStringResource == EXAMPLES_DIR)
    	{
    	    if (gProgram.isJava) 
    	        pmFileStringResource = IDS_JAVA_EXAMPLES_DIR;
    	    else if (gProgram.isTuring) 
    	        pmFileStringResource = IDS_TURING_EXAMPLES_DIR;
	}
    	EdGUI_LoadString (pmFileStringResource, myString, sizeof (myString));
    	strcat (pmPathName, myString);
    }
    else if ((pmFileStringResource == DIR_ONLY_NO_SLASH) &&
    	     EdGlob_EndsWith (pmPathName, "\\") && 
             !EdGlob_EndsWith (pmPathName, ":\\"))
    {
        pmPathName [strlen (pmPathName) - 1] = 0;
    }        
} // EdFile_ConstructPath

					      
/************************************************************************/
/* EdFile_CreateDirectoriesIfNecessary					*/
/************************************************************************/
BOOL	EdFile_CreateDirectoriesIfNecessary (const char *pmPathName)
{
    FilePath	myPathName;
    const char	*myPtr;

    myPtr = pmPathName;

    // Skip over x:\ or \\server\ 
    if (isalpha (pmPathName [0]))
    {
	myPtr = &pmPathName [3];
    }
    else
    {
	myPtr = &pmPathName [2];
	while (*myPtr != '\\')
	{
	    myPtr++;
	}
	myPtr++;
    }

    while (*myPtr != 0)
    {
	while ((*myPtr != 0) && (*myPtr != '\\'))
	{
	    myPtr++;
	}

	strncpy (myPathName, pmPathName, myPtr - pmPathName);
	myPathName [myPtr - pmPathName] = 0;
	if (EdFile_FileExists (myPathName))
	{
	    EdGUI_Message1 (NULL, MB_ICONEXCLAMATION,
			    IDS_CREATE_DIR_FAILED_TITLE,
			    IDS_FILE_EXISTS, myPathName,
			    pmPathName);
	    return FALSE;
	}
	if (!EdFile_DirectoryExists (myPathName))
	{
	    if (!CreateDirectory (myPathName, NULL))
	    {
		EdGUI_Message1 (NULL, MB_ICONEXCLAMATION,
		    IDS_CREATE_DIR_FAILED_TITLE,
		    IDS_CREATE_DIR_FAILED, myPathName,
		    pmPathName, GetLastError ());
		return FALSE;
	    }
	}
	
	if (*myPtr == '\\')
	{
	    myPtr++;
	}
    }

    return TRUE;
} // EdFile_CreateDirectoriesIfNecessary


/************************************************************************/
/* EdFile_DirectoryExists						*/
/************************************************************************/
BOOL	EdFile_DirectoryExists (const char *pmPathName)
{
    WIN32_FIND_DATA	myFindFileData;
    HANDLE		myFile;
    
    // Open the file if it already exists
    myFile = FindFirstFile (pmPathName, &myFindFileData);
    
    if (myFile == INVALID_HANDLE_VALUE)
    {
    	return FALSE;
    }
    else
    {
    	FindClose (myFile);
    	return myFindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
    }

    return TRUE;
} // EdFile_DirectoryExists


/************************************************************************/
/* EdFile_FileExists							*/
/************************************************************************/
BOOL	EdFile_FileExists (const char *pmPathName)
{
    HANDLE	myFile;
    
    // Open the file if it already exists
    myFile = CreateFile (pmPathName, GENERIC_READ, 
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
    	OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    	
    // If call failed, then return FALSE
    if (myFile == (HANDLE) INVALID_HANDLE_VALUE)
    {
    	return FALSE;
    }
    
    CloseHandle (myFile);
	
    return TRUE;
} // EdFile_FileExists


/************************************************************************/
/* EdFile_GetActiveDirectory						*/
/************************************************************************/
void	EdFile_GetActiveDirectory (FilePath pmPathName)
{
    strcpy (pmPathName, stActiveDirectory);
} // EdFile_GetActiveDirectory


/************************************************************************/
/* EdFile_GetApplicationDirectory					*/
/************************************************************************/
void	EdFile_GetApplicationDirectory (char *pmApplicationDirectory)
{
    char	*myPtr;
    
    GetModuleFileName (NULL, pmApplicationDirectory, PATHNAME_LENGTH);
    myPtr = strrchr (pmApplicationDirectory, '\\');
    if (myPtr == NULL)
    {
    	// ERROR! No backslash in pathname!
    	EdFail_Fatal (IDS_BADDIRECTORY, __FILE__, __LINE__, 0,
    		      pmApplicationDirectory);
    }
    myPtr++;
    *myPtr = 0;
} // EdFile_GetApplicationDirectory
#endif // #ifndef TPROLOG


/************************************************************************/
/* EdFile_GetCurrentDirectory						*/
/*									*/
/* Returns the editors current directory, always with trailing '\'.	*/
/************************************************************************/
void	EdFile_GetCurrentDirectory (char *pmCurrentDirectory)
{
    GetCurrentDirectory (PATHNAME_LENGTH, pmCurrentDirectory);
    if (pmCurrentDirectory [strlen (pmCurrentDirectory) - 1] != '\\')
    {
    	strcat (pmCurrentDirectory, "\\");
    }
} // EdFile_GetCurrentDirectory


#ifndef TPROLOG
/************************************************************************/
/* EdFile_GetDirectoryFromPath						*/
/************************************************************************/
void	EdFile_GetDirectoryFromPath (const char *pmPath, char *pmDirectory)
{
    char	*myPtr;
    
    strcpy (pmDirectory, pmPath);
    myPtr = strrchr (pmDirectory, '\\');
    if (myPtr == NULL)
    {
    	// ERROR! No backslash in pathname!
    	EdFail_Fatal (IDS_BADDIRECTORY, __FILE__, __LINE__, 0, pmDirectory);
    }
    myPtr++;
    *myPtr = 0;
} // EdFile_GetDirectoryFromPath


/************************************************************************/
/* EdFile_GetPathForDisplay						*/
/************************************************************************/
char	*EdFile_GetPathForDisplay (const char *pmPathName, int pmUnnamedNumber,
			    	   BOOL pmIsExample, const char *pmExampleName, 
				   BOOL pmFullPath, 
				   FilePath pmPathForDisplayBuffer)
{
    if (pmPathName != NULL)
    {
    	if (pmFullPath)
    	{
    	    strcpy (pmPathForDisplayBuffer, pmPathName);
    	}
    	else
    	{
    	    strcpy (pmPathForDisplayBuffer, EdFile_GetFileName (pmPathName));
	}
    }
    else
    {
    	if (pmIsExample)
    	{
    	    strcpy (pmPathForDisplayBuffer, pmExampleName);
    	}
    	else
    	{
            char	myUnnamedWindowTitle [256];
        
    	    // Load the unnamed window title 
            EdGUI_LoadString (IDS_UNNAMED_WINDOW_TITLE,
	    	myUnnamedWindowTitle, sizeof (myUnnamedWindowTitle));
    	
    	    wsprintf (pmPathForDisplayBuffer, myUnnamedWindowTitle, 
    	    	pmUnnamedNumber);
    	}
    }

    return pmPathForDisplayBuffer;
} // EdFile_GetPathForDisplay
#endif // #ifndef TPROLOG


/************************************************************************/
/* EdFile_GetFileName							*/
/************************************************************************/
const char	*EdFile_GetFileName (const char *pmPathName)
{
    char	*myPtr = strrchr (pmPathName, '\\');
    
    if (myPtr == NULL)
    {
    	return pmPathName;
    }
    else
    {
    	return myPtr + 1;
    }
} // EdFile_MyGetFileName


#ifndef TPROLOG
/************************************************************************/
/* EdFile_GetFileNameForwardSlash					*/
/************************************************************************/
const char	*EdFile_GetFileNameForwardSlash (const char *pmPathName)
{
    char	*myPtr = strrchr (pmPathName, '/');
    
    if (myPtr == NULL)
    {
    	return pmPathName;
    }
    else
    {
    	return myPtr + 1;
    }
} // EdFile_GetFileNameForwardSlash


/************************************************************************/
/* EdFile_GetFileSuffix							*/
/************************************************************************/
const char	*EdFile_GetFileSuffix (const char *pmPathName)
{
    char	*myPtr = strrchr (pmPathName, '.');
    char	*mySlashPtr = strrchr (pmPathName, '\\');
    
    if ((myPtr == NULL) || ((mySlashPtr != NULL) && (myPtr < mySlashPtr)))
    {
    	return "";
    }
    else
    {
    	return myPtr;
    }
} // EdFile_GetFileSuffix


/************************************************************************/
/* EdFile_GetUnnamedNumber						*/
/************************************************************************/
int		EdFile_GetUnnamedNumber (void)
{
    // The current unnamed window number
    static int	myCurrentUnnamedWindowNumber = 0;
    
    myCurrentUnnamedWindowNumber++;
    
    return myCurrentUnnamedWindowNumber;
} // EdFile_GetUnnamedNumber


/************************************************************************/
/* EdFile_GetWindowsFolder						*/
/*									*/
/* Returns 0 = Success							*/
/*         1 = Call okay, but file does not exist.			*/
/*         2 = Failure							*/
/************************************************************************/
int	EdFile_GetWindowsFolder (int pmCSIDL, char *pmPathName, BOOL pmCreate)
{
    HMODULE		mySHFolderLib;
    FARPROC		mySHGetFolderPath;
    HRESULT		myResult = -1;
    int			myCSIDL;
    
    // SHGetFolderPath can work everywhere SHFOLDER is installed.
    mySHFolderLib = LoadLibrary ("shfolder.dll");
    if (mySHFolderLib == NULL) 
    {
    	return 2;
    }
    
    // See if the function exists in the library
    * (FARPROC *) &mySHGetFolderPath = GetProcAddress(mySHFolderLib, 
         					"SHGetFolderPathA");
    if (mySHGetFolderPath == NULL)
    {
    	return 2;
    }

    // Calculate the CSIDL	    
    myCSIDL = pmCSIDL;
    if (pmCreate)
    {
    	myCSIDL |= CSIDL_FLAG_CREATE;
    }

    // Call SHGetFolderPath    
    myResult = mySHGetFolderPath (NULL, myCSIDL, NULL, NULL, pmPathName);
    if ((myResult != S_OK) && (myResult != S_FALSE))
    {
    	return 2;
    }
    
    // Add the trailing backslash
    if (pmPathName [strlen (pmPathName) - 1] != '\\')
    {
    	strcat (pmPathName, "\\");
    }
    
    if (myResult == S_OK)
    {
    	return 0;
    }
    else
    {
    	return 1;
    }
} // EdFile_GetWindowsFolder
#endif // #ifndef TPROLOG


/************************************************************************/
/* EdFile_OpenFileForWrite						*/
/************************************************************************/
FileEntryPtr	EdFile_OpenFileForWrite (const char *pmPathName, 
					 int *pmErrorCode, BOOL pmAlwaysFlush, 
					 BOOL pmAppend)
{
    HANDLE		myHandle;
    FileEntryPtr	myFileEntry;
    
    myHandle = CreateFile (pmPathName, GENERIC_WRITE, 0, NULL,
    	OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (myHandle == (HANDLE) INVALID_HANDLE_VALUE)
    {
    	*pmErrorCode = GetLastError ();
    	return NULL;
    }
    else
    {
    	if (pmAppend)
    	{
    	    SetFilePointer (myHandle, 0, NULL, FILE_END);
/*    	    
		INVALID_SET_FILE_POINTER doesn't exist!
	    if (SetFilePointer (myHandle, 0, NULL, FILE_END) == 
    	        				INVALID_SET_FILE_POINTER)
    	    {
    	    	*pmErrorCode = GetLastError ();
    	    	return NULL;
    	    }
*/    	    
	}
	else
	{
    	    if (!SetEndOfFile (myHandle))
    	    {
    	    	*pmErrorCode = GetLastError ();
    	    	return NULL;
    	    }
    	}

    	
    	myFileEntry = (FileEntryPtr) malloc (sizeof (FileEntry));
    	// TW check for NULL
    	myFileEntry -> handle = myHandle;
    	myFileEntry -> pathName = malloc (strlen (pmPathName) + 1);
    	// TW check for NULL
    	strcpy (myFileEntry -> pathName, pmPathName);
    	myFileEntry -> errorFound = FALSE;
    	myFileEntry -> alwaysFlush = pmAlwaysFlush;
    	*pmErrorCode = 0;
    	return myFileEntry;
    }
} // EdFile_OpenFileForWrite


#ifndef TPROLOG
/************************************************************************/
/* EdFile_ReadFileIntoBuffer						*/
/************************************************************************/
char	*EdFile_ReadFileIntoBuffer (HWND pmWindow, const char *pmPathName, 
				    DWORD *pmFileSize, BOOL pmOKIfNotFound)
{
    HANDLE	myFile;
    DWORD	myBytesRead;
    char	*myBuffer;
    ErrorBuffer	myErrorBuffer;
    DWORD	myFileSize;
    
    if (pmFileSize != NULL)
    {
    	*pmFileSize = -1;
    }
    
    // Open the file
    myFile = CreateFile (pmPathName, GENERIC_READ, FILE_SHARE_READ, NULL,
    	OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
    	NULL);
    if (myFile == (HANDLE) INVALID_HANDLE_VALUE)
    {
	int	myErrorCode = GetLastError ();
	
    	if ((myErrorCode != ERROR_FILE_NOT_FOUND) || !pmOKIfNotFound)
    	{
    	    if (myErrorCode == ERROR_FILE_NOT_FOUND)
    	    {
    	    	EdGUI_Message1 (pmWindow, 0, IDS_FILE_OPEN_FAILED_TITLE,
    	    			IDS_FILE_NOT_FOUND, pmPathName);
    	    }
    	    else
    	    {
    	        EdGUI_Message1 (pmWindow, 0, IDS_FILE_OPEN_FAILED_TITLE,
    	            IDS_FILE_OPEN_FAILED_MESSAGE, pmPathName, 
    	            EdFail_GetErrorMessage (myErrorBuffer), myErrorCode);
    	    }
    	}
    	return NULL;
    }
    
    // Get the file size
    myFileSize = GetFileSize (myFile, NULL);
    if (myFileSize == -1)
    {
    	EdGUI_Message1 (pmWindow, 0, IDS_FILE_READ_FAILED,
    	    IDS_CANT_GET_FILE_SIZE, pmPathName, 
    	    EdFail_GetErrorMessage (myErrorBuffer), GetLastError ());
	CloseHandle (myFile);
    	return NULL;
    }
    
    myBuffer = (char *) malloc (myFileSize + 3);
    myBuffer [myFileSize] = 0;
    if (ReadFile (myFile, myBuffer, myFileSize, &myBytesRead, NULL) == 0)
    {
    	EdGUI_Message1 (pmWindow, 0, IDS_FILE_READ_FAILED_TITLE,
    	    IDS_FILE_READ_FAILED, pmPathName, 
    	    EdFail_GetErrorMessage (myErrorBuffer), GetLastError ());
	CloseHandle (myFile);
	return NULL;
    }
    
    if (myFileSize != myBytesRead)
    {
    	EdGUI_Message1 (pmWindow, 0, IDS_FILE_READ_FAILED_TITLE,
    	    IDS_FILE_READ_SHORT, pmPathName, myBytesRead, myFileSize);
	CloseHandle (myFile);
	return NULL;
    }

    // Add a newline at the end if there isn't one
    if (myBuffer [myFileSize - 1] != '\n')
    {
    	myBuffer [myFileSize++] = '\r';
    	myBuffer [myFileSize++] = '\n';
    	myBuffer [myFileSize] = 0;
    }    	
    
    if (CloseHandle (myFile) == 0)
    {
    	EdGUI_Message1 (NULL, 0, IDS_FILE_WRITE_FAILED_TITLE, 
    	    IDS_CANT_CLOSE_FILE, pmPathName, 
    	    EdFail_GetErrorMessage (myErrorBuffer), GetLastError ());
    }
    
    if (pmFileSize != NULL)
    {
    	*pmFileSize = myFileSize;
    }
    
    return myBuffer;	
} // EdFile_ReadFileIntoBuffer


/************************************************************************/
/* EdFile_ReadLineFromBuffer						*/
/************************************************************************/
char 	*EdFile_ReadLineFromBuffer (char *pmBuffer, int pmFileSize, int pmRead)
{
    ReadBufferDataPtr	myPtr;
    char		*myStart;
    
    myPtr = stReadBufferDataHead;
    while (myPtr != NULL)
    {
	if (myPtr -> buffer == pmBuffer)
	{
	    break;
	}
	myPtr = myPtr -> next;
    }

    // Initialize the read, if necessary    
    if (pmRead == READ_FIRST)
    {
	if (myPtr == NULL)
	{
	    myPtr = (ReadBufferDataPtr) malloc (sizeof (ReadBufferData));
	    myPtr -> next = stReadBufferDataHead;
	    stReadBufferDataHead = myPtr;
	}
	myPtr -> buffer = pmBuffer;
    	myPtr -> bufPtr = pmBuffer;
    	myPtr -> end = myPtr -> bufPtr + pmFileSize;
    	myPtr -> ch = 0;
    	return NULL;
    }

    // Reset the zero'd char back to what is was
    if (myPtr -> ch != 0)
    {
    	*(myPtr -> charPtr) = myPtr -> ch;
    	myPtr -> ch = 0;
    }

    // If we've reached the end of the file, return NULL
    if (myPtr -> bufPtr >= myPtr -> end)
    {
        return NULL;
    }
    
    myStart = myPtr -> bufPtr;
            
    // Skip to end of line (indicated by \n, \r or \n\r)
    while ((myPtr -> bufPtr < myPtr -> end) && (*(myPtr -> bufPtr) != '\r') && 
	   (*(myPtr -> bufPtr) != '\n'))
    {
    	myPtr -> bufPtr++;
    }

    myPtr -> charPtr = myPtr -> bufPtr;
    myPtr -> ch = *(myPtr -> charPtr);
    *(myPtr -> charPtr) = 0;
    	
    if ((myPtr -> ch == '\r') && (*(myPtr -> bufPtr + 1) == '\n'))
    {
	myPtr -> bufPtr++;
    }
	
    if (myPtr -> bufPtr < myPtr -> end)
    {
	myPtr -> bufPtr++;
    }

    return myStart;
} // EdFile_ReadFromLineBuffer


/************************************************************************/
/* EdFile_SaveRTFToFile							*/
/************************************************************************/
BOOL	EdFile_SaveRTFToFile (HWND pmWindow, char *pmPathName, TextPtr pmText, 
			      int *pmBytesSaved, int *pmLinesSaved)
{
    ErrorBuffer	myErrorBuffer;
    HANDLE	myFile;
    int		mySaveTo;
    int		myCurrentPos, myIndentPos, myEOLPos;
    int		myStartOfIdentPos, myEndOfIdentPos;
    char	myTabBuffer[] = "\t";
    DWORD	myNumBytesWritten;
    int		myLineNumber;
    int		myLine;
        
    // Open the RTF file
    myFile = CreateFile (pmPathName, GENERIC_WRITE, 0, NULL,
    	CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (myFile == (HANDLE) INVALID_HANDLE_VALUE)
    {
    	int		myErrorCode = GetLastError ();
    	
        EdGUI_Message1 (pmWindow, 0, IDS_FILE_OPEN_FAILED_TITLE,
            IDS_FILE_OPEN_FAILED_MESSAGE, pmPathName, 
            EdFail_GetErrorMessage (myErrorBuffer), myErrorCode);
	return FALSE;
    }

    // Write the file
    if (pmBytesSaved != NULL)
    {
        *pmBytesSaved = 0;
    }
    if (pmLinesSaved != NULL)
    {
        *pmLinesSaved = 0;
    }
    
    // Write the RTF Header
    myLine = 0;
    while (stRTFHeader [myLine] [0] != 0)
    {
    	if (!MyWriteFile (pmWindow, pmPathName, myFile, stRTFHeader [myLine], 
    			  pmBytesSaved))
    	{
    	    return FALSE;
    	}
    	myLine++;
    }

    mySaveTo = pmText -> textLen;
    
    myCurrentPos = 0;
    myLineNumber = 1;
    while (myCurrentPos < mySaveTo)
    {
    	// Start of line, convert spaces to integers as necessary
    	myIndentPos = myCurrentPos;
    	while ((myIndentPos < mySaveTo) && 
    	       (pmText -> text [myIndentPos] == ' '))
    	{
    	    myIndentPos++;
    	}
    	
    	// Write out the tabs
    	while (myIndentPos - myCurrentPos >= gProperties.editIndentSize)
    	{
    	    myCurrentPos += gProperties.editIndentSize;
    	    if (!MyWriteFile (pmWindow, pmPathName, myFile, RTF_TAB, 
    	    		      pmBytesSaved))
    	    {
    	    	return FALSE;
    	    }
	}
	
	// Write out the rest of the line
	myEOLPos = myCurrentPos;
    	while ((myEOLPos < mySaveTo) && (pmText -> text [myEOLPos] != '\n'))
    	{
    	    myEOLPos++;
    	}
    	while (myEOLPos != myCurrentPos)
    	{
	    myStartOfIdentPos = myCurrentPos;
		
	    // Move myPos until reaching an identifier, a keyword, the end 
	    // of the line or until no more will fit on the line.
  	    while ((myStartOfIdentPos < myEOLPos) && 
	       (pmText -> textType [myStartOfIdentPos] != TEXT_IDENTIFIER) &&
	       (pmText -> textType [myStartOfIdentPos] != TEXT_KEYWORD))
	    {
	        myStartOfIdentPos++;
	    }
	    	
	    // There are some non-identifier, non-keyword stuff.  Print it
	    if (myStartOfIdentPos > myCurrentPos)
	    {
	    	int	myTempPos;
	    	
	    	myTempPos = myCurrentPos;
	    	
	    	while (myTempPos < myStartOfIdentPos)
	    	{
	    	    char	myChar = pmText -> text [myTempPos];
	    	    
	    	    if ((myChar == '{') || (myChar == '}') || 
	    	    	(myChar == '\\') || (myChar == '\t') ||
		        (myChar == '-'))
	    	    {
	    	    	// Write from myCurrentPos to myTempPos - 1
	    	    	if (myTempPos > myCurrentPos)
	    	    	{
		    	    // Write out the rest of the line
		    	    if (!WriteFile (myFile, 
		    	        &pmText -> text [myCurrentPos], 
		    	        myTempPos - myCurrentPos,
		    	        &myNumBytesWritten, NULL))
		    	    {
			    	// ERROR! Write failed!
		    	    	EdGUI_Message1 (pmWindow, 0, 
		    	    	    IDS_FILE_WRITE_FAILED_TITLE,
		    	    	    IDS_FILE_WRITE_FAILED, pmPathName, 
				    EdFail_GetErrorMessage (myErrorBuffer), 
				    GetLastError ());
				CloseHandle (myFile);				    
				return FALSE;
		    	    }
		    	    if (myNumBytesWritten != 
		    	    		(DWORD) (myTempPos - myCurrentPos))
		    	    {
			    	// ERROR! Write short!
		    	    	EdGUI_Message1 (pmWindow, 0, 
		    	    	    IDS_FILE_WRITE_FAILED_TITLE,
		    	    	    IDS_FILE_WRITE_SHORT, pmPathName, 
				    (int) myNumBytesWritten, 
				    (int) (myTempPos - myCurrentPos));
				CloseHandle (myFile);				    
				return FALSE;				    
			    }
			    if (pmBytesSaved != NULL)
			    {
		    	    	*pmBytesSaved += myNumBytesWritten;
		    	    }
	    		} // if
	    		myCurrentPos = myTempPos + 1;
	    	    	
	    	    	if (myChar == '{')
	    	    	{
	    	    	    if (!MyWriteFile (pmWindow, pmPathName, myFile, 
	    	    	    		      RTF_OPEN_BRACE, pmBytesSaved))
			    {
			    	return FALSE;
			    }	    	    	    		     
	    	    	}
		        else if (myChar == '}')
	    	    	{
	    	    	    if (!MyWriteFile (pmWindow, pmPathName, myFile, 
	    	    	    		      RTF_CLOSE_BRACE, pmBytesSaved))
			    {
			    	return FALSE;
			    }	    	    	    		     
	    	    	}
		    	else if (myChar == '\\')
	    	    	{
	    	    	    if (!MyWriteFile (pmWindow, pmPathName, myFile, 
	    	    	    		      RTF_BACKSLASH, pmBytesSaved))
			    {
			    	return FALSE;
			    }	    	    	    		     
	    	    	}
		    	else if (myChar == '\t')
	    	    	{
	    	    	    if (!MyWriteFile (pmWindow, pmPathName, myFile, 
	    	    	    		      RTF_TAB, pmBytesSaved))
			    {
			    	return FALSE;
			    }	    	    	    		     
	    	    	}
		    	else if (myChar == '-')
	    	    	{
			    if (pmText -> textType [myTempPos] == TEXT_NORMAL)
			    {
	    	    	    	if (!MyWriteFile (pmWindow, pmPathName, myFile, 
	    	    	    			  RTF_M_DASH, pmBytesSaved))
			    	{
			    	    return FALSE;
			    	}
	    	    	    }
	    	    	    else
	    	    	    {
	    	    	    	if (!MyWriteFile (pmWindow, pmPathName, myFile, 
	    	    	    			  RTF_N_DASH, pmBytesSaved))
			    	{
			    	    return FALSE;
			    	}
	    	    	    }
	    	    	}
	    	    } // if pmText -> textType [myTempPos] is special char
	    	    
	    	    myTempPos++;
		} // while
		
	    	// Write from myCurrentPos to myTempPos - 1
	    	if (myTempPos > myCurrentPos)
	    	{
		    // Write out the rest of the line
		    if (!WriteFile (myFile, &pmText -> text [myCurrentPos], 
		    	myTempPos - myCurrentPos, &myNumBytesWritten, NULL))
		    {
		    	// ERROR! Write failed!
	    	    	EdGUI_Message1 (pmWindow, 0, 
	    	    	    IDS_FILE_WRITE_FAILED_TITLE, IDS_FILE_WRITE_FAILED,
			    pmPathName, EdFail_GetErrorMessage (myErrorBuffer), 
			    GetLastError ());
			CloseHandle (myFile);				    
			return FALSE;
		    }
		    if (myNumBytesWritten != (DWORD) (myTempPos - myCurrentPos))
		    {
		    	// ERROR! Write short!
	    	    	EdGUI_Message1 (pmWindow, 0, 
	    	    	    IDS_FILE_WRITE_FAILED_TITLE, IDS_FILE_WRITE_SHORT,
			    pmPathName, (int) myNumBytesWritten, 
			    (int) (myTempPos - myCurrentPos));
			CloseHandle (myFile);				    
			return FALSE;				    
		    }
		    if (pmBytesSaved != NULL)
		    {
	   	    	*pmBytesSaved += myNumBytesWritten;
	    	    }
	    	    
	    	    myCurrentPos = myTempPos;
    		} // if
    	    } // if (myStartOfIdentPos > myCurrentPos)

	    // Exit loop if we're at the end of the line
    	    if (myEOLPos == myCurrentPos)
    	    {
    	    	break;
    	    }
    	    
    	    // We're now at the begining of an identifier or keyword
	    // Move myPos until reaching the end of the identifier or keyword
	    myEndOfIdentPos = myCurrentPos;
	    while ((myEndOfIdentPos < myEOLPos) && 
	        (pmText -> textType [myCurrentPos] == 
	            pmText -> textType [myEndOfIdentPos]))
	    {
	        myEndOfIdentPos++;
	    }
	    
	    if (pmText -> textType [myCurrentPos] == TEXT_IDENTIFIER)
	    {
	    	if (!MyWriteFile (pmWindow, pmPathName, myFile, RTF_ITALIC_TAG, 
	    			  pmBytesSaved))
		{
		    return FALSE;
		}	    			  
	    }
	    else
	    {
	    	if (!MyWriteFile (pmWindow, pmPathName, myFile, RTF_BOLD_TAG, 
	    			  pmBytesSaved))
		{
		    return FALSE;
		}	    			  
	    }
    	    // Write out the identifier
    	    if (!WriteFile (myFile, &pmText -> text [myCurrentPos], 
    	    	myEndOfIdentPos - myCurrentPos, &myNumBytesWritten, NULL))
    	    {
	    	// ERROR! Write failed!
    	    	EdGUI_Message1 (pmWindow, 0, 
    	    	    IDS_FILE_WRITE_FAILED_TITLE, IDS_FILE_WRITE_FAILED, 
    	    	    pmPathName, EdFail_GetErrorMessage (myErrorBuffer), 
    	    	    GetLastError ());
		CloseHandle (myFile);				    
		return FALSE;
    	    }
    	    if (myNumBytesWritten != (DWORD) (myEndOfIdentPos - myCurrentPos))
    	    {
	    	// ERROR! Write short!
    	    	EdGUI_Message1 (pmWindow, 0, IDS_FILE_WRITE_FAILED_TITLE, 
    	    	    IDS_FILE_WRITE_SHORT, pmPathName, (int) myNumBytesWritten, 
    	    	    (int) (myEndOfIdentPos - myCurrentPos));
		CloseHandle (myFile);				    
		return FALSE;				    
	    }
	    if (pmBytesSaved != NULL)
	    {
    	    	*pmBytesSaved += myNumBytesWritten;
    	    }
	    myCurrentPos = myEndOfIdentPos;
    	    if (!MyWriteFile (pmWindow, pmPathName, myFile, RTF_END_TAG, 
    	    		      pmBytesSaved))
    	    {
    	    	return FALSE;
    	    }
	} // while (myEOLPos != myPos)
	
	// Write out the newlines
    	while ((myCurrentPos < mySaveTo) && 
    	    (pmText -> text [myCurrentPos] == '\n'))
    	{
	    myCurrentPos++;
	    myLineNumber++;
	    if (!MyWriteFile (pmWindow, pmPathName, myFile, RTF_NEWLINE, 
	    		      pmBytesSaved))
	    {
	    	return FALSE;
	    }
	}
    } // while (myPos < mySaveTo)

    // Write the RTF Trailer
    if (!MyWriteFile (pmWindow, pmPathName, myFile, stRTFTrailer, pmBytesSaved))
    {
    	return FALSE;
    }
    
    // Write the file    
    if (CloseHandle (myFile) == 0)
    {
    	// ERROR! Close failed!
    	EdGUI_Message1 (pmWindow, 0, 
    	    IDS_FILE_CLOSE_FAILED_TITLE, IDS_FILE_CLOSE_FAILED, pmPathName, 
    	    EdFail_GetErrorMessage (myErrorBuffer), GetLastError ());
	return FALSE;
    }
    
    return TRUE;
} // EdFile_SaveRTFToFile


/************************************************************************/
/* EdFile_SaveTextToFile						*/
/************************************************************************/
BOOL	EdFile_SaveTextToFile (HWND pmWindow, char *pmPathName, TextPtr pmText, 
			       BOOL pmSelection, int *pmBytesSaved,
			       int *pmLinesSaved)
{
    BOOL	myConvertSpacesToTabs;
    int		mySelFrom, mySelTo;
    FilePath	myPathNameDir;
    FilePath	myTempPathName;
    ErrorBuffer	myErrorBuffer;
    HANDLE	myFile;
    int		mySaveFrom, mySaveTo;
    int		myPos, myIndentPos, myEOLPos;
    char	myTabBuffer[] = "\t";
    DWORD	myNumBytesWritten;
    int		myLineNumber;
    int		myBytesSaved = 0;
    DWORD	myFileSize;
    
    // We decide if we are converting spaces to tabs.  We do this
    // if the convert to tabs preference is true and we are either
    // saving the entire file, or a selection that starts at the beginning
    // of a line and ends at the end of a line.
    myConvertSpacesToTabs = FALSE;
    mySelFrom = min (pmText -> selFrom, pmText -> selTo);
    mySelTo = max (pmText -> selFrom, pmText -> selTo);
    if (gProperties.editLeadingSpacesToTabs)
    {
    	if (pmSelection)
    	{
	    
	    if (((mySelFrom == 0) || 
	         (pmText -> text [mySelFrom - 1] == '\n')) &&
	        (pmText -> text [mySelTo - 1] == '\n'))
	    {
    	    	myConvertSpacesToTabs = TRUE;
    	    }
    	}
    	else
    	{
    	    myConvertSpacesToTabs = TRUE;
    	}
    }
    
    // 
    // File write strategy: Write to a temporary file in the same directory.
    // Once it is successfully written, then either delete the real file and
    // rename the temp file, or delete the backup file, rename the real file to
    // the backup file and then rename the temp file to the real file.
    //
    // Addendum: Test that the file was actually written by comparing the file
    // length.  At least in one case, the system returned that the file was
    // written successfully, when actually the system (due to disk quotas) had
    // not actually written the file!
    //
    
    // Get the temporary file name
    EdFile_GetDirectoryFromPath (pmPathName, myPathNameDir);
    // This creates a zero length file.
    if (GetTempFileName (myPathNameDir, "rdy", 0, myTempPathName) == 0)
    {
    	EdGUI_Message1 (pmWindow, 0, IDS_TEMP_FILE_NAME_CREATE_TITLE,
    	    IDS_TEMP_FILE_NAME_CREATE_MSG,
    	    EdFail_GetErrorMessage (myErrorBuffer), GetLastError ());
	return FALSE;
    }
    
    // Open the temporary file
    myFile = CreateFile (myTempPathName, GENERIC_WRITE, 0, NULL,
    	TRUNCATE_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (myFile == (HANDLE) INVALID_HANDLE_VALUE)
    {
    	EdGUI_Message1 (pmWindow, 0, IDS_FILE_OPEN_FAILED_TITLE,
    	    IDS_FILE_OPEN_TEMP_FAILED, myTempPathName, 
    	    EdFail_GetErrorMessage (myErrorBuffer), GetLastError ());
	return FALSE;
    }

    // Write the file
    if (pmBytesSaved != NULL)
    {
        *pmBytesSaved = 0;
    }
    if (pmLinesSaved != NULL)
    {
        *pmLinesSaved = 0;
    }
    
    if (pmSelection)
    {
    	mySaveFrom = mySelFrom;
    	mySaveTo = mySelTo;
    }
    else
    {
    	mySaveFrom = 0;
    	mySaveTo = pmText -> textLen;
    }
    
    myPos = mySaveFrom;
    myLineNumber = 1;
    while (myPos < mySaveTo)
    {
    	// Start of line, convert spaces to integers as necessary
    	myIndentPos = myPos;
    	while (myConvertSpacesToTabs && (myIndentPos < mySaveTo) && 
    	       (pmText -> text [myIndentPos] == ' '))
    	{
    	    myIndentPos++;
    	}
    	// Write out the tabs
    	while (myIndentPos - myPos >= gProperties.editNumSpacesPerTab)
    	{
    	    myPos += gProperties.editNumSpacesPerTab;
    	    if (!WriteFile (myFile, myTabBuffer, 1, &myNumBytesWritten, NULL))
    	    {
	    	// ERROR! Write failed!
    	    	EdGUI_Message1 (pmWindow, 0, 
    	    	    IDS_FILE_WRITE_FAILED_TITLE, IDS_FILE_WRITE_FAILED, 
    	    	    pmPathName, EdFail_GetErrorMessage (myErrorBuffer), 
    	    	    GetLastError ());
		CloseHandle (myFile);				    
		return FALSE;
    	    }
    	    if(myNumBytesWritten != 1)
    	    {
	    	// ERROR! Write short!
    	    	EdGUI_Message1 (pmWindow, 0, IDS_FILE_WRITE_FAILED_TITLE, 
    	    	    IDS_FILE_WRITE_SHORT, pmPathName, 
    	    	    (int) myNumBytesWritten, 1);
		CloseHandle (myFile);				    
		return FALSE;				    
    	    }
    	    myBytesSaved++;
	}
	
	// Write out the rest of the line
	myEOLPos = myPos;
    	while ((myEOLPos < mySaveTo) && (pmText -> text [myEOLPos] != '\n'))
    	{
    	    myEOLPos++;
    	}
    	if (myEOLPos != myPos)
    	{
    	    // Write out the rest of the line
    	    if (!WriteFile (myFile, &pmText -> text [myPos], myEOLPos - myPos,
    	        &myNumBytesWritten, NULL))
    	    {
	    	// ERROR! Write failed!
    	    	EdGUI_Message1 (pmWindow, 0, 
    	    	    IDS_FILE_WRITE_FAILED_TITLE, IDS_FILE_WRITE_FAILED, 
    	    	    pmPathName, EdFail_GetErrorMessage (myErrorBuffer), 
    	    	    GetLastError ());
		CloseHandle (myFile);				    
		return FALSE;
    	    }
    	    if (myNumBytesWritten != (DWORD) (myEOLPos - myPos))
    	    {
	    	// ERROR! Write short!
    	    	EdGUI_Message1 (pmWindow, 0, 
    	    	    IDS_FILE_WRITE_FAILED_TITLE, IDS_FILE_WRITE_SHORT,
		    pmPathName, (int) myNumBytesWritten, 
		    (int) (myEOLPos - myPos));
		CloseHandle (myFile);				    
		return FALSE;				    
	    }
    	    myBytesSaved += myNumBytesWritten;
    	    myPos = myEOLPos;
	}
	
	// Write out the newlines
    	while ((myPos < mySaveTo) && (pmText -> text [myPos] == '\n'))
    	{
	    myPos++;
	    myLineNumber++;
    	    if (!WriteFile (myFile, NEW_LINE, strlen (NEW_LINE), 
    	        &myNumBytesWritten, NULL))
    	    {
	    	// ERROR! Write failed!
    	    	EdGUI_Message1 (pmWindow, 0, 
    	    	    IDS_FILE_WRITE_FAILED_TITLE, IDS_FILE_WRITE_FAILED, 
    	    	    pmPathName, EdFail_GetErrorMessage (myErrorBuffer), 
    	    	    GetLastError ());
		CloseHandle (myFile);				    
		return FALSE;
    	    }
    	    if (myNumBytesWritten != strlen (NEW_LINE))
    	    {
	    	// ERROR! Write short!
    	    	EdGUI_Message1 (pmWindow, 0, 
    	    	    IDS_FILE_WRITE_FAILED_TITLE, IDS_FILE_WRITE_SHORT,
		    pmPathName, (int) myNumBytesWritten, strlen (NEW_LINE));
		CloseHandle (myFile);				    
		return FALSE;				    
	    }
	    myBytesSaved += myNumBytesWritten;
	    if (pmLinesSaved != NULL)
	    {
	    	(*pmLinesSaved)++;
	    }
	}
    } // while (myPos < mySaveTo)

    // If our selection being saved doesn't end with a newline, add one
    if (pmSelection && (pmText -> text [mySelTo - 1] != '\n'))
    {
    	if (!WriteFile (myFile, NEW_LINE, strlen (NEW_LINE), 
    	    &myNumBytesWritten, NULL))
    	{
	    // ERROR! Write failed!
    	    EdGUI_Message1 (pmWindow, 0, 
    	    	IDS_FILE_WRITE_FAILED_TITLE, IDS_FILE_WRITE_FAILED, 
    	    	pmPathName, EdFail_GetErrorMessage (myErrorBuffer), 
    	    	GetLastError ());
	    CloseHandle (myFile);				    
	    return FALSE;
    	}
    	if (myNumBytesWritten != strlen (NEW_LINE))
    	{
	    // ERROR! Write short!
    	    EdGUI_Message1 (pmWindow, 0, 
    	    	IDS_FILE_WRITE_FAILED_TITLE, IDS_FILE_WRITE_SHORT,
		pmPathName, (int) myNumBytesWritten, strlen (NEW_LINE));
	    CloseHandle (myFile);				    
	    return FALSE;				    
	}
	myBytesSaved += myNumBytesWritten;
    	if (pmLinesSaved != NULL)
    	{
	    (*pmLinesSaved)++;
	}
    }
    
    // Write the file    
    if (CloseHandle (myFile) == 0)
    {
    	// ERROR! Close failed!
    	EdGUI_Message1 (pmWindow, 0, 
    	    IDS_FILE_CLOSE_FAILED_TITLE, IDS_FILE_CLOSE_FAILED, pmPathName, 
    	    EdFail_GetErrorMessage (myErrorBuffer), GetLastError ());
	return FALSE;
    }

    if (pmBytesSaved != NULL)
    {
    	*pmBytesSaved = myBytesSaved;
    }
    
    // Just in case, let's make certain the file actually was written properly.
    // We've some suspicion that occasionally the system reports that the file
    // was written successfully when it wasn't.  This especially applies when
    // we're facing disk quotas.
        // Open the file
    myFile = CreateFile (myTempPathName, GENERIC_READ, FILE_SHARE_READ, NULL,
    	OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
    	NULL);
    if (myFile == (HANDLE) INVALID_HANDLE_VALUE)
    {
	// ERROR! Unable to open the file we just wrote!
    	EdGUI_Message1 (pmWindow, 0, 
    	    IDS_FILE_WRITE_FAILED_TITLE, IDS_FILE_CHECK_FAILED, 1,
    	    pmPathName, myTempPathName, EdFail_GetErrorMessage (myErrorBuffer), 
    	    GetLastError ());
    	return FALSE;
    }
    
    // Get the file size
    myFileSize = GetFileSize (myFile, NULL);
    if (myFileSize == -1)
    {
    	EdGUI_Message1 (pmWindow, 0, 
    	    IDS_FILE_WRITE_FAILED_TITLE, IDS_FILE_CHECK_FAILED, 2,
    	    pmPathName, myTempPathName, EdFail_GetErrorMessage (myErrorBuffer), 
    	    GetLastError ());
	CloseHandle (myFile);
    	return FALSE;
    }

    if (CloseHandle (myFile) == 0)
    {
    	// ERROR! Close failed!
    	EdGUI_Message1 (pmWindow, 0, 
    	    IDS_FILE_WRITE_FAILED_TITLE, IDS_FILE_CHECK_FAILED, 3,
    	    pmPathName, myTempPathName, EdFail_GetErrorMessage (myErrorBuffer), 
    	    GetLastError ());
	return FALSE;
    }

    if ((int) myFileSize != myBytesSaved)
    {
    	// ERROR! Close failed!
    	if (EdGUI_Message1 (pmWindow, MB_YESNO | MB_DEFBUTTON2, 
    	    		    IDS_FILE_WRITE_FAILED_TITLE, IDS_FILE_SIZE_WRONG, 
		    	    pmPathName, myTempPathName, myFileSize, 
		    	    myBytesSaved) == IDNO)
	{		    	    
    	    return FALSE;
    	}
    }
    
    // We've successfully written to a temporary file.  Now rename it properly
    if (gProperties.editSaveBackup)
    {
        FilePath	myBackupPathName;
    	FilePath	myExtension;
    	
    	if (strcmp (MyGetBackupPathName (pmPathName, myBackupPathName, 
    			myExtension), ".BAK") != 0)
    	{
    	    // First, delete the backup file, if it exists
    	    if (!DeleteFile (myBackupPathName))
    	    {
    		int		myErrorCode = GetLastError ();
		
	    	if (myErrorCode != ERROR_FILE_NOT_FOUND)
	    	{
		    ErrorBuffer	myErrorBuffer;
		    
	    	    EdGUI_Message1 (pmWindow, 0, IDS_FILE_DELETE_FAILED_TITLE,
	    	    	IDS_FILE_DELETE_FAILED, myBackupPathName, 
	    	    	myTempPathName, EdFail_GetErrorMessage (myErrorBuffer), 
    		        myErrorCode);
    		    return FALSE;
    		}
    	    }
    	    
    	    // Second, rename the real file to the backup file name.
    	    if (!MoveFile (pmPathName, myBackupPathName))
    	    {
    		int	myErrorCode = GetLastError ();

	    	if (myErrorCode != ERROR_FILE_NOT_FOUND)
	    	{
	    	    EdGUI_Message1 (pmWindow, 0, IDS_FILE_RENAME_FAILED_TITLE,
	    	    	IDS_FILE_RENAME_FAILED, pmPathName, myBackupPathName, 
	    	    	myTempPathName, EdFail_GetErrorMessage (myErrorBuffer), 
    		        myErrorCode);
    		    return FALSE;
    		}
    	    }
    	    
    	    // Third, rename the temp file to the real file name
    	    if (!MoveFile (myTempPathName, pmPathName))
    	    {
    		int	myErrorCode = GetLastError ();
		
	    	if (myErrorCode != ERROR_FILE_NOT_FOUND)
	    	{
	    	    EdGUI_Message1 (pmWindow, 0, IDS_FILE_RENAME_FAILED_TITLE,
	    	    	IDS_FILE_RENAME_TEMP_FAILED, pmPathName, myTempPathName, 
	    	    	EdFail_GetErrorMessage (myErrorBuffer), myErrorCode);
    		    return FALSE;
    		}
   	    }
   	}
    }
    else
    {
    	// First, delete the old version of the file, if it exists
    	if (!DeleteFile (pmPathName))
    	{
    	    int	myErrorCode = GetLastError ();
	
	    if (myErrorCode != ERROR_FILE_NOT_FOUND)
	    {
	    	EdGUI_Message1 (pmWindow, 0, IDS_FILE_DELETE_FAILED_TITLE,
	    	    IDS_FILE_DELETE_FAILED, pmPathName, 
	    	    myTempPathName, EdFail_GetErrorMessage (myErrorBuffer), 
    		    myErrorCode);
    		    return FALSE;
    	    }
    	}
    	    
    	// Second, rename the temp file to the real file name
    	if (!MoveFile (myTempPathName, pmPathName))
    	{
    	    int	myErrorCode = GetLastError ();
	
	    if (myErrorCode != ERROR_FILE_NOT_FOUND)
	    {
	    	EdGUI_Message1 (pmWindow, 0, IDS_FILE_RENAME_FAILED_TITLE,
	    	    IDS_FILE_RENAME_TEMP_FAILED, pmPathName, myTempPathName, 
	    	    EdFail_GetErrorMessage (myErrorBuffer), myErrorCode);
    		return FALSE;
   	    }
   	}
    }
    
    return TRUE;
} // EdFile_SaveTextToFile


/************************************************************************/
/* EdFile_SetActiveDirectory						*/
/************************************************************************/
void	EdFile_SetActiveDirectory (const char *pmPathName, BOOL pmIsFilePath)
{
    char	*myPtr;
    
    if (pmPathName == NULL)
    {
    	return;
    }
    
    // What we're passed in is actually a file path, so eliminate the last
    // element to get the path.
    
    if (!pmIsFilePath)
    {
	strcpy (stActiveDirectory, pmPathName); 
	return;
    }

    myPtr = strrchr (pmPathName, '\\');
    if (myPtr - pmPathName < 2)
    {
    	return;
    }
    else if (stActiveDirectory - myPtr == 2)
    {
    	strncpy (stActiveDirectory, pmPathName, 3);
	stActiveDirectory [3] = 0;
    }
    else
    {
    	strncpy (stActiveDirectory, pmPathName, (myPtr - pmPathName));
	stActiveDirectory [myPtr - pmPathName] = 0;
    }
} // EdFile_SetActiveDirectory


/************************************************************************/
/* EdFile_SetCurrentDirectory						*/
/************************************************************************/
void	EdFile_SetCurrentDirectory (const char *pmDirectory)
{
    SetCurrentDirectory (pmDirectory);
} // EdFile_SetCurrentDirectory
#endif // #ifndef TPROLOG


/************************************************************************/
/* EdFile_Write								*/
/************************************************************************/
void	EdFile_Write (FileEntryPtr pmFile, const char *pmFormatString, ...)
{
    va_list	myArgList;
    char	myFullText [2048];

    // Create the string to be written
    va_start (myArgList, pmFormatString);
    wvsprintf (myFullText, pmFormatString, myArgList);
    va_end (myArgList);
    
    MyWriteFileEntry (pmFile, myFullText);
} // EdFile_Write


/************************************************************************/
/* EdFile_Writeln							*/
/************************************************************************/
void	EdFile_Writeln (FileEntryPtr pmFile, const char *pmFormatString, ...)
{
    va_list	myArgList;
    char	myFullText [2048];

    // Create the string to be written
    va_start (myArgList, pmFormatString);
    wvsprintf (myFullText, pmFormatString, myArgList);
    va_end (myArgList);
    
    MyWriteFileEntry (pmFile, myFullText);
    EdFile_WriteNL (pmFile);
} // EdFile_Writeln


/************************************************************************/
/* EdFile_WritelnString							*/
/************************************************************************/
void	EdFile_WritelnString (FileEntryPtr pmFile, const char *pmString)
{
    MyWriteFileEntry (pmFile, pmString);
    EdFile_WriteNL (pmFile);
} // EdFile_WritelnString


/************************************************************************/
/* EdFile_WriteNL							*/
/************************************************************************/
void	EdFile_WriteNL (FileEntryPtr pmFile)
{
    MyWriteFileEntry (pmFile, NEW_LINE);
} // EdFile_WriteNL


/************************************************************************/
/* EdFile_WriteString							*/
/************************************************************************/
void	EdFile_WriteString (FileEntryPtr pmFile, const char *pmString)
{
    MyWriteFileEntry (pmFile, pmString);
} // EdFile_WriteString


/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/
#ifndef TPROLOG
/************************************************************************/
/* MyConvertSlashToBackslash						*/
/************************************************************************/
static void	MyConvertSlashToBackslash (char *pmString)
{
    char    *myPtr = pmString;

    while (*myPtr != 0)
    {
	if (*myPtr == '/')
	{
	    *myPtr = '\\';
	}
	myPtr++;
    } // while
} // MyConvertSlashToBackslash


/************************************************************************/
/* MyGetBackupPathName							*/
/************************************************************************/
static char	*MyGetBackupPathName (char *pmPathName, 
				      char *pmPathWithBakExtension,
				      char *pmExtension)
{
    char	*myPtr = pmPathName + strlen (pmPathName) - 1;
    
    while ((myPtr > pmPathName) && (*myPtr != '.') && (*myPtr != '\\'))
        myPtr--;
    
    if (*myPtr == '.')
    {
    	strcpy (pmExtension, myPtr);
    	EdGlob_ConvertToUpperCase (pmExtension);
    	strncpy (pmPathWithBakExtension, pmPathName, (myPtr - pmPathName));
    	pmPathWithBakExtension [myPtr - pmPathName] = 0;
    	strcat (pmPathWithBakExtension, BACKUP_SUFFIX);
    }
    else
    {
    	pmExtension [0] = 0;
    	strcpy (pmPathWithBakExtension, pmPathName);
    	strcat (pmPathWithBakExtension, BACKUP_SUFFIX);
    }
    return pmExtension;
} // MyGetBackupPathName


/************************************************************************/
/* MyWriteFile								*/
/************************************************************************/
static BOOL	MyWriteFile (HWND pmWindow, char *pmPathName, HANDLE pmFile, 
			     char *pmString, int *pmBytesSaved)
{
    DWORD 	myNumBytesWritten, myLen;
    ErrorBuffer	myErrorBuffer;
    
    myLen = strlen (pmString);
    if (!WriteFile (pmFile, pmString, myLen, &myNumBytesWritten, NULL))
    {
    	// ERROR! Write failed!
    	EdGUI_Message1 (pmWindow, 0, 
    	    IDS_FILE_WRITE_FAILED_TITLE, IDS_FILE_WRITE_FAILED, pmPathName, 
    	    EdFail_GetErrorMessage (myErrorBuffer), GetLastError ());
	CloseHandle (pmFile);				    
	return FALSE;
    }
    if (myNumBytesWritten != myLen)
    {
    	// ERROR! Write short!
    	EdGUI_Message1 (pmWindow, 0, 
    	    IDS_FILE_WRITE_FAILED_TITLE, IDS_FILE_WRITE_SHORT, pmPathName, 
    	    (int) myNumBytesWritten, (int) myLen);
	CloseHandle (pmFile);				    
	return FALSE;				    
    }
    
    if (pmBytesSaved != NULL)
    {
    	*pmBytesSaved += myLen;
    }
    
    return TRUE;
} // MyWriteFile
#endif // #ifndef TPROLOG


/************************************************************************/
/* MyWriteFileEntry							*/
/************************************************************************/
static void	MyWriteFileEntry (FileEntryPtr pmFile, const char *pmString)
{
    DWORD	myNumBytesWritten;
    HANDLE	myHandle = pmFile -> handle;
    int		myErrorCode;
    ErrorBuffer	myErrorBuffer;

    // If an error has already occurred writing this file out, don't
    // bother trying again.    
    if (pmFile -> errorFound)
    {
    	return;
    }
    
    if (!WriteFile (myHandle, pmString, strlen (pmString), 
    		    &myNumBytesWritten, NULL))
    {
    	// ERROR! Write failed!
    	myErrorCode = GetLastError ();
    	EdGUI_Message1 (NULL, 0, IDS_FILE_WRITE_FAILED_TITLE,
		    	IDS_FILE_WRITE_FAILED1, pmFile -> pathName, 
			EdFail_GetErrorMessage (myErrorBuffer), 
			myErrorCode);
    	pmFile -> errorFound = TRUE;
    	return;
    }
    if (myNumBytesWritten != strlen (pmString))
    {
    	// ERROR! Write short!
    	EdGUI_Message1 (NULL, 0, IDS_FILE_WRITE_FAILED_TITLE,
		    	IDS_FILE_WRITE_FAILED1, pmFile -> pathName, 
		    	myNumBytesWritten, strlen (pmString));
    	pmFile -> errorFound = TRUE;
    	return;
    }
    if (pmFile -> alwaysFlush)
    {
        if (!FlushFileBuffers (myHandle))
    	{
    	    // ERROR! Flush failed!
    	    myErrorCode = GetLastError ();
    	    EdGUI_Message1 (NULL, 0, IDS_FILE_WRITE_FAILED_TITLE,
		    	    IDS_FILE_FLUSH_FAILED, pmFile -> pathName, 
			    EdFail_GetErrorMessage (myErrorBuffer), 
			    myErrorCode);
    	    pmFile -> errorFound = TRUE;
    	    return;
    	}
    }
} // MyWriteFileEntry
