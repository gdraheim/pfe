#ifndef _PFE_FILE_SUB_H
#define _PFE_FILE_SUB_H 984413846
/* generated 2001-0312-1717 ../../pfe/../mk/Make-H.pl ../../pfe/file-sub.c */

#include <pfe/incl-sub.h>

/** 
 *  Subroutines for file access
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 5.4 %
 *    (%date_modified: Mon Mar 12 10:32:19 2001 %)
 */

#ifdef __cplusplus
extern "C" {
#endif




_extern  long fsize (FILE * f) /* Result: file length, -1 on error */ ; /*{*/

_extern  long fn_size (const char *fn) /* Result: file length, -1 on error */ ; /*{*/

_extern  long fn_copy (const char *src, const char *dst, long limit) /* * Copies file, but at most limit characters. * Returns destination file length if successful, -1 otherwise. */ ; /*{*/

_extern  int fn_move (const char *src, const char *dst) ; /*{*/

_extern  int fn_resize (const char *fn, long new_size) ; /*{*/

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
