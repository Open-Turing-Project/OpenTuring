
/*
** Convert a double to a string in decimal.
**
** If expformat = 1, ndigits is the number of digits to convert,
** If expformat = 0, ndigits is the number of digits after the
** decimal point to convert.
**
** *Decpnt is set to the position of the decimal point relative
** to the beginning of the string (negative values mean to the
** left of the string).
**
** *Sign is set to 0 if arg >= 0, otherwise 1.
**
** We return a pointer to the string of converted digits.
** The result is rounded to the number of digits specified;
** ties are rounded up
**
** This routine is taken from the C library
** and modified to round correctly.
*/


// #define DEBUG

#ifdef DEBUG
#include <stdio.h>
#endif

extern double modf();


#define	StringLength	255
#define	BufferSize	500
static char buffer[BufferSize];


static char *hex(double v)
{
   static char str[100];

   unsigned long *v1 = (unsigned long *) &v;
   unsigned long *v2 = v1+1;

   sprintf(str, "(0x%lx,0x%lx)", *v1, *v2);
   return str;
}

TL_TLA_TLAVRS (result, arg, ndigits, decpt, sign, expformat, err)
char *result;
double arg;
long ndigits;
long *decpt;
char *sign;
char expformat;
char *err;
{
    register int exponent;
    double intPart;
    register char *dst, *ptr;

#ifdef DEBUG
    FILE *logfp = fopen("dblog.txt", "a");

    fprintf(logfp, "TLAVRS:  Generate string for: %lf %s\n", arg, hex(arg));
#endif

    *err = 0;

    /*
    ** Make sure ndigits is in the range 0 .. StringLength-1.
    */
    if (ndigits > StringLength-1) {
        ndigits = StringLength-1;
    }

    /*
    ** Determine the sign of the value and then replace	it with
    ** an absolute value.
    */
    if (arg < 0) {
        *sign = 1;
        arg = -arg;
    } else {
        *sign = 0;
    }

    /*
    ** Set "arg" to be the fractional part of the value
    ** and "intPart" to be the integral part.
    */
    arg = modf(arg, &intPart);

    /*
    ** Convert the integer part to string format.
    ** Use exponent to count the number of significant digits before
    ** the decimal point (negative or zero if value < 1).
    ** Leave "dst" pointing to the next available place in "result".
    */
    exponent = 0;
    dst = result;
    if (intPart != 0) {
	double digit;

	/*
	** Generate digits and store in backwards order.
	*/
	for (ptr = &buffer[BufferSize]; intPart != 0; exponent++) {
            digit = modf(intPart/10, &intPart);
	    *--ptr = (int)((digit+.03)*10.0) + '0';
#ifdef DEBUG
	    fprintf (logfp, "TLAVRS:  Generated digit %c, rem = %lf %s\n",
		    *ptr, intPart, hex(intPart));
#endif
        }
	/*
	** Now store the as many digits as we can into result.
	*/
	while (ptr < &buffer[BufferSize] && dst <= &result[StringLength-1]) {
	    *dst++ = *ptr++;
	}
    } else if (arg != 0) {
	double temp;

	/*
	** Multiply the fraction to normalize it, thus ignoring
	** leading fractional zeroes, and count the number of
	** insignificant zeroes.
	*/
        while ((temp = arg*10.0) < 1) {
            arg = temp;
            exponent--;
        }
    }

    /*
    ** Save the exponent.
    */
    *decpt = exponent;

    /*
    ** Point ptr to the first truncated digit.
    */
    ptr = &result[ndigits];
    if (!expformat)
        ptr += exponent;

    /*
    ** If we are doing a fixed format conversion and the number
    ** of leading 0's in the fraction is greater than the number
    ** of displayed digits, just return a null string.
    */
    if (ptr < &result[0]) {
        result[0] = '\0';
	*decpt = -ndigits;
	*sign = 0;
#ifdef DEBUG
	fclose(logfp);
#endif
	return;
    }

    /*
    ** Check if we have enough space for the conversion.
    */
    if (ptr > &result[StringLength-1]) {
	*err = 1;
#ifdef DEBUG
	fclose(logfp);
#endif
	return;
    }

    /*
    ** Get as many digits as required from the fraction.
    */
    while (dst <= ptr) {
	double digit;

        arg = modf(arg * 10.0, &digit);
        *dst++ = (int)digit + '0';
#ifdef DEBUG
	fprintf (logfp, "TLAVRS:  Generated fractional digit %c, rem = %lf %s\n",
		*(dst-1), arg, hex(arg));
#endif
    }

    /*
    ** Increment the last digit if the situation requires rounding up,
    ** and then propagate through the fraction. Rounding up is required
    ** if the truncated digits are >= .5 in the last place and it is
    ** positive, or > .5 and number is negative.
    */
    dst = ptr;
    if (*ptr > '5' || (*sign == 0 && *ptr == '5')) {
	for (;;) {
            if (--ptr >= &result[0]) {
		if (*ptr == '9') {
		    /*
		    ** Set the digit to '0' and carry on rounding.
		    */
		    *ptr = '0';
		} else {
		    /*
		    ** Increment the digit and quit rounding.
		    */
		    (*ptr)++;
		    break;
		}
            } else {
		/*
		** The number rounds to an exact power of 10.  At this point,
		** the entire stored string consists of '0's.  Set the first
		** character to a '1' and increment the exponent.
		*/
                if (expformat) {
		    if (dst == &result[0]) {
			/*
			** Whoops.  He's specified ndigits=0, presumably to
			** get the exponent.  Don't round up.
			*/
			break;
		    }
		} else {
		    /*
		    ** Since we want a fixed number of fractional digits,
		    ** we have to tack a '0' onto the end of the string.
		    */
		    *dst++ = '0';
                }
		result[0] = '1';
		(*decpt)++;
		break;
            }
        }
    }
    *dst = '\0';

#ifdef DEBUG
    fprintf (logfp, "TLAVRS: Final value is .%se%d\n\n", result, *decpt);
    fclose(logfp);
#endif

    if (result[0] == '\0') {
	/* number = 0 */
	*sign = 0;
    }
    return;
}
