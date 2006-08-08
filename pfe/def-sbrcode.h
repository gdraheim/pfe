#ifndef __PFE_DEF_SBRCODE_H
#define __PFE_DEF_SBRCODE_H

/** 
 * -- Define macros for definition of subroutine machine code.
 *
 *  Copyright (C) Guido Draheim, 2004
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE             @(#) %derived_by: guidod %
 *  @version %version: bln_mpt1!33.36 %
 *    (%date_modified: Wed Sep 11 17:01:27 2002 %)
 *
 *  @description
 *         Declares the types and variables for the Forth Virtual Machine.
 *         Using GCC these can be put into CPU registers that are
 *         valid across function calls.
 */
/*@{*/

#include <pfe/def-config.h>

#if !defined __PFE_DEF_REGS_H && !defined __PFE_DEF_MACRO_H
#error do not use this file directly - use def-regs.h or def-macro.h
#endif

#if defined PFE_SBR_CALL_THREADING && defined __GNUC__
#define PFE_SBR_REAL_RETVAL ((P4_REGIP_T) (__builtin_return_address(0)))
#define PFE_SBR_REAL_RFRAME ((P4_REGRP_T) (__builtin_frame_address(0)))
#endif


/* ========================================================== ARCH_I386 === */
# if defined HOST_CPU_I386 || defined HOST_CPU_I486 \
  || defined HOST_CPU_I586 || defined HOST_CPU_I686 \
  || defined HOST_ARCH_I386 || defined __target_arch_i386 \
  || defined HOST_ARCH_X86_64 || defined HOST_CPU_X86_64

# if defined HOST_ARCH_X86_64 || defined HOST_CPU_X86_64
# define PFE_SBR_ARCH_I386_64 1
# define PFE_SBR_ARCH_I386 1
# define PFE_SBR_ARCH "x86_64"
# else
# define PFE_SBR_ARCH_I386 1
# define PFE_SBR_ARCH "i386"
# endif

/* note: we will define the 32bit/64bit register name macros in def-regmacro */

# define PFE_SBR_NO_REGIP
# define PFE_SBR_NO_REGRP

/* and the SBR assembler snippets.... */
# define PFE_TAKE_CODE_RP    /* RP is taken via local setup code */
# define PFE_TAKE_IP_VIA_RP  /* IP is taken dereferencing our RP */

/* CALL-near uses relative address (from next instruction). The x86_64 uses 
 * a 32bit signed offset for the call target. So we have to load a 64bit
 * value to %ecx and call indirectly. */
# if PFE_SBR_ARCH_I386_64+0
#  define _PFE_SBR_COMPILE_CALL(X,V) { \
              p4cell __sbr_offset = (p4char*)(V) - (p4char*)(X); \
              if ((-1L<<30) < __sbr_offset && __sbr_offset < (1L<<30)) { \
                *P4_INC_(p4char*,(X)) = '\xE8' ; \
                *P4_INC_(long*,(X))  = (long)(__sbr_offset-5) ; \
 	      } else { \
                *P4_INC_(p4char*,(X)) = '\x48' ; \
                *P4_INC_(p4char*,(X)) = '\xE8' ; \
                *P4_INC_(void**,(X))  = (void*)(__sbr_offset-10) ; } }
#  define PFE_SBR_COMPILE_CALL(X,V) { \
              p4cell __sbr_offset = (p4char*)(V) - (p4char*)(X); \
              if ((-1L<<30) < __sbr_offset && __sbr_offset < (1L<<30)) { \
                *P4_INC_(p4char*,(X)) = '\xE8' ; \
                *P4_INC_(int*,(X))  = (int)(__sbr_offset-5) ; \
 	      } else { \
                *P4_INC_(p4char*,(X)) = '\x48' ; \
                *P4_INC_(p4char*,(X)) = '\xB9' ; \
                *P4_INC_(void**,(X))  = (void*)(V) ; \
                *P4_INC_(p4char*,(X)) = '\xFF' ; \
                *P4_INC_(p4char*,(X)) = '\xD1' ; } }
# elif 0
#  define _PFE_SBR_COMPILE_CALL(X,V) { \
             *P4_INC_(p4char*,(X)) = '\xE8' ; \
             *P4_INC_(void**,(X)) = (void*)(V) ; \
              P4_PTR_(p4char**,(X))[-1] -= ((p4char*)(X)); }
# else
#  define PFE_SBR_COMPILE_CALL(X,V) { \
             *P4_INC_(p4char*,(X)) = '\xE8' ; \
             *P4_INC_(void**,(X))  = (void*)(V) ; \
              P4_PTR_(p4cell*,(X))[-1] = \
                  (((p4char**)(X))[-1]) -  ((p4char*)(X)); }
# endif
#  define PFE_SBR_COMPILE_EXIT(X) \
             *P4_INC_(p4char*,(X)) = '\xC3' /* RET near    */

# if 0
#  define _PFE_SBR_SET_RP(X,V) { \
             asm volatile ("movl %0, %%esp" :: "r" ((X)-1)); \
             asm volatile ("movl %0, (%%esp)" :: "r" (V)); }
# elif PFE_SBR_ARCH_I386_64+0
#     define PFE_SBR_SET_RP(X,V) { \
               asm volatile ("movq %0, %%rsp" :: "r" (X)); \
               asm volatile ("push %0" :: "r" (V)); }
# else
#     define PFE_SBR_SET_RP(X,V) { \
               asm volatile ("movl %0, %%esp" :: "r" (X)); \
               asm volatile ("push %0" :: "r" (V)); }
# endif
#     define PFE_SBR_EXIT_RP { \
               asm volatile ("ret"); }

#  if defined PFE_OMIT_FRAME_POINTER
#  define PFE_SBR_RP_OFFSET 1
#  else
#  define PFE_SBR_RP_OFFSET 2
#  endif

#  ifdef  PFE_SBR_CALL_ARG_PREFIXING
#  define PFE_SBR_CALL_ARG_THREADING 1
/* "mov $imm, %eax" ; body_ptr "%eax" */
#  define P4_SBR_TAKE_BODY       asm ("%"PFE_i386_EAX"")
#  define FX_SBR_TAKE_BODY       /* nothing - it's an argument... */
                                 /* MOV dw, ax / MOV dd, eax */
#  define FX_SBR_GIVE_BODY(X,A)  *((p4char*)(X))++ = '\xb8' ; \
                                  *((void**)(X))++ = (void*)(A);
#  if 0
/* code_ptr = RETVAL */
#  define P4_SBR_TAKE_CODE       = PFE_SBR_RP
#  define FX_SBR_GIVE_CODE(X)    /* nothing - we use RETVAL */
/* most gcc3 versions are buggy - they use %ebp for the builtin_frame
 * function even when running in -fomit-frame-pointer mode. Sorry. */
#  else
/* "mov %esp, %eax" ; code_ptr "%eax" */
#  define P4_SBR_TAKE_CODE       asm ("%"PFE_i386_EAX"")
#  define FX_SBR_TAKE_CODE       /* nothing - it's an argument... */
                                 /* LEA esp, eax (LEA eax, esp?) */
#  define FX_SBR_GIVE_CODE(X)    *((p4char*)(X))++ = '\x89' ; \
                                 *((p4char*)(X))++ = '\xe0' ; 
/*                               *((p4char*)(X))++ = '\xc4' ; ? */
#  endif /* asm/gcc */
#  endif /* SBR_CALL_ARG_PREFIXING */

/* .... */
#  define PFE_SBR_DECOMPILE_PROC(X) (X)
#  define PFE_SBR_DECOMPILE_IS_BODY_ARG(X) (((p4char*)(X))[0] == '\xb8')
#  define PFE_SBR_DECOMPILE_IS_CODE_ARG(X) ( \
                           (((p4char*)(X))[0] == '\x89') && \
                           (((p4char*)(X))[1] == '\xe0') )
#  define PFE_SBR_DECOMPILE_IS_CALL_CODE(X) ( \
                           (((p4char*)(X))[0] == '\xE8') || ( \
                           (((p4char*)(X))[0] == '\x48') && \
                           (((p4char*)(X))[1] == '\xE8') ))
#  define PFE_SBR_DECOMPILE_IS_EXIT_CODE(X) (((p4char*)(X))[0] == '\xC3')

/* ======================================================== ARCH_POWERPC === */
# elif defined HOST_OS_AIX3 || defined HOST_CPU_POWERPC \
    || defined __target_os_aix3 || defined __target_cpu_powerpc \
    || defined HOST_ARCH_POWERPC

#   define PFE_SBR_ARCH_POWERPC 1
#   define PFE_SBR_ARCH "powerpc"

#   define PFE_SBR_NO_REGIP
/* #define PFE_SBR_NO_REGRP // we use a different register than the C ABI */
#   define PFE_SBR_DEF_REGRP "16"

/* and the SBR assembler snippets.... */

/* mflr r0 ; stwu r0, -4(r16) ( r16 = REGRP ) */
#  define PFE_SBR_COMPILE_PROC(X) *((p4cell*)(X))++ = 0x7c0802a6; \
                                  *((p4cell*)(X))++ = 0x9410fffc;
/* lwz r0, 0(r16) ; mtlr r0 ; addi r16,r16,4 ; blr */
#  define PFE_SBR_COMPILE_EXIT(X) *((p4cell*)(X))++ = 0x80100000; \
                                  *((p4cell*)(X))++ = 0x7c0803a6; \
                                  *((p4cell*)(X))++ = 0x3a100004; \
                                  *((p4cell*)(X))++ = 0x4e800020; 

/* opcode 0x48 is branch,  0x03 is absolute, 0x01 is relative (from the
 * branch instruction itself!), addresses have 26bit max range (no longcall) */
#  define PFE_SBR_COMPILE_CALL(X,V)  \
          { *(void**)(X) = (void*)(V); \
            if ( ((p4char*)(X))[0] <  0x02) goto __sbr_absolute; \
            if ( ((p4char*)(X))[0] >= 0x0E) goto __sbr_absolute_upper; \
            (*(p4cell*)(X)) = (*(p4char**)(X)) - ((p4char*)(X)); \
            if ( ((p4char*)(X))[0] <  0x02) goto __sbr_relative; \
            if ( ((p4char*)(X))[0] >= 0x0E) goto __sbr_relative_back; \
            P4_fail ("could not 'COMPILE,' sbr-call - using NOOP"); \
            (*(p4cell*)(X))  = 0x01; \
            goto __sbr_finalize; \
          __sbr_relative_back: \
            ((p4char*)(X))[0] &= 0x03; \
          __sbr_relative: \
            ((p4char*)(X))[3] |= 0x01; \
            goto __sbr_finalize; \
          __sbr_absolute_upper: \
            ((p4char*)(X))[0] &= 0x03; \
          __sbr_absolute: \
            ((p4char*)(X))[3] |= 0x03; \
            \
          __sbr_finalize: \
            ((p4char*)(X))[0] |= 0x48; \
            ((void**)(X))++; \
          }

/* #if defined PFE_SBR_CALL_THREADING */

#  define PFE_SBR_RP (PFE_SBR_REAL_RFRAME+2)
/*#define PFE_SBR_RP (&PFE_SBR_REAL_RETVAL)  */

/* the PPC ABI is a bit weird - before calling a routine, the caller
 * is responsible to push the arguments *and* make additional room for
 * one return-address cell on top *under* its own frame-pointer savearea
 * all this is just because the branch-opcode will not save the IP into
 * the stack and the callee will not use it - unless the callee wants to
 * call another routine (or use some locals) in which case it can use 
 * the additional cell on top of the callframe. To get the caller's IP
 * we have to fetch the LR register, and changing RP must take into
 * account the additional blank cell since the C routines will expect
 * to be able to use it.
 *
 * Here we choose another model which is not along the traditional way
 * of direct-threading - we choose an extra forth RP. This makes a lot 
 * of routines easier and bypasses the weird ABI - but you can not
 * anymore place the routine as a direct callback of a system function.
 * however you did not want to do that anyway, right...
 */

# ifdef  PFE_SBR_CALL_ARG_PREFIXING
# define PFE_SBR_CALL_ARG_THREADING 1
/* # define PFE_SBR_CALL_ARG_IS_GLOBAL 1 ** not a scratch register */

#define PFE_TAKE_CODE_IP       /* the IP is taken via local setup code */
#ifndef P4_REGRP
#error  P4_REGRP still needed with this SBR-CALL-ARG threading variant
#endif

/* "load r2, $imm -> lis r2,upper ; ori r2, lower" */
#  define P4_SBR_TAKE_BODY       asm ("2")
#  define FX_SBR_TAKE_BODY       /* nothing - it's an argument... */
#  define FX_SBR_GIVE_BODY(X,A)  { register p4ucell _u = (A); \
               *((unsigned short*)(X))++ = 0x3c40 ; \
               *((unsigned short*)(X))++ = _u>>16; \
               *((unsigned short*)(X))++ = 0x6042 ; \
               *((unsigned short*)(X))++ = (unsigned short)_u ; }

/* "mr r2, r1" */
#  define P4_SBR_TAKE_CODE
#  define FX_SBR_GIVE_CODE(X)    *((p4cell*)(X))++ = 0x7c220b78;
#  define FX_SBR_TAKE_CODE  \
          register P4_REGIP_T p4IP  asm ("2"); \
          asm volatile ("mr 1,%0" :: "r" (p4IP)); \
          asm volatile ("mflr %0" : "=r" (p4IP)); 
#  define FX_SBR_EXIT_CODE { \
          asm volatile ("mtlr %0" :: "r" (p4IP)); \
          asm volatile ("blr"); }
#  endif /* SBR_CALL_ARG */
/* # endif // SBR_CALL */

/* ... */
#  define PFE_SBR_DECOMPILE_PROC(X) (((p4code*)(X))+2)
#  define PFE_SBR_DECOMPILE_IS_CODE_ARG(X) (((p4cell*)(X))[0] == 0x7c220b78)
#  define PFE_SBR_DECOMPILE_IS_BODY_ARG(X) ( \
                           (((unsigned short*)(X))[0] == 0x3c40) && \
                           (((unsigned short*)(X))[2] == 0x6042) )
#  define PFE_SBR_DECOMPILE_IS_CALL_CODE(X) ( \
                          ((p4char*)(X))[0] == 0x48 && ( \
                          ((p4char*)(X))[3] & 0x03 == 0x03 || \
                          ((p4char*)(X))[3] & 0x03 == 0x01 ))
#  define PFE_SBR_DECOMPILE_IS_EXIT_CODE(X) ( \
                          (((p4cell*)(X))[3]= 0x4e800020) )
#  define PFE_SBR_DECOMPILE_TO_BODY(X,P) { \
                           p4cell arg = 0; \
                           arg = ((unsigned short*)(X))[1]; arg <<= 16; \
	                   arg += ((unsigned short*)(X))[3]; }
                          

/* ========================================================== ARCH_M68K === */
# elif defined HOST_CPU_M68K || defined __target_cpu_m68k \
    || defined HOST_ARCH_M68K

#   define PFE_SBR_ARCH_M68K 1
#   define PFE_SBR_ARCH "m68k"

#   define PFE_SBR_NO_REGIP
#   define PFE_SBR_NO_REGRP

/* and the SBR assembler snippets.... */
# define PFE_TAKE_CODE_RP    /* RP is taken via local setup code */
# define PFE_TAKE_IP_VIA_RP  /* IP is taken dereferencing our RP */

#  define PFE_SBR_COMPILE_EXIT(X) *((short*)(X))++ = 0x4E75 
/* 32bit immediate address for sbr-call */
#  define PFE_SBR_COMPILE_CALL(X,V) { \
                                  *((short*)(X))++ = 0x4EB9 ; \
                                  *((void**)(X))++ = (void*)(V); }
/* #  define PFE_SBR_SET_RP(X,V) { \
 *             asm volatile ("movl %0, %%esp" :: "r" ((X)-1)); \
 *             asm volatile ("movl %0, (%%esp)" :: "r" (V)); }
 */
#     define PFE_SBR_SET_RP(X,V) { \
               asm volatile ("movl %0, %%a7" :: "r" (X)); \
               asm volatile ("movl %0, %%a7@-" :: "r" (V)); }
#     define PFE_SBR_EXIT_RP { \
               asm volatile ("ret"); }

# ifdef  PFE_SBR_CALL_ARG_PREFIXING
# define PFE_SBR_CALL_ARG_THREADING 1
/* # define PFE_SBR_CALL_ARG_IS_GLOBAL 1 ** not a scratch register */

#  if 0   /* the use of %a0 seems to irritate the compiler */
/* "mov $imm, %a0" */
#  define P4_SBR_TAKE_BODY       asm ("%a0")
#  define FX_SBR_TAKE_BODY       /* nothing - it's an argument... */
#  define FX_SBR_GIVE_BODY(X,A)  *((short*)(X))++ = 0x207C ; \
                                  *((void**)(X))++ = (void*)(A);
/* "mov %a7, %a0" */
#  define P4_SBR_TAKE_CODE       asm ("%a0")
#  define FX_SBR_TAKE_CODE       /* nothing - it's an argument... */
#  define FX_SBR_GIVE_CODE(X)    *((short*)(X))++ = 0x204F ; 

#  else  /* however %a1 is a scratch register too */
/* "mov $imm, %a1" */
#  define P4_SBR_TAKE_BODY       asm ("%a1")
#  define FX_SBR_TAKE_BODY       /* nothing - it's an argument... */
#  define FX_SBR_GIVE_BODY(X,A)  *((short*)(X))++ = 0x227C ; \
                                  *((void**)(X))++ = (void*)(A);
/* "mov %a7, %a1" */
#  define P4_SBR_TAKE_CODE       asm ("%a1")
#  define FX_SBR_TAKE_CODE       /* nothing - it's an argument... */
#  define FX_SBR_GIVE_CODE(X)    *((short*)(X))++ = 0x224F ; 
#  endif 
# endif /* SBR_CALL_ARG_PREFIXING */

/* .... */
#  define PFE_SBR_DECOMPILE_PROC(__dp) (__dp)

/* ========================================================= ARCH_SPARC === */
# elif defined HOST_CPU_SPARC || defined __target_cpu_sparc \
    || defined HOST_ARCH_SPARC

#   define PFE_SBR_ARCH_SPARC 1
#   define PFE_SBR_ARCH "sparc"

/* "sub %o6, 4, %o6" "st %o7, [ %o6 ]" a.k.a. "push %o7, %sp" */
#define PFE_SBR_COMPILE_PROC(X)   *((p4cell*)(X))++ = 0x9c03bffc ; \
                                  *((p4cell*)(X))++ = 0xde238000 ;

/* "ld [ %o6 ], %o7" "retl" "add %o6, 4, %o6" a.k.a. "pop %sp, %o7" "ret %o7"*/
#define PFE_SBR_COMPILE_EXIT(X)   *((p4cell*)(X))++ = 0xde038000 ; \
                                  *((p4cell*)(X))++ = 0x81c3e008 ; \
                                  *((p4cell*)(X))++ = 0x9c03a004 ;

/* fill the delay-slot with the decr-%sp half of the "push %o7, %sp" operation
 * - that will probably not work for C compiled primitives but just for forth
 * compiled colon-routines - the sbr-threading is double-size anyway due to the
 * ("nop") delay-slot and we can use it to spare a memory-access on runtime */
#define PFE_SBR_COMPILE_CALL(X,V) { \
             if ( *(p4cell*)(V) == 0x9c03bffc ) { \
                  *((p4cell**)(X))++ = ((p4cell*)V)+1; \
                  *((p4cell*)(X))++ = 0x9c04bffc; \
             }else{ \
                  *((p4cell**)(X))++ = ((p4cell*)V); \
                  *((p4cell*)(X))++ = 0x01000000; \
             } }

#if defined PFE_SBR_CALL_THREADING
#  define PFE_SBR_RP  (PFE_SBR_REAL_RFRAME+2)
/*#define PFE_SBR_RP (&PFE_SBR_REAL_RETVAL)  */
#  define PFE_SBR_IP  (PFE_SBR_REAL_RETVAL)
#  define PFE_TAKE_IP_VIA_RP  /* IP is taken dereferencing our RP */
#endif /* SBR_CALL_THREADING */

/* ... */
#  define PFE_SBR_DECOMPILE_PROC(__dp) (((p4code*)(__dp))+2)

#endif /* SBR_ARCH */
/* ======================================================= CLEAR ARCH === */

#ifndef PFE_SBR_COMPILE_PROC
#define PFE_SBR_COMPILE_PROC(X)
#endif

#define FX_COMPILE_PROC PFE_SBR_COMPILE_PROC(p4_DP)

/* Forth RP to RFRAME offset */
#ifndef PFE_SBR_RP_OFFSET
#define PFE_SBR_RP_OFFSET 2
#endif

/* how much is the IP above the forth RP when forth-RP is derived f/ cpu-RP */
/* #define PFE_SBR_IP_VIA_RP PFE_SBR_RP[PFE_SBR_IP_OFFSET] */
/* remember: the SBR-threading Forth-IP is identical w/ the callframe RETVAL */
#ifndef PFE_SBR_IP_OFFSET
#define PFE_SBR_IP_OFFSET -1
#endif

/*
 * define PFE_SBR_RP  (PFE_SBR_REAL_RFRAME+2)
 * define PFE_SBR_RP (&PFE_SBR_REAL_RETVAL) 
 * define PFE_SBR_IP  (PFE_SBR_REAL_RETVAL)
 */

#ifndef PFE_SBR_RP
#define PFE_SBR_RP  (PFE_SBR_REAL_RFRAME+PFE_SBR_RP_OFFSET)
#endif

#ifndef PFE_SBR_IP
#define PFE_SBR_IP  (PFE_SBR_REAL_RETVAL)
#endif

#endif /* once */
