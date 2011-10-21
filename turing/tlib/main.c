#include "include\cinterface"

void main (argc, argv)
TLint4	argc;
TLaddressint	argv;
{

    extern void TL ();

    extern void TProg ();

    extern void TL_TLK_TLKINI ();

    extern void TL_TLK_TLKFINI ();
    extern TLint4	TL_TLI_TLIARC;
    extern TLaddressint	TL_TLI_TLIARV;

    extern void close ();
    TL_TLI_TLIARC = argc - 1;
    TL_TLI_TLIARV = (TLaddressint) argv;
    {
	register TLint4	fd;
	for (fd = 3; fd <= 255; fd++) {
	    close((TLint4) fd);
	};
    };
    TL();
    TL_TLK_TLKINI();
    TProg();
    TL_TLK_TLKFINI();
}
