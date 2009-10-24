/**
 * BLOCK -- BLOCK file access
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
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) =
"@(#) $Id$";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-limits.h>

#include <pfe/os-string.h>
#include <limits.h>

#include <pfe/block-sub.h>
#include <pfe/_missing.h>

/** BLOCK ( block-u -- block-addr ) [ANS]
 * load the specified block into a block buffer
 * and return the address of that block buffer
 * - see also => BUFFER
 */
void FXCode (p4_block)
{
    *SP = (p4cell) p4_blockfile_block (BLOCK_FILE, *SP);
}

/** BUFFER ( block-u -- block-addr ) [ANS]
 * get the block buffer address for the specified
 * block - if it had not been loaded already it
 * is <b>not</b> filled with data from the disk
 * unlike => BLOCK does.
 */
void FXCode (p4_buffer)
{
    *SP = (p4cell) p4_blockfile_buffer (BLOCK_FILE, *SP);
}

/** SAVE-BUFFERS ( -- ) [ANS]
 * write all modified buffer to
 * the disk, see => UPDATE and
 * => FLUSH
 */
void FXCode (p4_save_buffers)
{
    p4_blockfile_save_buffers (BLOCK_FILE);
}

/** EMPTY-BUFFERS ( -- ) [ANS]
 * unassign all block buffers, does not even => UPDATE
 */
void FXCode (p4_empty_buffers)
{
    p4_blockfile_empty_buffers (BLOCK_FILE);
}

/** FLUSH ( -- ) [ANS]
 * call => SAVE-BUFFERS and then unassign all
 * block buffers with => EMPTY-BUFFERS
 */
void FXCode (p4_flush)
{
    FX (p4_save_buffers);
    FX (p4_empty_buffers);
}

/** LIST ( block-u -- ) [ANS]
 * display the block
 */
void FXCode (p4_list)
{
    p4_blockfile_list (BLOCK_FILE, SCR = *SP++);
}

/** LOAD ( block-u -- ?? ) [FORTH]
 * => INTERPRET the specified => BLOCK
 */
void FXCode (p4_load)
{
    p4_blockfile_load (BLOCK_FILE, *SP++);
}

/** THRU ( block1-u block2-u -- ) [ANS]
 * => LOAD a number of blocks in sequence.
 *
 * see also => FILE-THRU
 */
void FXCode (p4_thru)
{
    p4_blk_t hi = (p4_blk_t) *SP++;
    p4_blk_t lo = (p4_blk_t) *SP++;

    p4_blockfile_thru (BLOCK_FILE, lo, hi);
}

/** UPDATE ( -- ) [ANS]
 * mark the current block buffer as modified,
 * see => FLUSH
 */
void FXCode (p4_update)
{
    p4_blockfile_update (BLOCK_FILE);
}

/** BLK ( -- block-u* ) [ANS]
 * the direct use of => BLK and => SCR is depracated
 * (very traditional variables for I/O system)
 */

/** SCR ( -- scr-n* ) [ANS]
 * the direct use of => BLK and => SCR is depracated
 * (very traditional variables for I/O system)
 */

/** */
P4_LISTWORDSET (block) [] =
{
    P4_INTO ("[ANS]", 0), /* block */
    P4_DVaR ("BLK",		input.blk),
    P4_FXco ("BLOCK",		p4_block),
    P4_FXco ("BUFFER",		p4_buffer),
    P4_FXco ("FLUSH",		p4_flush),
    P4_FXco ("LOAD",		p4_load),
    P4_FXco ("SAVE-BUFFERS",	p4_save_buffers),
    P4_FXco ("UPDATE",		p4_update),
    /* FXco ("EVALUATE",	...) */

    P4_INTO ("[ANS]", 0), /* block ext */
    P4_FXco ("EMPTY-BUFFERS",	p4_empty_buffers),
    P4_FXco ("LIST",		p4_list),
    P4_DVaR ("SCR",		scr),
    P4_FXco ("THRU",		p4_thru),
    /* FXco ("REFILL",		...) */
    /* FXco ("\\",		...) */

    P4_INTO ("ENVIRONMENT", 0 ),
    /* enviroment hints (testing for -EXT will mark this wordset as present) */
    P4_OCON ("BLOCK-EXT",	1994 ),
};
P4_COUNTWORDSET (block, "Block Words + extensions");

/*@}*/
