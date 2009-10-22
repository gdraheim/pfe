#ifndef __PFE_DEF_PATHS_H
#define __PFE_DEF_PATHS_H
/**
 * -- path style defines
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
 *             System location on disk and file name conventions,
 *             detect from PFE_PATH_STYLE ; used in pfe sources...
 */
/*@{*/

#include <pfe/def-config.h>

#ifndef PFE_VARIANT
#define PFE_VARIANT ""
#endif
#ifndef PFE_PACKAGE
#define PFE_PACKAGE "pfe"
#endif
#define PFE_PKGVARIANT PFE_PACKAGE PFE_VARIANT

#if PFE_PATH_STYLE & 16
/* MS-DOS like file and path names */

#if defined HOST_OS_WATCOM
# define _STR_(X) #X
#else
# define _STR_(X) P4STRING(X)
#endif

# ifndef PFE_EPREFIX
#   ifdef EPREFIX
#   define PFE_EPREFIX		EPREFIX
#   else
#   define PFE_EPREFIX		"C:"
#   endif
# endif
# ifndef PFE_PKGLIBDIR
# define PFE_PKGLIBDIR	        PFE_EPREFIX"\\bin\\"PFE_PKGVARIANT
# endif
# ifndef PFE_PKGDATADIR
# define PFE_PKGDATADIR	        PFE_EPREFIX"\\share\\"PFE_PACKAGE
# endif
# ifndef PFE_PKGHELPDIR
# define PFE_PKGHELPDIR		PFE_PKGLIBDIR"\\HELP"
# endif

# define PFE_PATH_DELIMITER	';'
# define PFE_DIR_DELIMITER	'\\'
# define PFE_DIR_DELIMSTR       "\\"
# define PFE_LIB_PATH           PFE_PKGLIBDIR
# define PFE_INC_PATH		";"PFE_PKGDATADIR
# define PFE_INC_EXTENSIONS	";.P4;.FS;.4TH;.FTH"
# define PFE_BLK_PATH		PFE_INC_PATH
# define PFE_BLK_EXTENSIONS	";.BLK;.SCR;.FB"
# define PFE_DEFAULT_BLKFILE	"PFE.BLK"
# define PFE_LLCMD		"DIR"
# define PFE_LSCMD		"DIR /W"

#elif PFE_PATH_STYLE & 32
/* UNIX-like file and path names */

# ifndef PFE_EPREFIX
#   ifdef EPREFIX
#   define PFE_EPREFIX		EPREFIX
#   else
#   define PFE_EPREFIX		"/usr/local"
#   endif
# endif
# ifndef PFE_PKGLIBDIR
# define PFE_PKGLIBDIR	        PFE_EPREFIX"/lib/"PFE_PKGVARIANT
# endif
# ifndef PFE_PKGDATADIR
# define PFE_PKGDATADIR	        PFE_EPREFIX"/share/"PFE_PACKAGE
# endif
# ifndef PFE_PKGHELPDIR
# define PFE_PKGHELPDIR		PFE_PKGLIBDIR"/help"
# endif


# define PFE_PATH_DELIMITER	':'
# define PFE_DIR_DELIMITER	'/'
# define PFE_DIR_DELIMSTR       "/"
# define PFE_LIB_PATH           PFE_PKGLIBDIR
# define PFE_INC_PATH		":~/.pfe:"PFE_PKGDATADIR
# define PFE_INC_EXTENSIONS	":.p4:.fs:.4th:.fth:.forth"
# define PFE_BLK_PATH		PFE_INC_PATH
# define PFE_BLK_EXTENSIONS	":.blk:.scr:.fb"
# define PFE_DEFAULT_BLKFILE	"pfe.blk"
# define PFE_LLCMD		"ls -alF"
# define PFE_LSCMD		"ls -C"

#elif PFE_PATH_STYLE & 64
/* WEB-like file and path names */

# ifndef PFE_EPREFIX
#   if defined EPREFIX
#   define PFE_EPREFIX		EPREFIX
#   elif defined EPREFIX
#   define PFE_EPREFIX          EPREFIX
#   else
#   define PFE_EPREFIX		"file:/usr/local"
#   endif
# endif
# ifndef PFE_PKGLIBDIR
# define PFE_PKGLIBDIR	        PFE_EPREFIX"/lib/"PFE_PKGVARIANT
# endif
# ifndef PFE_PKGDATADIR
# define PFE_PKGDATADIR	        PFE_EPREFIX"/share/"PFE_PACKAGE
# endif
# ifndef PFE_PKGHELPDIR
# define PFE_PKGHELPDIR		PFE_PKGLIBDIR"/help"
# endif

# define PFE_PATH_DELIMITER	';'
# define PFE_DIR_DELIMITER	'/'
# define PFE_DIR_DELIMSTR       "/"
# define PFE_LIB_PATH           PFE_PKGLIBDIR
# define PFE_INC_PATH		";~/.pfe;"PFE_PKGDATADIR
# define PFE_INC_EXTENSIONS	";.p4;.fs;.4th;.fth;.forth"
# define PFE_BLK_PATH	        PFE_INC_PATH
# define PFE_BLK_EXTENSIONS	";.blk;.scr;.fb"
# define PFE_DEFAULT_BLKFILE	"pfe.blk"
# define PFE_LLCMD		"ls -alF"
# define PFE_LSCMD		"ls -C"

#else
/* UNKNOWN file and path names */

#error "Don't know what kind of file names your system uses, check pfe/_config.h"

#endif

#ifndef PFE_PREFIX
#define PFE_PREFIX              PFE_EPREFIX
#endif

#ifndef PFE_DEF_PATH_MAX
#define PFE_DEF_PATH_MAX        256
#endif

/*@}*/
#endif
