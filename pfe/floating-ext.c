/** 
 *  -- The Optional Floating-Point Word Set
 * 
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.9 $
 *     (modified $Date: 2008-05-10 17:04:17 $)
 *
 *  @description
 *         The Optional Floating-Point Wordset is not usually
 *         used on embedded platforms. The PFE can be configured
 *         to even not allocate the separate floating-point stack
 *         that most of the floating-point words refer to.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: floating-ext.c,v 1.9 2008-05-10 17:04:17 guidod Exp $";
#endif

#define _P4_SOURCE 1
#define _GNU_SOURCE 1            /* glibc's pow10 */

#if !defined P4_NO_FP

#include <pfe/pfe-base.h>
#include <pfe/def-limits.h>

#include <stdlib.h>
#include <pfe/os-string.h>
#include <pfe/os-ctype.h>
#include <float.h>
#include <math.h>

#include <pfe/option-ext.h>
#include <pfe/def-comp.h>
#include <pfe/double-sub.h>
/* #include <pfe/_missing.h> */

#define CELLBITS	BITSOF (p4cell)


/* ------------------------------------------------------------------ 
 * Exact comparison of raw floats.  The following is intended to
 * capture the sizes listed for IEEE 754 by William Kahan,
 * "Fclass: a Proposed Classification of Standard Floating-Point
 * Operands", March 2, 2002.  He lists 4, >=6, 8, 10, 12, or 16
 * 8-bit bytes.  We assume that 4, 8, 10, 12, or 16 might
 * correspond to a double, and probably 4 could be omitted.  
 * --KM&DNW 2Mar03
 */
#if PFE_SIZEOF_DOUBLE == PFE_SIZEOF_INT
#  define EXACTLY_EQUAL(A,B)  ( *((int*) &(A)) == *((int*) &(B)) )
#elif PFE_SIZEOF_DOUBLE == 2 * PFE_SIZEOF_INT
#  define EXACTLY_EQUAL(A,B) \
        ( *((int*) &(A)) == *((int*) &(B)) \
       && *(((int*) &(A)) + 1) == *(((int*) &(B)) + 1) )
#elif PFE_SIZEOF_DOUBLE == 2 * PFE_SIZEOF_INT + PFE_SIZEOF_SHORT
#  define EXACTLY_EQUAL(A,B) \
        ( *((int*) &(A)) == *((int*) &(B)) \
       && *(((int*) &(A)) + 1) == *(((int*) &(B)) + 1) \
       && (short)*(((int*) &(A)) + 2) == (short)*(((int*) &(B)) + 2) )
#elif PFE_SIZEOF_DOUBLE == 3 * PFE_SIZEOF_INT
#  define EXACTLY_EQUAL(A,B) \
        ( *((int*) &(A)) == *((int*) &(B)) \
       && *(((int*) &(A)) + 1) == *(((int*) &(B)) + 1) \
       && *(((int*) &(A)) + 2) == *(((int*) &(B)) + 2) )
#elif PFE_SIZEOF_DOUBLE == 4 * PFE_SIZEOF_INT
#  define EXACTLY_EQUAL(A,B) \
        ( *((int*) &(A)) == *((int*) &(B)) \
       && *(((int*) &(A)) + 1) == *(((int*) &(B)) + 1) \
       && *(((int*) &(A)) + 2) == *(((int*) &(B)) + 2) \
       && *(((int*) &(A)) + 3) == *(((int*) &(B)) + 3) )
#else
#  define EXACTLY_EQUAL(A,B)  (p4_memcmp (&(A), &(B), sizeof (double)) == 0)
#  ifdef __GNUC__
#  warning using p4_memcmp() in p4_f_proximate()
#  elif !defined _PFE_FLOATING_USING_MEMCMP
#  error   using p4_memcmp() in p4_f_proximate()
#  endif
#endif

#ifndef USE_STRTOD	/* USER-CONFIG: */
#define USE_STRTOD 1	/* most systems have good strtod */
#endif

#ifndef USE_SSCANF	/* USER-CONFIG: */
#define USE_SSCANF 1	/* define this if you fully trust your scanf */
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

#if !defined HAVE_POW10 && !defined PFE_HAVE_POW10
#define pow10(X) pow(10.0,(X))
#endif

/* ------------------------------------------------------------------ */

/**
 * return double float-aligned address
 */
_export p4cell
p4_dfaligned (p4cell n)	
{
    while (!P4_DFALIGNED (n))
        n++;
    return n;
}

/**
 *  used in engine
 */
_export int
p4_to_float (const p4_char_t *p, p4cell n, double *r)
{
# if USE_STRTOD		/* most systems have good strtod */

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
    while (p4_isspace (*q))
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
        p4_char_t c = *p++;

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
                  if (p4_isspace (c))
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


FCode (p4_d_f_align); /* forward */

#if USE_SSCANF		/* define this if you fully trust your scanf */

/*
 * This is a working solution on most machines.
 * Unfortunately it relies on pretty obscure features of sscanf()
 * which are not truly implemented everywhere.
 */
FCode (p4_to_float)		
{
    char buf[80]; p4_char_t* p;
    static const char *fmt[] =
    {
        "%lf%n %n%d%n$",
        "%lf%*1[DdEe]%n %n%d%n$",
    };
    int i, n, exp, n1, n2, n3;
    double r;
    
    p = (p4_char_t *) SP[1];
    n = p4_dash_trailing (p, *SP++);
    if (n == 0)
    {
        *--FP = 0;
        *SP = P4_TRUE;
        return;
    }
    p4_store_c_string (p, n, buf, sizeof buf);
    p4_strcat (buf, "$");
# if defined SYS_EMX
    /* emx' sscanf(), %lf conversion, doesn't read past 0E accepting the
     * "0" as good number when no exponent follows.  Therefore we change
     * the 'E' to 'D', ugly hack but helps. */
    p4_upper (buf, n);
    if (p4_strchr (buf, 'E'))
        *p4_strchr (buf, 'E') = 'D';
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
        p4char c = *p++;

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
                  if (p4_isspace (c))
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
                  if (p4_isspace (c))
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
                  if (p4_isspace (c))
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
                  if (p4_isspace (c))
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
             if (p4_isspace (c))
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
             if (p4_isspace (c))
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

/** ( f: a -- n,n )
 * b is the integer representation of a
 * 
 * we use truncation towards zero.
 * compare with =>"F>S" and its => "FROUND>S" / => "FTRUNC>S"
 */
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

#if 0
static p4xt* see_f_variable_RT (char* out, p4xt xt, p4char* nfa)
{
    /*else if (*P4_TO_CODE(xt) == PFX (p4_f_variable_RT)) */
    sprintf (out, "%g FVARIABLE %.*s", 
	     *(double *) p4_dfaligned ((p4cell) P4_TO_BODY (xt)),
	     NAMELEN(nfa), NAMEPTR(nfa));
    return 0; /* no colon */
}
#endif

static p4xcode* p4_f_constant_RT_SEE (char* out, p4xt xt, p4char* nfa)
{
    /*  (*P4_TO_CODE(xt) == PFX (p4_f_constant_RT)) */
    sprintf (out, "%g FCONSTANT %.*s", 
	     *(double *) p4_dfaligned ((p4cell) P4_TO_BODY (xt)),
	     NAMELEN(nfa), NAMEPTR(nfa));
    return 0; /* no colon */
}

FCode_RT (p4_f_constant_RT)
{
    FX_USE_BODY_ADDR;
    *--FP = *(double *) p4_dfaligned ((p4cell) FX_POP_BODY_ADDR);
}

FCode (p4_f_constant)
{
    FX_RUNTIME_HEADER;
    FX_RUNTIME1 (p4_f_constant);
    FX (p4_d_f_align);
    FX_FCOMMA (*FP++);
}
P4RUNTIMES1_(p4_f_constant, p4_f_constant_RT, 0,p4_f_constant_RT_SEE);

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

/* originally P4_SKIPS_FLOAT */
p4xcode*
p4_lit_float_SEE (p4xcode* ip, char* p, p4_Semant* s)
{
# if PFE_ALIGNOF_DFLOAT > PFE_ALIGNOF_CELL
    if (!P4_DFALIGNED (ip))
        ip++;
# endif
    sprintf (p, "%e ", *(double *) ip);
    P4_INC (ip, double);
    
    return ip;
}

FCode_XE (p4_f_literal_execution)
{
    FX_USE_CODE_ADDR;
    *--FP= P4_POP_ (double, IP);
    FX_USE_CODE_EXIT;
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
	     p4_lit_float_SEE, P4_DEFAULT_STYLE);

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

#ifndef FROUND_FLOOR                /* USER-CONFIG: */
#define FROUND_FLOOR 0              /* FROUND identical with floor(fp+0.5) ? */
#endif

FCode (p4_f_round)
{
#  if defined HAVE_RINT || defined PFE_HAVE_RINT
    /* correct and fast */
    *FP = rint (*FP);
#  elif FROUND_FLOOR
    /* incorrect but fast */
    *FP = floor (*FP + 0.5); 
#  else
    /* correct but slow */
    double whole, frac, offset;
 
    frac = fabs(modf(*FP, &whole));
    *FP = whole;
    FX(p4_f_to_d);  /* execute F>D */
    offset = (*SP < 0) ? -1. : 1.;
    
    if (*(SP+1) & 1)  /* check even or odd */
    {
	if (frac >= 0.5) whole += offset;
    }
    else
    {
	if (frac > 0.5) whole += offset;
    }
    *--FP = whole; SP += 2;  
#  endif
}

FCode (p4_f_swap)
{
    double h = FP[1];
    
    FP[1] = FP[0];
    FP[0] = h;
}

FCode_RT (p4_f_variable_RT)
{
    FX_USE_BODY_ADDR;
    FX_PUSH_SP = p4_dfaligned ((p4cell) FX_POP_BODY_ADDR);
}

FCode (p4_f_variable)
{
    FX_RUNTIME_HEADER;
    FX_RUNTIME1 (p4_f_variable);
    FX (p4_d_f_align);
    FX_FCOMMA (0.);
}
P4RUNTIME1(p4_f_variable, p4_f_variable_RT); 

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
    p4_memcpy (p, buf + 2, u);
    
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
    p4_outf ("%.*f ", (int) PRECISION, *FP++);
}

FCode (p4_f_abs)
{
  *FP = fabs (*FP);
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
    p4_outf ("%+*.*fE%+03d ", (int) PRECISION + 5, (int) PRECISION,
      *FP++ * pow10 (n), -n);
}

FCode (p4_f_s_dot)
{
    p4_outf ("%.*E ", (int) PRECISION, *FP++);
}

FCode (p4_f_proximate)
{
    double a, b, c;

    a = FP[2];
    b = FP[1];
    c = FP[0];
    FP += 3;
    *--SP = P4_FLAG
        (c > 0 
          ? fabs (a - b) < c 
          : c < 0 
          ? fabs (a - b) < -c * (fabs (a) + fabs (b))
          : EXACTLY_EQUAL (a, b));
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
#if 1  /* ante C99 */
FCode (p4_f_expm1)	{ *FP = exp (*FP) - 1.0; }
#else  /* post C99 */
FCode (p4_f_expm1)	{ *FP = expm1 (*FP); }
#endif
FCode (p4_f_ln)		{ *FP = log (*FP); }
#if 1  /* ante C99 */
FCode (p4_f_lnp1)	{ *FP = log (*FP + 1.0); }
#else  /* post C99 */
FCode (p4_f_lnp1)	{ *FP = log1p (*FP); }
#endif
FCode (p4_f_log)	{ *FP = log10 (*FP); }
FCode (p4_f_sin)	{ *FP = sin (*FP); }
FCode (p4_f_sincos)	{ --FP; FP [0] = cos (FP [1]); FP [1] = sin (FP [1]); }
FCode (p4_f_sinh)	{ *FP = sinh (*FP); }
FCode (p4_f_sqrt)	{ *FP = sqrt (*FP); }
FCode (p4_f_tan)	{ *FP = tan (*FP); }
FCode (p4_f_tanh)	{ *FP = tanh (*FP); }

/* environment queries */

static FCode (p__floating_stack)
{
    FX_PUSH ((PFE.f0 - PFE.fstack) / sizeof(double));
}

static FCode (p__max_float)
{
    *--FP = DBL_MAX;
}

/* words not from the ansi'94 forth standard  */

/* ================= INTERPRET =================== */

static p4ucell FXCode (interpret_float) /*hereclean*/
{
    /* scanned word sits at PFE.word. (not at HERE) */
# ifndef P4_NO_FP
    if (! BASE == 10 || ! FLOAT_INPUT) return 0; /* quick path */

    {
	double f;
	/* WORD-string is at HERE */
	if (! p4_to_float (PFE.word.ptr, PFE.word.len, &f)) 
	    return 0; /* quick path */
	
	if (STATE)
	{
#          if PFE_ALIGNOF_DFLOAT > PFE_ALIGNOF_CELL
	    if (P4_DFALIGNED (DP))
		FX_COMPILE2 (p4_f_literal);
#          endif
	    FX_COMPILE1 (p4_f_literal);
	    FX_FCOMMA (f);
	}else{ 
	    *--FP = f;
	}
	return 1;
    }
#  else
	return 0;
#  endif
}

static FCode(abort_float)
{
    FP = p4_F0;
}

# if 0
static int decompile_floating (char* nfa, p4xt xt)
{
    if (*P4_TO_CODE(xt) == PFX (p4_f_constant_RT))          
    {
        p4_outf ("%g FCONSTANT ", 
          *(double *) p4_dfaligned ((p4cell) P4_TO_BODY (xt)));
        p4_dot_name (nfa);
        return 1;
    }
    else if (*P4_TO_CODE(xt) == PFX (p4_f_variable_RT))
    {
        p4_outf ("%g FVARIABLE ", 
          *(double *) p4_dfaligned ((p4cell) P4_TO_BODY (xt)));
        p4_dot_name (nfa);
        return 1;
    } 
    return 0;
}
# endif

/* ************************************************** init / deinit */

static FCode (p4_interpret_float_execution)
{
    FX_USE_CODE_ADDR;
    if (FX (interpret_float)) FX_BRANCH; else FX_SKIP_BRANCH;
    FX_USE_CODE_EXIT;
}
FCode (p4_interpret_float)
{
    p4_Q_pairs (P4_DEST_MAGIC); /* BEGIN ... AGAIN */
    FX_COMPILE (p4_interpret_float);
    FX (p4_dup);
    FX (p4_backward_resolve);
    FX_PUSH (P4_DEST_MAGIC);
}
P4COMPILES (p4_interpret_float, p4_interpret_float_execution,
  P4_SKIPS_OFFSET, P4_NEW1_STYLE);
/** INTERPRET-FLOAT ( CS: dest* -- dest* ) executes ( -- F: f# ) experimental
 *  check the next word from => QUERY and try to parse it as a
 *  floating number - if parseable then postpone the value on the
 *  floating stack and branch out of the loop body (usually do it => AGAIN )
 */

#ifndef FLOATING_INTERPRET_SLOT       /* USER-CONFIG: */
#define FLOATING_INTERPRET_SLOT 2     /* 1 == smart-ext / 2 == floating-ext */
#endif

static FCode_RT(floating_deinit)
{
    FX_USE_BODY_ADDR; 
    FX_POP_BODY_ADDR_UNUSED;
/*  PFE.decompile[FLOATING_INTERPRET_SLOT] = 0; */
    PFE.interpret[FLOATING_INTERPRET_SLOT] = 0;
    PFE.abort[FLOATING_INTERPRET_SLOT] = 0;
    {   /* HACK: FIXME: verrrry experimental FLOAT-NUMBER? deactivate */
	void* old_DP = PFE.dp; 
	PFE.dp = (p4_byte_t*) PFE.interpret_compile_float;
	PFE.state = P4_TRUE;
	FX_PUSH (PFE.interpret_compile_resolve);
	FX_PUSH (P4_DEST_MAGIC);
	FX (p4_interpret_nothing); // compiles...
	FX_2DROP;
	PFE.state = P4_FALSE;
	PFE.dp = old_DP;
    }
}

#ifndef FLT_STACK_SIZE          /* USER-CONFIG: --fp-stack-size */
#define	FLT_STACK_SIZE	0	/* 0 -> P4_KB*1024 / 16 */
#endif

#ifndef FLOATING_HEADROOM       /* USER-CONFIG: */
#define FLOATING_HEADROOM 2     /* F-stack underflow does no harm for these */
#endif

static FCode(floating_init)
{
    p4ucell flt_stack_size =
	p4_search_option_value ((const p4_char_t*) "/fp-stack", 9, 
				FLT_STACK_SIZE ? FLT_STACK_SIZE 
				: (PFE_set.total_size / 32) / sizeof(double),
				PFE.set);

    if (flt_stack_size < 6) /* ANS Forth (dpans94), section 12.3.3 : */
	flt_stack_size = 6; /* The size of a floating-point stack
			       shall be at least 6 items. */

    if (! p4_dict_allocate (flt_stack_size, sizeof(double), 
			    PFE_ALIGNOF_DFLOAT, 
			    (void**) &PFE.fstack, (void**) &PFE.f0)
	) p4_throw (P4_ON_DICT_OVER); /** FIXME: no good idea to throw here */

    PFE.f0 -= FLOATING_HEADROOM;

    FP = PFE.f0; /* same as abort_float above. Is that a rule? askmee */

    PFE.interpret[FLOATING_INTERPRET_SLOT] = PFX (interpret_float);
    PFE.abort[FLOATING_INTERPRET_SLOT] = PFX(abort_float);
/*  PFE.decompile[FLOATING_INTERPRET_SLOT] = decompile_floating; */
    p4_forget_word ("deinit:floating:%i", FLOATING_INTERPRET_SLOT, 
		    PFX(floating_deinit), FLOATING_INTERPRET_SLOT);

    
    {   /* HACK: FIXME: verrrry experimental FLOAT-NUMBER? activate */
	void* old_DP = PFE.dp; 
	PFE.dp = (p4_byte_t*) PFE.interpret_compile_float;
	PFE.state = P4_TRUE;
	FX_PUSH (PFE.interpret_compile_resolve);
	FX_PUSH (P4_DEST_MAGIC);
	FX (p4_interpret_float); // compiles...
	FX_2DROP;
	PFE.state = P4_FALSE;
	PFE.dp = old_DP;
    }
}


extern p4Words P4WORDS(floating_misc);

P4_LISTWORDS (floating) =
{
    P4_INTO ("[ANS]", 0),
    P4_FXco (">FLOAT",		 p4_to_float),
    P4_FXco ("D>F",		 p4_d_to_f),
    P4_FXco ("F!",		 p4_f_store),
    P4_FXco ("F*",		 p4_f_star),
    P4_FXco ("F+",		 p4_f_plus),
    P4_FXco ("F-",		 p4_f_minus),
    P4_FXco ("F/",		 p4_f_slash),
    P4_FXco ("F0<",		 p4_f_zero_less),
    P4_FXco ("F0=",		 p4_f_zero_equal),
    P4_FXco ("F<",		 p4_f_less_than),
    P4_FXco ("F>D",		 p4_f_to_d),
    P4_FXco ("F@",		 p4_f_fetch),
    P4_FXco ("FALIGN",		 p4_d_f_align),
    P4_FXco ("FALIGNED",	 p4_d_f_aligned),
    P4_RTco ("FCONSTANT",	 p4_f_constant),
    P4_FXco ("FDEPTH",		 p4_f_depth),
    P4_FXco ("FDROP",		 p4_f_drop),
    P4_FXco ("FDUP",		 p4_f_dup),
    P4_SXco ("FLITERAL",	 p4_f_literal),
    P4_FXco ("FLOAT+",		 p4_d_float_plus),
    P4_FXco ("FLOATS",		 p4_d_floats),
    P4_FXco ("FLOOR",		 p4_floor),
    P4_FXco ("FMAX",		 p4_f_max),
    P4_FXco ("FMIN",		 p4_f_min),
    P4_FXco ("FNEGATE",		 p4_f_negate),
    P4_FXco ("FOVER",		 p4_f_over),
    P4_FXco ("FROT",		 p4_f_rot),
    P4_FXco ("FROUND",		 p4_f_round),
    P4_FXco ("FSWAP",		 p4_f_swap),
    P4_RTco ("FVARIABLE",	 p4_f_variable),
    P4_FXco ("REPRESENT",	 p4_represent),
    /* floating point extension words */
    P4_FXco ("DF!",		 p4_f_store),
    P4_FXco ("DF@",		 p4_f_fetch),
    P4_FXco ("DFALIGN",		 p4_d_f_align),
    P4_FXco ("DFALIGNED",	 p4_d_f_aligned),
    P4_FXco ("DFLOAT+",		 p4_d_float_plus),
    P4_FXco ("DFLOATS",		 p4_d_floats),
    P4_FXco ("F**",		 p4_f_star_star),
    P4_FXco ("F.",		 p4_f_dot),
    P4_FXco ("FABS",		 p4_f_abs),
    P4_FXco ("FACOS",		 p4_f_acos),
    P4_FXco ("FACOSH",		 p4_f_acosh),
    P4_FXco ("FALOG",		 p4_f_alog),
    P4_FXco ("FASIN",		 p4_f_asin),
    P4_FXco ("FASINH",		 p4_f_asinh),
    P4_FXco ("FATAN",		 p4_f_atan),
    P4_FXco ("FATAN2",		 p4_f_atan2),
    P4_FXco ("FATANH",		 p4_f_atanh),
    P4_FXco ("FCOS",		 p4_f_cos),
    P4_FXco ("FCOSH",		 p4_f_cosh),
    P4_FXco ("FE.",		 p4_f_e_dot),
    P4_FXco ("FEXP",		 p4_f_exp),
    P4_FXco ("FEXPM1",		 p4_f_expm1),
    P4_FXco ("FLN",		 p4_f_ln),
    P4_FXco ("FLNP1",		 p4_f_lnp1),
    P4_FXco ("FLOG",		 p4_f_log),
    P4_FXco ("FS.",		 p4_f_s_dot),
    P4_FXco ("FSIN",		 p4_f_sin),
    P4_FXco ("FSINCOS",		 p4_f_sincos),
    P4_FXco ("FSINH",		 p4_f_sinh),
    P4_FXco ("FSQRT",		 p4_f_sqrt),
    P4_FXco ("FTAN",		 p4_f_tan),
    P4_FXco ("FTANH",		 p4_f_tanh),
    P4_FXco ("F~",		 p4_f_proximate),
    P4_DVaL ("PRECISION",	 precision),
    P4_FXco ("SET-PRECISION",	 p4_set_precision),
    P4_FXco ("SF!",		 p4_s_f_store),
    P4_FXco ("SF@",		 p4_s_f_fetch),
    P4_FXco ("SFALIGN",		 p4_align),
    P4_FXco ("SFALIGNED",	 p4_aligned),
    P4_FXco ("SFLOAT+",		 p4_s_float_plus),
    P4_FXco ("SFLOATS",		 p4_s_floats),
    P4_LOAD ("", floating_misc),

    P4_INTO ("ENVIRONMENT", 0 ),
    P4_OCoN ("FLOATING-EXT",	 1994 ),
    P4_FXco ("FLOATING-STACK",	 p__floating_stack ),
    P4_FXco ("MAX-FLOAT",	 p__max_float ),
    P4_OCoN ("forth200x/fp-stack",       2006 ),
    P4_SXco ("INTERPRET-FLOAT",	 p4_interpret_float),
    P4_XXco ("FLOATING-LOADED",  floating_init),
};
P4_COUNTWORDS (floating, "Floating point + extensions");

/* if !defined P4_NO_FP */
#endif 

/*@}*/
/* 
 * Local variables:
 * c-file-style: "stroustrup"
 * End:
 */
