#include "include\cinterface"
typedef	TLnat4	TL_TL_priority_t;
struct	TL_TL_ExceptionInfo {
    TLint4	quitCode;
    TLint4	libraryQuitCode;
    TLstring	errorMsg;
};
typedef	TLnat1	__x431[1];
struct	TL_TL_HandlerArea {
    TLint4	quitCode;
    TLnat4	lineAndFile;
    TLaddressint	fileTable;
    struct TL_TL_HandlerArea	*nextHandler;
    __x431	savedState;
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
typedef	TLaddressint	TL_TLI___x434[21];
typedef	TL_TLI___x434	TL_TLI_ArgList;
typedef	TLchar	TL_TLI___x437[16];
typedef	TL_TLI___x437	TL_TLI___x436[3];
typedef	struct TL_TLI_StreamEntryType	TL_TLI___x438[33];
extern TL_TLI___x438	TL_TLI_TLIS;
extern TLboolean	TL_TLI_TLIUXS;
extern TLint4	TL_TLI_TLIXSN;
extern TLaddressint	TLIstdin;
extern TLaddressint	TLIstdout;
extern TLaddressint	TLIstderr;
extern TLint4	TL_TLI_TLIARC;
extern TLaddressint	TL_TLI_TLIARV;
typedef	TLchar	TL_TLI___x442[42];
extern TL_TLI___x442	TL_TLI_TLIPXL;
typedef	TLchar	TL_TLI___x443[44];
extern TL_TLI___x443	TL_TLI_TLIPXN;
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
void TL_TLI_TLICL (streamNo)
TLint4	streamNo;
{
    TLBINDREG((*stream), struct TL_TLI_StreamEntryType);
    if ((streamNo < -2) || (streamNo > 30)) {
	{
	    TLstring	__x445;
	    TL_TLS_TLSVIS((TLint4) streamNo, (TLint4) 1, (TLint4) 10, __x445);
	    {
		TLstring	__x444;
		TL_TLS_TLSCAT("Close attempted on illegal stream number ", __x445, __x444);
		TL_TLQUIT(__x444, (TLint4) 73);
	    };
	};
    };
    if (streamNo <= 0) {
	{
	    TLstring	__x448;
	    TL_TLS_TLSVIS((TLint4) streamNo, (TLint4) 1, (TLint4) 10, __x448);
	    {
		TLstring	__x447;
		TL_TLS_TLSCAT("Close of standard stream ", __x448, __x447);
		{
		    TLstring	__x446;
		    TL_TLS_TLSCAT(__x447, " is not allowed", __x446);
		    TL_TLQUIT(__x446, (TLint4) 87);
		};
	    };
	};
    };
    stream = &(TL_TLI_TLIS[streamNo - -2]);
    if (((*stream).mode) == 0) {
	{
	    TLstring	__x450;
	    TL_TLS_TLSVIS((TLint4) streamNo, (TLint4) 1, (TLint4) 10, __x450);
	    {
		TLstring	__x449;
		TL_TLS_TLSCAT("Close attempted on unopened stream number ", __x450, __x449);
		TL_TLQUIT(__x449, (TLint4) 86);
	    };
	};
    };
    if ((TLSMLSMLSETCONST(15) & ((*stream).mode)) != 0) {
	{
	    TLstring	__x452;
	    TL_TLS_TLSVIS((TLint4) streamNo, (TLint4) 1, (TLint4) 10, __x452);
	    {
		TLstring	__x451;
		TL_TLS_TLSCAT("Close attempted on closed stream number ", __x452, __x451);
		TL_TLQUIT(__x451, (TLint4) 85);
	    };
	};
    };
    if ((TLSMLSMLSETCONST(14) & ((*stream).mode)) == 0) {
	(*TL_TLI_TLIFCL)((TLaddressint) ((*stream).info));
	if (streamNo > TL_TLI_TLIARC) {
	    TL_TLB_TLBMFR((TLaddressint) ((*stream).fileName));
	};
    };
    (*stream).mode = 0x8000;
}
