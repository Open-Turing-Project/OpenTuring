#include "include\cinterface"
typedef	TLnat4	TL_TL_priority_t;
struct	TL_TL_ExceptionInfo {
    TLint4	quitCode;
    TLint4	libraryQuitCode;
    TLstring	errorMsg;
};
typedef	TLnat1	__x374[1];
struct	TL_TL_HandlerArea {
    TLint4	quitCode;
    TLnat4	lineAndFile;
    TLaddressint	fileTable;
    struct TL_TL_HandlerArea	*nextHandler;
    __x374	savedState;
};
typedef	TLnat4	TL_TL_hardwarePriority_t;
typedef	TL_TL_hardwarePriority_t	TL_TL_lockStatus_t;
typedef	TLnat4	TL_TL_lock_t;
struct	TL_TL_link_t {
    struct TL_TL_ProcessDescriptor	*flink;
    struct TL_TL_ProcessDescriptor	*blink;
};
struct	TL_TL_ProcessDescriptor {
    TLnat4	lineAndFile;
    TLaddressint	fileTable;
    TLaddressint	stackLimit;
    TLaddressint	stackPointer;
    struct TL_TL_HandlerArea	*handlerQhead;
    struct TL_TL_HandlerArea	*currentHandler;
    TLaddressint	name;
    struct TL_TL_ExceptionInfo	exception;
    TLnat4	waitParameter;
    struct TL_TL_ProcessDescriptor	*monitorQlink;
    TLnat4	timeOutStatus;
    TLnat4	pid;
    TLaddressint	memoryBase;
    TLnat4	timeoutTime;
    TLnat4	timeoutEpoch;
    struct TL_TL_link_t	timeoutQ;
    TLboolean	timedOut;
    TLboolean	pausing;
    TL_TL_priority_t	dispatchPriority;
    struct TL_TL_ProcessDescriptor	*runQlink;
    TLboolean	ready;
    struct TL_TL_ProcessDescriptor	*tsyncWaiter;
    TLnat4	quantum;
    TLnat4	quantumCntr;
    TLnat4	devmonLevel;
    TLaddressint	otherInfo;
};

extern void TL_TLM_TLMUDUMP ();

extern void TL_TLB_TLBMAL ();

extern void TL_TLB_TLBMFR ();

extern void TL_TLB_TLBNWU ();

extern void TL_TLB_TLBFRU ();

extern void TL_TLI_TLIFS ();

extern void TL_TLI_TLIAON ();

extern void TL_TLI_TLIAOFF ();

extern void TL_TLI_TLIUDUMP ();

extern void TL_TLI_TLIFINI ();

extern void TL_TLE_TLELF ();
typedef	TLint4	TL_Cint;
extern TLboolean	TL_TLECU;
extern struct TL_TL_ProcessDescriptor	*TL_TLKPD;

void TL_TLQUIT ();

extern void TL_TLK_TLKINI ();

extern void TL_TLK_TLKFINI ();

extern void TL_TLK_TLKUXRS ();

extern void TL_TLK_TLKUXRE ();

extern void TL_TLK_TLKUEXIT ();

extern void TL_TLK_TLKUDMPP ();

extern void TL_TLK_TLKPFORK ();

extern TLboolean TL_TLK_TLKFRKED ();

extern void TL_TLK_TLKLKON ();

extern void TL_TLK_TLKLKOFF ();

extern void TL_TLK_TLKSSYNC ();

extern void TL_TLK_TLKSWAKE ();

extern void TL_TLK_TLKSTIMO ();

extern void TL_TLK_TLKCINI ();

extern TLnat4 TL_TLK_TLKPGETP ();

extern void TL_TLK_TLKPSETP ();

extern void TL_TLK_TLKIPINI ();

extern void TL_TLK_TLKIPENT ();

extern void TL_TLK_TLKIPEXT ();

extern TLboolean TL_TLK_TLKDMINI ();

extern void TL_TLK_TLKDMENT ();

extern void TL_TLK_TLKDMEXT ();

extern void TL_TLK_TLKPPAUS ();
struct	TL_TLM_TL_queue_t {
    struct TL_TL_ProcessDescriptor	*head;
    struct TL_TL_ProcessDescriptor	*tail;
};
struct	TL_TLM_TL_MonitorDescriptor {
    TL_TL_lock_t	mQLock;
    TLint4	entryParameter;
    TLint4	monitorPriority;
    TLboolean	deviceMonitor;
    struct TL_TLM_TL_queue_t	entryQ;
    struct TL_TLM_TL_queue_t	reEntryQ;
    TLaddressint	name;
    struct TL_TLM_TL_ConditionDescriptor	*firstCondition;
    struct TL_TLM_TL_MonitorDescriptor	*nextMonitor;
    struct TL_TLM_TL_MonitorDescriptor	*prevMonitor;
    TLaddressint	otherInfo;
};
struct	TL_TLM_TL_ConditionDescriptor {
    struct TL_TLM_TL_queue_t	signalQ;
    struct TL_TLM_TL_MonitorDescriptor	*md;
    TLaddressint	name;
    TLnat4	index;
    struct TL_TLM_TL_ConditionDescriptor	*nextCondition;
    struct TL_TLM_TL_ConditionDescriptor	*prevCondition;
    TLaddressint	otherInfo;
};
typedef	struct TL_TLM_TL_ConditionDescriptor	TL_TLM___x377[65535];
typedef	TLint4	TL_TLM_Signalled;
typedef	TLint4	TL_TLM_OnEntryQ;
extern struct TL_TLM_TL_MonitorDescriptor	*TL_TLM_TLMMLH;
extern TL_TL_lock_t	TL_TLM_TLMMLL;

extern void TL_TLM_TLMUDUMP ();

extern void TL_TLM_TLMGNEP ();

extern void TL_TLM_TLMRINI ();

extern void TL_TLM_TLMDCTWT ();

extern void TL_TLM_TLMCRWT ();

extern void TL_TLM_TLMCRINI ();
void TL_TLM_TLMCTINI (name, cdArray, cdArraySize, md)
TLaddressint	name;
TL_TLM___x377	*cdArray;
TLnat4	cdArraySize;
struct TL_TLM_TL_MonitorDescriptor	*md;
{
    TL_TLM_TLMCRINI((TLaddressint) name, cdArray, (TLnat4) cdArraySize, md);
}
