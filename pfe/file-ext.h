#ifndef _PFE_FILE_EXT_H
#define _PFE_FILE_EXT_H 984413842
/* generated 2001-0312-1717 ../../pfe/../mk/Make-H.pl ../../pfe/file-ext.c */

#include <pfe/incl-ext.h>

/** 
 * FILE ---  Optional File-Access Word Set
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 5.5 %
 *    (%date_modified: Mon Mar 12 10:32:18 2001 %)
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




/** BIN ( access-mode -- access-mode' )
 * modify the give file access-mode to be a binary-mode
 */
extern P4_CODE (p4_bin);

/** CLOSE-FILE ( file -- code )
 * close the file and return the status-code
 */
extern P4_CODE (p4_close_file);

/** CREATE-FILE ( str-adr str-len mode -- file code )
 * create the file with the given name and open
 * it - returns the file id and a status code.
 * A code of zero means success. An existing file
 * of the same name is truncated upon open.
 */
extern P4_CODE (p4_create_file);

/** DELETE-FILE ( str-adr str-len -- code )
 * delete the named file and return a status code
 */
extern P4_CODE (p4_delete_file);

/** FILE-POSITION ( file -- p.pos code )
 * return the current position in the file and
 * return a status code. A code of zero means success.
 */
extern P4_CODE (p4_file_position);

/** FILE-SIZE ( file -- s.size code )
 * return the current size of the file and
 * return a status code. A code of zero means success.
 */
extern P4_CODE (p4_file_size);

/** INCLUDE-FILE ( file -- )
 * => INTERPRET the given file
 */
extern P4_CODE (p4_include_file);

/** INCLUDED ( str-adr str-len -- )
 * open the named file and then => INCLUDE-FILE
 * see also the interactive => INCLUDE
 */
extern P4_CODE (p4_included);

/** OPEN-FILE ( str-adr str-len mode -- file code )
 * open the named file with mode. returns the
 * file id and a status code. A code of zero
 * means success.
 */
extern P4_CODE (p4_open_file);

/** READ-FILE ( str-adr str-len file -- count code )
 * fill the given string buffer with characters
 * from the buffer. A status code of zero means
 * success and the returned count gives the
 * number of bytes actually read. If an error
 * occurs the number of already transferred bytes 
 * is returned.
 */
extern P4_CODE (p4_read_file);

/** READ-LINE ( str-adr str-len file -- count flag code )
 * fill the given string buffer with one line
 * from the file. A line termination character
 * (or character sequence under WIN/DOS) may
 * also be placed in the buffer but is not
 * included in the final count. In other respects
 * this function performs a => READ-FILE
 */
extern P4_CODE (p4_read_line);

/** REPOSITION-FILE ( o.offset file -- code )
 * reposition the file offset - the next => FILE-POSITION
 * would return o.offset then. returns a status code.
 */
extern P4_CODE (p4_reposition_file);

/** RESIZE-FILE ( s.size file -- code )
 * resize the give file, returns a status code.
 */
extern P4_CODE (p4_resize_file);

/** WRITE-FILE ( str-adr str-len file -- code )
 * write characters from the string buffer to a file,
 * returns a status code.
 */
extern P4_CODE (p4_write_file);

/** WRITE-LINE ( str-adr str-len file -- )
 * write characters from the string buffer to a file,
 * and add the line-terminator to the end of it.
 * returns a status code.
 */
extern P4_CODE (p4_write_line);

/** FILE-STATUS ( str-adr str-len -- sub-code code )
 * check the named file - if it exists
 * the status code is zero. The sub-code
 * is implementation-specific.
 */
extern P4_CODE (p4_file_status);

/** FLUSH-FILE ( file -- code )
 * flush all unsaved buffers of the file to disk.
 * A status code of zero means success.
 */
extern P4_CODE (p4_flush_file);

/** RENAME-FILE ( str-adr1 str-len1 str-adr2 str-len2 -- code )
 * rename the file named by string1 to the name of string2.
 * returns a status-code
 */
extern P4_CODE (p4_rename_file);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
