/** 
 * -- Memory Allocation Words
 * 
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 1.7 %
 *    (%date_modified: Mon Mar 12 10:32:32 2001 %)
 *
 *  @description
 *     memory allocation interfaces to the surrounding OS.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: memory-sub.c,v 0.31 2001-03-19 21:50:41 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-xtra.h>

#include <stdlib.h>
#include <stdio.h> /* FIXME: has to be in logging.h */
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#include <fcntl.h>

#ifdef HAVE_UNISTD_H
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

/**
 * given the archive name, it will try to load the image
 * to memory. This has may have side effect on PFE_MEM, PFE.mapfile_fd
 * and their like
 */
_export int
p4_load_dict_image (char const * ar_file)
{
    auto p4_Thread th;
    register int fd;
    register void* addr;

    fd = open (ar_file, O_RDONLY, 0);
    if (fd == -1)
    {
	return 0;
    }

    if (p4_read_image (fd, &th, sizeof(th), "thread", 0) < sizeof(th))
    {
	close(fd);
	return 0;
    }
    
    addr = th.p[P4_MEM_SLOT];

    if (! addr) 
    {
	P4_fail1 ("[%p], image file has no base, probably corrupted file",
		  p4TH );
	return 0; 
    }

    if (PFE_MEM != addr)
    {
	P4_fail1 ("[%p], re-mapping not implemented !!", p4TH);
	close (fd);
	return 0;
    }

    /* great, so go ahead and load the binary */
    if (!p4_read_image (fd, addr, th.dp - th.fence, "dictfence", th.nr))
    {
	P4_fail ("read_image dictfence failed");
	close (fd);
	return 0;
    }

    /* set some system variables according to the image */
    PFE.dp = th.dp;
    PFE.fence = th.fence;
    /* any other idea? add here... */

    P4_info ("dict_image loaded");
    close (fd); /* don't need it anymore */
    return 1;
}

/* ------------------------------------------------------------------ *
 * pfe binaries are `ar` compatible.
 */

#ifndef ARMAG 
#define ARMAG   "!<arch>\n"     /* String that begins an archive file.  */
#define SARMAG  8               /* Size of that string.  */

#define ARFMAG  "`\n"           /* String in ar_fmag at end of each header.  */

struct ar_hdr
{
    char ar_name[16];           /* Member file name, sometimes / terminated. */
    char ar_date[12];           /* File date, decimal seconds since Epoch.  */
    char ar_uid[6], ar_gid[6];  /* User and group IDs, in ASCII decimal.  */
    char ar_mode[8];            /* File mode, in ASCII octal.  */
    char ar_size[10];           /* File size, in ASCII decimal.  */
    char ar_fmag[2];            /* Always contains ARFMAG.  */
};
/* gotten from glibc2 ar.h */
#endif

#ifdef HAVE_GID
#define GET_GID (getgid ())
#else
#define GET_GID 0
#endif

#ifdef HAVE_UID
#define GET_UID (getuid ())
#else
#define GET_UID 0
#endif

#ifndef O_SYNC
#define O_SYNC 0
#endif

/**
 * create the file with this name and write the `ar` fileheader
 * which contains a magic that is checked in read. The file
 * descriptor is returned and should be written with
 * => p4_write_image
 */
_export int
p4_creat_image (const char* name, mode_t mode)
{
    register int f;
    auto struct ar_hdr ar;
    memset(&ar, 0, sizeof(ar));

    if (!mode) mode = 0664;
    f = open (name, O_CREAT|O_TRUNC|O_SYNC|O_WRONLY, mode);
    
    if (f != -1)
    {
	write (f, ARMAG, SARMAG);
        memcpy  (ar.ar_fmag, ARFMAG, sizeof(ar.ar_fmag));
        sprintf (ar.ar_date, "%ld", (long) time(0));
        sprintf (ar.ar_mode, GET_UID ? "664" : "666");
        sprintf (ar.ar_uid, "%d", GET_UID);
        sprintf (ar.ar_gid, "%d", GET_GID);
 	
        sprintf (ar.ar_name, "%s-%s", PFE_PACKAGE, P4_VERSION);
        sprintf (ar.ar_size, "0");
	
        write (f, (void*) &ar, sizeof(ar));
    }else{
        P4_warn1 ("could not open file %s", name);
    }
    
    return f;
}
    
/**
 * write a memory chunk to that file under the specified name.
 * The name should only be a filebasename (no / and no .). The
 * nr-argument is appended as an extension. Hence using
   write_image(f, &PFE, sizeof(PFE), "thread", 4)
 * will create an `ar`-filentry named "thread.004" containing
 * the pfe-thread as following data. A (nr < 0) will result in
 * "name.0".
 */
_export long
p4_write_image (int f, void* p, long l, const char* name, int nr)
{
    auto struct ar_hdr ar;
    memset(&ar, 0, sizeof(ar));

    if (f != -1)
    {
        memcpy  (ar.ar_fmag, ARFMAG, sizeof(ar.ar_fmag));
        sprintf (ar.ar_date, "%ld", (long) time(0));
        sprintf (ar.ar_mode, GET_UID ? "664" : "666");
        sprintf (ar.ar_uid, "%d", GET_UID);
        sprintf (ar.ar_gid, "%d", GET_GID);
    
        if (nr >= 0)
            sprintf (ar.ar_name, "%s.%03i", name, nr);
        else
            sprintf (ar.ar_name, "%s.0", name);
        sprintf (ar.ar_size, "%ld", l);
    
        if (write (f, (void*) &ar, sizeof(ar)) != sizeof(ar)) return 0;
        return write (f, p, l);
    }
    
    return 0;
}
    
/**
 * read a memory chunk from file known under the specified name.
 *
 * The name should only be a filebasename (no / and no .). The
 * nr-argument is appended as an extension, but if nr <= 0, it
 * will look with filepatter "name.0", so it will find the first
 * instance where the => p4_write_image was used with nr < 100.
 *
 * the return value is the number of => read(2) bytes. The
 * file descriptor can be any seekable fd - the function will
 * first seek(0) and check the `ar`-filemagic, and it will then
 * go through the chained fileheaders looking for the name.
 * no extra information is stored, so the file descriptor can
 * simply be =>'close(2)'d  later.
 */
_export long
p4_read_image (int f, void* p, long l, char const * name, int nr)
{
    auto struct ar_hdr ar;
    auto char nm[sizeof(ar.ar_name)];
    register int s; 

    /* startover at header */
    if (lseek (f, 0, SEEK_SET)) return 0;
    if (read (f, nm, sizeof(SARMAG)) != SARMAG) return 0;
    if (memcmp (nm, ARMAG, SARMAG)) return 0;
    
    /* make the name to search for */
    if (nr >= 0)
	sprintf (nm, "%s.%03i", name, nr);
    else
	sprintf (nm, "%s.0", name);

    while(1) 
    {
	if (read (f, (void*) &ar, sizeof(ar)) != sizeof(ar)) return 0;
	s = atoi (ar.ar_size);
	if (! memcmp (ar.ar_name, nm, strlen(nm)))
	{
	    if (s > l) s = l;
	    return read (f, p, s);
	}else{
	    if (lseek (f, s, SEEK_CUR) == ((off_t) -1)) return 0;
	}
    }
}

