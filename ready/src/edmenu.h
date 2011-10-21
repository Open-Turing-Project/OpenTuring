/************/
/* edmenu.h */
/************/

#ifndef _EDMENU_H_
#define _EDMENU_H_

/*******************/
/* System includes */
/*******************/

/******************/
/* Other includes */
/******************/

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
#define NEW_MENU_TEXT		TRUE
#define INSERT_MENU_TEXT	FALSE

// Used by EdMenu_GetMenu
#define FILE_MENU	0
#define EDIT_MENU	1
#define SEARCH_MENU	2
#define MARK_MENU	3
#define RUN_MENU	4
#define DEBUGGER_MENU	5
#define WINDOW_MENU	6
#define HELP_MENU	7

// Used by EdMenu_ShowHideDebuggerMenu/EdMenu_ShowHideWindowMenu
#define SHOW_MENU	101
#define HIDE_MENU	102
#define TOGGLE_MENU	103

/*********/
/* Types */
/*********/
typedef struct
{
    int	dummy;
} EdMenu_Properties;

/**********************/
/* External variables */
/**********************/

/***********************/
/* External procedures */
/***********************/
// Init, Finalize subprograms
extern BOOL	EdMenu_Init (void);
extern void	EdMenu_Finalize (void);
// Properties subprograms
extern void	EdMenu_PropertiesSet (EdMenu_Properties pmProperties);
extern void	EdMenu_PropertiesImplementChanges (void);
// Other subprograms
extern void	EdMenu_AddDebuggerMenuItem (HWND pmEditWindow);
extern void	EdMenu_AddDebuggerWindowMenuIfNecessary (HWND pmEditWindow);
extern void	EdMenu_AddHowToGetReadyMenuItem (HWND pmEditWindow);
extern void	EdMenu_AddWindowMenu (HWND pmEditWindow);
extern void	EdMenu_CreateNewInsertMenus (HMENU pmNewMenu, 
					     HMENU pmInsertMenu);
extern HMENU	EdMenu_GetMenu (HWND pmEditWindow, int pmMenu);
extern const char	*EdMenu_GetName (BOOL pmNewMenu, int pmSkeleton);
extern void	EdMenu_GetText (BOOL pmNewMenu, int pmSkeleton, char **pmText, 
				char **pmFileSuffix, DWORD *pmTextLen);
extern BOOL	EdMenu_IsDebuggerMenuVisible ();
extern void	EdMenu_ShowHideDebuggerMenu (int pmShowHideToggle);
extern void	EdMenu_ShowHideWindowMenu (int pmShowHideToggle);

#endif // #ifndef _EDMENU_H_
