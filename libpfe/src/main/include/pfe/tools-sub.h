#ifndef PFE_TOOLS_SUB_H
#define PFE_TOOLS_SUB_H 1256212374
/* generated 2009-1022-1352 make-header.py ../../c/tools-sub.c */

#include <pfe/pfe-sub.h>

/**
 * -- implementation words for TOOLS-EXT / TOOLS-MIX
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2008-04-20 04:46:31 $)
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
