#ifndef _PFE_MEMORY_ALLOC_EXT_H
#define _PFE_MEMORY_ALLOC_EXT_H 984413843
/* generated 2001-0312-1717 ../../pfe/../mk/Make-H.pl ../../pfe/memory-alloc-ext.c */

#include <pfe/incl-ext.h>

/** 
 * -- The Optional Memory Allocation Word Set
 * 
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 5.6 %
 *    (%date_modified: Mon Mar 12 10:32:31 2001 %)
 *
 *  @description
 *     The optional memory allocation wordset interfaces to
 *     the surrounding OS heap memory management.
 */

#ifdef __cplusplus
extern "C" {
#endif




/** ALLOCATE ( size -- ptr|0 code )
 * allocate a chunk of memory from the system heap.
 * use => FREE to release the memory area back to the system. <br>
 * a code of zero means success.
 */
extern P4_CODE (p4_allocate);

/** FREE ( ptr -- code )
 * free the memory from => ALLOCATE
 * a code of zero means success.
 */
extern P4_CODE (p4_free);

/** RESIZE ( ptr newsize -- ptr' code )
 * resize the system memory chunk.
 * a code of zero means success.
 */
extern P4_CODE (p4_resize);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
