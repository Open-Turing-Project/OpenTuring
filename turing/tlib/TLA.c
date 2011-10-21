#include "include\cinterface"
typedef	TLnat4	TL_TL_priority_t;
struct	TL_TL_ExceptionInfo {
    TLint4	quitCode;
    TLint4	libraryQuitCode;
    TLstring	errorMsg;
};
typedef	TLnat1	__x601[1];
struct	TL_TL_HandlerArea {
    TLint4	quitCode;
    TLnat4	lineAndFile;
    TLaddressint	fileTable;
    struct TL_TL_HandlerArea	*nextHandler;
    __x601	savedState;
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
typedef	TLint4	TL_TLA_Real4SignBit;
typedef	TLint4	TL_TLA_Real4ExponentBits;
typedef	TLint4	TL_TLA_Real4MantissaBits;
typedef	TLint4	TL_TLA_Real8SignBit;
typedef	TLint4	TL_TLA_Real8ExponentBits;
typedef	TLint4	TL_TLA_Real8MantissaBits;
typedef	TLnat4	TL_TLA___x604[11];
TL_TLA___x604	TL_TLA_TLARS;
TLint4	TL_TLA_TLA8HI;
TLint4	TL_TLA_TLA8LO;

extern TLnat4 TL_TLA_TLA4AD ();

extern TLint4 TL_TLA_TLA4CM ();

extern TLnat4 TL_TLA_TLA4DN ();

extern TLint4 TL_TLA_TLA4DV ();

extern TLnat4 TL_TLA_TLA4ML ();

extern TLnat4 TL_TLA_TLA4MN ();

extern TLnat4 TL_TLA_TLA4MX ();

extern void TL_TLA_TLAV8D ();

extern void TL_TLA_TLAVD8 ();

extern TLint4 TL_TLA_TLA8GX ();

extern TLreal8 TL_TLA_TLA8SX ();

extern TLreal8 TL_TLA_TLA8T ();

extern TLreal8 TL_TLA_TLA8AD ();

extern TLreal8 TL_TLA_TLA8CD ();

extern TLint4 TL_TLA_TLA8CL ();

extern TLint4 TL_TLA_TLA8CM ();

extern TLreal8 TL_TLA_TLA8CR ();

extern TLreal8 TL_TLA_TLA8DN ();

extern TLint4 TL_TLA_TLA8DV ();

extern TLint4 TL_TLA_TLA8FL ();

extern TLreal8 TL_TLA_TLA8LN ();

extern TLreal8 TL_TLA_TLA8MD ();

extern TLreal8 TL_TLA_TLA8ML ();

extern TLreal8 TL_TLA_TLA8MN ();

extern TLreal8 TL_TLA_TLA8MX ();

extern TLreal8 TL_TLA_TLA8QR ();

extern TLint4 TL_TLA_TLA8RD ();

extern TLreal8 TL_TLA_TLA8RE ();

extern TLreal8 TL_TLA_TLA8SD ();

extern TLint4 TL_TLA_TLA8SG ();

extern TLreal8 TL_TLA_TLA8SR ();

extern TLreal8 TL_TLA_TLA8TD ();

extern TLreal8 TL_TLA_TLA8TR ();

extern void TL_TLA_TLA8X ();

extern TLreal8 TL_TLA_TLA8XP ();

extern TLint4 TL_TLA_TLAIDV ();

extern TLint4 TL_TLA_TLAIMD ();

extern TLint4 TL_TLA_TLAIMN ();

extern TLint4 TL_TLA_TLAIMX ();

extern TLint4 TL_TLA_TLAIML ();

extern TLnat4 TL_TLA_TLANDV ();

extern TLnat4 TL_TLA_TLANMD ();

extern TLnat4 TL_TLA_TLANML ();

extern TLnat4 TL_TLA_TLANMU ();

extern TLint4 TL_TLA_TLAPII ();

extern TLreal8 TL_TLA_TLAPRI ();

extern TLreal8 TL_TLA_TLAPRR ();

extern void TL_TLA_TLARSC ();

extern void TL_TLA_TLARNI ();

extern void TL_TLA_TLARNR ();

extern void TL_TLA_TLARNZ ();

extern void TL_TLA_TLARSR ();

extern void TL_TLA_TLARSZ ();

extern void TL_TLA_TLARZ ();

extern TLreal8 TL_TLA_TLAV48 ();

extern TLnat4 TL_TLA_TLAV84 ();

extern void TL_TLA_TLAVRS ();

extern void TL_TLA_TLAVES ();

extern void TL_TLA_TLAVFS ();

extern TLnat4 TL_TLA_TLAVI4 ();

extern TLreal8 TL_TLA_TLAVI8 ();

extern TLnat4 TL_TLA_TLAVN4 ();

extern TLreal8 TL_TLA_TLAVN8 ();

extern void TL_TLA_TLAVS8 ();

extern void TL_TLA_TLAVSI ();

extern void TL_TLA_TLAVSN ();

void TL_TLA () {
    TL_TLA_TLA8HI = 0;
    TL_TLA_TLA8LO = 0;
    {
	static TLreal8	one;
	typedef	TLnat4	__x607[2];
	typedef	__x607	LongArray;
	one = 1;
	if (((* (LongArray *) &one)[0]) == 0) {
	    TL_TLA_TLA8HI = 1;
	    TL_TLA_TLA8LO = 0;
	} else {
	    if (((* (LongArray *) &one)[1]) == 0) {
		TL_TLA_TLA8HI = 0;
		TL_TLA_TLA8LO = 1;
	    };
	};
    };
    TL_TLA_TLARZ();
}
