/**
 * BLOCK -- BLOCK file access
 * 
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2006 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2006-09-21 19:10:27 $)
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
"@(#) $Id: block-ext.c,v 1.3 2006-09-21 19:10:27 guidod Exp $";
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
FCode (p4_block)
{
    *SP = (p4cell) p4_block (BLOCK_FILE, *SP);
}

/** BUFFER ( block-u -- block-addr ) [ANS]
 * get the block buffer address for the specified 
 * block - if it had not been loaded already it
 * is <b>not</b> filled with data from the disk
 * unlike => BLOCK does.
 */
FCode (p4_buffer)
{
    int n;

    *SP = (p4cell) p4_buffer (BLOCK_FILE, *SP, &n);
}

/** SAVE-BUFFERS ( -- ) [ANS]
 * write all modified buffer to
 * the disk, see => UPDATE and
 * => FLUSH
 */
FCode (p4_save_buffers)
{
    p4_save_buffers (BLOCK_FILE);
}

/** EMPTY-BUFFERS ( -- ) [ANS]
 * unassign all block buffers, does not even => UPDATE
 */
FCode (p4_empty_buffers)
{
    p4_empty_buffers (BLOCK_FILE);
}

/** FLUSH ( -- ) [ANS]
 * call => SAVE-BUFFERS and then unassign all
 * block buffers with => EMPTY-BUFFERS
 */
FCode (p4_flush)
{
    FX (p4_save_buffers);
    FX (p4_empty_buffers);
}

/** LIST ( block-u -- ) [ANS]
 * display the block
 */
FCode (p4_list)
{
    p4_list (BLOCK_FILE, SCR = *SP++);
}

/** LOAD ( block-u -- ?? ) [FORTH]
 * => INTERPRET the specified => BLOCK
 */
FCode (p4_load)
{
    p4_load (BLOCK_FILE, *SP++);
}

/** THRU ( block1-u block2-u -- ) [ANS]
 * => LOAD a number of block in sequence.
 */
FCode (p4_thru)
{
    int hi = *SP++;
    int lo = *SP++;

    p4_thru (BLOCK_FILE, lo, hi);
}

/** UPDATE ( -- ) [ANS]
 * mark the current block buffer as modified,
 * see => FLUSH
 */
FCode (p4_update)
{
    p4_update (BLOCK_FILE);
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
P4_LISTWORDS (block) =
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
P4_COUNTWORDS (block, "Block Words + extensions");

/*@}*/
