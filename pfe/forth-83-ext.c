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
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: forth-83-ext.c,v 0.31 2001-05-12 18:15:46 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-xtra.h>

#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <pfe/def-comp.h>
#include <pfe/facility-ext.h>
#include <pfe/logging.h>
#include <pfe/_missing.h>

/************************************************************************/
/* required word set                                                    */
/************************************************************************/

/** 2+ ( i -- i ) 
 *  add 2 to the value on stack (and leave the result there)
 simulate:
   : 2+ 2 + ;
 */
FCode (p4_two_plus) 
{
    *SP += 2;
}

/** 2- ( i -- i )
 *  substract 2 from the value on stack (and leave the result there)
 simulate:
   : 2- 2 - ;
 */
FCode (p4_two_minus)
{
    *SP -= 2;
}

/** COMPILE ( 'word' -- ) 
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
          
/** ((VOCABULARY)) ( -- )
 * runtime of a => VOCABULARY
 */ 
FCode (p4_vocabulary_RT)
{
    CONTEXT[0] = (Wordl *) WP_PFA;
}

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
FCode (p4_vocabulary)
{
    p4_header (PFX(p4_vocabulary_RT), 0);
    p4_make_wordlist ();
}

/** CASE-SENSITIVE-VOC ( -- ) 
 * accesses => CONTEXT which is generally the last named => VOCABULARY .
 * sets a flag in the vocabulary-definition so that words are matched
 * case-sensitive. 
 example: 
    VOCABULARY MY-VOC  MY-VOC CASE-SENSITIVE-VOC
 */
FCode (p4_case_sensitive_voc)
{
    if (!CONTEXT[0]) return;
    CONTEXT[0]->flag &=~ WORDL_NOCASE ; 
}

/** SEARCH-ALSO-VOC ( -- )
 * binds => CONTEXT with =>'CURRENT'. If the => CURRENT => VOCABULARY is in
 * the search-order (later), then the => CONTEXT vocabulary will 
 * be searched also. If the result of this word could lead into 
 * a recursive lookup with => FIND it will throw <c>CURRENT_DELETED</c>
 * and leave the => CURRENT => VOCABULARY unaltered.
 example:
 * MY-VOC DEFINITIONS  MY-VOC-PRIVATE SEARCH-ALSO-VOC
 */
FCode (p4_search_also_voc)
{
    if (!CONTEXT[0] || !CURRENT) return;
    { /* sanity check -> CURRENT may not be part of CONTEXT also-chain */
        register Wordl* wl; 
        for (wl = CONTEXT[0]; wl; wl=wl->also) 
            if (wl == CURRENT) p4_throw (P4_ON_CURRENT_DELETED);  
    }
    CURRENT->also = CONTEXT[0] ; 
}

/************************************************************************/
/* Controlled reference words                                           */
/************************************************************************/

/** --> ( -- ) no-return
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

/** K ( -- counter-val )
 * the 3rd loop index just like => I and => J
 */
FCode (p4_k)			
{
    FX_PUSH (FX_RP[6] + FX_RP[7]);
}

/** OCTAL ( -- )
 * sets => BASE to 8. Compare with => HEX and => DECIMAL
 simulate:
   : OCTAL  8 BASE ! ;
 */
FCode (p4_octal)
{
    BASE = 8;
}

/** SP@ ( -- )
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

/** !BITS ( bits addr mask -- )
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

/** ** ( a b -- r )
 * raise second to top power
 */
FCode (p4_power)
{
    p4cell i = *SP++;
    p4cell n = *SP, m;

    for (m = 1; --i >= 0; m *= n) { }
    *SP = m;
}

/** >< ( a -- a' )
 * byte-swap a word
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

/** >MOVE< ( from-addr to-addr count -- )
 * see => MOVE , does byte-swap for each word underway
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

/** @BITS ( addr mask -- value )
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

/** SEAL ( -- )
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

/************************************************************************/
/* Definition field address conversion operators                        */
/************************************************************************/

/** >NAME ( cfa -- nfa )
 * converts a pointer to the code-field (CFA) to point
 * then to the corresponding name-field (NFA)
 implementation-specific simulation:
   : >NAME  >LINK L>NAME ;
 */
FCode (p4_to_name)
{
    *SP = (p4cell) p4_to_name ((p4xt) *SP);
}

/** >LINK ( cfa -- lfa )
 * converts a pointer to the code-field (CFA) to point
 * then to the corresponding link-field (LFA)
 */
FCode (p4_to_link) 
{
    *SP = (p4cell) p4_to_link ((p4xt) *SP);
}

/** BODY> ( pfa -- cfa )
 * converts a pointer to the parameter-field (PFA) to point
 * then to the corresponding code-field (CFA)
 implementation-specific simulation:
   : BODY> CELL - ;
 */
FCode (p4_body_from)
{
    *SP = (p4cell) BODY_FROM (*SP);
}

/** NAME> ( nfa -- cfa )
 * converts a pointer to the name-field (NFA) to point
 * then to the corresponding code-field (CFA)
 implementation-specific simulation:
   : NAME>  N>LINK LINK> ;
 */
FCode (p4_name_from)
{
    *SP = (p4cell) p4_name_from ((char *) *SP);
}

/** LINK> ( lfa -- cfa )
 * converts a pointer to the link-field (LFA) to point
 * then to the corresponding code-field (CFA)
 */
FCode (p4_link_from)
{
    *SP = (p4cell) p4_link_from ((char **) *SP);
}

/** L>NAME ( lfa -- nfa )
 * converts a pointer to the link-field (LFA) to point
 * then to the corresponding name-field (CFA)
 */
FCode (p4_l_to_name)
{
    *SP = (p4cell) p4_link_to_name ((char **) *SP);
}

/** N>LINK ( nfa -- lfa )
 * converts a pointer to the name-field (NFA) to point
 * then to the corresponding link-field (LFA)
 implementation-specific configure-dependent simulation:
   : N>LINK  C@ + ;
 */
FCode (p4_n_to_link)
{
    *SP = (p4cell) p4_name_to_link ((char *) *SP);
}

/** >FFA ( nfa -- ffa )
 * converts a pointer to the name-field (NFA) to point
 * then to the corresponding flag-field (FFA) - in traditinal
 * Forth this is the same address. pfe _can_ do different.
 implementation-specific configure-dependent simulation:
   : FFA  1- ;
 */
FCode (p4_to_ffa)
{
#  ifdef PFE_WITH_FFA
    *SP = (p4cell) (*(char**)SP)-1;
#  endif
}

/** FFA> ( ffa -- nfa )
 * converts a pointer to the flag-field (FFA) to point
 * then to the corresponding name-field (NFA) - in traditinal
 * Forth this is the same address. pfe _can_ do different.
 implementation-specific configure-dependent simulation:
   : FFA  1+ ;
 */
FCode (p4_ffa_from)
{
#  ifdef PFE_WITH_FFA
    *SP = (p4cell) (*(char**)SP)+1;
#  endif
}

/** NOT ( x - ~x )
 * an alias for => INVERT - the => NOT is not portable as in some
 * systems it is an alias for => 0= ... therefore try to avoid it.
 */
FCode (p4_not)
{
    FX_COMPILE(p4_not);
    P4_warn ("forth' NOT is not portable, use INVERT or 0= ");
}
P4COMPILES(p4_not, p4_invert, P4_SKIPS_NOTHING, P4_DEFAULT_STYLE);

P4_LISTWORDS (forth_83) =
{
    /* FORTH-83 required word set */
    CO ("2+",		p4_two_plus),
    CO ("2-",		p4_two_minus),
    CO ("?TERMINAL",	p4_key_question),
    CS ("COMPILE",	p4_compile),
    CS ("NOT",		p4_not), 
    CO ("VOCABULARY",	p4_vocabulary),
    /* FORTH-83 controlled reference words */
    CI ("-->",		p4_next_block),
    CO ("INTERPRET",	p4_interpret),
    CO ("K",		p4_k),
    CO ("OCTAL",	p4_octal),
    CO ("SP@",		p4_s_p_fetch),
    /* FORTH-83 uncontrolled reference words */
    CO ("!BITS",	p4_store_bits),
    CO ("@BITS",	p4_fetch_bits),
    CO ("><",		p4_byte_swap),
    CO (">MOVE<",	p4_byte_swap_move),
    CO ("**",		p4_power),
    DV ("DPL",		dpl),
    /* FORTH-83 Search order specification and control */
    CO ("SEAL",		p4_seal),
    /* FORTH-83 definition field address conversion operators */
    CO ("BODY>",	p4_body_from),
    CO (">LINK",	p4_to_link),
    CO ("LINK>",	p4_link_from),
    CO (">NAME",	p4_to_name),
    CO ("NAME>",	p4_name_from),
    CO ("L>NAME",	p4_l_to_name),
    CO ("N>LINK",	p4_n_to_link),
    /*GD*/
    CO (">FLAGS",	p4_to_ffa),
    CO (">FFA",		p4_to_ffa),
    CO ("FFA>",		p4_ffa_from),
    CO ("CASE-SENSITIVE-VOC", p4_case_sensitive_voc),
    CO ("SEARCH-ALSO-VOC", p4_search_also_voc),
    P4_INTO ("ENVIRONMENT", 0 ),
    P4_OCON ("FORTH-83",	1983),
};
P4_COUNTWORDS (forth_83, "Forth'83 compatibility");

/*@}*/

