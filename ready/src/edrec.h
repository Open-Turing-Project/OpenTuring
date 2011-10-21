/***********/
/* edrec.h */
/***********/

#ifndef _EDREC_H_
#define _EDREC_H_

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
// Where to add recent files
#define ADD_TO_TOP		TRUE
#define ADD_TO_BOTTOM		FALSE

/*********/
/* Types */
/*********/
typedef struct
{
    BOOL	useFullPathNames;
    int		numFilesToDisplay;
    int		noRecentFiles;
    BOOL	useLastDirectory;
    BOOL	useMyDocuments;
} EdRec_Properties;

/**********************/
/* External variables */
/**********************/

/***********************/
/* External procedures */
/***********************/
// Init, Finalize subprograms
extern BOOL	EdRec_Init (void);
extern void	EdRec_Finalize (void);
// Properties subprograms
extern void	EdRec_PropertiesSet (EdRec_Properties pmProperties);
extern void	EdRec_PropertiesImplementChanges (void);

// Other subprograms
extern void		EdRec_AddFile (const char *pmPathName, BOOL pmAddToTop);
extern void		EdRec_CreateRecentMenu (HMENU pmRecentMenu);
extern const char	*EdRec_GetPathName (int pmNumber);
extern void		EdRec_RemoveFile (char *pmPathName);
extern void		EdRec_WriteRecentFiles (void);

#endif // #ifndef _EDREC_H_
