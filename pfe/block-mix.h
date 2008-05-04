#ifndef _VOL_8_SRC_CVS_PFE_33_PFE_BLOCK_MIX_H
#define _VOL_8_SRC_CVS_PFE_33_PFE_BLOCK_MIX_H 1209868836
/* generated 2008-0504-0440 /vol/8/src/cvs/pfe-33/pfe/../mk/Make-H.pl /vol/8/src/cvs/pfe-33/pfe/block-mix.c */

#include <pfe/pfe-mix.h>

/** 
 * -- miscellaneous useful extra words for BLOCK-EXT
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.5 $
 *     (modified $Date: 2008-05-04 02:57:30 $)
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

/** USING-NEW ( "filename" -- ) [EXT] [obsolete]
 * like => USING but can create the file
 * OBSOLETE word, use => CREATE-BLOCKFILE
 : USING-NEW 0 CREATE-BLOCKFILE ;
 */
extern P4_CODE (p4_zero_create_blockfile);

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
