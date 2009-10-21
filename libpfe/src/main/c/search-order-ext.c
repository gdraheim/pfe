/**
 * --  The Optional Search Order Word Set
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2008-04-20 04:46:30 $)
 *
 *  @description
 *    	The Search Order Word Set as defined by the Standard.
 *
 *      Note that there a some extensions in the Portable
 *      Forth Environment. Wordlists can be made case-sensitive
 *      always or only at request. Wordlists can be linear
 *      or hashed vocabularies. There are other words to
 *      recursivly search an implicit vocabulary along with another.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) =
"@(#) $Id: search-order-ext.c,v 1.3 2008-04-20 04:46:30 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-limits.h>

#include <pfe/os-string.h>

#include <pfe/option-ext.h>
#include <pfe/def-comp.h>
#include <pfe/_missing.h>

/** DEFINITIONS ( -- )
 * make the current context-vocabulary the definition-vocabulary,
 * that is where new names are declared in. see => ORDER
 */
void FXCode (p4_definitions)
{
    CURRENT = CONTEXT[0];
}

/** GET-CURRENT ( -- voc )
 * return the current definition vocabulary, see => DEFINITIONS
 */
void FXCode (p4_get_current)
{
    FX_PUSH (CURRENT);
}

/** GET-ORDER ( -- vocn ... voc1 n )
 * get the current search order onto the stack, see => SET-ORDER
 */
void FXCode (p4_get_order)
{
    Wordl **p;
    p4cell n = 0;

    for (p = &CONTEXT[PFE_set.wordlists]; --p >= CONTEXT;)
        if (*p)
        { FX_PUSH(*p); n++; }

    FX_PUSH (n);
}

/** SEARCH-WORDLIST ( str-ptr str-len voc -- 0 | xt 1 | xt -1 )
 * almost like => FIND or => (FIND) -- but searches only the
 * specified vocabulary.
 */
void FXCode (p4_search_wordlist)
{
    p4_namebuf_t* nfa;

    nfa = p4_search_wordlist ((p4_char_t *) SP[2], SP[1], (Wordl *) SP[0]);
    if (nfa == NULL)
    {
        SP += 2;
        SP[0] = 0;
    }else{
        SP += 1;
        SP[0] = P4_NFA_xIMMEDIATE(nfa) ? P4_POSITIVE : P4_NEGATIVE;
        SP[1] = (p4cell) p4_name_from (nfa);
    }
}

/** SET-CURRENT ( voc -- )
 * set the definition-vocabulary. see => DEFINITIONS
 */
void FXCode (p4_set_current)
{
    CURRENT = (Wordl *) FX_POP;
}

/** SET-ORDER ( vocn ... voc1 n -- )
 * set the search-order -- probably saved beforehand using
 * => GET-ORDER
 */
void FXCode (p4_set_order)
{
    p4cell i, n = FX_POP;

    if (n == -1)			/* minimum search order */
        n = 0;			/* equals cleared search order */
    if ((p4ucell) n > PFE_set.wordlists)
        p4_throw (P4_ON_SEARCH_OVER);
    for (i = 0; i < n; i++)
        CONTEXT[i] = (Wordl *) FX_POP;
    for (; i < PFE_set.wordlists; i++)
        CONTEXT[i] = NULL;
}

/** WORDLIST ( -- voc )
 * return a new vocabulary-body for private definitions.
 */
void FXCode (p4_wordlist)
{
    FX_PUSH  (p4_make_wordlist (0));
}

/* Search order extension words ============================================ */

/** ALSO ( -- )
 * a => DUP on the search => ORDER - each named vocabulary
 * replaces the topmost => ORDER vocabulary. Using => ALSO
 * will make it fixed to the search-order. (but it is
 * not nailed in trap-conditions as if using => DEFAULT-ORDER )
 order:   vocn ... voc2 voc1 -- vocn ... voc2 voc1 voc1
 */
void FXCode (p4_also)
{
  int i;

  if (CONTEXT[PFE_set.wordlists - 1])
      p4_throw (P4_ON_SEARCH_OVER);
  for (i = PFE_set.wordlists; --i > 0;)
      CONTEXT[i] = CONTEXT[i - 1];
}

/** ORDER ( -- )
 * show the current search-order, followed by
 * the => CURRENT => DEFINITIONS vocabulary
 * and the => ONLY base vocabulary
 */
void FXCode (p4_order)
{
    int i;

    FX (p4_get_order);
    for (i = FX_POP; --i >= 0;)
    {
        Wordl *w = (Wordl *) FX_POP;
        p4_dot_name (w->nfa);
    }
    FX (p4_cr);
    p4_dot_name (CURRENT->nfa);
    p4_outs ("DEFINITIONS           ");
    p4_dot_name (ONLY->nfa);
}

/** PREVIOUS ( -- )
 * the invers of => ALSO , does a => DROP on the search => ORDER
 * of vocabularies.
 order: vocn ... voc2 voc1 -- vocn ... voc2
 example: ALSO PRIVATE-VOC DEFINTIONS (...do some...) PREVIOUS DEFINITIONS
 */
void FXCode (p4_previous)
{
    int i;

    for (i = 0; i < PFE_set.wordlists - 1; i++)
        CONTEXT[i] = CONTEXT[i + 1];
    CONTEXT[i] = NULL;
    for (i = 0; i < PFE_set.wordlists; i++)
        if (CONTEXT[i])
            return;
    p4_throw (P4_ON_SEARCH_UNDER); /* all CONTEXT-entries are null */
}

/** DEFAULT-ORDER ( -- )
 * nail the current search => ORDER so that it will even
 * survive a trap-condition. This default-order can be
 * explicitly loaded with => RESET-ORDER
 */
void FXCode (p4_default_order)
{
    p4_memcpy (p4_DFORDER, p4_CONTEXT, PFE_set.wordlists);
    p4_DFCURRENT = p4_CURRENT;
}

/** RESET-ORDER ( -- )
 * load the => DEFAULT-ORDER into the current search => ORDER
 * - this is implicitly done when a trap is encountered.
 */
void FXCode (p4_reset_order)
{
    p4_memcpy (p4_CONTEXT, p4_DFORDER, PFE_set.wordlists);
    p4_CURRENT = p4_DFCURRENT;
}

/** "ENVIRONMENT WORDLISTS" ( -- value )
 * the maximum number of wordlists in the search order
 */

void FXCode (p4_search_init)
{
#if 0 /*FIXME*/
    p4_create_option_value ((p4_char_t*) "WORDLISTS", 9,
                            PFE_set.wordlists, PFE.set);
#else
    p4_header_comma ((p4_char_t*) "WORDLISTS", 9, CURRENT);
    FX_RUNTIME1(p4_value); FX_COMMA(PFE_set.wordlists);
#endif
}

/** FORTH-WORDLIST ( -- voc )
 * return the voc-address of the base FORTH-vocabulary
 * (quite often the actual name is not => FORTH )
 */

P4_LISTWORDSET (search) [] =
{
    P4_INTO ("[ANS]", 0),
    P4_FXco ("DEFINITIONS",		p4_definitions),
    P4_FXco ("GET-CURRENT",		p4_get_current),
    P4_FXco ("GET-ORDER",		p4_get_order),
    P4_FXco ("SEARCH-WORDLIST",		p4_search_wordlist),
    P4_FXco ("SET-CURRENT",		p4_set_current),
    P4_FXco ("SET-ORDER",		p4_set_order),
    P4_FXco ("WORDLIST",		p4_wordlist),
    P4_FXco ("ALSO",			p4_also),
    P4_FXco ("ORDER",			p4_order),
    P4_FXco ("PREVIOUS",		p4_previous),
    P4_DVaL ("FORTH-WORDLIST",		forth_wl),

    /* hook to activate pfe extensions: */
    P4_INTO ("FORTH", 0),
    P4_FXco ("DEFAULT-ORDER",		p4_default_order),
    P4_FXco ("RESET-ORDER",		p4_reset_order),
    P4_DVaR ("RESET-ORDER-IS",          reset_order),

    P4_INTO ("ENVIRONMENT", 0 ),
    P4_OCON ("SEARCH-ORDER-EXT",	1994 ),
    P4_OCON ("CHAIN-WORDLISTS",		P4_TRUE ),
    P4_XXco ("SEARCH-LOADED",           p4_search_init),
};
P4_COUNTWORDSET (search, "Search-order + extensions");

/*@}*/
