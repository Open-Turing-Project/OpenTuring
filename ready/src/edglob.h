/************/
/* edglob.h */
/************/

#ifndef _EDGLOB_H_
#define _EDGLOB_H_

/*******************/
/* System includes */
/*******************/

/******************/
/* Other includes */
/******************/

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/

#ifndef CONSOLE
/*********/
/* Types */
/*********/
typedef struct StringLink
{
    char		*string;
    struct StringLink	*next;
} *StringLink;


/**********************/
/* External variables */
/**********************/

/***********************/
/* External procedures */
/***********************/
extern char		*EdGlob_AddString (char *pmBuffer, int *pmSize, 
					   const char *pmAddString);
extern BOOL		EdGlob_ConvertStringToNum (char *pmString, 
						   int *pmValue);
extern void		EdGlob_ConvertToLowerCase (char *pmString);
extern void		EdGlob_ConvertToUpperCase (char *pmString);
extern StringLink	EdGlob_CreateStringLink (const char *pmString);
extern BOOL		EdGlob_EndsWith (const char *pmString, 
					 const char *pmSearchString);
extern void		EdGlob_FreeStringLinkList (StringLink *pmLink);
// extern BOOL		EdGlob_IsLegalChar (char pmChar);
extern BOOL		EdGlob_IsWhiteSpace (char pmChar);
extern char		*EdGlob_MallocString (const char *pmString);
extern void		EdGlob_Trim (char *pmString);
#endif // #ifndef CONSOLE	

#endif // #ifndef _EDGLOB_H_
