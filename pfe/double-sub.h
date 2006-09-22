#ifndef _PFE_DOUBLE_SUB_H
#define _PFE_DOUBLE_SUB_H 1158897467
/* generated 2006-0922-0557 ../../pfe/../mk/Make-H.pl ../../pfe/double-sub.c */

#include <pfe/pfe-sub.h>

#ifdef __cplusplus
extern "C" {
#endif


#define dnegate p4_d_negate
#define dadd p4_d_plus
#define ummul p4_d_ummul
#define mmul p4_d_mmul
#define umdiv p4_d_umdiv
#define smdiv p4_d_smdiv
#define fmdiv p4_d_fmdiv


/** left shift of *a by n positions */
_extern  void p4_d_shiftleft (p4dcell *a, int n) ; /*{*/

/** arithm. right shift of *a by n positions */
_extern  void p4_d_shiftright (p4dcell *a, int n) ; /*{*/

/** add b to a */
_extern  void p4_um_plus (p4dcell * a, p4ucell b) ; /*{*/

/** add b to a */
_extern  void p4_d_plus (p4dcell * a, p4dcell * b) ; /*{*/

/** subtract b from a */
_extern  void p4_d_minus (p4dcell * a, p4dcell * b) ; /*{*/

/** negate a */
_extern  void p4_d_negate (p4dcell * a) ; /*{*/

/** result: a < b */
_extern  int p4_d_less (p4dcell * a, p4dcell * b) ; /*{*/

/** result: a < b */
_extern  int p4_d_u_less (p4udcell * a, p4udcell * b) ; /*{*/

/** unsigned multiply, mixed precision */
_extern  p4udcell p4_d_ummul (p4ucell a, p4ucell b) ; /*{*/

/** signed multiply, mixed precision */
_extern  p4dcell p4_d_mmul (p4cell a, p4cell b) ; /*{*/

/** unsigned divide procedure, mixed precision */
_extern  udiv_t p4_d_umdiv (p4udcell num, p4ucell denom) ; /*{*/

/** symmetric divide procedure, mixed precision */
_extern  fdiv_t p4_d_smdiv (p4dcell num, p4cell denom) ; /*{*/

/** floored divide procedure, mixed precision */
_extern  fdiv_t p4_d_fmdiv (p4dcell num, p4cell denom) ; /*{*/

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
