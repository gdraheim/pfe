#ifndef _PFE_FORTH_USUAL_EXT_H
#define _PFE_FORTH_USUAL_EXT_H 990144519
/* generated 2001-0518-0208 ../../pfe/../mk/Make-H.pl ../../pfe/forth-usual-ext.c */

#include <pfe/incl-ext.h>

/** 
 * -- usually implemented words.
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE               @(#) %derived_by: guidod %
 *  @version %version: 5.10 %
 *    (%date_modified: Mon Mar 12 10:32:22 2001 %)
 *
 *  @description
 *              There are lots of useful words that do not appear
 *              in any standard. This wordset defines some of them.
 */

#ifdef __cplusplus
extern "C" {
#endif




/** [DEFINED]             ( [name] -- flag )
 *  Search the dictionary for _name_. If _name_ is found,
 *  return TRUE; otherwise return FALSE. Immediate for use in
 *  definitions.
  
 * This word will actually return what => FIND returns (the NFA). 
 * does check for the word using find (so it does not throw like => ' )
 * and puts it on stack. As it is immediate it does work in compile-mode
 * too, so it places its argument in the cs-stack then. This is most
 * useful with a directly following => [IF] clause, so that sth. like
 * an <c>[IFDEF] word</c> can be simulated through <c>[DEFINED] word [IF]</c>

 : [DEFINED] BL WORD FIND NIP ; IMMEDIATE
 */
extern P4_CODE (p4_bracket_defined);

/** [UNDEFINED]          ( [name] -- flag )
 *  Search the dictionary for _name_. If _name_ is found,
 *  return FALSE; otherwise return TRUE. Immediate for use in
 *  definitions.
 *
 *  see => [DEFINED]
 */
extern P4_CODE (p4_bracket_undefined);

/** C+!  ( n addr -- )
 *  Add the low-order byte of _n_ to the byte at _addr_,
 *  removing both from the stack.
 */
extern P4_CODE (p4_c_plus_store);

/** BOUNDS                ( str len -- str+len str )
 *  Convert _str len_ to range for DO-loop.
 : BOUNDS  ( str len -- str+len str )  OVER + SWAP ;
 */
extern P4_CODE (p4_bounds);

/** OFF                   ( addr  -- )
 *  Store 0 at _addr_. See `ON`.
  : OFF  ( addr -- )  0 SWAP ! ;
 */
extern P4_CODE (p4_off);

/** ON                    ( addr -- )
 *  Store -1 at _addr_. See `OFF`.
  : ON  ( addr -- )  -1 SWAP ! ;
 */
extern P4_CODE (p4_on);

/** APPEND                ( str len add2 -- )
 *  Append string _str len_ to the counted string at _addr_.
 *  AKA `+PLACE`.
 : APPEND   2DUP 2>R  COUNT +  SWAP MOVE ( ) 2R> C+! ;
 */
extern P4_CODE (p4_append);

/** APPEND-CHAR           ( char addr -- )
 *  Append _char_ to the counted string at _addr_.
 : APPEND-CHAR   DUP >R  COUNT  DUP 1+ R> C!  +  C! ;
 */
extern P4_CODE (p4_append_char);

/** PLACE                 ( str len addr -- )
 *  Place the string _str len_ at _addr_, formatting it as a
 *  counted string.
 : PLACE  2DUP 2>R  1+ SWAP  MOVE  2R> C! ;
 : PLACE  2DUP C!   1+ SWAP CMOVE ;
 */
extern P4_CODE (p4_place);

/** STRING,               ( str len -- )
 *  Store a string in data space as a counted string.
 : STRING, HERE  OVER 1+  ALLOT  PLACE ;
 */
extern P4_CODE (p4_string_comma);

/** PARSE,                    ( "chars<">" -- )
 *  Store a char-delimited string in data space as a counted
 *  string. As seen in Bawd's
 : ," [CHAR] " PARSE  STRING, ; IMMEDIATE
 *
 * this implementation is much different from Bawd's
 : PARSE, PARSE STRING, ;
 */
extern P4_CODE (p4_parse_comma);

/** PARSE,"                   ( "chars<">" -- )
 *  Store a quote-delimited string in data space as a counted
 *  string.
 : ," [CHAR] " PARSE  STRING, ; IMMEDIATE
 *
 * implemented here as
 : PARSE," [CHAR] " PARSE, ; IMMEDIATE
 */
extern P4_CODE (p4_parse_comma_quote);

/** PARSE-WORD ( "chars" -- c-addr u )
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
extern P4_CODE (p4_parse_word);

/** PERFORM ( addr -- ? ) 
  simulate:
    : PERFORM  @ EXECUTE 
  */
extern P4_CODE (p4_perform);

/** ?LEAVE ( cond -- )
 * leave a (innermost) loop if condition is true
 */
extern P4_CODE (p4_question_leave);

/** NOOP ( -- )
 * do nothing, used as a place-holder where 
 * an execution word is needed
 */
extern P4_CODE (p4_noop);

/** RP@ ( -- addr ) 
 * returns the return stack pointer 
 example:
   : R@ RP@ @ ;
 */
extern P4_CODE (p4_r_p_fetch);

/** RP! ( addr -- ) 
 * sets the return stack pointer, reverse of => RP@
 */
extern P4_CODE (p4_r_p_store);

/** SP! ( ... addr -- ) 
 * sets the stack pointer, reverse of => SP@
 */
extern P4_CODE (p4_s_p_store);

/** -ROT ( a b c -- c a b )
 * inverse of => ROT
 */
extern P4_CODE (p4_dash_rot);

/** CSET ( n addr -- ) 
 * set bits in byte at given address 
 simulate:
   : CSET  TUCK @ SWAP OR SWAP ! ;
 */
extern P4_CODE (p4_c_set);

/** CRESET ( n addr -- ) 
 *  reset bits in byte at given address 
 simulate:
   : CRESET  TUCK @ SWAP NOT AND SWAP ! ;
 */
extern P4_CODE (p4_c_reset);

/** CTOGGLE ( n addr -- ) 
 * toggle bits in byte at given address 
 simulate:
   : CTOGGLE  TUCK @ SWAP XOR SWAP ! ;
 */
extern P4_CODE (p4_c_toggle);

/** 3DUP                ( x y z -- x y z x y z )
 *  Copy top three elements on the stack onto top of stack.
 : 3DUP   THIRD THIRD THIRD ;
 *
 * or
 : 3DUP  3 PICK 3 PICK 3 PICK ;
 */
extern P4_CODE (p4_three_dup);

/** 3DROP               ( x y z -- )
 *  Drop the top three elements from the stack.
 : 3DROP   DROP 2DROP ;
 */
extern P4_CODE (p4_three_drop);

/** 4DUP ( a b c d -- a b c d a b c d )
 simulate:
  : 4DUP  4 PICK 4 PICK 4 PICK 4 PICK ;
 */
extern P4_CODE (p4_four_dup);

/** 4DROP               ( x y z -- )
 *  Drop the top three elements from the stack.
 : 4DROP   2DROP 2DROP ;
 */
extern P4_CODE (p4_four_drop);

/** UPC ( c1 -- c2 ) 
 * convert a single character to upper case 
   : UPC  >R _toupper ;
 */
extern P4_CODE (p4_upc);

/** UPPER ( addr cnt -- ) 
 * convert string to upper case 
 simulate:
   : UPPER  0 DO  DUP I +  DUP C@ UPC SWAP C!  LOOP  DROP ;
 */
extern P4_CODE (p4_upper);

/** LOWER ( addr cnt -- ) 
 * convert string to lower case
 * This is not in L&P's F83 but provided for symmetry 
 simulate:
   : LOWER  0 DO  DUP I +  DUP C@ >R _tolower SWAP C!  LOOP  DROP ;
 */
extern P4_CODE (p4_lower);

/** ASCII ( [word] -- val )
 * state smart version of => CHAR or => [CHAR] resp.
 simulate:
   : ASCII  [COMPILE] [CHAR] 
            STATE @ IF [COMPILE] LITERAL THEN ;
 */
extern P4_CODE (p4_ascii);

/** CONTROL ( [word] -- val )
 * see =>'ASCII', but returns char - '@' 
 simulate:
   : CONTROL  [COMPILE] [CHAR]  [CHAR] @ -  
              STATE @ IF [COMPILE] LITERAL THEN ;
 */
extern P4_CODE (p4_control);

/** NUMBER? ( addr -- d flag ) 
 * convert counted string to number - used in inner interpreter 
 * ( => INTERPRET ), flags if conversion was successful
 example:
   BL WORD  HERE NUMBER? 0= IF ." not a number " THEN . 
 */
extern P4_CODE (p4_number_question);

/** VOCS ( -- )
 * list all vocabularies in the system
 simulate:
   : VOCS VOC-LINK @ BEGIN DUP WHILE
                           DUP BODY> >NAME .NAME
                           ->VOC-LINK @
                     REPEAT DROP ; 
 */
extern P4_CODE (p4_vocs);

/** ((DEFER)) ( -- )
 * runtime of => DEFER words
 */
extern P4_CODE (p4_defer_RT);

/** DEFER ( 'word' -- )
 * create a new word with ((DEFER))-semantics
 simulate:
   : DEFER  CREATE 0, DOES> ( the ((DEFER)) runtime ) 
      @ ?DUP IF EXECUTE THEN ;
   : DEFER  DEFER-RT HEADER 0 , ;
 *
 * declare as <c>"DEFER deferword"</c>  <br>
 * and set as <c>"['] executionword IS deferword"</c>
 * (in pfe, you can also use <c>TO deferword</c> to set the execution)
 */
extern P4_CODE (p4_defer);

extern P4_CODE (p4_is_execution);

/** IS ( xt-value [word] -- )
 * set a => DEFER word
 : IS ' 
   STATE @ IF LITERAL, POSTPONE >DOES-BODY POSTPONE ! 
   ELSE >DOES-BODY ! THEN 
 ; IMMEDIATE
 */
extern P4_CODE (p4_is);

/** @EXECUTE ( xt -- ? )
 * same as => @ => EXECUTE , but checks for null as xt and
 * silently ignores it. Same as in most forths where defined.
 simulate:
   : @EXECUTE  @ ?DUP IF EXECUTE THEN ;
 */
extern P4_CODE (p4_fetch_execute);

/** EMITS           ( n char -- )
 *  Emit _char_ _n_ times.
 : EMITS             ( n char -- )
    SWAP 0 ?DO  DUP EMIT  LOOP DROP ;
 * also compare
 : SPACES BL EMITS ;
 : SPACE BL EMIT ;
 */
extern P4_CODE(p4_emits);

/** FILE-CHECK        ( n -- )
 *  Check for file access error.
 \ : FILE-CHECK    ( n -- )  THROW ;
 : FILE-CHECK      ( n -- )  ABORT" File Access Error " ;
 */
extern P4_CODE (p4_file_check);

/** MEMORY-CHECK      ( n -- )
 *  Check for memory allocation error.
 \ : MEMORY-CHECK  ( n -- )  THROW ;
 : MEMORY-CHECK    ( n -- )  ABORT" Memory Allocation Error " ;
 */
extern P4_CODE (p4_memory_check);

/** H#                    ( "hexnumber" -- n )
 *  Get the next word in the input stream as a hex
 *  single-number literal.  (Adopted from Open Firmware.)
 : H#  ( "hexnumber" -- n )  \  Simplified for easy porting.
    0 0 BL WORD COUNT                  
    BASE @ >R  HEX  >NUMBER  R> BASE !
        ABORT" Not Hex " 2DROP          ( n)
    STATE @ IF  POSTPONE LITERAL  THEN
    ; IMMEDIATE
 */
extern P4_CODE (p4_h_sh);

/** ++                  ( addr -- )
 *  Increment the value at _addr_.
 : ++  ( addr -- )  1 SWAP +! ;
 */
extern P4_CODE (p4_plus_plus);

/** @+                  ( addr -- addr' x )
 *  Fetch the value _x_ from _addr_, and increment the address
 *  by one cell.
 : @+  ( addr -- addr' x )  DUP CELL+ SWAP  @ ;
 */
extern P4_CODE (p4_fetch_plus);

/** !+                  ( addr x -- addr' )
 *  Store the value _x_ into _addr_, and increment the address
 *  by one cell.
 : !+  ( addr x -- addr' )  OVER !  CELL+ ;
 */
extern P4_CODE (p4_store_plus);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
