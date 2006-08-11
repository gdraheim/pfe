/** 
 * -- Memory Allocation Words
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
 *     memory allocation interfaces to the surrounding OS.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: memory-sub.c,v 1.2 2006-08-11 22:56:05 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-limits.h>

#include <stdlib.h>
#include <stdio.h> /* FIXME: has to be in logging.h */
#include <pfe/os-string.h>
#include <errno.h>
#include <fcntl.h>

#ifdef PFE_HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#include <fcntl.h>

#ifdef PFE_HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef USE_MMAP 
#include <sys/mman.h>
#endif 

#include <pfe/logging.h>

_export void *
p4_xcalloc (int n_elem, size_t size) /* allocate memory, die when failed */
{
    void *p = calloc (n_elem, size);

    P4_debug3 (13, "xcalloc 0x%p[%i*%lu]", p, n_elem, (unsigned long) size);
    
    if (p == NULL)
    {
        P4_fatal ("out of memory");
	PFE.exitcode = 6;
	p4_longjmp_exit ();
    }
  
    return p;
}

_export void *
p4_calloc (int n_elem, size_t size) /* allocate memory, with debug info */
{
    void *p = calloc (n_elem, size);

    if (p)
    {
	P4_debug3 (13, "calloc 0x%p[%i*%lu]", p, n_elem, (unsigned long)size);
    }else{
        P4_warn2 ("calloc is null[%i*%lu]", n_elem, (unsigned long)size);
    }

    return p;
}

_export void *
p4_xalloc (size_t size)	/* allocate memory, throw when failed */
{
    void *p = calloc (1, size);

    P4_debug2 (13, "xalloc 0x%p[%lu]", p, (unsigned long)size);

    if (p == NULL)
        p4_throw (P4_ON_OUT_OF_MEMORY);

    return p;
}

_export void
p4_xfree (void* p)
{
    P4_debug1 (13, "xfree 0x%p", p);
    free (p);
}

/**
 * helper routine to allocate a portion of the dictionary
 * especially for some stack-areas of the forth system
 * ... just decreases PFE.dictlimit, returns 0 if impossible.
 */
_export void*
p4_dict_allocate (int items, int size, int align, 
		  void** lower, void** upper)
{
    register p4char* memtop = PFE.dictlimit;
    if (! align) align = sizeof(p4cell);
    memtop =(p4char*)( ((p4cell)memtop) &~ ((p4cell)(align) -1) );
    if (upper) *upper = memtop;
    memtop -= items * size;
    if (lower) *lower = memtop;
    if (upper) PFE.dictlimit = memtop; /* always save if upper-ref given */
    if (memtop < PFE.dp + 256) return 0; /* error condition */
    return (PFE.dictlimit = memtop);
}

/* ------------------------------------------------------------------ *
 *   virtual alloc
 */

#ifdef USE_MMAP

#ifndef MAP_FAILED
#define MAP_FAILED ((void*) -1)
#endif

_export int 
p4_mmap_creat(char* name, void* addr, long size)
{
    int fd;
    if (! addr || size < 16)
    {
	P4_warn1 ("[%p], use the function only with args != 0 !!!", p4TH);
	return 0;
    }
    fd = open (PFE_set.mapfile, O_RDWR|O_CREAT|O_TRUNC, 0660);
    if (fd == -1)
    {
	P4_info3 ("[%p] %s: could not open: %s",
		  p4TH, PFE_set.mapfile, strerror(errno));
	return 0;
    }else{
	register void* done;

	/* sparse write first to ensure all mmap-handling 
	   is done now. Some systems delay the actual mm-inits
	   which we don't like to see here.
	*/
	if (lseek (fd, size-3, SEEK_SET) != size-3) { close(fd); return 0; }
	write (fd, "END", 3);

	done = MAP_FAILED;
	if (addr)
	{
	    done = mmap (addr, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	    if (done == MAP_FAILED)
	    {
		P4_fail4 ("[%p] %s: mmap failed for addr %8p : %s",
			  p4TH, name, addr, strerror(errno));
	    }
	}
	if (done == MAP_FAILED)
	{
	    done = mmap (0, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	    if (done == MAP_FAILED)
	    {
		P4_fail3 ("[%p] %s: mmap failed anyway : %s",
			  p4TH, name, strerror(errno));
	    }
	}
	if (done == MAP_FAILED)
	{
	    close (fd);
	    return 0;
	}else{
	    P4_info3 ("[%p] mapped at %8p len %ld",
		      p4TH, PFE_MEM, size);
	    return fd;
	}
    }
}

_export void
p4_mmap_close(int fd, void* addr, long size)
{
    munmap (addr, size);
    close (fd);
    P4_info1 ("[%p] unmapped", p4TH);
}

#endif

