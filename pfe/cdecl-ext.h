#ifndef _PFE_CDECL_EXT_H
#define _PFE_CDECL_EXT_H 1158897467
/* generated 2006-0922-0557 ../../pfe/../mk/Make-H.pl ../../pfe/cdecl-ext.c */

#include <pfe/pfe-ext.h>

/** 
 * -- C-like declaration primitives
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.4 $
 *     (modified $Date: 2008-04-20 04:46:29 $)
 *
 *  @description
 *       some words that mimic partial behaviour of a C compiler
 *       especially it's preprocessor.
 */

#ifdef __cplusplus
extern "C" {
#endif




/** #ELSE ( -- ) [FTH]
 * The implementation of #ELSE is done in C for speed and
 * being less error prone. Better use the ANSI-compatible
 * => [IF] => [ELSE] => [THEN] construct.
 */
extern P4_CODE (p4_sh_else);

/** #ENDIF ( -- ) [FTH] 
 * end of => #IF => #IFDEF => #IFNOTDEF and => #ELSE contructs
   (a dummy word that does actually nothing, but #ELSE may look for it)
 */
extern P4_CODE(p4_sh_endif);

/** #IF ( -- C: state-save-flag mfth-if-magic S: ) [FTH]
 * prepares for a following => #IS_TRUE or => #IS_FALSE,
 * does basically switch off compile-mode for the enclosed
 * code. <br>
 * better use the ANSI style => [IF] => [ELSE] => [THEN] construct.
 */
extern P4_CODE (p4_sh_if);

/** #IS_TRUE ( C: state-save-flag mfth-if-magic S: test-flag -- ) [FTH]
 * checks the condition on the <c>CS-STACK</c>. <br>
 * Pairs with => #IF <br>
 * better use the ANSI style => [IF] => [ELSE] => [THEN] construct.
 */
extern P4_CODE (p4_sh_is_true);

/** #IS_FALSE ( C: state-save-flag mfth-if-magic S: test-flag -- ) [FTH]
 * checks the condition on the <c>CS-STACK</c>. <br>
 * Pairs with => #IF <br>
 * better use the ANSI style => [IF] => [ELSE] => [THEN] construct.
 */
extern P4_CODE (p4_sh_is_false);

/** #IFDEF ( "word" -- ) [FTH]
 * better use <c>[DEFINED] word [IF]</c> - the word => [IF]
 * is ANSI-conform.
 */
extern P4_CODE (p4_sh_ifdef);

/** #IFNOTDEF ( "word" -- ) [FTH]
 * better use <c>[DEFINED] word [NOT] [IF]</c> - the word => [IF]
 * and => [ELSE] are ANSI-conform, while => #IFDEF => #ELSE are not.
 */
extern P4_CODE (p4_sh_ifnotdef);

/** #define ( "name" "value" -- ) [FTH]
 * create an alias, will actually make a =>"DEFER"ed word,
 * and it has the magic to handle number-arguments
 */
extern P4_CODE (p4_sh_define);

/** #pragma ( "word" "evaluate<cr>" -- ) [FTH]
 * pass the word to => ENVIRONMENT?
 * If the word does *not* exist, the rest of the line is parsed
 * away with => // - therefore, if the executed word does not
 * consume the line itself, the rest of the line is still executed.
 * examples:
 #pragma warnings on // if warnings is a variable, ON can set it
 #pragma stack-cells 50 < [if] .( not enough stackcells ) [then]
 #pragma simply anything else you like to have in environment or not

 * implementation:
 : #pragma ?exec
   bl word count environment? if exit then ( interpret the rest of the line )
   [compile] \               ( parse away the rest of the line as a comment )
 ;
 */
extern P4_CODE (p4_sh_pragma);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
