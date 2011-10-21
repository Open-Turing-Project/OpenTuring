#undef CHECKING
#include "cinterface"

#include <stdio.h>

// #define DEBUG

typedef	TLnat4	TL_TL_priority_t;
struct	TL_TL_ExceptionInfo {
    TLint4	quitCode;
    TLint4	libraryQuitCode;
    TLstring	errorMsg;
};
typedef	TLnat1	__x585[1];
struct	TL_TL_HandlerArea {
    TLint4	quitCode;
    TLnat4	lineAndFile;
    TLaddressint	fileTable;
    struct TL_TL_HandlerArea	*nextHandler;
    __x585	savedState;
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
typedef	TLnat4	TL_TLA___x588[11];
extern TL_TLA___x588	TL_TLA_TLARS;

extern void TL_TLA_TLAV8D ();

extern void TL_TLA_TLAVD8 ();

extern TLint4 TL_TLA_TLA8GX ();

extern TLreal8 TL_TLA_TLA8SX ();

extern TLreal8 TL_TLA_TLA8T ();

extern void TL_TLA_TLA8X ();

extern void TL_TLA_TLARSC ();

extern void TL_TLA_TLAVRS ();

extern void TL_TLA_TLAVES ();

static char *hex(double v)
{
   static char str[100];

   unsigned long *v1 = (unsigned long *) &v;
   unsigned long *v2 = v1+1;

   sprintf(str, "(0x%lx,0x%lx)", *v1, *v2);
   return str;
}

void TL_TLA_TLAVFS (source, width, fWidth, target, suppress, error)
TLreal8	source;
TLint4	width;
TLint4	fWidth;
TLstring	target;
TLboolean	suppress;
TLboolean	*error;
{
#ifdef DEBUG
    FILE *logfp;
#endif

    TLstring	fractionString;
    register TLaddressint	src;
    TLint4	exponent;
    TLboolean	negative;
    TLboolean	giveDecPt;
    TLnat2	widthNeeded;
    register TLaddressint	dst;
    TLSTKCHKSLF(100035);
    TLSETF();
    TLSETL(100035);

#ifdef DEBUG
    logfp = fopen("c:\\winoot\\dblog.txt", "a");
    fprintf(logfp, "TLAVFS:  Generate string for: %lf %s\n",
	     source, hex(source));
#endif

    {
	TLnat4	hiOrder;
	TLnat4	loOrder;
	TLSETL(100040);
	TL_TLA_TLAV8D((TLreal8) source, &(hiOrder), &(loOrder));
	if (((hiOrder & 2146435072) >> 20) == 2047) {
	    TLnat4	mantissa;
	    register TLaddressint	dst;
	    TLSETL(100041);
	    mantissa = (hiOrder & 1048575) >> 0;
	    TLINCL();
	    dst = (TLaddressint) ((unsigned long)target);
	    TLSETL(100044);
	    if ((mantissa == 0) && (loOrder == 0)) {
#ifdef DEBUG
	        fprintf(logfp, "TLAVFS: Code detected: +/- Inf\n");
#endif
		TLSETL(100045);
		{
		    register TLint4	__x572;
		    for (__x572 = width; __x572 >= 4; __x572--) {
			TLSETL(100046);
			(* (TLchar *) dst) = ' ';
			TLINCL();
			dst += 1;
		    };
		};
		TLSETL(100049);
		if (((hiOrder & (TLint4) -0x80000000) >> 31) == 0) {
		    TLSETL(100051);
		    (* (TLchar *) dst) = '+';
		    TLINCL();
		    dst += 1;
		} else {
		    TLSETL(100055);
		    (* (TLchar *) dst) = '-';
		    TLINCL();
		    dst += 1;
		};
		TLSETL(100058);
		(* (TLchar *) dst) = 'I';
		TLINCL();
		dst += 1;
		TLINCL();
		(* (TLchar *) dst) = 'n';
		TLINCL();
		dst += 1;
		TLINCL();
		(* (TLchar *) dst) = 'f';
		TLINCL();
		dst += 1;
	    } else {
#ifdef DEBUG
	        fprintf(logfp, "TLAVFS: Code detected: Nan\n");
#endif
		TLSETL(100066);
		{
		    register TLint4	__x573;
		    for (__x573 = width; __x573 >= 3; __x573--) {
			TLSETL(100067);
			(* (TLchar *) dst) = ' ';
			TLINCL();
			dst += 1;
		    };
		};
		TLSETL(100070);
		(* (TLchar *) dst) = 'N';
		TLINCL();
		dst += 1;
		TLINCL();
		(* (TLchar *) dst) = 'a';
		TLINCL();
		dst += 1;
		TLINCL();
		(* (TLchar *) dst) = 'N';
		TLINCL();
		dst += 1;
	    };
#ifdef DEBUG
	    fclose(logfp);
#endif
	    TLSETL(100077);
	    (* (TLchar *) dst) = '\0';
	    TLINCL();
	    (*error) = 0;
	    TLSETL(100080);
	    TLRESTORELF();
	    return;
	};
    };

#ifdef DEBUG
    fprintf(logfp, "TLAVFS: Passing onto TL_TLA_TLAVRS\n");
    fclose(logfp);
#endif

    TLSETL(100088);
    giveDecPt = 1;
    TLSETL(100095);
    TL_TLA_TLAVRS(fractionString, (TLreal8) source, (TLint4) fWidth, &(exponent), &(negative), (TLboolean) 0, &((*error)));
    if ((*error)) {
	TLSETL(100097);
	TLRESTORELF();
	return;
    };
    TLSETL(100099);
    if (suppress) {
	register TLnat2	fWidthTaken;
	TLSETL(100100);
	fWidthTaken = fWidth;
	TLSETL(100102);
	src = (TLaddressint) (((unsigned long) ((unsigned long)fractionString) + TL_TLS_TLSLEN(fractionString)) - 1);
	TLSETL(100104);
	for(;;) {
	    TLSETL(100104);
	    if (((* (TLchar *) src)) != '0') {
		break;
	    };
	    TLSETL(100105);
	    src -= 1;
	    TLINCL();
	    fWidthTaken -= 1;
	    TLINCL();
	    if (fWidthTaken <= 0) {
		break;
	    };
	};
	TLSETL(100109);
	if (fWidthTaken == 0) {
	    TLSETL(100110);
	    giveDecPt = 0;
	};
	TLSETL(100112);
	src += 1;
	TLINCL();
	(* (TLchar *) src) = '\0';
    };
    TLSETL(100124);
    widthNeeded = TL_TLS_TLSLEN(fractionString);
    TLSETL(100126);
    if (negative) {
	TLSETL(100127);
	widthNeeded += 1;
    };
    TLSETL(100129);
    if (giveDecPt) {
	TLSETL(100130);
	widthNeeded += 1;
    };
    TLSETL(100132);
    if (exponent <= 0) {
	TLSETL(100133);
	widthNeeded += 1 - exponent;
    };
    TLSETL(100136);
    if (widthNeeded > 255) {
	TLSETL(100137);
	(*error) = 1;
	TLSETL(100139);
	TLRESTORELF();
	return;
    };
    TLSETL(100141);
    dst = (TLaddressint) ((unsigned long)target);
    TLSETL(100146);
    {
	register TLint4	__x582;
	for (__x582 = width - widthNeeded; __x582 >= 1; __x582--) {
	    TLSETL(100147);
	    (* (TLchar *) dst) = ' ';
	    TLINCL();
	    dst += 1;
	};
    };
    TLSETL(100154);
    if (negative) {
	TLSETL(100155);
	(* (TLchar *) dst) = '-';
	TLINCL();
	dst += 1;
    };
    TLSETL(100163);
    src = (TLaddressint) ((unsigned long)fractionString);
    TLINCL();
    if (exponent > 0) {
	TLSETL(100165);
	{
	    register TLint4	__x583;
	    for (__x583 = exponent; __x583 >= 1; __x583--) {
		TLSETL(100166);
		(* (TLchar *) dst) = (* (TLchar *) src);
		TLINCL();
		dst += 1;
		TLINCL();
		src += 1;
	    };
	};
	TLSETL(100170);
	if (giveDecPt) {
	    TLSETL(100171);
	    (* (TLchar *) dst) = '.';
	    TLINCL();
	    dst += 1;
	};
    } else {
	TLSETL(100175);
	(* (TLchar *) dst) = '0';
	TLINCL();
	dst += 1;
	TLINCL();
	if (giveDecPt) {
	    TLSETL(100178);
	    (* (TLchar *) dst) = '.';
	    TLINCL();
	    dst += 1;
	    TLINCL();
	    {
		register TLint4	__x584;
		for (__x584 = exponent; __x584 <= -1; __x584++) {
		    TLSETL(100181);
		    (* (TLchar *) dst) = '0';
		    TLINCL();
		    dst += 1;
		};
	    };
	};
    };
    TLSETL(100187);
    for(;;) {
	TLSETL(100187);
	(* (TLchar *) dst) = (* (TLchar *) src);
	TLINCL();
	dst += 1;
	TLINCL();
	if (((* (TLchar *) src)) == '\0') {
	    break;
	};
	TLSETL(100190);
	src += 1;
    };
    TLRESTORELF();
}
