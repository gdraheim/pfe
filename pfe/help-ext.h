#ifndef _PFE_HELP_EXT_H
#define _PFE_HELP_EXT_H 1158897469
/* generated 2006-0922-0557 ../../pfe/../mk/Make-H.pl ../../pfe/help-ext.c */

#include <pfe/pfe-ext.h>

/** 
 * -- extra words for external HELP system
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2006 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2006-09-22 04:43:03 $)
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
