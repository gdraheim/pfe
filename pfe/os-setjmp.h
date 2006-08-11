#ifndef __PFE_OS_SETJMP_H
#define __PFE_OS_SETJMP_H

/*
 *  Copyright (C) 2005 - 2006 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.2 $
 *     (modified $Date: 2006-08-11 22:56:05 $)
 *
 * There is a lot of confusion about the implementation of signal()
 * and longjmp(). I had to notice a problem in latest linux implementations
 * which will kill signal-hooks upon a longjmp out of a signal-handler.
 * <p>
 * The net result: a SIGBUS can only be caught once (with forth CATCH)
 * while the second one will fail as the signal-handler was not reloaded
 * and the system signal handler will receive the next system exception.
 * <p>
 * Doing a few tests I had to notice that the only way to get around this
 * is about the usage of sigsetjmp/siglongjmp with a nonzero second argument.
 * According to the manpage:
 *   sigsetjmp() is similar to setjmp().  If savesigs is nonzero, the set of
 *   blocked signals is saved in env and will be restored if a  siglongjmp()
 *   is later performed with this env.
 * <p>
 * It seems that some signal-service implementations have interpreted that
 * to save the signal-hook table. The real bits are still unknown to me
 * but... it seems to me that setjmp(x) is identical to sigsetjmp(x,0)
 * which creates a sigjmp_buf with a signal-hook save-table filled with
 * the defaults as given in the system. Whatever signal-hooks we have been 
 * setting before the catch-domain and whatever reloading has been done
 * inside the signal-handler does not matter anymore as they are set back
 * to the default assignments: we would essentially need to reload the
 * signal-hooks before/after each single catch-domain/setjmp.
 * <p>
 * The better solution: whenever we find sigsetjmp to exist in the system
 * environment then always use that one and let it save the signal-hooks.
 */

#include <pfe/def-config.h>
#include <setjmp.h>

#ifdef PFE_HAVE_SIGSETJMP
#define PFE_USE_SIGSETJMP 1
#ifndef sigsetjmp
#ifdef     setjmp
#ifdef __GNUC__
#warning no sigsetjmp in this modus (may need to set -D_POSIX_SOURCE)
#else
#error   no sigsetjmp in this modus (may need to set -D_POSIX_SOURCE)
/* atleast glibc headers do not define sigsetjmp in "gcc -ansi" mode */
#endif
#endif
#endif
#endif

#if PFE_USE_SIGSETJMP+0
#define p4_jmp_buf sigjmp_buf
#define p4_setjmp(_buf) sigsetjmp((_buf), 1)
#define p4_longjmp(_buf,_val) siglongjmp((_buf), (_val))
#else
#define p4_jmp_buf jmp_buf
#define p4_setjmp(_buf) setjmp((_buf), 1)
#define p4_longjmp(_buf,_val) longjmp((_buf), (_val))
#endif

/* once */
#endif
