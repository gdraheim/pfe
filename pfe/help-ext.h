#ifndef _PFE_HELP_EXT_H
#define _PFE_HELP_EXT_H 1105051254
/* generated 2005-0106-2340 ../../../pfe/../mk/Make-H.pl ../../../pfe/help-ext.c */

#include <pfe/pfe-ext.h>

/** 
 * -- extra words for external HELP system
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version:  %
 *    (%date_modified:  %)
 *
 *  @description
 *      we grep the installed header files for comments ;-)
 */

#ifdef __cplusplus
extern "C" {
#endif




/** (HELP) ( str-ptr str-len -- )
 * display help for the specified word
 * (not functional yet)
 */
extern P4_CODE (p4_paren_help);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
