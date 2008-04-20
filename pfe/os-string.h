#ifndef __PFE_OS_STRING_H
#define __PFE_OS_STRING_H
/**
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.4 $
 *     (modified $Date: 2008-04-20 04:46:29 $)
 *
 * GCC 3.x uses builtin string routines on i386 which is supported
 * by native opcodes of that processor architecture - however the
 * i386 ISA does restrict those instructions to very specific
 * registers. Almost all of the registers in the architecture do
 * have a special assignment to one of those special instructions
 * that are so much favorable - their execution is often highly
 * optimized in hardware. The problem: the gcc programmers are
 * able to push/pop those registers if they are loaded with
 * scratch values of the compiled function but they are not able
 * to temporarily push/pop the global register assignments that
 * we are using for the forth virtual machine registers.
 * <p>
 * As time passed by, the gcc developers were introducing more and
 * more support to use hardware instruction opcodes in the backend
 * of the compiler which has led us to reduce the number of global
 * registers alloctions on the i386 architecture. Finally, the
 * remaining set of usuable registers for the forth virtual machine
 * has gone down to just one being used for the pfe thread poiner of
 * course. So, here we try to start over with another approach in
 * that we override the builtin opcodes with function calls which
 * is about the same what gcc has been compiling before the builtin
 * string operation optimizations came about.
 * <p>
 * Of course that leads to suboptimal code and we are still in the
 * hope that the gcc developers are able to fix their problems with
 * temporary push/pop operations of global register assignments. The
 * global register assignments is a gcc special feature that has been
 * used by many other developers of virtual machines. At the moment
 * the compiler will error out if one of the special registers is being
 * allocated with as message like "can not find spill registers" or 
 * something like that.
 * <p>
 * In the first instance of this code we were simply re-#define-ing
 * the memcpy/strlen builtin functions but as far as I have heard
 * the gcc 3.4.x will contain code to protect some function calls
 * from being deliberatly overridden. Therefore, we work it out the
 * other way round: all usage of memcpy/strlen must be changed into
 * the usage of a local definition p4_memcpy/p4_strlen that may or
 * may not be a shallow redefinition using the native memcpy/strlen
 * instead of the definition in the accompinying ".c" source file.
 */

#include <pfe/def-config.h>
#include <string.h>

#if __GNUC__+0 == 3 && __GNUC_MINOR__ >= 3
#define PFE_GCC_OVEROPTIMIZES
#elif __GNUC__+0 > 3
#define PFE_GCC_OVEROPTIMIZES
#endif

#ifndef PFE_OVERRIDE_STRING_H
# if defined PFE_GCC_OVEROPTIMIZES && defined __i386__
# define PFE_OVERRIDE_STRING_H 1
# else 
# define PFE_OVERRIDE_STRING_H 0
# endif
#endif

#include <string.h>

#if defined PFE_HAVE_STRINGS_H
# include <strings.h>	/* place where old BSD systems keep bcopy() */
#endif

#if defined PFE_HAVE_MEM_H
# include <mem.h>	/* some systems have memcpy() here */
#endif

#if defined PFE_HAVE_MEMORY_H
# include <memory.h>	/* or here */
#endif

#  if PFE_OVERRIDE_STRING_H+0
#include <stddef.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#  if PFE_OVERRIDE_STRING_H+0 > 0
extern size_t p4_strlen(const char *s);
extern void * p4_memcpy(void *dest, const void *src, size_t n);
extern void * p4_memset(void *s, int c, size_t n);
extern char * p4_strchr(const char *s, int c);
#  else
#define p4_strlen strlen
#define p4_memcpy memcpy
#define p4_memset memset
#define p4_strchr strchr
#  endif

#define p4_memcmp  memcmp
#define p4_memchr  memchr
#define p4_strcmp  strcmp
#define p4_strstr  strstr
#define p4_strcat  strcat
#define p4_strcpy  strcpy
#define p4_strncpy strncpy
#define p4_strncat strncat

#if defined PFE_HAVE_MEMMOVE
#define p4_memmove memmove
#elif defined HAVE_BCOPY || defined PFE_HAVE_BCOPY
#define p4_memmove(D,S,C) bcopy (S,D,C)
#else
#define PFE_HAVE_P4_MEMMOVE 1
extern void p4_memmove (char*, const char*, size_t n);
#endif

#define p4_memequal !p4_memcmp
#define p4_strequal !p4_strcmp

#  if defined PFE_HAVE_STRNCASECMP 
#define p4_strncmpi strncasecmp
#  elif defined PFE_HAVE_STRNICMP
#define p4_strncmpi strnicmp
#  elif defined strncmpi
/*  cygwin32 and borland and probably other win32 platforms */
#define p4_strncmpi strncmpi
#  else
extern int p4_strncmpi (const char *s1, const char* s2, size_t n); 
#  endif

#  if defined PFE_HAVE_STRLCAT
#define p4_strlcat strlcat
#  else
extern size_t p4_strlcat (char *dest, const char* src, size_t n);
#  endif

static inline int p4_strncmp(const char* a, const char* b, int c) {
    return strncmp(a,b,c);
}

#ifdef __cplusplus
} /* extern "C" */
#endif

/* once */
#endif
