#ifndef _PFE_MEMORY_SUB_H
#define _PFE_MEMORY_SUB_H 984413846
/* generated 2001-0312-1717 ../../pfe/../mk/Make-H.pl ../../pfe/memory-sub.c */

#include <pfe/incl-sub.h>

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

#ifdef __cplusplus
extern "C" {
#endif




_extern  void * p4_xcalloc (int n_elem, size_t size) /* allocate memory, die when failed */ ; /*{*/

_extern  void * p4_calloc (int n_elem, size_t size) /* allocate memory, with debug info */ ; /*{*/

_extern  void * p4_xalloc (size_t size) /* allocate memory, throw when failed */ ; /*{*/

_extern  void p4_xfree (void* p) ; /*{*/

_extern  int p4_mmap_creat(char* name, void* addr, long size) ; /*{*/

_extern  void p4_mmap_close(int fd, void* addr, long size) ; /*{*/

/**
 * given the archive name, it will try to load the image
 * to memory. This has may have side effect on PFE_MEM, PFE.mapfile_fd
 * and their like
 */
_extern  int p4_load_dict_image (char* ar_file) ; /*{*/

/**
 * create the file with this name and write the `ar` fileheader
 * which contains a magic that is checked in read. The file
 * descriptor is returned and should be written with
 * => p4_write_image
 */
_extern  int p4_creat_image (const char* name, mode_t mode) ; /*{*/

/**
 * write a memory chunk to that file under the specified name.
 * The name should only be a filebasename (no / and no .). The
 * nr-argument is appended as an extension. Hence using
   write_image(f, &PFE, sizeof(PFE), "thread", 4)
 * will create an `ar`-filentry named "thread.004" containing
 * the pfe-thread as following data. A (nr < 0) will result in
 * "name.0".
 */
_extern  long p4_write_image (int f, void* p, long l, const char* name, int nr) ; /*{*/

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
_extern  long p4_read_image (int f, void* p, long l, char* name, int nr) ; /*{*/

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
