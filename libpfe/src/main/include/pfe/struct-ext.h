#ifndef PFE_STRUCT_EXT_H
#define PFE_STRUCT_EXT_H 1256209149
/* generated 2009-1022-1259 make-header.py ../../c/struct-ext.c */

#include <pfe/pfe-ext.h>

/**
 *  Copyright (C) 2000 - 2001 Guido U. Draheim
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2008-04-20 04:46:29 $)
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

#ifdef __cplusplus
extern "C" {
#endif




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
extern P4_CODE (p4_field);

/** SIZEOF ( "name" -- size )
 * get the size-value from a previous structure definition
 : SIZEOF   ' >BODY @  STATE @ IF [COMPILE] LITERAL THEN ; IMMEDIATE
 */
extern P4_CODE (p4_sizeof_XT);

extern P4_CODE (p4_sizeof);

/** STRUCTURE ( "name" -- here zero-offset ) exec
 * start a structure definition
 : STRUCTURE: CREATE !CSP
   HERE
   0 DUP ,
 DOES>
   CREATE @ ALLOT
 ;
 */
extern P4_CODE (p4_structure_RT);

extern P4_CODE (p4_structure);

/** ENDSTRUCTURE ( here some-offset -- )
 * finalize a previously started => STRUCTURE definition
 : ENDSTRUCTURE  SWAP !  ?CSP ;
 */
extern P4_CODE (p4_endstructure);

/** STRUCT ( "name" -- here zero-offset )
 * begin definition of a new structure (mpe.000)
 : STRUCT CREATE  !CSP
   HERE
   0 DUP ,
 DOES>
   @
 ;
 */
extern P4_CODE (p4_struct);

/** END-STRUCT ( here some-offset -- )
 * terminate definition of a new structure (mpe.000)
 : END-STRUCT  SWAP !  ?CSP ;
 */
extern P4_CODE (p4_end_struct);

/** SUBRECORD ( outer-offset "name" -- outer-offset here zero-offset )
 * begin definition of a subrecord (mpe.000)
 : STRUCT CREATE
   HERE
   0 DUP ,
 DOES>
   @
 ;
 */
extern P4_CODE (p4_subrecord);

/** END-SUBRECORD ( outer-offset here some-offset -- outer-offset+some )
 * end definition of a subrecord (mpe.000)
 : END-SUBRECORD  TUCK SWAP !  + ;
 */
extern P4_CODE (p4_end_subrecord);

/** ARRAY-OF ( some-offset n len "name" -- some-offset )
 * a =>"FIELD"-array
 : ARRAY-OF * FIELD ;
 */
extern P4_CODE (p4_array_of);

/** VARIANT ( outer-offset "name" -- outer-offset here zero-offset )
 * Variant records describe an alternative view of the
 * current record or subrecord from the start to the current point.
 * The variant need not be of the same length, but the larger is taken
 : VARIANT SUBRECORD ;
 */
extern P4_CODE (p4_variant);

/** END-VARIANT ( outer-offset here some-offset -- outer-offset )
 * terminate definition of a new variant (mpe.000)
 : END-STRUCT  TUCK SWAP !  2DUP < IF NIP ELSE DROP THEN ;
 */
extern P4_CODE (p4_end_variant);

/** INSTANCE ( len "name" -- )
 * Create a named instance of a named structure.
 : INSTANCE  CREATE ALLOT ;
 */
extern P4_CODE (p4_instance);

/** INSTANCE-ADDR ( len -- addr )
 * Create nameless instance of a structure and return base address.
 : INSTANCE-ADDR  HERE SWAP ALLOT ;
 */
extern P4_CODE (p4_instance_addr);

extern P4_CODE (p4_char_mod);

extern P4_CODE (p4_cell_mod);

extern P4_CODE (p4_wchar_mod);

extern P4_CODE (p4_double_mod);

extern P4_CODE (p4_float_mod);

extern P4_CODE (p4_sfloat_mod);

extern P4_CODE (p4_dfloat_mod);

_extern  void p4_field(p4cell size) ; /*{*/

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
