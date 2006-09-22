#ifndef _PFE_EXCEPTION_SUB_H
#define _PFE_EXCEPTION_SUB_H 1158897468
/* generated 2006-0922-0557 ../../pfe/../mk/Make-H.pl ../../pfe/exception-sub.c */

#include <pfe/pfe-sub.h>

/** 
 * --  Exception-oriented Subroutines.
 * 
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2006 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2006-09-22 04:43:03 $)
 */

#ifdef __cplusplus
extern "C" {
#endif


#define p4_longjmp_abort()	(p4_longjmp_loop('A'))
#define p4_longjmp_exit()	(p4_longjmp_loop('X'))
#define p4_longjmp_quit()	(p4_longjmp_loop('Q'))
#define p4_longjmp_yield()	(p4_longjmp_loop('S'))


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
