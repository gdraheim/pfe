/** 
 * -- Version File
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.  All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 6.3 %
 *    (%date_modified: Mon Mar 12 10:33:09 2001 %)
 *
 *  @description
 *      there are some internal functions in the version-file 
 *      that are used to print out compile-time, -date, -version 
 *      and the license of course.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: version-sub.c,v 0.30 2001-03-12 09:33:09 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/incl-sub.h>
#include <pfe/def-macro.h>

#ifdef HAVE_VERSION_H
#include "version.h"
#endif
 
#ifndef MAKETIME
#define MAKETIME __TIME__
#endif

#ifndef MAKEDATE 
#define MAKEDATE __DATE__
#endif

_export const char* p4_version_string()
{
    return
	"Portable Forth Environment "P4_VERSION" (" MAKEDATE " " MAKETIME ")";
}

_export const char* p4_copyright_string()
{
    return
	"\nCopyright (C) Dirk Uwe Zoller 1993 - 1995."
	"\nCopyright (C) Tektronix, Inc. 1998 - 2001.";
}

_export const char* p4_license_string ()
{
    return
	"\n"
	"\nThis program is free software; you can redistribute it and/or"
	"\nmodify it under the terms of the GNU Library General Public"
	"\nLicense as published by the Free Software Foundation; either"
	"\nversion 2 of the License, or (at your option) any later version. ";
}

_export const char* p4_warranty_string ()
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

_export const char* p4_compile_date () { return MAKEDATE; }
_export const char* p4_compile_time () { return MAKETIME; }

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
