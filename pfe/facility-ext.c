/** 
 * FACILITY --- The Optional Facility Word Set
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2008-04-20 04:46:29 $)
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
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: facility-ext.c,v 1.3 2008-04-20 04:46:29 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-limits.h>
#include <pfe/os-delay.h>

#include <time.h>

#include <pfe/term-sub.h>
#include <pfe/_missing.h>

/** AT-XY ( col# row# -- ) [ANS]
 * move the cursor position to the given row and column
 * of the screen. If the output device is not a terminal
 * this will have no effect but can still send an
 * escape sequence.
 */
FCode (p4_at_x_y)
{
    p4_gotoxy (SP[1], SP[0]);
    SP += 2;
}

/** KEY? ( -- key-flag ) [ANS]
 * if a character is available from the keyboard, return true.
 * The => KEY word will retrieve the actual character.
 */
FCode (p4_key_question)
{
    FX_PUSH (P4_FLAG (p4_keypressed ()));
}

/* Facility Extension Words */

/** EKEY ( -- key-code# ) [ANS]
 * return a keyboard event, the encoding may differ, esp.
 * that it can contain special keys.
 */
FCode (p4_ekey)
{
    FX_PUSH (p4_getekey ());
}

/** EKEY>CHAR ( key-code# -- key-code# 0 | char# true! ) [ANS]
 */
FCode (p4_ekey_to_char)
{
    --SP;
    SP[0] = P4_FLAG ((p4ucell) SP[1] < 0x100);
}

/** EKEY? ( -- ekey-flag ) [ANS]
 * check if a character is available from the keyboard
 * to be received - unlike => KEY? it will not discard
 * non-visible codes.
 */
FCode (p4_ekey_question)
{
    FX_PUSH (P4_FLAG (p4_ekeypressed ()));
}

/** EMIT? ( -- emit-flag ) [ANS]
 * if => EMIT can safely output characters without
 * blocking the forth by waiting for an indefinite time.
 */
FCode (p4_emit_question)
{
    FX_PUSH (P4_TRUE);
}

/** MS ( milliseconds# -- ) [ANS]
 * wait at least the specified milliseconds
 * (suspend the forth tasklet)
 */
FCode (p4_ms)
{
    p4_delay (*SP++);
}

/** TIME&amp;DATE ( -- sec# min# hrs# day# month# year# ) [ANS]
 * return the broken down current time
 */
FCode (p4_time_and_date)
{
    time_t t;
    struct tm *tm;
    
    time (&t);
    tm = localtime (&t);
    SP -= 6;
    SP[5] = tm->tm_sec;
    SP[4] = tm->tm_min;
    SP[3] = tm->tm_hour;
    SP[2] = tm->tm_mday;
    SP[1] = tm->tm_mon + 1;
    SP[0] = tm->tm_year + 1900;
}

/** PAGE ( -- ) [ANS]
 * => CLRSCR
 */

extern FCode(p4_dot_clrscr);
#define p4_dot_clrscr_ p4_dot_clrscr

P4_LISTWORDS (facility) =
{
    P4_INTO ("[ANS]", 0),
    P4_FXco ("AT-XY",		p4_at_x_y),
    P4_FXco ("KEY?",		p4_key_question),
    P4_FXco ("PAGE",		p4_dot_clrscr),
    P4_FXco ("EKEY",		p4_ekey),
    P4_FXco ("EKEY>CHAR",	p4_ekey_to_char),
    P4_FXco ("EKEY?",		p4_ekey_question),
    P4_FXco ("EMIT?",		p4_emit_question),
    P4_FXco ("MS",		p4_ms),
    P4_FXco ("TIME&DATE",	p4_time_and_date),

    P4_INTO ("ENVIRONMENT",	0 ),
    P4_OCON ("FACILITY-EXT",	1994 ),
};
P4_COUNTWORDS (facility, "Facility + extensions");

/*@}*/

