#include "include\cinterface"
typedef	TLnat4	TL_TL_priority_t;
struct	TL_TL_ExceptionInfo {
    TLint4	quitCode;
    TLint4	libraryQuitCode;
    TLstring	errorMsg;
};
typedef	TLnat1	__x432[1];
struct	TL_TL_HandlerArea {
    TLint4	quitCode;
    TLnat4	lineAndFile;
    TLaddressint	fileTable;
    struct TL_TL_HandlerArea	*nextHandler;
    __x432	savedState;
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
typedef	TLaddressint	TL_TLI___x435[21];
typedef	TL_TLI___x435	TL_TLI_ArgList;
typedef	TLchar	TL_TLI___x438[16];
typedef	TL_TLI___x438	TL_TLI___x437[3];
typedef	struct TL_TLI_StreamEntryType	TL_TLI___x439[33];
extern TL_TLI___x439	TL_TLI_TLIS;
extern TLboolean	TL_TLI_TLIUXS;
extern TLint4	TL_TLI_TLIXSN;
extern TLaddressint	TLIstdin;
extern TLaddressint	TLIstdout;
extern TLaddressint	TLIstderr;
extern TLint4	TL_TLI_TLIARC;
extern TLaddressint	TL_TLI_TLIARV;
typedef	TLchar	TL_TLI___x443[42];
extern TL_TLI___x443	TL_TLI_TLIPXL;
typedef	TLchar	TL_TLI___x444[44];
extern TL_TLI___x444	TL_TLI_TLIPXN;
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
void TL_TLI_TLIOS (streamNo, openMode, dieOnError)
TLint4	*streamNo;
TL_TLI_StreamModeSet	openMode;
TLboolean	dieOnError;
{
    TLBINDREG((*stream), struct TL_TLI_StreamEntryType);
    TLnat2	inputModes;
    TLnat2	outputModes;
    TLboolean	binaryMode;
    stream = &(TL_TLI_TLIS[(*streamNo) - -2]);
    inputModes = openMode & ((TLSMLSMLSETCONST(1) | TLSMLSMLSETCONST(3)));
    outputModes = openMode & ((TLSMLSMLSETCONST(2) | TLSMLSMLSETCONST(4)));
    if (((openMode & ((TLSMLSMLSETCONST(1) | TLSMLSMLSETCONST(2)))) != 0) && ((openMode & ((TLSMLSMLSETCONST(3) | TLSMLSMLSETCONST(4)))) != 0)) {
//	if (dieOnError) {
	    TL_TLQUIT("You may not mix put/get with read/write on the Windows platform", (TLint4) 81);
//	} else {
//	    (*streamNo) = 0;
//	    return;
//	};
    };
    binaryMode = (((TLSMLSMLSETCONST(4) | TLSMLSMLSETCONST(3))) & openMode) != 0;
    if (outputModes != 0) {
	if ((TLSMLSMLSETCONST(5) & openMode) != 0) {
	    if (binaryMode) {
		(*stream).info = (TLaddressint) (*TL_TLI_TLIFOP)((* (TLstring *) ((*stream).fileName)), "r+b");
	    } else {
		(*stream).info = (TLaddressint) (*TL_TLI_TLIFOP)((* (TLstring *) ((*stream).fileName)), "r+");
	    };
	    if ((unsigned long) ((*stream).info) == 0) {
		if (binaryMode) {
		    (*stream).info = (TLaddressint) (*TL_TLI_TLIFOP)((* (TLstring *) ((*stream).fileName)), "w+b");
		} else {
		    (*stream).info = (TLaddressint) (*TL_TLI_TLIFOP)((* (TLstring *) ((*stream).fileName)), "w+");
		};
		(*stream).atEof = 1;
	    } else {
		(*stream).atEof = 0;
	    };
	} else {
	    if (inputModes == 0) {
		if (binaryMode) {
		    (*stream).info = (TLaddressint) (*TL_TLI_TLIFOP)((* (TLstring *) ((*stream).fileName)), "wb");
		} else {
		    (*stream).info = (TLaddressint) (*TL_TLI_TLIFOP)((* (TLstring *) ((*stream).fileName)), "w");
		};
		(*stream).atEof = 1;
	    } else {
		if (binaryMode) {
		    (*stream).info = (TLaddressint) (*TL_TLI_TLIFOP)((* (TLstring *) ((*stream).fileName)), "w+b");
		} else {
		    (*stream).info = (TLaddressint) (*TL_TLI_TLIFOP)((* (TLstring *) ((*stream).fileName)), "w+");
		};
		(*stream).atEof = 0;
	    };
	};
    } else {
	if (inputModes != 0) {
	    if (binaryMode) {
		(*stream).info = (TLaddressint) (*TL_TLI_TLIFOP)((* (TLstring *) ((*stream).fileName)), "rb");
	    } else {
		(*stream).info = (TLaddressint) (*TL_TLI_TLIFOP)((* (TLstring *) ((*stream).fileName)), "r");
	    };
	    (*stream).atEof = 0;
	} else {
	    TL_TLQUIT("Must have at least one of \"get\", \"put\", \"read\", or \"write\" in open mode", (TLint4) 81);
	};
    };
    if ((unsigned long) ((*stream).info) == 0) {
	if (dieOnError) {
	    {
		TLstring	__x456;
		TL_TLS_TLSCAT("Cannot open file \"", (* (TLstring *) ((*stream).fileName)), __x456);
		{
		    TLstring	__x455;
		    TL_TLS_TLSCAT(__x456, "\"", __x455);
		    TL_TLQUIT(__x455, (TLint4) 82);
		};
	    };
	} else {
	    (*streamNo) = 0;
	    return;
	};
    };
    (*stream).mode = openMode;
    (*stream).lastOp = 0;
}
