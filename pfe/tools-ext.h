#ifndef _PFE_TOOLS_EXT_H
#define _PFE_TOOLS_EXT_H 1159392646
/* generated 2006-0927-2330 ../../pfe/../mk/Make-H.pl ../../pfe/tools-ext.c */

#include <pfe/pfe-ext.h>

/** 
 * -- The Optional Programming-Tools Word Set
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2006 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.8 $
 *     (modified $Date: 2006-10-24 00:54:08 $)
 *
 *  @description
 *      The ANS Forth defines some "Programming Tools", words to
 *      inspect the stack (=>'.S'), memory (=>'DUMP'), 
 *      compiled code (=>'SEE') and what words
 *      are defined (=>'WORDS').
 *
 *      There are also word that provide some precompiler support 
 *      and explicit acces to the =>'CS-STACK'.
 * 
 */

#ifdef __cplusplus
extern "C" {
#endif




/** .S ( -- )
 *     print the stack content in vertical nice format.
 *     tries to show cell-stack and float-stack side-by-side,
 *
 *	 Depending on configuration,
 *	there are two parameter stacks: for integers and for
 *	floating point operations. If both stacks are empty, =>'.S'
 *	will display the message <code>&lt;stacks empty&gt;</code>.
 *
 *	If only the floating point stack is empty, =>'.S' displays
 *	the integer stack items  in one column, one item per line,
 *	both in hex and in decimal like this (the first item is topmost):
 12345 HEX 67890 .S
    	424080 [00067890]
         12345 [00003039] ok
 *
 *      If both stacks ar not empty, => .S displays both stacks, in two
 *	columns, one item per line
 HEX 123456.78E90 ok
 DECIMAL 123456.78E90 .S
    	   291 [00000123]          1.234568E+95
    1164414608 [45678E90] ok
 * 	Confusing example? Remember that floating point input only works
 * 	when the => BASE number is =>'DECIMAL'. The first number looks like
 * 	a floating point but it is a goodhex double integer too - the number
 * 	base is =>'HEX'. Thus it is accepted as a hex number. Second try 
 *      with a decimal base will input the floating point number.
 *
 *      If only the integer stack is empty, => .S shows two columns, but
 *      he first columns is called <tt>&lt;stack empty&gt;</tt>, and the
 *      second column is the floating point stack, topmost item first.
 */
extern P4_CODE (p4_dot_s);

/** ? ( addr -- )
 * Display the (integer) content of at address <tt>addr</tt>.
 * This word is sensitive to =>'BASE'
 simulate:
   : ?  @ . ;
 */
extern P4_CODE (p4_question);

/** DUMP ( addr len -- )
 * show a hex-dump of the given area, if it's more than a screenful
 * it will ask using => ?CR
 *
 * You can easily cause a segmentation fault of something like that
 * by accessing memory that does not belong to the pfe-process.
 */
extern P4_CODE (p4_dump);

/** SEE ( "word" -- )
 *  decompile word - tries to show it in re-compilable form.
 *
 *  => (SEE) tries to display the word as a reasonable indented
 *  source text. If you defined your own control structures or
 *  use extended control-flow patterns, the indentation may be
 *  suboptimal.
 simulate:
   : SEE  [COMPILE] ' (SEE) ; 
 */
extern P4_CODE (p4_see);

/** WORDS ( -- )
 * uses CONTEXT and lists the words defined in that vocabulary.
 * usually the vocabulary to list is named directly in before.
 example:
    FORTH WORDS  or  LOADED WORDS
 */
extern P4_CODE (p4_words);

/** AHEAD ( -- DP-mark ORIG-magic ) compile-only
 simulate:
   : AHEAD  BRANCH MARK> (ORIG#) ;
 */
extern P4_CODE (p4_new_ahead);

/** BYE ( -- ) no-return
 * should quit the forth environment completly
 */
extern P4_CODE (p4_bye);

/** CS-PICK ( 2a 2b 2c ... n -- 2a 2b 2c ... 2a )
 * pick a value in the compilation-stack - note that the compilation
 * stack _can_ be seperate in some forth-implemenations. In PFE
 * the parameter-stack is used in a double-cell fashion, so CS-PICK
 * would 2PICK a DP-mark and a COMP-magic, see => PICK
 */
extern P4_CODE (p4_cs_pick);

/** CS-ROLL ( 2a 2b 2c ... n -- 2b 2c ... 2a )
 * roll a value in the compilation-stack - note that the compilation
 * stack _can_ be seperate in some forth-implemenations. In PFE
 * the parameter-stack is used in a double-cell fashion, so CS-ROLL
 * would 2ROLL a DP-mark and a COMP-magic, see => ROLL
 */
extern P4_CODE (p4_cs_roll);

/** FORGET ( "word" -- )
 simulate:
   : FORGET  [COMPILE] '  >NAME (FORGET) ; IMMEDIATE
 */
extern P4_CODE (p4_forget);

/** [ELSE] ( -- )
 * eat up everything upto and including the next [THEN]. count
 * nested [IF] ... [THEN] constructs. see => [IF]
 this word provides a simple pre-compiler mechanism
 */
extern P4_CODE (p4_bracket_else);

/** [IF] ( flag -- )
 * check the condition in the CS-STACK. If true let the following
 * text flow into => INTERPRET , otherwise eat up everything upto
 * and including the next => [ELSE] or => [THEN] . In case of 
 * skipping, count nested [IF] ... [THEN] constructs.
 this word provides a simple pre-compiler mechanism
 */
extern P4_CODE (p4_bracket_if);

/** ASSEMBLER ( -- )
 * set the => ASSEMBLER-WORDLIST as current => CONTEXT
 */
extern P4_CODE (p4_assembler);

/** CODE ( "name" -- )
 * => CREATE a new name and put PFA adress into the CFA place. 
 *
 * NOTE: this description (PFA into CFA) is only correct for traditional
 * indirect threaded code (ITC). The other variants use a block info
 * in the CFA - there we will start a normal colon word which is cut
 * off immediately by a => ;CODE directive to enter the machine-level.
 *
 * WARNING: This word from the => FORTH wordlist does not add => ASSEMBLER 
 * to wordlist which differs from the enhanced behavior of the ASSEMBLER-EXT 
 * module adding a second => CODE to the => EXTENSIONS wordlist.
 *
 * BE AWARE:
 * The TOOLS-EXT will not provide an => END-CODE or any other word in the
 * => ASSEMBLER wordlist which is required to start any useful assembler 
 * programming. After requiring ASSEMBLER-EXT you will see a second "CODE"
 * in the => EXTENSIONS wordlist that will also provide an optimized execution
 * than the result of this standard-forth implemenation.
 */
extern P4_CODE (p4_create_code);

/** ;CODE ( -- )
 * Does end the latest word (being usually some DOES> part) and enters
 * machine-level (in EXEC-mode). 
 *
 * WARNING: This word from the => FORTH wordlist does not add => ASSEMBLER 
 * to wordlist which differs from the enhanced behavior of the ASSEMBLER-EXT 
 * module adding a second => ;CODE to the => EXTENSIONS wordlist.
 *
 * BE AWARE:
 * The TOOLS-EXT will not provide an => END-CODE or any other word in the
 * => ASSEMBLER wordlist which is required to start any useful assembler 
 * programming. After requiring ASSEMBLER-EXT you will see a second ";CODE"
 * in the => EXTENSIONS wordlist that will also provide an optimized execution
 * than the result of this standard-forth implemenation.
 *
 * The Standard-Forth implementation will actually compile a derivate of
 * => BRANCH into the dictionary followed by =>";". The compiled word
 * will not jump to the target adress (following the execution token)
 * but it will call the target adress via the host C stack. The target
 * machine level word (C domain) will just return here for being
 * returned (Forth domain). Hence => END-CODE may be a simple RET, comma!
 */
extern P4_CODE (p4_semicolon_code_execution);

extern P4_CODE (p4_semicolon_code);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
