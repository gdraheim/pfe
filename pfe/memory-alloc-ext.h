#ifndef _PFE_MEMORY_ALLOC_EXT_H
#define _PFE_MEMORY_ALLOC_EXT_H 1105051253
/* generated 2005-0106-2340 ../../../pfe/../mk/Make-H.pl ../../../pfe/memory-alloc-ext.c */

#include <pfe/pfe-ext.h>

/** 
 * -- The Optional Memory Allocation Word Set
 * 
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: bln_mpt1!33.08 %
 *    (%date_modified: Mon Mar 12 10:32:31 2001 %)
 *
 *  @description
 *     The optional memory allocation wordset interfaces to
 *     the surrounding OS heap memory management.
 */

#ifdef __cplusplus
extern "C" {
#endif




/** ALLOCATE ( size# -- alloc*! 0 | 0 errno#! ) [ANS]
 * Allocate a chunk of memory from the system heap.
 * use => FREE to release the memory area back to the system. <br>
 * A code of zero means success.
 */
extern P4_CODE (p4_allocate);

/** FREE ( alloc* -- errno# ) [ANS]
 * Free the memory from => ALLOCATE
 * A code of zero means success.
 */
extern P4_CODE (p4_free);

/** RESIZE ( alloc* newsize# -- alloc*' errno# ) [ANS]
 * Resize the system memory chunk. A code of zero means success.
 * Our implementation returns the old pointer on failure.
 */
extern P4_CODE (p4_resize);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
