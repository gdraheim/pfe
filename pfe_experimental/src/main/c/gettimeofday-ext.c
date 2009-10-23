#define _P4_SOURCE 1
/*
 * -- gettimeofday from POSIX functions
 *
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author$)
 *  @version $Revision$
 *     (modified $Date$)
 *
 *  @description
 *  gettimeofday is incompatible with 16-bit systems as the numbers
 *  can not be properly represented, hence => TIME&DATE is more portable.
 *
 */

#include <pfe/pfe-base.h>
#include <pfe/def-limits.h>
#include <time.h>
#include <pfe/_nonansi.h>
#include <pfe/_missing.h>

#if defined PFE_HAVE_WINBASE_H
/* --target mingw32msvc */
# undef LP
# undef CONTEXT
# include <windows.h>
#endif

/**
 * helper function - both arg pointers MUST be given
 */
void
p4_gettimeofday (p4ucell* sec, p4ucell* usec)
{
# if defined PFE_HAVE_CLOCK_GETTIME
    struct timespec tv;
    clock_gettime (CLOCK_REALTIME, &tv);
    if (usec) *usec = tv.tv_nsec/1000;
    *sec  = tv.tv_sec;
# elif defined PFE_HAVE_WINBASE_H
    SYSTEMTIME stime;
    GetSystemTime (&stime);
    if (usec) *usec = stime.wMilliseconds*1000;
    *sec = time(0);
# elif defined PFE_HAVE_UNISTD_H || defined PFE_HAVE_GETTIMEOFDAY
    struct timeval tv;
    gettimeofday (&tv, 0);
    if (usec) *usec = tv.tv_usec;
    *sec = tv.tv_sec;
# else
    if (usec) *usec = 0;
    *sec = time(0);
# endif
};

/** GETTIMEOFDAY ( -- milliseconds# epochseconds# ) [EXT]
 * returns SVR/BSD gettimeofday(2).
 * Incompatible with 16-bit systems as the numbers can not be properly
 * represented, hence => TIME&DATE is more portable.
 */
static void FXCode (gettimeofday)
{
    FX_2ROOM;
    p4_gettimeofday ((p4ucell*) &SP[0], (p4ucell*) &SP[1]);
}

P4_LISTWORDSET (gettimeofday) [] =
{
    P4_INTO ("EXTENSIONS", 0),
    P4_FXco ("GETTIMEOFDAY",	gettimeofday),
};
P4_COUNTWORDSET (gettimeofday, "GETTIMEOFDAY-EXT extra words");

/*@}*/
