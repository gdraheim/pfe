#ifndef PFE_EDIT_EXT_H
#define PFE_EDIT_EXT_H 1256212371
/* generated 2009-1022-1352 make-header.py ../../c/edit-ext.c */

#include <pfe/pfe-ext.h>

/**
 * -- simple FORTH-screenfile editor
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2000.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.4 $
 *     (modified $Date: 2008-05-01 21:49:01 $)
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




extern void FXCode_RT (p4_edit_forget);

extern void FXCode (p4_edit_init);

/** EDIT-BLOCK ( blk -- )
 * start the internal block-editor on the assigned block
 */
extern void FXCode (p4_edit_block);

/** EDIT-TEXT name ( -- )
 * start an external => EDITOR with the specified filename
 */
extern void FXCode (p4_edit_text);

/** EDIT-ERROR ( -- )
 * if an error occured, this routine can be called to invoke
 * an appropriate => EDITOR (see also =>"EDIT-BLOCK")
 */
extern void FXCode (p4_edit_error);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
