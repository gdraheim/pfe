/** 
 *  -- The Optional Floating-Point Word Set
 * 
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE             @(#) %derived_by: guidod %
 *  @version %version: 5.7 %
 *    (%date_modified: Mon Mar 12 10:32:20 2001 %)
 *
 *  @description
 *         The Optional Floating-Point Wordset is not usually
 *         used on embedded platforms. The PFE can be configured
 *         to even not allocate the seperate floating-point stack
 *         that most of the floating-point words refer to.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: floating-ext.c,v 0.30.86.1 2001-03-12 09:32:20 guidod Exp $";
#endif

#define _P4_SOURCE 1
#define _GNU_SOURCE 1            /* glibc's pow10 */

#ifndef P4_NO_FP

#include <pfe/pfe-base.h>
#include <pfe/def-xtra.h>

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <float.h>
#include <math.h>

#include <pfe/def-comp.h>
#include <pfe/double-sub.h>
#include <pfe/_missing.h>

#define CELLBITS	BITSOF (p4cell)

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

#if !defined HAVE_POW10 && !defined PFE_HAVE_POW10
#define pow10(X) pow(10.0,(X))
#endif

/* ------------------------------------------------------------------ */

/**
 *  used in engine
 */
_export int
p4_to_float (char *p, p4cell n, double *r)
{
# if defined USE_STRTOD		/* most systems have good strtod */

    char buf[80], *q;
  
    if (!*p) return 0; 
    /* strtod does crash on vxworks being empty non-null *gud*/

    p4_store_c_string (p, n, buf, sizeof buf);
    if (tolower (buf[n - 1]) == 'e')
        buf[n++] = '0';
    buf[n] = '\0';
    *r = strtod (buf, &q);
    if (q == NULL)
        return 1;
    while (isspace (*q))
        q++;
    return *q == '\0';

# else				/* but some haven't */

    enum state 			/* states of the state machine */
    {
        bpn,			/* before point, maybe sign */
        bp,			/* before point, no more sign (had one) */
        ap,			/* after point */
        exn,			/* exponent, maybe sign */
        ex,			/* exponent, no more sign, yet no digit */
        exd			/* exponent, no more sign, had one digit */
    };
    enum state state = bpn;
    int sign = 1;		/* sign of mantissa */
    long double mant = 0;	/* the mantissa */
    int esign = 1;		/* sign of exponent */
    int exp = 0;		/* the exponent */
    int bdigs = 0;		/* digits before point */
    int scale = 0;		/* number of digits after point */
    
    while (--n >= 0)
    {
        char c = *p++;

        switch (state)
	{
         case bpn:
             switch (c)
             {
              case '-':
                  sign = -1;
              case '+':
                  state = bp;
                  continue;
              case '.':
                  state = ap;
                  continue;
              default:
                  if (isspace (c))
                      continue;
                  if (isdigit (c))
                  {
                      bdigs = 1;
                      mant = c - '0';
                      state = bp;
                      continue;
                  }
             }
             return 0;
         case bp:
             switch (c)
             {
              case '.':
                  state = ap;
                  continue;
#            if 0
              case '-':
                  esign = -1;
              case '+':
                  state = ex;
                  continue;
#            endif
              case 'e':
              case 'E':
                  state = exn;
                  continue;
              default:
                  if (isdigit (c))
                  {
                      bdigs++;
                      mant *= 10;
                      mant += c - '0';
                      continue;
                  }
             }
             return 0;
         case ap:
             switch (c)
             {
              case '-':
                  esign = -1;
              case '+':
                  state = ex;
                  continue;
              case 'e':
              case 'E':
                  state = exn;
                  continue;
              default:
                  if (isdigit (c))
                  {
                      mant *= 10;
                      mant += c - '0';
                      scale--;
                      continue;
                  }
             }
             return 0;
         case exn:
             state = ex;
             switch (c)
             {
              case '-':
                  esign = -1;
              case '+':
                  continue;
              default: ;
             }
         case ex:
             if (isdigit (c))
             {
                 exp = c - '0';
                 state = exd;
                 continue;
             }
             return 0;
         case exd:
             if (isdigit (c))
             {
                 exp *= 10;
                 exp += c - '0';
                 continue;
             }
             return 0;
	}
    }
    *r = sign * mant * pow10 (scale + esign * exp);
    return bdigs - scale > 0;

# endif
}


/**
 */
FCode (p4_d_f_align);

#if defined USE_SSCANF		/* define this if you fully trust your scanf */

/*
 * This is a working solution on most machines.
 * Unfortunately it relies on pretty obscure features of sscanf()
 * which are not truly implemented everywhere.
 */
FCode (p4_to_float)		
{
    char *p, buf[80];
    static char *fmt[] =
    {
        "%lf%n %n%d%n$",
        "%lf%*1[DdEe]%n %n%d%n$",
    };
    int i, n, exp, n1, n2, n3;
    double r;
    
    p = (char *) SP[1];
    n = p4_dash_trailing (p, *SP++);
    if (n == 0)
    {
        *--FP = 0;
        *SP = P4_TRUE;
        return;
    }
    p4_store_c_string (p, n, buf, sizeof buf);
    strcat (buf, "$");
# if defined SYS_EMX
    /* emx' sscanf(), %lf conversion, doesn't read past 0E accepting the
     * "0" as good number when no exponent follows.  Therefore we change
     * the 'E' to 'D', ugly hack but helps. */
    p4_upper (buf, n);
    if (strchr (buf, 'E'))
        *strchr (buf, 'E') = 'D';
# endif
    if (1 == sscanf (buf, "%lf%n$", &r, &n1) 
      && n == n1)
    {
        *--FP = r;
        *SP = P4_TRUE;
        return;
    }
    for (i = 0; i < DIM (fmt); i++)
    {
        switch (sscanf (buf, fmt[i], &r, &n1, &n2, &exp, &n3))
        {
         case 1:
             if (n < n2)
                 break;
             *--FP = r;
             *SP = P4_TRUE;
             return;
         case 2:
             if (n1 != n2 || n < n3)
                 break;
             *--FP = r * pow10 (exp);
             *SP = P4_TRUE;
             return;
        }
    }
    *SP = P4_FALSE;
}

#else

FCode (p4_to_float)	
/*
 * This is an implementation based on a simple state machine.
 * Uses nothing but simple character manipulation and floating point math.
 */
{
    enum state			/* states of the state machine */
    {
        bpn,			/* before point, maybe sign */
        bp,			/* before point, no more sign (had one) */
        ap,			/* after point */
        exn,			/* exponent, maybe sign */
        ex,			/* exponent, no more sign */
        ts			/* trailing space */
    };
    enum state state = bpn;
    int sign = 1;		/* sign of mantissa */
    long double mant = 0;	/* the mantissa */
    int esign = 1;		/* sign of exponent */
    int exp = 0;		/* the exponent */
    int scale = 0;		/* number of digits after point */
    int n = *SP++;		/* string length */
    char *p = (char *) *SP;	/* points to string */

    while (--n >= 0)
    {
        char c = *p++;

        switch (state)
	{
         case bpn:
             switch (c)
             {
              case '-':
                  sign = -1;
              case '+':
                  state = bp;
                  continue;
              case '.':
                  state = ap;
                  continue;
              default:
                  if (isspace (c))
                      continue;
                  if (isdigit (c))
                  {
                      mant = c - '0';
                      state = bp;
                      continue;
                  }
             }
             goto bad;
         case bp:
             switch (c)
             {
              case '.':
                  state = ap;
                  continue;
              case '-':
                  esign = -1;
              case '+':
                  state = ex;
                  continue;
              case 'D':
              case 'd':
              case 'E':
              case 'e':
                  state = exn;
                  continue;
              default:
                  if (isspace (c))
                  {
                      state = ts;
                      continue;
                  }
                  if (isdigit (c))
                  {
                      mant *= 10;
                      mant += c - '0';
                      continue;
                  }
             }
             goto bad;
         case ap:
             switch (c)
             {
              case '-':
                  esign = -1;
              case '+':
                  state = ex;
                  continue;
              case 'D':
              case 'd':
              case 'E':
              case 'e':
                  state = exn;
                  continue;
              default:
                  if (isspace (c))
                  {
                      state = ts;
                      continue;
                  }
                  if (isdigit (c))
                  {
                      mant *= 10;
                      mant += c - '0';
                      scale--;
                      continue;
                  }
             }
             goto bad;
         case exn:
             switch (c)
             {
              case '-':
                  esign = -1;
              case '+':
                  state = ex;
                  continue;
              default:
                  if (isspace (c))
                  {
                      state = ts;
                      continue;
                  }
                  if (isdigit (c))
                  {
                      exp = c - '0';
                      state = ex;
                      continue;
                  }
             }
             goto bad;
         case ex:
             if (isspace (c))
             {
                 state = ts;
                 continue;
             }
             if (isdigit (c))
             {
                 exp *= 10;
                 exp += c - '0';
                 continue;
             }
             goto bad;
         case ts:
             if (isspace (c))
                 continue;
             goto bad;
	}
    }
    *--FP = sign * mant * pow10 (scale + esign * exp);
    *SP = P4_TRUE;
    return;
 bad:
    *SP = P4_FALSE;
    return;
}

#endif

FCode (p4_d_to_f)
{
    int sign;
    double res;
    
    if (SP[0] < 0)
        sign = 1, dnegate ((p4dcell *) &SP[0]);
    else
        sign = 0;
#if Linux /*FIXME:*/
    /* slackware 2.2.0.1 (at least) has a bug in ldexp()  */
    res = (p4ucell) SP[0] * ((double)(1<<31) * 2) + (p4ucell) SP[1];
#else
    res = ldexp ((p4ucell) SP[0], CELLBITS) + (p4ucell) SP[1];
#endif
    SP += 2;
    *--FP = sign ? -res : res;
}

FCode (p4_f_store)
{
    *(double *) *SP++ = *FP++;
}

FCode (p4_f_star)
{
    FP[1] *= FP[0];
    FP++;
}

FCode (p4_f_plus)
{
    FP[1] += FP[0];
    FP++;
}

FCode (p4_f_minus)
{
    FP[1] -= FP[0];
    FP++;
}

FCode (p4_f_slash)
{
    FP[1] /= FP[0];
    FP++;
}

FCode (p4_f_zero_less)
{
    *--SP = P4_FLAG (*FP++ < 0);
}

FCode (p4_f_zero_equal)
{
    *--SP = P4_FLAG (*FP++ == 0);
}

FCode (p4_f_less_than)
{
    *--SP = P4_FLAG (FP[1] < FP[0]);
    FP += 2;
}

FCode (p4_f_to_d)
{
    double a, hi, lo;
    int sign;
    
    if ((a = *FP++) < 0)
        sign = 1, a = -a;
    else
        sign = 0;
    lo = modf (ldexp (a, -CELLBITS), &hi);
    SP -= 2;
    SP[0] = (p4ucell) hi;
    SP[1] = (p4ucell) ldexp (lo, CELLBITS);
    if (sign)
        dnegate ((p4dcell *) &SP[0]);
}

FCode (p4_f_fetch)
{
    *--FP = *(double *) *SP++;
}

FCode (p4_f_constant_RT)
{
    *--FP = *(double *) p4_dfaligned ((p4cell) WP_PFA);
}

FCode (p4_f_constant)
{
    p4_header (p4_f_constant_RT_ , 0);
    FX (p4_d_f_align);
    FX_FCOMMA (*FP++);
}

FCode (p4_f_depth)
{
    *--SP = p4_F0 - FP;
}

FCode (p4_f_drop)
{
    FP++;
}

FCode (p4_f_dup)
{
    FP--;
    FP[0] = FP[1];
}

FCode (p4_f_literal_execution)
{
    *--FP= P4_POP_ (double, IP);
}

FCode (p4_f_literal)
{
    _FX_STATESMART_Q_COMP;
    if (STATESMART)
    {
#if PFE_ALIGNOF_DFLOAT > PFE_ALIGNOF_CELL
        if (P4_DFALIGNED (DP))
            FX_COMPILE2 (p4_f_literal);
#endif
        FX_COMPILE1 (p4_f_literal);
        FX_FCOMMA (*FP++);
    }
}
P4COMPILES2 (p4_f_literal, p4_f_literal_execution, p4_noop,
  P4_SKIPS_FLOAT, P4_DEFAULT_STYLE);

FCode (p4_floor)
{
  *FP = floor (*FP);
}

FCode (p4_f_max)
{
    if (FP[0] > FP[1])
        FP[1] = FP[0];
    FP++;
}

FCode (p4_f_min)
{
    if (FP[0] < FP[1])
        FP[1] = FP[0];
    FP++;
}

FCode (p4_f_negate)
{
    *FP = -*FP;
}

FCode (p4_f_over)
{
    FP--;
    FP[0] = FP[2];
}

FCode (p4_f_rot)
{
    double h = FP[2];
    
    FP[2] = FP[1];
    FP[1] = FP[0];
    FP[0] = h;
}

FCode (p4_f_round)
{
    *FP = floor (*FP + 0.5);
}

FCode (p4_f_swap)
{
    double h = FP[1];
    
    FP[1] = FP[0];
    FP[0] = h;
}

FCode (p4_f_variable_RT)
{
    *--SP = p4_dfaligned ((p4cell) WP_PFA);
}

FCode (p4_f_variable)
{
    p4_header (p4_f_variable_RT_ , 0);
    FX (p4_d_f_align);
    FX_FCOMMA (0.);
}

FCode (p4_represent)		/* with help from Lennart Benshop */
{
    char *p, buf[0x80];
    int u, log, sign;
    double f;
    
    f = *FP++;
    p = (char *) SP[1];
    u = SP[0];
    SP--;
    
    if (f < 0)
        sign = P4_TRUE, f = -f;
    else
        sign = P4_FALSE;
    if (f != 0)
    {
        log = (int) floor (log10 (f)) + 1;
        f *= pow10 (-log);
        if (f + 0.5 * pow10 (-u) >= 1)
            f /= 10, log++;
    }
    else
        log = 0;
    sprintf (buf, "%0.*f", u, f);
    memcpy (p, buf + 2, u);
    
    SP[2] = log;
    SP[1] = sign;
    SP[0] = P4_TRUE;
}

/* ********************************************************************** */
/* Floating point extension words:                                        */
/* ********************************************************************** */

FCode (p4_d_f_align)
{
    while (!P4_DFALIGNED (DP))
        *DP++ = 0;
}

FCode (p4_d_f_aligned)
{
    SP[0] = p4_dfaligned (SP[0]);
}

FCode (p4_d_float_plus)
{
    *SP += sizeof (double);
}

FCode (p4_d_floats)
{
    *SP *= sizeof (double);
}

FCode (p4_f_star_star)
{
    FP[1] = pow (FP[1], FP[0]);
    FP++;
}

FCode (p4_f_dot)
{
    p4_outf ("%.*f ", PRECISION, *FP++);
}

FCode (p4_f_abs)
{
    if (*FP < 0)
        *FP = -*FP;
}

FCode (p4_f_e_dot)			/* with help from Lennart Benshop */
{
    double f = fabs (*FP);
    double h = 0.5 * pow10 (-PRECISION);
    int n;

    if (f == 0)
        n = 0;
    else if (f < 1)
    {
        h = 1 - h;
        for (n = 3; f * pow10 (n) < h; n += 3);
    }else{
        h = 1000 - h;
        for (n = 0; h <= f * pow10 (n); n -= 3);
    }
    p4_outf ("%+*.*fE%+03d ", PRECISION + 5, PRECISION,
      *FP++ * pow10 (n), -n);
}

FCode (p4_f_s_dot)
{
    p4_outf ("%.*E ", PRECISION, *FP++);
}

FCode (p4_f_proximate)
{
    double a, b, c;

    a = FP[2];
    b = FP[1];
    c = FP[0];
    FP += 3;
# if 0
    SP--;
    if (c > 0)
        *SP = P4_FLAG (fabs (a - b) < c);
    else if (c < 0)
        *SP = P4_FLAG (fabs (a - b) < -c * (fabs (a) + fabs (b)));
    else
        *SP = P4_FLAG (memcmp (&a, &b, sizeof (double)) == 0);
    
# else
    *--SP = P4_FLAG
        (c > 0 
          ? fabs (a - b) < c 
          : c < 0 
          ? fabs (a - b) < -c * (fabs (a) + fabs (b))
          : a == b);
# endif
}

FCode (p4_set_precision)
{
    PRECISION = *SP++;
}

FCode (p4_s_f_store)
{
    *(float *) *SP++ = *FP++;
}

FCode (p4_s_f_fetch)
{
    *--FP = *(float *) *SP++;
}

FCode (p4_s_float_plus)
{
    *SP += sizeof (float);
}

FCode (p4_s_floats)
{
    *SP *= sizeof (float);
}

/*-- simple mappings to the ANSI-C library  --*/

FCode (p4_f_acos)	{ *FP = acos (*FP); }
FCode (p4_f_acosh)	{ *FP = acosh (*FP); }
FCode (p4_f_alog)	{ *FP = pow10 (*FP); }
FCode (p4_f_asin)	{ *FP = asin (*FP); }
FCode (p4_f_asinh)	{ *FP = asinh (*FP); }
FCode (p4_f_atan)	{ *FP = atan (*FP); }
FCode (p4_f_atan2)	{ FP [1] = atan2 (FP [1], FP [0]); FP++; }
FCode (p4_f_atanh)	{ *FP = atanh (*FP); }
FCode (p4_f_cos)	{ *FP = cos (*FP); }
FCode (p4_f_cosh)	{ *FP = cosh (*FP); }
FCode (p4_f_exp)	{ *FP = exp (*FP); }
FCode (p4_f_expm1)	{ *FP = exp (*FP) - 1.0; }
FCode (p4_f_ln)	{ *FP = log (*FP); }
FCode (p4_f_lnp1)	{ *FP = log (*FP + 1.0); }
FCode (p4_f_log)	{ *FP = log10 (*FP); }
FCode (p4_f_sin)	{ *FP = sin (*FP); }
FCode (p4_f_sincos)	{ --FP; FP [0] = cos (FP [1]); FP [1] = sin (FP [1]); }
FCode (p4_f_sinh)	{ *FP = sinh (*FP); }
FCode (p4_f_sqrt)	{ *FP = sqrt (*FP); }
FCode (p4_f_tan)	{ *FP = tan (*FP); }
FCode (p4_f_tanh)	{ *FP = tanh (*FP); }

#ifndef P4_NO_FP

/** FP@ ( -- addr )
 * returns the floating point stack pointer 
 */
FCode (p4_f_p_fetch)		
{				
    *--SP = (p4cell) FP;
}

/** FP! ( addr -- )
 * sets the floating point stack pointer -
 * this is the inverse of => FP@
 */
FCode (p4_f_p_store)		
{		
    FP = (double *) *SP++;
}

#endif /* _NO_FP */

/* environment queries */

static FCode (p__floating_stack)
{
    FX_PUSH (P4_opt.flt_stack_size);
}

static FCode (p__max_float)
{
    *--FP = DBL_MAX;
}

/* words not from the ansi'94 forth standard  */


P4_LISTWORDS (floating) =
{
    CO (">FLOAT",	 p4_to_float),
    CO ("D>F",		 p4_d_to_f),
    CO ("F!",		 p4_f_store),
    CO ("F*",		 p4_f_star),
    CO ("F+",		 p4_f_plus),
    CO ("F-",		 p4_f_minus),
    CO ("F/",		 p4_f_slash),
    CO ("F0<",		 p4_f_zero_less),
    CO ("F0=",		 p4_f_zero_equal),
    CO ("F<",		 p4_f_less_than),
    CO ("F>D",		 p4_f_to_d),
    CO ("F@",		 p4_f_fetch),
    CO ("FALIGN",	 p4_d_f_align),
    CO ("FALIGNED",	 p4_d_f_aligned),
    CO ("FCONSTANT",	 p4_f_constant),
    CO ("FDEPTH",	 p4_f_depth),
    CO ("FDROP",	 p4_f_drop),
    CO ("FDUP",		 p4_f_dup),
    CS ("FLITERAL",	 p4_f_literal),
    CO ("FLOAT+",	 p4_d_float_plus),
    CO ("FLOATS",	 p4_d_floats),
    CO ("FLOOR",	 p4_floor),
    CO ("FMAX",		 p4_f_max),
    CO ("FMIN",		 p4_f_min),
    CO ("FNEGATE",	 p4_f_negate),
    CO ("FOVER",	 p4_f_over),
    CO ("FROT",		 p4_f_rot),
    CO ("FROUND",	 p4_f_round),
    CO ("FSWAP",	 p4_f_swap),
    CO ("FVARIABLE",	 p4_f_variable),
    CO ("REPRESENT",	 p4_represent),
    /* floating point extension words */
    CO ("DF!",		 p4_f_store),
    CO ("DF@",		 p4_f_fetch),
    CO ("DFALIGN",	 p4_d_f_align),
    CO ("DFALIGNED",	 p4_d_f_aligned),
    CO ("DFLOAT+",	 p4_d_float_plus),
    CO ("DFLOATS",	 p4_d_floats),
    CO ("F**",		 p4_f_star_star),
    CO ("F.",		 p4_f_dot),
    CO ("FABS",		 p4_f_abs),
    CO ("FACOS",	 p4_f_acos),
    CO ("FACOSH",	 p4_f_acosh),
    CO ("FALOG",	 p4_f_alog),
    CO ("FASIN",	 p4_f_asin),
    CO ("FASINH",	 p4_f_asinh),
    CO ("FATAN",	 p4_f_atan),
    CO ("FATAN2",	 p4_f_atan2),
    CO ("FATANH",	 p4_f_atanh),
    CO ("FCOS",		 p4_f_cos),
    CO ("FCOSH",	 p4_f_cosh),
    CO ("FE.",		 p4_f_e_dot),
    CO ("FEXP",		 p4_f_exp),
    CO ("FEXPM1",	 p4_f_expm1),
    CO ("FLN",		 p4_f_ln),
    CO ("FLNP1",	 p4_f_lnp1),
    CO ("FLOG",		 p4_f_log),
    CO ("FS.",		 p4_f_s_dot),
    CO ("FSIN",		 p4_f_sin),
    CO ("FSINCOS",	 p4_f_sincos),
    CO ("FSINH",	 p4_f_sinh),
    CO ("FSQRT",	 p4_f_sqrt),
    CO ("FTAN",		 p4_f_tan),
    CO ("FTANH",	 p4_f_tanh),
    CO ("F~",		 p4_f_proximate),
    DC ("PRECISION",	 precision),
    CO ("SET-PRECISION", p4_set_precision),
    CO ("SF!",		 p4_s_f_store),
    CO ("SF@",		 p4_s_f_fetch),
    CO ("SFALIGN",	 p4_align),
    CO ("SFALIGNED",	 p4_aligned),
    CO ("SFLOAT+",	 p4_s_float_plus),
    CO ("SFLOATS",	 p4_s_floats),

    /* some non-standard extra words */
    CO ("FLIT",		p4_f_literal_execution), 
    DV ("F0",		f0),
    DV ("FLOAT-INPUT",	float_input),
    CO ("FP@",		p4_f_p_fetch),
    CO ("FP!",		p4_f_p_store),

    P4_INTO ("ENVIRONMENT", 0 ),
    P4_OCON ("FLOATING-EXT",		1994 ),
    P4_FXCO ("FLOATING-STACK",		p__floating_stack ),
    P4_FXCO ("MAX-FLOAT",		p__max_float ),
};
P4_COUNTWORDS (floating, "Floating point + extensions");

#endif /* _NO_FP */

/*@}*/

