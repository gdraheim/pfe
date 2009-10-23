/**
 * -- Words making sense in POSIX-like systems only.
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author$)
 *  @version $Revision$
 *     (modified $Date$)
 *
 *  @description
 *              This file exports a set of system words for
 *              a posixish OS environment. So should do
 *              any alternative wordset you might create for your OS.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) =
"@(#) $Id$";
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

#include <pfe/_nonansi.h>
#include <pfe/_missing.h>
#include <pfe/logging.h>

/**
 * CLOCKS_PER_SEC - usually one million ticks, but can be
 * <i>very</i> different on a specific system. Exported
 * as a constant. see => CLOCK
 */
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
void FXCode (p4_ignore_line)
{
    p4_refill ();
}

#endif

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
void FXCode (p4_clock_fetch)
{
    /* mingw has it rerouted to GetClock ? */
    FX_PUSH(clock());
}

/** "ENVIRONMENT CLOCKS_PER_SEC" ( -- tick-count# ) [ENVIRONMENT]
 * the system's scheduler heartbeat clock
 * (also known as jiffies or simply HZ)
 * for every function that expects time-values in ticks.
 */
static void FXCode(p4__clocks_per_sec)
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
 * The granularity is per clock ticks as per =>"ENVIRONMENT CLOCKS_PER_SEC"
 * For the current wall clock in milliseconds, ask =>"GETTIMEOFDAY".
 *
 * Remember that the process clock will wrap around at some point,
 * therefore only use difference values between two clock reads.
 * Also note that on many desktop systems the process scheduler ticks
 * per 1/100s or 1/60s so that the difference of two continuous reads
 * of the process clock will show steps of 10 to 16 milliseconds.
 *
 * see also => CLOCK@ and => MS
 */
static void FXCode(p4_milliseconds_fetch)
{
    FX (p4_clock_fetch);
    FX_PUSH(1000000);
    FX (p4__clocks_per_sec);
    FX (p4_star_slash);
}

P4_LISTWORDSET (facility_mix) [] =
{
    P4_INTO ("EXTENSIONS", 0),
# ifdef DEFINED_ignore_line
    P4_FXco ("#!",		p4_ignore_line),
# endif
    P4_FXco ("MS@",             p4_milliseconds_fetch),
    P4_FXco ("CLOCK@",		p4_clock_fetch),

    P4_INTO ("ENVIRONMENT", 0 ),
    P4_FXCO ("CLOCKS_PER_SEC",	p4__clocks_per_sec),
};
P4_COUNTWORDSET (facility_mix, "FACILITY-MIX extra words");

/*@}*/
