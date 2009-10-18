#ifndef _VOL_8_SRC_CVS_PFE_33_PFE_EDIT_EXT_H
#define _VOL_8_SRC_CVS_PFE_33_PFE_EDIT_EXT_H 1209868838
/* generated 2008-0504-0440 /vol/8/src/cvs/pfe-33/pfe/../mk/Make-H.pl /vol/8/src/cvs/pfe-33/pfe/edit-ext.c */

#include <pfe/pfe-ext.h>

/** 
 * -- simple FORTH-screenfile editor
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2000.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.5 $
 *     (modified $Date: 2008-05-04 02:57:30 $)
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




extern P4_CODE (p4_edit_forget);

extern P4_CODE (p4_edit_init);

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
