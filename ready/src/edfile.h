/************/
/* edfile.h */
/************/

#ifndef _EDFILE_H_
#define _EDFILE_H_

/*******************/
/* System includes */
/*******************/

/******************/
/* Other includes */
/******************/
#include "edtext.h"

/**********/
/* Macros */
/**********/
// Length of pathnames in Turing
#define PATHNAME_LENGTH		1024

#ifndef CONSOLE
// Values for EdFile_ReadFileIntoBuffer
#define OK_IF_NOT_FOUND		TRUE
#define ERROR_IF_NOT_FOUND	FALSE

// Values for EdFile_OpenFileForWrite
#define ALWAYS_FLUSH		TRUE
#define BUFFER_OUTPUT		FALSE
#define APPEND_TO_FILE		TRUE
#define CREATE_NEW_FILE		FALSE

// Values for EdFile_SetActiveDirectory

#define FILE_PATH		TRUE
#define DIRECTORY_PATH		FALSE

// Values for EdFile_ConstructPathName
#define ADMIN_PREF		-100
#define USER_PREF		-101
#define RECENT_FILES		-102
#define FROM_STARTUP		-99
#define FROM_APPDATA		-98
#define NO_FILE_NAME		-99
#define KEYWORD_LOOKUP		-98
#define DIR_ONLY_NO_SLASH	-97
#define EXAMPLES_DIR		-96

// Used for EdFile_ReadLineFromBuffer
#define READ_FIRST			1
#define READ_NEXT			2
#endif // #ifndef CONSOLE

/*************/
/* Constants */
/*************/

/*********/
/* Types */
/*********/
// The data type for all paths used in Turing
typedef char	FilePath [PATHNAME_LENGTH];

#ifndef CONSOLE
typedef struct
{
    HANDLE	handle;
    char	*pathName;
    BOOL	errorFound;
    BOOL	alwaysFlush;
} FileEntry, *FileEntryPtr;

/**********************/
/* External variables */
/**********************/

/***********************/
/* External procedures */
/***********************/
// Init, Finalize subprograms
extern BOOL		EdFile_Init (void);
extern void		EdFile_Finalize (void);
// Properties subprograms
extern void		EdFile_PropertiesSet (void);
extern void		EdFile_PropertiesImplementChanges (void);
// Other subprograms
extern void		EdFile_CloseFile (FileEntryPtr pmFile);
extern BOOL		EdFile_CombinePath (const char *pmBasePath, 
					    const char *pmNewPath,
					    char *pmAbsolutePath);
extern void		EdFile_ConstructPath (char *pmPathName, 
					      int pmDirectoryStringResource,
					      int pmFileStringResource);
extern BOOL		EdFile_CreateDirectoriesIfNecessary (
							const char *pmPathName);
extern BOOL		EdFile_DirectoryExists (const char *pmPathName);					      
extern BOOL		EdFile_FileExists (const char *pmPathName);					      
extern void		EdFile_GetActiveDirectory (FilePath pmDirectory);
extern void		EdFile_GetApplicationDirectory (
						char *pmApplicationDirectory);
extern void		EdFile_GetCurrentDirectory (char *pmCurrentDirectory);
extern void		EdFile_GetDirectoryFromPath (const char *pmPath, 
						     char *pmDirectory);
extern const char	*EdFile_GetFileName (const char *pmPathName);
extern const char	*EdFile_GetFileNameForwardSlash (
							const char *pmPathName);
extern const char	*EdFile_GetFileSuffix (const char *pmPathName);
extern char		*EdFile_GetPathForDisplay (const char *pmPathName, 
			    		int pmUnnamedNumber,
			    		BOOL pmIsExample,
			    		const char *pmExampleName, 
			    		BOOL pmFullPath, 
					FilePath pmPathForDisplayBuffer);
extern int		EdFile_GetUnnamedNumber (void);
extern int		EdFile_GetWindowsFolder (int pmCSIDL, char *pmPathName, 
						 BOOL pmCreate);
extern FileEntryPtr	EdFile_OpenFileForWrite (const char *pmPathName, 
						 int *pmErrorCode,
						 BOOL pmAlwaysFlush, 
						 BOOL pmAppend);
extern char 		*EdFile_ReadLineFromBuffer (char *myBuffer, 
						    int pmFileSize, 
						    int pmRead);
extern char		*EdFile_ReadFileIntoBuffer (HWND pmWindow, 
						    const char *pmPathName, 
						    DWORD *pmFileSize,
						    BOOL pmOKIfNotFound);

extern BOOL		EdFile_SaveRTFToFile (HWND pmWindow, char *pmPathName, 
			       TextPtr pmText, int *pmBytesSaved, 
			       int *pmLinesSaved);
extern BOOL		EdFile_SaveTextToFile (HWND pmWindow, char *pmPathName, 
			       TextPtr pmText, BOOL pmSelection, 
			       int *pmBytesSaved, int *pmLinesSaved);
extern void		EdFile_SetActiveDirectory (const char *pmPathName,
						   BOOL pmIsFilePath);
extern void		EdFile_SetCurrentDirectory (const char *pmDirectory);
extern void		EdFile_Write (FileEntryPtr pmFile, 
				      const char *pmFormatString, ...);
extern void		EdFile_Writeln (FileEntryPtr pmFile, 
					const char *pmFormatString, ...);
extern void		EdFile_WritelnString (FileEntryPtr pmFile, 
					      const char *pmString);
extern void		EdFile_WriteNL (FileEntryPtr pmFile);
extern void		EdFile_WriteString (FileEntryPtr pmFile, 
					    const char *pmString);
#endif // #ifndef CONSOLE

#endif // #ifndef _EDFILE_H_
