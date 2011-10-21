#include "include\cinterface"
typedef	TLnat4	TL_TL_priority_t;
struct	TL_TL_ExceptionInfo {
    TLint4	quitCode;
    TLint4	libraryQuitCode;
    TLstring	errorMsg;
};
typedef	TLnat1	__x339[1];
struct	TL_TL_HandlerArea {
    TLint4	quitCode;
    TLnat4	lineAndFile;
    TLaddressint	fileTable;
    struct TL_TL_HandlerArea	*nextHandler;
    __x339	savedState;
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
void TL_TLE_TLEABT (abortNo)
TL_Cint	abortNo;
{
    TLBINDREG((*E), struct TL_TL_ExceptionInfo);
    E = &(TL_TLKPD->exception);
    switch (abortNo) {
	case 1:
	    {
		TLSTRASS(255, (*E).errorMsg, "Array subscript is out of range");
		(*E).quitCode = 11;
	    }
	    break;
	case 2:
	    {
		TLSTRASS(255, (*E).errorMsg, "Dynamic array upper bound is less than lower bound");
		(*E).quitCode = 12;
	    }
	    break;
	case 3:
	    {
		TLSTRASS(255, (*E).errorMsg, "Union field is not in alternative selected by current tag value");
		(*E).quitCode = 13;
	    }
	    break;
	case 4:
	    {
		TLSTRASS(255, (*E).errorMsg, "Tag value is out of range");
		(*E).quitCode = 14;
	    }
	    break;
	case 5:
	    {
		TLSTRASS(255, (*E).errorMsg, "Assignment value is out of range");
		(*E).quitCode = 15;
	    }
	    break;
	case 6:
	    {
		TLSTRASS(255, (*E).errorMsg, "Pre condition is false");
		(*E).quitCode = 16;
	    }
	    break;
	case 7:
	    {
		TLSTRASS(255, (*E).errorMsg, "Post condition is false");
		(*E).quitCode = 17;
	    }
	    break;
	case 8:
	    {
		TLSTRASS(255, (*E).errorMsg, "Loop invariant is false");
		(*E).quitCode = 18;
	    }
	    break;
	case 9:
	    {
		TLSTRASS(255, (*E).errorMsg, "For loop invariant is false");
		(*E).quitCode = 19;
	    }
	    break;
	case 10:
	    {
		TLSTRASS(255, (*E).errorMsg, "Module invariant is false");
		(*E).quitCode = 20;
	    }
	    break;
	case 11:
	    {
		TLSTRASS(255, (*E).errorMsg, "Assert condition is false");
		(*E).quitCode = 21;
	    }
	    break;
	case 12:
	    {
		TLSTRASS(255, (*E).errorMsg, "Value parameter is out of range");
		(*E).quitCode = 22;
	    }
	    break;
	case 13:
	    {
		TLSTRASS(255, (*E).errorMsg, "Length of string parameter exceeds max length of string formal");
		(*E).quitCode = 23;
	    }
	    break;
	case 14:
	    {
		TLSTRASS(255, (*E).errorMsg, "Result value is out of range");
		(*E).quitCode = 24;
	    }
	    break;
	case 15:
	    {
		TLSTRASS(255, (*E).errorMsg, "Case selector is out of range");
		(*E).quitCode = 25;
	    }
	    break;
	case 16:
	    {
		TLSTRASS(255, (*E).errorMsg, "Function failed to give a result");
		(*E).quitCode = 26;
	    }
	    break;
	case 17:
	    {
		TLSTRASS(255, (*E).errorMsg, "Collection element has been freed");
		(*E).quitCode = 27;
	    }
	    break;
	case 18:
	    {
		TLSTRASS(255, (*E).errorMsg, "Collection subscript is nil");
		(*E).quitCode = 28;
	    }
	    break;
	case 19:
	    {
		TLSTRASS(255, (*E).errorMsg, "Set element is out of range");
		(*E).quitCode = 29;
	    }
	    break;
	case 20:
	    {
		TLSTRASS(255, (*E).errorMsg, "Subprogram calls nested too deeply. (Probable cause: infinite recursion)");
		(*E).quitCode = 30;
	    }
	    break;
	case 21:
	    {
		TLSTRASS(255, (*E).errorMsg, "Bound variables overlap");
		(*E).quitCode = 31;
	    }
	    break;
	case 22:
	    {
		TLSTRASS(255, (*E).errorMsg, "Reference parameters overlap");
		(*E).quitCode = 32;
	    }
	    break;
	case 23:
	    {
		TLSTRASS(255, (*E).errorMsg, "Division (or modulus) by zero");
		(*E).quitCode = 33;
	    }
	    break;
	case 24:
	    {
		TLSTRASS(255, (*E).errorMsg, "Union tag has not been set");
		(*E).quitCode = 34;
	    }
	    break;
	case 25:
	    {
		TLSTRASS(255, (*E).errorMsg, "Length of string value exceeds max length of variable");
		(*E).quitCode = 35;
	    }
	    break;
	case 26:
	    {
		TLSTRASS(255, (*E).errorMsg, "Illegal parameter to \"chr\"");
		(*E).quitCode = 36;
	    }
	    break;
	case 27:
	    {
		TLSTRASS(255, (*E).errorMsg, "Parameter to \"ord\" is not of length one");
		(*E).quitCode = 37;
	    }
	    break;
	case 28:
	    {
		TLSTRASS(255, (*E).errorMsg, "Pred applied to the first value of the enumeration");
		(*E).quitCode = 38;
	    }
	    break;
	case 29:
	    {
		TLSTRASS(255, (*E).errorMsg, "Succ applied to the last value of the enumeration");
		(*E).quitCode = 39;
	    }
	    break;
	case 30:
	    {
		TLSTRASS(255, (*E).errorMsg, "Invalid subscript in charstring([*-]expn)");
		(*E).quitCode = 40;
	    }
	    break;
	case 31:
	    {
		TLSTRASS(255, (*E).errorMsg, "string or char(n) assigned to char is not length 1");
		(*E).quitCode = 41;
	    }
	    break;
	case 32:
	    {
		TLSTRASS(255, (*E).errorMsg, "right side of assignment to char(n) is not length \'n\'");
		(*E).quitCode = 42;
	    }
	    break;
	case 33:
	    {
		TLSTRASS(255, (*E).errorMsg, "char converted to string contains EOS or uninitchar");
		(*E).quitCode = 43;
	    }
	    break;
	case 34:
	    {
		TLSTRASS(255, (*E).errorMsg, "string or char(n) coerced to char is not length 1");
		(*E).quitCode = 44;
	    }
	    break;
	case 35:
	    {
		TLSTRASS(255, (*E).errorMsg, "Variable has no value");
		(*E).quitCode = 45;
	    }
	    break;
	case 36:
	    {
		TLSTRASS(255, (*E).errorMsg, "Overflow in Integer expression");
		(*E).quitCode = 50;
	    }
	    break;
	case 37:
	    {
		TLSTRASS(255, (*E).errorMsg, "Division (or modulus) by zero in Integer expression");
		(*E).quitCode = 33;
	    }
	    break;
	case 38:
	    {
		TLSTRASS(255, (*E).errorMsg, "Overflow in Real expression");
		(*E).quitCode = 51;
	    }
	    break;
	case 39:
	    {
		TLSTRASS(255, (*E).errorMsg, "Division (or modulus) by zero in Real expression");
		(*E).quitCode = 33;
	    }
	    break;
	case 40:
	    {
		TLSTRASS(255, (*E).errorMsg, "Underflow in Real expression");
		(*E).quitCode = 52;
	    }
	    break;
	default :
	    {
		TLSTRASS(255, (*E).errorMsg, "Internal Turing+ System Error - Unexpected abort");
		(*E).quitCode = 61;
	    }
	    break;
    };
    TL_TLE_TLEQUIT ((TLint4) (*E).quitCode, (char *) 0, 0);
}
