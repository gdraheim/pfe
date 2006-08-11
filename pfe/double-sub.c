/**@name double-sub --- Subroutines for double number (64 Bit) arithmetics.
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2003.
 *  Copyright (C) 2005 - 2006 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.2 $
 *     (modified $Date: 2006-08-11 22:56:04 $)
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: double-sub.c,v 1.2 2006-08-11 22:56:04 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-sub.h>
#include <pfe/def-macro.h>
#include <pfe/def-limits.h>
#include <pfe/double-sub.h>

#define HALFCELL (sizeof(p4cell)*4)
#define CELLBITS	BITSOF (p4cell)
#define HIGHBIT(X)	((X) >> (BITSOF (X) - 1))

#ifndef _export
#define _export
#define dnegate p4_d_negate
#define dadd p4_d_plus
#define ummul p4_d_ummul
#define mmul p4_d_mmul
#define umdiv p4_d_umdiv
#define smdiv p4_d_smdiv
#define fmdiv p4_d_fmdiv
#endif

/** left shift of *a by n positions */
_export void
p4_d_shiftleft (p4dcell *a, int n) 
{
    while (--n >= 0)
    {
        a->hi <<= 1;
        a->hi += HIGHBIT (a->lo);
        a->lo <<= 1;
    }
}

/** arithm. right shift of *a by n positions */
_export void
p4_d_shiftright (p4dcell *a, int n)
{
    while (--n >= 0)
    {
        a->lo >>= 1;
        a->lo += a->hi << (CELLBITS - 1);
        a->hi >>= 1;
    }
}

/** add b to a */
_export void
p4_um_plus (p4dcell * a, p4ucell b) 
{
    p4ucell c;			/* carry */

    P4xD3(*a) = c  = (p4ucell) P4xD3(*a) + P4xW1(b); c >>= HALFCELL;
    P4xD2(*a) = c += (p4ucell) P4xD2(*a) + P4xW0(b); c >>= HALFCELL;
    P4xD1(*a) = c += (p4ucell) P4xD1(*a);            c >>= HALFCELL;
    P4xD0(*a) = c +  (p4ucell) P4xD0(*a);
}

/** add b to a */
_export void
p4_d_plus (p4dcell * a, p4dcell * b)	
{
    p4ucell c;			/* carry */

    P4xD3 (*a) = c  = (p4ucell) P4xD3 (*a) + P4xD3 (*b); c >>= HALFCELL;
    P4xD2 (*a) = c += (p4ucell) P4xD2 (*a) + P4xD2 (*b); c >>= HALFCELL;
    P4xD1 (*a) = c += (p4ucell) P4xD1 (*a) + P4xD1 (*b); c >>= HALFCELL;
    P4xD0 (*a) = c +  (p4ucell) P4xD0 (*a) + P4xD0 (*b);
}

/** subtract b from a */
_export void
p4_d_minus (p4dcell * a, p4dcell * b)	
{
    p4cell c;			/* carry */

    P4xD3(*a) = c  = (p4cell) P4xD3(*a) - (p4cell) P4xD3(*b); c >>= HALFCELL;
    P4xD2(*a) = c += (p4cell) P4xD2(*a) - (p4cell) P4xD2(*b); c >>= HALFCELL;
    P4xD1(*a) = c += (p4cell) P4xD1(*a) - (p4cell) P4xD1(*b); c >>= HALFCELL;
    P4xD0(*a) = c +  (p4cell) P4xD0(*a) - (p4cell) P4xD0(*b);
}

/** negate a */
_export void
p4_d_negate (p4dcell * a) 
{
    p4cell c;			/* carry */

    P4xD3 (*a) = c = -(p4cell) P4xD3 (*a); c >>= HALFCELL;
    P4xD2 (*a) = c -= (p4cell) P4xD2 (*a); c >>= HALFCELL;
    P4xD1 (*a) = c -= (p4cell) P4xD1 (*a); c >>= HALFCELL;
    P4xD0 (*a) = c -  (p4cell) P4xD0 (*a);
}

/** result: a < b */
_export int
p4_d_less (p4dcell * a, p4dcell * b)	
{
    return a->hi != b->hi
        ? a->hi < b->hi
        : a->lo < b->lo;
}

/** result: a < b */
_export int
p4_d_u_less (p4udcell * a, p4udcell * b) 
{
    return a->hi != b->hi ? a->hi < b->hi : a->lo < b->lo;
}

/** unsigned multiply, mixed precision */
_export p4udcell
p4_d_ummul (p4ucell a, p4ucell b)	
{
    p4udcell res;
    p4ucell c, p;

    res.lo = (p4ucell) P4xW1(a) * P4xW1(b);
    if (P4xW0(a))
    {
        p = (p4ucell) P4xW0(a) * P4xW1(b);
        if (P4xW0 (b))
	{
            p4ucell q = (p4ucell) P4xW1(a) * P4xW0(b);
            res.hi = (p4ucell) P4xW0(a) * P4xW0(b);
            P4xD2(res)  = c = (p4ucell) P4xD2(res) + P4xW1(p) + P4xW1(q); 
            /* - */ c >>= HALFCELL;
            P4xD1(res)  = c += (p4ucell) P4xD1(res) + P4xW0(p) + P4xW0(q); 
            /* - */ c >>= HALFCELL;
            P4xD0(res) += c;
	}else
            goto three;
    }else{
        if (P4xW0(b))
	{
            p = (p4ucell) P4xW1(a) * P4xW0(b);
         three:
            P4xD2(res) = c = (p4ucell) P4xD2(res) + P4xW1(p); c >>= HALFCELL;
            P4xD1(res) = c + P4xW0(p);
            P4xD0(res) = 0;
	}else
            res.hi = 0;
    }
    return res;
}

/** signed multiply, mixed precision */
_export p4dcell
p4_d_mmul (p4cell a, p4cell b) 
{
    p4dcell res;
    int s = 0;
    
    if (a < 0) { a = -a; s ^= 1; }
    if (b < 0) { b = -b; s ^= 1; }
    *(p4udcell *) &res = p4_d_ummul (a, b);
    if (s) p4_d_negate (&res);
    return res;
}

/* Divide unsigned double by single precision using shifts and subtracts.
   Return quotient in u->lo, remainder in u->hi. */
static void
shift_subtract (p4udcell * u, p4ucell v)
{
    int i = CELLBITS, c = 0;
    p4ucell q = 0, h = u->hi, l = u->lo;

    for (;;)
    {
        if (c || h >= v)
	{
            q++;
            h -= v;
	}
        if (--i < 0)
            break;
        c = HIGHBIT (h);
        h <<= 1;
        h += HIGHBIT (l);
        l <<= 1;
        q <<= 1;
    }
    u->hi = h;
    u->lo = q;
}

/** unsigned divide procedure, mixed precision */
_export udiv_t
p4_d_umdiv (p4udcell num, p4ucell denom)
{
    udiv_t res;

    if (num.hi == 0)
    {
        res.quot = num.lo / denom;
        res.rem = num.lo % denom;
    }else{
        shift_subtract (&num, denom);
        res.quot = num.lo;
        res.rem = num.hi;
    }
    return res;
}

/** symmetric divide procedure, mixed precision */
_export fdiv_t
p4_d_smdiv (p4dcell num, p4cell denom)
{
    fdiv_t res;
    int sq = 0, sr = 0;

    if (num.hi < 0)
    {
        if (num.hi == -1 && (p4cell) num.lo < 0)
            goto simple;
        p4_d_negate (&num);
        sq ^= 1;
        sr ^= 1;
    }
    else if (num.hi == 0 && (p4cell) num.lo > 0)
    {
     simple:
        res.quot = (p4cell) num.lo / denom;
        res.rem  = (p4cell) num.lo % denom;
        return res;
    }
    if (denom < 0)
    {
        denom = -denom;
        sq ^= 1;
    }
    shift_subtract ((p4udcell *) &num, denom);
    res.quot = sq ? -num.lo : num.lo;
    res.rem  = sr ? -num.hi : num.hi;
    return res;
}

/** floored divide procedure, mixed precision */
_export fdiv_t
p4_d_fmdiv (p4dcell num, p4cell denom)
{
    fdiv_t res = smdiv (num, denom);
    if (res.rem && (num.hi ^ denom) < 0)
    { res.quot--; res.rem += denom; }
    return res;
}

/*@}*/

