#ifndef _VOL_8_SRC_CVS_PFE_33_PFE_ENGINE_SET_H
#define _VOL_8_SRC_CVS_PFE_33_PFE_ENGINE_SET_H 1209868836
/* generated 2008-0504-0440 /vol/8/src/cvs/pfe-33/pfe/../mk/Make-H.pl /vol/8/src/cvs/pfe-33/pfe/engine-set.c */

#include <pfe/pfe-base.h>

/** 
 * -- setup forth memory and start up.
 * 
 *  Copyright (C) Tektronix, Inc. 1998 - 2003.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.7 $
 *     (modified $Date: 2008-05-04 02:57:30 $)
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
