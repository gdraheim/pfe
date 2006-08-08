/** 
 * -- Version File
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2003.  All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: bln_mpt1!33.19 %
 *    (%date_modified: Tue Jul 23 16:17:26 2002 %)
 *
 *  @description
 *      there are some internal functions in the version-file 
 *      that are used to print out compile-time, -date, -version 
 *      and the license of course.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: version-sub.c,v 1.1.1.1 2006-08-08 09:09:40 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-sub.h>
#include <pfe/def-macro.h>

#ifdef HAVE__VERSION_H
# include "_version.h"
# ifndef MAKETIME
# warning MAKETIME not declared in _version.h
# else  /* omit next-following include-statement */
# undef HAVE_VERSION_H
# endif
#endif

#ifdef HAVE_VERSION_H
# undef VXWORKS_VERSION
# include "version.h"
# ifdef VXWORKS_VERSION
# warning cpp found VX_Include<CPU>/version.h before Release/<TARGET>/version.h
# endif
# ifndef MAKETIME
# warning MAKETIME not declared in version.h
# endif
#endif
 
#ifndef MAKETIME
#define MAKETIME __TIME__
#endif

#ifndef MAKEDATE 
#define MAKEDATE __DATE__
#endif

#ifndef PFE_VERSION
#define PFE_VERSION "33.x"
#endif

_export const char* p4_version_string(void) P4_GCC_CONST
{
    return
	"Portable Forth Environment "PFE_VERSION" (" MAKEDATE " " MAKETIME ")";
}

_export const char* p4_copyright_string(void) P4_GCC_CONST
{
    return
	"\nCopyright (C) Dirk Uwe Zoller 1993 - 1995."
	"\nCopyright (C) Tektronix, Inc. 1998 - 2003.";
}

_export const char* p4_license_string (void) P4_GCC_CONST
{
    return
	"\n"
	"\nThis program is free software; you can redistribute it and/or"
	"\nmodify it under the terms of the GNU Library General Public"
	"\nLicense as published by the Free Software Foundation; either"
	"\nversion 2 of the License, or (at your option) any later version. ";
}

_export const char* p4_warranty_string (void) P4_GCC_CONST
{
    return
	"\n"
	"\nThis program is distributed in the hope that it will be useful,"
	"\nbut WITHOUT ANY WARRANTY; without even the implied warranty of"
	"\nMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU"
	"\nLibrary General Public License for more details."
	"\n"
	"\nYou should have received a copy of the GNU Library General Public"
	"\nLicense along with this program; if not, write to the Free Software"
	"\nFoundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. ";
}

_export const char* p4_compile_date (void) P4_GCC_CONST { return MAKEDATE; }
_export const char* p4_compile_time (void) P4_GCC_CONST { return MAKETIME; }

#if 0
/*deleted*/ p4ucell
p4_version_code (void)
/* return a magic-number identifiing the exact version */
{
    static char *str [] = { 
        p4_version_string, 
        p4_compile_date, 
        p4_compile_time 
    };
    p4ucell n = 0;
    int i;
    char *p;
    
    for (i = 0; i < DIM (str); i++)
        for (p = str [i]; *p; p++)
            n = n * 7 + *p - '@';
    return n;
}
#endif

/*@}*/
