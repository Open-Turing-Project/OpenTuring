/************************************************************************/
/*				ReadyCon				*/
/*									*/
/* This program is used to preserve the visibility of the console	*/
/* window when java is run in it.  Specifically, it allows the console	*/
/* window _not_ to disappear when the Java process terminates.		*/
/*									*/
/* When run, it's passed a command line with several arguments.		*/
/*	1) The command to start the java process.			*/
/*	2) The command line to execute to start the java process.	*/
/*	3) The class being run						*/
/*	4) The argument is 0 or 1 to indicate whether the java process 	*/
/* 	   should be started with the window hidden (=1).		*/
/*	5) An event handle that ReadyCon signals when it detects that 	*/
/*	   the java process has terminated.				*/
/*	6) An event handle is signalled when the the main program	*/
/*	   wishes to stop the java process.			 	*/
/************************************************************************/
#include <windows.h>

#define JAVA_FINISHED		0
#define KILL_JAVA		1
#define KILL_JAVA_SELECTED	(WAIT_OBJECT_0 + 1)

static BOOL WINAPI	MyConsoleHandler (UINT pmConsoleEvent);

static HWND	stConsoleWindow;
static HANDLE	stJVMProcessHandle;
static HANDLE	stSignalJavaHalted;	
static UINT	stJVMProcessID;
static BOOL	stHalted;

int main (int argc, char *argv[])
{
    char		myConsoleName [256];
    RECT		myConsoleRect, myScreenRect;
    PROCESS_INFORMATION	myProcessInfo;
    STARTUPINFO		myStartupInfo;
    int			myCreateProcessFlags;
    char		myErrorLine [256];
    int			myResult;
    HANDLE		mySignalJavaFinished;	
    HANDLE		stCloseDOSWindowEventHandle;
    HANDLE		myHandles [2];
    char		*myPtr;
    	    
    // Set the title
    wsprintf (myConsoleName, 
        "%s - Running - Standard Input/Output - Ready to Program", argv [3]);
    SetConsoleTitle (myConsoleName);
	
    // Wait
    Sleep (40);
	
    // Get the console window with that title
    stConsoleWindow = FindWindow (NULL, myConsoleName);
	
    // Move the window to the upper right corner
    GetWindowRect (stConsoleWindow, &myConsoleRect);
    SystemParametersInfo (SPI_GETWORKAREA, 0, &myScreenRect, 0);
    SetWindowPos (stConsoleWindow, HWND_TOP, myScreenRect.right - 
        (myConsoleRect.right - myConsoleRect.left), myScreenRect.top,
        0, 0, SWP_NOSIZE);

    if (argc != 9)
    {
	wsprintf (myErrorLine, 
	    "Wrong number of arguments to ReadyCon (%d instead of 8)", 
	    argc - 1);
    	MessageBox (stConsoleWindow, myErrorLine, "Bad Call to ReadyCon", 0);
    	return 0;
    }
    
    // Convert command line single quotes to double quotes
    myPtr = argv [2];
    while (*myPtr != 0)
    {
        if (*myPtr == '\'')
        {
            *myPtr = '"';
        }
        myPtr++;
    }

    SetConsoleCtrlHandler ((PHANDLER_ROUTINE) MyConsoleHandler, TRUE);
    
    //
    // Create the java.exe process     
    //
    ZeroMemory (&myStartupInfo, sizeof (myStartupInfo));
    myStartupInfo.cb = sizeof (myStartupInfo);
    myStartupInfo.hStdInput = GetStdHandle (STD_INPUT_HANDLE);
    myStartupInfo.hStdOutput = GetStdHandle (STD_OUTPUT_HANDLE);
    myStartupInfo.hStdError = GetStdHandle (STD_ERROR_HANDLE);
    myStartupInfo.dwFlags = STARTF_USESTDHANDLES;

    myCreateProcessFlags = CREATE_NEW_PROCESS_GROUP;
    if (strcmp (argv [4], "1") == 0)
    {
    	myCreateProcessFlags |= CREATE_NO_WINDOW;
    }
        
    stHalted = FALSE;
            
    myResult = CreateProcess (argv [1], argv [2],
    	NULL, NULL, TRUE, myCreateProcessFlags, 
    	NULL, NULL, &myStartupInfo, &myProcessInfo);

    CloseHandle (GetStdHandle (STD_ERROR_HANDLE));
    
    if (!myResult)
    {
	wsprintf (myErrorLine, 
	    "Unable to start java with command:\n\n      %s\n\nError = %d", 
	    argv [2], GetLastError ());
    	MessageBox (stConsoleWindow, myErrorLine, "Bad Call to ReadyCon", 0);
    	return 0;
    }
    
    stJVMProcessHandle = myProcessInfo.hProcess;
    stJVMProcessID = myProcessInfo.dwProcessId;
    
    // Now wait for either the java process to finish, in which case
    // signal the java process finished event, or for the kill java 
    // process event to be signaled, in which case close the process 
    // down and signal the java process finished event.
    mySignalJavaFinished = (HANDLE) atoi (argv [5]);
    stSignalJavaHalted = (HANDLE) atoi (argv [6]);
    myHandles [JAVA_FINISHED] = stJVMProcessHandle;
    myHandles [KILL_JAVA] = (HANDLE) atoi (argv [7]);
    stCloseDOSWindowEventHandle = (HANDLE) atoi (argv [8]);
    
    // Wait until either the Java process ends, or we receive a signal
    // to kill it.
    myResult = WaitForMultipleObjects (2, myHandles, FALSE, INFINITE);
    
    if (myResult == KILL_JAVA_SELECTED)
    {
    	int	myCharsWritten;
    	
    	GenerateConsoleCtrlEvent (CTRL_BREAK_EVENT, stJVMProcessID);
    	if (WaitForSingleObject (stJVMProcessHandle, 250) == WAIT_TIMEOUT)
    	{
    	    TerminateProcess (stJVMProcessHandle, 0);
    	}
    	
    	stHalted = TRUE;
    	
    	// Output ^C to remind user he's killed the program
	WriteConsole (GetStdHandle(STD_OUTPUT_HANDLE), "^C\n", 3,
	    &myCharsWritten, NULL);
    }

    stJVMProcessHandle = NULL;

    //
    // Set the console title    
    if (stHalted)
    {
    	wsprintf (myConsoleName, 
            "%s - Halted - Standard Input/Output - Ready to Program", argv [3]);
    }
    else
    {
    	wsprintf (myConsoleName, 
          "%s - Finished - Standard Input/Output - Ready to Program", argv [3]);
    }
    SetConsoleTitle (myConsoleName);
    
    SetEvent (mySignalJavaFinished);

    //    
    // Sleep until main program wants window closed or console event
    // occurs that closes the window.
    //
    WaitForSingleObject (stCloseDOSWindowEventHandle, INFINITE);
    
    return 0;
} // main


BOOL WINAPI	MyConsoleHandler (UINT pmConsoleEvent)
{
    // Handle CTRL_C_EVENT, CTRL_BREAK_EVENT, 
    // CTRL_CLOSE_EVENT, CTRL_LOGOFF_EVENT
    
    //
    // Terminate the Java process (if necessary)
    //
    if (stJVMProcessHandle != NULL)
    {
    	stHalted = TRUE;

    	SetEvent (stSignalJavaHalted);
    	
    	GenerateConsoleCtrlEvent (CTRL_BREAK_EVENT, stJVMProcessID);
    	if (WaitForSingleObject (stJVMProcessHandle, 250) == WAIT_TIMEOUT)
    	{
    	    TerminateProcess (stJVMProcessHandle, 0);
    	}
    }
    
    // If it's a close event or a log off event, terminate this process
    if ((pmConsoleEvent == CTRL_CLOSE_EVENT) || 
        (pmConsoleEvent == CTRL_LOGOFF_EVENT)) 
    {
    	ExitProcess (0);
    }
    else
    {
    	int	myCharsWritten;
    	
    	// Output ^C to remind user he's killed the program
	WriteConsole (GetStdHandle(STD_OUTPUT_HANDLE), "^C\n", 3,
	    &myCharsWritten, NULL);
    }
    
    return TRUE;
}