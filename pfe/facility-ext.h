#ifndef _PFE_FACILITY_EXT_H
#define _PFE_FACILITY_EXT_H 984413842
/* generated 2001-0312-1717 ../../pfe/../mk/Make-H.pl ../../pfe/facility-ext.c */

#include <pfe/incl-ext.h>

/** 
 * FACILITY --- The Optional Facility Word Set
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 5.8 %
 *    (%date_modified: Mon Mar 12 10:32:17 2001 %)
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




/** AT-XY ( col row -- )
 * move the cursor position to the given row and column
 * of the screen. If the output device is not a terminal
 * this will have no effect but can still send an
 * escape sequence.
 */
extern P4_CODE (p4_at_x_y);

/** KEY? ( -- flag )
 * if a character is available from the keyboard, return true.
 * The => KEY word will retrieve the actual character.
 */
extern P4_CODE (p4_key_question);

/** EKEY ( -- keycode )
 * return a keyboard event, the encoding may differ, esp.
 * that it can contain special keys.
 */
extern P4_CODE (p4_ekey);

/** EKEY>CHAR ( keycode -- keycode false | char true )
 */
extern P4_CODE (p4_ekey_to_char);

/** EKEY? ( -- flag )
 * check if a character is available from the keyboard
 * to be received - unlike => KEY? it will not discard
 * non-visible codes.
 */
extern P4_CODE (p4_ekey_question);

/** EMIT? ( -- flag )
 * if => EMIT can safely output characters without
 * blocking the forth by waiting for an indefinite time.
 */
extern P4_CODE (p4_emit_question);

/** MS ( n -- )
 * wait at least the specified milliseconds
 * (suspend the forth tasklet)
 */
extern P4_CODE (p4_ms);

/** TIME&amp;DATE ( -- sec min hrs day month year )
 * return the broken down current time
 */
extern P4_CODE (p4_time_and_date);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
