/**
 * -- Words to open a shared prim-module pre-linked to the executable
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author$)
 *  @version $Revision$
 *     (modified $Date$)
 *
 *  @description
 *		This file exports a set of words referencing
 *              loader-wordsets of shared-lib prim-modules that
 *              have been pre-linked to the executable. It is
 *              formed to be a list of internal modules that are
 *              used by the dl-loader facilities.
 *              This internal-module-llist is commonly used if
 *              the target system does not support a nice way
 *              of creating shared-libs and binding them at runtime.
 *              It is a kind of libtool dlpreopen facility that is
 *              implemented here, in that it also works on some
 *              of the more peculiar embedded hostsystems around.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) =
"@(#) $Id$";
#endif

#define _P4_SOURCE 1

#ifndef _export
#include <pfe/def-config.h>
#endif

#include <pfe/def-words.h>

#ifdef PFE_DSTRINGS_EXT_C
extern const p4Words P4WORDS(dstrings);
#endif

#ifdef PFE_TOOLBELT_EXT_C
extern const p4Words P4WORDS(toolbelt);
#endif

#ifdef PFE_GFORTH_EXT_C
extern const p4Words P4WORDS(gforth);
#endif

#ifdef PFE_ZCHAR_EXT_C
extern const p4Words P4WORDS(zchar);
#endif

#if 0 && defined PFE_SPY_C
extern const p4Words P4WORDS(spy);
#endif

#ifdef PFE_EDIT_EXT_C
extern const p4Words P4WORDS(edit);
#endif

#ifdef PFE_STRUCT_EXT_C
extern const p4Words P4WORDS(struct);
#endif

#ifdef PFE_HELP_EXT_C
extern const p4Words P4WORDS(help);
#endif

#include <pfe/def-words.h>

P4_LISTWORDS(internal) =
{
    P4_INTO ("FORTH", 0),
# if defined PFE_DSTRINGS_EXT_C
    P4_LOAD ("dstrings", dstrings),
# endif
# if defined PFE_TOOLBELT_EXT_C
    P4_LOAD ("toolbelt", toolbelt),
# endif
# if defined PFE_GFORTH_EXT_C
    P4_LOAD ("gforth", gforth),
# endif
# if defined PFE_ZCHAR_EXT_C
    P4_LOAD ("zchar", zchar),
# endif
# if 0 && defined PFE_SPY_EXT_C
    P4_LOAD ("spy", spy),
# endif
# if defined PFE_EDIT_EXT_C
    P4_LOAD ("edit", edit),
# endif
# if defined PFE_STRUCT_EXT_C
    P4_LOAD ("struct", struct),
# endif
# if defined PFE_HELP_EXT_C
    P4_LOAD ("help", help),
# endif
};
P4_COUNTWORDS(internal, "Internal dynamic modules being pre-linked");

/*@}*/
