#include <signal.h>


extern void TL_TLQUIT( );


/*
** Trap handler.
** This routine will be called when a signal (error) occurs.
** We must convert the signal into the exception.
**
** Notes:
**
** Since we got into this routine via a signal, but we're not going
** to return from it, we must simulate the effect of the return so
** the OS will like us.
**	In BSD systems, this involves resetting the signal mask
**	to re-enable the current signal.
**	In SysV systems, this involves re-catching the signal
**	to re-install the signal handler.
*/

void
TrapHandler ( signalNo )
    int signalNo;
{
    /*
    ** Reset the signal handler.
    */
    (void) signal( signalNo, TrapHandler );

    switch( signalNo ){
	case SIGINT:
	    TL_TLQUIT( "Program terminated", 150+signalNo );
	    break;
	case SIGILL:
	    TL_TLQUIT( "Illegal instruction", 150+signalNo );
	    break;
	case SIGFPE:
	    TL_TLQUIT( "Floating exception", 150+signalNo );
	    break;
	case SIGSEGV:
	    TL_TLQUIT( "Segmentation violation", 150+signalNo );
	    break;
	default:
	    TL_TLQUIT( "Internal Turing+ System Error - Unexpected signal",
		    150+signalNo );
	    break;
    }
    /*NOTREACHED*/
}


/*
** Trap UNIX signals to the 'TrapHandler' routine.
**
** These signals are considered to be Turing Plus exceptions
** and user is allowed to catch these exceptions.
** Since these UNIX signals are exceptions we DO NOT treat them
** like interrupt procedures. (e.g don't use TLKIPENT or TLKIPEXT)
**
** NOTE:
**     User can catch the signals directly but
**     device monitors and interrupt procedures must be used.
*/
void
TL_TLE_TLETR( )
{
    /* If SIGINT is being ignored, continue to ignore it */
    if( signal( SIGINT, SIG_IGN ) != SIG_IGN ){
	(void) signal( SIGINT, TrapHandler );
    }

    /* The following signals are considered T+ exceptions */
    (void) signal( SIGILL, TrapHandler );
    (void) signal( SIGFPE, TrapHandler );
    (void) signal( SIGSEGV, TrapHandler );
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
    if( signal( SIGINT, SIG_IGN ) != SIG_IGN ){
	(void) signal( SIGINT, SIG_DFL );
    }
    if( signal( SIGILL, SIG_IGN ) != SIG_IGN ){
	(void) signal( SIGILL, SIG_DFL );
    }
    if( signal( SIGFPE, SIG_IGN ) != SIG_IGN ){
	(void) signal( SIGFPE, SIG_DFL );
    }
    if( signal( SIGSEGV, SIG_IGN ) != SIG_IGN ){
	(void) signal( SIGSEGV, SIG_DFL );
    }
}
