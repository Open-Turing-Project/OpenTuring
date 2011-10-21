#include "include\cinterface"
typedef	TLnat4	TL_TL_priority_t;
struct	TL_TL_ExceptionInfo {
    TLint4	quitCode;
    TLint4	libraryQuitCode;
    TLstring	errorMsg;
};
typedef	TLnat1	__x358[1];
struct	TL_TL_HandlerArea {
    TLint4	quitCode;
    TLnat4	lineAndFile;
    TLaddressint	fileTable;
    struct TL_TL_HandlerArea	*nextHandler;
    __x358	savedState;
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
typedef	TLint4	TL_TLB_Timestamp;
struct	TL_TLB_CheckedPointer {
    TLaddressint	objAddress;
    TL_TLB_Timestamp	timestamp;
};
typedef	TLint4	TL_TLB_CountTypeBits;
typedef	TLint4	TL_TLB_OpcodeBits;
typedef	TLint4	TL_TLB_CountByteBits;
typedef	TLint2	TL_TLB_OpcodeType;
extern TL_TLB_Timestamp	TL_TLB_TLBTS;

extern void TL_TLB_TLBMAL ();

extern void TL_TLB_TLBMFR ();

extern void TL_TLB_TLBFRU ();

extern void TL_TLB_TLBNWU ();
void TL_TLB_TLBUNR (variable, pattern, count)
TLaddressint	*variable;
TLaddressint	*pattern;
TLint4	count;
{
    TLaddressint	opcodeStart;
    register TL_TLB_OpcodeType	op;
    TLnat4	opCount;
    opcodeStart = (TLaddressint) (*pattern);
    op = (* (TL_TLB_OpcodeType *) (*pattern));
    switch ((op & 3) >> 0) {
	case 0:
	    {
		opCount = 0;
		(*pattern) += sizeof(TL_TLB_OpcodeType);
	    }
	    break;
	case 3:
	    {
		opCount = (op & 65280) >> 8;
		(*pattern) += sizeof(TL_TLB_OpcodeType);
	    }
	    break;
	case 2:
	    {
		opCount = (* (TLnat2 *) ((unsigned long) (*pattern) + sizeof(TL_TLB_OpcodeType)));
		(*pattern) += 2 * sizeof(TL_TLB_OpcodeType);
	    }
	    break;
	case 1:
	    {
		opCount = ((TLint4) ((* (TLnat2 *) ((unsigned long) (*pattern) + sizeof(TL_TLB_OpcodeType)))) << 16) + ((* (TLnat2 *) ((unsigned long) (*pattern) + (2 * sizeof(TL_TLB_OpcodeType)))));
		(*pattern) += 3 * sizeof(TL_TLB_OpcodeType);
	    }
	    break;
    };
    switch ((op & 252) >> 2) {
	case 0:
	    {
		(*variable) += opCount;
	    }
	    break;
	case 1:
	    {
		for(;;) {
		    if (((((* (TL_TLB_OpcodeType *) (*pattern))) & 252) >> 2) == 2) {
			break;
		    };
		    TL_TLB_TLBUNR(&((*variable)), &((*pattern)), (TLint4) 0);
		};
		(*pattern) += sizeof(TL_TLB_OpcodeType);
	    }
	    break;
	case 2:
	    {
	    }
	    break;
	case 3:
	    {
		TLaddressint	origPattern;
		if (opCount == 0) {
		    opCount = count;
		};
		origPattern = (TLaddressint) (*pattern);
		{
		    register TLint4	__x350;
		    TLint4	__x366;
		    __x366 = opCount;
		    __x350 = 1;
		    if (__x350 <= __x366) {
			for(;;) {
			    (*pattern) = (TLaddressint) origPattern;
			    TL_TLB_TLBUNR(&((*variable)), &((*pattern)), (TLint4) 0);
			    if (__x350 == __x366) break;
			    __x350++;
			}
		    };
		};
	    }
	    break;
	case 4:
	    {
		TLaddressint	subPattern;
		subPattern = (TLaddressint) ((unsigned long) opcodeStart - opCount);
		TL_TLB_TLBUNR(&((*variable)), &(subPattern), (TLint4) 0);
	    }
	    break;
	case 10:
	    {
		typedef	TLint4	t;
		(* (TLnat4 *) (*variable)) = 0x80000000;
		(*variable) += sizeof(t);
	    }
	    break;
	case 11:
	    {
		typedef	TLnat4	t;
		(* (TLnat4 *) (*variable)) = 0xFFFFFFFF;
		(*variable) += sizeof(t);
	    }
	    break;
	case 12:
	    {
		typedef	TLint4	t;
		(* (TLnat4 *) (*variable)) = 0x80000000;
		(*variable) += sizeof(t);
		(* (TLnat4 *) (*variable)) = 0x80000000;
		(*variable) += sizeof(t);
	    }
	    break;
	case 13:
	    {
		typedef	TLboolean	t;
		(* (TLint1 *) (*variable)) = -1;
		(*variable) += sizeof(t);
	    }
	    break;
	case 14:
	    {
		switch (opCount) {
		    case 1:
			{
			    (* (TLnat1 *) (*variable)) = 128;
			}
			break;
		    case 2:
			{
			    (* (TLnat2 *) (*variable)) = 32768;
			}
			break;
		    case 4:
			{
			    (* (TLnat4 *) (*variable)) = 0x80000000;
			}
			break;
		    default :
			break;
		};
		(*variable) += opCount;
	    }
	    break;
	case 15:
	    {
		(* (TLnat1 *) (*variable)) = 128;
		(* (TLnat1 *) ((unsigned long) (*variable) + 1)) = 0;
		(*variable) += opCount;
	    }
	    break;
	case 16:
	    {
		if (opCount == 8) {
		    typedef	TLaddressint	t;
		    (* (TLaddressint *) (*variable)) = (TLaddressint) 0xFFFFFFFF;
		    (*variable) += sizeof(t);
		    (* (TLaddressint *) (*variable)) = (TLaddressint) 0;
		    (*variable) += sizeof(t);
		} else {
		    typedef	TLaddressint	t;
		    (* (TLaddressint *) (*variable)) = (TLaddressint) 0xFFFFFFFF;
		    (*variable) += sizeof(t);
		};
	    }
	    break;
    };
}
