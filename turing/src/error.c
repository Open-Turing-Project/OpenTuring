#include "cinterface"
static struct {
    TLint4	dummy;
    char	dummy2[491];
} TLFTAB =
    { 10,
    {'/', 'e', 'x', 'p', 'o', 'r', 't', '/', 'h', 'o', 'm', 'e', '/', 't', 'u', 'r', 'i', 'n', 'g', '/', 'o', 'o', 't', '/', 's', 'r', 'c', '/', 'l', 'a', 'n', 'g', 'u', 'a', 'g', 'e', '/', 'e', 'r', 'r', 'o', 'r', '.', 'b', 'd', '\0',
    '/', 'e', 'x', 'p', 'o', 'r', 't', '/', 'h', 'o', 'm', 'e', '/', 't', 'u', 'r', 'i', 'n', 'g', '/', 'o', 'o', 't', '/', 's', 'r', 'c', '/', 'l', 'a', 'n', 'g', 'u', 'a', 'g', 'e', '/', 'e', 'r', 'r', 'o', 'r', '.', 's', 't', '\0',
    '/', 'e', 'x', 'p', 'o', 'r', 't', '/', 'h', 'o', 'm', 'e', '/', 't', 'u', 'r', 'i', 'n', 'g', '/', 'o', 'o', 't', '/', 's', 'r', 'c', '/', 'l', 'a', 'n', 'g', 'u', 'a', 'g', 'e', '/', '.', '.', '/', 'f', 'i', 'l', 'e', 'm', 'a', 'n', '/', 'f', 'i', 'l', 'e', 'm', 'a', 'n', '.', 's', 't', '\0',
    '/', 'e', 'x', 'p', 'o', 'r', 't', '/', 'h', 'o', 'm', 'e', '/', 't', 'u', 'r', 'i', 'n', 'g', '/', 'o', 'o', 't', '/', 's', 'r', 'c', '/', 'l', 'a', 'n', 'g', 'u', 'a', 'g', 'e', '/', 'd', 'u', 'm', 'm', 'y', '.', 't', '\0',
    '/', 'e', 'x', 'p', 'o', 'r', 't', '/', 'h', 'o', 'm', 'e', '/', 't', 'u', 'r', 'i', 'n', 'g', '/', 'o', 'o', 't', '/', 's', 'r', 'c', '/', 'l', 'a', 'n', 'g', 'u', 'a', 'g', 'e', '/', 'e', 'n', 'v', 'i', 'r', 'o', 'n', '.', 's', 't', '\0',
    '/', 'e', 'x', 'p', 'o', 'r', 't', '/', 'h', 'o', 'm', 'e', '/', 't', 'u', 'r', 'i', 'n', 'g', '/', 'o', 'o', 't', '/', 's', 'r', 'c', '/', 'l', 'a', 'n', 'g', 'u', 'a', 'g', 'e', '/', 'm', 's', 'g', 's', '.', 'd', 'e', 'f', '\0',
    '/', 'e', 'x', 'p', 'o', 'r', 't', '/', 'h', 'o', 'm', 'e', '/', 't', 'u', 'r', 'i', 'n', 'g', '/', 'o', 'o', 't', '/', 's', 'r', 'c', '/', 'l', 'a', 'n', 'g', 'u', 'a', 'g', 'e', '/', 'd', 'e', 'b', 'u', 'g', '.', 'd', 'e', 'f', '\0',
    '/', 'e', 'x', 'p', 'o', 'r', 't', '/', 'h', 'o', 'm', 'e', '/', 't', 'u', 'r', 'i', 'n', 'g', '/', 'o', 'o', 't', '/', 's', 'r', 'c', '/', 'l', 'a', 'n', 'g', 'u', 'a', 'g', 'e', '/', 'l', 'a', 'n', 'g', 'u', 'a', 'g', 'e', '.', 's', 't', '\0',
    '/', 'e', 'x', 'p', 'o', 'r', 't', '/', 'h', 'o', 'm', 'e', '/', 't', 'u', 'r', 'i', 'n', 'g', '/', 'o', 'o', 't', '/', 's', 'r', 'c', '/', 'l', 'a', 'n', 'g', 'u', 'a', 'g', 'e', '/', 'l', 'a', 'n', 'g', 'u', 'a', 'g', 'e', '.', 'b', 'd', '\0',
    '/', 'e', 'x', 'p', 'o', 'r', 't', '/', 'h', 'o', 'm', 'e', '/', 't', 'u', 'r', 'i', 'n', 'g', '/', 'o', 'o', 't', '/', 's', 'r', 'c', '/', 'l', 'a', 'n', 'g', 'u', 'a', 'g', 'e', '/', 'u', 'n', 'i', 't', 'm', 'a', 'n', '/', 'u', 'n', 'i', 't', 'm', 'a', 'n', '.', 's', 't', '\0'
    }};
typedef	TLnat2	FileManager_FileNoType;
typedef	TLnat4	FileManager_FileTimeStamp;
typedef	TLint2	FileManager_ResultCode;

extern void FileManager_OpenNamedHandle ();

extern void FileManager_OpenUnnamedHandle ();

extern void FileManager_OpenFileHandle ();

extern void FileManager_SetModified ();

extern void FileManager_WriteFile ();

extern void FileManager_WriteBackup ();

extern void FileManager_WriteTemp ();

extern void FileManager_RmTemp ();

extern void FileManager_CloseFileHandle ();

extern void FileManager_OpenNamedText ();

extern void FileManager_OpenFileText ();

extern void FileManager_CloseFileText ();

extern void FileManager_CloseAllText ();

extern void FileManager_ChangeDirectory ();

extern void FileManager_GetDirectory ();

extern void FileManager_ChangeExecDirectory ();

extern void FileManager_GetExecDirectory ();

extern void FileManager_ShortName ();

extern void FileManager_RelativeName ();

extern void FileManager_PathName ();

extern void FileManager_ExecPathName ();

extern void FileManager_ExtendName ();

extern void FileManager_FileName ();

extern TLboolean FileManager_IsUnnamed ();

extern TLboolean FileManager_IsOnDisk ();

extern TLboolean FileManager_IsSameFile ();

extern TLboolean FileManager_IsReadOK ();

extern TLboolean FileManager_IsWriteOK ();

extern TLboolean FileManager_IsOlder ();

extern TLboolean FileManager_FileExists ();

extern void FileManager_AddReference ();

extern void FileManager_DeleteReference ();

extern void FileManager_ReadTimeStamp ();

extern void FileManager_FlushUnreferencedFiles ();

extern void FileManager_CheckReferencedFiles ();

extern void FileManager_RmFile ();

extern void FileManager_SetDefaultInclude ();

extern void FileManager_AssertTextClosed ();

extern void FileManager_Dump ();

extern TLboolean AbortCheck ();

extern void FeedBack ();
struct	SrcPosition {
    TLnat2	lineNo;
    FileManager_FileNoType	fileNo;
    TLnat2	linePos;
    TLnat2	tokLen;
};
typedef	TLchar	__x462[256];
struct	ErrMsgDef {
    struct ErrMsgDef	*next;
    struct SrcPosition	srcPos;
    __x462	text;
};
typedef	TLint4	CookieKind;
struct	Cookie {
    CookieKind	kind;
    TLaddressint	item;
    TLaddressint	fp;
    TLaddressint	data;
};
struct	CallDef {
    struct CallDef	*next;
    TLstring	routineName;
    struct SrcPosition	srcPos;
    struct Cookie	cookie;
};
struct	ThreadDef {
    struct ThreadDef	*next;
    TLaddressint	threadId;
    TLaddressint	threadName;
    struct CallDef	*traceback;
};
struct	ThreadQueueDef {
    struct ThreadQueueDef	*next;
    TLaddressint	queueId;
    TLstring	queueName;
    struct ThreadDef	*threadList;
};
struct	DumpIndexDef {
    struct DumpIndexDef	*next;
    TLint4	lowerBound;
    TLint4	upperBound;
};
struct	DumpSymbolDef {
    struct DumpSymbolDef	*next;
    TLaddressint	symbolName;
    TLaddressint	typeName;
    struct SrcPosition	srcPos;
    TLstring	value;
    struct Cookie	cookie;
    TLboolean	isPredef;
};
struct	DumpScopeDef {
    struct DumpSymbolDef	symbolInfo;
    struct Cookie	expandCookie;
    struct DumpSymbolDef	*parameters;
    struct DumpSymbolDef	*declarations;
};

extern void Language_Debugging ();

extern void Language_Paragraph ();

extern void Language_Reset ();

extern void Language_SetMaxErrorCount ();
typedef	TLstring	__x463[20];

extern void Language_EnterPreprocSymbols ();

extern void Language_CompileUnit ();

extern void Language_CompileProgram ();

extern void Language_WriteObjectFile ();
typedef	TLstring	__x466[21];
typedef	TLstring	__x470[21];

extern void Language_SetupExecution ();
typedef	TLint1	Language_Step;

extern void Language_SetStep ();
typedef	TLint1	Language_ReturnState;
struct	Language_RunStatus {
    Language_ReturnState	state;
    TLint4	moreStateInfo;
    struct SrcPosition	srcPos;
    TLaddressint	threadId;
};

extern void Language_ExecuteProgram ();

extern void Language_EndExecution ();

extern void Language_StopExecution ();

extern void Language_DumpThreadList ();

extern void Language_DumpUnitList ();

extern void Language_DumpScopeInfo ();

extern void Language_DumpSymbolInfo ();

extern void Language_DumpArrayBounds ();

extern void Language_DumpArrayValues ();
extern TLboolean	Language_debug;
extern struct SrcPosition	Language_nullSrcPosition;
extern struct Cookie	Language_nullCookie;
extern struct DumpSymbolDef	Language_nullSymbolDef;
extern struct DumpScopeDef	Language_nullScopeDef;
typedef	TLint1	Language_UnitManager_UnitKind;
typedef	TLint1	Language_UnitManager_UnitStatus;

extern void Language_UnitManager_AddDepend ();

extern void Language_UnitManager_AddIncludeFile ();

extern void Language_UnitManager_AllocateCompileChunk ();

extern void * Language_UnitManager_BodyUnit ();

extern void Language_UnitManager_ClearAllMarks ();

extern void Language_UnitManager_ClearConfiguration ();

extern TLaddressint Language_UnitManager_CodeTable ();

extern FileManager_FileNoType Language_UnitManager_FileNo ();

extern void Language_UnitManager_FlushOldUnits ();

extern void Language_UnitManager_GetNextDepend ();

extern void Language_UnitManager_GetNextUnit ();

extern void Language_UnitManager_GetUnitPtr ();

extern TLaddressint Language_UnitManager_GlobalArea ();

extern TLint4 Language_UnitManager_GlobalSize ();

extern TLboolean Language_UnitManager_IsCompiled ();

extern TLboolean Language_UnitManager_IsMarked ();

extern TLboolean Language_UnitManager_IsPredef ();

extern Language_UnitManager_UnitKind Language_UnitManager_Kind ();

extern TLaddressint Language_UnitManager_ManifestTable ();

extern void * Language_UnitManager_OverrideBody ();

extern void Language_UnitManager_PrepareForCompile ();

extern void Language_UnitManager_PrepareForDepends ();

extern void Language_UnitManager_PrepareForScan ();

extern void Language_UnitManager_Reset ();

extern void Language_UnitManager_RewindDependList ();

extern void Language_UnitManager_RewindUnitList ();

extern void Language_UnitManager_SetCodeTable ();

extern void Language_UnitManager_SetDefaultBody ();

extern void Language_UnitManager_SetGlobalArea ();

extern void Language_UnitManager_SetKind ();

extern void Language_UnitManager_SetManifestTable ();

extern void Language_UnitManager_SetMark ();

extern void Language_UnitManager_SetOverrideBody ();

extern void Language_UnitManager_SetPredef ();

extern void Language_UnitManager_SetStub ();

extern void Language_UnitManager_SetSymbolTable ();

extern void Language_UnitManager_StartNewProgram ();

extern void * Language_UnitManager_StubUnit ();

extern void Language_UnitManager_SuccessfulCompile ();

extern TLaddressint Language_UnitManager_SymbolTable ();
typedef	TLint1	Language_ErrorModule_Severity;

void Language_ErrorModule_Initialize ();

void Language_ErrorModule_SetMaxErrors ();

void Language_ErrorModule_Message ();

void Language_ErrorModule_AbortIfErrors ();

void Language_ErrorModule_ReturnErrors ();
static struct ErrMsgDef	*Language_ErrorModule_firstMsg;
static struct ErrMsgDef	*Language_ErrorModule_lastMsg;
static TLint4	Language_ErrorModule_numErrors;
static TLint4	Language_ErrorModule_maxErrors;
static struct ErrMsgDef	*Language_ErrorModule_fatalErrorMsg;
static TLboolean	Language_ErrorModule_memoryAbort;

static void Language_ErrorModule_NewMsg (srcPos)
struct SrcPosition	*srcPos;
{
    TLSTKCHKSLF(100023);
    TLSETF();
    TLSETL(100023);
    if (Language_ErrorModule_lastMsg == ((struct ErrMsgDef *) 0)) {
	TLSETL(100024);
	TL_TLB_TLBNWU(& Language_ErrorModule_lastMsg, (TLint4)  sizeof (	struct ErrMsgDef));
	TLINCL();
	if (Language_ErrorModule_lastMsg == ((struct ErrMsgDef *) 0)) {
	    TLSETL(100026);
	    Language_ErrorModule_memoryAbort = 1;
	    TLINCL();
	    Language_ErrorModule_lastMsg = Language_ErrorModule_fatalErrorMsg;
	};
	TLSETL(100029);
	Language_ErrorModule_firstMsg = Language_ErrorModule_lastMsg;
    } else {
	TLSETL(100031);
	TL_TLB_TLBNWU(& Language_ErrorModule_lastMsg->next, (TLint4)  sizeof (	struct ErrMsgDef));
	TLINCL();
	if ((Language_ErrorModule_lastMsg->next) == ((struct ErrMsgDef *) 0)) {
	    TLSETL(100033);
	    Language_ErrorModule_memoryAbort = 1;
	    TLINCL();
	    Language_ErrorModule_lastMsg->next = Language_ErrorModule_fatalErrorMsg;
	};
	TLSETL(100036);
	Language_ErrorModule_lastMsg = Language_ErrorModule_lastMsg->next;
    };
    TLSETL(100038);
    Language_ErrorModule_lastMsg->next = (struct ErrMsgDef *) 0;
    TLSETL(100040);
    TLSTRCTASS(Language_ErrorModule_lastMsg->srcPos, (*srcPos), struct SrcPosition);
    TLRESTORELF();
}

void Language_ErrorModule_Message (srcPos, status, text)
struct SrcPosition	*srcPos;
Language_ErrorModule_Severity	status;
TLstring	text;
{
    register TLaddressint	dst;
    register TLaddressint	src;
    register TLaddressint	lastAddr;
    TLSTKCHKSLF(100048);
    TLSETF();
    TLSETL(100048);
    Language_ErrorModule_NewMsg(&((*srcPos)));
    dst = (TLaddressint) ((unsigned long)Language_ErrorModule_lastMsg->text);
    TLINCL();
    src = (TLaddressint) ((unsigned long)text);
    TLINCL();
    lastAddr = (TLaddressint) ((unsigned long) dst + 255);
    TLSETL(100052);
    if (status == 0) {
	TLSETL(100053);
	TLSTRASS(255, Language_ErrorModule_lastMsg->text, "(Warning) ");
	TLINCL();
	dst += 10;
    };
    TLSETL(100058);
    for(;;) {
	TLSETL(100058);
	(* (TLchar *) dst) = (* (TLchar *) src);
	TLINCL();
	if (((* (TLchar *) dst)) == '\0') {
	    break;
	};
	TLSETL(100060);
	dst += 1;
	TLINCL();
	if ((unsigned long) dst == (unsigned long) lastAddr) {
	    break;
	};
	TLSETL(100062);
	src += 1;
    };
    TLSETL(100065);
    if (status == 3) {
	TLSETL(100066);
	if ((unsigned long) dst == (unsigned long) lastAddr) {
	    TLSETL(100067);
	    (* (TLchar *) ((unsigned long) dst - 1)) = '.';
	} else {
	    TLSETL(100069);
	    (* (TLchar *) dst) = '.';
	    TLINCL();
	    dst += 1;
	};
    };
    TLSETL(100073);
    (* (TLchar *) dst) = '\0';
    TLSETL(100075);
    if (status > 1) {
	TLSETL(100076);
	Language_ErrorModule_numErrors += 1;
    };
    TLSETL(100078);
    if ((status == 3) || Language_ErrorModule_memoryAbort) {
	TLSETL(100079);
	TL_TLE_TLEQUIT ((TLint4) 1, (char *) 0, 0);
    };
    TLSETL(100081);
    if (Language_ErrorModule_numErrors >= Language_ErrorModule_maxErrors) {
	TLSETL(100083);
	Language_ErrorModule_NewMsg(&((*srcPos)));
	TLSTRASS(255, Language_ErrorModule_lastMsg->text, "Too many errors -- compilation aborted");
	TLSETL(100085);
	TL_TLE_TLEQUIT ((TLint4) 1, (char *) 0, 0);
    };
    TLRESTORELF();
}

void Language_ErrorModule_ReturnErrors (firstErrorMsg, errorCount)
struct ErrMsgDef	**firstErrorMsg;
TLint4	*errorCount;
{
    TLSTKCHKSLF(100092);
    TLSETF();
    TLSETL(100092);
    (*firstErrorMsg) = Language_ErrorModule_firstMsg;
    TLINCL();
    (*errorCount) = Language_ErrorModule_numErrors;
    TLRESTORELF();
}

void Language_ErrorModule_AbortIfErrors () {
    TLSTKCHKSLF(100098);
    TLSETF();
    TLSETL(100098);
    if (Language_ErrorModule_numErrors > 0) {
	TLSETL(100099);
	TL_TLE_TLEQUIT ((TLint4) 1, (char *) 0, 0);
    };
    TLRESTORELF();
}

void Language_ErrorModule_SetMaxErrors (maxErrorCount)
TLint4	maxErrorCount;
{
    TLSTKCHKSLF(100105);
    TLSETF();
    TLSETL(100105);
    if (maxErrorCount > 0) {
	TLSETL(100106);
	Language_ErrorModule_maxErrors = maxErrorCount;
    };
    TLRESTORELF();
}

void Language_ErrorModule_Initialize () {
    register struct ErrMsgDef	*msg;
    TLSTKCHKSLF(100112);
    TLSETF();
    TLSETL(100112);
    msg = Language_ErrorModule_firstMsg;
    TLSETL(100114);
    for(;;) {
	struct ErrMsgDef	*oldMsg;
	TLSETL(100114);
	if ((msg == ((struct ErrMsgDef *) 0)) || (msg == Language_ErrorModule_fatalErrorMsg)) {
	    break;
	};
	TLSETL(100115);
	oldMsg = msg;
	TLINCL();
	msg = msg->next;
	TLINCL();
	TL_TLB_TLBFRU(& oldMsg, (TLint4)  sizeof (	struct ErrMsgDef));
    };
    TLSETL(100119);
    Language_ErrorModule_firstMsg = (struct ErrMsgDef *) 0;
    TLINCL();
    Language_ErrorModule_lastMsg = (struct ErrMsgDef *) 0;
    TLINCL();
    Language_ErrorModule_numErrors = 0;
    TLINCL();
    Language_ErrorModule_memoryAbort = 0;
    TLRESTORELF();
}

void Language_ErrorModule () {
    TLSAVELF();
    TLSETF();
    TLSETL(100010);
    Language_ErrorModule_firstMsg = (struct ErrMsgDef *) 0;
    TLSETL(100014);
    Language_ErrorModule_lastMsg = (struct ErrMsgDef *) 0;
    TLINCL();
    Language_ErrorModule_numErrors = 0;
    TLSETL(100017);
    Language_ErrorModule_maxErrors = 21;
    TLSETL(100019);
    Language_ErrorModule_memoryAbort = 0;
    TLSETL(100127);
    TL_TLB_TLBNWU(& Language_ErrorModule_fatalErrorMsg, (TLint4)  sizeof (    struct ErrMsgDef));
    TLINCL();
    Language_ErrorModule_fatalErrorMsg->next = (struct ErrMsgDef *) 0;
    TLRESTORELF();
}
