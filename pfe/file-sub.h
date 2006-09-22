#ifndef _PFE_FILE_SUB_H
#define _PFE_FILE_SUB_H 1158897468
/* generated 2006-0922-0557 ../../pfe/../mk/Make-H.pl ../../pfe/file-sub.c */

#include <pfe/pfe-sub.h>

/** 
 *  Subroutines for file access
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2006 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2006-09-22 04:43:03 $)
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
