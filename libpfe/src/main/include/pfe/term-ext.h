#ifndef PFE_TERM_EXT_H
#define PFE_TERM_EXT_H 1256214395
/* generated 2009-1022-1426 make-header.py ../../c/term-ext.c */

#include <pfe/pfe-ext.h>

/**
 * --   terminal i/o, system independent parts
 *
 *  Copyright (C) Tektronix, Inc, 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.8 $
 *     (modified $Date: 2008-10-07 02:35:39 $)
 *
 *  @description
 *       this wordset exports words to talk to the terminal driver
 *       of forth. Depending on the active terminal driver, not
 *       all of these words are fully functional.
 */

#ifdef __cplusplus
extern "C" {
#endif




/** SHOW-TERM-CONTROLS ( -- ) for debugging
 * show the current mappings for the terminal output
 * may give hints about what is wrong if the output
 * seems to miss some functionality
 */
extern void FXCode (p4_show_control_strings);

/** SHOW-TERM-ESC-KEYS ( -- ) for debugging
 * show the current mappings for the terminal input
 * may give hints about what is wrong if the input
 * seems to miss some functionality
 */
extern void FXCode (p4_show_rawkey_strings);

/** SHOW-TERMCAP ( -- ) for debugging
 * print the termcap strings used for input and output
 * may give hints about what is wrong if the terminal
 * seems to miss some functionality
 */
extern void FXCode (p4_show_termcap);

/** ASSUME_VT100 ( -- )
 * load hardwired VT100-termcap into the terminal-driver
 */
extern void FXCode (p4_assume_vt100);

/** ASSUME_DUMBTERM ( -- )
 * load hardwired DUMBTERM-termcap into the terminal-driver
 */
extern void FXCode (p4_assume_dumbterm);

/** GOTOXY ( x y -- )
 * move the cursor to the specified position on the screen -
 * this is usually done by sending a corresponding esc-sequence
 * to the terminal.
 */
extern void FXCode (p4_gotoxy);

/** ?XY ( -- x y )
 * returns the cursor position on screen, on a real unix system
 * this includes a special call to the screen driver, in remote
 * systems this can be the expected position as seen on the
 * client side's terminal driver.
 */
extern void FXCode (p4_question_xy);

/** EKEY>FKEY ( key-code# -- key-code# 0 | fkey-code# true! )
 * If the input ekey value was not an extended key
 * then flag is set to FALSE and the value is left
 * unchanged. Compare to EKEY>CHAR for the inverse.
 *
 * If the input eky was an extended key then the value
 * will be modified such that shifted values are transposed
 * to their base EKEY plus => K-SHIFT-MASK - therefore the
 * K-SHIFT-MASK is only apropriate for the result fkey-code
 * values of this function.
 */
extern void FXCode(p4_ekey_to_fkey);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
