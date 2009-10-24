/**
 * -- Stackhelp for Optional File-Access Word Set
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

P4_LISTWORDSET (file_check) [] =
{
    P4_STKi ("BIN",		"access-mode# -- access-mode#'"),
    P4_STKi ("CLOSE-FILE",	"file* -- errno#"),
    P4_STKi ("CREATE-FILE",	"str-ptr str-len open-mode# -- file* errno#"),
    P4_STKi ("DELETE-FILE",	"str-ptr -str-len -- errno#"),
    P4_STKi ("FILE-POSITION",	"file* -- p,pos# errno#"),
    P4_STKi ("FILE-SIZE",	"file* -- s,size# errno#"),
    P4_STKi ("INCLUDE-FILE",	"file* --"),
    P4_STKi ("INCLUDED",	"str-ptr str-len"),
    P4_STKi ("OPEN-FILE",	"str-ptr str-len open-mode# -- file* errno#"),
    P4_STKi ("R/O",		"-- readonly-mode#"),
    P4_STKi ("R/W",		"-- readwrite-mode#"),
    P4_STKi ("READ-FILE",    "str-ptr str-len file* -- count errno#"),
    P4_STKi ("READ-LINE",    "str-ptr str-len file* -- count flag? errno#"),
    P4_STKi ("REPOSITION-FILE",	"o,offset# file* -- errno#"),
    P4_STKi ("RESIZE-FILE",	"s,size file* -- errno#"),
    P4_STKi ("W/O",		"-- writeonly-mode#"),
    P4_STKi ("WRITE-FILE",	"str-ptr str-len file* -- errno#"),
    P4_STKi ("WRITE-LINE",	"str-ptr str-len file* -- errno#"),
    P4_STKi ("FILE-STATUS",	"str-ptr str-len -- code# errno#"),
    P4_STKi ("FLUSH-FILE",	"file* -- errno#"),
    P4_STKi ("RENAME-FILE",  "str1-ptr str1-len str2-ptr str2-len -- errno#"),

    /* file-mix */
    P4_STKi ("INCLUDE",		"<name> -- ???"),

    P4_STKi ("COPY-FILE",	"src-ptr src-len dst-ptr dst-len -- errno#"),
    P4_STKi ("MOVE-FILE",	"src-ptr str-len dst-ptr dst-len -- errno#"),
    P4_STKi ("FILE-R/W",	"buffer* block# flag? file* --"),
    P4_STKi ("FILE-BLOCK",	"bock# file* -- buffer*"),
    P4_STKi ("FILE-BUFFER",	"block# file* -- buffer*"),
    P4_STKi ("FILE-EMPTY-BUFFERS", "file* --"),
    P4_STKi ("FILE-FLUSH",	"file* --"),
    P4_STKi ("FILE-LIST",	"block# file* --"),
    P4_STKi ("FILE-LOAD",	"block# file* --"),
    P4_STKi ("FILE-SAVE-BUFFERS", "file* --"),
    P4_STKi ("FILE-THRU",	"lo-block# hi-block# file* --"),
    P4_STKi ("FILE-UPDATE",	"file* --"),

    P4_INTO ("ENVIRONMENT", 0),
    P4_STKi ("FILE-EXT",        "-- year# true! | 0"),
    P4_STKi ("MAX-FILES",       "-- filesmax#"),
};
P4_COUNTWORDSET (file_check, "Check-File-access + extensions");

/*@}*/
