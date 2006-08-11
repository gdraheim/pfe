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
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: memory-alloc-ext.c,v 1.2 2006-08-11 22:56:05 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-limits.h>

#include <stdlib.h>
#include <errno.h>

#include <pfe/_missing.h>
#include <pfe/logging.h>

/** ALLOCATE ( size# -- alloc*! 0 | 0 errno#! ) [ANS]
 * Allocate a chunk of memory from the system heap.
 * use => FREE to release the memory area back to the system. <br>
 * A code of zero means success.
 */
FCode (p4_allocate)
{
    errno = 0;
    if ((*SP = (p4cell) p4_calloc (1, (size_t) * SP)) != 0)
        *--SP = 0;
    else *--SP = errno;
}

/** FREE ( alloc* -- errno# ) [ANS]
 * Free the memory from => ALLOCATE
 * A code of zero means success.
 */
FCode (p4_free)
{
    errno = 0;
    { p4_xfree (*(void **) SP); }
    *SP = errno;
}

/** RESIZE ( alloc* newsize# -- alloc*' errno# ) [ANS]
 * Resize the system memory chunk. A code of zero means success.
 * Our implementation returns the old pointer on failure.
 */
FCode (p4_resize)
{
    void *p;
  
    errno = 0;
    p = realloc ((void *) SP[1], (size_t) SP[0]);
    
    if (p == NULL)
    {
        SP[0] = errno;
    }else{          
        P4_debug2 (14, "realloc %p -> %p", (void*)SP[1], p);
        SP[0] = 0;
        SP[1] = (p4cell) p;
    }
}

P4_LISTWORDS (memory) =
{
    P4_INTO ("[ANS]", 0),
    P4_FXco ("ALLOCATE",	p4_allocate),
    P4_FXco ("FREE",		p4_free),
    P4_FXco ("RESIZE",		p4_resize),

    P4_INTO ("ENVIRONMENT", 0 ),
    P4_OCON ("MEMORY-ALLOC-EXT",	1994 ),
    
};
P4_COUNTWORDS (memory, "Memory-Alloc extension");

/*@}*/

