/** 
 * -- Assembler Words for the Optional Programming-Tools Word Set
 *
 *  Copyright (C) 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.5 $
 *     (modified $Date: 2008-05-11 12:48:04 $)
 *
 *  @description
 *      The ANS Forth defines some "Programming Tools" containing
 *      also an ASSEMBLER wordlist plus CODE and ;CODE to create
 *      callable words with a machine-level text inbetween. This
 *      wordset.
 *
 *      Unlike most systems, the PFE is a portable system - therefore
 *      it is asserted that the machine-level words for CODE / END-CODE
 *      are known for the specific machine that this Forth is being
 *      compiled for. Hence you must explicitly require the assembler
 *      to make it working.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: assembler-ext.c,v 1.5 2008-05-11 12:48:04 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-limits.h>
#include <pfe/def-comp.h>
#include <pfe/header-ext.h>
#include <pfe/tools-ext.h>

#define ___ {
#define ____ }

/** CODE ( "name" -- )
 * call => ALSO and add ASSEMBLER wordlist if available. Add PROC ENTER
 * assembler snippet as needed for the architecture into the PFA. The
 * CFA is setup (a) with the PFA adress in traditional ITC or (b)
 * with an infoblock as for sbr-coded colon words.
 * 
 * Remember that not all architectures are support and that the
 * ASSEMBLER wordset is not compiled into pfe by default. Use always
 * the corresponding => END-CODE for each => CODE start. The new
 * word name is not smudged.
 */
FCode (p4_asm_create_code)
{
    FX_HEADER; /* FX_SMUDGED; */
#  if !defined PFE_SBR_CALL_THREADING
    /* indirect threaded */
    ___ p4xcode* dp = (p4xcode*) DP; 
    FX_COMMA (dp+1); ____;
#  else
    FX (p4_colon);
    FX (p4_colon_EXIT);
#  endif
    FX (p4_also); CONTEXT[0] = PFE.assembler_wl;
}

FCode (p4_asm_semicolon_code)
{
# if   !defined PFE_CALL_THREADING
    FX (p4_colon_EXIT);
    ___ p4cell* here = (p4cell*) p4_HERE;
    FX_COMMA(here+1);
    FX_COMMA(here+2); ____;
# elif !defined PFE_SBR_CALL_THREADING
    FX (p4_colon_EXIT);
    ___ p4cell* here = (p4cell*) p4_HERE;
    FX_COMMA(here+1); ____;
# else
    /* nothing - we are already native */
    FX (p4_colon_EXIT);
# endif
    FX (p4_also); CONTEXT[0] = PFE.assembler_wl;
}
P4COMPILES(p4_asm_semicolon_code, p4_semicolon_code_execution,
	   P4_SKIPS_OFFSET, P4_NEW1_STYLE);

/** END-CODE ( "name" -- )
 * call => PREVIOUS and  add PROC LEAVE assembler snippet as needed
 * for the architecture -  usually includes bits to "return from
 * subroutine". Remember that not all architectures are support and
 * PFE usually does only do variants of call-threading with a separate
 * loop for the inner interpreter that does "call into subroutine".
 * Some forth implementations do "jump into routine" and the PROC
 * LEAVE part would do "jump to next routine" also known as 
 * next-threading. The sbr-call-threading is usually similar to the
 * native subroutine-coding of the host operating system. See => CODE
 */
FCode (p4_asm_end_code)
{
    FX (p4_previous); /* kick out ASSEMBLER wordlist */
    PFE_SBR_COMPILE_EXIT (DP);
    PFE.locals = 0;
}

/* missing TOOLS-EXT ASSEMBLER */
/* missing TOOLS-EXT CODE */
/* missing TOOLS-EXT ;CODE */
/* missing TOOLS-EXT EDITOR */

P4_LISTWORDS (assembler) =
{
    P4_INTO ("EXTENSIONS", 0),
    P4_FXco ("CODE",           p4_asm_create_code),    /* redefine "CODE" */
    P4_FXco (";CODE",          p4_asm_semicolon_code), /* redefine ";CODE" */
    P4_INTO ("ASSEMBLER", 0),
    P4_FXco ("END-CODE",       p4_asm_end_code),

    P4_INTO ("ENVIRONMENT", 0 ),
    P4_OCON ("ASSEMBLER-EXT",	1994 ),
};
P4_COUNTWORDS (assembler, "ASSEMBLER Programming-Tools (see TOOLS-EXT)");
