/**************/
/* edsearch.c */
/**************/

/*******************/
/* System includes */
/*******************/
#include <windows.h>
#include <dlgs.h>
#include <crtdbg.h>

/****************/
/* Self include */
/****************/
#include "edsearch.h"

/******************/
/* Other includes */
/******************/
#include "ed.h"
#include "edfail.h"
#include "edgui.h"

#include "edcommands.h"
#include "eddialogs.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
// Length of find/replace strings
#define FIND_STRING_LEN		256
#define UNINIT_VAL		-9999

/********************/
/* Global variables */
/********************/
UINT	EdSearch_FindReplaceMessage;
HWND	EdSearch_FindReplaceDialog;

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
// Properties of the status window
static EdSearch_Properties	stProperties;
static BOOL			stPropertiesChanged;

// 
static FINDREPLACE	stFindReplace;
static BOOL		stIsFindDialog;
static char		stFindWhat [FIND_STRING_LEN];
static char		stReplaceWith [FIND_STRING_LEN];
static HWND		stDialogOwner;

/******************************/
/* Static callback procedures */
/******************************/
static UINT CALLBACK 	MyFindDialogProcedure (HWND pmWindow, UINT pmMessage, 
    					       WPARAM pmWParam, 
    					       LPARAM pmLParam);

/*********************/
/* Static procedures */
/*********************/
static int	MyFindLiteral (TextPtr pmText, BOOL pmMatchCase, 
			       BOOL pmWholeWord, BOOL pmForward, BOOL pmWrap);
static BOOL	MyIsMatch (char *pmText, int pmPos, int pmLen, BOOL pmMatchCase, 
			   BOOL pmWholeWord);


/***********************/
/* External procedures */
/***********************/
/************************************************************************/
/* EdSearch_Init							*/
/************************************************************************/
BOOL	EdSearch_Init (void)
{
    /*************************************/
    /* Initialize find/replace structure */
    /*************************************/
    ZeroMemory (&stFindReplace, sizeof (FINDREPLACE));
    stFindReplace.lStructSize = sizeof (FINDREPLACE);
    stFindReplace.lpstrFindWhat = stFindWhat;
    stFindReplace.wFindWhatLen = FIND_STRING_LEN;
    stFindReplace.lpstrReplaceWith = stReplaceWith;
    stFindReplace.wReplaceWithLen = FIND_STRING_LEN;
    stFindReplace.lpfnHook = MyFindDialogProcedure;
    stFindReplace.Flags = FR_DOWN | FR_MATCHCASE | FR_ENABLEHOOK;
    
    EdSearch_FindReplaceMessage = RegisterWindowMessage (FINDMSGSTRING);
    if (EdSearch_FindReplaceMessage == 0)
    {
    	EdFail_Warn (IDS_REGISTERWINDOWMESSAGEFAIL, __FILE__, __LINE__, 
    			  GetLastError ());
    	return FALSE;
    }
    
    return TRUE;
} // EdSearch_Init


/************************************************************************/
/* EdSearch_Finalize							*/
/************************************************************************/
void	EdSearch_Finalize (void)
{
} // EdSearch_Finalize


/************************************************************************/
/* EdSearch_PropertiesSet						*/
/************************************************************************/
void	EdSearch_PropertiesSet (EdSearch_Properties pmProperties)
{
    stProperties = pmProperties;
    stPropertiesChanged = TRUE;
} // EdSearch_PropertiesSet

	
/************************************************************************/
/* EdSearch_PropertiesImplementChanges					*/
/************************************************************************/
void	EdSearch_PropertiesImplementChanges (void)
{
    stPropertiesChanged = FALSE;
} // EdSearch_PropertiesImplementChanges


/************************************************************************/
/* EdSearch_CloseDialog							*/
/************************************************************************/
void	EdSearch_CloseDialog (BOOL pmCloseWindowManually)
{
    if (pmCloseWindowManually)
    {
    	DestroyWindow (EdSearch_FindReplaceDialog);
    }
    EdSearch_FindReplaceDialog = NULL;
} // EdSearch_CloseDialog


/************************************************************************/
/* EdSearch_Find							*/
/************************************************************************/
int	EdSearch_Find (TextPtr pmText, BOOL pmForward, BOOL pmWrap)
{
    BOOL	mySearchDirection = stFindReplace.Flags & FR_DOWN;
    
    if (!pmForward)
    {
    	mySearchDirection = !mySearchDirection;
    }
    
    return MyFindLiteral (pmText, stFindReplace.Flags & FR_MATCHCASE, 
    	stFindReplace.Flags & FR_WHOLEWORD, mySearchDirection, pmWrap);
} // EdSearch_Find


/************************************************************************/
/* EdSearch_FindDialogVisible						*/
/************************************************************************/
BOOL	EdSearch_FindDialogVisible (void)
{
    return (EdSearch_FindReplaceDialog != NULL) && stIsFindDialog;
} // EdSearch_FindDialogVisible


/************************************************************************/
/* EdSearch_FindSelectionString						*/
/*									*/
/* Find the string currently selected.  Place the string in the find	*/
/* and search in the same direction as the FindReplace info indicates.	*/
/* Return NOT_FOUND, WRAPPED or FOUND depending on whether the string	*/
/* was not found, found with wrapping or found without wrapping		*/
/************************************************************************/
int	EdSearch_FindSelectionString (TextPtr pmText, char *pmSelectionString)
{
    strcpy (stFindWhat, pmSelectionString);
    return MyFindLiteral (pmText, TRUE, FALSE, stFindReplace.Flags & FR_DOWN,
    			  WRAP_ALLOWED);
} // EdSearch_FindSelectionString
    

/************************************************************************/
/* EdSearch_InsertReplaceText						*/
/************************************************************************/
int	EdSearch_InsertReplaceText (TextPtr pmTextPtr)
{
    return EdText_InsertText (pmTextPtr, stReplaceWith, strlen (stReplaceWith),
        TRUE, NULL);
} // EdSearch_InsertReplaceText


/************************************************************************/
/* EdSearch_IsFindStringEmpty						*/
/************************************************************************/
BOOL	EdSearch_IsFindStringEmpty (void)
{
    return (stFindWhat [0] == 0);
} // EdSearch_IsFindStringEmpty


/************************************************************************/
/* EdSearch_RepositionReplaceDialog					*/
/*									*/
/* Move the replace dialog so that it is not over top of the selection	*/
/* as specified by pmSelectRect. pmSelectRect is in screen coordinates.	*/
/************************************************************************/
void	EdSearch_RepositionReplaceDialog (HWND pmEditWindow, RECT pmSelectRect)
{
    RECT	myReplaceRect, myWindowRect, myScreenRect;
    int		myReplaceHeight, myReplaceWidth;
    
    // Get the window's rectangle    	
    GetWindowRect (EdSearch_FindReplaceDialog, &myReplaceRect);
    myReplaceHeight = myReplaceRect.bottom - myReplaceRect.top;
    myReplaceWidth = myReplaceRect.right - myReplaceRect.left;
    
    // Get the edit window rectangle
    GetWindowRect (pmEditWindow, &myWindowRect);
    
    // Get the screen's rectangle
    myScreenRect = EdGUI_GetWorkArea (); 		   	
    
    // First solution, move the Replace dialog to the right of the edit window
    if (myWindowRect.right + myReplaceWidth + 5 < myScreenRect.right)
    {
    	SetWindowPos (EdSearch_FindReplaceDialog, NULL, myWindowRect.right + 5,
            myReplaceRect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        return;
    }
    
    // Attempt 2, move the replace window as right as possible    	
    if (pmSelectRect.right + myReplaceWidth + 5 < myScreenRect.right)
    {
    	SetWindowPos (EdSearch_FindReplaceDialog, NULL, 
    	    myScreenRect.right - myReplaceWidth - 5, myReplaceRect.top, 
            0, 0, SWP_NOSIZE | SWP_NOZORDER);
	return;
    }	            

    // If there's more space above than below, place the Replace dialog
    // above the selection
    if (pmSelectRect.top - myScreenRect.top > myScreenRect.bottom - 
        pmSelectRect.bottom)
    {
    	// Can I place the Replace dialog just above the window
    	if (myWindowRect.top - myReplaceHeight - 5 > myScreenRect.top)
    	{
	    SetWindowPos (EdSearch_FindReplaceDialog, NULL, myReplaceRect.left,
	        myWindowRect.top - myReplaceHeight - 5, 0, 0, 
	        SWP_NOSIZE | SWP_NOZORDER);
	    return;
    	}
    		
    	// Will it fit above the selection?
    	if (pmSelectRect.top - myReplaceHeight - 5 < myScreenRect.top)
    	{
    	    // Won't fit above, wedge it along the top
	    SetWindowPos (EdSearch_FindReplaceDialog, NULL, myReplaceRect.left,
	        myScreenRect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	    return;
	}
	
	// Place it just above the selection
	SetWindowPos (EdSearch_FindReplaceDialog, NULL, myReplaceRect.left,
	    myScreenRect.top + (pmSelectRect.top - myScreenRect.top - 
	    myReplaceHeight) / 2, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	return;
    }
    else
    {
    	// Can I place the Replace dialog just below the window
    	if (myWindowRect.bottom + myReplaceHeight + 5 < myScreenRect.bottom)
    	{
	    SetWindowPos (EdSearch_FindReplaceDialog, NULL, myReplaceRect.left,
	        myWindowRect.bottom + 5, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	    return;
    	}
    		
    	// Will it fit below the selection?
    	if (pmSelectRect.bottom + myReplaceHeight + 5 > myScreenRect.bottom)
    	{
    	    // Won't fit below, wedge it along the top
	    SetWindowPos (EdSearch_FindReplaceDialog, NULL, myReplaceRect.left,
	        myScreenRect.bottom - myReplaceHeight, 0, 0, 
	        SWP_NOSIZE | SWP_NOZORDER);
	    return;
	}
	
	// Place it just below the selection
	SetWindowPos (EdSearch_FindReplaceDialog, NULL, myReplaceRect.left,
	    pmSelectRect.bottom + (myScreenRect.bottom - pmSelectRect.bottom - 
	    myReplaceHeight) / 2, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	return;
    }
} // EdSearch_RepositionReplaceDialog


/************************************************************************/
/* EdSearch_SelectionMatchesFindWhat					*/
/************************************************************************/
BOOL	EdSearch_SelectionMatchesFindWhat (TextPtr pmTextPtr)
{
    int		myPos;
    int		myLen;
    BOOL	myMatch;
    
    myMatch = FALSE;
    
    myPos = min (pmTextPtr -> selFrom, pmTextPtr -> selTo);
    myLen = max (abs (pmTextPtr -> selTo - pmTextPtr -> selFrom),
    		 (int) strlen (stFindWhat));
    
    if (stFindReplace.Flags & FR_MATCHCASE)
    {
    	if (strncmp (stFindWhat, &pmTextPtr -> text [myPos], myLen) == 0)
    	{
    	    myMatch = TRUE;
    	}
    }
    else
    {
    	if (_strnicmp (stFindWhat, &pmTextPtr -> text [myPos], myLen) == 0)
    	{
    	    myMatch = TRUE;
    	}
    }
    	
    if (myMatch && (stFindReplace.Flags & FR_WHOLEWORD))
    {
    	if (EdText_IsIdentChar (stFindWhat [0]))
    	{
    	    if ((myPos > 0) && 
    	        EdText_IsIdentChar (pmTextPtr -> text [myPos - 1]))
    	    {
    	    	myMatch = FALSE;
    	    }
    	}
    	if (EdText_IsIdentChar (stFindWhat [myLen - 1]))
    	{
    	    if ((myPos + myLen >= pmTextPtr -> textLen) || 
    	        EdText_IsIdentChar (pmTextPtr -> text [myPos + myLen]))
    	    {
    	    	myMatch = FALSE;
    	    }
    	}
    }
    	
    return myMatch;
} // EdSearch_SelectionMatchesFindWhat


/************************************************************************/
/* EdSearch_SetDialogOwner						*/
/************************************************************************/
void	EdSearch_SetDialogOwner (HWND pmTextDisplayWindow)
{
    if (EdSearch_FindReplaceDialog != NULL)
    {
    	// If the top level window is different from the current 
    	// owner window then destroy and recreate the window
    	HWND	myTopWindow;
    	
    	myTopWindow = pmTextDisplayWindow;
    	while (GetParent (myTopWindow) != NULL)
    	{
    	    myTopWindow = GetParent (myTopWindow);
    	}
    	if (GetWindow (EdSearch_FindReplaceDialog, GW_OWNER) != myTopWindow)
    	{
    	    // We must destroy and recreate the window
    	    if (stIsFindDialog)
    	    {
    	    	EdSearch_ShowFindDialog (pmTextDisplayWindow, NULL);
    	    }
    	    else
    	    {
    	    	EdSearch_ShowReplaceDialog (pmTextDisplayWindow, NULL);
	    }
	    return;
    	}
    }
    
    // Set the find replace window appropriately
    stFindReplace.hwndOwner = pmTextDisplayWindow;
} // EdSearch_SetDialogOwner


/************************************************************************/
/* EdSearch_ShowFindDialog						*/
/************************************************************************/
void	EdSearch_ShowFindDialog (HWND pmTextDisplayWindow, char *pmFindString)
{
    if (EdSearch_FindReplaceDialog != NULL)
    {
    	if ((stFindReplace.hwndOwner == pmTextDisplayWindow) && stIsFindDialog)
    	{
    	    // Bring the window to the front
    	    BringWindowToTop (EdSearch_FindReplaceDialog);
    	    return;
    	}
    	else
    	{
    	    // Close the current window
    	    DestroyWindow (EdSearch_FindReplaceDialog);
    	}
    }
    else
    {
    	if ((pmFindString != NULL) && (pmFindString [0] != 0))
    	{
    	    strcpy (stFindWhat, pmFindString);
    	}
    }
    stFindReplace.hwndOwner = pmTextDisplayWindow;
    stFindReplace.Flags &= 
        ~(FR_DIALOGTERM | FR_FINDNEXT | FR_REPLACE | FR_REPLACEALL); 
    EdSearch_FindReplaceDialog = FindText (&stFindReplace);
    stIsFindDialog = TRUE;
} // EdSearch_ShowFindDialog


/************************************************************************/
/* EdSearch_ShowReplaceDialog						*/
/************************************************************************/
void	EdSearch_ShowReplaceDialog (HWND pmTextDisplayWindow, 
				    char *pmFindString)
{
    if (EdSearch_FindReplaceDialog != NULL)
    {
    	if ((stFindReplace.hwndOwner == pmTextDisplayWindow) && !stIsFindDialog)
    	{
    	    // Bring the window to the front
    	    BringWindowToTop (EdSearch_FindReplaceDialog);
    	    return;
    	}
    	else
    	{
    	    // Close the current window
    	    DestroyWindow (EdSearch_FindReplaceDialog);
    	}
    }
    else
    {
    	if ((pmFindString != NULL) && (pmFindString [0] != 0))
    	{
    	    strcpy (stFindWhat, pmFindString);
    	}
    }

    stFindReplace.hwndOwner = pmTextDisplayWindow;
    stFindReplace.Flags &= 
        ~(FR_DIALOGTERM | FR_FINDNEXT | FR_REPLACE | FR_REPLACEALL); 
    EdSearch_FindReplaceDialog = ReplaceText (&stFindReplace);
    stIsFindDialog = FALSE;
} // EdSearch_ShowReplaceDialog
        	    

/******************************/
/* Static callback procedures */
/******************************/
/************************************************************************/
/* MyFindDialogProcedure						*/
/************************************************************************/
static UINT CALLBACK 	MyFindDialogProcedure (HWND pmWindow, UINT pmMessage, 
    					       WPARAM pmWParam,
    					       LPARAM pmLParam)
{
    static int	stLastX = UNINIT_VAL, stLastY = UNINIT_VAL;
    
    if (pmMessage == WM_INITDIALOG)
    {
	EdSearch_FindReplaceDialog = pmWindow;
	
    	if ((stLastX == UNINIT_VAL) && (stLastY == UNINIT_VAL))
    	{
	    EdGUI_CentreDialogBox (pmWindow);
	}
	else
	{
	    SetWindowPos (pmWindow, HWND_TOP, stLastX, stLastY, 
	        0, 0, SWP_NOSIZE);
	}
	
	return 1;
    }
    else if (pmMessage == WM_DESTROY)
    {
    	RECT	myDialogRect;
    	
    	GetWindowRect (pmWindow, &myDialogRect);
    	
    	stLastX = myDialogRect.left;
    	stLastY = myDialogRect.top;
    }
    else if (pmMessage == WM_ACTIVATE)
    {
        if (LOWORD (pmWParam) == WA_INACTIVE)
        {
            Ed_SetActiveWindow (NULL, NO_WINDOW);
            GetDlgItemText (pmWindow, edt1, stFindWhat, FIND_STRING_LEN);
            GetDlgItemText (pmWindow, edt2, stReplaceWith, FIND_STRING_LEN);
        }
        else
        {
            Ed_SetActiveWindow (pmWindow, FIND_REPLACE_DIALOG);
        }
        if (IsWindow (stFindReplace.hwndOwner))
        {
            RedrawWindow (stFindReplace.hwndOwner, NULL, NULL, 
            	RDW_INVALIDATE);
        }
    }
    else if (pmMessage == WM_COMMAND)
    {
    	if (HIWORD (pmWParam) == 1)
    	{
    	    if (LOWORD (pmWParam) == COMMAND_CLOSE_DIALOG)
    	    {
    	    	SendMessage (pmWindow, WM_CLOSE, 0, 0);
    	    }
    	    else
    	    {
    	    	SendMessage (stFindReplace.hwndOwner, WM_COMMAND, pmWParam, 0);
    	    }
    	    return 1;
    	}
/*    	
    	if (!stIsFindDialog && (LOWORD (pmWParam) == COMMAND_FIND))
    	{
    	    EdSearch_ShowFindDialog (stFindReplace.hwndOwner);
    	}
    	else if (stIsFindDialog && (LOWORD (pmWParam) == COMMAND_REPLACE))
    	{
    	    EdSearch_ShowReplaceDialog (stFindReplace.hwndOwner);
    	}
*/    	
    }

    return 0;
} // MyFindDialogProcedure


/*********************/
/* Static procedures */
/*********************/
/************************************************************************/
/* MyFindLiteral							*/
/************************************************************************/
static int	MyFindLiteral (TextPtr pmText, BOOL pmMatchCase, 
			       BOOL pmWholeWord, BOOL pmForward, BOOL pmWrap)
{
    int		myStartPos;
    int		myTextLen;
    int		myCurPos;
    int		myLen;
    BOOL	myMatch, myInitialMatch;

    // If there's no text to search, then no text was found!
    if (pmText -> textLen == 0)
    {
    	return NOT_FOUND;
    }
    
    myTextLen = pmText -> textLen;
    myStartPos = min (pmText -> selFrom, pmText -> selTo);
    if (myStartPos == myTextLen)
    {
	myStartPos = 0;
	if (!pmWrap) 
	{
	    return NOT_FOUND;
	}
    }
    myLen = strlen (stFindWhat);
    myMatch = FALSE;
    myInitialMatch = FALSE;
    
    if (MyIsMatch (pmText -> text, myStartPos, myLen, pmMatchCase, pmWholeWord)
        && (abs (pmText -> selTo - pmText -> selFrom) == myLen))
    {
	if (pmForward)
        {
	    myCurPos = (myStartPos + 1) % myTextLen;
        }
	else
        {
	    myCurPos = (myStartPos + myTextLen - 1) % myTextLen;
	}
	myInitialMatch = TRUE;
    }
    else
    {
	myCurPos = myStartPos;
    }

    do
    {
	myMatch = MyIsMatch (pmText -> text, myCurPos, myLen, pmMatchCase, 
			     pmWholeWord);
    	
    	if (myMatch)
    	{
    	    EdText_SetSelection1 (pmText, myCurPos, myCurPos + myLen);
    	    if (((myCurPos >= myStartPos) && pmForward) ||
    	    	((myCurPos <= myStartPos) && !pmForward))
    	    	return FOUND;
    	    else
    	    	return WRAPPED;
    	}
    	
        if (pmForward)
        {
            myCurPos = (myCurPos + 1) % myTextLen;

            if (!pmWrap && (myCurPos == 0))
            {
            	break;
            }
        }
        else
        {
            if (!pmWrap && (myCurPos == 0))
            {
            	break;
            }
        	
            myCurPos = (myCurPos + myTextLen - 1) % myTextLen;
        }
    } while (myCurPos != myStartPos);
    
    // If we've already selected the sole occurrence, give a message to
    // that effect
    if (myInitialMatch)
    {
    	return ONE_OCCURRENCE;
    }
    return NOT_FOUND;
} // MyFindLiteral


static BOOL	MyIsMatch (char *pmText, int pmPos, int pmLen, BOOL pmMatchCase, 
			   BOOL pmWholeWord)
{
    BOOL	myMatch = FALSE;
    
    if (pmMatchCase)
    {
    	if (strncmp (stFindWhat, &pmText [pmPos], pmLen) == 0)
    	{
    	    myMatch = TRUE;
    	}
    }
    else
    {
    	if (_strnicmp (stFindWhat, &pmText [pmPos], pmLen) == 0)
    	{
    	    myMatch = TRUE;
    	}
    }
    	
    if (myMatch && pmWholeWord)
    {
    	if (EdText_IsIdentChar (stFindWhat [0]))
    	{
    	    if ((pmPos > 0) && EdText_IsIdentChar (pmText [pmPos - 1]))
    	    {
    	    	myMatch = FALSE;
    	    }
    	}
    	if (EdText_IsIdentChar (stFindWhat [pmLen - 1]))
    	{
    	    if (EdText_IsIdentChar (pmText [pmPos + pmLen]))
    	    {
    	    	myMatch = FALSE;
    	    }
    	}
    }
	
    return myMatch;    
} // MyIsMatch