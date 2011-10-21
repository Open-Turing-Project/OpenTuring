/************************************************************************/
/* edmenu.c 								*/
/*									*/
/* This module holds routines that modify some of the menus in the	*/
/* editor window.  It also contains the routines to get a menu handle	*/
/* for those menus (this is needed as menu positions can change).	*/
/************************************************************************/

/*******************/
/* System includes */
/*******************/
#include <windows.h>
#include <crtdbg.h>

/****************/
/* Self include */
/****************/
#include "edmenu.h"

/******************/
/* Other includes */
/******************/
#include "ed.h"
#include "edfail.h"
#include "edfile.h"
#include "edgui.h"
#include "edprog.h"

#include "edcommands.h"
#include "eddialogs.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
#define NEW_ENTRY_STRING	"#ENTRY"
#define SUFFIX_STRING		"#SUFFIX"

// Position of the menus
#define FILE_MENU_POSITION		0
#define EDIT_MENU_POSITION		1
#define SEARCH_MENU_POSITION		2
#define MARK_MENU_POSITION		3
#define RUN_MENU_POSITION		4
#define DEBUGGER_MENU_POSITION		5
#define BASE_WINDOW_MENU_POSITION	5
#define BASE_HELP_MENU_POSITION		5

#define MENU_NOT_AVAILABLE		-1

/********************/
/* Global variables */
/********************/

/*********/
/* Types */
/*********/
typedef struct MenuEntry
{
    char		*menuItemName;
    char		*text;
    char		*suffix;
    int			textLen;
    struct MenuEntry	*next;
} MenuEntry, *MenuEntryPtr;

/**********************/
/* External variables */
/**********************/

/********************/
/* Static constants */
/********************/
static char		*stEmptyEntry = "";

/********************/
/* Static variables */
/********************/
static MenuEntryPtr	stNewMenuHeader = NULL, stInsertMenuHeader = NULL;
static char		*stNewMenuBuffer = NULL, *stInsertMenuBuffer = NULL;

// The position in the menu bar of the various menus
static int		stDebuggerMenuNumber, stWindowMenuNumber;
static int		stHelpMenuNumber;

/******************************/
/* Static callback procedures */
/******************************/

/*********************/
/* Static procedures */
/*********************/
static void	MyHideDebuggerMenu (HWND pmWindow, void *pmDummy);
static void	MyHideWindowMenu (HWND pmWindow, void *pmDummy);
static void	MyReadMenuFile (int pmFileNameResource,
				MenuEntryPtr *pmEntryHead, char **pmBuffer);
static void	MyShowDebuggerMenu (HWND pmWindow, void *pmDummy);
static void	MyShowWindowMenu (HWND pmWindow, void *pmDummy);

/***********************/
/* External procedures */
/***********************/
/************************************************************************/
/* EdMenu_Init								*/
/************************************************************************/
BOOL	EdMenu_Init (void)
{
    MyReadMenuFile (IDS_NEW_MENU, &stNewMenuHeader, &stNewMenuBuffer);
    MyReadMenuFile (IDS_INSERT_MENU, &stInsertMenuHeader, &stInsertMenuBuffer);
        
    stWindowMenuNumber = MENU_NOT_AVAILABLE;
    stDebuggerMenuNumber = MENU_NOT_AVAILABLE;
    stHelpMenuNumber = BASE_HELP_MENU_POSITION;
    
    return TRUE;
} // EdMenu_Init


/************************************************************************/
/* EdMenu_Finalize							*/
/************************************************************************/
void	EdMenu_Finalize (void)
{
    MenuEntryPtr	myMenuEntry, myNextMenuEntry;
    
    // Free up the new menu
    myMenuEntry = stNewMenuHeader;
    while (myMenuEntry != NULL)
    {			    	
    	myNextMenuEntry = myMenuEntry -> next;
    	free (myMenuEntry -> menuItemName);
    	free (myMenuEntry);
    	myMenuEntry = myNextMenuEntry;
    } // while (for each new menu item)
    if (stNewMenuBuffer != NULL)
    {
    	free (stNewMenuBuffer);
    }
    
    // Free up the insert menu
    myMenuEntry = stInsertMenuHeader;
    while (myMenuEntry != NULL)
    {			    	
    	myNextMenuEntry = myMenuEntry -> next;
    	free (myMenuEntry -> menuItemName);
    	free (myMenuEntry);
    	myMenuEntry = myNextMenuEntry;
    } // while (for each insert menu item)
    if (stInsertMenuBuffer != NULL)
    {
    	free (stInsertMenuBuffer);
    }
} // EdMenu_Finalize


/************************************************************************/
/* EdMenu_PropertiesSet							*/
/************************************************************************/
void	EdMenu_PropertiesSet (EdMenu_Properties pmProperties)
{
} // EdMenu_PropertiesSet


/************************************************************************/
/* EdMenu_PropertiesImplementChanges					*/
/************************************************************************/
void	EdMenu_PropertiesImplementChanges (void)
{
    if (gProperties.debuggerMenu)
    {
    	EdMenu_ShowHideDebuggerMenu (SHOW_MENU);
    }
    else
    {
    	EdMenu_ShowHideDebuggerMenu (HIDE_MENU);
    }
    if (gProperties.windowMenu)
    {
    	EdMenu_ShowHideWindowMenu (SHOW_MENU);
    }
    else
    {
    	EdMenu_ShowHideWindowMenu (HIDE_MENU);
    }
} // EdMenu_PropertiesImplementChanges


/************************************************************************/
/* EdMenu_AddDebuggerMenuItem						*/
/*									*/
/* Add the Show/Hide Debugger menu item to the Run menu.		*/
/************************************************************************/
void	EdMenu_AddDebuggerMenuItem (HWND pmEditWindow)
{
    HMENU	myRunMenu;
    char	myMenuItemName [256];
    	
    myRunMenu = EdMenu_GetMenu (pmEditWindow, RUN_MENU);

    if (stDebuggerMenuNumber == MENU_NOT_AVAILABLE)
    {
        EdGUI_LoadString (IDS_SHOW_DEBUGGER_MENU, myMenuItemName, 
            sizeof (myMenuItemName));
    }
    else
    {
        EdGUI_LoadString (IDS_HIDE_DEBUGGER_MENU, myMenuItemName, 
       	    sizeof (myMenuItemName));
    }
            
    if (gProgram.isJava)
    {
    	AppendMenu (myRunMenu, MF_SEPARATOR, 0, NULL);
    	AppendMenu (myRunMenu, MF_STRING, COMMAND_SHOW_HIDE_DEBUG_MENU, 
		    myMenuItemName);
    }
    else if (gProgram.isTuring)
    {
    	InsertMenu (myRunMenu, COMMAND_COMPILER_RESET, MF_BYCOMMAND | MF_STRING, 
    	    	    COMMAND_SHOW_HIDE_DEBUG_MENU, myMenuItemName);
    	InsertMenu (myRunMenu, COMMAND_COMPILER_RESET, 
    	    	    MF_BYCOMMAND | MF_SEPARATOR, 0, NULL);
    }
} // EdMenu_AddDebuggerMenuItem


/************************************************************************/
/* EdMenu_AddDebuggerWindowMenuIfNecessary				*/
/************************************************************************/
void	EdMenu_AddDebuggerWindowMenuIfNecessary (HWND pmEditWindow)
{
    if (stDebuggerMenuNumber != MENU_NOT_AVAILABLE)
    {
    	MyShowDebuggerMenu (pmEditWindow, NULL);
    }
    if (stWindowMenuNumber != MENU_NOT_AVAILABLE)
    {
    	MyShowWindowMenu (pmEditWindow, NULL);
    }
} // EdMenu_AddDebuggerWindowMenuIfNecessary


/************************************************************************/
/* EdMenu_AddHowToGetReadyMenuItem					*/
/************************************************************************/
void	EdMenu_AddHowToGetReadyMenuItem (HWND pmEditWindow)
{
    HMENU	myHelpMenu;
    char	myMenuItemName [256];
    	
    myHelpMenu = EdMenu_GetMenu (pmEditWindow, HELP_MENU);
    	
    if (gProgram.isJava)
    {
        EdGUI_LoadString (IDS_OBTAIN_READY_MENU_ITEM, myMenuItemName, 
            		  sizeof (myMenuItemName));
    	InsertMenu (myHelpMenu, 0, MF_BYPOSITION | MF_STRING, 
    	    	    COMMAND_HELP_READY_OBTAIN, myMenuItemName);
    }
    else if (gProgram.isTuring)
    {
        EdGUI_LoadString (IDS_OBTAIN_TURING_MENU_ITEM, myMenuItemName, 
            		  sizeof (myMenuItemName));
    	InsertMenu (myHelpMenu, 0, MF_BYPOSITION | MF_STRING, 
    	    	    COMMAND_HELP_TURING_OBTAIN, myMenuItemName);
    }
    InsertMenu (myHelpMenu, 1, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
} // EdMenu_AddHowToGetReadyMenuItem


/************************************************************************/
/* EdMenu_CreateNewInsertMenus						*/
/************************************************************************/
void	EdMenu_CreateNewInsertMenus (HMENU pmNewMenu, HMENU pmInsertMenu)
{
    MenuEntryPtr	myMenuEntry;
    int			myCommand;

    // Add the commands to the New menu    
    myCommand = COMMAND_NEW_BASE;
    myMenuEntry = stNewMenuHeader;
    while (myMenuEntry != NULL)
    {			    	
	// Add the item to the end
        AppendMenu (pmNewMenu, MF_STRING, myCommand, 
            myMenuEntry -> menuItemName);
    	myCommand++;
    	myMenuEntry = myMenuEntry -> next;
    } // while (for each new menu item)

    
    // Delete the dummy item in the Insert menu
    RemoveMenu (pmInsertMenu, 0, MF_BYPOSITION);
    
    // Add the commands to the Insert menu   
    myCommand = COMMAND_INSERT_BASE; 
    myMenuEntry = stInsertMenuHeader;
    while (myMenuEntry != NULL)
    {			    	
	// Add the item to the end
        AppendMenu (pmInsertMenu, MF_STRING, myCommand, 
            myMenuEntry -> menuItemName);
    	myCommand++;
    	myMenuEntry = myMenuEntry -> next;
    } // while (for each insert menu item)
} // EdMenu_CreateNewInsertMenus


/************************************************************************/
/* EdMenu_GetMenu							*/
/************************************************************************/
HMENU	EdMenu_GetMenu (HWND pmEditWindow, int pmMenu)
{
    HMENU	myMenuBar, myMenu;
    
    int myMenuPosition;
    
    if (pmMenu == FILE_MENU) myMenuPosition = FILE_MENU_POSITION;
    else if (pmMenu == EDIT_MENU) myMenuPosition = EDIT_MENU_POSITION;
    else if (pmMenu == SEARCH_MENU) myMenuPosition = SEARCH_MENU_POSITION;
    else if (pmMenu == MARK_MENU) myMenuPosition = MARK_MENU_POSITION;
    else if (pmMenu == RUN_MENU) myMenuPosition = RUN_MENU_POSITION;
    else if (pmMenu == DEBUGGER_MENU) myMenuPosition = stDebuggerMenuNumber;
    else if (pmMenu == WINDOW_MENU) myMenuPosition = stWindowMenuNumber;
    else if (pmMenu == HELP_MENU) myMenuPosition = stHelpMenuNumber;
    
    if (myMenuPosition == MENU_NOT_AVAILABLE)
    {
    	EdFail_Fatal (IDS_BAD_MENU, __FILE__, __LINE__, 0, pmMenu);
    	return NULL;
    }

    myMenuBar = GetMenu (pmEditWindow);
    myMenu = GetSubMenu (myMenuBar, myMenuPosition);
    
    return myMenu;
} // EdMenu_GetMenu


/************************************************************************/
/* EdMenu_GetName							*/
/************************************************************************/
const char	*EdMenu_GetName (BOOL pmNewMenu, int pmSkeleton)
{
    MenuEntryPtr	myMenuEntry;
    int			cnt;
    
    // Add the commands to the New menu
    if (pmNewMenu)
    {
    	myMenuEntry = stNewMenuHeader;
    }
    else
    {
    	myMenuEntry = stInsertMenuHeader;
    }

    for (cnt = 1 ; cnt < pmSkeleton ; cnt++)
    {
    	if (myMenuEntry == NULL)
    	{
    	    break;
    	}
    	myMenuEntry = myMenuEntry -> next;
    }

    if (myMenuEntry == NULL)
    {
    	EdFail_Fatal (IDS_NEW_SKELETON_NOT_FOUND, __FILE__, __LINE__, 0, 
    	    pmSkeleton);
    }
    
    return myMenuEntry -> menuItemName;
} // EdMenu_GetName


/************************************************************************/
/* EdMenu_GetText							*/
/************************************************************************/
void	EdMenu_GetText (BOOL pmNewMenu, int pmSkeleton, char **pmText, 
			char **pmFileSuffix, DWORD *pmTextLen)
{
    MenuEntryPtr	myMenuEntry;
    int			cnt;
    
    // Add the commands to the New menu
    if (pmNewMenu)
    {
    	myMenuEntry = stNewMenuHeader;
    }
    else
    {
    	myMenuEntry = stInsertMenuHeader;
    }

    for (cnt = 1 ; cnt < pmSkeleton ; cnt++)
    {
    	if (myMenuEntry == NULL)
    	{
    	    break;
    	}
    	myMenuEntry = myMenuEntry -> next;
    }

    if (myMenuEntry == NULL)
    {
    	EdFail_Warn (IDS_NEW_SKELETON_NOT_FOUND, __FILE__, __LINE__, 0, 
    	    pmSkeleton);
    }
    
    *pmText = myMenuEntry -> text;
    *pmFileSuffix = myMenuEntry -> suffix;
    *pmTextLen = myMenuEntry -> textLen;
} // EdMenu_GetText


/************************************************************************/
/* EdMenu_IsDebuggerMenuVisible						*/
/************************************************************************/
BOOL	EdMenu_IsDebuggerMenuVisible ()
{
    return (stDebuggerMenuNumber != MENU_NOT_AVAILABLE);
} // EdMenu_IsDebuggerMenuVisible


/************************************************************************/
/* EdMenu_RemoveWindowMenu						*/
/************************************************************************/
void	EdMenu_RemoveWindowMenu (HWND pmEditWindow)
{
	// TW
} // EdMenu_RemoveWindowMenu


/************************************************************************/
/* EdMenu_ShowHideDebuggerMenu						*/
/*									*/
/* Go through the list of editor windows and add or remove the debugger	*/
/* menu	to each of them.  (Add if not visible, remove if visible.	*/
/************************************************************************/
void	EdMenu_ShowHideDebuggerMenu (int pmShowHideToggle)
{
    int	myNewDebuggerMenuNumber;
    
    if (pmShowHideToggle == SHOW_MENU)
    {
    	myNewDebuggerMenuNumber = DEBUGGER_MENU_POSITION;
    }
    else if (pmShowHideToggle == HIDE_MENU)
    {
    	myNewDebuggerMenuNumber = MENU_NOT_AVAILABLE;
    }
    else if (pmShowHideToggle == TOGGLE_MENU)
    {
    	if (stDebuggerMenuNumber == MENU_NOT_AVAILABLE)
    	{
    	    myNewDebuggerMenuNumber = DEBUGGER_MENU_POSITION;
    	}
    	else
    	{
    	    myNewDebuggerMenuNumber = MENU_NOT_AVAILABLE;
    	}
    }    	    
    
    // If there's no change, return.
    if (myNewDebuggerMenuNumber == stDebuggerMenuNumber)
    {
    	return;
    }
        
    if (myNewDebuggerMenuNumber == MENU_NOT_AVAILABLE)
    {
    	Ed_EnumerateWindows (MyHideDebuggerMenu, NULL);
    	// Change stDebuggerMenuNumber after hiding the menus, because we use 
    	// stDebuggerMenuNumber to determine which menu to hide.
    	stDebuggerMenuNumber = myNewDebuggerMenuNumber;
    	// Shuffle the other menus to the left
    	if (stWindowMenuNumber != MENU_NOT_AVAILABLE)
    	{
    	    stWindowMenuNumber--;
    	}
    	stHelpMenuNumber--;
    }
    else
    {
    	// Change stDebuggerMenuNumber before showing the menus, because we use 
    	// stDebuggerMenuNumber to determine which menu to show.
    	stDebuggerMenuNumber = myNewDebuggerMenuNumber;
    	Ed_EnumerateWindows (MyShowDebuggerMenu, NULL);
    	// Shuffle the other menus to the right
    	if (stWindowMenuNumber != MENU_NOT_AVAILABLE)
    	{
    	    stWindowMenuNumber++;
    	}
    	stHelpMenuNumber++;
    }
} // EdMenu_ShowHideDebuggerMenu


/************************************************************************/
/* EdMenu_ShowHideWindowMenu						*/
/************************************************************************/
void	EdMenu_ShowHideWindowMenu (int pmShowHideToggle)
{
    int	myNewWindowMenuNumber;
    
    if (pmShowHideToggle == SHOW_MENU)
    {
    	myNewWindowMenuNumber = BASE_WINDOW_MENU_POSITION;
    	if (stWindowMenuNumber != MENU_NOT_AVAILABLE)
    	{
    	    myNewWindowMenuNumber++;
    	}
    }
    else if (pmShowHideToggle == HIDE_MENU)
    {
    	myNewWindowMenuNumber = MENU_NOT_AVAILABLE;
    }
    else if (pmShowHideToggle == TOGGLE_MENU)
    {
    	if (stDebuggerMenuNumber == MENU_NOT_AVAILABLE)
    	{
    	    myNewWindowMenuNumber = BASE_WINDOW_MENU_POSITION;
    	    if (stWindowMenuNumber != MENU_NOT_AVAILABLE)
    	    {
    	    	myNewWindowMenuNumber++;
    	    }
    	}
    	else
    	{
    	    myNewWindowMenuNumber = MENU_NOT_AVAILABLE;
    	}
    }    	    
    
    // If there's no change, return.
    if (myNewWindowMenuNumber == stWindowMenuNumber)
    {
    	return;
    }
    
    if (stWindowMenuNumber == MENU_NOT_AVAILABLE)
    {
    	Ed_EnumerateWindows (MyHideWindowMenu, NULL);
    	// Change stWindowMenuNumber after hiding the menus, because we use 
    	// stWindowMenuNumber to determine which menu to hide.
    	stWindowMenuNumber = myNewWindowMenuNumber;
    	// Shuffle the other menus to the left
    	stHelpMenuNumber--;
    }
    else
    {
    	// Change stWindowMenuNumber before showing the menus, because we use 
    	// stWindowMenuNumber to determine which menu to show.
    	stWindowMenuNumber = myNewWindowMenuNumber;
    	Ed_EnumerateWindows (MyShowWindowMenu, NULL);
    	// Shuffle the other menus to the right
    	stHelpMenuNumber++;
    }
} // EdMenu_ShowHideWindowMenu


/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/
/************************************************************************/
/* MyHideDebuggerMenu							*/
/************************************************************************/
static void	MyHideDebuggerMenu (HWND pmWindow, void *pmDummy)
{
    HMENU		myMenuBar = GetMenu (pmWindow);
    HMENU		myRunMenu = EdMenu_GetMenu (pmWindow, RUN_MENU);
    MENUITEMINFO	myMenuItemInfo;
    char		myNewMenuItemName [256];
    
    if (RemoveMenu (myMenuBar, stDebuggerMenuNumber, MF_BYPOSITION))
    {
	DrawMenuBar (pmWindow);
    }

    // Change the "Show/Hide Debugger Menu" appropriately
    myRunMenu = EdMenu_GetMenu (pmWindow, RUN_MENU);
    EdGUI_LoadString (IDS_SHOW_DEBUGGER_MENU, myNewMenuItemName, 
       	    	      sizeof (myNewMenuItemName));
 
    myMenuItemInfo.cbSize = sizeof (MENUITEMINFO);
    myMenuItemInfo.fMask = MIIM_TYPE;
    myMenuItemInfo.fType = MFT_STRING;
    myMenuItemInfo.dwTypeData = myNewMenuItemName;
    SetMenuItemInfo (myRunMenu, COMMAND_SHOW_HIDE_DEBUG_MENU, FALSE,
    		     &myMenuItemInfo);
} // MyHideDebuggerMenu


/************************************************************************/
/* MyHideWindowMenu							*/
/************************************************************************/
static void	MyHideWindowMenu (HWND pmWindow, void *pmDummy)
{
    HMENU		myMenuBar = GetMenu (pmWindow);
    
    if (RemoveMenu (myMenuBar, stWindowMenuNumber, MF_BYPOSITION))
    {
	DrawMenuBar (pmWindow);
    }
} // MyHideWindowMenu


/************************************************************************/
/* MyReadMenuFile							*/
/************************************************************************/
static void	MyReadMenuFile (int pmFileResource, MenuEntryPtr *pmEntryHead,
				char **pmBuffer)
{
    MenuEntryPtr	myLastEntry;
    FilePath		myMenuPathName;
    char		*myLine;
    DWORD		myFileSize;
    int			myLineNumber;
    
    myLastEntry = *pmEntryHead;
    
    EdFile_ConstructPath (myMenuPathName, IDS_INI_DIR, pmFileResource);    
        
    if (!EdFile_FileExists (myMenuPathName))
    {
    	return;
    }
        
    // Read file into buffer
    *pmBuffer = EdFile_ReadFileIntoBuffer (NULL, myMenuPathName, &myFileSize, 
    				           OK_IF_NOT_FOUND);
    if (*pmBuffer == NULL)
    {
    	return;
    }

    // The files are stored in order from most recent to least    
    EdFile_ReadLineFromBuffer (*pmBuffer, myFileSize, READ_FIRST);

    // Read a line from the file
    myLine = EdFile_ReadLineFromBuffer (*pmBuffer, 0, READ_NEXT);
    myLineNumber = 1;
    
    while (myLine != NULL)
    {
    	if (_strnicmp (myLine, NEW_ENTRY_STRING, strlen (NEW_ENTRY_STRING)) == 0)
    	{
    	    MenuEntryPtr	myNewEntry;
    	    char		*myPtr;
    	
    	    // Determine the length of the previous entry
    	    if (myLastEntry != NULL)
    	    {
    	    	if (myLastEntry -> text == NULL)
    	    	{
    	    	    myLastEntry -> text = stEmptyEntry;
    	    	    myLastEntry -> textLen = 0;
    	    	}
    	    	else
    	    	{
    	    	    myLastEntry -> textLen = myLine - myLastEntry -> text;
    	    	}
    	    }
    	    
    	    // Get the entry name
    	    myPtr = &myLine [strlen (NEW_ENTRY_STRING)];
    	    
    	    while ((*myPtr == ' ') || (*myPtr == '\t'))
    	    {
    	    	myPtr++;
    	    }
    	    
    	    // If no name, then warn and return
    	    if (*myPtr == 0)
    	    {
    	        EdFail_Warn (IDS_NO_MENU_NAME, __FILE__, __LINE__, 0, 
    	            myLineNumber, myMenuPathName);
    	        return;
    	    }
    	    
    	    // Allocate and fill menu entry.  If no memory, warn and return
    	    myNewEntry = (MenuEntryPtr) malloc (sizeof (MenuEntry));
    	    if (myNewEntry == NULL)
    	    {
    	        EdFail_Warn (IDS_NO_MEM_FOR_MENU, __FILE__, __LINE__, 0,
    	            myLineNumber, myMenuPathName);
    	        return;
    	    }
    	    
    	    // Allocate and fill menu item name
    	    myNewEntry -> menuItemName = malloc (strlen (myPtr) + 1);
    	    if (myNewEntry -> menuItemName == NULL)
    	    { 
    	        EdFail_Warn (IDS_NO_MEM_FOR_MENU, __FILE__, __LINE__, 0,
    	            myLineNumber, myMenuPathName);
    	        return;
    	    }
	    strcpy (myNewEntry -> menuItemName, myPtr);
	    
	    // Fill the text to NULL (will be filled when next line read)    	    
	    myNewEntry -> text = NULL;
	    myNewEntry -> suffix = NULL;
	    myNewEntry -> next = NULL;
	    		    
    	    // Add the entry to the end of the list
    	    if (myLastEntry == NULL)
    	    {
    	    	*pmEntryHead = myNewEntry;
    	    }
    	    else
    	    {
    	        myLastEntry -> next = myNewEntry;
    	    }
    	    myLastEntry = myNewEntry;
    	} // if #BEGIN
	else if (_strnicmp (myLine, SUFFIX_STRING, strlen (SUFFIX_STRING)) == 0)
	{
    	    // Get the entry name
    	    char    *myPtr = &myLine [strlen (SUFFIX_STRING)];
    	    
    	    while ((*myPtr == ' ') || (*myPtr == '\t'))
    	    {
    	    	myPtr++;
    	    }
    	    
    	    // If no name, then warn and return
    	    if (*myPtr == 0)
    	    {
    	        EdFail_Warn (IDS_MALFORMED_SUFFIX, __FILE__, __LINE__, 0, 
    	            myLineNumber, myMenuPathName);
    	        return;
    	    }
    	    
	    if (strlen (myPtr) > 10)
    	    {
    	        EdFail_Warn (IDS_MALFORMED_SUFFIX, __FILE__, __LINE__, 0, 
    	            myLineNumber, myMenuPathName);
    	        return;
    	    }

	    if (myLastEntry == NULL)
	    {
    	        EdFail_Warn (IDS_MALFORMED_SUFFIX, __FILE__, __LINE__, 0, 
    	            myLineNumber, myMenuPathName);
    	        return;
    	    }

    	    myLastEntry -> suffix = malloc (strlen (myPtr) + 1);
    	    if (myLastEntry -> suffix == NULL)
    	    { 
    	        EdFail_Warn (IDS_NO_MEM_FOR_MENU, __FILE__, __LINE__, 0,
    	            myLineNumber, myMenuPathName);
    	        return;
    	    }
	    strcpy (myLastEntry -> suffix, myPtr);
	} // if #SUFFIX
    	else
    	{
    	    if ((myLastEntry != NULL) && (myLastEntry -> text == NULL))
    	    {
    	    	myLastEntry -> text = myLine;
    	    	myLastEntry -> textLen = 0;
    	    }
    	}

        // Read a line from the file
        myLine = EdFile_ReadLineFromBuffer (*pmBuffer, 0, READ_NEXT);
        myLineNumber++;
    }
    
    if (myLastEntry != NULL)
    {
    	if (myLastEntry -> text == NULL)
    	{
    	    myLastEntry -> text = stEmptyEntry;
    	    myLastEntry -> textLen = 0;
    	}
	else if (myLastEntry -> textLen == 0)
	{
	    myLastEntry -> textLen = myFileSize - 
	        (myLastEntry -> text - *pmBuffer);
	}
    }
} // MyReadMenuFile


/************************************************************************/
/* MyShowDebuggerMenu							*/
/************************************************************************/
static void	MyShowDebuggerMenu (HWND pmWindow, void *pmDummy)
{
    HMENU		myMenuBar, myRunMenu, myDebuggerMenu;
    MENUITEMINFO	myMenuItemInfo;
    char		myBuffer [50], myNewMenuItemName [256];
        
    // Insert the menu
    myDebuggerMenu = LoadMenu (gProgram.applicationInstance, 
    			       MAKEINTRESOURCE (EDIT_WINDOW_DEBUGGER_MENU));
    myMenuItemInfo.cbSize = sizeof (MENUITEMINFO);
    myMenuItemInfo.fMask = MIIM_DATA | MIIM_ID | MIIM_SUBMENU |MIIM_TYPE;
    myMenuItemInfo.fType = MFT_STRING;
    myMenuItemInfo.fState = MFS_DEFAULT;
    myMenuItemInfo.dwTypeData = myBuffer;
    myMenuItemInfo.cch = 40; // sizeof(myBuffer)

    if (GetMenuItemInfo (myDebuggerMenu, 0, TRUE, &myMenuItemInfo))
    {
    	myMenuBar = GetMenu (pmWindow);
        if (InsertMenuItem (myMenuBar, stDebuggerMenuNumber, TRUE, 
        		    &myMenuItemInfo))
        {
	    DrawMenuBar (pmWindow);
        }
    }   

    // Change the "Show/Hide Debugger Menu" appropriately
    myRunMenu = EdMenu_GetMenu (pmWindow, RUN_MENU);
    EdGUI_LoadString (IDS_HIDE_DEBUGGER_MENU, myNewMenuItemName, 
       	    	      sizeof (myNewMenuItemName));

    myMenuItemInfo.cbSize = sizeof (MENUITEMINFO);
    myMenuItemInfo.fMask = MIIM_TYPE;
    myMenuItemInfo.fType = MFT_STRING;
    myMenuItemInfo.dwTypeData = myNewMenuItemName;
    SetMenuItemInfo (myRunMenu, COMMAND_SHOW_HIDE_DEBUG_MENU, FALSE,
    		     &myMenuItemInfo);
} // MyShowDebuggerMenu


/************************************************************************/
/* MyShowWindowMenu							*/
/************************************************************************/
static void	MyShowWindowMenu (HWND pmWindow, void *pmDummy)
{
    HMENU		myMenuBar, myWindowMenu;
    MENUITEMINFO	myWindowMenuInfo;
    char		myBuffer [50];
        
    // Insert the menu
    myWindowMenu = LoadMenu (gProgram.applicationInstance, 
    			     MAKEINTRESOURCE (EDIT_WINDOW_WINDOW_MENU));
    myWindowMenuInfo.cbSize = sizeof (MENUITEMINFO);
    myWindowMenuInfo.fMask = MIIM_DATA | MIIM_ID | MIIM_SUBMENU |MIIM_TYPE;
    myWindowMenuInfo.fType = MFT_STRING;
    myWindowMenuInfo.fState = MFS_DEFAULT;
    myWindowMenuInfo.dwTypeData = myBuffer;
    myWindowMenuInfo.cch = 40; // sizeof(myBuffer)

    if (GetMenuItemInfo (myWindowMenu, 0, TRUE, &myWindowMenuInfo))
    {
    	myMenuBar = GetMenu (pmWindow);
        if (InsertMenuItem (myMenuBar, stWindowMenuNumber, TRUE, 
        		    &myWindowMenuInfo))
        {
	    DrawMenuBar (pmWindow);
        }
    }   
} // MyShowWindowMenu
