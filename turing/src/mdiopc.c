/************/
/* mdiopc.c */
/************/

/*******************/
/* System includes */
/*******************/
#include <windows.h>
#include <conio.h>

/****************/
/* Self include */
/****************/
#include "mdiopc.h"

/******************/
/* Other includes */
/******************/
#include "miotypes.h"
#include "mio.h"
#include "mioerr.h"

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
typedef unsigned char (_stdcall *ReadPortAddressProc) (unsigned long pmPort);
typedef void (_stdcall *WritePortAddressProc) (unsigned long pmPort, 
					       unsigned char pmValue);
typedef BOOL (_stdcall *Toolhelp32ReadProcessMemoryProc) (
					DWORD pmProcessID,
  					LPCVOID pmBaseAddress,
  					LPVOID pmBuffer,
  					DWORD pmRead,
  					LPDWORD pmNumberOfBytesRead);

/**********************/
/* External variables */
/**********************/
					      
/********************/
/* Static constants */
/********************/

/********************/
/* Static variables */
/********************/
static BOOL 			stIsDLPortIOInitialized = FALSE;
static BOOL 			stUseDriver = FALSE;
static long 			stReadPortAddress [4];
static long 			stWritePortAddress [4];
static WritePortAddressProc 	stWritePortAddressProc;
static ReadPortAddressProc 	stReadPortAddressProc;

/******************************/
/* Static callback procedures */
/******************************/

/*********************/
/* Static procedures */
/*********************/
static long	MyGetParallelControllerKey (char *pmKey);
static long	MyGetPortAddressFromMemory (int pmPort);
static long	MyGetPortAddressFromRegistry (int pmPort);
static void	MyDeterminePortAddress (int pmPort);
static void 	MyInitializeDLPortIO (void);

/***********************/
/* External procedures */
/***********************/
/************************************************************************/
/* MDIOPC_InPort							*/
/************************************************************************/
int	MDIOPC_InPort (int pmPortAddress)
{
    int	myPortValue;
    
    if (!stIsDLPortIOInitialized)
    {
	MyInitializeDLPortIO ();
    }

    if (stUseDriver)
    {
	myPortValue = (stReadPortAddressProc) (pmPortAddress);
    }
    else
    {
	myPortValue = _inp ((unsigned short) pmPortAddress);
    }
    
    return myPortValue;
} // MDIOPC_InPort


/************************************************************************/
/* MDIOPC_OutPort							*/
/************************************************************************/
void	MDIOPC_OutPort (int pmPortAddress, unsigned char pmValue)
{
    if (!stIsDLPortIOInitialized)
    {
	MyInitializeDLPortIO ();
    }
    
    if (stUseDriver)
    {
        (stWritePortAddressProc) (pmPortAddress, pmValue);
    }
    else
    {
	_outp ((unsigned short) pmPortAddress, pmValue);
    }
} // MDIOPC_OutPort


/************************************************************************/
/* MDIOPC_ParallelGet							*/
/************************************************************************/
int	MDIOPC_ParallelGet (int pmPort)
{
    int	myPortValue;
    
    if (!stIsDLPortIOInitialized)
    {
	MyInitializeDLPortIO ();
    }

    if ((pmPort < 1) || (pmPort > 4))
    {
    	ABORT_WITH_ERRNO (E_PC_ILLEGAL_PORT_ERROR);
    }
    
    pmPort--; // Ports go from 0 - 3
    
    if (stWritePortAddress [pmPort] == 0)
    {
    	MyDeterminePortAddress (pmPort);
    }
    
    if (stWritePortAddress [pmPort] == -1)
    {
    	MIOError_AbortWithErrMsg (E_PC_INIT_FAILED, 
	    			  "Unable to find I/O port address for port %d", 
    		  		  pmPort + 1);
    }
    
    if (stUseDriver)
    {
	myPortValue = (stReadPortAddressProc) (stReadPortAddress [pmPort]);
    }
    else
    {
	myPortValue = _inp ((unsigned short) stReadPortAddress [pmPort]);
    }
    
    // This sets all the non-input bits to zero.
    myPortValue &= 0xF8;
    
    return myPortValue;
} // MDIOPC_ParallelGet


/************************************************************************/
/* MDIOPC_ParallelPut							*/
/************************************************************************/
void	MDIOPC_ParallelPut (int pmPort, unsigned char pmValue)
{
    if (!stIsDLPortIOInitialized)
    {
	MyInitializeDLPortIO ();
    }

    if ((pmPort < 1) || (pmPort > 4))
    {
    	ABORT_WITH_ERRNO (E_PC_ILLEGAL_PORT_ERROR);
    }
    
    pmPort--; // Ports go from 0 - 3

    if (stWritePortAddress [pmPort] == 0)
    {
    	MyDeterminePortAddress (pmPort);
    }
    
    if (stWritePortAddress [pmPort] == -1)
    {
    	MIOError_AbortWithErrMsg (E_PC_INIT_FAILED, 
	    			  "Unable to find I/O port address for port %d", 
    		  		  pmPort + 1);
    }
    
    if (stUseDriver)
    {
        (stWritePortAddressProc) (stWritePortAddress [pmPort], pmValue);
    }
    else
    {
	_outp ((unsigned short) stWritePortAddress [pmPort], pmValue);
    }
} // MIOPC_ParallelPut


/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/
/************************************************************************/
/* MyGetParallelControllerKey						*/
/************************************************************************/
static long	MyGetParallelControllerKey (char *pmParControlKey)
{
    long	myResult;
    HKEY	myRegKey, myRegKey1, myRegKey2;
    char	myKey [256], myKey1 [256], myKey2 [256];
    char	myData [256];
    DWORD	mySize;
    DWORD	myIndex1, myIndex2, myIndex3;
    FILETIME	myLastWriteTime;
    
    *pmParControlKey = 0;
    
    strcpy (myKey, "HARDWARE\\DESCRIPTION\\System");
    
    myResult = RegOpenKeyEx (HKEY_LOCAL_MACHINE, myKey, 0, KEY_READ, &myRegKey);
    if (myResult != ERROR_SUCCESS)
    {
    	ABORT_WITH_ERRMSG (E_PC_INIT_FAILED, 
    			  "Unable to find I/O port address [1]");
    	return -1;
    }

    for (myIndex1 = 0 ; myIndex1 <= 10 ; myIndex1++)
    {
    	mySize = sizeof (myData);
    	myResult = RegEnumKeyEx (myRegKey, myIndex1, myData, &mySize, NULL, 
    				 NULL, NULL, &myLastWriteTime);
	if (myResult == ERROR_SUCCESS) // ERROR_SUCCESS 1
	{
	    wsprintf (myKey1, "%s\\%s", myKey, myData);
	    myResult = RegOpenKeyEx (HKEY_LOCAL_MACHINE, myKey1, 0, KEY_READ,
	    			     &myRegKey1);
	    if (myResult != ERROR_SUCCESS)
	    {
	    	RegCloseKey (myRegKey);
	    	ABORT_WITH_ERRMSG (E_PC_INIT_FAILED, 
    				  "Unable to find I/O port address [2]");
	    	return -1;
	    }

	    for (myIndex2 = 0 ; myIndex2 <= 10 ; myIndex2++)
	    {
	    	mySize = sizeof (myData);
	    	myResult = RegEnumKeyEx (myRegKey1, myIndex2, myData, &mySize, 
	    				 NULL, NULL, NULL, &myLastWriteTime);
		if (myResult == ERROR_SUCCESS) // ERROR_SUCCESS 2
		{
		    wsprintf (myKey2, "%s\\%s", myKey1, myData);
		    myResult = RegOpenKeyEx (HKEY_LOCAL_MACHINE, myKey2, 0, 
		    			     KEY_READ, &myRegKey2);
		    if (myResult != ERROR_SUCCESS)
		    {
		    	RegCloseKey (myRegKey);
		    	RegCloseKey (myRegKey1);
		    	ABORT_WITH_ERRMSG (E_PC_INIT_FAILED, 
    					"Unable to find I/O port address [3]");
		    	return -1;
		    }
		    
		    for (myIndex3 = 0 ; myIndex3 <= 10 ; myIndex3++)
		    {
		    	mySize = sizeof (myData);
		    	myResult = RegEnumKeyEx (myRegKey2, myIndex3, myData, 
		    				 &mySize, NULL, NULL, NULL, 
		    				 &myLastWriteTime);
			if (myResult == ERROR_SUCCESS) // ERROR_SUCCESS 3
			{
			    if (strcmp (myData, "ParallelController") == 0)
			    {
			    	wsprintf (pmParControlKey, "%s\\%s", 
			    		  myKey2, myData);
			    	RegCloseKey (myRegKey);
			    	RegCloseKey (myRegKey1);
			    	RegCloseKey (myRegKey2);
			        return 0;
			    }
			} // ERROR_SUCCESS 3
		    } // for
		    RegCloseKey (myRegKey2);
		} // ERROR_SUCCESS 2
	    } // for
	    RegCloseKey (myRegKey1);
	} //  // ERROR_SUCCESS 1
    } // for
    RegCloseKey (myRegKey);
        
    ABORT_WITH_ERRMSG (E_PC_INIT_FAILED, "Unable to find I/O port address [4]");
    
    return -1;
} // MyGetParallelControllerKey


/************************************************************************/
/* MyGetPortAddressFromMemory						*/
/************************************************************************/
static long	MyGetPortAddressFromMemory (int pmPort)
{
    HINSTANCE				myDLL;
    Toolhelp32ReadProcessMemoryProc	myToolhelp32ReadProcessMemoryProc;
    int					myPortAddress [] = {0, 0, 0, 0, 0};
    BOOL				myReturn = 0;
    DWORD				myBytesRead = 0;
    
    myDLL = LoadLibrary ("kernel32");
    if (myDLL == NULL)
    {
    	ABORT_WITH_ERRMSG (E_PC_INIT_FAILED, 
    			  "Unable to find I/O port address [5]");
    	return -1;
    }

    myToolhelp32ReadProcessMemoryProc = 
    	(Toolhelp32ReadProcessMemoryProc) GetProcAddress (myDLL, 
					      "Toolhelp32ReadProcessMemory");
    if (myToolhelp32ReadProcessMemoryProc == NULL)
    {
    	ABORT_WITH_ERRMSG (E_PC_INIT_FAILED, 
    			  "Unable to find I/O port address [6]");
    	return -1;
    }

    myReturn = (myToolhelp32ReadProcessMemoryProc) (0, (LPCVOID *) 0x408,
    		myPortAddress, 8, NULL);
    		
    FreeLibrary (myDLL);
    
    if (myReturn == 0)
    {
    	ABORT_WITH_ERRMSG (E_PC_INIT_FAILED, 
    			  "Unable to find I/O port address [7]");
    	return -1;
    }
    
    if (myPortAddress [pmPort] <= 0)
    {
    	MIOError_AbortWithErrMsg (E_PC_INIT_FAILED, 
	    	"Unable to find I/O port address for port %d [8]", pmPort + 1);
    	return -1;
    }
    
    if (myPortAddress [pmPort] >= 0x1000)
    {
    	MIOError_AbortWithErrMsg (E_PC_INIT_FAILED, 
	    	"Unable to find I/O port address for port %d [9]", pmPort + 1);
    	return -1;
    }
    
    return myPortAddress [pmPort];
} // MyGetPortAddressFromMemory


/************************************************************************/
/* MyGetPortAddressFromRegistry						*/
/************************************************************************/
static long	MyGetPortAddressFromRegistry (int pmPort)
{
    int		myResult;
    HKEY	myRegKey;
    char	myKey [255];
    char	myData [255];
    DWORD	mySize;
    DWORD	myType;
    
    myResult = MyGetParallelControllerKey (myKey);
    if (myResult < 0)
    {
    	ABORT_WITH_ERRMSG (E_PC_INIT_FAILED, 
    			  "Unable to find I/O port address [10]");
    	return -1;
    }
    
    wsprintf (myData, "%s\\%d", myKey, pmPort);
    
    myResult = RegOpenKeyEx (HKEY_LOCAL_MACHINE, myData, 0, KEY_READ, 
    			     &myRegKey);
    if (myResult != ERROR_SUCCESS)
    {
    	MIOError_AbortWithErrMsg (E_PC_INIT_FAILED, 
	    	"Unable to find I/O port address for port %d [11]", pmPort + 1);
    	return -1;
    }
    
    mySize = sizeof (myData);
    myType = REG_BINARY;
    
    myResult = RegQueryValueEx (myRegKey, 		// Handle to key to qry
    				"Configuration Data",	// Name of value to qry
    				NULL, &myType, (unsigned char *) myData,
    				&mySize);
    if (myResult != ERROR_SUCCESS)
    {
    	MIOError_AbortWithErrMsg (E_PC_INIT_FAILED, 
	    	"Unable to find I/O port address for port %d [12]", pmPort + 1);
    	return -1;
    }

    RegCloseKey (myRegKey);
    
    return myData [0x14] | myData [0x15] << 8;    				
} // MyGetPortAddressFromRegistry


/************************************************************************/
/* MyDeterminePortAddress						*/
/*									*/
/* This code is taken from Jon Axelson's Parallel Port FAQ at 		*/
/* www.lvr.com/jansfaq.html.						*/
/************************************************************************/
static void	MyDeterminePortAddress (int pmPort)
{
    OSVERSIONINFO	myOSVersionInfo;
    long		myAddr;
    
    ZeroMemory (&myOSVersionInfo, sizeof (myOSVersionInfo));
    myOSVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx (&myOSVersionInfo);

    if (MIO_parallelIOPort != 0)
    {
    	myAddr = MIO_parallelIOPort;
    }
    else
    {
    	if (myOSVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
    	{
    	    myAddr = MyGetPortAddressFromMemory (pmPort);
    	}
    	else
    	{
    	    myAddr = MyGetPortAddressFromRegistry (pmPort);
    	}
    }
    
    if (myAddr != -1)
    {
    	stWritePortAddress [pmPort] = myAddr;
    	stReadPortAddress [pmPort] = myAddr + 1;
    }
    else
    {
    	stWritePortAddress [pmPort] = -1;
    	stReadPortAddress [pmPort] = -1;
    }
} // MyDeterminePortAddress


/************************************************************************/
/* MyInitializeDLPortIO							*/
/*									*/
/* This module knows that the DLPortIO.DLL is located at		*/
/*									*/
/*		[turing]\support\bin\dlportio.dll			*/
/************************************************************************/
static void	MyInitializeDLPortIO (void)
{
    OSVERSIONINFO	myOSVersionInfo;
    char		myDLPortIODLLPath [4096];
    char		*myPtr;
    HINSTANCE		myLibrary;
    HKEY		myRegKey;
    long		myResult;
    
    ZeroMemory (&myOSVersionInfo, sizeof (myOSVersionInfo));
    myOSVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx (&myOSVersionInfo);

    // Initialize these to zero.  Figure out the value when called.
    stWritePortAddress [0] = 0;
    stWritePortAddress [1] = 0;
    stWritePortAddress [2] = 0;
    stWritePortAddress [3] = 0;
    stReadPortAddress [0] = 0;
    stReadPortAddress [1] = 0;
    stReadPortAddress [2] = 0;
    stReadPortAddress [3] = 0;

    if (myOSVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) 
    {
        stIsDLPortIOInitialized = TRUE;
	return;
    }

    stUseDriver = TRUE;

    GetModuleFileName (NULL, myDLPortIODLLPath, 4095);
    myPtr = strrchr (myDLPortIODLLPath, '\\');
    if (myPtr == NULL)
    {
    	// TW ERROR! No backslash in pathname!
    }
    myPtr++;
    *myPtr = 0;
    
    strcat (myDLPortIODLLPath, "Support\\Bin\\DLPORTIO.DLL");
    
    myLibrary = LoadLibrary (myDLPortIODLLPath);
    if (myLibrary == NULL)
    {
	int	myErrorCode = GetLastError ();
	char	*myErrorMessage;
	char	myMessage [1024];
	
	if (myErrorCode = ERROR_MOD_NOT_FOUND)
	{
	    wsprintf (myMessage, 
	    	      "Parallel port initialization failed. "
	    	      "Unable to open required DLL at \"%s\"",
	    	      myDLPortIODLLPath);
	}
	else
	{
	    FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM | 
	    	FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
    	    	NULL, myErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
    	    	(LPTSTR) &myErrorMessage, 0, NULL);
    	    strcpy (myMessage, myErrorMessage);
	    LocalFree (myErrorMessage);
    	}
    	
    	ABORT_WITH_ERRMSG (E_PC_INIT_FAILED, myMessage);
    	return;
    }

    stReadPortAddressProc = 
	(ReadPortAddressProc) GetProcAddress (myLibrary, 
					      "DlPortReadPortUchar");
    if (stReadPortAddressProc == NULL)
    {
    	ABORT_WITH_ERRMSG (E_PC_INIT_FAILED, 
    			  "Unable to find PortRead routine in DLL");
	return;    			 
    }
    
    stWritePortAddressProc = 
	(WritePortAddressProc) GetProcAddress (myLibrary, 
					       "DlPortWritePortUchar");
    if (stWritePortAddressProc == NULL)
    {
    	ABORT_WITH_ERRMSG (E_PC_INIT_FAILED, 
    			  "Unable to find PortWrite routine in DLL");
	return;    			 
    }
    
    stIsDLPortIOInitialized = TRUE;

    // Check to make certain driver is installed
    myResult = RegOpenKeyEx (HKEY_LOCAL_MACHINE, 
                             "System\\CurrentControlSet\\Services\\DLPortIO",
                             0, KEY_READ, &myRegKey);
    if (myResult != ERROR_SUCCESS)
    {
    	ABORT_WITH_ERRMSG (E_PC_INIT_FAILED, 
    			  "Unable to find I/O port device driver. DLPortIO.sys "
    			  "device driver must be correctly installed");
    	return;
    }
    RegCloseKey (myRegKey);
} // MyInitializeDLPortIO