#ifndef _PFE_FACILITY_EXT_H
#define _PFE_FACILITY_EXT_H 1158897468
/* generated 2006-0922-0557 ../../pfe/../mk/Make-H.pl ../../pfe/facility-ext.c */

#include <pfe/pfe-ext.h>

/** 
 * FACILITY --- The Optional Facility Word Set
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2006 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2006-09-22 04:43:03 $)
 *
 *  @description
 *      There are the following primitive words for input and output:
 *
 *      => KEY waits for a character typed on the keyboard and returns
 *      that character, but => KEY does not return non-character
 *      input events like function keys pressed - use => EKEY for 
 *      a more complete keyboard query.
 *      Furthermore, => KEY? returns true if a key is available for
 *      reading using => KEY (function key presses are not detected
 *      but discarded).
 *
 *      => EMIT will display the character at the current cursor
 *      position, control characters take effect depending on the
 *      system. => TYPE displays all the chars in the given string 
 *      buffer.
 *
 *      To get the current cursor position, use =>'AT-XY'.
 */

#ifdef __cplusplus
extern "C" {
#endif




/** AT-XY ( col# row# -- ) [ANS]
 * move the cursor position to the given row and column
 * of the screen. If the output device is not a terminal
 * this will have no effect but can still send an
 * escape sequence.
 */
extern P4_CODE (p4_at_x_y);

/** KEY? ( -- key-flag ) [ANS]
 * if a character is available from the keyboard, return true.
 * The => KEY word will retrieve the actual character.
 */
extern P4_CODE (p4_key_question);

/** EKEY ( -- key-code# ) [ANS]
 * return a keyboard event, the encoding may differ, esp.
 * that it can contain special keys.
 */
extern P4_CODE (p4_ekey);

/** EKEY>CHAR ( key-code# -- key-code# 0 | char# true! ) [ANS]
 */
extern P4_CODE (p4_ekey_to_char);

/** EKEY? ( -- ekey-flag ) [ANS]
 * check if a character is available from the keyboard
 * to be received - unlike => KEY? it will not discard
 * non-visible codes.
 */
extern P4_CODE (p4_ekey_question);

/** EMIT? ( -- emit-flag ) [ANS]
 * if => EMIT can safely output characters without
 * blocking the forth by waiting for an indefinite time.
 */
extern P4_CODE (p4_emit_question);

/** MS ( milliseconds# -- ) [ANS]
 * wait at least the specified milliseconds
 * (suspend the forth tasklet)
 */
extern P4_CODE (p4_ms);

/** TIME&amp;DATE ( -- sec# min# hrs# day# month# year# ) [ANS]
 * return the broken down current time
 */
extern P4_CODE (p4_time_and_date);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
