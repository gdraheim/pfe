#ifndef __PFE__MISSING_H
#define __PFE__MISSING_H "%full_filespec: _missing.h~5.5:incl:bln_12xx!1 %"
/** 
 * -- declarations sometimes missing.
 * 
 *  Copyright (C) Tektronix, Inc. 1998 - 2000. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE            @(#) %derived_by: guidod %
 *  @version %version: 5.5 %
 *    (%date_modified: Mon Mar 12 10:33:13 2001 %)
 *
 *  @description
 *		This file is included after all system includes.
 * 
 *              It will only work with _P4_SOURCE and for pfe's config.h.
 *              Use only for pfe-specific pfe-loadable third-party modules.
 */
/*@{*/
#if defined _P4_SOURCE && defined HAVE_CONFIG_H
#include "config.h"

#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
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

#if !defined HAVE_MEMMOVE && defined HAVE_BCOPY
#define memmove(D,S,C) bcopy (S,D,C)
#define HAVE_MEMMOVE 2
#else
/* void memmove (char*, const char*, unsigned); */
#endif

#ifndef HAVE_STRDUP
char* strdup (const char* s);
#endif

#if !defined HAVE_STRERROR && !defined HAVE_SYS_ERRLIST
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

#ifndef HAVE_RENAME
int rename (const char*, const char*);
#endif

#ifndef HAVE_ACCESS
int access (const char* fn, int how);
#endif

#ifndef HAVE_GETCWD
#define getcwd(P,L) getwd(P)
#endif

#ifndef HAVE_TRUNCATE
int truncate (const char* path, long length);
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

