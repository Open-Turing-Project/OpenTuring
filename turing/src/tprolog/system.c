#include "cinterface"
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
typedef	TLchar	__x1589[256];
struct	ErrMsgDef {
    struct ErrMsgDef	*next;
    struct SrcPosition	srcPos;
    __x1589	text;
};
typedef	TLstring	__x1590[20];

extern void Language_EnterPreprocSymbols ();
typedef	TLstring	__x1593[21];

extern void Language_SetupExecutionFromObjectFile ();

extern void Language_GetFileName ();
typedef	TLstring	__x1597[21];
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

extern void Language_ErrorModule_Initialize ();

extern void Language_ErrorModule_SetMaxErrors ();

extern void Language_ErrorModule_Message ();

extern void Language_ErrorModule_AbortIfErrors ();

extern void Language_ErrorModule_ReturnErrors ();
typedef	Language_ErrorModule_Severity	Language_ErrorSeverity;
typedef	TLstring	Language_StringDef;
typedef	TLnat4	Language_Offset;
extern TLint4	Language_codeOprSize;
extern TLint4	Language_codeInt1Size;
extern TLint4	Language_codeInt2Size;
extern TLint4	Language_codeIntSize;
extern TLint4	Language_codeRealSize;
extern TLint4	Language_codeAddrSize;
extern TLint4	Language_codeOffsetSize;
typedef	TLchar	Language_string1[2];
typedef	TLnat1	Language_set8;
typedef	TLnat2	Language_set16;
typedef	TLnat4	Language_set32;
typedef	TLint2	Language_Opcode;
struct	Language_CodeHeader {
    TLaddressint	bodyCode;
};
struct	Language___x1601 {
    TLint4	lower, range;
};
typedef	struct Language___x1601	Language___x1600[255];
struct	Language_ArrayDescriptor {
    TLnat4	sizeArray;
    TLnat4	sizeElement;
    TLnat4	elementCount;
    TLnat4	numDimensions;
    Language___x1600	dim;
};
struct	Language_ForDescriptor {
    TLint4	counter;
    TLint4	right;
    TLint4	step;
    TLaddressint	savedSP;
};
typedef	Language_Offset	Language___x1602[1001];
struct	Language_CaseDescriptor {
    TLint4	lower, upper;
    Language_Offset	otherwise;
    Language___x1602	caseVector;
};
struct	Language_PointerDescriptor {
    TLaddressint	dataPtr;
    TLint4	timestamp;
};
typedef	TLnat2	Language___x1603[301];
struct	Language_UnionMapDescriptor {
    TLint4	lower, upper;
    Language___x1603	map;
};
typedef	TLnat4	Language_ClassAttr;
struct	Language_ClassDescriptor {
    TLaddressint	baseClass;
    TLaddressint	expandClass;
    Language_ClassAttr	attributes;
    TLnat4	objSize;
    TLaddressint	classId;
    TLaddressint	initRoutine;
    TLnat4	numOperations;
};
typedef	TLint1	Language_OpenKind;
typedef	TLint1	Language_PutKind;
typedef	TLint1	Language_GetKind;
typedef	TLint1	Language_SetStreamKind;
typedef	TLnat1	Language_StreamModeSet;
typedef	TLint1	Language_SubstringKind;
typedef	TLint1	Language_RangeCheckKind;
typedef	TLint1	Language_ConditionKind;
typedef	TLstring	__x1606[21];

extern void Language_Execute_Initialize ();

extern TLint4 Language_Execute_GetStackAllocated ();

extern void Language_Execute_Executor ();

extern void Language_Execute_EndExecution ();

extern void Language_Execute_RecoverAllMemory ();
extern TLint2	Language_Execute_executionTimeout;
extern TLboolean	Language_Execute_externalExecutionTimeoutFlag;
extern TLint4	Language_Execute_stackAllocated;
extern TLboolean	Language_Execute_allWaitingForKeyboard;
extern TLboolean	Language_Execute_allWaitingForMouse;
extern TLboolean	Language_Execute_allWaitingForDelay;
extern TLint4	Language_Execute_minimumDelayCount;

extern void TL_TLB_TLBUUN ();

extern void TL_TLB_TLBUNI ();

extern void strcpy ();
typedef	TLint2	Language_Execute_ExecutionError_OverflowKind;

void Language_Execute_ExecutionError_Abort ();

void Language_Execute_ExecutionError_SetOverflowKind ();
typedef	TLnat4	Language_Execute_ExecutionError_TL_priority_t;
struct	Language_Execute_ExecutionError_TL_ExceptionInfo {
    TLint4	quitCode;
    TLint4	libraryQuitCode;
    TLstring	errorMsg;
};
typedef	TLnat1	Language_Execute_ExecutionError___x1609[1];
struct	Language_Execute_ExecutionError_TL_HandlerArea {
    TLint4	quitCode;
    TLnat4	lineAndFile;
    TLaddressint	fileTable;
    struct Language_Execute_ExecutionError_TL_HandlerArea	*nextHandler;
    Language_Execute_ExecutionError___x1609	savedState;
};
typedef	TLnat4	Language_Execute_ExecutionError_TL_hardwarePriority_t;
typedef	Language_Execute_ExecutionError_TL_hardwarePriority_t	Language_Execute_ExecutionError_TL_lockStatus_t;
typedef	TLnat4	Language_Execute_ExecutionError_TL_lock_t;
struct	Language_Execute_ExecutionError_TL_link_t {
    struct Language_Execute_ExecutionError_TL_ProcessDescriptor	*flink;
    struct Language_Execute_ExecutionError_TL_ProcessDescriptor	*blink;
};
struct	Language_Execute_ExecutionError_TL_ProcessDescriptor {
    TLnat4	lineAndFile;
    TLaddressint	fileTable;
    TLaddressint	stackLimit;
    TLaddressint	stackPointer;
    struct Language_Execute_ExecutionError_TL_HandlerArea	*handlerQhead;
    struct Language_Execute_ExecutionError_TL_HandlerArea	*currentHandler;
    TLaddressint	name;
    struct Language_Execute_ExecutionError_TL_ExceptionInfo	exception;
    TLnat4	waitParameter;
    struct Language_Execute_ExecutionError_TL_ProcessDescriptor	*monitorQlink;
    TLnat4	timeOutStatus;
    TLnat4	pid;
    TLaddressint	memoryBase;
    TLnat4	timeoutTime;
    TLnat4	timeoutEpoch;
    struct Language_Execute_ExecutionError_TL_link_t	timeoutQ;
    TLboolean	timedOut;
    TLboolean	pausing;
    Language_Execute_ExecutionError_TL_priority_t	dispatchPriority;
    struct Language_Execute_ExecutionError_TL_ProcessDescriptor	*runQlink;
    TLboolean	ready;
    struct Language_Execute_ExecutionError_TL_ProcessDescriptor	*tsyncWaiter;
    TLnat4	quantum;
    TLnat4	quantumCntr;
    TLnat4	devmonLevel;
    TLaddressint	otherInfo;
};
struct	Language_Execute_ExecutionError_TL_queue_t {
    struct Language_Execute_ExecutionError_TL_ProcessDescriptor	*head;
    struct Language_Execute_ExecutionError_TL_ProcessDescriptor	*tail;
};
struct	Language_Execute_ExecutionError_TL_MonitorDescriptor {
    Language_Execute_ExecutionError_TL_lock_t	mQLock;
    TLint4	entryParameter;
    TLint4	monitorPriority;
    TLboolean	deviceMonitor;
    struct Language_Execute_ExecutionError_TL_queue_t	entryQ;
    struct Language_Execute_ExecutionError_TL_queue_t	reEntryQ;
    TLaddressint	name;
    struct Language_Execute_ExecutionError_TL_ConditionDescriptor	*firstCondition;
    struct Language_Execute_ExecutionError_TL_MonitorDescriptor	*nextMonitor;
    struct Language_Execute_ExecutionError_TL_MonitorDescriptor	*prevMonitor;
    TLaddressint	otherInfo;
};
struct	Language_Execute_ExecutionError_TL_ConditionDescriptor {
    struct Language_Execute_ExecutionError_TL_queue_t	signalQ;
    struct Language_Execute_ExecutionError_TL_MonitorDescriptor	*md;
    TLaddressint	name;
    TLnat4	index;
    struct Language_Execute_ExecutionError_TL_ConditionDescriptor	*nextCondition;
    struct Language_Execute_ExecutionError_TL_ConditionDescriptor	*prevCondition;
    TLaddressint	otherInfo;
};
typedef	struct Language_Execute_ExecutionError_TL_ConditionDescriptor	Language_Execute_ExecutionError___x1612[65535];
extern struct Language_Execute_ExecutionError_TL_ProcessDescriptor	*TL_TLKPD;
extern struct Language_Execute_ExecutionError_TL_ProcessDescriptor	*TL_TLK_TLKTQH;
extern struct Language_Execute_ExecutionError_TL_MonitorDescriptor	*TL_TLM_TLMMLH;

void Language_Execute_ExecutionError_GetAbort ();

void Language_Execute_ExecutionError_Initialize ();

extern void Language_Execute_Heap_Alloc ();

extern void Language_Execute_Heap_CleanUp ();

extern void Language_Execute_Heap_Dealloc ();

extern void Language_Execute_Heap_Free ();

extern void Language_Execute_Heap_New ();

extern void Language_Execute_Heap_Realloc ();
typedef	TLint1	MIO_EventMode;
struct	MIO_EventDescriptor {
    MIO_EventMode	mode;
    TLint4	count;
    TLaddressint    data;
};

extern TLboolean MIO_HasEvent ();

extern TLboolean MIO_SetActive ();

extern void MIO_NextEvent ();

extern void MIO_SetEvents ();

extern void MIO_GetEvent ();
typedef	TLchar	__x1613[4];

extern void MIO_KeyTranslate ();

extern TLchar MIO_Getch ();

extern TLboolean MIO_Hasch ();

extern void MIO_Clip ();

extern void MIO_ClipOff ();

extern void MIO_Clear ();

extern TLint4 MIO_ColourNum ();

extern void MIO_DrawArc ();

extern void MIO_DrawBox ();

extern void MIO_DrawDot ();

extern void MIO_DrawFill ();

extern void MIO_DrawFillArc ();

extern void MIO_DrawFillBox ();

extern void MIO_DrawFillOval ();

extern void MIO_DrawFillPolygon ();

extern void MIO_DrawLine ();

extern void MIO_DrawOval ();

extern void MIO_DrawPic ();

extern void MIO_DrawPolygon ();

extern void MIO_DrawText ();

extern void MIO_DrawTextSize ();

extern TLint4 MIO_FontNum ();

extern void MIO_GetColour ();

extern TLint4 MIO_GetDotColour ();

extern void MIO_GetGraphMax ();

extern TLint4 MIO_GetMaxColour ();

extern TLint4 MIO_GetMaxFont ();

extern TLint4 MIO_GetPalette ();

extern void MIO_GetPos ();

extern void MIO_GetRgb ();

extern void MIO_GetScreenSize ();

extern void MIO_GetTextChar ();

extern TLint4 MIO_GetTextFont ();

extern void MIO_GetTextMax ();

extern void MIO_NumColour ();

extern void MIO_NumFont ();

extern void MIO_SetBackgroundColour ();

extern void MIO_SetPalette ();

extern void MIO_SetPos ();

extern void MIO_SetPosXY ();

extern void MIO_SetRgb ();

extern void MIO_SetTextBackgroundColour ();

extern void MIO_SetTextColour ();

extern void MIO_SetTextFont ();

extern TLint4 MIO_SizePic ();

extern void MIO_TakePic ();

extern void MIO_Init ();

extern void MIO_End ();

extern void MIO_Mode ();

extern void MIO_Delay ();

extern void MIO_PlaySound ();

extern void MIO_Play ();

extern TLboolean MIO_PlayDone ();

extern void MIO_RegisterOpen ();

extern void MIO_RegisterClose ();
extern TLnat2	Language_Execute_numArguments;
typedef	TLint1	Language_Execute___x1614[23];
extern Language_Execute___x1614	Language_Execute_fileMap;
typedef	Language_StringDef	*Language_Execute___x1617[21];
extern Language_Execute___x1617	Language_Execute_argumentNames;
extern TLboolean	TL_TLI_TLIUXS;
extern TLint4	TL_TLI_TLIXSN;
struct	Language_Execute_LibraryAbort {
    TLint4	quitCode;
    TLstring	errorMsg;
};
struct	Language_Execute_SavedRegisters {
    TLaddressint	fp;
    TLaddressint	sp;
};
struct	Language_Execute_HandlerArea {
    TLint4	quitCode;
    struct SrcPosition	srcPos;
    struct Language_Execute_HandlerArea	*nextHandler;
    TLaddressint	handlerRoutine;
    struct Language_Execute_SavedRegisters	savedRegisters;
};
struct	Language_Execute_PendingEvent {
    TLint4	stream;
    struct MIO_EventDescriptor	event;
};
struct	Language_Execute_ProcessDescriptor {
    TLaddressint	pc;
    TLaddressint	fp;
    TLaddressint	sp;
    TLaddressint	minimumSP;
    TLaddressint	stackBase;
    struct SrcPosition	srcPos;
    TLint4	activeStream;
    struct Language_Execute_PendingEvent	pendingEvent;
    struct Language_Execute_LibraryAbort	abort;
    TLint4	errno;
    TLstring	errMessage;
    TLint4	prevErrno;
    TLstring	prevErrMessage;
    TLaddressint	name;
    struct Language_Execute_ProcessDescriptor	*nextProcess;
    TLint4	dispatchPriority;
    TLnat4	waitParameter;
    struct Language_Execute_ConditionDescriptor	*waitCondition;
    struct Language_Execute_ProcessDescriptor	*nextTimeout;
    TLnat4	totalPause;
    struct Language_Execute_HandlerArea	*activeHandler;
    struct Language_Execute_HandlerArea	*runningHandler;
};
struct	Language_Execute_MonitorDescriptor {
    TLint2	entryParameter;
    TLint2	monitorPriority;
    struct Language_Execute_ProcessDescriptor	*entryHead;
    struct Language_Execute_ProcessDescriptor	*entryTail;
    TLaddressint	name;
    struct Language_Execute_ConditionDefinition	*firstCondition;
    struct Language_Execute_MonitorDescriptor	*nextMonitor;
    struct Language_Execute_MonitorDescriptor	*prevMonitor;
};
struct	Language_Execute_ConditionDescriptor {
    struct Language_Execute_ProcessDescriptor	*signalHead;
    struct Language_Execute_ProcessDescriptor	*signalTail;
    struct Language_Execute_MonitorDescriptor	*md;
};
typedef	struct Language_Execute_ConditionDescriptor	Language_Execute___x1618[65535];
struct	Language_Execute_ConditionDefinition {
    TLnat2	cdArraySize;
    Language_Execute___x1618	*cdArray;
    TLaddressint	name;
    struct Language_Execute_ConditionDefinition	*nextVariable;
};
extern struct Language_Execute_ProcessDescriptor	*Language_Execute_RQHead;
extern struct Language_Execute_ProcessDescriptor	*Language_Execute_EQHead;
extern TLnat4	Language_Execute_Chrono;
extern TLint4	Language_Execute_processCount;
extern TLaddressint	Language_Execute_globalPC;
extern TLaddressint	Language_Execute_globalSP;
typedef	Language_Opcode	Language_Execute___x1619[1];
extern Language_Execute___x1619	Language_Execute_nullRoutine;

void Language_Execute_RunSrcPosition ();

void Language_Execute_SetErrno ();

void Language_Execute_GetErrno ();
struct	Language_Execute___x1626 {
    TLint4	streamNo;
    TLstring	msg;
    struct SrcPosition	srcPos;
};
typedef	struct Language_Execute___x1626	Language_Execute___x1625[10];

void Language_Execute_AddFile ();

void Language_Execute_CloseFile ();

void Language_Execute_SetActive ();

extern TLboolean Language_Execute_Kernel_ConditionEmpty ();

extern TLnat4 Language_Execute_Kernel_GetProcessPriority ();

extern void Language_Execute_Kernel_Initialize ();

extern void Language_Execute_Kernel_InitializeCondition ();

extern void Language_Execute_Kernel_MonitorEnter ();

extern void Language_Execute_Kernel_MonitorExit ();

extern void Language_Execute_Kernel_MonitorFree ();

extern void Language_Execute_Kernel_MonitorInitialize ();

extern void Language_Execute_Kernel_Pause ();

extern void Language_Execute_Kernel_RunQueueDispatch ();

extern void Language_Execute_Kernel_RunQueueInsert ();

extern void Language_Execute_Kernel_SetProcessPriority ();

extern void Language_Execute_Kernel_SignalDeferredCondition ();

extern void Language_Execute_Kernel_SignalPriorityCondition ();

extern void Language_Execute_Kernel_SignalRegularCondition ();

extern void Language_Execute_Kernel_SignalTimeoutCondition ();

extern void Language_Execute_Kernel_TimeSlice ();

extern void Language_Execute_Kernel_WaitDeferredCondition ();

extern void Language_Execute_Kernel_WaitPriorityCondition ();

extern void Language_Execute_Kernel_WaitRegularCondition ();

extern void Language_Execute_Kernel_WaitTimeoutCondition ();

TLboolean Language_Execute_HasEvent ();

void Language_Execute_EventQueueInsert ();

TLboolean Language_Execute_Strintok ();

TLboolean Language_Execute_Strnatok ();

void Language_Execute_Open ();

void Language_Execute_Put ();

void Language_Execute_Get ();

void Language_Execute_DispatchHandler ();

extern void TLEABT ();

void Language_Execute_ExtraRoutines ();
extern TLint4	Language_Execute_quantaCount;

extern void Math_Arctan ();

extern void Math_Arctand ();

extern void Math_Ceil ();

extern void Math_Cos ();

extern void Math_Cosd ();

extern void Math_Exp ();

extern void Math_Floor ();

extern void Math_Intreal ();

extern void Math_Ln ();

extern void Math_Natreal ();

extern void Math_Rand ();

extern void Math_Randint ();

extern void Math_Randnext ();

extern void Math_Randomize ();

extern void Math_Randseed ();

extern void Math_Round ();

extern void Math_Sign ();

extern void Math_Sin ();

extern void Math_Sind ();

extern void Math_Sqrt ();

extern void String_Erealstr ();

extern void String_Frealstr ();

extern void String_Index ();

extern void String_Length ();

extern void String_Realstr ();

extern void String_Repeat ();

extern void String_Strreal ();

void Language_Execute_System_Clock ();

void Language_Execute_System_Date ();

void Language_Execute_System_Fetcharg ();

void Language_Execute_System_Flushstreams ();

void Language_Execute_System_Getenv ();

void Language_Execute_System_Getpid ();

void Language_Execute_System_Nargs ();

void Language_Execute_System_Setactive ();

void Language_Execute_System_Simutime ();

void Language_Execute_System_Sysclock ();

void Language_Execute_System_System ();

void Language_Execute_System_Time ();

void Language_Execute_System_Wallclock ();
extern TLint4	TL_TLI_TLIARC;

extern void TL_TLI_TLIFA ();

extern void TL_TLX_TLXGE ();

extern void TL_TLX_TLXDT ();

extern void TL_TLX_TLXTM ();

extern void TL_TLX_TLXCL ();

extern void TL_TLX_TLXSC ();

extern void time ();

extern void TL_TLX_TLXSYS ();

extern TLint4 getpid ();

extern void TL_TLI_TLIFS ();

extern void TL_TLK_TLKUEXIT ();
extern TLnat4	TL_TLK_TLKTIME;
extern TLnat4	TL_TLK_TLKEPOCH;

extern void TL_TLK_TLKUDMPA ();

extern void TL_TLK_TLKCINI ();
extern TLboolean	TL_TLK_TLKCLKON;
extern TLnat4	TL_TLK_TLKHZ;
extern TLnat4	TL_TLK_TLKCRESO;
extern TLnat4	TL_TLK_TLKTIME;
extern TLnat4	TL_TLK_TLKEPOCH;

extern void TL_TLK_TLKPSID ();

extern TLnat4 TL_TLK_TLKPGID ();

extern void TL_TLK_TLKRSETP ();

void Language_Execute_System_Clock (sp)
TLaddressint	sp;
{
    TLSTACKCHECK();
    TL_TLX_TLXCL(&((* (TLint4 *) ((* (TLaddressint *) sp)))));
}

void Language_Execute_System_Date (sp)
TLaddressint	sp;
{
    TLSTACKCHECK();
    (* (TLint2 *) ((* (TLaddressint *) sp))) = 0;
    TL_TLX_TLXDT((* (TLstring *) ((* (TLaddressint *) sp))));
}

void Language_Execute_System_Fetcharg (sp)
TLaddressint	sp;
{
    register TLint4	i;
    TLSTACKCHECK();
    i = (* (TLint4 *) ((unsigned long) sp + 4));
    if ((i >= 0) && (i <= Language_Execute_numArguments)) {
	(* (TLint2 *) ((* (TLaddressint *) sp))) = 0;
	TLSTRASS(255, (* (TLstring *) ((* (TLaddressint *) sp))), (*(Language_Execute_argumentNames[TLINRANGELOW(i, 0, 20, 1)])));
    } else {
	Language_Execute_ExecutionError_Abort((TLint4) 10000, "Argument to \'fetcharg\' is out of range");
    };
}

void Language_Execute_System_Flushstreams (sp)
TLaddressint	sp;
{
    TLSTACKCHECK();
    TL_TLI_TLIFS();
}

void Language_Execute_System_Getenv (sp)
TLaddressint	sp;
{
    TLSTACKCHECK();
    (* (TLint2 *) ((* (TLaddressint *) sp))) = 0;
    {
	TLstring	__x1945;
	TL_TLX_TLXGE((* (TLstring *) ((* (TLaddressint *) ((unsigned long) sp + 4)))), __x1945);
	TLSTRASS(255, (* (TLstring *) ((* (TLaddressint *) sp))), __x1945);
    };
}

void Language_Execute_System_Getpid (sp)
TLaddressint	sp;
{
    TLSTACKCHECK();
    (* (TLint4 *) ((* (TLaddressint *) sp))) = getpid();
}

void Language_Execute_System_Nargs (sp)
TLaddressint	sp;
{
    TLSTACKCHECK();
    (* (TLint4 *) ((* (TLaddressint *) sp))) = Language_Execute_numArguments;
}

void Language_Execute_System_Setactive (sp)
TLaddressint	sp;
{
    TLint4	stream;
    TLSTACKCHECK();
    stream = (* (TLint4 *) sp);
    if ((stream < (- 2)) || (stream > 20)) {
	Language_Execute_ExecutionError_Abort((TLint4) 10000, "Illegal stream number passed to \'setactive\'");
    };
    if (stream <= 0) {
	Language_Execute_RQHead->activeStream = 0;
	Language_Execute_SetActive(&(Language_Execute_RQHead->activeStream));
    } else {
	Language_Execute_RQHead->activeStream =  TLRANGECHECK (stream, 0, 20, 5);
	Language_Execute_SetActive(&(Language_Execute_RQHead->activeStream));
	if ((Language_Execute_RQHead->activeStream) == 0) {
	    Language_Execute_ExecutionError_Abort((TLint4) 10000, "Unopened or closed stream passed to \'setactive\'");
	};
    };
}

void Language_Execute_System_Simutime (sp)
TLaddressint	sp;
{
    TLSTACKCHECK();
    (* (TLint4 *) ((* (TLaddressint *) sp))) = Language_Execute_Chrono;
}

void Language_Execute_System_Sysclock (sp)
TLaddressint	sp;
{
    TLSTACKCHECK();
    TL_TLX_TLXSC(&((* (TLint4 *) ((* (TLaddressint *) sp)))));
}

void Language_Execute_System_System (sp)
TLaddressint	sp;
{
    TLSTACKCHECK();
    TL_TLX_TLXSYS((* (TLstring *) ((* (TLaddressint *) sp))), &((* (TLint4 *) ((* (TLaddressint *) ((unsigned long) sp + 4))))));
}

void Language_Execute_System_Time (sp)
TLaddressint	sp;
{
    TLSTACKCHECK();
    (* (TLint2 *) ((* (TLaddressint *) sp))) = 0;
    TL_TLX_TLXTM((* (TLstring *) ((* (TLaddressint *) sp))));
}

void Language_Execute_System_Wallclock (sp)
TLaddressint	sp;
{
    TLSTACKCHECK();
    time(&((* (TLint4 *) ((* (TLaddressint *) sp)))));
}

void Language_Execute_System () {
}
