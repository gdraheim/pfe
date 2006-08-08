#ifndef _PFE_OPTION_SET_H
#define _PFE_OPTION_SET_H 1105542193
/* generated 2005-0112-1603 ../../../pfe/../mk/Make-H.pl ../../../pfe/option-set.c */

# include <pfe/def-types.h>
# include <pfe/def-words.h>

/** 
 * -- Process command line, init option block, prepare for start.
 * 
 *  Copyright (C) Tektronix, Inc. 1998 - 2003. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 33.59 %
 *    (%date_modified: Wed Aug 14 16:10:36 2002 %)
 *
 *  @description
 *  Process command line, init option block, prepare for start.
 *  The init of ofe memory and start up the interpret loop in PFE
 *  is done in the engine-set part.
 */

#ifdef __cplusplus
extern "C" {
#endif




/**
 * fill the session struct with precompiled options
 */
_extern  void p4_SetOptionsDefault(p4_sessionP set, int len) ; /*{*/

/**
 * parse the command-line options and put them into the session-structure
 * that is used in thread->set. 
 * returns status code (0 == ok, 1 == normal, 2 == error)
 *
 * note, that these argc/argv are given as references! 
 */
_extern  int p4_AddOptions (p4_sessionP set, int argc, const char** argv) ; /*{*/

/**
 * initalize the session struct
 *
 * => p4_SetOptionsDefault , => p4_AddOptions , => FreeOptions
 */
_extern  int p4_SetOptions (p4_sessionP set, int len, int argc, const char** argv) ; /*{*/

/** 
 * de-init the session struct
 *
 * => p4_SetOptions , => p4_AddOptions
 */
_extern  int p4_FreeOptions (int returncode, p4_sessionP set) ; /*{*/

/**
 * set prelinked-modules-table
 */
_extern  int p4_SetModules (p4_sessionP set, p4Words* modules) ; /*{*/

_extern  p4_sessionP p4_NewSessionOptions (int extra) ; /*{*/

_extern  p4_threadP p4_NewThreadOptions (p4_sessionP set) ; /*{*/

_extern  p4_threadP p4_SetThreadOf(p4_threadP ptr, p4_sessionP set) ; /*{*/

_extern  char p4_FreeSessionPtr (p4_sessionP ptr) ; /*{*/

_extern  char p4_FreeThreadPtr (p4_threadP ptr) ; /*{*/

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
