/****************/
/* MIOWinDlg.c */
/****************/

/*******************/
/* System includes */
/*******************/

/****************/
/* Self include */
/****************/
#include "miowindlg.h"

/******************/
/* Other includes */
/******************/
#include "mio.h"
#include "mdio.h"

#include "miodialogs.h"

#include "miowin.h"
#include "mdiowin.h"
#include "mdiowindlg.h"

#include "edint.h"
//#include "edgui.h"

// Test to make certain we're not accidentally including <windows.h> which
// might allow for windows contamination of platform independent code.
#ifdef _INC_WINDOWS
xxx
#endif

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/

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

/******************************/
/* Static callback procedures */
/******************************/

/*********************/
/* Static procedures */
/*********************/

/***********************/
/* External procedures */
/***********************/
/************************************************************************/
/* MIOWinDlg_Init							*/
/************************************************************************/
void	MIOWinDlg_Init (void)
{
} // MIOWinDlg_Init


/************************************************************************/
/* MIOWinDlg_Finalize							*/
/************************************************************************/
void	MIOWinDlg_Finalize (void)
{
} // MIOWinDlg_Finalize


/************************************************************************/
/* MIOWinDlg_PropertiesSet						*/
/************************************************************************/
void	MIOWinDlg_PropertiesSet (void)
{
} // MIOWinDlg_PropertiesSet

			  
/************************************************************************/
/* MIOWinDlg_PropertiesImplementChanges					*/
/************************************************************************/
void	MIOWinDlg_PropertiesImplementChanges (void)
{
} // MIOWinDlg_PropertiesImplementChanges


/************************************************************************/
/* MIOWinDlg_CreateWindow						*/
/************************************************************************/
WIND	MIOWinDlg_CreateWindow (WindowAttribPtr pmWindowAttribs)
{
    return MDIOWinDlg_CreateWindow (pmWindowAttribs);
} // MIOWinDlg_CreateWindow

			      
/************************************************************************/
/* MIOWinDlg_MouseDownHandler						*/
/************************************************************************/
BOOL	MIOWinDlg_MouseDownHandler (WIND pmInnerWindow, int pmX, int pmY, 
				  BOOL pmShift, BOOL pmAllowOutOfBounds)
{
    return TRUE;
} // MIOWinDlg_MouseDownHandler


/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/
