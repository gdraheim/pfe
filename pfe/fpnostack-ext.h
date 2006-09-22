#ifndef _PFE_FPNOSTACK_EXT_H
#define _PFE_FPNOSTACK_EXT_H 1158897469
/* generated 2006-0922-0557 ../../pfe/../mk/Make-H.pl ../../pfe/fpnostack-ext.c */

#include <pfe/pfe-ext.h>

/** 
 *  -- The No-FP-Stack Floating-Point Word Set
 * 
 *  Copyright (C) Krishna Myneni and Guido Draheim, 2002
 *  Copyright (C) 2005 - 2006 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Krishna Myneni              (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2006-09-22 04:43:03 $)
 *
 *  @description
 *         The No-FP-Stack Floating-Point Wordset is not usually
 *         used on embedded platforms. This Module implements
 *         the floating-point words but expects and puts the
 *         floating-point values on the forth parameter-stack.
 */

#ifdef __cplusplus
extern "C" {
#endif




extern P4_CODE (p4_nofp_d_f_align);

extern P4_CODE (p4_nofp_to_float);

extern P4_CODE (p4_nofp_to_float);

extern P4_CODE (p4_nofp_d_to_f);

extern P4_CODE (p4_nofp_f_store);

extern P4_CODE (p4_nofp_f_star);

extern P4_CODE (p4_nofp_f_plus);

extern P4_CODE (p4_nofp_f_minus);

extern P4_CODE (p4_nofp_f_slash);

extern P4_CODE (p4_nofp_f_zero_less);

extern P4_CODE (p4_nofp_f_zero_equal);

extern P4_CODE (p4_nofp_f_equal);

extern P4_CODE (p4_nofp_f_not_equal);

extern P4_CODE (p4_nofp_f_less_than);

extern P4_CODE (p4_nofp_f_greater_than);

extern P4_CODE (p4_nofp_f_less_than_or_equal);

extern P4_CODE (p4_nofp_f_greater_than_or_equal);

extern P4_CODE (p4_nofp_f_to_d);

/** S>F  ( n -- x )
 */
extern P4_CODE (p4_nofp_s_to_f);

/** FTRUNC>S  ( x -- n )
 */
extern P4_CODE (p4_nofp_f_trunc_to_s);

/** FROUND>S ( x -- n )
 */
extern P4_CODE (p4_nofp_f_round_to_s);

/** FTRUNC ( x -- x' )
 */
extern P4_CODE (p4_nofp_f_trunc);

/** -FROT  ( x1 x2 x3 -- x3 x1 x2 )
 */
extern P4_CODE (p4_nofp_minus_f_rot);

/** FNIP  ( x1 x2 -- x2 )
 */
extern P4_CODE (p4_nofp_f_nip);

/** FTUCK  ( x1 x2 -- x2 x1 x2 )
 */
extern P4_CODE (p4_nofp_f_tuck);

/** 1/F  ( x -- 1/x )
 */
extern P4_CODE (p4_nofp_one_over_f);

/** F^2  ( x -- x^2 )
 */
extern P4_CODE (p4_nofp_f_square);

/** F^N  ( x u -- x^u )
 * For large exponents, use F** instead.  Of course u=-1 is large. 
 */
extern P4_CODE (p4_nofp_f_power_n);

/** F2/  ( x -- x/2 )
 */
extern P4_CODE (p4_nofp_f_two_slash);

/** F2*  ( x -- x*2 )
 */
extern P4_CODE (p4_nofp_f_two_star);

/** F0>  ( x -- flag )
*/
extern P4_CODE (p4_nofp_f_zero_greater);

/** F0<>  ( x -- flag )
*/
extern P4_CODE (p4_nofp_f_zero_not_equal);

extern P4_CODE (p4_nofp_f_fetch);

extern P4_CODE (p4_nofp_f_constant_RT);

extern P4_CODE (p4_nofp_f_constant);

extern P4_CODE (p4_nofp_f_depth);

extern P4_CODE (p4_nofp_f_drop);

extern P4_CODE (p4_nofp_f_dup);

extern P4_CODE (p4_nofp_f_literal_execution);

extern P4_CODE (p4_nofp_f_literal);

extern P4_CODE (p4_nofp_floor);

extern P4_CODE (p4_nofp_f_max);

extern P4_CODE (p4_nofp_f_min);

extern P4_CODE (p4_nofp_f_negate);

extern P4_CODE (p4_nofp_f_over);

extern P4_CODE (p4_nofp_f_rot);

extern P4_CODE (p4_nofp_f_round);

extern P4_CODE (p4_nofp_f_swap);

extern P4_CODE (p4_nofp_f_variable_RT);

extern P4_CODE (p4_nofp_f_variable);

extern P4_CODE (p4_nofp_represent);

extern P4_CODE (p4_nofp_d_f_align);

extern P4_CODE (p4_nofp_d_f_aligned);

extern P4_CODE (p4_nofp_d_float_plus);

extern P4_CODE (p4_nofp_d_floats);

extern P4_CODE (p4_nofp_f_star_star);

extern P4_CODE (p4_nofp_f_dot);

extern P4_CODE (p4_nofp_f_abs);

extern P4_CODE (p4_nofp_f_e_dot);

extern P4_CODE (p4_nofp_f_s_dot);

extern P4_CODE (p4_nofp_f_proximate);

extern P4_CODE (p4_nofp_set_precision);

extern P4_CODE (p4_nofp_s_f_store);

extern P4_CODE (p4_nofp_s_f_fetch);

extern P4_CODE (p4_nofp_s_float_plus);

extern P4_CODE (p4_nofp_s_floats);

extern P4_CODE (p4_nofp_f_acos);

extern P4_CODE (p4_nofp_f_acosh);

extern P4_CODE (p4_nofp_f_alog);

extern P4_CODE (p4_nofp_f_asin);

extern P4_CODE (p4_nofp_f_asinh);

extern P4_CODE (p4_nofp_f_atan);

extern P4_CODE (p4_nofp_f_atan2);

extern P4_CODE (p4_nofp_f_atanh);

extern P4_CODE (p4_nofp_f_cos);

extern P4_CODE (p4_nofp_f_cosh);

extern P4_CODE (p4_nofp_f_exp);

extern P4_CODE (p4_nofp_f_expm1);

extern P4_CODE (p4_nofp_f_expm1);

extern P4_CODE (p4_nofp_f_ln);

extern P4_CODE (p4_nofp_f_lnp1);

extern P4_CODE (p4_nofp_f_lnp1);

extern P4_CODE (p4_nofp_f_log);

extern P4_CODE (p4_nofp_f_sin);

extern P4_CODE (p4_nofp_f_sincos);

extern P4_CODE (p4_nofp_f_sinh);

extern P4_CODE (p4_nofp_f_sqrt);

extern P4_CODE (p4_nofp_f_tan);

extern P4_CODE (p4_nofp_f_tanh);

/**
 * return double float-aligned address
 */
_extern  p4cell p4_nofp_dfaligned (p4cell n) ; /*{*/

/**
 *  used in engine
 */
_extern  int p4_nofp_to_float (const p4_char_t *p, p4cell n, double *r) ; /*{*/

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
