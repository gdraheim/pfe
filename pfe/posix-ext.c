/** 
 * -- Words making sense in POSIX-like systems only.
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2006 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.2 $
 *     (modified $Date: 2006-08-11 22:56:05 $)
 *
 *  @description
 *              This file exports a set of system words for 
 *              a posixish OS environment. So should do
 *              any alternative wordset you might create for your OS.
 *
 *              NOTE: this wordset is going to be removed soon.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: posix-ext.c,v 1.2 2006-08-11 22:56:05 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-limits.h>

#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <pfe/os-string.h>
#include <time.h>

#ifdef VxWorks
#include <sysLib.h>
#endif

#if defined PFE_HAVE_WINBASE_H 
/* --target mingw32msvc */
# undef LP
# undef CONTEXT
# include <windows.h>
#endif

/* ntohl() */
#ifndef HOST_WIN32
/* FIXME: #ifdef PFE_HAVE_NETINET_IN_H */
#include <netinet/in.h>
/* FIXME: #endif */
#else
/* well, here's a highlevel copy... */
/* (btw, #include <winsock.h> ... would make a dependency on -lws32 !!!) */
# if PFE_BYTEORDER+0 == 4321
# define ntohl(x)        (x)
# define ntohs(x)        (x)
# define htonl(x)        (x)
# define htons(x)        (x)
# else
# define ntohl(x)       ((((x) & 0x000000ff) << 24) | \
                         (((x) & 0x0000ff00) <<  8) | \
                         (((x) & 0x00ff0000) >>  8) | \
                         (((x) & 0xff000000) >> 24))

# define htonl(x)       ((((x) & 0x000000ff) << 24) | \
                         (((x) & 0x0000ff00) <<  8) | \
                         (((x) & 0x00ff0000) >>  8) | \
                         (((x) & 0xff000000) >> 24))

# define ntohs(x)       ((((x) & 0x00ff) << 8) | \
                         (((x) & 0xff00) >> 8))

# define htons(x)       ((((x) & 0x00ff) << 8) | \
                         (((x) & 0xff00) >> 8))
# endif
#endif

#include <pfe/_nonansi.h>
#include <pfe/_missing.h>
#include <pfe/logging.h>

#if 0
#define PFE_NTOHS_DIRECT 1
#endif

/** NTOHS ( w -- w' )
 * if current host-encoding is bigendian, this is a NOOP
 * otherwise byteswap the lower 16-bit bits of the topofstack.
 * see =>'W@' and =>'W!'
 * (on some platforms, the upper bits are erased, on others not)
 */ 
FCode (p4_ntohs)
{
# ifdef PFE_NTOHS_DIRECT
    *(unsigned short**)SP = ntohs (*(unsigned short**)SP);
# else
    register p4ucell item = *SP;
    *SP = ntohs (item);
# endif
}

/** NTOHL ( l -- l' )
 * if current host-encoding is bigendian, this is a NOOP
 * otherwise byteswap the lower 32-bit bits of the topofstack.
 * see =>'L@' and =>'L!' (being usually just =>'@' and =>'!' )
 * (on some platforms, the upper bits are erased, on others not)
 */ 
FCode (p4_ntohl)
{
# ifdef PFE_NTOHS_DIRECT
    *(unsigned long**)SP = ntohl (*(unsigned long**)SP);
# else
    register p4ucell item = *SP;
    *SP = ntohl (item);
# endif
}
 

P4_LISTWORDS (posix) =
{
    P4_INTO ("EXTENSIONS", 0),
    P4_FXco ("NTOHL",                   p4_ntohl),
    P4_FXco ("HTONL",                   p4_ntohl),
    P4_FXco ("NTOHS",                   p4_ntohs),
    P4_FXco ("HTONS",                   p4_ntohs),
};
P4_COUNTWORDS (posix, "POSIX'like words");

/*@}*/

