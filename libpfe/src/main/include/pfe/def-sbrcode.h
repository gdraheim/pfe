#ifndef __PFE_DEF_SBRCODE_H
#define __PFE_DEF_SBRCODE_H

/**
 * -- Define macros for definition of subroutine machine code.
 *
 *  Copyright (C) Guido U. Draheim, 2004
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author$)
 *  @version $Revision$
 *     (modified $Date$)
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

/** ADD EA, IMM 100000sw mo010r/m dblo dbhi
                10000011 11010100 (sign-extend word-size reg4(sp))
    ADD EA, IMM 100000sw mo010r/m dblo dbhi
                10000001 11010100          = $81 $D4
    SUB EA, IMM 100000sw mo101r/m dblo dbhi
                10000001 11101100          = $81 $EC
*/

#define _PFE_SBR_COMPILE_RP_DROP(X, V) P4_BCOMMA(X, '\x81'); \
                                       P4_BCOMMA(X, '\xD4'); \
                                       P4_LCOMMA(X, (V));
#define _PFE_SBR_COMPILE_RP_ROOM(X, V) P4_BCOMMA(X, '\x81'); \
                                       P4_BCOMMA(X, '\xEC'); \
                                       P4_LCOMMA(X, (V));
#define _PFE_SBR_SIZEOF_RP_ROOM 6
#define _PFE_SBR_SIZEOF_RP_DROP 6

#define PFE_SBR_COMPILE_RP_DROP(X, V) P4_BCOMMA(X, '\x83'); \
                                      P4_BCOMMA(X, '\xC4'); \
                                      P4_BCOMMA(X, (V))
#define PFE_SBR_COMPILE_RP_ROOM(X, V) P4_BCOMMA(X, '\x83'); \
                                      P4_BCOMMA(X, '\xEC'); \
                                      P4_BCOMMA(X, (V))
#define PFE_SBR_SIZEOF_RP_ROOM 3
#define PFE_SBR_SIZEOF_RP_DROP 3

/* saved framepointer... */
#  define PFE_SBR_RP_OFFSET 2

#  ifdef  PFE_SBR_CALL_ARG_PREFIXING
#  define PFE_SBR_CALL_ARG_THREADING 1
/* "mov $imm, %eax" ; body_ptr "%eax" */
#  define P4_SBR_TAKE_BODY       asm ("%"PFE_i386_EAX"")
#  define FX_SBR_TAKE_BODY       /* nothing - it's an argument... */
                                 /* MOV dw, ax / MOV dd, eax */
#  define FX_SBR_GIVE_BODY(X,A)  P4_BCOMMA(X, '\xb8'); \
                                 P4_PCOMMA(X, (A));

/* "mov %esp, %eax" ; code_ptr "%eax" */
#  define P4_SBR_TAKE_CODE       asm ("%"PFE_i386_EAX"")
#  define FX_SBR_TAKE_CODE       /* nothing - it's an argument... */
                                 /* LEA esp, eax (LEA eax, esp?) */
#  define FX_SBR_GIVE_CODE(X)    P4_BCOMMA(X, '\x89'); \
                                 P4_BCOMMA(X, '\xe0');
/*                               P4_BCOMMA(X, '\xc4'); ? */
#  endif /* SBR_CALL_ARG_PREFIXING */

/* .... */
#  define PFE_SBR_DECOMPILE_PROC(X) (X)
#  define PFE_SBR_DECOMPILE_IS_BODY_ARG(X) (((p4char*)(X))[0] == '\xb8')
#  define PFE_SBR_DECOMPILE_IS_CODE_ARG(X) ( \
                           (((p4char*)(X))[0] == (p4char) '\x89') && \
                           (((p4char*)(X))[1] == (p4char) '\xe0') )
#  define PFE_SBR_DECOMPILE_IS_CALL_CODE(X) ( \
                           (((p4char*)(X))[0] == (p4char) '\xE8') || ( \
                           (((p4char*)(X))[0] == (p4char) '\x48') && \
                           (((p4char*)(X))[1] == (p4char) '\xE8') ))
#  define PFE_SBR_DECOMPILE_IS_EXIT_CODE(X) (((p4char*)(X))[0] == (p4char) '\xC3')

#  define PFE_SBR_DECOMPILE_BCOMMA 1

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
#  define PFE_SBR_COMPILE_PROC(X) P4_LCOMMA(X, 0x7c0802a6); \
                                  P4_LCOMMA(X, 0x9410fffc);
/* lwz r0, 0(r16) ; mtlr r0 ; addi r16,r16,4 ; blr */
#  define PFE_SBR_COMPILE_EXIT(X) P4_LCOMMA(X, 0x80100000); \
                                  P4_LCOMMA(X, 0x7c0803a6); \
                                  P4_LCOMMA(X, 0x3a100004); \
                                  P4_LCOMMA(X, 0x4e800020);

/* opcode 0x48 is branch,  0x03 is absolute, 0x01 is relative (from the
 * branch instruction itself!), addresses have 26bit max range (no longcall) */
#  define PFE_SBR_COMPILE_CALL(X,V)  \
          { *(void**)(X) = (void*)(V); \
            if ( ((p4char*)(X))[0] <  0x02) goto PFE_SBR_LABEL_(absolute); \
            if ( ((p4char*)(X))[0] >= 0x0E) goto PFE_SBR_LABEL_(absolute_upper); \
            (*(p4cell*)(X)) = (*(p4char**)(X)) - ((p4char*)(X)); \
            if ( ((p4char*)(X))[0] <  0x02) goto PFE_SBR_LABEL_(relative); \
            if ( ((p4char*)(X))[0] >= 0x0E) goto PFE_SBR_LABEL_(relative_back); \
            PFE_SBR_COMPILE_CALL_FAILED ("using NOOP"); \
            (*(p4cell*)(X))  = 0x01; \
            goto PFE_SBR_LABEL_(finalize); \
          PFE_SBR_LABEL_(relative_back): \
            ((p4char*)(X))[0] &= 0x03; \
          PFE_SBR_LABEL_(relative): \
            ((p4char*)(X))[3] |= 0x01; \
            goto PFE_SBR_LABEL_(finalize); \
          PFE_SBR_LABEL_(absolute_upper): \
            ((p4char*)(X))[0] &= 0x03; \
          PFE_SBR_LABEL_(absolute): \
            ((p4char*)(X))[3] |= 0x03; \
            \
          PFE_SBR_LABEL_(finalize): \
            ((p4char*)(X))[0] |= 0x48; \
            ((void**)(X))++; \
          }
# define PFE_SBR_COMPILE_CALL_FAILED(X) \
          P4_fail("could not 'COMPILE,' sbr-call:" X)

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
               P4_WCOMMA(X, 0x3c40); P4_WCOMMA(X, _u>>16); \
               P4_WCOMMA(X, 0x6042); P4_WCOMMA(X, _u); }

/* "mr r2, r1" */
#  define P4_SBR_TAKE_CODE
#  define FX_SBR_GIVE_CODE(X)    P4_LCOMMA(X, 0x7c220b78);
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
                          (((p4cell*)(X))[3] == 0x4e800020) )
#  define PFE_SBR_DECOMPILE_TO_BODY(X,P) { \
                           p4cell arg = 0; \
                           arg = ((unsigned short*)(X))[1]; arg <<= 16; \
                           arg += ((unsigned short*)(X))[3]; }

#  define PFE_SBR_DECOMPILE_LCOMMA 1

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

#  define PFE_SBR_COMPILE_EXIT(X) P4_WCOMMA(X, 0x4E75)
/* 32bit immediate address for sbr-call */
#  define PFE_SBR_COMPILE_CALL(X,V) { \
                                  P4_WCOMMA(X, 0x4EB9); \
                                  P4_PCOMMA(X, (V)); }
/* #  define PFE_SBR_SET_RP(X,V) { \
 *             asm volatile ("movl %0, %%esp" :: "r" ((X)-1)); \
 *             asm volatile ("movl %0, (%%esp)" :: "r" (V)); }
 */
#     define PFE_SBR_SET_RP(X,V) { \
               asm volatile ("movl %0, %%a7" :: "r" (X)); \
               asm volatile ("movl %0, %%a7@-" :: "r" (V)); }
#     define PFE_SBR_EXIT_RP { \
               asm volatile ("ret"); }

/** ADD data16, An = 1101 ddd0 1111 1100 yyyy yyyy yyyy yyyy
 *  SUB data16, An = 1001 ddd0 1111 1100 yyyy yyyy yyyy yyyy
 * (note that the "quick" opcodes are insufficient because we use
 *  often 3-cells increment where 12 bytes exceeds the maximum
 *  data3 immediate which is 3 bits = max 7)
 */
# define PFE_SBR_COMPILE_RP_DROP (X, V) P4_WCOMMA(X, '\xDEFC') \
                                        P4_WCOMMA(X, (V))
# define PFE_SBR_COMPILE_RP_ROOM (X, V) P4_WCOMMA(X, '\x9EFC') \
                                        P4_WCOMMA(X, (V))
# define PFE_SBR_SIZEOF_RP_DROP 4
# define PFE_SBR_SIZEOF_RP_ROOM 4


# ifdef  PFE_SBR_CALL_ARG_PREFIXING
# define PFE_SBR_CALL_ARG_THREADING 1
/* # define PFE_SBR_CALL_ARG_IS_GLOBAL 1 ** not a scratch register */

#  if 0   /* the use of %a0 seems to irritate the compiler */
/* "mov $imm, %a0" */
#  define P4_SBR_TAKE_BODY       asm ("%a0")
#  define FX_SBR_TAKE_BODY       /* nothing - it's an argument... */
#  define FX_SBR_GIVE_BODY(X,A)  P4_WCOMMA(X, 0x207C); \
                                 P4_PCOMMA(X, (A));
/* "mov %a7, %a0" */
#  define P4_SBR_TAKE_CODE       asm ("%a0")
#  define FX_SBR_TAKE_CODE       /* nothing - it's an argument... */
#  define FX_SBR_GIVE_CODE(X)    P4_WCOMMA(X, 0x204F);

#  else  /* however %a1 is a scratch register too */
/* "mov $imm, %a1" */
#  define P4_SBR_TAKE_BODY       asm ("%a1")
#  define FX_SBR_TAKE_BODY       /* nothing - it's an argument... */
#  define FX_SBR_GIVE_BODY(X,A)  P4_WCOMMA(X, 0x227C); \
                                 P4_PCOMMA(X, (A));
/* "mov %a7, %a1" */
#  define P4_SBR_TAKE_CODE       asm ("%a1")
#  define FX_SBR_TAKE_CODE       /* nothing - it's an argument... */
#  define FX_SBR_GIVE_CODE(X)    P4_WCOMMA(X, 0x224F);
#  endif
# endif /* SBR_CALL_ARG_PREFIXING */

/* .... */
#  define PFE_SBR_DECOMPILE_PROC(__dp) (__dp)
#  define PFE_SBR_DECOMPILE_WCOMMA 1

/* ========================================================= ARCH_SPARC === */
# elif defined HOST_CPU_SPARC || defined __target_cpu_sparc \
    || defined HOST_ARCH_SPARC

#   define PFE_SBR_ARCH_SPARC 1
#   define PFE_SBR_ARCH "sparc"

/* "sub %o6, 4, %o6" "st %o7, [ %o6 ]" a.k.a. "push %o7, %sp" */
#define PFE_SBR_COMPILE_PROC(X)   P4_LCOMMA(X, 0x9c03bffc); \
                                  P4_LCOMMA(X, 0xde238000);

/* "ld [ %o6 ], %o7" "retl" "add %o6, 4, %o6" a.k.a. "pop %sp, %o7" "ret %o7"*/
#define PFE_SBR_COMPILE_EXIT(X)   P4_LCOMMA(X,  0xde038000); \
                                  P4_LCOMMA(X,  0x81c3e008); \
                                  P4_LCOMMA(X,  0x9c03a004);

/* fill the delay-slot with the decr-%sp half of the "push %o7, %sp" operation
 * - that will probably not work for C compiled primitives but just for forth
 * compiled colon-routines - the sbr-threading is double-size anyway due to the
 * ("nop") delay-slot and we can use it to spare a memory-access on runtime */
#define PFE_SBR_COMPILE_CALL(X,V) { \
             if ( *(p4cell*)(V) == 0x9c03bffc ) { \
                  P4_LCOMMA(X, ((p4cell*)V)+1); \
                  P4_LCOMMA(X, 0x9c04bffc); \
             }else{ \
                  P4_LCOMMA(X, ((p4cell*)V)  ); \
                  P4_LCOMMA(X, 0x01000000); \
             } }

#if defined PFE_SBR_CALL_THREADING
#  define PFE_SBR_RP  (PFE_SBR_REAL_RFRAME+2)
/*#define PFE_SBR_RP (&PFE_SBR_REAL_RETVAL)  */
#  define PFE_SBR_IP  (PFE_SBR_REAL_RETVAL)
#  define PFE_TAKE_IP_VIA_RP  /* IP is taken dereferencing our RP */
#endif /* SBR_CALL_THREADING */

/* ... */
#  define PFE_SBR_DECOMPILE_PROC(__dp) (((p4code*)(__dp))+2)
#  define PFE_SBR_DECOMPILE_LCOMMA 1

#endif /* SBR_ARCH */
/* ======================================================= CLEAR ARCH === */
#ifndef PFE_SBR_LABEL_
#define PFE_SBR_LABEL_(label) __pfe_sbr_label_##label
#endif

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
