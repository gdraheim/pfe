/**
 * -- StackHelp for Memory-Alloc-Ext
 *
 *  Copyright (C) Tektronix, Inc. 2003 - 2003.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author$)
 *  @version $Revision$
 *     (modified $Date$)
 *
 *  @description
 *      see stackhelp-ext for details
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) =
"@(#) $Id$";
#endif

#define _P4_SOURCE 1
#include <pfe/pfe-base.h>


P4_LISTWORDSET (memory_check) [] =
{
    P4_STKi ("ALLOCATE", "size# -- ptr* 0 | 0 code#"),
    P4_STKi ("FREE",     "ptr*  -- 0 | code#"),
    P4_STKi ("RESIZE",   "ptr* newsize# -- ptr*' 0 | ptr* code#"),

};
P4_COUNTWORDSET (memory_check, "Check-Memory-Alloc words + extension");

/*@}*/
