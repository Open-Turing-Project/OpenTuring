/**********/
/* jsll.c */
/**********/

/*******************/
/* System includes */
/*******************/

/****************/
/* Self include */
/****************/
#include "jsdll.h"

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
static char		*stBufferPtr = NULL;
static char		*stBuffer = NULL;
static char		*stOutputBuffer = NULL;
static BOOL		stStdinEOF = FALSE;
// static BOOL		stReachedEndOfLine = TRUE;
static HANDLE		stNeedALineEvent, stHaveALineEvent = NULL;
static HANDLE		stLineAvailEvent, stLineReadEvent;
static CRITICAL_SECTION	stReadLineCriticalSection, stSetLineCriticalSection;
static CRITICAL_SECTION	stWriteLineCriticalSection, stGetLineCriticalSection;
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
    	// Open signals
    	stLineAvailEvent = CreateEvent (NULL, TRUE, FALSE, NULL);
    	stLineReadEvent = CreateEvent (NULL, TRUE, FALSE, NULL);
    	
    	// Create Critical Section
    	InitializeCriticalSection (&stReadLineCriticalSection);
    	InitializeCriticalSection (&stSetLineCriticalSection);
    	InitializeCriticalSection (&stWriteLineCriticalSection);
    	InitializeCriticalSection (&stGetLineCriticalSection);
    }    
    return TRUE;
} // DllMain


/************************************************************************/
/* Java_ReadyInputStream_read						*/
/************************************************************************/
JNIEXPORT jint JNICALL Java_ready_ReadyInputStream_read (JNIEnv *pmEnv, 
							 jobject pmThis)
{
    // Enter the critical section so that we only have one thing asking for
    // input at a time.
    char	myChar;
    
    EnterCriticalSection (&stReadLineCriticalSection);
    
    // If we've received an EOF
    if (stStdinEOF)
    {
    	return -1;
    }
/*    
    if (((stBufferPtr == NULL) || (*stBufferPtr == 0)) && (!stReachedEndOfLine))
    {
    	stReachedEndOfLine = TRUE;
    	return -1;
    }
*/
    if (stNeedALineEvent == NULL)
    {
    	stNeedALineEvent = OpenEvent (EVENT_ALL_ACCESS, FALSE, 
				      "JavaSupNeedALine");
    }
	
    // Check to see if we can send back a character immediately
    while (((stBufferPtr == NULL) || (*stBufferPtr == 0)) && !stStdinEOF)
    {
    	// Signal main proram that we need a line
	ResetEvent (stLineAvailEvent);
    	SetEvent (stNeedALineEvent);
    	
    	// Wait for the line
    	WaitForSingleObject (stLineAvailEvent, INFINITE);
    }
    
    // If we've received an EOF
    if (stStdinEOF)
    {
    	// Log the output	
    	MyWriteLog ("[1005] From JSDLL: Read from Console: EOF");
    
    	return -1;
    }
    
    myChar = *stBufferPtr;
    stBufferPtr++;
    	    
    // Log the output	
    MyWriteLog ("[1006] From JSDLL: Read from Console: \"%c\"", myChar);
    
    LeaveCriticalSection (&stReadLineCriticalSection);

    return myChar;
} // Java_ReadyInputStream_read


/************************************************************************/
/* Java_ready_ReadyInputStream_readBytes				*/
/************************************************************************/
JNIEXPORT jint JNICALL Java_ready_ReadyInputStream_readBytes (JNIEnv *pmEnv, 
							  jobject pmThis,
							  jbyteArray pmBytes,
							  jint pmOffset,
							  jint pmCount)
{
    // Enter the critical section so that we only have one thing asking for
    // input at a time.
    char	*myPtr;
    int		myNumBytesAvailable = 0;
    
    EnterCriticalSection (&stReadLineCriticalSection);
    
    // If we've received an EOF
    if (stStdinEOF)
    {
    	return -1;
    }
/*    
    if (((stBufferPtr == NULL) || (*stBufferPtr == 0)) && (!stReachedEndOfLine))
    {
    	stReachedEndOfLine = TRUE;
    	return -1;
    }
*/
    if (stNeedALineEvent == NULL)
    {
    	stNeedALineEvent = OpenEvent (EVENT_ALL_ACCESS, FALSE, 
				      "JavaSupNeedALine");
    }

    // Check to see if we can send back a character immediately
    while (((stBufferPtr == NULL) || (*stBufferPtr == 0)) && !stStdinEOF)
    {
    	// Signal main proram that we need a line
	ResetEvent (stLineAvailEvent);
    	SetEvent (stNeedALineEvent);
    	
    	// Wait for the line
    	WaitForSingleObject (stLineAvailEvent, INFINITE);
    }
    
    // If we've received an EOF
    if (stStdinEOF)
    {
    	// Log the output	
    	MyWriteLog ("[1003] From JSDLL: Read from Console: EOF");
    
    	return -1;
    }
    
    // Log the output	
    MyWriteLog ("[1004] From JSDLL: Read from Console: \"%s\"", stBufferPtr);
    
    //
    // We now have one or more characters in the input buffer that we can
    // send back.
    //
    // How many bytes are available in the buffer?
    myPtr = stBufferPtr;
    while (*myPtr != 0)
    {
    	myPtr++;
    	myNumBytesAvailable++;
    }
    
    myNumBytesAvailable = min (myNumBytesAvailable, pmCount);

    (*pmEnv) -> SetByteArrayRegion (pmEnv, pmBytes, pmOffset, 
    	myNumBytesAvailable, stBufferPtr);

    stBufferPtr += myNumBytesAvailable;
    	    
    LeaveCriticalSection (&stReadLineCriticalSection);

    return myNumBytesAvailable;
} // Java_ready_ReadyInputStream_readBytes

							  
/************************************************************************/
/* Java_ready_ReadyOutputStream_write					*/
/************************************************************************/
JNIEXPORT void JNICALL Java_ready_ReadyOutputStream_write (JNIEnv *pmEnv, 
							   jobject pmThis,
							   jbyte pmByte)
{
    // Enter the critical section so that we only have one thing displaying
    // output at a time.
    EnterCriticalSection (&stWriteLineCriticalSection);
    
    stOutputBuffer = GlobalAlloc (GPTR, 2);
    stOutputBuffer [0] = pmByte;
    stOutputBuffer [1] = 0;

    // Log the output	
    MyWriteLog ("[1002] From JSDLL: Output to Console: \"%s\"", stOutputBuffer);
    
    // Signal main proram that we need a line
    ResetEvent (stLineReadEvent);
    
    if (stHaveALineEvent == NULL)
    {
    	stHaveALineEvent = OpenEvent (EVENT_ALL_ACCESS, FALSE, 
    				      "JavaSupHaveALine");
    }
    
    SetEvent (stHaveALineEvent);
    	
    // Wait for the line
    WaitForSingleObject (stLineReadEvent, INFINITE);
    	    
    LeaveCriticalSection (&stWriteLineCriticalSection);
} // Java_ready_ReadyOutputStream_write


/************************************************************************/
/* Java_ready_ReadyOutputStream_writeBytes				*/
/************************************************************************/
JNIEXPORT void JNICALL Java_ready_ReadyOutputStream_writeBytes (JNIEnv *pmEnv, 
							  jobject pmThis,
							  jbyteArray pmBytes,
							  jint pmOffset,
							  jint pmCount)
{
    // Enter the critical section so that we only have one thing displaying
    // output at a time.
    EnterCriticalSection (&stWriteLineCriticalSection);
    
    stOutputBuffer = GlobalAlloc (GPTR, pmCount + 1);
    stOutputBuffer [pmCount] = 0;
    
    (*pmEnv) -> GetByteArrayRegion (pmEnv, pmBytes, pmOffset, pmCount, 
    	stOutputBuffer);
    
    // Log the output	
    MyWriteLog ("[1001] From JSDLL: Output to Console: \"%s\"", stOutputBuffer);
    
    // Signal main proram that we need a line
    ResetEvent (stLineReadEvent);
    
    if (stHaveALineEvent == NULL)
    {
    	stHaveALineEvent = OpenEvent (EVENT_ALL_ACCESS, FALSE, 
    				      "JavaSupHaveALine");
    }
    
    SetEvent (stHaveALineEvent);
    	
    // Wait for the line
    WaitForSingleObject (stLineReadEvent, INFINITE);
    	    
    LeaveCriticalSection (&stWriteLineCriticalSection);
} // Java_ready_ReadyOutputStream_writeBytes


/************************************************************************/
/* DLL_GetWrittenLine							*/
/************************************************************************/
EXPORT char * CALLBACK DLL_GetWrittenLine (void)
{
    // Enter the critical section so that we only have one thing displaying
    // output at a time.
    char	*myLine;
    
    EnterCriticalSection (&stGetLineCriticalSection);
    
    myLine = stOutputBuffer;
    stOutputBuffer = NULL;
    	
    SetEvent (stLineReadEvent);

    LeaveCriticalSection (&stGetLineCriticalSection);
    
    return myLine;
} // DLL_GetWrittenLine


/************************************************************************/
/* DLL_SetLineToRead							*/
/************************************************************************/
EXPORT void CALLBACK DLL_SetLineToRead (char *pmLine)
{
    // Enter the critical section so that we only have one thing asking for
    // input at a time.
    EnterCriticalSection (&stSetLineCriticalSection);
    	
    // An empty string is our signal that we've received an EOF
    if (pmLine [0] == 0)
    {
    	stStdinEOF = TRUE;
    }
    
    if (stBuffer != NULL)
    {
    	stBufferPtr = NULL;
    	free (stBuffer);
    }
    stBuffer = malloc (strlen (pmLine) + 1);
    
    if (stBuffer != NULL)
    {
    	strcpy (stBuffer, pmLine);
    	stBufferPtr = stBuffer;
//    	stReachedEndOfLine = FALSE;
    }
    
    SetEvent (stLineAvailEvent);
    
    LeaveCriticalSection (&stSetLineCriticalSection);
} // DLL_SetLineToRead


/************************************************************************/
/* DLL_SetLoggingInfo							*/
/************************************************************************/
EXPORT void CALLBACK DLL_SetLogging (BOOL pmLogging, HANDLE pmLogFileHandle)
{
    if (pmLogging)
    {
	stLogging = TRUE;
	stLogFileHandle = pmLogFileHandle;
	
	MyWriteLog ("[1000] From JSDLL: Logging started");
    }
} // DLL_SetLogging


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


    	