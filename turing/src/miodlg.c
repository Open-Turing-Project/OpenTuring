/************/
/* miodlg.c */
/************/

/*******************/
/* System includes */
/*******************/
#include <string.h>

/****************/
/* Self include */
/****************/
#include "miodlg.h"

/******************/
/* Other includes */
/******************/
//#include "miowin.h"
#include "mio.h"
#include "mdio.h"
//#include "mdiowin.h"
#include "mdiodlg.h"

#include "mioerr.h"

#include "edint.h"

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
/* MIODlg_Alert								*/
/************************************************************************/
void	MIODlg_Alert (const char *pmTitle, const char *pmMsg)
{
    MDIODlg_Alert (pmTitle, pmMsg);
} // MIODlg_Alert


/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/
