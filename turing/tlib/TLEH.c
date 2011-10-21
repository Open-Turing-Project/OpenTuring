#include "include\cinterface"
typedef	TLnat4	TL_TL_priority_t;
struct	TL_TL_ExceptionInfo {
    TLint4	quitCode;
    TLint4	libraryQuitCode;
    TLstring	errorMsg;
};
typedef	TLnat1	__x363[1];
struct	TL_TL_HandlerArea {
    TLint4	quitCode;
    TLnat4	lineAndFile;
    TLaddressint	fileTable;
    struct TL_TL_HandlerArea	*nextHandler;
    __x363	savedState;
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

extern void TL_TLX_TLXPID ();

extern void TL_TLX_TLXTIM ();

extern void TL_TLX_TLXATF ();

extern void TL_TLX_TLXDT ();

extern void TL_TLX_TLXTM ();

extern void TL_TLX_TLXCL ();

extern void TL_TLX_TLXSC ();

extern void TL_TLX_TLXGE ();

extern void TL_TLX_TLXSYS ();

extern void TL_TLA_TLAVES ();

extern void TL_TLA_TLAVFS ();

extern void TL_TLA_TLAVS8 ();

extern void TL_TLA_TLAVSI ();

extern void TL_TLA_TLAVSN ();
struct	TL_TLE_Place {
    TLnat4	lineAndFile;
    TLaddressint	fileTable;
};
extern struct TL_TL_HandlerArea	TL_TLE_TLEDEFHA;

extern void TL_TLE_TLEABT ();

extern void TL_TLE_TLETR_TLETRR ();

extern void TL_TLE_TLEDH ();

extern void TL_TLE_TLELF ();
void TL_TLE_TLEH () {
    TLBINDREG((*ha), struct TL_TL_HandlerArea);
    TLnat4	lineNumber;
    TLaddressint	fileName;
    TL_TLI_TLIFS();
    ha = &((*(TL_TLKPD->currentHandler)));
    TL_TLE_TLELF((TLnat4) ((*ha).lineAndFile), (TLaddressint) ((*ha).fileTable), &(lineNumber), &(fileName));
    if ((unsigned long) fileName != 0) {
	{
	    TLstring	__x366;
	    TL_TLS_TLSVIS((TLint4) lineNumber, (TLint4) 1, (TLint4) 10, __x366);
	    TL_TLI_TLISS ((TLint4) 0, (TLint2) 2);
	    TL_TLI_TLIPK ((TLint2) 0);
	    TL_TLI_TLIPS ((TLint4) 0, "Line ", (TLint2) 0);
	    TL_TLI_TLIPS ((TLint4) 0, __x366, (TLint2) 0);
	    TL_TLI_TLIPS ((TLint4) 0, " of ", (TLint2) 0);
	    TL_TLI_TLIPS ((TLint4) 0, ((* (TLstring *) fileName)), (TLint2) 0);
	    TL_TLI_TLIPS ((TLint4) 0, ": ", (TLint2) 0);
	};
    } else {
	TL_TLI_TLISS ((TLint4) 0, (TLint2) 2);
	TL_TLI_TLIPS ((TLint4) 0, "", (TLint2) 0);
	TL_TLI_TLIPK ((TLint2) 0);
    };
    if (TL_TLK_TLKFRKED()) {
	TL_TLI_TLISS ((TLint4) 0, (TLint2) 2);
	TL_TLI_TLIPS ((TLint4) 0, "Process \"", (TLint2) 0);
	TL_TLI_TLIPS ((TLint4) 0, ((* (TLstring *) (TL_TLKPD->name))), (TLint2) 0);
	TL_TLI_TLIPS ((TLint4) 0, "\": ", (TLint2) 0);
    };
    if (((TL_TLKPD->exception.libraryQuitCode) != 0) && (((*ha).quitCode) == (TL_TLKPD->exception.libraryQuitCode))) {
	TL_TLI_TLISS ((TLint4) 0, (TLint2) 2);
	TL_TLI_TLIPS ((TLint4) 0, (TL_TLKPD->exception.errorMsg), (TLint2) 0);
    } else {
	{
	    TLstring	__x369;
	    TL_TLS_TLSVIS((TLint4) ((*ha).quitCode), (TLint4) 1, (TLint4) 10, __x369);
	    TL_TLI_TLISS ((TLint4) 0, (TLint2) 2);
	    TL_TLI_TLIPS ((TLint4) 0, "Quit #", (TLint2) 0);
	    TL_TLI_TLIPS ((TLint4) 0, __x369, (TLint2) 0);
	};
    };
    TL_TLI_TLISS ((TLint4) 0, (TLint2) 2);
    TL_TLI_TLIPS ((TLint4) 0, ".", (TLint2) 0);
    TL_TLI_TLIPK ((TLint2) 0);
    TL_TLI_TLIFS();
    TL_TLK_TLKUEXIT((TL_Cint) ((*ha).quitCode));
}
