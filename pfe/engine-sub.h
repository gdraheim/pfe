#ifndef _PFE_ENGINE_SUB_H
#define _PFE_ENGINE_SUB_H 984413839
/* generated 2001-0312-1717 ../../pfe/../mk/Make-H.pl ../../pfe/engine-sub.c */

#include <pfe/incl-sub.h>

/** 
 * --  Subroutines for the Internal Forth-System
 * 
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE            @(#) %derived_by: guidod %
 *  @version %version: 1.11 %
 *    (%date_modified: Mon Mar 12 14:09:49 2001 %)
 */

#ifdef __cplusplus
extern "C" {
#endif




/**
 * longjmp via (jumpbuf*) following inline
 * - purpose: stop the inner interpreter
 */
extern P4_CODE (p4_jump);

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
 * Run a forth word from within C-code
 * - this is the inner interpreter
 */
_extern  void p4_run_forth (p4xt xt) ; /*{*/

/**
 */
_extern  void p4_call (p4xt xt) ; /*{*/

/**
 * the NEXT call. Can be replaced by p4_debug_execute to
 * trace the inner forth interpreter.
 */
_extern  void p4_normal_execute (p4xt xt) ; /*{*/

/**
 * => INTERPRET buffer
 */
_extern  void p4_evaluate (char *p, int n) ; /*{*/

/**
 */
_extern  void p4_include_file (p4_File *fid) ; /*{*/

/**
 * called by INCLUDED and INCLUDE
 */
_extern  int p4_included1 (const char *name, int len, int throws) ; /*{*/

/**
 * INCLUDED
 */
_extern  void p4_included (const char* name, int len) ; /*{*/

_extern  void p4_unnest_input (p4_Iframe *p) ; /*{*/

/** 
 * the outer interpreter, in PFE the jumppoint for both => ABORT and => QUIT
 */
_extern  int p4_interpret_loop () ; /*{*/

/**
 * setup all system variables and initialize the dictionary
 */
_extern  void p4_boot_system (void) ; /*{*/

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
