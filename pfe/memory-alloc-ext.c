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
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: memory-alloc-ext.c,v 0.30 2001-03-12 09:32:31 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-xtra.h>

#include <stdlib.h>
#include <errno.h>

#include <pfe/_missing.h>
#include <pfe/logging.h>

/** ALLOCATE ( size -- ptr|0 code )
 * allocate a chunk of memory from the system heap.
 * use => FREE to release the memory area back to the system. <br>
 * a code of zero means success.
 */
FCode (p4_allocate)
{
    errno = 0;
    if ((*SP = (p4cell) p4_calloc (1, (size_t) * SP)) != 0)
        *--SP = 0;
    else *--SP = errno;
}

/** FREE ( ptr -- code )
 * free the memory from => ALLOCATE
 * a code of zero means success.
 */
FCode (p4_free)
{
    errno = 0;
    { p4_xfree (*(void **) SP); }
    *SP = errno;
}

/** RESIZE ( ptr newsize -- ptr' code )
 * resize the system memory chunk.
 * a code of zero means success.
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
    CO ("ALLOCATE",	p4_allocate),
    CO ("FREE",		p4_free),
    CO ("RESIZE",	p4_resize),
    P4_INTO ("ENVIRONMENT", 0 ),
    /* enviroment hints (testing for -EXT will mark this wordset as present) */
    P4_OCON ("MEMORY-ALLOC-EXT",	1994 ),
    
};
P4_COUNTWORDS (memory, "Memory-Alloc extension");

/*@}*/

