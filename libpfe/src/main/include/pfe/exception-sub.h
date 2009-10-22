#ifndef PFE_EXCEPTION_SUB_H
#define PFE_EXCEPTION_SUB_H 1256209148
/* generated 2009-1022-1259 make-header.py ../../c/exception-sub.c */

#include <pfe/pfe-sub.h>

/**
 * --  Exception-oriented Subroutines.
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.6 $
 *     (modified $Date: 2008-05-05 02:04:52 $)
 */

#ifdef __cplusplus
extern "C" {
#endif


#define p4_longjmp_abort()	(p4_longjmp_loop('A'))
#define p4_longjmp_exit()	(p4_longjmp_loop('X'))
#define p4_longjmp_quit()	(p4_longjmp_loop('Q'))
#define p4_longjmp_yield()	(p4_longjmp_loop('S'))


extern P4_CODE (p4_cr_show_input);

/**
 * just call longjmp on PFE.loop
 */
_extern  void p4_longjmp_loop(int arg) ; /*{*/

/**
 * the CATCH impl
 */
_extern  int p4_catch (p4xt xt) ; /*{*/

_extern  void p4_throw (int id) ; /*{*/

_extern  void p4_throwstr (int id, const char* description) ; /*{*/

/**
 * the THROW impl
 */
_extern  void p4_throws (int id, const p4_char_t* description, int len) ; /*{*/

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
