#ifndef __PFE_OS_SETENV_H
#define __PFE_OS_SETENV_H

/*
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author$)
 *  @version $Revision$
 *     (modified $Date$)
 *
 * This is similar to os-setjmp.h in that is checks for an implentation
 * of mathlib fesetenv() / fegetenv(). These are needed to store the FPU
 * state across setjmp calls - i.e. THROW/CATCH blocks.
 *
 * http://www.opengroup.org/onlinepubs/009695399/functions/fegetenv.html
 */

#if !defined P4_NO_FP && defined(PFE_HAVE_FENV_H)
#include <fenv.h>

#define p4_fegetenv(fenv_buffer)  fegetenv(fenv_buffer)
#define p4_fesetenv(fenv_buffer)  fesetenv(fenv_buffer)
typedef struct {
	int initialized;
	fenv_t fenv;
} p4_fenv_t;

#else

#define p4_fegetenv(fenv_buffer)  0
#define p4_fesetenv(fenv_buffer)  0
typedef struct {
	int initialized;
	unsigned fenv;
} p4_fenv_t;

#endif
#endif
