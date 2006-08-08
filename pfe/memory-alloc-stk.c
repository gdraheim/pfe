/** 
 * -- StackHelp for Memory-Alloc-Ext
 * 
 *  Copyright (C) Tektronix, Inc. 2003 - 2003. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 33.11 %
 *    (%date_modified: %)
 *
 *  @description
 *      see stackhelp-ext for details
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: memory-alloc-stk.c,v 1.1.1.1 2006-08-08 09:09:05 guidod Exp $";
#endif

#define _P4_SOURCE 1
#include <pfe/pfe-base.h>


P4_LISTWORDS (memory_check) =
{
    P4_STKi ("ALLOCATE", "size# -- ptr* 0 | 0 code#"),
    P4_STKi ("FREE",     "ptr*  -- 0 | code#"),
    P4_STKi ("RESIZE",   "ptr* newsize# -- ptr*' 0 | ptr* code#"),

};
P4_COUNTWORDS (memory_check, "Check-Memory-Alloc words + extension");

/*@}*/

/* 
 * Local variables:
 * c-file-style: "stroustrup"
 * End:
 */
