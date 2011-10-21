#include "include\cinterface"
typedef	TLnat4	TL_TL_priority_t;
struct	TL_TL_ExceptionInfo {
    TLint4	quitCode;
    TLint4	libraryQuitCode;
    TLstring	errorMsg;
};
typedef	TLnat1	__x383[1];
struct	TL_TL_HandlerArea {
    TLint4	quitCode;
    TLnat4	lineAndFile;
    TLaddressint	fileTable;
    struct TL_TL_HandlerArea	*nextHandler;
    __x383	savedState;
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
typedef	TLnat4	TL_TLA___x386[11];
extern TL_TLA___x386	TL_TLA_TLARS;
extern TLint4	TL_TLA_TLA8HI;
extern TLint4	TL_TLA_TLA8LO;
TLnat4 TL_TLA_TLA4AD (right, left)
TLnat4	right;
TLnat4	left;
{
    TLnat4	bigger;
    TLnat2	bExponent;
    TLnat4	bMantissa;
    TLnat4	smaller;
    TLnat2	sExponent;
    TLnat4	sMantissa;
    TLnat2	lExponent;
    TLnat4	lMantissa;
    TLnat2	rExponent;
    TLnat4	rMantissa;
    TLint2	exponent;
    register TLnat4	mantissa;
    TLnat4	answer;
    lExponent = (left & 2139095040) >> 23;
    lMantissa = ((left & 8388607) >> 0) | ((TLint4) 1 << 23);
    rExponent = (right & 2139095040) >> 23;
    rMantissa = ((right & 8388607) >> 0) | ((TLint4) 1 << 23);
    if (lExponent > rExponent) {
	bigger = left;
	bExponent = lExponent;
	bMantissa = lMantissa;
	smaller = right;
	sExponent = rExponent;
	sMantissa = rMantissa;
    } else {
	if (lExponent < rExponent) {
	    bigger = right;
	    bExponent = rExponent;
	    bMantissa = rMantissa;
	    smaller = left;
	    sExponent = lExponent;
	    sMantissa = lMantissa;
	} else {
	    if (lMantissa > rMantissa) {
		bigger = left;
		bExponent = lExponent;
		bMantissa = lMantissa;
		smaller = right;
		sExponent = rExponent;
		sMantissa = rMantissa;
	    } else {
		bigger = right;
		bExponent = rExponent;
		bMantissa = rMantissa;
		smaller = left;
		sExponent = lExponent;
		sMantissa = lMantissa;
	    };
	};
    };
    if (bExponent == 255) {
	TL_TLQUIT("Infinite/indefinite value being added to", (TLint4) 51);
    };
    exponent = bExponent;
    mantissa = sMantissa;
    if (bExponent > sExponent) {
	if (bExponent > (sExponent + 1)) {
	    mantissa >>= (bExponent - sExponent) - 1;
	};
    } else {
	mantissa <<= 1;
    };
    if (((bigger & (TLint4) -0x80000000) >> 31) == ((smaller & (TLint4) -0x80000000) >> 31)) {
	mantissa += (TLint4) bMantissa << 1;
	if ((mantissa & ((TLint4) 1 << (23 + 2))) != 0) {
	    exponent += 1;
	    mantissa += 2;
	    mantissa >>= 2;
	} else {
	    mantissa += 1;
	    if ((mantissa & ((TLint4) 1 << (23 + 2))) != 0) {
		exponent += 1;
		mantissa >>= 2;
	    } else {
		mantissa >>= 1;
	    };
	};
    } else {
	mantissa = ((TLint4) bMantissa << 1) - mantissa;
	if ((mantissa & ((TLint4) 1 << (23 + 1))) != 0) {
	    mantissa += 1;
	    mantissa >>= 1;
	} else {
	    if (mantissa == 0) {
		return (0);
	    } else {
		for(;;) {
		    exponent -= 1;
		    if ((mantissa & ((TLint4) 1 << 23)) != 0) {
			break;
		    };
		    mantissa <<= 1;
		};
	    };
	};
    };
    if (exponent < 0) {
	if (TL_TLECU) {
	    TL_TLQUIT("Underflow in real4 addition", (TLint4) 52);
	};
	return (0);
    };
    if (exponent >= 255) {
	TL_TLQUIT("Overflow in real4 addition", (TLint4) 51);
    };
    answer = (((TLint4) ((bigger & (TLint4) -0x80000000) >> 31) << 31) & (TLint4) -0x80000000) | (answer & 2147483647);
    answer = (((TLint4) exponent << 23) & 2139095040) | (answer & -2139095041);
    answer = (((TLint4) mantissa << 0) & 8388607) | (answer & -8388608);
    return (answer);
    /* NOTREACHED */
}
