#ifndef _VOL_8_SRC_CVS_PFE_33_PFE_HELP_EXT_H
#define _VOL_8_SRC_CVS_PFE_33_PFE_HELP_EXT_H 1209868838
/* generated 2008-0504-0440 /vol/8/src/cvs/pfe-33/pfe/../mk/Make-H.pl /vol/8/src/cvs/pfe-33/pfe/help-ext.c */

#include <pfe/pfe-ext.h>

/** 
 * -- extra words for external HELP system
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
 *      we grep the installed header files for comments ;-)
 */

#ifdef __cplusplus
extern "C" {
#endif




/** (HELP) ( str-ptr str-len -- )
 * display help for the specified word
 * (not functional yet)
 */
extern P4_CODE (p4_paren_help);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
