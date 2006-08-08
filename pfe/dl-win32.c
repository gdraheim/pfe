/** 
 * -- Words to open a shared code object
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE                %derived_by: guidod %
 *  @version %version: bln_mpt1!1.14 %
 *    (%date_modified: Tue Aug 13 12:48:38 2002 %)
 *
 *  @description
 *		This file exports a set of system words for 
 *              any OS that can dynamically bind object code to
 *		the interpreter. This part will then try to look
 *              up a symbol that can return a loadlist-table to
 *              be fed to the loadlist-loader routine.
 *
 *              implementation for win32 using
 *              LoadLibrary GetProcAddress FreeLibrary
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: dl-win32.c,v 1.1.1.1 2006-08-08 09:08:49 guidod Exp $";
#endif

#ifndef _P4_SOURCE
#define _P4_SOURCE 1
#endif

#ifndef _export
#include <pfe/def-config.h>
#endif

#ifdef __GNUC__
#warning using dl-win32.c, now including windows.h
#endif

#undef OUT
#undef LP
#undef CURRENT
#undef CONTEXT
#include <windows.h> /* <winbase.h> */

/** win32: pointer to program symbol table */
static HMODULE p4_dlself = 0;

/** win32: init dl symbol table, dl error */
_export int 
p4_dlinit (void)
{
    if (! p4_dlself)  
    {
        p4_dlself = GetModuleHandle (NULL);
    }
    
    if (! p4_dlself)
        return -1;
    else
        return 0;
}

/** win32: describe last dl-error */
_export const char* 
p4_dlerror (void)
{
    return "(N/A)"; /* FIXME: */
}  

/** win32: load shared-object into program codespace */
_export void* 
p4_dlopenext (const char* name)
{
    char libname[255];
    
    if (! name) return 0;
    if (! p4_dlself) p4_dlinit ();

    p4_strncpy (libname, name, 255);
    p4_strncat (libname, ".dll", 255);

    { char* p; while ((p=p4_strchr(libname, '/'))) *p = '\\'; }
    if (! memchr (libname, '\0', 255)) return 0;
    return (void*) LoadLibrary (libname);
}

/** win32: remove shared-object from program codespace */
_export int 
p4_dlclose (const void* lib)
{
    return FreeLibrary ((HMODULE)(lib));
}

/** win32: find symbol in loaded object */
_export void* 
p4_dlsym (const void* lib, const char* symbol)
{
    if (! symbol) return 0;

    if (! lib)
        return GetProcAddress ((HMODULE)(p4_dlself), symbol);
    else
        return GetProcAddress ((HMODULE)(lib), symbol);
}

/*@}*/

