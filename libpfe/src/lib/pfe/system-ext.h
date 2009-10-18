#ifndef _VOL_8_SRC_CVS_PFE_33_PFE_SYSTEM_EXT_H
#define _VOL_8_SRC_CVS_PFE_33_PFE_SYSTEM_EXT_H 1209868837
/* generated 2008-0504-0440 /vol/8/src/cvs/pfe-33/pfe/../mk/Make-H.pl /vol/8/src/cvs/pfe-33/pfe/system-ext.c */

#include <pfe/pfe-ext.h>

/** 
 * --  FORTH-83 System Extensions
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
 *     forth-83 did define a system extension word set which
 *     was deleted in dpans94 due to some problems with the
 *     native-compiling variants of forth. pfe is traditional
 *     enough to use the words as they were originally intended.
 */

#ifdef __cplusplus
extern "C" {
#endif




/** <MARK ( -- DP-mark ) compile-only
 * memorizes the current => DP on the CS-STACK
 * used for => <RESOLVE later. Useful for creation of 
 * compiling words, eg. => BEGIN , see => AHEAD
 simulate:
   : <MARK ?COMP  HERE ;
 */
extern P4_CODE (p4_backward_mark);

/** <RESOLVE ( DP-mark -- ) compile-only
 * resolves a previous => <MARK , actually pushes
 * the DP-address memorized at <MARK into the dictionary.
 * Mostly used after => BRANCH or => ?BRANCH in compiling
 * words like => UNTIL
 simulate:
   : <RESOLVE ?COMP  , ;
 */
extern P4_CODE (p4_backward_resolve);

/** MARK> ( -- DP-mark ) compile-only
 * makes room for a pointer in the dictionary to
 * be resolved through => RESOLVE> and does therefore
 * memorize that cell's address on the CS-STACK
 * Mostly used after => BRANCH or => ?BRANCH in compiling
 * words like => IF or => ELSE
 simulate:
   : MARK> ?COMP  HERE 0 , ;
 */
extern P4_CODE (p4_forward_mark);

/** RESOLVE> ( DP-mark -- ) compile-only
 * resolves a pointer created by => MARK>
 * Mostly used in compiling words like => THEN
 simulate:
   : RESOLVE> ?COMP  HERE SWAP ! ;
 */
extern P4_CODE (p4_forward_resolve);

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
extern P4_CODE (p4_branch);

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
extern P4_CODE (p4_q_branch);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
