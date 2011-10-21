/************/
/* edprop.c */
/************/

/*******************/
/* System includes */
/*******************/
#include <windows.h>
#include <crtdbg.h>
#include <stddef.h>
#include <stdio.h>
#include <shlobj.h>

/****************/
/* Self include */
/****************/
#include "edprop.h"

/******************/
/* Other includes */
/******************/
#include "ed.h"
#include "edfail.h"
#include "edfile.h"
#include "edglob.h"
#include "edgui.h"
#include "edlog.h"
#include "edprog.h"
#include "edreg.h"
#include "edtext.h"
#include "edwin.h"

#include "eddialogs.h"

/**********/
/* Macros */
/**********/
#define NO_SECTION			0
#define ADMIN_SECTION			1
#define GENERAL_SECTION			2
#define APPEARANCE_SECTION		3
#define PRINTER_SECTION			4
#define RUN_SECTION			5
#define JAVA_SECTION			6
#define TURING_SECTION			7
#define SPECIAL_SECTION			8
#define DEBUG_SECTION			9

#define GENERAL_PAGE			0
#define APPEARANCE_PAGE			1
#define RUN_PAGE			2
#define PRINT_PAGE			3
#define JAVA_PAGE			4
#define ADMIN_PAGE			5
#define MAX_PAGES			6

#define STRING_PROP			1
#define INT_PROP			2
#define BOOL_PROP			3

// Used by MyCanWritePrefs
#define ADMIN_PREFS			TRUE
#define USER_PREFS			FALSE

// Flags for changing prefs
// Admin
#define STARTUPDIR_CHANGED		0x0001
#define CANCHANGEUSERNAME_CHANGED	0x0002
#define CANENTERADVANCED_CHANGED	0x0004
#define DEBUGGERAVAIL_CHANGED		0x0008
#define ONLYSTARTUPDIR_CHANGED		0x0010
#define ALLOWEDDRIVES_CHANGED		0x0020
#define ALLOWEDDRIVESLIST_CHANGED	0x0040
#define FORBIDDENDRIVES_CHANGED		0x0080
#define FORBIDDENDRIVESLIST_CHANGED	0x0100
#define CANTCHANGEPREF_CHANGED		0x0200
#define USEMYDOCUMENTS_CHANGED		0x0400
#define PROHIBITSYSEXEC_CHANGED		0x0800
#define NOSOUND_CHANGED			0x1000

// General
#define BEGINNER_CHANGED		0x0001
#define ONEWINDOW_CHANGED		0x0002
#define USEFULLPATH_CHANGED		0x0004
#define SAVE_BACKUP_CHANGED		0x0008
#define INDENTSIZE_CHANGED		0x0010
#define CONFIRMQUIT_CHANGED		0x0020
#define SKIPSPLASH_CHANGED		0x0040
#define ADDSUFFIX_CHANGED		0x0080
#define SPACESTOTABS_CHANGED		0x0100
#define OPENBRACESAMELINE_CHANGED	0x0200
#define SPACESPERTAB_CHANGED		0x0400
#define NORECENTFILES_CHANGED		0x0800
#define USELASTACTIVEDIRECTORY_CHANGED	0x1000
#define FINDUSESSELECTION_CHANGED	0x2000

// Appearances
#define FONT_CHANGED			0x001
#define SYNTAXCOLOUR_CHANGED		0x002
#define BEGINFULLSCREEN_CHANGED		0x004
#define ADVFULLSCREEN_CHANGED		0x008
#define EDITTEXTROWS_CHANGED		0x010
#define EDITTEXTCOLS_CHANGED		0x020
#define CARETWIDTH_CHANGED		0x040
#define DLGFONTSIZE_CHANGED		0x080
#define BUTTONSHORTCUT_CHANGED		0x100
#define SHOWDEBUGGERMENU_CHANGED	0x200
#define SHOWWINDOWMENU_CHANGED		0x400

// Printing
#define BOLDKEYWORDS_CHANGED		0x0002
#define ITALIDENT_CHANGED		0x0004
#define USERNAME_CHANGED		0x0008
#define PRINTHEADER_CHANGED		0x0010
#define PRINTUSERNAME_CHANGED		0x0020
#define PRINTLINENUMBER_CHANGED		0x0040
#define PRINTTWOUP_CHANGED		0x0080
#define PRINTBORDER_CHANGED		0x0100
#define PRINTMARGIN_CHANGED		0x0200
#define PRINTHDRFONT_CHANGED		0x0400

// Run Window
#define RUNPLAINCONSOLE_CHANGED		0x0002
#define RUNGRAPHICSMODE_CHANGED		0x0004
#define RUNFULLSCREEN_CHANGED		0x0008
#define RUNTEXTROWS_CHANGED		0x0010
#define RUNTEXTCOLS_CHANGED		0x0020
#define RUNAPPLETXSIZE_CHANGED		0x0040
#define RUNAPPLETYSIZE_CHANGED		0x0080
#define RUNUSESMALLFONT_CHANGED		0x0100

// Java
#define JAVAADDCLASSPATH_CHANGED	0x0001
#define JAVAJIKESOPTION_CHANGED		0x0002
#define JAVAJVMOPTION_CHANGED		0x0004
#define JAVAAPPLVIEWOPTION_CHANGED	0x0008
#define JAVAUSENEWJVM_CHANGED		0x0100
#define JAVAJVMSIZE_CHANGED		0x0200
#define JAVA13COMPILE_CHANGED		0x0400
#define JAVA13JVM_CHANGED		0x0800

/*************/
/* Constants */
/*************/

/********************/
/* Global variables */
/********************/

/*********/
/* Types */
/*********/

typedef struct
{
    int		section;
    char	*name;
    int		kind;
    char	*defaultStringValue;
    int		defaultIntValue;
    int		offset;
    int		dialogPart;
    int		changeBit;
} PropertyType;

// Used by the prefs dialog box
typedef struct
{
    int			editFontSizeInPixels;	
    int			printFontSizeInPixels;
    unsigned int	adminPrefChanges;
    unsigned int	appearPrefChanges;
    unsigned int	generalPrefChanges;
    unsigned int	javaPrefChanges;
    unsigned int	printPrefChanges;
    unsigned int	runPrefChanges;
    BOOL		isAdmin;
    BOOL		canWritePrefs;
    BOOL		isDialogCentred;
    BOOL		changesMade;
} PropertyPageInfo;
    
/********************/
/* Static constants */
/********************/
static char	stGeneralPrefDialogName[] = "GeneralPrefDialog";
static char	stAppearancePrefDialogName[] = "AppearancePrefDialog";
static char	stRunPrefDialogName[] = "RunPrefDialog";
static char	stJavaPrefDialogName[] = "JavaPrefDialog";
static char	stPrintingPrefDialogName[] = "PrintPrefDialog";
static char	stAdminPrefDialogName[] = "AdminPrefDialog";

static char	stCantOpenExplanation[] = "This is a Java bytecode file and contains binary data that is not editable by the user. You should open the \".java\" file instead.";

/***********************************************************************/
/* Note, when changing the properties structure, you need to change    */
/* the Properties structure in edprop.h and the properties array below */
/***********************************************************************/
//static Properties	stProperties;
static PropertyType	stPropertiesInitializer[] =
{
    {ADMIN_SECTION, "startupDirectory", STRING_PROP, "", 0,
        offsetof (Properties, startupDirectory), 
        PREF_STARTUPDIR, STARTUPDIR_CHANGED},
    {ADMIN_SECTION, "useMyDocuments", BOOL_PROP, NULL, FALSE,
        offsetof (Properties, useMyDocuments),
        PREF_USEMYDOCUMENTS, USEMYDOCUMENTS_CHANGED},
    {ADMIN_SECTION, "studentsCanChangeUserName", BOOL_PROP, "", TRUE,
        offsetof (Properties, studentsCanChangeUserName),
        PREF_CHANGEUSERNAME, CANCHANGEUSERNAME_CHANGED},
    {ADMIN_SECTION, "studentCanEnterAdvancedMode", BOOL_PROP, "", TRUE,
        offsetof (Properties, studentCanEnterAdvancedMode),
        PREF_ENTERADVANCED, CANENTERADVANCED_CHANGED},
    {ADMIN_SECTION, "debuggerAvailable", BOOL_PROP, NULL, 
#ifdef TURING    
		    TRUE,
#else // #ifdef TURING
		    FALSE,
#endif // #else - #ifdef TURING
        offsetof (Properties, debuggerAvailable), 
        PREF_DEBUGGERAVAIL, DEBUGGERAVAIL_CHANGED},
    {ADMIN_SECTION, "studentRestrictedToStartupDir", BOOL_PROP, "", FALSE,
        offsetof (Properties, studentRestrictedToStartupDir),
        PREF_ONLYSTARTUPDIR, ONLYSTARTUPDIR_CHANGED},        
    {ADMIN_SECTION, "studentDrivesAllowed", BOOL_PROP, "", FALSE,
        offsetof (Properties, studentDrivesAllowed),
        PREF_ALLOWEDDRIVES, ALLOWEDDRIVES_CHANGED},
    {ADMIN_SECTION, "studentDrivesAllowedList", STRING_PROP, 
        "*ABCDEFGHIJKLMNOPQRSTUVWXYZ", 0,
        offsetof (Properties, studentDrivesAllowedList),
        PREF_ALLOWEDDRIVESLIST, ALLOWEDDRIVESLIST_CHANGED},
    {ADMIN_SECTION, "studentDrivesForbidden", BOOL_PROP, "", FALSE,
        offsetof (Properties, studentDrivesForbidden),
        PREF_FORBIDDENDRIVES, FORBIDDENDRIVES_CHANGED},
    {ADMIN_SECTION, "studentDrivesForbiddenList", STRING_PROP, 
        "*ABCDEFGHIJKLMNOPQRSTUVWXYZ", 0,
        offsetof (Properties, studentDrivesForbiddenList),
        PREF_FORBIDDENDRIVESLIST, FORBIDDENDRIVESLIST_CHANGED},
    {ADMIN_SECTION, "studentCantChangePrefs", BOOL_PROP, "", FALSE,
        offsetof (Properties, studentCantChangePrefs),
        PREF_CANTCHANGEPREF, CANTCHANGEPREF_CHANGED},
    {ADMIN_SECTION, "prohibitSysExec", BOOL_PROP, "", FALSE,
        offsetof (Properties, prohibitSysExec),
        PREF_PROHIBITSYSEXEC, PROHIBITSYSEXEC_CHANGED},
    {ADMIN_SECTION, "noSound", BOOL_PROP, "", FALSE,
        offsetof (Properties, noSound),
        PREF_NOSOUND, NOSOUND_CHANGED},

    {GENERAL_SECTION, "beginnerMode", BOOL_PROP, NULL, FALSE,
        offsetof (Properties, beginnerMode),
        PREF_BEGINNER, BEGINNER_CHANGED},
    {GENERAL_SECTION, "oneWindowMode", BOOL_PROP, NULL, FALSE,
        offsetof (Properties, oneWindowMode),
        PREF_ONEWINDOW, ONEWINDOW_CHANGED},
    {GENERAL_SECTION, "fullPathNames", BOOL_PROP, NULL, FALSE,
        offsetof (Properties, useFullPathNames),
        PREF_EDITFULLPATH, USEFULLPATH_CHANGED},
    {GENERAL_SECTION, "editorSaveBackup", BOOL_PROP, NULL, FALSE,
        offsetof (Properties, editSaveBackup),
        PREF_EDITSAVEBACKUP, SAVE_BACKUP_CHANGED},
    {GENERAL_SECTION, "editorIndentSize", INT_PROP, NULL, 4,
        offsetof (Properties, editIndentSize),
        PREF_INDENTSIZE, INDENTSIZE_CHANGED},
    {GENERAL_SECTION, "confirmQuit", BOOL_PROP, NULL, TRUE,
        offsetof (Properties, confirmQuit),
        PREF_CONFIRMQUIT, CONFIRMQUIT_CHANGED},
    {GENERAL_SECTION, "skipSplashScreen", BOOL_PROP, NULL, FALSE,
        offsetof (Properties, skipSplashScreen),
        PREF_SKIPSPLASH, SKIPSPLASH_CHANGED},
    {GENERAL_SECTION, "addSuffixToFilesWithNone", BOOL_PROP, NULL, TRUE,
        offsetof (Properties, addSuffixToFilesWithNone),
        PREF_ADDSUFFIX, ADDSUFFIX_CHANGED},
    {GENERAL_SECTION, "editorSpacesConvertedToTabs", BOOL_PROP, NULL, TRUE,
        offsetof (Properties, editLeadingSpacesToTabs),
        PREF_EDITSPACESTOTABS, SPACESTOTABS_CHANGED},
    {GENERAL_SECTION, "editorOpenBraceOnSameLine", BOOL_PROP, NULL, FALSE,
        offsetof (Properties, editOpenBraceOnSameLine),
        PREF_OPENBRACESAMELINE, OPENBRACESAMELINE_CHANGED},
    {GENERAL_SECTION, "editorNumSpacesPerTab", INT_PROP, NULL, 8,
        offsetof (Properties, editNumSpacesPerTab),
        PREF_EDITSPACESPERTAB, SPACESPERTAB_CHANGED},
    {GENERAL_SECTION, "noRecentFiles", BOOL_PROP, NULL, FALSE,
        offsetof (Properties, noRecentFiles),
        PREF_NORECENTFILES, NORECENTFILES_CHANGED},
    {GENERAL_SECTION, "useLastDirectory", BOOL_PROP, NULL, FALSE,
        offsetof (Properties, useLastDirectory),
        PREF_USELASTACTIVEDIRECTORY, USELASTACTIVEDIRECTORY_CHANGED},
    {GENERAL_SECTION, "findUsesSelection", BOOL_PROP, NULL, TRUE,
        offsetof (Properties, findUsesSelection),
        PREF_FINDUSESSELECTION, FINDUSESSELECTION_CHANGED},
    {GENERAL_SECTION, "displayDebuggerMenu", BOOL_PROP, NULL, FALSE,
        offsetof (Properties, displayDebuggerMenu),
        0, 0},
    {GENERAL_SECTION, "displayWindowMenu", BOOL_PROP, NULL, FALSE,
        offsetof (Properties, displayWindowMenu),
        0, 0},
        
    {APPEARANCE_SECTION, "editorFontName", STRING_PROP, "Courier New", 0, 
        offsetof (Properties, editFontName),
        PREF_EDITFONTNAME, FONT_CHANGED},
    {APPEARANCE_SECTION, "editorFontSize", INT_PROP, NULL, 10,
        offsetof (Properties, editFontSize),
        PREF_EDITFONTSIZE, FONT_CHANGED},
    {APPEARANCE_SECTION, "editorSyntaxColouring", BOOL_PROP, NULL, TRUE,
        offsetof (Properties, editSyntaxColouring),
        PREF_EDITCOLOUR, SYNTAXCOLOUR_CHANGED},
    {APPEARANCE_SECTION, "editorBeginnerFullScreen", BOOL_PROP, NULL, TRUE,
        offsetof (Properties, editBeginnerFullScreen),
        PREF_BEGINFULLSCREEN, BEGINFULLSCREEN_CHANGED},
    {APPEARANCE_SECTION, "editorAdvancedFullScreen", BOOL_PROP, NULL, FALSE,
        offsetof (Properties, editAdvancedFullScreen),
        PREF_ADVFULLSCREEN, ADVFULLSCREEN_CHANGED},
    {APPEARANCE_SECTION, "editorTextRows", INT_PROP, NULL, 25,
        offsetof (Properties, editTextRows),
        PREF_EDITTEXTROWS, EDITTEXTROWS_CHANGED},
    {APPEARANCE_SECTION, "editorTextCols", INT_PROP, NULL, 80,
        offsetof (Properties, editTextCols),
        PREF_EDITTEXTCOLS, EDITTEXTCOLS_CHANGED},
    {APPEARANCE_SECTION, "editorCaretWidth", INT_PROP, NULL, 2,
        offsetof (Properties, editCaretWidth),
        PREF_CARETWIDTH, CARETWIDTH_CHANGED},
    {APPEARANCE_SECTION, "debuggerMenu", BOOL_PROP, NULL, FALSE,
        offsetof (Properties, debuggerMenu),
        PREF_SHOWDEBUGGERMENU, SHOWDEBUGGERMENU_CHANGED},
    {APPEARANCE_SECTION, "windowMenu", BOOL_PROP, NULL, FALSE,
        offsetof (Properties, windowMenu),
        PREF_SHOWWINDOWMENU, SHOWWINDOWMENU_CHANGED},
            

    {RUN_SECTION, "useGraphicsMode", BOOL_PROP, NULL, TRUE,
        offsetof (Properties, useGraphicsMode),
        PREF_RUNGRAPHICSMODE, RUNGRAPHICSMODE_CHANGED},
    {RUN_SECTION, "runConsoleFontName", STRING_PROP, "Courier New", TRUE,
        offsetof (Properties, runConsoleFontName),
        PREF_RUNFONTNAME, FONT_CHANGED},
    {RUN_SECTION, "runConsoleFontSize", INT_PROP, NULL, 10,
        offsetof (Properties, runConsoleFontSize),
        PREF_RUNFONTSIZE, FONT_CHANGED},
    {RUN_SECTION, "runConsoleFullScreen", BOOL_PROP, NULL, FALSE,
        offsetof (Properties, runConsoleFullScreen),
        PREF_RUNFULLSCREEN, RUNFULLSCREEN_CHANGED},
    {RUN_SECTION, "runConsoleTextRows", INT_PROP, NULL, 25,
        offsetof (Properties, runConsoleTextRows),
        PREF_RUNTEXTROWS, RUNTEXTROWS_CHANGED},
    {RUN_SECTION, "runConsoleTextCols", INT_PROP, NULL, 80,
        offsetof (Properties, runConsoleTextCols),
        PREF_RUNTEXTCOLS, RUNTEXTCOLS_CHANGED},
    {RUN_SECTION, "runAppletWidth", INT_PROP, NULL, 300,
        offsetof (Properties, runAppletWidth),
        PREF_RUNAPPLETXSIZE, RUNAPPLETXSIZE_CHANGED},
    {RUN_SECTION, "runAppletHeight", INT_PROP, NULL, 200,
        offsetof (Properties, runAppletHeight),
        PREF_RUNAPPLETYSIZE, RUNAPPLETYSIZE_CHANGED},
    {RUN_SECTION, "runUseSmallFont", BOOL_PROP, NULL, TRUE,
        offsetof (Properties, runUseSmallFont),
        PREF_RUNUSESMALLFONT, RUNUSESMALLFONT_CHANGED},

    {JAVA_SECTION, "additionalClassPath", STRING_PROP, "", TRUE,
        offsetof (Properties, additionalClassPath),
        PREF_JAVAADDCLASSPATH, JAVAADDCLASSPATH_CHANGED},
    {JAVA_SECTION, "jikesOptions", STRING_PROP, "", TRUE,
        offsetof (Properties, jikesOptions),
        PREF_JAVAJIKESOPTION, JAVAJIKESOPTION_CHANGED},
    {JAVA_SECTION, "memoryForJVMInMB", INT_PROP, NULL, 0,
        offsetof (Properties, memoryForJVMInMB),
        PREF_JAVAJVMSIZE, JAVAJVMSIZE_CHANGED},
    {JAVA_SECTION, "oldJavaCompile", BOOL_PROP, NULL, TRUE,
        offsetof (Properties, oldJavaCompile),
        PREF_JAVA13COMPILE, JAVA13COMPILE_CHANGED},
    {JAVA_SECTION, "JVMType", INT_PROP, NULL, JRE_BUILT_IN,
        offsetof (Properties, JVMType),
        0, 0},
    {JAVA_SECTION, "startJavaRunnerInDebugger", BOOL_PROP, NULL, FALSE,
        offsetof (Properties, startJavaRunnerInDebugger),
        0, 0},
    {JAVA_SECTION, "versionInRegistry", STRING_PROP, "", TRUE,
        offsetof (Properties, versionInRegistry),
        0, 0},

    {TURING_SECTION, "turingStackSizeInKB", INT_PROP, NULL, 0,
        offsetof (Properties, turingStackSizeInKB),
        0, 0},
    {TURING_SECTION, "executionDelay", INT_PROP, NULL, 0,
        offsetof (Properties, executionDelay),
        0, 0},
    {TURING_SECTION, "parallelIOPort", INT_PROP, NULL, 0,
        offsetof (Properties, parallelIOPort),
        0, 0},

    {PRINTER_SECTION, "printerFontName", STRING_PROP, "Courier New", 0, 
        offsetof (Properties, printFontName),
        PREF_PRINTFONTNAME, FONT_CHANGED},
    {PRINTER_SECTION, "userName", STRING_PROP, "", 0, 
        offsetof (Properties, userName),
        PREF_PRINTUSERNAME, USERNAME_CHANGED},
    {PRINTER_SECTION, "printerFontSize", INT_PROP, NULL, 10,
        offsetof (Properties, printFontSize),
        PREF_PRINTFONTSIZE, FONT_CHANGED},
    {PRINTER_SECTION, "printerBoldKeywords", BOOL_PROP, NULL, TRUE,
        offsetof (Properties, printBoldKeywords),
        PREF_PRINTBOLD, BOLDKEYWORDS_CHANGED},
    {PRINTER_SECTION, "printerItalicizeIdentifiers", BOOL_PROP, NULL, TRUE,
        offsetof (Properties, printItalicizeIdents),
        PREF_PRINTITAL, ITALIDENT_CHANGED},
    {PRINTER_SECTION, "printerPageHeader", BOOL_PROP, NULL, TRUE,
        offsetof (Properties, printPageHeader),
        PREF_PRINTHEADER, PRINTHEADER_CHANGED},
    {PRINTER_SECTION, "printerUserName", BOOL_PROP, NULL, TRUE,
        offsetof (Properties, printUserName),
        PRINT_PRINTPRINTUSERNAME, PRINTUSERNAME_CHANGED},
    {PRINTER_SECTION, "printerLineNumbers", BOOL_PROP, NULL, FALSE,
        offsetof (Properties, printLineNumbers),
        PREF_PRINTLINENUMBER, PRINTLINENUMBER_CHANGED},
    {PRINTER_SECTION, "printerTwoUp", BOOL_PROP, NULL, FALSE,
        offsetof (Properties, printTwoUp),
        PREF_PRINTTWOUP, PRINTTWOUP_CHANGED},
    {PRINTER_SECTION, "printerBorder", BOOL_PROP, NULL, FALSE,
        offsetof (Properties, printBorder),
        PREF_PRINTBORDER, PRINTBORDER_CHANGED},
    {PRINTER_SECTION, "printerLeftMargin", INT_PROP, NULL, 500,
        offsetof (Properties, printMarginLeft),
        PREF_PRINTLEFTMARG, PRINTMARGIN_CHANGED},
    {PRINTER_SECTION, "printerRightMargin", INT_PROP, NULL, 500,
        offsetof (Properties, printMarginRight),
        PREF_PRINTRIGHTMARG, PRINTMARGIN_CHANGED},
    {PRINTER_SECTION, "printerTopMargin", INT_PROP, NULL, 500,
        offsetof (Properties, printMarginTop),
        PREF_PRINTTOPMARG, PRINTMARGIN_CHANGED},
    {PRINTER_SECTION, "printerBottomMargin", INT_PROP, NULL, 500,
        offsetof (Properties, printMarginBottom),
        PREF_PRINTBOTTOMMARG, PRINTMARGIN_CHANGED},
    {PRINTER_SECTION, "printerHeaderFontName", STRING_PROP, "Courier New", 0, 
        offsetof (Properties, printHeaderFontName),
        PREF_PRINTHDRFONTNAME, PRINTHDRFONT_CHANGED},
    {PRINTER_SECTION, "printerHeaderFontSize", INT_PROP, NULL, 10,
        offsetof (Properties, printHeaderFontSize),
        PREF_PRINTHDRFONTSIZE, PRINTHDRFONT_CHANGED},

    {SPECIAL_SECTION, "editorOneWindowMargin", INT_PROP, NULL, 0,
        offsetof (Properties, editOneWindowMargin),
        0, 0},
    {SPECIAL_SECTION, "numberRecentFiles", INT_PROP, NULL, 5,
        offsetof (Properties, numRecentFiles), 
        0, 0},
    {SPECIAL_SECTION, "catchExceptions", BOOL_PROP, NULL, TRUE,
        offsetof (Properties, catchExceptions), 
        0, 0},

    {DEBUG_SECTION, "debug", BOOL_PROP, NULL, FALSE,  // TW FALSE when finished
        offsetof (Properties, debug),
        0, 0},
    {DEBUG_SECTION, "debugWindowAreas", BOOL_PROP, NULL, FALSE,
        offsetof (Properties, debugWindowAreas),
        0, 0},
    {DEBUG_SECTION, "logging", BOOL_PROP, NULL, FALSE,
        offsetof (Properties, logWindowMessages),
        0, 0},
    {DEBUG_SECTION, "logWindowMessages", BOOL_PROP, NULL, FALSE,
        offsetof (Properties, logWindowMessages),
        0, 0},
    {DEBUG_SECTION, "stream0ToStderr", BOOL_PROP, NULL, FALSE,
        offsetof (Properties, stream0ToStderr),
        0, 0},
    {DEBUG_SECTION, "errorsToStderr", BOOL_PROP, NULL, FALSE,
        offsetof (Properties, errorsToStderr),
        0, 0},
    {DEBUG_SECTION, "infoToStderr", BOOL_PROP, NULL, FALSE,
        offsetof (Properties, infoToStderr),
        0, 0}
};
static int stNumProperties = sizeof (stPropertiesInitializer) / 
							sizeof (PropertyType);

static char	*stSectionNames [] = {"", "[Admin]", "[General]", "[Editor]", 
				      "[Printing]", "[Run]", "[Java]", 
				      "[Turing]", "[Special]", "[Debug]"};
      
/********************/
/* Static variables */
/********************/
static Properties	stDefaultProperties;
static Properties	stAdminProperties;
static Properties	stUserProperties;
static BOOL		stIsAdmin;
static BOOL		stCantWriteWarningGiven;
static FilePath		stApplicationDirectory;
static FilePath		stStartupDirectory;
    
// Flag to indicate whether all possible prefs should be written
static BOOL		stWriteAllPrefs = FALSE;

// Debugging flag
static BOOL		stDebugPrefs = FALSE;

/******************************/
/* Static callback procedures */
/******************************/
static BOOL CALLBACK MyAdminPrefDialogProcedure (HWND pmDialog, UINT pmMessage, 
    WPARAM pmWParam, LPARAM pmLParam);
static BOOL CALLBACK MyAppearancePrefDialogProcedure (HWND pmDialog, 
    UINT pmMessage, WPARAM pmWParam, LPARAM pmLParam);
static BOOL CALLBACK MyGeneralPrefDialogProcedure (HWND pmDialog, 
    UINT pmMessage, WPARAM pmWParam, LPARAM pmLParam);
static BOOL CALLBACK MyJavaPrefDialogProcedure (HWND pmDialog, 
    UINT pmMessage, WPARAM pmWParam, LPARAM pmLParam);
static BOOL CALLBACK MyPrintingPrefDialogProcedure (HWND pmDialog, 
    UINT pmMessage, WPARAM pmWParam, LPARAM pmLParam);
static BOOL CALLBACK MyRunPrefDialogProcedure (HWND pmDialog, 
    UINT pmMessage, WPARAM pmWParam, LPARAM pmLParam);
    
/*********************/
/* Static procedures */
/*********************/
static void	MyAdjustChangeFlag (HWND pmDialog, int pmDialogPart, 
				    unsigned int *pmFlag);
static BOOL	MyCanWritePrefs (BOOL pmAdminPrefs, int *myErrorCode);
static BOOL	MyCheckedToBool (HWND pmDialog, int pmCheckBox);
static void	MyClearJavaFileAssoc (HWND pmDialog);
static void	MyConvertToAppProperties (Ed_Properties *myAppProperties);
static BOOL	MyDialogToMargin (HWND pmDialog, int pmDialogPart, 
				  int *pmValue);
static void	MyDialogToProperty (HWND pmDialog, int pmDialogPart);
static void	MyDisplayFileAssoc (HWND pmDialog);
static void	MyMarginToDialog (HWND pmDialog, int pmDialogPart, int pmValue);
static void	MyPropertyToDialog (HWND pmDialog, int pmDialogPart);
static void	MyReadPropertiesFromDirectory (Properties *pmProp, 
	    				       Properties *pmBaseProp, 
    					       int pmDirectoryResource,
    					       int pmFileNameResource);
static void	MySetCheckBox (HWND pmDialog, int pmCheckBox, BOOL pmValue);
static void	MySetDefaultProperties (Properties *pmProp);
static void	MySetStartupDir (Properties *pmProp);


/***********************/
/* External procedures */
/***********************/

/**********************************************************************/
/* EdProp_Init 							      */
/*								      */
/* Read from the INI file located in the installation directory, then */
/* modify it with the INI file located in the startup directory       */
/**********************************************************************/
BOOL	EdProp_Init (BOOL pmAdmin, BOOL pmLogging)
{
    Ed_Properties	myAppProperties;
    
    // TW Bring fails into line.

    // We haven't warned the user about not being able to write        
    stCantWriteWarningGiven = FALSE;

    // Read the properties file from the application directory
    EdFile_GetApplicationDirectory (stApplicationDirectory);
    
    // Make certain that if we are asking for admin privs, we can actually
    // write to the admin directory.  If we can't, then notify the user and
    // turn admin privs off.
    if (pmAdmin)
    {
    	FilePath	myPrefPathName;
    	ErrorBuffer	myErrorBuffer;
    	int		myErrorCode;

    	EdFile_ConstructPath (myPrefPathName, IDS_INI_DIR, ADMIN_PREF);
    	
    	if (MyCanWritePrefs (ADMIN_PREFS, &myErrorCode))
    	{
    	    EdGUI_Message1 (NULL, 0, IDS_ADMIN_TITLE, IDS_ADMIN_MESSAGE,
    	    	myPrefPathName);
    	    stIsAdmin = TRUE;
    	}
    	else
    	{
    	    EdGUI_Message1 (NULL, 0, IDS_ADMIN_TITLE, IDS_ADMIN_FAIL_MESSAGE,
    	    	myPrefPathName, 
    	    	EdFail_GetErrorMessage1 (myErrorBuffer, myErrorCode), 
    	    	myErrorCode);
    	    stIsAdmin = FALSE;
    	}
    }
    else
    {
    	stIsAdmin = FALSE;
    }
        
    // Set the default properties
    MySetDefaultProperties (&stDefaultProperties);

    MyReadPropertiesFromDirectory (&stAdminProperties, &stDefaultProperties,
            IDS_INI_DIR, ADMIN_PREF);

    // Set the startup directory, if necessary
    MySetStartupDir (&stAdminProperties);
    
    // Read the properties file from the current directory
    EdFile_GetCurrentDirectory (stStartupDirectory);
    
    if (stIsAdmin)
    {
    	gProperties = stAdminProperties;
    }
    else
    {
    	if (stAdminProperties.useMyDocuments)
    	{
    	    MyReadPropertiesFromDirectory (&stUserProperties, 
    	        &stAdminProperties, FROM_APPDATA, USER_PREF);
	}
	else
	{
    	    MyReadPropertiesFromDirectory (&stUserProperties, 
    	        &stAdminProperties, FROM_STARTUP, USER_PREF);
    	}
    	gProperties = stUserProperties;
    }

    // If logging was set with a command line option, set it on now
    if (pmLogging)
    {
    	gProperties.logging = TRUE;
    }

    // Note, Windows 95 cannot handle anything after Java 1.3.1
    if (gProgram.operatingSystem == WIN_95)
    {
    	gProperties.JVMType = JRE_131;
    }
    // Note if running JVM 1.3.1, "assert" disabled
    if (gProperties.JVMType == JRE_131)
    {
    	gProperties.oldJavaCompile = TRUE;
    }
    
    // Set the properties
    MyConvertToAppProperties (&myAppProperties);
    Ed_PropertiesSet (myAppProperties);
                
    return TRUE;
} // EdProp_Init


/************************************************************************/
/* EdProp_ChangePreferences						*/
/************************************************************************/
void	EdProp_ChangePreferences (HWND pmWindow)
{
    HINSTANCE		myApplicationInstance;
    PROPSHEETPAGE	myPropSheetPage [MAX_PAGES];
    PROPSHEETHEADER	myPropSheetHeader;
    HDC			myDisplayContext;
    PropertyPageInfo	myPropertyPageInfo;
    int			myAdminPage;
    int			myDummy;
    
    // If the user has ctrl+shift pressed at the same time as selecting the
    // preferences menu, then present some debugging information.
    stDebugPrefs = EdGUI_CtrlShiftKeysPressed ();
    
    // Return with message if not allowed to change preferences
    if (gProperties.studentCantChangePrefs && !stIsAdmin)
    {
    	EdGUI_Message1 (pmWindow, 0, IDS_WARNING_TITLE, IDS_CANT_CHANGE_PREFS);
    	EdWin_ShowStatus (pmWindow, "Preferences Unchanged");
    	return;
    }
    
    // Get the program instance
    myApplicationInstance = 
        (HINSTANCE) GetWindowLong (pmWindow, GWL_HINSTANCE);
        
    myDisplayContext = GetDC (pmWindow);
    myPropertyPageInfo.editFontSizeInPixels = 
        -MulDiv (gProperties.editFontSize, 
    	    GetDeviceCaps (myDisplayContext, LOGPIXELSY), 72);
    myPropertyPageInfo.printFontSizeInPixels = 
        -MulDiv (gProperties.printFontSize, 
    	    GetDeviceCaps (myDisplayContext, LOGPIXELSY), 72);
    ReleaseDC (pmWindow, myDisplayContext);
    myPropertyPageInfo.adminPrefChanges = 0;
    myPropertyPageInfo.appearPrefChanges = 0;
    myPropertyPageInfo.generalPrefChanges = 0;
    myPropertyPageInfo.javaPrefChanges = 0;
    myPropertyPageInfo.printPrefChanges = 0;
    myPropertyPageInfo.runPrefChanges = 0;
    myPropertyPageInfo.isAdmin = stIsAdmin;
    myPropertyPageInfo.canWritePrefs = MyCanWritePrefs (stIsAdmin, &myDummy);
    myPropertyPageInfo.isDialogCentred = FALSE;
    myPropertyPageInfo.changesMade = FALSE;
    
    if (!myPropertyPageInfo.canWritePrefs && !stCantWriteWarningGiven)
    {
    	EdGUI_Message1 (pmWindow, 0, IDS_WARNING_TITLE, IDS_NO_WRITE_PREFS);
    	stCantWriteWarningGiven = TRUE;
    }
    	
    ZeroMemory (myPropSheetPage, MAX_PAGES * sizeof (PROPSHEETPAGE));
    
    myPropSheetPage [GENERAL_PAGE].dwSize = sizeof (PROPSHEETPAGE);
    myPropSheetPage [GENERAL_PAGE].dwFlags = PSP_USETITLE;
    myPropSheetPage [GENERAL_PAGE].hInstance = myApplicationInstance;
    myPropSheetPage [GENERAL_PAGE].pszTemplate = stGeneralPrefDialogName;
    myPropSheetPage [GENERAL_PAGE].pszIcon = NULL;
    myPropSheetPage [GENERAL_PAGE].pfnDlgProc = MyGeneralPrefDialogProcedure;
    myPropSheetPage [GENERAL_PAGE].pszTitle = "General";
    myPropSheetPage [GENERAL_PAGE].lParam = (LPARAM) &myPropertyPageInfo;

    myPropSheetPage [APPEARANCE_PAGE].dwSize = sizeof (PROPSHEETPAGE);
    myPropSheetPage [APPEARANCE_PAGE].dwFlags = PSP_USETITLE;
    myPropSheetPage [APPEARANCE_PAGE].hInstance = myApplicationInstance;
    myPropSheetPage [APPEARANCE_PAGE].pszTemplate = stAppearancePrefDialogName;
    myPropSheetPage [APPEARANCE_PAGE].pszIcon = NULL;
    myPropSheetPage [APPEARANCE_PAGE].pfnDlgProc = 
    						MyAppearancePrefDialogProcedure;
    myPropSheetPage [APPEARANCE_PAGE].pszTitle = "Editor Window";
    myPropSheetPage [APPEARANCE_PAGE].lParam = (LPARAM) &myPropertyPageInfo;

    myPropSheetPage [RUN_PAGE].dwSize = sizeof (PROPSHEETPAGE);
    myPropSheetPage [RUN_PAGE].dwFlags = PSP_USETITLE;
    myPropSheetPage [RUN_PAGE].hInstance = myApplicationInstance;
    myPropSheetPage [RUN_PAGE].pszTemplate = stRunPrefDialogName;
    myPropSheetPage [RUN_PAGE].pszIcon = NULL;
    myPropSheetPage [RUN_PAGE].pfnDlgProc = MyRunPrefDialogProcedure;
    myPropSheetPage [RUN_PAGE].pszTitle = "Run Window";
    myPropSheetPage [RUN_PAGE].lParam = (LPARAM) &myPropertyPageInfo;

    myPropSheetPage [PRINT_PAGE].dwSize = sizeof (PROPSHEETPAGE);
    myPropSheetPage [PRINT_PAGE].dwFlags = PSP_USETITLE;
    myPropSheetPage [PRINT_PAGE].hInstance = myApplicationInstance;
    myPropSheetPage [PRINT_PAGE].pszTemplate = stPrintingPrefDialogName;
    myPropSheetPage [PRINT_PAGE].pszIcon = NULL;
    myPropSheetPage [PRINT_PAGE].pfnDlgProc = MyPrintingPrefDialogProcedure;
    myPropSheetPage [PRINT_PAGE].pszTitle = "Printing";
    myPropSheetPage [PRINT_PAGE].lParam = (LPARAM) &myPropertyPageInfo;

    if (gProgram.isJava)
    {
    	myPropSheetPage [JAVA_PAGE].dwSize = sizeof (PROPSHEETPAGE);
    	myPropSheetPage [JAVA_PAGE].dwFlags = PSP_USETITLE;
    	myPropSheetPage [JAVA_PAGE].hInstance = myApplicationInstance;
    	myPropSheetPage [JAVA_PAGE].pszTemplate = stJavaPrefDialogName;
    	myPropSheetPage [JAVA_PAGE].pszIcon = NULL;
    	myPropSheetPage [JAVA_PAGE].pfnDlgProc = MyJavaPrefDialogProcedure;
    	myPropSheetPage [JAVA_PAGE].pszTitle = "Java";
    	myPropSheetPage [JAVA_PAGE].lParam = (LPARAM) &myPropertyPageInfo;
    	
    	myAdminPage = ADMIN_PAGE;
    }
    else
    {
    	myAdminPage = ADMIN_PAGE - 1;
    }

    if (stIsAdmin)
    {
    	myPropSheetPage [myAdminPage].dwSize = sizeof (PROPSHEETPAGE);
    	myPropSheetPage [myAdminPage].dwFlags = PSP_USETITLE;
    	myPropSheetPage [myAdminPage].hInstance = myApplicationInstance;
    	myPropSheetPage [myAdminPage].pszTemplate = stAdminPrefDialogName;
    	myPropSheetPage [myAdminPage].pszIcon = NULL;
    	myPropSheetPage [myAdminPage].pfnDlgProc = MyAdminPrefDialogProcedure;
    	myPropSheetPage [myAdminPage].pszTitle = "Admin";
    	myPropSheetPage [myAdminPage].lParam = (LPARAM) &myPropertyPageInfo;
    }
    
    ZeroMemory (&myPropSheetHeader, sizeof (PROPSHEETHEADER));
    
    myPropSheetHeader.dwSize = sizeof (PROPSHEETHEADER);
    myPropSheetHeader.dwFlags = PSH_PROPSHEETPAGE;
    myPropSheetHeader.hwndParent = pmWindow;
    myPropSheetHeader.hInstance = myApplicationInstance;
    myPropSheetHeader.pszIcon = NULL;
    myPropSheetHeader.pszCaption = (LPSTR) "Preferences";
    if (stIsAdmin)
    {
        myPropSheetHeader.nPages = ADMIN_PAGE + 1;
    }
    else
    {
    	myPropSheetHeader.nPages = JAVA_PAGE + 1;
    }
    
    // No "Java" preference page if not Java
    if (!gProgram.isJava)
    {
    	myPropSheetHeader.nPages--;
    }
    
    myPropSheetHeader.ppsp = (LPCPROPSHEETPAGE) &myPropSheetPage;

    // Set the propery sheet for the dialog box.
    PropertySheet (&myPropSheetHeader);

    if (myPropertyPageInfo.changesMade)
    {    
    	if (EdProp_WritePrefs ())
    	{
    	    EdWin_ShowStatus (pmWindow, "New Preferences Saved");
	}
	else
    	{
    	    EdWin_ShowStatus (pmWindow, "Unable to Save Preferences");
	}
    }
    else
    {
    	EdWin_ShowStatus (pmWindow, "Preferences Unchanged");
    }
} // EdProp_ChangePreferences


/************************************************************************/
/* EdProp_GetApplicationDirectory					*/
/************************************************************************/
const char	*EdProp_GetApplicationDirectory (void)
{
    return stApplicationDirectory;
} // EdProp_GetApplicationDirectory


/************************************************************************/
/* EdProp_GetStartupDirectory						*/
/************************************************************************/
const char	*EdProp_GetStartupDirectory (void)
{
    return stStartupDirectory;
} // EdProp_GetStartupDirectory


/************************************************************************/
/* EdProp_SetWriteAllPrefs						*/
/************************************************************************/
void	EdProp_SetWriteAllPrefs (void)
{
    stWriteAllPrefs = TRUE;
} // EdProp_SetWriteAllPrefs


/************************************************************************/
/* EdProp_WritePrefs							*/
/************************************************************************/
BOOL	EdProp_WritePrefs (void)
{
    Properties		*myProp, *myBaseProp;
    FilePath		myPrefPathName;
    FileEntryPtr	myFile;
    int			myErrorCode;
    int			myCurrentSection = NO_SECTION;
    char		myLineToWrite [1024];
    int			cnt;
    BOOL		myAPreferenceWritten;
    char		*myPtr;
    
    myProp = &gProperties;
    if (stIsAdmin)
    {
        // Write prefs that differ from default to application directory
        myBaseProp = &stDefaultProperties;
    	EdFile_ConstructPath (myPrefPathName, IDS_INI_DIR, ADMIN_PREF);
    }
    else
    {
        // Write prefs that differ from admin to start up directory
        myBaseProp = &stAdminProperties;
        if (gProperties.useMyDocuments)
        {
    	    EdFile_ConstructPath (myPrefPathName, FROM_APPDATA, USER_PREF);
	}
	else
	{
    	    EdFile_ConstructPath (myPrefPathName, FROM_STARTUP, USER_PREF);
    	}
    }
    
    // Try opening the recent files file in the startup directory
    myFile = EdFile_OpenFileForWrite (myPrefPathName, &myErrorCode, 
        BUFFER_OUTPUT, CREATE_NEW_FILE);
    if (myFile == NULL)
    {
    	// Write to log
    	EdLog_Log ("Unable to open pref file (\"%s\") for writing.  "
    		"Error = %d", myPrefPathName, myErrorCode);
    	return FALSE;
    }

    // Write to log
    EdLog_Log ("Writing pref files file to \"%s\"", myPrefPathName);

    // Start with no preferences written        
    myAPreferenceWritten = FALSE;
    
    // Write out anything that differs from the default.  If none do, don't
    // write the file.
    for (cnt = 0 ; cnt < stNumProperties ; cnt++)
    {
    	// By default, don't write anything
    	myLineToWrite [0] = 0;
    	
    	if (stPropertiesInitializer [cnt].kind == STRING_PROP)
    	{
    	    char	*myPtr, *myBasePtr;
    	    
    	    myPtr = ((char *) myProp) + stPropertiesInitializer [cnt].offset;
    	    myBasePtr = ((char *) myBaseProp) + 
    	    	stPropertiesInitializer [cnt].offset;
    	    
    	    if (strcmp (myPtr, myBasePtr) != 0)
    	    {
    	    	wsprintf (myLineToWrite, "%s=%s", 
    	    	    stPropertiesInitializer [cnt].name, myPtr);
    	    } 
    	}
    	else if (stPropertiesInitializer [cnt].kind == INT_PROP)
    	{
    	    int		myValue, myBaseValue;
    	    
	    myValue = * (int *) (((char *) myProp) + 
	    	stPropertiesInitializer [cnt].offset);
	    myBaseValue = * (int *) (((char *) myBaseProp) + 
	        stPropertiesInitializer [cnt].offset);
	        
	    if (myValue != myBaseValue)
	    {
	        // Write out the property
    	    	wsprintf (myLineToWrite, "%s=%d", 
    	    	    stPropertiesInitializer [cnt].name, myValue);
	    }
	}
    	else if (stPropertiesInitializer [cnt].kind == BOOL_PROP)
    	{
    	    int		myValue, myBaseValue;
    	    
	    myValue = * (int *) (((char *) myProp) + 
	        stPropertiesInitializer [cnt].offset);
	    myBaseValue = * (int *) (((char *) myBaseProp) + 
	        stPropertiesInitializer [cnt].offset);

	    if (myValue != myBaseValue)
	    {
	        // Write out the property
	        if (myValue == 0)
	        {
    	    	    wsprintf (myLineToWrite, "%s=false", 
    	    	        stPropertiesInitializer [cnt].name);
		}
		else
	        {
    	    	    wsprintf (myLineToWrite, "%s=true", 
    	    	        stPropertiesInitializer [cnt].name);
		}
	    }
	} // if
	
	if ((myLineToWrite [0] == 0) && stWriteAllPrefs)
    	{
    	    // Write out the just the property name
	    wsprintf (myLineToWrite, "%s=", stPropertiesInitializer [cnt].name);
    	}

	// Check if we have something to write
	if (myLineToWrite [0] != 0)
	{
	    // Indicate that we did write out a preference
	    myAPreferenceWritten = TRUE;
	    
    	    // Write out section header
    	    if (stPropertiesInitializer [cnt].section != myCurrentSection)
    	    {
    	    	if (myCurrentSection == NO_SECTION)
    	    	{
		    char	myDateBuffer [256];
		    
		    // Write the time at the head of the log file.
		    EdFile_Writeln (myFile, 
		        "; %s Properties File.  Created: %s", 
		        gProgram.environmentName,
		        EdGUI_GetCurrentDate (myDateBuffer));
    	    	}
    	    	myCurrentSection = stPropertiesInitializer [cnt].section;
    	    	EdFile_WriteNL (myFile);
    	    	EdFile_Writeln (myFile, stSectionNames [myCurrentSection]);
    	    }

    	    // Write out the just the property name
    	    // Exception, we first have to convert any '%' to '%%%'
    	    myPtr = myLineToWrite;
    	    while (strchr (myPtr, '%') != NULL)
    	    {
    	    	myPtr = strchr (myPtr, '%');
    	    	memmove (myPtr + 3, myPtr, strlen (myPtr) + 1);
    	    	*myPtr++ = '%';
    	    	*myPtr++ = '%';
    	    	*myPtr++ = '%';
    	    	myPtr++;
    	    }
    	    EdFile_Writeln (myFile, myLineToWrite);
	} // if    	    	
    } // for
    
    EdFile_CloseFile (myFile);
    
    if (!myAPreferenceWritten)
    {
    	// No preferences were actually written (the user hasn't changed 
    	// from the default settings), so delete the file after all
    	if (!DeleteFile (myPrefPathName))
    	{
	    // Write to log
	    EdLog_Log ("Unable to delete empty pref file.  Error = %d", 
	    	       GetLastError ());
    	}
    }
    
    return TRUE;
} // EdProp_WritePrefs


/******************************/
/* Static callback procedures */
/******************************/
/************************************************************************/
/* MyAdminPrefDialogProcedure						*/
/************************************************************************/
static BOOL CALLBACK MyAdminPrefDialogProcedure (HWND pmDialog, UINT pmMessage, 
    WPARAM pmWParam, LPARAM pmLParam)
{
    HWND			myItem;
    char			myString [PROP_STRING_SIZE], myTemp;
    static PropertyPageInfo	*stPropertyPageInfo;
    UINT			cnt, cnt1, cnt2;
    
    switch (pmMessage)
    {
    	case WM_INITDIALOG:
    	    stPropertyPageInfo = 
    	        (PropertyPageInfo * ) ((PROPSHEETPAGE *) pmLParam) -> lParam;

    	    // Center dialog, if necessary
    	    if (!stPropertyPageInfo -> isDialogCentred)
    	    {
	        EdGUI_CentreDialogBox (GetParent (pmDialog));
	        stPropertyPageInfo -> isDialogCentred = TRUE;
	    }

   	    // Initialize the dialog box values 
   	    // from the current properties
    	    MyPropertyToDialog (pmDialog, PREF_CHANGEUSERNAME);
    	    MyPropertyToDialog (pmDialog, PREF_USEMYDOCUMENTS);
   	    MyPropertyToDialog (pmDialog, PREF_STARTUPDIR);
    	    MyPropertyToDialog (pmDialog, PREF_ENTERADVANCED);
    	    MyPropertyToDialog (pmDialog, PREF_DEBUGGERAVAIL);
    	    MyPropertyToDialog (pmDialog, PREF_ONLYSTARTUPDIR);
    	    MyPropertyToDialog (pmDialog, PREF_ALLOWEDDRIVES);
    	    MyPropertyToDialog (pmDialog, PREF_ALLOWEDDRIVESLIST);
    	    MyPropertyToDialog (pmDialog, PREF_FORBIDDENDRIVES);
    	    MyPropertyToDialog (pmDialog, PREF_FORBIDDENDRIVESLIST);
    	    MyPropertyToDialog (pmDialog, PREF_CANTCHANGEPREF);
    	    MyPropertyToDialog (pmDialog, PREF_PROHIBITSYSEXEC);
    	    MyPropertyToDialog (pmDialog, PREF_NOSOUND);
	    
	    // If we're using the "My Documents" folder for startup
	    // then ignore the STartup Dirtcory text box.
	    if (MyCheckedToBool (pmDialog, PREF_USEMYDOCUMENTS))
	    {
	        myItem = GetDlgItem (pmDialog, PREF_STARTUPDIR);
	        EnableWindow (myItem, FALSE);
	    }
	    
	    // If the allowed drives box is checked, disable the
	    // forbidden drives and if forbidden drives is checked
	    // disable the allowed drives.  As well, enable a list
	    // only if corresponding check box is checked
	    if (MyCheckedToBool (pmDialog, PREF_ALLOWEDDRIVES))
	    {
	        myItem = GetDlgItem (pmDialog, PREF_FORBIDDENDRIVES);
	        EnableWindow (myItem, FALSE);
	    }
	    else
	    {
	        myItem = GetDlgItem (pmDialog, PREF_ALLOWEDDRIVESLIST);
	        EnableWindow (myItem, FALSE);
	    }
	    
	    if (MyCheckedToBool (pmDialog, PREF_FORBIDDENDRIVES))
	    {
	        myItem = GetDlgItem (pmDialog, PREF_ALLOWEDDRIVES);
	        EnableWindow (myItem, FALSE);
	    }
	    else
	    {
	        myItem = GetDlgItem (pmDialog, 
	            PREF_FORBIDDENDRIVESLIST);
	        EnableWindow (myItem, FALSE);
	    }
    	    return TRUE;
    	
    	case WM_COMMAND:
	    switch (LOWORD (pmWParam))
	    {
	    	case PREF_STARTUPDIR:
    	    	    if (HIWORD (pmWParam) == EN_CHANGE)
    	    	    {
    	    	        MyAdjustChangeFlag (pmDialog, LOWORD (pmWParam),
    	    	            &stPropertyPageInfo -> adminPrefChanges);
	    	    }
	    	    else
	    	    {
	    	    	return FALSE;
	    	    }
	    	    break;
	    	case PREF_USEMYDOCUMENTS:
   	    	    MyAdjustChangeFlag (pmDialog, LOWORD (pmWParam),
    	    	        &stPropertyPageInfo -> adminPrefChanges);
		    if (MyCheckedToBool (pmDialog, PREF_USEMYDOCUMENTS))
		    {
		        myItem = GetDlgItem (pmDialog, PREF_STARTUPDIR);
		        EnableWindow (myItem, FALSE);
		    }
		    else
		    {
		        myItem = GetDlgItem (pmDialog, PREF_STARTUPDIR);
		        EnableWindow (myItem, TRUE);
		    }
		    break;	    	    
	    	case PREF_CHANGEUSERNAME:
	    	case PREF_ENTERADVANCED:
	    	case PREF_DEBUGGERAVAIL:
	    	case PREF_ONLYSTARTUPDIR:
	    	case PREF_CANTCHANGEPREF:
	    	case PREF_PROHIBITSYSEXEC:
	    	case PREF_NOSOUND:
   	    	    MyAdjustChangeFlag (pmDialog, LOWORD (pmWParam),
    	    	        &stPropertyPageInfo -> adminPrefChanges);
    	    	    break;
	    	case PREF_ALLOWEDDRIVES:
   	    	    MyAdjustChangeFlag (pmDialog, LOWORD (pmWParam),
    	    	        &stPropertyPageInfo -> adminPrefChanges);
		    if (MyCheckedToBool (pmDialog, PREF_ALLOWEDDRIVES))
		    {
		        myItem = GetDlgItem (pmDialog, PREF_ALLOWEDDRIVESLIST);
		        EnableWindow (myItem, TRUE);
		        myItem = GetDlgItem (pmDialog, PREF_FORBIDDENDRIVES);
		        EnableWindow (myItem, FALSE);
		    }
		    else
		    {
		        myItem = GetDlgItem (pmDialog, PREF_ALLOWEDDRIVESLIST);
		        EnableWindow (myItem, FALSE);
		        myItem = GetDlgItem (pmDialog, PREF_FORBIDDENDRIVES);
		        EnableWindow (myItem, TRUE);
		    }
    	    	    break;
	    	case PREF_FORBIDDENDRIVES:
   	    	    MyAdjustChangeFlag (pmDialog, LOWORD (pmWParam),
    	    	        &stPropertyPageInfo -> adminPrefChanges);
		    if (MyCheckedToBool (pmDialog, PREF_FORBIDDENDRIVES))
		    {
		        myItem = GetDlgItem (pmDialog, 
		            PREF_FORBIDDENDRIVESLIST);
		        EnableWindow (myItem, TRUE);
		        myItem = GetDlgItem (pmDialog, PREF_ALLOWEDDRIVES);
		        EnableWindow (myItem, FALSE);
		    }
		    else
		    {
		        myItem = GetDlgItem (pmDialog, 
		            PREF_FORBIDDENDRIVESLIST);
		        EnableWindow (myItem, FALSE);
		        myItem = GetDlgItem (pmDialog, PREF_ALLOWEDDRIVES);
		        EnableWindow (myItem, TRUE);
		    }
    	    	    break;
    	    	case PREF_ALLOWEDDRIVESLIST:
    	    	case PREF_FORBIDDENDRIVESLIST:
    	    	    if (HIWORD (pmWParam) == EN_UPDATE)
    	    	    {
   	    	        MyAdjustChangeFlag (pmDialog, LOWORD (pmWParam),
    	    	            &stPropertyPageInfo -> adminPrefChanges);
    	    	    	EdGUI_DlgLettersOnly (pmDialog, LOWORD (pmWParam),
    	    	    	    STAR_ALLOWED);
    	    	    }
		    break;	
	    	default:
	    	    return FALSE;
	    } // switch
	    
	    if (stPropertyPageInfo -> adminPrefChanges)
	    {
	    	PropSheet_Changed (GetParent (pmDialog), pmDialog);
	    }
	    else
	    {
	    	PropSheet_UnChanged (GetParent (pmDialog), pmDialog);
	    }
	    break;
	    
    	case WM_NOTIFY:
    	    switch (((NMHDR FAR *)pmLParam)->code)
    	    {
    	    	case PSN_SETACTIVE:
	    	    break;
    	    	case PSN_APPLY:
    	    	    // Set the properties from the dialog box values
	    	    MyDialogToProperty (pmDialog, PREF_CHANGEUSERNAME);
	    	    MyDialogToProperty (pmDialog, PREF_USEMYDOCUMENTS);
    	    	    MyDialogToProperty (pmDialog, PREF_STARTUPDIR);
	    	    MyDialogToProperty (pmDialog, PREF_ENTERADVANCED);
	    	    MyDialogToProperty (pmDialog, PREF_DEBUGGERAVAIL);
	    	    MyDialogToProperty (pmDialog, PREF_ONLYSTARTUPDIR);
	    	    MyDialogToProperty (pmDialog, PREF_ALLOWEDDRIVES);
	    	    MyDialogToProperty (pmDialog, PREF_ALLOWEDDRIVESLIST);
	    	    MyDialogToProperty (pmDialog, PREF_FORBIDDENDRIVES);
	    	    MyDialogToProperty (pmDialog, PREF_FORBIDDENDRIVESLIST);
	    	    MyDialogToProperty (pmDialog, PREF_CANTCHANGEPREF);
	    	    MyDialogToProperty (pmDialog, PREF_PROHIBITSYSEXEC);
	    	    MyDialogToProperty (pmDialog, PREF_NOSOUND);

		    // Set the flags
		    stPropertyPageInfo -> adminPrefChanges = 0;
		    
		    // Set the flag so prefs will be written
		    stPropertyPageInfo -> changesMade = TRUE;

    	    	    break;
    	    	case PSN_KILLACTIVE:
    	    	    // Remove duplicates and sort the allowed drive letters
	    	    GetDlgItemText (pmDialog, PREF_ALLOWEDDRIVESLIST, myString, 
	    	        PROP_STRING_SIZE);
	    	    for (cnt = 0 ; cnt < strlen (myString) ; cnt++)
	    	    {
	    	    	for (cnt1 = cnt + 1 ; cnt1 < strlen (myString) ; cnt1++)
	    	    	{
	    	    	    if (myString [cnt] > myString [cnt1])
	    	    	    {
	    	    	    	myTemp = myString [cnt1];
	    	    	    	myString [cnt1] = myString [cnt];
	    	    	    	myString [cnt] = myTemp;
	    	    	    }
	    	    	    else if (myString [cnt] == myString [cnt1])
	    	    	    {
	    	    	    	for (cnt2 = cnt1 + 1 ; 
	    	    	    	    cnt2 <= strlen (myString) ; cnt2++)
	    	    	    	{
	    	    	    	    myString [cnt2 - 1] = myString [cnt2];
	    	    	    	}
	    	    	    }
	    	    	}
	    	    }
		    SetDlgItemText (pmDialog, PREF_ALLOWEDDRIVESLIST, myString);

    	    	    // Check for duplicates and sort the forbidden drive letters
	    	    GetDlgItemText (pmDialog, PREF_FORBIDDENDRIVESLIST, 
	    	        myString, PROP_STRING_SIZE);
	    	    for (cnt = 0 ; cnt < strlen (myString) ; cnt++)
	    	    {
	    	    	for (cnt1 = cnt + 1 ; cnt1 < strlen (myString) ; cnt1++)
	    	    	{
	    	    	    if (myString [cnt] > myString [cnt1])
	    	    	    {
	    	    	    	myTemp = myString [cnt1];
	    	    	    	myString [cnt1] = myString [cnt];
	    	    	    	myString [cnt] = myTemp;
	    	    	    }
	    	    	    else if (myString [cnt] == myString [cnt1])
	    	    	    {
	    	    	    	for (cnt2 = cnt1 + 1 ; 
	    	    	    	    cnt2 <= strlen (myString) ; cnt2++)
	    	    	    	{
	    	    	    	    myString [cnt2 - 1] = myString [cnt2];
	    	    	    	}
	    	    	    }
	    	    	}
	    	    }
		    SetDlgItemText (pmDialog, PREF_FORBIDDENDRIVESLIST, 
		        myString);

		    SetWindowLong (pmDialog, DWL_MSGRESULT, FALSE);
		    return 1;
	    	    break;
    	    	case PSN_RESET:
		    break;
	    } // switch
    } // switch	
    
    return FALSE;
} // MyAdminPrefDialogProcedure


/************************************************************************/
/* MyAppearancePrefDialogProcedure					*/
/************************************************************************/
static BOOL CALLBACK MyAppearancePrefDialogProcedure (HWND pmDialog, 
    UINT pmMessage, WPARAM pmWParam, LPARAM pmLParam)
{
    char			myString [PROP_STRING_SIZE];
    LOGFONT			myLogFont;
    CHOOSEFONT			myChooseFont;
    Ed_Properties		myAppProperties;
    static PropertyPageInfo	*stPropertyPageInfo;
           
    switch (pmMessage)
    {
    	case WM_INITDIALOG:
    	    stPropertyPageInfo = 
    	        (PropertyPageInfo * ) ((PROPSHEETPAGE *) pmLParam) -> lParam;

    	    // Center dialog, if necessary
    	    if (!stPropertyPageInfo -> isDialogCentred)
    	    {
	        EdGUI_CentreDialogBox (GetParent (pmDialog));
	        stPropertyPageInfo -> isDialogCentred = TRUE;
	    }

   	    // Initialize the dialog box values 
   	    // from the current properties
    	    MyPropertyToDialog (pmDialog, PREF_EDITFONTNAME);
    	    MyPropertyToDialog (pmDialog, PREF_EDITFONTSIZE);
    	    MyPropertyToDialog (pmDialog, PREF_EDITCOLOUR);
    	    MyPropertyToDialog (pmDialog, PREF_BEGINFULLSCREEN);
    	    MyPropertyToDialog (pmDialog, PREF_ADVFULLSCREEN);
    	    MyPropertyToDialog (pmDialog, PREF_EDITTEXTROWS);
    	    MyPropertyToDialog (pmDialog, PREF_EDITTEXTCOLS);
    	    MyPropertyToDialog (pmDialog, PREF_CARETWIDTH);
    	    MyPropertyToDialog (pmDialog, PREF_DLGFONTSIZE);
    	    MyPropertyToDialog (pmDialog, PREF_BUTTONSHORTCUT);
    	    MyPropertyToDialog (pmDialog, PREF_SHOWDEBUGGERMENU);
    	    MyPropertyToDialog (pmDialog, PREF_SHOWWINDOWMENU);
	 
	    // Beginner mode is only available in Turing
	    if (!gProgram.isTuring)
	    {
	        EnableWindow (GetDlgItem (pmDialog, PREF_BEGINFULLSCREEN), 
	        	      FALSE);
	    }

    	    return TRUE;

	case WM_COMMAND:
	    switch (LOWORD (pmWParam))
	    {
	    	case PREF_EDITCHANGEFONT:
	    	    // Create the logical font
		    ZeroMemory (&myLogFont, sizeof (LOGFONT));
		    // Set the font size
	    	    GetDlgItemText (pmDialog, PREF_EDITFONTSIZE, myString, 
	    	        PROP_STRING_SIZE);
	    	    myLogFont.lfHeight = 
	    	        stPropertyPageInfo -> editFontSizeInPixels;
	    	    // Set the font name
	    	    GetDlgItemText (pmDialog, PREF_EDITFONTNAME, 
	    	        myLogFont.lfFaceName, LF_FACESIZE);

		    ZeroMemory (&myChooseFont, sizeof (CHOOSEFONT));
	    	    myChooseFont.lStructSize = sizeof (CHOOSEFONT);
	    	    myChooseFont.hwndOwner = pmDialog;
	    	    myChooseFont.lpLogFont = &myLogFont;
	    	    myChooseFont.Flags = CF_INITTOLOGFONTSTRUCT | 
	    	        CF_FIXEDPITCHONLY | CF_SCREENFONTS;

	   	    // Set the display font size
	    	    
	    	    if (ChooseFont (&myChooseFont))
	    	    {
	    	    	int	myFontSize;
	    	    	
	    	        SetDlgItemText (pmDialog, PREF_EDITFONTNAME, 
	    	            myLogFont.lfFaceName);
	    	        myFontSize = (int) 
	    	            (myChooseFont.iPointSize / 10.0 + 0.5);
	    	    	wsprintf (myString, "%d", myFontSize);
	    	    	SetDlgItemText (pmDialog, PREF_EDITFONTSIZE, myString);
	    	    	stPropertyPageInfo -> editFontSizeInPixels = 
	    	    	    myLogFont.lfHeight;
	    	    
	    	        if ((strcmp (gProperties.editFontName, 
	    	                myLogFont.lfFaceName) == 0) &&
	    	            (gProperties.editFontSize == myFontSize))
	    	                
	    	        {
	    	    	    stPropertyPageInfo -> appearPrefChanges &= 
	    	    	        ~FONT_CHANGED;
	    	        }
	    	        else
	    	        {
	    	    	    stPropertyPageInfo -> appearPrefChanges |= 
	    	    	        FONT_CHANGED;
	    	        }
	    	    }
	    	    break;
	    	case PREF_EDITCOLOUR:
	    	case PREF_BEGINFULLSCREEN:
	    	case PREF_ADVFULLSCREEN:
	    	case PREF_BUTTONSHORTCUT:
	    	case PREF_SHOWDEBUGGERMENU:
	    	case PREF_SHOWWINDOWMENU:
    	    	    MyAdjustChangeFlag (pmDialog, LOWORD (pmWParam),
    	    	        &stPropertyPageInfo -> appearPrefChanges);
	    	    break;
	    	case PREF_EDITTEXTROWS:
	    	case PREF_EDITTEXTCOLS:
	    	case PREF_CARETWIDTH:
	    	case PREF_DLGFONTSIZE:
    	    	    if (HIWORD (pmWParam) == EN_UPDATE)
    	    	    {
   	    	        MyAdjustChangeFlag (pmDialog, LOWORD (pmWParam),
    	    	            &stPropertyPageInfo -> appearPrefChanges);
    	    	    	EdGUI_DlgNumbersOnly (pmDialog, LOWORD (pmWParam));
    	    	    }
    	    	    break;
	    	default:
	    	    return FALSE;
	    } // switch
	    
	    if (stPropertyPageInfo -> appearPrefChanges)
	    {
	    	PropSheet_Changed (GetParent (pmDialog), pmDialog);
	    }
	    else
	    {
	    	PropSheet_UnChanged (GetParent (pmDialog), pmDialog);
	    }
	    break;
	        	
    	case WM_NOTIFY:
    	    switch (((NMHDR FAR *)pmLParam)->code)
    	    {
    	    	case PSN_SETACTIVE:    	    	    
	    	    break;
    	    	case PSN_APPLY:
    	    	    // Set the properties from the dialog box values
	    	    MyDialogToProperty (pmDialog, PREF_EDITFONTNAME);
	    	    MyDialogToProperty (pmDialog, PREF_EDITFONTSIZE);
	    	    MyDialogToProperty (pmDialog, PREF_EDITCOLOUR);
	    	    MyDialogToProperty (pmDialog, PREF_BEGINFULLSCREEN);
	    	    MyDialogToProperty (pmDialog, PREF_ADVFULLSCREEN);
 	   	    MyDialogToProperty (pmDialog, PREF_EDITTEXTROWS);
 	   	    MyDialogToProperty (pmDialog, PREF_EDITTEXTCOLS);
	    	    MyDialogToProperty (pmDialog, PREF_CARETWIDTH);
	    	    MyDialogToProperty (pmDialog, PREF_DLGFONTSIZE);
 	   	    MyDialogToProperty (pmDialog, PREF_BUTTONSHORTCUT);
	    	    MyDialogToProperty (pmDialog, PREF_SHOWDEBUGGERMENU);
	    	    MyDialogToProperty (pmDialog, PREF_SHOWWINDOWMENU);
		        
		    // Set the flags
		    stPropertyPageInfo -> appearPrefChanges = 0;

		    // Set the properties
		    MyConvertToAppProperties (&myAppProperties);
		    Ed_PropertiesSet (myAppProperties);
                
                    // Implement the new properties
                    Ed_PropertiesImplementChanges ();
		    
		    // Set the flag so prefs will be written
		    stPropertyPageInfo -> changesMade = TRUE;

    	    	    break;
    	    	case PSN_KILLACTIVE:
	    	    GetDlgItemText (pmDialog, PREF_EDITTEXTROWS, myString, 
	    	        PROP_STRING_SIZE);
	    	    if (myString [0] == 0)
	    	    {
	    	    	MyPropertyToDialog (pmDialog, PREF_EDITTEXTROWS);
	    	    }
	    	    GetDlgItemText (pmDialog, PREF_EDITTEXTCOLS, myString, 
	    	        PROP_STRING_SIZE);
	    	    if (myString [0] == 0)
	    	    {
	    	    	MyPropertyToDialog (pmDialog, PREF_EDITTEXTCOLS);
	    	    }
	    	    GetDlgItemText (pmDialog, PREF_CARETWIDTH, myString, 
	    	        PROP_STRING_SIZE);
	    	    if (myString [0] == 0)
	    	    {
	    	    	MyPropertyToDialog (pmDialog, PREF_CARETWIDTH);
	    	    }
	    	    GetDlgItemText (pmDialog, PREF_DLGFONTSIZE, myString, 
	    	        PROP_STRING_SIZE);
	    	    if (myString [0] == 0)
	    	    {
	    	    	MyPropertyToDialog (pmDialog, PREF_DLGFONTSIZE);
	    	    }
		    SetWindowLong (pmDialog, DWL_MSGRESULT, FALSE);
		    return 1;
    	    	case PSN_RESET:
		    break;
	    } // switch
    } // switch	
    
    return FALSE;
} // MyAppearancePrefDialogProcedure


/************************************************************************/
/* MyGeneralPrefDialogProcedure						*/
/************************************************************************/
static BOOL CALLBACK MyGeneralPrefDialogProcedure (HWND pmDialog, 
    UINT pmMessage, WPARAM pmWParam, LPARAM pmLParam)
{
    char			myString [PROP_STRING_SIZE];
    Ed_Properties		myAppProperties;
    static PropertyPageInfo	*stPropertyPageInfo;
           
    switch (pmMessage)
    {
    	case WM_INITDIALOG:
    	    stPropertyPageInfo = 
    	        (PropertyPageInfo * ) ((PROPSHEETPAGE *) pmLParam) -> lParam;

    	    // Center dialog, if necessary
    	    if (!stPropertyPageInfo -> isDialogCentred)
    	    {
	        EdGUI_CentreDialogBox (GetParent (pmDialog));
	        stPropertyPageInfo -> isDialogCentred = TRUE;
	    }

   	    // Initialize the dialog box values 
   	    // from the current properties
    	    MyPropertyToDialog (pmDialog, PREF_BEGINNER);
    	    MyPropertyToDialog (pmDialog, PREF_ONEWINDOW);
    	    MyPropertyToDialog (pmDialog, PREF_EDITFULLPATH);
    	    MyPropertyToDialog (pmDialog, PREF_EDITSAVEBACKUP);
    	    MyPropertyToDialog (pmDialog, PREF_INDENTSIZE);
    	    MyPropertyToDialog (pmDialog, PREF_CONFIRMQUIT);
    	    MyPropertyToDialog (pmDialog, PREF_SKIPSPLASH);
    	    MyPropertyToDialog (pmDialog, PREF_ADDSUFFIX);
    	    MyPropertyToDialog (pmDialog, PREF_EDITSPACESTOTABS);
//    	    MyPropertyToDialog (pmDialog, PREF_OPENBRACESAMELINE);
    	    MyPropertyToDialog (pmDialog, PREF_EDITSPACESPERTAB);
    	    MyPropertyToDialog (pmDialog, PREF_NORECENTFILES);
    	    MyPropertyToDialog (pmDialog, PREF_USELASTACTIVEDIRECTORY);
    	    MyPropertyToDialog (pmDialog, PREF_FINDUSESSELECTION);
	 
	    // Beginner mode is only available in Turing
	    if (!gProgram.isTuring)
	    {
	        EnableWindow (GetDlgItem (pmDialog, PREF_BEGINNER), FALSE);
	    }
	    EnableWindow (GetDlgItem (pmDialog, PREF_ONEWINDOW), FALSE);

	    MyDisplayFileAssoc (pmDialog);

    	    return TRUE;

	case WM_COMMAND:
	    switch (LOWORD (pmWParam))
	    {
	    	case PREF_BEGINNER:
	    	case PREF_ONEWINDOW:
	    	case PREF_EDITFULLPATH:
	    	case PREF_EDITSAVEBACKUP:
	    	case PREF_CONFIRMQUIT:
	    	case PREF_SKIPSPLASH:
	    	case PREF_ADDSUFFIX:
	    	case PREF_EDITSPACESTOTABS:
//	    	case PREF_OPENBRACESAMELINE:
	    	case PREF_NORECENTFILES:
		case PREF_USELASTACTIVEDIRECTORY:
		case PREF_FINDUSESSELECTION:
    	    	    MyAdjustChangeFlag (pmDialog, LOWORD (pmWParam),
    	    	        &stPropertyPageInfo -> generalPrefChanges);
	    	    break;
	    	case PREF_INDENTSIZE:
	    	case PREF_EDITSPACESPERTAB:
    	    	    if (HIWORD (pmWParam) == EN_UPDATE)
    	    	    {
   	    	        MyAdjustChangeFlag (pmDialog, LOWORD (pmWParam),
    	    	            &stPropertyPageInfo -> generalPrefChanges);
    	    	    	EdGUI_DlgNumbersOnly (pmDialog, LOWORD (pmWParam));
    	    	    }
    	    	    break;
		case PREF_RESETFILEASSOC:
		    EdReg_ResetFileAssoc (pmDialog);
	    	    SHChangeNotify (SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, 
	    	    		    NULL);
		    MyDisplayFileAssoc (pmDialog);
		    break;
		case PREF_ASSOCWITHREADY:
		    EdReg_SetFileAssoc (pmDialog);
	    	    SHChangeNotify (SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, 
	    	    		    NULL);
		    MyDisplayFileAssoc (pmDialog);
		    break;
	    	default:
	    	    return FALSE;
	    } // switch
	    
	    if (stPropertyPageInfo -> generalPrefChanges)
	    {
	    	PropSheet_Changed (GetParent (pmDialog), pmDialog);
	    }
	    else
	    {
	    	PropSheet_UnChanged (GetParent (pmDialog), pmDialog);
	    }
	    break;
	        	
    	case WM_NOTIFY:
    	    switch (((NMHDR FAR *)pmLParam)->code)
    	    {
    	    	case PSN_SETACTIVE:    	    	    
	    	    break;
    	    	case PSN_APPLY:
    	    	    // Set the properties from the dialog box values
	    	    MyDialogToProperty (pmDialog, PREF_BEGINNER);
	    	    MyDialogToProperty (pmDialog, PREF_ONEWINDOW);
	    	    MyDialogToProperty (pmDialog, PREF_EDITFULLPATH);
	    	    MyDialogToProperty (pmDialog, PREF_EDITSAVEBACKUP);
	    	    MyDialogToProperty (pmDialog, PREF_INDENTSIZE);
 	   	    MyDialogToProperty (pmDialog, PREF_CONFIRMQUIT);
 	   	    MyDialogToProperty (pmDialog, PREF_SKIPSPLASH);
	    	    MyDialogToProperty (pmDialog, PREF_ADDSUFFIX);
	    	    MyDialogToProperty (pmDialog, PREF_EDITSPACESTOTABS);
	    	    MyDialogToProperty (pmDialog, PREF_OPENBRACESAMELINE);
 	   	    MyDialogToProperty (pmDialog, PREF_EDITSPACESPERTAB);
 	   	    MyDialogToProperty (pmDialog, PREF_NORECENTFILES);
 	   	    MyDialogToProperty (pmDialog, PREF_USELASTACTIVEDIRECTORY);
 	   	    MyDialogToProperty (pmDialog, PREF_FINDUSESSELECTION);
		        
		    // Set the flags
		    stPropertyPageInfo -> generalPrefChanges = 0;

		    // Set the properties
		    MyConvertToAppProperties (&myAppProperties);
		    Ed_PropertiesSet (myAppProperties);
                
                    // Implement the new properties
                    Ed_PropertiesImplementChanges ();
		    
		    // Set the flag so prefs will be written
		    stPropertyPageInfo -> changesMade = TRUE;

    	    	    break;
    	    	case PSN_KILLACTIVE:
	    	    GetDlgItemText (pmDialog, PREF_EDITSPACESPERTAB, myString, 
	    	        PROP_STRING_SIZE);
	    	    if (myString [0] == 0)
	    	    {
	    	    	MyPropertyToDialog (pmDialog, PREF_EDITSPACESPERTAB);
	    	    }
	    	    GetDlgItemText (pmDialog, PREF_INDENTSIZE, myString, 
	    	        PROP_STRING_SIZE);
	    	    if (myString [0] == 0)
	    	    {
	    	    	MyPropertyToDialog (pmDialog, PREF_INDENTSIZE);
	    	    }
		    SetWindowLong (pmDialog, DWL_MSGRESULT, FALSE);
		    return 1;
	    	    break;
    	    	case PSN_RESET:
		    break;
	    } // switch
    } // switch	
    
    return FALSE;
} // MyGeneralPrefDialogProcedure


/************************************************************************/
/* MyJavaPrefDialogProcedure						*/
/************************************************************************/
static BOOL CALLBACK MyJavaPrefDialogProcedure (HWND pmDialog, 
    UINT pmMessage, WPARAM pmWParam, LPARAM pmLParam)
{
    Ed_Properties		myAppProperties;
//    FilePath			myAlternatePath;
//    HWND			myItem;
    int				myDialogJVMType;
    static PropertyPageInfo	*stPropertyPageInfo;
           
    switch (pmMessage)
    {
    	case WM_INITDIALOG:
    	    stPropertyPageInfo = 
    	        (PropertyPageInfo * ) ((PROPSHEETPAGE *) pmLParam) -> lParam;

    	    // Center dialog, if necessary
    	    if (!stPropertyPageInfo -> isDialogCentred)
    	    {
	        EdGUI_CentreDialogBox (GetParent (pmDialog));
	        stPropertyPageInfo -> isDialogCentred = TRUE;
	    }

/*
	    // Create the path for the Java 2 DLL
	    // We're reusing a path name variable here to cut down on stack 
	    // use.  This *isn't* the alternate path
	    EdFile_ConstructPath (myAlternatePath, IDS_JAVA_BIN_NEW, 
	    			  IDS_JAVA_DLL);

	    // Disable the New Java VM check box if new Java VM is not installed
	    myItem = GetDlgItem (pmDialog, PREF_JAVAUSENEWJVM);
	    EnableWindow (myItem, EdFile_FileExists (myAlternatePath));
*/

   	    // Initialize the dialog box values 
   	    // from the current properties
    	    MyPropertyToDialog (pmDialog, PREF_JAVAADDCLASSPATH);
    	    MyPropertyToDialog (pmDialog, PREF_JAVAJIKESOPTION);
    	    MyPropertyToDialog (pmDialog, PREF_JAVAJVMSIZE);
            if (gProperties.oldJavaCompile)
	    {
            	CheckDlgButton (pmDialog, PREF_JAVA13COMPILE, BST_CHECKED);
            	CheckDlgButton (pmDialog, PREF_JAVA14COMPILE, BST_UNCHECKED);
            }
            else
            {
            	CheckDlgButton (pmDialog, PREF_JAVA13COMPILE, BST_UNCHECKED);
            	CheckDlgButton (pmDialog, PREF_JAVA14COMPILE, BST_CHECKED);
            }
            if (gProperties.JVMType == JRE_131)
            {
            	CheckDlgButton (pmDialog, PREF_JAVA13JVM, BST_CHECKED);
            	CheckDlgButton (pmDialog, PREF_JAVA14JVM, BST_UNCHECKED);
            }
            else
            {
            	CheckDlgButton (pmDialog, PREF_JAVA13JVM, BST_UNCHECKED);
            	CheckDlgButton (pmDialog, PREF_JAVA14JVM, BST_CHECKED);
            }
            // TW Future expansion here for alternatives
    	    
    	    if (gProperties.memoryForJVMInMB == 0)
    	    {
    	      	SetDlgItemText (pmDialog, PREF_JAVAJVMSIZE, "");
    	    }

    	    return TRUE;

	case WM_COMMAND:
	    switch (LOWORD (pmWParam))
	    {
	    	case PREF_JAVAADDCLASSPATH:
	    	case PREF_JAVAJIKESOPTION:
//	    	case PREF_JAVAUSENEWJVM:
    	    	    MyAdjustChangeFlag (pmDialog, LOWORD (pmWParam),
    	    	        &stPropertyPageInfo -> javaPrefChanges);
    	    	    break;
	    	case PREF_JAVA13COMPILE:
	    	case PREF_JAVA14COMPILE:
	    	    if (MyCheckedToBool (pmDialog, PREF_JAVA14COMPILE) &&
	    	        MyCheckedToBool (pmDialog, PREF_JAVA13JVM))
	    	    {
		    	EdGUI_Message1 (NULL, 0, IDS_JAVA_13_NO_ASSERTS_TITLE, 
		    	    		    IDS_JAVA_13_NO_ASSERTS);
	            	CheckDlgButton (pmDialog, PREF_JAVA13COMPILE, 
	            	    		BST_CHECKED);
	            	CheckDlgButton (pmDialog, PREF_JAVA14COMPILE, 
	            	    		BST_UNCHECKED);
	    	    }
	    	    else
	    	    {
    	    	    	MyAdjustChangeFlag (pmDialog, PREF_JAVA13COMPILE,
    	    	            &stPropertyPageInfo -> javaPrefChanges);
    	    	    }
		    break;
	    	case PREF_JAVA13JVM:
	    	case PREF_JAVA14JVM:
	    	    if (MyCheckedToBool (pmDialog, PREF_JAVA13JVM))
	    	    {
	    	    	// If we're switching to a 1.3 machine, disallow
	    	    	// the assert statement
	    	    	myDialogJVMType = JRE_131;
	    	    	if (MyCheckedToBool (pmDialog, PREF_JAVA14COMPILE))
	    	    	{
		    	    EdGUI_Message1 (NULL, 0, 
		    	    		    IDS_JAVA_13_NO_ASSERTS_TITLE, 
		    	    		    IDS_JAVA_13_NO_ASSERTS);
	            	    CheckDlgButton (pmDialog, PREF_JAVA13COMPILE, 
	            	    		    BST_CHECKED);
	            	    CheckDlgButton (pmDialog, PREF_JAVA14COMPILE, 
	            	    		    BST_UNCHECKED);
    	    	    	    MyAdjustChangeFlag (pmDialog, PREF_JAVA13COMPILE,
    	    	                &stPropertyPageInfo -> javaPrefChanges);
	    	    	}
	    	    }
	    	    else
	    	    {
	    	    	if ((gProgram.operatingSystem == WIN_95) ||
	    	    	    (gProgram.operatingSystem == WIN_95_OSR2))
			{
		    	    EdGUI_Message1 (NULL, 0, IDS_CANT_CHANGE_JVM_TITLE, 
		    	    		    IDS_CANT_CHANGE_JVM);
	            	    CheckDlgButton (pmDialog, PREF_JAVA13JVM, 
	            	    		    BST_CHECKED);
	            	    CheckDlgButton (pmDialog, PREF_JAVA14JVM, 
	            	    		    BST_UNCHECKED);
	    	    	    myDialogJVMType = JRE_131;
			}
			else
			{
	    	    	    myDialogJVMType = JRE_BUILT_IN;
	    	    	}
	    	    }
    	            if (gProperties.JVMType == myDialogJVMType)
    	            {
    	    	    	stPropertyPageInfo -> javaPrefChanges &= 
    	    	    		~JAVA13JVM_CHANGED;
    	            }
    	            else
    	            {
    	    	    	stPropertyPageInfo -> javaPrefChanges |= 
    	    	    		JAVA13JVM_CHANGED;
    	            }
		    break;
	    	case PREF_JAVAJVMSIZE:
   	    	    if (HIWORD (pmWParam) == EN_UPDATE)
    	    	    {
   	    	        MyAdjustChangeFlag (pmDialog, LOWORD (pmWParam),
    	    	            &stPropertyPageInfo -> runPrefChanges);
    	    	    	EdGUI_DlgNumbersOnly (pmDialog, LOWORD (pmWParam));
    	    	    }
	    	    break;
	    	default:
	    	    return FALSE;
	    } // switch
	    
	    if (stPropertyPageInfo -> javaPrefChanges)
	    {
	    	PropSheet_Changed (GetParent (pmDialog), pmDialog);
	    }
	    else
	    {
	    	PropSheet_UnChanged (GetParent (pmDialog), pmDialog);
	    }
	    break;
	        	
    	case WM_NOTIFY:
    	    switch (((NMHDR FAR *)pmLParam)->code)
    	    {
    	    	case PSN_SETACTIVE:    	    	    
	    	    break;
    	    	case PSN_APPLY:
    	    	    // Set the properties from the dialog box values
 	   	    MyDialogToProperty (pmDialog, PREF_JAVAADDCLASSPATH);
	    	    MyDialogToProperty (pmDialog, PREF_JAVAJIKESOPTION);
	    	    MyDialogToProperty (pmDialog, PREF_JAVAJVMSIZE);
    	    	    MyDialogToProperty (pmDialog, PREF_JAVA13COMPILE);
	    	    if (MyCheckedToBool (pmDialog, PREF_JAVA13JVM))
	    	    {
	    	    	gProperties.JVMType = JRE_131;
	    	    }
	    	    else
	    	    {
	    	    	gProperties.JVMType = JRE_BUILT_IN;
	    	    }
		        
		    // Set the flags
		    stPropertyPageInfo -> javaPrefChanges = 0;

		    // Set the properties
		    MyConvertToAppProperties (&myAppProperties);
		    Ed_PropertiesSet (myAppProperties);
                
                    // Implement the new properties
                    Ed_PropertiesImplementChanges ();
		    
		    // Set the flag so prefs will be written
		    stPropertyPageInfo -> changesMade = TRUE;

    	    	    break;
    	    	case PSN_KILLACTIVE:
		    SetWindowLong (pmDialog, DWL_MSGRESULT, FALSE);
		    return 1;
    	    	case PSN_RESET:
		    break;
	    } // switch
    } // switch	
    
    return FALSE;
} // MyJavaPrefDialogProcedure


/************************************************************************/
/* MyPrintingPrefDialogProcedure					*/
/************************************************************************/
static BOOL CALLBACK MyPrintingPrefDialogProcedure (HWND pmDialog, 
    UINT pmMessage, WPARAM pmWParam, LPARAM pmLParam)
{
    char			myString [PROP_STRING_SIZE];
    LOGFONT			myLogFont;
    CHOOSEFONT			myChooseFont;
    int				myDialogPart;
    HWND			myItem;
    int				myValue;
    Ed_Properties		myAppProperties;
    static PropertyPageInfo	*stPropertyPageInfo;
           
    switch (pmMessage)
    {
    	case WM_INITDIALOG:
    	    stPropertyPageInfo = 
    	        (PropertyPageInfo * ) ((PROPSHEETPAGE *) pmLParam) -> lParam;

    	    // Center dialog, if necessary
    	    if (!stPropertyPageInfo -> isDialogCentred)
    	    {
	        EdGUI_CentreDialogBox (GetParent (pmDialog));
	        stPropertyPageInfo -> isDialogCentred = TRUE;
	    }

    	    // Initialize the dialog box values 
    	    // from the current properties
    	    MyPropertyToDialog (pmDialog, PREF_PRINTFONTNAME);
    	    MyPropertyToDialog (pmDialog, PREF_PRINTFONTSIZE);
    	    MyPropertyToDialog (pmDialog, PREF_PRINTUSERNAME);
    	    MyPropertyToDialog (pmDialog, PREF_PRINTBOLD);
    	    MyPropertyToDialog (pmDialog, PREF_PRINTITAL);
    	    MyPropertyToDialog (pmDialog, PREF_PRINTHEADER);
    	    MyPropertyToDialog (pmDialog, PRINT_PRINTPRINTUSERNAME);
    	    MyPropertyToDialog (pmDialog, PREF_PRINTLINENUMBER);
    	    MyPropertyToDialog (pmDialog, PREF_PRINTTWOUP);
    	    MyPropertyToDialog (pmDialog, PREF_PRINTBORDER);
    	    MyPropertyToDialog (pmDialog, PREF_PRINTHDRFONTNAME);
    	    MyPropertyToDialog (pmDialog, PREF_PRINTHDRFONTSIZE);
    	    MyMarginToDialog (pmDialog, PREF_PRINTLEFTMARG, 
    	    	gProperties.printMarginLeft);
    	    MyMarginToDialog (pmDialog, PREF_PRINTRIGHTMARG, 
    	    	gProperties.printMarginRight);
    	    MyMarginToDialog (pmDialog, PREF_PRINTTOPMARG, 
    	    	gProperties.printMarginTop);
    	    MyMarginToDialog (pmDialog, PREF_PRINTBOTTOMMARG, 
    	    	gProperties.printMarginBottom);

    	    // Disable  "User Name in Header" if no header being printed
	    myItem = GetDlgItem (pmDialog, PRINT_PRINTPRINTUSERNAME);
	    EnableWindow (myItem, gProperties.printPageHeader);
	    	        
    	    return TRUE;
    	    
	case WM_COMMAND:
	    switch (LOWORD (pmWParam))
	    {
	    	case PREF_PRINTCHANGEFONT:
	    	    // Create the logical font
		    ZeroMemory (&myLogFont, sizeof (LOGFONT));
		    // Set the font size
	    	    GetDlgItemText (pmDialog, PREF_PRINTFONTSIZE, myString, 
	    	        PROP_STRING_SIZE);
	    	    myLogFont.lfHeight = 
	    	        stPropertyPageInfo -> printFontSizeInPixels;
	    	    // Set the font name
	    	    GetDlgItemText (pmDialog, PREF_PRINTFONTNAME, 
	    	        myLogFont.lfFaceName, LF_FACESIZE);

		    ZeroMemory (&myChooseFont, sizeof (CHOOSEFONT));
	    	    myChooseFont.lStructSize = sizeof (CHOOSEFONT);
	    	    myChooseFont.hwndOwner = pmDialog;
	    	    myChooseFont.lpLogFont = &myLogFont;
	    	    myChooseFont.Flags = CF_INITTOLOGFONTSTRUCT | 
	    	        CF_SCREENFONTS;

	   	    // Set the display font size
	    	    
	    	    if (ChooseFont (&myChooseFont))
	    	    {
	    	    	int	myFontSize;
	    	    	
	    	        SetDlgItemText (pmDialog, PREF_PRINTFONTNAME, 
	    	            myLogFont.lfFaceName);
	    	        myFontSize = (int) 
	    	            (myChooseFont.iPointSize / 10.0 + 0.5);
	    	    	wsprintf (myString, "%d", myFontSize);
	    	    	SetDlgItemText (pmDialog, PREF_PRINTFONTSIZE, myString);
	    	    	stPropertyPageInfo -> printFontSizeInPixels = 
	    	    	    myLogFont.lfHeight;
	    	    
	    	        if ((strcmp (gProperties.printFontName, 
	    	                myLogFont.lfFaceName) == 0) &&
	    	            (gProperties.printFontSize == myFontSize))
	    	                
	    	        {
	    	    	    stPropertyPageInfo ->printPrefChanges &= 
	    	    	        ~FONT_CHANGED;
	    	        }
	    	        else
	    	        {
	    	    	    stPropertyPageInfo -> printPrefChanges |= 
	    	    	        FONT_CHANGED;
	    	        }
	    	    }
	    	    break;
	    	case PREF_PRINTHDRCHANGEFONT:
	    	    // Create the logical font
		    ZeroMemory (&myLogFont, sizeof (LOGFONT));
		    // Set the font size
	    	    GetDlgItemText (pmDialog, PREF_PRINTHDRFONTSIZE, myString, 
	    	        PROP_STRING_SIZE);
	    	    myLogFont.lfHeight = 
	    	        stPropertyPageInfo -> printFontSizeInPixels;
	    	    // Set the font name
	    	    GetDlgItemText (pmDialog, PREF_PRINTHDRFONTNAME, 
	    	        myLogFont.lfFaceName, LF_FACESIZE);

		    ZeroMemory (&myChooseFont, sizeof (CHOOSEFONT));
	    	    myChooseFont.lStructSize = sizeof (CHOOSEFONT);
	    	    myChooseFont.hwndOwner = pmDialog;
	    	    myChooseFont.lpLogFont = &myLogFont;
	    	    myChooseFont.Flags = CF_INITTOLOGFONTSTRUCT | 
	    	        CF_SCREENFONTS;

	   	    // Set the display font size
	    	    
	    	    if (ChooseFont (&myChooseFont))
	    	    {
	    	    	int	myFontSize;
	    	    	
	    	        SetDlgItemText (pmDialog, PREF_PRINTHDRFONTNAME, 
	    	            myLogFont.lfFaceName);
	    	        myFontSize = (int) 
	    	            (myChooseFont.iPointSize / 10.0 + 0.5);
	    	    	wsprintf (myString, "%d", myFontSize);
	    	    	SetDlgItemText (pmDialog, PREF_PRINTHDRFONTSIZE, 
	    	    	    myString);
	    	    	stPropertyPageInfo -> printFontSizeInPixels = 
	    	    	    myLogFont.lfHeight;
	    	    
	    	        if ((strcmp (gProperties.printFontName, 
	    	                myLogFont.lfFaceName) == 0) &&
	    	            (gProperties.printFontSize == myFontSize))
	    	                
	    	        {
	    	    	    stPropertyPageInfo ->printPrefChanges &= 
	    	    	        ~PRINTHDRFONT_CHANGED;
	    	        }
	    	        else
	    	        {
	    	    	    stPropertyPageInfo -> printPrefChanges |= 
	    	    	        PRINTHDRFONT_CHANGED;
	    	        }
	    	    }
	    	    break;
	    	case PREF_PRINTUSERNAME:
    	    	    if (HIWORD (pmWParam) == EN_CHANGE)
    	    	    {
    	    	    	MyAdjustChangeFlag (pmDialog, LOWORD (pmWParam),
    	    	    	    &stPropertyPageInfo -> printPrefChanges);
	    	    }
	    	    else
	    	    {
	    	    	return FALSE;
	    	    }
	    	    break;
	    	case PREF_PRINTLEFTMARG:
	    	case PREF_PRINTRIGHTMARG:
	    	case PREF_PRINTTOPMARG:
	    	case PREF_PRINTBOTTOMMARG:
    	    	    if (HIWORD (pmWParam) == EN_CHANGE)
    	    	    {
    	    	    	stPropertyPageInfo -> printPrefChanges |= 
    	    	    	    PRINTMARGIN_CHANGED;
	    	    }
	    	    break;
	    	case PREF_PRINTBOLD:
	    	case PREF_PRINTITAL:
	    	case PRINT_PRINTPRINTUSERNAME:
	    	case PREF_PRINTLINENUMBER:
	    	case PREF_PRINTTWOUP:
	    	case PREF_PRINTBORDER:
     	    	    MyAdjustChangeFlag (pmDialog, LOWORD (pmWParam),
    	    	        &stPropertyPageInfo -> printPrefChanges);
	    	    break;
	    	case PREF_PRINTHEADER:
    	    	    MyAdjustChangeFlag (pmDialog, LOWORD (pmWParam),
    	    	        &stPropertyPageInfo -> printPrefChanges);
	    	    
	    	    // Disable  "User Name in Header" if no header being printed
		    myItem = GetDlgItem (pmDialog, PRINT_PRINTPRINTUSERNAME);
		    EnableWindow (myItem, 
		        MyCheckedToBool (pmDialog, PREF_PRINTHEADER));			
	    	    break;
	    	default:
	    	    return FALSE;
	    } // switch
	    
	    if (stPropertyPageInfo -> printPrefChanges)
	    {
	    	PropSheet_Changed (GetParent (pmDialog), pmDialog);
	    }
	    else
	    {
	    	PropSheet_UnChanged (GetParent (pmDialog), pmDialog);
	    }
	    break;

    	case WM_NOTIFY:
    	    switch (((NMHDR FAR *)pmLParam)->code)
    	    {
    	    	case PSN_SETACTIVE:
	    	    break;
    	    	case PSN_APPLY:
    	    	    // Set the properties from the dialog box values
	    	    MyDialogToProperty (pmDialog, PREF_PRINTFONTNAME);
	    	    MyDialogToProperty (pmDialog, PREF_PRINTFONTSIZE);
	    	    MyDialogToProperty (pmDialog, PREF_PRINTUSERNAME);
	    	    MyDialogToProperty (pmDialog, PREF_PRINTBOLD);
	    	    MyDialogToProperty (pmDialog, PREF_PRINTITAL);
	    	    MyDialogToProperty (pmDialog, PREF_PRINTHEADER);
	    	    MyDialogToProperty (pmDialog, PRINT_PRINTPRINTUSERNAME);
	    	    MyDialogToProperty (pmDialog, PREF_PRINTLINENUMBER);
	    	    MyDialogToProperty (pmDialog, PREF_PRINTTWOUP);
	    	    MyDialogToProperty (pmDialog, PREF_PRINTBORDER);
	    	    MyDialogToProperty (pmDialog, PREF_PRINTHDRFONTNAME);
	    	    MyDialogToProperty (pmDialog, PREF_PRINTHDRFONTSIZE);
    	            MyDialogToMargin (pmDialog, PREF_PRINTLEFTMARG, 
    	                &gProperties.printMarginLeft);
    	            MyDialogToMargin (pmDialog, PREF_PRINTRIGHTMARG, 
    	                &gProperties.printMarginRight);
    	            MyDialogToMargin (pmDialog, PREF_PRINTTOPMARG, 
    	                &gProperties.printMarginTop);
    	            MyDialogToMargin (pmDialog, PREF_PRINTBOTTOMMARG, 
    	                &gProperties.printMarginBottom);

		    // Set the flags
		    stPropertyPageInfo -> printPrefChanges = 0;

		    // Set the properties
		    MyConvertToAppProperties (&myAppProperties);
		    Ed_PropertiesSet (myAppProperties);
                
                    // Implement the new properties
                    Ed_PropertiesImplementChanges ();
		    
		    // Set the flag so prefs will be written
		    stPropertyPageInfo -> changesMade = TRUE;

    	    	    break;
    	    	case PSN_KILLACTIVE:
    	    	    for (myDialogPart = FIRST_MARGIN ; 
    	    	         myDialogPart <= LAST_MARGIN ; myDialogPart++)
    	    	    {
    	    	    	if (!MyDialogToMargin (pmDialog, myDialogPart, 
    	    	    	    		       &myValue))
    	    	    	{
    	    	    	    myItem = GetDlgItem (pmDialog, myDialogPart);
		    	    EdGUI_Message1 (pmDialog, 0, IDS_PREF_ERROR_TITLE, 
		    	    	IDS_NOT_A_NUMBER);
			    MessageBeep (MB_ICONEXCLAMATION);
    	    	    	    SetFocus (myItem);
			    SendMessage (myItem, EM_SETSEL, (WPARAM) 0, -1);
		    	    SetWindowLong (pmDialog, DWL_MSGRESULT, TRUE);
		    	    return 1;
		        }
		    }
		    SetWindowLong (pmDialog, DWL_MSGRESULT, FALSE);
		    return 1;
    	    	case PSN_RESET:
		    break;
	    } // switch
    } // switch	
    
    return FALSE;
} // MyPrintingPrefDialogProcedure


/************************************************************************/
/* MyRunPrefDialogProcedure						*/
/************************************************************************/
static BOOL CALLBACK MyRunPrefDialogProcedure (HWND pmDialog, 
    UINT pmMessage, WPARAM pmWParam, LPARAM pmLParam)
{
    char			myString [PROP_STRING_SIZE];
    LOGFONT			myLogFont;
    CHOOSEFONT			myChooseFont;
    Ed_Properties		myAppProperties;
    static PropertyPageInfo	*stPropertyPageInfo;
           
    switch (pmMessage)
    {
    	case WM_INITDIALOG:
    	    stPropertyPageInfo = 
    	        (PropertyPageInfo * ) ((PROPSHEETPAGE *) pmLParam) -> lParam;

    	    // Center dialog, if necessary
    	    if (!stPropertyPageInfo -> isDialogCentred)
    	    {
	        EdGUI_CentreDialogBox (GetParent (pmDialog));
	        stPropertyPageInfo -> isDialogCentred = TRUE;
	    }

   	    // Initialize the dialog box values 
   	    // from the current properties
    	    MyPropertyToDialog (pmDialog, PREF_RUNFONTNAME);
    	    MyPropertyToDialog (pmDialog, PREF_RUNFONTSIZE);
    	    MyPropertyToDialog (pmDialog, PREF_RUNGRAPHICSMODE);
    	    MyPropertyToDialog (pmDialog, PREF_RUNFULLSCREEN);
    	    MyPropertyToDialog (pmDialog, PREF_RUNTEXTROWS);
    	    MyPropertyToDialog (pmDialog, PREF_RUNTEXTCOLS);
    	    MyPropertyToDialog (pmDialog, PREF_RUNAPPLETXSIZE);
    	    MyPropertyToDialog (pmDialog, PREF_RUNAPPLETYSIZE);
    	    MyPropertyToDialog (pmDialog, PREF_RUNUSESMALLFONT);
    	    return TRUE;

	case WM_COMMAND:
	    switch (LOWORD (pmWParam))
	    {
	    	case PREF_RUNCHANGEFONT:
	    	    // Create the logical font
		    ZeroMemory (&myLogFont, sizeof (LOGFONT));
		    // Set the font size
	    	    GetDlgItemText (pmDialog, PREF_RUNFONTSIZE, myString, 
	    	        PROP_STRING_SIZE);
	    	    myLogFont.lfHeight = 
	    	        stPropertyPageInfo -> editFontSizeInPixels;
	    	    // Set the font name
	    	    GetDlgItemText (pmDialog, PREF_RUNFONTNAME, 
	    	        myLogFont.lfFaceName, LF_FACESIZE);

		    ZeroMemory (&myChooseFont, sizeof (CHOOSEFONT));
	    	    myChooseFont.lStructSize = sizeof (CHOOSEFONT);
	    	    myChooseFont.hwndOwner = pmDialog;
	    	    myChooseFont.lpLogFont = &myLogFont;
	    	    myChooseFont.Flags = CF_INITTOLOGFONTSTRUCT | 
	    	        CF_FIXEDPITCHONLY | CF_SCREENFONTS;

	   	    // Set the display font size
	    	    
	    	    if (ChooseFont (&myChooseFont))
	    	    {
	    	    	int	myFontSize;
	    	    	
	    	        SetDlgItemText (pmDialog, PREF_RUNFONTNAME, 
	    	            myLogFont.lfFaceName);
	    	        myFontSize = (int) 
	    	            (myChooseFont.iPointSize / 10.0 + 0.5);
	    	    	wsprintf (myString, "%d", myFontSize);
	    	    	SetDlgItemText (pmDialog, PREF_RUNFONTSIZE, myString);
	    	    	stPropertyPageInfo -> editFontSizeInPixels = 
	    	    	    myLogFont.lfHeight;
	    	    
	    	        if ((strcmp (gProperties.editFontName, 
	    	                myLogFont.lfFaceName) == 0) &&
	    	            (gProperties.editFontSize == myFontSize))
	    	                
	    	        {
	    	    	    stPropertyPageInfo -> runPrefChanges &= 
	    	    	        ~FONT_CHANGED;
	    	        }
	    	        else
	    	        {
	    	    	    stPropertyPageInfo -> runPrefChanges |= 
	    	    	        FONT_CHANGED;
	    	        }
	    	    }
	    	    break;
	    	case PREF_RUNGRAPHICSMODE:
	    	case PREF_RUNFULLSCREEN:
		case PREF_RUNUSESMALLFONT:
    	    	    MyAdjustChangeFlag (pmDialog, LOWORD (pmWParam),
    	    	        &stPropertyPageInfo -> runPrefChanges);
	    	    break;
	    	case PREF_RUNTEXTROWS:
	    	case PREF_RUNTEXTCOLS:
	    	case PREF_RUNAPPLETXSIZE:
	    	case PREF_RUNAPPLETYSIZE:
   	    	    if (HIWORD (pmWParam) == EN_UPDATE)
    	    	    {
   	    	        MyAdjustChangeFlag (pmDialog, LOWORD (pmWParam),
    	    	            &stPropertyPageInfo -> runPrefChanges);
    	    	    	EdGUI_DlgNumbersOnly (pmDialog, LOWORD (pmWParam));
    	    	    }
    	    	    break;
	    	default:
	    	    return FALSE;
	    } // switch
	    
	    if (stPropertyPageInfo -> runPrefChanges)
	    {
	    	PropSheet_Changed (GetParent (pmDialog), pmDialog);
	    }
	    else
	    {
	    	PropSheet_UnChanged (GetParent (pmDialog), pmDialog);
	    }
	    break;
	        	
    	case WM_NOTIFY:
    	    switch (((NMHDR FAR *)pmLParam)->code)
    	    {
    	    	case PSN_SETACTIVE:    	    	    
	    	    break;
    	    	case PSN_APPLY:
    	    	    // Set the properties from the dialog box values
	    	    MyDialogToProperty (pmDialog, PREF_RUNFONTNAME);
	    	    MyDialogToProperty (pmDialog, PREF_RUNFONTSIZE);
	    	    MyDialogToProperty (pmDialog, PREF_RUNGRAPHICSMODE);
	    	    MyDialogToProperty (pmDialog, PREF_RUNFULLSCREEN);
	    	    MyDialogToProperty (pmDialog, PREF_RUNTEXTROWS);
 	   	    MyDialogToProperty (pmDialog, PREF_RUNTEXTCOLS);
 	   	    MyDialogToProperty (pmDialog, PREF_RUNAPPLETXSIZE);
 	   	    MyDialogToProperty (pmDialog, PREF_RUNAPPLETYSIZE);
 	   	    MyDialogToProperty (pmDialog, PREF_RUNUSESMALLFONT);
		        
		    // Set the flags
		    stPropertyPageInfo -> runPrefChanges = 0;

		    // Set the properties
		    MyConvertToAppProperties (&myAppProperties);
		    Ed_PropertiesSet (myAppProperties);
                
                    // Implement the new properties
                    Ed_PropertiesImplementChanges ();
		    
		    // Set the flag so prefs will be written
		    stPropertyPageInfo -> changesMade = TRUE;

    	    	    break;
    	    	case PSN_KILLACTIVE:
	    	    GetDlgItemText (pmDialog, PREF_RUNTEXTROWS, myString, 
	    	        PROP_STRING_SIZE);
	    	    if (myString [0] == 0)
	    	    {
	    	    	MyPropertyToDialog (pmDialog, PREF_RUNTEXTROWS);
	    	    }
	    	    GetDlgItemText (pmDialog, PREF_RUNTEXTCOLS, myString, 
	    	        PROP_STRING_SIZE);
	    	    if (myString [0] == 0)
	    	    {
	    	    	MyPropertyToDialog (pmDialog, PREF_RUNTEXTCOLS);
	    	    }
	    	    GetDlgItemText (pmDialog, PREF_RUNAPPLETXSIZE, myString, 
	    	        PROP_STRING_SIZE);
	    	    if (myString [0] == 0)
	    	    {
	    	    	MyPropertyToDialog (pmDialog, PREF_RUNAPPLETXSIZE);
	    	    }
	    	    GetDlgItemText (pmDialog, PREF_RUNAPPLETYSIZE, myString, 
	    	        PROP_STRING_SIZE);
	    	    if (myString [0] == 0)
	    	    {
	    	    	MyPropertyToDialog (pmDialog, PREF_RUNAPPLETYSIZE);
	    	    }
		    SetWindowLong (pmDialog, DWL_MSGRESULT, FALSE);
		    return 1;
    	    	case PSN_RESET:
		    break;
	    } // switch
    } // switch	
    
    return FALSE;
} // MyRunPrefDialogProcedure


/*********************/
/* Static procedures */
/*********************/
/************************************************************************/
/* MyAdjustChangeFlag							*/
/*									*/
/* This procedure handles the adjustment to a change flag when a dialog */
/* item is changed.							*/
/************************************************************************/
static void	MyAdjustChangeFlag (HWND pmDialog, int pmDialogPart, 
				    unsigned int *pmFlag)
{
    char	*myCurrentStringValue, myDialogStringValue [PROP_STRING_SIZE];
    BOOL	myCurrentBoolValue, myDialogBoolValue;
    int		cnt;
   
    for (cnt = 0 ; cnt < stNumProperties ; cnt++)
    {
    	if (stPropertiesInitializer [cnt].dialogPart == pmDialogPart)
    	{
    	    if (stPropertiesInitializer [cnt].kind == STRING_PROP)
    	    {
    	        myCurrentStringValue = ((char *) & gProperties) + 
    	            stPropertiesInitializer [cnt].offset;
	    	
	    	GetDlgItemText (pmDialog, pmDialogPart, myDialogStringValue, 
	    	    PROP_STRING_SIZE);
	    	    
    	        if (strcmp (myCurrentStringValue, myDialogStringValue) == 0)
    	        {
    	    	    *pmFlag &= ~stPropertiesInitializer [cnt].changeBit;
    	        }
    	        else
    	        {
    	    	    *pmFlag |= stPropertiesInitializer [cnt].changeBit;
    	        }
	    } 
	    else if (stPropertiesInitializer [cnt].kind == INT_PROP)
	    {
		// TW Handle later, error now
	    }
	    else if (stPropertiesInitializer [cnt].kind == BOOL_PROP)
	    {
    	        myCurrentBoolValue = * (BOOL *) (((char *) &gProperties) + 
    	            stPropertiesInitializer [cnt].offset);
    	        myDialogBoolValue = MyCheckedToBool (pmDialog, pmDialogPart);
    	        
    	        if (myCurrentBoolValue == myDialogBoolValue)
    	        {
    	    	    *pmFlag &= ~stPropertiesInitializer [cnt].changeBit;
    	        }
    	        else
    	        {
    	    	    *pmFlag |= stPropertiesInitializer [cnt].changeBit;
    	        }
    	    }
    	
    	    return;
    	}
    }
    
    // TW No match!
} // MyAdjustChangeFlag


/************************************************************************/
/* MyCanWritePrefs							*/
/*									*/
/* This determines whether the user has pref-writing privs.  It does so */
/* by trying to open the properties file for writing (must previously   */
/* exist).  If the file exists and could be opened, then return TRUE, 	*/
/* if it exists and we couldn't open it, then return FALSE.  If it 	*/
/* doesn't exist, then try again opening it for write (may create).	*/
/* If it is successful, then delete the file and return TRUE.		*/
/************************************************************************/
static BOOL	MyCanWritePrefs (BOOL pmAdminPrefs, int *myErrorCode)
{
    FilePath	myTempPathName;
    HANDLE	myFile;
    ErrorBuffer	myErrorBuffer;

    // Create the path name
    if (pmAdminPrefs)
    {
    	EdFile_ConstructPath (myTempPathName, IDS_INI_DIR, ADMIN_PREF);
    }
    else
    {    	
    	EdFile_ConstructPath (myTempPathName, FROM_STARTUP, USER_PREF);
    }    	
    
    // Open the file
    myFile = CreateFile (myTempPathName, GENERIC_WRITE, 
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
    	OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    	
    // If it exists, return true
    if (myFile != (HANDLE) INVALID_HANDLE_VALUE)
    {
    	if (CloseHandle (myFile) == 0)
    	{
	    EdGUI_Message1 (NULL, 0, IDS_PREF_ERROR_TITLE, 
		IDS_CANT_CLOSE_FILE, myTempPathName, 
		EdFail_GetErrorMessage (myErrorBuffer), GetLastError ());
	}
    	return TRUE;
    }
    
    // It failed.  If it failed because the file doesn't exist,
    // then keep going, otherwise return false
    if (GetLastError () != ERROR_FILE_NOT_FOUND)
    {
    	*myErrorCode = GetLastError ();
    	return FALSE;
    }
    	
    myFile = CreateFile (myTempPathName, GENERIC_WRITE, 0, NULL,
    	CREATE_NEW, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_DELETE_ON_CLOSE, NULL);
    if (myFile == (HANDLE) INVALID_HANDLE_VALUE)
    {
    	*myErrorCode = GetLastError ();
    	return FALSE;
    }
    
    if (CloseHandle (myFile) == 0)
    {
	EdGUI_Message1 (NULL, 0, IDS_PREF_ERROR_TITLE, 
	    IDS_CANT_CLOSE_FILE, myTempPathName, 
	    EdFail_GetErrorMessage (myErrorBuffer), GetLastError ());
    }

    return TRUE;
} // MyCanWritePrefs


/************************************************************************/
/* MyCheckedToBool							*/
/************************************************************************/
static BOOL	MyCheckedToBool (HWND pmDialog, int pmCheckBox)
{
    return (IsDlgButtonChecked (pmDialog, pmCheckBox) == BST_CHECKED);
} // MyCheckedToBool


/************************************************************************/
/* MyClearJavaFileAssoc							*/
/************************************************************************/
static void	MyClearJavaFileAssoc (HWND pmDialog)
{
    int	myResult;
    
    myResult = RegDeleteKey (HKEY_CLASSES_ROOT, ".java");
    if (myResult != 0)
    {
    	ErrorBuffer	myErrorBuffer;
    	
	EdGUI_Message1 (NULL, 0, IDS_PREF_ERROR_TITLE, 
	    IDS_CANT_CLEAR_FILE_ASSOC, 
	    EdFail_GetErrorMessage1 (myErrorBuffer, myResult), myResult);
    }
    else
    {
    	SHChangeNotify (SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
    	MyDisplayFileAssoc (pmDialog);
    }
} // MyClearJavaFileAssoc


/************************************************************************/
/* MyConvertToAppProperties						*/
/*									*/
/* Note, we have converted to a singlestructure holding all the		*/
/* properties, so much of this is probably redundant.			*/
/************************************************************************/
static void	MyConvertToAppProperties (Ed_Properties *myAppProperties)
{
    EdDispOnly_Properties	*myDispProp;
//    EdGUI_Properties		*myGUIProp;
    EdPrint_Properties		*myPrintProp;
    EdRec_Properties		*myRecProp;
    EdStat_Properties		*myStatProp;
    EdTextOnly_Properties	*myTextProp;
    EdBBar_Properties		*myTopBProp;
    EdWinOnly_Properties	*myWinProp;    

    if ((gProperties.editCaretWidth < 1) || 
	(gProperties.editCaretWidth > 3))
    {
	gProperties.editCaretWidth = 2;
    }

    // Ed properties
    				
    // EdCon properties (TW Adjust for run window
	
    // EdDDE properties
    
    // EdDisp properties
    myDispProp = &myAppProperties -> winProperties.dispProperties.myProperties;
    myDispProp -> displayFullPathNames = gProperties.useFullPathNames;
    myDispProp -> syntaxColour = gProperties.editSyntaxColouring;
    myDispProp -> addSuffix = gProperties.addSuffixToFilesWithNone;
    myDispProp -> textFontSize = gProperties.editFontSize;
    strcpy (myDispProp -> textFontName, gProperties.editFontName);
    myDispProp -> caretWidth = gProperties.editCaretWidth;
    myDispProp -> findUsesSelection = gProperties.findUsesSelection;
    // Debugging properties
    myDispProp -> debugWindowAreas = gProperties.debugWindowAreas;
    myDispProp -> logging = gProperties.logging;
    myDispProp -> logWindowMessages = gProperties.logWindowMessages;
    
    // EdFile properties

    // EdGUI properties
    
    // EdJava properties
    
    // EdLog properties

    // EdMenu properties
        
    // EdPrint properties
    myPrintProp = 
        &myAppProperties -> winProperties.dispProperties.printProperties;
    strcpy (myPrintProp -> fontName, gProperties.printFontName);
    myPrintProp -> fontSize = gProperties.printFontSize;
    myPrintProp -> boldKeywords = gProperties.printBoldKeywords;
    myPrintProp -> italicizeIdents = gProperties.printItalicizeIdents;
    myPrintProp -> pageHeader = gProperties.printPageHeader;
    strcpy (myPrintProp -> userName, gProperties.userName);
    myPrintProp -> printUserName = gProperties.printUserName;
    myPrintProp -> lineNumbers = gProperties.printLineNumbers;
    strcpy (myPrintProp -> headerFontName, gProperties.printHeaderFontName);
    myPrintProp -> headerFontSize = gProperties.printHeaderFontSize;
    myPrintProp -> border = gProperties.printBorder;
    myPrintProp -> margins.left = gProperties.printMarginLeft;
    myPrintProp -> margins.right = gProperties.printMarginRight;
    myPrintProp -> margins.top = gProperties.printMarginTop;
    myPrintProp -> margins.bottom = gProperties.printMarginBottom;
    myPrintProp -> twoUp = gProperties.printTwoUp;
    myPrintProp -> fullPathName = gProperties.useFullPathNames;

    // EdRec properties
    myRecProp = &myAppProperties -> winProperties.recProperties;
    myRecProp -> useFullPathNames = gProperties.useFullPathNames;
    myRecProp -> numFilesToDisplay = gProperties.numRecentFiles;
    myRecProp -> noRecentFiles = gProperties.noRecentFiles;
    myRecProp -> useLastDirectory = gProperties.useLastDirectory;
    myRecProp -> useMyDocuments = gProperties.useMyDocuments;

    // EdSearch properties
    
    // EdSplash properties
    // Debugging properties

    // EdStat properties
    myStatProp = &myAppProperties -> winProperties.statProperties;
    myStatProp -> oneWindowMode = gProperties.oneWindowMode;
        
    // EdText properties
    myTextProp = 
        &myAppProperties -> 
            winProperties.dispProperties.textProperties.myProperties;
    myTextProp -> numSpacesPerTab = gProperties.editNumSpacesPerTab;
    myTextProp -> indentSize = gProperties.editIndentSize;
    myTextProp -> verifyLineDB = gProperties.debug;
        
    // EdBBar properties
    myTopBProp = &myAppProperties -> winProperties.topbProperties;
    myTopBProp -> oneWindowMode = gProperties.oneWindowMode;
    
    // EdWin properties
    myWinProp = &myAppProperties -> winProperties.myProperties;
    myWinProp -> beginnerMode = gProperties.beginnerMode;
    myWinProp -> oneWindowMode = gProperties.oneWindowMode;
    myWinProp -> beginnerFullScreen = gProperties.editBeginnerFullScreen;
    myWinProp -> advancedFullScreen = gProperties.editAdvancedFullScreen;
    myWinProp -> textRows = gProperties.editTextRows;
    myWinProp -> textCols = gProperties.editTextCols;
    myWinProp -> tabMargin = gProperties.editOneWindowMargin;
    // Debugging properties
    myWinProp -> logging = gProperties.logging;
    myWinProp -> logWindowMessages = gProperties.logWindowMessages;
} // MyConvertToAppProperties


/************************************************************************/
/* MyDialogToProperty							*/
/************************************************************************/
static void	MyDialogToProperty (HWND pmDialog, int pmDialogPart)
{
    char	*myPropertyStringValue, myDialogStringValue [PROP_STRING_SIZE];
    int		*myPropertyIntValue;
    BOOL	*myPropertyBoolValue;
    int		cnt;
   
    for (cnt = 0 ; cnt < stNumProperties ; cnt++)
    {
    	if (stPropertiesInitializer [cnt].dialogPart == pmDialogPart)
    	{
    	    if (stPropertiesInitializer [cnt].kind == STRING_PROP)
    	    {
    	        myPropertyStringValue = ((char *) & gProperties) + 
    	            stPropertiesInitializer [cnt].offset;
	    	GetDlgItemText (pmDialog, pmDialogPart, myPropertyStringValue, 
	    	    PROP_STRING_SIZE);	    	    
	    } 
	    else if (stPropertiesInitializer [cnt].kind == INT_PROP)
	    {
    	        myPropertyIntValue = (int *) (((char *) & gProperties) + 
    	            stPropertiesInitializer [cnt].offset);
	    	GetDlgItemText (pmDialog, pmDialogPart, myDialogStringValue, 
	    	    PROP_STRING_SIZE);
	    	*myPropertyIntValue = atoi (myDialogStringValue);    	    
	    }
	    else if (stPropertiesInitializer [cnt].kind == BOOL_PROP)
	    {
    	        myPropertyBoolValue = (BOOL *) (((char *) &gProperties) + 
    	            stPropertiesInitializer [cnt].offset);
    	        *myPropertyBoolValue = MyCheckedToBool (pmDialog, pmDialogPart);
    	    }
    	
    	    return;
    	}
    }
    
    // TW No match!
} // MyDialogToProperty


/************************************************************************/
/* MyDialogToMargin							*/
/************************************************************************/
static BOOL	MyDialogToMargin (HWND pmDialog, int pmDialogPart, int *pmValue)
{
    char	*myPtr, myDialogStringValue [PROP_STRING_SIZE];
    float	myFloatValue;
    int		myIntValue;
    
    GetDlgItemText (pmDialog, pmDialogPart, myDialogStringValue, 
	    	    PROP_STRING_SIZE);
    myPtr = &myDialogStringValue [strlen (myDialogStringValue)];
    myPtr--;
    // Get rid of trailing spaces
    while (*myPtr == ' ')
    {
    	*myPtr = 0;
    	myPtr--;
    }
    // Get rid of trailing inch mark
    if (*myPtr == '"')    
    {
    	*myPtr = 0;
    	myPtr--;
    }
    // Get rid of trailing spaces
    while (*myPtr == ' ')
    {
    	*myPtr = 0;
    	myPtr--;
    }
    myPtr = myDialogStringValue;
    while (*myPtr == ' ')
    {
    	myPtr++;
    }
    sscanf (myDialogStringValue, "%f", &myFloatValue);
    myIntValue = (int) (myFloatValue * 1000 + 0.5);
    if ((250 <= myIntValue) && (myIntValue <= 3000))
    {
    	*pmValue = myIntValue;
    	return TRUE;
    }
    
    return FALSE;
} // MyDialogToMargin


/************************************************************************/
/* MyDisplayFileAssoc							*/
/************************************************************************/
static void	MyDisplayFileAssoc (HWND pmDialog)
{
    char	*myFileAssociation;
    
    // Set, enable and disable the File Association Buttons	    
    myFileAssociation = EdReg_GetCurrentFileAssoc (pmDialog, stDebugPrefs);
    if (myFileAssociation == NULL)
    {
    	SetDlgItemText (pmDialog, PREF_OPENEDWITH, "No Associated Application");
    }
    else if (myFileAssociation == ASSOC_WITH_READY)
    {
    	if (gProgram.isTuring)
    	{
    	    SetDlgItemText (pmDialog, PREF_OPENEDWITH, "Turing");
	}
	else
	{    	    		    
    	    SetDlgItemText (pmDialog, PREF_OPENEDWITH, 
    	    		    "Ready to Program with Java");
	}    	    		    
    }
    else
    {
   	SetDlgItemText (pmDialog, PREF_OPENEDWITH, myFileAssociation);
   	free (myFileAssociation);
    }

    // Set, enable and disable the File Association Buttons	    
    myFileAssociation = EdReg_GetPreviousFileAssoc (pmDialog, stDebugPrefs);
    if (myFileAssociation == NULL)
    {
    	SetDlgItemText (pmDialog, PREF_WASOPENEDWITH, 
    			"No Associated Application");
    }
    else
    {
   	SetDlgItemText (pmDialog, PREF_WASOPENEDWITH, myFileAssociation);
   	free (myFileAssociation);
    }
} // MyDisplayFileAssoc


/************************************************************************/
/* MyMarginToDialog							*/
/************************************************************************/
static void	MyMarginToDialog (HWND pmDialog, int pmDialogPart, int pmValue)
{
    char	*myPtr, myString [PROP_STRING_SIZE];
   
    sprintf (myString, "%.3f", (double) pmValue / 1000);
    
    // Eliminate trailing zeros
    myPtr = &myString [strlen (myString)];
    myPtr--;
    if (*myPtr == '0')
    {
    	*myPtr = 0;
    	myPtr--;
    }
    if (*myPtr == '0')
    {
    	*myPtr = 0;
    	myPtr--;
    }
    if (*myPtr == '0')
    {
    	*myPtr = 0;
    	myPtr--;
    	if (*myPtr == '.')
    	{
    	    *myPtr = 0;
    	}
    }
    
    // Add inch mark
    strcat (myString, "\"");
    
    SetDlgItemText (pmDialog, pmDialogPart, myString);
} // MyMarginToDialog


/************************************************************************/
/* MyPropertyToDialog							*/
/************************************************************************/
static void	MyPropertyToDialog (HWND pmDialog, int pmDialogPart)
{
    char	*myPropertyStringValue, myString [PROP_STRING_SIZE];
    int		myPropertyIntValue;
    BOOL	myPropertyBoolValue;
    int		cnt;
   
    for (cnt = 0 ; cnt < stNumProperties ; cnt++)
    {
    	if (stPropertiesInitializer [cnt].dialogPart == pmDialogPart)
    	{
    	    if (stPropertiesInitializer [cnt].kind == STRING_PROP)
    	    {
    	        myPropertyStringValue = ((char *) & gProperties) + 
    	            stPropertiesInitializer [cnt].offset;
	    	SetDlgItemText (pmDialog, pmDialogPart, myPropertyStringValue);
	    } 
	    else if (stPropertiesInitializer [cnt].kind == INT_PROP)
	    {
    	        myPropertyIntValue = * (int *) (((char *) & gProperties) + 
    	            stPropertiesInitializer [cnt].offset);
	    	wsprintf (myString, "%d", myPropertyIntValue);
	    	SetDlgItemText (pmDialog, pmDialogPart, myString);
	    }
	    else if (stPropertiesInitializer [cnt].kind == BOOL_PROP)
	    {
    	        myPropertyBoolValue = 
    	            * (BOOL *) (((char *) &gProperties) + 
    	            stPropertiesInitializer [cnt].offset);
	    	MySetCheckBox (pmDialog, pmDialogPart, myPropertyBoolValue);
    	    }
    	
    	    return;
    	}
    }
    
    // TW No match!
} // MyPropertyToDialog


/************************************************************************/
/* MyReadPropertiesFromDirectory					*/
/************************************************************************/
static void	MyReadPropertiesFromDirectory (Properties *pmProp, 
	    				       Properties *pmBaseProp, 
    					       int pmDirectoryResource,
    					       int pmFileNameResource)
{
    FilePath	myPropertyPathName;
    char	*myLine, *myPtr, *myStart;
    char	*myBuffer;
    DWORD	myFileSize;
    char	myProperty [1024], myPropertyValue [1024];
    int		myLineNumber = 0;
    int		cnt;
    BOOL	myMatch;
    	    
    *pmProp = *pmBaseProp;
    EdFile_ConstructPath (myPropertyPathName, pmDirectoryResource, 
        pmFileNameResource);
        
    // Read file into buffer
    myBuffer = EdFile_ReadFileIntoBuffer (NULL, myPropertyPathName, 
    					  &myFileSize, OK_IF_NOT_FOUND);
    if (myBuffer == NULL)
    {
    	return;
    }
    
    EdFile_ReadLineFromBuffer (myBuffer, myFileSize, READ_FIRST);
    while (TRUE)
    {
    	// Read a line from the file
    	myLine = EdFile_ReadLineFromBuffer (myBuffer, 0, READ_NEXT);
    	if (myLine == NULL)
    	{
    	    break;
    	}
    	myLineNumber++;
    	
    	myPtr = myLine;
    	
    	// Skip the whitespace
    	while ((*myPtr == ' ') || (*myPtr == '\t'))
    	{
    	    myPtr++;
    	}
    	
    	// Check for section header, comment or blank line
    	if ((*myPtr == '[') || (*myPtr == ';') || (*myPtr == '#') || 
    	    (*myPtr == 0))
    	{
    	    continue;
    	}
    	
    	// Read the property
    	myStart = myPtr;
    	while (isalnum (*myPtr))
    	{
    	    myPtr++;
    	}
    	strncpy (myProperty, myStart, myPtr - myStart);
    	myProperty [myPtr - myStart] = 0;
    	
    	// Skip the whitespace
    	while ((*myPtr == ' ') || (*myPtr == '\t'))
    	{
    	    myPtr++;
    	}

	// If an '=' doesn't follow the property name, give an error    	
    	if (*myPtr != '=')
    	{
    	    if (*myPtr == 0)
    	    {
		EdGUI_Message1 (NULL, 0, IDS_PREF_ERROR_TITLE, 
		    IDS_BAD_PREF1, myPropertyPathName, myLineNumber, 
		    myLine);
    	    }
    	    else
    	    {
		EdGUI_Message1 (NULL, 0, IDS_PREF_ERROR_TITLE, 
		    IDS_BAD_PREF1, myPropertyPathName, myLineNumber, 
		    myLine);
    	    }
    	
	    *pmProp = *pmBaseProp;
    	    return;
    	}

	// Skip the equal sign
	myPtr++;    	    	
	    
    	// Skip the whitespace
    	while ((*myPtr == ' ') || (*myPtr == '\t'))
    	{
    	    myPtr++;
    	}
    
    	// Read the property value
    	myStart = myPtr;
    	while ((*myPtr != 0) && (*myPtr != ';'))
    	{
    	    myPtr++;
    	}
    	
    	// Skip any trailing whitespace
    	myPtr--;
    	while ((*myPtr == '\t') || (*myPtr == ' '))
    	{
    	    myPtr--;
    	}
    	myPtr++;
    	
    	// Copy the property value
    	strncpy (myPropertyValue, myStart, myPtr - myStart);
    	myPropertyValue [myPtr - myStart] = 0;
    	
    	myMatch = FALSE;
    	
    	// Find the property value
    	for (cnt = 0 ; cnt < stNumProperties ; cnt++)
    	{
    	    if (_stricmp (myProperty, stPropertiesInitializer [cnt].name) == 0)
    	    {
    	    	myMatch = TRUE;
    	    	
    	    	// Allow for "<propertyName>=" with no value
    	    	if (strlen (myPropertyValue) == 0)
    	    	{
    	    	    break;
    	    	}
    	    	
    	    	if (stPropertiesInitializer [cnt].kind == STRING_PROP)
    	    	{
    	    	    myPtr = ((char *) pmProp) + 
    	    	        stPropertiesInitializer [cnt].offset;
    	    	    strcpy (myPtr, myPropertyValue);
    	    	}
    	    	else if (stPropertiesInitializer [cnt].kind == INT_PROP)
    	    	{
	    	    int *myIntPtr = (int *) (((char *) pmProp) + 
	    	        stPropertiesInitializer [cnt].offset);
	    	    int	myNum;
	    	    
	    	    if (EdGlob_ConvertStringToNum (myPropertyValue, &myNum))
	    	    {
	    	        *myIntPtr = myNum;
	    	    }
	    	    else
	    	    {
			EdGUI_Message1 (NULL, 0, IDS_PREF_ERROR_TITLE, 
			    IDS_PREF_NEEDS_INT, myPropertyPathName, 
			    myLineNumber, myLine);
    	    	    }
    	    	}
    	    	else if (stPropertiesInitializer [cnt].kind == BOOL_PROP)
    	    	{
	    	    int *myIntPtr = (int *) (((char *) pmProp) + 
	    	        stPropertiesInitializer [cnt].offset);
	    	    if (_stricmp (myPropertyValue, "true") == 0)
	    	    {
	    	        *myIntPtr = 1;
	    	    }
	    	    else if (_stricmp (myPropertyValue, "false") == 0)
	    	    {
	    	        *myIntPtr = 0;
	    	    }
	    	    else
	    	    {
			EdGUI_Message1 (NULL, 0, IDS_PREF_ERROR_TITLE, 
			    IDS_PREF_NEEDS_BOOL, myPropertyPathName, 
			    myLineNumber, myLine);
    	    	    }
    	    	} // if kind
    	    	break;
    	    } // if match
    	} // for
    	
    	if (!myMatch)
    	{    
    	    // Failed to match any known properties
	    EdGUI_Message1 (NULL, 0, IDS_PREF_ERROR_TITLE, 
	        IDS_UNKNOWN_PROPERTY, myPropertyPathName, myLineNumber, 
		myLine);
    	} // if (!myMatch)
    } // while
    free (myBuffer); 
} // MyReadPropertiesFromDirectory


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


/************************************************************************/
/* MySetDefaultProperties						*/
/************************************************************************/
static void	MySetDefaultProperties (Properties *pmProp)
{
    int			cnt;
    
    for (cnt = 0 ; cnt < stNumProperties ; cnt++)
    {
    	if (stPropertiesInitializer [cnt].kind == STRING_PROP)
    	{
    	    char *ptr = ((char *) pmProp) + 
    	        stPropertiesInitializer [cnt].offset;
    	    strcpy (ptr, stPropertiesInitializer [cnt].defaultStringValue);
	} 
	else if ((stPropertiesInitializer [cnt].kind == INT_PROP) ||
	         (stPropertiesInitializer [cnt].kind == BOOL_PROP))
	{
    	    int *ptr = (int *) (((char *) pmProp) + 
    	        stPropertiesInitializer [cnt].offset);
    	    *ptr = stPropertiesInitializer [cnt].defaultIntValue;
	}
    }
} // MySetDefaultProperties

    	
/************************************************************************/
/* MySetStartupDir							*/
/************************************************************************/
static void	MySetStartupDir (Properties *pmProp)
{
    FilePath	myStartupPath;
    FilePath	myEnvironmentVariable;
    FilePath	myValue;
    int		myPathIndex = 0, myVarIndex;
    ErrorBuffer	myError;
    UINT	cnt;
    
    if (pmProp -> useMyDocuments)
    {
    	if (EdFile_GetWindowsFolder (CSIDL_PERSONAL, myStartupPath, TRUE) == 0)
    	{
	    if (SetCurrentDirectory (myStartupPath) != 0)
	    {
	    	return;
	    }

	    EdGUI_Message1 (NULL, 0, IDS_STARTUP_ERROR_TITLE, 
	    	IDS_CANT_CHANGE_STARTUP_DIR, myStartupPath,
	    	pmProp -> startupDirectory, EdFail_GetErrorMessage (myError));
    	}
    }

    if (pmProp -> startupDirectory [0] == 0)
    {
    	return;
    }
    
    for (cnt = 0 ; cnt < strlen (pmProp -> startupDirectory) ; cnt++)
    {
    	if (pmProp -> startupDirectory [cnt] != '%')
    	{
    	    myStartupPath [myPathIndex++] = pmProp -> startupDirectory [cnt];
    	}
    	else
    	{
    	    cnt++;
    	    myVarIndex = 0;
    	    while ((cnt < strlen (pmProp -> startupDirectory)) &&
    	           (pmProp -> startupDirectory [cnt] != '%'))
    	    {
    	    	myEnvironmentVariable [myVarIndex++] = 
    	    	    pmProp -> startupDirectory [cnt++];
    	    }
    	    myEnvironmentVariable [myVarIndex] = 0;
    	    if (cnt == strlen (pmProp -> startupDirectory))
    	    {
	    	EdGUI_Message1 (NULL, 0, IDS_STARTUP_ERROR_TITLE, 
	    	    IDS_UNTERMINATED_ENV_VAR, myEnvironmentVariable,
	    	    pmProp -> startupDirectory);
	    	return;
    	    }
    	    if (GetEnvironmentVariable (myEnvironmentVariable, myValue, 
    	                                PATHNAME_LENGTH) == 0)
    	    {
	    	EdGUI_Message1 (NULL, 0, IDS_STARTUP_ERROR_TITLE, 
	    	    IDS_CANT_FIND_ENVIRONMENT_VAR, myEnvironmentVariable,
	    	    pmProp -> startupDirectory);
	    	return;
    	    }
    	    else
    	    {
	        myStartupPath [myPathIndex] = 0;
    	    	strcat (myStartupPath, myValue);
    	    	myPathIndex += strlen (myValue);
    	    }
    	}
    }
    myStartupPath [myPathIndex] = 0;
    
    if (SetCurrentDirectory (myStartupPath) == 0)
    {
    	EdGUI_Message1 (NULL, 0, IDS_STARTUP_ERROR_TITLE, 
    	    IDS_CANT_CHANGE_STARTUP_DIR, myStartupPath,
    	    pmProp -> startupDirectory, EdFail_GetErrorMessage (myError));
    }
} // MySetStartupDir

