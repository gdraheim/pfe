/**
 * -- os-like / shell-like commands for pfe
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
 *        These builtin words are modelled after common shell commands,
 *        so that the Portable Forth Environment can often
 *        be put in the place of a normal OS shell.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char * id __attribute__((unused)) =
"@(#) $Id$";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-limits.h>

#include <stdio.h>
#include <stdlib.h>
#include <pfe/os-string.h>
#include <limits.h>
#include <fcntl.h>
#include <errno.h>
#ifdef PFE_HAVE_IO_H
#include <io.h>
#endif
#ifdef PFE_HAVE_UNISTD_H
#include <unistd.h>
#endif

/*FIXME: remove this with a proper pfe-config.h define */
#if defined PFE_HAVE_IO_H && ! defined PFE_HAVE_UNISTD_H
#define PFE_HAVE_DIRECT_H
#endif

#if defined PFE_HAVE_DIRECT_H
#include <direct.h> /* getcwd, mkdir */
#endif

#if defined NO_SYSTEM && defined PFE_HAVE_DIRENT_H
#include <dirent.h>
#include <sys/stat.h>
#endif

#include <pfe/def-comp.h>
#include <pfe/file-sub.h>
#include <pfe/_nonansi.h>
#include <pfe/_missing.h>
#include <pfe/logging.h>

typedef int (*syscall_f)( const char* );
                /*GD* used in do_one, so we don't get warnings */

#ifdef PFE_HAVE_PID
/** $PID ( -- pid )
 * calls system's <c> getpid </c>
 */
void FXCode (p4_getpid)	{ *--SP = (p4cell)getpid (); }
#endif

#ifdef PFE_HAVE_UID
/** $UID ( -- val )
 * calls system's <c> getuid </c>
 */
void FXCode (p4_getuid)	{ *--SP = (p4cell)getuid (); }

/** $EUID ( -- val )
 * calls system's <c> geteuid </c>
 */
void FXCode (p4_geteuid)	{ *--SP = (p4cell)geteuid (); }
#endif

#ifdef PFE_HAVE_GID
/** $GID ( -- val )
 * calls system's <c> getgid </c>
 */
void FXCode (p4_getgid)	{ *--SP = (p4cell)getgid (); }
#endif

#ifdef PFE_HAVE_UMASK
/** UMASK ( val -- ret )
 * calls system's <c> umask </c>
 */
void FXCode (p4_umask)	{ *SP = (p4cell)umask (*SP); }
#endif

/** $HOME ( -- str-ptr str-len )
 * calls system's <c> getenv(HOME) </c>
 */
void FXCode (p4_home)	{ p4_strpush (getenv ("HOME")); }

/** $USER ( -- str-ptr str-len )
 * calls system's <c> getenv(USER) </c>
 */
void FXCode (p4_user)	{ p4_strpush (getenv ("USER")); }

/** $CWD ( -- str-ptr str-len )
 * calls system's <c> getcwd </c>
 */
void FXCode (p4_cwd)	{ p4_strpush (getcwd (p4_pocket (), PATH_LENGTH)); }

/** PWD ( -- )
 * calls system's <c> getcwd </c> and prints it to the screen
 : PWD  $CWD TYPE ;
 */
void FXCode (p4_pwd)
{
    p4_outs (getcwd (p4_pocket (), PATH_LENGTH));
    FX (p4_space);
}

/** CHDIR ( bstring -- )
 * change the current directory. <br>
 */
void FXCode (p4_chdir)
{
    /* pocket_filename expands "~" and replaces "\" and "/" */
    chdir (p4_pocket_filename ((* (p4_char_t**) SP) + 1, (int) **(p4_char_t**) SP));
    FX_DROP;
}

/* shell word helper macros _________________________________________ */

static p4_charbuf_t *
p4_word_comma (char del)
{
    p4_char_t *p = p4_word (del);

    DP += P4_CHARBUF_LEN(p) + 1;
    FX (p4_align);
    return p;
}

static void
do_one (p4_char_t *p, int (*syscall) (const char *))
{
    char* name = p4_pocket_filename (P4_CHARBUF_PTR(p), P4_CHARBUF_LEN(p));
    if (syscall (name))
        p4_throwstr (FX_IOR, name);
}

#if !defined PFE_SBR_CALL_THREADING
#define SHWORD1(X)				\
void FXCode_XE (P4CAT3 (p4_,X,_execution))		\
{						\
    FX_USE_CODE_ADDR;				\
    do_one ((p4_char_t *)IP, (syscall_f)X);	\
    FX_SKIP_STRING;				\
    FX_USE_CODE_EXIT;				\
}						\
void FXCode (P4CAT(p4_,X))				\
{						\
    if (STATE)					\
    {						\
        FX_COMPILE (P4CAT(p4_,X));		\
        p4_word_comma (' ');			\
    }						\
    else					\
        do_one (p4_word (' '), (syscall_f)X);	\
}						\
P4COMPILES (P4CAT(p4_,X), P4CAT3 (p4_,X,_execution),	\
  P4_SKIPS_STRING, P4_DEFAULT_STYLE)
#else /* SBR_THREADING */
#define SHWORD1(X)				\
void FXCode_XE (P4CAT3 (p4_,X,_execution))		\
{						\
    FX_USE_CODE_ADDR;				\
    FX_NEW_IP_WORK;				\
    do_one (FX_NEW_IP_CHAR, (syscall_f)X);	\
    FX_NEW_IP_SKIP_STRING;			\
    FX_NEW_IP_DONE;				\
    FX_USE_CODE_EXIT;				\
}						\
void FXCode (P4CAT(p4_,X))				\
{						\
    if (STATE)					\
    {						\
        FX_COMPILE (P4CAT(p4_,X));		\
        p4_word_comma (' ');			\
    }						\
    else					\
        do_one (p4_word (' '), (syscall_f)X);	\
}						\
P4COMPILES (P4CAT(p4_,X), P4CAT3 (p4_,X,_execution),	\
  P4_SKIPS_STRING, P4_DEFAULT_STYLE)
#endif
static void
do_two (p4_charbuf_t *p1, p4_charbuf_t *p2, int (*syscall) (const char *, const char *))
{
    char* nm1 = p4_pocket_filename (P4_CHARBUF_PTR(p1), P4_CHARBUF_LEN(p1));
    char* nm2 = p4_pocket_filename (P4_CHARBUF_PTR(p2), P4_CHARBUF_LEN(p2));
    if (syscall (nm1, nm2))
        p4_throwstr (FX_IOR, nm1);
}

#if !defined PFE_SBR_CALL_THREADING
#define SHWORD2(X)				\
void FXCode_XE (P4CAT3 (p4_,X,_execution))		\
{   FX_USE_CODE_ADDR; 				\
    p4_charbuf_t *p = (p4_charbuf_t*)IP;	\
    FX_SKIP_STRING;				\
    do_two (p, (p4_charbuf_t*)IP, X);		\
    FX_SKIP_STRING;				\
    FX_USE_CODE_EXIT;				\
}						\
void FXCode (P4CAT(p4_,X))				\
{						\
    if (STATE)					\
    {						\
        FX_COMPILE (P4CAT(p4_,X));		\
        p4_word_comma (' ');			\
        p4_word_comma (' ');			\
    }else{                                      \
        p4_charbuf_t *p = p4_pocket ();         \
        p4_charbuf_t *word = p4_word (' ');     \
        p4_memcpy (p, word, (*word)+1);	        \
        do_two (p, p4_word (' '), X);		\
    }						\
}						\
P4COMPILES (P4CAT(p4_,X), P4CAT3(p4_,X,_execution),	\
  P4_SKIPS_2STRINGS, P4_DEFAULT_STYLE)
#else /* SBR_THREADING */
#define SHWORD2(X)				\
void FXCode_XE (P4CAT3 (p4_,X,_execution))		\
{   FX_USE_CODE_ADDR;				\
    FX_NEW_IP_WORK;				\
    {		                		\
     p4_charbuf_t *p = FX_NEW_IP_CHAR;		\
     FX_NEW_IP_SKIP_STRING;			\
     do_two (p, FX_NEW_IP_CHAR, X);		\
    }		                		\
    FX_NEW_IP_SKIP_STRING;			\
    FX_NEW_IP_DONE;				\
    FX_USE_CODE_EXIT;				\
}						\
void FXCode (P4CAT(p4_,X))				\
{						\
    if (STATE)					\
    {						\
        FX_COMPILE (P4CAT(p4_,X));		\
        p4_word_comma (' ');			\
        p4_word_comma (' ');			\
    }else{                                      \
        p4_charbuf_t *p = p4_pocket ();		\
        p4_charbuf_t *word = p4_word (' ');     \
        p4_memcpy (p, word, (*word)+1);	        \
        do_two (p, p4_word (' '), X);		\
    }						\
}						\
P4COMPILES (P4CAT(p4_,X), P4CAT3(p4_,X,_execution),	\
  P4_SKIPS_2STRINGS, P4_DEFAULT_STYLE)
#endif
/* ---------------------------------------- shell word helper macros ----- */

#ifdef S_IRUSR
# ifdef S_IWGRP
# define RWALL	(S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)
# else
# define RWALL  (S_IRUSR | S_IWUSR)
# endif
# ifdef S_IXGRP
# define RWXALL	(RWALL | S_IXUSR | S_IXGRP | S_IXOTH)
# else
# define RWXALL (RWALL | S_IXUSR)
# endif
#else
# define RWALL	0666
# define RWXALL	0777
#endif

static int
md (const char *s)
{
#if defined PFE_MKDIR_TAKES_ONE_ARG
    return mkdir ((char*)(s));
#else
    return mkdir (s, RWXALL);
#endif
}

static int
touch (const char *s)
{
    int result;

#ifdef PFE_HAVE_ACCESS
    if (_P4_access (s, F_OK) == 0)
        return utime (s, NULL);
#endif
    result = open (s, O_WRONLY | O_CREAT, RWALL);
    if (result < 0)
        return result;
    close (result);
    return 0;
}

static int
cp (const char *src, const char *dst)
{
    return p4_file_copy (src, dst, P4_OFF_T_MAX) == -1;
}

#ifndef NO_SYSTEM
static int
ls (const char *p)
{
    FX (p4_cr);
    return p4_systemf (PFE_LSCMD" %s", p);
}
static int
ll (const char *p)
{
    FX (p4_cr);
    return p4_systemf (PFE_LLCMD" %s", p);
}
#define GOT_DIR
#elif defined PFE_HAVE_DIRENT_H
static int
ls (const char* p)
{
    DIR* dir;
    struct dirent* dirent;
    FX (p4_cr);

    dir = opendir (p);
    if (!dir) return -1;

    while ((dirent=readdir(dir)))
    {
        if (dirent->d_name[0] == '.') continue;
        p4_type_on_line (dirent->d_name, p4_strlen(dirent->d_name));
        FX (p4_space);
    }
    return closedir (dir);
}

static int
ll (const char* p)
{
    DIR* dir;
    struct dirent* dirent;
    struct stat st;
    struct tm tm;
    char buf[255];

    FX (p4_cr);
    FX (p4_start_Q_cr);

    dir = opendir (p);
    if (!dir) return -1;

    while ((dirent=readdir(dir)))
    {
        p4_strncpy (buf, p, 255);
        p4_strncat (buf, "/", 255);
        p4_strncat (buf, dirent->d_name, 255);
        stat (buf, &st);
        p4_memcpy (&tm, localtime (&st.st_mtime), sizeof(struct tm));

        if (S_ISREG (st.st_mode))
        {
            p4_outf ("%8i  %2i-%02i-%04i %2i:%02i:%02i  %s", st.st_size,
              tm.tm_mday, tm.tm_mon+1, tm.tm_year+1900,
              tm.tm_hour, tm.tm_min, tm.tm_sec,
              dirent->d_name);
        } else if (S_ISDIR (st.st_mode))
        {
            p4_outf ("DIRECTORY %2i-%02i-%04i %2i:%02i:%02i  %s",
              tm.tm_mday, tm.tm_mon+1, tm.tm_year+1900,
              tm.tm_hour, tm.tm_min, tm.tm_sec,
              dirent->d_name);
        } else {
            p4_outf ("SPECIAL   %2i-%02i-%04i %2i:%02i:%02i  %s",
              tm.tm_mday, tm.tm_mon+1, tm.tm_year+1900,
              tm.tm_hour, tm.tm_min, tm.tm_sec,
              dirent->d_name);
        }

        if (p4_Q_cr())
            break;
    }
    return closedir (dir);
}
#define GOT_DIR
#endif

#ifndef NO_SYSTEM
static int
mv (const char *p1, const char* p2)
{
    FX (p4_cr);
    return p4_systemf ("mv %s %s", p1, p2);
}
#endif /* NO_SYSTEM */

#if !defined PFE_HAVE_REMOVE
# ifdef remove
# undef remove
# endif
/*
 * For the macro SHWORD1 to work, it is required that remove is not a
 * macro. If this system lacks remove() and this is normally fixed by
 * #define remove unlink, then #undef remove and implement remove as
 * a valid function here to whatever has been detected as a replacement.
 */
static int remove (const char *name) { return _pfe_remove (name); }
#endif

#if defined __target_os_aix1
extern int link();
extern int remove();
extern int chdir();
extern int rmdir();
#endif

SHWORD1(remove);
SHWORD1(touch);
SHWORD1(rmdir);
SHWORD1(md);
#ifdef GOT_DIR
SHWORD1(ls);
SHWORD1(ll);
#endif
#ifndef NO_SYSTEM
SHWORD2(mv);
#endif /* NO_SYSTEM */
SHWORD2(cp);
#ifdef PFE_HAVE_LINK
SHWORD2(link);
#endif

P4_LISTWORDSET (shell) [] =
{
    P4_INTO ("EXTENSIONS", 0),
#ifdef PFE_HAVE_PID
    P4_FXco ("$PID",		p4_getpid),
#endif
#ifdef PFE_HAVE_UID
    P4_FXco ("$UID",		p4_getuid),
    P4_FXco ("$EUID",		p4_geteuid),
#endif
#ifdef PFE_HAVE_GID
    P4_FXco ("$GID",		p4_getgid),
#endif
#ifdef PFE_HAVE_UMASK
    P4_FXco ("UMASK",		p4_umask),
#endif
    P4_FXco ("$HOME",		p4_home),
    P4_FXco ("$USER",		p4_user),
    P4_FXco ("$CWD",		p4_cwd),
    P4_FXco ("PWD",		p4_pwd),
    /** mimics a unix'ish shell-command - =>'PARSE's one filename/dirname */
    P4_SXco ("RM",		p4_remove),
    P4_SXco ("TOUCH",		p4_touch),
    P4_FXco ("CHDIR",		p4_chdir),
    P4_SXco ("RMDIR",		p4_rmdir),
    P4_SXco ("MKDIR",		p4_md),
#ifdef PFE_HAVE_LINK
    /** mimics a unix'ish shell-command - =>'PARSE's two filenames/dirnames */
    P4_SXco ("LN",		p4_link),
#endif /* PFE_HAVE_LINK */
#ifndef NO_SYSTEM
    /** will invoke a shell-command with the command and a two filenames */
    P4_SXco ("MV",		p4_mv),
#endif
#ifdef GOT_DIR
    /** mimics a unix'ish shell-command - =>'PARSE's one filename/dirname */
    P4_SXco ("LL",		p4_ll),
    P4_SXco ("LS",		p4_ls),
#endif /* NO_SYSTEM */
    /** mimics a unix'ish shell-command - =>'PARSE's two filenames/dirname */
    P4_SXco ("CP",		p4_cp),
};
P4_COUNTWORDSET (shell, "Shell like words");

/*@}*/
