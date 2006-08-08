/** 
 * --  Compatiblity with the FORTH-83 standard.
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: bln_mpt1!33.27 %
 *    (%date_modified: Wed Mar 19 17:31:24 2003 %)
 *
 *  @description
 *     All FORTH-83-Standard words are included here that are not 
 *     in the dpANS already.
 *     Though most of the "uncontrolled reference words" are omitted.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: forth-83-ext.c,v 1.1.1.1 2006-08-08 09:09:41 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-limits.h>

#include <stdlib.h>
#include <errno.h>
#include <pfe/os-string.h>

#include <pfe/def-comp.h>
#include <pfe/facility-ext.h>
#include <pfe/logging.h>
#include <pfe/_missing.h>

/************************************************************************/
/* required word set                                                    */
/************************************************************************/

/** 2+ ( a# -- a#' | a* -- a*' | a -- a' [??] ) [FTH]
 *  add 2 to the value on stack (and leave the result there)
 simulate:
   : 2+ 2 + ;
 */
FCode (p4_two_plus) 
{
    *SP += 2;
}

/** 2- ( a# -- a#' | a* -- a*' | a -- a' [??] ) [FTH]
 *  substract 2 from the value on stack (and leave the result there)
 simulate:
   : 2- 2 - ;
 */
FCode (p4_two_minus)
{
    *SP -= 2;
}

/** COMPILE ( "word" -- )  [FTH]
 * compile the next word. The next word should not be immediate,
 * in which case you would have to use =>'[COMPILE]'. For this
 * reason, you should use the word =>'POSTPONE', which takes care
 * it.
 simulate:
   : COMPILE  R> DUP @ , CELL+ >R ;  ( not immediate !!! )
 */
FCode (p4_compile)		
{
    FX_COMPILE (p4_compile);
    FX (p4_bracket_compile);
}
extern FCode (p4_postpone_execution);
P4COMPILES (p4_compile, p4_postpone_execution,
  P4_SKIPS_CELL, P4_DEFAULT_STYLE);
          
/** ((VOCABULARY)) ( -- ) [HIDDEN]
 * runtime of a => VOCABULARY
 */ 
FCode_RT (p4_vocabulary_RT)
{
    FX_USE_BODY_ADDR;
    CONTEXT[0] = (Wordl *) FX_POP_BODY_ADDR;
}

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
FCode (p4_vocabulary)
{
    FX_HEADER;
    FX_RUNTIME1(p4_vocabulary);
    p4_make_wordlist (LAST);
}
P4RUNTIME1(p4_vocabulary, p4_vocabulary_RT);

/************************************************************************/
/* Controlled reference words                                           */
/************************************************************************/

/** --> ( -- ) [FTH]
 * does increase => BLK and refills the input-buffer
 * from there. Does hence break interpretation of the
 * current BLK and starts with the next. Old-style
 * forth mechanism. You should use => INCLUDE
 */
FCode (p4_next_block)		
{
    FX (p4_Q_loading);
    p4_refill ();
}

/** K ( -- k# ) [FTH]
 * the 3rd loop index just like => I and => J
 */
FCode (p4_k)			
{
    FX_COMPILE (p4_k);
}
FCode (p4_k_execution)			
{
    FX_USE_CODE_ADDR;
    FX_PUSH (FX_RP[6] + FX_RP[7]);
    FX_USE_CODE_EXIT;
}
P4COMPILES (p4_k, p4_k_execution,
	    P4_SKIPS_NOTHING, P4_DEFAULT_STYLE);

/** OCTAL ( -- ) [FTH]
 * sets => BASE to 8. Compare with => HEX and => DECIMAL
 simulate:
   : OCTAL  8 BASE ! ;
 */
FCode (p4_octal)
{
    BASE = 8;
}

/** SP@ ( -- sp-cell* ) [FTH]
 * the address of the top of stack. Does save it onto
 * the stack. You could do 
   : DUP  SP@ @ ;
 */
FCode (p4_s_p_fetch)		
{
    void *p = SP;

    *--SP = (p4cell) p;
}

/************************************************************************/
/* Some uncontrolled reference words                                    */
/************************************************************************/

/** !BITS ( x-bits# x-addr mask# -- ) [FTH]
 * at the cell pointed to by addr, change only the bits that
 * are enabled in mask
 simulate:
   : !BITS  >R 2DUP @ R NOT AND SWAP R> AND OR SWAP ! DROP ;
 */
FCode (p4_store_bits)		
{
    p4ucell mask = SP[0];
    p4ucell *ptr = (p4ucell *) SP[1];
    p4ucell bits = SP[2];
    
    SP += 3;
    *ptr = (*ptr & ~mask) | (bits & mask);
}

/** ** ( a# b# -- power-a# ) [FTH]
 * raise second to top power
 */
FCode (p4_power)
{
    p4cell i = *SP++;
    p4cell n = *SP, m;

    for (m = 1; --i >= 0; m *= n) { }
    *SP = m;
}

/** >< ( a -- a' ) [FTH] [OLD]
 * byte-swap a word
 *
 * depracated: use =>"NTOHS" which does the same as this word when
 * the local byte-order seems to have no match, and be otherwise
 * a no-op. Note that only the two lower bytes of the top-of-cell
 * are swapped.
 */
FCode (p4_byte_swap)
{
    p4char *p = (p4char *) SP
# if PFE_BYTEORDER == 4321
        + (sizeof (p4cell) - 2)
# endif
        , h;

    h = p[1];
    p[1] = p[0];
    p[0] = h;
}

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
FCode (p4_byte_swap_move)
{
    p4char *p = (p4char *) SP[2];
    p4char *q = (p4char *) SP[1];
    p4cell n = SP[0];

    SP += 3;
    for (; n > 0; n -= 2)
    {
        q[1] = p[0];
        q[0] = p[1];
        p += 2;
        q += 2;
    }
}

/** @BITS ( x-addr mask# -- x-value# ) [FTH]
 * see the companion word => !BITS
 simulate:
   : @BITS  SWAP @ AND ;
 */ 
FCode (p4_fetch_bits)
{
    SP[1] = *(p4cell *) SP[1] & SP[0];
    SP++;
}

/************************************************************************/
/* Search order specification and control                               */
/************************************************************************/

/** SEAL ( -- ) [FTH]
 * looks through the search-order and kills the ONLY wordset -
 * hence you can't access the primary vocabularies from there.
 */
FCode (p4_seal)
{
    Wordl **w;

    for (w = CONTEXT; w <= &ONLY; w++)
        if (*w == ONLY)
            w = NULL;
}

/** NOT ( x# - x#' [?] ) [FTH]
 * a => SYNONYM for => INVERT - the word => NOT is not portable as in some
 * systems it is a => SYNONYM for => 0= ... therefore try to avoid it.
 *
 * we declare it as an OBSOLETED-SYNONYM now to create a warning message
 * since code using NOT is inherently not portable. The forth systems
 * seem to be divided about 50%-50% for F83'style bitwise =>"INVERT" or 
 * a logical NOT as =>"0=",  with a slight drift into the direction of 
 *  =>"0=". We  default to bitwise style for earlier FIG/F83 usage. 
 * 
 * Remember that you can override the defaults in your application code
 * e.g. a SYNONYM NOT 0= to have the logical style as the behavior. As
 * for all synonyms the decompilation will show "0=" in that case.
 */

P4_LISTWORDS (forth_83) =
{
    P4_INTO ("FORTH", "[ANS]"),

    /* FORTH-83 required word set */
    P4_FXco ("2+",		p4_two_plus),
    P4_FXco ("2-",		p4_two_minus),
    P4_FXco ("?TERMINAL",	p4_key_question),
    P4_SXco ("COMPILE",		p4_compile),
    P4_xOLD ("NOT",		"INVERT"),
    P4_RTco ("VOCABULARY",	p4_vocabulary),

    /* FORTH-83 controlled reference words */
    P4_IXco ("-->",		p4_next_block),
    P4_FXco ("INTERPRET",	p4_interpret),
    P4_SXco ("K",		p4_k),
    P4_FXco ("OCTAL",		p4_octal),
    P4_FXco ("SP@",		p4_s_p_fetch),

    /* FORTH-83 uncontrolled reference words */
    P4_FXco ("!BITS",		p4_store_bits),
    P4_FXco ("@BITS",		p4_fetch_bits),
    P4_FXco ("_like:NTOHS",	p4_byte_swap),
    P4_xOLD ("><",		"_like:NTOHS"),
    P4_FXco ("_like:NTOHS-MOVE", p4_byte_swap_move),
    P4_xOLD (">MOVE<",		"_like:NTOHS-MOVE"),
    P4_FXco ("**",		p4_power),
    P4_DVaR ("DPL",		dpl),

    /* FORTH-83 Search order specification and control */
    P4_FXco ("SEAL",		p4_seal),
};
P4_COUNTWORDS (forth_83, "Forth'83 compatibility");

/*@}*/

/*
 * Local variables:
 * c-file-style: "stroustrup"
 * End:
 */

