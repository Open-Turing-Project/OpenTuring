/**************/
/* edsearch.h */
/**************/

#ifndef _EDSEARCH_H_
#define _EDSEARCH_H_

/*******************/
/* System includes */
/*******************/
#include <windows.h>

/******************/
/* Other includes */
/******************/
#include "edtext.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
#define		FOUND		1
#define 	WRAPPED		2
#define 	NOT_FOUND	3
#define		ONE_OCCURRENCE	4

#define		CLOSE_DIALOG	TRUE
#define		USER_CLOSED	FALSE

// Parameters for EdSearch_Find
#define SEARCH_FORWARD		TRUE
#define WRAP_ALLOWED		TRUE
#define NO_WRAP			FALSE

/*********/
/* Types */
/*********/
typedef struct
{
    int	dummy;
} EdSearch_Properties;

/**********************/
/* External variables */
/**********************/

/***********************/
/* External procedures */
/***********************/
// Init, Finalize subprograms
extern BOOL	EdSearch_Init (void);
extern void	EdSearch_Finalize (void);
// Properties subprograms
extern void	EdSearch_PropertiesSet (EdSearch_Properties pmProperties);
extern void	EdSearch_PropertiesImplementChanges (void);
// Other subprograms
extern UINT	EdSearch_FindReplaceMessage;
extern HWND	EdSearch_FindReplaceDialog;

extern void	EdSearch_CloseDialog (BOOL pmCloseWindowManually);
extern int	EdSearch_Find (TextPtr pmText, BOOL pmForward, BOOL pmWrap);
extern BOOL	EdSearch_FindDialogVisible (void);
extern int	EdSearch_FindSelectionString (TextPtr pmText, 
					      char *pmSelectionString);
extern int	EdSearch_InsertReplaceText (TextPtr pmTextPtr);
extern BOOL	EdSearch_IsFindStringEmpty (void);
extern void	EdSearch_RepositionReplaceDialog (HWND pmEditWindow, 
						  RECT pmSelectRect);
extern BOOL	EdSearch_SelectionMatchesFindWhat (TextPtr pmTextPtr);
extern void	EdSearch_SetDialogOwner (HWND pmWindow);
extern void	EdSearch_ShowFindDialog (HWND pmWindow, char *pmFindString);
extern void	EdSearch_ShowReplaceDialog (HWND pmWindow, char *pmFindString);

#endif // #ifndef _EDSEARCH_H_
