/************************************************************************/
/* mdioutil.h								*/
/*									*/
/* This module contains extremely platform dependent routines.  Even	*/
/* the ".h" file is platform dependent.					*/
/************************************************************************/

#ifndef _MDIOUTIL_H_
#define _MDIOUTIL_H_

/*******************/
/* System includes */
/*******************/
#include <windows.h>
#include <windowsx.h>

/******************/
/* Other includes */
/******************/
#include "miotypes.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/

/*********/
/* Types */
/*********/

/**********************/
/* External variables */
/**********************/

/***********************/
/* External procedures */
/***********************/
extern BOOL	MDIOUtil_ConvertBitMapToDIB (HDC pmDeviceContext, HBITMAP pmBitMap, 
		    HPALETTE pmPalette, int pmX, int pmY, int pmWidth, int pmHeight,
		    BOOL pmAlloc, BITMAPINFO **pmBitMapInfo,
		    int *pmBitMapInfoSize, BYTE **pmBits, int *pmBitsSize);

#endif // #ifndef _MDIOUTIL_H_
