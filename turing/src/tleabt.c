#include "cinterface"
void TLEABT (eNum)
TLint4	eNum;
{

    extern void TL_TLE_TLEABT ();
    TLint4	abortNo;
    TLSTACKCHECK();
    switch (eNum) {
	case 11:
	    {
		abortNo = 1;
	    }
	    break;
	case 12:
	    {
		abortNo = 2;
	    }
	    break;
	case 13:
	    {
		abortNo = 3;
	    }
	    break;
	case 14:
	    {
		abortNo = 4;
	    }
	    break;
	case 15:
	    {
		abortNo = 5;
	    }
	    break;
	case 16:
	    {
		abortNo = 6;
	    }
	    break;
	case 17:
	    {
		abortNo = 7;
	    }
	    break;
	case 18:
	    {
		abortNo = 8;
	    }
	    break;
	case 19:
	    {
		abortNo = 9;
	    }
	    break;
	case 20:
	    {
		abortNo = 10;
	    }
	    break;
	case 21:
	    {
		abortNo = 11;
	    }
	    break;
	case 22:
	    {
		abortNo = 12;
	    }
	    break;
	case 23:
	    {
		abortNo = 13;
	    }
	    break;
	case 24:
	    {
		abortNo = 14;
	    }
	    break;
	case 25:
	    {
		abortNo = 15;
	    }
	    break;
	case 26:
	    {
		abortNo = 16;
	    }
	    break;
	case 27:
	    {
		abortNo = 17;
	    }
	    break;
	case 28:
	    {
		abortNo = 18;
	    }
	    break;
	case 29:
	    {
		abortNo = 19;
	    }
	    break;
	case 30:
	    {
		abortNo = 20;
	    }
	    break;
	case 31:
	    {
		abortNo = 21;
	    }
	    break;
	case 32:
	    {
		abortNo = 22;
	    }
	    break;
	case 34:
	    {
		abortNo = 24;
	    }
	    break;
	case 35:
	    {
		abortNo = 25;
	    }
	    break;
	case 36:
	    {
		abortNo = 26;
	    }
	    break;
	case 37:
	    {
		abortNo = 27;
	    }
	    break;
	case 38:
	    {
		abortNo = 28;
	    }
	    break;
	case 39:
	    {
		abortNo = 29;
	    }
	    break;
	case 40:
	    {
		abortNo = 30;
	    }
	    break;
	case 41:
	    {
		abortNo = 31;
	    }
	    break;
	case 42:
	    {
		abortNo = 32;
	    }
	    break;
	case 43:
	    {
		abortNo = 33;
	    }
	    break;
	case 44:
	    {
		abortNo = 34;
	    }
	    break;
	case 45:
	    {
		abortNo = 35;
	    }
	    break;
	case 50:
	    {
		abortNo = 36;
	    }
	    break;
	case 33:
	    {
		abortNo = 37;
	    }
	    break;
	case 51:
	    {
		abortNo = 38;
	    }
	    break;
	case 113:
	    {
		abortNo = 39;
	    }
	    break;
	case 52:
	    {
		abortNo = 40;
	    }
	    break;
	default :
	    {
		abortNo = 41;
	    }
	    break;
    };
    TL_TLE_TLEABT((TLint4) abortNo);
}
