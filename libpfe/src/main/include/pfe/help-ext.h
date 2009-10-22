#ifndef PFE_HELP_EXT_H
#define PFE_HELP_EXT_H 1256212373
/* generated 2009-1022-1352 make-header.py ../../c/help-ext.c */

#include <pfe/pfe-ext.h>

/**
 * -- extra words for external HELP system
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
 *      we grep the installed header files for comments ;-)
 */

#ifdef __cplusplus
extern "C" {
#endif




/** (HELP) ( str-ptr str-len -- )
 * display help for the specified word
 * (not functional yet)
 */
extern void FXCode (p4_paren_help);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
