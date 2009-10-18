#ifndef _VOL_8_SRC_CVS_PFE_33_PFE_STACKHELP_EXT_H
#define _VOL_8_SRC_CVS_PFE_33_PFE_STACKHELP_EXT_H 1209868838
/* generated 2008-0504-0440 /vol/8/src/cvs/pfe-33/pfe/../mk/Make-H.pl /vol/8/src/cvs/pfe-33/pfe/stackhelp-ext.c */

#include <pfe/pfe-ext.h>

/** 
 * -- The Additional StackHelp TypeCheck Word Set
 * 
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.5 $
 *     (modified $Date: 2008-05-04 02:57:30 $)
 *
 *  @description
 *    These are routines to add stackchecking capabilities. The
 *    loader routine will hook into the outer interpreter and
 *    _before_ any word is compiled/interpreted it is being 
 *    sent through an stackcheck interpreter. The stackcheck
 *    interpreter code is fully independent from the rest of
 *    the code. It will only work on the stackcheck hints given
 *    with each word. In most cases the stackcheck hints are
 *    simple strings which get parsed for information - these
 *    strings may be taken from the usual stack notation of a
 *    a colon word. The parser is detecting the syntax given
 *    in the OpenFirmware recommendations for specifying a compact
 *    stack and parsing behavior of a word. They are expanded for
 *    extra type hints and tracing through splitstack parts.
 *
 *    Other than pure strings, the stackchecking can be done
 *    through code words that work on the checkstack - they are
 *    similar to immediate words in the normal forth interpreter
 *    whereas the stacknotation strings are checkstacked by the 
 *    checkstack parser directly instead of blackboxing it 
 *    through a call to a routine somewhere. It does however
 *    bring in a lot of flexibility and allows for complex
 *    stackcheck code in extension modules. The core-stk module
 *    is an integral part of the stackcheck behavior for forth
 *    and encompasses stackchecking for IF-ELSE branches and
 *    the various LOOPs and EXIT-points of a ':'-colon word.
 *
 *   implementation notes:
 *     The parsing of a single stack notation ( .... ) follows
 *     these rules:
 *     (a) recognize "changer"s with embedded "--" and seperated
 *            by " |" twochar sequence 
 *     (b) cut into input / output effect notation without "--"
 *            with a resulting "notation"
 *     (c) each stack effect may have "variant" notations seperated
 *         by "| " twochar sequence
 *     (d) cut into stack effect parts with the "stack" notation broken
 *          at each "X:" up to the following "Y:" marker. The
 *          resulting notation applies to the specified stack only.
 *     (e) each argument is seperated by whitespace, but an argument
 *         may be broken down further.
 *     (f) an argument may have ellipsis expansion with "..."
 *         for multi-cell notations
 *     (g) an argument may have  a singular "alternative" notation with
 *         a "|" that has no whitespace on either side.
 *     (h) an argument may have  a singular expansion with ","
 *         used for double-cell notations
 *     (i) after seperation as above, each part is broken down
 *         into name-prefix and type-suffix-list where types may be
 *         non-ascii singlechar or ascii-named strings introduced
 *         by a "-", i.e. "a-ptr" is the same as "a*".
 *     (j) a literal may occur ("0"), or self-parsing hint as seen
 *         in traditional forth stack notations ("[text<eol>]")
 *     (x) note the ambiguity of " | ", the changer recognition
 *         is supposed to be greedy, i.e.
 *         0 NARROW-CHANGER( a b -- c d | x y | u v -- m n )
 *                         ..^^^^^^^^^^^^^^^^^..
 *         1 NARROW-CHANGER( a b -- c d | x y | u v -- m n )
 *                         ....................^^^^^^^^^^^^..
 *         1 NARROW-CHANGER( a b -- c d | x y| u v -- m n )
 *                         ..............^^^^^^^^^^^^^^^^^..
 *         1 NARROW-CHANGER( a b -- c d |x y |u v -- m n )
 *                         ..............^^^^^^^^^^^^^^^^..
 *         1 NARROW-CHANGER( a b -- c d| x y| u v -- m n )
 *                [invalid -- no separator for second changer]
 *
 *     (y) a "notation" is a single stack layout definition per changer
 *         which includes the syntaxhint at the start. This scheme allows
 *         to step through user-provided "notation"s in order and "rewrite" 
 *         each stack layout without changing the original order. Therefore
 *         we have
 *         0 0 NARROW-INPUT-NOTATION( a b R: x y | u v -- m n )
 *                                    ^^^^
 *         1 0 NARROW-INPUT-NOTATION( a b R: x y | u v -- m n )
 *                                        ^^^^^^^
 *         2 0 NARROW--INPUT-NOTATION( a b R: x y | u v -- m n )
 *                                                ^^^^^^
 *
 * preliminary usage:
 *         start compiling a colon word and add a stack notation with
 *         the help of "|( ..... )". The stackhelp checker will show
 *         which words it did recognize and at the final semicolon (or
 *         any other exitpoint) it will check the resulting stack
 *         layout with the layout given first as the intended stack
 *         notation. Try ": |( a -- a a ) dup dup ;" for a start. And
 *         remember that stackhelp is an external module which you must
 *         load before as "needs stackhelp-ext".
 *
 *         Additionally there are a few helper words, most prominently
 *         you can fetch the stackhelp notation for any registered word
 *         by saying "stackhelp 'wordname'". Note that forth is really
 *         a nontyped language and you are able to add "hint narrowing"
 *         on each portion of a stack item in a postfix notation of 
 *         each item. Therefore ": dup |( a -- a a)" will work simply
 *         on anything while ": @ |( a* -- a)" will issue a warning if
 *         you give it "depth @" since the depth word is defined with
 *         a numeric postfix hint by ": depth |( -- value#)". 
 *
 *         Furthermore, there are a number of debugging words that can
 *         be used also to interactivly test whether some type narrowing
 *         will work later in practice. Start off with giving a current
 *         stack notation as the input line notation 
 *                 REWRITE-LINE( a b 0 | c 1! )
 *         which is a split notation example with differing depths in
 *         each of the possible inputs. You can get back the current
 *         stack notation by saying REWRITE-SHOW. Next one can test a
 *         few changer notations, where changer means anything that
 *         does contain a "--" seperator. One can first test which part
 *         of a multivariant changer would be used by asking with
 *         REWRITE-SELECT( a 0 -- | a 1! -- a ). The same with a
 *         REWRITE-TEST( a 0 -- | a 1! -- a) would say "Ok!" since we
 *         do can apply the changer to the current stack line. Then
 *         we can try with REWRITE-EXPAND( a 0 -- | a 1! -- a) and
 *         REWRITE-RESULT( a 0 -- | a 1! -- a) where the second one
 *         will also try to collapse a splitstack result to a simple
 *         one. 
 */

#ifdef __cplusplus
extern "C" {
#endif




/** "NARROW-CHANGER(" ( changer# "stackhelp<rp>" -- ) [EXT]
 */
extern P4_CODE(p4_narrow_changer);

/** "NARROW-INPUTLIST(" ( changer# "stackhelp<rp>" -- ) [EXT]
 */
extern P4_CODE(p4_narrow_inputlist);

/** "NARROW-OUTPUTLIST(" ( changer# "stackhelp<rp>" -- ) [EXT]
 */
extern P4_CODE(p4_narrow_outputlist);

/** "NARROW-INPUT-VARIANT(" ( variant# changer# "stackhelp<rp>" -- ) [EXT]
 * 0 = default, 1 = 'S', 2 = 'R', ... 4 = 'P', ... 7 = 'M', .. 14 = 'F' 
 */
extern P4_CODE(p4_narrow_input_variant);

/** "NARROW-OUTPUT-VARIANT(" ( variant# changer# "stackhelp<rp>" -- ) [EXT]
 */
extern P4_CODE(p4_narrow_output_variant);

/** "NARROW-INPUT-STACK(" ( stk-char variant# changer# "stackhelp<rp>" -- ) [EXT]
 * 0 = default, 1 = 'S', 2 = 'R', ... 4 = 'P', ... 7 = 'M', .. 14 = 'F' 
 */
extern P4_CODE(p4_narrow_input_stack);

/** "NARROW-OUTPUT-STACK(" ( stk-char variant# changer# "stackhelp<rp>" -- ) [EXT]
 */
extern P4_CODE(p4_narrow_output_stack);

/** "NARROW-INPUT-ARGUMENT(" ( arg# stk-char variant# changer# "stackhelp<rp>" -- ) [EXT]
 * 0 = default, 1 = 'S', 2 = 'R', ... 4 = 'P', ... 7 = 'M', .. 14 = 'F' 
 * arg# is [0] = TOS and [1] = UNDER, same as the pick values where
 * 3 2 1 0 2 pick . =:= 2
 */
extern P4_CODE(p4_narrow_input_argument);

/** "NARROW-OUTPUT-ARGUMENT(" ( arg# stk-char variant# changer# "stackhelp<rp>" -- ) [EXT]
 * arg# is [0] = TOS and [1] = UNDER, same as the pick values where
 * 3 2 1 0 2 pick . =:= 2
 */
extern P4_CODE(p4_narrow_output_argument);

/** "NARROW-INPUT-ARGUMENT-NAME(" ( arg# stk-char variant# changer# "stackhelp<rp>" -- ) [EXT]
 * 0 = default, 1 = 'S', 2 = 'R', ... 4 = 'P', ... 7 = 'M', .. 14 = 'F' 
 * arg# is [0] = TOS and [1] = UNDER, same as the pick values where
 * 3 2 1 0 2 pick . =:= 2
 */
extern P4_CODE(p4_narrow_input_argument_name);

/** "NARROW-OUTPUT-ARGUMENT-NAME(" ( arg# stk-char variant# changer# "stackhelp<rp>" -- ) [EXT]
 * arg# is [0] = TOS and [1] = UNDER, same as the pick values where
 * 3 2 1 0 2 pick . =:= 2
 */
extern P4_CODE(p4_narrow_output_argument_name);

/** "NARROW-INPUT-ARGUMENT-TYPE(" ( arg# stk-char variant# changer# "stackhelp<rp>" -- ) [EXT]
 * 0 = default, 1 = 'S', 2 = 'R', ... 4 = 'P', ... 7 = 'M', .. 14 = 'F' 
 * arg# is [0] = TOS and [1] = UNDER, same as the pick values where
 * 3 2 1 0 2 pick . =:= 2
 */
extern P4_CODE(p4_narrow_input_argument_type);

/** "NARROW-OUTPUT-ARGUMENT-TYPE(" ( arg# stk-char which# "stackhelp<rp>" -- ) [EXT]
 * arg# is [0] = TOS and [1] = UNDER, same as the pick values where
 * 3 2 1 0 2 pick . =:= 2
 */
extern P4_CODE(p4_narrow_output_argument_type);

/** "CANONIC-INPUT-TYPE(" ( arg# stk-char variant# changer# "stackhelp<rp>" -- ) [EXT]
 * 0 = default, 1 = 'S', 2 = 'R', ... 4 = 'P', ... 7 = 'M', .. 14 = 'F' 
 * arg# is [0] = TOS and [1] = UNDER, same as the pick values where
 * 3 2 1 0 2 pick . =:= 2
 */
extern P4_CODE(p4_canonic_input_type);

/** "CANONIC-OUTPUT-TYPE(" ( arg# stk-char variant# changer# "stackhelp<rp>" -- ) [EXT]
 * arg# is [0] = TOS and [1] = UNDER, same as the pick values where
 * 3 2 1 0 2 pick . =:= 2
 */
extern P4_CODE(p4_canonic_output_type);

/** "REWRITER-TEST(" ( "tracked-stack -- input-stack<rp>" -- ) [EXT]
 * suppose that the left side is a tracked stack line during compiling
 * and the right side is a candidate changer input stack. Test whethr
 * the candidate does match and the complete changer would be allowed
 * to run a rewrite in the track stack buffer.
 *
 * Possible conditions include:
 *   the left side has not enough arguments or...
 *      any argument on the right side has a type specialization
 *      that does not match as a valid suffix to their counterpart
 *      on the left side.
 */
extern P4_CODE(p4_rewriter_test);

/** "REWRITER-INPUT-ARG(" ( arg# "tracked-stack -- changer<rp>" -- ) [EXT]
 * suppose that the left side is a tracked stack line during compiling
 * and the right side is a candidate changer input stack. Assume the
 * righthand candidate does match - look at the given argument on the
 * left side and show the prefix being copied to the output trackstack
 * when the rewrite-rule is gettin applied later.
 */
extern P4_CODE(p4_rewriter_input_arg);

/** "REWRITE-LINE(" ( "stack-layout<rp>" -- )  [EXT]
 * fill rewrite-buffer with a stack-layout to be processed.
 * see =>"REWRITE-SHOW."
 */
extern P4_CODE (p4_rewrite_line);

/** "REWRITE-SHOW." ( -- ) [EXT]
 * show current rewrite-buffer.
 */
extern P4_CODE (p4_rewrite_show);

/** "REWRITE-STACK-TEST(" ( "stackhelp<rp>" -- ) [EXT]
 *  check whether this stackhelp does match on current rewrite-buffer
 *  and say oK/No respectivly.
 */
extern P4_CODE (p4_rewrite_stack_test);

/** "REWRITE-INPUT-ARG(" ( arg# "stackhelp<rp>" -- ) [EXT]
 *  check whether this stackhelp does match on current rewrite-buffer
 *  and in the given input match show us the argument but only the
 *  good prefix i.e. the type constraint being cut off already.
 */
extern P4_CODE(p4_rewrite_input_arg);

/** "REWRITE-STACK-RESULT(" ( "stackhelp<rp>" -- ) [EXT]
 * rewrite the current rewrite-buffer and show the result that
 * would occur with this stackhelp being applied.
 */
extern P4_CODE (p4_rewrite_stack_result);

/** "NARROW-INPUT-NOTATION(" ( notation# changer# "stackhelp<rp>" -- ) [EXT]
 */
extern P4_CODE(p4_narrow_input_notation);

/** "NARROW-OUTPUT-NOTATION(" ( notation# changer# "stackhelp<rp>" -- ) [EXT]
 */
extern P4_CODE(p4_narrow_output_notation);

/** "REWRITE-STACKDEF-TEST(" ( "stackdef<rp>" )  [EXT]
 *  match a stackdef (single variant of stacks).
 *  assume: single variant in rewrite-buffer and
 *          single variant in stackdef-arg and
 *          only one changer in arg-stackhelp
 */
extern P4_CODE (p4_rewrite_stackdef_test);

/** "REWRITE-STACKDEF-RESULT(" ( "stackhelp<rp>" -- ) [EXT]
 *  assume:
 *       only one changer (if more are provided then only the first is used)
 *       only one stackdef variant in inputlist
 */
extern P4_CODE (p4_rewrite_stackdef_result);

/** "REWRITE-TEST(" ( "stackhelp<rp>" -- ) [EXT]
 * Test whether the given changer would match the current line.
 * assume:
 *      only one changer (if more are provided then only the first is used)
 */
extern P4_CODE (p4_rewrite_test);

extern P4_CODE (p4_rewrite_changer_select);

extern P4_CODE (p4_rewrite_changer_expand);

extern P4_CODE (p4_rewrite_changer_result);

extern P4_CODE (p4_rewrite_select);

extern P4_CODE (p4_rewrite_expand);

extern P4_CODE (p4_rewrite_result);

extern P4_CODE (p4_stackhelp_when_done);

/** "|(" ( [string<rp>] -- ) [EXT]
 *  add a checkstack notation for the LAST word or just try to
 *  match the given notation with the stacklayout traced so
 *  far - possibly casting a few types as needed.
 */
extern P4_CODE (p4_stackhelpcomment);

/** STACKHELPS ( [name] -- ) [EXT]
 *  show all possible stackhelps for this name.
 */
extern P4_CODE (p4_stackhelps);

/** STACKHELP ( [name] -- ) [EXT]
 *  show the stackhelp info registered for this name.
 */
extern P4_CODE (p4_stackhelp);

extern P4_CODE (p4_stackhelp_exitpoint);

extern P4_CODE (p4_stackhelp_when_exit);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
