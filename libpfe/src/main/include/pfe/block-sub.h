#ifndef PFE_BLOCK_SUB_H
#define PFE_BLOCK_SUB_H 1256214391
/* generated 2009-1022-1426 make-header.py ../../c/block-sub.c */

#include <pfe/pfe-sub.h>

/**
 * --  Block-oriented Subroutines
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2008-04-20 04:46:29 $)
 */

#ifdef __cplusplus
extern "C" {
#endif




/**
 * Return best possible access method,
 * 0 if no access but file exists, -1 if file doesn't exist.
 */
_extern  int p4_file_access (const p4_char_t *fn, int len) ; /*{*/

/**
 * open file
 */
_extern  p4_File * p4_open_file (const p4_char_t *name, int len, int mode) ; /*{*/

/**
 * create file
 */
_extern  p4_File * p4_create_file (const p4_char_t *name, int len, int mode) ; /*{*/

/**
 * close file
 */
_extern  int p4_close_file (p4_File *fid) ; /*{*/

/**
 * seek file
 */
_extern  int p4_reposition_file (p4_File *fid, _p4_off_t pos) ; /*{*/

/**
 * read file
 */
_extern  int p4_read_file (void *p, p4ucell *n, p4_File *fid) ; /*{*/

/**
 * write file
 */
_extern  int p4_write_file (void *p, p4ucell n, p4_File *fid) ; /*{*/

/**
 * resize file
 */
_extern  int p4_resize_file (p4_File *fid, _p4_off_t size) ; /*{*/

/**
 * read line
 */
_extern  int p4_read_line (void* buf, p4ucell *u, p4_File *fid, p4cell *ior) ; /*{*/

/**
 * source input: read from block-file
 */
_extern  p4_File * p4_open_blockfile (const p4_char_t *name, int len) ; /*{*/

/**
 * set fid as current block-file, possibly close the old one.
 * (does nothing if argument is null, returns the argument)
 */
_extern  p4_File* p4_set_blockfile (p4_File* fid) ; /*{*/

/**
 * very traditional block read/write primitive
 */
_extern  void p4_read_write (p4_File *fid, void *p, p4ucell n, int readflag) ; /*{*/

/**
 * traditional BUFFER impl
 */
_extern  void* p4_buffer (p4_File *fid, p4ucell n, int *reload) ; /*{*/

/**
 * traditional BLOCK impl
 */
_extern  void* p4_block (p4_File *fid, p4ucell n) ; /*{*/

/**
 * EMPTY-BUFFERS
 */
_extern  void p4_empty_buffers (p4_File *fid) ; /*{*/

/**
 * SAVE-BUFFERS
 */
_extern  void p4_save_buffers (p4_File *fid) ; /*{*/

/**
 * UPDATE
 */
_extern  void p4_update (p4_File *fid) ; /*{*/

/**
 * LIST
 */
_extern  void p4_list (p4_File *fid, int n) ; /*{*/

/**
 * => INTERPET file
 */
_extern  void p4_load (p4_File *fid, p4ucell blk) ; /*{*/

/**
 * open and => LOAD
 */
_extern  void p4_load_file (const p4_char_t *fn, int cnt, int blk) ; /*{*/

/**
 * => THRU
 */
_extern  void p4_thru (p4_File *fid, int lo, int hi) ; /*{*/

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
