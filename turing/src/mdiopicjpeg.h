/****************/
/* mdiopicjpeg.h */
/****************/

#ifndef _MDIOPICJPEG_H_
#define _MDIOPICJPEG_H_

/*******************/
/* System includes */
/*******************/

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
extern BOOL	MDIOPic_LoadJPEG (const char *pmPathName, int *pmWidth,
				  int *pmHeight, PIC *pmMDPicInfo);

#endif // #ifndef _MDIOPICJPEG_H_
