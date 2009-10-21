/**
 * -- definitions that may be missing in your system
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2000.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2008-04-20 04:46:30 $)
 *
 *  @description
 *              put here any definitions missing in your system -
 *		most of this file contributed by Marko Teiste
 *
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) =
"@(#) $Id: _missing.c,v 1.3 2008-04-20 04:46:30 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-limits.h>

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <unistd.h>
#include <assert.h>

#if defined HOST_OS_WATCOM
#include <i86.h>
#endif

#include <pfe/_nonansi.h>
#include <pfe/_missing.h>

/* ======================================================================== */

#ifdef VXWORKS
/* fixme: vxworks headers should be fixed!! */
#define stat(_X_,_Y_) (stat((char*)(_X_),(_Y_)))
#endif


#if !defined PFE_HAVE_ACCESS
#if defined linux && defined __GNUC__
#pragma warning configure problem, glibc has access() but it was not seen
#elif  defined unix && defined __GNUC__
#pragma warning every real unix has access() but it was not seen here
#endif

/** use as _P4_access, a #define from _missing.h */
int
_p4_access (const char *fn, int how)
{
# if defined PFE_HAVE_STAT
    struct stat st;		/* version using stat() - assuming that the */
    int result = 0;             /* files themselves are uid/gid of the task */
                                /* which is right on non-unix systems, uhmm */
    if (stat (fn, &st) != 0)
        return -1;
    if (how & F_OK)
        return result;
    if (how & X_OK && ! st.st_mode & S_IEXEC)        result = -1;
    if (how & R_OK && ! st.st_mode & S_IREAD)        result = -1;
    if (how & W_OK && ! st.st_mode & S_IWRITE)       result = -1;
    return result;
# else
    FILE *f;			/* limited version using ANSI-C fopen() */

    switch (how)
    {
     case 0:
     case F_OK:
     case R_OK:
         if ((f = fopen (fn, "r")) == NULL)
             return -1;
         fclose (f);
         return 0;
     case X_OK:
         return -1;
     default:
         if ((f = fopen (fn, "r+")) == NULL)
             return -1;
         fclose (f);
         return 0;
    }
# endif
}
#endif

/* ======================================================================== */

#ifndef PFE_HAVE_RENAME

/**
 * use _P4_rename from _missing.h
 *
 * Rename file - this is not a foolproof routine, one of those
 * "I'll do it better when I have more time" things. -mte
 */
int
_p4_rename (const char *source, const char *target)
{
    char save_name [PATH_LENGTH+1]; /* Name of saved file */

    if (_P4_access(target, F_OK) == 0)
    {
        sprintf (save_name, "%s~", target);
        if (_P4_access (save_name, F_OK) == 0
          || unlink (save_name) == -1
          || link (target, save_name) == -1
          || unlink (target) == -1)
            return -1;
    }
    if (link (source, target) == -1)
    {
        if (save_name != NULL)
            if (link (save_name, target) == -1)
                return -1;
            else
                unlink (save_name);
        return -1;
    }
    if (unlink (source) == -1)
        return -1;
    if (save_name != NULL)
        unlink (save_name);
    return 0;
}

#endif /*PFE_HAVE_RENAME*/

/*@}*/
