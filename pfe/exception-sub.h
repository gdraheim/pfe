#ifndef _PFE_EXCEPTION_SUB_H
#define _PFE_EXCEPTION_SUB_H 1105095033
/* generated 2005-0107-1150 ../../../pfe/../mk/Make-H.pl ../../../pfe/exception-sub.c */

#include <pfe/pfe-sub.h>

/** 
 * --  Exception-oriented Subroutines.
 * 
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE            @(#) %derived_by: guidod %
 *  @version %version: bln_mpt1!1.22 %
 *    (%date_modified: Tue Jun 04 16:34:59 2002 %)
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

/**
 * the THROW impl
 */
_extern  void p4_throwstr (int id, const char* addr) ; /*{*/

/**
 * the THROW impl
 */
_extern  void p4_throws (int id, const p4_char_t* addr, int len) ; /*{*/

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
