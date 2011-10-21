/************/
/* edjdll.c */
/************/

/*******************/
/* System includes */
/*******************/

/****************/
/* Self include */
/****************/
#include "edjdll.h"

/******************/
/* Other includes */
/******************/
#include "jni.h"

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
static void	(*stNotifyDebuggerDisconnect) (void);
static void	(*stOutputFromDebugger) (int pmReason, 
					 const char *pmOutputBuffer);
/*static void	(*stMessageBox) (char *pmTitle, char *pmMessage, ...);
static void	(*stAddError) (char *pmPathName, WORD pmTuringFileNo,
			       int pmLineFrom, int pmColFrom, 
			       int pmLineTo, int pmColTo, 
			       const char *pmErrorMessage);
static void	(*stFindFirstError) (HWND pmTextDisplayWindow, 
				     BOOL pmMessageIfNoError,
				     BOOL pmActivateSourceWindow);
static char	*stCodeBase;
/*
static char		*stBufferPtr = NULL;
static char		*stBuffer = NULL;
static char		*stOutputBuffer = NULL;
static BOOL		stStdinEOF = FALSE;
// static BOOL		stReachedEndOfLine = TRUE;
static HANDLE		stNeedALineEvent, stHaveALineEvent = NULL;
static HANDLE		stLineAvailEvent, stLineReadEvent;
static CRITICAL_SECTION	stReadLineCriticalSection, stSetLineCriticalSection;
static CRITICAL_SECTION	stWriteLineCriticalSection, stGetLineCriticalSection;
*/
// For logging
static BOOL		stLogging = FALSE;
static HANDLE		stLogFileHandle = NULL;

/******************************/
/* Static callback procedures */
/******************************/

/*********************/
/* Static procedures */
/*********************/
static void	MyWriteLog (char *pmFormatString, ...);

/***********************/
/* External procedures */
/***********************/

/************************************************************************/
/* DllMain								*/
/************************************************************************/
int WINAPI DllMain (HINSTANCE pmInstance, DWORD pmReason, LPVOID pmReserved)
{
    if (pmReason == DLL_PROCESS_ATTACH)
    {
/*
	// Open signals
    	stLineAvailEvent = CreateEvent (NULL, TRUE, FALSE, NULL);
    	stLineReadEvent = CreateEvent (NULL, TRUE, FALSE, NULL);
    	
    	// Create Critical Section
    	InitializeCriticalSection (&stReadLineCriticalSection);
    	InitializeCriticalSection (&stSetLineCriticalSection);
    	InitializeCriticalSection (&stWriteLineCriticalSection);
    	InitializeCriticalSection (&stGetLineCriticalSection);
*/
    }    
    return TRUE;
} // DllMain


/************************************************************************/
/* DLL_Initialize							*/
/************************************************************************/
EXPORT void __cdecl DLL_Initialize (void (*pmNotifyDebuggerDisconnect) (void),
				   void (*pmOutputFromDebugger) (int pmReason, 
					       const char *pmOutputBuffer))
{
    stNotifyDebuggerDisconnect = pmNotifyDebuggerDisconnect;
    stOutputFromDebugger = pmOutputFromDebugger;
} // DLL_Initialize


/************************************************************************/
/* DLL_SetLoggingInfo							*/
/************************************************************************/
EXPORT void __cdecl DLL_SetLogging (BOOL pmLogging, HANDLE pmLogFileHandle)
{
    if (pmLogging)
    {
	stLogging = TRUE;
	stLogFileHandle = pmLogFileHandle;
	
	MyWriteLog ("[2000] From JSDLL: Logging started");
    }
} // DLL_SetLogging


/************************************************************************/
/* Java_ready_ReadyDebug_notifyDebuggerDisconnect			*/
/************************************************************************/
JNIEXPORT void JNICALL Java_ready_ReadyDebug_notifyDebuggerDisconnect (
					    JNIEnv *pmEnv, jclass pmClass)
{
    (*stNotifyDebuggerDisconnect) ();
} // Java_ready_ReadyDebug_notifyDebuggerDisconnect


/************************************************************************/
/* Java_ready_ReadyDebug_outputBytes					*/
/************************************************************************/
JNIEXPORT void JNICALL Java_ready_ReadyDebug_outputBytes (JNIEnv *pmEnv,
				jclass pmClass, jint pmPurpose, jint pmSize, 
				jbyteArray pmData)
{
    char    *myOutputBuffer;

    // We need to use GlobalAlloc, because we will be using this pointer
    // (and eventually freeing this pointer) outside this DLL (and the
    // DLL's heap).
    myOutputBuffer = GlobalAlloc (GPTR, pmSize + 1);
    myOutputBuffer [pmSize] = 0;
    
    (*pmEnv) -> GetByteArrayRegion (pmEnv, pmData, 0, pmSize, 
    	myOutputBuffer);

    stOutputFromDebugger (pmPurpose, myOutputBuffer);
} // Java_ready_ReadyDebug_outputBytes


/************************************************************************/
/* MyWriteLog								*/
/************************************************************************/
static void	MyWriteLog (char *pmFormatString, ...)
{
    va_list	myArgList;
    char	myFullMessage [2048];
    DWORD	myNumBytesWritten;
    
    if (stLogging)
    {
        va_start (myArgList, pmFormatString);
        wvsprintf (myFullMessage, pmFormatString, myArgList);
        va_end (myArgList);
        
    	WriteFile (stLogFileHandle, myFullMessage, strlen (myFullMessage), 
    		   &myNumBytesWritten, NULL);
    	WriteFile (stLogFileHandle, "\r\n", 2, &myNumBytesWritten, NULL);
        FlushFileBuffers (stLogFileHandle);
    }
} // MyWriteLog


    	