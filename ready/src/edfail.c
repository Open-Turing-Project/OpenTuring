/************/
/* edfail.c */
/************/

/*******************/
/* System includes */
/*******************/
#include <windows.h>
#include <crtdbg.h>

/****************/
/* Self include */
/****************/
#include "edfail.h"

/******************/
/* Other includes */
/******************/
#include "ed.h"
#include "edcrash.h"
#include "edfile.h"
#include "edgui.h"
#include "edprop.h"

#include "eddialogs.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
#define CRASH_BUFFER_SIZE	20000

#define NUM_PROGRESS_MESSAGES	150
#define MESSAGE_BUFFER_SIZE	500

#define WARNING		FALSE
#define FATAL		TRUE

#define LINE_OF_DASHES			"------------------------------------------------------"

// English string error messages in case the 
// string resources have become corrupted
#define WINDOW_TITLE_REPL		"Internal Error"
#define CRASH_PREAMBLE_REPL1		"The environment (not your program) has crashed.\r\n\r\nThis error is caused by a bug in the environment and is not your fault!\r\nDetails about the crash have been saved in the file \""
#define CRASH_PREAMBLE_REPL2		"\".\r\n\r\nIf possible, please e-mail this file to readybugs@hsa.on.ca\r\n\r\nHere are some details about the crash:\r\n"
#define FAIL_PREAMBLE_REPL1		"The environment has experienced an unexpected internal error.\r\n\r\nThis error is caused by a bug in the environment and is not your fault!\r\nDetails about the crash have been saved in the file \""
#define FAIL_PREAMBLE_REPL2		"\".\r\n\r\nIf possible, please e-mail this file to readybugs@hsa.on.ca\r\n\r\nHere are some details about the error:\r\n"
#define FAIL_REASON_NOT_FOUND_REPL	"   Error code = %d\r\n"
#define FAIL_REASON_FOUND_REPL		"   Error = %s (%d)\r\n"
#define FAIL_WINDOWS_ERROR_REPL		"   Windows error = %s (%d)\r\n"
#define FAIL_WINDOWS_UNKNOWN_ERROR_REPL	"   Windows error code = %d\r\n"
#define FAIL_FILE_LINE_REPL		"   File = %s\r\n   Line = %d\r\n"
#define FAIL_SAVE_FILE_REPL		"\r\nAll unsaved open files will be saved with a "".sav"" or "".sv#"" extension."
#define PANIC_LOG_FILE_REPL		"panic.log"

// Status
#define OPEN_SUCCESS			71
#define OPEN_FAIL_CURRENT_IS_STARTUP	72
#define OPEN_FAIL_BOTH_FAILED		73

/********************/
/* Global variables */
/********************/

/*********/
/* Types */
/*********/
typedef struct
{
    HANDLE		thread;	
    FileEntryPtr	file;
} StackDumpThreadInfo;
    
/**********************/
/* External variables */
/**********************/

/********************/
/* Static constants */
/********************/

/********************/
/* Static variables */
/********************/
static char	stCrashBuffer [CRASH_BUFFER_SIZE];
static char	*stCrashBufferPtr;
static UINT	stCrashBufferLeft;
static char	stWindowTitle [80];
static char	*stProgressMessage [NUM_PROGRESS_MESSAGES];
static int	stProgressMessageHead = 0;
static int	stCrashing = FALSE;

/******************************/
/* Static callback procedures */
/******************************/

/*********************/
/* Static procedures */
/*********************/
static void	MyAddToBuffer (char *pmString,  ...);
static void	MyAddPreviousCommandsToBuffer (void);
static void	MyEmergencySave (char *pmPathName, HWND pmEditWindow, 
		    		 HWND pmTextDisplayWindow, TextPtr pmTextPtr, 
				 void *pmSaveList);
static void	MyFailErrorBox (BOOL pmFatal, int pmReason, char *pmFile, 
		     		int pmLine, int pmErrorCode, va_list pmArgs);
static void	MyGetEnvironmentInfo (void);
static void	MyLoadString (int pmStringResource, char *pmString, 
			      size_t pmStringSize, const char *pmReplaceString);
static void	MySaveAllFiles (void);
static int	MyStackDump (LPVOID pmThreadData);
			    

/***********************/
/* External procedures */
/***********************/
/************************************************************************/
/* EdFail_Init								*/
/************************************************************************/
BOOL	EdFail_Init (void)
{
    int	cnt;
    
    for (cnt = 0 ; cnt < NUM_PROGRESS_MESSAGES ; cnt++)
    {
    	stProgressMessage [cnt] = malloc (MESSAGE_BUFFER_SIZE);
    	if (stProgressMessage [cnt] == NULL)
    	{
    	    EdFail_Warn (IDS_OUTOFMEMORY, __FILE__, __LINE__, 0);
            return FALSE;
    	}
    	stProgressMessage [cnt][0] = 0;
    }

#ifndef TPROLOG    
    EdCrash_Init ();
#endif // #ifndef TPROLOG
    return TRUE;
} // EdFail_Init


#ifndef TPROLOG
/************************************************************************/
/* EdFail_Finalize							*/
/************************************************************************/
void	EdFail_Finalize (void)
{   
    int	cnt;
    
    EdCrash_Finalize ();
    
    for (cnt = 0 ; cnt < NUM_PROGRESS_MESSAGES ; cnt++)
    {
	free (stProgressMessage [cnt]);
    } // for
} // EdFail_Finalize


/************************************************************************/
/* EdFail_AddMessage							*/
/************************************************************************/
void	EdFail_AddMessage (int pmMsgType, const char *pmMessage, ...)
{
    static int	stLastMessage = NORMAL;
    static int	stLastProgessMessageHead = 0;
    static int	stCount = 0;
    static char	stLineOfText [1024];
    va_list 	myArgList;
    char	myLetter [10];
    
    if (stCrashing) return;
    
    switch (pmMsgType)
    {
    	case NORMAL:
    	    stLastMessage = pmMsgType;
    	    break;
    	case LETTER_MESSAGE:
    	    if ((stLastMessage == LETTER_MESSAGE) && (stCount < 1000))
    	    {
    	    	stProgressMessageHead = stLastProgessMessageHead;
    	    	stCount++;
    	    }
    	    else
    	    {
    	    	stLastProgessMessageHead = stProgressMessageHead;
    	    	stCount = 1;
    	    	stLineOfText [0] = 0;
    	    }
   	    stLastMessage = pmMsgType;
    	    // Get letter being added and add it to the list.
	    va_start (myArgList, pmMessage);
	    wvsprintf (myLetter, "%c", myArgList);
	    va_end (myArgList);
	    strcat (stLineOfText, myLetter);
    	    break;
    	case MOUSE_MESSAGE:
    	    if (stLastMessage == MOUSE_MESSAGE)
    	    {
    	    	stProgressMessageHead = stLastProgessMessageHead;
    	    	stCount++;
    	    }
    	    else
    	    {
    	    	stLastProgessMessageHead = stProgressMessageHead;
    	    	stCount = 1;
    	    }
    	    stLastMessage = pmMsgType;
    	    break;
    	case EXTRA_MESSAGE:
    	    break;
    }

    va_start (myArgList, pmMessage);
    wvsprintf (stProgressMessage [stProgressMessageHead], pmMessage, myArgList);
    va_end (myArgList);
    
    if ((pmMsgType == MOUSE_MESSAGE) || (pmMsgType == LETTER_MESSAGE))
    {
    	char	myTempStr [20];
    	
	wsprintf (myTempStr, " (x%d)", stCount);
	strcat (stProgressMessage [stProgressMessageHead], myTempStr);
    }

    if (pmMsgType == LETTER_MESSAGE)
    {
    	char	myTempStr [1028];
    	
	wsprintf (myTempStr, " [%s]", stLineOfText);
	strcat (stProgressMessage [stProgressMessageHead], myTempStr);
    }

    stProgressMessageHead = (stProgressMessageHead + 1) % NUM_PROGRESS_MESSAGES;
} // EdFail_AddMessage


/************************************************************************/
/* EdFail_CrashHandler							*/
/************************************************************************/
long	EdFail_CrashHandler (struct _EXCEPTION_POINTERS *pmExceptionInfo)
{
    static int		stInCrashHandler = 0;
    static FilePath	stMyPanicLogName, stMyAltPanicLogName;
    static HANDLE	stMyPanicFile;
    static DWORD	stMyNumBytesWritten;
    static int		stMyFileOpenStatus;
    static int		stMyFileOpenError;
        
    stCrashing = TRUE;
    
    if (stCrashBuffer == NULL)
    {
    	return EXCEPTION_EXECUTE_HANDLER;
    }

    if (stInCrashHandler > 1)
    {
    	return EXCEPTION_EXECUTE_HANDLER;
    }
    
    stInCrashHandler++;
    
    if (stInCrashHandler == 2)
    {
    	MyAddToBuffer ("\r\nCrashed in Crash Handler!\r\n");
    }
    else
    {
    	// Get file name
    	strcpy (stMyPanicLogName, EdProp_GetStartupDirectory ());

    	MyLoadString (IDS_PANIC_LOG_FILE, 
    	    &stMyPanicLogName [strlen (stMyPanicLogName)], 
            sizeof (stMyPanicLogName), PANIC_LOG_FILE_REPL);
            
	// Try opening the file.            
    	stMyPanicFile = CreateFile (stMyPanicLogName, GENERIC_WRITE, 0, NULL,
    	    OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	// If opening the file failed, then try current directory instead.    	    
    	if (stMyPanicFile != (HANDLE) INVALID_HANDLE_VALUE)
    	{
    	    // File opened successfully.  Now close it
	    stMyFileOpenStatus = OPEN_SUCCESS;
    	    CloseHandle (stMyPanicFile);
    	}
    	else
	{
    	    stMyFileOpenError = GetLastError ();
    	    
	    // If the opening failed, try using the current directory.
	    EdFile_GetCurrentDirectory (stMyAltPanicLogName);
	    
	    if (strcmp (stMyAltPanicLogName, 
	    		EdProp_GetStartupDirectory ()) == 0)
	    {
	    	stMyFileOpenStatus = OPEN_FAIL_CURRENT_IS_STARTUP;
	    }
	    else
	    {
    	    	MyLoadString (IDS_PANIC_LOG_FILE, 
    	    	    &stMyAltPanicLogName [strlen (stMyAltPanicLogName)], 
            	    sizeof (stMyAltPanicLogName), PANIC_LOG_FILE_REPL);
            
	    	// Try opening the file.            
    	    	stMyPanicFile = CreateFile (stMyAltPanicLogName, GENERIC_WRITE, 
    	    			  	    0, NULL, OPEN_ALWAYS, 
    	    			  	    FILE_ATTRIBUTE_NORMAL, NULL);
    	    	if (stMyPanicFile != (HANDLE) INVALID_HANDLE_VALUE)
    	    	{
    	    	    // File opened successfully.  Now close it
	    	    stMyFileOpenStatus = OPEN_SUCCESS;
    	    	    CloseHandle (stMyPanicFile);
    	    	    strcpy (stMyPanicLogName, stMyAltPanicLogName);
    	    	}
    	    	else
	    	{
    	    	    stMyFileOpenError = GetLastError ();
	    	    stMyFileOpenStatus = OPEN_FAIL_BOTH_FAILED;
	    	}    	    
            }
	}

    	//
    	// Display the crash dialog box
    	//
    	stCrashBufferPtr = stCrashBuffer;
    	stCrashBufferLeft = CRASH_BUFFER_SIZE - 10;
        
    	// Get the window title (e.g. "Turing Internal Error")
    	MyLoadString (IDS_FATALWINDOWTITLE, stWindowTitle, 
    		      sizeof (stWindowTitle), WINDOW_TITLE_REPL);
    
    	// Get the preamble (e.g. "An unexpected error... Please record...")
    	MyLoadString (IDS_CRASHPREAMBLE1, stCrashBufferPtr, stCrashBufferLeft,
    		      CRASH_PREAMBLE_REPL1);
    	stCrashBufferLeft -= strlen (stCrashBufferPtr);
    	stCrashBufferPtr += strlen (stCrashBufferPtr);
    	MyAddToBuffer (stMyPanicLogName);
    	MyLoadString (IDS_CRASHPREAMBLE2, stCrashBufferPtr, stCrashBufferLeft,
    		      CRASH_PREAMBLE_REPL2);
    	stCrashBufferLeft -= strlen (stCrashBufferPtr);
    	stCrashBufferPtr += strlen (stCrashBufferPtr);

    	// Add the reason for the crash
    	MyAddToBuffer ("Cause of Crash: ");
    	EdCrash_GetExceptionName (stCrashBufferPtr, 
    			pmExceptionInfo -> ExceptionRecord -> ExceptionCode);
    	stCrashBufferLeft -= strlen (stCrashBufferPtr);
    	stCrashBufferPtr += strlen (stCrashBufferPtr);
    	MyAddToBuffer ("\r\n\r\n");
	
    	// Add the register dump
    	EdCrash_RegisterDump (stCrashBufferPtr, pmExceptionInfo -> ContextRecord);
    	stCrashBufferLeft -= strlen (stCrashBufferPtr);
    	stCrashBufferPtr += strlen (stCrashBufferPtr);
    	MyAddToBuffer ("\r\n");

    	// Add the saving files message    
    	MyLoadString (IDS_FAILSAVEFILE, stCrashBufferPtr, stCrashBufferLeft,
        	      FAIL_SAVE_FILE_REPL);
    	stCrashBufferLeft -= strlen (stCrashBufferPtr);
    	stCrashBufferPtr += strlen (stCrashBufferPtr);
    
    	// Display the error message    		
    	MessageBox (NULL, stCrashBuffer, stWindowTitle, 
            MB_OK | MB_ICONERROR | MB_TASKMODAL);

    	//
    	// Write to panic.log
    	//
    	stCrashBufferPtr = stCrashBuffer;
    	stCrashBufferLeft = CRASH_BUFFER_SIZE - 10;
   
    	// Title
    	MyAddToBuffer ("*** ENVIRONMENT CRASH ***\r\n\r\n");
    	
    	// Get environment information
    	MyGetEnvironmentInfo ();

    	// Add the reason for the crash
    	MyAddToBuffer ("\r\n\r\n");
    	MyAddToBuffer ("Cause of Crash: ");
        EdCrash_GetExceptionName (stCrashBufferPtr, 
    			pmExceptionInfo -> ExceptionRecord -> ExceptionCode);
    	stCrashBufferLeft -= strlen (stCrashBufferPtr);
    	stCrashBufferPtr += strlen (stCrashBufferPtr);

	// Add previous operations
	MyAddPreviousCommandsToBuffer ();
	    
    	// Add the register dump
    	EdCrash_RegisterDump (stCrashBufferPtr, pmExceptionInfo -> ContextRecord);
    	stCrashBufferLeft -= strlen (stCrashBufferPtr);
    	stCrashBufferPtr += strlen (stCrashBufferPtr);
    	MyAddToBuffer ("\r\n\r\n");

    	// Add the stack dump    
    	EdCrash_StackDump (stCrashBufferPtr, stCrashBufferLeft, 
    		           pmExceptionInfo -> ContextRecord);
    	stCrashBufferLeft -= strlen (stCrashBufferPtr);
    	stCrashBufferPtr += strlen (stCrashBufferPtr);

	MyAddToBuffer (LINE_OF_DASHES);
    	MyAddToBuffer ("\r\n");
    }

    if (stMyFileOpenStatus != OPEN_SUCCESS)
    {
    	if (stMyFileOpenStatus == OPEN_FAIL_CURRENT_IS_STARTUP)
    	{
    	    EdGUI_Message ("Panic log failure", 
    	        "Unable to open \"%s\" for writing.\nError = %d", 
    	        stMyPanicLogName, stMyFileOpenError);
    	}
    	else
    	{
	    EdGUI_Message ("Panic log failure", 
	    	"Unable to open \"%s\" or \"%s\" for writing.\nError = %d", 
	    	stMyPanicLogName, stMyAltPanicLogName, stMyFileOpenError);
	}	    	
    	return EXCEPTION_EXECUTE_HANDLER;
    }

    stMyPanicFile = CreateFile (stMyPanicLogName, GENERIC_WRITE, 0, NULL,
    	OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (stMyPanicFile == (HANDLE) INVALID_HANDLE_VALUE)
    {
    	EdGUI_Message ("Panic log failure", 
    	    "Unable to open \"%s\" for writing.\nError = %d", 
    	    stMyPanicLogName, GetLastError ());
    	return EXCEPTION_EXECUTE_HANDLER;
    }
    
    // Move to the end of the file
    SetFilePointer (stMyPanicFile, 0, NULL, FILE_END);

    if (!WriteFile (stMyPanicFile, stCrashBuffer, strlen (stCrashBuffer),
    		    &stMyNumBytesWritten, NULL))
    {
    	EdGUI_Message ("Panic log failure",
    	    "Unable to write to \"%s\"\nError = %d", 
    	    stMyPanicLogName, GetLastError ());
    	return EXCEPTION_EXECUTE_HANDLER;
    }
    if (stMyNumBytesWritten != strlen (stCrashBuffer))
    {
    	EdGUI_Message ("Panic log failure",
    	    "Unable to write to \"%s\"\n%d bytes written instead of %d bytes",
    	    stMyPanicLogName, stMyNumBytesWritten, strlen (stCrashBuffer));
    	return EXCEPTION_EXECUTE_HANDLER;
    }
    if (!FlushFileBuffers (stMyPanicFile))
    {
    	EdGUI_Message ("Panic log failure",
    	    "Unable to flush buffers of \"%s\"\nError = %d", 
    	    stMyPanicLogName, GetLastError ());
    	return EXCEPTION_EXECUTE_HANDLER;
    }

    if (!CloseHandle (stMyPanicFile))
    {
    	EdGUI_Message ("Panic log failure",
    	    "Unable to close \"%s\"\nError = %d", 
    	    stMyPanicLogName, GetLastError ());
    	return EXCEPTION_EXECUTE_HANDLER;
    }

    return EXCEPTION_EXECUTE_HANDLER;
} // EdFail_CrashHandler
#endif // #ifndef TPROLOG


/************************************************************************/
/* EdFail_Fatal								*/
/*									*/
/* This procedure is called when Turing has an internal failure, and	*/
/* for safety reasons should exit immediately.  It output a message	*/
/* based on the string resource passed in pmReason.			*/
/************************************************************************/
void	EdFail_Fatal (int pmReason, char *pmFile, int pmLine, 
		      int pmErrorCode, ...)
{
    va_list 	myArgList;

    stCrashing = TRUE;
    
    // Output the error message to dialog box and file    
    va_start (myArgList, pmErrorCode);
    MyFailErrorBox (FATAL, pmReason, pmFile, pmLine, pmErrorCode, myArgList);
    va_end (myArgList);
    
#ifndef TPROLOG    
    // Save all unsaved open files
    MySaveAllFiles ();

    // If we're not catching exceptions, then we're probably using
    // a debugger, in which case call the debugger
    if (!gProperties.catchExceptions)
    {
    	DebugBreak ();
    }
#endif // #ifndef TPROLOG
    exit (1);
} // EdFail_Fatal


/************************************************************************/
/* EdFail_Fatal1							*/
/*									*/
/* This procedure is called by other routines outside of the editor	*/
/* to generate a fatal error.						*/
/************************************************************************/
void	EdFail_Fatal1 (int pmReason, char *pmFile, int pmLine, 
		       int pmErrorCode, va_list pmArgsList)
{
    stCrashing = TRUE;
    MyFailErrorBox (FATAL, pmReason, pmFile, pmLine, pmErrorCode, pmArgsList);
    
#ifndef TPROLOG
    // Save all unsaved open files
    MySaveAllFiles ();

    // If we're not catching exceptions, then we're probably using
    // a debugger, in which case call the debugger
    if (!gProperties.catchExceptions)
    {
    	DebugBreak ();
    }
#endif // #ifndef TPROLOG
    exit (1);
} // EdFail_Fatal1


/************************************************************************/
/* EdFail_GetErrorMessage 						*/
/************************************************************************/
char	*EdFail_GetErrorMessage (ErrorBuffer pmBuffer)
{
    return EdFail_GetErrorMessage1 (pmBuffer, GetLastError ());
} // EdFail_GetErrorMessage


/************************************************************************/
/* EdFail_GetErrorMessage1						*/
/************************************************************************/
char	*EdFail_GetErrorMessage1 (ErrorBuffer pmBuffer, int pmErrorCode)
{
    int	myResult;

    myResult = FormatMessage (
		   FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
    		   NULL, pmErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
    		   (LPTSTR) pmBuffer, ERROR_BUFFER_SIZE, NULL);

    if (myResult == 0)
    {
	pmBuffer [0] = 0;
    }
    else if ((pmBuffer [strlen (pmBuffer) - 1] == '\n') &&
             (pmBuffer [strlen (pmBuffer) - 2] == '\r'))
    {
	// Get rid of the trailing CRLF.
    	pmBuffer [strlen (pmBuffer) - 2] = 0;
    }
    return pmBuffer;
} // EdFail_GetErrorMessage1


/************************************************************************/
/* EdFail_Warn								*/
/*									*/
/* This procedure is called when Turing has an internal failure, but	*/
/* doesn't necessarily want to exit immediately.  It output a message	*/
/* based on the string resource passed in pmReason.			*/
/************************************************************************/
void	EdFail_Warn (int pmReason, char *pmFile, int pmLine, 
		     int pmErrorCode, ...)
{
    va_list 	myArgList;
    
    va_start (myArgList, pmErrorCode);
    MyFailErrorBox (WARNING, pmReason, pmFile, pmLine, pmErrorCode, myArgList);
    va_end (myArgList);
} // EdFail_Warn


#ifndef TPROLOG
/************************************************************************/
/* EdFail_WriteToLog							*/
/************************************************************************/
void	EdFail_WriteToLog (char *pmString1, char *pmString2, ...)
{
    va_list 		myArgList;
    char		myMessage [1024];
    FilePath		myPanicLogName;
    FileEntryPtr	myPanicFile;
    int			myErrorCode;
    
    myMessage [0] = 0;
    
    if (pmString1 != NULL)
    {
    	strcpy (myMessage, pmString1);
    	strcat (myMessage, ": ");
    }
        
    if (pmString2 != NULL)
    {    
    	va_start (myArgList, pmString2);
    	wvsprintf (&myMessage [strlen (myMessage)], pmString2, myArgList);
    	va_end (myArgList);
    }
    else
    {
    	strcat (myMessage, "<NULL>");
    }
    
    //
    // Write to panic.log
    //
    
    // Get file name.  If EdProp has not been initialized, this will be
    // an empty directory, in which case, use the current directory (which
    // will be the correct one about 90% of the time.
    strcpy (myPanicLogName, EdProp_GetStartupDirectory ());
    if (myPanicLogName [0] == 0)
    {
    	EdFile_GetCurrentDirectory (myPanicLogName);
    }

    MyLoadString (IDS_PANIC_LOG_FILE, &myPanicLogName [strlen (myPanicLogName)], 
        sizeof (myPanicLogName), PANIC_LOG_FILE_REPL);
    myPanicFile = EdFile_OpenFileForWrite (myPanicLogName, &myErrorCode,
    	BUFFER_OUTPUT, APPEND_TO_FILE);
    if (myPanicFile == NULL)
    {
    	EdGUI_Message (pmString1, myMessage);
    }
    else
    {
    	EdFile_WritelnString (myPanicFile, myMessage);
    	EdFile_CloseFile (myPanicFile);
    }
} // EdFail_WriteToLog
#endif // #ifndef TPROLOG


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
    UINT	myLen;
        
    va_start (myArgList, pmString);
    wvsprintf (myStMessage, pmString, myArgList);
    va_end (myArgList);

    myLen = strlen (myStMessage);
    
    if (myLen < stCrashBufferLeft)
    {
    	strcpy (stCrashBufferPtr, myStMessage);
    	stCrashBufferPtr += myLen;
    	stCrashBufferLeft -= myLen;
    }
    else
    {
    	strncpy (stCrashBufferPtr, myStMessage, stCrashBufferLeft);
    	stCrashBufferPtr += stCrashBufferLeft;
    	*stCrashBufferPtr = 0;
    	stCrashBufferLeft = 0;
    }
} // MyAddToBuffer


#ifndef TPROLOG
/************************************************************************/
/* MyAddPreviousCommandsToBuffer					*/
/************************************************************************/
static void	MyAddPreviousCommandsToBuffer (void)
{
    int	myMessageNumber = stProgressMessageHead;
    
    MyAddToBuffer ("\r\n\r\nPrevious Operations:\r\n");
    do
    {
    	if ((stProgressMessage [myMessageNumber] != 0) && 
	    (stProgressMessage [myMessageNumber][0] != 0))
    	{
    	    if (strncmp (stProgressMessage [myMessageNumber], "CMD:", 4) == 0)
    	    {
	        MyAddToBuffer ("  ");
	    }
	    else
	    {
	        MyAddToBuffer ("    ");
	    }
	    MyAddToBuffer (stProgressMessage [myMessageNumber]);
	    MyAddToBuffer ("\r\n");
	}
        myMessageNumber = (myMessageNumber + 1) % NUM_PROGRESS_MESSAGES;
    } while (myMessageNumber != stProgressMessageHead);
    
    MyAddToBuffer ("\r\n\r\n");
} // MyAddPreviousCommandsToBuffer


/************************************************************************/
/* MyEmergencySave							*/
/************************************************************************/
static void	MyEmergencySave (char *pmPathName, HWND pmEditWindow, 
		    		 HWND pmTextDisplayWindow, TextPtr pmTextPtr, 
				 void *pmSaveList)
{
    int		myResult;
    FilePath	myOriginalName, myEmergencyName;
    
    myResult = EdDisp_EmergencySave (pmTextDisplayWindow, pmTextPtr,
    				     myOriginalName, myEmergencyName);
    if (myResult != NOT_SAVED)			             
    {
    	if (pmSaveList != NULL)
    	{
    	    strcat (pmSaveList, "    \"");
    	    strcat (pmSaveList, myOriginalName);
    	    if (myResult == SAVE_FAILED)
    	    {
    	    	strcat (pmSaveList, "\" could not be saved - changes lost.\n");
    	    }
    	    else
    	    {
    	    	strcat (pmSaveList, "\" saved as \"");
    	    	strcat (pmSaveList, myEmergencyName);
    	    	strcat (pmSaveList, "\".\n");
    	    }
    	}
    }			      
} // MyEmergencySave
#endif // #ifndef TPROLOG


/************************************************************************/
/* MyFailErrorBox							*/
/************************************************************************/
static void	MyFailErrorBox (BOOL pmFatal, int pmReason, char *pmFile, 
		     		int pmLine, int pmErrorCode, va_list pmArgs)
{
    int			myTitleStringRsrc;
    char		myWindowTitle [80];
    char		myFinalMessage [4096], *myFinalPtr;
    char		myString [1024], myPartialString [1024];
    char		myFullString [1024];    
    int			myResult;
    ErrorBuffer		myErrorBuffer;
    FilePath		myPanicLogName;
    FileEntryPtr	myPanicFile;
    int			myErrorCode;
//    char		myDateTimeString [256];
    char		*myPtr, *myNewPtr, *myPtr1, *myPtr2;
#ifndef TPROLOG
    StackDumpThreadInfo	myStackDumpThreadInfo;
    HANDLE		myStackDumpThread;
    DWORD		myThreadID;
#endif // #ifndef TPROLOG
        
    if (pmFatal) 
    {
        myTitleStringRsrc = IDS_FATALWINDOWTITLE;
    }
    else
    {
        myTitleStringRsrc = IDS_WARNWINDOWTITLE;
    }
    
    // Get file name.  If EdProp has not been initialized, this will be
    // an empty directory, in which case, use the current directory (which
    // will be the correct one about 90% of the time.
    strcpy (myPanicLogName, EdProp_GetStartupDirectory ());
    if (myPanicLogName [0] == 0)
    {
    	EdFile_GetCurrentDirectory (myPanicLogName);
    }

    MyLoadString (IDS_PANIC_LOG_FILE, &myPanicLogName [strlen (myPanicLogName)], 
        sizeof (myPanicLogName), PANIC_LOG_FILE_REPL);

    // Get the window title (e.g. "Turing Internal Error")
    strcpy (myWindowTitle, gProgram.environmentName);
    strcat (myWindowTitle, " ");
    MyLoadString (myTitleStringRsrc, &myWindowTitle [strlen (myWindowTitle)], 
    		  sizeof (myWindowTitle) - strlen (myWindowTitle),
    		  WINDOW_TITLE_REPL);

    // Get the preamble (e.g. "An unexpected error... Please record...")
    MyLoadString (IDS_FAILPREAMBLE1, myFinalMessage, sizeof (myFinalMessage),
    		  FAIL_PREAMBLE_REPL1);
    strcat (myFinalMessage, myPanicLogName);
    myFinalPtr = myFinalMessage + strlen (myFinalMessage);
    MyLoadString (IDS_FAILPREAMBLE2, myFinalPtr, 
    		  sizeof (myFinalMessage) - strlen (myFinalMessage),
    		  FAIL_PREAMBLE_REPL2);
        		
    // Get the error message (e.g. "LoadAccelerators failed (%d)")
    myResult = LoadString (gProgram.applicationInstance, pmReason,
			   myString, sizeof (myString));
			   
    // Compose the error message			   
    if (myResult == 0)	  
    {
    	// Get the reason not found error message (e.g. "Error code = %d")
    	MyLoadString (IDS_FAILREASONNOTFOUND, myString, sizeof (myString),
    		      FAIL_REASON_NOT_FOUND_REPL);

	wsprintf (myFullString, myString, pmReason);
    }
    else
    {
    	wvsprintf (myPartialString, myString, pmArgs);
    	
    	// Get the reason found error message (e.g. "Error code = %s (%d)")
    	MyLoadString (IDS_FAILREASONFOUND, myString, sizeof (myString),
    		      FAIL_REASON_FOUND_REPL);
	
	wsprintf (myFullString, myString, myPartialString, pmReason);
    }
    strcat (myFinalMessage, myFullString);

    // Compose the Windows error message
    if (pmErrorCode != 0)
    {
        // Get the line/file part (e.g. "Windows error = %s (%d)\n")
        MyLoadString (IDS_FAILWINDOWSERROR, myString, sizeof (myString),
        	      FAIL_WINDOWS_ERROR_REPL);

	EdFail_GetErrorMessage1 (myErrorBuffer, pmErrorCode);

	if (myErrorBuffer [0] == 0)
	{
            MyLoadString (IDS_FAILWINDOWSUNKNOWNERROR, myString, 
			  sizeof (myString), FAIL_WINDOWS_UNKNOWN_ERROR_REPL);
    	    wsprintf (myFullString, myString, pmErrorCode);
	}
	else
	{
    	    wsprintf (myFullString, myString, myErrorBuffer, pmErrorCode);
	}

	strcat (myFinalMessage, myFullString);
    }

    
    // Get the line/file part (e.g. "File=%s\nLine=%s\n")
    MyLoadString (IDS_FAILFILELINE, myString, sizeof (myString), 
    		  FAIL_FILE_LINE_REPL);
    
    wsprintf (myFullString, myString, pmFile, pmLine);
    strcat (myFinalMessage, myFullString);

    if (pmFatal)
    {
        MyLoadString (IDS_FAILSAVEFILE, myString, sizeof (myString),
        	      FAIL_SAVE_FILE_REPL);
    	
    	strcat (myFinalMessage, myString);
    }
    
    // Display the error message    		
    MessageBox (NULL, myFinalMessage, myWindowTitle, 
        MB_OK | MB_ICONERROR | MB_TASKMODAL);
    
    //
    // Write to panic.log
    //
    
    myPanicFile = EdFile_OpenFileForWrite (myPanicLogName, &myErrorCode,
    	BUFFER_OUTPUT, APPEND_TO_FILE);
    if (myPanicFile == NULL)
    {
    	// Write to log
    	EdGUI_Message ("Panic log failure", 
    	    "Unable to open panic log file (\"%s\") for writing.\n"
    		"Error = %d", myPanicLogName, myErrorCode);
    	return;
    }
   
    // Write preamble
    //          *** Fatal Error in Ready 0.98 at Date:Time
    //          Message
    //		----
    if (pmFatal)
    {          
        EdFile_WriteString (myPanicFile, "*** FATAL INTERNAL ERROR");
    }
    else
    {
        EdFile_WriteString (myPanicFile, "*** NON-FATAL INTERNAL ERROR");
    }
    EdFile_WriteString (myPanicFile, " in ");
    EdFile_WriteString (myPanicFile, gProgram.environmentName);
    EdFile_WritelnString (myPanicFile, " ***");

    // Get information about the application and save it to the crash buffer
    stCrashBufferPtr = stCrashBuffer;
    stCrashBufferLeft = CRASH_BUFFER_SIZE - 10;
    MyGetEnvironmentInfo ();
    
#ifndef TPROLOG
    // Add previous operations
    MyAddPreviousCommandsToBuffer ();	    
#endif // #ifndef TPROLOG

/*  GetDateFormat (LOCALE_USER_DEFAULT, 0, NULL, "MMM d yyyy", 
        myDateTimeString, sizeof (myDateTimeString));
    EdFile_WriteString (myPanicFile, myDateTimeString);
    
    GetTimeFormat (LOCALE_USER_DEFAULT, TIME_NOSECONDS, NULL, NULL, 
        myDateTimeString, sizeof (myDateTimeString));
    EdFile_WriteString (myPanicFile, " ");
    EdFile_WriteString (myPanicFile, myDateTimeString);
*/    
    EdFile_WritelnString (myPanicFile, stCrashBuffer);
    
    // Write the message, writing each line seperately
    myPtr = myFinalMessage;
    myPtr1 = strchr (myPtr, '\n');
    myPtr2 = strchr (myPtr, '\r');
    while ((myPtr1 != NULL) || (myPtr2 != NULL))
    {
    	if (myPtr2 < myPtr1 - 1)
    	{
    	    *myPtr2 = 0;
    	    myNewPtr = myPtr2 + 1;
    	}
    	else if (myPtr2 == myPtr1 - 1)
    	{
    	    *myPtr2 = 0;
    	    myNewPtr = myPtr1 + 1;
	}
	else
	{
    	    *myPtr1 = 0;
    	    myNewPtr = myPtr1 + 1;
	}
    	EdFile_WritelnString (myPanicFile, myPtr);
    	myPtr = myNewPtr;
    	myPtr1 = strchr (myPtr, '\n');
    	myPtr2 = strchr (myPtr, '\r');
    }
    if (*myPtr != 0)
    {
    	EdFile_WritelnString (myPanicFile, myPtr);
    }
    
#ifndef TPROLOG
    //
    // Add a stack dump
    //
    
    // First, we need to get the context of the current thread.  To do this,
    // we must suspend the current thread and have another thread actually
    // do the stack dump.
    DuplicateHandle (GetCurrentProcess(), GetCurrentThread (),
                     GetCurrentProcess(), &myStackDumpThreadInfo.thread, 0, FALSE, 
                     DUPLICATE_SAME_ACCESS);
    myStackDumpThreadInfo.file = myPanicFile;
    myStackDumpThread = CreateThread (NULL, 0, 
        (LPTHREAD_START_ROUTINE) MyStackDump, &myStackDumpThreadInfo, 0, 
        &myThreadID);
    if (myStackDumpThread != NULL)
    {
    	// Sleep for two seconds while the stack dump is taking place.
	WaitForSingleObject (myStackDumpThread, 15000);
    }
#endif // #ifndef TPROLOG

    EdFile_WritelnString (myPanicFile, LINE_OF_DASHES);
    EdFile_CloseFile (myPanicFile);
} // MyFailErrorBox   


/************************************************************************/
/* MyGetEnvironmentInfo							*/
/************************************************************************/
static void	MyGetEnvironmentInfo (void)
{
    static FilePath	stMyApplicationPath;
    static DWORD	stMyDummy;
    static DWORD	stMyVersionSize;
    static char		*stMyVersionInfo;
    static UINT		stMyVersionInfoSize;
    static char		stMyTempString [256];
    static HANDLE	stMyFile;
    static DWORD	stMyFileSize;
    static FILETIME	stMyFileTime, stMyDummy1, stMyDummy2;
    static SYSTEMTIME	stMySystemTime;
    	
    //
    // Line 1: Date: Oct 20, 2000
    //
    
    // Start with current date label
    MyAddToBuffer ("Date: ");

    // Add the current date
    GetDateFormat (LOCALE_USER_DEFAULT, 0, NULL, "MMM d yyyy", 
        stMyTempString, sizeof (stMyTempString));
    MyAddToBuffer (stMyTempString);

    // Add a space
    MyAddToBuffer (" ");
    
    // Add the current time    
    GetTimeFormat (LOCALE_USER_DEFAULT, TIME_NOSECONDS, NULL, NULL, 
        stMyTempString, sizeof (stMyTempString));
    MyAddToBuffer (stMyTempString);
    
    // Add a newline
    MyAddToBuffer ("\r\n\r\n");
    
    //
    // Line 2: File Name: d:\ready\ready.exe
    //
    
    // Start with file name label
    MyAddToBuffer ("File Name: ");

    // Add the application path
    GetModuleFileName (NULL, stMyApplicationPath, sizeof (stMyApplicationPath));
    MyAddToBuffer (stMyApplicationPath);

    // Add a newline
    MyAddToBuffer ("\r\n");
    
    //
    // Line 3 (Maybe): Version: 1.0.2  [Mini/Restricted/IBM/71]
    //

    // Start with the version label
    MyAddToBuffer ("Version: ");
    
    // Set to empty string by default
    stMyVersionSize = GetFileVersionInfoSize (stMyApplicationPath, &stMyDummy);
    if ((stMyVersionSize != 0) && (stMyVersionSize <= stCrashBufferLeft))
    {
	if (GetFileVersionInfo (stMyApplicationPath, stMyDummy, 
				stCrashBufferLeft, stCrashBufferPtr))
	{
	    if (VerQueryValue (stCrashBufferPtr, 
			    "\\StringFileInfo\\04090000\\ProductVersion", 
			    &stMyVersionInfo, &stMyVersionInfoSize))
	    {			    
		// Add the version number (size includes \0)
		memmove (stCrashBufferPtr, stMyVersionInfo, 
			 stMyVersionInfoSize);
		stCrashBufferLeft -= stMyVersionInfoSize - 1;
		stCrashBufferPtr += stMyVersionInfoSize - 1; 
	    }
	    else
	    {
	    	MyAddToBuffer ("Unknown");
	    }
	}
	else
	{
	    MyAddToBuffer ("Unknown");
	}
    }
    else
    {
    	MyAddToBuffer ("Unknown");
    }
    
    if (gProgram.globalsInitialized)
    {
	// Add a spaces
	MyAddToBuffer ("  [");

	if (gProgram.miniVersion)
            MyAddToBuffer ("Mini/");
	if (gProgram.restrictedVersion)
            MyAddToBuffer ("Restricted/");
	if (gProgram.assistedByIBM)
            MyAddToBuffer ("IBM/");

	if (gProgram.expiryDateString [0] != 0)
	{
            MyAddToBuffer (gProgram.expiryDateString);
            MyAddToBuffer ("/");
	}

	wsprintf (stMyTempString, "%d]", gProgram.installKind);										    					        
        MyAddToBuffer (stMyTempString);
    } // if (gProgram.globalsInitialized)
		    
    // Add a newline
    MyAddToBuffer ("\r\n");
    
    //
    // Line 4: File Size: 2876346
    //
    stMyFile = CreateFile (stMyApplicationPath, GENERIC_READ, FILE_SHARE_READ,
                         NULL, OPEN_EXISTING, 0, 0);
    if (stMyFile != (HANDLE) INVALID_HANDLE_VALUE)
    {
    	stMyFileSize = GetFileSize (stMyFile, NULL);

    	// Start with the size label
	MyAddToBuffer ("File Size: ");
    
	// Add the file size
	wsprintf (stMyTempString, "%d\r\n", stMyFileSize);
	MyAddToBuffer (stMyTempString);
    	
    	//
    	// Line 5: File Date: Oct 10, 2000
    	//
    	GetFileTime (stMyFile, &stMyDummy1, &stMyDummy2, &stMyFileTime);
	if (FileTimeToSystemTime (&stMyFileTime, &stMySystemTime))
	{
    	    // Start with the date label
	    MyAddToBuffer ("File Date: ");
    
	    // Add the file date
	    GetDateFormat (LOCALE_USER_DEFAULT, 0, &stMySystemTime, 
	    	"MMM d yyyy", stMyTempString, sizeof (stMyTempString));
	    MyAddToBuffer (stMyTempString);
	    MyAddToBuffer (" ");
    	
	    // Add the file time
	    GetTimeFormat (LOCALE_USER_DEFAULT, TIME_NOSECONDS, &stMySystemTime, 
	        NULL, stMyTempString, sizeof (stMyTempString));
	    MyAddToBuffer (stMyTempString);
	}
	
	CloseHandle (stMyFile);
    }
    
    //
    // Line 6: Operating System: Windows 95
    //
    MyAddToBuffer ("\r\nOperating System: ");
    switch (gProgram.operatingSystem)
    {
    	case UNKNOWN_OS:
    	    MyAddToBuffer ("Unknown Windows version\r\n");
    	    break;
    	case WIN_95:
    	    MyAddToBuffer ("Windows 95\r\n");
    	    break;
	case WIN_95_OSR2:
    	    MyAddToBuffer ("Windows 95 Service Pack 2\r\n");
    	    break;
	case WIN_98:
    	    MyAddToBuffer ("Windows 98\r\n");
    	    break;
	case WIN_98_SE:
    	    MyAddToBuffer ("Windows 98 Second Edition\r\n");
    	    break;
	case WIN_ME:
    	    MyAddToBuffer ("Windows Me\r\n");
    	    break;
	case WIN_NT:
    	    MyAddToBuffer ("Windows NT\r\n");
    	    break;
	case WIN_2000:
    	    MyAddToBuffer ("Windows 2000\r\n");
    	    break;
	case WIN_XP:
    	    MyAddToBuffer ("Windows XP\r\n");
    	    break;
	case WIN_NEWER:
    	    MyAddToBuffer ("After Windows XP\r\n");
    	    break;
    	default:
    	    MyAddToBuffer ("Bad OS Value (%d)\r\n", gProgram.operatingSystem);
    	    break;
    	
    } // switch
    
    // 
    // Line 7: JVM: Ready Built-in JRE (1.4.2 or later)
    //
    MyAddToBuffer ("JVM: ");
    switch (gProperties.JVMType)
    {
	case JRE_131:
	    MyAddToBuffer ("Ready JRE 1.3.1\r\n");
	    break;
	case JRE_BUILT_IN:
	    MyAddToBuffer ("Ready Built-in JRE (1.4.2 or later)\r\n");
	    break;
	case JRE_IN_JRE_DIR:
	    MyAddToBuffer ("Ready User-installed JRE\r\n");
	    break;
	case JDK_IN_JRE_DIR:
	    MyAddToBuffer ("Ready User-installed JDK\r\n");
	    break;
	case JRE_IN_REGISTRY:
	    MyAddToBuffer ("Ready System JRE\r\n");
	    break;
	case JDK_IN_REGISTRY:
	    MyAddToBuffer ("Ready System JDK\r\n");
	    break;
    } // switch            
    
    // 
    // Line 8: "assert": enabled
    //
    if (gProperties.oldJavaCompile)
    {
    	MyAddToBuffer ("\"assert\" Statements: disabled\r\n");
    }
    else
    {
    	MyAddToBuffer ("\"assert\" Statements: enabled\r\n");
    }    	
} // MyGetEnvironmentInfo


/************************************************************************/
/* MyLoadString								*/
/*									*/
/* For drastic error messages, we try an load the message from a file.	*/
/* However, if for some reason it fails, we use an English replacement	*/
/* string so that we never have a blank error message window.		*/
/************************************************************************/
static void	MyLoadString (int pmStringResource, char *pmString, 
			      size_t pmStringSize, const char *pmReplaceString)
{
    int	myResult;
    
    myResult = LoadString (gProgram.applicationInstance, pmStringResource, 
    			   pmString, pmStringSize);
    if (myResult == 0)
    {
	strcpy (pmString, pmReplaceString);
    }	    	    
} // MyLoadString
			      

#ifndef TPROLOG
/************************************************************************/
/* MySaveAllFiles							*/
/*									*/
/* Save all open files.							*/
/************************************************************************/
static void	MySaveAllFiles (void)
{
    char	*myEmergencyFilesSavedList;
    
    //
    // Save all unsaved open files
    //
    myEmergencyFilesSavedList = malloc (4096);
    if (myEmergencyFilesSavedList != NULL)
    {
    	myEmergencyFilesSavedList [0] = 0;
    }
    Ed_EnumerateFiles (MyEmergencySave, myEmergencyFilesSavedList);
    if (myEmergencyFilesSavedList != NULL)
    {
    	if (myEmergencyFilesSavedList [0] == 0)
    	{
	    EdGUI_Message ("No Files Saved", "There were no unsaved files");
	}
	else
	{
	    EdGUI_Message ("Files Saved", 
	        "The following files were saved:\n%s", 
	        myEmergencyFilesSavedList);
	}
    }		
} // MySaveAllFiles


/************************************************************************/
/* MyStackDump								*/
/************************************************************************/
static int	MyStackDump (LPVOID pmThreadData)
{
    HANDLE		myThread;
    FileEntryPtr	myFile;
    CONTEXT		myContext;

    Sleep (100);
    
    myThread = ((StackDumpThreadInfo *) pmThreadData) -> thread;
    myFile = ((StackDumpThreadInfo *) pmThreadData) -> file;

    SuspendThread (myThread);

    myContext.ContextFlags = CONTEXT_FULL;
    if (GetThreadContext (myThread, &myContext))
    {
    	EdFile_WritelnString (myFile, "");
    	stCrashBufferPtr = stCrashBuffer;
    	stCrashBufferLeft = CRASH_BUFFER_SIZE - 10;
    	EdCrash_StackDump (stCrashBufferPtr, stCrashBufferLeft, 
    		           &myContext);
    	EdFile_WritelnString (myFile, stCrashBuffer);
    }
    	
    ResumeThread (myThread);

    return 0;
} // MyStackDump
#endif // #ifndef TPROLOG
