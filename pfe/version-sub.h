#ifndef _PFE_VERSION_SUB_H
#define _PFE_VERSION_SUB_H 1105051254
/* generated 2005-0106-2340 ../../../pfe/../mk/Make-H.pl ../../../pfe/version-sub.c */

#include <pfe/pfe-sub.h>

/** 
 * -- Version File
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2003.  All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: bln_mpt1!33.19 %
 *    (%date_modified: Tue Jul 23 16:17:26 2002 %)
 *
 *  @description
 *      there are some internal functions in the version-file 
 *      that are used to print out compile-time, -date, -version 
 *      and the license of course.
 */

#ifdef __cplusplus
extern "C" {
#endif




_extern  const char* p4_version_string(void) P4_GCC_CONST ; /*{*/

_extern  const char* p4_copyright_string(void) P4_GCC_CONST ; /*{*/

_extern  const char* p4_license_string (void) P4_GCC_CONST ; /*{*/

_extern  const char* p4_warranty_string (void) P4_GCC_CONST ; /*{*/

_extern  const char* p4_compile_date (void) P4_GCC_CONST ; /*{*/

_extern  const char* p4_compile_time (void) P4_GCC_CONST ; /*{*/

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
