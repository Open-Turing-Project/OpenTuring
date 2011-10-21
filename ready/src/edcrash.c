/*************/
/* edcrash.c */
/*************/
// Left out - Set true path for symbols
// Load alternate for DebugHlp

/*******************/
/* System includes */
/*******************/
#include <windows.h>
#include <psapi.h>

/****************/
/* Self include */
/****************/
#include "edcrash.h"

/******************/
/* Other includes */
/******************/
#include "edfail.h"
#include "edfile.h"
#include "edglob.h"

#include "eddialogs.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
#define SYMBOL_BUFFER_SIZE	 1024
#define MAX_MODULES		  100
#define AUTO_LOAD_MODULES	FALSE

/********************/
/* Global variables */
/********************/

/*********/
/* Types */
/*********/
typedef struct ModuleInfo
{
    HMODULE	module;
    DWORD	moduleBaseAddress;
    DWORD	moduleSize;
    char	*moduleName;
    char	*modulePath;
} ModuleInfo;

//
// Found in DbgHelp.dll
//
// SymCleanup()
typedef BOOL (__stdcall *DbgHelp_SymCleanUp) (HANDLE pmProcess);
// SymFunctionTableAccess()
typedef PVOID (__stdcall *DbgHelp_SymFuncTableAcc) (HANDLE pmProcess, 
						    DWORD pmAddressBase);

// SymGetLineFromAddr()
typedef BOOL (__stdcall *DbgHelp_SymGetLineFromAddr) (HANDLE pmProcess, 
		DWORD pmAddress, PDWORD pmDisplacement, PIMAGEHLP_LINE pmLine);
// SymGetModuleBase()
typedef DWORD (__stdcall *DbgHelp_SymGetModBase) (HANDLE pmProcess, 
						  DWORD pmAddress);
/*
// SymGetModuleInfo()
typedef BOOL (__stdcall *DbgHelp_SymGetModInfo) (HANDLE pmProcess, 
		DWORD pmAddress, PIMAGEHLP_MODULE stModuleInfo);
*/
// SymGetOptions()
typedef DWORD (__stdcall *DbgHelp_SymGetOpt) (VOID);
// SymGetSymFromAddr()
typedef BOOL (__stdcall *DbgHelp_SymGetSymFromAddr) (HANDLE pmProcess, 
		DWORD pmAddress, PDWORD pmDisplacement, 
		PIMAGEHLP_SYMBOL Symbol);
// SymInitialize()
typedef BOOL (__stdcall *DbgHelp_SymInit) (HANDLE hProcess, PSTR UserSearchPath, 
					   BOOL fInvadeProcess);
// SymLoadModule()
typedef DWORD (__stdcall *DbgHelp_SymLoadMod) (HANDLE pmProcess, HANDLE pmFile,
		PSTR pmImageName, PSTR pmModuleName, DWORD pmBaseOfDll, 
		DWORD pmSizeOfDll);
// SymSetOptions()
typedef DWORD (__stdcall *DbgHelp_SymSetOpt) (DWORD SymOptions);
// StackWalk()
typedef BOOL (__stdcall *DbgHelp_StackWalk) (DWORD MachineType, HANDLE hProcess,
		HANDLE hThread, LPSTACKFRAME StackFrame, PVOID ContextRecord,
		PREAD_PROCESS_MEMORY_ROUTINE ReadMemoryRoutine,
		PFUNCTION_TABLE_ACCESS_ROUTINE FunctionTableAccessRoutine,
		PGET_MODULE_BASE_ROUTINE GetModuleBaseRoutine,
		PTRANSLATE_ADDRESS_ROUTINE TranslateAddress);

//
// Found in PSAPI.dll
//
typedef BOOL (WINAPI *PSAPI_EnumProcMods) (HANDLE pmProcess, 
		HMODULE *pmModule, DWORD pmCB, LPDWORD pmCBNeeded);
typedef DWORD (WINAPI *PSAPI_GetModBaseName) (HANDLE pmProcess, 
		HMODULE pmModule, LPTSTR pmBaseName, DWORD pmNSize);
typedef DWORD (WINAPI *PSAPI_GetModFileName) (HANDLE pmProcess, 
		HMODULE pmModule, LPTSTR pmFilename, DWORD pmNSize);
typedef BOOL (WINAPI *PSAPI_GetModInfo) (HANDLE pmProcess, HMODULE pmModule,
		LPMODULEINFO pmInfo, DWORD pmSize);

//
// Found in ToolHelp.dll
//
typedef BOOL (WINAPI *ToolHelp_ModuleWalk) (HANDLE pmSnapshot,
                                    	    LPMODULEENTRY32 pmModuleEntry);
typedef BOOL (WINAPI *ToolHelp_ThreadWalk) (HANDLE pmSnapshot,
	                                    LPTHREADENTRY32 pmThreadEntry);
typedef BOOL (WINAPI *ToolHelp_ProcessWalk) (HANDLE pmSnapshot,
	                                     LPPROCESSENTRY32 pmProcessEntry);
typedef HANDLE (WINAPI *ToolHelp_CreateSnap) (DWORD pmFlags, DWORD pmProcessID);


/**********************/
/* External variables */
/**********************/

/********************/
/* Static constants */
/********************/

/********************/
/* Static variables */
/********************/
static BOOL			stCrashInited = FALSE;
static BOOL			stInCrashHandler;
static char			*stRegisterBuffer = NULL;
static char			*stDumpBuffer = NULL, *stDumpBufferPtr;
static int			stDumpBufferLeft;
static char			stSymbolBuffer [SYMBOL_BUFFER_SIZE];
static IMAGEHLP_LINE		stLineNumberInfo;
static CONTEXT			stThreadContext;
static STACKFRAME		stStackFrame;
static ModuleInfo		stModuleInfo [MAX_MODULES];
static HMODULE			stModule [MAX_MODULES];
static int			stNumModules;

// This is used by a variety of functions whenever a path is required.
static FilePath			stPath; 

static BOOL			stIsNT, stIsNT4;
static HINSTANCE		stDebugHelpDLL = NULL;
static HINSTANCE		stPSAPIDLL = NULL;
static HINSTANCE		stKernel32DLL = NULL;
static HANDLE			stCurrentProcess;
static HANDLE			stCurrentProcessHandle;
static DWORD			stCurrentProcessID;

// Symbols loaded from DbgHelp.dll
static DbgHelp_SymInit		stSymInitializeProc = NULL;
static DbgHelp_SymLoadMod	stSymLoadModProc = NULL;
static DbgHelp_SymGetOpt	stSymGetOptProc = NULL;
static DbgHelp_SymSetOpt	stSymSetOptProc = NULL;
static DbgHelp_StackWalk	stStackWalkProc = NULL;
static DbgHelp_SymFuncTableAcc	stSymFuncTableAccProc = NULL;
static DbgHelp_SymGetModBase	stSymGetModBaseProc = NULL;
static DbgHelp_SymGetSymFromAddr	stSymGetSymFromAddrProc = NULL;
static DbgHelp_SymGetLineFromAddr	stSymGetLineFromAddrProc = NULL;

// Symbols loaded from PSAPI.dll
static PSAPI_EnumProcMods	stEnumProcModsProc = NULL;
static PSAPI_GetModBaseName	stGetModBaseNameProc = NULL;
static PSAPI_GetModFileName	stGetModFileNameProc = NULL;
static PSAPI_GetModInfo		stGetModInfoProc = NULL;

// Symbols loaded from ToolHelp.dll
static ToolHelp_CreateSnap	stCreateSnapProc = NULL;
static ToolHelp_ModuleWalk	stModFirstProc = NULL;
static ToolHelp_ModuleWalk	stModNextProc = NULL;
static ToolHelp_ProcessWalk	stProcFirstProc = NULL;
static ToolHelp_ProcessWalk	stProcNextProc = NULL;
static ToolHelp_ThreadWalk	stThreadFirstProc = NULL;
static ToolHelp_ThreadWalk	stThreadNextProc = NULL;


/******************************/
/* Static callback procedures */
/******************************/

/*********************/
/* Static procedures */
/*********************/
static void	MyAddToBuffer (char *pmString,  ...);
static BOOL	MyGetLoadedModules (int *pmModuleCount);
static BOOL	MyInitializeDbgHelp (void);
static BOOL	MyInitializePSAPI (void);
static BOOL	MyInitializeSymbols (void);
static BOOL	MyInitializeToolHelp (void);
static BOOL __stdcall 	MyReadProcessMemory (HANDLE pmHandle, 
					     LPCVOID lpBaseAddress,
                                      	     LPVOID lpBuffer, DWORD nSize,
                                      	     LPDWORD lpNumberOfBytesRead);

/***********************/
/* External procedures */
/***********************/
/************************************************************************/
/* EdCrash_Init								*/
/************************************************************************/
BOOL	EdCrash_Init (void)
{
    OSVERSIONINFO	myOSVersionInfo;

    ZeroMemory (&myOSVersionInfo, sizeof (myOSVersionInfo));
    myOSVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx (&myOSVersionInfo);

    if (myOSVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
    {
    	stIsNT = FALSE;
    }
    else
    {
    	stIsNT = TRUE;
    	if (myOSVersionInfo.dwMajorVersion == 4)
    	{
    	    stIsNT4 = TRUE;
    	}
    	else
    	{
    	    stIsNT4 = FALSE;
    	}
    }

    stCrashInited = TRUE;
    
    return TRUE;    	
} // EdCrash_Init


/************************************************************************/
/* EdCrash_Finalize							*/
/************************************************************************/
void	EdCrash_Finalize (void)
{
} // EdCrash_Finalize


/************************************************************************/
/* EdCrash_GetExceptionName						*/
/************************************************************************/
void	EdCrash_GetExceptionName (char *pmBuffer, DWORD pmException)
{
    switch (pmException)
    {
    	case EXCEPTION_ACCESS_VIOLATION:
    	    strcpy (pmBuffer, "Access Violation");
    	    break;
    	case EXCEPTION_DATATYPE_MISALIGNMENT:
    	    strcpy (pmBuffer, "Datatype Misaligned");
    	    break;
	case EXCEPTION_BREAKPOINT:
    	    strcpy (pmBuffer, "Breakpoint Reached");
    	    break;
	case EXCEPTION_SINGLE_STEP:
    	    strcpy (pmBuffer, "Debugger Single Step");
    	    break;
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
    	    strcpy (pmBuffer, "Array Bounds Exceeded");
    	    break;
	case EXCEPTION_FLT_DENORMAL_OPERAND:
    	    strcpy (pmBuffer, "Floating Point Denormal Operand");
    	    break;
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
    	    strcpy (pmBuffer, "Floating Point Divide by Zero");
    	    break;
	case EXCEPTION_FLT_INEXACT_RESULT:
    	    strcpy (pmBuffer, "Floating Point Inexact Result");
    	    break;
	case EXCEPTION_FLT_INVALID_OPERATION:
    	    strcpy (pmBuffer, "Floating Point Invalid Operation");
    	    break;
	case EXCEPTION_FLT_OVERFLOW:
    	    strcpy (pmBuffer, "Floating Point Overflow");
    	    break;
	case EXCEPTION_FLT_STACK_CHECK:
    	    strcpy (pmBuffer, "Floating Point Stack Check");
    	    break;
	case EXCEPTION_FLT_UNDERFLOW:
    	    strcpy (pmBuffer, "Floating Point Underflow");
    	    break;
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
    	    strcpy (pmBuffer, "Integer Divide by Zero");
    	    break;
	case EXCEPTION_INT_OVERFLOW:
    	    strcpy (pmBuffer, "Integer Overflow");
    	    break;
	case EXCEPTION_PRIV_INSTRUCTION:
    	    strcpy (pmBuffer, "Priveledged Instruction");
    	    break;
	case EXCEPTION_IN_PAGE_ERROR:
    	    strcpy (pmBuffer, "In Page Error");
    	    break;
	case EXCEPTION_ILLEGAL_INSTRUCTION:
    	    strcpy (pmBuffer, "Illegal Instruction");
    	    break;
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:
    	    strcpy (pmBuffer, "Non-Continuable Exception");
    	    break;
	case EXCEPTION_STACK_OVERFLOW:
    	    strcpy (pmBuffer, "Stack Overflow");
    	    break;
	case EXCEPTION_INVALID_DISPOSITION:
    	    strcpy (pmBuffer, "Invalid Dispostition");
    	    break;
	case EXCEPTION_GUARD_PAGE:
    	    strcpy (pmBuffer, "Guard Page");
    	    break;
	case EXCEPTION_INVALID_HANDLE:
    	    strcpy (pmBuffer, "Invalid Handle");
    	    break;
    	default:
    	    wsprintf (pmBuffer, "Unknown Exception #%d", pmException);
    	    break;
    } // switch
} // EdCrash_GetExceptionName


/************************************************************************/
/* EdCrash_RegisterDump							*/
/************************************************************************/
void	EdCrash_RegisterDump (char *pmBuffer, CONTEXT *pmContext)
{
    wsprintf (pmBuffer, "Registers\r\n"\
    	    "  EAX=%08X  EBX=%08X  ECX=%08X  EDX=%08X  ESI=%08X\r\n"\
    	    "  EDI=%08X  EBP=%08X  ESP=%08X  EIP=%08X  FLG=%08X\r\n"\
    	    "  CS=%04X  DS=%04X  SS=%04X  ES=%04X  FS=%04X  GS=%04X",
	    pmContext -> Eax, pmContext -> Ebx, pmContext -> Ecx,
	    pmContext -> Edx, pmContext -> Esi, pmContext -> Edi,
	    pmContext -> Ebp, pmContext -> Esp, pmContext -> Eip,
	    pmContext -> EFlags, pmContext -> SegCs, pmContext -> SegDs,
	    pmContext -> SegSs, pmContext -> SegEs, pmContext -> SegFs,
	    pmContext -> SegGs);
} // EdCrash_RegisterDump


/************************************************************************/
/* EdCrash_StackDump							*/
/*									*/
/* Does a stack dump to the buffer indicated by pmBuffer.  Note that 	*/
/* it tries to use the stack (which may be blown) as little as 		*/
/* possible, which is why all the variables are static.			*/
/************************************************************************/
void	EdCrash_StackDump (char *pmBuffer, int pmSize, CONTEXT *pmContext)
{
    static DWORD 		stMyImageType = IMAGE_FILE_MACHINE_I386;
    static HANDLE		stMyThread;
    static int 			stMyFrameNumber;
    static DWORD		stMyModuleBase;
    static IMAGEHLP_SYMBOL	*stMySymbol;
    static DWORD		stMyDisplacement;
    static int			stMyModuleNumber;
    static int			cnt;

    if (!stCrashInited)
    {
    	EdCrash_Init ();
    }
    
    stMyThread = GetCurrentThread ();
    stMySymbol = (IMAGEHLP_SYMBOL *) stSymbolBuffer;
    
    stDumpBufferPtr = pmBuffer;
    stDumpBufferLeft = pmSize;

    if (stInCrashHandler)
    {
    	MyAddToBuffer ("TROUBLE!  You're already in the crash handler!\r\n\r\n");
    	return;
    }
    
    stInCrashHandler = TRUE;
            
    if (!MyInitializeDbgHelp ())
    {
    	return;
    }
    	
    (*stSymSetOptProc) ((*stSymGetOptProc) () | SYMOPT_UNDNAME |
    			SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);

    stCurrentProcessHandle = GetCurrentProcess ();
    stCurrentProcessID = GetCurrentProcessId ();
    
    if (stIsNT)
    {
    	stCurrentProcess = stCurrentProcessHandle;
    }
    else
    {
    	stCurrentProcess = (HANDLE) stCurrentProcessID;
    }

    if (!MyInitializeSymbols ())
    {
    	return;
    }

    // Prepare initial stack frame
    ZeroMemory (&stStackFrame, sizeof (stStackFrame));
    stStackFrame.AddrPC.Offset = pmContext -> Eip;
    stStackFrame.AddrPC.Mode = AddrModeFlat;
    stStackFrame.AddrStack.Offset = pmContext -> Esp;
    stStackFrame.AddrStack.Mode = AddrModeFlat;
    stStackFrame.AddrFrame.Offset = pmContext -> Ebp;
    stStackFrame.AddrFrame.Mode = AddrModeFlat;

    stMyFrameNumber = 0;
    
    memcpy ((char *) &stThreadContext, (char *) pmContext, sizeof (CONTEXT));

    while (stMyFrameNumber < 50)
    {
    	// Get the next stack frame using StackWalk
    	if (!(*stStackWalkProc) (stMyImageType, stCurrentProcess, stMyThread, 
    				 &stStackFrame, &stThreadContext,
    				 (PREAD_PROCESS_MEMORY_ROUTINE) 
    				 	MyReadProcessMemory, 
    				 (*stSymFuncTableAccProc),
    				 (*stSymGetModBaseProc), NULL))
	{
	    break;
	}    				 
    	
    	// Ascertain if the address is legit.
    	if (stStackFrame.AddrFrame.Offset == 0)
    	{
    	    break;
    	}
    	
    	// Make certain that what's returned is a legit address
    	stMyModuleBase = (*stSymGetModBaseProc) (stCurrentProcess,
    					       stStackFrame.AddrPC.Offset);
	if (stMyModuleBase == 0)
	{
	    break;
	}    					      
    					       
	// Display its contents
    	MyAddToBuffer (
	        "%3d %c%c PC=%04x:%08lx Ret=%08lx Frame=%08lx Stack=%08lx\r\n",
		stMyFrameNumber, stStackFrame.Far? 'F': '.', 
		stStackFrame.Virtual? 'V': '.',
		stThreadContext.SegCs, stStackFrame.AddrPC.Offset, 
		stStackFrame.AddrReturn.Offset,
		stStackFrame.AddrFrame.Offset, stStackFrame.AddrStack.Offset);
		
	// Output possible parameters
    	MyAddToBuffer ("    Parameters (0x%08x 0x%08x 0x%08x 0x%08x)\r\n", 
		       stStackFrame.Params [0], stStackFrame.Params [1], 
		       stStackFrame.Params [2], stStackFrame.Params [3]);
		        
	// Get the module number
	stMyModuleNumber = -1;
	for (cnt = 0 ; cnt < stNumModules ; cnt++)
	{
	    if (stModuleInfo [cnt].moduleBaseAddress == stMyModuleBase)
	    {
	    	stMyModuleNumber = cnt;
	    	break;
	    }
	}
	
	// Get the symbol name
	ZeroMemory (stMySymbol, sizeof (SYMBOL_BUFFER_SIZE));
	stMySymbol -> SizeOfStruct = sizeof (IMAGEHLP_SYMBOL);
	stMySymbol -> MaxNameLength = SYMBOL_BUFFER_SIZE - 
						sizeof (IMAGEHLP_SYMBOL);
	if ((*stSymGetSymFromAddrProc) (stCurrentProcess, 
					stStackFrame.AddrPC.Offset,
				    	&stMyDisplacement, stMySymbol))
	{
	    // Get the line number information
	    ZeroMemory (&stLineNumberInfo, sizeof (IMAGEHLP_LINE));
	    stLineNumberInfo.SizeOfStruct = sizeof (IMAGEHLP_LINE);
	    if ((*stSymGetLineFromAddrProc) (stCurrentProcess,
					     stStackFrame.AddrPC.Offset,
				    	     &stMyDisplacement, 
				    	     &stLineNumberInfo))
	    {
	    	MyAddToBuffer ("    %s, %s (), Line %04d+%04d byte(s)", 
	    		       stLineNumberInfo.FileName,
	    		       stMySymbol -> Name, 
 	    	               stLineNumberInfo.LineNumber, stMyDisplacement);
	    }
	    else
	    {
	    	if (stMyModuleNumber == -1)
	    	{
	            MyAddToBuffer ("    Unknown module, %s() + %04d byte(s)", 
	        	           stMySymbol -> Name, stMyDisplacement);
	    	}
	    	else
	    	{
	            MyAddToBuffer ("    %s, %s() + %04d byte(s)", 
	            		   stModuleInfo [stMyModuleNumber].moduleName,
	        	           stMySymbol -> Name, stMyDisplacement);
	    	}
	    
	    }
	}
	else
	{
	    if (stMyModuleNumber == -1)
	    {
	        MyAddToBuffer ("    Unknown module");
	    }
	    else
	    {
	        MyAddToBuffer ("    %s", 
	        	       stModuleInfo [stMyModuleNumber].moduleName);
	    }
	}
	
	// Terminate the string
	MyAddToBuffer ("\r\n");

	stMyFrameNumber++;		
    } // while
    MyAddToBuffer ("\r\n");
} // EdCrash_StackDump


/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/
/************************************************************************/
/* MyAddToBuffer							*/
/************************************************************************/
static void	MyAddToBuffer (char *pmString,  ...)
{
    va_list 	myArgList;
    static char myStMessage [1024];
    int		myLen;
        
    va_start (myArgList, pmString);
    wvsprintf (myStMessage, pmString, myArgList);
    va_end (myArgList);

    myLen = strlen (myStMessage);
    
    if (myLen < stDumpBufferLeft)
    {
    	strcpy (stDumpBufferPtr, myStMessage);
    	stDumpBufferPtr += myLen;
    	stDumpBufferLeft -= myLen;
    }
    else
    {
    	strncpy (stDumpBufferPtr, myStMessage, stDumpBufferLeft);
    	stDumpBufferPtr += stDumpBufferLeft;
    	*stDumpBufferPtr = 0;
    	stDumpBufferLeft = 0;
    }
} // MyAddToBuffer


/************************************************************************/
/* MyGetLoadedModules							*/
/************************************************************************/
static BOOL	MyGetLoadedModules (int *pmModuleCount)
{
    static MODULEINFO	stMyInfo;
    
    if (stIsNT4)
    {
    	//
    	// Use the PSAPI.dll to enumerate the loaded modules
    	//
    	DWORD	myTotal;
    	BOOL	myResult;
    	int	cnt;
    	
    	if (!MyInitializePSAPI ())
    	{
    	    return FALSE;
    	}
    	
    	if (IsBadCodePtr ((FARPROC) stEnumProcModsProc))
        {
            MyAddToBuffer ("PSAPI.DLL has been unloaded on us.\r\n\r\n");
            return FALSE;
        }
        
        myResult = (*stEnumProcModsProc) (stCurrentProcessHandle, stModule, 
        			          MAX_MODULES * sizeof (HMODULE), 
        			          &myTotal);
	*pmModuleCount = myTotal / sizeof (HMODULE);
	if ((myResult == FALSE) || (myTotal == 0))
	{
            MyAddToBuffer ("EnumProcessModules failed.  Error = %d.\r\n\r\n",
	            	   GetLastError ());
            return FALSE;
	}
	if (*pmModuleCount > MAX_MODULES)
	{
            MyAddToBuffer ("Not enough space for modules.\r\n\r\n");
            return FALSE;
	}
	
	// Now fill in the rest of the information
	for (cnt = 0 ; cnt < *pmModuleCount ; cnt++)
	{
	    stModuleInfo [cnt].module = stModule [cnt];
	    (*stGetModInfoProc) (stCurrentProcessHandle, stModule [cnt],
	    			 &stMyInfo, sizeof (stMyInfo));
	    stModuleInfo [cnt].moduleBaseAddress = (DWORD) stMyInfo.lpBaseOfDll;
	    stModuleInfo [cnt].moduleSize = stMyInfo.SizeOfImage;
	    (*stGetModBaseNameProc) (stCurrentProcessHandle, stModule [cnt],
	    			     stPath, sizeof (stPath));
	    stModuleInfo [cnt].moduleName = EdGlob_MallocString (stPath);
	    (*stGetModFileNameProc) (stCurrentProcessHandle, stModule [cnt],
	    			     stPath, sizeof (stPath));
	    stModuleInfo [cnt].modulePath = EdGlob_MallocString (stPath);
	}		        	
    }
    else
    {
    	//
    	// Use the ToolHelp to enumerate the loaded modules
    	//
    	HANDLE		myModuleSnapshot = NULL;
    	MODULEENTRY32	myModuleEntry;
    	
    	if (!MyInitializeToolHelp ())
    	{
    	    return FALSE;
    	}
    	
    	myModuleSnapshot = (*stCreateSnapProc) (TH32CS_SNAPMODULE, 
    						stCurrentProcessID);
	if (myModuleSnapshot == INVALID_HANDLE_VALUE)
	{
            MyAddToBuffer ("CreateToolhelp32Snapshot failed.  Error = %d.\r\n\r\n",
	    	           GetLastError ());
            return FALSE;
	}
	
	ZeroMemory (&myModuleEntry, sizeof (MODULEENTRY32));
	myModuleEntry.dwSize = sizeof (MODULEENTRY32);
	
	*pmModuleCount = 0;
	
	if (!(*stModFirstProc) (myModuleSnapshot, &myModuleEntry))
	{
            MyAddToBuffer ("Module32First failed.  Error = %d.\r\n\r\n", 
            		   GetLastError ());
            return FALSE;
	}

        do
        {
            // Make certain there's enough room
            if (*pmModuleCount == MAX_MODULES)
            {
	        MyAddToBuffer ("Not enough space for modules.\r\n\r\n");
	        return FALSE;
	    }
	    stModuleInfo [*pmModuleCount].module = myModuleEntry.hModule;
	    stModuleInfo [*pmModuleCount].moduleBaseAddress = 
	        			(DWORD) myModuleEntry.modBaseAddr;
	    stModuleInfo [*pmModuleCount].moduleSize = 
	    					myModuleEntry.modBaseSize;
	    stModuleInfo [*pmModuleCount].moduleName = 
	        		EdGlob_MallocString (myModuleEntry.szModule);
	    stModuleInfo [*pmModuleCount].modulePath = 
	        		EdGlob_MallocString (myModuleEntry.szExePath);

            // Bump up the real total count.
            *pmModuleCount += 1;
        }
        while ((*stModNextProc) (myModuleSnapshot, &myModuleEntry));
    }

    return TRUE;
} // MyGetLoadedModules


/************************************************************************/
/* MyInitializeDbgHelp							*/
/*									*/
/* We try loading it generally, and if that fails, then we load it from	*/
/* Support\Microsoft.							*/
/************************************************************************/
static BOOL	MyInitializeDbgHelp (void)
{
    if (stDebugHelpDLL != NULL)
    {
    	return TRUE;
    }
    
    stDebugHelpDLL = LoadLibrary ("dbghelp.dll");
    if (stDebugHelpDLL == NULL)
    {
    	DWORD	myError = GetLastError ();
    	EdFile_GetApplicationDirectory (stPath);
	strcat (stPath, "Support\\Microsoft\\dbghelp.dll");
    	stDebugHelpDLL = LoadLibrary (stPath);
    	if (stDebugHelpDLL == NULL)
	{
    	    MyAddToBuffer ("Unable to load dbghelp.  Error=%d/%d.\r\n\r\n", 
    	    		   myError, GetLastError ());
    	    return FALSE;
    	}
    }

    stSymInitializeProc = 
    	(DbgHelp_SymInit) GetProcAddress (stDebugHelpDLL, "SymInitialize");

    if (stSymInitializeProc == NULL)
    {
    	MyAddToBuffer ("Unable to find SymInitialize in dbghelp.dll.\r\n\r\n");
    	return FALSE;
    }
    
    stSymLoadModProc = 
    	(DbgHelp_SymLoadMod) GetProcAddress (stDebugHelpDLL, "SymLoadModule");

    if (stSymLoadModProc == NULL)
    {
    	MyAddToBuffer ("Unable to find SymLoadModule in dbghelp.dll.\r\n\r\n");
    	return FALSE;
    }
    
    stSymGetOptProc = 
    	(DbgHelp_SymGetOpt) GetProcAddress (stDebugHelpDLL, "SymGetOptions");

    if (stSymGetOptProc == NULL)
    {
    	MyAddToBuffer ("Unable to find SymGetOptions in dbghelp.dll.\r\n\r\n");
    	return FALSE;
    }
    
    stSymSetOptProc = 
    	(DbgHelp_SymSetOpt) GetProcAddress (stDebugHelpDLL, "SymSetOptions");

    if (stSymSetOptProc == NULL)
    {
    	MyAddToBuffer ("Unable to find SymSetOptions in dbghelp.dll.\r\n\r\n");
    	return FALSE;
    }
    
    stStackWalkProc = 
    	(DbgHelp_StackWalk) GetProcAddress (stDebugHelpDLL, "StackWalk");

    if (stStackWalkProc == NULL)
    {
    	MyAddToBuffer ("Unable to find StackWalk in dbghelp.dll.\r\n\r\n");
    	return FALSE;
    }
    
    stSymFuncTableAccProc = 
    	(DbgHelp_SymFuncTableAcc) GetProcAddress (stDebugHelpDLL, 
    						  "SymFunctionTableAccess");

    if (stSymFuncTableAccProc == NULL)
    {
    	MyAddToBuffer (
    	    "Unable to find SymFunctionTableAccess in dbghelp.dll.\r\n\r\n");
    	return FALSE;
    }
    
    stSymGetModBaseProc = 
    	(DbgHelp_SymGetModBase) GetProcAddress (stDebugHelpDLL, 
    						"SymGetModuleBase");

    if (stSymGetModBaseProc == NULL)
    {
    	MyAddToBuffer (
    		  "Unable to find SymGetModuleBase in dbghelp.dll.\r\n\r\n");
    	return FALSE;
    }

    stSymGetSymFromAddrProc =
        (DbgHelp_SymGetSymFromAddr) GetProcAddress(stDebugHelpDLL, 
        					   "SymGetSymFromAddr");
        					       
    if (stSymGetSymFromAddrProc == NULL)
    {
    	MyAddToBuffer (
    		  "Unable to find SymGetSymFromAddr in dbghelp.dll.\r\n\r\n");
    	return FALSE;
    }

    stSymGetLineFromAddrProc =
        (DbgHelp_SymGetLineFromAddr) GetProcAddress (stDebugHelpDLL, 
        					     "SymGetLineFromAddr");
        					       
    if (stSymGetLineFromAddrProc == NULL)
    {
    	MyAddToBuffer (
    		  "Unable to find SymGetLineFromAddr in dbghelp.dll.\r\n\r\n");
    	return FALSE;
    }

/*        
	pSC = (tSC) GetProcAddress( hImagehlpDll, "SymCleanup" );
	pSGMI = (tSGMI) GetProcAddress( hImagehlpDll, "SymGetModuleInfo" );
	pUDSN = (tUDSN) GetProcAddress( hImagehlpDll, "UnDecorateSymbolName" );
	pSLM = (tSLM) GetProcAddress( hImagehlpDll, "SymLoadModule" );
*/
    return TRUE;
} // MyInitializeDbgHelp


/************************************************************************/
/* MyInitializePSAPI							*/
/************************************************************************/
static BOOL	MyInitializePSAPI (void)
{
    if (stPSAPIDLL != NULL)
    {
    	return TRUE;
    }
    
    stPSAPIDLL = LoadLibrary ("PSAPI.dll");
    if (stPSAPIDLL == NULL)
    {
    	MyAddToBuffer ("Unable to load PSAPI.dll.  Error=%d.\r\n\r\n", 
    		       GetLastError ());
    	return FALSE;
    }

    stEnumProcModsProc = 
    	(PSAPI_EnumProcMods) GetProcAddress (stPSAPIDLL, "EnumProcessModules");

    if (stEnumProcModsProc == NULL)
    {
    	MyAddToBuffer ("Unable to find EnumProcessModules in PSAPI.dll\r\n\r\n");
    	return FALSE;
    }
    
    stGetModBaseNameProc = 
    	(PSAPI_GetModBaseName) GetProcAddress (stPSAPIDLL, 
    							"GetModuleBaseNameA");

    if (stGetModBaseNameProc == NULL)
    {
    	MyAddToBuffer ("Unable to find GetModuleBaseNameA in PSAPI.dll\r\n\r\n");
    	return FALSE;
    }
    
    stGetModFileNameProc = 
    	(PSAPI_GetModFileName) GetProcAddress (stPSAPIDLL, 
    							"GetModuleFileNameExA");

    if (stGetModFileNameProc == NULL)
    {
    	MyAddToBuffer ("Unable to find GetModuleFileNameExA in PSAPI.dll\r\n\r\n");
    	return FALSE;
    }
    
    stGetModInfoProc = 
    	(PSAPI_GetModInfo) GetProcAddress (stPSAPIDLL, "GetModuleInformation");

    if (stGetModInfoProc == NULL)
    {
    	MyAddToBuffer ("Unable to find GetModuleInfo in PSAPI.dll\r\n\r\n");
    	return FALSE;
    }
    
    return TRUE;
} // MyInitializePSAPI
   
   
/************************************************************************/
/* MyInitializeSymbols							*/
/************************************************************************/
static BOOL	MyInitializeSymbols (void)
{
    static int	stMyModuleCount;
    static int	cnt;	

    // 1) [ApplicationDir]\Support\Bin
    EdFile_GetApplicationDirectory (stPath);
    if (stPath [strlen (stPath) - 1] == '\\')
    	strcat (stPath, "Support\\Bin;");
    else
    	strcat (stPath, "\\Support\\Bin;");
    
    
    // 2) [ApplicationDir]
    EdFile_GetApplicationDirectory (&stPath [strlen (stPath)]);
    if (stPath [strlen (stPath) - 1] == '\\')
    	stPath [strlen (stPath) - 1] = 0;
    strcat (stPath, ";");

    // 3) [ApplicationDir]\obj (For debugging on development machine)
    EdFile_GetApplicationDirectory (&stPath [strlen (stPath)]);
    if (stPath [strlen (stPath) - 1] == '\\')
    	strcat (stPath, "obj;");
    else
    	strcat (stPath, "\\obj;");

    // 4) [CurrentDir]    
    EdFile_GetCurrentDirectory (&stPath [strlen (stPath)]);
    if (stPath [strlen (stPath) - 1] == '\\')
    	stPath [strlen (stPath) - 1] = 0;
    strcat (stPath, ";");

    // 5) environment variable _NT_SYMBOL_PATH
    if (GetEnvironmentVariable ("_NT_SYMBOL_PATH", 
			    &stPath [strlen (stPath)], 
			    sizeof (stPath) - strlen (stPath)))
    {			    
	strcat (stPath, ";");
    }

    // 6) environment variable _NT_ALTERNATE_SYMBOL_PATH
    if (GetEnvironmentVariable ("_NT_ALTERNATE_SYMBOL_PATH", 
			    &stPath [strlen (stPath)], 
			    sizeof (stPath) - strlen (stPath)))
    {			    
	strcat (stPath, ";");
    }

    // 7) environment variable SYSTEMROOT
    if (GetEnvironmentVariable ("SYSTEMROOT", 
			    &stPath [strlen (stPath)], 
			    sizeof (stPath) - strlen (stPath)))
    {			    
	strcat (stPath, ";");
    }

    if (stIsNT)
    {
    	if (AUTO_LOAD_MODULES)
    	{
    	    if (!(*stSymInitializeProc) (stCurrentProcess, stPath, TRUE))
    	    {
    	    	MyAddToBuffer ("SymInitialize returned error : %d.\r\n\r\n", 
    		 	       GetLastError ());
    	        return FALSE;
    	    }
    	}
    }

    //
    // For Windows 9x, we need to load modules by hand.  For now, we always
    // load by hand, regardless of OS.
    //
    
    // Initialize the Symbol engine
    if (!(*stSymInitializeProc) (stCurrentProcess, stPath, FALSE))
    {
    	MyAddToBuffer ("SymInitialize returned error : %d.\r\n\r\n",
    		       GetLastError ());
    	return FALSE;
    }

    // Load the modules
    if (!MyGetLoadedModules (&stMyModuleCount))
    {
    	return FALSE;
    }

    MyAddToBuffer ("List of Modules: \r\n");
        
    for (cnt = 0 ; cnt < stMyModuleCount ; cnt++)
    {
	MyAddToBuffer ("    %s\r\n", stModuleInfo [cnt].modulePath);

        // For whatever reason, SymLoadModule can return zero, but
        // it still loads the modules.  Sheez.
        if (!(*stSymLoadModProc) (stCurrentProcess, NULL, 
        			  stModuleInfo [cnt].modulePath,
        			  stModuleInfo [cnt].moduleName, 
        		          stModuleInfo [cnt].moduleBaseAddress, 
				  stModuleInfo [cnt].moduleSize))
	{    
	    int	myError = GetLastError ();
	    
	    if (myError != 0)
	    {
    	        MyAddToBuffer ("\nSymLoadModule returned error : %d.\r\n\r\n", 
    	        	       GetLastError ());
    	        return FALSE;
	    }
	}	    
    }
    
    MyAddToBuffer ("\r\n");
	
    return TRUE;
} // MyInitializeSymbols


/************************************************************************/
/* MyInitializeToolHelp							*/
/************************************************************************/
static BOOL	MyInitializeToolHelp (void)
{
    if (stKernel32DLL != NULL)
    {
    	return TRUE;
    }
    
    stKernel32DLL = LoadLibrary ("Kernel32.dll");
    if (stKernel32DLL == NULL)
    {
    	MyAddToBuffer ("Unable to load Kernel32.dll.  Error=%d.\r\n\r\n", 
    		       GetLastError ());
    	return FALSE;
    }

    stCreateSnapProc = 
    	(ToolHelp_CreateSnap) GetProcAddress (stKernel32DLL, 
    						"CreateToolhelp32Snapshot");

    if (stCreateSnapProc == NULL)
    {
    	MyAddToBuffer (
    	    "Unable to find CreateToolhelp32Snapshot in Kernel32.dll\r\n\r\n");
    	return FALSE;
    }
    
    stModFirstProc = 
    	(ToolHelp_ModuleWalk) GetProcAddress (stKernel32DLL, "Module32First");

    if (stModFirstProc == NULL)
    {
    	MyAddToBuffer (
    		  "Unable to find Module32First in Kernel32.dll\r\n\r\n");
    	return FALSE;
    }
    
    stModNextProc = 
    	(ToolHelp_ModuleWalk) GetProcAddress (stKernel32DLL, "Module32Next");

    if (stModNextProc == NULL)
    {
    	MyAddToBuffer ("Unable to find Module32Next in Kernel32.dll\r\n\r\n");
    	return FALSE;
    }
    
    stProcFirstProc = 
    	(ToolHelp_ProcessWalk) GetProcAddress (stKernel32DLL, "Process32First");

    if (stProcFirstProc == NULL)
    {
    	MyAddToBuffer (
    		  "Unable to find Process32First in Kernel32.dll\r\n\r\n");
    	return FALSE;
    }
    
    stProcNextProc = 
    	(ToolHelp_ProcessWalk) GetProcAddress (stKernel32DLL, "Process32Next");

    if (stProcNextProc == NULL)
    {
    	MyAddToBuffer ("Unable to find Process32Next in Kernel32.dll\r\n\r\n");
    	return FALSE;
    }
    
    stThreadFirstProc = 
    	(ToolHelp_ThreadWalk) GetProcAddress (stKernel32DLL, "Thread32First");

    if (stThreadFirstProc == NULL)
    {
    	MyAddToBuffer (
    		  "Unable to find Thread32First in Kernel32.dll\r\n\r\n");
    	return FALSE;
    }
    
    stThreadNextProc = 
    	(ToolHelp_ThreadWalk) GetProcAddress (stKernel32DLL, "Thread32Next");

    if (stThreadNextProc == NULL)
    {
    	MyAddToBuffer ("Unable to find Thread32Next in Kernel32.dll\r\n\r\n");
    	return FALSE;
    }
    
    return TRUE;
} // MyInitializeToolHelp
   
   
/************************************************************************/
/* MyReadProcessMemory							*/
/************************************************************************/
static BOOL __stdcall 	MyReadProcessMemory (HANDLE pmHandle, 
					     LPCVOID lpBaseAddress,
                                      	     LPVOID lpBuffer, DWORD nSize,
                                      	     LPDWORD lpNumberOfBytesRead)
{
    return (ReadProcessMemory (GetCurrentProcess (), lpBaseAddress, lpBuffer,
                               nSize, lpNumberOfBytesRead));
} // MyReadProcessMemory
