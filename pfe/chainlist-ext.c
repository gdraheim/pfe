/** 
 * -- CHAINLIST words - executable WORDLISTs
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 33.17 %
 *    (%date_modified: Wed Mar 19 15:51:30 2003 %)
 *
 *  @description
 *              This wordset implements CHAINLISTs.
 *              Unlike standard search-order WORDLISTs, these are
 *              never hashed lists, instead they are always in order
 *              and they do not take the VOCABULARY runtime even that
 *              they are allowed to live in the search-order itself.
 *              Instead, these singular wordlists may be subject to
 *              a DO-ALL-WORDS that executes the words contained in
 *              this wordlist, not much unlike that win32for DO-CHAIN.
 *              There are words to create these wordlists and list
 *              them to the user.
 *
 *              There is already a wordlist known in pfe for quite a
 *              time being the ATEXIT-WORDLIST which has been not
 *              exported however to the forth-level directly so far
 *              and executions have been stored there using the older
 *              ALIAS-ATEXIT word.
 *
 *              for an example, try the PROMPT-WORDLIST as included
 *              with the outer interpreter - for immediate stack visuals
 *              use:
 *                     PROMPT-WORDLIST DO-SYNONYM .S .S
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: chainlist-ext.c,v 1.1.1.1 2006-08-08 09:08:09 guidod Exp $";
#endif

/*
 * FIXME: rename p4_name_from into p4_name_to_xt to avoid confusion!!
 */
 
#define _P4_SOURCE 1
#include <pfe/pfe-base.h>
#include <pfe/chainlist-ext.h>
#include <pfe/header-ext.h>
#include <pfe/logging.h>

/** create a single-threaded wordlist - compare with p4_make_wordlist */
_export p4_Wordl* p4_new_wordlist (p4char* nfa)
{
    register p4_Wordl* voc = p4_make_wordlist (nfa);
    voc->flag |= WORDL_NOHASH;
    return voc;
}

/** NEW-WORDLIST ( "name" -- ) [EXT] [DOES: -- new-wordlist* ]
 *
 * create a new => WORDLIST and a "name" with a runtime of ( -- wordlist* )
 *
 : NEW-WORDLIST WORDLIST VALUE ;
 : NEW-WORDLIST CREATE: WORDLIST ;
 *                         usually used for => DO-ALL-WORDS / => DO-SYNONYM
*/
FCode (p4_new_wordlist)
{
    FX (p4_create_var); p4_new_wordlist (LAST);
}

/** .WORDS ( some-wordlist* -- ) [EXT]
 * 
 * print the => WORDLIST interactivly to the user
 *
 : .WORDS ALSO SET-CONTEXT WORDS PREVIOUS ;
 *  
 * => WORDS / => ORDER / => NEW-WORDLIST / => DO-ALL-WORDS
 */
FCode (p4_dot_words)
{
    p4_wild_words ((p4_Wordl*)(FX_POP), "*", NULL);
}

_export void p4_do_all_words(p4_Wordl* wl)
{
    register p4char* name;

    if (! wl) return;
    if (! (wl->flag & WORDL_NOHASH))
    {
        P4_fail ("trying to DO-ALL-WORDS of a hashed WORDLIST");
        return;
    }else
    name = wl->thread[0];
    
    while (name)
    {
        /* HINT: as for =>"SYNONYM"s,
         * p4_name_from(name) != P4_LINK_FROM(p4_name_to_link(name))
         */
        PFE.execute (p4_name_from(name));
        name = *p4_name_to_link(name);
    }
}

/* ------------------------------------------------------------------- */

/** DO-ALL-WORDS ( some-wordlist* -- ) [EXT]
 * 
 * => EXECUTE each entry in the wordlist in the reverse order defined
 *
 : DO-ALL-WORDS
      0 FIRST-NAME
      BEGIN ?DUP WHILE 
         DUP NAME> EXECUTE
         NAME-NEXT
      REPEAT
 ;
 * to run the => NEW-WORDLIST in original order, use => REDO-ALL-WORDS
 */
FCode (p4_do_all_words)
{
    p4_do_all_words ((p4_Wordl*)(FX_POP));
}

_export void p4_redo_all_words(p4_Wordl* wl)
{
    register p4char* name;

    if (! wl) return;
    if (! (wl->flag & WORDL_NOHASH))
    {
        P4_fail ("trying to REDO-ALL-WORDS of a hashed WORDLIST");
        return;
    }
    name = wl->thread[0];

    FX_PUSH (0);
    
    while (name)
    {
        /* HINT: as for =>"SYNONYM"s,
         * p4_name_from(name) != P4_LINK_FROM(p4_name_to_link(name))
         */
        FX_PUSH (p4_name_from(name));
        name = *p4_name_to_link (name);
    }

    while (*SP)
    {
        PFE.execute ((p4xt)(FX_POP));
    }

    FX_DROP;
}

/** REDO-ALL-WORDS ( some-wordlist* -- ) [EXT]
 * 
 * => EXECUTE each entry in the wordlist in the original order defined
 *
 : REDO-ALL-WORDS
      0 FIRST-NAME
      0 SWAP ( under )
      BEGIN ?DUP WHILE 
         DUP NAME> SWAP ( under )
         NAME-NEXT
      REPEAT
      BEGIN ?DUP WHILE
         EXECUTE
      REPEAT
 ;
 * to run the => NEW-WORDLIST in last-run-first order, use => DO-ALL-WORDS
 */
FCode (p4_redo_all_words)
{
    p4_redo_all_words ((p4_Wordl*)(FX_POP));
}

_export void p4_do_all_words_while(p4_Wordl* wl, p4xt xt)
{
    register p4char* name;

    if (! wl) return;
    if (! (wl->flag & WORDL_NOHASH))
    {
        P4_fail ("trying to DO-ALL-WORDS of a hashed WORDLIST");
        return;
    }else
    name = wl->thread[0];
    
    while (name)
    {
        PFE.execute (xt);
        if (!(FX_POP)) break;

        /* HINT: as for =>"SYNONYM"s,
         * p4_name_from(name) != P4_LINK_FROM(p4_name_to_link(name))
         */
        PFE.execute (p4_name_from(name));
        name = *p4_name_to_link(name);
    }
}

/** DO-ALL-WORDS-WHILE-LOOP ( some-wordlist* test-xt* -- ) [EXT]
 * 
 * => EXECUTE each entry in the wordlist in the reverse order defined
 *    but only as long as after EXECUTE of "word" a TRUE flag is left
 *    on the stack. The wordlist execution is cut when a FALSE flag is seen.
 *    (the current wordlist entry is _not_ on the stack!)
 *
 : DO-ALL-WORDS-WHILE-LOOP >R
      0 FIRST-NAME
      BEGIN ?DUP WHILE 
         R@ EXECUTE 0= IF R>DROP DROP EXIT THEN
         DUP NAME> EXECUTE
         NAME-NEXT
      REPEAT R>DROP
 ;
 * compare with => DO-ALL-WORDS-WHILE
 */
FCode (p4_do_all_words_while_loop)
{
    register p4xt xt = (p4xt)(FX_POP);
    if (! xt) return;
    p4_do_all_words_while ((p4_Wordl*)(FX_POP), xt);
}

FCode (p4_do_all_words_while_execution)
{
    FX_USE_CODE_ADDR;
    p4_do_all_words_while ((p4_Wordl*)(FX_POP), (p4xt)(*IP++));
    FX_USE_CODE_EXIT;
}

/** DO-ALL-WORDS-WHILE ( some-wordlist* "word" -- ) [EXT]
 * 
 * => EXECUTE each entry in the wordlist in the reverse order defined
 *    but only as long as after EXECUTE of "word" a TRUE flag is left
 *    on the stack. The wordlist execution is cut when a FALSE flag is seen.
 *    (the current wordlist entry is _not_ on the stack!)
 *
 : DO-ALL-WORDS-WHILE ' 
      STATE @ IF LITERAL, COMPILE DO-ALL-WORDS-WHILE-LOOP EXIT THEN
      >R 0 FIRST-NAME
      BEGIN ?DUP WHILE 
         R@ EXECUTE 0= IF R>DROP DROP EXIT THEN
         DUP NAME> EXECUTE
         NAME-NEXT
      REPEAT R>DROP
 ;
 * to run the => NEW-WORDLIST in original order, use => REDO-ALL-WORDS
 */
FCode (p4_do_all_words_while)
{
    p4xt xt = p4_tick_cfa ();
    if (! STATE)
    {
        p4_do_all_words_while ((p4_Wordl*)(FX_POP), xt);
    }else{
        /* 
         *  FX_LITERAL_COMMA(xt);
         *  FX_COMPILE(p4_do_all_words_while);
         */
        FX_COMPILE(p4_do_all_words_while);
        FX_COMMA(xt);
    }
}
P4COMPILES(p4_do_all_words_while, p4_do_all_words_while_execution,
           P4_SKIPS_TO_TOKEN, P4_LOCALS_STYLE);

/* ------------------------------------------------------------------- */


/** DO-SYNONYM ( some-wordlist* "do-name" "orig-name" -- ) [EXT]
 * 
 * create a => SYNONYM in the specified wordlist.
 *
 : DO-SYNONYM GET-CURRENT SWAP SET-CURRENT SYNONYM SET-CURRENT ;
 *
 * => DO-ALIAS / => DO-ALL-WORDS / => NEW-WORDLIST / => WORDLIST / => ORDER
 */
FCode (p4_do_synonym)
{
    p4_Wordl* old = CURRENT; CURRENT = (p4_Wordl*)(FX_POP);
    FX (p4_synonym);
    CURRENT = old;

    /* fixme: swap p4_synonym and p4_do_synonym by making
     * p4_synonym FX_PUSH(CURRENT) and call p4_do_synonym
     */
}

extern FCode(p4_defer); /* -> DOER */

/** ALIAS ( some-xt* "name" -- ) [EXT]
 * create a defer word that is initialized with the given x-token.
 *                                                           => DO-ALIAS
 */
FCode (p4_alias)
{
    FX_HEADER;
    FX_RUNTIME_BODY;
    FX_RUNTIME1 (p4_defer); /* fixme? p4_alias_RT */
    FX_XCOMMA (0); /* DOES-CODE field (later may be used for chain link)*/
    FX_XCOMMA (FX_POP); /* set DOES-BODY here */
}

/** ALIAS-ATEXIT ( some-xt* "name" -- ) [EXT]
 *
 * create a defer word that is initialized with the given x-token.
 *
 : ALIAS-ATEXIT ATEXIT-WORDLIST DO-ALIAS ;
 *                                        => ATEXIT-WORDLIST => DO-ALL-WORDS
 */
FCode (p4_alias_atexit)
{
    FX_HEADER_(PFE.atexit_wl); /* <-- the difference with => ALIAS */
    FX_RUNTIME_BODY;
    FX_RUNTIME1 (p4_defer); /* fixme? p4_alias_atexit_RT */
    FX_XCOMMA (0); 
    FX_XCOMMA (FX_POP);

#ifdef PFE_WITH_FFA
    P4_NFA_FLAGS(LAST) |= P4xONxDESTROY; /* fixme: p4_alias_atexit_RT !! */
#endif
}

/** DO-ALIAS ( some-xt* definition-wordlist* "do-name" -- ) [EXT]
 * 
 * create an => ALIAS with the exec-token in the specified wordlist
 *
 : DO-ALIAS GET-CURRENT SWAP SET-CURRENT SWAP ALIAS SET-CURRENT ;
 *                                                           => DO-SYNONYM
 */
FCode (p4_do_alias)
{
    FX_HEADER_((p4_Wordl*)(FX_POP)); /* <-- the difference with => ALIAS */
    FX_RUNTIME_BODY;
    FX_RUNTIME1 (p4_defer); 
    FX_XCOMMA (0); 
    FX_XCOMMA (FX_POP);
}

/** ATEXIT-WORDLIST ( -- atexit-do-wordlist* ) [EXT]
 *
 * => BYE will run this wordlist, last added being run first
 : BYE ... ATEXIT-WORDLIST DO-ALL-WORDS ... ;
 : FORGET ... ATEXIT-WORDLIST DO-ALL-WORDS-WHILE BIGGER-THAN-HERE ... ;
 WORDLIST VALUE ATEXIT-WORDLIST
 *                                        => DO-ALL-WORDS / => LOADED
 * note: parts of these will be run by 
 * => FORGET when the HERE mark becomes
 * lower than the definition you had
 * added after that to this chainlist.
 * (that is to run destroyer aliases).    => ALIAS-ATEXIT / => FORGET
 */

/** PROMPT-WORDLIST ( -- prompt-do-wordlist* ) [EXT]
 *
 * => QUIT inits will run this wordlist, last added being run first
 : QUIT ... PROMPT-WORDLIST DO-ALL-WORDS ... ;
 WORDLIST VALUE PROMPT-WORDLIST
 *                                        => DO-ALL-WORDS / => ABORT-WORDLIST
 */

/** ABORT-WORDLIST ( -- abort-redo-wordlist* ) [EXT]
 *
 * => ABORT inits will run this wordlist, first added being run first
 : ABORT ... ABORT-WORDLIST REDO-ALL-WORDS ... ;
 WORDLIST VALUE ABORT-WORDLIST
 *                                       => REDO-ALL-WORDS / => PROMPT-WORDLIST
 */

P4_LISTWORDS (chainlist) =
{
    P4_INTO ("EXTENSIONS", 0),
    P4_FXco ("NEW-WORDLIST",              p4_new_wordlist),
    P4_FXco (".WORDS",                    p4_dot_words),
    P4_FXco ("REDO-ALL-WORDS",            p4_redo_all_words),
    P4_FXco ("DO-ALL-WORDS",              p4_do_all_words),
    P4_FXco ("DO-ALL-WORDS-WHILE-LOOP",   p4_do_all_words_while_loop),
    P4_SXco ("DO-ALL-WORDS-WHILE",        p4_do_all_words_while),
    P4_FXco ("DO-SYNONYM",                p4_do_synonym),
    P4_FXco ("DO-ALIAS",                  p4_do_alias),
    P4_FXco ("ALIAS-ATEXIT",              p4_alias_atexit),
    P4_FXco ("ALIAS",                     p4_alias),
    P4_DVaL ("ATEXIT-WORDLIST",           atexit_wl),
    P4_DVaL ("PROMPT-WORDLIST",           prompt_wl),
    P4_DVaL ("ABORT-WORDLIST",            abort_wl),
};
P4_COUNTWORDS (chainlist, "chainlists - executable wordlists");

/*@}*/

/* 
 * Local variables:
 * c-file-style: "stroustrup"
 * End:
 */
