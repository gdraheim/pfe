#ifndef _PFE_FORTH_83_EXT_H
#define _PFE_FORTH_83_EXT_H 990144518
/* generated 2001-0518-0208 ../../pfe/../mk/Make-H.pl ../../pfe/forth-83-ext.c */

#include <pfe/incl-ext.h>

/** 
 * --  Compatiblity with the FORTH-83 standard.
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 5.12 %
 *    (%date_modified: Mon Mar 12 10:32:21 2001 %)
 *
 *  @description
 *     All FORTH-83-Standard words are included here that are not 
 *     in the dpANS already.
 *     Though most of the "uncontrolled reference words" are omitted.
 */

#ifdef __cplusplus
extern "C" {
#endif




/** 2+ ( i -- i ) 
 *  add 2 to the value on stack (and leave the result there)
 simulate:
   : 2+ 2 + ;
 */
extern P4_CODE (p4_two_plus);

/** 2- ( i -- i )
 *  substract 2 from the value on stack (and leave the result there)
 simulate:
   : 2- 2 - ;
 */
extern P4_CODE (p4_two_minus);

/** COMPILE ( 'word' -- ) 
 * compile the next word. The next word should not be immediate,
 * in which case you would have to use =>'[COMPILE]'. For this
 * reason, you should use the word =>'POSTPONE', which takes care
 * it.
 simulate:
   : COMPILE  R> DUP @ , CELL+ >R ;  ( not immediate !!! )
 */
extern P4_CODE (p4_compile);

/** ((VOCABULARY)) ( -- )
 * runtime of a => VOCABULARY
 */
extern P4_CODE (p4_vocabulary_RT);

/** VOCABULARY ( 'name' -- )
 * create a vocabulary of that name. If the named vocabulary
 * is called later, it will run => ((VOCABULARY)) , thereby
 * putting it into the current search order.
 * Special pfe-extensions are accessible via 
 * => CASE-SENSITIVE-VOC and => SEARCH-ALSO-VOC
 simulate:
   : VOCABULARY  CREATE ALLOT-WORDLIST
        DOES> ( the ((VOCABULARY)) runtime )
          CONTEXT ! 
   ; IMMEDIATE
 */
extern P4_CODE (p4_vocabulary);

/** CASE-SENSITIVE-VOC ( -- ) 
 * accesses => CONTEXT which is generally the last named => VOCABULARY .
 * sets a flag in the vocabulary-definition so that words are matched
 * case-sensitive. 
 example: 
    VOCABULARY MY-VOC  MY-VOC CASE-SENSITIVE-VOC
 */
extern P4_CODE (p4_case_sensitive_voc);

/** SEARCH-ALSO-VOC ( -- )
 * binds => CONTEXT with =>'CURRENT'. If the => CURRENT => VOCABULARY is in
 * the search-order (later), then the => CONTEXT vocabulary will 
 * be searched also. If the result of this word could lead into 
 * a recursive lookup with => FIND it will throw <c>CURRENT_DELETED</c>
 * and leave the => CURRENT => VOCABULARY unaltered.
 example:
 * MY-VOC DEFINITIONS  MY-VOC-PRIVATE SEARCH-ALSO-VOC
 */
extern P4_CODE (p4_search_also_voc);

/** --> ( -- ) no-return
 * does increase => BLK and refills the input-buffer
 * from there. Does hence break interpretation of the
 * current BLK and starts with the next. Old-style
 * forth mechanism. You should use => INCLUDE
 */
extern P4_CODE (p4_next_block);

/** K ( -- counter-val )
 * the 3rd loop index just like => I and => J
 */
extern P4_CODE (p4_k);

/** OCTAL ( -- )
 * sets => BASE to 8. Compare with => HEX and => DECIMAL
 simulate:
   : OCTAL  8 BASE ! ;
 */
extern P4_CODE (p4_octal);

/** SP@ ( -- )
 * the address of the top of stack. Does save it onto
 * the stack. You could do 
   : DUP  SP@ @ ;
 */
extern P4_CODE (p4_s_p_fetch);

/** !BITS ( bits addr mask -- )
 * at the cell pointed to by addr, change only the bits that
 * are enabled in mask
 simulate:
   : !BITS  >R 2DUP @ R NOT AND SWAP R> AND OR SWAP ! DROP ;
 */
extern P4_CODE (p4_store_bits);

/** ** ( a b -- r )
 * raise second to top power
 */
extern P4_CODE (p4_power);

/** >< ( a -- a' )
 * byte-swap a word
 */
extern P4_CODE (p4_byte_swap);

/** >MOVE< ( from-addr to-addr count -- )
 * see => MOVE , does byte-swap for each word underway
 */
extern P4_CODE (p4_byte_swap_move);

/** @BITS ( addr mask -- value )
 * see the companion word => !BITS
 simulate:
   : @BITS  SWAP @ AND ;
 */
extern P4_CODE (p4_fetch_bits);

/** SEAL ( -- )
 * looks through the search-order and kills the ONLY wordset -
 * hence you can't access the primary vocabularies from there.
 */
extern P4_CODE (p4_seal);

/** >NAME ( cfa -- nfa )
 * converts a pointer to the code-field (CFA) to point
 * then to the corresponding name-field (NFA)
 implementation-specific simulation:
   : >NAME  >LINK L>NAME ;
 */
extern P4_CODE (p4_to_name);

/** >LINK ( cfa -- lfa )
 * converts a pointer to the code-field (CFA) to point
 * then to the corresponding link-field (LFA)
 */
extern P4_CODE (p4_to_link);

/** BODY> ( pfa -- cfa )
 * converts a pointer to the parameter-field (PFA) to point
 * then to the corresponding code-field (CFA)
 implementation-specific simulation:
   : BODY> CELL - ;
 */
extern P4_CODE (p4_body_from);

/** NAME> ( nfa -- cfa )
 * converts a pointer to the name-field (NFA) to point
 * then to the corresponding code-field (CFA)
 implementation-specific simulation:
   : NAME>  N>LINK LINK> ;
 */
extern P4_CODE (p4_name_from);

/** LINK> ( lfa -- cfa )
 * converts a pointer to the link-field (LFA) to point
 * then to the corresponding code-field (CFA)
 */
extern P4_CODE (p4_link_from);

/** L>NAME ( lfa -- nfa )
 * converts a pointer to the link-field (LFA) to point
 * then to the corresponding name-field (CFA)
 */
extern P4_CODE (p4_l_to_name);

/** N>LINK ( nfa -- lfa )
 * converts a pointer to the name-field (NFA) to point
 * then to the corresponding link-field (LFA)
 implementation-specific configure-dependent simulation:
   : N>LINK  C@ + ;
 */
extern P4_CODE (p4_n_to_link);

/** >FFA ( nfa -- ffa )
 * converts a pointer to the name-field (NFA) to point
 * then to the corresponding flag-field (FFA) - in traditinal
 * Forth this is the same address. pfe _can_ do different.
 implementation-specific configure-dependent simulation:
   : FFA  1- ;
 */
extern P4_CODE (p4_to_ffa);

/** FFA> ( ffa -- nfa )
 * converts a pointer to the flag-field (FFA) to point
 * then to the corresponding name-field (NFA) - in traditinal
 * Forth this is the same address. pfe _can_ do different.
 implementation-specific configure-dependent simulation:
   : FFA  1+ ;
 */
extern P4_CODE (p4_ffa_from);

/** NOT ( x - ~x )
 * an alias for => INVERT - the => NOT is not portable as in some
 * systems it is an alias for => 0= ... therefore try to avoid it.
 */
extern P4_CODE (p4_not);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
