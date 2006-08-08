#ifndef _PFE_TOOLS_SUB_H
#define _PFE_TOOLS_SUB_H 1105051254
/* generated 2005-0106-2340 ../../../pfe/../mk/Make-H.pl ../../../pfe/tools-sub.c */

#include <pfe/pfe-sub.h>

/** 
 * -- implementation words for TOOLS-EXT / TOOLS-MIX
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version:  %
 *    (%date_modified:  %)
 *
 *  @description
 *      These are tool words used throughout the system implemenation.
 */

#ifdef __cplusplus
extern "C" {
#endif


# ifndef HASNT_SYSTEM
  _extern int p4_systemf (const char* s, ...);
# endif


/**
 * ?PAIRS
 */
_extern  void p4_Q_pairs (p4cell n) ; /*{*/

/**
 * ?OPEN
 */
_extern  void p4_Q_file_open (p4_File *fid) ; /*{*/

/** _?stop_ ( -- ?key )
 * check for a keypress, and if it was 'q' being pressed
 : _?stop_ _key?_ _key_ [char] q = ;
 */
_extern  int p4_Q_stop (void) ; /*{*/

/** _?cr_ ( -- ?stopped )
 * Like CR but stop after one screenful and return flag if 'q' pressed.
 * Improved by aph@oclc.org (Andrew Houghton)
 */
_extern  int p4_Q_cr (void) ; /*{*/

/**
 * ABORT" string" impl.
 */
_extern  void p4_abortq (const char *fmt,...) ; /*{*/

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
