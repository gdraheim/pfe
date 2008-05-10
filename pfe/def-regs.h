#ifndef __PFE_DEF_REGS_H
#define __PFE_DEF_REGS_H

/** 
 * -- Declares types and variables of the forth virtual machine.
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.7 $
 *     (modified $Date: 2008-05-10 16:51:25 $)
 *
 *  @description
 *         Declares the types and variables for the Forth Virtual Machine.
 *         Using GCC these can be put into CPU registers that are
 *         valid across function calls.
 */
/*@{*/

#include <pfe/def-config.h>
#include <pfe/def-cell.h>
#include <pfe/def-regmacro.h>
#include <pfe/def-regth.h>

#ifdef _P4_NO_REGS_SOURCE
#ifdef __GNUC__
#warning including def-regs.h in a pfe c source file that does not want it
/* some sources do not want the reserved set of global machine registers */
#endif
#endif

typedef void (*p4code) (void);		/* pointer to executable code */

#if !defined PFE_CALL_THREADING
typedef p4code *p4xt;			/* type of the "execution token" */
typedef p4xt    p4xcode;                /* compiled "execution token" */
#else
typedef union p4_ExecToken* p4xt;       /* runtime definition hints */
typedef p4code  p4xcode;                /* compiled "execution token" */
#endif

typedef struct { p4cell  quot, rem; } fdiv_t;
typedef struct { p4ucell quot, rem; } udiv_t;

/* Virtual machine registers, declared as variables: ======================= */

/*_____________________________*/
#define P4_REGSP_TARGET_T p4cell
#define P4_REGSP_T p4cell*

#ifdef P4_REGSP		/* the stack pointer */
register P4_REGSP_T p4SP asm (P4_REGSP);
#else
#define p4SP (PFE.sp)
#endif

/*_____________________________*/
#define P4_REGRP_TARGET_T p4xcode*
#define P4_REGRP_T p4xcode**

/* choose P4_RP_IN_VM to detect extra forth RP stack, otherwise system stack */

#ifdef P4_REGRP		/* the return stack pointer */
register P4_REGRP_T  p4RP asm (P4_REGRP);
#define P4_RP_IN_VM
#elif ! defined PFE_SBR_CALL_THREADING
#define p4RP (PFE.rp)
#define P4_RP_IN_VM
#elif ! defined PFE_SBR_CALL_ARG_THREADING
/* always use FX_RP_WORK + FX_RP_DONE to modify the RP */
/* #define p4RP (&(PFE_SBR_RP[1])) */
#define    p4RP    PFE_SBR_RP
#elif   defined PFE_TAKE_CODE_RP
 /* SBR_CALL_ARG_THREADING uses only local p4RP */
#else
# ifdef __GNUC__
# warning NO RP ARG GIVEN
# else
# error   NO RP ARG GIVEN
# endif
#endif

/*_____________________________*/
#define P4_REGIP_TARGET_T p4xcode
#define P4_REGIP_T  p4xcode*

#ifdef P4_REGIP			/* the instruction pointer */
register P4_REGIP_T p4IP asm (P4_REGIP);
#define PFE_IP_IN_VM
#elif ! defined PFE_SBR_CALL_THREADING
#define p4IP   (PFE.ip)
#define P4_IP_IN_VM
#elif ! defined PFE_SBR_CALL_ARG_THREADING || \
       (defined PFE_TAKE_CODE_RP && defined PFE_TAKE_IP_VIA_RP)
/* #def p4IP   (PFE_SBR_RP[PFE_SBR_IP_OFFSET])*/
#define p4IP   (p4RP[PFE_SBR_IP_OFFSET])
#define P4_IP_VIA_RP
#else
 /* NO RP ARG GIVEN - the IP is a firsthand copy */
#endif

#ifdef  PFE_SBR_IP
#define PFE_GET_IP PFE_SBR_IP
#else
#define PFE_GET_IP (p4IP)
#endif

/*_____________________________*/
#define P4_REGW_T p4xt

/* the word pointer may be always accessed like this, but
   having an additional register will speed up the inner interpreter
   (and some other words, too)
*/
#define P4_WP (p4IP[-1])

#ifdef P4_REGW			/* used inside the inner interpreter */
register P4_REGW_T p4WP asm (P4_REGW);
#else
# ifdef P4_REGIP
# define p4WP	  P4_WP
# define P4_WP_VIA_IP 1
# else
# define p4WP (PFE.wp)
# endif
#endif


/*_____________________________*/
#define P4_REGLP_TARGET_T p4cell
#define P4_REGLP_T p4cell*

#ifdef P4_REGLP		/* pointer to local variables */
register P4_REGLP_T	p4LP asm (P4_REGLP);
#else
#define p4LP (PFE.lp)
#endif

/*_____________________________*/
#define P4_REGFP_TARGET_T double
#define P4_REGFP_T double*

#ifndef P4_NO_FP
# ifdef P4_REGFP	/* the floating point stack pointer */
register P4_REGFP_T p4FP asm (P4_REGFP);
# else
# define p4FP (PFE.fp)
# endif
#endif

/*----------------------------*/

#ifdef _P4_SOURCE
/* variables from the Forth Virtual Machine */
# define WP P4_WP
# define IP p4IP
# define SP p4SP
# define RP p4RP
# define LP p4LP
# define FP p4FP
#endif

/* both SBR_CALL and SBR_CALL_ARG use IP_VIA_RP - and before anyone wants to
 * modify the RP, the return_address must be saved away
 */
#ifdef   P4_IP_VIA_RP
#define  FX_SAVE_IP_  register P4_REGIP_T p4_return_address = p4IP;
#define  FX_DONE_IP_               p4IP = p4_return_address;
#else
#define  FX_SAVE_IP_
#define  FX_DONE_IP_
#endif

/* 
 * There is an exception when we do not use the hardware IP/RP
 * and instead allocating separate forth-VM registers. This is done
 * on powerpc where the native sbr-call overhead does not pay and
 * actually there are no specialized IP/RP system registers anyway.
 * Most other instruction set architectures do have dedicated opcodes
 * for sbr-call including special system registers being touched.
 */

# if defined   PFE_SBR_CALL_THREADING && defined PFE_SBR_NO_REGRP
#  define FX_EXECUTE_RP_DROP(X) FX_EXECUTE_RP_ARG_UNUSED
#  define FX_EXECUTE_RP_ROOM(X) FX_EXECUTE_RP_ARG_UNUSED
#  define FX_COMPILE_RP_DROP(X) PFE_SBR_COMPILE_RP_DROP(p4_DP, (X)*(sizeof(p4code)))
#  define FX_COMPILE_RP_ROOM(X) PFE_SBR_COMPILE_RP_ROOM(p4_DP, (X)*(sizeof(p4code)))
#  define FX_EXECUTE_RP_DROP_SKIPS (*(p4char**)p4IP) += PFE_SBR_SIZEOF_RP_DROP
#  define FX_EXECUTE_RP_ROOM_SKIPS (*(p4char**)p4IP) += PFE_SBR_SIZEOF_RP_ROOM
# else
#  define FX_EXECUTE_RP_DROP(X) p4RP += 3
#  define FX_EXECUTE_RP_ROOM(X) p4RP -= 3
#  define FX_COMPILE_RP_DROP(X)
#  define FX_COMPILE_RP_ROOM(X)
#  define FX_EXECUTE_RP_DROP_SKIPS
#  define FX_EXECUTE_RP_ROOM_SKIPS
# endif

# define FX_EXECUTE_RP_ARG_UNUSED /* old trick:*/ p4RP = p4RP

# define  FX_ALIGNED_VALUE(X) (( ((p4cell)(X)) + \
                               (PFE_SIZEOF_CELL-1) ) &~ (PFE_SIZEOF_CELL-1) )

# if defined   PFE_SBR_CALL_THREADING
#  if defined P4_IP_IN_VM || defined PFE_TAKE_CODE_IP
#  define  FX_NEW_IP_PREP 
#  define  FX_NEW_IP_WORK      
#  define  FX_NEW_IP_CHAR      ((p4char*)p4IP)
#  define  FX_NEW_IP_CELL      ((p4cell*)p4IP)
#  define  FX_NEW_IP_CODE      ((p4xcode*)p4IP)
#  define  FX_NEW_IP_DONE      
#  elif defined PFE_TAKE_IP_VIA_RP
#  define  FX_NEW_IP_PREP  register P4_REGIP_T* __p4_IP_REF = p4RP; \
                                  __p4_IP_REF += PFE_SBR_IP_OFFSET;
#  define  FX_NEW_IP_WORK      { FX_NEW_IP_PREP {
#  define  FX_NEW_IP_CHAR      (*(p4char**)__p4_IP_REF)
#  define  FX_NEW_IP_CELL      (*(p4cell**)__p4_IP_REF)
#  define  FX_NEW_IP_CODE      (*(p4xcode**)__p4_IP_REF)
#  define  FX_NEW_IP_DONE      }}
#  else
#  error   no NEW_IP defined
#  endif /* _TAKEN_IP_VIA_RP */
# define  FX_NEW_IP_SKIP_STRING \
	      FX_NEW_IP_CHAR += 1 + *FX_NEW_IP_CHAR; \
          FX_NEW_IP_CHAR = (p4char*) FX_ALIGNED_VALUE(FX_NEW_IP_CHAR);
# endif /* _SBR_CALL */

#if ! defined PFE_SBR_CALL_ARG_THREADING
/* call-threading and sbr-call-threading. */
#define P4_SBR_TAKE_BODY_T    
#define P4_SBR_TAKE_CODE_T    
#define P4_USE_CODE_ADDR
#define FX_USE_CODE_ADDR
#define FX_USE_CODE_EXIT
#define FX_ARG_CODE_ADDR(X)
#define FX_PUT_CODE_ADDR(X)
#define P4_USE_BODY_ADDR
#define FX_USE_BODY_ADDR
#define FX_ARG_BODY_ADDR(X,A)
#define FX_PUT_BODY_ADDR(X,A) { *(X)++ = (p4xcode)(A); }
#else
/* sbr-call-arg-threading */
#define P4_USE_BODY_ADDR      /*P4_ARG_TAKE_BODY_T p4_BODY P4_SBR_TAKE_BODY*/
#define FX_USE_BODY_ADDR      P4_ARG_TAKE_BODY_T p4_BODY P4_SBR_TAKE_BODY;
#define FX_ARG_BODY_ADDR(X,A) FX_SBR_GIVE_BODY(X,A)
#define FX_PUT_BODY_ADDR(X,A) 
#define P4_ARG_TAKE_BODY_T    register p4cell* 
#define P4_ARG_TAKE_CODE_T    register P4_REGRP_T
# if ! defined FX_SBR_EXIT_CODE
#define P4_USE_CODE_ADDR      /*P4_ARG_TAKE_CODE_T p4RP   P4_SBR_TAKE_CODE*/
#define FX_USE_CODE_ADDR      register P4_REGRP_T p4RP   P4_SBR_TAKE_CODE; 
#define FX_USE_CODE_EXIT
#define FX_ARG_CODE_ADDR(X)   FX_SBR_GIVE_CODE(X)
#define FX_PUT_CODE_ADDR(X)
# else
#define P4_USE_CODE_ADDR      /*P4_ARG_TAKE_CODE_T p4RP   P4_SBR_TAKE_CODE*/
#define FX_USE_CODE_ADDR      FX_SBR_TAKE_CODE
#define FX_USE_CODE_EXIT      FX_SBR_EXIT_CODE
#define FX_ARG_CODE_ADDR(X)   FX_SBR_GIVE_CODE(X)
#define FX_PUT_CODE_ADDR(X)
# endif
#endif

#define FX_USE_CODE_ADDR_UNUSED
#define FX_USE_CODE_EXIT_UNUSED

/* ------------------------------------------------------------------ */

#if !defined PFE_SBR_CALL_THREADING
#define FX_COMPILE1_CALL(X,V)      { *(X)++ = (p4xcode)(V); }
#define FX_COMPILE1_p4_exit        FX_COMPILE1(p4_exit)
#define FX_COMPILE2_p4_exit        FX_COMPILE2(p4_exit)
#define FX_COMPILE1_p4_semicolon   FX_COMPILE1(p4_semicolon)
#define FX_COMPILE2_p4_semicolon   FX_COMPILE2(p4_semicolon)
#define FX_RP_ENTER 
#define FX_RP_LEAVE
#else
#define FX_COMPILE1_CALL(X,V)      PFE_SBR_COMPILE_CALL(X,V) 
#define FX_COMPILE1_p4_exit        PFE_SBR_COMPILE_EXIT(p4_DP) 
#define FX_COMPILE2_p4_exit        PFE_SBR_COMPILE_EXIT(p4_DP) 
#define FX_COMPILE1_p4_semicolon   PFE_SBR_COMPILE_EXIT(p4_DP) 
#define FX_COMPILE2_p4_semicolon   PFE_SBR_COMPILE_EXIT(p4_DP) 
#define FX_RP_ENTER                PFE_SBR_RP_ENTER 
#define FX_RP_LEAVE                PFE_SBR_RP_LEAVE
#endif

/* ------------------------------------------------------------------ */
#if defined PFE_SBR_CALL_ARG_THREADING
#define PFE_THREADING_TYPE "SBR"
#elif defined PFE_SBR_CALL_THREADING
#define PFE_TRHEADING_TYPE "STC"
#elif defined PFE_CALL_THREADING
#define PFE_THREADING_TYPE "CTC"
#else
#define PFE_THREADING_TYPE "ITC"
#endif

#if defined HAVE_VXWORKS_H || defined PFE_HAVE_VXWORKS_H
# include <types/vxCpu.h>
# if defined HOST_CPU_M68K || defined __target_cpu_m68k
# define CPU_FAMILY MC680X0
# elif defined HOST_CPU_I960 || defined __target_cpu_i960
# define CPU_FAMILY I960
# elif defined HOST_CPU_POWERPC || defined __target_cpu_powerpc
# define CPU_FAMILY PPC
# endif
# include <vxWorks.h>
# if defined __GNUC__ && (_ARCH_INT_MIN+0) == (-2147483648)
# undef  _ARCH_INT_MIN
# define _ARCH_INT_MIN (-2147483647-1)
# endif
# if defined __GNUC__ && (_ARCH_INT_MAX+0) == (2147483647)
# undef  _ARCH_INT_MAX
# define _ARCH_INT_MAX 2147483647u
# endif
# if defined __GNUC__ && (_ARCH_UINT_MAX+0) == (4294967295u)
# undef  _ARCH_UINT_MAX
# define _ARCH_UINT_MAX 4294967295u
# endif
# if defined __GNUC__ && (_ARCH_ULONG_MAX+0) == (4294967295u)
# undef  _ARCH_ULONG_MAX
# define _ARCH_ULONG_MAX 4294967295u
# endif
#endif

/*@}*/
#endif 
