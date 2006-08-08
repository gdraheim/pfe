/** 
 *  -- The No-FP-Stack Floating-Point Word Set
 * 
 *  Copyright (C) Krishna Myneni and Guido Draheim, 2002
 *
 *  @see     GNU LGPL
 *  @author  Krishna Myneni        @(#) %derived_by: guidod %
 *  @version %version: 33.18 %
 *    (%date_modified: Tue Mar 18 15:42:53 2003 %)
 *
 *  @description
 *         The No-FP-Stack Floating-Point Wordset is not usually
 *         used on embedded platforms. This Module implements
 *         the floating-point words but expects and puts the
 *         floating-point values on the forth parameter-stack.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: fpnostack-ext.c,v 1.1.1.1 2006-08-08 09:09:10 guidod Exp $";
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
#include <pfe/logging.h>
#include <pfe/_missing.h>

#define CELLBITS	BITSOF (p4cell)

#if PFE_SIZEOF_CELL+0 == PFE_SIZEOF_DOUBLE+0
#define FSPINC  SP++
#define FSPDEC  SP--
#define FSP  ((double*) SP)
#define DFCELLS 1
#define DFHALF  0
#else
#define FSPINC  SP++; SP++
#define FSPDEC  SP--; SP--
#define FSP  ((double*) SP)
#define DFCELLS 2
#define DFHALF  1
#endif

#define FX_F_DROP FSPINC
#define FX_F_1DROP FSPINC
#define FX_F_2DROP FSPINC; FSPINC
#define FX_F_3DROP FSPINC; FSPINC; FSPINC
#define FX_F_1ROOM FSPDEC
#define FX_F_2ROOM FSPDEC; FSPDEC
#define FX_F_3ROOM FSPDEC; FSPDEC; FSPDEC


/* -----------------------------------------------------------------
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
p4_nofp_dfaligned (p4cell n)	
{
    while (!P4_DFALIGNED (n))
        n++;
    return n;
}

/**
 *  used in engine
 */
_export int
p4_nofp_to_float (const p4_char_t *p, p4cell n, double *r)
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


FCode (p4_nofp_d_f_align); /* forward */

#if USE_SSCANF		/* define this if you fully trust your scanf */

/*
 * This is a working solution on most machines.
 * Unfortunately it relies on pretty obscure features of sscanf()
 * which are not truly implemented everywhere.
 */
FCode (p4_nofp_to_float)		
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
    n = p4_dash_trailing (p, *SP);
    if (n == 0)
    {
	FX_2DROP; FX_F_1ROOM; /*fixme?: no-op on ILP32! */
        *FSP = 0.;
	FX_PUSH (P4_TRUE);
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
	FX_2DROP; FX_F_1ROOM; /*fixme?: no-op on ILP32! */
	*FSP = r;
	FX_PUSH (P4_TRUE);
        return;
    }
    for (i = 0; i < DIM (fmt); i++)
    {
        switch (sscanf (buf, fmt[i], &r, &n1, &n2, &exp, &n3))
        {
         case 1:
             if (n < n2)
                 break;

	     FX_2DROP; FX_F_1ROOM; /*fixme?: no-op on ILP32! */
	     *FSP = r;
	     FX_PUSH (P4_TRUE);
             return;
         case 2:
             if (n1 != n2 || n < n3)
                 break;

	     FX_2DROP; FX_F_1ROOM; /*fixme?: no-op on ILP32! */
	     *FSP = r * pow10 (exp);
	     FX_PUSH (P4_TRUE);
             return;
        }
    }
    FX_2DROP; FX_F_1ROOM; /*fixme?: no-op on ILP32! */
    *FSP = 0; 
    FX_PUSH (P4_FALSE);
}

#else

FCode (p4_nofp_to_float)	
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
    int n = *SP;		/* string length */
    char *p = (char *) *(SP+1);	/* points to string */

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

    FX_2DROP; FX_F_1ROOM; /*fixme?: no-op on ILP32! */
    *FSP = sign * mant * pow10 (scale + esign * exp);
    FX_PUSH (P4_TRUE);
    return;
 bad:
    FX_2DROP; FX_F_1ROOM; /*fixme?: no-op on ILP32! */
    FX_PUSH (P4_FALSE);
    return;
}

#endif

FCode (p4_nofp_d_to_f)
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

    FX_2DROP; FX_F_1ROOM; /* fixme?: no-op on 32bit! */
    *FSP = sign ? -res : res;
}

FCode (p4_nofp_f_store)
{
    *((double *) *SP) = *((double*) (SP+1));
    FX_DROP;
    FX_F_DROP;
}

FCode (p4_nofp_f_star)
{
    FSP[1] *= FSP[0];
    FX_F_DROP;
}

FCode (p4_nofp_f_plus)
{
    FSP[1] += FSP[0];
    FX_F_DROP;
}

FCode (p4_nofp_f_minus)
{
    FSP[1] -= FSP[0];
    FX_F_DROP;
}

FCode (p4_nofp_f_slash)
{
    FSP[1] /= FSP[0];
    FX_F_DROP;
}

FCode (p4_nofp_f_zero_less)
{
    *(SP+DFHALF) = P4_FLAG (*FSP < 0);
    SP+=DFHALF; /* FX_F_DROP; FX_1ROOM */
}

FCode (p4_nofp_f_zero_equal)
{
    *(SP+DFHALF) = P4_FLAG (*FSP == 0); 
    SP+=DFHALF; /* FX_F_DROP; FX_1ROOM */
}

FCode (p4_nofp_f_equal)
{
    int flag;
    flag = P4_FLAG (FSP[1] == FSP[0]);
    FX_F_2DROP; FX_1ROOM;
    *SP = flag;
}

FCode (p4_nofp_f_not_equal)
{
    int flag;
    flag = P4_FLAG (FSP[1] != FSP[0]);
    FX_F_2DROP; FX_1ROOM;
    *SP = flag;
}

FCode (p4_nofp_f_less_than)
{
    int flag;
    flag = P4_FLAG (FSP[1] < FSP[0]);
    FX_F_2DROP; FX_1ROOM;
    *SP = flag;
}

FCode (p4_nofp_f_greater_than)
{
    int flag;
    flag = P4_FLAG (FSP[1] > FSP[0]);
    FX_F_2DROP; FX_1ROOM;
    *SP = flag;
}

FCode (p4_nofp_f_less_than_or_equal)
{
    int flag;
    flag = P4_FLAG (FSP[1] <= FSP[0]);
    FX_F_2DROP; FX_1ROOM;
    *SP = flag;
}

FCode (p4_nofp_f_greater_than_or_equal)
{
    int flag;
    flag = P4_FLAG (FSP[1] >= FSP[0]);
    FX_F_2DROP; FX_1ROOM;
    *SP = flag;
}

FCode (p4_nofp_f_to_d)
{
    double a, hi, lo;
    int sign;
    
    if ((a = *FSP) < 0)
        sign = 1, a = -a;
    else
        sign = 0;
    lo = modf (ldexp (a, -CELLBITS), &hi);
    FX_F_1DROP; FX_2ROOM; /* fixme? no-op on ILP32! */
    SP[0] = (p4ucell) hi;
    SP[1] = (p4ucell) ldexp (lo, CELLBITS);
    if (sign)
        dnegate ((p4dcell *) &SP[0]);
}

/* some words Julian Noble found useful, plus a few more */

/** S>F  ( n -- x )
 */
FCode (p4_nofp_s_to_f)
{
    p4cell n = FX_POP; FX_F_1ROOM;
    *FSP = (double) n;  
}

/** FTRUNC>S  ( x -- n )
 */
FCode (p4_nofp_f_trunc_to_s)
{
    double h = *FSP; FX_F_DROP;
    FX_PUSH((p4cell) h);
}

/** FROUND>S ( x -- n )
 */
FCode (p4_nofp_f_round_to_s)
{
    extern FCode (p4_nofp_f_round); /* defined later */
    FX (p4_nofp_f_round);
    FX (p4_nofp_f_trunc_to_s);
}

/** FTRUNC ( x -- x' )
 */
FCode (p4_nofp_f_trunc)
{
#  if __STDC_VERSION__+0 > 199900
    *FSP = trunc (*FSP);
#  else
    if (*FSP > 0)
	*FSP = floor (*FSP);
    else
	*FSP = ceil (*FSP);
#  endif
}

/** -FROT  ( x1 x2 x3 -- x3 x1 x2 )
 */
FCode (p4_nofp_minus_f_rot)
{
    double h = FSP[0];

    FSP[0] = FSP[1];
    FSP[1] = FSP[2];
    FSP[2] = h;
}

/** FNIP  ( x1 x2 -- x2 )
 */
FCode (p4_nofp_f_nip)
{
   FSP[1] = FSP[0];
   FSPINC;
}

/** FTUCK  ( x1 x2 -- x2 x1 x2 )
 */
FCode (p4_nofp_f_tuck)
{
    FX_F_1ROOM;
    FSP[0] = FSP[1];
    FSP[1] = FSP[2];
    FSP[2] = FSP[0];
}

/** 1/F  ( x -- 1/x )
 */
FCode (p4_nofp_one_over_f)
{
    *FSP = 1.0 / *FSP; 
}

/** F^2  ( x -- x^2 )
 */
FCode (p4_nofp_f_square)
{
    *FSP = *FSP * *FSP; 
}

/** F^N  ( x u -- x^u )
 * For large exponents, use F** instead.  Of course u=-1 is large. 
 */
FCode (p4_nofp_f_power_n)
{
    p4ucell n = FX_POP;
    double x = *FSP;

    if ( n == 1 ) return;
 
    {   double r = 1.0;
 
        if ( n )
        {
            double xsq = x * x;

            if ( n & 1 ) r = x;
            for ( n = n/2; n > 0; n-- ) r = r * xsq;
        }
        *FSP = r;
    }
}

/** F2/  ( x -- x/2 )
 */
FCode (p4_nofp_f_two_slash)
{
    *FSP = ldexp (*FSP, -1); 
}

/** F2*  ( x -- x*2 )
 */
FCode (p4_nofp_f_two_star)
{
    *FSP = ldexp (*FSP, 1); 
}

/** F0>  ( x -- flag )
*/

FCode (p4_nofp_f_zero_greater)
{
    int flag;
    flag = P4_FLAG (*FSP > 0.);
    SP+=DFHALF; *SP = flag;
}

/** F0<>  ( x -- flag )
*/
FCode (p4_nofp_f_zero_not_equal)
{
    int flag;
    flag = P4_FLAG (*FSP != 0.);
    SP+=DFHALF; *SP = flag;
}

/* ------ */

FCode (p4_nofp_f_fetch)
{
    *((double*) (SP-DFHALF)) = *((double*) *SP); SP-=DFHALF; /* fixme? */
}

FCode_RT (p4_nofp_f_constant_RT)
{
    FX_USE_BODY_ADDR;
    FX_F_1ROOM;
    *FSP = *(double *) p4_nofp_dfaligned ((p4cell) FX_POP_BODY_ADDR);
}

FCode (p4_nofp_f_constant)
{
    FX_RUNTIME_HEADER;
    FX_RUNTIME1 (p4_nofp_f_constant);
    FX (p4_nofp_d_f_align);
    FX_FCOMMA (*FSP);
    FX_F_DROP;
}
P4RUNTIME1(p4_nofp_f_constant, p4_nofp_f_constant_RT);

FCode (p4_nofp_f_depth)
{
    int depth = (p4_S0 - SP)/DFCELLS; 
    FX_PUSH (depth);
}

FCode (p4_nofp_f_drop)
{
    FX_F_DROP;
}

FCode (p4_nofp_f_dup)
{
    FX_F_1ROOM;
    FSP[0] = FSP[1];
}

/* originally P4_SKIPS_FLOAT */
p4xcode* 
p4_lit_nofp_float_SEE (p4xcode* ip, char* p, p4_Semant* s)
{
# if PFE_ALIGNOF_DFLOAT > PFE_ALIGNOF_CELL
    if (!P4_DFALIGNED (ip))
        ip++;
# endif
    sprintf (p, "%e ", *(double *) ip);
    P4_INC (ip, double);
    
    return ip;
}

FCode_XE (p4_nofp_f_literal_execution)
{
    FX_USE_CODE_ADDR;
    FX_F_1ROOM;
    *FSP= P4_POP_ (double, IP);
    FX_USE_CODE_EXIT;
}

FCode (p4_nofp_f_literal)
{
    _FX_STATESMART_Q_COMP;
    if (STATESMART)
    {
#if PFE_ALIGNOF_DFLOAT > PFE_ALIGNOF_CELL
        if (P4_DFALIGNED (DP))
            FX_COMPILE2 (p4_nofp_f_literal);
#endif
        FX_COMPILE1 (p4_nofp_f_literal);
        FX_FCOMMA (*FSP);
	FX_F_DROP;
    }
}
P4COMPILES2 (p4_nofp_f_literal, p4_nofp_f_literal_execution, p4_noop,
	     p4_lit_nofp_float_SEE, P4_DEFAULT_STYLE);

FCode (p4_nofp_floor)
{
  *FSP = floor (*FSP);
}

FCode (p4_nofp_f_max)
{
    if (FSP[0] > FSP[1])
        FSP[1] = FSP[0];
    FX_F_DROP;
}

FCode (p4_nofp_f_min)
{
    if (FSP[0] < FSP[1])
        FSP[1] = FSP[0];
    FX_F_DROP;
}

FCode (p4_nofp_f_negate)
{
    *FSP = -*FSP;
}

FCode (p4_nofp_f_over)
{
    FX_F_1ROOM;
    FSP[0] = FSP[2];
}

FCode (p4_nofp_f_rot)
{
    double h = FSP[2];
    
    FSP[2] = FSP[1];
    FSP[1] = FSP[0];
    FSP[0] = h;
}

#ifndef FROUND_FLOOR                /* same user.config as in floating-ext! */
#define FROUND_FLOOR 0              /* FROUND identical with floor(fp+0.5) ? */
#endif

FCode (p4_nofp_f_round)
{
#  if defined HAVE_RINT || defined PFE_HAVE_RINT
    /* correct and fast */
    *FSP = rint (*FSP);
#  elif FROUND_FLOOR
    /* incorrect but fast */
    *FSP = floor (*FSP + 0.5); 
#  else
    /* correct but slow */
    double whole, frac, offset;
 
    frac = fabs(modf(*FSP, &whole));
    *FSP = whole;
    FX(p4_nofp_f_to_d);  /* execute F>D */
    offset = (*SP < 0) ? -1. : 1.;
    
    if (*(SP+1) & 1)  /* check even or odd */
    {
	if (frac >= 0.5) whole += offset;
    }
    else
    {
	if (frac > 0.5) whole += offset;
    }
    *FSP = whole;
#  endif  
}

FCode (p4_nofp_f_swap)
{
    double h = FSP[1];
    
    FSP[1] = FSP[0];
    FSP[0] = h;
}

FCode_RT (p4_nofp_f_variable_RT)
{
    FX_USE_BODY_ADDR;
    FX_PUSH_SP = p4_nofp_dfaligned ((p4cell) FX_POP_BODY_ADDR);
}

FCode (p4_nofp_f_variable)
{
    FX_RUNTIME_HEADER;
    FX_RUNTIME1 (p4_nofp_f_variable);
    FX (p4_nofp_d_f_align);
    FX_FCOMMA (0.);
}
P4RUNTIME1(p4_nofp_f_variable, p4_nofp_f_variable_RT); 

FCode (p4_nofp_represent)		/* with help from Lennart Benshop */
{
    char *p, buf[0x80];
    int u, log, sign;
    double f;
    
    p = (char *) SP[1];
    u = SP[0];
    FX_2DROP;
    f = *FSP;
    FX_F_DROP;
    
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

    FX_3ROOM;
    SP[2] = log;
    SP[1] = sign;
    SP[0] = P4_TRUE;
}

/* ********************************************************************** */
/* Floating point extension words:                                        */
/* ********************************************************************** */

FCode (p4_nofp_d_f_align)
{
    while (!P4_DFALIGNED (DP))
        *DP++ = 0;
}

FCode (p4_nofp_d_f_aligned)
{
    SP[0] = p4_nofp_dfaligned (SP[0]);
}

FCode (p4_nofp_d_float_plus)
{
    *SP += sizeof (double);
}

FCode (p4_nofp_d_floats)
{
    *SP *= sizeof (double);
}

FCode (p4_nofp_f_star_star)
{
    FSP[1] = pow (FSP[1], FSP[0]);
    FX_F_DROP;
}

FCode (p4_nofp_f_dot)
{
    p4_outf ("%.*f ", (int) PRECISION, *FSP);
    FX_F_DROP;
}

FCode (p4_nofp_f_abs)
{
    *FSP = fabs(*FSP);
}

FCode (p4_nofp_f_e_dot)			/* with help from Lennart Benshop */
{
    double f = fabs (*FSP);
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
      *FSP * pow10 (n), -n);
    FX_F_DROP;
}

FCode (p4_nofp_f_s_dot)
{
    p4_outf ("%.*E ", (int) PRECISION, *FSP); FSPINC;
}

FCode (p4_nofp_f_proximate)
{
    double a, b, c;

    a = FSP[2];
    b = FSP[1];
    c = FSP[0];
    FX_F_3DROP; FX_1ROOM;
    *SP = P4_FLAG
        (c > 0 
          ? fabs (a - b) < c 
          : c < 0 
          ? fabs (a - b) < -c * (fabs (a) + fabs (b))
          : EXACTLY_EQUAL (a, b));
}

FCode (p4_nofp_set_precision)
{
    PRECISION = *SP++;
}

FCode (p4_nofp_s_f_store)
{
    *(float *) *SP = *((double*) (SP+1));
    FX_DROP; FX_F_DROP;
}

FCode (p4_nofp_s_f_fetch)
{
    *((double*)(SP-DFHALF)) = *(float *) *SP;
    SP-=DFHALF; /* fixme: store before making room */
}

FCode (p4_nofp_s_float_plus)
{
    *SP += sizeof (float);
}

FCode (p4_nofp_s_floats)
{
    *SP *= sizeof (float);
}

/*-- simple mappings to the ANSI-C library  --*/

FCode (p4_nofp_f_acos)	{ *FSP = acos (*FSP); }
FCode (p4_nofp_f_acosh)	{ *FSP = acosh (*FSP); }
FCode (p4_nofp_f_alog)	{ *FSP = pow10 (*FSP); }
FCode (p4_nofp_f_asin)	{ *FSP = asin (*FSP); }
FCode (p4_nofp_f_asinh)	{ *FSP = asinh (*FSP); }
FCode (p4_nofp_f_atan)	{ *FSP = atan (*FSP); }
FCode (p4_nofp_f_atan2)	{ FSP [1] = atan2 (FSP [1], FSP [0]); FX_F_DROP; }
FCode (p4_nofp_f_atanh)	{ *FSP = atanh (*FSP); }
FCode (p4_nofp_f_cos)	{ *FSP = cos (*FSP); }
FCode (p4_nofp_f_cosh)	{ *FSP = cosh (*FSP); }
FCode (p4_nofp_f_exp)	{ *FSP = exp (*FSP); }
#if 1  /* ante C99 */
FCode (p4_nofp_f_expm1)	{ *FSP = exp (*FSP) - 1.0; }
#else  /* post C99 */
FCode (p4_nofp_f_expm1)	{ *FSP = expm1 (*FSP); }
#endif
FCode (p4_nofp_f_ln)	{ *FSP = log (*FSP); }
#if 1  /* ante C99 */
FCode (p4_nofp_f_lnp1)	{ *FSP = log (*FSP + 1.0); }
#else  /* post C99 */
FCode (p4_nofp_f_lnp1)	{ *FSP = log1p (*FSP); }
#endif
FCode (p4_nofp_f_log)	{ *FSP = log10 (*FSP); }
FCode (p4_nofp_f_sin)	{ *FSP = sin (*FSP); }
FCode (p4_nofp_f_sincos){ FX_F_1ROOM; FSP[0]=cos(FSP[1]); FSP[1]=sin(FSP[1]);}
FCode (p4_nofp_f_sinh)	{ *FSP = sinh (*FSP); }
FCode (p4_nofp_f_sqrt)	{ *FSP = sqrt (*FSP); }
FCode (p4_nofp_f_tan)	{ *FSP = tan (*FSP); }
FCode (p4_nofp_f_tanh)	{ *FSP = tanh (*FSP); }

/* environment queries */

static FCode (p__nofp_max_float)
{
    FX_F_1ROOM;
    *FSP = DBL_MAX;
}

/* words not from the ansi'94 forth standard  */

/* ================= INTERPRET =================== */

#ifndef DOUBLE_ALIGNED
#if defined HOST_ARCH_SPARC     || defined __target_arch_sparc
#define DOUBLE_ALIGNED 1
#elif defined HOST_ARCH_HPPA    || defined __target_arch_hppa
#define DOUBLE_ALIGNED 1
#elif defined HOST_ARCH_POWERPC || defined __target_arch_powerpc 
#define DOUBLE_ALIGNED 1
#else
#define DOUBLE_ALIGNED 0
#endif
#endif

static p4ucell FXCode (interpret_float) /*hereclean*/
{
    /* scanned word sits at PFE.word. (not at HERE) */
# ifndef P4_NO_FP
    if (! BASE == 10 || ! FLOAT_INPUT) return 0; /* quick path */

    {
	double f;
	/* WORD-string is at HERE */
	if (! p4_nofp_to_float (PFE.word.ptr, PFE.word.len, &f)) 
	    return 0; /* quick path */
	
	if (STATE)
	{
#          if PFE_ALIGNOF_DFLOAT > PFE_ALIGNOF_CELL
	    if (P4_DFALIGNED (DP))
		FX_COMPILE2 (p4_nofp_f_literal);
#          endif
	    FX_COMPILE1 (p4_nofp_f_literal);
	    FX_FCOMMA (f);
	}else{
	    FX_F_1ROOM;
#          if DOUBLE_ALIGNED
            if (((long)(void*)SP)&7) { 
		FX_1ROOM; P4_fail("auto dfaligned SP"); }
#          endif
	    *FSP = f;
	}
	return 1;
    }
#  else
	return 0;
#  endif
}

static int decompile_floating (p4_namebuf_t* nfa, p4xt xt)
{
    if (*P4_TO_CODE(xt) == PFX (p4_nofp_f_constant_RT))          
    {
        p4_outf ("%g FCONSTANT ( fpnostack )", 
          *(double *) p4_nofp_dfaligned ((p4cell) P4_TO_BODY (xt)));
        p4_dot_name (nfa);
        return 1;
    }
    else if (*P4_TO_CODE(xt) == PFX (p4_nofp_f_variable_RT))
    {
        p4_outf ("%g FVARIABLE ( fpnostack )", 
          *(double *) p4_nofp_dfaligned ((p4cell) P4_TO_BODY (xt)));
        p4_dot_name (nfa);
        return 1;
    } 
    return 0;
}

/*
 * slot 1 == p4_interpret_smart
 * slot 2 == p4_interpret_floating
 */
#ifndef FPNOSTACK_INTERPRET_SLOT       /* USER-CONFIG: */
#define FPNOSTACK_INTERPRET_SLOT 2     /* 1 == smart-ext / 2 == floating-ext */
#endif

static FCode_RT(fpnostack_deinit)
{
    FX_USE_BODY_ADDR; 
    FX_POP_BODY_ADDR_UNUSED;
    PFE.decompile[FPNOSTACK_INTERPRET_SLOT] = 0;
    PFE.interpret[FPNOSTACK_INTERPRET_SLOT] = 0;
}

static FCode(fpnostack_init)
{
    PFE.interpret[FPNOSTACK_INTERPRET_SLOT] = PFX (interpret_float);
    PFE.decompile[FPNOSTACK_INTERPRET_SLOT] = decompile_floating;
    p4_forget_word ("deinit:fpnostack:%i", FPNOSTACK_INTERPRET_SLOT, 
		    PFX(fpnostack_deinit), FPNOSTACK_INTERPRET_SLOT);
}

P4_LISTWORDS (fpnostack) =
{
    P4_INTO ("EXTENSIONS", 0),
    P4_FXco (">FLOAT",		 p4_nofp_to_float),
    P4_FXco ("D>F",		 p4_nofp_d_to_f),
    P4_FXco ("F!",		 p4_nofp_f_store),
    P4_FXco ("F*",		 p4_nofp_f_star),
    P4_FXco ("F+",		 p4_nofp_f_plus),
    P4_FXco ("F-",		 p4_nofp_f_minus),
    P4_FXco ("F/",		 p4_nofp_f_slash),
    P4_FXco ("F0<",		 p4_nofp_f_zero_less),
    P4_FXco ("F0=",		 p4_nofp_f_zero_equal),
    P4_FXco ("F<",		 p4_nofp_f_less_than),
    P4_FXco ("F>",               p4_nofp_f_greater_than),
    P4_FXco ("F=",               p4_nofp_f_equal),
    P4_FXco ("F<>",              p4_nofp_f_not_equal),
    P4_FXco ("F<=",              p4_nofp_f_less_than_or_equal),
    P4_FXco ("F>=",              p4_nofp_f_greater_than_or_equal),
    P4_FXco ("F>D",		 p4_nofp_f_to_d),
    P4_FXco ("F@",		 p4_nofp_f_fetch),
    P4_FXco ("FALIGN",		 p4_nofp_d_f_align),
    P4_FXco ("FALIGNED",	 p4_nofp_d_f_aligned),
    P4_RTco ("FCONSTANT",	 p4_nofp_f_constant),
    P4_FXco ("FDEPTH",		 p4_nofp_f_depth),
    P4_FXco ("FDROP",		 p4_nofp_f_drop),
    P4_FXco ("FDUP",		 p4_nofp_f_dup),
    P4_SXco ("FLITERAL",	 p4_nofp_f_literal),
    P4_FXco ("FLOAT+",		 p4_nofp_d_float_plus),
    P4_FXco ("FLOATS",		 p4_nofp_d_floats),
    P4_FXco ("FLOOR",		 p4_nofp_floor),
    P4_FXco ("FMAX",		 p4_nofp_f_max),
    P4_FXco ("FMIN",		 p4_nofp_f_min),
    P4_FXco ("FNEGATE",		 p4_nofp_f_negate),
    P4_FXco ("FOVER",		 p4_nofp_f_over),
    P4_FXco ("FROT",		 p4_nofp_f_rot),
    P4_FXco ("FROUND",		 p4_nofp_f_round),
    P4_FXco ("FSWAP",		 p4_nofp_f_swap),
    P4_RTco ("FVARIABLE",	 p4_nofp_f_variable),
    P4_FXco ("REPRESENT",	 p4_nofp_represent),
    /* floating point extension words */
    P4_FXco ("DF!",		 p4_nofp_f_store),
    P4_FXco ("DF@",		 p4_nofp_f_fetch),
    P4_FXco ("DFALIGN",		 p4_nofp_d_f_align),
    P4_FXco ("DFALIGNED",	 p4_nofp_d_f_aligned),
    P4_FXco ("DFLOAT+",		 p4_nofp_d_float_plus),
    P4_FXco ("DFLOATS",		 p4_nofp_d_floats),
    P4_FXco ("F**",		 p4_nofp_f_star_star),
    P4_FXco ("F.",		 p4_nofp_f_dot),
    P4_FXco ("FABS",		 p4_nofp_f_abs),
    P4_FXco ("FACOS",		 p4_nofp_f_acos),
    P4_FXco ("FACOSH",		 p4_nofp_f_acosh),
    P4_FXco ("FALOG",		 p4_nofp_f_alog),
    P4_FXco ("FASIN",		 p4_nofp_f_asin),
    P4_FXco ("FASINH",		 p4_nofp_f_asinh),
    P4_FXco ("FATAN",		 p4_nofp_f_atan),
    P4_FXco ("FATAN2",		 p4_nofp_f_atan2),
    P4_FXco ("FATANH",		 p4_nofp_f_atanh),
    P4_FXco ("FCOS",		 p4_nofp_f_cos),
    P4_FXco ("FCOSH",		 p4_nofp_f_cosh),
    P4_FXco ("FE.",		 p4_nofp_f_e_dot),
    P4_FXco ("FEXP",		 p4_nofp_f_exp),
    P4_FXco ("FEXPM1",		 p4_nofp_f_expm1),
    P4_FXco ("FLN",		 p4_nofp_f_ln),
    P4_FXco ("FLNP1",		 p4_nofp_f_lnp1),
    P4_FXco ("FLOG",		 p4_nofp_f_log),
    P4_FXco ("FS.",		 p4_nofp_f_s_dot),
    P4_FXco ("FSIN",		 p4_nofp_f_sin),
    P4_FXco ("FSINCOS",		 p4_nofp_f_sincos),
    P4_FXco ("FSINH",		 p4_nofp_f_sinh),
    P4_FXco ("FSQRT",		 p4_nofp_f_sqrt),
    P4_FXco ("FTAN",		 p4_nofp_f_tan),
    P4_FXco ("FTANH",		 p4_nofp_f_tanh),
    P4_FXco ("F~",		 p4_nofp_f_proximate),
    P4_DVaL ("PRECISION",	 precision),
    P4_FXco ("SET-PRECISION",	 p4_nofp_set_precision),
    P4_FXco ("SF!",		 p4_nofp_s_f_store),
    P4_FXco ("SF@",		 p4_nofp_s_f_fetch),
    P4_FXco ("SFALIGN",		 p4_align), /* alias cell-aligned */
    P4_FXco ("SFALIGNED",	 p4_aligned),
    P4_FXco ("SFLOAT+",		 p4_nofp_s_float_plus),
    P4_FXco ("SFLOATS",		 p4_nofp_s_floats),

    /* more useful nonstandard words */
    P4_FXco ("S>F",		 p4_nofp_s_to_f),
    P4_FXco ("FTRUNC>S",	 p4_nofp_f_trunc_to_s),
    P4_FXco ("FROUND>S",	 p4_nofp_f_round_to_s),
    P4_FNYM ("F>S",              "FTRUNC>S"),
    P4_FXco ("FTRUNC",           p4_nofp_f_trunc),

    P4_FXco ("-FROT",		 p4_nofp_minus_f_rot),
    P4_FNYM ("F-ROT",		 "-FROT"),
    P4_FXco ("FNIP",		 p4_nofp_f_nip),
    P4_FXco ("FTUCK",		 p4_nofp_f_tuck),
    P4_FXco ("1/F",		 p4_nofp_one_over_f),
    P4_FXco ("F^2",		 p4_nofp_f_square),
    P4_FXco ("F^N",		 p4_nofp_f_power_n),
    P4_FXco ("F2/",		 p4_nofp_f_two_slash),
    P4_FXco ("F2*",		 p4_nofp_f_two_star),
    P4_FXco ("F0>",		 p4_nofp_f_zero_greater),
    P4_FXco ("F0<>",		 p4_nofp_f_zero_not_equal),

    P4_INTO ("ENVIRONMENT", 0 ),
    P4_OCoN ("FLOATING",         1994 ),
    P4_OCoN ("FPNOSTACK-EXT",	 1994 ),
    P4_FXco ("MAX-FLOAT",	 p__nofp_max_float ),
    P4_XXco ("FPNOSTACK-LOADED", fpnostack_init),
#  if ! DOUBLE_ALIGNED
    P4_OCoN ("CELL-FALIGNED",    2003 ),
#  endif
};
P4_COUNTWORDS (fpnostack, "FpNoStack Floating point + extensions");

/* if !defined P4_NO_FP */
#endif 

/*@}*/
/* 
 * Local variables:
 * c-file-style: "stroustrup"
 * End:
 */

