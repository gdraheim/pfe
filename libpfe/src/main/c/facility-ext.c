/**
 * FACILITY --- The Optional Facility Word Set
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author$)
 *  @version $Revision$
 *     (modified $Date$)
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
"@(#) $Id$";
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
void FXCode (p4_at_x_y)
{
    p4_gotoxy (SP[1], SP[0]);
    SP += 2;
}

/** KEY? ( -- key-flag ) [ANS]
 * if a character is available from the keyboard, return true.
 * The => KEY word will retrieve the actual character.
 */
void FXCode (p4_key_question)
{
    FX_PUSH (P4_FLAG (p4_keypressed ()));
}

/* Facility Extension Words */

/** EKEY ( -- key-code# ) [ANS]
 * return a keyboard event, the encoding may differ, esp.
 * that it can contain special keys.
 */
void FXCode (p4_ekey)
{
    FX_PUSH (p4_getekey ());
}

/** EKEY>CHAR ( key-code# -- key-code# 0 | char# true! ) [ANS]
 */
void FXCode (p4_ekey_to_char)
{
    --SP;
    SP[0] = P4_FLAG ((p4ucell) SP[1] < 0x100);
}

/** EKEY? ( -- ekey-flag ) [ANS]
 * check if a character is available from the keyboard
 * to be received - unlike => KEY? it will not discard
 * non-visible codes.
 */
void FXCode (p4_ekey_question)
{
    FX_PUSH (P4_FLAG (p4_ekeypressed ()));
}

/** EMIT? ( -- emit-flag ) [ANS]
 * if => EMIT can safely output characters without
 * blocking the forth by waiting for an indefinite time.
 */
void FXCode (p4_emit_question)
{
    FX_PUSH (P4_TRUE);
}

/** MS ( milliseconds# -- ) [ANS]
 * wait at least the specified milliseconds
 * (suspend the forth tasklet)
 */
void FXCode (p4_ms)
{
    p4_delay (*SP++);
}

/** TIME&amp;DATE ( -- sec# min# hrs# day# month# year# ) [ANS]
 * return the broken down current time
 */
void FXCode (p4_time_and_date)
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

extern void FXCode(p4_dot_clrscr);
#define p4_dot_clrscr_ p4_dot_clrscr

/* ======================= Forth200x ========================== */

static P4_CODE_RUN(p4_add_field_RT_SEE)
{
    p4_strcat (p, p4_str_dot (*P4_TO_BODY (xt), p+200, BASE));
    p4_strcat (p, "+FIELD ");
    p4_strncat (p, (char*) NAMEPTR(nfa), NAMELEN(nfa));
    return 0;
}

void FXCode_RT (p4_add_field_RT)
{
    FX_USE_BODY_ADDR;
    *SP += FX_POP_BODY_ADDR[0];
}

/** +FIELD ( n1 n2 "name" -- n3 )
 * Skip leading space delimiters. Parse name delimited by a space.
 * Create a definition for name with the execution semantics defined
 * below. Return n3=n1+n2 where n1 is the offset in the data
 * structure before +FIELD executes, and n2 is the size of the data
 * to be added to the data structure. N1 and n2 are in address units.
 *
 * Execution ( addr -- addr+n1 )
 * Add n1 from the execution of +FIELD above to addr.
 */
void FXCode (p4_add_field)
{
    FX_RUNTIME_HEADER;
    FX_RUNTIME1 (p4_add_field);
    FX_UCOMMA (SP[1]);
    SP[1] += SP[0]; FX_DROP;
}
P4RUNTIMES1_(p4_add_field, p4_add_field_RT, 0, p4_add_field_RT_SEE);

/** BEGIN-STRUCTURE ( "name" -- struct-sys 0 )
 * Skip leading space delimiters. Parse name delimited by a space.
 * Create a definition for name with the execution semantics defined
 * below. Return a struct-sys (zero or more implementation dependent
 * items) that will be used by => END-STRUCTURE and an initial offset of 0.
 *
 * Execution: ( -- +n )
 * +n is the size in memory expressed in adress units of the data
 * structure.
 *
 * Ambiguous conditions:
 * If name is executed before the closing => END-STRUCTURE has been
 * executed.
 *
 * Implentation is like
 * start a structure definition
 : BEGIN-STRUCTURE CREATE !CSP
   HERE
   0 DUP ,
 DOES>
   CREATE @ ALLOT
 ;
 */
void FXCode_RT (p4_begin_structure_RT)
{   FX_USE_BODY_ADDR;
    FX_POP_BODY_ADDR_p4_BODY;
    FX (p4_create_var);
    FX_ALLOT (p4_BODY[1]);
}
void FXCode (p4_begin_structure)
{
    FX (p4_Q_exec);
    FX_RUNTIME_HEADER;
    FX_RUNTIME1(p4_begin_structure);
    FX (p4_store_csp);
    FX_PCOMMA (0);     /* unused here */
    FX_PUSH (p4_HERE); /* adress of... */
    FX_UCOMMA (0);     /* sizeof value */
    FX_PUSH (0);       /* initial offset */
}
P4RUNTIME1(p4_begin_structure, p4_begin_structure_RT);


/** END-STRUCTURE ( struct-sys +n -- )
 * Terminate definition of a structure started by => BEGIN-STRUCTURE
 * This will finalize a previously started => BEGIN-STRUCTURE definition
 : ENDSTRUCTURE  SWAP !  ?CSP ;
 */
void FXCode (p4_end_structure)
{
    *(p4cell *)SP[1] = SP[0];
    SP += 2;
    FX (p4_Q_csp);
}

/** CFIELD: ( struct-sys offset "name" -- struct-sys offset+x )
 * The semantics of CFIELD: are identical to the execution
 * semantics of the phrase
  1 CHARS +FIELD
 */
void FXCode (p4_c_field_colon)
{
	FX_PUSH(sizeof(p4_char_t));
	FX (p4_add_field);
}

/** FIELD: ( struct-sys offset "name" -- struct-sys offset+x )
 * The semantics of FIELD: are identical to the execution
 * semantics of the phrase
  ALIGNED  1 CELLS +FIELD
 */
void FXCode (p4_u_field_colon)
{
	while (! P4_ALIGNED(*SP)) *SP += 1;
	FX_PUSH(sizeof(p4_cell_t));
	FX (p4_add_field);
}

/** FFIELD: ( struct-sys offset "name" -- struct-sys offset+x )
 * The semantics of FFIELD: are identical to the execution
 * semantics of the phrase
  FALIGNED  1 FLOATS +FIELD
 * see => DFFIELD:
 */


/** SFFIELD:  ( struct-sys offset "name" -- struct-sys offset+x )
 * The semantics of SFFIELD: are identical to the execution
 * semantics of the phrase
  SFALIGNED  1 SFLOATS +FIELD
 */
void FXCode (p4_sf_field_colon)
{
	while (! P4_SFALIGNED(*SP)) *SP += 1;
	FX_PUSH(sizeof(float));
	FX (p4_add_field);
}

/** DFFIELD: ( struct-sys offset "name" -- struct-sys offset+x )
 * The semantics of DFFIELD: are identical to the execution
 * semantics of the phrase
  DFALIGNED  1 DFLOATS +FIELD
 * see => DFFIELD:
 */
void FXCode (p4_df_field_colon)
{
	while (! P4_DFALIGNED(*SP)) *SP += 1;
	FX_PUSH(sizeof(double));
	FX (p4_add_field);
}

P4_LISTWORDSET (facility) [] =
{
    P4_INTO ("[ANS]", 0),
    P4_FXco ("AT-XY",		    p4_at_x_y),
    P4_FXco ("KEY?",		    p4_key_question),
    P4_FXco ("PAGE",		    p4_dot_clrscr),
    P4_FXco ("EKEY",		    p4_ekey),
    P4_FXco ("EKEY>CHAR",	    p4_ekey_to_char),
    P4_FXco ("EKEY?",		    p4_ekey_question),
    P4_FXco ("EMIT?",		    p4_emit_question),
    P4_FXco ("MS",		        p4_ms),
    P4_FXco ("TIME&DATE",	    p4_time_and_date),
    /* ----- Forth200x */
    P4_RTco ("+FIELD",          p4_add_field),
    P4_RTco ("BEGIN-STRUCTURE", p4_begin_structure),
    P4_FXco ("END-STRUCTURE",   p4_end_structure),
    P4_FXco ("CFIELD:",         p4_c_field_colon),
    P4_FXco ("FIELD:",          p4_u_field_colon),
    P4_FXco ("SFFIELD:",        p4_sf_field_colon),
    P4_FXco ("DFFIELD:",        p4_df_field_colon),
    P4_FNYM ("FFIELD:",         "DFFIELD:"),

    P4_INTO ("ENVIRONMENT",	0 ),
    P4_OCON ("FACILITY-EXT",	1994 ),
};
P4_COUNTWORDSET (facility, "Facility + extensions");

/*@}*/
