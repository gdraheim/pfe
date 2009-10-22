#ifndef PFE_FILE_EXT_H
#define PFE_FILE_EXT_H 1256212372
/* generated 2009-1022-1352 make-header.py ../../c/file-ext.c */

#include <pfe/pfe-ext.h>

/**
 * FILE ---  Optional File-Access Word Set
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
 *       The Optional File-Access Word Set and
 *       File-Access Extension Words.
 *       These words imply some kind of file-system unlike
 *       the BLOCK wordset.
 *
 */

#ifdef __cplusplus
extern "C" {
#endif




/** BIN ( access-mode# -- access-mode#' ) [ANS]
 * modify the give file access-mode to be a binary-mode
 */
extern void FXCode (p4_bin);

/** CLOSE-FILE ( some-file* -- some-errno# ) [ANS]
 * close the file and return the status-code
 */
extern void FXCode (p4_close_file);

/** CREATE-FILE ( name-ptr name-len open-mode# -- name-file* name-errno# ) [ANS]
 * create the file with the given name and open
 * it - returns the file id and a status code.
 * A code of zero means success. An existing file
 * of the same name is truncated upon open.
 */
extern void FXCode (p4_create_file);

/** DELETE-FILE ( name-ptr name-len -- name-errno# ) [ANS]
 * delete the named file and return a status code
 */
extern void FXCode (p4_delete_file);

/** FILE-POSITION ( some-file* -- p,pos# some-errno# ) [ANS]
 * return the current position in the file and
 * return a status code. A code of zero means success.
 */
extern void FXCode (p4_file_position);

/** FILE-SIZE ( some-file* -- s,size# some-errno# ) [ANS]
 * return the current size of the file and
 * return a status code. A code of zero means success.
 */
extern void FXCode (p4_file_size);

/** INCLUDE-FILE ( some-file* -- ) [ANS]
 * => INTERPRET the given file
 */
extern void FXCode (p4_include_file);

/** INCLUDED ( name-ptr name-len -- ) [ANS]
 * open the named file and then => INCLUDE-FILE
 * see also the interactive => INCLUDE
 */
extern void FXCode (p4_included);

/** OPEN-FILE ( name-ptr name-len open-mode# -- name-file* name-errno# ) [ANS]
 * open the named file with mode. returns the
 * file id and a status code. A code of zero
 * means success.
 */
extern void FXCode (p4_open_file);

/** READ-FILE ( buf-ptr buf-len some-file* -- buf-count some-errno# ) [ANS]
 * fill the given string buffer with characters
 * from the buffer. A status code of zero means
 * success and the returned count gives the
 * number of bytes actually read. If an error
 * occurs the number of already transferred bytes
 * is returned.
 */
extern void FXCode (p4_read_file);

/** READ-LINE ( buf-ptr buf-len some-file* -- buf-count buf-flag some-errno# ) [ANS]
 * fill the given string buffer with one line
 * from the file. A line termination character
 * (or character sequence under WIN/DOS) may
 * also be placed in the buffer but is not
 * included in the final count. In other respects
 * this function performs a => READ-FILE
 */
extern void FXCode (p4_read_line);

/** REPOSITION-FILE ( o,offset# some-file* -- some-errno# ) [ANS]
 * reposition the file offset - the next => FILE-POSITION
 * would return o.offset then. returns a status code where zero means success.
 */
extern void FXCode (p4_reposition_file);

/** RESIZE-FILE ( s,size# some-file* -- some-errno# ) [ANS]
 * resize the give file, returns a status code where zero means success.
 */
extern void FXCode (p4_resize_file);

/** WRITE-FILE ( buf-ptr buf-len some-file* -- some-errno# ) [ANS]
 * write characters from the string buffer to a file,
 * returns a status code where zero means success.
 */
extern void FXCode (p4_write_file);

/** WRITE-LINE ( buf-ptr buf-len some-file* -- some-errno# ) [ANS]
 * write characters from the string buffer to a file,
 * and add the line-terminator to the end of it.
 * returns a status code.
 */
extern void FXCode (p4_write_line);

/** FILE-STATUS ( file-ptr file-len -- file-subcode# file-errno# ) [ANS]
 * check the named file - if it exists
 * the status errno code is zero. The status subcode
 * is implementation-specific and usually matches the
 * file access permission bits of the filesystem.
 */
extern void FXCode (p4_file_status);

/** FLUSH-FILE ( some-file* -- some-errno# ) [ANS]
 * flush all unsaved buffers of the file to disk.
 * A status code of zero means success.
 */
extern void FXCode (p4_flush_file);

/** RENAME-FILE ( oldname-ptr oldname-len newname-ptr newname-len -- newname-errno# ) [ANS]
 * rename the file named by "oldname" to the name of "newname"
 * returns a status-code where zero means success.
 */
extern void FXCode (p4_rename_file);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
