
#include "cinterface"
#include <stdio.h>

typedef TLnat4  TL_TL_priority_t;
struct  TL_TL_ExceptionInfo {
    TLint4      quitCode;
    TLint4      libraryQuitCode;
    TLstring    errorMsg;
};
struct  TL_TL_HandlerArea {
    TLint4      quitCode;
    TLnat4      lineAndFile;
    TLaddressint        fileTable;
    struct TL_TL_HandlerArea    *nextHandler;
    jmp_buf savedState;
};
typedef TLnat4  TL_sigmask;
struct  TL_sigstack {
    TLaddressint        ss_sp;
    TLint4      ss_onstack;
};
struct  TL_sigcontext {
    TLint4      sc_onstack;
    TL_sigmask  sc_mask;
    TLint4      sc_sp;
    TLint4      sc_pc;
    TLint4      sc_ps;
};
typedef void    TL_sigproc();
struct  TL_sigvec_t {
    TL_sigproc  (*sv_handler);
    TL_sigmask  sv_mask;
    TLint4      sv_flags;
};
typedef TL_sigmask      TL_TL_hardwarePriority_t;
typedef TL_TL_hardwarePriority_t        TL_TL_lockStatus_t;
typedef TLnat4  TL_TL_lock_t;
struct  TL_TL_link_t {
    struct TL_TL_ProcessDescriptor      *flink;
    struct TL_TL_ProcessDescriptor      *blink;
};
struct  TL_TL_ProcessDescriptor {
    TLnat4      lineAndFile;
    TLaddressint        fileTable;
    TLaddressint        stackLimit;
    TLaddressint        stackPointer;
    struct TL_TL_HandlerArea    *handlerQhead;
    struct TL_TL_HandlerArea    *currentHandler;
    TLaddressint        name;
    struct TL_TL_ExceptionInfo  exception;
    TLnat4      waitParameter;
    struct TL_TL_ProcessDescriptor      *monitorQlink;
    TLnat4      timeOutStatus;
    TLnat4      pid;
    TLaddressint        memoryBase;
    TLnat4      timeoutTime;
    TLnat4      timeoutEpoch;
    struct TL_TL_link_t timeoutQ;
    TLboolean   timedOut;
    TLboolean   pausing;
    TL_TL_priority_t    dispatchPriority;
    struct TL_TL_ProcessDescriptor      *runQlink;
    TLboolean   ready;
    struct TL_TL_ProcessDescriptor      *tsyncWaiter;
    TLnat4      quantum;
    TLnat4      quantumCntr;
    TLnat4      devmonLevel;
    TLaddressint        otherInfo;
};

void TL_TLE_TLEDH(void)
{
    extern struct TL_TL_ProcessDescriptor *TL_TLKPD;

#if 0

    int i;

    FILE *fp = fopen("longjmp.log", "a");

    char *by = (char *) TL_TLKPD->currentHandler->savedState;

    for (i=0; i<sizeof(jmp_buf); i++) {
	char val [10];

	fprintf(fp, "%d ", by[i]);
    }

    fprintf(fp, "\n");

    fclose(fp);
#endif

    longjmp(TL_TLKPD->currentHandler->savedState, 2); 
}
