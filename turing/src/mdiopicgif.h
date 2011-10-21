/****************/
/* mdiopicgif.h */
/****************/

#ifndef _MDIOPICGIF_H_
#define _MDIOPICGIF_H_

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
extern int	MDIOPic_GetGIFFrames (const char *pmFullPathName);
extern BOOL	MDIOPic_LoadGIF (const char *pmPathName, int *pmWidth,
				 int *pmHeight, int *pmTransColour, 
				 PIC *pmMDPicInfo);
extern BOOL	MDIOPic_LoadGIFFrames (const char *pmPathName, int *pmWidth,
				       int *pmHeight, int *pmTransColour, 
				       PIC *pmMDPicInfo, int pmNumFrames,
				       int *pmDelay);

#endif // #ifndef _MDIOPICGIF_H_
