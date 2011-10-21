/**************/
/* mdiofont.h */
/**************/

#ifndef _MDIOFONT_H_
#define _MDIOFONT_H_

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
extern FONT	MDIOFont_CreateFont (const char *pmTypeFace, int pmHeight,
				      int pmWidth, int pmBold, int pmItalic,
				      int pmUnderline, BOOL pmNoAntiAlias);
extern void	MDIOFont_EnumerateAllFonts (void);
extern void	MDIOFont_FreeFont (FONT pmMDFontInfo);
extern void	MDIOFont_GetName (FONT pmFont, char *pmFontName);
extern void	MDIOFont_GetSizes (FONT pmFont, int *pmHeight, int *pmAscent,
				   int *pmDescent, int *pmIntLeading);
extern int	MDIOFont_GetWidth (FONT pmFont, char *pmString);

#endif // #ifndef _MDIOFONT_H_
