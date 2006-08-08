/** 
 * -- Process command line, get memory and start up.
 * 
 *  Copyright (C) Tektronix, Inc. 1999 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: bln_mpt1!1.14 %
 *    (%date_modified: Fri Mar 14 17:00:39 2003 %)
 *
 *  @description
 *  Process command line, get memory and start up the interpret loop of PFE
 */
/*@{*/

#include "pfe/_config.h"

#if defined PFE_WITH_STATIC_REGS
#include "main-alloc.c"
#elif defined PFE_WITH_STATIC_DICT
#include "main-static.c"
#else
#include "main-stdc.c"
#endif

/*@}*/

