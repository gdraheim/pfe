/** 
 * --  FORTH-83 System Extensions
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
 *     forth-83 did define a system extension word set which
 *     was deleted in dpans94 due to some problems with the
 *     native-compiling variants of forth. pfe is traditional
 *     enough to use the words as they were originally intended.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: system-ext.c,v 1.3 2006-09-22 04:43:03 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-comp.h>
#include <pfe/def-limits.h>
#include <pfe/_missing.h>

// extern FCode (p4_q_branch_execution); /* ?BRANCH */
// extern FCode (p4_branch_execution);	 /* BRANCH */

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
    FX_QCOMMA ((char *) FX_POP - (char *) DP);
#else
    FX_QCOMMA (FX_POP);
#endif
}

/** MARK> ( -- DP-mark ) compile-only
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
    FX_QCOMMA(0);
}

/** RESOLVE> ( DP-mark -- ) compile-only
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
 : BRANCH COMPILE (BRANCH) ;
 */
FCode (p4_branch)
{
    FX_COMPILE (p4_else);
}

/** ?BRANCH ( -- )
 * compiles a cond-branch-runtime into the dictionary that
 * can be resolved with =>"&gt;MARK&"d or =>"RESOLVE&gt;".
 * Usage:
     ?BRANCH MARK&lt;     or
     ?BRANCH &gt;RESOLVE  or ...
 * this is the runtime-portion of => IF - the use of
 * => IF should be preferred. See also => BRANCH
 : ?BRANCH COMPILE (?BRANCH) ;
 */
FCode (p4_q_branch)
{
    FX_COMPILE (p4_if);
}

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

P4_LISTWORDS (system) =
{
    P4_INTO ("FORTH", 0),
    P4_FXco ("<MARK",		p4_backward_mark),
    P4_FXco ("<RESOLVE",	p4_backward_resolve),
    P4_FXco ("MARK>",		p4_forward_mark),
    P4_FXco ("RESOLVE>",	p4_forward_resolve),
    P4_IXco ("BRANCH",		p4_branch),
    P4_IXco ("?BRANCH",		p4_q_branch),
    /** <c>SEARCH</c> => ORDER variables, 
       for => VOCABULARY => ALSO => DEFINITIONS 
    */
    P4_DVaL ("CONTEXT",		context),
    P4_DVaR ("CURRENT",		current),

    P4_INTO ("ENVIRONMENT", 0 ),
    P4_OCON ("SYSTEM-EXT",	1983 ),
};
P4_COUNTWORDS (system, "System-extension wordset from forth-83");

/*@}*/

