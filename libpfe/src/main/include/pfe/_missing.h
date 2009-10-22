#ifndef __PFE__MISSING_H
#define __PFE__MISSING_H
/**
 * -- declarations sometimes missing.
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2000.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author$)
 *  @version $Revision$
 *     (modified $Date$)
 *
 *  @description
 *		This file is included after all system includes.
 *
 *              It will only work with _P4_SOURCE and for pfe's config.h.
 *              Use only for pfe-specific pfe-loadable third-party modules.
 */
/*@{*/
#if defined _P4_SOURCE

/* we are local... really */
#include "pfe/_config.h"

#if defined HAVE_SYS_STAT_H || defined PFE_HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#if defined HAVE_FCNTL_H || defined PFE_HAVE_FCNTL_H
#include <fcntl.h>
#endif

#if defined HAVE_IO_H || defined PFE_HAVE_IO_H
#include <io.h>
#endif

#ifndef STDIN_FILENO
#define STDIN_FILENO 0
#endif
#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1
#endif
#ifndef STDERR_FILENO
#define STDERR_FILENO 2
#endif


#ifndef SEEK_SET
#define SEEK_SET 0		/* Set file pointer to "offset" */
#endif
#ifndef SEEK_CUR
#define SEEK_CUR 1		/* Set file pointer to current plus "offset" */
#endif
#ifndef SEEK_END
#define SEEK_END 2		/* Set file pointer to EOF plus "offset" */
#endif


#ifndef F_OK
#define F_OK 0
#endif
#ifndef X_OK
#define X_OK 1
#endif
#ifndef W_OK
#define W_OK 2
#endif
#ifndef R_OK
#define R_OK 4
#endif

#ifndef S_IREAD
#define S_IREAD S_IRUSR
#endif
#ifndef S_IWRITE
#define S_IWRITE S_IWUSR
#endif
#ifndef S_IEXEC
#define S_IEXEC S_IXUSR
#endif

#ifndef EPERM
#define EPERM -1
#endif

#if defined SYS_WC_OS2V2 && !defined SIG_ACK
#define SIG_ACK ((void (*)(int))4)
#endif

#if !defined PFE_HAVE_STRERROR && !defined HAVE_SYS_ERRLIST
   /* well if it doesn't exist, you'll have to write it yourself */
   extern char* sys_errlist [];
#  define strerror(x) sys_errlist[n]
#endif

#ifndef _pfe_remove
# if defined HAVE_REMOVE || defined PFE_HAVE_REMOVE
# define _pfe_remove remove
# else
# define _pfe_remove unlink
# endif
#endif

#ifndef PFE_HAVE_TRUNCATE
int _p4_truncate (const char*, _p4_off_t len);
#define _P4_truncate _p4_truncate
#else
#define _P4_truncate(_A_,_B_) truncate(_A_,_B_)
#endif

#ifndef PFE_HAVE_RENAME
int _p4_rename (const char*, const char*);
#define _P4_rename _p4_rename
#else
#define _P4_rename(_A_,_B_) rename(_A_,_B_)
#endif

#ifndef PFE_HAVE_ACCESS
int _p4_access (const char* fn, int how);
#define _P4_access _p4_access
#else
#define _P4_access(_A_,_B_) access(_A_,_B_)
#endif

#ifndef PFE_HAVE_GETCWD
#define getcwd(P,L) getwd(P)
#endif

#ifndef _pfe_raise
# if defined HAVE_RAISE || defined PFE_HAVE_RAISE
# define _pfe_raise raise
# else
# define _pfe_raise(X) kill (getpid (), X)
# endif
#endif

#endif
 /* complete file only included if _P4_SOURCE defined */

/*@}*/
#endif

