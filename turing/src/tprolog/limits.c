#include "cinterface"

void Limits_Getexp ();

void Limits_Rreb ();

void Limits_Setexp ();

extern TLint4 TL_TLA_TLA8GX ();

extern TLreal8 TL_TLA_TLA8SX ();

extern TLreal8 TL_TLA_TLA8RE ();

void Limits_Getexp (sp)
TLaddressint	sp;
{
    TLSTACKCHECK();
    (* (TLint4 *) ((* (TLaddressint *) sp))) = TL_TLA_TLA8GX((TLreal8) ((* (TLreal8 *) ((unsigned long) sp + 4))));
}

void Limits_Rreb (sp)
TLaddressint	sp;
{
    TLSTACKCHECK();
    (* (TLreal8 *) ((* (TLaddressint *) sp))) = TL_TLA_TLA8RE();
}

void Limits_Setexp (sp)
TLaddressint	sp;
{
    TLSTACKCHECK();
    (* (TLreal8 *) ((* (TLaddressint *) sp))) = TL_TLA_TLA8SX((TLreal8) ((* (TLreal8 *) ((unsigned long) sp + 4))), (TLint4) ((* (TLint4 *) ((unsigned long) sp + 12))));
}

static TLboolean __x40;
void Limits () {
    if (__x40 == 0) {
	__x40 = 1;
    }
}
