#include "cinterface"

void Math_Arctan ();

void Math_Arctand ();

void Math_Ceil ();

void Math_Cos ();

void Math_Cosd ();

void Math_Exp ();

void Math_Floor ();

void Math_Intreal ();

void Math_Ln ();

void Math_Natreal ();

void Math_Rand ();

void Math_Randint ();

void Math_Randnext ();

void Math_Randomize ();

void Math_Randseed ();

void Math_Round ();

void Math_Sign ();

void Math_Sin ();

void Math_Sind ();

void Math_Sqrt ();

void Math_Arctan (sp)
TLaddressint	sp;
{
    TLSTACKCHECK();
    (* (TLreal8 *) ((* (TLaddressint *) sp))) = TL_TLA_TLA8TR((TLreal8) ((* (TLreal8 *) ((unsigned long) sp + 4))));
}

void Math_Arctand (sp)
TLaddressint	sp;
{
    TLSTACKCHECK();
    (* (TLreal8 *) ((* (TLaddressint *) sp))) = TL_TLA_TLA8TD((TLreal8) ((* (TLreal8 *) ((unsigned long) sp + 4))));
}

void Math_Ceil (sp)
TLaddressint	sp;
{
    TLSTACKCHECK();
    (* (TLint4 *) ((* (TLaddressint *) sp))) = TL_TLA_TLA8CL((TLreal8) ((* (TLreal8 *) ((unsigned long) sp + 4))));
}

void Math_Cos (sp)
TLaddressint	sp;
{
    TLSTACKCHECK();
    (* (TLreal8 *) ((* (TLaddressint *) sp))) = TL_TLA_TLA8CR((TLreal8) ((* (TLreal8 *) ((unsigned long) sp + 4))));
}

void Math_Cosd (sp)
TLaddressint	sp;
{
    TLSTACKCHECK();
    (* (TLreal8 *) ((* (TLaddressint *) sp))) = TL_TLA_TLA8CD((TLreal8) ((* (TLreal8 *) ((unsigned long) sp + 4))));
}

void Math_Exp (sp)
TLaddressint	sp;
{
    TLSTACKCHECK();
    (* (TLreal8 *) ((* (TLaddressint *) sp))) = TL_TLA_TLA8XP((TLreal8) ((* (TLreal8 *) ((unsigned long) sp + 4))));
}

void Math_Floor (sp)
TLaddressint	sp;
{
    TLSTACKCHECK();
    (* (TLint4 *) ((* (TLaddressint *) sp))) = TL_TLA_TLA8FL((TLreal8) ((* (TLreal8 *) ((unsigned long) sp + 4))));
}

void Math_Intreal (sp)
TLaddressint	sp;
{
    TLSTACKCHECK();
    (* (TLreal8 *) ((* (TLaddressint *) sp))) = (* (TLint4 *) ((unsigned long) sp + 4));
}

void Math_Ln (sp)
TLaddressint	sp;
{
    TLSTACKCHECK();
    (* (TLreal8 *) ((* (TLaddressint *) sp))) = TL_TLA_TLA8LN((TLreal8) ((* (TLreal8 *) ((unsigned long) sp + 4))));
}

void Math_Natreal (sp)
TLaddressint	sp;
{
    TLSTACKCHECK();
    (* (TLreal8 *) ((* (TLaddressint *) sp))) = (* (TLnat4 *) ((unsigned long) sp + 4));
}

void Math_Rand (sp)
TLaddressint	sp;
{
    TLSTACKCHECK();
    TL_TLA_TLARNR(&((* (TLreal8 *) ((* (TLaddressint *) sp)))));
}

void Math_Randint (sp)
TLaddressint	sp;
{
    TLSTACKCHECK();
    TL_TLA_TLARNI(&((* (TLint4 *) ((* (TLaddressint *) sp)))), (TLint4) ((* (TLint4 *) ((unsigned long) sp + 4))), (TLint4) ((* (TLint4 *) ((unsigned long) sp + 8))));
}

void Math_Randnext (sp)
TLaddressint	sp;
{
    TLSTACKCHECK();
    TL_TLA_TLARSR(&((* (TLreal8 *) ((* (TLaddressint *) sp)))), (TLint4) ( TLRANGECHECK ((* (TLint4 *) ((unsigned long) sp + 4)), 1, 10, 5)));
}

void Math_Randomize (sp)
TLaddressint	sp;
{
    TLSTACKCHECK();
    TL_TLA_TLARNZ();
}

void Math_Randseed (sp)
TLaddressint	sp;
{
    TLSTACKCHECK();
    TL_TLA_TLARSZ((TLint4) ((* (TLint4 *) sp)), (TLint4) ( TLRANGECHECK ((* (TLint4 *) ((unsigned long) sp + 4)), 1, 10, 5)));
}

void Math_Round (sp)
TLaddressint	sp;
{
    TLSTACKCHECK();
    (* (TLint4 *) ((* (TLaddressint *) sp))) = TL_TLA_TLA8RD((TLreal8) ((* (TLreal8 *) ((unsigned long) sp + 4))));
}

void Math_Sign (sp)
TLaddressint	sp;
{
    TLSTACKCHECK();
    (* (TLint4 *) ((* (TLaddressint *) sp))) = TL_TLA_TLA8SG((TLreal8) ((* (TLreal8 *) ((unsigned long) sp + 4))));
}

void Math_Sin (sp)
TLaddressint	sp;
{
    TLSTACKCHECK();
    (* (TLreal8 *) ((* (TLaddressint *) sp))) = TL_TLA_TLA8SR((TLreal8) ((* (TLreal8 *) ((unsigned long) sp + 4))));
}

void Math_Sind (sp)
TLaddressint	sp;
{
    TLSTACKCHECK();
    (* (TLreal8 *) ((* (TLaddressint *) sp))) = TL_TLA_TLA8SD((TLreal8) ((* (TLreal8 *) ((unsigned long) sp + 4))));
}

void Math_Sqrt (sp)
TLaddressint	sp;
{
    TLSTACKCHECK();
    (* (TLreal8 *) ((* (TLaddressint *) sp))) = TL_TLA_TLA8QR((TLreal8) ((* (TLreal8 *) ((unsigned long) sp + 4))));
}

static TLboolean __x99;
void Math () {
    if (__x99 == 0) {
	__x99 = 1;
    }
}
