/** 
 * -- definitions that may be missing in your system
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2000. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE             @(#) %derived_by: guidod %
 *  @version %version: 5.4 %
 *    (%date_modified: Mon Mar 12 10:31:58 2001 %)
 *
 *  @description
 *              put here any definitions missing in your system -
 *		most of this file contributed by Marko Teiste
 * 
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: _missing.c,v 0.30 2001-03-12 09:31:58 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-xtra.h>

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <unistd.h>
#include <assert.h>

#if defined SYS_WATCOM
#include <i86.h>
#endif

#include <pfe/_nonansi.h>
#include <pfe/_missing.h>

/* ========================================================================= */

#ifndef HAVE_STRDUP

#include <stdlib.h>
#include <string.h>

char *
strdup (const char *s)
{
    static char failed [] = "\n<calloc failed>\n";
    char *p = (char *) p4_calloc (1, strlen (s) + 1);
    return p ? strcpy (p, s) : failed;
}

#endif /*HAVE_STRDUP*/

#ifndef HAVE_MEMMOVE

void
memmove (char *d, const char *s, unsigned n)
{
    if (n)
    {
        if (s > d)
        {
            do {
                *d++ = *s++;
            } while (--n > 0);
        }else{
            do {
                --n;
                d [n] = s [n];
            } while (n > 0);
        }
    }
}

#endif /*HAVE_MEMMOVE*/

#ifndef HAVE_RENAME

int
rename (const char *source, const char *target)
/*
 * Rename file,
 * This is not a foolproof routine, one of those
 * "I'll do it better when I have more time" things. -mte
 */
{
    char save_name [PATH_LENGTH+1]; /* Name of saved file */

    if (access(target, 0) == 0)
    {
        sprintf (save_name, "%s~", target);
        if (access (save_name, 0) == 0 
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

#endif /*HAVE_RENAME*/

#ifndef HAVE_FCNTL
int
fcntl (int fildes,
       int cmd,
       ... /* arg */)
{
    return 0;
}
#endif /*HAVE_FCNTL*/

/*@}*/

