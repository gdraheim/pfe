#ifndef PFE_FACILITY_MIX_H
#define PFE_FACILITY_MIX_H 1256212372
/* generated 2009-1022-1352 make-header.py ../../c/facility-mix.c */

#include <pfe/pfe-mix.h>

/**
 * -- Words making sense in POSIX-like systems only.
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2008-04-20 04:46:30 $)
 *
 *  @description
 *              This file exports a set of system words for
 *              a posixish OS environment. So should do
 *              any alternative wordset you might create for your OS.
 */

#ifdef __cplusplus
extern "C" {
#endif




/** #! ( "...<eol>" -- )
 * ignores the rest of the line,
 * defining `#!' is used to support forth scripts
 * executed by the unix kernel
 */
extern void FXCode (p4_ignore_line);

/** CLOCK@ ( --- clock-ticks# ) [EXT]
 * return clock(2) - the number of clocks of this process.
 * To get the number of seconds, divide by => CLOCKS_PER_SEC
 * as represented in the => ENVIROMENT for a hosted forth system.
 * A similar scheme is used by => MS@ to compute the clock time
 * rounded to milliseconds (named in similarity with => MS sleep).
 *
 * Remember that the process clock will wrap around at some point,
 * therefore only use difference values between two clock reads.
 *
 * OLD: this was also called CLOCK up to PFE 0.33.x
 */
extern void FXCode (p4_clock_fetch);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
