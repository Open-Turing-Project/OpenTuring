#include "include\cinterface"
#include <stdio.h>
typedef	TLnat4	TL_TL_priority_t;
struct	TL_TL_ExceptionInfo {
    TLint4	quitCode;
    TLint4	libraryQuitCode;
    TLstring	errorMsg;
};
typedef	TLnat1	__x567[1];
struct	TL_TL_HandlerArea {
    TLint4	quitCode;
    TLnat4	lineAndFile;
    TLaddressint	fileTable;
    struct TL_TL_HandlerArea	*nextHandler;
    __x567	savedState;
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
typedef	TLaddressint	TL_TLI___x570[21];
typedef	TL_TLI___x570	TL_TLI_ArgList;
typedef	TLchar	TL_TLI___x573[16];
typedef	TL_TLI___x573	TL_TLI___x572[3];
static TL_TLI___x572	TL_TLI_StreamName = 
    {"Standard Input", "Standard Output", "Standard Error"};
typedef	struct TL_TLI_StreamEntryType	TL_TLI___x574[33];
TL_TLI___x574	TL_TLI_TLIS;
TLboolean	TL_TLI_TLIUXS;
TLint4	TL_TLI_TLIXSN;
extern TLaddressint	TLIstdin;
extern TLaddressint	TLIstdout;
extern TLaddressint	TLIstderr;
TLint4	TL_TLI_TLIARC;
TLaddressint	TL_TLI_TLIARV;
typedef	TLchar	TL_TLI___x578[42];
TL_TLI___x578	TL_TLI_TLIPXL;
typedef	TLchar	TL_TLI___x579[44];
TL_TLI___x579	TL_TLI_TLIPXN;
TLaddressint 	(*TL_TLI_TLIFOP)();
void 	(*TL_TLI_TLIFFL)();
void 	(*TL_TLI_TLIFCL)();
TL_Cint 	(*TL_TLI_TLIFGC)();
void 	(*TL_TLI_TLIFUG)();
void 	(*TL_TLI_TLIFPC)();
void 	(*TL_TLI_TLIFPS)();
void 	(*TL_TLI_TLIFSK)();
TLint4 	(*TL_TLI_TLIFTL)();
TL_Cint 	(*TL_TLI_TLIFRE)();
TL_Cint 	(*TL_TLI_TLIFWR)();
void 	(*TL_TLI_TLIFZ)();

extern void TL_TLI_TLIGT ();

extern void TL_TLI_TLIOS ();

extern void TL_TLI_TLICL ();

extern void TL_TLI_TLIEFR ();

extern TLboolean TL_TLI_TLIEOF ();

extern void TL_TLI_TLIFA ();

extern void TL_TLI_TLIFS ();

extern void TL_TLI_TLIGC ();

extern void TL_TLI_TLIGCB ();

extern void TL_TLI_TLIGI ();

extern void TL_TLI_TLIGIB ();

extern void TL_TLI_TLIGK ();

extern void TL_TLI_TLIGN ();

extern void TL_TLI_TLIGR ();

extern void TL_TLI_TLIGS ();

extern void TL_TLI_TLIGSS ();

extern void TL_TLI_TLIGSW ();

extern void TL_TLI_TLIOA ();

extern void TL_TLI_TLIOF ();

extern void TL_TLI_TLIOP ();

extern void TL_TLI_TLIWRR ();

extern void TL_TLI_TLIPC ();

extern void TL_TLI_TLIPE ();

extern void TL_TLI_TLIPF ();

extern void TL_TLI_TLIPI ();

extern void TL_TLI_TLIPK ();

extern void TL_TLI_TLIPN ();

extern void TL_TLI_TLIPR ();

extern void TL_TLI_TLIPS ();

extern void TL_TLI_TLIRER ();

extern void TL_TLI_TLIRE ();

extern void TL_TLI_TLISS ();

extern void TL_TLI_TLISK ();

extern void TL_TLI_TLISKE ();

extern void TL_TLI_TLISSI ();

extern void TL_TLI_TLISSO ();

extern void TL_TLI_TLISSS ();

extern void TL_TLI_TLITL ();

extern void TL_TLI_TLIWR ();

extern TLaddressint TL_TLI_TLIGF ();

extern void TL_TLI_TLISF ();

extern void TL_TLI_TLIUDUMP ();

extern void TL_TLI_TLIAON ();

extern void TL_TLI_TLIAOFF ();

extern void TL_TLI_TLIFINI ();

void TL_TLI () {
    TL_TLI_TLIUXS = 0;
    {
	register TLint4	sn;
	for (sn = -2; sn <= 30; sn++) {
	    TL_TLI_TLIS[sn - -2].mode = 0x0;
	    TL_TLI_TLIS[sn - -2].waitingForInput = (struct TL_TL_ProcessDescriptor *) 0;
	};
    };
    TL_TLI_TLIS[0].fileName = (TLaddressint) ((unsigned long)TL_TLI_StreamName[0]);
    TL_TLI_TLIS[0].info = (TLaddressint) stdin;
    TL_TLI_TLIS[0].lastOp = 0;
    TL_TLI_TLIS[0].atEof = 0;
    TL_TLI_TLIS[1].fileName = (TLaddressint) ((unsigned long)TL_TLI_StreamName[1]);
    TL_TLI_TLIS[1].info = (TLaddressint) stdout;
    TL_TLI_TLIS[1].lastOp = 0;
    TL_TLI_TLIS[1].atEof = 1;
    TL_TLI_TLIS[2].fileName = (TLaddressint) ((unsigned long)TL_TLI_StreamName[2]);
    TL_TLI_TLIS[2].info = (TLaddressint) stderr;
    TL_TLI_TLIS[2].lastOp = 0;
    TL_TLI_TLIS[2].atEof = 1;
    {
	register TLint4	sn;
	static TLint4	__x575;
	__x575 =  TLSIMPLEMIN(TL_TLI_TLIARC, 20);
	sn = 1;
	if (sn <= __x575) {
	    for(;;) {
		TL_TLI_TLIS[sn - -2].fileName = (TLaddressint) ((* (TL_TLI_ArgList *) TL_TLI_TLIARV)[sn]);
		TL_TLI_TLIS[sn - -2].mode = 0x4000;
		if (sn == __x575) break;
		sn++;
	    }
	};
    };
    TLSTRASS(41, TL_TLI_TLIPXL, "String generated for put item is too long");
    TLSTRASS(43, TL_TLI_TLIPXN, "Negative field width specified for put item");
    TL_TLI_TLIZ();
}
