/**
 *  -- Complex Arithmetic Word Set
 *     Version 0.8.9
 *
 *     Code derived from the original source:
 *     Copyright (C) 1998 Julian V. Noble.
 *     This copyright notice must be preserved.
 *
 *     Code derived from pfe:
 *     Copyright (C) 1998-2004 Tektronix, Inc.
 *
 *     Code not derived from the above:
 *     Copyright (C) 2002-2006 David N. Williams
 *
 *  @see     GNU LGPL
 *  @author  Julian V. Noble         (modified by $Author: guidod $)
 *  @version $Revision: 1.8 $
 *     (modified $Date: 2008-05-25 11:42:39 $)
 *
 *  @description
 *         This is a port of Julian Noble's complex arithmetic
 *         lexicon to pfe.  There are a few differences, but his
 *         word set is pretty much intact.
 *
 *         This implementation uses the pfe default type for
 *         floats (double), and requires the pfe floating
 *         module.
 *
 *         In particular, it assumes a separate stack for
 *         floats.
 *
 *         It does not construct a separate complex number
 *         stack.
 *
 *         Complex numbers x+iy are stored on the fp stack as
 *         (f: -- x y).
 *
 *         Angles are in radians.
 *
 *         Our code uses higher-accuracy algorithms by William
 *         Kahan [1]. It uses the principal argument, with -pi <
 *         arg <= pi, for ARG, ZSQRT, ZLN, and Z^.  The Kahan
 *         alorithms implement an OpenMath compliant treatment
 *         of principal expressions, branch cuts, and branches
 *         for the elementary functions [2], which is based on
 *         Abramowitz and Stegun [3].
 *
 *         Kahan pays attention to signed zero, where available
 *         in IEEE 754/854 implementations.  We address that in
 *         this file as follows.
 *
 *         1. ZSINH, ZASINH, ZTANH, ZATANH, ZSIN, ZASIN, ZTAN,
 *            and ZATAN conserve the sign of zero, and 1/Z and
 *            the minimal compuation words ZF*, ZF/, FZ*, FZ/,
 *            ZIF*, ZIF/, IFZ*, IFZ/ do the right thing.
 *
 *         2. We would like the analytic functions that are real
 *            and analytic on the real axis to do the right
 *            thing for the sign of the zero imaginary part.
 *            This is not completely tested yet, and we're not
 *            sure it's always practical to implement.
 *
 *         3. For functions having branch cuts, signed zero in
 *            the appropriate x or y input produces correct
 *            values on the cuts.  This is probably the most
 *            important concern.
 *
 *         Kahan also uses IEEE-prescribed exception handling to
 *         avoid spurious overflow, underflow, and divide by
 *         zero signals.  We usually include that.
 */

/* REFERENCES

1. William Kahan, "Branch cuts for complex elementary
   functions", The State of the Art in Numerical Analysis, A.
   Iserles and M.J.D. Powell, eds., Clarendon Press, Oxford,
   1987, pp. 165-211.

2. Robert M. Corless, James H. Davenport, David J. Jeffrey,
   Stephen M. Watt, "'According to Abramowitz and Stegun' or
   arcoth needn't be uncouth", ACM SIGSAM Bulletin, June, 2000,
   pp. 58-65.

3. M. Abramowitz and I. Stegun, Handbook of Mathematical
   Functions with Formulas, Graphs, and Mathematical Tables, US
   Government Printing Office, 10th Printing December 1972,
   Secs. 4.4, 4.6.
*/

/*@{*/

#define _P4_SOURCE 1
#define _GNU_SOURCE 1            /* glibc's pow10 */

#if !defined P4_NO_FP && !defined P4_NO_COMPLEX_EXT

#include <pfe/pfe-base.h>

#if !defined PFE_HAVE_ISINF
#ifdef __GNUC__
#warning "ERROR: no isinf detected, need to skip complex-ext"
#endif
#else

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <float.h>
#include <math.h>
#ifdef PFE_HAVE_FENV_H
#include <fenv.h>
#if __STDC_VERSION__+0 > 199900L
#pragma STDC FENV_ACCESS ON  /* still ignored by gcc? */
#endif
#endif

#include <pfe/option-ext.h>
#include <pfe/def-comp.h>
#include <pfe/double-sub.h>

#include <pfe/complex-ext.h>

#ifndef PFE_HAVE_FENV_H
#ifdef __GNUC__
#warning ERROR: no fenv.h available, overflow/underflow can not be detected
#else
#error    OOPS: no fenv.h available, overflow/underflow can not be detected
#endif
#define FE_INVALID 0
#define FE_UNDERFLOW 0
#define FE_OVERFLOW 0
#define FE_DIVBYZERO 0
typedef int fexcept_t;
/* signbit fegetexceptflag feclearexcept fetestexcept fmax fmin */
#ifndef signbit
inline int signbit(double x) { return x < 0; }
#endif
inline double fmax(double x, double y) { return (x > y) ? x : y; }
inline double fmin(double x, double y) { return (x < y) ? x : y; }
inline int fetestexcept(int excepts) { return 0; }
inline void feclearexcept(int excepts) { /*void*/ }
inline void fegetexceptflag(fexcept_t* flagp, int excepts) { *flagp = 0; }
inline void fesetexceptflag(const fexcept_t* flagp, int excepts) {}
#endif

#define CELLBITS	BITSOF (p4cell)

#ifndef _export
#define _export

/* 19.26 decimal digits is enough for IEEE 754 80-bit extended
   double; Sparc requires 36 */
# ifndef  P4_PI
#  define P4_PI              3.1415926535897932384626433832795028842
# endif
# ifndef  P4_PI_OVER_2
#  define P4_PI_OVER_2       1.5707963267948966192313216916397514421
# endif
# ifndef  P4_1_OVER_SQRT_2
#  define P4_1_OVER_SQRT_2  0.70710678118654752440084436210484903928
# endif
# ifndef  P4_LN_2
#  define P4_LN_2           0.69314718055994530941723212145817656808
# endif
# ifndef  P4_SQRT_2
#  define P4_SQRT_2          1.4142135623730950488016887242096980786
# endif

#endif

/* ------------------------------------------------------------------
 * static helper routines for missing functionality.
 */

#if !defined PFE_HAVE_ACOSH
/*
 * Simple acosh(), asinh(), atanh() for those unfortunates who don't
 * have them. These are oversimplified routines (no error or boundry
 * checking). !!! DONT TRUST THESE ROUTINES !!!
 */
#include <math.h>

static double acosh (double n)
{
    return log (n + sqrt (n * n - 1));
}

static double asinh (double n)
{
    return (n < 0 ? -1.0 : 1.0)
        * log (fabs (n) + sqrt (n * n + 1));
}

static double atanh (double n)
{
    return log (1.0 + ((2.0 * n) / (1.0 - n))) * 0.5;
}

#endif

#if 0
#if !defined HAVE_POW10 && !defined PFE_HAVE_POW10
#define pow10(X) pow(10.0,(X))
#endif
#endif

#ifdef P4_C99_COMPLEX
#include <complex.h>
#undef I
#endif


/* ******************************************************************* */
/* Complex load and store                                              */
/* ******************************************************************* */

/** Z@  ( addr --  f: z )
 */
void FXCode (p4_z_fetch)
{
    double *addr = (double *) *SP++;

    FP -= 2;
    FP[1] = *addr++;
    FP[0] = *addr;
}

/** Z!  ( addr f: z -- )
 */
void FXCode (p4_z_store)
{
    double *addr = (double *) *SP++;

    *addr++ = FP[1];
    *addr = FP[0];
    FP += 2;
}

/** X@  ( zaddr --  f: x )
 */
void FXCode (p4_x_fetch)
{
    *--FP = * (double *) *SP++;
}

/** X!  ( zaddr f: x -- )
 */
void FXCode (p4_x_store)
{
    * (double *) *SP++ = *FP++;
}

/** Y@  ( zaddr --  f: y )
 */
void FXCode (p4_y_fetch)
{
    double *addr = (double *) *SP++;

    *--FP = *(addr + 1);
}

/** Y!  ( zaddr f: x -- )
 */
void FXCode (p4_y_store)
{
    double *addr = (double *) *SP++;

    *(addr + 1) = *FP++;
}

/* ******************************************************************* */
/* Complex fp stack manipulation                                       */
/* ******************************************************************* */

/** Z.  (f: z -- )
 * Emit the complex number, including the sign of zero when
 * =>"signbit()" is available.
 */
void FXCode (p4_z_dot)
{
    if ( signbit (FP[1]) ) p4_outs ("-");
    else p4_outs (" ");
    p4_outf ("%.*f ", (int) PRECISION, fabs (FP[1]));

    if ( signbit (FP[0])) p4_outs ("- i ");
    else p4_outs ("+ i ");
    p4_outf ("%.*f ", (int) PRECISION, fabs (FP[0]));
    FP += 2;
}

/** ZS.  (f: z -- )
 * Emit the complex number in scientific notation, including the
 * sign of zero when =>"signbit()" is available.
 */
void FXCode (p4_z_s_dot)
{
    if ( signbit (FP[1]) ) p4_outs ("-");
    else p4_outs (" ");
    p4_outf ("%.*E ", (int) PRECISION, fabs (FP[1]));

    if ( signbit (FP[0])) p4_outs ("- i ");
    else p4_outs ("+ i ");
    p4_outf ("%.*E ", (int) PRECISION, fabs (FP[0]));
    FP += 2;
}

/** REAL  (f: x y -- x )
 */
void FXCode (p4_real)
{
    FP++ ;
}

/** IMAG  (f: x y -- y )
 */
void FXCode (p4_imag)
{
    FP[1] = FP[0];
    FP++;
}

/** CONJG  (f: x y -- x -y )
 */
void FXCode (p4_conjg)
{
    *FP = -*FP;
}

/** ZDROP  (f: z -- )
 */
void FXCode (p4_z_drop)
{
    FP += 2;
}

/** ZDUP  (f: z -- z z )
 */
void FXCode (p4_z_dup)
{
    FP -= 2;
    FP[0] = FP[2];
    FP[1] = FP[3];
}

/** ZSWAP  (f: z1 z2 -- z2 z1 )
 */
void FXCode (p4_z_swap)
{
    double h;

    h = FP[0];
    FP[0] = FP[2];
    FP[2] = h;
    h = FP[1];
    FP[1] = FP[3];
    FP[3] = h;
}

/** ZOVER  (f: z1 z2 -- z1 z2 z1 )
 */
void FXCode (p4_z_over)
{
    FP -= 2;
    FP[0] = FP[4];
    FP[1] = FP[5];
}

/** ZNIP  (f: z1 z2 -- z2 )
 */
void FXCode (p4_z_nip)
{
   FP[2] = FP[0];
   FP[3] = FP[1];
   FP += 2;
}

/** ZTUCK  (f: z1 z2 -- z2 z1 z2 )
 */
void FXCode (p4_z_tuck)
{
    FP -= 2;
    FP[0] = FP[2];
    FP[1] = FP[3];
    FP[2] = FP[4];
    FP[3] = FP[5];
    FP[4] = FP[0];
    FP[5] = FP[1];
}

/** ZROT  (f: z1 z2 z3 -- z2 z3 z1 )
 */
void FXCode (p4_z_rot)
{
    double h;

    h = FP[4];
    FP[4] = FP[2];
    FP[2] = FP[0];
    FP[0] = h;
    h = FP[5];
    FP[5] = FP[3];
    FP[3] = FP[1];
    FP[1] = h;
}

/** -ZROT  (f: z1 z2 z3 -- z3 z1 z2 )
 */
void FXCode (p4_minus_z_rot)
{
    double h = FP[0];

    FP[0] = FP[2];
    FP[2] = FP[4];
    FP[4] = h;
    h = FP[1];
    FP[1] = FP[3];
    FP[3] = FP[5];
    FP[5] = h;
}


/* ******************************************************************* */
/* Complex arithmetic                                                  */
/* ******************************************************************* */

/** Z+  (f: z1 z2 -- z1+z2 )
 */
void FXCode (p4_z_plus)
{
    FP[2] = FP[0] + FP[2];
    FP[3] = FP[1] + FP[3];
    FP += 2;
}

/** Z-  (f: z1 z2 -- z1-z2 )
 */
void FXCode (p4_z_minus)
{
    FP[2] = FP[2] - FP[0];
    FP[3] = FP[3] - FP[1];
    FP += 2;
}

/** Z*  (f: x y u v -- x*u-y*v  x*v+y*u )
 * Uses the algorithm followed by JVN:
 *     (x+iy)*(u+iv) = [(x+y)*u - y*(u+v)] + i[(x+y)*u + x*(v-u)]
 * Requires 3 multiplications and 5 additions.
 */
void FXCode (p4_z_star)
{
    double h1 = (FP[2] + FP[3]) * FP[1];  /* (y+x)*u */
    double h2 = (FP[0] + FP[1]) * FP[2];  /* (v+u)*y */
    double h3 = (FP[0] - FP[1]) * FP[3];  /* (v-u)*x */

    FP += 2;
    FP[1] = h1 - h2;
    FP[0] = h1 + h3;
}

/** Z/  (f: u+iv z -- u/z+iv/z )
 * Kahan-like algorithm *without* due attention to spurious
 * over/underflows and zeros and infinities.
 */
void FXCode (p4_z_slash)
{
    double u = FP[3], v = FP[2], x = FP[1], y = FP[0];
    double h, r;

    FP += 2;

    if ( fabs (x) > fabs (y) )
    {
        r = y / x;
        h = 1 / ( x + r * y );
        FP[1] = ( u  + v * r ) * h;
        FP[0] = ( -u * r  + v ) * h;
    }
    else
    {
        r = x / y;
        h = 1 / ( y + r * x );
        FP[1] = ( u * r + v ) * h;
        FP[0] = ( -u + v * r ) * h;
    }
}

/** ZNEGATE  (f: z -- -z )
 */
void FXCode (p4_z_negate)
{
    FP[0] = -FP[0];
    FP[1] = -FP[1];
}

/** Z2*  (f: z -- z*2 )
 */
void FXCode (p4_z_two_star)
{
    FP[0] = ldexp (FP[0], 1);
    FP[1] = ldexp (FP[1], 1);
}

/** Z2/  (f: z -- z/2 )
 */
void FXCode (p4_z_two_slash)
{
    FP[0] = ldexp (FP[0], -1);
    FP[1] = ldexp (FP[1], -1);
}

/** I*  (f: x y -- -y x )
 */
void FXCode (p4_i_star)
{
    double h = FP[1];

    FP[1] = -FP[0];
    FP[0] = h;
}

/** -I*  (f: x y -- y -x )
 */
void FXCode (p4_minus_i_star)
{
    double h = FP[1];

    FP[1] = FP[0];
    FP[0] = -h;
}

/** 1/Z  (f: z -- 1/z )
 * Kahan algorithm *without* due attention to spurious
 * over/underflows and zeros and infinities.
 */
void FXCode (p4_one_slash_z)
{
    double h, r;

    if ( fabs (FP[1]) <= fabs (FP[0]) )
    {
        r = FP[1] / FP[0];
        h = 1 / ( FP[0] + r * FP[1] );
        FP[1] = r * h;  FP[0] = -h;
    }
    else
    {
        r = FP[0] / FP[1];
        h = 1 / ( FP[1] + r * FP[0] );
        FP[1] = h;  FP[0] = -r * h;
    }
}

/** Z^2  (f: z -- z^2 )
 * Kahan algorithm without removal of any spurious NaN created
 * by overflow.  It deliberately uses (x-y)(x+y) instead of
 * x^2-y^2 for the real part.
 */
void FXCode (p4_z_hat_two)
{
    double x = FP[1];

    FP[1] = ( x + FP[0] ) * ( x - FP[0] );
    FP[0] = ldexp ( FP[0] * x, 1);  /* 2xy */
}

/** |Z|^2  (f: x y -- |z|^2 )
 */
void FXCode (p4_z_abs_hat_two)
{
    FP[1] = FP[0] * FP[0] + FP[1] * FP[1];
    FP++;
}

/** Z^N  ( n f: z -- z^n )
 */
void FXCode (p4_z_hat_n)
{
    p4ucell n = *SP++;

    if ( n == 1 ) return;

  { double x = 1.0, y = 0.0;

    if ( n != 0 )
    {
        double h1 = FP[1], h2 = FP[0];
        double rsq = ( h1 + h2 ) * ( h1 - h2 );
        double isq = ldexp (h1 * h2, 1);

        if ( n % 2 )
        {
            x = h1;  y = h2;
        }
        for ( n = n/2; n > 0; n-- )
        {
            double h1 = x, h2 = y;

            x = h1 * rsq - h2 * isq;
            y = h1 * isq + h2 * rsq;
        }
    }
    FP[1] = x;  FP[0] = y;
  }
}


/* ******************************************************************* */
/* Minimal (mixed) operations                                          */
/* ******************************************************************* */

/** X+  (f: z a -- x+a y )
 */
void FXCode (p4_x_plus)
{
    FP[2] = FP[0] + FP[2];
    FP += 1;
}

/** X-  (f: z a -- x-a y )
 */
void FXCode (p4_x_minus)
{
    FP[2] = FP[2] - FP[0];
    FP += 1;
}

/** Y+  (f: z a -- x y+a )
 */
void FXCode (p4_y_plus)
{
    FP[1] = FP[1] + FP[0];
    FP += 1;
}

/** Y-  (f: z a -- x y-a )
 */
void FXCode (p4_y_minus)
{
    FP[1] = FP[1] - FP[0];
    FP += 1;
}

/** Z*F  (f: x y f -- x*f y*f )
 */
void FXCode (p4_z_star_f)
{
    double f = *FP++;

    FP[0] *= f ;
    FP[1] *= f ;
}

/** Z/F  (f: x y f -- x/f y/f )
 */
void FXCode (p4_z_slash_f)
{
    double f = *FP++;

    FP[0] /= f ;
    FP[1] /= f ;
}

/** F*Z  (f: f x y -- f*x f*y )
 */
void FXCode (p4_f_star_z)
{
    double f = FP[2];

    FP[2] = f * FP[1]; FP[1] = f * FP[0];
    FP++;
}

/** F/Z  (f: f z -- f/z )
 * Kahan algorithm *without* due attention to spurious
 * over/underflows and zeros and infinities.
 */
void FXCode (p4_f_slash_z)
{
    double y = *FP++, x = *FP;
    double h, r;

    if ( fabs (x) <= fabs (y) )
    {
        r = x / y;
        h = FP[1] / ( y + r * x );
        FP[1] = r * h;  FP[0] = -h;
    }
    else
    {
        r = y / x;
        h = FP[1] / ( x + r * y );
        FP[1] = h;  FP[0] = -r * h;
    }
}

/** Z*I*F  (f: z f --  z*if )
 */
void FXCode (p4_z_star_i_star_f)
{
    double f = *FP++, y = *FP;

    FP[0] = f * FP[1];
    FP[1] = -f * y;
}

/** -I*Z/F  (f: z f --  z/[if] )
 */
void FXCode (p4_minus_i_star_z_slash_f)
{
    double f = *FP++, y = *FP;

    FP[0] = -FP[1] / f;
    FP[1] = y / f;
}

/** I*F*Z  (f: f z -- if*z )
 */
void FXCode (p4_i_star_f_star_z)
{
    double f = FP[2], y = *FP++;

    FP[0] = f * FP[0];
    FP[1] = -f * y;
}

/** I*F/Z  (f: f z -- [0+if]/z )
 * Kahan algorithm *without* due attention to spurious
 * over/underflows and zeros and infinities.
 */
void FXCode (p4_i_star_f_slash_z)
{
    double y = *FP++, x = *FP;
    double h, r;

    if ( fabs (x) <= fabs (y) )
    {
        r = x / y;
        h = FP[1] / ( y + r * x );
        FP[1] = h;  FP[0] = r * h;
    }
    else
    {
        r = y / x;
        h = FP[1] / ( x + r * y );
        FP[1] = r * h;  FP[0] = h;
    }
}

_export double
p4_real_of_one_over_z (double x, double y)
{
    double r;

    if ( fabs (x) <= fabs (y) )
    {
        r = x / y;
        return r / (y + r * x);
    }
    else
    {
        r = y / x;
        return  1 / (x + r * y);
    }
}

_export double
p4_imag_of_one_over_z (double x, double y)
{
    double r;

    if ( fabs (x) <= fabs (y) )
    {
        r = x / y;
        return -1 / (y + r * x);
    }
    else
    {
        r = y / x;
        return -r / (x + r * y);
    }
}

_export double
p4_real_of_z_star (double x1, double y1, double x2, double y2)
{
    return x1 * x2 - y1 * y2;
}

_export double
p4_imag_of_z_star (double x1, double y1, double x2, double y2)
{
    return x1 * y2 + x2 * y1;
}

/** Z*>REAL  (f: z1 z2 -- Re[z1*z2] )
 * Compute the real part of the complex product without
 * computing the imaginary part.  Recommended by Kahan to avoid
 * gratuitous overflow or underflow signals from the unnecessary
 * part.
 */
void FXCode (p4_z_star_to_real)
{
    FP[3] = p4_real_of_z_star ( FP[3], FP[2], FP[1], FP[0]);
    FP += 3;
}

/** Z*>IMAG  (f: z1 z2 -- Im[z1*z2] )
 * Compute the imaginary part of the complex product without
 * computing the real part.
 */
void FXCode (p4_z_star_to_imag)
{
    FP[3] = p4_imag_of_z_star ( FP[3], FP[2], FP[1], FP[0]);
    FP += 3;
}


/* ******************************************************************* */
/* Complex functions                                                   */
/* ******************************************************************* */

#if 1  /* ante C99 */
  #define R2P1  2.414213562373094923430     /* 1+sqrt(2) trunc'd to 53 bits */
  #define T2P1  1.253716717905021982261e-16 /* 1+sqrt(2) - R2P1 */
#else  /* post C99 */
  #define R2P1  0x2.6a09e667f3bccp0   /* 1+sqrt(2) trunc'd to 53 bits */
  #define T2P1  0x0.908b2fb1366ebp-52 /* 1+sqrt(2) - R2P1 */
#endif

/*
 * Kahan, pp. 198, 199.  Claimed to be good to one ulp, without
 * spurious overflow or underflow.  DNW tested hypot() on
 * Darwin, and found it not nearly as good.
 */
_export double
p4_cabs (double x, double y)
{
    double s, t;
    fexcept_t iflag, uflag;

    fegetexceptflag (&iflag, FE_INVALID);
    x = fabs (x);  y = fabs (y);
    if ( x < y )
    {
        s = x;  x = y;  y = s;
    }

    /* x >= y >= 0 if not NaN */
    s = 0.0;

    if (isinf (y))  x = y;
    t = x - y;

    if ( !isinf (x) && t != x )
    {
        /* x <> inf, y <> inf, y not neglible */

        fegetexceptflag (&uflag, FE_UNDERFLOW);
        if ( t > y )
        {
            double u = x/y;

            if ( u < (2.0 / DBL_EPSILON) )
            {
                s = u + sqrt (1.0 + u*u);
            }
        }
        else  /* 1 <= x/y <= 2*/
        {
            s = t / y;
            t = (2.0 + s) * s;
            /* hope the compiler follows this order */
            s = (T2P1 + t / (P4_SQRT_2 + sqrt (2.0 + t)) + s) + R2P1;
        }

        s = y / s ;  /* gradual underflow ok here */
        fesetexceptflag (&uflag, FE_UNDERFLOW);
        /* any overflow now is deserved */
    }

    fesetexceptflag (&iflag, FE_INVALID);
    return x + s;
}

/** |Z|  (f: x y -- |z| )
 */
void FXCode (p4_z_abs)
{
    FP[1] = p4_cabs (FP[1], FP[0]);
    FP++;
}

/** ZBOX  (f: z -- box[z] )
 * Defined *only* for zero and infinite arguments. This difffers
 * from Kahan's =>"CBOX" [p. 198] by conserving signs when only
 * one of x or y is infinite, consistent with the other cases, and
 * with its use in his =>"ARG" [p. 199].
 */
void FXCode (p4_z_box)
{
    double x = FP[1], y = FP[0];

    if ( x == 0 && y == 0 )
    {
        x = copysign (1, x);
    }
    else if ( isinf (x) )
    {
        if ( isinf (y) )
        {
            x = copysign (1, x);
            y = copysign (1, y);
        }
        else
        {
            y = y / fabs (x);
            x = copysign (1, x);
        }
    }
    else if ( isinf (y) )
    {
        x = x / fabs (y);
        y = copysign (1, y);
    }
    else
    {
        x = 0.0 / 0.0;  /* invalid use */
        y = 0.0 / 0.0;
    }
    FP[1] = x;  FP[0] = y;
}

/* Kahan, p. 199.  He says better accuracy can be obtained by
 * further case reduction and identities like atan(y/x) = pi/4 +
 * atan[(y-x)/(y+x)].
 */
_export  double
p4_carg (double x, double y)
{
    double theta;

    if ( x == 0 && y == 0 )  x = copysign (1, x);

    if ( isinf (x) || isinf (y) )
    {
        *--FP = x;  *--FP = y;
        FX (p4_z_box);
        y = *FP++;  x = *FP++;  /* leaves signs unchanged */
    }

    if ( fabs (y) > fabs (x) )
    {
        theta = copysign (P4_PI_OVER_2, y) - atan (x/y);
    }
    else if ( x < 0 )
    {
        theta = copysign (P4_PI, y) + atan (y/x);
    }
    else
    {
        theta = atan (y/x);
    }
    if ( fabs (theta) >= 0.125 )  feclearexcept (FE_UNDERFLOW);
    return theta;
}

/** ARG  (f: z -- principal.arg[z] )
 */
void FXCode (p4_arg)
{
    FP[1] = p4_carg (FP[1], FP[0]);
//    FP[1] = atan2 (FP[0], FP[1]);
    FP += 1;
}

/** >POLAR  (f: x y -- r theta )
 * Convert the complex number z to its polar representation,
 * where theta is the principal argument.
 */
void FXCode (p4_to_polar)
{
    double x = FP[1], y = FP[0];

    FP[0] = p4_carg (x, y);
    FP[1] = p4_cabs (x, y);
}

/** POLAR>  (f: r theta -- x y )
 */
void FXCode (p4_polar_from)
{
    double r = FP[1], theta = FP[0];

    FP[1] = r * cos (theta);
    FP[0] = r * sin (theta);
}

/*
 * Return |(x+iy)/2^k|^2, scaled to avoid overflow or underflow,
 * and set the scaling integer k.  Kahan, p. 200.
 */
_export double
p4_cssqs ( double x, double y, int *k)
{
    int m = 0;
    double rho;
    fexcept_t flags;

    fegetexceptflag (&flags, FE_OVERFLOW | FE_UNDERFLOW);
    feclearexcept (FE_OVERFLOW | FE_UNDERFLOW);

    rho = x * x + y * y;
    if ( (rho != rho || isinf (rho)) && (isinf (x) || isinf (y)) )
    {
      rho = 1.0 / 0.0;
    }
    else if ( fetestexcept (FE_OVERFLOW)
              || ( fetestexcept (FE_UNDERFLOW) && (rho
                  < (4 * (1 - DBL_EPSILON) / DBL_MAX / DBL_EPSILON)) ) )
    {
        m = ilogb ( fmax (fabs (x), fabs (y)) );
        x = scalbn ( x, -m );  y = scalbn ( y, -m );
        rho = x * x + y * y;
    }
    fesetexceptflag (&flags, FE_OVERFLOW | FE_UNDERFLOW);

    *k = m;
    return rho;
}

/** ZSSQS  (f: z -- rho s: k )
 * Compute rho = |(x+iy)/2^k|^2, scaled to avoid overflow or
 * underflow, and leave the scaling integer k.  Kahan, p. 200.
 */
void FXCode (p4_z_ssqs)
{
    double rho;
    int k;

    rho = p4_cssqs (FP[1], FP[0], &k);
    *++FP = rho;
    *--SP = (int) k;
}

/** ZSQRT  (f: z -- sqrt[z] )
 * Compute the principal branch of the square root, with
 * Re sqrt[z] >= 0.  Kahan, p. 201.
 */
void FXCode (p4_z_sqrt)
{
#ifdef P4_C99_COMPLEX
    double complex z;

    z = csqrt ( FP[1] + FP[0]*_Complex_I );
    FP[1] = creal (z); FP[0] = cimag (z);

#else
    double x = FP[1], y = FP[0];

#if 1  /* good to less than 0.5 ulp (half a bit) */
    int k;
    double rho = p4_cssqs (x, y, &k);

    if ( x == x )  rho = scalbn ( fabs (x), -k ) + sqrt (rho);

    if ( k % 2 )
    {
        k = ( k - 1 ) / 2;
    }
    else
    {
        k = k/2 - 1 ;
        rho = ldexp (rho, 1);
    }

    /* sqrt( (|z| + |x|)/2 ) without overflow or underflow */
    rho = scalbn ( sqrt (rho), k );

#else  /* (superficially?) better (0.0 ulp) in zelefunt at
          nonextreme points, worse at XMAX + i*XMAX */
    double rho = sqrt ( ldexp ( p4_cabs (x, y) + fabs (x), -1 ) );
#endif

    {   double rx = rho , ry = y;

        if ( rho != 0 )
        {
            if ( !isinf (ry) )
            {
                ry = ldexp ( ry / rho, -1 );
                /* signal expected for ry underflow */
            }

            if ( x < 0 )
            {
                rx = fabs (ry) ;
                ry = copysign (rho, y);
            }
        }
        FP[1] = rx;  FP[0] = ry;
    }
#endif
}

/** ZLN  (f: z -- ln|z|+i*theta )
 * Compute the principal branch of the complex natural
 * logarithm. The angle theta is the principal argument.  This
 * code uses Kahan's algorithm for the scaled logarithm
 * =>"CLOGS(z,J)" = ln(z*2^J), with J=0 and blind choices of the
 * threshholds T0, T1, and T2.  Namely, T0 = 1/sqrt(2), T1 =
 * 5/4, and T2 = 3;
 */
void FXCode (p4_z_ln)
{
#ifdef P4_C99_COMPLEX
    double complex z;

    z = clog ( FP[1] + FP[0]*_Complex_I );
    FP[1] = creal (z); FP[0] = cimag (z);

#else
    double x = FP[1], y = FP[0], hmax, hmin;
    int k;
    double rho  = p4_cssqs (x, y, &k);

    FP[0] = p4_carg (x, y);

    x = fabs (x);  y = fabs (y);
    hmax = fmax (x, y);
    hmin = fmin (x, y);

    if ( (k == 0) && (P4_1_OVER_SQRT_2 < hmax)
                && ( (hmax <= 5.0/4.0) || (rho < 3.0) ) )
    {
        FP[1] = ldexp ( log1p ( (hmax - 1.0) * ( hmax + 1.0)
                        + hmin * hmin ), -1 );
    }
    else
    {
        FP[1] = ldexp ( log (rho), -1 ) + k * P4_LN_2;
    }
#endif
}

/** ZEXP  (f: z -- exp[z] )
 */
void FXCode (p4_z_exp)
{
#ifdef P4_C99_COMPLEX
    double complex z;

    z = cexp ( FP[1] + FP[0]*_Complex_I );
    FP[1] = creal (z); FP[0] = cimag (z);
#else
    double x = FP[1], y = FP[0], expx = exp (x);

    FP[1] = expx * cos (y);
    FP[0] = expx * sin (y);
#endif
}

/** Z^  (f: x y u v -- [x+iy]^[u+iv] )
 * Compute in terms of the principal argument of x+iy.
 */
void FXCode (p4_z_hat)
{
#ifdef P4_C99_COMPLEX
    double complex z;

    z = cpow ( FP[3] + FP[2]*_Complex_I, FP[1] + FP[0]*_Complex_I );
    FP += 2;
    FP[1] = creal (z); FP[0] = cimag (z);

#else
    double x = FP[3], y = FP[2], lnr, theta, mod, angle;

    theta = p4_carg (x, y);
    lnr = log ( p4_cabs (y, x) );
    angle = FP[1] * theta + FP[0] * lnr;
    mod = exp (FP[1] * lnr - FP[0] * theta);
    FP += 2;
    FP[1] = mod * cos (angle);
    FP[0] = mod * sin (angle);
#endif
}

/** ZCOSH  (f: z -- cosh[z] )
 */
void FXCode (p4_z_cosh)
{
#ifdef P4_C99_COMPLEX
    double complex z;

    z = ccosh ( FP[1] + FP[0]*_Complex_I );
    FP[1] = creal (z); FP[0] = cimag (z);

#else
    double x = FP[1], y = FP[0];

    FP[1] = cosh (x) * cos (y);
    FP[0] = sinh (x) * sin (y);
#endif
}

/** ZSINH  (f: z -- sinh[z] )
 */
void FXCode (p4_z_sinh)
{
#ifdef P4_C99_COMPLEX
    double complex z;

    z = csinh ( FP[1] + FP[0]*_Complex_I );
    FP[1] = creal (z); FP[0] = cimag (z);

#else
    double x = FP[1], y = FP[0];

    FP[1] = sinh (x) * cos (y);
    FP[0] = cosh (x) * sin (y);
#endif
}

/** ZTANH  (f: z -- tanh[z] )
 * Kahan, p. 204, including his divide by zero signal
 * suppression for infinite values of =>"tan()".  To quote the very
 * informative "=>'man math'" on our Darwin system about IEEE 754:
 * "Divide-by-Zero is signaled only when a function takes
 * exactly infinite values at finite operands."
 */
void FXCode (p4_z_tanh)
{
    double x = FP[1], y = FP[0];
    double beta, rho;
    fexcept_t flag;

    if ( fabs (x) > PFE.asinh_MAX_over_4 )
    {
        FP[1] = copysign (1, x);
        FP[0] = copysign (0, y);
    }
    else
    {
#define sinhx x
#define tany y
        fegetexceptflag (&flag, FE_DIVBYZERO);
        tany = tan (y);
        fesetexceptflag (&flag, FE_DIVBYZERO);
        beta = 1 + tany*tany;  /* 1/cos(x)^2 */
        sinhx = sinh (x);
        rho = sqrt (1 + sinhx*sinhx);  /* cosh(x) */

        if ( isinf (tany) )
        {
            FP[1] = rho/sinhx;  /* signal ok if sinhx = 0 */
            FP[0] = 1/tany;
        }
        else
        {   double h = 1 + beta*sinhx*sinhx;

            FP[1] = beta*rho*sinhx/h;
            FP[0] = tany/h;
        }
#undef sinhx
#undef tany
    }
}

/** ZCOTH  (f: z -- 1/tanh[z] )
 */
void FXCode (p4_z_coth)
{
    FX (p4_z_tanh);
    FX (p4_one_slash_z);
}

/** ZCOS  (f: z -- cosh[i*z] )
 */
void FXCode (p4_z_cos)
{
    double h = FP[1];

    FP[1] = FP[0];
    FP[0] = -h;
    FX (p4_z_cosh);
}

/** ZSIN  (f: z -- -i*sinh[i*z] )
 */
void FXCode (p4_z_sin)
{
    double h = FP[1];

    FP[1] = FP[0];
    FP[0] = -h;
    FX (p4_z_sinh);
    h = FP[0];
    FP[0] = FP[1];
    FP[1] = -h;
}

/** ZTAN  (f: z -- -i*tanh[i*z] )
 */
void FXCode (p4_z_tan)
{
    double h = FP[1];

    FP[1] = FP[0];
    FP[0] = -h;
    FX (p4_z_tanh);
    h = FP[0];
    FP[0] = FP[1];
    FP[1] = -h;
}

/** ZCOT  (f: z -- -i*coth[-i*z] )
 */
void FXCode (p4_z_cot)
{
    double h = FP[1];

    FP[1] = FP[0];
    FP[0] = -h;
    FX (p4_z_coth);
    h = FP[1];
    FP[1] = FP[0];
    FP[0] = -h;
}


/* ******************************************************************* */
/* Complex inverse functions                                           */
/* ******************************************************************* */

/** ZACOS  (f: z -- u+iv=acos[z] )
 * Kahan, p.202.
 */
void FXCode (p4_z_acos)
{
    double x = FP[1], y = FP[0];
    double h = 1 + x;
    fexcept_t flag;
    int xleqm1 = (h <= 0);

    FP -= 2;
    FP[1] = h;  FP[0] = y;
    FX (p4_z_sqrt);  /* sqrt(1+z) */

    FP -= 2;
    FP[1] = 1 - x;  FP[0] = -y;
    FX (p4_z_sqrt);  /* sqrt(1-z) */

    if ( xleqm1 )  fegetexceptflag (&flag, FE_DIVBYZERO);
    FP[5] = ldexp (atan (FP[1]/FP[3]) , 1);
    if ( xleqm1 )  fesetexceptflag (&flag, FE_DIVBYZERO);

    FP[4] = asinh ( p4_imag_of_z_star (FP[3], -FP[2], FP[1], FP[0]) );

    FP += 4;
}

/** ZACOSH  (f: z -- u+iv=acosh[z] )
 * Kahan, p.203.
 */
void FXCode (p4_z_acosh)
{
    double x = FP[1], y = FP[0];
    double h = 1 + x;
    fexcept_t flag;
    int xleqm1 = (h <= 0);

    FP -= 2;
    FP[1] = x - 1;  FP[0] = y;
    FX (p4_z_sqrt);  /* sqrt(z-1) */

    FP -= 2;
    FP[1] = h ;  FP[0] = y;
    FX (p4_z_sqrt);  /* sqrt(z+1) */

    if ( xleqm1 )  fegetexceptflag (&flag, FE_DIVBYZERO);
    FP[4] = ldexp (atan (FP[2]/FP[1]) , 1);
    if ( xleqm1 )  fesetexceptflag (&flag, FE_DIVBYZERO);

    FP[5] = asinh ( p4_real_of_z_star (FP[3], -FP[2], FP[1], FP[0]) );

    FP += 4;
}

/** ZASIN  (f: z -- u+iv=asin[z] )
 * Kahan, p.203.
 */
void FXCode (p4_z_asin)
{
    double x = FP[1], y = FP[0];
    double h = 1 + x;
    fexcept_t flag;
    int xleqm1 = (h <= 0);

    FP -= 2;
    FP[1] = 1 - x;  FP[0] = -y;
    FX (p4_z_sqrt);  /* sqrt(1-z) */

    FP -= 2;
    FP[1] = h ;  FP[0] = y;
    FX (p4_z_sqrt);  /* sqrt(z+1) */

    if ( xleqm1 )  fegetexceptflag (&flag, FE_DIVBYZERO);
    FP[5] = atan (x / p4_real_of_z_star (FP[3], FP[2], FP[1], FP[0]) );
    if ( xleqm1 )  fesetexceptflag (&flag, FE_DIVBYZERO);

    FP[4] = asinh ( p4_imag_of_z_star (FP[3], -FP[2], FP[1], FP[0]) );

    FP += 4;
}

/** ZASINH  (f: z -- -i*asin[i*z] )
 * Kahan, p. 203, couth.
 */
void FXCode (p4_z_asinh)
{
    double h = FP[1];

    FP[1] = -FP[0];  FP[0] = h;
    FX (p4_z_asin);
    h = FP[1];
    FP[1] = FP[0]; FP[0] = -h;
}

/** ZATANH  (f: z -- u+iv=atanh[z] )
 * Kahan, p. 203.
 */
void FXCode (p4_z_atanh)
{
    double x = FP[1], y = FP[0];
    double beta = copysign (1, x);
    double  u, v;
    double theta = PFE.sqrt_MAX_over_4;

    /* cope with unsigned zero */
    x = beta * x ;  y = -y * beta;

    if ( x > theta || fabs (y) > theta )
    {
        u = p4_real_of_one_over_z (x, y);
        v = copysign (P4_PI_OVER_2, y);
    }
    else
    {
        theta = fabs (y) + 1 / theta ;
        if ( x == 1 )
        {
            u = log ( sqrt ( sqrt (4 + y*y) ) / sqrt (theta) );
            v = ldexp ( copysign ( P4_PI_OVER_2
                    + atan ( ldexp (theta, -1) ), y ), -1 );
        }
        else  /* normal case, with ln1p(u) accurate even for tiny u */
        {   double onemx = 1 - x;

            theta = theta * theta;
            u = ldexp ( log1p ( 4*x / (onemx*onemx + theta) ), -2);
            v = ldexp ( p4_carg (onemx * (1 + x) - theta,
                    ldexp (y, 1)), -1);
        }
    }
    FP[1] = beta * u;  FP[0] = -beta * v;
}

/** ZATAN  (f: z -- -i*atanh[i*z] )
 * Kahan, p. 204, couth.
 */
void FXCode (p4_z_atan)
{
    double h = FP[1];

    FP[1] = -FP[0];  FP[0] = h;
    FX (p4_z_atanh);
    h = FP[1];
    FP[1] = FP[0]; FP[0] = -h;
}


/* ******************************************************************* */
/* ZCONSTANT, ZLITERAL, ZVARIABLE                                      */
/* ******************************************************************* */

/**
 * return double float-aligned address
 */
static p4cell
p4_dfaligned (p4cell n)
{
    while (!P4_DFALIGNED (n))
        n++;
    return n;
}

static
void FXCode (p4_d_f_align)
{
    while (!P4_DFALIGNED (DP))
        *DP++ = 0;
}


static p4xcode* p4_z_constant_RT_SEE (char* out, p4xt xt, p4char* nfa)
{
    double* Z = (double*) p4_dfaligned ((p4cell) P4_TO_BODY (xt));
    sprintf (out, "%e %e ZCONSTANT %.*s", Z[0], Z[1], NAMELEN(nfa), NAMEPTR(nfa));
    return 0;
}

void FXCode_RT (p4_z_constant_RT)
{
    FX_USE_BODY_ADDR;
    FX_POP_BODY_ADDR_p4_BODY;

    p4_BODY = (void*) p4_dfaligned ((p4cell) p4_BODY);
    *--FP = P4_PTR_(double *, p4_BODY)[0];
    *--FP = P4_PTR_(double *, p4_BODY)[1];
}

/** ZCONSTANT ( "name" f: z -- )  "name" execution: (f: -- z )
 * Define a word that leaves x+iy on the fp stack upon execution.
 */
void FXCode (p4_z_constant)
{
    FX_RUNTIME_HEADER;
    FX_RUNTIME1 (p4_z_constant);
    FX (p4_d_f_align);
    FX_FCOMMA (FP[1]);
    FX_FCOMMA (FP[0]);
    FP += 2;
}
P4RUNTIMES1_(p4_z_constant, p4_z_constant_RT, 0,p4_z_constant_RT_SEE);

p4xcode*
p4_z_literal_SEE (p4xcode* ip, char* p, p4_Semant* s)
{
# if PFE_ALIGNOF_DFLOAT > PFE_ALIGNOF_CELL
    if (!P4_DFALIGNED (ip))
        ip++;
# endif
    sprintf (p, "%e %e ", *(double *) ip, *((double *) ip + 1) );
    P4_INC (ip, double);
    P4_INC (ip, double);

    return ip;
}

void FXCode_XE (p4_z_literal_execution)
{
    FX_USE_CODE_ADDR;
    *--FP= P4_POP_ (double, IP);
    *--FP= P4_POP_ (double, IP);
    FX_USE_CODE_EXIT;
}

/** ZLITERAL    Compilation: (f: z -- )  Run: (f: -- z )
 */
void FXCode (p4_z_literal)
{
    _FX_STATESMART_Q_COMP;
    if (STATESMART)
    {
#if PFE_ALIGNOF_DFLOAT > PFE_ALIGNOF_CELL
        if (P4_DFALIGNED (DP))
            FX_COMPILE2 (p4_z_literal);
#endif
        FX_COMPILE1 (p4_z_literal);
        FX_FCOMMA (FP[1]);
        FX_FCOMMA (FP[0]);
        FP += 2;
    }
}
P4COMPILES2 (p4_z_literal, p4_z_literal_execution, p4_noop,
             p4_z_literal_SEE, P4_DEFAULT_STYLE);

void FXCode_RT (p4_z_variable_RT)
{
    FX_USE_BODY_ADDR;
    FX_PUSH_SP = p4_dfaligned ((p4cell) FX_POP_BODY_ADDR);
}

/** ZVARIABLE  ( "name" -- )  "name" exection: ( -- zaddr )
 * Allocate aligned memory for an fp complex number, with the
 * real part first in memory, and define a word that leaves
 * the address on the data stack.
 */
void FXCode (p4_z_variable)
{
    FX_RUNTIME_HEADER;
    FX_RUNTIME1 (p4_z_variable);
    FX (p4_d_f_align);
    FX_FCOMMA (0.);
    FX_FCOMMA (0.);
}
P4RUNTIME1(p4_z_variable, p4_z_variable_RT);

/* DEBUG */
static void FXCode (r2p1)
{
    *--FP = R2P1;
}

static void FXCode (t2p1)
{
    *--FP = T2P1;
}

static void FXCode (lambda_slash_epsilon)
{
    *--FP = 4 * (1 - DBL_EPSILON) / DBL_MAX / DBL_EPSILON;
}

#undef complex  /* defined by complex.h */
static void FXCode (complex_init)
{
    PFE.asinh_MAX_over_4 = asinh (DBL_MAX) / 4;
    PFE.sqrt_MAX_over_4 = ldexp (sqrt (DBL_MAX), -2);
}


P4_LISTWORDSET (complex) [] =
{
    P4_NEED ("floating-ext"),
    P4_INTO ("EXTENSIONS", 0),
    P4_FXco ("R2P1",             r2p1),
    P4_FXco ("T2P1",             t2p1),
    P4_FXco ("LAMBDA/EPSILON",   lambda_slash_epsilon),
    /* load, store */
    P4_FXco ("Z@",		 p4_z_fetch),
    P4_FXco ("Z!",		 p4_z_store),
    P4_FXco ("X@",		 p4_x_fetch),
    P4_FXco ("X!",		 p4_x_store),
    P4_FXco ("Y@",		 p4_y_fetch),
    P4_FXco ("Y!",		 p4_y_store),
    /* complex fp stack manipulation */
    P4_FXco ("Z.",		 p4_z_dot),
    P4_FXco ("ZS.",		 p4_z_s_dot),
    P4_FXco ("REAL",		 p4_real),
    P4_FXco ("IMAG",		 p4_imag),
    P4_FXco ("CONJG",		 p4_conjg),
    P4_FXco ("ZDROP",		 p4_z_drop),
    P4_FXco ("ZDUP",		 p4_z_dup),
    P4_FXco ("ZSWAP",		 p4_z_swap),
    P4_FXco ("ZOVER",		 p4_z_over),
    P4_FXco ("ZNIP",		 p4_z_nip),
    P4_FXco ("ZTUCK",		 p4_z_tuck),
    P4_FXco ("ZROT",		 p4_z_rot),
    P4_FXco ("-ZROT",		 p4_minus_z_rot),
    P4_xOLD ("Z-ROT",         "-ZROT"),
    /* complex arithmetic */
    P4_FXco ("Z+",		 p4_z_plus),
    P4_FXco ("Z-",		 p4_z_minus),
    P4_FXco ("Z*",		 p4_z_star),
    P4_FXco ("Z/",		 p4_z_slash),
    P4_FXco ("ZNEGATE",		 p4_z_negate),
    P4_FXco ("Z2*",		 p4_z_two_star),
    P4_FXco ("Z2/",		 p4_z_two_slash),
    P4_FXco ("I*",		 p4_i_star),
    P4_FXco ("-I*",		 p4_minus_i_star),
    P4_xOLD ("(-I)*",		 "-I*"),
    P4_xOLD ("I/",		 "-I*"),
    P4_FXco ("1/Z",		 p4_one_slash_z),
    P4_FXco ("Z^2",		 p4_z_hat_two),
    P4_FXco ("|Z|^2",		 p4_z_abs_hat_two),
    P4_FXco ("Z^N",		 p4_z_hat_n),
    /* minimal (mixed) operations */
    P4_FXco ("X+",		 p4_x_plus),
    P4_FXco ("X-",		 p4_x_minus),
    P4_FXco ("Y+",		 p4_y_plus),
    P4_FXco ("Y-",		 p4_y_minus),
    P4_FXco ("Z*F",		 p4_z_star_f),
    P4_xOLD ("ZF*",		 "Z*F"),
    P4_FXco ("Z/F",		 p4_z_slash_f),
    P4_xOLD ("ZF/",		 "Z/F"),
    P4_FXco ("F*Z",		 p4_f_star_z),
    P4_xOLD ("FZ*",		 "F*Z"),
    P4_FXco ("F/Z",		 p4_f_slash_z),
    P4_xOLD ("FZ/",		 "F/Z"),
    P4_FXco ("Z*I*F",		 p4_z_star_i_star_f),
    P4_xOLD ("ZIF*",		 "Z*I*F"),
    P4_FXco ("-I*Z/F",		 p4_minus_i_star_z_slash_f),
    P4_xOLD ("ZIF/",		 "-I*Z/F"),
    P4_FXco ("I*F*Z",		 p4_i_star_f_star_z),
    P4_xOLD ("IFZ*",		 "I*F*Z"),
    P4_FXco ("I*F/Z",		 p4_i_star_f_slash_z),
    P4_xOLD ("IFZ/",		 "I*F/Z"),
    P4_FXco ("Z*>REAL",		 p4_z_star_to_real),
    P4_xOLD ("Z*REAL",		 "Z*>REAL"),
    P4_FXco ("Z*>IMAG",		 p4_z_star_to_imag),
    P4_xOLD ("Z*IMAG",		 "Z*>IMAG"),
    /* complex functions */
    P4_FXco ("|Z|",		 p4_z_abs),
    P4_FXco ("ZBOX",		 p4_z_box),
    P4_FXco ("ARG",		 p4_arg),
    P4_FXco (">POLAR",		 p4_to_polar),
    P4_FXco ("POLAR>",		 p4_polar_from),
    P4_FXco ("ZSSQS",		 p4_z_ssqs),
    P4_FXco ("ZSQRT",		 p4_z_sqrt),
    P4_FXco ("ZLN",		 p4_z_ln),
    P4_FXco ("ZEXP",		 p4_z_exp),
    P4_FXco ("Z^",		 p4_z_hat),
    P4_FXco ("ZCOSH",		 p4_z_cosh),
    P4_FXco ("ZSINH",		 p4_z_sinh),
    P4_FXco ("ZTANH",		 p4_z_tanh),
    P4_FXco ("ZCOTH",		 p4_z_coth),
    P4_FXco ("ZCOS",		 p4_z_cos),
    P4_FXco ("ZSIN",		 p4_z_sin),
    P4_FXco ("ZTAN",		 p4_z_tan),
    P4_FXco ("ZCOT",		 p4_z_cot),
    /* complex inverse functions */
    P4_FXco ("ZACOS",		 p4_z_acos),
    P4_FXco ("ZACOSH",		 p4_z_acosh),
    P4_FXco ("ZASIN",		 p4_z_asin),
    P4_FXco ("ZASINH",		 p4_z_asinh),
    P4_FXco ("ZATANH",		 p4_z_atanh),
    P4_FXco ("ZATAN",		 p4_z_atan),
    /* zconstant, zliteral, zvariable */
    P4_RTco ("ZCONSTANT",	 p4_z_constant),
    P4_SXco ("ZLITERAL",	 p4_z_literal),
    P4_RTco ("ZVARIABLE",	 p4_z_variable),

    P4_INTO ("ENVIRONMENT",	0 ),
    P4_OCoN ("COMPLEX-EXT",	2006 ),
    P4_XXco ("COMPLEX-INIT",    complex_init),
};
P4_COUNTWORDSET (complex, "Complex floating point");

/* if defined PFE_HAVE_ISINF */
#endif
/* if !defined P4_NO_FP */
#endif

/*@}*/
