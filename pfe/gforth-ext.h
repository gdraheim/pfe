#ifndef _PFE_GFORTH_EXT_H
#define _PFE_GFORTH_EXT_H 984413847
/* generated 2001-0312-1717 ../../pfe/../mk/Make-H.pl ../../pfe/gforth-ext.c */

#include <pfe/incl-ext.h>

/** 
 * -- Words designed to mimic gforth behaviour.
 *
 *  Copyright (C) Guido Draheim 2001 - xxxx. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Guido Draheim               %derived_by: guidod %
 *  @version %version: 1.4 %
 *    (%date_modified: Mon Mar 12 10:32:23 2001 %)
 *
 *  @description
 *		Usually the following words are defined to mimic
 *		a certain gforth extension that some application
 *		writers need. They are only added on request, and
 *		they may be removed without warning if pfe is going
 *		to get functionality that can be wrapped at the
 *		source-level into sth. quite like gforth does.
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
