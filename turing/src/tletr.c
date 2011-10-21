
#include "cinterface"
#include <windows.h>
#include <signal.h>

#include "panic.h"

/*
** Trap handler.
** 
** This routine will be called when a signal (error) occurs.
** We must convert the signal into the exception.
**
** Notes:
**
** We reset the signal handler back to the TrapHandler function,
** and then dispatch the currently active exception handler using TL_TLQUIT
**
** If the default handler is active, this will indicate a hardware
** exception occured within the OOT environment (bad), so a Panic will
** occur which will allow the user to save their files.
**
** The OOT executor sets up a chain of exception handlers. A hardware
** exception while running the executor is blamed (perhaps unfairly) on
** user code, and treated as a fatal error condition.
**
*/

extern vaOkMsgBox(char *, ...);

void TrapHandler(int signalNo)
{
    extern void TL_TLQUIT(char *, long);

    (void) signal(signalNo, TrapHandler);

    switch (signalNo)
    {
    case SIGILL:
	TL_TLQUIT("Illegal instruction", excpSIGILL);
	break;

    case SIGFPE:
	TL_TLQUIT("Floating point exception", excpSIGFPE);
	break;

    case SIGSEGV:
	TL_TLQUIT("General protection (segmentation) fault", excpSIGSEGV);
	break;

    default:
	TL_TLQUIT("Internal System Error - Unexpected signal",
			excpSIGBASE + signalNo);
	break;
    }
}


/*
** Trap signals/exceptions to the 'TrapHandler' routine.
**
** These signals are considered to be Turing Plus exceptions
** and user is allowed to catch these exceptions.
*/
void
TL_TLE_TLETR( )
{
    /* The following signals are considered T+ exceptions */
    // The following commented out by Tom West until we can 
    // distinguish between errors in the Turing environment 
    // and errors in the user's Turing program.
#if 0    
    (void) signal( SIGILL, TrapHandler );
    (void) signal( SIGFPE, TrapHandler );
    (void) signal( SIGSEGV, TrapHandler );

    SetErrorMode(SEM_NOGPFAULTERRORBOX);
#endif    
}


/*
** Reset the trap handling to system defaults.  This is done
** when about to run the default handler, to make sure that
** an incoming signal does not leave us with no handler to
** execute.
*/
void
TL_TLE_TLETR_TLETRR( )
{
#if 0
    if( signal( SIGILL, SIG_IGN ) != SIG_IGN ){
	(void) signal( SIGILL, SIG_DFL );
    }
    if( signal( SIGFPE, SIG_IGN ) != SIG_IGN ){
	(void) signal( SIGFPE, SIG_DFL );
    }
    if( signal( SIGSEGV, SIG_IGN ) != SIG_IGN ){
	(void) signal( SIGSEGV, SIG_DFL );
    }
#endif
}
