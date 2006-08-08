/** 
 * -- miscellaneous useful extra words for DOUBLE-EXT
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version:  33.12 %
 *    (%date_modified:  Fri Mar 14 14:44:22 2003 %)
 *
 *  @description
 *      Compatiblity with former standards, miscellaneous useful words.
 *      ... for DOUBLE-EXT  (which is almost complete... just almost)
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: double-mix.c,v 1.1.1.1 2006-08-08 09:07:20 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/double-sub.h>

#define UDP ((p4udcell *)SP)

/** 2ROT ( d1,d1 d2,d2 d3,d3 -- d2,d2 d3,d3 d1,d1 )
 * the double-cell => ROT operation.
 * actively moves six cells, i.e.
   ( x1 x2 x3 x4 x5 x6 -- x3 x4 x5 x6 x1 x2 )
 */
FCode (p4_two_rot)
{
    p4cell h;

    h = SP[4];
    SP[4] = SP[2];
    SP[2] = SP[0];
    SP[0] = h;
    h = SP[5];
    SP[5] = SP[3];
    SP[3] = SP[1];
    SP[1] = h;
}


/** DU< ( d1,d1 d2,d2 -- flag )
 * the double-cell unsigned-is-less operation ( =>"U<" )
 */
FCode (p4_d_u_less)
{
    SP[3] = P4_FLAG (p4_d_u_less (&UDP[1], &UDP[0]));
    SP += 3;
}

/** LITERAL, ( value -- )
 * take the value from stack (or cs-stack) and compile a runtime-code and
 * the value as for => LITERAL ... this word is never state-smart, it
 * is not immediate, and has therefore no complications with => POSTPONE
 * (compare also with =>"COMPILE," to make a call-stub with an exectoken)
 */
FCode (p4_literal_comma)
{
    FX_COMPILE (p4_literal);
    FX_VCOMMA (*SP++);
}

/** 2LITERAL, ( x1,x2 -- )
 * take the double-value from stack (or cs-stack) and compile a runtime-code 
 * and the value as for => 2LITERAL ... this word is never state-smart, it
 * is not immediate, and has therefore no complications with => POSTPONE
 * (compare also with =>"COMPILE," to make a call-stub with an exectoken)
 */
FCode (p4_two_literal_comma)
{
    FX_COMPILE (p4_two_literal);
    FX_VCOMMA (((p4dcell*)SP)->hi);
    FX_VCOMMA (((p4dcell*)SP)->lo);
    FX_2DROP;
}

/** DCELLS ( x -- x' )
 * computes the number of address units for the specified number 
 * of double-cells
 : DCELLS CELLS 2* ;
 */
FCode (p4_dcells)
{
    *SP *= sizeof(p4dcell);
}

/** DLSHIFT ( x1,x2 y -- z1,z2 )
 * shift-left a double-cell value.
 * The shift-count is given as a single-cell.
 */
FCode (p4_d_shiftleft)
{
    int shiftcount = FX_POP;
    p4_d_shiftleft ((p4dcell*)SP, shiftcount);
}

/** DRSHIFT ( x1,x2 y -- z1,z2 )
 * shift-right a double-cell value.
 * The shift-count is given as a single-cell.
 * This is an arithmetic shift as for a signed double-cell value.
 */
FCode (p4_d_shiftright)
{
    int shiftcount = FX_POP;
    p4_d_shiftright ((p4dcell*)SP, shiftcount);
}

P4_LISTWORDS (double_misc) =
{
    P4_INTO ("FORTH", "[ANS]"),

    P4_FXco ("2ROT",         p4_two_rot),
    P4_FXco ("DU<",          p4_d_u_less),

    P4_FXco ("LITERAL,",     p4_literal_comma),
    P4_FXco ("2LITERAL,",    p4_two_literal_comma),

    P4_FXco ("DCELLS",       p4_dcells),
    P4_OCoN ("/DCELL",       sizeof(p4dcell)),

    P4_FXco ("DLSHIFT",     p4_d_shiftleft),
    P4_FXco ("DRSHIFT",     p4_d_shiftright),
};
P4_COUNTWORDS (double_misc, "DOUBLE-Misc Compatibility words");

/*@}*/
/* 
 * Local variables:
 * c-file-style: "stroustrup"
 * End:
 */


