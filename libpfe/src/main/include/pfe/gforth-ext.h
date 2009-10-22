#ifndef PFE_GFORTH_EXT_H
#define PFE_GFORTH_EXT_H 1256209149
/* generated 2009-1022-1259 make-header.py ../../c/gforth-ext.c */

#include <pfe/pfe-ext.h>

/**
 * -- Words designed to mimic gforth behaviour.
 *
 *  Copyright (C) Guido U. Draheim 2001 - 2004.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2008-04-20 04:46:31 $)
 *
 *  @description
 *		Usually the following words are defined to mimic
 *		a certain gforth extension that some application
 *		writers need. They are only added on request, and
 *		they may be removed without warning requesting to
 *              use the functionality provided by pfe itself.
 */

#ifdef __cplusplus
extern "C" {
#endif




/** open-dir   ( c_addr u -- wdirid wior )  gforth  open_dir
 * will vanish without warning. see gforth documentation.
 */
extern P4_CODE(p4_gforth_open_dir);

/** read-dir   ( c_addr u1 wdirid -- u2 flag wior )  gforth  read_dir
 * will vanish without warning. see gforth documentation.
 */
extern P4_CODE(p4_gforth_read_dir);

/** close-dir       ( wdirid -- wior )      gforth  close_dir
 * will vanish without warning. see gforth documentation.
 */
extern P4_CODE(p4_gforth_close_dir);

/** linked ( list -- ) \ gforth
 : linked        here over @ a, swap ! ;
 * (note: win32forth calls it "link," )
 */
extern P4_CODE (p4_gforth_linked);

/** chained       ( xt list -- ) \ gforth
 * generic chains
 : chained  linked , ;
 */
extern P4_CODE(p4_gforth_chained);

/** chainperform  ( list -- ) \ gforth
 *
 : chainperform  BEGIN @ dup WHILE dup cell+ perform REPEAT drop ;
 */
extern P4_CODE (p4_gforth_chainperform);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
