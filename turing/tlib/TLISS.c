#include "include\cinterface"
typedef	TLnat4	TL_TL_priority_t;
struct	TL_TL_ExceptionInfo {
    TLint4	quitCode;
    TLint4	libraryQuitCode;
    TLstring	errorMsg;
};
typedef	TLnat1	__x546[1];
struct	TL_TL_HandlerArea {
    TLint4	quitCode;
    TLnat4	lineAndFile;
    TLaddressint	fileTable;
    struct TL_TL_HandlerArea	*nextHandler;
    __x546	savedState;
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
typedef	TLint4	TL_TLI_StreamNumberType;
typedef	TLnat2	TL_TLI_StreamModeSet;
struct	TL_TLI_StreamEntryType {
    TLaddressint	fileName;
    TLaddressint	info;
    TL_Cint	lastOp;
    TL_TLI_StreamModeSet	mode;
    TLboolean	atEof;
    struct TL_TL_ProcessDescriptor	*waitingForInput;
};
typedef	TLaddressint	TL_TLI___x549[21];
typedef	TL_TLI___x549	TL_TLI_ArgList;
typedef	TLchar	TL_TLI___x552[16];
typedef	TL_TLI___x552	TL_TLI___x551[3];
typedef	struct TL_TLI_StreamEntryType	TL_TLI___x553[33];
extern TL_TLI___x553	TL_TLI_TLIS;
extern TLboolean	TL_TLI_TLIUXS;
extern TLint4	TL_TLI_TLIXSN;
extern TLaddressint	TLIstdin;
extern TLaddressint	TLIstdout;
extern TLaddressint	TLIstderr;
extern TLint4	TL_TLI_TLIARC;
extern TLaddressint	TL_TLI_TLIARV;
typedef	TLchar	TL_TLI___x557[42];
extern TL_TLI___x557	TL_TLI_TLIPXL;
typedef	TLchar	TL_TLI___x558[44];
extern TL_TLI___x558	TL_TLI_TLIPXN;
extern TLaddressint 	(*TL_TLI_TLIFOP)();
extern void 	(*TL_TLI_TLIFFL)();
extern void 	(*TL_TLI_TLIFCL)();
extern TL_Cint 	(*TL_TLI_TLIFGC)();
extern void 	(*TL_TLI_TLIFUG)();
extern void 	(*TL_TLI_TLIFPC)();
extern void 	(*TL_TLI_TLIFPS)();
extern void 	(*TL_TLI_TLIFSK)();
extern TLint4 	(*TL_TLI_TLIFTL)();
extern TL_Cint 	(*TL_TLI_TLIFRE)();
extern TL_Cint 	(*TL_TLI_TLIFWR)();
extern void 	(*TL_TLI_TLIFZ)();

extern void TL_TLI_TLIGT ();

extern void TL_TLI_TLIOS ();

extern void TL_TLI_TLIFS ();

extern void TL_TLI_TLIWRR ();

extern void TL_TLI_TLIPS ();

extern void TL_TLI_TLIRER ();
void TL_TLI_TLISS (streamNo, ioMode)
TLint4	streamNo;
TL_Cint	ioMode;
{
    typedef	TLchar	__x562[16];
    typedef	__x562	__x561[5];
    static __x561	operation = 
	{"Seek or Tell", "Get", "Put", "Read", "Write"};
    typedef	TLchar	__x563[61];
    __x563	errorMsg;
    register TLnat2	streamMode;
    TLBINDREG((*stream), struct TL_TLI_StreamEntryType);
    if ((streamNo < -2) || (streamNo > 30)) {
	if ((streamNo > 0) && (streamNo <= TL_TLI_TLIARC)) {
	    {
		TLstring	__x566;
		TL_TLS_TLSVIS((TLint4) streamNo, (TLint4) 1, (TLint4) 10, __x566);
		{
		    TLstring	__x565;
		    TL_TLS_TLSCAT("Argument number ", __x566, __x565);
		    {
			TLstring	__x564;
			TL_TLS_TLSCAT(__x565, " is too large", __x564);
			TLSTRASS(60, errorMsg, __x564);
		    };
		};
	    };
	} else {
	    {
		TLstring	__x569;
		TL_TLS_TLSVIS((TLint4) streamNo, (TLint4) 1, (TLint4) 10, __x569);
		{
		    TLstring	__x568;
		    TL_TLS_TLSCAT(operation[ioMode], " attempted on illegal stream number ", __x568);
		    {
			TLstring	__x567;
			TL_TLS_TLSCAT(__x568, __x569, __x567);
			TLSTRASS(60, errorMsg, __x567);
		    };
		};
	    };
	};
	TL_TLQUIT(errorMsg, (TLint4) 73);
    };
    streamMode = TL_TLI_TLIS[streamNo - -2].mode;
    if (!TL_TLI_TLIUXS) {
	TL_TLI_TLIXSN = streamNo;
    };
    if (streamMode == 0) {
	if (streamNo <= TL_TLI_TLIARC) {
	    {
		TLstring	__x572;
		TL_TLS_TLSVIS((TLint4) TL_TLI_TLIXSN, (TLint4) 1, (TLint4) 10, __x572);
		{
		    TLstring	__x571;
		    TL_TLS_TLSCAT("Argument number ", __x572, __x571);
		    {
			TLstring	__x570;
			TL_TLS_TLSCAT(__x571, " is too large", __x570);
			TL_TLQUIT(__x570, (TLint4) 73);
		    };
		};
	    };
	} else {
	    {
		TLstring	__x575;
		TL_TLS_TLSVIS((TLint4) TL_TLI_TLIXSN, (TLint4) 1, (TLint4) 10, __x575);
		{
		    TLstring	__x574;
		    TL_TLS_TLSCAT(operation[ioMode], " attempted on unopened stream number ", __x574);
		    {
			TLstring	__x573;
			TL_TLS_TLSCAT(__x574, __x575, __x573);
			TL_TLQUIT(__x573, (TLint4) 77);
		    };
		};
	    };
	};
    };
    if ((TLSMLSMLSETCONST(15) & streamMode) != 0) {
	{
	    TLstring	__x578;
	    TL_TLS_TLSVIS((TLint4) TL_TLI_TLIXSN, (TLint4) 1, (TLint4) 10, __x578);
	    {
		TLstring	__x577;
		TL_TLS_TLSCAT(operation[ioMode], " attempted on closed stream number ", __x577);
		{
		    TLstring	__x576;
		    TL_TLS_TLSCAT(__x577, __x578, __x576);
		    TL_TLQUIT(__x576, (TLint4) 75);
		};
	    };
	};
    };
    if ((TLSMLSMLSETCONST(14) & streamMode) != 0) {
	TLint4	sn;
	sn = streamNo;
	TL_TLI_TLIOS(&(sn), (TL_TLI_StreamModeSet) ((TLSMLSMLSETCONST(ioMode))), (TLboolean) 1);
    } else {
	if ((TLSMLSMLSETCONST(ioMode) & streamMode) == 0) {
	    {
		TLstring	__x581;
		TL_TLS_TLSVIS((TLint4) TL_TLI_TLIXSN, (TLint4) 1, (TLint4) 10, __x581);
		{
		    TLstring	__x580;
		    TL_TLS_TLSCAT(operation[ioMode], " attempted on incompatible stream number ", __x580);
		    {
			TLstring	__x579;
			TL_TLS_TLSCAT(__x580, __x581, __x579);
			TL_TLQUIT(__x579, (TLint4) 76);
		    };
		};
	    };
	} else {
	    if (streamNo == -2) {
		(*TL_TLI_TLIFFL)((TLaddressint) (TL_TLI_TLIS[1].info));
		(*TL_TLI_TLIFFL)((TLaddressint) (TL_TLI_TLIS[2].info));
	    };
	};
    };
    stream = &(TL_TLI_TLIS[streamNo - -2]);
    if (((*stream).lastOp) != ioMode) {
	if ((((*stream).lastOp) == 1) || (((*stream).lastOp) == 3)) {
	    if ((ioMode == 2) || (ioMode == 4)) {
		if ((*stream).atEof) {
		    (*TL_TLI_TLIFSK)((TLaddressint) ((*stream).info), (TLint4) 0, (TL_Cint) 2);
		} else {
		    (*TL_TLI_TLIFSK)((TLaddressint) ((*stream).info), (TLint4) 0, (TL_Cint) 1);
		};
	    };
	} else {
	    if ((((*stream).lastOp) == 2) || (((*stream).lastOp) == 4)) {
		if ((ioMode == 1) || (ioMode == 3)) {
		    (*TL_TLI_TLIFSK)((TLaddressint) ((*stream).info), (TLint4) 0, (TL_Cint) 1);
		};
	    };
	};
	(*stream).lastOp = ioMode;
    };
}
