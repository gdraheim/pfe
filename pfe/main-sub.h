#ifndef _PFE_MAIN_SUB_H
#define _PFE_MAIN_SUB_H 985049931
/* generated 2001-0320-0158 ../../pfe/../mk/Make-H.pl ../../pfe/main-sub.c */

# include <pfe/incl-sub.h>
# include <pfe/def-types.h>


/** 
 * -- Process command line, get memory and start up.
 * 
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 5.22 %
 *    (%date_modified: Mon Mar 12 10:32:30 2001 %)
 *
 *  @description
 *  Process command line, get memory and start up the interpret loop of PFE
 */

#ifdef __cplusplus
extern "C" {
#endif


# define p4_GetOptions     p4_SetOptions
# define p4_DefaultSession p4_SetOptionsDefault
  /* and a few internals */
# define _p4_thread_save() (p4_thread_save(p4_main_threadP))
# define _p4_thread_load() (p4_thread_load(p4_main_threadP))

# ifdef P4_REGTH
# define _p4_thread_local() (p4TH = p4_main_threadP)
# define _p4_thread_ENTER_(arg_threadP) \
   	register p4_threadP _p4_thread_SAVED_ = p4TH; \
                        	p4TH = arg_threadP;
# define _p4_thread_LEAVE_() p4TH = _p4_thread_SAVED_;
# else
# define _p4_thread_local()
# define _p4_thread_ENTER_(ignored)
# define _p4_thread_LEAVE_()
# endif


/** two helper functions (thread_save and thead_load)
 * that can be called from non-pfe compiled context
 */
_extern  void p4_thread_save (p4_threadP p) ; /*{*/

_extern  void p4_thread_load (p4_threadP p) ; /*{*/

/**
 * fill the session struct with precompiled options
 */
_extern  void p4_SetOptionsDefault(p4_sessionP set) ; /*{*/

/**
 * parse the command-line options and put them into the session-structure
 * that is used in thread->set. 
 * returns status code (0 == ok, 1 == normal, 2 == error)
 *
 * note, that these argc/argv are given as references! 
 */
_extern  int p4_AddOptions (p4_sessionP set, int argc, char* argv[]) ; /*{*/

/**
 * initalize the session struct
 *
 * => p4_SetOptionsDefault , => p4_AddOptions , => FreeOptions
 */
_extern  int p4_SetOptions (p4_sessionP set, int argc, char* argv[]) ; /*{*/

/** 
 * de-init the session struct
 *
 * => p4_SetOptions , => p4_AddOptions
 */
_extern  int p4_FreeOptions (int returncode, p4_sessionP set) ; /*{*/

_extern  p4_threadP p4_main_threadP ; /*=*/

/** 
 * init and execute the previously allocated forth-maschine,
 * e.g. pthread_create(&thread_id,0,p4_Exec,threadP);
 */
_extern  int p4_Exec(p4_threadP th) ; /*{*/

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
