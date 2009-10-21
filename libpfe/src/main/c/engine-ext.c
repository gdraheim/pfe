/**
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.6 $
 *     (modified $Date: 2009-05-31 21:32:47 $)
 *
 *  @description
 *               Initial wordsets loaded at boottime.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) =
"@(#) $Id: engine-ext.c,v 1.6 2009-05-31 21:32:47 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-limits.h>

extern const p4Words
    P4WORDS(core),		P4WORDS(core_misc),
    P4WORDS(block),	P4WORDS(block_misc),
    P4WORDS(doublez),	P4WORDS(double_misc),
    P4WORDS(exception), P4WORDS(cdecl),
    P4WORDS(environ),
    P4WORDS(facility),	P4WORDS(facility_mix),
    P4WORDS(file),		P4WORDS(file_misc),
    P4WORDS(header),	P4WORDS(locals),
    P4WORDS(memory),	P4WORDS(chainlist),
    P4WORDS(tools),	P4WORDS(tools_misc),
    P4WORDS(search),	P4WORDS(string),
    P4WORDS(forth_83),	P4WORDS(forth_usual),
    P4WORDS(misc),		P4WORDS(debug),
    P4WORDS(shell),	P4WORDS(posix),
    P4WORDS(term),		P4WORDS(dlfcn),
    P4WORDS(signals),	P4WORDS(system),
#if 1 && defined PFE_WITH_SPY
    P4WORDS(with_spy),
#endif
#if 1 && defined _K12_SOURCE
    P4WORDS(host_k12),
#endif
    P4WORDS(option),
    P4WORDS(useful),    P4WORDS(your);

extern void FXCode (p4_interpret_next);
extern FX_DEF_COMPILES (p4_interpret_next);
extern void FXCode (p4_interpret_find);
extern FX_DEF_COMPILES (p4_interpret_find);
extern void FXCode (p4_interpret_number);
extern FX_DEF_COMPILES (p4_interpret_number);
extern void FXCode (p4_interpret_nothing);
extern FX_DEF_COMPILES (p4_interpret_nothing);
extern void FXCode (p4_interpret_undefined);
extern FX_DEF_COMPILES (p4_interpret_undefined);

P4_LISTWORDSET (forth) [] =
{
    P4_INTO ("[ANS]", 0),
    P4_LOAD ("", search),
    P4_LOAD ("", core),
    P4_LOAD ("", block),
    P4_LOAD ("", doublez),
    P4_LOAD ("", exception),
    P4_LOAD ("", facility),
    P4_LOAD ("", file),
    P4_LOAD ("", locals),
    P4_LOAD ("", memory),
    P4_LOAD ("", tools),
    P4_LOAD ("", string),
    P4_INTO ("FORTH", 0),
    P4_LOAD ("", header),
    P4_LOAD ("", cdecl),
    P4_LOAD ("", environ),
    P4_LOAD ("", dlfcn),
    P4_LOAD ("", forth_83),
    P4_LOAD ("", forth_usual),
    P4_LOAD ("", core_misc),
    P4_LOAD ("", block_misc),
    P4_LOAD ("", double_misc),
    P4_LOAD ("", file_misc),
    P4_LOAD ("", tools_misc),
    P4_LOAD ("", misc),
    P4_LOAD ("", debug),
    P4_LOAD ("", system),
    P4_INTO ("EXTENSIONS", "FORTH"),
    P4_LOAD ("", option),
    P4_LOAD ("", signals),
    /* experimental extras for a compiled interpret-loop */
    P4_SXco ("INTERPRET-NEXT", p4_interpret_next),
    P4_SXco ("INTERPRET-FIND", p4_interpret_find),
    P4_SXco ("INTERPRET-NUMBER", p4_interpret_number),
    P4_SXco ("INTERPRET-NOTHING", p4_interpret_nothing),
    P4_SXco ("INTERPRET-UNDEFINED", p4_interpret_undefined),
    P4_DVaR ("INTERPRET-COMPILED", interpret_compiled),
    P4_DVaR ("INTERPRET-LOOP", interpret_loop),
};
P4_COUNTWORDSET(forth, "Forth Base system");

P4_LISTWORDSET(extensions) [] =
{
    P4_INTO ("EXTENSIONS", "FORTH"),
    P4_LOAD ("", facility_mix),
    P4_LOAD ("", chainlist),
    P4_LOAD ("", shell),
    P4_LOAD ("", posix),
    P4_LOAD ("", term),
    P4_LOAD ("", useful),
    P4_LOAD ("", your),
#if 1 && defined PFE_WITH_SPY
    P4_LOAD ("", with_spy),
#endif
#if 1 && defined _K12_SOURCE
    P4_LOAD ("", host_k12),
#endif
};
P4_COUNTWORDSET(extensions, "Extensions To Forth Base System");

/** [ANS] ( -- ) immediate
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

#include <pfe/lined.h>

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
