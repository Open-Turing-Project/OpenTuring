#include "cinterface"

void String_Erealstr ();

void String_Frealstr ();

void String_Index ();

void String_Length ();

void String_Realstr ();

void String_Repeat ();

void String_Strreal ();

void String_Erealstr (sp)
TLaddressint	sp;
{
    TLSTACKCHECK();
    {
	TLstring	__x23;
	TL_TLS_TLSVES((TLreal8) ((* (TLreal8 *) ((unsigned long) sp + 4))), (TLint4) ((* (TLint4 *) ((unsigned long) sp + 12))), (TLint4) ((* (TLint4 *) ((unsigned long) sp + 16))), (TLint4) ((* (TLint4 *) ((unsigned long) sp + 20))), __x23);
	TLSTRASS(255, (* (TLstring *) ((* (TLaddressint *) sp))), __x23);
    };
}

void String_Frealstr (sp)
TLaddressint	sp;
{
    TLSTACKCHECK();
    {
	TLstring	__x29;
	TL_TLS_TLSVFS((TLreal8) ((* (TLreal8 *) ((unsigned long) sp + 4))), (TLint4) ((* (TLint4 *) ((unsigned long) sp + 12))), (TLint4) ((* (TLint4 *) ((unsigned long) sp + 16))), __x29);
	TLSTRASS(255, (* (TLstring *) ((* (TLaddressint *) sp))), __x29);
    };
}

void String_Index (sp)
TLaddressint	sp;
{
    TLSTACKCHECK();
    (* (TLint4 *) ((* (TLaddressint *) sp))) = TL_TLS_TLSIND(((* (TLstring *) ((* (TLaddressint *) ((unsigned long) sp + 4))))), ((* (TLstring *) ((* (TLaddressint *) ((unsigned long) sp + 8))))));
}

void String_Length (sp)
TLaddressint	sp;
{
    TLSTACKCHECK();
    (* (TLint4 *) ((* (TLaddressint *) sp))) = TL_TLS_TLSLEN(((* (TLstring *) ((* (TLaddressint *) ((unsigned long) sp + 4))))));
}

void String_Realstr (sp)
TLaddressint	sp;
{
    TLSTACKCHECK();
    {
	TLstring	__x44;
	TL_TLS_TLSVRS((TLreal8) ((* (TLreal8 *) ((unsigned long) sp + 4))), (TLint4) ((* (TLint4 *) ((unsigned long) sp + 12))), __x44);
	TLSTRASS(255, (* (TLstring *) ((* (TLaddressint *) sp))), __x44);
    };
}

void String_Repeat (sp)
TLaddressint	sp;
{
    TLSTACKCHECK();
    {
	TLstring	__x50;
	TL_TLS_TLSRPT(((* (TLstring *) ((* (TLaddressint *) ((unsigned long) sp + 4))))), (TLint4) ((* (TLint4 *) ((unsigned long) sp + 8))), __x50);
	TLSTRASS(255, (* (TLstring *) ((* (TLaddressint *) sp))), __x50);
    };
}

void String_Strreal (sp)
TLaddressint	sp;
{
    TLSTACKCHECK();
    (* (TLreal8 *) ((* (TLaddressint *) sp))) = TL_TLS_TLSVS8(((* (TLstring *) ((* (TLaddressint *) ((unsigned long) sp + 4))))));
}

static TLboolean __x55;
void String () {
    if (__x55 == 0) {
	__x55 = 1;
    }
}
