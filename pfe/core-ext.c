/**
 *  CORE-EXT -- The standard CORE and CORE-EXT wordset
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 5.14 %
 *    (%date_modified: Mon Mar 12 10:32:02 2001 %)
 *
 *  @description
 *      The Core Wordset contains the most of the essential words
 *      for ANS Forth.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
      "@(#) $Id: core-ext.c,v 0.30 2001-03-12 09:32:02 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-xtra.h>

#include <ctype.h>
#include <string.h>
#ifndef P4_NO_FP
#include <float.h>
#endif
#include <limits.h>

#include <pfe/core-ext.h>
#include <pfe/double-sub.h>
#include <pfe/double-ext.h>
#include <pfe/dict-sub.h>
#include <pfe/term-sub.h>
#include <pfe/_missing.h>

#include <pfe/logging.h>

/**@name OPENBOOT EXTENSION
 * OpenBoot compatible extensions
 */
/**@{*/

/** 
   these two can replace old p4th's F_(alloc_parsed_string) 
   in words like  S" and C"; It still needs some polish.
   E.g. There are no controls or Byte-definitions in here.
   The words S" and C" don't decompile correctly, since 
   debug doesn't know about this routine yet,
   /and/ it works only in compilation mode so far
   (a lot to do though)
*/
int
p4_parse_openstring_into (char del, char* px, int lx)
{
    int n = 0;
    char* p; unsigned l;
    p4_source (&p, &l);

    if (TO_IN >= l)
        return n;

    while(1)
    {
        while (p[TO_IN] != del && TO_IN < l)
        {
            px[n++] = p[TO_IN++];
            if (TO_IN >= l) return n;
        }
        TO_IN++;

        if (isspace ((unsigned char) p[TO_IN]))
            return n;

        switch (p[TO_IN])
        {
         case '|':
         case 'n':  px[n++] = '\n'; TO_IN++; continue;
         case 'r':  px[n++] = '\r'; TO_IN++; continue;
         case 'f':  px[n++] = '\f'; TO_IN++; continue;
         case 't':  px[n++] = '\t'; TO_IN++; continue;
         case 'b':  px[n++] = '\b'; TO_IN++; continue;
         case '!':  px[n++] = '\a'; TO_IN++; continue;
         case '\"': px[n++] = '\"'; TO_IN++; continue;
         case '\'': px[n++] = '\''; TO_IN++; continue;
        }

        if (!isalnum ((unsigned char) p[TO_IN]))
            return n;

        px[n++] = p[TO_IN++]; continue;
    }
}

/**
 */
char* p4_alloc_parse_openstring (char del)
{
    char* x = DP++;
    *x = 0;

    DP += *x = p4_parse_openstring_into (del, x+1, 255);
    FX (p4_align);
    return x;
}

/**@}*/


/************************************************************************/
/* Core Words                                                           */
/************************************************************************/

/** ! ( val addr -- )
 */
FCode (p4_store)
{
    *(p4cell *) SP[0] = SP[1];
    SP += 2;
}

/** # ( n.n -- n.n' ) 
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

/** #> ( n.n -- str-addr str-len ) 
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

/** #S ( n.n -- n.n ) f
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

/** "'" ( 'name' -- xt ) 
 * return the execution token of the following name. This word
 * is _not_ immediate and may not do what you expect in
 * compile-mode. See => ['] and => '> - note that in FIG-forth
 * the word of the same name had returned the PFA (not the CFA)
 * and was immediate/smart, so beware when porting forth-code
 * from FIG-forth to ANSI-forth.
 */
FCode (p4_tick)
{
    p4_tick ((p4xt *) --SP);
}

/** (  ( 'comment<closeparen>' -- ) 
 * eat everything up to the next closing paren - treat it
 * as a comment.
 */
FCode (p4_paren)
{
    char *p;
    p4ucell n;
    
    switch (SOURCE_ID)
    {
     case -1:
     case 0:
         p4_parse (')', &p, &n);
         break;
     default:
         while (!p4_parse (')', &p, &n) && p4_refill ());
         break;
    }
}

/** * ( a b -- a*b ) 
 * return the multiply of the two args
 */
FCode (p4_star)
{
    SP[1] = SP[0] * SP[1];
    SP++;
}

/** "*\/" ( a b c -- a*b/c ) 
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

/** "*\/MOD" ( a b c -- m n )
 * has an adavantage over the sequence of => *
 * and => /MOD by using an intermediate double-cell
 * value.
 */
FCode (p4_star_slash_mod)
{
    *(fdiv_t *) &SP[1] = p4_d_fmdiv (p4_d_mmul (SP[2], SP[1]), SP[0]);
    SP++;
}

/** + ( a b -- a+b ) 
 * return the sum of the two args
 */
FCode (p4_plus)
{
    SP[1] += SP[0];
    SP++;
}

/** +! ( val addr -- ) 
 * add val to the value found in addr
 simulate:
   : +! TUCK @ + SWAP ! ;
 */
FCode (p4_plus_store)
{
    *(p4cell *) SP[0] += SP[1];
    SP += 2;
}

/** ((+LOOP)) ( increment -- ) 
 * compiled by => +LOOP
 */ 
FCode (p4_plus_loop_execution)
{
    p4cell i = *SP++;

    if (i < 0
      ? (*FX_RP += i) >= 0
      : (*FX_RP += i) < 0)
        IP = RP[2];
    else
        RP += 3;
}

/** +LOOP ( increment -- ) 
 * compile => ((+LOOP)) which will use the increment
 * as the loop-offset instead of just 1. See the
 * => DO and => LOOP construct.
 */
FCode (p4_plus_loop)
{
    p4_Q_pairs (LOOP_MAGIC);
    FX_COMPILE1 (p4_plus_loop);
    FX (p4_forward_resolve);
}
P4COMPILES (p4_plus_loop, p4_plus_loop_execution,
	  P4_SKIPS_NOTHING, P4_LOOP_STYLE);

/** , ( val -- ) 
 * store the value in the dictionary
 simulate:
   : , DP  1 CELLS DP +!  ! ;
 */
FCode (p4_comma)
{
    FX_COMMA (*SP++);
}

/** - ( a b -- a-b ) 
 * return the difference of the two arguments
 */
FCode (p4_minus)
{
    SP[1] -= SP[0];
    SP++;
}

/** . ( val -- ) 
 * print the numerical value to stdout - uses => BASE
 */
FCode (p4_dot)
{
    FX (p4_s_to_d);
    FX (p4_d_dot);
}

/** ((.")) ( -- ) _skip_string_
 * compiled by => ." string"
 */ 
FCode (p4_dot_quote_execution)
{
    char *p = (char *) IP;

    p4_type (p + 1, *p);
    FX_SKIP_STRING;
}

/** ." ( [string<">] -- ) 
 * print the string to stdout
 */
FCode (p4_dot_quote)
{
    _FX_STATESMART_Q_COMP;
    if (STATESMART) 
    {
        FX_COMPILE1 (p4_dot_quote);
        p4_parse_comma('"');
    }else{
        char* p; p4ucell n;
        p4_parse ('"', &p, &n);
        p4_type (p, n);
    }
}
P4COMPILES (p4_dot_quote, p4_dot_quote_execution,
	  P4_SKIPS_STRING, P4_DEFAULT_STYLE);

/** / ( a b  -- a/b ) 
 * return the quotient of the two arguments
 */
FCode (p4_slash)
{
    fdiv_t res = p4_fdiv (SP[1], SP[0]);

    *++SP = res.quot;
}

/** /MOD ( a b -- m n ) 
 * divide a and b and return both
 * quotient n and remainder m
 */
FCode (p4_slash_mod)
{
    *(fdiv_t *) SP = p4_fdiv (SP[1], SP[0]);
}

/** 0< ( val -- cond ) 
 * return a flag that is true if val is lower than zero
 simulate:
  : 0< 0 < ;
 */
FCode (p4_zero_less)
{
    *SP = P4_FLAG (*SP < 0);
}

/** 0= ( val -- cond ) 
 * return a flag that is true if val is just zero
 simulate:
  : 0= 0 = ;
 */
FCode (p4_zero_equal)
{
    *SP = P4_FLAG (*SP == 0);
}

/** 1+ ( val -- val+1 ) 
 * return the value incremented by one
 simulate:
  : 1+ 1 + ;
 */
FCode (p4_one_plus)
{
    ++*SP;
}

/** 1- ( val -- val-1 ) 
 * return the value decremented by one
 simulate:
   : 1- 1 - ;
 */
FCode (p4_one_minus)
{
    --*SP;
}

/** 2! ( a,a addr -- ) 
 * double-cell store 
 */
FCode (p4_two_store)
{
    *(p4dcell *) *SP = *(p4dcell *) &SP[1];
    SP += 3;
}

/** 2* ( a -- a*2 ) 
 * multiplies the value with two - but it
 * does actually use a shift1 to be faster
 simulate:
  : 2* 2 * ; ( canonic) : 2* 1 LSHIFT ; ( usual)
 */
FCode (p4_two_star)
{
    *SP <<= 1;
}

/** 2/ ( a -- a/2 ) 
 * divides the value by two - but it
 * does actually use a shift1 to be faster
 simulate:
  : 2/ 2 / ; ( canonic) : 2/ 1 RSHIFT ; ( usual)
 */
FCode (p4_two_slash)
{
    *SP >>= 1;
}

/** 2@ ( addr -- a,a ) 
 * double-cell fetch
 */
FCode (p4_two_fetch)
{
    p4dcell *p = (p4dcell *) *SP--;

    *(p4dcell *) SP = *p;
}

/** 2DROP ( a b -- ) 
 * double-cell drop, also used to drop two items
 */
FCode (p4_two_drop)
{
    SP += 2;
}

/** 2DUP ( a,a -- a,a a,a ) 
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

/** 2OVER ( a,a b,b -- a,a b,b a,a ) 
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

/** 2SWAP ( a,a b,b -- b,b a,a ) 
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

/** (NEST) ( -- ) 
 * compiled by => :
 */
FCode (p4_colon_RT)
{
    *--RP = IP;
    IP = (p4xt *) WP_PFA;
}

FCode (p4_colon_EXIT)
{
    FX (p4_Q_csp);
    STATE = P4_FALSE;
    FX (p4_unsmudge);
}    

/** : ( 'name' -- ) 
 * create a header for a nesting word and go to compiling
 * mode then. This word is usually ended with => ; but
 * the execution of the resulting colon-word can also 
 * return with => EXIT
 */
FCode (p4_colon)
{
    FX (p4_Q_exec);
    p4_header (p4_colon_RT_, P4xSMUDGED);
    FX (p4_store_csp);
    STATE = P4_TRUE;
    PFE.locals = NULL;
    PFE.semicolon_code = PFX(p4_colon_EXIT);
}



/** ((;)) ( -- ) 
 * compiled by => ; and maybe => ;AND --
 * it will perform an => EXIT
 */ 
FCode (p4_semicolon_execution)
{
    IP = *RP++;
}


/** ; ( -- ) 
 * compiles => ((;)) which does => EXIT the current
 * colon-definition. It does then end compile-mode
 * and returns to execute-mode. See => : and => :NONAME
 */
FCode (p4_semicolon)
{
#  ifdef PFE_WITH_DSTRINGS_EXT  /* (dnw 3feb01) */
    extern FCode (p4_do_drop_str_frame);
    FX (p4_do_drop_str_frame);
#  endif

    if (PFE.locals)
    {
        FX_COMPILE2 (p4_semicolon);
        PFE.locals = NULL;
    }
    else
        FX_COMPILE1 (p4_semicolon);

    if (PFE.semicolon_code)
    {
        PFE.semicolon_code ();
    }else{
        PFE.state = P4_FALSE; /* atleast switch off compiling mode */
    }
}

P4COMPILES2 (p4_semicolon, p4_semicolon_execution, p4_locals_exit_execution,
	   P4_SKIPS_NOTHING, P4_SEMICOLON_STYLE);

/** < ( a b -- cond ) 
 * return a flag telling if a is lower than b
 */
FCode (p4_less_than)
{
    SP[1] = P4_FLAG (SP[1] < SP[0]);
    SP++;
}

/** <# ( -- ) 
 * see also => HOLD for old-style forth-formatting words
 * and => PRINTF of the C-style formatting - this word
 * does initialize the pictured numeric output space.
 */
FCode (p4_less_sh)
{
    p4_HLD = p4_PAD;
}

/** = ( a b -- cond )
 * return a flag telling if a is equal to b
 */
FCode (p4_equals)
{
    SP[1] = P4_FLAG (SP[1] == SP[0]);
    SP++;
}

/** > ( a b -- cond )
 * return a flag telling if a is greater than b
 */
FCode (p4_greater_than)
{
    SP[1] = P4_FLAG (SP[1] > SP[0]);
    SP++;
}

/** >BODY ( addr -- addr' )
 * adjust the execution-token (ie. the CFA) to point
 * to the parameter field (ie. the PFA) of a word.
 * this is implementation dependent and is usually
 * either "1 CELLS +" or "2 CELLS +"
 */
FCode (p4_to_body) 
{
    *SP = (p4cell) TO_BODY (*SP);
}

/** >NUMBER ( a,a str-adr str-len -- a,a' str-adr' str-len) 
 * try to convert a string into a number, and place
 * that number at a,a respeciting => BASE
 */
FCode (p4_to_number)
{
    SP[1] = (p4cell)
        p4_to_number (
                      (char *) SP[1],
                      (p4ucell *) &SP[0],
                      (p4udcell *) &SP[2],
                      BASE);
}

/** >R ( value -- )
 * save the value onto the return stack. The return
 * stack must be returned back to clean state before
 * an exit and you should note that the return-stack
 * is also touched by the => DO ... => WHILE loop.
 * Use => R> to clean the stack and => R@ to get the 
 * last value put by => >R
 */
FCode (p4_to_r)
{
    RP_PUSH (FX_POP);
}

/** ?DUP ( value -- value|[nothing] )
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

/** @ ( addr -- value )
 * fetch the value from the variables address
 */
FCode (p4_fetch)
{
    *SP = *(p4cell *) *SP;
}

/** ABS ( value -- value' )
 * return the absolute value
 */
FCode (p4_abs)
{
    if (*SP < 0)
        *SP = -*SP;
}

/** ACCEPT ( a n -- n' ) 
 * get a string from terminal into the named input 
 * buffer, returns the number of bytes being stored
 * in the buffer. May provide line-editing functions.
 */
FCode (p4_accept)
{
    SP[1] = p4_accept ((char *) SP[1], SP[0]);
    SP += 1;
}

/** ALIGN ( -- )
 * will make the dictionary aligned, usually to a
 * cell-boundary, see => ALIGNED
 */
FCode (p4_align)
{
    while (!P4_ALIGNED (DP))
        *DP++ = 0;
}

/** ALIGNED ( addr -- addr' )
 * uses the value (being usually a dictionary-address)
 * and increment it to the required alignment for the
 * dictionary which is usually in => CELLS - see also
 * => ALIGN
 */
FCode (p4_aligned)
{
    *SP = p4_aligned (*SP);
}

/** ALLOT ( count -- )
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

/** AND ( val mask -- val' )
 * mask with a bitwise and - be careful when applying
 * it to logical values.
 */
FCode (p4_and)
{
    SP[1] &= SP[0];
    SP++;
}

/** BEGIN ( -- ) compile-time: ( -- cs-marker )
 * start a control-loop, see => WHILE and => REPEAT
 */
FCode (p4_begin)
{
    FX_COMPILE1 (p4_begin);
    FX (p4_backward_mark);
    *--SP = DEST_MAGIC;
}
P4COMPILES (p4_begin, p4_noop, P4_SKIPS_NOTHING, P4_BEGIN_STYLE);

/** C! ( value address -- )
 * store the byte-value at address, see => !
 */
FCode (p4_c_store)
{
    *(char *) SP[0] = SP[1];
    SP += 2;
}

/** C, ( value -- )
 * store a new byte-value in the dictionary, implicit 1 ALLOT,
 * see => ,
 */
FCode (p4_c_comma)
{
    *DP++ = (p4char) *SP++;
}

/** C@ ( addr -- value )
 * fetch a byte-value from the address, see => @
 */
FCode (p4_c_fetch)
{
    *SP = *(p4char *) *SP;
}

/** CELL+ ( value -- value' )
 * adjust the value by adding a single Cell's width
 * - the value is often an address or offset, see => CELLS
 */
FCode (p4_cell_plus)
{
    *SP += sizeof (p4cell);
}

/** CELLS ( value -- value' )
 * scale the value by the sizeof a Cell
 * the value is then often applied to an address or
 * fed into => ALLOT
 */
FCode (p4_cells)
{
    *SP *= sizeof (p4cell);
}

/** CHAR ( 'word' -- value )
 * return the (ascii-)value of the following word's
 * first character. 
 */
FCode (p4_char)
{
    char *p;
    p4ucell n;

    p4_skip_delimiter (' ');
    p4_parse (' ', &p, &n);
    if (n == 0)
        p4_throw (P4_ON_INVALID_NAME);
    *--SP = *(p4char *) p;
}

/** CHAR+ ( value -- value' )
 * increment the value by the sizeof one char
 * - the value is often a pointer or an offset,
 * see => CHARS
 */
FCode (p4_char_plus)
{
    *SP += sizeof (char);
}

/** CHARS ( value -- value' )
 * scale the value by the sizeof a char
 * - the value is then often applied to an address or
 * fed into => ALLOT (did you expect that sizeof(p4char)
 * may actually yield 2 bytes?)
 */
FCode (p4_chars)
{
    *SP *= sizeof (char);
}

/** ((CONSTANT)) ( -- )
 * runtime compiled by => CONSTANT
 */ 
FCode (p4_constant_RT)
{
    *--SP = WP_PFA[0];
}
/** CONSTANT ( value 'name' -- )
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
    p4_header (p4_constant_RT_, 0);
    FX_COMMA (*SP++);
}

/** COUNT ( counted-string -- string-pointer string-length )
 * usually before calling => TYPE
 */
FCode (p4_count)
{
    /* can not unpack twice - this trick prevents from many common errors */
    if (256 > (p4ucell)(SP[0])) return;
    
    --SP;
    SP[0] = *((p4char*)(SP[1]))++;
}

/** CR ( -- )
 * print a carriage-return/new-line on stdout
 */  
FCode (p4_cr)
{
    p4_outc ('\n');
    OUT = 0;
    PFE.lines++;
}

/** ((VAR)) ( -- pfa )
 * the runtime compiled by => CREATE which
 * is the usual thing to do in => VARIABLE
 */ 
FCode (p4_create_RT)
{
    *--SP = (p4cell) WP_PFA;
}

/** CREATE ( 'name' -- )
 * create a name with runtime => ((VAR)) so
 * that everywhere the name is used the pfa
 * of the name's body is returned. This word
 * is not immediate and is usually used in
 * the first part of a => DOES> defining
 * word.
 */
FCode (p4_create)
{
    p4_header (p4_create_RT_, 0);
}

/** DECIMAL ( -- )
 * set the => BASE to 10
 simulate:
   : DECIMAL 10 BASE ! ;
 */
FCode (p4_decimal)
{
    BASE = 10;
}

/** DEPTH ( -- value )
 * return the depth of the parameter stack before
 * the call, see => SP@ - the return-value is in => CELLS
 */
FCode (p4_depth)
{
    size_t n;

    n = p4_S0 - SP;
    *--SP = n;
}

/** ((DO)) ( end start -- )
 * compiled by => DO
 */ 
FCode (p4_do_execution)
{
    RP -= 3;			/* push onto return-stack: */
    RP[2] = ++IP;		/*  IP to jump back to just after DO */
    RP[1] = (p4xt *) SP[1];	/*  upper limit */
    RP[0] = (p4xt *) (SP[0] - /*lower_minus*/  SP[1] /*upper_limit*/ );
    SP += 2;
}

/** DO ( end start -- ) ... LOOP
 *  pushes $end and $start onto the return-stack ( => >R )
 *  and starts a control-loop that ends with => LOOP or
 *  => +LOOP and may get a break-out with => LEAVE . The
 *  loop-variable can be accessed with => I
 */
FCode (p4_do)
{
    FX_COMPILE1 (p4_do);
    FX (p4_forward_mark);
    *--SP = LOOP_MAGIC;
}
P4COMPILES (p4_do, p4_do_execution,
  P4_SKIPS_OFFSET, P4_DO_STYLE);

/** ((DOES>)) ( -- pfa )
 * runtime compiled by DOES>
 */ 
FCode (p4_does_defined_RT)
{
#if 1
    FX_PUSH (P4_TO_DOES_BODY(WP_CFA));
    *--RP = IP;
    IP = *P4_TO_DOES_CODE(WP_CFA);
#else
# ifndef PFE_WITH_FIG
    FX_PUSH (WP_PFA);
    *--RP = IP;
    IP = (p4xt *) WP_CFA[-1]; /*TO_EXTRA*/
# else
    FX_PUSH (&WP_PFA[1]);
    *--RP = IP;
    IP = (p4xt *) WP_PFA[0]; /*TO_BODY*/
# endif
#endif
}

/** (DOES>) ( -- pfa )
 * execution compiled by => DOES>
 */ 
FCode (p4_does_execution)
{
    p4xt xt;
    if (! LAST)
        p4_throw (P4_ON_ARG_TYPE);

    xt = p4_name_from (LAST);
    *xt = PFX (p4_does_defined_RT);
    *P4_TO_DOES_CODE(xt) = IP;

    if (LP != FX_RP)
        IP = *RP++;
    else
        FX (p4_locals_exit_execution);
}

/** DOES> ( -- pfa )
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
	FX_COMPILE1 (p4_does);
	PFE.locals = NULL;
    }else{
	/* see p4_does_execution above */
	p4xt xt;
	if (! LAST)
	    p4_throw (P4_ON_ARG_TYPE);
	FX (p4_align);

	xt = p4_name_from (LAST);
	*xt = PFX (p4_does_defined_RT);
	*P4_TO_DOES_CODE(xt) = (p4xt*) DP;

	/* now, see p4_colon */
	FX (p4_store_csp);
	STATE = P4_TRUE;
	PFE.locals = NULL;
	PFE.semicolon_code = PFX(p4_colon_EXIT);
    }
}
P4COMPILES (p4_does, p4_does_execution,
  P4_SKIPS_NOTHING, P4_DOES_STYLE);

/** DROP ( a -- )
 * just drop the word on the top of stack, see => DUP
 */
FCode (p4_drop)
{
    SP++;
}

/** DUP ( a -- a a )
 * duplicate the cell on top of the stack - so the
 * two topmost cells have the same value (they are
 * equal w.r.t => = ) , see => DROP for the inverse
 */
FCode (p4_dup)
{
    --SP;
    SP[0] = SP[1];
}

/** ((ELSE)) ( -- )
 * execution compiled by => ELSE - just a simple
 * => BRANCH
 */ 
FCode (p4_else_execution)
{
    FX_BRANCH;
}

/** ELSE ( -- )
 * will compile an => ((ELSE)) => BRANCH that performs an 
 * unconditional jump to the next => THEN - and it resolves 
 * an => IF for the non-true case
 */
FCode (p4_else)
{
    p4_Q_pairs (ORIG_MAGIC);
    FX_COMPILE1 (p4_else);
    FX (p4_ahead) ;
    FX (p4_rot) ;
    FX (p4_forward_resolve) ;
}
P4COMPILES (p4_else, p4_else_execution,
  P4_SKIPS_OFFSET, P4_ELSE_STYLE);

/** EMIT ( char -- )
 * print the char-value on stack to stdout
 */
FCode (p4_emit)
{
    PFE.execute (PFE.emit);
}

/** ENVIRONMENT? ( a1 n1 -- false | ?? true )
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

 * special extension: a search for CORE will also find a definition
 * of CORE-EXT or CORE-EXT-EXT or CORE-EXT-EXT-EXT - it just has to
 * be below the ansi-standard maximum length of 31 chars.
 */
FCode (p4_environment_Q)
{
    auto char query[32];
    p4cell len = SP[0];

    if (len > 256 || -256 > len ) 
    {  /* this scheme allows you to submit a forth counted string */
	P4_warn ("counted string at query to ENVIRONMENT?");
	FX (p4_count); 
	len = SP[0];
    }

    if (len < 31 )
    {   /* this scheme allows you also to submit a zero-terminated string */
	memset (query, 0, sizeof(query));
	strncpy (query, (char*) SP[1], len );
	len = strlen (query);
	p4_upper (query, len);
    }

    if (0 < len && len < 32 && PFE.environ_wl) 
    { 
	int i = 3;
	while (--i)
	{
	    p4char* nfa = p4_search_wordlist (query, len, PFE.environ_wl);
	    if (nfa)
	    {
		FX_2DROP;
#             ifdef PFE_WITH_FFA
		if (*_FFA(nfa) & P4xONxDESTROY)
		    FX_PUSH (TO_BODY(p4_name_from(nfa)));
		else
#             endif
		    p4_call (p4_name_from(nfa));
		FX_PUSH(P4_TRUE);
		return;
	    }

	    if (len < 28)
	    {
		strcat (query, "-EXT");
		len = strlen (query);
		continue;
	    }else
		break;
	}
    }

    /* not found */
    FX_DROP;
    *SP = 0;
}


/** EVALUATE ( str-ptr str-len -- ) 
 * => INTERPRET the given string, => SOURCE id
 * is -1 during that time.
 */
FCode (p4_evaluate)
{
    char *p = (char *) SP[1];
    int n = SP[0];

    SP += 2;
    p4_evaluate (p, n);
}

/** EXECUTE ( xt -- )
 * run the execution-token on stack - this will usually
 * trap if it was null for some reason, see => >EXECUTE
 simulate:
  : EXECUTE >R EXIT ;
 */
FCode (p4_execute)
{
    PFE.execute ((p4xt) *SP++);
}

/** EXIT ( -- )
 * will unnest the current colon-word so it will actually
 * return the word calling it. This can be found in the
 * middle of a colon-sequence between => : and => ;
 */
FCode (p4_exit)
{
    if (PFE.locals)
        FX_COMPILE2 (p4_exit);
    else
        FX_COMPILE1 (p4_exit);
}
P4COMPILES2 (p4_exit, p4_semicolon_execution, p4_locals_exit_execution,
	   P4_SKIPS_NOTHING, P4_DEFAULT_STYLE);

/** FILL ( mem-addr mem-length char -- )
 * fill a memory area with the given char, does now
 * simply call memset()
 */
FCode (p4_fill)
{
    memset ((void *) SP[2], SP[0], SP[1]);
    SP += 3;
}

/** FIND ( bstring -- cfa|bstring -1|0|1 )
 * looks into the current search-order and tries to find
 * the name string as the name of a word. Returns its
 * execution-token or the original-bstring if not found,
 * along with a flag-like value that is zero if nothing
 * could be found. Otherwise it will be 1 if the word had
 * been immediate, -1 otherwise.
 */
FCode (p4_find)
{
    char *p = (char *) *SP;

    p = p4_find (p + 1, *p);
    if (p)
    {
        *SP = (p4cell) p4_name_from (p);
        *--SP = *_FFA(p) & P4xIMMEDIATE ? 1 : -1;
    }
    else
        *--SP = 0;
}

/** FM/MOD ( n1.n1 n2 -- m n )
 * divide the double-cell value n1 by n2 and return
 * both (floored) quotient n and remainder m 
 */
FCode (p4_f_m_slash_mod)
{
    p4cell denom = *SP++;

    *(fdiv_t *) SP = p4_d_fmdiv (*(p4dcell *) SP, denom);
}

/** HERE ( -- dp-value )
 * used with => WORD and many compiling words
 simulate:   : HERE DP @ ;
 */
FCode (p4_here)
{
    *--SP = (p4cell) DP;
}

/** HOLD ( char -- ) 
 * the old-style forth-formatting system -- this
 * word adds a char to the picutred output string.
 */
FCode (p4_hold)
{
    p4_hold ((char) *SP++);
}

/** I ( -- value )
 * returns the index-value of the innermost => DO .. => LOOP
 */
FCode (p4_i)
{
    *--SP = FX_RP[0] + FX_RP[1];
}

/** ((IF)) ( -- )
 * execution word compiled by => IF - just some simple => ?BRANCH
 */
FCode (p4_if_execution)
{
    if (!*SP++)
        FX_BRANCH;
    else
        IP++;
}

/** IF ( value -- ) .. THEN
 * checks the value on the stack (at run-time, not compile-time)
 * and if true executes the code-piece between => IF and the next
 * => ELSE or => THEN . Otherwise it has compiled a branch over
 * to be executed if the value on stack had been null at run-time.
 */
FCode (p4_if)
{
    FX_COMPILE1 (p4_if);
    FX (p4_ahead);
}
P4COMPILES (p4_if, p4_if_execution,
  P4_SKIPS_OFFSET, P4_IF_STYLE);

/** IMMEDIATE ( -- )
 * make the => LATEST word immediate, see also => CREATE
 */
FCode (p4_immediate)
{
    if (LAST)
        *_FFA(LAST) |= P4xIMMEDIATE;
    else
        p4_throw (P4_ON_ARG_TYPE);
}

/** INVERT ( value -- value' )
 * make a bitwise negation of the value on stack.
 * see also => NEGATE
 */
FCode (p4_invert)
{
    *SP = ~*SP;
}

/** J ( -- value )
 * get the current => DO ... => LOOP index-value being
 * the not-innnermost. (the second-innermost...)
 * see also for the other loop-index-values at
 * => I and => K
 */
FCode (p4_j)
{
    *--SP = FX_RP[3] + FX_RP[4];
}

/** KEY ( -- char ) 
 * return a single character from the keyboard - the
 * key is not echoed.
 */
FCode (p4_key)
{
    PFE.execute (PFE.key);
}

/** LEAVE ( -- )
 * quit the innermost => DO .. => LOOP  - it does even
 * clean the return-stack and branches to the place directly
 * after the next => LOOP
 */
FCode (p4_leave)
{
    IP = RP[2] - 1;
    RP += 3;
    FX_BRANCH;
}

/** ((LIT)) ( -- value )
 * execution compiled by => LITERAL
 */ 
FCode (p4_literal_execution)
{
    FX_PUSH (P4_POP (IP));
}

/** LITERAL ( value -- )
 * if compiling this will take the value from the compiling-stack 
 * and puts in dictionary so that it will pop up again at the
 * run-time of the word currently in creation. This word is used
 * in compiling words but may also be useful in making a hard-constant
 * value in some code-piece like this:
 : DCELLS [ 2 CELLS ] LITERAL * ; ( will save a multiplication at runtime)
 */
FCode (p4_literal)
{
    _FX_STATESMART_Q_COMP;
    if (STATESMART)
    {
        FX_COMPILE1 (p4_literal);
        FX_COMMA (*SP++);
    }
}
P4COMPILES (p4_literal, p4_literal_execution,
  P4_SKIPS_CELL, P4_DEFAULT_STYLE);

/** ((LOOP)) ( -- )
 * execution compiled by => LOOP
 */
FCode (p4_loop_execution)
{
    if (++*FX_RP)			/* increment top of return stack */
        IP = RP[2];			/* if nonzero: loop back */
    else
        RP += 3;			/* if zero: terminate loop */
}

/** LOOP ( -- )
 * resolves a previous => DO thereby compiling => ((LOOP)) which
 * does increment/decrement the index-value and branch back if
 * the end-value of the loop has not been reached.
 */
FCode (p4_loop)
{
    p4_Q_pairs (LOOP_MAGIC);
    FX_COMPILE1 (p4_loop);
    FX (p4_forward_resolve);
}
P4COMPILES (p4_loop, p4_loop_execution,
  P4_SKIPS_NOTHING, P4_LOOP_STYLE);

/** LSHIFT ( value shift-val -- value' )
 * does a bitwise left-shift on value
 */
FCode (p4_l_shift)
{
    SP[1] <<= SP[0];
    SP++;
}

/** M* ( a b -- m,m )
 * multiply and return a double-cell result
 */
FCode (p4_m_star)
{
    *(p4dcell *) SP = mmul (SP[0], SP[1]);
}

/** MAX ( a b -- c )
 * return the maximum of a and b
 */
FCode (p4_max)
{
    if (SP[0] > SP[1])
        SP[1] = SP[0];
    SP++;
}

/** MIN ( a b -- c )
 * return the minimum of a and b
 */
FCode (p4_min)
{
    if (SP[0] < SP[1])
        SP[1] = SP[0];
    SP++;
}

/** MOD ( a b -- c )
 * return the module of "a mod b"
 */
FCode (p4_mod)
{
    fdiv_t res = p4_fdiv (SP[1], SP[0]);
    
    *++SP = res.rem;
}

/** MOVE ( from to length -- ) 
 * memcpy an area
 */
FCode (p4_move)
{
    memmove ((void *) SP[1], (void *) SP[2], (size_t) SP[0]);
    SP += 3;
}

/** NEGATE ( value -- value' )
 * return the arithmetic negative of the (signed) cell
 simulate:   : NEGATE -1 * ;
 */
FCode (p4_negate)
{
    *SP = -*SP;
}

/** OR ( a b -- ab )
 * return the bitwise OR of a and b - unlike => AND this
 * is usually safe to use on logical values
 */
FCode (p4_or)
{
    SP[1] |= SP[0];
    SP++;
}

/** OVER ( a b -- a b a )
 * get the value from under the top of stack. The inverse
 * operation would be => TUCK
 */
FCode (p4_over)
{
    --SP;
    SP[0] = SP[2];
}

/** ((POSTPONE)) ( -- )
 * execution compiled by => POSTPONE
 */ 
FCode (p4_postpone_execution)
{
    FX_COMMA( P4_POP (IP) );
}

/** POSTPONE ( [word] -- )
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
    p4xt xt;

    FX (p4_Q_comp);
    if (!( *_FFA (p4_tick (&xt)) & P4xIMMEDIATE))
        FX_COMPILE1 (p4_postpone);
    FX_COMMA (xt);
}
P4COMPILES (p4_postpone, p4_postpone_execution,
	  P4_SKIPS_NOTHING, P4_DEFAULT_STYLE);

/** QUIT ( -- ) no-return
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

/** R> ( -- value )
 * get back a value from the return-stack that had been saved
 * there using => >R . This is the traditional form of a local
 * var space that could be accessed with => R@ later. If you
 * need more local variables you should have a look at => LOCALS|
 * which does grab some space from the return-stack too, but names
 * them the way you like.
 */
FCode (p4_r_from)
{
    FX_PUSH (RP_POP());
}

/** R@ ( -- value )
 * fetch the (upper-most) value from the return-stack that had
 * been saved there using => >R - This is the traditional form of a local
 * var space. If you need more local variables you should have a 
 * look at => LOCALS| , see also => >R and => R> .
 */
FCode (p4_r_fetch)
{
    *--SP = *FX_RP;
}

/** RECURSE ( ? -- ? )
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
        FX_COMMA (p4_name_from (LAST));
    else
        p4_throw (P4_ON_ARG_TYPE);
}

/** REPEAT ( -- )
 * ends an unconditional loop, see => BEGIN
 */
FCode (p4_repeat)
{
    p4_Q_pairs (DEST_MAGIC);
    FX_COMPILE1 (p4_repeat);
    FX (p4_backward_resolve);
    p4_Q_pairs (ORIG_MAGIC);
    FX (p4_forward_resolve);
}
P4COMPILES (p4_repeat, p4_else_execution,
  P4_SKIPS_OFFSET, P4_REPEAT_STYLE);

/** ROT ( a b c -- b c a )
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

/** RSHIFT ( value shift-val -- value' )
 * does a bitwise logical right-shift on value
 * (ie. the value is considered to be unsigned)
 */
FCode (p4_r_shift)
{
    *(p4ucell *) &SP[1] >>= SP[0];
    SP++;
}

/** ((S")) ( -- string-address string-length )
 * execution compiled by => S"
 */ 
FCode (p4_s_quote_execution)
{
    p4char *p = (p4char *) IP;

    SP -= 2;
    SP[0] = *p;
    SP[1] = (p4cell) (p + 1);
    FX_SKIP_STRING;
}

/** 'S"' ( [string<">] -- string-address string-length)
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
        FX_COMPILE1 (p4_s_quote);
        p4_parse_comma('"');
    }else{
        char *p, *q;
        p4ucell n;

        p = p4_pocket ();
        p4_parse ('"', &q, &n);
        if (n > 255)
            n = 255;
        *p++ = n;
        memcpy (p, q, n);
        *--SP = (p4cell) p;
        *--SP = n;
    }
}
P4COMPILES (p4_s_quote, p4_s_quote_execution,
  P4_SKIPS_STRING, P4_DEFAULT_STYLE);

/** S>D ( a -- a,a' )
 * signed extension of a single-cell value to a double-cell value
 */
FCode (p4_s_to_d)
{
    SP--;
    SP[0] = SP[1] < 0 ? -1 : 0;
}

/** SIGN ( a -- )
 * put the sign of the value into the hold-space, this is
 * the forth-style output formatting, see => HOLD
 */
FCode (p4_sign)
{
    if (*SP++ < 0)
        p4_hold ('-');
}

/** SM/REM ( a.a b -- c d ) 
 * see => /MOD or => FM/MOD or => UM/MOD or => SM/REM
 */
FCode (p4_s_m_slash_rem)
{
    p4cell denom = *SP++;

    *(fdiv_t *) SP = p4_d_smdiv (*(p4dcell *) SP, denom);
}

/** SOURCE ( -- buffer IN-offset )
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
    char *p;
    int in;

    p4_source (&p, &in);
    SP -= 2;
    SP[1] = (p4cell) p;
    SP[0] = in;
}

/** SPACE ( -- )
 * print a single space to stdout, see => SPACES
 simulate:    : SPACE  BL EMIT ;
 */
FCode (p4_space)
{
    p4_outc (' ');
}

/** SPACES ( n -- )
 * print n space to stdout, actually a loop over n calling => SPACE ,
 * but the implemenation may take advantage of printing chunks of
 * spaces to speed up the operation.
 */
FCode (p4_spaces)
{
    p4_emits (*SP++, ' ');
}

/** SWAP ( a b -- b a )
 * exchanges the value on top of the stack with the value beneath it
 */
FCode (p4_swap)
{
    p4cell h = SP[1];

    SP[1] = SP[0];
    SP[0] = h;
}

/** THEN ( -- )
 * does resolve a branch coming from either => IF or => ELSE
 */
FCode (p4_then)
{
    FX_COMPILE1 (p4_then);
    p4_Q_pairs (ORIG_MAGIC);
    FX (p4_forward_resolve);
}
P4COMPILES (p4_then, p4_noop, P4_SKIPS_NOTHING, P4_THEN_STYLE);

/** TYPE ( string-pointer string-length -- )
 * prints the string-buffer to stdout, see => COUNT and => EMIT
 */
FCode (p4_type)
{
    PFE.execute (PFE.type);
}

/** U. ( value )
 * print unsigned number to stdout
 */
FCode (p4_u_dot)
{
    *--SP = 0;
    FX (p4_d_dot);
}

/** U< ( a b -- cond )
 * unsigned comparison, see => <
 */
FCode (p4_u_less_than)
{
    SP[1] = P4_FLAG ((p4ucell) SP[1] < (p4ucell) SP[0]);
    SP++;
}

/** UM* ( a b -- c,c )
 * unsigned multiply returning double-cell value
 */
FCode (p4_u_m_star)
{
    *(p4udcell *) SP = p4_d_ummul ((p4ucell) SP[0], (p4ucell) SP[1]);
}

/** UM/MOD ( a b -- c,c )
 * see => /MOD and => SM/REM
 */
FCode (p4_u_m_slash_mod)
{
    p4ucell denom = (p4ucell) *SP++;

    *(udiv_t *) SP = p4_d_umdiv (*(p4udcell *) SP, denom);
}

/** UNLOOP ( -- )
 * drop the => DO .. => LOOP runtime variables from the return-stack,
 * usually used just in before an => EXIT call. Using this multiple
 * times can unnest multiple nested loops.
 */
FCode (p4_unloop)
{
    RP += 3;
}

/** UNTIL ( cond -- )
 * ends an control-loop, see => BEGIN and compare with => WHILE
 */
FCode (p4_until)
{
    p4_Q_pairs (DEST_MAGIC);
    FX_COMPILE1 (p4_until);
    FX (p4_backward_resolve);
}
P4COMPILES (p4_until, p4_if_execution,
	  P4_SKIPS_OFFSET, P4_UNTIL_STYLE);

/** VARIABLE ( 'name' -- )
 * => CREATE a new variable, so that everytime the variable is
 * name, the address is returned for using with => @ and => !
 * - be aware that in FIG-forth VARIABLE did take an argument
 * being the initial value. ANSI-forth does different here.
 */
FCode (p4_variable)
{
    FX (p4_create);
    FX_COMMA (0);
}

/** WHILE ( cond -- )
 * middle part of a => BEGIN .. => WHILE .. => REPEAT 
 * control-loop - if cond is true the code-piece up to => REPEAT
 * is executed which will then jump back to => BEGIN - and if
 * the cond is null then => WHILE will branch to right after
 * the => REPEAT
 * (compare with => UNTIL that forms a => BEGIN .. => UNTIL loop)
 */
FCode (p4_while)
{
    p4_Q_pairs (DEST_MAGIC);
    *--SP = DEST_MAGIC;
    FX_COMPILE1 (p4_while);
    FX (p4_ahead);
    FX (p4_two_swap);
}
P4COMPILES (p4_while, p4_if_execution,
  P4_SKIPS_OFFSET, P4_WHILE_STYLE);

/** WORD ( delimiter-char -- here-addr )
 * read the next => SOURCE section (thereby moving => >IN ) up
 * to the point reaching $delimiter-char - the text is placed
 * at => HERE - where you will find a counted string. You may
 * want to use => PARSE instead.
 */
FCode (p4_word)
{
    *SP = (p4cell) p4_word ((char) *SP);
}

/** XOR ( a b -- ab )
 * return the bitwise-or of the two arguments - it may be unsafe
 * use it on logical values. beware.
 */
FCode (p4_xor)
{
    SP[1] ^= SP[0];
    SP++;
}

/** [ ( -- )
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

/** ['] ( [name] -- ) immediate
 * will place the execution token of the following word into
 * the dictionary. See => ' for non-compiling variant.
 */
FCode (p4_bracket_tick)
{
    FX (p4_tick);

    _FX_STATESMART_Q_COMP;
    if (STATESMART)
    {
	FX_COMPILE1 (p4_bracket_tick);
	FX (p4_comma);
    }
}
P4COMPILES (p4_bracket_tick, p4_literal_execution,
  P4_SKIPS_NOTHING, P4_DEFAULT_STYLE);

/** [CHAR] ( [word] -- char )
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
	FX_COMPILE1 (p4_bracket_char);
	FX (p4_comma);
    }
}
P4COMPILES (p4_bracket_char, p4_literal_execution,
  P4_SKIPS_CELL, P4_DEFAULT_STYLE);

/** ] ( -- )
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

/** .( ( [message<closeparen>] -- )
 * print the message to the screen while reading a file. This works
 * too while compiling, so you can whatch the interpretation/compilation
 * to go on. Some Forth-implementations won't even accept a => ." message"
 * outside compile-mode while the (current) pfe does.
 */
FCode (p4_dot_paren)
{
    char *p;
    p4ucell n;

    switch (SOURCE_ID)
    {
     case -1:
     case 0:
         p4_parse (')', &p, &n);
         p4_type (p, n);
         break;
     default:
         while (!p4_parse (')', &p, &n))
         {
             p4_type (p, n);
             if (!p4_refill ())
                 return;
             FX (p4_cr);
         }
         p4_type (p, n);
    }
}

/** .R ( val prec -- ) 
 * print with precision - that is to fill
 * a field of the give prec-with with 
 * right-aligned number from the converted value
 */
FCode (p4_dot_r)
{
    FX (p4_to_r);
    FX (p4_s_to_d);
    FX (p4_r_from);
    FX (p4_d_dot_r);
}

/** '0<>' ( value -- cond )
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

/** 0> ( value -- cond )
 * return value greater than zero
 simulate:    : 0> 0 > ;
 */
FCode (p4_zero_greater)
{
    *SP = P4_FLAG (*SP > 0);
}

/** 2>R ( a,a -- )
 * save a double-cell value onto the return-stack, see => >R
 */
FCode (p4_two_to_r)
{
    RP_PUSH (SP[1]);
    RP_PUSH (SP[0]);
    SP += 2;
}

/** 2R> ( -- a,a )
 * pop back a double-cell value from the return-stack, see => R>
 * and the earlier used => 2>R
 */
FCode (p4_two_r_from)
{
    SP -= 2;
    SP[0] = RP_POP ();
    SP[1] = RP_POP ();
}

/** 2R@ ( -- a,a )
 * fetch a double-cell value from the return-stack, that had been
 * previously been put there with => 2>R - see => R@
 */
FCode (p4_two_r_fetch)
{
    SP -= 2;
    SP[0] = FX_RP[0];
    SP[1] = FX_RP[1];
}


#define NONAME_MAGIC MAKE_MAGIC('N','N','A','M')
FCode (p4_colon_noname_EXIT)
{
    p4_Q_pairs(NONAME_MAGIC);
    PFE.semicolon_code = (void*) FX_POP;
    PFE.locals = (void*) FX_POP; 
    PFE.state = FX_POP;
    /* leave pointer to colon_RT */
}
/** :NONAME ( -- cs.value )
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
    FX_COMMA (p4_colon_RT_);
    FX_PUSH(PFE.state);  PFE.state = P4_TRUE; 
    FX_PUSH(PFE.locals); PFE.locals = NULL;
    FX_PUSH(PFE.semicolon_code); 
    PFE.semicolon_code = PFX(p4_colon_noname_EXIT);
    FX_PUSH(NONAME_MAGIC);
}

/** '<>' ( a b -- cond )
 * return true if a and b are not equal, see => = 
 */
FCode (p4_not_equals)
{
    SP[1] = P4_FLAG (SP[0] != SP[1]);
    SP++;
}

/** ((?DO)) ( a b -- )
 * execution compiled by => ?DO
 */
FCode (p4_Q_do_execution)
{
    if (SP[0] == SP[1])		/* if limits are equal */
    {   SP += 2; FX_BRANCH; }	/* drop them and branch */
    else
        FX (p4_do_execution);	/* else like DO */
}

/** ?DO ( end start -- ) .. LOOP
 * start a control-loop just like => DO - but don't execute
 * atleast once. Instead jump over the code-piece if the loop's
 * variables are not in a range to allow any loop.
 */
FCode (p4_Q_do)
{
    FX_COMPILE1 (p4_Q_do);
    FX (p4_forward_mark);
    FX_PUSH (LOOP_MAGIC);
}
P4COMPILES (p4_Q_do, p4_Q_do_execution,
  P4_SKIPS_OFFSET, P4_DO_STYLE);

/** AGAIN ( -- )
 * ends an infinite loop, see => BEGIN and compare with
 * => WHILE
 */
FCode (p4_again)
{
    p4_Q_pairs (DEST_MAGIC);
    FX_COMPILE1 (p4_again);
    FX (p4_backward_resolve);
}
P4COMPILES (p4_again, p4_else_execution,
	  P4_SKIPS_OFFSET, P4_AGAIN_STYLE);

/** ((C")) ( -- bstring )
 * execution compiled by => C" string"
 */ 
FCode (p4_c_quote_execution)
{
    *--SP = (p4cell) IP;
    FX_SKIP_STRING;
}

/** 'C"' ( [string<">] -- bstring )
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
        FX_COMPILE1 (p4_c_quote);
        p4_parse_comma('"');
    }else{
        FX (p4_s_quote);
        FX (p4_drop);
        --*SP;
    }
}
P4COMPILES (p4_c_quote, p4_c_quote_execution,
  P4_SKIPS_STRING, P4_DEFAULT_STYLE);

/** CASE ( comp-value -- comp-value )
 * start a CASE construct that ends at => ENDCASE
 * and compares the value on stack at each => OF place
 */
FCode (p4_case)
{
    FX_COMPILE1 (p4_case);
    FX_PUSH (CSP);
    CSP = SP;
    FX_PUSH (CASE_MAGIC);
}
P4COMPILES (p4_case, p4_noop, P4_SKIPS_NOTHING, P4_CASE_STYLE);

/** COMPILE, ( xt -- )
 * place the execution-token on stack into the dictionary - in
 * traditional forth this is not even the least different than
 * a simple => , but in call-threaded code there's a big 
 * difference - so COMPILE, is the portable one. Unlike 
 * => COMPILE , => [COMPILE] and => POSTPONE this word does
 * not need the xt to have actually a name, see => :NONAME
 */
FCode (p4_compile_comma)
{
    FX_COMMA (*SP++);
}

/** CONVERT ( a b -- a b ) 
 * digit conversion, obsolete, superseded by => >NUMBER
 */
FCode (p4_convert)
{
    p4ucell n = UINT_MAX;

    SP[0] = (p4cell) 
        p4_to_number ((char *) SP[0] + 1, &n, 
          (p4udcell *) &SP[1], BASE);
}

/** ENDCASE ( comp-value -- )
 * ends a => CASE construct that may surround multiple sections of
 * => OF ... => ENDOF code-portions. The => ENDCASE has to resolve the
 * branches that are necessary at each => ENDOF to point to right after
 * => ENDCASE
 */
FCode (p4_endcase)
{
    p4_Q_pairs (CASE_MAGIC);
    FX_COMPILE1 (p4_endcase);
    while (SP < CSP)
        FX (p4_forward_resolve);
    CSP = (p4cell *) FX_POP;
}
P4COMPILES (p4_endcase, p4_drop,
  P4_SKIPS_NOTHING, P4_ENDCASE_STYLE);

/** ENDOF ( -- ) 
 * resolve the branch need at the previous => OF to mark
 * a code-piece and leave with an unconditional branch
 * at the next => ENDCASE (opened by => CASE )
 */
FCode (p4_endof)
{
    p4_Q_pairs (OF_MAGIC);
    FX_COMPILE1 (p4_endof);
    FX (p4_forward_mark);
    FX (p4_swap);
    FX (p4_forward_resolve);
    FX_PUSH (CASE_MAGIC);
}
P4COMPILES (p4_endof, p4_else_execution,
  P4_SKIPS_OFFSET, P4_ENDOF_STYLE);

/** ERASE ( ptr len -- )
 * fill an area will zeros.
 2000 CREATE DUP ALLOT ERASE
 */
FCode (p4_erase)
{
    memset ((void *) SP[1], 0, SP[0]);
    SP += 2;
}

/** EXPECT ( str-adr str-len -- ) 
 * input handling, see => WORD and => PARSE and => QUERY
 * the input string is placed at str-adr and its length
 in => SPAN - this word is superceded by => ACCEPT
 */
FCode (p4_expect)
{
    PFE.execute (PFE.expect);
}

/** HEX ( -- )
 * set the input/output => BASE to hexadecimal
 simulate:        : HEX 16 BASE ! ;
 */
FCode (p4_hex)
{
    BASE = 16;
}

struct marker_RT
{
    void* forget_address;
    /* see the section in p4_thread, aka the type of PFE */
    p4char* fence;
    char* last;
    Wordl* voc_link;
    Wordl* context[ORDER_LEN];
    Wordl* only;
    Wordl* current;
    Wordl* dforder[ORDER_LEN];
};

/** ((MARKER)) ( -- )
 * runtime compiled by => MARKER
 */ 
FCode (p4_marker_RT)
{
    struct marker_RT* RT = (void*) WP_PFA;
    /* restore p4_thread variables that affect the dict/order */
    PFE.fence = RT->fence;
    PFE.last = RT->last;
    /* # PFE.voc_link = RT->voc_link; implicitly in forget */
    memcpy(PFE.context, RT->context, ORDER_LEN*sizeof(void*));
    PFE.only = RT->only;
    PFE.current = RT->current;
    memcpy(PFE.dforder, RT->dforder, ORDER_LEN*sizeof(void*));  

    p4_forget (RT->forget_address); /* will set the PFE.dp */
}

/** MARKER ( 'name' -- )
 * create a named marker that you can use to => FORGET ,
 * running the created word will reset the dict/order variables
 * to the state of creation of this name.
 */
FCode (p4_marker)
{
    struct marker_RT* RT;
    char* forget_address = PFE.dp;
    p4_header (p4_marker_RT_, 0);
    RT = (void*) PFE.dp;
    PFE.dp += sizeof(*RT);

    RT->forget_address = forget_address; /* PFE.dp restore */
    /* fill the rest of the dict restores */
    RT->fence = PFE.fence;
    RT->last = PFE.last;
    /* # RT->voc_link = PFE.voc_link; implicitly in forget */
    memcpy(RT->context, PFE.context, ORDER_LEN*sizeof(void*));
    RT->only = PFE.only;
    RT->current = PFE.current;
    memcpy(RT->dforder, PFE.dforder, ORDER_LEN*sizeof(void*));  
}

/** NIP ( a b -- b )
 * drop the value under the top of stack, inverse of => TUCK
 simulate:        : NIP SWAP DROP ;
 */
FCode (p4_nip)
{
    SP[1] = SP[0];
    SP++;
}

/** ((OF)) ( check val -- check )
 * execution compiled by => OF
 */
FCode (p4_of_execution)
{
    if (SP[0] != SP[1])		/* tos equals second? */
    { SP += 1; FX_BRANCH; }	/* no: drop top, branch */
    else
    { SP += 2; IP++; }		/* yes: drop both, don't branch */
}

/** OF ( comp-value case-value -- comp-value ) .. ENDOF
 * compare the case-value placed lately with the comp-value 
 * being available since => CASE - if they are equal run the 
 * following code-portion up to => ENDOF after which the
 * case-construct ends at the next => ENDCASE
 */
FCode (p4_of)
{
    p4_Q_pairs (CASE_MAGIC);
    FX_COMPILE1 (p4_of);
    FX (p4_forward_mark);
    FX_PUSH (OF_MAGIC);
}
P4COMPILES (p4_of, p4_of_execution,
  P4_SKIPS_OFFSET, P4_OF_STYLE);

/** PAD ( -- addr ) 
 * transient buffer region 
 */
FCode (p4_pad)
{
    FX_PUSH (p4_PAD);
}

/** PARSE ( buffer-start buffer-count delim-char -- )
 * parse a piece of input (not much unlike WORD) and place
 * it into the given buffer. The difference with word is
 * also that WORD would first skip any delim-char while
 * PARSE does not and thus may yield that one.
 */
FCode (p4_parse)
{
    char delim = *SP;

    --SP;

#if 0
    switch (SOURCE_ID)
    {
     case -1:
     case 0:
         p4_parse (delim, (char **) &SP[1], (p4ucell *) &SP[0]);
         break;
     default:
         while (!p4_parse (delim, (char **) &SP[1], (p4ucell *) &SP[0])
           && p4_refill ());
         break;
    }
#else
    p4_parse (delim, (char **) &SP[1], (p4ucell *) &SP[0]);
#endif
}

/** PICK ( n -- value )
 * pick the nth value from under the top of stack and push it
 * note that
   0 PICK -> DUP         1 PICK -> OVER
 */
FCode (p4_pick)		
{
    *SP = SP[*SP + 1];
}

/** REFILL ( -- flag ) 
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

/** RESTORE-INPUT ( xn ... x1 -- )
 * inverse of => SAVE-INPUT
 */
FCode (p4_restore_input)
{
    if (*SP++ != sizeof (Iframe) / sizeof (p4cell))
        p4_throw (P4_ON_ARG_TYPE);

    SP = (p4cell *) p4_restore_input (SP);
    *--SP = 0;
}

/** ROLL ( xn xm ... x1 n -- xm ... x1 xn )
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

/** SAVE-INPUT ( -- xn .. x1 )
 * fetch the current state of the input-channel which
 * may be restored with => RESTORE-INPUT later
 */
FCode (p4_save_input)
{
    SP = (p4cell *) p4_save_input (SP);
    FX_PUSH  (sizeof (Iframe) / sizeof (p4cell));
}

/** ((TO)) ( value -- )
 * execution compiled by => TO
 */ 
FCode (p4_to_execution)
{
    *TO_BODY (*IP++) = *SP++;
}

int 
p4_tick_local(p4xt* xt)
{
    int n;
    char* p = p4_word (' ');
    int l = *(p4char*) p++;
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

/** TO ( value [name] -- )
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
            FX_COMMA (n);
        }else{
            FX_COMPILE1 (p4_to);
            FX_COMMA (xt);
        }
    }else{
        p4_tick (&xt);
        *TO_BODY (xt) = *SP++;
    }
}
P4COMPILES2 (p4_to, p4_to_execution, p4_to_local_execution,
	   P4_SKIPS_TO_TOKEN, P4_DEFAULT_STYLE);

/** TUCK ( a b -- b a b )
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

/** U.R ( value prec -- )
 * print right-aligned in a prec-field, treat value to
 * be unsigned as opposed to => .R
 */
FCode (p4_u_dot_r)
{
    *--SP = 0;
    FX (p4_swap);
    FX (p4_d_dot_r);
}

/** U> ( a b -- ab )
 * unsigned comparison of a and b, see => >
 */
FCode (p4_u_greater_than)
{
    SP[1] = P4_FLAG ((p4ucell) SP[1] > (p4ucell) SP[0]);
    SP++;
}

/** UNUSED ( -- val )
 * return the number of cells that are left to be used
 * above => HERE
 */
FCode (p4_unused)
{
    FX_PUSH (PFE.dictlimit - DP);
}

/** ((VALUE)) ( -- value )
 * runtime compiled by => VALUE
 */ 
FCode (p4_value_RT)
{
    *--SP = WP_PFA[0];
}

/** VALUE ( value 'name' -- )
 * => CREATE a word and initialize it with value. Using it
 * later will push the value back onto the stack. Compare with
 * => VARIABLE and => CONSTANT - look also for => LOCALS| and
 * => VAR
 */
FCode (p4_value)
{
    p4_header (p4_value_RT_, 0);
    FX_COMMA (*SP++);
}

/** WITHIN ( a b c -- cond )
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

/** [COMPILE] ( [word] -- )
 * while compiling the next word will be place in the currently
 * defined word no matter if that word is immediate (like => IF )
 * - compare with => COMPILE and => POSTPONE
 */
FCode (p4_bracket_compile)
{
    FX (p4_Q_comp);
    FX (p4_tick);
    FX (p4_comma);
}

/** "\\" ( [comment<eol>] -- )
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

/** '"' ( [string<">] -- bstring ) or perhaps ( [..] -- str-ptr str-len )
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
          
/** BL ( -- value )
 * a quick constant returning the blank character in host encoding,
 * in ascii that is 0x20
 */

/** TIB ( -- value )
 * traditional variable for forth terminal I/O system.
 * (Terminal Input Buffer)    ( => TIB => SPAN => SOURCE => BLK )
 */

/** SOURCE ( -- value )
 * traditional variable for forth terminal I/O system.
 *                            ( => TIB => SPAN => SOURCE => BLK )
 */

/** SPAN ( -- value )
 * traditional variable for forth terminal I/O system.
 *                            ( => TIB => SPAN => SOURCE => BLK )
 */

/** SPAN ( -- value )
 * traditional variable for forth terminal I/O system.
 *                            ( => TIB => SPAN => #SPAN => SOURCE => BLK )
 */

/* -------------------------------------------------------------- */

static FCode (p__stack_cells)
{
    FX_PUSH (PFE_set.stack_size);
}

static FCode (p__return_stack_cells)
{
    FX_PUSH (PFE_set.ret_stack_size);
}

P4_LISTWORDS (core) =
{
    /* core words */
    CO ("!",		p4_store),
    CO ("#",		p4_sh),
    CO ("#>",		p4_sh_greater),
    CO ("#S",		p4_sh_s),
    CO ("'",		p4_tick),
    CI ("(",		p4_paren),
    CO ("*",		p4_star),
    CO ("*/",		p4_star_slash),
    CO ("*/MOD",	p4_star_slash_mod),
    CO ("+",		p4_plus),
    CO ("+!",		p4_plus_store),
    CS ("+LOOP",	p4_plus_loop),
    CO (",",		p4_comma),
    CO ("-",		p4_minus),
    CO (".",		p4_dot),
    CS (".\"",		p4_dot_quote),
    CO ("/",		p4_slash),
    CO ("/MOD",		p4_slash_mod),
    CO ("0<",		p4_zero_less),
    CO ("0=",		p4_zero_equal),
    CO ("1+",		p4_one_plus),
    CO ("1-",		p4_one_minus),
    CO ("2!",		p4_two_store),
    CO ("2*",		p4_two_star),
    CO ("2/",		p4_two_slash),
    CO ("2@",		p4_two_fetch),
    CO ("2DROP",	p4_two_drop),
    CO ("2DUP",		p4_two_dup),
    CO ("2OVER",	p4_two_over),
    CO ("2SWAP",	p4_two_swap),
    CO (":",		p4_colon),
    CS (";",		p4_semicolon),
    CO ("<",		p4_less_than),
    CO ("<#",		p4_less_sh),
    CO ("=",		p4_equals),
    CO (">",		p4_greater_than),
    CO (">BODY",	p4_to_body),
    DV (">IN",		input.to_in),
    CO (">NUMBER",	p4_to_number),
    CO (">R",		p4_to_r),
    CO ("?DUP",		p4_Q_dup),
    CO ("@",		p4_fetch),
    CO ("ABS",		p4_abs),
    CO ("ACCEPT",	p4_accept),
    CO ("ALIGN",	p4_align),
    CO ("ALIGNED",	p4_aligned),
    CO ("ALLOT",	p4_allot),
    CO ("AND",		p4_and),
    DV ("BASE",		base),
    CS ("BEGIN",	p4_begin),
    OC ("BL",		' '),
    CO ("C!",		p4_c_store),
    CO ("C,",		p4_c_comma),
    CO ("C@",		p4_c_fetch),
    CO ("CELL+",	p4_cell_plus),
    CO ("CELLS",	p4_cells),
    CO ("CHAR",		p4_char),
    CO ("CHAR+",	p4_char_plus),
    CO ("CHARS",	p4_chars),
    CO ("CONSTANT",	p4_constant),
    CO ("COUNT",	p4_count),
    CO ("CR",		p4_cr),
    CO ("CREATE",	p4_create),
    CO ("DECIMAL",	p4_decimal),
    CO ("DEPTH",	p4_depth),
    CS ("DO",		p4_do),
    CS ("DOES>",	p4_does),
    CO ("DROP",		p4_drop),
    CO ("DUP",		p4_dup),
    CS ("ELSE",		p4_else),
    CO ("EMIT",		p4_emit),
    CO ("ENVIRONMENT?",	p4_environment_Q),
    CO ("EVALUATE",	p4_evaluate),
    CO ("EXECUTE",	p4_execute),
    CS ("EXIT",		p4_exit),
    CO ("FILL",		p4_fill),
    CO ("FIND",		p4_find),
    CO ("FM/MOD",	p4_f_m_slash_mod),
    CO ("HERE",		p4_here),
    CO ("HOLD",		p4_hold),
    CO ("I",		p4_i),
    CS ("IF",		p4_if),
    CO ("IMMEDIATE",	p4_immediate),
    CO ("INVERT",	p4_invert),
    CO ("J",		p4_j),
    CO ("KEY",		p4_key),
    CO ("LEAVE",	p4_leave),
    CS ("LITERAL",	p4_literal),
    CS ("LOOP",		p4_loop),
    CO ("LSHIFT",	p4_l_shift),
    CO ("M*",		p4_m_star),
    CO ("MAX",		p4_max),
    CO ("MIN",		p4_min),
    CO ("MOD",		p4_mod),
    CO ("MOVE",		p4_move),
    CO ("NEGATE",	p4_negate),
    CO ("OR",		p4_or),
    CO ("OVER",		p4_over),
    CS ("POSTPONE",	p4_postpone),
    CO ("QUIT",		p4_quit),
    CO ("R>",		p4_r_from),
    CO ("R@",		p4_r_fetch),
    CI ("RECURSE",	p4_recurse),
    CS ("REPEAT",	p4_repeat),
    CO ("ROT",		p4_rot),
    CO ("RSHIFT",	p4_r_shift),
    CS ("S\"",		p4_s_quote),
    CO ("S>D",		p4_s_to_d),
    CO ("SIGN",		p4_sign),
    CO ("SM/REM",	p4_s_m_slash_rem),
    CO ("SOURCE",	p4_source),
    CO ("SPACE",	p4_space),
    CO ("SPACES",	p4_spaces),
    DV ("STATE",	state),
    CO ("SWAP",		p4_swap),
    CS ("THEN",		p4_then),
    CO ("TYPE",		p4_type),
    CO ("U.",		p4_u_dot),
    CO ("U<",		p4_u_less_than),
    CO ("UM*",		p4_u_m_star),
    CO ("UM/MOD",	p4_u_m_slash_mod),
    CO ("UNLOOP",	p4_unloop),
    CS ("UNTIL",	p4_until),
    CO ("VARIABLE",	p4_variable),
    CS ("WHILE",	p4_while),
    CO ("WORD",		p4_word),
    CO ("XOR",		p4_xor),
    CI ("[",		p4_left_bracket),
    CS ("[']",		p4_bracket_tick),
    CS ("[CHAR]",	p4_bracket_char),
    CO ("]",		p4_right_bracket),
    /* core extension words */
    DV ("#TIB",		input.number_tib),
    CI (".(",		p4_dot_paren),
    CO (".R",		p4_dot_r),
    CO ("0<>",		p4_zero_not_equals),
    CO ("0>",		p4_zero_greater),
    CO ("2>R",		p4_two_to_r),
    CO ("2R>",		p4_two_r_from),
    CO ("2R@",		p4_two_r_fetch),
    CO (":NONAME",	p4_colon_noname),
    CO ("<>",		p4_not_equals),
    CS ("?DO",		p4_Q_do),
    CS ("AGAIN",	p4_again),
    CS ("C\"",		p4_c_quote),
    CS ("CASE",		p4_case),
    CO ("COMPILE,",	p4_compile_comma),
    CO ("CONVERT",	p4_convert),
    CS ("ENDCASE",	p4_endcase),
    CS ("ENDOF",	p4_endof),
    CO ("ERASE",	p4_erase),
    CO ("EXPECT",	p4_expect),
    OC ("FALSE",	P4_FALSE),
    CO ("HEX",		p4_hex),
    CO ("MARKER",	p4_marker),
    CO ("NIP",		p4_nip),
    CS ("OF",		p4_of),
    CO ("PAD",		p4_pad),
    CO ("PARSE",	p4_parse),
    CO ("PICK",		p4_pick),
    CO ("QUERY",	p4_query),
    CO ("REFILL",	p4_refill),
    CO ("RESTORE-INPUT",p4_restore_input),
    CO ("ROLL",		p4_roll),
    CO ("SAVE-INPUT",	p4_save_input),
    DC ("SOURCE-ID",	input.source_id),
    DV ("SPAN",		span),
    DC ("TIB",		input.tib),
    CS ("TO",		p4_to),
    OC ("TRUE",		P4_TRUE),
    CO ("TUCK",		p4_tuck),
    CO ("U.R",		p4_u_dot_r),
    CO ("U>",		p4_u_greater_than),
    CO ("UNUSED",	p4_unused),
    CO ("VALUE",	p4_value),
    CO ("WITHIN",	p4_within),
    CI ("[COMPILE]",	p4_bracket_compile),
    CI ("\\",		p4_backslash),

    /*extra (simple-quote is non-portable) */
    CI ("\"",           p4_quote),

    P4_INTO ("ENVIRONMENT", 0),
    /* enviroment hints (testing for -EXT will mark this wordset as present) */
    P4_OCON ("CORE-EXT",		1994 ),
    P4_OCON ("/COUNTED-STRING",		UCHAR_MAX ),
    P4_OCON ("/HOLD",			MIN_HOLD ),
    P4_OCON ("/PAD",			MIN_PAD ),
    P4_OCON ("ADDRESS-UNIT-BITS",	CHAR_BIT ),
    P4_OCON ("FLOORED",			P4_TRUE ),
    P4_OCON ("MAX-CHAR",		UCHAR_MAX ),
    P4_OCON ("MAX-N",			CELL_MAX ),
    P4_OCON ("MAX-U",			UCELL_MAX ),
    P4_FXCO ("STACK-CELLS",		p__stack_cells),
    P4_FXCO ("RETURN-STACK-CELLS",	p__return_stack_cells),
};
P4_COUNTWORDS (core, "Core words + extensions");

/*@}*/

