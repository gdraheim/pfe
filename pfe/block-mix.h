#ifndef _PFE_BLOCK_MIX_H
#define _PFE_BLOCK_MIX_H 1155333834
/* generated 2006-0812-0003 ../../pfe/../mk/Make-H.pl ../../pfe/block-mix.c */

#include <pfe/pfe-mix.h>

/** 
 * -- miscellaneous useful extra words for BLOCK-EXT
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
 *      Compatiblity with former standards, miscellaneous useful words.
 *      ... for BLOCK-EXT
 */

#ifdef __cplusplus
extern "C" {
#endif




/** CLOSE-BLOCKFILE ( -- ) [FTH] w32for
 * w32for-implementation:
 blockhandle -1 <> if flush close-file drop then
 -1 set-blockfile
 * in pfe:
 : CLOSE-BLOCKFILE 
   BLOCK-FILE ?DUP IF FLUSH CLOSE-FILE DROP THEN 
   OFF> BLOCK-FILE ;
 */
extern P4_CODE (p4_close_blockfile);

/** OPEN-BLOCKFILE ( "filename" -- ) [FTH] w32for
 * w32for-implementation:
   close-blockfile
   parse-word r/w open-file abort" failed to open block-file"
   set-blockfile
   empty-buffers 
 */
extern P4_CODE (p4_open_blockfile);

/** CREATE-BLOCKFILE ( blocks-count "filename" -- ) [FTH] w32for
 * w32for-implementation:
   close-blockfile
   parse-word r/w create-file abort" failed to create block-file"
   set-blockfile
   dup b/buf m* blockhandle resize-file
   abort" unable to create a file of that size"
   empty-buffers
   0 do i wipe loop 
   flush
 * pfe does not wipe the buffers
 */
extern P4_CODE (p4_create_blockfile);

/** USING ( "filename" -- ) [EXT] [OLD]
 * use filename as a block file 
 * OBSOLETE!! use => OPEN-BLOCKFILE
 : USING OPEN-BLOCKFILE ;
 */
extern P4_CODE (p4_using);

/** USING-NEW ( "filename" -- ) [EXT] [OLD]
 * like => USING but can create the file
 * OBSOLETE!! use => CREATE-BLOCKFILE
 : USING-NEW 0 CREATE-BLOCKFILE ;
 */
extern P4_CODE (p4_using_new);

/** SET-BLOCKFILE ( block-file* -- ) [EXT] win32for
 * win32forth uses a system-filedescriptor where -1 means unused
 * in the BLOCKHANDLE, but we use a "FILE*"-like structure, so NULL
 * means NOT-IN-USE. Here we set it.
 */
extern P4_CODE(p4_set_blockfile);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
