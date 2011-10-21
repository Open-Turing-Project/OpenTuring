/*************/
/* miofont.h */
/*************/

#ifndef _MIOFONT_H_
#define _MIOFONT_H_

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
#define DEFAULT_FONT_ID		-10

/*********/
/* Types */
/*********/

/**********************/
/* External variables */
/**********************/

/*********************************************/
/* External procedures for Turing predefines */
/*********************************************/
extern void	MIOFont_Free (OOTint pmFontID);
extern void	MIOFont_GetStyle (char *pmFontName, OOTboolean *pmBold, 
				  OOTboolean *pmItalic, 
				  OOTboolean *pmUnderline);
extern void	MIOFont_GetName (char *pmFontName);
extern OOTint	MIOFont_GetSize (void);
extern void	MIOFont_Name (OOTint pmFontID, OOTstring pmFontName);
extern OOTint	MIOFont_New (OOTstring pmFontStr, SRCPOS *pmSrcPos);
extern void	MIOFont_Sizes (OOTint pmFontID, OOTint *pmHeight, 
			       OOTint *pmAscent, OOTint *pmDescent,
			       OOTint *pmInternalLeading);
extern void	MIOFont_StartName (void);
extern void	MIOFont_StartSize (const OOTstring pmFontName, 
				   const OOTstring pmFontStyle);
extern OOTint	MIOFont_Width (OOTint pmFontID, OOTstring pmString);

/***************************************/
/* External procedures for MIO library */
/***************************************/
extern void	MIOFont_Init_Run (void);
extern void	MIOFont_Finalize_Run (void);
extern void	MIOFont_AddFont (const char *pmFontName, int pmHeight, 
			         BOOL pmIsBold, BOOL pmIsItalic, 
			         BOOL pmUnderline);
extern FONT	MIOFont_GetFont (OOTint pmFontID);
extern int	MIOFont_GetFontAscent (OOTint pmFontID);

#endif // #ifndef _MIOFONT_H_
