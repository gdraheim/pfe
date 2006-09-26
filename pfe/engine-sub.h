#ifndef _PFE_ENGINE_SUB_H
#define _PFE_ENGINE_SUB_H 1159277187
/* generated 2006-0926-1526 ../../pfe/../mk/Make-H.pl ../../pfe/engine-sub.c */

#include <pfe/pfe-sub.h>

/** 
 * --  Subroutines for the Internal Forth-System
 * 
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2006 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.4 $
 *     (modified $Date: 2006-09-26 14:10:24 $)
 */

#ifdef __cplusplus
extern "C" {
#endif




extern P4_CODE(p4_noop);

/**
 * longjmp via (Except->jmp) following inline
 * - purpose: stop the inner interpreter
 */
extern P4_CODE (p4_call_stop);

extern P4_CODE (p4_interpret_find);

extern P4_CODE (p4_interpret_number);

/**
 * the => INTERPRET as called by the outer interpreter
 */
extern P4_CODE (p4_interpret);

/**
 * walk the filedescriptors and close/free the fds. This function
 * is usefully called from => ABORT - otherwise it may rip too
 * many files in use.
 */
extern P4_CODE (p4_closeall_files);

/**
 * a little helper that just emits "ok", called in outer interpreter,
 * also useful on the command line to copy lines for re-execution
 */
extern P4_CODE (p4_ok);

extern P4_CODE (p4_paren_abort);

/**
 * setup all system variables and initialize the dictionary
 * to reach a very clean status as if right after cold boot.
 */
extern P4_CODE (p4_cold_system);

/**
 * setup all system variables and initialize the dictionary
 */
extern P4_CODE (p4_boot_system);

/**
 * Run a forth word from within C-code
 * - this is the inner interpreter
 */
_extern  void p4_call_loop (p4xt xt) ; /*{*/

/**
 */
_extern  void p4_call (p4xt xt) ; /*{*/

/**
 * the NEXT call. Can be replaced by p4_debug_execute to
 * trace the inner forth interpreter.
 */
_extern  void p4_normal_execute (p4xt xt) ; /*{*/

/**
 * quick execute - unsafe and slow and simple
 *
 * use this routine for callbacks that might go through some forth
 * colon-routines - that code shall not THROW or do some other nifty
 * tricks with the return-stack or the inner interpreter. 
 * Just simple things - use only for primitives or colon-routines,
 * nothing curried with a DOES part in SBR-threading or sth. like that.
 */
_extern  void p4_simple_execute (p4xt xt) ; /*{*/

/**
 * => INTERPRET buffer
 */
_extern  void p4_evaluate (const p4_char_t *p, int n) ; /*{*/

/**
 */
_extern  void p4_include_file (p4_File *fid) ; /*{*/

/**
 * called by INCLUDED and INCLUDE
 */
_extern  int p4_included1 (const p4_char_t *name, int len, int throws) ; /*{*/

/**
 * INCLUDED
 */
_extern  void p4_included (const p4_char_t* name, int len) ; /*{*/

_extern  void p4_unnest_input (p4_Iframe *p) ; /*{*/

/** 
 * the outer interpreter, in PFE the jumppoint for both => ABORT and => QUIT
 */
_extern  int p4_interpret_loop (P4_VOID) ; /*{*/

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
