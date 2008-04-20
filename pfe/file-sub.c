/** 
 *  Subroutines for file access
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2008-04-20 04:46:30 $)
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: file-sub.c,v 1.3 2008-04-20 04:46:30 guidod Exp $";
#endif

#define _P4_SOURCE
#define _P4_NO_REGS_SOURCE 1

#include <pfe/def-cell.h>
#include <pfe/def-limits.h>

#include <stdio.h>
#include <stdlib.h>

#include <pfe/logging.h>
#include <pfe/_nonansi.h>
#include <pfe/_missing.h>

#ifdef PFE_HAVE_TRUNCATE
#ifdef PFE_HAVE_UNISTD_H
#include <unistd.h>
#endif
#endif

#ifdef VXWORKS
/* fixme: vxworks headers should be fixed!! */
#define stat(_X_,_Y_) (stat((char*)(_X_),(_Y_)))
#endif

_export _p4_off_t
p4_file_size (FILE * f)		/* Result: file length, -1 on error */
{
# if defined PFE_HAVE_FSTAT && defined PFE_HAVE_FILENO

    struct stat st;		/* version using fstat() */
    int fh = fileno (f);
    
    if (fh < 0 || fstat (fh, &st) < 0)
        return -1;
    return st.st_size;

# else

    _p4_off_t pos, len;		/* ANSI-C version using fseek()/ftell() */
    
    clearerr (f);
    pos = _p4_ftello (f);
    if (pos == -1)
        return -1;
    if (_p4_fseeko (f, 0, SEEK_END) != 0)
        return -1;
    len = _p4_ftello (f);
    if (_p4_fseeko (f, pos, SEEK_SET) != 0)
    { P4_fatal1 ("could not reset to start position %li", pos); }
    else
    { P4_leave1 ("success after call at file offset %li", pos); }
    return len;
    
# endif
}

static _p4_off_t
file_size (const char *fn)		/* Result: file length, -1 on error */
{
# if defined PFE_HAVE_STAT
    struct stat st;

    if (stat (fn, &st) != 0)
        return -1;
    return st.st_size;
# else
    FILE *f;
    _p4_off_t len;
    
    f = fopen (fn, "r");
    if (f == NULL)
        return -1;
    len = p4_file_size (f);
    fclose (f);
    return len;
# endif
}

_export _p4_off_t
p4_file_copy (const char *src, const char *dst, _p4_off_t limit)
/*
 * Copies file, but at most limit characters.
 * Returns destination file length if successful, -1 otherwise.
 */
{
    FILE *f, *g;
    char buf[BUFSIZ];
    size_t n;
    _p4_off_t m;

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
p4_file_move (const char *src, const char *dst)
{
    if (_P4_rename (src, dst) == 0)
        return 0;
    if (p4_file_copy (src, dst, LONG_MAX) != -1)
    {
        return _pfe_remove (src);
    }else{
        _pfe_remove (dst);
        return -1;
    }
}

/** make file longer */
static int
_fextend (FILE * f, _p4_off_t size)	
{
    _p4_off_t n;
    
    if (_p4_fseeko (f, 0, SEEK_END) != 0)
        return -1;
    for (n = _p4_ftello (f); n < size; n++)
        if (putc (0, f) == EOF)
            return -1;
    return 0;
}

static int
file_extend (const char *fn, _p4_off_t size)
{
    FILE *f;
    int result;
    
    f = fopen (fn, "ab");
    if (f == NULL)
        return -1;
    result = _fextend (f, size);
    fclose (f);
    return result;
}

#ifndef PFE_HAVE_TRUNCATE
static int
_p4_truncate (const char *path, _p4_off_t length)
{
    char tfn[L_tmpnam];
    _p4_off_t len;
    
    tmpnam (tfn);
    len = p4_file_copy (path, tfn, length);
    if (len == length && _pfe_remove (path) == 0)
    {
        return p4_file_move (tfn, path);
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
p4_file_resize (const char *fn, _p4_off_t new_size)
{
    _p4_off_t old_size;
    
    old_size = file_size (fn);
    if (old_size == -1)
        return -1;
    if (old_size <= new_size)
        return file_extend (fn, new_size);
    else
        return _P4_truncate (fn, new_size);
}

/*@}*/

