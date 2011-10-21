#include <windows.h>

#include <stdio.h>

#include "jssignals.h"

#define READY_DIR			"e:\\Projects\\Ready\\"
//#define READY_DIR			"c:\\hsa\\Ready\\"

//#define CLASS_NAME			"ConsoleText"
#define CLASS_NAME			"SlowOutput"
//#define CLASS_NAME			"Numbers"

#define USING_DOS
//#define USING_DOS			"-dos"

//#define REDIRECT_INPUT			"-ii t.in"
#define REDIRECT_INPUT

#define REDIRECT_OUTPUT			"-oo t.out"
//#define REDIRECT_OUTPUT


//
// The rest of these don't need modification
//

#define SUPPORT_DIR			READY_DIR "Support\\"

#define JAVASUP_APPLICATION_NAME	"javasup.exe"
#define COMMAND_LINE_ARGS 		"-java \"" SUPPORT_DIR \
	"JVM\\bin\\javai.dll\" -cp \".;" SUPPORT_DIR \
	"JVM\\classes;" SUPPORT_DIR "JVM\\lib\\rt.jar;" SUPPORT_DIR \
	"JVM\\lib\\math.jar;" SUPPORT_DIR "Classes\" -class " CLASS_NAME \
	" -dir \"" READY_DIR "test\" " USING_DOS " " \
	REDIRECT_INPUT " " REDIRECT_OUTPUT " " \
	"-props F 25 80 10 \"Courier New\" 2 F F 8 8 T"

static HANDLE	stToJavaSupWriteDup, stFromJavaSupReadDup;

static int	MyGetJavaSupSignals (LPVOID pmConsoleWindow);

main (int argc, char *argv[])
{
    SECURITY_ATTRIBUTES	myAttribs;
    STARTUPINFO		myStartupInfo;
    PROCESS_INFORMATION	myProcessInfo;
    HANDLE		myToJavaSupRead, myToJavaSupWrite;
    HANDLE		myFromJavaSupRead, myFromJavaSupWrite;
    HANDLE		myMainSaysPause, myMainSaysResume, myMainSaysTerminate;
    HANDLE		myThread;
    DWORD		myThreadID;
    char		myArgs [256];
    int			myResult;
    int			myBytesWritten;
    
    //
    // Create the events
    //
    myMainSaysPause = CreateEvent (NULL, TRUE, FALSE, READY_MAIN_SAYS_PAUSE);
    myMainSaysResume = CreateEvent (NULL, TRUE, FALSE, READY_MAIN_SAYS_RESUME);
    myMainSaysTerminate = CreateEvent (NULL, TRUE, FALSE, 
    				       READY_MAIN_SAYS_TERMINATE);

    //
    // Set up attributes for inheritance
    //
    ZeroMemory (&myAttribs, sizeof (myAttribs));
    myAttribs.nLength = sizeof (myAttribs);
    myAttribs.bInheritHandle = TRUE;

    // 
    // Initialize process startup info
    //
    ZeroMemory (&myStartupInfo, sizeof (myStartupInfo));
    myStartupInfo.cb = sizeof (myStartupInfo);

    //
    // Create "To JavaSup" Pipe
    //
    if (!CreatePipe (&myToJavaSupRead, &myToJavaSupWrite, &myAttribs, 0))
    {
    	// TW Problem
    	return TRUE;
    }
    
    if (!DuplicateHandle (GetCurrentProcess (), myToJavaSupWrite, 
    			  GetCurrentProcess (), &stToJavaSupWriteDup,
    			  0, FALSE, DUPLICATE_SAME_ACCESS))
    {
    	// TW Problem
    	return TRUE;
    }

    // This leaves us with myToJavaSupRead (inheritable) and
    // myToJavaSupWriteDup (non-inheritable)
    CloseHandle (myToJavaSupWrite);
    
    //
    // Create "From JavaSup" Pipe
    //
    if (!CreatePipe (&myFromJavaSupRead, &myFromJavaSupWrite, &myAttribs, 0))
    {
    	// TW Problem
    	return TRUE;
    }
    
    if (!DuplicateHandle (GetCurrentProcess (), myFromJavaSupRead, 
    			  GetCurrentProcess (), &stFromJavaSupReadDup,
    			  0, FALSE, DUPLICATE_SAME_ACCESS))
    {
    	// TW Problem
    	return TRUE;
    }

    // This leaves us with myFromJavaSupRead (non-inheritable) and
    // myFromJavaSupWriteDup (inheritable)
    CloseHandle (myFromJavaSupRead);

    //
    // Create thread to get messages back from JavaSup
    //
    myThread = CreateThread (NULL, 0, 
        (LPTHREAD_START_ROUTINE) MyGetJavaSupSignals, NULL, 0, &myThreadID);
    if (myThread == 0)
    {
	printf ("Unable to create thread (%d)\n", GetLastError ());
	return 0;
    }    		       

    //
    // Create the Process
    //
    wsprintf (myArgs, "%s -pipe %d %d", JAVASUP_APPLICATION_NAME, 
    	myToJavaSupRead, myFromJavaSupWrite);
    myResult = CreateProcess (JAVASUP_APPLICATION_NAME,
    	    myArgs, NULL, NULL, TRUE, DETACHED_PROCESS, 
    	    NULL, NULL, &myStartupInfo, &myProcessInfo);

    if (!myResult)
    {
    	printf ("Error with CreateProcess (%d)\n", GetLastError ());
    	return 0;
    }
    
    //
    // Send the command line arguments to JavaSup
    //
    if ((!WriteFile (stToJavaSupWriteDup, COMMAND_LINE_ARGS, 
    	             strlen (COMMAND_LINE_ARGS), &myBytesWritten, NULL)) || 
    	(myBytesWritten != strlen (COMMAND_LINE_ARGS)))
    {
        printf ("Error writing command line arguments\n");
        return 0;
    }	

    if ((!WriteFile (stToJavaSupWriteDup, "\0", 1, &myBytesWritten, NULL)) || 
    	(myBytesWritten != 1))
    {
        printf ("Error writing command line arguments terminator\n");
        return 0;
    }
    
    //
    // Main loop
    //
    // Commands: P = Pause, R = Resume, T = Terminate
    //
    SetConsoleTitle ("Running");

    while (TRUE)
    {
    	char	myBuffer [256];
    	
    	printf ("Enter command (P)ause, (R)esume, (T)erminate, (Q)uit: ");
   	gets (myBuffer);
	
	if (stricmp (myBuffer, "p") == 0)
	{
	    SetEvent (myMainSaysPause);
	}
	else if (stricmp (myBuffer, "r") == 0)
	{
	    SetEvent (myMainSaysResume);
	}
	else if (stricmp (myBuffer, "t") == 0)
	{
	    SetEvent (myMainSaysTerminate);
	}
	else if (stricmp (myBuffer, "q") == 0)
	{
	    SetEvent (myMainSaysTerminate);
	    ExitProcess (0);
	}
	else
	{
	    printf ("Unknown order!\n");
	}
    } // while (TRUE)
} // main


static int	MyGetJavaSupSignals (LPVOID pmConsoleWindow)
{
    HANDLE 	myProgramPaused, myProgramResumed, myProgramFinished;
    HANDLE	myProgramHalted, myTerminateProgram, myGetPathName;
    HANDLE	mySentOutput, myPrintOutput, myHandles [7];
    int		myResult, myBytesWritten, myBytesRead;
    char	myBuffer [256];
        	
    myProgramPaused = CreateEvent (NULL, TRUE, FALSE, 
    				   READY_NOTIFY_JAVA_PROGRAM_PAUSED);
    myProgramResumed = CreateEvent (NULL, TRUE, FALSE, 
    				    READY_NOTIFY_JAVA_PROGRAM_RESUMED);
    myProgramFinished = CreateEvent (NULL, TRUE, FALSE, 
    				     READY_NOTIFY_JAVA_PROGRAM_FINISHED);
    myProgramHalted = CreateEvent (NULL, TRUE, FALSE, 
    				   READY_NOTIFY_JAVA_PROGRAM_HALTED);
    myTerminateProgram = CreateEvent (NULL, TRUE, FALSE, 
    				      READY_NOTIFY_TERMINATE);
    mySentOutput = CreateEvent (NULL, TRUE, FALSE, 
    				READY_NOTIFY_SEND_OUTPUT);
    myPrintOutput = CreateEvent (NULL, TRUE, FALSE, 
    				 READY_NOTIFY_PRINT_OUTPUT);

    myHandles [0] = myProgramPaused;
    myHandles [1] = myProgramResumed;
    myHandles [2] = myProgramFinished;
    myHandles [3] = myProgramHalted;
    myHandles [4] = myTerminateProgram;
    myHandles [5] = mySentOutput;
    myHandles [6] = myPrintOutput;

    while (TRUE)
    {
    	myResult = WaitForMultipleObjects (8, myHandles, FALSE, INFINITE);
    	
    	if (myResult == (WAIT_OBJECT_0 + 0))
    	{
	    ResetEvent (myProgramPaused);
	    SetConsoleTitle ("Paused");
	}
    	else if (myResult == (WAIT_OBJECT_0 + 1))
    	{
	    ResetEvent (myProgramResumed);
	    SetConsoleTitle ("Running");
	}
    	else if (myResult == (WAIT_OBJECT_0 + 2))
    	{
	    ResetEvent (myProgramFinished);
	    SetConsoleTitle ("Finished");
	}
    	else if (myResult == (WAIT_OBJECT_0 + 3))
    	{
	    ResetEvent (myProgramHalted);
	    SetConsoleTitle ("Halted");
	}
    	else if (myResult == (WAIT_OBJECT_0 + 4))
    	{
	    ResetEvent (myTerminateProgram);
    	    ExitProcess (0);
	}
    	else if (myResult == (WAIT_OBJECT_0 + 5))
    	{
	    ResetEvent (mySentOutput);

    	    printf ("Output of program\n");
    	    printf ("-----------------\n");

	    do
	    {
	    	if (!ReadFile (stFromJavaSupReadDup, myBuffer, 255, 
	    		       &myBytesRead, NULL))
 	    	{
 	            printf ("Error reading program input\n");
	            ExitProcess (0);
 	    	}
 	    	
 	    	printf ("%s", myBuffer);
 	    } while (myBuffer [myBytesRead - 1] != 0);
	}
    	else if (myResult == (WAIT_OBJECT_0 + 6))
	{
	    ResetEvent (myPrintOutput);

	    printf ("To be sent to printer\n");
	    printf ("---------------------\n");

	    do
	    {
	    	int	myBytesRead;
	    	
	    	if (!ReadFile (stFromJavaSupReadDup, myBuffer, 255, 
	    		       &myBytesRead, NULL))
 	    	{
 	            printf ("Error reading program input\n");
	            ExitProcess (0);
 	    	}
 	    	
 	    	printf ("%s", myBuffer);
 	    } while (myBuffer [myBytesRead - 1] != 0);
	}
    }
} // MyGetJavaSupSignals

