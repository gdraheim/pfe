/**@name DOUBLE-EXT ---  The Optional Double Number Word Set
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: bln_mpt1!5.18 %
 *    (%date_modified: Mon Feb 24 20:25:21 2003 %)
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
"@(#) $Id: double-ext.c,v 1.1.1.1 2006-08-08 09:07:30 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-limits.h>
#include <pfe/def-comp.h>
#include <pfe/double-sub.h>
#include <pfe/os-string.h>

#define DSP	((p4dcell *)SP)
#define UDP	((p4udcell *)SP)

static P4_CODE_RUN(p4_two_constant_RT_SEE)
{
    p4_strcat (p, p4_str_d_dot_r (*(p4dcell*) P4_TO_BODY (xt), p+200, 0, BASE));
    p4_strcat (p, ". 2CONSTANT ");
    p4_strncat (p, (const char*) P4_NFA_PTR(nfa), P4_NFA_LEN(nfa));
    return 0;
}

/** (2CONSTANT) ( -- x1 x2 )
 * runtime portion of => 2CONSTANT
 */
FCode_RT (p4_two_constant_RT)
{   FX_USE_BODY_ADDR {
    FX_POP_BODY_ADDR_p4_BODY;
    FX_PUSH (p4_BODY[1]);
    FX_PUSH (p4_BODY[0]);
}}

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
    FX_RUNTIME_HEADER;
    FX_RUNTIME1 (p4_two_constant);
    FX_COMMA_ (FX_POP,'V');
    FX_COMMA_ (FX_POP,'v');
}
P4RUNTIMES1_(p4_two_constant, p4_two_constant_RT, 0,p4_two_constant_RT_SEE);

/** (2LITERAL) ( -- x1 x2 )
 * runtime portion of => 2LITERAL
 */
FCode_XE (p4_two_literal_execution)
{   FX_USE_CODE_ADDR {
    p4cell h;

    h = P4_POP (IP);
    FX_PUSH (P4_POP (IP));
    FX_PUSH (h);
    FX_USE_CODE_EXIT;
}}

/** 2LITERAL ( x1 x2 -- ) immediate
 * compile a double-cell number to the current definition. When
 * run, the doubele-cell is left on the stack for execution.
   ( -- x1 x2 )
 * (in most configurations this word is statesmart and it will do nothing
 *  in interpret-mode. See =>"2LITERAL," for a non-immediate variant)
 */
FCode (p4_two_literal)
{
    _FX_STATESMART_Q_COMP;
    if (STATESMART)
    {
        FX_COMPILE (p4_two_literal);
        FX_COMMA_ (DSP->hi,'V');
        FX_COMMA_ (DSP->lo,'v');
        SP += 2;
    }
}
P4COMPILES (p4_two_literal, p4_two_literal_execution,
  P4_SKIPS_DCELL, P4_DEFAULT_STYLE);

FCode_RT (p4_two_variable_RT) /* we need this for proper decompiling */
{                             /* otherwise identical with p4_variable_RT */
    FX_USE_BODY_ADDR;
    FX_PUSH_SP = (p4cell) FX_POP_BODY_ADDR;
}

/** 2VARIABLE ( -- )
 * => CREATE a new variable definition. When executed leave
 * the =>">BODY" address on stack. In pfe, the data area
 * of a => 2VARIABLE is =>"ERASE"d initially.
 */
FCode (p4_two_variable)
{
    FX_RUNTIME_HEADER;
    FX_RUNTIME1(p4_two_variable);
    FX_COMMA_ (0,'V');
    FX_COMMA_ (0,'v');
}
P4RUNTIME1(p4_two_variable, p4_two_variable_RT);

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
 * using a triple-cell intermediate result for =>'*'
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
    P4_INTO ("[ANS]", 0),
    P4_RTco ("2CONSTANT",	p4_two_constant),
    P4_RTco ("2VARIABLE",	p4_two_variable),
    P4_SXco ("2LITERAL",	p4_two_literal),
    P4_FXco ("D+",		p4_d_plus),
    P4_FXco ("D-",		p4_d_minus),
    P4_FXco ("D.",		p4_d_dot),
    P4_FXco ("D.R",		p4_d_dot_r),
    P4_FXco ("D0<",		p4_d_zero_less),
    P4_FXco ("D0=",		p4_d_zero_equals),
    P4_FXco ("D2*",		p4_d_two_star),
    P4_FXco ("D2/",		p4_d_two_slash),
    P4_FXco ("D<",		p4_d_less),
    P4_FXco ("D=",		p4_d_equals),
    P4_FXco ("D>S",		p4_d_to_s),
    P4_FXco ("DABS",		p4_d_abs),
    P4_FXco ("DMAX",		p4_d_max),
    P4_FXco ("DMIN",		p4_d_min),
    P4_FXco ("DNEGATE",		p4_d_negate),
    P4_FXco ("M*/",		p4_m_star_slash),
    P4_FXco ("M+",		p4_m_plus),
    P4_INTO ("ENVIRONMENT", 0),
    P4_OCON ("DOUBLE-EXT",	1994 ),
    P4_FXCO ("MAX-D",		p__max_d),
    P4_FXCO ("MAX-UD",		p__max_ud),
};
P4_COUNTWORDS (double, "Double number + extensions");

/*@}*/

