#ifndef _PFE_ENGINE_SET_H
#define _PFE_ENGINE_SET_H 1105542193
/* generated 2005-0112-1603 ../../../pfe/../mk/Make-H.pl ../../../pfe/engine-set.c */

# include <pfe/def-types.h>
# include <pfe/def-comp.h>

/** 
 * -- setup forth memory and start up.
 * 
 *  Copyright (C) Tektronix, Inc. 1998 - 2003. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 33.63 %
 *    (%date_modified: Wed Aug 14 16:10:36 2002 %)
 *
 *  @description
 *  Process options via options block (set in option-set), get memory 
 *  and init variables, and finally start up the interpret loop of PFE
 */

#ifdef __cplusplus
extern "C" {
#endif




extern P4_CODE(p4_script_files);

_extern  p4_threadP p4_main_threadP ; /*=*/

/** 
 * init and execute the previously allocated forth-maschine,
 * e.g. pthread_create(&thread_id,0,p4_Exec,threadP);
 *
 * The following words have been extracted from a big boot init
 * procedure previously existing in PFE. In the boot_system we
 * do initialize all inputs/outputs and load the wordset extensions
 * and the boot-preinit-block or boot-preinit-script. After that,
 * we run script_files to init the application code, and finally
 * the application is started - and if no APPLICATION was set then
 * we do the fallback to the forth interactive INTERPRET loop. The
 * latter is the usual case, use BYE to exit that inifinite loop.
 *
 * When the mainloop returns, we run the cleanup-routines. They are
 * registered seperatly so they can be run asynchronously - if the
 * application has broken down or it blocks hard on some hardware
 * then we can still run cleanup code in a new forthish context.
 */
_extern  int p4_Exec(p4_threadP th) ; /*{*/

_extern  int p4_InitVM(p4_threadP th, p4_Session* set) ; /*{*/

_extern  int p4_LoopVM(p4_threadP th) ; /*{*/

_extern  int p4_Evaluate(p4_threadP th, const p4_char_t* p, int n) ; /*{*/

_extern  int p4_DeinitVM(p4_threadP th) ; /*{*/

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
