/** 
 *  Subroutines for file access
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 5.4 %
 *    (%date_modified: Mon Mar 12 10:32:19 2001 %)
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: file-sub.c,v 0.30.86.1 2001-03-12 09:32:19 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-xtra.h>

#include <stdio.h>
#include <stdlib.h>

#include <pfe/_nonansi.h>
#include <pfe/_missing.h>

#if !defined HAVE_ACCESS
int
access (const char *fn, int how)
{
# if defined HAVE_STAT
    struct stat st;		/* version using stat() */

    if (stat (fn, &st) != 0)
        return -1;
    switch (how)
    {
     default:
         return -1;
     case F_OK:
         return 0;
     case R_OK:
         return (st.st_mode & S_IREAD) ? 0 : -1;
     case W_OK:
         return (st.st_mode & S_IWRITE) ? 0 : -1;
     case X_OK:
         return (st.st_mode & S_IEXEC) ? 0 : -1;
    }
# else
    FILE *f;			/* limited version using ANSI-C fopen() */

    switch (how)
    {
     default:
         return -1;
     case F_OK:
     case R_OK:
         if ((f = fopen (fn, "r")) == NULL)
             return -1;
         fclose (f);
         return 0;
     case W_OK:
         if ((f = fopen (fn, "r+")) == NULL)
             return -1;
         fclose (f);
         return 0;
     case X_OK:
         return -1;
    }
# endif
}
#endif

_export long
fsize (FILE * f)		/* Result: file length, -1 on error */
{
# if defined HAVE_FSTAT && defined HAVE_FILENO

    struct stat st;		/* version using fstat() */
    int fh = fileno (f);
    
    if (fh < 0 || fstat (fh, &st) < 0)
        return -1;
    return st.st_size;

# else

    long pos, len;		/* ANSI-C version using fseek()/ftell() */
    
    clearerr (f);
    pos = ftell (f);
    if (pos == -1)
        return -1;
    if (fseek (f, 0, SEEK_END) != 0)
        return -1;
    len = ftell (f);
    if (fseek (f, pos, SEEK_SET) != 0)
        return -1;
    return len;
    
# endif
}

_export long
fn_size (const char *fn)		/* Result: file length, -1 on error */
{
# if defined HAVE_STAT
    struct stat st;

    if (stat (fn, &st) != 0)
        return -1;
    return st.st_size;
# else
    FILE *f;
    long len;

    f = fopen (fn, "r");
    if (f == NULL)
        return -1;
    len = fsize (f);
    fclose (f);
    return len;
# endif
}

_export long
fn_copy (const char *src, const char *dst, long limit)
/*
 * Copies file, but at most limit characters.
 * Returns destination file length if successful, -1 otherwise.
 */
{
    FILE *f, *g;
    char buf[BUFSIZ];
    size_t n;
    long m;

    if ((f = fopen (src, "rb")) == NULL)
        return -1;
    if ((g = fopen (dst, "wb")) == NULL)
    {
        fclose (f);
        return -1;
    }
    for (m = limit; m; m -= n)
    {
        n = (size_t) (BUFSIZ < m ? BUFSIZ : m);
        n = fread (buf, 1, n, f);
        if (n == 0 || n != fwrite (buf, 1, n, g))
            break;
    }
    n = ferror (f) || ferror (g);
    fclose (f);
    fclose (g);
    return n ? -1 : limit - m;
}

/*
 * Renames or moves file, returns 0 on success, -1 on error.
 */
_export int
fn_move (const char *src, const char *dst)
{
    if (rename (src, dst) == 0)
        return 0;
    if (fn_copy (src, dst, LONG_MAX) != -1)
    {
        return _pfe_remove (src);
    }else{
        _pfe_remove (dst);
        return -1;
    }
}

/** make file longer */
static int
fextend (FILE * f, long size)	
{
    long n;
    
    if (fseek (f, 0, SEEK_END) != 0)
        return -1;
    for (n = ftell (f); n < size; n++)
        if (putc (0, f) == EOF)
            return -1;
    return 0;
}

static int
fn_extend (const char *fn, long size)
{
    FILE *f;
    int result;
    
    f = fopen (fn, "ab");
    if (f == NULL)
        return -1;
    result = fextend (f, size);
    fclose (f);
    return result;
}

#ifndef HAVE_TRUNCATE
int
truncate (const char *path, long length)
{
    char tfn[L_tmpnam];
    long len;
    
    tmpnam (tfn);
    len = fn_copy (path, tfn, length);
    if (len == length && _pfe_remove (path) == 0)
    {
        return fn_move (tfn, path);
    }else{
        _pfe_remove (tfn);
        return 0;
    }
}
#endif

/*
 * Truncates or extends file.
 * Returns 0 if successful, -1 otherwise.
 */
_export int
fn_resize (const char *fn, long new_size)
{
    long old_size;
    
    old_size = fn_size (fn);
    if (old_size == -1)
        return -1;
    if (old_size <= new_size)
        return fn_extend (fn, new_size);
    else
        return truncate (fn, new_size);
}

/*@}*/

