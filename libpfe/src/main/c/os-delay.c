/**
 * -- os-delay -- delay in milli-seconds
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2008-04-20 04:46:31 $)
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) =
"@(#) $Id: os-delay.c,v 1.3 2008-04-20 04:46:31 guidod Exp $";
#endif

#include <pfe/pfe-base.h>
#include <pfe/def-limits.h>

#include <stdlib.h>

#ifdef PFE_HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef PFE_HAVE_WINBASE_H
#include <windows.h> /* Sleep */
#endif

/*
 * Somehow wait ms milli-seconds.
 */
_export void
p4_delay (int ms)
{
/*
 * Versions using poll and select according to Stevens'
 * "Advanced Programming in the UNIX Environment" p.705
 */
# if defined PFE_HAVE_DELAY
    delay (ms);
# elif defined EMX
    _sleep2 (ms);
# elif defined PFE_HAVE_USLEEP
    usleep (ms * 1000);
# elif defined PFE_HAVE_WINBASE_H
    Sleep (ms);
# elif defined PFE_HAVE_POLL
    static struct pollfd dummy = { 0, POLLHUP, POLLHUP };
    poll (&dummy, 1, ms);
# elif defined PFE_HAVE_SELECT || defined PFE_HAVE_SYS_SELECT_H
    struct timeval tval;
    tval.tv_sec = ms / 1000;
    tval.tv_usec = ms % 1000 * 1000;
    select (0, NULL, NULL, NULL, &tval);
# else
    sleep ((ms + 999) / 1000); /* full seconds */
# endif
}

/*@}*/
