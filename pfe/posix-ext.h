#ifndef _PFE_POSIX_EXT_H
#define _PFE_POSIX_EXT_H 984413843
/* generated 2001-0312-1717 ../../pfe/../mk/Make-H.pl ../../pfe/posix-ext.c */

#include <pfe/incl-ext.h>

/** 
 * -- Words making sense in POSIX-like systems only.
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE                %derived_by: guidod %
 *  @version %version: 5.12 %
 *    (%date_modified: Mon Mar 12 14:31:49 2001 %)
 *
 *  @description
 *		This file exports a set of system words for 
 *              a posixish OS environment. So should do
 *		any alternative wordset you might create for your OS.
 */

#ifdef __cplusplus
extern "C" {
#endif




/** #! ( "...<eol>" -- ) 
 * ignores the rest of the line,
 * defining `#!' is used to support forth scripts 
 * executed by the unix kernel
 */
extern P4_CODE (p4_ignore_line);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
