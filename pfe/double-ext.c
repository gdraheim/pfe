/**@name DOUBLE-EXT ---  The Optional Double Number Word Set
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 5.6 %
 *    (%date_modified: Mon Mar 12 10:32:08 2001 %)
 *
 *  @description
 *   	Double Numbers use two cells per number.
 *
 *  The forth text interpreter' number is conversion is changed to
 *  see numbers immediatly followed by a decimal point as a two-cell
 *  number. Entering <c>decimal 1234</c> leaves a single-cell number
 *  <c>1234</c> on the stack, and entering <c>decimal 1234.</c> leaves the
 *  double cell number <c>1234 0</c> on the stack.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: double-ext.c,v 0.31 2001-05-12 18:15:46 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-xtra.h>
#include <pfe/def-comp.h>
#include <pfe/double-sub.h>

#define DSP	((p4dcell *)SP)
#define UDP	((p4udcell *)SP)

/** (2CONSTANT) ( -- x1 x2 )
 * runtime portion of => 2CONSTANT
 */
FCode (p4_two_constant_RT)
{
    FX_PUSH (WP_PFA[1]);
    FX_PUSH (WP_PFA[0]);
}

/** 2CONSTANT ( x1 x2 "name" -- )
 * create a word that contains the specified twocell number in its body.
 * when the name is executed, these numbers are left on the stack
   12. 2CONSTANT X .s 
   <emtpy stack> ok
   X .s
   0 12 ok
 */
FCode (p4_two_constant)
{
    p4_header (p4_two_constant_RT_ , 0);
    FX_COMMA (FX_POP);
    FX_COMMA (FX_POP);
}

/** (2LITERAL) ( -- x1 x2 )
 * runtime portion of => 2LITERAL
 */
FCode (p4_two_literal_execution)
{
    p4cell h;

    h = P4_POP (IP);
    FX_PUSH (P4_POP (IP));
    FX_PUSH (h);
}

/** 2LITERAL ( x1 x2 -- ) 
 * compile a double-cell number to the current definition. When
 * run, the doubele-cell is left on the stack for execution.
   ( -- x1 x2 )
 */
FCode (p4_two_literal)
{
    _FX_STATESMART_Q_COMP;
    if (STATESMART)
    {
        FX_COMPILE (p4_two_literal);
        FX_COMMA (DSP->hi);
        FX_COMMA (DSP->lo);
        SP += 2;
    }
}
P4COMPILES (p4_two_literal, p4_two_literal_execution,
  P4_SKIPS_DCELL, P4_DEFAULT_STYLE);

/** 2VARIABLE ( -- )
 * => CREATE a new variable definition. When executed leave
 * the =>">BODY" address on stack. In pfe, the data area
 * of a => 2VARIABLE is =>"ERASE"d initially.
 */
FCode (p4_two_variable)
{
    p4_header (p4_create_RT_ , 0);
    FX_COMMA (0);
    FX_COMMA (0);
}

/** D+ ( d1.ud1 d2.ud2 -- d3.ud3 )
 * the double-cell sum operation ( => + )
 */
FCode (p4_d_plus)
{
    p4_d_plus (&DSP[1], &DSP[0]);
    SP += 2;
}

/** D-( d1.ud1 d2.ud2 -- d3.ud3 )
 * the double-cell diff operation ( => - )
 */
FCode (p4_d_minus)
{
    p4_d_minus (&DSP[1], &DSP[0]);
    SP += 2;
}

/** D.R ( d1.d1 n -- )
 * aligned output for a double-cell number ( => .R )
 */
FCode (p4_d_dot_r)
{
    p4cell w = *SP++;
    int sign;

    if (*SP < 0)
    {
        sign = 1; p4_d_negate (&DSP[0]);
    }else
        sign = 0;
    FX (p4_less_sh);
    FX (p4_sh_s);
    if (sign)
        p4_hold ('-');
    FX (p4_sh_greater);
    p4_emits (w - *SP, ' ');
    FX (p4_type);
}

/** D. ( d1.d1 -- )
 * freefield output for a double-cell number ( => . )
 */
FCode (p4_d_dot)
{
    FX_PUSH (0);
    FX (p4_d_dot_r);
    FX (p4_space);
}

/** D0< ( d1.d1 -- flag )
 * the double-cell less-than-zero operation ( =>"0<" )
 */
FCode (p4_d_zero_less)
{
    SP[1] = P4_FLAG (SP[0] < 0);
    SP++;
}

/** D0= ( d1.d1 -- flag )
 * the double-cell equal-to-zero operation ( =>"0=" )
 */
FCode (p4_d_zero_equals)
{
    SP[1] = P4_FLAG (SP[0] == 0 && SP[1] == 0);
    SP++;
}

/** D2* ( d1.d1 -- d1.d1' )
 * the double-cell arithmetic shiftleft-by-1 operation ( =>"2*" )
 */
FCode (p4_d_two_star)
{
    p4_d_shiftleft ((p4dcell *) &SP[0], 1);
}

/** D2/ ( d1.d1 -- d1.d1' )
 * the double-cell arithmetic shiftright-by-1 operation ( =>"2/" )
 */
FCode (p4_d_two_slash)
{
    p4_d_shiftright ((p4dcell *) &SP[0], 1);
}

/** D< ( d1.d1 d2.d2 -- flag )
 * the double-cell is-less operation ( =>"<" )
 */
FCode (p4_d_less)
{
    SP[3] = P4_FLAG (p4_d_less (&DSP[1], &DSP[0]));
    SP += 3;
}

/** D>S ( d.d -- n )
 * result is the numeric equivalent of d. If the double number was
 * greater than what could fit into a single cell number, the 
 * modulo cellsize will be left since the higher-significant bits
 * are just =>"DROP"ed
 */
FCode (p4_d_to_s)
{
    SP++;
}

/** D= ( d1.d1 d2.d2 -- flag )
 * the double-cell is-equal operation ( =>"=" )
 */
FCode (p4_d_equals)
{
    SP[3] = P4_FLAG (SP[2] == SP[0] && SP[3] == SP[1]);
    SP += 3;
}

/** DABS ( d1.d1 -- d1.d1'  )
 * the double-cell abs operation ( =>"ABS" )
 */
FCode (p4_d_abs)
{
    if (*SP < 0)
        p4_d_negate (&DSP[0]);
}

/** DMAX ( d1.d1 d2.d2 -- d1.d1|d2.d2  )
 * the double-cell max operation ( =>"MAX" )
 */
FCode (p4_d_max)
{
    if (p4_d_less (&DSP[1], &DSP[0]))
        DSP[1] = DSP[0];
    SP += 2;
}

/** DMIN ( d1.d1 d2.d2 -- d1.d1|d2.d2  )
 * the double-cell max operation ( =>"MIN" )
 */
FCode (p4_d_min)
{
    if (p4_d_less (&DSP[0], &DSP[1]))
        DSP[1] = DSP[0];
    SP += 2;
}

/** DNEGATE ( d1.d1 -- d1.d1' )
 * the double-cell arithmetic negate operation ( =>"NEGATE" )
 */
FCode (p4_d_negate)
{
    p4_d_negate (&DSP[0]);
}

/** "M*\/" ( d1.d1 n1 +n2 -- d2.d2 )
 * the double-cell multiply-divide operation 
 * using a triple-cell intermediate result for => '*'
 * ( =>"*\/" )
 */
FCode (p4_m_star_slash)
{
    p4udcell lo, hi;
    p4cell p, q;
    udiv_t r1, r2;
    int sign = 0;

    if ((q = *SP++) < 0) { q = -q; sign ^= 1; }
    if ((p = *SP++) < 0) { p = -p; sign ^= 1; }
    if (*SP < 0) { p4_d_negate (&DSP[0]); sign ^= 1; }
    hi = p4_d_ummul (SP[0], p);
    lo = p4_d_ummul (SP[1], p);
    p4_um_plus ((p4dcell *) &hi, lo.hi);
    r1 = p4_d_umdiv (hi, q);
    lo.hi = r1.rem;
    r2 = p4_d_umdiv (lo, q);
    SP[0] = r1.quot;
    SP[1] = r2.quot;
    if (sign)
        p4_d_negate (&DSP[0]);
}

/** "M+" ( d1.d1 n1 -- d2.d2 )
 * the double-cell mixed-operand sum operation ( => + / => D+ )
 */
FCode (p4_m_plus)
{
    p4dcell b;
    b.lo = FX_POP;
    b.hi = ((p4cell)b.lo) < 0 ? -1 : 0;
    p4_d_plus ((p4dcell*) &SP[0], &b);
}

/** 2ROT ( d1.d1 d2.d2 d3.d3 -- d2.d2 d3.d3 d1.d1 )
 * the double-cell => ROT operation.
 * actively moves six cells, i.e.
   ( x1 x2 x3 x4 x5 x6 -- x3 x4 x5 x6 x1 x2 )
 */
FCode (p4_two_rot)
{
    p4cell h;

    h = SP[4];
    SP[4] = SP[2];
    SP[2] = SP[0];
    SP[0] = h;
    h = SP[5];
    SP[5] = SP[3];
    SP[3] = SP[1];
    SP[1] = h;
}


/** DU< ( d1.d1 d2.d2 -- flag )
 * the double-cell unsigned-is-less operation ( =>"U<" )
 */
FCode (p4_d_u_less)
{
    SP[3] = P4_FLAG (p4_d_u_less (&UDP[1], &UDP[0]));
    SP += 3;
}

/** "ENVIRONMENT MAX-D" ( -- d.d )
 * =>"ENVIRONMENT" definition to check with =>"ENVIRONMENT?"
 * if the double-cell wordset is present, the signed-max
 * number is left.
 */
static FCode (p__max_d)
{
    FX_PUSH (UINT_MAX);
    FX_PUSH (INT_MAX);
}

/** "ENVIRONMENT MAX-UD ( -- d.d )
 * =>"ENVIRONMENT" definition to check with =>"ENVIRONMENT?"
 * if the double-cell wordset is present, the unsigned-max
 * number is left.
 */
static FCode (p__max_ud)
{
    FX_PUSH (UINT_MAX);
    FX_PUSH (UINT_MAX);
}

P4_LISTWORDS (double) =
{
  CO ("2CONSTANT",	p4_two_constant),
  CS ("2LITERAL",	p4_two_literal),
  CO ("2VARIABLE",	p4_two_variable),
  CO ("D+",		p4_d_plus),
  CO ("D-",		p4_d_minus),
  CO ("D.",		p4_d_dot),
  CO ("D.R",		p4_d_dot_r),
  CO ("D0<",		p4_d_zero_less),
  CO ("D0=",		p4_d_zero_equals),
  CO ("D2*",		p4_d_two_star),
  CO ("D2/",		p4_d_two_slash),
  CO ("D<",		p4_d_less),
  CO ("D=",		p4_d_equals),
  CO ("D>S",		p4_d_to_s),
  CO ("DABS",		p4_d_abs),
  CO ("DMAX",		p4_d_max),
  CO ("DMIN",		p4_d_min),
  CO ("DNEGATE",	p4_d_negate),
  CO ("M*/",		p4_m_star_slash),
  CO ("M+",		p4_m_plus),
  CO ("2ROT",		p4_two_rot),
  CO ("DU<",		p4_d_u_less),
  P4_INTO ("ENVIRONMENT", 0),
  P4_OCON ("DOUBLE-EXT",	1994 ),
  P4_FXCO ("MAX-D",		p__max_d),
  P4_FXCO ("MAX-UD",		p__max_ud),
};
P4_COUNTWORDS (double, "Double number + extensions");

/*@}*/

