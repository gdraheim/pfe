/** 
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see    LGPL
 *  @author Tektronix CTE             @(#) %derived_by: guidod %
 *  @version %version: 5.11 %
 *    (%date_modified: Mon Mar 12 10:32:36 2001 %)
 *  @description
 *               Initial wordsets loaded at boottime.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: pfe-words.c,v 0.30 2001-03-12 09:32:36 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-xtra.h>

extern const p4Words
    P4WORDS(core),	P4WORDS(block),
    P4WORDS(double),	P4WORDS(exception),
    P4WORDS(cdecl),     P4WORDS(environ),
    P4WORDS(facility),	P4WORDS(file),
#ifndef P4_NO_FP
    P4WORDS(floating), 
#endif
    P4WORDS(locals),
    P4WORDS(memory),	P4WORDS(toolkit),
    P4WORDS(search),	P4WORDS(string),
    P4WORDS(forth_83),	P4WORDS(forth_usual),
    P4WORDS(misc),	P4WORDS(debug),
    P4WORDS(shell),	P4WORDS(posix),
    P4WORDS(term),      P4WORDS(dlfcn),
    P4WORDS(signals),   P4WORDS(system),
#ifdef PFE_WITH_SPY
    P4WORDS(with_spy),
#endif
#ifdef PFE_WITH_DSTRINGS_EXT
    P4WORDS(dstrings),
#endif
#ifdef PFE_WITH_GFORTH_EXT
    P4WORDS(gforth),
#endif
    P4WORDS(useful),    P4WORDS(your);

P4_LISTWORDS(forth) =
{
    P4_INTO ("ANS'", 0),
    P4_LOAD ("", search),
    P4_LOAD ("", core),
    P4_LOAD ("", block),
    P4_LOAD ("", double),
    P4_LOAD ("", exception),
    P4_LOAD ("", facility),
    P4_LOAD ("", file),
# ifndef P4_NO_FP
    P4_LOAD ("", floating),
# endif
    P4_LOAD ("", locals),
    P4_LOAD ("", memory),
    P4_LOAD ("", toolkit),
    P4_LOAD ("", string),
    P4_INTO ("FORTH", 0),
    P4_LOAD ("", cdecl),
    P4_LOAD ("", environ),
    P4_LOAD ("", dlfcn),
    P4_LOAD ("", forth_83),
    P4_LOAD ("", forth_usual), 
    P4_LOAD ("", misc), 	
    P4_LOAD ("", debug), 
    P4_LOAD ("", system),
    P4_INTO ("EXTENSIONS", 0),
    P4_LOAD ("", signals), 
};    
P4_COUNTWORDS(forth, "Forth Base system");

P4_LISTWORDS(extensions) =
{
    P4_INTO ("EXTENSIONS", "FORTH"),
    P4_LOAD ("", shell), 	
    P4_LOAD ("", posix), 
    P4_LOAD ("", term),
    P4_LOAD ("", useful),   
    P4_LOAD ("", your),
#ifdef PFE_WITH_SPY
    P4_LOAD ("", with_spy),
#endif
#ifdef PFE_WITH_DSTRINGS_EXT
    P4_LOAD ("", dstrings),
#endif
#ifdef PFE_WITH_GFORTH_EXT
    P4_LOAD ("", gforth),
#endif
};
P4_COUNTWORDS(extensions, "Extensions To Forth Base System");

/** ANS' ( -- )
 * the => VOCABULARY that holds only the standardwords.
 * see => FORTH for a definition that will search-also point to => ANS'
 */

/** FORTH ( -- )
 * in pfe it is not actually a => VOCABULARY anymore but 
 * behaves likewise. it will fetch the => VALUE from => FORTH-WORDLIST 
 * and execute it as if it were a => VOCABULARY
 */

/** EXTENSIONS ( -- )
 * in pfe it is not actually a => VOCABULARY anymore but 
 * behaves likewise. it will fetch the => VALUE from => EXTRA-WORDLIST 
 * and execute it as if it were a => VOCABULARY
 */

/* --------------------------------------------------------------- *
 * some old cruft
 */

#ifdef P4_MODULES 

#include "lined.h"

void*
p4_lined_executes (void* a)
{
    void* R = PFE.accept_lined.executes;
    PFE.accept_lined.executes = a;
    return R;
}

void* p4_wait_for_stdin (void* a)
{
    void* R = PFE.wait_for_stdin;
    PFE.wait_for_stdin = a;
    return R;
}

#endif /* P4_MODULES */

/*@}*/




