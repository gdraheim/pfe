/** 
 * -- Words to open a shared code object
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE                %derived_by: guidod %
 *  @version %version: 1.9 %
 *    (%date_modified: Mon Mar 12 10:32:07 2001 %)
 *
 *  @description
 *		This file exports a set of system words for 
 *              any OS that can dynamically bind object code to
 *		the interpreter. This part will then try to look
 *              up a symbol that can return a loadlist-table to
 *              be fed to the loadlist-loader routine.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: dlfcn-sub.c,v 0.31 2001-03-20 01:31:34 guidod Exp $";
#endif

#define _P4_SOURCE 1

#ifndef _export
#include <pfe/def-config.h>
#endif

#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <time.h>

#include <pfe/logging.h>

#ifndef PATH_MAX
# ifdef _POSIX_PATH_MAX
# define PATH_MAX _POSIX_PATH_MAX
# else
# define PATH_MAX 255
# endif
#endif

#if defined HAVE_LTDL_H || defined WITH_LTDL || defined HAVE_DLFCN_H
   /* linux/solaris, win, bsd, hpux, win16, win32, beos, libdld, 
    * autoconf-dlpreopen - libltdl and libdl are very close in its 
    * api functionality, hence prefer ifdef-noise 
    */
#if defined WITH_LTDL
#define USE_LTDL
#elif defined HAVE_DLFCN_H
#define USE_DLFCN
#else
#define USE_LTDL
#endif

#ifdef USE_LTDL
# include <ltdl.h>
#else
# include <dlfcn.h>
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
p4_dlopen (const char* dir, const char* name)
{
    char libname[PATH_MAX+1];
    
    if (! name) return 0;
    if (! p4_dlself) p4_dlinit ();
    
    if (dir && dir[0]) 
    { 
        int len = strlen (dir);
        if (len > PATH_MAX-2) return NULL;
        memcpy (libname, dir, len+1);
        if (libname[len-1] != '/') strcat (libname+len-1, "/");
    } else {
        *libname = '\0';
    }
    
    strncat (libname, name, PATH_MAX);
    
#  ifdef USE_LTDL
    return (void*)lt_dlopenext(libname);
#  else
    strncat (libname, ".so", PATH_MAX);
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
    if (! lib)
        return dlsym ((void*)p4_dlself , symbol);
    else
        return dlsym ((void*)lib, symbol);
#  endif
}

/* --- */

#elif defined __target_os_hpux

#include <dl.h>

/* hp-specific: pointer to program symbol table */
static void* p4_dlself = 0;

/* hp-specific: init dl symbol table, dl error */
int 
p4_dlinit (void)
{
    p4_dlself = (void*)PROG_HANDLE;
    return 0;
}

/* hp-specific: describe last dl-error */
const char*
p4_dlerror (void)
{
    return strerror (errno);
}  

/* hp-specific: load shared-object into program codespace */
void* 
p4_dlopen (const char* dir, const char* name)
{
    void* lib;
    char libname[PATH_MAX+1];
    
    if (! name) return NULL;
    if (! p4_dlself ) p4_dlinit ();
    
    if (dir && dir[0]) 
    { 
        int len = strlen (dir);
        if (len > PATH_MAX-2) return NULL;
        memcpy (libname, dir, len+1);
        if (libname[len-1] != '/') strcat (libname+len-1, "/");
    } else {
        *libname = 0;
    }
    
    strncat (libname, name, PATH_MAX);
    strncat (libname, ".sl", PATH_MAX);
    
    lib = shl_load (libname, BIND_IMMEDIATE|BIND_NONFATAL, 0);
  
    return ((void*) lib);
}

/* hp-specific: remove shared-object from program codespace */
int 
p4_dlclose (const void* lib)
{
    return shl_unload ((shl_t)lib);
}

/* hp-specific: find symbol in loaded object */
void* 
p4_dlsym (const void* lib, const char* symbol)
{
    shl_t my_lib;
    void* val;
    
    if (! symbol) return NULL;
    
    if (! lib) 
        my_lib = (shl_t)p4_dlself ;
    else
        my_lib = (shl_t)lib;
    
    if (shl_findsym (&my_lib, symbol, 0, &val) == (-1)) 
        val = NULL;
    
    return val;
}

#elif defined SYS_vxworks || defined VxWorks || defined __target_os_vxworks

#include <ioLib.h>
#include <symLib.h>
#include <loadLib.h>
#include <unldLib.h>

/* vx-specific: pointer to program symbol table */
static void* p4_dlself = 0;

/* vx-specific: init dl symbol table and dl error */
int 
p4_dlinit (void)
{
    extern SYMTAB_ID sysSymTbl; /* the global SymTbl of loadModule */
    
    p4_dlself = (void*)sysSymTbl;
    
    return 0;
}

/* vx-specific: describe last dl-error */
const char*
p4_dlerror (void)
{
    return strerror (errno);
}  

/* vx-specific: load shared-object into program codespace */
void* 
p4_dlopen (const char* dir, const char* name)
{
    int fd;
    MODULE_ID lib = 0;
    char libname[PATH_MAX+1];
    
    if (! name) return NULL;
    if (! p4_dlself ) p4_dlinit ();
    
    if (dir && dir[0]) 
    { 
        int len = strlen (dir);
        if (len > PATH_MAX-2) return NULL;
        memcpy (libname, dir, len+1);
        if (libname[len-1] != '/') strcat (libname+len-1, "/");
    } else {
        *libname = 0;
    }
  
    strncat (libname, name, PATH_MAX);
    strncat (libname, ".O", PATH_MAX);
    
    fd = open (libname, O_RDONLY, 0);
    if (fd != ERROR) {
        P4_enter1("ld < '%s'", libname);
        lib = loadModule (fd, GLOBAL_SYMBOLS); /* or ALL_SYMBOLS *askmee*/
        P4_leave1("ld done = %p", lib);
        close (fd);
    }
  
    return ((void*) lib);
}

/* vx-specific: remove shared-object from program codespace */
int 
p4_dlclose (const void* lib)
{
    return ((int)
      (unldByModuleId ((MODULE_ID)lib, 0)));
}

/* vx-specific: find symbol in loaded object -
   BEWARE: vxworks uses the same symbol table for all shared-objects,
   so you may find a symbol from another lib - the argument is ignored! 
*/
void *
p4_dlsym (const void* lib, const char* symbol)
{
    void* val;
    lib=lib; /* lib is ignored - suppress compiler warning */
    
    if (! symbol) return 0;
    
    if (symFindByName ((SYMTAB_ID) p4_dlself, (char*) symbol, (char**) &val, 0)
      == ERROR)
    {
        if (strlen(symbol) < 127) 
        {
            auto char _symbol[128];
            _symbol[0] = '_';  strcpy (&_symbol[1], symbol);
            
            if (symFindByName ((SYMTAB_ID) p4_dlself, 
              _symbol, (char**) &val, 0)
              == ERROR)
            {
                val = 0;
            }
        }
    }
    
    return val;
}
#else 
 /* no system specific dl-loading found, so just skip it and define some
  * dummy routines that will always fail.
  */

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
void* p4_dlopen (const char* dir, const char* name)
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
#endif /* HAVE_P4_DLFCN layer */

/*@}*/

