/** 
 * FILE ---  Optional File-Access Word Set
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 5.5 %
 *    (%date_modified: Mon Mar 12 10:32:18 2001 %)
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
"@(#) $Id: file-ext.c,v 0.30 2001-03-12 09:32:18 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-xtra.h>

#include <stdio.h>
#include <errno.h>

#include <pfe/file-sub.h>
#include <pfe/_missing.h>

/* long <-> p4dcell conversion macros, won't work on 16 bit machines */
#define UL2UDC(UL, UDC)	((UDC).hi = 0, (UDC).lo = (p4ucell)(UL))
#define UDC2UL(HI, LO)	(LO)

/* ================================================================= */

/** BIN ( access-mode -- access-mode' )
 * modify the give file access-mode to be a binary-mode
 */
FCode (p4_bin)
{
    *SP += FMODE_BIN;
}

/** CLOSE-FILE ( file -- code )
 * close the file and return the status-code
 */
FCode (p4_close_file)
{
    File *fid = (File *) SP[0];

    SP[0] = p4_close_file (fid) ? errno : 0;
}

/** CREATE-FILE ( str-adr str-len mode -- file code )
 * create the file with the given name and open
 * it - returns the file id and a status code.
 * A code of zero means success. An existing file
 * of the same name is truncated upon open.
 */
FCode (p4_create_file)
{
    char *fn = (char *) SP[2];	/* c-addr, name */
    p4ucell u = SP[1];		/* length of name */
    p4cell fam = SP[0];		/* file access mode */
    File *fid = p4_create_file (fn, u, fam);
    
    SP += 1;
    SP[1] = (p4cell) fid;
    SP[0] = fid ? 0 : errno;
}

/** DELETE-FILE ( str-adr str-len -- code )
 * delete the named file and return a status code
 */
FCode (p4_delete_file)
{
    char* fnz = p4_pocket_filename ((char*)SP[1], SP[0]) ; /* as asciiz */
    SP += 1;
    SP[0] = _pfe_remove (fnz) ? errno : 0;
}

/** FILE-POSITION ( file -- p.pos code )
 * return the current position in the file and
 * return a status code. A code of zero means success.
 */
FCode (p4_file_position)
{
    File *fid = (File *) SP[0];	/* file-id */
    long pos = ftell (fid->f);
    p4udcell ud;

    SP -= 2;
    if (pos != -1)
    {
        UL2UDC (pos, ud);
        SP[0] = 0;		/* ior */
    }else{
        ud.lo = ud.hi = UCELL_MAX;
        SP[0] = errno;		/* ior */
    }
    *(p4udcell *) &SP[1] = ud;	/* ud */
}

/** FILE-SIZE ( file -- s.size code )
 * return the current size of the file and
 * return a status code. A code of zero means success.
 */
FCode (p4_file_size)
{
    File *fid = (File *) SP[0];	/* fileid */
    long size = fsize (fid->f);
    p4udcell ud;

    SP -= 2;
    if (size != -1)
    {
        UL2UDC (size, ud);
        SP[0] = 0;		/* ior */
    }else{
        ud.lo = ud.hi = UCELL_MAX;
        SP[0] = errno;		/* ior */
    }
    *(p4udcell *) &SP[1] = ud;	/* ud */
}

/** INCLUDE-FILE ( file -- )
 * => INTERPRET the given file
 */
FCode (p4_include_file)
{
    p4_include_file ((File *) *SP++);
}

/** INCLUDED ( str-adr str-len -- )
 * open the named file and then => INCLUDE-FILE
 * see also the interactive => INCLUDE
 */
FCode (p4_included)
{
    char *fn = (char *) SP[1];	/* c-addr, name */
    p4ucell u = SP[0];		/* length of name */

    SP += 2;
    p4_included (fn, u);
}

/** OPEN-FILE ( str-adr str-len mode -- file code )
 * open the named file with mode. returns the
 * file id and a status code. A code of zero
 * means success.
 */
FCode (p4_open_file)
{
    char *fn = (char *) SP[2];	/* c-addr, name */
    p4ucell u = SP[1];		/* length of name */
    p4cell fam = SP[0];		/* file access mode */
    File *fid = p4_open_file (fn, u, fam);

    SP += 1;
    SP[1] = (p4cell) fid;
    SP[0] = fid ? 0 : errno;
}

/** READ-FILE ( str-adr str-len file -- count code )
 * fill the given string buffer with characters
 * from the buffer. A status code of zero means
 * success and the returned count gives the
 * number of bytes actually read. If an error
 * occurs the number of already transferred bytes 
 * is returned.
 */
FCode (p4_read_file)
{
    char *c_addr = (char *) SP[2];
    p4ucell u = SP[1];
    File *fid = (File *) SP[0];
    p4cell r = p4_read_file (c_addr, &u, fid);

    SP += 1;
    SP[1] = u;
    SP[0] = r;
}

/** READ-LINE ( str-adr str-len file -- count flag code )
 * fill the given string buffer with one line
 * from the file. A line termination character
 * (or character sequence under WIN/DOS) may
 * also be placed in the buffer but is not
 * included in the final count. In other respects
 * this function performs a => READ-FILE
 */
FCode (p4_read_line)
{
    char *c_addr = (char *) SP[2];
    p4ucell u = SP[1];
    File *fid = (File *) SP[0];
    p4cell ior;
    int r = p4_read_line (c_addr, &u, fid, &ior);
    
    SP[2] = u;
    SP[1] = r;
    SP[0] = ior;
}

/** REPOSITION-FILE ( o.offset file -- code )
 * reposition the file offset - the next => FILE-POSITION
 * would return o.offset then. returns a status code.
 */
FCode (p4_reposition_file)
{
    File *fid = (File *) SP[0];
    long pos = UDC2UL (SP[1], SP[2]);
    
    SP += 2;
    SP[0] = p4_reposition_file (fid, pos);
}

/** RESIZE-FILE ( s.size file -- code )
 * resize the give file, returns a status code.
 */
FCode (p4_resize_file)
{
    File *fid = (File *) SP[0];
    long size = UDC2UL (SP[1], SP[2]);
    
    SP += 2;
    if (p4_resize_file (fid, size) != 0)
        *SP = errno;
    else
        *SP = 0, fid->size = (p4ucell) (size / BPBUF);
}

/** WRITE-FILE ( str-adr str-len file -- code )
 * write characters from the string buffer to a file,
 * returns a status code.
 */ 
FCode (p4_write_file)
{
    char *c_addr = (char *) SP[2];
    p4ucell u = SP[1];
    File *fid = (File *) SP[0];

    SP += 2;
    SP[0] = p4_write_file (c_addr, u, fid);
}

/** WRITE-LINE ( str-adr str-len file -- )
 * write characters from the string buffer to a file,
 * and add the line-terminator to the end of it.
 * returns a status code.
 */
FCode (p4_write_line)
{
    char *c_addr = (char *) SP[2];
    p4ucell u = SP[1];
    File *fid = (File *) SP[0];

    SP += 2;
    if ((SP[0] = p4_write_file (c_addr, u, fid)) == 0)
        putc ('\n', fid->f);
}

/** FILE-STATUS ( str-adr str-len -- sub-code code )
 * check the named file - if it exists
 * the status code is zero. The sub-code
 * is implementation-specific.
 */
FCode (p4_file_status)
{
    int mode = p4_file_access ((char *) SP[1], SP[0]);

    if (mode == -1)
    {
        SP[1] = 0;
        SP[0] = errno;
    }else{
        SP[1] = mode;
        SP[0] = 0;
    }
}

/** FLUSH-FILE ( file -- code )
 * flush all unsaved buffers of the file to disk.
 * A status code of zero means success.
 */
FCode (p4_flush_file)
{
    File *fid = (File *) SP[0];

    if (BLOCK_FILE == fid)
    {
        FX (p4_save_buffers);
        SP[0] = 0;
    }else{
        if (fflush (fid->f))
            SP[0] = errno;
        else
            SP[0] = 0;
    }
}

/** RENAME-FILE ( str-adr1 str-len1 str-adr2 str-len2 -- code )
 * rename the file named by string1 to the name of string2.
 * returns a status-code
 */
FCode (p4_rename_file)
{
    char* oldnm;
    char* newnm;

    oldnm = p4_pocket_filename ((char *) SP[3], SP[2]);
    newnm = p4_pocket_filename ((char *) SP[1], SP[0]);
    SP += 3;
    *SP = rename (oldnm, newnm) ? errno : 0;
}

static FCode (p__max_files)
{
    FX_PUSH (PFE_set.max_files);
}


/** R/O ( -- bitmask )
 * a bitmask for => OPEN-FILE ( => R/O => R/W => W/O => BIN )
 */

/** W/O ( -- bitmask )
 * a bitmask for => OPEN-FILE or => CREATE-FILE ( => R/O => R/W => W/O => BIN )
 */

/** R/W ( -- bitmask )
 * a bitmask for => OPEN-FILE or => CREATE-FILE ( => R/O => R/W => W/O => BIN )
 */

/** "ENVIRONMENT MAX-FILES" ( -- number )
 * the number of opened file-ids allowed during compilation.
 * portable programs can check this with => ENVIRONMENT?
 */

P4_LISTWORDS (file) =
{
    CO ("BIN",		 p4_bin),
    CO ("CLOSE-FILE",	 p4_close_file),
    CO ("CREATE-FILE",	 p4_create_file),
    CO ("DELETE-FILE",	 p4_delete_file),
    CO ("FILE-POSITION", p4_file_position),
    CO ("FILE-SIZE",	 p4_file_size),
    CO ("INCLUDE-FILE",	 p4_include_file),
    CO ("INCLUDED",	 p4_included),
    CO ("OPEN-FILE",	 p4_open_file),
    OC ("R/O",		 FMODE_RO),
    OC ("R/W",		 FMODE_RW),
    CO ("READ-FILE",	 p4_read_file),
    CO ("READ-LINE",	 p4_read_line),
    CO ("REPOSITION-FILE",p4_reposition_file),
    CO ("RESIZE-FILE",	 p4_resize_file),
    OC ("W/O",		 FMODE_WO),
    CO ("WRITE-FILE",	 p4_write_file),
    CO ("WRITE-LINE",	 p4_write_line),
    CO ("FILE-STATUS",	 p4_file_status),
    CO ("FLUSH-FILE",	 p4_flush_file),
    CO ("RENAME-FILE",	 p4_rename_file),
    P4_INTO ("ENVIRONMENT", 0 ),
    /* enviroment hints (testing for -EXT will mark this wordset as present) */
    P4_OCON ("FILE-EXT",	1994 ),
    P4_FXCO ("MAX-FILES",	p__max_files),
    
};
P4_COUNTWORDS (file, "File-access + extensions");

/*@}*/

