#ifndef _PFE_FILE_MIX_H
#define _PFE_FILE_MIX_H 1105051253
/* generated 2005-0106-2340 ../../../pfe/../mk/Make-H.pl ../../../pfe/file-mix.c */

#include <pfe/pfe-mix.h>

/** 
 * -- miscellaneous useful extra words for FILE-EXT
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version:  33.13 %
 *    (%date_modified:  %)
 *
 *  @description
 *      Compatiblity with former standards, miscellaneous useful words.
 *      ... for FILE-EXT
 */

#ifdef __cplusplus
extern "C" {
#endif




/** INCLUDE ( "filename" -- ??? ) [FTH]
 * load the specified file, see also => LOAD" filename"
 */
extern P4_CODE (p4_include);

/** COPY-FILE ( src-ptr src-len dst-ptr dst-len -- copy-errno# ) [FTH]
 * like =>'RENAME-FILE', copies the file from src-name to dst-name
 * and returns an error-code or null
 */
extern P4_CODE (p4_copy_file);

/** MOVE-FILE ( src-ptr src-len dst-ptr dst-len -- move-errno# ) [FTH]
 * like =>'RENAME-FILE', but also across-volumes <br>
 * moves the file from src-name to dst-name and returns an
 * error-code or null
 */
extern P4_CODE (p4_move_file);

/** FILE-R/W ( buffer* use-block# flag? some-file* -- ) [FTH]
 * like FIG-Forth <c> R/W </c>
 */
extern P4_CODE (p4_file_rw);

/** FILE-BLOCK ( use-block# some-file* -- buffer* ) [FTH]
 */
extern P4_CODE (p4_file_block);

/** FILE-BUFFER ( use-block# some-file* -- buffer* ) [FTH]
 */
extern P4_CODE (p4_file_buffer);

/** FILE-EMPTY-BUFFERS ( some-file* -- ) [FTH]
 */
extern P4_CODE (p4_file_empty_buffers);

/** FILE-FLUSH ( some-file* -- ) [FTH]
 simulate      : FILE-FLUSH DUP FILE-SAVE-BUFFERS FILE-EMTPY-BUFFERS ;
 */
extern P4_CODE (p4_file_flush);

/** FILE-LIST ( use-block# some-file* -- ) [FTH]
 */
extern P4_CODE (p4_file_list);

/** FILE-LOAD ( use-block# some-file* -- ) [FTH]
 */
extern P4_CODE (p4_file_load);

/** FILE-SAVE-BUFFERS ( some-file* -- ) [FTH]
 */
extern P4_CODE (p4_file_save_buffers);

/** FILE-THRU ( lo-block# hi-block# some-file* -- ) [FTH]
 * see => THRU
 */
extern P4_CODE (p4_file_thru);

/** FILE-UPDATE ( some-file* -- ) [FTH]
 */
extern P4_CODE (p4_file_update);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
