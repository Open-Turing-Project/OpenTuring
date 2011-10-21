#include <stdio.h>

// FILE *TLIstdin	= stdin;
// FILE *TLIstdout	= stdout;
// FILE *TLIstderr	= stderr;


static int save_flags;

void
TLIsaveFlags( )
{
/*
    save_flags = fcntl( 0, F_GETFL, 0 );
*/
}

void
TLIrestoreFlags( )
{
/*
    (void) fcntl( 0, F_SETFL, save_flags );
*/
}

void
TLIenableSIGIO( )
{
    /*
    int flags = fcntl( 0, F_GETFL, 0 );
    (void) fcntl( 0, F_SETFL, flags|FASYNC );
    */
}

void
TLIdisableSIGIO( )
{
    /*
    int flags = fcntl( 0, F_GETFL, 0 );
    (void) fcntl( 0, F_SETFL, flags&~FASYNC );
    */
}

char
TLIinputAvail( )
{
    return 0;
}

void
TLIreset( fd )
    FILE *fd;
{
    clearerr( fd );
    fd->_cnt = 0;
}
