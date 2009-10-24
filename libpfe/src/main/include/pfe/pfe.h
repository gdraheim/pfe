/*****************************************************************************/
/**
  * Important definitions for the PFE modules - use only for SWIG@PFE
  *
  * Copyright (C) Tektronix, Inc. 2000 - 2003. All Rights Reserved.
  *
  * This file is free software; you can redistribute it and/or
  * modify it under the terms of the GNU Library General Public
  * License as published by the Free Software Foundation; either
  * version 2 of the License, or (at your option) any later version.
  *
  * This library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  * Library General Public License for more details.
  *
  * You should have received a copy of the GNU Library General Public
  * License along with this library; if not, write to the
  * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
  * Boston, MA 02111-1307, USA.
  *
  * @(#) $Id$
  *
  * WARNING: This file is only used in connection with the swigpfe tool.
  *          The swigpfe tool is extension of SWIG (see http://swig.org).
  *          It takes C header files and generates a C source file that
  *          can be compiled into a PFE loadable module. This is used to
  *          interface to third party libraries through the declarations
  *          in their C header file. It wraps the C calls into valid PFE
  *          forth primitives that take/put parameters to the forth stack.
  *
  * To write a PFE native extension module, please read the documentation
  * in section "how to write a pfe module" and include "pfe/pfe-base.h".
  * Since some developers forgot about that, using -DPFE_USE_PFE_BASE_H
  * can fix the situation as it will forward to the correct include file.
  */

#ifndef __PFE_PFE_H
#define __PFE_PFE_H
#if !defined __PFE_DEF_CONFIG_H
#if defined PFE_USE_PFE_BASE_H
#include <pfe/pfe-base.h>
#else

#ifdef PFE_SUFFIX
#define P4WLIST_(SET,SUFFIX)  SET ## _LTX_p4_WLIST ## SUFFIX
#define P4WORDS_(SET,SUFFIX)  SET ## _LTX_p4_WORDS ## SUFFIX
#define P4WLIST(SET)  P4WLIST_(SET,PFE_SUFFIX)
#define P4WORDS(SET)  P4WORDS_(SET,PFE_SUFFIX)
#else
#define P4WLIST(SET)  SET ## _LTX_p4_WLIST
#define P4WORDS(SET)  SET ## _LTX_p4_WORDS
#endif
#define extern_P4WORDS(SET) extern const p4Words P4WORDS (SET)

typedef void (*p4code) (void);  /* pointer to executable code */

typedef struct			/* describe a word for registration */
{				/* in the dictionary */
  const char *name;		/* name of word */
  p4code ptr;			/* code it executes or something */
}				/* according to spc@pineal.math.fau.edu: */
p4Word;				/* 'void *' doesn't work here on SGI */

typedef struct			/* describes the set of words */
{				/* a module loads into the dictionary */
  int n;			/* how many words? */
  const p4Word *w;		/* pointer to vector of words */
  const char *name;		/* name of word set, or null */
}
p4Words;

#ifdef MODULE /* shall change to latter sometimes soon... */
#define P4_MODULE_LIST(name) \
        void* p4_LTX_p4_MODULE () \
        {               	 \
          return (void*) &P4WORDS (name); \
        }
#else
#define P4_MODULE_LIST(name) \
        void* name##_LTX_p4_MODULE () \
        {               	 \
          return (void*) &P4WORDS (name); \
        }
#endif

#if defined HOST_CPU_I386 || defined HOST_CPU_I486 \
 || defined HOST_CPU_I586 || defined HOST_CPU_I686
#  define P4_REGTH "%ebx"
#  define P4_REGIP "%esi"
#elif defined HOST_CPU_POWERPC || defined __target_cpu_powerpc
#  define P4_REGTH "14"
#  define P4_REGSP "15"
#  define P4_REGRP "16"
#  define P4_REGIP "17"
#elif defined HOST_CPU_M68K || defined __target_cpu_m68k
#  define P4_REGTH "%a4"
#  define P4_REGSP "%a5"
#elif defined HOST_CPU_I960 || defined __target_cpu_i960
/* i960 has register based calling - if you use too many args, these
   registers will be clobbered - this is actually a gcc-bug, where the
   system call sequence generator doesn't save these regs */
#  define P4_UPPER_REGS
#  define P4_REGTH "g10"
#  define P4_REGSP "g11"
#  define P4_REGIP "g12"
#else
#warning "CPU is unknown"
#endif

#ifndef P4_MOPTRS
#define P4_MOPTRS 128
#endif

typedef unsigned char p4char;

typedef struct p4_Dictionary    p4_Dictionary;

struct p4_Dictionary
{
    p4char* last;  /* PFE.last -> PFE.dict.last */
    p4char* link;  /* PFE.link -> PFE.dict.link */
    p4char* here;  /* PFE.dp   -> PFE.dict.here */
    p4char* base;  /* PFE.dict -> PFE.dict.base */
    p4char* limit; /* PFE.dictlimit -> PFE.dict.limit */
};

struct p4_Thread
{
    void* p [P4_MOPTRS];
    p4_Dictionary dict;
};

#ifdef P4_REGTH                 /* forth thread pointer */
register struct p4_Thread* p4TH asm (P4_REGTH);
#else
extern struct p4_Thread* p4TH;
# if defined REENTRANT || defined _REENTRANT
#  ifdef __GNUC__
#  warning pfe lost reentrance capability here
#  else
#  error   pfe lost reentrance capability here
#  endif
# endif
#endif

typedef int p4cell;

#ifdef P4_REGSP                 /* the stack pointer */
register p4cell * p4SP asm (P4_REGSP);
#else
#define p4SP (PFE.sp)
#endif

#define SP p4SP

#define PFE (*p4TH)

#define P4CODE(X) X ## _
#define FXCode(X) P4CODE (X) (void) /* declare a primitive */
#define FCode(X) void FXCode(X)     /* obsoleted w/ FCode */

#define P4_INC(P,T)	(((T *)(P))++)
#define FX_DROP		(P4_INC(p4SP,p4cell))
#define FX_PUSH(X)      (*--p4SP = (p4cell)(X))
#define FX(X) (P4CODE(X) ())

#ifndef DIM
#define DIM(X)		((int)(sizeof (X) / sizeof *(X)))
#endif

#define	P4_FXCO(NM,PCODE)      { "p\237"NM, &P4CODE (PCODE) }
#define	P4_IXCO(NM,PCODE)      { "P\377"NM, &P4CODE (PCODE) }
#define P4_OVAR(NM)	       { "v\237"NM, (p4code)0) }
#define	P4_OCON(NM,VAL)	       { "c\237"NM, (p4code)(VAL) }

#define P4_SLOT(NM, SLOTVAR)   { "s\237"NM, (p4code)(SLOTVAR) }
#define P4_SSIZ(NM, SIZE)      { "S\377"NM, (p4code)(SIZE) }
#define P4_INTO(NM, ALSO)      { "i\377"NM, (p4code)(ALSO) }
#define P4_LOAD(NM, WORDS)     { "I\377"NM, (p4code)(&P4WORDS(WORDS)) }

#define P4_LISTWORDSET(SET) static const p4Word P4WLIST (SET)
#define P4_LISTWORDS(SET) static const p4Word P4WLIST (SET) []

#define P4_COUNTWORDSET(SET,NAME) P4_COUNTWORDS(SET,NAME)
#define P4_COUNTWORDS(SET,NAME) \
        const p4Words P4WORDS(SET) = \
        {			     \
          DIM (P4WLIST (SET)),	     \
          P4WLIST (SET),	     \
          NAME			     \
        }

extern char *p4_store_c_string (const char *src, int n, char *dst, int max);

extern void FXCode (p4_allot);
extern void FXCode (p4_here);

#define p4_DP   (PFE.dict.here)
#define p4_HERE (PFE.dict.here)
#define HERE    p4_HERE

/* __PFE_USE_PFE_BASE_H */
#endif
/* __PFE_DEF_CONFIG_H */
#endif
/* __PFE_PFE_H */
#endif
