/**
 * (C) 2000 - 2001 Guido Draheim
 *
 * @see LGPL for license
 * @description
 *	this wordset implements a simple module-system.
 *      the basic module-system is based on the three
 *      words MODULE/END-MODULE/EXPORT - these three were
 *      present TEK' mforth, here the MPE' forth is used
 *      as an example for the naming scheme. This 
 *      implementation has also mpe's EXPOSE-MODULE,
 *      and it has a serious form of mpe's REQUIRES
 *      under the name ALSO-MODULE which clarifies that
 *      the search-order is modified. Note also that
 *      pfe defines a REQUIRES in require-ext that works
 *      like INCLUDE-ONCE
 */

#define _P4_SOURCE 1
#include <pfe/pfe-base.h>
#include <pfe/os-string.h>
#include <pfe/forth-83-ext.h>
#include <pfe/forth-usual-ext.h>

/* the name of the hidden wordlist in a module vocabulary */
static const p4_char_t p4_lit_HIDDEN[] = "HIDDEN'";

/** MODULE ( "name" -- old-current )
 * create a new => WORDLIST with the given name. It
 * will also have an implicit hidden vocabulary just as
 * well and all => DEFINITIONS will go into that
 * hidden wordlist. Therefore the old => CURRENT is
 * memorized on the cs-stack.
 *
 * effectivly, CONTEXT[1] will have the wordlist-id
 * of the public wordlist "name" and CONTEXT[0] will
 * have the hidden wordlist contained in "name" - the
 * hidden wordlist will always be known as HIDDEN' so
 * that it can be re-referenced without need to use
 * => ALSO just to access a single definition from 
 * just another vocabulary. Note that HIDDEN' is 
 * defined immediate (a => VOCABULARY' ) to modify
 * the => ORDER inside a colon definition.
 : MODULE
   CURRENT @ ( -- old-current )
   VOCABULARY
   ALSO LATEST NAME> EXECUTE ALSO DEFINITIONS
   C" HIDDEN'" $CREATE WORDLIST CONTEXT !
 ;
 */
FCode (p4_module)
{
    FX_PUSH (CURRENT);
    FX (p4_also); FX (p4_also);
    FX_HEADER; 
    FX_RUNTIME1 (p4_vocabulary);
    CONTEXT[1] = CURRENT = p4_make_wordlist (LAST);
    p4_header_comma(p4_lit_HIDDEN, sizeof p4_lit_HIDDEN -1, CURRENT); 
    FX_IMMEDIATE;
    FX_RUNTIME1 (p4_vocabulary);
    CONTEXT[0] = CURRENT = p4_make_wordlist (LAST);
}

/** END-MODULE ( old-current -- )
 * clean up the cs-stack from the last => MODULE 
 * definition. Effectivly, MODULE definitions can
 * be nested.
 : END-MODULE ( old-current )
   PREVIOUS PREVIOUS CURRENT ! 
 */
FCode (p4_end_module)
{
    FX (p4_previous);
    FX (p4_previous);
    CURRENT = (Wordl*) FX_POP;
}

/** EXPORT ( old-current "name" -- old-current )
 * the named word in the hidden dictionary (i.e. 
 * the wordlist referenced in =>"CURRENT") is exported
 * into the public wordlist of it (i.e. which is in
 * this implementation =>"CONTEXT"[1]). The actual
 * implemenation will create a =>"DEFER"-word in the
 * public wordlist withits parameter area pointing
 * to the cfa of the hidden implementation.
 : EXPORT
   CURRENT @ CONTEXT CELL+ @ CURRENT !
   DEFER CURRENT !
   LATEST COUNT CURRENT @ SEARCH-WORDLIST
   IF LATEST NAME> >BODY ! ELSE ABORT" can't find word to export" THEN
 ;
 */
FCode (p4_export)
{
    extern FCode (p4_defer);
    register Wordl* hidden; 
    register p4char* nfa;

    FX (p4_Q_exec);
    hidden = CURRENT; CURRENT = CONTEXT[1];
    FX (p4_defer);    CURRENT = hidden;
    nfa = p4_search_wordlist (PFE.last+1, NFACNT(*PFE.last), hidden);
    if (! nfa) { p4_abortq ("can't find word to export "); }
    *p4_to_body (p4_name_from(PFE.last)) = /* DEFER BODY !! => IS */
        (p4cell) p4_name_from(nfa);
}

/** EXPOSE-MODULE ( "name" -- )
 * affects the search order, ALSO module-wid CONTEXT ! hidden'
 : EXPOSE-MODULE 
    ALSO S" HIDDEN'" 
    ' DUP VOC? ABORT?" is no vocabulary" >VOC 
    SEARCH-WORDLIST 0= IF " no hidden vocabulary found" THEN
    DUP VOC? ABORT?" hidden is no vocabulary" EXECUTE
 ;
 */
FCode (p4_expose_module)
{
    register p4_namebuf_t* nfa;
    auto p4xt xt;

    xt = p4_tick_cfa (FX_VOID);
    if (*P4_TO_CODE(xt) != PFX(p4_vocabulary_RT)) 
    { p4_abortq ("is no vocabulary"); }
    nfa = p4_search_wordlist (p4_lit_HIDDEN, sizeof p4_lit_HIDDEN -1, 
			      p4_to_wordlist(xt));
    if (! nfa) 
    { p4_abortq ("no hidden vocabulary found"); }
    xt = p4_name_from(nfa);
    if (*P4_TO_CODE(xt) != PFX(p4_vocabulary_RT)) 
    { p4_abortq ("hidden is no voc"); }
    FX (p4_also);
    CONTEXT[0] = p4_to_wordlist(xt);
}

/** ALSO-MODULE ( "name" -- )
 * affects the search-order, ALSO module-wid CONTEXT ! 
 : ALSO-MODULE
   ' DUP VOC? ABORT?" is no vocabulary" 
   ALSO EXECUTE
 ;
 */
FCode (p4_also_module)
{
    register p4xt xt = p4_tick_cfa (FX_VOID);
    if (*P4_TO_CODE(xt) != PFX(p4_vocabulary_RT)) 
    { p4_abortq ("is no vocabulary"); }
    FX (p4_also);
    CONTEXT[0] = p4_to_wordlist(xt);
}

P4_LISTWORDS(module) =
{
    P4_INTO ("EXTENSIONS", 0),
    P4_FXco ("MODULE",               p4_module),
    P4_FXco ("END-MODULE",           p4_end_module),
    P4_FXco ("EXPORT",               p4_export),
    P4_FXco ("EXPOSE-MODULE",        p4_expose_module),
    P4_FXco ("ALSO-MODULE",          p4_also_module),
};
P4_COUNTWORDS(module, "MODULE - simple module implementation");

/*
 * Local variables:
 * c-file-style: "stroustrup"
 * End:
 */

