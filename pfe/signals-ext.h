#ifndef _PFE_SIGNALS_EXT_H
#define _PFE_SIGNALS_EXT_H 984413844
/* generated 2001-0312-1717 ../../pfe/../mk/Make-H.pl ../../pfe/signals-ext.c */

#include <pfe/incl-ext.h>

/** 
 * -- Handle signals in forth
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE            @(#) %derived_by: guidod %
 *  @version %version: 5.9 %
 *    (%date_modified: Mon Mar 12 10:32:41 2001 %)
 *
 *  @description
 *  Signal-Klassen, die ich in PFE SIGNALS extension aufmache:
 *
 *  <dl>
 *  <dt>P4_ON_XXXXX:</dt><dd>
 *   	ein Signal dem direkt ein bestimmter THROW in Forth
 *	entspricht, z.B. SIGFPE.
 *  </dd>
 *  <dt>Abort:</dt><dd>		
 *	ein Signal, das uns nicht gleich tötet, dem aber kein
 *	THROW entspricht, z.B. SIGILL. Es führt zu einem
 *	ABORT" Signalbeschreibung"
 *   </dd>
 *  <dt>Fatal:</dt><dd>
 *	wir sterben gracefully.
 *  </dd>
 *  <dt>Default:</dt><dd>
 *	Ein Signal, mit dem ich nichts anfangen kann.
 *	Wenn z.B. ein Forth-Programmierer und AIX weiß,
 *	was SIGGRANT bedeutet, kann er ein Forth-Wort drauf
 *	setzen, ansonsten bleibt dieses Signal unberührt. 
 *  </dd>
 *  <dt>Chandled:</dt><dd>
 *	Ein Signal, das pfe intern braucht und behandelt,
 *	konkret Job-Control und SIGWINCH, wenn xterm die
 *	Größe ändert.
 *  </dd>
 *  </dl>
 *
 * Auf den ersten drei Typen sitzt der default Forth Signal-Handler.
 * Dessen Aktion ist, gucken, ob Forth-Routine registriert ist (dann
 * ausführen) oder nicht (dann throw, abort" oder tot).
 *
 * Auf den vierten Typ wird der Forth-Signal-Handler erst dann
 * installiert, wenn der User ein Forth-Wort draufschaltet, ansonsten
 * bleibt es bei der System-default-Aktion.
 *
 * Auf den vierten Typ wird unter keinen Umständen der default
 * Forth-Signal-Handler geschaltet. Also können da auch keine Forth-Worte
 * drauf. Wenn ich das alles jetzt noch richtig verstehe :-)
 *                             <p align=right> Dirk-Uwe Zoller </p>
 */

#ifdef __cplusplus
extern "C" {
#endif




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
