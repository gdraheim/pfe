#ifndef _PFE_MEMORY_SUB_H
#define _PFE_MEMORY_SUB_H 1105051254
/* generated 2005-0106-2340 ../../../pfe/../mk/Make-H.pl ../../../pfe/memory-sub.c */

#include <pfe/pfe-sub.h>

/** 
 * -- Memory Allocation Words
 * 
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: bln_mpt1!1.15 %
 *    (%date_modified: Wed Aug 14 16:09:33 2002 %)
 *
 *  @description
 *     memory allocation interfaces to the surrounding OS.
 */

#ifdef __cplusplus
extern "C" {
#endif




_extern  void * p4_xcalloc (int n_elem, size_t size) /* allocate memory, die when failed */ ; /*{*/

_extern  void * p4_calloc (int n_elem, size_t size) /* allocate memory, with debug info */ ; /*{*/

_extern  void * p4_xalloc (size_t size) /* allocate memory, throw when failed */ ; /*{*/

_extern  void p4_xfree (void* p) ; /*{*/

/**
 * helper routine to allocate a portion of the dictionary
 * especially for some stack-areas of the forth system
 * ... just decreases PFE.dictlimit, returns 0 if impossible.
 */
_extern  void* p4_dict_allocate (int items, int size, int align, void** lower, void** upper) ; /*{*/

_extern  int p4_mmap_creat(char* name, void* addr, long size) ; /*{*/

_extern  void p4_mmap_close(int fd, void* addr, long size) ; /*{*/

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
