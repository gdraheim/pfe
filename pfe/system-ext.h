#ifndef _PFE_SYSTEM_EXT_H
#define _PFE_SYSTEM_EXT_H 985272240
/* generated 2001-0322-1544 ../../pfe/../mk/Make-H.pl ../../pfe/system-ext.c */

#include <pfe/incl-ext.h>

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

/** >MARK ( -- DP-mark ) compile-only
 * makes room for a pointer in the dictionary to
 * be resolved through => RESOLVE> and does therefore
 * memorize that cell's address on the CS-STACK
 * Mostly used after => BRANCH or => ?BRANCH in compiling
 * words like => IF or => ELSE
 simulate:
   : MARK> ?COMP  HERE 0 , ;
 */
extern P4_CODE (p4_forward_mark);

/** >RESOLVE ( DP-mark -- ) compile-only
 * resolves a pointer created by => MARK>
 * Mostly used in compiling words like => THEN
 simulate:
   : RESOLVE> ?COMP  HERE SWAP ! ;
 */
extern P4_CODE (p4_forward_resolve);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
