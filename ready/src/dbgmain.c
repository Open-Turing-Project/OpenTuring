/*************/
/* dbgmain.c */
/*************/

/*******************/
/* System includes */
/*******************/

/****************/
/* Self include */
/****************/
#include "dbgmain.h"

/******************/
/* Other includes */
/******************/
#include "edgui.h"

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
/* DbgMain_LocateWindow							*/
/************************************************************************/
void	DbgMain_LocateWindow (HWND pmWindow, int pmWindowWidth, 
			      int pmWindowHeight)
{
    RECT	myScreenRect;
    int		myWindowLeft, myWindowTop;
    static int	myStCurrentLeft = 0;
    static int 	myStCurrentBottom = 0;
    
    // Calculate the top, left corner of the window
    myScreenRect = EdGUI_GetWorkArea (); 		   	

    if (myScreenRect.left + myStCurrentLeft + pmWindowWidth > 
    							myScreenRect.right)
    {
    	myStCurrentLeft = 0;
    	
    	if (myScreenRect.left + pmWindowWidth > myScreenRect.right)
    	{
	    pmWindowWidth = myScreenRect.right - myScreenRect.left;
	}
    }
    
    if (myScreenRect.bottom - myStCurrentBottom - pmWindowHeight < 
    							myScreenRect.top)
    {
    	myStCurrentBottom = 0;
    	
    	if (myScreenRect.top + pmWindowHeight > myScreenRect.bottom)
    	{
	    pmWindowHeight = myScreenRect.bottom - myScreenRect.top;
	}
    }
    
    myWindowLeft = myScreenRect.left + myStCurrentLeft;
    myWindowTop = myScreenRect.bottom - myStCurrentBottom - pmWindowHeight;
    
    myStCurrentLeft += GetSystemMetrics (SM_CYCAPTION);    
    myStCurrentBottom += GetSystemMetrics (SM_CYCAPTION);
    
    // Set the window to the correct position
    SetWindowPos (pmWindow, HWND_TOP, myWindowLeft, myWindowTop,
    	pmWindowWidth, pmWindowHeight, 0);
} // DbgMain_LocateWindow



/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/


