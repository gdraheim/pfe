/*****************************************************************************/
/**
  * Important definitions for the PFE modules
  *
  * Copyright (C) Tektronix, Inc. 2000 - 2001. All Rights Reserved.
  *
  * Project K1297-G20
  *
  * NOTE: This file is only used in connection with the swigpfe tool.
  *       Do not rely on any of the definitions in this file.  They
  *       are likely to be changed together with the swigpfe tool.
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
  * @(#) $Id: pfe.h,v 0.30 2001-03-12 09:33:42 guidod Exp $
  *
  * !! this file is only used for the SWIG that generates 
  * !! a pfe loadable module source from C header files.
  */

#define P4WLIST(SET)  SET ## _LTX_p4_WLIST
#define P4WORDS(SET)  SET ## _LTX_p4_WORDS
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

#define P4_LOADSTRUCT(NAME) p4_ ## NAME ## _LoadList
#define P4_LOADLIST(NAME) const void* P4_LOADSTRUCT (NAME) []

/* extern_P4WORDS(hola); */
#define P4_LOAD_END   ((void *) 0)
#define P4_LOAD_INTO  ((void *) 1)
#define P4_LOAD_ORDER ((void *) 3)

#if 1 /* shall change to latter sometimes soon... */
#define P4_MODULE_LIST(name) \
        void* p4_LTX_p4_MODULE () \
        {               	 \
          return &P4_LOADSTRUCT (name); \
        }
#else
#define P4_MODULE_LIST(name) \
        void* name##_LTX_p4_MODULE () \
        {               	 \
          return &P4_LOADSTRUCT (name); \
        }
#endif

#if defined CPU_i386 || defined CPU_i486 || defined CPU_i586 || defined CPU_i686
#  define P4_REGTH "%ebx"
#  define P4_REGIP "%esi"
#elif defined CPU_powerpc || defined __target_cpu_powerpc
#  define P4_REGTH "14"
#  define P4_REGSP "15"
#  define P4_REGRP "16"
#  define P4_REGIP "17"
#elif defined CPU_m68k || defined __target_cpu_m58k
#  define P4_REGTH "%a4"
#  define P4_REGSP "%a5"
#elif defined CPU_i960 || defined __target_cpu_i960
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

struct p4_Thread
{
    void* p [P4_MOPTRS];
    p4char* dp;
    p4char* dict;
    p4char* dictlimit;
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
#define FCode(X) void P4CODE (X) (void) /* declare a primitive */

#define P4_INC(P,T)	(((T *)(P))++)
#define FX_DROP		(P4_INC(p4SP,p4cell))
#define FX_PUSH(X)      (*--p4SP = (p4cell)(X))
#define FX(X) (P4CODE(X) ())

#ifndef DIM
#define DIM(X)		((int)(sizeof (X) / sizeof *(X)))
#endif

#define	P4_FXCO(NM,PCODE)      { "p\x9F"NM, &P4CODE (PCODE) }
#define	P4_IXCO(NM,PCODE)      { "P\xFF"NM, &P4CODE (PCODE) }
#define P4_OVAR(NM)	       { "v\x9F"NM, (p4code)0) }
#define	P4_OCON(NM,VAL)	       { "c\x9F"NM, (p4code)(VAL) }

#define P4_LISTWORDS(SET) static const p4Word P4WLIST (SET) []
#define P4_COUNTWORDS(SET,NAME) \
        const p4Words P4WORDS(SET) = \
        {			     \
          DIM (P4WLIST (SET)),	     \
          P4WLIST (SET),	     \
          NAME			     \
        }

/* defines without P4_ prefix are depracated and will be gone soon */
#define	CO(NM,PCODE)	{ "p\x9F"NM, &P4CODE (PCODE) }
#define	CI(NM,PCODE)	{ "P\xFF"NM, &P4CODE (PCODE) }
#define OV(NM)		{ "v\x9F"NM, (p4code)0) }
#define	OC(NM,VAL)	{ "c\x9F"NM, (p4code)(VAL) }

#define LISTWORDS(SET) static const p4Word P4WLIST (SET) []
#define COUNTWORDS(SET,NAME) \
        const p4Words P4WORDS(SET) = \
        {			     \
          DIM (P4WLIST (SET)),	     \
          P4WLIST (SET),	     \
          NAME			     \
        }

extern char *p4_store_c_string (const char *src, int n, char *dst, int max);

extern FCode (p4_allot);
extern FCode (p4_here);

#define p4_DP (PFE.dp)
#define DP    p4_DP
