/** 
 * -- StackHelp for Block-Ext
 * 
 *  Copyright (C) Tektronix, Inc. 2003 - 2003.
 *  Copyright (C) 2005 - 2006 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.2 $
 *     (modified $Date: 2006-08-11 22:56:04 $)
 *
 *  @description
 *      see stackhelp-ext for details
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: block-stk.c,v 1.2 2006-08-11 22:56:04 guidod Exp $";
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
