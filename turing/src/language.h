#ifndef _LANGUAGE_
#define _LANGUAGE_

#include "fileman.h"


typedef signed char	TLint1;
typedef char TLstring[256];


struct SrcPosition {
    unsigned short	lineNo;
    FileNoType		fileNo;
    unsigned short	linePos;
    unsigned short	tokLen;
};
typedef struct SrcPosition SrcPosition;
typedef struct SrcPosition SRCPOS;

#define Language_MaxErrTextLen 255
struct Language_ErrMsgs {
    struct Language_ErrMsgs	*next;
    struct SrcPosition		srcPos;
    char			text[Language_MaxErrTextLen+1];
};
typedef struct Language_ErrMsgs Language_ErrMsgs;
typedef struct Language_ErrMsgs ERRMSG;

enum Language_CookieKind {
    CookieKind_Null,
    CookieKind_Value,
    CookieKind_Symbol,
    CookieKind_ObjectScope,
    CookieKind_Scope
};
struct Language_Cookie {
    long kind;
    char *item;
    char *fp;
    char *data;
};
typedef struct Language_Cookie Language_Cookie;

struct Language_Calls {
    struct Language_Calls	*next;
    TLstring 			routineName;
    SrcPosition			srcPos;
    Language_Cookie		cookie;
};
typedef struct Language_Calls Language_Calls;

struct Language_Threads {
    struct Language_Threads	*next;
    void			*threadId;
    char			*threadName;
    Language_Calls		*traceback;
};
typedef struct Language_Threads Language_Threads;

struct Language_ThreadQueues {
    struct Language_ThreadQueues *next;
    void			*queueId;
    TLstring			queueName;
    Language_Threads		*threadList;
};
typedef struct Language_ThreadQueues Language_ThreadQueues;

struct Language_DumpIndices {
    struct Language_DumpIndices	*next;
    long			lowerBound;
    long			upperBound;
};
typedef struct Language_DumpIndices Language_DumpIndices;

struct Language_DumpSymbols {
    struct Language_DumpSymbols	*next;
    char			*symbolName;
    char			*typeName;
    SrcPosition			srcPos;
    TLstring			value;
    Language_Cookie		cookie;
    char			isPredef;
};
typedef struct Language_DumpSymbols Language_DumpSymbols;

struct Language_DumpScopeDef {
    Language_DumpSymbols	symbolInfo;
    Language_Cookie		expandCookie;
    Language_DumpSymbols	*parameters;
    Language_DumpSymbols	*declarations;
};
typedef struct Language_DumpScopeDef Language_DumpScopeDef;


extern void Language (void);
extern void Language_Reset (void);
extern void Language_Debugging (char val);
extern void Language_Paragraph (FileNoType fileNo);

#define MaxDefs     20
typedef TLstring OOTdefs[MaxDefs];
extern void Language_EnterPreprocSymbols (OOTdefs preDefs, long numDefs);

extern void Language_CompileUnit (TLstring fileName, FileNoType fileNo,
	Language_ErrMsgs **firstErrPtr, long *errorCount);
extern void Language_CompileProgram (TLstring fileName, FileNoType fileNo,
	Language_ErrMsgs **firstErrPtr, long *errorCount);

#define MaxArgs     20
typedef TLstring OOTargs[MaxArgs+1];

extern void Language_SetupExecution (unsigned long maxStackSize,
	TLstring inName, TLstring outName, OOTargs args, long numArgs);

enum Language_Step {
    Language_Step_None,
    Language_Step_StepIntoCall,
    Language_Step_NextOverCall,
    Language_Step_ReturnFromCall
};
extern void Language_SetStep (enum Language_Step step);

enum Language_ReturnState {
    Finished,
    Hiccup,
    EventWait,
    Stopped,
    NextLine,
    ContextSwap,
    KeyboardEventWait,
    MouseEventWait,
    DelayEventWait
};
struct Language_RunStatus {
    char		state;
    int			moreStateInfo;
    SrcPosition		srcPos;
    void		*threadId;
};
typedef struct Language_RunStatus Language_RunStatus;
extern void Language_ExecuteProgram (Language_RunStatus *status,
	Language_ErrMsgs **firstErrPtr, long *errorCount);

extern void Language_EndExecution (void);

extern void Language_DumpThreadList (Language_ThreadQueues **threadQueue);

extern void Language_DumpUnitList (Language_DumpSymbols **firstSymbol);

extern void Language_DumpScopeInfo (Language_Cookie *cookie,
	Language_DumpScopeDef *scopeInfo);

extern void Language_DumpSymbolInfo (Language_Cookie *cookie,
	Language_DumpSymbols **firstSymbol);

extern void Language_DumpArrayBounds (Language_Cookie *cookie,
	Language_DumpIndices **firstIndex);

extern void Language_DumpArrayValues (Language_Cookie *cookie,
	Language_DumpIndices *dumpIndices,
	Language_DumpSymbols **firstValue);

/*
** The following callback routines are internal to the executor, and
** are meant for MIO support.
*/

extern void Language_Execute_ExecutionError_Abort (long quitCode,
	TLstring message);

extern void Language_Execute_SetErrno (long errNo, char *errMessage);

extern void Language_Execute_GetErrno (long *errNo, long *prevErrno,
	char **errMessage, char **prevErrMessage);

extern void Language_SetMaxErrorCount (long errorCount);


/*
 * The following is used for informational messages.
 */

extern void Language_Execute_RunSrcPosition (SrcPosition *srcPos);
 
/*
 * These are used for ootcomp and ootexec
 */

extern void TL_TLI_TLIOF (unsigned short mode, TLstring name, long *fd);
extern void TL_TLI_TLIRE (void *data, OOTint dataSize, OOTint *status, long fd);
extern void TL_TLI_TLIWR (void *data, OOTint dataSize, OOTint *status, long fd);
extern void TL_TLI_TLICL (long fd);
extern void TL_TLI_TLISK (OOTint location, long fd);


extern void Language_WriteObjectFile (TLstring fileName, FileNoType fileNo,
	Language_ErrMsgs **firstErrPtr, long *errorCount, OOTint fd);

extern void Language_SetupExecution (unsigned long maxStackSize,
	TLstring inName, TLstring outName, OOTargs args, long numArgs);

#endif /* #ifndef _LANGUAGE_ */
