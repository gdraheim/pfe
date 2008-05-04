#ifndef _VOL_8_SRC_CVS_PFE_33_PFE_CHAIN_EXT_H
#define _VOL_8_SRC_CVS_PFE_33_PFE_CHAIN_EXT_H 1209868836
/* generated 2008-0504-0440 /vol/8/src/cvs/pfe-33/pfe/../mk/Make-H.pl /vol/8/src/cvs/pfe-33/pfe/chain-ext.c */

#include <pfe/pfe-ext.h>

/** 
 * -- CHAIN words - compare with win32forth
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.5 $
 *     (modified $Date: 2008-05-04 02:57:30 $)
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

#ifdef __cplusplus
extern "C" {
#endif


/* use a linkvariable lvalue */
# define FX_LINK_COMMA(__link) do { \
         register void** __here = (void*) p4_DP; \
         __here[0] = (void*)(__link);  \
         (void*)(__link) = (void*)__here;  \
         p4_DP = (p4char*)(++__here); } while (0)
/* use a linkvariable lvalue */
# define FX_LINK_COMMA_AT_END(__link) do { \
         register void** __prev = (void**) &(__link); \
         while (*__prev) __prev = (void**) (*__prev);  \
         *__prev = (void*)(p4_DP); FX_PCOMMA (0); } while (0)
#define FX_DO_CHAIN(__link)  do { \
        while ((p4xt*)(__link)) { \
              if (((p4xt*)(__link))[1]) { \
                  FX_CALL_EXECUTE (((p4xt*)(__link))[1]); } \
              (p4xt*)__link = (void*) (((p4xt*)(__link))[0]); \
        } } while(0)
/* ___ CALL_EXECUTE(p4xcode*) : void ____ */
# if !defined PFE_SBR_THREADING
# define FX_CALL_EXECUTE(X) p4_simple_execute(X)
# else
# define FX_CALL_EXECUTE(__x) (*(p4xcode)(__x))()
# endif
/* ___ CALL_COMMA(link, p4xt) : p4xcode* ____ */
# if !defined PFE_CALL_THREADING
#  define FX_CALL_COMMA(__x) FX_XCOMMA((p4xt)(__x))
# elif !defined PFE_SBR_THREADING
#  define FX_CALL_COMMA(__x) do { \
          FX_XCOMMA(*P4_TO_CODE(__x)); \
          FX_PCOMMA( P4_TO_BODY(__x)); } while (0)
# else
#  define FX_CALL_COMMA(__x) FX_XCOMMA(*P4_TO_CODE(__x))
# endif
/* ___ CALL_COMMA_PRIM(link, p4xcode) : p4xcode* ____ */
# if !defined PFE_CALL_THREADING
#  define FX_CALL_COMMA_CODE(__x) do { \
         { register void** __here = (void*) p4_DP; FX_PCOMMA((__here+1)); } \
           FX_XCOMMA(__x); } while(0)
# else
#  define FX_CALL_COMMA_CODE(__x) FX_XCOMMA(__x); 
# endif

# define FX_CHAIN_COMMA(__xt)        FX_CALL_COMMA(__xt)
# define FX_CHAIN_COMMA_CODE(__code) FX_CALL_COMMA_CODE(__xt)


/** link, ( some-list* -- ) [EXT]
 : link,        here over @ a, swap ! ;
 */
extern P4_CODE (p4_link_comma);

/** new-chain ( "name" -- ) [EXT] [DOES: -- new-chain* ]
 *  create a new chain and register in chain-link
 : new-chain create: 0 , ['] noop , chain-link link, ;
 * layout of a chain:
 * /cell field ->chain.link
 * /cell field ->chain.exec
 * /cell field ->chain.next
 */
extern P4_CODE (p4_new_chain);

/** .chain ( some-chain* -- ) [EXT]
 * show chain - compare with => WORDS
 */
extern P4_CODE (p4_dot_chain);

/** .chains ( -- ) [EXT]
 * show all chains registered in the system - compare with => VLIST
 */
extern P4_CODE (p4_dot_chains);

/** do-chain ( some-chain* -- ) [EXT]
 * execute chain
 : do-chain being @ ?dup while dup>r cell+ @execute r> repeat ;
 */
extern P4_CODE (p4_do_chain);

/** chain-add-before ( some-chain* "word-to-add" -- ) [EXT]
 * add chain item, for reverse chain like BYE
 : chain-add-before ' >r here over @ , r> , swap ! ;
 ( chain-add-before link, ' , )
 */
extern P4_CODE (p4_chain_add_before);

/** chain-add ( some-chain* "word-to-add" -- ) [EXT]
 * add chain item, for normal setup, at end of do-chain
 : chain-add ' >r begin dup @ while @ repeat here swap ! 0 , r> , ;
 ( chain-add begin dup @ while @ repeat  here swap ! 0, ' , )
 */
extern P4_CODE (p4_chain_add);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
