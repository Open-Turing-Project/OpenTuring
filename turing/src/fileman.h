

/* ANSI C prototyping for file manager's functions, types, and constants */

#ifndef _ED_FILEMAN_H_
#define _ED_FILEMAN_H_

#define MAX_NUM_FILES 100

/* Result codes */

#define FileManager_ResultCode_Ok		0
#define FileManager_ResultCode_NoRead		1
#define FileManager_ResultCode_NoWrite		2
#define FileManager_ResultCode_NonAscii		3
#define FileManager_ResultCode_NoSpace		4
#define FileManager_ResultCode_IsOpen		5

#include "turing.h"

typedef unsigned short	FileNoType;
typedef char **		TextHandleType;
typedef char *		TextPtrType;
typedef OOTint *	SizePtrType;
typedef short		ResultCodeType;


extern void FileManager (void);

extern void
FileManager_OpenNamedHandle (OOTstring, FileNoType *, TextHandleType *, 
			     SizePtrType *, ResultCodeType *);

extern void FileManager_OpenUnnamedHandle (FileNoType *, TextHandleType *, 
					   SizePtrType *);

extern void FileManager_SetModified (FileNoType);

extern void FileManager_WriteFile (OOTstring, FileNoType *,
				   TextHandleType *, SizePtrType *, 
				   ResultCodeType *);

extern void FileManager_CloseFileHandle (FileNoType);

extern void FileManager_OpenNamedText (OOTstring, FileNoType, FileNoType *,
				       TextPtrType *, ResultCodeType *);

extern void FileManager_OpenFileText (FileNoType, TextPtrType,
				      ResultCodeType *);
extern void FileManager_CloseFileText (FileNoType);
extern void FileManager_CloseAllText (void);

extern void FileManager_ChangeDirectory (OOTstring);
extern void FileManager_GetDirectory (OOTstring, OOTstring);

extern void FileManager_ChangeExecDirectory (OOTstring);
extern void FileManager_GetExecDirectory (OOTstring);

extern void FileManager_SetLongFileNameFlag (OOTboolean);


/* Query */

extern void FileManager_ExecPathName (OOTstring, OOTstring);
extern void FileManager_PathName (OOTstring, OOTstring);
extern void FileManager_FileName (FileNoType, OOTstring);
extern void FileManager_ShortName (OOTstring, OOTstring);

extern OOTboolean FileManager_IsUnnamed (FileNoType);
extern OOTboolean FileManager_IsOnDisk (FileNoType);
extern OOTboolean FileManager_IsReadOK (OOTstring);
extern OOTboolean FileManager_IsWriteOK (OOTstring);

extern void FileManager_SetDefaultInclude (OOTstring);
extern void FileManager_SetHomeDir (OOTstring);
extern void FileManager_SetJobDir (OOTstring);

/* Added by TW */

extern void FileManager_FlushUnreferencedFiles (void);
extern OOTboolean FileManager_IsSameFile (FileNoType, FileNoType);
extern void FileManager_WriteBackup (FileNoType, OOTstring);
extern void FileManager_AddReference (FileNoType);
extern void FileManager_DeleteReference (FileNoType);
extern void FileManager_PrefixedName (OOTstring, OOTstring);

#endif /* _ED_FILEMAN_H_ */




