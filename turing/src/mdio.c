/**********/
/* mdio.c */
/**********/

/****************/
/* Self include */
/****************/
#include "mdio.h"

/******************/
/* Other includes */
/******************/
#include "mioerr.h"

#include "mio.h"

#include "edint.h"

/*******************/
/* System includes */
/*******************/
#include <windows.h>
#include <crtdbg.h>
#include "psapi.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
// Defined in winuser.h for W2K and above
#define GR_GDIOBJECTS     0       /* Count of GDI objects */
#define GR_USEROBJECTS    1       /* Count of USER objects */

/********************/
/* Global variables */
/********************/

/*********/
/* Types */
/*********/
typedef struct MSWindowsFilePtr
{
    HANDLE	handle;
    BOOL	isBinary;
    int		ungetChar;
} MSWindowsFilePtr;

typedef BOOL (_stdcall *GetProcessMemoryInfoProc) (HANDLE Process,
		PPROCESS_MEMORY_COUNTERS ppsmemCounters, DWORD cb);
typedef DWORD (_stdcall *GetGuiResourcesProc) (HANDLE hProcess, DWORD uiFlags);


/**********************/
/* External variables */
/**********************/
extern void	Ed_CreateMemoryLogFile (void);

/********************/
/* Static constants */
/********************/

/********************/
/* Static variables */
/********************/
static GetProcessMemoryInfoProc	stGetProcessMemoryInfoProc = NULL;
static GetGuiResourcesProc	stGetGuiResourcesProc = NULL;

/******************************/
/* Static callback procedures */
/******************************/

/*********************/
/* Static procedures */
/*********************/

/***********************/
/* External procedures */
/***********************/
extern void TL_TLI_TLISS ();extern void TL_TLI_TLIPS ();


/************************************************************************/
/* MDIO_Init								*/
/************************************************************************/
void	MDIO_Init (void)
{
    OSVERSIONINFO	myOSVersionInfo;
    HMODULE		myDLL;
    
    // Find out which version of Windows we're using.
    ZeroMemory (&myOSVersionInfo, sizeof (myOSVersionInfo));
    myOSVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx (&myOSVersionInfo);

    // To avoid trying to call a non-existent function, we make all the calls
    // by loading up the function's address
    if ((myOSVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT) &&
        (myOSVersionInfo.dwMajorVersion >= 4))
    {
    	myDLL = LoadLibrary ("psapi.dll");
    	if (myDLL != NULL)
    	{
    	    stGetProcessMemoryInfoProc = 
    		(GetProcessMemoryInfoProc) GetProcAddress (myDLL, 
    							"GetProcessMemoryInfo");
    	}    							
    }    					

    if ((myOSVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT) &&
        (myOSVersionInfo.dwMajorVersion >= 5))
    {
    	myDLL = LoadLibrary ("user32.dll");
    	if (myDLL != NULL)
    	{
    	    stGetGuiResourcesProc = 
    		(GetGuiResourcesProc) GetProcAddress (myDLL, 
    						      "GetGuiResources");
    	}    							
    }    					
} // MDIO_Init


/************************************************************************/
/* MDIO_Beep								*/
/************************************************************************/
void	MDIO_Beep (void)
{
    MessageBeep (MB_ICONEXCLAMATION);
} // MDIO_Beep


/************************************************************************/
/* MDIO_DiskFile_Close							*/
/************************************************************************/
BOOL	MDIO_DiskFile_Close (void *pmFilePtr)
{
    MSWindowsFilePtr 	*myPtr = (MSWindowsFilePtr *) pmFilePtr;
    BOOL	myResult;
    
    myResult = CloseHandle (myPtr -> handle);
    
    if (!myResult)
    {
    	MDIO_ProcessMSWindowsError (GetLastError ());
    }
    
    free (pmFilePtr);
    
    if (myResult)
    {
    	return TRUE;
    }
    else
    {
    	return FALSE;
    }
} // MDIO_DiskFile_Close


/************************************************************************/
/* MDIO_DiskFile_Flush							*/
/************************************************************************/
int	MDIO_DiskFile_Flush (void *pmFilePtr)
{
    MSWindowsFilePtr 	*myPtr = (MSWindowsFilePtr *) pmFilePtr;
    HANDLE		myHandle = myPtr -> handle;

    if (!FlushFileBuffers (myHandle))
    {
    	MDIO_ProcessMSWindowsError (GetLastError ());
    	return -1;
    }
    
    return 0;
} // MDIO_DiskFile_Flush


/************************************************************************/
/* MDIO_DiskFile_Getc							*/
/************************************************************************/
int	MDIO_DiskFile_Getc (void *pmFilePtr)
{
    MSWindowsFilePtr 	*myPtr = (MSWindowsFilePtr *) pmFilePtr;
    HANDLE		myHandle = myPtr -> handle;
    unsigned char	myChar;
    DWORD		myNumBytesRead;
    BOOL		myResult;
    
    if (myPtr -> ungetChar != EOF)
    {
    	myChar = myPtr -> ungetChar;
    	myPtr -> ungetChar = EOF;
    	return myChar;
    }
    else
    {
    	myResult = ReadFile (myHandle, &myChar, 1, &myNumBytesRead, NULL);
    
    	if (!myResult || (myNumBytesRead != 1))
    	{
    	    MDIO_ProcessMSWindowsError (GetLastError ());
    	    return EOF;
    	}
    }

    if (!myPtr -> isBinary)
    {
    	// This allows us to translate '\r', '\n' or '\r\n' to be end of line
    	if (myChar == '\r')
    	{
	    myResult = ReadFile (myHandle, &myChar, 1, &myNumBytesRead, NULL);
    
	    if (!myResult || (myNumBytesRead != 1))
	    {
    		MDIO_ProcessMSWindowsError (GetLastError ());
	    	return EOF;
    	    }
	        
	    if (myChar != '\n')
	    {
	        myPtr -> ungetChar = myChar;
	    }
	    
	    myChar = '\n';
	}    	    	
    } // if (!pmIsBinary)
    
    return (int) myChar;    	    		 
} // MDIO_DiskFile_Getc


/************************************************************************/
/* MDIO_DiskFile_Open							*/
/*									*/
/* mode is one of 							*/
/*    "r" - read text mode						*/
/*    "w" - write text mode						*/
/*    "r+" - reading/writing text file - file must exist		*/
/*    "w+" - reading/writing text file - create empty file		*/
/*    "rb" - read binary mode						*/
/*    "wb" - write binary mode						*/
/*    "r+b" - reading/writing binary file - file must exist		*/
/*    "w+b" - reading/writing binary file - create empty file		*/
/************************************************************************/
void	*MDIO_DiskFile_Open (const char *pmPathName, char *pmMode)
{
    BOOL		myIsBinary;
    BOOL		myMustExist;
    BOOL		myOpenForRead;
    BOOL		myOpenForWrite;
    int			myFileAccess = 0, myShareAccess, myOpenKind;
    HANDLE		myHandle;
    char		*mode;
    MSWindowsFilePtr	*myMSWindowsFilePtr;
    
    myIsBinary = (strchr (pmMode, 'b') != NULL);
    myMustExist = (strchr (pmMode, 'r') != NULL);
    myOpenForRead = ((strchr (pmMode, 'r') != NULL) || 
    		     (strchr (pmMode, '+') != NULL));
    myOpenForWrite = ((strchr (pmMode, 'w') != NULL) || 
    		      (strchr (pmMode, '+') != NULL));

    if (myOpenForRead)
    {
    	myFileAccess |= GENERIC_READ;
    }
    if (myOpenForWrite)
    {
    	myFileAccess |= GENERIC_WRITE;
    }
    
    if (myOpenForWrite)
    {
    	myShareAccess = FILE_SHARE_READ;
    }
    else
    {
    	myShareAccess = FILE_SHARE_READ | FILE_SHARE_WRITE;
    }
    
    if (myMustExist)
    {
    	myOpenKind = OPEN_EXISTING;
    }
    else
    {
    	myOpenKind = CREATE_ALWAYS;
    }

    myHandle = CreateFile (pmPathName, myFileAccess, myShareAccess, NULL, 
    			   myOpenKind, 
    			   FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
		    	   NULL);
    if (myHandle == (HANDLE) INVALID_HANDLE_VALUE)
    {
    	DWORD	myError = GetLastError ();
    	
    	if (myError == ERROR_FILE_NOT_FOUND)
    	{
	    SET_ERRMSG (E_FSYS_FILE_NOT_FOUND, "File \"%s\" not found", 
	    		pmPathName);
    	}
    	else if (myError == ERROR_PATH_NOT_FOUND)
    	{
	    SET_ERRMSG (E_FSYS_PATH_NOT_FOUND, 
	    		"Path not found (could not find the directory "
	    	 	"that file \"%s\" is in)", pmPathName);
    	}
    	else if (myError == ERROR_SHARING_VIOLATION)
    	{
	    SET_ERRMSG (E_FSYS_SHARING_VIOLATION, 
	    		"File \"%s\" is already open (may not be opened twice)", 
	    		pmPathName);
    	}
    	else
    	{
    	    MDIO_ProcessMSWindowsError (myError);
    	}
    	return NULL;
    }

    if (myOpenForRead)
    {
	if (myOpenForWrite)
	{
	    if (myIsBinary)
		mode = "read, write";
	    else
		mode = "get, put";
	}
	else
	{
	    if (myIsBinary)
		mode = "read";
	    else
		mode = "get";
	}
    }
    else
    {
	if (myIsBinary)
	    mode = "write";
	else
	    mode = "put";
    }
    MIO_MoreInfo ("File \"%s\" opened for %s", pmPathName, mode);

    myMSWindowsFilePtr = (MSWindowsFilePtr *) malloc (sizeof (MSWindowsFilePtr));
    myMSWindowsFilePtr -> handle = myHandle;
    myMSWindowsFilePtr -> isBinary = myIsBinary;
    myMSWindowsFilePtr -> ungetChar = EOF;

    return myMSWindowsFilePtr;
} // MDIO_DiskFile_Open


/************************************************************************/
/* MDIO_DiskFile_Putc							*/
/************************************************************************/
int	MDIO_DiskFile_Putc (OOTint pmChar, void *pmFilePtr)
{
    MSWindowsFilePtr 	*myPtr = (MSWindowsFilePtr *) pmFilePtr;
    HANDLE		myHandle = myPtr -> handle;
    DWORD		myBytesWritten;
    BOOL		myResult;
    
    // Okay, this is pretty perverse, but what happens if a user does a
    // eof at the file position and then does a write.  Without the
    // following code, the output will be displaced a character
    if (myPtr -> ungetChar != EOF)
    {
        SetFilePointer (myHandle, -1, NULL, FILE_CURRENT);
	myPtr -> ungetChar = EOF;
    }

    // If we're writing to a text file, convert \n's to \r\n's
    if ((pmChar == '\n') && !myPtr -> isBinary)
    {
    	char	myReturn = '\r';
    	
    	myResult = WriteFile (myHandle, &myReturn, 1, &myBytesWritten, NULL);
    
    	if (!myResult || (myBytesWritten != 1))
    	{
    	    MDIO_ProcessMSWindowsError (GetLastError ());
    	    return EOF;
        }
    }
    	
    myResult = WriteFile (myHandle, &pmChar, 1, &myBytesWritten, NULL);
    
    if (!myResult || (myBytesWritten != 1))
    {
    	MDIO_ProcessMSWindowsError (GetLastError ());
    	return EOF;
    }

    return pmChar;
} // MDIO_DiskFile_Putc


/************************************************************************/
/* MDIO_DiskFile_Puts							*/
/************************************************************************/
int	MDIO_DiskFile_Puts (char *pmString, void *pmFilePtr)
{
    MSWindowsFilePtr 	*myPtr = (MSWindowsFilePtr *) pmFilePtr;
    HANDLE		myHandle = myPtr -> handle;
    DWORD		myLen, myBytesWritten;
    BOOL		myResult;
    char		*myStringPtr = pmString, *myNLPtr;
    
    // Okay, this is pretty perverse, but what happens if a user does a
    // eof at the file position and then does a write.  Without the
    // following code, the output will be displaced a character
    if (myPtr -> ungetChar != EOF)
    {
        SetFilePointer (myHandle, -1, NULL, FILE_CURRENT);
	myPtr -> ungetChar = EOF;
    }

    while ((myNLPtr = strchr (myStringPtr, '\n')) != NULL)
    {
    	//
    	// Write up to the new line
    	//
    	myLen = (DWORD) (myNLPtr - myStringPtr);
    	myResult = WriteFile (myHandle, myStringPtr, myLen, &myBytesWritten, 
    			      NULL);
    
    	if (!myResult || (myBytesWritten != myLen))
    	{
    	    MDIO_ProcessMSWindowsError (GetLastError ());
    	    return EOF;
        }
        
        // Write the \r\n
        MDIO_DiskFile_Putc (*myNLPtr, pmFilePtr);
        
        myStringPtr = myNLPtr + 1;
    } // while (strchr (myStringPtr, '\n') != NULL)

    myLen = strlen (myStringPtr);
    myResult = WriteFile (myHandle, myStringPtr, myLen, &myBytesWritten, NULL);
    
    if (!myResult || (myBytesWritten != myLen))
    {
    	MDIO_ProcessMSWindowsError (GetLastError ());
    	return EOF;
    }

    return 0;
} // MDIO_DiskFile_Puts


/************************************************************************/
/* MDIO_DiskFile_Read							*/
/************************************************************************/
int	MDIO_DiskFile_Read (BYTE *pmBuffer, int pmSize, void *pmFilePtr)
{
    MSWindowsFilePtr 	*myPtr = (MSWindowsFilePtr *) pmFilePtr;
    HANDLE		myHandle = myPtr -> handle;
    DWORD		myBytesRead, myUngotBytesRead = 0;
    BOOL		myResult;
    
    if (myPtr -> ungetChar != EOF)
    {
    	*pmBuffer++ = myPtr -> ungetChar;
    	myPtr -> ungetChar = EOF;
	pmSize--;
	myUngotBytesRead++;
	if (pmSize == 0)
	{
	    return 1;
	}
    }

    myResult = ReadFile (myHandle, pmBuffer, pmSize, &myBytesRead, NULL);
    
    if (!myResult || ((int) myBytesRead != pmSize))
    {
    	MDIO_ProcessMSWindowsError (GetLastError ());
    	return EOF;
    }

    return myBytesRead + myUngotBytesRead;
} // MDIO_DiskFile_Read


/************************************************************************/
/* MDIO_DiskFile_ReadLine						*/
/************************************************************************/
char	*MDIO_DiskFile_ReadLine (char *pmBuffer, int pmSize, void *pmFilePtr)
{
    char		myChar;
    int			myPos = 0;

    while (myPos < pmSize - 1)
    {
	myChar = MDIO_DiskFile_Getc (pmFilePtr);

	if (myChar == EOF)
	{
	    return NULL;
	}

	pmBuffer [myPos++] = myChar;

	if (myChar == '\n')
	{
	    break;
	}
    }

    pmBuffer [myPos++] = 0;
    return pmBuffer;
} // MDIO_DiskFile_ReadLine


/************************************************************************/
/* MDIO_DiskFile_Seek							*/
/************************************************************************/
void	MDIO_DiskFile_Seek (int pmLocation, void *pmFilePtr, int pmWhence)
{
    MSWindowsFilePtr 	*myPtr = (MSWindowsFilePtr *) pmFilePtr;
    HANDLE		myHandle = myPtr -> handle;

    // The ungotChar is irrelevant now.
    myPtr -> ungetChar = EOF;

    if (pmWhence == SEEK_BEGIN)
    {
        SetFilePointer (myHandle, pmLocation, NULL, FILE_BEGIN);
    }
    else if (pmWhence == SEEK_CURRENT)
    {
        SetFilePointer (myHandle, pmLocation, NULL, FILE_CURRENT);
    }
    else if (pmWhence == SEEK_END)
    {
        SetFilePointer (myHandle, pmLocation, NULL, FILE_END);
    }
} // MDIO_DiskFile_Seek


/************************************************************************/
/* MDIO_DiskFile_Tell							*/
/************************************************************************/
int	MDIO_DiskFile_Tell (void *pmFilePtr)
{
    MSWindowsFilePtr 	*myPtr = (MSWindowsFilePtr *) pmFilePtr;
    HANDLE		myHandle = myPtr -> handle;
    int			myPos;
    
    myPos = SetFilePointer (myHandle, 0, NULL, FILE_CURRENT); 
    if (myPtr -> ungetChar != EOF)
    {
    	myPos--;
    }
    
    return myPos;
} // MDIO_DiskFile_Tell


/************************************************************************/
/* MDIO_DiskFile_Ungetc							*/
/************************************************************************/
void	MDIO_DiskFile_Ungetc (OOTint pmChar, void *pmFilePtr)
{
    MSWindowsFilePtr 	*myPtr = (MSWindowsFilePtr *) pmFilePtr;
    
    myPtr -> ungetChar = pmChar;
} // MDIO_DiskFile_Ungetc


/************************************************************************/
/* MDIO_DiskFile_Write							*/
/************************************************************************/
int	MDIO_DiskFile_Write (void *pmBuffer, int pmSize, void *pmFilePtr)
{
    MSWindowsFilePtr 	*myPtr = (MSWindowsFilePtr *) pmFilePtr;
    HANDLE		myHandle = myPtr -> handle;
    DWORD		myBytesWritten;
    BOOL		myResult;
    
    // Okay, this is pretty perverse, but what happens if a user does a
    // eof at the file position and then does a write.  Without the
    // following code, the output will be displaced a character
    if (myPtr -> ungetChar != EOF)
    {
        SetFilePointer (myHandle, -1, NULL, FILE_CURRENT);
	myPtr -> ungetChar = EOF;
    }

    myResult = WriteFile (myHandle, pmBuffer, pmSize, &myBytesWritten, NULL);
    
    if (!myResult || ((int) myBytesWritten != pmSize))
    {
    	MDIO_ProcessMSWindowsError (GetLastError ());
    	return EOF;
    }

    return myBytesWritten;
} // MDIO_DiskFile_Write


/************************************************************************/
/* MDIO_GetResourceCount						*/
/************************************************************************/
void	MDIO_GetSystemInfo (int *pmResourceCount, DWORD *pmMemUsed)
{
    MEMORYSTATUS		myMemoryStatus;
    PROCESS_MEMORY_COUNTERS	myMemoryCounter;
    int				myResources = 0;
    DWORD			myMemUsed = 0;
			    
    if (stGetGuiResourcesProc != NULL)
    {
    	myResources = 
    		(stGetGuiResourcesProc) (GetCurrentProcess (), GR_GDIOBJECTS) +
		(stGetGuiResourcesProc) (GetCurrentProcess (), GR_USEROBJECTS);
    }

    // Get memory usage    
    if (stGetProcessMemoryInfoProc != NULL)
    {
	if ((stGetProcessMemoryInfoProc) (GetCurrentProcess (), 
					  &myMemoryCounter,
				          sizeof (PROCESS_MEMORY_COUNTERS)))
	{
    	    myMemUsed = myMemoryCounter.WorkingSetSize;
	}
    }
    if (myMemUsed == 0)
    {
    	GlobalMemoryStatus (&myMemoryStatus);
    	myMemUsed = myMemoryStatus.dwTotalPhys - myMemoryStatus.dwAvailPhys;
    }
    
    if (pmResourceCount != NULL) *pmResourceCount = myResources;
    if (pmMemUsed != NULL) *pmMemUsed = myMemUsed / 1024;
} // MDIO_GetSystemInfo


/************************************************************************/
/* MDIO_MemoryLeakTest							*/
/*									*/
/* Output any memory allocations that occurred between the last time	*/
/* this was called and this time.  Note, there is an allocation of 12	*/
/* bytes to start with the data "Memory Test" to ensure that we are	*/
/* getting something.							*/
/************************************************************************/
/*void	MDIO_MemoryLeakTest (void)
{
    #define MEM_STRING	"Memory Test"

    _CrtMemState	myMemCheckpoint, myDiffMem;
    char		*myPtr;

    static BOOL		myStMemCheckOccurred = FALSE;
    static _CrtMemState	myStMemCheckpoint;
    static int		myStLastResourceUsed, myStCurrentResourceUsed;
    static DWORD	myStLastMemUsed, myStCurrentMemUsed;

    if (myStMemCheckOccurred)
    {        	    
	_CrtMemDumpAllObjectsSince (&myStMemCheckpoint);
	_CrtMemCheckpoint (&myMemCheckpoint);    
	_CrtMemDifference (&myDiffMem, &myStMemCheckpoint, &myMemCheckpoint);
        _CrtMemDumpStatistics (&myDiffMem);
	myStMemCheckpoint = myMemCheckpoint;
    }
    else
    {
	Ed_CreateMemoryLogFile ();
	_CrtMemCheckpoint (&myStMemCheckpoint);    
	myStMemCheckOccurred = TRUE;
    }

    // We allocate a few bytes to make it clear that this routine works
    myPtr = malloc (strlen (MEM_STRING) + 1);
    strcpy (myPtr, MEM_STRING);

    _CrtMemDumpAllObjectsSince (&myStMemCheckpoint);

    // If we need to break at a memory allocation, do it here.
//    _crtBreakAlloc = 4849;

    // This call also allows us (in a debugger, to see the resource usage
    myStLastResourceUsed = myStCurrentResourceUsed;
    myStLastMemUsed = myStCurrentMemUsed;
    MDIO_GetSystemInfo (&myStCurrentResourceUsed, &myStCurrentMemUsed);
} // MDIO_MemoryLeakTest
*/

/************************************************************************/
/* MDIO_OutputSystemInfo						*/
/*									*/
/* Output the resource and memory usage of the program			*/
/************************************************************************/
void	MDIO_OutputSystemInfo (const char *pmMessage)
{
    int 	myResCount;
    DWORD	myMemUsed;
    char	myMsg [1024];
    	
    MDIO_GetSystemInfo (&myResCount, &myMemUsed);
    MDIO_sprintf (myMsg, "%s: R:%d M:%u\n", pmMessage, myResCount, myMemUsed);
    
    TL_TLI_TLISS (0, (short) 2);
    TL_TLI_TLIPS (0, myMsg, (short) 0);
} // MDIO_OutputSystemInfo


/************************************************************************/
/* MDIO_ProcessMSWindowsError						*/
/************************************************************************/
void	MDIO_ProcessMSWindowsError (DWORD pmMSWindowsErrorCode)
{
    LPVOID myMessageBuffer;

    switch (pmMSWindowsErrorCode)
    {
    	case ERROR_FILE_NOT_FOUND:
	    SET_ERRNO(E_FSYS_FILE_NOT_FOUND);
    	    break;
    	case ERROR_PATH_NOT_FOUND:
	    SET_ERRNO(E_FSYS_PATH_NOT_FOUND);
    	    break;
    	case ERROR_TOO_MANY_OPEN_FILES:
	    SET_ERRNO(E_FSYS_TOO_MANY_OPEN_FILES);
    	    break;
    	case ERROR_ACCESS_DENIED:
	    SET_ERRNO(E_FSYS_ACCESS_DENIED);
    	    break;
    	case ERROR_INVALID_HANDLE:
	    SET_ERRNO(E_FSYS_HANDLE_INVALID);
    	    break;
    	case ERROR_NOT_ENOUGH_MEMORY:
    	case ERROR_OUTOFMEMORY:
	    SET_ERRNO(E_FSYS_INSUFFICIENT_MEMORY);
    	    break;
    	case ERROR_BAD_ENVIRONMENT:
	    SET_ERRNO(E_FSYS_ENVIRONMENT_INVALID);
    	    break;
    	case ERROR_BAD_FORMAT:
	    SET_ERRNO(E_FSYS_FORMAT_INVALID);
    	    break;
    	case ERROR_INVALID_ACCESS:
	    SET_ERRNO(E_FSYS_ACCESS_CODE_INVALID);
    	    break;
    	case ERROR_INVALID_DATA:
	    SET_ERRNO(E_FSYS_DATA_INVALID);
    	    break;
    	case ERROR_INVALID_DRIVE:
	    SET_ERRNO(E_FSYS_DISK_DRIVE_INVALID);
    	    break;
    	case ERROR_NOT_SAME_DEVICE:
	    SET_ERRNO(E_FSYS_NOT_SAME_DEVICE);
    	    break;
    	case ERROR_WRITE_PROTECT:
	    SET_ERRNO(E_FSYS_WRITE_PROTECTED_DISK);
    	    break;
    	case ERROR_NOT_READY:
	    SET_ERRNO(E_FSYS_DRIVE_NOT_READY);
    	    break;
    	case ERROR_CRC:
	    SET_ERRNO(E_FSYS_DATA_ERROR_CRC);
    	    break;
    	case ERROR_READ_FAULT:
	    SET_ERRNO(E_FSYS_READ_ERROR);
    	    break;
    	case ERROR_WRITE_FAULT:
	    SET_ERRNO(E_FSYS_WRITE_ERROR);
    	    break;
    	case ERROR_SECTOR_NOT_FOUND:
	    SET_ERRNO(E_FSYS_SECTOR_NOT_FOUND);
    	    break;
    	case ERROR_GEN_FAILURE:
	    SET_ERRNO(E_FSYS_GENERAL_FAULT);
    	    break;
    	case ERROR_SHARING_VIOLATION:
    	    SET_ERRNO(E_FSYS_SHARING_VIOLATION);
    	    break;
    	default:
	    if (!FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM | 
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, pmMSWindowsErrorCode, 
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
				(LPTSTR) &myMessageBuffer, 0, NULL))
	    {
		SET_ERRMSG (E_FSYS_UNKNOWN_ERROR, 
	        	    "Unknown file system error (Windows error = %d)", 
	    		    pmMSWindowsErrorCode);
	    }
	    else
	    {
		// There is often a newline in the error.  Remove it.
		char	*myPtr = myMessageBuffer;
		while (*myPtr != 0)
		{
		    if ((*myPtr == '\n') || (*myPtr == '\r'))
		    {
			*myPtr = ' ';
		    }
		    myPtr++;
		} // while
		myPtr--;
		while (*myPtr == ' ')
		{
		    *myPtr-- = 0;
		} // while
		SET_ERRMSG (E_FSYS_UNKNOWN_ERROR, 
	        	    "%s (Windows error #%d)", 
	    		    myMessageBuffer, pmMSWindowsErrorCode);
		LocalFree (myMessageBuffer);
	    }
    	    break;
    } // switch
} // MDIO_ProcessMSWindowsError
/*

7 The storage control blocks were destroyed.  		ERROR_ARENA_TRASHED 
9 The storage control block address is invalid.  	ERROR_INVALID_BLOCK 
16 The directory cannot be removed.  			ERROR_CURRENT_DIRECTORY 
18 There are no more files.  				ERROR_NO_MORE_FILES 
20 The system cannot find the device specified.  	ERROR_BAD_UNIT 
22 The device does not recognize the command.  		ERROR_BAD_COMMAND 
24 The program issued a command but the command length is incorrect.  ERROR_BAD_LENGTH 
25 The drive cannot locate a specific area or track on the disk.  ERROR_SEEK 
26 The specified disk or diskette cannot be accessed.  	ERROR_NOT_DOS_DISK 
28 The printer is out of paper.  			ERROR_OUT_OF_PAPER 
32 The process cannot access the file because it is being used by another process.  		ERROR_SHARING_VIOLATION 
33 The process cannot access the file because another process has locked a portion of the file. ERROR_LOCK_VIOLATION 
34 The wrong diskette is in the drive. Insert %2 (Volume Serial Number: %3) into drive %1.  	ERROR_WRONG_DISK 
36 Too many files opened for sharing.  			ERROR_SHARING_BUFFER_EXCEEDED 
38 Reached the end of the file.  			ERROR_HANDLE_EOF 
39 The disk is full.  					ERROR_HANDLE_DISK_FULL 

E_FSYS_READ_ONLY	E_FSYS_NO_MORE_FILES		E_FSYS_NO_DISK_IN_DRIVE		
E_FSYS_UNKNOWN_COMMAND	E_FSYS_SEEK_ERROR		E_FSYS_UNKNOWN_MEDIA_TYPE			
E_FSYS_PRINTER_OUT_OF_PAPER	
						
E_FSYS_NO_SPACE_LEFT		E_FSYS_FILE_EXISTS		E_FSYS_DIR_EXISTS		
E_FSYS_NOT_A_FILE		E_FSYS_CANT_READ_FROM_WDWFILE	E_FSYS_CANT_WRITE_TO_WDWFILE	
E_FSYS_PATHNAME_MODIFIED	E_FSYS_CANT_GO_UP_FROM_ROOT_DIR	E_FSYS_BAD_CHAR_IN_PATHNAME	
E_FSYS_MALFORMED_PATHNAME	E_FSYS_FUNCTION_NUMBER_INVALID	E_FSYS_MCB_DESTROYED		
E_FSYS_MCB_INVALID		E_FSYS_BAD_REQ_LEN_STRUCT	E_FSYS_UNKNOWN_UNIT		
*/


/************************************************************************/
/* MDIO_PutMIOWindowsOnTop						*/
/*									*/
/* This goes through the window list and places all the MIO windows on	*/
/* the top, preserving the Z order relative to themselves.		*/
/************************************************************************/
void	MDIO_PutMIOWindowsOnTop (void)
{
    // Go through the list of windows, putting all the MIO windows
    // on the top.
    HWND    myTopMIOWindow = NULL;
    HWND    myWindow = GetTopWindow (GetDesktopWindow ());
    
    while (myWindow != NULL)
    {
	int myWindowType = MIOWin_GetWindowType (myWindow);

        if ((myWindowType == WINDOW_TYPE_MIO_TEXT) ||
	    (myWindowType == WINDOW_TYPE_MIO_GRAPHICS) ||
	    (myWindowType == WINDOW_TYPE_MIO_DIALOG))
	{
	    if (myTopMIOWindow == NULL)
	    {
		SetForegroundWindow (myWindow);
	    }
	    else
	    {
		SetWindowPos (myWindow, myTopMIOWindow, 0, 0, 0, 0,
			      SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
	    }
	    myTopMIOWindow = myWindow;
	}
	myWindow = GetNextWindow (myWindow, GW_HWNDNEXT);
    } // while
} // MDIO_PutMIOWindowsOnTop


/************************************************************************/
/* MDIO_sprintf								*/
/*									*/
/* This is included so that we can use sprintf in MIO routines without	*/
/* knowing how it's implemented on each individual system.		*/
/************************************************************************/
void	MDIO_sprintf (char *pmOutputStr, const char *pmFormat, ...)
{
    va_list	myArgList;

    va_start (myArgList, pmFormat);
    wvsprintf (pmOutputStr, pmFormat, myArgList);
    va_end (myArgList);
} // MDIO_sprintf


/************************************************************************/
/* MDIO_vsprintf							*/
/*									*/
/* This is included so that we can use sprintf in MIO routines without	*/
/* knowing how it's implemented on each individual system.		*/
/************************************************************************/
void	MDIO_vsprintf (char *pmOutputStr, const char *pmFormat, 
		       va_list pmArgList)
{
    wvsprintf (pmOutputStr, pmFormat, pmArgList);
} // MDIO_sprintf


/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/
