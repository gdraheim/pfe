/**
 * BLOCK -- BLOCK file access
 * 
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 5.5 %
 *    (%date_modified: Mon Mar 12 10:31:59 2001 %)
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
"@(#) $Id: block-ext.c,v 0.30.86.1 2001-03-12 09:31:59 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-xtra.h>

#include <string.h>
#include <limits.h>

#include <pfe/block-sub.h>
#include <pfe/_missing.h>

/** BLOCK ( u -- addr )
 * load the specified block into a block buffer
 * and return the address of that block buffer
 * - see also => BUFFER
 */
FCode (p4_block)
{
    *SP = (p4cell) p4_block (BLOCK_FILE, *SP);
}

/** BUFFER ( u -- addr )
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

/** SAVE-BUFFERS ( -- )
 * write all modified buffer to
 * the disk, see => UPDATE and
 * => FLUSH
 */
FCode (p4_save_buffers)
{
    p4_save_buffers (BLOCK_FILE);
}

/** EMPTY-BUFFERS ( -- )
 * unassign all block buffers, does not even => UPDATE
 */
FCode (p4_empty_buffers)
{
    p4_empty_buffers (BLOCK_FILE);
}

/** FLUSH ( -- )
 * call => SAVE-BUFFERS and then unassign all
 * block buffers with => EMPTY-BUFFERS
 */
FCode (p4_flush)
{
    FX (p4_save_buffers);
    FX (p4_empty_buffers);
}

/** LIST ( u -- )
 * display the block
 */
FCode (p4_list)
{
    p4_list (BLOCK_FILE, SCR = *SP++);
}

/** LOAD ( u -- )
 * => INTERPRET the specified => BLOCK
 */
FCode (p4_load)
{
    p4_load (BLOCK_FILE, *SP++);
}

/** THRU ( u1 u2 -- )
 * => LOAD a number of block in sequence.
 */
FCode (p4_thru)
{
    int hi = *SP++;
    int lo = *SP++;

    p4_thru (BLOCK_FILE, lo, hi);
}

/** UPDATE ( -- )
 * mark the current block buffer as modified,
 * see => FLUSH
 */
FCode (p4_update)
{
    p4_update (BLOCK_FILE);
}

/** BLK ( -- val )
 * the direct use of => BLK and => SCR is depracated
 * (very traditional variables for I/O system)
 */

/** SCR ( -- val )
 * the direct use of => BLK and => SCR is depracated
 * (very traditional variables for I/O system)
 */

P4_LISTWORDS (block) =
{
    DV ("BLK", input.blk),
    CO ("BLOCK", p4_block),
    CO ("BUFFER", p4_buffer),
    CO ("EMPTY-BUFFERS", p4_empty_buffers),
    CO ("FLUSH", p4_flush),
    CO ("LIST", p4_list),
    CO ("LOAD", p4_load),
    CO ("SAVE-BUFFERS", p4_save_buffers),
    DV ("SCR", scr),
    CO ("THRU", p4_thru),
    CO ("UPDATE", p4_update),
    P4_INTO ("ENVIRONMENT", 0 ),
    /* enviroment hints (testing for -EXT will mark this wordset as present) */
    P4_OCON ("BLOCK-EXT",	1994 ),
};
P4_COUNTWORDS (block, "Block Words + extensions");

/*@}*/
