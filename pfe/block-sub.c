/** 
 * --  Block-oriented Subroutines
 * 
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE            @(#) %derived_by: guidod %
 *  @version %version: 1.6 %
 *    (%date_modified: Mon Mar 12 10:32:00 2001 %)
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: block-sub.c,v 0.30 2001-03-13 09:36:14 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-xtra.h>

#include <errno.h>
#include <string.h>

#include <pfe/file-sub.h>
#include <pfe/_nonansi.h>
#include <pfe/_missing.h>

/* ********************************************************************** 
 * file interface
 */

/**
 */
static p4_File *
p4_free_file_slot (void)
{
    p4_File *f;

    for (f = PFE.files; f < PFE.files_top; f++)
        if (f->f == NULL)
        {
            memset (f, 0, sizeof *f);
            return f;
        }
    return NULL;
}

/**
 * Return best possible access method,
 * 0 if no access but file exists, -1 if file doesn't exist.
 */
_export int
p4_file_access (const char *fn, int len)
{
    char* buf;

    buf = p4_pocket_filename (fn, len);
    if (access (buf, F_OK) != 0)
        return -1;
    if (access (buf, R_OK | W_OK) == 0)
        return FMODE_RW;
    if (access (buf, R_OK) == 0)
        return FMODE_RO;
    if (access (buf, W_OK) == 0)
        return FMODE_WO;
    return 0;
}

static char open_mode[][4] =	/* mode strings for fopen() */
{
    "r", "r+", "r+",		/* R/O W/O R/W */
    "rb", "r+b", "r+b",		/* after application of BIN */
};

/**
 * open file
 */
_export p4_File *
p4_open_file (const char *name, int len, int mode)
{
    p4_File *fid;

    fid = p4_free_file_slot ();
    if (fid == NULL)
        return NULL;
    p4_store_filename (name, len, fid->name, sizeof fid->name);
    fid->mode = mode;
    fid->last_op = 0;
    strcpy (fid->mdstr, open_mode[mode - FMODE_RO]);
    if ((fid->f = fopen (fid->name, fid->mdstr)) == NULL)
        return NULL;
    fid->size = (p4ucell) (fsize (fid->f) / BPBUF);
    fid->n = (unsigned) -1;
    return fid;
}

/**
 * create file 
 */
_export p4_File *
p4_create_file (const char *name, int len, int mode)
{
    char* fn;
    p4_File *fid;
    
    fn = p4_pocket_filename (name, len);
    fclose (fopen (fn, "wb"));
    fid = p4_open_file (name, len, mode);
    if (fid)
    {
        return fid;
    }else{
        _pfe_remove (fn);
        return NULL;
    }
}

/**
 * close file
 */
_export int
p4_close_file (p4_File *fid)
{
    int res = 0;
    
    if (fid->f)
    {
        res = fclose (fid->f);
        memset (fid, 0, sizeof *fid);
    }
    return res;
}

/**
 * seek file
 */
_export int
p4_reposition_file (p4_File *fid, long pos)
{
    fid->last_op = 0;
    return fseek (fid->f, pos, SEEK_SET) ? errno : 0;
}

/*
 * Called before trying to read from a file.
 * Checks if you may, maybe fseeks() so you can.
 */
static int
p4_can_read (p4_File *fid)
{
    switch (fid->mode)		/* check permission */
    {
     case FMODE_WO:
     case FMODE_WOB:
         return 0;
    }
    if (fid->last_op < 0)		/* last operation was write? */
        fseek (fid->f, 0, SEEK_CUR); /* then seek to this position */
    fid->last_op = 1;
    return 1;
}

/*
 * Called before trying to write to a file.
 * Checks if you may, maybe fseeks() so you can.
 */
static int
p4_can_write (p4_File *fid)
{
    switch (fid->mode)		/* check permission */
    {
     case FMODE_RO:
     case FMODE_ROB:
         return 0;
    }
    if (fid->last_op > 0)		/* last operation was read? */
        fseek (fid->f, 0, SEEK_CUR); /* then seek to this position */
    fid->last_op = -1;
    return 1;
}

/**
 * read file
 */
_export int
p4_read_file (void *p, p4ucell *n, p4_File *fid)
{
    int m;

    if (!p4_can_read (fid))
        return EPERM;
    errno = 0;
    m = fread (p, 1, *n, fid->f);
    if (m != (int) *n)
    {
        *n = m;
        return errno;
    }
    else
        return 0;
}

/**
 * write file
 */
_export int
p4_write_file (void *p, p4ucell n, p4_File *fid)
{
    if (!p4_can_write (fid))
        return EPERM;
    errno = 0;
    return (p4ucell) fwrite (p, 1, n, fid->f) != n ? errno : 0;
}

/**
 * resize file
 */
_export int
p4_resize_file (p4_File *fid, long size)
{
    long pos;
    int r;

    if (fid == NULL || fid->f == NULL)
        p4_throw (P4_ON_FILE_NEX);

    pos = ftell (fid->f);
    if (pos == -1)
        return -1;
    
    fclose (fid->f);
    r = fn_resize (fid->name, size);
    fid->f = fopen (fid->name, fid->mdstr);
    
    if (pos < size)
        fseek (fid->f, pos, SEEK_SET);
    else
        fseek (fid->f, 0, SEEK_END);
    return r;
}

/**
 * read line
 */
_export int
p4_read_line (char *p, p4ucell *u, p4_File *fid, p4cell *ior)
{
    int c, n;
    
    if (!p4_can_read (fid))
        return EPERM;
    fid->pos = ftell (fid->f);
    for (n = 0; (p4ucell) n < *u; n++)
    {
        switch (c = getc (fid->f))
        {
         default:
             *p++ = c;
             continue;
         case EOF:
             *u = n;
             if (ferror (fid->f))
                 *ior = errno;
             else
                 *ior = 0;
             return P4_FLAG (n > 0);
         case '\r':
             c = getc (fid->f);
             if (c != '\n')
                 ungetc (c, fid->f);
         case '\n':
             goto happy;
        }
    }
 happy:
    *u = n;
    *ior = 0;
    fid->n++;
    return P4_TRUE;
}


/* -------------------------------------------------------------------- *
 * block-files
 */

/**
 * source input: read from block-file 
 */
_export p4_File *
p4_open_block_file (const char *name, int len)
{
    char* fn;
    int mode;

    fn = p4_pocket_expanded_filename (name, len, 
				      P4_opt.blkpaths, P4_opt.blkext);
    mode = p4_file_access (fn, strlen (fn));
    if (mode <= 0)
        return NULL;
    return p4_open_file (fn, strlen (fn), mode + FMODE_BIN);
}

/**
 * source input: use block file 
 */
_export int
p4_use_block_file (const char *name, int len)
{
    File *fid;
    
    fid = p4_open_block_file (name, len);
    if (fid == NULL)
        return P4_FALSE;
    if (BLOCK_FILE)
    {
        FX (p4_save_buffers);
        p4_close_file (BLOCK_FILE);
    }
    BLOCK_FILE = fid;
    return P4_TRUE;
}

/**
 * very traditional block read/write primitive 
 */
_export void
p4_read_write (p4_File *fid, char *p, p4ucell n, int readflag)
{
    size_t len;
    
    p4_Q_file_open (fid);
    clearerr (fid->f);
    if (n > fid->size)
        p4_throw (P4_ON_INVALID_BLOCK);
    if (readflag && n == fid->size)
    {
        memset (p, ' ', BPBUF);
        return;
    }
    if (fseek (fid->f, n * BPBUF, SEEK_SET) != 0)
        p4_throws (FX_IOR, fid->name, 0);
    if (readflag)
    {
        if (!p4_can_read (fid))
	p4_throw (P4_ON_BLOCK_READ);
        len = fread (p, 1, BPBUF, fid->f);
        if (ferror (fid->f))
            p4_throws (FX_IOR, fid->name, 0);
        memset (p + len, ' ', BPBUF - len);
    }else{
        if (!p4_can_write (fid))
	p4_throw (P4_ON_BLOCK_WRITE);
        len = fwrite (p, 1, BPBUF, fid->f);
        if (len < BPBUF || ferror (fid->f))
            p4_throws (FX_IOR, fid->name, 0);
        if (n == fid->size)
            fid->size++;
    }
    return;
}

/**
 * traditional BUFFER impl
 */
_export char *
p4_buffer (p4_File *fid, p4ucell n, int *reload)
{
    p4_Q_file_open (fid);
    if (fid->n != n)
    {
        if (fid->updated)
            p4_read_write (fid, fid->buffer, fid->n, P4_FALSE);
        fid->n = n;
        *reload = 1;
    }else{
        *reload = 0;
    }
    return fid->buffer;
}

/**
 * traditional BLOCK impl
 */
_export char *
p4_block (p4_File *fid, p4ucell n)
{
    char *p;
    int reload;
    
    p = p4_buffer (fid, n, &reload);
    if (reload)
        p4_read_write (fid, p, n, P4_TRUE);
    return p;
}

/**
 * EMPTY-BUFFERS
 */
_export void
p4_empty_buffers (p4_File *fid)
{
    p4_Q_file_open (fid);
    ZERO (fid->buffer);
    fid->n = UINT_MAX;
    fid->updated = 0;
}

/**
 * SAVE-BUFFERS
 */
_export void
p4_save_buffers (p4_File *fid)
{
    if (fid && fid->updated)
    {
        p4_read_write (fid, fid->buffer, fid->n, P4_FALSE);
        fflush (fid->f);
        fid->updated = 0;
    }
}

/**
 * UPDATE
 */
_export void
p4_update (p4_File *fid)
{
    p4_Q_file_open (fid);
    if ((int) fid->n < 0)
        p4_throw (P4_ON_INVALID_BLOCK);
    fid->updated = 1;
}

/**
 * LIST
 */
_export void
p4_list (p4_File *fid, int n)
{
    int i;

    for (i = 0; i < 16; i++)
    {
        FX (p4_cr);
        p4_outf ("%2d: ", i);
        p4_dot_line (fid, n, i);
    }
    FX (p4_space);
    SCR = n;
}



/**
 * => INTERPET file
 */
_export void
p4_load (p4_File *fid, p4ucell blk)
{
    if (blk == 0)
        p4_throw (P4_ON_INVALID_BLOCK);
    RP = (p4xt **) p4_save_input (RP);
    BLOCK_FILE = fid;
    SOURCE_ID = 0;
    BLK = blk;
    TO_IN = 0;
    FX (p4_interpret);
    RP = (p4xt **) p4_restore_input (RP);
}

/**
 * open and => LOAD
 */
_export void
p4_load_file (char *fn, int cnt, int blk)
{
    File *fid = p4_open_block_file (fn, cnt);
    
    if (fid == NULL)
        p4_throws (FX_IOR, fn, cnt);
    p4_load (fid, blk);
}

/**
 * => THRU
 */
_export void
p4_thru (p4_File *fid, int lo, int hi)
{
    int i;

    for (i = lo; i <= hi; i++)
        p4_load (fid, i);
}

/*@}*/

