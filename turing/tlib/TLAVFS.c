#include "include\cinterface"
typedef	TLnat4	TL_TL_priority_t;
struct	TL_TL_ExceptionInfo {
    TLint4	quitCode;
    TLint4	libraryQuitCode;
    TLstring	errorMsg;
};
typedef	TLnat1	__x592[1];
struct	TL_TL_HandlerArea {
    TLint4	quitCode;
    TLnat4	lineAndFile;
    TLaddressint	fileTable;
    struct TL_TL_HandlerArea	*nextHandler;
    __x592	savedState;
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
typedef	TLnat4	TL_TLA___x595[11];
extern TL_TLA___x595	TL_TLA_TLARS;
extern TLint4	TL_TLA_TLA8HI;
extern TLint4	TL_TLA_TLA8LO;

extern void TL_TLA_TLAV8D ();

extern void TL_TLA_TLAVD8 ();

extern TLint4 TL_TLA_TLA8GX ();

extern TLreal8 TL_TLA_TLA8SX ();

extern TLreal8 TL_TLA_TLA8T ();

extern void TL_TLA_TLA8X ();

extern void TL_TLA_TLARSC ();

extern void TL_TLA_TLAVRS ();

extern void TL_TLA_TLAVES ();
void TL_TLA_TLAVFS (source, width, fWidth, target, suppress, error)
TLreal8	source;
TLint4	width;
TLint4	fWidth;
TLstring	target;
TLboolean	suppress;
TLboolean	*error;
{
    TLstring	fractionString;
    register TLaddressint	src;
    TLint4	exponent;
    TLboolean	negative;
    TLboolean	giveDecPt;
    TLnat2	widthNeeded;
    register TLaddressint	dst;
    {
	TLnat4	hiOrder;
	TLnat4	loOrder;
	TL_TLA_TLAV8D((TLreal8) source, &(hiOrder), &(loOrder));
	if (((hiOrder & 2146435072) >> 20) == 2047) {
	    TLnat4	mantissa;
	    register TLaddressint	dst;
	    mantissa = (hiOrder & 1048575) >> 0;
	    dst = (TLaddressint) ((unsigned long)target);
	    if ((mantissa == 0) && (loOrder == 0)) {
		{
		    register TLint4	__x579;
		    for (__x579 = width; __x579 >= 4; __x579--) {
			(* (TLchar *) dst) = ' ';
			dst += 1;
		    };
		};
		if (((hiOrder & (TLint4) -0x80000000) >> 31) == 0) {
		    (* (TLchar *) dst) = '+';
		    dst += 1;
		} else {
		    (* (TLchar *) dst) = '-';
		    dst += 1;
		};
		(* (TLchar *) dst) = 'I';
		dst += 1;
		(* (TLchar *) dst) = 'n';
		dst += 1;
		(* (TLchar *) dst) = 'f';
		dst += 1;
	    } else {
		{
		    register TLint4	__x580;
		    for (__x580 = width; __x580 >= 3; __x580--) {
			(* (TLchar *) dst) = ' ';
			dst += 1;
		    };
		};
		(* (TLchar *) dst) = 'N';
		dst += 1;
		(* (TLchar *) dst) = 'a';
		dst += 1;
		(* (TLchar *) dst) = 'N';
		dst += 1;
	    };
	    (* (TLchar *) dst) = '\0';
	    (*error) = 0;
	    return;
	};
    };
    giveDecPt = 1;
    TL_TLA_TLAVRS(fractionString, (TLreal8) source, (TLint4) fWidth, &(exponent), &(negative), (TLboolean) 0, &((*error)));
    if ((*error)) {
	return;
    };
    if (suppress) {
	register TLnat2	fWidthTaken;
	fWidthTaken = fWidth;
	src = (TLaddressint) (((unsigned long) ((unsigned long)fractionString) + TL_TLS_TLSLEN(fractionString)) - 1);
	for(;;) {
	    if (((* (TLchar *) src)) != '0') {
		break;
	    };
	    src -= 1;
	    fWidthTaken -= 1;
	    if (fWidthTaken <= 0) {
		break;
	    };
	};
	if (fWidthTaken == 0) {
	    giveDecPt = 0;
	};
	src += 1;
	(* (TLchar *) src) = '\0';
    };
    widthNeeded = TL_TLS_TLSLEN(fractionString);
    if (negative) {
	widthNeeded += 1;
    };
    if (giveDecPt) {
	widthNeeded += 1;
    };
    if (exponent <= 0) {
	widthNeeded += 1 - exponent;
    };
    if (widthNeeded > 255) {
	(*error) = 1;
	return;
    };
    dst = (TLaddressint) ((unsigned long)target);
    {
	register TLint4	__x589;
	for (__x589 = width - widthNeeded; __x589 >= 1; __x589--) {
	    (* (TLchar *) dst) = ' ';
	    dst += 1;
	};
    };
    if (negative) {
	(* (TLchar *) dst) = '-';
	dst += 1;
    };
    src = (TLaddressint) ((unsigned long)fractionString);
    if (exponent > 0) {
	{
	    register TLint4	__x590;
	    for (__x590 = exponent; __x590 >= 1; __x590--) {
		(* (TLchar *) dst) = (* (TLchar *) src);
		dst += 1;
		src += 1;
	    };
	};
	if (giveDecPt) {
	    (* (TLchar *) dst) = '.';
	    dst += 1;
	};
    } else {
	(* (TLchar *) dst) = '0';
	dst += 1;
	if (giveDecPt) {
	    (* (TLchar *) dst) = '.';
	    dst += 1;
	    {
		register TLint4	__x591;
		for (__x591 = exponent; __x591 <= -1; __x591++) {
		    (* (TLchar *) dst) = '0';
		    dst += 1;
		};
	    };
	};
    };
    for(;;) {
	(* (TLchar *) dst) = (* (TLchar *) src);
	dst += 1;
	if (((* (TLchar *) src)) == '\0') {
	    break;
	};
	src += 1;
    };
}
