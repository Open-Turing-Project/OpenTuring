/***********/
/* edgui.h */
/***********/

#ifndef _EDGUI_H_
#define _EDGUI_H_

/*******************/
/* System includes */
/*******************/

/******************/
/* Other includes */
/******************/
#ifndef CONSOLE
#include "edjava.h"
#endif // #ifndef CONSOLE

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
#ifndef CONSOLE
#define RAISED_AREA_CLASS_NAME		"Raise"
#endif // #ifndef CONSOLE	

#define	BOLD		TRUE
#define NO_BOLD		FALSE
#define	ITALIC		TRUE
#define NO_ITALIC	FALSE

#ifndef CONSOLE
    #define STAR_ALLOWED	TRUE
    #define ONLY_LETTERS	FALSE
#endif // #ifndef CONSOLE	

/*********/
/* Types */
/*********/

/**********************/
/* External variables */
/**********************/

/***********************/
/* External procedures */
/***********************/
// Init, Finalize subprograms
extern BOOL	EdGUI_Init (void);

#ifndef CONSOLE
    extern void	EdGUI_Finalize (void);
    // Properties subprograms
    extern void	EdGUI_PropertiesSet (void);
    extern void	EdGUI_PropertiesImplementChanges (void);
#endif // #ifndef CONSOLE	

// Other subprograms
extern void	EdGUI_CentreDialogBox (HWND pmDialog);

#ifndef CONSOLE
    extern BOOL	EdGUI_CtrlShiftKeysPressed (void);
    extern void	EdGUI_DlgLettersOnly (HWND pmDialog, int pmDialogPart, 
				      BOOL pmStarAllowed);
    extern void	EdGUI_DlgNumbersOnly (HWND pmDialog, int pmDialogPart);
#endif // #ifndef CONSOLE	

extern HFONT	EdGUI_CreateFont (HWND pmWindow, HDC pmDisplayContext, 
				  const char *pmFontName, int pmFontSize, 
				  BOOL pmBold, BOOL pmItalic);
extern HFONT	EdGUI_CreateFontAngle (HWND pmWindow, HDC pmDisplayContext, 
				       const char *pmFontName, int pmFontSize, 
				       BOOL pmBold, BOOL pmItalic, int pmAngle);
				       
#ifndef CONSOLE				  
    extern void	EdGUI_ErrorMessage (char *pmMessage, int pmErrorCode);
    extern BOOL	EdGUI_GetClassLibs (HWND pmWindow, char *pmPathName, 
				    char *pmTitle);
    extern int	EdGUI_GetColourDepth (void);				    
    extern int	EdGUI_GetCommonControlVersionNumber (void);
    extern char	*EdGUI_GetCurrentDate (char *pmDateBuffer);
    extern BOOL	EdGUI_GetDLLName (HWND pmWindow, char *pmPathName, 
				  char *pmTitle);
#endif // #ifndef CONSOLE	

extern int	EdGUI_GetMouseWheelMessage (void);
extern int	EdGUI_GetMouseWheelSettings (void);

#ifndef CONSOLE
    extern BOOL	EdGUI_GetOpenFile (HWND pmWindow, char *pmPathName);
    extern BOOL	EdGUI_GetRedirectInputFile (HWND pmWindow, char *pmPathName);
    extern BOOL	EdGUI_GetRedirectOutputFile (HWND pmWindow, char *pmPathName);
    extern BOOL	EdGUI_GetSaveEXEFile (HWND pmWindow, char *pmPathName);
#endif // #ifndef CONSOLE	

extern BOOL	EdGUI_GetSaveFile (HWND pmWindow, char *pmPathName, 
				   BOOL pmSelection);
#ifndef CONSOLE
    extern BOOL	EdGUI_GetSaveRTFFile (HWND pmWindow, char *pmPathName);
    extern long	EdGUI_GetTicks (void);
#endif // #ifndef CONSOLE	

extern HWND	EdGUI_GetTopWindow (HWND pmWindow);				   
extern RECT	EdGUI_GetWorkArea (void);
				       
#ifndef CONSOLE
    extern BOOL	EdGUI_IsAllowedAccess (HWND pmWindow, const char *pmPathName, 
				       BOOL pmIsSave);
    extern BOOL	EdGUI_JavaIdentifierOnly (HWND pmDialog, int pmDialogPart, 
					  int pmMessagePart);
    extern void	EdGUI_LoadFormattedString (int pmStringResource, char *pmString, 
					   int pmStringSize, ...);
    extern BOOL	EdGUI_LoadResource (HMODULE pmLibrary, int pmResourceName, 
				    void *pmData, DWORD pmSize);
#endif // #ifndef CONSOLE	

extern void	EdGUI_LoadString (int pmStringResource, char *pmString,
				  int pmStringSize);

#ifndef CONSOLE
    extern void	EdGUI_Message (char *pmTitle, char *pmMessage, ...);
#endif // #ifndef CONSOLE	

extern int	EdGUI_Message1 (HWND pmWindow, int pmMessageBoxParams, 
				int pmTitleStringResource, 
			        int pmMessageStringResource, ...);

#endif // #ifndef _EDGUI_H_
