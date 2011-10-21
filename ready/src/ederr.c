/***********/
/* ederr.c */
/***********/

/*******************/
/* System includes */
/*******************/

/****************/
/* Self include */
/****************/
#include "ederr.h"

/******************/
/* Other includes */
/******************/
#include "ed.h"
#include "edrun.h"
#include "edtext.h"

#include "dbgctrl.h"

#include "eddialogs.h"

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
static ErrorPtr		stErrorHead;
static ErrorPtr		stCurrentError;

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
/* EdErr_Init								*/
/************************************************************************/
BOOL	EdErr_Init (void)
{
    stErrorHead = NULL;

    return TRUE;
} // EdErr_Init


/************************************************************************/
/* EdErr_Finalize							*/
/************************************************************************/
void	EdErr_Finalize (void)
{
    EdErr_ClearErrors ();    
} // EdErr_Finalize


// Other subprograms
/************************************************************************/
/* EdErr_AddError							*/
/*									*/
/* For Java, pmPathName always specifies the absolute path name of the	*/
/* file with the error in it (or the absolute path name of the pseudo	*/
/* path name).								*/
/*									*/
/* For Turing, pmPathName contains the absolute path name of the file	*/
/* if there is an associated file and "unnamed" if there isn't.		*/
/*									*/
/* What is stored in the error structure is the absolute path name if	*/
/* one is available, in which case 0 is stored for the Turing file	*/
/* number, or if there is no absolute path name, (i.e. the file was 	*/
/* unnamed) the Turing file number is stored.				*/
/************************************************************************/
BOOL     EdErr_AddError (const char *pmPathName, WORD pmTuringFileNo,
			 int pmLineFrom, int pmColFrom, 
			 int pmLineTo, int pmColTo, const char *pmErrorMessage)
{
    FilePath	myBackSlashPathName, myDisplayPath;
    ErrorPtr	myNewError, myErrorPtr, myPrevErrorPtr;
    TextPtr	myTextPtr;
    char	*myPtr;
    BOOL	myNewFile;

    //
    // Create the new error structure
    //
    myNewError = (ErrorPtr) malloc (sizeof (Error));
    if (myNewError == NULL)
    {
    	// TW Warn
    	return FALSE;
    }

    //
    // Set the file number to zero by default
    //
    myNewError -> turingFileNo = 0;

    //
    // Copy the path name 
    //
    strcpy (myBackSlashPathName, pmPathName);
    
    //
    // If this is a Turing error message, determine whether there is an 
    // associated file name (in which case use it), or not (in which case
    // use a short display name and set the turing file number.
    //
    if (pmTuringFileNo != 0)
    {
    	HWND 	myTextDisplayWindow;
    	
    	myTextDisplayWindow = EdRun_MatchTuringFileNo (pmTuringFileNo);
    	if (myTextDisplayWindow != NULL)
    	{
    	    if (EdDisp_GetPath (myTextDisplayWindow) == NULL)
    	    {
    	        strcpy (myBackSlashPathName,
    	        	EdDisp_GetPathForDisplay (myTextDisplayWindow, 
    	        				  USE_FILENAME, 
						  DONT_SHOW_DIRTY_MARK,
						  myDisplayPath));
	    	myNewError -> turingFileNo = pmTuringFileNo;
	    }						      
    	}
    }    

    //
    // Set the path name (absolute path for Java, display path for Turing)
    //
    myNewError -> pathName = malloc (strlen (myBackSlashPathName) + 1);
    if (myNewError -> pathName == NULL)
    {
        // TW Warn
        return FALSE;
    }
        
    // Convert forward slashes to backward slashes
    myPtr = myBackSlashPathName;
    while ((myPtr = strchr (myPtr, '/')) != NULL)
    {
	*myPtr = '\\';
    }

    strcpy (myNewError -> pathName, myBackSlashPathName);

    //
    // Set the line and column
    //
    myNewError -> line = max (0, pmLineFrom - 1);
    if (pmColFrom == 0)
    {
	myNewError -> tokenPos = 0;
	myNewError -> tokenLen = 0;
    }
    else
    {
	myNewError -> tokenPos = pmColFrom - 1;
	
        if (pmLineFrom != pmLineTo)
	{
	    // We have a special provision for if we're missing a semicolon
	    if (strstr (pmErrorMessage, "\";\"") != 0)
	    {
		myNewError -> line = pmLineTo - 1;
		myNewError -> tokenPos = pmColTo;
	    }
    	    myNewError -> tokenLen = 0;
	}
	else
        {
	    myNewError -> tokenLen = pmColTo - pmColFrom + 1;
        }
    }
    
    //
    // Set the error message
    //
    myNewError -> errorMessage = malloc (strlen (pmErrorMessage) + 1);
    if (myNewError -> errorMessage == NULL)
    {
    	// TW Warn
    	return FALSE;
    }
    strcpy (myNewError -> errorMessage, pmErrorMessage);
    myNewError -> next = NULL;

    //
    // If the error message takes place in an open window, add it to the
    // window.
    //
    if (pmTuringFileNo == 0)
    {
    	myTextPtr = EdRun_MatchPath (myBackSlashPathName);
    }
    else
    {
    	HWND	myTextDisplayWindow = EdRun_MatchTuringFileNo (pmTuringFileNo);
    	
    	if (myTextDisplayWindow == NULL)
    	{
    	    myTextPtr = NULL;
    	}
    	else
    	{
    	    myTextPtr = EdDisp_GetTextPtr (myTextDisplayWindow);
    	}
    }
    if (myTextPtr != NULL)
    {
    	EdText_AddError (myTextPtr, myNewError);
    }
    
    // Add it to the end of the list, but bunch the same file together
    myErrorPtr = stErrorHead;
    myPrevErrorPtr = NULL;
    myNewFile = TRUE;
    while (myErrorPtr != NULL)
    {
    	if (_stricmp (myErrorPtr -> pathName, myBackSlashPathName) == 0)
    	{
    	    myNewFile = FALSE;
    	    
    	    // Now cycle through until a non-match
    	    while ((myErrorPtr != NULL) &&
    	           (_stricmp (myErrorPtr -> pathName, myBackSlashPathName) == 0))
    	    {
	    	myPrevErrorPtr = myErrorPtr;
	    	myErrorPtr = myErrorPtr -> next;
	    }
    	    break;
    	}
    	myPrevErrorPtr = myErrorPtr;
    	myErrorPtr = myErrorPtr -> next;
    }
    
    if (myPrevErrorPtr == NULL)
    {
    	myNewError -> next = stErrorHead;
    	stErrorHead = myNewError;
    }
    else
    {
    	myNewError -> next = myPrevErrorPtr -> next;
    	myPrevErrorPtr -> next = myNewError;
    }
    
    return myNewFile;
} // EdErr_AddError
			    
			    
/************************************************************************/
/* EdErr_ClearErrors							*/
/************************************************************************/
void	EdErr_ClearErrors (void)
{
    ErrorPtr	myPtr, myNext;
    
    myPtr = stErrorHead;
    while (myPtr != NULL)
    {
    	myNext = myPtr -> next;
    	if (myPtr -> pathName != NULL)
    	{
    	    free (myPtr -> pathName);
    	}
    	free (myPtr -> errorMessage);
    	free (myPtr);
    	myPtr = myNext;
    }
    
    Ed_EnumerateFiles (EdDisp_ClearErrors, NULL);
    
    stErrorHead = NULL;
    stCurrentError = NULL;
} // EdErr_ClearErrors


/************************************************************************/
/* EdErr_DeleteError							*/
/************************************************************************/
void	EdErr_DeleteError (ErrorPtr pmError)
{
    ErrorPtr	myError, myPrevError, myErrorToDelete;
    
    myError = stErrorHead;
    myPrevError = NULL;
    while (myError != NULL)
    {
    	if (myError == pmError)
    	{
    	    if (myPrevError == NULL)
    	    {
    	    	stErrorHead = myError -> next;
    	    	if (stCurrentError == myError)
    	    	{
    	    	    stCurrentError = NULL;
    	    	}
    	    }
    	    else
    	    {
		myPrevError -> next = myError -> next;
    	    	if (stCurrentError == myError)
    	    	{
    	    	    stCurrentError = myPrevError;
    	    	}
	    }
	    myErrorToDelete = myError;
    	    myError = myError -> next;
    	    if (myErrorToDelete -> pathName != NULL)
    	    {
	    	free (myErrorToDelete -> pathName);
	    }
	    free (myErrorToDelete -> errorMessage);
    	    free (myErrorToDelete);
    	}
    	else
    	{
    	    myPrevError = myError;
    	    myError = myError -> next;
    	}
    } // while
} // EdErr_DeleteError


/************************************************************************/
/* EdErr_DisplayNumberOfErrorsDialog					*/
/*									*/
/* Displays a dialog displaying the number of errors and warnings.	*/
/************************************************************************/
BOOL	EdErr_DisplayNumberOfErrorsDialog (HWND pmTextDisplayWindow, 
					   const char *pmCompiledPathName,
					   int pmNumErrors, int pmNumWarnings, 
					   int pmNumFiles, BOOL pmCompileOnly)
{
    // Display the first error, if any
    if ((pmNumErrors > 0) || (pmNumWarnings > 0))
    {
	EdErr_FindFirstError (pmTextDisplayWindow, NO_MESSAGE_IF_NO_ERROR,
			      ACTIVATE_SOURCE_WINDOW);
    }

    // Display a dialog box with the total number of errors
    if (pmNumFiles > 1)
    {    
    	if ((pmNumErrors > 0) && (pmNumWarnings > 0))
    	{
    	    int	myMessageNumber, myStatusNumber;

    	    if ((pmNumErrors > 1) && (pmNumWarnings > 1))
    	    {
    	    	myMessageNumber = IDS_ERRORS_AND_WARNINGS_MF_FOUND;
    	    	myStatusNumber = IDS_ERRORS_AND_WARNINGS_MF_STATUS;
    	    }
    	    else if ((pmNumErrors > 1) && (pmNumWarnings == 1))
    	    {
    	    	myMessageNumber = IDS_ERRORS_AND_WARNING_MF_FOUND;
    	    	myStatusNumber = IDS_ERRORS_AND_WARNING_MF_STATUS;
    	    }
    	    else if ((pmNumErrors == 1) && (pmNumWarnings > 1))
    	    {
    	    	myMessageNumber = IDS_ERROR_AND_WARNINGS_MF_FOUND;
    	    	myStatusNumber = IDS_ERROR_AND_WARNINGS_MF_STATUS;
    	    }
    	    else
    	    {
    	    	myMessageNumber = IDS_ERROR_AND_WARNING_MF_FOUND;
    	    	myStatusNumber = IDS_ERROR_AND_WARNING_MF_STATUS;
    	    }
    	    EdGUI_Message1 (GetForegroundWindow (), 0, IDS_ERRORS_FOUND_TITLE, 
    	    		    myMessageNumber, pmNumErrors, pmNumWarnings, 
    	    		    pmNumFiles);
//    	    EdWin_ShowStatus1 (pmTextDisplayWindow, myStatusNumber, pmNumErrors, 
//    	    		       pmNumWarnings, pmNumFiles, 
//    	    		       EdDisp_GetPath (pmTextDisplayWindow));
    	    return FALSE;
        }
    
        if (pmNumErrors > 0)
        {
    	    int	myMessageNumber, myStatusNumber;
    	
    	    if (pmNumErrors > 1)
    	    {
    	    	myMessageNumber = IDS_ERRORS_MF_FOUND;
    	    	myStatusNumber = IDS_ERRORS_MF_STATUS;
    	    }
    	    else
    	    {
    	    	// Assert FALSE!  You can't have 1 error over more than 1 file!
    	    	myMessageNumber = IDS_ERROR_MF_FOUND;
    	    	myStatusNumber = IDS_ERROR_MF_STATUS;
    	    }
    	    EdGUI_Message1 (GetForegroundWindow (), 0, IDS_ERRORS_FOUND_TITLE, 
    	    		    myMessageNumber, pmNumErrors, pmNumFiles);
//    	    EdWin_ShowStatus1 (pmTextDisplayWindow, myStatusNumber, pmNumErrors, 
//    	    		       pmNumFiles, pmCompiledPathName);

    	    return FALSE;
    	}
    
    	if (pmNumWarnings > 0)
    	{
    	    int	myMessageNumber, myMessageNumberNoAsk, myStatusNumber, myResult;
    	
    	    if (pmNumWarnings > 1)
    	    {
    	    	myMessageNumber = IDS_WARNINGS_MF_FOUND;
    	    	myMessageNumberNoAsk = IDS_WARNINGS_MF_NO_ASK_FOUND;
    	    	myStatusNumber = IDS_WARNINGS_MF_STATUS;
    	    }
    	    else
    	    {
    	    	// Assert FALSE!  You can't have 1 error over more than 1 file!
    	    	myMessageNumber = IDS_WARNING_MF_FOUND;
    	    	myMessageNumberNoAsk = IDS_WARNING_MF_NO_ASK_FOUND;
    	    	myStatusNumber = IDS_WARNING_MF_STATUS;
    	    }
//    	    EdWin_ShowStatus1 (pmTextDisplayWindow, myStatusNumber, 
//    	    		       pmNumWarnings, pmNumFiles, pmCompiledPathName);
    	    if (pmCompileOnly)
    	    {
    	    	EdGUI_Message1 (GetForegroundWindow (), 0, 
    	    			IDS_ERRORS_FOUND_TITLE, 
    	    			myMessageNumberNoAsk, pmNumWarnings, 
    	    			pmNumFiles);
	    	return FALSE;
    	    }
    	    else
    	    {
    	    	myResult = EdGUI_Message1 (GetForegroundWindow (), 
    	    		   	MB_OKCANCEL | MB_ICONWARNING, 
	    	           	IDS_ERRORS_FOUND_TITLE, myMessageNumber, 
	    	           	pmNumWarnings, pmNumFiles);
    	    	if (myResult == IDCANCEL)
    	    	{
	    	    return FALSE;
    	        }
    	    }
        }
    } // if (pmNumFiles > 1)
    else
    {
    	if ((pmNumErrors > 0) && (pmNumWarnings > 0))
    	{
    	    int	myMessageNumber, myStatusNumber;
    	
    	    if ((pmNumErrors > 1) && (pmNumWarnings > 1))
    	    {
    	    	myMessageNumber = IDS_ERRORS_AND_WARNINGS_FOUND;
    	    	myStatusNumber = IDS_ERRORS_AND_WARNINGS_STATUS;
    	    }
    	    else if ((pmNumErrors > 1) && (pmNumWarnings == 1))
    	    {
    	    	myMessageNumber = IDS_ERRORS_AND_WARNING_FOUND;
    	    	myStatusNumber = IDS_ERRORS_AND_WARNING_STATUS;
    	    }
    	    else if ((pmNumErrors == 1) && (pmNumWarnings > 1))
    	    {
    	    	myMessageNumber = IDS_ERROR_AND_WARNINGS_FOUND;
    	    	myStatusNumber = IDS_ERROR_AND_WARNINGS_STATUS;
    	    }
    	    else
    	    {
    	    	myMessageNumber = IDS_ERROR_AND_WARNING_FOUND;
    	    	myStatusNumber = IDS_ERROR_AND_WARNING_STATUS;
    	    }
    	    EdGUI_Message1 (GetForegroundWindow (), 0, IDS_ERRORS_FOUND_TITLE, 
    	    		    myMessageNumber, pmNumErrors, pmNumWarnings);
//    	    EdWin_ShowStatus1 (pmTextDisplayWindow, myStatusNumber, pmNumErrors, 
//    	    		       pmNumWarnings, pmCompiledPathName);
    	    return FALSE;
        }
    
        if (pmNumErrors > 0)
        {
    	    int	myMessageNumber, myStatusNumber;
    	
    	    if (pmNumErrors > 1)
    	    {
    	    	myMessageNumber = IDS_ERRORS_FOUND;
    	    	myStatusNumber = IDS_ERRORS_STATUS;
    	    }
    	    else
    	    {
    	    	myMessageNumber = IDS_ERROR_FOUND;
    	    	myStatusNumber = IDS_ERROR_STATUS;
    	    }
    	    EdGUI_Message1 (GetForegroundWindow (), 0, IDS_ERRORS_FOUND_TITLE, 
    	    		    myMessageNumber, pmNumErrors);
//    	    EdWin_ShowStatus1 (pmTextDisplayWindow, myStatusNumber, pmNumErrors, 
//    	    		       pmCompiledPathName);
    	    return FALSE;
    	}
    
    	if (pmNumWarnings > 0)
    	{
    	    int	myMessageNumber, myMessageNumberNoAsk, myStatusNumber, myResult;
    	
    	    if (pmNumWarnings > 1)
    	    {
    	    	myMessageNumber = IDS_WARNINGS_FOUND;
    	    	myMessageNumberNoAsk = IDS_WARNINGS_NO_ASK_FOUND;
    	    	myStatusNumber = IDS_WARNINGS_STATUS;
    	    }
    	    else
    	    {
    	    	myMessageNumber = IDS_WARNING_FOUND;
    	    	myMessageNumberNoAsk = IDS_WARNING_NO_ASK_FOUND;
    	    	myStatusNumber = IDS_WARNING_STATUS;
    	    }
//    	    EdWin_ShowStatus1 (pmTextDisplayWindow, myStatusNumber, 
//    	    		       pmNumWarnings, pmCompiledPathName);
    	    if (pmCompileOnly)
    	    {
    	    	EdGUI_Message1 (GetForegroundWindow (), 0, 
    	    			IDS_ERRORS_FOUND_TITLE, 
    	    			myMessageNumberNoAsk, pmNumWarnings);
	    	return FALSE;
    	    }
    	    else
    	    {
    	    	myResult = EdGUI_Message1 (GetForegroundWindow (), 
    	    		   	MB_OKCANCEL | MB_ICONWARNING, 
	    	           	IDS_ERRORS_FOUND_TITLE, myMessageNumber, 
	    	           	pmNumWarnings);
    	    	if (myResult == IDCANCEL)
    	    	{
	    	    return FALSE;
    	        }
    	    }
        }
    } // else (pmNumFiles > 1)
    
    EdWin_ShowStatus1 (pmTextDisplayWindow, IDS_SUCCESSFUL_COMPILE_STATUS, 
    		       pmCompiledPathName);
    
    return TRUE;
} // EdErr_DisplayNumberOfErrorsDialog


/************************************************************************/
/* EdErr_FindFirstError							*/
/*									*/
/* This jumps to the next error, opening files if necessary.		*/
/************************************************************************/
void	EdErr_FindFirstError (HWND pmTextDisplayWindow, BOOL pmMessageIfNoError,
			      BOOL pmActivateSourceWindow)
{
    stCurrentError = NULL;
    EdErr_FindNextError (pmTextDisplayWindow, pmMessageIfNoError,
			 pmActivateSourceWindow);
} // EdErr_FindFirstError


/************************************************************************/
/* EdErr_FindNextError							*/
/*									*/
/* This jumps to the next error, opening files if necessary.		*/
/************************************************************************/
void	EdErr_FindNextError (HWND pmTextDisplayWindow, BOOL pmMessageIfNoError,
			     BOOL pmActivateSourceWindow)
{
    HWND	myEditWindow, myTextDisplayWindow;	
//    const char	*myErrorMessage;

    // Debug message
    EdFail_AddMessage (NORMAL, "Find Next Error (1) - Text: %x - Activate: %d", 
        	       pmTextDisplayWindow, pmActivateSourceWindow);

    // Jump to next error
    if (stErrorHead == NULL)
    {
    	if (pmMessageIfNoError)
    	{
    	    EdWin_ShowStatus (pmTextDisplayWindow, "No Errors in File");
    	    MessageBeep (MB_ICONEXCLAMATION);
    	}
    	return;
    }

    if (stCurrentError != NULL)
    {
    	stCurrentError = stCurrentError -> next;
    }
    
    // If we haven't used this or we reached the end of the list, start at
    // the beginning.
    if (stCurrentError == NULL)
    {
    	stCurrentError = stErrorHead;
    }

    // If the original compiled file was unnamed, use the turing file number to
    // try and look it up.
    if (stCurrentError -> turingFileNo != 0)
    {
        EdDisp_GetWindowWithMatchingTuringFileNo (
        	stCurrentError -> turingFileNo, 
		&myEditWindow, &myTextDisplayWindow);
    }
    else
    {	    		
        EdDisp_GetWindowWithMatchingPathName (stCurrentError -> pathName, 
		&myEditWindow, &myTextDisplayWindow);
    }
    
    if (myTextDisplayWindow != NULL)
    {
	// If the window is iconic, expand it.
	if (IsIconic (myEditWindow))
	{
	    EdDisp_SetDisplayResizeMessage (FALSE);
	    ShowWindow (myEditWindow, SW_RESTORE);
	    EdDisp_SetDisplayResizeMessage (TRUE);
	}


    	if ((GetForegroundWindow () != myEditWindow) || 
    	    (myTextDisplayWindow != pmTextDisplayWindow))
    	{
	    // The next error is in an open window
	    if (pmActivateSourceWindow)
	    {
		BringWindowToTop (myEditWindow);
	    }
	    else
	    {
		Ed_PutWindowAboveAllOtherEdWindows (myEditWindow);
	    }
	    EdWin_ActivateTextDisplay (myEditWindow, myTextDisplayWindow);
	}
	
    	// Display the error in the current window
    	if ((stCurrentError -> tokenPos == 0) && 
    	    (stCurrentError -> tokenLen == 0))
	{
    	    EdDisp_GotoError (myTextDisplayWindow, stCurrentError -> line,
    	        -1, -1, stCurrentError -> errorMessage);
	}
	else
	{
    	    EdDisp_GotoError (myTextDisplayWindow, stCurrentError -> line,
    	        stCurrentError -> tokenPos, stCurrentError -> tokenLen, 
    	        stCurrentError -> errorMessage);
    	}
	
	return;
    }
    else
    {
    	BOOL	myFileOpened = FALSE;
    	
    	// If the turing file number is not zero, then we can't open it
    	if ((stCurrentError -> turingFileNo == 0) &&
	    (strcmp (stCurrentError -> pathName, "unnamed#?") != 0) &&
	    (strcmp (stCurrentError -> pathName, "<No file>") != 0))
    	{
    	    myFileOpened = EdWin_OpenFile (
    	    			EdGUI_GetTopWindow (pmTextDisplayWindow), 
			    	stCurrentError -> pathName, NULL,
			    	PURPOSE_DISPLAY_ERROR);
	}
	
	if (!myFileOpened)
	{
	    // Could not open a file by that name.  Display the error message
	    // in the current window instead.
    	    if ((strcmp (stCurrentError -> pathName, "unnamed#?") != 0) &&
		(strcmp (stCurrentError -> pathName, "<No file>") != 0))
	    {
		EdWin_ShowStatusBold (EdGUI_GetTopWindow (pmTextDisplayWindow),
		    "Line %d of \"%s\": %s", stCurrentError -> line + 1,
		    stCurrentError -> pathName, stCurrentError -> errorMessage);
	    }
	    else
	    {
		if (stCurrentError -> line == 0)
		{
		    EdWin_ShowStatusBold (EdGUI_GetTopWindow (pmTextDisplayWindow),
		        stCurrentError -> errorMessage);
		}
		else
		{
		    EdWin_ShowStatusBold (EdGUI_GetTopWindow (pmTextDisplayWindow),
		        "Line %d of Unnamed File: %s", stCurrentError -> line + 1,
		        stCurrentError -> errorMessage);
		}
	    }
	}
    }
} // EdErr_FindNextError


/************************************************************************/
/* EdErr_HasErrors							*/
/************************************************************************/
BOOL	EdErr_HasErrors (void)
{
    return (stErrorHead != NULL);
} // EdErr_HasErrors


/************************************************************************/
/* EdErr_HilightLine							*/
/*									*/
/* This jumps to the specified line using the turing file number, if	*/
/* non-zero, the path name otherwise.  If the window is not currently	*/
/* open, then it will open the file in a window if the file was		*/
/* specified using a path name.  It won't open it if it was specified	*/
/* using a Turing file number because this module is not Turing related	*/
/* and thus cannot make any FileManager calls.  (i.e. using pathname	*/
/* is vastly preferred.							*/
/************************************************************************/
BOOL	EdErr_HilightLine (HWND pmTextDisplayWindow, const char *pmPathName,
			   WORD pmTuringFileNo, int pmLine, 
			   BOOL pmActivateSourceWindow, const char *pmMessage)
{
    HWND	myEditWindow, myTextDisplayWindow;	
    BOOL	myFileOpened = FALSE;
    
    // If the original compiled file was unnamed, use the turing file number to
    // try and look it up.
    if (pmTuringFileNo != 0)
    {
        EdDisp_GetWindowWithMatchingTuringFileNo (
        	pmTuringFileNo, 
		&myEditWindow, &myTextDisplayWindow);
    }
    else
    {	    		
        EdDisp_GetWindowWithMatchingPathName (pmPathName, 
		&myEditWindow, &myTextDisplayWindow);
    }
    
    if (myTextDisplayWindow != NULL)
    {
	// If the window is iconic, expand it.
	if (IsIconic (myEditWindow))
	{
	    EdDisp_SetDisplayResizeMessage (FALSE);
	    ShowWindow (myEditWindow, SW_RESTORE);
	    EdDisp_SetDisplayResizeMessage (TRUE);
	}


    	if ((GetForegroundWindow () != myEditWindow) || 
    	    (myTextDisplayWindow != pmTextDisplayWindow))
    	{
	    // The next error is in an open window
	    if (pmActivateSourceWindow)
	    {
		BringWindowToTop (myEditWindow);
	    }
	    else
	    {
		Ed_PutWindowAboveAllOtherEdWindows (myEditWindow);
	    }
	    EdWin_ActivateTextDisplay (myEditWindow, myTextDisplayWindow);
	}
	
    	// Highlight the current line
    	EdDisp_HilightLine (myTextDisplayWindow, pmLine - 1);
	
    	if ((pmMessage != NULL) && (*pmMessage != 0))
    	{
    	    EdWin_ShowStatus (myTextDisplayWindow, pmMessage);
    	}
    	
	return TRUE;
    }

    //
    // The file is not in an open window.  We'll now try and open it.
    //
        	
    // If the turing file number is not zero, or the path name is related
    // to an unnamed file, then we can't open the file into a window.
    if ((pmPathName [0] != 0) &&
	(strncmp (pmPathName, "unnamed", 7) != 0) &&
	(strcmp (pmPathName, "<No file>") != 0))
    {
    	myFileOpened = EdWin_OpenFile (
    	    			EdGUI_GetTopWindow (pmTextDisplayWindow), 
			    	pmPathName, NULL, PURPOSE_DISPLAY_ERROR);
    }
	
    if (!myFileOpened)
    {
    	return FALSE;
    }
    
    // Now the file has been opened, let's do a match
    if (pmTuringFileNo != 0)
    {
        EdDisp_GetWindowWithMatchingTuringFileNo (pmTuringFileNo, 
			&myEditWindow, &myTextDisplayWindow);
    }
    else
    {	    		
        EdDisp_GetWindowWithMatchingPathName (pmPathName, 
		&myEditWindow, &myTextDisplayWindow);
    }
	    
    if (myTextDisplayWindow != NULL)
    {
    	EdDisp_HilightLine (myTextDisplayWindow, pmLine - 1);
    	
    	if ((pmMessage != NULL) && (*pmMessage != 0))
    	{
    	    EdWin_ShowStatus (myTextDisplayWindow, pmMessage);
    	}
       	return TRUE;
    }

    return FALSE;
} // EdErr_HilightLine


/************************************************************************/
/* EdErr_LoadErrors							*/
/*									*/
/* When a file is loaded, check the list of errors to see if there are	*/
/* any errors in this file.  If there is, add them to the text and 	*/
/* also jump to the first error, making it the current error.		*/
/************************************************************************/
void	EdErr_LoadErrors (HWND pmTextDisplayWindow)
{
    ErrorPtr	myErrorPtr;
    char	*myPath;
    BOOL	myFoundErrorInFile;
       	
    if (stErrorHead == NULL)
    {
    	return;
    }

    myPath = EdDisp_GetPath (pmTextDisplayWindow);
    
    if (myPath == NULL)
    {
    	return;
    }

    //    
    // Add the errors to the window
    //
    myErrorPtr = stErrorHead;
    myFoundErrorInFile = FALSE;
    while (myErrorPtr != NULL)
    {
    	//
    	// If window path matches error, add error to text
    	//
    	const char	*myPath1, *myPath2;
    	BOOL	myMatch = TRUE;
    	    
    	myPath1 = myPath;
    	myPath2 = myErrorPtr -> pathName;
    	
    	if (myPath2 == NULL)
    	{
    	    continue;
    	}

	do
	{
	    if ((tolower (*myPath1) != tolower (*myPath2)) &&
	    	(!((*myPath1 == '/') && (*myPath2 == '\\'))) &&
	    	(!((*myPath1 == '\\') && (*myPath2 == '/'))))
	    {
		myMatch = FALSE;
		break;
	    }	    	    
	    myPath1++;
	    myPath2++;
	} while ((*myPath1 != 0) && (*myPath2 != 0));
	    
	if (myMatch && (*myPath1 == *myPath2))
	{
	    // There's a match, add the error
	    EdText_AddError (EdDisp_GetTextPtr (pmTextDisplayWindow), 
	    		     myErrorPtr);
	    myFoundErrorInFile = TRUE;
	}
	
	myErrorPtr = myErrorPtr -> next;
    }

    //
    // Now we jump to the next current error that's in the file
    //
    if (!myFoundErrorInFile)
    {
    	return;
    }
    
    myErrorPtr = stCurrentError;
    
    do
    {
    	//
    	// If window path matches error, add error to text
    	//
    	const char	*myPath1, *myPath2;
    	BOOL	myMatch = TRUE;
    	    
    	myPath1 = myPath;
    	myPath2 = myErrorPtr -> pathName;

	if (myPath2 == NULL)
	{
	    continue;
	}
	do
	{
	    if ((tolower (*myPath1) != tolower (*myPath2)) &&
	    	(!((*myPath1 == '/') && (*myPath2 == '\\'))) &&
	    	(!((*myPath1 == '\\') && (*myPath2 == '/'))))
	    {
		myMatch = FALSE;
		break;
	    }	    	    
	    myPath1++;
	    myPath2++;
	} while ((*myPath1 != 0) && (*myPath2 != 0));
	    
	if (myMatch && (*myPath1 == *myPath2))
	{
    	    // Display the error in the current window
    	    stCurrentError = myErrorPtr;
    	    if ((stCurrentError -> tokenPos == 0) && 
    	        (stCurrentError -> tokenLen == 0))
	    {
    	    	EdDisp_GotoError (pmTextDisplayWindow, stCurrentError -> line,
    	            -1, -1, stCurrentError -> errorMessage);
	    }
	    else
	    {
    	    	EdDisp_GotoError (pmTextDisplayWindow, stCurrentError -> line,
    	            stCurrentError -> tokenPos, stCurrentError -> tokenLen, 
    	            stCurrentError -> errorMessage);
    	    }
	
	    return;
	}
	
	myErrorPtr = myErrorPtr -> next;
    	if (myErrorPtr == NULL)
    	{
    	    myErrorPtr = stErrorHead;
    	}
    } while (myErrorPtr != stCurrentError);
} // EdErr_LoadErrors


/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/
