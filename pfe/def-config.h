#ifndef __PFE_DEF_CONFIG_H
#define __PFE_DEF_CONFIG_H "%full_filespec: def-config.h~5.10:incl:bln_12xx!1 %"

/* include the configure generated headers */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "pfe-config.h"
#include "pfe-target.h"

/* try to detect errornous headers.. */

#if defined PFE_CONFIGVERSION && defined PFE_PFE_CONFIGVERSION
# if PFE_CONFIGVERSION != PFE_PFE_CONFIGVERSION
#  ifdef __GNUC__
#  warning "wrong pfe-config.h included, version mismatch with local config.h, expect PROBLEMS"
#  warning "please check the gcc's include-defs (in Makefile) for the order of $builddir"
#  else
#  error   "wrong pfe-config.h included, version mismatch with local config.h, check INCLUDE"
#  endif
# endif
#endif

/* library defines */

#ifndef _extern
#define _extern extern
#endif

#ifndef _export
#define _export
#endif

#ifndef __dllexport
#define __dllexport __attribute__((dllexport))
#endif

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

/* suspend problems with important defines from pfe-config.h */

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
#ifndef P4_KB		/* size of system altogether in KBytes */
#define P4_KB 		1024
#endif

#if !defined P4_C_QUOTE && !defined P4_S_QUOTE
#define P4_C_QUOTE 1		/* quote means c_quote */
#endif

#ifdef PFE_WITH_FIG
#define PFE_WITH_NO_FFA 1
#endif

#if !defined PFE_WITH_FFA && !defined PFE_WITH_NO_FFA
#define PFE_WITH_FFA 	1	/* use seperate FlagField */
#endif

#if !defined PFE_WITH_OPENBOOT && !defined PFE_WITH_NO_OPENBOOT
#define PFE_WITH_OPENBOOT 1
# if 0 /* there's a bug in the openstring routine */
# define P4_USE_openstring 1     /* openforth's semantics of quoted strings */
# endif
#endif

#ifndef P4_MODULES
#define P4_MODULES 1		/* use shared object modules */
#endif			
#ifndef P4_STDC
#define P4_STDC 1		/* some words from the C-language family */
#endif

#ifdef _P4_SOURCE
/* warpback section */
# ifdef PFE_WITH_FFA
# define USE_FFA PFE_WITH_FFA
# endif
# define SMART_WORDS 1
#endif

#endif
