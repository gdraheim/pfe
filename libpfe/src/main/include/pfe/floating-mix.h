#ifndef PFE_FLOATING_MIX_H
#define PFE_FLOATING_MIX_H 1256212372
/* generated 2009-1022-1352 make-header.py ../../c/floating-mix.c */

#include <pfe/pfe-mix.h>

/**
 * -- miscellaneous useful extra words for FLOATING-EXT
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2003.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2008-04-20 04:46:31 $)
 *
 *  @description
 *      Compatiblity with former standards, miscellaneous useful words.
 *      ... for FLOATING-EXT
 */

#ifdef __cplusplus
extern "C" {
#endif




/** FP@ ( -- addr )
 * returns the floating point stack pointer
 */
extern void FXCode (p4_f_p_fetch);

/** FP! ( addr -- )
 * sets the floating point stack pointer -
 * this is the inverse of => FP@
 */
extern void FXCode (p4_f_p_store);

/** F? ( f: a b -- s: a==b )
 */
extern void FXCode (p4_f_equal);

/** F<> ( f: a b -- s: a!=b )
 */
extern void FXCode (p4_f_not_equal);

extern void FXCode (p4_f_less_than);

extern void FXCode (p4_f_greater_than);

extern void FXCode (p4_f_less_than_or_equal);

extern void FXCode (p4_f_greater_than_or_equal);

/** S>F  ( n -- f: x )
 * it's inverse is => F>S - convert a cell parameter to floating-point.
 */
extern void FXCode (p4_s_to_f);

/** FTRUNC>S  (f: x -- s: n )
 *
 * The word =>"F>S" was sometimes defined with a different behavior
 * than =>"FTRUNC>S" which is the type-cast behaviour of C according
 * to C99 section 6.3.1.4 - truncation would also match the ANS-Forth
 * specification for =>"F>D".
 *
 * Some systems used =>"F>S" defined to =>"FROUND>S" instead. The pfe
 * provides explicit words for both conversions, the word =>"FROUND>S"
 * and =>"FTRUNC>S" which return single-cell parameters for a floating
 * point number with the conversion method of => FTRUNC or => FROUND.
 *
 * In PFE, =>"F>S" is a synonym pointing to =>"FTRUNC>S" in analogy
 * of the behavior of =>"F>D" where no explicit word exists. The
 * inverse of =>"F>S" is the cast conversion of =>"S>F".
 */
extern void FXCode (p4_f_trunc_to_s);

/** FROUND>S (f: x -- s: n)
 * complements =>"FTRUNC>S" for applications that expect =>"F>S" to
 * be defined with a rounding behavior like
 : FROUND>S FROUND FTRUNC>S ;
 */
extern void FXCode (p4_f_round_to_s);

/** FTRUNC (f: x -- x' )
 * truncate towards zero, discard a fractional part. See also =>"FTRUNC>S"
 * conversion and the => FROUND and => FLOOR adaptors.
 : FTRUNC FDUP F0< IF FCEIL ELSE FLOOR THEN ;
 * (When available, uses a single call to C99 trunc() internally)
 */
extern void FXCode (p4_f_trunc);

/** -FROT  (f: x1 x2 x3 -- x3 x1 x2 )
 *
 * F-stack equivalent of => -ROT
 *
 * note, some systems call this work F-ROT,
 * here it is the inverse of => FROT
 */
extern void FXCode (p4_minus_f_rot);

/** FNIP  (f: x1 x2 -- x2 )
 *
 * F-stack equivalent of => NIP
 */
extern void FXCode (p4_f_nip);

/** FTUCK  (f: x1 x2 -- x2 x1 x2 )
 *
 * F-stack equivalent of => TUCK
 */
extern void FXCode (p4_f_tuck);

/** 1/F  (f: x -- 1/x )
 */
extern void FXCode (p4_one_over_f);

/** F^2  (f: x -- x^2 )
 */
extern void FXCode (p4_f_square);

/** F^N  ( u f: x -- x^u )
 * For large exponents, use F** instead.  Of course u=-1 is large.
 */
extern void FXCode (p4_f_power_n);

/** F2/  (f: x -- x/2 )
 */
extern void FXCode (p4_f_two_slash);

/** F2*  (f: x -- x*2 )
 */
extern void FXCode (p4_f_two_star);

/** F0>  (f: x -- s: flag )
*/
extern void FXCode (p4_f_zero_greater);

/** F0<>  (f: x -- s: flag )
*/
extern void FXCode (p4_f_zero_not_equal);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
