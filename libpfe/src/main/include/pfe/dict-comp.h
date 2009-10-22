#ifndef PFE_DICT_COMP_H
#define PFE_DICT_COMP_H 1256212371
/* generated 2009-1022-1352 make-header.py ../../c/dict-comp.c */

#include <pfe/def-comp.h>

/**
 *  Compile definitions, load-time with load-wordl, runtime with compile-comma
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.15 $
 *     (modified $Date: 2008-09-11 01:27:20 $)
 */

#ifdef __cplusplus
extern "C" {
#endif


extern p4xcode* p4_compile_comma (p4xcode* at, p4xt);
extern p4xcode* p4_compile_xcode (p4xcode* at, p4xcode);
extern p4xcode* p4_compile_xcode_CODE (p4xcode* at, p4xcode);
extern p4xcode* p4_compile_xcode_BODY (p4xcode* at, p4xcode, p4cell*);


extern void FXCode_RT (p4_forget_wordset_RT);

_extern  void p4_load_words (const p4Words* ws, p4_Wordl* wid, int unused) ; /*{*/

_extern  void p4_sbr_call (p4xt xt) ; /*{*/

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
