/** 
 * -- Words making sense in POSIX-like systems only.
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2006 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.2 $
 *     (modified $Date: 2006-08-11 22:56:04 $)
 *
 *  @description
 *              This file exports a set of system words for 
 *              a posixish OS environment. So should do
 *              any alternative wordset you might create for your OS.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: facility-mix.c,v 1.2 2006-08-11 22:56:04 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-limits.h>

#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <pfe/os-string.h>
#include <pfe/p4-gettimeofday.h>
#include <time.h>

#ifdef VxWorks
#include <sysLib.h>
#endif

#include <pfe/_nonansi.h>
#include <pfe/_missing.h>
#include <pfe/logging.h>

/** 
 * CLOCKS_PER_SEC - usually one million ticks, but can be
 * <i>very</i> different on a specific system. Exported
 * as a constant. see => CLOCK
 */  /*"CLK_TCK"*/
#ifdef  CLOCKS_PER_SEC_BUG
#undef  CLOCKS_PER_SEC
#define CLOCKS_PER_SEC sysClkRateGet()
#endif


#if !(defined SYS_EMX || defined HOST_OS_WATCOM)

#define DEFINED_ignore_line
/** #! ( "...<eol>" -- ) 
 * ignores the rest of the line,
 * defining `#!' is used to support forth scripts 
 * executed by the unix kernel
 */
FCode (p4_ignore_line)
{
    p4_refill ();
}

#endif

/** CLOCK@ ( --- clock-ticks# ) [EXT]
 * return clock(2) - the number of clocks of this proces.
 * To get the number of seconds, divide by CLOCKS_PER_SEC a.k.a. CLK_TCK
 * as represented in the => ENVIROMENT for a hosted forth system.
 *
 * Remember that the process clock will wrap around at some point, 
 * therefore only use difference values between two clock reads.
 */
FCode (p4_clock_fetch)                   
{
    /* mingw has it rerouted to GetClock ? */
    FX_PUSH(clock());
}

/** GETTIMEOFDAY ( -- milliseconds# epochseconds# ) [EXT]
 * returns SVR/BSD gettimeofday(2). 
 * Incompatible with 16-bit systems as the numbers can not be properly
 * represented, hence => TIME&DATE is more portable.
 */
static FCode (gettimeofday)
{
    FX_2ROOM;
    p4_gettimeofday ((p4ucell*) &SP[0], (p4ucell*) &SP[1]);
}

/** "ENVIRONMENT CLOCKS_PER_SEC" ( -- tick-count# ) [ENVIRONMENT]
 * the system's scheduler heartbeat clock 
 * (also known as jiffies or CLK_TCK or simply HZ)
 * for every function that expects time-values in ticks.
 */
static FCode(p4__clocks_per_sec)
{
# if defined CLOCKS_PER_SEC
# define  P4_CLOCKS_PER_SEC CLOCKS_PER_SEC
# elif defined CLK_TCK
# define  P4_CLOCKS_PER_SEC CLK_TCK
# else
    /* including HOST_OS_AIX1 - otherwise just a guess :-) */
# define P4_CLOCKS_PER_SEC  1000000
# endif
    FX_PUSH (P4_CLOCKS_PER_SEC);
}

/** MS@ ( -- milliseconds# ) [EXT]
 * elapsed time since start of process (or system) - in millseconds.
 * The granularity is per clockticks as per =>"ENVIRONMENT CLOCKS_PER_SEC"
 * For the current wallclock in milliseconds, ask =>"GETTIMEOFDAY".
 *
 * Remember that the process clock will wrap around at some point, 
 * therefore only use difference values between two clock reads.
 *
 * see also => CLOCK@ and => MS
 */
static FCode(p4_milliseconds_fetch)
{
    FX (p4_clock_fetch);
    FX_PUSH(1000000);
    FX (p4__clocks_per_sec);
    FX (p4_star_slash);
}

P4_LISTWORDS (facility_mix) =
{
    P4_INTO ("EXTENSIONS", 0),
# ifdef DEFINED_ignore_line
    P4_FXco ("#!",		p4_ignore_line),
# endif
    P4_FXco ("GETTIMEOFDAY",	gettimeofday),
    P4_FXco ("MS@",             p4_milliseconds_fetch),
    P4_FXco ("CLOCK@",		p4_clock_fetch),
    P4_xOLD ("CLOCK",		"CLOCK@"),

    P4_INTO ("ENVIRONMENT", 0 ),
    P4_FXCO ("CLOCKS_PER_SEC",	p4__clocks_per_sec),
    P4_xOLD ("CLK_TCK",		"CLOCKS_PER_SEC"),

};
P4_COUNTWORDS (facility_mix, "FACILITY-MIX extra words");

/*@}*/

