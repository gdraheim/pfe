/**
 * --  Block-oriented Subroutines
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author$)
 *  @version $Revision$
 *     (modified $Date$)
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) =
"@(#) $Id$";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-limits.h>

#include <errno.h>
#include <pfe/os-string.h>
#include <stdio.h>

#include <pfe/file-sub.h>
#include <pfe/logging.h>
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
            p4_memset (f, 0, sizeof *f);
            return f;
        }
    P4_warn ("not enough file slots in pfe io subsystem");
    return NULL;
}

/**
 * Return best possible access method,
 * 0 if no access but file exists, -1 if file doesn't exist.
 */
int
p4_file_access (const p4_char_t *fn, int len)
{
    char* buf = p4_pocket_filename (fn, len);
    if (_P4_access (buf, F_OK) != 0)
        return -1;
    if (_P4_access (buf, R_OK | W_OK) == 0)
        return FMODE_RW;
    if (_P4_access (buf, R_OK) == 0)
        return FMODE_RO;
    if (_P4_access (buf, W_OK) == 0)
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
p4_File *
p4_open_file (const p4_char_t *name, int len, int mode)
{
    p4_File *fid;
    mode &= 7;

    fid = p4_free_file_slot ();
    if (fid == NULL)
        return NULL;
    p4_store_filename (name, len, fid->name, sizeof fid->name);
    fid->mode = mode;
    fid->last_op = 0;
    p4_strcpy (fid->mdstr, open_mode[mode - FMODE_RO]);
    if ((fid->f = fopen (fid->name, fid->mdstr)) == NULL)
        return NULL;
    fid->size = (p4ucell) (p4_file_size (fid->f) / BPBUF);
    fid->n = (p4ucelll)(p4celll) (-1); /* before first line */
    return fid;
}

/**
 * create file
 */
p4_File *
p4_create_file (const p4_char_t *name, int len, int mode)
{
    char* fn = p4_pocket_filename (name, len);
    /* create an empty file via open("w") */
    FILE* f = fopen (fn, "wb");
    if (! f) {
    	if (mode > 256) /* updec! */
    	{   P4_fail2 ("%s : %s", fn, strerror(PFE_io_errno));   }
    	return NULL;
	}
    fclose (f);
    /* then create a forth-file with normal open */
    p4_File *fid = p4_open_file (name, len, mode);
    if (fid) {
        return fid;
    }else{
        _pfe_remove (fn);
        return NULL;
    }
}

/**
 * close file
 */
int
p4_close_file (p4_File *fid)
{
    int res = 0;

    if (fid->f)
    {
        res = fclose (fid->f);
        p4_memset (fid, 0, sizeof *fid);
    }
    return res;
}

/**
 * seek file
 */
int
p4_reposition_file (p4_File *fid, _p4_off_t pos)
{
    fid->last_op = 0;
    return _p4_fseeko (fid->f, pos, SEEK_SET) ? PFE_io_errno : 0;
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
        _p4_fseeko (fid->f, 0, SEEK_CUR); /* then seek to this position */
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
        _p4_fseeko (fid->f, 0, SEEK_CUR); /* then seek to this position */
    fid->last_op = -1;
    return 1;
}

/**
 * read file
 */
int
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
        return PFE_io_errno;
    }
    else
        return 0;
}

/**
 * write file
 */
int
p4_write_file (void *p, p4ucell n, p4_File *fid)
{
    if (!p4_can_write (fid))
        return EPERM;
    errno = 0;
    return (p4ucell) fwrite (p, 1, n, fid->f) != n ? PFE_io_errno : 0;
}

/**
 * resize file
 */
int
p4_resize_file (p4_File *fid, _p4_off_t size)
{
    _p4_off_t pos;
    int r;

    if (fid == NULL || fid->f == NULL)
        p4_throw (P4_ON_FILE_NEX);

    pos = _p4_ftello (fid->f);
    if (pos == -1)
        return -1;

    fclose (fid->f);
    r = p4_file_resize (fid->name, size);
    fid->f = fopen (fid->name, fid->mdstr);

    if (pos < size)
        _p4_fseeko (fid->f, pos, SEEK_SET);
    else
        _p4_fseeko (fid->f, 0, SEEK_END);
    return r;
}

/**
 * read line
 */
int
p4_read_line (void* buf, p4ucell *u, p4_File *fid, p4cell *ior)
{
    int c, n; char* p = buf;

    if (!p4_can_read (fid))
        return EPERM;
    fid->line.pos = _p4_ftello (fid->f); /* fixme: the only reference to it!*/
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
                 *ior = PFE_io_errno;
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
p4_File *
p4_open_blockfile (const p4_char_t *name, int len)
{
    p4_char_t* fn = (p4_char_t*) p4_pocket_expanded_filename (
        name, len, *P4_opt.blk_paths, *P4_opt.blk_ext);
    int fn_len = p4_strlen ((char*) fn);
    int mode = p4_file_access (fn, fn_len);
    if (mode <= 0) return NULL;
    return p4_open_file (fn, fn_len, mode + FMODE_BIN);
}

/**
 * set fid as current block-file, possibly close the old one.
 * (does nothing if argument is null, returns the argument)
 */
p4_File*
p4_set_blockfile (p4_File* fid)
{
    if (! fid) return fid;
    if (BLOCK_FILE)
    {
        FX (p4_save_buffers);
        p4_close_file (BLOCK_FILE);
    }
    return ((BLOCK_FILE = fid));
}

/**
 * very traditional block read/write primitive
 */
void
p4_blockfile_read_write (p4_File *fid, void *p, p4ucell n, int readflag)
{
    size_t len;

    p4_Q_file_open (fid);
    clearerr (fid->f);
    if (n > fid->size)
        p4_throw (P4_ON_INVALID_BLOCK);
    if (readflag && n == fid->size)
    {
        p4_memset (p, ' ', BPBUF);
        return;
    }
    if (_p4_fseeko (fid->f, (_p4_off_t)n * BPBUF, SEEK_SET) != 0)
        p4_throwstr (FX_IOR, fid->name);
    if (readflag)
    {
        if (!p4_can_read (fid))
        p4_throw (P4_ON_BLOCK_READ);
        len = fread (p, 1, BPBUF, fid->f);
        if (ferror (fid->f))
            p4_throwstr (FX_IOR, fid->name);
        p4_memset ((char*) p + len, ' ', BPBUF - len);
    }else{
        if (!p4_can_write (fid))
        p4_throw (P4_ON_BLOCK_WRITE);
        len = fwrite (p, 1, BPBUF, fid->f);
        if (len < BPBUF || ferror (fid->f))
            p4_throwstr (FX_IOR, fid->name);
        if (n == fid->size)
            fid->size++;
    }
    return;
}

/**
 * traditional BUFFER impl
 */
void*
p4_blockfile_buffer (p4_File *fid, p4_blk_t blk, int *reload)
{
    p4_Q_file_open (fid);
    if (fid->n != blk)
    {
        if (fid->updated)
            p4_blockfile_read_write (fid, fid->buffer, fid->n, P4_FALSE);
        fid->n = blk;
        *reload = 1;
    }else{
        *reload = 0;
    }
    return fid->buffer;
}

/**
 * traditional BLOCK impl
 */
void*
p4_blockfile_block (p4_File *fid, p4_blk_t blk)
{
    int reload;
    void * buf = p4_blockfile_buffer (fid, blk, &reload);
    if (reload)
        p4_blockfile_read_write (fid, buf, blk, P4_TRUE);
    return buf;
}

/**
 * EMPTY-BUFFERS
 */
void
p4_blockfile_empty_buffers (p4_File *fid)
{
    p4_Q_file_open (fid);
    _p4_buf_zero (fid->buffer);
    fid->n = UINT_MAX;
    fid->updated = 0;
}

/**
 * SAVE-BUFFERS
 */
void
p4_blockfile_save_buffers (p4_File *fid)
{
    if (fid && fid->updated)
    {
        p4_blockfile_read_write (fid, fid->buffer, fid->n, P4_FALSE);
        fflush (fid->f);
        fid->updated = 0;
    }
}

/**
 * UPDATE
 */
void
p4_blockfile_update (p4_File *fid)
{
    p4_Q_file_open (fid);
    if ((int) fid->n < 0)
        p4_throw (P4_ON_INVALID_BLOCK);
    fid->updated = 1;
}

/**
 * LIST
 */
void
p4_blockfile_list (p4_File *fid, int n)
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
void
p4_blockfile_load (p4_File *fid, p4_blk_t blk)
{
    if (blk == 0)
        p4_throw (P4_ON_INVALID_BLOCK);
# ifdef P4_RP_IN_VM
    RP = (p4xcode **) p4_save_input (RP);
# else
    /* RP = (p4xcode **) p4_save_input (RP); ** FIXME: !! */
# endif
    BLOCK_FILE = fid;
    SOURCE_ID = 0;
    BLK = blk;
    TO_IN = 0;
    FX (p4_interpret);
# ifdef P4_RP_IN_VM
    RP = (p4xcode **) p4_restore_input (RP);
# else
    /*  RP = (p4xcode **) p4_restore_input (RP); ** FIXME: !! */
# endif
}

/**
 * => THRU
 */
void
p4_blockfile_thru (p4_File *fid, p4_blk_t lo, p4_blk_t hi)
{
    int i;

    for (i = lo; i <= hi; i++)
        p4_blockfile_load (fid, i);
}

/*@}*/
