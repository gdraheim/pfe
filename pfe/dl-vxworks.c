/** 
 * -- Words to open a shared code object
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE                %derived_by: guidod %
 *  @version %version: 1.13 %
 *    (%date_modified: Thu May 10 12:01:59 2001 %)
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
"@(#) $Id: dl-vxworks.c,v 1.1.1.1 2006-08-08 09:07:59 guidod Exp $";
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
p4_dlopenext (const char* name)
{
    int fd;
    MODULE_ID lib = 0;
    char libname[255];
    
    if (! name) return NULL;
    if (! p4_dlself ) p4_dlinit ();
    
    p4_strncpy (libname, name, 255);
    p4_strncat (libname, ".O", 255);
    
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
        if (p4_strlen(symbol) < 127) 
        {
            auto char _symbol[128];
            _symbol[0] = '_';  p4_strcpy (&_symbol[1], symbol);
            
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

/*@}*/

