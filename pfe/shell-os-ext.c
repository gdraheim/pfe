/** 
 * -- os-like / shell-like commands for pfe
 * 
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE         @(#) %derived_by: guidod %
 *  @version %version: 5.13 %
 *    (%date_modified: Mon Mar 12 10:32:40 2001 %)
 *
 *  @description
 *        These builtin words are modelled after common shell commands,
 *        so that the Portable Forth Environment can often 
 *        be put in the place of a normal OS shell.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char * id __attribute__((unused)) = 
"@(#) $Id: shell-os-ext.c,v 0.30 2001-03-12 09:32:40 guidod Exp $";
#endif
                  
#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-xtra.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#if defined NO_SYSTEM && defined HAVE_DIRENT_H
#include <dirent.h>
#include <sys/stat.h>
#endif

#ifdef PFE_HAVE_VXWORKS_H
#include <ioLib.h> /* chdir */
#endif

#include <pfe/def-comp.h>
#include <pfe/file-sub.h>
#include <pfe/_nonansi.h>
#include <pfe/_missing.h>

#if defined VxWorks
extern char* getwd ( char* pathname );
#endif
typedef int (*syscall_f)( const char* ); 
		/*GD* used in do_one, so we don't get warnings */

#ifdef HAVE_PID
/** $$ ( -- pid )
 * calls system's <c> getpid </c>
 * <small>(this function should better changes its name...)</name>
 */
FCode (p4_getpid)	{ *--SP = (p4cell)getpid (); }
#endif

#ifdef HAVE_UID
/** $UID ( -- val )
 * calls system's <c> getuid </c>
 */
FCode (p4_getuid)	{ *--SP = (p4cell)getuid (); }

/** $EUID ( -- val )
 * calls system's <c> geteuid </c>
 */
FCode (p4_geteuid)	{ *--SP = (p4cell)geteuid (); }
#endif

#ifdef HAVE_GID
/** $GID ( -- val )
 * calls system's <c> getgid </c>
 */
FCode (p4_getgid)	{ *--SP = (p4cell)getgid (); }
#endif

#ifdef HAVE_UMASK
/** UMASK ( val -- ret )
 * calls system's <c> umask </c>
 */
FCode (p4_umask)	{ *SP = (p4cell)umask (*SP); }
#endif

/** $HOME ( -- str-ptr str-len )
 * calls system's <c> getenv(HOME) </c>
 */
FCode (p4_home)	{ p4_strpush (getenv ("HOME")); }

/** $USER ( -- str-ptr str-len )
 * calls system's <c> getenv(USER) </c>
 */
FCode (p4_user)	{ p4_strpush (getenv ("USER")); }

/** $CWD ( -- str-ptr str-len )
 * calls system's <c> getcwd </c>
 */
FCode (p4_cwd)	{ p4_strpush (getcwd (p4_pocket (), PATH_LENGTH)); }

/** PWD ( -- )
 * calls system's <c> getcwd </c> and prints it to the screen
 : PWD  $CWD TYPE ;
 */
FCode (p4_pwd)
{
    p4_outs (getcwd (p4_pocket (), PATH_LENGTH));
    FX (p4_space);
}

static void
do_one (char *p, int (*syscall) (const char *))
{
    char* name = p4_pocket_filename (p + 1, (p4char)*p);
    if (syscall (name))
        p4_throws (FX_IOR, name, 0);
}

/** CHDIR ( bstring -- )
 * change the current directory. <br>
 * <small> (under VxWorks it is global!) </small>
 */
FCode (p4_chdir)
{
    char buf[PATH_MAX];

    chdir (p4_store_c_string (
                              (* (char**) SP) + 1,
                              (int) **(char**) SP,
                              buf,
                              PATH_MAX));
    FX_DROP;
}

#define SHWORD1(X)				\
FCode (P4CAT3 (p4_,X,_execution))		\
{						\
    do_one ((char *)IP, (syscall_f)X);		\
    FX_SKIP_STRING;				\
}						\
FCode (P4CAT(p4_,X))				\
{						\
    if (STATE)					\
    {						\
        FX_COMPILE1 (P4CAT(p4_,X));		\
        p4_word_comma (' ');			\
    }						\
    else					\
        do_one (p4_word (' '), (syscall_f)X);	\
}						\
P4COMPILES (P4CAT(p4_,X), P4CAT3 (p4_,X,_execution),	\
  P4_SKIPS_STRING, P4_DEFAULT_STYLE)

static void
do_two (char *p1, char *p2, int (*syscall) (const char *, const char *))
{
    char* nm1 = p4_pocket_filename (p1 + 1, *(p4char*)p1);
    char* nm2 = p4_pocket_filename (p2 + 1, *(p4char*)p2);
    if (syscall (nm1, nm2))
        p4_throws (FX_IOR, nm1, 0);
}

#define SHWORD2(X)				\
FCode (P4CAT3 (p4_,X,_execution))		\
{						\
    char *p = (char *)IP;			\
    FX_SKIP_STRING;				\
    do_two (p, (char *)IP, X);			\
    FX_SKIP_STRING;				\
}						\
FCode (P4CAT(p4_,X))				\
{						\
    if (STATE)					\
    {						\
        FX_COMPILE1 (P4CAT(p4_,X));		\
        p4_word_comma (' ');			\
        p4_word_comma (' ');			\
    }else{                                      \
        char *p = p4_pocket ();			\
        strcpy (p, p4_word (' '));		\
        do_two (p, p4_word (' '), X);		\
    }						\
}						\
P4COMPILES (P4CAT(p4_,X), P4CAT3(p4_,X,_execution),	\
  P4_SKIPS_2STRINGS, P4_DEFAULT_STYLE)

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

/* vxworks, mingw, (msvc) don't want modbits at mkdir */
#ifdef PFE_HAVE_VXWORKS_H 
# ifndef PFE_MKDIR_TAKES_ONE_ARG
# define PFE_MKDIR_TAKES_ONE_ARG 1
# endif
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

#ifdef HAVE_ACCESS
    if (access (s, F_OK) == 0)
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
    return fn_copy (src, dst, LONG_MAX) == -1;
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
#else defined HAVE_DIRENT_H
static int
ls (const char* p)
{
    DIR* dir;
    struct dirent* dirent;
    FX (p4_cr);
  
#  ifdef VxWorks
    dir = opendir ((char*)p);   /* non-const char* in vxworks headers */
#  else
    dir = opendir (p);
#  endif
    if (!dir) return -1;
  
    while ((dirent=readdir(dir)))
    {
        if (dirent->d_name[0] == '.') continue;
        p4_type_on_line (dirent->d_name, strlen(dirent->d_name));
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


#  ifdef VxWorks
    dir = opendir ((char*)p);   /* non-const char* in vxworks headers */
#  else
    dir = opendir (p);
#  endif
    if (!dir) return -1;
    
    while ((dirent=readdir(dir)))
    {
        strncpy (buf, p, 255);
        strncat (buf, "/", 255);
        strncat (buf, dirent->d_name, 255);
        stat (buf, &st);
        memcpy (&tm, localtime (&st.st_mtime), sizeof(struct tm));
        
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
#ifdef HAVE_LINK
SHWORD2(link);
#endif

P4_LISTWORDS (shell) =
{
#ifdef HAVE_PID
    CO ("$$",		p4_getpid),
#endif
#ifdef HAVE_UID
    CO ("$UID",		p4_getuid),
    CO ("$EUID",	p4_geteuid),
#endif
#ifdef HAVE_GID
    CO ("$GID",		p4_getgid),
#endif
#ifdef HAVE_UMASK
    CO ("UMASK",	p4_umask),
#endif
    CO ("$HOME",	p4_home),
    CO ("$USER",	p4_user),
    CO ("$CWD",		p4_cwd),
    CO ("PWD",		p4_pwd),
    /** mimics a unix'ish shell-command - =>'PARSE's one filename/dirname */
    CS ("RM",		p4_remove),
    CS ("TOUCH",	p4_touch),
    CO ("CD",		p4_chdir),
    CO ("CHDIR",	p4_chdir),
    CS ("RMDIR",	p4_rmdir),
    CS ("MKDIR",	p4_md),
#ifdef HAVE_LINK
    /** mimics a unix'ish shell-command - =>'PARSE's two filenames/dirnames */
    CS ("LN",		p4_link),
#endif /* HAVE_LINK */
#ifndef NO_SYSTEM
    /** will invoke a shell-command with the command and a two filenames */
    CS ("MV",		p4_mv),
#endif
#ifdef GOT_DIR
    /** mimics a unix'ish shell-command - =>'PARSE's one filename/dirname */
    CS ("LL",		p4_ll),
    CS ("LS",		p4_ls),
#endif /* NO_SYSTEM */
    /** mimics a unix'ish shell-command - =>'PARSE's two filenames/dirname */
    CS ("CP",		p4_cp),
};
P4_COUNTWORDS (shell, "Shell like words");

/*@}*/

