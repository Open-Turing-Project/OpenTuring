/************/
/* eddisp.c */
/************/

/*******************/
/* System includes */
/*******************/
#include <windows.h>
#include <windowsx.h>
#include <crtdbg.h>

/****************/
/* Self include */
/****************/
#include "eddisp.h"

/******************/
/* Other includes */
/******************/
#include "ed.h"
#include "ederr.h"
#include "edfail.h"
#include "edglob.h"
#include "edgui.h"
#include "edlog.h"
#include "edmenu.h"
#include "edprint.h"
#include "edrec.h"
#include "edrun.h"
#include "edtext.h"
#include "edtur.h"
#include "edsearch.h"
#include "edwin.h"

#include "edcommands.h"
#include "eddialogs.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
// Argument for MyDisplayText
#define ALL_LINES		-99

// Some colours                        
#define COLOUR_WHITE		RGB (255, 255, 255)
#define COLOUR_LIGHTGREY	RGB (192, 192, 192)
#define COLOUR_DARKGREY		RGB (40, 40, 40)
#define COLOUR_BLACK		RGB (0, 0, 0)
#define COLOUR_RED		RGB (160, 0, 0)
#define COLOUR_GREEN		RGB (0, 160, 0)
#define COLOUR_BLUE		RGB (0, 0, 255)
#define COLOUR_DEEPBLUE		RGB (0, 0, 132)

// Text colours
#define	CLR_NORMAL		COLOUR_BLACK
#define CLR_IDENTIFIER		COLOUR_BLUE
#define CLR_PREDEF		COLOUR_BLACK
#define CLR_KEYWORD		COLOUR_BLACK
#define CLR_STRING		COLOUR_RED
#define CLR_LINE_COMMENT	COLOUR_GREEN
#define CLR_LONG_COMMENT	COLOUR_GREEN

// The colour the selection will appear in
#define SELECTION_COLOUR	COLOUR_DEEPBLUE

// The max size of selection on which the Find Selection command can be used
#define MAX_SEL_SIZE		50

// Used for scrolling with the mouse
#define SCROLL_TIMER		1
#define HORIZ_TICKS_PER_SCROLL	4

// Scrolling instructions for MyScrollToSelection
#define SCROLL_TOP_OF_SCREEN	100
#define SCROLL_BOTTOM_OF_SCREEN	101
#define SCROLL_PAGE_DOWN	102
#define SCROLL_PAGE_UP		103
#define SCROLL_NORMAL		104

// MyFileOpen parameters
#define CHECK_FOR_CLOSE		TRUE
#define DONT_CHECK		FALSE

// MyFileSave parameters
#define SAVE_COMMAND		FALSE
#define SAVE_AS_COMMAND		TRUE
#define SAVE_WHOLE_FILE		FALSE
#define SAVE_SELECTION		TRUE

// MyHandleKeystroke parameters
#define NORMAL_KEY			 0
#define DELETE_KEY			 1
#define BACKTAB_KEY			 2

// MyCreatePseudoPath parameters
#define GATHER_REAL_FILES_FIRST		 1
#define GATHER_REAL_FILES		 2
#define CREATE_PSEUDO_PATHS		 3
#define FILE_NAME_AMBIGUITY		 4

// The whitespace along the left and top edges
#define MARGIN				 2

// The width of the breakpoint marking
#define BREAKPOINT_MARGIN		12

// Used in MyDisplayText
#define NO_SEL_ON_THIS_LINE	    -1
#define TO_END_OF_LINE	 	999999
#define IN_SELECTION		     1
#define IN_HIGHLIGHT		     2
#define IN_NORMAL		     3

// Parameters for MyEditCommentRange
#define COMMENT			TRUE
#define UNCOMMENT		FALSE

// Parameters for MyEditCutCopy
#define CUT			TRUE
#define COPY			FALSE

// Used when saving RTF/EXE
#define RTF_SUFFIX		".rtf"
#define EXE_SUFFIX		".exe"

// Used for by the mouse EdGUI_GetMouseWheelSettings
#ifndef WM_MOUSEWHEEL
// Message ID for IntelliMouse wheel
#define WM_MOUSEWHEEL WM_MOUSELAST+1 
#endif // #ifndef WM_MOUSEWHEEL

// Used to determine where caret should go in error message
#define SEMICOLON_ERROR_1	"Missing \";\" at end of statement"
#define SEMICOLON_ERROR_2	"\";\" inserted to complete"
    
// Used by OpenOver dialog
#define SAVE_AND_OPEN		IDYES
#define DONT_SAVE_AND_OPEN	IDNO

// Used by MyAddFileSuffix
#define USE_NO_SUFFIX		0
#define USE_JAVA_SUFFIX		1
#define USE_TURING_SUFFIX	2
#define USE_MENU_SUFFIX		3

/********************/
/* Global variables */
/********************/

/*********/
/* Types */
/*********/
typedef struct
{
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
    char	selection [MAX_SEL_SIZE]; // The selection to be searched for
    
    int		accumulateDelta; // For the wheel mouse
    
    BOOL	isReallyJava;	// True if filename ends in ".java"
    BOOL	isReallyTuring;	// True if filename ends in ".t", ".tur", ...
    BOOL	isJava;		// True if filename ends in ".java" or
    				// no suffix and environment is java
    BOOL	isTuring;	// True if filename ends in ".t", ".tur", ... or
    				// no suffix and environment is turing
    
    // Used for mouse selection
    BOOL	selectingWithMouse;
    int		horizontalTicks, verticalTicks;
    int		numClicks, lastClickTime;
    int		lastMouseLine, lastMouseCol;
    BOOL	clickOnSameChar;
    
    // File name information
    BOOL	isExample;
    char	*exampleName;
    char	*pathName;
    char	*pseudoPathName;
    int		unnamedNumber;
    char	*suffix;	// The default suffix for this file gotten
				// from the New menu.  Otherwise NULL
    
    // Turing information
    // Note that when a file is saved with a new name, the Turing file number
    // changes.  In order to make certain we're loading the same file we used
    // when there was an error, we save the original Turing file numbers at
    // the beginning of each run.  Without it, if we run an unnamed file, save
    // it with a different name while it's running and then halt it, we get
    // an error message saying "Unnamed#? not found".
    WORD	turingFileNo;
    WORD	turingFileNoDuringRun;
    char	**turingTextHandle;
    int		*turingSizePtr;
    int		turingInitializedWhere;
    
    BOOL	syntaxColour; // True if filename suffix is ".java" or unnamed
    BOOL	isMain;
        
    // Window modes
    BOOL	beginnerMode;
    BOOL	oneWindowMode;
    
    // Information on current cursor
    BOOL	isBreakpointCursor;
} TextDisplayWindowInfo, *TextDisplayWindowInfoPtr;

typedef struct
{
    BOOL	beginnerMode;
    BOOL	oneWindowMode;
} WindowCreationInfo, *WindowCreationInfoPtr;

typedef struct 
{
    const char 	*pathName;
    WORD	turingFileNo;
    HWND	*editWindow;
    HWND	*textDisplayWindow;
} MatchWindow;

typedef struct FileList
{
    char		*pathName;
    struct FileList	*next;
} FileList, *FileListPtr;

typedef struct EnumerateInfo
{
    HWND	window;
    int		task;
} EnumerateInfo;

typedef struct CreateEXEArgs
{
    BOOL	closeWindowsOnTerminate;
    BOOL	displayRunWithArgs;
    BOOL	centerOutputWindow;
    BOOL	stopUserClose;
} CreateEXEArgs;

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
static EdDispOnly_Properties	stProperties;
static BOOL			stPropertiesChanged;

//
static HBRUSH		stBlackBrush;

static HCURSOR		stBreakpointCursor, stIBeamCursor;

static HICON		stBreakpointIcon;

static HPEN		stDashPen;

// Colours and associated variables used to display the text
static BOOL		stTextColoursInitialized = FALSE;
static COLORREF		stTextColours [NUM_TEXT_TYPES];
static COLORREF		stSelectionColour;

// Used to display the text
static HFONT		stNormalFont, stBoldFont;
static int		stTextFontCharWidth, stTextFontCharHeight;

// The contextual menu
static HMENU		stContextualMenu;

// Used by the wheel mouse
static int		stWheelMouseDeltaPerLine;
static UINT		stMouseWheelMessage;

// List of enumerated files for purposes of calculating pseudo path names
static FileListPtr	stFileListHead = NULL;

// Whether the resize message should be displayed
static BOOL		stDisplayResizeMessage = TRUE;

// Whether the break points should be displayed or not
static BOOL		stDisplayBreakpoints = FALSE;

// Create EXE dialog box parameters.  Here so that state is saved between calls
static CreateEXEArgs	stCreateEXEArgs;

// Whether a program is currently executing.
static BOOL		stProgramIsRunning = FALSE;

/******************************/
/* Static callback procedures */
/******************************/
static LRESULT CALLBACK MyTextDisplayWindowProcedure (HWND pmTextDisplayWindow, 
						      UINT pmMessage, 
    						      WPARAM pmWParam, 
    						      LPARAM pmLParam);
static LRESULT CALLBACK MyTextDisplayMDIWindowProcedure (
						HWND pmTextDisplayWindow, 
						UINT pmMessage, 
    						WPARAM pmWParam, 
    						LPARAM pmLParam);
static UINT CALLBACK 	MyCreateEXEDialogProcedure (HWND pmWindow, 
						    UINT pmMessage, 
    						    WPARAM pmWParam, 
    						    LPARAM pmLParam);
static BOOL CALLBACK 	MyGetClassNameDialogProcedure (HWND pmDialog, 
						       UINT pmMessage, 
    					       	       WPARAM pmWParam, 
    					               LPARAM pmLParam);
static BOOL CALLBACK 	MyGetProgramKindProcedure (HWND pmDialog, 
						   UINT pmMessage, 
    					       	   WPARAM pmWParam, 
    					           LPARAM pmLParam);
static BOOL CALLBACK 	MyGetProgramNameDialogProcedure (HWND pmDialog, 
						         UINT pmMessage, 
    					       	         WPARAM pmWParam, 
    					                 LPARAM pmLParam);
static BOOL CALLBACK 	MyGotoDialogProcedure (HWND pmDialog, UINT pmMessage, 
    					       WPARAM pmWParam, 
    					       LPARAM pmLParam);
static BOOL CALLBACK 	MyOpenFileOverDialogProcedure (HWND pmDialog, 
						       UINT pmMessage, 
						       WPARAM pmWParam, 
						       LPARAM pmLParam);

/*********************/
/* Static procedures */
/*********************/
static void			MyAddFileSuffix (char *pmPathName, 
						 int pmSuffixType,
						 char *pmFileSuffix);
static void			MyAdjustContextualMenu (
					HWND pmTextDisplayWindow);
static void			MyCaretCreate (HWND pmTextDisplayWindow);
static void			MyCaretDisplay (HWND pmTextDisplayWindow);
static void			MyCaretHide (HWND pmTextDisplayWindow);
static void			MyCaretMove (HWND pmTextDisplayWindow);
static BOOL			MyCheckForRevert (HWND pmTextDisplayWindow);
static BOOL			MyCommandHandler (HWND pmTextDisplayWindow, 
						  short command);
static BOOL			MyCommonMessageHandler (
					HWND pmTextDisplayWindow, 
					UINT pmMessage, WPARAM pmWParam, 
    					LPARAM pmLParam, BOOL *pmReturnVal);
static void			MyCreateEXE (HWND pmTextDisplayWindow);
static BOOL			MyCreatePseudoPaths (HWND pmWindow);
static void			MyCreatePseudoPathEnumerator (char *pmPathName, 
		    				HWND pmEditWindow, 
		    				HWND pmTextDisplayWindow, 
		    				TextPtr pmTextPtr, 
		    				void *pmEnumerateInfoPtr);
static int			MyCreateTextDisplayWindow (
						HWND pmTextDisplayWindow, 
						BOOL pmBeginnerMode, 
						BOOL pmOneWindowMode);
static TextDisplayWindowInfoPtr	MyCreateTextDisplayWindowInfo (void);
static void			MyDetermineEmergencyName (char *pmPathName,
						char *pmEmergencyPathName);
static void			MyDisplayText (HWND pmTextDisplayWindow, 
					       BOOL pmPaintUpdate,
					       int pmFromLine, int pmToLine);
static void			MyDispose (HWND pmTextDisplayWindow);
static void			MyEditUndo (HWND pmTextDisplayWindow, 
					    BOOL pmUndo);
static void			MyEditCutCopy (HWND pmTextDisplayWindow,
					       BOOL pmCut);
static void			MyEditPaste (HWND pmTextDisplayWindow);
static void			MyEditSelectAll (HWND pmTextDisplayWindow);
static void			MyEditCommentRange (HWND pmTextDisplayWindow,
						    BOOL pmComment);
static void			MyEditCompleteStructure (
						HWND pmTextDisplayWindow);
static void			MyEditIndent (HWND pmTextDisplayWindow);
static void			MyEditInsert (HWND pmTextDisplayWindow,
					      int pmInsertSkeleton);
static void			MyFileNew (HWND pmTextDisplayWindow,
					   int pmNewSkeleton);
static BOOL			MyFileOpen (HWND pmTextDisplayWindow, 
					    const char *pmPathName,
					    const char *pmExampleName,
					    BOOL pmCheckForClose);
static void			MyFileRevert (HWND pmTextDisplayWindow);
static BOOL			MyFileSave (HWND pmWindow, BOOL	pmSaveAs, 
					    BOOL pmSelection);
static void			MyFindMatchingBrace (HWND pmTextDisplayWindow);
static void			MyFindNext (HWND pmTextDisplayWindow,
					    BOOL pmForward);
static void			MyFindReplace (HWND pmTextDisplayWindow);
static void			MyFindReplaceAll (HWND pmTextDisplayWindow);
static void			MyFindSelection (HWND pmTextDisplayWindow);
static void			MyFindStatusDisplay (HWND pmTextDisplayWindow, 
						     int pmStatus, 
    				     		     BOOL pmWasSelection);
static HWND			MyGetEditWindow (HWND pmTextDisplayWindow);
static int 			MyGetKindOfProgramToRun (
						HWND pmTextDisplayWindow);
static const char 		*MyGetPathNameToRun (HWND pmTextDisplayWindow);
static int			MyGetStructure (TextPtr pmTextPtr, 
						int pmLineNum, 
						char *pmEndStruct);
static TextDisplayWindowInfoPtr	MyGetTextDisplayWindowInfo (
						HWND pmTextDisplayWindow);
static TextPtr			MyGetTextPtr (HWND pmTextDisplayWindow);
static void			MyGotoLine (HWND pmTextDisplayWindow);
static void			MyInitializeColours (HWND pmTextDisplayWindow);
static BOOL			MyIsMovementCommand (int pmCommand);
static void			MyKeystrokeHandler (HWND pmTextDisplayWindow, 
						    int pmKeystroke, 
						    int pmSpecialKey);
static void			MyMarkMoveToLocation (HWND pmTextDisplayWindow, 
				   		      int pmMarkNumber);
static void			MyMatchPathName (char *pmPathName, 
						 HWND pmEditWindow, 
				    		 HWND pmTextDisplayWindow, 
				    		 TextPtr pmTextPtr, 
				    		 void *pmMatch);
static void			MyMenuItemEnable (HMENU pmMenu, int pmItemPosition, 
						  BOOL pmEnabled);
static BOOL			MyMouseDownHandler (HWND pmTextDisplayWindow, 
						    int pmX, int pmY,
		    				    BOOL pmShift, 
		    				    BOOL pmAllowOutOfBounds);
static void			MyMouseExtendSelectionToLine (
						HWND pmTextDisplayWindow);
static void			MyMouseExtendSelectionToWord (
						HWND pmTextDisplayWindow);
static BOOL			MyMouseInBreakpoint (HWND pmTextDisplayWindow, 
						     int pmX, int pmY);
static BOOL			MyMouseInTextRect (HWND pmTextDisplayWindow, 
						   int pmX, int pmY);
static void			MyResizeTextDisplayWindow (
						HWND pmTextDisplayWindow);
static void			MySaveRTF (HWND pmTextDisplayWindow);		
static void			MySelectionDelete (HWND pmTextDisplayWindow);
static BOOL			MySetDisplayFonts (HWND pmTextDisplayWindow);
static void			MyScrollHandlerHorz (HWND pmTextDisplayWindow, 
						     int pmPart, int pmValue);
static void			MyScrollHandlerVert (HWND pmTextDisplayWindow, 
						     int pmPart, int pmValue);
static void			MyScrollToSelection (HWND pmTextDisplayWindow, 
						     int pmSkipSize, 
						     BOOL pmRedraw);
static BOOL			MySetLanguageFromPath (
						TextDisplayWindowInfoPtr pmInfo,
						const char *pmPath);
static void			MyShowStatusForCursorMove (						     
						HWND pmTextDisplayWindow);
static void			MySynchScrollBarsToText (
						HWND pmTextDisplayWindow);

/***********************/
/* External procedures */
/***********************/
/************************************************************************/
/* EdDisp_Init								*/
/*									*/
/* EdGUI_Init must have been run before EdDisp_Init			*/
/************************************************************************/
BOOL	EdDisp_Init (void)
{
    char	myTextDisplayWindowClassName [256];
    char	myTextDisplayMDIWindowClassName [256];
    int		myResult;
    WNDCLASSEX	myWindowClass;

    //
    // Initialize each of the submodules that need initialization 
    // 
    
    if (!EdPrint_Init ())
    {
    	return FALSE;
    }
    
    if (!EdSearch_Init ())
    {
    	return FALSE;
    }
    
    if (!EdText_Init ())
    {
    	return FALSE;
    }
    
    //
    // Initialize the EdDisp module
    //

    // Initialize stuff for the Create EXE dialog
    stCreateEXEArgs.closeWindowsOnTerminate = FALSE;
    stCreateEXEArgs.displayRunWithArgs = FALSE;
    stCreateEXEArgs.centerOutputWindow = FALSE;
    stCreateEXEArgs.stopUserClose = FALSE;

    // Load the contextual menu
    stContextualMenu = LoadMenu (gProgram.applicationInstance, 
				 MAKEINTRESOURCE (EDIT_CONTEXTUAL_MENU));
    stContextualMenu = GetSubMenu (stContextualMenu, 0);

    // Create Brush
    stBlackBrush = CreateSolidBrush (COLOUR_BLACK);
    if (stBlackBrush == NULL)	  
    {
    	EdFail_Warn (IDS_CREATEBRUSHFAIL, __FILE__, __LINE__, GetLastError ());
    	return FALSE;
    }
    
    // Load the icon
    stBreakpointIcon = LoadIcon (gProgram.applicationInstance, 
    				 MAKEINTRESOURCE (BREAKPOINT_ICON)); 
    if (stBreakpointIcon == NULL)	  
    {
    	EdFail_Warn (IDS_LOADICONFAIL, __FILE__, __LINE__, GetLastError (),
    		     BREAKPOINT_ICON);
    	return FALSE;
    }
    				 
    // Load the cursors
    stBreakpointCursor = LoadCursor (gProgram.applicationInstance, 
    				     MAKEINTRESOURCE (BREAKPOINT_CURSOR)); 
    if (stBreakpointCursor == NULL)	  
    {
    	EdFail_Warn (IDS_LOADCURSORFAIL, __FILE__, __LINE__, GetLastError (),
    		     BREAKPOINT_CURSOR);
    	return FALSE;
    }
    stIBeamCursor = LoadCursor (NULL, IDC_IBEAM);
    if (stIBeamCursor == NULL)	  
    {
    	EdFail_Warn (IDS_LOADCURSORFAIL, __FILE__, __LINE__, GetLastError (),
    		     -1);
    	return FALSE;
    }
    
    // Create the Pens
    stDashPen = CreatePen (PS_DOT, 1, 0);

    // Get the class name
    EdGUI_LoadString (IDS_TEXTDISPLAY_WINDOW_NAME, myTextDisplayWindowClassName, 
	sizeof (myTextDisplayWindowClassName));
    
    //
    // Register the text display window class
    //
    myWindowClass.cbSize = 	  sizeof (myWindowClass);
    // Set window class to redraw when window size changes
    myWindowClass.style = 	  CS_HREDRAW | CS_VREDRAW;
    // Procedure to be called with messages for this window class
    myWindowClass.lpfnWndProc =   MyTextDisplayWindowProcedure;
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
    myWindowClass.lpszClassName = myTextDisplayWindowClassName; 
    // Set the icon for this class.
    myWindowClass.hIconSm = 	  NULL;
    
    myResult = RegisterClassEx (&myWindowClass);
    if (myResult == 0)
    {
    	EdFail_Warn (IDS_REGISTERCLASSFAIL, __FILE__, __LINE__, 
    		     GetLastError ());
    	return FALSE;
    }

    // Get the class name
    EdGUI_LoadString (IDS_TEXTDISPLAY_MDI_WINDOW_NAME, 
	myTextDisplayMDIWindowClassName, 
	sizeof (myTextDisplayMDIWindowClassName));

    //
    // Register the text display MDI window class
    //
    myWindowClass.cbSize = 	  sizeof (myWindowClass);
    // Set window class to redraw when window size changes
    myWindowClass.style = 	  CS_HREDRAW | CS_VREDRAW;
    // Procedure to be called with messages for this window class
    myWindowClass.lpfnWndProc =   MyTextDisplayMDIWindowProcedure;
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
    myWindowClass.lpszClassName = myTextDisplayMDIWindowClassName; 
    // Set the icon for this class.
    myWindowClass.hIconSm = 	  NULL;
    
    myResult = RegisterClassEx (&myWindowClass);
    if (myResult == 0)
    {
    	EdFail_Warn (IDS_REGISTERCLASSFAIL, __FILE__, __LINE__, 
    		     GetLastError ());
    	return FALSE;
    }

    // Initialize the wheel mouse
    EdDisp_MouseWheelSettingsInit (); 
    stMouseWheelMessage = EdGUI_GetMouseWheelMessage ();
       
    return TRUE;
} // EdDisp_Init


/************************************************************************/
/* EdDisp_Finalize							*/
/************************************************************************/
void	EdDisp_Finalize (void)
{
    FileListPtr	myFilePtr, myNextFilePtr;
    
    EdPrint_Finalize ();
    EdSearch_Finalize ();
    EdText_Finalize ();
    
    // Erase the linked list of files in the editor
    myFilePtr = stFileListHead;
    while (myFilePtr != NULL)
    {
    	myNextFilePtr = myFilePtr -> next;
    	free (myFilePtr -> pathName);
    	free (myFilePtr);
    	myFilePtr = myNextFilePtr;
    }
} // EdDisp_Finalize


/************************************************************************/
/* EdDisp_PropertiesSet							*/
/************************************************************************/
void	EdDisp_PropertiesSet (EdDisp_Properties pmProperties)
{
    EdPrint_PropertiesSet (pmProperties.printProperties);
    EdSearch_PropertiesSet (pmProperties.searchProperties);
    EdText_PropertiesSet (pmProperties.textProperties);

    stProperties = pmProperties.myProperties;
    stPropertiesChanged = TRUE;
} // EdDisp_PropertiesSet

	
/************************************************************************/
/* EdDisp_PropertiesImplementChanges					*/
/************************************************************************/
void	EdDisp_PropertiesImplementChanges (HWND pmTextDisplayWindow)
{
    TextDisplayWindowInfoPtr	myInfo;

    if (stPropertiesChanged)
    {
    	// Make changes for the module
    	EdPrint_PropertiesImplementChanges ();
    	EdSearch_PropertiesImplementChanges ();

    	MySetDisplayFonts (pmTextDisplayWindow);
    	stPropertiesChanged = FALSE;
    }
    
    // Make changes for the object
    EdText_PropertiesImplementChanges (MyGetTextPtr (pmTextDisplayWindow));

    // Changing the font sizes can change the window somewhat.
    myInfo = MyGetTextDisplayWindowInfo (pmTextDisplayWindow);
    myInfo -> windowWidth = (myInfo -> textRect.right - 
        myInfo -> textRect.left) / stTextFontCharWidth;
    myInfo -> windowHeight = (myInfo -> textRect.bottom - 
        myInfo -> textRect.top) / stTextFontCharHeight;
        
    MySynchScrollBarsToText (pmTextDisplayWindow);
} // EdDisp_PropertiesImplementChanges


/*************************************************************************/
/* EdDisp_Create							 */
/*************************************************************************/
HWND	EdDisp_Create (HWND pmEditWindow, BOOL pmBeginnerMode, 
		       BOOL pmOneWindowMode)
{
    HINSTANCE		myApplicationInstance;
    char		myClassName [256];
    HWND		myTextDisplayWindow;
    WindowCreationInfo	myWindowCreationInfo;
    
    // Get the program instance
    myApplicationInstance = 
        (HINSTANCE) GetWindowLong (pmEditWindow, GWL_HINSTANCE);
        
    // Load the class name (we've already loaded it once, so no error checking)    

    myWindowCreationInfo.beginnerMode = pmBeginnerMode;
    myWindowCreationInfo.oneWindowMode = pmOneWindowMode;
    
    if (pmOneWindowMode)
    {
	MDICREATESTRUCT	myMDICreateStruct;
	
    	EdGUI_LoadString (IDS_TEXTDISPLAY_MDI_WINDOW_NAME, myClassName, 
    	    sizeof (myClassName));

    	ZeroMemory (&myMDICreateStruct, sizeof (myMDICreateStruct));
	myMDICreateStruct.szClass = myClassName;
	myMDICreateStruct.szTitle = "Test";
	myMDICreateStruct.hOwner = myApplicationInstance;
	myMDICreateStruct.style = WS_VSCROLL | WS_HSCROLL | WS_MAXIMIZE;
	myMDICreateStruct.lParam = (LPARAM) &myWindowCreationInfo;
	
	// In the case on oneWindowMode, pmEditWindow is actually the MDI client
	myTextDisplayWindow = (HWND) SendMessage (pmEditWindow, WM_MDICREATE,
	    0, (LPARAM) &myMDICreateStruct);
    }
    else
    {
        // Create the editor window
    	EdGUI_LoadString (IDS_TEXTDISPLAY_WINDOW_NAME, myClassName,
    	    sizeof (myClassName));

    	myTextDisplayWindow = CreateWindow (
    	    myClassName, 			// Window class
            NULL,	 			// Window title
            WS_CHILDWINDOW | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL,
			                	// Window style
    	    0, 0, 0, 0, 			// Initial location & size
    	    pmEditWindow, 			// Parent window handle
    	    NULL, 				// Window menu handle
    	    myApplicationInstance,	 	// Program instance handle
    	    &myWindowCreationInfo);		// Creation parameters
    }
    
    if (stProperties.logWindowMessages)
    {
    	EdLog_Log ("Text Display Window = %x  (Finished Creation)", 
    	    myTextDisplayWindow);
    }

    return myTextDisplayWindow;
} // EdDisp_Create


/************************************************************************/
/* EdDisp_Create2							*/
/*									*/
/* This is called after all the child windows have been created.  This	*/
/* opens up the specified file or opens a new file.			*/
/************************************************************************/
void	EdDisp_Create2 (HWND pmTextDisplayWindow, const char *pmPathName,
			const char *pmExampleName, int pmNewSkeleton)
{
    // Open a file in the editor window if one is passed in.    
    if (pmPathName == NULL)
    {
    	// Create a new unnamed file
    	MyFileNew (pmTextDisplayWindow, pmNewSkeleton);
    }
    else
    {
    	// Load the file specified by the path.
    	if (!MyFileOpen (pmTextDisplayWindow, pmPathName, pmExampleName, 
			 CHECK_FOR_CLOSE))
    	{
    	    // Open failed, create a new unnamed file
    	    MyFileNew (pmTextDisplayWindow, 0);
    	}
    }
} // EdDisp_Create2


/************************************************************************/
/* EdDisp_AreBreakpointsVisible						*/
/************************************************************************/
BOOL	EdDisp_AreBreakpointsVisible (void)
{
    return stDisplayBreakpoints;
} // EdDisp_AreBreakpointsVisible


/************************************************************************/
/* EdDisp_CheckForClose							*/
/************************************************************************/
BOOL	EdDisp_CheckForClose (HWND pmTextDisplayWindow)
{
    TextDisplayWindowInfoPtr	myInfo;
    TextPtr			myTextPtr;
    const char			*myName;
    int				myResult;

    myInfo = MyGetTextDisplayWindowInfo (pmTextDisplayWindow);
    myTextPtr = myInfo -> text;

    // If the file has not been modified, then okay to close    
    if (!myTextPtr -> isDirty)
    {
    	return TRUE;
    }

    // It's also okay to close the window if it's empty
    if (EdDisp_IsEmptyFile (pmTextDisplayWindow) &&
        (myInfo -> pathName == NULL))
    {
    	return TRUE;
    }
        
    // Get a pathname    
    if (myInfo -> pathName != NULL)
    {
    	myName = EdFile_GetFileName (myInfo -> pathName);
    }
    else
    {
    	myName = "file";
    }

    myResult = EdGUI_Message1 (pmTextDisplayWindow, 
    	MB_YESNOCANCEL | MB_ICONWARNING, IDS_APPLICATION_NAME, 
    	IDS_SAVE_CHANGES_MESSAGE, myName);

    // Press Cancel, then don't close
    if (myResult == IDCANCEL)
    {
	return FALSE;
    }

    // Press no, then cancel with no save    
    if (myResult == IDNO)
    {
    	return TRUE;
    }
    
    // Pressed yes.  Try and save and close if save successful
    return MyFileSave (pmTextDisplayWindow, SAVE_COMMAND, SAVE_WHOLE_FILE);
} // EdDisp_CheckForClose


/************************************************************************/
/* EdDisp_ClearBreakpoints						*/
/************************************************************************/
void	EdDisp_ClearBreakpoints (char *pmDummy1, HWND pmDummy2, 
			    	 HWND pmTextDisplayWindow, TextPtr pmTextPtr,
			    	 void *pmDummy)
{
    if (pmTextPtr -> breakLine == NULL)
    {
    	return;
    }
    
    EdText_ClearAllBreakpoints (pmTextPtr);
    MyDisplayText (pmTextDisplayWindow, FALSE, ALL_LINES, ALL_LINES);
} // EdDisp_ClearBreakpoints

			    
/************************************************************************/
/* EdDisp_ClearErrors							*/
/************************************************************************/
void	EdDisp_ClearErrors (char *pmDummy1, HWND pmDummy2, 
			    HWND pmTextDisplayWindow, TextPtr pmTextPtr,
			    void *pmDummy)
{
    if (pmTextPtr -> errorLine == NULL)
    {
    	return;
    }
    
    EdText_ClearErrors (pmTextPtr);
    MyDisplayText (pmTextDisplayWindow, FALSE, ALL_LINES, ALL_LINES);
} // EdDisp_ClearErrors

			    
/************************************************************************/
/* EdDisp_CreateMarkMenu						*/
/************************************************************************/
void	EdDisp_CreateMarkMenu (HWND pmTextDisplayWindow, HMENU pmMenu)
{
    int		myNumItems;
    int		myCommand;
    TextPtr	myText;
    MarkLinePtr	myMark;
    int		cnt;
    
    // Delete the current contents of the menu    
    myNumItems = GetMenuItemCount (pmMenu);
    
    for (cnt = 0 ; cnt < myNumItems ; cnt++)
    {
    	RemoveMenu (pmMenu, 0, MF_BYPOSITION);
    }
    
    myCommand = COMMAND_MARK_BASE;
    
    myText = MyGetTextPtr (pmTextDisplayWindow);
    myMark = myText -> markLine;

    while (myMark != NULL)
    {
    	// Add the item to the menu
    	AppendMenu (pmMenu, MF_STRING, myCommand, myMark -> name);
    	myCommand++;
    	myMark = myMark -> next;
    }
} // EdDisp_CreateMarkMenu


/************************************************************************/
/* EdDisp_EmergencySave							*/
/************************************************************************/
int	EdDisp_EmergencySave (HWND pmTextDisplayWindow, TextPtr pmTextPtr,
			      FilePath myOriginalName, FilePath myEmergencyName)
{
    TextDisplayWindowInfoPtr	myInfo;
    
    myInfo = MyGetTextDisplayWindowInfo (pmTextDisplayWindow);
    
    // Don't save if the file is not dirty, or if its empty
    if ((!pmTextPtr -> isDirty) ||
        (EdDisp_IsEmptyFile (pmTextDisplayWindow) &&
         (myInfo -> pathName == NULL)))
    {
    	return NOT_SAVED;
    }
    
    EdFile_GetPathForDisplay (myInfo -> pathName, myInfo -> unnamedNumber, 
			      myInfo -> isExample, myInfo -> exampleName,
			      FALSE, myOriginalName);

    MyDetermineEmergencyName (myInfo -> pathName, myEmergencyName);
    
    if (myEmergencyName [0] == 0)
    {
    	return SAVE_FAILED;
    }
    
    // Save the text to the file.  If it fails, return immediately
    if (EdFile_SaveTextToFile (pmTextDisplayWindow, myEmergencyName, pmTextPtr, 
    			       FALSE, NULL, NULL)) 
    {
        return SAVED;
    }
    
    return SAVE_FAILED;
} // EdDisp_EmergencySave

			      
/************************************************************************/
/* EdDisp_EnumerateFile							*/
/************************************************************************/
void	EdDisp_EnumerateFile (HWND pmEditWindow, 
			HWND pmTextDisplayWindow, 
			void (*enumProc) (char *, HWND, HWND, void *, void *),
			void *pmUserParam)
{
    TextDisplayWindowInfoPtr	myInfo;
    
    myInfo = MyGetTextDisplayWindowInfo (pmTextDisplayWindow);
    
    (*enumProc) (myInfo -> pathName, pmEditWindow, pmTextDisplayWindow, 
    		 myInfo -> text, pmUserParam);
} // EdDisp_EnumerateFile

			      
/************************************************************************/
/* EdDisp_GetMainFile							*/
/************************************************************************/
char	*EdDisp_GetMainFile (FilePath pmPathDisplayBuffer)
{
    HWND	myTextDisplayWindow;
    const char	*myMainPathName;
    
    myTextDisplayWindow = EdRun_GetMainWindow ();
    if (myTextDisplayWindow != NULL)
    {
    	return EdDisp_GetPathForDisplay (myTextDisplayWindow, USE_FILENAME, 
    	    DONT_SHOW_DIRTY_MARK, pmPathDisplayBuffer);
    }
    
    myMainPathName = EdRun_GetMainPathName ();
    if (myMainPathName != NULL)
    {
    	strcpy (pmPathDisplayBuffer, EdFile_GetFileName (myMainPathName));
    	return pmPathDisplayBuffer;
    }
    
    return NULL;
} // EdDisp_GetMainFile


/************************************************************************/
/* EdDisp_GetPathForDisplay						*/
/************************************************************************/
char	*EdDisp_GetPathForDisplay (HWND pmTextDisplayWindow, 
				   BOOL pmDefaultPathSize, BOOL pmNoDirtyMark,
				   FilePath pmPathForDisplayBuffer)
{
    TextDisplayWindowInfoPtr	myInfo;
    
    myInfo = MyGetTextDisplayWindowInfo (pmTextDisplayWindow);
    
    EdFile_GetPathForDisplay (myInfo -> pathName, myInfo -> unnamedNumber,
        myInfo -> isExample, myInfo -> exampleName,
        stProperties.displayFullPathNames && pmDefaultPathSize, 
        pmPathForDisplayBuffer);
    
    if (myInfo -> text -> isDirty && !pmNoDirtyMark)
    {
    	strcat (pmPathForDisplayBuffer, "*");
    }
    
    return pmPathForDisplayBuffer;
} // EdDisp_GetPathForDisplay


/************************************************************************/
/* EdDisp_GetPath							*/
/************************************************************************/
char	*EdDisp_GetPath (HWND pmTextDisplayWindow)
{
    return MyGetTextDisplayWindowInfo (pmTextDisplayWindow) -> pathName;
} // EdDisp_GetPath


/************************************************************************/
/* EdDisp_GetPseudoPath							*/
/************************************************************************/
char	*EdDisp_GetPseudoPath (HWND pmTextDisplayWindow)
{
    return MyGetTextDisplayWindowInfo (pmTextDisplayWindow) -> pseudoPathName;
} // EdDisp_GetPseudoPath


/************************************************************************/
/* EdDisp_GetRedoStringResource						*/
/************************************************************************/
int	EdDisp_GetRedoStringResource (HWND pmTextDisplayWindow)
{
    TextPtr	myTextPtr;
    
    myTextPtr = MyGetTextPtr (pmTextDisplayWindow);
    if (myTextPtr -> redoTail == NULL)
    {
    	return 0;
    }
    else
    {
    	return myTextPtr -> redoTail -> menuName;
    }
} // EdDisp_GetRedoStringResource


/************************************************************************/
/* EdDisp_GetSelectionLocation						*/
/************************************************************************/
void	EdDisp_GetSelectionLocation (HWND pmTextDisplayWindow, int *pmLine, 
				     int *pmCol, int *pmNumLines)
{
    TextPtr	myTextPtr = MyGetTextPtr (pmTextDisplayWindow);
    
    EdText_GetSelectionLocation (myTextPtr, pmLine, pmCol);
    *pmNumLines = myTextPtr -> numLines;
} // EdDisp__GetSelectionLocation

				      
/************************************************************************/
/* EdDisp_GetTextPtr							*/
/************************************************************************/
TextPtr	EdDisp_GetTextPtr (HWND pmTextDisplayWindow)
{
    return MyGetTextDisplayWindowInfo (pmTextDisplayWindow) -> text;
} // EdDisp_GetTextPtr


/************************************************************************/
/* EdDisp_GetTuringFileNo						*/
/************************************************************************/
WORD	EdDisp_GetTuringFileNo (HWND pmTextDisplayWindow)
{
    return MyGetTextDisplayWindowInfo (pmTextDisplayWindow) -> turingFileNo;
} // EdDisp_GetTuringFileNo


/************************************************************************/
/* EdDisp_GetTuringSizePtr						*/
/************************************************************************/
int	*EdDisp_GetTuringSizePtr (HWND pmTextDisplayWindow)
{
    TextDisplayWindowInfoPtr	myInfo;
    TextPtr			myTextPtr;
    int				*myTuringSizePtr;

    myInfo = MyGetTextDisplayWindowInfo (pmTextDisplayWindow);
    myTextPtr = myInfo -> text;
    myTuringSizePtr = myInfo -> turingSizePtr;
    
    if ((myTuringSizePtr == NULL) || (myTuringSizePtr == (int *) 0xCCCCCCCC))
    {
    	char	*myPathName = myInfo -> pathName;
    	
    	if (myPathName == NULL) myPathName = "(null)";
    	
    	// Major problem here.  The TuringSizePtr has not been properly 
    	// initialized!  We're going down!
       	EdFail_Fatal (IDS_TURING_INFO_NOT_INIT, __FILE__, __LINE__, 0,
       		      (int) myTuringSizePtr, myInfo -> turingInitializedWhere,
		      (int) myInfo -> turingFileNo, myPathName, 
		      myInfo -> unnamedNumber);
    }
    
    return myTuringSizePtr;
} // EdDisp_GetTuringSizePtr


/************************************************************************/
/* EdDisp_GetTuringTextHandle						*/
/************************************************************************/
char	**EdDisp_GetTuringTextHandle (HWND pmTextDisplayWindow)
{
    return MyGetTextDisplayWindowInfo (pmTextDisplayWindow) -> turingTextHandle;
} // EdDisp_GetTuringTextHandle


/************************************************************************/
/* EdDisp_GetUndoStringResource						*/
/************************************************************************/
int	EdDisp_GetUndoStringResource (HWND pmTextDisplayWindow)
{
    TextPtr	myTextPtr;
    
    myTextPtr = MyGetTextPtr (pmTextDisplayWindow);
    if (myTextPtr -> undoTail == NULL)
    {
    	return 0;
    }
    else
    {
    	return myTextPtr -> undoTail -> menuName;
    }
} // EdDisp_GetUndoStringResource


/************************************************************************/
/* EdDisp_GetUnnamedNumber						*/
/************************************************************************/
int	EdDisp_GetUnnamedNumber (HWND pmTextDisplayWindow)
{
    return MyGetTextDisplayWindowInfo (pmTextDisplayWindow) -> unnamedNumber;
} // EdDisp_GetUnnamedNumber


/************************************************************************/
/* EdDisp_GetWindowSize							*/
/************************************************************************/
void	EdDisp_GetWindowSize (HWND pmTextDisplayWindow, int pmNumRows, 
			      int pmNumCols, long *pmPixelWidth, 
			      long *pmPixelHeight)
{
    *pmPixelWidth = stTextFontCharWidth * pmNumCols + MARGIN +
        GetSystemMetrics (SM_CXVSCROLL);
    *pmPixelHeight = stTextFontCharHeight * pmNumRows + MARGIN +
        GetSystemMetrics (SM_CYHSCROLL);
} // EdDisp_GetWindowSize


/************************************************************************/
/* EdDisp_GetWindowWithMatchingPathName					*/
/************************************************************************/
BOOL	EdDisp_GetWindowWithMatchingPathName (const char *pmPathName, 
					      HWND *pmEditWindow,
					      HWND *pmTextDisplayWindow)
{
    MatchWindow	myMatchWindow;
    HWND	myEditWindow, myTextDisplayWindow;
    
    myMatchWindow.pathName = pmPathName;
    if (pmEditWindow != NULL) 
    {
    	myMatchWindow.editWindow = pmEditWindow;
    }
    else
    {
    	myMatchWindow.editWindow = &myEditWindow;
    }
    *(myMatchWindow.editWindow) = NULL;
    if (pmTextDisplayWindow != NULL) 
    {
    	myMatchWindow.textDisplayWindow = pmTextDisplayWindow;
    }
    else
    {
    	myMatchWindow.textDisplayWindow = &myTextDisplayWindow;
    }
    *(myMatchWindow.textDisplayWindow) = NULL;

    Ed_EnumerateFiles (MyMatchPathName, &myMatchWindow);
    
    if (*(myMatchWindow.editWindow) == NULL)
    {
    	return FALSE;
    }

    return TRUE;    
} // Ed_GetWindowWithMatchingPathName


/************************************************************************/
/* EdDisp_GetWindowWithMatchingTuringFileNo				*/
/************************************************************************/
BOOL	EdDisp_GetWindowWithMatchingTuringFileNo (
			WORD pmTuringFileNo, HWND *pmEditWindow,
			HWND *pmTextDisplayWindow)
{
    MatchWindow	myMatchWindow;
    
    myMatchWindow.pathName = NULL;
    myMatchWindow.turingFileNo = pmTuringFileNo;
    myMatchWindow.editWindow = pmEditWindow;
    *(myMatchWindow.editWindow) = NULL;
    myMatchWindow.textDisplayWindow = pmTextDisplayWindow;
    *(myMatchWindow.textDisplayWindow) = NULL;

    Ed_EnumerateFiles (MyMatchPathName, &myMatchWindow);
    
    if (*(myMatchWindow.editWindow) == NULL)
    {
    	return FALSE;
    }

    return TRUE;    
} // EdDisp_GetWindowWithMatchingTuringFileNo


/************************************************************************/
/* EdDisp_GotoError							*/
/************************************************************************/
void	EdDisp_GotoError (HWND pmTextDisplayWindow, int pmLine,
			  int pmTokenPos, int pmTokenLen, 
			  const char *pmErrorMessage)
{
    TextPtr	myTextPtr = MyGetTextPtr (pmTextDisplayWindow);
	
    //
    // While this should never happen, in case we have an error message on a
    // line passed the last line in the file, we display the error message
    // and move the cursor to the end of the file.
    //
    if (pmLine > myTextPtr -> numLines)
    {
    	FilePath	myFilePathBuffer;
    	
    	if (EdText_MoveProgramBottom (myTextPtr, FALSE) == HIDE_SELECTION)
    	{
            MyCaretDisplay (pmTextDisplayWindow);
    	}
    	MyCaretMove (pmTextDisplayWindow);
    	MyScrollToSelection (pmTextDisplayWindow, SCROLL_NORMAL, TRUE);
        
	EdWin_ShowStatusBold (EdGUI_GetTopWindow (pmTextDisplayWindow),
		    "Line %d of \"%s\": %s", pmLine + 1,
		    EdDisp_GetPathForDisplay (pmTextDisplayWindow, FALSE, FALSE,
		    myFilePathBuffer), pmErrorMessage);
    }
    else
    {    	
    	if (pmTokenPos == -1)
    	{
    	    // The entire line is an error, in which case, go to the first text
    	    // on the line
    	    int		myCol = 0;
    	    char	*myLine = 
    	    		  &myTextPtr -> text [myTextPtr -> lineStarts [pmLine]];
    	
    	    while (*myLine == ' ')
    	    {
    	    	myCol++;
    	    	myLine++;
    	    }
    	    if (*myLine != 0)
    	    {
    	    	pmTokenPos = myCol;
    	    }
    	}

        // If the message is complaining about a missing ";", then place the 
        // cursor at the end of the message because that's where the ";" 
        // should go.
        if ((strncmp (pmErrorMessage, SEMICOLON_ERROR_1, 
    	              strlen (SEMICOLON_ERROR_1)) == 0) ||
    	    (strncmp (pmErrorMessage, SEMICOLON_ERROR_2, 
    	              strlen (SEMICOLON_ERROR_2)) == 0))
    	{
    	    pmTokenPos += pmTokenLen;
    	}
    
    	if (EdText_SetSelection (myTextPtr, pmLine, pmTokenPos))
    	{
            MyCaretDisplay (pmTextDisplayWindow);
    	}
    	MyCaretMove (pmTextDisplayWindow);
    	MyScrollToSelection (pmTextDisplayWindow, SCROLL_NORMAL, TRUE);
        
    	EdWin_ShowStatusBold (pmTextDisplayWindow, pmErrorMessage);
    }
    // Reset the Undo buffer
    EdText_UndoSetMoreKeystrokes (myTextPtr, FALSE);
    
    // Debug message
    EdFail_AddMessage (NORMAL, 
    	"Goto Error - sz:%d:%d - slFr:%d - slTo:%d - nlA:%d",
        myTextPtr -> numLines, myTextPtr -> textLen, 
        myTextPtr -> selFrom, myTextPtr -> selTo, 
        myTextPtr -> newLineAdded);
} // EdDisp_GotoError

			  
/************************************************************************/
/* EdDisp_HandleClose							*/
/************************************************************************/
void	EdDisp_HandleClose (HWND pmTextDisplayWindow)
{
    TextDisplayWindowInfoPtr	myInfo;

    // Get the window information
    myInfo = MyGetTextDisplayWindowInfo (pmTextDisplayWindow);

    // If the window we're closing is the main window, we need to make
    // the main a file name rather than a window, or, if the window has no
    // associated file, we need to float the main.
    if (myInfo -> isMain)
    {
    	if (myInfo -> pathName == NULL)
    	{
    	    EdRun_FloatMain ();
    	}
    	else
        {
            EdRun_FixMainPathName (myInfo -> pathName);
        }
    }

    // Tell the Turing File Manager that the file has been closed.
    EdRun_NotifyFileClosed (pmTextDisplayWindow);
} // EdDisp_HandleClose


/************************************************************************/
/* EdDisp_HasErrors							*/
/************************************************************************/
BOOL	EdDisp_HasErrors (HWND pmTextDisplayWindow)
{
    return (MyGetTextPtr (pmTextDisplayWindow) -> errorLine != NULL);
} // EdDisp_HasErrors


/************************************************************************/
/* EdDisp_HilightLine							*/
/************************************************************************/
void	EdDisp_HilightLine (HWND pmTextDisplayWindow, int pmLine)
{
    // Hilight the entire line (used for tracing)
    TextPtr	myTextPtr = MyGetTextPtr (pmTextDisplayWindow);
    int		myStatus = EdText_SelectLine1 (myTextPtr, pmLine);

    if (myStatus == HIDE_SELECTION)
    {
	MyCaretDisplay (pmTextDisplayWindow);
	MyCaretMove (pmTextDisplayWindow);
    }
    else if (myStatus == CREATE_SELECTION)
    {
	MyCaretHide (pmTextDisplayWindow);
    }

    MyScrollToSelection (pmTextDisplayWindow, SCROLL_NORMAL, 
			 myStatus != NO_REDRAW);

    // Clear any display messages
    EdWin_ShowStatus (pmTextDisplayWindow, "");
    
    // Reset the Undo buffer
    EdText_UndoSetMoreKeystrokes (myTextPtr, FALSE);
} // EdDisp_HilightLine


/************************************************************************/
/* EdDisp_IsAssociated							*/
/************************************************************************/
BOOL	EdDisp_IsAssociated (HWND pmTextDisplayWindow)
{
    return 
        (MyGetTextDisplayWindowInfo (pmTextDisplayWindow) -> pathName != NULL);
} // EdDisp_IsAssociated


/************************************************************************/
/* EdDisp_IsDirtyFile							*/
/************************************************************************/
BOOL	EdDisp_IsDirtyFile (HWND pmTextDisplayWindow)
{
    return MyGetTextPtr (pmTextDisplayWindow) -> isDirty;
} // EdDisp_IsDirtyFile


/************************************************************************/
/* EdDisp_IsEmptyFile							*/
/************************************************************************/
BOOL	EdDisp_IsEmptyFile (HWND pmTextDisplayWindow)
{
    return EdText_IsEmpty (MyGetTextPtr (pmTextDisplayWindow));
} // EdDisp_IsEmptyFile


/************************************************************************/
/* EdDisp_IsKnownLanguage						*/
/************************************************************************/
BOOL	EdDisp_IsKnownLanguage (HWND pmTextDisplayWindow)
{
    TextDisplayWindowInfoPtr	myInfo;

    myInfo = MyGetTextDisplayWindowInfo (pmTextDisplayWindow);
    return (myInfo -> isTuring || myInfo -> isJava);
} // EdDisp_IsKnownLanguage


/************************************************************************/
/* EdDisp_IsNearBrace							*/
/*									*/
/* In order for us to be near a brace, either (1) the selection must	*/
/* contain only one brace, or (2) the selection point must be adjacent	*/
/* to one brace.							*/
/************************************************************************/
int	EdDisp_IsNearBrace (HWND pmTextDisplayWindow)
{
    TextPtr	myTextPtr;
    int		myFromPos, myToPos, myPos;
    int		cnt;
    
    myTextPtr = MyGetTextPtr (pmTextDisplayWindow);
    myPos = NO_BRACE_FOUND;    	
    myFromPos = min (myTextPtr -> selFrom, myTextPtr -> selTo);
    myToPos = max (myTextPtr -> selFrom, myTextPtr -> selTo);

    if (myTextPtr -> selFrom == myTextPtr -> selTo)
    {
    	//
    	// Determine if there's a brace before/after the selection point
    	//
    	
    	// Go backwards from the selection point until one reaches either
    	// a non-comment brace, a non-comment non whitespace or a newline
	myFromPos--;
	while ((myFromPos >= 0) &&
	       ((myTextPtr -> text [myFromPos] == ' ') ||
	        (myTextPtr -> textType [myFromPos] >= TEXT_LINE_COMMENT)))
	{
	    myFromPos--;
	}
	if ((myFromPos >= 0) && 
	    ((myTextPtr -> text [myFromPos] == '{') ||
	     (myTextPtr -> text [myFromPos] == '}')) &&
	    (myTextPtr -> textType [myFromPos] < TEXT_LINE_COMMENT))
	{
	    myPos = myFromPos;
	}

    	// Go forwards from the selection point until one reaches either
    	// a non-comment non whitespace or a newline
	while ((myToPos < myTextPtr -> textLen) &&
	       ((myTextPtr -> text [myToPos] == ' ') ||
	        (myTextPtr -> textType [myToPos] >= TEXT_LINE_COMMENT)))
	{
	    myToPos++;
	}
	if ((myToPos >= 0) && 
	    ((myTextPtr -> text [myToPos] == '{') ||
	     (myTextPtr -> text [myToPos] == '}')) &&
	    (myTextPtr -> textType [myToPos] < TEXT_LINE_COMMENT))
	{
    	    // Check to see if we've already found one brace
    	    if (myPos != NO_BRACE_FOUND)
    	    {
    	    	return NO_BRACE_FOUND;
    	    }
	    myPos = myToPos;
	}
    } // if
    else
    {
    	//
    	// Determine if there's more than one brace in the selection
    	//
    	for (cnt = myFromPos ; cnt < myToPos ; cnt++)
    	{
    	    if (((myTextPtr -> text [cnt] == '{') ||
    	         (myTextPtr -> text [cnt] == '}')) &&
    	        (myTextPtr -> textType [cnt] == TEXT_NORMAL))
    	    {
    	    	// Check to see if we've already found one brace
    	    	if (myPos != NO_BRACE_FOUND)
    	    	{
    	    	    return NO_BRACE_FOUND;
    	    	}
    	    	myPos = cnt;
    	    } // if
    	} // for
    } // else

    return myPos;
} // EdDisp_IsNearBrace


/************************************************************************/
/* EdDisp_IsSelectionActive						*/
/************************************************************************/
BOOL	EdDisp_IsSelectionActive (HWND pmTextDisplayWindow)
{
    TextPtr	myTextPtr = MyGetTextPtr (pmTextDisplayWindow);
    
    return myTextPtr -> selTo != myTextPtr -> selFrom;
} // EdDisp_IsSelectionActive


/************************************************************************/
/* EdDisp_MatchesPathName						*/
/************************************************************************/
BOOL	EdDisp_MatchesPathName (HWND pmTextDisplayWindow, 
				const char *pmPathName)
{
    const char	*myPathName;
    
    myPathName = MyGetTextDisplayWindowInfo (pmTextDisplayWindow) -> pathName;
    if ((myPathName != NULL) && (_stricmp (pmPathName, myPathName) == 0))
    {
    	return TRUE;
    }
    
    return FALSE;
} // EdDisp_MatchesPathName


/************************************************************************/
/* EdDisp_MouseWheelSettingsInit					*/
/************************************************************************/
void	EdDisp_MouseWheelSettingsInit (void)
{
    stWheelMouseDeltaPerLine = EdGUI_GetMouseWheelSettings ();
} // EdDisp_MouseWheelSettingsInit


/************************************************************************/
/* EdDisp_OpenOver							*/
/*									*/
/* Used mostly in beginner mode, this method is called when the system	*/
/* wants to open a file over top of the current TextDisplayWindow, 	*/
/* removing it from memory.						*/
/*									*/
/* If the current window is empty, load the new file.  If it's not	*/
/* empty and not dirty, give a message to allow the user not to load	*/
/* the file.  If it's dirty, then give the user the chance to save 	*/
/* the current contents.						*/
/************************************************************************/
BOOL	EdDisp_OpenOver (HWND pmTextDisplayWindow, const char *pmPathName,
			 int pmPurpose)
{
    if (!EdDisp_IsEmptyFile (pmTextDisplayWindow))
    {
    	FilePath	myOriginalFileName;
    	int		myResult;

    	if (EdDisp_IsDirtyFile (pmTextDisplayWindow))
    	{
    	    char	myMessage [4096];
    	    	
	    EdDisp_GetPathForDisplay (pmTextDisplayWindow, USE_FILENAME, 
	    			      DONT_SHOW_DIRTY_MARK, myOriginalFileName);
	    if (pmPurpose == PURPOSE_LOAD_FILE)
	    {    	    
		EdGUI_LoadFormattedString (IDS_OPEN_FILE_DIRTY, myMessage,
		  			   sizeof (myMessage), 
		  			   myOriginalFileName, 
		    			   EdFile_GetFileName (pmPathName));

		Ed_EnableAllWindows (FALSE, pmTextDisplayWindow);
	        myResult = DialogBoxParam (gProgram.applicationInstance, 
        		       		MAKEINTRESOURCE (LOAD_FILE_OVER_DIALOG),
					MyGetEditWindow (pmTextDisplayWindow), 
			       		MyOpenFileOverDialogProcedure, 
			       		(LPARAM) myMessage);
		Ed_EnableAllWindows (TRUE, pmTextDisplayWindow);
	    }
	    else
	    {    	    
		EdGUI_LoadFormattedString (IDS_DISPLAY_ERR_DIRTY, myMessage,
		    			   sizeof (myMessage), 
		    			   myOriginalFileName, 
		    			   EdFile_GetFileName (pmPathName));

		Ed_EnableAllWindows (FALSE, pmTextDisplayWindow);
	        myResult = DialogBoxParam (gProgram.applicationInstance, 
        		       	     MAKEINTRESOURCE (LOAD_ERROR_OVER_DIALOG),
				     MyGetEditWindow (pmTextDisplayWindow), 
			       	     MyOpenFileOverDialogProcedure, 
			       	     (LPARAM) myMessage);
		Ed_EnableAllWindows (TRUE, pmTextDisplayWindow);
	    }
		
	    if (myResult == SAVE_AND_OPEN)
	    {
		// Attempt to save the file
	        if (MyFileSave (pmTextDisplayWindow, SAVE_COMMAND, 
	        		SAVE_WHOLE_FILE))
		{
		    myResult = IDYES;
		}
		else
		{
		    myResult = IDCANCEL;
		}	        		
	    }
	    else if (myResult == DONT_SAVE_AND_OPEN)
	    {
		myResult = IDYES;
	    }
	}
	else
	{
	    if (pmPurpose == PURPOSE_LOAD_FILE)
	    {    	    
		myResult = EdGUI_Message1 (pmTextDisplayWindow, MB_YESNO, 
		    		    	   IDS_OPEN_FILE_TITLE,
			            	   IDS_OPEN_FILE_NOT_DIRTY, 
			            	   EdFile_GetFileName (pmPathName));
	    }
	    else
	    {    	    
		myResult = EdGUI_Message1 (pmTextDisplayWindow, MB_YESNO, 
		    		    	   IDS_DISPLAY_ERR_TITLE,
			            	   IDS_DISPLAY_ERR_NOT_DIRTY, 
			            	   EdFile_GetFileName (pmPathName));
	    }
	}
	if (myResult != IDYES)
	{		            
	    return FALSE;
	}
    }

    // Load the file.
    return MyFileOpen (pmTextDisplayWindow, pmPathName, NULL, DONT_CHECK);
} // EdDisp_OpenOver


/************************************************************************/
/* EdDisp_ResizeWindow							*/
/*									*/
/* This is currently used as an enumeration function for 		*/
/* Ed_EnumerateFiles which is called when the user shows/hides 		*/
/* breakpoints.								*/
/************************************************************************/
void	EdDisp_ResizeWindow (char *pmPathName, HWND pmEditWindow, 
			     HWND pmTextDisplayWindow, TextPtr pmTextPtr, 
			     void *pmEnumerateInfoPtr)
{
    MyResizeTextDisplayWindow (pmTextDisplayWindow);
    MyDisplayText (pmTextDisplayWindow, FALSE, ALL_LINES, ALL_LINES);
} // EdDisp_ResizeWindow

			     
/************************************************************************/
/* EdDisp_Run								*/
/*									*/
/* pmStartWithStep is true when the program should execute a single	*/
/* statement and then pause.						*/
/************************************************************************/
void	EdDisp_Run (HWND pmTextDisplayWindow, BOOL pmExecute, BOOL pmWithArgs,
		    BOOL pmStartWithStep)
{
    const char	*myPathNameToRun;
    BOOL	myNoErrors;
    int		myKindOfProgram;
    BOOL	myRunAgain;
    static HWND	myStWindowToRunNext;
    static BOOL	myStExecuteNext;
    static BOOL	myStWithArgsNext;
        
    // Debug messages
    EdFail_AddMessage (NORMAL, 
    	"Run (1) - Window: %x - Ex: %d - Args: %d - Step: %d", 
    	pmTextDisplayWindow, pmExecute, pmWithArgs, pmStartWithStep);
        
    // Determine the file to run
    if (gProgram.isJava)
    {
    	myPathNameToRun = MyGetPathNameToRun (pmTextDisplayWindow);
    	if (myPathNameToRun == NULL)
    	{
    	    return;
    	}
    
    	myNoErrors = EdRun_Compile (pmTextDisplayWindow, myPathNameToRun, 
    				    !pmExecute);
    	if (myNoErrors && pmExecute)
    	{
    	    myKindOfProgram = MyGetKindOfProgramToRun (pmTextDisplayWindow);
    	    if (myKindOfProgram != NO_PROGRAM)
    	    {
    	    	EdRun_Execute (pmTextDisplayWindow, myPathNameToRun, 
    	    		       myKindOfProgram, pmWithArgs, pmStartWithStep);
	    }
    	}
    }
    else if (gProgram.isTuring)
    {
	//
	// If we get a command to run another window while we're running a
	// program, we want to stop execution of the currently running program
	// before starting compoilation and execution of the new window.  The
	// problem is that the call stack currently looks like 
	//          EdRun_Execute
	//          Ed_ProcessWaitingEvents
	//          MyRun (right here)
	// We handle this by signalling the EdRun module that it should stop
	// execution and that it should then compile and run this window next.
	//
    	// For Turing, Main file is handled in EdRun 
	if (stProgramIsRunning)
	{
	    myStWindowToRunNext = pmTextDisplayWindow;
	    myStExecuteNext = pmExecute;
	    myStWithArgsNext = pmWithArgs;

	    EdRun_KillRunningProgramAndRerun ();
	}
	else
	{
	    stProgramIsRunning = TRUE;

	    // If we've pressed F1 in a run window after a program has 
	    // finished execution, then we use all the arguments from
	    // the last time we ran.
	    
	    if (pmTextDisplayWindow == SAME_WINDOW_AS_LAST_TIME)
	    {
		pmExecute = myStExecuteNext;
		pmWithArgs = myStWithArgsNext;
	    }

	    do
	    {
		myStWindowToRunNext = SAME_WINDOW_AS_LAST_TIME;
		myStExecuteNext = pmExecute;
		myStWithArgsNext = pmWithArgs;
		myRunAgain = FALSE;

		//
		// If we're rerunning the same program (using F1 in run window)
		// then don't bother recompiling and note that the original
		// editor window may no longer be open.  However, we also
		// have to initialize a bit of the Turing engine that normally
		// gets initialized when we recompile (parts of
		// Language_Execute_Initialize)
		//
		if (pmTextDisplayWindow == SAME_WINDOW_AS_LAST_TIME)
		{
		    pmTextDisplayWindow = Ed_GetLastActiveEditWindow ();
		    EdRun_InitializeForRunWithoutCompile (pmTextDisplayWindow);
		}
		else
		{
    		    myNoErrors = EdRun_Compile (pmTextDisplayWindow, 
						NULL, !pmExecute);
		}
    		if (myNoErrors && pmExecute)
    		{
    		    myRunAgain = EdRun_Execute (pmTextDisplayWindow, "", 
						0, pmWithArgs, pmStartWithStep);
		}

		pmTextDisplayWindow = myStWindowToRunNext;
		pmExecute = myStExecuteNext;
		pmWithArgs = myStWithArgsNext;
	    } while (myRunAgain);

	    stProgramIsRunning = FALSE;
	}
    }
} // EdDisp_Run


/************************************************************************/
/* EdDisp_SetBreakpointVisible						*/
/*									*/
/* After this is called, all the window *must* be resized.		*/
/************************************************************************/
void	EdDisp_SetBreakpointVisible (int pmVisible)
{
    if (pmVisible == SHOW_BREAKPOINTS)
    {
    	stDisplayBreakpoints = TRUE;
    }
    else if (pmVisible == HIDE_BREAKPOINTS)
    {
    	stDisplayBreakpoints = FALSE;
    }
    else if (pmVisible == TOGGLE_BREAKPOINTS)
    {
    	stDisplayBreakpoints = !stDisplayBreakpoints;
    }
    else
    {
    	// TW Assert Failure
    }
} // EdDisp_SetBreakpointVisible


/************************************************************************/
/* EdDisp_SetDisplayResizeMessage     					*/
/************************************************************************/
void	EdDisp_SetDisplayResizeMessage (BOOL pmDisplayResizeMessage)
{
    stDisplayResizeMessage = pmDisplayResizeMessage;
} // EdDisp_SetDisplayResizeMessage


/************************************************************************/
/* EdDisp_SetFindSelectionMenuItem     					*/
/*				     					*/
/* Set the find selection menu item. 					*/
/************************************************************************/
BOOL	EdDisp_SetFindSelectionMenuItem (HWND pmTextDisplayWindow, 
					 char *pmFindSelectionMenuItemString)
{
    TextDisplayWindowInfoPtr	myInfo;
    TextPtr			myTextPtr;
        
    // Get the selection, if possible
    myInfo = MyGetTextDisplayWindowInfo (pmTextDisplayWindow);
    myTextPtr = MyGetTextPtr (pmTextDisplayWindow);
    if (myTextPtr -> selTo == myTextPtr -> selFrom)
    {
    	myInfo -> selection [0] = 0;
    }
    else
    {
    	int	startPos, endPos;
    	
    	startPos = min (myTextPtr -> selFrom, myTextPtr -> selTo);
    	endPos = max (myTextPtr -> selFrom, myTextPtr -> selTo);
    	if (endPos - startPos >= MAX_SEL_SIZE)
    	{
    	    myInfo -> selection [0] = 0;
    	}
    	else
    	{
    	    int	cnt, pos = 0;
    	    
    	    for (cnt = startPos ; cnt < endPos ; cnt++)
    	    {
    	    	if (myTextPtr -> text [cnt] == '\n')
    	    	{
	    	    myInfo -> selection [0] = 0;
    	    	}
    	    	else
    	    	{
    	    	    myInfo -> selection [pos++] = myTextPtr -> text [cnt];
    	    	}
    	    }
    	    myInfo -> selection [pos] = 0;
    	}
    }
    
    if (pmFindSelectionMenuItemString != NULL)
    {
    	wsprintf (pmFindSelectionMenuItemString, "Find \"%s\"\tCtrl+L", 
            myInfo -> selection);
    }

    return (myInfo -> selection [0] != 0);
} // EdDisp_SetFindSelectionMenuItem


/************************************************************************/
/* EdDisp_SetTuringValues						*/
/*									*/
/* The pmWhere parameter is mostly used for debugging purposes.		*/
/************************************************************************/
void	EdDisp_SetTuringValues (HWND pmTextDisplayWindow, 
				WORD pmTuringFileNo, 
				char **pmTuringTextHandle, 
		    		int *pmTuringSizePtr, int pmWhere)
{
    TextDisplayWindowInfoPtr	myInfo;
            
    // Debugger message
    EdFail_AddMessage (NORMAL, "Set Turing Values - Window: %x - fileno: %d - "
    		       "sizeptr: %x - where: %d",
    		       pmTextDisplayWindow, pmTuringFileNo, pmTuringSizePtr, 
    		       pmWhere);

    // Set Turing file info
    myInfo = MyGetTextDisplayWindowInfo (pmTextDisplayWindow);
    myInfo -> turingFileNo = pmTuringFileNo;
    myInfo -> turingTextHandle = pmTuringTextHandle;
    myInfo -> turingSizePtr = pmTuringSizePtr;
    myInfo -> turingInitializedWhere = pmWhere;
} // EdDisp_SetTuringValues

		    		
/******************************/
/* Static callback procedures */
/******************************/
/************************************************************************/
/* MyTextDisplayWindowProcedure						*/
/*									*/
/* Callback procedure for the main editor window.			*/
/************************************************************************/
static LRESULT CALLBACK MyTextDisplayWindowProcedure (HWND pmTextDisplayWindow, 
						      UINT pmMessage, 
    						      WPARAM pmWParam, 
    						      LPARAM pmLParam)
{
    int	myReturnVal;
    
    // Log the windows messages
    if (stProperties.logWindowMessages)
    {
	EdLog_PrintWindowsMessage ("Text", pmTextDisplayWindow, pmMessage, 
	    pmWParam, pmLParam);
    }
    
    myReturnVal = 0;        
    if (MyCommonMessageHandler (pmTextDisplayWindow, pmMessage, pmWParam,
    				pmLParam, &myReturnVal))
    {
    	return myReturnVal;
    }

    switch (pmMessage)
    {    
	case WM_SIZE:
	    MyResizeTextDisplayWindow (pmTextDisplayWindow);
	    return 0;
        case WM_SETFOCUS:
	    MyCaretCreate (pmTextDisplayWindow);
	    EdSearch_SetDialogOwner (pmTextDisplayWindow);
	    EdFile_SetActiveDirectory (
	        MyGetTextDisplayWindowInfo (pmTextDisplayWindow) -> pathName,
	        FILE_PATH);
            return 0;
    }
    
    return DefWindowProc (pmTextDisplayWindow, pmMessage, pmWParam, 
        		  pmLParam);
} // MyTextDisplayWindowProcedure


/************************************************************************/
/* MyTextDisplayWindowProcedure						*/
/*									*/
/* Callback procedure for the main editor window.			*/
/************************************************************************/
static LRESULT CALLBACK MyTextDisplayMDIWindowProcedure (
						HWND pmTextDisplayWindow, 
						UINT pmMessage, 
    						WPARAM pmWParam, 
    						LPARAM pmLParam)
{
    int	myReturnVal;
        
    if (stProperties.logWindowMessages)
    {
	EdLog_PrintWindowsMessage ("MDText", pmTextDisplayWindow, pmMessage, 
	    pmWParam, pmLParam);
    }
    
    myReturnVal = 0;
    if (MyCommonMessageHandler (pmTextDisplayWindow, pmMessage, pmWParam,
    				pmLParam, &myReturnVal))
    {
    	return myReturnVal;
    }
    
    switch (pmMessage)
    {    
	case WM_SIZE:
	    MyResizeTextDisplayWindow (pmTextDisplayWindow);
	    break;
        case WM_SETFOCUS:
	    MyCaretCreate (pmTextDisplayWindow);
	    EdSearch_SetDialogOwner (pmTextDisplayWindow);
            break;
    }
    
    return DefMDIChildProc (pmTextDisplayWindow, pmMessage, pmWParam, pmLParam);
} // MyTextDisplayMDIWindowProcedure


/************************************************************************/
/* MyGetClassNameDialogProcedure					*/
/*						 			*/
/* Callback procedure for the Get Class Name dialog box.		*/
/************************************************************************/
static BOOL CALLBACK MyGetClassNameDialogProcedure (HWND pmDialog, 
			UINT pmMessage, WPARAM pmWParam, LPARAM pmLParam)
{
    HWND	myOK;
    char	myPackage [256];
    static char	*myResult;
    
    switch (pmMessage)
    {
    	case WM_INITDIALOG:
    	    myOK = GetDlgItem (pmDialog, IDOK);
	    EnableWindow (myOK, FALSE);
	    Ed_SetModalDialog (pmDialog); 
	    EdGUI_CentreDialogBox (pmDialog);
	    myResult = (char *) pmLParam;
    	    return TRUE;
    	case WM_COMMAND:
    	    switch (LOWORD (pmWParam))
    	    {
    	    	case IDOK:
    	    	    GetDlgItemText (pmDialog, CLASSNAME_EDITBOX, myResult, 
    	    	    	1024);
		    if (!EdJava_IsUpperCase (myResult [0]))
		    {
		    	int myReturn = EdGUI_Message1 (pmDialog,  
		    	    MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2, 
		    	    IDS_CLASS_NAME_TITLE, IDS_CLASS_NAME_BAD_STYLE);
		    	if (myReturn == IDYES)
		    	{
	    	    	    EndDialog (pmDialog, TRUE);
    		    	    return TRUE;
    		    	}
    		    	else
    		    	{
    		    	    break;
    		    	}
    		    }
    		    else if (EdJava_IsInJavaLang (myResult))
    		    {
		    	int myReturn = EdGUI_Message1 (pmDialog,  
		    	    MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2, 
		    	    IDS_CLASS_NAME_TITLE, IDS_CLASS_NAME_IN_LANG,
			    myResult);
		    	if (myReturn == IDYES)
		    	{
	    	    	    EndDialog (pmDialog, TRUE);
    		    	    return TRUE;
    		    	}
    		    	else
    		    	{
    		    	    break;
    		    	}
    		    }
    		    else if (EdJava_IsInOtherJavaPackage (myResult, myPackage))
    		    {
		    	int myReturn = EdGUI_Message1 (pmDialog,  
		    	    MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2, 
		    	    IDS_CLASS_NAME_TITLE, IDS_CLASS_NAME_IN_USE,
			    myResult, myPackage);
		    	if (myReturn == IDYES)
		    	{
	    	    	    EndDialog (pmDialog, TRUE);
    		    	    return TRUE;
    		    	}
    		    	else
    		    	{
    		    	    break;
    		    	}
    		    }
    		    else
		    {
	    	    	EndDialog (pmDialog, TRUE);
    		    	return TRUE;
    		    }
    	    	case IDCANCEL:
    	    	    EndDialog (pmDialog, FALSE);
    	    	    return TRUE;
    	    	case CLASSNAME_EDITBOX:
    	    	    if (HIWORD (pmWParam) == EN_UPDATE)
    	    	    {
    	    	    	myOK = GetDlgItem (pmDialog, IDOK);
			if (EdGUI_JavaIdentifierOnly (pmDialog, 
						      CLASSNAME_EDITBOX, 
				  		      CLASSNAME_WARNING_LABEL))
			{				  		      
    	    	    	    EnableWindow (myOK, TRUE);
     	    		}
     	    		else
    	    	    	{
    	    	    	    EnableWindow (myOK, FALSE);
    	    	    	}
    	    	    }
    	    	    break;
    	    } // switch
    	    break;
    } // switch
    return FALSE;
} // MyGetClassNameDialogProcedure


/************************************************************************/
/* MyCreateEXEDialogProcedure						*/
/************************************************************************/
static UINT CALLBACK 	MyCreateEXEDialogProcedure (HWND pmDialog, 
						    UINT pmMessage, 
    						    WPARAM pmWParam, 
    						    LPARAM pmLParam)
{
    static CreateEXEArgs	*myStCreateEXEArgs;
    
    switch (pmMessage)
    {
    	case WM_INITDIALOG:
    	    EdGUI_CentreDialogBox (pmDialog);
	    myStCreateEXEArgs = (CreateEXEArgs *) pmLParam;
            if (myStCreateEXEArgs -> closeWindowsOnTerminate)
	    {
            	CheckDlgButton (pmDialog, CLOSE_ON_TERMINATE, BST_CHECKED);
            	CheckDlgButton (pmDialog, LEAVE_OPEN, BST_UNCHECKED);
            }
            else
            {
            	CheckDlgButton (pmDialog, CLOSE_ON_TERMINATE, BST_UNCHECKED);
            	CheckDlgButton (pmDialog, LEAVE_OPEN, BST_CHECKED);
            }
	    if (myStCreateEXEArgs -> displayRunWithArgs)
	    {
            	CheckDlgButton (pmDialog, DISPLAY_R_W_A, BST_CHECKED);
            }
            else
            {
            	CheckDlgButton (pmDialog, DISPLAY_R_W_A, BST_UNCHECKED);
            }
	    if (myStCreateEXEArgs -> centerOutputWindow)
	    {
            	CheckDlgButton (pmDialog, CENTER_OUTPUT_WINDOW, BST_CHECKED);
            }
            else
            {
            	CheckDlgButton (pmDialog, CENTER_OUTPUT_WINDOW, BST_UNCHECKED);
            }
	    if (myStCreateEXEArgs -> stopUserClose)
	    {
            	CheckDlgButton (pmDialog, STOP_USER_CLOSE, BST_CHECKED);
            }
            else
            {
            	CheckDlgButton (pmDialog, STOP_USER_CLOSE, BST_UNCHECKED);
            }
    	    return TRUE;
    	case WM_COMMAND:
    	    switch (LOWORD (pmWParam))
    	    {
    	    	case IDOK:
    	    	    //
    	    	    // Copy the contents of the controls to the run args struct
    	    	    //    
    	    	    myStCreateEXEArgs -> closeWindowsOnTerminate =
    	    	    	(IsDlgButtonChecked (pmDialog, CLOSE_ON_TERMINATE) == 
    	    	    	    BST_CHECKED);
    	    	    myStCreateEXEArgs -> displayRunWithArgs = 
    	    	    	(IsDlgButtonChecked (pmDialog, DISPLAY_R_W_A) == 
    	    	    	    BST_CHECKED);
    	    	    myStCreateEXEArgs -> centerOutputWindow = 
    	    	    	(IsDlgButtonChecked (pmDialog, CENTER_OUTPUT_WINDOW) == 
    	    	    	    BST_CHECKED);
    	    	    myStCreateEXEArgs -> stopUserClose = 
    	    	    	(IsDlgButtonChecked (pmDialog, STOP_USER_CLOSE) == 
    	    	    	    BST_CHECKED);
   	    	    EndDialog (pmDialog, TRUE);
    	    	    return TRUE;
    	    	case IDCANCEL:
    	    	    EndDialog (pmDialog, FALSE);
    	    	    return TRUE;
		case CLOSE_ON_TERMINATE:
            	    CheckDlgButton (pmDialog, CLOSE_ON_TERMINATE, BST_CHECKED);
            	    CheckDlgButton (pmDialog, LEAVE_OPEN, BST_UNCHECKED);
		    break;
		case LEAVE_OPEN:
            	    CheckDlgButton (pmDialog, CLOSE_ON_TERMINATE, 
            	    		    BST_UNCHECKED);
            	    CheckDlgButton (pmDialog, LEAVE_OPEN, BST_CHECKED);
		    break;
    	    } // switch
    	    break;
    } // switch
    return FALSE;
} // MyCreateEXEDialogProcedure


/***********************************************************************/
/* MyGetProgramKindProcedure				               */
/***********************************************************************/
static BOOL CALLBACK 	MyGetProgramKindProcedure (HWND pmDialog, 
			    UINT pmMessage, WPARAM pmWParam, LPARAM pmLParam)
{
    switch (pmMessage)
    {
        case WM_INITDIALOG:
	    Ed_SetModalDialog (pmDialog); 
	    EdGUI_CentreDialogBox (pmDialog);
    	    return TRUE;
    	case WM_COMMAND:
    	    switch (LOWORD (pmWParam))
    	    {
        	case APPLICATION_BUTTON:
    		    EndDialog (pmDialog, APPLICATION);
   		    return TRUE;
    		case APPLET_BUTTON:
   		    EndDialog (pmDialog, APPLET);
   		    return TRUE;
    		case IDCANCEL:
   		    EndDialog (pmDialog, NO_PROGRAM);
   		    return TRUE;
	    } // switch
    } // switch
    return FALSE;
} // MyGetProgramKindProcedure

    					                 
/************************************************************************/
/* MyGetProgramNameDialogProcedure					*/
/*						 			*/
/* Callback procedure for the Get Class Name dialog box.		*/
/************************************************************************/
static BOOL CALLBACK MyGetProgramNameDialogProcedure (HWND pmDialog, 
			UINT pmMessage, WPARAM pmWParam, LPARAM pmLParam)
{
    HWND	myOK;
    static char	*myResult;
    
    switch (pmMessage)
    {
    	case WM_INITDIALOG:
    	    myOK = GetDlgItem (pmDialog, IDOK);
	    EnableWindow (myOK, FALSE);
	    Ed_SetModalDialog (pmDialog); 
	    EdGUI_CentreDialogBox (pmDialog);
	    myResult = (char *) pmLParam;
    	    return TRUE;
    	case WM_COMMAND:
    	    switch (LOWORD (pmWParam))
    	    {
    	    	case IDOK:
	    	    EndDialog (pmDialog, TRUE);
    		    return TRUE;
    	    	case IDCANCEL:
    	    	    EndDialog (pmDialog, FALSE);
    	    	    return TRUE;
    	    	case CLASSNAME_EDITBOX:
    	    	    if (HIWORD (pmWParam) == EN_UPDATE)
    	    	    {
    	    	        GetDlgItemText (pmDialog, CLASSNAME_EDITBOX, myResult,
    	    	    		        1024);
    	    	    	myOK = GetDlgItem (pmDialog, IDOK);
			if (strlen (myResult) > 0)
			{				  		      
    	    	    	    EnableWindow (myOK, TRUE);
     	    		}
     	    		else
    	    	    	{
    	    	    	    EnableWindow (myOK, FALSE);
    	    	    	}
    	    	    }
    	    	    break;
    	    } // switch
    	    break;
    } // switch
    return FALSE;
} // MyGetProgramNameDialogProcedure


/***********************************************************************/
/* MyGotoDialogProcedure				               */
/*						 		       */
/* Callback procedure for the Goto Line dialog box.  There is extra    */
/* code here because ES_NUMERIC doesn't actually work under WinNT 4.0! */
/***********************************************************************/
static BOOL CALLBACK MyGotoDialogProcedure (HWND pmDialog, UINT pmMessage, 
    WPARAM pmWParam, LPARAM pmLParam)
{
    HWND	myOK;
    char	myNumber [128];
    static int	*myResult;
    
    switch (pmMessage)
    {
    	case WM_INITDIALOG:
    	    myOK = GetDlgItem (pmDialog, IDOK);
	    EnableWindow (myOK, FALSE);
	    Ed_SetModalDialog (pmDialog); 
	    EdGUI_CentreDialogBox (pmDialog);
	    myResult = (int *) pmLParam;
    	    return TRUE;
    	case WM_COMMAND:
    	    switch (LOWORD (pmWParam))
    	    {
    	    	case IDOK:
    	    	    GetDlgItemText (pmDialog, GOTO_EDITBOX, myNumber, 
    	    	        sizeof (myNumber));
    	    	    *myResult = atoi (myNumber);
    	    	    EndDialog (pmDialog, TRUE);
    	    	    return TRUE;
    	    	case IDCANCEL:
    	    	    EndDialog (pmDialog, FALSE);
    	    	    return TRUE;
    	    	case GOTO_EDITBOX:
    	    	    if (HIWORD (pmWParam) == EN_UPDATE)
    	    	    {
    	    	    	EdGUI_DlgNumbersOnly (pmDialog, GOTO_EDITBOX);
    	    	    	
    	    	    	GetDlgItemText (pmDialog, GOTO_EDITBOX, myNumber, 
    	    	    	    sizeof (myNumber));
    	    	    	myOK = GetDlgItem (pmDialog, IDOK);
     	    	    	
    	    	    	if (myNumber [0] == 0)
    	    	    	    EnableWindow (myOK, FALSE);
    	    	    	else
    	    	    	    EnableWindow (myOK, TRUE);
    	    	    }
    	    	    break;
    	    } // switch
    	    break;
    } // switch
    return FALSE;
} // MyGotoDialogProcedure


/************************************************************************/
/* MyOpenFileOverDialogProcedure				       	*/
/************************************************************************/
static BOOL CALLBACK 	MyOpenFileOverDialogProcedure (HWND pmDialog, 
			    UINT pmMessage, WPARAM pmWParam, LPARAM pmLParam)
{
    switch (pmMessage)
    {
        case WM_INITDIALOG:
            SetDlgItemText (pmDialog, OPEN_OVER_MESSAGE, (char *) pmLParam);
	    Ed_SetModalDialog (pmDialog); 
	    EdGUI_CentreDialogBox (pmDialog);
    	    return TRUE;
    	case WM_COMMAND:
    	    switch (LOWORD (pmWParam))
    	    {
        	case IDYES:
    		    EndDialog (pmDialog, SAVE_AND_OPEN);
   		    return TRUE;
    		case IDNO:
   		    EndDialog (pmDialog, DONT_SAVE_AND_OPEN);
   		    return TRUE;
    		case IDCANCEL:
   		    EndDialog (pmDialog, IDCANCEL);
   		    return TRUE;
	    } // switch
    } // switch
    return FALSE;
} // MyOpenFileOverDialogProcedure

    					                 
/*********************/
/* Static procedures */
/*********************/
/************************************************************************/
/* MyAddFileSuffix							*/
/************************************************************************/
static void	MyAddFileSuffix (char *pmPathName, int pmSuffixType, 
				 char *pmFileSuffix)
{
    const char	*myFileName = EdFile_GetFileName (pmPathName);
    const char	*mySuffix = EdFile_GetFileSuffix (pmPathName);
    BOOL	mySuffixAlphabetic = FALSE;
    int	    	cnt;
    
    for (cnt = 0 ; cnt < (int) strlen (mySuffix) ; cnt++)
    {
    	if (isalpha (mySuffix [cnt]))
    	{
    	    mySuffixAlphabetic = TRUE;
    	    break;
    	}
    }
    
    if ((strlen (mySuffix) == 0) || (strlen (mySuffix) > 5) || 
        !mySuffixAlphabetic)
    {
        if (pmSuffixType == USE_JAVA_SUFFIX)
        {
       	    strcat (pmPathName, JAVA_SUFFIX);
       	}
        else if (pmSuffixType == USE_TURING_SUFFIX)
        {
	    BOOL    myLowerCase = FALSE, myUpperCase = FALSE;

	    for (cnt = 0 ; cnt < (int) strlen (myFileName) ; cnt++)
	    {
       	        if (('a' <= myFileName [cnt]) && (myFileName [cnt] <= 'z'))
		{
		    myLowerCase = TRUE;
		}
       	        if (('A' <= myFileName [cnt]) && (myFileName [cnt] <= 'Z'))
		{
		    myUpperCase = TRUE;
		}
	    }
	    if (myLowerCase || !myUpperCase)
       	    {
       	    	strcat (pmPathName, TURING_SUFFIX_LOWER);
       	    }
       	    else
       	    {
       	    	strcat (pmPathName, TURING_SUFFIX_UPPER);
       	    }
       	}
	else if (pmSuffixType == USE_MENU_SUFFIX)
	{
       	    strcat (pmPathName, pmFileSuffix);
	}
    }
} // MyAddFileSuffix


/************************************************************************/
/* MyAdjustContextualMenu						*/
/*									*/
/* Adjust the entry and enabled/disabled status of any 			*/
/* menu items prior to the user selecting a menu item  			*/
/************************************************************************/
static void	MyAdjustContextualMenu (HWND pmTextDisplayWindow)
{
    BOOL	mySelectionActive, myEmptyFile;
    BOOL	myIsKnownLanguage;
    int		myUndoStringResource, myRedoStringResource;
    BOOL	myEnable;
    char	myUndoMenuItemName [80];
        
    mySelectionActive = EdDisp_IsSelectionActive (pmTextDisplayWindow);
    myEmptyFile = EdDisp_IsEmptyFile (pmTextDisplayWindow);
    myIsKnownLanguage = EdDisp_IsKnownLanguage (pmTextDisplayWindow);
    myUndoStringResource = EdDisp_GetUndoStringResource (pmTextDisplayWindow);
    myRedoStringResource = EdDisp_GetRedoStringResource (pmTextDisplayWindow);

    // "Undo" off if no undoable action.  Also undo menu item name set.
    if (myUndoStringResource == 0)
    {
        EdGUI_LoadString (IDS_NORMAL_UNDO, myUndoMenuItemName, 
    	    sizeof (myUndoMenuItemName));
	ModifyMenu (stContextualMenu, COMMAND_UNDO, 
	    MF_BYCOMMAND | MF_STRING | MF_GRAYED, 
	    COMMAND_UNDO, myUndoMenuItemName); 
    }
    else
    {
    	EdGUI_LoadString (myUndoStringResource, myUndoMenuItemName, 
    	    sizeof (myUndoMenuItemName));
	ModifyMenu (stContextualMenu, COMMAND_UNDO, 
	     MF_BYCOMMAND | MF_STRING | MF_ENABLED, 
	    COMMAND_UNDO, myUndoMenuItemName); 
    }
    // "Redo" off if no undoable action.  Also redo menu item name set.
    if (myRedoStringResource == 0)
    {
        EdGUI_LoadString (IDS_REDO_BASE, myUndoMenuItemName, 
    	    sizeof (myUndoMenuItemName));
	ModifyMenu (stContextualMenu, COMMAND_REDO, 
	    MF_BYCOMMAND | MF_STRING | MF_GRAYED, 
	    COMMAND_REDO, myUndoMenuItemName); 
    }
    else    
    {
    	EdGUI_LoadString (myRedoStringResource, myUndoMenuItemName, 
    	    sizeof (myUndoMenuItemName));
	ModifyMenu (stContextualMenu, COMMAND_REDO, 
	    MF_BYCOMMAND | MF_STRING | MF_ENABLED, 
	    COMMAND_REDO, myUndoMenuItemName); 
    }
    // "Cut" off if no selection
    MyMenuItemEnable (stContextualMenu, COMMAND_CUT, mySelectionActive);
    // "Copy" off if no selection
    MyMenuItemEnable (stContextualMenu, COMMAND_COPY, mySelectionActive);
    // "Paste" off if empty paste buffer
    myEnable = IsClipboardFormatAvailable (CF_TEXT);
    MyMenuItemEnable (stContextualMenu, COMMAND_PASTE, myEnable);
    // "Select All" off if file empty
    MyMenuItemEnable (stContextualMenu, COMMAND_SELECT_ALL, !myEmptyFile);
    // "Comment range" off if no selection or if file type not known
    MyMenuItemEnable (stContextualMenu, COMMAND_COMMENT_RANGE, 
    		      myIsKnownLanguage && mySelectionActive);
    // "Uncomment range" off if no selection or if file type not known
    MyMenuItemEnable (stContextualMenu, COMMAND_UNCOMMENT_RANGE, 
    		      myIsKnownLanguage && mySelectionActive);
} // MyAdjustContextualMenu


/************************************************************************/
/* MyCaretCreate							*/
/************************************************************************/
static void	MyCaretCreate (HWND pmTextDisplayWindow)
{
    TextDisplayWindowInfoPtr	myInfo;
    int				myCaretWidth;
    
    if (stProperties.caretWidth == 3)
    {
	myCaretWidth = stTextFontCharWidth;
    }
    else
    {
	myCaretWidth = stProperties.caretWidth;
    }

    CreateCaret (pmTextDisplayWindow, NULL, myCaretWidth, stTextFontCharHeight);
    myInfo = MyGetTextDisplayWindowInfo (pmTextDisplayWindow);
    myInfo -> caretCreated = TRUE;
    if (myInfo -> text -> selFrom == myInfo -> text -> selTo)
    {
        MyCaretDisplay (pmTextDisplayWindow);
    }
    MyDisplayText (pmTextDisplayWindow, FALSE, ALL_LINES, ALL_LINES);
} // MyCaretCreate


/************************************************************************/
/* MyCaretDisplay							*/
/************************************************************************/
static void	MyCaretDisplay (HWND pmTextDisplayWindow)
{
    TextDisplayWindowInfoPtr	myInfo;
            
    // Create the ddit window information
    myInfo = MyGetTextDisplayWindowInfo (pmTextDisplayWindow);
    	
    if (!myInfo -> caretCreated) return;

    if (myInfo -> text -> selFrom == myInfo -> text -> selTo)
    {
        int	myLine, myCol, myX, myY;
            	
        EdText_GetSelectionLocation (myInfo -> text, &myLine, &myCol);
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
	    ShowCaret (pmTextDisplayWindow);
        }
    }
    else
    {
    	if (myInfo -> caretShowing)
    	{
            myInfo -> caretShowing = FALSE;
    	    HideCaret (pmTextDisplayWindow);
    	}
    }
} // MyCaretDisplay


/************************************************************************/
/* MyCaretHide								*/
/************************************************************************/
static void	MyCaretHide (HWND pmTextDisplayWindow)
{
    TextDisplayWindowInfoPtr	myInfo;
            
    // Get the text display window information
    myInfo = MyGetTextDisplayWindowInfo (pmTextDisplayWindow);
    	
    if (myInfo -> caretShowing)
    {
        myInfo -> caretShowing = FALSE;
    	HideCaret (pmTextDisplayWindow);
    }
} // MyCaretHide


/************************************************************************/
/* MyCaretMove								*/
/************************************************************************/
static void	MyCaretMove (HWND pmTextDisplayWindow)
{
    TextDisplayWindowInfoPtr	myInfo;
    int				myLine, myCol, myX, myY;
            
    // Create the ddit window information
    myInfo = MyGetTextDisplayWindowInfo (pmTextDisplayWindow);
            	
    EdText_GetSelectionLocation (myInfo -> text, &myLine, &myCol);
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
/* MyCheckForRevert							*/
/************************************************************************/
static BOOL	MyCheckForRevert (HWND pmTextDisplayWindow)
{
    TextDisplayWindowInfoPtr	myInfo;
    TextPtr			myTextPtr;
    const char			*myName;
    int				myResult;

    myInfo = MyGetTextDisplayWindowInfo (pmTextDisplayWindow);
    myTextPtr = myInfo -> text;

    // If the file has not been modified, then okay to close    
    if (!myTextPtr -> isDirty)
    {
    	return TRUE;
    }

    // Get a pathname    
    if (myInfo -> pathName != NULL)
    {
    	myName = EdFile_GetFileName (myInfo -> pathName);
    }
    else
    {
    	myName = "file";
    }

    myResult = EdGUI_Message1 (pmTextDisplayWindow, 
    	MB_OKCANCEL | MB_ICONWARNING, IDS_APPLICATION_NAME, 
    	IDS_REVERT_MESSAGE, myName);

    // Press Cancel, then don't close
    if (myResult == IDCANCEL)
    {
	return FALSE;
    }

    return TRUE;
} // MyCheckForRevert


/************************************************************************/
/* MyCommandHandler							*/
/************************************************************************/
static BOOL	MyCommandHandler (HWND pmTextDisplayWindow, short pmCommand)
{	
    TextDisplayWindowInfoPtr	myInfo;
    BOOL			myMoved;
    int				myStatus;

    myInfo = MyGetTextDisplayWindowInfo (pmTextDisplayWindow);
    
    // Handle mark commands
    if ((COMMAND_MARK_BASE <= pmCommand) && 
        (pmCommand <= COMMAND_LAST_MARK_COMMAND))
    {
    	MyMarkMoveToLocation (pmTextDisplayWindow, 
    	    pmCommand - COMMAND_MARK_BASE);
	EdWin_ShowCursorPosition (pmTextDisplayWindow);
    	return TRUE;
    }

    // Handle recent files commands
    if ((COMMAND_RECENT_BASE <= pmCommand) &&
        (pmCommand <= COMMAND_LAST_RECENT_COMMAND))
    {
	if (myInfo -> beginnerMode ||
	    ((myInfo -> unnamedNumber == 1) && 
	     EdDisp_IsEmptyFile (pmTextDisplayWindow)))
	{
	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_RECENT");
	    EdFail_AddMessage (NORMAL, 
	    		       "Open Recent File (D) - Window: %x - Recent: %d",
	    		       pmTextDisplayWindow, 
	    		       pmCommand - COMMAND_RECENT_BASE);

    	    MyFileOpen (pmTextDisplayWindow, 
    		    EdRec_GetPathName (pmCommand - COMMAND_RECENT_BASE), NULL,
    		    CHECK_FOR_CLOSE);
    	    return TRUE;
	}
	return FALSE;
    }
    
    // Handle new skeleton code commands
    if ((COMMAND_NEW_BASE <= pmCommand) &&
        (pmCommand <= COMMAND_LAST_NEW_COMMAND))
    {
	if (myInfo -> beginnerMode ||
	    ((myInfo -> unnamedNumber == 1) && 
	     EdDisp_IsEmptyFile (pmTextDisplayWindow)))
	{
	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_NEW");
	    EdFail_AddMessage (NORMAL, 
	    		       "Open New File (D) - Window: %x - New: %d",
	    		       pmTextDisplayWindow, 
	    		       pmCommand - COMMAND_NEW_BASE + 1);

	    MyFileNew (pmTextDisplayWindow, pmCommand - COMMAND_NEW_BASE + 1);
    	    return TRUE;
    	}
	return FALSE;
    }
    
    // Handle insert skeleton code commands
    if ((COMMAND_INSERT_BASE <= pmCommand) &&
        (pmCommand <= COMMAND_LAST_INSERT_COMMAND))
    {
	EdFail_AddMessage (NORMAL, "CMD: COMMAND_INSERT");
	EdFail_AddMessage (NORMAL, "Insert Text - Window: %x - Insert: %d",
	    		   pmTextDisplayWindow, 
	    		   pmCommand - COMMAND_INSERT_BASE + 1);

	MyEditInsert (pmTextDisplayWindow, pmCommand - COMMAND_INSERT_BASE + 1);

    	return TRUE;
    }
    
    switch (pmCommand)
    {
    	
	case COMMAND_MOVE_COLUMN_LEFT:
    	case COMMAND_SELECT_COLUMN_LEFT:
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_MOVE/SELECT_COLUMN_LEFT");
    	    myStatus = EdText_MoveColumnLeft (
    	        MyGetTextPtr (pmTextDisplayWindow),
    	        pmCommand == COMMAND_SELECT_COLUMN_LEFT);
    	    break;
    	case COMMAND_MOVE_COLUMN_RIGHT:
    	case COMMAND_SELECT_COLUMN_RIGHT:
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_MOVE/SELECT_COLUMN_RIGHT");
    	    myStatus = EdText_MoveColumnRight (
    	        MyGetTextPtr (pmTextDisplayWindow),
    	        pmCommand == COMMAND_SELECT_COLUMN_RIGHT);
    	    break;
    	case COMMAND_MOVE_LINE_UP:
    	case COMMAND_SELECT_LINE_UP:
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_MOVE/SELECT_LINE_UP");
    	    myStatus = EdText_MoveLineUp (MyGetTextPtr (pmTextDisplayWindow),
    	        pmCommand == COMMAND_SELECT_LINE_UP);
    	    break;
       	case COMMAND_MOVE_LINE_DOWN:
    	case COMMAND_SELECT_LINE_DOWN:
    	    // If at the end of the program, scroll down a line
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_MOVE/SELECT_LINE_DOWN");
    	    myStatus = EdText_MoveLineDown (MyGetTextPtr (pmTextDisplayWindow), 
    	        &myMoved, pmCommand == COMMAND_SELECT_LINE_DOWN);
    	    if (!myMoved)
    	    {
    	    	MyScrollHandlerVert (pmTextDisplayWindow, SB_LINEDOWN, 0);
    	    }
    	    break;
	case COMMAND_MOVE_LINE_BEGIN:
	case COMMAND_SELECT_LINE_BEGIN:
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_MOVE/SELECT_LINE_BEGIN");
    	    myStatus = EdText_MoveLineBegin (MyGetTextPtr (pmTextDisplayWindow),
    	        pmCommand == COMMAND_SELECT_LINE_BEGIN);
	    break;
	case COMMAND_MOVE_LINE_END:
	case COMMAND_SELECT_LINE_END:
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_MOVE/SELECT_LINE_END");
    	    myStatus = EdText_MoveLineEnd (MyGetTextPtr (pmTextDisplayWindow),
    	        pmCommand == COMMAND_SELECT_LINE_END);
	    break;
	case COMMAND_MOVE_WORD_PREV:
	case COMMAND_SELECT_WORD_PREV:
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_MOVE/SELECT_WORD_PREV");
    	    myStatus = EdText_MoveWordPrev (MyGetTextPtr (pmTextDisplayWindow),
    	        pmCommand == COMMAND_SELECT_WORD_PREV);
	    break;
	case COMMAND_MOVE_WORD_NEXT:
	case COMMAND_SELECT_WORD_NEXT:
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_MOVE/SELECT_WORD_NEXT");
    	    myStatus = EdText_MoveWordNext (MyGetTextPtr (pmTextDisplayWindow),
    	        pmCommand == COMMAND_SELECT_WORD_NEXT);
	    break;
	case COMMAND_MOVE_PAGE_HALF_UP:
	case COMMAND_SELECT_PAGE_HALF_UP:
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_MOVE/SELECT_PAGE_HALF_UP");
    	    myStatus = EdText_MovePageUp (myInfo -> text, 
    	        myInfo -> windowHeight / 2,
    	        pmCommand == COMMAND_SELECT_PAGE_HALF_UP);
    	    break;
	case COMMAND_MOVE_PAGE_HALF_DOWN:
	case COMMAND_SELECT_PAGE_HALF_DOWN:
   	    EdFail_AddMessage (NORMAL, 
   	    		       "CMD: COMMAND_MOVE/SELECT_PAGE_HALF_DOWN");
    	    myStatus = EdText_MovePageDown (myInfo -> text, 
    	        myInfo -> windowHeight / 2, 
    	        &myMoved, pmCommand == COMMAND_SELECT_PAGE_HALF_DOWN);
    	    if (!myMoved)
    	    {
    	    	MyScrollHandlerVert (pmTextDisplayWindow, SB_PAGEDOWN, 0);
    	    }
    	    break;
	case COMMAND_MOVE_PAGE_UP:
	case COMMAND_SELECT_PAGE_UP:
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_MOVE/SELECT_PAGE_UP");
    	    myStatus = EdText_MovePageUp (myInfo -> text, 
    	        myInfo -> windowHeight - 2,
    	        pmCommand == COMMAND_SELECT_PAGE_UP);
    	    break;
	case COMMAND_MOVE_PAGE_DOWN:
	case COMMAND_SELECT_PAGE_DOWN:
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_MOVE/SELECT_PAGE_DOWN");
    	    myStatus = EdText_MovePageDown (myInfo -> text, 
    	        myInfo -> windowHeight - 2, &myMoved, 
    	        pmCommand == COMMAND_SELECT_PAGE_DOWN);
    	    if (!myMoved)
    	    {
    	    	MyScrollHandlerVert (pmTextDisplayWindow, SB_PAGEDOWN, 0);
    	    }
    	    break;
	case COMMAND_MOVE_PROGRAM_TOP:
	case COMMAND_SELECT_PROGRAM_TOP:
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_MOVE/SELECT_PROGRAM_TOP");
    	    myStatus = EdText_MoveProgramTop (
    	        MyGetTextPtr (pmTextDisplayWindow),
    	        pmCommand == COMMAND_SELECT_PROGRAM_TOP);
	    break;
	case COMMAND_MOVE_PROGRAM_BOTTOM:
	case COMMAND_SELECT_PROGRAM_BOTTOM:
   	    EdFail_AddMessage (NORMAL, 
   	    		       "CMD: COMMAND_MOVE/SELECT_PROGRAM_BOTTOM");
    	    myStatus = EdText_MoveProgramBottom (
    	        MyGetTextPtr (pmTextDisplayWindow),
    	        pmCommand == COMMAND_SELECT_PROGRAM_BOTTOM);
	    break;

	// Special keystroke commands
	case COMMAND_DEL:
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_DEL");
	    MyKeystrokeHandler (pmTextDisplayWindow, 0, DELETE_KEY);
	    break;
	case COMMAND_BACKTAB:
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_BACKTAB");
	    MyKeystrokeHandler (pmTextDisplayWindow, 0, BACKTAB_KEY);
	    break;
	    		    
	// Menu commands	    
	case COMMAND_NEW_UNNAMED:
	    if (!myInfo -> beginnerMode)
	    {
	    	return FALSE;
	    }
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_NEW_UNNAMED");
	    MyFileNew (pmTextDisplayWindow, 0);
	    break;
    	case COMMAND_OPEN_FILE:
	    if (myInfo -> beginnerMode ||
	        ((myInfo -> unnamedNumber == 1) && 
	         EdDisp_IsEmptyFile (pmTextDisplayWindow)))
	    {
   	        EdFail_AddMessage (NORMAL, "CMD: COMMAND_OPEN_FILE (D)");
    	        MyFileOpen (pmTextDisplayWindow, NULL, NULL, CHECK_FOR_CLOSE);
	    }
	    else
	    {
    	        return FALSE;
	    }
    	    break;
    	case COMMAND_SAVE_FILE:
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_SAVE_FILE");
    	    if (EdDisp_IsAssociated (pmTextDisplayWindow) ||
    	        !EdDisp_IsEmptyFile (pmTextDisplayWindow))
    	    {
    	    	MyFileSave (pmTextDisplayWindow, SAVE_COMMAND, SAVE_WHOLE_FILE);
    	    }
    	    else
    	    {
    	    	EdGUI_Message1 (pmTextDisplayWindow, 0, IDS_EMPTY_FILE_TITLE,
    	    	    IDS_NOTHING_TO_SAVE);
	    }    	    	    
	    break;
	case COMMAND_SAVEAS_FILE:
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_SAVEAS_FILE");
    	    if (EdDisp_IsAssociated (pmTextDisplayWindow) ||
    	        !EdDisp_IsEmptyFile (pmTextDisplayWindow))
    	    {
    	    	MyFileSave (pmTextDisplayWindow, SAVE_AS_COMMAND, 
    	    	    SAVE_WHOLE_FILE);
    	    }
    	    else
    	    {
    	    	EdGUI_Message1 (pmTextDisplayWindow, 0, IDS_EMPTY_FILE_TITLE,
    	    	    IDS_NOTHING_TO_SAVE);
	    }    	    	    
	    break;
	case COMMAND_SAVE_SELECTION:
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_SAVE_SELECTION");
	    if (EdDisp_IsSelectionActive (pmTextDisplayWindow))
	    {
    	    	MyFileSave (pmTextDisplayWindow, SAVE_AS_COMMAND, 
    	    	    SAVE_SELECTION);
    	    }
    	    else
    	    {
    	    	EdGUI_Message1 (pmTextDisplayWindow, 0, IDS_NO_SELECTION_TITLE,
    	    	    IDS_NO_SELECTION_TO_SAVE);
	    }    	    	    
	    break;
	case COMMAND_REVERT_FILE:
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_REVERT_FILE");
    	    if (EdDisp_IsAssociated (pmTextDisplayWindow))
    	    {
	    	if (EdDisp_IsDirtyFile (pmTextDisplayWindow))
	    	{
	    	    MyFileRevert (pmTextDisplayWindow);
	    	}
	    	else
	    	{
    	    	    EdGUI_Message1 (pmTextDisplayWindow, 0,
    	    	    	IDS_NOT_CHANGED_TITLE, IDS_FILE_HAS_NOT_BEEN_CHANGED);
	    	}
	    }
    	    else
    	    {
    	    	EdGUI_Message1 (pmTextDisplayWindow, 0, IDS_NO_ASSOC_FILE_TITLE,
    	    	    IDS_FILE_HAS_NO_ASSOCIATED_FILE);
	    }    	    	    
	    break;
	case COMMAND_PRINT_FILE:
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_PRINT_FILE");
    	    if (!EdDisp_IsEmptyFile (pmTextDisplayWindow))
    	    {
	    	EdPrint_PrintEditorWindow (
	    	    MyGetEditWindow (pmTextDisplayWindow),
	            MyGetTextPtr (pmTextDisplayWindow), myInfo -> pathName, 
	            myInfo -> unnamedNumber, myInfo -> isExample,
	            myInfo -> exampleName, myInfo -> syntaxColour, 
	            PROGRAM_FILE);
	    }	            
    	    else
    	    {
    	    	EdGUI_Message1 (pmTextDisplayWindow, 0, IDS_EMPTY_FILE_TITLE,
    	    	    IDS_NOTHING_TO_PRINT);
	    }    	    	    
	    break;
	case COMMAND_FIX_MAIN:
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_FIX_MAIN");
    	    if (!EdDisp_IsEmptyFile (pmTextDisplayWindow))
    	    {
	    	HWND		myTextDisplayWindow;
	    	FilePath	myPathName;
	    	
	    	myTextDisplayWindow = EdRun_GetMainWindow ();
	    	if (myTextDisplayWindow != NULL)
	    	{
		    MyGetTextDisplayWindowInfo (pmTextDisplayWindow) -> isMain = 
		        FALSE;
		}
	    	EdRun_FixMain (pmTextDisplayWindow);
	    	MyGetTextDisplayWindowInfo (pmTextDisplayWindow) -> isMain = 
	    	    TRUE;
		EdDisp_GetPathForDisplay (pmTextDisplayWindow, USE_FILENAME, 
    	    	    DONT_SHOW_DIRTY_MARK, myPathName);
    	    	EdWin_ShowStatus1 (pmTextDisplayWindow, IDS_FIX_MAIN_STATUS,
	    			   myPathName);
	    }
    	    else
	    {
    	    	EdGUI_Message1 (pmTextDisplayWindow, 0, IDS_EMPTY_FILE_TITLE,
    	    	    IDS_NOTHING_TO_FIX_AS_MAIN);
	    }    	    	    
	    break;	    
	case COMMAND_FLOAT_MAIN:
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_FLOAT_MAIN");
	    if (EdRun_GetMainWindow () != NULL)
	    {
	    	HWND	myTextDisplayWindow;
	    	
	    	myTextDisplayWindow = EdRun_GetMainWindow ();
	    	if (myTextDisplayWindow != NULL)
	    	{
		    MyGetTextDisplayWindowInfo (pmTextDisplayWindow) -> isMain = 
		        FALSE;
		}
		
	    	EdRun_FloatMain ();
	    	
	    	EdWin_ShowStatus1 (pmTextDisplayWindow, IDS_FLOAT_MAIN_STATUS);
	    }
	    else
	    {
    	    	EdGUI_Message1 (pmTextDisplayWindow, 0, IDS_NO_MAIN_PROG_TITLE,
    	    	    IDS_NO_MAIN_PROG_SELECTED);
	    }    	    	    
	    break;	    
	case COMMAND_UNDO:
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_UNDO");
	    if (EdDisp_GetUndoStringResource (pmTextDisplayWindow) != 0)
	    {
	    	MyEditUndo (pmTextDisplayWindow, UNDO);
	    }
	    else
	    {
    	    	EdGUI_Message1 (pmTextDisplayWindow, 0, IDS_NO_UNDO_TITLE,
    	    	    IDS_NOTHING_TO_UNDO);
	    }    	    	    
	    break;
	case COMMAND_REDO:
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_REDO");
	    if (EdDisp_GetRedoStringResource (pmTextDisplayWindow) != 0)
	    {
	    	MyEditUndo (pmTextDisplayWindow, REDO);
	    }
	    else
	    {
    	    	EdGUI_Message1 (pmTextDisplayWindow, 0, IDS_NO_UNDO_TITLE,
    	    	    IDS_NOTHING_TO_UNDO);
	    }    	    	    
	    break;
	case COMMAND_CUT:
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_CUT");
	    if (EdDisp_IsSelectionActive (pmTextDisplayWindow))
	    {
	    	MyEditCutCopy (pmTextDisplayWindow, CUT);
    	    }
    	    else
    	    {
    	    	EdGUI_Message1 (pmTextDisplayWindow, 0, IDS_NO_SELECTION_TITLE,
    	    	    IDS_NO_SELECTION_TO_CUT);
	    }    	    	    
	    break;
	case COMMAND_COPY:
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_COPY");
	    if (EdDisp_IsSelectionActive (pmTextDisplayWindow))
	    {
	    	MyEditCutCopy (pmTextDisplayWindow, COPY);
    	    }
    	    else
    	    {
    	    	EdGUI_Message1 (pmTextDisplayWindow, 0, IDS_NO_SELECTION_TITLE,
    	    	    IDS_NO_SELECTION_TO_COPY);
	    }    	    	    
	    break;
	case COMMAND_PASTE:
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_PASTE");
	    if (IsClipboardFormatAvailable (CF_TEXT))
	    {
	    	MyEditPaste (pmTextDisplayWindow);
	    }
    	    else
    	    {
    	    	EdGUI_Message1 (pmTextDisplayWindow, 0, IDS_NO_CLIPBOARD_TITLE,
    	    	    IDS_NO_TEXT_IN_CLIPBOARD);
	    }    	    	    
	    break;
	case COMMAND_SELECT_ALL:
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_SELECT_ALL");
    	    if (!EdDisp_IsEmptyFile (pmTextDisplayWindow))
    	    {
	    	MyEditSelectAll (pmTextDisplayWindow);
	    }
    	    else
	    {
    	    	EdGUI_Message1 (pmTextDisplayWindow, 0, IDS_EMPTY_FILE_TITLE,
    	    	    IDS_NOTHING_TO_SELECT);
	    }    	    	    
	    break;
	case COMMAND_COMMENT_RANGE:
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_COMMENT_RANGE");
	    if (!EdDisp_IsSelectionActive (pmTextDisplayWindow))
	    {
    	    	EdGUI_Message1 (pmTextDisplayWindow, 0, IDS_NO_SELECTION_TITLE,
    	    	    IDS_NO_SELECTION_TO_COMMENT);
    	    }
    	    else if (!EdDisp_IsKnownLanguage (pmTextDisplayWindow))
    	    {
    	    	EdGUI_Message1 (pmTextDisplayWindow, 0, 
    	    	    IDS_UNKNOWN_FILE_TYPE_TITLE,
    	    	    IDS_UNKNOWN_COMMENT_FOR_FILE_TYPE,
    	    	    EdFile_GetFileSuffix (myInfo -> pathName));
    	    }
    	    else
    	    {
	    	MyEditCommentRange (pmTextDisplayWindow, COMMENT);
    	    }
	    break;
	case COMMAND_UNCOMMENT_RANGE:
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_UNCOMMENT_RANGE");
	    if (!EdDisp_IsSelectionActive (pmTextDisplayWindow))
	    {
    	    	EdGUI_Message1 (pmTextDisplayWindow, 0, IDS_NO_SELECTION_TITLE,
    	    	    IDS_NO_SELECTION_TO_UNCOMMENT);
    	    }
    	    else if (!EdDisp_IsKnownLanguage (pmTextDisplayWindow))
    	    {
    	    	EdGUI_Message1 (pmTextDisplayWindow, 0, 
    	    	    IDS_UNKNOWN_FILE_TYPE_TITLE,
    	    	    IDS_UNKNOWN_COMMENT_FOR_FILE_TYPE,
    	    	    EdFile_GetFileSuffix (myInfo -> pathName));
    	    }
    	    else
    	    {
	    	MyEditCommentRange (pmTextDisplayWindow, UNCOMMENT);
	    }    	    	    
	    break;
	case COMMAND_COMPLETE_STRUCT:
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_COMPLETE_STRUCT");
   	    MyEditCompleteStructure (pmTextDisplayWindow);
	    break;
	case COMMAND_INDENT:
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_INDENT");
    	    if (EdDisp_IsEmptyFile (pmTextDisplayWindow))
    	    {
    	    	EdGUI_Message1 (pmTextDisplayWindow, 0, IDS_EMPTY_FILE_TITLE,
    	    	    IDS_NOTHING_TO_INDENT);
	    }
    	    else if (!EdDisp_IsKnownLanguage (pmTextDisplayWindow))
    	    {
    	    	EdGUI_Message1 (pmTextDisplayWindow, 0, 
    	    	    IDS_UNKNOWN_FILE_TYPE_TITLE,
    	    	    IDS_UNKNOWN_INDENT_FOR_FILE_TYPE,
    	    	    EdFile_GetFileSuffix (myInfo -> pathName));
	    }    	    	    		
    	    else
	    {
	    	MyEditIndent (pmTextDisplayWindow);
	    }    	    	    
	    break;
	case COMMAND_FIND:
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_FIND");
    	    if (!EdDisp_IsEmptyFile (pmTextDisplayWindow))
    	    {
	    	// Debug messages
    		EdFail_AddMessage (NORMAL, "Find Text Dialog - Window: %x", 
    				   pmTextDisplayWindow);
        
    	    	EdWin_ShowStatus (pmTextDisplayWindow, "Find Text");
		if (stProperties.findUsesSelection)
		{
		    if (EdDisp_SetFindSelectionMenuItem (pmTextDisplayWindow, 
		    					 NULL))
		    {
		    	EdSearch_ShowFindDialog (pmTextDisplayWindow,
		    	    MyGetTextDisplayWindowInfo (pmTextDisplayWindow) 
		    	    					-> selection);
		    }
		    else
		    {
		    	EdSearch_ShowFindDialog (pmTextDisplayWindow, NULL);
		    }
		}
		else
		{
		    EdSearch_ShowFindDialog (pmTextDisplayWindow, NULL);
		}
	    }
    	    else
	    {
    	    	EdGUI_Message1 (pmTextDisplayWindow, 0, IDS_EMPTY_FILE_TITLE,
    	    	    IDS_NOTHING_TO_SEARCH);
	    }    	    	    
	    break;
	case COMMAND_FIND_NEXT:
	case COMMAND_FIND_PREV:
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_FIND_NEXT/PREV");
    	    if (!EdDisp_IsEmptyFile (pmTextDisplayWindow))
    	    {
	    	if (!EdSearch_IsFindStringEmpty ())
	    	{
	    	    MyFindNext (pmTextDisplayWindow, 
	    	    		pmCommand == COMMAND_FIND_NEXT);
	    	}
	    	else
	    	{
    	    	    EdGUI_Message1 (pmTextDisplayWindow, 0, 
    	    	    	IDS_NO_SEARCH_TEXT_TITLE, IDS_NO_SEARCH_TEXT);
	    	}
	    }
    	    else
	    {
    	    	EdGUI_Message1 (pmTextDisplayWindow, 0, IDS_EMPTY_FILE_TITLE,
    	    	    IDS_NOTHING_TO_SEARCH);
	    }    	    	    
	    break;
	case COMMAND_FIND_SELECTION:
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_FIND_SELECTION");
    	    if (!EdDisp_IsEmptyFile (pmTextDisplayWindow))
    	    {
    	    	// TW - No checking to see if there's a findable selection
	    	MyFindSelection (pmTextDisplayWindow);
	    }
    	    else
	    {
    	    	EdGUI_Message1 (pmTextDisplayWindow, 0, IDS_EMPTY_FILE_TITLE,
    	    	    IDS_NOTHING_TO_SEARCH);
	    }    	    	    
	    break;	        
	case COMMAND_REPLACE:
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_REPLACE");
    	    if (!EdDisp_IsEmptyFile (pmTextDisplayWindow))
    	    {
		if (stProperties.findUsesSelection)
		{
		    if (EdDisp_SetFindSelectionMenuItem (pmTextDisplayWindow, 
		    					 NULL))
		    {
		    	EdSearch_ShowReplaceDialog (pmTextDisplayWindow,
		    	    MyGetTextDisplayWindowInfo (pmTextDisplayWindow) 
		    	    					-> selection);
		    }
		    else
		    {
		    	EdSearch_ShowReplaceDialog (pmTextDisplayWindow, NULL);
		    }
		}
		else
		{
		    EdSearch_ShowReplaceDialog (pmTextDisplayWindow, NULL);
		}
	    }
    	    else
	    {
    	    	EdGUI_Message1 (pmTextDisplayWindow, 0, IDS_EMPTY_FILE_TITLE,
    	    	    IDS_NOTHING_TO_SEARCH);
	    }    	    	    
	    break;
	case COMMAND_FIND_NEXT_ERROR:
	    // TW Add check for errors
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_FIND_NEXT_ERROR");
	    EdErr_FindNextError (pmTextDisplayWindow, MESSAGE_IF_NO_ERROR,
				 ACTIVATE_SOURCE_WINDOW);
	    break;
	case COMMAND_FIND_MATCH_BRACE:
	    // TW Add check for errors
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_FIND_MATCH_BRACE");
	    MyFindMatchingBrace (pmTextDisplayWindow);
	    break;
	case COMMAND_GOTO_LINE:
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_GOTO_LINE");
    	    if (!EdDisp_IsEmptyFile (pmTextDisplayWindow))
    	    {
	    	MyGotoLine (pmTextDisplayWindow);
	    }
    	    else
	    {
    	    	EdGUI_Message1 (pmTextDisplayWindow, 0, IDS_EMPTY_FILE_TITLE,
    	    	    IDS_NOTHING_TO_GOTO);
	    }    	    	    
	    break;
	case COMMAND_RUN:
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_RUN");
	    if ((EdRun_GetMainWindow () != NULL) || 
	        (EdRun_GetMainPathName () != NULL) || 
	        (!EdDisp_IsEmptyFile (pmTextDisplayWindow)))
	    {
	    	EdDisp_Run (pmTextDisplayWindow, COMPILE_AND_EXECUTE, 
	    		    NO_RUN_ARGS_DIALOG, RUN_CONTINUOUSLY);
	    }
    	    else
	    {
    	    	EdGUI_Message1 (pmTextDisplayWindow, 0, IDS_EMPTY_FILE_TITLE,
    	    	    IDS_NOTHING_TO_RUN);
	    }    	    	    
	    break;
	case COMMAND_RUN_ARGS:
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_RUN_ARGS");
	    if ((EdRun_GetMainWindow () != NULL) || 
	        (EdRun_GetMainPathName () != NULL) || 
	        (!EdDisp_IsEmptyFile (pmTextDisplayWindow)))
	    {
	    	EdDisp_Run (pmTextDisplayWindow, COMPILE_AND_EXECUTE, 
	    		    RUN_ARGS_DIALOG, RUN_CONTINUOUSLY);
	    }
    	    else
	    {
    	    	EdGUI_Message1 (pmTextDisplayWindow, 0, IDS_EMPTY_FILE_TITLE,
    	    	    IDS_NOTHING_TO_RUN);
	    }    	    	    
	    break;
	case COMMAND_COMPILE:
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_COMPILE");
	    if ((EdRun_GetMainWindow () != NULL) || 
	        (EdRun_GetMainPathName () != NULL) || 
	        (!EdDisp_IsEmptyFile (pmTextDisplayWindow)))
	    {
	    	EdDisp_Run (pmTextDisplayWindow, COMPILE_ONLY, 
	    		    NO_RUN_ARGS_DIALOG, RUN_CONTINUOUSLY);
	    }
    	    else
	    {
    	    	EdGUI_Message1 (pmTextDisplayWindow, 0, IDS_EMPTY_FILE_TITLE,
    	    	    IDS_NOTHING_TO_RUN);
	    }    	    	    
	    break;
        case COMMAND_CREATE_EXE:
	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_CREATE_EXE");
    	    MyCreateEXE (pmTextDisplayWindow);
	    break;
	case COMMAND_KEYWORD_PROTO:
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_KEYWORD_PROTO");
	    EdTur_KeywordLookup (pmTextDisplayWindow, 
	    			 MyGetTextPtr (pmTextDisplayWindow));
	    break;
	case COMMAND_KEYWORD_REF:
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_KEYWORD_REF");
	    EdTur_KeywordDisplayPage (pmTextDisplayWindow, 
	    			      MyGetTextPtr (pmTextDisplayWindow));
	    break;
	    	    			 
	// These commands come from the debugger controls.
	case COMMAND_DEBUG_STEP:
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_DEBUG_STEP");
	    if (EdRun_IsProgramRunning ())
	    {
	    	EdRun_SetStep (STEP);
	    }
	    else
	    {
	    	EdDisp_Run (pmTextDisplayWindow, COMPILE_AND_EXECUTE, 
	    		    NO_RUN_ARGS_DIALOG, START_WITH_STEP);
	    }
	    break;
	case COMMAND_DEBUG_STEP_OVER:
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_DEBUG_STEP_OVER");
	    EdRun_SetStep (STEP_OVER);
	    break;
	case COMMAND_DEBUG_STEP_RETURN:
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_DEBUG_STEP_RETURN");
	    EdRun_SetStep (STEP_RETURN);
	    break;

	//
	// Developer Debug commands
	//	    
	case COMMAND_CRASH_ENVIRONMENT:
	    // TW Generate a GPF for testing purposes
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_CRASH_ENVIRONMENT");
	    {
	    	char *pmNullPointer = NULL;
	    	strcpy (pmNullPointer, "This had better cause a crash");
	    }
	    break;
	case COMMAND_CAUSE_ASSERT_FAIL:
	    // TW cause an assertion failure for testing purposes
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_CAUSE_ASSERT_FAIL");
            EdFail_Fatal (IDS_USER_ASSERT_FAILURE, __FILE__, __LINE__, 0);
	    break;
	case COMMAND_DUMP_MEMORY:
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_DUMP_MEMORY");
	    Ed_DumpMemory ();
	    EdWin_ShowStatus (pmTextDisplayWindow, "Memory dumped to file");
	    break;
	
	//
	// Special Commands
	//
	case COMMAND_SAVE_TO_RTF:
   	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_SAVE_TO_RTF");
	    MySaveRTF (pmTextDisplayWindow);
	    break;
	    
	// 
	// Unimplemented commands
	//
	    
        // Key commands	    
	case COMMAND_DEL_EOL:
	case COMMAND_JOIN:
	case COMMAND_DEL_LINE:
	
	// Run
	case COMMAND_SUBMIT:
	    EdGUI_Message1 (pmTextDisplayWindow, 0, IDS_UNIMPL_FEATURE_TITLE,
	        IDS_UNIMPLEMENTED_FEATURE);
	    break;
	    
    	default:
    	    return FALSE;
    } // switch	   

    // If the command was a movement command, then do the following
    if (MyIsMovementCommand (pmCommand))
    {
    	if (myStatus == HIDE_SELECTION)
    	{
    	    MyCaretDisplay (pmTextDisplayWindow);
    	    MyCaretMove (pmTextDisplayWindow);
    	}
    	else if (myStatus == CREATE_SELECTION)
    	{
    	    MyCaretHide (pmTextDisplayWindow);
    	}
    	else if (myStatus == MOVE_CARET)
    	{
    	    MyCaretMove (pmTextDisplayWindow);
	}    	
    	MyScrollToSelection (pmTextDisplayWindow, SCROLL_NORMAL, 
    	    (myStatus != NO_REDRAW) && (myStatus != MOVE_CARET));
    	    
	MyShowStatusForCursorMove (pmTextDisplayWindow);
    
    	// Reset the Undo buffer
	EdText_UndoSetMoreKeystrokes (MyGetTextPtr (pmTextDisplayWindow), 
				      FALSE);
    }

    EdWin_ShowCursorPosition (pmTextDisplayWindow);
        
    return TRUE;
} // MyCommandHandler


/************************************************************************/
/* MyCommonMessageHandler						*/
/*									*/
/* This handles the messages that are common to both the child window	*/
/* and MDI window text display window classes.				*/
/************************************************************************/
static BOOL	MyCommonMessageHandler (HWND pmTextDisplayWindow, 
					UINT pmMessage, WPARAM pmWParam, 
    					LPARAM pmLParam, BOOL *pmReturnVal)
{
    TextDisplayWindowInfoPtr	myInfo;
    WindowCreationInfoPtr	myWindowCreationInfoPtr;
    int				myX, myY;
        
    switch (pmMessage)
    {
    	case WM_CREATE:
	    EdFail_AddMessage (NORMAL, "CMD: WM_CREATE (D)");
	    if (stProperties.logWindowMessages)
	    {
	    	EdLog_Log ("Text Display Window = %x  (Starting Creation)", 
	    	    pmTextDisplayWindow);
	    }

    	    myWindowCreationInfoPtr = 
	        (WindowCreationInfoPtr) 
	            (((LPCREATESTRUCT) pmLParam) -> lpCreateParams);

	    *pmReturnVal = MyCreateTextDisplayWindow (pmTextDisplayWindow, 
	    		myWindowCreationInfoPtr -> beginnerMode,
	    		myWindowCreationInfoPtr -> oneWindowMode);
	    return TRUE;

	case WM_DESTROY:
	    EdFail_AddMessage (NORMAL, "CMD: WM_DESTROY (D)");
	    MyDispose (pmTextDisplayWindow);
	    break;
	    	    
        case WM_PAINT:
            MyDisplayText (pmTextDisplayWindow, TRUE, ALL_LINES, ALL_LINES);
            return TRUE;
        
        case WM_COMMAND:
            if (MyCommandHandler (pmTextDisplayWindow, LOWORD (pmWParam)))
            {
                return TRUE;
            }
            if (pmLParam == SENT_FROM_EDWIN)
            {
            	EdFail_Fatal (IDS_COMMAND_NOT_PROCESSED, __FILE__, __LINE__, 0,
            	    LOWORD (pmWParam));
            }
	    *pmReturnVal =  SendMessage (MyGetEditWindow (pmTextDisplayWindow), 
	    			WM_COMMAND, pmWParam, 
	    			(LPARAM) (SENT_FROM_EDDISP));
	    return TRUE;

	//
	// Scroll bar handling
	//
	case WM_HSCROLL:
	    MyScrollHandlerHorz (pmTextDisplayWindow, LOWORD (pmWParam), 
	    	HIWORD (pmWParam));
	    return TRUE;
	    
	case WM_VSCROLL:
	    MyScrollHandlerVert (pmTextDisplayWindow, LOWORD (pmWParam), 
	        HIWORD (pmWParam));
	    return TRUE;

	//
	// Keystroke handling
	//
	case WM_CHAR:
	    if ((32 <= pmWParam) && (pmWParam < 128))
	    {
	    	EdFail_AddMessage (LETTER_MESSAGE, "CMD: WM_CHAR [%c] (D)", 
	    			   pmWParam);
	    }
	    else
	    {
	    	EdFail_AddMessage (NORMAL, "CMD: WM_CHAR [#%d] (D)", pmWParam);
	    }
	    
	    MyKeystrokeHandler (pmTextDisplayWindow, pmWParam, NORMAL_KEY);
	    return TRUE;

	//
	// Mouse handling
	//
	case WM_MOUSEMOVE:
	    myInfo = MyGetTextDisplayWindowInfo (pmTextDisplayWindow);
	    if (myInfo -> selectingWithMouse)
	    {
	    	if (MyMouseDownHandler (pmTextDisplayWindow, 
	    	    GET_X_LPARAM (pmLParam), GET_Y_LPARAM (pmLParam), 
	    	    TRUE, FALSE))
                {
            	    myInfo -> verticalTicks = 0;
            	    myInfo -> horizontalTicks = 0;
                }
	    }
	    else if (stDisplayBreakpoints)
	    {
	    	int	myX = GET_X_LPARAM (pmLParam);
	    	int	myY = GET_Y_LPARAM (pmLParam);
	    	
	        if ((0 <= myX) && (myX < BREAKPOINT_MARGIN) &&
	            (0 <= myY) && (myY < myInfo -> textRect.bottom))
		{
		    if (!myInfo -> isBreakpointCursor)
		    {
			myInfo -> isBreakpointCursor = TRUE;
			SetClassLong (pmTextDisplayWindow,
				      GCL_HCURSOR, (LONG) stBreakpointCursor);
		    }
		}
	        else if (myInfo -> isBreakpointCursor)
	    	{
		    myInfo -> isBreakpointCursor = FALSE;
		    SetClassLong (pmTextDisplayWindow,
				  GCL_HCURSOR, (LONG) stIBeamCursor);
		}
	    }
	    return TRUE;
	
//	case WM_SETCURSOR:
//	    myInfo = MyGetTextDisplayWindowInfo (pmTextDisplayWindow);
//	    if (stDisplayBreakpoints && myInfo -> isBreakpointCursor)
//	    {
//            	return TRUE;
//            }
//	    return FALSE;
	    
        case WM_CONTEXTMENU:
	    EdFail_AddMessage (NORMAL, "CMD: WM_CONTEXTMENU (D)");
	    myX = GET_X_LPARAM (pmLParam);
	    myY = GET_Y_LPARAM (pmLParam);
	    MyAdjustContextualMenu (pmTextDisplayWindow);
	    TrackPopupMenu (stContextualMenu, 0, myX, myY, 
			    0, pmTextDisplayWindow, NULL);
	    return 0;

        case WM_LBUTTONDOWN:
	    EdFail_AddMessage (NORMAL, "CMD: WM_LBUTTONDOWN (D)");
            // Returns true if mouse down in text region
            if (MyMouseDownHandler (pmTextDisplayWindow, 
                GET_X_LPARAM (pmLParam), GET_Y_LPARAM (pmLParam), 
                pmWParam & MK_SHIFT, FALSE))
            {
	    	myInfo = MyGetTextDisplayWindowInfo (pmTextDisplayWindow);
    
            	SetCapture (pmTextDisplayWindow);
            	SetTimer (pmTextDisplayWindow, SCROLL_TIMER, 1, NULL);
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
            	    MyMouseExtendSelectionToWord (pmTextDisplayWindow);
		}
		else if (myInfo -> numClicks == 3)
		{
            	    MyMouseExtendSelectionToLine (pmTextDisplayWindow);
		}
		else if (myInfo -> numClicks == 4)
            	{
            	    myInfo -> numClicks = 1;
            	}

            	return TRUE;
            }
            break;
                    
        case WM_LBUTTONUP:
	    EdFail_AddMessage (NORMAL, "CMD: WM_LBUTTONUP (D)");
	    myInfo = MyGetTextDisplayWindowInfo (pmTextDisplayWindow);
    
            if (myInfo -> selectingWithMouse)
            {
            	MyMouseDownHandler (pmTextDisplayWindow, 
            	    GET_X_LPARAM (pmLParam), GET_Y_LPARAM (pmLParam), 
            	    TRUE, FALSE);
            	ReleaseCapture ();
            	KillTimer (pmTextDisplayWindow, SCROLL_TIMER);
            	myInfo -> selectingWithMouse = FALSE;
            	return TRUE;
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
	    	
	    	myInfo = MyGetTextDisplayWindowInfo (pmTextDisplayWindow);
	    	
                myRect = myInfo -> textRect;
                
	    	myInfo -> verticalTicks++;
	    	myInfo -> horizontalTicks++;
	    	
	    	GetCursorPos (&myPoint);
	    	ScreenToClient (pmTextDisplayWindow, &myPoint);
	    	
	    	if (myPoint.x < myRect.left)
	    	{
	    	    while (myInfo -> horizontalTicks > HORIZ_TICKS_PER_SCROLL)
	    	    {
	    	    	MyScrollHandlerHorz (pmTextDisplayWindow, 
	    	    	    SB_LINELEFT, 0);
	    	    	myInfo -> horizontalTicks -= HORIZ_TICKS_PER_SCROLL;
	    	    }
	    	    MyMouseDownHandler (pmTextDisplayWindow, myPoint.x, 
	    	    	myPoint.y, TRUE, TRUE);
	    	}
	    	else if (myPoint.x > myRect.right)
	    	{
	    	    while (myInfo -> horizontalTicks > HORIZ_TICKS_PER_SCROLL)
	    	    {
	    	    	MyScrollHandlerHorz (pmTextDisplayWindow, 
	    	    	    SB_LINERIGHT, 0);
	    	    	myInfo -> horizontalTicks -= HORIZ_TICKS_PER_SCROLL;
	    	    }
	    	    MyMouseDownHandler (pmTextDisplayWindow, myPoint.x, 
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
	    	    	MyScrollHandlerVert (pmTextDisplayWindow, SB_LINEUP, 0);
	    	    	myInfo -> verticalTicks -= ticks;
	    	    }
	    	    MyMouseDownHandler (pmTextDisplayWindow, myPoint.x, 
	    	        myPoint.y, TRUE, TRUE);
	    	}
	    	else if (myPoint.y > myRect.bottom)
	    	{
	    	    int ticks = max (1, 6 - ((myPoint.y - myRect.bottom) / 
	    	        stTextFontCharHeight));
	    	    while (myInfo -> verticalTicks > ticks)
	    	    {
	    	    	MyScrollHandlerVert (pmTextDisplayWindow, 
	    	    	    SB_LINEDOWN, 0);
	    	    	myInfo -> verticalTicks -= ticks;
	    	    }
	    	    MyMouseDownHandler (pmTextDisplayWindow, myPoint.x,
	    	        myPoint.y, TRUE, TRUE);
	    	}
	    	else
	    	{
	    	    myInfo -> verticalTicks = 0;
	    	}
	    }
	    return TRUE;
	    
	case WM_MOUSEWHEEL:
	    if (stWheelMouseDeltaPerLine != 0)
	    {
	    	myInfo = MyGetTextDisplayWindowInfo (pmTextDisplayWindow);
	    	
	        myInfo -> accumulateDelta += (short) HIWORD (pmWParam);
	        while (myInfo -> accumulateDelta >= stWheelMouseDeltaPerLine)
	    	{
	    	    MyScrollHandlerVert (pmTextDisplayWindow, SB_LINEUP, 0);
	    	    myInfo -> accumulateDelta -= stWheelMouseDeltaPerLine;
	    	}
	    	while (myInfo -> accumulateDelta <= -stWheelMouseDeltaPerLine)
	    	{
	    	    MyScrollHandlerVert (pmTextDisplayWindow, SB_LINEDOWN, 0);
	    	    myInfo -> accumulateDelta += stWheelMouseDeltaPerLine;
	    	}
	    	return TRUE;
	    }
	    break;
	    
	//
	// Focus handling
	//
	case WM_KILLFOCUS:
	    myInfo = MyGetTextDisplayWindowInfo (pmTextDisplayWindow);
	    // Make it back to normal cursor, if necessary
	    if (myInfo -> isBreakpointCursor)
	    {
		myInfo -> isBreakpointCursor = FALSE;
		SetClassLong (pmTextDisplayWindow, GCL_HCURSOR, 
			      (LONG) stIBeamCursor);
	    }			    
            if (myInfo -> text -> selFrom == myInfo -> text -> selTo)
            {
	        MyCaretHide (pmTextDisplayWindow);
            }
            myInfo -> caretCreated = FALSE;
	    DestroyCaret ();
            MyDisplayText (pmTextDisplayWindow, FALSE, ALL_LINES, ALL_LINES);
	    return TRUE;
	
	//
	// Default Messages	    
	//
        default:
            if (pmMessage == EdSearch_FindReplaceMessage)
            {
            	LPFINDREPLACE	myFindReplace = (LPFINDREPLACE) pmLParam;
            	
            	if (myFindReplace -> Flags & FR_DIALOGTERM)
            	{
            	    EdSearch_CloseDialog (USER_CLOSED);
            	}
            	else if (myFindReplace -> Flags & FR_FINDNEXT)
            	{
            	    MyFindNext (pmTextDisplayWindow, SEARCH_FORWARD);
        	}
        	else if (myFindReplace -> Flags & FR_REPLACE)
        	{
        	    MyFindReplace (pmTextDisplayWindow);
        	}
        	else if (myFindReplace -> Flags & FR_REPLACEALL)
        	{
        	    MyFindReplaceAll (pmTextDisplayWindow);
        	}
                return TRUE;
            }
            else if ((pmMessage == stMouseWheelMessage) && 
	    	     (stWheelMouseDeltaPerLine != 0))
	    {
	    	myInfo = MyGetTextDisplayWindowInfo (pmTextDisplayWindow);
	    	
	        myInfo -> accumulateDelta += (short) HIWORD (pmWParam);
	        while (myInfo -> accumulateDelta >= stWheelMouseDeltaPerLine)
	    	{
	    	    MyScrollHandlerVert (pmTextDisplayWindow, SB_LINEUP, 0);
	    	    myInfo -> accumulateDelta -= stWheelMouseDeltaPerLine;
	    	}
	    	while (myInfo -> accumulateDelta <= -stWheelMouseDeltaPerLine)
	    	{
	    	    MyScrollHandlerVert (pmTextDisplayWindow, SB_LINEDOWN, 0);
	    	    myInfo -> accumulateDelta += stWheelMouseDeltaPerLine;
	    	}
	    	return TRUE;
            }
    } // switch
    
    return FALSE;
} // MyCommonMessageHandler 


/****************************************************************/
/* MyCreatePseudoPaths						*/
/*								*/
/* This subprogram enumerates all the open text windows and	*/
/* when appropriate, sets the "pseudo-path" for it.		*/
/****************************************************************/
static BOOL	MyCreatePseudoPaths (HWND pmWindow)
{
    EnumerateInfo	myEnumerateInfo;
    
    myEnumerateInfo.task = GATHER_REAL_FILES_FIRST;
    myEnumerateInfo.window = pmWindow;
    
    Ed_EnumerateFiles (MyCreatePseudoPathEnumerator, &myEnumerateInfo);
    
    myEnumerateInfo.task = CREATE_PSEUDO_PATHS;
    
    Ed_EnumerateFiles (MyCreatePseudoPathEnumerator, &myEnumerateInfo);
    
    if (myEnumerateInfo.task == FILE_NAME_AMBIGUITY)
    {
    	return FALSE;
    }
    
    return TRUE;
} // MyCreatePseudoPaths


/****************************************************************/
/* MyCreateEXE							*/
/****************************************************************/
static void	MyCreateEXE (HWND pmTextDisplayWindow)
{
    char			myPackageName [1024], myClassName [1024];
    FilePath			myPathName;
    TextDisplayWindowInfoPtr	myInfo;
    
    // If a Turing program is running, then inform the user that we cannot
    // create the program yet.
    if (stProgramIsRunning)
    {
	EdGUI_Message1 (pmTextDisplayWindow, 0, IDS_CREATE_EXE_FAILED_TITLE, 
    	    IDS_CANNOT_CREATE_EXE_WHILE_PROG_RUNNING);
        EdWin_ShowStatus (pmTextDisplayWindow, "Create EXE cancelled");
	return;
    }

    // Show status
    EdWin_ShowStatus (pmTextDisplayWindow, "Creating Standalone Executable");
    
    //
    // Show the Create EXE options dialog
    //
    if (!DialogBoxParam (gProgram.applicationInstance, 
            MAKEINTRESOURCE (TURING_CREATE_EXE_DIALOG),
	    pmTextDisplayWindow, MyCreateEXEDialogProcedure, 
	    (LPARAM) &stCreateEXEArgs))
    {
        EdWin_ShowStatus (pmTextDisplayWindow, "Create EXE cancelled");
	return;
    }
    
    // Regardless of what ending it did have, save it as .exe"    	
    // Get the window information
    myInfo = MyGetTextDisplayWindowInfo (pmTextDisplayWindow);
    
    if (myInfo -> isJava)
    {
    	EdJava_GetPackageAndClass (myInfo -> text -> text, 
	    myInfo -> text -> textLen, myPackageName, myClassName);
    	if (myClassName [0] == 0)
    	{
    	    EdGUI_Message1 (pmTextDisplayWindow, 0, IDS_UNKNOWN_FILE_TYPE_TITLE, 
    	        IDS_RTF_MUST_BE_KNOWN_LANG);
    	    return;
    	}
    }
    else if (myInfo -> isTuring)
    {    
    	EdTur_GetProgramName (myInfo -> text -> text, 
	    myInfo -> text -> textLen, myClassName);
    }
    else
    {
    	EdGUI_Message1 (pmTextDisplayWindow, 0, IDS_UNKNOWN_FILE_TYPE_TITLE, 
    	    IDS_RTF_MUST_BE_KNOWN_LANG);
    	return;
    }
        	
    if (myInfo -> pathName == NULL)
    {
    	if (myClassName [0] != 0)
    	{
    	    strcpy (myPathName, myClassName);
    	    strcat (myPathName, EXE_SUFFIX);
    	}
    	else
    	{
    	    strcpy (myPathName, "Unnamed");
    	    strcat (myPathName, EXE_SUFFIX);
    	}
    }
    else
    {
    	// Use original file name, with EXE ending
    	const char	*myPtr;
    	
    	strcpy (myPathName, myInfo -> pathName);
    	myPtr = EdFile_GetFileSuffix (myPathName);
    	strcpy (&myPathName [myPtr - myPathName], EXE_SUFFIX);
    }
    	
    if (!EdGUI_GetSaveEXEFile (pmTextDisplayWindow, myPathName))
    {
        EdWin_ShowStatus (pmTextDisplayWindow, "Create EXE cancelled");
        return;
    }
    
    EdRun_CreateEXE (pmTextDisplayWindow, myPathName, 
    		     stCreateEXEArgs.closeWindowsOnTerminate,
    		     stCreateEXEArgs.displayRunWithArgs, 
    		     stCreateEXEArgs.centerOutputWindow, 
    		     stCreateEXEArgs.stopUserClose);
} // MyCreateEXE

    
/************************************************************************/
/* MyCreatePseudoPathEnumerator						*/
/************************************************************************/
static void	MyCreatePseudoPathEnumerator (char *pmPathName, 
		    HWND pmEditWindow, HWND pmTextDisplayWindow, 
		    TextPtr pmTextPtr, void *pmEnumerateInfoPtr)
{
    FileListPtr		myFilePtr, myNextFilePtr;
    EnumerateInfo	*myEnumerateInfoPtr;
    FilePath		myPseudoPath;
    TextDisplayWindowInfoPtr	myInfo;
    
    if ((pmPathName != NULL) && !EdGlob_EndsWith (pmPathName, JAVA_SUFFIX))
    {
    	return;
    }
    
    myEnumerateInfoPtr = (EnumerateInfo *) pmEnumerateInfoPtr;
    if (myEnumerateInfoPtr -> task == FILE_NAME_AMBIGUITY)
    {
    	return;
    }
    
    if (myEnumerateInfoPtr -> task == GATHER_REAL_FILES_FIRST)
    {
    	// Erase the linked list
    	myFilePtr = stFileListHead;
    	while (myFilePtr != NULL)
    	{
    	    myNextFilePtr = myFilePtr -> next;
    	    free (myFilePtr -> pathName);
    	    free (myFilePtr);
    	    myFilePtr = myNextFilePtr;
    	}
    	stFileListHead = NULL;
    	
	myEnumerateInfoPtr -> task = GATHER_REAL_FILES;
    }
    
    if (myEnumerateInfoPtr -> task == GATHER_REAL_FILES)
    {
	if (pmPathName != NULL)
        {
    	    myFilePtr = malloc (sizeof (FileList));
        	// TW Check for malloc
    	    myFilePtr -> pathName = malloc (strlen (pmPathName) + 1);
        	// TW Check for malloc
        	strcpy (myFilePtr -> pathName, pmPathName);
		myFilePtr -> next = stFileListHead;
    	    stFileListHead = myFilePtr;
	}
        return;
    }
    
    // myEnumerateInfoPtr -> task = CREATE_PSEUDO_PATHS
    myInfo = MyGetTextDisplayWindowInfo (pmTextDisplayWindow);
    if (myInfo -> pseudoPathName != NULL)
    {
    	free (myInfo -> pseudoPathName);
    	myInfo -> pseudoPathName = NULL;
    }

    if (pmPathName != NULL)
    {    
    	strcpy (myPseudoPath, pmPathName);
    }
    else
    {
    	myPseudoPath [0] = 0;
    }
    
    if (EdRun_GetPseudoPath (myEnumerateInfoPtr -> window, pmTextPtr -> text, 
    			     pmTextPtr -> textLen, myPseudoPath))
    {
    	myFilePtr = stFileListHead;
    	while (myFilePtr != NULL)
    	{
    	    if (strcmp (myFilePtr -> pathName, myPseudoPath) == 0)
    	    {
    	    	EdGUI_Message1 (myEnumerateInfoPtr -> window, 0, 
    	    			IDS_FILE_NAME_COLLISION_TITLE,
    	    			IDS_FILE_NAME_COLLISION, myPseudoPath);
		myEnumerateInfoPtr -> task = FILE_NAME_AMBIGUITY;
    	    	return;		
    	    }
	    myFilePtr = myFilePtr -> next;
    	}
    	
    	// No matches, add it to the list of files.
    	myFilePtr = malloc (sizeof (FileList));
    	// TW Check for malloc
    	myFilePtr -> pathName = malloc (strlen (myPseudoPath) + 1);
    	// TW Check for malloc
    	strcpy (myFilePtr -> pathName, myPseudoPath);
    	myFilePtr -> next = stFileListHead;
    	stFileListHead = myFilePtr;
    	
    	// Also make it the file's pseudo path
    	myInfo = MyGetTextDisplayWindowInfo (pmTextDisplayWindow);
    	if (myInfo -> pseudoPathName != NULL)
    	{
	    free (myInfo -> pseudoPathName);
	}
    	myInfo -> pseudoPathName = malloc (strlen (myPseudoPath) + 1);
    	// TW Check for malloc
    	strcpy (myInfo -> pseudoPathName, myPseudoPath);
    }    			     
} // MyCreatePseudoPathEnumerator

		    
/************************************************************************/
/* MyCreateTextDisplayWindow						*/
/************************************************************************/
static int	MyCreateTextDisplayWindow (HWND pmTextDisplayWindow, 
					   BOOL pmBeginnerMode, 
					   BOOL pmOneWindowMode)
{
    TextDisplayWindowInfoPtr	myInfo;

    //
    // Initialize the extra window memory
    //
	
    // Set the window type            
    SetWindowLong (pmTextDisplayWindow, WINDOW_MEM_WINDOW_TYPE, 
        (long) WINDOW_TYPE_TEXT_DISPLAY);

    // Set the window initialized                
    SetWindowLong (pmTextDisplayWindow, WINDOW_MEM_WINDOW_INITIALIZED, 
        (long) FALSE);
            
    // Create the ddit window information
    myInfo = MyCreateTextDisplayWindowInfo ();
    if (myInfo == NULL)
    {
    	return -1;
    }
    SetWindowLong (pmTextDisplayWindow, WINDOW_MEM_WINDOW_INFO, (long) myInfo);

    myInfo -> beginnerMode = pmBeginnerMode;
    myInfo -> oneWindowMode = pmOneWindowMode;
        	    
    //
    // Add the elements to the window
    //
    
    // Add the Edit box (an edit window)
    myInfo -> text = EdText_Create ();
    if (myInfo -> text == NULL)
        return -1;
         
    // If the font properties have not already been determined, 
    // determine them now (we needed a window to do so)
    EdDisp_PropertiesImplementChanges (pmTextDisplayWindow);
    	    
    // Set the window to initialized
    SetWindowLong (pmTextDisplayWindow, WINDOW_MEM_WINDOW_INITIALIZED, 
        (long) TRUE);
    
    return 0;
} // MyCreateTextDisplayWindow

				    
/************************************************************************/
/* MyCreateTextDisplayWindowInfo					*/
/************************************************************************/
static TextDisplayWindowInfoPtr	MyCreateTextDisplayWindowInfo (void)
{
    TextDisplayWindowInfoPtr	myInfo;
    
    // Allocate the structure for the information about the edit window
    myInfo = (TextDisplayWindowInfoPtr) malloc (sizeof (TextDisplayWindowInfo));
    
    if (myInfo != NULL)
    {
    	ZeroMemory (myInfo, sizeof (TextDisplayWindowInfo));

        myInfo -> textRect.top = -1000;
	myInfo -> textRect.bottom = -1000;
        myInfo -> textRect.left = -1000;
	myInfo -> textRect.right = -1000;
	
	myInfo -> turingInitializedWhere = NOTIFY_NEVER;
    }
    
    return myInfo;  
} // MyCreateTextDisplayWindowInfo


/******************************************************************************/
/* MyDetermineEmergencyName						      */
/******************************************************************************/
static void	MyDetermineEmergencyName (char *pmPathName,
					  FilePath pmEmergencyPathName)
{
    char	*mySuffix = NULL;
    int		cnt;
    
    if (pmPathName != NULL)
    {
    	strcpy (pmEmergencyPathName, pmPathName);
    	mySuffix = strrchr (pmEmergencyPathName, '.');
    }
    else
    {
        GetCurrentDirectory (sizeof (FilePath), pmEmergencyPathName);
        if (pmEmergencyPathName [strlen (pmEmergencyPathName) - 1] != '\\')
        {
            strcat (pmEmergencyPathName, "\\");
        }
    	strcat (pmEmergencyPathName, "unnamed");
    }
    
    if (mySuffix == NULL)
    {
    	mySuffix = &pmEmergencyPathName [strlen (pmEmergencyPathName)];
    }
    
    // We try in order, ".sav", then ".sv0", ".sv1", ...
    strcpy (mySuffix, ".sav");
    if ((GetFileAttributes (pmEmergencyPathName) == -1) && 
        (GetLastError () == ERROR_FILE_NOT_FOUND))
    {
    	return;
    }
    
    for (cnt = 0 ; cnt < 99 ; cnt++)
    {
    	wsprintf (mySuffix, ".sv%d", cnt);
    	if ((GetFileAttributes (pmEmergencyPathName) == -1) && 
            (GetLastError () == ERROR_FILE_NOT_FOUND))
    	{
    	    return;
    	}
    }
    
    pmEmergencyPathName [0] = 0;	
} // MyDetermineEmergencyName


/******************************************************************************/
/* MyDetermineLanguage							      */
/******************************************************************************/
static void	MyDetermineLanguage (TextDisplayWindowInfoPtr pmInfo)
{
    if ((pmInfo -> suffix == NULL) || (pmInfo -> suffix [0] == 0))
    {
    	pmInfo -> isJava = gProgram.isJava;
    	pmInfo -> isTuring = gProgram.isTuring;
    }	     
    else if (_stricmp (pmInfo -> suffix, ".java") == 0)
    {
    	pmInfo -> isJava = TRUE;
    	pmInfo -> isTuring = FALSE;
    }
    else if ((_stricmp (pmInfo -> suffix, ".t") == 0) ||
	     (_stricmp (pmInfo -> suffix, ".tur") == 0) ||
	     (_stricmp (pmInfo -> suffix, ".ti") == 0) ||
	     (_stricmp (pmInfo -> suffix, ".tu") == 0) ||
	     (_stricmp (pmInfo -> suffix, ".dem") == 0))
    {
    	pmInfo -> isJava = FALSE;
    	pmInfo -> isTuring = TRUE;
    }
    else
    {
    	// The file has an unrecognized suffix.  It's not Turing or Java
    	// This prevents accidentally indenting your HTML, etc.
    	pmInfo -> isJava = FALSE;
    	pmInfo -> isTuring = FALSE;
    }

    pmInfo -> text -> isJava = pmInfo -> isJava;
    pmInfo -> text -> isTuring = pmInfo -> isTuring;

    pmInfo -> syntaxColour = pmInfo -> isJava || pmInfo -> isTuring;
} // MyDetermineLanguage


/******************************************************************************/
/* MyDisplayText							      */
/* 									      */
/* The text should be displayed in the specified colours.  The special line   */
/* should display with normal coloured text and a light grey background.  The */
/* special token should display in dark grey with white text.  The selection  */
/* appears above all others and appears in dark blue with inverted text.      */
/******************************************************************************/
void	MyDisplayText (HWND pmTextDisplayWindow, BOOL pmPaintUpdate, 
		       int pmFromLine, int pmToLine)
{
    HDC				myDisplayContext;
    RECT			myLineRect, mySectionRect, myBreakRect;
    TextPtr			myTextPtr;
    HBRUSH			mySelectionBrush, myErrorLineBrush;
    HBRUSH			myBackgroundBrush;
    int				myWidth, myHeight;
    int				myNumVisibleLines;
    int				myLastVisibleLine;
    int				myFirstLine, myLastLine;
    int				myLineNumber;
    int				mySelFromLine, mySelFromCol;
    int				mySelToLine, mySelToCol;
    BOOL			myHighlightedLineDraw = FALSE;
    BOOL			myEraseAtEnd = FALSE;
    BOOL			myEraseLastLine = FALSE;
    ErrorLinePtr		myErrorLine;
    BreakLinePtr		myBreakLine;
    TextDisplayWindowInfoPtr	myInfo;
    BOOL			myWindowActive;
    HRGN			mySelectionRegion = NULL;
    PAINTSTRUCT			myPaintStruct;

    // Get the window info    
    myInfo = MyGetTextDisplayWindowInfo (pmTextDisplayWindow);
    
    // Make certain the colours are initialized if they're not already
    if (!stTextColoursInitialized)    
    {
    	MyInitializeColours (pmTextDisplayWindow);
    }

    // Create the appropriate brushes
    mySelectionBrush = CreateSolidBrush (stSelectionColour);
    myErrorLineBrush = CreateSolidBrush (COLOUR_LIGHTGREY);
    myBackgroundBrush = (HBRUSH) GetStockObject (WHITE_BRUSH);
    
    // Determine if the selection is drawn or framed
    {
	HWND	myForegroundWindow, myEditWindow, myNextWindow;
	
    	myForegroundWindow = GetForegroundWindow ();
    	myEditWindow = MyGetEditWindow (pmTextDisplayWindow);
    	myNextWindow = EdGUI_GetTopWindow (
    			GetNextWindow (myForegroundWindow, GW_HWNDNEXT));

    	if ((myForegroundWindow == myEditWindow) ||
            ((myForegroundWindow == EdSearch_FindReplaceDialog) && 
             (myNextWindow == myEditWindow)))
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
    
    // Set the rectange encompassing the line to be displayed
    myLineRect = myInfo -> textRect;
    myLineRect.bottom = myLineRect.top + stTextFontCharHeight;

    myBreakRect.left = 0;
    myBreakRect.right = BREAKPOINT_MARGIN;
    
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

    myErrorLine = myTextPtr -> errorLine;
    myBreakLine = myTextPtr -> breakLine;
    
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
        myDisplayContext = BeginPaint (pmTextDisplayWindow, &myPaintStruct);
    }        
    else
    {
        myDisplayContext = GetDC (pmTextDisplayWindow);
    }        
    HideCaret (pmTextDisplayWindow);

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
	
	while ((myErrorLine != NULL) && 
	       (myErrorLine -> errorPtr -> line < myLineNumber))
	{
	    myErrorLine = myErrorLine -> next;
	}
	
	while ((myBreakLine != NULL) && 
	       (myBreakLine -> line < myLineNumber))
	{
	    myBreakLine = myBreakLine -> next;
	}
	
	// Set the background if its the highlight line
	if ((myErrorLine != NULL) && 
	    (myErrorLine -> errorPtr -> line == myLineNumber))
	{
	    myHighlightedLineDraw = TRUE;
	}
	else
	{
	    myHighlightedLineDraw = FALSE;
	}
	
	if (stDisplayBreakpoints)
	{
	    myBreakRect.top = myLineRect.top;
	    myBreakRect.bottom = myLineRect.bottom;
	    
    	    FillRect (myDisplayContext, &myBreakRect, myBackgroundBrush);
    	    
	    if ((myBreakLine != NULL) && (myBreakLine -> line == myLineNumber))
	    {
	    	DrawIcon (myDisplayContext, myBreakRect.left + 1, 
	    		  myBreakRect.top + 3, stBreakpointIcon);
	    }
	}
	
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
    	    	// Skip over error before the myStartPos
    	    	while ((myErrorLine != NULL) && 
    	    	       (myLineNumber == myErrorLine -> errorPtr -> line) && 
    	    	       (myErrorLine -> errorPtr -> tokenPos + 
    	    	        myErrorLine -> errorPtr -> tokenLen <= 
    	    	        myStartPos))
    	    	{
    	    	    myErrorLine = myErrorLine -> next;
    	    	}
    	    	
    	    	if ((myErrorLine != NULL) &&
    	    	    (myLineNumber == myErrorLine -> errorPtr -> line) &&
    	    	    (myErrorLine -> errorPtr -> tokenPos <= myStartPos) && 
    	    	    (myStartPos < myErrorLine -> errorPtr -> tokenPos + 
    	    	        myErrorLine -> errorPtr -> tokenLen))
    	    	{
    	    	    myStatus = IN_HIGHLIGHT;
    	    	    myEndPos = min (myLength, 
    	    	    	myErrorLine -> errorPtr -> tokenPos + 
    	    	        myErrorLine -> errorPtr -> tokenLen);
    	    	        
    	            if (myWindowActive && 
    	                (mySelStart != NO_SEL_ON_THIS_LINE) &&
    	                (myStartPos < mySelStart))
    	            {
    	            	myEndPos = min (myEndPos, mySelStart);
    	            }
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
    	    	    if ((myErrorLine != NULL) &&
    	    	        (myLineNumber == myErrorLine -> errorPtr -> line) &&
    	    	    	(myStartPos < myErrorLine -> errorPtr -> tokenPos))
    	    	    {
    	    	    	myEndPos = min (myEndPos, 
    	    	    			myErrorLine -> errorPtr -> tokenPos);
    	    	    }
    	    	    myEndPos = min (myEndPos, myLength);
    	    	}
    	    }
    	    
    	    if (myStatus == IN_NORMAL)
    	    {
    	    	char	*myTextType, *myEndTextType;
	    	int	myTokenTextType;
	    	int	myStringLen;
    	    	
    	    	if (myHighlightedLineDraw)
    	    	{
	    	    SetBkColor (myDisplayContext, COLOUR_LIGHTGREY);
	    	}
	    	else
	    	{
		    SetBkColor (myDisplayContext, COLOUR_WHITE);
		}

		myTextType = &myTextPtr -> textType [
	    	    myTextPtr -> lineStarts [myLineNumber] + myStartPos];
    	    	myEndTextType = &myTextPtr -> textType [
	    	    myTextPtr -> lineStarts [myLineNumber] + myEndPos];
    	
    	        while (myTextType < myEndTextType)
    	        {
    	            myTokenTextType = *myTextType;
    	            myTextType++;
    	            myStringLen = 1;
    	            while (((*myTextType == myTokenTextType) ||
    	            	    (!myInfo -> syntaxColour) || 
    	            	    (!stProperties.syntaxColour)) && 
    	                   (myTextType < myEndTextType))
    	            {
    	    	    	myTextType++;
    	    	    	myStringLen++;
    	            }
    	            if (myInfo -> syntaxColour && stProperties.syntaxColour)
    	            {
    	            	if (myTokenTextType == TEXT_KEYWORD)
    	            	{
		    	    SelectFont (myDisplayContext, stBoldFont);
	            	}
	            	SetTextColor (myDisplayContext, 
	            		      stTextColours [myTokenTextType]);
	            }
	    
	            // If we've reached the end of line, include the area 
	            // to the right of the line, otherwise only include 
	            // the area in the text
	            mySectionRect.right = mySectionRect.left + 
		        myStringLen * stTextFontCharWidth;

	            ExtTextOut (myDisplayContext, mySectionRect.left, 
	                mySectionRect.top, ETO_CLIPPED | ETO_OPAQUE, 
	                &mySectionRect, myLine, myStringLen, NULL);
    	            mySectionRect.left = mySectionRect.right;
    	            myLine += myStringLen;
    	        
    	            if (myInfo -> syntaxColour && stProperties.syntaxColour && 
    	                (myTokenTextType == TEXT_KEYWORD))
    	            {
		        SelectFont (myDisplayContext, stNormalFont);
	            }
    	        } // while
    	    }
    	    else if (myStatus == IN_HIGHLIGHT)
    	    {
		SetTextColor (myDisplayContext, COLOUR_WHITE);
		SetBkColor (myDisplayContext, COLOUR_DARKGREY);
		mySectionRect.right = mySectionRect.left + 
		    (myEndPos - myStartPos) * stTextFontCharWidth;
	        ExtTextOut (myDisplayContext, mySectionRect.left, 
	            mySectionRect.top, ETO_CLIPPED | ETO_OPAQUE, &mySectionRect, 
    	            myLine, (myEndPos - myStartPos), NULL);
    	        mySectionRect.left = mySectionRect.right;
    	        myLine += myEndPos - myStartPos;
		SetTextColor (myDisplayContext, COLOUR_BLACK);
    	    }
    	    else if (myStatus == IN_SELECTION)
    	    {
		SetTextColor (myDisplayContext, COLOUR_WHITE);
		SetBkColor (myDisplayContext, stSelectionColour);
		mySectionRect.right = mySectionRect.left + 
		    (myEndPos - myStartPos) * stTextFontCharWidth;
	        ExtTextOut (myDisplayContext, mySectionRect.left, 
	            mySectionRect.top, ETO_CLIPPED | ETO_OPAQUE, &mySectionRect, 
    	            myLine, (myEndPos - myStartPos), NULL);
    	        mySectionRect.left = mySectionRect.right;
    	        myLine += myEndPos - myStartPos;
		SetTextColor (myDisplayContext, COLOUR_BLACK);
    	    }
    	    
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
    	else if (myHighlightedLineDraw)
    	{
    	    FillRect (myDisplayContext, &mySectionRect, myErrorLineBrush);
    	}
    	else
    	{
    	    FillRect (myDisplayContext, &mySectionRect, myBackgroundBrush);
    	}

	myLineRect.top += stTextFontCharHeight;
	myLineRect.bottom += stTextFontCharHeight;
    } // for 
    
    DeleteBrush (mySelectionBrush);
    DeleteBrush (myErrorLineBrush);
	
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
    	if (myLineRect.top < myInfo -> textRect.bottom)
    	{
	    myLineRect.bottom = myInfo -> textRect.bottom;
	    FillRect (myDisplayContext, &myLineRect, myBackgroundBrush);
    	}
    }
    else if (myEraseLastLine)
    {
    	// Needed when we are the last character in a line at the end of
    	// the file
	FillRect (myDisplayContext, &myLineRect, myBackgroundBrush);
    }
    
    // Clear the margins
    {
    	RECT	myRect;
    	
    	// The top margin
    	myRect.left = myInfo -> textRect.left - MARGIN;
    	myRect.right = myInfo -> textRect.right;
    	myRect.top = 0;
    	myRect.bottom = 2;
	FillRect (myDisplayContext, &myRect, myBackgroundBrush);
	
	// The left margin    
    	myRect.left = myInfo -> textRect.left - MARGIN;
    	myRect.right = myInfo -> textRect.left;
    	myRect.top = 0;
    	myRect.bottom = myInfo -> textRect.bottom;
	FillRect (myDisplayContext, &myRect, myBackgroundBrush);
    }
    	    
    // If breakpoints are showing, draw the breakpoints
    if (stDisplayBreakpoints)
    {
    	RECT	myRect;
    	
    	myRect.left = 0;
    	myRect.right = BREAKPOINT_MARGIN;
    	myRect.top = 0;
    	myRect.bottom = 2;
	FillRect (myDisplayContext, &myRect, myBackgroundBrush);
	SelectObject (myDisplayContext, stDashPen);
	MoveToEx (myDisplayContext, BREAKPOINT_MARGIN, 0, NULL);
	LineTo (myDisplayContext, BREAKPOINT_MARGIN, myInfo -> textRect.bottom);
	SelectObject (myDisplayContext, GetStockObject (BLACK_PEN));
    }
    	
    ShowCaret (pmTextDisplayWindow);
    if (pmPaintUpdate)
    {
    	EndPaint (pmTextDisplayWindow, &myPaintStruct);
    }
    else
    {
    	ReleaseDC (pmTextDisplayWindow, myDisplayContext);
    }
    
    myInfo -> oldTopLine = myInfo -> topLine;
    myInfo -> oldLeftCol = myInfo -> leftCol;
} // MyDisplayText


/************************************************************************/
/* MyDispose								*/
/************************************************************************/
static void	MyDispose (HWND pmTextDisplayWindow)
{
    TextDisplayWindowInfoPtr	myInfo;
    
    // Tell the Turing File Manager that the file has been closed.
    EdRun_NotifyFileClosed (pmTextDisplayWindow);
    
    myInfo = MyGetTextDisplayWindowInfo (pmTextDisplayWindow);
    EdText_Dispose (myInfo -> text);
    
    if (myInfo -> pathName != NULL)
    {
    	free (myInfo -> pathName);
    }
    
    if (myInfo -> pseudoPathName != NULL)
    {
    	free (myInfo -> pseudoPathName);
    }
    
    free (myInfo);
} // MyDispose


/************************************************************************/
/* MyEditUndo								*/
/************************************************************************/
static void	MyEditUndo (HWND pmTextDisplayWindow, BOOL pmUndo)
{
    TextPtr	myTextPtr;
    int		myStatusStringResource;
    
    myTextPtr = MyGetTextPtr (pmTextDisplayWindow);
    if (pmUndo)
    {
	myStatusStringResource = 
	    EdDisp_GetUndoStringResource (pmTextDisplayWindow) + 300;
    }
    else
    {
	myStatusStringResource = 
	    EdDisp_GetRedoStringResource (pmTextDisplayWindow) + 300;
    }

    if ((pmUndo && (myTextPtr -> undoTail == NULL)) ||
	(!pmUndo && (myTextPtr -> redoTail == NULL)))
    {
    	EdWin_ShowStatus (pmTextDisplayWindow, "Nothing to undo");
    	return;
    }
    
    EdText_Undo (myTextPtr, pmUndo);
    MySynchScrollBarsToText (pmTextDisplayWindow);
    
    EdWin_MarkMenuCreate (MyGetEditWindow (pmTextDisplayWindow));
    
    EdWin_SetWindowTitle (pmTextDisplayWindow);

    MyCaretDisplay (pmTextDisplayWindow);
    MyDisplayText (pmTextDisplayWindow, FALSE, ALL_LINES, ALL_LINES);

    MyScrollToSelection (pmTextDisplayWindow, SCROLL_NORMAL, TRUE);

    EdWin_ShowStatus1 (pmTextDisplayWindow, myStatusStringResource);    
} // MyEditUndo


/************************************************************************/
/* MyEditCutCopy							*/
/************************************************************************/
static void	MyEditCutCopy (HWND pmTextDisplayWindow, BOOL pmCut)
{
    TextPtr	myTextPtr;
    int		mySelectionLen;
    int		myFromLine, myFromCol, myToLine, myToCol;
    HGLOBAL	myGlobalHandle;
    char	*myGlobalPtr;
    char	*mySrc, *myEnd, *myDest;
    int		myNumLines;
    
    // Debug messages
    EdFail_AddMessage (NORMAL, "Cut/Copy (1) - Window: %x - Cut: %d", 
    		       pmTextDisplayWindow, pmCut);
        
    myTextPtr = MyGetTextPtr (pmTextDisplayWindow);
    
    // We need to convert the \n's to \r\n's in the clipboard.
    mySelectionLen = abs (myTextPtr -> selTo - myTextPtr -> selFrom);
    
    if (mySelectionLen == 0)
    {
    	EdWin_ShowStatusBold (pmTextDisplayWindow, 
    	    "No selection to copy");
    	return;
    }

    EdText_GetSelectionLinesCols (myTextPtr, &myFromLine, &myFromCol,
    				  &myToLine, &myToCol);
    myNumLines = myToLine - myFromLine;
    if (myToCol > 0)
    {
    	myNumLines++;
    }
    
    // Debug messages
    EdFail_AddMessage (NORMAL, "Cut/Copy (2) - from:%d:%d:%d - to:%d:%d:%d "
    		       "- lines:%d - nla: %d", 
    		       myFromLine, myFromCol, myTextPtr -> selFrom,
    		       myToLine, myToCol, myTextPtr -> selTo, myNumLines,
    		       myTextPtr -> numLines, myTextPtr -> newLineAdded);

    //    				 
    // Copy the text to a global handle
    //
    
    // Allocate the global memory and lock it down into a pointer
    myGlobalHandle = GlobalAlloc (GHND | GMEM_SHARE, mySelectionLen + 
    				  myToLine - myFromLine + 1);
    if (myGlobalHandle == NULL)
    {
    	EdGUI_Message1 (pmTextDisplayWindow, 0, IDS_COPY_FAILED_TITLE,
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
    OpenClipboard (pmTextDisplayWindow);
    EmptyClipboard ();
    SetClipboardData (CF_TEXT, myGlobalHandle);
    CloseClipboard ();
    
    if (pmCut)
    {
    	// Save Undo information
    	EdText_UndoSaveState (MyGetTextPtr (pmTextDisplayWindow), IDS_UNDO_CUT);
    
    	MySelectionDelete (pmTextDisplayWindow);
    	
    	EdWin_ShowStatus (pmTextDisplayWindow, "%d Lines Cut to Paste Buffer",
    	    myNumLines);
    }
    else
    {
    	EdWin_ShowStatus (pmTextDisplayWindow, 
    	    "%d Lines Copied to Paste Buffer", myNumLines);
    }
} // MyEditCutCopy


/************************************************************************/
/* MyEditPaste								*/
/************************************************************************/
static void	MyEditPaste (HWND pmTextDisplayWindow)
{
    TextPtr	myTextPtr;
    int		mySelectionLen;
    HGLOBAL	myGlobalHandle;
    char	*myGlobalPtr;
    int		myAction;
    char	*myBuffer;
    BOOL	mySelectionDeleted;
    int		myLinesDeleted, myLinesInserted;
    
    myTextPtr = MyGetTextPtr (pmTextDisplayWindow);
    
    // We need to convert the \n's to \r\n's in the clipboard.
    mySelectionLen = myTextPtr -> selTo - myTextPtr -> selFrom;
    
    // Debug messages
    EdFail_AddMessage (NORMAL, "Paste (1) - Window: %x - SelLen: %d", 
    		       pmTextDisplayWindow, mySelectionLen);
        
    //
    // Copy the data from the clipboard into a buffer
    //
    OpenClipboard (pmTextDisplayWindow);
    myGlobalHandle = GetClipboardData (CF_TEXT);
    if (myGlobalHandle == NULL)
    {
    	EdWin_ShowStatus (pmTextDisplayWindow, "No text in clipboard");
    	MessageBeep (MB_ICONEXCLAMATION);
    	CloseClipboard ();
    	return;
    }
    myBuffer = malloc (GlobalSize (myGlobalHandle));
    myGlobalPtr = GlobalLock (myGlobalHandle);
    strcpy (myBuffer, myGlobalPtr);
    GlobalUnlock (myGlobalHandle);
    CloseClipboard ();

    // Save Undo information
    EdText_UndoSaveState (myTextPtr, IDS_UNDO_PASTE);
    
    // If there's an outstanding selection, delete it
    if (myTextPtr -> selFrom == myTextPtr -> selTo)
    {
    	myAction = 0;
	mySelectionDeleted = FALSE;
    }
    else
    {    
    	int	myFromLine, myFromCol, myToLine, myToCol;
    	
    	EdText_GetSelectionLinesCols (myTextPtr, &myFromLine, &myFromCol,
    				      &myToLine, &myToCol);
    	myLinesDeleted = myToLine - myFromLine;
    	if (myToCol > 0)
    	{
    	    myLinesDeleted++;
    	}
    	myAction = EdText_SelectionDelete (myTextPtr);
	mySelectionDeleted = TRUE;
    }
    
    myAction |= EdText_InsertText (myTextPtr, myBuffer, strlen (myBuffer),
        FALSE, &myLinesInserted);
    
    if (mySelectionDeleted)
    {
    	EdWin_ShowStatus (pmTextDisplayWindow,
    	    "%d Lines Pasted Over %d Line Selection",
    	    myLinesInserted, myLinesDeleted);
    }
    else
    {
    	EdWin_ShowStatus (pmTextDisplayWindow, "%d Lines Pasted",
    	    myLinesInserted);
    }

    // Set the undo buffer
    if (myTextPtr -> undoTail != NULL)
    {
	myTextPtr -> undoTail -> replTo = myTextPtr -> selFrom;
    }

    if (myAction & (KEY_LONGEST_LINE_CHANGED | KEY_NUM_LINES_CHANGED))
    {
    	MySynchScrollBarsToText (pmTextDisplayWindow);
    }
    
    if (myAction & KEY_MARK_REMOVED)
    {
        EdWin_MarkMenuCreate (MyGetEditWindow (pmTextDisplayWindow));
    }

    if (myAction & KEY_ERROR_REMOVED)
    {
    	// Don't do anything -- EdWin_ShowStatus (pmTextDisplayWindow, " ");
    }

    if (myAction & KEY_FILE_DIRTY)
    {
    	EdWin_SetWindowTitle (pmTextDisplayWindow);
    }
    
    if (myAction & KEY_REDRAW_FROM_SELECTION)
    {
        MyCaretDisplay (pmTextDisplayWindow);
    	MyDisplayText (pmTextDisplayWindow, FALSE, ALL_LINES, ALL_LINES);
    }
    else if (myAction & KEY_UPDATE_LINE)
    {
    	int	myLine, myCol;
    	
        MyCaretDisplay (pmTextDisplayWindow);
        EdText_GetSelectionLocation (myTextPtr, &myLine, &myCol);
    	MyDisplayText (pmTextDisplayWindow, FALSE, myLine, myLine);
    }
    
    MyScrollToSelection (pmTextDisplayWindow, SCROLL_NORMAL, FALSE);
    
    free (myBuffer);
} // MyEditPaste


/************************************************************************/
/* MyEditSelectAll							*/
/************************************************************************/
static void	MyEditSelectAll (HWND pmTextDisplayWindow)
{
    int	myStatus;
    
    myStatus = EdText_SelectAll (MyGetTextPtr (pmTextDisplayWindow));
    if (myStatus == HIDE_SELECTION)
    {
        MyCaretDisplay (pmTextDisplayWindow);
        MyCaretMove (pmTextDisplayWindow);
    }
    else if (myStatus == CREATE_SELECTION)
    {
    	MyCaretHide (pmTextDisplayWindow);
    }
    MyScrollToSelection (pmTextDisplayWindow, SCROLL_NORMAL, 
        myStatus != NO_REDRAW);
        
    EdWin_ShowStatus (pmTextDisplayWindow, "%d Lines Selected",
        MyGetTextPtr (pmTextDisplayWindow) -> numLines);
} // MyEditSelectAll


/************************************************************************/
/* MyEditCommentRange							*/
/************************************************************************/
static void	MyEditCommentRange (HWND pmTextDisplayWindow, BOOL pmComment)
{
    TextPtr	myTextPtr;
    int		myFromLine, myFromCol, myToLine, myToCol, myFromPos, myToPos;
    int		myRedoString, myAction;
    
    myTextPtr = MyGetTextPtr (pmTextDisplayWindow);
    
    EdText_GetSelectionLinesCols (myTextPtr, &myFromLine, &myFromCol, 
				  &myToLine, &myToCol);
    // Calculate the selection for lines for undo purposes
    myFromPos = myTextPtr -> selFrom - myFromCol;
    if (myToCol > 0)
    {
    	myToLine++;
    	myToPos = myTextPtr -> lineStarts [myToLine];
    }
    myToPos = myTextPtr -> lineStarts [myToLine];
    
    if (pmComment)
    {
    	myRedoString = IDS_UNDO_COMMENT;
    }
    else
    {
    	myRedoString = IDS_UNDO_UNCOMMENT;
    }
    
    EdText_UndoSaveSpecified (myTextPtr, myRedoString, myFromPos, myToPos);
    EdText_UndoSetReplFrom (myTextPtr, myFromPos);

    if (pmComment)
    {    
    	myAction = EdText_SelectionComment (myTextPtr);
    }
    else
    {
    	myAction = EdText_SelectionUncomment (myTextPtr);
    }
    
    // Not all the lines were comments!  
    if (myAction & KEY_SOME_LINES_NOT_COMMENTS)
    {
    	MyDisplayText (pmTextDisplayWindow, FALSE, ALL_LINES, ALL_LINES);
        MyScrollToSelection (pmTextDisplayWindow, SCROLL_NORMAL, FALSE);
    	EdWin_ShowStatusBold (pmTextDisplayWindow, "Line is not a comment");
	EdText_UndoFreeLastBuffer (myTextPtr);
    	MessageBeep (MB_ICONEXCLAMATION);
    	return;
    }

    // Set the undo buffer
    if (myTextPtr -> undoTail != NULL)
    {
	myTextPtr -> undoTail -> replTo = myTextPtr -> selTo;
    }
    
    if (myAction & (KEY_LONGEST_LINE_CHANGED | KEY_NUM_LINES_CHANGED))
    {
    	MySynchScrollBarsToText (pmTextDisplayWindow);
    }
    
    if (myAction & KEY_MARK_REMOVED)
    {
        EdWin_MarkMenuCreate (MyGetEditWindow (pmTextDisplayWindow));
    }

    if (myAction & KEY_ERROR_REMOVED)
    {
    	// Don't do anything -- EdWin_ShowStatus (pmTextDisplayWindow, " ");
    }

    if (myAction & KEY_FILE_DIRTY)
    {
    	EdWin_SetWindowTitle (pmTextDisplayWindow);
    }
    
    if (myAction & KEY_REDRAW_FROM_SELECTION)
    {
        MyCaretDisplay (pmTextDisplayWindow);
    	MyDisplayText (pmTextDisplayWindow, FALSE, ALL_LINES, ALL_LINES);
    }
    else if (myAction & KEY_UPDATE_LINE)
    {
    	int	myLine, myCol;
    	
        MyCaretDisplay (pmTextDisplayWindow);
        EdText_GetSelectionLocation (myTextPtr, &myLine, &myCol);
    	MyDisplayText (pmTextDisplayWindow, FALSE, myLine, myLine);
    }
    
    MyScrollToSelection (pmTextDisplayWindow, SCROLL_NORMAL, FALSE);
    
    if (pmComment)
    {
    	EdWin_ShowStatus (pmTextDisplayWindow, "%d Lines Commented",
    	    myToLine - myFromLine);
    }
    else
    {
    	EdWin_ShowStatus (pmTextDisplayWindow, "%d Lines Uncommented",
    	    myToLine - myFromLine);
    }
} // MyEditCommentRange


/************************************************************************/
/* MyEditCompleteStructure						*/
/*									*/
/* This does structure completion.  Starting from the bottom of the 	*/
/* selection, it goes through looking for unclosed structures.  It	*/
/* then adds a line after the selection with a closing structure.	*/
/*									*/
/* Note that it is not rigorous.  It can make mistakes because it only	*/
/* looks at the first token in each line to determine the structures.	*/
/************************************************************************/
static void	MyEditCompleteStructure (HWND pmTextDisplayWindow)
{
    TextPtr	myTextPtr = MyGetTextPtr (pmTextDisplayWindow);
    int		myStatus;
    int		myAction;
    
    myStatus = EdText_CompleteStructure (myTextPtr, &myAction);

    switch (myStatus)
    {
	case INDENT_TOO_DEEP:
    	    // A structure was indented too deeply to do a struct completion
	    EdWin_ShowStatus (pmTextDisplayWindow, 
		"Completion failed: A structure is indented too deeply");
	    return;
	case NO_IDENT_FOUND:
    	    // In this case, myEndStruct contains the error message
	    EdWin_ShowStatus (pmTextDisplayWindow, 
		"Completion failed: Missing identifier after structure name");
	    return;
	case CANT_COMPLETE_JAVA:
    	    // Cannot complete a Java program
	    EdWin_ShowStatus (pmTextDisplayWindow, 
		"Completion failed: Cannot complete Java structures");
	    return;
	case CANNOT_COMPLETE_STRUCTURE:
    	    // Cannot complete a Java program
	    EdWin_ShowStatus (pmTextDisplayWindow, 
		"Completion failed: Cannot find structure to complete");
	    return;
    }

/*
    // Calculate the selection for lines for undo purposes
    myFromPos = myTextPtr -> selFrom - myFromCol;
    if (myToCol > 0)
    {
    	myToLine++;
    	myToPos = myTextPtr -> lineStarts [myToLine];
    }
    myToPos = myTextPtr -> lineStarts [myToLine];
    
    EdText_UndoSaveSpecified (myTextPtr, IDS_UNDO_COMPLETION, myFromPos, myToPos);
    EdText_UndoSetReplFrom (myTextPtr, myFromPos);

    if (pmComment)
    {    
    	myAction = EdText_SelectionComment (myTextPtr);
    }
    else
    {
    	myAction = EdText_SelectionUncomment (myTextPtr);
    }

  // Set the undo buffer
    if (myTextPtr -> undoTail != NULL)
    {
	myTextPtr -> undoTail -> replTo = myTextPtr -> selTo;
    }
*/    
    
    if (myAction & (KEY_LONGEST_LINE_CHANGED | KEY_NUM_LINES_CHANGED))
    {
    	MySynchScrollBarsToText (pmTextDisplayWindow);
    }
    
    if (myAction & KEY_MARK_REMOVED)
    {
        EdWin_MarkMenuCreate (MyGetEditWindow (pmTextDisplayWindow));
    }

    if (myAction & KEY_ERROR_REMOVED)
    {
    	// Don't do anything -- EdWin_ShowStatus (pmTextDisplayWindow, " ");
    }

    if (myAction & KEY_FILE_DIRTY)
    {
    	EdWin_SetWindowTitle (pmTextDisplayWindow);
    }
    
    if (myAction & KEY_REDRAW_FROM_SELECTION)
    {
        MyCaretDisplay (pmTextDisplayWindow);
    	MyDisplayText (pmTextDisplayWindow, FALSE, ALL_LINES, ALL_LINES);
    }
    else if (myAction & KEY_UPDATE_LINE)
    {
    	int	myLine, myCol;
    	
        MyCaretDisplay (pmTextDisplayWindow);
        EdText_GetSelectionLocation (myTextPtr, &myLine, &myCol);
    	MyDisplayText (pmTextDisplayWindow, FALSE, myLine, myLine);
    }

    MyScrollToSelection (pmTextDisplayWindow, SCROLL_NORMAL, FALSE);
    
    EdWin_ShowStatus (pmTextDisplayWindow, "Structure completed");
} // MyEditCompleteStructure


/************************************************************************/
/* MyEditIndent								*/
/************************************************************************/
static void	MyEditIndent (HWND pmTextDisplayWindow)
{
    TextPtr	myTextPtr;
    BOOL	myWasDirty;
    FilePath	myPathBuffer;
    
    myTextPtr = MyGetTextPtr (pmTextDisplayWindow);
    myWasDirty = myTextPtr -> isDirty;

    // Save the Undo buffer
    EdText_UndoSaveSpecified (myTextPtr, IDS_UNDO_INDENT,
        UNDO_ALL_TEXT, UNDO_ALL_TEXT);
    EdText_UndoSetReplFrom (myTextPtr, 0);

    // Indent the text of the file    
    EdText_Indent (pmTextDisplayWindow, myTextPtr);

    // Set the text to be replaced by undo    
    EdText_UndoSetReplTo (myTextPtr, myTextPtr -> textLen);
    
    // Set the scroll bars anew
    MySynchScrollBarsToText (pmTextDisplayWindow);
    
    // Create the new mark menu
    EdWin_MarkMenuCreate (MyGetEditWindow (pmTextDisplayWindow));
    
    // Handle the removal of all errors
    // TW Handle error removal

    // If necessary, handle the change in window title
    if (myWasDirty != myTextPtr -> isDirty)
    {
    	EdWin_SetWindowTitle (pmTextDisplayWindow);
    }

    // Display the indented file    
    MyCaretDisplay (pmTextDisplayWindow);
    MyDisplayText (pmTextDisplayWindow, FALSE, ALL_LINES, ALL_LINES);
    
    EdWin_ShowStatus (pmTextDisplayWindow, "%s Indented: %d Lines",
        EdDisp_GetPathForDisplay (pmTextDisplayWindow, USE_FILENAME, 
    	    DONT_SHOW_DIRTY_MARK, myPathBuffer), myTextPtr -> numLines);
} // MyEditIndent


/************************************************************************/
/* MyEditInsert								*/
/************************************************************************/
static void	MyEditInsert (HWND pmTextDisplayWindow, int pmInsertSkeleton)
{
    TextPtr	myTextPtr;
    int		myFromLine, myFromCol, myToLine, myToCol, myFromPos;
    char	*myBuffer, *mySuffix;
    int		myBufferSize;
    int		myAction;
    int		myLinesInserted;
    
    myTextPtr = MyGetTextPtr (pmTextDisplayWindow);
    
    EdText_GetSelectionLinesCols (myTextPtr, &myFromLine, &myFromCol, 
				  &myToLine, &myToCol);
    // Calculate the selection for lines for undo purposes
    myFromPos = myTextPtr -> selFrom - myFromCol;
    
    EdText_UndoSaveSpecified (myTextPtr, IDS_UNDO_INSERT, myFromPos, myFromPos);
    EdText_UndoSetReplFrom (myTextPtr, myFromPos);

    // Get the text to be inserted
    EdMenu_GetText (INSERT_MENU_TEXT, pmInsertSkeleton, &myBuffer, &mySuffix,
        &myBufferSize);
    
    // Insert the text
    EdText_SetSelection1 (myTextPtr, myFromPos, myFromPos);
    
    myAction = EdText_InsertText (myTextPtr, myBuffer, myBufferSize,
        FALSE, &myLinesInserted);
    
    EdWin_ShowStatus (pmTextDisplayWindow, "%d Lines Inserted", 
        myLinesInserted);
    
    // Set the undo buffer
    EdText_UndoSetReplTo (myTextPtr, myTextPtr -> selFrom);
    
    if (myAction & (KEY_LONGEST_LINE_CHANGED | KEY_NUM_LINES_CHANGED))
    {
    	MySynchScrollBarsToText (pmTextDisplayWindow);
    }
    
    if (myAction & KEY_MARK_REMOVED)
    {
        EdWin_MarkMenuCreate (MyGetEditWindow (pmTextDisplayWindow));
    }

    if (myAction & KEY_ERROR_REMOVED)
    {
    	// Don't do anything -- EdWin_ShowStatus (pmTextDisplayWindow, " ");
    }

    if (myAction & KEY_FILE_DIRTY)
    {
    	EdWin_SetWindowTitle (pmTextDisplayWindow);
    }

    // Redraw the screen    
    MyCaretDisplay (pmTextDisplayWindow);
    MyDisplayText (pmTextDisplayWindow, FALSE, ALL_LINES, ALL_LINES);
    
    MyScrollToSelection (pmTextDisplayWindow, SCROLL_NORMAL, FALSE);
} // MyEditInsert


/************************************************************************/
/* MyFileNew								*/
/************************************************************************/
static void	MyFileNew (HWND pmTextDisplayWindow, int pmNewSkeleton)
{
    TextDisplayWindowInfoPtr	myInfo;
    char			myClassName [1024], myProgramName [1024];
    HINSTANCE			myApplicationInstance;
    char			myGenericClassName [256], myGenericProgramName [256];
    char			*myBuffer, *mySuffix, *myNewBuffer, *mySrc, *myDest;
    DWORD			myFileSize, myNewFileSize, cnt;
    DWORD			myGenericClassNameLen, myGenericProgramNameLen;
    BOOL			myClassNameAskedFor = FALSE;
    BOOL			myProgramNameAskedFor = FALSE;

    // Debug messages
    EdFail_AddMessage (NORMAL, "New File - Window: %x - Skel: %d", 
    		       pmTextDisplayWindow, pmNewSkeleton);
    
    // Get the window information
    myInfo = MyGetTextDisplayWindowInfo (pmTextDisplayWindow);
    
    // Allow changes to be saved
    if (!EdDisp_CheckForClose (pmTextDisplayWindow))
        return;

    // If the window we're closing is the main window, we need to make
    // the main a file name rather than a window, or, if the window has no
    // associated file, we need to float the main.
    EdDisp_HandleClose (pmTextDisplayWindow);

    EdText_InitializeTextRecord (MyGetTextPtr (pmTextDisplayWindow));  

    myClassName [0] = 0;
    myProgramName [0] = 0;

    if (pmNewSkeleton != 0)
    {
    	// Get the class name
    	myApplicationInstance = 
            (HINSTANCE) GetWindowLong (pmTextDisplayWindow, GWL_HINSTANCE);
        
    	EdGUI_LoadString (IDS_CLASSNAMEHOLDER, myGenericClassName,
    			  sizeof (myGenericClassName));
    	EdGUI_LoadString (IDS_PROGRAMNAMEHOLDER, myGenericProgramName,
    			  sizeof (myGenericProgramName));

	// Get the information for the menu entry
	EdMenu_GetText (NEW_MENU_TEXT, pmNewSkeleton, &myBuffer, &mySuffix, 
			&myFileSize);
	
	// Copy the buffer to a new buffer, replacing all occurances of
	// myGenericClassName with myClassName
	myNewBuffer = (char *) malloc (myFileSize + 3000);
	// TW Check for NULL
	myNewFileSize = myFileSize;
	
	mySrc = myBuffer;
	myDest = myNewBuffer;
	myGenericClassNameLen = strlen (myGenericClassName);
	myGenericProgramNameLen = strlen (myGenericProgramName);

	for (cnt = 0 ; cnt < myFileSize ; cnt++)
	{
    	    if (strncmp (mySrc, myGenericClassName, myGenericClassNameLen) == 0)
    	    {
		if (!myClassNameAskedFor)
		{
		    BOOL    myResult;

		    Ed_EnableAllWindows (FALSE, pmTextDisplayWindow);
                    myResult = DialogBoxParam (myApplicationInstance, 
				   MAKEINTRESOURCE (GET_CLASSNAME_DIALOG),
	    			   MyGetEditWindow (pmTextDisplayWindow), 
	    			   MyGetClassNameDialogProcedure, 
	    			   (LPARAM) myClassName);
		    Ed_EnableAllWindows (TRUE, pmTextDisplayWindow);

		    if (!myResult)
	            {
			free (myNewBuffer);
	                return;
	            }
		    myClassNameAskedFor = TRUE;
		}
    	
    	    	*myDest = 0;
    	    	strcat (myDest, myClassName);
		cnt += (myGenericClassNameLen - 1);
		mySrc += myGenericClassNameLen;
    	    	myDest += strlen (myClassName);
    	    	myNewFileSize += strlen (myClassName) - myGenericClassNameLen;
    	    }
    	    else if (strncmp (mySrc, myGenericProgramName, myGenericProgramNameLen) == 0)
	    {
		if (!myProgramNameAskedFor)
		{
		    BOOL    myResult;

		    Ed_EnableAllWindows (FALSE, pmTextDisplayWindow);
                    myResult = DialogBoxParam (myApplicationInstance, 
			    MAKEINTRESOURCE (GET_PROGRAMNAME_DIALOG),
	    		    MyGetEditWindow (pmTextDisplayWindow), 
	    		    MyGetProgramNameDialogProcedure, 
	    		    (LPARAM) myProgramName);
		    Ed_EnableAllWindows (TRUE, pmTextDisplayWindow);

		    if (!myResult)
	            {
			free (myNewBuffer);
	                return;
	            }
		    myProgramNameAskedFor = TRUE;
		}
    	
    	    	*myDest = 0;
    	    	strcat (myDest, myProgramName);
		cnt += (myGenericProgramNameLen - 1);
		mySrc += myGenericProgramNameLen;
    	    	myDest += strlen (myProgramName);
    	    	myNewFileSize += strlen (myProgramName) - myGenericProgramNameLen;
	    }
	    else
    	    {
    	    	*myDest++ = *mySrc++;
    	    }
    	}
	
	// Determine the language of the file.
        myInfo -> suffix = mySuffix;
	MyDetermineLanguage (myInfo);

	// Set the file text language to the environment language
    	EdText_SetNewText (myInfo -> text, myNewBuffer, myNewFileSize, 
    			   ERASE_UNDO);

	// The file starts dirty, so it must be saved
    	myInfo -> text -> isDirty = TRUE;		   
        myInfo -> suffix = mySuffix;
	free (myNewBuffer);    			   
    }
    else
    {
	// Determine the language of the file.
	MyDetermineLanguage (myInfo);
    }

    myInfo -> topLine = 0;
    myInfo -> leftCol = 0;
    
    MySynchScrollBarsToText (pmTextDisplayWindow);
    
    MyDisplayText (pmTextDisplayWindow, FALSE, ALL_LINES, ALL_LINES);
    MyCaretDisplay (pmTextDisplayWindow);
    
    if (myInfo -> pathName != NULL)
    {
    	free (myInfo -> pathName);
    }
    
    if (myInfo -> pseudoPathName != NULL)
    {
    	free (myInfo -> pseudoPathName);
    }
    
    myInfo -> pathName = NULL;
    myInfo -> pseudoPathName = NULL;
    myInfo -> unnamedNumber = EdFile_GetUnnamedNumber ();
    myInfo -> isExample = FALSE;
    myInfo -> exampleName = NULL;
    myInfo -> isMain = FALSE;
    myInfo -> isReallyJava = FALSE;
    myInfo -> isReallyTuring = FALSE;
            
    //
    // Handle the changes to the parent window        
    //
    
    // Create new mark menu
    EdWin_MarkMenuCreate (MyGetEditWindow (pmTextDisplayWindow));
    
    // Change the name of the window
    EdWin_SetWindowTitle (pmTextDisplayWindow);

    // Output a status message
    if (pmNewSkeleton == 0)
    {
    	EdWin_ShowStatus (pmTextDisplayWindow, "New File");
    }
    else if (strlen (myClassName) > 0)
    {
    	EdWin_ShowStatus (pmTextDisplayWindow, "New %s - %s", 
    	    EdMenu_GetName (NEW_MENU_TEXT, pmNewSkeleton), myClassName);
    }
    else if (strlen (myProgramName) > 0)
    {
    	EdWin_ShowStatus (pmTextDisplayWindow, "New %s - %s", 
    	    EdMenu_GetName (NEW_MENU_TEXT, pmNewSkeleton), myProgramName);
    }
    else
    {
    	EdWin_ShowStatus (pmTextDisplayWindow, "New %s", 
    	    EdMenu_GetName (NEW_MENU_TEXT, pmNewSkeleton));
    }
        
    // Tell the Turing File Manager that the file has been opened
    EdRun_NotifyFileOpened (pmTextDisplayWindow);
    
    // Draw the new cursor position
    EdWin_ShowCursorPosition (pmTextDisplayWindow); 
} // MyFileNew


/************************************************************************/
/* MyFileOpen								*/
/*									*/
/* If pmRecentNumber is not equal to USE_OPEN_DIALOG or	USE_PATHNAME, 	*/
/* then this procedure opens up the pmRecentNumber'th file in the list	*/
/* of recent files. Otherwise it uses an Open File dialog box if	*/
/* USE_OPEN_DIALOG is specified or the passed in path name if 		*/
/* USE_PATHNAME is specified.						*/
/************************************************************************/
static BOOL	MyFileOpen (HWND pmTextDisplayWindow, const char *pmPathName,
			    const char *pmExampleName, BOOL pmCheckForClose)
{
    FilePath			myPathName;
    DWORD			myFileSize;
    char			*myBuffer;
    TextDisplayWindowInfoPtr	myInfo;

    // Debug messages
    if (pmPathName != NULL)
    {
	if (pmExampleName == NULL)
	{
    	    EdFail_AddMessage (NORMAL, 
		"Open File (1) - Window: %x - Name: %s - Example: <null>", 
		pmTextDisplayWindow, pmPathName);
	}
	else
	{
    	    EdFail_AddMessage (NORMAL, 
		"Open File (1) - Window: %x - Name: %s - Example: %s", 
		pmTextDisplayWindow, pmPathName, pmExampleName);
	}
    }
    else
    {
    	EdFail_AddMessage (NORMAL, "Open File (1) - Window: %x - Name: <null>", 
    		       	   pmTextDisplayWindow);
    }    		       	  
    		       
    // Get the window information
    myInfo = MyGetTextDisplayWindowInfo (pmTextDisplayWindow);
    
    // Allow changes to be saved
    if (pmCheckForClose)
    {
    	if (!EdDisp_CheckForClose (pmTextDisplayWindow))
    	{
            return FALSE;
        }
    }

    // Get the actual path to open        
    if (pmPathName == NULL)
    {
        // Get the path name from a Open File dialog
        if (!EdGUI_GetOpenFile (pmTextDisplayWindow, myPathName))
        {
      	    return FALSE;
      	}
    }
    else
    {
    	// Use the passed in pathname
    	strcpy (myPathName, pmPathName);
    }

    if (EdGlob_EndsWith (myPathName, ".class"))
    {
    	FilePath	myJavaPath;
    	
    	strcpy (myJavaPath, myPathName);
    	myJavaPath [EdFile_GetFileSuffix (myJavaPath) - myJavaPath] = 0;
    	strcat (myJavaPath, ".java");
    	if (EdFile_FileExists (myJavaPath))
    	{
    	    int	myResult;
    	    
	    myResult = EdGUI_Message1 (pmTextDisplayWindow, MB_YESNO, 
	    		    IDS_CANT_OPEN_CLASS_TITLE,
		            IDS_CANT_OPEN_CLASS_USE_JAVA, 
		            EdFile_GetFileName (myPathName),
		            EdFile_GetFileName (myJavaPath));
	    if (myResult == IDYES)
	    {		            
	        strcpy (myPathName, myJavaPath);
	    }
	    else
	    {
	    	return FALSE;
	    }
	}
	else
	{
	    EdGUI_Message1 (pmTextDisplayWindow, 0, IDS_CANT_OPEN_CLASS_TITLE,
		            IDS_CANT_OPEN_CLASS, 
		            EdFile_GetFileName (pmPathName));
	    return FALSE;
	}
    }

    // Open the pathname    
    myBuffer = EdFile_ReadFileIntoBuffer (pmTextDisplayWindow, 
        myPathName, &myFileSize, ERROR_IF_NOT_FOUND);
    if (myBuffer == NULL)
    {
    	EdRec_RemoveFile (myPathName);
    	return FALSE;
    }    

    // If the window we're closing is the main window, we need to make
    // the main a file name rather than a window, or, if the window has no
    // associated file, we need to float the main.
    EdDisp_HandleClose (pmTextDisplayWindow);

    // If opening an example, don't make it the active directory.
    if (pmExampleName == NULL)
    {
	EdFile_SetActiveDirectory (myPathName, FILE_PATH);
    }

    //	
    // Process the file
    //
    
    MySetLanguageFromPath (myInfo, myPathName);

    // Turn off syntax colouring if filename doesn't end in ".java"

    EdText_SetNewText (myInfo -> text, myBuffer, myFileSize, ERASE_UNDO);

    free (myBuffer);
        
    myInfo -> topLine = 0;
    myInfo -> leftCol = 0;
    
    MySynchScrollBarsToText (pmTextDisplayWindow);
    
    MyDisplayText (pmTextDisplayWindow, FALSE, ALL_LINES, ALL_LINES);
    MyCaretMove (pmTextDisplayWindow);

    // Save the path name    
    if (myInfo -> pathName != NULL)
    {
    	free (myInfo -> pathName);
    }
    
    if (myInfo -> pseudoPathName != NULL)
    {
    	free (myInfo -> pseudoPathName);
    }

    if (pmExampleName == NULL)
    {
	myInfo -> pathName = EdGlob_MallocString (myPathName);
	myInfo -> exampleName = NULL;
    }
    else
    {
	myInfo -> pathName = NULL;
	myInfo -> exampleName = EdGlob_MallocString (pmExampleName);
	myInfo -> isExample = TRUE;
    }
    myInfo -> pseudoPathName = NULL;
    myInfo -> isMain = FALSE;
    
    // Check if the new file name is the same as the main program
    if (EdRun_IsMainPathName (myPathName))
    {
        EdRun_FixMain (pmTextDisplayWindow);
    }
        
    //
    // Handle the changes to the parent window        
    //
    
    // Add to the recent files list (auto updates the Recent Files menu)
    if (pmExampleName == NULL)
    {
	EdRec_AddFile (myPathName, ADD_TO_TOP);
    }

    // Create new mark menu
    EdWin_MarkMenuCreate (MyGetEditWindow (pmTextDisplayWindow));
    
    // Change the name of the window
    EdWin_SetWindowTitle (pmTextDisplayWindow);

    // Output a status message
    if (pmExampleName == NULL)
    {
	EdWin_ShowStatus (pmTextDisplayWindow, "%s: %d Lines", 
			  EdFile_GetFileName (myPathName), 
			  MyGetTextPtr (pmTextDisplayWindow) -> numLines);
    }
    else
    {
	EdWin_ShowStatus (pmTextDisplayWindow, "%s: %d Lines", pmExampleName,
			  MyGetTextPtr (pmTextDisplayWindow) -> numLines);
    }

    // Draw the new cursor position
    EdWin_ShowCursorPosition (pmTextDisplayWindow); 

    // Tell the Turing File Manager that the file has been opened
    EdRun_NotifyFileOpened (pmTextDisplayWindow);
    
    // Load all the errors, if any    
    EdErr_LoadErrors (pmTextDisplayWindow);
    
    // Debug messages
    EdFail_AddMessage (NORMAL, "Open File (2) - fileno: %d - sizeptr: %x", 
    		       myInfo -> turingFileNo, myInfo -> turingSizePtr);
    return TRUE;
} // MyFileOpen


/****************************************************************/
/* MyFileRevert							*/
/****************************************************************/
static void	MyFileRevert (HWND pmTextDisplayWindow)
{
    FilePath			myPathName;
    DWORD			myFileSize;
    char			*myBuffer;
    TextDisplayWindowInfoPtr	myInfo;

    // Debug messages
    EdFail_AddMessage (NORMAL, "Revert File - Window: %x", pmTextDisplayWindow);
    		       
    // Get the window information
    myInfo = MyGetTextDisplayWindowInfo (pmTextDisplayWindow);
    
    // Allow changes to be saved
    if (!MyCheckForRevert (pmTextDisplayWindow))
    {
        return;
    }

    // Get the actual path to open        
    if (myInfo -> pathName == NULL)
    {
    	EdFail_Warn (IDS_CANT_REVERT_NO_NAME_FILE, __FILE__, __LINE__, 0);
       	return;
    }

    // Open the pathname    
    myBuffer = EdFile_ReadFileIntoBuffer (pmTextDisplayWindow, 
        myInfo -> pathName, &myFileSize, ERROR_IF_NOT_FOUND);
    if (myBuffer == NULL)
    {
    	EdRec_RemoveFile (myPathName);
    	EdWin_ShowStatus (pmTextDisplayWindow, "Revert Failed");
    	return;
    }    

    // Save the Undo buffer
    EdText_UndoSaveSpecified (myInfo -> text, IDS_UNDO_REVERT,
        UNDO_ALL_TEXT, UNDO_ALL_TEXT);
    EdText_UndoSetReplFrom (myInfo -> text, 0);

    // Process the file
    //
    EdText_SetNewText (myInfo -> text, myBuffer, myFileSize, DONT_ERASE_UNDO);
    
    // Set the text to be replaced by undo    
    EdText_UndoSetReplTo (myInfo -> text, myInfo -> text -> textLen);
    
    free (myBuffer);
        
    myInfo -> topLine = 0;
    myInfo -> leftCol = 0;
    
    MySynchScrollBarsToText (pmTextDisplayWindow);
    
    MyDisplayText (pmTextDisplayWindow, FALSE, ALL_LINES, ALL_LINES);
    MyCaretMove (pmTextDisplayWindow);

    //
    // Handle the changes to the parent window        
    //
    
    // Create new mark menu
    EdWin_MarkMenuCreate (MyGetEditWindow (pmTextDisplayWindow));
    
    // Change the name of the window
    EdWin_SetWindowTitle (pmTextDisplayWindow);

    // Output a status message
    EdWin_ShowStatus (pmTextDisplayWindow, "%s: Reverted - %d Lines", 
        EdFile_GetFileName (myInfo -> pathName), 
        MyGetTextPtr (pmTextDisplayWindow) -> numLines);
        
    // Draw the new cursor position
    EdWin_ShowCursorPosition (pmTextDisplayWindow); 
} // MyFileRevert


/****************************************************************/
/* MyFileSave							*/
/*								*/
/* If pmSaveAs is false, save the file with the old name unless	*/
/* it hasn't got one.  If pmSaveAs is true, then save the file	*/
/* with a name specified in the Save file dialog.  Returns TRUE */
/* if the file was successfully written.			*/
/****************************************************************/
static BOOL	MyFileSave (HWND pmTextDisplayWindow, BOOL pmSaveAs, 
			    BOOL pmSelection)
{
    BOOL			myNewPathName;
    FilePath			myPathName;
    TextDisplayWindowInfoPtr	myInfo;
    BOOL			myPrevSyntaxColour;
    int				mySuffixType = USE_NO_SUFFIX;
    char			myPackageName [1024], myClassName [1024];
    BOOL			myProgramNameRead, myClassNameRead;
    int				myBytesSaved, myLinesSaved;
    HWND			myEditWindow, myTextDisplayWindow;
    
    // Get the window information
    myInfo = MyGetTextDisplayWindowInfo (pmTextDisplayWindow);
    
    // Debugger message
    EdFail_AddMessage (NORMAL, "Save File (1) - Window: %x - SaveAs: %d"
    		       " - Sel: %d - fileno: %d - sizeptr: %x",
    		       pmTextDisplayWindow, pmSaveAs, pmSelection,
    		       myInfo -> turingFileNo, myInfo -> turingSizePtr);

    // Save the file
    myNewPathName = FALSE;
    myProgramNameRead = FALSE;
    myClassNameRead = FALSE;
    
    // Show status
    if (pmSelection)
    {
    	EdWin_ShowStatus (pmTextDisplayWindow, "Saving Selection");
    }
    else if (pmSaveAs || (myInfo -> pathName == NULL))
    {
    	EdWin_ShowStatus (pmTextDisplayWindow, "Saving as New File");
    }
    else
    {
    	EdWin_ShowStatus (pmTextDisplayWindow, "Saving File");
    }

    if (pmSaveAs || (myInfo -> pathName == NULL))
    {
	if ((gProgram.isJava && (myInfo -> suffix == NULL) && 
	    (myInfo -> pathName == NULL)) || myInfo -> text -> isJava)
	{	    
	    EdJava_GetPackageAndClass (myInfo -> text -> text, 
	        myInfo -> text -> textLen, myPackageName, myClassName);
    	    myClassNameRead = TRUE;
    	    mySuffixType = USE_JAVA_SUFFIX;
	}
	else if ((gProgram.isTuring && (myInfo -> suffix == NULL) && 
		 (myInfo -> pathName == NULL)) || myInfo -> text -> isTuring)
	{	    
	    EdTur_GetProgramName (myInfo -> text -> text, 
	        myInfo -> text -> textLen, myClassName);
    	    myProgramNameRead = TRUE;
    	    mySuffixType = USE_TURING_SUFFIX;
	}
	else if (myInfo -> suffix != NULL)
	{
	    mySuffixType = USE_MENU_SUFFIX;
	    myClassName [0] = 0;
	}
	else
	{
	    myClassName [0] = 0;
	}

    	// Get the path name from a Open File dialog
    	if (myInfo -> pathName != NULL)
    	{
    	    //
    	    // This section is executed if there was a previous name for 
    	    // this file.
    	    //
    	    if (pmSelection)
    	    {
    	    	strcpy (myPathName, myInfo -> pathName);
    	    	strcat (myPathName, " Selection");
    	    }
    	    else
    	    {
    	    	// Use the previous path name if there isn't a recognizable
    	    	// class name.  If there is, use the class name.
    	    	if (myClassName [0] == 0)
    	    	{
    	    	    strcpy (myPathName, myInfo -> pathName);
    	    	}
    	    	else
    	    	{
    	    	    strcpy (myPathName, myClassName);
    	    	    MyAddFileSuffix (myPathName, mySuffixType, 
				     myInfo -> suffix);
    	    	}
    	    }
    	}
    	else if (pmSelection)
    	{
    	    strcpy (myPathName, "Unnamed Selection");
    	}
    	else
    	{
    	    //
    	    // This section is executed if there was no previous name for 
    	    // this file.
    	    //
    	    if (myClassName [0] == 0)
    	    {
    	    	strcpy (myPathName, "Unnamed");
    	    }
    	    else
    	    {
    	    	strcpy (myPathName, myClassName);
    	    }
    	    MyAddFileSuffix (myPathName, mySuffixType, myInfo -> suffix);
    	}
    	
    	if (!EdGUI_GetSaveFile (pmTextDisplayWindow, myPathName, pmSelection))
    	{
    	    EdWin_ShowStatus (pmTextDisplayWindow, "Save Cancelled");
            return FALSE;
        }
        
        // If there is no file suffix, consider adding .t (if it's a pref)
        if ((stProperties.addSuffix) && (mySuffixType != USE_NO_SUFFIX))
        {
            MyAddFileSuffix (myPathName, mySuffixType, myInfo -> suffix);
        }
        
    	if (EdDisp_GetWindowWithMatchingPathName (myPathName, &myEditWindow, 
						  &myTextDisplayWindow))
    	{
	    // We're allowed to do a Save As with the same file name as before.
	    if (pmTextDisplayWindow != myTextDisplayWindow)
	    {
    		EdGUI_Message1 (pmTextDisplayWindow, 0, 
    	    	    IDS_FILE_SAVE_DISALLOWED_TITLE, IDS_FILE_ALREADY_OPEN);
    		EdWin_ShowStatus (pmTextDisplayWindow, "Save cancelled");
    		return FALSE;
	    }
    	}

        myNewPathName = TRUE;
    }
    else
    {
    	strcpy (myPathName, myInfo -> pathName);
    	myClassNameRead = FALSE;
    }
    
    // Let's see if the class name matches with the path name and warn
    // the user if it doesn't.  Then check with the package name and
    // the directory names.
    if (EdGlob_EndsWith (myPathName, JAVA_SUFFIX))
    {
	if (!myClassNameRead)
	{	    
	    EdJava_GetPackageAndClass (myInfo -> text -> text, 
	        myInfo -> text -> textLen, myPackageName, myClassName);
	}
		        
	if (myClassName [0] != 0)
	{
	    strcat (myClassName, JAVA_SUFFIX);
	    if (strcmp (myClassName, EdFile_GetFileName (myPathName)) != 0)
	    {
	    	int	myResult;
	    	    
	    	myClassName [strlen (myClassName) - 5] = 0;
	    	myResult = EdGUI_Message1 (pmTextDisplayWindow, 
	    	    MB_YESNO | MB_DEFBUTTON2, IDS_ILLEGAL_FILE_NAME_TITLE, 
	    	    IDS_ILLEGAL_FILE_NAME, myClassName, 
	    	    EdFile_GetFileName (myPathName));
	    	
	    	if (myResult == IDNO)
	    	{
    	    	    EdWin_ShowStatus (pmTextDisplayWindow, "Save Cancelled");
	    	    return FALSE;
	    	}
	    } // if class name != file name
	    else
	    {
	    	myClassName [strlen (myClassName) - 5] = 0;
	    }
	} // if found class name
	
	if (myPackageName [0] != 0)
	{
	    char	*myPtr;
	    char	myPackagePathName [1024];
	    int		myPackageNameLen;
	    int		cnt;
	    
	    myPackageNameLen = strlen (myPackageName);
	    strcpy (myPackagePathName, myPackageName);
	    for (cnt = 0 ; cnt < myPackageNameLen ; cnt++)
	    {
	    	if (myPackagePathName [cnt] == '.')
	    	{
	    	    myPackagePathName [cnt] = '\\';
	    	}
	    }
	    myPtr = strrchr (myPathName, '\\');
	    if ((myPathName + myPackageNameLen > myPtr) ||
	        (_strnicmp (myPtr - myPackageNameLen, myPackagePathName,
	        	   myPackageNameLen) != 0))
	    {
	    	int	myResult;
	    	    
	    	myResult = EdGUI_Message1 (pmTextDisplayWindow, 
	    	    MB_YESNO | MB_DEFBUTTON2, IDS_ILLEGAL_PATH_NAME_TITLE, 
	    	    IDS_ILLEGAL_PATH_NAME, myPackageName, myPathName,
	    	    myPackagePathName, myClassName);
	    	
	    	if (myResult == IDNO)
	    	{
    	    	    EdWin_ShowStatus (pmTextDisplayWindow, "Save Cancelled");
	    	    return FALSE;
	    	}
	    } // if package name ! found in path name
	} // if found package name
    } // if path ends in .java
   
    // Save the text to the file.  If it fails, return immediately
    if (!EdFile_SaveTextToFile (pmTextDisplayWindow, myPathName, 
    				MyGetTextPtr (pmTextDisplayWindow), 
        			pmSelection, &myBytesSaved, &myLinesSaved))
    {
	EdWin_ShowStatus (pmTextDisplayWindow, "Save Failed");
    	return FALSE;
    }
    
    if (myNewPathName && !pmSelection)
    {
    	if (myInfo -> pathName != NULL)
    	{
    	    free (myInfo -> pathName);
    	}
    	
    	myInfo -> isExample = FALSE;
    	if (myInfo -> exampleName != NULL)
    	{
    	    free (myInfo -> exampleName);
    	}

    	if (myInfo -> pseudoPathName != NULL)
    	{
    	    free (myInfo -> pseudoPathName);
        }
    	
        myInfo -> pathName = (char *) malloc (strlen (myPathName) + 1);
    	strcpy (myInfo -> pathName, myPathName);
        myInfo -> pseudoPathName = NULL;
	myInfo -> suffix = NULL;

	// Set the active directory to the directory you just saved in.
	EdFile_SetActiveDirectory (myPathName, FILE_PATH);
        
    	// Tell the Turing File Manager that the file has been renamed.
        EdRun_NotifyFileSaved (pmTextDisplayWindow);
        
    	// Turn off syntax colouring if filename doesn't end in ".java"
    	myPrevSyntaxColour = myInfo -> syntaxColour;
    	
	if (MySetLanguageFromPath (myInfo, myPathName))
	{
	    // Create new mark menu
	    EdWin_MarkMenuCreate (MyGetEditWindow (pmTextDisplayWindow));
	}
	
    	if (myPrevSyntaxColour != myInfo -> syntaxColour)
    	{
    	    // Now display the text with/without syntax colouring
    	    MyDisplayText (pmTextDisplayWindow, FALSE, ALL_LINES, ALL_LINES);
    	}
    
        // Check if the new file name is the same as the main program
        if (EdRun_IsMainPathName (myPathName))
        {
            EdRun_FixMain (pmTextDisplayWindow);
        }
        
    	// Change the name of the window
    	EdWin_SetWindowTitle (pmTextDisplayWindow);
    	
    	// Add to the recent files list
    	EdRec_AddFile (myPathName, ADD_TO_TOP);
    }
    
    if (pmSelection)
    {
    	EdWin_ShowStatus (pmTextDisplayWindow, 
    	    "Selection saved to \"%s\" - %d lines - %d bytes", 
    	    EdFile_GetFileName (myPathName), myLinesSaved, myBytesSaved);
    }
    else
    {
    	if (myNewPathName)
    	{
    	    EdWin_ShowStatus (pmTextDisplayWindow, 
    	    	"Saved as %s - %d lines - %d bytes", 
    	        EdFile_GetFileName (myPathName), myLinesSaved, myBytesSaved);
    	}
    	else
    	{
    	    EdWin_ShowStatus (pmTextDisplayWindow, 
    	        "%s Saved - %d lines - %d bytes", 
    	    	EdFile_GetFileName (myPathName), myLinesSaved, myBytesSaved);
    	}
    	
    	if (myInfo -> text -> isDirty)
    	{
    	    myInfo -> text -> isDirty = FALSE;
    	    myInfo -> text -> lastSavedTextLen = myInfo -> text -> textLen;
    	    myInfo -> text -> lastSavedCRC = 
    	    				EdText_CalculateCRC (myInfo -> text);
    	    EdWin_SetWindowTitle (pmTextDisplayWindow);
    	}
    }
    
    // Debugger messages
    EdFail_AddMessage (NORMAL, "Save File (2) - name: %s - fileno: %d"
    		       " - sizeptr: %x", myPathName, myInfo -> turingFileNo, 
    		       myInfo -> turingSizePtr);

    return TRUE;
} // MyFileSave


/************************************************************************/
/* MyFindMatchingBrace							*/
/*									*/
/* In order for us to be near a brace, either (1) the selection must	*/
/* contain only one brace, or (2) the selection point must be adjacent	*/
/* to one brace.							*/
/************************************************************************/
static void	MyFindMatchingBrace (HWND pmTextDisplayWindow)
{
    TextPtr	myTextPtr;
    int		myPos;
    int		myLevel;
    BOOL	myWasSelection;
    
    // Debugger messages
    EdFail_AddMessage (NORMAL, "Match Brc (1) - Window: %x", 
    		       pmTextDisplayWindow);

    myTextPtr = MyGetTextPtr (pmTextDisplayWindow);
    myPos = EdDisp_IsNearBrace (pmTextDisplayWindow);
    myWasSelection = (myTextPtr -> selFrom == myTextPtr -> selTo);

    if (myPos == NO_BRACE_FOUND)
    {
    	EdWin_ShowStatusBold (pmTextDisplayWindow, "No brace to match");
    	MessageBeep (MB_ICONEXCLAMATION);
    	return;
    }
    
    myLevel = 0;
    
    if (myTextPtr -> text [myPos] == '{')
    {
    	// Search forward for matching }
    	while (myPos < myTextPtr -> textLen)
    	{
    	    if ((myTextPtr -> text [myPos] == '{') && 
    	        (myTextPtr -> textType [myPos] == TEXT_NORMAL))
    	    {
    	    	myLevel++;
    	    }
    	    if ((myTextPtr -> text [myPos] == '}') && 
    	        (myTextPtr -> textType [myPos] == TEXT_NORMAL))
    	    {
    	    	myLevel--;
    	    	if (myLevel == 0)
    	    	{
    	    	    break;
    	    	}
    	    }
    	    myPos++;
    	}
    	if (myLevel != 0)
    	{
    	    EdWin_ShowStatusBold (pmTextDisplayWindow, 
    	    			  "No matching ""}"" found");
    	    MessageBeep (MB_ICONEXCLAMATION);
    	    return;
    	}
    }
    else
    {
    	// Search backward for matching {
    	while (myPos >= 0)
    	{
    	    if ((myTextPtr -> text [myPos] == '}') && 
    	        (myTextPtr -> textType [myPos] == TEXT_NORMAL))
    	    {
    	    	myLevel++;
    	    }
    	    if ((myTextPtr -> text [myPos] == '{') && 
    	        (myTextPtr -> textType [myPos] == TEXT_NORMAL))
    	    {
    	    	myLevel--;
    	    	if (myLevel == 0)
    	    	{
    	    	    break;
    	    	}
    	    }
    	    myPos--;
    	}
    	if (myLevel != 0)
    	{
    	    EdWin_ShowStatusBold (pmTextDisplayWindow, 
    	    			  "No matching ""{"" found");
    	    MessageBeep (MB_ICONEXCLAMATION);
    	    return;
    	}
    	
    }
    
    // Reset the Undo buffer
    EdText_UndoSetMoreKeystrokes (myTextPtr, FALSE);
    	
    EdText_SetSelection1 (myTextPtr, myPos, myPos + 1);

    if (myWasSelection)
    {
    	MyCaretHide (pmTextDisplayWindow);
    }
    
    MyScrollToSelection (pmTextDisplayWindow, SCROLL_NORMAL, TRUE);
    
    EdWin_ShowStatus (pmTextDisplayWindow, "Matching brace found");
        
    // Debug messages
    EdFail_AddMessage (NORMAL, "Match Brc (2) - sz:%d:%d - slFr:%d - slTo:%d"
        " - nlA:%d", myTextPtr -> numLines, myTextPtr -> textLen, 
        myTextPtr -> selFrom, myTextPtr -> selTo, 
        myTextPtr -> newLineAdded);
} // MyFindMatchingBrace


/************************************************************************/
/* MyFindNext								*/
/************************************************************************/
static void	MyFindNext (HWND pmTextDisplayWindow, BOOL pmForward)
{
    int				myFindStatus;
    BOOL			myWasSelection;
    TextDisplayWindowInfoPtr	myInfo;
    TextPtr			myTextPtr = MyGetTextPtr (pmTextDisplayWindow);
			            	    
    // Debug messages
    EdFail_AddMessage (NORMAL, "Find (1) - Window: %x - Forward: %d", 
    		       pmTextDisplayWindow, pmForward);

    myWasSelection = (myTextPtr -> selFrom == myTextPtr -> selTo);
    myFindStatus = EdSearch_Find (myTextPtr, pmForward, WRAP_ALLOWED);
    
    // Display the status of the find
    MyFindStatusDisplay (pmTextDisplayWindow, myFindStatus, myWasSelection);

    // Close the Find dialog box after a successful search
    if ((myFindStatus != NOT_FOUND) && EdSearch_FindDialogVisible ())
    {
	EdSearch_CloseDialog (CLOSE_DIALOG);
    }
    else if (EdSearch_FindReplaceDialog != NULL)
    {
    	//
    	// Move the Replace dialog so that it is not over the selection
    	//
    	
    	RECT	mySelectRect, myReplaceRect; // Both RECTS in screen coords
    	RECT	myRect;
	int	myFromLine, myFromCol, myToLine, myToCol;
	int	myLastVisibleLine;
	
	// Get the window's rectangle    	
    	GetWindowRect (EdSearch_FindReplaceDialog, &myReplaceRect);

	// Get the selection's rectangle
	myInfo = MyGetTextDisplayWindowInfo (pmTextDisplayWindow);
	
	myLastVisibleLine = myInfo -> topLine + myInfo -> windowHeight - 1;
	
    	EdText_GetSelectionLinesCols (myTextPtr, &myFromLine, &myFromCol, 
    	    &myToLine, &myToCol);
    	if (myFromLine == myToLine)
    	{
    	    if ((myInfo -> topLine <= myFromLine) &&
    	        (myFromLine <= myLastVisibleLine))
    	    {
	    	// Make rectangle contain entire selection
    	        int	mySelectionLen = myToCol - myFromCol;
    	    
    	        mySelectRect.left = max (0, (myFromCol - myInfo -> leftCol) * 
    	            stTextFontCharWidth);
                mySelectRect.top = (myFromLine - myInfo -> topLine) *
                    stTextFontCharHeight;
                mySelectRect.right = min (myInfo -> textRect.right - 
                    myInfo -> textRect.left,
                    (mySelectionLen * stTextFontCharWidth) + mySelectRect.left);
                mySelectRect.bottom = mySelectRect.top + stTextFontCharHeight;
    	    }
     	}
    	else
    	{
    	    // Make rectangle contain entire selection
            mySelectRect.top = (max (myFromLine, myInfo -> topLine) - 
            	myInfo -> topLine) * stTextFontCharHeight;
            mySelectRect.bottom = (min (myToLine, myLastVisibleLine) - 
            	myInfo -> topLine + 1) * stTextFontCharHeight;
    	    mySelectRect.left =  0;
            mySelectRect.right = myInfo -> textRect.right - 
                myInfo -> textRect.left;
    	} // if
    	
    	GetWindowRect (pmTextDisplayWindow, &myRect);
    	
    	mySelectRect.right += myInfo -> textRect.left + myRect.left;
    	mySelectRect.left += myInfo -> textRect.left + myRect.left;
    	mySelectRect.top += myInfo -> textRect.top + myRect.top;
    	mySelectRect.bottom += myInfo -> textRect.top + myRect.top;
    	
    	if (IntersectRect (&myRect, &myReplaceRect, &mySelectRect))
    	{
    	    // The selection and the Replace dialog box overlap.  We must
    	    // move the Replace dialog box.
    	    EdSearch_RepositionReplaceDialog (
    	        MyGetEditWindow (pmTextDisplayWindow), mySelectRect);
    	}
    } // if selection visible

    // Debug messages
    EdFail_AddMessage (NORMAL, 
    	"Find (2) - sz:%d:%d - slFr:%d - slTo:%d - nlA:%d",
        myTextPtr -> numLines, myTextPtr -> textLen, 
        myTextPtr -> selFrom, myTextPtr -> selTo, 
        myTextPtr -> newLineAdded);
        
    // Adjust the cursor position status
    EdWin_ShowCursorPosition (pmTextDisplayWindow);
    
    // Reset the Undo buffer
    EdText_UndoSetMoreKeystrokes (myTextPtr, FALSE);
} // MyFindNext


/************************************************************************/
/* MyFindReplace							*/
/************************************************************************/
static void	MyFindReplace (HWND pmTextDisplayWindow)
{
    TextPtr			myTextPtr = MyGetTextPtr (pmTextDisplayWindow);
			            	    
    // Debug messages
    EdFail_AddMessage (NORMAL, "Replace (1) - Window: %x", pmTextDisplayWindow);

    // Check to see if the current selection matches the Find field.
    if (EdSearch_SelectionMatchesFindWhat (myTextPtr))
    {
    	//
    	// Replace the selection
    	//
    	
    	int	myAction;
    	
    	// Save the Undo state
    	EdText_UndoSaveState (myTextPtr, IDS_UNDO_REPLACE);
    	
    	// Delete the current selection
    	myAction = EdText_SelectionDelete (myTextPtr);

    	myAction |= EdSearch_InsertReplaceText (myTextPtr);
    	
	EdText_UndoSetReplTo (myTextPtr, myTextPtr -> selFrom);

	if (myAction & (KEY_LONGEST_LINE_CHANGED | KEY_NUM_LINES_CHANGED))
    	{
    	    MySynchScrollBarsToText (pmTextDisplayWindow);
    	}
    
    	if (myAction & KEY_MARK_REMOVED)
    	{
            EdWin_MarkMenuCreate (MyGetEditWindow (pmTextDisplayWindow));
    	}

    	if (myAction & KEY_ERROR_REMOVED)
    	{
    	    // Don't do anything -- EdWin_ShowStatus (pmTextDisplayWindow, " ");
    	}

    	if (myAction & KEY_FILE_DIRTY)
    	{
    	    EdWin_SetWindowTitle (pmTextDisplayWindow);
    	}
    
    	if (myAction & KEY_REDRAW_FROM_SELECTION)
    	{
            MyCaretDisplay (pmTextDisplayWindow);
    	    MyDisplayText (pmTextDisplayWindow, FALSE, ALL_LINES, ALL_LINES);
        }
    	else if (myAction & KEY_UPDATE_LINE)
    	{
    	    int	myLine, myCol;
    	
            MyCaretDisplay (pmTextDisplayWindow);
            EdText_GetSelectionLocation (myTextPtr, &myLine, &myCol);
    	    MyDisplayText (pmTextDisplayWindow, FALSE, myLine, myLine);
    	}
    }
    
    MyFindNext (pmTextDisplayWindow, SEARCH_FORWARD);
        
    // Debug messages
    EdFail_AddMessage (NORMAL, 
        "Replace (2) - sz:%d:%d - slFr:%d - slTo:%d - nlA:%d",
        myTextPtr -> numLines, myTextPtr -> textLen, 
        myTextPtr -> selFrom, myTextPtr -> selTo, 
        myTextPtr -> newLineAdded);
} // MyFindReplace


/************************************************************************/
/* MyFindReplaceAll							*/
/************************************************************************/
static void	MyFindReplaceAll (HWND pmTextDisplayWindow)
{
    int				myOrigSelFrom, myOrigSelTo;
    int				myFindStatus;
    BOOL			myWasSelection;
    TextDisplayWindowInfoPtr	myInfo;
    TextPtr			myTextPtr = MyGetTextPtr (pmTextDisplayWindow);
    int				myReplaceCount;

    // Debug messages
    EdFail_AddMessage (NORMAL, "Find Repl All (1) - Window: %x", 
    		       pmTextDisplayWindow);

    myOrigSelFrom = myTextPtr -> selFrom;
    myOrigSelTo = myTextPtr -> selTo;			            	    
    myWasSelection = (myTextPtr -> selFrom == myTextPtr -> selTo);
    
    myTextPtr -> selFrom = myTextPtr -> selTo = 0;
    
    myFindStatus = EdSearch_Find (myTextPtr, SEARCH_FORWARD, NO_WRAP);
    if (myFindStatus == NOT_FOUND)
    {
    	myTextPtr -> selFrom = myOrigSelFrom;
    	myTextPtr -> selTo = myOrigSelTo;
    	
    	// Display the status of the find
    	MyFindStatusDisplay (pmTextDisplayWindow, myFindStatus, myWasSelection);
    	return;
    }

    myReplaceCount = 0;
    
    // Save the Undo buffer
    EdText_UndoSaveSpecified (myTextPtr, IDS_UNDO_REPLACE_ALL,
        myTextPtr -> selFrom, myTextPtr -> textLen);
    EdText_UndoSetReplFrom (myTextPtr, myTextPtr -> selFrom);

    while (myFindStatus != NOT_FOUND)
    {
    	//
    	// Replace the selection and find the next one
    	//
    	myReplaceCount++;
    		
    	EdText_SelectionDelete (myTextPtr);
	EdSearch_InsertReplaceText (myTextPtr);    	
	myFindStatus = EdSearch_Find (myTextPtr, SEARCH_FORWARD, NO_WRAP);
    }	
    
    // Set the text to be replaced by undo  
    EdText_UndoSetReplTo (myTextPtr, myTextPtr -> textLen);

    MySynchScrollBarsToText (pmTextDisplayWindow);
    
    EdWin_MarkMenuCreate (MyGetEditWindow (pmTextDisplayWindow));
    
    EdWin_SetWindowTitle (pmTextDisplayWindow);

    MyCaretDisplay (pmTextDisplayWindow);
    MyDisplayText (pmTextDisplayWindow, FALSE, ALL_LINES, ALL_LINES);

    MyScrollToSelection (pmTextDisplayWindow, SCROLL_NORMAL, TRUE);
    
    EdWin_ShowStatus (pmTextDisplayWindow, "%d Changes", myReplaceCount);

    // Close the Find dialog box after a successful search
    if (EdSearch_FindReplaceDialog != NULL)
    {
    	//
    	// Move the Replace dialog so that it is not over the selection
    	//
    	
    	RECT	mySelectRect, myReplaceRect; // Both RECTS in screen coords
    	RECT	myRect;
	int	myFromLine, myFromCol, myToLine, myToCol;
	int	myLastVisibleLine;
	
	// Get the window's rectangle    	
    	GetWindowRect (EdSearch_FindReplaceDialog, &myReplaceRect);

	// Get the selection's rectangle
	myInfo = MyGetTextDisplayWindowInfo (pmTextDisplayWindow);
	
	myLastVisibleLine = myInfo -> topLine + myInfo -> windowHeight - 1;
	
    	EdText_GetSelectionLinesCols (myTextPtr, &myFromLine, &myFromCol, 
    	    &myToLine, &myToCol);
    	if (myFromLine == myToLine)
    	{
    	    if ((myInfo -> topLine <= myFromLine) &&
    	        (myFromLine <= myLastVisibleLine))
    	    {
	    	// Make rectangle contain entire selection
    	        int	mySelectionLen = myToCol - myFromCol;
    	    
    	        mySelectRect.left = max (0, (myFromCol - myInfo -> leftCol) * 
    	            stTextFontCharWidth);
                mySelectRect.top = (myFromLine - myInfo -> topLine) *
                    stTextFontCharHeight;
                mySelectRect.right = min (myInfo -> textRect.right - 
                    myInfo -> textRect.left,
                    (mySelectionLen * stTextFontCharWidth) + mySelectRect.left);
                mySelectRect.bottom = mySelectRect.top + stTextFontCharHeight;
    	    }
     	}
    	else
    	{
    	    // Make rectangle contain entire selection
            mySelectRect.top = (max (myFromLine, myInfo -> topLine) - 
            	myInfo -> topLine) * stTextFontCharHeight;
            mySelectRect.bottom = (min (myToLine, myLastVisibleLine) - 
            	myInfo -> topLine + 1) * stTextFontCharHeight;
    	    mySelectRect.left =  0;
            mySelectRect.right = myInfo -> textRect.right - 
                myInfo -> textRect.left;
    	} // if
    	
    	GetWindowRect (pmTextDisplayWindow, &myRect);
    	
    	mySelectRect.right += myInfo -> textRect.left + myRect.left;
    	mySelectRect.left += myInfo -> textRect.left + myRect.left;
    	mySelectRect.top += myInfo -> textRect.top + myRect.top;
    	mySelectRect.bottom += myInfo -> textRect.top + myRect.top;
    	
    	if (IntersectRect (&myRect, &myReplaceRect, &mySelectRect))
    	{
    	    // The selection and the Replace dialog box overlap.  We must
    	    // move the Replace dialog box.
    	    EdSearch_RepositionReplaceDialog (
    	        MyGetEditWindow (pmTextDisplayWindow), mySelectRect);
    	}
    } // if selection visible

    // Debug messages
    EdFail_AddMessage (NORMAL, "Find Repl All (2) - sz:%d:%d - slFr:%d - "
        "slTo:%d - nlA:%d",
        myTextPtr -> numLines, myTextPtr -> textLen, 
        myTextPtr -> selFrom, myTextPtr -> selTo, 
        myTextPtr -> newLineAdded);
        
    // Adjust the cursor position status
    EdWin_ShowCursorPosition (pmTextDisplayWindow);
} // MyFindReplaceAll


/************************************************************************/
/* MyFindSelection							*/
/************************************************************************/
static void	MyFindSelection (HWND pmTextDisplayWindow)
{
    int		myFindStatus;
    BOOL	myWasSelection;
    TextPtr	myTextPtr = MyGetTextPtr (pmTextDisplayWindow);
			            	    
    myWasSelection = (myTextPtr -> selFrom == myTextPtr -> selTo);
    myFindStatus = EdSearch_FindSelectionString (myTextPtr, 
	MyGetTextDisplayWindowInfo (pmTextDisplayWindow) -> selection);
    MyFindStatusDisplay (pmTextDisplayWindow, myFindStatus, myWasSelection);

    // Adjust the cursor position status
    EdWin_ShowCursorPosition (pmTextDisplayWindow);
} // MyFindSelection


/************************************************************************/
/* MyFindStatusDisplay							*/
/************************************************************************/
static void	MyFindStatusDisplay (HWND pmTextDisplayWindow, int pmStatus, 
    				     BOOL pmWasSelection)
{
    switch (pmStatus)
    {
    	case NOT_FOUND:
    	    EdWin_ShowStatusBold (pmTextDisplayWindow, 
    	        "Search string not found");
    	    MessageBeep (MB_ICONEXCLAMATION);
    	    return;
    	case WRAPPED:
    	    EdWin_ShowStatus (pmTextDisplayWindow, 
    	        "Search wrapped around");
    	    break;
    	case FOUND:
    	    EdWin_ShowStatus (pmTextDisplayWindow, 
    	        "Search string found");
    	    break;
    	case ONE_OCCURRENCE:
    	    EdWin_ShowStatus (pmTextDisplayWindow, 
    	        "No other occurrences found");
    	    break;
    	default:
    	    EdWin_ShowStatus (pmTextDisplayWindow, 
    	        "Unknown Find Return code");
    	    break;
    } // switch    	    
    
    if (pmWasSelection)
    {
    	MyCaretHide (pmTextDisplayWindow);
    }
    
    MyScrollToSelection (pmTextDisplayWindow, SCROLL_NORMAL, TRUE);
} // MyFindStatusDisplay


/************************************************************************/
/* MyGetEditWindow							*/
/*									*/
/* Returns the application window that contains pmTextDisplayWindow.	*/
/************************************************************************/
static HWND	MyGetEditWindow (HWND pmTextDisplayWindow)
{
    HWND	myWindow;
    int		myWindowType;
        
    myWindow = EdGUI_GetTopWindow (pmTextDisplayWindow);
    
    myWindowType = GetWindowLong (myWindow, WINDOW_MEM_WINDOW_TYPE);
    if (myWindowType != WINDOW_TYPE_EDIT)
    {
    	EdFail_Fatal (IDS_BADWINDOWTYPE, __FILE__, __LINE__, 0,
    		      myWindowType, WINDOW_TYPE_EDIT);
    }

    return myWindow;
} // MyGetEditWindow


/************************************************************************/
/* MyGetKindOfProgramToRun						*/
/************************************************************************/
static int MyGetKindOfProgramToRun (HWND pmTextDisplayWindow)
{
    // Determine the file to run
    HWND			myTextDisplayWindowToRun;
    TextDisplayWindowInfoPtr	myInfo;
    BOOL			myIsApplication;
    BOOL			myIsApplet;
    BOOL			myResult;
    
    // Check to see if there's a main file to run
    if (EdRun_GetMainPathName () != NULL)
    {
    	return EdRun_GetMainProgramType ();
    }
    
    // Check to see if there's a main window to run
    myTextDisplayWindowToRun = EdRun_GetMainWindow ();
    if (myTextDisplayWindowToRun == NULL)
    {
    	// Else run the current window
    	myTextDisplayWindowToRun = pmTextDisplayWindow;
    }

    myInfo = MyGetTextDisplayWindowInfo (myTextDisplayWindowToRun);
    
    if ((myInfo -> pseudoPathName == NULL) &&
        ((myInfo -> pathName == NULL) ||
	 !EdGlob_EndsWith (myInfo -> pathName, JAVA_SUFFIX)))
    {
    	return 0;
    }
    
    EdJava_GetProgramType (myInfo -> text -> text, myInfo -> text -> textLen, 
    			   &myIsApplication, &myIsApplet);
    if (myIsApplication && myIsApplet)
    {
    	HINSTANCE	myApplicationInstance;
        
	// Get the program instance
    	myApplicationInstance = 
            (HINSTANCE) GetWindowLong (pmTextDisplayWindow, GWL_HINSTANCE);
            
	Ed_EnableAllWindows (FALSE, pmTextDisplayWindow);
        myResult = DialogBoxParam (myApplicationInstance, 
        		       MAKEINTRESOURCE (PROGRAM_TYPE_1_DIALOG),
			       MyGetEditWindow (pmTextDisplayWindow), 
			       MyGetProgramKindProcedure, 0);
	Ed_EnableAllWindows (TRUE, pmTextDisplayWindow);

	return myResult;
    } 
    else if (myIsApplication && !myIsApplet)
    {
    	return APPLICATION;
    }
    else if ((!myIsApplication) && myIsApplet)
    {
    	return APPLET;
    }
    else
    {
    	HINSTANCE	myApplicationInstance;
    
	// Get the program instance
    	myApplicationInstance = 
            (HINSTANCE) GetWindowLong (pmTextDisplayWindow, GWL_HINSTANCE);
            
	Ed_EnableAllWindows (FALSE, pmTextDisplayWindow);
        myResult =  DialogBoxParam (myApplicationInstance, 
        		       MAKEINTRESOURCE (PROGRAM_TYPE_2_DIALOG),
			       MyGetEditWindow (pmTextDisplayWindow), 
			       MyGetProgramKindProcedure, 0);
	Ed_EnableAllWindows (TRUE, pmTextDisplayWindow);

	return myResult;
    }
} // MyGetKindOfProgramToRun


/************************************************************************/
/* MyGetPathNameToRun							*/
/************************************************************************/
static const char *MyGetPathNameToRun (HWND pmTextDisplayWindow)
{
    // Determine the file to run
    HWND			myTextDisplayWindowToRun;
    TextDisplayWindowInfoPtr	myInfo;
    const char			*myPathNameToRun;
    
    // Create Pseudo-paths for all open text files
    if (!MyCreatePseudoPaths (pmTextDisplayWindow))
    {
    	// There was a file name ambiguity, abort the run.
    	EdWin_ShowStatus (pmTextDisplayWindow, "Compilation aborted");
    	return NULL;
    }
    
    // Check to see if there's a main file to run
    myPathNameToRun = EdRun_GetMainPathName ();
    if (myPathNameToRun != NULL)
    {
    	return myPathNameToRun;
    }
    
    // Check to see if there's a main window to run
    myTextDisplayWindowToRun = EdRun_GetMainWindow ();
    if (myTextDisplayWindowToRun == NULL)
    {
    	// Else run the current window
    	myTextDisplayWindowToRun = pmTextDisplayWindow;
    }
    	
    myInfo = MyGetTextDisplayWindowInfo (myTextDisplayWindowToRun);
    if (myInfo -> pseudoPathName != NULL)
    {
    	// Run under pseudo-file name
    	return myInfo -> pseudoPathName;
    }    
    else if ((myInfo -> pathName != NULL) &&
	     EdGlob_EndsWith (myInfo -> pathName, JAVA_SUFFIX))
    {
	// Run under the real file name if it ends in ".java"
	return myInfo -> pathName;
    }
    else if (myInfo -> pseudoPathName == NULL)
    {
    	// Not a java file
    	EdGUI_Message1 (pmTextDisplayWindow, 0, IDS_RUN_ERROR_TITLE,
    	    IDS_NOT_A_JAVA_FILE);
    	EdWin_ShowStatus (pmTextDisplayWindow, "Compilation aborted");
    	return NULL;
    }
    else
    {
	// Can't run files that don't end in ".java"
        EdGUI_Message1 (pmTextDisplayWindow, 0, IDS_RUN_ERROR_TITLE,
    	    IDS_FILE_DOES_NOT_END_IN_JAVA);
    	EdWin_ShowStatus (pmTextDisplayWindow, "Compilation aborted");
	return NULL;
    }	        			 
} // MyGetPathNameToRun


/************************************************************************/
/* MyGetTextDisplayWindowInfo						*/
/************************************************************************/
static TextDisplayWindowInfoPtr	MyGetTextDisplayWindowInfo (
						HWND pmTextDisplayWindow)
{
    int				myWindowType;
    TextDisplayWindowInfoPtr	myInfo;
    
    myWindowType = GetWindowLong (pmTextDisplayWindow, WINDOW_MEM_WINDOW_TYPE);
    if (myWindowType != WINDOW_TYPE_TEXT_DISPLAY)
    {
    	EdFail_Fatal (IDS_BADWINDOWTYPE, __FILE__, __LINE__, 0,
    		      myWindowType, WINDOW_TYPE_TEXT_DISPLAY);
    }
    	
    myInfo = (TextDisplayWindowInfoPtr) GetWindowLong (pmTextDisplayWindow, 
        WINDOW_MEM_WINDOW_INFO);
    if (myInfo == NULL)
    {
    	EdFail_Fatal (IDS_GETWINDOWLONGFAIL, __FILE__, __LINE__,
    		      GetLastError (), WINDOW_MEM_WINDOW_INFO);
    }
       
    return myInfo;
} // MyGetTextDisplayWindowInfo


/************************************************************************/
/* MyGetTextPtr								*/
/************************************************************************/
static TextPtr	MyGetTextPtr (HWND pmTextDisplayWindow)
{
    return MyGetTextDisplayWindowInfo (pmTextDisplayWindow) -> text;
} // MyGetTextPtr


/************************************************************************/
/* MyGotoLine								*/
/************************************************************************/
static void	MyGotoLine (HWND pmTextDisplayWindow)
{
    HINSTANCE	myApplicationInstance;
    int		myGotoLine;
    BOOL	myRedraw = FALSE;
    BOOL	myResult;

    // Debug messages
    EdFail_AddMessage (NORMAL, "GotoLine (1) - Window: %x", 
    		       pmTextDisplayWindow);

    // Get the program instance
    myApplicationInstance = 
        (HINSTANCE) GetWindowLong (pmTextDisplayWindow, GWL_HINSTANCE);
        
    Ed_EnableAllWindows (FALSE, pmTextDisplayWindow);
    myResult = DialogBoxParam (myApplicationInstance, 
        MAKEINTRESOURCE (GOTO_LINE_DIALOG),
	MyGetEditWindow (pmTextDisplayWindow), MyGotoDialogProcedure, 
	(LPARAM) &myGotoLine);
    Ed_EnableAllWindows (TRUE, pmTextDisplayWindow);

    if (myResult)
    {
    	TextPtr	myTextPtr = MyGetTextPtr (pmTextDisplayWindow);
    	
        if (EdText_SetSelection (myTextPtr, myGotoLine - 1, 0))
        {
            myRedraw = TRUE;
            MyCaretDisplay (pmTextDisplayWindow);
        }
    	// Debug messages
    	EdFail_AddMessage (NORMAL, 
    	    "GotoLn (2) - sz:%d:%d - slFr:%d - slTo:%d - nlA:%d",
            myTextPtr -> numLines, myTextPtr -> textLen, 
            myTextPtr -> selFrom, myTextPtr -> selTo, 
            myTextPtr -> newLineAdded);
        
        MyCaretMove (pmTextDisplayWindow);
        MyScrollToSelection (pmTextDisplayWindow, SCROLL_NORMAL, myRedraw);
        
        EdWin_ShowStatus (pmTextDisplayWindow, "Go to line %d", 
            myGotoLine);
    
    	// Reset the Undo buffer
        EdText_UndoSetMoreKeystrokes (myTextPtr, FALSE);
    }
} // MyGotoLine


/************************************************************************/
/* MyInitializeColours							*/
/************************************************************************/
static void	MyInitializeColours (HWND pmTextDisplayWindow)
{
    HDC	myDisplayContext = GetDC (pmTextDisplayWindow);
    
    stTextColours [TEXT_NORMAL] = 
    	GetNearestColor (myDisplayContext, CLR_NORMAL);
    stTextColours [TEXT_IDENTIFIER] = 
        GetNearestColor (myDisplayContext, CLR_IDENTIFIER);
    stTextColours [TEXT_PREDEF] = 
    	GetNearestColor (myDisplayContext, CLR_PREDEF);
    stTextColours [TEXT_KEYWORD] = 
    	GetNearestColor (myDisplayContext, CLR_KEYWORD);
    stTextColours [TEXT_DQSTRING] = 
    	GetNearestColor (myDisplayContext, CLR_STRING);
    stTextColours [TEXT_DQSTRING_CLOSE] = stTextColours [TEXT_DQSTRING];
    stTextColours [TEXT_SQSTRING] = stTextColours [TEXT_DQSTRING];
    stTextColours [TEXT_SQSTRING_CLOSE] = stTextColours [TEXT_DQSTRING];
    stTextColours [TEXT_LINE_COMMENT] = 
        GetNearestColor (myDisplayContext, CLR_LINE_COMMENT);
    stTextColours [TEXT_LINE_COMMENT_OPEN] = stTextColours [TEXT_LINE_COMMENT];
    stTextColours [TEXT_LONG_COMMENT] = 
    	GetNearestColor (myDisplayContext, CLR_LONG_COMMENT);
    stTextColours [TEXT_LONG_COMMENT_OPEN] = stTextColours [TEXT_LONG_COMMENT];
    stTextColours [TEXT_LONG_COMMENT_CLOSE] = stTextColours [TEXT_LONG_COMMENT];
    
    stSelectionColour = GetNearestColor (myDisplayContext, SELECTION_COLOUR);
    
    stTextColoursInitialized = TRUE;
    
    ReleaseDC (pmTextDisplayWindow, myDisplayContext);
} // MyInitializeColours


/************************************************************************/
/* MyIsMovementCommand							*/
/************************************************************************/
static BOOL	MyIsMovementCommand (int pmCommand)
{
    return ((COMMAND_FIRST_MOVEMENT_COMMAND <= pmCommand) &&
        (pmCommand <= COMMAND_LAST_MOVEMENT_COMMAND));
} // MyIsMovementCommand


/************************************************************************/
/* MyKeystrokeHandler							*/
/************************************************************************/
static void	MyKeystrokeHandler (HWND pmTextDisplayWindow, int pmKeystroke, 
				    BOOL pmSpecialKey)
{
    TextPtr		myTextPtr;
    UndoBufferPtr	myUndoPtr;
    int			myAction;
    char		*myDeletedChars = NULL;
    
    myTextPtr = MyGetTextPtr (pmTextDisplayWindow);

    // Set the undo buffer, if necessary
    if ((myTextPtr -> undoTail == NULL) || 
	(!myTextPtr -> undoTail -> moreKeystrokes))
    {
    	EdText_UndoSaveState (myTextPtr, IDS_UNDO_TYPING);
	EdText_UndoSetMoreKeystrokes (myTextPtr, TRUE);
    }
    myUndoPtr = myTextPtr -> undoTail;

    // Process the keystroke
    if (pmSpecialKey == NORMAL_KEY)
    {
    	myAction = EdText_Key (myTextPtr, (char) pmKeystroke, &myDeletedChars);
    }
    else if (pmSpecialKey == DELETE_KEY)
    {    
    	myAction = EdText_KeyDelete (myTextPtr, &myDeletedChars);
    }
    else if (pmSpecialKey == BACKTAB_KEY)
    {    
    	myAction = EdText_KeyBackTab (myTextPtr, &myDeletedChars);
    }

    // Adjust the cursor position status
    EdWin_ShowCursorPosition (pmTextDisplayWindow);

    // Set the undo buffer appropriately
    EdText_UndoSetReplTo (myTextPtr, myTextPtr -> selTo);
    
    // Handle the case where we're deleting characters in front of the original
    // breakpoint (using multiple \b's)
    if ((myUndoPtr != NULL) &&
	(myTextPtr -> selFrom < myUndoPtr -> replFrom))
    {
        int	myOldLength;
        char	*myNewText;
       
        // TW Assert myDeletedChars != NULL
        
        myOldLength = myUndoPtr -> textSize;
        myUndoPtr -> textSize += strlen (myDeletedChars);
        myNewText = malloc (myUndoPtr -> textSize);
        if (myNewText == NULL)
        {
	    // Free all the undo buffers
	    while (myTextPtr -> undoTail != NULL)
	    {
		EdText_UndoFreeLastBuffer (myTextPtr);
	    }
    	    EdGUI_Message1 (pmTextDisplayWindow, MB_OK | MB_ICONWARNING, 
    	        IDS_OUT_OF_TEXT_MEM_TITLE, IDS_NO_MEM_FOR_UNDO_MESSAGE);
        }
        else
        {
            memmove (myNewText, myDeletedChars, strlen (myDeletedChars));
            memmove ((char *) (myNewText + strlen (myDeletedChars)), 
                myUndoPtr -> text, myOldLength);
            if (myUndoPtr -> text != NULL)
            {
            	free (myUndoPtr -> text);
            }
            myUndoPtr -> text = myNewText;
            myUndoPtr -> replFrom = myTextPtr -> selFrom;
        }
        free (myDeletedChars);
        myDeletedChars = NULL;
    }    
    else if ((myUndoPtr != NULL) &&
	     (pmSpecialKey != NORMAL_KEY) && (myDeletedChars != NULL))
    {
        int	myOldLength;
        char	*myNewText;
       
        myOldLength = myUndoPtr -> textSize;
        myUndoPtr -> textSize += strlen (myDeletedChars);
        myNewText = realloc (myUndoPtr -> text, myUndoPtr -> textSize);
        if (myNewText == NULL)
        {
	    // Free all the undo buffers
	    while (myTextPtr -> undoTail != NULL)
	    {
		EdText_UndoFreeLastBuffer (myTextPtr);
	    }
    	    EdGUI_Message1 (pmTextDisplayWindow, MB_OK | MB_ICONWARNING, 
    	        IDS_OUT_OF_TEXT_MEM_TITLE, IDS_NO_MEM_FOR_UNDO_MESSAGE);
        }
        else
        {
	    myUndoPtr -> text = myNewText;
            memmove (&myUndoPtr -> text [myOldLength], myDeletedChars,
                strlen (myDeletedChars));
        }
        free (myDeletedChars);
        myDeletedChars = NULL;
    }    

    if (myDeletedChars != NULL)
    {
    	free (myDeletedChars);
    }
    
    if (myAction & KEY_ILLEGAL_CHAR)
    {
    	EdWin_ShowStatus (pmTextDisplayWindow,
    	    "Illegal Key '%c' (%d)", pmKeystroke, pmKeystroke);
   	MessageBeep (MB_ICONEXCLAMATION);
    	return;
    }
    
    if (myAction & KEY_OUT_OF_MEMORY)
    {
    	EdGUI_Message1 (pmTextDisplayWindow, MB_OK | MB_ICONWARNING, 
    	    IDS_OUT_OF_TEXT_MEM_TITLE, IDS_OUT_OF_TEXT_MEM_MESSAGE);
    	return;
    }

    if (myAction & KEY_CANT_DELETE)
    {
    	EdWin_ShowStatus (pmTextDisplayWindow, "Can't delete character");
   	MessageBeep (MB_ICONEXCLAMATION);
    	return;
    }
    
    if (myAction & (KEY_LONGEST_LINE_CHANGED | KEY_NUM_LINES_CHANGED))
    {
    	MySynchScrollBarsToText (pmTextDisplayWindow);
    }
    
    if (myAction & KEY_MARK_REMOVED)
    {
        EdWin_MarkMenuCreate (MyGetEditWindow (pmTextDisplayWindow));
    }

    if (myAction & KEY_ERROR_REMOVED)
    {
    	EdWin_ShowStatus (pmTextDisplayWindow, "");
    }

    if (myAction & KEY_FILE_DIRTY)
    {
    	EdWin_SetWindowTitle (pmTextDisplayWindow);
    }
    
    if (myAction & KEY_REDRAW_FROM_SELECTION)
    {
        MyCaretDisplay (pmTextDisplayWindow);
    	MyDisplayText (pmTextDisplayWindow, FALSE, ALL_LINES, ALL_LINES);
    }
    else if (myAction & KEY_UPDATE_LINE)
    {
    	int	myLine, myCol;
    	
        MyCaretDisplay (pmTextDisplayWindow);
        EdText_GetSelectionLocation (myTextPtr, &myLine, &myCol);
    	MyDisplayText (pmTextDisplayWindow, FALSE, myLine, myLine);
    }
    
    MyScrollToSelection (pmTextDisplayWindow, SCROLL_NORMAL, FALSE);
} // MyKeystrokeHandler


/************************************************************************/
/* MyMarkMoveToLocation							*/
/************************************************************************/
static void	MyMarkMoveToLocation (HWND pmTextDisplayWindow, 
				      int pmMarkNumber)
{
    TextPtr	myTextPtr = MyGetTextPtr (pmTextDisplayWindow);
    MarkLinePtr	myMark = myTextPtr -> markLine;
    BOOL	myRedraw = FALSE;
    int		cnt;
    
    EdFail_AddMessage (NORMAL, "Move to Mark - Window: %x - Mark #: %d", 
    		       pmTextDisplayWindow, pmMarkNumber);
    		       
    for (cnt = 0 ; cnt < pmMarkNumber ; cnt++)
    {
    	if (myMark != NULL)
    	    myMark = myMark -> next;
    }
    
    if (myMark == NULL)
    {
    	EdFail_Warn (IDS_CANTFINDMARK, __FILE__, __LINE__, 0, pmMarkNumber);
    	return;
    }
            
    EdFail_AddMessage (NORMAL, "Move to Mark (2) - Line: %d of %d - Name: %s", 
    		       myMark -> line, myTextPtr -> numLines, myMark -> name);
    		       
    if (EdText_SetSelection (myTextPtr, myMark -> line, 0))
    {
        myRedraw = TRUE;
        MyCaretDisplay (pmTextDisplayWindow);
    }
    MyCaretMove (pmTextDisplayWindow);
    MyScrollToSelection (pmTextDisplayWindow, SCROLL_NORMAL, myRedraw);
    EdWin_ShowStatus (pmTextDisplayWindow, "Jump to %s", myMark -> name);
    
    // Reset the Undo buffer
    EdText_UndoSetMoreKeystrokes (myTextPtr, FALSE);
} // MyMarkMoveToLocation


/************************************************************************/
/* MyMatchPathName							*/
/************************************************************************/
static void	MyMatchPathName (char *pmPathName, HWND pmEditWindow, 
		    		 HWND pmTextDisplayWindow, TextPtr pmTextPtr, 
				 void *pmMatch)
{
    const char	*myErrorPathName = ((MatchWindow *) pmMatch) -> pathName;
    WORD	myErrorTuringFileNo = ((MatchWindow *) pmMatch) -> turingFileNo;
    char	*myPseudoPathName;
    
    myPseudoPathName = MyGetTextDisplayWindowInfo (pmTextDisplayWindow) ->
    								pseudoPathName;

    if (myErrorPathName == NULL)
    {
	if (myErrorTuringFileNo == EdDisp_GetTuringFileNo (pmTextDisplayWindow))
	{
    	    *(((MatchWindow *) pmMatch) -> editWindow) = pmEditWindow;
    	    *(((MatchWindow *) pmMatch) -> textDisplayWindow) = 
			    	    			pmTextDisplayWindow;
	}
    	return;
    }
        
    if (((pmPathName != NULL) && 
	 (_stricmp (pmPathName, myErrorPathName) == 0)) ||
        ((myPseudoPathName != NULL) && 
	 (_stricmp (myPseudoPathName, myErrorPathName) == 0)))

    {
    	*(((MatchWindow *) pmMatch) -> editWindow) = pmEditWindow;
    	*(((MatchWindow *) pmMatch) -> textDisplayWindow) = pmTextDisplayWindow;
    }
} // MyMatchPathName

				 
/************************************************************************/
/* MyMenuItemEnable							*/
/************************************************************************/
static void	MyMenuItemEnable (HMENU pmMenu, int pmItemPosition, 
				  BOOL pmEnabled)
{
    if (pmEnabled)
    {
    	EnableMenuItem (pmMenu, pmItemPosition, MF_BYCOMMAND | MF_ENABLED);
    }
    else
    {
    	EnableMenuItem (pmMenu, pmItemPosition, MF_BYCOMMAND | MF_GRAYED);
    }
} // MyMenuItemEnable


/************************************************************************/
/* MyMouseDownHandler							*/
/************************************************************************/
static BOOL	MyMouseDownHandler (HWND pmTextDisplayWindow, int pmX, int pmY,
    				    BOOL pmShift, BOOL pmAllowOutOfBounds)
{
    TextDisplayWindowInfoPtr	myInfo;
    RECT			myRect;
    int				myLeft = 0;
    
    myInfo = MyGetTextDisplayWindowInfo (pmTextDisplayWindow);
    myRect = myInfo -> textRect;
    
    // Adjust the mouse position so that if it is in the margins, we move it
    // into the text rectangle
    if (stDisplayBreakpoints)
    {
	myLeft = BREAKPOINT_MARGIN;
    }
    if ((myLeft <= pmX) && (pmX < myRect.left))
    {
    	pmX = myRect.left;
    }
    if ((0 <= pmY) && (pmY < myRect.top))
    {
    	pmY = myRect.top;
    }
    
    if (MyMouseInTextRect (pmTextDisplayWindow, pmX, pmY) || pmAllowOutOfBounds)
    {
    	int 	myLine;
    	int 	myCol;
    	BOOL	myRedraw = FALSE;

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
	    int	myOrigSelTo = myInfo -> text -> selTo;
	    
	    myStatus = EdText_SelectExtendSelection (myInfo -> text, 
    	        myLine + myInfo -> topLine, myCol + myInfo -> leftCol);

	    if (myOrigSelTo == myInfo -> text -> selTo)
	    {
		return TRUE;
	    }

    	    if (myStatus == HIDE_SELECTION)
    	    {
    	        MyCaretDisplay (pmTextDisplayWindow);
    	        MyCaretMove (pmTextDisplayWindow);
    	    }
    	    else if (myStatus == CREATE_SELECTION)
    	    {
    	    	MyCaretHide (pmTextDisplayWindow);
    	    }
    	    MyScrollToSelection (pmTextDisplayWindow, SCROLL_NORMAL, 
    	        myStatus != NO_REDRAW);
	}
	else
	{
     	    if (EdText_SetSelection (myInfo -> text, 
    	        myLine + myInfo -> topLine, myCol + myInfo -> leftCol))
      	    {
   	        myRedraw = TRUE;
    	        MyCaretDisplay (pmTextDisplayWindow);
      	    }
    	    MyCaretMove (pmTextDisplayWindow);
    	    MyScrollToSelection (pmTextDisplayWindow, SCROLL_NORMAL, myRedraw);
        }
        
	EdWin_ShowCursorPosition (pmTextDisplayWindow);
    
    	// Reset the Undo buffer
	EdText_UndoSetMoreKeystrokes (myInfo -> text, FALSE);

        // Status message
	MyShowStatusForCursorMove (pmTextDisplayWindow);
	
    	return TRUE;
    }
    else if (stDisplayBreakpoints && 
    	     MyMouseInBreakpoint (pmTextDisplayWindow, pmX, pmY))
    {
    	int 	myLine;

	pmY = min (max (pmY, myRect.top), myRect.bottom);
    	myLine = (pmY - myRect.top) / stTextFontCharHeight;
	myLine = min (myLine, myInfo -> windowHeight - 1) + myInfo -> topLine;

	EdText_ToggleBreakpoint (myInfo -> text, myLine);

	MyDisplayText (pmTextDisplayWindow, FALSE, myLine, myLine);
	
    	return FALSE;
    }
    else        	     
    {
    	return FALSE;
    }
} // MyMouseDownHandler


/************************************************************************/
/* MyMouseExtendSelectionToLine						*/
/************************************************************************/
static void	MyMouseExtendSelectionToLine (HWND pmTextDisplayWindow)
{
    int	myStatus;
	    
    myStatus = EdText_SelectLine (MyGetTextPtr (pmTextDisplayWindow));
      	    
    if (myStatus == HIDE_SELECTION)
    {
        MyCaretDisplay (pmTextDisplayWindow);
        MyCaretMove (pmTextDisplayWindow);
    }
    else if (myStatus == CREATE_SELECTION)
    {
    	MyCaretHide (pmTextDisplayWindow);
    }
    MyScrollToSelection (pmTextDisplayWindow, SCROLL_NORMAL, 
        myStatus != NO_REDRAW);
        
    // Adjust the cursor position status
    EdWin_ShowCursorPosition (pmTextDisplayWindow);

    EdWin_ShowStatus (pmTextDisplayWindow, "Line selected");        
} // MyMouseExtendSelectionToLine


/************************************************************************/
/* MyMouseExtendSelectionToWord						*/
/************************************************************************/
static void	MyMouseExtendSelectionToWord (HWND pmTextDisplayWindow)
{
    int	myStatus;
	    
    myStatus = EdText_SelectWord (MyGetTextPtr (pmTextDisplayWindow));
      	    
    if (myStatus == HIDE_SELECTION)
    {
        MyCaretDisplay (pmTextDisplayWindow);
        MyCaretMove (pmTextDisplayWindow);
    }
    else if (myStatus == CREATE_SELECTION)
    {
    	MyCaretHide (pmTextDisplayWindow);
    }
    MyScrollToSelection (pmTextDisplayWindow, SCROLL_NORMAL, 
        myStatus != NO_REDRAW);
        
    // Adjust the cursor position status
    EdWin_ShowCursorPosition (pmTextDisplayWindow);

    EdWin_ShowStatus (pmTextDisplayWindow, "Word selected");        
} // MyMouseExtendSelectionToWord


/************************************************************************/
/* MyMouseInBreakpoint							*/
/************************************************************************/
static BOOL	MyMouseInBreakpoint (HWND pmTextDisplayWindow, int pmX, int pmY)
{
    RECT	myRect;
    
    myRect = MyGetTextDisplayWindowInfo (pmTextDisplayWindow) -> textRect;
    
    return ((pmX < BREAKPOINT_MARGIN) &&
            (myRect.top <= pmY) && (pmY <= myRect.bottom));
} // MyMouseInTextRect


/************************************************************************/
/* MyMouseInTextRect							*/
/************************************************************************/
static BOOL	MyMouseInTextRect (HWND pmTextDisplayWindow, int pmX, int pmY)
{
    RECT	myRect;
    
    myRect = MyGetTextDisplayWindowInfo (pmTextDisplayWindow) -> textRect;
    
    return ((myRect.left <= pmX) && (pmX <= myRect.right) &&
            (myRect.top <= pmY) && (pmY <= myRect.bottom));
} // MyMouseInTextRect


/************************************************************************/
/* MyResizeTextDisplayWindow						*/
/************************************************************************/
static void	MyResizeTextDisplayWindow (HWND pmTextDisplayWindow)
{
    RECT			myWindowRect;
    TextDisplayWindowInfoPtr	myInfo;
    int				myHeight, myMaxLine;
    int				myWidth, myMaxCol;

    // The first resizing comes before there's a client area, so
    // we put this check here to ignore it
    if (GetClientRect (pmTextDisplayWindow, &myWindowRect) == 0)
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
    myInfo = MyGetTextDisplayWindowInfo (pmTextDisplayWindow);
    
    // Set the text rectangle
    myInfo -> textRect = myWindowRect;
    myInfo -> textRect.top += MARGIN;
    myInfo -> textRect.left += MARGIN;
    if (stDisplayBreakpoints)
    {
    	myInfo -> textRect.left += BREAKPOINT_MARGIN;
    }

    // Set the window width and height        
    myInfo -> windowWidth = (myInfo -> textRect.right - 
        myInfo -> textRect.left) / stTextFontCharWidth;
    myInfo -> windowHeight = (myInfo -> textRect.bottom - 
        myInfo -> textRect.top) / stTextFontCharHeight;
    
    // Check to see if we've increased the size of the window
    // and we move the window down.
    myHeight = myInfo -> windowHeight;
    myMaxLine = max (0, myInfo -> text -> numLines - (myHeight - 2));
    myInfo -> topLine = max (0, min (myInfo -> topLine, myMaxLine));
    myWidth = myInfo -> windowWidth;
    myMaxCol = max (0, myInfo -> text -> longestLineLen - (myWidth - 2));
    myInfo -> leftCol = max (0, min (myInfo -> leftCol, myMaxCol));

    if (stDisplayResizeMessage)
    {
	// Display a message indicating the new screen size
	EdWin_ShowStatus (pmTextDisplayWindow, "%d Columns by %d Rows",
	    myInfo -> windowWidth, myInfo -> windowHeight);
    }

    MySynchScrollBarsToText (pmTextDisplayWindow);
    
    MyCaretDisplay (pmTextDisplayWindow);
} // MyResizeTextDisplayWindow


/************************************************************************/
/* MySaveRTF								*/
/************************************************************************/
static void	MySaveRTF (HWND pmTextDisplayWindow)
{
    BOOL			myNewPathName;
    FilePath			myPathName;
    TextDisplayWindowInfoPtr	myInfo;
    char			myPackageName [1024], myClassName [1024];
    int				myBytesSaved, myLinesSaved;
    const char			*mySuffix;
    
    // Initialize the class name to the empty string
    myClassName [0] = 0;
    
    // Indent the program properly
    MyEditIndent (pmTextDisplayWindow);
    
    // Get the window information
    myInfo = MyGetTextDisplayWindowInfo (pmTextDisplayWindow);
    
    myNewPathName = FALSE;
    
    // Show status
    EdWin_ShowStatus (pmTextDisplayWindow, "Saving File in RTF format");

    if (myInfo -> isJava)
    {
    	EdJava_GetPackageAndClass (myInfo -> text -> text, 
	    myInfo -> text -> textLen, myPackageName, myClassName);
    	if (myClassName [0] == 0)
    	{
    	    EdGUI_Message1 (pmTextDisplayWindow, 0, IDS_UNKNOWN_FILE_TYPE_TITLE, 
    	        IDS_RTF_MUST_BE_KNOWN_LANG);
    	    return;
    	}
    }
    else if (myInfo -> isTuring)
    {    
    	EdTur_GetProgramName (myInfo -> text -> text, 
	    myInfo -> text -> textLen, myClassName);
    }
    else
    {
    	EdGUI_Message1 (pmTextDisplayWindow, 0, IDS_UNKNOWN_FILE_TYPE_TITLE, 
    	    IDS_RTF_MUST_BE_KNOWN_LANG);
    	return;
    }
        	
    if (myInfo -> pathName == NULL)
    {
    	if (myClassName [0] != 0)
    	{
    	    strcpy (myPathName, myClassName);
    	    strcat (myPathName, RTF_SUFFIX);
    	}
    	else
    	{
    	    strcpy (myPathName, "Unnamed");
    	    strcat (myPathName, RTF_SUFFIX);
    	}
    }
    else
    {
    	// Use original file name, with RTF ending
    	const char	*myPtr;
    	
    	strcpy (myPathName, myInfo -> pathName);
    	myPtr = EdFile_GetFileSuffix (myPathName);
    	strcpy (&myPathName [myPtr - myPathName], RTF_SUFFIX);
    }
    	
    // Regardless of what ending it did have, save it as .rtf"    	
    if (!EdGUI_GetSaveRTFFile (pmTextDisplayWindow, myPathName))
    {
        EdWin_ShowStatus (pmTextDisplayWindow, "Save in RTF format cancelled");
        return;
    }
        
    // If there is no file suffix, add .rtf
    mySuffix = EdFile_GetFileSuffix (myPathName);
            
    if (strlen (mySuffix) == 0)
    {
       	strcat (myPathName, RTF_SUFFIX);
    }

    if (myClassName [0] != 0)
    {
    	strcat (myClassName, RTF_SUFFIX);
    	if (strcmp (myClassName, EdFile_GetFileName (myPathName)) != 0)
    	{
   	    int	myResult;

	    if (myInfo -> pathName == NULL)
	    {	    	    
    	    	myResult = EdGUI_Message1 (pmTextDisplayWindow, 
   	    	    MB_YESNO | MB_DEFBUTTON2, IDS_ILLEGAL_FILE_NAME_TITLE, 
    	    	    IDS_ILLEGAL_RTF_FILE_NAME1, 
    	    	    EdFile_GetFileName (myPathName), myClassName);
    	    }
    	    else
	    {	    	    
    	    	myResult = EdGUI_Message1 (pmTextDisplayWindow, 
   	    	    MB_YESNO | MB_DEFBUTTON2, IDS_ILLEGAL_FILE_NAME_TITLE, 
    	    	    IDS_ILLEGAL_RTF_FILE_NAME2, 
    	    	    EdFile_GetFileName (myInfo -> pathName), 
    	    	    EdFile_GetFileName (myPathName), myClassName);
    	    }
	    	
    	    if (myResult == IDNO)
    	    {
     	    	EdWin_ShowStatus (pmTextDisplayWindow, 
     	    	    "Save in RTF format cancelled");
   	    	return;
    	    }
        } // if class name != file name
    }
   
    // Save the text to the file.  If it fails, return immediately
    if (!EdFile_SaveRTFToFile (pmTextDisplayWindow, myPathName, 
    			       MyGetTextPtr (pmTextDisplayWindow), 
        		       &myBytesSaved, &myLinesSaved))
    {
	EdWin_ShowStatus (pmTextDisplayWindow, "Save in RTF format failed");
    }

    EdWin_ShowStatus (pmTextDisplayWindow, 
    	"Saved RTF as %s - %d lines - %d bytes", 
        EdFile_GetFileName (myPathName), myLinesSaved, myBytesSaved);
} // MySaveRTF


/************************************************************************/
/* MyScrollHandlerHorz							*/
/************************************************************************/
static void	MyScrollHandlerHorz (HWND pmTextDisplayWindow, int pmPart, 
				     int pmValue)
{
    TextDisplayWindowInfoPtr	myInfo;
    int				myWidth;
    int				myMaxCol;
    int 			myNewLeftCol;
        
    myInfo = MyGetTextDisplayWindowInfo (pmTextDisplayWindow);
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
    SetScrollPos (pmTextDisplayWindow, SB_HORZ, myNewLeftCol, TRUE);
    MyCaretMove (pmTextDisplayWindow);    
    MyDisplayText (pmTextDisplayWindow, FALSE, ALL_LINES, ALL_LINES);
} // MyScrollHandlerHorz


/************************************************************************/
/* MyScrollHandlerVert							*/
/************************************************************************/
static void	MyScrollHandlerVert (HWND pmTextDisplayWindow, int pmPart, 
				     int pmValue)
{
    TextDisplayWindowInfoPtr	myInfo;
    int				myHeight;
    int				myMaxLine;
    int 			myNewTopLine;
        
    myInfo = MyGetTextDisplayWindowInfo (pmTextDisplayWindow);
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
    SetScrollPos (pmTextDisplayWindow, SB_VERT, myNewTopLine, TRUE);
    MyCaretMove (pmTextDisplayWindow);    
    MyDisplayText (pmTextDisplayWindow, FALSE, ALL_LINES, ALL_LINES);
} // MyScrollHandlerVert


/************************************************************************/
/* MyScrollToSelection							*/
/************************************************************************/
static void	MyScrollToSelection (HWND pmTextDisplayWindow, 
				     int pmSkipDistance, 
				     BOOL pmRedraw)
// One DEFAULT, DOWN_PAGE, UP_PAGE, SCROLL_TOP_OF_SCREEN, SCROLL_BOTTOM_OF_SCREEN,
{
    TextDisplayWindowInfoPtr	myInfo;
    int				myLine, myCol;
    int				myFirstVisibleLine, myLastVisibleLine;
    int				myFirstVisibleCol,myLastVisibleCol;
    int				myNewTopLine, myNewLeftCol;
    int				myHeight, myWidth;
    int				myMaxLine, myMaxCol;
    int				myPageDistance;
    
    myInfo = MyGetTextDisplayWindowInfo (pmTextDisplayWindow);
    myNewTopLine = myInfo -> topLine;
    myNewLeftCol = myInfo -> leftCol;
    myHeight = myInfo -> windowHeight;
    myWidth = myInfo -> windowWidth;
    myMaxLine = max (0, myInfo -> text -> numLines - (myHeight - 2));
    myMaxCol = max (0, myInfo -> text -> longestLineLen - (myWidth - 2));
    myPageDistance = myHeight - 2;

    EdText_GetSelectionLocation (myInfo -> text, &myLine, &myCol);
    
    myFirstVisibleLine = myInfo -> topLine;
    myLastVisibleLine = myFirstVisibleLine + myHeight - 1;
    myFirstVisibleCol = myInfo -> leftCol;
    myLastVisibleCol = myFirstVisibleCol + myInfo -> windowWidth - 1;
    
    if ((myFirstVisibleLine <= myLine) && (myLine <= myLastVisibleLine) &&
        (myFirstVisibleCol <= myCol) && (myCol <= myLastVisibleCol) &&
        (myFirstVisibleLine <= myMaxLine) && (myFirstVisibleCol < myMaxCol))
    {
    	// The selection is currently visible
    	if (pmRedraw)
    	{
    	    MyDisplayText (pmTextDisplayWindow, FALSE, ALL_LINES, ALL_LINES);
    	}
    	return;
    }

    // Handle vertical displacement first    
    if ((myFirstVisibleLine <= myLine) && (myLine <= myLastVisibleLine))
    {
    	// Do nothing, and skip all the other checks
    }
    else if (pmSkipDistance == SCROLL_TOP_OF_SCREEN)
    {
    	// Place selection one line from top of screen
    	myNewTopLine = myLine - 1;
    }
    else if (pmSkipDistance == SCROLL_BOTTOM_OF_SCREEN)
    {
    	// Place selection one line from bottom of screen
    	myNewTopLine = myLine - myHeight + 2;
    }
    else if (pmSkipDistance == SCROLL_PAGE_DOWN)
    {
    	// If scrolling down one page would place the selection in the 
    	// window, scroll down one page
    	if ((myLastVisibleLine < myLine) && 
    	    (myLine <= myLastVisibleLine + myPageDistance))
    	{
    	    myNewTopLine = myFirstVisibleLine + myPageDistance;
    	}
    	else
    	{
      	    // Place selection three lines from top of screen
    	    myNewTopLine = myLine - 3;
    	}
    }
    else if (pmSkipDistance == SCROLL_PAGE_UP)
    {
    	// If scrolling up one page would place the selection in the 
    	// window, scroll up on page
    	if ((myFirstVisibleLine - myPageDistance <= myLine) && 
    	    (myLine < myFirstVisibleLine))
    	{
    	    myNewTopLine = myFirstVisibleLine - myPageDistance;
    	}
    	else
    	{
      	    // Place selection three lines from top of screen
    	    myNewTopLine = myLine - 3;
    	}
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
    	MyCaretMove (pmTextDisplayWindow);
	MySynchScrollBarsToText (pmTextDisplayWindow);
    	MyDisplayText (pmTextDisplayWindow, FALSE, ALL_LINES, ALL_LINES);
    }
} // MyScrollToSelection


/************************************************************************/
/* MySelectionDelete							*/
/************************************************************************/
static void	MySelectionDelete (HWND pmTextDisplayWindow)
{
    TextPtr	myTextPtr;
    int		myAction;
    
    myTextPtr = MyGetTextPtr (pmTextDisplayWindow);

    myAction = EdText_SelectionDelete (myTextPtr);
    
    if (myAction & (KEY_LONGEST_LINE_CHANGED | KEY_NUM_LINES_CHANGED))
    {
    	MySynchScrollBarsToText (pmTextDisplayWindow);
    }
    
    if (myAction & KEY_MARK_REMOVED)
    {
        EdWin_MarkMenuCreate (MyGetEditWindow (pmTextDisplayWindow));
    }

    if (myAction & KEY_ERROR_REMOVED)
    {
//    	EdWin_ShowStatus (pmTextDisplayWindow, " ");
    }

    if (myAction & KEY_FILE_DIRTY)
    {
    	EdWin_SetWindowTitle (pmTextDisplayWindow);
    }
    
    if (myAction & KEY_REDRAW_FROM_SELECTION)
    {
        MyCaretDisplay (pmTextDisplayWindow);
    	MyDisplayText (pmTextDisplayWindow, FALSE, ALL_LINES, ALL_LINES);
    }
    else if (myAction & KEY_UPDATE_LINE)
    {
    	int	myLine, myCol;
    	
        MyCaretDisplay (pmTextDisplayWindow);
        EdText_GetSelectionLocation (myTextPtr, &myLine, &myCol);
    	MyDisplayText (pmTextDisplayWindow, FALSE, myLine, myLine);
    }
        
    MyScrollToSelection (pmTextDisplayWindow, SCROLL_NORMAL, FALSE);
} // MySelectionDelete


/************************************************************************/
/* MySetDisplayFonts							*/
/************************************************************************/
static BOOL	MySetDisplayFonts (HWND pmTextDisplayWindow)
{
    HINSTANCE	myApplicationInstance;
    HDC		myDisplayContext;
    TEXTMETRIC	myTextMetrics;
    
    // Get the font name
    myApplicationInstance = (HINSTANCE) GetWindowLong (pmTextDisplayWindow, 
        GWL_HINSTANCE);

    // Delete any previously existing font
    if (stNormalFont != NULL)
    {
    	DeleteObject (stNormalFont);
    }
    
    // Create the new font
    stNormalFont = EdGUI_CreateFont (pmTextDisplayWindow, NULL, 
        stProperties.textFontName, stProperties.textFontSize, 
        NO_BOLD, NO_ITALIC);
    if (stNormalFont == NULL)
    {
    	EdFail_Warn (IDS_CREATEFONTFAIL, __FILE__, __LINE__, GetLastError ());
    	return FALSE;
    }

    // Delete any previously existing font
    if (stBoldFont != NULL)
    {
    	DeleteObject (stBoldFont);
    }
    
    // Create the new font
    stBoldFont =  EdGUI_CreateFont (pmTextDisplayWindow, NULL, 
        stProperties.textFontName, stProperties.textFontSize, BOLD, NO_ITALIC);
    if (stBoldFont == NULL)
    {
    	EdFail_Warn (IDS_CREATEFONTFAIL, __FILE__, __LINE__, GetLastError ());
    	return FALSE;
    }

    myDisplayContext = GetDC (pmTextDisplayWindow);
    SelectFont (myDisplayContext, stNormalFont);
    GetTextMetrics (myDisplayContext, &myTextMetrics);
    stTextFontCharHeight = myTextMetrics.tmHeight;
    stTextFontCharWidth = myTextMetrics.tmAveCharWidth;
    ReleaseDC (pmTextDisplayWindow, myDisplayContext);
    
    return TRUE;
} // MySetDisplayFonts


/************************************************************************/
/* MySetLanguageFromPath			      			*/
/************************************************************************/
static BOOL	MySetLanguageFromPath (TextDisplayWindowInfoPtr pmInfo,
				       const char *pmPath)
{
    BOOL	myWasJava = pmInfo -> isJava;
    BOOL	myWasTuring = pmInfo -> isTuring;
    BOOL	myLangChanged = FALSE;
    const char	*mySuffix = EdFile_GetFileSuffix (pmPath);
    
    if (_stricmp (mySuffix, ".java") == 0)
    {
    	pmInfo -> isReallyJava = TRUE;
    	pmInfo -> isJava = TRUE;
    	pmInfo -> isReallyTuring = FALSE;
    	pmInfo -> isTuring = FALSE;
    }
    else if ((_stricmp (mySuffix, ".t") == 0) ||
	     (_stricmp (mySuffix, ".tur") == 0) ||
	     (_stricmp (mySuffix, ".ti") == 0) ||
	     (_stricmp (mySuffix, ".tu") == 0) ||
	     (_stricmp (mySuffix, ".dem") == 0))
    {
    	pmInfo -> isReallyJava = FALSE;
    	pmInfo -> isJava = FALSE;
    	pmInfo -> isReallyTuring = TRUE;
    	pmInfo -> isTuring = TRUE;
    }
    else if ((mySuffix [0] == 0) || (strcmp (mySuffix, ".") == 0))
    {
    	pmInfo -> isReallyJava = FALSE;
    	pmInfo -> isJava = gProgram.isJava;
    	pmInfo -> isReallyTuring = FALSE;
    	pmInfo -> isTuring = gProgram.isTuring;
    }	     
    else
    {
    	// The file has an unrecognized suffix.  It's not Turing or Java
    	// This prevents accidentally indenting your HTML, etc.
    	pmInfo -> isReallyJava = FALSE;
    	pmInfo -> isJava = FALSE;
    	pmInfo -> isReallyTuring = FALSE;
    	pmInfo -> isTuring = FALSE;
    }

    pmInfo -> text -> isJava = pmInfo -> isJava;
    pmInfo -> text -> isTuring = pmInfo -> isTuring;

    pmInfo -> syntaxColour = pmInfo -> isReallyJava || pmInfo -> isReallyTuring;
    
    if ((pmInfo -> isJava != myWasJava) || (pmInfo -> isTuring != myWasTuring))
    {
    	// The type of file has changed.  Reclassify the contents of the file
    	// for syntax colouring purposes and then eliminate the mark menu.
        myLangChanged = TRUE;
    	EdText_ClassifyText (pmInfo -> text);
    }

    return myLangChanged;
} // MySetLanguageFromPath


/************************************************************************/
/* MyShowStatusForCursorMove			      			*/
/************************************************************************/
static void	MyShowStatusForCursorMove (HWND pmTextDisplayWindow)
{
    TextDisplayWindowInfoPtr	myInfo;
    TextPtr			myTextPtr;
    
    // Get the window information
    myInfo = MyGetTextDisplayWindowInfo (pmTextDisplayWindow);
    myTextPtr = myInfo -> text;
    
    if (myTextPtr -> selFrom == myTextPtr -> selTo)
    {
	// if there's errors on this line, show an error message
	int		myLine, myCol;
	ErrorLinePtr	myErrorLine, myErrorToDisplay;
	int		myDistance = 99999;
	
	EdText_GetSelectionLocation (myTextPtr, &myLine, &myCol);
	
        myErrorLine = myTextPtr -> errorLine;
	while ((myErrorLine != NULL) && 
	       (myErrorLine -> errorPtr -> line < myLine))
	{
	    myErrorLine = myErrorLine -> next;
	}
	if ((myErrorLine == NULL) ||
	    (myErrorLine -> errorPtr -> line != myLine))
	{
            EdWin_ShowStatus (pmTextDisplayWindow, "");
            return;
	}
	
	// Display the closest error message
	while ((myErrorLine != NULL) && 
	       (myErrorLine -> errorPtr -> line == myLine))
	{
	    int	myLocalDistance;
	    
	    myLocalDistance = min (
	        abs (myErrorLine -> errorPtr -> tokenPos - myCol),
	        abs (myCol - (myErrorLine -> errorPtr -> tokenPos +
	                 myErrorLine -> errorPtr -> tokenLen)));
	    if (myLocalDistance < myDistance)
	    {
		myDistance = myLocalDistance;
	        myErrorToDisplay = myErrorLine;
	    }
	    myErrorLine = myErrorLine -> next;
	}	    
	EdWin_ShowStatusBold (pmTextDisplayWindow, 
	    myErrorToDisplay -> errorPtr -> errorMessage);
    }
    else
    {
	int	myFromLine, myFromCol, myToLine, myToCol, myNumLines;
	   
	EdText_GetSelectionLinesCols (myInfo -> text, &myFromLine, 
	    &myFromCol, &myToLine, &myToCol);
	myNumLines = myToLine - myFromLine;
	if ((myNumLines != 0) && (myToCol > 0))
	{
	    myNumLines++;
	}
	if (myNumLines > 0)
	{
	    EdWin_ShowStatus (pmTextDisplayWindow, "%d lines selected",
	    	myNumLines);
	}
	else
	{
	    EdWin_ShowStatus (pmTextDisplayWindow, 
	    	"%d characters selected", myToCol - myFromCol);
	}
    }
} // MyShowStatusForCursorMove


/************************************************************************/
/* MySynchScrollBarsToText			      			*/
/*						      			*/
/* Set the scroll bars to correspond with the current top-most line and	*/
/* left-most column.  Also change the size of the scroll bars to 	*/
/* reflect the number of lines and the longest line.			*/
/************************************************************************/
static void	MySynchScrollBarsToText (HWND pmTextDisplayWindow)
{
    TextDisplayWindowInfoPtr	myInfo;
    TextPtr			myText;
    SCROLLINFO			myScrollInfo;
    
    // Get the window information
    myInfo = MyGetTextDisplayWindowInfo (pmTextDisplayWindow);
    myText = myInfo -> text;
    
    myScrollInfo.cbSize = sizeof (SCROLLINFO);
    myScrollInfo.fMask = SIF_DISABLENOSCROLL | SIF_PAGE | SIF_POS | SIF_RANGE;
    myScrollInfo.nMin = 0;
    myScrollInfo.nMax = myText -> numLines + 1;
    myScrollInfo.nPage = myInfo -> windowHeight;
    myScrollInfo.nPos = myInfo -> topLine;
    
    SetScrollInfo (pmTextDisplayWindow, SB_VERT, &myScrollInfo, TRUE);

    myScrollInfo.cbSize = sizeof (SCROLLINFO);
    myScrollInfo.fMask = SIF_DISABLENOSCROLL | SIF_PAGE | SIF_POS | SIF_RANGE;
    myScrollInfo.nMin = 0;
    myScrollInfo.nMax = myText -> longestLineLen + 1;
    myScrollInfo.nPage = myInfo -> windowWidth;
    myScrollInfo.nPos = myInfo -> leftCol;
    
    SetScrollInfo (pmTextDisplayWindow, SB_HORZ, &myScrollInfo, TRUE);
    
    GetScrollInfo (pmTextDisplayWindow, SB_VERT, &myScrollInfo);
    GetScrollInfo (pmTextDisplayWindow, SB_HORZ, &myScrollInfo);
} // MySynchScrollBarsToText


