#ifndef _PFE_DOUBLE_EXT_H
#define _PFE_DOUBLE_EXT_H 984413841
/* generated 2001-0312-1717 ../../pfe/../mk/Make-H.pl ../../pfe/double-ext.c */

#include <pfe/incl-ext.h>

#ifdef __cplusplus
extern "C" {
#endif




/** (2CONSTANT) ( -- x1 x2 )
 * runtime portion of => 2CONSTANT
 */
extern P4_CODE (p4_two_constant_RT);

/** 2CONSTANT ( x1 x2 "name" -- )
 * create a word that contains the specified twocell number in its body.
 * when the name is executed, these numbers are left on the stack
   12. 2CONSTANT X .s 
   <emtpy stack> ok
   X .s
   0 12 ok
 */
extern P4_CODE (p4_two_constant);

/** (2LITERAL) ( -- x1 x2 )
 * runtime portion of => 2LITERAL
 */
extern P4_CODE (p4_two_literal_execution);

/** 2LITERAL ( x1 x2 -- ) 
 * compile a double-cell number to the current definition. When
 * run, the doubele-cell is left on the stack for execution.
   ( -- x1 x2 )
 */
extern P4_CODE (p4_two_literal);

/** 2VARIABLE ( -- )
 * => CREATE a new variable definition. When executed leave
 * the =>">BODY" address on stack. In pfe, the data area
 * of a => 2VARIABLE is =>"ERASE"d initially.
 */
extern P4_CODE (p4_two_variable);

/** D+ ( d1.ud1 d2.ud2 -- d3.ud3 )
 * the double-cell sum operation ( => + )
 */
extern P4_CODE (p4_d_plus);

/** D-( d1.ud1 d2.ud2 -- d3.ud3 )
 * the double-cell diff operation ( => - )
 */
extern P4_CODE (p4_d_minus);

/** D.R ( d1.d1 n -- )
 * aligned output for a double-cell number ( => .R )
 */
extern P4_CODE (p4_d_dot_r);

/** D. ( d1.d1 -- )
 * freefield output for a double-cell number ( => . )
 */
extern P4_CODE (p4_d_dot);

/** D0< ( d1.d1 -- flag )
 * the double-cell less-than-zero operation ( =>"0<" )
 */
extern P4_CODE (p4_d_zero_less);

/** D0= ( d1.d1 -- flag )
 * the double-cell equal-to-zero operation ( =>"0=" )
 */
extern P4_CODE (p4_d_zero_equals);

/** D2* ( d1.d1 -- d1.d1' )
 * the double-cell arithmetic shiftleft-by-1 operation ( =>"2*" )
 */
extern P4_CODE (p4_d_two_star);

/** D2/ ( d1.d1 -- d1.d1' )
 * the double-cell arithmetic shiftright-by-1 operation ( =>"2/" )
 */
extern P4_CODE (p4_d_two_slash);

/** D< ( d1.d1 d2.d2 -- flag )
 * the double-cell is-less operation ( =>"<" )
 */
extern P4_CODE (p4_d_less);

/** D>S ( d.d -- n )
 * result is the numeric equivalent of d. If the double number was
 * greater than what could fit into a single cell number, the 
 * modulo cellsize will be left since the higher-significant bits
 * are just =>"DROP"ed
 */
extern P4_CODE (p4_d_to_s);

/** D= ( d1.d1 d2.d2 -- flag )
 * the double-cell is-equal operation ( =>"=" )
 */
extern P4_CODE (p4_d_equals);

/** DABS ( d1.d1 -- d1.d1'  )
 * the double-cell abs operation ( =>"ABS" )
 */
extern P4_CODE (p4_d_abs);

/** DMAX ( d1.d1 d2.d2 -- d1.d1|d2.d2  )
 * the double-cell max operation ( =>"MAX" )
 */
extern P4_CODE (p4_d_max);

/** DMIN ( d1.d1 d2.d2 -- d1.d1|d2.d2  )
 * the double-cell max operation ( =>"MIN" )
 */
extern P4_CODE (p4_d_min);

/** DNEGATE ( d1.d1 -- d1.d1' )
 * the double-cell arithmetic negate operation ( =>"NEGATE" )
 */
extern P4_CODE (p4_d_negate);

/** "M*\/" ( d1.d1 n1 +n2 -- d2.d2 )
 * the double-cell multiply-divide operation 
 * using a triple-cell intermediate result for => '*'
 * ( =>"*\/" )
 */
extern P4_CODE (p4_m_star_slash);

/** "M+" ( d1.d1 n1 -- d2.d2 )
 * the double-cell mixed-operand sum operation ( => + / => D+ )
 */
extern P4_CODE (p4_m_plus);

/** 2ROT ( d1.d1 d2.d2 d3.d3 -- d2.d2 d3.d3 d1.d1 )
 * the double-cell => ROT operation.
 * actively moves six cells, i.e.
   ( x1 x2 x3 x4 x5 x6 -- x3 x4 x5 x6 x1 x2 )
 */
extern P4_CODE (p4_two_rot);

/** DU< ( d1.d1 d2.d2 -- flag )
 * the double-cell unsigned-is-less operation ( =>"U<" )
 */
extern P4_CODE (p4_d_u_less);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
