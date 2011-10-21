/***********/
/* jstop.h */
/***********/

#ifndef _JSTOP_H_
#define _JSTOP_H_

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

/*********/
/* Types */
/*********/
typedef struct
{
    int 	dialogFontSize;
} JSTop_Properties;

/**********************/
/* External variables */
/**********************/

/***********************/
/* External procedures */
/***********************/
// Init, Finalize subprograms
extern void	JSTop_Init (void);
extern void	JSTop_Finalize (void);
// Properties subprograms
extern void	JSTop_PropertiesSet (JSTop_Properties pmProperties);
extern void	JSTop_PropertiesImplementChanges (HWND pmButtonBarWindow);
// Create, Dispose subprograms
extern HWND	JSTop_Create (HWND pmEditWindow);
// Other subprograms
extern int	JSTop_GetWindowHeight (void);
extern void	JSTop_DisablePauseButton (HWND pmButtonBarWindow);
extern void	JSTop_SwitchPauseToResume (HWND pmButtonBarWindow);
extern void	JSTop_SwitchResumeToPause (HWND pmButtonBarWindow);

#endif // #ifndef _JSTOP_H_
