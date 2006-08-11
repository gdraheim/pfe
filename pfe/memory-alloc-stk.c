/** 
 * -- StackHelp for Memory-Alloc-Ext
 * 
 *  Copyright (C) Tektronix, Inc. 2003 - 2003.
 *  Copyright (C) 2005 - 2006 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.2 $
 *     (modified $Date: 2006-08-11 22:56:05 $)
 *
 *  @description
 *      see stackhelp-ext for details
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: memory-alloc-stk.c,v 1.2 2006-08-11 22:56:05 guidod Exp $";
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
