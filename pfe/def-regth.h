#ifndef __PFE_DEF_THREADP_H
#define __PFE_DEF_THREADP_H

/* requires P4_REGTH declaration */
#include <pfe/def-regmacro.h>

#ifdef _P4_NO_REGS_SOURCE
#ifdef __GNUC__
#warning including def-regs.h in a pfe c source file that does not want it
/* some sources do not want the reserved set of global machine registers */
#endif
#endif

struct p4_Thread;
struct p4_Session;

# ifdef P4_REGTH
#  define PFE_USE_THREAD_POINTER
# elif defined PFE_WITH_STATIC_REGS
#  define PFE_USE_THREAD_BLOCK
# else
#  define PFE_USE_THREAD_POINTER
# endif

# if PFE_HAS_TLS_SUPPORT+0
# define PFE_CC_THREADED __thread
/* FIXME: __thread seems to be broken */
# undef  PFE_CC_THREADED
# define PFE_CC_THREADED
# else
# define PFE_CC_THREADED
# endif

# ifdef PFE_USE_THREAD_POINTER
#  ifdef P4_REGTH
register struct p4_Thread* p4TH asm (P4_REGTH);
#  else
extern PFE_CC_THREADED struct p4_Thread* p4TH;
#  endif
# else
# define p4TH (&p4_reg)
extern PFE_CC_THREADED struct p4_Thread  p4_reg;
extern PFE_CC_THREADED struct p4_Session p4_opt;
# endif

/* the THREAD_BLOCK structs are defined in option-set.c */

/* actually, the following two defines are the same when p4TH == &p4_reg */
/* but for debugging, the .c -> .i precompiler output is better readable */
#ifdef PFE_USE_THREAD_BLOCK
#define PFE p4_reg
#else  /* _USE_THREAD_POINTER */
#define PFE (*p4TH)
#endif

#endif
