/************/
/* edprog.h */
/************/

#ifndef _EDPROG_H_
#define _EDPROG_H_

/*******************/
/* System includes */
/*******************/
#include <windows.h>

/******************/
/* Other includes */
/******************/
#include "edfile.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
// All Turing editor windows define 12 bytes of extra data as follows
#define	WINDOW_MEM_WINDOW_TYPE		 0
#define	WINDOW_MEM_WINDOW_INITIALIZED	 4
#define WINDOW_MEM_WINDOW_INFO		 8

#define WINDOW_EXTRA_MEMORY		12

// These are all the type of windows defined in the Turing editor
#define	WINDOW_TYPE_EDIT		101
#define WINDOW_TYPE_STATUS		102
#define WINDOW_TYPE_TEXT_DISPLAY	103
#define WINDOW_TYPE_BEGINNER_TOP	104
#define WINDOW_TYPE_SINGLE_WIN_TOP	105
#define WINDOW_TYPE_MULTI_WIN_TOP	106
#define WINDOW_TYPE_SPLASH		107
#define WINDOW_TYPE_CONSOLE		108
#define WINDOW_TYPE_CONSOLE_TEXT	109

// All string formatted properties in Turing have a maximum length
#define PROPERTY_LENGTH			1024
#define PROP_STRING_SIZE		 128
#define PROP_LONG_STRING_SIZE		1024

// The operating system
#define UNKNOWN_OS			 300
#define WIN_95				 301
#define WIN_95_OSR2			 302
#define WIN_98				 303
#define WIN_98_SE			 304
#define WIN_ME				 305
#define WIN_NT				 306
#define WIN_2000			 307
#define WIN_XP				 308
#define WIN_NEWER			 400

// The type of Java used
#define JRE_131				 501
#define JRE_BUILT_IN			 502
#define JRE_IN_JRE_DIR			 503
#define JDK_IN_JRE_DIR			 504
#define JRE_IN_REGISTRY			 505
#define JDK_IN_REGISTRY			 506

/*********/
/* Types */
/*********/
typedef struct Globals
{
    HINSTANCE	applicationInstance;
    BOOL	globalsInitialized;
    BOOL	isTuring;
    BOOL	isJava;
    BOOL	isCpp;
    
    int		language;
    char	environmentName [80];
    int		installKind;
    BOOL	miniVersion;
    BOOL	restrictedVersion;
    BOOL	assistedByIBM;
    char	expiryDateString [80];
    FILETIME	expiryDate;
    char	licensedTo [140];
    
    char	versionNumber [20];

    int		operatingSystem;
    char	servicePack [140];
    
    HWND	globalWindow;
} Globals;

typedef struct Properties
{
    BOOL	propertiesInitialized;
    
    // Admin section
    FilePath	startupDirectory;
    BOOL	useMyDocuments;
    BOOL	studentsCanChangeUserName;
    BOOL	studentCanEnterAdvancedMode;
    BOOL	studentRestrictedToStartupDir;
    BOOL	studentDrivesAllowed;
    char	studentDrivesAllowedList [27];
    BOOL	studentDrivesForbidden;
    char	studentDrivesForbiddenList [27];
    BOOL	studentCantChangePrefs;
    
    // General section
    BOOL	beginnerMode;
    BOOL	oneWindowMode;
    BOOL	useFullPathNames;
    BOOL	editSaveBackup;
    int		editIndentSize;
    BOOL	confirmQuit;
    BOOL	skipSplashScreen;
    BOOL	addSuffixToFilesWithNone;
    BOOL	editLeadingSpacesToTabs;
    BOOL	editOpenBraceOnSameLine;
    int		editNumSpacesPerTab;
    BOOL	noRecentFiles;
    BOOL	useLastDirectory;
    BOOL	findUsesSelection;
    BOOL	displayDebuggerMenu;
    BOOL	displayWindowMenu;
        
    // Appearance section
    char	editFontName [PROP_STRING_SIZE];
    int		editFontSize;
    BOOL	editSyntaxColouring;
    BOOL	editBeginnerFullScreen;
    BOOL	editAdvancedFullScreen;
    int		editTextRows;
    int		editTextCols;
    // editCaretWidth: 1 = 1 pixel wide, 2 = 2 pixel wide, 3 = char wide
    int		editCaretWidth;
    BOOL	debuggerMenu;
    BOOL	windowMenu;
    
    // Run section
    BOOL	useGraphicsMode;
    char	runConsoleFontName [PROP_STRING_SIZE];
    int		runConsoleFontSize;
    BOOL	runConsoleFullScreen;
    int		runConsoleTextRows;
    int		runConsoleTextCols;
    int		runAppletWidth;
    int		runAppletHeight;
    BOOL	runUseSmallFont;
    
    // Java section
    char	additionalClassPath [PROP_LONG_STRING_SIZE];
    char	jikesOptions [PROP_LONG_STRING_SIZE];
    BOOL	startJavaRunnerInDebugger;
    int		memoryForJVMInMB;
    BOOL	oldJavaCompile;	// TRUE = Target Java 1.3 = No asserts
    int		JVMType;	// Only partially supported
    char	versionInRegistry [20];
    
    // Turing section
    int		turingStackSizeInKB;
    int		executionDelay;
    int		parallelIOPort;
    BOOL	prohibitSysExec;
    BOOL	noSound;

    // Print section
    char	printFontName [PROP_STRING_SIZE];
    char	userName [PROP_STRING_SIZE];
    int		printFontSize;
    BOOL	printBoldKeywords;
    BOOL	printItalicizeIdents;
    BOOL	printPageHeader;
    BOOL	printUserName;
    BOOL	printLineNumbers;
    char	printHeaderFontName [PROP_STRING_SIZE];
    int		printHeaderFontSize;
    BOOL	printBorder;
    int		printMarginLeft, printMarginRight;
    int		printMarginTop, printMarginBottom;
    BOOL	printTwoUp;
    
    // Special section
    int		numRecentFiles;			// Not in dialog
    int		editOneWindowMargin;		// Not in dialog
    BOOL	catchExceptions;		// Not in dialog
    BOOL	debuggerAvailable;		// Not in dialog
    
    // Debug section
    BOOL	debug;			// Verifies line DB
    BOOL	debugWindowAreas;	// Colours window areas
    BOOL	logging;		// Writes a log
    BOOL	logWindowMessages;	// Logs messages to windows
    // Normally Turing maps stream 0 to stdout instead of stderr!
    BOOL	stream0ToStderr;	// Output to stream 0 to stderr
    BOOL	errorsToStderr;		// Library errors to stderr
    BOOL	infoToStderr;		// Library info to stderr
} Properties;

/**********************/
/* External variables */
/**********************/
extern Globals		gProgram;
extern Properties	gProperties;

/***********************/
/* External procedures */
/***********************/
extern BOOL	EdProg_Init (void);

#endif // #ifndef _EDPROG_H_
