/**************/
/* dbgalloc.c */
/**************/

/*******************/
/* System includes */
/*******************/
#include <windows.h>
#include <commctrl.h>

/****************/
/* Self include */
/****************/
#include "dbgalloc.h"

/******************/
/* Other includes */
/******************/
#include "dbgmain.h"

#include "edfail.h"
#include "edglob.h"
#include "edgui.h"
#include "edprog.h"

#include "eddialogs.h"

#include "mio.h"

#include "fileman.h"

#include "language.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
#define NUM_COLUMNS		4

#define BY_ID			120
#define BY_LOCATION		121

#define DEFAULT_LOCATION	-999
#define DEFAULT_WIDTH		527
#define DEFAULT_HEIGHT		287
				
#define TURINGSTDIN		(-2)
#define TURINGSTDOUT		(-1)

/********************/
/* Global variables */
/********************/

/*********/
/* Types */
/*********/
typedef struct Object
{
    int 		objectType;
    int 		ID;
    SRCPOS 		srcPos;
    int			freed;
    char 		*description;
    struct Object	*next;
} Object, *ObjectPtr;

/**********************/
/* External variables */
/**********************/

/********************/
/* Static constants */
/********************/
static int	stIconNumber[] = 
{
    DBG_FILE_ICON, DBG_WINDOW_ICON, DBG_NET_ICON, DBG_FONT_ICON, DBG_DIR_ICON,
    DBG_PIC_ICON, DBG_SPRITE_ICON, DBG_LEXER_ICON, DBG_UNKNOWN_ICON
};

static int	stIconLookup[] = 
{
    FILE_STREAM, WINDOW_STREAM, NET_STREAM, FONT_ID, DIR_ID, 
    PIC_ID, SPRITE_ID, LEXER_ID
};

static char	*stObjectTypes[] = 
{
    "File", "Window", "Net", "Font", "Directory",
    "Picture", "Sprite", "Lexer", "Unknown"
};

static int	stColumnSizes[] = {80, 40, 200, 200};
	
/********************/
/* Static variables */
/********************/
static ObjectPtr	stHead = NULL;
static HWND		stDebuggerAllocWindow = NULL;
static BOOL		stDebuggerAllocVisible = FALSE;
static int		stNumObjectTypes;
static int		stSortingCriteria = BY_ID;
static HWND		stListView;
static HIMAGELIST	stImageList;
static BOOL		stReallyFree = TRUE;
static int		stX = DEFAULT_LOCATION, stY = DEFAULT_LOCATION;
static int		stWidth = DEFAULT_WIDTH, stHeight = DEFAULT_HEIGHT;
static BOOL		stInitiallyVisible = FALSE;
static BOOL		stMovedOrResized = FALSE;

/******************************/
/* Static callback procedures */
/******************************/
static LRESULT CALLBACK MyDebugAllocWindowProcedure (HWND pmWindow, 
						     UINT pmMessage, 
	    					     WPARAM pmWParam, 
	    					     LPARAM pmLParam);
static LRESULT CALLBACK MyListViewCompareProc (LPARAM pmParam1, LPARAM pmParam2,
					       LPARAM pmParamSort);

/*********************/
/* Static procedures */
/*********************/
static void	MyCreateAndPlaceListView (HWND pmParentWindow);
static int	MyGetImageNumber (int pmObjectType);
static void	MyNotifyHandler (HWND pmWindow, UINT pmMessage, WPARAM pmWParam, 
	    			 LPARAM pmLParam);


/***********************/
/* External procedures */
/***********************/
/************************************************************************/
/* DbgAlloc_Init							*/
/************************************************************************/
BOOL	DbgAlloc_Init (void)
{
    // Create the Window
    char	myWindowClassName [256];
    WNDCLASSEX	myWindowClass;
    int		myResult;
    int		cnt;
        
    // Get the class name
    EdGUI_LoadString (IDS_DEBUG_ALLOC_WINDOW_NAME, 
	myWindowClassName, sizeof (myWindowClassName));
    
    //
    // Register the debugger controller window class
    //
    myWindowClass.cbSize = sizeof (myWindowClass);
    // Set window class to redraw when window size changes
    myWindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    // Procedure to be called with messages for this window class
    myWindowClass.lpfnWndProc = MyDebugAllocWindowProcedure;
    // The extra space in class struct
    myWindowClass.cbClsExtra = 0;
    // The extra space in window struct for the pointer to text data
    // and editor window
    myWindowClass.cbWndExtra = 0;
    // The application's handle
    myWindowClass.hInstance = gProgram.applicationInstance;
    // Set the icon for this window class
    myWindowClass.hIcon = LoadIcon (gProgram.applicationInstance, 
	MAKEINTRESOURCE (APP_ICON));
    // Set the cursor for this window class
    myWindowClass.hCursor = LoadCursor (NULL, IDC_ARROW);
    // Set the background colour for this window
    myWindowClass.hbrBackground = GetSysColorBrush (COLOR_BTNFACE);
    // Set the menu for this window class
    myWindowClass.lpszMenuName = NULL;
    // Name of the window class
    myWindowClass.lpszClassName = myWindowClassName; 
    // Set the icon for this class.
    myWindowClass.hIconSm = LoadIcon (gProgram.applicationInstance, 
	MAKEINTRESOURCE (DEBUG_SMALL_ICON));
    
    myResult = RegisterClassEx (&myWindowClass);
    if (myResult == 0)
    {
	EdFail_Warn (IDS_REGISTERCLASSFAIL, __FILE__, __LINE__, 
	    GetLastError ());
	return FALSE;
    }
    
    // Load the images into an image list
    stNumObjectTypes = sizeof (stIconNumber) / sizeof (int);
    if (stNumObjectTypes != (sizeof (stIconLookup) / sizeof (int)) + 1)
    {
	EdFail_Warn (IDS_ICONMISMATCHFAIL, __FILE__, __LINE__, 0, 1,
	    stNumObjectTypes, (sizeof (stIconLookup) / sizeof (int)) + 1);
    	return FALSE;
    }
    if (stNumObjectTypes != (sizeof (stObjectTypes) / sizeof (char *)))
    {
	EdFail_Warn (IDS_ICONMISMATCHFAIL, __FILE__, __LINE__, 0, 2,
	    stNumObjectTypes, (sizeof (stObjectTypes) / sizeof (char *)));
    	return FALSE;
    }
    
    stImageList = ImageList_Create (16, 16, 0, stNumObjectTypes, 0);
    for (cnt = 0 ; cnt < stNumObjectTypes ; cnt++)
    {
    	HICON	myIcon = LoadIcon (gProgram.applicationInstance, 
    				   MAKEINTRESOURCE (stIconNumber [cnt]));
	if (myIcon == NULL)
	{
	    EdFail_Warn (IDS_LOADICONFAIL, __FILE__, __LINE__, 
	    	GetLastError (), stIconNumber [cnt]);
	    return FALSE;
	}
	if (ImageList_AddIcon (stImageList, myIcon) == -1)
	{
	    EdFail_Warn (IDS_ADDICONFAIL, __FILE__, __LINE__, 
	    	GetLastError (), stIconNumber [cnt]);
	    return FALSE;
	}
    } // for
    
    // Create the debugger controller window    
    stDebuggerAllocWindow = CreateWindow (
	myWindowClassName, 			// Window class
	"Allocated Objects",			// Window title
	WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, 	// Window style
	CW_USEDEFAULT, 				// Initial x location
	CW_USEDEFAULT, 				// Initial y location
	CW_USEDEFAULT, 				// Initial x size
	CW_USEDEFAULT, 				// Initial y size
	HWND_DESKTOP, 				// Parent window handle
	NULL, 					// Window menu handle
	gProgram.applicationInstance,		// Program instance handle
	NULL);					// Creation parameters

    if (stDebuggerAllocWindow == NULL)
    {
	EdFail_Warn (IDS_WINDOWNOTCREATEDFAIL, __FILE__, __LINE__,
		     GetLastError ());
	return FALSE;
    }

    if (stListView == NULL)
    {
	EdFail_Warn (IDS_LISTVIEWNOTCREATEDFAIL, __FILE__, __LINE__, 
		     GetLastError ());
   	return FALSE;
    }
    return TRUE;
} // DbgAlloc_Init


/************************************************************************/
/* DbgAlloc_Allocate							*/
/*									*/
/* When are things allocated:	[] = Turing				*/
/*	STDIN:	(STDIN is window) MIO_Init_Run -> MIOWindow_RegisterWindow
/*	STDOUT:	(STDOUT is window) MIO_Init_Run -> MIOWindow_RegisterWindow
/*	STDERR:	MIO_Init_Run -> MIOWindow_RegisterWindow
/*	File:	(open:) Language_Execute_Open -> Language_Execute_AddFile -> MIO_RegisterOpen
/*	Window:	(Window.Open) [Window.Open] -> [win_registeropen] -> MIO_win_registeropen -> MIOWindow_RegisterOpen
/*		Window.Open:	open : winId, "%window(" + winSetupStr + ")", put, get
/*				old_setactive (winId)
/*				win_registeropen (winId)
/*	Net:	(Net.OpenConnection) [Net.OpenConnection] -> [net_registeropen] -> MIO_net_registeropen -> MIONet_RegisterOpen
/*		Net.OpenConnection: 	var netId: int
/*					open: netId, "%net(C:" + intstr(port) + ":" + netAddr + ")", put, get
/*					if netId > 0 then
/*					    net_registeropen(netId)
/*					end if
/*		(Net.WaitForConnection) [Net.WaitForConnection] -> [net_registeropen] -> MIO_net_registeropen -> MIONet_RegisterOpen
/*		Net.WaitForConnection:	var netId: int
/*					open: netId, "%net(A:" + intstr(port) + ")", put, get
/*					if netId > 0 then
/*					    net_registeropen(netId)
/*					    net_waitforconnect(netId)
/*					    net_getaddr(netId, netAddr)
/*					end if
/*	Font:	(Font.New) [Font.New] -> MIOFont_New -> MIO_IDAdd
/*	Dir:	(Dir.Open) [Dir.Open] -> MIODir_Open -> MIO_IDAdd
/*	Pic:	(Pic.New) [Pic.Open] -> MIOPic_New -> MIO_IDAdd
/*	Sprite:
/*	Lexer:
*/
/************************************************************************/
void	DbgAlloc_Allocate (int pmObjectType, int pmID, MYSRCPOS pmSrcPos, 
			   const char *pmDescription)
{
    ObjectPtr	myObject = (ObjectPtr) malloc (sizeof (Object));
    ObjectPtr	myPtr = stHead, myPrev = NULL;
    char	myObjectPathName [1024], myPtrPathName [1024];
    char	*myObjectCompName = myObjectPathName;
    char	*myPtrCompName = myPtrPathName;
    int		myCompare;
    LVITEM	myItem;
    int		myNumItems, myCurrentItem;
    int		cnt;
    
    myObject -> objectType = pmObjectType;
    myObject -> ID = pmID;
    myObject -> srcPos = * (SRCPOS *) pmSrcPos;
    myObject -> freed = 0;
    myObject -> description = EdGlob_MallocString (pmDescription);

    // Always add the object to the beginning of the linked list on the 
    // assumption its mostly likely to be freed next.
    myObject -> next = stHead;
    stHead = myObject;
    
    // Now, find out where it actually should be placed on the list
    myNumItems = ListView_GetItemCount (stListView);
    myCurrentItem = 0;
    myItem.mask = LVIF_PARAM;
    myItem.iSubItem = 0;
    while (myCurrentItem < myNumItems)
    {
    	myItem.iItem = myCurrentItem;
    	if (!ListView_GetItem (stListView, &myItem))
    	{
	    EdGUI_Message1 (stDebuggerAllocWindow, MB_ICONEXCLAMATION, 
		IDS_DEBUGGER_FAILURE_TITLE, IDS_CANT_GET_LIST_ITEM,
		stObjectTypes [MyGetImageNumber (myObject -> objectType)],
    		GetLastError ());
    	    return;
    	}
    	myPtr = (ObjectPtr) myItem.lParam;
    	
    	// We've got the item.  Let's try and sort it.
	if (stSortingCriteria == BY_ID)
	{
	    if ((myObject -> objectType < myPtr -> objectType) ||
	        ((myObject -> objectType == myPtr -> objectType) &&
	         (myObject -> ID < myPtr -> ID)))
	    {
	    	break;
	    }
	}
	else if (stSortingCriteria == BY_LOCATION)
	{
	    if (myObject -> srcPos.fileNo == 0)
	    {
	    	myCompare = -1;
	    }
	    else if (myPtr -> srcPos.fileNo == 0)
	    {
	    	myCompare = 1;
	    }
	    else
	    {
    	    	FileManager_FileName (myObject -> srcPos.fileNo, 
    	    			      myObjectPathName);
    	    	FileManager_FileName (myPtr -> srcPos.fileNo, myPtrPathName);
	    	if (!gProperties.useFullPathNames)
	    	{
		    myObjectCompName = strrchr (myObjectPathName, '\\');
		    if (myObjectCompName != NULL) myObjectCompName++;
		    else myObjectCompName = myObjectPathName;				
		    myPtrCompName = strrchr (myPtrPathName, '\\');
		    if (myPtrCompName != NULL) myPtrCompName++;
		    else myPtrCompName = myPtrPathName;				
	    	}
	    	myCompare = strcmp (myObjectCompName, myPtrCompName);
	    }
	    if ((myCompare < 0) ||
	        ((myCompare == 0) &&
	         (myObject -> srcPos.lineNo < myPtr -> srcPos.lineNo)))
	    {
	    	break;
	    }
	}	         
    		
    	myCurrentItem++;
    }

    // Insert the item at location myCurrentItem
    myItem.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
    myItem.state = 0;
    myItem.stateMask = 0;
    myItem.iItem = myCurrentItem;
    myItem.iSubItem = 0;
    myItem.pszText = LPSTR_TEXTCALLBACK;
    myItem.cchTextMax = 100;
    myItem.iImage = MyGetImageNumber (pmObjectType);
    myItem.lParam = (LPARAM) myObject;
    
    ListView_InsertItem (stListView, &myItem);
    
    // Set all the fields
    for (cnt = 0 ; cnt < NUM_COLUMNS ; cnt++)
    {
    	ListView_SetItemText (stListView, myCurrentItem, cnt, 
    			      LPSTR_TEXTCALLBACK);
    }    			     
} // DbgAlloc_Allocate

			   
/************************************************************************/
/* DbgAlloc_Finalize_Run						*/
/************************************************************************/
void	DbgAlloc_Finalize_Run (void)
{
    stReallyFree = FALSE;
} // DbgAlloc_Finalize_Run


/************************************************************************/
/* DbgAlloc_Free							*/
/*									*/
/* When are things deallocated:						*/
/*	STDIN:	(Terminate Run, STDIN is window) - DbgAlloc_Free (-2) called when DbgAlloc_Free (-1) called
/*	STDOUT:	(Terminate Run, STDOUT is window, not visible) MIO_Finalize_Run -> MyCloseRunWindows -> MIOWin_CloseWindow -> MDIOWin_WindowProcedure (WM_DESTROY) -> MIOWindow_RegisterClose
/*		(Terminate Run, STDOUT is window, visible)  EdRun_Compile -> MIO_Init_Free -> MIO_CloseAllRunWindows -> MyCloseRunWindows -> MIOWin_CloseWindow -> MDIOWin_WindowProcedure (WM_DESTROY) -> MIOWindow_RegisterClose
/*	STDERR:	(Terminate Run, not visible) EdRun_Compile -> MIO_Init_Free -> MIO_CloseAllRunWindows -> MyCloseRunWindows -> MIOWin_CloseWindow -> MDIOWin_WindowProcedure (WM_DESTROY) -> MIOWindow_RegisterClose
/*	File:	(close:) Language_Execute_CloseFile -> MIO_RegisterClose
/*		(Terminate Run) Language_Execute_CloseAllFiles -> MIO_RegisterClose
/*	Window:	(Terminate Run, visible) EdRun_Compile -> MIO_Init_Free -> MIO_CloseAllRunWindows -> MyCloseRunWindows -> MIOWin_CloseWindow -> MDIOWin_WindowProcedure (WM_DESTROY) -> MIOWindow_RegisterClose
/*		(Terminate Run, not visible) MIO_Finalize_Run -> MyCloseRunWindows -> MIOWin_CloseWindow -> MDIOWin_WindowProcedure (WM_DESTROY) -> MIOWindow_RegisterClose
/*		(Window.Close) [Window.Close] -> Language_Execute_CloseFile -> TL_TLI_TLICL -> MyTuringFclose -> MIOWin_CloseWindow -> MDIOWin_WindowProcedure (WM_DESTROY) -> MIOWindow_RegisterClose
/*		Window.Close:	close : winId
/*				win_registerclose (winId)
/*	Net:	(Net.CloseConnection) Language_Execute_CloseFile -> MIO_RegisterClose
/*		Net.CloseConnection:	close: netId
/*					net_registerclose(netId)
/*		** Note MIONet_RegisterClose doesn't actually notify debugger
/*		** because debugger already notified in "close:"
/*		(Terminate Run) Language_Execute_CloseAllFiles -> MIO_RegisterClose
/*	Font:	(Font.Free) [Font.Free] -> MIO_font_free -> MIOFont_Free -> MIO_IDRemove
/*		(Terminate Run) MIO_Finalize_Run -> MIO_IDFree -> MIOFont_Free -> MIO_IDRemove
/*	Dir:	(Dir.Close) [Dir.Close] -> MIO_dir_close -> MIODir_Close -> MIO_IDRemove
/*		(Terminate Run) MIO_Finalize_Run -> MIO_IDFree -> MIODir_Free -> MIO_IDRemove
/*	Pic:	(Pic.Free) [Pic.Free] -> MIO_pic_free -> MIOPic_Free -> MIO_IDRemove
/*		(Terminate Run) MIO_Finalize_Run -> MIO_IDFree -> MIOPic_Free -> MIO_IDRemove
/*	Sprite:
/*	Lexer:								*/
/************************************************************************/
void	DbgAlloc_Free (int pmID)
{
    ObjectPtr	myPtr = stHead, myPrev = NULL, myOtherPtr;
    BOOL	myMatch = FALSE;
    LVFINDINFO	myFindInfo;
    int		myIndex, myOtherID;
    
    while (myPtr != NULL)
    {
    	if ((pmID == myPtr -> ID) && (!myPtr -> freed))
    	{
    	    myMatch = TRUE;
	    break;    		
    	}
    	myPrev = myPtr;
    	myPtr = myPtr -> next;
    }
    
    if (!myMatch)
    {
    	EdFail_Fatal (IDS_FREED_ITEM_NOT_IN_LIST, __FILE__, __LINE__, 0, pmID);
    	return;
    }

    //
    // Okay, special case here.  Stdin and Stdout share a window, despite having
    // two associated identifiers.  If we're closing stdin or stdout and 
    // there exists a stdout/stdin with the same description, then close them
    // both.
    //
    if ((pmID == TURINGSTDIN) || (pmID == TURINGSTDOUT))
    {
    	if (pmID == TURINGSTDIN) myOtherID = TURINGSTDOUT;
    	else myOtherID = TURINGSTDIN;
    	
    	myOtherPtr = stHead;
    	
    	while (myOtherPtr != NULL)
    	{
    	    if ((myOtherID == myOtherPtr -> ID) && 
    	    	(strcmp (myPtr -> description, myOtherPtr -> description) == 0))
    	    {
    	    	// Now, we need to stop this from beginning an infinite 
    	    	// recursion by changing the description.
    	    	free (myOtherPtr -> description);
    	    	myOtherPtr -> description = calloc (1, 1);
    	    	DbgAlloc_Free (myOtherID);
	    	break;    		
    	    }
    	    myOtherPtr = myOtherPtr -> next;
    	}
    }
    
    // If we've finished running the program, don't delete the object 
    // because it's Turing doing the auto cleanup and we may want to 
    // examine resources left open.  Instead, just mark it as freed.
    if (!stReallyFree)
    {
	myPtr -> freed = TRUE;
	return;
    }

    //
    // Remove the item from the ListView
    //
    
    // Find the item's location in the list view
    myFindInfo.flags = LVFI_PARAM;
    myFindInfo.lParam = (LPARAM) myPtr;
    myIndex = ListView_FindItem (stListView, -1, &myFindInfo);
    if (myIndex == -1)
    {
	EdGUI_Message1 (stDebuggerAllocWindow, MB_ICONEXCLAMATION, 
	    IDS_DEBUGGER_FAILURE_TITLE, IDS_CANT_FIND_LIST_ITEM,
	    stObjectTypes [MyGetImageNumber (myPtr -> objectType)],
    	    GetLastError ());
    }
    else
    {
    	// Delete the item from the list view
    	ListView_DeleteItem (stListView, myIndex);
    }

    //
    // Remove object from the linked list
    //
    if (myPrev == NULL)
    {
	stHead = myPtr -> next;
    }
    else
    {
	myPrev -> next = myPtr -> next;
    }
    free (myPtr -> description);
    free (myPtr);
} // DbgAlloc_Free


/************************************************************************/
/* DbgAlloc_Init_Run							*/
/*									*/
/* Free any entries that have the 'freed' flag set.  This is necessary	*/
/* because the system normally frees all open streams as soon as the	*/
/* program terminates.  Unfortunately, we may want to examine all the	*/
/* resources that were used by the program and weren't properly closed.	*/
/* So, DbgAlloc_Finalize_Run is called when a program finishes to flag	*/
/* Dbg_Alloc not to actually get rid of entries that have been		*/
/* deallocated by the system, but instead flag them for future deletion.*/
/* DbgAlloc_Init_Run actually frees all the records that have been	*/
/* marked as freed.  Note, the reason it doesn't just immediately free	*/
/* everything is as a check to make certain that all resources are	*/
/* actually freed by the system.
/************************************************************************/
void	DbgAlloc_Init_Run (void)
{
    ObjectPtr	myPtr = stHead, myPrev;
    
    while (myPtr != NULL)
    {
        EdFail_AddMessage (NORMAL, "Deallocating object %d (free=%d)", 
        		   myPtr -> ID, myPtr -> freed);

//#ifdef NOT_MINISTRY_VERSION
	// This is bug checking code, but it is not in the ministry version
    	if (!myPtr -> freed)
    	{
    	    EdFail_Fatal (IDS_NOT_ALL_ITEMS_IN_LIST_FREED, __FILE__, __LINE__, 
    	    		  0, myPtr -> ID);
    	}
//#endif // #ifdef NOT_MINISTRY_VERSION

	myPrev = myPtr;
    	myPtr = myPtr -> next;
        free (myPrev -> description);
        free (myPrev);
    }

    stHead = NULL;

    // Delete the item from the list view
    ListView_DeleteAllItems (stListView);

    stReallyFree = TRUE;
} // DbgAlloc_Init_Run


/************************************************************************/
/* DbgAlloc_IsWindowVisible						*/
/************************************************************************/
BOOL	DbgAlloc_IsWindowVisible (void)
{
    return stDebuggerAllocVisible;
} // DbgAlloc_IsWindowVisible


/************************************************************************/
/* DbgAlloc_ToggleWindowVisibility					*/
/************************************************************************/
void	DbgAlloc_ToggleWindowVisibility (void)
{
    if (stDebuggerAllocVisible)
    {
        ShowWindow (stDebuggerAllocWindow, SW_HIDE);
    }
    else
    {
        ShowWindow (stDebuggerAllocWindow, SW_SHOWNORMAL);
    }

    stDebuggerAllocVisible = !stDebuggerAllocVisible;
} // DbgAlloc_ToggleWindowVisibility


/******************************/
/* Static callback procedures */
/******************************/
/************************************************************************/
/* MyDebugAllocWindowProcedure						*/
/*									*/
/* Callback procedure for the debugger allocated objects window.	*/
/************************************************************************/
static LRESULT CALLBACK MyDebugAllocWindowProcedure (HWND pmWindow, 
						     UINT pmMessage, 
	    					     WPARAM pmWParam, 
	    					     LPARAM pmLParam)
{
    RECT	myRect;
    
    switch (pmMessage)
    {
    	case WM_CREATE:
    	    MyCreateAndPlaceListView (pmWindow);
    	    DbgMain_LocateWindow (pmWindow, DEFAULT_WIDTH, DEFAULT_HEIGHT);
    	    return 0;
    	    
        case WM_CLOSE:
            // Since the window must be visible to close it, it will hide 
            // the window.
            DbgAlloc_ToggleWindowVisibility ();
            return 0;

	case WM_SIZE:
	    GetClientRect (pmWindow, &myRect);
	    SetWindowPos (stListView, HWND_TOP, myRect.left, myRect.top,
	    		  myRect.right - myRect.left, 
	    		  myRect.bottom - myRect.top, 0);
	    // Resize the description column to fit the window.
	    ListView_SetColumnWidth (stListView, 3, LVSCW_AUTOSIZE_USEHEADER);
	    return 0;
	    
	case WM_NOTIFY:
	    MyNotifyHandler (pmWindow, pmMessage, pmWParam, pmLParam);
	    break;
    } // switch
    
    return DefWindowProc (pmWindow, pmMessage, pmWParam, pmLParam);
} // MyDebugAllocWindowProcedure


/************************************************************************/
/* MyListViewCompareProc						*/
/************************************************************************/
static LRESULT CALLBACK MyListViewCompareProc (LPARAM pmParam1, LPARAM pmParam2,
					       LPARAM pmParamSort)
{
    ObjectPtr	myObject1 = (ObjectPtr) (pmParam1);
    ObjectPtr	myObject2 = (ObjectPtr) (pmParam2);
    char	myObject1PathName [1024], myObject2PathName [1024];
    char	*myObject1CompName, *myObject2CompName;
    int		myCompare;
    LRESULT	myResult = 0;
    
    if (!myObject1 || !myObject2)
    {
    	return 0;
    }

    if (pmParamSort == 2)
    {
	if (myObject1 -> srcPos.fileNo == 0)
	{
	    myCompare = -1;
	}
	else if (myObject2 -> srcPos.fileNo == 0)
	{
	    myCompare = 1;
	}
	else
	{
	    FileManager_FileName (myObject1 -> srcPos.fileNo, 
		myObject1PathName);
	    FileManager_FileName (myObject2 -> srcPos.fileNo, 
		myObject2PathName);
	    if (!gProperties.useFullPathNames)
	    {
		myObject1CompName = strrchr (myObject1PathName, '\\');
		if (myObject1CompName != NULL) myObject1CompName++;
		else myObject1CompName = myObject1PathName;				
		myObject2CompName = strrchr (myObject2PathName, '\\');
		if (myObject2CompName != NULL) myObject2CompName++;
		else myObject2CompName = myObject2PathName;				
	    }
	    myCompare = strcmp (myObject1CompName, myObject2CompName);
	}
	if ((myCompare < 0) ||
	    ((myCompare == 0) &&
	    (myObject1 -> srcPos.lineNo < myObject2 -> srcPos.lineNo)))
	{
	    return -1;
	}
	else if((myCompare == 0) &&
	    (myObject1 -> srcPos.lineNo == myObject2 -> srcPos.lineNo))
	{
	    return 0;
	}
	else
	{
	    return 1;
	}
    }
    else
    {
	if ((myObject1 -> objectType < myObject2 -> objectType) ||
	    ((myObject1 -> objectType == myObject2 -> objectType) &&
	    (myObject1 -> ID < myObject2 -> ID)))
	{
	    return -1;
	}
	else if ((myObject1 -> objectType == myObject2 -> objectType) &&
	    (myObject1 -> ID == myObject2 -> ID))
	{
	    return 0;
	}
	else
	{
	    return 1;
	}
    }
} // MyListViewCompareProc


/*********************/
/* Static procedures */
/*********************/
/************************************************************************/
/* MyCreateAndPlaceListView						*/
/************************************************************************/
static void	MyCreateAndPlaceListView (HWND pmParentWindow)
{
    RECT	myParentRect;
    LVCOLUMN	myCol;
    char	myColTitle [1024];
    int		myWidth, myHeight;
    int		cnt;
        
    GetClientRect (pmParentWindow, &myParentRect);
    myWidth = myParentRect.right - myParentRect.left;
    myHeight = myParentRect.bottom - myParentRect.top;
    stListView = CreateWindowEx (0, WC_LISTVIEW, "", 
    				 WS_VISIBLE | WS_CHILD | LVS_REPORT | LVS_SINGLESEL,
    				 myParentRect.left, myParentRect.top,
    				 myWidth, myHeight, pmParentWindow, NULL, 
    				 gProgram.applicationInstance, NULL);
    if (stListView == NULL)
    {
   	return;
    }
    
    // Add full column
    ListView_SetExtendedListViewStyle (stListView, LVS_EX_FULLROWSELECT);
   
    // Set the list of images used by the list view
    ListView_SetImageList (stListView, stImageList, LVSIL_SMALL);
   
    // Set the columns used by the list view.
    myCol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    myCol.fmt = LVCFMT_LEFT;
    myCol.pszText = myColTitle;
    
    // The columns are: Type (File, Window, Net, Font, Dir, Pic, Sprite, Lexer), 
    //			ID, (up to 5 digits)
    //			File and Line
    //			Description 
    for (cnt = 0 ; cnt < NUM_COLUMNS ; cnt++)
    {
    	myCol.iSubItem = cnt;
    	myCol.cx = stColumnSizes [cnt];
    	EdGUI_LoadString (IDS_DEBUG_ALLOC_COL_NAME + cnt, myColTitle, 
    			  sizeof (myColTitle));
	if (ListView_InsertColumn (stListView, cnt, &myCol) == -1)
	{
	    EdFail_Warn (IDS_CANTINSERTCOLUMNFAIL, __FILE__, __LINE__, 
		         GetLastError ());
	    stListView = NULL;
	    return;
	}    			 
    }
    ListView_SetColumnWidth (stListView, 3, LVSCW_AUTOSIZE_USEHEADER);
} // MyCreateAndPlaceListView
 
 
/************************************************************************/
/* MyGetImageNumber							*/
/************************************************************************/
static int	MyGetImageNumber (int pmObjectType)
{
    int	cnt;
    
    for (cnt = 0 ; cnt < stNumObjectTypes ; cnt++)
    {
    	if (pmObjectType == stIconLookup [cnt])
    	{
    	    return cnt;
    	}
    }
    return stNumObjectTypes;
} // MyGetImageNumber


/************************************************************************/
/* MyNotifyHandler							*/
/************************************************************************/
static void	MyNotifyHandler (HWND pmWindow, UINT pmMessage, WPARAM pmWParam, 
	    			 LPARAM pmLParam)
{
    NMLVDISPINFO	*myDispInfo = (NMLVDISPINFO *) pmLParam;
    NMLISTVIEW		*myListView = (NMLISTVIEW *) pmLParam;
    LVITEM		myItem;
    ObjectPtr		myObject = (ObjectPtr) (myDispInfo -> item.lParam);
    char		myPathName [1024], myMessage [1024];
    int			myObjectIndex;
    static char		myStString [1024];
    
    switch (myDispInfo -> hdr.code)
    {
    	case LVN_GETDISPINFO:
    	    switch (myDispInfo -> item.iSubItem)
    	    {
    	    	case 0:
    	    	    myObjectIndex = MyGetImageNumber (myObject -> objectType);
    		    myDispInfo -> item.pszText = stObjectTypes [myObjectIndex];
    	    	    break;
    	    	case 1:
    	    	    wsprintf (myStString, "%d", myObject -> ID);
    		    myDispInfo -> item.pszText = myStString;
    	    	    break;
    	    	case 2:
    	    	    if (myObject -> srcPos.fileNo == 0)
    	    	    {
    	    	    	myDispInfo -> item.pszText = "During Initialization";
    	    	    }
    	    	    else
    	    	    {
    	    	    	char	*myPtr = myPathName;
    	    	    	FileManager_FileName (myObject -> srcPos.fileNo,
    	    	    			      myPathName);
			if (!gProperties.useFullPathNames)
			{
			    myPtr = strrchr (myPathName, '\\');
			    if (myPtr != NULL) myPtr++;
			    else myPtr = myPathName;				
			}
			wsprintf (myStString, "%s - Line %d", myPtr,
				  myObject -> srcPos.lineNo);
			    				    
    		    	myDispInfo -> item.pszText = myStString;
    		    }
    		    break;
    		case 3:
    		    myDispInfo -> item.pszText = myObject -> description;
    		    break;
    	    } // switch
    	    break;
    	case LVN_COLUMNCLICK:
	    if (myListView -> iSubItem == 2)
	    {
		stSortingCriteria = BY_LOCATION;
	    }
	    else
	    {
		stSortingCriteria = BY_ID;
	    }
    	    ListView_SortItems (stListView, MyListViewCompareProc, 
    	    			(LPARAM) (myListView -> iSubItem));
    	    break;
	case LVN_ITEMACTIVATE:
    	    myItem.mask = LVIF_PARAM;
    	    myItem.iSubItem = 0;
	    myItem.iItem = ListView_GetNextItem (stListView, -1, LVNI_SELECTED);
    	    if (!ListView_GetItem (stListView, &myItem))
    	    {
	    	MessageBeep (MB_ICONEXCLAMATION);
    	    	break;
    	    }
    	    myObject = (ObjectPtr) myItem.lParam;
	    myObjectIndex = MyGetImageNumber (myObject -> objectType);
	    if (myObject -> srcPos.fileNo == 0)
	    {
	    	MessageBeep (MB_ICONEXCLAMATION);
	    	break;
	    }	    	
  	    FileManager_FileName (myObject -> srcPos.fileNo, myPathName);
	    if (myObjectIndex <= 2)
	    {
	    	wsprintf (myMessage, "%s stream #%d opened here.", 
	    		  stObjectTypes [myObjectIndex], myObject -> ID);
	    }
	    else
	    {
	    	wsprintf (myMessage, "%s ID #%d allocated here.", 
	    		  stObjectTypes [myObjectIndex], myObject -> ID);
	    }
	    if (!EdErr_HilightLine (stDebuggerAllocWindow, myPathName, 
	    		            myObject -> srcPos.fileNo, 
	    		            myObject -> srcPos.lineNo, TRUE, myMessage))
	    {
	    	MessageBeep (MB_ICONEXCLAMATION);
	    }	    		            
	    break;
    } // switch
} // MyNotifyHandler
	    			 
