#ifndef _PFE_CHAINLIST_EXT_H
#define _PFE_CHAINLIST_EXT_H 1155333834
/* generated 2006-0812-0003 ../../pfe/../mk/Make-H.pl ../../pfe/chainlist-ext.c */

#include <pfe/pfe-ext.h>

/** 
 * -- CHAINLIST words - executable WORDLISTs
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2006 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.2 $
 *     (modified $Date: 2006-08-11 22:56:04 $)
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

#ifdef __cplusplus
extern "C" {
#endif




/** NEW-WORDLIST ( "name" -- ) [EXT] [DOES: -- new-wordlist* ]
 *
 * create a new => WORDLIST and a "name" with a runtime of ( -- wordlist* )
 *
 : NEW-WORDLIST WORDLIST VALUE ;
 : NEW-WORDLIST CREATE: WORDLIST ;
 *                         usually used for => DO-ALL-WORDS / => DO-SYNONYM
*/
extern P4_CODE (p4_new_wordlist);

/** .WORDS ( some-wordlist* -- ) [EXT]
 * 
 * print the => WORDLIST interactivly to the user
 *
 : .WORDS ALSO SET-CONTEXT WORDS PREVIOUS ;
 *  
 * => WORDS / => ORDER / => NEW-WORDLIST / => DO-ALL-WORDS
 */
extern P4_CODE (p4_dot_words);

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
extern P4_CODE (p4_do_all_words);

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
extern P4_CODE (p4_redo_all_words);

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
extern P4_CODE (p4_do_all_words_while_loop);

extern P4_CODE (p4_do_all_words_while_execution);

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
extern P4_CODE (p4_do_all_words_while);

/** DO-SYNONYM ( some-wordlist* "do-name" "orig-name" -- ) [EXT]
 * 
 * create a => SYNONYM in the specified wordlist.
 *
 : DO-SYNONYM GET-CURRENT SWAP SET-CURRENT SYNONYM SET-CURRENT ;
 *
 * => DO-ALIAS / => DO-ALL-WORDS / => NEW-WORDLIST / => WORDLIST / => ORDER
 */
extern P4_CODE (p4_do_synonym);

/** ALIAS ( some-xt* "name" -- ) [EXT]
 * create a defer word that is initialized with the given x-token.
 *                                                           => DO-ALIAS
 */
extern P4_CODE (p4_alias);

/** ALIAS-ATEXIT ( some-xt* "name" -- ) [EXT]
 *
 * create a defer word that is initialized with the given x-token.
 *
 : ALIAS-ATEXIT ATEXIT-WORDLIST DO-ALIAS ;
 *                                        => ATEXIT-WORDLIST => DO-ALL-WORDS
 */
extern P4_CODE (p4_alias_atexit);

/** DO-ALIAS ( some-xt* definition-wordlist* "do-name" -- ) [EXT]
 * 
 * create an => ALIAS with the exec-token in the specified wordlist
 *
 : DO-ALIAS GET-CURRENT SWAP SET-CURRENT SWAP ALIAS SET-CURRENT ;
 *                                                           => DO-SYNONYM
 */
extern P4_CODE (p4_do_alias);

/** create a single-threaded wordlist - compare with p4_make_wordlist */
_extern  p4_Wordl* p4_new_wordlist (p4char* nfa) ; /*{*/

_extern  void p4_do_all_words(p4_Wordl* wl) ; /*{*/

_extern  void p4_redo_all_words(p4_Wordl* wl) ; /*{*/

_extern  void p4_do_all_words_while(p4_Wordl* wl, p4xt xt) ; /*{*/

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
