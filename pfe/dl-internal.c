/** 
 * -- Words to open a shared prim-module pre-linked to the executable
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE                %derived_by: guidod %
 *  @version %version: 1.19 %
 *    (%date_modified: Tue Jul 17 12:51:20 2001 %)
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
"@(#) $Id: dl-internal.c,v 1.1.1.1 2006-08-08 09:08:03 guidod Exp $";
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


/* K12/+.h API generated with a SWIG-module */
#ifdef PFE_K12_GENSRC_ENC_CONST_C
#include "gensrc/MBSlib.c"
#include "gensrc/codint.c"
#include "gensrc/enc_const.c"
#include "gensrc/rcsnd.c"
#include "gensrc/timeserv.c"
#include "gensrc/aal0cap.c"
#include "gensrc/dec_ftyp.c"
#include "gensrc/error.c"
#include "gensrc/realchrt.c"
#include "gensrc/types.c"
#endif

#ifdef PFE_K12_MFORTH_MFORTH_WORDS_C
#include "mforth/ipc-unix.c"
#include "mforth/ipc-ext.c"
#include "mforth/timer-unix.c"
#include "mforth/timer-ext.c"
#include "mforth/tmfkey-ext.c"
#include "mforth/tmtimer-ext.c"
#include "mforth/tm-ext.c"
#include "mforth/mforth-ext.c"
#include "mforth/mforth-words.c"
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

# ifdef PFE_K12_GENSRC_ENC_CONST_C
    P4_LOAD ("MBSlib", mbslib),
    P4_LOAD ("codint", codint),
    P4_LOAD ("enc_const", enc_const),
    P4_LOAD ("rcsnd", rcsnd),
    P4_LOAD ("timeserv", timeserv),
    P4_LOAD ("aal0cap", aal0cap),
    P4_LOAD ("dec_ftyp", dec_ftyp),
    P4_LOAD ("error", error),
    P4_LOAD ("realchrt", realchrt),
    P4_LOAD ("types", types),
# endif

#ifdef PFE_K12_MFORTH_MFORTH_WORDS_C
    P4_LOAD ("mforth", tmforth),
#endif
};
P4_COUNTWORDS(internal, "Internal dynamic modules being pre-linked");


/*@}*/

