#ifndef _PFE_USEFUL_EXT_H
#define _PFE_USEFUL_EXT_H 1158897469
/* generated 2006-0922-0557 ../../pfe/../mk/Make-H.pl ../../pfe/useful-ext.c */

#include <pfe/pfe-ext.h>

/** 
 * -- useful additional primitives
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2006 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2006-09-22 04:43:03 $)
 *
 *  @description
 *              This wordset adds some additional primitives that
 *		are useful. The structure of this file follows the
 *              the example in your-ext.c, yet some of the words here
 *              must be bound statically into the main pfe-object to
 *              work out smart and nicely.
 */

#ifdef __cplusplus
extern "C" {
#endif




/** "_for:COMPILE," ( xt -- )
 *  does the work of => POSTPONE on the execution token that 
 *  you got from somewhere else - so it checks if the name
 *  (that correspond to the execution-token argument) is
 *  actually immediate, so it has to be executed to compile
 *  something, e.g. => IF or => THEN - see also => POSTPONE ,
 *  => COMPILE , => [COMPILE] , => INTERPRET
 *
 *  warning: do not use this word anymore, it is an error to
 *  compile a token where the immediate-bit must be checked.
 *  The immediate-bit is a pure header information not present
 *  with headerless words as such. Furthermore, this function
 *  is subject to POSTPONE problems as well.
 *
 *  Newer code should use =>"COMPILE," when trying to simulate
 *  a behavior similar to => POSTPONE.
 */
extern P4_CODE (p4_to_compile);

/** "($" ( [word] -- cs-token ) compile-only
 *  takes the execution token of the following word and
 *  saves it on the compile-stack. The correspondig closing
 *  => ) will then feed it into => >COMPILE - so this pair
 *  of word provides you with a prefix-operation syntax
 *  that you may have been seen in lisp-like languages.
   ($ IF ($ 0= A1 @ )) ($ THEN ." hello " )
 * Note that an opening simple => ( paren is a comment.
 */
extern P4_CODE (p4_prefix_begin);

/** ")" ( cs-token -- )
 * takes the execution-token from => ($ and compiles
 * it using => >COMPILE
 */
extern P4_CODE (p4_prefix_end);

/** "))" ( cs-token cs-token -- )
 * takes two execution-tokens from two of => ($ and compiles
 * them on after another using => >COMPILE
 simulate:
    : )) [COMPILE] ) [COMPILE] ) ; IMMEDIATE
 */
extern P4_CODE (p4_prefix_end_doubled);

/** PFE-SPRINTF ( args ... format$ dest$ -- len-dest ) 
 * just like the standard sprintf() function in C, but
 * the format is a counted string and accepts %#s to
 * be the format-symbol for a forth-counted string.
 * The result is a zeroterminated string at dest$ having
 * a length being returned. To create a forth-counted
 * string, you could use:
   variable A 256 ALLOT
   15 " example" " the %#s value is %i" A 1+ SPRINTF A C!
   A COUNT TYPE
 */
extern P4_CODE (p4_sprintf);

/** PFE-PRINTF ( args ... format$ -- )
 * uses => SPRINTF to print to a temporary 256-char buffer
 * and prints it to stdout afterwards. See the example
 * at => SPRINTF of what it does internally.
 */
extern P4_CODE (p4_printf);

/** LOADF ( "filename" -- )
 *  loads a file just like => INCLUDE but does also put
 *  a => MARKER in the => LOADED dictionary that you can
 *  do a => FORGET on to kill everything being loaded
 *  from that file.
 */
extern P4_CODE (p4_loadf);

/** "(LOADF-LOCATE)" ( xt -- nfa )
 * the implementation of => LOADF-LOCATE
 */
extern P4_CODE(p4_paren_loadf_locate);

/** LOADF-LOCATE ( "name" -- )
 * look for the filename created by => LOADF that had been
 * defining the given name. => LOADF has created a marker
 * that is <em>above</em> the => INCLUDED file and that
 * marker has a body-value just <em>below</em> the 
 * => INCLUDED file. Hence the symbol was defined during
 * => LOADF execution of that file.
 : LOADF-LOCATE ?EXEC POSTPONE ' (LOADF-LOCATE) .NAME ;
 */
extern P4_CODE(p4_loadf_locate);

/** "(;AND)" ( -- )
 * compiled by => ;AND
 */
extern P4_CODE (p4_semicolon_and_execution);

/** ";AND" ( -- )
 * For the code piece between => MAKE and => ;AND , this word
 * will do just an => EXIT . For the code outside of
 * the => MAKE construct a branch-around must be resolved then.
 */
extern P4_CODE (p4_semicolon_and);

/** "((MAKE-))" ( -- )
 * compiled by => MAKE
 */
extern P4_CODE (p4_make_to_local_execution);

/** "((MAKE))" ( -- )
 * compiled by => MAKE
 */
extern P4_CODE (p4_make_to_execution);

/** MAKE ( [word] -- ) ... ;AND
 * make a seperated piece of code between => MAKE and => ;AND 
 * and on execution of the => MAKE the named word is twisted
 * to point to this piece of code. The word is usually 
 * a => DOER but the current implementation works 
 * on => DEFER just as well, just as it does on other words who
 * expect to find an execution-token in its PFA. You could even
 * create a colon-word that starts with => NOOP and can then make
 * that colon-word be prefixed with the execution of the code piece. 
 * This => MAKE
 * does even work on => LOCALS| and => VAR but it is uncertain
 * what that is good for.
 */
extern P4_CODE (p4_make);

/** OFFSET-RT ( value -- value+offset )
 *  this runtime will add the body-value to the value at top-of-stack.
 *  used heavily in structure access words, compiled by => /FIELD
 */
extern P4_CODE (p4_offset_RT);

/** +CONSTANT ( offset "name" -- )
 * create a new offsetword. The word is created and upon execution
 * it adds the offset, ie. compiling the => OFFSET-RT runtime:
       ( address -- address+offset )
 * This word is just a convenience word, just use the word => +FIELD 
 * directly and choose a => DROP to flag the end of a current
 * offset-field declaration series. See also => /FIELD series to
 * declare simple structures which end with a final => CONSTANT to
 * memorize the complete size. The => /FIELD style is more traditional.
 */
extern P4_CODE (p4_offset_constant);

/** +FIELD ( offset "name" -- offset )
 * created a new name with an => OFFSET-RT runtime using the given offset. 
 * Leave the offset-value untouched, so it can be modified with words
 * like => CHAR+ and => CELL+ and => SFLOAT+ ; This word is the simplest way 
 * to declared structure access words in forth - the two => STRUCT modules 
 * contain a more elaborate series of words. Use this one like:
 0                        ( a fresh definition is started )
 +FIELD zapp.a+ CHAR+     ( zero offset from the base of the struct )
 +FIELD zapp.b+ CELL+     ( no alignment, starts off at 1 from base )
 +FIELD zapp+   DROP      ( store size of complete zap structure )

 0 zapp+                  ( extend the zap structure )
 +FIELD zappx.c+ CELL+    ( a new field )
 +FIELD zappx+   DROP     ( and save it again )

 CREATE zapp1  0 zapp+ ALLOT ( a way to allocate a strucutre )

 zapp2 zapp.b+ @         ( read a value from the field )
 16 zapp2 zapp.b+ !      ( store a value in there )

 * this form is not the traditional form used in forth, it is however
 * quite simple. Use the simplefield declaration with => /FIELD to
 * be compatible with traditional styles that build on top of sizeof
 * constants in forth (which are not part of the ANS Forth standard).
 */
extern P4_CODE (p4_plus_field);

/** /FIELD ( offset size "name" -- offset+size )
 * created a new => +FIELD name with an => OFFSET-RT
 * of offset. Then add the size value to the offset so that
 * the next => /FIELD declaration will start at the end of the
 * field currently declared. This word is the simplest way to
 * declared structure access words in forth - the two => STRUCT modules 
 * contain a more elaborate series of words. This one is used like:
 0                        ( a fresh definition is started )
 /CHAR /FIELD ->zapp.a    ( zero offset from the base of the struct )
 /CELL /FIELD ->zapp.b    ( no alignment, starts off at 1 from base )
 CONSTANT /zapp           ( store size of complete zap structure )

 /zapp                    ( extend the zap structure )
 /CELL /FIELD ->zappx.c   ( a new field )
 CONSTANT /zappx          ( and save it again )

 CREATE zapp1 /zapp ALLOT ( a way to allocate a strucutre )
 /zapp BUFFER: zapp2      ( another way to do it, semi-standard )

 zapp2 ->zapp.b @         ( read a value from the field )
 16 zapp2 ->zapp.b !      ( store a value in there )

 * compare also with => /CHAR => /WCHAR => /CELL => /DCELL
 * and use => +FIELD as the lowlevel word, can simulate as
 : /FIELD SWAP +FIELD + ;
 */
extern P4_CODE (p4_slash_field);

/** [NOT] ( a -- a' )
 * executes => 0= but this word is immediate so that it does 
 * affect the cs-stack while compiling rather than compiling
 * anything. This is useful just before words like => [IF] to
 * provide semantics of an <c>[IFNOT]</c>. It is most useful in
 * conjunction with "=> [DEFINED] word" as it the sequence
 * "<c>[DEFINED] word [NOT] [IF]</c>" can simulate "<c>[IFNOTDEF] word</c>"
 */
extern P4_CODE (p4_bracket_not);

/** REPLACE-IN ( to-xt from-xt n "name" -- )
 * will handle the body of the named word as a sequence of cells (or tokens) 
 * and replaces the n'th occurences of from-xt into to-xt. A negative value
 * will change all occurences. A zero value will not change any.
 */
extern P4_CODE(p4_replace_in);

/** 'X"' ( "hex-q" -- bstring ) 
 * places a counted string on stack
 * containing bytes specified by hex-string
 * - the hex string may contain spaces which will delimit the bytes
 example: 
    X" 41 42 4344" COUNT TYPE ( shows ABCD )
 */
extern P4_CODE (p4_x_quote);

/** EVALUATE-WITH ( i*x addr len xt[i*x--j*x] -- j*x ) 
 * added to be visible on the forth command line on request by MLG,
 * he has explained the usage before a lot, you can get an idea from:
    : EVALUATE ['] INTERPRET EVALUATE-WITH ;
 * The word is used internally in PFE for the loadlist evaluation of
 * the binary modules: where previously each loadercode had its own
 * CREATE-execution we do now call the original forthish CREATE-word
 * like, so bootstrapping a => VARIABLE will now call VARIABLE itself
 * and of course we need to set up the TIB-area to point to the name
 * of the variable that shall be created in the forth dictionary:
 : LOAD-WORD ( arg-value str-ptr str-len loader-code -- )
      CASE
        #LOAD-VARIABLE OF ['] VARIABLE EVALUATE-WITH ENDOF
        ....
      ENDCASE
      CLEARSTACK
 ;
 */
extern P4_CODE(p4_evaluate_with);

/** [VOCABULARY] ( "name" -- )
 * create an immediate vocabulary. Provides for basic 
 * modularization.
 : [VOCABULARY] VOCABULARY IMMEDIATE ;
 */
extern P4_CODE (p4_bracket_vocabulary);

/** [POSSIBLY] ( [name] -- ?? )
 * check if the name exists, and execute it immediatly
 * if found. Derived from POSSIBLY as seen in other forth systems.
 : [POSSIBLY] (') ?DUP IF EXECUTE THEN ; IMMEDIATE
 */
extern P4_CODE (p4_bracket_possibly);

/** [DEF] ( -- )
 * immediatly set topmost => CONTEXT voc to => CURRENT compilation voc.
 : DEF' CURRENT @ CONTEXT ! ; IMMEDIATE
 * note that in PFE most basic vocabularies are immediate, so that
 * you can use a sequence of
 FORTH ALSO  DEFINITIONS
 [DEF] : GET-FIND-3  [ANS] ['] FIND  [FIG] ['] FIND  [DEF] ['] FIND ;
 * where the first wordlist to be searched via the search order are
 * [ANS] and [FIG] and FORTH (in this order) and which may or may not 
 * yield different flavours of the FIND routine (i.e. different XTs)
 */
extern P4_CODE (p4_bracket_def);

/** CONTEXT? ( -- number )
 * GET-CONTEXT and count how many times it is in the order but
 * the CONTEXT variable itself. The returned number is therefore
 * minus one the occurences in the complete search-order.
 * usage:
   ALSO EXTENSIONS CONTEXT? [IF] PREVIOUS [THEN]
   ALSO DEF' DEFAULT-ORDER
 : CONTEXT? 
   0 LVALUE _count
   GET-ORDER 1- SWAP  LVALUE _context
   0 ?DO _context = IF 1 +TO _count THEN LOOP
   _count
 ;
 */
extern P4_CODE (p4_context_Q);

/** DEFS-ARE-CASE-SENSITIVE ( -- ) 
 * accesses => CURRENT which is generally the last wordlist that the
 * => DEFINITIONS shall go in. sets there a flag in the vocabulary-definition
 * so that words are matched case-sensitive. 
 example: 
    VOCABULARY MY-VOC  MY-VOC DEFINITIONS DEFS-ARE-CASE-SENSITIVE
 */
extern P4_CODE (p4_defs_are_case_sensitive);

/** CASE-SENSITIVE-VOC ( -- ) 
 * accesses => CONTEXT which is generally the last named => VOCABULARY .
 * sets a flag in the vocabulary-definition so that words are matched
 * case-sensitive. 
 example: 
    VOCABULARY MY-VOC  MY-VOC CASE-SENSITIVE-VOC
 * OBSOLETE! use => DEFS-ARE-CASE-SENSITIVE
 */
extern P4_CODE (p4_case_sensitive_voc);

/** DEFS-ARE-SEARCHED-ALSO ( -- )
 * binds => CONTEXT with =>'CURRENT'. If the => CURRENT => VOCABULARY is in
 * the search-order (later), then the => CONTEXT vocabulary will 
 * be searched also. If the result of this word could lead into 
 * a recursive lookup with => FIND it will throw <c>CURRENT_DELETED</c>
 * and leave the => CURRENT => VOCABULARY unaltered.
 example:
 * MY-VOC DEFINITIONS  MY-VOC-PRIVATE DEFS-ARE-SEARCHED-ALSO
 */
extern P4_CODE (p4_defs_are_searched_also);

/** [EXECUTE] ( [word] -- )
 * ticks the following word, and executes it - even in compiling mode.
 : [EXECUTE] ' EXECUTE ;
 */
extern P4_CODE (p4_bracket_execute);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
