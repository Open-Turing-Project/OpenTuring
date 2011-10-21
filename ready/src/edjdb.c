/***********/
/* edjdb.c */
/***********/

/*******************/
/* System includes */
/*******************/

/****************/
/* Self include */
/****************/
#include "edjdb.h"

/******************/
/* Other includes */
/******************/
#include "jni.h"

#include "ed.h"
#include "edfail.h"
#include "edfile.h"
#include "edglob.h"
#include "edgui.h"
#include "edlog.h"
#include "edrun.h"
#include "edwin.h"
//#include "edjdll.h"
#include "eddialogs.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
#define READY_DEBUGGER_JVM_STARTED	"ReadyDebuggerJVMStarted"
#define CLASSPATH			"-Djava.class.path="
#define LIBRARY_PATH			"-Djava.library.path="
#define READY_DEBUG_CLASS_NAME		"ready/ReadyDebug"
// Method names
#define READY_LOAD_METHOD_NAME		"attachToNothing"
#define READY_ATTACH_METHOD_NAME	"attachToTarget"
#define READY_DETACH_METHOD_NAME	"detachFromTarget"
#define READY_PAUSE_METHOD_NAME		"pauseVM"
#define READY_RESUME_METHOD_NAME	"resumeVM"

// Starting JVM Errors
#define CANT_LOAD_JVM			-20
#define CANT_FIND_PROC			-21
#define JVM_CREATION_FAILED		-22
#define CANT_FIND_DEBUGGER_CLASS	-23
#define CANT_FIND_DEBUGGER_METHOD	-24
#define CANT_LOAD_SUN_DEBUG_CLASS	-25

/********************/
/* Global variables */
/********************/

/*********/
/* Types */
/*********/
typedef jint (JNICALL *JNI_CreateJavaVMCall) (JavaVM **pvm, void **penv, 
					      void *args);
typedef void (__cdecl *DLL_Initialize) (
		void (*pmNotifyDebuggerDisconnect) (void),
		void (*pmOutputFromDebugger) (int pmReason, 
					      const char *pmOutputBuffer));
					      
/**********************/
/* External variables */
/**********************/

/********************/
/* Static constants */
/********************/

/********************/
/* Static variables */
/********************/
// Flag for if we've tried and failed to start the debugger
static BOOL		stCantStartDebugger = FALSE;
// The JNI Environment for the debugger
static JNIEnv		*stJNIEnv = NULL;
// The Java VM for the debugger
static JavaVM		*stDebuggerJVM = NULL;
// Signal indicating we're done
static HANDLE		stNotifyDebuggerJVMStarted;
// Error we encountered
static int		stStartJVMError = 0;
// Error Strings
static char		stErrorString1 [1024], stErrorString2 [1024];
static int		stErrorNumber;
// Are we attached to the current VM
static BOOL		stAttachedToCurrentVM = FALSE;
// The debugger class and methods in it.
static jclass		stDebuggerClass;
static jmethodID	stAttachMethod, stDetachMethod;
static jmethodID	stPauseMethod, stResumeMethod;

/******************************/
/* Static callback procedures */
/******************************/

/*********************/
/* Static procedures */
/*********************/
static BOOL			MyAttachIfNecessary (void);
static BOOL			MyAttachToCurrentVM (void);
static JNI_CreateJavaVMCall	MyFindJNIAddress (char *pmDLLPath, 
						  char *pmProcName);
static void			*MyFindLibraryAddress (char *pmDLLPath, 
						       char *pmProcName);
static void			MyNotifyDebuggerDisconnect (void);
static BOOL			MyStartJavaVM (void);
static int			MyStartJavaVMThread (LPVOID pmThreadData);
static void			MyStartTracing (void);
static void			MyStopTracing (void);


/***********************/
/* External procedures */
/***********************/
/************************************************************************/
/* EdJdb_Init								*/
/************************************************************************/
BOOL	EdJdb_Init (char *pmCodeBase)
{
    FilePath		myDLLPathName;
    HMODULE		myLibrary;
    DLL_Initialize	myDLL_Initialize;
    HANDLE		myThread;
    DWORD		myThreadID;

    EdFile_ConstructPath (myDLLPathName, IDS_READY_BIN, IDS_JDB_DLL);
    
    myLibrary = LoadLibrary (myDLLPathName);
    if (myLibrary == NULL)
    {
    	EdFail_Warn (IDS_LOADLIBRARYFAIL, __FILE__, __LINE__, GetLastError (),
    		     myDLLPathName);
    	return FALSE;
    }

    myDLL_Initialize = (DLL_Initialize) GetProcAddress (myLibrary, 
    							"DLL_Initialize");
    if (myDLL_Initialize == NULL)
    {
    	EdFail_Warn (IDS_GETPROCADDRESSFAIL, __FILE__, __LINE__, GetLastError (),
    		     "DLL_Initialize", myDLLPathName);
    	return FALSE;
    }
    
    (*myDLL_Initialize) (MyNotifyDebuggerDisconnect, EdJdb_DebuggerOutput_T);

    stNotifyDebuggerJVMStarted = CreateEvent (NULL, TRUE, FALSE, 
    					      READY_DEBUGGER_JVM_STARTED);
    ResetEvent (stNotifyDebuggerJVMStarted);

    // Create a thread that will load the debugger JVM.
    myThread = CreateThread (NULL, 0, 
        (LPTHREAD_START_ROUTINE) MyStartJavaVMThread, NULL, 0, 
        &myThreadID);
    if (myThread == 0)
    {
    	ErrorBuffer	myErrorBuffer;
    	
	EdGUI_Message1 (NULL, 0, IDS_CANT_START_JAVAVM_TITLE,
			IDS_JVM_THREAD_CREATION_FAILED, 
			EdFail_GetErrorMessage (myErrorBuffer), 
			GetLastError ());
    	EdLog_Log ("CreateThread for Debugger JVM failed with error = %d",
	    GetLastError ());
    }
    
    return TRUE;
} // EdJdb_Init


/************************************************************************/
/* EdJdb_DetachFromVM							*/
/************************************************************************/
void	EdJdb_DetachFromVM (void)
{
    if (stDebuggerJVM != NULL)
    {
	if (stAttachedToCurrentVM)
	{
	    // TW Detach from VM
	    stAttachedToCurrentVM = FALSE;
	}
    }
} // EdJdb_DetachFromVM


/************************************************************************/
/* EdJdb_DebuggerOutput_T						*/
/*									*/
/* NOTE: This routine operates in the thread created by the ReadyDebug	*/
/* Java class that is used to process events from the Debugger JVM	*/
/*									*/
/* It sends output to the Windows dispatcher so it will be placed in	*/
/* the main thread.							*/
/************************************************************************/
void	EdJdb_DebuggerOutput_T (int pmPurpose, const char *pmOutputBuffer)
{
    HWND	myCurrentEditorWindow = Ed_GetLastActiveEditWindow ();
    
    SendNotifyMessage (myCurrentEditorWindow, WM_DEBUGGER_OUTPUT, 
	    	       (WPARAM) pmOutputBuffer, (LPARAM) pmPurpose);
} // EdJdb_DebuggerOutput_T


/************************************************************************/
/* EdJdb_Pause								*/
/************************************************************************/
void	EdJdb_Pause (void)
{
    if (!MyAttachIfNecessary ()) return;

    (*stJNIEnv) -> CallStaticVoidMethod (stJNIEnv, stDebuggerClass, 
					 stPauseMethod, NULL);
    if ((*stJNIEnv) -> ExceptionOccurred (stJNIEnv) != NULL)
    {
	EdGUI_Message1 (NULL, 0, IDS_DEBUGGER_FAILURE_TITLE,
			IDS_EXCEPTIONOCCUREDINPAUSEMETHOD);
	stCantStartDebugger = TRUE;
    }
} // EdJdb_Pause


/************************************************************************/
/* EdJdb_Resume								*/
/************************************************************************/
void	EdJdb_Resume (void)
{
    if (!MyAttachIfNecessary ()) return;

    (*stJNIEnv) -> CallStaticVoidMethod (stJNIEnv, stDebuggerClass, 
					 stResumeMethod, NULL);
    if ((*stJNIEnv) -> ExceptionOccurred (stJNIEnv) != NULL)
    {
	EdGUI_Message1 (NULL, 0, IDS_DEBUGGER_FAILURE_TITLE,
			IDS_EXCEPTIONOCCUREDINRESUMEMETHOD);
	stCantStartDebugger = TRUE;
    }
} // EdJdb_Resume


/************************************************************************/
/* EdJdb_StartTracing							*/
/************************************************************************/
void	EdJdb_StartTracing (void)
{
    if (!MyAttachIfNecessary ()) return;

    MyStartTracing ();
} // EdJdb_StartTracing


/************************************************************************/
/* EdJdb_StopTracing							*/
/************************************************************************/
void	EdJdb_StopTracing (void)
{
    if (stDebuggerJVM && stAttachedToCurrentVM)
    {
	MyStopTracing ();
    }
} // EdJdb_StartTracing


/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/
/************************************************************************/
/* MyAttachIfNecessary							*/
/************************************************************************/
static BOOL	MyAttachIfNecessary (void)
{
    if (stCantStartDebugger)
    {
	return FALSE;
    }

    if (stJNIEnv == NULL)
    {
	if (!MyStartJavaVM ())
	{
	    stCantStartDebugger = TRUE;
	    return FALSE;
	}
    }

    if (!stAttachedToCurrentVM)
    {
	if (!MyAttachToCurrentVM ())
	{
	    stCantStartDebugger = TRUE;
	    return FALSE;
	}

	stAttachedToCurrentVM = TRUE;
    }

    return TRUE;
} // MyAttachIfNecessary


/************************************************************************/
/* MyAttachToCurrentVM							*/
/************************************************************************/
static BOOL	MyAttachToCurrentVM (void)
{
    jint	myResult;	    
    
    myResult = (*stJNIEnv) -> CallStaticIntMethod (stJNIEnv, stDebuggerClass, 
						   stAttachMethod, NULL);
    if ((*stJNIEnv) -> ExceptionOccurred (stJNIEnv) != NULL)
    {
	EdGUI_Message1 (NULL, 0, IDS_CANT_ATTACH_TITLE,
			IDS_EXCEPTIONOCCUREDINATTACHMETHOD);
	return FALSE;
    }

    if (myResult != 1)
    {
	EdGUI_Message1 (NULL, 0, IDS_CANT_ATTACH_TITLE,
			IDS_UNABLETOATTACH, myResult);
	return FALSE;
    }

    return TRUE;
} // MyAttachToCurrentVM


/************************************************************************/
/* MyFindJNIAddress							*/
/*									*/
/* Only to be used with MyStartJVMThread				*/
/************************************************************************/
static JNI_CreateJavaVMCall	MyFindJNIAddress (char *pmDLLPath, 
						  char *pmProcName)
{
    HINSTANCE			myLibrary;
    JNI_CreateJavaVMCall	myCreateJavaVM;
    
    myLibrary = LoadLibrary (pmDLLPath);
    if (myLibrary == NULL)
    {
	int	myErrorCode = GetLastError ();
	char	*myErrorMessage;
	
	FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM | 
	    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
    	    NULL, myErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
    	    (LPTSTR) &myErrorMessage, 0, NULL);
	stStartJVMError = CANT_LOAD_JVM;
	strcpy (stErrorString1, pmDLLPath);
	strcpy (stErrorString2, myErrorMessage);
	stErrorNumber = myErrorCode;
    	LocalFree (myErrorMessage);
	return NULL;
    }

    myCreateJavaVM = (JNI_CreateJavaVMCall) GetProcAddress (myLibrary, 
    							    pmProcName);
    if (myCreateJavaVM == NULL)
    {
	stStartJVMError = CANT_FIND_PROC;
	strcpy (stErrorString1, pmProcName);
	strcpy (stErrorString2, pmDLLPath);
	return NULL;
    }

    return myCreateJavaVM;
} // MyFindJNIAddress


/************************************************************************/
/* MyNotifyDebuggerDisconnect						*/
/************************************************************************/
static void	MyNotifyDebuggerDisconnect (void)
{
    stAttachedToCurrentVM = FALSE;
//    EdGUI_Message ("Disconnected", "We're disconnected!");
} // MyNotifyDebuggerDisconnect


/************************************************************************/
/* MyStartJavaVM							*/
/************************************************************************/
static BOOL	MyStartJavaVM (void)
{
    WaitForSingleObject (stNotifyDebuggerJVMStarted, INFINITE);

    if (stStartJVMError == 0)
    {
    	// We need to get a new copy of the JNI environment because the JNI
    	// environment that you get from creating the JVM is only good for 
    	// the thread that created it!  Therefor we get a new environment
    	// for this thread.
    	JavaVMAttachArgs	myArgs;
    	int			myVal;
    	
    	myArgs.version = JNI_VERSION_1_2;
    	myArgs.name = NULL;
    	myArgs.group = NULL;
    	
	myVal = (*stDebuggerJVM) -> AttachCurrentThread (stDebuggerJVM, 
							 (void **) &stJNIEnv, 
				    			 &myArgs);
	if (myVal != 0)
	{
	    EdGUI_Message1 (NULL, 0, IDS_CANT_START_JAVAVM_TITLE,
			    IDS_CANT_ATTACH_THREAD, myVal);
	    return FALSE;
	}
    	return TRUE;
    }
    
    
    switch (stStartJVMError)
    {
        case CANT_LOAD_JVM:
	    EdGUI_Message1 (NULL, 0, IDS_CANT_START_JAVAVM_TITLE,
			    IDS_CANT_LOAD_JAVAVM, stErrorString1, 
			    stErrorString2, stErrorNumber);
	    break;			    
        case CANT_FIND_PROC:
	    EdGUI_Message1 (NULL, 0, IDS_CANT_START_JAVAVM_TITLE,
			    IDS_UNABLE_TO_FIND_PROC, stErrorString1, 
			    stErrorString2);
	    break;			    
        case JVM_CREATION_FAILED:
	    EdGUI_Message1 (NULL, 0, IDS_CANT_START_JAVAVM_TITLE,
			    IDS_JVM_CREATION_FAILED, stErrorNumber);
	    break;			    
        case CANT_FIND_DEBUGGER_CLASS:
	    EdGUI_Message1 (NULL, 0, IDS_CANT_ATTACH_TITLE,
			    IDS_CANT_FIND_DEBUGGER_CLASS, 
			    READY_DEBUG_CLASS_NAME);
	    break;			    
        case CANT_FIND_DEBUGGER_METHOD:
	    EdGUI_Message1 (NULL, 0, IDS_CANT_ATTACH_TITLE,
			    IDS_CANT_FIND_DEBUGGER_METHOD, 
			    stErrorString1);
	    break;			    
        case CANT_LOAD_SUN_DEBUG_CLASS:
	    EdGUI_Message1 (NULL, 0, IDS_CANT_ATTACH_TITLE,
			    IDS_CANT_LOAD_SUN_DEBUG_CLASS);
	    break;
	default:
	    EdGUI_Message1 (NULL, 0, IDS_CANT_ATTACH_TITLE,
			    IDS_UNKNOWN_JVM_START_ERROR, stStartJVMError);
	    break;
    } // switch
    
    return FALSE;
} // MyStartJavaVM


/************************************************************************/
/* MyStartJavaVMThread							*/
/************************************************************************/
static int	MyStartJavaVMThread (LPVOID pmThreadData)
{
    FilePath			myPathName, myClassPath, myLibraryPath;
    jint			myResult;
    JNI_CreateJavaVMCall	myJNI_CreateJavaVM;
    JavaVMInitArgs		myVM12Arguments;
    JavaVMOption		my12Options [10];
    FilePath			myJavaDLLPath;
    JNIEnv			*myJNIEnv;
    jmethodID			myLoadMethod;
    
    EdRun_GetJavaDLLPath (myJavaDLLPath);

    //
    // Create the class path. 
    //
    // [Ready]\support\Ready Classes\tools.jar
    EdFile_ConstructPath (myClassPath, IDS_READY_CLASSES_DIR, 
			  IDS_JAVA_TOOLS_JAR);
    strcat (myClassPath, ";");
    // [Ready]\support\Ready Classes
    EdFile_ConstructPath (myPathName, IDS_READY_CLASSES_DIR, DIR_ONLY_NO_SLASH);
    strcat (myClassPath, myPathName);

    // Create the options used when starting up the debugger JVM
    my12Options [0].optionString = malloc (strlen (CLASSPATH) + 
    					   strlen (myClassPath) + 1);
    strcpy (my12Options [0].optionString, CLASSPATH);
    strcat (my12Options [0].optionString, myClassPath);

    //
    // Create the library path. 
    //
    // [Ready]\support\Classes\tools.jar
    EdFile_ConstructPath (myLibraryPath, IDS_READY_BIN, DIR_ONLY_NO_SLASH);

    // Create the options used when starting up the debugger JVM
    my12Options [1].optionString = malloc (strlen (LIBRARY_PATH) + 
    					   strlen (myLibraryPath) + 1);
    strcpy (my12Options [1].optionString, LIBRARY_PATH);
    strcat (my12Options [1].optionString, myLibraryPath);
    
    myVM12Arguments.version = 0x00010002;
    myVM12Arguments.options = my12Options;
    myVM12Arguments.nOptions = 2;
    myVM12Arguments.ignoreUnrecognized = JNI_TRUE;
	
    // Get the address of JNI_CreateJavaVM
    myJNI_CreateJavaVM = MyFindJNIAddress (myJavaDLLPath, "JNI_CreateJavaVM");
    if (myJNI_CreateJavaVM == NULL)
    {
	// Errors already set in MyFindLibraryAddress
	SetEvent (stNotifyDebuggerJVMStarted);
	return 0;
    }
	    						 
    // Create the Java VM
    myResult = (myJNI_CreateJavaVM) (&stDebuggerJVM, (void **) &myJNIEnv, 
				     &myVM12Arguments);
    
    if (myResult < 0)
    {
	stStartJVMError = JVM_CREATION_FAILED;
	stErrorNumber = myResult;
	SetEvent (stNotifyDebuggerJVMStarted);
	return 0;
    }

    //
    // Find the debugger class
    //
    stDebuggerClass = (*myJNIEnv) -> FindClass (myJNIEnv, 
						READY_DEBUG_CLASS_NAME);
    if (stDebuggerClass == NULL)
    {
	stStartJVMError = CANT_FIND_DEBUGGER_CLASS;
	SetEvent (stNotifyDebuggerJVMStarted);
	return 0;
    }
    
    //
    // Find the methods in the class
    //
    myLoadMethod = (*myJNIEnv) -> GetStaticMethodID (myJNIEnv, 
					stDebuggerClass, 
					READY_LOAD_METHOD_NAME, "()V");
    if (myLoadMethod == NULL)
    {
	stStartJVMError = CANT_FIND_DEBUGGER_METHOD;
	strcpy (stErrorString1, READY_LOAD_METHOD_NAME);
	SetEvent (stNotifyDebuggerJVMStarted);
	return 0;
    }

    stAttachMethod = (*myJNIEnv) -> GetStaticMethodID (myJNIEnv, 
					stDebuggerClass, 
					READY_ATTACH_METHOD_NAME, "()I");
    if (stAttachMethod == NULL)
    {
	stStartJVMError = CANT_FIND_DEBUGGER_METHOD;
	strcpy (stErrorString1, READY_ATTACH_METHOD_NAME);
	SetEvent (stNotifyDebuggerJVMStarted);
	return 0;
    }

    stDetachMethod = (*myJNIEnv) -> GetStaticMethodID (myJNIEnv, 
					stDebuggerClass, 
					READY_DETACH_METHOD_NAME, "()V");
    if (stDetachMethod == NULL)
    {
	stStartJVMError = CANT_FIND_DEBUGGER_METHOD;
	strcpy (stErrorString1, READY_DETACH_METHOD_NAME);
	SetEvent (stNotifyDebuggerJVMStarted);
	return 0;
    }
    
    stPauseMethod = (*myJNIEnv) -> GetStaticMethodID (myJNIEnv, 
					stDebuggerClass, 
					READY_PAUSE_METHOD_NAME, "()V");
    if (stPauseMethod == NULL)
    {
	stStartJVMError = CANT_FIND_DEBUGGER_METHOD;
	strcpy (stErrorString1, READY_PAUSE_METHOD_NAME);
	SetEvent (stNotifyDebuggerJVMStarted);
	return 0;
    }
    
    stResumeMethod = (*myJNIEnv) -> GetStaticMethodID (myJNIEnv, 
					stDebuggerClass, 
					READY_RESUME_METHOD_NAME, "()V");
    if (stResumeMethod == NULL)
    {
	stStartJVMError = CANT_FIND_DEBUGGER_METHOD;
	strcpy (stErrorString1, READY_RESUME_METHOD_NAME);
	SetEvent (stNotifyDebuggerJVMStarted);
	return 0;
    }

    //
    // Load the ReadyDebug class
    //    
    (*myJNIEnv) -> CallStaticVoidMethod (myJNIEnv, stDebuggerClass, 
					 myLoadMethod, NULL);
    if ((*myJNIEnv) -> ExceptionOccurred (myJNIEnv) != NULL)
    {
	stStartJVMError = CANT_LOAD_SUN_DEBUG_CLASS;
	SetEvent (stNotifyDebuggerJVMStarted);
	return 0;
    }

    SetEvent (stNotifyDebuggerJVMStarted);
    return 0;
} // MyStartJavaVM


/************************************************************************/
/* MyStartTracing							*/
/************************************************************************/
static void	MyStartTracing (void)
{
} // MyStartTracing


/************************************************************************/
/* MyStopTracing							*/
/************************************************************************/
static void	MyStopTracing (void)
{
} // MyStopTracing
