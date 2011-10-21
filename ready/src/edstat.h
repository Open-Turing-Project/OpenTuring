/************/
/* edstat.h */
/************/

#ifndef _EDSTAT_H_
#define _EDSTAT_H_

/*******************/
/* System includes */
/*******************/
#include "stdarg.h"

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
    int		dialogFontSize;
    BOOL	oneWindowMode;
} EdStat_Properties;

/**********************/
/* External variables */
/**********************/

/***********************/
/* External procedures */
/***********************/
// Init, Finalize subprograms
extern BOOL	EdStat_Init (void);
extern void	EdStat_Finalize (void);
// Properties subprograms
extern void	EdStat_PropertiesSet (EdStat_Properties pmProperties);
extern void	EdStat_PropertiesImplementChanges (HWND pmStatusWindow);
// Create, Dispose subprograms
extern HWND	EdStat_Create (HWND pmEditWindow);
// Other subprograms
extern int	EdStat_GetWindowHeight (void);
extern void	EdStat_Set (HWND pmStatusWindow, const char *pmMessage, 
			    va_list pmArgList);
extern void	EdStat_SetBold (HWND pmStatusWindow, 
				const char *pmMessage, va_list pmArgList);
extern void	EdStat_SetPositionInfo (HWND pmStatusWindow, int pmLine, 
					int pmCol, int pmNumLines);

#endif // #ifndef _EDSTAT_H_
