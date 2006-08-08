#ifndef __PFE_DEF_CONFIG_H
#define __PFE_DEF_CONFIG_H "%full_filespec: def-config.h~bln_mpt1!5.26:incl:bln_12xx!1 %"

/* include the configure generated headers */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "pfe/_config.h"
#include "pfe/_target.h" 
#include <pfe/def-gcc.h>

/* try to detect errornous headers.. */

#if defined PFE_CONFIGVERSION && defined PFE_PFE_CONFIGVERSION
# if PFE_CONFIGVERSION != PFE_PFE_CONFIGVERSION
#  ifdef __GNUC__
#  warning "wrong pfe/_config.h included, version mismatch with local config.h, expect PROBLEMS"
#  warning "please check the gcc's include-defs (in Makefile) for the order of $builddir"
#  else
#  error   "wrong pfe/_config.h included, version mismatch with local config.h, check INCLUDE"
#  endif
# endif
#endif

/* post configure defines */
#ifndef _pfe_const
#define _pfe_const const
#endif

#ifndef _pfe_inline
#define _pfe_inline inline
#endif

#ifndef _pfe_restrict
#define _pfe_restrict restrict
#endif

#ifndef _pfe_off_t
#define _pfe_off_t off_t
#endif

/* library defines */

#ifndef _extern
#define _extern extern
#endif

#ifdef __WATCOMC__
#undef  _export
typedef int mode_t;
/* 106 = truncated constants */
/* 121 = text following pre-processor symbols is not standard C */
/* 201 = unreachable code */
#pragma disable_message (106, 121, 201)
#endif

#ifndef _export
  /* libtool 1.4 supports this in sharedlib ojbect builds */
# ifdef PFE_DLL_EXPORT
# define _export __dllexport
# else
# define _export
# endif
#endif

#ifndef __dllexport
#define __dllexport P4_GCC_DLLEXPORT
#endif

/* HOST_WIN32 detection */
#if defined __host_os_mingw || defined HOST_OS_MINGW 
#define HOST_WIN32
#endif
#if defined __WATCOMC__ && defined _WIN32
#define HOST_WIN32
#endif
#if defined __BORLANDC__ && defined _WIN32
#define HOST_WIN32
#endif

/* CELL type detection */
#if !defined PFE_SIZEOF_SHORT && defined SIZEOF_SHORT
#define PFE_SIZEOF_SHORT SIZEOF_SHORT
#endif
#if !defined PFE_SIZEOF_INT && defined SIZEOF_INT
#define PFE_SIZEOF_INT SIZEOF_INT
#endif
#if !defined PFE_SIZEOF_LONG && defined SIZEOF_LONG
#define PFE_SIZEOF_LONG SIZEOF_LONG
#endif

/* a cell has atleast the size of a pointer but the type of an integer */
#if !defined PFE_TYPEOF_CELL || !defined PFE_SIZEOF_CELL
# if defined PFE_SIZEOF_INT && PFE_SIZEOF_INT >= PFE_SIZEOF_VOIDP
# define       PFE_SIZEOF_CELL PFE_SIZEOF_INT
# define       PFE_TYPEOF_CELL int
# elif defined PFE_SIZEOF_LONG && PFE_SIZEOF_LONG >= PFE_SIZEOF_VOIDP
# define       PFE_SIZEOF_CELL PFE_SIZEOF_LONG
# define       PFE_TYPEOF_CELL long
# else 
# error cell type and size not detected.
# endif
#endif

#if !defined PFE_TYPEOF_HALFCELL
# if defined PFE_SIZEOF_INT && PFE_SIZEOF_INT == PFE_SIZEOF_CELL / 2 
# define       PFE_TYPEOF_HALFCELL int
# elif defined PFE_SIZEOF_SHORT && PFE_SIZEOF_SHORT == PFE_SIZEOF_CELL / 2 
# define       PFE_TYPEOF_HALFCELL short
# else
# error halfcell type not detected
# endif
#endif

#ifndef PFE_ALIGNOF_CELL
#define PFE_ALIGNOF_CELL PFE_SIZEOF_INT
#endif
#ifndef PFE_ALIGNOF_SFLOAT
#define PFE_ALIGNOF_SFLOAT PFE_SIZEOF_FLOAT
#endif
#ifndef PFE_ALIGNOF_DFLOAT
#define PFE_ALIGNOF_DFLOAT PFE_SIZEOF_DOUBLE
#endif

/* suspend problems with important defines from pfe/_config.h */

#ifndef PFE_BYTEORDER
# if defined WORDS_BIGENDIAN
# define PFE_BYTEORDER 4321
# elif defined BYTEORDER
# define PFE_BYTEORDER BYTEORDER
# else
#  ifdef __GNUC__
#  warning no byteorder defined, assuming little-endian
#  define PFE_BYTEORDER 1234
#  else
#  error no byteorder defined, define BYTEORDER or PFE_BYTEORDER
#  endif
# endif
#endif

#ifndef PFE_PATH_STYLE
# if     defined DOS_FILENAMES
# define PFE_PATH_STYLE 16
# elif   defined UNIX_FILENAMES
# define PFE_PATH_STYLE 32
# elif   defined URL_FILENAMES
# define PFE_PATH_STYLE 64
# else
#  if defined HAVE_LONG_FILE_NAMES || defined PFE_HAVE_LONG_FILE_NAMES
#  define PFE_PATH_STYLE 32
#  else
#   ifdef __GNUC__
#   warning no path_style defined, assuming unix-style
#   define PFE_PATH_STYLE 32
#   else
#   error could not detect path style
#   endif
#  endif
# endif
#endif

#ifndef PFE_PACKAGE 
#define PFE_PACKAGE "pfe"
#endif

#ifndef PFE_VERSION
#define PFE_VERSION "0.33.x"
#endif

#if !defined PFE_HAVE_STRINGIZE && !defined PFE_OLD_STRINGIZE
#define PFE_HAVE_STRINGIZE 1   /* just make it the default */
#endif 

/* OS specific */
#if defined HAVE_VXWORKS_H || defined PFE_HAVE_VXWORKS_H
# define NO_SYSTEM
# define STATIC_MAIN
# ifndef __vxworks
# define __vxworks 1
# endif
# ifndef VXWORKS
# define VXWORKS 1
# endif
# ifndef VxWorks
# define VxWorks 1
# endif
#endif

#if defined _K12_SOURCE
# ifndef PFE_WITH_SPY   /* use always in k12xx environment */
# define PFE_WITH_SPY 1
# endif
#endif

/* some options */
#ifndef P4_KB           /* size of system altogether in KBytes */
#define P4_KB           1024
#endif

#if !defined P4_C_QUOTE && !defined P4_S_QUOTE
#define P4_C_QUOTE 1            /* quote means c_quote */
#endif

#ifdef PFE_WITH_FIG
#define PFE_WITH_NO_FFA 1
#endif

#if !defined PFE_WITH_FFA && !defined PFE_WITH_NO_FFA
#define PFE_WITH_FFA    1       /* use seperate FlagField */
#endif

#ifndef P4_MODULES
#define P4_MODULES 1            /* use shared object modules */
#endif                  
#ifndef P4_STDC
#define P4_STDC 1               /* some words from the C-language family */
#endif

#ifndef PFE_USE_QUOTED_PARSE
# if defined _K12_SOURCE || defined PFE_WITH_FIG
# define PFE_USE_QUOTED_PARSE 0
# else
# define PFE_USE_QUOTED_PARSE 1
# endif
#endif

#if defined PFE_HAVE_FTELLO && defined PFE_HAVE_FSEEKO
#define PFE_USE_FSEEKO
#define _p4_ftello ftello
#define _p4_fseeko fseeko
#define _p4_off_t  _pfe_off_t
#else
#define _p4_ftello ftell
#define _p4_fseeko fseek
#define _p4_off_t  long
#endif

/* A glibc bug: fseeko is only prototyped when UNIX98 or LFS or GNU_SOURCE set.
 * The gcc will warn about the missing prototype, but link to the existing
 * fseeko in the libc. However, in a largefile-sensitive system it should be
 * linked to fseeko64 instead, otherwise we get a nice callframe mismatch!
 */
#if defined PFE_USE_FSEEKO
#ifndef _LARGEFILE_SOURCE
#define _LARGEFILE_SOURCE 1 /* AC_SYS_LAREFILE does not set this on its own */
#endif
#endif

#ifndef P4_OFF_T_MAX
#define P4_OFF_T_MAX (((_p4_off_t)1) << (sizeof(_p4_off_t)*8-1))
#endif

# if defined PFE_WITH_FIG
# define PFE_NAY_SBR_THREADING
# endif

# if defined PFE_NAY_SBR_THREADING \
  || defined HOST_ARCH_SPARC || defined __target_arch_sparc 
# define PFE_NAY_SBR_ARG_THREADING
# endif


#ifdef PFE_WITH_SBR_THREADING
# ifdef PFE_NAY_SBR_ARG_THREADING
# undef PFE_SBR_CALL_ARG_ENABLED
# else
/*  note: PFE_SBR_CALL_ARG_THREADING should be used for #ifdefs */
#  ifndef PFE_SBR_CALL_ARG_PREFIXING
#  define PFE_SBR_CALL_ARG_PREFIXING 1
#  endif
#  ifndef PFE_SBR_CALL_THREADING
#  define PFE_SBR_CALL_THREADING 1
#  endif
#  ifndef PFE_CALL_THREADING
#  define PFE_CALL_THREADING 1
#  endif
# endif
#endif

#ifdef PFE_WITH_SBR_CALL_THREADING
# ifdef PFE_NAY_SBR_THREADING
# undef PFE_SBR_CALL_THREADING
# else
#  ifndef PFE_SBR_CALL_THREADING
#  define PFE_SBR_CALL_THREADING 1
#  endif
#  ifndef PFE_CALL_THREADING
#  define PFE_CALL_THREADING 1
#  endif
# endif
#endif

#ifdef PFE_WITH_CALL_THREADING
# ifdef PFE_WITH_FIG
# undef PFE_CALL_THREADING
# else
#  ifndef PFE_CALL_THREADING
#  define PFE_CALL_THREADING 1
#  endif
# endif
#endif

#endif
