#ifndef _PFE_MISC_EXT_H
#define _PFE_MISC_EXT_H 984413843
/* generated 2001-0312-1717 ../../pfe/../mk/Make-H.pl ../../pfe/misc-ext.c */

#include <pfe/incl-ext.h>

/** 
 * -- miscellaneous useful words, mostly stemming from fig-forth
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 5.16 %
 *    (%date_modified: Mon Mar 12 10:32:33 2001 %)
 *
 *  @description
 *      Compatiblity with former standards, miscellaneous useful words.
 */

#ifdef __cplusplus
extern "C" {
#endif




/** COLD ( -- )
 * cold abort - reinitialize everything and go to => QUIT routine
 * ... this routine is implemented as a warm-boot in pfe.
 : WARM FENCE @ (FORGET) INCLUDE-FILE ?DUP IF COUNT INCLUDED THEN QUIT ;
 */
extern P4_CODE (p4_cold);

/** .LINE ( line# block# -- )
 */
extern P4_CODE (p4_dot_line);

/** !CSP ( -- )
 * put => SP into => CSP
 * <br> used in control-words
 */
extern P4_CODE (p4_store_csp);

/** ?CSP ( -- )
 * check that => SP == => CSP otherwise => THROW
 * <br> used in control-words
 */
extern P4_CODE (p4_Q_csp);

/** ?COMP ( -- )
 * check that the current => STATE is compiling
 * otherwise => THROW
 * <br> often used in control-words
 */
extern P4_CODE (p4_Q_comp);

/** ?EXEC ( -- )
 * check that the current => STATE is executing
 * otherwise => THROW
 * <br> often used in control-words
 */
extern P4_CODE (p4_Q_exec);

/** ?FILE ( file-id -- )
 * check the file-id otherwise (fixme)
 */
extern P4_CODE (p4_Q_file);

/** ?LOADING ( -- )
 * check that the currently interpreted text is 
 * from a file/block, otherwise => THROW
 */
extern P4_CODE (p4_Q_loading);

/** ?PAIRS ( a b -- )
 * if compiling, check that the two magics on
 * the => CS-STACK are identical, otherwise throw
 * <br> used in control-words
 */
extern P4_CODE (p4_Q_pairs);

/** ?STACK ( -- )
 * check all stacks for underflow and overflow conditions,
 * and if such an error condition is detected => THROW
 */
extern P4_CODE (p4_Q_stack);

/** TOGGLE ( c-addr charmask -- ) 
 * toggle the bits given in charmask, see also => SMUDGE and = UNSMUDGE
 example: the fig-style SMUDGE had been defined such
   : FIG-SMUDGE LATEST >FFA (SMUDGE#) TOGGLE ;
 */
extern P4_CODE (p4_toggle);

/** LATEST ( -- nfa )
 * return the NFA of the lateset definition in the
 * => CURRENT vocabulary
 */
extern P4_CODE (p4_latest);

/** SMUDGE (modified from FIG definition) 
 *   FIG definition toggles the bit! 
 */
extern P4_CODE (p4_smudge);

/** UNSMUDGE (turn smudge-bit off) 
 *   neccessary because => SMUDGE modified 
 */
extern P4_CODE (p4_unsmudge);

/** UD.R ( 2val r -- )
 */
extern P4_CODE (p4_u_d_dot_r);

/** UD. ( 2val -- )
 * see also => UD.R
 */
extern P4_CODE (p4_u_d_dot);

/** .NAME ( nfa -- )
 * print the name-field which is effectivly a bstring - in
 * some forth implementations the name-field is identical 
 * with the flag-field, so the nfa's count has to be masked
 * the, e.g. 
 : .NAME COUNT 32 AND TYPE ;
 * if the name-field and flag-field are different, then this
 * word does effectivly 
 : .NAME COUNT TYPE ;
 */
extern P4_CODE (p4_dot_name);

/** ID. ( nfa -- )
 * print the name-field pointed to by the nfa-argument.
 * a synonym for .NAME that is more portable due its
 * heritage from fig-forth.
 */
extern P4_CODE(p4_id_dot);

/** -ROLL ( xn ... x2 x1 n -- x1 xn ... x2 )
 * the inverse of => ROLL
 */
extern P4_CODE (p4_dash_roll);

/** R>DROP ( -- )
 * shortcut (e.g. in CSI-Forth)
 * <br> note that
 : R>DROP R> DROP ; 
 * would be wrong - for a non-direct-threaded forth it would be
 : R>DROP R> R> NIP >R ;
 */
extern P4_CODE (p4_r_from_drop);

/** DUP>R ( val -- val )
 * shortcut, see => R>DROP
 * <br> note again that the following will fail:
 : DUP>R DUP >R ;
 */
extern P4_CODE (p4_dup_to_r);

/** RANDOM ( n1 -- n2 )
 * returns random number with 0 <= n2 < n1)
 */
extern P4_CODE (p4_random);

/** SRAND ( n -- )
 */
extern P4_CODE (p4_srand);

/** UNDER+ ( n1 n2 -- n1+n2 n2 )
 * quicker than
 : UNDER+ TUCK + SWAP ;
 */
extern P4_CODE (p4_under_plus);

/** ((+TO)) ( val -- )
 * execution compiled by => +TO
 * adds the stack-val to the lvalue compiled
 */
extern P4_CODE (p4_plus_to_execution);

/** ((+TO.local)) ( val -- )
 * same as => ((+TO)) when the lvalue is a => LOCALS| value
 * <br> compiled by => +TO
 */
extern P4_CODE (p4_plus_to_local_execution);

/** +TO ( val [name] -- )
 * add the val to the named => VALUE or => LOCALS| value
 */
extern P4_CODE (p4_plus_to);

/** BUILD-ARRAY ( n1 n2 ... nX X --- n )
 * writes X, n1, ... nX into the dictionary - 
 * returns product n1 * n2 * ... * nX 
 */
extern P4_CODE (p4_build_array);

/** ACCESS-ARRAY ( i1 i2 ... iX addr1 --- addr2 n )
 * see => BUILD-ARRAY
 */
extern P4_CODE (p4_access_array);

/** 0<= ( a -- flag )
 simulate    : 0<= 0> 0= ;
 */
extern P4_CODE (p4_zero_less_equal);

/** 0>= ( a -- flag )
 simulate    : 0>= 0< 0= ;
 */
extern P4_CODE (p4_zero_greater_equal);

/** <= ( a b -- flag )
 simulate    : <= > 0= ;
 */
extern P4_CODE (p4_less_equal);

/** >= ( a b -- flag )
 simulate    : >= < 0= ;
 */
extern P4_CODE (p4_greater_equal);

/** U<= ( a b -- flag )
 simulate    : U<= U> 0= ;
 */
extern P4_CODE (p4_u_less_equal);

/** U>= ( a b -- flag )
 simulate    : U>= U< 0= ;
 */
extern P4_CODE (p4_u_greater_equal);

/** UMAX ( a b -- max )
 * see => MAX
 */
extern P4_CODE (p4_u_max);

/** UMIN ( a b -- min )
 * see => MIN , => MAX and => UMAX
 */
extern P4_CODE (p4_u_min);

/** SOURCE-LINE ( -- n )
 * if => SOURCE is from => EVALUATE (or => QUERY ) then
 * the result is 0 else the line-numbers start from 1 
 */
extern P4_CODE (p4_source_line);

/** TH'POCKET ( n -- addr u )
 * returns the specified pocket as a => S" string reference
 */
extern P4_CODE (p4_th_pocket);

/** WL-HASH ( c-addr n1 -- n2 )
 * calc hash-code for selection of thread
 * in a threaded-vocabulary
 */
extern P4_CODE (p4_wl_hash);

/** TOPMOST ( wid -- a-addr )
 * that last valid word in the specified vocabulary
 */
extern P4_CODE (p4_topmost);

/** LS.WORDS ( -- )
 * see => WORDS
 */
extern P4_CODE (p4_ls_words);

/** LS.PRIMITIVES ( -- )
 * see => WORDS
 */
extern P4_CODE (p4_ls_primitives);

/** LS.COLON-DEFS ( -- )
 * see => WORDS
 */
extern P4_CODE (p4_ls_cdefs);

/** LS.DOES-DEFS ( -- )
 * see => WORDS
 */
extern P4_CODE (p4_ls_ddefs);

/** LS.CONSTANTS ( -- )
 * see => WORDS
 */
extern P4_CODE (p4_ls_constants);

/** LS.VARIABLES ( -- )
 * see => WORDS
 */
extern P4_CODE (p4_ls_variables);

/** LS.VOCABULARIES ( -- )
 * see => WORDS
 */
extern P4_CODE (p4_ls_vocabularies);

/** LS.MARKERS ( -- )
 * see => WORDS
 */
extern P4_CODE (p4_ls_markers);

/** W@ ( addr -- w-val )
 * fetch a 2byte-val from address
 */
extern P4_CODE (p4_w_fetch);

/** W! ( w-val addr -- )
 * store a 2byte-val at addressed 2byte-value
 */
extern P4_CODE (p4_w_store);

/** W+! ( w-val addr -- )
 * add a 2byte-val to addressed 2byte-value
 */
extern P4_CODE (p4_w_plus_store);

/** (FORGET) ( addr -- )
 * forget everything above addr
 * - used by => FORGET
 */
extern P4_CODE (p4_paren_forget);

/** TAB ( n -- )
 * jump to next column divisible by n 
 */
extern P4_CODE (p4_tab);

/** BACKSPACE ( -- )
 * reverse of => SPACE
 */
extern P4_CODE (p4_backspace);

/** ?STOP ( -- flag )
 * check for 'q' pressed
 * - see => ?CR
 */
extern P4_CODE (p4_Q_stop);

/** START?CR ( -- )
 * initialized for more-like effect
 * - see => ?CR
 */
extern P4_CODE (p4_start_Q_cr);

/** ?CR ( -- flag )
 * like => CR , stop 25 lines past => START?CR
 */
extern P4_CODE (p4_Q_cr);

/** CLOSE-ALL-FILES ( -- )
 */
extern P4_CODE (p4_close_all_files);

/** .MEMORY ( -- )
 */
extern P4_CODE (p4_dot_memory);

/** .VERSION ( -- )
 */
extern P4_CODE (p4_dot_version);

/** .PFE-DATE ( -- )
 */
extern P4_CODE (p4_dot_date);

/** LICENSE ( -- )
 */
extern P4_CODE (p4_license);

/** WARRANTY ( -- )
 */
extern P4_CODE (p4_warranty);

/** SHOW-STATUS ( -- )
 * display internal variables 
 */
extern P4_CODE (p4_show_status);

/** (EMIT) ( val -- )
 * like => EMIT and always to screen 
 * - the routine to be put into => *EMIT*
 */
extern P4_CODE (p4_paren_emit);

/** (EXPECT) ( . -- . )
 * like => EXPECT and always from screen
 * - the routine to be put into => *EXPECT*
 */
extern P4_CODE (p4_paren_expect);

/** (KEY) ( -- ... )
 * like => KEY and always from screen
 * - the routine to be put into => *KEY*
 */
extern P4_CODE (p4_paren_key);

/** (TYPE) ( val -- )
 * like => TYPE and always to screen 
 * - the routine to be put into => *TYPE*
 */
extern P4_CODE (p4_paren_type);

/** STANDARD-I/O ( -- )
 * initialize => *TYPE* , => *EMIT* , => *EXPECT* and => *KEY*
 * to point directly to the screen I/O routines, <br> 
 * namely => (TYPE) , => (EMIT) , => (EXPECT) , => (KEY) 
 */
extern P4_CODE (p4_standard_io);

/** ((EXECUTES)) ( n -- )
 * compiled by => EXECUTES
 */
extern P4_CODE (p4_executes_execution);

/** EXECUTES ( n [word] -- )
 * stores the execution token of following word into
 * the callback pointer for the specified function-key 
 */
extern P4_CODE (p4_executes);

/** HELP ( -- )
 * non-functional at the moment.
 */
extern P4_CODE (p4_help);

/** COPY-FILE ( src-str src-strlen dst-str dst-strlen -- errno|0 )
 * like =>'RENAME-FILE', copies the file from src-name to dst-name
 * and returns an error-code or null
 */
extern P4_CODE (p4_copy_file);

/** MOVE-FILE ( src-str src-strlen dst-str dst-strlen -- errno|0 )
 * like =>'RENAME-FILE', but also across-volumes <br>
 * moves the file from src-name to dst-name and returns an
 * error-code or null
 */
extern P4_CODE (p4_move_file);

/** FILE-R/W ( addr blk f fid -- )
 * like FIG-Forth <c> R/W </c>
 */
extern P4_CODE (p4_file_rw);

/** FILE-BLOCK ( a file-id -- c )
 */
extern P4_CODE (p4_file_block);

/** FILE-BUFFER ( a file-id -- c )
 */
extern P4_CODE (p4_file_buffer);

/** FILE-EMPTY-BUFFERS ( file-id -- )
 */
extern P4_CODE (p4_file_empty_buffers);

/** FILE-FLUSH ( file-id -- )
 simulate      : FILE-FLUSH DUP FILE-SAVE-BUFFERS FILE-EMTPY-BUFFERS ;
 */
extern P4_CODE (p4_file_flush);

/** FILE-LIST ( x file-id -- )
 */
extern P4_CODE (p4_file_list);

/** FILE-LOAD ( x file-id -- )
 */
extern P4_CODE (p4_file_load);

/** FILE-SAVE-BUFFERS ( file-id -- )
 */
extern P4_CODE (p4_file_save_buffers);

/** FILE-THRU ( lo hi file-id -- )
 * see => THRU
 */
extern P4_CODE (p4_file_thru);

/** FILE-UPDATE ( file-id -- )
 */
extern P4_CODE (p4_file_update);

/** ARGC ( -- n )
 */
extern P4_CODE (p4_argc);

/** ARGV ( n -- addr u )
 */
extern P4_CODE (p4_argv);

/** EXPAND-FN ( addr1 u1 addr2 -- addr2 cnt2 )
 */
extern P4_CODE (p4_expand_fn);

/** USING ( 'filename' -- )
 * use filename as a block file
 */
extern P4_CODE (p4_using);

/** USING-NEW ( 'filename' -- )
 * like => USING but can create the file
 */
extern P4_CODE (p4_using_new);

/** ((LOAD")) ( -- ? )
 */
extern P4_CODE (p4_load_quote_execution);

/** LOAD"  ( [filename<">] -- ? )
 * load the specified file - this word can be
 * compiled into a word-definition
 */
extern P4_CODE (p4_load_quote);

/** INCLUDE ( 'filename' -- ? )
 * load the specified file, see also => LOAD" filename"
 */
extern P4_CODE (p4_include);

/** SYSTEM ( addr u -- ret-val )
 * run a shell command
 * <br> (embedded systems have no shell)
 */
extern P4_CODE (p4_system);

/** ((SYSTEM")) 
 * compiled by => SYSTEM" commandline"
 */
extern P4_CODE (p4_system_quote_execution);

/** SYSTEM" ( [commandline<">] -- ret-val )
 * run a shell command 
 * <br> (embedded systems have no shell)
 */
extern P4_CODE (p4_system_quote);

/** RAISE ( n -- )
 * send a => SIGNAL to self
 */
extern P4_CODE (p4_raise);

/** SIGNAL ( xt1 n -- xt2 )
 * install signal handler
 * - return old signal handler
 */
extern P4_CODE (p4_signal);

/** HEADER ( bstring -- )
 * => CREATE a new header in the dictionary from the given string,
 * the runtime is the same as with => CREATE
 usage: : CREATE  BL WORD HERE HEADER ;
 */
extern P4_CODE (p4_header);

/** (DICTLIMIT)   ( -- constvalue )
 * the upper limit of the forth writeable memory space,
 * the variable => DICTLIMIT must be below this line.
 * stack-space and other space-areas are often allocated
 * above => DICTLIMIT upto this constant.
 *
 * => DICTFENCE is the lower end of the writeable dictionary
 */
extern P4_CODE(p4_paren_dictlimit);

/** (DICTFENCE)   ( -- constvalue )
 * the lower limit of the forth writeable memory space,
 * the variable => DICTFENCE must be above this line.
 * Some code-areas are often moved in between => DICTFENCE and
 * this constant. To guard normal Forth code from deletion
 * the usual practice goes with the => FENCE variable
 *
 * => DICTLIMIT is the upper end of the writeable dictionary
 */
extern P4_CODE(p4_paren_dictfence);

/** >DOES-BODY ( xt -- cell* )
 * in ans'forth a synonym for => >BODY
 : >DOES-BODY >BODY ;
 * in fig'forth it adds another cell
 : >DOES-BODY >BODY CELL+ ;
 */
extern P4_CODE (p4_to_does_body);

/** >DOES-CODE ( xt -- xt* )
 * in fig'forth a synonym for => >BODY
 : >DOES-CODE >BODY ;
 * in ans'forth it returns the field that points to DOES>
 * which is just before the cfa in pfe.
 : >DOES-CODE CELL- ;
 */
extern P4_CODE (p4_to_does_code);

/** <BUILDS ( 'name' -- )
 *  => CREATE a name whose runtime will be changed later
 *  using => DOES>  <br />
 *  note that ans'forth does not define => <BUILDS and
 *  it suggests to use => CREATE directly. <br />
 *  in fig'forth however it did also => ALLOT a cell
 *  to hold the address where => DOES> starts.
 : <BUILDS CREATE ; \ ans'forth 
 : <BUILDS CREATE 0 , ; \ fig'forth
 */
extern P4_CODE (p4_does_create);

/** R'@ ( R: a b -- a R: a b )
 * fetch the next-under value from the returnstack.
 * used to interpret the returnstack to hold two => LOCALS| values.
 * ( => R@ / => 2R@ / => R>DROP )
 */
extern P4_CODE (p4_r_tick_fetch);

/** R'! ( x R: a b -- R: x b )
 * store the value into the next-under value in the returnstack.
 * used to interpret the returnstack to hold two => LOCALS| values.
 */
extern P4_CODE (p4_r_tick_store);

/** CLEARSTACK ( -- )
 * reset the parameter stack to be empty
 : CLEARSTACK  S0 SP! ;
 */
extern P4_CODE (p4_clearstack);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
