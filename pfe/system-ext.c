/** 
 * --  FORTH-83 SYSTEM EXTENSION WORD SET
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 1.4 %
 *    (%date_modified: Mon Mar 12 10:32:46 2001 %)
 *
 *  @description
 *     forth-83 did define a system extension word set which
 *     was deleted in dpans94 due to some problems with the
 *     native-compiling variants of forth. pfe is traditional
 *     enough to use the words as they were originally intended.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: system-ext.c,v 0.31 2001-03-22 18:28:29 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-xtra.h>
#include <pfe/def-comp.h>
#include <pfe/_missing.h>

extern FCode (p4_if_execution);		/* ?BRANCH */
extern FCode (p4_else_execution);	/* BRANCH */

/** <MARK ( -- DP-mark ) compile-only
 * memorizes the current => DP on the CS-STACK
 * used for => <RESOLVE later. Useful for creation of 
 * compiling words, eg. => BEGIN , see => AHEAD
 simulate:
   : <MARK ?COMP  HERE ;
 */
FCode (p4_backward_mark)	
{
    FX (p4_Q_comp);
    FX_PUSH (DP);
}

/** <RESOLVE ( DP-mark -- ) compile-only
 * resolves a previous => <MARK , actually pushes
 * the DP-address memorized at <MARK into the dictionary.
 * Mostly used after => BRANCH or => ?BRANCH in compiling
 * words like => UNTIL
 simulate:
   : <RESOLVE ?COMP  , ;
 */
FCode (p4_backward_resolve)		
{
    FX (p4_Q_comp);
#if 0
    FX_COMMA ((char *) FX_POP - (char *) DP);
#else
    FX_COMMA (FX_POP);
#endif
}

/** >MARK ( -- DP-mark ) compile-only
 * makes room for a pointer in the dictionary to
 * be resolved through => RESOLVE> and does therefore
 * memorize that cell's address on the CS-STACK
 * Mostly used after => BRANCH or => ?BRANCH in compiling
 * words like => IF or => ELSE
 simulate:
   : MARK> ?COMP  HERE 0 , ;
 */
FCode (p4_forward_mark)	
{
    FX (p4_backward_mark);
    P4_INC (DP, p4cell);          /* or FX_COMMA(0) */
}

/** >RESOLVE ( DP-mark -- ) compile-only
 * resolves a pointer created by => MARK>
 * Mostly used in compiling words like => THEN
 simulate:
   : RESOLVE> ?COMP  HERE SWAP ! ;
 */
FCode (p4_forward_resolve)
{
    FX (p4_Q_comp);
# if 0
    *(p4cell *) *SP = (char *) DP - (char *) *SP;
    FX_DROP;
# else
    *(p4char **) FX_POP = DP;
# endif
}

/** BRANCH ( -- )
 * compiles a branch-runtime into the dictionary that
 * can be resolved with =>"MARK&lt;"d or =>"&lt;RESOLVE".
 * Usage:
     BRANCH MARK&lt;     or
     BRANCH &gt;RESOLVE  or ...
 * this is the runtime-portion of => ELSE - the use of
 * => ELSE should be preferred. See also => ?BRANCH
 */

/** ?BRANCH ( -- )
 * compiles a cond-branch-runtime into the dictionary that
 * can be resolved with =>"&gt;MARK&"d or =>"RESOLVE&gt;".
 * Usage:
     ?BRANCH MARK&lt;     or
     ?BRANCH &gt;RESOLVE  or ...
 * this is the runtime-portion of => IF - the use of
 * => IF should be preferred. See also => BRANCH
 */

/** CONTEXT ( addr -- )
 * The variable that holds the or the topmost search-order
 * wordlist. The new ansforth standard suggests the use 
 * of =>"SET-CONTEXT" and =>"GET-CONTEXT" instead of
 * using => CONTEXT => ! and => CONTEXT => @
 */

/** CURRENT ( addr -- )
 * The variable that holds the or the topmost compile-order
 * wordlist. The new ansforth standard suggests the use 
 * of =>"SET-CURRENT" and =>"GET-CURRENT" instead of
 * using => CURRENT => ! and => CURRENT => @
 */

/** MARK> ( -- DP-mark )
 * depracated, use fst83 word => >MARK
 */

/** RESOLVE> ( DP-mark -- )
 * depracated, use fst83 word => >RESOLVE
 */

P4_LISTWORDS (system) =
{
    CO ("<MARK",	p4_backward_mark),
    CO ("<RESOLVE",	p4_backward_resolve),
    CO (">MARK",	p4_forward_mark),
    CO (">RESOLVE",	p4_forward_resolve),
    CO ("MARK>",	p4_forward_mark),
    CO ("RESOLVE>",	p4_forward_resolve),
    CO ("BRANCH",	p4_else_execution),
    CO ("?BRANCH",	p4_if_execution),
    /** <c>SEARCH</c> => ORDER variables, 
       for => VOCABULARY => ALSO => DEFINITIONS 
    */
    DV ("CONTEXT",	context),
    DV ("CURRENT",	current),
    P4_INTO ("ENVIRONMENT", 0 ),
    P4_OCON ("SYSTEM-EXT",	1983 ),
};
P4_COUNTWORDS (system, "System-extension wordset from forth-83");

/*@}*/

