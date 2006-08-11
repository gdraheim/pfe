#ifndef _PFE_MEMORY_ALLOC_EXT_H
#define _PFE_MEMORY_ALLOC_EXT_H 1155333835
/* generated 2006-0812-0003 ../../pfe/../mk/Make-H.pl ../../pfe/memory-alloc-ext.c */

#include <pfe/pfe-ext.h>

/** 
 * -- The Optional Memory Allocation Word Set
 * 
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2006 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.2 $
 *     (modified $Date: 2006-08-11 22:56:05 $)
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
