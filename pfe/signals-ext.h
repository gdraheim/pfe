#ifndef _VOL_8_SRC_CVS_PFE_33_PFE_SIGNALS_EXT_H
#define _VOL_8_SRC_CVS_PFE_33_PFE_SIGNALS_EXT_H 1209868837
/* generated 2008-0504-0440 /vol/8/src/cvs/pfe-33/pfe/../mk/Make-H.pl /vol/8/src/cvs/pfe-33/pfe/signals-ext.c */

#include <pfe/pfe-ext.h>

/** 
 * -- Handle signals in forth
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.5 $
 *     (modified $Date: 2008-05-04 02:57:30 $)
 *
 *  @description
 *  The signal-callback system divides signals internally
 *  into the following classes:
 *
 *  <dl>
 *  <dt>P4_ON_XXXXX:</dt><dd>
 *      a signal which will be assigned a corresponding THROW
 *      on forth level, e.g. for SIGFPE
 *  </dd>
 *  <dt>Abort:</dt><dd>		
 *      a signal that will not kill the current forth process
 *      but which has not forth-level THROW either, e.g. SIGILL.
 *      It will result in an ABORT" Signal-Description"
 *   </dd>
 *  <dt>Fatal:</dt><dd>
 *	the current forth process will die gracefully.
 *  </dd>
 *  <dt>Default:</dt><dd>
 *      A signal with some unknown meaning, exported to allow
 *      a forth-programmer to hook it anyway, e.g. to let
 *      a programmer on AIX to intercept SIGGRANT and run
 *      a forth word, otherwise the signal will be left untouched.
 *  </dd>
 *  <dt>Chandled:</dt><dd>
 *      A signal used internally be PFE and initially hooked
 *      by the runtime system, in general this would be the
 *      Job-Control signals and SIGWINCH that will get send 
 *      when an xterm changes its size.
 *  </dd>
 *  </dl>
 *
 * The first three classes will go to the default Forth Signal-Handler.
 * Its execution will look for user-routine being registered (in 
 * which cases that forth-routine will be executed) and otherwise
 * do its default action (to throw, abort" or exit).
 *
 * The fourth class is not hooked until some user-code requests that
 * signal in which case the user-defiend forth-routine is executed
 * as its action, otherwise the system-defined default-action will
 * be left untouched.
 *
 * The signal of the fourth type are not handled by the default
 * signal handler defined herein, so can not just call a
 * forth word, if I still get this right ;-)
 *                             <p align=right> Dirk-Uwe Zoller </p>
 *
 * note: forth-level callbacks might not work as expected on
 *       all systems that the rest of PFE runs on. Be careful.
 *                             <p align=right> Guido U. Draheim </p>
 */

#ifdef __cplusplus
extern "C" {
#endif




/** (RAISE) ( signal# -- ) [FTH]
 * send a => SIGNAL to self
 * OLD: use RAISE-SIGNAL
 */
extern P4_CODE (p4_raise);

/** RAISE-SIGNAL ( signal# -- ior ) [FTH]
 */
extern P4_CODE (p4_raise_signal);

/** FORTH-SIGNAL ( handler-xt* signal# -- old-signal-xt* ) [FTH]
 * install signal handler
 * - return old signal handler
 */
extern P4_CODE (p4_forth_signal);

/** 
 * the signals-constructor will declare the available
 * system signals as contants - usually sth. like
 * => SIGALRM or => SIGHUP or => SIGABRT
 * <p>
 * some signals are only valid in specific systems,
 * like => SIGBREAK or => SIGMSG or => SIGVIRT
 */
extern P4_CODE (p4_load_signals);

/**
 * install all signal handlers:
 */
_extern  void p4_install_signal_handlers (void) ; /*{*/

/**
 * switch between p4th setting of signals and state before 
 */
_extern  void p4_swap_signals (void) ; /*{*/

/**
 * xt != NULL: install forth word as signal handler for signal
 * xt == NULL: install p4th default signal handler for signal
 */
_extern  p4xt p4_forth_signal (int sig, p4xt xt) ; /*{*/

/**
 * Load constants for each signal found into the dictionary.
 */
_extern  void p4_load_signals (p4_Wordl *wid) ; /*{*/

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
