#ifndef _PFE_VERSION_SUB_H
#define _PFE_VERSION_SUB_H 984413847
/* generated 2001-0312-1717 ../../pfe/../mk/Make-H.pl ../../pfe/version-sub.c */

#include <pfe/incl-sub.h>

/** 
 * -- Version File
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.  All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 6.3 %
 *    (%date_modified: Mon Mar 12 10:33:09 2001 %)
 *
 *  @description
 *      there are some internal functions in the version-file 
 *      that are used to print out compile-time, -date, -version 
 *      and the license of course.
 */

#ifdef __cplusplus
extern "C" {
#endif




_extern  const char* p4_version_string() ; /*{*/

_extern  const char* p4_copyright_string() ; /*{*/

_extern  const char* p4_license_string () ; /*{*/

_extern  const char* p4_warranty_string () ; /*{*/

_extern  const char* p4_compile_date () ; /*{*/

_extern  const char* p4_compile_time () ; /*{*/

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
