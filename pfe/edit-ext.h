#ifndef _PFE_EDIT_EXT_H
#define _PFE_EDIT_EXT_H 1158897469
/* generated 2006-0922-0557 ../../pfe/../mk/Make-H.pl ../../pfe/edit-ext.c */

#include <pfe/pfe-ext.h>

/** 
 * -- simple FORTH-screenfile editor
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2000.
 *  Copyright (C) 2005 - 2006 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2006-09-22 04:43:03 $)
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
