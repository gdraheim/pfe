/**
 * FILE ---  Optional File-Access Word Set
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author$)
 *  @version $Revision$
 *     (modified $Date$)
 *
 *  @description
 *       The Optional File-Access Word Set and
 *       File-Access Extension Words.
 *       These words imply some kind of file-system unlike
 *       the BLOCK wordset.
 *
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) =
"@(#) $Id$";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-limits.h>

#include <stdio.h>
#include <errno.h>

#include <pfe/file-sub.h>
#include <pfe/logging.h>
#include <pfe/_missing.h>

#ifndef _NULLFILE_ROBUST   /* USER-CONFIG */
#ifdef  PFE_HAVE_WINBASE_H /* read/write check for NULL-file w/in pfe itself */
#define _NULLFILE_ROBUST 1 /* win32 API will SIGBUS on NULL-file read/write */
#else
#define _NULLFILE_ROBUST 0
#endif
#endif

/* when no _NULLFILE_ROBUST is set, then let the compiler do some
 * code removal of unreachable code - because of that if(0) part.
 */
#if _NULLFILE_ROBUST+0
#define _is_nullfile(X) !(X)
#else
#define _is_nullfile(X) 0
#endif

#ifdef PFE_WITH_FIG
#define CHECKFILE " - did an earlier FILE-OPEN fail?" \
                  " - often it is file permissions" \
                  " - file or directory read-only?"
#else
#define CHECKFILE " (did some FILE-OPEN fail?)"
#endif

/* ================================================================= */

/** BIN ( access-mode# -- access-mode#' ) [ANS]
 * modify the give file access-mode to be a binary-mode
 */
void FXCode (p4_bin)
{
    *SP += FMODE_BIN;
}

/** CLOSE-FILE ( some-file* -- some-errno# ) [ANS]
 * close the file and return the status-code
 */
void FXCode (p4_close_file)
{
    register File *fid = (File *) SP[0];

    if (_is_nullfile(fid)) goto nullfile;
    SP[0] = p4_close_file (fid) ? PFE_io_errno : 0;
    return;
 nullfile:
    SP[0] = EINVAL;
    P4_warn ("close on NULL file");
}

/** CREATE-FILE ( name-ptr name-len open-mode# -- name-file* name-errno# ) [ANS]
 * create the file with the given name and open
 * it - returns the file id and a status code.
 * A code of zero means success. An existing file
 * of the same name is truncated upon open.
 */
void FXCode (p4_create_file)
{
    register p4_char_t *fn = (p4_char_t *) SP[2]; /* c-addr, name */
    register p4ucell u = SP[1];	                  /* length of name */
    register p4cell fam = SP[0];                  /* file access mode */
    File *fid = p4_create_file (fn, u, fam);

    SP += 1;
    SP[1] = (p4cell) fid;
    SP[0] = fid ? 0 : PFE_io_errno;
}

/** DELETE-FILE ( name-ptr name-len -- name-errno# ) [ANS]
 * delete the named file and return a status code
 */
void FXCode (p4_delete_file)
{
    register char* filename =
        p4_pocket_filename ((p4_char_t*)SP[1], SP[0]) ; /* as asciiz */
    SP += 1;
    SP[0] = _pfe_remove (filename) ? PFE_io_errno : 0;
}

/** FILE-POSITION ( some-file* -- p,pos# some-errno# ) [ANS]
 * return the current position in the file and
 * return a status code. A code of zero means success.
 */
void FXCode (p4_file_position)
{
    register File *fid = (File *) SP[0];	/* file-id */
    register _p4_off_t pos;

    SP -= 2;
    if (_is_nullfile(fid)) goto nullfile;
    pos = _p4_ftello (fid->f);
    if (pos != -1)
    {
        SP[2] = (p4ucell)(pos);
        if (sizeof (*SP) >= sizeof(pos)) /* compile-time decision !*/
            SP[1] = 0;
        else                            /* assume: 1x or 2x sizeof(*SP) */
            SP[1] = (p4ucell)(pos >> 8*(sizeof(pos)-sizeof(*SP)));
        SP[0] = 0;		/* ior */
    }else{
        SP[2] = UCELL_MAX;      /* set to -1 */
        SP[1] = UCELL_MAX;
        SP[0] = PFE_io_errno;	/* ior */
    }
    return;
 nullfile:
    SP[2] = 0;
    SP[1] = 0;
    SP[0] = EINVAL;
    P4_warn ("trying seek on NULL file");
}

/** FILE-SIZE ( some-file* -- s,size# some-errno# ) [ANS]
 * return the current size of the file and
 * return a status code. A code of zero means success.
 */
void FXCode (p4_file_size)
{
    File *fid = (File *) SP[0];	/* fileid */
    _p4_off_t size;

    if (_is_nullfile(fid)) goto nullfile;
    size = p4_file_size (fid->f);
    SP -= 2;
    if (size != -1)
    {
        SP[2] = (p4ucell)(size);
        if (sizeof (*SP) >= sizeof(size)) /* compile-time decision !*/
            SP[1] = 0;
        else                            /* assume: 1x or 2x sizeof(*SP) */
            SP[1] = (p4ucell)(size >> 8*(sizeof(size)-sizeof(*SP)));
        SP[0] = 0;		/* ior */
    }else{
        SP[2] = UCELL_MAX;      /* set to -1 */
        SP[1] = UCELL_MAX;
        SP[0] = PFE_io_errno;	/* ior */
    }
    return;
 nullfile:
    SP[2] = 0;
    SP[1] = 0;
    SP[0] = EINVAL;
    P4_warn ("trying seek on NULL file");
}

/** INCLUDE-FILE ( some-file* -- ) [ANS]
 * => INTERPRET the given file
 */
void FXCode (p4_include_file)
{
    p4_include_file ((File *) *SP++);
}

/** INCLUDED ( name-ptr name-len -- ) [ANS]
 * open the named file and then => INCLUDE-FILE
 * see also the interactive => INCLUDE
 */
void FXCode (p4_included)
{
    register p4_char_t *fn = (p4_char_t *) SP[1]; /* c-addr, name */
    register p4ucell u = SP[0];	                  /* length of name */

    SP += 2;
    p4_included (fn, u);
}

/** OPEN-FILE ( name-ptr name-len open-mode# -- name-file* name-errno# ) [ANS]
 * open the named file with mode. returns the
 * file id and a status code. A code of zero
 * means success.
 */
void FXCode (p4_open_file)
{
    register p4_char_t *fn = (p4_char_t *) SP[2]; /* c-addr, name */
    register p4ucell u = SP[1];	                  /* length of name */
    register p4cell fam = SP[0];                  /* file access mode */
    register File *fid = p4_open_file (fn, u, fam);

    SP += 1;
    SP[1] = (p4cell) fid;
    SP[0] = fid ? 0 : PFE_io_errno;
}

/** READ-FILE ( buf-ptr buf-len some-file* -- buf-count some-errno# ) [ANS]
 * fill the given string buffer with characters
 * from the buffer. A status code of zero means
 * success and the returned count gives the
 * number of bytes actually read. If an error
 * occurs the number of already transferred bytes
 * is returned.
 */
void FXCode (p4_read_file)
{
    register p4_char_t *  buf = (p4_char_t *) SP[2];
    register p4ucell len = SP[1];
    register File *  fid = (File *) SP[0];
    SP += 1;
    if (_is_nullfile(fid)) goto nullfile;
    SP[1] = len;
    SP[0] = p4_read_file (buf, ((p4ucell*)SP) + 1, fid);
    return;
 nullfile:
    SP[0] = EINVAL;
    SP[1] = 0;
    P4_fail ("trying read from NULL file" CHECKFILE);
}

/** READ-LINE ( buf-ptr buf-len some-file* -- buf-count buf-flag some-errno# ) [ANS]
 * fill the given string buffer with one line
 * from the file. A line termination character
 * (or character sequence under WIN/DOS) may
 * also be placed in the buffer but is not
 * included in the final count. In other respects
 * this function performs a => READ-FILE
 */
void FXCode (p4_read_line)
{
    register p4_char_t *  buf = (p4_char_t *) SP[2];
    register p4ucell len = SP[1];
    register File *  fid = (File *) SP[0];
    if (_is_nullfile(fid)) goto nullfile;
    SP[2] = len;
    SP[1] = p4_read_line (buf, ((p4ucell*)SP) + 2, fid, & SP[0]);
    return;
 nullfile:
    SP[0] = EINVAL;
    SP[1] = EINVAL;
    SP[2] = 0;
    P4_fail ("trying read from NULL file" CHECKFILE);
}

/** REPOSITION-FILE ( o,offset# some-file* -- some-errno# ) [ANS]
 * reposition the file offset - the next => FILE-POSITION
 * would return o.offset then. returns a status code where zero means success.
 */
void FXCode (p4_reposition_file)
{
    register File *fid = (File *) SP[0];
    register _p4_off_t pos;
    if (sizeof (*SP) >= sizeof(pos))  /* compile-time decision !*/
    {
        pos = SP[2];
    } else
    {
        pos = (p4ucell) SP[1];
        pos <<= 8*(sizeof(pos)-sizeof(*SP)); /* assume: 1x or 2x sizeof(*SP) */
        pos |=  (p4ucell)(SP[2]);
    }

    SP += 2;
    if (_is_nullfile(fid)) goto nullfile;
    SP[0] = p4_reposition_file (fid, pos);
    return;
 nullfile:
    SP[0] = EINVAL;
    P4_warn ("trying seek on NULL file");
}

/** RESIZE-FILE ( s,size# some-file* -- some-errno# ) [ANS]
 * resize the give file, returns a status code where zero means success.
 */
void FXCode (p4_resize_file)
{
    register File *fid = (File *) SP[0];
    register _p4_off_t size;
    if (sizeof (*SP) >= sizeof(size))  /* compile-time decision !*/
    {
        size = SP[2];
    } else
    {
        size = (p4ucell) SP[1];
        size <<= 8*(sizeof(size)-sizeof(*SP)); /* assume: 1x or 2x size(*SP) */
        size |=  (p4ucell)(SP[2]);
    }

    SP += 2;
    if (_is_nullfile(fid)) goto nullfile;
    if (p4_resize_file (fid, size) != 0)
        *SP = PFE_io_errno;
    else
        *SP = 0, fid->size = (p4ucell) (size / BPBUF);
    return;
 nullfile:
    SP[0] = EINVAL;
    P4_fail ("trying seek on NULL file" CHECKFILE);
}

/** WRITE-FILE ( buf-ptr buf-len some-file* -- some-errno# ) [ANS]
 * write characters from the string buffer to a file,
 * returns a status code where zero means success.
 */
void FXCode (p4_write_file)
{
    register char *  buf = (char *) SP[2];
    register p4ucell len = SP[1];
    register File *  fid = (File *) SP[0];

    SP += 2;
    if (_is_nullfile(fid)) goto nullfile;
    SP[0] = p4_write_file (buf, len, fid);
    return;
 nullfile:
    SP[0] = EINVAL;
    P4_fail ("trying write to NULL file" CHECKFILE);
}

/** WRITE-LINE ( buf-ptr buf-len some-file* -- some-errno# ) [ANS]
 * write characters from the string buffer to a file,
 * and add the line-terminator to the end of it.
 * returns a status code.
 */
void FXCode (p4_write_line)
{
    register char *  buf = (char *) SP[2];
    register p4ucell len = SP[1];
    register File *  fid = (File *) SP[0];

    SP += 2;
    if (_is_nullfile(fid)) goto nullfile;
    if (! (SP[0] = p4_write_file (buf, len, fid)))
        putc ('\n', fid->f);
    return;
 nullfile:
    SP[0] = EINVAL;
    P4_fail ("trying write to NULL file" CHECKFILE);
}

/** FILE-STATUS ( file-ptr file-len -- file-subcode# file-errno# ) [ANS]
 * check the named file - if it exists
 * the status errno code is zero. The status subcode
 * is implementation-specific and usually matches the
 * file access permission bits of the filesystem.
 */
void FXCode (p4_file_status)
{
    register int mode = p4_file_access ((p4_char_t *) SP[1], SP[0]);

    if (mode == -1)
    {
        SP[1] = 0;
        SP[0] = PFE_io_errno;
    }else{
        SP[1] = mode;
        SP[0] = 0;
    }
}

/** FLUSH-FILE ( some-file* -- some-errno# ) [ANS]
 * flush all unsaved buffers of the file to disk.
 * A status code of zero means success.
 */
void FXCode (p4_flush_file)
{
    register File *fid = (File *) SP[0];

    if (_is_nullfile(fid)) goto nullfile;
    if (BLOCK_FILE == fid)
    {
        FX (p4_save_buffers);
        SP[0] = 0;
    }
    else if (fflush (fid->f))
        SP[0] = PFE_io_errno;
    else
        SP[0] = 0;
    return;
 nullfile:
    SP[0] = EINVAL;
    P4_warn ("trying flush on NULL file");
}

/** RENAME-FILE ( oldname-ptr oldname-len newname-ptr newname-len -- newname-errno# ) [ANS]
 * rename the file named by "oldname" to the name of "newname"
 * returns a status-code where zero means success.
 */
void FXCode (p4_rename_file)
{
    register char* oldnm;
    register char* newnm;

    oldnm = p4_pocket_filename ((p4_char_t *) SP[3], SP[2]);
    newnm = p4_pocket_filename ((p4_char_t *) SP[1], SP[0]);
    SP += 3;
    *SP = _P4_rename (oldnm, newnm) ? PFE_io_errno : 0;
}

static void FXCode (p__max_files)
{
    FX_PUSH (PFE.files_top - PFE.files); /* in items, div sizeof(p4_File) */
}


/** R/O ( -- readonly-mode# ) [ANS]
 * a bitmask for => OPEN-FILE ( => R/O => R/W => W/O => BIN )
 */

/** W/O ( -- writeonly-mode# ) [ANS]
 * a bitmask for => OPEN-FILE or => CREATE-FILE ( => R/O => R/W => W/O => BIN )
 */

/** R/W ( -- readwrite-mode# ) [ANS]
 * a bitmask for => OPEN-FILE or => CREATE-FILE ( => R/O => R/W => W/O => BIN )
 */

/** "ENVIRONMENT MAX-FILES" ( -- files-max ) [ENVIRONMENT]
 * the number of opened file-ids allowed during compilation.
 * portable programs can check this with => ENVIRONMENT?
 */

P4_LISTWORDSET (file) [] =
{
    P4_INTO ("[ANS]", 0),
    P4_FXco ("BIN",		 p4_bin),
    P4_FXco ("CLOSE-FILE",	 p4_close_file),
    P4_FXco ("CREATE-FILE",	 p4_create_file),
    P4_FXco ("DELETE-FILE",	 p4_delete_file),
    P4_FXco ("FILE-POSITION",	 p4_file_position),
    P4_FXco ("FILE-SIZE",	 p4_file_size),
    P4_FXco ("INCLUDE-FILE",	 p4_include_file),
    P4_FXco ("INCLUDED",	 p4_included),
    P4_FXco ("OPEN-FILE",	 p4_open_file),
    P4_OCoN ("R/O",		 FMODE_RO),
    P4_OCoN ("R/W",		 FMODE_RW),
    P4_FXco ("READ-FILE",	 p4_read_file),
    P4_FXco ("READ-LINE",	 p4_read_line),
    P4_FXco ("REPOSITION-FILE",	 p4_reposition_file),
    P4_FXco ("RESIZE-FILE",	 p4_resize_file),
    P4_OCoN ("W/O",		 FMODE_WO),
    P4_FXco ("WRITE-FILE",	 p4_write_file),
    P4_FXco ("WRITE-LINE",	 p4_write_line),
    P4_FXco ("FILE-STATUS",	 p4_file_status),
    P4_FXco ("FLUSH-FILE",	 p4_flush_file),
    P4_FXco ("RENAME-FILE",	 p4_rename_file),

    P4_INTO ("ENVIRONMENT", 0 ),
    P4_OCON ("FILE-EXT",	 1994 ),
    P4_FXCO ("MAX-FILES",	 p__max_files),

};
P4_COUNTWORDSET (file, "File-access + extensions");

/*@}*/
