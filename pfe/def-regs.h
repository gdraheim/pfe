#ifndef __PFE_DEF_REGS_H
#define __PFE_DEF_REGS_H "%full_filespec: def-regs.h~5.7:incl:bln_12xx!1 %"

/** 
 * -- Declares types and variables of the virtual machine.
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE             @(#) %derived_by: guidod %
 *  @version %version: 5.7 %
 *    (%date_modified: Mon Mar 12 10:33:24 2001 %)
 *
 *  @description
 *         Declares the types and variables for the Forth Virtual Machine.
 *         Using GCC these can be put into CPU registers that are
 *         valid across function calls.
 */
/*@{*/

#include <pfe/def-config.h>

/* First the register assignments if GNU-C is used. ======================== */

#if !defined __GNUC__ || defined  __STRICT_ANSI__ || defined __cplusplus
# undef PFE_USE_REGS
#elif defined PFE_WITH_REGS
# define PFE_USE_REGS PFE_WITH_REGS
#elif defined USE_REGS /* TODO: shall vanish soon*/
# define PFE_USE_REGS USE_REGS
#endif

#if defined PFE_USE_REGS && PFE_USE_REGS

# if defined CPU_i386 || defined CPU_i486 || defined CPU_i586 || defined __target_arch_i386

/* gcc 2.95.x is broken !!! it will just mark global-regs as not being
 * in use for a scratch-register - which makes him believe that one can
 * save the thing to the stack and reuse it, even across calls. #$%&!
 * If you need performance on ia32-gcc, use gcc 2.8.x, here we will
 * just take care to be atleast multithreaded. It seems that gcc 2.95.x
 * does not like the %ebp register for some other reasons. Then again,
 * who cares, we have a need for it, so we can use it here 
 */
#  if (__GNUC__ == 2 && __GNUC_MINOR__ >= 9)
#   undef PFE_USE_REGS
#   define PFE_USE_REGS 1
#  endif

/* register %esi/%edi is used for many builtin operations in gcc 2.9x 
 * (e.g. strcpy), so may be one has to use -fno-builtin with gcc 2.9x.
 * The %ebx is suspicously used for sharedlib builds for the some magic
 * pointer movements, and 2.95.x is totally off any sane limits.
 * ... and %eax/%edx is call-clobbered anyway. Hmmm...
 */
#  if PFE_USE_REGS == 1
#     define P4_REGTH "%ebp" /* -fomit-frame-pointer assumed */
#  elif PFE_USE_REGS == 2
#     define P4_REGTH "%ebx" 
#   if !defined SYS_AIX1
#     define P4_REGSP "%ebp" 
#   else
#     define P4_REGSP "%esi"
#   endif
#  elif PFE_USE_REGS >= 3
#     define P4_REGTH "%ebx" 
#     define P4_REGIP "%edi" 
#   if !defined SYS_AIX1
#     define P4_REGSP "%ebp" 
#   else
#     define P4_REGSP "%esi"
#   endif
#  endif

# elif defined __target_os_aix3 || defined __target_cpu_powerpc

#   define P4_REGTH "14"
#   define P4_REGSP "15"
#   define P4_REGRP "16"
#   define P4_REGIP "17"
#  if !defined _K12_SOURCE && PFE_USE_REGS > 2
#   define P4_REGW  "18"
#   define P4_REGLP "19"
#   define P4_REGFP "20"
#  endif

# elif defined CPU_m68k || defined __target_cpu_m68k

#   define P4_REGTH "%a4" 
#   define P4_REGSP "%a5"
#  if !defined _K12_SOURCE && PFE_USE_REGS > 2
#   define P4_REGIP "%a6"
#   define P4_REGRP "%a7"
#   define P4_REGW  "%d6" 
#   define P4_REGLP "%d7" 
#  endif

# elif defined CPU_i960 || defined __target_cpu_i960

#  define P4_UPPER_REGS  /* i960 has register based calling - if you use too */
#  define P4_REGTH "g10" /* many args, these registers will be clobbered - */
#  define P4_REGSP "g11" /* this is actually a gcc-bug, where the system */
#  define P4_REGIP "g12" /* call sequence generator doesn't save these regs */

# elif defined CPU_hppa1 || defined CPU_hppa || defined __target_cpu_hppa

#   define P4_REGTH "%r12"
#   define P4_REGSP "%r13"
#   define P4_REGRP "%r14"
#   define P4_REGIP "%r15"
#   define P4_REGW  "%r16"
#   define P4_REGLP "%r17"
#   define P4_REGFP "%r18"

# elif defined SYS_ultrix || defined __target_os_ultrix

#   define P4_REGTH "$17"
#   define P4_REGSP "$18"
#   define P4_REGRP "$19"
#   define P4_REGIP "$20"
#   define P4_REGW  "$21"
#   define P4_REGLP "$22"
#   define P4_REGFP "$23"

# elif defined SYS_OSF1 || defined __target_os_osf1

#   define P4_REGTH "$9"
#   define P4_REGSP "$10"
#   define P4_REGRP "$11"
#   define P4_REGIP "$12"
#   define P4_REGW  "$13"
#   define P4_REGLP "$14"
#   define P4_REGFP "$15"

# else

#   undef PFE_USE_REGS

# endif
#endif

/* @name The basic types

   note that TYPEOF_CELL is either long or int.
   It must be atleast as big as a pointer.
 */
 
typedef unsigned char  p4char; /* hopefully an 8-bit type */
typedef unsigned short p4word; /* hopefully a 16-bit type */

typedef PFE_TYPEOF_CELL			p4cell;	 /* a stack item */
typedef unsigned PFE_TYPEOF_CELL	p4ucell; /* dito unsigned */

typedef struct
{ 
    p4cell hi; 
    p4ucell lo; 
} p4dcell;	/* dito, double precision signed */
typedef struct 
{ 
    p4ucell hi;
    p4ucell lo; 
} p4udcell;	/* dito, double precision unsigned */

typedef void (*p4code) (void);		/* pointer to executable code */
typedef p4code *p4xt;			/* type of the "execution token" */

typedef struct { p4cell  quot, rem; } fdiv_t;
typedef struct { p4ucell quot, rem; } udiv_t;

typedef struct				/* "map" of a cell */
{
#if PFE_BYTEORDER == 4321
    unsigned PFE_TYPEOF_HALFCELL hi;
    unsigned PFE_TYPEOF_HALFCELL lo;
#else
    unsigned PFE_TYPEOF_HALFCELL lo;
    unsigned PFE_TYPEOF_HALFCELL hi;
#endif
} p4ucell_hi_lo;

struct p4_Thread;

#ifdef P4_REGTH                 /* forth thread pointer */
register struct p4_Thread* p4TH asm (P4_REGTH);
#else
extern struct p4_Thread* p4TH; 
# if defined PFE_WITH_REGS && (defined REENTRANT || defined _REENTRANT)
#  ifdef __GNUC__
#  warning pfe lost reentrance capability
#  else
#  error pfe lost reentrance capability here
#  endif
# endif
#endif

/* Virtual machine registers, declared as variables: ======================= */

#ifdef P4_REGIP			/* the instruction pointer */
register p4xt * p4IP asm (P4_REGIP);
#else
#define p4IP (PFE.ip)
#endif

/* the word pointer may be always accessed like this, but
   having an additional register will speed up the inner interpreter
   (and some other words, too)
*/
#define P4_WP (p4IP[-1])

#ifdef P4_REGW			/* used inside the inner interpreter */
register p4xt	p4WP asm (P4_REGW);
#else
# ifdef P4_REGIP
#   define p4WP	  P4_WP
#   define P4_WP_VIA_IP 1
# else
#define p4WP (PFE.wp)
# endif
#endif

#ifdef P4_REGSP		/* the stack pointer */
register p4cell *	p4SP asm (P4_REGSP);
#else
#define p4SP (PFE.sp)
#endif

#ifdef P4_REGRP		/* the return stack pointer */
register p4xt **	p4RP asm (P4_REGRP);
#else
#define p4RP (PFE.rp)
#endif

#ifdef P4_REGLP		/* pointer to local variables */
register p4cell *	p4LP asm (P4_REGLP);
#else
#define p4LP (PFE.lp)
#endif

#ifndef P4_NO_FP
#ifdef P4_REGFP		/* the floating point stack pointer */
register double *p4FP asm (P4_REGFP);
#else
#define p4FP (PFE.fp)
#endif
#endif


#ifdef _P4_SOURCE
/* variables from the Forth Virtual Machine */

# define WP P4_WP
# define IP p4IP
# define SP p4SP
# define RP p4RP
# define LP p4LP
# define FP p4FP
#endif

#if defined HAVE_VXWORKS_H || defined PFE_HAVE_VXWORKS_H
# include <types/vxCpu.h>
# if defined CPU_m68k || defined __target_cpu_m68k
# define CPU_FAMILY MC680X0
# elif defined CPU_i960 || defined __target_cpu_i960
# define CPU_FAMILY I960
# elif defined CPU_powerpc || defined __target_cpu_powerpc
# define CPU_FAMILY PPC
# endif
# include <vxWorks.h>
#endif

/*@}*/
#endif 
