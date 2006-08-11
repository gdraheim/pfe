#ifndef __PFE_DEF_REGMACRO_H
#define __PFE_DEF_REGMACRO_H 
/** 
 * -- Define macros for definition of the forth virtual machine.
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2006 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.2 $
 *     (modified $Date: 2006-08-11 22:56:04 $)
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

#include <pfe/def-sbrcode.h>

/* First the register assignments if GNU-C is used. ======================== */

#if !defined __GNUC__ || defined  __STRICT_ANSI__ || defined __cplusplus
# undef PFE_USE_REGS
#elif defined PFE_WITH_REGS
# define PFE_USE_REGS PFE_WITH_REGS
#elif defined USE_REGS /* TODO: shall vanish soon*/
# define PFE_USE_REGS USE_REGS
#endif

#if defined  PFE_WITH_STATIC_REGS
#define PFE_NEED_NO_REGTH
#endif

#ifdef PFE_CALL_THREADING
# undef P4_NEED_NO_REGW
#endif

#if PFE_USE_REGS+0

# ifdef  PFE_SBR_ARCH            /* we have sbr asm snippets for this arch   */
# ifdef  PFE_SBR_CALL_THREADING  /* and the build variant uses sbr threading */
# ifdef  PFE_SBR_NO_REGIP        /* with special machine register for the IP */
# define PFE_NEED_NO_REGIP       /* then need no separate forth register.    */
# endif                          
# ifdef  PFE_SBR_NO_REGRP        /* On some archs we prefer the Forth SBR */
# define PFE_NEED_NO_REGRP       /* to not use the C SBR machine register */
# endif                          /* for the return stack items (powerpc!) */
# endif
# endif

/* ___________________________________________________  ARCH_I386  ______ */
# if defined HOST_CPU_I386 || defined HOST_CPU_I486 \
  || defined HOST_CPU_I586 || defined HOST_CPU_I686 \
  || defined HOST_ARCH_I386 || defined __target_arch_i386 \
  || defined HOST_ARCH_X86_64 || defined HOST_CPU_X86_64

# define PFE_HOST_ARCH_I386 1
# if defined HOST_ARCH_X86_64 || defined HOST_CPU_X86_64
# define PFE_HOST_ARCH_I386_64 1
# define PFE_i386_EAX "rax"
# define PFE_i386_EBX "rbx"
# define PFE_i386_ECX "rcx"
# define PFE_i386_EDX "rdx"
# define PFE_i386_EBP "rbp"
# define PFE_i386_ESP "rsp"
# define PFE_i386_ESI "rsi"
# define PFE_i386_EDI "rdi"
# else
# define PFE_i386_EAX "eax"
# define PFE_i386_EBX "ebx"
# define PFE_i386_ECX "ecx"
# define PFE_i386_EDX "edx"
# define PFE_i386_EBP "ebp"
# define PFE_i386_ESP "esp"
# define PFE_i386_ESI "esi"
# define PFE_i386_EDI "edi"
# endif
/*
 * The i386 is register-starved - plus many opcodes are restricted to
 * one of the "general purpose" registers (which they are not for real).
 * The EAX/EDX is the return value, along with ECX they are scratch
 * registers that do not need to be saved but which make them unavailable
 * for global register assignments for the forth machine.
 * 
 * If compiled as a shared library / dll then we see that some register
 * is reserved for the dll-block pointer. In linux 2.4 that was the
 * EBX pointer and we see similar things for other ABI systems. If
 * we can use --fomit-frame-pointer then we can take that one as a
 * global register but in many C compiler versions it just means that
 * the compiler will generate different assembler snippets to use one
 * of the scratch registers as the local frame pointer - so it does not
 * help much in terms of optimization.
 *
 * If you sum up the items above then you see that EAX/EBX/ECX/EDX are
 * unavailable in linux 2.4 and ESI/EDI used for --fbuiltin operations
 * including struct-copy using an invisible memcpy operation. Plus some
 * compiler versions generating bad code for EBP usage. So, what's left?
 * It simply says there is no safe choice of register variables on i386.
 *
 * Btw, gforth does not have a similar problem since all executions are
 * actually part of a single one assembler procedure using a case-label to 
 * adress the execution code while pfe compiles each execution into a separate
 * subroutine. It allows to extend the system dynamically with new
 * executions via C-compiled plugins which gforth is unable to support.
 * Yet, in pfe we have to use cross-procedure global registers thereby
 * trying to get away with the host operating system ABI (binary interface).
 *
 * As for the segment registers, the CS,DS,SS are reserved for code,
 * data and stack. The ES is used along with ESI/EDI builtin operations.
 * The FS is used in windows for the PCB (process control block). The gcc 
 * will use it for the TLS (GS in 32bit and FS in 64bit).
 */

/* gcc 2.95.x is broken !!! it will just mark global-regs as not being
 * in use for a scratch-register - which makes him believe that one can
 * save the thing to the stack and reuse it, even across calls. #$%&!
 * If you need performance on ia32-gcc, use gcc 2.8.x, here we will
 * just take care to be atleast multithreaded. It seems that gcc 2.95.x
 * does not like the %ebp register for some other reasons. Then again,
 * who cares, we have a need for it, so we can use it here 
 */

/* register %esi/%edi is used for many builtin operations in gcc 2.9x 
 * (e.g. strcpy), so may be one has to use -fno-builtin with gcc 2.9x.
 */
#if (__GNUC__ == 2 && __GNUC_MINOR__ >= 9)
#  define PFE_DO_OMIT_EDI
#  define PFE_DO_OMIT_ESI
#endif

#if PFE_HOST_ARCH_I386_64+0
#  define PFE_DO_OMIT_EDI
#  define PFE_DO_OMIT_ESI
#  define PFE_CAN_USE_R15
#  define PFE_CAN_USE_R14
#  define PFE_CAN_USE_R13
#  define PFE_CAN_USE_R12
#endif

/* %ebx is used by shared-lib handling on linux - avoid it */
# if defined HOST_WIN32 || ! defined PFE_ENABLE_SHARED
# define PFE_CAN_USE_EBX
# endif

/* -fomit-frame-pointer interferes heavily with the needed gcc 
 *  extensions __builtin_frame_pointer /__builtin_return_addres 
 */
# if defined PFE_SBR_CALL_THREADING && ! defined PFE_SBR_CALL_ARG_PREFIXING
# define PFE_DO_OMIT_EBP
/* #else _execution frame given through arg (in accumulator) */
# endif

# if defined HOST_OS_AIX1 && !defined PFE_DO_OMIT_EBP
# define PFE_DO_OMIT_EBP
# endif

# if defined __newstuff__
# if !defined PFE_DO_OMIT_EBP && !defined PFE_OMIT_FRAME_POINTER
# define PFE_DO_OMIT_EBP
# endif
# endif

# ifndef PFE_DO_OMIT_EBP
# define PFE_CAN_USE_EBP  /* needs -fomit-frame-pointer */
# endif

# ifndef PFE_DO_OMIT_EDI
# define PFE_CAN_USE_EDI  /* needs -fomit-frame-pointer */
# endif

# ifndef PFE_DO_OMIT_ESI
# define PFE_CAN_USE_ESI  /* needs -fomit-frame-pointer */
# endif

/* the register-starved i386 architecture needs a loooong ifdef series */
#  if PFE_USE_REGS >= 1
#    if   ! defined PFE_NEED_NO_REGTH
#      if   defined PFE_CAN_USE_R15
#             undef PFE_CAN_USE_R15
#            define PFE_ASM_USE_R15 "TH"
#            define P4_REGTH       "%r15"
#      elif defined PFE_CAN_USE_EBP
#             undef PFE_CAN_USE_EBP
#            define PFE_ASM_USE_EBP "TH"
#            define P4_REGTH       "%"PFE_i386_EBP
#      elif defined PFE_CAN_USE_EBX
#             undef PFE_CAN_USE_EBX
#            define PFE_ASM_USE_EBX "TH"
#            define P4_REGTH       "%"PFE_i386_EBX
#      elif defined PFE_CAN_USE_EDI
#             undef PFE_CAN_USE_EDI
#            define PFE_ASM_USE_EDI "TH"
#            define P4_REGTH       "%"PFE_i386_EDI
#      else
#      error no cpu register found for regTH
#      endif
#    elif ! defined PFE_NEED_NO_REGIP
#      if   defined PFE_CAN_USE_R15
#             undef PFE_CAN_USE_R15
#            define PFE_ASM_USE_R15 "IP"
#            define P4_REGIP       "%r15"
#      elif defined PFE_CAN_USE_EBP
#             undef PFE_CAN_USE_EBP
#            define PFE_ASM_USE_EBP "IP"
#            define P4_REGIP       "%"PFE_i386_EBP
#      elif defined PFE_CAN_USE_EBX
#             undef PFE_CAN_USE_EBX
#            define PFE_ASM_USE_EBX "IP"
#            define P4_REGIP       "%"PFE_i386_EBX
#      elif defined PFE_CAN_USE_ESI
#             undef PFE_CAN_USE_ESI
#            define PFE_ASM_USE_ESI "IP"
#            define P4_REGIP       "%"PFE_i386_ESI
#      endif
#    else
#      if   defined PFE_CAN_USE_R14
#             undef PFE_CAN_USE_R14
#            define PFE_ASM_USE_R14 "SP"
#            define P4_REGSP       "%r14"
#      elif defined PFE_CAN_USE_EBP
#             undef PFE_CAN_USE_EBP
#            define PFE_ASM_USE_EBP "SP"
#            define P4_REGSP       "%"PFE_i386_EBP
#      elif defined PFE_CAN_USE_EBX
#             undef PFE_CAN_USE_EBX
#            define PFE_ASM_USE_EBX "SP"
#            define P4_REGSP       "%"PFE_i386_EBX
#      elif defined PFE_CAN_USE_ESI
#             undef PFE_CAN_USE_ESI
#            define PFE_ASM_USE_ESI "SP"
#            define P4_REGSP       "%"PFE_i386_ESI
#      endif
#    endif
#  endif

#  if PFE_USE_REGS >= 2
#    if   ! defined PFE_NEED_NO_REGIP && !defined P4_REGIP
#      if   defined PFE_CAN_USE_R15
#             undef PFE_CAN_USE_R15
#            define PFE_ASM_USE_R15 "IP"
#            define P4_REGIP       "%r15"
#      elif defined PFE_CAN_USE_EBX
#             undef PFE_CAN_USE_EBX
#            define PFE_ASM_USE_EBX "IP"
#            define P4_REGIP       "%"PFE_i386_EBX
#      elif defined PFE_CAN_USE_ESI
#             undef PFE_CAN_USE_ESI
#            define PFE_ASM_USE_ESI "IP"
#            define P4_REGIP       "%"PFE_i386_ESI
#      endif
#    elif ! defined PFE_NEED_NO_REGSP && !defined P4_REGSP
#      if   defined PFE_CAN_USE_R14
#             undef PFE_CAN_USE_R14
#            define PFE_ASM_USE_R14 "SP"
#            define P4_REGSP       "%r14"
#      elif defined PFE_CAN_USE_EBX
#             undef PFE_CAN_USE_EBX
#            define PFE_ASM_USE_EBX "SP"
#            define P4_REGSP       "%"PFE_i386_EBX
#      elif defined PFE_CAN_USE_ESI
#             undef PFE_CAN_USE_ESI
#            define PFE_ASM_USE_ESI "SP"
#            define P4_REGSP       "%"PFE_i386_ESI
#      endif
#    elif ! defined PFE_NEED_NO_REGRP && !defined P4_REGRP
#      if   defined PFE_CAN_USE_R13
#             undef PFE_CAN_USE_R13
#            define PFE_ASM_USE_R13 "RP"
#            define P4_REGRP       "%r13"
#      elif defined PFE_CAN_USE_EBX
#             undef PFE_CAN_USE_EBX
#            define PFE_ASM_USE_EBX "RP"
#            define P4_REGRP       "%"PFE_i386_EBX
#      elif defined PFE_CAN_USE_ESI
#             undef PFE_CAN_USE_ESI
#            define PFE_ASM_USE_ESI "RP"
#            define P4_REGRP       "%"PFE_i386_ESI
#      endif
#    endif
#  endif

#  if PFE_USE_REGS >= 3
#    if   ! defined PFE_NEED_NO_REGSP && !defined P4_REGSP
#      if   defined PFE_CAN_USE_R14
#             undef PFE_CAN_USE_R14
#            define PFE_ASM_USE_R14 "SP"
#            define P4_REGSP       "%r14"
#      elif defined PFE_CAN_USE_EDI
#             undef PFE_CAN_USE_EDI
#            define PFE_ASM_USE_EDI "SP"
#            define P4_REGSP       "%"PFE_i386_EDI
#      elif defined PFE_CAN_USE_ESI
#             undef PFE_CAN_USE_ESI
#            define PFE_ASM_USE_ESI "SP"
#            define P4_REGSP       "%"PFE_i386_ESI
#      endif
#    elif ! defined PFE_NEED_NO_REGRP && !defined P4_REGRP
#      if   defined PFE_CAN_USE_R13
#             undef PFE_CAN_USE_R13
#            define PFE_ASM_USE_R13 "RP"
#            define P4_REGRP       "%r13"
#      elif defined PFE_CAN_USE_EDI
#             undef PFE_CAN_USE_EDI
#            define PFE_ASM_USE_EDI "RP"
#            define P4_REGRP       "%"PFE_i386_EDI
#      elif defined PFE_CAN_USE_ESI
#            define PFE_ASM_USE_ESI "RP"
#             undef PFE_CAN_USE_ESI
#            define P4_REGRP       "%"PFE_i386_ESI
#      endif
#    endif
#  endif

#if (__GNUC__ == 3 && __GNUC_MINOR__ >= 3) && \
    (defined PFE_ASM_USE_ESI || defined PFE_ASM_USE_EDI)
#  define PFE_AVOID_BUILTIN_MEMCPY
#endif

/* _________________________________________________  ARCH_POWERPC _____ */
# elif defined HOST_OS_AIX3 || defined HOST_CPU_POWERPC \
    || defined __target_os_aix3 || defined __target_cpu_powerpc \
    || defined HOST_ARCH_POWERPC
#   define PFE_HOST_ARCH_POWERPC 1

#   define P4_REGTH "14"
#   define P4_REGSP "15"
#   define P4_REGRP "16" /* also used in sbr-threading! */
#   define P4_REGIP "17"
#  if !defined _K12_SOURCE && PFE_USE_REGS > 2
#   define P4_REGW  "18"
#   define P4_REGLP "19"
#   define P4_REGFP "20"
#  endif


/* ____________________________________________________  ARCH_M68K _____ */
# elif defined HOST_CPU_M68K || defined __target_cpu_m68k \
    || defined HOST_ARCH_M68K
#   define PFE_HOST_ARCH_M68K 1

#   define P4_REGTH "%a4" 
#   define P4_REGSP "%a5"
#  if !defined _K12_SOURCE && PFE_USE_REGS > 2
#   if !defined PFE_NEED_NO_REGIP
#   define P4_REGIP "%a6"
#   endif
#   if !defined PFE_NEED_NO_REGRP
#   define P4_REGRP "%a7"
#   endif
#   if !defined PFE_NEED_NO_REGW
#   define P4_REGW  "%d6" 
#   endif
#   if !defined PFE_NEED_NO_REGRP
#   define P4_REGLP "%d7" 
#   endif
#  endif


/* _________________________________________________  ARCH_I960 ________ */
# elif defined HOST_CPU_I960 || defined __target_cpu_i960 \
    || defined HOST_ARCH_I960
#   define PFE_HOST_ARCH_I960 1

#  define P4_UPPER_REGS  /* i960 has register based calling - if you use too */
#  define P4_REGTH "g10" /* many args, these registers will be clobbered - */
#  define P4_REGSP "g11" /* this is actually a gcc-bug, where the system */
#  define P4_REGIP "g12" /* call sequence generator doesn't save these regs */

/* _________________________________________________  ARCH_HPPA ________ */
# elif defined HOST_CPU_HPPA1 || defined HOST_CPU_HPPA \
    || defined HOST_ARCH_HPPA || defined __target_cpu_hppa
#   define PFE_HOST_ARCH_HPPA 1

#   define P4_REGTH "%r12"
#   define P4_REGSP "%r13"
#   define P4_REGRP "%r14"
#   define P4_REGIP "%r15"
#   define P4_REGW  "%r16"
#   define P4_REGLP "%r17"
#   define P4_REGFP "%r18"

/* _________________________________________________  ARCH_SPARC _______ */
# elif defined HOST_CPU_SPARC || defined __target_cpu_sparc \
    || defined HOST_ARCH_SPARC
#   define PFE_HOST_ARCH_SPARC 1
#   define P4_REGTH  "%g2"
#   define P4_REGSP  "%g3"

/* _________________________________________________  SOME ULTRIX ______ */
# elif defined HOST_OS_ULTRIX || defined __target_os_ultrix

#   define P4_REGTH "$17"
#   define P4_REGSP "$18"
#   define P4_REGRP "$19"
#   define P4_REGIP "$20"
#   define P4_REGW  "$21"
#   define P4_REGLP "$22"
#   define P4_REGFP "$23"

/* _________________________________________________  SOME OSF1 ________ */
# elif defined HOST_OS_OSF1 || defined __target_os_osf1

#   define P4_REGTH "$9"
#   define P4_REGSP "$10"
#   define P4_REGRP "$11"
#   define P4_REGIP "$12"
#   define P4_REGW  "$13"
#   define P4_REGLP "$14"
#   define P4_REGFP "$15"

/* _________________________________________________  UNKNOWN ARCH _____ */
# else

#   undef PFE_USE_REGS

# endif
#endif

/* ---------------------------------------------------- END ARCH -------- */
/* ----- CLEAR EASY ARCHs ----------------------------------------------- */


#ifdef PFE_NEED_NO_REGTH
# undef P4_REGTH
#endif

#ifdef PFE_NEED_NO_REGW
# undef P4_REGW
#endif

#ifdef PFE_NEED_NO_REGIP
# undef P4_REGIP
#endif

#ifdef PFE_NEED_NO_REGRP
# undef P4_REGRP
#endif


/*@}*/
#endif 
