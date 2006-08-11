#ifndef _PFE_BLOCK_EXT_H
#define _PFE_BLOCK_EXT_H 1155333834
/* generated 2006-0812-0003 ../../pfe/../mk/Make-H.pl ../../pfe/block-ext.c */

#include <pfe/pfe-ext.h>

/**
 * BLOCK -- BLOCK file access
 * 
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2006 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.2 $
 *     (modified $Date: 2006-08-11 22:56:04 $)
 *
 *  @description
 *     Historically Forth was implemented on small computers as an
 *     operating system in its own right. Mass storage was not
 *     organized in files but as a sequence of 1 KB blocks. A block
 *     was addressed with a block number. This way a diskette drive
 *     provided a few hundred blocks and if you had a fixed disk
 *     you simply had thousands of those blocks.
 *
 *     Both program text and arbitrary data can be stored in blocks.
 *     In order to hold source text the 1K block is treated as
 *     having 16 lines with 64 charactes each. This is often 
 *     referred to as a 'screen'.
 *
 *     When loading (i.e. interpreting) a block with source text it
 *     is simply taking to be a single line of 1024 characters. The
 *     only exception to this is the word => \ (begin comment to
 *     end of line) which skips text up to the end of a 64-char line
 *     in a block.
 */

#ifdef __cplusplus
extern "C" {
#endif




/** BLOCK ( block-u -- block-addr ) [ANS]
 * load the specified block into a block buffer
 * and return the address of that block buffer
 * - see also => BUFFER
 */
extern P4_CODE (p4_block);

/** BUFFER ( block-u -- block-addr ) [ANS]
 * get the block buffer address for the specified 
 * block - if it had not been loaded already it
 * is <b>not</b> filled with data from the disk
 * unlike => BLOCK does.
 */
extern P4_CODE (p4_buffer);

/** SAVE-BUFFERS ( -- ) [ANS]
 * write all modified buffer to
 * the disk, see => UPDATE and
 * => FLUSH
 */
extern P4_CODE (p4_save_buffers);

/** EMPTY-BUFFERS ( -- ) [ANS]
 * unassign all block buffers, does not even => UPDATE
 */
extern P4_CODE (p4_empty_buffers);

/** FLUSH ( -- ) [ANS]
 * call => SAVE-BUFFERS and then unassign all
 * block buffers with => EMPTY-BUFFERS
 */
extern P4_CODE (p4_flush);

/** LIST ( block-u -- ) [ANS]
 * display the block
 */
extern P4_CODE (p4_list);

/** LOAD ( block-u -- ?? ) [FORTH]
 * => INTERPRET the specified => BLOCK
 */
extern P4_CODE (p4_load);

/** THRU ( block1-u block2-u -- ) [ANS]
 * => LOAD a number of block in sequence.
 */
extern P4_CODE (p4_thru);

/** UPDATE ( -- ) [ANS]
 * mark the current block buffer as modified,
 * see => FLUSH
 */
extern P4_CODE (p4_update);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
