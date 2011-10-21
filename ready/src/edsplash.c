/************************************************************************/
/* edsplash.c								*/
/*									*/
/* Here are the possible versions					*/
/* _OPEN_JAVA_ 			[Java with no restrictions]		*/
/*		Created by Holt Software Associates Inc.		*/
/*		Lead Programmer: Tom West				*/
/*		Version x.xx						*/
/*		Ready uses IBM's Jikes Java Compiler			*/
/*		Java Trademark Info					*/
/* _OPEN_TURING_		[Turing with no restrictions]		*/
/*		Created by Holt Software Associates Inc.		*/
/*		Version x.xx						*/
/* _PERSONAL_JAVA_		[Java for single users]			*/
/*		Created by Holt Software Associates Inc.		*/
/*		Lead Programmer: Tom West				*/
/*		Personal Version x.xx					*/
/*		Licensed for Individual Use By xxx			*/
/*		Ready uses IBM's Jikes Java Compiler			*/
/*		Java Trademark Info					*/
/* _PERSONAL_TURING_		[Turing for single users]		*/
/*		Created by Holt Software Associates Inc.		*/
/*		Personal Version x.xx					*/
/*		Licensed for Individual Use By xxx			*/
/* _SITE_JAVA_			[Java for sites]			*/
/*		Created by Holt Software Associates Inc.		*/
/*		Lead Programmer: Tom West				*/
/*		Site Version x.xx					*/
/*		Licensed to xxx						*/
/*		Ready uses IBM's Jikes Java Compiler			*/
/*		Java Trademark Info					*/
/* _SITE_TURING_		[Turing for sites]			*/
/*		Created by Holt Software Associates Inc.		*/
/*		Site Version x.xx					*/
/*		Licensed to xxx						*/
/* _REDISTRIB_JAVA_		[Java for student redistribution]	*/
/*		Created by Holt Software Associates Inc.		*/
/*		Lead Programmer: Tom West				*/
/*		Redistributable Version x.xx				*/
/*		Licensed for redistribution to students of xxx		*/
/*		Ready uses IBM's Jikes Java Compiler			*/
/*		Java Trademark Info					*/
/* _REDISTRIB_TURING_		[Turing for student redistribution]	*/
/*		Created by Holt Software Associates Inc.		*/
/*		Redistributable Version x.xx				*/
/*		Licensed for redistribution to students of xxx		*/
/* _MINI_JAVA_			[Mini Ready]				*/
/*		Created by Holt Software Associates Inc.		*/
/*		Lead Programmer: Tom West				*/
/*		Mini Version x.xx					*/
/*		Licensed to xxx						*/
/*		This version of the software cannot compile programs of greater than 100 lines in length.	*/
/*		Ready uses IBM's Jikes Java Compiler			*/
/*		Java Trademark Info					*/
/* _MINI_TURING_		[MiniTuring]				*/
/*		Created by Holt Software Associates Inc.		*/
/*		Mini Version x.xx					*/
/*		Licensed to xxx						*/
/*		This version of the software cannot compile programs of greater than 100 lines in length.	*/
/* _APEVAL_			[Personal evaluation copy of Ready for AP]	*/
/*		Created by Holt Software Associates Inc.		*/
/*		Lead Programmer: Tom West				*/
/*		Evaluation Version x.xx					*/
/*		This version is for personal evaluation purposes only.	*/
/*		It may not be redistributed or used for teaching purposes.  	*/
/*		See the "Help" menu for information on obtaining Ready.	*/
/*		This version is time limited and will not execute Java programs after xxx */
/*		Ready uses IBM's Jikes Java Compiler			*/
/*		Java Trademark Info					*/
/* _DEMO_JAVA_			[Demo copy of Java]			*/
/*		Created by Holt Software Associates Inc.		*/
/*		Lead Programmer: Tom West				*/
/*		Evaluation Version x.xx					*/
/*		This version is for personal evaluation purposes only.	*/
/*		It may not be redistributed or used for teaching purposes.  	*/
/*		See the "Help" menu for information on obtaining Ready.	*/
/*		This demonstration version of the software is restricted and	*/
/*		   (1) Cannot save programs,				*/
/*		   (2) Cannot compile programs of greater than 100 lines in length, and	*/
/*		   (3) Will not execute programs after xxx		*/
/*		Ready uses IBM's Jikes Java Compiler			*/
/*		Java Trademark Info					*/
/* _DEMO_TURING_		[Demo copy of Turing]			*/
/*		Created by Holt Software Associates Inc.		*/
/*		Evaluation Version x.xx					*/
/*		This version is for personal evaluation purposes only.	*/
/*		It may not be redistributed or used for teaching purposes.  	*/
/*		See the "Help" menu for information on obtaining Ready.	*/
/*		This demonstration version of the software is restricted and	*/
/*		   (1) Cannot save programs,				*/
/*		   (2) Cannot compile programs of greater than 100 lines in length, and	*/
/*		   (3) Will not execute programs after xxx		*/
/* _IBM_JAVA_			[Ready for distribution by IBM]		*/
/*		Created by Holt Software Associates Inc.		*/
/*		Development Assisted by IBM Corp.			*/
/*		Lead Programmer: Tom West				*/
/*		Site Version x.xx					*/
/*		Licensed to xxx						*/
/*		Ready uses IBM's Jikes Java Compiler			*/
/*		Java Trademark Info					*/
/* _MINIED_TURING_		[Turing for Ministry of Education	*/
/*		Created by Holt Software Associates Inc.		*/
/*		Development Assisted by IBM Corp.			*/
/*		Lead Programmer: Tom West				*/
/*		Site Version x.xx					*/
/*		Licensed to xxx						*/
/*		Ready uses IBM's Jikes Java Compiler			*/
/*		Java Trademark Info					*/
/************************************************************************/

/*******************/
/* System includes */
/*******************/
#include <windows.h>
#include <windowsx.h>
#include <crtdbg.h>

/****************/
/* Self include */
/****************/
#include "edsplash.h"

/******************/
/* Other includes */
/******************/
#include "ed.h"
#include "edfile.h"
#include "edgui.h"
#include "edprog.h"

#include "eddialogs.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
#define SPLASH_TIMER			   1
#define SPLASH_TIME_IN_MILLISEC		5000

// Margin on either side of picture in pixels
#define MARGIN				  15

// Gaps between titles
#define SMALL_GAP			  10
#define LARGE_GAP			  20

/********************/
/* Global variables */
/********************/

/*********/
/* Types */
/*********/
typedef char	String [256];
typedef struct
{
    HANDLE	picture;
    char	*productNameMsg [2];
    int		productNameLines;
    char	*createdByMsg;
    char	*assistedByMsg;
    char	*leadProgrammerMsg;
    char	*versionString;
    char	*miniMsg [3];
    char	*evalMsg [4];
    char	*betaMsg [4];
    char	*restrictMsg [4];
    char	*miniEdMsg [10];
    char	*siteMsg;
    char	*personalMsg;
    char	*redistribMsg;
    char	*usingJikesMsg;
    char	*win95Msg;
    char	*jreLevelMsg;
    char	*trademarkMsg;
    HFONT	productNameFont;
    HFONT	createdByFont;
    HFONT	chiefProgrammerFont;
    HFONT	versionFont;
    HFONT	usingJikesFont;
    HFONT	trademarkFont;
    int		windowWidth;
    int		bitmapHeight;
    BOOL	usePicture;
} SplashWindowInfo, *SplashWindowInfoPtr;

/**********************/
/* External variables */
/**********************/

/********************/
/* Static constants */
/********************/

/********************/
/* Static variables */
/********************/
// 
static BOOL			stClosingWindow;

/******************************/
/* Static callback procedures */
/******************************/
static LRESULT CALLBACK MySplashWindowProcedure (HWND pmSplashWindow, 
						 UINT pmMessage, 
    						 WPARAM pmWParam, 
    						 LPARAM pmLParam);

/*********************/
/* Static procedures */
/*********************/
static int			MyCreateSplashWindow (HWND pmSplashWindow, 
						      BOOL pmAbout);
static SplashWindowInfoPtr	MyCreateSplashWindowInfo (void);
static void			MyDispose (HWND pmSplashWindow);
static void			MyDrawString (SplashWindowInfoPtr pmInfo, 
					      HDC pmDisplayContext, 
					      HFONT pmFont, char *pmString, 
					      int *pmY, BOOL pmDraw, 
					      BOOL pmOpaque);
static void			MyDrawStrings (SplashWindowInfoPtr pmInfo, 
					       HDC pmDisplayContext, 
					       HFONT pmFont, char *pmString [], 
					       int pmLines, int *pmY, 
					       BOOL pmDraw, BOOL pmOpaque);
static SplashWindowInfoPtr	MyGetSplashWindowInfo (HWND pmSplashWindow);
static void			MyGetVersionNumberAndTradmarkInfo (
						char **pmVersionNumber,
						char **pmTrademarkMsg);
static char			*MyLoadString (int pmResource, char *pmAppend);						
static int			MyPaintSplash (HWND pmSplashWindow, 
					       BOOL pmDraw);


/***********************/
/* External procedures */
/***********************/
/************************************************************************/
/* EdSplash_Init							*/
/* 									*/
/* Initialize the EdSplash module.					*/
/************************************************************************/
BOOL	EdSplash_Init (void)
{
    char	mySplashWindowClassName [256];
    int		myResult;
    WNDCLASSEX	myWindowClass;
    
    // Get the class name
    EdGUI_LoadString (IDS_SPLASH_WINDOW_NAME, mySplashWindowClassName, 
	sizeof (mySplashWindowClassName));
		     
    //
    // Register the editor window class
    //
    myWindowClass.cbSize = 	  sizeof (myWindowClass);
    // Set window class to redraw when window size changes
    myWindowClass.style = 	  CS_OWNDC;
    // Procedure to be called with messages for this window class
    myWindowClass.lpfnWndProc =   MySplashWindowProcedure;
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
    myWindowClass.lpszClassName = mySplashWindowClassName; 
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
    
    return TRUE;
} // EdSplash_Init


/************************************************************************/
/* EdSplash_Finalize							*/
/************************************************************************/
void	EdSplash_Finalize (void)
{
} // EdSplash_Finalize


/************************************************************************/
/* EdSplash_PropertiesSet						*/
/************************************************************************/
void	EdSplash_PropertiesSet (void)
{
    EdSplash_PropertiesImplementChanges ();
} // EdSplash_PropertiesSet

			  
/************************************************************************/
/* EdSplash_PropertiesImplementChanges					*/
/************************************************************************/
void	EdSplash_PropertiesImplementChanges (void)
{
    // Do nothing, for the moment
} // EdSplash_PropertiesImplementChanges


/************************************************************************/
/* EdSplash_Create							*/
/************************************************************************/
HWND	EdSplash_Create (BOOL pmAbout)
{
    char	mySplashWindowClassName [256];
    HWND	mySplashWindow;
    
    // Load the class name (we've already loaded it once, so no error checking)    
    EdGUI_LoadString (IDS_SPLASH_WINDOW_NAME, mySplashWindowClassName, 
        sizeof (mySplashWindowClassName));

    // Create the editor window    
    mySplashWindow = CreateWindow (
    	mySplashWindowClassName, 		// Window class
        NULL,					// Window title
        WS_POPUP | WS_DLGFRAME,			// Window style
    	CW_USEDEFAULT, 				// Initial x location
    	CW_USEDEFAULT, 				// Initial y location
    	CW_USEDEFAULT, 				// Initial x size
    	CW_USEDEFAULT, 				// Initial y size
    	HWND_DESKTOP, 				// Parent window handle
    	NULL, 					// Window menu handle
    	gProgram.applicationInstance,		// Program instance handle
    	&pmAbout);				// Creation parameters

    // If we failed to create the window, return immediately
    if (mySplashWindow == NULL)
    {
    	return NULL;
    }
    
    if (gProperties.logWindowMessages)
    {
    	EdLog_Log ("Splash Window = %x  (Finished Creation)", mySplashWindow);
    }

    stClosingWindow = FALSE;
    
    ShowWindow (mySplashWindow, SW_SHOWNORMAL);
    
    return mySplashWindow;
} // EdSplash_Create

    			      
/******************************/
/* Static callback procedures */
/******************************/
/************************************************************************/
/* MySplashWindowProcedure						*/
/*									*/
/* Callback procedure for the main editor window.			*/
/************************************************************************/
static LRESULT CALLBACK MySplashWindowProcedure (HWND pmSplashWindow, 
						 UINT pmMessage, 
    						 WPARAM pmWParam, 
    						 LPARAM pmLParam)
{
    BOOL	*pmAboutPtr;
    
    // Log the windows messages
    if (gProperties.logWindowMessages)
    {
	EdLog_PrintWindowsMessage ("Splash", pmSplashWindow, pmMessage, 
	    pmWParam, pmLParam);
    }
    
    switch (pmMessage)
    {
    	case WM_CREATE:
	    if (gProperties.logWindowMessages)
	    {
	    	EdLog_Log ("Splash Window = %x  (Starting Creation)", 
	    	    pmSplashWindow);
	    }

	    pmAboutPtr = (BOOL *) 
	        (((LPCREATESTRUCT) pmLParam) -> lpCreateParams);
	    return MyCreateSplashWindow (pmSplashWindow, *pmAboutPtr);

	case WM_DESTROY:
	    MyDispose (pmSplashWindow);
	    break;

	case WM_PAINT:
	    MyPaintSplash (pmSplashWindow, TRUE);
	    return TRUE;
	    
	case WM_TIMER:
	    if (pmWParam == SPLASH_TIMER)
	    {
	    	KillTimer (pmSplashWindow, SPLASH_TIMER);
	    	if (!stClosingWindow)
	    	{
	    	    stClosingWindow = TRUE;
	    	    DestroyWindow (pmSplashWindow);
	    	}
	    }
	    return TRUE;

	case WM_CHAR:
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
	    if (!stClosingWindow)
	    {
	    	stClosingWindow = TRUE;
	    	DestroyWindow (pmSplashWindow);
	    }
	    return TRUE;
	    	    	    	    
        case WM_ACTIVATE:
            if (LOWORD (pmWParam) == WA_INACTIVE)
            {
            	Ed_SetActiveWindow (NULL, NO_WINDOW);
	    	if (!stClosingWindow)
	    	{
	    	    stClosingWindow = TRUE;
	    	    DestroyWindow (pmSplashWindow);
	    	}
            }
            else
            {
            	Ed_SetActiveWindow (pmSplashWindow, SPLASH_WINDOW);
            }
            return 0;
    } // switch
    
    return DefWindowProc (pmSplashWindow, pmMessage, pmWParam, pmLParam);
} // MySplashWindowProcedure




/*********************/
/* Static procedures */
/*********************/
/************************************************************************/
/* MyCreateSplashWindow							*/
/************************************************************************/
static int	MyCreateSplashWindow (HWND pmSplashWindow, BOOL pmAbout)
{
    HINSTANCE		myApplicationInstance;
    SplashWindowInfoPtr	myInfo;
    HDC			myDisplayContext, myMemDC;
    BITMAP		myBitmap;
    RECT		myRect, myScreenRect;
    int			cnt;
    
    // Get the program instance
    myApplicationInstance = 
        (HINSTANCE) GetWindowLong (pmSplashWindow, GWL_HINSTANCE);
        
    //
    // Initialize the extra window memory
    //
	
    // Set the window type            
    SetWindowLong (pmSplashWindow, WINDOW_MEM_WINDOW_TYPE, 
        (long) WINDOW_TYPE_SPLASH);

    // Set the window initialized                
    SetWindowLong (pmSplashWindow, WINDOW_MEM_WINDOW_INITIALIZED, (long) FALSE);
            
    // Create the ddit window information
    myInfo = MyCreateSplashWindowInfo ();
    if (myInfo == NULL)
    {
    	return -1;
    }
    SetWindowLong (pmSplashWindow, WINDOW_MEM_WINDOW_INFO, (long) myInfo);
    	    
    //
    // Add the elements to the window
    //
    
    // Add the picture
    if (gProgram.isJava)
    {
    	myInfo -> picture = LoadImage (myApplicationInstance, 
    	    	MAKEINTRESOURCE (READY_SPLASH_IMAGE), IMAGE_BITMAP, 0, 0,
    	    	LR_CREATEDIBSECTION);
    }
    else if (gProgram.isTuring)
    {
    	if (EdGUI_GetColourDepth () == 8)
    	{
    	    myInfo -> picture = LoadImage (myApplicationInstance, 
    	    	MAKEINTRESOURCE (TURING_256_SPLASH_IMAGE), IMAGE_BITMAP, 0, 0,
    	    	LR_CREATEDIBSECTION);
    	}
    	else
    	{
    	    myInfo -> picture = LoadImage (myApplicationInstance, 
    	    	MAKEINTRESOURCE (TURING_SPLASH_IMAGE), IMAGE_BITMAP, 0, 0,
    	    	LR_CREATEDIBSECTION);
    	}
    }
    	
    // Get the version number
    MyGetVersionNumberAndTradmarkInfo (&(myInfo -> versionString), 
        &(myInfo -> trademarkMsg));
    
    // Get the rest of the information
    if (gProgram.isTuring)
    {
    	myInfo -> productNameMsg [0] = 
    				MyLoadString (IDS_TURING_PRODUCT_NAME, "");
	myInfo -> productNameLines = 1;    				
    }
    else if (gProgram.isJava)
    {
    	myInfo -> productNameMsg [0] = 
    				MyLoadString (IDS_JAVA_PRODUCT_NAME_1, "");
    	myInfo -> productNameMsg [1] = 
    				MyLoadString (IDS_JAVA_PRODUCT_NAME_2, "");
	myInfo -> productNameLines = 2;
    }
    myInfo -> createdByMsg = MyLoadString (IDS_CREATED_BY, "");
    myInfo -> assistedByMsg = MyLoadString (IDS_IBM_ASSIST, "");
    myInfo -> leadProgrammerMsg = MyLoadString (IDS_LEAD_PROGRAMMER, "");
    myInfo -> miniMsg [0] = MyLoadString (IDS_MINI_MSG_1, gProgram.licensedTo);
    myInfo -> miniMsg [1] = MyLoadString (IDS_MINI_MSG_2, "");
    myInfo -> miniMsg [2] = MyLoadString (IDS_MINI_MSG_3, "");
    myInfo -> evalMsg [0] = MyLoadString (IDS_EVAL_MSG_1, "");
    myInfo -> evalMsg [1] = MyLoadString (IDS_EVAL_MSG_2, "");
    myInfo -> evalMsg [2] = MyLoadString (IDS_EVAL_MSG_3, "");
    if (strlen (gProgram.expiryDateString) > 0)
    {
    	myInfo -> evalMsg [3] = MyLoadString (IDS_EVAL_MSG_4, 
    					  gProgram.expiryDateString);
    }
    else
    {
    	myInfo -> evalMsg [3] = EdGlob_MallocString ("    ");
    }    					  
    myInfo -> restrictMsg [0] = MyLoadString (IDS_RESTRICT_MSG_1, "");
    myInfo -> restrictMsg [1] = MyLoadString (IDS_RESTRICT_MSG_2, "");
    myInfo -> restrictMsg [2] = MyLoadString (IDS_RESTRICT_MSG_3, "");
    myInfo -> restrictMsg [3] = MyLoadString (IDS_RESTRICT_MSG_4, 
    					  gProgram.expiryDateString);
    for (cnt = 0 ; cnt < 10 ; cnt++)
    {
        myInfo -> miniEdMsg [cnt] = MyLoadString (IDS_MINIED_MSG_1 + cnt, "");
    }        

    myInfo -> betaMsg [0] = MyLoadString (IDS_BETA_MSG_1, "");
    myInfo -> betaMsg [1] = MyLoadString (IDS_BETA_MSG_2, "");
    myInfo -> betaMsg [2] = MyLoadString (IDS_BETA_MSG_3, "");
    myInfo -> betaMsg [3] = MyLoadString (IDS_BETA_MSG_4, 
    					  gProgram.expiryDateString);
    myInfo -> siteMsg = MyLoadString (IDS_SITE_MSG_1, gProgram.licensedTo);
    myInfo -> personalMsg = MyLoadString (IDS_PERSONAL_MSG_1, 
    					  gProgram.licensedTo);
    myInfo -> redistribMsg = MyLoadString (IDS_REDISTRIB_MSG_1, 
    					   gProgram.licensedTo);
    myInfo -> usingJikesMsg = MyLoadString (IDS_USES_JIKES, "");
    myInfo -> win95Msg = MyLoadString (IDS_WIN_95_JRE_131, "");
    myInfo -> jreLevelMsg =  MyLoadString (IDS_JRE_LEVEL, "");
   
    // Create the fonts
    myInfo -> productNameFont = EdGUI_CreateFont (pmSplashWindow, NULL,
        "Times New Roman", 30, BOLD, NO_ITALIC);
        
    myInfo -> createdByFont = EdGUI_CreateFont (pmSplashWindow, NULL,
        "Times New Roman", 14, BOLD, NO_ITALIC);

    myInfo -> chiefProgrammerFont = EdGUI_CreateFont (pmSplashWindow, NULL,
        "Times New Roman", 10, NO_BOLD, NO_ITALIC);

    myInfo -> versionFont = EdGUI_CreateFont (pmSplashWindow, NULL,
        "Times New Roman", 12, BOLD, NO_ITALIC);

    myInfo -> usingJikesFont = EdGUI_CreateFont (pmSplashWindow, NULL,
        "Times New Roman", 12, NO_BOLD, ITALIC);

    myInfo -> trademarkFont = EdGUI_CreateFont (pmSplashWindow, NULL,
        "Arial", 8, NO_BOLD, NO_ITALIC);

    myDisplayContext = GetDC (pmSplashWindow);
    
    // Get bitmap size
    myMemDC = CreateCompatibleDC (myDisplayContext);
    SelectBitmap (myMemDC, myInfo -> picture);
    GetObject(myInfo -> picture, sizeof(BITMAP), (LPSTR) &myBitmap);
    DeleteDC (myMemDC);

    ReleaseDC (pmSplashWindow, myDisplayContext);

    // If the font properties have not already been determined, 
    // determine them now (we needed a window to do so).  This must be
    // called *after* the top widow and status window created
    EdSplash_PropertiesImplementChanges ();

    // Set the window to initialized
    SetWindowLong (pmSplashWindow, WINDOW_MEM_WINDOW_INITIALIZED, (long) TRUE);

    // Save the bitmap's height    
    myInfo -> bitmapHeight = myBitmap.bmHeight;
    
    // Set the window size.  If the splash window would not fit onto the
    // screen, use a text header.
    myRect.left = 0;
    myRect.right = myBitmap.bmWidth;
    if (gProgram.isJava)
    {
    	myRect.right += 2 * MARGIN;
    }
    myRect.top = 0;
    MyGetSplashWindowInfo (pmSplashWindow) -> usePicture = TRUE;
    myRect.bottom = MyPaintSplash (pmSplashWindow, FALSE) + 2;
    myScreenRect = EdGUI_GetWorkArea ();
    if (myRect.bottom >= myScreenRect.bottom - myScreenRect.top)
    {
    	MyGetSplashWindowInfo (pmSplashWindow) -> usePicture = FALSE;
        myRect.bottom = MyPaintSplash (pmSplashWindow, FALSE) + 2;
    }
    AdjustWindowRect (&myRect, WS_POPUP | WS_DLGFRAME, FALSE);
        	    
    // Set the window to the correct position
    SetWindowPos (pmSplashWindow, HWND_TOP, 0, 0,
    	myRect.right - myRect.left, myRect.bottom - myRect.top, SWP_NOMOVE);

    myInfo -> windowWidth = myRect.right - myRect.left;
        	
    EdGUI_CentreDialogBox (pmSplashWindow);

    ShowWindow (pmSplashWindow, SW_NORMAL);
    
    // If this is the initial splash screen, set the timer.
    if (!pmAbout)
    {
    	SetTimer (pmSplashWindow, SPLASH_TIMER, SPLASH_TIME_IN_MILLISEC, NULL);
    }
    
    return 0;
} // MyCreateSplashWindow

				    
/************************************************************************/
/* MyCreateSplashWindowInfo						*/
/************************************************************************/
static SplashWindowInfoPtr	MyCreateSplashWindowInfo (void)
{
    SplashWindowInfoPtr	myInfo;
    
    // Allocate the structure for the information about the edit window
    myInfo = (SplashWindowInfoPtr) malloc (sizeof (SplashWindowInfo));
    
    if (myInfo != NULL)
    {
    	ZeroMemory (myInfo, sizeof (SplashWindowInfo));
    }

    return myInfo;  
} // MyCreateSplashWindowInfo


/************************************************************************/
/* MyDispose								*/
/************************************************************************/
static void	MyDispose (HWND pmSplashWindow)
{
    SplashWindowInfoPtr	myInfo;
    int			cnt;
        
    myInfo = MyGetSplashWindowInfo (pmSplashWindow);
       
    DeleteObject (myInfo -> picture);

    if (myInfo -> productNameMsg [0] != NULL)
    {
    	free (myInfo -> productNameMsg [0]);
    }
    if (myInfo -> productNameMsg [1] != NULL)
    {
    	free (myInfo -> productNameMsg [1]);
    }
    if (myInfo -> createdByMsg != NULL)
    {
    	free (myInfo -> createdByMsg);
    }
    if (myInfo -> assistedByMsg != NULL)
    {
    	free (myInfo -> assistedByMsg);
    }
    if (myInfo -> leadProgrammerMsg != NULL)
    {
    	free (myInfo -> leadProgrammerMsg);
    }
    if (myInfo -> versionString != NULL)
    {
    	free (myInfo -> versionString);
    }
    for (cnt = 0 ; cnt < 3 ; cnt++)
    {
    	if (myInfo -> miniMsg [cnt] != NULL)
    	{
    	    free (myInfo -> miniMsg [cnt]);
    	}
    }
    for (cnt = 0 ; cnt < 4 ; cnt++)
    {
    	if (myInfo -> evalMsg [cnt] != NULL)
    	{
    	    free (myInfo -> evalMsg [cnt]);
    	}
    }
    for (cnt = 0 ; cnt < 4 ; cnt++)
    {
    	if (myInfo -> restrictMsg [cnt] != NULL)
    	{
    	    free (myInfo -> restrictMsg [cnt]);
    	}
    }
    for (cnt = 0 ; cnt < 4 ; cnt++)
    {
    	if (myInfo -> betaMsg [cnt] != NULL)
    	{
    	    free (myInfo -> betaMsg [cnt]);
    	}
    }
    if (myInfo -> siteMsg != NULL)
    {
    	free (myInfo -> siteMsg);
    }
    if (myInfo -> personalMsg != NULL)
    {
    	free (myInfo -> personalMsg);
    }
    if (myInfo -> redistribMsg != NULL)
    {
    	free (myInfo -> redistribMsg);
    }
    if (myInfo -> usingJikesMsg != NULL)
    {
    	free (myInfo -> usingJikesMsg);
    }
    if (myInfo -> win95Msg != NULL)
    {
    	free (myInfo -> win95Msg);
    }
    if (myInfo -> jreLevelMsg != NULL)
    {
    	free (myInfo -> jreLevelMsg);
    }
    if (myInfo -> trademarkMsg != NULL)
    {
    	free (myInfo -> trademarkMsg);
    }
    DeleteObject (myInfo -> productNameFont);
    DeleteObject (myInfo -> versionFont);
    DeleteObject (myInfo -> createdByFont);
    DeleteObject (myInfo -> chiefProgrammerFont);
    DeleteObject (myInfo -> usingJikesFont);
    DeleteObject (myInfo -> trademarkFont);
    
    free (myInfo);
} // MyDispose


/************************************************************************/
/* MyDrawString								*/
/************************************************************************/
static void	MyDrawString (SplashWindowInfoPtr pmInfo, HDC pmDisplayContext, 
			      HFONT pmFont, char *pmString, int *pmY, 
			      BOOL pmDraw, BOOL pmOpaque)
{
    SIZE		mySize;
    int			myX;
    
    SelectFont (pmDisplayContext, pmFont);
    GetTextExtentPoint32 (pmDisplayContext, pmString, strlen (pmString), 
        &mySize);
    if (pmDraw)
    {
    	myX = (pmInfo -> windowWidth - mySize.cx) / 2;
    	if (!pmOpaque)
    	{
    	    SetBkMode (pmDisplayContext, TRANSPARENT);
    	}
    	ExtTextOut (pmDisplayContext, myX, *pmY, 0, NULL, 
    	    pmString, strlen (pmString), NULL);
    	if (!pmOpaque)
    	{
    	    SetBkMode (pmDisplayContext, OPAQUE);
    	}
    }
    *pmY = *pmY + mySize.cy;
} // MyDrawString 


/************************************************************************/
/* MyDrawStrings							*/
/************************************************************************/
static void	MyDrawStrings (SplashWindowInfoPtr pmInfo, HDC pmDisplayContext, 
			     HFONT pmFont, char *pmString[], int pmLines, 
			     int *pmY, BOOL pmDraw, BOOL pmOpaque)
{
    SIZE		mySize;
    int			myX, myWidth, cnt;
    
    SelectFont (pmDisplayContext, pmFont);
    GetTextExtentPoint32 (pmDisplayContext, pmString [0], strlen (pmString [0]), 
        &mySize);
    myWidth = mySize.cx;
    for (cnt = 1 ; cnt < pmLines ; cnt++)
    {
    	GetTextExtentPoint32 (pmDisplayContext, pmString [cnt],
	    strlen (pmString [cnt]), &mySize);
    	myWidth = max (myWidth, mySize.cx);
    }
    myX = (pmInfo -> windowWidth - myWidth) / 2;
    	
    for (cnt = 0 ; cnt < pmLines ; cnt++)
    {	
    	if (pmDraw)
    	{
    	    if (!pmOpaque)
    	    {
    	    	SetBkMode (pmDisplayContext, TRANSPARENT);
    	    }
    	    ExtTextOut (pmDisplayContext, myX, *pmY, 0, NULL, 
    	    	pmString [cnt], strlen (pmString [cnt]), NULL);
    	    if (!pmOpaque)
    	    {
    	    	SetBkMode (pmDisplayContext, OPAQUE);\
    	    }
    	}
    	*pmY = *pmY + mySize.cy;
    }
} // MyDrawStrings 


/************************************************************************/
/* MyGetSplashWindowInfo						*/
/************************************************************************/
static SplashWindowInfoPtr	MyGetSplashWindowInfo (HWND pmSplashWindow)
{
    int			myWindowType;
    SplashWindowInfoPtr	myInfo;
    
    myWindowType = GetWindowLong (pmSplashWindow, WINDOW_MEM_WINDOW_TYPE);
    if (myWindowType != WINDOW_TYPE_SPLASH)
    {
    	EdFail_Fatal (IDS_BADWINDOWTYPE, __FILE__, __LINE__, 0,
    		      myWindowType, WINDOW_TYPE_EDIT);
    }
    	
    myInfo = (SplashWindowInfoPtr) GetWindowLong (pmSplashWindow, 
        WINDOW_MEM_WINDOW_INFO);
    if (myInfo == NULL)
    {
    	EdFail_Fatal (IDS_GETWINDOWLONGFAIL, __FILE__, __LINE__,
    		      GetLastError (), WINDOW_MEM_WINDOW_INFO);
    }
       
    return myInfo;
} // MyGetSplashWindowInfo


/************************************************************************/
/* MyGetVersionNumberAndTradmarkInfo					*/
/************************************************************************/
static void	MyGetVersionNumberAndTradmarkInfo (char **pmVersionNumber,
						   char **pmTrademarkMsg)
{
    FilePath			myApplicationName;
    char			myTempString [256];
    DWORD			myDummy;
    DWORD			myVersionSize;
    char			*myVersionData, *myVersionInfo;
    UINT			myVersionInfoSize;
    

    // Set to empty string by default
    *pmVersionNumber = NULL;
    *pmTrademarkMsg = NULL;
        
    // Get the version information
    // Read the properties file from the application directory
    // Get the applicaton name
    if (GetModuleFileName (NULL, myApplicationName, PATHNAME_LENGTH) == 0)
    {
    	return;
    }
    
    myVersionSize = GetFileVersionInfoSize (myApplicationName, &myDummy);
    if (myVersionSize == 0)
    {
    	return;
    }
    
    myVersionData = malloc (myVersionSize);
    if (myVersionData == NULL)
    {
    	return;
    }
    
    if (!GetFileVersionInfo (myApplicationName, myDummy, myVersionSize, 
        myVersionData))
    {
    	free (myVersionData);
    	return;
    }
    	
    if (!VerQueryValue (myVersionData, 
        "\\StringFileInfo\\04090000\\ProductVersion", &myVersionInfo, 
        &myVersionInfoSize))
    {
    	free (myVersionData);
    	return;
    }

    *pmVersionNumber = malloc (myVersionInfoSize + 100);
    if (*pmVersionNumber == NULL)
    {
    	free (myVersionData);
    	return;
    }

    if (gProgram.miniVersion)    
    {
    	EdGUI_LoadString (IDS_MINI_VERSION_STRING, myTempString, 
    	    sizeof (myTempString));
    }
    else if (gProgram.installKind == INSTALL_KIND_OPEN)
    {
    	EdGUI_LoadString (IDS_OPEN_VERSION_STRING, myTempString, 
    	    sizeof (myTempString));
    }
    else if (gProgram.installKind == INSTALL_KIND_BETA)
    {
    	EdGUI_LoadString (IDS_BETA_VERSION_STRING, myTempString, 
    	    sizeof (myTempString));
    }
    else if (gProgram.installKind == INSTALL_KIND_EVAL)
    {
    	EdGUI_LoadString (IDS_EVAL_VERSION_STRING, myTempString, 
    	    sizeof (myTempString));
    }
    else if (gProgram.installKind == INSTALL_KIND_SITE)
    {
    	EdGUI_LoadString (IDS_SITE_VERSION_STRING, myTempString, 
    	    sizeof (myTempString));
    }
    else if (gProgram.installKind == INSTALL_KIND_PERSONAL)
    {
    	EdGUI_LoadString (IDS_PERSONAL_VERSION_STRING, myTempString, 
    	    sizeof (myTempString));
    }
    else if (gProgram.installKind == INSTALL_KIND_REDISTRIB)
    {
    	EdGUI_LoadString (IDS_REDISTRIB_VERSION_STRING, myTempString, 
    	    sizeof (myTempString));
    }
    else
    {
    	EdFail_Fatal (IDS_BADINSTALLTYPE, __FILE__, __LINE__, 0,
    		      gProgram.installKind);
    }
    strcpy (*pmVersionNumber, myTempString);
    strncat (*pmVersionNumber, myVersionInfo, myVersionInfoSize);
    
    if (gProgram.installKind == INSTALL_KIND_BETA)
    {
	strcat (*pmVersionNumber, " (");
	strcat (*pmVersionNumber, __DATE__);
	strcat (*pmVersionNumber, ")");
    }
    if (!VerQueryValue (myVersionData, 
        "\\StringFileInfo\\04090000\\LegalTrademarks", &myVersionInfo, 
        &myVersionInfoSize))
    {
    	free (myVersionData);
    	return;
    }
    
    *pmTrademarkMsg = malloc (myVersionInfoSize + 1);
    if (*pmTrademarkMsg == NULL)
    {
    	free (myVersionData);
    	return;
    }
    
    strncpy (*pmTrademarkMsg, myVersionInfo, myVersionInfoSize);

    free (myVersionData);
} // MyGetVersionNumber


/************************************************************************/
/* MyLoadString								*/
/************************************************************************/
static char	*MyLoadString (int pmResource, char *pmAppend)
{
    char	myString [4096], *myPtr;
    
    EdGUI_LoadString (pmResource, myString, sizeof (myString));
    myPtr = malloc (strlen (myString) + strlen (pmAppend) + 1);
    strcpy (myPtr, myString);
    strcat (myPtr, pmAppend);    		   
    
    return myPtr;
} // MyLoadString


/************************************************************************/
/* MyPaintSplash							*/
/*									*/
/* This returns the desired window height. If pmDraw is false, it does	*/
/* not draw to the screen.						*/
/************************************************************************/
static int	MyPaintSplash (HWND pmSplashWindow, BOOL pmDraw)
{
    SplashWindowInfoPtr	myInfo;
    HDC			myDisplayContext, myMemDC;
    PAINTSTRUCT		myPaintStruct;
    BITMAP		myBitmap;
    HBITMAP		myOldBitmap;
    HPALETTE		myPalette, myOldPalette;
    POINT		myPoint, myOrigin;
    int			myY;

    myInfo = MyGetSplashWindowInfo (pmSplashWindow);
        		
    myDisplayContext = BeginPaint (pmSplashWindow, &myPaintStruct);

    if (pmDraw && myInfo -> usePicture)
    {
    	myMemDC = CreateCompatibleDC (NULL);
    	myOldBitmap = SelectBitmap (myMemDC, myInfo -> picture);

    	GetObject (myInfo -> picture, sizeof(BITMAP), (LPSTR) &myBitmap);

    	myPoint.x = myBitmap.bmWidth;
    	myPoint.y = myBitmap.bmHeight;
    	DPtoLP (myDisplayContext, &myPoint, 1);

    	myOrigin.x = 0;
    	myOrigin.y = 0;
    	DPtoLP (myMemDC, &myOrigin, 1);
    	
    	//
    	// We need to perform all sorts of stuff to get the palette right...
    	//
   	// If the DIBSection is 256 color or less, it has a color table
   	if ((myBitmap.bmBitsPixel * myBitmap.bmPlanes) <= 8)
   	{
	   RGBQUAD      myClrTbl [256];
	   LPLOGPALETTE myLogPal;
	   WORD         cnt;

   	    // Get the DIBSection's color table
   	    GetDIBColorTable (myMemDC, 0, 256, myClrTbl);
   	    
	    // Create a palette from the color tabl
	    myLogPal = (LOGPALETTE *) malloc (sizeof(LOGPALETTE) + 
	    					(256 * sizeof(PALETTEENTRY)));
	    myLogPal -> palVersion = 0x300;
	    myLogPal -> palNumEntries = 256;
	    for (cnt = 0 ; cnt < 256 ; cnt++)
	    {
	 	myLogPal -> palPalEntry [cnt].peRed = myClrTbl [cnt].rgbRed;
		myLogPal -> palPalEntry [cnt].peGreen = myClrTbl [cnt].rgbGreen;
		myLogPal -> palPalEntry [cnt].peBlue = myClrTbl [cnt].rgbBlue;
		myLogPal -> palPalEntry [cnt].peFlags = 0;
	    }
   	    myPalette = CreatePalette (myLogPal);
	    free (myLogPal);
   	    
	    // Select Palette
	    myOldPalette = SelectPalette (myDisplayContext, myPalette, FALSE);
	    RealizePalette (myDisplayContext);
	}

	if (gProgram.isTuring)
	{
    	    BitBlt (myDisplayContext, 0, 0, myPoint.x, myPoint.y,
            	myMemDC, myOrigin.x, myOrigin.y, SRCCOPY);
	}
	else
	{
    	    BitBlt (myDisplayContext, MARGIN, 0, myPoint.x, myPoint.y,
            	myMemDC, myOrigin.x, myOrigin.y, SRCCOPY);
        }

	SelectObject (myMemDC, myOldBitmap);
   	if ((myBitmap.bmBitsPixel * myBitmap.bmPlanes) <= 8)
   	{
   	    SelectPalette (myDisplayContext, myOldPalette, FALSE);
   	    DeleteObject (myPalette);
   	}
    	DeleteDC (myMemDC);
    }

    // Place Ministry message over top of the Sun
    if (gProgram.assistedByIBM && gProgram.isTuring)
    {
    	//
    	// Output "Ministry of Education License Message"
    	//
    	myY = LARGE_GAP;
        MyDrawString (myInfo, myDisplayContext, myInfo -> versionFont,
    		      myInfo -> miniEdMsg [0], &myY, pmDraw, FALSE);
	myY += SMALL_GAP;
        MyDrawStrings (myInfo, myDisplayContext, myInfo -> chiefProgrammerFont,
    		  &myInfo -> miniEdMsg [1], 9, &myY, pmDraw, FALSE);
    } // if (gProgram.assistedByIBM)
       
    if (myInfo -> usePicture)
    {
    	// The starting Y position
    	myY = myInfo -> bitmapHeight + MARGIN;
    }
    else
    {
    	myY = SMALL_GAP;
        MyDrawStrings (myInfo, myDisplayContext, myInfo -> productNameFont,
    		  myInfo -> productNameMsg, myInfo -> productNameLines, 
    		  &myY, pmDraw, TRUE);
    	myY += LARGE_GAP;
    }

    //
    // Output "Created By Holt Software Associates Inc."
    //
    MyDrawString (myInfo, myDisplayContext, myInfo -> createdByFont,
    		  myInfo -> createdByMsg, &myY, pmDraw, TRUE);
    
    if (gProgram.assistedByIBM && gProgram.isJava)
    {
    	//
    	// Output "Development Assisted by IBM Corp."
    	//
        MyDrawString (myInfo, myDisplayContext, myInfo -> createdByFont,
    		      myInfo -> assistedByMsg, &myY, pmDraw, TRUE);
    } // if (gProgram.assistedByIBM)

    //
    //        	    
    // Output "Lead Programmer: Tom West"
    //
    if (gProgram.isJava)
    {
        MyDrawString (myInfo, myDisplayContext, myInfo -> chiefProgrammerFont,
    		      myInfo -> leadProgrammerMsg, &myY, pmDraw, TRUE);
    } // if (gProgram.isJava)
	    	
    //    	    
    // Output version number	    	
    //
    myY += SMALL_GAP;
    MyDrawString (myInfo, myDisplayContext, myInfo -> versionFont,
    		  myInfo -> versionString, &myY, pmDraw, TRUE);

    //
    // Output install kind specific info
    if (gProgram.miniVersion)
    {
    	//
	// Output: Licensed to xxx.
	//	   This version cannot compile programs of greater than 100 lines in length
	//
        myY += SMALL_GAP;
        MyDrawStrings (myInfo, myDisplayContext, myInfo -> chiefProgrammerFont,
    		       myInfo -> miniMsg, 3, &myY, pmDraw, TRUE);
    }
    else if (gProgram.installKind == INSTALL_KIND_EVAL)
    {
    	//
	// Output: This version is for personal evaluation purposes only.
	//	   It may not be redistributed or used for teaching purposes.
	//	   See the "Help" menu for information on obtaining Ready.
	//	   This version is time limited and will not execute Java programs after xxx
	// or
	//
	// Output: This version is for personal evaluation purposes only.
	//	   It may not be redistributed or used for teaching purposes.
	//	   See the "Help" menu for information on obtaining Ready.
	//	   This demonstration version of the software is restricted and
	//		(1) Cannot save programs,
	//    	        (2) Cannot compile programs of greater than 100 lines in length, and
	//		(3) Will not execute programs after xxx
	//
        myY += SMALL_GAP;
	if (gProgram.restrictedVersion)
	{
            MyDrawStrings (myInfo, myDisplayContext, 
            		   myInfo -> chiefProgrammerFont,
    		           myInfo -> evalMsg, 3, &myY, pmDraw, TRUE);
	    myY += SMALL_GAP;    		           
            MyDrawStrings (myInfo, myDisplayContext, 
            		   myInfo -> chiefProgrammerFont,
    		           myInfo -> restrictMsg, 4, &myY, pmDraw, TRUE);
	}
	else
	{
            MyDrawStrings (myInfo, myDisplayContext, 
            		   myInfo -> chiefProgrammerFont,
    		           myInfo -> evalMsg, 4, &myY, pmDraw, TRUE);
	}
    }
    else if (gProgram.installKind == INSTALL_KIND_BETA)
    {
    	//
	// Output: This version is for personal beta test purposes only.
	//	   It may not be redistributed or used for teaching purposes.
	//	   Please report all bugs found in this software to west@hsa.on.ca
	//	   This version of the software will not execute programs after xxx
	//
        myY += SMALL_GAP;
        MyDrawStrings (myInfo, myDisplayContext, 
            	       myInfo -> chiefProgrammerFont,
    		       myInfo -> betaMsg, 4, &myY, pmDraw, TRUE);
    }
    else if (gProgram.installKind == INSTALL_KIND_PERSONAL)
    {
        myY += SMALL_GAP;
        MyDrawString (myInfo, myDisplayContext, myInfo -> chiefProgrammerFont,
    		      myInfo -> personalMsg, &myY, pmDraw, TRUE);
    }
    else if (gProgram.installKind == INSTALL_KIND_SITE)
    {
        myY += SMALL_GAP;
        MyDrawString (myInfo, myDisplayContext, myInfo -> chiefProgrammerFont,
    		      myInfo -> siteMsg, &myY, pmDraw, TRUE);
    }
    else if (gProgram.installKind == INSTALL_KIND_REDISTRIB)
    {
        myY += SMALL_GAP;
        MyDrawString (myInfo, myDisplayContext, myInfo -> chiefProgrammerFont,
    		      myInfo -> redistribMsg, &myY, pmDraw, TRUE);
    }
    
    if (gProgram.isJava)
    {
    	//
    	// Output "Using IBM's Jikes Java Compiler"
    	//
        myY += SMALL_GAP;
        MyDrawString (myInfo, myDisplayContext, myInfo -> usingJikesFont,
    		      myInfo -> usingJikesMsg, &myY, pmDraw, TRUE);

        myY += SMALL_GAP;
	if (gProgram.operatingSystem == WIN_95)
	{
            MyDrawString (myInfo, myDisplayContext, myInfo -> trademarkFont,
	    		  myInfo -> win95Msg, &myY, pmDraw, TRUE);
	}
	else
	{
            MyDrawString (myInfo, myDisplayContext, myInfo -> trademarkFont,
	    		  myInfo -> jreLevelMsg, &myY, pmDraw, TRUE);
	}

    	//    	        
    	// Output trademark information
    	//
        myY += SMALL_GAP;
        MyDrawString (myInfo, myDisplayContext, myInfo -> trademarkFont,
    		      myInfo -> trademarkMsg, &myY, pmDraw, TRUE);
    } // if (gProgram.isJava)
    
    EndPaint (pmSplashWindow, &myPaintStruct);
    
    return myY;
} // MyPaintSplash
