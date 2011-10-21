/***********/
/* edlog.c */
/***********/

/*******************/
/* System includes */
/*******************/
// We define _WIN32_WINNT to get the extra windows messages
#define _WIN32_WINNT	0x0400
#include <windows.h>
#undef _WIN32_WINNT
#include <crtdbg.h>

/****************/
/* Self include */
/****************/
#include "edlog.h"

/******************/
/* Other includes */
/******************/
//#include "ed.h"
#include "edfail.h"
#include "edfile.h"
//#include "edglob.h"
#include "edgui.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
#ifdef TURING
#define LOG_FILE_NAME		"turing.log"
#else
#define LOG_FILE_NAME		"ready.log"
#endif
#define PANIC_LOG_FILE_NAME	"panic.log"

#define NO_LOGGING		FALSE

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
// Properties of the Log module
static EdLog_Properties	stProperties;
static BOOL		stPropertiesChanged;

// Other static variables
static BOOL		stLoggingRequested = FALSE;
static BOOL		stPanicLogOpened;
static FileEntryPtr	stLogFile = NULL;
static FileEntryPtr	stPanicLogFile;

/******************************/
/* Static callback procedures */
/******************************/
int CALLBACK	MyLogFontEnumerateAllFonts (LPLOGFONT pmLogFont, 
					    LPTEXTMETRIC pmTextMetric, 
					    DWORD pmFontType, LPARAM pmHDC);
int CALLBACK	MyLogFontEnumerateSingleFont (LPLOGFONT pmLogFont, 
    					      LPTEXTMETRIC pmTextMetric, 
    					      DWORD pmFontType, 
    					      LPARAM pmNumFontsInFamily);

/*********************/
/* Static procedures */
/*********************/
static FileEntryPtr	MyOpenLogFile (char *pmFileName, char *pmErrorTitle,
				       char *pmErrorFileName);

/***********************/
/* External procedures */
/***********************/
/************************************************************************/
/* EdLog_Init								*/
/************************************************************************/
BOOL	EdLog_Init (void)
{
    return TRUE;
} // EdLog_Init


/************************************************************************/
/* EdLog_Finalize							*/
/************************************************************************/
void	EdLog_Finalize (void)
{
} // EdLog_Finalize


/************************************************************************/
/* EdLog_PropertiesSet							*/
/************************************************************************/
void	EdLog_PropertiesSet (EdLog_Properties pmProperties)
{
    stProperties = pmProperties;
    stPropertiesChanged = TRUE;
} // EdLog_PropertiesSet

	
/************************************************************************/
/* EdLog_PropertiesImplementChanges					*/
/************************************************************************/
void	EdLog_PropertiesImplementChanges (void)
{
    stPropertiesChanged = FALSE;
} // EdLog_PropertiesImplementChanges


/************************************************************************/
/* EdLog_GetLogFileHandle						*/
/************************************************************************/
HANDLE	EdLog_GetLogFileHandle (void)
{
    if (stLogFile == NULL)
    {
    	// TW Error
    	return NULL;
    }
    
    return stLogFile -> handle;
} // EdLog_GetLogFileHandle


/************************************************************************/
/* EdLog_Log								*/
/************************************************************************/
void	EdLog_Log (char *pmFormatString, ...)
{
    if (stLoggingRequested)
    {
        va_list	myArgList;
        char	myFullMessage [2048];
    
        va_start (myArgList, pmFormatString);
        wvsprintf (myFullMessage, pmFormatString, myArgList);
        va_end (myArgList);
        
    	EdFile_Writeln (stLogFile, myFullMessage);
    }
} // EdLog_Log


/*****************/
/* EdLog_LogFont */
/*****************/
void	EdLog_LogFont (const char *pmLabel, HDC pmHDC)
{
    if (stLoggingRequested)
    {
	char		myTextFace [256];
	TEXTMETRIC	myTextMetric;

	EdFile_Writeln (stLogFile, pmLabel);
	GetTextFace (pmHDC, 256, myTextFace);
	EdFile_Writeln (stLogFile, "Text Face = \"%s\"", myTextFace);
	GetTextMetrics (pmHDC, &myTextMetric);
	EdFile_Writeln (stLogFile, "Text Height = %d",  
	    myTextMetric.tmHeight);
	EdFile_Writeln (stLogFile, "Text Width = %d",  
	    myTextMetric.tmAveCharWidth);
	EdFile_Writeln (stLogFile, "Text Weight = %d",  
	    myTextMetric.tmWeight);
	EdFile_Writeln (stLogFile, "Text Pitch = 0x%x",  
	    myTextMetric.tmPitchAndFamily);
	EdFile_Writeln (stLogFile, "Text Char Set = 0x%x", 
	    myTextMetric.tmCharSet);
    }
} // EdLog_LogFont


/**************************/
/* EdLog_LogFontEnumerate */
/**************************/
void	EdLog_LogFontEnumerate (HDC hdc)
{
    if (stLoggingRequested)
    {
    	EdFile_WriteNL (stLogFile);
    	EdFile_Writeln (stLogFile, "    Font Enumeration");
    	EdFile_Writeln (stLogFile, "    ---- -----------");
	EnumFontFamilies (hdc, NULL, (FONTENUMPROC) MyLogFontEnumerateAllFonts, 
	    (LPARAM) hdc);
    }
} // logEnumFonts


/***********************/
/* EdLog_SetWriteToLog */
/***********************/
void	EdLog_SetWriteToLog (void)
{
    stLoggingRequested = TRUE;
    
    stLogFile = MyOpenLogFile (LOG_FILE_NAME, "Log File Error", "log file");
    if (stLogFile == NULL)
    {
        stLoggingRequested = FALSE;
    }
} // EdLog_SetWriteToLog


/***********************/
/* EdLog_WritePanicLog */
/***********************/
void	EdLog_WritePanicLog  (char *pmFormatString, ...)
{
    va_list	myArgList;
    char	myFullMessage [2048];
    
    va_start (myArgList, pmFormatString);
    wvsprintf (myFullMessage, pmFormatString, myArgList);
    va_end (myArgList);
    
    if (!stPanicLogOpened)
    {
    	stPanicLogFile = MyOpenLogFile (PANIC_LOG_FILE_NAME, "Panic Log Error", 
    				        "panic log");
    }
    if (stPanicLogFile != NULL)
    {
    	EdFile_Writeln (stPanicLogFile, myFullMessage);
    }
} // EdLog_WritePanicLog


/*****************************/
/* EdLog_PrintWindowsMessage */
/*****************************/
void EdLog_PrintWindowsMessage (const char *pmTitle, HWND pmWindow, 
				int pmMessage, WPARAM pmWParam, LPARAM pmLParam)
{
    char	*myMessage;
    char	myUnknownMessage [128];
    
    switch (pmMessage)
    {
 	case WM_NULL: myMessage = "WM_NULL"; break;
	case WM_CREATE: myMessage = "WM_CREATE"; break;
	case WM_DESTROY: myMessage = "WM_DESTROY"; break;
	case WM_MOVE: myMessage = "WM_MOVE"; break;
	case WM_SIZE: myMessage = "WM_SIZE"; break;
	case WM_ACTIVATE: myMessage = "WM_ACTIVATE"; break;
	case WM_SETFOCUS: myMessage = "WM_SETFOCUS"; break;
	case WM_KILLFOCUS: myMessage = "WM_KILLFOCUS"; break;
	case WM_ENABLE: myMessage = "WM_ENABLE"; break;
	case WM_SETREDRAW: myMessage = "WM_SETREDRAW"; break;
	case WM_SETTEXT: myMessage = "WM_SETTEXT"; break;
	case WM_GETTEXT: myMessage = "WM_GETTEXT"; break;
	case WM_GETTEXTLENGTH: myMessage = "WM_GETTEXTLENGTH"; break;
	case WM_PAINT: myMessage = "WM_PAINT"; break;
	case WM_CLOSE: myMessage = "WM_CLOSE"; break;
	case WM_QUERYENDSESSION: myMessage = "WM_QUERYENDSESSION"; break;
	case WM_QUIT: myMessage = "WM_QUIT"; break;
	case WM_QUERYOPEN: myMessage = "WM_QUERYOPEN"; break;
	case WM_ERASEBKGND: myMessage = "WM_ERASEBKGND"; break;
	case WM_SYSCOLORCHANGE: myMessage = "WM_SYSCOLORCHANGE"; break;
	case WM_ENDSESSION: myMessage = "WM_ENDSESSION"; break;
	case WM_SHOWWINDOW: myMessage = "WM_SHOWWINDOW"; break;
	case WM_SETTINGCHANGE: myMessage = "WM_SETTINGCHANGE"; break;
	case WM_DEVMODECHANGE: myMessage = "WM_DEVMODECHANGE"; break;
	case WM_ACTIVATEAPP: myMessage = "WM_ACTIVATEAPP"; break;
	case WM_FONTCHANGE: myMessage = "WM_FONTCHANGE"; break;
	case WM_TIMECHANGE: myMessage = "WM_TIMECHANGE"; break;
	case WM_CANCELMODE: myMessage = "WM_CANCELMODE"; break;
	case WM_SETCURSOR: myMessage = "WM_SETCURSOR"; break;
	case WM_MOUSEACTIVATE: myMessage = "WM_MOUSEACTIVATE"; break;
	case WM_CHILDACTIVATE: myMessage = "WM_CHILDACTIVATE"; break;
	case WM_QUEUESYNC: myMessage = "WM_QUEUESYNC"; break;
	case WM_GETMINMAXINFO: myMessage = "WM_GETMINMAXINFO"; break;
	case WM_PAINTICON: myMessage = "WM_PAINTICON"; break;
	case WM_ICONERASEBKGND: myMessage = "WM_ICONERASEBKGND"; break;
	case WM_NEXTDLGCTL: myMessage = "WM_NEXTDLGCTL"; break;
	case WM_SPOOLERSTATUS: myMessage = "WM_SPOOLERSTATUS"; break;
	case WM_DRAWITEM: myMessage = "WM_DRAWITEM"; break;
	case WM_MEASUREITEM: myMessage = "WM_MEASUREITEM"; break;
	case WM_DELETEITEM: myMessage = "WM_DELETEITEM"; break;
	case WM_VKEYTOITEM: myMessage = "WM_VKEYTOITEM"; break;
	case WM_CHARTOITEM: myMessage = "WM_CHARTOITEM"; break;
	case WM_SETFONT: myMessage = "WM_SETFONT"; break;
	case WM_GETFONT: myMessage = "WM_GETFONT"; break;
	case WM_SETHOTKEY: myMessage = "WM_SETHOTKEY"; break;
	case WM_GETHOTKEY: myMessage = "WM_GETHOTKEY"; break;
	case WM_QUERYDRAGICON: myMessage = "WM_QUERYDRAGICON"; break;
	case WM_COMPAREITEM: myMessage = "WM_COMPAREITEM"; break;
	case WM_COMPACTING: myMessage = "WM_COMPACTING"; break;
	case WM_COMMNOTIFY  /* no longer suported */: myMessage = "WM_COMMNOTIFY  /* no longer suported */"; break;
	case WM_WINDOWPOSCHANGING: myMessage = "WM_WINDOWPOSCHANGING"; break;
	case WM_WINDOWPOSCHANGED: myMessage = "WM_WINDOWPOSCHANGED"; break;
	case WM_POWER: myMessage = "WM_POWER"; break;
	case WM_COPYDATA: myMessage = "WM_COPYDATA"; break;
	case WM_CANCELJOURNAL: myMessage = "WM_CANCELJOURNAL"; break;
	case WM_NOTIFY: myMessage = "WM_NOTIFY"; break;
	case WM_INPUTLANGCHANGEREQUEST: myMessage = "WM_INPUTLANGCHANGEREQUEST"; break;
	case WM_INPUTLANGCHANGE: myMessage = "WM_INPUTLANGCHANGE"; break;
	case WM_TCARD: myMessage = "WM_TCARD"; break;
	case WM_HELP: myMessage = "WM_HELP"; break;
	case WM_USERCHANGED: myMessage = "WM_USERCHANGED"; break;
	case WM_NOTIFYFORMAT: myMessage = "WM_NOTIFYFORMAT"; break;
	case WM_CONTEXTMENU: myMessage = "WM_CONTEXTMENU"; break;
	case WM_STYLECHANGING: myMessage = "WM_STYLECHANGING"; break;
	case WM_STYLECHANGED: myMessage = "WM_STYLECHANGED"; break;
	case WM_DISPLAYCHANGE: myMessage = "WM_DISPLAYCHANGE"; break;
	case WM_GETICON: myMessage = "WM_GETICON"; break;
	case WM_SETICON: myMessage = "WM_SETICON"; break;
	case WM_NCCREATE: myMessage = "WM_NCCREATE"; break;
	case WM_NCDESTROY: myMessage = "WM_NCDESTROY"; break;
	case WM_NCCALCSIZE: myMessage = "WM_NCCALCSIZE"; break;
	case WM_NCHITTEST: myMessage = "WM_NCHITTEST"; break;
	case WM_NCPAINT: myMessage = "WM_NCPAINT"; break;
	case WM_NCACTIVATE: myMessage = "WM_NCACTIVATE"; break;
	case WM_GETDLGCODE: myMessage = "WM_GETDLGCODE"; break;
	case WM_NCMOUSEMOVE: myMessage = "WM_NCMOUSEMOVE"; break;
	case WM_NCLBUTTONDOWN: myMessage = "WM_NCLBUTTONDOWN"; break;
	case WM_NCLBUTTONUP: myMessage = "WM_NCLBUTTONUP"; break;
	case WM_NCLBUTTONDBLCLK: myMessage = "WM_NCLBUTTONDBLCLK"; break;
	case WM_NCRBUTTONDOWN: myMessage = "WM_NCRBUTTONDOWN"; break;
	case WM_NCRBUTTONUP: myMessage = "WM_NCRBUTTONUP"; break;
	case WM_NCRBUTTONDBLCLK: myMessage = "WM_NCRBUTTONDBLCLK"; break;
	case WM_NCMBUTTONDOWN: myMessage = "WM_NCMBUTTONDOWN"; break;
	case WM_NCMBUTTONUP: myMessage = "WM_NCMBUTTONUP"; break;
	case WM_NCMBUTTONDBLCLK: myMessage = "WM_NCMBUTTONDBLCLK"; break;
	case WM_KEYDOWN: myMessage = "WM_KEYDOWN"; break;
	case WM_KEYUP: myMessage = "WM_KEYUP"; break;
	case WM_CHAR: myMessage = "WM_CHAR"; break;
	case WM_DEADCHAR: myMessage = "WM_DEADCHAR"; break;
	case WM_SYSKEYDOWN: myMessage = "WM_SYSKEYDOWN"; break;
	case WM_SYSKEYUP: myMessage = "WM_SYSKEYUP"; break;
	case WM_SYSCHAR: myMessage = "WM_SYSCHAR"; break;
	case WM_SYSDEADCHAR: myMessage = "WM_SYSDEADCHAR"; break;
	case WM_KEYLAST: myMessage = "WM_KEYLAST"; break;
	case WM_IME_STARTCOMPOSITION: myMessage = "WM_IME_STARTCOMPOSITION"; break;
	case WM_IME_ENDCOMPOSITION: myMessage = "WM_IME_ENDCOMPOSITION"; break;
	case WM_IME_COMPOSITION: myMessage = "WM_IME_COMPOSITION"; break;
	case WM_INITDIALOG: myMessage = "WM_INITDIALOG"; break;
	case WM_COMMAND: myMessage = "WM_COMMAND"; break;
	case WM_SYSCOMMAND: myMessage = "WM_SYSCOMMAND"; break;
	case WM_TIMER: myMessage = "WM_TIMER"; break;
	case WM_HSCROLL: myMessage = "WM_HSCROLL"; break;
	case WM_VSCROLL: myMessage = "WM_VSCROLL"; break;
	case WM_INITMENU: myMessage = "WM_INITMENU"; break;
	case WM_INITMENUPOPUP: myMessage = "WM_INITMENUPOPUP"; break;
	case WM_MENUSELECT: myMessage = "WM_MENUSELECT"; break;
	case WM_MENUCHAR: myMessage = "WM_MENUCHAR"; break;
	case WM_ENTERIDLE: myMessage = "WM_ENTERIDLE"; break;
	case WM_CTLCOLORMSGBOX: myMessage = "WM_CTLCOLORMSGBOX"; break;
	case WM_CTLCOLOREDIT: myMessage = "WM_CTLCOLOREDIT"; break;
	case WM_CTLCOLORLISTBOX: myMessage = "WM_CTLCOLORLISTBOX"; break;
	case WM_CTLCOLORBTN: myMessage = "WM_CTLCOLORBTN"; break;
	case WM_CTLCOLORDLG: myMessage = "WM_CTLCOLORDLG"; break;
	case WM_CTLCOLORSCROLLBAR: myMessage = "WM_CTLCOLORSCROLLBAR"; break;
	case WM_CTLCOLORSTATIC: myMessage = "WM_CTLCOLORSTATIC"; break;
	case WM_MOUSEMOVE: myMessage = "WM_MOUSEMOVE"; break;
	case WM_LBUTTONDOWN: myMessage = "WM_LBUTTONDOWN"; break;
	case WM_LBUTTONUP: myMessage = "WM_LBUTTONUP"; break;
	case WM_LBUTTONDBLCLK: myMessage = "WM_LBUTTONDBLCLK"; break;
	case WM_RBUTTONDOWN: myMessage = "WM_RBUTTONDOWN"; break;
	case WM_RBUTTONUP: myMessage = "WM_RBUTTONUP"; break;
	case WM_RBUTTONDBLCLK: myMessage = "WM_RBUTTONDBLCLK"; break;
	case WM_MBUTTONDOWN: myMessage = "WM_MBUTTONDOWN"; break;
	case WM_MBUTTONUP: myMessage = "WM_MBUTTONUP"; break;
	case WM_MBUTTONDBLCLK: myMessage = "WM_MBUTTONDBLCLK"; break;
	case WM_MOUSEWHEEL: myMessage = "WM_MOUSEWHEEL"; break;
	case WM_PARENTNOTIFY: myMessage = "WM_PARENTNOTIFY"; break;
	case WM_ENTERMENULOOP: myMessage = "WM_ENTERMENULOOP"; break;
	case WM_EXITMENULOOP: myMessage = "WM_EXITMENULOOP"; break;
	case WM_NEXTMENU: myMessage = "WM_NEXTMENU"; break;
	case WM_SIZING: myMessage = "WM_SIZING"; break;
	case WM_CAPTURECHANGED: myMessage = "WM_CAPTURECHANGED"; break;
	case WM_MOVING: myMessage = "WM_MOVING"; break;
	case WM_POWERBROADCAST: myMessage = "WM_POWERBROADCAST"; break;
	case WM_DEVICECHANGE: myMessage = "WM_DEVICECHANGE"; break;
	case WM_IME_SETCONTEXT: myMessage = "WM_IME_SETCONTEXT"; break;
	case WM_IME_NOTIFY: myMessage = "WM_IME_NOTIFY"; break;
	case WM_IME_CONTROL: myMessage = "WM_IME_CONTROL"; break;
	case WM_IME_COMPOSITIONFULL: myMessage = "WM_IME_COMPOSITIONFULL"; break;
	case WM_IME_SELECT: myMessage = "WM_IME_SELECT"; break;
	case WM_IME_CHAR: myMessage = "WM_IME_CHAR"; break;
	case WM_IME_KEYDOWN: myMessage = "WM_IME_KEYDOWN"; break;
	case WM_IME_KEYUP: myMessage = "WM_IME_KEYUP"; break;
	case WM_MDICREATE: myMessage = "WM_MDICREATE"; break;
	case WM_MDIDESTROY: myMessage = "WM_MDIDESTROY"; break;
	case WM_MDIACTIVATE: myMessage = "WM_MDIACTIVATE"; break;
	case WM_MDIRESTORE: myMessage = "WM_MDIRESTORE"; break;
	case WM_MDINEXT: myMessage = "WM_MDINEXT"; break;
	case WM_MDIMAXIMIZE: myMessage = "WM_MDIMAXIMIZE"; break;
	case WM_MDITILE: myMessage = "WM_MDITILE"; break;
	case WM_MDICASCADE: myMessage = "WM_MDICASCADE"; break;
	case WM_MDIICONARRANGE: myMessage = "WM_MDIICONARRANGE"; break;
	case WM_MDIGETACTIVE: myMessage = "WM_MDIGETACTIVE"; break;
	case WM_MDISETMENU: myMessage = "WM_MDISETMENU"; break;
	case WM_ENTERSIZEMOVE: myMessage = "WM_ENTERSIZEMOVE"; break;
	case WM_EXITSIZEMOVE: myMessage = "WM_EXITSIZEMOVE"; break;
	case WM_DROPFILES: myMessage = "WM_DROPFILES"; break;
	case WM_MDIREFRESHMENU: myMessage = "WM_MDIREFRESHMENU"; break;
	case WM_MOUSEHOVER: myMessage = "WM_MOUSEHOVER"; break;
	case WM_MOUSELEAVE: myMessage = "WM_MOUSELEAVE"; break;
	case WM_CUT: myMessage = "WM_CUT"; break;
	case WM_COPY: myMessage = "WM_COPY"; break;
	case WM_PASTE: myMessage = "WM_PASTE"; break;
	case WM_CLEAR: myMessage = "WM_CLEAR"; break;
	case WM_UNDO: myMessage = "WM_UNDO"; break;
	case WM_RENDERFORMAT: myMessage = "WM_RENDERFORMAT"; break;
	case WM_RENDERALLFORMATS: myMessage = "WM_RENDERALLFORMATS"; break;
	case WM_DESTROYCLIPBOARD: myMessage = "WM_DESTROYCLIPBOARD"; break;
	case WM_DRAWCLIPBOARD: myMessage = "WM_DRAWCLIPBOARD"; break;
	case WM_PAINTCLIPBOARD: myMessage = "WM_PAINTCLIPBOARD"; break;
	case WM_VSCROLLCLIPBOARD: myMessage = "WM_VSCROLLCLIPBOARD"; break;
	case WM_SIZECLIPBOARD: myMessage = "WM_SIZECLIPBOARD"; break;
	case WM_ASKCBFORMATNAME: myMessage = "WM_ASKCBFORMATNAME"; break;
	case WM_CHANGECBCHAIN: myMessage = "WM_CHANGECBCHAIN"; break;
	case WM_HSCROLLCLIPBOARD: myMessage = "WM_HSCROLLCLIPBOARD"; break;
	case WM_QUERYNEWPALETTE: myMessage = "WM_QUERYNEWPALETTE"; break;
	case WM_PALETTEISCHANGING: myMessage = "WM_PALETTEISCHANGING"; break;
	case WM_PALETTECHANGED: myMessage = "WM_PALETTECHANGED"; break;
	case WM_HOTKEY: myMessage = "WM_HOTKEY"; break;
	case WM_PRINT: myMessage = "WM_PRINT"; break;
	case WM_PRINTCLIENT: myMessage = "WM_PRINTCLIENT"; break;
	case WM_HANDHELDFIRST: myMessage = "WM_HANDHELDFIRST"; break;
	case WM_HANDHELDLAST: myMessage = "WM_HANDHELDLAST"; break;
	case WM_AFXFIRST: myMessage = "WM_AFXFIRST"; break;
	case WM_AFXLAST: myMessage = "WM_AFXLAST"; break;
	case WM_PENWINFIRST: myMessage = "WM_PENWINFIRST"; break;
	case WM_PENWINLAST: myMessage = "WM_PENWINLAST"; break;
	default:
	    wsprintf (myUnknownMessage, "Unknown message [%d]", pmMessage);
	    myMessage = myUnknownMessage;
	    break;
    } // switch
    
    EdFile_Writeln (stLogFile, "%6s [%8x]  MSG=%15s  W=%8x  L=%4x", 
        pmTitle, pmWindow, myMessage, pmWParam, pmLParam);
} // EdLog_PrintWindowsMessage


/******************************/
/* Static callback procedures */
/******************************/
/******************************/
/* MyLogFontEnumerateAllFonts */
/******************************/
int CALLBACK	MyLogFontEnumerateAllFonts (LPLOGFONT pmLogFont, 
					    LPTEXTMETRIC pmTextMetric, 
					    DWORD pmFontType, LPARAM pmHDC)
{
    char	myFontTypeStr [256];
    int		myFontTypeCharacteristics = 0;
    int		myNumFontsInFamily = 0;

    myFontTypeStr [0] = 0;
    if (pmFontType & DEVICE_FONTTYPE)
    {
        strcat (myFontTypeStr, "DEVICE");
        myFontTypeCharacteristics++;
    }
    if (pmFontType & RASTER_FONTTYPE)
    {
        if (myFontTypeCharacteristics > 0) 
            strcat (myFontTypeStr, ", RASTER");
        else 
            strcat (myFontTypeStr, "RASTER");
        myFontTypeCharacteristics++;
    }
    if (pmFontType & TRUETYPE_FONTTYPE)
    {
        if (myFontTypeCharacteristics > 0) 
            strcat (myFontTypeStr, ", TRUE TYPE");
        else 
            strcat (myFontTypeStr, "TRUE TYPE");
        myFontTypeCharacteristics++;
    }
    EdFile_Write (stLogFile, "Font=\"%s\"  ", pmLogFont->lfFaceName);
    if (myFontTypeCharacteristics > 0) 
        EdFile_Write (stLogFile, "[%s]  ", myFontTypeStr);

    EnumFontFamilies ((HDC) pmHDC, pmLogFont->lfFaceName, 
	(FONTENUMPROC) MyLogFontEnumerateSingleFont, 
	(LPARAM) &myNumFontsInFamily);

    if (myNumFontsInFamily > 0) 
        EdFile_Write (stLogFile, "]");
    EdFile_WriteNL (stLogFile);

    return TRUE;
} // MyLogFontEnumerateAllFonts


/********************************/
/* MyLogFontEnumerateSingleFont */
/********************************/
int CALLBACK	MyLogFontEnumerateSingleFont (LPLOGFONT pmLogFont, 
    					      LPTEXTMETRIC pmTextMetric, 
    					      DWORD pmFontType, 
    					      LPARAM pmNumFontsInFamily)
{

    if (* (int *) pmNumFontsInFamily == 0) 
        EdFile_Write (stLogFile, "[");
    else 
        EdFile_Write (stLogFile, ", ");

    (* (int *) pmNumFontsInFamily)++;

    if (pmFontType & TRUETYPE_FONTTYPE)
    {
	EdFile_Write (stLogFile, "%s", ((LPENUMLOGFONT) pmLogFont)->elfStyle);
    }
    else
    {
	EdFile_Write (stLogFile, "%dx%d", pmLogFont->lfHeight, 
	    pmLogFont->lfWidth);
    }

    return TRUE;
} // MyLogFontEnumerateSingleFont


/*********************/
/* Static procedures */
/*********************/

/*****************/
/* MyOpenLogFile */
/*****************/
static FileEntryPtr	MyOpenLogFile (char *pmFileName, char *pmErrorTitle,
				       char *pmErrorFileName)
{
    int			myErrorCode;
    FilePath		myPathName;
    FileEntryPtr	myFile;
    char		myDateBuffer [256];

    // Create log file path in the actual start up directory
    GetCurrentDirectory (PATHNAME_LENGTH, myPathName);
    if (myPathName [strlen (myPathName) - 1] != '\\')
    {
    	strcat (myPathName, "\\");
    }
    strcat (myPathName, pmFileName);
    	
    // Try opening the log file in the startup directory
    myFile = EdFile_OpenFileForWrite (myPathName, &myErrorCode, ALWAYS_FLUSH,
    				      CREATE_NEW_FILE);
    	
    if (myFile == NULL)
    {
	// If that fails, try and open it up in the user's home directory
	char	*myHomeDrive, *myHomePath;

	myHomeDrive = getenv ("HOMEDRIVE");
	myHomePath = getenv ("HOMEPATH");
	    
	if ((myHomeDrive == NULL) || (myHomePath == NULL))
	{
	    ErrorBuffer	myErrorBuffer;
	    	
	    EdGUI_Message (
	    	pmErrorTitle,
	    	"Unable to open %s \"%s\".\n"
	    	"Unable to open %s in home directory\n"
	    	"because environment variables HOMEDRIVE and\n"
	    	"HOMEPATH are not defined\n\n%s\n[Error code: %d]",
	    	pmErrorFileName, myPathName, pmErrorFileName, 
	    	EdFail_GetErrorMessage (myErrorBuffer), myErrorCode);
	    return NULL;
	}

	// Create the log file path in the user's home directory
	wsprintf (myPathName, "%s%s", myHomeDrive, myHomePath);
	if (myPathName [strlen (myPathName) - 1] != '\\')
	{
	    strcat (myPathName, "\\");
	}
	strcat (myPathName, pmFileName);
		
	// Try opening the log file in the user's home directory
	myFile = EdFile_OpenFileForWrite (myPathName, &myErrorCode, 
	    ALWAYS_FLUSH, CREATE_NEW_FILE);
	if (myFile == NULL)
	{
	    ErrorBuffer	myErrorBuffer;
	    	
	    EdGUI_Message (
	    	pmErrorTitle,
	    	"Unable to open %s \"%s\".\n\n%s\n[Error code: %d]",
	    	pmErrorFileName, myPathName, 
	    	EdFail_GetErrorMessage (myErrorBuffer), myErrorCode);
	    return NULL;
	}
    } // if (myFile == NULL)

    EdGUI_Message ("Information", "Writing %s to \"%s\"", pmErrorFileName,
    		   myPathName);
	
    // Write the time at the head of the log file.
    EdFile_Writeln (myFile, "----- Log begins: %s", 
        EdGUI_GetCurrentDate (myDateBuffer));
        
    return myFile;
} // MyOpenLogFile
