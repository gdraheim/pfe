/** 
 * -- Words making sense in POSIX-like systems only.
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE                %derived_by: guidod %
 *  @version %version: 5.12 %
 *    (%date_modified: Mon Mar 12 14:31:49 2001 %)
 *
 *  @description
 *		This file exports a set of system words for 
 *              a posixish OS environment. So should do
 *		any alternative wordset you might create for your OS.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: posix-ext.c,v 0.30 2001-03-12 13:37:02 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-xtra.h>

#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <time.h>

#ifdef VxWorks
#include <sysLib.h>
#endif

#ifdef HAVE_WINBASE_H
/* --target mingw32msvc */
#undef OUT
#undef LP
#undef CONTEXT
#include <windows.h>
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
#define CLOCKS_PER_SEC sysClkRateGet()
#endif


#if !(defined SYS_EMX || defined SYS_WATCOM)

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

/** CLOCK ( --- ticks )	
 * return clock() 
 */
static
FCode (clock)			
{
    FX_PUSH(clock());
}

/** GETTIMEOFDAY ( -- double-time )
 * returns SVR/BSD gettimeofday(2). 
 * Never defined on 16-bit systems, hence
 * => TIME&DATE is more portable.
 */
static
FCode (gettimeofday)
{
# ifdef HAVE_VXWORKS_H
# define HAVE_CLOCK_GETTIME
# endif
 
# if defined HAVE_CLOCK_GETTIME
    struct timespec tv;
    clock_gettime(CLOCK_REALTIME, &tv);
    FX_PUSH(tv.tv_nsec/1000);
    FX_PUSH(tv.tv_sec);
# elif defined HAVE_WINBASE_H
    SYSTEMTIME stime;
    GetSystemTime(&stime);
    FX_PUSH (stime.wMilliseconds*1000);
    FX_PUSH (time(0));
# else
    struct timeval tv;
    gettimeofday(&tv, 0);
    FX_PUSH(tv.tv_usec);
    FX_PUSH(tv.tv_sec);
# endif
};

/** "ENVIRONMENT CLK_TCK" ( -- HZ )
 * the system's scheduler heartbeat clock (a.k.a. jiffies a.k.a. HZ)
 * for every function that expects time-values in ticks.
 */
static FCode(p4__clk_tck)
{
# if defined CLOCKS_PER_SEC
    FX_PUSH (CLOCKS_PER_SEC);
# elif defined CLK_TCK
    FX_PUSH (CLK_TCK);
# else
    /* including SYS_AIX1 */
    FX_PUSH (1000000); /* just a guess :-) */
# endif
}


P4_LISTWORDS (posix) =
{
# ifdef DEFINED_ignore_line
    CO ("#!",		  p4_ignore_line),
# endif
    CO ("CLOCK",	  clock),
    CO ("GETTIMEOFDAY",   gettimeofday),

    P4_INTO ("ENVIRONMENT", 0 ),
    P4_FXCO ("CLK_TCK",	  p4__clk_tck),
};
P4_COUNTWORDS (posix, "POSIX'like words");

/*@}*/

