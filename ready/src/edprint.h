/*************/
/* edprint.h */
/*************/

#ifndef _EDPRINT_H_
#define _EDPRINT_H_

/*******************/
/* System includes */
/*******************/

/******************/
/* Other includes */
/******************/
#include "edglob.h"
#include "edprog.h"
#include "edtext.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
// Parameters for EdPrint_PrintEditorWindow
#define PROGRAM_FILE		FALSE
#define OUTPUT_FILE		TRUE

#define NO_SYNTAX_COLOUR	FALSE

/*********/
/* Types */
/*********/
typedef struct
{
    char	fontName [PROPERTY_LENGTH];
    int		fontSize;
    BOOL	boldKeywords;
    BOOL	italicizeIdents;
    BOOL	pageHeader;
    char	userName [PROPERTY_LENGTH];
    BOOL	printUserName;
    BOOL	lineNumbers;
    char	headerFontName [PROPERTY_LENGTH];
    int		headerFontSize;
    BOOL	border;
    RECT	margins;
    BOOL	twoUp;
    BOOL	fullPathName;
} EdPrint_Properties;

/***********************/
/* External procedures */
/***********************/
// Init, Finalize subprograms
extern BOOL	EdPrint_Init (void);
extern void	EdPrint_Finalize (void);
// Properties subprograms
extern void	EdPrint_PropertiesSet (EdPrint_Properties pmProperties);
extern void	EdPrint_PropertiesImplementChanges (void);
// Other subprograms
extern void	EdPrint_PageSetup (HWND pmWindow);
extern void	EdPrint_PrintEditorWindow (HWND pmWindow, TextPtr pmTextPtr, 
					   const char *pmPathName, 
					   int pmUnnamedNumber, 
				   	   BOOL pmIsExample, 
				   	   const char *pmExampleName,
					   BOOL pmLangFile, BOOL pmOutput);
extern void	EdPrint_PrintGraphics (HWND pmEditWindow, HBITMAP pmBitMap, 
			       int pmWidth, int pmHeight, HPALETTE pmPalette, 
			       int pmNormalWidth, BOOL pmDisplayPrintDialog, 
			       BOOL pmDrawBorder, BOOL pmScaleToFit, 
			       BOOL pmDrawHeader, char *pmTitle);
extern void	EdPrint_PrintOutput (HWND pmWindow, char *pmText, int pmTextLen, 
				     const char *pmClassName);

#endif // #ifndef _EDPRINT_H_
