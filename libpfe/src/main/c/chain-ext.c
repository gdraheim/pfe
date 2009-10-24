/**
 * -- CHAIN words - compare with win32forth
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
 *              This wordset implements CHAINs of words as seen in
 *              win32forth - unlike LINK-chains these chains will
 *              be watched by the system and referenced globabally
 *              via the chain-link variable. During FORGET we can
 *              prune these chains and keep them in the state they
 *              deserve to be at that point. In general, CHAINs
 *              are used as defer-chains which hold a series of
 *              of executions tokens in each chain item, and a call
 *              to `do-chain` will execute each one in that chain.
 *
 *  new-chain semicolon-chain : items are usually called resolve-*
 *  new-chain forget-chain    : items are usually called trim-*
 *  new-chain unload-chain    : items are usually called release-*
 *
 *  a chain-item is either a PRIMITIVE or a COLONWORD
 *  we simulate that it is part of a colonword - setting the IP
 *  to the address of the CHAIN-ITEM's body should make it callable.
 *      ITC:
 *           with XT:      the XT
 *           at PRIM:      HERE+1 | CODE* (simulating a prim XT)
 *      CTC:
 *           with XT:      flatten XT being: CODE* | BODY*
 *           at PRIM       CODE*
 *      STC variants:
 *           COLONWORDS are PRIMITIVES too, so just CODE*
 *
 * WARNING: this wordset is not complete - it should hang on to
 *  the forget-routine to be able to prune chains given that their
 *  chain-heads are registered in a system-wide chainlist too.
 *  This has not been implemented.
 *
 * The win32forth model has shown to be not directly usable within
 * the pfe core parts - in win32forth each routine is itself just
 * a forth routine while in pfe there is usually a difference
 * between a colon-routine and a (C-made) primitive-routine so
 * that they can not easily be universally referenced as XTs. It
 * would be a rather complex endavour requiring quite some system
 * runtime resources according to time and speed. Instead, the
 * chain-wordlist system has been modelled at greater extents
 * giving you largely the same functionality on different grounds.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) =
"@(#) $Id$";
#endif

#define _P4_SOURCE 1
#include <pfe/pfe-base.h>
#include <pfe/chain-ext.h>
#include <pfe/debug-ext.h>
#include <pfe/logging.h>

#ifndef _export
#define _export
/* use a linkvariable lvalue */
# define FX_LINK_COMMA(__link) do { \
         register void** __here = (void*) HERE; \
         __here[0] = (void*)(__link);  \
         (void*)(__link) = (void*)__here;  \
         HERE = (p4char*)(++__here); } while (0)
/* use a linkvariable lvalue */
# define FX_LINK_COMMA_AT_END(__link) do { \
         register void** __prev = (void**) &(__link); \
         while (*__prev) __prev = (void**) (*__prev);  \
         *__prev = (void*)(HERE); FX_PCOMMA (0); } while (0)
#endif
/*
  define FX_LINK_COMMA(__link) do {
         register void* __here = (void*) HERE;
         FX_PCOMMA ((void*)(__link));
         (void*)(__link) = __here; } while (0)
*/


/** link, ( some-list* -- ) [EXT]
 : link,        here over @ a, swap ! ;
 */
void FXCode (p4_link_comma)
{
    register void** link = (void**) FX_POP;
    register void*  here = (void*)  p4_HERE;

    FX_PCOMMA (*link);
    *link = here;
}


/** chain-link ( -- chain-link* ) [EXT]
 * a system variable used at => new-chain
 */

typedef struct _Link Link;
struct _Link
{
    Link* next;
    p4xt  exec;
};

typedef struct _Chain Chain;
struct _Chain
{
    Link   link; /* { Link* next; p4xt exec ; } */
    Chain* next; /* actually points to (&Chain.next), due to "link," */
};

/*
 *              Hint: since in PFE, the IS will setup PFA[1], you
 *              can modify a link with it if you created a name for
 *              it just beforehand:
 *              : my-execution .... ;
 *              CREATE: my-exec forget-chain chain-add my-execution
 *              ' my-later-execution IS my-exec
 */

/** new-chain ( "name" -- ) [EXT] [DOES: -- new-chain* ]
 *  create a new chain and register in chain-link
 : new-chain create: 0 , ['] noop , chain-link link, ;
 * layout of a chain:
 * /cell field ->chain.link
 * /cell field ->chain.exec
 * /cell field ->chain.next
 */
void FXCode (p4_new_chain)
{
    FX_RUNTIME_HEADER;
    FX_RUNTIME1(p4_new_chain);
    FX_PCOMMA (0);
    FX_PCOMMA (PFX(p4_noop));
    FX_LINK_COMMA (PFE.chain_link);
}
P4RUNTIME1(p4_new_chain, p4_variable_RT);

/** .chain ( some-chain* -- ) [EXT]
 * show chain - compare with => WORDS
 */
void FXCode (p4_dot_chain)
{
    register const p4char* nfa;
    register Link* link = (void*) FX_POP; /* actually, a Chain-Link */
    if (! link->next) { p4_outs ("Empty"); return; }

    FX (p4_cr);
    FX (p4_start_Q_cr);
    while ((link = link->next))
    {
# if defined PFE_CALL_THREADING
        { /* always primitive */
            nfa = p4_dladdr (link->exec, 0);
            if (! nfa) { p4_outf ("%8p: %08p     (???)", link, link->exec); }
            else { p4_outf ("%8p: %08p     %s", link, link->exec, nfa); }
        }
# else /* ITC: */
        if ((void*)(link->exec) == (void*) (&(link->exec) + 1)) /* primitive */
        {
            nfa = p4_dladdr (link->exec, 0);
            if (! nfa) { p4_outf ("%8p: %08p     (???)", link, link->exec); }
            else { p4_outf ("%8p: %08p     %s", link, link->exec, nfa); }
        }else{
            nfa = p4_addr_to_name ((void*) link->exec);
            if (! nfa) { p4_outf ("%8p: %08p     (???)", link, link->exec); }
            else
            {
                register p4xt xt = p4_name_from (nfa);
                if (xt == link->exec)
                    p4_outf ("%8p: %08p %.*s", link, link->exec,
                             P4_NAMELEN(nfa), P4_NAMEPTR(nfa));
                else
                    p4_outf ("%8p, %08p %.*s [%+i]", link, link->exec,
                             P4_NAMELEN(nfa), P4_NAMEPTR(nfa), xt - link->exec);
            }
        }
# endif
        if (p4_Q_cr ())
            break;
    }
}

/** .chains ( -- ) [EXT]
 * show all chains registered in the system - compare with => VLIST
 */
void FXCode (p4_dot_chains)
{
    register Link* link = (void*) PFE.chain_link; /* actually, at Chain.next */
    while (link)
    {
        p4xt xt = P4_BODY_FROM(&link[-1]);
        register p4char* nfa = p4_to_name(xt);
        p4_outf ("CHAIN %.*s", P4_NAMELEN(nfa), P4_NAMEPTR(nfa));
        p4_simple_execute (xt); FX (p4_dot_chain);
        link = link->next;
    }
    /* we chose EXECUTE so the chain-head can both be a VaR or a DVaR */
}

#if 0
void
p4_do_chain (Link* link)
{
    while (link)
    {
        if (link->exec) { FX_CALL_EXECUTE (link->exec); }
        link = link->next;
    }
}
void
p4_do_chain (p4xt* link)
{
    while (link)
    {
        if (link[1]) { FX_CALL_EXECUTE (link[1]); }
        link = (void*) link[0];
    }
}
#endif

#ifndef _export
#define FX_DO_CHAIN(__link)  do { \
        while ((p4xt*)(__link)) { \
              if (((p4xt*)(__link))[1]) { \
                  FX_CALL_EXECUTE (((p4xt*)(__link))[1]); } \
              (p4xt*)__link = (void*) (((p4xt*)(__link))[0]); \
        } } while(0)
#endif

#ifndef _export
/*  CALL_EXECUTE(p4xcode*) : void */
# if !defined PFE_SBR_THREADING
# define FX_CALL_EXECUTE(X) p4_simple_execute(X)
# else
# define FX_CALL_EXECUTE(__x) (*(p4xcode)(__x))()
# endif
/* CALL_COMMA(link, p4xt) : p4xcode* */
# if !defined PFE_CALL_THREADING
#  define FX_CALL_COMMA(__x) FX_XCOMMA((p4xt)(__x))
# elif !defined PFE_SBR_THREADING
#  define FX_CALL_COMMA(__x) do { \
          FX_XCOMMA(*P4_TO_CODE(__x)); \
          FX_PCOMMA( P4_TO_BODY(__x)); } while (0)
# else
#  define FX_CALL_COMMA(__x) FX_XCOMMA(*P4_TO_CODE(__x))
# endif
/* CALL_COMMA_PRIM(link, p4xcode) : p4xcode* */
# if !defined PFE_CALL_THREADING
#  define FX_CALL_COMMA_CODE(__x) do { \
         { register void** __here = (void*) HERE; FX_PCOMMA((__here+1)); } \
           FX_XCOMMA(__x); } while(0)
# else
#  define FX_CALL_COMMA_CODE(__x) FX_XCOMMA(__x);
# endif

# define FX_CHAIN_COMMA(__xt)        FX_CALL_COMMA(__xt)
# define FX_CHAIN_COMMA_CODE(__code) FX_CALL_COMMA_CODE(__xt)
#endif

/** do-chain ( some-chain* -- ) [EXT]
 * execute chain
 : do-chain being @ ?dup while dup>r cell+ @execute r> repeat ;
 */
void FXCode (p4_do_chain)
{
# if 0
    p4_do_chain (*(void**)(FX_POP)); /* at & Chain.link.next */
# else
    p4xt* link = *(void**) (FX_POP);
    FX_DO_CHAIN(link);
# endif
}

/** chain-add-before ( some-chain* "word-to-add" -- ) [EXT]
 * add chain item, for reverse chain like BYE
 : chain-add-before ' >r here over @ , r> , swap ! ;
 ( chain-add-before link, ' , )
 */
void FXCode (p4_chain_add_before)
{
    register Chain* ch = (void*) FX_POP; /* actually Chain.link.next */
    register p4xt  xt = p4_tick_cfa ();

    FX_LINK_COMMA (ch->link.next);
    FX_CHAIN_COMMA (xt);
}

/** chain-add ( some-chain* "word-to-add" -- ) [EXT]
 * add chain item, for normal setup, at end of do-chain
 : chain-add ' >r begin dup @ while @ repeat here swap ! 0 , r> , ;
 ( chain-add begin dup @ while @ repeat  here swap ! 0, ' , )
 */
void FXCode (p4_chain_add)
{
    register Chain* ch = (void*) FX_POP; /* actually Chain.link.next */
    register p4xt  xt = p4_tick_cfa ();

    FX_LINK_COMMA_AT_END(ch->link.next);
    FX_CHAIN_COMMA (xt);
}

/** semicolon-chain ( -- semicolon-chain* ) [EXT] [OLD]
 * do not use.
 */

P4_LISTWORDSET (chain) [] =
{
    P4_INTO ("EXTENSIONS", 0),
    P4_FXco ("link,",                   p4_link_comma),
    P4_DVaR ("chain-link",              chain_link),
    P4_FXco (".chain",                  p4_dot_chain),
    P4_FXco (".chains",                 p4_dot_chains),
    P4_FXco ("chain-add",               p4_chain_add),
    P4_FXco ("chain-add-before",        p4_chain_add_before),

    P4_FXco ("do-chain",		p4_do_chain),
    P4_RTco ("new-chain",               p4_new_chain),
    P4_FNYM ("xdo-chain",		"do-chain"),
    P4_FNYM ("new-sys-chain",           "new-chain"),
/*  P4_DVaH ("semicolon-chain",         semicolon_chain), */
};
P4_COUNTWORDSET (chain, "chain of executions");

/*@}*/
