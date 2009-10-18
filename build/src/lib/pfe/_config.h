#ifndef __CONFIG_H
#define __CONFIG_H 1
 
/* _config.h. Generated automatically at end of configure. */
/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* 1234 = LIL_ENDIAN, 4321 = BIGENDIAN */
#ifndef PFE_BYTEORDER 
#define PFE_BYTEORDER  1234 
#endif

/* set to { if variable declarations need a block start before */
/* #undef DECLARE_BLOCK */

/* if C variables must be declared at the beginning of a block */
/* #undef DECLARE_BLOCK_NEEDED */

/* set to } if variable declarations need a block start before */
/* #undef DECLARE_END */

/* whether --enable-shared was used during setup */
#ifndef PFE_ENABLE_SHARED 
#define PFE_ENABLE_SHARED  1 
#endif

/* --exec-prefix or its default */
#ifndef PFE_EPREFIX 
#define PFE_EPREFIX  "/usr/local" 
#endif

/* Define to 1 if you have the `access' function. */
#ifndef PFE_HAVE_ACCESS 
#define PFE_HAVE_ACCESS  1 
#endif

/* Define to 1 if you have the `acosh' function. */
#ifndef PFE_HAVE_ACOSH 
#define PFE_HAVE_ACOSH  1 
#endif

/* Define to 1 if you have the `alarm' function. */
#ifndef PFE_HAVE_ALARM 
#define PFE_HAVE_ALARM  1 
#endif

/* Define to 1 if you have the <conio.h> header file. */
/* #undef HAVE_CONIO_H */

/* Define to 1 if you have the <curses.h> header file. */
#ifndef PFE_HAVE_CURSES_H 
#define PFE_HAVE_CURSES_H  1 
#endif

/* Define to 1 if you have the declaration of `sys_siglist', and to 0 if you
   don't. */
#ifndef PFE_HAVE_DECL_SYS_SIGLIST 
#define PFE_HAVE_DECL_SYS_SIGLIST  1 
#endif

/* Define to 1 if you have the `delay' function. */
/* #undef HAVE_DELAY */

/* Define to 1 if you have the <dirent.h> header file, and it defines `DIR'.
   */
#ifndef PFE_HAVE_DIRENT_H 
#define PFE_HAVE_DIRENT_H  1 
#endif

/* Define to 1 if you have the <dlfcn.h> header file. */
#ifndef PFE_HAVE_DLFCN_H 
#define PFE_HAVE_DLFCN_H  1 
#endif

/* Define to 1 if you have the <dl.h> header file. */
/* #undef HAVE_DL_H */

/* Define to 1 if you have the <dos.h> header file. */
/* #undef HAVE_DOS_H */

/* Define to 1 if you have the <fcntl.h> header file. */
#ifndef PFE_HAVE_FCNTL_H 
#define PFE_HAVE_FCNTL_H  1 
#endif

/* Define to 1 if you have the <fenv.h> header file. */
#ifndef PFE_HAVE_FENV_H 
#define PFE_HAVE_FENV_H  1 
#endif

/* Define to 1 if you have the `fileno' function. */
#ifndef PFE_HAVE_FILENO 
#define PFE_HAVE_FILENO  1 
#endif

/* Define to 1 if you have the `fseeko' function. */
#ifndef PFE_HAVE_FSEEKO 
#define PFE_HAVE_FSEEKO  1 
#endif

/* Define to 1 if you have the `fstat' function. */
#ifndef PFE_HAVE_FSTAT 
#define PFE_HAVE_FSTAT  1 
#endif

/* Define to 1 if you have the `ftello' function. */
#ifndef PFE_HAVE_FTELLO 
#define PFE_HAVE_FTELLO  1 
#endif

/* Define to 1 if you have the `getcwd' function. */
#ifndef PFE_HAVE_GETCWD 
#define PFE_HAVE_GETCWD  1 
#endif

/* Define to 1 if you have the `getgid' function. */
#ifndef PFE_HAVE_GETGID 
#define PFE_HAVE_GETGID  1 
#endif

/* Define to 1 if you have the `getpid' function. */
#ifndef PFE_HAVE_GETPID 
#define PFE_HAVE_GETPID  1 
#endif

/* Define to 1 if you have the `getuid' function. */
#ifndef PFE_HAVE_GETUID 
#define PFE_HAVE_GETUID  1 
#endif

/* gnuish system might have a dladdr in dlfcn.h */
#ifndef PFE_HAVE_GNU_DLADDR 
#define PFE_HAVE_GNU_DLADDR  1 
#endif

/* Define to 1 if you have the <inttypes.h> header file. */
#ifndef PFE_HAVE_INTTYPES_H 
#define PFE_HAVE_INTTYPES_H  1 
#endif

/* Define to 1 if you have the <io.h> header file. */
/* #undef HAVE_IO_H */

/* Define to 1 if you have the `isinf' function. */
#ifndef PFE_HAVE_ISINF 
#define PFE_HAVE_ISINF  1 
#endif

/* Define to 1 if you have the <libc.h> header file. */
/* #undef HAVE_LIBC_H */

/* Define to 1 if you have the `dl' library (-ldl). */
#ifndef PFE_HAVE_LIBDL 
#define PFE_HAVE_LIBDL  1 
#endif

/* Define to 1 if you have the `dld' library (-ldld). */
/* #undef HAVE_LIBDLD */

/* Define to 1 if you have the `m' library (-lm). */
#ifndef PFE_HAVE_LIBM 
#define PFE_HAVE_LIBM  1 
#endif

/* Define to 1 if you have the `link' function. */
#ifndef PFE_HAVE_LINK 
#define PFE_HAVE_LINK  1 
#endif

/* Define to 1 if you have the <locale.h> header file. */
#ifndef PFE_HAVE_LOCALE_H 
#define PFE_HAVE_LOCALE_H  1 
#endif

/* compiler understands long long */
#ifndef PFE_HAVE_LONG_LONG 
#define PFE_HAVE_LONG_LONG  1 
#endif

/* Define to 1 if you have the <ltdl.h> header file. */
#ifndef PFE_HAVE_LTDL_H 
#define PFE_HAVE_LTDL_H  1 
#endif

/* Define to 1 if you have the <math.h> header file. */
#ifndef PFE_HAVE_MATH_H 
#define PFE_HAVE_MATH_H  1 
#endif

/* memmove */
#ifndef PFE_HAVE_MEMMOVE 
#define PFE_HAVE_MEMMOVE  /**/ 
#endif

/* Define to 1 if you have the <memory.h> header file. */
#ifndef PFE_HAVE_MEMORY_H 
#define PFE_HAVE_MEMORY_H  1 
#endif

/* Define to 1 if you have the <mem.h> header file. */
/* #undef HAVE_MEM_H */

/* Define to 1 if you have the `mkdir' function. */
#ifndef PFE_HAVE_MKDIR 
#define PFE_HAVE_MKDIR  1 
#endif

/* Define to 1 if you have the <ncurses.h> header file. */
#ifndef PFE_HAVE_NCURSES_H 
#define PFE_HAVE_NCURSES_H  1 
#endif

/* Define to 1 if you have the <ndir.h> header file, and it defines `DIR'. */
/* #undef HAVE_NDIR_H */

/* Define to 1 if you have the `ospeed' function. */
/* #undef HAVE_OSPEED */

/* Define to 1 if you have the <poll.h> header file. */
#ifndef PFE_HAVE_POLL_H 
#define PFE_HAVE_POLL_H  1 
#endif

/* Define to 1 if you have the `pow10' function. */
#ifndef PFE_HAVE_POW10 
#define PFE_HAVE_POW10  1 
#endif

/* Define to 1 if you have the <process.h> header file. */
/* #undef HAVE_PROCESS_H */

/* Define to 1 if you have the <pthread.h> header file. */
#ifndef PFE_HAVE_PTHREAD_H 
#define PFE_HAVE_PTHREAD_H  1 
#endif

/* Define to 1 if you have the <pth.h> header file. */
#ifndef PFE_HAVE_PTH_H 
#define PFE_HAVE_PTH_H  1 
#endif

/* Define to 1 if you have the <pwd.h> header file. */
#ifndef PFE_HAVE_PWD_H 
#define PFE_HAVE_PWD_H  1 
#endif

/* Define to 1 if you have the `raise' function. */
#ifndef PFE_HAVE_RAISE 
#define PFE_HAVE_RAISE  1 
#endif

/* Define to 1 if you have the `random' function. */
#ifndef PFE_HAVE_RANDOM 
#define PFE_HAVE_RANDOM  1 
#endif

/* Define to 1 if you have the `remove' function. */
#ifndef PFE_HAVE_REMOVE 
#define PFE_HAVE_REMOVE  1 
#endif

/* Define to 1 if you have the `rename' function. */
#ifndef PFE_HAVE_RENAME 
#define PFE_HAVE_RENAME  1 
#endif

/* Define to 1 if you have the `rint' function. */
#ifndef PFE_HAVE_RINT 
#define PFE_HAVE_RINT  1 
#endif

/* Define to 1 if you have the `select' function. */
#ifndef PFE_HAVE_SELECT 
#define PFE_HAVE_SELECT  1 
#endif

/* Define to 1 if you have the <sgtty.h> header file. */
#ifndef PFE_HAVE_SGTTY_H 
#define PFE_HAVE_SGTTY_H  1 
#endif

/* Define to 1 if you have the `signbit' function. */
/* #undef HAVE_SIGNBIT */

/* sigsetjmp in setjmp.h */
#ifndef PFE_HAVE_SIGSETJMP 
#define PFE_HAVE_SIGSETJMP  /**/ 
#endif

/* Define to 1 if you have the `snprintf' function. */
#ifndef PFE_HAVE_SNPRINTF 
#define PFE_HAVE_SNPRINTF  1 
#endif

/* Define to 1 if you have the `stat' function. */
#ifndef PFE_HAVE_STAT 
#define PFE_HAVE_STAT  1 
#endif

/* Define to 1 if you have the <stdint.h> header file. */
#ifndef PFE_HAVE_STDINT_H 
#define PFE_HAVE_STDINT_H  1 
#endif

/* Define to 1 if you have the <stdlib.h> header file. */
#ifndef PFE_HAVE_STDLIB_H 
#define PFE_HAVE_STDLIB_H  1 
#endif

/* Define to 1 if you have the `strerror' function. */
#ifndef PFE_HAVE_STRERROR 
#define PFE_HAVE_STRERROR  1 
#endif

/* Define to 1 if cpp supports the ANSI # stringizing operator. */
#ifndef PFE_HAVE_STRINGIZE 
#define PFE_HAVE_STRINGIZE  1 
#endif

/* Define to 1 if you have the <strings.h> header file. */
#ifndef PFE_HAVE_STRINGS_H 
#define PFE_HAVE_STRINGS_H  1 
#endif

/* Define to 1 if you have the <string.h> header file. */
#ifndef PFE_HAVE_STRING_H 
#define PFE_HAVE_STRING_H  1 
#endif

/* Define to 1 if you have the `strlcat' function. */
/* #undef HAVE_STRLCAT */

/* Define to 1 if you have the `strlcpy' function. */
/* #undef HAVE_STRLCPY */

/* Define to 1 if you have the `strncasecmp' function. */
#ifndef PFE_HAVE_STRNCASECMP 
#define PFE_HAVE_STRNCASECMP  1 
#endif

/* Define to 1 if you have the `strnicmp' function. */
/* #undef HAVE_STRNICMP */

/* Define to 1 if you have the `system' function. */
#ifndef PFE_HAVE_SYSTEM 
#define PFE_HAVE_SYSTEM  1 
#endif

/* Define to 1 if you have the <sys/dir.h> header file, and it defines `DIR'.
   */
/* #undef HAVE_SYS_DIR_H */

/* Define to 1 if you have the <sys/ioctl.h> header file. */
#ifndef PFE_HAVE_SYS_IOCTL_H 
#define PFE_HAVE_SYS_IOCTL_H  1 
#endif

/* Define to 1 if you have the <sys/ndir.h> header file, and it defines `DIR'.
   */
/* #undef HAVE_SYS_NDIR_H */

/* Define to 1 if you have the <sys/poll.h> header file. */
#ifndef PFE_HAVE_SYS_POLL_H 
#define PFE_HAVE_SYS_POLL_H  1 
#endif

/* Define to 1 if you have the <sys/process.h> header file. */
/* #undef HAVE_SYS_PROCESS_H */

/* Define to 1 if you have the <sys/resource.h> header file. */
#ifndef PFE_HAVE_SYS_RESOURCE_H 
#define PFE_HAVE_SYS_RESOURCE_H  1 
#endif

/* Define to 1 if you have the <sys/select.h> header file. */
#ifndef PFE_HAVE_SYS_SELECT_H 
#define PFE_HAVE_SYS_SELECT_H  1 
#endif

/* Define to 1 if you have the <sys/stat.h> header file. */
#ifndef PFE_HAVE_SYS_STAT_H 
#define PFE_HAVE_SYS_STAT_H  1 
#endif

/* Define to 1 if you have the <sys/time.h> header file. */
#ifndef PFE_HAVE_SYS_TIME_H 
#define PFE_HAVE_SYS_TIME_H  1 
#endif

/* Define to 1 if you have the <sys/types.h> header file. */
#ifndef PFE_HAVE_SYS_TYPES_H 
#define PFE_HAVE_SYS_TYPES_H  1 
#endif

/* Define to 1 if you have the <sys/utime.h> header file. */
/* #undef HAVE_SYS_UTIME_H */

/* Define to 1 if you have the <termcap.h> header file. */
#ifndef PFE_HAVE_TERMCAP_H 
#define PFE_HAVE_TERMCAP_H  1 
#endif

/* Define to 1 if you have the <termios.h> header file. */
#ifndef PFE_HAVE_TERMIOS_H 
#define PFE_HAVE_TERMIOS_H  1 
#endif

/* Define to 1 if you have the <termio.h> header file. */
#ifndef PFE_HAVE_TERMIO_H 
#define PFE_HAVE_TERMIO_H  1 
#endif

/* Define to 1 if you have the <term.h> header file. */
#ifndef PFE_HAVE_TERM_H 
#define PFE_HAVE_TERM_H  1 
#endif

/* whether operating system is assumed to support tls registers */
/* #undef HAVE_TLS_SUPPORT */

/* Define to 1 if you have the `truncate' function. */
#ifndef PFE_HAVE_TRUNCATE 
#define PFE_HAVE_TRUNCATE  1 
#endif

/* Define to 1 if you have the `umask' function. */
#ifndef PFE_HAVE_UMASK 
#define PFE_HAVE_UMASK  1 
#endif

/* Define to 1 if you have the <unistd.h> header file. */
#ifndef PFE_HAVE_UNISTD_H 
#define PFE_HAVE_UNISTD_H  1 
#endif

/* Define to 1 if you have the `usleep' function. */
#ifndef PFE_HAVE_USLEEP 
#define PFE_HAVE_USLEEP  1 
#endif

/* Define to 1 if you have the <utime.h> header file. */
#ifndef PFE_HAVE_UTIME_H 
#define PFE_HAVE_UTIME_H  1 
#endif

/* Define to 1 if you have the `vsnprintf' function. */
#ifndef PFE_HAVE_VSNPRINTF 
#define PFE_HAVE_VSNPRINTF  1 
#endif

/* Define to 1 if you have the <vxWorks.h> header file. */
/* #undef HAVE_VXWORKS_H */

/* Define to 1 if you have the <winbase.h> header file. */
/* #undef HAVE_WINBASE_H */

/* Define to 1 if you have the <wincon.h> header file. */
/* #undef HAVE_WINCON_H */

/* Define to 1 if you have the `_mkdir' function. */
/* #undef HAVE__MKDIR */

/* Header Files of package */
#ifndef PFE_INCLUDEDIR 
#define PFE_INCLUDEDIR  "/usr/local/include" 
#endif

/* whether the system defaults to 32bit off_t but can do 64bit when requested
   */
#ifndef PFE_LARGEFILE_SENSITIVE 
#define PFE_LARGEFILE_SENSITIVE  1 
#endif

/* --libdir or its default */
#ifndef PFE_LIBDIR 
#define PFE_LIBDIR  "/usr/local/lib" 
#endif

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#ifndef PFE_LT_OBJDIR 
#define PFE_LT_OBJDIR  ".libs/" 
#endif

/* Define if mkdir takes only one argument. */
/* #undef MKDIR_TAKES_ONE_ARG */

/* Name of package */
#ifndef PFE_PACKAGE 
#define PFE_PACKAGE  "pfe" 
#endif

/* Define to the address where bug reports for this package should be sent. */
#ifndef PFE_PACKAGE_BUGREPORT 
#define PFE_PACKAGE_BUGREPORT  "" 
#endif

/* Define to the full name of this package. */
#ifndef PFE_PACKAGE_NAME 
#define PFE_PACKAGE_NAME  "" 
#endif

/* Define to the full name and version of this package. */
#ifndef PFE_PACKAGE_STRING 
#define PFE_PACKAGE_STRING  "" 
#endif

/* Define to the one symbol short name of this package. */
#ifndef PFE_PACKAGE_TARNAME 
#define PFE_PACKAGE_TARNAME  "" 
#endif

/* Define to the version of this package. */
#ifndef PFE_PACKAGE_VERSION 
#define PFE_PACKAGE_VERSION  "" 
#endif

/* path style 16=dos 32=unix 64=url 128=mac */
#ifndef PFE_PATH_STYLE 
#define PFE_PATH_STYLE  32 
#endif

/* $PACKAGE versionlevel */
#ifndef PFE_PFE_CONFIGVERSION 
#define PFE_PFE_CONFIGVERSION  340001 
#endif

/* "enable replacement (v)snprintf if system (v)snprintf is broken" */
/* #undef PREFER_PORTABLE_SNPRINTF */

/* Define as the return type of signal handlers (`int' or `void'). */
#ifndef PFE_RETSIGTYPE 
#define PFE_RETSIGTYPE  void 
#endif

/* The number of bytes in type char */
#ifndef PFE_SIZEOF_CHAR 
#define PFE_SIZEOF_CHAR  1 
#endif

/* The number of bytes in type double */
#ifndef PFE_SIZEOF_DOUBLE 
#define PFE_SIZEOF_DOUBLE  8 
#endif

/* The number of bytes in type float */
#ifndef PFE_SIZEOF_FLOAT 
#define PFE_SIZEOF_FLOAT  4 
#endif

/* The number of bytes in type int */
#ifndef PFE_SIZEOF_INT 
#define PFE_SIZEOF_INT  4 
#endif

/* The number of bytes in type long */
#ifndef PFE_SIZEOF_LONG 
#define PFE_SIZEOF_LONG  8 
#endif

/* The number of bytes in type short */
#ifndef PFE_SIZEOF_SHORT 
#define PFE_SIZEOF_SHORT  2 
#endif

/* The number of bytes in type unsigned */
#ifndef PFE_SIZEOF_UNSIGNED 
#define PFE_SIZEOF_UNSIGNED  4 
#endif

/* The number of bytes in type void* */
#ifndef PFE_SIZEOF_VOIDP 
#define PFE_SIZEOF_VOIDP  8 
#endif

/* Define to 1 if you have the ANSI C header files. */
#ifndef PFE_STDC_HEADERS 
#define PFE_STDC_HEADERS  1 
#endif

/* which terminal driver to use as default */
#ifndef PFE_TERM_DEF 
#define PFE_TERM_DEF  6 
#endif

/* Variant suffix string for package */
#ifndef PFE_VARIANT 
#define PFE_VARIANT  "-forth" 
#endif

/* Version of package */
#ifndef PFE_VERSION 
#define PFE_VERSION  "0.34.01" 
#endif

/* whether call-threading feature is enabled */
/* #undef WITH_CALL_THREADING */

/* whether chains feature is enabled */
/* #undef WITH_CHAINS */

/* whether fig feature is enabled */
/* #undef WITH_FIG */

/* whether float feature is enabled */
#ifndef PFE_WITH_FLOAT 
#define PFE_WITH_FLOAT  1 
#endif

/* whether modules feature is enabled */
#ifndef PFE_WITH_MODULES 
#define PFE_WITH_MODULES  1 
#endif

/* whether nosmart feature is enabled */
/* #undef WITH_NOSMART */

/* whether no-completion feature is enabled */
/* #undef WITH_NO_COMPLETION */

/* "--with-regs=level" */
#ifndef PFE_WITH_REGS 
#define PFE_WITH_REGS  1 
#endif

/* whether sbr-call-threading feature is enabled */
/* #undef WITH_SBR_CALL_THREADING */

/* whether sbr-threading feature is enabled */
/* #undef WITH_SBR_THREADING */

/* whether spy feature is enabled */
/* #undef WITH_SPY */

/* whether static-dict feature is enabled */
/* #undef WITH_STATIC_DICT */

/* whether static-regs feature is enabled */
/* #undef WITH_STATIC_REGS */

/* whether byteorder is bigendian */
/* #undef WORDS_BIGENDIAN */

/* Number of bits in a file offset, on hosts where this is settable. */
/* #undef _FILE_OFFSET_BITS */

/* Define for large files, on AIX-style hosts. */
/* #undef _LARGE_FILES */

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
/* #undef inline */
#endif

/* Define to `long int' if <sys/types.h> does not define. */
/* #undef off_t */

/* Define to equivalent of C99 restrict keyword, or to nothing if this is not
   supported. Do not define if restrict is supported directly. */
/* #undef restrict */
 
/* once: __CONFIG_H */
#endif
