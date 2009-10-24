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
 *      plus some compatibility sugar for the gforth' struct
 *      (gforth/struct.fs)
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

extern void FXCode (p4_add_field_RT);

/** "field" is required also in the STRUCTS implementation
 * (may be obsoleted with Forth200x structs)
 */
void
p4_field(p4cell size)
{
    FX_RUNTIME_HEADER;
    FX_RUNTIME1 (p4_field);
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
void FXCode (p4_field)
{
    p4_field (FX_POP);
}
P4RUNTIME1(p4_field, p4_add_field_RT);

/** SIZEOF ( "name" -- size )
 * get the size-value from a previous structure definition
 : SIZEOF   ' >BODY @  STATE @ IF [COMPILE] LITERAL THEN ; IMMEDIATE
 */
void FXCode_XE (p4_sizeof_XT)
{
    FX_USE_CODE_ADDR;
    /* well, we could have compiled the sizeof at compile-time
     * and just use p4_literal_execution. But with the xt in here
     * we will get a nice decompile-result with the struct-name
     */
    FX_PUSH (P4_TO_BODY(P4_POP(IP))[1]); /* == P4_TO_DOES_BODY */
    FX_USE_CODE_EXIT;
}

void FXCode (p4_sizeof)
{
    p4xt xt = p4_tick_cfa (FX_VOID);
    if (STATE)
    {
        FX_COMPILE(p4_sizeof);
        FX_XCOMMA (xt);
    }else{
        FX_PUSH (P4_TO_BODY(xt)[1]); /* == P4_TO_DOES_BODY */
    }
}
P4COMPILES(p4_sizeof, p4_sizeof_XT,
           P4_SKIPS_TO_TOKEN, P4_DEFAULT_STYLE);

/** STRUCTURE ( "name" -- here zero-offset ) exec
 * start a structure definition
 : STRUCTURE: CREATE !CSP
   HERE
   0 DUP ,
 DOES>
   CREATE @ ALLOT
 ;
 */
void FXCode_RT (p4_structure_RT)
{   FX_USE_BODY_ADDR;
    FX_POP_BODY_ADDR_p4_BODY;
    FX (p4_create_var);
    FX_ALLOT (p4_BODY[1]);
}
void FXCode (p4_structure)
{
    FX (p4_Q_exec);
    FX_RUNTIME_HEADER;
    FX_RUNTIME1(p4_structure);
    FX (p4_store_csp);
    FX_PCOMMA (0);     /* unused here */
    FX_PUSH (p4_HERE); /* adress of... */
    FX_UCOMMA (0);     /* sizeof value */
    FX_PUSH (0);       /* initial offset */
}
P4RUNTIME1(p4_structure, p4_structure_RT);


/** ENDSTRUCTURE ( here some-offset -- )
 * finalize a previously started => STRUCTURE definition
 : ENDSTRUCTURE  SWAP !  ?CSP ;
 */
void FXCode (p4_endstructure)
{
    *(p4cell *)SP[1] = SP[0];
    SP += 2;
    FX (p4_Q_csp);
}

/** STRUCT ( "name" -- here zero-offset )
 * begin definition of a new structure (mpe.000)
 : STRUCT CREATE  !CSP
   HERE
   0 DUP ,
 DOES>
   @
 ;
 */
void FXCode (p4_struct)
{
    FX (p4_Q_exec);
    FX_RUNTIME_HEADER;
    FX_RUNTIME1 (p4_constant); /* fixme? p4_struct_RT */
    FX (p4_store_csp);
    FX_PCOMMA (0);     /* unused here */
    FX_PUSH (p4_HERE); /* address of... */
    FX_UCOMMA (0);     /* sizeof value */
    FX_PUSH (0);       /* initial offset */
}

/** END-STRUCT ( here some-offset -- )
 * terminate definition of a new structure (mpe.000)
 : END-STRUCT  SWAP !  ?CSP ;
 */
void FXCode (p4_end_struct)
{
    *(p4cell *)SP[1] = SP[0];
    SP += 2;
    FX (p4_Q_csp);
}

/** SUBRECORD ( outer-offset "name" -- outer-offset here zero-offset )
 * begin definition of a subrecord (mpe.000)
 : STRUCT CREATE
   HERE
   0 DUP ,
 DOES>
   @
 ;
 */
void FXCode (p4_subrecord)
{
    FX (p4_struct);
}

/** END-SUBRECORD ( outer-offset here some-offset -- outer-offset+some )
 * end definition of a subrecord (mpe.000)
 : END-SUBRECORD  TUCK SWAP !  + ;
 */
void FXCode (p4_end_subrecord)
{
    *(p4cell *)SP[1] = SP[0];
    SP[2] += SP[0];
    SP += 2;
}

/** ARRAY-OF ( some-offset n len "name" -- some-offset )
 * a =>"FIELD"-array
 : ARRAY-OF * FIELD ;
 */
void FXCode (p4_array_of)
{
    p4_field (SP[0]*SP[1]);
    SP += 2;
}

/** VARIANT ( outer-offset "name" -- outer-offset here zero-offset )
 * Variant records describe an alternative view of the
 * current record or subrecord from the start to the current point.
 * The variant need not be of the same length, but the larger is taken
 : VARIANT SUBRECORD ;
 */
void FXCode (p4_variant)
{
    FX (p4_struct);
}

/** END-VARIANT ( outer-offset here some-offset -- outer-offset )
 * terminate definition of a new variant (mpe.000)
 : END-STRUCT  TUCK SWAP !  2DUP < IF NIP ELSE DROP THEN ;
 */
void FXCode (p4_end_variant)
{
    *(p4cell *)SP[1] = SP[0];
    if (SP[2] < SP[0])
        SP[2] = SP[0];
    SP += 2;
}

/** INSTANCE ( len "name" -- )
 * Create a named instance of a named structure.
 : INSTANCE  CREATE ALLOT ;
 */
void FXCode (p4_instance)
{
    FX (p4_create_var);
    DP += FX_POP;
}

/** INSTANCE-ADDR ( len -- addr )
 * Create nameless instance of a structure and return base address.
 : INSTANCE-ADDR  HERE SWAP ALLOT ;
 */
void FXCode (p4_instance_addr)
{
    register p4cell size = *SP;
    *SP = (p4cell) DP;
    DP += size;
}

/* -------------------------------------------------------------
 * gforth-like things
 *
 * these are not the same, since gforth's => CHAR% will leave
 * two values - one for alignment, the other for the size, i.e.
 : CELL% ( -- align size  ) CELL CELL ;
 * while in pfe it will expect the struct-offset on stack
 * that it will align *before* putting the size on stack that
 * will then be consumed by the => FIELD word. The field word
 * does hence not need to know about alignments, nor is the
 * user responsible for calling the correct one explicitly -
 * the name => CELL% means that it will call the correct
 * alignement word in before leaveing => CELL on stack.
 */

/* CHAR% ( struct-offset -- struct-offset' sizeof-char )
 : CHAR% SIZEOF CHAR ;
 */
void FXCode (p4_char_mod)
{
    FX_PUSH (sizeof(p4char));
}

/* CELL% ( struct-offset -- struct-offset' sizeof-cell )
 : CELL% ALIGNED CELL  ;
 */
void FXCode (p4_cell_mod)
{
    *SP = P4_ALIGNED(*SP);
    FX_PUSH (sizeof(p4cell));
}

/* WCHAR% ( struct-offset -- struct-offset' sizeof-wchar )
 : WCHAR% WALIGNED WCHAR ;
 */
void FXCode (p4_wchar_mod)
{
    *SP += *SP & 1;
    FX_PUSH (sizeof(p4char) * 2);
}

/* DOUBLE% ( struct-offset -- struct-offset' sizeof-double )
 : DOUBLE% ALIGNED DOUBLE ;
 */
void FXCode (p4_double_mod)
{
    *SP = P4_ALIGNED(*SP);
    FX_PUSH (sizeof(p4cell) * 2);
}

/* FLOAT% ( struct-offset -- struct-offset' float-size )
 : FLOAT% FALIGNED FLOAT ;
 */
void FXCode (p4_float_mod)
{
    SP[1] = P4_SFALIGNED(SP[1]);
    FX_PUSH (sizeof(float));
}

/* SFLOAT% ( struct-offset -- struct-offset' sfloat-size )
 : SFLOAT% SFALIGNED SFLOAT ;
 */
void FXCode (p4_sfloat_mod)
{
    SP[1] = P4_SFALIGNED(SP[1]);
    FX_PUSH (sizeof(float));
}

/* DFLOAT% ( struct-offset "name" -- struct-offset' dfloat-size )
 : DFLOAT% DFALIGNED DFLOAT ;
 */
void FXCode (p4_dfloat_mod)
{
    SP[1] = P4_DFALIGNED(SP[1]);
    FX_PUSH (sizeof(double));
}

P4_LISTWORDSET (structz) [] =
{
    P4_INTO ("EXTENSIONS", 0),
    /* NEON-MOPS-MPE variant */
    P4_FXco ("STRUCT",			p4_struct),
    P4_FXco ("END-STRUCT",		p4_end_struct),
    P4_RTco ("FIELD",			p4_field),
    P4_FXco ("SUBRECORD",		p4_subrecord),
    P4_FXco ("END-SUBRECORD",		p4_end_subrecord),
    P4_FXco ("ARRAY-OF",		p4_array_of),
    P4_FXco ("VARIANT",			p4_variant),
    P4_FXco ("END-VARIANT",		p4_end_variant),
    P4_FXco ("INSTANCE",		p4_instance),
    P4_FXco ("INSTANCE-ADDR",		p4_instance_addr),

    /* traditional wording */
    P4_RTco ("STRUCTURE",		p4_structure),
    P4_FXco ("ENDSTRUCTURE",		p4_endstructure),
    P4_SXco ("SIZEOF",			p4_sizeof),

    /* gforth compatibility */
    P4_FXco ("CHAR%",			p4_char_mod),
    P4_FXco ("CELL%",			p4_cell_mod),
    P4_FXco ("WCHAR%",			p4_wchar_mod),
    P4_FXco ("DOUBLE%",			p4_double_mod),
    P4_FXco ("FLOAT%",			p4_float_mod),
    P4_FXco ("SFLOAT%",			p4_sfloat_mod),
    P4_FXco ("DFLOAT%",			p4_dfloat_mod),
};
P4_COUNTWORDSET (structz, "STRUCT - simple struct implementation");
