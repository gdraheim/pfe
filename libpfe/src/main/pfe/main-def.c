/** 
 * -- Process command line, get memory and start up.
 * 
 *  Copyright (C) Tektronix, Inc. 1999 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2008-04-20 04:46:30 $)
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

