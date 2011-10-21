#include "include\cinterface"
typedef	TLnat4	TL_TL_priority_t;
struct	TL_TL_ExceptionInfo {
    TLint4	quitCode;
    TLint4	libraryQuitCode;
    TLstring	errorMsg;
};
typedef	TLnat1	__x660[1];
struct	TL_TL_HandlerArea {
    TLint4	quitCode;
    TLnat4	lineAndFile;
    TLaddressint	fileTable;
    struct TL_TL_HandlerArea	*nextHandler;
    __x660	savedState;
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
typedef	TLaddressint	TL_TLI___x663[21];
typedef	TL_TLI___x663	TL_TLI_ArgList;
typedef	TLchar	TL_TLI___x666[16];
typedef	TL_TLI___x666	TL_TLI___x665[3];
typedef	struct TL_TLI_StreamEntryType	TL_TLI___x667[33];
extern TL_TLI___x667	TL_TLI_TLIS;
extern TLboolean	TL_TLI_TLIUXS;
extern TLint4	TL_TLI_TLIXSN;
extern TLaddressint	TLIstdin;
extern TLaddressint	TLIstdout;
extern TLaddressint	TLIstderr;
extern TLint4	TL_TLI_TLIARC;
extern TLaddressint	TL_TLI_TLIARV;
typedef	TLchar	TL_TLI___x671[42];
extern TL_TLI___x671	TL_TLI_TLIPXL;
typedef	TLchar	TL_TLI___x672[44];
extern TL_TLI___x672	TL_TLI_TLIPXN;
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

extern void TL_TLI_TLISS ();

extern void TL_TLI_TLIUDUMP ();

extern void TL_TLI_TLIAON ();

extern void TL_TLI_TLIAOFF ();

extern void TL_TLI_TLIFINI ();
extern TLaddressint	TLIstdin;
extern TLaddressint	TLIstdout;
extern TLaddressint	TLIstderr;

extern void TLIsaveFlags ();

extern void TLIrestoreFlags ();

void TL_TLI_TLIAON () {
}

void TL_TLI_TLIAOFF () {
}

void TL_TLI_TLIFINI () {
    TLIrestoreFlags();
}

void TL_TLI_TLIUDUMP () {
}

extern TLaddressint fopen ();

extern void fflush ();

extern void fclose ();

extern TL_Cint fgetc ();

extern void ungetc ();

extern void fputc ();

extern void fputs ();

extern void fseek ();

extern TLint4 ftell ();

extern TL_Cint fread ();

extern TL_Cint fwrite ();

extern void TLIreset ();

static TL_Cint TL_TLI_TLIZ_FgetcAsync (stream)
TLaddressint	stream;
{
    TL_Cint	tmp;
    tmp = fgetc((TLaddressint) stream);
    return (tmp);
    /* NOTREACHED */
}

static TLaddressint TL_TLI_TLIZ_Fopen (fileName, fileMode)
TLstring	fileName;
TLstring	fileMode;
{
//    TLstring	dosFileMode;
    TLaddressint	tmp;
//    {
//	TLstring	__x675;
//	TL_TLS_TLSCAT(fileMode, "b", __x675);
//	TLSTRASS(255, dosFileMode, __x675);
//    };
    tmp = (TLaddressint) fopen(fileName, fileMode);
    return ((TLaddressint) tmp);
    /* NOTREACHED */
}

static void TL_TLI_TLIZ_Fflush (stream)
TLaddressint	stream;
{
    fflush((TLaddressint) stream);
}

static void TL_TLI_TLIZ_Fclose (stream)
TLaddressint	stream;
{
    fclose((TLaddressint) stream);
}

static void TL_TLI_TLIZ_Ungetc (oopsChar, stream)
TL_Cint	oopsChar;
TLaddressint	stream;
{
    ungetc((TL_Cint) oopsChar, (TLaddressint) stream);
}

static void TL_TLI_TLIZ_Fputc (outChar, stream)
TLchar	outChar;
TLaddressint	stream;
{
    fputc((TLchar) outChar, (TLaddressint) stream);
}

static void TL_TLI_TLIZ_Fputs (stringPtr, stream)
TLstring	stringPtr;
TLaddressint	stream;
{
    fputs(stringPtr, (TLaddressint) stream);
}

static void TL_TLI_TLIZ_Fseek (stream, offset, offsetType)
TLaddressint	stream;
TLint4	offset;
TL_Cint	offsetType;
{
    fseek((TLaddressint) stream, (TLint4) offset, (TL_Cint) offsetType);
}

static TLint4 TL_TLI_TLIZ_Ftell (stream)
TLaddressint	stream;
{
    TLint4	tmp;
    tmp = ftell((TLaddressint) stream);
    return (tmp);
    /* NOTREACHED */
}

static TL_Cint TL_TLI_TLIZ_Fread (objAddr, byteSize, objSize, stream)
TLaddressint	objAddr;
TL_Cint	byteSize;
TL_Cint	objSize;
TLaddressint	stream;
{
    TL_Cint	tmp;
    tmp = fread((TLaddressint) objAddr, (TL_Cint) byteSize, (TL_Cint) objSize, (TLaddressint) stream);
    return (tmp);
    /* NOTREACHED */
}

static TL_Cint TL_TLI_TLIZ_Fwrite (objAddr, byteSize, objSize, stream)
TLaddressint	objAddr;
TL_Cint	byteSize;
TL_Cint	objSize;
TLaddressint	stream;
{
    TL_Cint	tmp;
    tmp = fwrite((TLaddressint) objAddr, (TL_Cint) byteSize, (TL_Cint) objSize, (TLaddressint) stream);
    return (tmp);
    /* NOTREACHED */
}

static void TL_TLI_TLIZ_Freset (stream)
TLaddressint	stream;
{
    TLIreset((TLaddressint) stream);
}

void TL_TLI_TLIZ () {
    TL_TLI_TLIS[0].mode = 0xA;
    TL_TLI_TLIS[1].mode = 0x14;
    TL_TLI_TLIS[2].mode = 0x14;
    TLIsaveFlags();
    TL_TLI_TLIFOP = TL_TLI_TLIZ_Fopen;
    TL_TLI_TLIFFL = TL_TLI_TLIZ_Fflush;
    TL_TLI_TLIFCL = TL_TLI_TLIZ_Fclose;
    TL_TLI_TLIFGC = TL_TLI_TLIZ_FgetcAsync;
    TL_TLI_TLIFUG = TL_TLI_TLIZ_Ungetc;
    TL_TLI_TLIFPC = TL_TLI_TLIZ_Fputc;
    TL_TLI_TLIFPS = TL_TLI_TLIZ_Fputs;
    TL_TLI_TLIFSK = TL_TLI_TLIZ_Fseek;
    TL_TLI_TLIFTL = TL_TLI_TLIZ_Ftell;
    TL_TLI_TLIFRE = TL_TLI_TLIZ_Fread;
    TL_TLI_TLIFWR = TL_TLI_TLIZ_Fwrite;
    TL_TLI_TLIFZ = TL_TLI_TLIZ_Freset;
}
