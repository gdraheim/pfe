#ifndef _PFE_SEARCH_ORDER_EXT_H
#define _PFE_SEARCH_ORDER_EXT_H 1155333836
/* generated 2006-0812-0003 ../../pfe/../mk/Make-H.pl ../../pfe/search-order-ext.c */

#include <pfe/pfe-ext.h>

/** 
 * --  The Optional Search Order Word Set
 * 
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2006 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.2 $
 *     (modified $Date: 2006-08-11 22:56:05 $)
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

#ifdef __cplusplus
extern "C" {
#endif




/** DEFINITIONS ( -- )
 * make the current context-vocabulary the definition-vocabulary,
 * that is where new names are declared in. see => ORDER
 */
extern P4_CODE (p4_definitions);

/** GET-CURRENT ( -- voc )
 * return the current definition vocabulary, see => DEFINITIONS
 */
extern P4_CODE (p4_get_current);

/** GET-ORDER ( -- vocn ... voc1 n )
 * get the current search order onto the stack, see => SET-ORDER
 */
extern P4_CODE (p4_get_order);

/** SEARCH-WORDLIST ( str-ptr str-len voc -- 0 | xt 1 | xt -1 )
 * almost like => FIND or => (FIND) -- but searches only the
 * specified vocabulary.
 */
extern P4_CODE (p4_search_wordlist);

/** SET-CURRENT ( voc -- )
 * set the definition-vocabulary. see => DEFINITIONS
 */
extern P4_CODE (p4_set_current);

/** SET-ORDER ( vocn ... voc1 n -- )
 * set the search-order -- probably saved beforehand using
 * => GET-ORDER
 */
extern P4_CODE (p4_set_order);

/** WORDLIST ( -- voc )
 * return a new vocabulary-body for private definitions.
 */
extern P4_CODE (p4_wordlist);

/** ALSO ( -- )
 * a => DUP on the search => ORDER - each named vocabulary
 * replaces the topmost => ORDER vocabulary. Using => ALSO
 * will make it fixed to the search-order. (but it is 
 * not nailed in trap-conditions as if using => DEFAULT-ORDER )
 order:   vocn ... voc2 voc1 -- vocn ... voc2 voc1 voc1
 */
extern P4_CODE (p4_also);

/** ORDER ( -- )
 * show the current search-order, followed by 
 * the => CURRENT => DEFINITIONS vocabulary 
 * and the => ONLY base vocabulary
 */
extern P4_CODE (p4_order);

/** PREVIOUS ( -- )
 * the invers of => ALSO , does a => DROP on the search => ORDER
 * of vocabularies.
 order: vocn ... voc2 voc1 -- vocn ... voc2 
 example: ALSO PRIVATE-VOC DEFINTIONS (...do some...) PREVIOUS DEFINITIONS
 */
extern P4_CODE (p4_previous);

/** DEFAULT-ORDER ( -- )
 * nail the current search => ORDER so that it will even
 * survive a trap-condition. This default-order can be
 * explicitly loaded with => RESET-ORDER
 */
extern P4_CODE (p4_default_order);

/** RESET-ORDER ( -- )
 * load the => DEFAULT-ORDER into the current search => ORDER
 * - this is implicitly done when a trap is encountered.
 */
extern P4_CODE (p4_reset_order);

/** "ENVIRONMENT WORDLISTS" ( -- value )
 * the maximum number of wordlists in the search order
 */
extern P4_CODE (p4_search_init);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
