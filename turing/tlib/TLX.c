/*
 *
 * body "../TL/TLX/TLX.st" module TLX
 *
 */

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <process.h>
#include <malloc.h>

#define CLOCK_SCALE_FACTOR	1000/CLOCKS_PER_SEC

extern time_t time();

static clock_t startTime;


TL_TLX()
{
    /* initialize TLX module here */
    startTime = clock(); 
}

 /* routines to be called before and after calls to UNIX library 
    and system call routines */
extern void TL_TLK_TLKUXRS();
extern void TL_TLK_TLKUXRE();

/*
 * external procedure Date (var d : string)
 *	d := date in form "dd mmm yy"
 */
TL_TLX_TLXDT(result)
char *result;
{
    time_t t ;
    register char *p;

    TL_TLK_TLKUXRS();
    t = time( (time_t *) 0 );
    TL_TLK_TLKUXRE();

    /* form = Sun Sep 16 01:03:52 1973\n\0" */
    /*        012345678911111111112222 2 2  */
    /*		        01234567890123 4 5  */
    TL_TLK_TLKUXRS();
    p = ctime(&t);
    TL_TLK_TLKUXRE();

    result[0] = p[8];	/* dd mmm yy */
    result[1] = p[9];
    result[2] = ' ';
    result[3] = p[4];
    result[4] = p[5];
    result[5] = p[6];
    result[6] = ' ';
    result[7] = p[22];
    result[8] = p[23];
    result[9] = '\0';
}

/*
 * external proc Time(var t : string)
 *	t := time in format "hh:mm:ss"
 */
TL_TLX_TLXTM(result)
char *result;
{
    time_t t ;
    register char *p;

    TL_TLK_TLKUXRS();
    t = time( (time_t *) 0 );
    TL_TLK_TLKUXRE();

    /* form = Sun Sep 16 01:03:52 1973\n\0" */
    /*        012345678911111111112222 2 2  */
    /*		        01234567890123 4 5  */
    TL_TLK_TLKUXRS();
    p = ctime(&t);
    TL_TLK_TLKUXRE();

    result[0] = p[11];	/* hh:mm:ss */
    result[1] = p[12];
    result[2] = p[13];
    result[3] = p[14];
    result[4] = p[15];
    result[5] = p[16];
    result[6] = p[17];
    result[7] = p[18];
    result[8] = '\0';
}

/*
 * external procedure Clock(var c : int)
 *	c := milliseconds of elapsed time.
 */
TL_TLX_TLXCL( result )
    long *result;
{
    clock_t currentTime;

    TL_TLK_TLKUXRS();
    currentTime = clock(); 
    TL_TLK_TLKUXRE();
    *result = (currentTime-startTime)*CLOCK_SCALE_FACTOR; 
}

/*
 * external procedure Sysclock(var c : int)
 *	c := milliseconds of system time.
 */
TL_TLX_TLXSC( result )
    long *result;
{
    clock_t usageTime;

    TL_TLK_TLKUXRS();
    usageTime = clock(); 
    TL_TLK_TLKUXRE();
    *result = usageTime * CLOCK_SCALE_FACTOR;
}



extern char * getenv ();
#define MAXLEN 255

/*
 * external procedure getenv(symbol : string, var result : string)
 */
TL_TLX_TLXGE (symbol, result)
char *symbol;
char *result;
{
    char *v;

    TL_TLK_TLKUXRS();
    if( v = getenv( symbol ) ){
	/* only copy the first MAXLEN bytes */
	(void) strncpy( result, v, MAXLEN );
	result[MAXLEN] = '\0';
    } else {
	result[0] = '\0';
    }
    TL_TLK_TLKUXRE();
}


/*
 * external procedure System(str:string, var ret:int)
 *
 * Execute the command str as if it was typed at the terminal to /bin/sh
 */
extern int system();

TL_TLX_TLXSYS(str, ret)
long *ret;
char *str;
{
    TL_TLK_TLKUXRS();
    *ret = system(str);
    TL_TLK_TLKUXRE();
}


/* 
 * malloc and free
 */

void
TL_TLB_TLBMAL( size, addr )
    unsigned size;
    char **addr;
{

    TL_TLK_TLKUXRS();
    *addr = malloc(size);
    TL_TLK_TLKUXRE();
}

void
TL_TLB_TLBMFR( addr )
    char *addr;
{
    TL_TLK_TLKUXRS();
    free(addr);
    TL_TLK_TLKUXRE();
}


TL_TLX_TLXPID (x)
    long *x;
{
    TL_TLK_TLKUXRS();
    *x = getpid();
    TL_TLK_TLKUXRE();
}

TL_TLX_TLXTIM (x)
    long *x;
{
    TL_TLK_TLKUXRS();
    *x = time( (time_t *) 0 );
    TL_TLK_TLKUXRE();
}



/* procedure atof ( char*, real) */

TL_TLX_TLXATF(str, value)
char * str;
double *value;
{
    TL_TLK_TLKUXRS();
    *value = atof(str);
    TL_TLK_TLKUXRE();
}
