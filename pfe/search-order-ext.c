/** 
 * --  The Optional Search Order Word Set
 * 
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE             @(#) %derived_by: guidod %
 *  @version %version: 5.8 %
 *    (%date_modified: Mon Mar 12 10:32:38 2001 %)
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
"@(#) $Id: search-order-ext.c,v 0.30.86.1 2001-03-12 09:32:38 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-xtra.h>

#include <string.h>

#include <pfe/def-comp.h>
#include <pfe/_missing.h>

/** DEFINITIONS ( -- )
 * make the current context-vocabulary the definition-vocabulary,
 * that is where new names are declared in. see => ORDER
 */
FCode (p4_definitions)
{
    CURRENT = CONTEXT[0];
}

/** GET-CURRENT ( -- voc )
 * return the current definition vocabulary, see => DEFINITIONS
 */
FCode (p4_get_current)
{
    FX_PUSH (CURRENT);
}

/** GET-ORDER ( -- vocn ... voc1 n )
 * get the current search order onto the stack, see => SET-ORDER
 */
FCode (p4_get_order)
{
    Wordl **p;
    p4cell n = 0;

    for (p = &CONTEXT[ORDER_LEN]; --p >= CONTEXT;)
        if (*p)
        { FX_PUSH(*p); n++; }

    FX_PUSH (n);
}

/** SEARCH-WORDLIST ( str-ptr str-len voc -- 0 | xt 1 | xt -1 )
 * almost like => FIND or => (FIND) -- but searches only the
 * specified vocabulary.
 */
FCode (p4_search_wordlist)
{
    char *nfa;

    nfa = p4_search_wordlist ((char *) SP[2], SP[1], (Wordl *) SP[0]);
    if (nfa == NULL)
    {
        SP += 2;
        SP[0] = 0;
    }else{
        SP += 1;
        SP[0] = *_FFA(nfa) & P4xIMMEDIATE ? 1 : -1;
        SP[1] = (p4cell) p4_name_from (nfa);
    }
}

/** SET-CURRENT ( voc -- )
 * set the definition-vocabulary. see => DEFINITIONS
 */
FCode (p4_set_current)
{
    CURRENT = (Wordl *) FX_POP;
}

/** SET-ORDER ( vocn ... voc1 n -- )
 * set the search-order -- probably saved beforehand using
 * => GET-ORDER
 */
FCode (p4_set_order)
{
    p4cell i, n = FX_POP;

    if (n == -1)			/* minimum search order */
        n = 0;			/* equals cleared search order */
    if ((p4ucell) n > ORDER_LEN)
        p4_throw (P4_ON_SEARCH_OVER);
    for (i = 0; i < n; i++)
        CONTEXT[i] = (Wordl *) FX_POP;
    for (; i < ORDER_LEN; i++)
        CONTEXT[i] = NULL;
}

/** WORDLIST ( -- voc )
 * return a new vocabulary-body for private definitions.
 */
FCode (p4_wordlist)
{
    FX_PUSH  (p4_make_wordlist ());
}

/* Search order extension words ============================================ */

/** ALSO ( -- )
 * a => DUP on the search => ORDER - each named vocabulary
 * replaces the topmost => ORDER vocabulary. Using => ALSO
 * will make it fixed to the search-order. (but it is 
 * not nailed in trap-conditions as if using => DEFAULT-ORDER )
 order:   vocn ... voc2 voc1 -- vocn ... voc2 voc1 voc1
 */
FCode (p4_also)
{
  int i;

  if (CONTEXT[ORDER_LEN - 1])
      p4_throw (P4_ON_SEARCH_OVER);
  for (i = ORDER_LEN; --i > 0;)
      CONTEXT[i] = CONTEXT[i - 1];
}

/** ORDER ( -- )
 * show the current search-order, followed by 
 * the => CURRENT => DEFINITIONS vocabulary 
 * and the => ONLY base vocabulary
 */
FCode (p4_order)
{
    int i;

    FX (p4_get_order);
    for (i = FX_POP; --i >= 0;)
    {
        Wordl *w = (Wordl *) FX_POP;
        
        p4_dot_name (p4_to_name (BODY_FROM (w)));
    }
    FX (p4_cr);
    p4_dot_name (p4_to_name (BODY_FROM (CURRENT)));
    p4_outs ("DEFINITIONS           ");
    p4_dot_name (p4_to_name (BODY_FROM (ONLY)));
}

/** PREVIOUS ( -- )
 * the invers of => ALSO , does a => DROP on the search => ORDER
 * of vocabularies.
 order: vocn ... voc2 voc1 -- vocn ... voc2 
 example: ALSO PRIVATE-VOC DEFINTIONS (...do some...) PREVIOUS DEFINITIONS
 */
FCode (p4_previous)
{
    int i;

    for (i = 0; i < ORDER_LEN - 1; i++)
        CONTEXT[i] = CONTEXT[i + 1];
    CONTEXT[i] = NULL;
    for (i = 0; i < ORDER_LEN; i++)
        if (CONTEXT[i])
            return;
    p4_throw (P4_ON_SEARCH_UNDER);
}

/** DEFAULT-ORDER ( -- )
 * nail the current search => ORDER so that it will even
 * survive a trap-condition. This default-order can be
 * explicitly loaded with => RESET-ORDER
 */
FCode (p4_default_order)
{
    memcpy (DEFAULT_ORDER, CONTEXT, sizeof (CONTEXT));
}

/** RESET-ORDER ( -- )
 * load the => DEFAULT-ORDER into the current search => ORDER
 * - this is implicitly done when a trap is encountered.
 */
FCode (p4_reset_order)
{
    memcpy (CONTEXT, DEFAULT_ORDER, sizeof (CONTEXT));
}

/** FORTH-WORDLIST ( -- voc )
 * return the voc-address of the base FORTH-vocabulary
 * (quite often the actual name is not => FORTH )
 */
FCode (p4_forth_wordlist)
{
    FX_PUSH (PFE.forth_wl);
}

P4_LISTWORDS (search) =
{
    CO ("DEFINITIONS",	  p4_definitions),
    CO ("FORTH-WORDLIST", p4_forth_wordlist),
    CO ("GET-CURRENT",	  p4_get_current),
    CO ("GET-ORDER",	  p4_get_order),
    CO ("SEARCH-WORDLIST",p4_search_wordlist),
    CO ("SET-CURRENT",	  p4_set_current),
    CO ("SET-ORDER",	  p4_set_order),
    CO ("WORDLIST",	  p4_wordlist),
    CO ("ALSO",		  p4_also),
    CO ("ORDER",	  p4_order),
    CO ("PREVIOUS",	  p4_previous),
    /* hook to activate pfe extensions: */
    CO ("DEFAULT-ORDER",  p4_default_order),
    CO ("RESET-ORDER",	  p4_reset_order),
    P4_INTO ("ENVIRONMENT", 0 ),
    P4_OCON ("SEARCH-ORDER-EXT",	1994 ), 
    P4_OCON ("WORDLISTS",		ORDER_LEN ),
    P4_OCON ("CHAIN-WORDLISTS",		P4_TRUE ),
};
P4_COUNTWORDS (search, "Search-order + extensions");

/*@}*/
