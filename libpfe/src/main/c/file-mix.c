/**
 * -- miscellaneous useful extra words for FILE-EXT
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2008-04-20 04:46:31 $)
 *
 *  @description
 *      Compatiblity with former standards, miscellaneous useful words.
 *      ... for FILE-EXT
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) =
"@(#) $Id: file-mix.c,v 1.3 2008-04-20 04:46:31 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/file-sub.h>

#include <errno.h>

/** INCLUDE ( "filename" -- ??? ) [FTH]
 * load the specified file, see also => LOAD" filename"
 */
void FXCode (p4_include)
{
    p4_char_t *fn = p4_word (' ');

    p4_included (P4_CHARBUF_PTR(fn), P4_CHARBUF_LEN(fn));
}

/************************************************************************/
/* more file manipulation                                               */
/************************************************************************/

/** COPY-FILE ( src-ptr src-len dst-ptr dst-len -- copy-errno# ) [FTH]
 * like =>'RENAME-FILE', copies the file from src-name to dst-name
 * and returns an error-code or null
 */
void FXCode (p4_copy_file)
{
    char* src = p4_pocket_filename ((p4_char_t *) SP[3], SP[2]);
    char* dst = p4_pocket_filename ((p4_char_t *) SP[1], SP[0]);
    SP += 3;
    *SP = p4_file_copy (src, dst, P4_OFF_T_MAX) ? PFE_io_errno : 0;
}

/** MOVE-FILE ( src-ptr src-len dst-ptr dst-len -- move-errno# ) [FTH]
 * like =>'RENAME-FILE', but also across-volumes <br>
 * moves the file from src-name to dst-name and returns an
 * error-code or null
 */
void FXCode (p4_move_file)
{
    char* src = p4_pocket_filename ((p4_char_t *) SP[3], SP[2]);
    char* dst = p4_pocket_filename ((p4_char_t *) SP[1], SP[0]);
    SP += 3;
    *SP = p4_file_move (src, dst) ? PFE_io_errno : 0;
}

/** FILE-R/W ( buffer* use-block# flag? some-file* -- ) [FTH]
 * like FIG-Forth <c> R/W </c>
 */
void FXCode (p4_file_rw)
{
    p4_read_write (
                   (File *) SP[0],	/* file to read from */
                   (char *) SP[3],	/* buffer address, 1K */
                   (p4ucell) SP[2],	/* block number */
                   SP[0]);		/* readflag */
    SP += 4;
}

/** FILE-BLOCK ( use-block# some-file* -- buffer* ) [FTH]
 */
void FXCode (p4_file_block)
{
    File *fid = (File *) *SP++;

    *SP = (p4cell) p4_block (fid, *SP);
}

/** FILE-BUFFER ( use-block# some-file* -- buffer* ) [FTH]
 */
void FXCode (p4_file_buffer)
{
    File *fid = (File *) *SP++;
    int n;

    *SP = (p4cell) p4_buffer (fid, *SP, &n);
}

/** FILE-EMPTY-BUFFERS ( some-file* -- ) [FTH]
 */
void FXCode (p4_file_empty_buffers)
{
    p4_empty_buffers ((File *) *SP++);
}

/** FILE-FLUSH ( some-file* -- ) [FTH]
 simulate      : FILE-FLUSH DUP FILE-SAVE-BUFFERS FILE-EMTPY-BUFFERS ;
 */
void FXCode (p4_file_flush)
{
    File *fid = (File *) *SP++;

    p4_save_buffers (fid);
    p4_empty_buffers (fid);
}

/** FILE-LIST ( use-block# some-file* -- ) [FTH]
 */
void FXCode (p4_file_list)
{
    File *fid = (File *) *SP++;

    p4_list (fid, SCR = *SP++);
}

/** FILE-LOAD ( use-block# some-file* -- ) [FTH]
 */
void FXCode (p4_file_load)
{
    File *fid = (File *) *SP++;

    p4_load (fid, *SP++);
}

/** FILE-SAVE-BUFFERS ( some-file* -- ) [FTH]
 */
void FXCode (p4_file_save_buffers)
{
    File *fid = (File *) *SP++;

    p4_save_buffers (fid);
}

/** FILE-THRU ( lo-block# hi-block# some-file* -- ) [FTH]
 * see => THRU
 */
void FXCode (p4_file_thru)
{
    File *fid = (File *) *SP++;
    int hi = *SP++;
    int lo = *SP++;

    p4_thru (fid, lo, hi);
}

/** FILE-UPDATE ( some-file* -- ) [FTH]
 */
void FXCode (p4_file_update)
{
    p4_update ((File *) *SP++);
}


P4_LISTWORDSET (file_misc) [] =
{
    P4_INTO ("FORTH", "[ANS]"),
    P4_FXco ("INCLUDE",		p4_include),

    /* more file-manipulation */
    P4_FXco ("COPY-FILE",	p4_copy_file),
    P4_FXco ("MOVE-FILE",	p4_move_file),
    P4_FXco ("FILE-R/W",	p4_file_rw),
    /** the FILE-operations can can also be => USING blocks from a file */
    P4_FXco ("FILE-BLOCK",	p4_file_block),
    P4_FXco ("FILE-BUFFER",	p4_file_buffer),
    P4_FXco ("FILE-EMPTY-BUFFERS", p4_file_empty_buffers),
    P4_FXco ("FILE-FLUSH",	p4_file_flush),
    P4_FXco ("FILE-LIST",	p4_file_list),
    P4_FXco ("FILE-LOAD",	p4_file_load),
    P4_FXco ("FILE-SAVE-BUFFERS", p4_file_save_buffers),
    P4_FXco ("FILE-THRU",	p4_file_thru),
    P4_FXco ("FILE-UPDATE",	p4_file_update),
};
P4_COUNTWORDSET (file_misc, "FILE-Misc Compatibility words");

/*@}*/
