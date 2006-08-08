/** 
 * -- StackHelp for Block-Ext
 * 
 *  Copyright (C) Tektronix, Inc. 2003 - 2003. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 33.12 %
 *    (%date_modified: %)
 *
 *  @description
 *      see stackhelp-ext for details
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: block-stk.c,v 1.1.1.1 2006-08-08 09:09:05 guidod Exp $";
#endif

#define _P4_SOURCE 1
#include <pfe/pfe-base.h>


P4_LISTWORDS (block_check) =
{
    P4_STKi ("BLOCK",    "block# -- block*"),
    P4_STKi ("BUFFER",   "block# -- block*"),
    P4_STKi ("SAVE-BUFFERS", "--"),
    P4_STKi ("EMPTY-BUFFERS", "--"),
    P4_STKi ("FLUSH", "--"),
    P4_STKi ("LIST",  "block# --"),
    P4_STKi ("LOAD",  "block# -- ?? "),
    P4_STKi ("THRU",  "block1# block2# --"),
    P4_STKi ("UPDATE", "--"),
    P4_STKi ("BLK",    "-- block#"),
    P4_STKi ("SCR",    "-- scr"),
    /* block_misc */
    P4_STKi ("CLOSE-BLOCKFILE", "--"),
    P4_STKi ("OPEN-BLOCKFILE", "<filename> --"),
    P4_STKi ("CREATE-BLOCKFILE", "<filename> --"),
    P4_STKi ("SET-BLOCKFILE", "file* --"),
    P4_STKi ("BLOCK-FILE", "-- file*"),
    P4_STKi ("B/BUF", "-- blocksize#"),
};
P4_COUNTWORDS (block_check, "Check-Block words + extension");

/*@}*/

/* 
 * Local variables:
 * c-file-style: "stroustrup"
 * End:
 */
