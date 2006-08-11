#ifndef __PFE_DEF_GCC_H
#define __PFE_DEF_GCC_H

/*
 *  Copyright (C) 2005 - 2006 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2006-08-11 22:56:04 $)
 */

#ifdef __GNUC__
#define P4_GCC_ATLEAST(__M,__N) (__GNUC__+0 >= __M && __GNUC_MINOR__+0 >= __N)
#else
#define P4_GCC_ATLEAST(__M,__N)
#endif

/** ATLEAST(2,2) means we do not know the first version it came about */

#if P4_GCC_ATLEAST(3,2)
#define P4_GCC_DEPRACATED __attribute__((__deprecated__))
#define P4_GCC_DEPRECATED __attribute__((__deprecated__))
#else
#define P4_GCC_DEPRACATED
#define P4_GCC_DEPRECATED
#endif

#if P4_GCC_ATLEAST(2,5)
#define P4_GCC_NORETURN __attribute__((__noreturn__))
#else
#define P4_GCC_NORETURN
#endif

#if P4_GCC_ATLEAST(2,5)
#define P4_GCC_NORETURN __attribute__((__noreturn__))
#else
#define P4_GCC_NORETURN
#endif

#if P4_GCC_ATLEAST(2,2)
#define P4_GCC_NOINLINE __attribute__((__noinline__))
#else
#define P4_GCC_NOINLINE
#endif

#if P4_GCC_ATLEAST(2,96)
#define P4_GCC_PURE __attribute__((__pure__))
#else
#define P4_GCC_PURE
#endif

#if P4_GCC_ATLEAST(2,5)
#define P4_GCC_CONST __attribute__((__const__))
#else
#define P4_GCC_CONST
#endif

#if P4_GCC_ATLEAST(3,2)
#define P4_GCC_NOTHROW __attribute__((__nothrow__))
#else
#define P4_GCC_NOTHROW
#endif

#if P4_GCC_ATLEAST(2,2)
#define P4_GCC_PRINTF __attribute__((format (__printf__,1,2)))
#define P4_GCC_FPRINTF __attribute__((format (__printf__,2,3)))
#define P4_GCC_SPRINTF __attribute__((format (__printf__,2,3)))
#define P4_GCC_SNPRINTF __attribute__((format (__printf__,3,4)))
#else
#define P4_GCC_PRINTF
#define P4_GCC_FPRINTF
#define P4_GCC_SPRINTF
#define P4_GCC_SNPRINTF
#endif

#if P4_GCC_ATLEAST(2,2)
#define P4_GCC_NO_INSTRUMENT_FUNCTION __attribute__((__no_instrument_function__))
#else
#define P4_GCC_NO_INSTRUMENT_FUNCTION
#endif

#if P4_GCC_ATLEAST(2,2)
#define P4_GCC_UNUSED __attribute__((__unused__))
#else
#define P4_GCC_UNUSED
#endif

#if P4_GCC_ATLEAST(2,2)
#define P4_GCC_USED __attribute__((__used__))
#else
#define P4_GCC_USED
#endif

#if P4_GCC_ATLEAST(3,4)
#define P4_GCC_WARN_UNUSED_RESULT __attribute__((__warn_unused_result__))
#else
#define P4_GCC_WARN_UNUSED_RESULT
#endif

#if P4_GCC_ATLEAST(2,96)
#define P4_GCC_MALLOC __attribute__((__malloc__))
#else
#define P4_GCC_MALLOC
#endif

#if P4_GCC_ATLEAST(2,2)
#define P4_GCC_SIGNAL __attribute__((__signal__))
#else
#define P4_GCC_SIGNAL
#endif

#if P4_GCC_ATLEAST(2,2)
#define P4_GCC_WEAK __attribute__((__weak__))
#else
#define P4_GCC_WEAK
#endif

#if P4_GCC_ATLEAST(2,2)
#define P4_GCC_DLLEXPORT __attribute__((__dllexport__))
#else
#define P4_GCC_DLLEXPORT
#endif

#endif
