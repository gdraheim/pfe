#ifndef _VOL_8_SRC_CVS_PFE_33_PFE_FLOATING_EXT_H
#define _VOL_8_SRC_CVS_PFE_33_PFE_FLOATING_EXT_H 1209868837
/* generated 2008-0504-0440 /vol/8/src/cvs/pfe-33/pfe/../mk/Make-H.pl /vol/8/src/cvs/pfe-33/pfe/floating-ext.c */

#include <pfe/pfe-ext.h>

/** 
 *  -- The Optional Floating-Point Word Set
 * 
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.7 $
 *     (modified $Date: 2008-05-04 02:57:30 $)
 *
 *  @description
 *         The Optional Floating-Point Wordset is not usually
 *         used on embedded platforms. The PFE can be configured
 *         to even not allocate the separate floating-point stack
 *         that most of the floating-point words refer to.
 */

#ifdef __cplusplus
extern "C" {
#endif




extern P4_CODE (p4_d_f_align);

extern P4_CODE (p4_to_float);

extern P4_CODE (p4_to_float);

extern P4_CODE (p4_d_to_f);

extern P4_CODE (p4_f_store);

extern P4_CODE (p4_f_star);

extern P4_CODE (p4_f_plus);

extern P4_CODE (p4_f_minus);

extern P4_CODE (p4_f_slash);

extern P4_CODE (p4_f_zero_less);

extern P4_CODE (p4_f_zero_equal);

extern P4_CODE (p4_f_less_than);

/** ( f: a -- n,n )
 * b is the integer representation of a
 * 
 * we use truncation towards zero.
 * compare with =>"F>S" and its => "FROUND>S" / => "FTRUNC>S"
 */
extern P4_CODE (p4_f_to_d);

extern P4_CODE (p4_f_fetch);

extern P4_CODE (p4_f_constant_RT);

extern P4_CODE (p4_f_constant);

extern P4_CODE (p4_f_depth);

extern P4_CODE (p4_f_drop);

extern P4_CODE (p4_f_dup);

extern P4_CODE (p4_f_literal_execution);

extern P4_CODE (p4_f_literal);

extern P4_CODE (p4_floor);

extern P4_CODE (p4_f_max);

extern P4_CODE (p4_f_min);

extern P4_CODE (p4_f_negate);

extern P4_CODE (p4_f_over);

extern P4_CODE (p4_f_rot);

extern P4_CODE (p4_f_round);

extern P4_CODE (p4_f_swap);

extern P4_CODE (p4_f_variable_RT);

extern P4_CODE (p4_f_variable);

extern P4_CODE (p4_represent);

extern P4_CODE (p4_d_f_align);

extern P4_CODE (p4_d_f_aligned);

extern P4_CODE (p4_d_float_plus);

extern P4_CODE (p4_d_floats);

extern P4_CODE (p4_f_star_star);

extern P4_CODE (p4_f_dot);

extern P4_CODE (p4_f_abs);

extern P4_CODE (p4_f_e_dot);

extern P4_CODE (p4_f_s_dot);

extern P4_CODE (p4_f_proximate);

extern P4_CODE (p4_set_precision);

extern P4_CODE (p4_s_f_store);

extern P4_CODE (p4_s_f_fetch);

extern P4_CODE (p4_s_float_plus);

extern P4_CODE (p4_s_floats);

extern P4_CODE (p4_f_acos);

extern P4_CODE (p4_f_acosh);

extern P4_CODE (p4_f_alog);

extern P4_CODE (p4_f_asin);

extern P4_CODE (p4_f_asinh);

extern P4_CODE (p4_f_atan);

extern P4_CODE (p4_f_atan2);

extern P4_CODE (p4_f_atanh);

extern P4_CODE (p4_f_cos);

extern P4_CODE (p4_f_cosh);

extern P4_CODE (p4_f_exp);

extern P4_CODE (p4_f_expm1);

extern P4_CODE (p4_f_expm1);

extern P4_CODE (p4_f_ln);

extern P4_CODE (p4_f_lnp1);

extern P4_CODE (p4_f_lnp1);

extern P4_CODE (p4_f_log);

extern P4_CODE (p4_f_sin);

extern P4_CODE (p4_f_sincos);

extern P4_CODE (p4_f_sinh);

extern P4_CODE (p4_f_sqrt);

extern P4_CODE (p4_f_tan);

extern P4_CODE (p4_f_tanh);

extern P4_CODE (p4_interpret_float);

/**
 * return double float-aligned address
 */
_extern  p4cell p4_dfaligned (p4cell n) ; /*{*/

/**
 *  used in engine
 */
_extern  int p4_to_float (const p4_char_t *p, p4cell n, double *r) ; /*{*/

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
