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
typedef	TLchar	__x2307[256];
struct	ErrMsgDef {
    struct ErrMsgDef	*next;
    struct SrcPosition	srcPos;
    __x2307	text;
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
typedef	TLstring	__x2308[20];

extern void Language_EnterPreprocSymbols ();

extern void Language_CompileUnit ();

extern void Language_CompileProgram ();

extern void Language_WriteObjectFile ();
typedef	TLstring	__x2311[21];
typedef	TLstring	__x2315[21];

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
typedef	TLchar	Language___x2319[22];
typedef	Language___x2319	Language___x2318[255];
extern Language___x2318	Language_OpcodeName;
struct	Language_CodeHeader {
    TLaddressint	bodyCode;
};
struct	Language___x2321 {
    TLint4	lower, range;
};
typedef	struct Language___x2321	Language___x2320[255];
struct	Language_ArrayDescriptor {
    TLnat4	sizeArray;
    TLnat4	sizeElement;
    TLnat4	elementCount;
    TLnat4	numDimensions;
    Language___x2320	dim;
};
struct	Language_ForDescriptor {
    TLint4	counter;
    TLint4	right;
    TLint4	step;
    TLaddressint	savedSP;
};
typedef	Language_Offset	Language___x2322[1001];
struct	Language_CaseDescriptor {
    TLint4	lower, upper;
    Language_Offset	otherwise;
    Language___x2322	caseVector;
};
struct	Language_PointerDescriptor {
    TLaddressint	dataPtr;
    TLint4	timestamp;
};
typedef	TLnat2	Language___x2323[301];
struct	Language_UnionMapDescriptor {
    TLint4	lower, upper;
    Language___x2323	map;
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

extern void Language_DebugModule_Initialize ();

extern void Language_DebugModule_NewQueue ();

extern void Language_DebugModule_NewThread ();

extern void Language_DebugModule_NewCall ();

extern void Language_DebugModule_ReturnThreadDump ();

extern void Language_DebugModule_NewSymbolList ();

extern void Language_DebugModule_NewSymbol ();

extern void Language_DebugModule_ReturnSymbolDump ();

extern void Language_DebugModule_NewIndexList ();

extern void Language_DebugModule_NewIndex ();

extern void Language_DebugModule_ReturnIndexDump ();

extern void Language_Compile_Reset ();

extern void Language_Compile_Initialize ();
typedef	TLstring	__x2326[20];

extern void Language_Compile_EnterPreprocSymbols ();

extern void Language_Compile_ScanUnit ();

extern void Language_Compile_CompileUnit ();

extern void Language_Compile_InstallUnit ();

extern void Language_Compile_CallInfo ();

extern void Language_Compile_DumpUnitList ();

extern void Language_Compile_DumpScopeInfo ();

extern void Language_Compile_DumpSymbolInfo ();

extern void Language_Compile_DumpArrayBounds ();

extern void Language_Compile_DumpArrayValues ();

extern void Language_Compile_SetObjectFileStream ();
typedef	TLstring	__x2329[21];

extern void Language_Execute_Initialize ();

extern TLint4 Language_Execute_GetStackAllocated ();

extern void Language_Execute_Executor ();

extern void Language_Execute_EndExecution ();

extern void Language_Execute_RecoverAllMemory ();

extern void Language_Execute_SetStep ();

extern void Language_Execute_DumpThreadList ();

extern void Language_Execute_UnitInitState ();
extern TLint2	Language_Execute_executionTimeout;
extern TLboolean	Language_Execute_externalExecutionTimeoutFlag;
extern TLint4	Language_Execute_stackAllocated;
extern TLboolean	Language_Execute_allWaitingForKeyboard;
extern TLboolean	Language_Execute_allWaitingForMouse;
extern TLboolean	Language_Execute_allWaitingForDelay;
extern TLint4	Language_Execute_minimumDelayCount;

extern void TL_TLB_TLBUUN ();

extern void TL_TLB_TLBUNI ();

extern char *strcpy ();
typedef	TLint2	Language_Execute_ExecutionError_OverflowKind;

void Language_Execute_ExecutionError_Abort ();

void Language_Execute_ExecutionError_SetOverflowKind ();
typedef	TLnat4	Language_Execute_ExecutionError_TL_priority_t;
struct	Language_Execute_ExecutionError_TL_ExceptionInfo {
    TLint4	quitCode;
    TLint4	libraryQuitCode;
    TLstring	errorMsg;
};
typedef	TLnat1	Language_Execute_ExecutionError___x2332[1];
struct	Language_Execute_ExecutionError_TL_HandlerArea {
    TLint4	quitCode;
    TLnat4	lineAndFile;
    TLaddressint	fileTable;
    struct Language_Execute_ExecutionError_TL_HandlerArea	*nextHandler;
    Language_Execute_ExecutionError___x2332	savedState;
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
typedef	struct Language_Execute_ExecutionError_TL_ConditionDescriptor	Language_Execute_ExecutionError___x2335[65535];
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
typedef	TLchar	__x2336[4];

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
typedef	TLint1	Language_Execute___x2337[23];
extern Language_Execute___x2337	Language_Execute_fileMap;
typedef	Language_StringDef	*Language_Execute___x2340[21];
extern Language_Execute___x2340	Language_Execute_argumentNames;
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
    Language_Step	stepSpec;
    TLaddressint	stepFP;
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
typedef	struct Language_Execute_ConditionDescriptor	Language_Execute___x2341[65535];
struct	Language_Execute_ConditionDefinition {
    TLnat2	cdArraySize;
    Language_Execute___x2341	*cdArray;
    TLaddressint	name;
    struct Language_Execute_ConditionDefinition	*nextVariable;
};
extern struct Language_Execute_ProcessDescriptor	*Language_Execute_RQHead;
extern struct Language_Execute_ProcessDescriptor	*Language_Execute_EQHead;
extern TLnat4	Language_Execute_Chrono;
extern TLint4	Language_Execute_processCount;
extern TLaddressint	Language_Execute_globalPC;
extern TLaddressint	Language_Execute_globalSP;
typedef	Language_Opcode	Language_Execute___x2342[1];
extern Language_Execute___x2342	Language_Execute_nullRoutine;

void Language_Execute_RunSrcPosition ();

void Language_Execute_SetErrno ();

void Language_Execute_GetErrno ();
struct	Language_Execute___x2349 {
    TLint4	streamNo;
    TLstring	msg;
    struct SrcPosition	srcPos;
};
typedef	struct Language_Execute___x2349	Language_Execute___x2348[10];

void Language_Execute_AddFile ();

void Language_Execute_CloseFile ();

void Language_Execute_SetActive ();
extern TLaddressint	Language_Execute_savedInitPC;
extern TLaddressint	Language_Execute_savedInitFP;

void Language_Execute_DumpStack ();

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

extern void Language_Execute_Kernel_DumpThreadList ();

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

extern void Language_Execute_System_Clock ();

extern void Language_Execute_System_Date ();

extern void Language_Execute_System_Fetcharg ();

extern void Language_Execute_System_Flushstreams ();

extern void Language_Execute_System_Getenv ();

extern void Language_Execute_System_Getpid ();

extern void Language_Execute_System_Nargs ();

extern void Language_Execute_System_Setactive ();

extern void Language_Execute_System_Simutime ();

extern void Language_Execute_System_Sysclock ();

extern void Language_Execute_System_System ();

extern void Language_Execute_System_Time ();

extern void Language_Execute_System_Wallclock ();

extern void Limits_Getexp ();

extern void Limits_Rreb ();

extern void Limits_Setexp ();

extern void Language_Execute_Graphics_Drawpic ();

extern void Language_Execute_Graphics_Getch ();

extern void Language_Execute_Graphics_Hasch ();

extern void Language_Execute_Graphics_Sizepic ();

extern void Language_Execute_Graphics_Takepic ();

extern void Language_Execute_Event_Getevent ();

extern void Language_Execute_Event_Hasevent ();

extern void MT_InterpretIfc ();

extern void Star_initialize ();

extern void Star_getToken ();

extern void Star_finalize ();
struct	Language_Execute_RunDescriptor {
    Language_ReturnState	state;
    TLboolean	doQuit;
};
void Language_Execute_Run (runDesc)
struct Language_Execute_RunDescriptor	*runDesc;
{
    struct TLHAREA	quitCode;
    TLSTACKCHECK();
    if (TLHANDENTER(quitCode)) {
	if ((quitCode.quitCode == 1) || (Language_Execute_RQHead == ((struct Language_Execute_ProcessDescriptor *) 0))) {
	    TL_TLE_TLEQUIT ((TLint4) quitCode.quitCode, (char *) 0, 1);
	} else {
	    (*runDesc).state = 1;
	    (*runDesc).doQuit = 1;
	    Language_Execute_RQHead->abort.quitCode = quitCode.quitCode;
	};
    } else {
	register TLaddressint	fp;
	struct Language_Execute_ProcessDescriptor	*me;
	fp = (TLaddressint) (Language_Execute_RQHead->fp);
	me = Language_Execute_RQHead;
	Language_Execute_ExecutionError_SetOverflowKind((Language_Execute_ExecutionError_OverflowKind) 0);
	if (Language_debug) {
	    Language_Execute_executionTimeout = 10;
	} else {
	    Language_Execute_executionTimeout = 2000;
	};
	for(;;) {
	    TLnat2	op;
	    TLASSERT (Language_Execute_RQHead == me);
	    TLASSERT (((unsigned long) Language_Execute_globalPC & (1)) == 0);
	    op = (* (Language_Opcode *) Language_Execute_globalPC);
	    Language_Execute_globalPC += Language_codeOprSize;
	    if (Language_debug) {
		if ((unsigned long) (Language_Execute_RQHead->name) != 0) {
		    TL_TLI_TLISS ((TLint4) 0, (TLint2) 2);
		    TL_TLI_TLIPS ((TLint4) 0, "<", (TLint2) 0);
		    TL_TLI_TLIPS ((TLint4) 0, ((* (TLstring *) (Language_Execute_RQHead->name))), (TLint2) 0);
		    TL_TLI_TLIPS ((TLint4) 0, ">= ", (TLint2) 0);
		    TL_TLI_TLIPN ((TLint4) 0, (TLnat4) (* (TLnat4 *) &Language_Execute_RQHead), (TLint2) 0);
		    TL_TLI_TLIPS ((TLint4) 0, " ", (TLint2) 0);
		};
		TLASSERT ((unsigned long) Language_Execute_globalSP <= (* (TLnat4 *) &Language_Execute_RQHead));
		TL_TLI_TLISS ((TLint4) 0, (TLint2) 2);
		TL_TLI_TLIPS ((TLint4) 0, " PC=", (TLint2) 0);
		TL_TLI_TLIPI ((TLint4) 0, (TLint4) ((unsigned long) Language_Execute_globalPC - Language_codeOprSize), (TLint2) 0);
		TL_TLI_TLIPS ((TLint4) 0, " OPCODE=", (TLint2) 0);
		TL_TLI_TLIPS ((TLint4) 0, (Language_OpcodeName[TLINRANGELOW(op, 0, 254, 1)]), (TLint2) 0);
		TL_TLI_TLIPS ((TLint4) 0, " FP=", (TLint2) 0);
		TL_TLI_TLIPN ((TLint4) 0, (TLnat4) fp, (TLint2) 0);
		TL_TLI_TLIPS ((TLint4) 0, " SP=", (TLint2) 0);
		TL_TLI_TLIPN ((TLint4) 0, (TLnat4) Language_Execute_globalSP, (TLint2) 0);
		TL_TLI_TLIPS ((TLint4) 0, " (", (TLint2) 0);
		if (((unsigned long) Language_Execute_globalSP + 16) <= (* (TLnat4 *) &Language_Execute_RQHead)) {
		    TL_TLI_TLISS ((TLint4) 0, (TLint2) 2);
		    TL_TLI_TLIPS ((TLint4) 0, "...,", (TLint2) 0);
		    TL_TLI_TLIPN ((TLint4) 0, (TLnat4) ((* (TLnat4 *) ((unsigned long) Language_Execute_globalSP + 8))), (TLint2) 0);
		    TL_TLI_TLIPS ((TLint4) 0, ",", (TLint2) 0);
		    TL_TLI_TLIPN ((TLint4) 0, (TLnat4) ((* (TLnat4 *) ((unsigned long) Language_Execute_globalSP + 4))), (TLint2) 0);
		    TL_TLI_TLIPS ((TLint4) 0, ",", (TLint2) 0);
		    TL_TLI_TLIPN ((TLint4) 0, (TLnat4) ((* (TLnat4 *) Language_Execute_globalSP)), (TLint2) 0);
		} else {
		    if (((unsigned long) Language_Execute_globalSP + 12) <= (* (TLnat4 *) &Language_Execute_RQHead)) {
			TL_TLI_TLISS ((TLint4) 0, (TLint2) 2);
			TL_TLI_TLIPN ((TLint4) 0, (TLnat4) ((* (TLnat4 *) ((unsigned long) Language_Execute_globalSP + 8))), (TLint2) 0);
			TL_TLI_TLIPS ((TLint4) 0, ",", (TLint2) 0);
			TL_TLI_TLIPN ((TLint4) 0, (TLnat4) ((* (TLnat4 *) ((unsigned long) Language_Execute_globalSP + 4))), (TLint2) 0);
			TL_TLI_TLIPS ((TLint4) 0, ",", (TLint2) 0);
			TL_TLI_TLIPN ((TLint4) 0, (TLnat4) ((* (TLnat4 *) Language_Execute_globalSP)), (TLint2) 0);
		    } else {
			if (((unsigned long) Language_Execute_globalSP + 8) <= (* (TLnat4 *) &Language_Execute_RQHead)) {
			    TL_TLI_TLISS ((TLint4) 0, (TLint2) 2);
			    TL_TLI_TLIPN ((TLint4) 0, (TLnat4) ((* (TLnat4 *) ((unsigned long) Language_Execute_globalSP + 4))), (TLint2) 0);
			    TL_TLI_TLIPS ((TLint4) 0, ",", (TLint2) 0);
			    TL_TLI_TLIPN ((TLint4) 0, (TLnat4) ((* (TLnat4 *) Language_Execute_globalSP)), (TLint2) 0);
			} else {
			    if (((unsigned long) Language_Execute_globalSP + 4) <= (* (TLnat4 *) &Language_Execute_RQHead)) {
				TL_TLI_TLISS ((TLint4) 0, (TLint2) 2);
				TL_TLI_TLIPN ((TLint4) 0, (TLnat4) ((* (TLnat4 *) Language_Execute_globalSP)), (TLint2) 0);
			    };
			};
		    };
		};
		TL_TLI_TLISS ((TLint4) 0, (TLint2) 2);
		TL_TLI_TLIPS ((TLint4) 0, ")", (TLint2) 0);
		TL_TLI_TLIPK ((TLint2) 0);
	    };
	    switch (op) {
		case 0:
		case 1:
		    {
			if ((op == 0) || (((* (TLint4 *) Language_Execute_globalSP)) == 0)) {
			    switch ((* (TLint1 *) Language_Execute_globalPC)) {
				case 1:
				    {
					TLEABT((TLint4) 21);
				    }
				    break;
				case 2:
				    {
					TLEABT((TLint4) 16);
				    }
				    break;
				case 3:
				    {
					TLEABT((TLint4) 17);
				    }
				    break;
				case 4:
				    {
					TLEABT((TLint4) 19);
				    }
				    break;
				case 5:
				    {
					TLEABT((TLint4) 18);
				    }
				    break;
				case 6:
				    {
					Language_Execute_RQHead->srcPos.lineNo = (* (TLnat2 *) (((unsigned long) fp + 4) + 2));
					Language_Execute_RQHead->srcPos.fileNo = (* (TLnat2 *) ((unsigned long) fp + 4));
					TLEABT((TLint4) 20);
				    }
				    break;
				case 7:
				    {
					Language_Execute_ExecutionError_Abort((TLint4) 10000, "Cannot convert pointer to more specific class");
				    }
				    break;
				case 8:
				    {
					TLEABT((TLint4) 25);
				    }
				    break;
				case 9:
				    {
					TLEABT((TLint4) 26);
				    }
				    break;
				default:
				    TLCASEABORT;
			    };
			};
			Language_Execute_globalSP += 4;
			Language_Execute_globalPC += Language_codeInt1Size;
		    }
		    break;
		case 2:
		    {
			(* (TLint4 *) Language_Execute_globalSP) =  abs(((* (TLint4 *) Language_Execute_globalSP)));
		    }
		    break;
		case 3:
		    {
			(* (TLreal8 *) Language_Execute_globalSP) =  fabs(((* (TLreal8 *) Language_Execute_globalSP)));
		    }
		    break;
		case 4:
		    {
			register TLint4	rVal;
			register TLint4	lVal;
			rVal = (* (TLint4 *) Language_Execute_globalSP);
			Language_Execute_globalSP += 4;
			lVal = (* (TLint4 *) Language_Execute_globalSP);
			(* (TLint4 *) Language_Execute_globalSP) += rVal;
			if (((rVal < 0) && (((* (TLint4 *) Language_Execute_globalSP)) > lVal)) || ((rVal > 0) && (((* (TLint4 *) Language_Execute_globalSP)) < lVal))) {
			    TLEABT((TLint4) 50);
			};
		    }
		    break;
		case 5:
		    {
			register TLnat4	rVal;
			register TLint4	lVal;
			rVal = (* (TLnat4 *) Language_Execute_globalSP);
			Language_Execute_globalSP += 4;
			lVal = (* (TLint4 *) Language_Execute_globalSP);
			(* (TLint4 *) Language_Execute_globalSP) += rVal;
			if (((* (TLint4 *) Language_Execute_globalSP)) < lVal) {
			    TLEABT((TLint4) 50);
			};
		    }
		    break;
		case 6:
		    {
			register TLnat4	rVal;
			rVal = (* (TLnat4 *) Language_Execute_globalSP);
			Language_Execute_globalSP += 4;
			(* (TLnat4 *) Language_Execute_globalSP) += rVal;
			if (((* (TLnat4 *) Language_Execute_globalSP)) < rVal) {
			    TLEABT((TLint4) 50);
			};
		    }
		    break;
		case 7:
		    {
			register TLint4	rVal;
			register TLnat4	lVal;
			rVal = (* (TLint4 *) Language_Execute_globalSP);
			Language_Execute_globalSP += 4;
			lVal = (* (TLnat4 *) Language_Execute_globalSP);
			(* (TLnat4 *) Language_Execute_globalSP) += rVal;
			if (((rVal < 0) && (((* (TLnat4 *) Language_Execute_globalSP)) > lVal)) || ((rVal > 0) && (((* (TLnat4 *) Language_Execute_globalSP)) < lVal))) {
			    TLEABT((TLint4) 50);
			};
		    }
		    break;
		case 8:
		    {
			register TLreal8	rVal;
			rVal = (* (TLreal8 *) Language_Execute_globalSP);
			Language_Execute_globalSP += 8;
			if ((((rVal < 0) && (((* (TLreal8 *) Language_Execute_globalSP)) < 0)) || ((rVal > 0) && (((* (TLreal8 *) Language_Execute_globalSP)) > 0))) && (( fabs(((* (TLreal8 *) Language_Execute_globalSP)))) > (.1797693134862315517e307 - ( TLSIMPLEABS(rVal))))) {
			    TLEABT((TLint4) 51);
			};
			(* (TLreal8 *) Language_Execute_globalSP) += rVal;
		    }
		    break;
		case 9:
		    {
			Language_Offset	sz;
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			sz = (* (Language_Offset *) Language_Execute_globalPC);
			Language_Execute_globalPC += Language_codeOffsetSize;
			if (sz <= 4) {
			    (* (Language_set32 *) ((unsigned long) Language_Execute_globalSP + 4)) |= (* (Language_set32 *) Language_Execute_globalSP);
			    Language_Execute_globalSP += 4;
			} else {
			    register TLaddressint	t;
			    register TLaddressint	r;
			    register TLaddressint	l;
			    t = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			    r = (TLaddressint) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4)));
			    l = (TLaddressint) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + (2 * 4))));
			    {
				register TLint4	__x1991;
				TLint4	__x2705;
				__x2705 = sz >> 1;
				__x1991 = 1;
				if (__x1991 <= __x2705) {
				    for(;;) {
					(* (Language_set16 *) t) = ((* (Language_set16 *) l)) | ((* (Language_set16 *) r));
					t += 2;
					l += 2;
					r += 2;
					if (__x1991 == __x2705) break;
					__x1991++;
				    }
				};
			    };
			    (* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + (2 * 4))) = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			    Language_Execute_globalSP += 8;
			};
		    }
		    break;
		case 10:
		    {
			TLaddressint	uninitAddr;
			TLBINDREG((*desc), struct Language_ArrayDescriptor);
			register TLaddressint	ref;
			TLaddressint	memAddr;
			uninitAddr = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			Language_Execute_globalSP += 4;
			desc = (void *) ((* (TLaddressint *) Language_Execute_globalSP));
			Language_Execute_globalSP += 4;
			ref = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			Language_Execute_globalSP += 4;
			Language_Execute_Heap_Alloc(&(memAddr), (TLnat4) (((*desc).sizeArray) + 4));
			if ((unsigned long) memAddr == 0) {
			    Language_Execute_ExecutionError_Abort((TLint4) 10000, "Not enough memory for dynamic array allocation");
			};
			(* (TLaddressint *) memAddr) = (TLaddressint) ref;
			(* (TLaddressint *) ref) = (TLaddressint) ((unsigned long) memAddr + 4);
			if ((unsigned long) uninitAddr != 0) {
			    TL_TLB_TLBUNI((TLaddressint) ((* (TLaddressint *) ref)), (TLaddressint) uninitAddr, (TLint4) ((*desc).elementCount));
			};
		    }
		    break;
		case 11:
		    {
			register TLint4	cSize;
			register TLaddressint	cAddr;
			cSize = (* (TLint4 *) Language_Execute_globalSP);
			Language_Execute_globalSP += 4;
			cAddr = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			Language_Execute_globalSP += 4;
			cSize = (cSize + 3) & (3 ^ ((TLnat4) (- 1)));
			Language_Execute_Heap_Alloc(&((* (TLaddressint *) cAddr)), (TLnat4) cSize);
			if ((unsigned long) ((* (TLaddressint *) cAddr)) == 0) {
			    Language_Execute_ExecutionError_Abort((TLint4) 10000, "Not enough memory for dynamic allocation");
			};
		    }
		    break;
		case 12:
		    {
			TLaddressint	uninitAddr;
			TLBINDREG((*desc), struct Language_ArrayDescriptor);
			register TLaddressint	ref;
			uninitAddr = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			Language_Execute_globalSP += 4;
			desc = (void *) ((* (TLaddressint *) Language_Execute_globalSP));
			Language_Execute_globalSP += 4;
			ref = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			Language_Execute_globalSP += 4;
			if (((*desc).elementCount) == 0) {
			    TLEABT((TLint4) 12);
			};
			Language_Execute_Heap_Alloc(&((* (TLaddressint *) ref)), (TLnat4) ((*desc).sizeArray));
			if ((unsigned long) ((* (TLaddressint *) ref)) == 0) {
			    Language_Execute_ExecutionError_Abort((TLint4) 10000, "Not enough memory for dynamic array allocation");
			};
			if ((unsigned long) uninitAddr != 0) {
			    TL_TLB_TLBUNI((TLaddressint) ((* (TLaddressint *) ref)), (TLaddressint) uninitAddr, (TLint4) ((*desc).elementCount));
			};
		    }
		    break;
		case 13:
		    {
			register TLint4	cSize;
			register TLaddressint	cAddr;
			cSize = (* (TLint4 *) Language_Execute_globalSP);
			Language_Execute_globalSP += 4;
			cAddr = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			Language_Execute_globalSP += 4;
			cSize = (cSize + 3) & (3 ^ ((TLnat4) (- 1)));
			if (((unsigned long) Language_Execute_globalSP - (unsigned long) (Language_Execute_RQHead->minimumSP)) < cSize) {
			    {
				TLstring	__x2740;
				TL_TLS_TLSVIS((TLint4) Language_Execute_stackAllocated, (TLint4) 1, (TLint4) 10, __x2740);
				{
				    TLstring	__x2739;
				    TL_TLS_TLSVIS((TLint4) ((unsigned long) Language_Execute_globalSP - (unsigned long) (Language_Execute_RQHead->minimumSP)), (TLint4) 1, (TLint4) 10, __x2739);
				    {
					TLstring	__x2738;
					TL_TLS_TLSVIS((TLint4) cSize, (TLint4) 1, (TLint4) 10, __x2738);
					{
					    TLstring	__x2737;
					    TL_TLS_TLSCAT("Stack overflow during dynamic allocation.  ", "Attempted to allocate ", __x2737);
					    {
						TLstring	__x2736;
						TL_TLS_TLSCAT(__x2737, __x2738, __x2736);
						{
						    TLstring	__x2735;
						    TL_TLS_TLSCAT(__x2736, " bytes in dynamic array with ", __x2735);
						    {
							TLstring	__x2734;
							TL_TLS_TLSCAT(__x2735, __x2739, __x2734);
							{
							    TLstring	__x2733;
							    TL_TLS_TLSCAT(__x2734, " bytes left of stack space. (Original stack size: ", __x2733);
							    {
								TLstring	__x2732;
								TL_TLS_TLSCAT(__x2733, __x2740, __x2732);
								{
								    TLstring	__x2731;
								    TL_TLS_TLSCAT(__x2732, " bytes)", __x2731);
								    Language_Execute_ExecutionError_Abort((TLint4) 10000, __x2731);
								};
							    };
							};
						    };
						};
					    };
					};
				    };
				};
			    };
			};
			Language_Execute_globalSP -= cSize;
			(* (TLaddressint *) cAddr) = (TLaddressint) Language_Execute_globalSP;
		    }
		    break;
		case 14:
		    {
			TLaddressint	uninitAddr;
			TLBINDREG((*desc), struct Language_ArrayDescriptor);
			register TLaddressint	ref;
			uninitAddr = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			Language_Execute_globalSP += 4;
			desc = (void *) ((* (TLaddressint *) Language_Execute_globalSP));
			Language_Execute_globalSP += 4;
			ref = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			Language_Execute_globalSP += 4;
			if (((*desc).elementCount) == 0) {
			    TLEABT((TLint4) 12);
			};
			if (((unsigned long) Language_Execute_globalSP - (unsigned long) (Language_Execute_RQHead->minimumSP)) < ((*desc).sizeArray)) {
			    {
				TLstring	__x2755;
				TL_TLS_TLSVIS((TLint4) Language_Execute_stackAllocated, (TLint4) 1, (TLint4) 10, __x2755);
				{
				    TLstring	__x2754;
				    TL_TLS_TLSVIS((TLint4) ((unsigned long) Language_Execute_globalSP - (unsigned long) (Language_Execute_RQHead->minimumSP)), (TLint4) 1, (TLint4) 10, __x2754);
				    {
					TLstring	__x2753;
					TL_TLS_TLSVIS((TLint4) ((*desc).sizeArray), (TLint4) 1, (TLint4) 10, __x2753);
					{
					    TLstring	__x2752;
					    TL_TLS_TLSCAT("Stack overflow during dynamic allocation.  ", "Attempted to allocate ", __x2752);
					    {
						TLstring	__x2751;
						TL_TLS_TLSCAT(__x2752, __x2753, __x2751);
						{
						    TLstring	__x2750;
						    TL_TLS_TLSCAT(__x2751, " bytes in dynamic array with ", __x2750);
						    {
							TLstring	__x2749;
							TL_TLS_TLSCAT(__x2750, __x2754, __x2749);
							{
							    TLstring	__x2748;
							    TL_TLS_TLSCAT(__x2749, " bytes left of stack space. (Original stack size: ", __x2748);
							    {
								TLstring	__x2747;
								TL_TLS_TLSCAT(__x2748, __x2755, __x2747);
								{
								    TLstring	__x2746;
								    TL_TLS_TLSCAT(__x2747, " bytes)", __x2746);
								    Language_Execute_ExecutionError_Abort((TLint4) 10000, __x2746);
								};
							    };
							};
						    };
						};
					    };
					};
				    };
				};
			    };
			};
			Language_Execute_globalSP -= (*desc).sizeArray;
			(* (TLaddressint *) ref) = (TLaddressint) Language_Execute_globalSP;
			if ((unsigned long) uninitAddr != 0) {
			    TL_TLB_TLBUNI((TLaddressint) Language_Execute_globalSP, (TLaddressint) uninitAddr, (TLint4) ((*desc).elementCount));
			};
		    }
		    break;
		case 15:
		    {
			Language_Execute_globalSP += 4;
			(* (TLnat4 *) Language_Execute_globalSP) &= (* (TLnat4 *) ((unsigned long) Language_Execute_globalSP - 4));
		    }
		    break;
		case 16:
		    {
			register TLint4	dim;
			TLBINDREG((*desc), struct Language_ArrayDescriptor);
			TLASSERT (((unsigned long) Language_Execute_globalPC & (1)) == 0);
			dim = (* (TLint2 *) Language_Execute_globalPC);
			desc = (void *) ((* (TLaddressint *) Language_Execute_globalSP));
			if ((dim > ((*desc).numDimensions)) || (dim <= 0)) {
			    Language_Execute_ExecutionError_Abort((TLint4) 10000, "Array dimension passed to \'upper\' is out of range");
			};
			Language_Execute_globalSP += 0;
			(* (TLint4 *) Language_Execute_globalSP) = (((*desc).dim[TLINRANGELOW(dim, 1, 255, 1)].range) + ((*desc).dim[TLINRANGELOW(dim, 1, 255, 1)].lower)) - 1;
			Language_Execute_globalPC += Language_codeInt2Size;
		    }
		    break;
		case 17:
		    {
			(* (TLaddressint *) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4)))) = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			Language_Execute_globalSP += 8;
		    }
		    break;
		case 18:
		    {
			(* (TLaddressint *) ((* (TLaddressint *) Language_Execute_globalSP))) = (TLaddressint) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4)));
			Language_Execute_globalSP += 8;
		    }
		    break;
		case 19:
		    {
			register TLint4	value;
			value = (* (TLint4 *) Language_Execute_globalSP);
			if (value == (TLint4) -0x80000000) {
			    TLEABT((TLint4) 50);
			};
			(* (TLint4 *) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4)))) = value;
			Language_Execute_globalSP += 8;
		    }
		    break;
		case 20:
		    {
			register TLint4	value;
			value = (* (TLint4 *) ((unsigned long) Language_Execute_globalSP + 4));
			if (value == (TLint4) -0x80000000) {
			    TLEABT((TLint4) 50);
			};
			(* (TLint4 *) ((* (TLaddressint *) Language_Execute_globalSP))) = value;
			Language_Execute_globalSP += 8;
		    }
		    break;
		case 21:
		    {
			register TLint4	value;
			register TLaddressint	tAddr;
			value = (* (TLint4 *) Language_Execute_globalSP);
			tAddr = (TLaddressint) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4)));
			(* (TLint1 *) tAddr) = value;
			if (((* (TLint1 *) tAddr)) != value) {
			    TLEABT((TLint4) 15);
			};
			Language_Execute_globalSP += 8;
		    }
		    break;
		case 22:
		    {
			register TLaddressint	tAddr;
			register TLint4	value;
			tAddr = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			value = (* (TLint4 *) ((unsigned long) Language_Execute_globalSP + 4));
			(* (TLint1 *) tAddr) = value;
			if (((* (TLint1 *) tAddr)) != value) {
			    TLEABT((TLint4) 15);
			};
			Language_Execute_globalSP += 8;
		    }
		    break;
		case 23:
		    {
			register TLint4	value;
			register TLaddressint	tAddr;
			value = (* (TLint4 *) Language_Execute_globalSP);
			tAddr = (TLaddressint) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4)));
			(* (TLint2 *) tAddr) = value;
			if (((* (TLint2 *) tAddr)) != value) {
			    TLEABT((TLint4) 15);
			};
			Language_Execute_globalSP += 8;
		    }
		    break;
		case 24:
		    {
			register TLaddressint	tAddr;
			register TLint4	value;
			tAddr = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			value = (* (TLint4 *) ((unsigned long) Language_Execute_globalSP + 4));
			(* (TLint2 *) tAddr) = value;
			if (((* (TLint2 *) tAddr)) != value) {
			    TLEABT((TLint4) 15);
			};
			Language_Execute_globalSP += 8;
		    }
		    break;
		case 25:
		    {
			(* (TLint4 *) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4)))) = (* (TLint4 *) Language_Execute_globalSP);
			Language_Execute_globalSP += 8;
		    }
		    break;
		case 26:
		    {
			(* (TLint4 *) ((* (TLaddressint *) Language_Execute_globalSP))) = (* (TLint4 *) ((unsigned long) Language_Execute_globalSP + 4));
			Language_Execute_globalSP += 8;
		    }
		    break;
		case 27:
		    {
			register TLnat4	value;
			value = (* (TLnat4 *) Language_Execute_globalSP);
			if (value == 0xFFFFFFFF) {
			    TLEABT((TLint4) 50);
			};
			(* (TLnat4 *) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4)))) = value;
			Language_Execute_globalSP += 8;
		    }
		    break;
		case 28:
		    {
			register TLnat4	value;
			value = (* (TLnat4 *) ((unsigned long) Language_Execute_globalSP + 4));
			if (value == 0xFFFFFFFF) {
			    TLEABT((TLint4) 50);
			};
			(* (TLnat4 *) ((* (TLaddressint *) Language_Execute_globalSP))) = value;
			Language_Execute_globalSP += 8;
		    }
		    break;
		case 29:
		    {
			register TLnat4	value;
			register TLaddressint	tAddr;
			value = (* (TLnat4 *) Language_Execute_globalSP);
			tAddr = (TLaddressint) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4)));
			(* (TLnat1 *) tAddr) = value;
			if (((* (TLnat1 *) tAddr)) != value) {
			    TLEABT((TLint4) 15);
			};
			Language_Execute_globalSP += 8;
		    }
		    break;
		case 30:
		    {
			register TLaddressint	tAddr;
			register TLnat4	value;
			tAddr = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			value = (* (TLnat4 *) ((unsigned long) Language_Execute_globalSP + 4));
			(* (TLnat1 *) tAddr) = value;
			if (((* (TLnat1 *) tAddr)) != value) {
			    TLEABT((TLint4) 15);
			};
			Language_Execute_globalSP += 8;
		    }
		    break;
		case 31:
		    {
			register TLnat4	value;
			register TLaddressint	tAddr;
			value = (* (TLnat4 *) Language_Execute_globalSP);
			tAddr = (TLaddressint) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4)));
			(* (TLnat2 *) tAddr) = value;
			if (((* (TLnat2 *) tAddr)) != value) {
			    TLEABT((TLint4) 15);
			};
			Language_Execute_globalSP += 8;
		    }
		    break;
		case 32:
		    {
			register TLaddressint	tAddr;
			register TLnat4	value;
			tAddr = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			value = (* (TLnat4 *) ((unsigned long) Language_Execute_globalSP + 4));
			(* (TLnat2 *) tAddr) = value;
			if (((* (TLnat2 *) tAddr)) != value) {
			    TLEABT((TLint4) 15);
			};
			Language_Execute_globalSP += 8;
		    }
		    break;
		case 33:
		    {
			(* (TLnat4 *) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4)))) = (* (TLnat4 *) Language_Execute_globalSP);
			Language_Execute_globalSP += 8;
		    }
		    break;
		case 34:
		    {
			(* (TLnat4 *) ((* (TLaddressint *) Language_Execute_globalSP))) = (* (TLnat4 *) ((unsigned long) Language_Execute_globalSP + 4));
			Language_Execute_globalSP += 8;
		    }
		    break;
		case 35:
		    {

			extern void memcpy ();
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			memcpy((TLaddressint) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4))), (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP)), (TLint4) ((* (Language_Offset *) Language_Execute_globalPC)));
			Language_Execute_globalSP += 8;
			Language_Execute_globalPC += Language_codeOffsetSize;
		    }
		    break;
		case 36:
		    {

			extern void memcpy ();
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			memcpy((TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP)), (TLaddressint) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4))), (TLint4) ((* (Language_Offset *) Language_Execute_globalPC)));
			Language_Execute_globalSP += 8;
			Language_Execute_globalPC += Language_codeOffsetSize;
		    }
		    break;
		case 37:
		    {
			TLBINDREG((*p), struct Language_PointerDescriptor);
			p = (void *) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4)));
			(*p).dataPtr = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			if ((unsigned long) ((*p).dataPtr) != 0) {
			    (*p).timestamp = (* (TLint4 *) ((unsigned long) ((*p).dataPtr) - 4));
			};
			Language_Execute_globalSP += 8;
		    }
		    break;
		case 38:
		    {
			TLBINDREG((*p), struct Language_PointerDescriptor);
			p = (void *) ((* (TLaddressint *) Language_Execute_globalSP));
			(*p).dataPtr = (TLaddressint) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4)));
			if ((unsigned long) ((*p).dataPtr) != 0) {
			    (*p).timestamp = (* (TLint4 *) ((unsigned long) ((*p).dataPtr) - 4));
			};
			Language_Execute_globalSP += 8;
		    }
		    break;
		case 39:
		    {
			(* (TLreal8 *) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 8)))) = (* (TLreal8 *) Language_Execute_globalSP);
			Language_Execute_globalSP += 12;
		    }
		    break;
		case 40:
		    {
			(* (TLreal8 *) ((* (TLaddressint *) Language_Execute_globalSP))) = (* (TLreal8 *) ((unsigned long) Language_Execute_globalSP + 4));
			Language_Execute_globalSP += 12;
		    }
		    break;
		case 41:
		    {
			(* (TLreal4 *) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 8)))) = (* (TLreal8 *) Language_Execute_globalSP);
			Language_Execute_globalSP += 12;
		    }
		    break;
		case 42:
		    {
			(* (TLreal4 *) ((* (TLaddressint *) Language_Execute_globalSP))) = (* (TLreal8 *) ((unsigned long) Language_Execute_globalSP + 4));
			Language_Execute_globalSP += 12;
		    }
		    break;
		case 43:
		    {
			(* (TLreal8 *) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 8)))) = (* (TLreal8 *) Language_Execute_globalSP);
			Language_Execute_globalSP += 12;
		    }
		    break;
		case 44:
		    {
			(* (TLreal8 *) ((* (TLaddressint *) Language_Execute_globalSP))) = (* (TLreal8 *) ((unsigned long) Language_Execute_globalSP + 4));
			Language_Execute_globalSP += 12;
		    }
		    break;
		case 45:
		case 46:
		    {
			register TLaddressint	target;
			register TLaddressint	source;
			register TLint4	maxLen;
			TLstring	tstring;
			maxLen = (* (TLint4 *) Language_Execute_globalSP);
			if (op == 45) {
			    source = (TLaddressint) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4)));
			    target = (TLaddressint) ((* (TLaddressint *) (((unsigned long) Language_Execute_globalSP + 4) + 4)));
			} else {
			    target = (TLaddressint) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4)));
			    source = (TLaddressint) ((* (TLaddressint *) (((unsigned long) Language_Execute_globalSP + 4) + 4)));
			};
			if (TL_TLS_TLSLEN(((* (TLstring *) source))) > maxLen) {
			    TLEABT((TLint4) 35);
			};
			strcpy(tstring, (* (TLstring *) source));
			(* (TLnat1 *) target) = 0;
			strcpy((* (TLstring *) target), tstring);
			Language_Execute_globalSP += 12;
		    }
		    break;
		case 50:
		    {
			register TLaddressint	newPC;
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			newPC = (TLaddressint) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + ((* (Language_Offset *) Language_Execute_globalPC)))));
			Language_Execute_globalPC += Language_codeOffsetSize;
			if (((4 == 4) && ((unsigned long) newPC == 0xFFFFFFFF)) || ((4 == 2) && ((unsigned long) newPC == 65535))) {
			    TLEABT((TLint4) 45);
			} else {
			    if ((unsigned long) newPC == 0) {
				Language_Execute_ExecutionError_Abort((TLint4) 10000, "Deferred subroutine has not been resolved");
			    };
			};
			Language_Execute_globalSP -= 4;
			(* (TLaddressint *) Language_Execute_globalSP) = (TLaddressint) Language_Execute_globalPC;
			if ((unsigned long) Language_Execute_savedInitPC == 0) {
			    Language_Execute_savedInitPC = (TLaddressint) Language_Execute_globalPC;
			    Language_Execute_savedInitFP = (TLaddressint) fp;
			};
			Language_Execute_globalPC = (TLaddressint) newPC;
			if (((Language_Execute_RQHead->stepSpec) == 2) && ((unsigned long) (Language_Execute_RQHead->stepFP) == 0)) {
			    Language_Execute_RQHead->stepFP = (TLaddressint) fp;
			};
		    }
		    break;
		case 51:
		    {
			struct	LookupEntry {
    TLaddressint	procName;
    void 	(*procRoutine)();
};
			typedef	struct LookupEntry	__x2869[10000];
			typedef	__x2869	LookupTable;
			extern LookupTable	externLookupTable;
			extern TLint4	externLookupCount;
			TLint4	externIndex;
			Language_Execute_SetErrno((TLint4) 0, (TLaddressint) 0);
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			externIndex = (* (Language_Offset *) Language_Execute_globalPC);
			TLASSERT (externIndex < externLookupCount);
			(externLookupTable[TLINRANGELOW(externIndex, 0, 9999, 1)].procRoutine)((TLaddressint) Language_Execute_globalSP);
			if (Language_Execute_RQHead != me) {
			    Language_Execute_globalPC -= Language_codeOprSize;
			} else {
			    Language_Execute_globalPC += Language_codeOffsetSize;
			};
		    }
		    break;
		case 52:
		    {
			TLaddressint	newPC;
			Language_Execute_globalPC += Language_codeOffsetSize;
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			newPC = (TLaddressint) ((* (struct Language_CodeHeader *) ((* (TLaddressint *) Language_Execute_globalPC))).bodyCode);
			TLASSERT ((unsigned long) newPC != 0);
			Language_Execute_globalSP -= 4;
			(* (TLaddressint *) Language_Execute_globalSP) = (TLaddressint) ((unsigned long) Language_Execute_globalPC + Language_codeOffsetSize);
			Language_Execute_globalPC = (TLaddressint) newPC;
		    }
		    break;
		case 53:
		    {
			register TLint4	selector;
			TLaddressint	descAddr;
			TLBINDREG((*desc), struct Language_CaseDescriptor);
			selector = (* (TLint4 *) Language_Execute_globalSP);
			Language_Execute_globalSP += 4;
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			descAddr = (TLaddressint) ((unsigned long) Language_Execute_globalPC + ((* (Language_Offset *) Language_Execute_globalPC)));
			TLASSERT (((unsigned long) descAddr & (3)) == 0);
			desc = (void *) descAddr;
			if ((selector < ((*desc).lower)) || (selector > ((*desc).upper))) {
			    Language_Execute_globalPC += (*desc).otherwise;
			} else {
			    Language_Execute_globalPC += (*desc).caseVector[TLINRANGELOW(selector - ((*desc).lower), 0, 1000, 1)];
			};
		    }
		    break;
		case 54:
		    {
			register TLaddressint	t;
			register TLaddressint	r;
			register TLaddressint	l;
			t = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			r = (TLaddressint) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4)));
			l = (TLaddressint) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + (2 * 4))));
			if ((TL_TLS_TLSLEN(((* (TLstring *) l))) + TL_TLS_TLSLEN(((* (TLstring *) r)))) > (255)) {
			    Language_Execute_ExecutionError_Abort((TLint4) 10000, "String generated by string catenation too long");
			};
			{
			    TLstring	__x2883;
			    TL_TLS_TLSCAT((* (TLstring *) l), (* (TLstring *) r), __x2883);
			    TLSTRASS(255, (* (TLstring *) t), __x2883);
			};
			Language_Execute_globalSP += 8;
			(* (TLaddressint *) Language_Execute_globalSP) = (TLaddressint) t;
		    }
		    break;
		case 62:
		    {
			register TLaddressint	stkAddr;
			TLboolean	abort;
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			stkAddr = (TLaddressint) ((unsigned long) Language_Execute_globalSP + ((* (Language_Offset *) Language_Execute_globalPC)));
			Language_Execute_globalPC += Language_codeOffsetSize;
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			if (((* (TLint4 *) Language_Execute_globalPC)) <= ((* (TLint4 *) ((unsigned long) Language_Execute_globalPC + Language_codeIntSize)))) {
			    abort = (((* (TLint4 *) stkAddr)) < ((* (TLint4 *) Language_Execute_globalPC))) || (((* (TLint4 *) stkAddr)) > ((* (TLint4 *) ((unsigned long) Language_Execute_globalPC + Language_codeIntSize))));
			} else {
			    abort = (((* (TLnat4 *) stkAddr)) < ((* (TLnat4 *) Language_Execute_globalPC))) || (((* (TLnat4 *) stkAddr)) > ((* (TLnat4 *) ((unsigned long) Language_Execute_globalPC + Language_codeIntSize))));
			};
			Language_Execute_globalPC += 2 * Language_codeIntSize;
			if (abort) {
			    switch ((* (Language_RangeCheckKind *) Language_Execute_globalPC)) {
				case 0:
				    {
					TLEABT((TLint4) 15);
				    }
				    break;
				case 1:
				    {
					Language_Execute_ExecutionError_Abort((TLint4) 10000, "Size of dynamic char(n) is less than 1");
				    }
				    break;
				case 2:
				    {
					Language_Execute_ExecutionError_Abort((TLint4) 10000, "Value passed to \'chr\' is < 0 or > 255");
				    }
				    break;
				case 3:
				    {
					TLEABT((TLint4) 50);
				    }
				    break;
				case 4:
				    {
					TLEABT((TLint4) 24);
				    }
				    break;
				case 5:
				    {
					Language_Execute_ExecutionError_Abort((TLint4) 10000, "Zero or negative for loop step");
				    }
				    break;
				case 6:
				    {
					TLEABT((TLint4) 38);
				    }
				    break;
				case 7:
				    {
					TLEABT((TLint4) 39);
				    }
				    break;
				case 8:
				    {
					TLEABT((TLint4) 14);
				    }
				    break;
				case 9:
				    {
					TLEABT((TLint4) 22);
				    }
				    break;
				default:
				    TLCASEABORT;
			    };
			};
			TLASSERT (sizeof(Language_RangeCheckKind) == 1);
			Language_Execute_globalPC += Language_codeInt1Size;
		    }
		    break;
		case 63:
		    {
			TLASSERT (((unsigned long) Language_Execute_globalPC & (1)) == 0);
			if (TL_TLS_TLSLEN(((* (TLstring *) ((* (TLaddressint *) Language_Execute_globalSP))))) > ((* (TLint2 *) Language_Execute_globalPC))) {
			    TLEABT((TLint4) 23);
			};
			Language_Execute_globalPC += Language_codeInt2Size;
		    }
		    break;
		case 65:
		    {
			Language_Execute_CloseFile((TLint4) ((* (TLint4 *) Language_Execute_globalSP)));
			Language_Execute_globalSP += 4;
		    }
		    break;
		case 66:
		    {
			TLBINDREG((*oldDesc), struct Language_ArrayDescriptor);
			TLBINDREG((*newDesc), struct Language_ArrayDescriptor);
			oldDesc = (void *) ((* (TLaddressint *) Language_Execute_globalSP));
			Language_Execute_globalSP += 4;
			newDesc = (void *) ((* (TLaddressint *) Language_Execute_globalSP));
			Language_Execute_globalSP += 4;
			(*newDesc).sizeArray = (*oldDesc).sizeArray;
			(*newDesc).sizeElement = (*oldDesc).sizeElement;
			(*newDesc).elementCount = (*oldDesc).elementCount;
			(*newDesc).numDimensions = (*oldDesc).numDimensions;
			{
			    register TLint4	i;
			    TLint4	__x2907;
			    __x2907 = (*oldDesc).numDimensions;
			    i = 1;
			    if (i <= __x2907) {
				for(;;) {
				    TLSTRCTASS((*newDesc).dim[TLINRANGELOW(i, 1, 255, 1)], (*oldDesc).dim[TLINRANGELOW(i, 1, 255, 1)], struct Language___x2321);
				    if (i == __x2907) break;
				    i++;
				}
			    };
			};
		    }
		    break;
		case 70:
		    {
			TLaddressint	memAddr;
			memAddr = (TLaddressint) ((unsigned long) ((* (TLaddressint *) Language_Execute_globalSP)) - 4);
			(* (TLaddressint *) ((* (TLaddressint *) memAddr))) = (TLaddressint) 0;
			Language_Execute_Heap_Dealloc(&(memAddr));
			Language_Execute_globalSP += 4;
		    }
		    break;
		case 71:
		    {
			Language_Offset	amount;
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			amount = (* (Language_Offset *) Language_Execute_globalPC);
			if (amount != 0) {

			    extern void memset ();
			    Language_Execute_globalSP -= amount;
			    memset((TLaddressint) Language_Execute_globalSP, (TLint4) 0, (TLint4) amount);
			};
			Language_Execute_globalPC += Language_codeOffsetSize;
		    }
		    break;
		case 72:
		    {
			register TLint4	rVal;
			rVal = (* (TLint4 *) Language_Execute_globalSP);
			if (rVal == 0) {
			    TLEABT((TLint4) 33);
			};
			Language_Execute_globalSP += 4;
			(* (TLint4 *) Language_Execute_globalSP) /= rVal;
		    }
		    break;
		case 74:
		    {
			register TLreal8	rVal;
			register TLint4	intVal;
			register TLreal8	realVal;
			rVal = (* (TLreal8 *) Language_Execute_globalSP);
			Language_Execute_globalSP += 8;
			if (rVal == 0) {
			    TLEABT((TLint4) 113);
			};
			Language_Execute_ExecutionError_SetOverflowKind((Language_Execute_ExecutionError_OverflowKind) 2);
			intVal = TL_TLA_TLA8DV((TLreal8) rVal, (TLreal8) ((* (TLreal8 *) Language_Execute_globalSP)));
			realVal = ((TLreal8) ((* (TLreal8 *) Language_Execute_globalSP)))  / ((TLreal8) rVal);
			if (realVal < 0) {
			    if ((realVal - intVal) <= (- 1.)) {
				Language_Execute_ExecutionError_Abort((TLint4) 10000, "Overflow in real \'div\' operation");
			    };
			} else {
			    if ((realVal - intVal) >= 1.) {
				Language_Execute_ExecutionError_Abort((TLint4) 10000, "Overflow in real \'div\' operation");
			    };
			};
			Language_Execute_globalSP += 4;
			(* (TLint4 *) Language_Execute_globalSP) = intVal;
			Language_Execute_ExecutionError_SetOverflowKind((Language_Execute_ExecutionError_OverflowKind) 0);
		    }
		    break;
		case 76:
		    {
			TLBIND((*desc), struct Language_ForDescriptor);
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			desc = (void *) ((* (TLaddressint *) Language_Execute_globalSP));
			Language_Execute_globalSP = (TLaddressint) ((*desc).savedSP);
			if (((*desc).step) > 0) {
			    if (((2147483647 - ((*desc).step)) >= ((*desc).counter)) && ((((*desc).counter) + ((*desc).step)) <= ((*desc).right))) {
				(*desc).counter += (*desc).step;
				Language_Execute_globalPC -= (* (Language_Offset *) Language_Execute_globalPC);
			    } else {
				Language_Execute_globalPC += Language_codeOffsetSize;
			    };
			} else {
			    if ((((TLint4) -0x80000000 - ((*desc).step)) <= ((*desc).counter)) && ((((*desc).counter) + ((*desc).step)) >= ((*desc).right))) {
				(*desc).counter += (*desc).step;
				Language_Execute_globalPC -= (* (Language_Offset *) Language_Execute_globalPC);
			    } else {
				Language_Execute_globalPC += Language_codeOffsetSize;
			    };
			};
		    }
		    break;
		case 78:
		    {
			TLaddressint	a1;
			TLaddressint	a2;
			a1 = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			a2 = (TLaddressint) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4)));
			Language_Execute_globalSP += 4;
			if ((unsigned long) a1 == (unsigned long) a2) {
			    (* (TLint4 *) Language_Execute_globalSP) = 1;
			} else {
			    (* (TLint4 *) Language_Execute_globalSP) = 0;
			};
		    }
		    break;
		case 80:
		    {
			Language_Execute_globalSP += 4;
			if (((* (TLint4 *) ((unsigned long) Language_Execute_globalSP - 4))) == ((* (TLint4 *) Language_Execute_globalSP))) {
			    (* (TLint4 *) Language_Execute_globalSP) = 1;
			} else {
			    (* (TLint4 *) Language_Execute_globalSP) = 0;
			};
		    }
		    break;
		case 83:
		    {
			if (((* (TLreal8 *) ((unsigned long) Language_Execute_globalSP + 8))) == ((* (TLreal8 *) Language_Execute_globalSP))) {
			    Language_Execute_globalSP += 12;
			    (* (TLint4 *) Language_Execute_globalSP) = 1;
			} else {
			    Language_Execute_globalSP += 12;
			    (* (TLint4 *) Language_Execute_globalSP) = 0;
			};
		    }
		    break;
		case 85:
		    {
			if (strcmp((* (TLstring *) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4)))), (* (TLstring *) ((* (TLaddressint *) Language_Execute_globalSP)))) == 0) {
			    Language_Execute_globalSP += 4;
			    (* (TLint4 *) Language_Execute_globalSP) = 1;
			} else {
			    Language_Execute_globalSP += 4;
			    (* (TLint4 *) Language_Execute_globalSP) = 0;
			};
		    }
		    break;
		case 86:
		    {
			{
			    TLTMPPOINTER((*__x2940), &((* (TLint4 *) ((unsigned long) Language_Execute_globalSP + 4))), TLint4);
			    (*__x2940) = TL_TLA_TLAPII((TLint4) ((* (TLint4 *) Language_Execute_globalSP)), (TLint4) (*__x2940));
			};
			Language_Execute_globalSP += 4;
		    }
		    break;
		case 87:
		    {
			{
			    TLTMPPOINTER((*__x2942), &((* (TLreal8 *) ((unsigned long) Language_Execute_globalSP + 4))), TLreal8);
			    (*__x2942) = TL_TLA_TLAPRI((TLint4) ((* (TLint4 *) Language_Execute_globalSP)), (TLreal8) (*__x2942));
			};
			Language_Execute_globalSP += 4;
		    }
		    break;
		case 88:
		    {
			{
			    TLTMPPOINTER((*__x2944), &((* (TLreal8 *) ((unsigned long) Language_Execute_globalSP + 8))), TLreal8);
			    (*__x2944) = TL_TLA_TLAPRR((TLreal8) ((* (TLreal8 *) Language_Execute_globalSP)), (TLreal8) (*__x2944));
			};
			Language_Execute_globalSP += 8;
		    }
		    break;
		case 89:
		    {
			(* (TLaddressint *) Language_Execute_globalSP) = (TLaddressint) ((* (TLaddressint *) ((* (TLaddressint *) Language_Execute_globalSP))));
			if ((unsigned long) ((* (TLaddressint *) Language_Execute_globalSP)) == 0xFFFFFFFF) {
			    TLEABT((TLint4) 45);
			};
		    }
		    break;
		case 90:
		    {
			TLint4	value;
			value = (* (TLnat1 *) ((* (TLaddressint *) Language_Execute_globalSP)));
			if (value == 255) {
			    TLEABT((TLint4) 45);
			};
			Language_Execute_globalSP += 0;
			if (value != 0) {
			    (* (TLint4 *) Language_Execute_globalSP) = 1;
			} else {
			    (* (TLint4 *) Language_Execute_globalSP) = 0;
			};
		    }
		    break;
		case 91:
		    {
			register TLaddressint	value;
			value = (TLaddressint) ((* (TLint4 *) ((* (TLaddressint *) Language_Execute_globalSP))));
			if ((unsigned long) value == (TLint4) -0x80000000) {
			    TLEABT((TLint4) 45);
			};
			Language_Execute_globalSP -= 0;
			(* (TLint4 *) Language_Execute_globalSP) = value;
		    }
		    break;
		case 92:
		    {
			register TLint4	value;
			value = (* (TLint1 *) ((* (TLaddressint *) Language_Execute_globalSP)));
			Language_Execute_globalSP -= 0;
			(* (TLint4 *) Language_Execute_globalSP) = value;
		    }
		    break;
		case 93:
		    {
			register TLint4	value;
			value = (* (TLint2 *) ((* (TLaddressint *) Language_Execute_globalSP)));
			Language_Execute_globalSP -= 0;
			(* (TLint4 *) Language_Execute_globalSP) = value;
		    }
		    break;
		case 94:
		    {
			register TLint4	value;
			value = (* (TLint4 *) ((* (TLaddressint *) Language_Execute_globalSP)));
			Language_Execute_globalSP -= 0;
			(* (TLint4 *) Language_Execute_globalSP) = value;
		    }
		    break;
		case 95:
		    {
			register TLnat4	value;
			value = (* (TLnat4 *) ((* (TLaddressint *) Language_Execute_globalSP)));
			if (value == 0xFFFFFFFF) {
			    TLEABT((TLint4) 45);
			};
			Language_Execute_globalSP -= 0;
			(* (TLnat4 *) Language_Execute_globalSP) = value;
		    }
		    break;
		case 96:
		    {
			register TLnat4	value;
			value = (* (TLnat1 *) ((* (TLaddressint *) Language_Execute_globalSP)));
			Language_Execute_globalSP -= 0;
			(* (TLnat4 *) Language_Execute_globalSP) = value;
		    }
		    break;
		case 97:
		    {
			register TLnat4	value;
			value = (* (TLnat2 *) ((* (TLaddressint *) Language_Execute_globalSP)));
			Language_Execute_globalSP -= 0;
			(* (TLnat4 *) Language_Execute_globalSP) = value;
		    }
		    break;
		case 98:
		    {
			register TLnat4	value;
			value = (* (TLnat4 *) ((* (TLaddressint *) Language_Execute_globalSP)));
			Language_Execute_globalSP -= 0;
			(* (TLnat4 *) Language_Execute_globalSP) = value;
		    }
		    break;
		case 99:
		    {
			TLBINDREG((*p), struct Language_PointerDescriptor);
			p = (void *) ((* (TLaddressint *) Language_Execute_globalSP));
			if ((unsigned long) ((*p).dataPtr) == 0xFFFFFFFF) {
			    TLEABT((TLint4) 45);
			} else {
			    if (((unsigned long) ((*p).dataPtr) != 0) && (((*p).timestamp) != ((* (TLint4 *) ((unsigned long) ((*p).dataPtr) - 4))))) {
				Language_Execute_ExecutionError_Abort((TLint4) 10000, "Reference to previously freed pointer");
			    };
			};
			(* (TLaddressint *) Language_Execute_globalSP) = (TLaddressint) ((*p).dataPtr);
		    }
		    break;
		case 100:
		    {
			register TLaddressint	valAddr;
			valAddr = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			if ((((* (TLint4 *) valAddr)) == (TLint4) -0x80000000) && (((* (TLint4 *) ((unsigned long) valAddr + 4))) == (TLint4) -0x80000000)) {
			    TLEABT((TLint4) 45);
			};
			Language_Execute_globalSP -= 4;
			(* (TLreal8 *) Language_Execute_globalSP) = (* (TLreal8 *) valAddr);
		    }
		    break;
		case 101:
		    {
			register TLreal8	value;
			value = (* (TLreal4 *) ((* (TLaddressint *) Language_Execute_globalSP)));
			Language_Execute_globalSP -= 4;
			(* (TLreal8 *) Language_Execute_globalSP) = value;
		    }
		    break;
		case 102:
		    {
			register TLreal8	value;
			value = (* (TLreal8 *) ((* (TLaddressint *) Language_Execute_globalSP)));
			Language_Execute_globalSP -= 4;
			(* (TLreal8 *) Language_Execute_globalSP) = value;
		    }
		    break;
		case 104:
		    {
			if (((* (TLnat1 *) ((* (TLaddressint *) Language_Execute_globalSP)))) == 128) {
			    TLEABT((TLint4) 45);
			};
		    }
		    break;
		case 105:
		    {
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			(* (TLaddressint *) Language_Execute_globalSP) += (* (Language_Offset *) Language_Execute_globalPC);
			Language_Execute_globalPC += Language_codeOffsetSize;
		    }
		    break;
		case 106:
		    {
			register TLint4	r;
			register TLint4	l;
			TLint4	step;
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			r = (* (TLint4 *) (((unsigned long) Language_Execute_globalSP + 4) + 4));
			l = (* (TLint4 *) (((unsigned long) Language_Execute_globalSP + 4) + (2 * 4)));
			step = (* (TLint4 *) ((unsigned long) Language_Execute_globalSP + 4));
			if (((step > 0) && (l <= r)) || ((step < 0) && (l >= r))) {
			    TLBIND((*desc), struct Language_ForDescriptor);
			    desc = (void *) ((* (TLaddressint *) Language_Execute_globalSP));
			    (*desc).counter = l;
			    (*desc).right = r;
			    (*desc).step = step;
			    Language_Execute_globalSP += 16;
			    (*desc).savedSP = (TLaddressint) Language_Execute_globalSP;
			    Language_Execute_globalPC += Language_codeOffsetSize;
			} else {
			    Language_Execute_globalSP += 16;
			    Language_Execute_globalPC += (* (Language_Offset *) Language_Execute_globalPC);
			};
		    }
		    break;
		case 108:
		    {
			TLBIND((*p), struct Language_PointerDescriptor);
			p = (void *) ((* (TLaddressint *) Language_Execute_globalSP));
			if ((unsigned long) ((*p).dataPtr) == 0xFFFFFFFF) {
			    TLEABT((TLint4) 45);
			} else {
			    if ((unsigned long) ((*p).dataPtr) == 0) {
				Language_Execute_ExecutionError_Abort((TLint4) 10000, "Nil pointer passed to \'free\'");
			    } else {
				if (((*p).timestamp) != ((* (TLint4 *) ((unsigned long) ((*p).dataPtr) - 4)))) {
				    TLEABT((TLint4) 27);
				};
			    };
			};
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			if (((* (Language_Offset *) Language_Execute_globalPC)) != 0) {
			    {
				TLint4	__x3008;
				__x3008 = (unsigned long) ((*p).dataPtr) + ((* (Language_Offset *) Language_Execute_globalPC));
				Language_Execute_Kernel_MonitorFree((* (struct Language_Execute_MonitorDescriptor * *) &__x3008));
			    };
			};
			Language_Execute_Heap_Free(&((*p)));
			Language_Execute_globalSP += 4;
			Language_Execute_globalPC += Language_codeOffsetSize;
		    }
		    break;
		case 109:
		    {
			struct Language_PointerDescriptor	p;
			p.dataPtr = (TLaddressint) ((* (TLaddressint *) ((* (TLaddressint *) Language_Execute_globalSP))));
			if ((unsigned long) (p.dataPtr) == 0) {
			    Language_Execute_ExecutionError_Abort((TLint4) 10000, "Nil pointer passed to \'free\'");
			};
			p.timestamp = (* (TLint4 *) ((unsigned long) (p.dataPtr) - 4));
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			if (((* (Language_Offset *) Language_Execute_globalPC)) != 0) {
			    {
				TLint4	__x3015;
				__x3015 = (unsigned long) (p.dataPtr) + ((* (Language_Offset *) Language_Execute_globalPC));
				Language_Execute_Kernel_MonitorFree((* (struct Language_Execute_MonitorDescriptor * *) &__x3015));
			    };
			};
			Language_Execute_Heap_Free(&(p));
			Language_Execute_globalSP += 4;
			Language_Execute_globalPC += Language_codeOffsetSize;
		    }
		    break;
		case 110:
		    {
			if ((unsigned long) ((* (TLaddressint *) ((* (TLaddressint *) Language_Execute_globalSP)))) == 0) {
			    Language_Execute_ExecutionError_Abort((TLint4) 10000, "Nil pointer passed to \'free\'");
			};
			Language_Execute_Heap_Dealloc(&((* (TLaddressint *) ((* (TLaddressint *) Language_Execute_globalSP)))));
			Language_Execute_globalSP += 4;
		    }
		    break;
		case 112:
		    {
			register TLaddressint	desc;
			TLaddressint	refDesc;
			desc = (TLaddressint) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4)));
			refDesc = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			Language_Execute_globalSP += 4;
			if ((unsigned long) desc != 0) {
			    for(;;) {
				if ((unsigned long) desc == (unsigned long) refDesc) {
				    break;
				};
				desc = (TLaddressint) ((* (struct Language_ClassDescriptor *) desc).expandClass);
				if ((unsigned long) desc == 0) {
				    break;
				};
			    };
			    if ((unsigned long) desc == 0) {
				(* (TLint4 *) Language_Execute_globalSP) = 0;
			    } else {
				(* (TLint4 *) Language_Execute_globalSP) = 1;
			    };
			} else {
			    (* (TLint4 *) Language_Execute_globalSP) = 1;
			};
		    }
		    break;
		case 113:
		    {
			Language_Execute_globalSP += 4;
			if (((* (TLint4 *) Language_Execute_globalSP)) >= ((* (TLint4 *) ((unsigned long) Language_Execute_globalSP - 4)))) {
			    (* (TLint4 *) Language_Execute_globalSP) = 1;
			} else {
			    (* (TLint4 *) Language_Execute_globalSP) = 0;
			};
		    }
		    break;
		case 117:
		    {
			if (((* (TLreal8 *) ((unsigned long) Language_Execute_globalSP + 8))) >= ((* (TLreal8 *) Language_Execute_globalSP))) {
			    Language_Execute_globalSP += 12;
			    (* (TLint4 *) Language_Execute_globalSP) = 1;
			} else {
			    Language_Execute_globalSP += 12;
			    (* (TLint4 *) Language_Execute_globalSP) = 0;
			};
		    }
		    break;
		case 118:
		    {
			Language_Offset	sz;
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			sz = (* (Language_Offset *) Language_Execute_globalPC);
			Language_Execute_globalPC += Language_codeOffsetSize;
			if (sz <= 4) {
			    Language_Execute_globalSP += 4;
			    if (0 == (((* (Language_set32 *) ((unsigned long) Language_Execute_globalSP - 4)))  & ~ ((* (Language_set32 *) Language_Execute_globalSP)))) {
				(* (TLint4 *) Language_Execute_globalSP) = 1;
			    } else {
				(* (TLint4 *) Language_Execute_globalSP) = 0;
			    };
			} else {
			    register TLaddressint	l;
			    register TLaddressint	r;
			    l = (TLaddressint) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4)));
			    r = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			    Language_Execute_globalSP += 4;
			    (* (TLint4 *) Language_Execute_globalSP) = 1;
			    {
				register TLint4	__x2102;
				TLint4	__x3043;
				__x3043 = sz >> 1;
				__x2102 = 1;
				if (__x2102 <= __x3043) {
				    for(;;) {
					if (!(0 == (((* (Language_set16 *) r))  & ~ ((* (Language_set16 *) l))))) {
					    (* (TLint4 *) Language_Execute_globalSP) = 0;
					    break;
					};
					l += 2;
					r += 2;
					if (__x2102 == __x3043) break;
					__x2102++;
				    }
				};
			    };
			};
		    }
		    break;
		case 119:
		    {
			if (strcmp((* (TLstring *) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4)))), (* (TLstring *) ((* (TLaddressint *) Language_Execute_globalSP)))) >= 0) {
			    Language_Execute_globalSP += 4;
			    (* (TLint4 *) Language_Execute_globalSP) = 1;
			} else {
			    Language_Execute_globalSP += 4;
			    (* (TLint4 *) Language_Execute_globalSP) = 0;
			};
		    }
		    break;
		case 122:
		    {
			register TLaddressint	desc;
			TLaddressint	refDesc;
			desc = (TLaddressint) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4)));
			refDesc = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			Language_Execute_globalSP += 4;
			if ((unsigned long) desc != 0) {
			    for(;;) {
				desc = (TLaddressint) ((* (struct Language_ClassDescriptor *) desc).expandClass);
				if (((unsigned long) desc == 0) || ((unsigned long) desc == (unsigned long) refDesc)) {
				    break;
				};
			    };
			    if ((unsigned long) desc == 0) {
				(* (TLint4 *) Language_Execute_globalSP) = 0;
			    } else {
				(* (TLint4 *) Language_Execute_globalSP) = 1;
			    };
			} else {
			    if ((unsigned long) refDesc == 0) {
				(* (TLint4 *) Language_Execute_globalSP) = 0;
			    } else {
				(* (TLint4 *) Language_Execute_globalSP) = 1;
			    };
			};
		    }
		    break;
		case 123:
		    {
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			if (((* (TLint4 *) Language_Execute_globalSP)) == 0) {
			    Language_Execute_globalPC += (* (Language_Offset *) Language_Execute_globalPC);
			} else {
			    Language_Execute_globalPC += Language_codeOffsetSize;
			};
			Language_Execute_globalSP += 4;
		    }
		    break;
		case 124:
		    {
			Language_Offset	sz;
			register TLint4	lo;
			register TLint4	hi;
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			sz = (* (Language_Offset *) Language_Execute_globalPC);
			Language_Execute_globalPC += Language_codeOffsetSize;
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			lo = (* (TLint4 *) Language_Execute_globalPC);
			hi = (* (TLint4 *) ((unsigned long) Language_Execute_globalPC + Language_codeIntSize));
			Language_Execute_globalPC += 2 * Language_codeIntSize;
			if (sz <= 4) {
			    register TLnat4	s;
			    register TLint4	i;
			    s = (* (Language_set32 *) Language_Execute_globalSP);
			    i = (* (TLint4 *) ((unsigned long) Language_Execute_globalSP + 4));
			    Language_Execute_globalSP += 4;
			    if ((i < lo) || (i > hi)) {
				TLEABT((TLint4) 29);
			    } else {
				if ((TLSMLSETCONST(TLINRANGELOW(i - lo, 0, 31, 19)) & s) != 0) {
				    (* (TLint4 *) Language_Execute_globalSP) = 1;
				} else {
				    (* (TLint4 *) Language_Execute_globalSP) = 0;
				};
			    };
			} else {
			    register TLint4	i;
			    i = (* (TLint4 *) ((unsigned long) Language_Execute_globalSP + 4));
			    Language_Execute_globalSP += 4;
			    if ((i < lo) || (i > hi)) {
				TLEABT((TLint4) 29);
			    } else {
				TLint4	s;
				i -= lo;
				s = i & 15;
				if ((TLSMLSMLSETCONST(TLINRANGELOW(s, 0, 15, 19)) & ((* (Language_set16 *) ((unsigned long) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP - 4))) + (2 * (i >> 4)))))) != 0) {
				    (* (TLint4 *) Language_Execute_globalSP) = 1;
				} else {
				    (* (TLint4 *) Language_Execute_globalSP) = 0;
				};
			    };
			};
		    }
		    break;
		case 125:
		    {
			Language_Execute_RQHead->srcPos.lineNo += 1;
			if (Language_debug) {
			    {
				TLstring	__x3074;
				FileManager_FileName((FileManager_FileNoType) (Language_Execute_RQHead->srcPos.fileNo), __x3074);
				TL_TLI_TLISS ((TLint4) 0, (TLint2) 2);
				TL_TLI_TLIPS ((TLint4) 0, "line ", (TLint2) 0);
				TL_TLI_TLIPN ((TLint4) 0, (TLnat4) (Language_Execute_RQHead->srcPos.lineNo), (TLint2) 0);
				TL_TLI_TLIPS ((TLint4) 0, " of \"", (TLint2) 0);
				TL_TLI_TLIPS ((TLint4) 0, __x3074, (TLint2) 0);
				TL_TLI_TLIPS ((TLint4) 0, "\"", (TLint2) 0);
				TL_TLI_TLIPK ((TLint2) 0);
			    };
			};
			if (((Language_Execute_RQHead->stepSpec) != 0) && ((unsigned long) (Language_Execute_RQHead->stepFP) == 0)) {
			    (*runDesc).state = 4;
			    goto __x3763;
			};
		    }
		    break;
		case 126:
		    {
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			Language_Execute_globalSP += (* (Language_Offset *) Language_Execute_globalPC);
			Language_Execute_globalPC += Language_codeOffsetSize;
		    }
		    break;
		case 127:
		    {
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			if (((* (TLint4 *) Language_Execute_globalSP)) == 0) {
			    Language_Execute_globalPC += (* (Language_Offset *) Language_Execute_globalPC);
			} else {
			    Language_Execute_globalSP += 4;
			    Language_Execute_globalPC += Language_codeOffsetSize;
			};
		    }
		    break;
		case 128:
		    {
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			if (((* (TLint4 *) Language_Execute_globalSP)) != 0) {
			    Language_Execute_globalPC += (* (Language_Offset *) Language_Execute_globalPC);
			} else {
			    Language_Execute_globalSP += 4;
			    Language_Execute_globalPC += Language_codeOffsetSize;
			};
		    }
		    break;
		case 129:
		    {
			TLBINDREG((*desc), struct Language_ArrayDescriptor);
			TLaddressint	lowerAddr;
			desc = (void *) ((* (TLaddressint *) Language_Execute_globalSP));
			Language_Execute_globalSP += 4;
			(*desc).sizeElement = (* (TLint4 *) Language_Execute_globalSP);
			Language_Execute_globalSP += 4;
			(*desc).numDimensions = (* (TLint4 *) Language_Execute_globalSP);
			Language_Execute_globalSP += 4;
			lowerAddr = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			Language_Execute_globalSP += 4;
			(*desc).elementCount = 1;
			{
			    register TLint4	i;
			    for (i = (*desc).numDimensions; i >= 1; i--) {
				TLint4	lowerBound;
				TLint4	upperBound;
				TLint4	range;
				TLnat4	oldElementCount;
				lowerAddr -= 4;
				lowerBound = (* (TLint4 *) lowerAddr);
				upperBound = (* (TLint4 *) Language_Execute_globalSP);
				Language_Execute_globalSP += 4;
				range = (upperBound - lowerBound) + 1;
				if (upperBound >= lowerBound) {
				    if (range <= 0) {
					Language_Execute_ExecutionError_Abort((TLint4) 10000, "Index range for dynamic array is too large");
				    };
				} else {
				    if (upperBound < (lowerBound - 1)) {
					Language_Execute_ExecutionError_Abort((TLint4) 10000, "Index range for dynamic array is negative");
				    };
				};
				(*desc).dim[TLINRANGELOW(i, 1, 255, 1)].lower = lowerBound;
				(*desc).dim[TLINRANGELOW(i, 1, 255, 1)].range = range;
				oldElementCount = (*desc).elementCount;
				(*desc).elementCount *= range;
				if ((range != 0) && ((((*desc).elementCount) / range) != oldElementCount)) {
				    Language_Execute_ExecutionError_Abort((TLint4) 10000, "Size of dynamic array is too large");
				};
			    };
			};
			(*desc).sizeArray = (((*desc).elementCount) * ((*desc).sizeElement)) + 3;
			(*desc).sizeArray &= 0xFFFFFFFC;
		    }
		    break;
		case 132:
		    {
			register TLaddressint	initAddr;
			Language_Execute_globalPC += Language_codeOffsetSize;
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			initAddr = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalPC));
			Language_Execute_globalPC += Language_codeAddrSize;
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			if (((* (TLint1 *) initAddr)) != 0) {
			    Language_Execute_globalPC += Language_codeOffsetSize + Language_codeInt1Size;
			} else {
			    (* (TLint1 *) initAddr) = (* (TLint1 *) ((unsigned long) Language_Execute_globalPC + Language_codeOffsetSize));
			    Language_Execute_globalPC += (* (Language_Offset *) Language_Execute_globalPC);
			};
			Language_Execute_savedInitPC = (TLaddressint) 0;
			Language_Execute_savedInitFP = (TLaddressint) 0;
		    }
		    break;
		case 133:
		    {
			Language_Execute_globalSP += -4;
			(* (TLreal8 *) Language_Execute_globalSP) = (* (TLint4 *) ((unsigned long) Language_Execute_globalSP + (4)));
		    }
		    break;
		case 134:
		    {
			TLreal8	temp;
			temp = (* (TLreal8 *) Language_Execute_globalSP);
			Language_Execute_globalSP += -4;
			(* (TLreal8 *) Language_Execute_globalSP) = temp;
			(* (TLreal8 *) ((unsigned long) Language_Execute_globalSP + 8)) = (* (TLint4 *) ((unsigned long) Language_Execute_globalSP + ((2 * 8) - 4)));
		    }
		    break;
		case 135:
		    {
			register TLaddressint	tAddr;
			tAddr = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			{
			    TLstring	__x3103;
			    TL_TLS_TLSVIS((TLint4) ((* (TLint4 *) (((unsigned long) Language_Execute_globalSP + 4) + (2 * 4)))), (TLint4) ((* (TLint4 *) (((unsigned long) Language_Execute_globalSP + 4) + 4))), (TLint4) ((* (TLint4 *) ((unsigned long) Language_Execute_globalSP + 4))), __x3103);
			    TLSTRASS(255, (* (TLstring *) tAddr), __x3103);
			};
			Language_Execute_globalSP += 12;
			(* (TLaddressint *) Language_Execute_globalSP) = (TLaddressint) tAddr;
		    }
		    break;
		case 136:
		    {
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			Language_Execute_globalSP -= 4;
			(* (TLaddressint *) Language_Execute_globalSP) = (TLaddressint) ((unsigned long) Language_Execute_globalPC + Language_codeOffsetSize);
			Language_Execute_globalPC -= (* (Language_Offset *) Language_Execute_globalPC);
		    }
		    break;
		case 137:
		    {
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			Language_Execute_globalPC += (* (Language_Offset *) Language_Execute_globalPC);
		    }
		    break;
		case 138:
		    {
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			Language_Execute_globalPC -= (* (Language_Offset *) Language_Execute_globalPC);
		    }
		    break;
		case 140:
		    {
			register TLaddressint	desc;
			TLaddressint	refDesc;
			desc = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			refDesc = (TLaddressint) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4)));
			Language_Execute_globalSP += 4;
			if ((unsigned long) desc != 0) {
			    for(;;) {
				if ((unsigned long) desc == (unsigned long) refDesc) {
				    break;
				};
				desc = (TLaddressint) ((* (struct Language_ClassDescriptor *) desc).expandClass);
				if ((unsigned long) desc == 0) {
				    break;
				};
			    };
			    if ((unsigned long) desc == 0) {
				(* (TLint4 *) Language_Execute_globalSP) = 0;
			    } else {
				(* (TLint4 *) Language_Execute_globalSP) = 1;
			    };
			} else {
			    (* (TLint4 *) Language_Execute_globalSP) = 1;
			};
		    }
		    break;
		case 141:
		    {
			Language_Execute_globalSP += 4;
			if (((* (TLint4 *) Language_Execute_globalSP)) <= ((* (TLint4 *) ((unsigned long) Language_Execute_globalSP - 4)))) {
			    (* (TLint4 *) Language_Execute_globalSP) = 1;
			} else {
			    (* (TLint4 *) Language_Execute_globalSP) = 0;
			};
		    }
		    break;
		case 145:
		    {
			if (((* (TLreal8 *) ((unsigned long) Language_Execute_globalSP + 8))) <= ((* (TLreal8 *) Language_Execute_globalSP))) {
			    Language_Execute_globalSP += 12;
			    (* (TLint4 *) Language_Execute_globalSP) = 1;
			} else {
			    Language_Execute_globalSP += 12;
			    (* (TLint4 *) Language_Execute_globalSP) = 0;
			};
		    }
		    break;
		case 146:
		    {
			Language_Offset	sz;
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			sz = (* (Language_Offset *) Language_Execute_globalPC);
			Language_Execute_globalPC += Language_codeOffsetSize;
			if (sz <= 4) {
			    Language_Execute_globalSP += 4;
			    if (0 == (((* (Language_set32 *) Language_Execute_globalSP))  & ~ ((* (Language_set32 *) ((unsigned long) Language_Execute_globalSP - 4))))) {
				(* (TLint4 *) Language_Execute_globalSP) = 1;
			    } else {
				(* (TLint4 *) Language_Execute_globalSP) = 0;
			    };
			} else {
			    register TLaddressint	l;
			    register TLaddressint	r;
			    l = (TLaddressint) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4)));
			    r = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			    Language_Execute_globalSP += 4;
			    (* (TLint4 *) Language_Execute_globalSP) = 1;
			    {
				register TLint4	__x2127;
				TLint4	__x3131;
				__x3131 = sz >> 1;
				__x2127 = 1;
				if (__x2127 <= __x3131) {
				    for(;;) {
					if (!(0 == (((* (Language_set16 *) l))  & ~ ((* (Language_set16 *) r))))) {
					    (* (TLint4 *) Language_Execute_globalSP) = 0;
					    break;
					};
					l += 2;
					r += 2;
					if (__x2127 == __x3131) break;
					__x2127++;
				    }
				};
			    };
			};
		    }
		    break;
		case 147:
		    {
			if (strcmp((* (TLstring *) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4)))), (* (TLstring *) ((* (TLaddressint *) Language_Execute_globalSP)))) <= 0) {
			    Language_Execute_globalSP += 4;
			    (* (TLint4 *) Language_Execute_globalSP) = 1;
			} else {
			    Language_Execute_globalSP += 4;
			    (* (TLint4 *) Language_Execute_globalSP) = 0;
			};
		    }
		    break;
		case 148:
		    {
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			Language_Execute_globalSP -= 4;
			(* (TLaddressint *) Language_Execute_globalSP) = (TLaddressint) (((unsigned long) Language_Execute_globalSP + 4) + ((* (Language_Offset *) Language_Execute_globalPC)));
			Language_Execute_globalPC += Language_codeOffsetSize;
		    }
		    break;
		case 149:
		    {
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			Language_Execute_globalSP -= 4;
			(* (TLaddressint *) Language_Execute_globalSP) = (TLaddressint) ((unsigned long) ((* (TLaddressint *) ((unsigned long) fp + 12))) + ((* (Language_Offset *) Language_Execute_globalPC)));
			Language_Execute_globalPC += Language_codeOffsetSize;
		    }
		    break;
		case 150:
		    {
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			Language_Execute_globalSP -= 4;
			(* (TLaddressint *) Language_Execute_globalSP) = (TLaddressint) ((unsigned long) fp - ((* (Language_Offset *) Language_Execute_globalPC)));
			Language_Execute_globalPC += Language_codeOffsetSize;
		    }
		    break;
		case 151:
		    {
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			Language_Execute_globalSP -= 4;
			(* (TLaddressint *) Language_Execute_globalSP) = (TLaddressint) (((unsigned long) fp + 12) + ((* (Language_Offset *) Language_Execute_globalPC)));
			Language_Execute_globalPC += Language_codeOffsetSize;
		    }
		    break;
		case 152:
		    {
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			Language_Execute_globalSP -= 4;
			(* (TLaddressint *) Language_Execute_globalSP) = (TLaddressint) (((unsigned long) fp - ((* (Language_Offset *) Language_Execute_globalPC))) + ((* (Language_Offset *) ((unsigned long) Language_Execute_globalPC + Language_codeOffsetSize))));
			Language_Execute_globalPC += 2 * Language_codeOffsetSize;
		    }
		    break;
		case 153:
		    {
			register TLaddressint	desc;
			TLaddressint	refDesc;
			desc = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			refDesc = (TLaddressint) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4)));
			Language_Execute_globalSP += 4;
			if ((unsigned long) desc != 0) {
			    for(;;) {
				desc = (TLaddressint) ((* (struct Language_ClassDescriptor *) desc).expandClass);
				if (((unsigned long) desc == 0) || ((unsigned long) desc == (unsigned long) refDesc)) {
				    break;
				};
			    };
			    if ((unsigned long) desc == 0) {
				(* (TLint4 *) Language_Execute_globalSP) = 0;
			    } else {
				(* (TLint4 *) Language_Execute_globalSP) = 1;
			    };
			} else {
			    if ((unsigned long) refDesc == 0) {
				(* (TLint4 *) Language_Execute_globalSP) = 0;
			    } else {
				(* (TLint4 *) Language_Execute_globalSP) = 1;
			    };
			};
		    }
		    break;
		case 154:
		    {
			Language_Execute_globalSP += 4;
			(* (TLint4 *) Language_Execute_globalSP) =  TL_TLA_TLAIMX ((TLint4) ((* (TLint4 *) Language_Execute_globalSP)), (TLint4) ((* (TLint4 *) ((unsigned long) Language_Execute_globalSP - 4))));
		    }
		    break;
		case 156:
		    {
			Language_Execute_globalSP += 8;
			(* (TLreal8 *) Language_Execute_globalSP) =  TL_TLA_TLA8MX((TLreal8) ((* (TLreal8 *) Language_Execute_globalSP)), (TLreal8) ((* (TLreal8 *) ((unsigned long) Language_Execute_globalSP - 8))));
		    }
		    break;
		case 157:
		    {
			Language_Execute_globalSP += 4;
			(* (TLint4 *) Language_Execute_globalSP) =  TL_TLA_TLAIMN((TLint4) ((* (TLint4 *) Language_Execute_globalSP)), (TLint4) ((* (TLint4 *) ((unsigned long) Language_Execute_globalSP - 4))));
		    }
		    break;
		case 159:
		    {
			Language_Execute_globalSP += 8;
			(* (TLreal8 *) Language_Execute_globalSP) =  TL_TLA_TLA8MN((TLreal8) ((* (TLreal8 *) Language_Execute_globalSP)), (TLreal8) ((* (TLreal8 *) ((unsigned long) Language_Execute_globalSP - 8))));
		    }
		    break;
		case 160:
		    {
			register TLint4	rVal;
			register TLint4	intVal;
			rVal = (* (TLint4 *) Language_Execute_globalSP);
			Language_Execute_globalSP += 4;
			if (rVal == 0) {
			    TLEABT((TLint4) 33);
			};
			Language_Execute_ExecutionError_SetOverflowKind((Language_Execute_ExecutionError_OverflowKind) 1);
			intVal = ((* (TLint4 *) Language_Execute_globalSP)) / rVal;
			if (((* (TLint4 *) Language_Execute_globalSP)) != (rVal * intVal)) {
			    if ((rVal < 0) != (((* (TLint4 *) Language_Execute_globalSP)) < 0)) {
				intVal -= 1;
			    };
			    (* (TLint4 *) Language_Execute_globalSP) -= rVal * intVal;
			} else {
			    (* (TLint4 *) Language_Execute_globalSP) = 0;
			};
			Language_Execute_ExecutionError_SetOverflowKind((Language_Execute_ExecutionError_OverflowKind) 0);
		    }
		    break;
		case 161:
		    {
			register TLnat4	rVal;
			rVal = (* (TLnat4 *) Language_Execute_globalSP);
			Language_Execute_globalSP += 4;
			if (rVal == 0) {
			    TLEABT((TLint4) 33);
			};
			(* (TLnat4 *) Language_Execute_globalSP) -= rVal * (((* (TLnat4 *) Language_Execute_globalSP)) / rVal);
		    }
		    break;
		case 162:
		    {
			register TLreal8	rVal;
			register TLreal8	realVal;
			register TLint4	intVal;
			rVal = (* (TLreal8 *) Language_Execute_globalSP);
			Language_Execute_globalSP += 8;
			if (rVal == 0) {
			    TLEABT((TLint4) 113);
			};
			Language_Execute_ExecutionError_SetOverflowKind((Language_Execute_ExecutionError_OverflowKind) 1);
			realVal = ((TLreal8) ((* (TLreal8 *) Language_Execute_globalSP)))  / ((TLreal8) rVal);
			intVal = TL_TLA_TLA8FL((TLreal8) realVal);
			if (( fabs((intVal - realVal))) >= 1.) {
			    Language_Execute_ExecutionError_Abort((TLint4) 10000, "Overflow in real \'mod\' operation");
			};
			(* (TLreal8 *) Language_Execute_globalSP) -= rVal * intVal;
			Language_Execute_ExecutionError_SetOverflowKind((Language_Execute_ExecutionError_OverflowKind) 0);
		    }
		    break;
		case 165:
		    {
			register TLint4	rVal;
			register TLint4	lVal;
			rVal = (* (TLint4 *) Language_Execute_globalSP);
			Language_Execute_globalSP += 4;
			lVal = (* (TLint4 *) Language_Execute_globalSP);
			(* (TLint4 *) Language_Execute_globalSP) *= rVal;
			if ((rVal != 0) && ((((* (TLint4 *) Language_Execute_globalSP)) / rVal) != lVal)) {
			    TLEABT((TLint4) 50);
			};
		    }
		    break;
		case 167:
		    {
			register TLreal8	rVal;
			rVal = (* (TLreal8 *) Language_Execute_globalSP);
			Language_Execute_globalSP += 8;
			if ((( TLSIMPLEABS(rVal)) > 1.) && (( fabs(((* (TLreal8 *) Language_Execute_globalSP)))) > (((TLreal8) .1797693134862315517e307)  / ((TLreal8) ( TLSIMPLEABS(rVal)))))) {
			    TLEABT((TLint4) 51);
			};
			(* (TLreal8 *) Language_Execute_globalSP) *= rVal;
		    }
		    break;
		case 168:
		    {
			Language_Offset	sz;
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			sz = (* (Language_Offset *) Language_Execute_globalPC);
			Language_Execute_globalPC += Language_codeOffsetSize;
			if (sz <= 4) {
			    (* (Language_set32 *) ((unsigned long) Language_Execute_globalSP + 4)) &= (* (Language_set32 *) Language_Execute_globalSP);
			    Language_Execute_globalSP += 4;
			} else {
			    register TLaddressint	t;
			    register TLaddressint	r;
			    register TLaddressint	l;
			    t = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			    r = (TLaddressint) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4)));
			    l = (TLaddressint) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + (2 * 4))));
			    {
				register TLint4	__x2143;
				TLint4	__x3197;
				__x3197 = sz >> 1;
				__x2143 = 1;
				if (__x2143 <= __x3197) {
				    for(;;) {
					(* (Language_set16 *) t) = ((* (Language_set16 *) l)) & ((* (Language_set16 *) r));
					t += 2;
					l += 2;
					r += 2;
					if (__x2143 == __x3197) break;
					__x2143++;
				    }
				};
			    };
			    (* (TLaddressint *) (((unsigned long) Language_Execute_globalSP + 4) + 4)) = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			    Language_Execute_globalSP += 8;
			};
		    }
		    break;
		case 172:
		    {
			if (((* (TLint4 *) Language_Execute_globalSP)) == (TLint4) -0x80000000) {
			    TLEABT((TLint4) 50);
			};
			(* (TLint4 *) Language_Execute_globalSP) = - ((* (TLint4 *) Language_Execute_globalSP));
		    }
		    break;
		case 173:
		    {
			(* (TLreal8 *) Language_Execute_globalSP) = - ((* (TLreal8 *) Language_Execute_globalSP));
		    }
		    break;
		case 174:
		    {
			TLBIND((*ptr), struct Language_PointerDescriptor);
			ptr = (void *) ((* (TLaddressint *) (((unsigned long) Language_Execute_globalSP + 4) + 4)));
			Language_Execute_Heap_New(&((*ptr)), (TLnat4) ((* (TLnat4 *) ((unsigned long) Language_Execute_globalSP + 4))));
			if (((unsigned long) ((*ptr).dataPtr) != 0) && ((unsigned long) ((* (TLaddressint *) Language_Execute_globalSP)) != 0)) {
			    TL_TLB_TLBUNI((TLaddressint) ((*ptr).dataPtr), (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP)), (TLint4) 0);
			};
			Language_Execute_globalSP += 12;
		    }
		    break;
		case 175:
		    {
			TLBINDREG((*desc), struct Language_ArrayDescriptor);
			TLaddressint	uninitAddr;
			TLaddressint	upperAddr;
			TLnat4	newElementCount;
			TLboolean	mustCopy;
			TLaddressint	memAddr;
			TLnat4	newSizeArray;
			desc = (void *) ((* (TLaddressint *) Language_Execute_globalSP));
			Language_Execute_globalSP += 4;
			uninitAddr = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			Language_Execute_globalSP += 4;
			upperAddr = (TLaddressint) Language_Execute_globalSP;
			newElementCount = 1;
			mustCopy = 0;
			{
			    register TLint4	i;
			    for (i = (*desc).numDimensions; i >= 1; i--) {
				TLint4	range;
				range = (((* (TLint4 *) upperAddr)) - ((*desc).dim[TLINRANGELOW(i, 1, 255, 1)].lower)) + 1;
				if (range < 0) {
				    Language_Execute_ExecutionError_Abort((TLint4) 10000, "New index range for flexible array is negative");
				};
				newElementCount *= range;
				if ((i != 1) && (range != ((*desc).dim[TLINRANGELOW(i, 1, 255, 1)].range))) {
				    mustCopy = 1;
				};
				upperAddr += 4;
			    };
			};
			memAddr = (TLaddressint) ((unsigned long) ((* (TLaddressint *) upperAddr)) - 4);
			newSizeArray = (newElementCount * ((*desc).sizeElement)) + 3;
			newSizeArray &= 0xFFFFFFFC;
			if (((((*desc).elementCount) != 0) && (newElementCount != 0)) && mustCopy) {
			    Language_Execute_ExecutionError_Abort((TLint4) 10000, "Complex multi-dimensioned flexible array reallocation not implemented yet - sorry");
			} else {
			    Language_Execute_Heap_Realloc(&(memAddr), (TLnat4) (newSizeArray + 4));
			    if ((unsigned long) memAddr == 0) {
				Language_Execute_ExecutionError_Abort((TLint4) 10000, "Not enough memory for flexible array reallocation");
			    };
			    (* (TLaddressint *) ((* (TLaddressint *) memAddr))) = (TLaddressint) ((unsigned long) memAddr + 4);
			    if (((unsigned long) uninitAddr != 0) && (newElementCount > ((*desc).elementCount))) {
				TL_TLB_TLBUNI((TLaddressint) (((unsigned long) memAddr + 4) + (((*desc).elementCount) * ((*desc).sizeElement))), (TLaddressint) uninitAddr, (TLint4) (newElementCount - ((*desc).elementCount)));
			    };
			};
			(*desc).sizeArray = newSizeArray;
			(*desc).elementCount = newElementCount;
			{
			    register TLint4	i;
			    for (i = (*desc).numDimensions; i >= 1; i--) {
				(*desc).dim[TLINRANGELOW(i, 1, 255, 1)].range = (((* (TLint4 *) Language_Execute_globalSP)) - ((*desc).dim[TLINRANGELOW(i, 1, 255, 1)].lower)) + 1;
				Language_Execute_globalSP += 4;
			    };
			};
			Language_Execute_globalSP += 4;
		    }
		    break;
		case 176:
		    {
			register TLaddressint	descAddr;
			struct Language_PointerDescriptor	ptr;
			descAddr = (TLaddressint) ((* (struct Language_ClassDescriptor *) ((* (TLaddressint *) Language_Execute_globalSP))).baseClass);
			if ((unsigned long) descAddr == 0) {
			    Language_Execute_ExecutionError_Abort((TLint4) 10000, "Class implement-by(s) have not been initialized");
			};
			Language_Execute_Heap_New(&(ptr), (TLnat4) ((* (struct Language_ClassDescriptor *) descAddr).objSize));
			(* (TLaddressint *) Language_Execute_globalSP) = (TLaddressint) (ptr.dataPtr);
			Language_Execute_globalSP -= 4;
			if ((unsigned long) (ptr.dataPtr) != 0) {
			    (* (TLaddressint *) (ptr.dataPtr)) = (TLaddressint) 0;
			    if ((unsigned long) ((* (struct Language_ClassDescriptor *) descAddr).initRoutine) != 0) {
				(* (TLaddressint *) Language_Execute_globalSP) = (TLaddressint) ((* (struct Language_ClassDescriptor *) descAddr).initRoutine);
			    } else {
				(* (TLaddressint *) Language_Execute_globalSP) = (TLaddressint) ((unsigned long)Language_Execute_nullRoutine);
			    };
			} else {
			    (* (TLaddressint *) Language_Execute_globalSP) = (TLaddressint) ((unsigned long)Language_Execute_nullRoutine);
			};
			Language_Execute_globalSP -= 4;
			(* (TLint4 *) Language_Execute_globalSP) = 1;
			Language_Execute_globalSP -= 4;
			(* (TLaddressint *) Language_Execute_globalSP) = (TLaddressint) (ptr.dataPtr);
		    }
		    break;
		case 177:
		    {
			TLBIND((*ptr), TLaddressint);
			ptr = (void *) ((* (TLaddressint *) (((unsigned long) Language_Execute_globalSP + 4) + 4)));
			Language_Execute_Heap_Alloc(&((*ptr)), (TLnat4) ((* (TLnat4 *) ((unsigned long) Language_Execute_globalSP + 4))));
			if (((unsigned long) (*ptr) != 0) && ((unsigned long) ((* (TLaddressint *) Language_Execute_globalSP)) != 0)) {
			    TL_TLB_TLBUNI((TLaddressint) (*ptr), (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP)), (TLint4) 0);
			};
			Language_Execute_globalSP += 12;
		    }
		    break;
		case 178:
		    {
			if (((* (TLint4 *) Language_Execute_globalSP)) == 0) {
			    (* (TLint4 *) Language_Execute_globalSP) = 1;
			} else {
			    (* (TLint4 *) Language_Execute_globalSP) = 0;
			};
		    }
		    break;
		case 180:
		    {
			if ((unsigned long) ((* (TLaddressint *) Language_Execute_globalSP)) != 0) {
			    (* (TLaddressint *) Language_Execute_globalSP) = (TLaddressint) ((* (TLaddressint *) ((* (TLaddressint *) Language_Execute_globalSP))));
			};
		    }
		    break;
		case 183:
		    {
			register TLaddressint	sAddr;
			sAddr = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			Language_Execute_globalSP += 0;
			if ((((* (TLnat1 *) sAddr)) == 0) || (((* (TLnat1 *) ((unsigned long) sAddr + 1))) != 0)) {
			    TLEABT((TLint4) 37);
			};
			(* (TLint4 *) Language_Execute_globalSP) = (* (TLnat1 *) sAddr);
		    }
		    break;
		case 185:
		    {
			if (((* (TLint4 *) Language_Execute_globalSP)) == (TLint4) -0x80000000) {
			    TLEABT((TLint4) 50);
			};
			(* (TLint4 *) Language_Execute_globalSP) -= 1;
		    }
		    break;
		case 186:
		    {
			Language_Offset	localSize;
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			localSize = (* (Language_Offset *) Language_Execute_globalPC);
			Language_Execute_globalPC += Language_codeOffsetSize;
			if (((unsigned long) Language_Execute_globalSP < (unsigned long) (Language_Execute_RQHead->minimumSP)) || (((unsigned long) Language_Execute_globalSP - (unsigned long) (Language_Execute_RQHead->minimumSP)) <= (((2 * 2) + 4) + localSize))) {
			    {
				TLstring	__x3262;
				TL_TLS_TLSVIS((TLint4) Language_Execute_stackAllocated, (TLint4) 1, (TLint4) 10, __x3262);
				{
				    TLstring	__x3261;
				    TL_TLS_TLSVIS((TLint4) ((unsigned long) Language_Execute_globalSP - (unsigned long) (Language_Execute_RQHead->minimumSP)), (TLint4) 1, (TLint4) 10, __x3261);
				    {
					TLstring	__x3260;
					TL_TLS_TLSVIS((TLint4) localSize, (TLint4) 1, (TLint4) 10, __x3260);
					{
					    TLstring	__x3259;
					    TL_TLS_TLSCAT("Stack overflow! ", "Attempted to allocate ", __x3259);
					    {
						TLstring	__x3258;
						TL_TLS_TLSCAT(__x3259, __x3260, __x3258);
						{
						    TLstring	__x3257;
						    TL_TLS_TLSCAT(__x3258, " bytes of local variables with ", __x3257);
						    {
							TLstring	__x3256;
							TL_TLS_TLSCAT(__x3257, __x3261, __x3256);
							{
							    TLstring	__x3255;
							    TL_TLS_TLSCAT(__x3256, " bytes left of stack space. (Original stack size: ", __x3255);
							    {
								TLstring	__x3254;
								TL_TLS_TLSCAT(__x3255, __x3262, __x3254);
								{
								    TLstring	__x3253;
								    TL_TLS_TLSCAT(__x3254, " bytes)", __x3253);
								    Language_Execute_ExecutionError_Abort((TLint4) 10000, __x3253);
								};
							    };
							};
						    };
						};
					    };
					};
				    };
				};
			    };
			};
			Language_Execute_globalSP -= 2;
			(* (TLnat2 *) Language_Execute_globalSP) = Language_Execute_RQHead->srcPos.lineNo;
			Language_Execute_globalSP -= 2;
			(* (TLnat2 *) Language_Execute_globalSP) = Language_Execute_RQHead->srcPos.fileNo;
			Language_Execute_globalSP -= 4;
			(* (TLaddressint *) Language_Execute_globalSP) = (TLaddressint) fp;
			fp = (TLaddressint) Language_Execute_globalSP;
			Language_Execute_RQHead->fp = (TLaddressint) fp;
			if (localSize != 0) {

			    extern void memset ();
			    Language_Execute_globalSP -= localSize;
			    memset((TLaddressint) Language_Execute_globalSP, (TLint4) 0, (TLint4) localSize);
			};
		    }
		    break;
		case 187:
		    {
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			Language_Execute_globalSP -= 4;
			(* (TLaddressint *) Language_Execute_globalSP) = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalPC));
			Language_Execute_globalPC += Language_codeAddrSize;
		    }
		    break;
		case 188:
		    {
			Language_Execute_globalPC += Language_codeOffsetSize;
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			Language_Execute_globalSP -= 4;
			(* (TLaddressint *) Language_Execute_globalSP) = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalPC));
			Language_Execute_globalPC += Language_codeAddrSize;
		    }
		    break;
		case 189:
		    {
			Language_Execute_globalSP -= 4;
			(* (TLaddressint *) Language_Execute_globalSP) = (TLaddressint) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4)));
		    }
		    break;
		case 190:
		    {
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			Language_Execute_globalSP -= 4;
			(* (TLint4 *) Language_Execute_globalSP) = (* (TLint4 *) Language_Execute_globalPC);
			Language_Execute_globalPC += Language_codeIntSize;
		    }
		    break;
		case 191:
		    {
			Language_Execute_globalSP -= 4;
			(* (TLint4 *) Language_Execute_globalSP) = (* (TLint1 *) Language_Execute_globalPC);
			Language_Execute_globalPC += Language_codeInt1Size;
		    }
		    break;
		case 192:
		    {
			TLASSERT (((unsigned long) Language_Execute_globalPC & (1)) == 0);
			Language_Execute_globalSP -= 4;
			(* (TLint4 *) Language_Execute_globalSP) = (* (TLint2 *) Language_Execute_globalPC);
			Language_Execute_globalPC += Language_codeInt2Size;
		    }
		    break;
		case 193:
		    {
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			Language_Execute_globalSP -= 8;
			(* (TLreal8 *) Language_Execute_globalSP) = (* (TLreal8 *) Language_Execute_globalPC);
			Language_Execute_globalPC += Language_codeRealSize;
		    }
		    break;
		case 194:
		    {
			Language_Execute_globalSP -= 4;
			(* (TLint4 *) Language_Execute_globalSP) = 0;
		    }
		    break;
		case 195:
		    {
			Language_Execute_globalSP -= 4;
			(* (TLint4 *) Language_Execute_globalSP) = 1;
		    }
		    break;
		case 199:
		    {
			register TLreal8	rVal;
			rVal = (* (TLreal8 *) Language_Execute_globalSP);
			Language_Execute_globalSP += 8;
			if (rVal == 0) {
			    TLEABT((TLint4) 113);
			};
			if ((( TLSIMPLEABS(rVal)) < 1.) && (( fabs(((* (TLreal8 *) Language_Execute_globalSP)))) > (.1797693134862315517e307 * ( TLSIMPLEABS(rVal))))) {
			    TLEABT((TLint4) 51);
			};
			(* (TLreal8 *) Language_Execute_globalSP) /= rVal;
		    }
		    break;
		case 200:
		    {
			register TLint4	rVal;
			rVal = (* (TLint4 *) Language_Execute_globalSP);
			Language_Execute_globalSP += 4;
			if (rVal == 0) {
			    TLEABT((TLint4) 33);
			};
			(* (TLint4 *) Language_Execute_globalSP) -= rVal * (((* (TLint4 *) Language_Execute_globalSP)) / rVal);
		    }
		    break;
		case 201:
		    {
			register TLreal8	rVal;
			register TLreal8	realVal;
			register TLint4	intVal;
			rVal = (* (TLreal8 *) Language_Execute_globalSP);
			Language_Execute_globalSP += 8;
			if (rVal == 0) {
			    TLEABT((TLint4) 113);
			};
			Language_Execute_ExecutionError_SetOverflowKind((Language_Execute_ExecutionError_OverflowKind) 1);
			realVal = ((TLreal8) ((* (TLreal8 *) Language_Execute_globalSP)))  / ((TLreal8) rVal);
			intVal = TL_TLA_TLA8DV((TLreal8) rVal, (TLreal8) ((* (TLreal8 *) Language_Execute_globalSP)));
			if (( fabs((intVal - realVal))) >= 1.) {
			    Language_Execute_ExecutionError_Abort((TLint4) 10000, "Overflow in real \'mod\' operation");
			};
			(* (TLreal8 *) Language_Execute_globalSP) -= rVal * intVal;
			Language_Execute_ExecutionError_SetOverflowKind((Language_Execute_ExecutionError_OverflowKind) 0);
		    }
		    break;
		case 202:
		    {
			register TLaddressint	descAddr;
			descAddr = (TLaddressint) ((* (TLaddressint *) ((* (TLaddressint *) Language_Execute_globalSP))));
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			if ((unsigned long) descAddr != 0) {
			    (* (TLaddressint *) Language_Execute_globalSP) = (TLaddressint) ((* (TLaddressint *) ((unsigned long) descAddr + ((* (Language_Offset *) Language_Execute_globalPC)))));
			} else {
			    Language_Execute_ExecutionError_Abort((TLint4) 10000, "Module/class has not been initialized");
			};
			Language_Execute_globalPC += Language_codeOffsetSize;
		    }
		    break;
		case 203:
		    {
			if ((unsigned long) ((* (TLaddressint *) Language_Execute_globalSP)) == 0) {
			    Language_Execute_ExecutionError_Abort((TLint4) 10000, "Reference through nil pointer");
			};
		    }
		    break;
		case 204:
		    {
			Language_Execute_globalSP = (TLaddressint) ((* (TLaddressint *) ((* (TLaddressint *) Language_Execute_globalSP))));
		    }
		    break;
		case 205:
		    {
			register TLaddressint	oldSP;
			oldSP = (TLaddressint) Language_Execute_globalSP;
			Language_Execute_globalSP = (TLaddressint) fp;
			fp = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			Language_Execute_RQHead->fp = (TLaddressint) fp;
			if ((unsigned long) fp == 0) {
			    Language_Execute_RQHead = Language_Execute_RQHead->nextProcess;
			    Language_Execute_Kernel_RunQueueDispatch();
			    Language_Execute_globalPC = (TLaddressint) 0;
			} else {
			    TLnat2	newFileNo;
			    TLnat2	newLineNo;
			    newFileNo = (* (TLnat2 *) ((unsigned long) Language_Execute_globalSP + 4));
			    newLineNo = (* (TLnat2 *) (((unsigned long) Language_Execute_globalSP + 4) + 2));
			    Language_Execute_globalPC = (TLaddressint) ((* (TLaddressint *) (((unsigned long) Language_Execute_globalSP + 4) + (2 * 2))));
			    Language_Execute_globalSP += 12;
			    if (Language_debug) {
				{
				    TLstring	__x3304;
				    FileManager_FileName((FileManager_FileNoType) newFileNo, __x3304);
				    TL_TLI_TLISS ((TLint4) 0, (TLint2) 2);
				    TL_TLI_TLIPS ((TLint4) 0, "line ", (TLint2) 0);
				    TL_TLI_TLIPN ((TLint4) 0, (TLnat4) newLineNo, (TLint2) 0);
				    TL_TLI_TLIPS ((TLint4) 0, " of \"", (TLint2) 0);
				    TL_TLI_TLIPS ((TLint4) 0, __x3304, (TLint2) 0);
				    TL_TLI_TLIPS ((TLint4) 0, "\"", (TLint2) 0);
				    TL_TLI_TLIPK ((TLint2) 0);
				};
			    };
			    if ((unsigned long) Language_Execute_savedInitPC == (unsigned long) Language_Execute_globalPC) {
				Language_Execute_savedInitPC = (TLaddressint) 0;
				Language_Execute_savedInitFP = (TLaddressint) 0;
			    };
			    if ((newFileNo != (Language_Execute_RQHead->srcPos.fileNo)) || (newLineNo != (Language_Execute_RQHead->srcPos.lineNo))) {
				Language_Execute_RQHead->srcPos.fileNo = newFileNo;
				Language_Execute_RQHead->srcPos.lineNo = newLineNo;
				if ((unsigned long) (Language_Execute_RQHead->stepFP) == 0) {
				    if ((Language_Execute_RQHead->stepSpec) != 0) {
					(*runDesc).state = 4;
					goto __x3763;
				    };
				} else {
				    if ((unsigned long) fp == (unsigned long) (Language_Execute_RQHead->stepFP)) {
					Language_Execute_RQHead->stepFP = (TLaddressint) 0;
					if ((Language_Execute_RQHead->stepSpec) == 3) {
					    (*runDesc).state = 4;
					    goto __x3763;
					};
				    };
				};
			    } else {
				if (((unsigned long) (Language_Execute_RQHead->stepFP) != 0) && ((unsigned long) fp == (unsigned long) (Language_Execute_RQHead->stepFP))) {
				    Language_Execute_RQHead->stepFP = (TLaddressint) 0;
				    if ((Language_Execute_RQHead->stepSpec) == 3) {
					(*runDesc).state = 4;
					goto __x3763;
				    };
				};
			    };
			};
		    }
		    break;
		case 206:
		    {
			Language_Execute_globalPC = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			Language_Execute_globalSP += 4;
		    }
		    break;
		case 207:
		    {
			Language_Execute_globalSP += 4;
			(* (TLaddressint *) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP - 4)))) = (TLaddressint) Language_Execute_globalSP;
		    }
		    break;
		case 208:
		    {
			TLint4	stream;
			TL_TLI_TLIXSN = (* (TLint4 *) ((* (TLaddressint *) Language_Execute_globalSP)));
			Language_Execute_globalSP += 4;
			stream = Language_Execute_fileMap[TLINRANGELOW(TL_TLI_TLIXSN, -2, 20, 1)];
			if (stream == -3) {
			    {
				TLstring	__x3311;
				TL_TLS_TLSVIS((TLint4) TL_TLI_TLIXSN, (TLint4) 1, (TLint4) 10, __x3311);
				{
				    TLstring	__x3310;
				    TL_TLS_TLSCAT("I/O attempted on closed stream ", __x3311, __x3310);
				    Language_Execute_ExecutionError_Abort((TLint4) 10000, __x3310);
				};
			    };
			};
			TL_TLI_TLISK ((TLint4) ((* (TLint4 *) Language_Execute_globalSP)), (TLint4) stream);
			Language_Execute_globalSP += 4;
		    }
		    break;
		case 209:
		    {
			TLint4	stream;
			TL_TLI_TLIXSN = (* (TLint4 *) ((* (TLaddressint *) Language_Execute_globalSP)));
			Language_Execute_globalSP += 4;
			stream = Language_Execute_fileMap[TLINRANGELOW(TL_TLI_TLIXSN, -2, 20, 1)];
			if (stream == -3) {
			    {
				TLstring	__x3316;
				TL_TLS_TLSVIS((TLint4) TL_TLI_TLIXSN, (TLint4) 1, (TLint4) 10, __x3316);
				{
				    TLstring	__x3315;
				    TL_TLS_TLSCAT("I/O attempted on closed stream ", __x3316, __x3315);
				    Language_Execute_ExecutionError_Abort((TLint4) 10000, __x3315);
				};
			    };
			};
			TL_TLI_TLISKE ((TLint4) stream);
		    }
		    break;
		case 212:
		    {
			Language_Offset	sz;
			register TLint4	lo;
			register TLint4	hi;
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			sz = (* (Language_Offset *) Language_Execute_globalPC);
			Language_Execute_globalPC += Language_codeOffsetSize;
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			lo = (* (TLint4 *) Language_Execute_globalPC);
			hi = (* (TLint4 *) ((unsigned long) Language_Execute_globalPC + Language_codeIntSize));
			Language_Execute_globalPC += 2 * Language_codeIntSize;
			if (sz <= 4) {
			    register TLint4	value;
			    value = (* (TLint4 *) Language_Execute_globalSP);
			    if ((value < lo) || (value > hi)) {
				TLEABT((TLint4) 29);
			    };
			    (* (Language_set32 *) Language_Execute_globalSP) = (TLSMLSETCONST(TLINRANGELOW(value - lo, 0, 31, 19)));
			} else {
			    register TLaddressint	tgt;
			    register TLint4	value;
			    tgt = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			    value = (* (TLint4 *) ((unsigned long) Language_Execute_globalSP + 4));
			    if ((value < lo) || (value > hi)) {
				TLEABT((TLint4) 29);
			    };
			    {
				register TLint4	__x2181;
				TLint4	__x3324;
				__x3324 = sz >> 1;
				__x2181 = 1;
				if (__x2181 <= __x3324) {
				    for(;;) {
					(* (Language_set16 *) tgt) = 0x0;
					tgt += 2;
					if (__x2181 == __x3324) break;
					__x2181++;
				    }
				};
			    };
			    value -= lo;
			    (* (Language_set16 *) ((unsigned long) ((* (TLaddressint *) Language_Execute_globalSP)) + (2 * (value >> 4)))) = (TLSMLSMLSETCONST(TLINRANGELOW(value & 15, 0, 15, 19)));
			    (* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4)) = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			    Language_Execute_globalSP += 4;
			};
		    }
		    break;
		case 213:
		    {
			TLnat2	newFileNo;
			TLnat2	newLineNo;
			TLASSERT (((unsigned long) Language_Execute_globalPC & (1)) == 0);
			newFileNo = (* (TLnat2 *) Language_Execute_globalPC);
			Language_Execute_globalPC += Language_codeInt2Size;
			newLineNo = (* (TLnat2 *) Language_Execute_globalPC);
			Language_Execute_globalPC += Language_codeInt2Size;
			if (Language_debug) {
			    {
				TLstring	__x3332;
				FileManager_FileName((FileManager_FileNoType) newFileNo, __x3332);
				TL_TLI_TLISS ((TLint4) 0, (TLint2) 2);
				TL_TLI_TLIPS ((TLint4) 0, "line ", (TLint2) 0);
				TL_TLI_TLIPN ((TLint4) 0, (TLnat4) newLineNo, (TLint2) 0);
				TL_TLI_TLIPS ((TLint4) 0, " of \"", (TLint2) 0);
				TL_TLI_TLIPS ((TLint4) 0, __x3332, (TLint2) 0);
				TL_TLI_TLIPS ((TLint4) 0, "\"", (TLint2) 0);
				TL_TLI_TLIPK ((TLint2) 0);
			    };
			};
			if ((newFileNo != (Language_Execute_RQHead->srcPos.fileNo)) || (newLineNo != (Language_Execute_RQHead->srcPos.lineNo))) {
			    Language_Execute_RQHead->srcPos.fileNo = newFileNo;
			    Language_Execute_RQHead->srcPos.lineNo = newLineNo;
			    if (((Language_Execute_RQHead->stepSpec) != 0) && ((unsigned long) (Language_Execute_RQHead->stepFP) == 0)) {
				(*runDesc).state = 4;
				goto __x3763;
			    };
			};
		    }
		    break;
		case 214:
		    {
			TLnat2	newLineNo;
			TLASSERT (((unsigned long) Language_Execute_globalPC & (1)) == 0);
			newLineNo = (* (TLnat2 *) Language_Execute_globalPC);
			Language_Execute_globalPC += Language_codeInt2Size;
			if (Language_debug) {
			    {
				TLstring	__x3334;
				FileManager_FileName((FileManager_FileNoType) (Language_Execute_RQHead->srcPos.fileNo), __x3334);
				TL_TLI_TLISS ((TLint4) 0, (TLint2) 2);
				TL_TLI_TLIPS ((TLint4) 0, "line ", (TLint2) 0);
				TL_TLI_TLIPN ((TLint4) 0, (TLnat4) newLineNo, (TLint2) 0);
				TL_TLI_TLIPS ((TLint4) 0, " of \"", (TLint2) 0);
				TL_TLI_TLIPS ((TLint4) 0, __x3334, (TLint2) 0);
				TL_TLI_TLIPS ((TLint4) 0, "\"", (TLint2) 0);
				TL_TLI_TLIPK ((TLint2) 0);
			    };
			};
			if (newLineNo != (Language_Execute_RQHead->srcPos.lineNo)) {
			    Language_Execute_RQHead->srcPos.lineNo = newLineNo;
			    if (((Language_Execute_RQHead->stepSpec) != 0) && ((unsigned long) (Language_Execute_RQHead->stepFP) == 0)) {
				(*runDesc).state = 4;
				goto __x3763;
			    };
			};
		    }
		    break;
		case 221:
		    {
			TLint4	i;
			i = TL_TLS_TLSVSI(((* (TLstring *) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4))))), (TLint4) ((* (TLint4 *) Language_Execute_globalSP)));
			Language_Execute_globalSP += 4;
			(* (TLint4 *) Language_Execute_globalSP) = i;
		    }
		    break;
		case 222:
		    {
			TLboolean	ok;
			ok = Language_Execute_Strintok((* (TLstring *) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4)))), (TLint4) ((* (TLint4 *) Language_Execute_globalSP)));
			Language_Execute_globalSP += 4;
			if (ok) {
			    (* (TLint4 *) Language_Execute_globalSP) = 1;
			} else {
			    (* (TLint4 *) Language_Execute_globalSP) = 0;
			};
		    }
		    break;
		case 223:
		    {
			TLnat4	n;
			n = TL_TLS_TLSVSN(((* (TLstring *) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4))))), (TLint4) ((* (TLint4 *) Language_Execute_globalSP)));
			Language_Execute_globalSP += 4;
			(* (TLnat4 *) Language_Execute_globalSP) = n;
		    }
		    break;
		case 224:
		    {
			TLboolean	ok;
			ok = Language_Execute_Strnatok((* (TLstring *) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4)))), (TLint4) ((* (TLint4 *) Language_Execute_globalSP)));
			Language_Execute_globalSP += 4;
			if (ok) {
			    (* (TLint4 *) Language_Execute_globalSP) = 1;
			} else {
			    (* (TLint4 *) Language_Execute_globalSP) = 0;
			};
		    }
		    break;
		case 226:
		    {
			register TLint4	rVal;
			register TLint4	lVal;
			rVal = (* (TLint4 *) Language_Execute_globalSP);
			Language_Execute_globalSP += 4;
			lVal = (* (TLint4 *) Language_Execute_globalSP);
			(* (TLint4 *) Language_Execute_globalSP) -= rVal;
			if (((rVal < 0) && (((* (TLint4 *) Language_Execute_globalSP)) < lVal)) || ((rVal > 0) && (((* (TLint4 *) Language_Execute_globalSP)) > lVal))) {
			    TLEABT((TLint4) 50);
			};
		    }
		    break;
		case 230:
		    {
			register TLreal8	rVal;
			rVal = (* (TLreal8 *) Language_Execute_globalSP);
			Language_Execute_globalSP += 8;
			if ((((rVal < 0) && (((* (TLreal8 *) Language_Execute_globalSP)) > 0)) || ((rVal > 0) && (((* (TLreal8 *) Language_Execute_globalSP)) < 0))) && (( fabs(((* (TLreal8 *) Language_Execute_globalSP)))) > (.1797693134862315517e307 - ( TLSIMPLEABS(rVal))))) {
			    TLEABT((TLint4) 38);
			};
			(* (TLreal8 *) Language_Execute_globalSP) -= rVal;
		    }
		    break;
		case 231:
		    {
			TLBIND((*desc), struct Language_ArrayDescriptor);
			TLnat4	accum;
			desc = (void *) ((* (TLaddressint *) Language_Execute_globalSP));
			Language_Execute_globalSP += 4 + (4 * ((*desc).numDimensions));
			accum = 0;
			{
			    register TLint4	i;
			    TLint4	__x3365;
			    __x3365 = (*desc).numDimensions;
			    i = 1;
			    if (i <= __x3365) {
				for(;;) {
				    TLBINDREG((*dim), struct Language___x2321);
				    TLint4	subscr;
				    dim = &((*desc).dim[TLINRANGELOW(i, 1, 255, 1)]);
				    accum *= (*dim).range;
				    subscr = (* (TLint4 *) ((unsigned long) Language_Execute_globalSP - (4 * i)));
				    if (subscr < ((*dim).lower)) {
					TLEABT((TLint4) 11);
				    };
				    subscr -= (*dim).lower;
				    if (subscr >= ((*dim).range)) {
					TLEABT((TLint4) 11);
				    };
				    accum += subscr;
				    if (i == __x3365) break;
				    i++;
				}
			    };
			};
			(* (TLaddressint *) Language_Execute_globalSP) += accum * ((*desc).sizeElement);
		    }
		    break;
		case 232:
		    {
			Language_Offset	sz;
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			sz = (* (Language_Offset *) Language_Execute_globalPC);
			Language_Execute_globalPC += Language_codeOffsetSize;
			if (sz <= 4) {
			    (* (Language_set32 *) ((unsigned long) Language_Execute_globalSP + 4)) &= ~ (* (Language_set32 *) Language_Execute_globalSP);
			    Language_Execute_globalSP += 4;
			} else {
			    register TLaddressint	t;
			    register TLaddressint	r;
			    register TLaddressint	l;
			    t = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			    r = (TLaddressint) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4)));
			    l = (TLaddressint) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + (2 * 4))));
			    {
				register TLint4	__x2201;
				TLint4	__x3374;
				__x3374 = sz >> 1;
				__x2201 = 1;
				if (__x2201 <= __x3374) {
				    for(;;) {
					(* (Language_set16 *) t) = ((* (Language_set16 *) l))  & ~ ((* (Language_set16 *) r));
					t += 2;
					l += 2;
					r += 2;
					if (__x2201 == __x3374) break;
					__x2201++;
				    }
				};
			    };
			    (* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + (2 * 4))) = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			    Language_Execute_globalSP += 8;
			};
		    }
		    break;
		case 233:
		    {
			register TLaddressint	sAddr;
			register TLaddressint	tAddr;
			register TLint4	Index;
			register TLint4	slen;
			sAddr = (TLaddressint) ((* (TLaddressint *) (((unsigned long) Language_Execute_globalSP + 4) + 4)));
			tAddr = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			Index = (* (TLint4 *) ((unsigned long) Language_Execute_globalSP + 4));
			TLASSERT (sizeof(Language_SubstringKind) == 1);
			slen = TL_TLS_TLSLEN(((* (TLstring *) sAddr)));
			if (((* (Language_SubstringKind *) Language_Execute_globalPC)) == 1) {
			    Index += slen;
			} else {
			    TLASSERT (((* (Language_SubstringKind *) Language_Execute_globalPC)) == 0);
			};
			if (Index <= 0) {
			    Language_Execute_ExecutionError_Abort((TLint4) 10000, "Substring index is less than 1");
			} else {
			    if (Index > slen) {
				Language_Execute_ExecutionError_Abort((TLint4) 10000, "Substring index is greater than length of string");
			    };
			};
			(* (TLint2 *) tAddr) = 0;
			{
			    TLchar	__x3388[2];
			    TL_TLS_TLSBX(__x3388, (TLint4) Index, ((* (TLstring *) sAddr)));
			    TLSTRASS(1, (* (Language_string1 *) tAddr), __x3388);
			};
			Language_Execute_globalSP += 8;
			(* (TLaddressint *) Language_Execute_globalSP) = (TLaddressint) tAddr;
			Language_Execute_globalPC += Language_codeInt1Size;
		    }
		    break;
		case 234:
		    {
			register TLaddressint	sAddr;
			register TLaddressint	tAddr;
			register TLint4	firstIndex;
			register TLint4	lastIndex;
			register TLint4	slen;
			sAddr = (TLaddressint) ((* (TLaddressint *) (((unsigned long) Language_Execute_globalSP + 4) + (2 * 4))));
			tAddr = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			firstIndex = (* (TLint4 *) (((unsigned long) Language_Execute_globalSP + 4) + 4));
			lastIndex = (* (TLint4 *) ((unsigned long) Language_Execute_globalSP + 4));
			TLASSERT (sizeof(Language_SubstringKind) == 1);
			slen = TL_TLS_TLSLEN(((* (TLstring *) sAddr)));
			switch ((* (Language_SubstringKind *) Language_Execute_globalPC)) {
			    case 2:
				break;
			    case 3:
				{
				    lastIndex += slen;
				}
				break;
			    case 4:
				{
				    firstIndex += slen;
				}
				break;
			    case 5:
				{
				    firstIndex += slen;
				    lastIndex += slen;
				}
				break;
			    default:
				TLCASEABORT;
			};
			if (firstIndex <= 0) {
			    Language_Execute_ExecutionError_Abort((TLint4) 10000, "Left bound of substring is less than 1");
			} else {
			    if ((firstIndex - lastIndex) > 1) {
				Language_Execute_ExecutionError_Abort((TLint4) 10000, "Left bound of substring exceeds right bounds by more than 1");
			    } else {
				if (lastIndex > slen) {
				    Language_Execute_ExecutionError_Abort((TLint4) 10000, "Right bound of substring is greater than length of string");
				};
			    };
			};
			(* (TLint2 *) tAddr) = 0;
			{
			    TLstring	__x3399;
			    TL_TLS_TLSBXX(__x3399, (TLint4) lastIndex, (TLint4) firstIndex, ((* (TLstring *) sAddr)));
			    TLSTRASS(255, (* (TLstring *) tAddr), __x3399);
			};
			Language_Execute_globalSP += 12;
			(* (TLaddressint *) Language_Execute_globalSP) = (TLaddressint) tAddr;
			Language_Execute_globalPC += Language_codeInt1Size;
		    }
		    break;
		case 235:
		    {
			if (((* (TLint4 *) Language_Execute_globalSP)) == 2147483647) {
			    TLEABT((TLint4) 50);
			};
			(* (TLint4 *) Language_Execute_globalSP) += 1;
		    }
		    break;
		case 253:
		    {
			TLint4	exitCode;
			exitCode = (* (TLint4 *) Language_Execute_globalSP);
			Language_Execute_globalSP += 4;
			if (exitCode != 0) {
			    {
				TLstring	__x3406;
				TL_TLS_TLSVIS((TLint4) exitCode, (TLint4) 1, (TLint4) 10, __x3406);
				{
				    TLstring	__x3405;
				    TL_TLS_TLSCAT("Program exit code ", __x3406, __x3405);
				    Language_ErrorModule_Message(&(Language_Execute_RQHead->srcPos), (Language_ErrorModule_Severity) 2, __x3405);
				};
			    };
			};
			(*runDesc).state = 0;
			goto __x3763;
		    }
		    break;
		case 236:
		    {
			TLBINDREG((*um), struct Language_UnionMapDescriptor);
			TLaddressint	uninitMapAddr;
			register TLint4	tagVal;
			register TLaddressint	unionAddr;
			um = (void *) ((* (TLaddressint *) Language_Execute_globalSP));
			Language_Execute_globalSP += 4;
			uninitMapAddr = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			Language_Execute_globalSP += 4;
			tagVal = (* (TLint4 *) Language_Execute_globalSP);
			Language_Execute_globalSP += 4;
			unionAddr = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			Language_Execute_globalSP += 4;
			if ((tagVal < ((*um).lower)) || (tagVal > ((*um).upper))) {
			    TLEABT((TLint4) 14);
			};
			switch ((* (TLint1 *) Language_Execute_globalPC)) {
			    case 1:
				{
				    (* (TLint1 *) unionAddr) = tagVal;
				}
				break;
			    case 2:
				{
				    (* (TLint2 *) unionAddr) = tagVal;
				}
				break;
			    case 4:
				{
				    (* (TLint4 *) unionAddr) = tagVal;
				}
				break;
			    default:
				TLCASEABORT;
			};
			TL_TLB_TLBUUN((TLaddressint) unionAddr, (TLint4) (tagVal - ((*um).lower)), (TLint4) ((unsigned long)(*um).map), (TLint4) uninitMapAddr);
			Language_Execute_globalPC += Language_codeInt1Size;
		    }
		    break;
		case 237:
		    {
			TLint4	stream;
			TL_TLI_TLIXSN = (* (TLint4 *) ((* (TLaddressint *) Language_Execute_globalSP)));
			Language_Execute_globalSP += 4;
			stream = Language_Execute_fileMap[TLINRANGELOW(TL_TLI_TLIXSN, -2, 20, 1)];
			if (stream == -3) {
			    {
				TLstring	__x3419;
				TL_TLS_TLSVIS((TLint4) TL_TLI_TLIXSN, (TLint4) 1, (TLint4) 10, __x3419);
				{
				    TLstring	__x3418;
				    TL_TLS_TLSCAT("I/O attempted on closed stream ", __x3419, __x3418);
				    Language_Execute_ExecutionError_Abort((TLint4) 10000, __x3418);
				};
			    };
			};
			TL_TLI_TLITL (&((* (TLint4 *) ((* (TLaddressint *) Language_Execute_globalSP)))), (TLint4) stream);
			Language_Execute_globalSP += 4;
		    }
		    break;
		case 238:
		    {
			TLBINDREG((*um), struct Language_UnionMapDescriptor);
			register TLint4	tagVal;
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			um = (void *) ((* (TLaddressint *) Language_Execute_globalSP));
			switch ((* (Language_Offset *) ((unsigned long) Language_Execute_globalPC + (2 * Language_codeOffsetSize)))) {
			    case 1:
				{
				    tagVal = (* (TLint1 *) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4))));
				}
				break;
			    case 2:
				{
				    tagVal = (* (TLint2 *) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4))));
				}
				break;
			    case 4:
				{
				    tagVal = (* (TLint4 *) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4))));
				}
				break;
			    default:
				TLCASEABORT;
			};
			if ((tagVal < ((*um).lower)) || (tagVal > ((*um).upper))) {
			    TLEABT((TLint4) 34);
			} else {
			    if (((*um).map[TLINRANGELOW(tagVal - ((*um).lower), 0, 300, 1)]) != ((* (Language_Offset *) ((unsigned long) Language_Execute_globalPC + Language_codeOffsetSize)))) {
				TLEABT((TLint4) 13);
			    };
			};
			Language_Execute_globalSP += 4;
			(* (TLaddressint *) Language_Execute_globalSP) += (* (Language_Offset *) Language_Execute_globalPC);
			Language_Execute_globalPC += 3 * Language_codeOffsetSize;
		    }
		    break;
		case 239:
		    {
			TL_TLB_TLBUNI((TLaddressint) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4))), (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP)), (TLint4) 0);
			Language_Execute_globalSP += 8;
		    }
		    break;
		case 240:
		    {
			(* (TLaddressint *) ((* (TLaddressint *) Language_Execute_globalSP))) = (TLaddressint) 0xFFFFFFFF;
			Language_Execute_globalSP += 4;
		    }
		    break;
		case 241:
		    {
			(* (TLnat1 *) ((* (TLaddressint *) Language_Execute_globalSP))) = 255;
			Language_Execute_globalSP += 4;
		    }
		    break;
		case 242:
		    {
			(* (TLint4 *) ((* (TLaddressint *) Language_Execute_globalSP))) = (TLint4) -0x80000000;
			Language_Execute_globalSP += 4;
		    }
		    break;
		case 244:
		    {
			(* (TLnat4 *) ((* (TLaddressint *) Language_Execute_globalSP))) = (TLint4) -0x80000000;
			(* (TLnat4 *) ((unsigned long) ((* (TLaddressint *) Language_Execute_globalSP)) + 4)) = (TLint4) -0x80000000;
			Language_Execute_globalSP += 4;
		    }
		    break;
		case 245:
		    {
			(* (TLnat1 *) ((* (TLaddressint *) Language_Execute_globalSP))) = 128;
			(* (TLnat1 *) ((unsigned long) ((* (TLaddressint *) Language_Execute_globalSP)) + 1)) = 0;
			Language_Execute_globalSP += 4;
		    }
		    break;
		case 247:
		    {
			Language_Offset	sz;
			register TLint4	lo;
			register TLint4	hi;
			register TLint4	subVal;
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			sz = (* (Language_Offset *) Language_Execute_globalPC);
			Language_Execute_globalPC += Language_codeOffsetSize;
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			lo = (* (TLint4 *) Language_Execute_globalPC);
			hi = (* (TLint4 *) ((unsigned long) Language_Execute_globalPC + Language_codeIntSize));
			Language_Execute_globalPC += 2 * Language_codeIntSize;
			subVal = (* (TLint4 *) Language_Execute_globalSP);
			if ((subVal < lo) || (subVal > hi)) {
			    TLEABT((TLint4) 11);
			};
			Language_Execute_globalSP += 4;
			(* (TLaddressint *) Language_Execute_globalSP) += (subVal - lo) * sz;
		    }
		    break;
		case 249:
		    {
			TLint4	status;
			TLint4	sizeWritten;
			register TLaddressint	statusAddr;
			TLint4	stream;
			register TLaddressint	resultAddr;
			statusAddr = (TLaddressint) ((* (TLaddressint *) ((* (TLaddressint *) Language_Execute_globalSP))));
			Language_Execute_globalSP += 4;
			TL_TLI_TLIXSN = (* (TLint4 *) ((* (TLaddressint *) Language_Execute_globalSP)));
			Language_Execute_globalSP += 4;
			stream = Language_Execute_fileMap[TLINRANGELOW(TL_TLI_TLIXSN, -2, 20, 1)];
			if (stream == -3) {
			    {
				TLstring	__x3460;
				TL_TLS_TLSVIS((TLint4) TL_TLI_TLIXSN, (TLint4) 1, (TLint4) 10, __x3460);
				{
				    TLstring	__x3459;
				    TL_TLS_TLSCAT("I/O attempted on closed stream ", __x3460, __x3459);
				    Language_Execute_ExecutionError_Abort((TLint4) 10000, __x3459);
				};
			    };
			};
			resultAddr = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			Language_Execute_globalSP += 4;
			TL_TLI_TLISSS (&status, (TLint4) stream, (TLint2) 4);
			TL_TLI_TLIWRR (&sizeWritten, &((* (TLint4 *) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4))))), (TLint4) ((* (TLint4 *) Language_Execute_globalSP)), &status, (TLint2) stream);
			if ((unsigned long) statusAddr != 0) {
			    (* (TLint4 *) statusAddr) = status;
			};
			if ((unsigned long) resultAddr != 0) {
			    (* (TLint4 *) resultAddr) = sizeWritten;
			};
			Language_Execute_globalSP += 8;
		    }
		    break;
		case 250:
		    {
			Language_Execute_globalSP += 4;
			(* (TLnat4 *) Language_Execute_globalSP) = ((* (TLnat4 *) Language_Execute_globalSP)) ^ ((* (TLnat4 *) ((unsigned long) Language_Execute_globalSP - 4)));
		    }
		    break;
		case 251:
		    {
			Language_Offset	sz;
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			sz = (* (Language_Offset *) Language_Execute_globalPC);
			Language_Execute_globalPC += Language_codeOffsetSize;
			if (sz <= 4) {
			    (* (Language_set32 *) ((unsigned long) Language_Execute_globalSP + 4)) ^= (* (Language_set32 *) Language_Execute_globalSP);
			    Language_Execute_globalSP += 4;
			} else {
			    register TLaddressint	t;
			    register TLaddressint	r;
			    register TLaddressint	l;
			    t = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			    r = (TLaddressint) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4)));
			    l = (TLaddressint) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + (2 * 4))));
			    {
				register TLint4	__x2232;
				TLint4	__x3476;
				__x3476 = sz >> 1;
				__x2232 = 1;
				if (__x2232 <= __x3476) {
				    for(;;) {
					(* (Language_set16 *) t) = ((* (Language_set16 *) l)) ^ ((* (Language_set16 *) r));
					t += 2;
					l += 2;
					r += 2;
					if (__x2232 == __x3476) break;
					__x2232++;
				    }
				};
			    };
			    (* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + (2 * 4))) = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			    Language_Execute_globalSP += 8;
			};
		    }
		    break;
		case 47:
		    {
			TLBIND((*HA), struct Language_Execute_HandlerArea);
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			HA = ((unsigned long) fp - ((* (Language_Offset *) ((unsigned long) Language_Execute_globalPC + Language_codeOffsetSize))));
			(*HA).nextHandler = Language_Execute_RQHead->activeHandler;
			(*HA).handlerRoutine = (TLaddressint) ((unsigned long) Language_Execute_globalPC + (2 * Language_codeOffsetSize));
			(*HA).savedRegisters.sp = (TLaddressint) Language_Execute_globalSP;
			(*HA).savedRegisters.fp = (TLaddressint) fp;
			* (TLnat4 *) &Language_Execute_RQHead->activeHandler = ((unsigned long)&((*HA)));
			Language_Execute_globalPC += (* (Language_Offset *) Language_Execute_globalPC);
		    }
		    break;
		case 48:
		    {
			Language_Offset	sz;
			register TLnat4	mask;
			register TLint4	shift;
			register TLaddressint	tgt;
			register TLnat4	value;
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			sz = (* (Language_Offset *) Language_Execute_globalPC);
			Language_Execute_globalPC += Language_codeOffsetSize;
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			mask = (* (TLnat4 *) Language_Execute_globalPC);
			shift = (* (TLint4 *) ((unsigned long) Language_Execute_globalPC + Language_codeIntSize));
			Language_Execute_globalPC += 2 * Language_codeIntSize;
			tgt = (TLaddressint) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4)));
			value = (TLint4) ((* (TLnat4 *) Language_Execute_globalSP)) << shift;
			if (((value >> shift) != ((* (TLnat4 *) Language_Execute_globalSP))) || ((value & mask) != value)) {
			    TLEABT((TLint4) 50);
			};
			switch (sz) {
			    case 1:
				{
				    (* (TLnat1 *) tgt) &= ((TLnat4) (- 1)) ^ mask;
				    (* (TLnat1 *) tgt) |= value;
				}
				break;
			    case 2:
				{
				    (* (TLnat2 *) tgt) &= ((TLnat4) (- 1)) ^ mask;
				    (* (TLnat2 *) tgt) |= value;
				}
				break;
			    case 4:
				{
				    (* (TLnat4 *) tgt) &= ((TLnat4) (- 1)) ^ mask;
				    (* (TLnat4 *) tgt) |= value;
				}
				break;
			    default:
				TLCASEABORT;
			};
			Language_Execute_globalSP += 8;
		    }
		    break;
		case 49:
		    {
			TLBINDREG((*val), TLnat4);
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			val = (void *) Language_Execute_globalSP;
			(*val) = ((*val) & ((* (TLnat4 *) Language_Execute_globalPC))) >> ((* (TLnat4 *) ((unsigned long) Language_Execute_globalPC + Language_codeIntSize)));
			Language_Execute_globalPC += 2 * Language_codeIntSize;
		    }
		    break;
		case 57:
		    {
			(* (TLchar *) ((* (TLaddressint *) Language_Execute_globalSP))) = ((TLchar) ((* (TLint4 *) ((unsigned long) Language_Execute_globalSP + 4))));
			(* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4)) = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			Language_Execute_globalSP += 4;
		    }
		    break;
		case 58:
		    {
			register TLaddressint	tAddr;
			register TLint4	value;
			tAddr = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			value = (* (TLint4 *) ((unsigned long) Language_Execute_globalSP + 4));
			if ((value & 127) == 0) {
			    Language_Execute_ExecutionError_Abort((TLint4) 10000, "char converted to string is EOS or uninitchar");
			};
			(* (TLnat2 *) tAddr) = 0;
			(* (TLchar *) tAddr) = ((TLchar) value);
			(* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4)) = (TLaddressint) tAddr;
			Language_Execute_globalSP += 4;
		    }
		    break;
		case 59:
		    {
			register TLaddressint	tAddr;
			register TLint4	value;
			tAddr = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			value = (* (TLint4 *) ((unsigned long) Language_Execute_globalSP + (2 * 4)));
			if ((value & 127) == 0) {
			    Language_Execute_ExecutionError_Abort((TLint4) 10000, "char converted to string is EOS or uninitchar");
			};
			(* (TLnat2 *) tAddr) = 0;
			(* (TLchar *) tAddr) = ((TLchar) value);
			(* (TLaddressint *) (((unsigned long) Language_Execute_globalSP + 4) + 4)) = (TLaddressint) tAddr;
			(* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4)) = (TLaddressint) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4)));
			Language_Execute_globalSP += 4;
		    }
		    break;
		case 61:
		    {
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			if (TL_TLS_TLSLEN(((* (TLstring *) ((* (TLaddressint *) Language_Execute_globalSP))))) > ((* (TLint4 *) Language_Execute_globalPC))) {
			    {
				TLstring	__x3522;
				TL_TLS_TLSVIS((TLint4) ((* (TLint4 *) Language_Execute_globalPC)), (TLint4) 1, (TLint4) 10, __x3522);
				{
				    TLstring	__x3520;
				    TL_TLS_TLSCAT("Length of char(n) parameter is not ", __x3522, __x3520);
				    Language_Execute_ExecutionError_Abort((TLint4) 10000, __x3520);
				};
			    };
			};
			Language_Execute_globalPC += Language_codeIntSize;
		    }
		    break;
		case 60:
		    {
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			if (((* (TLint4 *) Language_Execute_globalSP)) != ((* (Language_Offset *) Language_Execute_globalPC))) {
			    TLEABT((TLint4) 42);
			};
			Language_Execute_globalSP += 4;
			Language_Execute_globalPC += Language_codeOffsetSize;
		    }
		    break;
		case 64:
		    {
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			if (TL_TLS_TLSLEN(((* (TLstring *) ((* (TLaddressint *) Language_Execute_globalSP))))) != ((* (Language_Offset *) Language_Execute_globalPC))) {
			    TLEABT((TLint4) 42);
			};
			Language_Execute_globalPC += Language_codeOffsetSize;
		    }
		    break;
		case 67:
		    {
			if (((* (TLint4 *) Language_Execute_globalSP)) != 1) {
			    Language_Execute_ExecutionError_Abort((TLint4) 10000, "char(n) coerced to char is not length 1");
			};
			(* (TLint4 *) ((((unsigned long) Language_Execute_globalSP + 4) + 4) - 4)) = ((TLnat4) ((* (TLchar *) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4))))));
			Language_Execute_globalSP += 4;
		    }
		    break;
		case 68:
		    {
			register TLaddressint	tAddr;
			register TLaddressint	sAddr;
			register TLint4	value;
			tAddr = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			sAddr = (TLaddressint) ((* (TLaddressint *) (((unsigned long) Language_Execute_globalSP + 4) + 4)));
			if (((* (TLint4 *) ((unsigned long) Language_Execute_globalSP + 4))) >= 256) {
			    {
				TLstring	__x3538;
				TL_TLS_TLSVIS((TLint4) ((* (TLint4 *) ((unsigned long) Language_Execute_globalSP + 4))), (TLint4) 1, (TLint4) 10, __x3538);
				{
				    TLstring	__x3536;
				    TL_TLS_TLSCAT("char(n) size of ", __x3538, __x3536);
				    {
					TLstring	__x3535;
					TL_TLS_TLSCAT(__x3536, " is too large to convert to string", __x3535);
					Language_Execute_ExecutionError_Abort((TLint4) 10000, __x3535);
				    };
				};
			    };
			};
			{
			    register TLint4	__x2247;
			    TLint4	__x3540;
			    __x3540 = (* (TLint4 *) ((unsigned long) Language_Execute_globalSP + 4));
			    __x2247 = 1;
			    if (__x2247 <= __x3540) {
				for(;;) {
				    value = (* (TLnat1 *) sAddr);
				    sAddr += 1;
				    if ((value & 127) == 0) {
					Language_Execute_ExecutionError_Abort((TLint4) 10000, "char(n) converted to string contains EOS of uninitchar");
				    };
				    (* (TLnat1 *) tAddr) = value;
				    tAddr += 1;
				    if (__x2247 == __x3540) break;
				    __x2247++;
				}
			    };
			};
			(* (TLnat1 *) tAddr) = 0;
			(* (TLaddressint *) (((unsigned long) Language_Execute_globalSP + 4) + 4)) = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			Language_Execute_globalSP += 8;
		    }
		    break;
		case 69:
		    {
			register TLaddressint	tAddr;
			register TLaddressint	sAddr;
			register TLint4	value;
			tAddr = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			sAddr = (TLaddressint) ((* (TLaddressint *) (((unsigned long) Language_Execute_globalSP + (2 * 4)) + 4)));
			if (((* (TLint4 *) ((unsigned long) Language_Execute_globalSP + 4))) >= 256) {
			    {
				TLstring	__x3552;
				TL_TLS_TLSVIS((TLint4) ((* (TLint4 *) ((unsigned long) Language_Execute_globalSP + 4))), (TLint4) 1, (TLint4) 10, __x3552);
				{
				    TLstring	__x3550;
				    TL_TLS_TLSCAT("char(n) size of ", __x3552, __x3550);
				    {
					TLstring	__x3549;
					TL_TLS_TLSCAT(__x3550, " is too large to convert to string", __x3549);
					Language_Execute_ExecutionError_Abort((TLint4) 10000, __x3549);
				    };
				};
			    };
			};
			{
			    register TLint4	__x2251;
			    TLint4	__x3554;
			    __x3554 = (* (TLint4 *) ((unsigned long) Language_Execute_globalSP + 4));
			    __x2251 = 1;
			    if (__x2251 <= __x3554) {
				for(;;) {
				    value = (* (TLnat1 *) sAddr);
				    sAddr += 1;
				    if ((value & 127) == 0) {
					Language_Execute_ExecutionError_Abort((TLint4) 10000, "char(n) converted to string contains EOS of uninitchar");
				    };
				    (* (TLnat1 *) tAddr) = value;
				    tAddr += 1;
				    if (__x2251 == __x3554) break;
				    __x2251++;
				}
			    };
			};
			(* (TLnat1 *) tAddr) = 0;
			(* (TLaddressint *) (((unsigned long) Language_Execute_globalSP + (2 * 4)) + 4)) = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			Language_Execute_globalSP += 8;
		    }
		    break;
		case 73:
		    {
			register TLnat4	rVal;
			rVal = (* (TLnat4 *) Language_Execute_globalSP);
			if (rVal == 0) {
			    TLEABT((TLint4) 33);
			};
			Language_Execute_globalSP += 4;
			(* (TLnat4 *) Language_Execute_globalSP) /= rVal;
		    }
		    break;
		case 75:
		    {
			if (Language_Execute_Kernel_ConditionEmpty((* (struct Language_Execute_ConditionDescriptor * *) Language_Execute_globalSP))) {
			    Language_Execute_globalSP += 0;
			    (* (TLint4 *) Language_Execute_globalSP) = 1;
			} else {
			    Language_Execute_globalSP += 0;
			    (* (TLint4 *) Language_Execute_globalSP) = 0;
			};
		    }
		    break;
		case 79:
		    {
			register TLaddressint	r;
			register TLaddressint	l;
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			r = (TLaddressint) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4)));
			l = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			Language_Execute_globalSP += -4;
			(* (TLint4 *) Language_Execute_globalSP) = 1;
			{
			    register TLint4	__x2255;
			    TLint4	__x3569;
			    __x3569 = (* (Language_Offset *) Language_Execute_globalPC);
			    __x2255 = 1;
			    if (__x2255 <= __x3569) {
				for(;;) {
				    if (((* (TLchar *) r)) != ((* (TLchar *) l))) {
					(* (TLint4 *) Language_Execute_globalSP) = 0;
					break;
				    };
				    r += 1;
				    l += 1;
				    if (__x2255 == __x3569) break;
				    __x2255++;
				}
			    };
			};
			Language_Execute_globalPC += Language_codeOffsetSize;
		    }
		    break;
		case 81:
		    {
			Language_Execute_globalSP += 4;
			if (((* (TLint4 *) Language_Execute_globalSP)) < 0) {
			    (* (TLint4 *) Language_Execute_globalSP) = 0;
			} else {
			    if (((* (TLint4 *) ((unsigned long) Language_Execute_globalSP - 4))) == ((* (TLint4 *) Language_Execute_globalSP))) {
				(* (TLint4 *) Language_Execute_globalSP) = 1;
			    } else {
				(* (TLint4 *) Language_Execute_globalSP) = 0;
			    };
			};
		    }
		    break;
		case 84:
		    {
			Language_Offset	sz;
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			sz = (* (Language_Offset *) Language_Execute_globalPC);
			Language_Execute_globalPC += Language_codeOffsetSize;
			if (sz <= 4) {
			    Language_Execute_globalSP += 4;
			    if (((* (Language_set32 *) Language_Execute_globalSP)) == ((* (Language_set32 *) ((unsigned long) Language_Execute_globalSP - 4)))) {
				(* (TLint4 *) Language_Execute_globalSP) = 1;
			    } else {
				(* (TLint4 *) Language_Execute_globalSP) = 0;
			    };
			} else {
			    register TLaddressint	l;
			    register TLaddressint	r;
			    l = (TLaddressint) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4)));
			    r = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			    Language_Execute_globalSP += 4;
			    (* (TLint4 *) Language_Execute_globalSP) = 1;
			    {
				register TLint4	__x2259;
				TLint4	__x3587;
				__x3587 = sz >> 1;
				__x2259 = 1;
				if (__x2259 <= __x3587) {
				    for(;;) {
					if (!(((* (Language_set16 *) l)) == ((* (Language_set16 *) r)))) {
					    (* (TLint4 *) Language_Execute_globalSP) = 0;
					    break;
					};
					l += 2;
					r += 2;
					if (__x2259 == __x3587) break;
					__x2259++;
				    }
				};
			    };
			};
		    }
		    break;
		case 103:
		    {
			Language_Offset	sz;
			register TLaddressint	sAddr;
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			sz = (* (Language_Offset *) Language_Execute_globalPC);
			Language_Execute_globalPC += Language_codeOffsetSize;
			sAddr = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			Language_Execute_globalSP += 0;
			switch (sz) {
			    case 1:
				{
				    if (((* (TLnat1 *) sAddr)) == 128) {
					TLEABT((TLint4) 45);
				    };
				    (* (TLnat4 *) Language_Execute_globalSP) = (* (TLnat1 *) sAddr);
				}
				break;
			    case 2:
				{
				    if (((* (TLnat2 *) sAddr)) == 32768) {
					TLEABT((TLint4) 45);
				    };
				    (* (TLnat4 *) Language_Execute_globalSP) = (* (TLnat2 *) sAddr);
				}
				break;
			    case 4:
				{
				    if (((* (TLnat4 *) sAddr)) == 0x80000000) {
					TLEABT((TLint4) 45);
				    };
				    (* (TLnat4 *) Language_Execute_globalSP) = (* (TLnat4 *) sAddr);
				}
				break;
			    default:
				TLCASEABORT;
			};
		    }
		    break;
		case 107:
		    {
			TLaddressint	procName;
			Language_Offset	formalSize;
			TLaddressint	procAddr;
			TLint4	stackSize;
			TLaddressint	status;
			TLaddressint	newPC;
			TLaddressint	stackBase;
			Language_Execute_globalPC += Language_codeOffsetSize;
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			procName = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalPC));
			Language_Execute_globalPC += Language_codeAddrSize;
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			formalSize = (* (Language_Offset *) Language_Execute_globalPC);
			Language_Execute_globalPC += Language_codeOffsetSize;
			procAddr = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			stackSize = (* (TLint4 *) ((unsigned long) Language_Execute_globalSP + 4));
			status = (TLaddressint) ((* (TLaddressint *) (((unsigned long) Language_Execute_globalSP + 4) + 4)));
			newPC = (TLaddressint) ((* (TLaddressint *) ((((unsigned long) Language_Execute_globalSP + (2 * 4)) + 4) + formalSize)));
			if ((stackSize & 3) != 0) {
			    stackSize += 4 - (stackSize & 3);
			};
			stackBase = (TLaddressint) 0;
			if (stackSize > (formalSize + 12)) {
			    Language_Execute_Heap_Alloc(&(stackBase), (TLnat4) ((256 + stackSize) + sizeof(struct Language_Execute_ProcessDescriptor)));
			};
			if ((unsigned long) stackBase == 0) {
			    if ((unsigned long) status != 0) {
				(* (TLboolean *) status) = 0;
			    } else {
				Language_Execute_ExecutionError_Abort((TLint4) 10000, "Insufficient memory to fork new process");
			    };
			} else {
			    register struct Language_Execute_ProcessDescriptor	*newProc;
			    register TLaddressint	tgt;
			    register TLaddressint	src;
			    Language_Execute_processCount += 1;
			    newProc = ((unsigned long) stackBase + 256) + stackSize;
			    TLSTRCTASS((*(newProc)), (*(Language_Execute_RQHead)), struct Language_Execute_ProcessDescriptor);
			    newProc->stackBase = (TLaddressint) stackBase;
			    TLSTRASS(255, newProc->errMessage, "");
			    TLSTRASS(255, newProc->prevErrMessage, "");
			    newProc->name = (TLaddressint) procName;
			    newProc->totalPause = 0;
			    newProc->activeHandler = (struct Language_Execute_HandlerArea *) 0;
			    newProc->runningHandler = (struct Language_Execute_HandlerArea *) 0;
			    newProc->stepSpec = 0;
			    newProc->stepFP = (TLaddressint) 0;
			    tgt = (TLaddressint) (((TLnat4) newProc) - formalSize);
			    src = (TLaddressint) (((unsigned long) Language_Execute_globalSP + (2 * 4)) + 4);
			    {
				register TLint4	__x2272;
				TLint4	__x3609;
				__x3609 = formalSize >> 2;
				__x2272 = 1;
				if (__x2272 <= __x3609) {
				    for(;;) {
					(* (TLint4 *) tgt) = (* (TLint4 *) src);
					tgt += 4;
					src += 4;
					if (__x2272 == __x3609) break;
					__x2272++;
				    }
				};
			    };
			    newProc->fp = (TLaddressint) 0;
			    newProc->sp = (TLaddressint) ((((TLnat4) newProc) - formalSize) - 4);
			    (* (TLaddressint *) (newProc->sp)) = (TLaddressint) Language_Execute_globalPC;
			    newProc->minimumSP = (TLaddressint) ((unsigned long) stackBase + 256);
			    newProc->pc = (TLaddressint) newPC;
			    if ((unsigned long) status != 0) {
				(* (TLboolean *) status) = 1;
			    };
			    if ((unsigned long) ((* (TLaddressint *) Language_Execute_globalSP)) != 0) {
				(* (TLaddressint *) ((* (TLaddressint *) Language_Execute_globalSP))) = (TLaddressint) ((TLnat4) newProc);
			    };
			    Language_Execute_Kernel_RunQueueInsert(newProc);
			};
			Language_Execute_globalSP += ((3 * 4) + 4) + formalSize;
		    }
		    break;
		case 111:
		    {
			register TLaddressint	r;
			register TLaddressint	l;
			r = (TLaddressint) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4)));
			l = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			Language_Execute_globalSP += -4;
			(* (TLint4 *) Language_Execute_globalSP) = 1;
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			{
			    register TLint4	__x2275;
			    TLint4	__x3621;
			    __x3621 = (* (Language_Offset *) Language_Execute_globalPC);
			    __x2275 = 1;
			    if (__x2275 <= __x3621) {
				for(;;) {
				    if (((* (TLchar *) r)) < ((* (TLchar *) l))) {
					(* (TLint4 *) Language_Execute_globalSP) = 0;
					break;
				    };
				    r += 1;
				    l += 1;
				    if (__x2275 == __x3621) break;
				    __x2275++;
				}
			    };
			};
			Language_Execute_globalPC += Language_codeOffsetSize;
		    }
		    break;
		case 114:
		    {
			Language_Execute_globalSP += 4;
			if (((* (TLint4 *) ((unsigned long) Language_Execute_globalSP - 4))) < 0) {
			    (* (TLint4 *) Language_Execute_globalSP) = 0;
			} else {
			    if (((* (TLint4 *) Language_Execute_globalSP)) >= ((* (TLint4 *) ((unsigned long) Language_Execute_globalSP - 4)))) {
				(* (TLint4 *) Language_Execute_globalSP) = 1;
			    } else {
				(* (TLint4 *) Language_Execute_globalSP) = 0;
			    };
			};
		    }
		    break;
		case 115:
		    {
			Language_Execute_globalSP += 4;
			if (((* (TLnat4 *) Language_Execute_globalSP)) >= ((* (TLnat4 *) ((unsigned long) Language_Execute_globalSP - 4)))) {
			    (* (TLint4 *) Language_Execute_globalSP) = 1;
			} else {
			    (* (TLint4 *) Language_Execute_globalSP) = 0;
			};
		    }
		    break;
		case 116:
		    {
			Language_Execute_globalSP += 4;
			if (((* (TLint4 *) Language_Execute_globalSP)) < 0) {
			    (* (TLint4 *) Language_Execute_globalSP) = 1;
			} else {
			    if (((* (TLint4 *) Language_Execute_globalSP)) >= ((* (TLint4 *) ((unsigned long) Language_Execute_globalSP - 4)))) {
				(* (TLint4 *) Language_Execute_globalSP) = 1;
			    } else {
				(* (TLint4 *) Language_Execute_globalSP) = 0;
			    };
			};
		    }
		    break;
		case 121:
		    {
			Language_Execute_globalSP -= 4;
			(* (TLint4 *) Language_Execute_globalSP) = Language_Execute_Kernel_GetProcessPriority();
		    }
		    break;
		case 130:
		    {
			Language_Execute_globalPC += Language_codeOffsetSize;
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			Language_Execute_Kernel_InitializeCondition((TLaddressint) ((* (TLaddressint *) Language_Execute_globalPC)), (* (Language_Execute___x2341 * *) (((unsigned long) Language_Execute_globalSP + (2 * 4)) + 4)), (* (struct Language_Execute_ConditionDefinition * *) Language_Execute_globalSP), (TLnat4) ((* (TLint4 *) ((unsigned long) Language_Execute_globalSP + (2 * 4)))), (* (struct Language_Execute_MonitorDescriptor * *) ((unsigned long) Language_Execute_globalSP + 4)));
			Language_Execute_globalSP += 16;
			Language_Execute_globalPC += Language_codeAddrSize;
		    }
		    break;
		case 131:
		    {
			Language_Execute_globalPC += Language_codeOffsetSize;
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			Language_Execute_Kernel_MonitorInitialize((TLaddressint) ((* (TLaddressint *) Language_Execute_globalPC)), (* (struct Language_Execute_MonitorDescriptor * *) Language_Execute_globalSP));
			Language_Execute_globalSP += 4;
			Language_Execute_globalPC += Language_codeAddrSize;
		    }
		    break;
		case 139:
		    {
			register TLaddressint	r;
			register TLaddressint	l;
			r = (TLaddressint) ((* (TLaddressint *) ((unsigned long) Language_Execute_globalSP + 4)));
			l = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			Language_Execute_globalSP += -4;
			(* (TLint4 *) Language_Execute_globalSP) = 1;
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			{
			    register TLint4	__x2278;
			    TLint4	__x3653;
			    __x3653 = (* (Language_Offset *) Language_Execute_globalPC);
			    __x2278 = 1;
			    if (__x2278 <= __x3653) {
				for(;;) {
				    if (((* (TLchar *) r)) > ((* (TLchar *) l))) {
					(* (TLint4 *) Language_Execute_globalSP) = 0;
					break;
				    };
				    r += 1;
				    l += 1;
				    if (__x2278 == __x3653) break;
				    __x2278++;
				}
			    };
			};
			Language_Execute_globalPC += Language_codeOffsetSize;
		    }
		    break;
		case 142:
		    {
			Language_Execute_globalSP += 4;
			if (((* (TLint4 *) ((unsigned long) Language_Execute_globalSP - 4))) < 0) {
			    (* (TLint4 *) Language_Execute_globalSP) = 1;
			} else {
			    if (((* (TLint4 *) Language_Execute_globalSP)) <= ((* (TLint4 *) ((unsigned long) Language_Execute_globalSP - 4)))) {
				(* (TLint4 *) Language_Execute_globalSP) = 1;
			    } else {
				(* (TLint4 *) Language_Execute_globalSP) = 0;
			    };
			};
		    }
		    break;
		case 143:
		    {
			Language_Execute_globalSP += 4;
			if (((* (TLnat4 *) Language_Execute_globalSP)) <= ((* (TLnat4 *) ((unsigned long) Language_Execute_globalSP - 4)))) {
			    (* (TLint4 *) Language_Execute_globalSP) = 1;
			} else {
			    (* (TLint4 *) Language_Execute_globalSP) = 0;
			};
		    }
		    break;
		case 144:
		    {
			Language_Execute_globalSP += 4;
			if (((* (TLint4 *) Language_Execute_globalSP)) < 0) {
			    (* (TLint4 *) Language_Execute_globalSP) = 0;
			} else {
			    if (((* (TLint4 *) Language_Execute_globalSP)) <= ((* (TLint4 *) ((unsigned long) Language_Execute_globalSP - 4)))) {
				(* (TLint4 *) Language_Execute_globalSP) = 1;
			    } else {
				(* (TLint4 *) Language_Execute_globalSP) = 0;
			    };
			};
		    }
		    break;
		case 155:
		    {
			Language_Execute_globalSP += 4;
			(* (TLnat4 *) Language_Execute_globalSP) =  TL_TLA_TLAIMX ((TLint4) ((* (TLnat4 *) Language_Execute_globalSP)), (TLint4) ((* (TLnat4 *) ((unsigned long) Language_Execute_globalSP - 4))));
		    }
		    break;
		case 158:
		    {
			Language_Execute_globalSP += 4;
			(* (TLnat4 *) Language_Execute_globalSP) =  TL_TLA_TLAIMN((TLint4) ((* (TLnat4 *) Language_Execute_globalSP)), (TLint4) ((* (TLnat4 *) ((unsigned long) Language_Execute_globalSP - 4))));
		    }
		    break;
		case 163:
		    {
			Language_Execute_Kernel_MonitorEnter((* (struct Language_Execute_MonitorDescriptor * *) Language_Execute_globalSP));
			Language_Execute_globalSP += 4;
		    }
		    break;
		case 164:
		    {
			Language_Execute_Kernel_MonitorExit((* (struct Language_Execute_MonitorDescriptor * *) Language_Execute_globalSP));
			Language_Execute_globalSP += 4;
		    }
		    break;
		case 166:
		    {
			register TLnat4	rVal;
			register TLnat4	lVal;
			rVal = (* (TLnat4 *) Language_Execute_globalSP);
			Language_Execute_globalSP += 4;
			lVal = (* (TLnat4 *) Language_Execute_globalSP);
			(* (TLnat4 *) Language_Execute_globalSP) *= rVal;
			if ((rVal != 0) && ((((* (TLnat4 *) Language_Execute_globalSP)) / rVal) != lVal)) {
			    TLEABT((TLint4) 50);
			};
		    }
		    break;
		case 169:
		    {
			Language_Execute_globalSP += -4;
			(* (TLreal8 *) Language_Execute_globalSP) = (* (TLnat4 *) ((unsigned long) Language_Execute_globalSP + (4)));
		    }
		    break;
		case 170:
		    {
			TLreal8	temp;
			temp = (* (TLreal8 *) Language_Execute_globalSP);
			Language_Execute_globalSP += -4;
			(* (TLreal8 *) Language_Execute_globalSP) = temp;
			(* (TLreal8 *) ((unsigned long) Language_Execute_globalSP + 8)) = (* (TLnat4 *) ((unsigned long) Language_Execute_globalSP + ((2 * 8) - 4)));
		    }
		    break;
		case 171:
		    {
			register TLaddressint	tAddr;
			tAddr = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			{
			    TLstring	__x3696;
			    TL_TLS_TLSVNS((TLnat4) ((* (TLnat4 *) (((unsigned long) Language_Execute_globalSP + 4) + (2 * 4)))), (TLint4) ((* (TLint4 *) (((unsigned long) Language_Execute_globalSP + 4) + 4))), (TLint4) ((* (TLint4 *) ((unsigned long) Language_Execute_globalSP + 4))), __x3696);
			    TLSTRASS(255, (* (TLstring *) tAddr), __x3696);
			};
			Language_Execute_globalSP += 12;
			(* (TLaddressint *) Language_Execute_globalSP) = (TLaddressint) tAddr;
		    }
		    break;
		case 179:
		    {
			TLint4	elementCount;
			elementCount = (* (struct Language_ArrayDescriptor *) ((* (TLaddressint *) Language_Execute_globalSP))).elementCount;
			Language_Execute_globalSP += 0;
			(* (TLint4 *) Language_Execute_globalSP) = elementCount;
		    }
		    break;
		case 184:
		    {
			Language_Execute_Kernel_Pause((TLnat4) ((* (TLint4 *) Language_Execute_globalSP)));
			Language_Execute_globalSP += 4;
		    }
		    break;
		case 182:
		    {
			Language_Execute_globalSP += 4;
			(* (TLnat4 *) Language_Execute_globalSP) |= (* (TLnat4 *) ((unsigned long) Language_Execute_globalSP - 4));
		    }
		    break;
		case 197:
		    {
			switch ((* (TLint4 *) ((unsigned long) Language_Execute_globalSP + 4))) {
			    case -1:
				{
				    Language_Execute_RQHead->srcPos.lineNo = (* (TLnat2 *) (((unsigned long) fp + 4) + 2));
				    Language_Execute_RQHead->srcPos.fileNo = (* (TLnat2 *) ((unsigned long) fp + 4));
				    Language_Execute_RQHead->abort.quitCode = 0;
				}
				break;
			    case 0:
				{
				    Language_Execute_RQHead->abort.quitCode = 0;
				}
				break;
			    case 1:
				{
				    register struct Language_Execute_HandlerArea	*runHa;
				    runHa = Language_Execute_RQHead->runningHandler;
				    TLSTRCTASS(Language_Execute_RQHead->srcPos, runHa->srcPos, struct SrcPosition);
				    (* (TLint4 *) Language_Execute_globalSP) = runHa->quitCode;
				}
				break;
			    default:
				TLCASEABORT;
			};
			Language_Execute_DispatchHandler((TLint4) ((* (TLint4 *) Language_Execute_globalSP)));
			fp = (TLaddressint) (Language_Execute_RQHead->fp);
		    }
		    break;
		case 210:
		    {
			Language_Offset	sz;
			register TLaddressint	tgt;
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			sz = (* (Language_Offset *) Language_Execute_globalPC);
			Language_Execute_globalPC += Language_codeOffsetSize;
			tgt = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			{
			    register TLint4	__x2287;
			    TLint4	__x3711;
			    __x3711 = sz >> 1;
			    __x2287 = 2;
			    if (__x2287 <= __x3711) {
				for(;;) {
				    (* (Language_set16 *) tgt) = 0xFFFF;
				    tgt += 2;
				    if (__x2287 == __x3711) break;
				    __x2287++;
				}
			    };
			};
			TLASSERT (((unsigned long) Language_Execute_globalPC & (1)) == 0);
			(* (TLnat2 *) tgt) = (* (TLnat2 *) Language_Execute_globalPC);
			Language_Execute_globalPC += Language_codeInt2Size;
		    }
		    break;
		case 211:
		    {
			Language_Offset	sz;
			register TLaddressint	tgt;
			TLASSERT (((unsigned long) Language_Execute_globalPC & (3)) == 0);
			sz = (* (Language_Offset *) Language_Execute_globalPC);
			Language_Execute_globalPC += Language_codeOffsetSize;
			tgt = (TLaddressint) ((* (TLaddressint *) Language_Execute_globalSP));
			{
			    register TLint4	__x2290;
			    TLint4	__x3717;
			    __x3717 = sz >> 1;
			    __x2290 = 1;
			    if (__x2290 <= __x3717) {
				for(;;) {
				    (* (Language_set16 *) tgt) = 0x0;
				    tgt += 2;
				    if (__x2290 == __x3717) break;
				    __x2290++;
				}
			    };
			};
		    }
		    break;
		case 215:
		    {
			Language_Execute_Kernel_SetProcessPriority((TLnat2) ((* (TLint4 *) Language_Execute_globalSP)));
			Language_Execute_globalSP += 4;
		    }
		    break;
		case 218:
		    {
			TLnat4	value;
			value = (TLint4) ((* (TLnat4 *) ((unsigned long) Language_Execute_globalSP + 4))) << ((* (TLnat4 *) Language_Execute_globalSP));
			if ((value >> ((* (TLnat4 *) Language_Execute_globalSP))) != ((* (TLnat4 *) ((unsigned long) Language_Execute_globalSP + 4)))) {
			    TLEABT((TLint4) 50);
			};
			Language_Execute_globalSP += 4;
			(* (TLnat4 *) Language_Execute_globalSP) = (TLint4) ((* (TLnat4 *) Language_Execute_globalSP)) << ((* (TLnat4 *) ((unsigned long) Language_Execute_globalSP - 4)));
		    }
		    break;
		case 219:
		    {
			Language_Execute_globalSP += 4;
			(* (TLnat4 *) Language_Execute_globalSP) = ((* (TLnat4 *) Language_Execute_globalSP)) >> ((* (TLnat4 *) ((unsigned long) Language_Execute_globalSP - 4)));
		    }
		    break;
		case 220:
		    {
			TLASSERT (sizeof(Language_ConditionKind) == 1);
			switch ((* (Language_ConditionKind *) Language_Execute_globalPC)) {
			    case 0:
				{
				    Language_Execute_Kernel_SignalRegularCondition((* (struct Language_Execute_ConditionDescriptor * *) Language_Execute_globalSP));
				}
				break;
			    case 1:
				{
				    Language_Execute_Kernel_SignalPriorityCondition((* (struct Language_Execute_ConditionDescriptor * *) Language_Execute_globalSP));
				}
				break;
			    case 2:
				{
				    Language_Execute_Kernel_SignalDeferredCondition((* (struct Language_Execute_ConditionDescriptor * *) Language_Execute_globalSP));
				}
				break;
			    case 3:
				{
				    Language_Execute_Kernel_SignalTimeoutCondition((* (struct Language_Execute_ConditionDescriptor * *) Language_Execute_globalSP));
				}
				break;
			    default:
				TLCASEABORT;
			};
			Language_Execute_globalSP += 4;
			Language_Execute_globalPC += Language_codeInt1Size;
		    }
		    break;
		case 225:
		    {
			if (TL_TLS_TLSLEN(((* (TLstring *) ((* (TLaddressint *) Language_Execute_globalSP))))) != 1) {
			    Language_Execute_ExecutionError_Abort((TLint4) 10000, "String coerced to char is not length 1");
			};
			(* (TLint4 *) (((unsigned long) Language_Execute_globalSP + 4) - 4)) = ((TLnat4) ((* (TLchar *) ((* (TLaddressint *) Language_Execute_globalSP)))));
			Language_Execute_globalSP += 0;
		    }
		    break;
		case 227:
		    {
			register TLnat4	rVal;
			register TLint4	lVal;
			rVal = (* (TLnat4 *) Language_Execute_globalSP);
			Language_Execute_globalSP += 4;
			lVal = (* (TLint4 *) Language_Execute_globalSP);
			(* (TLint4 *) Language_Execute_globalSP) -= rVal;
			if (((* (TLint4 *) Language_Execute_globalSP)) > lVal) {
			    TLEABT((TLint4) 50);
			};
		    }
		    break;
		case 228:
		    {
			register TLnat4	rVal;
			register TLnat4	lVal;
			rVal = (* (TLnat4 *) Language_Execute_globalSP);
			Language_Execute_globalSP += 4;
			lVal = (* (TLnat4 *) Language_Execute_globalSP);
			(* (TLnat4 *) Language_Execute_globalSP) -= rVal;
			if (((* (TLnat4 *) Language_Execute_globalSP)) > lVal) {
			    TLEABT((TLint4) 36);
			};
		    }
		    break;
		case 229:
		    {
			register TLint4	rVal;
			register TLnat4	lVal;
			rVal = (* (TLint4 *) Language_Execute_globalSP);
			Language_Execute_globalSP += 4;
			lVal = (* (TLnat4 *) Language_Execute_globalSP);
			(* (TLnat4 *) Language_Execute_globalSP) -= rVal;
			if (((rVal < 0) && (((* (TLnat4 *) Language_Execute_globalSP)) < lVal)) || ((rVal > 0) && (((* (TLnat4 *) Language_Execute_globalSP)) > lVal))) {
			    TLEABT((TLint4) 50);
			};
		    }
		    break;
		case 243:
		    {
			(* (TLnat4 *) ((* (TLaddressint *) Language_Execute_globalSP))) = 0xFFFFFFFF;
			Language_Execute_globalSP += 4;
		    }
		    break;
		case 246:
		    {
			Language_Execute_RQHead->activeHandler = Language_Execute_RQHead->activeHandler->nextHandler;
		    }
		    break;
		case 248:
		    {
			TLASSERT (sizeof(Language_ConditionKind) == 1);
			switch ((* (Language_ConditionKind *) Language_Execute_globalPC)) {
			    case 0:
				{
				    Language_Execute_Kernel_WaitRegularCondition((* (struct Language_Execute_ConditionDescriptor * *) Language_Execute_globalSP));
				    Language_Execute_globalSP += 4;
				}
				break;
			    case 1:
				{
				    Language_Execute_Kernel_WaitPriorityCondition((TLnat4) ((* (TLnat4 *) Language_Execute_globalSP)), (* (struct Language_Execute_ConditionDescriptor * *) ((unsigned long) Language_Execute_globalSP + 4)));
				    Language_Execute_globalSP += 8;
				}
				break;
			    case 2:
				{
				    Language_Execute_Kernel_WaitDeferredCondition((* (struct Language_Execute_ConditionDescriptor * *) Language_Execute_globalSP));
				    Language_Execute_globalSP += 4;
				}
				break;
			    case 3:
				{
				    Language_Execute_Kernel_WaitTimeoutCondition((TLnat4) ((* (TLnat4 *) Language_Execute_globalSP)), (* (struct Language_Execute_ConditionDescriptor * *) ((unsigned long) Language_Execute_globalSP + 4)));
				    Language_Execute_globalSP += 8;
				}
				break;
			    default:
				TLCASEABORT;
			};
			Language_Execute_globalPC += Language_codeInt1Size;
		    }
		    break;
		case 252:
		    {
			(*runDesc).state = 3;
			goto __x3763;
		    }
		    break;
		default :
		    {
			Language_Execute_ExtraRoutines((Language_Opcode) op);
		    }
		    break;
	    };
	    if (Language_Execute_RQHead == ((struct Language_Execute_ProcessDescriptor *) 0)) {
		if ((unsigned long) Language_Execute_globalPC == 0) {
		    TLaddressint	stackBase;
		    stackBase = (TLaddressint) (me->stackBase);
		    TLASSERT ((unsigned long) stackBase != 0);
		    Language_Execute_Heap_Dealloc(&(stackBase));
		    Language_Execute_processCount -= 1;
		    me = (struct Language_Execute_ProcessDescriptor *) 0;
		    if (Language_debug) {
			TL_TLI_TLISS ((TLint4) 0, (TLint2) 2);
			TL_TLI_TLIPS ((TLint4) 0, "*PROCESS DEATH*", (TLint2) 0);
			TL_TLI_TLIPK ((TLint2) 0);
		    };
		} else {
		    me->pc = (TLaddressint) Language_Execute_globalPC;
		    me->sp = (TLaddressint) Language_Execute_globalSP;
		    if ((unsigned long) Language_Execute_savedInitPC == 0) {
			Language_Execute_savedInitPC = (TLaddressint) Language_Execute_globalPC;
			Language_Execute_savedInitFP = (TLaddressint) fp;
		    };
		    if (Language_debug) {
			TL_TLI_TLISS ((TLint4) 0, (TLint2) 2);
			TL_TLI_TLIPS ((TLint4) 0, "*PROCESS SWAP*", (TLint2) 0);
			TL_TLI_TLIPK ((TLint2) 0);
		    };
		};
		if (Language_Execute_EQHead == ((struct Language_Execute_ProcessDescriptor *) 0)) {
		    (*runDesc).state = 0;
		} else {
		    (*runDesc).state = 2;
		};
		break;
	    } else {
		if (Language_Execute_processCount > 0) {
		    if (Language_Execute_RQHead == me) {
			Language_Execute_quantaCount -= 1;
			if (Language_Execute_quantaCount <= 0) {
			    Language_Execute_Kernel_TimeSlice();
			};
		    };
		    if (Language_Execute_RQHead != me) {
			TLboolean	wasStepping;
			wasStepping = (me->stepSpec) != 0;
			if ((unsigned long) Language_Execute_globalPC == 0) {
			    TLaddressint	stackBase;
			    stackBase = (TLaddressint) (me->stackBase);
			    TLASSERT ((unsigned long) stackBase != 0);
			    Language_Execute_Heap_Dealloc(&(stackBase));
			    Language_Execute_processCount -= 1;
			    me = (struct Language_Execute_ProcessDescriptor *) 0;
			    if (Language_debug) {
				TL_TLI_TLISS ((TLint4) 0, (TLint2) 2);
				TL_TLI_TLIPS ((TLint4) 0, "*PROCESS DEATH*", (TLint2) 0);
				TL_TLI_TLIPK ((TLint2) 0);
			    };
			} else {
			    me->pc = (TLaddressint) Language_Execute_globalPC;
			    me->sp = (TLaddressint) Language_Execute_globalSP;
			    if ((unsigned long) Language_Execute_savedInitPC == 0) {
				Language_Execute_savedInitPC = (TLaddressint) Language_Execute_globalPC;
				Language_Execute_savedInitFP = (TLaddressint) fp;
			    };
			    if (Language_debug) {
				TL_TLI_TLISS ((TLint4) 0, (TLint2) 2);
				TL_TLI_TLIPS ((TLint4) 0, "*PROCESS SWAP*", (TLint2) 0);
				TL_TLI_TLIPK ((TLint2) 0);
			    };
			};
			me = Language_Execute_RQHead;
			Language_Execute_globalPC = (TLaddressint) (me->pc);
			Language_Execute_globalSP = (TLaddressint) (me->sp);
			fp = (TLaddressint) (me->fp);
			if ((unsigned long) Language_Execute_savedInitPC == (unsigned long) Language_Execute_globalPC) {
			    Language_Execute_savedInitPC = (TLaddressint) 0;
			    Language_Execute_savedInitFP = (TLaddressint) 0;
			};
			Language_Execute_SetActive(&(me->activeStream));
			if (Language_debug) {
			    {
				TLstring	__x3762;
				FileManager_FileName((FileManager_FileNoType) (me->srcPos.fileNo), __x3762);
				TL_TLI_TLISS ((TLint4) 0, (TLint2) 2);
				TL_TLI_TLIPS ((TLint4) 0, "line ", (TLint2) 0);
				TL_TLI_TLIPN ((TLint4) 0, (TLnat4) (me->srcPos.lineNo), (TLint2) 0);
				TL_TLI_TLIPS ((TLint4) 0, " of \"", (TLint2) 0);
				TL_TLI_TLIPS ((TLint4) 0, __x3762, (TLint2) 0);
				TL_TLI_TLIPS ((TLint4) 0, "\"", (TLint2) 0);
				TL_TLI_TLIPK ((TLint2) 0);
			    };
			};
			Language_Execute_quantaCount = 20;
			if (wasStepping) {
			    (*runDesc).state = 5;
			    break;
			};
		    };
		};
	    };
	    Language_Execute_executionTimeout -= 1;
	    if ((Language_Execute_executionTimeout <= 0) || Language_Execute_externalExecutionTimeoutFlag) {
		(*runDesc).state = 1;
		break;
	    };
	}
	__x3763:;
	(*runDesc).doQuit = 0;
	TL_TLE_TLEHX();
    }
}
