/**
 * -- Version File
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2003.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.5 $
 *     (modified $Date: 2008-04-20 04:51:55 $)
 *
 *  @description
 *      there are some internal functions in the version-file
 *      that are used to print out compile-time, -date, -version
 *      and the license of course.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) =
"@(#) $Id: version-sub.c,v 1.5 2008-04-20 04:51:55 guidod Exp $";
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

#ifndef MAKETIME
#define MAKETIME __TIME__
#endif

#ifndef MAKEDATE
#define MAKEDATE __DATE__
#endif

#ifndef PFE_VERSION
#define PFE_VERSION "33.x"
#endif

/** PFE-VERSION */
P4_GCC_CONST const char* p4_version_string(void)
{
    return
        "Portable Forth Environment "PFE_VERSION" (" MAKEDATE " " MAKETIME ")";
}

/** PFE-COPYRIGHT */
P4_GCC_CONST const char* p4_copyright_string(void)
{
    return
        "\nCopyright (C) Dirk Uwe Zoller  1993 - 1995."
        "\nCopyright (C) Tektronix, Inc.  1998 - 2003."
        "\nCopyright (C) Guido U. Draheim 2005 - 2008.";
}

/** PFE-LICENSE */
P4_GCC_CONST const char* p4_license_string (void)
{
    return
        "\n"
        "\nThis program is free software; you can redistribute it and/or"
        "\nmodify it under the terms of the GNU Library General Public"
        "\nLicense as published by the Free Software Foundation; either"
        "\nversion 2 of the License, or (at your option) any later version. ";
}

/** PFE-WARRANTY */
P4_GCC_CONST const char* p4_warranty_string (void)
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

/** PFE-COMPILE-DATE */
P4_GCC_CONST const char* p4_compile_date (void) { return MAKEDATE; }
/** PFE-COMPILE-TIME */
P4_GCC_CONST const char* p4_compile_time (void) { return MAKETIME; }

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
