/** 
 * -- Words to open a shared code object
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2006 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.2 $
 *     (modified $Date: 2006-08-11 22:56:04 $)
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
"@(#) $Id: dl-hpux.c,v 1.2 2006-08-11 22:56:04 guidod Exp $";
#endif

#define _P4_SOURCE 1

#ifndef _export
#include <pfe/def-config.h>
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
p4_dlopenext (const char* name)
{
    void* lib;
    char libname[255];
    
    if (! name) return NULL;
    if (! p4_dlself ) p4_dlinit ();
    
    p4_strncpy (libname, name, 255);
    p4_strncat (libname, ".sl", 255);
    if (! memchr (libname, '\0', 255)) return NULL;
    
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

/*@}*/

