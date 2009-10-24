/**
 * --  FORTH-83 System Extensions
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author$)
 *  @version $Revision$
 *     (modified $Date$)
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
"@(#) $Id$";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-comp.h>
#include <pfe/def-limits.h>
#include <pfe/_missing.h>

// extern void FXCode (p4_q_branch_execution); /* ?BRANCH */
// extern void FXCode (p4_branch_execution);	 /* BRANCH */

/** <MARK ( -- dict-mark ) compile-only
 * memorizes the current => HERE point on the CS-STACK
 * used for => <RESOLVE later. Useful for creation of
 * compiling words, eg. => BEGIN , see => AHEAD
 simulate:
   : <MARK ?COMP  HERE ;
 */
void FXCode (p4_backward_mark)
{
    FX (p4_Q_comp);
    FX_PUSH (HERE);
}

/** <RESOLVE ( dict-mark -- ) compile-only
 * resolves a previous => <MARK , actually pushes
 * the HERE-address memorized at <MARK into the dictionary.
 * Mostly used after => BRANCH or => ?BRANCH in compiling
 * words like => UNTIL
 simulate:
   : <RESOLVE ?COMP  , ;
 */
void FXCode (p4_backward_resolve)
{
    FX (p4_Q_comp);
#if 0
    FX_QCOMMA ((char *) FX_POP - (char *) HERE);
#else
    FX_QCOMMA (FX_POP);
#endif
}

/** MARK> ( -- dict-mark ) compile-only
 * makes room for a pointer in the dictionary to
 * be resolved through => RESOLVE> and does therefore
 * memorize that cell's address on the CS-STACK
 * Mostly used after => BRANCH or => ?BRANCH in compiling
 * words like => IF or => ELSE
 simulate:
   : MARK> ?COMP  HERE 0 , ;
 */
void FXCode (p4_forward_mark)
{
    FX (p4_backward_mark);
    FX_QCOMMA(0);
}

/** RESOLVE> ( dict-mark -- ) compile-only
 * resolves a pointer created by => MARK>
 * Mostly used in compiling words like => THEN
 simulate:
   : RESOLVE> ?COMP  HERE SWAP ! ;
 */
void FXCode (p4_forward_resolve)
{
    FX (p4_Q_comp);
# if 0
    *(p4cell *) *SP = (char *) HERE - (char *) *SP;
    FX_DROP;
# else
    *(p4char **) FX_POP = HERE;
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
void FXCode (p4_branch)
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
void FXCode (p4_q_branch)
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

P4_LISTWORDSET (system) [] =
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
P4_COUNTWORDSET (system, "System-extension wordset from forth-83");

/*@}*/
