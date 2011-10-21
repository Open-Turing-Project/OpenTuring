/*************/
/* mdiodlg.c */
/*************/

/*******************/
/* System includes */
/*******************/
#include <string.h>

/****************/
/* Self include */
/****************/
#include "mdiodlg.h"

/******************/
/* Other includes */
/******************/
//#include "miowin.h"
#include "mio.h"
#include "mdio.h"
//#include "mdiowin.h"

#include "mioerr.h"

#include "edint.h"

// Test to make certain we're not accidentally including <windows.h> which
// might allow for windows contamination of platform independent code.
#ifdef _INC_WINDOWS
xxx
#endif

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

/******************************/
/* Static callback procedures */
/******************************/

/*********************/
/* Static procedures */
/*********************/

/***********************/
/* External procedures */
/***********************/
/************************************************************************/
/* MDIODlg_Init								*/
/************************************************************************/
void	MDIODlg_Init (const char *pmTitle, const char *pmMsg)
{
/*    char	myAlertDlgWindowClassName [256];
    int		myResult;
    WNDCLASSEX	myWindowClass;

    //
    // Initialize the palette
    //
    if (!MyCreateDefaultPalette ())
    {
    	return FALSE;
    }

    // Get the class name
    EdInt_LoadString (IDS_MIODLG_ALERT_WINDOW_NAME, myAlertDlgWindowClassName, 
	sizeof (myAlertDlgWindowClassName));

    //
    // Register the console window class
    //
    myWindowClass.cbSize = 	  sizeof (myWindowClass);
    // Set window class to redraw when window size changes
    myWindowClass.style = 	  CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    // Procedure to be called with messages for this window class
    myWindowClass.lpfnWndProc =   MyAlertDlgWindowProcedure;
    // The extra space in class struct
    myWindowClass.cbClsExtra = 	  0;
    // The extra space in window struct for the pointer to text data
    // and editor window
    myWindowClass.cbWndExtra = 	  WINDOW_EXTRA_MEMORY;
    // The application's handle
    myWindowClass.hInstance = 	  MIO_applicationInstance;
    // Set the icon for this window class
    myWindowClass.hIcon = 	  LoadIcon (MIO_applicationInstance, 
    					MAKEINTRESOURCE (MIO_RUN_ICON));
    // Set the cursor for this window class
    myWindowClass.hCursor = 	  LoadCursor (NULL, IDC_ARROW);
    // Set the background colour for this window
    myWindowClass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH);
    // Set the menu for this window class
    myWindowClass.lpszMenuName =  NULL;
    // Name of the window class
    myWindowClass.lpszClassName = myAlertDlgWindowClassName; 
    // Set the icon for this class.
    myWindowClass.hIconSm = 	  LoadIcon (MIO_applicationInstance, 
    					MAKEINTRESOURCE (MIO_RUN_SMALL_ICON));
    
    myResult = RegisterClassEx (&myWindowClass);
    if (myResult == 0)
    {
    	EdInt_FailWarn (IDS_MIO_REGISTERCLASSFAIL, __FILE__, __LINE__, 
    			GetLastError ());
	return FALSE;    			  
    }
    return TRUE;
*/    
} // MDIODlg_Init


/************************************************************************/
/* MDIODlg_Alert							*/
/************************************************************************/
void	MDIODlg_Alert (const char *pmTitle, const char *pmMsg)
{
/*    char		myRunWindowClassName [256];
    HWND		myRunWindow;
    MIOWinInfoPtr	myInfo;
    HPEN		myPen;
    HBRUSH		myBrush;
        
    // Load the class name (we've already loaded it once, so no error checking)    
    EdInt_LoadString (IDS_MIODLG_ALERT_WINDOW_NAME,
	myRunWindowClassName, sizeof (myRunWindowClassName));

    // Create the editor window    
    myRunWindow = CreateWindow (
    	myRunWindowClassName, 			// Window class
        "Run Windows",		 		// Window title
        WS_EX_DLGMODALFRAME, WS_EX_WINDOWEDGE, 	// Window style
    	CW_USEDEFAULT, 				// Initial x location
    	CW_USEDEFAULT, 				// Initial y location
    	CW_USEDEFAULT, 				// Initial x size
    	CW_USEDEFAULT, 				// Initial y size
    	HWND_DESKTOP, 				// Parent window handle
    	NULL, 					// Window menu handle
    	MIO_applicationInstance,		// Program instance handle
    	NULL);					// Creation parameters
    
    if (MIOWin_GetProperties () -> logWindowMessages)
    {
    	// EdLog_Log ("Run Window = %x  (Finished Creation)", myRunWindow);
    }

    // If we failed to create the window, return immediately
    if (myRunWindow == NULL)
    {
	EdInt_HaltEnvironment (IDS_MIO_CREATEWINDOWFAIL, __FILE__, __LINE__, 
		               GetLastError ());
    }
    
    MIOWin_SetRunWindowTitle (myRunWindow);

    //
    // Clear the screen in colour 0
    //
    myInfo = MIOWin_GetInfo (myRunWindow);
    myPen = CreatePen (0, 1, PALETTEINDEX (0));
    myBrush = CreateSolidBrush (PALETTEINDEX (0));
    SelectBrush (myInfo -> offscreenDeviceContext, myPen);
    SelectBrush (myInfo -> offscreenDeviceContext, myBrush);
    Rectangle (myInfo -> offscreenDeviceContext, 0, 0, myInfo -> width,
	       myInfo -> height);
    DeleteObject (myPen);
    DeleteObject (myBrush);
    
    return myRunWindow;
    // Create the window.
    // Show it
    // Don't return u
*/
} // MDIODlg_Alert


/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/
/************************************************************************/
/* MIODlg_InitializeWindow						*/
/************************************************************************/
int	MIODlg_InitializeWindow (WIND pmWindow, 
				 WindowAttribPtr pmWindowAttribs)
{
/*
    MIOWinInfoPtr	myInfo;
    BOOL		myResult;
    
    //
    // Initialize the extra window memory
    //
	
    // Set the window type
    if (pmWindowAttribs -> mode == TEXT_MODE)
    {
        MDIOWin_SetWindowType (pmWindow, WINDOW_TYPE_MIO_TEXT);
    }
    else
    {
        MDIOWin_SetWindowType (pmWindow, WINDOW_TYPE_MIO_GRAPHICS);
    }
    
    // Set the window initialized                
    MDIOWin_SetWindowInitialization (pmWindow, FALSE);
            
    // If the font properties have not already been determined, 
    // determine them now (we needed a window to do so).  This must be
    // called *after* the top widow and status window created and
    // before the window info.
    MIOWin_PropertiesImplementChanges (pmWindow);
        	    
    // Create the edit window information
    // Allocate the structure for the information about the edit window
    myInfo = (MIOWinInfoPtr) malloc (sizeof (MIOWindowInfo));
    
    if (myInfo == NULL)
    {
        EdInt_AddFailMessage ("MIOWin_InitializeWindow: No room for info");

	return -1;
    }

    memset (myInfo, 0, sizeof (MIOWindowInfo));
    
    myInfo -> windowKind = pmWindowAttribs -> mode;
    
    myInfo -> initXPos = pmWindowAttribs -> xPosition;
    myInfo -> initYPos = pmWindowAttribs -> yPosition;
    myInfo -> initRows = pmWindowAttribs -> rows;
    myInfo -> initColumns = pmWindowAttribs -> columns;
    myInfo -> initXSize = pmWindowAttribs -> xSize;
    myInfo -> initYSize = pmWindowAttribs -> ySize;
    
    myInfo -> fontCharWidth = stTextFontCharWidth;
    myInfo -> fontCharHeight = stTextFontCharHeight;
    myInfo -> fontCharAscent = stTextFontCharAscent;
    myInfo -> fontCharDescent = stTextFontCharDescent;
    myInfo -> fontCharIntLeading = stTextFontCharIntLeading;
    myInfo -> font = stNormalFont;
    myInfo -> selectionColour = stSelectionColour;

    myInfo -> lineBufferLastNewLine = -1;
    myInfo -> ungetChar = EOF;
        
    myInfo -> textRect.top = -1000;
    myInfo -> textRect.bottom = -1000;
    myInfo -> textRect.left = -1000;
    myInfo -> textRect.right = -1000;

	
    myInfo -> acceptingInput = FALSE;
    myInfo -> waitingForInputMessageDisplayed = FALSE;

    myInfo -> cursor = pmWindowAttribs -> cursor;
    myInfo -> echo = pmWindowAttribs -> echo;
    myInfo -> visibility = pmWindowAttribs -> visibility;
    myInfo -> displayOnScreen = pmWindowAttribs -> displayOnScreen;
    myInfo ->msdosCharSet = pmWindowAttribs -> msdosCharSet;
    myInfo -> zOrder = pmWindowAttribs -> topMost;

    MDIOWin_SetWindowInfo (pmWindow, myInfo);
    
    //
    // Add the elements to the window
    //

    // Add the button bar window
    EdInt_AddFailMessage ("MIOWin_InitializeWindow: Create button bar");
    if (pmWindowAttribs -> buttonBar == 1)
    {
	myInfo -> buttonBarWindow = MDIOWinTop_Create (pmWindow, 
    				(pmWindowAttribs -> mode == TEXT_MODE));
    
	if (myInfo -> buttonBarWindow == NULL)
	{
            EdInt_AddFailMessage ("MIOWin_InitializeWindow: No button bar");
            
    	    return -1;
    	}
    }

    // Specify the outer window
    myInfo -> outerWindow = pmWindow;
    
    // Initialize info fields
    strcpy (myInfo -> title, pmWindowAttribs -> title);
    MDIOWin_SetWindowTitle (pmWindow, myInfo -> title);
//EdLog_Log ("Run Window created: hwnd=%x title='%s'", pmWindow, myInfo -> title);

    // Set the window to initialized
    MDIOWin_SetWindowInitialization (pmWindow, TRUE);
    
    // Calculate the window size
    MIOWin_CalculateWindowSize (pmWindow, pmWindowAttribs);

    if (pmWindowAttribs -> mode == TEXT_MODE)
    {
    	// Add the console text window
    	EdInt_AddFailMessage ("MIOWin_InitializeWindow: Create inner text win");
    	myInfo -> innerWindow = MDIOWinText_CreateInnerWindow (pmWindow);
	myResult = MDIOWinText_InitializeWindow (pmWindow);
    }
    else
    {
    	// Add the console text window
    	EdInt_AddFailMessage ("MIOWin_InitializeWindow: Create inner grph win");
    	myInfo -> innerWindow = MDIOWinGraph_CreateInnerWindow (pmWindow);
	myResult = MDIOWinGraph_InitializeWindow (pmWindow);
    }
    
    if (myInfo -> innerWindow == NULL)
    {
        EdInt_AddFailMessage ("MIOWin_InitializeWindow: No inner window");
            
    	return -1;
    }

    if (!myResult)
    {
        EdInt_AddFailMessage (
            "MIOWin_InitializeWindow: MDIOWinGraph_InitializeWindow failed");
            
    	return -1;
    }

    // Calculate the top, left corner of the window
    MIOWin_CalculateWindowDisplacement (pmWindow, pmWindowAttribs);

    // Set the window to the correct position
    MDIOWin_SetWindowPosition (pmWindow, myInfo -> initWindowLeft, 
        myInfo -> initWindowTop, myInfo -> maxWindowWidth, 
        myInfo -> maxWindowHeight);

    // Hide the scroll bars
    if (pmWindowAttribs -> mode == TEXT_MODE)
    {
    	MDIOWinText_SynchScrollBarsToText (myInfo -> innerWindow);
    }
    else
    {
    	MDIOWinGraph_SynchScrollBarsToGraphics (myInfo -> innerWindow);
    }
    							
    
    // Add the window to the list of MIO windows
    MIO_AddToRunWindowList (pmWindow);

    // Get a Turing window ID for this window
//    myInfo -> turingWindowID = MIO_StreamAdd (WINDOW_STREAM, myInfo);

    EdInt_AddFailMessage ("MIOWin_InitializeWindow: Window initialized");
*/    
    return 0;
} // MIODlg_InitializeWindow
