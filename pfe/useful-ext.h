#ifndef _PFE_USEFUL_EXT_H
#define _PFE_USEFUL_EXT_H 985049575
/* generated 2001-0320-0152 ../../pfe/../mk/Make-H.pl ../../pfe/useful-ext.c */

#include <pfe/incl-ext.h>

/** 
 * -- useful additional primitives
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 1.14 %
 *    (%date_modified: Mon Mar 12 10:33:06 2001 %)
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




/** (IMMEDIATE#) ( -- bit-mask )
 *  returns the bit-mask to check if a found word is immediate
    " my-word" FIND IF >FFA C@ (IMMEDIATE#) AND 
                       IF ." immediate" THEN ELSE DROP THEN
 */
extern P4_CODE (p4_immediate_bit);

/** (SMUDGE#) ( -- bit-mask )
 *  returns the bit-mask to check if a found word is smudge
    " my-word" FIND IF >FFA C@ (SMUDGE#) AND 
                       IF ." smudge" THEN ELSE DROP THEN
 */
extern P4_CODE (p4_smudge_bit);

/** >COMPILE ( xt -- )
 *  does the work of => POSTPONE on the execution token that 
 *  you got from somewhere else - so it checks if the name
 *  (that correspond to the execution-token argument) is
 *  actually immediate, so it has to be executed to compile
 *  something, e.g. => IF or => THEN - see also => POSTPONE ,
 *  => COMPILE , => [COMPILE] , => INTERPRET
 */
extern P4_CODE (p4_to_compile);

/** ($ ( [word] -- cs-token ) compile-only
 *  takes the execution token of the following word and
 *  saves it on the compile-stack. The correspondig closing
 *  => ) will then feed it into => >COMPILE - so this pair
 *  of word provides you with a prefix-operation syntax
 *  that you may have been seen in lisp-like languages.
   ($ IF ($ 0= A1 @ )) ($ THEN ." hello " )
 * Note that an opening simple => ( paren is a comment.
 */
extern P4_CODE (p4_prefix_begin);

/** ) ( cs-token -- )
 * takes the execution-token from => ($ and compiles
 * it using => >COMPILE
 */
extern P4_CODE (p4_prefix_end);

/** )) ( cs-token cs-token -- )
 * takes two execution-tokens from two of => ($ and compiles
 * them on after another using => >COMPILE
 simulate:
    : )) [COMPILE] ) [COMPILE] ) ; IMMEDIATE
 */
extern P4_CODE (p4_prefix_end_doubled);

/** SMART-INTERPRET-INIT ( -- )
 * creates a set of interpret-words that are used in the inner
 * interpreter, so if a word is unknown to the interpreter-loop
 * it will use the first char of that word, attach it to an 
 * "interpret-" prefix, and tries to use that =>'IMMEDIATE'-=>'DEFER'-word 
 * on the rest of the word. This => SMART-INTERPRET-INIT will set up
 * words like interpret-" so you can write 
 * <c>"hello"</c>  instead of   <c>" hello"</c>
 * and it creates interpret-\ so that words like <c>\if-unix</c> are
 * ignoring the line if the word <c>\if-unknown</c> is unknown in itself.
 * This is usually <i>not</i> activated on startup.
 */
extern P4_CODE (p4_smart_interpret_init);

/** SMART-INTERPRET-OFF ( -- )
 * disables the SMART-INTERPRET extension in => INTERPRET ,
 * see => SMART-INTERPRET-INIT
 */
extern P4_CODE (p4_smart_interpret_off);

/** SMART-INTERPRET-ON ( -- )
 * enables the SMART-INTERPRET extension in => INTERPRET ,
 * see => SMART-INTERPRET-INIT - the default for smart-interpret 
 * is always off
 */
extern P4_CODE (p4_smart_interpret_on);

/** SPRINTF ( args ... format$ dest$ -- len-dest ) 
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

/** PRINTF ( args ... format$ -- )
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

/** (LOADF-LOCATE) ( xt -- nfa )
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

/** (;AND)
 * compiled by => ;AND
 */
extern P4_CODE (p4_semicolon_and_execution);

/** ;AND ( -- )
 * For the code piece between => MAKE and => ;AND , this word
 * will do just an => EXIT . For the code outside of
 * the => MAKE construct a branch-around must be resolved then.
 */
extern P4_CODE (p4_semicolon_and);

/** ((MAKE-))
 * compiled by => MAKE
 */
extern P4_CODE (p4_make_to_local_execution);

/** ((MAKE))
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

/** FIELD-OFFSET ( offset "name" -- )
 * create a new offsetword. The word is created and upon execution
 * it add the offset, ie. compiling runtime:
       ( address -- address+offset )
 */
extern P4_CODE (p4_field_offset);

extern P4_CODE (p4_offset_RT);

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

/** ALIAS ( xt "name" -- )
 * create a defer word that is initialized with the given x-token.
 */
extern P4_CODE (p4_alias);

/** ALIAS-ATEXIT ( xt "name" -- )
 * create a defer word that is initialized with the given x-token.
 */
extern P4_CODE (p4_alias_atexit);

/** VOCABULARY' ( "name" -- )
 * create an immediate vocabulary. Provides for basic 
 * modularization.
 : VOCABULARY' VOCABULARY IMMEDIATE ;
 */
extern P4_CODE (p4_vocabulary_tick);

/** 'X"' ( "hex-q" -- bstring ) 
 * places a counted string on stack
 * containing bytes specified by hex-string
 * - the hex string may contain spaces which will delimit the bytes
 example: 
    X" 41 42 4344" COUNT TYPE ( shows ABCD )
 */
extern P4_CODE (p4_x_quote);

extern P4_CODE (p4_boot_script_fetch);

extern P4_CODE (p4_boot_script_colon);

/** BOOT-SCRIPT@ ( -- s-a s-n )
 * the file that will be include on next => COLD boot
 * DO NOT USE! will vanish w/o warning in the next version!
 */
extern P4_CODE (p4_boot_script_fetch);

/** BOOT-SCRIPT: ( "string" -- )
 * DO NOT USE! will vanish w/o warning in the next version!
 * see => BOOT-SCRIPT@
 */
extern P4_CODE (p4_boot_script_colon);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
