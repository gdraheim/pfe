/**
 * (C) 2000 - 2001 Guido Draheim
 *
 * lisence: GNU LGPL, non-substantial parts may promote to 
 *                    any opensource.org approved license.
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

extern FCode (p4_offset_RT);

_export void
p4_field(p4cell size)
{
    p4_header (PFX(p4_offset_RT), 0);
    FX_COMMA (*SP);
    FX_COMMA (size);
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
FCode (p4_field)
{
    p4_field (FX_POP);
}

/** SIZEOF ( "name" -- size ) 
 * get the size-value from a previous structure definition
 : SIZEOF   ' >BODY @  STATE @ IF [COMPILE] LITERAL THEN ; IMMEDIATE
 */
FCode (p4_sizeof_RT)
{
    /* well, we could have compiled the sizeof at compile-time
     * and just use p4_literal_execution. But with the xt in here
     * we will get a nice decompile-result with the struct-name
     */
    FX_PUSH (TO_BODY(P4_POP(IP))[1]);
}
FCode (p4_sizeof)
{
    p4xt xt;
    p4_tick(&xt);
    if (STATE)
    {
	FX_COMPILE(p4_sizeof);
	FX_COMMA (xt);
    }else{
	FX_PUSH (TO_BODY(xt)[1]);
    }
}
P4COMPILES(p4_sizeof, p4_sizeof_RT,
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
FCode (p4_structure_RT)
{
    FX (p4_create);
    FX_ALLOT (WP_PFA[1]);
}
FCode (p4_structure)
{
    FX (p4_Q_exec);
    p4_header (PFX(p4_structure_RT), 0);
    FX (p4_store_csp);
    FX_COMMA (0); /* unused here */
    FX_HERE;
    FX_COMMA (0);
    FX_PUSH (0);
}


/** ENDSTRUCTURE ( here some-offset -- )
 * finalize a previously started => STRUCTURE definition
 : ENDSTRUCTURE  SWAP !  ?CSP ;
 */
FCode (p4_endstructure)
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
FCode (p4_struct)
{
    FX (p4_Q_exec);
    p4_header (PFX(p4_constant_RT), 0);
    FX (p4_store_csp);
    FX_COMMA (0); /* unused here */
    FX_HERE;
    FX_COMMA (0);
    FX_PUSH (0);
}

/** END-STRUCT ( here some-offset -- )
 * terminate definition of a new structure (mpe.000)
 : END-STRUCT  SWAP !  ?CSP ;
 */
FCode (p4_end_struct)
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
FCode (p4_subrecord)
{
    FX (p4_struct);
}

/** END-SUBRECORD ( outer-offset here some-offset -- outer-offset+some )
 * end definition of a subrecord (mpe.000)
 : END-SUBRECORD  TUCK SWAP !  + ;
 */
FCode (p4_end_subrecord)
{
    *(p4cell *)SP[1] = SP[0];
    SP[2] += SP[0];
    SP += 2;
}

/** ARRAY-OF ( some-offset n len "name" -- some-offset )
 * a =>"FIELD"-array
 : ARRAY-OF * FIELD ;
 */
FCode (p4_array_of)
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
FCode (p4_variant)
{
    FX (p4_struct);
}

/** END-VARIANT ( outer-offset here some-offset -- outer-offset )
 * terminate definition of a new variant (mpe.000)
 : END-STRUCT  TUCK SWAP !  2DUP < IF NIP ELSE DROP THEN ;
 */
FCode (p4_end_variant)
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
FCode (p4_instance)
{
    FX (p4_create);
    DP += FX_POP;
}

/** INSTANCE-ADDR ( len -- addr )
 * Create nameless instance of a structure and return base address. 
 : INSTANCE-ADDR  HERE SWAP ALLOT ;
 */
FCode (p4_instance_addr)
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
FCode (p4_char_mod)
{
    FX_PUSH (sizeof(p4char));
}

/* CELL% ( struct-offset -- struct-offset' sizeof-cell )
 : CELL% ALIGNED CELL  ;
 */
FCode (p4_cell_mod)
{
    *SP = P4_ALIGNED(*SP);
    FX_PUSH (sizeof(p4cell));
}

/* WCHAR% ( struct-offset -- struct-offset' sizeof-wchar )
 : WCHAR% WALIGNED WCHAR ;
 */
FCode (p4_wchar_mod)
{
    *SP += *SP & 1;
    FX_PUSH (sizeof(p4char) * 2);
}

/* DOUBLE% ( struct-offset -- struct-offset' sizeof-double )
 : DOUBLE% ALIGNED DOUBLE ;
 */
FCode (p4_double_mod)
{
    *SP = P4_ALIGNED(*SP);
    FX_PUSH (sizeof(p4cell) * 2);
}

/* FLOAT% ( struct-offset -- struct-offset' float-size )
 : FLOAT% FALIGNED FLOAT ;
 */
FCode (p4_float_mod)
{
    SP[1] = P4_SFALIGNED(SP[1]);
    FX_PUSH (sizeof(float));
}

/* SFLOAT% ( struct-offset -- struct-offset' sfloat-size )
 : SFLOAT% SFALIGNED SFLOAT ;
 */
FCode (p4_sfloat_mod)
{
    SP[1] = P4_SFALIGNED(SP[1]);
    FX_PUSH (sizeof(float));
}

/* DFLOAT% ( struct-offset "name" -- struct-offset' dfloat-size )
 : DFLOAT% DFALIGNED DFLOAT ;
 */
FCode (p4_dfloat_mod)
{
    SP[1] = P4_DFALIGNED(SP[1]);
    FX_PUSH (sizeof(double));
}

P4_LISTWORDS(struct) =
{
    /* NEON-MOPS-MPE variant */
    CO ("STRUCT",		p4_struct),
    CO ("END-STRUCT",		p4_end_struct),
    CO ("FIELD",		p4_field),
    CO ("SUBRECORD",		p4_subrecord),
    CO ("END-SUBRECORD",	p4_end_subrecord),
    CO ("ARRAY-OF",		p4_array_of),
    CO ("VARIANT",		p4_variant),
    CO ("END-VARIANT",		p4_end_variant),
    CO ("INSTANCE",		p4_instance),
    CO ("INSTANCE-ADDR",	p4_instance_addr),

    /* traditional wording */
    CO ("STRUCTURE",		p4_structure),
    CO ("ENDSTRUCTURE",		p4_endstructure),
    CS ("SIZEOF",		p4_sizeof),

    /* gforth compatibility */
    CO ("CHAR%",		p4_char_mod),
    CO ("CELL%",		p4_cell_mod),
    CO ("WCHAR%",		p4_wchar_mod),
    CO ("DOUBLE%",		p4_double_mod),
    CO ("FLOAT%",		p4_float_mod),
    CO ("SFLOAT%",		p4_sfloat_mod),
    CO ("DFLOAT%",		p4_dfloat_mod),
};
P4_COUNTWORDS(struct, "STRUCT - simple struct implementation");

P4_LOADLIST (struct) =
{
    P4_LOAD_INTO, "EXTENSIONS",
    &P4WORDS(struct),
    P4_LOAD_END
};
P4_MODULE_LIST (struct);

/*
 * Local variables:
 * c-file-style: "stroustrup"
 * End:
 */
