/**
 * -- miscellaneous useful extra words for FLOATING-EXT
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2003.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2008-04-20 04:46:31 $)
 *
 *  @description
 *      Compatiblity with former standards, miscellaneous useful words.
 *      ... for FLOATING-EXT
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) =
"@(#) $Id: floating-mix.c,v 1.3 2008-04-20 04:46:31 guidod Exp $";
#endif

#define _P4_SOURCE 1

#ifndef P4_NO_FP

#include <pfe/pfe-base.h>
#include <pfe/def-limits.h>

#include <math.h>


/** FP@ ( -- addr )
 * returns the floating point stack pointer
 */
void FXCode (p4_f_p_fetch)
{
    *--SP = (p4cell) FP;
}

/** FP! ( addr -- )
 * sets the floating point stack pointer -
 * this is the inverse of => FP@
 */
void FXCode (p4_f_p_store)
{
    FP = (double *) *SP++;
}

/** F? ( f: a b -- s: a==b )
 */
void FXCode (p4_f_equal)
{
    *--SP = P4_FLAG (FP[1] == FP[0]);
    FP += 2;
}

/** F<> ( f: a b -- s: a!=b )
 */
void FXCode (p4_f_not_equal)
{
    *--SP = P4_FLAG (FP[1] != FP[0]);
    FP += 2;
}

/* void FXCode (p4_f_less_than) // already in [ANS] floating-ext
{
    *--SP = P4_FLAG (FP[1] < FP[0]);
    FP += 2;
} */

void FXCode (p4_f_greater_than)
{
    *--SP = P4_FLAG (FP[1] > FP[0]);
    FP += 2;
}

void FXCode (p4_f_less_than_or_equal)
{
    *--SP = P4_FLAG (FP[1] <= FP[0]);
    FP += 2;
}

void FXCode (p4_f_greater_than_or_equal)
{
    *--SP = P4_FLAG (FP[1] >= FP[0]);
    FP += 2;
}

/* some words Julian Noble found useful, plus a few more */

/** S>F  ( n -- f: x )
 * it's inverse is => F>S - convert a cell parameter to floating-point.
 */
void FXCode (p4_s_to_f)
{
    *--FP = *SP++;
}

/** FTRUNC>S  (f: x -- s: n )
 *
 * The word =>"F>S" was sometimes defined with a different behavior
 * than =>"FTRUNC>S" which is the type-cast behaviour of C according
 * to C99 section 6.3.1.4 - truncation would also match the ANS-Forth
 * specification for =>"F>D".
 *
 * Some systems used =>"F>S" defined to =>"FROUND>S" instead. The pfe
 * provides explicit words for both conversions, the word =>"FROUND>S"
 * and =>"FTRUNC>S" which return single-cell parameters for a floating
 * point number with the conversion method of => FTRUNC or => FROUND.
 *
 * In PFE, =>"F>S" is a synonym pointing to =>"FTRUNC>S" in analogy
 * of the behavior of =>"F>D" where no explicit word exists. The
 * inverse of =>"F>S" is the cast conversion of =>"S>F".
 */
void FXCode (p4_f_trunc_to_s)
{
    *--SP = *FP++;
}

/** FROUND>S (f: x -- s: n)
 * complements =>"FTRUNC>S" for applications that expect =>"F>S" to
 * be defined with a rounding behavior like
 : FROUND>S FROUND FTRUNC>S ;
 */
void FXCode (p4_f_round_to_s)
{
    extern void FXCode (p4_f_round);
    FX (p4_f_round);
/*  FX (p4_f_trunc_to_s); */ *--SP = *FP++;
}

/** FTRUNC (f: x -- x' )
 * truncate towards zero, discard a fractional part. See also =>"FTRUNC>S"
 * conversion and the => FROUND and => FLOOR adaptors.
 : FTRUNC FDUP F0< IF FCEIL ELSE FLOOR THEN ;
 * (When available, uses a single call to C99 trunc() internally)
 */
void FXCode (p4_f_trunc)
{
#  if __STDC_VERSION__+0 > 199900
    *FP = trunc (*FP);
#  else
    if (*FP > 0)
	*FP = floor (*FP);
    else
	*FP = ceil (*FP);
#  endif
}

/** -FROT  (f: x1 x2 x3 -- x3 x1 x2 )
 *
 * F-stack equivalent of => -ROT
 *
 * note, some systems call this work F-ROT,
 * here it is the inverse of => FROT
 */
void FXCode (p4_minus_f_rot)
{
    double h = FP[0];

    FP[0] = FP[1];
    FP[1] = FP[2];
    FP[2] = h;
}

/** FNIP  (f: x1 x2 -- x2 )
 *
 * F-stack equivalent of => NIP
 */
void FXCode (p4_f_nip)
{
   FP[1] = FP[0];
   FP++;
}

/** FTUCK  (f: x1 x2 -- x2 x1 x2 )
 *
 * F-stack equivalent of => TUCK
 */
void FXCode (p4_f_tuck)
{
    --FP;
    FP[0] = FP[1];
    FP[1] = FP[2];
    FP[2] = FP[0];
}

/** 1/F  (f: x -- 1/x )
 */
void FXCode (p4_one_over_f)
{
    *FP = 1.0 / *FP;
}

/** F^2  (f: x -- x^2 )
 */
void FXCode (p4_f_square)
{
    *FP = *FP * *FP;
}

/** F^N  ( u f: x -- x^u )
 * For large exponents, use F** instead.  Of course u=-1 is large.
 */
void FXCode (p4_f_power_n)
{
    p4ucell n = *SP++;

    if ( n == 1 ) return;

    {   double r = 1.0;

        if ( n != 0 )
        {
            double x = *FP;
            double xsq = x * x;

            if ( n & 1 ) r = x;
            for ( n = n/2; n > 0; n-- ) r = r * xsq;
        }
        *FP = r;
    }
}

/** F2/  (f: x -- x/2 )
 */
void FXCode (p4_f_two_slash)
{
    *FP = ldexp (*FP, -1);
}

/** F2*  (f: x -- x*2 )
 */
void FXCode (p4_f_two_star)
{
    *FP = ldexp (*FP, 1);
}

/** F0>  (f: x -- s: flag )
*/
void FXCode (p4_f_zero_greater)
{
    *--SP = P4_FLAG (*FP++ > 0);
}

/** F0<>  (f: x -- s: flag )
*/
void FXCode (p4_f_zero_not_equal)
{
    *--SP = P4_FLAG (*FP++ != 0);
}


P4_LISTWORDSET (floating_misc) [] =
{
    P4_INTO ("FORTH", 0),

    P4_FXco ("FLIT",		 p4_f_literal_execution),
    P4_DVaR ("F0",		 f0),
    P4_DVaR ("FLOAT-INPUT",	 float_input),
    P4_FXco ("FP@",		 p4_f_p_fetch),
    P4_FXco ("FP!",		 p4_f_p_store),

    P4_FXco ("F=",               p4_f_equal),
    P4_FXco ("F<>",              p4_f_not_equal),
    P4_FXco ("F>",               p4_f_greater_than),
    P4_FXco ("F<=",              p4_f_less_than_or_equal),
    P4_FXco ("F>=",              p4_f_greater_than_or_equal),

    /* more useful nonstandard words */
    P4_FXco ("S>F",		 p4_s_to_f),
    P4_FXco ("FTRUNC>S",	 p4_f_trunc_to_s),
    P4_FXco ("FROUND>S",	 p4_f_round_to_s),
    P4_FNYM ("F>S",              "FTRUNC>S"),
    P4_FXco ("FTRUNC",           p4_f_trunc),

    P4_FXco ("-FROT",		 p4_minus_f_rot),
    P4_FNYM ("F-ROT",            "-FROT"),
    P4_FXco ("FNIP",		 p4_f_nip),
    P4_FXco ("FTUCK",		 p4_f_tuck),
    P4_FXco ("1/F",		 p4_one_over_f),
    P4_FXco ("F^2",		 p4_f_square),
    P4_FXco ("F^N",		 p4_f_power_n),
    P4_FXco ("F2/",		 p4_f_two_slash),
    P4_FXco ("F2*",		 p4_f_two_star),
    P4_FXco ("F0>",		 p4_f_zero_greater),
    P4_FXco ("F0<>",		 p4_f_zero_not_equal),
};
P4_COUNTWORDSET (floating_misc, "FLOATING-Misc Compatibility words");

#endif /* _NO_FP */

/*@}*/
/*
 * Local variables:
 * c-file-style: "stroustrup"
 * End:
 */
