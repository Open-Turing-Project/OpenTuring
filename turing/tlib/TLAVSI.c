#include "include\cinterface"
typedef	TLnat4	TL_TL_priority_t;
struct	TL_TL_ExceptionInfo {
    TLint4	quitCode;
    TLint4	libraryQuitCode;
    TLstring	errorMsg;
};
typedef	TLnat1	__x602[1];
struct	TL_TL_HandlerArea {
    TLint4	quitCode;
    TLnat4	lineAndFile;
    TLaddressint	fileTable;
    struct TL_TL_HandlerArea	*nextHandler;
    __x602	savedState;
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
typedef	TLnat4	TL_TLA___x605[11];
extern TL_TLA___x605	TL_TLA_TLARS;
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

extern void TL_TLA_TLAVFS ();

extern void TL_TLA_TLAVS8 ();
void TL_TLA_TLAVSI (source, base, answer, error, quitOnError)
TLstring	source;
TLnat1	base;
TLint4	*answer;
TLboolean	*error;
TLboolean	quitOnError;
{
    register TLaddressint	src;
    register TLnat4	value;
    TLboolean	isNegative;
    TLnat4	maxValueDivBase;
    TLnat1	maxValueModBase;
    (*error) = 0;
    src = (TLaddressint) ((unsigned long)source);
    value = 0;
    isNegative = 0;
    for(;;) {
	if (((* (TLchar *) src)) != ' ') {
	    break;
	};
	src += 1;
    };
    if (((* (TLchar *) src)) == '-') {
	src += 1;
	isNegative = 1;
	maxValueDivBase = - (-2147483647 / base);
	maxValueModBase = - (-2147483647 % base);
    } else {
	maxValueDivBase = 0x7FFFFFFF / base;
	maxValueModBase = 0x7FFFFFFF % base;
	if (((* (TLchar *) src)) == '+') {
	    src += 1;
	};
    };
    if (((* (TLchar *) src)) == '\0') {
	if (quitOnError) {
	    TL_TLQUIT("String passed to \"strint\" is not in correct format", (TLint4) 90);
	};
	(*error) = 1;
	return;
    };
    for(;;) {
	if (((* (TLchar *) src)) != '0') {
	    break;
	};
	src += 1;
    };
    for(;;) {
	register TLnat1	digit;
	digit = (* (TLnat1 *) src);
	src += 1;
	if (digit == ((TLnat1) '\0')) {
	    break;
	};
	if ((digit >= ((TLnat1) '0')) && (digit <= ((TLnat1) '9'))) {
	    digit -= (TLnat1) '0';
	} else {
	    if ((digit >= ((TLnat1) 'A')) && (digit <= ((TLnat1) 'I'))) {
		digit -= ((TLnat1) 'A') - 10;
	    } else {
		if ((digit >= ((TLnat1) 'a')) && (digit <= ((TLnat1) 'i'))) {
		    digit -= ((TLnat1) 'a') - 10;
		} else {
		    if ((digit >= ((TLnat1) 'J')) && (digit <= ((TLnat1) 'R'))) {
			digit -= ((TLnat1) 'J') - 19;
		    } else {
			if ((digit >= ((TLnat1) 'j')) && (digit <= ((TLnat1) 'r'))) {
			    digit -= ((TLnat1) 'j') - 19;
			} else {
			    if ((digit >= ((TLnat1) 'S')) && (digit <= ((TLnat1) 'Z'))) {
				digit -= ((TLnat1) 'S') - 28;
			    } else {
				if ((digit >= ((TLnat1) 's')) && (digit <= ((TLnat1) 'z'))) {
				    digit -= ((TLnat1) 's') - 28;
				} else {
				    (*error) = 1;
				};
			    };
			};
		    };
		};
	    };
	};
	if ((*error) || (digit >= base)) {
	    if (quitOnError) {
		TL_TLQUIT("Illegal character in string passed to \"strint\"", (TLint4) 90);
	    };
	    (*error) = 1;
	    return;
	};
	if ((value > maxValueDivBase) || ((value == maxValueDivBase) && (digit > maxValueModBase))) {
	    if (quitOnError) {
		TL_TLQUIT("Overflow in result of \"strint\"", (TLint4) 50);
	    };
	    (*error) = 1;
	    return;
	};
	value *= base;
	value += digit;
    };
    if (isNegative) {
	(*answer) = - value;
    } else {
	(*answer) = value;
    };
}
