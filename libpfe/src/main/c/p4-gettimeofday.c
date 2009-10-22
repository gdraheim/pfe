#define _P4_SOURCE 1
/*
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2008-04-20 04:46:30 $)
 */

#include <pfe/pfe-base.h>
#include <pfe/def-limits.h>
#include <pfe/p4-gettimeofday.h>
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
_export void
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
