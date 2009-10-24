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

static P4_CODE_RUN(p4_field_colon_RT_SEE)
{
    p4_strcat (p, p4_str_dot (*P4_TO_BODY (xt), p+200, BASE));
    p4_strcat (p, "FIELD ");
    p4_strncat (p, (char*) NAMEPTR(nfa), NAMELEN(nfa));
    return 0;
}

void FXCode_RT (p4_field_colon_RT)
{
    FX_USE_BODY_ADDR;
    *SP += FX_POP_BODY_ADDR[0];
}

/** "field" is required also in the STRUCTS implementation
 * (may be obsoleted with Forth200x structs)
 */
void
p4_field_colon(p4cell size)
{
    FX_RUNTIME_HEADER;
    FX_RUNTIME1 (p4_field_colon);
    FX_UCOMMA (*SP);
    FX_UCOMMA (size);
    *SP += size;
}

/** FIELD ( offset size "name" -- offset+size )
 * create a field - the workhorse for both => STRUCT and => STRUCTURE
 * implementations. The created fieldname is an =>"OFFSET:"-word
 * that memorizes the current offset in its PFA and will add
 * that offset on runtime. This forth-word does *not* align.
 : FIELD CREATE
   OVER ,
   +
 DOES>
    @ +
 ;
 */
void FXCode (p4_field_colon)
{
    p4_field_colon (FX_POP);
}
P4RUNTIME1(p4_field_colon, p4_field_colon_RT);

/** SIZEOF ( "name" -- size )
 * get the size-value from a previous structure definition
 : SIZEOF   ' >BODY @  STATE @ IF [COMPILE] LITERAL THEN ; IMMEDIATE
 */
void FXCode_XE (p4_sizeof_colon_XT)
{
    FX_USE_CODE_ADDR;
    /* well, we could have compiled the sizeof at compile-time
     * and just use p4_literal_execution. But with the xt in here
     * we will get a nice decompile-result with the struct-name
     */
    FX_PUSH (P4_TO_BODY(P4_POP(IP))[1]); /* == P4_TO_DOES_BODY */
    FX_USE_CODE_EXIT;
}

void FXCode (p4_sizeof_colon)
{
    p4xt xt = p4_tick_cfa (FX_VOID);
    if (STATE)
    {
        FX_COMPILE(p4_sizeof_colon);
        FX_XCOMMA (xt);
    }else{
        FX_PUSH (P4_TO_BODY(xt)[1]); /* == P4_TO_DOES_BODY */
    }
}
P4COMPILES(p4_sizeof_colon, p4_sizeof_colon_XT,
           P4_SKIPS_TO_TOKEN, P4_DEFAULT_STYLE);

/** STRUCTURE: ( "name" -- here zero-offset ) exec
 * start a structure definition
 : STRUCTURE: CREATE !CSP
   HERE
   0 DUP ,
 DOES>
   CREATE @ ALLOT
 ;
 */
void FXCode_RT (p4_structure_colon_RT)
{   FX_USE_BODY_ADDR;
    FX_POP_BODY_ADDR_p4_BODY;
    FX (p4_create_var);
    FX_ALLOT (p4_BODY[1]);
}
void FXCode (p4_structure_colon)
{
    FX (p4_Q_exec);
    FX_RUNTIME_HEADER;
    FX_RUNTIME1(p4_structure_colon);
    FX (p4_store_csp);
    FX_PCOMMA (0);     /* unused here */
    FX_PUSH (p4_HERE); /* adress of... */
    FX_UCOMMA (0);     /* sizeof value */
    FX_PUSH (0);       /* initial offset */
}
P4RUNTIME1(p4_structure_colon, p4_structure_colon_RT);

/** ;ENDSTRUCTURE ( here some-offset -- )
 * finalize a previously started => STRUCTURE definition
 : ENDSTRUCTURE  SWAP !  ?CSP ;
 */
void FXCode (p4_endstructure_colon)
{
    *(p4cell *)SP[1] = SP[0];
    SP += 2;
    FX (p4_Q_csp);
}

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
    p4_field_colon (sizeof(p4char));
}

/* CELL: ( struct-offset "name" -- struct-offset' )
 : CELL: ALIGNED CELL FIELD ;
 */
void FXCode (p4_cell_colon)
{
    *SP = P4_ALIGNED(*SP);
    p4_field_colon (sizeof(p4cell));
}

/* WCHAR: ( struct-offset "name" -- struct-offset' )
 : WCHAR: WALIGNED WCHAR FIELD ;
 */
void FXCode (p4_wchar_colon)
{
    *SP += *SP & 1;
    p4_field_colon (sizeof(p4char) * 2);
}

/* DOUBLE: ( struct-offset "name" -- struct-offset' )
 : DOUBLE: ALIGNED DOUBLE FIELD ;
 */
void FXCode (p4_two_cell_colon)
{
    *SP = P4_ALIGNED(*SP);
    p4_field_colon (sizeof(p4cell) * 2);
}

/* CHARS: ( struct-offset "name" -- struct-offset' )
 : CHARS: CHARS FIELD ;
 */
void FXCode (p4_chars_colon)
{
    p4_field_colon (sizeof(p4char) * FX_POP);
}

/* CELLS: ( struct-offset "name" -- struct-offset' )
 : CELLS: CELLS SWAP ALIGNED SWAP FIELD ;
 */
void FXCode (p4_cells_colon)
{
    SP[1] = P4_ALIGNED(SP[1]);
    p4_field_colon (sizeof(p4cell) * FX_POP);
}

/* WCHARS: ( struct-offset "name" -- struct-offset' )
 : WCHARS: WCHARS FIELD ;
 */
void FXCode (p4_wchars_colon)
{
    SP[1] += SP[1] & 1;
    p4_field_colon (sizeof(p4char) * FX_POP);
}

/* FLOAT: ( struct-offset "name" -- struct-offset' )
 : FLOAT: FLOATS SWAP FALIGNED SWAP FIELD ;
 */
void FXCode (p4_float_colon)
{
    SP[1] = P4_SFALIGNED(SP[1]);
    p4_field_colon (sizeof(double) * FX_POP);
}

/** ARRAY: ( some-offset n len "name" -- some-offset )
 * a =>"FIELD"-array
 : ARRAY: * STRUCT: ;
 */
void FXCode (p4_array_colon)
{
    p4_field (SP[0]*SP[1]);
    SP += 2;
}

P4_LISTWORDSET (structs) [] =
{
    P4_INTO ("EXTENSIONS", 0),
    P4_FXco ("STRUCTURE:",		p4_structure_colon),
    P4_FXco (";STRUCTURE",		p4_endstructure_colon),
    P4_SXco ("SIZEOF:",			p4_sizeof_colon),
    P4_SNYM ("SIZEOF",          "SIZEOF:"),

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
    P4_FXco ("STRUCT:",			p4_field_colon),
    P4_FXco ("ARRAY:",			p4_array_colon),
};
P4_COUNTWORDSET (structs, "STRUCTS - simple structure implementation 0");
