#ifndef _VOL_8_SRC_CVS_PFE_33_PFE_TERM_EXT_H
#define _VOL_8_SRC_CVS_PFE_33_PFE_TERM_EXT_H 1209868837
/* generated 2008-0504-0440 /vol/8/src/cvs/pfe-33/pfe/../mk/Make-H.pl /vol/8/src/cvs/pfe-33/pfe/term-ext.c */

#include <pfe/pfe-ext.h>

/** 
 * --   terminal i/o, system independent parts
 * 
 *  Copyright (C) Tektronix, Inc, 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.5 $
 *     (modified $Date: 2008-05-04 02:57:30 $)
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
extern P4_CODE (p4_show_control_strings);

/** SHOW-TERM-ESC-KEYS ( -- ) for debugging
 * show the current mappings for the terminal input
 * may give hints about what is wrong if the input
 * seems to miss some functionality
 */
extern P4_CODE (p4_show_rawkey_strings);

/** SHOW-TERMCAP ( -- ) for debugging
 * print the termcap strings used for input and output
 * may give hints about what is wrong if the terminal
 * seems to miss some functionality
 */
extern P4_CODE (p4_show_termcap);

/** ASSUME_VT100 ( -- )
 * load hardwired VT100-termcap into the terminal-driver
 */
extern P4_CODE (p4_assume_vt100);

/** ASSUME_DUMBTERM ( -- )
 * load hardwired DUMBTERM-termcap into the terminal-driver
 */
extern P4_CODE (p4_assume_dumbterm);

/** GOTOXY ( x y -- )
 * move the cursor to the specified position on the screen -
 * this is usually done by sending a corresponding esc-sequence
 * to the terminal. 
 */
extern P4_CODE (p4_gotoxy);

/** ?XY ( -- x y )
 * returns the cursor position on screen, on a real unix system
 * this includes a special call to the screen driver, in remote
 * systems this can be the expected position as seen on the
 * client side's terminal driver.
 */
extern P4_CODE (p4_question_xy);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
