/** 
 * -- miscellaneous useful extra words for BLOCK-EXT
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2008-04-20 04:46:29 $)
 *
 *  @description
 *      Compatiblity with former standards, miscellaneous useful words.
 *      ... for BLOCK-EXT
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: block-mix.c,v 1.3 2008-04-20 04:46:29 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/block-sub.h>

#include <errno.h>
#include <pfe/os-string.h>
#include <pfe/logging.h>


/** CLOSE-BLOCKFILE ( -- ) [FTH] w32for
 * w32for-implementation:
 blockhandle -1 <> if flush close-file drop then
 -1 set-blockfile
 * in pfe:
 : CLOSE-BLOCKFILE 
   BLOCK-FILE ?DUP IF FLUSH CLOSE-FILE DROP THEN 
   OFF> BLOCK-FILE ;
 */
FCode (p4_close_blockfile)
{
    if (BLOCK_FILE)
    {
	FX (p4_flush);
	p4_close_file (BLOCK_FILE);
    }
    BLOCK_FILE = 0;
}

/** OPEN-BLOCKFILE ( "filename" -- ) [FTH] w32for
 * w32for-implementation:
   close-blockfile
   parse-word r/w open-file abort" failed to open block-file"
   set-blockfile
   empty-buffers 
 */
FCode (p4_open_blockfile)
{
    FX (p4_close_blockfile);
    p4_word_parseword (' '); *DP=0; /* PARSE-WORD-NOHERE */
    if (! PFE.word.len)
        p4_throw (P4_ON_INVALID_NAME);
    if (! p4_set_blockfile (p4_open_blockfile (PFE.word.ptr, PFE.word.len)))
        p4_throws (FX_IOR, PFE.word.ptr, PFE.word.len);
}


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
FCode (p4_create_blockfile)
{
    register p4_File *fid;
    
    FX (p4_close_blockfile);
    p4_word_parseword (' '); *DP=0; /* PARSE-WORD-NOHERE */
    if (! PFE.word.len)
        p4_throw (P4_ON_INVALID_NAME);
    switch (p4_file_access (PFE.word.ptr, PFE.word.len))
    {
     case -1:
     case 0:
         fid = p4_create_file (PFE.word.ptr, PFE.word.len, FMODE_RWB);
         if (fid == NULL)
             p4_throws (FX_IOR, PFE.word.ptr, PFE.word.len);
         p4_close_file (fid);
    }
    if (! p4_set_blockfile (p4_open_blockfile (PFE.word.ptr, PFE.word.len)))
        p4_throws (FX_IOR, PFE.word.ptr, PFE.word.len);
    p4_resize_file (BLOCK_FILE, (_p4_off_t)(FX_POP)*BPBUF);
}

/** USING ( "filename" -- ) [EXT] [OLD]
 * use filename as a block file 
 * OBSOLETE!! use => OPEN-BLOCKFILE
 : USING OPEN-BLOCKFILE ;
 */
FCode (p4_using)
{
    P4_fail ("DO NOT use USING - use OPEN-BLOCKFILE");
    FX (p4_open_blockfile);
}

/** USING-NEW ( "filename" -- ) [EXT] [OLD]
 * like => USING but can create the file
 * OBSOLETE!! use => CREATE-BLOCKFILE
 : USING-NEW 0 CREATE-BLOCKFILE ;
 */
FCode (p4_using_new)
{
    P4_fail ("DO NOT use USING-NEW - use 0 CREATE-BLOCKFILE");
    FX_PUSH (0);
    FX (p4_create_blockfile);
}

/** SET-BLOCKFILE ( block-file* -- ) [EXT] win32for
 * win32forth uses a system-filedescriptor where -1 means unused
 * in the BLOCKHANDLE, but we use a "FILE*"-like structure, so NULL
 * means NOT-IN-USE. Here we set it.
 */
FCode(p4_set_blockfile)
{
    p4_set_blockfile ((p4_File*) FX_POP);
}

/** BLOCK-FILE ( -- block-file* ) [EXT]
 * return blockhandle as last set by => SET-BLOCKFILE
 */

/** B/BUF ( -- bytesperbuffer-count ) [EXT]
 */

P4_LISTWORDS (block_misc) =
{
    P4_INTO ("FORTH", "[ANS]"),
    P4_FXco ("CLOSE-BLOCKFILE",		p4_close_blockfile),
    P4_FXco ("OPEN-BLOCKFILE",		p4_open_blockfile),
    P4_FXco ("CREATE-BLOCKFILE",	p4_create_blockfile),

    P4_INTO ("EXTENSIONS", 0),
    P4_DVaL ("BLOCK-FILE",		input.block_file),
    P4_FXco ("SET-BLOCKFILE",		p4_set_blockfile),
    P4_FXco ("_use:OPEN-BLOCKFILE",	p4_using),
    P4_xOLD ("USING",			"_use:OPEN-BLOCKFILE"),
    P4_FXco ("_use:0:CREATE-BLOCKFILE",	p4_using_new),
    P4_xOLD ("USING-NEW",		"_use:0:CREATE-BLOCKFILE"),
    P4_OCoN ("B/BUF",			P4_BPBUF),
};
P4_COUNTWORDS (block_misc, "BLOCK-Misc Compatibility words");

/*@}*/
/* 
 * Local variables:
 * c-file-style: "stroustrup"
 * End:
 */


