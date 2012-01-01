/***********/
/* edwin.c */
/***********/

/*******************/
/* System includes */
/*******************/
#include <windows.h>
#include <commctrl.h>
#include <crtdbg.h>
#include <htmlhelp.h>

/****************/
/* Self include */
/****************/
#include "edwin.h"

/******************/
/* Other includes */
/******************/
#include "ed.h"
#include "edbbar.h"
#include "eddisp.h"
#include "ederr.h"
#include "edfail.h"
#include "edfile.h"
#include "edglob.h"
#include "edprop.h"
#include "edrec.h"
#include "edrun.h"
#include "edstat.h"

#include "dbgalloc.h"

#include "eddialogs.h"
#include "edcommands.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
// Menu item positions
#define NEW_MENU_POSITION	0
#define RECENT_MENU_POSITION	2

#define FIND_SELECTION_POSITION	2

#define INSERT_MENU_POSITION	12

// Minimum size of the window in rows and columns
#define MIN_ROWS		10
#define MIN_COLS		30

// The number of text display windows allowed in single window mode
#define MAX_WINDOWS		30

//
#define RESIZE_IF_NECESSARY	FALSE
#define ALWAYS_RESIZE		TRUE

// The button value for the 1st copy button in the Examples dialog.
// Subsequent buttons have value +1
#define COPY_BUTTON		 10
#define ID_TEXT			200
/********************/
/* Global variables */
/********************/

/*********/
/* Types */
/*********/
typedef struct
{
    BOOL	beginnerMode;
    BOOL	oneWindowMode;

    // The elements in the window
    HWND	buttonBarWindow;
    HWND	tabBarWindow;
    HWND	mdiClientWindow;	 // For use in one window mode
    HWND	sunkenWindow;		 // For use in one window mode
    int		activeTextDisplayWindow; // For use in one window mode
    int		numTextDisplayWindows;	 // For use in one window mode
    HWND	textDisplayWindow [MAX_WINDOWS];
    HWND	statusWindow;
    RECT	oldWindowRect;
} EditWindowInfo, *EditWindowInfoPtr;

typedef struct
{
    const char	*pathName;
    const char	*exampleName;
    int		newSkeleton;
    BOOL	beginnerMode;
    BOOL	oneWindowMode;
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
static EdWinOnly_Properties	stProperties;
static BOOL			stPropertiesChanged;

// 
static HFONT			stTabFont;

// Menu for single window mode with no open windows
static HMENU			stSingleWindowInitMenu;

// The editor window in one window mode
static HWND			stEditorWindow;

/******************************/
/* Static callback procedures */
/******************************/

static BOOL CALLBACK	MyCopyExamplesDialogProcedure (HWND pmDialog, 
						       UINT pmMessage, 
    						       WPARAM pmWParam, 
    						       LPARAM pmLParam);
static LRESULT CALLBACK MyEditorMDIFrameWindowProcedure (HWND pmEditWindow, 
							 UINT pmMessage, 
    							 WPARAM pmWParam, 
    							 LPARAM pmLParam);
static LRESULT CALLBACK MyEditorWindowProcedure (HWND pmWindow, UINT pmMessage, 
						 WPARAM pmWParam, 
						 LPARAM pmLParam);
static LRESULT CALLBACK	MySunkenFrameWindowProcedure (HWND pmWindow, 
						      UINT pmMessage, 
						      WPARAM pmWParam, 
						      LPARAM pmLParam);

/*********************/
/* Static procedures */
/*********************/
static LPWORD			MyAlignOnDoubleWord (LPWORD pmPtr);
static BOOL			MyCommandHandler (HWND pmEditWindow, 
						 short command);
static void			MyCopyExampleDirectory (Example *pmExample,
							HWND pmWindow);
static int			MyCreateEditWindow (HWND pmEditWindow, 
						    const char *pmPathName, 
						    const char *pmExampleName, 
						    int pmNewSkeleton, 
						    BOOL pmBeginnerMode, 
						    BOOL pmOneWindowMode);
static EditWindowInfoPtr	MyCreateEditWindowInfo (void);
static void			MyCreateNewandInsertMenu (HWND pmEditWindow);
static void			MyDispose (HWND pmEditWindow);
static HWND			MyGetEditWindow (HWND pmWindow);
static EditWindowInfoPtr	MyGetEditWindowInfo (HWND pmEditWindow);
static HWND			MyGetButtonBarWindow (HWND pmEditWindow);
static HWND			MyGetStatusWindow (HWND pmEditWindow);				      		
static HWND			MyGetTextDisplayWindow (HWND pmEditWindow);
static void			MyGetWindowDisplacement (BOOL pmReset, 
							 long *pmWindowWidth, 
							 long *pmWindowHeight, 
							 long *pmLeft, 
					 		 long *pmTop);
static void			MyGetWindowSize (HWND pmEditWindow, int pmRows,
						 int pmCols, long *pmWidth,
						 long *pmHeight);
static void			MyMenusAdjust (HWND pmEditWindow, 
					       HMENU pmMenuToAdjust);
static void			MyMenuItemEnable (HMENU pmMenu, 
						  int pmItemPosition, 
						  BOOL pmEnabled);
static void			MyResizeEditWindow (HWND pmEditWindow,
						    BOOL pmAlways);
static BOOL			MySetFindSelectionMenuItem (HWND pmEditWindow);
static BOOL			MySetTabFont (HWND pmTabBarWindow);

/***********************/
/* External procedures */
/***********************/
/************************************************************************/
/* EdWin_Init								*/
/* 									*/
/* Initialize the modules under EdWin and register the new class	*/
/************************************************************************/
BOOL	EdWin_Init (void)
{
    char	myEditWindowClassName [256];
    char	myEditMDIFrameWindowClassName [256];
    char	mySunkenFrameWindowClassName [256];
    int		myResult;
    WNDCLASSEX	myWindowClass, mySunkenFrameClass;

    //
    // Initialize each of the submodules that need initialization 
    // 
    
    if (!EdDisp_Init ())
    {
    	return FALSE;
    }
    
    if (!EdErr_Init ())
    {
    	return FALSE;
    }
    
    if (!EdMenu_Init ())
    {
    	return FALSE;
    }
    
    if (!EdRec_Init ())
    {
    	return FALSE;
    }
    
    if (!EdRun_Init ())
    {
    	return FALSE;
    }
    
    if (!EdStat_Init ())
    {
    	return FALSE;
    }
    
    if (!EdBBar_Init ())
    {
    	return FALSE;
    }

    //
    // Initialize the EdWin module
    //

    // Load the menu
    stSingleWindowInitMenu = LoadMenu (gProgram.applicationInstance,
        MAKEINTRESOURCE (EDIT_INIT_SINGLE_WINDOW_MENUS));
        
    // Get the class name
    EdGUI_LoadString (IDS_EDIT_WINDOW_NAME, myEditWindowClassName, 
	sizeof (myEditWindowClassName));
		     
    //
    // Register the editor window class
    //
    myWindowClass.cbSize = 	  sizeof (myWindowClass);
    // Set window class to redraw when window size changes
    myWindowClass.style = 	  CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    // Procedure to be called with messages for this window class
    myWindowClass.lpfnWndProc =   MyEditorWindowProcedure;
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
    myWindowClass.lpszMenuName =  MAKEINTRESOURCE (EDIT_WINDOW_MENUS);
    // Name of the window class
    myWindowClass.lpszClassName = myEditWindowClassName; 
    // Set the icon for this class.
    myWindowClass.hIconSm = 	  LoadIcon (gProgram.applicationInstance, 
    					    MAKEINTRESOURCE (APP_SMALL_ICON));
    
    myResult = RegisterClassEx (&myWindowClass);
    if (myResult == 0)
    {
    	EdFail_Warn (IDS_REGISTERCLASSFAIL, __FILE__, __LINE__, 
    			  GetLastError ());
    	return FALSE;
    }
    
    // Get the class name
    EdGUI_LoadString (IDS_EDIT_MDI_FRAME_WINDOW_NAME, 
        myEditMDIFrameWindowClassName, sizeof (myEditMDIFrameWindowClassName));
		     
    //
    // Register the editor window MDI frame class
    //
    myWindowClass.cbSize = 	  sizeof (myWindowClass);
    // Set window class to redraw when window size changes
    myWindowClass.style = 	  CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    // Procedure to be called with messages for this window class
    myWindowClass.lpfnWndProc =   MyEditorMDIFrameWindowProcedure;
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
    myWindowClass.lpszClassName = myEditMDIFrameWindowClassName; 
    // Set the icon for this class.
    myWindowClass.hIconSm = 	  LoadIcon (gProgram.applicationInstance, 
    					    MAKEINTRESOURCE (APP_SMALL_ICON));
    
    myResult = RegisterClassEx (&myWindowClass);
    if (myResult == 0)
    {
    	EdFail_Warn (IDS_REGISTERCLASSFAIL, __FILE__, __LINE__, 
    			  GetLastError ());
    	return FALSE;
    }
    
    // Get the class name
    EdGUI_LoadString (IDS_SUNKEN_FRAME_WINDOW_NAME, 
        mySunkenFrameWindowClassName, sizeof (mySunkenFrameWindowClassName));
		     
    /************************************/
    /* Register the raised window class */
    /************************************/
    mySunkenFrameClass.cbSize =        sizeof (mySunkenFrameClass);
    // Set window class to redraw when window size changes
    mySunkenFrameClass.style =	       CS_HREDRAW | CS_VREDRAW;
    // Procedure to be called with messages for this window class
    mySunkenFrameClass.lpfnWndProc =   MySunkenFrameWindowProcedure;
    // The extra space in class struct
    mySunkenFrameClass.cbClsExtra =    0;
    // The extra space in window struct
    mySunkenFrameClass.cbWndExtra =    0;
    // The application's handle
    mySunkenFrameClass.hInstance =     gProgram.applicationInstance;
    // Set the icon for this window class
    mySunkenFrameClass.hIcon =	       NULL;
    // Set the cursor for this window class
    mySunkenFrameClass.hCursor =       LoadCursor (NULL, IDC_ARROW);
    // Set the background colour for this window
    mySunkenFrameClass.hbrBackground = (HBRUSH) GetStockObject (LTGRAY_BRUSH);
    // Set the menu for this window class
    mySunkenFrameClass.lpszMenuName =  NULL;
    // Name of the window class
    mySunkenFrameClass.lpszClassName = mySunkenFrameWindowClassName; 
    // Set the icon for this class.
    mySunkenFrameClass.hIconSm =       NULL;
    
    myResult = RegisterClassEx (&mySunkenFrameClass);
    if (myResult == 0)
    {
    	EdFail_Warn (IDS_REGISTERCLASSFAIL, __FILE__, __LINE__, 
    			  GetLastError ());
    	return FALSE;
    }
    
    return TRUE;
} // EdWin_Init


/************************************************************************/
/* EdWin_Finalize							*/
/************************************************************************/
void	EdWin_Finalize (void)
{
    EdDisp_Finalize ();
    EdErr_Finalize ();
    EdMenu_Finalize ();
    EdRec_Finalize ();
    EdRun_Finalize ();
    EdStat_Finalize ();
    EdBBar_Finalize ();
} // EdWin_Finalize


/************************************************************************/
/* EdWin_PropertiesSet							*/
/************************************************************************/
void	EdWin_PropertiesSet (EdWin_Properties pmProperties)
{
    EdMenu_PropertiesSet (pmProperties.menuProperties);
    EdRec_PropertiesSet (pmProperties.recProperties);
    EdDisp_PropertiesSet (pmProperties.dispProperties);
    EdStat_PropertiesSet (pmProperties.statProperties);
    EdBBar_PropertiesSet (pmProperties.topbProperties);
    
    stProperties = pmProperties.myProperties;
    stPropertiesChanged = TRUE;
} // EdWin_PropertiesSet

			  
/************************************************************************/
/* EdWin_PropertiesImplementChanges					*/
/************************************************************************/
void	EdWin_PropertiesImplementChanges (HWND pmEditWindow)
{
    EditWindowInfoPtr	myInfo;
    int			cnt;
        
    myInfo = MyGetEditWindowInfo (pmEditWindow);
    
    if (stPropertiesChanged)
    {
    	// Make changes for the module
        MySetTabFont (myInfo -> tabBarWindow);
        
    	EdMenu_PropertiesImplementChanges ();
    	EdRec_PropertiesImplementChanges ();
    	stPropertiesChanged = FALSE;
    }
    
    SendMessage (myInfo -> tabBarWindow, WM_SETFONT, (WPARAM) stTabFont, 0);
    EdBBar_PropertiesImplementChanges (myInfo -> buttonBarWindow);
    for (cnt = 0 ; cnt < myInfo -> numTextDisplayWindows ; cnt++)
    {
    	EdDisp_PropertiesImplementChanges (myInfo -> textDisplayWindow [cnt]);
    }
    EdStat_PropertiesImplementChanges (myInfo -> statusWindow);

    // ... 
} // EdWin_PropertiesImplementChanges


/************************************************************************/
/* EdWin_Create								*/
/************************************************************************/
HWND	EdWin_Create (const char *pmPathName, const char *pmExampleName,
		      int pmNewSkeleton, BOOL pmBeginnerMode, 
		      BOOL pmOneWindowMode) 
{
	return EdWin_CreateShow(pmPathName,pmExampleName, pmNewSkeleton, pmBeginnerMode, pmOneWindowMode,TRUE);
}
// this allows command line running with no visible editor while keeping existing infrastructure. Yes it's ugly.
HWND	EdWin_CreateShow (const char *pmPathName, const char *pmExampleName,
		      int pmNewSkeleton, BOOL pmBeginnerMode, 
		      BOOL pmOneWindowMode,BOOL pmShowWindow)
{
    char		myEditWindowClassName [256];
    HWND		myEditWindow, myTextDisplayWindow;
    WindowCreationInfo	myWindowCreationInfo;

    // Debug messages
    if (pmPathName == NULL)
    {
    	EdFail_AddMessage (NORMAL, "Create File (W1) - Skel: %d", 
    			   pmNewSkeleton);
    }
    else
    {
    	EdFail_AddMessage (NORMAL, "Create File (W1) - Name: %s - Skel: %d", 
    		           pmPathName, pmNewSkeleton);
    }    		          
    		       
    // If the window is already open, then bring it to the front rather
    // than opening it a second time.    
    if ((pmPathName != NULL) && 
        EdDisp_GetWindowWithMatchingPathName (
    			pmPathName, &myEditWindow, &myTextDisplayWindow))
    {
    	if (IsIconic (myEditWindow))
    	{
    	    OpenIcon (myEditWindow);
    	}
	BringWindowToTop (myEditWindow);
	EdWin_ActivateTextDisplay (myEditWindow, myTextDisplayWindow);
	return NULL;
    }

    // Load the class name (we've already loaded it once, so no error checking)    
    if (pmOneWindowMode)
    {
    	EdGUI_LoadString (IDS_EDIT_MDI_FRAME_WINDOW_NAME,
	    myEditWindowClassName, sizeof (myEditWindowClassName));
    }
    else   	
    {
    	EdGUI_LoadString (IDS_EDIT_WINDOW_NAME,
	    myEditWindowClassName, sizeof (myEditWindowClassName));
    }

    myWindowCreationInfo.pathName = pmPathName;
    myWindowCreationInfo.exampleName = pmExampleName;
    myWindowCreationInfo.newSkeleton = pmNewSkeleton;
    myWindowCreationInfo.beginnerMode = pmBeginnerMode;
    myWindowCreationInfo.oneWindowMode = pmOneWindowMode;
    
    // Create the editor window    
    myEditWindow = CreateWindow (
    	myEditWindowClassName, 			// Window class
        "New Editor", 				// Window title
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
    	EdLog_Log ("Editor Window = %x  (Finished Creation)", myEditWindow);
    }

    // If we failed to create the window, return immediately
    if (myEditWindow == NULL)
    {
    	return NULL;
    }
    
    // We must show the window here because if we maximize the window 
    // in the creation routine, Windows makes the normal rect the same
    // as the maximized rect!  Yuck!
	if(pmShowWindow) {
		if ((pmBeginnerMode && stProperties.beginnerFullScreen) || 	
			((!pmBeginnerMode) && stProperties.advancedFullScreen))
		{
    		ShowWindow (myEditWindow, SW_SHOWMAXIMIZED);
		}        
		else
		{	    	
			ShowWindow (myEditWindow, SW_SHOWNORMAL);
		}
	}
    if (pmBeginnerMode || pmOneWindowMode)
    {
    	stEditorWindow = myEditWindow;
    }
        
    return myEditWindow;
} // EdWin_Create

    			      
/************************************************************************/
/* EdWin_ActivateTextDisplay						*/
/************************************************************************/
void	EdWin_ActivateTextDisplay (HWND pmEditWindow, HWND pmTextDisplayWindow)
{
    // TW Do something if in one window mode, otherwise do nothing
} // EdWin_ActivateTextDisplay


/************************************************************************/
/* EdWin_AddFile							*/
/************************************************************************/
void	EdWin_AddFile (HWND pmEditWindow, const char *pmPathName,
		       const char *pmExampleName, int pmNewSkeleton)
{
    EditWindowInfoPtr	myInfo;
    RECT		myRect;
    int			myActiveWindow, myOldActiveWindow;    
    TC_ITEM		myTabItem;
    FilePath		myPathForTab;
    
    // Debug messages
    EdFail_AddMessage (NORMAL, 
    		       "Add File (W1) - Window: %x - Name: %s - Skel: %d", 
    		       pmEditWindow, pmPathName, pmNewSkeleton);
    		       
    myInfo = MyGetEditWindowInfo (pmEditWindow);

    if (!myInfo -> oneWindowMode)
    {
    	// TW Assert failed
    }
	
    if (myInfo -> numTextDisplayWindows == MAX_WINDOWS)
    {
//    	TW EdGUI_Message1 (
    	return;
    }
    
    //
    // Add the elements to the window
    //
    
    myOldActiveWindow = myInfo -> activeTextDisplayWindow;
    myInfo -> activeTextDisplayWindow = myInfo -> numTextDisplayWindows;
    myActiveWindow = myInfo -> activeTextDisplayWindow;
    
    myInfo -> numTextDisplayWindows++;
    myInfo -> textDisplayWindow [myActiveWindow] = EdDisp_Create (pmEditWindow,
    	myInfo -> beginnerMode, myInfo -> oneWindowMode);
        
    if (myInfo -> textDisplayWindow [myActiveWindow] == NULL)
    {
    	return;
    }
    
    // Add the tab
    myTabItem.mask = TCIF_TEXT;
    myTabItem.pszText = EdDisp_GetPathForDisplay (
    	MyGetTextDisplayWindow (pmEditWindow), USE_FILENAME, SHOW_DIRTY_MARK,
    	myPathForTab);

    if (TabCtrl_InsertItem (myInfo -> tabBarWindow, myActiveWindow, 
        		    &myTabItem) == -1)
    { 
        // TW Error message
        return;
    }

    // The Z order seems to get messed up here, so we'll put them in
    // order now.
    // Inserting the tab seems to move it to the tpo of the Z order!
    // Let's move the sunken window to the top and then the text display window
    
    SetWindowPos (myInfo -> sunkenWindow, HWND_TOP, 0, 0, 0, 0, 
        SWP_NOMOVE | SWP_NOSIZE);
        
    // Size the new window
    GetWindowRect (myInfo -> textDisplayWindow [myOldActiveWindow], &myRect);
    
    SetWindowPos (myInfo -> textDisplayWindow [myActiveWindow], 
        HWND_TOP, myRect.left, myRect.top,
        myRect.right - myRect.left, myRect.bottom - myRect.top, 0);
        
    // Load the file into text display window    	    
    EdDisp_Create2 (myInfo -> textDisplayWindow [myActiveWindow], pmPathName,
		    pmExampleName, pmNewSkeleton);
    
    ShowWindow (myInfo -> textDisplayWindow [myOldActiveWindow], SW_HIDE);
    ShowWindow (myInfo -> textDisplayWindow [myActiveWindow], SW_SHOWNORMAL);
} // EdWin_AddFile


/************************************************************************/
/* EdWin_CopyExamples							*/
/*									*/
/* Handle the "Copy Examples" command.  This displays a dialog box	*/
/* and performs copying as requested by the user.			*/
/************************************************************************/
void	EdWin_CopyExamples (HWND pmWindow)
{
    Examples		*myExamples;
    HGLOBAL		myGlobalMemory;
    LPDLGTEMPLATE	myDialog;
    LPDLGITEMTEMPLATE	myItem;
    LPWORD		myWord;
    LRESULT		myResult;
    HINSTANCE		myApplicationInstance;
    int			myMsgNumber, cnt;
    char		myMessage [256];
    Example		*myExample;

    // Give appropriate messages such as
    //     - Using Ready directory as home directory - Examples 
    //             found in the Examples directory
    //     - Could not find Examples.txt
    //     - Examples.txt was malformed
    //     - Examples.txt was empty
    
    if (!gProperties.useMyDocuments)
    {
    	FilePath	myApplicationDirectory;
    	char		*myStartupDirectory = gProperties.startupDirectory;
	// gProperties.startupDirectory does not have a trailing \ 
	// (unless x:\), while myApplicationDirectory always does.
	// Remove the trailing slash unless gProperties.startupDirectory
    	EdFile_GetApplicationDirectory (myApplicationDirectory);
	if ((myStartupDirectory [strlen (myStartupDirectory) - 1] != '\\') &&
	    (myApplicationDirectory [strlen (myApplicationDirectory) - 1] == '\\'))
	{
	    myApplicationDirectory [strlen (myApplicationDirectory) - 1] = 0;
	}
    	if (_stricmp (gProperties.startupDirectory, myApplicationDirectory) == 0)
    	{
    	    EdGUI_Message1 (pmWindow, 0, IDS_CANT_COPY_EXAMPLES_TITLE,
    	    		    IDS_HOME_IS_APPLICATION_DIR);
    	    return;
	}
    }
    
    myExamples = Ed_GetExampleDirectories (pmWindow);
    
    if (myExamples == EXAMPLES_NOT_FOUND)
    {
    	EdGUI_Message1 (pmWindow, 0, IDS_CANT_COPY_EXAMPLES_TITLE,
    	    		IDS_EXAMPLES_NOT_FOUND);
    	return;
    }
    else if (myExamples == EXAMPLES_BAD_FORMAT)
    {
    	EdGUI_Message1 (pmWindow, 0, IDS_CANT_COPY_EXAMPLES_TITLE,
    	    		IDS_EXAMPLES_BAD_FORMAT);
    	return;
    }
    else if (myExamples == EXAMPLES_EMPTY)
    {
    	EdGUI_Message1 (pmWindow, 0, IDS_CANT_COPY_EXAMPLES_TITLE,
    	    		IDS_EXAMPLES_EMPTY);
    	return;
    }
    else if (myExamples == EXAMPLES_OUT_OF_MEMORY)
    {
    	EdGUI_Message1 (pmWindow, 0, IDS_CANT_COPY_EXAMPLES_TITLE,
    	    		IDS_EXAMPLES_OUT_OF_MEMORY);
    	return;
    }
    
    //
    // Create the dialog template
    //
    myGlobalMemory = GlobalAlloc (GMEM_ZEROINIT, 8192);
    if (myGlobalMemory == NULL)
    {
    	// Out of memory
    	return;
    }
    myDialog = (LPDLGTEMPLATE) GlobalLock (myGlobalMemory);
    
    // Define the dialog
    myDialog -> style = WS_BORDER | WS_CAPTION | DS_MODALFRAME | 
    	WS_POPUP | WS_SYSMENU | DS_3DLOOK | DS_CONTEXTHELP | DS_SETFONT; 
    myDialog -> cdit = 3 + 2 * myExamples -> numExamples;
    myDialog -> x = 30;
    myDialog -> y = 73;
    myDialog -> cx = 300;
    myDialog -> cy = 59 + 18 * myExamples -> numExamples;
    
    myWord = (LPWORD) (myDialog + 1);
    *myWord++ = 0;				    // No menu
    *myWord++ = 0; // Predefined dialog box class (by default)
    
    if (gProperties.useMyDocuments)
    {
    	myWord += MultiByteToWideChar (CP_ACP, 0, 
    					   "Copy Examples to My Documents",
    					   -1, (LPWSTR) myWord, 100);
    }
    else
    {
    	myWord += MultiByteToWideChar (CP_ACP, 0, 
    					   "Copy Examples to Home Directory",
    					   -1, (LPWSTR) myWord, 100);
    }    					      
    *myWord++ = 8;				// Specify the font size
    myWord += MultiByteToWideChar (		// Specify the font name
		    CP_ACP, 0, "MS Sans Serif", -1, (LPWSTR) myWord, 100);

    // Define the first line of static text (16,8) (200x8)
    myWord = MyAlignOnDoubleWord (myWord);
    myItem = (LPDLGITEMTEMPLATE) myWord;
    myItem -> x = 16; 
    myItem -> y = 8;
    myItem -> cx = 200; 
    myItem -> cy = 8;
    myItem -> id = ID_TEXT;
    myItem -> style = WS_CHILD | WS_VISIBLE | SS_LEFT;
    myWord = (LPWORD) (myItem + 1);
    *myWord++ = 0xFFFF;				// Specify static text class
    *myWord++ = 0x0082;
    EdGUI_LoadString (IDS_EXAMPLE_DLG_1, myMessage, sizeof (myMessage));
    myWord += 1 + MultiByteToWideChar (		// Copy myMessage to template
		    CP_ACP, 0, myMessage, -1, (LPWSTR) myWord, 200);
    *myWord++ = 0;				// Specify no creation data
        
    // Define the second line of static text (16,16) (200x8)
    myWord = MyAlignOnDoubleWord (myWord);
    myItem = (LPDLGITEMTEMPLATE) myWord;
    myItem -> x = 16; 
    myItem -> y = 16;
    myItem -> cx = 200; 
    myItem -> cy = 8;
    myItem -> id = ID_TEXT;
    myItem -> style = WS_CHILD | WS_VISIBLE | SS_LEFT;
    myWord = (LPWORD) (myItem + 1);
    *myWord++ = 0xFFFF;				// Specify static text class
    *myWord++ = 0x0082;
    if (gProperties.useMyDocuments)
    {
	if (gProgram.isJava) myMsgNumber = IDS_EXAMPLE_DLG_2A_JAVA;
	else myMsgNumber = IDS_EXAMPLE_DLG_2A_TURING;
    }
    else
    {
	if (gProgram.isJava) myMsgNumber = IDS_EXAMPLE_DLG_2B_JAVA;
	else myMsgNumber = IDS_EXAMPLE_DLG_2B_TURING;
    }
    EdGUI_LoadString (myMsgNumber, myMessage, sizeof (myMessage));
    myWord += 1 + MultiByteToWideChar (		// Copy myMessage to template
		    CP_ACP, 0, myMessage, -1, (LPWSTR) myWord, 200);
    *myWord++ = 0;				// Specify no creation data
    
    // For each item
    myExample = myExamples -> head;
    for (cnt = 0 ; cnt < myExamples -> numExamples ; cnt++)
    {

        // For each item, define the Copy button (8, 33 + 18 * loc) (38x14)
	myWord = MyAlignOnDoubleWord (myWord);
	myItem = (LPDLGITEMTEMPLATE) myWord;
	myItem -> x = 8; 
	myItem -> y = 33 + 18 * cnt;
	myItem -> cx = 38; 
	myItem -> cy = 14;
	myItem -> id = COPY_BUTTON + cnt;
	myItem -> style = WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_GROUP;
	myWord = (LPWORD) (myItem + 1);
	*myWord++ = 0xFFFF;			// Specify button class
	*myWord++ = 0x0080;  
        myWord += 1 + MultiByteToWideChar (	// Copy "Copy" to template
		    CP_ACP, 0, "Copy", -1, (LPWSTR) myWord, 20);
	*myWord++ = 0;			        // Specify no creation data
        
        // For each item, define the text (52, 36 + 18 * loc) (240x8)
	myWord = MyAlignOnDoubleWord (myWord);
	myItem = (LPDLGITEMTEMPLATE) myWord;
	myItem -> x = 52; 
	myItem -> y = 36 + 18 * cnt;
	myItem -> cx = 240; 
	myItem -> cy = 8;
	myItem -> id = ID_TEXT;
	myItem -> style = WS_CHILD | WS_VISIBLE | SS_LEFT;
	myWord = (LPWORD) (myItem + 1);
	*myWord++ = 0xFFFF;			// Specify button class
	*myWord++ = 0x0082;
        myWord += MultiByteToWideChar (	// Copy label to template
		    CP_ACP, 0, myExample -> label, -1, (LPWSTR) myWord, 200);
	*myWord++ = 0;			        //  Specify no creation data

	// Go to next example
	myExample = myExample -> next;
    }

    // Define the Finish button (243, 74) (50x14)
    myWord = MyAlignOnDoubleWord (myWord);
    myItem = (LPDLGITEMTEMPLATE) myWord;
    myItem -> x = 243; 
    myItem -> y = 38 + 18 * myExamples -> numExamples;
    myItem -> cx = 50; 
    myItem -> cy = 14;
    myItem -> id = IDOK;
    myItem -> style = WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | WS_GROUP;
    myWord = (LPWORD) (myItem + 1);
    *myWord++ = 0xFFFF;				// Specify button class
    *myWord++ = 0x0080;
    myWord += 1 + MultiByteToWideChar (		// Copy "Finish" to template
		    CP_ACP, 0, "Finish", -1, (LPWSTR) myWord, 20);
    *myWord++ = 0;				// Specify no creation data

    GlobalUnlock (myGlobalMemory);
    
    // Display the dialog.
    myApplicationInstance = (HINSTANCE) GetWindowLong (pmWindow, GWL_HINSTANCE);
        
    Ed_EnableAllWindows (FALSE, pmWindow);
    myResult = DialogBoxIndirectParam (myApplicationInstance, 
		   (LPDLGTEMPLATE) myGlobalMemory, MyGetEditWindow (pmWindow),
		   MyCopyExamplesDialogProcedure, (LPARAM) myExamples);
    Ed_EnableAllWindows (TRUE, pmWindow);
} // EdWin_CopyExamples


/************************************************************************/
/* EdWin_CreateRecentMenu						*/
/************************************************************************/
void	EdWin_CreateRecentMenu (HWND pmEditWindow)
{
    HMENU	myFileMenu, myRecentMenu;

    myFileMenu = EdMenu_GetMenu (pmEditWindow, FILE_MENU);
    myRecentMenu = GetSubMenu (myFileMenu, RECENT_MENU_POSITION);

    EdRec_CreateRecentMenu (myRecentMenu);

    if (GetMenuItemCount (myRecentMenu) == 0)
    {
    	EnableMenuItem (myFileMenu, RECENT_MENU_POSITION, 
    	    MF_BYPOSITION | MF_GRAYED);
    }
    else
    {
    	EnableMenuItem (myFileMenu, RECENT_MENU_POSITION, 
    	    MF_BYPOSITION | MF_ENABLED);
    }
} // EdWin_CreateRecentMenu


/************************************************************************/
/* EdWin_EnumerateFiles							*/
/************************************************************************/
void	EdWin_EnumerateFiles (HWND pmEditWindow, 
			      void (*enumProc) (char *, HWND, HWND, void *, void *),
			      void *pmUserParam)
{
    EditWindowInfoPtr	myInfo;
    
    myInfo = MyGetEditWindowInfo (pmEditWindow);
    if (myInfo -> oneWindowMode)
    {
    	int	cnt;
    	
    	for (cnt = 0 ; cnt < myInfo -> numTextDisplayWindows ; cnt++)
    	{
    	    EdDisp_EnumerateFile (pmEditWindow, 
    	        myInfo -> textDisplayWindow [cnt], enumProc, pmUserParam);
    	}
    }
    else
    {
    	EdDisp_EnumerateFile (pmEditWindow, 
    	    MyGetTextDisplayWindow (pmEditWindow), enumProc, pmUserParam);
    }
} // EdWin_EnumerateFiles

				      
/************************************************************************/
/* EdWin_GetActiveDisplayWindow						*/
/************************************************************************/
HWND	EdWin_GetActiveDisplayWindow (HWND pmEditWindow)
{
    return MyGetTextDisplayWindow (pmEditWindow);
} // EdWin_GetActiveDisplayWindow


/************************************************************************/
/* EdWin_MarkMenuCreate							*/
/************************************************************************/
void	EdWin_MarkMenuCreate (HWND pmEditWindow)
{
    HMENU	myMarkMenu;
    
    myMarkMenu = EdMenu_GetMenu (pmEditWindow, MARK_MENU);
    EdDisp_CreateMarkMenu (MyGetTextDisplayWindow (pmEditWindow), myMarkMenu);
} // EdWin_MarkMenuCreate


/************************************************************************/
/* EdWin_MatchesPathName						*/
/************************************************************************/
BOOL	EdWin_MatchesPathName (HWND pmEditWindow, const char *pmPathName)
{
    return EdDisp_MatchesPathName (MyGetTextDisplayWindow (pmEditWindow),
    				   pmPathName);     
} // EdWin_MatchesPathName


/************************************************************************/
/* EdWin_OpenExample							*/
/*									*/
/* This routine opens example programs. It is used in conjunction with	*/
/* WM_TCARD messages that are issued by the HTMLHelp sequence when the	*/
/* user presses the appropriate buttons.				*/
/*									*/
/* pmLParam should have form "[filename];[title of example window]	*/
/************************************************************************/
void	EdWin_OpenExample (HWND pmEditWindow, int pmLineNum, 
			   const char *pmLParam)
{
    FilePath	myLParam, myExamplePath;
    char    	*myPtr;
    
    EdFail_AddMessage (NORMAL, "  Open Example - [%d,%s]", pmLineNum, pmLParam);
    
    strcpy (myLParam, pmLParam);
    myPtr = myLParam;
    
    while ((*myPtr != ';') && (*myPtr != 0))
    {
	myPtr++;
    }
    
    // If pmLParam is malformed (i.e. no ;), then use a blank title
    if (*myPtr != 0)
    {
    	*myPtr++ = 0;
    }

    EdFile_ConstructPath (myExamplePath, IDS_HELP_EXAMPLES_DIR, NO_FILE_NAME);
    strcat (myExamplePath, myLParam);

    EdWin_OpenFile (pmEditWindow, myExamplePath, myPtr, PURPOSE_LOAD_FILE);
} // EdWin_OpenExample


/************************************************************************/
/* EdWin_OpenFile							*/
/*									*/
/* Used to open a file in the editor.  Called either when the user	*/
/* clicks on a file in Explorer or when the system wants to load a file	*/
/* to display the next error.						*/
/************************************************************************/
BOOL	EdWin_OpenFile (HWND pmEditWindow, const char *pmPathName,
			const char *pmExampleName, int pmPurpose)
{
    BOOL	myInBeginnerMode, myInOneWindowMode;
    FilePath	myPathName, myJavaPath;
    const char	*myFileToOpen;
    
    // Remove any trailing period
    strcpy (myPathName, pmPathName);
    if (myPathName [strlen (myPathName) - 1] == '.')
    {
    	myPathName [strlen (myPathName) - 1] = 0;
    }
    
    myFileToOpen = myPathName;
    
    if (EdGlob_EndsWith (myPathName, ".class"))
    {
    	strcpy (myJavaPath, myPathName);
    	myJavaPath [EdFile_GetFileSuffix (myJavaPath) - myJavaPath] = 0;
    	strcat (myJavaPath, ".java");
    	if (EdFile_FileExists (myJavaPath))
    	{
    	    int	myResult;
    	    
	    myResult = EdGUI_Message1 (pmEditWindow, MB_YESNO, 
	    		    IDS_CANT_OPEN_CLASS_TITLE,
		            IDS_CANT_OPEN_CLASS_USE_JAVA, 
		            EdFile_GetFileName (myPathName),
		            EdFile_GetFileName (myJavaPath));
	    if (myResult == IDYES)
	    {		            
	        myFileToOpen = myJavaPath;		            
	    }
	    else
	    {
	    	return FALSE;
	    }
	}
	else
	{
	    EdGUI_Message1 (pmEditWindow, 0, IDS_CANT_OPEN_CLASS_TITLE,
		            IDS_CANT_OPEN_CLASS, 
		            EdFile_GetFileName (myPathName));
	    return FALSE;
	}
    }

    if (pmEditWindow == NULL)
    {
    	pmEditWindow = Ed_GetLastActiveEditWindow ();
    }
    
    if (pmEditWindow != NULL)
    {
    	myInBeginnerMode = MyGetEditWindowInfo (pmEditWindow) -> beginnerMode;
    	myInOneWindowMode = MyGetEditWindowInfo (pmEditWindow) -> oneWindowMode;
    }
    else
    {
    	myInBeginnerMode = stProperties.beginnerMode;
    	myInOneWindowMode = stProperties.oneWindowMode;
    }
    
    if (myInBeginnerMode & (pmExampleName == NULL))
    {
    	HWND	myTextDisplayWindow = MyGetTextDisplayWindow (stEditorWindow);

	return EdDisp_OpenOver (myTextDisplayWindow, myPathName, pmPurpose);
    }
    else if (myInOneWindowMode)
    {
    	if (pmEditWindow == NULL)
    	{
    	    // If in one window mode, do something
    	    EdWin_AddFile (stEditorWindow, myFileToOpen, NULL, 0);
    	}
    	else
    	{
    	    // If in one window mode, do something
    	    EdWin_AddFile (pmEditWindow, myFileToOpen, NULL, 0);
	}
	    	
    	return FALSE;
    }
    else
    {
	return (EdWin_Create (myFileToOpen, pmExampleName, 0, FALSE, FALSE) 
		!= NULL);
    }
} // EdWin_OpenFile

	    	
/************************************************************************/
/* EdWin_ShowCursorPosition						*/
/************************************************************************/
void	EdWin_ShowCursorPosition (HWND pmWindow)
{
    HWND	myEditWindow, myStatusWindow;

    // If window has been closed when we get here, ignore the call.
    if (!IsWindow (pmWindow))
    {
	return;
    }

    myEditWindow = MyGetEditWindow (pmWindow);
    myStatusWindow = MyGetStatusWindow (myEditWindow);
    
    if (myStatusWindow != NULL)
    {
    	int	myLine, myCol, myNumLines;
    	
    	EdDisp_GetSelectionLocation (MyGetTextDisplayWindow (myEditWindow),
    	    &myLine, &myCol, &myNumLines);
    	    
	EdStat_SetPositionInfo (myStatusWindow, myLine, myCol, myNumLines);
    }
} // EdWin_ShowCursorPosition


/************************************************************************/
/* EdWin_ShowStatus							*/
/************************************************************************/
void	EdWin_ShowStatus (HWND pmWindow, const char *pmMessage, ...)
{
    HWND	myEditWindow = MyGetEditWindow (pmWindow);
    HWND	myStatusWindow = MyGetStatusWindow (myEditWindow);
    
    if (myStatusWindow != NULL)
    {
    	va_list 	myArgList;
    
    	va_start (myArgList, pmMessage);
        EdStat_Set (myStatusWindow, pmMessage, myArgList);
    }
} // EdWin_ShowStatus


/************************************************************************/
/* EdWin_ShowStatus1							*/
/************************************************************************/
void	EdWin_ShowStatus1 (HWND pmWindow, int pmMessageRsrc, ...)
{
    HWND	myEditWindow = MyGetEditWindow (pmWindow);
    HWND	myStatusWindow = MyGetStatusWindow (myEditWindow);
    char	myString [1024];
    char	*myFormattedMessage;
    va_list 	myArgList;
    int		myResult;	    	
    
    if (myStatusWindow == NULL)
    {
    	return;
    }
    
    EdGUI_LoadString (pmMessageRsrc, myString, sizeof (myString));
    
    // Process the additional arguments for use in FormatMessage
    va_start (myArgList, pmMessageRsrc);
    
    // Format the message
    myResult = FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_STRING, myString, 0, 
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &myFormattedMessage, 0, &myArgList);
    if (myResult == 0)	  
    {
    	EdFail_Fatal (IDS_FORMATMESSAGEFAIL, __FILE__, __LINE__, 
    	    GetLastError (), myString);
    	// Never reaches here
    	return;
    }    

    // Close the variable args    
    va_end (myArgList);

    EdStat_Set (myStatusWindow, myFormattedMessage, NULL);

    LocalFree (myFormattedMessage);
} // EdWin_ShowStatus1


/************************************************************************/
/* EdWin_ShowStatusBold							*/
/************************************************************************/
void	EdWin_ShowStatusBold (HWND pmWindow, const char *pmMessage, ...)
{
    HWND	myEditWindow = MyGetEditWindow (pmWindow);
    HWND	myStatusWindow = MyGetStatusWindow (myEditWindow);
    
    if (myStatusWindow != NULL)
    {
    	va_list 	myArgList;
    
    	va_start (myArgList, pmMessage);
        EdStat_SetBold (myStatusWindow, pmMessage, myArgList);
    }
} // EdWin_ShowStatusBold


/************************************************************************/
/* EdWin_SetWindowTitle							*/
/************************************************************************/
void	EdWin_SetWindowTitle (HWND pmWindow)
{
    HWND	myEditWindow = MyGetEditWindow (pmWindow);
    FilePath	myPathForDisplayBuffer;
    
    EdDisp_GetPathForDisplay (MyGetTextDisplayWindow (myEditWindow), 
	USE_DEFAULT, SHOW_DIRTY_MARK, myPathForDisplayBuffer);

    if (gProgram.isJava)
    {
    	strcat (myPathForDisplayBuffer, " - Ready to Program");
    }
    else if (gProgram.isTuring)
    {
    	strcat (myPathForDisplayBuffer, " - Turing");
    }
	
    SetWindowText (myEditWindow, myPathForDisplayBuffer);
} // EdWin_SetWindowTitle


/******************************/
/* Static callback procedures */
/******************************/
/************************************************************************/
/* MyCopyExamplesDialogProcedure			                */
/*						 		        */
/* Callback procedure for the CopyExamples dialog box.			*/
/************************************************************************/
static BOOL CALLBACK MyCopyExamplesDialogProcedure (HWND pmDialog, 
						    UINT pmMessage, 
    						    WPARAM pmWParam, 
    						    LPARAM pmLParam)
{
    int			myCopyItem, cnt;
    Example		*myExample;
    static Examples	*stExamples;
    
    switch (pmMessage)
    {
    	case WM_INITDIALOG:
	    Ed_SetModalDialog (pmDialog); 
	    EdGUI_CentreDialogBox (pmDialog);
	    stExamples = (Examples *) pmLParam;
    	    return TRUE;
    	case WM_COMMAND:
    	    switch (LOWORD (pmWParam))
    	    {
    	    	case IDCANCEL:
    	    	case IDOK:
    	    	    EndDialog (pmDialog, TRUE);
    	    	    return TRUE;
    	    	default:
    	    	    myCopyItem = LOWORD (pmWParam - COPY_BUTTON);
    	    	    if ((myCopyItem < 0) || 
    	    	    	(myCopyItem >= stExamples -> numExamples))
    	    	    {
    	    	    	break;
    	    	    }
    	    	    myExample = stExamples -> head;
    	    	    for (cnt = 0 ; cnt < myCopyItem ; cnt++)
    	    	    {
    	    	    	myExample = myExample -> next;
    	    	    }
    	    	    MyCopyExampleDirectory (myExample, pmDialog);
    	    	    break;
    	    } // switch
    	    break;
    } // switch
    return FALSE;
} // MyCopyExamplesDialogProcedure


/************************************************************************/
/* MyEditorMDIFrameWindowProcedure					*/
/*									*/
/* Callback procedure for the main editor window.			*/
/************************************************************************/
static LRESULT CALLBACK MyEditorMDIFrameWindowProcedure (HWND pmEditWindow, 
							 UINT pmMessage, 
    							 WPARAM pmWParam, 
    							 LPARAM pmLParam)
{
    WindowCreationInfoPtr	myWindowCreationInfoPtr;
    HWND			myMDIClientWindow;
    
    // Log the windows messages
    if (stProperties.logWindowMessages)
    {
	EdLog_PrintWindowsMessage ("Edit MDI Frame", pmEditWindow, pmMessage, 
	    pmWParam, pmLParam);
    }
    
    switch (pmMessage)
    {
    	case WM_CREATE:
	    EdFail_AddMessage (NORMAL, "CMD: WM_CREATE (W)");
	    if (stProperties.logWindowMessages)
	    {
	    	EdLog_Log ("Edit Window = %x  (Starting Creation)", 
	    	    pmEditWindow);
	    }

	    myWindowCreationInfoPtr = 
	        (WindowCreationInfoPtr) 
	            (((LPCREATESTRUCT) pmLParam) -> lpCreateParams);
	    return 
	        MyCreateEditWindow (pmEditWindow, 
	    			    myWindowCreationInfoPtr -> pathName,
	    			    myWindowCreationInfoPtr -> exampleName,
	    			    myWindowCreationInfoPtr -> newSkeleton,
	    			    myWindowCreationInfoPtr -> beginnerMode,
	    			    myWindowCreationInfoPtr -> oneWindowMode);

        case WM_CLOSE:
            // TW This needs to be reworked for one window mode
	    EdFail_AddMessage (NORMAL, "CMD: WM_CLOSE (W)");
            if (EdDisp_CheckForClose (MyGetTextDisplayWindow (pmEditWindow)))
            {
            	if (Ed_RemoveFromWindowList (pmEditWindow))
            	{
            	    EdDisp_HandleClose (MyGetTextDisplayWindow (pmEditWindow));

            	    // Handle normally (= the window will be destroyed)
            	    break;
            	}
            }
            return 0;

	case WM_DESTROY:
	    EdFail_AddMessage (NORMAL, "CMD: WM_DESTROY (W)");
	    MyDispose (pmEditWindow);
	    break;
	    	    
	case WM_SIZE:
	    MyResizeEditWindow (pmEditWindow, RESIZE_IF_NECESSARY);
	    return 0;
	    
        case WM_COMMAND:
            if (MyCommandHandler (pmEditWindow, LOWORD (pmWParam)))
            {
                return 0;
            }
            if (pmLParam == SENT_FROM_EDDISP)
            {
            	EdFail_Fatal (IDS_COMMAND_NOT_PROCESSED, __FILE__, __LINE__, 0,
            	    LOWORD (pmWParam));
            }
	    return SendMessage (MyGetTextDisplayWindow (pmEditWindow), 
	    			WM_COMMAND, pmWParam, 
	    			(LPARAM) (SENT_FROM_EDWIN));

        case WM_ACTIVATE:
            if (LOWORD (pmWParam) == WA_INACTIVE)
            {
	    	EdFail_AddMessage (NORMAL, "CMD: WM_ACTIVATE (W) (deactivate)");
            	Ed_SetActiveWindow (NULL, NO_WINDOW);
            	EdWin_ShowStatus (pmEditWindow, "");
            }
            else
            {
	    	EdFail_AddMessage (NORMAL, "CMD: WM_ACTIVATE (W) (activate)");
		if (!Ed_CheckWindowPreemption ())
		{
            	    Ed_SetActiveWindow (pmEditWindow, EDIT_WINDOW);
            	    SetFocus (MyGetTextDisplayWindow (pmEditWindow));
		}
            }
            return 0;
            
        case WM_SETFOCUS:
            SetFocus (MyGetTextDisplayWindow (pmEditWindow));
            return 0;	

	case WM_GETMINMAXINFO:
	    if (GetWindowLong (pmEditWindow, WINDOW_MEM_WINDOW_INITIALIZED))
	    {
	    	MINMAXINFO	*myMinMaxInfo = (MINMAXINFO *) pmLParam;
	    	long		myWindowWidth, myWindowHeight;

		// Calculate the rectangle size
		MyGetWindowSize (pmEditWindow, MIN_ROWS, MIN_COLS, 
		    &myWindowWidth, &myWindowHeight);

		// Place the rectangle	    	
	    	myMinMaxInfo -> ptMinTrackSize.x = myWindowWidth;
	    	myMinMaxInfo -> ptMinTrackSize.y = myWindowHeight;
	    	  
	    	return 0;
	    }
	    break;
        
        case WM_SETTINGCHANGE:
	    // Set wheel mouse settings
	    EdFail_AddMessage (NORMAL, "CMD: WM_SETTINGCHANGE (W)");
	    EdDisp_MouseWheelSettingsInit ();
            return 0;
            
        case WM_INITMENU:
	    EdFail_AddMessage (NORMAL, "CMD: WM_INITMENU (W)");
            MyMenusAdjust (pmEditWindow, (HMENU) pmWParam);
            return 0;
    }
    
    // Get the MDI client window.  We have to deal with the fact that the
    // main window might not exist yet!
    myMDIClientWindow = NULL;
    
    if (GetWindowLong (pmEditWindow, WINDOW_MEM_WINDOW_TYPE) == 
    							WINDOW_TYPE_EDIT)
    {
	EditWindowInfoPtr	myInfo;
    	
    	myInfo = (EditWindowInfoPtr) GetWindowLong (pmEditWindow, 
						        WINDOW_MEM_WINDOW_INFO);
    	if (myInfo != NULL)
    	{
    	    myMDIClientWindow = myInfo -> mdiClientWindow;
    	}
    }
       
    return DefFrameProc (pmEditWindow, myMDIClientWindow, pmMessage, 
    			 pmWParam, pmLParam);
} // MyEditorMDIFrameWindowProcedure


/************************************************************************/
/* MyEditorWindowProcedure						*/
/*									*/
/* Callback procedure for the main editor window.			*/
/************************************************************************/
static LRESULT CALLBACK MyEditorWindowProcedure (HWND pmEditWindow, 
						 UINT pmMessage, 
    						 WPARAM pmWParam, 
    						 LPARAM pmLParam)
{
    WindowCreationInfoPtr	myWindowCreationInfoPtr;
    
    // Log the windows messages
    if (stProperties.logWindowMessages)
    {
	EdLog_PrintWindowsMessage ("Edit", pmEditWindow, pmMessage, 
	    pmWParam, pmLParam);
    }
    
    switch (pmMessage)
    {
    	case WM_CREATE:
	    EdFail_AddMessage (NORMAL, "CMD: WM_CREATE (W)");
	    if (stProperties.logWindowMessages)
	    {
	    	EdLog_Log ("Edit Window = %x  (Starting Creation)", 
	    	    pmEditWindow);
	    }

	    myWindowCreationInfoPtr = 
	        (WindowCreationInfoPtr) 
	            (((LPCREATESTRUCT) pmLParam) -> lpCreateParams);
	    return 
	        MyCreateEditWindow (pmEditWindow, 
	    			    myWindowCreationInfoPtr -> pathName,
	    			    myWindowCreationInfoPtr -> exampleName,
	    			    myWindowCreationInfoPtr -> newSkeleton,
	    			    myWindowCreationInfoPtr -> beginnerMode,
	    			    myWindowCreationInfoPtr -> oneWindowMode);

        case WM_CLOSE:
            // TW This needs to be reworked for one window mode
	    EdFail_AddMessage (NORMAL, "CMD: WM_CLOSE (W)");
            if (EdDisp_CheckForClose (MyGetTextDisplayWindow (pmEditWindow)))
            {
            	if (Ed_RemoveFromWindowList (pmEditWindow))
            	{
            	    EdDisp_HandleClose (MyGetTextDisplayWindow (pmEditWindow));

            	    // Handle normally (= the window will be destroyed)
            	    break;
            	}
            }
            return 0;

	case WM_DESTROY:
	    EdFail_AddMessage (NORMAL, "CMD: WM_DESTROY (W)");
	    MyDispose (pmEditWindow);
	    break;
	    	    
	case WM_SIZE:
	    MyResizeEditWindow (pmEditWindow, RESIZE_IF_NECESSARY);
	    return 0;
	    
        case WM_COMMAND:
            if (MyCommandHandler (pmEditWindow, LOWORD (pmWParam)))
            {
                return 0;
            }
            if (pmLParam == SENT_FROM_EDDISP)
            {
            	EdFail_Fatal (IDS_COMMAND_NOT_PROCESSED, __FILE__, __LINE__, 0,
            	    LOWORD (pmWParam));
            }
	    return SendMessage (MyGetTextDisplayWindow (pmEditWindow), 
	    			WM_COMMAND, pmWParam, 
	    			(LPARAM) (SENT_FROM_EDWIN));

        case WM_ACTIVATE:
	    EdFail_AddMessage (NORMAL, "CMD: WM_ACTIVATE (W)");
            if (LOWORD (pmWParam) == WA_INACTIVE)
            {
            	Ed_SetActiveWindow (NULL, NO_WINDOW);
            }
            else
            {
            	Ed_SetActiveWindow (pmEditWindow, EDIT_WINDOW);
            	SetFocus (MyGetTextDisplayWindow (pmEditWindow));
            }
            return 0;
            
        case WM_SETFOCUS:
	    EdFail_AddMessage (NORMAL, "CMD: WM_SETFOCUS (W)");
	    if (!Ed_CheckWindowPreemption ())
	    {
		SetFocus (MyGetTextDisplayWindow (pmEditWindow));
	    }
            return 0;	

//	case WM_TCARD:
//	    EdFail_AddMessage (NORMAL, "CMD: WM_TCARD");
//	    MyOpenExample (pmEditWindow, (int) pmWParam, (char *) pmLParam);
//	    return 0;

	case WM_GETMINMAXINFO:
	    if (GetWindowLong (pmEditWindow, WINDOW_MEM_WINDOW_INITIALIZED))
	    {
	    	MINMAXINFO	*myMinMaxInfo = (MINMAXINFO *) pmLParam;
	    	long		myWindowWidth, myWindowHeight;

		// Calculate the rectangle size
		MyGetWindowSize (pmEditWindow, MIN_ROWS, MIN_COLS, 
		    &myWindowWidth, &myWindowHeight);

		// Place the rectangle	    	
	    	myMinMaxInfo -> ptMinTrackSize.x = myWindowWidth;
	    	myMinMaxInfo -> ptMinTrackSize.y = myWindowHeight;
	    	  
	    	return 0;
	    }
	    break;
        
        case WM_SETTINGCHANGE:
	    // Set wheel mouse settings
	    EdFail_AddMessage (NORMAL, "CMD: WM_SETTINGCHANGE (W)");
	    EdDisp_MouseWheelSettingsInit ();
            return 0;
            
        case WM_INITMENU:
	    EdFail_AddMessage (NORMAL, "CMD: WM_INITMENU (W)");
            MyMenusAdjust (pmEditWindow, (HMENU) pmWParam);
            return 0;

	//
	// Program-defined messages
	//
	case WM_SET_BUTTONS:
	    EdBBar_SetButtons (MyGetButtonBarWindow (pmEditWindow), 
	    		       (int) pmWParam);
	    return 0;
	    
	case WM_SETSTATUSMESSAGE:
	    {
	    	char *myStatusMessage;
	    	
	    	myStatusMessage = (char *) pmWParam;
	    	
    	        EdWin_ShowStatus (pmEditWindow, myStatusMessage);
    	        
    	        free (myStatusMessage);
    	    }
    	    return 0;

    	case WM_PRINTOUTPUTJOB:
    	    {
    	    	OutputPrintJob	*myJob = (OutputPrintJob *) pmWParam;
    	    	
    	    	EdPrint_PrintOutput (pmEditWindow, myJob -> programOutput,
    	    			     myJob -> outputLen, myJob -> className);
    	    	if (myJob -> submitterWindow != NULL)
    	    	{
		    SetForegroundWindow (myJob -> submitterWindow);
		}
		free (myJob -> programOutput);
		free (myJob -> className);
		free (myJob);				           
	    }
	    return 0;

	case WM_CHANGEDEBUGGERCONTROL:
	    MyResizeEditWindow (pmEditWindow, ALWAYS_RESIZE);
	    SendMessage (MyGetButtonBarWindow (pmEditWindow), WM_SIZE, 0, 0);
	    return 0;
	    
	case WM_SETTRACESPEED:
    	    Ed_SendMessageToAllWindows (FALSE, WM_SETTRACESPEEDSLIDER,
	    				(WPARAM) pmEditWindow,
	    				(LPARAM) pmWParam);
    	    EdRun_SetTraceSpeed ((int) pmWParam);
    	    return 0;

	case WM_DEBUGGER_OUTPUT:
	    EdRun_DebuggerOutput ((int) pmLParam, (char *) pmWParam);
	    return 0;
	        	   
        case WM_SETTRACECHECKBOX:
        case WM_SETSHOWVARSCHECKBOX:
        case WM_SETTRACESPEEDSLIDER:
            if (pmEditWindow != (HWND) pmWParam)
            {
	        SendMessage (MyGetButtonBarWindow (pmEditWindow), 
	        	     pmMessage, pmLParam, 0);
	    }            	
    }
    
    return DefWindowProc (pmEditWindow, pmMessage, pmWParam, pmLParam);
} // MyEditorWindowProcedure


/************************************************************************/
/* MySunkenFrameWindowProcedure						*/
/*									*/
/* Callback routine from the raised frame window. 			*/
/************************************************************************/
static LRESULT CALLBACK	MySunkenFrameWindowProcedure (HWND pmWindow, 
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
    	DrawEdge (myDeviceContext, &myWindowRect, EDGE_SUNKEN, BF_RECT);
    	EndPaint (pmWindow, &myPaintStruct);
    	return 0;
    }
        	
    return DefWindowProc (pmWindow, pmMessage, pmWParam, pmLParam);
} // MySunkenFrameWindowProcedure


/*********************/
/* Static procedures */
/*********************/
/************************************************************************/
/* MyAlignOnDoubleWord							*/
/************************************************************************/
static LPWORD	MyAlignOnDoubleWord (LPWORD pmPtr)
{
    ULONG   myUnsignedLong;

    myUnsignedLong = (ULONG) pmPtr;
    myUnsignedLong +=3;
    myUnsignedLong >>=2;
    myUnsignedLong <<=2;
    return (LPWORD) myUnsignedLong;
} // MyAlignOnDoubleWord


/************************************************************************/
/* MyCommandHandler							*/
/************************************************************************/
static BOOL	MyCommandHandler (HWND pmEditWindow, short pmCommand)
{
    BOOL	myBeginnerMode, myOneWindowMode;
    
    myBeginnerMode = MyGetEditWindowInfo (pmEditWindow) -> beginnerMode;
    myOneWindowMode = MyGetEditWindowInfo (pmEditWindow) -> oneWindowMode;
    
    // Handle recent files commands
    if ((COMMAND_RECENT_BASE <= pmCommand) &&
        (pmCommand <= COMMAND_LAST_RECENT_COMMAND))
    {
	if (myBeginnerMode ||
	    ((EdDisp_GetUnnamedNumber (
	       MyGetTextDisplayWindow (pmEditWindow)) == 1) && 
	     EdDisp_IsEmptyFile (MyGetTextDisplayWindow (pmEditWindow))))
	{
	    // Let this be handled in the EdDisp event loop
	    // which will cause the new file to opened over
	    // the current window.
	    return FALSE;
	}
	
	EdFail_AddMessage (NORMAL, "CMD: COMMAND_RECENT");
	EdFail_AddMessage (NORMAL, 
			   "Open Recent File (W) - Window: %x - Recent: %d",
	    		   pmEditWindow, pmCommand - COMMAND_RECENT_BASE);
	    		   
	if (myOneWindowMode)
	{
	    EdWin_AddFile (pmEditWindow,
	    		   EdRec_GetPathName (pmCommand - COMMAND_RECENT_BASE), 
			   NULL, 0);
	}
	else
	{
	    EdWin_Create (EdRec_GetPathName (pmCommand - COMMAND_RECENT_BASE), 
	    		  NULL, 0, myBeginnerMode, myOneWindowMode);
    	}
    	return TRUE;
    }
    
    // Handle new skeleton code commands
    if ((COMMAND_NEW_BASE <= pmCommand) &&
        (pmCommand <= COMMAND_LAST_NEW_COMMAND))
    {
	if (myBeginnerMode ||
	    ((EdDisp_GetUnnamedNumber (
	       MyGetTextDisplayWindow (pmEditWindow)) == 1) && 
	     EdDisp_IsEmptyFile (MyGetTextDisplayWindow (pmEditWindow))))
	{
	    // Let this be handled in the EdDisp event loop
	    // which will cause the new file to opened over
	    // the current window.
	    return FALSE;
	}
	
	EdFail_AddMessage (NORMAL, "CMD: COMMAND_NEW");
	EdFail_AddMessage (NORMAL, "Open New File (W) - Window: %x - New: %d",
	    		   pmEditWindow, pmCommand - COMMAND_NEW_BASE + 1);

	if (myOneWindowMode)
	{
	    EdWin_AddFile (pmEditWindow, NULL, NULL, 
			   pmCommand - COMMAND_NEW_BASE + 1);
	}
	else
	{
	    EdWin_Create (NULL, NULL, pmCommand - COMMAND_NEW_BASE + 1, 
	    		  myBeginnerMode, myOneWindowMode);
    	}
    	return TRUE;
    }
    
    switch (pmCommand)
    {
	case COMMAND_NEW_UNNAMED:
	    if (myBeginnerMode)
	    {
	    	return FALSE;
	    }
	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_NEW_UNNAMED");
	    if (myOneWindowMode)
	    {
	        EdWin_AddFile (pmEditWindow, NULL, NULL, 0);
	    }
	    else
	    {
	    	EdWin_Create (NULL, NULL, 0, myBeginnerMode, myOneWindowMode);
	    }
	    break;
    	case COMMAND_OPEN_FILE:
	    if (myBeginnerMode ||
	        ((EdDisp_GetUnnamedNumber (
	           MyGetTextDisplayWindow (pmEditWindow)) == 1) && 
	         EdDisp_IsEmptyFile (MyGetTextDisplayWindow (pmEditWindow))))
	    {
		// Let this be handled in the EdDisp event loop
		// which will cause the new file to opened over
		// the current window.
	    	return FALSE;
	    }
	    else
	    {
	        FilePath	myPathName;
	        
	        EdFail_AddMessage (NORMAL, "CMD: COMMAND_OPEN_FILE (W)");
	        
	        // Get the path name from a Open File dialog
    	    	if (!EdGUI_GetOpenFile (pmEditWindow, myPathName))
    	    	{
       	    	    return TRUE;
       	    	}
       	    	
       	    	EdWin_OpenFile (pmEditWindow, myPathName, NULL, PURPOSE_LOAD_FILE);
	    }
    	    break;
    	case COMMAND_CLOSE_FILE:
	    if (myOneWindowMode)
	    {
	    	return FALSE;
	    }
	    else
	    {
	        EdFail_AddMessage (NORMAL, "CMD: COMMAND_CLOSE_FILE");
	    	SendMessage (pmEditWindow, WM_CLOSE, 0, 0);
	    }
	    break;
	case COMMAND_PAGE_SETUP:
	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_PAGE_SETUP");
    	    EdPrint_PageSetup (pmEditWindow);
	    break;
	case COMMAND_REDRAW:
	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_REDRAW");
	    Ed_RedrawWindows ();
	    return TRUE;
	case COMMAND_COPY_EXAMPLES:
	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_COPY_EXAMPLES");
	    EdWin_CopyExamples (pmEditWindow);
	    return TRUE;
	case COMMAND_PREFS:
	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_PREFS");
    	    EdProp_ChangePreferences (pmEditWindow);
	    break;
	case COMMAND_EXIT:
	case COMMAND_EXIT_IMMEDIATE:
	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_EXIT");
	    Ed_ExitApplication (pmEditWindow, 
	    			(pmCommand == COMMAND_EXIT_IMMEDIATE));
    	    break;
	case COMMAND_STOP:
	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_STOP");
	    EdRun_KillRunningProgram ();
	    break;
	case COMMAND_PAUSERESUME:
	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_PAUSERESUME");
	    if (EdRun_IsProgramRunning ())
	    {
	    	if (EdRun_IsProgramRunningAnApplet ())
	    	{
    	    	    EdGUI_Message1 (pmEditWindow, 0, IDS_NO_PAUSE_TITLE,
    	    	    	IDS_CANT_PAUSE_APPLET_MESSAGE);
		}
		else
		{
		    EdRun_PauseResumeProgram (ACTIVATE_SOURCE_WINDOW, PAUSING);
		}
	    }
    	    else
	    {
    	    	EdGUI_Message1 (pmEditWindow, 0, IDS_NO_PAUSE_TITLE,
    	    	    IDS_NO_PROGRAM_RUNNING_MESSAGE);
	    }    	    	    
	    break;
	case COMMAND_COMPILER_RESET:
	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_COMPILER_RESET");
	    EdRun_ResetCompiler ();
	    break;
	case COMMAND_CLOSE_ALL_RUN_WIN:
	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_CLOSE_ALL_RUN_WIN");
	    EdRun_CloseAllRunWindows (pmEditWindow);
	    break;
	case COMMAND_SHOW_HIDE_DEBUG_MENU:
	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_SHOW_HIDE_DEBUG_MENU");
	    EdMenu_ShowHideDebuggerMenu (TOGGLE_MENU);
	    break;
	case COMMAND_DEBUG_SHOW_HIDE_CONTROLS:
	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_DEBUG_SHOW_HIDE_CONTROLS");
	    Ed_ToggleDebuggerControlsVisibility ();
	    break;
	case COMMAND_DEBUG_SHOW_BREAKPOINTS:
	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_DEBUG_SHOW_BREAKPOINTS");
	    Ed_ToggleBreakpointVisibility ();
	    break;
	case COMMAND_DEBUG_CLEAR_BREAKPOINTS:
	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_DEBUG_CLEAR_BREAKPOINTS");
	    Ed_ClearBreakpoints ();
	    break;	    
	case COMMAND_DEBUG_SHOW_ALLOCATED_OBJECTS:
	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_DEBUG_SHOW_ALLOCATED_OBJECTS");
	    DbgAlloc_ToggleWindowVisibility ();
	    break;
	case COMMAND_DEBUG_STREAM_0_TO_STDERR:
	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_DEBUG_STREAM_0_TO_STDERR");
	    gProperties.stream0ToStderr = !gProperties.stream0ToStderr;
	    EdRun_SetStream0Destination ();
	    break;
	case COMMAND_DEBUG_ERRORS_TO_STDERR:
	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_DEBUG_ERRORS_TO_STDERR");
	    gProperties.errorsToStderr = !gProperties.errorsToStderr;
	    EdRun_SendInfoToStderr (gProperties.errorsToStderr,
				    gProperties.infoToStderr);
	    break;
	case COMMAND_DEBUG_LIB_INFO_TO_STDERR:
	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_DEBUG_LIB_INFO_TO_STDERR");
	    gProperties.infoToStderr = !gProperties.infoToStderr;
	    EdRun_SendInfoToStderr (gProperties.errorsToStderr,
				    gProperties.infoToStderr);
	    break;
	   
	// These commands come from the debugger controls.
	case COMMAND_DEBUG_TRACE:
	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_DEBUG_TRACE");
	    EdRun_ToggleTracing ();
	    // Toggle all the other windows check box.
	    Ed_SendMessageToAllWindows (FALSE, WM_SETTRACECHECKBOX,
	    				(WPARAM) pmEditWindow,
	    				(LPARAM) EdRun_IsTracing ());
	    break;
	    	    
    	case COMMAND_HELP_CLASSLIB:
	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_HELP_CLASSLIB");
    	    {
    	        FilePath	myHelpPath;
    	        
    	        EdFile_ConstructPath (myHelpPath, IDS_HELP_DIR, 
    	            IDS_CLASS_LIB_HELP_FILE);
    	        HtmlHelp (gProgram.globalWindow, myHelpPath, HH_DISPLAY_TOC, 0);
    	    }
    	    break;
    	case COMMAND_HELP_READY_OBTAIN:
	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_HELP_READY_OBTAIN");
    	    ShellExecute (NULL, "open", 
    	    		  "http://www.holtsoft.com/ready/avail.html",
    	    		  NULL, NULL, SW_SHOWNORMAL);
	    break;    	    		  
    	
    	case COMMAND_HELP_READY_HOME:
	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_HELP_READY_HOME");
    	    ShellExecute (NULL, "open", "http://www.holtsoft.com/ready",
    	    		  NULL, NULL, SW_SHOWNORMAL);
	    break;    	    		  
    	case COMMAND_HELP_READY_SUPPORT:
	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_HELP_READY_SUPPORT");
    	    ShellExecute (NULL, "open", "http://www.holtsoft.com/ready/support",
    	    		  NULL, NULL, SW_SHOWNORMAL);
	    break;    	    		  
    	case COMMAND_HELP_READY_TEXTBOOKS:
	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_HELP_READY_TEXTBOOKS");
    	    ShellExecute (NULL, "open", "http://www.holtsoft.com/ready/textbooks",
    	    		  NULL, NULL, SW_SHOWNORMAL);
	    break;    	    		  
    	case COMMAND_HELP_READY_RESOURCES:
	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_HELP_READY_RESOURCES");
    	    ShellExecute (NULL, "open", "http://www.holtsoft.com/ready/resources",
    	    		  NULL, NULL, SW_SHOWNORMAL);
	    break;    	    		  
    	case COMMAND_HELP_JAVA_HELP:
	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_HELP_JAVA_HELP");
    	    ShellExecute (NULL, "open", "http://www.holtsoft.com/ready/morehelp",
    	    		  NULL, NULL, SW_SHOWNORMAL);
	    break;    	    		  
    	case COMMAND_HELP_IBM_ACAD:
	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_HELP_IBM_ACAD");
    	    ShellExecute (NULL, "open", "http://www.ibm.com/java/academic",
    	    		  NULL, NULL, SW_SHOWNORMAL);
	    break;    	    		  
    	case COMMAND_HELP_IBM_HOME:
	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_HELP_READY_TEXTBOOKS");
    	    ShellExecute (NULL, "open", "http://www.ibm.com",
    	    		  NULL, NULL, SW_SHOWNORMAL);
	    break;    	
    	case COMMAND_HELP_HSA_HOME:
	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_HELP_HSA_HOME");
    	    ShellExecute (NULL, "open", "http://www.holtsoft.com",
    	    		  NULL, NULL, SW_SHOWNORMAL);
	    break;    	    		  
    	case COMMAND_HELP_TURING_REF:
	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_HELP_TURING_REF");
    	    {
    	        FilePath	myHelpPath;
    	        
    	        EdFile_ConstructPath (myHelpPath, IDS_HELP_DIR, 
    	            IDS_TURING_HELP_FILE);
    	        HtmlHelp (gProgram.globalWindow, myHelpPath, HH_DISPLAY_TOC, 0);
    	    }
    	    break;
    	case COMMAND_HELP_TURING_OBTAIN:
	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_HELP_TURING_OBTAIN");
    	    ShellExecute (NULL, "open", 
    	    		  "http://www.holtsoft.com/turing/avail.html",
    	    		  NULL, NULL, SW_SHOWNORMAL);
	    break;    	    		  
    	case COMMAND_HELP_TURING_HOME:
	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_HELP_TURING_HOME");
    	    ShellExecute (NULL, "open", "http://www.holtsoft.com/turing",
    	    		  NULL, NULL, SW_SHOWNORMAL);
	    break;    	    		  
    	case COMMAND_HELP_TURING_SUPPORT:
	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_HELP_TURING_SUPPORT");
    	    ShellExecute (NULL, "open", "http://www.holtsoft.com/turing/support",
    	    		  NULL, NULL, SW_SHOWNORMAL);
	    break;    	    		  
    	case COMMAND_HELP_TURING_UPDATE:
	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_HELP_TURING_UPDATE");
    	    {
    	    	char	myCommand [1024];
    	    	
    	    	wsprintf (myCommand, 
    	    	    "http://www.holtsoft.com/turing/support/update%c.html?%s",
    	    	    gProgram.versionNumber [0], gProgram.versionNumber);
    	    	ShellExecute (NULL, "open", myCommand, NULL, NULL, 
    	    		      SW_SHOWNORMAL);
	    }    	    		     
	    break;    	    		  
    	case COMMAND_HELP_TURING_TEXTBOOKS:
	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_HELP_TURING_TEXTBOOKS");
    	    ShellExecute (NULL, "open", "http://www.holtsoft.com/turing/textbooks",
    	    		  NULL, NULL, SW_SHOWNORMAL);
	    break;    	    		  
    	case COMMAND_HELP_TURING_RESOURCES:
	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_HELP_TURING_RESOURCES");
    	    ShellExecute (NULL, "open", "http://www.holtsoft.com/turing/resources",
    	    		  NULL, NULL, SW_SHOWNORMAL);
	    break;    	    		  
    	case COMMAND_HELP_TURING_HELP:
	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_HELP_TURING_HELP");
    	    ShellExecute (NULL, "open", "http://www.holtsoft.com/turing/morehelp",
    	    		  NULL, NULL, SW_SHOWNORMAL);
	    break;    	    		  
    	case COMMAND_ABOUT:
    	    // Display the splash screen
	    EdFail_AddMessage (NORMAL, "CMD: COMMAND_ABOUT");
    	    EdSplash_Create (TRUE);
	    break;
    	default:
    	    return FALSE;
    }
    
    return TRUE;
} // MyCommandHandler


/************************************************************************/
/* MyCopyExampleDirectory						*/
/************************************************************************/
static void	MyCopyExampleDirectory (Example *pmExample, HWND pmWindow)
{
    FilePath		mySourceDirectory, myDestDirectory;
    int			myResult;
    char		*myDestName;
    SHFILEOPSTRUCT	myFileOp;

    if (gProgram.isJava)
    {
	myDestName = "Java Examples";
    }
    else
    {
	myDestName = "Turing Examples";
    }

    // First verify that the source directory exists.
    EdFile_ConstructPath (mySourceDirectory, IDS_EXAMPLES_DIR, NO_FILE_NAME);
    strcat (mySourceDirectory, pmExample -> directoryName);
    
    // Make sure the string ends in a double-null
    mySourceDirectory [strlen (mySourceDirectory) + 1] = 0;

    if (!EdFile_DirectoryExists (mySourceDirectory))
    {
	EdGUI_Message1 (pmWindow, 0, IDS_EXAMPLES_COPY_FAILED_TITLE,
			IDS_CANNOT_FIND_SOURCE_DIR, pmExample -> label, 
			pmExample -> directoryName);
	return;
    }
    				   
    // First, determine if the directory already exists, and if so 
    // ask the user if they want it replaced with a fresh copy.
    EdFile_ConstructPath (myDestDirectory, FROM_STARTUP, EXAMPLES_DIR);
    
    // Make sure the Java/Turing Examples directory exists (to be fixed later)
    if (!EdFile_DirectoryExists (myDestDirectory))
    {
    	CreateDirectory (myDestDirectory, NULL);
    }
    strcat (myDestDirectory, "\\");
    strcat (myDestDirectory, pmExample -> directoryName);

    // Make sure the string ends in a double-null
    myDestDirectory [strlen (myDestDirectory) + 1] = 0;

    if (EdFile_DirectoryExists (myDestDirectory))
    {
	myResult = EdGUI_Message1 (pmWindow, MB_OKCANCEL,
				   IDS_REPLACE_EXAMPLE_DIR_TITLE,
				   IDS_REPLACE_EXAMPLE_DIR,
				   pmExample -> directoryName, myDestName);
	if (myResult == IDCANCEL)
	{
	    return;
	}

	// Then delete the directory tree in the destination location
	ZeroMemory (&myFileOp, sizeof (SHFILEOPSTRUCT));

	myFileOp.hwnd = pmWindow;
	myFileOp.wFunc = FO_DELETE;
	myFileOp.pFrom = myDestDirectory;
	myFileOp.pTo = NULL;
	myFileOp.fFlags = FOF_NOCONFIRMATION;
	myFileOp.fAnyOperationsAborted = FALSE;
	myFileOp.hNameMappings = NULL;
	myFileOp.lpszProgressTitle = NULL;
	myResult = SHFileOperation (&myFileOp);
	if (myResult != 0)
	{
	    // Delete failed or was cancelled, etc.
	    return;
	}
    }

    // Then trace the destination tree, making directories and copying 
    // files as necessary
    EdFile_ConstructPath (mySourceDirectory, IDS_EXAMPLES_DIR, NO_FILE_NAME);
    strcat (mySourceDirectory, pmExample -> directoryName);

    // Make sure the string ends in a double-null
    mySourceDirectory [strlen (mySourceDirectory) + 1] = 0;

    ZeroMemory (&myFileOp, sizeof (SHFILEOPSTRUCT));
    
    myFileOp.hwnd = pmWindow;
    myFileOp.wFunc = FO_COPY;
    myFileOp.pFrom = mySourceDirectory;
    myFileOp.pTo = myDestDirectory;
    myFileOp.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR;
    myFileOp.fAnyOperationsAborted = FALSE;
    myFileOp.hNameMappings = NULL;
    myFileOp.lpszProgressTitle = NULL;
    myResult = SHFileOperation (&myFileOp);

    if (myResult == 0)
    {
	EdGUI_Message1 (pmWindow, 0, IDS_EXAMPLES_COPIED_TITLE,
		        IDS_EXAMPLES_COPIED, pmExample -> label,
			pmExample -> directoryName, myDestName);
    }
    else
    {
	if (myFileOp.fAnyOperationsAborted)
	{
	    EdGUI_Message1 (pmWindow, 0, IDS_EXAMPLES_COPY_FAILED_TITLE,
		IDS_EXAMPLES_COPY_ABORTED, pmExample -> label);

	    // The user aborted the operation, get rid of the partial copy.
	    ZeroMemory (&myFileOp, sizeof (SHFILEOPSTRUCT));
	    
	    myFileOp.hwnd = pmWindow;
	    myFileOp.wFunc = FO_DELETE;
	    myFileOp.pFrom = myDestDirectory;
	    myFileOp.pTo = NULL;
	    myFileOp.fFlags = FOF_NOCONFIRMATION | FOF_SILENT | FOF_NOERRORUI;
	    myFileOp.fAnyOperationsAborted = FALSE;
	    myFileOp.hNameMappings = NULL;
	    myFileOp.lpszProgressTitle = NULL;
	    SHFileOperation (&myFileOp);
	}
	else
	{
	    EdGUI_Message1 (pmWindow, 0, IDS_EXAMPLES_COPY_FAILED_TITLE,
		IDS_EXAMPLES_COPY_FAILED, pmExample -> label);
	}
    }
} // MyCopyExampleDirectory


/************************************************************************/
/* MyCreateEditWindow							*/
/************************************************************************/
static int	MyCreateEditWindow (HWND pmEditWindow, const char *pmPathName, 
				    const char *pmExampleName, int pmNewSkeleton, 
				    BOOL pmBeginnerMode, BOOL pmOneWindowMode)
{
    EditWindowInfoPtr	myInfo;
    HMENU		myFileMenu, myRecentMenu;
    long		myWindowLeft, myWindowTop;
    long		myWindowWidth, myWindowHeight;
    
    //
    // Initialize the extra window memory
    //
	
    // Set the window type            
    SetWindowLong (pmEditWindow, WINDOW_MEM_WINDOW_TYPE, 
        (long) WINDOW_TYPE_EDIT);

    // Set the window initialized                
    SetWindowLong (pmEditWindow, WINDOW_MEM_WINDOW_INITIALIZED, (long) FALSE);
            
    // Create the ddit window information
    myInfo = MyCreateEditWindowInfo ();
    if (myInfo == NULL)
    {
    	return -1;
    }
    SetWindowLong (pmEditWindow, WINDOW_MEM_WINDOW_INFO, (long) myInfo);
    	    
    //
    // Add the elements to the window
    //

    myInfo -> beginnerMode = pmBeginnerMode;
    myInfo -> oneWindowMode = pmOneWindowMode;
        
    // Add the Edit box (an edit window)
    myInfo -> buttonBarWindow = EdBBar_Create (pmEditWindow);
    if (myInfo -> buttonBarWindow == NULL)
    	return -1;

    // Add the tab bar if in one window mode
    if (myInfo -> oneWindowMode)
    {
    	HINSTANCE		myApplicationInstance;
    	char			mySunkenFrameWindowClassName [256];
    	CLIENTCREATESTRUCT	myClientCreate;
	HMENU			myEditMenu;
	    	
    	myApplicationInstance = 
    	    (HINSTANCE) GetWindowLong (pmEditWindow, GWL_HINSTANCE);
    	    
    	// Create the tab bar
    	myInfo -> tabBarWindow = CreateWindow (WC_TABCONTROL, "", 
            WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 
            0, 0, 0, 0, pmEditWindow, NULL, myApplicationInstance, NULL);
        if (myInfo -> tabBarWindow == NULL)
        {
	    EdFail_Warn (IDS_CREATEWINDOWFAIL, __FILE__, __LINE__, 
	        GetLastError ());
            return -1;
        }
        
	// Create the sunken frame in the tab display area
    	EdGUI_LoadString (IDS_SUNKEN_FRAME_WINDOW_NAME,
    	    mySunkenFrameWindowClassName, 
    	    sizeof (mySunkenFrameWindowClassName));
		
    	myInfo -> sunkenWindow = CreateWindow (mySunkenFrameWindowClassName,
    	    NULL, WS_CHILD | WS_DISABLED | WS_VISIBLE, 0, 0, 0, 0,
    	    pmEditWindow, NULL, myApplicationInstance, NULL);
        if (myInfo -> sunkenWindow == NULL)
        {
	    EdFail_Warn (IDS_CREATEWINDOWFAIL, __FILE__, __LINE__, 
	        GetLastError ());
            return -1;
        }
    	    
	// Create the MDI client window
	myClientCreate.hWindowMenu = NULL;
	myClientCreate.idFirstChild = MDI_WINDOW_BASE;
	myInfo -> mdiClientWindow = CreateWindow ("MDICLIENT", NULL,
	    WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE, 
	    0, 0, 0, 0, pmEditWindow, (HMENU) 1, myApplicationInstance, 
	    (PSTR) &myClientCreate);
        if (myInfo -> mdiClientWindow == NULL)
        {
	    EdFail_Warn (IDS_CREATEWINDOWFAIL, __FILE__, __LINE__, 
	        GetLastError ());
            return -1;
        }

    	myInfo -> textDisplayWindow [0] = EdDisp_Create (
    	    myInfo -> mdiClientWindow, myInfo -> beginnerMode, 
    	    myInfo -> oneWindowMode);
    	if (myInfo -> textDisplayWindow [0] == NULL)
    	{
    	    // An error message was given in eddisp.c
    	    return -1;
    	}
    	
    	myEditMenu = LoadMenu (myApplicationInstance, 
    			       MAKEINTRESOURCE (EDIT_WINDOW_MENUS));
	SendMessage (myInfo -> mdiClientWindow, WM_MDISETMENU, 
		     (WPARAM) myEditMenu, (LPARAM) NULL);
    }
    else
    {
    	myInfo -> textDisplayWindow [0] = EdDisp_Create (pmEditWindow,
    	    myInfo -> beginnerMode, myInfo -> oneWindowMode);
    	if (myInfo -> textDisplayWindow [0] == NULL)
    	    return -1;
    }
    myInfo -> numTextDisplayWindows = 1;
    myInfo -> activeTextDisplayWindow = 0;
    						 
    myInfo -> statusWindow = EdStat_Create (pmEditWindow);
    if (myInfo -> statusWindow == NULL)
     	return -1;
     	
    // If the font properties have not already been determined, 
    // determine them now (we needed a window to do so).  This must be
    // called *after* the top widow and status window created
    EdWin_PropertiesImplementChanges (pmEditWindow);

    // Remove the dummy menu item from the Recent folder
    myFileMenu = EdMenu_GetMenu (pmEditWindow, FILE_MENU);
    myRecentMenu = GetSubMenu (myFileMenu, RECENT_MENU_POSITION);
    RemoveMenu (myRecentMenu, 0, MF_BYPOSITION);
    EnableMenuItem (myFileMenu, RECENT_MENU_POSITION, 
    	MF_BYPOSITION | MF_GRAYED);

    // Set the window to initialized
    SetWindowLong (pmEditWindow, WINDOW_MEM_WINDOW_INITIALIZED, (long) TRUE);
    
    // Add to the window list
    Ed_AddToWindowList (pmEditWindow, WINDOW_TYPE_EDIT);

    // Create the "New" and "Insert" menus
    MyCreateNewandInsertMenu (pmEditWindow);
    
    // Create the "Recent" menu
    EdWin_CreateRecentMenu (pmEditWindow);

    // Add the "Debugger" and "Window" menus if they're selected
    EdMenu_AddDebuggerWindowMenuIfNecessary (pmEditWindow);
    
    // Add the "How to get Ready/Turing" to the evaluation menu"
    if (gProgram.installKind == INSTALL_KIND_EVAL)
    {
    	EdMenu_AddHowToGetReadyMenuItem (pmEditWindow);
    }

    if (gProperties.debuggerAvailable && !gProperties.debuggerMenu &&
    	!myInfo -> beginnerMode)
    {
	EdMenu_AddDebuggerMenuItem (pmEditWindow);
    }

    // If this is in beginner mode, delete the Close menu item.
    if (myInfo -> beginnerMode)
    {
    	DeleteMenu (myFileMenu, COMMAND_CLOSE_FILE, MF_BYCOMMAND);
    }
    
    // If this is in one window mode, add the tab
    if (myInfo -> oneWindowMode)
    {
    	TC_ITEM		myTabItem;
    	FilePath	myPathForTab;
	
    	myTabItem.mask = TCIF_TEXT;
    	myTabItem.pszText = EdDisp_GetPathForDisplay (
    	    MyGetTextDisplayWindow (pmEditWindow), USE_FILENAME, 
    	    SHOW_DIRTY_MARK, myPathForTab);

        if (TabCtrl_InsertItem (myInfo -> tabBarWindow, 0, &myTabItem) == -1)
        { 
            // TW Error message
            return -1;
        }
    }

    // Calculate the window size
    MyGetWindowSize (pmEditWindow, stProperties.textRows, stProperties.textCols, 
 		     &myWindowWidth, &myWindowHeight);

    // Calculate the top, left corner of the window
    MyGetWindowDisplacement (FALSE, &myWindowWidth, &myWindowHeight,
    			     &myWindowLeft, &myWindowTop);

    // Set the window to the correct position
    SetWindowPos (pmEditWindow, HWND_TOP, myWindowLeft, myWindowTop,
    		  myWindowWidth, myWindowHeight, 0);

    // Load the file into text display window    	    
    EdDisp_Create2 (myInfo -> textDisplayWindow [0], pmPathName,
		    pmExampleName, pmNewSkeleton);
    
    return 0;
} // MyCreateEditWindow


/************************************************************************/
/* MyCreateEditWindowInfo						*/
/************************************************************************/
static EditWindowInfoPtr	MyCreateEditWindowInfo (void)
{
    EditWindowInfoPtr	myInfo;
    
    // Allocate the structure for the information about the edit window
    myInfo = (EditWindowInfoPtr) malloc (sizeof (EditWindowInfo));
    
    if (myInfo != NULL)
    {
    	ZeroMemory (myInfo, sizeof (EditWindowInfo));
    }

    return myInfo;  
} // MyCreateEditWindowInfo


/************************************************************************/
/* MyCreateNewandInsertMenu						*/
/************************************************************************/
static void	MyCreateNewandInsertMenu (HWND pmEditWindow)
{
    HMENU	myFileMenu, myNewMenu, myEditMenu, myInsertMenu;

    myFileMenu = EdMenu_GetMenu (pmEditWindow, FILE_MENU);
    myNewMenu = GetSubMenu (myFileMenu, NEW_MENU_POSITION);
    myEditMenu = EdMenu_GetMenu (pmEditWindow, EDIT_MENU);
    myInsertMenu = GetSubMenu (myEditMenu, INSERT_MENU_POSITION);

    EdMenu_CreateNewInsertMenus (myNewMenu, myInsertMenu);

    // If we're in beginner mode, or there are no skeletons, turn
    // the New menu into a simple New menu item
    if ((MyGetEditWindowInfo (pmEditWindow) -> beginnerMode) ||
        (GetMenuItemCount (myNewMenu) == 1))
    {
        MENUITEMINFO	myMenuInfo;
        char		myNewMenuItemName [80];
        
        EdGUI_LoadString (IDS_NEW_MENU_ITEM, myNewMenuItemName, 
            sizeof (myNewMenuItemName));

    	myMenuInfo.cbSize = sizeof (MENUITEMINFO);
    	myMenuInfo.fMask = MIIM_SUBMENU | MIIM_ID | MIIM_TYPE;
    	myMenuInfo.fType = MFT_STRING;
    	myMenuInfo.dwTypeData = myNewMenuItemName;
    	myMenuInfo.hSubMenu = NULL;
    	myMenuInfo.wID = COMMAND_NEW_UNNAMED;

    	SetMenuItemInfo (myFileMenu, NEW_MENU_POSITION, TRUE, &myMenuInfo);
    }  
      
    if (GetMenuItemCount (myInsertMenu) == 0)
    {
    	EnableMenuItem (myEditMenu, INSERT_MENU_POSITION, 
    	    MF_BYPOSITION | MF_GRAYED);
    }
    else
    {
    	EnableMenuItem (myEditMenu, INSERT_MENU_POSITION, 
    	    MF_BYPOSITION | MF_ENABLED);
    }
} // MyCreateNewandInsertMenu


/************************************************************************/
/* MyDispose								*/
/************************************************************************/
static void	MyDispose (HWND pmEditWindow)
{
    EditWindowInfoPtr	myInfo;
    
    myInfo = MyGetEditWindowInfo (pmEditWindow);
        
    // Make certain we don't get information from a closed window.
    SetWindowLong (pmEditWindow, WINDOW_MEM_WINDOW_TYPE, -999);
    SetWindowLong (pmEditWindow, WINDOW_MEM_WINDOW_INITIALIZED, (long) FALSE);
    SetWindowLong (pmEditWindow, WINDOW_MEM_WINDOW_INFO, (long) -997);

    free (myInfo);
} // MyDispose


/************************************************************************/
/* MyGetEditWindow							*/
/************************************************************************/
static HWND	MyGetEditWindow (HWND pmWindow)
{
    HWND		myWindow;
    int			myWindowType;
    
    myWindow = pmWindow;
    while (GetParent (myWindow) != NULL)
    {
    	myWindow = GetParent (myWindow);
    }
    
    myWindowType = GetWindowLong (myWindow, WINDOW_MEM_WINDOW_TYPE);
    if (myWindowType != WINDOW_TYPE_EDIT)
    {
    	EdFail_Fatal (IDS_BADWINDOWTYPE, __FILE__, __LINE__, 0,
    		      myWindowType, WINDOW_TYPE_EDIT);
    }

    return myWindow;    	
} // MyGetEditWindow


/************************************************************************/
/* MyGetEditWindowInfo							*/
/************************************************************************/
static EditWindowInfoPtr	MyGetEditWindowInfo (HWND pmEditWindow)
{
    int			myWindowType;
    EditWindowInfoPtr	myInfo;
    
    myWindowType = GetWindowLong (pmEditWindow, WINDOW_MEM_WINDOW_TYPE);
    if (myWindowType != WINDOW_TYPE_EDIT)
    {
    	EdFail_Fatal (IDS_BADWINDOWTYPE, __FILE__, __LINE__, 0,
    		      myWindowType, WINDOW_TYPE_EDIT);
    }
    	
    myInfo = (EditWindowInfoPtr) GetWindowLong (pmEditWindow, 
        WINDOW_MEM_WINDOW_INFO);
    if (myInfo == NULL)
    {
    	EdFail_Fatal (IDS_GETWINDOWLONGFAIL, __FILE__, __LINE__,
    		      GetLastError (), WINDOW_MEM_WINDOW_INFO);
    }
       
    return myInfo;
} // MyGetEditWindowInfo


/************************************************************************/
/* MyGetStatusWindow							*/
/************************************************************************/
static HWND	MyGetStatusWindow (HWND pmEditWindow)
{
    return MyGetEditWindowInfo (pmEditWindow) -> statusWindow;
} // MyGetStatusWindow


/************************************************************************/
/* MyGetButtonBarWindow							*/
/************************************************************************/
static HWND	MyGetButtonBarWindow (HWND pmEditWindow)
{
    return MyGetEditWindowInfo (pmEditWindow) -> buttonBarWindow;
} // MyGetButtonBarWindow


/************************************************************************/
/* MyGetTextDisplayWindow						*/
/************************************************************************/
static HWND	MyGetTextDisplayWindow (HWND pmEditWindow)
{
    EditWindowInfoPtr	myInfo;
    
    // Get the window information
    myInfo = MyGetEditWindowInfo (pmEditWindow);
    
    return myInfo -> textDisplayWindow [myInfo -> activeTextDisplayWindow];
} // MyGetTextDisplayWindow


/************************************************************************/
/* MyGetWindowDisplacement						*/
/************************************************************************/
static void	MyGetWindowDisplacement (BOOL pmReset, long *pmWindowWidth, 
					 long *pmWindowHeight, long *pmLeft, 
					 long *pmTop)
{
    RECT	myScreenRect;
    static int	myCurrentLeft = 0;
    static int 	myCurrentTop = 0;
    
    if (pmReset)
    {
    	myCurrentLeft = 0;
    	myCurrentTop = 0;
    	return;
    }
    
    // Calculate the top, left corner of the window
    myScreenRect = EdGUI_GetWorkArea (); 		   	

    if (myScreenRect.left + myCurrentLeft + *pmWindowWidth > myScreenRect.right)
    {
    	myCurrentLeft = 0;
    	
    	if (myScreenRect.left + *pmWindowWidth > myScreenRect.right)
    	{
	    *pmWindowWidth = myScreenRect.right - myScreenRect.left;
	}
    }
    
    if (myScreenRect.top + myCurrentTop + *pmWindowHeight > myScreenRect.bottom)
    {
    	myCurrentTop = 0;
    	
    	if (myScreenRect.top + *pmWindowHeight > myScreenRect.bottom)
    	{
	    *pmWindowHeight = myScreenRect.bottom - myScreenRect.top;
	}
    }
    
    *pmLeft = myScreenRect.left + myCurrentLeft;
    *pmTop = myScreenRect.top + myCurrentTop;
    
    myCurrentLeft += GetSystemMetrics (SM_CYCAPTION);    
    myCurrentTop += GetSystemMetrics (SM_CYCAPTION);
} // MyGetWindowDisplacement


/************************************************************************/
/* MyGetWindowSize							*/
/************************************************************************/
static void	MyGetWindowSize (HWND pmEditWindow, int pmRows, int pmCols, 
				 long *pmWidth, long *pmHeight)
{
    RECT		myRect;
    EditWindowInfoPtr	myInfo;
    HWND		myTabBar;

    myInfo = MyGetEditWindowInfo (pmEditWindow);
    myTabBar = myInfo -> tabBarWindow;
    
    myRect.left = 0;
    myRect.top = 0; 	
    EdDisp_GetWindowSize (MyGetTextDisplayWindow (pmEditWindow),
 	pmRows, pmCols, &(myRect.right), &(myRect.bottom));

    if (myInfo -> oneWindowMode)
    {
    	TabCtrl_AdjustRect (myTabBar, TRUE, &myRect);
    	myRect.right += 4 + 2 * stProperties.tabMargin;
    	myRect.bottom += 6 + 2 * stProperties.tabMargin;
    }

    myRect.bottom += EdBBar_GetWindowHeight ();
    myRect.bottom += EdStat_GetWindowHeight ();	    	
    AdjustWindowRect (&myRect, WS_OVERLAPPEDWINDOW, TRUE);
    
    *pmWidth = myRect.right - myRect.left;
    *pmHeight = myRect.bottom - myRect.top;
} // MyGetWindowSize

				 
/************************************************************************/
/* MyMenusAdjust							*/
/*									*/
/* Adjust the entry and enabled/disabled status of any 			*/
/* menu items prior to the user selecting a menu item  			*/
/************************************************************************/
static void	MyMenusAdjust (HWND pmEditWindow, HMENU pmMenuToAdjust)
{
    HMENU	myMenu;
    HWND	myTextDisplayWindow;
    BOOL	mySelectionActive, myEmptyFile, myDirtyFile;
    BOOL	myHasAssociatedFile, myErrorsInFile, myNearBrace;
    BOOL	myIsKnownLanguage;
    BOOL	myProgramRunning, myProgramPaused;
    BOOL	myAreAnyRunWindowsVisible;
    int		myUndoStringResource, myRedoStringResource;
    BOOL	myEnable;
    char	myUndoMenuItemName [80];
    char	myRunMenuItemName [80];
    char	myCompileMenuItemName [80];
    char	myShowHideMenuItemName [80];
    FilePath	myMainPathBuffer;
    char	*myMainFile;
        
    myTextDisplayWindow = MyGetTextDisplayWindow (pmEditWindow);
    	    
    mySelectionActive = EdDisp_IsSelectionActive (myTextDisplayWindow);
    myEmptyFile = EdDisp_IsEmptyFile (myTextDisplayWindow);
    myDirtyFile = EdDisp_IsDirtyFile (myTextDisplayWindow);
    myHasAssociatedFile = EdDisp_IsAssociated (myTextDisplayWindow);
    myErrorsInFile = EdErr_HasErrors ();
    myUndoStringResource = EdDisp_GetUndoStringResource (myTextDisplayWindow);
    myRedoStringResource = EdDisp_GetRedoStringResource (myTextDisplayWindow);
    myMainFile = EdDisp_GetMainFile (myMainPathBuffer);
    myNearBrace = (EdDisp_IsNearBrace (myTextDisplayWindow) != NO_BRACE_FOUND);
    myIsKnownLanguage = EdDisp_IsKnownLanguage (myTextDisplayWindow);
    myProgramRunning = EdRun_IsProgramRunning ();
    myProgramPaused = EdRun_IsProgramPaused () & myProgramRunning;
    myAreAnyRunWindowsVisible = EdRun_IsAnyRunWindowVisible ();
//    myIsDebuggerMenuVisible = EdMenu_IsDebuggerMenuVisible ();

    //
    // File menu
    //

    myMenu = EdMenu_GetMenu (pmEditWindow, FILE_MENU);
    // "New", "Open", "Close", "Recent Files" always on
    // "Save" off if file empty or no changes
    MyMenuItemEnable (myMenu, COMMAND_SAVE_FILE, 
        myDirtyFile && (myHasAssociatedFile || !myEmptyFile));
    // "Save As" off if file empty
    MyMenuItemEnable (myMenu, COMMAND_SAVEAS_FILE, 
        (myHasAssociatedFile || !myEmptyFile));
    // "Save Selection" off if no selection
    MyMenuItemEnable (myMenu, COMMAND_SAVE_SELECTION, mySelectionActive);
    // "Revert" off if no changes, or no associated file
    MyMenuItemEnable (myMenu, COMMAND_REVERT_FILE, 
        myHasAssociatedFile && myDirtyFile);
    // "Print" off if empty
    MyMenuItemEnable (myMenu, COMMAND_PRINT_FILE, !myEmptyFile);
    // "Page Setup", always on
    // "Select as fixed main program" off if file empty
    MyMenuItemEnable (myMenu, COMMAND_FIX_MAIN, !myEmptyFile);
    // "No fixed main program" off if no fixed main program selected
    MyMenuItemEnable (myMenu, COMMAND_FLOAT_MAIN, myMainFile != NULL);
    // "Redraw", "Preferences", "Exit" always on
    
    /*************/
    /* Edit menu */
    /*************/
    
    myMenu = EdMenu_GetMenu (pmEditWindow, EDIT_MENU);
    // "Undo" off if no undoable action.  Also undo menu item name set.
    if (myUndoStringResource == 0)
    {
    	EdGUI_LoadString (IDS_NORMAL_UNDO, myUndoMenuItemName, 
    	    sizeof (myUndoMenuItemName));
        ModifyMenu (myMenu, COMMAND_UNDO, MF_BYCOMMAND | MF_STRING | MF_GRAYED, 
            COMMAND_UNDO, myUndoMenuItemName); 
    }
    else
    {
    	EdGUI_LoadString (myUndoStringResource, myUndoMenuItemName, 
    	    sizeof (myUndoMenuItemName));
        ModifyMenu (myMenu, COMMAND_UNDO, MF_BYCOMMAND | MF_STRING | MF_ENABLED, 
            COMMAND_UNDO, myUndoMenuItemName); 
    }
    // "Redo" off if no undoable action.  Also redo menu item name set.
    if (myRedoStringResource == 0)
    {
    	EdGUI_LoadString (IDS_REDO_BASE, myUndoMenuItemName, 
    	    sizeof (myUndoMenuItemName));
        ModifyMenu (myMenu, COMMAND_REDO, MF_BYCOMMAND | MF_STRING | MF_GRAYED, 
            COMMAND_REDO, myUndoMenuItemName); 
    }
    else
    {
    	EdGUI_LoadString (myRedoStringResource, myUndoMenuItemName, 
    	    sizeof (myUndoMenuItemName));
        ModifyMenu (myMenu, COMMAND_REDO, MF_BYCOMMAND | MF_STRING | MF_ENABLED, 
            COMMAND_REDO, myUndoMenuItemName); 
    }
    // "Cut" off if no selection
    MyMenuItemEnable (myMenu, COMMAND_CUT, mySelectionActive);
    // "Copy" off if no selection
    MyMenuItemEnable (myMenu, COMMAND_COPY, mySelectionActive);
    // "Paste" off if empty paste buffer
    myEnable = IsClipboardFormatAvailable (CF_TEXT);
    MyMenuItemEnable (myMenu, COMMAND_PASTE, myEnable);
    // "Select All" off if file empty
    MyMenuItemEnable (myMenu, COMMAND_SELECT_ALL, !myEmptyFile);
    // "Comment range" off if no selection or if file type not known
    MyMenuItemEnable (myMenu, COMMAND_COMMENT_RANGE, 
    					myIsKnownLanguage && mySelectionActive);
    // "Uncomment range" off if no selection or if file type not known
    MyMenuItemEnable (myMenu, COMMAND_UNCOMMENT_RANGE, 
    					myIsKnownLanguage && mySelectionActive);
    // "Insert" always on
    // "Paragraph (Indent) file" off if file empty or if file type not known
    MyMenuItemEnable (myMenu, COMMAND_INDENT, 
    					myIsKnownLanguage && !myEmptyFile);

    /***************/
    /* Search menu */
    /***************/

    myMenu = EdMenu_GetMenu (pmEditWindow, SEARCH_MENU);
    // "Find" off if file empty
    MyMenuItemEnable (myMenu, COMMAND_FIND, !myEmptyFile);
    // "Find Next" off if no find string or file empty
    myEnable = !EdSearch_IsFindStringEmpty ();
    MyMenuItemEnable (myMenu, COMMAND_FIND_NEXT, myEnable && !myEmptyFile);
    // "Find "selection" follows rules below
    myEnable = MySetFindSelectionMenuItem (pmEditWindow);
    MyMenuItemEnable (myMenu, COMMAND_FIND_SELECTION, myEnable && !myEmptyFile);
    // "Replace always on
    MyMenuItemEnable (myMenu, COMMAND_REPLACE, !myEmptyFile);
    // "Find Next Error" off if no errors
    MyMenuItemEnable (myMenu, COMMAND_FIND_NEXT_ERROR, myErrorsInFile);
    // "Find Matching Brace" off if no brace selected
    MyMenuItemEnable (myMenu, COMMAND_FIND_MATCH_BRACE, myNearBrace);
    // "Goto line" off if file empty
    MyMenuItemEnable (myMenu, COMMAND_GOTO_LINE, !myEmptyFile);

    /************/
    /* Run menu */
    /************/
    
    myMenu = EdMenu_GetMenu (pmEditWindow, RUN_MENU);
    // "Run" off if file empty
    // If there's a main program, change the menu item string
    if (myMainFile == NULL)
    {
    	EdGUI_LoadString (IDS_RUN_MENU_ITEM, myRunMenuItemName, 
    	    sizeof (myRunMenuItemName));
        ModifyMenu (myMenu, COMMAND_RUN, MF_BYCOMMAND | MF_STRING, 
            COMMAND_RUN, myRunMenuItemName); 
        MyMenuItemEnable (myMenu, COMMAND_RUN, !myEmptyFile);
    }
    else
    {
    	char	*myArgList [1], *myActualRunMenuItemName;
    	int	myResult;
    	
    	EdGUI_LoadString (IDS_RUN_MAIN_MENU_ITEM, myRunMenuItemName, 
    	    sizeof (myRunMenuItemName));
	    
    	// Format the message
	myArgList [0] = myMainFile;
    	myResult = FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER | 
            FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ARGUMENT_ARRAY, 
            myRunMenuItemName, 0, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR) &myActualRunMenuItemName, 0, myArgList);
    	if (myResult == 0)	  
    	{
    	    EdFail_Fatal (IDS_FORMATMESSAGEFAIL, __FILE__, __LINE__, 
    		GetLastError (), myRunMenuItemName);
    	    // Never reaches here
    	    return;
    	}

        ModifyMenu (myMenu, COMMAND_RUN, MF_BYCOMMAND | MF_STRING | MF_ENABLED, 
            COMMAND_RUN, myActualRunMenuItemName); 
            
        LocalFree (myActualRunMenuItemName);
    }
    // "Run with Args" off if no main program and file empty
    MyMenuItemEnable (myMenu, COMMAND_RUN_ARGS, 
        (myMainFile != NULL) || !myEmptyFile);

    if (gProgram.isJava)
    {      
        // "Compile" off if file empty
    	// If there's a main program, change the menu item string
    	if (myMainFile == NULL)
    	{
    	    EdGUI_LoadString (IDS_COMPILE_MENU_ITEM, myCompileMenuItemName, 
    	    	sizeof (myCompileMenuItemName));
            ModifyMenu (myMenu, COMMAND_COMPILE, MF_BYCOMMAND | MF_STRING, 
            	COMMAND_COMPILE, myCompileMenuItemName); 
            MyMenuItemEnable (myMenu, COMMAND_COMPILE, !myEmptyFile);
    	}
    	else
    	{
    	    char	*myArgList [1], *myActualCompileMenuItemName;
    	    int		myResult;
    	
    	    EdGUI_LoadString (IDS_COMPILE_MAIN_MENU_ITEM, myCompileMenuItemName, 
    	    	sizeof (myCompileMenuItemName));
	    
    	    // Format the message
	    myArgList [0] = myMainFile;
    	    myResult = FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ARGUMENT_ARRAY, 
                myCompileMenuItemName, 0, 
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPTSTR) &myActualCompileMenuItemName, 0, myArgList);
    	    if (myResult == 0)	  
    	    {
    	    	EdFail_Fatal (IDS_FORMATMESSAGEFAIL, __FILE__, __LINE__, 
    		    GetLastError (), myCompileMenuItemName);
    	    	// Never reaches here
    	    	return;
    	    }    

            ModifyMenu (myMenu, COMMAND_COMPILE, 
            	MF_BYCOMMAND | MF_STRING | MF_ENABLED, 
            	COMMAND_COMPILE, myActualCompileMenuItemName); 
            
            LocalFree (myActualCompileMenuItemName);
        }
    } // if (gProgram.isJava)

    // "Stop" on if Java program running
    MyMenuItemEnable (myMenu, COMMAND_STOP, myProgramRunning);
    // "Pause" on if Java program running
    if (myProgramPaused)
    {
    	char	myMenuItemName [256];
    	
    	EdGUI_LoadString (IDS_RESUME_BUTTON, myMenuItemName, 
    	    sizeof (myMenuItemName));
        ModifyMenu (myMenu, COMMAND_PAUSERESUME, 
            MF_BYCOMMAND | MF_STRING | MF_ENABLED, COMMAND_PAUSERESUME, 
            myMenuItemName); 
    }
    else
    {
    	char	myMenuItemName [256];
    	
    	EdGUI_LoadString (IDS_PAUSE_BUTTON, myMenuItemName, 
    	    sizeof (myMenuItemName));
        ModifyMenu (myMenu, COMMAND_PAUSERESUME, 
            MF_BYCOMMAND | MF_STRING | MF_ENABLED, COMMAND_PAUSERESUME, 
            myMenuItemName); 
    }
    MyMenuItemEnable (myMenu, COMMAND_PAUSERESUME, myProgramRunning);

    if (gProgram.isJava)
    {      
    	// "Submit" off if no main program and file empty
    	MyMenuItemEnable (myMenu, COMMAND_SUBMIT, 
            (myMainFile != NULL) || !myEmptyFile);
    }
    else if (gProgram.isTuring)
    {
    	// "Generate stand-alone program" off if file empty
    	MyMenuItemEnable (myMenu, COMMAND_CREATE_EXE, !myEmptyFile);
	// "Close Run Windows" off if no visible run windows
    	MyMenuItemEnable (myMenu, COMMAND_CLOSE_ALL_RUN_WIN, 
	    myAreAnyRunWindowsVisible);
    }    	

    /*****************/
    /* Debugger menu */
    /*****************/
    if (EdMenu_IsDebuggerMenuVisible ())
    {
	myMenu = EdMenu_GetMenu (pmEditWindow, DEBUGGER_MENU);
	
	// Show/Hide Debugger Controls
	if (EdBBar_AreDebuggerControlsVisible ())
	{
	    EdGUI_LoadString (IDS_HIDE_DEBUGGER_CONTROLS, 
		myShowHideMenuItemName, sizeof (myShowHideMenuItemName));
	}
	else
	{
	    EdGUI_LoadString (IDS_SHOW_DEBUGGER_CONTROLS, 
		myShowHideMenuItemName, sizeof (myShowHideMenuItemName));
	}    	    
	ModifyMenu (myMenu, COMMAND_DEBUG_SHOW_HIDE_CONTROLS, 
	    MF_BYCOMMAND | MF_STRING, COMMAND_DEBUG_SHOW_HIDE_CONTROLS,
	    myShowHideMenuItemName);
	    
	// Show/Hide Breakpoints	    
	if (EdDisp_AreBreakpointsVisible ())
	{
	    EdGUI_LoadString (IDS_HIDE_BREAKPOINTS, 
		myShowHideMenuItemName, sizeof (myShowHideMenuItemName));
	}
	else
	{
	    EdGUI_LoadString (IDS_SHOW_BREAKPOINTS, 
		myShowHideMenuItemName, sizeof (myShowHideMenuItemName));
	}    	    
	ModifyMenu (myMenu, COMMAND_DEBUG_SHOW_BREAKPOINTS, 
	    MF_BYCOMMAND | MF_STRING, COMMAND_DEBUG_SHOW_BREAKPOINTS,
	    myShowHideMenuItemName);
	    
	// Show/Hide Breakpoints	    
	if (DbgAlloc_IsWindowVisible ())
	{
	    EdGUI_LoadString (IDS_HIDE_DBG_ALLOC, 
		myShowHideMenuItemName, sizeof (myShowHideMenuItemName));
	}
	else
	{
	    EdGUI_LoadString (IDS_SHOW_DBG_ALLOC, 
		myShowHideMenuItemName, sizeof (myShowHideMenuItemName));
	}    	    
	ModifyMenu (myMenu, COMMAND_DEBUG_SHOW_ALLOCATED_OBJECTS, 
	    MF_BYCOMMAND | MF_STRING, COMMAND_DEBUG_SHOW_ALLOCATED_OBJECTS,
	    myShowHideMenuItemName);
	    
	// Temporarily disable unavailable items
    	MyMenuItemEnable (myMenu, COMMAND_DEBUG_SHOW_UNIT_LIST, FALSE);
    	MyMenuItemEnable (myMenu, COMMAND_DEBUG_SHOW_PROCESSES, FALSE);
    	MyMenuItemEnable (myMenu, COMMAND_DEBUG_SHOW_QUEUES, FALSE);
    	MyMenuItemEnable (myMenu, COMMAND_DEBUG_SHOW_EXECUTION_LINE, FALSE);
    	MyMenuItemEnable (myMenu, COMMAND_DEBUG_SHOW_DEFINITION, FALSE);

	// Checkmark "Stream 0 to Stderr" menu item if selected
    	MyMenuItemEnable (myMenu, COMMAND_DEBUG_STREAM_0_TO_STDERR, TRUE);
	if (gProperties.stream0ToStderr)
	{
	    // Add checkmark to COMMAND_DEBUG_STREAM_0_TO_STDERR
	    CheckMenuItem (myMenu, COMMAND_DEBUG_STREAM_0_TO_STDERR, 
		MF_CHECKED);
	}
	else
	{
	    // Add checkmark to COMMAND_DEBUG_STREAM_0_TO_STDERR
	    CheckMenuItem (myMenu, COMMAND_DEBUG_STREAM_0_TO_STDERR,
		MF_UNCHECKED);
	}

	// Enable COMMAND_DEBUG_ERRORS_TO_STDERR
	MyMenuItemEnable (myMenu, COMMAND_DEBUG_ERRORS_TO_STDERR, TRUE);
	if (gProperties.errorsToStderr)
	{
	    // Add checkmark to COMMAND_DEBUG_ERRORS_TO_STDERR
	    CheckMenuItem (myMenu, COMMAND_DEBUG_ERRORS_TO_STDERR,
		MF_CHECKED);
	}
	else
	{
	    // Remove checkmark to COMMAND_DEBUG_ERRORS_TO_STDERR
	    CheckMenuItem (myMenu, COMMAND_DEBUG_ERRORS_TO_STDERR,
		MF_UNCHECKED);
	}
	
	// Enable COMMAND_DEBUG_LIB_INFO_TO_STDERR
	MyMenuItemEnable (myMenu, COMMAND_DEBUG_LIB_INFO_TO_STDERR, TRUE);
	if (gProperties.infoToStderr)
	{
	    // Add checkmark to COMMAND_DEBUG_LIB_INFO_TO_STDERR
	    CheckMenuItem (myMenu, COMMAND_DEBUG_LIB_INFO_TO_STDERR,
		MF_CHECKED);
	}
	else
	{
	    // Remove checkmark to COMMAND_DEBUG_LIB_INFO_TO_STDERR
	    CheckMenuItem (myMenu, COMMAND_DEBUG_LIB_INFO_TO_STDERR,
		MF_UNCHECKED);
	}
    }

    /**************/
    /* Help menu  */
    /**************/

    myMenu = EdMenu_GetMenu (pmEditWindow, HELP_MENU);
    MyMenuItemEnable (myMenu, COMMAND_KEYWORD_PROTO, !myEmptyFile);
    MyMenuItemEnable (myMenu, COMMAND_KEYWORD_REF, !myEmptyFile);
} // MyMenusAdjust


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
/* MyResizeEditWindow							*/
/*									*/
/* pmAlways is true when the window must be resized, even if it hasn't	*/
/* actually been resized.  This occurs when the debugging controls are	*/
/* being shown/hidden.							*/
/************************************************************************/
static void	MyResizeEditWindow (HWND pmEditWindow, BOOL pmAlways)
{
    RECT		myWindowRect;
    EditWindowInfoPtr	myInfo;
    int			myWindowWidth;
    int			myTopFrameBorder, myBottomFrameBorder;
    int			myStatusWindowHeight, myTextDisplayWindowHeight;
    HDWP		myDeferWindowPosHandle;

    // The first resizing comes before there's a client area, so
    // we put this check here to ignore it
    if (GetClientRect (pmEditWindow, &myWindowRect) == 0)
    {
    	return;
    }
    
    // Get the window infor    
    myInfo = MyGetEditWindowInfo (pmEditWindow);
    
    // If the window hasn't really been resized, return immediately 
    if (!pmAlways && (myWindowRect.bottom == myInfo -> oldWindowRect.bottom) && 
        (myWindowRect.right == myInfo -> oldWindowRect.right) &&
        (myWindowRect.top == myInfo -> oldWindowRect.top) &&
        (myWindowRect.left == myInfo -> oldWindowRect.left))
    {
    	return;
    }
    
    // If we've been iconisized, ignore the resizing
    if (!pmAlways & IsIconic (pmEditWindow))
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
    myTopFrameBorder = EdBBar_GetWindowHeight ();
    DeferWindowPos (myDeferWindowPosHandle, myInfo -> buttonBarWindow, HWND_TOP, 
        0, 0, myWindowRect.right, myTopFrameBorder, SWP_NOACTIVATE);

    //
    // Resize the status window
    //
    myStatusWindowHeight = EdStat_GetWindowHeight ();
    myBottomFrameBorder = myWindowRect.bottom - myStatusWindowHeight;
    DeferWindowPos (myDeferWindowPosHandle, myInfo -> statusWindow, HWND_TOP, 
        0, myBottomFrameBorder, myWindowRect.right, myStatusWindowHeight, 
        SWP_NOACTIVATE);

    myTextDisplayWindowHeight = myBottomFrameBorder - myTopFrameBorder;
    if (myInfo -> oneWindowMode)
    {
    	RECT	myTabDisplayArea;
    	int	myWidth, myHeight;
    	
    	// 
    	// Resize the tab bar and its contents
    	// 
        DeferWindowPos (myDeferWindowPosHandle, myInfo -> tabBarWindow, 
            HWND_TOP, 0, myTopFrameBorder, myWindowRect.right, 
            myTextDisplayWindowHeight, SWP_NOACTIVATE);
        
        // Calculate the display area of the tab bar that the sunken frame
        // and the text display area will be placed in
        myTabDisplayArea.left = 0;
        myTabDisplayArea.top = myTopFrameBorder;
        myTabDisplayArea.right = myWindowRect.right;
        myTabDisplayArea.bottom = myTopFrameBorder + myTextDisplayWindowHeight;
    	TabCtrl_AdjustRect(myInfo -> tabBarWindow, FALSE, &myTabDisplayArea);
    	 
    	// Resize the sunken frame 
    	myWidth = myTabDisplayArea.right - myTabDisplayArea.left;
    	myHeight = myTabDisplayArea.bottom - myTabDisplayArea.top;
    	
    	DeferWindowPos (myDeferWindowPosHandle, 
    	    myInfo -> sunkenWindow, HWND_TOP, 
    	    myTabDisplayArea.left + stProperties.tabMargin, 
            myTabDisplayArea.top + 2 + stProperties.tabMargin,
            myWidth - 2 * stProperties.tabMargin, 
            myHeight - 2 - 2 * stProperties.tabMargin, 
            SWP_NOACTIVATE);
            
        // Resize the text display     
//    	DeferWindowPos (myDeferWindowPosHandle, 
//   	    myInfo -> textDisplayWindow [myInfo -> activeTextDisplayWindow], 
//            HWND_TOP, myTabDisplayArea.left + stProperties.tabMargin + 2, 
//            myTabDisplayArea.top + 4 + stProperties.tabMargin,
//            myWidth - 4 - 2 * stProperties.tabMargin, 
//            myHeight - 6 - 2 * stProperties.tabMargin, 
//            SWP_NOACTIVATE);
    	DeferWindowPos (myDeferWindowPosHandle, 
   	    myInfo -> mdiClientWindow, 
            HWND_TOP, myTabDisplayArea.left + stProperties.tabMargin + 2, 
            myTabDisplayArea.top + 4 + stProperties.tabMargin,
            myWidth - 4 - 2 * stProperties.tabMargin, 
            myHeight - 6 - 2 * stProperties.tabMargin, 
            SWP_NOACTIVATE);
    }
    else
    {    
        //
    	// Resize the text display window
    	//
    	DeferWindowPos (myDeferWindowPosHandle, 
    	    myInfo -> textDisplayWindow [myInfo -> activeTextDisplayWindow], 
            HWND_TOP, 0, myTopFrameBorder, myWindowRect.right, 
            myTextDisplayWindowHeight, SWP_NOACTIVATE);
    }
        
    if (!EndDeferWindowPos (myDeferWindowPosHandle))
    {
        EdFail_Fatal (IDS_NO_RESIZE, __FILE__, __LINE__, GetLastError ());
    }
} // MyResizeEditWindow


/************************************************************************/
/* MySetFindSelectionMenuItem						*/
/************************************************************************/
static BOOL	MySetFindSelectionMenuItem (HWND pmEditWindow)
{
    BOOL		myResult;
    char		myFindSelectionString [80];
    HMENU		mySearchMenu;
    MENUITEMINFO	myInfo;
    
    myResult = EdDisp_SetFindSelectionMenuItem (
        MyGetTextDisplayWindow (pmEditWindow), myFindSelectionString);

    mySearchMenu = EdMenu_GetMenu (pmEditWindow, SEARCH_MENU);
    
    myInfo.cbSize = sizeof (MENUITEMINFO);
    myInfo.fMask = MIIM_TYPE;
    myInfo.fType = MFT_STRING;
    myInfo.dwTypeData = myFindSelectionString;
    SetMenuItemInfo (mySearchMenu, FIND_SELECTION_POSITION, TRUE, &myInfo);
    
    return myResult;
} // MySetFindSelectionMenuItem


/************************************************************************/
/* MySetTabFont								*/
/************************************************************************/
static BOOL	MySetTabFont (HWND pmTabBarWindow)
{
    char		myFontName [256];
    
    // Get the font name
    EdGUI_LoadString (IDS_DIALOG_FONT_NAME, myFontName, sizeof (myFontName));

    // Delete any previously existing font
    if (stTabFont != NULL)
    {
    	DeleteObject (stTabFont);
    }
    
    // Create the new font
    stTabFont = EdGUI_CreateFont (pmTabBarWindow, NULL, myFontName,
        stProperties.tabFontSize, NO_BOLD, NO_ITALIC);
    if (stTabFont == NULL)
    {
    	EdFail_Warn (IDS_CREATEFONTFAIL, __FILE__, __LINE__, 
    			  GetLastError ());
    	return FALSE;
    }

    return TRUE;
} // MySetTabFont
