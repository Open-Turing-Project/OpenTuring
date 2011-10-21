#include "cinterface"
static struct {
    TLint4	dummy;
    char	dummy2[388];
} TLFTAB =
    { 8,
    {'/', 'e', 'x', 'p', 'o', 'r', 't', '/', 'h', 'o', 'm', 'e', '/', 't', 'u', 'r', 'i', 'n', 'g', '/', 'o', 'o', 't', '/', 's', 'r', 'c', '/', 'l', 'a', 'n', 'g', 'u', 'a', 'g', 'e', '/', 'e', 'r', 'r', 'o', 'r', '.', 'b', 'd', '\0',
    '/', 'e', 'x', 'p', 'o', 'r', 't', '/', 'h', 'o', 'm', 'e', '/', 't', 'u', 'r', 'i', 'n', 'g', '/', 'o', 'o', 't', '/', 's', 'r', 'c', '/', 'l', 'a', 'n', 'g', 'u', 'a', 'g', 'e', '/', 'e', 'r', 'r', 'o', 'r', '.', 's', 't', '\0',
    '/', 'e', 'x', 'p', 'o', 'r', 't', '/', 'h', 'o', 'm', 'e', '/', 't', 'u', 'r', 'i', 'n', 'g', '/', 'o', 'o', 't', '/', 's', 'r', 'c', '/', 'l', 'a', 'n', 'g', 'u', 'a', 'g', 'e', '/', '.', '.', '/', 'f', 'i', 'l', 'e', 'm', 'a', 'n', '/', 'f', 'i', 'l', 'e', 'm', 'a', 'n', '.', 's', 't', '\0',
    '/', 'e', 'x', 'p', 'o', 'r', 't', '/', 'h', 'o', 'm', 'e', '/', 't', 'u', 'r', 'i', 'n', 'g', '/', 'o', 'o', 't', '/', 's', 'r', 'c', '/', 'l', 'a', 'n', 'g', 'u', 'a', 'g', 'e', '/', 'd', 'u', 'm', 'm', 'y', '.', 't', '\0',
    '/', 'e', 'x', 'p', 'o', 'r', 't', '/', 'h', 'o', 'm', 'e', '/', 't', 'u', 'r', 'i', 'n', 'g', '/', 'o', 'o', 't', '/', 's', 'r', 'c', '/', 'l', 'a', 'n', 'g', 'u', 'a', 'g', 'e', '/', 'e', 'n', 'v', 'i', 'r', 'o', 'n', '.', 's', 't', '\0',
    '/', 'e', 'x', 'p', 'o', 'r', 't', '/', 'h', 'o', 'm', 'e', '/', 't', 'u', 'r', 'i', 'n', 'g', '/', 'o', 'o', 't', '/', 's', 'r', 'c', '/', 'l', 'a', 'n', 'g', 'u', 'a', 'g', 'e', '/', 'm', 's', 'g', 's', '.', 'd', 'e', 'f', '\0',
    '/', 'e', 'x', 'p', 'o', 'r', 't', '/', 'h', 'o', 'm', 'e', '/', 't', 'u', 'r', 'i', 'n', 'g', '/', 'o', 'o', 't', '/', 's', 'r', 'c', '/', 'l', 'a', 'n', 'g', 'u', 'a', 'g', 'e', '/', 'l', 'a', 'n', 'g', 'u', 'a', 'g', 'e', '.', 's', 't', '\0',
    '/', 'e', 'x', 'p', 'o', 'r', 't', '/', 'h', 'o', 'm', 'e', '/', 't', 'u', 'r', 'i', 'n', 'g', '/', 'o', 'o', 't', '/', 's', 'r', 'c', '/', 'l', 'a', 'n', 'g', 'u', 'a', 'g', 'e', '/', 'l', 'a', 'n', 'g', 'u', 'a', 'g', 'e', '.', 'b', 'd', '\0'
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

extern TLboolean AbortCheck ();

extern void FeedBack ();
struct	SrcPosition {
    TLnat2	lineNo;
    FileManager_FileNoType	fileNo;
    TLnat2	linePos;
    TLnat2	tokLen;
};
typedef	TLchar	__x250[256];
struct	ErrMsgDef {
    struct ErrMsgDef	*next;
    struct SrcPosition	srcPos;
    __x250	text;
};
typedef	TLstring	__x251[20];

extern void Language_EnterPreprocSymbols ();
typedef	TLstring	__x254[21];

extern void Language_SetupExecutionFromObjectFile ();

extern void Language_GetFileName ();
typedef	TLstring	__x258[21];
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
extern struct SrcPosition	Language_nullSrcPosition;
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
