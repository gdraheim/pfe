/**
 *  CORE-EXT -- The standard CORE and CORE-EXT wordset
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: bln_mpt1!33.74 %
 *    (%date_modified: Wed Mar 19 16:03:59 2003 %)
 *
 *  @description
 *      The Core Wordset contains the most of the essential words
 *      for ANS Forth.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
      "@(#) $Id: core-ext.c,v 1.1.1.1 2006-08-08 09:08:43 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>

#include <pfe/os-ctype.h>
#include <pfe/os-string.h>
#ifndef P4_NO_FP
#include <float.h>
#endif
#include <limits.h>

#include <pfe/core-ext.h>
#include <pfe/core-mix.h>
#include <pfe/double-sub.h>
#include <pfe/double-ext.h>
#include <pfe/tools-sub.h>
#include <pfe/tools-mix.h>
#include <pfe/dict-sub.h>
#include <pfe/header-ext.h>
#include <pfe/term-sub.h>
#include <pfe/_missing.h>

#include <pfe/logging.h>

/************************************************************************/
/* Core Words                                                           */
/************************************************************************/

/** ! ( value some-cell* -- | value addr* -- [?] ) [ANS]
 * store value at addr (sizeof =>"CELL")
 */
FCode (p4_store)
{
    *(p4cell *) SP[0] = SP[1];
    SP += 2;
}

/** # ( n,n -- n,n' ) [ANS]
 * see also => HOLD for old-style forth-formatting words
 * and => PRINTF of the C-style formatting - this word
 * divides the argument by => BASE and add it to the
 * picture space - it should be used inside of => <#
 * and => #>
 */
FCode (p4_sh)
{
    p4_hold (p4_num2dig (p4_u_d_div ((p4udcell *) SP, BASE)));
}

/** #> ( n,n -- hold-str-ptr hold-str-len ) [ANS]
 * see also => HOLD for old-style forth-formatting words
 * and => PRINTF of the C-style formatting - this word
 * drops the argument and returns the picture space
 * buffer
 */
FCode (p4_sh_greater)
{
    SP[1] = (p4cell) p4_HLD;
    SP[0] = (p4cell) (p4_PAD - p4_HLD);
}

/** #S ( n,n -- 0,0 ) [ANS]
 * see also => HOLD for old-style forth-formatting words
 * and => PRINTF of the C-style formatting - this word
 * does repeat the word => # for a number of times, until
 * the argument becomes zero. Hence the result is always
 * null - it should be used inside of => <# and => #>
 */
FCode (p4_sh_s)
{
    do {
        FX (p4_sh);
    } while (SP[0] || SP[1]);
}

/** "CFA'" ( 'name' -- name-xt* ) [FTH]
 * return the execution token of the following name. This word
 * is _not_ immediate and may not do what you expect in
 * compile-mode. See => ['] and => '> - note that in FIG-forth
 * the word =>"'" had returned the PFA (not the CFA) and therefore
 * this word was introduced being the => SYNONYM of the ans-like
 * word =>"'"
 */

/** "'" ( 'name' -- name-xt* ) [ANS]
 * return the execution token of the following name. This word
 * is _not_ immediate and may not do what you expect in
 * compile-mode. See => ['] and => '> - note that in FIG-forth
 * the word of the same name had returned the PFA (not the CFA)
 * and was immediate/smart, so beware when porting forth-code
 * from FIG-forth to ANSI-forth.
 */
FCode (p4_tick)
{
    FX_PUSH_SP = (p4cell) p4_tick_cfa (FX_VOID);
}

/** "("  ( 'comment<closeparen>' -- ) [ANS]
 * eat everything up to the next closing paren - treat it
 * as a comment.
 */
FCode (p4_paren)
{
    switch (SOURCE_ID)
    {
     case -1:
     case 0:
         p4_word_parse (')'); /* PARSE-NOHERE-NOTHROW */
         break;
     default:
         while (! p4_word_parse (')') && p4_refill ()); /* PARSE-NOHERE-NOTH */
         break;
    }
}

/** "*" ( a# b# -- mul-a#' | a b -- mul-a' [??] ) [ANS]
 * return the multiply of the two args
 */
FCode (p4_star)
{
    SP[1] = SP[0] * SP[1];
    SP++;
}

/** "*\/" ( a# b# c# -- scale-a#' | a b c -- scale-a' [??] ) [ANS]
 * regard the b/c as element Q - this word
 * has an advantage over the sequence of => *
 * and => / by using an intermediate double-cell
 * value
 */
FCode (p4_star_slash)
{
    fdiv_t res = p4_d_fmdiv (p4_d_mmul (SP[2], SP[1]), SP[0]);

    SP += 2;
    SP[0] = res.quot;
}

/** "*\/MOD" ( a# b# c# -- div-a# mod-a# | a b c -- div-a mod-a [??] ) [ANS]
 * has an adavantage over the sequence of => *
 * and => /MOD by using an intermediate double-cell
 * value.
 */
FCode (p4_star_slash_mod)
{
    *(fdiv_t *) &SP[1] = p4_d_fmdiv (p4_d_mmul (SP[2], SP[1]), SP[0]);
    SP++;
}

/** + ( a* b# -- a*' | a# b* -- b*' | a# b# -- a#' | a b -- a' [??] ) [ANS]
 * return the sum of the two args
 */
FCode (p4_plus)
{
    SP[1] += SP[0];
    SP++;
}

/** +! ( value# some-cell* -- | value some* -- [?] ) [ANS]
 * add val to the value found in addr
 simulate:
   : +! TUCK @ + SWAP ! ;
 */
FCode (p4_plus_store)
{
    *(p4cell *) SP[0] += SP[1];
    SP += 2;
}

/** "((+LOOP))" ( increment# -- ) [HIDDEN]
 * compiled by => +LOOP
 */ 
FCode_XE (p4_plus_loop_execution)
{   FX_USE_CODE_ADDR {

#  ifndef PFE_SBR_CALL_THREADING
    p4cell i = *SP++;
    if (i < 0
      ? (*FX_RP += i) >= 0
      : (*FX_RP += i) < 0)
    {
        IP = RP[2];
    }else{
        FX_RP_DROP (3);
        FX_RP_EXIT;
    }
#  else
    if (*SP < 0)
    {
	FX_RP[0] += *SP++;
	if (FX_RP[0] >= 0)
	    goto branch;
    }else{
	FX_RP[0] += *SP++;
	if (FX_RP[0] < 0)
	    goto branch;
    }
    FX_RP_DROP (3);
    FX_RP_EXIT;
    return;
 branch:
    FX_NEW_RP_WORK;
    FX_NEW_RETVAL = FX_NEW_RP [2];
    FX_NEW_RP_DONE;
    FX_NEW_RP_EXIT;
#  endif
    FX_USE_CODE_EXIT;
}}

/** +LOOP ( increment# R: some,loop -- ) [ANS]
 * compile => ((+LOOP)) which will use the increment
 * as the loop-offset instead of just 1. See the
 * => DO and => LOOP construct.
 */
FCode (p4_plus_loop)
{
    p4_Q_pairs (P4_LOOP_MAGIC);
    FX_COMPILE (p4_plus_loop);
    FX (p4_forward_resolve);
}
P4COMPILES (p4_plus_loop, p4_plus_loop_execution,
          P4_SKIPS_NOTHING, P4_LOOP_STYLE);

/** "," ( value* -- | value# -- | value -- [?] ) [ANS]
 * store the value in the dictionary
 simulate:
   : , DP  1 CELLS DP +!  ! ;
 */
FCode (p4_comma)
{
    FX_VCOMMA (*SP++);
}

/** "-" ( a* b# -- a*' | a# b* -- b*' | a# b# -- a#' | a* b* -- diff-b#' | a b -- a' [??] ) [ANS]
 * return the difference of the two arguments
 */
FCode (p4_minus)
{
    SP[1] -= SP[0];
    SP++;
}

/** "." ( value# -- | value* -- [?] | value -- [??] ) [ANS]
 * print the numerical value to stdout - uses => BASE
 */
FCode (p4_dot)
{
    FX (p4_s_to_d);
    FX (p4_d_dot);
}

/** '((.\"))' ( -- ) [HIDDEN] skipstring
 * compiled by => ." string"
 */ 
FCode_XE (p4_dot_quote_execution)
{   FX_USE_CODE_ADDR {
#  ifndef PFE_SBR_CALL_THREADING
    register p4_char_t *p = (p4_char_t *) IP;
    p4_type (p + 1, *p);
    FX_SKIP_STRING;
#  else
    FX_NEW_IP_WORK;
    p4_type (FX_NEW_IP_CHAR +1, *FX_NEW_IP_CHAR);
    FX_NEW_IP_SKIP_STRING;
    FX_NEW_IP_DONE;
#  endif
    FX_USE_CODE_EXIT;
}}

/** '.\"' ( [string<">] -- ) [ANS]
 * print the string to stdout
 */
FCode (p4_dot_quote)
{
    _FX_STATESMART_Q_COMP;
    if (STATESMART) 
    {
        FX_COMPILE (p4_dot_quote);
	FX (p4_parse_comma_quote);
    }else{
        p4_word_parse ('"'); /* PARSE - no throw HERE */
        p4_type (PFE.word.ptr, PFE.word.len);
    }
}
P4COMPILES (p4_dot_quote, p4_dot_quote_execution,
          P4_SKIPS_STRING, P4_DEFAULT_STYLE);

/** "/" ( a# b#  -- a#' | a b -- a' [???] ) [ANS]
 * return the quotient of the two arguments
 */
FCode (p4_slash)
{
    fdiv_t res = p4_fdiv (SP[1], SP[0]);

    *++SP = res.quot;
}

/** "/MOD" ( a# b# -- div-a#' mod-a#' | a b -- div-a' mod-a' [??] ) [ANS]
 * divide a and b and return both
 * quotient n and remainder m
 */
FCode (p4_slash_mod)
{
    *(fdiv_t *) SP = p4_fdiv (SP[1], SP[0]);
}

/** 0< ( value -- test-flag ) [ANS]
 * return a flag that is true if val is lower than zero
 simulate:
  : 0< 0 < ;
 */
FCode (p4_zero_less)
{
    *SP = P4_FLAG (*SP < 0);
}

/** 0= ( 0 -- test-flag! | value! -- 0 | value -- test-flag ) [ANS]
 * return a flag that is true if val is just zero
 simulate:
  : 0= 0 = ;
 */
FCode (p4_zero_equal)
{
    *SP = P4_FLAG (*SP == 0);
}

/** 1+ ( value -- value' ) [ANS]
 * return the value incremented by one
 simulate:
  : 1+ 1 + ;
 */
FCode (p4_one_plus)
{
    ++*SP;
}

/** 1- ( value -- value' ) [ANS]
 * return the value decremented by one
 simulate:
   : 1- 1 - ;
 */
FCode (p4_one_minus)
{
    --*SP;
}

/** 2! ( x,x variable* -- ) [ANS]
 * double-cell store 
 */
FCode (p4_two_store)
{
    *(p4dcell *) *SP = *(p4dcell *) &SP[1];
    SP += 3;
}

/** 2* ( a# -- a#' | a -- a' [??] ) [ANS]
 * multiplies the value with two - but it
 * does actually use a shift1 to be faster
 simulate:
  : 2* 2 * ; ( canonic) : 2* 1 LSHIFT ; ( usual)
 */
FCode (p4_two_star)
{
    *SP <<= 1;
}

/** 2/ ( a# -- a#' | a -- a' [??] ) [ANS]
 * divides the value by two - but it
 * does actually use a shift1 to be faster
 simulate:
  : 2/ 2 / ; ( canonic) : 2/ 1 RSHIFT ; ( usual)
 */
FCode (p4_two_slash)
{
    *SP >>= 1;
}

/** 2@ ( variable* -- x,x ) [ANS]
 * double-cell fetch
 */
FCode (p4_two_fetch)
{
    p4dcell *p = (p4dcell *) *SP--;

    *(p4dcell *) SP = *p;
}

/** 2DROP ( a b -- ) [ANS]
 * double-cell drop, also used to drop two items
 */
FCode (p4_two_drop)
{
    SP += 2;
}

/** 2DUP ( a,a -- a,a a,a ) [ANS]
 * double-cell duplication, also used to duplicate
 * two items
 simulate:
   : 2DUP OVER OVER ; ( wrong would be : 2DUP DUP DUP ; !!) 
 */
FCode (p4_two_dup)
{
    SP -= 2;
    SP[0] = SP[2];
    SP[1] = SP[3];
}

/** 2OVER ( a,a b,b -- a,a b,b a,a ) [ANS]
 * double-cell over, see => OVER and => 2DUP
 simulate:
   : 2OVER SP@ 2 CELLS + 2@ ;
 */
FCode (p4_two_over)
{
    SP -= 2;
    SP[0] = SP[4];
    SP[1] = SP[5];
}

/** 2SWAP ( a,a b,b -- b,b a,a ) [ANS]
 * double-cell swap, see => SWAP and => 2DUP
 simulate:
   : 2SWAP LOCALS| B1 B2 A1 A2 | B2 B1 A2 A1 ;
 */
FCode (p4_two_swap)
{
    p4cell h;

    h = SP[0];
    SP[0] = SP[2];
    SP[2] = h;
    h = SP[1];
    SP[1] = SP[3];
    SP[3] = h;
}

/** "(NEST)" ( -- ) [HIDDEN]
 * compiled by => :
 * (see also => (NONAME) compiled by => :NONAME )
 */
FCode_RT (p4_colon_RT)
{   FX_USE_BODY_ADDR {
#  if   ! defined PFE_CALL_THREADING
    FX_PUSH_RP = IP;
    IP = (p4xcode *) FX_POP_BODY_ADDR;
#  elif ! defined PFE_SBR_CALL_THREADING
    FX_POP_BODY_ADDR_p4_BODY;
    FX_PUSH_RP = IP; IP = (p4xcode *) p4_BODY;
#  else
    p4code c = (p4code) FX_POP_BODY_ADDR;
    c ();
#  endif
}}

FCode (p4_colon_EXIT)
{
    FX (p4_Q_csp);
    STATE = P4_FALSE;
    FX (p4_reveal);
}    

/** ":" ( 'name' -- ) [ANS] [NEW]
 * create a header for a nesting word and go to compiling
 * mode then. This word is usually ended with => ; but
 * the execution of the resulting colon-word can also 
 * return with => EXIT
 */
FCode (p4_colon)
{
    FX (p4_Q_exec);
    FX_RUNTIME_HEADER; FX_SMUDGED;
#  ifndef PFE_SBR_CALL_THREADING
    FX_RUNTIME1 (p4_colon);
#  else
    { 
	/*atic const char* nest_code = "_"; */
	static const char _nest_code[] = { p4_NEST, 0 };
	static const char* nest_code = _nest_code;
	FX_COMMA (&nest_code); /* CODE trampoline */ 
    }
    FX_COMPILE_PROC;
#  endif 
    FX (p4_store_csp);
    STATE = P4_TRUE;
    PFE.locals = NULL;
    PFE.semicolon_code = PFX(p4_colon_EXIT);
}
P4RUNTIME1(p4_colon, p4_colon_RT);

/** "((;))" ( -- ) [HIDDEN] [EXIT]
 * compiled by => ; and maybe => ;AND --
 * it will perform an => EXIT
 */ 
FCode_XE (p4_semicolon_execution)
{
#  if !defined PFE_SBR_CALL_THREADING
    FX_USE_CODE_ADDR;
    IP = *RP++;
    FX_USE_CODE_EXIT;
#  endif
}


/** ";" ( -- ) [ANS] [EXIT] [END]
 * compiles => ((;)) which does => EXIT the current
 * colon-definition. It does then end compile-mode
 * and returns to execute-mode. See => : and => :NONAME
 */
FCode (p4_semicolon)
{
    if (PFE.semicolon_code)
    {
        PFE.semicolon_code ();
    }else{
        PFE.state = P4_FALSE; /* atleast switch off compiling mode */
    }

    if (PFE.locals)
    {
        FX_COMPILE2_p4_semicolon;
        PFE.locals = NULL;
    }
    else
        FX_COMPILE1_p4_semicolon; /* in SBR-threading, compiles RET-code */
}

P4COMPILES2 (p4_semicolon, p4_semicolon_execution, p4_locals_exit_execution,
           P4_SKIPS_NOTHING, P4_SEMICOLON_STYLE);

/** < ( a* b* -- test-flag | a# b# -- test-flag | a b -- test-flag [?] ) [ANS]
 * return a flag telling if a is lower than b
 */
FCode (p4_less_than)
{
    SP[1] = P4_FLAG (SP[1] < SP[0]);
    SP++;
}

/** <# ( -- ) [ANS]
 * see also => HOLD for old-style forth-formatting words
 * and => PRINTF of the C-style formatting - this word
 * does initialize the pictured numeric output space.
 */
FCode (p4_less_sh)
{
    p4_HLD = p4_PAD;
}

/** = ( a* b* -- test-flag | a# b# -- test-flag | a b -- test-flag [?] ) [ANS]
 * return a flag telling if a is equal to b
 */
FCode (p4_equals)
{
    SP[1] = P4_FLAG (SP[1] == SP[0]);
    SP++;
}

/** > ( a* b* -- test-flag | a# b# -- test-flag | a b -- test-flag [?] ) [ANS]
 * return a flag telling if a is greater than b
 */
FCode (p4_greater_than)
{
    SP[1] = P4_FLAG (SP[1] > SP[0]);
    SP++;
}

/** >BODY ( some-xt* -- some-body* ) [ANS]
 * adjust the execution-token (ie. the CFA) to point
 * to the parameter field (ie. the PFA) of a word.
 * this is not a constant operation - most words have their
 * parameters at "1 CELLS +" but CREATE/DOES-words have the
 * parameters at "2 CELLS +" and ROM/USER words go indirect
 * with a rom'ed offset i.e. "CELL + @ UP +"
 */
FCode (p4_to_body) 
{
    *SP = (p4cell) p4_to_body ((p4xt) *SP);
}

/** >NUMBER ( a,a str-ptr str-len -- a,a' str-ptr' str-len) [ANS]
 * try to convert a string into a number, and place
 * that number at a,a respeciting => BASE
 */
FCode (p4_to_number)
{
    SP[1] = (p4cell)
        p4_to_number (
                      (p4_char_t *) SP[1],
                      (p4ucell *) &SP[0],
                      (p4udcell *) &SP[2],
                      BASE);
}

/** >R ( value -- R: value ) [ANS]
 * save the value onto the return stack. The return
 * stack must be returned back to clean state before
 * an exit and you should note that the return-stack
 * is also touched by the => DO ... => WHILE loop.
 * Use => R> to clean the stack and => R@ to get the 
 * last value put by => >R
 */
FCode (p4_to_r)
{
    FX (p4_Q_comp);
    FX_COMPILE (p4_to_r);
}
FCode_XE (p4_to_r_execution)
{
    FX_USE_CODE_ADDR;
#  if !defined PFE_SBR_CALL_THREADING
    RP_PUSH (FX_POP);
#  else
    FX_NEW_RP_WORK;
    FX_NEW_RP_PUSH (FX_POP);
    FX_NEW_RP_DONE;
    FX_NEW_RP_EXIT;
#  endif
    FX_USE_CODE_EXIT;
}
P4COMPILES (p4_to_r, p4_to_r_execution, 
	    P4_SKIPS_NOTHING, P4_DEFAULT_STYLE);
    

/** ?DUP ( 0 -- 0 | value! -- value! value! | value -- 0 | value! value! ) [ANS]
 * one of the rare words whose stack-change is 
 * condition-dependet. This word will duplicate
 * the value only if it is not zero. The usual
 * place to use it is directly before a control-word
 * that can go to different places where we can
 * spare an extra => DROP on the is-null-part.
 * This makes the code faster and often a little
 * easier to read.
 example:
   : XX BEGIN ?DUP WHILE DUP . 2/ REPEAT ; instead of
   : XX BEGIN DUP WHILE DUP . 2/ REPEAT DROP ;
 */
FCode (p4_Q_dup)
{
    if (*SP)
        --SP, SP[0] = SP[1];
}

/** @ ( value* -- value ) [ANS]
 * fetch the value from the variables address
 */
FCode (p4_fetch)
{
    *SP = *(p4cell *) *SP;
}

/** ABS ( value# -- value#' ) [ANS]
 * return the absolute value
 */
FCode (p4_abs)
{
    if (*SP < 0)
        *SP = -*SP;
}

/** ACCEPT ( buffer-ptr buffer-max -- buffer-len ) [ANS]
 * get a string from terminal into the named input 
 * buffer, returns the number of bytes being stored
 * in the buffer. May provide line-editing functions.
 */
FCode (p4_accept)
{
    SP[1] = p4_accept ((p4_char_t *) SP[1], SP[0]);
    SP += 1;
}

/** ALIGN ( -- ) [ANS]
 * will make the dictionary aligned, usually to a
 * cell-boundary, see => ALIGNED
 */
FCode (p4_align)
{
    while (! P4_ALIGNED (DP))
        *DP++ = 0;
}

/** ALIGNED ( addr -- addr' ) [ANS]
 * uses the value (being usually a dictionary-address)
 * and increment it to the required alignment for the
 * dictionary which is usually in => CELLS - see also
 * => ALIGN
 */
FCode (p4_aligned)
{
    *SP = p4_aligned (*SP);
}

/** ALLOT ( allot-count -- ) [ANS]
 * make room in the dictionary - usually called after
 * a => CREATE word like => VARIABLE or => VALUE
 * to make for an array of variables. Does not 
 * initialize the space allocated from the dictionary-heap.
 * The count is in bytes - use => CELLS ALLOT to allocate 
 * a field of cells.
 */
FCode (p4_allot)
{
    DP += *SP++;
}

/** AND ( value mask -- value' ) [ANS]
 * mask with a bitwise and - be careful when applying
 * it to logical values.
 */
FCode (p4_and)
{
    SP[1] &= SP[0];
    SP++;
}

/** BEGIN ( -- ) [ANS] [LOOP]
 * start a control-loop, see => WHILE and => REPEAT
 */
FCode (p4_begin)
{
    FX_COMPILE (p4_begin);
    FX (p4_backward_mark);
    FX_PUSH_SP = P4_DEST_MAGIC;
}
P4COMPILES (p4_begin, p4_noop, P4_SKIPS_NOTHING, P4_BEGIN_STYLE);

/** C! ( value# variable#* -- | value# variable* [?] ) [ANS]
 * store the byte-value at address, see => !
 */
FCode (p4_c_store)
{
    *(char *) SP[0] = SP[1];
    SP += 2;
}

/** C, ( value# -- ) [ANS]
 * store a new byte-value in the dictionary, implicit 1 ALLOT,
 * see => ,
 */
FCode (p4_c_comma)
{
    *DP++ = (p4char) *SP++;
}

/** C@ ( value#* -- value# | value* -- value# [?] ) [ANS]
 * fetch a byte-value from the address, see => @
 */
FCode (p4_c_fetch)
{
    *SP = *(p4char *) *SP;
}

/** CELL+ ( value -- value' ) [ANS]
 * adjust the value by adding a single Cell's width
 * - the value is often an address or offset, see => CELLS
 */
FCode (p4_cell_plus)
{
    *SP += sizeof (p4cell);
}

/** CELLS ( value# -- value#' ) [ANS]
 * scale the value by the sizeof a Cell
 * the value is then often applied to an address or
 * fed into => ALLOT
 */
FCode (p4_cells)
{
    *SP *= sizeof (p4cell);
}

/** CHAR ( 'word' -- char# ) [ANS]
 * return the (ascii-)value of the following word's
 * first character. 
 */
FCode (p4_char)
{
    p4_word_parseword (' '); *DP=0; /* PARSE-WORD-NOHERE */
    if (! PFE.word.len)
        p4_throw (P4_ON_INVALID_NAME);
    FX_PUSH_SP = (p4ucell) *(p4char*)PFE.word.ptr;
}

/** CHAR+ ( value -- value' ) [ANS]
 * increment the value by the sizeof one char
 * - the value is often a pointer or an offset,
 * see => CHARS
 */
FCode (p4_char_plus)
{
    *SP += sizeof (char);
}

/** CHARS ( value# -- value#' ) [ANS]
 * scale the value by the sizeof a char
 * - the value is then often applied to an address or
 * fed into => ALLOT (did you expect that sizeof(p4char)
 * may actually yield 2 bytes?)
 */
FCode (p4_chars)
{
    *SP *= sizeof (char);
}

static P4_CODE_RUN(p4_constant_RT_SEE)
{
    p4_strcat (p, p4_str_dot (*P4_TO_BODY (xt), p+200, BASE));
    p4_strcat (p, "CONSTANT ");
    p4_strncat (p, (char*) P4_NFA_PTR(nfa), P4_NFA_LEN(nfa));
    return 0;
}

/** "((CONSTANT))" ( -- ) [HIDDEN]
 * runtime compiled by => CONSTANT
 */ 
FCode_RT (p4_constant_RT)
{
    FX_USE_BODY_ADDR;
    FX_PUSH_SP = FX_POP_BODY_ADDR[0];
}

/** CONSTANT ( value 'name' -- ) [ANS] [DOES: -- value ]
 * => CREATE a new word with runtime => ((CONSTANT))
 * so that the value placed here is returned everytime
 * the constant's name is used in code. See => VALUE
 * for constant-like names that are expected to change
 * during execution of the program. In a ROM-able
 * forth the CONSTANT-value may get into a shared
 * ROM-area and is never copied to a RAM-address.
 */
FCode (p4_constant)
{
    FX_RUNTIME_HEADER; 
    FX_RUNTIME1 (p4_constant);
    FX_VCOMMA (*SP++);
}
P4RUNTIMES1_(p4_constant, p4_constant_RT, 0,p4_constant_RT_SEE);


/** COUNT ( string-bstr* -- string-ptr' string-len | some* -- some*' some-len [?] ) [ANS]
 * usually before calling => TYPE
 *
 * (as an unwarranted extension, this word does try to be idempotent).
 */
FCode (p4_count)
{
    /* can not unpack twice - this trick prevents from many common errors */
    if (256 > (p4ucell)(SP[0])) goto possibly_idempotent;
    --SP;
    SP[0] = *P4_INCC(SP[1], p4char); /* SP[0] = *((p4char*) SP[1] )++; */
    return;

    /* an idempotent COUNT allows to ease the transition from counted-strings
     * to string-spans:
     c" hello world" count type ( is identical with...)
     s" hello world" count type
     * however: it makes some NULL argument or just illegal argument to be
     * silently accepted that can make debugging programs a pain. Therefore
     * this function has been given some intelligence, with the counter effect
     * of being somewhat undetermined which part gets triggered at runtime.
     */
 possibly_idempotent:
    if (((p4char**)SP)[1][-1] == (p4char)(SP[0])) /* idempotent ? */
    { if ((p4char)(SP[0])) return; } /* only if not null-count ! */
    FX_PUSH (0); /* makes later functions to copy nothing at all */
}

/** CR ( -- ) [ANS]
 * print a carriage-return/new-line on stdout
 */  
FCode (p4_cr)
{
    p4_outc ('\n');
    p4_OUT = 0;
    PFE.lines++;
}

/** DECIMAL ( -- ) [ANS]
 * set the => BASE to 10
 simulate:
   : DECIMAL 10 BASE ! ;
 */
FCode (p4_decimal)
{
    BASE = 10;
}

/** DEPTH ( -- depth# ) [ANS]
 * return the depth of the parameter stack before
 * the call, see => SP@ - the return-value is in => CELLS
 */
FCode (p4_depth)
{
    register size_t n;

    n = p4_S0 - SP;
    FX_PUSH_SP = n;
}

/* implementation detail:
 * DO will compile (DO) and forward-address to LOOP
 * (DO) will set RP[2] to its point after that forward-adress
 * LOOP can just jump to RP[2]
 * LEAVE can jump via RP[2][-1] forward-address
 */

#  if defined P4_IP_VIA_RP || defined PFE_HOST_ARCH_I386
#  if defined PFE_SBR_CALL_THREADING /* PFE_SBR_CALL_ARG_THREADING */
#  define P4_SBR_DO_EXECUTION_3
static FCode(p4_sbr_do_execution_3);
#  endif
#  endif

#  if defined PFE_SBR_CALL_ARG_THREADING
#  define P4_SBR_DO_EXECUTION_3
#  endif

/** "((DO))" ( end# start# -- ) [HIDDEN]
 * compiled by => DO
 */ 
FCode_XE (p4_do_execution)
{
    FX_USE_CODE_ADDR;
#  if   ! defined PFE_SBR_CALL_THREADING
    RP -= 3;                     /* push onto return-stack: */
    RP[2] = ++IP;                /* IP to jump back to just after DO */
    RP[1] = (p4xcode *) SP[1];   /* upper limit */
    RP[0] = (p4xcode *) (SP[0] - /*lower_minus*/  SP[1] /*upper_limit*/ );
    FX_2DROP;
#  elif ! defined P4_SBR_DO_EXECUTION_3 /* ! PFE_SBR_CALL_ARG_THREADING */
    FX_NEW_RP_WORK; 
    FX_NEW_RP_ROOM (3);
    FX_NEW_RP_AT(2, ++FX_NEW_RETVAL);
    FX_NEW_RP_AT(1, SP[1]);
    FX_NEW_RP_AT(0, SP[0] - SP[1]);
    FX_2DROP;
    FX_NEW_RP_DONE;
    FX_NEW_RP_EXIT;
#  else /* this one is desperately needed on i386 */
    FX_NEW_RP_WORK; 
    FX_NEW_RP_ROOM (3);
    FX_PUSH(FX_NEW_RP);
    FX_NEW_RETVAL++;
    FX_NEW_RP_DONE;
    FX (p4_sbr_do_execution_3);
    FX_NEW_RP_EXIT;
#  endif
    FX_USE_CODE_EXIT;
}

#ifdef P4_SBR_DO_EXECUTION_3
static FCode(p4_sbr_do_execution_3) 
{
    P4_REGRP_T rP = (P4_REGRP_T) FX_POP;
    rP [2] = rP[-1]; /* get RETVAL -> IP_VIA_RP */
    rP [1] = (P4_REGRP_TARGET_T) (SP[1]);
    rP [0] = (P4_REGRP_TARGET_T) (SP[0] - SP[1]);
    FX_2DROP;
}
#endif

/** DO ( end# start# | end* start* -- R: some,loop ) [ANS] [LOOP]
 *  pushes $end and $start onto the return-stack ( => >R )
 *  and starts a control-loop that ends with => LOOP or
 *  => +LOOP and may get a break-out with => LEAVE . The
 *  loop-variable can be accessed with => I
 */
FCode (p4_do)
{
    FX_COMPILE (p4_do);
    FX (p4_forward_mark);
    FX_PUSH_SP = P4_LOOP_MAGIC;
}
P4COMPILES (p4_do, p4_do_execution,
  P4_SKIPS_OFFSET, P4_DO_STYLE);

/** "((VAR))" ( -- pfa ) [HIDDEN]
 * the runtime compiled by => VARIABLE
 */ 
FCode_RT (p4_variable_RT)
{
    FX_USE_BODY_ADDR;
    FX_PUSH_SP = (p4cell) FX_POP_BODY_ADDR;
}

static P4_CODE_RUN(p4_builds_RT_SEE)
{
    p4_strcat (p, "CREATE ");
    p4_strncat (p, (char*) P4_NFA_PTR(nfa), P4_NFA_LEN(nfa));
    return 0;
}

/** "((BUILDS))" ( -- pfa ) [HIDDEN]
 * the runtime compiled by => CREATE which
 * is not much unlike a => VARIABLE 
 * (in ANS Forth Mode we reserve an additional DOES-field)
 */ 
FCode (p4_builds_RT)
{
    FX_USE_BODY_ADDR;
    FX_PUSH_SP = (p4cell)( FX_POP_BODY_ADDR + 1 );
}

/** "((DOES>))" ( -- pfa ) [HIDDEN]
 * runtime compiled by DOES>
 */ 
FCode_RT (p4_does_RT)
{   FX_USE_BODY_ADDR {
#  if   ! defined PFE_CALL_THREADING
    FX_PUSH_SP = (p4cell) P4_TO_DOES_BODY(WP_CFA);  /* from CFA[2] */
    FX_PUSH_RP = IP; IP = *P4_TO_DOES_CODE(WP_CFA); /* from CFA[1] */
#  elif ! defined PFE_SBR_CALL_THREADING
    p4xt xt = (p4xt) (FX_POP_BODY_ADDR-1);
    FX_PUSH_SP = (p4cell) P4_TO_DOES_BODY(xt);  /* from CFA[2] */
    FX_PUSH_RP = IP; IP = *P4_TO_DOES_CODE(xt); /* from CFA[1] */
#  else
    p4xt xt = (p4xt) (FX_POP_BODY_ADDR-1);
    FX_PUSH_SP = (p4cell) P4_TO_DOES_BODY(xt);  /* from CFA[2] */
    ((p4code)(*P4_TO_DOES_CODE(xt)))(); /* from CFA[1] */
#  endif
}}
P4RUNTIME1(p4_does, p4_does_RT);

/** "(DOES>)" ( -- pfa ) [HIDDEN]
 * execution compiled by => DOES>
 */ 
FCode_XE (p4_does_execution)
{   FX_USE_CODE_ADDR {
#  if   ! defined PFE_SBR_CALL_THREADING
    p4xt xt;
    if (! LAST)
        p4_throw (P4_ON_ARG_TYPE);

    xt = p4_name_from (LAST);
    P4_XT_VALUE(xt) = FX_GET_RT (p4_does); 
    *P4_TO_DOES_CODE(xt) = IP; /* into CFA[1] */

    if (LP != FX_RP)
        IP = *RP++;   /* double-EXIT */
    else
        FX (p4_locals_exit_execution);
#  else
    FX_NEW_IP_WORK;
    if (! LAST)
        p4_throw (P4_ON_ARG_TYPE);
    
    {
        p4xt xt = p4_name_from (LAST);
        P4_XT_VALUE(xt) = FX_GET_RT (p4_does); 
        *P4_TO_DOES_CODE(xt) = FX_NEW_IP_CODE; /* into CFA[1] */
    }
    FX_NEW_IP_CODE = PFX (p4_noop);   /* double-EXIT */
    FX_NEW_IP_DONE;
#  endif
    FX_USE_CODE_EXIT;
}}

/** "DOES>" ( -- does* ) [ANS] [END] [NEW]
 * does twist the last => CREATE word to carry
 * the => (DOES>) runtime. That way, using the
 * word will execute the code-piece following => DOES>
 * where the pfa of the word is already on stack.
 * (note: FIG option will leave pfa+cell since does-rt is stored in pfa)
 */
FCode (p4_does)
{
    _FX_STATESMART_Q_COMP;
    if (STATESMART)
    {
        FX (p4_Q_csp);
        FX_COMPILE (p4_does);
        PFE.locals = NULL;
    }else{
        /* see p4_does_execution above */
        p4xt xt;
        if (! LAST)
            p4_throw (P4_ON_ARG_TYPE);
        FX (p4_align);

        xt = p4_name_from (LAST);
        P4_XT_VALUE(xt) = FX_GET_RT (p4_does); 
        *P4_TO_DOES_CODE(xt) = (p4xcode*) DP; /* into CFA[1] */

        /* now, see p4_colon */
        FX (p4_store_csp);
        STATE = P4_TRUE;
        PFE.locals = NULL;
        PFE.semicolon_code = PFX(p4_colon_EXIT);
    }
}
P4COMPILES (p4_does, p4_does_execution,
  P4_SKIPS_NOTHING, P4_DOES_STYLE);

/** CREATE ( 'name' -- ) [ANS]
 * create a name with runtime => ((VAR)) so that everywhere the name is used 
 * the pfa of the name's body is returned. This word is not immediate and 
 * according to the ANS Forth documents it may get directly used in the 
 * first part of a => DOES> defining word - in traditional forth systems
 * the word =>"<BUILDS" was used for that and => CREATE was defined to be
 * the first part of a => VARIABLE word (compare with =>"CREATE:" and the
 * portable expression =>"0" =>"BUFFER:")
 */

/** <BUILDS ( 'name' -- ) [FTH]
 *  make a => HEADER whose runtime will be changed later
 *  using => DOES>  <br />
 *  note that ans'forth does not define => <BUILDS and
 *  it suggests to use => CREATE directly. <br />
 *  ... if you want to write FIG-programs in pure pfe then you have
 *  to use => CREATE: to get the FIG-like meaning of => CREATE whereas
 *  the ans-forth => CREATE is the same as =>"<BUILDS"
 : <BUILDS BL WORD HEADER DOCREATE A, 0 A, ;
 */
FCode (p4_builds)
{
    FX_RUNTIME_HEADER;
    FX_RUNTIME1 (p4_builds);
    FX_RCOMMA (0);
}
P4RUNTIMES1_(p4_builds, p4_builds_RT, 0, p4_builds_RT_SEE);


/** DROP ( a -- ) [ANS]
 * just drop the word on the top of stack, see => DUP
 */
FCode (p4_drop)
{
    SP++;
}

/** DUP ( a -- a a ) [ANS]
 * duplicate the cell on top of the stack - so the
 * two topmost cells have the same value (they are
 * equal w.r.t => = ) , see => DROP for the inverse
 */
FCode (p4_dup)
{
    --SP;
    SP[0] = SP[1];
}

/** "((ELSE))" ( -- ) [HIDDEN]
 * execution compiled by => ELSE - just a simple
 * => BRANCH
 */ 
FCode_XE (p4_else_execution)
{
    FX_USE_CODE_ADDR;
    FX_BRANCH;
    FX_USE_CODE_EXIT;
}

/** ELSE ( -- ) [HIDDEN]
 * will compile an => ((ELSE)) => BRANCH that performs an 
 * unconditional jump to the next => THEN - and it resolves 
 * an => IF for the non-true case
 */
FCode (p4_else)
{
    p4_Q_pairs (P4_ORIG_MAGIC);
    FX_COMPILE (p4_else);
    FX (p4_ahead) ;
    FX (p4_rot) ;
    FX (p4_forward_resolve) ;
}
P4COMPILES (p4_else, p4_else_execution,
  P4_SKIPS_OFFSET, P4_ELSE_STYLE);

/** EMIT ( char# -- ) [ANS]
 * print the char-value on stack to stdout
 */
FCode (p4_emit)
{
    PFE.execute (PFE.emit);
}

/** ENVIRONMENT? ( name-ptr name-len -- 0 | ?? name-flag! ) [ANS]
 * check the environment for a property, usually
 * a condition like questioning the existance of 
 * specified wordset, but it can also return some
 * implementation properties like "WORDLISTS"
 * (the length of the search-order) or "#LOCALS"
 * (the maximum number of locals) 

 * Here it implements the environment queries as a => SEARCH-WORDLIST 
 * in a user-visible vocabulary called => ENVIRONMENT
 : ENVIRONMENT?
   ['] ENVIRONMENT >WORDLIST SEARCH-WORDLIST
   IF  EXECUTE TRUE ELSE  FALSE THEN ;
 */
FCode (p4_environment_Q_core)
{
# if 1
    extern FCode (p4_environment_Q);
    FX (p4_environment_Q);
# else
    p4cell len = SP[0];

    if (len > 256 || -256 > len ) 
    {  /* this scheme allows you to submit a forth counted string */
        P4_warn ("counted string at query to ENVIRONMENT?");
        FX (p4_count); 
        len = SP[0];
    }

    if (0 < len && len < 32 && PFE.environ_wl) 
    { 
        p4char* nfa = p4_search_wordlist ((void*) SP[1], len, PFE.environ_wl);
        if (nfa)
        {
            FX_2DROP;
            if (PFE_IS_DESTROYER(nfa))
                FX_PUSH_SP = P4_TO_BODY(p4_name_from(nfa));
            else
                p4_call (p4_name_from(nfa));
            FX_PUSH(P4_TRUE);
            return;
        }
    }

    /* not found */
    FX_DROP;
    *SP = 0;
# endif
}


/** EVALUATE ( str-ptr str-len -- ) [ANS]
 * => INTERPRET the given string, => SOURCE id
 * is -1 during that time.
 */
FCode (p4_evaluate)
{
    p4_char_t *p = (p4_char_t *) SP[1];
    int n = SP[0];

    SP += 2;
    p4_evaluate (p, n);
}

/** EXECUTE ( some-xt* -- ??? ) [ANS]
 * run the execution-token on stack - this will usually
 * trap if it was null for some reason, see => >EXECUTE
 simulate:
  : EXECUTE >R EXIT ;
 */
FCode (p4_execute)
{
#  ifndef PFE_CALL_THREADING
    PFE.execute ((p4xt) *SP++);
#  else
    p4_call ((p4xt) *SP++);
#  endif
}

/** EXIT ( -- ) [ANS] [EXIT]
 * will unnest the current colon-word so it will actually
 * return the word calling it. This can be found in the
 * middle of a colon-sequence between => : and => ;
 */
FCode (p4_exit)
{
    if (PFE.locals)
    {   FX_COMPILE2_p4_exit; }
    else
    { FX_COMPILE1_p4_exit; }
}
P4COMPILES2 (p4_exit, p4_semicolon_execution, p4_locals_exit_execution,
           P4_SKIPS_NOTHING, P4_DEFAULT_STYLE);

/** FILL ( mem-ptr mem-len char# -- ) [ANS]
 * fill a memory area with the given char, does now
 * simply call p4_memset()
 */
FCode (p4_fill)
{
    p4_memset ((void *) SP[2], SP[0], SP[1]);
    SP += 3;
}

/** FIND ( name-bstr* -- name-bstr* 0 | name-xt* -1|1 ) [ANS]
 * looks into the current search-order and tries to find
 * the name string as the name of a word. Returns its
 * execution-token or the original-bstring if not found,
 * along with a flag-like value that is zero if nothing
 * could be found. Otherwise it will be 1 (a positive value)
 * if the word had been immediate, -1 otherwise (a negative
 * value).
 */
FCode (p4_find)
{
    p4char *p = (p4char *) *SP;

    p = p4_find (p + 1, *p);
    if (p)
    {
        *SP = (p4cell) p4_name_from (p);
        FX_PUSH_SP = P4_NFA_xIMMEDIATE(p) ? P4_POSITIVE : P4_NEGATIVE;
    }
    else
        FX_PUSH_SP = 0;
}

/** "FM/MOD" ( n1,n1# n2# -- div-n1# mod-n1# ) [ANS]
 * divide the double-cell value n1 by n2 and return
 * both (floored) quotient n and remainder m 
 */
FCode (p4_f_m_slash_mod)
{
    p4cell denom = *SP++;

    *(fdiv_t *) SP = p4_d_fmdiv (*(p4dcell *) SP, denom);
}

/** HERE ( -- here* ) [ANS]
 * used with => WORD and many compiling words
 simulate:   : HERE DP @ ;
 */
FCode (p4_here)
{
    FX_PUSH_SP = (p4cell) DP;
}

/** HOLD ( char# -- ) [ANS]
 * the old-style forth-formatting system -- this
 * word adds a char to the picutred output string.
 */
FCode (p4_hold)
{
    p4_hold ((char) *SP++);
}

/** I ( R: some,loop -- S: i# ) [ANS]
 * returns the index-value of the innermost => DO .. => LOOP
 */
FCode (p4_i)
{
    FX_COMPILE (p4_i);
}
FCode_XE (p4_i_execution)
{
    FX_USE_CODE_ADDR;
    FX_PUSH_SP = FX_RP[0] + FX_RP[1];
    FX_USE_CODE_EXIT;
}
P4COMPILES (p4_i, p4_i_execution,
	    P4_SKIPS_NOTHING, P4_DEFAULT_STYLE);

/** "((IF))" ( -- ) [HIDDEN]
 * execution word compiled by => IF - just some simple => ?BRANCH
 */
FCode_XE (p4_if_execution)
{
    FX_USE_CODE_ADDR;
    if (!*SP++)
        FX_BRANCH;
    else
        IP++;
    FX_USE_CODE_EXIT;
}

/** IF ( value -- ) [ANS]
 * checks the value on the stack (at run-time, not compile-time)
 * and if true executes the code-piece between => IF and the next
 * => ELSE or => THEN . Otherwise it has compiled a branch over
 * to be executed if the value on stack had been null at run-time.
 */
FCode (p4_if)
{
    FX_COMPILE (p4_if);
    FX (p4_ahead);
}
P4COMPILES (p4_if, p4_if_execution,
  P4_SKIPS_OFFSET, P4_IF_STYLE);

/** IMMEDIATE ( -- ) [ANS]
 * make the => LATEST word immediate, see also => CREATE
 */
FCode (p4_immediate)
{
    if (LAST)
        P4_NFA_FLAGS(LAST) |= P4xIMMEDIATE;
    else
        p4_throw (P4_ON_ARG_TYPE);
}

/** INVERT ( value# -- value#' ) [ANS]
 * make a bitwise negation of the value on stack.
 * see also => NEGATE
 */
FCode (p4_invert)
{
    *SP = ~*SP;
}

/** J ( R: some,loop -- S: j# ) [ANS]
 * get the current => DO ... => LOOP index-value being
 * the not-innnermost. (the second-innermost...)
 * see also for the other loop-index-values at
 * => I and => K
 */
FCode (p4_j)
{
    FX_COMPILE (p4_j);
}
FCode_XE (p4_j_execution)
{
    FX_USE_CODE_ADDR;
    FX_PUSH_SP = FX_RP[3] + FX_RP[4];
    FX_USE_CODE_EXIT;
}
P4COMPILES (p4_j, p4_j_execution,
	    P4_SKIPS_NOTHING, P4_DEFAULT_STYLE);

/** KEY ( -- char# ) [ANS]
 * return a single character from the keyboard - the
 * key is not echoed.
 */
FCode (p4_key)
{
    PFE.execute (PFE.key);
}

/** LEAVE ( R: some,loop -- R: some,loop ) [ANS]
 * quit the innermost => DO .. => LOOP  - it does even
 * clean the return-stack and branches to the place directly
 * after the next => LOOP
 */
FCode (p4_leave)
{
    FX_COMPILE (p4_leave);
}
FCode_XE (p4_leave_execution)
{
    FX_USE_CODE_ADDR;
#  if   ! defined  PFE_SBR_CALL_THREADING
    IP = RP[2] - 1; /* the place after the next LOOP */
    RP += 3;        /* UNLOOP */
    FX_BRANCH;
#  else
    FX_NEW_RP_WORK;
    FX_NEW_RETVAL = FX_NEW_RP[2][-1];
    FX_NEW_RP_DROP (3);
    FX_NEW_RP_DONE;
    FX_NEW_RP_EXIT;
#  endif
    FX_USE_CODE_EXIT;
}
P4COMPILES (p4_leave, p4_leave_execution,
	    P4_SKIPS_NOTHING, P4_DEFAULT_STYLE);

/** "((LIT))" ( -- value ) [HIDDEN]
 * execution compiled by => LITERAL
 */ 
FCode_XE (p4_literal_execution)
{
    FX_USE_CODE_ADDR;
    FX_PUSH_SP = P4_POP (IP);
    FX_USE_CODE_EXIT;
}

/** LITERAL ( C: value -- S: value ) [ANS]
 * if compiling this will take the value from the compiling-stack 
 * and puts in dictionary so that it will pop up again at the
 * run-time of the word currently in creation. This word is used
 * in compiling words but may also be useful in making a hard-constant
 * value in some code-piece like this:
 : DCELLS [ 2 CELLS ] LITERAL * ; ( will save a multiplication at runtime)
 * (in most configurations this word is statesmart and it will do nothing
 *  in interpret-mode. See =>"LITERAL," for a non-immediate variant)
 */
FCode (p4_literal)
{
    _FX_STATESMART_Q_COMP;
    if (STATESMART)
    {
        FX_COMPILE (p4_literal);
        FX_SCOMMA (*SP++);
    }
}
P4COMPILES (p4_literal, p4_literal_execution,
  P4_SKIPS_CELL, P4_DEFAULT_STYLE);

/** "((LOOP))" ( -- ) [HIDDEN]
 * execution compiled by => LOOP
 */
FCode_XE (p4_loop_execution)
{
    FX_USE_CODE_ADDR;
#  ifndef PFE_SBR_CALL_THREADING
    if (++*FX_RP)                       /* increment top of return stack */
        IP = RP[2];                     /* if nonzero: loop back */
    else
    {
        FX_RP_DROP (3);             /* if zero: terminate loop */
        FX_RP_EXIT;
    }
#  else
    if (++*FX_RP)
    {
	FX_NEW_RP_WORK;
	FX_NEW_RETVAL = FX_NEW_RP [2]; 
	FX_NEW_RP_DONE;
        FX_NEW_RP_EXIT;
    }else{
        FX_RP_DROP (3);
        FX_RP_EXIT;
    }
#  endif
    FX_USE_CODE_EXIT;
}

/** LOOP ( R: some,loop -- ) [ANS] [REPEAT]
 * resolves a previous => DO thereby compiling => ((LOOP)) which
 * does increment/decrement the index-value and branch back if
 * the end-value of the loop has not been reached.
 */
FCode (p4_loop)
{
    p4_Q_pairs (P4_LOOP_MAGIC);
    FX_COMPILE (p4_loop);
    FX (p4_forward_resolve);
}
P4COMPILES (p4_loop, p4_loop_execution,
  P4_SKIPS_OFFSET, P4_LOOP_STYLE);

/** LSHIFT ( value# shift-count -- value#' ) [ANS]
 * does a bitwise left-shift on value
 */
FCode (p4_l_shift)
{
    SP[1] <<= SP[0];
    SP++;
}

/** M* ( a# b# -- a,a#' ) [ANS]
 * multiply and return a double-cell result
 */
FCode (p4_m_star)
{
    *(p4dcell *) SP = mmul (SP[0], SP[1]);
}

/** MAX ( a# b# -- a#|b# | a* b* -- a*|b* | a b -- a|b [??] ) [ANS]
 * return the maximum of a and b
 */
FCode (p4_max)
{
    if (SP[0] > SP[1])
        SP[1] = SP[0];
    SP++;
}

/** MIN ( a# b# -- a#|b# | a* b* -- a*|b* | a b -- a|b [??] ) [ANS]
 * return the minimum of a and b
 */
FCode (p4_min)
{
    if (SP[0] < SP[1])
        SP[1] = SP[0];
    SP++;
}

/** MOD ( a# b# -- mod-a# | a b# -- mod-a# [??] ) [ANS]
 * return the module of "a mod b"
 */
FCode (p4_mod)
{
    fdiv_t res = p4_fdiv (SP[1], SP[0]);
    
    *++SP = res.rem;
}

/** MOVE ( from-ptr to-ptr move-len -- ) [ANS]
 * p4_memcpy an area
 */
FCode (p4_move)
{
    p4_memmove ((void *) SP[1], (void *) SP[2], (size_t) SP[0]);
    SP += 3;
}

/** NEGATE ( value# -- value#' ) [ANS]
 * return the arithmetic negative of the (signed) cell
 simulate:   : NEGATE -1 * ;
 */
FCode (p4_negate)
{
    *SP = -*SP;
}

/** OR ( a b# -- a' | a# b -- b' | a b -- a' [??] ) [ANS]
 * return the bitwise OR of a and b - unlike => AND this
 * is usually safe to use on logical values
 */
FCode (p4_or)
{
    SP[1] |= SP[0];
    SP++;
}

/** OVER ( a b -- a b a ) [ANS]
 * get the value from under the top of stack. The inverse
 * operation would be => TUCK
 */
FCode (p4_over)
{
    --SP;
    SP[0] = SP[2];
}

/** "((POSTPONE))" ( -- ) [HIDDEN]
 * execution compiled by => POSTPONE
 */ 
FCode_XE (p4_postpone_execution)
{
    FX_USE_CODE_ADDR;
    FX_COMPILE_COMMA((p4xt)( P4_POP (IP) ));
    FX_USE_CODE_EXIT;
}

/** POSTPONE ( [word] -- ) [ANS]
 * will compile the following word at the run-time of the
 * current-word which is a compiling-word. The point is that
 * => POSTPONE takes care of the fact that word may be 
 * an IMMEDIATE-word that flags for a compiling word, so it
 * must be executed (and not pushed directly) to compile
 * sth. later. Choose this word in favour of => COMPILE
 * (for non-immediate words) and => [COMPILE] (for immediate
 * words)
 */
FCode (p4_postpone)
{
    p4char* nfa;

    FX (p4_Q_comp);
    nfa = p4_tick_nfa (FX_VOID);
# ifndef PFE_CALL_THREADING
    if (! P4_NFA_xIMMEDIATE (nfa))
        FX_COMPILE (p4_postpone);
    FX_XCOMMA (p4_name_from (nfa)); /* a.k.a. FX_COMPILE_COMMA */
# else
    if (! P4_NFA_xIMMEDIATE (nfa))
    {
	FX_COMPILE (p4_postpone);
	FX_XCOMMA (p4_name_from(nfa));
    }else{
	FX_COMPILE_COMMA (p4_name_from(nfa));
    }
# endif
}
P4COMPILES (p4_postpone, p4_postpone_execution,
          P4_SKIPS_NOTHING, P4_DEFAULT_STYLE);

/** QUIT ( -- ) [EXIT]
 * this will throw and lead back to the outer-interpreter.
 * traditionally, the outer-interpreter is called QUIT
 * in forth itself where the first part of the QUIT-word
 * had been to clean the stacks (and some other variables)
 * and then turn to an endless loop containing => QUERY 
 * and => EVALUATE (otherwise known as => INTERPRET )
 * - in pfe it is defined as a => THROW ,
 : QUIT -56 THROW ;
 */
FCode (p4_quit)
{
    p4_throw (P4_ON_QUIT);
}

/** R> ( R: a -- a R: ) [ANS]
 * get back a value from the return-stack that had been saved
 * there using => >R . This is the traditional form of a local
 * var space that could be accessed with => R@ later. If you
 * need more local variables you should have a look at => LOCALS|
 * which does grab some space from the return-stack too, but names
 * them the way you like.
 */
FCode (p4_r_from)
{
    FX (p4_Q_comp);
    FX_COMPILE (p4_r_from);
}
FCode_XE (p4_r_from_execution)
{
    FX_USE_CODE_ADDR;
#  if !defined PFE_SBR_CALL_THREADING
    FX_PUSH_SP = (p4cell) FX_POP_RP;
#  else
    FX_PUSH_SP = (p4cell) RP[0];
    FX_RP_DROP (1);
    FX_RP_EXIT;
#  endif
    FX_USE_CODE_EXIT;
}
P4COMPILES (p4_r_from, p4_r_from_execution, 
	    P4_SKIPS_NOTHING, P4_DEFAULT_STYLE);

/** R@ ( R: a -- a R: a ) [ANS]
 * fetch the (upper-most) value from the return-stack that had
 * been saved there using =>">R" - This is the traditional form of a local
 * var space. If you need more local variables you should have a 
 * look at => LOCALS| , see also =>">R" and =>"R>" . Without LOCALS-EXT
 * there are useful words like =>"2R@" =>"R'@" =>'R"@' =>'R!' 
 */
FCode (p4_r_fetch)
{
    FX (p4_Q_comp);
    FX_COMPILE (p4_r_fetch);
}
FCode_XE (p4_r_fetch_execution)
{
    FX_USE_CODE_ADDR;
    FX_PUSH_SP = *FX_RP;
    FX_USE_CODE_EXIT;
}
P4COMPILES (p4_r_fetch, p4_r_fetch_execution,
	    P4_SKIPS_NOTHING, P4_DEFAULT_STYLE);

/** RECURSE ( ? -- ? ) [ANS]
 * when creating a colon word the name of the currently-created
 * word is smudged, so that you can redefine a previous word
 * of the same name simply by using its name. Sometimes however
 * one wants to recurse into the current definition instead of
 * calling the older defintion. The => RECURSE word does it 
 * exactly this.
   traditionally the following code had been in use:
   : GREAT-WORD [ UNSMUDGE ] DUP . 1- ?DUP IF GREAT-WORD THEN ;
   now use
   : GREAT-WORD DUP . 1- ?DUP IF RECURSE THEN ;
 */
FCode (p4_recurse)
{
    FX (p4_Q_comp);
    if (LAST)
        FX_COMPILE_COMMA (p4_name_from (LAST));
    else
        p4_throw (P4_ON_ARG_TYPE);
}

/** REPEAT ( -- ) [ANS] [REPEAT]
 * ends an unconditional loop, see => BEGIN
 */
FCode (p4_repeat)
{
    p4_Q_pairs (P4_DEST_MAGIC);
    FX_COMPILE (p4_repeat);
    FX (p4_backward_resolve);
    p4_Q_pairs (P4_ORIG_MAGIC);
    FX (p4_forward_resolve);
}
P4COMPILES (p4_repeat, p4_else_execution,
  P4_SKIPS_OFFSET, P4_REPEAT_STYLE);

/** ROT ( a b c -- b c a ) [ANS]
 * rotates the three uppermost values on the stack,
 * the other direction would be with => -ROT - please
 * have a look at => LOCALS| and => VAR that can avoid 
 * its use.
 */
FCode (p4_rot)
{
    p4cell h = SP[2];

    SP[2] = SP[1];
    SP[1] = SP[0];
    SP[0] = h;
}

/** RSHIFT ( value# shift-count# -- value#' ) [ANS]
 * does a bitwise logical right-shift on value
 * (ie. the value is considered to be unsigned)
 */
FCode (p4_r_shift)
{
    *(p4ucell *) &SP[1] >>= SP[0];
    SP++;
}

/** '((S"))' ( -- string-ptr string-len ) [HIDDEN]
 * execution compiled by => S"
 */ 
FCode_XE (p4_s_quote_execution)
{   FX_USE_CODE_ADDR {
#  ifndef PFE_SBR_CALL_THREADING
    p4char *p = (p4char *) IP;
    
    SP -= 2;
    SP[0] = *p;
    SP[1] = (p4cell) (p + 1);
    FX_SKIP_STRING;
#  else
    FX_NEW_IP_WORK;
    SP -= 2;
    SP[0] = *FX_NEW_IP_CHAR;
    SP[1] = (p4cell) (FX_NEW_IP_CHAR + 1);
    FX_NEW_IP_SKIP_STRING;
    FX_NEW_IP_DONE;
#  endif
    FX_USE_CODE_EXIT;
}}

/** 'S"' ( [string<">] -- string-ptr string-len) [ANS]
 * if compiling then place the string into the currently
 * compiled word and on execution the string pops up
 * again as a double-cell value yielding the string's address
 * and length. To be most portable this is the word to be
 * best being used. Compare with =>'C"' and non-portable => "
 */
FCode (p4_s_quote)
{
    if (STATE) /* 'S"' is always STATESMART (required by FILE-EXT) */
    {
        FX_COMPILE (p4_s_quote);
        FX (p4_parse_comma_quote);
    }else{
        register p4char *p;
        register p4ucell n;

        p = p4_pocket ();
        p4_word_parse ('"'); /* PARSE - no throw HERE */
	n = PFE.word.len;
        if (n > P4_POCKET_SIZE-1)
            n = P4_POCKET_SIZE-1;
        *p++ = n;
        p4_memcpy (p, PFE.word.ptr, n);
        FX_PUSH(p);
        FX_PUSH(n);
    }
}
P4COMPILES (p4_s_quote, p4_s_quote_execution,
  P4_SKIPS_STRING, P4_DEFAULT_STYLE);

/** S>D ( a# -- a,a#' | a -- a,a#' [??] ) [ANS]
 * signed extension of a single-cell value to a double-cell value
 */
FCode (p4_s_to_d)
{
    SP--;
    SP[0] = SP[1] < 0 ? -1 : 0;
}

/** SIGN ( a# -- ) [ANS]
 * put the sign of the value into the hold-space, this is
 * the forth-style output formatting, see => HOLD
 */
FCode (p4_sign)
{
    if (*SP++ < 0)
        p4_hold ('-');
}

/** SM/REM ( a,a# b# -- div-a# rem-a# ) [ANS]
 * see => /MOD or => FM/MOD or => UM/MOD or => SM/REM
 */
FCode (p4_s_m_slash_rem)
{
    p4cell denom = *SP++;

    *(fdiv_t *) SP = p4_d_smdiv (*(p4dcell *) SP, denom);
}

/** SOURCE ( -- buffer* IN-offset# ) [ANS]
 *  the current point of interpret can be gotten through SOURCE.
 *  The buffer may flag out TIB or BLK or a FILE and IN gives
 *  you the offset therein. Traditionally, if the current SOURCE
 *  buffer is used up, => REFILL is called that asks for another
 *  input-line or input-block. This scheme would have made it
 *  impossible to stretch an [IF] ... [THEN] over different blocks,
 *  unless [IF] does call => REFILL
 */
FCode (p4_source)
{
    const p4_char_t *p;
    int in;

    p4_source (&p, &in);
    SP -= 2;
    SP[1] = (p4cell) p;
    SP[0] = in;
}

/** SPACE ( -- ) [ANS]
 * print a single space to stdout, see => SPACES
 simulate:    : SPACE  BL EMIT ;
 */
FCode (p4_space)
{
    p4_outc (' ');
}

/** SPACES ( space-count -- ) [ANS]
 * print n space to stdout, actually a loop over n calling => SPACE ,
 * but the implemenation may take advantage of printing chunks of
 * spaces to speed up the operation.
 */
FCode (p4_spaces)
{
    p4_emits (*SP++, ' ');
}

/** SWAP ( a b -- b a ) [ANS]
 * exchanges the value on top of the stack with the value beneath it
 */
FCode (p4_swap)
{
    p4cell h = SP[1];

    SP[1] = SP[0];
    SP[0] = h;
}

/** THEN ( -- ) [ANS]
 * does resolve a branch coming from either => IF or => ELSE
 */
FCode (p4_then)
{
    FX_COMPILE (p4_then);
    p4_Q_pairs (P4_ORIG_MAGIC);
    FX (p4_forward_resolve);
}
P4COMPILES (p4_then, p4_noop, P4_SKIPS_NOTHING, P4_THEN_STYLE);

/** TYPE ( string-ptr string-len -- ) [ANS]
 * prints the string-buffer to stdout, see => COUNT and => EMIT
 */
FCode (p4_type)
{
    PFE.execute (PFE.type);
}

/** U. ( value# -- | value -- [?] ) [ANS]
 * print unsigned number to stdout
 */
FCode (p4_u_dot)
{
    FX_PUSH( 0 );
    FX (p4_d_dot);
}

/** U< ( a b -- test-flag ) [ANS]
 * unsigned comparison, see => <
 */
FCode (p4_u_less_than)
{
    SP[1] = P4_FLAG ((p4ucell) SP[1] < (p4ucell) SP[0]);
    SP++;
}

/** UM* ( a# b# -- a,a#' ) [ANS]
 * unsigned multiply returning double-cell value
 */
FCode (p4_u_m_star)
{
    *(p4udcell *) SP = p4_d_ummul ((p4ucell) SP[0], (p4ucell) SP[1]);
}

/** "UM/MOD" ( a,a# b# -- div-a#' mod-a#' ) [ANS]
 * see => /MOD and => SM/REM
 */
FCode (p4_u_m_slash_mod)
{
    p4ucell denom = (p4ucell) *SP++;

    *(udiv_t *) SP = p4_d_umdiv (*(p4udcell *) SP, denom);
}

/** UNLOOP ( R: some,loop -- ) [ANS]
 * drop the => DO .. => LOOP runtime variables from the return-stack,
 * usually used just in before an => EXIT call. Using this multiple
 * times can unnest multiple nested loops.
 */
FCode (p4_unloop)
{
    FX_COMPILE (p4_unloop);
}
FCode_XE (p4_unloop_execution)
{
    FX_USE_CODE_ADDR;
    FX_RP_DROP (3);
    FX_RP_EXIT;
    FX_USE_CODE_EXIT;
}
P4COMPILES (p4_unloop, p4_unloop_execution,
	    P4_SKIPS_NOTHING, P4_DEFAULT_STYLE);

/** UNTIL ( test-flag -- ) [ANS] [REPEAT]
 * ends an control-loop, see => BEGIN and compare with => WHILE
 */
FCode (p4_until)
{
    p4_Q_pairs (P4_DEST_MAGIC);
    FX_COMPILE (p4_until);
    FX (p4_backward_resolve);
}
P4COMPILES (p4_until, p4_if_execution,
	    P4_SKIPS_OFFSET, P4_UNTIL_STYLE);

/** VARIABLE ( 'name' -- ) [ANS] [DOES: -- name* ]
 * => CREATE a new variable, so that everytime the variable is
 * name, the address is returned for using with => @ and => !
 * - be aware that in FIG-forth VARIABLE did take an argument
 * being the initial value. ANSI-forth does different here.
 */
FCode (p4_variable)
{
    FX_RUNTIME_HEADER;
    FX_RUNTIME1(p4_variable);
    FX_VCOMMA (0);
}
P4RUNTIME1(p4_variable, p4_variable_RT);

/** WHILE ( test-flag -- ) [ANS]
 * middle part of a => BEGIN .. => WHILE .. => REPEAT 
 * control-loop - if cond is true the code-piece up to => REPEAT
 * is executed which will then jump back to => BEGIN - and if
 * the cond is null then => WHILE will branch to right after
 * the => REPEAT
 * (compare with => UNTIL that forms a => BEGIN .. => UNTIL loop)
 */
FCode (p4_while)
{
    p4_Q_pairs (P4_DEST_MAGIC);
    FX_PUSH_SP = P4_DEST_MAGIC;
    FX_COMPILE (p4_while);
    FX (p4_ahead);
    FX (p4_two_swap);
}
P4COMPILES (p4_while, p4_if_execution,
	    P4_SKIPS_OFFSET, P4_WHILE_STYLE);

/** WORD ( delimiter-char# -- here* ) [ANS]
 * read the next => SOURCE section (thereby moving => >IN ) up
 * to the point reaching $delimiter-char - the text is placed
 * at => HERE - where you will find a counted string. You may
 * want to use => PARSE instead.
 */
FCode (p4_word)
{
    *SP = (p4cell) p4_word ((char) *SP);
}

/** XOR ( a# b# -- a#' ) [ANS]
 * return the bitwise-or of the two arguments - it may be unsafe
 * use it on logical values. beware.
 */
FCode (p4_xor)
{
    SP[1] ^= SP[0];
    SP++;
}

/** [ ( -- ) [ANS]
 * leave compiling mode - often used inside of a colon-definition
 * to make fetch some very constant value and place it into the
 * currently compiled colon-defintion with => , or => LITERAL
 * - the corresponding unleave word is => ]
 */
FCode (p4_left_bracket)
{
    FX (p4_Q_comp);
    STATE = P4_FALSE;
}

/** ['] ( [name] -- name-xt* ) [ANS] 
 * will place the execution token of the following word into
 * the dictionary. See => ' for non-compiling variant.
 */
FCode (p4_bracket_tick)
{
    FX (p4_tick);

    _FX_STATESMART_Q_COMP;
    if (STATESMART)
    {
        FX_COMPILE (p4_bracket_tick);
        FX (p4_comma);
    }
}
P4COMPILES (p4_bracket_tick, p4_literal_execution,
	    P4_SKIPS_NOTHING, P4_DEFAULT_STYLE);

/** [CHAR] ( [word] -- char# ) [ANS]
 * in compile-mode, get the (ascii-)value of the first charachter
 * in the following word and compile it as a literal so that it
 * will pop up on execution again. See => CHAR and forth-83 => ASCII
 */
FCode (p4_bracket_char)
{
    FX (p4_char);

    _FX_STATESMART_Q_COMP;
    if (STATESMART)
    {
        FX_COMPILE (p4_bracket_char);
        FX (p4_comma);
    }
}
P4COMPILES (p4_bracket_char, p4_literal_execution,
	    P4_SKIPS_CELL, P4_DEFAULT_STYLE);

/** ] ( -- ) [ANS]
 * enter compiling mode - often used inside of a colon-definition
 * to end a previous => [ - you may find a  => , or => LITERAL
 * nearby in example texts.
 */
FCode (p4_right_bracket)
{
    STATE = P4_TRUE;
}

/************************************************************************/
/* Core Extension Words                                                 */
/************************************************************************/

/** ".(" ( [message<closeparen>] -- ) [ANS]
 * print the message to the screen while reading a file. This works
 * too while compiling, so you can whatch the interpretation/compilation
 * to go on. Some Forth-implementations won't even accept a => ." message"
 * outside compile-mode while the (current) pfe does.
 */
FCode (p4_dot_paren)
{
    switch (SOURCE_ID)
    {
     case -1:
     case 0:
         p4_word_parse (')'); /* PARSE-NOHERE-NOTHROW */
         p4_type (PFE.word.ptr, PFE.word.len);
         break;
     default:
         while (! p4_word_parse (')')) /* PARSE-NOHERE-NOTHROW */
         {
             p4_type (PFE.word.ptr, PFE.word.len);
             if (! p4_refill ())
                 return;
             FX (p4_cr);
         }
         p4_type (PFE.word.ptr, PFE.word.len);
    }
}

/** .R ( value# precision# -- | value precision# -- [??] ) [ANS]
 * print with precision - that is to fill
 * a field of the give prec-with with 
 * right-aligned number from the converted value
 */
FCode (p4_dot_r)
{
    register p4cell tmp = FX_POP;
    FX (p4_s_to_d);
    FX_PUSH(tmp);
    FX (p4_d_dot_r);
}

/** "0<>" ( 0 -- 0 | value! -- value-flag! | value -- value-flag ) [ANS]
 * returns a logical-value saying if the value was not-zero.
 * This is most useful in turning a numerical value into a 
 * boolean value that can be fed into bitwise words like
 * => AND and => XOR - a simple => IF or => WHILE doesn't
 * need it actually.
 */
FCode (p4_zero_not_equals)
{
    *SP = P4_FLAG (*SP != 0);
}

/** 0> ( 0 -- 0 | value! -- value-flag! | value -- value-flag ) [ANS]
 * return value greater than zero
 simulate:    : 0> 0 > ;
 */
FCode (p4_zero_greater)
{
    *SP = P4_FLAG (*SP > 0);
}

/** 2>R ( a,a -- R: a,a ) [ANS]
 * save a double-cell value onto the return-stack, see => >R
 */
FCode (p4_two_to_r)
{
    FX (p4_Q_comp);
    FX_COMPILE (p4_two_to_r);
}
FCode_XE (p4_two_to_r_execution)
{
    FX_USE_CODE_ADDR;
#  if !defined PFE_SBR_CALL_THREADING
    RP_PUSH (SP[1]);
    RP_PUSH (SP[0]);
    FX_2DROP;
#  else
    FX_NEW_RP_WORK;
    FX_NEW_RP_ROOM (2);
    FX_NEW_RP_AT(0, SP[0]);
    FX_NEW_RP_AT(1, SP[1]);
    FX_2DROP;
    FX_NEW_RP_DONE;
    FX_NEW_RP_EXIT;
#  endif
    FX_USE_CODE_EXIT;
}
P4COMPILES(p4_two_to_r, p4_two_to_r_execution, 
	   P4_SKIPS_NOTHING, P4_DEFAULT_STYLE);

/** 2R> ( R: a,a -- a,a R: ) [ANS]
 * pop back a double-cell value from the return-stack, see => R>
 * and the earlier used => 2>R
 */
FCode (p4_two_r_from)
{
    FX (p4_Q_comp);
    FX_COMPILE (p4_two_r_from);
}
FCode_XE (p4_two_r_from_execution)
{
    FX_USE_CODE_ADDR;
#  if !defined PFE_SBR_CALL_THREADING
    FX_2ROOM;
    SP[0] = RP_POP ();
    SP[1] = RP_POP ();
#  else
    FX_2ROOM;
    SP[0] = (p4cell) RP[0];
    SP[1] = (p4cell) RP[1];
    FX_RP_DROP(2);
    FX_RP_EXIT;
#  endif
    FX_USE_CODE_EXIT;
}
P4COMPILES (p4_two_r_from, p4_two_r_from_execution,
	    P4_SKIPS_NOTHING, P4_DEFAULT_STYLE);

/** 2R@ ( R: a,a -- a,a R: a,a ) [ANS]
 * fetch a double-cell value from the return-stack, that had been
 * previously been put there with =>"2>R" - see =>"R@" for single value.
 * This can partly be a two-cell => LOCALS| value,  without LOCALS-EXT
 * there are alos other useful words like =>"2R!" =>"R'@" =>'R"@'
 */
FCode (p4_two_r_fetch)
{
    FX (p4_Q_comp);
    FX_COMPILE (p4_two_r_fetch);
}
FCode_XE (p4_two_r_fetch_execution)
{
    FX_USE_CODE_ADDR;
    SP -= 2;
    SP[0] = FX_RP[0];
    SP[1] = FX_RP[1];
    FX_USE_CODE_EXIT;
}
P4COMPILES (p4_two_r_fetch, p4_two_r_fetch_execution,
	    P4_SKIPS_NOTHING, P4_DEFAULT_STYLE);

/** "(NONAME)" ( -- ) [HIDDEN]
 * compiled by => :NONAME
 * (see also => (NEST) compiled by => : (execution is identical))
 */
FCode_RT (p4_colon_noname_RT)
{
    FX_USE_BODY_ADDR;
#  ifndef PFE_CALL_THREADING
    FX_PUSH_RP = IP;
    IP = (p4xcode *) FX_POP_BODY_ADDR;
#  else
    FX_POP_BODY_ADDR_UNUSED;
    // FIXME: !!
#  endif
}

#define P4_NONAME_MAGIC P4_MAGIC_('N','N','A','M')
FCode (p4_colon_noname_EXIT)
{
    p4_Q_pairs(P4_NONAME_MAGIC);
    PFE.semicolon_code = (void*) FX_POP;
    PFE.locals = (void*) FX_POP; 
    PFE.state = FX_POP;
    /* leave pointer to colon_RT */
}

/** :NONAME ( -- C: noname,word ) [ANS]
 * start a colon nested-word but do not use => CREATE - so no name
 * is given to the colon-definition that follows. When the definition
 * is finished at the corresponding => ; the start-address (ie.
 * the execution token) can be found on the outer cs.stack that may
 * be stored used elsewhere then.
 */
FCode (p4_colon_noname)
{
    FX (p4_Q_exec);
    FX (p4_align);
    FX_PUSH (PFE.dp);
    FX_RUNTIME1 (p4_colon_noname);
    FX_PUSH (PFE.state);  PFE.state = P4_TRUE; 
    FX_PUSH (PFE.locals); PFE.locals = NULL;
    FX_PUSH (PFE.semicolon_code); 
    PFE.semicolon_code = PFX(p4_colon_noname_EXIT);
    FX_PUSH (P4_NONAME_MAGIC);
}
P4RUNTIME1(p4_colon_noname, p4_colon_noname_RT);

/** "<>" ( a b -- a-flag ) [ANS]
 * return true if a and b are not equal, see => = 
 */
FCode (p4_not_equals)
{
    SP[1] = P4_FLAG (SP[0] != SP[1]);
    SP++;
}

/** "((?DO))" ( a b -- ) [HIDDEN]
 * execution compiled by => ?DO
 */
FCode_XE (p4_Q_do_execution)
{
    FX_USE_CODE_ADDR;
    if (SP[0] == SP[1])         /* if limits are equal */
    {   SP += 2; FX_BRANCH; }   /* drop them and branch */
    else
    {   FX (p4_do_execution); }  /* else like DO */
    FX_USE_CODE_EXIT;
}

/** ?DO ( end# start# | end* start* -- R: some,loop ) [ANS]
 * start a control-loop just like => DO - but don't execute
 * atleast once. Instead jump over the code-piece if the loop's
 * variables are not in a range to allow any loop.
 */
FCode (p4_Q_do)
{
    FX_COMPILE (p4_Q_do);
    FX (p4_forward_mark);
    FX_PUSH (P4_LOOP_MAGIC);
}
P4COMPILES (p4_Q_do, p4_Q_do_execution,
  P4_SKIPS_OFFSET, P4_DO_STYLE);

/** AGAIN ( -- ) [ANS] [REPEAT]
 * ends an infinite loop, see => BEGIN and compare with
 * => WHILE
 */
FCode (p4_again)
{
    p4_Q_pairs (P4_DEST_MAGIC);
    FX_COMPILE (p4_again);
    FX (p4_backward_resolve);
}
P4COMPILES (p4_again, p4_else_execution,
          P4_SKIPS_OFFSET, P4_AGAIN_STYLE);

/** '((C"))' ( -- string-bstr* ) [HIDDEN]
 * execution compiled by => C" string"
 */ 
FCode_XE (p4_c_quote_execution)
{
    FX_USE_CODE_ADDR;
#  ifndef PFE_SBR_CALL_THREADING
    FX_PUSH_SP = (p4cell) IP;
    FX_SKIP_STRING;
#  else
    FX_NEW_IP_WORK;
    FX_PUSH_SP = (p4cell) FX_NEW_IP_CODE;
    FX_NEW_IP_SKIP_STRING;
    FX_NEW_IP_DONE;
#  endif
    FX_USE_CODE_EXIT;
}

/** 'C"' ( [string<">] -- string-bstr* ) [ANS]
 * in compiling mode place the following string in the current
 * word and return the address of the counted string on execution.
 * (in exec-mode use a => POCKET and leave the bstring-address of it),
 * see => S" string" and the non-portable => " string"
 */
FCode (p4_c_quote)
{
    _FX_STATESMART_Q_COMP;
    if (STATESMART)
    {
        FX_COMPILE (p4_c_quote);
        FX (p4_parse_comma_quote);
    }else{
        FX (p4_s_quote); /* we know s_quote uses a counted string internally */
        FX_DROP;
        SP[0]--;
    }
}
P4COMPILES (p4_c_quote, p4_c_quote_execution,
  P4_SKIPS_STRING, P4_DEFAULT_STYLE);

#ifndef PFE_CALL_THREADING
#define p4_case_RT p4_noop
#else
static FCode_RT(p4_case_RT) { /*nothing*/ }
#endif

/** CASE ( value -- value ) [ANS]
 * start a CASE construct that ends at => ENDCASE
 * and compares the value on stack at each => OF place
 */
FCode (p4_case)
{
    FX_COMPILE (p4_case);
    FX_PUSH (CSP);
    CSP = SP;
    FX_PUSH (P4_CASE_MAGIC);
}
P4COMPILES (p4_case, p4_case_RT, P4_SKIPS_NOTHING, P4_CASE_STYLE);

/** "COMPILE," ( some-xt* -- ) [ANS]
 * place the execution-token on stack into the dictionary - in
 * traditional forth this is not even the least different than
 * a simple => , but in call-threaded code there's a big 
 * difference - so COMPILE, is the portable one. Unlike 
 * => COMPILE , => [COMPILE] and => POSTPONE this word does
 * not need the xt to have actually a name, see => :NONAME
 */
FCode (p4_compile_comma)
{
    FX_COMPILE_COMMA ((p4xt)( *SP++ ));
}

/** CONVERT ( a,a# string-bstr* -- a,a# a-len# ) [ANS] [OLD]
 * digit conversion, obsolete, superseded by => >NUMBER
 */
FCode (p4_convert)
{
    p4ucell n = UINT_MAX;

    SP[0] = (p4cell) 
        p4_to_number ((p4_char_t *) SP[0] + 1, &n, 
          (p4udcell *) &SP[1], BASE);
}

#ifndef PFE_CALL_THREADING
#define p4_endcase_RT p4_drop
#else
static FCode_RT(p4_endcase_RT) { FX (p4_drop); }
#endif

/** ENDCASE ( value -- ) [ANS]
 * ends a => CASE construct that may surround multiple sections of
 * => OF ... => ENDOF code-portions. The => ENDCASE has to resolve the
 * branches that are necessary at each => ENDOF to point to right after
 * => ENDCASE
 */
FCode (p4_endcase)
{
    p4_Q_pairs (P4_CASE_MAGIC);
    FX_COMPILE (p4_endcase);
    while (SP < CSP)
        FX (p4_forward_resolve);
    CSP = (p4cell *) FX_POP;
}
P4COMPILES (p4_endcase, p4_endcase_RT,
  P4_SKIPS_NOTHING, P4_ENDCASE_STYLE);

/** ENDOF ( -- ) [ANS]
 * resolve the branch need at the previous => OF to mark
 * a code-piece and leave with an unconditional branch
 * at the next => ENDCASE (opened by => CASE )
 */
FCode (p4_endof)
{
    p4_Q_pairs (P4_OF_MAGIC);
    FX_COMPILE (p4_endof);
    FX (p4_forward_mark);
    FX (p4_swap);
    FX (p4_forward_resolve);
    FX_PUSH (P4_CASE_MAGIC);
}
P4COMPILES (p4_endof, p4_else_execution,
  P4_SKIPS_OFFSET, P4_ENDOF_STYLE);

/** ERASE ( buffer-ptr buffer-len -- ) [ANS]
 * fill an area will zeros.
 2000 CREATE DUP ALLOT ERASE
 */
FCode (p4_erase)
{
    p4_memset ((void *) SP[1], 0, SP[0]);
    SP += 2;
}

/** EXPECT ( str-ptr str-len -- ) [ANS] [OLD]
 * input handling, see => WORD and => PARSE and => QUERY
 * the input string is placed at str-adr and its length
 in => SPAN - this word is superceded by => ACCEPT
 */
FCode (p4_expect)
{
    PFE.execute (PFE.expect);
}

/** HEX ( -- ) [ANS]
 * set the input/output => BASE to hexadecimal
 simulate:        : HEX 16 BASE ! ;
 */
FCode (p4_hex)
{
    BASE = 16;
}

/** MARKER ( 'name' -- ) [ANS]
 * create a named marker that you can use to => FORGET ,
 * running the created word will reset the dict/order variables
 * to the state at the creation of this name.
 : MARKER PARSE-WORD (MARKER) ;
 * see also => ANEW which is not defined in ans-forth but which uses
 * the => MARKER functionality in the way it should have been defined.
 : MARKER PARSE-WORD (MARKER) ;
 */
FCode (p4_marker)
{
    extern FCode (p4_paren_marker);
    FX (p4_parse_word);
    FX (p4_paren_marker);
}
P4RUNTIME1(p4_marker, p4_marker_RT);

/** NIP ( a b -- b ) [ANS]
 * drop the value under the top of stack, inverse of => TUCK
 simulate:        : NIP SWAP DROP ;
 */
FCode (p4_nip)
{
    SP[1] = SP[0];
    SP++;
}

/** "((OF))" ( check val -- check ) [HIDDEN]
 * execution compiled by => OF
 */
FCode_XE (p4_of_execution)
{
    FX_USE_CODE_ADDR;
    if (SP[0] != SP[1])         /* tos equals second? */
    { SP += 1; FX_BRANCH; }     /* no: drop top, branch */
    else
    { SP += 2; IP++; }          /* yes: drop both, don't branch */
    FX_USE_CODE_EXIT;
}

/** OF ( value test -- value ) [ANS]
 * compare the case-value placed lately with the comp-value 
 * being available since => CASE - if they are equal run the 
 * following code-portion up to => ENDOF after which the
 * case-construct ends at the next => ENDCASE
 */
FCode (p4_of)
{
    p4_Q_pairs (P4_CASE_MAGIC);
    FX_COMPILE (p4_of);
    FX (p4_forward_mark);
    FX_PUSH (P4_OF_MAGIC);
}
P4COMPILES (p4_of, p4_of_execution,
  P4_SKIPS_OFFSET, P4_OF_STYLE);

/** PAD ( -- pad* ) [ANS]
 * transient buffer region 
 */
FCode (p4_pad)
{
    FX_PUSH (p4_PAD);
}

/** PARSE ( delim-char# -- buffer-ptr buffer-len ) [ANS]
 * parse a piece of input (not much unlike WORD) and place
 * it into the given buffer. The difference with word is
 * also that => WORD would first skip any delim-char while
 * => PARSE does not and thus may yield that one. In a newer
 * version, => PARSE will not copy but just return the word-span
 * being seen in the input-buffer - therefore a transient space.
 */
FCode (p4_parse)
{
    FX_1ROOM;
    p4_word_parse ((p4char)(SP[1])); *DP=0; /* PARSE-NOHERE */
    SP[1] = (p4ucell) PFE.word.ptr;
    SP[0] = (p4ucell) PFE.word.len;
}

/** PARSE-WORD ( "chars" -- buffer-ptr buffer-len ) [ANS]
 * the ANS'94 standard describes this word in a comment
 * under =>"PARSE", section A.6.2.2008 - quote:
 * 
 * Skip leading spaces and parse name delimited by a space. c-addr 
 * is the address within the input buffer and u is the length of the
 * selected string. If the parse area is empty, the resulting string 
 * has a zero length. 
 *
 * If both => PARSE and => PARSE-WORD are present, the need for => WORD is 
 * largely eliminated. 
 */
FCode (p4_parse_word)
{
    FX_2ROOM;
    p4_word_parseword (' '); *DP=0; /* PARSE-WORD-NOHERE */
    SP[1] = (p4ucell) PFE.word.ptr;
    SP[0] = (p4ucell) PFE.word.len;
}

/** PICK ( value ...[n-1] n -- value ...[n-1] value ) [ANS]
 * pick the nth value from under the top of stack and push it
 * note that
   0 PICK -> DUP         1 PICK -> OVER
 */
FCode (p4_pick)         
{
    *SP = SP[*SP + 1];
}

/** REFILL ( -- refill-flag ) [ANS]
 * try to get a new input line from the => SOURCE and set
 * => >IN accordingly. Return a flag if sucessful, which is
 * always true if the current input comes from a 
 * terminal and which is always false if the current input
 * comes from => EVALUATE - and may be either if the 
 * input comes from a file
 */
FCode (p4_refill)
{
    FX_PUSH (p4_refill ());
}

/** RESTORE-INPUT ( input...[input-len] input-len -- ) [ANS]
 * inverse of => SAVE-INPUT
 */
FCode (p4_restore_input)
{
    if (*SP++ != sizeof (Iframe) / sizeof (p4cell))
        p4_throw (P4_ON_ARG_TYPE);

    SP = (p4cell *) p4_restore_input (SP);
    FX_PUSH( 0 );
}

/** ROLL ( value ...[n-1] n -- ...[n-1] value ) [ANS]
 * the extended form of => ROT
    2 ROLL -> ROT
 */
FCode (p4_roll)
{
    p4cell i = *SP++;
    p4cell h = SP[i];
    
    for (; i > 0; i--)
        SP[i] = SP[i - 1];
    SP[0] = h;
}

/** SAVE-INPUT ( -- input...[input-len] input-len ) [ANS]
 * fetch the current state of the input-channel which
 * may be restored with => RESTORE-INPUT later
 */
FCode (p4_save_input)
{
    SP = (p4cell *) p4_save_input (SP);
    FX_PUSH  (sizeof (Iframe) / sizeof (p4cell));
}

/** "((TO))" ( value -- ) [HIDDEN]
 * execution compiled by => TO
 */ 
FCode_XE (p4_to_execution)
{
    FX_USE_CODE_ADDR;
    *p4_to_body ((p4xt)(*IP++)) = *SP++;
    FX_USE_CODE_EXIT;
}

int 
p4_tick_local(p4xt* xt)
{
    int n;
    p4char* p = p4_word (' ');
    int l = *p++;
    if (PFE.locals && (n = p4_find_local(p, l)) != 0)
    {
        if (xt) *xt = 0;
        return n;
    }else{
        if ((p = p4_find (p, l)) == NULL)
            p4_throw (P4_ON_UNDEFINED);
        if (xt) *xt = p4_name_from(p);
        return 0;
    }
}

/** TO ( value [name] -- ) [ANS]
 * set the parameter field of name to the value, this is used
 * to change the value of a => VALUE and it can be also used
 * to change the value of => LOCALS|
 */
FCode (p4_to)
{
    p4xt xt;
    int n;

    if (STATE) /* "TO" is always STATESMART */
    {
        n = p4_tick_local(&xt);
        if (n)
        {
            FX_COMPILE2 (p4_to);
            FX_UCOMMA (n);
        }else{
            FX_COMPILE1 (p4_to);
            FX_XCOMMA (xt);
        }
    }else{
        xt = p4_tick_cfa (FX_VOID);
        *p4_to_body (xt) = FX_POP;
    }
}
P4COMPILES2 (p4_to, p4_to_execution, p4_to_local_execution,
           P4_SKIPS_TO_TOKEN, P4_DEFAULT_STYLE);

/** TUCK ( a b -- b a b ) [ANS]
 * shove the top-value under the value beneath. See => OVER
 * and => NIP
 simulate:    : TUCK  SWAP OVER ;
 */
FCode (p4_tuck)
{
    --SP;
    SP[0] = SP[1];
    SP[1] = SP[2];
    SP[2] = SP[0];
}

/** U.R ( value# precision# -- ) [ANS]
 * print right-aligned in a prec-field, treat value to
 * be unsigned as opposed to => .R
 */
FCode (p4_u_dot_r)
{
    FX_PUSH( 0 );
    FX (p4_swap);
    FX (p4_d_dot_r);
}

/** U> ( a b -- a-flag ) [ANS]
 * unsigned comparison of a and b, see => >
 */
FCode (p4_u_greater_than)
{
    SP[1] = P4_FLAG ((p4ucell) SP[1] > (p4ucell) SP[0]);
    SP++;
}

/** UNUSED ( -- unused-len ) [ANS]
 * return the number of cells that are left to be used
 * above => HERE
 */
FCode (p4_unused)
{
    FX_PUSH (PFE.dictlimit - DP);
}

static P4_CODE_RUN(p4_value_RT_SEE)
{
    p4_strcat (p, p4_str_dot (*P4_TO_BODY (xt), p+200, BASE));
    p4_strcat (p, "VALUE ");
    p4_strncat (p, (char*) P4_NFA_PTR(nfa), P4_NFA_LEN(nfa));
    return 0;
}

/** "((VALUE))" ( -- value ) [HIDDEN]
 * runtime compiled by => VALUE
 */ 
FCode_RT (p4_value_RT)
{
    FX_USE_BODY_ADDR;
    FX_PUSH( FX_POP_BODY_ADDR[0] );
}

/** VALUE ( value 'name' -- ) [HIDDEN] [DOES: -- value ]
 * => CREATE a word and initialize it with value. Using it
 * later will push the value back onto the stack. Compare with
 * => VARIABLE and => CONSTANT - look also for => LOCALS| and
 * => VAR
 */
FCode (p4_value)
{
    FX_RUNTIME_HEADER; 
    FX_RUNTIME1 (p4_value);
    FX_VCOMMA (*SP++);
}
P4RUNTIMES1_ (p4_value, p4_value_RT, 0,p4_value_RT_SEE);

/** WITHIN ( a# b# c# -- a-flag | a* b* c* -- a-flag ) [ANS]
 * a widely used word, returns ( b <= a && a < c ) so
 * that is very useful to check an index a of an array
 * to be within range b to c
 */
FCode (p4_within)
{
    SP[2] = P4_FLAG 
        ( (p4ucell) (SP[2] - SP[1]) <
          (p4ucell) (SP[0] - SP[1]) );
    SP += 2;
}

/** [COMPILE] ( [word] -- ) [ANS]
 * while compiling the next word will be place in the currently
 * defined word no matter if that word is immediate (like => IF )
 * - compare with => COMPILE and => POSTPONE
 */
FCode (p4_bracket_compile)
{
    FX (p4_Q_comp);
    FX (p4_tick);
    FX (p4_compile_comma);
}

/** "\\" ( [comment<eol>] -- ) [ANS]
 * eat everything up to the next end-of-line so that it is
 * getting ignored by the interpreter. 
 */
FCode (p4_backslash)
{
    switch (SOURCE_ID)
    {
     case 0:
         if (BLK)
         {
             TO_IN += 64 - TO_IN % 64;
             break;
         }
     case -1:
         TO_IN = NUMBER_TIB;
         break;
     default:
         p4_refill ();
    }
}

/** '"' ( [string<">] -- string-bstr* ) [FTH]
    or perhaps ( [string<">] -- string-ptr string-len )
 *  This is the non-portable word which is why the ANSI-committee
 *  on forth has created the two other words, namely => S" and => C" ,
 *  since each implementation (and in pfe configurable) uses another
 *  runtime behaviour. FIG-forth did return bstring which is the configure
 *  default for pfe.
 */
FCode (p4_quote)
{
#ifdef P4_C_QUOTE
    FX (p4_c_quote);      /* SEE will show C" ... " */
#else
    FX (p4_s_quote);      /* SEE will show S" ... " */
#endif
}
          
/** BL ( -- bl-char# ) [ANS]
 * a quick constant returning the blank character in host encoding,
 * in ascii that is 0x20
 */

/** TIB ( -- tib-char* ) [ANS]
 * traditional variable for forth terminal I/O system.
 * (Terminal Input Buffer)    ( => TIB => SPAN => SOURCE => BLK => REFILL )
 */

/** SOURCE ( -- source# ) [ANS]
 * traditional variable for forth terminal I/O system.
 *                            ( => TIB => SPAN => SOURCE => BLK => REFILL )
 */

/** >IN ( -- inputpoint# ) [ANS]
 * traditional variable for forth terminal I/O system.
 *                            ( => TIB => SPAN => SOURCE => BLK => REFILL )
 */

/** SPAN ( -- span-count ) [ANS]
 * traditional variable for forth terminal I/O system.
 *                            ( => TIB => SOURCE => SOURCE-ID => BLK )
 */

/** SOURCE-ID ( -- source# ) [ANS]
 * modern variable for forth terminal I/O system.
 *                            ( => TIB => SPAN =>">IN" => SOURCE => BLK )
 */

/** STATE ( -- state-flag ) [ANS]
 * traditional variable for forth outer interpreter.
 *  ( => : => [ => ] => LITERAL => COMPILE => POSTPONE => BASE => NUMBER )
 */

/** FALSE ( -- 0 ) [ANS]
 * places zero on the stack to express the code want to do a boolean
 * evaluation with this value. (=>"IF")
 */

/** TRUE ( -- true! ) [ANS]
 * places => FALSE => INVERT on the stack to express the code want to 
 * do a boolean evaluation with this value. (=>"IF") - we chose the
 * FIG79/FTH83 value of allbitsset to make it easier to do logical
 * computations using bitwise operands like => AND => OR => NOT ,
 * however ANS-Forth programs shall not assume a specific representation
 * of the truth value and handle just the cases =>"=0" and =>"<>0"
 */

/* -------------------------------------------------------------- */

/** "ENVIRONMENT STACK-CELLS" ( -- stackdepth# ) [ENVIRONMENT]
 * the number of cells allocated for the parameter stack of forth.
 * it can be set with a startup option to match different needs
 * of applications. (like =>"ENVIRONMENT RETURN-STACK-CELLS") unless
 * the parameter-stack is made in hardware (like on some special
 * forth CPUs)
 */
static FCode (p__stack_cells)
{
    FX_PUSH (PFE_set.stack_size);
}

/** "ENVIRONMENT RETURN-STACK-CELLS" ( -- stackdepth# ) [ENVIRONMENT]
 * the number of cells allocated for the return stack of forth.
 * it can be set with a startup option to match different needs
 * of applications. (like =>"ENVIRONMENT STACK-CELLS") unless
 * the return-stack is made in hardware (like on some special
 * forth CPUs) or aliased with the return-stack provided by
 * the hosting operating system for the forth process thread.
 */
static FCode (p__return_stack_cells)
{
    FX_PUSH (PFE_set.ret_stack_size);
}

/** "ENVIRONMENT /COUNTED-STRING" ( -- string-max ) [ENVIRONMENT]
 * the maximum number of chars in a counted string created with
 * words like =>"C\"" or => PLACE - in general, the counted string
 * can use all bits from the first => CHAR of the string, so it
 * expands to => 1 => /CHAR => LSHIFT => 1- being 255 for octet systems.
 */

/** "ENVIRONMENT /HOLD" ( -- hold-max ) [ENVIRONMENT]
 * traditional constant for the forth I/O system, moved into
 * the environment in ans-forth systems. 
 * ( => HLD => HOLD =>"ENVIRONMENT /PAD")
 */

/** "ENVIRONMENT /PAD" ( -- pad-max ) [ENVIRONMENT]
 * traditional constant for the forth I/O system, moved into
 * the environment in ans-forth systems. 
 * ( => HLD => HOLD =>"ENVIRONMENT /HOLD")
 */

/** "ENVIRONMENT ADDRESS-UNIT-BITS" ( -- unit-len ) [ENVIRONMENT]
 * a constant useful to retarget forth applications to unusual
 * hardware that has not 8 bits per address unit (i.e. not an
 * octet-addressable system). This includes some desktop calculators
 * and wristwatch calculators with a 4 bit address system, and
 * some special DSP cpus with 24 bit or 32 bit address units.
 * All these are deeply embedded for special purpose applications.
 */

/** "ENVIRONMENT FLOORED" ( -- true! ) [ENVIRONMENT]
 * a flag whether division/modulo is floored or not. since pfe
 * builds on top of C, it takes its style of computations in
 * all respects, please refer to the ISO C standard for further
 * interpretations.
 */

/** "ENVIRONMENT MAX-CHAR" ( -- char-max ) [ENVIRONMENT]
 * a constant to check representation limits, expands to 255 in
 * octet-char systems like PFE. 
 */

/** "ENVIRONMENT MAX-N" ( -- int-max ) [ENVIRONMENT]
 * a constant to check representation limits, here it is the
 * the largest usable signed integer. In a 16-bit two's-complement
 * system this is 32767, and likewise higher in 32-bit and 64-bit
 * versions of PFE. (like =>"ENVIRONMENT MAX-U")
   TRUE 1 RSHIFT CONSTANT MAX-N
 */

/** "ENVIRONMENT MAX-U" ( -- uint-max ) [ENVIRONMENT]
 * a constant to check representation limits, here it is the
 * the largest usable unsigned integer. In a 16-bit two's-complement
 * system this is 65535, and likewise higher in 32-bit and 64-bit
 * versions of PFE. (like =>"ENVIRONMENT MAX-N")
   TRUE CONSTANT MAX-U
 */


/* -------------------------------------------------------------- */
/*
 * a few notes: there are some synonyms in the table below that might
 * not look natural - these are synyms where the behaviour of a word
 * has changed from fig-forth to ans-forth. In these case I prefer to
 * "invent" a new word that is not ambiguous in its name between the
 * system. The names in the ans-forth sources do not need to change
 * but people should expect to see a different name being decompiled.
 * This is merely intended since I have to maintain quite a few programs
 * that are ported from fig-forth to ans-forth and where some parts are
 * in ans-forth speak while others are in fig-forth speak. Decompiling
 * the result will show to me whether there was an error during load
 * time of the program, e.g. the "tick" used will turn out to be either
 * a "cfa'" (the ans-forth result) or "pfa'" (the fig-forth result).
 * Likewise I let "create" decompile as either "<builds" or "create:"
 * because the latter is not supposed to be extended with "does>".
 */

P4_LISTWORDS (core) =
{
    P4_INTO ("[ANS]", 0),    /* core words */
    P4_FXco ("!",            p4_store),
    P4_FXco ("#",            p4_sh),
    P4_FXco ("#>",           p4_sh_greater),
    P4_FXco ("#S",           p4_sh_s),
    P4_IXco ("(",            p4_paren),
    P4_FXco ("*",            p4_star),
    P4_FXco ("*/",           p4_star_slash),
    P4_FXco ("*/MOD",        p4_star_slash_mod),
    P4_FXco ("+",            p4_plus),
    P4_FXco ("+!",           p4_plus_store),
    P4_SXco ("+LOOP",        p4_plus_loop),
    P4_FXco (",",            p4_comma),
    P4_FXco ("-",            p4_minus),
    P4_FXco (".",            p4_dot),
    P4_SXco (".\"",          p4_dot_quote),
    P4_FXco ("/",            p4_slash),
    P4_FXco ("/MOD",         p4_slash_mod),
    P4_FXco ("0<",           p4_zero_less),
    P4_FXco ("0=",           p4_zero_equal),
    P4_FXco ("1+",           p4_one_plus),
    P4_FXco ("1-",           p4_one_minus),
    P4_FXco ("2!",           p4_two_store),
    P4_FXco ("2*",           p4_two_star),
    P4_FXco ("2/",           p4_two_slash),
    P4_FXco ("2@",           p4_two_fetch),
    P4_FXco ("2DROP",        p4_two_drop),
    P4_FXco ("2DUP",         p4_two_dup),
    P4_FXco ("2OVER",        p4_two_over),
    P4_FXco ("2SWAP",        p4_two_swap),
    P4_RTco (":",            p4_colon),
    P4_SXco (";",            p4_semicolon),
    P4_FXco ("<",            p4_less_than),
    P4_FXco ("<#",           p4_less_sh),
    P4_FXco ("=",            p4_equals),
    P4_FXco (">",            p4_greater_than),
    P4_FXco (">BODY",        p4_to_body),
    P4_DVaR (">IN",          input.to_in),
    P4_FXco (">NUMBER",      p4_to_number),
    P4_SXco (">R",           p4_to_r),
    P4_FXco ("?DUP",         p4_Q_dup),
    P4_FXco ("@",            p4_fetch),
    P4_FXco ("ABS",          p4_abs),
    P4_FXco ("ACCEPT",       p4_accept),
    P4_FXco ("ALIGN",        p4_align),
    P4_FXco ("ALIGNED",      p4_aligned),
    P4_FXco ("ALLOT",        p4_allot),
    P4_FXco ("AND",          p4_and),
    P4_DVaR ("BASE",         base),
    P4_SXco ("BEGIN",        p4_begin),
    P4_OCoN ("BL",           ' '),
    P4_FXco ("C!",           p4_c_store),
    P4_FXco ("C,",           p4_c_comma),
    P4_FXco ("C@",           p4_c_fetch),
    P4_FXco ("CELL+",        p4_cell_plus),
    P4_FXco ("CELLS",        p4_cells),
    P4_FXco ("CHAR",         p4_char),
    P4_FXco ("CHAR+",        p4_char_plus),
    P4_FXco ("CHARS",        p4_chars),
    P4_RTco ("CONSTANT",     p4_constant),
    P4_FXco ("COUNT",        p4_count),
    P4_FXco ("CR",           p4_cr),
    P4_FXco ("DECIMAL",      p4_decimal),
    P4_FXco ("DEPTH",        p4_depth),
    P4_SXco ("DO",           p4_do),
    P4_SXco ("DOES>",        p4_does),
    P4_FXco ("DROP",         p4_drop),
    P4_FXco ("DUP",          p4_dup),
    P4_SXco ("ELSE",         p4_else),
    P4_FXco ("EMIT",         p4_emit),
    P4_FXco ("ENVIRONMENT?", p4_environment_Q_core),
    P4_FXco ("EVALUATE",     p4_evaluate),
    P4_FXco ("EXECUTE",      p4_execute),
    P4_SXco ("EXIT",         p4_exit),
    P4_FXco ("FILL",         p4_fill),
    P4_FXco ("FIND",         p4_find),
    P4_FXco ("FM/MOD",       p4_f_m_slash_mod),
    P4_FXco ("HERE",         p4_here),
    P4_FXco ("HOLD",         p4_hold),
    P4_SXco ("I",            p4_i),
    P4_SXco ("IF",           p4_if),
    P4_FXco ("IMMEDIATE",    p4_immediate),
    P4_FXco ("INVERT",       p4_invert),
    P4_SXco ("J",            p4_j),
    P4_FXco ("KEY",          p4_key),
    P4_SXco ("LEAVE",        p4_leave),
    P4_SXco ("LITERAL",      p4_literal),
    P4_SXco ("LOOP",         p4_loop),
    P4_FXco ("LSHIFT",       p4_l_shift),
    P4_FXco ("M*",           p4_m_star),
    P4_FXco ("MAX",          p4_max),
    P4_FXco ("MIN",          p4_min),
    P4_FXco ("MOD",          p4_mod),
    P4_FXco ("MOVE",         p4_move),
    P4_FXco ("NEGATE",       p4_negate),
    P4_FXco ("OR",           p4_or),
    P4_FXco ("OVER",         p4_over),
    P4_SXco ("POSTPONE",     p4_postpone),
    P4_FXco ("QUIT",         p4_quit),
    P4_SXco ("R>",           p4_r_from),
    P4_SXco ("R@",           p4_r_fetch),
    P4_IXco ("RECURSE",      p4_recurse),
    P4_SXco ("REPEAT",       p4_repeat),
    P4_FXco ("ROT",          p4_rot),
    P4_FXco ("RSHIFT",       p4_r_shift),
    P4_SXco ("S\"",          p4_s_quote),
    P4_FXco ("S>D",          p4_s_to_d),
    P4_FXco ("SIGN",         p4_sign),
    P4_FXco ("SM/REM",       p4_s_m_slash_rem),
    P4_FXco ("SOURCE",       p4_source),
    P4_FXco ("SPACE",        p4_space),
    P4_FXco ("SPACES",       p4_spaces),
    P4_DVaR ("STATE",        state),
    P4_FXco ("SWAP",         p4_swap),
    P4_SXco ("THEN",         p4_then),
    P4_FXco ("TYPE",         p4_type),
    P4_FXco ("U.",           p4_u_dot),
    P4_FXco ("U<",           p4_u_less_than),
    P4_FXco ("UM*",          p4_u_m_star),
    P4_FXco ("UM/MOD",       p4_u_m_slash_mod),
    P4_SXco ("UNLOOP",       p4_unloop),
    P4_SXco ("UNTIL",        p4_until),
    P4_RTco ("VARIABLE",     p4_variable),
    P4_SXco ("WHILE",        p4_while),
    P4_FXco ("WORD",         p4_word),
    P4_FXco ("XOR",          p4_xor),
    P4_IXco ("[",            p4_left_bracket),
    P4_SXco ("[']",          p4_bracket_tick),
    P4_SXco ("[CHAR]",       p4_bracket_char),
    P4_FXco ("]",            p4_right_bracket),

    /* core extension words */
    P4_DVaR ("#TIB",         input.number_tib),
    P4_IXco (".(",           p4_dot_paren),
    P4_FXco (".R",           p4_dot_r),
    P4_FXco ("0<>",          p4_zero_not_equals),
    P4_FXco ("0>",           p4_zero_greater),
    P4_SXco ("2>R",          p4_two_to_r),
    P4_SXco ("2R>",          p4_two_r_from),
    P4_SXco ("2R@",          p4_two_r_fetch),
    P4_RTco (":NONAME",      p4_colon_noname),
    P4_FXco ("<>",           p4_not_equals),
    P4_SXco ("?DO",          p4_Q_do),
    P4_SXco ("AGAIN",        p4_again),
    P4_SXco ("C\"",          p4_c_quote),
    P4_SXco ("CASE",         p4_case),
    P4_FXco ("COMPILE,",     p4_compile_comma),
    P4_FXco ("CONVERT",      p4_convert),
    P4_SXco ("ENDCASE",      p4_endcase),
    P4_SXco ("ENDOF",        p4_endof),
    P4_FXco ("ERASE",        p4_erase),
    P4_FXco ("EXPECT",       p4_expect),
    P4_OCoN ("FALSE",        P4_FALSE),
    P4_FXco ("HEX",          p4_hex),
    P4_RTco ("MARKER",       p4_marker),
    P4_FXco ("NIP",          p4_nip),
    P4_SXco ("OF",           p4_of),
    P4_FXco ("PAD",          p4_pad),
    P4_FXco ("PARSE",        p4_parse),
    P4_FXco ("PICK",         p4_pick),
    P4_FXco ("QUERY",        p4_query),
    P4_FXco ("REFILL",       p4_refill),
    P4_FXco ("RESTORE-INPUT",p4_restore_input),
    P4_FXco ("ROLL",         p4_roll),
    P4_FXco ("SAVE-INPUT",   p4_save_input),
    P4_DVaL ("SOURCE-ID",    input.source_id),
    P4_DVaR ("SPAN",         span),
    P4_DVaL ("TIB",          input.tib),
    P4_SXco ("TO",           p4_to),
    P4_OCoN ("TRUE",         P4_TRUE),
    P4_FXco ("TUCK",         p4_tuck),
    P4_FXco ("U.R",          p4_u_dot_r),
    P4_FXco ("U>",           p4_u_greater_than),
    P4_FXco ("UNUSED",       p4_unused),
    P4_RTco ("VALUE",        p4_value),
    P4_FXco ("WITHIN",       p4_within),
    P4_IXco ("[COMPILE]",    p4_bracket_compile),
    P4_IXco ("\\",           p4_backslash),

    P4_INTO ("FORTH", "[ANS]"),
    P4_SNYM ("\"",           "C\""),
    P4_FXco ("PARSE-WORD",   p4_parse_word),
    P4_RTco ("<BUILDS",      p4_builds),
    P4_FXco ("CFA'",         p4_tick),

    P4_INTO ("[ANS]", 0),
    P4_FNYM ("CREATE",       "<BUILDS"),
    P4_FNYM ("'",            "CFA'"),

    P4_INTO ("ENVIRONMENT", 0),
    /* enviroment hints (testing for -EXT will mark this wordset as present) */
    P4_OCoN ("CORE-EXT",                1994 ),
    P4_OCoN ("/COUNTED-STRING",         UCHAR_MAX ),
    P4_OCoN ("/HOLD",                   MIN_HOLD ),
    P4_OCoN ("/PAD",                    MIN_PAD ),
    P4_OCoN ("ADDRESS-UNIT-BITS",       CHAR_BIT ),
    P4_OCoN ("FLOORED",                 P4_TRUE ),
    P4_OCoN ("MAX-CHAR",                UCHAR_MAX ),
    P4_OCoN ("MAX-N",                   CELL_MAX ),
    P4_OCoN ("MAX-U",                   UCELL_MAX ),
    P4_FXco ("STACK-CELLS",             p__stack_cells),
    P4_FXco ("RETURN-STACK-CELLS",      p__return_stack_cells),
};
P4_COUNTWORDS (core, "Core words + extensions");

/*@}*/

