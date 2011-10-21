/***********/
/* jscon.c */
/***********/

/*******************/
/* System includes */
/*******************/
#include <windows.h>
#include <windowsx.h>
#include <crtdbg.h>

/****************/
/* Self include */
/****************/
#include "jscon.h"

/******************/
/* Other includes */
/******************/
#include "jsmain.h"
#include "jstop.h"

#include "edfile.h"
#include "edglob.h"
#include "edgui.h"
#include "edlog.h"
#include "edtext.h"

#include "jscommands.h"
#include "jsdialogs.h"

//#include "ed.h"
//#include "edfail.h"
//#include "edglob.h"
//#include "edgui.h"
//#include "edprop.h"

/**********/
/* Macros */
/**********/
#define EdFail_Warn		JS_ErrorMessage
#define EdFail_Fatal		JS_ErrorMessage

/*************/
/* Constants */
/*************/
// Input buffer size
#define INPUT_BUFFER_SIZE	1024
#define INC(x)			(((x) + 1) % INPUT_BUFFER_SIZE)
#define INC2(x)			(((x) + 2) % INPUT_BUFFER_SIZE)
#define INC3(x)			(((x) + 3) % INPUT_BUFFER_SIZE)
#define DEC(x)			(((x) + INPUT_BUFFER_SIZE - 1) % INPUT_BUFFER_SIZE)

#define BACKSPACE		 8
#define TAB			 9
#define NEWLINE			10
#define RETURN			13
#define CTRL_U			21
#define CTRL_Z			26

// Minimum size of the window in rows and columns
#define MIN_ROWS		10
#define MIN_COLS		30

// The number of text display windows allowed in single window mode
#define MAX_WINDOWS		30

// Argument for MyDisplayText
#define ALL_LINES		-99

// Used for scrolling with the mouse
#define SCROLL_TIMER		1
#define HORIZ_TICKS_PER_SCROLL	4

// Used for the 1 second delay before halting when a run-time error occurs
#define HALT_TIMER		2

// Used in MyDisplayText
#define NO_SEL_ON_THIS_LINE	    -1
#define TO_END_OF_LINE	 	999999
#define IN_SELECTION		     1
#define IN_HIGHLIGHT		     2
#define IN_NORMAL		     3

// MyHandleKeystroke parameters
#define NORMAL_KEY		FALSE
#define DELETE_KEY		TRUE

// Scrolling instructions for MyScrollToSelection
#define SCROLL_NORMAL		104

// The whitespace along the left and top edges
#define MARGIN			 2

// Used by WM_ADD_TEXT
#define NOT_AT_BEGINNING_OF_LINE	1
#define BEGINNING_OF_LINE		2
#define FREE_BUFFER			3

// Some colours                        
#define COLOUR_WHITE		RGB (255, 255, 255)
#define COLOUR_LIGHTGREY	RGB (192, 192, 192)
#define COLOUR_DARKGREY		RGB (40, 40, 40)
#define COLOUR_BLACK		RGB (0, 0, 0)
#define COLOUR_RED		RGB (160, 0, 0)
#define COLOUR_GREEN		RGB (0, 160, 0)
#define COLOUR_BLUE		RGB (0, 0, 160)
#define COLOUR_DEEPBLUE		RGB (0, 0, 132)

// The colour the selection will appear in
#define SELECTION_COLOUR	COLOUR_DEEPBLUE

// Used for by the mouse EdGUI_GetMouseWheelSettings
#ifndef WM_MOUSEWHEEL
// Message ID for IntelliMouse wheel
#define WM_MOUSEWHEEL WM_MOUSELAST+1 
#endif // !WM_MOUSEWHEEL

#define NEW_LINE	"\r\n"
    
/********************/
/* Global variables */
/********************/

/*********/
/* Types */
/*********/
typedef struct
{
    // The elements in the window
    HWND	buttonBarWindow;
    HWND	consoleTextWindow;
    RECT	oldWindowRect;

    //
    // Console Text Information
    //
    TextPtr	text;
    RECT	textRect;
    int		topLine; 	// Top visible line (0-based)
    int		leftCol;	// The left most visible char (0-based)
    int		oldTopLine;	// Top line when text last drawn
    int		oldLeftCol;	// Left col when text last drawn
    int		windowWidth;	// Window width in characters
    int		windowHeight;	// Window height in characters
    BOOL	caretCreated;
    BOOL	caretShowing;
    
    int		accumulateDelta; // For the wheel mouse
    
    // Used for mouse selection
    BOOL	selectingWithMouse;
    int		horizontalTicks, verticalTicks;
    int		numClicks, lastClickTime;
    int		lastMouseLine, lastMouseCol;
    BOOL	clickOnSameChar;
    
    char	*className;
    int		runStatus;
    
    BOOL	windowClosing;
        
    char	kbdBuffer [INPUT_BUFFER_SIZE];
    int		kbdBufferHead, kbdBufferTail;
    
    char	lineBuffer [INPUT_BUFFER_SIZE];
    int		lineBufferSize;
    BOOL	lineBufferNewLineWaiting;
    
    // These two variables indicate where the start of the last input occurred
    // and the end of the last output.  These are used so that if it is
    // necessary to delete all the input a user has typed, you can either
    // delete from the start of the last input, or, if there has been program
    // output since the start of the last input (which can occur if there's 
    // threads), then only delete to the text of the last output.  That way,
    // you never actually have any output deleted.
    int		textStartPosOfLastInput;
    int		textEndPosOfLastOutput;

    // Is the program accepting input at the moment
    BOOL	acceptingInput;

    // The user has already entered Ctrl+D or Ctrl+Z, accept no more input
    BOOL	eofAlreadyEntered;

    // When there is no more line input send an empty string
    BOOL	eofOnDeck;

    // When there is no more line input send an empty string
    BOOL	eofEchoed;

    // Is input being redirected from a file
    BOOL	inputFromFile;
} ConsoleWindowInfo, *ConsoleWindowInfoPtr;

typedef struct
{
    const char 	*className;
} WindowCreationInfo, *WindowCreationInfoPtr;

/**********************/
/* External variables */
/**********************/

/********************/
/* Static constants */
/********************/

/********************/
/* Static variables */
/********************/
// Properties of the text display window
static JSConOnly_Properties	stProperties;
static BOOL			stPropertiesChanged;

// 
static HFONT			stTabFont;

// Menu for single window mode with no open windows
static HMENU			stSingleWindowInitMenu;

// Colours and associated variables used to display the text
static COLORREF			stSelectionColour;

//
static HBRUSH			stBlackBrush;

// Used to display the text
static HFONT			stNormalFont;
static int			stTextFontCharWidth, stTextFontCharHeight;

// Used by the wheel mouse
static int			stWheelMouseDeltaPerLine;
static UINT			stMouseWheelMessage;

// Critical Section to stop more than one thread asking for input at a time
CRITICAL_SECTION		stGetLineCriticalSection;
CRITICAL_SECTION		stLineBufferCriticalSection;

static HANDLE			stNewLineReceivedEvent;


/******************************/
/* Static callback procedures */
/******************************/
static LRESULT CALLBACK MyConsoleTextWindowProcedure (HWND pmTextDisplayWindow, 
						      UINT pmMessage, 
    						      WPARAM pmWParam, 
    						      LPARAM pmLParam);
static LRESULT CALLBACK MyConsoleWindowProcedure (HWND pmWindow, UINT pmMessage, 
						 WPARAM pmWParam, 
						 LPARAM pmLParam);
static LRESULT CALLBACK	MyRaisedFrameWindowProcedure (HWND pmWindow, 
						      UINT pmMessage, 
						      WPARAM pmWParam, 
						      LPARAM pmLParam);

/*********************/
/* Static procedures */
/*********************/
static void		MyAddText (HWND pmConsoleWindow, char *pmText,
				   int pmFlag);
static void		MyCaretCreate (HWND pmConsoleTextWindow);
static void		MyCaretDisplay (HWND pmConsoleTextWindow);
static void		MyCaretHide (HWND pmConsoleTextWindow);
static void		MyCaretMove (HWND pmConsoleTextWindow);
static BOOL		MyCheckForClose (HWND pmConsoleWindow);
static BOOL		MyCommandHandler (HWND pmConsoleWindow, 
						 short command);
static HWND		MyConTextCreate (HWND pmConsoleWindow);
static void		MyCopy (HWND pmConsoleWindow);
static int		MyCreateConsoleTextWindow (HWND pmConsoleTextWindow);
static int		MyCreateConsoleWindow (HWND pmConsoleWindow, 
				    WindowCreationInfoPtr pmWindowCreationInfo);
static ConsoleWindowInfoPtr	MyCreateConsoleWindowInfo (void);
static void		MyDeleteText (HWND pmConsoleWindow, int pmPos);
static void		MyDisplayText (HWND pmConsoleTextWindow, 
				       BOOL pmPaintUpdate,
				       int pmFromLine, int pmToLine);
static void		MyFileSave (HWND pmWindow);
static HWND		MyGetConsoleTextWindow (HWND pmConsoleWindow);
static HWND		MyGetConsoleWindow (HWND pmWindow);
static ConsoleWindowInfoPtr	MyGetConsoleWindowInfo (HWND pmConsoleWindow);
static void		MyGetCurrentLineCol (TextPtr pmTextPtr, int *pmLine, 
					     int *pmCol);
static int		MyGetCurrentTextPos (TextPtr pmTextPtr);
static TextPtr		MyGetTextPtr (HWND pmConsoleTextWindow);
static void		MyGetWindowDisplacement (BOOL pmReset, 
						 long *pmWindowWidth, 
						 long *pmWindowHeight, 
						 long *pmLeft, 
				 		 long *pmTop);
static void		MyGetWindowSize (HWND pmConsoleWindow, int pmRows,
					 int pmCols, long *pmWidth,
					 long *pmHeight);
static void		MyKeystrokeHandler (HWND pmTextDisplayWindow, 
					    int pmKeystroke);
static BOOL		MyMouseDownHandler (HWND pmConsoleTextWindow, 
					    int pmX, int pmY, BOOL pmShift, 
		    			    BOOL pmAllowOutOfBounds);
static void		MyMouseExtendSelectionToLine (HWND pmConsoleTextWindow);
static void		MyMouseExtendSelectionToWord (HWND pmConsoleTextWindow);
static BOOL		MyMouseInTextRect (HWND pmConsoleTextWindow, 
					   int pmX, int pmY);
static void		MyPaste (HWND pmConsoleTextWindow);
static void	    	MyProcessFromKeyboardToLineBuffer (HWND pmWindow);
static void		MyResizeConsoleTextWindow (HWND pmConsoleTextWindow);
static void		MyResizeConsoleWindow (HWND pmConsoleWindow);
static BOOL		MySaveTextToFile (char *pmPathName, TextPtr pmText);
static void		MyScrollHandlerHorz (HWND pmConsoleTextWindow, 
					     int pmPart, int pmValue);
static void		MyScrollHandlerVert (HWND pmConsoleTextWindow, 
					     int pmPart, int pmValue);
static void		MyScrollToSelection (HWND pmConsoleTextWindow, 
					     int pmSkipSize, BOOL pmRedraw,
					     BOOL pmScrollToEnd);
static void		MySelectAll (HWND pmConsoleWindow);					     
static BOOL		MySetDisplayFonts (HWND pmConsoleTextWindow);
static void		MySetRunState (HWND pmConsoleWindow, int pmRunStatus);
static void		MySynchScrollBarsToText (HWND pmConsoleTextWindow);

/***********************/
/* External procedures */
/***********************/
/************************************************************************/
/* JSCon_Init								*/
/* 									*/
/* Initialize the modules under EdWin and register the new class	*/
/************************************************************************/
void	JSCon_Init ()
{
    char	myConsoleWindowClassName [256];
    char	myConsoleTextWindowClassName [256];
    char	myRaisedFrameWindowClassName [256];
    int		myResult;
    WNDCLASSEX	myWindowClass;

    //
    // Initialize each of the submodules that need initialization 
    // 
       
    JSTop_Init ();
    EdGUI_Init ();
    
    //
    // Initialize the JSCon module
    //

    // Create Brush
    stBlackBrush = CreateSolidBrush (COLOUR_BLACK);
    if (stBlackBrush == NULL)	  
    {
    	EdFail_Warn (IDS_CREATEBRUSHFAIL, __FILE__, __LINE__, GetLastError ());
    }
    
    // Initialize the wheel mouse
    stWheelMouseDeltaPerLine = EdGUI_GetMouseWheelSettings ();
    stMouseWheelMessage = EdGUI_GetMouseWheelMessage ();
    
    // Get the class name
    EdGUI_LoadString (IDS_CONSOLE_WINDOW_NAME, myConsoleWindowClassName, 
	sizeof (myConsoleWindowClassName));

    // Initialize the critical sections
    InitializeCriticalSection (&stGetLineCriticalSection);
    InitializeCriticalSection (&stLineBufferCriticalSection);

    // Initialize the event    
    stNewLineReceivedEvent = CreateEvent (NULL, FALSE, FALSE, NULL);
    
    //
    // Register the console window class
    //
    myWindowClass.cbSize = 	  sizeof (myWindowClass);
    // Set window class to redraw when window size changes
    myWindowClass.style = 	  CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    // Procedure to be called with messages for this window class
    myWindowClass.lpfnWndProc =   MyConsoleWindowProcedure;
    // The extra space in class struct
    myWindowClass.cbClsExtra = 	  0;
    // The extra space in window struct for the pointer to text data
    // and editor window
    myWindowClass.cbWndExtra = 	  WINDOW_EXTRA_MEMORY;
    // The application's handle
    myWindowClass.hInstance = 	  gProgram.applicationInstance;
    // Set the icon for this window class
    myWindowClass.hIcon = 	  LoadIcon (gProgram.applicationInstance, 
    					    MAKEINTRESOURCE (APP_ICON));
    // Set the cursor for this window class
    myWindowClass.hCursor = 	  LoadCursor (NULL, IDC_ARROW);
    // Set the background colour for this window
    myWindowClass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH);
    // Set the menu for this window class
    myWindowClass.lpszMenuName =  NULL;
    // Name of the window class
    myWindowClass.lpszClassName = myConsoleWindowClassName; 
    // Set the icon for this class.
    myWindowClass.hIconSm = 	  LoadIcon (gProgram.applicationInstance, 
    					    MAKEINTRESOURCE (RUN_SMALL_ICON));
    
    myResult = RegisterClassEx (&myWindowClass);
    if (myResult == 0)
    {
    	EdFail_Warn (IDS_REGISTERCLASSFAIL, __FILE__, __LINE__, 
    			  GetLastError ());
    }
    
    // Get the class name
    EdGUI_LoadString (IDS_CONSOLETEXT_WINDOW_NAME, myConsoleTextWindowClassName, 
	sizeof (myConsoleTextWindowClassName));
    
    //
    // Register the text display window class
    //
    myWindowClass.cbSize = 	  sizeof (myWindowClass);
    // Set window class to redraw when window size changes
    myWindowClass.style = 	  CS_HREDRAW | CS_VREDRAW;
    // Procedure to be called with messages for this window class
    myWindowClass.lpfnWndProc =   MyConsoleTextWindowProcedure;
    // The extra space in class struct
    myWindowClass.cbClsExtra = 	  0;
    // The extra space in window struct for the pointer to text data
    // and editor window
    myWindowClass.cbWndExtra = 	  WINDOW_EXTRA_MEMORY;
    // The application's handle
    myWindowClass.hInstance = 	  gProgram.applicationInstance;
    // Set the icon for this window class
    myWindowClass.hIcon = 	  NULL;
    // Set the cursor for this window class
    myWindowClass.hCursor = 	  LoadCursor (NULL, IDC_IBEAM);
    // Set the background colour for this window
    myWindowClass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH);
    // Set the menu for this window class
    myWindowClass.lpszMenuName =  NULL;
    // Name of the window class
    myWindowClass.lpszClassName = myConsoleTextWindowClassName; 
    // Set the icon for this class.
    myWindowClass.hIconSm = 	  NULL;
    
    myResult = RegisterClassEx (&myWindowClass);
    if (myResult == 0)
    {
    	EdFail_Warn (IDS_REGISTERCLASSFAIL, __FILE__, __LINE__, 
    		     GetLastError ());
    }
    
    // Get the class name
    EdGUI_LoadString (IDS_RAISED_FRAME_WINDOW_NAME, 
        myRaisedFrameWindowClassName, 
	sizeof (myRaisedFrameWindowClassName));
		     
    //
    // Register the raised window class
    //
    myWindowClass.cbSize =        sizeof (myWindowClass);
    // Set window class to redraw when window size changes
    myWindowClass.style =	  CS_HREDRAW | CS_VREDRAW;
    // Procedure to be called with messages for this window class
    myWindowClass.lpfnWndProc =   MyRaisedFrameWindowProcedure;
    // The extra space in class struct
    myWindowClass.cbClsExtra =    0;
    // The extra space in window struct
    myWindowClass.cbWndExtra =    0;
    // The application's handle
    myWindowClass.hInstance =     gProgram.applicationInstance;
    // Set the icon for this window class
    myWindowClass.hIcon =	  NULL;
    // Set the cursor for this window class
    myWindowClass.hCursor =       LoadCursor (NULL, IDC_ARROW);
    // Set the background colour for this window
    myWindowClass.hbrBackground = GetSysColorBrush (COLOR_BTNFACE);
    // Set the menu for this window class
    myWindowClass.lpszMenuName =  NULL;
    // Name of the window class
    myWindowClass.lpszClassName = myRaisedFrameWindowClassName; 
    // Set the icon for this class.
    myWindowClass.hIconSm =       NULL;
    
    myResult = RegisterClassEx (&myWindowClass);
    if (myResult == 0)
    {
    	EdFail_Warn (IDS_REGISTERCLASSFAIL, __FILE__, __LINE__, 
    		     GetLastError ());
    }
} // JSCon_Init


/************************************************************************/
/* JSCon_Finalize							*/
/************************************************************************/
void	JSCon_Finalize (void)
{
    JSTop_Finalize ();

    DeleteCriticalSection (&stGetLineCriticalSection);
    DeleteCriticalSection (&stLineBufferCriticalSection);
} // JSCon_Finalize


/************************************************************************/
/* JSCon_PropertiesSet							*/
/************************************************************************/
void	JSCon_PropertiesSet (JSCon_Properties pmProperties)
{
    JSTop_PropertiesSet (pmProperties.topProperties);
    EdText_PropertiesSet (pmProperties.textProperties);
    
    stProperties = pmProperties.myProperties;
    stPropertiesChanged = TRUE;
} // JSCon_PropertiesSet

			  
/************************************************************************/
/* JSCon_PropertiesImplementChanges					*/
/************************************************************************/
void	JSCon_PropertiesImplementChanges (HWND pmConsoleWindow)
{
    ConsoleWindowInfoPtr	myInfo;
        
    myInfo = MyGetConsoleWindowInfo (pmConsoleWindow);
    
    if (stPropertiesChanged)
    {
    	// Make changes for the module
    	MySetDisplayFonts (myInfo -> consoleTextWindow);
    	stPropertiesChanged = FALSE;
    }
    
    JSTop_PropertiesImplementChanges (myInfo -> buttonBarWindow);
    // ... 
} // JSCon_PropertiesImplementChanges


/************************************************************************/
/* JSCon_Create								*/
/************************************************************************/
HWND	JSCon_Create (const char *pmClassName)
{
    char			myConsoleWindowClassName [256];
    char			myWindowTitle [1024];
    HWND			myConsoleWindow;
    WindowCreationInfo		myWindowCreationInfo;
        
    // Load the class name (we've already loaded it once, so no error checking)    
    EdGUI_LoadString (IDS_CONSOLE_WINDOW_NAME,
	myConsoleWindowClassName, sizeof (myConsoleWindowClassName));

    myWindowCreationInfo.className = pmClassName;
    
    // Create the editor window    
    myConsoleWindow = CreateWindow (
    	myConsoleWindowClassName, 		// Window class
        "Standard Input/Output", 		// Window title
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, 	// Window style
    	CW_USEDEFAULT, 				// Initial x location
    	CW_USEDEFAULT, 				// Initial y location
    	CW_USEDEFAULT, 				// Initial x size
    	CW_USEDEFAULT, 				// Initial y size
    	HWND_DESKTOP, 				// Parent window handle
    	NULL, 					// Window menu handle
    	gProgram.applicationInstance,		// Program instance handle
    	&myWindowCreationInfo);			// Creation parameters
    
    if (stProperties.logWindowMessages)
    {
//    	EdLog_Log ("Console Window = %x  (Finished Creation)", myConsoleWindow);
    }

    // If we failed to create the window, return immediately
    if (myConsoleWindow == NULL)
    {
    	JS_ErrorMessage (IDS_CREATEWINDOWFAIL, GetLastError ());
    }
    
    strcpy (myWindowTitle, pmClassName);
    strcat (myWindowTitle, 
            " - Running - Standard Input/Output - Ready to Program");
    SetWindowText (myConsoleWindow, myWindowTitle);

    ShowWindow (myConsoleWindow, SW_SHOWNORMAL);

    return myConsoleWindow;
} // JSCon_Create

    			      
/************************************************************************/
/* JSCon_GetLineOfInput							*/
/*									*/
/* Algorithm: Convert all the text in the keyboard buffer into the line	*/
/*	      buffer up to a single newline. Then check if a newline	*/
/*	      was found.  If it was, then read the line off and return.	*/
/*	      Otherwise, set the indicator to note that we are		*/
/*	      accepting input and wait.  When we get the signal, we 	*/
/*	      check again.						*/
/************************************************************************/
void	JSCon_GetLineOfInput (HWND pmConsoleWindow, char *pmBuffer, 
			      int pmBufferSize)
{
    ConsoleWindowInfoPtr	myInfo;
    
    // Enter critical section to avoid interfering lineBuffer mods
    EnterCriticalSection (&stGetLineCriticalSection);

    // Process all the input currently in the keyboard buffer.
    SendMessage (pmConsoleWindow, WM_PROCESS_INPUT, (WPARAM) NULL, 
    		       (LPARAM) NULL);
    
    // Is there a newline there?
    myInfo = MyGetConsoleWindowInfo (pmConsoleWindow);
    
    while (!myInfo -> lineBufferNewLineWaiting)
    {
	myInfo -> acceptingInput = TRUE;    
    	SendNotifyMessage (pmConsoleWindow, WM_SHOW_CURSOR, (WPARAM) NULL, 
    			   (LPARAM) NULL);
    	
    	// Wait for signal that newline received
	WaitForSingleObject (stNewLineReceivedEvent, INFINITE);
	ResetEvent (stNewLineReceivedEvent);
    	SendNotifyMessage (pmConsoleWindow, WM_HIDE_CURSOR, (WPARAM) NULL, 
    			   (LPARAM) NULL);
    }

    if (pmBufferSize <= myInfo -> lineBufferSize)
    {
    	// TW Serious Problem
    }
    strncpy (pmBuffer, myInfo -> lineBuffer, myInfo -> lineBufferSize);
    pmBuffer [myInfo -> lineBufferSize] = 0;
    myInfo -> lineBufferNewLineWaiting = FALSE;
    myInfo -> lineBufferSize = 0;
    	    
    // Leave critical section to avoid interfering lineBuffer mods
    LeaveCriticalSection (&stGetLineCriticalSection);
} // JSCon_GetLineOfInput


/******************************/
/* Static callback procedures */
/******************************/
/************************************************************************/
/* MyConsoleTextWindowProcedure						*/
/*									*/
/* Callback procedure for the main editor window.			*/
/************************************************************************/
static LRESULT CALLBACK MyConsoleTextWindowProcedure (HWND pmConsoleTextWindow, 
						      UINT pmMessage, 
    						      WPARAM pmWParam, 
    						      LPARAM pmLParam)
{
    ConsoleWindowInfoPtr	myInfo;
    
    // Log the windows messages
    if (stProperties.logWindowMessages)
    {
//	EdLog_PrintWindowsMessage ("ConsoleText", pmConsoleTextWindow, 
//	    pmMessage, pmWParam, pmLParam);
    }
    
    switch (pmMessage)
    {    
    	case WM_CREATE:
	    if (stProperties.logWindowMessages)
	    {
//	    	EdLog_Log ("Console Text Window = %x  (Starting Creation)", 
//	    	    pmConsoleTextWindow);
	    }

	    return MyCreateConsoleTextWindow (pmConsoleTextWindow);

        case WM_PAINT:
            MyDisplayText (pmConsoleTextWindow, TRUE, ALL_LINES, ALL_LINES);
            return 0;
        
	case WM_SIZE:
	    MyResizeConsoleTextWindow (pmConsoleTextWindow);
	    return 0;
	    
	//
	// Scroll bar handling
	//
	case WM_HSCROLL:
	    MyScrollHandlerHorz (pmConsoleTextWindow, LOWORD (pmWParam), 
	    	HIWORD (pmWParam));
	    return 0;
	    
	case WM_VSCROLL:
	    MyScrollHandlerVert (pmConsoleTextWindow, LOWORD (pmWParam), 
	        HIWORD (pmWParam));
	    return 0;

	//
	// Keystroke handling
	//
	case WM_CHAR:
	    MyKeystrokeHandler (pmConsoleTextWindow, pmWParam);
	    return 0;

	//
	// Mouse handling
	//
	case WM_MOUSEMOVE:
	    myInfo = MyGetConsoleWindowInfo (pmConsoleTextWindow);
	    if (myInfo -> selectingWithMouse)
	    {
	    	if (MyMouseDownHandler (pmConsoleTextWindow, 
	    	    GET_X_LPARAM (pmLParam), GET_Y_LPARAM (pmLParam), 
	    	    TRUE, FALSE))
                {
            	    myInfo -> verticalTicks = 0;
            	    myInfo -> horizontalTicks = 0;
                }
	    }
            return 0;

        case WM_LBUTTONDOWN:
            // Returns true if mouse down in text region
            if (MyMouseDownHandler (pmConsoleTextWindow, 
                GET_X_LPARAM (pmLParam), GET_Y_LPARAM (pmLParam), 
                pmWParam & MK_SHIFT, FALSE))
            {
	    	myInfo = MyGetConsoleWindowInfo (pmConsoleTextWindow);
    
            	SetCapture (pmConsoleTextWindow);
            	SetTimer (pmConsoleTextWindow, SCROLL_TIMER, 1, NULL);
            	myInfo -> verticalTicks = 0;
            	myInfo -> horizontalTicks = 0;
            	myInfo -> selectingWithMouse = TRUE;

		if (((UINT) (GetMessageTime () - myInfo -> lastClickTime) >
		     GetDoubleClickTime ()) || !myInfo -> clickOnSameChar)
            	{
            	    myInfo -> numClicks = 1;
            	}
            	else
            	{
            	    myInfo -> numClicks++;
            	}
            	myInfo -> lastClickTime = GetMessageTime ();

		if (myInfo -> numClicks == 2)
		{
            	    MyMouseExtendSelectionToWord (pmConsoleTextWindow);
		}
		else if (myInfo -> numClicks == 3)
		{
            	    MyMouseExtendSelectionToLine (pmConsoleTextWindow);
		}
		else if (myInfo -> numClicks == 4)
            	{
            	    myInfo -> numClicks = 1;
            	}

            	return 0;
            }
            break;
                    
        case WM_LBUTTONUP:
	    myInfo = MyGetConsoleWindowInfo (pmConsoleTextWindow);
    
            if (myInfo -> selectingWithMouse)
            {
            	MyMouseDownHandler (pmConsoleTextWindow, 
            	    GET_X_LPARAM (pmLParam), GET_Y_LPARAM (pmLParam), 
            	    TRUE, FALSE);
            	ReleaseCapture ();
            	KillTimer (pmConsoleTextWindow, SCROLL_TIMER);
            	myInfo -> selectingWithMouse = FALSE;
            	return 0;
            }
            	    
	case WM_TIMER:
	    if (pmWParam == SCROLL_TIMER)
	    {
	    	// If the mouse is currently in the window (vertically), 
	    	// reset the timer to 0. Otherwise compare the accumulated
	    	// time to the distance from the top or bottom of the text
	    	// window and scroll if enough time has passed.  Scroll faster
	    	// if the mouse is farther away from the window.
	    	POINT	myPoint;
	    	RECT	myRect;
	    	
	    	myInfo = MyGetConsoleWindowInfo (pmConsoleTextWindow);
	    	
                myRect = myInfo -> textRect;
                
	    	myInfo -> verticalTicks++;
	    	myInfo -> horizontalTicks++;
	    	
	    	GetCursorPos (&myPoint);
	    	ScreenToClient (pmConsoleTextWindow, &myPoint);
	    	
	    	if (myPoint.x < myRect.left)
	    	{
	    	    while (myInfo -> horizontalTicks > HORIZ_TICKS_PER_SCROLL)
	    	    {
	    	    	MyScrollHandlerHorz (pmConsoleTextWindow, 
	    	    	    SB_LINELEFT, 0);
	    	    	myInfo -> horizontalTicks -= HORIZ_TICKS_PER_SCROLL;
	    	    }
	    	    MyMouseDownHandler (pmConsoleTextWindow, myPoint.x, 
	    	    	myPoint.y, TRUE, TRUE);
	    	}
	    	else if (myPoint.x > myRect.right)
	    	{
	    	    while (myInfo -> horizontalTicks > HORIZ_TICKS_PER_SCROLL)
	    	    {
	    	    	MyScrollHandlerHorz (pmConsoleTextWindow, 
	    	    	    SB_LINERIGHT, 0);
	    	    	myInfo -> horizontalTicks -= HORIZ_TICKS_PER_SCROLL;
	    	    }
	    	    MyMouseDownHandler (pmConsoleTextWindow, myPoint.x, 
	    	        myPoint.y, TRUE, TRUE);
	    	}
	    	else
	    	{
	    	    myInfo -> horizontalTicks = 0;
	    	}
	    	
	    	if (myPoint.y < myRect.top)
	    	{
	    	    int ticks = max (1, 6 - ((myRect.top - myPoint.y) / 
	    	        stTextFontCharHeight));
	    	    while (myInfo -> verticalTicks > ticks)
	    	    {
	    	    	MyScrollHandlerVert (pmConsoleTextWindow, SB_LINEUP, 0);
	    	    	myInfo -> verticalTicks -= ticks;
	    	    }
	    	    MyMouseDownHandler (pmConsoleTextWindow, myPoint.x, 
	    	        myPoint.y, TRUE, TRUE);
	    	}
	    	else if (myPoint.y > myRect.bottom)
	    	{
	    	    int ticks = max (1, 6 - ((myPoint.y - myRect.bottom) / 
	    	        stTextFontCharHeight));
	    	    while (myInfo -> verticalTicks > ticks)
	    	    {
	    	    	MyScrollHandlerVert (pmConsoleTextWindow, 
	    	    	    SB_LINEDOWN, 0);
	    	    	myInfo -> verticalTicks -= ticks;
	    	    }
	    	    MyMouseDownHandler (pmConsoleTextWindow, myPoint.x,
	    	        myPoint.y, TRUE, TRUE);
	    	}
	    	else
	    	{
	    	    myInfo -> verticalTicks = 0;
	    	}
	    }
	    return 0;
	    
	case WM_MOUSEWHEEL:
	    if (stWheelMouseDeltaPerLine != 0)
	    {
	    	myInfo = MyGetConsoleWindowInfo (pmConsoleTextWindow);
	    	
	        myInfo -> accumulateDelta += (short) HIWORD (pmWParam);
	        while (myInfo -> accumulateDelta >= stWheelMouseDeltaPerLine)
	    	{
	    	    MyScrollHandlerVert (pmConsoleTextWindow, SB_LINEUP, 0);
	    	    myInfo -> accumulateDelta -= stWheelMouseDeltaPerLine;
	    	}
	    	while (myInfo -> accumulateDelta <= -stWheelMouseDeltaPerLine)
	    	{
	    	    MyScrollHandlerVert (pmConsoleTextWindow, SB_LINEDOWN, 0);
	    	    myInfo -> accumulateDelta += stWheelMouseDeltaPerLine;
	    	}
	    	return 0;
	    }
	    break;
	    
	//
	// Focus handling
	//
        case WM_SETFOCUS:
	    MyCaretCreate (pmConsoleTextWindow);
            return 0;

	case WM_KILLFOCUS:
	    myInfo = MyGetConsoleWindowInfo (pmConsoleTextWindow);
            if (myInfo -> runStatus == STATUS_WAITING_FOR_INP)
            {
	        MyCaretHide (pmConsoleTextWindow);
            }
            myInfo -> caretCreated = FALSE;
	    DestroyCaret ();
            MyDisplayText (pmConsoleTextWindow, FALSE, ALL_LINES, ALL_LINES);
	    return 0;
	    
	 default:
	    if ((pmMessage == stMouseWheelMessage) && 
	    	(stWheelMouseDeltaPerLine != 0))
	    {
	    	myInfo = MyGetConsoleWindowInfo (pmConsoleTextWindow);
	    	
	        myInfo -> accumulateDelta += (short) HIWORD (pmWParam);
	        while (myInfo -> accumulateDelta >= stWheelMouseDeltaPerLine)
	    	{
	    	    MyScrollHandlerVert (pmConsoleTextWindow, SB_LINEUP, 0);
	    	    myInfo -> accumulateDelta -= stWheelMouseDeltaPerLine;
	    	}
	    	while (myInfo -> accumulateDelta <= -stWheelMouseDeltaPerLine)
	    	{
	    	    MyScrollHandlerVert (pmConsoleTextWindow, SB_LINEDOWN, 0);
	    	    myInfo -> accumulateDelta += stWheelMouseDeltaPerLine;
	    	}
	    	return 0;
            }
    }
    
    return DefWindowProc (pmConsoleTextWindow, pmMessage, pmWParam, 
        		  pmLParam);
} // MyConsoleTextWindowProcedure


/************************************************************************/
/* MyConsoleWindowProcedure						*/
/*									*/
/* Callback procedure for the main editor window.			*/
/************************************************************************/
static LRESULT CALLBACK MyConsoleWindowProcedure (HWND pmConsoleWindow, 
						  UINT pmMessage, 
    						  WPARAM pmWParam, 
    						  LPARAM pmLParam)
{
    WindowCreationInfoPtr	myWindowCreationInfo;
    
    // Log the windows messages
    if (stProperties.logWindowMessages)
    {
//	EdLog_PrintWindowsMessage ("Edit", pmConsoleWindow, pmMessage, 
//	    pmWParam, pmLParam);
    }
    
    switch (pmMessage)
    {
    	case WM_CREATE:
	    if (stProperties.logWindowMessages)
	    {
//	    	EdLog_Log ("Edit Window = %x  (Starting Creation)", 
//	    	    pmConsoleWindow);
	    }
	    myWindowCreationInfo = (WindowCreationInfoPtr)
	    	(((LPCREATESTRUCT) pmLParam) -> lpCreateParams);

	    return MyCreateConsoleWindow (pmConsoleWindow, 
	    				  myWindowCreationInfo);

        case WM_CLOSE:
            // TW This needs to be reworked for one window mode
            if (pmWParam || MyCheckForClose (pmConsoleWindow))
            {
//            	if (Ed_RemoveFromWindowList (pmConsoleWindow))
//            	{
            	    // Handle normally (= the window will be destroyed)
            	    break;
//            	}
            }
            return 0;

	case WM_DESTROY:
	    ShowWindow (pmConsoleWindow, SW_HIDE);
	    JS_TerminateProcess ();
	    break;
	    	    
	case WM_SIZE:
	    MyResizeConsoleWindow (pmConsoleWindow);
	    return 0;
	    
        case WM_COMMAND:
            if (MyCommandHandler (pmConsoleWindow, LOWORD (pmWParam)))
            {
                return 0;
            }
            EdFail_Fatal (IDS_COMMAND_NOT_PROCESSED, __FILE__, __LINE__, 0,
            	LOWORD (pmWParam));
	    break; // Never reaches here            	

        case WM_ACTIVATE:
            if (LOWORD (pmWParam) != WA_INACTIVE)
            {
            	SetFocus (MyGetConsoleTextWindow (pmConsoleWindow));
            }
            return 0;
            
        case WM_SETFOCUS:
            SetFocus (MyGetConsoleTextWindow (pmConsoleWindow));
            return 0;	

	case WM_GETMINMAXINFO:
	    if (GetWindowLong (pmConsoleWindow, WINDOW_MEM_WINDOW_INITIALIZED))
	    {
	    	MINMAXINFO	*myMinMaxInfo = (MINMAXINFO *) pmLParam;
	    	long		myWindowWidth, myWindowHeight;

		// Calculate the rectangle size
		MyGetWindowSize (pmConsoleWindow, MIN_ROWS, MIN_COLS, 
		    &myWindowWidth, &myWindowHeight);

		// Place the rectangle	    	
	    	myMinMaxInfo -> ptMinTrackSize.x = myWindowWidth;
	    	myMinMaxInfo -> ptMinTrackSize.y = myWindowHeight;
	    	  
	    	return 0;
	    }
	    break;
        
        case WM_SETTINGCHANGE:
	    // Set wheel mouse settings
	    stWheelMouseDeltaPerLine = EdGUI_GetMouseWheelSettings ();
            return 0;
            
	case WM_ADD_TEXT:
	    MyAddText (pmConsoleWindow, (char *) pmWParam, (int) pmLParam);
	    return 0;            
            
	case WM_DELETE_TEXT:
	    MyDeleteText (pmConsoleWindow, (int) pmWParam);
	    return 0;            

	case WM_SET_RUN_STATUS:
	    MySetRunState (pmConsoleWindow, (int) pmWParam);
	    return 0;
	    
	case WM_SHOW_CURSOR:
	    MyCaretDisplay (MyGetConsoleTextWindow (pmConsoleWindow));
	    return 0;
	    
	case WM_HIDE_CURSOR:
	    MyCaretHide (MyGetConsoleTextWindow (pmConsoleWindow));
	    return 0;
	    
	case WM_PROCESS_INPUT:
    	    MyProcessFromKeyboardToLineBuffer (
    	    	MyGetConsoleTextWindow (pmConsoleWindow));
	    return 0;    	    
    }
    
    return DefWindowProc (pmConsoleWindow, pmMessage, pmWParam, pmLParam);
} // MyConsoleWindowProcedure


/************************************************************************/
/* MyRaisedFrameWindowProcedure						*/
/*									*/
/* Callback routine from the raised frame window. 			*/
/************************************************************************/
static LRESULT CALLBACK	MyRaisedFrameWindowProcedure (HWND pmWindow, 
						      UINT pmMessage, 
						      WPARAM pmWParam, 
						      LPARAM pmLParam)
{
    HDC		myDeviceContext;
    PAINTSTRUCT	myPaintStruct;
    RECT	myWindowRect;
    
    if (pmMessage == WM_PAINT)
    {
    	myDeviceContext = BeginPaint (pmWindow, &myPaintStruct);
    	SetMapMode (myDeviceContext, MM_TEXT);
    	GetClientRect (pmWindow, &myWindowRect);
    	DrawEdge (myDeviceContext, &myWindowRect, EDGE_RAISED, BF_RECT);
    	EndPaint (pmWindow, &myPaintStruct);
    	return 0;
    }
        	
    return DefWindowProc (pmWindow, pmMessage, pmWParam, pmLParam);
} // MyRaisedFrameWindowProcedure


/*********************/
/* Static procedures */
/*********************/
/************************************************************************/
/* MyAddText								*/
/************************************************************************/
static void	MyAddText (HWND pmConsoleWindow, char *pmText,
			   int pmFlag)
{
    ConsoleWindowInfoPtr	myInfo;
    TextPtr			myTextPtr;
    HWND			myConsoleTextWindow;
    int 			myOrigSelFrom, myOrigSelTo;
    int				myTextPos;
    BOOL			myAtEnd;
    int				myAction;
    int				myLine, myCol;
    
    myInfo = MyGetConsoleWindowInfo (pmConsoleWindow);
    myTextPtr = myInfo -> text;
    myConsoleTextWindow = myInfo -> consoleTextWindow;
    myTextPos = MyGetCurrentTextPos (myTextPtr);

    if ((myInfo -> topLine <= myTextPtr -> numLines) &&
    	(myTextPtr -> numLines <= myInfo -> topLine + myInfo -> windowHeight))
    {
    	myAtEnd = TRUE;
    }
    else
    {
    	myAtEnd = FALSE;
    }        
    
    myOrigSelFrom = myTextPtr -> selFrom;
    myOrigSelTo = myTextPtr -> selTo;
    
    myTextPtr -> selFrom = myTextPos;
    myTextPtr -> selTo = myTextPos;
    myTextPtr -> locationOK = FALSE;
    
    // If this is echoing input
    if (pmFlag == BEGINNING_OF_LINE)
    {
    	myInfo -> textStartPosOfLastInput = myTextPtr -> selFrom;
    }           

    // Insert the text at that point
    myAction = EdText_InsertText (myTextPtr, pmText, strlen (pmText),
        			  FALSE, NULL);
    
    // If this is echoing output
    if (pmFlag == FREE_BUFFER)
    {
    	myInfo -> textEndPosOfLastOutput = myTextPtr -> selTo;
    	free (pmText);
    }           
        
    myTextPtr -> selFrom = myOrigSelFrom;
    myTextPtr -> selTo = myOrigSelTo;
    myTextPtr -> locationOK = FALSE;
    
    if (myAction & (KEY_LONGEST_LINE_CHANGED | KEY_NUM_LINES_CHANGED))
    {
    	MySynchScrollBarsToText (myConsoleTextWindow);
    }

    if (myAction & KEY_REDRAW_FROM_SELECTION)
    {
        MyCaretDisplay (myConsoleTextWindow);
    	MyDisplayText (myConsoleTextWindow, FALSE, ALL_LINES, ALL_LINES);
    }
    else if (myAction & KEY_UPDATE_LINE)
    {
        MyCaretDisplay (myConsoleTextWindow);
	MyGetCurrentLineCol (myTextPtr, &myLine, &myCol);
    	MyDisplayText (myConsoleTextWindow, FALSE, myLine, myLine);
    }

    if (myAtEnd)
    {    
    	MyScrollToSelection (myConsoleTextWindow, SCROLL_NORMAL, FALSE, TRUE);
    }
} // MyAddText


/************************************************************************/
/* MyCaretCreate							*/
/************************************************************************/
static void	MyCaretCreate (HWND pmConsoleTextWindow)
{
    ConsoleWindowInfoPtr	myInfo;
    int				myCaretWidth;
    
    if (stProperties.caretWidth == 3)
    {
	myCaretWidth = stTextFontCharWidth;
    }
    else
    {
	myCaretWidth = stProperties.caretWidth;
    }

    CreateCaret (pmConsoleTextWindow, NULL, myCaretWidth, stTextFontCharHeight);
    myInfo = MyGetConsoleWindowInfo (pmConsoleTextWindow);
    myInfo -> caretCreated = TRUE;
    if (myInfo -> runStatus == STATUS_WAITING_FOR_INP)
    {
        MyCaretDisplay (pmConsoleTextWindow);
    }
    MyDisplayText (pmConsoleTextWindow, FALSE, ALL_LINES, ALL_LINES);
} // MyCaretCreate


/************************************************************************/
/* MyCaretDisplay							*/
/************************************************************************/
static void	MyCaretDisplay (HWND pmConsoleTextWindow)
{
    ConsoleWindowInfoPtr	myInfo;
            
    myInfo = MyGetConsoleWindowInfo (pmConsoleTextWindow);
    	
    if (!myInfo -> caretCreated) return;

    if (myInfo -> acceptingInput)
    {
        int	myLine, myCol, myX, myY;

	// Get the position of the text input position (at end or one
	// from the end if a new line has been added)
	MyGetCurrentLineCol (myInfo -> text, &myLine, &myCol);
	
        myX = (myCol - myInfo -> leftCol) * stTextFontCharWidth +
            myInfo -> textRect.left;
        myY = (myLine - myInfo -> topLine) * stTextFontCharHeight +
            myInfo -> textRect.top;
	if (stProperties.caretWidth == 2)
	{
	    myX--;
	}
        SetCaretPos (myX, myY);
        if (!myInfo -> caretShowing)
        {
            myInfo -> caretShowing = TRUE;
	    ShowCaret (pmConsoleTextWindow);
        }
    }
    else
    {
    	if (myInfo -> caretShowing)
    	{
            myInfo -> caretShowing = FALSE;
    	    HideCaret (pmConsoleTextWindow);
    	}
    }
} // MyCaretDisplay


/************************************************************************/
/* MyCaretHide								*/
/************************************************************************/
static void	MyCaretHide (HWND pmConsoleTextWindow)
{
    ConsoleWindowInfoPtr	myInfo;
            
    // Get the text display window information
    myInfo = MyGetConsoleWindowInfo (pmConsoleTextWindow);
    	
    if (myInfo -> caretShowing)
    {
        myInfo -> caretShowing = FALSE;
    	HideCaret (pmConsoleTextWindow);
    }
} // MyCaretHide


/************************************************************************/
/* MyCaretMove								*/
/************************************************************************/
static void	MyCaretMove (HWND pmConsoleTextWindow)
{
    ConsoleWindowInfoPtr	myInfo;
    int				myLine, myCol, myX, myY;
            
    // Create the ddit window information
    myInfo = MyGetConsoleWindowInfo (pmConsoleTextWindow);
            	
    // Get the position of the text input position (at end or one
    // from the end if a new line has been added)
    MyGetCurrentLineCol (myInfo -> text, &myLine, &myCol);

    myX = (myCol - myInfo -> leftCol) * stTextFontCharWidth +
        myInfo -> textRect.left;
    myY = (myLine - myInfo -> topLine) * stTextFontCharHeight +
        myInfo -> textRect.top;
    if (stProperties.caretWidth == 2)
    {
        myX--;
    }
    SetCaretPos (myX, myY);
} // MyCaretMove


/************************************************************************/
/* MyCheckForClose							*/
/************************************************************************/
static BOOL	MyCheckForClose (HWND pmConsoleWindow)
{
    ConsoleWindowInfoPtr	myInfo;
    int				myResult;
            
    // Check if a program is running and if it is, give a message
    myInfo = MyGetConsoleWindowInfo (pmConsoleWindow);
    
    if ((myInfo -> runStatus == STATUS_PAUSED) ||
        (myInfo -> runStatus == STATUS_FINISHED))
    {
    	return TRUE;
    }
    
    myResult = EdGUI_Message1 (pmConsoleWindow, 
    	MB_YESNO | MB_ICONWARNING, IDS_STOP_PROGRAM_TITLE, 
    	IDS_STOP_PROGRAM, myInfo -> className);

    // Press Cancel, then don't close
    if (myResult == IDNO)
    {
	return FALSE;
    }

    return TRUE;
} // MyCheckForClose


/************************************************************************/
/* MyCommandHandler							*/
/************************************************************************/
static BOOL	MyCommandHandler (HWND pmConsoleWindow, short pmCommand)
{
    ConsoleWindowInfoPtr	myInfo;
    
    myInfo = MyGetConsoleWindowInfo (pmConsoleWindow);
    
    switch (pmCommand)
    {
	case COMMAND_PAUSE_RESUME:
	    if (myInfo -> runStatus == STATUS_PAUSED)
	    {
	    	JS_ResumeJavaProgram ();
	    }
	    else
	    {
	    	JS_PauseJavaProgram ();
	    }
	    break;
	case COMMAND_TERMINATE:
	    JS_TerminateProcess ();
	    break;
	case COMMAND_PRINT_FILE:
    	    if (!EdText_IsEmpty (myInfo -> text))
    	    {
    	    	JS_SubmitPrintJob (pmConsoleWindow, myInfo -> text -> text, 
    	    			   myInfo -> text -> textLen);
	    }	            
    	    else
    	    {
    	    	EdGUI_Message1 (pmConsoleWindow, 0, IDS_EMPTY_FILE_TITLE,
    	    	    IDS_NOTHING_TO_PRINT);
	    }    	    	    
	    break;
	case COMMAND_SAVEAS_FILE:
    	    if (!EdText_IsEmpty (myInfo -> text))
    	    {
    	    	MyFileSave (pmConsoleWindow); 
    	    }
    	    else
    	    {
    	    	EdGUI_Message1 (pmConsoleWindow, 0, IDS_EMPTY_FILE_TITLE,
    	    	    IDS_NOTHING_TO_SAVE);
	    }    	    	    
	    break;
	case COMMAND_COPY:
	    MyCopy (pmConsoleWindow);
	    break;
	case COMMAND_PASTE:
	    MyPaste (pmConsoleWindow);
	    break;
	case COMMAND_SELECT_ALL:
	    MySelectAll (MyGetConsoleTextWindow (pmConsoleWindow));
	    break;
	case COMMAND_CLOSE:
	    SendMessage (pmConsoleWindow, WM_CLOSE, 0, 0);
	    break;	    
	case COMMAND_EXIT:
	    JS_SignalMainToTerminate ();
	    JS_TerminateProcess ();
    	    break;
    	default:
    	    return FALSE;
    }
    
    return TRUE;
} // MyCommandHandler


/************************************************************************/
/* MyCreateConsoleTextWindow						*/
/************************************************************************/
static int	MyCreateConsoleTextWindow (HWND pmConsoleTextWindow)
{
    //
    // Initialize the extra window memory
    //
	
    // Set the window type            
    SetWindowLong (pmConsoleTextWindow, WINDOW_MEM_WINDOW_TYPE, 
        (long) WINDOW_TYPE_CONSOLE_TEXT);

    // Set the window initialized                
    SetWindowLong (pmConsoleTextWindow, WINDOW_MEM_WINDOW_INITIALIZED, 
        (long) TRUE);
            
    return 0;
} // MyCreateConsoleTextWindow

				    
/************************************************************************/
/* MyCreateConsoleWindow						*/
/************************************************************************/
static int	MyCreateConsoleWindow (HWND pmConsoleWindow, 
				   WindowCreationInfoPtr pmWindowCreationInfo)
{
    ConsoleWindowInfoPtr	myInfo;
    long			myWindowLeft, myWindowTop;
    long			myWindowWidth, myWindowHeight;
    
    //
    // Initialize the extra window memory
    //
	
    // Set the window type            
    SetWindowLong (pmConsoleWindow, WINDOW_MEM_WINDOW_TYPE, 
        (long) WINDOW_TYPE_CONSOLE);

    // Set the window initialized                
    SetWindowLong (pmConsoleWindow, WINDOW_MEM_WINDOW_INITIALIZED, 
        (long) FALSE);
            
    // Create the ddit window information
    myInfo = MyCreateConsoleWindowInfo ();
    if (myInfo == NULL)
    {
    	return -1;
    }
    SetWindowLong (pmConsoleWindow, WINDOW_MEM_WINDOW_INFO, (long) myInfo);
    	    
    //
    // Add the elements to the window
    //

    // Add the button bar window
    myInfo -> buttonBarWindow = JSTop_Create (pmConsoleWindow);
    if (myInfo -> buttonBarWindow == NULL)
    	return -1;

    // Add the console text window
    myInfo -> consoleTextWindow = MyConTextCreate (pmConsoleWindow);
    if (myInfo -> consoleTextWindow == NULL)
    	return -1;

    // Add the edit text
    myInfo -> text = EdText_Create ();
    if (myInfo -> text == NULL)
        return -1;
    
    // Initialize info fields     
    myInfo -> className = malloc (
    				strlen (pmWindowCreationInfo -> className) + 1);
    // TW Check for NULL
    strcpy (myInfo -> className, pmWindowCreationInfo -> className);
    myInfo -> runStatus = STATUS_RUNNING;
    myInfo -> acceptingInput = FALSE;

    EdText_InitializeTextRecord (myInfo -> text);  
    
    // If the font properties have not already been determined, 
    // determine them now (we needed a window to do so).  This must be
    // called *after* the top widow and status window created
    JSCon_PropertiesImplementChanges (pmConsoleWindow);

    // Set the window to initialized
    SetWindowLong (pmConsoleWindow, WINDOW_MEM_WINDOW_INITIALIZED, (long) TRUE);
    
    // Add to the window list
//    Ed_AddToWindowList (pmConsoleWindow, WINDOW_TYPE_CONSOLE);

    // Calculate the window size
    MyGetWindowSize (pmConsoleWindow, stProperties.textRows, 
        stProperties.textCols, &myWindowWidth, &myWindowHeight);

    // Calculate the top, left corner of the window
    MyGetWindowDisplacement (FALSE, &myWindowWidth, &myWindowHeight,
    	&myWindowLeft, &myWindowTop);

    // Set the window to the correct position
    SetWindowPos (pmConsoleWindow, HWND_TOP, myWindowLeft, myWindowTop,
    	myWindowWidth, myWindowHeight, 0);

    // Setore class name
    
    MySynchScrollBarsToText (myInfo -> consoleTextWindow);
    
    MyDisplayText (myInfo -> consoleTextWindow, FALSE, ALL_LINES, ALL_LINES);
    MyCaretDisplay (myInfo -> consoleTextWindow);

    return 0;
} // MyCreateConsoleWindow


/************************************************************************/
/* MyCreateConsoleWindowInfo						*/
/************************************************************************/
static ConsoleWindowInfoPtr	MyCreateConsoleWindowInfo (void)
{
    ConsoleWindowInfoPtr	myInfo;
    
    // Allocate the structure for the information about the edit window
    myInfo = (ConsoleWindowInfoPtr) malloc (sizeof (ConsoleWindowInfo));
    
    if (myInfo != NULL)
    {
    	ZeroMemory (myInfo, sizeof (ConsoleWindowInfo));
    
        myInfo -> textRect.top = -1000;
	myInfo -> textRect.bottom = -1000;
        myInfo -> textRect.left = -1000;
	myInfo -> textRect.right = -1000;
    }

    return myInfo;  
} // MyCreateConsoleWindowInfo


/************************************************************************/
/* MyConTextCreate							*/
/************************************************************************/
static HWND	MyConTextCreate (HWND pmConsoleWindow)
{
    HINSTANCE		myApplicationInstance;
    char		myClassName [256];
    HWND		myConsoleTextWindow;
    
    // Get the program instance
    myApplicationInstance = 
        (HINSTANCE) GetWindowLong (pmConsoleWindow, GWL_HINSTANCE);
        
    // Create the console text window
    EdGUI_LoadString (IDS_CONSOLETEXT_WINDOW_NAME, myClassName,
    	sizeof (myClassName));

    myConsoleTextWindow = CreateWindow (
    	myClassName, 			// Window class
        NULL,	 			// Window title
        WS_CHILDWINDOW | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL, // Window style
    	0, 0, 0, 0, 			// Initial location & size
    	pmConsoleWindow, 		// Parent window handle
    	NULL, 				// Window menu handle
    	myApplicationInstance,	 	// Program instance handle
    	NULL);				// Creation parameters
    
    if (stProperties.logWindowMessages)
    {
//    	EdLog_Log ("Text Display Window = %x  (Finished Creation)", 
//    	    myConsoleTextWindow);
    }

    return myConsoleTextWindow;
} // MyConTextCreate


/************************************************************************/
/* MyCopy								*/
/************************************************************************/
static void	MyCopy (HWND pmConsoleWindow)
{
    TextPtr	myTextPtr;
    int		mySelectionLen;
    int		myFromLine, myFromCol, myToLine, myToCol;
    HGLOBAL	myGlobalHandle;
    char	*myGlobalPtr;
    char	*mySrc, *myEnd, *myDest;
    
    myTextPtr = MyGetTextPtr (pmConsoleWindow);
    
    // We need to convert the \n's to \r\n's in the clipboard.
    mySelectionLen = abs (myTextPtr -> selTo - myTextPtr -> selFrom);
    
    if (mySelectionLen == 0)
    {
    	MessageBeep (MB_ICONEXCLAMATION);
    	return;
    }

    EdText_GetSelectionLinesCols (myTextPtr, &myFromLine, &myFromCol,
    				  &myToLine, &myToCol);
    
    //    				 
    // Copy the text to a global handle
    //
    
    // Allocate the global memory and lock it down into a pointer
    myGlobalHandle = GlobalAlloc (GHND | GMEM_SHARE, mySelectionLen + 
    				  myToLine - myFromLine + 1);
    if (myGlobalHandle == NULL)
    {
    	EdGUI_Message1 (pmConsoleWindow, 0, IDS_COPY_FAILED_TITLE,
    	    IDS_COPY_FAILED_MESSAGE);
    	return;
    }
    myGlobalPtr = GlobalLock (myGlobalHandle);
    
    // Copy all the selection, turning LF to CR LF
    mySrc = myTextPtr -> text + min (myTextPtr -> selFrom, myTextPtr -> selTo);
    myEnd = myTextPtr -> text + max (myTextPtr -> selFrom, myTextPtr -> selTo);
    myDest = myGlobalPtr;
    while (mySrc < myEnd)
    {
    	if (*mySrc == '\n')
    	{
    	    *myDest++ = '\r';
    	}
    	*myDest++ = *mySrc++;
    } // while

    // Unlock the handle
    GlobalUnlock (myGlobalHandle);

    //
    // Copy the data into the clipboard    
    //
    OpenClipboard (pmConsoleWindow);
    EmptyClipboard ();
    SetClipboardData (CF_TEXT, myGlobalHandle);
    CloseClipboard ();
} // MyCopy


/************************************************************************/
/* MyDeleteText								*/
/************************************************************************/
static void		MyDeleteText (HWND pmConsoleWindow, int pmPos)
{
    ConsoleWindowInfoPtr	myInfo;
    TextPtr			myTextPtr;
    HWND			myConsoleTextWindow;
    int 			myOrigSelFrom, myOrigSelTo;
    int				myTextPos;
    BOOL			myAtEnd;
    int				myAction;
   
    myInfo = MyGetConsoleWindowInfo (pmConsoleWindow);
    myTextPtr = myInfo -> text;
    myConsoleTextWindow = myInfo -> consoleTextWindow;
    myTextPos = MyGetCurrentTextPos (myTextPtr);

    if ((myInfo -> topLine <= myTextPtr -> numLines) &&
    	(myTextPtr -> numLines < myInfo -> topLine + myInfo -> windowHeight))
    {
    	myAtEnd = TRUE;
    }
    else
    {
    	myAtEnd = FALSE;
    }        
    
    myOrigSelFrom = myTextPtr -> selFrom;
    myOrigSelTo = myTextPtr -> selTo;
    
    myTextPtr -> selFrom = pmPos;
    myTextPtr -> selTo = myTextPos;
    myTextPtr -> locationOK = FALSE;
    
    myAction = EdText_SelectionDelete (myTextPtr);
    
    if (myOrigSelFrom > myTextPtr -> textLen)
    {
	myTextPtr -> selFrom = myTextPtr -> textLen;
    }
    else
    {
	myTextPtr -> selFrom = myOrigSelFrom;
    }
    if (myOrigSelTo > myTextPtr -> textLen)
    {
	myTextPtr -> selTo = myTextPtr -> textLen;
    }
    else
    {
	myTextPtr -> selTo = myOrigSelTo;
    }
    myTextPtr -> locationOK = FALSE;
    
    if (myAction & (KEY_LONGEST_LINE_CHANGED | KEY_NUM_LINES_CHANGED))
    {
    	MySynchScrollBarsToText (myConsoleTextWindow);
    }

    if (myAction & KEY_REDRAW_FROM_SELECTION)
    {
        MyCaretDisplay (myConsoleTextWindow);
    	MyDisplayText (myConsoleTextWindow, FALSE, ALL_LINES, ALL_LINES);
    }
    else if (myAction & KEY_UPDATE_LINE)
    {
    	int	myLine, myCol;
    	
        MyCaretDisplay (myConsoleTextWindow);
        MyGetCurrentLineCol (myTextPtr, &myLine, &myCol);
    	MyDisplayText (myConsoleTextWindow, FALSE, myLine, myLine);
    }

    if (myAtEnd)
    {    
    	MyScrollToSelection (myConsoleTextWindow, SCROLL_NORMAL, FALSE, TRUE);
    }
} // MyDeleteText


/******************************************************************************/
/* MyDisplayText							      */
/* 									      */
/* The text should be displayed in the specified colours.  The special line   */
/* should display with normal coloured text and a light grey background.  The */
/* special token should display in dark grey with white text.  The selection  */
/* appears above all others and appears in dark blue with inverted text.      */
/******************************************************************************/
static void	MyDisplayText (HWND pmConsoleTextWindow, BOOL pmPaintUpdate, 
			       int pmFromLine, int pmToLine)
{
    HDC				myDisplayContext;
    RECT			myLineRect, mySectionRect;
    TextPtr			myTextPtr;
    HBRUSH			mySelectionBrush;
    HBRUSH			myBackgroundBrush;
    int				myWidth, myHeight;
    int				myNumVisibleLines;
    int				myLastVisibleLine;
    int				myFirstLine, myLastLine;
    int				myLineNumber;
    int				mySelFromLine, mySelFromCol;
    int				mySelToLine, mySelToCol;
    BOOL			myEraseAtEnd = FALSE;
    BOOL			myEraseLastLine = FALSE;
    ConsoleWindowInfoPtr	myInfo;
    BOOL			myWindowActive;
    HRGN			mySelectionRegion = NULL;
    PAINTSTRUCT			myPaintStruct;
    
    // Get the window info    
    myInfo = MyGetConsoleWindowInfo (pmConsoleTextWindow);
    
    // Create the appropriate brushes
    mySelectionBrush = CreateSolidBrush (stSelectionColour);
    myBackgroundBrush = (HBRUSH) GetStockObject (WHITE_BRUSH);
    
    // Determine if the selection is drawn or framed
    {
	HWND	myForegroundWindow, myConsoleWindow, myNextWindow;
	
    	myForegroundWindow = GetForegroundWindow ();
    	myConsoleWindow = EdGUI_GetTopWindow (pmConsoleTextWindow);
    	myNextWindow = EdGUI_GetTopWindow (
    			GetNextWindow (myForegroundWindow, GW_HWNDNEXT));

    	if (myForegroundWindow == myConsoleWindow)
    	{
    	    myWindowActive = TRUE;
        }
        else
        {
    	    myWindowActive = FALSE;
        }
    }

    // Calculate the 
    myWidth = myInfo -> textRect.right - myInfo -> textRect.left;
    myHeight = myInfo -> textRect.bottom - myInfo -> textRect.top;
    
    // Translate it appropriately for the offscreen bitmap
    myLineRect = myInfo -> textRect;
    myLineRect.bottom = myLineRect.top + stTextFontCharHeight;
    
    myNumVisibleLines = (myHeight + (stTextFontCharHeight - 1)) / 
    				stTextFontCharHeight;

    myTextPtr = myInfo -> text;
    
    if (pmFromLine == ALL_LINES)
    {
    	pmFromLine = 0;
    	pmToLine = myTextPtr -> numLines;
    	myEraseAtEnd = TRUE;
    }
    else if (pmFromLine == myTextPtr -> numLines)
    {
    	myEraseLastLine = TRUE;
    }
    
    myLastVisibleLine = min (myInfo -> topLine + myNumVisibleLines, 
    	myInfo -> text -> numLines);

    /***********************************************************************/    
    /* Draw a line of text in the correct colours.  The highlight line     */
    /* will be properly highlighted, but the hiliteToken and the selection */
    /* won't be drawn by this command.					   */
    /***********************************************************************/    
    myFirstLine = max (myInfo -> topLine, pmFromLine);
    myLastLine = min (myLastVisibleLine - 1, pmToLine);
    myLineRect.top += (myFirstLine - myInfo -> topLine) * stTextFontCharHeight;
    myLineRect.bottom += (myFirstLine - myInfo -> topLine) * 
        stTextFontCharHeight;
    
    // Get the selection's line and column
    EdText_GetSelectionLinesCols (myTextPtr, &mySelFromLine, &mySelFromCol, 
    	&mySelToLine, &mySelToCol);

    // Prepare for painting   
    if (pmPaintUpdate)
    {
        myDisplayContext = BeginPaint (pmConsoleTextWindow, &myPaintStruct);
    }        
    else
    {
        myDisplayContext = GetDC (pmConsoleTextWindow);
    }        
    HideCaret (pmConsoleTextWindow);

    // Set the font for the window    
    SelectFont (myDisplayContext, stNormalFont);
    
    //
    // Loops once for each line to be drawn
    //
    for (myLineNumber = myFirstLine ; myLineNumber <= myLastLine ; 
         myLineNumber++)
    {
    	int	myStatus, myStartPos, myEndPos;
    	char	*myLine;
    	int	myLength;
    	int	mySelStart, mySelEnd;

	mySectionRect = myLineRect;
	
	//
	// Get the beginning and ending parts of the selection on this line
	//
	if ((mySelFromLine > myLineNumber) || 
	    (myTextPtr -> selFrom == myTextPtr -> selTo))
	{
	    // Selection starts after current line
	    mySelStart = NO_SEL_ON_THIS_LINE;
	}
	else if (mySelFromLine == myLineNumber)
	{
	    // Selection starts on current line
	    mySelStart = mySelFromCol;
	}
	else if (mySelFromLine < myLineNumber) 
	{
	    if (myLineNumber <= mySelToLine)
	    {
	    	// Selection starts before current line and ends on or
	    	// after current line
	    	mySelStart = 0;
	    }
	    else
	    {
	    	// Selection starts before current line and ends before
	    	// current line
	    	mySelStart = NO_SEL_ON_THIS_LINE;
	    }
	}
	if (myTextPtr -> selFrom == myTextPtr -> selTo)
	{
	    // Selection starts after current line
	    mySelEnd = NO_SEL_ON_THIS_LINE;
	}
	else if (myLineNumber < mySelToLine)
	{
	    if (mySelFromLine <= myLineNumber)
	    {
	    	// Selection ends after current line and starts on or
	    	// before current line
	    	mySelEnd = TO_END_OF_LINE;
	    }
	    else
	    {
	    	// Selection ends after current line and ends after 
	    	// current line
	    	mySelEnd = NO_SEL_ON_THIS_LINE;
	    }
	}
	else if (mySelToLine == myLineNumber)
	{
	    // Selection ends on current line
	    mySelEnd = mySelToCol;
	}
	else if (mySelToLine < myLineNumber) 
	{
	    // Selection ends before current line
	    mySelEnd = NO_SEL_ON_THIS_LINE;
	}
	
    	myStartPos = myInfo -> leftCol;
    	
    	myLength = myTextPtr -> lineStarts [myLineNumber + 1] - 
    	    myTextPtr -> lineStarts [myLineNumber] - 1;
    	    
    	myLine = &myTextPtr -> text [
    	    myTextPtr -> lineStarts [myLineNumber] + myStartPos];
    	
    	while (myStartPos < myLength)
    	{
	    //
	    // Determine what to draw and for how long
	    //
	    
    	    if (myWindowActive && (mySelStart != NO_SEL_ON_THIS_LINE) &&
    	        ((mySelStart <= myStartPos) && (myStartPos < mySelEnd)))
    	    {
    	    	myStatus = IN_SELECTION;
    	    	myEndPos = min (mySelEnd, myLength);
    	    }
    	    else
    	    {
    	    	myStatus = IN_NORMAL;
    	    	myEndPos = TO_END_OF_LINE;
    	        if (myWindowActive && 
    	            (mySelStart != NO_SEL_ON_THIS_LINE) &&
    	            (myStartPos < mySelStart))
    	        {
    	            myEndPos = mySelStart;
    	        }
    	    	myEndPos = min (myEndPos, myLength);
    	    }
    	    
    	    if (myStatus == IN_NORMAL)
    	    {
		SetTextColor (myDisplayContext, COLOUR_BLACK);
		SetBkColor (myDisplayContext, COLOUR_WHITE);
	    }
	    else
	    {
		SetTextColor (myDisplayContext, COLOUR_WHITE);
		SetBkColor (myDisplayContext, stSelectionColour);
	    }
	    mySectionRect.right = mySectionRect.left + 
	        (myEndPos - myStartPos) * stTextFontCharWidth;
	    ExtTextOut (myDisplayContext, mySectionRect.left, 
	        mySectionRect.top, ETO_CLIPPED | ETO_OPAQUE, &mySectionRect, 
    	        myLine, (myEndPos - myStartPos), NULL);
    	    mySectionRect.left = mySectionRect.right;
    	    myLine += myEndPos - myStartPos;
	    SetTextColor (myDisplayContext, COLOUR_BLACK);
    	    
    	    myStartPos = myEndPos;
    	} // while

	//
	// Draw the rest of the line
	//
	mySectionRect.right = myInfo -> textRect.right;
    	if (myWindowActive && (mySelEnd == TO_END_OF_LINE))
    	{
    	    FillRect (myDisplayContext, &mySectionRect, mySelectionBrush);
    	}
    	else
    	{
    	    FillRect (myDisplayContext, &mySectionRect, myBackgroundBrush);
    	}

	myLineRect.top += stTextFontCharHeight;
	myLineRect.bottom += stTextFontCharHeight;
    } // for 
    
    DeleteBrush (mySelectionBrush);
	
    // If there's a non-active selection, draw the frame
    if (!myWindowActive && (myTextPtr -> selFrom != myTextPtr -> selTo))
    {
    	RECT	myRect;
    	
    	if (mySelFromLine == mySelToLine)
    	{
    	    myRect.left = (mySelFromCol - myInfo -> leftCol) * 
    	        stTextFontCharWidth + myInfo -> textRect.left;
    	    myRect.right = (mySelToCol - myInfo -> leftCol) * 
    	        stTextFontCharWidth + myInfo -> textRect.left;
    	    myRect.top = (mySelFromLine - myInfo -> topLine) *
    	        stTextFontCharHeight + myInfo -> textRect.top;
    	    myRect.bottom = myRect.top + stTextFontCharHeight;
    	    FrameRect (myDisplayContext, &myRect, stBlackBrush);
    	}
    	else
    	{
    	    HRGN	mySelectionRegion, myTempRegion;
    	    
    	    // The top part of the selection
    	    myRect.left = (mySelFromCol - myInfo -> leftCol) * 
    	        stTextFontCharWidth + myInfo -> textRect.left;
    	    myRect.right = myInfo -> textRect.right;
    	    myRect.top = (mySelFromLine - myInfo -> topLine) *
    	        stTextFontCharHeight + myInfo -> textRect.top;
    	    myRect.bottom = myRect.top + stTextFontCharHeight;
            mySelectionRegion = CreateRectRgnIndirect (&myRect);
	
	    // The middle section of the selection    	    
    	    myRect.left = myInfo -> textRect.left;
    	    myRect.right = myInfo -> textRect.right;
    	    myRect.top = ((mySelFromLine + 1) - myInfo -> topLine) *
    	        stTextFontCharHeight + myInfo -> textRect.top;
    	    myRect.bottom = (mySelToLine - myInfo -> topLine) *
    	        stTextFontCharHeight + myInfo -> textRect.top;
    	        
    	    myTempRegion = CreateRectRgnIndirect (&myRect);
    	    CombineRgn (mySelectionRegion, mySelectionRegion, myTempRegion, 
    	        RGN_OR);
    	   
    	    // The bottom line of the selection
    	    myRect.left = myInfo -> textRect.left;
    	    myRect.right = (mySelToCol - myInfo -> leftCol) * 
    	        stTextFontCharWidth + myInfo -> textRect.left;
    	    myRect.top = (mySelToLine - myInfo -> topLine) *
    	        stTextFontCharHeight + myInfo -> textRect.top;
    	    myRect.bottom = myRect.top + stTextFontCharHeight;

    	    myTempRegion = CreateRectRgnIndirect (&myRect);
    	    CombineRgn (mySelectionRegion, mySelectionRegion, myTempRegion, 
    	        RGN_OR);
	
    	    FrameRgn (myDisplayContext, mySelectionRegion, stBlackBrush, 1, 1);
    	} // if
    	
  	// Set the drawing colours back to normal
	SetTextColor (myDisplayContext, COLOUR_BLACK);
	SetBkColor (myDisplayContext, COLOUR_WHITE);
    } // if non-active selection
    
    if (myEraseAtEnd)
    {
 	// Clear the rest of the window (TW)
    	if (myLineRect.top < myHeight)
    	{
	    myLineRect.bottom = myHeight;
	    FillRect (myDisplayContext, &myLineRect, 
	    	(HBRUSH) GetStockObject (WHITE_BRUSH));
    	}
    }
    else if (myEraseLastLine)
    {
    	// Needed when we are the last character in a line at the end of
    	// the file
	FillRect (myDisplayContext, &myLineRect, 
	    (HBRUSH) GetStockObject (WHITE_BRUSH));
    }
    
    // Clear the margins
    {
    	RECT	myRect;
    	
    	myRect.left = 0;
    	myRect.right = myInfo -> textRect.right;
    	myRect.top = 0;
    	myRect.bottom = 2;
	FillRect (myDisplayContext, &myRect, 
	    (HBRUSH) GetStockObject (WHITE_BRUSH));
	    
    	myRect.left = 0;
    	myRect.right = 2;
    	myRect.top = 0;
    	myRect.bottom = myInfo -> textRect.bottom;
	FillRect (myDisplayContext, &myRect, 
	    (HBRUSH) GetStockObject (WHITE_BRUSH));
    }
    	    
    	
    ShowCaret (pmConsoleTextWindow);
    if (pmPaintUpdate)
    {
    	EndPaint (pmConsoleTextWindow, &myPaintStruct);
    }
    else
    {
    	ReleaseDC (pmConsoleTextWindow, myDisplayContext);
    }
    
    myInfo -> oldTopLine = myInfo -> topLine;
    myInfo -> oldLeftCol = myInfo -> leftCol;
} // MyDisplayText


/****************************************************************/
/* MyFileSave							*/
/*								*/
/* If pmSaveAs is false, save the file with the old name unless	*/
/* it hasn't got one.  If pmSaveAs is true, then save the file	*/
/* with a name specified in the Save file dialog.  Returns TRUE */
/* if the file was successfully written.			*/
/****************************************************************/
static void	MyFileSave (HWND pmTextDisplayWindow)
{
    BOOL			myNewPathName;
    FilePath			myPathName;
    ConsoleWindowInfoPtr	myInfo;
    
    // Get the window information
    myInfo = MyGetConsoleWindowInfo (pmTextDisplayWindow);
    
    myNewPathName = FALSE;
    
    strcpy (myPathName, myInfo -> className);
    strcat (myPathName, " Output");

    if (!EdGUI_GetSaveFile (pmTextDisplayWindow, myPathName, FALSE))
    {
        return;
    }
        
    // Save the text to the file.  If it fails, return immediately
    MySaveTextToFile (myPathName, myInfo -> text);
} // MyFileSave


/************************************************************************/
/* MyGetConsoleTextWindow						*/
/************************************************************************/
static HWND	MyGetConsoleTextWindow (HWND pmConsoleWindow)
{
    ConsoleWindowInfoPtr	myInfo;
    
    // Get the window information
    myInfo = MyGetConsoleWindowInfo (pmConsoleWindow);
    
    return myInfo -> consoleTextWindow;
} // MyGetConsoleTextWindow


/************************************************************************/
/* MyGetConsoleWindow							*/
/************************************************************************/
static HWND	MyGetConsoleWindow (HWND pmWindow)
{
    HWND		myWindow;
    int			myWindowType;
    
    myWindow = EdGUI_GetTopWindow (pmWindow);
    
    myWindowType = GetWindowLong (myWindow, WINDOW_MEM_WINDOW_TYPE);
    if (myWindowType != WINDOW_TYPE_CONSOLE)
    {
    	EdFail_Fatal (IDS_BADWINDOWTYPE, __FILE__, __LINE__, 0,
    		      myWindowType, WINDOW_TYPE_CONSOLE);
    }

    return myWindow;    	
} // MyGetConsoleWindow


/************************************************************************/
/* MyGetConsoleWindowInfo						*/
/************************************************************************/
static ConsoleWindowInfoPtr	MyGetConsoleWindowInfo (HWND pmWindow)
{
    int				myWindowType;
    ConsoleWindowInfoPtr	myInfo;
    
    myWindowType = GetWindowLong (pmWindow, WINDOW_MEM_WINDOW_TYPE);

    if (myWindowType == WINDOW_TYPE_CONSOLE_TEXT)
    {
    	pmWindow = GetParent (pmWindow);
    	myWindowType = GetWindowLong (pmWindow, WINDOW_MEM_WINDOW_TYPE);
    }
    
    if (myWindowType != WINDOW_TYPE_CONSOLE)
    {
    	EdFail_Fatal (IDS_BADWINDOWTYPE, __FILE__, __LINE__, 0,
    		      myWindowType, WINDOW_TYPE_CONSOLE);
    }
    	
    myInfo = (ConsoleWindowInfoPtr) GetWindowLong (pmWindow, 
        					   WINDOW_MEM_WINDOW_INFO);
    if (myInfo == NULL)
    {
    	EdFail_Fatal (IDS_GETWINDOWLONGFAIL, __FILE__, __LINE__,
    		      GetLastError (), WINDOW_MEM_WINDOW_INFO);
    }
       
    return myInfo;
} // MyGetConsoleWindowInfo


/************************************************************************/
/* MyGetCurrentLineCol							*/
/************************************************************************/
static void	MyGetCurrentLineCol (TextPtr pmTextPtr, int *pmLine, int *pmCol)
{
    if (pmTextPtr -> newLineAdded)
    {
	*pmLine = pmTextPtr -> numLines - 1;
	*pmCol = pmTextPtr -> lineStarts [*pmLine + 1] -
	    	pmTextPtr -> lineStarts [*pmLine] - 1;
    }
    else
    {
	*pmLine = pmTextPtr -> numLines;
	*pmCol = 0;
    }
} // MyGetCurrentLineCol


/************************************************************************/
/* MyGetCurrentTextPos							*/
/************************************************************************/
static int	MyGetCurrentTextPos (TextPtr pmTextPtr)
{
    if (pmTextPtr -> newLineAdded)
    {
    	return pmTextPtr -> textLen - 1;
    }
    else
    {
    	return pmTextPtr -> textLen;
    }
} // MyGetCurrentTextPos


/************************************************************************/
/* MyGetTextPtr								*/
/************************************************************************/
static TextPtr	MyGetTextPtr (HWND pmConsoleTextWindow)
{
    return MyGetConsoleWindowInfo (pmConsoleTextWindow) -> text;
} // MyGetTextPtr


/************************************************************************/
/* MyGetTextWindowSize							*/
/************************************************************************/
static void	MyGetTextWindowSize (HWND pmTextDisplayWindow, int pmNumRows, 
				     int pmNumCols, long *pmPixelWidth, 
				     long *pmPixelHeight)
{
    *pmPixelWidth = stTextFontCharWidth * pmNumCols + MARGIN +
        GetSystemMetrics (SM_CXVSCROLL);
    *pmPixelHeight = stTextFontCharHeight * pmNumRows + MARGIN +
        GetSystemMetrics (SM_CYHSCROLL);
} // MyGetTextWindowSize


/************************************************************************/
/* MyGetWindowDisplacement						*/
/************************************************************************/
static void	MyGetWindowDisplacement (BOOL pmReset, long *pmWindowWidth, 
					 long *pmWindowHeight, long *pmLeft, 
					 long *pmTop)
{
    RECT	myScreenRect;
    static BOOL	myUsePreviousLocation = FALSE;

    if (pmReset)
    {
    	myUsePreviousLocation = FALSE;
    	return;
    }
    
    // Calculate the top, left corner of the window
    myScreenRect = EdGUI_GetWorkArea (); 		   	

    if (myUsePreviousLocation)
    {
    	// TW Use the previous location...
    	return;
    }
    
    // Make certain Console fits on the screen
    if (myScreenRect.left + *pmWindowWidth > myScreenRect.right)
    {
	*pmWindowWidth = myScreenRect.right - myScreenRect.left;
    }
    
    if (myScreenRect.top + *pmWindowHeight > myScreenRect.bottom)
    {
	*pmWindowHeight = myScreenRect.bottom - myScreenRect.top;
    }
    
    // Place console window in upper right corner
    *pmLeft = myScreenRect.right - *pmWindowWidth;
    *pmTop = 0;
} // MyGetWindowDisplacement


/************************************************************************/
/* MyGetWindowSize							*/
/************************************************************************/
static void	MyGetWindowSize (HWND pmConsoleWindow, int pmRows, int pmCols, 
				 long *pmWidth, long *pmHeight)
{
    RECT			myRect;
    ConsoleWindowInfoPtr	myInfo;

    myInfo = MyGetConsoleWindowInfo (pmConsoleWindow);
    
    myRect.left = 0;
    myRect.top = 0; 	
    MyGetTextWindowSize (MyGetConsoleTextWindow (pmConsoleWindow),
 	pmRows, pmCols, &(myRect.right), &(myRect.bottom));

    myRect.bottom += JSTop_GetWindowHeight ();
    AdjustWindowRect (&myRect, WS_OVERLAPPEDWINDOW, FALSE);
    
    *pmWidth = myRect.right - myRect.left;
    *pmHeight = myRect.bottom - myRect.top;
} // MyGetWindowSize

				 
/************************************************************************/
/* MyKeystrokeHandler							*/
/************************************************************************/
static void	MyKeystrokeHandler (HWND pmConsoleTextWindow, int pmKeystroke)
{
    ConsoleWindowInfoPtr	myInfo;
    
    // Get the window info    
    myInfo = MyGetConsoleWindowInfo (pmConsoleTextWindow);

    // Beep if input is coming from a file, the user has already
    // entered EOF, te program has finished running 
    // or the keyboard buffer is full
    if (myInfo -> inputFromFile || myInfo -> eofAlreadyEntered ||
        (myInfo -> runStatus == STATUS_FINISHED) ||
        (INC (myInfo -> kbdBufferHead) == myInfo -> kbdBufferTail))
    {
    	MessageBeep (MB_ICONEXCLAMATION);
    	return;
    }

    // Add to the keyboard buffer
    myInfo -> kbdBuffer [myInfo -> kbdBufferHead] = pmKeystroke;
    myInfo -> kbdBufferHead = INC (myInfo -> kbdBufferHead);
    
    // If the user entered Ctrl+Z, EOF has been sent
    if (pmKeystroke == CTRL_Z)
    {
    	myInfo -> eofAlreadyEntered = TRUE;
    }
    
    // If we're currently accepting input, process it immediately
    if ((myInfo -> acceptingInput) && (myInfo -> runStatus != STATUS_PAUSED))
    {
    	MyProcessFromKeyboardToLineBuffer (pmConsoleTextWindow);
    }
} // MyKeystrokeHandler

				    
/************************************************************************/
/* MyMouseDownHandler							*/
/************************************************************************/
static BOOL	MyMouseDownHandler (HWND pmConsoleTextWindow, int pmX, int pmY,
    				    BOOL pmShift, BOOL pmAllowOutOfBounds)
{
    ConsoleWindowInfoPtr	myInfo;
    RECT			myRect;
    
    myInfo = MyGetConsoleWindowInfo (pmConsoleTextWindow);
    myRect = myInfo -> textRect;
    
    // Adjust the mouse position so that if it is in the margins, we move it
    // into the text rectangle
    if ((0 <= pmX) && (pmX < myRect.left))
    {
    	pmX = myRect.left;
    }
    if ((0 <= pmY) && (pmY < myRect.top))
    {
    	pmY = myRect.top;
    }
    
    if (MyMouseInTextRect (pmConsoleTextWindow, pmX, pmY) || pmAllowOutOfBounds)
    {
    	int 	myLine;
    	int 	myCol;
    	BOOL	myRedraw;

	pmX = min (max (pmX, myRect.left), myRect.right);
	pmY = min (max (pmY, myRect.top), myRect.bottom);
    	myLine = (pmY - myRect.top) / stTextFontCharHeight;
    	myCol = (pmX - myRect.left + (stTextFontCharWidth / 2)) /
    	    stTextFontCharWidth;
	myCol = min (myCol, myInfo -> windowWidth - 1);
	myLine = min (myLine, myInfo -> windowHeight - 1);
	
	// Check to see if we clicked in the same place as last time
	if ((myInfo -> lastMouseCol == myCol) && 
	    (myInfo -> lastMouseLine == myLine))
	{
	    myInfo -> clickOnSameChar = TRUE;
	}
	else
	{
	    myInfo -> clickOnSameChar = FALSE;
	}
	myInfo -> lastMouseCol = myCol;
	myInfo -> lastMouseLine = myLine;
	
	if (pmShift)
	{
	    int	myStatus;
	    
	    myStatus = EdText_SelectExtendSelection (myInfo -> text, 
    	        myLine + myInfo -> topLine, myCol + myInfo -> leftCol);
      	    
    	    if (myStatus == HIDE_SELECTION)
    	    {
    	        MyCaretDisplay (pmConsoleTextWindow);
    	        MyCaretMove (pmConsoleTextWindow);
    	    }
    	    else if (myStatus == CREATE_SELECTION)
    	    {
    	    	MyCaretHide (pmConsoleTextWindow);
    	    }
    	    MyScrollToSelection (pmConsoleTextWindow, SCROLL_NORMAL, 
    	        myStatus != NO_REDRAW, FALSE);
	}
	else
	{
     	    if (EdText_SetSelection (myInfo -> text, 
    	        myLine + myInfo -> topLine, myCol + myInfo -> leftCol))
      	    {
   	        myRedraw = TRUE;
    	        MyCaretDisplay (pmConsoleTextWindow);
      	    }
    	    MyCaretMove (pmConsoleTextWindow);
    	    MyScrollToSelection (pmConsoleTextWindow, SCROLL_NORMAL, myRedraw,
    	    			 FALSE);
        }
        
    	return TRUE;
    }
    else
    {
    	return FALSE;
    }
} // MyMouseDownHandler


/************************************************************************/
/* MyMouseExtendSelectionToLine						*/
/************************************************************************/
static void	MyMouseExtendSelectionToLine (HWND pmConsoleTextWindow)
{
    int	myStatus;
	    
    myStatus = EdText_SelectLine (MyGetTextPtr (pmConsoleTextWindow));
      	    
    if (myStatus == HIDE_SELECTION)
    {
        MyCaretDisplay (pmConsoleTextWindow);
        MyCaretMove (pmConsoleTextWindow);
    }
    else if (myStatus == CREATE_SELECTION)
    {
    	MyCaretHide (pmConsoleTextWindow);
    }
    MyScrollToSelection (pmConsoleTextWindow, SCROLL_NORMAL, 
        myStatus != NO_REDRAW, FALSE);
} // MyMouseExtendSelectionToLine


/************************************************************************/
/* MyMouseExtendSelectionToWord						*/
/************************************************************************/
static void	MyMouseExtendSelectionToWord (HWND pmConsoleTextWindow)
{
    int	myStatus;
	    
    myStatus = EdText_SelectWord (MyGetTextPtr (pmConsoleTextWindow));
      	    
    if (myStatus == HIDE_SELECTION)
    {
        MyCaretDisplay (pmConsoleTextWindow);
        MyCaretMove (pmConsoleTextWindow);
    }
    else if (myStatus == CREATE_SELECTION)
    {
    	MyCaretHide (pmConsoleTextWindow);
    }
    MyScrollToSelection (pmConsoleTextWindow, SCROLL_NORMAL, 
        myStatus != NO_REDRAW, FALSE);
} // MyMouseExtendSelectionToWord


/************************************************************************/
/* MyMouseInTextRect							*/
/************************************************************************/
static BOOL	MyMouseInTextRect (HWND pmConsoleTextWindow, int pmX, int pmY)
{
    RECT	myRect;
    
    myRect = MyGetConsoleWindowInfo (pmConsoleTextWindow) -> textRect;
    
    return ((myRect.left <= pmX) && (pmX <= myRect.right) &&
        (myRect.top <= pmY) && (pmY <= myRect.bottom));
} // MyMouseInTextRect


/************************************************************************/
/* MyPaste								*/
/************************************************************************/
static void	MyPaste (HWND pmConsoleWindow)
{
    ConsoleWindowInfoPtr	myInfo;
    HGLOBAL			myGlobalHandle;
    char			*myGlobalPtr;
    char			*myBuffer;
    int				myBufferLen;
    int				cnt;
            
    // Get the window info    
    myInfo = MyGetConsoleWindowInfo (pmConsoleWindow);

    // Beep if input is coming from a file, the user has already
    // entered EOF, te program has finished running 
    // or the keyboard buffer is full
    if (myInfo -> inputFromFile || myInfo -> eofAlreadyEntered ||
        (myInfo -> runStatus == STATUS_FINISHED) ||
        (INC (myInfo -> kbdBufferHead) == myInfo -> kbdBufferTail))
    {
    	MessageBeep (MB_ICONEXCLAMATION);
    	return;
    }

    //
    // Copy the data from the clipboard into a buffer
    //
    OpenClipboard (pmConsoleWindow);
    myGlobalHandle = GetClipboardData (CF_TEXT);
    if (myGlobalHandle == NULL)
    {
    	MessageBeep (MB_ICONEXCLAMATION);
    	CloseClipboard ();
    	return;
    }
    myBufferLen = GlobalSize (myGlobalHandle);
    myBuffer = malloc (myBufferLen);
    myGlobalPtr = GlobalLock (myGlobalHandle);
    strcpy (myBuffer, myGlobalPtr);
    GlobalUnlock (myGlobalHandle);
    CloseClipboard ();

    // Add to the keyboard buffer
    for (cnt = 0 ; cnt < myBufferLen ; cnt++)
    {
    	if (myInfo -> eofAlreadyEntered ||
	    (INC (myInfo -> kbdBufferHead) == myInfo -> kbdBufferTail))
    	{
    	    MessageBeep (MB_ICONEXCLAMATION);
    	    break;
    	}
    	
    	// Convert '\r' or '\r\n' to '\n'
    	if (myBuffer [cnt] == '\r')
    	{
    	    myInfo -> kbdBuffer [myInfo -> kbdBufferHead] = RETURN;
    	    if ((cnt < myBufferLen - 1) &&
    	        (myBuffer [cnt + 1] == '\n'))
    	    {
    		cnt++;
    	    }
    	}
    	else if (myBuffer [cnt] == '\n')
    	{
    	    myInfo -> kbdBuffer [myInfo -> kbdBufferHead] = RETURN;
    	}
    	else
    	{
    	    myInfo -> kbdBuffer [myInfo -> kbdBufferHead] = myBuffer [cnt];
    	}
    	myInfo -> kbdBufferHead = INC (myInfo -> kbdBufferHead);
    	
    	// If the user entered Ctrl+Z, EOF has been sent
    	if (myBuffer [cnt] == CTRL_Z)
    	{
    	    myInfo -> eofAlreadyEntered = TRUE;
    	}
    }
    
    free (myBuffer);
    
    // If we're currently accepting input, process it immediately
    if ((myInfo -> acceptingInput) && (myInfo -> runStatus != STATUS_PAUSED))
    {
   	MyProcessFromKeyboardToLineBuffer (pmConsoleWindow);
    }
} // MyPaste

				    
/************************************************************************/
/* MyProcessFromKeyboardToLineBuffer					*/
/************************************************************************/
static void	    MyProcessFromKeyboardToLineBuffer (HWND pmWindow)
{
    ConsoleWindowInfoPtr	myInfo;
    char			myChar;
    BOOL			myFoundNewLine;
    char			myBuffer [2];
    	    	
    // Get the window info    
    myInfo = MyGetConsoleWindowInfo (pmWindow);

    // Leave immediately if already have our newline
    if (myInfo -> lineBufferNewLineWaiting)
    {
    	LeaveCriticalSection (&stLineBufferCriticalSection);
    	return;
    }
    
    // Return an empty string indicating an EOF if we've already entered
    // Note this is only true when called from JSCon_GetLine because once
    // and eof is on deck, the message loop never processes any more
    // keystrokes.
    if ((myInfo -> lineBufferSize == 0) && (myInfo -> eofOnDeck))
    {
    	if (myInfo -> eofOnDeck && !myInfo -> eofEchoed)
    	{
	    myInfo -> eofEchoed = TRUE;
    	    SendMessage (MyGetConsoleWindow (pmWindow), WM_ADD_TEXT, 
	    	(WPARAM) "^Z\n", (LPARAM) BEGINNING_OF_LINE);
        }
    
    	myInfo -> lineBufferNewLineWaiting = TRUE;
	myInfo -> acceptingInput = FALSE;
	SetEvent (stNewLineReceivedEvent);
    	LeaveCriticalSection (&stLineBufferCriticalSection);
    	return;
    }
    	
    myBuffer [1] = 0;
    
    myFoundNewLine = FALSE;

    while (myInfo -> kbdBufferHead != myInfo -> kbdBufferTail)
    {
    	myChar = myInfo -> kbdBuffer [myInfo -> kbdBufferTail];
    	myInfo -> kbdBufferTail = INC (myInfo -> kbdBufferTail);
    	
    	// Handle newline
    	if (myChar == RETURN)
    	{
    	    myFoundNewLine = TRUE;
    	    break;
    	}

	// Handle Ctrl+Z    	
    	if (myChar == CTRL_Z)
    	{
    	    myInfo -> eofOnDeck = TRUE;
    	    
    	    // If we're not at the beginning of a new line, then add a new line
	    if (myInfo -> lineBufferSize != 0)
	    {	    				
    	    	myFoundNewLine = TRUE;
    	    	break;
    	    }

	    // Activate the end of file right now.
	    myInfo -> eofEchoed = TRUE;
    	    SendMessage (MyGetConsoleWindow (pmWindow), WM_ADD_TEXT, 
	    	(WPARAM) "^Z\n", (LPARAM) BEGINNING_OF_LINE);
	    	
	    myInfo -> lineBufferNewLineWaiting = TRUE;	
	    myInfo -> acceptingInput = FALSE;
	    SetEvent (stNewLineReceivedEvent);
    	    LeaveCriticalSection (&stLineBufferCriticalSection);
    	    return;
	}
	
	// Handle Backspace    	
    	if (myChar == BACKSPACE)
    	{
	    if (myInfo -> lineBufferSize == 0)
	    {
	    	// If there's no characters to be backspaces over, then beep
	    	MessageBeep (MB_ICONEXCLAMATION);
	    }
	    else
	    {
	    	char	myCharToErase;
	    	int	cnt;
	    	
	    	myInfo -> lineBufferSize--;
	    	myCharToErase = myInfo -> lineBuffer [myInfo -> lineBufferSize];
	    	
	    	// If we delete a non-tab, see if the previous char is part
	    	// of the input and if it is not, repeat the line buffer on
	    	// a new line.  If it is a tab, then erase the entire line if
	    	// possible and redraw it.  If not, then erase the part 
	    	// available and redraw the line on a new line
	    	//
	    	// Examples:  Uppercase output, lowercase keystrokes
	    	//    ABCabc	       => ABCab^
	    	//    abcABCdefDEF     => abcABCdefDEF
	    	//                        abcde^
	    	//    abcABCdef        => abcABCde^
	    	//    ABCab\t	       -> ABCab^
	    	//    abcABCdefDEFgh\t => abcABCdefDEF
	    	//		          abcdefgh^
	    	
	    	if (myCharToErase != TAB)
	    	{
	    	    int	myCurrentPos;
	    	    
	    	    myCurrentPos = MyGetCurrentTextPos (myInfo -> text);
	    	    if (myInfo -> textEndPosOfLastOutput == myCurrentPos)
		    {
		 	char	myBuffer [2];

			myBuffer [1] = 0;
			
		    	// The previous character is from output, redraw
		    	// the rest of the line (on a new line if necessary)
		    	if ((myCurrentPos != 0) && 
 		    	    (myInfo -> text -> text [myCurrentPos - 1] != '\n'))
		 	{
		 	    // Add a newline
			    myBuffer [0] = '\n';
		    	    SendMessage (MyGetConsoleWindow (pmWindow), 
		    	    	WM_ADD_TEXT, (WPARAM) myBuffer, 
		    	    	(LPARAM) NOT_AT_BEGINNING_OF_LINE);
		 	}
		 	
		 	// Add the rest of the line buffer
		 	for (cnt = 0 ; cnt < myInfo -> lineBufferSize ; cnt++)
		 	{
		 	    myBuffer [0] = myInfo -> lineBuffer [cnt],
		    	    SendMessage (MyGetConsoleWindow (pmWindow), 
		    	        WM_ADD_TEXT, (WPARAM) myBuffer, 
		    	    	(LPARAM) NOT_AT_BEGINNING_OF_LINE);
			}
		    }
		    else
		    {
		    	// Remove the last character
		    	SendMessage (MyGetConsoleWindow (pmWindow), 
		    	    WM_DELETE_TEXT, (WPARAM) myCurrentPos - 1, 
		    	    (LPARAM) NULL);
		    }	    	    			
		} // if (myCharToErase != TAB)
		else
		{
		    // We want to delete all the input.  However, if there's
		    // been output since the beginning of the input line,
		    // then only delete the up to the end of the last
		    // output.  (That way we never delete any output.)
		    if (myInfo -> textEndPosOfLastOutput < 
		     			myInfo -> textStartPosOfLastInput)
		    {
		    	// Erase from myInfo -> textStartPosOfLastInput
		    	SendMessage (MyGetConsoleWindow (pmWindow), 
		    	    WM_DELETE_TEXT, 
		    	    (WPARAM) myInfo -> textStartPosOfLastInput, 
		    	    (LPARAM) NULL);
		    }
		    else
		    {
		    	// Erase from myInfo -> textEndPosOfLastOutput
		    	SendMessage (MyGetConsoleWindow (pmWindow), 
		    	    WM_DELETE_TEXT, 
		    	    (WPARAM) myInfo -> textEndPosOfLastOutput, 
		    	    (LPARAM) NULL);
		    	    
		    	if ((myInfo -> textEndPosOfLastOutput != 0) && 
 		    	    (myInfo -> text -> text [myInfo -> textEndPosOfLastOutput - 1] != '\n'))
		 	{
		 	    // Add a newline
			    myBuffer [0] = '\n';
		    	    SendMessage (MyGetConsoleWindow (pmWindow), 
		    	    	WM_ADD_TEXT, (WPARAM) myBuffer, 
		    	    	(LPARAM) NOT_AT_BEGINNING_OF_LINE);
		 	}
		    }
		    // Add the rest of the line buffer
		    for (cnt = 0 ; cnt < myInfo -> lineBufferSize ; cnt++)
		    {
		 	 myBuffer [0] = myInfo -> lineBuffer [cnt],
		    	 SendMessage (MyGetConsoleWindow (pmWindow), 
		    	     WM_ADD_TEXT, (WPARAM) myBuffer, 
		    	     (LPARAM) NOT_AT_BEGINNING_OF_LINE);
		    }
		} // else (myCharToErase == TAB)
	    } // else char available to erase
    	} // if (myChar == BACKSPACE)
    	else if (myChar == CTRL_U)
    	{
	    if (myInfo -> lineBufferSize == 0)
	    {
	    	// If there's no characters to be backspaces over, then beep
	    	MessageBeep (MB_ICONEXCLAMATION);
	    }
	    else
	    {
	    	// Wipe out text in the buffer
	    	myInfo -> lineBufferSize = 0;
	    				
		// Erase from the beginning of the input, or the end of
		// the last output	    				
		if (myInfo -> textEndPosOfLastOutput < 
		     			myInfo -> textStartPosOfLastInput)
		{
		    // Erase from myInfo -> textStartPosOfLastInput
		    SendMessage (MyGetConsoleWindow (pmWindow), 
		    	WM_DELETE_TEXT, 
		    	(WPARAM) myInfo -> textStartPosOfLastInput, 
		    	(LPARAM) NULL);
		}
		else
		{
		    // Erase from myInfo -> textEndPosOfLastOutput
		    SendMessage (MyGetConsoleWindow (pmWindow), 
		    	WM_DELETE_TEXT, 
		    	(WPARAM) myInfo -> textEndPosOfLastOutput, 
		    	(LPARAM) NULL);
		    	    
		    if ((myInfo -> textEndPosOfLastOutput != 0) && 
 		    	(myInfo -> text -> text [myInfo -> textEndPosOfLastOutput - 1] != '\n'))
		    {
		 	// Add a newline
			myBuffer [0] = '\n';
		    	SendMessage (MyGetConsoleWindow (pmWindow), 
		    	    WM_ADD_TEXT, (WPARAM) myBuffer, 
		    	    (LPARAM) NOT_AT_BEGINNING_OF_LINE);
		    }
		}
	    } // else char available to erase
	} // if CTRL_U
	else
	{
	    // This is a real character, enter it onto the queue
	    // TW Actually, check for ASCII char
	    
	    // Always make certain there's enough space in the
	    // queue for a newline
	    if (myInfo -> lineBufferSize < INPUT_BUFFER_SIZE - 3)
	    {
	    	myInfo -> lineBuffer [myInfo -> lineBufferSize] = myChar;
	    	myInfo -> lineBufferSize++;
	    	
	    	// Echo the character onto the screen
	    	myBuffer [0] = myChar;
	    	if (myInfo -> lineBufferSize == 0)
	    	{
	    	    SendMessage (MyGetConsoleWindow (pmWindow), 
	    	        WM_ADD_TEXT, (WPARAM) myBuffer, 
	    	        (LPARAM) BEGINNING_OF_LINE);
		}
		else
		{
	    	    SendMessage (MyGetConsoleWindow (pmWindow), 
	    	        WM_ADD_TEXT, (WPARAM) myBuffer, 
	    	        (LPARAM) NOT_AT_BEGINNING_OF_LINE);
		}		    	
	    }
	    else
	    {
	    	MessageBeep (MB_ICONEXCLAMATION);
	    }
	} // else regular char
    } // while
    
    if (myFoundNewLine)
    {
	myBuffer [0] = '\n';
    	SendMessage (MyGetConsoleWindow (pmWindow), WM_ADD_TEXT, 
	    (WPARAM) myBuffer, (LPARAM) BEGINNING_OF_LINE);

	myInfo -> lineBuffer [myInfo -> lineBufferSize] = '\n';
	myInfo -> lineBufferSize++;
	myInfo -> lineBufferNewLineWaiting = TRUE;	

	myInfo -> acceptingInput = FALSE;
		
	SetEvent (stNewLineReceivedEvent);
    }
    
    LeaveCriticalSection (&stLineBufferCriticalSection);
} // MyProcessFromKeyboardToLineBuffer


/************************************************************************/
/* MyResizeConsoleTextWindow						*/
/************************************************************************/
static void	MyResizeConsoleTextWindow (HWND pmConsoleTextWindow)
{
    RECT			myWindowRect;
    ConsoleWindowInfoPtr	myInfo;

    // The first resizing comes before there's a client area, so
    // we put this check here to ignore it
    if (GetClientRect (pmConsoleTextWindow, &myWindowRect) == 0)
    {
    	return;
    }
    
    // If the window hasn't really been resized, return immediately 
    if ((myWindowRect.bottom == 0) && (myWindowRect.right == 0) &&
        (myWindowRect.top == 0) && (myWindowRect.left == 0))
    {
    	return;
    }
    
    // Get the window information
    myInfo = MyGetConsoleWindowInfo (pmConsoleTextWindow);
    
    // Set the text rectangle
    myInfo -> textRect = myWindowRect;
    myInfo -> textRect.top += MARGIN;
    myInfo -> textRect.left += MARGIN;    

    // Set the window width and height        
    myInfo -> windowWidth = (myInfo -> textRect.right - 
        myInfo -> textRect.left) / stTextFontCharWidth;
    myInfo -> windowHeight = (myInfo -> textRect.bottom - 
        myInfo -> textRect.top) / stTextFontCharHeight;
    
    MySynchScrollBarsToText (pmConsoleTextWindow);
    
    MyCaretDisplay (pmConsoleTextWindow);
} // MyResizeConsoleTextWindow


/************************************************************************/
/* MyResizeConsoleWindow							*/
/************************************************************************/
static void	MyResizeConsoleWindow (HWND pmConsoleWindow)
{
    RECT			myWindowRect;
    ConsoleWindowInfoPtr	myInfo;
    int				myWindowWidth;
    int				myTopFrameBorder;
    HDWP			myDeferWindowPosHandle;

    // The first resizing comes before there's a client area, so
    // we put this check here to ignore it
    if (GetClientRect (pmConsoleWindow, &myWindowRect) == 0)
    {
    	return;
    }
    
    // Get the window infor    
    myInfo = MyGetConsoleWindowInfo (pmConsoleWindow);
    
    // If the window hasn't really been resized, return immediately 
    if ((myWindowRect.bottom == myInfo -> oldWindowRect.bottom) && 
        (myWindowRect.right == myInfo -> oldWindowRect.right) &&
        (myWindowRect.top == myInfo -> oldWindowRect.top) &&
        (myWindowRect.left == myInfo -> oldWindowRect.left))
    {
    	return;
    }
    
    // If we've been iconisized, ignore the resizing
    if (IsIconic (pmConsoleWindow))
    {
    	return;
    }

    // Save the old window size    
    myInfo -> oldWindowRect = myWindowRect;
    
    myWindowWidth = myWindowRect.right - myWindowRect.left;
    
    //
    // Resize the child windows
    //
    myDeferWindowPosHandle = BeginDeferWindowPos (5);
    if (myDeferWindowPosHandle == NULL)
    {
        EdFail_Fatal (IDS_NO_MEM_FOR_RESIZE, __FILE__, __LINE__, 
            GetLastError ());
    }

    //
    // Resize the button bar
    //
    myTopFrameBorder = JSTop_GetWindowHeight ();
    DeferWindowPos (myDeferWindowPosHandle, myInfo -> buttonBarWindow, HWND_TOP, 
        0, 0, myWindowRect.right, myTopFrameBorder, SWP_NOACTIVATE);

    //
    // Resize the text display window
    //
    DeferWindowPos (myDeferWindowPosHandle, 
    	myInfo -> consoleTextWindow,  HWND_TOP, 0, myTopFrameBorder, 
    	myWindowRect.right, myWindowRect.bottom - myTopFrameBorder, 
    	SWP_NOACTIVATE);
        
    if (!EndDeferWindowPos (myDeferWindowPosHandle))
    {
        EdFail_Fatal (IDS_NO_RESIZE, __FILE__, __LINE__, GetLastError ());
    }
} // MyResizeConsoleWindow


/************************************************************************/
/* MySaveTextToFile							*/
/************************************************************************/
static BOOL	MySaveTextToFile (char *pmPathName, TextPtr pmText)
{
//    ErrorBuffer	myErrorBuffer;
    HANDLE	myFile;
    int		myPos, myEOLPos;
    DWORD	myNumBytesWritten;
    
    // Open the file
    myFile = CreateFile (pmPathName, GENERIC_WRITE, 0, NULL,
    	CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (myFile == (HANDLE) INVALID_HANDLE_VALUE)
    {
/*    	EdGUI_Message (
    	    "File Open Error",
    	    "Unable to open file \"%s\".\n\n%s\n[Error code: %d]",
    	    pmPathName, EdFail_GetErrorMessage (myErrorBuffer), 
    	    GetLastError ());
*/
	return FALSE;
    }

    // Write the file
    myPos = 0;
    while (myPos < pmText -> textLen)
    {
	// Write out the rest of the line
	myEOLPos = myPos;
    	while ((myEOLPos < pmText -> textLen) && 
    	       (pmText -> text [myEOLPos] != '\n'))
    	{
    	    myEOLPos++;
    	}
    	if (myEOLPos != myPos)
    	{
    	    // Write out the rest of the line
    	    if (!WriteFile (myFile, &pmText -> text [myPos], myEOLPos - myPos,
    	        &myNumBytesWritten, NULL))
    	    {
    	    	// TW
    	    }
    	    if (myNumBytesWritten != (DWORD) (myEOLPos - myPos))
    	    {
    	    	// TW
	    }
    	    myPos = myEOLPos;
	}
	
	// Write out the newlines
    	while ((myPos < pmText -> textLen) && (pmText -> text [myPos] == '\n'))
    	{
	    myPos++;
    	    if (!WriteFile (myFile, NEW_LINE, strlen (NEW_LINE), 
    	        	    &myNumBytesWritten, NULL))
    	    {
    	    	// TW
    	    }
    	    if (myNumBytesWritten != strlen (NEW_LINE))
    	    {
    	    	// TW
	    }
	}
    } // while (myPos < pmText -> textLen)

    // Write the file    
    if (CloseHandle (myFile) == 0)
    {
	// TW
	return FALSE;
    }
    
    return TRUE;
} // MySaveTextToFile


/************************************************************************/
/* MyScrollHandlerHorz							*/
/************************************************************************/
static void	MyScrollHandlerHorz (HWND pmConsoleTextWindow, int pmPart, 
				     int pmValue)
{
    ConsoleWindowInfoPtr	myInfo;
    int				myWidth;
    int				myMaxCol;
    int 			myNewLeftCol;
        
    myInfo = MyGetConsoleWindowInfo (pmConsoleTextWindow);
    myWidth = myInfo -> windowWidth;
    myMaxCol = max (0, myInfo -> text -> longestLineLen - (myWidth - 2));
    
    switch (pmPart)
    {
    	case SB_PAGERIGHT:
    	    myNewLeftCol = myInfo -> leftCol + (myWidth - 2);
    	    break;
    	case SB_LINERIGHT:
    	    myNewLeftCol = myInfo -> leftCol + 1;
    	    break;
    	case SB_PAGELEFT:
    	    myNewLeftCol = myInfo -> leftCol - (myWidth - 2);
    	    break;
    	case SB_LINELEFT:
    	    myNewLeftCol = myInfo -> leftCol - 1;
    	    break;
    	case SB_LEFT:
    	    myNewLeftCol = 0;
    	    break;
    	case SB_RIGHT:
    	    myNewLeftCol = myMaxCol;
    	    break;
    	case SB_THUMBPOSITION:
	    myNewLeftCol = pmValue;    	    
    	    break;
    	case SB_THUMBTRACK:
    	    myNewLeftCol = pmValue;
    	    break;
    	default:
    	    return;
    } // switch
    
    myNewLeftCol = max (0, min (myNewLeftCol, myMaxCol));
    
    if (myNewLeftCol == myInfo -> leftCol)
        return;
    
    myInfo -> leftCol = myNewLeftCol;
    SetScrollPos (pmConsoleTextWindow, SB_HORZ, myNewLeftCol, TRUE);
    MyCaretMove (pmConsoleTextWindow);    
    MyDisplayText (pmConsoleTextWindow, FALSE, ALL_LINES, ALL_LINES);
} // MyScrollHandlerHorz


/************************************************************************/
/* MyScrollHandlerVert							*/
/************************************************************************/
static void	MyScrollHandlerVert (HWND pmConsoleTextWindow, int pmPart, 
				     int pmValue)
{
    ConsoleWindowInfoPtr	myInfo;
    int				myHeight;
    int				myMaxLine;
    int 			myNewTopLine;
        
    myInfo = MyGetConsoleWindowInfo (pmConsoleTextWindow);
    myHeight = myInfo -> windowHeight;
    myMaxLine = max (0, myInfo -> text -> numLines - (myHeight - 2));
        
    switch (pmPart)
    {
    	case SB_PAGEDOWN:
    	    myNewTopLine = myInfo -> topLine + (myHeight - 2);
    	    break;
    	case SB_LINEDOWN:
    	    myNewTopLine = myInfo -> topLine + 1;
    	    break;
    	case SB_PAGEUP:
    	    myNewTopLine = myInfo -> topLine - (myHeight - 2);
    	    break;
    	case SB_LINEUP:
    	    myNewTopLine = myInfo -> topLine - 1;
    	    break;
    	case SB_TOP:
    	    myNewTopLine = 0;
    	    break;
    	case SB_BOTTOM:
    	    myNewTopLine = myMaxLine;
    	    break;
    	case SB_THUMBPOSITION:
	    myNewTopLine = pmValue;    	    
    	    break;
    	case SB_THUMBTRACK:
    	    myNewTopLine = pmValue;
    	    break;
    	default:
    	    return;
    } // switch
    
    myNewTopLine = max (0, min (myNewTopLine, myMaxLine));
    
    if (myNewTopLine == myInfo -> topLine)
        return;
    
    myInfo -> topLine = myNewTopLine;
    SetScrollPos (pmConsoleTextWindow, SB_VERT, myNewTopLine, TRUE);
    MyCaretMove (pmConsoleTextWindow);    
    MyDisplayText (pmConsoleTextWindow, FALSE, ALL_LINES, ALL_LINES);
} // MyScrollHandlerVert


/************************************************************************/
/* MyScrollToSelection							*/
/************************************************************************/
static void	MyScrollToSelection (HWND pmConsoleTextWindow, 
				     int pmSkipDistance, BOOL pmRedraw,
				     BOOL pmScrollToEnd)
// One DEFAULT, DOWN_PAGE, UP_PAGE, SCROLL_TOP_OF_SCREEN, SCROLL_BOTTOM_OF_SCREEN,
{
    ConsoleWindowInfoPtr	myInfo;
    int				myLine, myCol;
    int				myFirstVisibleLine, myLastVisibleLine;
    int				myFirstVisibleCol,myLastVisibleCol;
    int				myNewTopLine, myNewLeftCol;
    int				myHeight, myWidth;
    int				myMaxLine, myMaxCol;
    int				myPageDistance;
    
    myInfo = MyGetConsoleWindowInfo (pmConsoleTextWindow);
    myNewTopLine = myInfo -> topLine;
    myNewLeftCol = myInfo -> leftCol;
    myHeight = myInfo -> windowHeight;
    myWidth = myInfo -> windowWidth;
    myMaxLine = max (0, myInfo -> text -> numLines - (myHeight - 2));
    myMaxCol = max (0, myInfo -> text -> longestLineLen - (myWidth - 2));
    myPageDistance = myHeight - 2;

    if (pmScrollToEnd)
    {
	MyGetCurrentLineCol (myInfo -> text, &myLine, &myCol);
    }
    else
    {
    	EdText_GetSelectionLocation (myInfo -> text, &myLine, &myCol);
    }
    
    myFirstVisibleLine = myInfo -> topLine;
    myLastVisibleLine = myFirstVisibleLine + myHeight - 1;
    myFirstVisibleCol = myInfo -> leftCol;
    myLastVisibleCol = myFirstVisibleCol + myInfo -> windowWidth - 1;
    
    if ((myFirstVisibleLine <= myLine) && (myLine <= myLastVisibleLine) &&
        (myFirstVisibleCol <= myCol) && (myCol <= myLastVisibleCol))
    {
    	// The selection is currently visible
    	if (pmRedraw)
    	{
    	    MyDisplayText (pmConsoleTextWindow, FALSE, ALL_LINES, ALL_LINES);
    	}
    	return;
    }

    // Handle vertical displacement first    
    if ((myFirstVisibleLine <= myLine) && (myLine <= myLastVisibleLine))
    {
    	// Do nothing, and skip all the other checks
    }
    else
    {
    	// If within three lines of the top of the screen, place the 
    	// selection at the top of the screen
    	if ((myFirstVisibleLine - 3 <= myLine) && (myLine < myFirstVisibleLine))
    	{
    	    // Place selection at top of screen
    	    myNewTopLine = myLine;
    	}
    	// If within three lines of the bottom, place the selection at 
    	// the bottom of the screen
    	else if ((myLastVisibleLine < myLine) && 
    	    (myLine <= myLastVisibleLine + 3))
    	{
    	    // Place selection one line from bottom of screen
    	    myNewTopLine = myLine - myHeight + 1;
	}
	else
	{
	    // Place the selection approximately 1/3 from the top of
	    // the screen
	    myNewTopLine = myLine - myHeight / 3;
	}
    }

    // Handle horizontal displacement second    
    if ((myFirstVisibleCol <= myCol) && (myCol <= myLastVisibleCol))
    {
    	// Do nothing, and skip all the other checks
    }
    // If we're within a screen's width of the left side, place it on the
    // left side
    else if (myCol < myWidth)
    {
    	myNewLeftCol = 0;
    }
    else
    {
    	// Otherwise, place the selection in the middle of the screen
    	myNewLeftCol = myCol - myWidth / 2;
    }
    

    // Make certain we're within bounds
    myNewTopLine = max (0, min (myMaxLine, myNewTopLine));
    myNewLeftCol = max (0, min (myMaxCol, myNewLeftCol));
    
    if (pmRedraw || (myNewTopLine != myInfo -> topLine) || 
        (myNewLeftCol != myInfo -> leftCol))
    {
    	myInfo -> topLine = myNewTopLine;
    	myInfo -> leftCol = myNewLeftCol;
    	MyCaretMove (pmConsoleTextWindow);
	MySynchScrollBarsToText (pmConsoleTextWindow);
    	MyDisplayText (pmConsoleTextWindow, FALSE, ALL_LINES, ALL_LINES);
    }
} // MyScrollToSelection


/************************************************************************/
/* MySelectAll								*/
/************************************************************************/
static void	MySelectAll (HWND pmConsoleWindow)
{
    HWND	myConsoleTextWindow;
    int		myStatus;

    myConsoleTextWindow = MyGetConsoleTextWindow (pmConsoleWindow);
    
    myStatus = EdText_SelectAll (MyGetTextPtr (pmConsoleWindow));
    if (myStatus == HIDE_SELECTION)
    {
        MyCaretDisplay (myConsoleTextWindow );
        MyCaretMove (myConsoleTextWindow );
    }
    else if (myStatus == CREATE_SELECTION)
    {
    	MyCaretHide (myConsoleTextWindow );
    }
    MyScrollToSelection (myConsoleTextWindow, SCROLL_NORMAL, 
        myStatus != NO_REDRAW, FALSE);
} // MySelectAll


/************************************************************************/
/* MySetDisplayFonts							*/
/************************************************************************/
static BOOL	MySetDisplayFonts (HWND pmConsoleTextWindow)
{
    HINSTANCE	myApplicationInstance;
    HDC		myDisplayContext;
    TEXTMETRIC	myTextMetrics;
    
    // Get the font name
    myApplicationInstance = (HINSTANCE) GetWindowLong (pmConsoleTextWindow, 
        GWL_HINSTANCE);

    // Delete any previously existing font
    if (stNormalFont != NULL)
    {
    	DeleteObject (stNormalFont);
    }
    
    // Create the new font
    stNormalFont = EdGUI_CreateFont (pmConsoleTextWindow, NULL, 
        stProperties.textFontName, stProperties.textFontSize, 
        NO_BOLD, NO_ITALIC);
    if (stNormalFont == NULL)
    {
    	EdFail_Warn (IDS_CREATEFONTFAIL, __FILE__, __LINE__, GetLastError ());
    	return FALSE;
    }

    myDisplayContext = GetDC (pmConsoleTextWindow);
    SelectFont (myDisplayContext, stNormalFont);
    GetTextMetrics (myDisplayContext, &myTextMetrics);
    stTextFontCharHeight = myTextMetrics.tmHeight;
    stTextFontCharWidth = myTextMetrics.tmAveCharWidth;
    
    // Initialize the selection colour		     
    stSelectionColour = GetNearestColor (myDisplayContext, SELECTION_COLOUR);

    ReleaseDC (pmConsoleTextWindow, myDisplayContext);
    
    return TRUE;
} // MySetDisplayFonts


/************************************************************************/
/* MySetRunState							*/
/************************************************************************/
static void	MySetRunState (HWND pmConsoleWindow, int pmRunStatus)
{
    ConsoleWindowInfoPtr	myInfo;
    char			myWindowTitle [1024];
  
    myInfo = MyGetConsoleWindowInfo (pmConsoleWindow);

    // Check if we are already finished    
    myInfo -> runStatus = pmRunStatus;
    
    strcpy (myWindowTitle, myInfo -> className);
    if (pmRunStatus == STATUS_RUNNING)
    {
    	strcat (myWindowTitle, " - Running");
	JSTop_SwitchResumeToPause (myInfo -> buttonBarWindow);
    }
    else if (pmRunStatus == STATUS_FINISHED)
    {
    	strcat (myWindowTitle, " - Finished ");
    	JSTop_DisablePauseButton (myInfo -> buttonBarWindow);
    }
    else if (pmRunStatus == STATUS_WAITING_FOR_INP)
    {
    	strcat (myWindowTitle, " - Waiting for Input ");
	JSTop_SwitchResumeToPause (myInfo -> buttonBarWindow);
    	SendNotifyMessage (pmConsoleWindow, WM_SHOW_CURSOR, (WPARAM) NULL, 
    			   (LPARAM) NULL);
    	MyProcessFromKeyboardToLineBuffer (pmConsoleWindow);
    }
    else if (pmRunStatus == STATUS_PAUSED)
    {
    	strcat (myWindowTitle, " - Paused ");
	JSTop_SwitchPauseToResume (myInfo -> buttonBarWindow);
    	SendNotifyMessage (pmConsoleWindow, WM_HIDE_CURSOR, (WPARAM) NULL, 
    			   (LPARAM) NULL);
    }
    else
    {
    	strcat (myWindowTitle, " - Unknown State ");
    }

    strcat (myWindowTitle, " - Standard Input/Output - Ready to Program");
    SetWindowText (pmConsoleWindow, myWindowTitle);
} // MySetRunState


/************************************************************************/
/* MySynchScrollBarsToText			      			*/
/*						      			*/
/* Set the scroll bars to correspond with the current top-most line and	*/
/* left-most column.  Also change the size of the scroll bars to 	*/
/* reflect the number of lines and the longest line.			*/
/************************************************************************/
static void	MySynchScrollBarsToText (HWND pmConsoleTextWindow)
{
    ConsoleWindowInfoPtr	myInfo;
    TextPtr			myText;
    SCROLLINFO			myScrollInfo;
    
    // Get the window information
    myInfo = MyGetConsoleWindowInfo (pmConsoleTextWindow);
    myText = myInfo -> text;
    
    myScrollInfo.cbSize = sizeof (SCROLLINFO);
    myScrollInfo.fMask = SIF_DISABLENOSCROLL | SIF_PAGE | SIF_POS | SIF_RANGE;
    myScrollInfo.nMin = 0;
    myScrollInfo.nMax = myText -> numLines + 1;
    myScrollInfo.nPage = myInfo -> windowHeight;
    myScrollInfo.nPos = myInfo -> topLine;
    
    SetScrollInfo (pmConsoleTextWindow, SB_VERT, &myScrollInfo, TRUE);

    myScrollInfo.cbSize = sizeof (SCROLLINFO);
    myScrollInfo.fMask = SIF_DISABLENOSCROLL | SIF_PAGE | SIF_POS | SIF_RANGE;
    myScrollInfo.nMin = 0;
    myScrollInfo.nMax = myText -> longestLineLen + 1;
    myScrollInfo.nPage = myInfo -> windowWidth;
    myScrollInfo.nPos = myInfo -> leftCol;
    
    SetScrollInfo (pmConsoleTextWindow, SB_HORZ, &myScrollInfo, TRUE);
    
    GetScrollInfo (pmConsoleTextWindow, SB_VERT, &myScrollInfo);
    GetScrollInfo (pmConsoleTextWindow, SB_HORZ, &myScrollInfo);
} // MySynchScrollBarsToText


