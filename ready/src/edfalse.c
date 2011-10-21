/************************************************************************/
/* edfalse.c								*/
/*									*/
/* This module is used to handle the "false" window. The "false" window	*/
/* never actually appears.  Instead, it is used when MS Windows 	*/
/* requires a window (usually to process messages), and there is no	*/
/* guarantee that any given window is still open.  For example, to	*/
/* open an HTML Help window, a window must be specified that is used to	*/
/* catch the WM_TCARD messages from the HTML Help engine that are used	*/
/* to open example files. If the user has closed this window and then	*/
/* clicks a button to open an example, nothing happens because the 	*/
/* WM_TCARD is sent to a (now) non-existent window.			*/
/*									*/
/* This module creates a hidden window that the messages to which the 	*/
/* windows can be sent.							*/
/************************************************************************/
/*******************/
/* System includes */
/*******************/
/*#include <windows.h>
#include <commctrl.h>
#include <crtdbg.h>
#include <htmlhelp.h>*/

/****************/
/* Self include */
/****************/
#include "edfalse.h"

/******************/
/* Other includes */
/******************/
#include "ed.h"
#include "edfail.h"
#include "edgui.h"
#include "edprog.h"
#include "edwin.h"

#include "eddialogs.h"

/*#include "ed.h"
#include "edbbar.h"
#include "eddisp.h"
#include "ederr.h"
#include "edfail.h"
#include "edfile.h"
#include "edprop.h"
#include "edrec.h"
#include "edrun.h"
#include "edstat.h"

#include "dbgalloc.h"

#include "edcommands.h"*/

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
static LRESULT CALLBACK	MyFalseWindowProcedure (HWND pmWindow, UINT pmMessage, 
						WPARAM pmWParam, 
						LPARAM pmLParam);

/*********************/
/* Static procedures */
/*********************/

/***********************/
/* External procedures */
/***********************/
/************************************************************************/
/* EdFalse_InitWindow							*/
/* 									*/
/* Initialize the modules under EdWin and register the new class	*/
/************************************************************************/
HWND	EdFalse_InitWindow (void)
{
    char	myFalseWindowClassName [256];
    WNDCLASSEX	myWindowClass;
    int		myResult;
    HWND	myWindow;
    
    // Get the class name
    EdGUI_LoadString (IDS_FALSE_WINDOW_NAME, myFalseWindowClassName, 
		      sizeof (myFalseWindowClassName));
		     
    //
    // Register the false window class
    //
    myWindowClass.cbSize = 	  sizeof (myWindowClass);
    // Set window class to redraw when window size changes
    myWindowClass.style = 	  CS_HREDRAW | CS_VREDRAW;
    // Procedure to be called with messages for this window class
    myWindowClass.lpfnWndProc =   MyFalseWindowProcedure;
    // The extra space in class struct
    myWindowClass.cbClsExtra = 	  0;
    // The extra space in window struct
    myWindowClass.cbWndExtra = 	  0;
    // The application's handle
    myWindowClass.hInstance = 	  gProgram.applicationInstance;
    // Set the icon for this window class
    myWindowClass.hIcon = 	  NULL;
    // Set the cursor for this window class
    myWindowClass.hCursor = 	  LoadCursor (NULL, IDC_ARROW);
    // Set the background colour for this window
    myWindowClass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH);
    // Set the menu for this window class
    myWindowClass.lpszMenuName =  NULL;
    // Name of the window class
    myWindowClass.lpszClassName = myFalseWindowClassName; 
    // Set the icon for this class.
    myWindowClass.hIconSm = 	  NULL;
    
    myResult = RegisterClassEx (&myWindowClass);
    if (myResult == 0)
    {
    	EdFail_Warn (IDS_REGISTERCLASSFAIL, __FILE__, __LINE__, 
    			  GetLastError ());
    	return NULL;
    }

    //
    // Create one window
    //
    myWindow = CreateWindow (myFalseWindowClassName,
    	    		     "False Window", 
    	    		     WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
    			     CW_USEDEFAULT, 		// Initial x location
    			     CW_USEDEFAULT, 		// Initial y location
    			     CW_USEDEFAULT, 		// Initial x size
    			     CW_USEDEFAULT, 		// Initial y size
		    	     HWND_DESKTOP, 		// Parent window handle
    			     NULL, 			// Window menu handle
    			     gProgram.applicationInstance, // Program instance
    	    		     NULL);
    if (myWindow == NULL)
    {
	EdFail_Warn (IDS_CREATEWINDOWFAIL, __FILE__, __LINE__, 
	        GetLastError ());
        return NULL;
    }
        		     
    return myWindow;
} // EdFalse_Init


/******************************/
/* Static callback procedures */
/******************************/
/************************************************************************/
/* MyFalseWindowProcedure						*/
/*									*/
/* Callback routine from the false window. 				*/
/************************************************************************/
static LRESULT CALLBACK	MyFalseWindowProcedure (HWND pmWindow, UINT pmMessage, 
						WPARAM pmWParam, LPARAM pmLParam)
{
    switch (pmMessage)
    {
    	case WM_TCARD:
	    EdFail_AddMessage (NORMAL, "CMD: WM_TCARD");
	    EdWin_OpenExample (Ed_GetLastActiveEditWindow (), 
	    		       (int) pmWParam, (const char *) pmLParam);
	    return 0;
    } // switch
            	
    return DefWindowProc (pmWindow, pmMessage, pmWParam, pmLParam);
} // MySunkenFrameWindowProcedure


