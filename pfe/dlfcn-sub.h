#ifndef _PFE_DLFCN_SUB_H
#define _PFE_DLFCN_SUB_H 985126515
/* generated 2001-0320-2315 ../../pfe/../mk/Make-H.pl ../../pfe/dlfcn-sub.c */

#include <pfe/def-config.h>

/** 
 * -- Words to open a shared code object
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE                %derived_by: guidod %
 *  @version %version: 1.9 %
 *    (%date_modified: Mon Mar 12 10:32:07 2001 %)
 *
 *  @description
 *		This file exports a set of system words for 
 *              any OS that can dynamically bind object code to
 *		the interpreter. This part will then try to look
 *              up a symbol that can return a loadlist-table to
 *              be fed to the loadlist-loader routine.
 */

#ifdef __cplusplus
extern "C" {
#endif




/** dlfcn: init dl symbol table, dl error */
_extern  int p4_dlinit (void) ; /*{*/

/** dlfcn: describe last dl-error */
_extern  const char* p4_dlerror (void) ; /*{*/

/** dlfcn: load shared-object into program codespace */
_extern  void* p4_dlopen (const char* dir, const char* name) ; /*{*/

/** dlfcn: remove shared-object from program codespace */
_extern  int p4_dlclose (const void* lib) ; /*{*/

/** dlfcn: find symbol in loaded object */
_extern  void* p4_dlsym (const void* lib, const char* symbol) ; /*{*/

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
