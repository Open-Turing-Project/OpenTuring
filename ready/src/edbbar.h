/************/
/* edbbar.h */
/************/

#ifndef _EDBBAR_H_
#define _EDBBAR_H_

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
// For use by EdBBar_SetDebuggerVisible
#define HIDE_DEBUGGER_CONTROLS		13
#define SHOW_DEBUGGER_CONTROLS		14
#define TOGGLE_DEBUGGER_CONTROLS	15

/*********/
/* Types */
/*********/
typedef struct
{
    BOOL	oneWindowMode;
} EdBBar_Properties;

/**********************/
/* External variables */
/**********************/

/***********************/
/* External procedures */
/***********************/
// Init, Finalize subprograms
extern BOOL	EdBBar_Init (void);
extern void	EdBBar_Finalize (void);
// Properties subprograms
extern void	EdBBar_PropertiesSet (EdBBar_Properties pmProperties);
extern void	EdBBar_PropertiesImplementChanges (HWND pmButtonBarWindow);
// Create, Dispose subprograms
extern HWND	EdBBar_Create (HWND pmEditWindow);
// Other subprograms
extern BOOL	EdBBar_AreDebuggerControlsVisible (void);
extern void	EdBBar_SetButtons (HWND pmButtonBarWindow, int pmStatus);
extern void	EdBBar_SetDebuggerVisible (int pmVisible);
extern int	EdBBar_GetWindowHeight (void);

#endif // #ifndef _EDBBAR_H_
