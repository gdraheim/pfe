#ifndef PFE_DEBUG_EXT_H
#define PFE_DEBUG_EXT_H 1256209147
/* generated 2009-1022-1259 make-header.py ../../c/debug-ext.c */

#include <pfe/pfe-ext.h>

/**
 * PFE-DEBUG --- analyze compiled code
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.15 $
 *     (modified $Date: 2008-05-11 12:48:04 $)
 *
 *  @description
 *	The Portable Forth Environment provides a decompiler for
 *      colon words and a single stepper for debugging. After
 *      setting a breakpoint at a word saying => DEBUG <tt>word</tt>.
 *  	The next time the <tt>word</tt> gets executed the single
 * 	stepper takes control.
 *
 * 	When this happens you see the top stack items displayed in one
 *	line. The topmost stack item is the first in line, the second and
 *	following stack items are displayed throughout the end of line.
 *	This line is empty if the stack is empty when the word in question
 *	executes.
 *
 *	On the next line you see the first word to become executed inside
 *	the debugged <tt>word</tt>. There is a prompt <tt>&gt;</tt> to
 *	the right of the displayed word. At this prompt you have several
 *	options. Choose one by typing a key (<tt>[h]</tt> shows helpscreen):
 *
 *	<dl>
 *	<dt> <tt>[enter], [x], [k], [down]</tt> </dt>  <dd>
 *	The displayed word will be executed without single stepping.
 *	Note that the execution of the word is slowed down a little
 *	compared to execution outside the single stepper. This is
 *	because the single stepper has to keep control to detect when
 *	the word has finished.
 *
 *	After the actual word finished execution the resulting stack
 *	is printed on the current line. The next line shows the next
 *	word to become executed.
 *
 *	Having repeated this step several times, you can see to the
 *	the right of every decompiled word what changes to the stack
 *	this word caused by comparing with the stack display just
 *	one line above.
 *      </dd>
 *	<dt> <tt>[d], [l], [right]</tt> </dt><dd>
 *	Begin single step the execution of the actual word. The first
 *	word to become executed inside the definition is displayed on
 *	the next line. The word's display is intended by two spaces
 *	for each nesting level.
 *
 *   	You can single step through colon-definitions and the children
 *	of defining words. Note that most of the words in PFE are
 *	rewritten in C for speed, and you can not step those kernel
 *	words.
 *      </dd>
 *      <dt> <tt>[s], [j], [left]</tt> </dt><dd>
 *	Leaves the nesting level. The rest of the definition currently
 *	being executed is run with further prompt. If you leave the
 *	outmost level, the single stepper won't get control again.
 *	Otherwise the debugger stops after the current word is
 *	finished and offers the next word in the previous nesting level.
 *	</dd>
 *	<dt> <tt>[space]</tt> </dt><dd>
 *	The next word to be executed is decompiled. This should help
 *	to decide as if to single step that word.
 *	</dd>
 *	<dt> <tt>[q]</tt> </dt><dd>
 *	Quits from the debugger. The execution of the debugged word is
 *	not continued. The stacks are not cleared or changed.
 *	</dd>
 *	<dt> <tt>[c]</tt> </dt><dd>
 *	Displays the profiling instruction counter.
 *	<dt> <tt>[r]</tt> </dt><dd>
 *	Reset the instruction counter, to profile some code. The
 *	debugger counts how often the inner interpreter i.e. how
 *	many Forth-primitives are executed. Use this option to
 *      reset the counter to 0 to measure an arbitrary part of code.
 *	</dd>
 *	</dl>
 */

#ifdef __cplusplus
extern "C" {
#endif




extern P4_CODE (p4_debug_colon_RT);

extern P4_CODE (p4_debug_does_RT);

/** DEBUG ( "word" -- ) [FTH]
 * this word will place an debug-runtime into
 * the => CFA of the following word. If the
 * word gets executed later, the user will
 * be prompted and can decide to single-step
 * the given word. The debug-stepper is
 * interactive and should be self-explanatory.
 * (use => NO-DEBUG to turn it off again)
 */
extern P4_CODE (p4_debug);

/** NO-DEBUG ( "word" -- ) [FTH]
 * the inverse of " => DEBUG word "
 */
extern P4_CODE (p4_no_debug);

/** (SEE) ( some-xt* -- ) [FTH]
 * decompile the token-sequence - used
 * by => SEE name
 */
extern P4_CODE (p4_paren_see);

/** ADDR>NAME ( word-addr* -- word-nfa!*' | 0 ) [FTH]
 * search the next corresponding namefield that address
 * is next too. If it is not in the base-dictionary, then
 * just return 0 as not-found.
 */
extern P4_CODE (p4_addr_to_name);

/** COME_BACK ( -- ) [FTH]
 * show the return stack before last exception
 * along with the best names as given by => ADDR>NAME
 */
extern P4_CODE (p4_come_back);

_extern  p4xcode* p4_locals_bar_SEE (p4xcode* ip, char* p, p4_Semant* s) ; /*{*/

_extern  p4xcode* p4_local_SEE (p4xcode* ip, char* p, p4_Semant* s) ; /*{*/

_extern  p4xcode* p4_literal_SEE (p4xcode* ip, char* p, p4_Semant* s) ; /*{*/

_extern  p4xcode* /* P4_SKIPS_TO_TOKEN */ p4_lit_to_token_SEE (p4xcode* ip, char* p, p4_Semant* s) ; /*{*/

_extern  p4xcode* /* P4_SKIPS_STRING */ p4_lit_string_SEE (p4xcode* ip, char* p, p4_Semant* s) ; /*{*/

_extern  p4xcode* /* P4_SKIPS_2STRINGS */ p4_lit_2strings_SEE (p4xcode* ip, char* p, p4_Semant* s) ; /*{*/

_extern  p4xcode* /* P4_SKIPS_DCELL */ p4_lit_dcell_SEE (p4xcode* ip, char* p, p4_Semant* s) ; /*{*/

_extern  void p4_decompile_rest (p4xcode *ip, int nl, int indent, p4_bool_t iscode) ; /*{*/

_extern  void p4_decompile (p4_namebuf_t* nfa, p4xt xt) ; /*{*/

_extern  char p4_category (p4code p) ; /*{*/

_extern  void p4_debug_off (void) ; /*{*/

/** ADDR>NAME ( word-addr* -- word-nfa*!' | 0 ) [FTH]
 * search the next corresponding namefield that address
 * is next too. If it is not in the base-dictionary, then
 * just return 0 as not-found.
 */
_extern  p4_namebuf_t const * p4_addr_to_name (const p4_byte_t* addr) ; /*{*/

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
