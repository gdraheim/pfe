#ifndef _PFE_CORE_EXT_H
#define _PFE_CORE_EXT_H 984413840
/* generated 2001-0312-1717 ../../pfe/../mk/Make-H.pl ../../pfe/core-ext.c */

#include <pfe/incl-ext.h>

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

#ifdef __cplusplus
extern "C" {
#endif




/** ! ( val addr -- )
 */
extern P4_CODE (p4_store);

/** # ( n.n -- n.n' ) 
 * see also => HOLD for old-style forth-formatting words
 * and => PRINTF of the C-style formatting - this word
 * divides the argument by => BASE and add it to the
 * picture space - it should be used inside of => <#
 * and => #>
 */
extern P4_CODE (p4_sh);

/** #> ( n.n -- str-addr str-len ) 
 * see also => HOLD for old-style forth-formatting words
 * and => PRINTF of the C-style formatting - this word
 * drops the argument and returns the picture space
 * buffer
 */
extern P4_CODE (p4_sh_greater);

/** #S ( n.n -- n.n ) f
 * see also => HOLD for old-style forth-formatting words
 * and => PRINTF of the C-style formatting - this word
 * does repeat the word => # for a number of times, until
 * the argument becomes zero. Hence the result is always
 * null - it should be used inside of => <# and => #>
 */
extern P4_CODE (p4_sh_s);

/** "'" ( 'name' -- xt ) 
 * return the execution token of the following name. This word
 * is _not_ immediate and may not do what you expect in
 * compile-mode. See => ['] and => '> - note that in FIG-forth
 * the word of the same name had returned the PFA (not the CFA)
 * and was immediate/smart, so beware when porting forth-code
 * from FIG-forth to ANSI-forth.
 */
extern P4_CODE (p4_tick);

/** (  ( 'comment<closeparen>' -- ) 
 * eat everything up to the next closing paren - treat it
 * as a comment.
 */
extern P4_CODE (p4_paren);

/** * ( a b -- a*b ) 
 * return the multiply of the two args
 */
extern P4_CODE (p4_star);

/** "*\/" ( a b c -- a*b/c ) 
 * regard the b/c as element Q - this word
 * has an advantage over the sequence of => *
 * and => / by using an intermediate double-cell
 * value
 */
extern P4_CODE (p4_star_slash);

/** "*\/MOD" ( a b c -- m n )
 * has an adavantage over the sequence of => *
 * and => /MOD by using an intermediate double-cell
 * value.
 */
extern P4_CODE (p4_star_slash_mod);

/** + ( a b -- a+b ) 
 * return the sum of the two args
 */
extern P4_CODE (p4_plus);

/** +! ( val addr -- ) 
 * add val to the value found in addr
 simulate:
   : +! TUCK @ + SWAP ! ;
 */
extern P4_CODE (p4_plus_store);

/** ((+LOOP)) ( increment -- ) 
 * compiled by => +LOOP
 */
extern P4_CODE (p4_plus_loop_execution);

/** +LOOP ( increment -- ) 
 * compile => ((+LOOP)) which will use the increment
 * as the loop-offset instead of just 1. See the
 * => DO and => LOOP construct.
 */
extern P4_CODE (p4_plus_loop);

/** , ( val -- ) 
 * store the value in the dictionary
 simulate:
   : , DP  1 CELLS DP +!  ! ;
 */
extern P4_CODE (p4_comma);

/** - ( a b -- a-b ) 
 * return the difference of the two arguments
 */
extern P4_CODE (p4_minus);

/** . ( val -- ) 
 * print the numerical value to stdout - uses => BASE
 */
extern P4_CODE (p4_dot);

/** ((.")) ( -- ) _skip_string_
 * compiled by => ." string"
 */
extern P4_CODE (p4_dot_quote_execution);

/** ." ( [string<">] -- ) 
 * print the string to stdout
 */
extern P4_CODE (p4_dot_quote);

/** / ( a b  -- a/b ) 
 * return the quotient of the two arguments
 */
extern P4_CODE (p4_slash);

/** /MOD ( a b -- m n ) 
 * divide a and b and return both
 * quotient n and remainder m
 */
extern P4_CODE (p4_slash_mod);

/** 0< ( val -- cond ) 
 * return a flag that is true if val is lower than zero
 simulate:
  : 0< 0 < ;
 */
extern P4_CODE (p4_zero_less);

/** 0= ( val -- cond ) 
 * return a flag that is true if val is just zero
 simulate:
  : 0= 0 = ;
 */
extern P4_CODE (p4_zero_equal);

/** 1+ ( val -- val+1 ) 
 * return the value incremented by one
 simulate:
  : 1+ 1 + ;
 */
extern P4_CODE (p4_one_plus);

/** 1- ( val -- val-1 ) 
 * return the value decremented by one
 simulate:
   : 1- 1 - ;
 */
extern P4_CODE (p4_one_minus);

/** 2! ( a,a addr -- ) 
 * double-cell store 
 */
extern P4_CODE (p4_two_store);

/** 2* ( a -- a*2 ) 
 * multiplies the value with two - but it
 * does actually use a shift1 to be faster
 simulate:
  : 2* 2 * ; ( canonic) : 2* 1 LSHIFT ; ( usual)
 */
extern P4_CODE (p4_two_star);

/** 2/ ( a -- a/2 ) 
 * divides the value by two - but it
 * does actually use a shift1 to be faster
 simulate:
  : 2/ 2 / ; ( canonic) : 2/ 1 RSHIFT ; ( usual)
 */
extern P4_CODE (p4_two_slash);

/** 2@ ( addr -- a,a ) 
 * double-cell fetch
 */
extern P4_CODE (p4_two_fetch);

/** 2DROP ( a b -- ) 
 * double-cell drop, also used to drop two items
 */
extern P4_CODE (p4_two_drop);

/** 2DUP ( a,a -- a,a a,a ) 
 * double-cell duplication, also used to duplicate
 * two items
 simulate:
   : 2DUP OVER OVER ; ( wrong would be : 2DUP DUP DUP ; !!) 
 */
extern P4_CODE (p4_two_dup);

/** 2OVER ( a,a b,b -- a,a b,b a,a ) 
 * double-cell over, see => OVER and => 2DUP
 simulate:
   : 2OVER SP@ 2 CELLS + 2@ ;
 */
extern P4_CODE (p4_two_over);

/** 2SWAP ( a,a b,b -- b,b a,a ) 
 * double-cell swap, see => SWAP and => 2DUP
 simulate:
   : 2SWAP LOCALS| B1 B2 A1 A2 | B2 B1 A2 A1 ;
 */
extern P4_CODE (p4_two_swap);

/** (NEST) ( -- ) 
 * compiled by => :
 */
extern P4_CODE (p4_colon_RT);

extern P4_CODE (p4_colon_EXIT);

/** : ( 'name' -- ) 
 * create a header for a nesting word and go to compiling
 * mode then. This word is usually ended with => ; but
 * the execution of the resulting colon-word can also 
 * return with => EXIT
 */
extern P4_CODE (p4_colon);

/** ((;)) ( -- ) 
 * compiled by => ; and maybe => ;AND --
 * it will perform an => EXIT
 */
extern P4_CODE (p4_semicolon_execution);

/** ; ( -- ) 
 * compiles => ((;)) which does => EXIT the current
 * colon-definition. It does then end compile-mode
 * and returns to execute-mode. See => : and => :NONAME
 */
extern P4_CODE (p4_semicolon);

/** < ( a b -- cond ) 
 * return a flag telling if a is lower than b
 */
extern P4_CODE (p4_less_than);

/** <# ( -- ) 
 * see also => HOLD for old-style forth-formatting words
 * and => PRINTF of the C-style formatting - this word
 * does initialize the pictured numeric output space.
 */
extern P4_CODE (p4_less_sh);

/** = ( a b -- cond )
 * return a flag telling if a is equal to b
 */
extern P4_CODE (p4_equals);

/** > ( a b -- cond )
 * return a flag telling if a is greater than b
 */
extern P4_CODE (p4_greater_than);

/** >BODY ( addr -- addr' )
 * adjust the execution-token (ie. the CFA) to point
 * to the parameter field (ie. the PFA) of a word.
 * this is implementation dependent and is usually
 * either "1 CELLS +" or "2 CELLS +"
 */
extern P4_CODE (p4_to_body);

/** >NUMBER ( a,a str-adr str-len -- a,a' str-adr' str-len) 
 * try to convert a string into a number, and place
 * that number at a,a respeciting => BASE
 */
extern P4_CODE (p4_to_number);

/** >R ( value -- )
 * save the value onto the return stack. The return
 * stack must be returned back to clean state before
 * an exit and you should note that the return-stack
 * is also touched by the => DO ... => WHILE loop.
 * Use => R> to clean the stack and => R@ to get the 
 * last value put by => >R
 */
extern P4_CODE (p4_to_r);

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
extern P4_CODE (p4_Q_dup);

/** @ ( addr -- value )
 * fetch the value from the variables address
 */
extern P4_CODE (p4_fetch);

/** ABS ( value -- value' )
 * return the absolute value
 */
extern P4_CODE (p4_abs);

/** ACCEPT ( a n -- n' ) 
 * get a string from terminal into the named input 
 * buffer, returns the number of bytes being stored
 * in the buffer. May provide line-editing functions.
 */
extern P4_CODE (p4_accept);

/** ALIGN ( -- )
 * will make the dictionary aligned, usually to a
 * cell-boundary, see => ALIGNED
 */
extern P4_CODE (p4_align);

/** ALIGNED ( addr -- addr' )
 * uses the value (being usually a dictionary-address)
 * and increment it to the required alignment for the
 * dictionary which is usually in => CELLS - see also
 * => ALIGN
 */
extern P4_CODE (p4_aligned);

/** ALLOT ( count -- )
 * make room in the dictionary - usually called after
 * a => CREATE word like => VARIABLE or => VALUE
 * to make for an array of variables. Does not 
 * initialize the space allocated from the dictionary-heap.
 * The count is in bytes - use => CELLS ALLOT to allocate 
 * a field of cells.
 */
extern P4_CODE (p4_allot);

/** AND ( val mask -- val' )
 * mask with a bitwise and - be careful when applying
 * it to logical values.
 */
extern P4_CODE (p4_and);

/** BEGIN ( -- ) compile-time: ( -- cs-marker )
 * start a control-loop, see => WHILE and => REPEAT
 */
extern P4_CODE (p4_begin);

/** C! ( value address -- )
 * store the byte-value at address, see => !
 */
extern P4_CODE (p4_c_store);

/** C, ( value -- )
 * store a new byte-value in the dictionary, implicit 1 ALLOT,
 * see => ,
 */
extern P4_CODE (p4_c_comma);

/** C@ ( addr -- value )
 * fetch a byte-value from the address, see => @
 */
extern P4_CODE (p4_c_fetch);

/** CELL+ ( value -- value' )
 * adjust the value by adding a single Cell's width
 * - the value is often an address or offset, see => CELLS
 */
extern P4_CODE (p4_cell_plus);

/** CELLS ( value -- value' )
 * scale the value by the sizeof a Cell
 * the value is then often applied to an address or
 * fed into => ALLOT
 */
extern P4_CODE (p4_cells);

/** CHAR ( 'word' -- value )
 * return the (ascii-)value of the following word's
 * first character. 
 */
extern P4_CODE (p4_char);

/** CHAR+ ( value -- value' )
 * increment the value by the sizeof one char
 * - the value is often a pointer or an offset,
 * see => CHARS
 */
extern P4_CODE (p4_char_plus);

/** CHARS ( value -- value' )
 * scale the value by the sizeof a char
 * - the value is then often applied to an address or
 * fed into => ALLOT (did you expect that sizeof(p4char)
 * may actually yield 2 bytes?)
 */
extern P4_CODE (p4_chars);

/** ((CONSTANT)) ( -- )
 * runtime compiled by => CONSTANT
 */
extern P4_CODE (p4_constant_RT);

/** CONSTANT ( value 'name' -- )
 * => CREATE a new word with runtime => ((CONSTANT))
 * so that the value placed here is returned everytime
 * the constant's name is used in code. See => VALUE
 * for constant-like names that are expected to change
 * during execution of the program. In a ROM-able
 * forth the CONSTANT-value may get into a shared
 * ROM-area and is never copied to a RAM-address.
 */
extern P4_CODE (p4_constant);

/** COUNT ( counted-string -- string-pointer string-length )
 * usually before calling => TYPE
 */
extern P4_CODE (p4_count);

/** CR ( -- )
 * print a carriage-return/new-line on stdout
 */
extern P4_CODE (p4_cr);

/** ((VAR)) ( -- pfa )
 * the runtime compiled by => CREATE which
 * is the usual thing to do in => VARIABLE
 */
extern P4_CODE (p4_create_RT);

/** CREATE ( 'name' -- )
 * create a name with runtime => ((VAR)) so
 * that everywhere the name is used the pfa
 * of the name's body is returned. This word
 * is not immediate and is usually used in
 * the first part of a => DOES> defining
 * word.
 */
extern P4_CODE (p4_create);

/** DECIMAL ( -- )
 * set the => BASE to 10
 simulate:
   : DECIMAL 10 BASE ! ;
 */
extern P4_CODE (p4_decimal);

/** DEPTH ( -- value )
 * return the depth of the parameter stack before
 * the call, see => SP@ - the return-value is in => CELLS
 */
extern P4_CODE (p4_depth);

/** ((DO)) ( end start -- )
 * compiled by => DO
 */
extern P4_CODE (p4_do_execution);

/** DO ( end start -- ) ... LOOP
 *  pushes $end and $start onto the return-stack ( => >R )
 *  and starts a control-loop that ends with => LOOP or
 *  => +LOOP and may get a break-out with => LEAVE . The
 *  loop-variable can be accessed with => I
 */
extern P4_CODE (p4_do);

/** ((DOES>)) ( -- pfa )
 * runtime compiled by DOES>
 */
extern P4_CODE (p4_does_defined_RT);

/** (DOES>) ( -- pfa )
 * execution compiled by => DOES>
 */
extern P4_CODE (p4_does_execution);

/** DOES> ( -- pfa )
 * does twist the last => CREATE word to carry
 * the => (DOES>) runtime. That way, using the
 * word will execute the code-piece following => DOES>
 * where the pfa of the word is already on stack.
 * (note: FIG option will leave pfa+cell since does-rt is stored in pfa)
 */
extern P4_CODE (p4_does);

/** DROP ( a -- )
 * just drop the word on the top of stack, see => DUP
 */
extern P4_CODE (p4_drop);

/** DUP ( a -- a a )
 * duplicate the cell on top of the stack - so the
 * two topmost cells have the same value (they are
 * equal w.r.t => = ) , see => DROP for the inverse
 */
extern P4_CODE (p4_dup);

/** ((ELSE)) ( -- )
 * execution compiled by => ELSE - just a simple
 * => BRANCH
 */
extern P4_CODE (p4_else_execution);

/** ELSE ( -- )
 * will compile an => ((ELSE)) => BRANCH that performs an 
 * unconditional jump to the next => THEN - and it resolves 
 * an => IF for the non-true case
 */
extern P4_CODE (p4_else);

/** EMIT ( char -- )
 * print the char-value on stack to stdout
 */
extern P4_CODE (p4_emit);

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
extern P4_CODE (p4_environment_Q);

/** EVALUATE ( str-ptr str-len -- ) 
 * => INTERPRET the given string, => SOURCE id
 * is -1 during that time.
 */
extern P4_CODE (p4_evaluate);

/** EXECUTE ( xt -- )
 * run the execution-token on stack - this will usually
 * trap if it was null for some reason, see => >EXECUTE
 simulate:
  : EXECUTE >R EXIT ;
 */
extern P4_CODE (p4_execute);

/** EXIT ( -- )
 * will unnest the current colon-word so it will actually
 * return the word calling it. This can be found in the
 * middle of a colon-sequence between => : and => ;
 */
extern P4_CODE (p4_exit);

/** FILL ( mem-addr mem-length char -- )
 * fill a memory area with the given char, does now
 * simply call memset()
 */
extern P4_CODE (p4_fill);

/** FIND ( bstring -- cfa|bstring -1|0|1 )
 * looks into the current search-order and tries to find
 * the name string as the name of a word. Returns its
 * execution-token or the original-bstring if not found,
 * along with a flag-like value that is zero if nothing
 * could be found. Otherwise it will be 1 if the word had
 * been immediate, -1 otherwise.
 */
extern P4_CODE (p4_find);

/** FM/MOD ( n1.n1 n2 -- m n )
 * divide the double-cell value n1 by n2 and return
 * both (floored) quotient n and remainder m 
 */
extern P4_CODE (p4_f_m_slash_mod);

/** HERE ( -- dp-value )
 * used with => WORD and many compiling words
 simulate:   : HERE DP @ ;
 */
extern P4_CODE (p4_here);

/** HOLD ( char -- ) 
 * the old-style forth-formatting system -- this
 * word adds a char to the picutred output string.
 */
extern P4_CODE (p4_hold);

/** I ( -- value )
 * returns the index-value of the innermost => DO .. => LOOP
 */
extern P4_CODE (p4_i);

/** ((IF)) ( -- )
 * execution word compiled by => IF - just some simple => ?BRANCH
 */
extern P4_CODE (p4_if_execution);

/** IF ( value -- ) .. THEN
 * checks the value on the stack (at run-time, not compile-time)
 * and if true executes the code-piece between => IF and the next
 * => ELSE or => THEN . Otherwise it has compiled a branch over
 * to be executed if the value on stack had been null at run-time.
 */
extern P4_CODE (p4_if);

/** IMMEDIATE ( -- )
 * make the => LATEST word immediate, see also => CREATE
 */
extern P4_CODE (p4_immediate);

/** INVERT ( value -- value' )
 * make a bitwise negation of the value on stack.
 * see also => NEGATE
 */
extern P4_CODE (p4_invert);

/** J ( -- value )
 * get the current => DO ... => LOOP index-value being
 * the not-innnermost. (the second-innermost...)
 * see also for the other loop-index-values at
 * => I and => K
 */
extern P4_CODE (p4_j);

/** KEY ( -- char ) 
 * return a single character from the keyboard - the
 * key is not echoed.
 */
extern P4_CODE (p4_key);

/** LEAVE ( -- )
 * quit the innermost => DO .. => LOOP  - it does even
 * clean the return-stack and branches to the place directly
 * after the next => LOOP
 */
extern P4_CODE (p4_leave);

/** ((LIT)) ( -- value )
 * execution compiled by => LITERAL
 */
extern P4_CODE (p4_literal_execution);

/** LITERAL ( value -- )
 * if compiling this will take the value from the compiling-stack 
 * and puts in dictionary so that it will pop up again at the
 * run-time of the word currently in creation. This word is used
 * in compiling words but may also be useful in making a hard-constant
 * value in some code-piece like this:
 : DCELLS [ 2 CELLS ] LITERAL * ; ( will save a multiplication at runtime)
 */
extern P4_CODE (p4_literal);

/** ((LOOP)) ( -- )
 * execution compiled by => LOOP
 */
extern P4_CODE (p4_loop_execution);

/** LOOP ( -- )
 * resolves a previous => DO thereby compiling => ((LOOP)) which
 * does increment/decrement the index-value and branch back if
 * the end-value of the loop has not been reached.
 */
extern P4_CODE (p4_loop);

/** LSHIFT ( value shift-val -- value' )
 * does a bitwise left-shift on value
 */
extern P4_CODE (p4_l_shift);

/** M* ( a b -- m,m )
 * multiply and return a double-cell result
 */
extern P4_CODE (p4_m_star);

/** MAX ( a b -- c )
 * return the maximum of a and b
 */
extern P4_CODE (p4_max);

/** MIN ( a b -- c )
 * return the minimum of a and b
 */
extern P4_CODE (p4_min);

/** MOD ( a b -- c )
 * return the module of "a mod b"
 */
extern P4_CODE (p4_mod);

/** MOVE ( from to length -- ) 
 * memcpy an area
 */
extern P4_CODE (p4_move);

/** NEGATE ( value -- value' )
 * return the arithmetic negative of the (signed) cell
 simulate:   : NEGATE -1 * ;
 */
extern P4_CODE (p4_negate);

/** OR ( a b -- ab )
 * return the bitwise OR of a and b - unlike => AND this
 * is usually safe to use on logical values
 */
extern P4_CODE (p4_or);

/** OVER ( a b -- a b a )
 * get the value from under the top of stack. The inverse
 * operation would be => TUCK
 */
extern P4_CODE (p4_over);

/** ((POSTPONE)) ( -- )
 * execution compiled by => POSTPONE
 */
extern P4_CODE (p4_postpone_execution);

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
extern P4_CODE (p4_postpone);

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
extern P4_CODE (p4_quit);

/** R> ( -- value )
 * get back a value from the return-stack that had been saved
 * there using => >R . This is the traditional form of a local
 * var space that could be accessed with => R@ later. If you
 * need more local variables you should have a look at => LOCALS|
 * which does grab some space from the return-stack too, but names
 * them the way you like.
 */
extern P4_CODE (p4_r_from);

/** R@ ( -- value )
 * fetch the (upper-most) value from the return-stack that had
 * been saved there using => >R - This is the traditional form of a local
 * var space. If you need more local variables you should have a 
 * look at => LOCALS| , see also => >R and => R> .
 */
extern P4_CODE (p4_r_fetch);

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
extern P4_CODE (p4_recurse);

/** REPEAT ( -- )
 * ends an unconditional loop, see => BEGIN
 */
extern P4_CODE (p4_repeat);

/** ROT ( a b c -- b c a )
 * rotates the three uppermost values on the stack,
 * the other direction would be with => -ROT - please
 * have a look at => LOCALS| and => VAR that can avoid 
 * its use.
 */
extern P4_CODE (p4_rot);

/** RSHIFT ( value shift-val -- value' )
 * does a bitwise logical right-shift on value
 * (ie. the value is considered to be unsigned)
 */
extern P4_CODE (p4_r_shift);

/** ((S")) ( -- string-address string-length )
 * execution compiled by => S"
 */
extern P4_CODE (p4_s_quote_execution);

/** 'S"' ( [string<">] -- string-address string-length)
 * if compiling then place the string into the currently
 * compiled word and on execution the string pops up
 * again as a double-cell value yielding the string's address
 * and length. To be most portable this is the word to be
 * best being used. Compare with =>'C"' and non-portable => "
 */
extern P4_CODE (p4_s_quote);

/** S>D ( a -- a,a' )
 * signed extension of a single-cell value to a double-cell value
 */
extern P4_CODE (p4_s_to_d);

/** SIGN ( a -- )
 * put the sign of the value into the hold-space, this is
 * the forth-style output formatting, see => HOLD
 */
extern P4_CODE (p4_sign);

/** SM/REM ( a.a b -- c d ) 
 * see => /MOD or => FM/MOD or => UM/MOD or => SM/REM
 */
extern P4_CODE (p4_s_m_slash_rem);

/** SOURCE ( -- buffer IN-offset )
 *  the current point of interpret can be gotten through SOURCE.
 *  The buffer may flag out TIB or BLK or a FILE and IN gives
 *  you the offset therein. Traditionally, if the current SOURCE
 *  buffer is used up, => REFILL is called that asks for another
 *  input-line or input-block. This scheme would have made it
 *  impossible to stretch an [IF] ... [THEN] over different blocks,
 *  unless [IF] does call => REFILL
 */
extern P4_CODE (p4_source);

/** SPACE ( -- )
 * print a single space to stdout, see => SPACES
 simulate:    : SPACE  BL EMIT ;
 */
extern P4_CODE (p4_space);

/** SPACES ( n -- )
 * print n space to stdout, actually a loop over n calling => SPACE ,
 * but the implemenation may take advantage of printing chunks of
 * spaces to speed up the operation.
 */
extern P4_CODE (p4_spaces);

/** SWAP ( a b -- b a )
 * exchanges the value on top of the stack with the value beneath it
 */
extern P4_CODE (p4_swap);

/** THEN ( -- )
 * does resolve a branch coming from either => IF or => ELSE
 */
extern P4_CODE (p4_then);

/** TYPE ( string-pointer string-length -- )
 * prints the string-buffer to stdout, see => COUNT and => EMIT
 */
extern P4_CODE (p4_type);

/** U. ( value )
 * print unsigned number to stdout
 */
extern P4_CODE (p4_u_dot);

/** U< ( a b -- cond )
 * unsigned comparison, see => <
 */
extern P4_CODE (p4_u_less_than);

/** UM* ( a b -- c,c )
 * unsigned multiply returning double-cell value
 */
extern P4_CODE (p4_u_m_star);

/** UM/MOD ( a b -- c,c )
 * see => /MOD and => SM/REM
 */
extern P4_CODE (p4_u_m_slash_mod);

/** UNLOOP ( -- )
 * drop the => DO .. => LOOP runtime variables from the return-stack,
 * usually used just in before an => EXIT call. Using this multiple
 * times can unnest multiple nested loops.
 */
extern P4_CODE (p4_unloop);

/** UNTIL ( cond -- )
 * ends an control-loop, see => BEGIN and compare with => WHILE
 */
extern P4_CODE (p4_until);

/** VARIABLE ( 'name' -- )
 * => CREATE a new variable, so that everytime the variable is
 * name, the address is returned for using with => @ and => !
 * - be aware that in FIG-forth VARIABLE did take an argument
 * being the initial value. ANSI-forth does different here.
 */
extern P4_CODE (p4_variable);

/** WHILE ( cond -- )
 * middle part of a => BEGIN .. => WHILE .. => REPEAT 
 * control-loop - if cond is true the code-piece up to => REPEAT
 * is executed which will then jump back to => BEGIN - and if
 * the cond is null then => WHILE will branch to right after
 * the => REPEAT
 * (compare with => UNTIL that forms a => BEGIN .. => UNTIL loop)
 */
extern P4_CODE (p4_while);

/** WORD ( delimiter-char -- here-addr )
 * read the next => SOURCE section (thereby moving => >IN ) up
 * to the point reaching $delimiter-char - the text is placed
 * at => HERE - where you will find a counted string. You may
 * want to use => PARSE instead.
 */
extern P4_CODE (p4_word);

/** XOR ( a b -- ab )
 * return the bitwise-or of the two arguments - it may be unsafe
 * use it on logical values. beware.
 */
extern P4_CODE (p4_xor);

/** [ ( -- )
 * leave compiling mode - often used inside of a colon-definition
 * to make fetch some very constant value and place it into the
 * currently compiled colon-defintion with => , or => LITERAL
 * - the corresponding unleave word is => ]
 */
extern P4_CODE (p4_left_bracket);

/** ['] ( [name] -- ) immediate
 * will place the execution token of the following word into
 * the dictionary. See => ' for non-compiling variant.
 */
extern P4_CODE (p4_bracket_tick);

/** [CHAR] ( [word] -- char )
 * in compile-mode, get the (ascii-)value of the first charachter
 * in the following word and compile it as a literal so that it
 * will pop up on execution again. See => CHAR and forth-83 => ASCII
 */
extern P4_CODE (p4_bracket_char);

/** ] ( -- )
 * enter compiling mode - often used inside of a colon-definition
 * to end a previous => [ - you may find a  => , or => LITERAL
 * nearby in example texts.
 */
extern P4_CODE (p4_right_bracket);

/** .( ( [message<closeparen>] -- )
 * print the message to the screen while reading a file. This works
 * too while compiling, so you can whatch the interpretation/compilation
 * to go on. Some Forth-implementations won't even accept a => ." message"
 * outside compile-mode while the (current) pfe does.
 */
extern P4_CODE (p4_dot_paren);

/** .R ( val prec -- ) 
 * print with precision - that is to fill
 * a field of the give prec-with with 
 * right-aligned number from the converted value
 */
extern P4_CODE (p4_dot_r);

/** '0<>' ( value -- cond )
 * returns a logical-value saying if the value was not-zero.
 * This is most useful in turning a numerical value into a 
 * boolean value that can be fed into bitwise words like
 * => AND and => XOR - a simple => IF or => WHILE doesn't
 * need it actually.
 */
extern P4_CODE (p4_zero_not_equals);

/** 0> ( value -- cond )
 * return value greater than zero
 simulate:    : 0> 0 > ;
 */
extern P4_CODE (p4_zero_greater);

/** 2>R ( a,a -- )
 * save a double-cell value onto the return-stack, see => >R
 */
extern P4_CODE (p4_two_to_r);

/** 2R> ( -- a,a )
 * pop back a double-cell value from the return-stack, see => R>
 * and the earlier used => 2>R
 */
extern P4_CODE (p4_two_r_from);

/** 2R@ ( -- a,a )
 * fetch a double-cell value from the return-stack, that had been
 * previously been put there with => 2>R - see => R@
 */
extern P4_CODE (p4_two_r_fetch);

extern P4_CODE (p4_colon_noname_EXIT);

/** :NONAME ( -- cs.value )
 * start a colon nested-word but do not use => CREATE - so no name
 * is given to the colon-definition that follows. When the definition
 * is finished at the corresponding => ; the start-address (ie.
 * the execution token) can be found on the outer cs.stack that may
 * be stored used elsewhere then.
 */
extern P4_CODE (p4_colon_noname);

/** '<>' ( a b -- cond )
 * return true if a and b are not equal, see => = 
 */
extern P4_CODE (p4_not_equals);

/** ((?DO)) ( a b -- )
 * execution compiled by => ?DO
 */
extern P4_CODE (p4_Q_do_execution);

/** ?DO ( end start -- ) .. LOOP
 * start a control-loop just like => DO - but don't execute
 * atleast once. Instead jump over the code-piece if the loop's
 * variables are not in a range to allow any loop.
 */
extern P4_CODE (p4_Q_do);

/** AGAIN ( -- )
 * ends an infinite loop, see => BEGIN and compare with
 * => WHILE
 */
extern P4_CODE (p4_again);

/** ((C")) ( -- bstring )
 * execution compiled by => C" string"
 */
extern P4_CODE (p4_c_quote_execution);

/** 'C"' ( [string<">] -- bstring )
 * in compiling mode place the following string in the current
 * word and return the address of the counted string on execution.
 * (in exec-mode use a => POCKET and leave the bstring-address of it),
 * see => S" string" and the non-portable => " string"
 */
extern P4_CODE (p4_c_quote);

/** CASE ( comp-value -- comp-value )
 * start a CASE construct that ends at => ENDCASE
 * and compares the value on stack at each => OF place
 */
extern P4_CODE (p4_case);

/** COMPILE, ( xt -- )
 * place the execution-token on stack into the dictionary - in
 * traditional forth this is not even the least different than
 * a simple => , but in call-threaded code there's a big 
 * difference - so COMPILE, is the portable one. Unlike 
 * => COMPILE , => [COMPILE] and => POSTPONE this word does
 * not need the xt to have actually a name, see => :NONAME
 */
extern P4_CODE (p4_compile_comma);

/** CONVERT ( a b -- a b ) 
 * digit conversion, obsolete, superseded by => >NUMBER
 */
extern P4_CODE (p4_convert);

/** ENDCASE ( comp-value -- )
 * ends a => CASE construct that may surround multiple sections of
 * => OF ... => ENDOF code-portions. The => ENDCASE has to resolve the
 * branches that are necessary at each => ENDOF to point to right after
 * => ENDCASE
 */
extern P4_CODE (p4_endcase);

/** ENDOF ( -- ) 
 * resolve the branch need at the previous => OF to mark
 * a code-piece and leave with an unconditional branch
 * at the next => ENDCASE (opened by => CASE )
 */
extern P4_CODE (p4_endof);

/** ERASE ( ptr len -- )
 * fill an area will zeros.
 2000 CREATE DUP ALLOT ERASE
 */
extern P4_CODE (p4_erase);

/** EXPECT ( str-adr str-len -- ) 
 * input handling, see => WORD and => PARSE and => QUERY
 * the input string is placed at str-adr and its length
 in => SPAN - this word is superceded by => ACCEPT
 */
extern P4_CODE (p4_expect);

/** HEX ( -- )
 * set the input/output => BASE to hexadecimal
 simulate:        : HEX 16 BASE ! ;
 */
extern P4_CODE (p4_hex);

/** ((MARKER)) ( -- )
 * runtime compiled by => MARKER
 */
extern P4_CODE (p4_marker_RT);

/** MARKER ( 'name' -- )
 * create a named marker that you can use to => FORGET ,
 * running the created word will reset the dict/order variables
 * to the state of creation of this name.
 */
extern P4_CODE (p4_marker);

/** NIP ( a b -- b )
 * drop the value under the top of stack, inverse of => TUCK
 simulate:        : NIP SWAP DROP ;
 */
extern P4_CODE (p4_nip);

/** ((OF)) ( check val -- check )
 * execution compiled by => OF
 */
extern P4_CODE (p4_of_execution);

/** OF ( comp-value case-value -- comp-value ) .. ENDOF
 * compare the case-value placed lately with the comp-value 
 * being available since => CASE - if they are equal run the 
 * following code-portion up to => ENDOF after which the
 * case-construct ends at the next => ENDCASE
 */
extern P4_CODE (p4_of);

/** PAD ( -- addr ) 
 * transient buffer region 
 */
extern P4_CODE (p4_pad);

/** PARSE ( buffer-start buffer-count delim-char -- )
 * parse a piece of input (not much unlike WORD) and place
 * it into the given buffer. The difference with word is
 * also that WORD would first skip any delim-char while
 * PARSE does not and thus may yield that one.
 */
extern P4_CODE (p4_parse);

/** PICK ( n -- value )
 * pick the nth value from under the top of stack and push it
 * note that
   0 PICK -> DUP         1 PICK -> OVER
 */
extern P4_CODE (p4_pick);

/** REFILL ( -- flag ) 
 * try to get a new input line from the => SOURCE and set
 * => >IN accordingly. Return a flag if sucessful, which is
 * always true if the current input comes from a 
 * terminal and which is always false if the current input
 * comes from => EVALUATE - and may be either if the 
 * input comes from a file
 */
extern P4_CODE (p4_refill);

/** RESTORE-INPUT ( xn ... x1 -- )
 * inverse of => SAVE-INPUT
 */
extern P4_CODE (p4_restore_input);

/** ROLL ( xn xm ... x1 n -- xm ... x1 xn )
 * the extended form of => ROT
    2 ROLL -> ROT
 */
extern P4_CODE (p4_roll);

/** SAVE-INPUT ( -- xn .. x1 )
 * fetch the current state of the input-channel which
 * may be restored with => RESTORE-INPUT later
 */
extern P4_CODE (p4_save_input);

/** ((TO)) ( value -- )
 * execution compiled by => TO
 */
extern P4_CODE (p4_to_execution);

/** TO ( value [name] -- )
 * set the parameter field of name to the value, this is used
 * to change the value of a => VALUE and it can be also used
 * to change the value of => LOCALS|
 */
extern P4_CODE (p4_to);

/** TUCK ( a b -- b a b )
 * shove the top-value under the value beneath. See => OVER
 * and => NIP
 simulate:    : TUCK  SWAP OVER ;
 */
extern P4_CODE (p4_tuck);

/** U.R ( value prec -- )
 * print right-aligned in a prec-field, treat value to
 * be unsigned as opposed to => .R
 */
extern P4_CODE (p4_u_dot_r);

/** U> ( a b -- ab )
 * unsigned comparison of a and b, see => >
 */
extern P4_CODE (p4_u_greater_than);

/** UNUSED ( -- val )
 * return the number of cells that are left to be used
 * above => HERE
 */
extern P4_CODE (p4_unused);

/** ((VALUE)) ( -- value )
 * runtime compiled by => VALUE
 */
extern P4_CODE (p4_value_RT);

/** VALUE ( value 'name' -- )
 * => CREATE a word and initialize it with value. Using it
 * later will push the value back onto the stack. Compare with
 * => VARIABLE and => CONSTANT - look also for => LOCALS| and
 * => VAR
 */
extern P4_CODE (p4_value);

/** WITHIN ( a b c -- cond )
 * a widely used word, returns ( b <= a && a < c ) so
 * that is very useful to check an index a of an array
 * to be within range b to c
 */
extern P4_CODE (p4_within);

/** [COMPILE] ( [word] -- )
 * while compiling the next word will be place in the currently
 * defined word no matter if that word is immediate (like => IF )
 * - compare with => COMPILE and => POSTPONE
 */
extern P4_CODE (p4_bracket_compile);

/** "\\" ( [comment<eol>] -- )
 * eat everything up to the next end-of-line so that it is
 * getting ignored by the interpreter. 
 */
extern P4_CODE (p4_backslash);

/** '"' ( [string<">] -- bstring ) or perhaps ( [..] -- str-ptr str-len )
 *  This is the non-portable word which is why the ANSI-committee
 *  on forth has created the two other words, namely => S" and => C" ,
 *  since each implementation (and in pfe configurable) uses another
 *  runtime behaviour. FIG-forth did return bstring which is the configure
 *  default for pfe.
 */
extern P4_CODE (p4_quote);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
