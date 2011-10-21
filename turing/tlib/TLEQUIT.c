#include "include\cinterface"
typedef	TLnat4	TL_TL_priority_t;
struct	TL_TL_ExceptionInfo {
    TLint4	quitCode;
    TLint4	libraryQuitCode;
    TLstring	errorMsg;
};
typedef	TLnat1	__x373[1];
struct	TL_TL_HandlerArea {
    TLint4	quitCode;
    TLnat4	lineAndFile;
    TLaddressint	fileTable;
    struct TL_TL_HandlerArea	*nextHandler;
    __x373	savedState;
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

extern void TL_TLE_TLEH ();

extern void TL_TLE_TLEHE ();
void TL_TLE_TLEQUIT (quitCode, callerPlace, quitType)
TLint4	quitCode;
TLaddressint	callerPlace;
TLint1	quitType;
{
    TLBINDREG((*pd), struct TL_TL_ProcessDescriptor);
    TLboolean	invokeDefault;
    TLBINDREG((*ha), struct TL_TL_HandlerArea);
    pd = &((*(TL_TLKPD)));
    invokeDefault = ((*pd).handlerQhead) == ((struct TL_TL_HandlerArea *) 0);
    if (invokeDefault) {
	{
	    TLaddressint	__x376;
	    __x376 = (TLaddressint) ((unsigned long)&(TL_TLE_TLEDEFHA));
	    if (((*pd).currentHandler) == ((* (struct TL_TL_HandlerArea * *) &__x376))) {
		TL_TLK_TLKUEXIT((TL_Cint) -1);
	    } else {
		TL_TLE_TLETR_TLETRR();
		{
		    TLaddressint	__x378;
		    __x378 = (TLaddressint) ((unsigned long)&(TL_TLE_TLEDEFHA));
		    (*pd).handlerQhead = (* (struct TL_TL_HandlerArea * *) &__x378);
		};
		TL_TLE_TLEDEFHA.nextHandler = (struct TL_TL_HandlerArea *) 0;
	    };
	};
    };
    ha = &((*((*pd).handlerQhead)));
    (*ha).quitCode = quitCode;
    switch (quitType) {
	case -1:
	    {
		if ((unsigned long) callerPlace != 0) {
		    (*ha).lineAndFile = (* (struct TL_TLE_Place *) callerPlace).lineAndFile;
		    (*ha).fileTable = (TLaddressint) ((* (struct TL_TLE_Place *) callerPlace).fileTable);
		} else {
		    (*ha).lineAndFile = (*pd).lineAndFile;
		    (*ha).fileTable = (TLaddressint) ((*pd).fileTable);
		};
		(*pd).exception.libraryQuitCode = (*pd).exception.quitCode;
		(*pd).exception.quitCode = 0;
	    }
	    break;
	case 0:
	    {
		(*ha).lineAndFile = (*pd).lineAndFile;
		(*ha).fileTable = (TLaddressint) ((*pd).fileTable);
		(*pd).exception.libraryQuitCode = (*pd).exception.quitCode;
		(*pd).exception.quitCode = 0;
	    }
	    break;
	case 1:
	    {
		TLBINDREG((*oldHa), struct TL_TL_HandlerArea);
		oldHa = &((*((*pd).currentHandler)));
		(*ha).lineAndFile = (*oldHa).lineAndFile;
		(*ha).fileTable = (TLaddressint) ((*oldHa).fileTable);
	    }
	    break;
	default :
	    {
		TL_TLI_TLISSO ();
		TL_TLI_TLIPS ((TLint4) 0, "TLEQUIT: INTERNAL LIBRARY ERROR. Unknown quitType!", (TLint2) -1);
		TL_TLI_TLIPK ((TLint2) -1);
		TL_TLK_TLKUEXIT((TL_Cint) -1);
	    }
	    break;
    };
    (*pd).currentHandler = (*pd).handlerQhead;
    (*pd).handlerQhead = (*ha).nextHandler;
    if (invokeDefault) {
	TL_TLE_TLEH();
    } else {
	TL_TLE_TLEDH();
    };
}
