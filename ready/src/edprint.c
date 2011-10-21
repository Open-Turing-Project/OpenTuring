/*************/
/* edprint.c */
/*************/

/*******************/
/* System includes */
/*******************/
#include <windows.h>
#include <windowsx.h>
#include <crtdbg.h>

/****************/
/* Self include */
/****************/
#include "edprint.h"

/******************/
/* Other includes */
/******************/
#ifndef TPROLOG
#include "edwin.h"
#endif // #ifndef TPROLOG
#include "ed.h"
#include "edfail.h"
#include "edfile.h"
#include "edgui.h"

#include "eddialogs.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
#define		NINETY_DEGREES		900

#define		SYNTAX_COLOUR_OFF	1
#define		FILE_NAME_OFF		2

#define		GRAPHICS_DRAW_BORDER	1
#define		GRAPHICS_SCALE_TO_FIT	2
#define		GRAPHICS_HEADER		4

/*********/
/* Types */
/*********/

/********************/
/* Static constants */
/********************/
static char	stPrintDialogName[] = "PrintAbortDialog";

/********************/
/* Static variables */
/********************/
// Properties of the status window
static EdPrint_Properties	stProperties;
static BOOL			stPropertiesChanged;

// Other static variables
static HWND		stPrintDialog;
static BOOL		stUserAbort;
static HGLOBAL		stDevMode, stDevNames;
static int		stGraphicsPrintProperties = 7;

/******************************/
/* Static callback procedures */
/******************************/
static BOOL CALLBACK 	MyAbortProc (HDC pmPrinterDC, int pmCode);
static UINT CALLBACK 	MyPrintDialogProcedure (HWND pmWindow, 
						UINT pmMessage, 
    						WPARAM pmWParam, 
    						LPARAM pmLParam);
static UINT CALLBACK 	MyPrintGraphicsDialogProcedure (
				HWND pmPrintDialogWindow, UINT pmMessage, 
    				WPARAM pmWParam, LPARAM pmLParam);
static UINT CALLBACK 	MyPageSetupDialogProcedure (HWND pmPrintDialogWindow, 
						    UINT pmMessage, 
	    					    WPARAM pmWParam, 
    						    LPARAM pmLParam);
static BOOL CALLBACK 	MyPrintStatusDialogProcedure (HWND pmWindow, 
						      UINT pmMessage, 
    						      WPARAM pmWParam, 
    						      LPARAM pmLParam);

/*********************/
/* Static procedures */
/*********************/
static int	MyCalculateFontSize (HDC pmPrinterDeviceContext,
				     BOOL *pmPortrait);
static BOOL	MyCheckedToBool (HWND pmDialog, int pmCheckBox);
static BITMAPINFO	*MyGetBitMapInfoFromBitMap (HBITMAP pmBitMap, 
						    HPALETTE pmPalette,
						    BYTE **pmBits);
static void	MyPrintPageHeader (HDC pmPrinterDeviceContext, 
				   HFONT pmHeaderFont, int pmPageNumber,
				   char *pmDate, char *pmJobName, 
				   BOOL pmPrinting,
				   int pmPageWidth, int pmLeftMargin,
				   int pmTopMargin, int pmRightMargin,
				   int *pmBaseLine);
static void	MyPrintProgramPage (HDC pmPrinterDeviceContext, 
				    int pmPageNumber, BOOL pmPrinting,
				    TextPtr pmTextPtr, int *pmCurrentPos,
				    int pmLastPos, char *pmJobName,
				    char *pmDate, int *pmLineNumber, 
				    BOOL *pmStartOfLine, int *pmStartSpace,
				    HFONT pmFont, HFONT pmBoldFont, 
				    HFONT pmItalicFont, HFONT pmHeaderFont);
static void	MyPrintTwoUpProgramPage (HDC pmPrinterDeviceContext, 
				    	 int pmPageNumber, BOOL pmPrinting,
				    	 TextPtr pmTextPtr, int *pmCurrentPos,
				    	 int pmLastPos, char *pmJobName,
				    	 char *pmDate, int *pmLineNumber, 
				    	 BOOL *pmStartOfLine, int *pmStartSpace,
				    	 HFONT pmFont, HFONT pmBoldFont, 
				    	 HFONT pmItalicFont, 
				    	 HFONT pmHeaderFont);
static void	MySetCheckBox (HWND pmDialog, int pmCheckBox, BOOL pmValue);


/***********************/
/* External procedures */
/***********************/
/************************************************************************/
/* EdPrint_Init								*/
/************************************************************************/
BOOL	EdPrint_Init (void)
{
    stPrintDialog = NULL;
    stUserAbort = FALSE;
    stDevMode = NULL;
    stDevNames = NULL;
    
    return TRUE;
} // EdPrint_Init


/************************************************************************/
/* EdPrint_Finalize							*/
/************************************************************************/
void	EdPrint_Finalize (void)
{
} // EdPrint_Finalize


/************************************************************************/
/* EdPrint_PropertiesSet						*/
/************************************************************************/
void	EdPrint_PropertiesSet (EdPrint_Properties pmProperties)
{
    stProperties = pmProperties;
    stPropertiesChanged = TRUE;
} // EdPrint_PropertiesSet

	
/************************************************************************/
/* EdPrint_PropertiesImplementChanges					*/
/************************************************************************/
void	EdPrint_PropertiesImplementChanges (void)
{
    stPropertiesChanged = FALSE;
} // EdPrint_PropertiesImplementChanges


#ifndef TPROLOG
/************************************************************************/
/* EdPrint_PageSetup							*/
/************************************************************************/
void	EdPrint_PageSetup (HWND pmEditWindow)
{
    PAGESETUPDLG	myPageSetup;
    BOOL		myResult;

    ZeroMemory (&myPageSetup, sizeof (myPageSetup));
    myPageSetup.lStructSize = sizeof (myPageSetup);
    myPageSetup.hwndOwner = pmEditWindow;
    myPageSetup.hDevMode = stDevMode;
    myPageSetup.hDevNames = stDevNames;
    myPageSetup.lpfnPageSetupHook = MyPageSetupDialogProcedure;
    myPageSetup.rtMargin = stProperties.margins;
    myPageSetup.Flags = PSD_INTHOUSANDTHSOFINCHES | PSD_MARGINS |
    			PSD_ENABLEPAGESETUPHOOK;

    Ed_EnableAllWindows (FALSE, pmEditWindow);
    myResult = PageSetupDlg (&myPageSetup);
    Ed_EnableAllWindows (TRUE, pmEditWindow);

    if (myResult)
    {
    	stDevMode = myPageSetup.hDevMode;
    	stDevNames = myPageSetup.hDevNames;
    	stProperties.margins = myPageSetup.rtMargin;
    	EdWin_ShowStatus (pmEditWindow, "Printer Setup");
    }
    else
    {
    	EdWin_ShowStatus (pmEditWindow, "Page Setup Cancelled");
    }
} // EdPrint_PageSetup
#endif // #ifndef TPROLOG


/************************************************************************/
/* EdPrint_PrintEditorWindow						*/
/************************************************************************/
void	EdPrint_PrintEditorWindow (HWND pmEditWindow, TextPtr pmTextPtr, 
				   const char *pmPathName, int pmUnnamedNumber,
				   BOOL pmIsExample, const char *pmExampleName,
				   BOOL pmSyntaxColour, BOOL pmOutput)
{
    BOOL	myIsEditorWindow;
    HINSTANCE	myApplicationInstance;
    PRINTDLG	myPrintDialog;
    DOCINFO	myDocInfo;
    FilePath	myJobName;
    char	myDateTimeString [160], myTimeString [80];
    BOOL	myPrintSuccess;
    int		myCopies, myFirstPage, myLastPage;
    BOOL	myPrintSelection;
    HDC		myPrinterDeviceContext;
    int		myCurrentPos, myLastPos;
    int		myLineNumber, myPageNumber;
    BOOL	myStartOfLine;
    int		myStartSpace;
    HFONT	myFont, myBoldFont, myItalicFont, myHeaderFont;
    BOOL	myOrigBoldKeywords, myOrigItalIdents;
    int		myPagesPrinted;
    BOOL	myResult;

#ifndef TPROLOG
    // Debug messages
    EdFile_GetPathForDisplay (pmPathName, pmUnnamedNumber, pmIsExample,
    	    		      pmExampleName, TRUE, myJobName);
    EdFail_AddMessage (NORMAL, "PrintEditorWindow - Window: %x - Path: %s", 
    		       pmEditWindow, myJobName);
#endif // #ifndef TPROLOG

    // Do the printing    		       
    myIsEditorWindow = (GetWindowLong (EdGUI_GetTopWindow (pmEditWindow), 
    				WINDOW_MEM_WINDOW_TYPE) == WINDOW_TYPE_EDIT);
 
#ifndef TPROLOG
    // Print Status Message
    if (myIsEditorWindow)
    {
    	EdWin_ShowStatus (pmEditWindow, "Printing %s", 
    	    EdFile_GetPathForDisplay (pmPathName, pmUnnamedNumber, pmIsExample,
    	    	pmExampleName, FALSE, myJobName));
    }
#endif // #ifndef TPROLOG
    
    // Get the program instance
    myApplicationInstance = 
        (HINSTANCE) GetWindowLong (pmEditWindow, GWL_HINSTANCE);

    // Store original values for bolding/italicizing if not a language file
    if (!pmSyntaxColour)
    {
    	myOrigBoldKeywords = stProperties.boldKeywords;
    	myOrigItalIdents = stProperties.italicizeIdents;
    	stProperties.boldKeywords = FALSE;
    	stProperties.italicizeIdents = FALSE;
    }

    ZeroMemory (&myPrintDialog, sizeof (myPrintDialog));
    
    myPrintDialog.lStructSize = sizeof (myPrintDialog);
    myPrintDialog.hwndOwner = pmEditWindow;
    myPrintDialog.hDevMode = stDevMode;
    myPrintDialog.hDevNames = stDevNames;
    myPrintDialog.Flags = PD_RETURNDC | PD_COLLATE | PD_ALLPAGES | 
        PD_ENABLEPRINTHOOK | PD_ENABLEPRINTTEMPLATE;
    myPrintDialog.lpfnPrintHook = MyPrintDialogProcedure;    
    myPrintDialog.hInstance = myApplicationInstance;
    myPrintDialog.lpPrintTemplateName = MAKEINTRESOURCE (READY_PRINT_DIALOG);
    if (!pmSyntaxColour)
    {
    	myPrintDialog.lCustData += SYNTAX_COLOUR_OFF;
    }
    if ((pmPathName == NULL) || pmOutput)
    {
    	myPrintDialog.lCustData += FILE_NAME_OFF;
    }
    myPrintDialog.nFromPage = 1;
    myPrintDialog.nToPage = 9999;
    myPrintDialog.nMinPage = 1;
    myPrintDialog.nMaxPage = 9999;
    if (pmTextPtr -> selFrom == pmTextPtr -> selTo)
    {
    	myPrintDialog.Flags |= PD_NOSELECTION;
    }

    // Call the Print Dialog Box
#ifndef TPROLOG
    Ed_EnableAllWindows (FALSE, pmEditWindow);
#endif // #ifndef TPROLOG
    myResult = PrintDlg (&myPrintDialog);
#ifndef TPROLOG
    Ed_EnableAllWindows (TRUE, pmEditWindow);
#endif // #ifndef TPROLOG

    if (!myResult)
    {
    	int	myError = CommDlgExtendedError ();
    	
    	// Restore original values for bolding/italicizing
    	if (!pmSyntaxColour)
    	{
    	    stProperties.boldKeywords = myOrigBoldKeywords;
    	    stProperties.italicizeIdents = myOrigItalIdents;
    	}
    
#ifndef TPROLOG
    	if (myIsEditorWindow)
    	{
    	    if (myError != 0)
    	    {
            	EdWin_ShowStatus (pmEditWindow, "Printing Cancelledx (%d)", 
            			  myError);
            }
            else
    	    {
            	EdWin_ShowStatus (pmEditWindow, "Printing Cancelled");
            }
        }
#endif // #ifndef TPROLOG
    	return;
    }

    // Save the device contexts    
    stDevMode = myPrintDialog.hDevMode;
    stDevNames = myPrintDialog.hDevNames;
    
    // Set the device context
    myPrinterDeviceContext = myPrintDialog.hDC;
    
    // Create the fonts used in printing
    if (stProperties.twoUp)
    {
    	int	myFontSize;
    	BOOL	myPortrait;
    	
    	myFontSize = MyCalculateFontSize (myPrinterDeviceContext, &myPortrait);
    	
    	if (!myPortrait)
    	{
    	    EdGUI_Message1 (pmEditWindow, 0, IDS_PRINT_ERROR_TITLE,
    	        IDS_NO_TWO_UP_LANDSCAPE);
	    DeleteDC (myPrinterDeviceContext);
	    
	    if (!pmSyntaxColour)
	    {
	    	stProperties.boldKeywords = myOrigBoldKeywords;
	    	stProperties.italicizeIdents = myOrigItalIdents;
	    }
	    
#ifndef TPROLOG
	    if (myIsEditorWindow)
	    {
            	EdWin_ShowStatus (pmEditWindow, "Printing Aborted");
            }
#endif // #ifndef TPROLOG
	    
	    return;    	        
    	}
    	
    	myFont = EdGUI_CreateFontAngle (NULL, myPrinterDeviceContext, 
    	    stProperties.fontName, -myFontSize, NO_BOLD, NO_ITALIC, 
    	    NINETY_DEGREES);
    	if (stProperties.boldKeywords)
    	{
    	    myBoldFont = EdGUI_CreateFontAngle (NULL, myPrinterDeviceContext, 
    	    	stProperties.fontName, -myFontSize, BOLD, NO_ITALIC, 
    	    	NINETY_DEGREES);
    	}
    	else
    	{
    	    myBoldFont = EdGUI_CreateFontAngle (NULL, myPrinterDeviceContext, 
    	        stProperties.fontName, -myFontSize, NO_BOLD, 
    	        NO_ITALIC, NINETY_DEGREES);
    	}
    	if (stProperties.italicizeIdents)
    	{
    	    myItalicFont = EdGUI_CreateFontAngle (NULL, myPrinterDeviceContext, 
    	    	stProperties.fontName, -myFontSize, NO_BOLD, ITALIC, 
    	    	NINETY_DEGREES);
    	}
    	else
    	{
    	    myItalicFont = EdGUI_CreateFontAngle (NULL, myPrinterDeviceContext, 
    	    	stProperties.fontName, -myFontSize, NO_BOLD, 
    	    	NO_ITALIC, NINETY_DEGREES);
    	}
    	myHeaderFont = EdGUI_CreateFontAngle (NULL, myPrinterDeviceContext, 
    	    stProperties.headerFontName, stProperties.headerFontSize, 
    	    BOLD, NO_ITALIC, NINETY_DEGREES);
    }
    else
    {    
    	myFont = EdGUI_CreateFont (NULL, myPrinterDeviceContext, 
    	    stProperties.fontName, stProperties.fontSize, NO_BOLD, NO_ITALIC);
    	if (stProperties.boldKeywords)
    	{
    	    myBoldFont = EdGUI_CreateFont (NULL, myPrinterDeviceContext, 
    	    	stProperties.fontName, stProperties.fontSize, BOLD, NO_ITALIC);
    	}
    	else
    	{
    	    myBoldFont = EdGUI_CreateFont (NULL, myPrinterDeviceContext, 
    	        stProperties.fontName, stProperties.fontSize, NO_BOLD, 
    	        NO_ITALIC);
    	}
    	if (stProperties.italicizeIdents)
    	{
    	    myItalicFont = EdGUI_CreateFont (NULL, myPrinterDeviceContext, 
    	    	stProperties.fontName, stProperties.fontSize, NO_BOLD, ITALIC);
    	}
    	else
    	{
    	    myItalicFont = EdGUI_CreateFont (NULL, myPrinterDeviceContext, 
    	    	stProperties.fontName, stProperties.fontSize, NO_BOLD, 
    	    	NO_ITALIC);
    	}
    	myHeaderFont = EdGUI_CreateFont (NULL, myPrinterDeviceContext, 
    	    stProperties.headerFontName, stProperties.headerFontSize, 
    	    BOLD, NO_ITALIC);
    }

    // Display the printing dialog box
    EnableWindow (pmEditWindow, FALSE);
    myPrintSuccess = TRUE;
    stUserAbort = FALSE;
    
    // Create the job title
    if (pmOutput)
    {
	if (gProgram.isJava)
	{
    	    strcpy (myJobName, "Output from ");
    	    strcat (myJobName, pmPathName);
	}
	else
	{
    	    strcpy (myJobName, pmPathName);
	}
    }
    else
    {
#ifdef TPROLOG
        GetModuleFileName (NULL, myJobName, sizeof (myJobName));
#else
    	EdFile_GetPathForDisplay (pmPathName, pmUnnamedNumber, pmIsExample,
            pmExampleName, stProperties.fullPathName, myJobName);
    	if (myPrintDialog.Flags & PD_SELECTION)
    	{
    	    strcat (myJobName, " Selection");
    	}
#endif // #else - #ifdef TPROLOG
    }
    
    // Create the date string
    GetDateFormat (LOCALE_USER_DEFAULT, 0, NULL, "MMM d yyyy", myDateTimeString, 
        sizeof (myDateTimeString));
    GetTimeFormat (LOCALE_USER_DEFAULT, TIME_NOSECONDS, NULL, NULL, 
        myTimeString, sizeof (myTimeString));
    strcat (myDateTimeString, " ");
    strcat (myDateTimeString, myTimeString);
    
    stPrintDialog = CreateDialog (
	myApplicationInstance, // Program instance handle
	stPrintDialogName, pmEditWindow, MyPrintStatusDialogProcedure);
    SetDlgItemText (stPrintDialog, PRINT_PAGENUMBER, "Introduction");
    SetDlgItemText (stPrintDialog, PRINT_JOBNAME, myJobName);
    SetAbortProc (myPrinterDeviceContext, MyAbortProc);

    myPagesPrinted = 0;
    
    // Set up the DOCINFO structure
    ZeroMemory (&myDocInfo, sizeof (myDocInfo));
    myDocInfo.cbSize = sizeof (myDocInfo);
    myDocInfo.lpszDocName = myJobName;
    if (StartDoc (myPrinterDeviceContext, &myDocInfo) <= 0)
    {
    	ErrorBuffer	myErrorBuffer;
	int		myErrorCode = GetLastError ();
    	
    	EdGUI_Message (
    	    "Printing Initialization Error",
    	    "Unable to open printing document.\n\n%s\n[Error code: %d]",
    	    EdFail_GetErrorMessage (myErrorBuffer), 
    	    myErrorCode);
    	
    	// Close the printing dialog window and free the printer DC
    	EnableWindow (pmEditWindow, TRUE);
    	DestroyWindow (stPrintDialog);
    	DeleteDC (myPrinterDeviceContext);
    	    
    	// Restore original values for bolding/italicizing
    	if (!pmSyntaxColour)
    	{
    	    stProperties.boldKeywords = myOrigBoldKeywords;
    	    stProperties.italicizeIdents = myOrigItalIdents;
    	}

#ifndef TPROLOG
    	if (myIsEditorWindow)
    	{
            EdWin_ShowStatus (pmEditWindow, "Printing Failed");
        }
#endif // #ifndef TPROLOG
        
    	return;
    }

    // Set the pages to print and whether to print only the selection
    if (myPrintDialog.Flags & PD_PAGENUMS)
    {
    	myFirstPage = myPrintDialog.nFromPage;
    	myLastPage = myPrintDialog.nToPage;
    }
    else
    {
    	myFirstPage = 1;
    	myLastPage = 9999;
    }
    if (myPrintDialog.Flags & PD_SELECTION)
    {
    	myPrintSelection = TRUE;
    }
    else
    {
    	myPrintSelection = FALSE;
    }

    for (myCopies = 0 ; myCopies < myPrintDialog.nCopies ; myCopies++)
    {
    	myLineNumber = 1;
    	if (myPrintSelection)
    	{
    	    int	dummy1, dummy2, dummy3;
    	    
    	    myCurrentPos = pmTextPtr -> selFrom;
    	    myLastPos = pmTextPtr -> selTo;
    	    
    	    // Calculate the starting line number
	    EdText_GetSelectionLinesCols (pmTextPtr, &myLineNumber, &dummy1,
	        &dummy2, &dummy3);
	    myLineNumber++; // Line numbers a zero based internally
	}
	else
	{    	    
    	    myCurrentPos = 0;
    	    myLastPos = pmTextPtr -> textLen;
    	}
    	myPageNumber = 1;
    	myStartOfLine = TRUE;
    	
    	while (myPageNumber <= myLastPage)
    	{
    	    if (myFirstPage <= myPageNumber)
    	    {
    	    	if (StartPage (myPrinterDeviceContext) < 0)
    	    	{
	    	    ErrorBuffer	myErrorBuffer;
		    int		myErrorCode = GetLastError ();
		    
    	    	    myPrintSuccess = FALSE;
	    	    EdGUI_Message (
	    	        "Printing Open Page Error",
    		        "Unable to open page %d.\n\n%s\n[Error code: %d]",
	    	        myPageNumber, 
	    	        EdFail_GetErrorMessage (myErrorBuffer), 
	    	        myErrorCode);
    	    	    break;
    	    	}
	    }    	    	

	    if (myFirstPage <= myPageNumber)
	    {
	    	char	myPrintMessage [128];
	    	
	    	if (myPrintDialog.nCopies > 1)
	    	{
		    wsprintf (myPrintMessage, "Copy %d   Page %d", 
		        myCopies + 1, myPageNumber);
	    	}
	    	else
	    	{
		    wsprintf (myPrintMessage, "Page %d", myPageNumber);
	    	}
    		SetDlgItemText (stPrintDialog, PRINT_PAGENUMBER, 
    		    myPrintMessage);
    	    }
    	    
    	    if (stProperties.twoUp)
    	    {
    	    	// Print the page (or if not in print range, move the pointer)
    	    	MyPrintTwoUpProgramPage (myPrinterDeviceContext, myPageNumber, 
    	            (myFirstPage <= myPageNumber), pmTextPtr, &myCurrentPos, 
    	            myLastPos, myJobName, myDateTimeString, &myLineNumber, 
    	            &myStartOfLine, &myStartSpace, myFont, myBoldFont, 
    	            myItalicFont, myHeaderFont);
    	    }
    	    else
    	    {
    	    	// Print the page (or if not in print range, move the pointer)
    	    	MyPrintProgramPage (myPrinterDeviceContext, myPageNumber, 
    	            (myFirstPage <= myPageNumber), pmTextPtr, &myCurrentPos, 
    	            myLastPos, myJobName, myDateTimeString, &myLineNumber, 
    	            &myStartOfLine, &myStartSpace, myFont, myBoldFont, 
    	            myItalicFont, myHeaderFont);
    	    }

    	    if (myFirstPage <= myPageNumber)
    	    {
    	    	if (EndPage (myPrinterDeviceContext) < 0)
    	    	{
	    	    ErrorBuffer	myErrorBuffer;
		    int		myErrorCode = GetLastError ();
		    
    	    	    myPrintSuccess = FALSE;
	    	    EdGUI_Message (
	    	        "Printing Close Page Error",
    		        "Unable to close page %d.\n\n%s\n[Error code: %d]",
	    	        myPageNumber, 
	    	        EdFail_GetErrorMessage (myErrorBuffer), 
	    	        myErrorCode);
    	    	    break;
    	    	}
    		    
		myPagesPrinted++;    		    
	    } // if
	    
	    myPageNumber++;

	    if ((myCurrentPos >= myLastPos) || stUserAbort)
	    {
	    	break;
	    }
	} // while
    } // for

    // Close the printer    	    	
    EndDoc (myPrinterDeviceContext);
    
#ifndef TPROLOG
    if (myPrintSuccess && myIsEditorWindow)
    {
    	if (stUserAbort)
    	{
    	    EdWin_ShowStatus (pmEditWindow, 
    	    	"Printing Cancelled by User: %d Pages Printed",
    	    	myPagesPrinted);
    	}
    	else
    	{
    	    EdWin_ShowStatus (pmEditWindow, "Printing %s: %d Pages Printed",
	    	EdFile_GetPathForDisplay (pmPathName, pmUnnamedNumber, 
	    	    pmIsExample, pmExampleName, FALSE, myJobName), 
	    	myPagesPrinted);
    	}
    }
    else if (myIsEditorWindow)
    {
    	EdWin_ShowStatus (pmEditWindow, "Printing Failed");
    }    
#endif // #ifndef TPROLOG
    
    if (!stUserAbort)
    {
    	EnableWindow (pmEditWindow, TRUE);
    	DestroyWindow (stPrintDialog);
    }

    DeleteDC (myPrinterDeviceContext);
    
    // Free the fonts
    DeleteObject (myFont);
    DeleteObject (myBoldFont);
    DeleteObject (myItalicFont);
    DeleteObject (myHeaderFont);
    
    // Restore original values for bolding/italicizing
    if (!pmSyntaxColour)
    {
    	stProperties.boldKeywords = myOrigBoldKeywords;
    	stProperties.italicizeIdents = myOrigItalIdents;
    }
} // EdPrint_PrintEditorWindow


/************************************************************************/
/* EdPrint_PrintGraphics						*/
/************************************************************************/
void	EdPrint_PrintGraphics (HWND pmEditWindow, HBITMAP pmBitMap, int pmWidth,
			       int pmHeight, HPALETTE pmPalette, 
			       int pmNormalWidth,
			       BOOL pmDisplayPrintDialog, 
			       BOOL pmDrawBorder, BOOL pmScaleToFit, 
			       BOOL pmDrawHeader, char *pmTitle)
{
    BOOL	myIsEditorWindow;
    HINSTANCE	myApplicationInstance;
    PRINTDLG	myPrintDialog;
    DOCINFO	myDocInfo;
    FilePath	myJobName;
    char	myDateTimeString [160], myTimeString [80];
    BOOL	myPrintSuccess;
    int		myCopies;
    char	myPrintMessage [128];
    HDC		myPrinterDeviceContext;
    HFONT	myHeaderFont;
    BOOL	myResult;
    int		myRealTopMargin, myRealLeftMargin;
    int		myRealBottomMargin, myRealRightMargin;
    int		myPageWidth, myPageHeight, myHalfInch;
    int		myTop, myLeft, myPictureWidth, myPictureHeight;
    int		myXTenthOfAnInch, myYTenthOfAnInch;
    double	myScaleFactor, myXScaleFactor, myYScaleFactor;
    BITMAPINFO	*myBitMapInfo;
    BYTE	*myBits;

    myIsEditorWindow = (GetWindowLong (EdGUI_GetTopWindow (pmEditWindow), 
    				WINDOW_MEM_WINDOW_TYPE) == WINDOW_TYPE_EDIT);
 
#ifndef TPROLOG
    // Print Status Message
    if (myIsEditorWindow)
    {
    	EdWin_ShowStatus (pmEditWindow, "Printing %s", pmTitle);
    }
#endif // #ifndef TPROLOG
    
    // Get the program instance
    myApplicationInstance = 
        (HINSTANCE) GetWindowLong (pmEditWindow, GWL_HINSTANCE);

    ZeroMemory (&myPrintDialog, sizeof (myPrintDialog));
    
    myPrintDialog.lStructSize = sizeof (myPrintDialog);
    myPrintDialog.hwndOwner = pmEditWindow;
    myPrintDialog.hDevMode = stDevMode;
    myPrintDialog.hDevNames = stDevNames;
    if (pmDisplayPrintDialog)
    {
	myPrintDialog.Flags = PD_RETURNDC | PD_COLLATE | PD_ALLPAGES | 
	    PD_ENABLEPRINTHOOK | PD_ENABLEPRINTTEMPLATE | PD_NOSELECTION |
	    PD_NOPAGENUMS;
	myPrintDialog.lpfnPrintHook = MyPrintGraphicsDialogProcedure;    
	myPrintDialog.lpPrintTemplateName = MAKEINTRESOURCE (
						READY_PRINT_GRAPHICS_DIALOG);
    }
    else
    {
	myPrintDialog.Flags = PD_RETURNDC | PD_COLLATE | PD_ALLPAGES | 
	    PD_NOSELECTION | PD_NOPAGENUMS;
    }
    myPrintDialog.hInstance = myApplicationInstance;
    myPrintDialog.nFromPage = 1;
    myPrintDialog.nToPage = 1;
    myPrintDialog.nMinPage = 1;
    myPrintDialog.nMaxPage = 1;

    // Call the Print Dialog Box
#ifndef TPROLOG
    Ed_EnableAllWindows (FALSE, pmEditWindow);
#endif // #ifndef TPROLOG
    myResult = PrintDlg (&myPrintDialog);
#ifndef TPROLOG
    Ed_EnableAllWindows (TRUE, pmEditWindow);
#endif // #ifndef TPROLOG

    if (!myResult)
    {
#ifndef TPROLOG
    	int	myError = CommDlgExtendedError ();
    	
    	if (myIsEditorWindow)
    	{
    	    if (myError != 0)
    	    {
            	EdWin_ShowStatus (pmEditWindow, "Printing Cancelledx (%d)", 
            			  myError);
            }
            else
    	    {
            	EdWin_ShowStatus (pmEditWindow, "Printing Cancelled");
            }
        }
#endif // #ifndef TPROLOG
    	return;
    }

    // Save the device contexts    
    stDevMode = myPrintDialog.hDevMode;
    stDevNames = myPrintDialog.hDevNames;
    
    // Set the device context
    myPrinterDeviceContext = myPrintDialog.hDC;
    
    myHeaderFont = EdGUI_CreateFont (NULL, myPrinterDeviceContext, 
    	    stProperties.headerFontName, stProperties.headerFontSize, 
    	    BOLD, NO_ITALIC);

    // Display the printing dialog box
    EnableWindow (pmEditWindow, FALSE);
    myPrintSuccess = TRUE;
    stUserAbort = FALSE;
    
    // Create the job title
    if (gProgram.isJava)
    {
    	strcpy (myJobName, "Output from ");
    	strcat (myJobName, pmTitle);
    }
    else
    {
    	strcpy (myJobName, pmTitle);
    }
    
    // Create the date string
    GetDateFormat (LOCALE_USER_DEFAULT, 0, NULL, "MMM d yyyy", myDateTimeString, 
        sizeof (myDateTimeString));
    GetTimeFormat (LOCALE_USER_DEFAULT, TIME_NOSECONDS, NULL, NULL, 
        myTimeString, sizeof (myTimeString));
    strcat (myDateTimeString, " ");
    strcat (myDateTimeString, myTimeString);
    
    stPrintDialog = CreateDialog (
	myApplicationInstance, // Program instance handle
	stPrintDialogName, pmEditWindow, MyPrintStatusDialogProcedure);
    SetDlgItemText (stPrintDialog, PRINT_PAGENUMBER, "Introduction");
    SetDlgItemText (stPrintDialog, PRINT_JOBNAME, myJobName);
    SetAbortProc (myPrinterDeviceContext, MyAbortProc);

    // Set up the DOCINFO structure
    ZeroMemory (&myDocInfo, sizeof (myDocInfo));
    myDocInfo.cbSize = sizeof (myDocInfo);
    myDocInfo.lpszDocName = myJobName;
    if (StartDoc (myPrinterDeviceContext, &myDocInfo) <= 0)
    {
    	ErrorBuffer	myErrorBuffer;
	int		myErrorCode = GetLastError ();
    	
    	EdGUI_Message (
    	    "Printing Initialization Error",
    	    "Unable to open printing document.\n\n%s\n[Error code: %d]",
    	    EdFail_GetErrorMessage (myErrorBuffer), 
    	    myErrorCode);
    	
    	// Close the printing dialog window and free the printer DC
    	EnableWindow (pmEditWindow, TRUE);
    	DestroyWindow (stPrintDialog);
    	DeleteDC (myPrinterDeviceContext);
    	    
#ifndef TPROLOG
    	if (myIsEditorWindow)
    	{
            EdWin_ShowStatus (pmEditWindow, "Printing Failed");
        }
#endif // #ifndef TPROLOG
        
    	return;
    }

    //
    // Calculate the actual margins
    //
    {
    	int	myXOffset, myYOffset, myXPixPerInch, myYPixPerInch;
    	int	myXPageSize, myYPageSize;
    	int	myXFortiethOfAnInch, myYFortiethOfAnInch;
    	
    	myXOffset = GetDeviceCaps (myPrinterDeviceContext, PHYSICALOFFSETX);
    	myYOffset = GetDeviceCaps (myPrinterDeviceContext, PHYSICALOFFSETY);
    	myXPixPerInch = GetDeviceCaps (myPrinterDeviceContext, LOGPIXELSX);
    	myYPixPerInch = GetDeviceCaps (myPrinterDeviceContext, LOGPIXELSY);
    	myXPageSize = GetDeviceCaps (myPrinterDeviceContext, HORZRES);
    	myYPageSize = GetDeviceCaps (myPrinterDeviceContext, VERTRES);
    	
    	myXFortiethOfAnInch = myXPixPerInch / 40;
    	myYFortiethOfAnInch = myYPixPerInch / 40;
    	
        myRealTopMargin = myYPixPerInch * stProperties.margins.top / 1000 -
            myYOffset;
        myRealLeftMargin = myXPixPerInch * stProperties.margins.left / 1000 -
            myXOffset;
        myRealBottomMargin = myYPageSize - 
            (myYPixPerInch * stProperties.margins.bottom / 1000 - myYOffset);
        myRealRightMargin = myXPageSize - 
            (myXPixPerInch * stProperties.margins.right / 1000 - myXOffset);
            
	myHalfInch = myYPixPerInch / 2;
    	myXTenthOfAnInch = myXPixPerInch / 10;
    	myYTenthOfAnInch = myYPixPerInch / 10;
	myPageWidth = myRealRightMargin - myRealLeftMargin - 
	    2 * myXTenthOfAnInch;
    }
        
    SetBkMode (myPrinterDeviceContext, TRANSPARENT);  
    myTop = myRealTopMargin;
    
    //
    // Print the pages
    //
    for (myCopies = 0 ; myCopies < myPrintDialog.nCopies ; myCopies++)
    {
    	if (StartPage (myPrinterDeviceContext) < 0)
    	{
  	    ErrorBuffer	myErrorBuffer;
	    int		myErrorCode = GetLastError ();
		    
    	    myPrintSuccess = FALSE;
   	    EdGUI_Message (
    	        "Printing Open Page Error",
	        "Unable to open page 1.\n\n%s\n[Error code: %d]",
    	        EdFail_GetErrorMessage (myErrorBuffer), 
    	        myErrorCode);
    	    break;
    	}

   	if (myPrintDialog.nCopies > 1)
    	{
	    wsprintf (myPrintMessage, "Printing Copy %d", myCopies + 1);
    	}
    	else
    	{
	    wsprintf (myPrintMessage, "Printing Graphics");
    	}
    	SetDlgItemText (stPrintDialog, PRINT_PAGENUMBER, myPrintMessage);

    	//
    	// Print the page header   
    	// 
    
    	if ((pmDisplayPrintDialog && 
    	     (stGraphicsPrintProperties & GRAPHICS_HEADER)) ||
    	    (!pmDisplayPrintDialog && pmDrawHeader))
    	{
    	    MyPrintPageHeader (myPrinterDeviceContext, myHeaderFont, 1,
			       myDateTimeString, pmTitle, TRUE, myPageWidth, 
			       myRealLeftMargin, myRealTopMargin, 
			       myRealRightMargin, &myTop);
    	} // if (stProperties.pageHeader)

	myTop += myHalfInch;
	myPageHeight = myRealBottomMargin - myTop - 2 * myXTenthOfAnInch;

	// 
	// Now draw the picture.  We scale the picture up so that a normal 
	// run window fits across an 8.5" page.  If the user made a larger 
	// picture, then we scale it so that it just fits on the page.  If 
	// scale-to-fit is on, then even smaller pictures are scaled up to 
	// fit on the page.
	//

	myXScaleFactor = (double) myPageWidth / pmWidth;
	myYScaleFactor = (double) myPageHeight / pmHeight;

    	if ((pmDisplayPrintDialog && 
    	     (stGraphicsPrintProperties & GRAPHICS_SCALE_TO_FIT)) ||
    	    (!pmDisplayPrintDialog && pmScaleToFit))
	{
	    myScaleFactor = min (myXScaleFactor, myYScaleFactor);
	} 
	else
	{
	    myScaleFactor = (double) min (myPageWidth, myPageHeight) / 
				     pmNormalWidth;
	    myScaleFactor = min (min (myScaleFactor, myXScaleFactor),
			         myYScaleFactor);
	}

	myPictureWidth = (int) myScaleFactor * pmWidth;
	myPictureHeight = (int) myScaleFactor * pmHeight;
	myLeft = ((myRealRightMargin + myRealLeftMargin) - myPictureWidth) / 2;

	myBitMapInfo = MyGetBitMapInfoFromBitMap (pmBitMap, pmPalette, &myBits);

	// Get the BITMAP from the HBITMAP.
	StretchDIBits (myPrinterDeviceContext, myLeft + myXTenthOfAnInch,
		       myTop + myYTenthOfAnInch, myPictureWidth, 
		       myPictureHeight, 0, 0, pmWidth, pmHeight,
		       myBits, myBitMapInfo,
		       DIB_RGB_COLORS, SRCCOPY);

	// Free up the allocated memory.
	free (myBitMapInfo);
	free (myBits);

    	if ((pmDisplayPrintDialog && 
    	    (stGraphicsPrintProperties & GRAPHICS_DRAW_BORDER)) ||
    	    (!pmDisplayPrintDialog && pmDrawBorder))
    	{
    	    // Draw border
	    MoveToEx (myPrinterDeviceContext, myLeft, myTop, NULL);
	    LineTo (myPrinterDeviceContext, 
		    myLeft + myPictureWidth + 2 * myXTenthOfAnInch,
		    myTop);
	    LineTo (myPrinterDeviceContext, 	  
		    myLeft + myPictureWidth + 2 * myXTenthOfAnInch,
		    myTop + myPictureHeight + 2 * myYTenthOfAnInch);
	    LineTo (myPrinterDeviceContext, myLeft,
		    myTop + myPictureHeight + 2 * myYTenthOfAnInch);
	    LineTo (myPrinterDeviceContext, myLeft, myTop);
    	}
	    	    
	if (EndPage (myPrinterDeviceContext) < 0)
    	{
    	    ErrorBuffer	myErrorBuffer;
	    int		myErrorCode = GetLastError ();
		    
    	    myPrintSuccess = FALSE;
    	    EdGUI_Message (
    	        "Printing Close Page Error",
	        "Unable to close page 1.\n\n%s\n[Error code: %d]",
    	        EdFail_GetErrorMessage (myErrorBuffer), 
    	        myErrorCode);
    	    break;
    	}
    		    
	if (stUserAbort)
	{
	    break;
	}
    } // for

    // Close the printer    	    	
    EndDoc (myPrinterDeviceContext);
    
#ifndef TPROLOG
    if (myPrintSuccess && myIsEditorWindow)
    {
    	if (stUserAbort)
    	{
    	    EdWin_ShowStatus (pmEditWindow, "Printing Cancelled by User");
    	}
    	else
    	{
    	    EdWin_ShowStatus (pmEditWindow, "%s Printed", pmTitle);
    	}
    }
    else if (myIsEditorWindow)
    {
    	EdWin_ShowStatus (pmEditWindow, "Printing Failed");
    }    
#endif // #ifndef TPROLOG
    
    if (!stUserAbort)
    {
    	EnableWindow (pmEditWindow, TRUE);
    	DestroyWindow (stPrintDialog);
    }

    DeleteDC (myPrinterDeviceContext);
    
    // Free the fonts
    DeleteObject (myHeaderFont);
} // EdPrint_PrintGraphicsWindow


/************************************************************************/
/* EdPrint_PrintOutput							*/
/************************************************************************/
extern void	EdPrint_PrintOutput (HWND pmWindow, char *pmText, int pmTextLen, 
				     const char *pmClassName)
{
    TextRecord	myTextRecord, *myTextPtr;
    
    myTextPtr = &myTextRecord;
    
    myTextPtr -> text = pmText;
    myTextPtr -> textLen = pmTextLen;
    myTextPtr -> textType = (BYTE *) calloc (pmTextLen, 1);
    EdPrint_PrintEditorWindow (pmWindow, myTextPtr, pmClassName, 
    			       0, FALSE, NULL, NO_SYNTAX_COLOUR, OUTPUT_FILE);
    free (myTextPtr -> textType);    			       
} // EdPrint_PrintOutput

						   
/******************************/
/* Static callback procedures */
/******************************/
/************************************************************************/
/* MyAbortProc								*/
/************************************************************************/
static BOOL CALLBACK	MyAbortProc (HDC pmPrinterDC, int pmCode)
{
    MSG	myMessage;
    
    while (!stUserAbort && PeekMessage (&myMessage, NULL, 0, 0, PM_REMOVE))
    {
    	if (!stPrintDialog || !IsDialogMessage (stPrintDialog, &myMessage))
    	{
    	    TranslateMessage (&myMessage);
    	    DispatchMessage (&myMessage);
    	}
    }
    return !stUserAbort;
} // MyAbortProc


/************************************************************************/
/* MyPrintDialogProcedure						*/
/************************************************************************/
static UINT CALLBACK 	MyPrintDialogProcedure (HWND pmPrintDialogWindow, 
						UINT pmMessage, 
    						WPARAM pmWParam, 
    						LPARAM pmLParam)
{
    switch (pmMessage)
    {
    	case WM_INITDIALOG:
    	    MySetCheckBox (pmPrintDialogWindow, PRINT_BOLD_KEYWORDS, 
    	        stProperties.boldKeywords);
    	    MySetCheckBox (pmPrintDialogWindow, PRINT_ITAL_IDENTS, 
    	        stProperties.italicizeIdents);
    	    MySetCheckBox (pmPrintDialogWindow, PRINT_FULL_PATHNAME, 
    	        stProperties.fullPathName);
    	    MySetCheckBox (pmPrintDialogWindow, PRINT_TWO_UP, 
    	        stProperties.twoUp);
    	    MySetCheckBox (pmPrintDialogWindow, PRINT_LINE_NUMBERS, 
    	        stProperties.lineNumbers);
    	    MySetCheckBox (pmPrintDialogWindow, PRINT_BORDERS, 
    	        stProperties.border);
    	    if ((((LPPRINTDLG) pmLParam) -> lCustData) & SYNTAX_COLOUR_OFF)
    	    {
	        EnableWindow (GetDlgItem (pmPrintDialogWindow, 
	            PRINT_BOLD_KEYWORDS), FALSE);
	        EnableWindow (GetDlgItem (pmPrintDialogWindow, 
	            PRINT_ITAL_IDENTS), FALSE);
    	    }
    	    if ((((LPPRINTDLG) pmLParam) -> lCustData) & FILE_NAME_OFF)
    	    {
	        EnableWindow (GetDlgItem (pmPrintDialogWindow, 
	            PRINT_FULL_PATHNAME), FALSE);
	    }
    	    if (stProperties.twoUp)
    	    {
	        EnableWindow (GetDlgItem (pmPrintDialogWindow, PRINT_BORDERS), 
	            FALSE);
	    }
	    Ed_SetModalDialog (pmPrintDialogWindow); 
	    EdGUI_CentreDialogBox (pmPrintDialogWindow);
	    break;
    	case WM_COMMAND:
    	    if (LOWORD (pmWParam) == IDOK)
    	    {
    	    	stProperties.boldKeywords = 
    	    	    MyCheckedToBool (pmPrintDialogWindow, PRINT_BOLD_KEYWORDS);
    	    	stProperties.italicizeIdents = 
    	    	    MyCheckedToBool (pmPrintDialogWindow, PRINT_ITAL_IDENTS);
    	    	stProperties.fullPathName = 
    	    	    MyCheckedToBool (pmPrintDialogWindow, PRINT_FULL_PATHNAME);
    	    	stProperties.twoUp = 
    	    	    MyCheckedToBool (pmPrintDialogWindow, PRINT_TWO_UP);
    	    	stProperties.lineNumbers = 
    	    	    MyCheckedToBool (pmPrintDialogWindow, PRINT_LINE_NUMBERS);
    	    	stProperties.border = 
    	    	    MyCheckedToBool (pmPrintDialogWindow, PRINT_BORDERS);
    	    } 
    	    else if (LOWORD (pmWParam) == PRINT_TWO_UP)
    	    {
    	    	EnableWindow (GetDlgItem (pmPrintDialogWindow, PRINT_BORDERS),
    	    	    !MyCheckedToBool (pmPrintDialogWindow, PRINT_TWO_UP));
    	    }
    } // switch
    return 0;
} // MyPrintDialogProcedure

    							
/************************************************************************/
/* MyPrintGraphicsDialogProcedure						*/
/************************************************************************/
static UINT CALLBACK 	MyPrintGraphicsDialogProcedure (
			    HWND pmPrintDialogWindow, UINT pmMessage, 
    			    WPARAM pmWParam, LPARAM pmLParam)
{
    switch (pmMessage)
    {
    	case WM_INITDIALOG:
    	    MySetCheckBox (pmPrintDialogWindow, PRINTG_DRAW_BORDER, 
    	        stGraphicsPrintProperties & GRAPHICS_DRAW_BORDER);
    	    MySetCheckBox (pmPrintDialogWindow, PRINTG_SCALE_TO_FIT, 
    	        stGraphicsPrintProperties & GRAPHICS_SCALE_TO_FIT);
    	    MySetCheckBox (pmPrintDialogWindow, PRINTG_HEADER, 
    	        stGraphicsPrintProperties & GRAPHICS_HEADER);
	    Ed_SetModalDialog (pmPrintDialogWindow); 
	    EdGUI_CentreDialogBox (pmPrintDialogWindow);
	    break;
    	case WM_COMMAND:
    	    if (LOWORD (pmWParam) == IDOK)
    	    {
		stGraphicsPrintProperties = 0;
		if (MyCheckedToBool (pmPrintDialogWindow, PRINTG_DRAW_BORDER))
		    stGraphicsPrintProperties += GRAPHICS_DRAW_BORDER;
		if (MyCheckedToBool (pmPrintDialogWindow, PRINTG_SCALE_TO_FIT))
		    stGraphicsPrintProperties += GRAPHICS_SCALE_TO_FIT;
		if (MyCheckedToBool (pmPrintDialogWindow, PRINTG_HEADER))
		    stGraphicsPrintProperties += GRAPHICS_HEADER;
    	    } 
    } // switch
    return 0;
} // MyPrintGraphicsDialogProcedure

    							
#ifndef TPROLOG
/************************************************************************/
/* MyPageSetupDialogProcedure						*/
/************************************************************************/
static UINT CALLBACK 	MyPageSetupDialogProcedure (HWND pmPrintDialogWindow, 
						    UINT pmMessage, 
	    					    WPARAM pmWParam, 
    						    LPARAM pmLParam)
{
    if (pmMessage == WM_INITDIALOG)
    {
	Ed_SetModalDialog (pmPrintDialogWindow); 
	EdGUI_CentreDialogBox (pmPrintDialogWindow);
    }
    return 0;
} // MyPageSetupDialogProcedure
#endif // #ifndef TPROLOG

    							
/************************************************************************/
/* MyPrintStatusDialogProcedure						*/
/************************************************************************/
static BOOL CALLBACK	MyPrintStatusDialogProcedure (HWND pmPrintStatusWindow, 
    UINT pmMessage, WPARAM pmWParam, LPARAM pmLParam)
{
    switch (pmMessage)
    {
    	case WM_INITDIALOG:
    	    EnableMenuItem (GetSystemMenu (pmPrintStatusWindow, FALSE),
    	    	SC_CLOSE, MF_GRAYED);
    	    EdGUI_CentreDialogBox (pmPrintStatusWindow);
    	    return TRUE;
    	    
    	case WM_COMMAND:
    	    stUserAbort = TRUE;
    	    EnableWindow (GetParent (pmPrintStatusWindow), TRUE);
    	    DestroyWindow (pmPrintStatusWindow);
    	    stPrintDialog = NULL;
    	    return TRUE;
    }
    return FALSE;
} // MyPrintStatusDialogProcedure 


/*********************/
/* Static procedures */
/*********************/

/*****************************************************************************/
/* MyCalculateFontSize							     */
/*****************************************************************************/
static int	MyCalculateFontSize (HDC pmPrinterDeviceContext, 
				     BOOL *pmPortrait)
{
    int	myXPageSize, myYPageSize, myTextWidth;
    int	myTextLength;
    int	myLowSize, myMidSize, myHighSize;
    
    myXPageSize = GetDeviceCaps (pmPrinterDeviceContext, HORZRES);
    myYPageSize = GetDeviceCaps (pmPrinterDeviceContext, VERTRES);
    
    //
    // Calculate the width of the text area
    //
    if (myXPageSize > myYPageSize)
    {
    	// Uh oh, we're already in landscape mode
    	int	myXOffset, myXPixPerInch;
    	int	myRealLeftMargin, myRealRightMargin;

    	myXOffset = GetDeviceCaps (pmPrinterDeviceContext, PHYSICALOFFSETX);
    	myXPixPerInch = GetDeviceCaps (pmPrinterDeviceContext, LOGPIXELSX);
        myRealLeftMargin = myXPixPerInch * stProperties.margins.left / 1000 -
            myXOffset;
        myRealRightMargin = myXPageSize - 
            (myXPixPerInch * stProperties.margins.right / 1000 - myXOffset);
	myTextWidth = (myRealRightMargin - myRealLeftMargin) / 2 - 
	    myXPixPerInch / 8;
	    
	*pmPortrait = FALSE;
    }
    else
    {
    	int	myYOffset, myYPixPerInch;
    	int	myRealTopMargin, myRealBottomMargin;

    	myYOffset = GetDeviceCaps (pmPrinterDeviceContext, PHYSICALOFFSETY);
    	myYPixPerInch = GetDeviceCaps (pmPrinterDeviceContext, LOGPIXELSY);
        myRealTopMargin = myYPixPerInch * stProperties.margins.top / 1000 -
            myYOffset;
        myRealBottomMargin = myYPageSize - 
            (myYPixPerInch * stProperties.margins.bottom / 1000 - myYOffset);
	myTextWidth = (myRealBottomMargin - myRealTopMargin) / 2 - 
	    myYPixPerInch / 8;
	    
	*pmPortrait = TRUE;
    }

    //
    // Determine the largest font that will fit 80 or 88 characters into the
    // width
    //    
    if (stProperties.lineNumbers)
    {
    	myTextLength = 88;
    }
    else
    {
    	myTextLength = 80;
    }
    
    // Start with a value of 400 and go from there using a binary search
    myLowSize = 0;
    myHighSize = 800;
    
    while (TRUE)
    {
    	HFONT	myFontSmall, myFontBig;
        LOGFONT	myLogFont;
        SIZE	myExtent;
	int	mySmallLen, myBigLen;
	    	
    	myMidSize = (myLowSize + myHighSize) / 2; 
    	
    	// Create the two font
        ZeroMemory (&myLogFont, sizeof (LOGFONT));
        myLogFont.lfHeight = -myMidSize;
        strcpy (myLogFont.lfFaceName, stProperties.fontName);
        myFontSmall = CreateFontIndirect (&myLogFont);

        myLogFont.lfHeight = -(myMidSize + 1);
        myFontBig = CreateFontIndirect (&myLogFont);

	// Select first font into the context and get the length of 80 chars
	SelectFont (pmPrinterDeviceContext, myFontSmall);
    	GetTextExtentPoint32 (pmPrinterDeviceContext, 
    	    "0000000000000000000000000000000000000000000000000000"
    	    "000000000000000000000000000000000000", myTextLength, &myExtent);
    	mySmallLen = myExtent.cx;
    	DeleteObject (SelectFont (pmPrinterDeviceContext, myFontBig));
	SelectFont (pmPrinterDeviceContext, myFontBig);
    	GetTextExtentPoint32 (pmPrinterDeviceContext, 
    	    "0000000000000000000000000000000000000000000000000000"
    	    "000000000000000000000000000000000000", myTextLength, &myExtent);
    	myBigLen = myExtent.cx;
    	DeleteObject (SelectFont (pmPrinterDeviceContext, 
    	    GetStockObject (SYSTEM_FONT)));
    	    
    	if ((mySmallLen <= myTextWidth) && (myTextWidth <= myBigLen))
    	{
    	    return myMidSize;
    	}
    	else
    	{
    	    if (myTextWidth > myBigLen)
    	    {
    	    	// Make the font larger
    	    	myLowSize  = myMidSize + 1;
    	    }
    	    else
    	    {
    	    	// Make the font larger
    	    	myHighSize = myMidSize;
    	    }
    	}
    } // while
} // MyCalculateFontSize


/************************************************************************/
/* MyCheckedToBool							*/
/************************************************************************/
static BOOL	MyCheckedToBool (HWND pmDialog, int pmCheckBox)
{
    return (IsDlgButtonChecked (pmDialog, pmCheckBox) == BST_CHECKED);
} // MyCheckedToBool


/*****************************************************************************/
/* MyGetBitMapInfoFromBitMap						     */
/*****************************************************************************/
static BITMAPINFO	*MyGetBitMapInfoFromBitMap (HBITMAP pmBitMap, 
						    HPALETTE pmPalette,
						    BYTE **pmBits)
{
    BITMAP		myBitMap;		// bitmap structure
    BITMAPINFO		*myBitMapInfo;		// bitmap header
    int			myColoursUsed;		// # of colours in DIB
    HDC			myDC;                   // handle to DC
    WORD		myBitsPerPixel;         // bits per pixel

    // Check if bitmap handle is valid
    if (!pmBitMap)
    {
	return NULL;
    }

    // Fill in BITMAP structure, return NULL if it didn't work
    if (!GetObject (pmBitMap, sizeof (myBitMap), (LPSTR) &myBitMap))
    {
	return NULL;
    }

    // If no palette is specified, use default palette */
    if (pmPalette == NULL)
    {
	pmPalette = GetStockObject (DEFAULT_PALETTE);
    }

    // Calculate bits per pixel
    myBitsPerPixel = myBitMap.bmPlanes * myBitMap.bmBitsPixel;

    // Make sure bits per pixel is valid
    if (myBitsPerPixel <= 1)
    {
	myBitsPerPixel = 1;
	myColoursUsed = 2;
    }
    else if (myBitsPerPixel <= 4)
    {
	myBitsPerPixel = 4;
	myColoursUsed = 16;
    }
    else if (myBitsPerPixel <= 8)
    {
	myBitsPerPixel = 8;
	myColoursUsed = 256;
    }
    else /* if greater than 8-bit, force to 24-bit */
    {
	myBitsPerPixel = 24;
	myColoursUsed = 0;
    }

    myBitMapInfo = malloc (sizeof (BITMAPINFOHEADER) + 
			   sizeof (RGBQUAD) * myColoursUsed);

    // Initialize BITMAPINFOHEADER
    myBitMapInfo -> bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    myBitMapInfo -> bmiHeader.biWidth = myBitMap.bmWidth;
    myBitMapInfo -> bmiHeader.biHeight = myBitMap.bmHeight;
    myBitMapInfo -> bmiHeader.biPlanes = 1;
    myBitMapInfo -> bmiHeader.biBitCount = myBitsPerPixel;
    myBitMapInfo -> bmiHeader.biCompression = BI_RGB;
    myBitMapInfo -> bmiHeader.biSizeImage = 0;
    myBitMapInfo -> bmiHeader.biXPelsPerMeter = 0;
    myBitMapInfo -> bmiHeader.biYPelsPerMeter = 0;
    myBitMapInfo -> bmiHeader.biClrUsed = 0;
    myBitMapInfo -> bmiHeader.biClrImportant = 0;

    // Get a DC
    myDC = GetDC(NULL);

    // Select and realize our palette */
    pmPalette = SelectPalette (myDC, pmPalette, FALSE);
    RealizePalette (myDC);

   /* alloc memory block to store our bitmap */
//   hDIB = GlobalAlloc(GHND, myLen);

   /* if we couldn't get memory block */
//   if (!hDIB)
//   {
      /* clean up and return NULL */
  //    SelectPalette(hDC, hPal, TRUE);
    //  RealizePalette(hDC);
      //ReleaseDC(NULL, hDC);
//      return NULL;
  // }

   /* lock memory and get pointer to it */
//   myBitMapInfoHdrPtr = (VOID FAR *)GlobalLock(hDIB);

   /* use our bitmap info. to fill BITMAPINFOHEADER */
//   *myBitMapInfoHdrPtr = myBitMapInfoHdr;

    // Call GetDIBits with a NULL lpBits param, so it will calculate the
    // biSizeImage field for us
    GetDIBits (myDC, pmBitMap, 0, (WORD) myBitMapInfo -> bmiHeader.biHeight, 
	       NULL, (LPBITMAPINFO) myBitMapInfo, DIB_RGB_COLORS);

   /* get the info. returned by GetDIBits and unlock memory block */
//   myBitMapInfoHdr = *myBitMapInfoHdrPtr;
//   GlobalUnlock(hDIB);

    // If the driver did not fill in the biSizeImage field, make one up */
    if (myBitMapInfo -> bmiHeader.biSizeImage == 0)
    {
	myBitMapInfo -> bmiHeader.biSizeImage = 
	    ((DWORD) myBitMap.bmWidth * myBitsPerPixel) + 31 / 32  * 4 *
	    myBitMap.bmHeight;
    }

    // Allocate a buffer to hold the bits.
    *pmBits = malloc (myBitMapInfo -> bmiHeader.biSizeImage);
//   myLen = myBitMapInfoHdr.biSize + PaletteSize((LPSTR)&bi) + myBitMapInfoHdr.biSizeImage;
   //if (h = GlobalReAlloc(hDIB, myLen, 0))
     // hDIB = h;
//   else
  // {
      /* clean up and return NULL */
    //  GlobalFree(hDIB);
      //hDIB = NULL;
      //SelectPalette(hDC, hPal, TRUE);
      //RealizePalette(hDC);
      //ReleaseDC(NULL, hDC);
      //return NULL;
   //}

   /* lock memory block and get pointer to it */
//   myBitMapInfoHdrPtr = (VOID FAR *)GlobalLock(hDIB);

    // Call GetDIBits with a NON-NULL lpBits param, and actualy get the
    // bits this time.
    GetDIBits (myDC, pmBitMap, 0, (WORD) myBitMapInfo -> bmiHeader.biHeight, 
	       *pmBits, (LPBITMAPINFO) myBitMapInfo, DIB_RGB_COLORS);
    // Clean up
//    GlobalUnlock(hDIB);
    SelectPalette (myDC, pmPalette, TRUE);
    RealizePalette (myDC);
    ReleaseDC (NULL, myDC);

    return myBitMapInfo;
} // MyGetBitMapInfoFromBitMap


/*****************************************************************************/
/* MyPrintPageHeader							     */
/*****************************************************************************/
static void	MyPrintPageHeader (HDC pmPrinterDeviceContext, 
				   HFONT pmHeaderFont, int pmPageNumber,
				   char *pmDate, char *pmJobName, 
				   BOOL pmPrinting,
				   int pmPageWidth, int pmLeftMargin,
				   int pmTopMargin, int pmRightMargin,
				   int *pmBaseLine)
{
    TEXTMETRIC	myTextMetrics;    
    SIZE	myExtent;
    BOOL	myOneLineHeader;
    int		myDateSize, myJobSize, myPageSize, mySpacesSize;
    char	myPageString [100], myUserName [128], *myUserNamePtr;           
    	
    SelectFont (pmPrinterDeviceContext, pmHeaderFont);
    GetTextMetrics (pmPrinterDeviceContext, &myTextMetrics);
    	
    // Header formats
    //    No user name
    //        Date            Job             Page
    //    No user name - all does not fit on one line
    //                        Job
    //        Date                            Page
    //    User name - all does not fit on one line
    //        User                             Job
    //        Date                            Page
    	
    // Get the user name string
    if (stProperties.userName [0] == 0)
    {
    	DWORD	myBufferSize = 128;

    	if (!GetUserName (myUserName, &myBufferSize))
    	{
    	    myUserName [0] = 0;
    	}
    	myUserNamePtr = myUserName;
    }
    else
    {
    	myUserNamePtr = stProperties.userName;
    }

    // Get the page number string
    wsprintf (myPageString, "Page %d", pmPageNumber);

    // Get the width of two spaces
    GetTextExtentPoint32 (pmPrinterDeviceContext, "  ", 2, &myExtent);
    mySpacesSize = myExtent.cx;
	
    // Determine the sizes of each of the header elements
    GetTextExtentPoint32 (pmPrinterDeviceContext, pmDate, 
	    strlen (pmDate), &myExtent);
    myDateSize = myExtent.cx;
    GetTextExtentPoint32 (pmPrinterDeviceContext, pmJobName,
	    strlen (pmJobName), &myExtent);
    myJobSize = myExtent.cx;
    GetTextExtentPoint32 (pmPrinterDeviceContext, myPageString,
	    strlen (myPageString), &myExtent);
    myPageSize = myExtent.cx;
	
    // Determine whether the header fits on one line or two
    if (stProperties.printUserName ||
	((myJobSize / 2 + myDateSize + mySpacesSize) > pmPageWidth) ||
	((myJobSize / 2 + myPageSize + mySpacesSize) > pmPageWidth))
    {
    	myOneLineHeader = FALSE;
    }
    else
    {
    	myOneLineHeader = TRUE;
    }
	    
    if (pmPrinting)
    {
    	if (myOneLineHeader)
    	{
    	    //    No user name
    	    //        Date            Job             Page
    		
    	    // Print the date on the left
	    SetTextAlign (pmPrinterDeviceContext, TA_LEFT);
    	    TextOut (pmPrinterDeviceContext, pmLeftMargin, pmTopMargin, 
    	    	     pmDate, strlen (pmDate));
    	            
    	    // Print job name middle
	    SetTextAlign (pmPrinterDeviceContext, TA_CENTER);
    	    TextOut (pmPrinterDeviceContext, (pmRightMargin + pmLeftMargin / 2), 
    	    	     pmTopMargin, pmJobName, strlen (pmJobName));

	    // Print the page count on the right
	    SetTextAlign (pmPrinterDeviceContext, TA_RIGHT);
	    TextOut (pmPrinterDeviceContext, pmRightMargin, 
    	    	     pmTopMargin, myPageString, strlen (myPageString));

    	    // Draw a line beneath the header
    	    MoveToEx (pmPrinterDeviceContext, pmLeftMargin, 
    	              pmTopMargin + myTextMetrics.tmHeight, NULL);
    	    LineTo (pmPrinterDeviceContext, pmRightMargin, 
    	            pmTopMargin + myTextMetrics.tmHeight);
    	} // if (myOneLineHeader)
    	else
    	{
    	    if (stProperties.printUserName)
    	    {
    		//    User name - all does not fit on one line
    		//        User                             Job
    		//        Date                            Page
    		    
    	    	// Print the user name on the left
	    	SetTextAlign (pmPrinterDeviceContext, TA_LEFT);
    	    	TextOut (pmPrinterDeviceContext, pmLeftMargin, 
    	    	         pmTopMargin, myUserNamePtr, strlen (myUserNamePtr));

    	    	// Print the job name on the right
	    	SetTextAlign (pmPrinterDeviceContext, TA_RIGHT);
	    	TextOut (pmPrinterDeviceContext, pmRightMargin, 
    	    	         pmTopMargin, pmJobName, strlen (pmJobName));
	    }
	    else
	    {
    		//    User name - all does not fit on one line
    		//                       Job
    		//        Date                            Page

    	        // Print job name middle
	    	SetTextAlign (pmPrinterDeviceContext, TA_CENTER);
    	    	TextOut (pmPrinterDeviceContext, 
    	    		 (pmRightMargin + pmLeftMargin / 2), 
    	    	    	 pmTopMargin, pmJobName, strlen (pmJobName));
	    }
		
    	    // Print the date on the left
	    SetTextAlign (pmPrinterDeviceContext, TA_LEFT);
    	    TextOut (pmPrinterDeviceContext, pmLeftMargin, 
    	    	     pmTopMargin + myTextMetrics.tmHeight, 
    	    	     pmDate, strlen (pmDate));
    	            
	    // Print the page count on the right side
	    SetTextAlign (pmPrinterDeviceContext, TA_RIGHT);
	    TextOut (pmPrinterDeviceContext, pmRightMargin, 
    	    	     pmTopMargin + myTextMetrics.tmHeight, 
    	    	     myPageString, strlen (myPageString));

    	    // Draw a line beneath the header
    	    MoveToEx (pmPrinterDeviceContext, pmLeftMargin, 
    	              pmTopMargin + 2 * myTextMetrics.tmHeight, NULL);
    	    LineTo (pmPrinterDeviceContext, pmRightMargin, 
    	            pmTopMargin + 2 * myTextMetrics.tmHeight);
    	} // else - if (myOneLineHeader)
    	    
	SetTextAlign (pmPrinterDeviceContext, TA_LEFT);
    } // if (pmPrint)
    	
    if (!myOneLineHeader)
    {
    	*pmBaseLine += myTextMetrics.tmHeight;
    }
    	
    *pmBaseLine += myTextMetrics.tmHeight + 2;
} // MyPrintPageHeader


/*****************************************************************************/
/* MyPrintProgramPage							     */
/*									     */
/* Parameters: 								     */
/*    pmPrinterDeviceContext	The printer device context                   */
/*    pmPageNumber		The current page number                      */
/*    pmPrinting		Whether this page should be physically       */
/*				printed (pages contain a variable number     */
/*				of lines)				     */
/*    pmTextPtr			The text record to be printed                */
/*    *pmCurrentPos		The location in the text where this page's   */
/*				printing starts (array index)		     */	
/*    pmLastPos			The location in the text to end printing     */
/*				(array index)				     */
/*    *pmJobName		The job name to appear on the header	     */
/*    *pmDate			The date to appear on the header	     */
/*    *pmLineNumber		The current line number in the text          */
/*    *pmStartLine		Whether the first line on the page is the    */
/*				start of a line of text			     */
/*    *pmStartSpace		The space in pixels at the start of a line.  */
/*				used when the indentation at the beginning   */
/*                              of a line is longer than a printer line      */
/*    pmFont, pmBoldFont, pmItalicFont, pmHeaderFont Fonts used for printing */		
/*****************************************************************************/
static void	MyPrintProgramPage (HDC pmPrinterDeviceContext, 
				    int pmPageNumber, BOOL pmPrinting,
				    TextPtr pmTextPtr, int *pmCurrentPos,
				    int pmLastPos, char *pmJobName,
				    char *pmDate, int *pmLineNumber, 
				    BOOL *pmStartOfLine, int *pmStartSpace,
				    HFONT pmFont, HFONT pmBoldFont, 
				    HFONT pmItalicFont, HFONT pmHeaderFont)
				    
{
    // The margins in pixels using the user specified values for margins
    int		myRealTopMargin, myRealLeftMargin;
    int		myRealBottomMargin, myRealRightMargin;
    int		myXFortiethOfAnInch, myYFortiethOfAnInch;
    // The text metrics for a font
    TEXTMETRIC	myTextMetrics;    
    int		myLineHeight, myBaseLine;
    int		myPageWidth, myLineNumberRightMargin;
    int		myTextLeftMargin, myTextWidth;
    SIZE	myExtent;
    int		myEndOfLinePos, myCurrentPos, myPos;
    int		myRemainingTextWidth, myCurrentLeftMargin;
    BOOL	myEndOfLineReached;

    //
    // Calculate the actual margins
    //
    {
    	int	myXOffset, myYOffset, myXPixPerInch, myYPixPerInch;
    	int	myXPageSize, myYPageSize;
    	
    	myXOffset = GetDeviceCaps (pmPrinterDeviceContext, PHYSICALOFFSETX);
    	myYOffset = GetDeviceCaps (pmPrinterDeviceContext, PHYSICALOFFSETY);
    	myXPixPerInch = GetDeviceCaps (pmPrinterDeviceContext, LOGPIXELSX);
    	myYPixPerInch = GetDeviceCaps (pmPrinterDeviceContext, LOGPIXELSY);
    	myXPageSize = GetDeviceCaps (pmPrinterDeviceContext, HORZRES);
    	myYPageSize = GetDeviceCaps (pmPrinterDeviceContext, VERTRES);
    	
    	myXFortiethOfAnInch = myXPixPerInch / 40;
    	myYFortiethOfAnInch = myYPixPerInch / 40;
    	
        myRealTopMargin = myYPixPerInch * stProperties.margins.top / 1000 -
            myYOffset;
        myRealLeftMargin = myXPixPerInch * stProperties.margins.left / 1000 -
            myXOffset;
        myRealBottomMargin = myYPageSize - 
            (myYPixPerInch * stProperties.margins.bottom / 1000 - myYOffset);
        myRealRightMargin = myXPageSize - 
            (myXPixPerInch * stProperties.margins.right / 1000 - myXOffset);
            
	myPageWidth = myRealRightMargin - myRealLeftMargin;            
    }
        
    SetBkMode (pmPrinterDeviceContext, TRANSPARENT);  
    SelectFont (pmPrinterDeviceContext, pmFont);
    GetTextMetrics (pmPrinterDeviceContext, &myTextMetrics);
    myLineHeight = myTextMetrics.tmHeight;
    myBaseLine = myRealTopMargin + myTextMetrics.tmAscent;
    
    // If we're using line numbers, calculate the margins
    if (stProperties.lineNumbers)
    {
    	GetTextExtentPoint32 (pmPrinterDeviceContext, "00000000", 8, &myExtent);
    	myTextLeftMargin = myRealLeftMargin + myExtent.cx;
    	myTextWidth = myPageWidth - myExtent.cx;
    	GetTextExtentPoint32 (pmPrinterDeviceContext, "00000", 5, &myExtent);
    	myLineNumberRightMargin = myRealLeftMargin + myExtent.cx;
    }
    else
    {
    	myTextLeftMargin = myRealLeftMargin;
    	myTextWidth = myPageWidth;
    }
 
    //
    // Print the page header   
    // 
    
    if (stProperties.pageHeader)
    {
    	MyPrintPageHeader (pmPrinterDeviceContext, pmHeaderFont, pmPageNumber,
			   pmDate, pmJobName, pmPrinting, myPageWidth, 
			   myRealLeftMargin, myRealTopMargin, myRealRightMargin,
			   &myBaseLine);
    } // if (stProperties.pageHeader)

    SelectFont (pmPrinterDeviceContext, pmFont);
    
    myCurrentPos = *pmCurrentPos;
    
    //
    // Draw the border, if desired
    //
    if (stProperties.border)
    {
    	RECT	myBorder;
    	
    	myBorder.top = myRealTopMargin - myYFortiethOfAnInch;
    	myBorder.left = myRealLeftMargin - myXFortiethOfAnInch;
    	myBorder.bottom = myRealBottomMargin + myYFortiethOfAnInch;
    	myBorder.right = myRealRightMargin + myXFortiethOfAnInch;
    	
    	FrameRect (pmPrinterDeviceContext, &myBorder, 
    	    GetStockObject (LTGRAY_BRUSH));
    }
    
    /*************************************************************************/
    /* Main Loop: Loop once through this for every printed line (not program */
    /*  line). myCurrentPos is the place to start printing from		     */
    /*************************************************************************/
    while (myCurrentPos < pmLastPos)
    {
    	// Draw the line number, if applicable
    	if (pmPrinting && stProperties.lineNumbers && *pmStartOfLine)
    	{
    	    char	myLineNumberString [128];
    	    
    	    wsprintf (myLineNumberString, "%d", *pmLineNumber);
    	    SetTextAlign (pmPrinterDeviceContext, TA_RIGHT);
    	    TextOut (pmPrinterDeviceContext, myLineNumberRightMargin, 
    	        myBaseLine, myLineNumberString, strlen (myLineNumberString));
    	    SetTextAlign (pmPrinterDeviceContext, TA_LEFT);
	}
	
	myRemainingTextWidth = myTextWidth;
	myCurrentLeftMargin = myTextLeftMargin;

	// Set myEndOfLinePos to the end of line (or last character)
	myEndOfLinePos = myCurrentPos;
	while ((myEndOfLinePos < pmLastPos) && 
	    (pmTextPtr -> text [myEndOfLinePos] != '\r') &&
	    (pmTextPtr -> text [myEndOfLinePos] != '\n'))
	{
	    myEndOfLinePos++;
	}
	
	// Calculate the leading space.  Initial spaces have the size of a 'm'.
	// We don't just print spaces because of non-proportional printing 
	// fonts
	if (*pmStartOfLine)
	{
	    myPos = myCurrentPos;
	    while ((pmTextPtr -> text [myPos] == ' ') && 
	        (myPos < myEndOfLinePos))
	    {
	    	myPos++;
	    }
	    GetTextExtentPoint32 (pmPrinterDeviceContext, " ", 1, &myExtent);

	    *pmStartSpace = (myPos - myCurrentPos) * myExtent.cx;
	    myCurrentPos = myPos;
	}

	// We set pmStartOfLine to false for every escape inthe middle of a line
	*pmStartOfLine = TRUE;
	
	// If the indentation won't fit on one line, skip to the next line
	if (myRemainingTextWidth <= *pmStartSpace)
	{
	    *pmStartSpace -= myRemainingTextWidth;
	    *pmStartOfLine = FALSE;
	    break;
	}
	else
	{
	    // Otherwise, indent appropriately
	    myRemainingTextWidth -= *pmStartSpace;
	    myCurrentLeftMargin += *pmStartSpace;
	    *pmStartSpace = 0;
	}

	/****************************************************************/
	/* Now loop, printing non-ident/non-keyword, then ident/keyword	*/
	/* until the end of the print line				*/
	/****************************************************************/
	while (myCurrentPos < myEndOfLinePos)
	{
	    /************************************************************/
	    /* Print all non-identifier/non-keywords one char at a time */
	    /************************************************************/
	    myPos = myCurrentPos;
	    myEndOfLineReached = FALSE;
		
	    // Move myPos until reaching an identifier, a keyword, the end 
	    // of the line or until no more will fit on the line.
  	    while ((myPos < myEndOfLinePos) && 
	        (pmTextPtr -> textType [myPos] != TEXT_IDENTIFIER) &&
	        (pmTextPtr -> textType [myPos] != TEXT_KEYWORD))
	    {
	        GetTextExtentPoint32 (pmPrinterDeviceContext, 
	            &pmTextPtr -> text [myCurrentPos], myPos - myCurrentPos, 
	            &myExtent);
	        if (myExtent.cx > myRemainingTextWidth)
	        {
	    	    myEndOfLineReached = TRUE;
	    	    *pmStartOfLine = FALSE;
	            break;
	        }
	        myPos++;
	    }
	
	    // If there are characters to be printed, print them.
	    if (myPos > myCurrentPos)
	    {
    	        SelectFont (pmPrinterDeviceContext, pmFont);
    	        if (pmPrinting)
    	        {
	    	    SetTextAlign (pmPrinterDeviceContext, TA_LEFT);
    	            TextOut (pmPrinterDeviceContext, myCurrentLeftMargin, 
    	                myBaseLine, &pmTextPtr -> text [myCurrentPos], 
    	                myPos - myCurrentPos);
    	        }
	        GetTextExtentPoint32 (pmPrinterDeviceContext, 
	            &pmTextPtr -> text [myCurrentPos], myPos - myCurrentPos, 
	            &myExtent);
	        myCurrentLeftMargin += myExtent.cx;
	        myRemainingTextWidth -= myExtent.cx;
	        myCurrentPos = myPos;
	    }
	
	    // If we've reached the end of the text line or the printer line,
	    // break
	    if ((myCurrentPos >= myEndOfLinePos) || myEndOfLineReached)
	    {
	        break;
	    }
	    
	    /********************************************************/
	    /* Print the identifier/keyword, if it fits on the line */
	    /********************************************************/
	    myPos = myCurrentPos + 1;
	
	    // Move myPos until reaching the end of the identifier or keyword
	    while ((myCurrentPos < myEndOfLinePos) && 
	        (pmTextPtr -> textType [myPos] == 
	            pmTextPtr -> textType [myCurrentPos]))
	    {
	        myPos++;
	    }

	    // Make certain the identifier/keyword will fit on the line
	    if (pmTextPtr -> textType [myCurrentPos] == TEXT_IDENTIFIER)
	    {
	        SelectFont (pmPrinterDeviceContext, pmItalicFont);
	    }
	    else
	    {
	        SelectFont (pmPrinterDeviceContext, pmBoldFont);
	    }
	    GetTextExtentPoint32 (pmPrinterDeviceContext, 
	        &pmTextPtr -> text [myCurrentPos], myPos - myCurrentPos, 
	        &myExtent);
	    if (myExtent.cx > myRemainingTextWidth)
	    {
	        SelectFont (pmPrinterDeviceContext, pmFont);
	        *pmStartOfLine = FALSE;
	        break;
	    }

	    if (myPos > myCurrentPos)
	    {
    	        if (pmPrinting)
    	        {
	    	    SetTextAlign (pmPrinterDeviceContext, TA_LEFT);
    	            TextOut (pmPrinterDeviceContext, myCurrentLeftMargin, 
    	                myBaseLine, &pmTextPtr -> text [myCurrentPos], 
    	                myPos - myCurrentPos);
    	        }
	        myCurrentLeftMargin += myExtent.cx;
	        myRemainingTextWidth -= myExtent.cx;
	        myCurrentPos = myPos;
	        SelectFont (pmPrinterDeviceContext, pmFont);
	    }
	} // while (myCurrentPos < myEndOfLinePos)
	
    	/*****************************************/	    	
    	/* We've reached the end of a print line */
    	/*****************************************/
    	if (*pmStartOfLine)
    	{
    	    *pmLineNumber += 1;
    	    // Assert that pmTextPtr -> text [myCurrentPos] == '\r' or '\n'
	    if ((pmTextPtr -> text [myCurrentPos] == '\r') &&
	        (pmTextPtr -> text [myCurrentPos + 1] == '\n'))
	    {
	    	myCurrentPos++;
	    }	        
    	    myCurrentPos++;
    	}
    	
	myBaseLine += myLineHeight;
    	if (myBaseLine + myLineHeight > myRealBottomMargin)
    	{
    	    break;
    	}
    } // while (myCurrentPos < pmLastPos)
    	
    *pmCurrentPos = myCurrentPos;
} // MyPrintProgramPage


/*****************************************************************************/
/* MyPrintTwoUpProgramPage						     */
/*									     */
/* Parameters: 								     */
/*    pmPrinterDeviceContext	The printer device context                   */
/*    pmPageNumber		The current page number                      */
/*    pmPrinting		Whether this page should be physically       */
/*				printed (pages contain a variable number     */
/*				of lines)				     */
/*    pmTextPtr			The text record to be printed                */
/*    *pmCurrentPos		The location in the text where this page's   */
/*				printing starts (array index)		     */	
/*    pmLastPos			The location in the text to end printing     */
/*				(array index)				     */
/*    *pmJobName		The job name to appear on the header	     */
/*    *pmDate			The date to appear on the header	     */
/*    *pmLineNumber		The current line number in the text          */
/*    *pmStartLine		Whether the first line on the page is the    */
/*				start of a line of text			     */
/*    *pmStartSpace		The space in pixels at the start of a line.  */
/*				used when the indentation at the beginning   */
/*                              of a line is longer than a printer line      */
/*    pmFont, pmBoldFont, pmItalicFont, pmHeaderFont Fonts used for printing */		
/*****************************************************************************/
static void	MyPrintTwoUpProgramPage (HDC pmPrinterDeviceContext, 
				    	 int pmPageNumber, BOOL pmPrinting,
				    	 TextPtr pmTextPtr, int *pmCurrentPos,
				    	 int pmLastPos, char *pmJobName,
				    	 char *pmDate, int *pmLineNumber, 
				    	 BOOL *pmStartOfLine, int *pmStartSpace,
				    	 HFONT pmFont, HFONT pmBoldFont, 
				    	 HFONT pmItalicFont,
				    	 HFONT pmHeaderFont)
{
    // The margins in pixels using the user specified values for margins
    // Note that we will use portrait oriented notation even though we're
    // printing in landscape mode
    int		myRealTopMargin, myRealLeftMargin;
    int		myRealBottomMargin, myRealRightMargin;
    int		myXFortiethOfAnInch, myYFortiethOfAnInch;
    int		myYTopPage1, myYBottomPage2;
    int		myPenWidth;
    TEXTMETRIC	myTextMetrics;
    HPEN	myLinePen;    
    char	myPageString [100], myUserName [128], *myUserNamePtr;
    int		mySubPage;
    int		myLineHeight, myBaseLine;
    int		myTextBottomMargin, myTextWidth;
    int		myLineNumberTopMargin;
    int		myEndOfLinePos, myCurrentPos, myPos;
    int		myRemainingTextWidth, myCurrentBottomMargin;
    BOOL	myEndOfLineReached;
    SIZE	myExtent;

    //
    // Calculate the actual margins
    //
    {
    	int	myXOffset, myYOffset, myXPixPerInch, myYPixPerInch;
    	int	myXPageSize, myYPageSize;
    	
    	myXOffset = GetDeviceCaps (pmPrinterDeviceContext, PHYSICALOFFSETX);
    	myYOffset = GetDeviceCaps (pmPrinterDeviceContext, PHYSICALOFFSETY);
    	myXPixPerInch = GetDeviceCaps (pmPrinterDeviceContext, LOGPIXELSX);
    	myYPixPerInch = GetDeviceCaps (pmPrinterDeviceContext, LOGPIXELSY);
    	myXPageSize = GetDeviceCaps (pmPrinterDeviceContext, HORZRES);
    	myYPageSize = GetDeviceCaps (pmPrinterDeviceContext, VERTRES);
    	
    	myXFortiethOfAnInch = myXPixPerInch / 40;
    	myYFortiethOfAnInch = myYPixPerInch / 40;
    	
        myRealTopMargin = myYPixPerInch * stProperties.margins.top / 1000 -
            myYOffset;
        myRealLeftMargin = myXPixPerInch * stProperties.margins.left / 1000 -
            myXOffset;
        myRealBottomMargin = myYPageSize - 
            (myYPixPerInch * stProperties.margins.bottom / 1000 - myYOffset);
        myRealRightMargin = myXPageSize - 
            (myXPixPerInch * stProperties.margins.right / 1000 - myXOffset);

	myYTopPage1 = (myRealTopMargin + myRealBottomMargin) / 2 + 
	    myYPixPerInch / 8;
	myYBottomPage2 = (myRealTopMargin + myRealBottomMargin) / 2 - 
	    myYPixPerInch / 8;
	    
	myPenWidth = myXPixPerInch / 72;

	if (myXPageSize > myYPageSize)
	{
	    // TW Assert FALSE!
	}
    }

    // Create the header font
    SetBkMode (pmPrinterDeviceContext, TRANSPARENT);

    // Get the user name string
    if (stProperties.userName [0] == 0)
    {
    	DWORD	myBufferSize = 128;
    	    
    	GetUserName (myUserName, &myBufferSize);
    	myUserNamePtr = myUserName;
    }
    else
    {
    	myUserNamePtr = stProperties.userName;
    }
    	
    for (mySubPage = 1 ; mySubPage <= 2 ; mySubPage++)
    {
    	int	mySubPageTop, mySubPageBottom;
    	
    	// Draw the outlines
    	SelectFont (pmPrinterDeviceContext, pmHeaderFont);
    	GetTextMetrics (pmPrinterDeviceContext, &myTextMetrics);
    	
    	if (mySubPage == 1)
    	{
    	    mySubPageTop = myYTopPage1;
    	    mySubPageBottom = myRealBottomMargin;
	}
	else
	{
    	    mySubPageTop = myRealTopMargin;
    	    mySubPageBottom = myYBottomPage2;
	}    	    
	
    	if (pmPrinting)
    	{
    	    
	    myLinePen = CreatePen (PS_SOLID, myPenWidth, 0);
	    SelectObject (pmPrinterDeviceContext, myLinePen);
    
	    	
	    // Draw page
	    MoveToEx (pmPrinterDeviceContext, 
	    	myRealLeftMargin - myXFortiethOfAnInch, 
	        mySubPageBottom + myYFortiethOfAnInch, NULL);
	    LineTo (pmPrinterDeviceContext, 
	    	myRealRightMargin + myXFortiethOfAnInch,
	    	mySubPageBottom + myYFortiethOfAnInch);
	    LineTo (pmPrinterDeviceContext, 
	    	myRealRightMargin + myXFortiethOfAnInch,
        	mySubPageTop - myYFortiethOfAnInch);
    	    LineTo (pmPrinterDeviceContext, 
    		myRealLeftMargin - myXFortiethOfAnInch,
		mySubPageTop - myYFortiethOfAnInch);
	    LineTo (pmPrinterDeviceContext, 
		myRealLeftMargin - myXFortiethOfAnInch, 
	        mySubPageBottom + myYFortiethOfAnInch);
	    MoveToEx (pmPrinterDeviceContext, 
	    	myRealLeftMargin + 2 * myTextMetrics.tmHeight, 
	        mySubPageBottom + myYFortiethOfAnInch, NULL);
	    LineTo (pmPrinterDeviceContext, 
	    	myRealLeftMargin + 2 * myTextMetrics.tmHeight, 
	        mySubPageTop - myYFortiethOfAnInch);
	    
	    // Draw the headings
	    if (stProperties.printUserName)
	    {
	    	//    User name - all does not fit on one line
	    	//        User                             Job
	    	//        Date                            Page

	    	// Print the user name on the left
		SetTextAlign (pmPrinterDeviceContext, TA_LEFT);
	    	TextOut (pmPrinterDeviceContext, myRealLeftMargin, 
	    	    mySubPageBottom, myUserNamePtr, strlen (myUserNamePtr));

	    	// Print the job name on the right
		SetTextAlign (pmPrinterDeviceContext, TA_RIGHT);
		TextOut (pmPrinterDeviceContext, myRealLeftMargin, 
		    mySubPageTop, pmJobName, strlen (pmJobName));
    	    }
    	    else
	    {
    		//    User name - all does not fit on one line
    		//                       Job
    		//        Date                            Page

    	        // Print job name middle
	        SetTextAlign (pmPrinterDeviceContext, TA_CENTER);
    	        TextOut (pmPrinterDeviceContext, myRealLeftMargin,
    	            (mySubPageBottom + mySubPageTop) / 2, 
    	            pmJobName, strlen (pmJobName));
	    }
    	    
    	    // Print the date on the left
	    SetTextAlign (pmPrinterDeviceContext, TA_LEFT);
    	    TextOut (pmPrinterDeviceContext, myRealLeftMargin + 
    	        myTextMetrics.tmHeight, mySubPageBottom,
    	        pmDate, strlen (pmDate));
    	            
	    // Get the page number string
	    wsprintf (myPageString, "Page %d", 
	        pmPageNumber * 2 - 2 + mySubPage);

	    // Print the page count on the right side
	    SetTextAlign (pmPrinterDeviceContext, TA_RIGHT);
	    TextOut (pmPrinterDeviceContext, myRealLeftMargin + 
    	    	    myTextMetrics.tmHeight,  mySubPageTop,
		    myPageString, strlen (myPageString));
		    
	    SelectObject (pmPrinterDeviceContext, GetStockObject (BLACK_PEN));
	    DeleteObject (myLinePen);
	}

	myBaseLine = myRealLeftMargin + 2 * myTextMetrics.tmHeight + 2;   
	
    	SelectFont (pmPrinterDeviceContext, pmFont);
    	GetTextMetrics (pmPrinterDeviceContext, &myTextMetrics);
    	myLineHeight = myTextMetrics.tmHeight;
    	myBaseLine += myTextMetrics.tmAscent;
    	
    	// If we're using line numbers, calculate the margins
    	if (stProperties.lineNumbers)
    	{
    	    GetTextExtentPoint32 (pmPrinterDeviceContext, "00000000", 8, 
    	        &myExtent);
    	    myTextBottomMargin = mySubPageBottom - myExtent.cx;
    	    myTextWidth = mySubPageBottom - mySubPageTop - myExtent.cx;
    	    GetTextExtentPoint32 (pmPrinterDeviceContext, "00000", 5, 
    	        &myExtent);
    	    myLineNumberTopMargin = mySubPageBottom - myExtent.cx;
        }
    	else
    	{
    	    myTextBottomMargin = mySubPageBottom;
    	    myTextWidth = mySubPageBottom - mySubPageTop;
        }
 
    	SelectFont (pmPrinterDeviceContext, pmFont);
    
    	myCurrentPos = *pmCurrentPos;
    
        //
        // Main Loop: Loop once through this for every printed line (not
        // program line). myCurrentPos is the place to start printing from		     
    	//
    	while (myCurrentPos < pmLastPos)
    	{
    	    // Draw the line number, if applicable
    	    if (pmPrinting && stProperties.lineNumbers && *pmStartOfLine)
    	    {
    	    	char	myLineNumberString [128];
    	    
    	    	wsprintf (myLineNumberString, "%d", *pmLineNumber);
    	    	SetTextAlign (pmPrinterDeviceContext, TA_RIGHT);
    	    	TextOut (pmPrinterDeviceContext, myBaseLine, 
    	    	    myLineNumberTopMargin, 
    	            myLineNumberString, strlen (myLineNumberString));
    	    	SetTextAlign (pmPrinterDeviceContext, TA_LEFT);
	    }
	
	    myRemainingTextWidth = myTextWidth;
	    myCurrentBottomMargin = myTextBottomMargin;

	    // Set myEndOfLinePos to the end of line (or last character)
	    myEndOfLinePos = myCurrentPos;
	    while ((myEndOfLinePos < pmLastPos) && 
	    	(pmTextPtr -> text [myEndOfLinePos] != '\r') &&
	    	(pmTextPtr -> text [myEndOfLinePos] != '\n'))
	    {
	    	myEndOfLinePos++;
	    }
	
	    // Calculate the leading space.  Initial spaces have the size of 
	    // a 'm'.  We don't just print spaces because of non-proportional
	    // printing fonts
	    if (*pmStartOfLine)
	    {
	        myPos = myCurrentPos;
	        while ((pmTextPtr -> text [myPos] == ' ') && 
	            (myPos < myEndOfLinePos))
	    	{
	    	    myPos++;
	    	}
	    	GetTextExtentPoint32 (pmPrinterDeviceContext, "M", 1, 
	    	    &myExtent);

	        *pmStartSpace = (myPos - myCurrentPos) * myExtent.cx;
	    	myCurrentPos = myPos;
	    }

	    // We set pmStartOfLine to false for every escape in
	    // the middle of a line
	    *pmStartOfLine = TRUE;
	
	    // If the indentation won't fit on one line, skip to the next line
	    if (myRemainingTextWidth <= *pmStartSpace)
	    {
	    	*pmStartSpace -= myRemainingTextWidth;
	    	*pmStartOfLine = FALSE;
	    	break;
	    }
	    else
	    {
	    	// Otherwise, indent appropriately
	    	myRemainingTextWidth -= *pmStartSpace;
	    	myCurrentBottomMargin -= *pmStartSpace;
	        *pmStartSpace = 0;
	    }

	    //
	    // Now loop, printing non-ident/non-keyword, then ident/keyword
	    // until the end of the print line
	    //
	    while (myCurrentPos < myEndOfLinePos)
	    {
	        //
	    	// Print all non-identifier/non-keywords one char at a time
	    	//
	    	myPos = myCurrentPos;
	    	myEndOfLineReached = FALSE;
		
	    	// Move myPos until reaching an identifier, a keyword, the end 
	    	// of the line or until no more will fit on the line.
  	    	while ((myPos < myEndOfLinePos) && 
	            (pmTextPtr -> textType [myPos] != TEXT_IDENTIFIER) &&
	            (pmTextPtr -> textType [myPos] != TEXT_KEYWORD))
	    	{
	            GetTextExtentPoint32 (pmPrinterDeviceContext, 
	            	&pmTextPtr -> text [myCurrentPos], myPos - myCurrentPos, 
	            	&myExtent);
	            if (myExtent.cx > myRemainingTextWidth)
	            {
	    	    	myEndOfLineReached = TRUE;
	    	    	*pmStartOfLine = FALSE;
	            	break;
	            }
	            myPos++;
	    	}
	
	    	// If there are characters to be printed, print them.
	    	if (myPos > myCurrentPos)
	    	{
    	            SelectFont (pmPrinterDeviceContext, pmFont);
    	            if (pmPrinting)
    	            {
	 		SetTextAlign (pmPrinterDeviceContext, TA_LEFT);
       	            	TextOut (pmPrinterDeviceContext, myBaseLine, 
    	                    myCurrentBottomMargin, 
    	                    &pmTextPtr -> text [myCurrentPos], 
    	                    myPos - myCurrentPos);
    	            }
	            GetTextExtentPoint32 (pmPrinterDeviceContext, 
	            	&pmTextPtr -> text [myCurrentPos], myPos - myCurrentPos, 
	            	&myExtent);
	            myCurrentBottomMargin -= myExtent.cx;
	            myRemainingTextWidth -= myExtent.cx;
	            myCurrentPos = myPos;
	    	}
	
	    	// If we've reached the end of the text line or the 
	    	// printer line, break
	    	if ((myCurrentPos >= myEndOfLinePos) || myEndOfLineReached)
	    	{
	            break;
	    	}
	    
	        //
	        // Print the identifier/keyword, if it fits on the line
	        //
	        myPos = myCurrentPos + 1;
	
	    	// Move myPos until reaching the end of the 
	    	// identifier or keyword
	        while ((myCurrentPos < myEndOfLinePos) && 
	               (pmTextPtr -> textType [myPos] == 
	                pmTextPtr -> textType [myCurrentPos]))
	    	{
	            myPos++;
	    	}

	    	// Make certain the identifier/keyword will fit on the line
	    	if (pmTextPtr -> textType [myCurrentPos] == TEXT_IDENTIFIER)
	    	{
	            SelectFont (pmPrinterDeviceContext, pmItalicFont);
	    	}
	    	else
	    	{
	            SelectFont (pmPrinterDeviceContext, pmBoldFont);
	    	}
	    	GetTextExtentPoint32 (pmPrinterDeviceContext, 
	            &pmTextPtr -> text [myCurrentPos], myPos - myCurrentPos, 
	            &myExtent);
	    	if (myExtent.cx > myRemainingTextWidth)
	    	{
	            SelectFont (pmPrinterDeviceContext, pmFont);
	            *pmStartOfLine = FALSE;
	            break;
	    	}

	    	if (myPos > myCurrentPos)
	    	{
    	            if (pmPrinting)
    	            {
		    	SetTextAlign (pmPrinterDeviceContext, TA_LEFT);
    	            	TextOut (pmPrinterDeviceContext, myBaseLine,
    	                    myCurrentBottomMargin, 
    	                    &pmTextPtr -> text [myCurrentPos], 
    	                    myPos - myCurrentPos);
    	            }
	            myCurrentBottomMargin -= myExtent.cx;
	            myRemainingTextWidth -= myExtent.cx;
	            myCurrentPos = myPos;
	            SelectFont (pmPrinterDeviceContext, pmFont);
	    	}
	    } // while (myCurrentPos < myEndOfLinePos)
	
    	    //
    	    // We've reached the end of a print line
    	    //
    	    if (*pmStartOfLine)
    	    {
    	    	*pmLineNumber += 1;
    	    	// Assert that pmTextPtr -> text [myCurrentPos] == '\r' or '\n'
	    	if ((pmTextPtr -> text [myCurrentPos] == '\r') &&
	            (pmTextPtr -> text [myCurrentPos + 1] == '\n'))
	    	{
	    	    myCurrentPos++;
	    	}
    	    	myCurrentPos++;
    	    }
    	
	    myBaseLine += myLineHeight;
    	    if (myBaseLine + myLineHeight > myRealRightMargin)
    	    {
    	        break;
    	    }
        } // while (myCurrentPos < pmLastPos)
    
        *pmCurrentPos = myCurrentPos;
        
        if (myCurrentPos >= pmLastPos)
        {
            break;
        }
    } // for subpage
} // MyPrintTwoUpProgramPage


/************************************************************************/
/* MySetCheckBox							*/
/************************************************************************/
static void	MySetCheckBox (HWND pmDialog, int pmCheckBox, BOOL pmValue)
{
    if (pmValue)
    {
        CheckDlgButton (pmDialog, pmCheckBox, BST_CHECKED);
    }
    else
    {
        CheckDlgButton (pmDialog, pmCheckBox, BST_UNCHECKED);
    }
} // MySetCheckBox

