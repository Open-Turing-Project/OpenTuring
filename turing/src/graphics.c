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
typedef	TLchar	__x1832[256];
struct	ErrMsgDef {
    struct ErrMsgDef	*next;
    struct SrcPosition	srcPos;
    __x1832	text;
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
typedef	TLstring	__x1833[20];

extern void Language_EnterPreprocSymbols ();

extern void Language_CompileUnit ();

extern void Language_CompileProgram ();

extern void Language_WriteObjectFile ();
typedef	TLstring	__x1836[21];
typedef	TLstring	__x1840[21];

extern void Language_SetupExecution ();
typedef	TLint1	Language_Step;

extern void Language_SetStep ();
typedef	TLint1	Language_ReturnState;
struct	Language_RunStatus {
    Language_ReturnState	state;
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
typedef	TLchar	Language___x1844[22];
typedef	Language___x1844	Language___x1843[255];
extern Language___x1843	Language_OpcodeName;
struct	Language_CodeHeader {
    TLaddressint	bodyCode;
};
struct	Language___x1846 {
    TLint4	lower, range;
};
typedef	struct Language___x1846	Language___x1845[255];
struct	Language_ArrayDescriptor {
    TLnat4	sizeArray;
    TLnat4	sizeElement;
    TLnat4	elementCount;
    TLnat4	numDimensions;
    Language___x1845	dim;
};
struct	Language_ForDescriptor {
    TLint4	counter;
    TLint4	right;
    TLint4	step;
    TLaddressint	savedSP;
};
typedef	Language_Offset	Language___x1847[1001];
struct	Language_CaseDescriptor {
    TLint4	lower, upper;
    Language_Offset	otherwise;
    Language___x1847	caseVector;
};
struct	Language_PointerDescriptor {
    TLaddressint	dataPtr;
    TLint4	timestamp;
};
typedef	TLnat2	Language___x1848[301];
struct	Language_UnionMapDescriptor {
    TLint4	lower, upper;
    Language___x1848	map;
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
typedef	TLstring	__x1851[20];

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
typedef	TLstring	__x1854[21];

extern void Language_Execute_Initialize ();

extern TLint4 Language_Execute_GetStackAllocated ();

extern void Language_Execute_Executor ();

extern void Language_Execute_EndExecution ();

extern void Language_Execute_RecoverAllMemory ();

extern void Language_Execute_SetStep ();

extern void Language_Execute_DumpThreadList ();

extern void Language_Execute_UnitInitState ();
extern TLint2	Language_Execute_executionTimeout;
extern TLint4	Language_Execute_stackAllocated;
extern TLboolean	Language_Execute_allWaitingForKeyboard;

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
typedef	TLnat1	Language_Execute_ExecutionError___x1857[1];
struct	Language_Execute_ExecutionError_TL_HandlerArea {
    TLint4	quitCode;
    TLnat4	lineAndFile;
    TLaddressint	fileTable;
    struct Language_Execute_ExecutionError_TL_HandlerArea	*nextHandler;
    Language_Execute_ExecutionError___x1857	savedState;
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
typedef	struct Language_Execute_ExecutionError_TL_ConditionDescriptor	Language_Execute_ExecutionError___x1860[65535];
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
typedef	TLchar	__x1861[4];

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
extern TLnat2	Language_Execute_numArguments;
typedef	TLint1	Language_Execute___x1862[23];
extern Language_Execute___x1862	Language_Execute_fileMap;
typedef	Language_StringDef	*Language_Execute___x1865[21];
extern Language_Execute___x1865	Language_Execute_argumentNames;
extern TLboolean	TL_TLI_TLIUXS;
extern TLint4	TL_TLI_TLIXSN;

void Language_Execute_AddFile ();

void Language_Execute_CloseFile ();

void Language_Execute_SetActive ();
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
typedef	struct Language_Execute_ConditionDescriptor	Language_Execute___x1880[65535];
struct	Language_Execute_ConditionDefinition {
    TLnat2	cdArraySize;
    Language_Execute___x1880	*cdArray;
    TLaddressint	name;
    struct Language_Execute_ConditionDefinition	*nextVariable;
};
extern struct Language_Execute_ProcessDescriptor	*Language_Execute_RQHead;
extern struct Language_Execute_ProcessDescriptor	*Language_Execute_EQHead;
extern TLnat4	Language_Execute_Chrono;
extern TLint4	Language_Execute_processCount;
extern TLaddressint	Language_Execute_globalPC;
extern TLaddressint	Language_Execute_globalSP;
typedef	Language_Opcode	Language_Execute___x1881[1];
extern Language_Execute___x1881	Language_Execute_nullRoutine;
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

void Language_Execute_RunSrcPosition ();

void Language_Execute_SetErrno ();

void Language_Execute_GetErrno ();

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

void Language_Execute_Graphics_Drawpic ();

void Language_Execute_Graphics_Getch ();

void Language_Execute_Graphics_Hasch ();

static void Language_Execute_Graphics_Palette ();

void Language_Execute_Graphics_Sizepic ();

void Language_Execute_Graphics_Takepic ();

static void Language_Execute_Graphics_Whatpalette ();

void Language_Execute_Graphics_Drawpic (sp)
TLaddressint	sp;
{
    TLSTACKCHECK();
    MIO_DrawPic((TLint4) ((* (TLint4 *) sp)), (TLint4) ((* (TLint4 *) ((unsigned long) sp + 4))), (TLaddressint) ((* (TLaddressint *) ((unsigned long) sp + (2 * 4)))), (TLint4) ((* (TLint4 *) (((unsigned long) sp + (3 * 4)) + 4))));
    Language_Execute_executionTimeout = 0;
}

void Language_Execute_Graphics_Getch (sp)
TLaddressint	sp;
{
    TLint4	stream;
    struct MIO_EventDescriptor	event;
    TLSTACKCHECK();
    stream = Language_Execute_RQHead->activeStream;
    if (stream == 0) {
	stream = -2;
    };
    event.mode = 3;
    event.count = 1;
    if (Language_Execute_HasEvent((TLint4) stream, &(event))) {
	(* (TLint2 *) ((* (TLaddressint *) sp))) = 0;
	(* (TLchar *) ((* (TLaddressint *) sp))) = MIO_Getch();
    } else {
	Language_Execute_EventQueueInsert((TLint4) stream, &(event));
    };
}

void Language_Execute_Graphics_Hasch (sp)
TLaddressint	sp;
{
    TLSTACKCHECK();
    (* (TLboolean *) ((* (TLaddressint *) sp))) = MIO_Hasch();
}

void Language_Execute_Graphics_Sizepic (sp)
TLaddressint	sp;
{
    TLnat4	picBytes;
    TLSTACKCHECK();
    picBytes = MIO_SizePic((TLint4) ((* (TLint4 *) ((unsigned long) sp + 4))), (TLint4) ((* (TLint4 *) (((unsigned long) sp + 4) + 4))), (TLint4) ((* (TLint4 *) (((unsigned long) sp + 4) + (2 * 4)))), (TLint4) ((* (TLint4 *) (((unsigned long) sp + 4) + (3 * 4)))));
    (* (TLint4 *) ((* (TLaddressint *) sp))) = (picBytes + 3) >> 2;
    Language_Execute_executionTimeout -= 500;
}

void Language_Execute_Graphics_Takepic (sp)
TLaddressint	sp;
{
    TLint4	x1;
    TLint4	y1;
    TLint4	x2;
    TLint4	y2;
    TLint4	dummy;
    TLSTACKCHECK();
    x1 = (* (TLint4 *) sp);
    y1 = (* (TLint4 *) ((unsigned long) sp + 4));
    x2 = (* (TLint4 *) ((unsigned long) sp + (2 * 4)));
    y2 = (* (TLint4 *) ((unsigned long) sp + (3 * 4)));
    if (((* (struct Language_ArrayDescriptor *) ((* (TLaddressint *) (((unsigned long) sp + (4 * 4)) + 4)))).sizeArray) < MIO_SizePic((TLint4) x1, (TLint4) y1, (TLint4) x2, (TLint4) y2)) {
	Language_Execute_ExecutionError_Abort((TLint4) 10000, "Array passed to \'takepic\' is too small to hold picture");
    };
    MIO_TakePic((TLint4) x1, (TLint4) y1, (TLint4) x2, (TLint4) y2, (TLaddressint) ((* (TLaddressint *) ((unsigned long) sp + (4 * 4)))), (TLaddressint) ((unsigned long)&(dummy)));
    Language_Execute_executionTimeout = 0;
}

void Language_Execute_Graphics () {
}
