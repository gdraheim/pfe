#ifndef _PFE_EXCEPTION_SUB_H
#define _PFE_EXCEPTION_SUB_H 984413841
/* generated 2001-0312-1717 ../../pfe/../mk/Make-H.pl ../../pfe/exception-sub.c */

#include <pfe/incl-sub.h>

/** 
 * --  Exception-oriented Subroutines.
 * 
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE            @(#) %derived_by: guidod %
 *  @version %version: 1.7 %
 *    (%date_modified: Mon Mar 12 10:32:16 2001 %)
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

/**
 * the THROW impl
 */
_extern  void p4_throws (int id, const char* addr, int len) ; /*{*/

_extern  void p4_throw (int id) ; /*{*/

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
