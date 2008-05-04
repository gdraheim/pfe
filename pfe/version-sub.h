#ifndef _VOL_8_SRC_CVS_PFE_33_PFE_VERSION_SUB_H
#define _VOL_8_SRC_CVS_PFE_33_PFE_VERSION_SUB_H 1209868837
/* generated 2008-0504-0440 /vol/8/src/cvs/pfe-33/pfe/../mk/Make-H.pl /vol/8/src/cvs/pfe-33/pfe/version-sub.c */

#include <pfe/pfe-sub.h>

/** 
 * -- Version File
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2003.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.5 $
 *     (modified $Date: 2008-05-04 02:57:30 $)
 *
 *  @description
 *      there are some internal functions in the version-file 
 *      that are used to print out compile-time, -date, -version 
 *      and the license of course.
 */

#ifdef __cplusplus
extern "C" {
#endif




_extern  P4_GCC_CONST const char* p4_version_string(void) ; /*{*/

_extern  P4_GCC_CONST const char* p4_copyright_string(void) ; /*{*/

_extern  P4_GCC_CONST const char* p4_license_string (void) ; /*{*/

_extern  P4_GCC_CONST const char* p4_warranty_string (void) ; /*{*/

_extern  P4_GCC_CONST const char* p4_compile_date (void) ; /*{*/

_extern  P4_GCC_CONST const char* p4_compile_time (void) ; /*{*/

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
