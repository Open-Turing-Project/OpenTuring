#include "cinterface"
#include <signal.h>
static struct {
    TLint4	dummy;
    char	dummy2[583];
} TLFTAB =
    { 36,
    {'T', 'L', 'E', 'Q', 'U', 'I', 'T', '.', 'c', 'h', '\0',
    '/', 'l', 'o', 'c', 'a', 'l', '/', 'i', 'n', 'c', 'l', 'u', 'd', 'e', '/', 't', 'p', 'l', 'u', 's', '/', 'S', 'U', 'N', '4', '/', 'k', 'e', 'r', 'n', 'e', 'l', 'T', 'y', 'p', 'e', 's', '\0',
    '/', 'l', 'o', 'c', 'a', 'l', '/', 'i', 'n', 'c', 'l', 'u', 'd', 'e', '/', 't', 'p', 'l', 'u', 's', '/', 'c', 'o', 'm', 'm', 'o', 'n', '/', 'h', 'a', 'n', 'd', 'l', 'e', 'r', '\0',
    '/', 'l', 'o', 'c', 'a', 'l', '/', 'i', 'n', 'c', 'l', 'u', 'd', 'e', '/', 't', 'p', 'l', 'u', 's', '/', 'S', 'U', 'N', '4', '/', 's', 'i', 'g', 'n', 'a', 'l', 'T', 'y', 'p', 'e', 's', '\0',
    '/', 'l', 'o', 'c', 'a', 'l', '/', 'i', 'n', 'c', 'l', 'u', 'd', 'e', '/', 't', 'p', 'l', 'u', 's', '/', 'c', 'o', 'm', 'm', 'o', 'n', '/', 'e', 'x', 'c', 'e', 'p', 't', 'i', 'o', 'n', 's', '\0',
    '/', 'l', 'o', 'c', 'a', 'l', '/', 'i', 'n', 'c', 'l', 'u', 'd', 'e', '/', 't', 'p', 'l', 'u', 's', '/', 'c', 'o', 'm', 'm', 'o', 'n', '/', 'l', 'i', 'm', 'i', 't', 's', '\0',
    '.', '.', '/', 'T', 'L', '.', 'c', 'h', '\0',
    '.', '.', '/', 'T', 'L', 'M', '/', 'T', 'L', 'M', '.', 'c', 'h', '\0',
    '.', '.', '/', 'T', 'L', 'B', '/', 'T', 'L', 'B', 'M', 'A', 'L', '.', 's', 't', '\0',
    '.', '.', '/', 'T', 'L', 'B', '/', 'T', 'L', 'B', '.', 'c', 'h', '\0',
    '.', '.', '/', 'T', 'L', 'B', '/', 'T', 'L', 'B', 'M', 'F', 'R', '.', 's', 't', '\0',
    '.', '.', '/', 'T', 'L', 'B', '/', 'T', 'L', 'B', 'N', 'W', 'U', '.', 'c', 'h', '\0',
    '.', '.', '/', 'T', 'L', 'B', '/', 'T', 'L', 'B', 'F', 'R', 'U', '.', 'c', 'h', '\0',
    '.', '.', '/', 'T', 'L', 'I', '/', 'T', 'L', 'I', '.', 'c', 'h', '\0',
    '.', '.', '/', 'T', 'L', 'E', '/', 'T', 'L', 'E', 'L', 'F', '.', 'c', 'h', '\0',
    '.', '.', '/', 'T', 'L', 'E', '/', 'T', 'L', 'E', '.', 'c', 'h', '\0',
    '.', '.', '/', 'T', 'L', '.', 'v', 'a', 'r', '\0',
    '.', '.', '/', 'T', 'L', 'K', '/', 'T', 'L', 'K', '.', 's', 't', '\0',
    '.', '.', '/', 'T', 'L', 'X', '/', 'T', 'L', 'X', '.', 's', 't', '\0',
    '.', '.', '/', 'T', 'L', 'A', '/', 'T', 'L', 'A', 'V', 'E', 'S', '.', 'c', 'h', '\0',
    '.', '.', '/', 'T', 'L', 'A', '/', 'T', 'L', 'A', '.', 'c', 'h', '\0',
    '.', '.', '/', 'T', 'L', 'A', '/', 'T', 'L', 'A', 'V', 'F', 'S', '.', 'c', 'h', '\0',
    '.', '.', '/', 'T', 'L', 'A', '/', 'T', 'L', 'A', 'V', 'S', '8', '.', 'c', 'h', '\0',
    '.', '.', '/', 'T', 'L', 'A', '/', 'T', 'L', 'A', 'V', 'S', 'I', '.', 'c', 'h', '\0',
    '.', '.', '/', 'T', 'L', 'A', '/', 'T', 'L', 'A', 'V', 'S', 'N', '.', 'c', 'h', '\0',
    'T', 'L', 'E', '.', 'c', 'h', '\0',
    'T', 'L', 'E', '.', 'd', 'e', 'f', '\0',
    'T', 'L', 'E', '.', 'v', 'a', 'r', '\0',
    'T', 'L', 'E', 'A', 'B', 'T', '.', 'c', 'h', '\0',
    'T', 'L', 'E', 'T', 'R', '.', 's', 't', '\0',
    'T', 'L', 'E', 'C', 'R', '.', 'c', 'h', '\0',
    'T', 'L', 'E', 'C', 'R', 'L', '.', 'c', 'h', '\0',
    'T', 'L', 'E', 'C', 'R', 'S', '.', 'c', 'h', '\0',
    'T', 'L', 'E', 'L', 'F', '.', 'c', 'h', '\0',
    'T', 'L', 'E', 'H', 'E', '.', 'c', 'h', '\0',
    'T', 'L', 'E', 'H', 'D', 'E', 'F', '.', 's', 't', '\0'
    }};
typedef	TLnat4	TL_TL_priority_t;
struct	TL_TL_ExceptionInfo {
    TLint4	quitCode;
    TLint4	libraryQuitCode;
    TLstring	errorMsg;
};
typedef	TLnat1	__x462[1];
struct	TL_TL_HandlerArea {
    TLint4	quitCode;
    TLnat4	lineAndFile;
    TLaddressint	fileTable;
    struct TL_TL_HandlerArea	*nextHandler;
    __x462	savedState;
};
typedef	TLnat4	TL_sigmask;
struct	TL_sigstack {
    TLaddressint	ss_sp;
    TLint4	ss_onstack;
};
struct	TL_sigcontext {
    TLint4	sc_onstack;
    TL_sigmask	sc_mask;
    TLint4	sc_sp;
    TLint4	sc_pc;
    TLint4	sc_ps;
};
typedef	void 	TL_sigproc();
struct	TL_sigvec_t {
    TL_sigproc	(*sv_handler);
    TL_sigmask	sv_mask;
    TLint4	sv_flags;
};
typedef	TL_sigmask	TL_TL_hardwarePriority_t;
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
TLint4	quitType;
{
    TLBINDREG((*pd), struct TL_TL_ProcessDescriptor);
    TLboolean	invokeDefault;
    TLBINDREG((*ha), struct TL_TL_HandlerArea);
    TLSTKCHKSLF(100017);
    TLSETF();
    TLSETL(100017);
    pd = &((*(TL_TLKPD)));
    TLSETL(100022);
    invokeDefault = ((*pd).handlerQhead) == ((struct TL_TL_HandlerArea *) 0);
    TLSETL(100024);
    if (invokeDefault) {
	TLSETL(100027);
	{
	    TLaddressint	__x465;
	    __x465 = (TLaddressint) ((unsigned long)&(TL_TLE_TLEDEFHA));
	    if (((*pd).currentHandler) == ((* (struct TL_TL_HandlerArea * *) &__x465))) {
		TLSETL(100031);
		TL_TLK_TLKUEXIT((TL_Cint) -1);
	    } else {
		TLSETL(100039);
		TL_TLE_TLETR_TLETRR();
		{
		    TLaddressint	__x467;
		    __x467 = (TLaddressint) ((unsigned long)&(TL_TLE_TLEDEFHA));
		    (*pd).handlerQhead = (* (struct TL_TL_HandlerArea * *) &__x467);
		};
		TLINCL();
		TL_TLE_TLEDEFHA.nextHandler = (struct TL_TL_HandlerArea *) 0;
	    };
	};
    };
    TLSETL(100044);
    ha = &((*((*pd).handlerQhead)));
    TLSETL(100046);
    (*ha).quitCode = quitCode;
    TLSETL(100048);
    switch (quitType) {
	case -1:
	    {
		TLSETL(100050);
		if ((unsigned long) callerPlace != 0) {
		    TLSETL(100051);
		    (*ha).lineAndFile = (* (struct TL_TLE_Place *) callerPlace).lineAndFile;
		    TLINCL();
		    (*ha).fileTable = (TLaddressint) ((* (struct TL_TLE_Place *) callerPlace).fileTable);
		} else {
		    TLSETL(100056);
		    (*ha).lineAndFile = (*pd).lineAndFile;
		    TLINCL();
		    (*ha).fileTable = (TLaddressint) ((*pd).fileTable);
		};
		TLSETL(100059);
		(*pd).exception.libraryQuitCode = (*pd).exception.quitCode;
		TLINCL();
		(*pd).exception.quitCode = 0;
	    }
	    break;
	case 0:
	    {
		TLSETL(100063);
		(*ha).lineAndFile = (*pd).lineAndFile;
		TLINCL();
		(*ha).fileTable = (TLaddressint) ((*pd).fileTable);
		TLINCL();
		(*pd).exception.libraryQuitCode = (*pd).exception.quitCode;
		TLINCL();
		(*pd).exception.quitCode = 0;
	    }
	    break;
	case 1:
	    {
		TLBINDREG((*oldHa), struct TL_TL_HandlerArea);
		TLSETL(100070);
		oldHa = &((*((*pd).currentHandler)));
		TLINCL();
		(*ha).lineAndFile = (*oldHa).lineAndFile;
		TLINCL();
		(*ha).fileTable = (TLaddressint) ((*oldHa).fileTable);
	    }
	    break;
	default:
	    TLCASEABORT;
    };
    TLSETL(100083);
    (*pd).currentHandler = (*pd).handlerQhead;
    TLINCL();
    (*pd).handlerQhead = (*ha).nextHandler;
    TLSETL(100086);

    if (invokeDefault) {
	TLSETL(100089);
	TL_TLE_TLEH();
    } else {
	TLSETL(100092);
	TL_TLE_TLEDH();
    };
    TLRESTORELF();
}
