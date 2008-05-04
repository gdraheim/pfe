#ifndef _VOL_8_SRC_CVS_PFE_33_PFE_DICT_COMP_H
#define _VOL_8_SRC_CVS_PFE_33_PFE_DICT_COMP_H 1209868837
/* generated 2008-0504-0440 /vol/8/src/cvs/pfe-33/pfe/../mk/Make-H.pl /vol/8/src/cvs/pfe-33/pfe/dict-comp.c */

#include <pfe/def-comp.h>

/** 
 *  Compile definitions, load-time with load-wordl, runtime with compile-comma
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.5 $
 *     (modified $Date: 2008-05-04 02:57:30 $)
 */

#ifdef __cplusplus
extern "C" {
#endif


extern p4xcode* p4_compile_comma (p4xcode* at, p4xt);
extern p4xcode* p4_compile_xcode (p4xcode* at, p4xcode);
extern p4xcode* p4_compile_xcode_CODE (p4xcode* at, p4xcode);
extern p4xcode* p4_compile_xcode_BODY (p4xcode* at, p4xcode, p4cell*);


extern P4_CODE (p4_forget_wordset_RT);

_extern  void p4_load_words (const p4Words* ws, p4_Wordl* wid, int unused) ; /*{*/

_extern  void p4_sbr_call (p4xt xt) ; /*{*/

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
