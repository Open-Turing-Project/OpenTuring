/***********/
/* eddde.h */
/***********/

#ifndef _EDDDE_H_
#define _EDDDE_H_

/*******************/
/* System includes */
/*******************/
#include <windows.h>

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
    int dummy;
} EdDDE_Properties;

/**********************/
/* External variables */
/**********************/

/***********************/
/* External procedures */
/***********************/
// Init, Finalize subprograms
extern BOOL	EdDDE_Init (void);
extern void	EdDDE_Finalize (void);
// Properties subprograms
extern void	EdDDE_PropertiesSet (void);
extern void	EdDDE_PropertiesImplementChanges (void);
// Other subprograms
extern BOOL	EdDDE_TellReadyOpen (int pmNumFiles, char *pmFiles[]);

#endif // #ifndef _EDDDE_H_
