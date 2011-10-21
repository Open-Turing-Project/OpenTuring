/***************/
/* mioconfig.h */
/***************/

#ifndef _MIOCONFIG_H_
#define _MIOCONFIG_H_

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
// Config.Display constants 
#define CD_BASE			1000

#define CD_SCREEN_HEIGHT	CD_BASE	    // Gaps allow for multiple screens
#define CD_SCREEN_WIDTH		CD_BASE+10
#define CD_MAX_NUM_COLOURS	CD_BASE+20

// Config.Lang constants
#define CL_BASE			2000

#define CL_RELEASE		CL_BASE
#define CL_LANGUAGE_VERSION	CL_BASE+1
#define CL_MAX_NUM_STREAMS	CL_BASE+2
#define CL_MAX_NUM_DIR_STREAMS	CL_BASE+3
#define CL_MAX_RUN_TIME_ARGS	CL_BASE+4

// Config.Machine constants
#define CM_BASE			3000

#define CM_PROCESSOR		CM_BASE
#define CM_FPU			CM_BASE+1
#define CM_OS			CM_BASE+2

/*********/
/* Types */
/*********/

/**********************/
/* External variables */
/**********************/

/***********************/
/* External procedures */
/***********************/
extern OOTint	MIOConfig_Display (OOTint pmDisplayCode);
extern OOTint	MIOConfig_Lang (OOTint pmLangCode);
extern OOTint	MIOConfig_Machine (OOTint pmMachineCode);

#endif // #ifndef _MIOCONFIG_H_
