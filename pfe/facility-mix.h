#ifndef _PFE_FACILITY_MIX_H
#define _PFE_FACILITY_MIX_H 1105051253
/* generated 2005-0106-2340 ../../../pfe/../mk/Make-H.pl ../../../pfe/facility-mix.c */

#include <pfe/pfe-mix.h>

/** 
 * -- Words making sense in POSIX-like systems only.
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE                %derived_by: guidod %
 *  @version %version: 33.25 %
 *    (%date_modified: Mon Feb 24 20:15:33 2003 %)
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
extern P4_CODE (p4_ignore_line);

/** CLOCK@ ( --- clock-ticks# ) [EXT]
 * return clock(2) - the number of clocks of this proces.
 * To get the number of seconds, divide by CLOCKS_PER_SEC a.k.a. CLK_TCK
 * as represented in the => ENVIROMENT for a hosted forth system.
 *
 * Remember that the process clock will wrap around at some point, 
 * therefore only use difference values between two clock reads.
 */
extern P4_CODE (p4_clock_fetch);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
