#include "include\cinterface"
typedef	TLnat4	TL_TL_priority_t;
struct	TL_TL_ExceptionInfo {
    TLint4	quitCode;
    TLint4	libraryQuitCode;
    TLstring	errorMsg;
};
typedef	TLnat1	__x280[1];
struct	TL_TL_HandlerArea {
    TLint4	quitCode;
    TLnat4	lineAndFile;
    TLaddressint	fileTable;
    struct TL_TL_HandlerArea	*nextHandler;
    __x280	savedState;
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

void TL_TLK_TLKINI ();

void TL_TLK_TLKFINI ();

void TL_TLK_TLKUXRS ();

void TL_TLK_TLKUXRE ();

void TL_TLK_TLKUEXIT ();

void TL_TLK_TLKUDMPP ();

static void TL_TLK_TLKRLOOP ();

static void TL_TLK_TLKRROUT ();

static void TL_TLK_TLKRRSCH ();

void TL_TLK_TLKPFORK ();

TLboolean TL_TLK_TLKFRKED ();

void TL_TLK_TLKLKON ();

void TL_TLK_TLKLKOFF ();

void TL_TLK_TLKSSYNC ();

void TL_TLK_TLKSWAKE ();

void TL_TLK_TLKSTIMO ();

void TL_TLK_TLKCINI ();

TLnat4 TL_TLK_TLKPGETP ();

void TL_TLK_TLKPSETP ();

void TL_TLK_TLKIPINI ();

void TL_TLK_TLKIPENT ();

void TL_TLK_TLKIPEXT ();

TLboolean TL_TLK_TLKDMINI ();

void TL_TLK_TLKDMENT ();

void TL_TLK_TLKDMEXT ();

void TL_TLK_TLKPPAUS ();

static void TL_TLK_TLKPDINI (pd)
struct TL_TL_ProcessDescriptor	*pd;
{
    pd->lineAndFile = 0;
    pd->fileTable = (TLaddressint) 0;
    pd->stackLimit = (TLaddressint) 0;
    pd->stackPointer = (TLaddressint) 0;
    pd->handlerQhead = (struct TL_TL_HandlerArea *) 0;
    pd->currentHandler = (struct TL_TL_HandlerArea *) 0;
    pd->name = (TLaddressint) 0;
    pd->exception.quitCode = 0;
    pd->exception.libraryQuitCode = 0;
    TLSTRASS(255, pd->exception.errorMsg, "");
    pd->waitParameter = 0;
    pd->monitorQlink = (struct TL_TL_ProcessDescriptor *) 0;
    pd->timeOutStatus = 0;
    pd->pid = 0;
    pd->memoryBase = (TLaddressint) 0;
    pd->timeoutTime = 0;
    pd->timeoutEpoch = 0;
    pd->timeoutQ.flink = (struct TL_TL_ProcessDescriptor *) 0;
    pd->timeoutQ.blink = (struct TL_TL_ProcessDescriptor *) 0;
    pd->timedOut = 0;
    pd->pausing = 0;
    pd->dispatchPriority = 1000;
    pd->runQlink = (struct TL_TL_ProcessDescriptor *) 0;
    pd->ready = 0;
    pd->tsyncWaiter = (struct TL_TL_ProcessDescriptor *) 0;
    pd->quantum = 0;
    pd->quantumCntr = 0;
    pd->devmonLevel = 0;
    pd->otherInfo = (TLaddressint) 0;
}

void TL_TLK_TLKINI () {
}

void TL_TLK_TLKFINI () {
    TL_TLK_TLKUEXIT((TL_Cint) 0);
}

void TL_TLK_TLKUXRS () {
}

void TL_TLK_TLKUXRE () {
}

void TL_TLK_TLKUEXIT (exitCode)
TL_Cint	exitCode;
{

    extern void exit ();
    TL_TLI_TLIFS();
    TL_TLI_TLIFINI();
    exit((TL_Cint) exitCode);
}

void TL_TLK_TLKUDMPP (pd)
struct TL_TL_ProcessDescriptor	*pd;
{
}

static void TL_TLK_TLKRLOOP () {
}

static void TL_TLK_TLKRROUT () {
}

static void TL_TLK_TLKRRSCH () {
}

void TL_TLK_TLKPFORK (paramSize, name, procAddress, forkAddress, stackSize, forkStatus, p1, p2, p3, p4, p5, p6, p7, p8, p9)
TLnat4	paramSize;
TLaddressint	name;
TLaddressint	procAddress;
TLaddressint	forkAddress;
TLnat4	stackSize;
TLaddressint	forkStatus;
TLint4	p1;
TLint4	p2;
TLint4	p3;
TLint4	p4;
TLint4	p5;
TLint4	p6;
TLint4	p7;
TLint4	p8;
TLint4	p9;
{
    if ((unsigned long) forkStatus != 0) {
	(* (TLboolean *) forkStatus) = 0;
	return;
    } else {
	{
	    TLstring	__x284;
	    TL_TLS_TLSCAT("Cannot fork ", (* (TLstring *) name), __x284);
	    TLSTRASS(255, TL_TLKPD->exception.errorMsg, __x284);
	};
	TL_TLKPD->exception.quitCode = 105;
	TL_TLE_TLEQUIT ((TLint4) 105, (char *) 0, 0);
    };
}

TLboolean TL_TLK_TLKFRKED () {
    return (0);
    /* NOTREACHED */
}

void TL_TLK_TLKLKON (lck, x)
TL_TL_lock_t	*lck;
TL_TL_lockStatus_t	*x;
{
}

void TL_TLK_TLKLKOFF (lck, x)
TL_TL_lock_t	*lck;
TL_TL_lockStatus_t	x;
{
}

void TL_TLK_TLKSSYNC (pd)
struct TL_TL_ProcessDescriptor	*pd;
{
}

void TL_TLK_TLKSWAKE (pd)
struct TL_TL_ProcessDescriptor	*pd;
{
}

void TL_TLK_TLKSTIMO (interval, timedOut)
TLnat4	interval;
TLboolean	*timedOut;
{
}

void TL_TLK_TLKCINI () {
}

TLnat4 TL_TLK_TLKPGETP () {
    return (0);
    /* NOTREACHED */
}

void TL_TLK_TLKPSETP (prio)
TLnat4	prio;
{
}

void TL_TLK_TLKIPINI (sig, sighandler, level)
TLnat4	sig;
TLaddressint	sighandler;
TLint4	level;
{
}

void TL_TLK_TLKIPENT () {
}

void TL_TLK_TLKIPEXT (info)
TLaddressint	info;
{
}

TLboolean TL_TLK_TLKDMINI (level)
TLint4	level;
{
    return (1);
    /* NOTREACHED */
}

void TL_TLK_TLKDMENT (level)
TLint4	level;
{
}

void TL_TLK_TLKDMEXT () {
}

void TL_TLK_TLKPPAUS (pauseTime)
TLnat4	pauseTime;
{
}
static struct TL_TL_ProcessDescriptor	TL_TLK_initPD;
typedef	TLchar	TL_TLK___x288[13];
static TL_TLK___x288	TL_TLK_name;

void TL_TLK () {
    {
	TLaddressint	__x286;
	__x286 = (TLaddressint) ((unsigned long)&(TL_TLK_initPD));
	TL_TLKPD = (* (struct TL_TL_ProcessDescriptor * *) &__x286);
    };
    TLSTRASS(12, TL_TLK_name, "Main Program");
    TL_TLK_TLKPDINI(TL_TLKPD);
    TL_TLKPD->name = (TLaddressint) ((unsigned long)TL_TLK_name);
    TL_TLKPD->stackLimit = (TLaddressint) 0;
}
