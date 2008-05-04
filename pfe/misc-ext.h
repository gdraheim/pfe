#ifndef _VOL_8_SRC_CVS_PFE_33_PFE_MISC_EXT_H
#define _VOL_8_SRC_CVS_PFE_33_PFE_MISC_EXT_H 1209868837
/* generated 2008-0504-0440 /vol/8/src/cvs/pfe-33/pfe/../mk/Make-H.pl /vol/8/src/cvs/pfe-33/pfe/misc-ext.c */

#include <pfe/pfe-ext.h>

/** 
 * -- miscellaneous useful words, mostly stemming from fig-forth
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.6 $
 *     (modified $Date: 2008-05-04 02:57:30 $)
 *
 *  @description
 *      Compatiblity with former standards, miscellaneous useful words.
 */

#ifdef __cplusplus
extern "C" {
#endif




/** COLD ( -- ) [FTH]
 * cold abort - reinitialize everything and go to => QUIT routine
 * ... this routine is implemented as a warm-boot in pfe.
 : COLD [ ALSO ENVIRONMENT ] EMPTY SCRIPT-FILE INCLUDED QUIT ;
 */
extern P4_CODE (p4_cold);

/** .LINE ( line# block# -- ) [FTH]
 */
extern P4_CODE (p4_dot_line);

/** UD.R ( x,x# r# -- ) [FTH]
 */
extern P4_CODE (p4_u_d_dot_r);

/** UD. ( x,x# -- ) [FTH]
 * see also => UD.R
 */
extern P4_CODE (p4_u_d_dot);

/** ID. ( some-nfa* -- ) [FTH]
 * print the name-field pointed to by the nfa-argument.
 * a synonym for .NAME - but this word is more portable due its
 * heritage from fig-forth. 
 * 
 * in fig-forth the name-field is effectivly a bstring with some flags,
 * so the nfa's count has to be masked out, e.g. 
 : .NAME COUNT 32 AND TYPE ;
 *
 * in other pfe configurations, the name might not contain the flags it
 * it just a counted string - and there may be even more possibilities.
 : .NAME COUNT TYPE ;
 * 
 * you should more and more convert your code to use the sequence
 * => NAME>STRING => TYPE which is widely regarded as the better
 * variant.
 */
extern P4_CODE(p4_id_dot);

/** -ROLL ( x...[n-1] y n# -- y x...[n-1] |  num# -- ) [FTH]
 * the inverse of => ROLL
 */
extern P4_CODE (p4_dash_roll);

/** RANDOM ( n# -- random# ) [FTH]
 * returns random number with 0 <= n2 < n1)
 : RANDOM ?DUP IF _random SWAP MOD ELSE _random THEN ;
 */
extern P4_CODE (p4_random);

/** SRAND ( seed# -- ) [FTH]
 */
extern P4_CODE (p4_srand);

/** +UNDER ( n1 x n2 -- n1+n2 x ) [EXT]
 *     quicker than
 : UNDER+  ROT + SWAP ;
 * Note: the old pfe version of UNDER+ is obsolete as it is in conflict
 * with a comus word of the same name. The behavior of this word will
 * continue to exist under the name of =>"(UNDER+)". Users are encouraged
 * to use the comus behavior of UNDER+ which does already exist under
 * the name of =>"+UNDER". In the future pfe will be changed to pick up
 * the comus behavior making UNDER+ and +UNDER to be synonyms. In the
 * current version there will be load-time warning on usages of "UNDER+".
 */
extern P4_CODE (p4_plus_under);

/** "(UNDER+)" ( n1 n2 -- n1+n2 n2 ) [FTH]
 * quicker than
 : (UNDER+) TUCK + SWAP ; or : (UNDER+) DUP UNDER+ ;
 */
extern P4_CODE (p4_under_plus);

/** ((+TO)) ( val -- ) [HIDDEN]
 * execution compiled by => +TO
 * adds the stack-val to the lvalue compiled
 */
extern P4_CODE (p4_plus_to_execution);

/** ((+TO.local)) ( val -- ) [HIDDEN]
 * same as => ((+TO)) when the lvalue is a => LOCALS| value
 * <br> compiled by => +TO
 */
extern P4_CODE (p4_plus_to_local_execution);

/** +TO ( val [name] -- ) [FTH]
 * add the val to the named => VALUE or => LOCALS| value
 */
extern P4_CODE (p4_plus_to);

/** BUILD-ARRAY ( x#...[dim] dim# -- memsize# ) [FTH]
 * writes X, n1, ... nX into the dictionary - 
 * returns product n1 * n2 * ... * nX 
 */
extern P4_CODE (p4_build_array);

/** ACCESS-ARRAY ( x#...[dim#] array* --- array* value# ) [FTH]
 * see => BUILD-ARRAY
 */
extern P4_CODE (p4_access_array);

/** SOURCE-LINE ( -- source-line# ) [FTH]
 * if => SOURCE is from => EVALUATE (or => QUERY ) then
 * the result is 0 else the line-numbers start from 1 
 */
extern P4_CODE (p4_source_line);

/** SOURCE-NAME ( -- source-name-ptr source-name-len ) [FTH]
 * if => SOURCE is from => INCLUDE then the result is the filename,
 * otherwise a generic name for the SOURCE-ID is given.
 */
extern P4_CODE (p4_source_name);

/** TH'POCKET ( pocket# -- pocket-ptr pocket-len ) [FTH]
 * returns the specified pocket as a => S" string reference
 */
extern P4_CODE (p4_th_pocket);

/** POCKET-PAD ( -- pocket-ptr ) [FTH]
 * Returns the next pocket.
 * A pocket has usually the size of a maxstring, see =>"ENVIRONMENT /STRING"
 * (but can be configured to be different, mostly when MAXPATH > /STRING )
 * Note that a pocket is a temporary and forth internal functions do
 * sometimes call => POCKET-PAD too, especially when building filenames
 * and getting a literal (but temporary) string from the keyboard.
 * Functions are not expected to hold references to this transient
 * area any longer than building a name and calling another word with it.

 * Usage of a pocket pad is a good way to make local temporary buffers
 * superfluous that are only used to construct a temporary string that 
 * usually gets swallowed by another function.
 depracated code:
   create temp-buffer 255 allot
   : make-temp ( str buf ) 
          temp-buffer place  " .tmp" count temp-buffer append 
          temp-buffer count make-file ;
 replace with this:
   : make-temp ( str buf )
        pocket-pad >r    
        r place  " .tmp" count r append
        r> count make-file
   ;
 */
extern P4_CODE (p4_pocket_pad);

/** WL-HASH ( buf-ptr buf-len -- buf-hash# ) [FTH]
 * calc hash-code for selection of thread
 * in a threaded-vocabulary
 */
extern P4_CODE (p4_wl_hash);

/** TOPMOST ( some-wordlist* -- some-topmost-nfa* ) [FTH]
 * that last valid word in the specified vocabulary
 */
extern P4_CODE (p4_topmost);

/** LS.WORDS ( -- ) [FTH]
 * see => WORDS
 */
extern P4_CODE (p4_ls_words);

/** LS.PRIMITIVES ( -- ) [FTH]
 * see => WORDS
 */
extern P4_CODE (p4_ls_primitives);

/** LS.COLON-DEFS ( -- ) [FTH]
 * see => WORDS
 */
extern P4_CODE (p4_ls_cdefs);

/** LS.DOES-DEFS ( -- ) [FTH]
 * see => WORDS
 */
extern P4_CODE (p4_ls_ddefs);

/** LS.CONSTANTS ( -- ) [FTH]
 * see => WORDS
 */
extern P4_CODE (p4_ls_constants);

/** LS.VARIABLES ( -- ) [FTH]
 * see => WORDS
 */
extern P4_CODE (p4_ls_variables);

/** LS.VOCABULARIES ( -- ) [FTH]
 * see => WORDS
 */
extern P4_CODE (p4_ls_vocabularies);

/** LS.MARKERS ( -- ) [FTH]
 * see => WORDS
 */
extern P4_CODE (p4_ls_markers);

/** W@ ( some-wchar* -- some-wchar# | some* -- some# [?] ) [FTH]
 * fetch a 2byte-val from address
 */
extern P4_CODE (p4_w_fetch);

/** W! ( value#  some-wchar* -- | value# wchar* -- [?] ) [FTH]
 * store a 2byte-val at addressed 2byte-value
 */
extern P4_CODE (p4_w_store);

/** W+! ( value# some-wchar* -- | value# wchar* -- [?] ) [FTH]
 * add a 2byte-val to addressed 2byte-value
 */
extern P4_CODE (p4_w_plus_store);

/** TAB ( tab-n# -- ) [FTH]
 * jump to next column divisible by n 
 */
extern P4_CODE (p4_tab);

/** BACKSPACE ( -- ) [FTH]
 * reverse of => SPACE
 */
extern P4_CODE (p4_backspace);

/** ?STOP ( -- stop-flag ) [FTH]
 * check for 'q' pressed
 * - see => ?CR
 */
extern P4_CODE (p4_Q_stop);

/** START?CR ( -- ) [FTH]
 * initialized for more-like effect
 * - see => ?CR
 */
extern P4_CODE (p4_start_Q_cr);

/** ?CR ( -- cr-flag ) [FTH]
 * like => CR , stop 25 lines past => START?CR
 */
extern P4_CODE (p4_Q_cr);

/** CLOSE-ALL-FILES ( -- ) [FTH]
 */
extern P4_CODE (p4_close_all_files);

/** .MEMORY ( -- ) [FTH]
 */
extern P4_CODE (p4_dot_memory);

/** .STATUS ( -- ) [FTH]
 * display internal variables 
 : .STATUS .VERSION .CVERSION .MEMORY .SEARCHPATHS .DICTVARS .REGSUSED ;
 */
extern P4_CODE (p4_dot_status);

/** (EMIT) ( char# -- ) [FTH]
 * like => EMIT and always to screen 
 * - the routine to be put into => *EMIT*
 */
extern P4_CODE (p4_paren_emit);

/** (EXPECT) ( a b -- ) [FTH]
 * like => EXPECT and always from screen
 * - the routine to be put into => *EXPECT*
 */
extern P4_CODE (p4_paren_expect);

/** (KEY) ( -- key# ) [FTH]
 * like => KEY and always from screen
 * - the routine to be put into => *KEY*
 */
extern P4_CODE (p4_paren_key);

/** (TYPE) ( str* len# -- ) [FTH]
 * like => TYPE and always to screen 
 * - the routine to be put into => *TYPE*
 */
extern P4_CODE (p4_paren_type);

/** STANDARD-I/O ( -- ) [FTH]
 * initialize => *TYPE* , => *EMIT* , => *EXPECT* and => *KEY*
 * to point directly to the screen I/O routines, <br> 
 * namely => (TYPE) , => (EMIT) , => (EXPECT) , => (KEY) 
 */
extern P4_CODE (p4_standard_io);

/** ((EXECUTES)) ( fkey# -- ) [HIDDEN]
 * compiled by => EXECUTES
 */
extern P4_CODE (p4_executes_execution);

/** EXECUTES ( fkey# [word] -- ) [EXT]
 * stores the execution token of following word into
 * the callback pointer for the specified function-key 
 */
extern P4_CODE (p4_executes);

/** HELP ( "name" -- ) [FTH] [EXEC]
 * will load the help module in the background and hand over the 
 * parsed name to => (HELP) to be resolved. If no => (HELP) word
 * can be loaded, nothing will happen.
 */
extern P4_CODE (p4_help);

/** EDIT-BLOCKFILE ( "name" -- ) [FTH] [EXEC]
 * will load the edit module in the background and look for a word
 * called => EDIT-BLOCK that could be used to edit the blockfile.
 * If no => EDIT-BLOCKFILE word can be loaded, nothing will happen.
 * Otherwise, => OPEN-BLOCKFILE is called followed by => 0 => EDIT-BLOCK
 * to start editing the file at the first block.
 */
extern P4_CODE (p4_edit_blockfile);

/** ARGC ( -- arg-count ) [FTH]
 */
extern P4_CODE (p4_argc);

/** ARGV ( arg-n# -- arg-ptr arg-len ) [FTH]
 */
extern P4_CODE (p4_argv);

/** EXPAND-FN ( name-ptr name-len buf-ptr -- buf-ptr buf-len ) [FTH]
 : e.g. s" includefile" POCKET-PAD EXPAND-FN ;
 */
extern P4_CODE (p4_expand_fn);

/** ((LOAD")) ( -- ? ) [HIDDEN]
 */
extern P4_CODE (p4_load_quote_execution);

/** LOAD"  ( [filename<">] -- ??? ) [FTH] [OLD]
 * load the specified file - this word can be compiled into a word-definition
 * obsolete! use => OPEN-BLOCKFILE name => LOAD
 */
extern P4_CODE (p4_load_quote);

/** SYSTEM ( command-ptr command-len -- command-exitcode# ) [FTH]
 * run a shell command  (note: embedded systems have no shell)
 */
extern P4_CODE (p4_system);

/** ((SYSTEM")) ( ... -- exitcode# ) [HIDDEN]
 * compiled by => SYSTEM" commandline"
 */
extern P4_CODE (p4_system_quote_execution);

/** SYSTEM" ( [command-line<">] -- command-exitcode# ) [FTH] [OLD]
 * run a shell command (note:embedded systems have no shell)
 * obsolete! use => S" string" => SYSTEM
 */
extern P4_CODE (p4_system_quote);

/** CREATE: ( "name" -- ) [FTH]
 * this creates a name with the => VARIABLE runtime.
 * Note that this is the FIG-implemenation of => CREATE whereas in
 * ANS-Forth mode we have a => CREATE identical to FIG-style =>"<BUILDS"
 : CREATE: BL WORD $HEADER DOVAR A, ;
 */
extern P4_CODE (p4_create_var);

/** BUFFER: ( size# "name" -- ) [FTH]
 * this creates a name with the => VARIABLE runtime and =>"ALLOT"s memory
 : BUFFER: BL WORD $HEADER DOVAR A, ALLOT ;
 */
extern P4_CODE (p4_buffer_var);

/** R'@ ( R: a b -- a R: a b ) [FTH]
 * fetch the next-under value from the returnstack.
 * used to interpret the returnstack to hold two => LOCALS| values.
 * ( =>'R@' / =>'2R@' / =>'R>DROP' / =>'R"@')
 */
extern P4_CODE (p4_r_tick_fetch);

extern P4_CODE (p4_r_tick_fetch_execution);

/** R'! ( x R: a b -- R: x b ) [FTH]
 * store the value into the next-under value in the returnstack.
 * used to interpret the returnstack to hold two => LOCALS| values.
 * see =>"R'@" for inverse operation
 */
extern P4_CODE (p4_r_tick_store);

extern P4_CODE (p4_r_tick_store_execution);

/** R"@ ( R: a b c -- a R: a b c ) [FTH]
 * fetch the second-under value from the returnstack.
 * used to interpret the returnstack to hold three => LOCALS| values.
 * see =>'R"!' for inverse operation ( =>"R'@" =>"R@" / =>"2R@" / =>"R>DROP" )
 */
extern P4_CODE (p4_r_quote_fetch);

extern P4_CODE (p4_r_quote_fetch_execution);

/** R"! ( x R: a b c -- R: x b c ) [FTH]
 * store the value into the second-under value in the returnstack.
 * used to interpret the returnstack to hold three => LOCALS| values.
 * see =>'R"@' for inverse operation
 */
extern P4_CODE (p4_r_quote_store);

extern P4_CODE (p4_r_quote_store_execution);

/** R! ( x R: a -- R: x ) [FTH]
 * store the value as the topmost value in the returnstack.
 * see =>"R@" for inverse operation ( =>"R'@" / =>'R"@' / =>'2R@' / =>'2R!')
 */
extern P4_CODE (p4_r_store);

extern P4_CODE (p4_r_store_execution);

/** 2R! ( x y R: a b -- R: x y ) [FTH]
 * store the value as the topmost value in the returnstack.
 * see =>"2R@" for inverse operation ( =>"R'@" / =>'R"@' / =>'2R@' / =>'2R!')
 */
extern P4_CODE (p4_two_r_store);

extern P4_CODE (p4_two_r_store_execution);

/** DUP>R ( val -- val R: val ) [FTH]
 * shortcut, see => R>DROP
 * <br> note again that the following will fail:
 : DUP>R DUP >R ;
 */
extern P4_CODE (p4_dup_to_r);

extern P4_CODE (p4_dup_to_r_execution);

/** R>DROP ( R: val -- R: ) [FTH]
 * shortcut (e.g. in CSI-Forth)
 * <br> note that the access to R is configuration dependent - only in
 * a traditional fig-forth each NEST will be one cell wide - in case that
 * there are no => LOCALS| of course. And remember, the word above reads
 * like the sequence => R> and => DROP but that is not quite true.
 : R>DROP R> DROP ; ( is bad - correct might be )  : R>DROP R> R> DROP >R ;
 */
extern P4_CODE (p4_r_from_drop);

extern P4_CODE (p4_r_from_drop_execution);

/** 2R>2DROP ( R: a b -- R: ) [FTH]
 * this is two times => R>DROP but a bit quicker.
 * it is however really quick compared to the sequence => 2R> and => 2DROP
 */
extern P4_CODE (p4_two_r_from_drop);

extern P4_CODE (p4_two_r_from_drop_execution);

/** CLEARSTACK ( -- ) [FTH]
 * reset the parameter stack to be empty
 : CLEARSTACK  S0 SP! ;
 */
extern P4_CODE (p4_clearstack);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
