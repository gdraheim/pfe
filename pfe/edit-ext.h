#ifndef _PFE_EDIT_EXT_H
#define _PFE_EDIT_EXT_H 984413841
/* generated 2001-0312-1717 ../../pfe/../mk/Make-H.pl ../../pfe/edit-ext.c */

#include <pfe/incl-ext.h>

/** 
 * -- simple FORTH-screenfile editor
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2000. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE            @(#) %derived_by: guidod %
 *  @version %version: 5.9 %
 *    (%date_modified: Mon Mar 12 10:32:12 2001 %)
 *
 *  @description
 *       This is a simple fullscreen FORTH block editor.
 *
 *       will be missing in most builds
 *
 */

#ifdef __cplusplus
extern "C" {
#endif




/** EDIT-BLOCK ( blk -- )
 * start the internal block-editor on the assigned block
 */
extern P4_CODE (p4_edit_block);

/** EDIT-TEXT name ( -- )
 * start an external => EDITOR with the specified filename
 */
extern P4_CODE (p4_edit_text);

/** EDIT-ERROR ( -- )
 * if an error occured, this routine can be called to invoke
 * an appropriate => EDITOR (see also =>"EDIT-BLOCK")
 */
extern P4_CODE (p4_edit_error);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
