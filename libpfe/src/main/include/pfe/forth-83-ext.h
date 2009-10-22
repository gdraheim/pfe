#ifndef PFE_FORTH_83_EXT_H
#define PFE_FORTH_83_EXT_H 1256214393
/* generated 2009-1022-1426 make-header.py ../../c/forth-83-ext.c */

#include <pfe/pfe-ext.h>

/**
 * --  Compatiblity with the FORTH-83 standard.
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.5 $
 *     (modified $Date: 2008-05-10 16:34:51 $)
 *
 *  @description
 *     All FORTH-83-Standard words are included here that are not
 *     in the dpANS already.
 *     Though most of the "uncontrolled reference words" are omitted.
 */

#ifdef __cplusplus
extern "C" {
#endif




/** 2+ ( a# -- a#' | a* -- a*' | a -- a' [??] ) [FTH]
 *  add 2 to the value on stack (and leave the result there)
 simulate:
   : 2+ 2 + ;
 */
extern void FXCode (p4_two_plus);

/** 2- ( a# -- a#' | a* -- a*' | a -- a' [??] ) [FTH]
 *  substract 2 from the value on stack (and leave the result there)
 simulate:
   : 2- 2 - ;
 */
extern void FXCode (p4_two_minus);

/** COMPILE ( "word" -- )  [FTH]
 * compile the next word. The next word should not be immediate,
 * in which case you would have to use =>'[COMPILE]'. For this
 * reason, you should use the word =>'POSTPONE', which takes care
 * it.
 simulate:
   : COMPILE  R> DUP @ , CELL+ >R ;  ( not immediate !!! )
 */
extern void FXCode (p4_compile);

/** ((VOCABULARY)) ( -- ) [HIDDEN]
 * runtime of a => VOCABULARY
 */
extern void FXCode_RT (p4_vocabulary_RT);

/** VOCABULARY ( "name" -- ) [FTH]
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
extern void FXCode (p4_vocabulary);

/** --> ( -- ) [FTH]
 * does increase => BLK and refills the input-buffer
 * from there. Does hence break interpretation of the
 * current BLK and starts with the next. Old-style
 * forth mechanism. You should use => INCLUDE
 : --> ?LOADING REFILL ;
 */
extern void FXCode (p4_next_block);

/** K ( -- k# ) [FTH]
 * the 3rd loop index just like => I and => J
 */
extern void FXCode (p4_k);

extern void FXCode (p4_k_execution);

/** OCTAL ( -- ) [FTH]
 * sets => BASE to 8. Compare with => HEX and => DECIMAL
 simulate:
   : OCTAL  8 BASE ! ;
 */
extern void FXCode (p4_octal);

/** SP@ ( -- sp-cell* ) [FTH]
 * the address of the top of stack. Does save it onto
 * the stack. You could do
   : DUP  SP@ @ ;
 */
extern void FXCode (p4_s_p_fetch);

/** !BITS ( x-bits# x-addr mask# -- ) [FTH]
 * at the cell pointed to by addr, change only the bits that
 * are enabled in mask
 simulate:
   : !BITS  >R 2DUP @ R NOT AND SWAP R> AND OR SWAP ! DROP ;
 */
extern void FXCode (p4_store_bits);

/** ** ( a# b# -- power-a# ) [FTH]
 * raise second to top power
 */
extern void FXCode (p4_power);

/** >< ( a -- a' ) [FTH] [OLD]
 * byte-swap a word
 *
 * depracated: use =>"NTOHS" which does the same as this word when
 * the local byte-order seems to have no match, and be otherwise
 * a no-op. Note that only the two lower bytes of the top-of-cell
 * are swapped.
 */
extern void FXCode (p4_byte_swap);

/** >MOVE< ( from-addr* to-addr* count# -- ) [FTH] [OLD]
 * see => MOVE , does byte-swap for each word underway.
 *
 * depracated: this word has not been very useful lately. It does
 * still stem from times of 16bit forth systems that wanted to
 * interchange data blocks. It is better to use functionality
 * based on => NTOHS or => NTOHL. Note that this word =>">MOVE<"
 * does swap each 2byte. It is not useful for byte-swapping
 * => WCHAR strings as the count is given in bytes, not wchar items.
 */
extern void FXCode (p4_byte_swap_move);

/** @BITS ( x-addr mask# -- x-value# ) [FTH]
 * see the companion word => !BITS
 simulate:
   : @BITS  SWAP @ AND ;
 */
extern void FXCode (p4_fetch_bits);

/** SEAL ( -- ) [FTH]
 * looks through the search-order and kills the ONLY wordset -
 * hence you can't access the primary vocabularies from there.
 */
extern void FXCode (p4_seal);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
