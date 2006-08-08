#ifndef _PFE_FILE_SUB_H
#define _PFE_FILE_SUB_H 1105095033
/* generated 2005-0107-1150 ../../../pfe/../mk/Make-H.pl ../../../pfe/file-sub.c */

#include <pfe/pfe-sub.h>

/** 
 *  Subroutines for file access
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: bln_mpt1!33.12 %
 *    (%date_modified: Tue Sep 10 13:29:33 2002 %)
 */

#ifdef __cplusplus
extern "C" {
#endif




_extern  _p4_off_t p4_file_size (FILE * f) /* Result: file length, -1 on error */ ; /*{*/

_extern  _p4_off_t p4_file_copy (const char *src, const char *dst, _p4_off_t limit) /* * Copies file, but at most limit characters. * Returns destination file length if successful, -1 otherwise. */ ; /*{*/

_extern  int p4_file_move (const char *src, const char *dst) ; /*{*/

_extern  int p4_file_resize (const char *fn, _p4_off_t new_size) ; /*{*/

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
