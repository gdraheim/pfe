/**
 *  Copyright (C) 2000 - 2001 Guido U. Draheim
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author$)
 *  @version $Revision$
 *     (modified $Date$)
 *
 * @description
 *      "struct" implements neon/mops/mpe-like structures.
 *      "structs" implements fsl/mforth-like structures.
 *
 *      the two wordsets are designed to let the sub-words
 *      to be used interchangably both inside STRUCT and
 *      STRUCTURE definitions. They will also work inside
 *      pfe's class-definitions btw.
 *
 *      The provided words try to be compatible
 *      with the simple implementation guidelines as
 *      provided in the survey at the comp.lang.forth.repository
 *      (http://forth.sourceforge.net/word/structure)
 *      and the documentation on MPE' forth's implementation
 *      (/vol/c/Programme/PfwVfx/Doc/VfxMan.Htm/struct.html)
 *      and the structs-source of the Forth Scientific Library
 *      (lib/fsl/structs.fth)
 *
 *   field-layout
 *      PFA[0] has the offset (elsewhere for the method-table)
 *      PFA[1] has the sizeof (may serve as minimalistic type-id)
 *
 *   struct-layout
 *      PFA[0] unused (elswehere method-table or type-id)
 *      PFA[1] has the sizeof (that is instantiated)
 *
 *   therefore SIZEOF is designed to give a nice result in
 *   both places.
 */

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/struct-ext.h>

extern void FXCode (p4_offset_RT);

/* ----------------------------------------------------------------
 * the first part is the traditional stuff, and here's what
 * comes from the Forth Scientific Library - it's implemented
 * widely, including swiftforth
 */

/* CHAR: ( struct-offset "name" -- struct-offset' )
 : CHAR: CHAR FIELD ;
 */
void FXCode (p4_char_colon)
{
    p4_field (sizeof(p4char));
}

/* CELL: ( struct-offset "name" -- struct-offset' )
 : CELL: ALIGNED CELL FIELD ;
 */
void FXCode (p4_cell_colon)
{
    *SP = P4_ALIGNED(*SP);
    p4_field (sizeof(p4cell));
}

/* WCHAR: ( struct-offset "name" -- struct-offset' )
 : WCHAR: WALIGNED WCHAR FIELD ;
 */
void FXCode (p4_wchar_colon)
{
    *SP += *SP & 1;
    p4_field (sizeof(p4char) * 2);
}

/* DOUBLE: ( struct-offset "name" -- struct-offset' )
 : DOUBLE: ALIGNED DOUBLE FIELD ;
 */
void FXCode (p4_two_cell_colon)
{
    *SP = P4_ALIGNED(*SP);
    p4_field (sizeof(p4cell) * 2);
}

/* CHARS: ( struct-offset "name" -- struct-offset' )
 : CHARS: CHARS FIELD ;
 */
void FXCode (p4_chars_colon)
{
    p4_field (sizeof(p4char) * FX_POP);
}

/* CELLS: ( struct-offset "name" -- struct-offset' )
 : CELLS: CELLS SWAP ALIGNED SWAP FIELD ;
 */
void FXCode (p4_cells_colon)
{
    SP[1] = P4_ALIGNED(SP[1]);
    p4_field (sizeof(p4cell) * FX_POP);
}

/* WCHARS: ( struct-offset "name" -- struct-offset' )
 : WCHARS: WCHARS FIELD ;
 */
void FXCode (p4_wchars_colon)
{
    SP[1] += SP[1] & 1;
    p4_field (sizeof(p4char) * FX_POP);
}

/* FLOAT: ( struct-offset "name" -- struct-offset' )
 : FLOAT: FLOATS SWAP FALIGNED SWAP FIELD ;
 */
void FXCode (p4_float_colon)
{
    SP[1] = P4_SFALIGNED(SP[1]);
    p4_field (sizeof(double) * FX_POP);
}

P4_LISTWORDSET (structs) [] =
{
    P4_INTO ("EXTENSIONS", 0),
    P4_FXco ("STRUCTURE:",		p4_structure),
    P4_FXco (";STRUCTURE",		p4_endstructure),

    P4_FXco ("CHAR:",			p4_char_colon),
    P4_FXco ("WCHAR:",			p4_wchar_colon),
    P4_FXco ("CELL:",			p4_cell_colon),
    P4_FXco ("DOUBLE:",			p4_two_cell_colon),
    P4_FXco ("FLOAT:",			p4_float_colon),

    P4_FXco ("CHARS:",			p4_chars_colon),
    P4_FXco ("WCHARS:",			p4_wchars_colon),
    P4_FXco ("CELLS:",			p4_cells_colon),

    P4_FXco ("INTEGER:",		p4_cell_colon),
    P4_FXco ("POINTER:",		p4_cell_colon),
    P4_FXco ("STRUCT:",			p4_field),
    P4_FXco ("ARRAY:",			p4_array_of),
};
P4_COUNTWORDSET (structs, "STRUCTS - simple structure implementation 0");
