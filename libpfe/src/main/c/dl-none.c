/**
 * -- Words to open a shared code object
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2008-04-20 04:46:30 $)
 *
 *  @description
 *		This file exports a set of system words for
 *              any OS that can dynamically bind object code to
 *		the interpreter. This part will then try to look
 *              up a symbol that can return a loadlist-table to
 *              be fed to the loadlist-loader routine.
 *
 *              this file is loaded when no dl-functionality
 *              could be detected. Therefore, nothing useful
 *              is done, and we define some dummy routines
 *              that will always fail.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) =
"@(#) $Id: dl-none.c,v 1.3 2008-04-20 04:46:30 guidod Exp $";
#endif

#define _P4_SOURCE 1

#ifndef _export
#include <pfe/def-config.h>
#endif

#include <errno.h>

/* no-dlopen: returns negative error code, description via strerror possible */
int
p4_dlinit(void)
{
#  if defined ENOSYS
    return -ENOSYS;
#  elif defined ENODEV
    return ENODEV;
#  elif defined ENOEXEC
    return -ENOEXEC;
#  else
    return -1;
#  endif
}

/* no-dlopen: describe the last dl-error. as there was no error, it returns null */
const char* p4_dlerror (void)
{
    return 0;
}

/* no-dlopen: nothing can be opened anyway, so just returns null */
void* p4_dlopenext (const char* name)
{
    return 0;
}

/* no-dlopen: nothing was ever opened, so it is a no op. */
int p4_dlclose (const void* lib)
{
    return 0;
}

/* no-dlopen: find symbol in loaded object - nothing to do */
void *
p4_dlsym (const void* lib, const char* symbol)
{
    return 0;
}

/*@}*/
