/*************/
/* dbgctrl.h */
/*************/

#ifndef _DBGCTRL_H_
#define _DBGCTRL_H_

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
// Used in DbgControl_SetButtons
#define SET_BUTTON_NOT_RUNNING		41
#define SET_BUTTON_IS_EXECUTING		42
#define SET_BUTTON_PAUSED		43
#define SET_BUTTON_WAITING		44

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
extern BOOL	DbgCtrl_Init (void);
// Other subprograms
extern BOOL	DbgCtrl_IsControllerVisible (void);
extern void	DbgCtrl_SetButtons (int pmStatus);
extern void	DbgCtrl_SetStatus (const char *pmMessage, ...);
extern void	DbgCtrl_ToggleControllerVisibility (void);

#endif // #ifndef _DBGCTRL_H_
