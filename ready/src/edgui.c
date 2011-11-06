/***********/
/* edgui.c */
/***********/

/*******************/
/* System includes */
/*******************/
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <zmouse.h>
#include <crtdbg.h>
#include <string.h>

/****************/
/* Self include */
/****************/
#include "edgui.h"

/******************/
/* Other includes */
/******************/
#ifdef CONSOLE
    #include "jsmain.h"

    #include "jsdialogs.h"
#else
    #include "ed.h"
    #include "edfail.h"
    #include "edglob.h"
    #include "edprog.h"
    #include "edprop.h"

    #include "eddialogs.h"
#endif // #ifndef CONSOLE

#include "edfile.h"

/**********/
/* Macros */
/**********/
#ifdef CONSOLE
    #define EdFail_Fatal	JS_ErrorMessage
#else
    #define MoveTo(pmDeviceContext, x, y)  MoveToEx(pmDeviceContext, x, y, NULL)
#endif // #ifndef CONSOLE
#define MIN(x,y)		((x) < (y) ? (x) : (y))
#define MAX(x,y)		((x) > (y) ? (x) : (y))

/*************/
/* Constants */
/*************/
#define SAVE_FILE		TRUE
#define OPEN_FILE		FALSE

#define ENV_NAME_STRING		"[ENVIRONMENT]"

// This is defined in winuser.h only if _WIN32_WINNT >= 0x400 or 
// _WIN32_WINDOWS > 0x400.  Thus we define it here ourself.
// #define SPI_GETWHEELSCROLLLINES	104

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
    static char		stFileNameFilter[] = "All files (*.*)\0*.*\0\0\0";
#if !defined (CONSOLE) && !defined (TPROLOG)
    static char		stJavaFileNameFilter[] = "All files (*.*)\0*.*\0"
	"Java files (.java)\0*.java\0"
	"Java/HTML files (.java;.html)\0*.java;*.html\0"
	"\0\0";
    static char		stTuringFileNameFilter[] = "All files (*.*)\0*.*\0"
	"Turing files (.t;.ti;.tu;.tur;.dem)\0*.t;*.ti;*.tu;*.tur;*.dem\0"
	"\0\0";
    static char		stRTFFileNameFilter[] = "All files (*.*)\0*.*\0"
	"RTF files (.rtf)\0*.rtf\0"
	"\0\0";
    static char		stJavaRTFFileNameFilter[] = "All files (*.*)\0*.*\0"
	"Java/RTF files (.java;.rtf)\0*.java;*.rtf\0"
	"RTF files (.rtf)\0*.rtf\0"
	"\0\0";
    static char		stTuringRTFFileNameFilter[] = "All files (*.*)\0*.*\0"
	"Turing/RTF files (.t;.ti;.tu;.tur;.dem;.rtf)\0*.t;*.ti;*.tu;*.tur;*.dem;*.rtf\0"
	"RTF files (.rtf)\0*.rtf\0"
	"\0\0";
    static char		stDLLNameFilter[] = 
	"Executable files (.dll)\0*.dll\0"
	"All files (*.*)\0*.*\0"
	"\0\0";
    static char		stClassLibFilter[] = 
	"Java Libraries (.zip;.jar)\0*.zip;*.jar\0"
	"All files (*.*)\0*.*\0"
	"\0\0";
    static char		stJavaEXEFileNameFilter[] = "All files (*.*)\0*.*\0"
	"Java/EXE files (.java;.rtf)\0*.java;*.exe\0"
	"EXE files (.exe)\0*.exe\0"
	"\0\0";
    static char		stTuringEXEFileNameFilter[] = "All files (*.*)\0*.*\0"
	"Turing/EXE files (.t;.ti;.tu;.tur;.dem;.exe)\0*.t;*.ti;*.tu;*.tur;*.dem;*.exe\0"
	"EXE files (.exe)\0*.exe\0"
	"\0\0";
    static char		stEXEFileNameFilter[] = "All files (*.*)\0*.*\0"
	"EXE files (.exe)\0*.exe\0"
	"\0\0";
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)

/********************/
/* Static variables */
/********************/
// Hook for MessageBox
static HHOOK		stMessageBoxHook;

// MSH_WHEELMOUSE message
static UINT		stMouseWheelMessage;

// Structure size of OPENFILENAME (larger in Win2K or higher)
static UINT		stOPENFILENAMESize;

// Variables used for the timer
static LARGE_INTEGER	stCounterFrequency;
static LARGE_INTEGER	stStartCount;
static BOOL		stUseTicks;

/******************************/
/* Static callback procedures */
/******************************/
static BOOL CALLBACK	MyFileDialogProcedure (HWND pmDialog, 
    					       UINT pmMessage, WPARAM pmWParam, 
    					       LPARAM pmLParam);
static LRESULT CALLBACK	MyMessageBoxCenteringProc (int pmHookCode, 
						   WPARAM pmWParam,
						   LPARAM pmLParam);
/*********************/
/* Static procedures */
/*********************/

/***********************/
/* External procedures */
/***********************/
/************************************************************************/
/* EdGUI_Init								*/
/************************************************************************/
BOOL	EdGUI_Init (void)
{
    OSVERSIONINFO	myOSVersionInfo;
    static BOOL		myStEdGUIInitialized = FALSE;
    
    if (myStEdGUIInitialized)
    {
    	return TRUE;
    }
    
    ZeroMemory (&myOSVersionInfo, sizeof (myOSVersionInfo));
    myOSVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx (&myOSVersionInfo);

    // Register the mouse wheel message if applicable
    stMouseWheelMessage = 0;
    if (((myOSVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) &&
	 (myOSVersionInfo.dwMajorVersion == 4) &&
	 (myOSVersionInfo.dwMinorVersion == 0)) ||
        ((myOSVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT) && 
          (myOSVersionInfo.dwMajorVersion < 4)))
    {
    	stMouseWheelMessage = RegisterWindowMessage (MSH_MOUSEWHEEL);
    }
    
    stOPENFILENAMESize = sizeof (OPENFILENAME);
    
    if (QueryPerformanceCounter (&stStartCount)) 
    {
    	stUseTicks = FALSE;

	QueryPerformanceFrequency (&stCounterFrequency);
    }
    else
    {
    	stUseTicks = TRUE;
    }

    myStEdGUIInitialized = TRUE;
    
    return TRUE;
} // EdGUI_Init


#ifndef TPROLOG
#ifndef CONSOLE
/************************************************************************/
/* EdGUI_Finalize							*/
/************************************************************************/
void	EdGUI_Finalize (void)
{
} // EdGUI_Finalize


/************************************************************************/
/* EdGUI_PropertiesSet							*/
/************************************************************************/
void	EdGUI_PropertiesSet (void)
{
    EdGUI_PropertiesImplementChanges ();
} // EdFile_PropertiesSet

	
/************************************************************************/
/* EdGUI_PropertiesImplementChanges					*/
/************************************************************************/
void	EdGUI_PropertiesImplementChanges (void)
{
    // Do nothing, for the moment
} // EdGUI_PropertiesImplementChanges
#endif // #ifndef CONSOLE
#endif // #ifndef TPROLOG

/************************************************************************/
/* EdGUI_CentreDialogBox						*/
/************************************************************************/
void	EdGUI_CentreDialogBox (HWND pmDialog)
{
    RECT	myParentRect, myDialogRect, myScreenRect;
    int		myParentWidth, myParentHeight, myDialogWidth, myDialogHeight;
    int		myX, myY;

    myScreenRect = EdGUI_GetWorkArea ();

    if (GetParent (pmDialog) == NULL)
    {
    	myParentRect = EdGUI_GetWorkArea ();
    }
    else
    {
    	GetWindowRect (GetParent (pmDialog), &myParentRect);
    }
    GetWindowRect (pmDialog, &myDialogRect);
    
    myParentWidth = myParentRect.right - myParentRect.left;
    myParentHeight = myParentRect.bottom - myParentRect.top;
    myDialogWidth = myDialogRect.right - myDialogRect.left;
    myDialogHeight = myDialogRect.bottom - myDialogRect.top;

    myX = myParentRect.left + (myParentWidth - myDialogWidth) / 2;
    myX = MAX (myScreenRect.left, MIN (myScreenRect.right - myDialogWidth, myX));
    myY = myParentRect.top + (myParentHeight - myDialogHeight) / 3;
    myY = MAX (myScreenRect.top, MIN (myScreenRect.bottom - myDialogHeight, myY));
    SetWindowPos (pmDialog, HWND_TOP, myX, myY, 0, 0, 
		  SWP_NOSIZE | SWP_NOACTIVATE);
} // EdGUI_CentreDialogBox


/*******************************************************/
/* Create the font specified by the font name and size */
/*******************************************************/
HFONT	EdGUI_CreateFont (HWND pmWindow, HDC pmDisplayContext, 
			  const char *pmFontName, int pmFontSize, 
			  BOOL pmBold, BOOL pmItalic)
{
    return EdGUI_CreateFontAngle (pmWindow, pmDisplayContext, pmFontName, 
    	       pmFontSize, pmBold, pmItalic, 0);
} // EdGUI_CreateFont


/*******************************************************/
/* Create the font specified by the font name and size */
/*******************************************************/
HFONT	EdGUI_CreateFontAngle (HWND pmWindow, HDC pmDisplayContext, 
			       const char *pmFontName, int pmFontSize, 
			       BOOL pmBold, BOOL pmItalic, int pmAngle)
{
    LOGFONT	myLogFont;
    HDC		myDisplayContext;
    HFONT	myFont;
       
    ZeroMemory (&myLogFont, sizeof (LOGFONT));
    if (pmDisplayContext != NULL)
    {
    	myDisplayContext = pmDisplayContext;
    }
    else
    {
    	myDisplayContext = GetDC (pmWindow);
    }
    if (pmFontSize > 0)
    {
        myLogFont.lfHeight = -MulDiv (pmFontSize, 
    	    GetDeviceCaps (myDisplayContext, LOGPIXELSY), 72);
    }
    else
    {
        myLogFont.lfHeight = pmFontSize;
    }
    
    // If we created the DC, then release it
    if (pmDisplayContext == NULL)
    {
    	ReleaseDC (pmWindow, myDisplayContext);
    }
    
    if (pmBold)
    {
        myLogFont.lfWeight = 700;
    }
    myLogFont.lfItalic = pmItalic;
    myLogFont.lfEscapement = myLogFont.lfOrientation = pmAngle;

    strcpy (myLogFont.lfFaceName, pmFontName);
    
    myFont = CreateFontIndirect (&myLogFont);
    
    return myFont;
} // EdGUI_CreateFontAngle


#ifndef TPROLOG
#ifndef CONSOLE
/************************************************************************/
/* EdGUI_CtrlShiftKeysPressed						*/
/************************************************************************/
BOOL	EdGUI_CtrlShiftKeysPressed (void)
{
    BYTE    myKeyboardByte [300];
    
    if (GetKeyboardState (myKeyboardByte))
    {
        if ((myKeyboardByte [VK_SHIFT] & 0x80) &&
            (myKeyboardByte [VK_CONTROL] & 0x80))
        {
            return TRUE;
        }
    }
    
    return FALSE;
} // EdGUI_CtrlShiftKeysPressed


/************************/
/* EdGUI_DlgLettersOnly */
/************************/
void	EdGUI_DlgLettersOnly (HWND pmDialog, int pmDialogPart,
			      BOOL pmStarAllowed)
{
    HWND	myItem;
    char	myString [1024];
    UINT	cnt, cnt1, myFrom, myTo;
    	    	    	
    myItem = GetDlgItem (pmDialog, pmDialogPart);

    GetWindowText (myItem, myString, sizeof (myString));
    SendMessage (myItem, EM_GETSEL, (WPARAM) &myFrom, (LPARAM) &myTo);
    	    	    	    
    for (cnt = 0 ; cnt < strlen (myString) ; cnt++)
    {
    	if (('a' <= myString [cnt]) && (myString [cnt] <= 'z'))
    	{
    	    myString [cnt] -= ('a' - 'A');
    	}
    	else if (('A' <= myString [cnt]) && (myString [cnt] <= 'Z'))
    	{
    	    continue;
    	}
    	else if (pmStarAllowed && (myString [cnt] == '*')) 
    	{
    	    continue;
    	}
    	else
    	{
    	    for (cnt1 = cnt + 1 ; cnt1 <= strlen (myString) ; cnt1++)
    	    	myString [cnt1 - 1] = myString [cnt1];
    	    if (myFrom > cnt)
    	    	myFrom--;
    	    if (myTo > cnt)
    	    	myTo--;
    	    SetWindowText (myItem, myString);
    	    SendMessage (myItem, EM_SETSEL, (WPARAM) myFrom, (LPARAM) myTo);
    	    MessageBeep (MB_ICONEXCLAMATION);
    	}
   }
} // EdGUI_DlgLettersOnly


/************************/
/* EdGUI_DlgNumbersOnly */
/************************/
void	EdGUI_DlgNumbersOnly (HWND pmDialog, int pmDialogPart)
{
    HWND	myItem;
    char	myNumber [128];
    UINT	cnt, cnt1, myFrom, myTo;
    	    	    	
    myItem = GetDlgItem (pmDialog, pmDialogPart);

    GetWindowText (myItem, myNumber, sizeof (myNumber));
    SendMessage (myItem, EM_GETSEL, (WPARAM) &myFrom, (LPARAM) &myTo);
    	    	    	    
    for (cnt = 0 ; cnt < strlen (myNumber) ; cnt++)
    {
    	if ((myNumber [cnt] < '0') || ('9' < myNumber [cnt]))
    	{
    	    for (cnt1 = cnt + 1 ; cnt1 <= strlen (myNumber) ; cnt1++)
    	    	myNumber [cnt1 - 1] = myNumber [cnt1];
    	    if (myFrom > cnt)
    	    	myFrom--;
    	    if (myTo > cnt)
    	    	myTo--;
    	    SetWindowText (myItem, myNumber);
    	    SendMessage (myItem, EM_SETSEL, (WPARAM) myFrom, (LPARAM) myTo);
    	    MessageBeep (MB_ICONEXCLAMATION);
    	}
   }
} // EdGUI_DlgNumbersOnly


/**********************/
/* EdGUI_GetClassLibs */
/**********************/
BOOL	EdGUI_GetClassLibs (HWND pmWindow, char *pmPathName, char *pmTitle)
{
    OPENFILENAME	myOpenFileName;
    BOOL		myOpenFileDialogResult;

    pmPathName [0] = 0;
    
    ZeroMemory (&myOpenFileName, sizeof (OPENFILENAME));

    myOpenFileName.lStructSize = stOPENFILENAMESize;
    myOpenFileName.hwndOwner   = pmWindow;
    myOpenFileName.lpstrFile   = pmPathName;
    myOpenFileName.nMaxFile    = PATHNAME_LENGTH;
    myOpenFileName.lCustData   = OPEN_FILE;
    myOpenFileName.lpstrTitle  = pmTitle;
    myOpenFileName.lpfnHook = MyFileDialogProcedure;
    myOpenFileName.Flags = OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | 
        OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_ENABLEHOOK | 
        OFN_ALLOWMULTISELECT | OFN_ENABLESIZING;

    myOpenFileName.lpstrFilter  = stClassLibFilter;
    myOpenFileName.nFilterIndex = 1;

    Ed_EnableAllWindows (FALSE, pmWindow);
    myOpenFileDialogResult = GetOpenFileName (&myOpenFileName);
    Ed_EnableAllWindows (TRUE, pmWindow);

    if (!myOpenFileDialogResult) 
    {
	DWORD	myErrorCode = CommDlgExtendedError();
	if (myErrorCode)
	{
     	    EdGUI_Message (
    	    	"GetOpenFileName Error",
    	    	"An error occurred while attempting to get the file name."
    	    	"\n\n[Error code: %d]", myErrorCode);
    	}

	return FALSE;
    }
    
    return TRUE;
} // EdGUI_GetClassLibs


/************************************************************************/
/* EdGUI_GetColourDepth							*/
/************************************************************************/
int	EdGUI_GetColourDepth (void)
{
    HDC		myDisplayContext;
    int		myColourDepth;
    
    myDisplayContext = GetDC (NULL);
    myColourDepth = GetDeviceCaps (myDisplayContext, BITSPIXEL);
    ReleaseDC (NULL, myDisplayContext);
    
    return myColourDepth;
} // EdGUI_GetColourDepth


/************************************************************************/
/* EdGUI_GetCommonControlVersionNumber					*/
/************************************************************************/
int	EdGUI_GetCommonControlVersionNumber (void)
{
    HMODULE		myCommonControlModule;
    FilePath		myCommonControlName;
    DWORD		myDummy;
    DWORD		myVersionSize;
    char		*myVersionData;
    VS_FIXEDFILEINFO	*myFixedVersionInfo;
    UINT		myFixedVersionInfoSize;
    int			myVersionNumber;
        
    myCommonControlModule = GetModuleHandle ("COMCTL32");

    if (GetModuleFileName (myCommonControlModule, myCommonControlName, 
    			   PATHNAME_LENGTH) == 0)
    {
    	return 0;
    }
    
    myVersionSize = GetFileVersionInfoSize (myCommonControlName, &myDummy);
    if (myVersionSize == 0)
    {
    	return 0;
    }

    myVersionData = malloc (myVersionSize);
    if (myVersionData == NULL)
    {
    	return 0;
    }
    
    if (!GetFileVersionInfo (myCommonControlName, myDummy, myVersionSize, 
        myVersionData))
    {
    	free (myVersionData);
    	return 0;
    }
    	
    if (!VerQueryValue (myVersionData, "\\", &myFixedVersionInfo, 
    			&myFixedVersionInfoSize))
    {
    	free (myVersionData);
    	return 0;
    }
    
    myVersionNumber = 100 * HIWORD (myFixedVersionInfo -> dwFileVersionMS) +
    	LOWORD (myFixedVersionInfo -> dwFileVersionMS);

    free (myVersionData);
    return myVersionNumber;
} // EdGUI_GetCommonControlVersionNumber


/************************************************************************/
/* EdGUI_GetCurrentDate							*/
/************************************************************************/
char	*EdGUI_GetCurrentDate (char *pmDateBuffer)
{
    SYSTEMTIME	mySystemTime;
    char	myDateBuffer [256], myTimeBuffer [256];
        
    GetLocalTime (&mySystemTime);
    
    pmDateBuffer [0] = 0;
    
    if (GetDateFormat (0, 0, &mySystemTime, NULL, myDateBuffer,
    	sizeof (myDateBuffer)) == 0)
    {
    	EdFail_Warn (IDS_GETDATEFORMATFAIL, __FILE__, __LINE__, 
    		     GetLastError ());
    }
    else
    { 
        if (GetTimeFormat (0, 0, &mySystemTime, NULL, myTimeBuffer, 
             sizeof (myTimeBuffer)) == 0)
        {
    	    EdFail_Warn (IDS_GETTIMEFORMATFAIL, __FILE__, __LINE__, 
    		         GetLastError ());
        }
        else
        {
	    wsprintf (pmDateBuffer, "%s %s", myDateBuffer, myTimeBuffer);
	}
    }
    return pmDateBuffer;
} // EdGUI_GetCurrentDate
#endif // #ifndef CONSOLE
#endif // #ifndef TPROLOG

/************************************************************************/
/* EdGUI_GetMouseWheelMessage						*/
/************************************************************************/
int	EdGUI_GetMouseWheelMessage (void)
{
    return stMouseWheelMessage;
} // EdGUI_GetMouseWheelMessage


/************************************************************************/
/* EdGUI_GetMouseWheelSettings						*/
/************************************************************************/
int	EdGUI_GetMouseWheelSettings (void)
{
    OSVERSIONINFO	myOSVersionInfo;
    int			myScrollLines;
    
    ZeroMemory (&myOSVersionInfo, sizeof (myOSVersionInfo));
    myOSVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx (&myOSVersionInfo);

    // In Windows 9x & WinNT3.51 query Mswheel for the
    // number of scroll lines and in WinNT 4.0 and later,
    // use SystemParametersInfo API

    if (((myOSVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) &&
	 (myOSVersionInfo.dwMajorVersion == 4) &&
	 (myOSVersionInfo.dwMinorVersion == 0)) ||
        ((myOSVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT) && 
          (myOSVersionInfo.dwMajorVersion < 4)))
    {
   	HWND	myWheel;
   	
        myWheel = FindWindow(MSH_WHEELMODULE_CLASS, MSH_WHEELMODULE_TITLE);
        if (myWheel)
        {
           UINT myScrollLinesMessage = RegisterWindowMessage (MSH_SCROLL_LINES);
           if (myScrollLinesMessage)
           {
                myScrollLines = (int) SendMessage (myWheel, 
                			myScrollLinesMessage, 0, 0);
            }
        }
    }         
    else 
    {
	SystemParametersInfo (SPI_GETWHEELSCROLLLINES, 0, &myScrollLines, 0);
    }
    
    if (myScrollLines != 0)
    {
    	return WHEEL_DELTA / myScrollLines;
    }
    else
    {
        return 0;
    }
} // EdGUI_GetMouseWheelSettings


#if !defined (CONSOLE) && !defined (TPROLOG)
/*********************/
/* EdGUI_GetOpenFile */
/*********************/
BOOL	EdGUI_GetOpenFile (HWND pmWindow, char *pmPathName)
{
    OPENFILENAME	myOpenFileName;
    FilePath		myActiveDirectory;
    BOOL		myOpenFileDialogResult;
    static int		myFilterNumber = 2;
    
    pmPathName [0] = 0;
    
    EdFile_GetActiveDirectory (myActiveDirectory);
    
    ZeroMemory (&myOpenFileName, sizeof (OPENFILENAME));

    myOpenFileName.lStructSize = stOPENFILENAMESize;
    myOpenFileName.hwndOwner   = pmWindow;
    myOpenFileName.lpstrFile   = pmPathName;
    myOpenFileName.nMaxFile    = PATHNAME_LENGTH;
    myOpenFileName.lCustData   = OPEN_FILE;
    myOpenFileName.lpstrTitle  = "Open for Editing";
    if (strlen (myActiveDirectory) > 0)
    {
    	myOpenFileName.lpstrInitialDir = myActiveDirectory;
    }
    myOpenFileName.lpfnHook = MyFileDialogProcedure;
    myOpenFileName.Flags = OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | 
        OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_ENABLEHOOK | OFN_ENABLESIZING;

    if (gProgram.isJava)
    {
        myOpenFileName.lpstrFilter  = stJavaFileNameFilter;
    }
    else if (gProgram.isTuring)
    {
        myOpenFileName.lpstrFilter  = stTuringFileNameFilter;
    }
    else
    {
    	myOpenFileName.lpstrFilter  = stFileNameFilter;
    }
    myOpenFileName.nFilterIndex = myFilterNumber;

    Ed_EnableAllWindows (FALSE, pmWindow);
    myOpenFileDialogResult = GetOpenFileName (&myOpenFileName);
    Ed_EnableAllWindows (TRUE, pmWindow);

    if (!myOpenFileDialogResult) 
    {
	DWORD	myErrorCode = CommDlgExtendedError();
	if (myErrorCode)
	{
     	    EdGUI_Message (
    	    	"GetOpenFileName Error",
    	    	"An error occurred while attempting to get the file name."
    	    	"\n\n[Error code: %d]", myErrorCode);
    	}

    	myFilterNumber = myOpenFileName.nFilterIndex;
	return FALSE;
    }
    
    myFilterNumber = myOpenFileName.nFilterIndex;

    // Remove any trailing period
    if (pmPathName [strlen (pmPathName) - 1] == '.')
    {
    	pmPathName [strlen (pmPathName) - 1] = 0;
    }
    
    return TRUE;
} // EdGUI_GetOpenFile


/*********************/
/* EdGUI_GetDLLName */
/*********************/
BOOL	EdGUI_GetDLLName (HWND pmWindow, char *pmPathName, char *pmTitle)
{
    OPENFILENAME	myOpenFileName;
    BOOL		myOpenFileDialogResult;
    
//    pmPathName [0] = 0;
    
    ZeroMemory (&myOpenFileName, sizeof (OPENFILENAME));

    myOpenFileName.lStructSize = stOPENFILENAMESize;
    myOpenFileName.hwndOwner   = pmWindow;
    myOpenFileName.lpstrFile   = pmPathName;
    myOpenFileName.nMaxFile    = PATHNAME_LENGTH;
    myOpenFileName.lCustData   = OPEN_FILE;
    myOpenFileName.lpstrTitle  = pmTitle;
    myOpenFileName.lpfnHook = MyFileDialogProcedure;
    myOpenFileName.Flags = OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | 
        OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_ENABLEHOOK | OFN_ENABLESIZING;

    myOpenFileName.lpstrFilter  = stDLLNameFilter;
    myOpenFileName.nFilterIndex = 1;

    Ed_EnableAllWindows (FALSE, pmWindow);
    myOpenFileDialogResult = GetOpenFileName (&myOpenFileName);
    Ed_EnableAllWindows (TRUE, pmWindow);

    if (!myOpenFileDialogResult) 
    {
	DWORD	myErrorCode = CommDlgExtendedError();
	if (myErrorCode)
	{
     	    EdGUI_Message (
    	    	"GetOpenFileName Error",
    	    	"An error occurred while attempting to get the file name."
    	    	"\n\n[Error code: %d]", myErrorCode);
    	}

	return FALSE;
    }
    
    return TRUE;
} // EdGUI_GetDLLName
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)


#ifndef CONSOLE
/************************************************************************/
/* EdGUI_GetRedirectInputFile						*/
/************************************************************************/
BOOL	EdGUI_GetRedirectInputFile (HWND pmWindow, char *pmPathName)
{
    OPENFILENAME	myOpenFileName;
    BOOL		myOpenFileDialogResult;
    
    ZeroMemory (&myOpenFileName, sizeof (OPENFILENAME));

    myOpenFileName.lStructSize = stOPENFILENAMESize;
    myOpenFileName.hwndOwner   = pmWindow;
    myOpenFileName.lpstrFile   = pmPathName;
    myOpenFileName.nMaxFile    = PATHNAME_LENGTH;
    myOpenFileName.lCustData   = OPEN_FILE;
    myOpenFileName.lpstrTitle  = "Select Input File";
    myOpenFileName.lpfnHook = MyFileDialogProcedure;
    myOpenFileName.Flags = OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | 
        OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_ENABLEHOOK | OFN_ENABLESIZING;

    myOpenFileName.lpstrFilter  = "All files (*.*)\0*.*\0\0\0";
    myOpenFileName.nFilterIndex = 1;

#ifndef TPROLOG
    Ed_EnableAllWindows (FALSE, pmWindow);
#endif // #ifndef TPROLOG
    myOpenFileDialogResult = GetOpenFileName (&myOpenFileName);
#ifndef TPROLOG
    Ed_EnableAllWindows (TRUE, pmWindow);
#endif // #ifndef TPROLOG

    if (!myOpenFileDialogResult) 
    {
	DWORD	myErrorCode = CommDlgExtendedError();
	if (myErrorCode)
	{
     	    EdGUI_Message (
    	    	"GetOpenFileName Error",
    	    	"An error occurred while attempting to get the file name."
    	    	"\n\n[Error code: %d]", myErrorCode);
    	}

	return FALSE;
    }
    
    return TRUE;
} // EdGUI_GetRedirectInputFile


/************************************************************************/
/* EdGUI_GetRedirectOutputFile						*/
/************************************************************************/
BOOL	EdGUI_GetRedirectOutputFile (HWND pmWindow, char *pmPathName)
{
    OPENFILENAME	myOpenFileName;
    BOOL		mySaveFileDialogResult;
    
    ZeroMemory (&myOpenFileName, sizeof (OPENFILENAME));

    myOpenFileName.lStructSize = stOPENFILENAMESize;
    myOpenFileName.hwndOwner   = pmWindow;
    myOpenFileName.lpstrFile   = pmPathName;
    myOpenFileName.nMaxFile    = PATHNAME_LENGTH;
    myOpenFileName.lCustData   = SAVE_FILE;
    myOpenFileName.lpstrTitle  = "Select Output File";
    myOpenFileName.lpfnHook = MyFileDialogProcedure;
    myOpenFileName.Flags = OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | 
        OFN_EXPLORER | OFN_ENABLEHOOK | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING;

    myOpenFileName.lpstrFilter  = "All files (*.*)\0*.*\0\0\0";
    myOpenFileName.nFilterIndex = 1;

#ifndef TPROLOG
    Ed_EnableAllWindows (FALSE, pmWindow);
#endif // #ifndef TPROLOG
    mySaveFileDialogResult = GetSaveFileName (&myOpenFileName);
#ifndef TPROLOG
    Ed_EnableAllWindows (TRUE, pmWindow);
#endif // #ifndef TPROLOG

    if (!mySaveFileDialogResult) 
    {
	DWORD	myErrorCode = CommDlgExtendedError();
	if (myErrorCode)
	{
     	    EdGUI_Message1 (pmWindow, MB_ICONEXCLAMATION, 
     	        IDS_FILE_DIALOG_ERROR_TITLE, IDS_FILE_DIALOG_ERROR_MESSAGE,
    	    	myErrorCode);
    	}

	return FALSE;
    }
    
    return TRUE;
} // EdGUI_GetRedirectOutputFile
#endif // #ifndef CONSOLE


#if !defined (CONSOLE) && !defined (TPROLOG)
/************************/
/* EdGUI_GetSaveEXEFile */
/************************/
BOOL	EdGUI_GetSaveEXEFile (HWND pmWindow, char *pmPathName)
{
    OPENFILENAME	myOpenFileName;
    FilePath		myActiveDirectory;
    BOOL		mySaveFileDialogResult;
    static int		myFilterNumber = 1;
    
    EdFile_GetActiveDirectory (myActiveDirectory);
    
    ZeroMemory (&myOpenFileName, sizeof (OPENFILENAME));

    myOpenFileName.lStructSize = stOPENFILENAMESize;
    myOpenFileName.hwndOwner   = pmWindow;
    myOpenFileName.lpstrFile   = pmPathName;
    if (strlen (myActiveDirectory) > 0)
    {
    	myOpenFileName.lpstrInitialDir = myActiveDirectory;
    }
    myOpenFileName.nMaxFile    = PATHNAME_LENGTH;
    myOpenFileName.lCustData   = SAVE_FILE;
    myOpenFileName.lpstrTitle  = "Save Stand-alone Executable";
    myOpenFileName.lpfnHook = MyFileDialogProcedure;
    myOpenFileName.Flags = OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | 
        OFN_EXPLORER | OFN_ENABLEHOOK | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING;

    if (gProgram.isJava)
    {
        myOpenFileName.lpstrFilter  = stJavaEXEFileNameFilter;
    }
    else if (gProgram.isTuring)
    {
        myOpenFileName.lpstrFilter  = stTuringEXEFileNameFilter;
    }
    else
    {
    	myOpenFileName.lpstrFilter  = stEXEFileNameFilter;
    }
    myOpenFileName.nFilterIndex = myFilterNumber;

    Ed_EnableAllWindows (FALSE, pmWindow);
    mySaveFileDialogResult = GetSaveFileName (&myOpenFileName);
    Ed_EnableAllWindows (TRUE, pmWindow);

    if (!mySaveFileDialogResult) 
    {
	DWORD	myErrorCode = CommDlgExtendedError();
	if (myErrorCode)
	{
     	    EdGUI_Message (
    	    	"GetSaveFileName Error",
    	    	"An error occurred while attempting to get the file name."
    	    	"\n\n[Error code: %d]", myErrorCode);
    	}

    	myFilterNumber = myOpenFileName.nFilterIndex;
	return FALSE;
    }

    myFilterNumber = myOpenFileName.nFilterIndex;
    return TRUE;
} // EdGUI_GetSaveEXEFile
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)


/************************************************************************/
/* EdGUI_GetSaveFile							*/
/************************************************************************/
BOOL	EdGUI_GetSaveFile (HWND pmWindow, char *pmPathName, BOOL pmSelection)
{
    OPENFILENAME	myOpenFileName;
    FilePath		myActiveDirectory;
    BOOL		mySaveFileDialogResult;
    static int		myFilterNumber = 1;
    
#if defined (CONSOLE) || defined (TPROLOG)
    myActiveDirectory [0] = 0;
#else // #if defined (CONSOLE) || defined (TPROLOG)
    EdFile_GetActiveDirectory (myActiveDirectory);
#endif // #else - #if defined (CONSOLE) || defined (TPROLOG)
    
    ZeroMemory (&myOpenFileName, sizeof (OPENFILENAME));

    myOpenFileName.lStructSize = stOPENFILENAMESize;
    myOpenFileName.hwndOwner   = pmWindow;
    myOpenFileName.lpstrFile   = pmPathName;
    if (strlen (myActiveDirectory) > 0)
    {
    	myOpenFileName.lpstrInitialDir = myActiveDirectory;
    }
    myOpenFileName.nMaxFile    = PATHNAME_LENGTH;
    myOpenFileName.lCustData   = SAVE_FILE;
    if (pmSelection)
    {
    	myOpenFileName.lpstrTitle  = "Save Selection";
    }
    else
    {
    	myOpenFileName.lpstrTitle  = "Save File";
    }
    myOpenFileName.lpfnHook = MyFileDialogProcedure;
    myOpenFileName.Flags = OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | 
        OFN_EXPLORER | OFN_ENABLEHOOK | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING;

#if !defined (CONSOLE) && !defined (TPROLOG)
    if (gProgram.isJava)
    {
        myOpenFileName.lpstrFilter  = stJavaFileNameFilter;
    }
    else if (gProgram.isTuring)
    {
        myOpenFileName.lpstrFilter  = stTuringFileNameFilter;
    }
    else
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)
    {
    	myOpenFileName.lpstrFilter  = stFileNameFilter;
    }
    myOpenFileName.nFilterIndex = myFilterNumber;

#if !defined (CONSOLE) && !defined (TPROLOG)
    Ed_EnableAllWindows (FALSE, pmWindow);
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)
    mySaveFileDialogResult = GetSaveFileName (&myOpenFileName);
#if !defined (CONSOLE) && !defined (TPROLOG)
    Ed_EnableAllWindows (TRUE, pmWindow);
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)
    if (!mySaveFileDialogResult) 
    {
	DWORD	myErrorCode = CommDlgExtendedError();
	if (myErrorCode)
	{
     	    EdGUI_Message1 (pmWindow, MB_ICONEXCLAMATION, 
     	        IDS_FILE_DIALOG_ERROR_TITLE, IDS_FILE_DIALOG_ERROR_MESSAGE,
    	    	myErrorCode);
    	}

    	myFilterNumber = myOpenFileName.nFilterIndex;
	return FALSE;
    }

    myFilterNumber = myOpenFileName.nFilterIndex;
    return TRUE;
} // EdGUI_GetSaveFile


#if !defined (CONSOLE) && !defined (TPROLOG)
/************************/
/* EdGUI_GetSaveRTFFile */
/************************/
BOOL	EdGUI_GetSaveRTFFile (HWND pmWindow, char *pmPathName)
{
    OPENFILENAME	myOpenFileName;
    FilePath		myActiveDirectory;
    BOOL		mySaveFileDialogResult;
    static int		myFilterNumber = 1;
    
    EdFile_GetActiveDirectory (myActiveDirectory);
    
    ZeroMemory (&myOpenFileName, sizeof (OPENFILENAME));

    myOpenFileName.lStructSize = stOPENFILENAMESize;
    myOpenFileName.hwndOwner   = pmWindow;
    myOpenFileName.lpstrFile   = pmPathName;
    if (strlen (myActiveDirectory) > 0)
    {
    	myOpenFileName.lpstrInitialDir = myActiveDirectory;
    }
    myOpenFileName.nMaxFile    = PATHNAME_LENGTH;
    myOpenFileName.lCustData   = SAVE_FILE;
    myOpenFileName.lpstrTitle  = "Save in RTF Format";
    myOpenFileName.lpfnHook = MyFileDialogProcedure;
    myOpenFileName.Flags = OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | 
        OFN_EXPLORER | OFN_ENABLEHOOK | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING;

    if (gProgram.isJava)
    {
        myOpenFileName.lpstrFilter  = stJavaRTFFileNameFilter;
    }
    else if (gProgram.isTuring)
    {
        myOpenFileName.lpstrFilter  = stTuringRTFFileNameFilter;
    }
    else
    {
    	myOpenFileName.lpstrFilter  = stRTFFileNameFilter;
    }
    myOpenFileName.nFilterIndex = myFilterNumber;

    Ed_EnableAllWindows (FALSE, pmWindow);
    mySaveFileDialogResult = GetSaveFileName (&myOpenFileName);
    Ed_EnableAllWindows (TRUE, pmWindow);

    if (!mySaveFileDialogResult) 
    {
	DWORD	myErrorCode = CommDlgExtendedError();
	if (myErrorCode)
	{
     	    EdGUI_Message (
    	    	"GetSaveFileName Error",
    	    	"An error occurred while attempting to get the file name."
    	    	"\n\n[Error code: %d]", myErrorCode);
    	}

    	myFilterNumber = myOpenFileName.nFilterIndex;
	return FALSE;
    }

    myFilterNumber = myOpenFileName.nFilterIndex;
    return TRUE;
} // EdGUI_GetSaveRTFFile


/************************************************************************/
/* EdGUI_GetTicks							*/
/************************************************************************/
long	EdGUI_GetTicks (void)
{
    if (stUseTicks)
    {
    	return timeGetTime ();
    }
    else
    {
    	LARGE_INTEGER	myLargeInteger;
	__int64		myDifference;
	
    	QueryPerformanceCounter (&myLargeInteger);
    	myDifference = ((__int64) myLargeInteger.QuadPart) - 
    		       ((__int64) stStartCount.QuadPart);
    	return (long) (myDifference * 1000 / 
    		       (__int64) stCounterFrequency.QuadPart);
    }
} // EdGUI_GetTicks
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)


/************************************************************************/
/* EdGUI_GetTopWindow							*/
/*									*/
/* Return the top level window associated with a child window.		*/
/************************************************************************/
HWND	EdGUI_GetTopWindow (HWND pmWindow)
{
    HWND	myWindow = pmWindow, myParentWindow;
    char	myWindowClass [256];
    
    if (myWindow != NULL)
    {
    	GetClassName (myWindow, myWindowClass, sizeof (myWindowClass));
    	while (((GetWindowLong (myWindow, GWL_STYLE) & WS_CHILD) != 0) ||
    	       (strcmp (myWindowClass, TOOLTIPS_CLASS) == 0))
    	{
    	    myParentWindow = GetParent (myWindow);
	    if (myParentWindow == NULL)
	    {
		return myWindow;
	    }
	    myWindow = myParentWindow;
    	    GetClassName (myWindow, myWindowClass, sizeof (myWindowClass));
    	}
    }
    
    return myWindow;
} // EdGUI_GetTopWindow


/**********************************************************/
/* Get the screen's work area (i.e. minus task bar. etc.) */
/**********************************************************/
RECT	EdGUI_GetWorkArea (void)
{
    RECT	myRect;

    SystemParametersInfo (SPI_GETWORKAREA, 0, &myRect, 0);

    return myRect;
} // EdGUI_GetWorkArea


#ifndef TPROLOG
#ifndef CONSOLE
/************************************************************************/
/* EdGUI_IsAllowedAccess						*/
/*									*/
/* We determine if the file is acceptable with the following		*/
/* algorithm: If studentRestrictedToStartupDir is true and		*/
/* the path is under the start-up directory, then its OK.  If		*/
/* studentRestrictedToStartupDir is true and and the path is		*/
/* on the same drive and not under the start-up directory, then		*/
/* the its rejected.  If studentDrivesAllowed is true and		*/
/* the path is on accepted drive, then its OK.  If			*/
/* studentDrivesAllowed is true and the path is not on an		*/
/* accepted drive, then its rejected.  If studentDrivesForbidden	*/
/* is true and the path is on a forbidden drive, then 			*/
/************************************************************************/
BOOL	EdGUI_IsAllowedAccess (HWND pmWindow, const char *pmPathName, 
			       BOOL pmIsSave)
{
    FilePath	myPath, myStartUpDir;
    int		myErrorDialogTitle;
    char	myDrive;
    
    strcpy (myPath, pmPathName);
    EdGlob_ConvertToUpperCase (myPath);

    if (pmIsSave)
    {
	myErrorDialogTitle = IDS_FILE_SAVE_DISALLOWED_TITLE;
    }
    else
    {
	myErrorDialogTitle = IDS_FILE_OPEN_DISALLOWED_TITLE;
    }
		    	        
    strcpy (myStartUpDir, EdProp_GetStartupDirectory ());
    EdGlob_ConvertToUpperCase (myStartUpDir);
    	    	
    if (gProperties.studentRestrictedToStartupDir &&
    	(strlen (myPath) > 1) && 
    	(strncmp (myPath, myStartUpDir, 2) == 0))
    {
	if (strncmp (myPath, myStartUpDir, strlen (myStartUpDir)) == 0)
    	{
	    return TRUE;
	}
	else
	{
	    EdGUI_Message1 (pmWindow, MB_ICONEXCLAMATION, myErrorDialogTitle,
	        IDS_FILE_ONLY_IN_HOME, EdProp_GetStartupDirectory ());
    	    MessageBeep (MB_ICONEXCLAMATION);
	    return FALSE;
    	}
    }
    	    	
    if (strncmp (myPath, "\\\\", 2) == 0)
    {
    	myDrive = '*';
    }
    else
    {
    	myDrive = myPath [0];
    }
    	    	
    if (gProperties.studentDrivesAllowed)
    {
    	if (strchr (gProperties.studentDrivesAllowedList, myDrive) == NULL)
    	{
    	    if (myDrive == '*')
    	    {
    	    	EdGUI_Message1 (pmWindow, MB_ICONEXCLAMATION, 
    	    	    myErrorDialogTitle, IDS_NETWORK_DRIVE_NOT_PERMITTED);
    	    }
    	    else
    	    {
    	    	EdGUI_Message1 (pmWindow, MB_ICONEXCLAMATION, 
    	    	    myErrorDialogTitle, IDS_DRIVE_NOT_PERMITTED, myDrive);
    	    }
    	    MessageBeep (MB_ICONEXCLAMATION);
    	    return FALSE;
    	}
    }
    else if (gProperties.studentDrivesForbidden)
    {
    	if (strchr (gProperties.studentDrivesForbiddenList, myDrive) != NULL)
    	{
    	    if (myDrive == '*')
    	    {
    	    	EdGUI_Message1 (pmWindow, MB_ICONEXCLAMATION, 
    	    	    myErrorDialogTitle, IDS_NETWORK_DRIVE_NOT_PERMITTED);
    	    }
    	    else
    	    {
    	    	EdGUI_Message1 (pmWindow, MB_ICONEXCLAMATION, 
    	    	    myErrorDialogTitle, IDS_DRIVE_NOT_PERMITTED, myDrive);
    	    }
    	    MessageBeep (MB_ICONEXCLAMATION);
    	    return FALSE;
    	}
    }
    
    return TRUE;
} // EdGUI_IsAllowedAccess


/************************************************************************/
/* EdGUI_JavaIdentifierOnly						*/
/************************************************************************/
BOOL	EdGUI_JavaIdentifierOnly (HWND pmDialog, int pmDialogPart, 
				  int pmMessagePart)
{
    HWND	myItem, myMessage;
    char	myClassName [128];
    char	myError [1024];
    UINT	cnt;
    	    	    	
    myItem = GetDlgItem (pmDialog, pmDialogPart);
    myMessage = GetDlgItem (pmDialog, pmMessagePart);

    GetWindowText (myItem, myClassName, sizeof (myClassName));

    if (strlen (myClassName) == 0)
    {
        SetWindowText (myMessage, "");
    	return FALSE;
    }
    
    // Check that the first character is legal
    if (!EdJava_IsIdentStart (myClassName [0]))
    {
	EdGUI_LoadString (IDS_BAD_FIRST_CLASSNAME_CHAR, myError, 
	    sizeof (myError));
	SetWindowText (myMessage, myError);
    	MessageBeep (MB_ICONEXCLAMATION);
    	return FALSE;
    }
    
    // Check subsequent characters are legal
    for (cnt = 1 ; cnt < strlen (myClassName) ; cnt++)
    {
    	if (!EdJava_IsIdentFollow (myClassName [cnt]))
    	{
	    EdGUI_LoadString (IDS_BAD_FOLLOW_CLASSNAME_CHAR, myError, 
	    	sizeof (myError));
	    SetWindowText (myMessage, myError);
    	    MessageBeep (MB_ICONEXCLAMATION);
    	    return FALSE;
    	}
   }

    // Check that the first character is upper case
    if (!EdJava_IsUpperCase (myClassName [0]))
    {
	EdGUI_LoadString (IDS_LOWER_FIRST_CLASSNAME_CHAR, myError, 
	    sizeof (myError));
	SetWindowText (myMessage, myError);
    	MessageBeep (MB_ICONEXCLAMATION);
    	return TRUE;
    }
    
    SetWindowText (myMessage, "");
    return TRUE;
} // EdGUI_JavaIdentifierOnly


/************************************************************************/
/* EdGUI_LoadFormattedString						*/
/************************************************************************/
void	EdGUI_LoadFormattedString (int pmStringResource, char *pmString, 
				   int pmStringSize, ...)
{
    char	*myPtr;
    int		myResult;
    char 	myString [4096];
    va_list 	myArgList;
    char	*myFormattedString;
    
    // Get the window format string    
    EdGUI_LoadString (pmStringResource, myString, sizeof (myString));
    myPtr = strstr (myString, ENV_NAME_STRING);
    while (myPtr != NULL)
    {
    	memmove (myPtr + strlen (gProgram.environmentName),
		 myPtr + strlen (ENV_NAME_STRING),
		 strlen (myPtr) - strlen (ENV_NAME_STRING) + 1);
	strncpy (myPtr, gProgram.environmentName, 
		 strlen (gProgram.environmentName));
        myPtr = strstr (myString, ENV_NAME_STRING);
    }
    
    // Process the additional arguments for use in FormatMessage
    va_start (myArgList, pmStringSize);
    
    // Format the message
    myResult = FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_STRING, myString, 0, 
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &myFormattedString, 0, &myArgList);
    if (myResult == 0)	  
    {
    	EdFail_Fatal (IDS_FORMATMESSAGEFAIL, __FILE__, __LINE__, 
    	    GetLastError (), myString);
    	// Never reaches here
    	return;
    }    

    // Close the variable args    
    va_end (myArgList);

    if ((int) strlen (myFormattedString) > pmStringSize)
    {
    	EdFail_Fatal (IDS_LOADSTRINGFAIL, __FILE__, __LINE__, GetLastError (),
    	    pmStringResource);
    	// Never reaches here
    	return;
    }
    
    strcpy (pmString, myFormattedString);
    
    // Free the buffer allocated by FormatMessage
    LocalFree (myFormattedString);
} // EdGUI_LoadFormattedString


/************************************************************************/
/* EdGUI_LoadResource							*/
/************************************************************************/
BOOL	EdGUI_LoadResource (HMODULE pmLibrary, int pmResourceName, 
			    void *pmData, DWORD pmSize)
{
    HRSRC		myResourceHandle;
    HGLOBAL		myResourceData;
    
    myResourceHandle = FindResource (pmLibrary, 
    				     MAKEINTRESOURCE (pmResourceName), 
    				     RT_RCDATA);
    if (myResourceHandle == NULL)
    {    				    
    	EdFail_Warn (IDS_FINDRESOURCEFAIL, __FILE__, __LINE__, GetLastError (),
    		     pmResourceName);
    	return FALSE;
    }
    if (SizeofResource (pmLibrary, myResourceHandle) != pmSize)
    {
    	EdFail_Warn (IDS_RESOURCEWRONGSIZE, __FILE__, __LINE__, 0,
    		     pmResourceName, SizeofResource (NULL, myResourceHandle),
    		     pmSize);
    	return FALSE;
    }
    myResourceData = LoadResource (pmLibrary, myResourceHandle);
    if (myResourceData == NULL)
    {
    	EdFail_Warn (IDS_LOADRESOURCEFAIL, __FILE__, __LINE__, GetLastError (),
    		     pmResourceName);
    	return FALSE;
    }
    
    memcpy (pmData, (void *) myResourceData, pmSize);
    return TRUE;
} // EdGUI_LoadResource
#endif // #ifndef CONSOLE
#endif // #ifndef TPROLOG

/************************************************************************/
/* EdGUI_LoadString							*/
/************************************************************************/
void	EdGUI_LoadString (int pmStringResource, char *pmString, 
			  int pmStringSize)
{
    int	myResult;
    
    myResult = LoadString (gProgram.applicationInstance, pmStringResource,
			   pmString, pmStringSize);
    if (myResult == 0)	  
    {
    	EdFail_Fatal (IDS_LOADSTRINGFAIL, __FILE__, __LINE__, GetLastError (),
    	    pmStringResource);
    	// Never reaches here
    	return;
    }
} // EdGUI_LoadString


#ifndef CONSOLE			  
/************************************************************************/
/* EdGUI_Message							*/
/************************************************************************/
void	EdGUI_Message (char *pmTitle, char *pmMessage, ...)
{
    va_list 	myArgList;
    char 	myFullMessage [4096];
    
    va_start (myArgList, pmMessage);
    wvsprintf (myFullMessage, pmMessage, myArgList);
    va_end (myArgList);
  
    // For now, we don't have an effective way of making the message box
    // task modal, so we'll comment out the following.
//    Ed_EnableAllWindows (FALSE);
    MessageBox (NULL, myFullMessage, pmTitle, 
    		MB_OK | MB_ICONINFORMATION | MB_TASKMODAL);
//    Ed_EnableAllWindows (TRUE);
} // EdGUI_Message
#endif // #ifndef CONSOLE


/************************************************************************/
/* EdGUI_Message1							*/
/************************************************************************/
int	EdGUI_Message1 (HWND pmWindow, int pmMessageBoxParams, 
			int pmTitleStringResource, 
		        int pmMessageStringResource, ...)
{
    char	*myPtr;
    int		myResult;
    char 	myDialogTitle [1024], myDialogMessage [4096];
    va_list 	myArgList;
    char	*myFormattedMessage;
    HWND	myWindow;
    
    // Get the window title
    EdGUI_LoadString (pmTitleStringResource, myDialogTitle, 
    		      sizeof (myDialogTitle));
    myPtr = strstr (myDialogTitle, ENV_NAME_STRING);
    while (myPtr != NULL)
    {
    	memmove (myPtr + strlen (gProgram.environmentName),
		 myPtr + strlen (ENV_NAME_STRING),
		 strlen (myPtr) - strlen (ENV_NAME_STRING) + 1);
	strncpy (myPtr, gProgram.environmentName, 
		 strlen (gProgram.environmentName));
    	myPtr = strstr (myDialogTitle, ENV_NAME_STRING);
    }
    
    // Get the window format string    
    EdGUI_LoadString (pmMessageStringResource, myDialogMessage, 
    		      sizeof (myDialogMessage));
    myPtr = strstr (myDialogMessage, ENV_NAME_STRING);
    while (myPtr != NULL)
    {
    	memmove (myPtr + strlen (gProgram.environmentName),
		 myPtr + strlen (ENV_NAME_STRING),
		 strlen (myPtr) - strlen (ENV_NAME_STRING) + 1);
	strncpy (myPtr, gProgram.environmentName, 
		 strlen (gProgram.environmentName));
        myPtr = strstr (myDialogMessage, ENV_NAME_STRING);
    }
    
    // Process the additional arguments for use in FormatMessage
    va_start (myArgList, pmMessageStringResource);
    
    // Format the message
    myResult = FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_STRING, myDialogMessage, 0, 
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &myFormattedMessage, 0, &myArgList);
    if (myResult == 0)	  
    {
    	EdFail_Fatal (IDS_FORMATMESSAGEFAIL, __FILE__, __LINE__, 
    	    GetLastError (), myDialogMessage);
    	// Never reaches here
    	return 0;
    }    

    // Close the variable args    
    va_end (myArgList);

    // Get the a non-child window
    myWindow = EdGUI_GetTopWindow (pmWindow);
    
    // Set some default parameters
    if (pmMessageBoxParams == 0)
    {
    	pmMessageBoxParams = MB_OK | MB_ICONINFORMATION | MB_TASKMODAL;
    }
    else
    {
    	pmMessageBoxParams |= MB_TASKMODAL;
    }

    // Centre the message box 
    stMessageBoxHook = SetWindowsHookEx (WH_CBT, MyMessageBoxCenteringProc,
        				 NULL, GetCurrentThreadId ());

    // Open the message box
#if !defined (CONSOLE) && !defined (TPROLOG)
    Ed_EnableAllWindows (FALSE, pmWindow);
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)
    myResult = MessageBox (myWindow, myFormattedMessage, myDialogTitle, 
        pmMessageBoxParams);
#if !defined (CONSOLE) && !defined (TPROLOG)
    Ed_EnableAllWindows (TRUE, pmWindow);
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)
    // Free the buffer allocated by FormatMessage
    LocalFree (myFormattedMessage);
    
    return myResult;
} // EdGUI_Message1


/******************************/
/* Static callback procedures */
/******************************/

/************************************************************************/
/* MyFileDialogProcedure						*/
/************************************************************************/
static BOOL CALLBACK	MyFileDialogProcedure (HWND pmDialog, 
    UINT pmMessage, WPARAM pmWParam, LPARAM pmLParam)
{
    static BOOL	stIsSave;
    
    switch (pmMessage)
    {
    	case WM_INITDIALOG:
    	    stIsSave = (BOOL) ((OPENFILENAME *) pmLParam) -> lCustData;
#if !defined (CONSOLE) && !defined (TPROLOG)
	    Ed_SetModalDialog (GetParent (pmDialog)); 
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)
	    EdGUI_CentreDialogBox (GetParent (pmDialog));
	    break;
#if !defined (CONSOLE) && !defined (TPROLOG)
    	case WM_NOTIFY:
    	    if (((LPOFNOTIFY) pmLParam) -> hdr.code == CDN_FILEOK)
    	    {
		FilePath	myPath;
		
    	    	CommDlg_OpenSave_GetFilePath (GetParent (pmDialog), 
    	    	    myPath, PATHNAME_LENGTH);
    	        
    	        if (!EdGUI_IsAllowedAccess (pmDialog, myPath, stIsSave))
    	        {
		    SetWindowLong (pmDialog, DWL_MSGRESULT, TRUE);
		    return 1;
		}
    	    } // if CDN_FILEOK
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)
    } // switch
    
    return 0;
} // MyFileDialogProcedure


/************************************************************************/
/* MyMessageBoxCenteringProc						*/
/************************************************************************/
static LRESULT CALLBACK	MyMessageBoxCenteringProc (int pmHookCode, 
						   WPARAM pmWParam,
						   LPARAM pmLParam)
{
    LRESULT	myResult;
    
    myResult = CallNextHookEx (stMessageBoxHook, pmHookCode, pmWParam, 
        pmLParam);
        
    if (pmHookCode == HCBT_ACTIVATE)
    {
	UnhookWindowsHookEx (stMessageBoxHook);
#if !defined (CONSOLE) && !defined (TPROLOG)
	Ed_SetModalDialog ((HWND) pmWParam); 
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)
    	EdGUI_CentreDialogBox ((HWND) pmWParam);
    }
    
    return myResult;
} // MyMessageBoxCenteringProc
						   
/*********************/
/* Static procedures */
/*********************/
