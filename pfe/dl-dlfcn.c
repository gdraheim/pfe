/** 
 * -- Words to open a shared code object
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2008-04-20 04:46:31 $)
 *
 *  @description
 *		This file exports a set of system words for 
 *              any OS that can dynamically bind object code to
 *		the interpreter. This part will then try to look
 *              up a symbol that can return a loadlist-table to
 *              be fed to the loadlist-loader routine.
 *
 *    linux/solaris, win, bsd, hpux, win16, win32, beos, libdld, 
 *    autoconf-dlpreopen - libltdl and libdl are very close in its 
 *    api functionality, hence prefer ifdef-noise in this file.
 *
 *    note that the dlopen() functionality is not only quite simple,
 *    it can also be implemented as a layer on top of most other
 *    foreign function interfaces of a system - and since this one
 *    is also in the unix98 specs we can expect dlfcn to become
 *    very very common. The only thing we have to watch out for:
 *    whether C symbols must be found with a leading underscore
 *    since that varies from platform to platform.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: dl-dlfcn.c,v 1.3 2008-04-20 04:46:31 guidod Exp $";
#endif

#define _P4_SOURCE 1

#ifndef _export
#include <pfe/def-config.h>
#endif

#ifdef PFE_HAVE_GNU_DLADDR
# ifndef _GNU_SOURCE
# define _GNU_SOURCE 1
# endif
#endif

#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <pfe/os-string.h>
#include <time.h>

#include <pfe/logging.h>

#ifndef PATH_MAX
# ifdef _POSIX_PATH_MAX
# define PATH_MAX _POSIX_PATH_MAX
# else
# define PATH_MAX 255
# endif
#endif

#if defined WITH_LTDL
#define USE_LTDL
#elif defined PFE_HAVE_DLFCN_H
#define USE_DLFCN
#else
#define USE_LTDL
#endif

#ifdef USE_LTDL
# include <ltdl.h>
#else
# include <dlfcn.h>
#endif

#ifndef PFE_NEED_USCORE    /* USER-CONFIG */
#define PFE_NEED_USCORE 0  /* whether override to find PFE module loadlist */
#endif

#ifndef RTLD_GLOBAL
#define RTLD_GLOBAL 0
#endif

#ifndef RTLD_NOW
#define RTLD_NOW 0
#endif

/** dlfcn: pointer to program symbol table */
static void* p4_dlself = 0;

/** dlfcn: init dl symbol table, dl error */
_export int 
p4_dlinit (void)
{
    if (! p4_dlself)  
    {
#      ifdef USE_LTDL
    	LTDL_SET_PRELOADED_SYMBOLS();
        lt_dlinit ();
        p4_dlself = lt_dlopen(0);
#      else
        p4_dlself = dlopen (0, RTLD_NOW|RTLD_GLOBAL);
#      endif
    }
    
    if (! p4_dlself)
        return -ENOEXEC;
    else
        return 0;
}

/** dlfcn: describe last dl-error */
_export const char* 
p4_dlerror (void)
{
#  ifdef USE_LTDL
    return lt_dlerror();
#  else
    return dlerror ();
#endif
}  

/** dlfcn: load shared-object into program codespace */
_export void* 
p4_dlopenext (const char* name)
{
    char libname[255];
    
    if (! name) return 0;
    if (! p4_dlself) p4_dlinit ();
    
    p4_strncpy (libname, name, 255);
    
#  ifdef USE_LTDL
    return (void*)lt_dlopenext(libname);
#  else
    p4_strncat (libname, ".so", 255);
    if (! memchr (libname, '\0', 255)) return 0;
    return (void*) dlopen (libname, RTLD_NOW|RTLD_GLOBAL);
#  endif
}

/** dlfcn: remove shared-object from program codespace */
_export int 
p4_dlclose (const void* lib)
{
#  ifdef USE_LTDL
    return lt_dlclose ((lt_dlhandle)lib);
#  else
    return dlclose ((void*)lib);
#  endif
}

/** dlfcn: find symbol in loaded object */
_export void* 
p4_dlsym (const void* lib, const char* symbol)
{
    if (! symbol) return 0;

#  ifdef USE_LTDL
    return lt_dlsym ((lt_dlhandle)lib, symbol);
#  else 

    {
#  if PFE_NEED_USCORE+0
	auto char _symbol[128];
	_symbol[0] = '_';  p4_strcpy (&_symbol[1], symbol);
#  define symbol _symbol
#  endif
	if (! lib)
	    return dlsym ((void*)p4_dlself , symbol);
	else
	    return dlsym ((void*)lib, symbol);
    }
#  endif
}

/** dlfcn: find name for address */
_export char*
p4_dladdr (void* addr, int* offset)
{
#ifdef PFE_HAVE_GNU_DLADDR
    auto Dl_info info;
    if (! dladdr (addr, &info))
	return 0;
    if (offset) *offset = (int) (addr - info.dli_saddr);
    return (char*)(info.dli_sname);
#else
    return 0;
#endif
}

/*@}*/

