#ifndef _PFE_CDECL_EXT_H
#define _PFE_CDECL_EXT_H 984413840
/* generated 2001-0312-1717 ../../pfe/../mk/Make-H.pl ../../pfe/cdecl-ext.c */

#include <pfe/incl-ext.h>

/** 
 * -- C-like declaration primitives
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 1.4 %
 *    (%date_modified: Mon Mar 12 10:32:01 2001 %)
 *
 *  @description
 *       some words that mimic partial behaviour of a C compiler
 *       especially it's preprocessor.
 */

#ifdef __cplusplus
extern "C" {
#endif




/** #ELSE ( -- )
 * The implementation of #ELSE is done in C for speed and
 * being less error prone. Better use the ANSI-compatible
 * => [IF] => [ELSE] => [THEN] construct.
 */
extern P4_CODE (p4_sh_else);

/** #ENDIF ( -- )
 * end of => #IF => #IFDEF => #IFNOTDEF and => #ELSE contructs
   (a dummy word that does actually nothing, but #ELSE may look for it)
 */
extern P4_CODE(p4_sh_endif);

/** #IF ( -- )
        ( -- state-save mfth-if-magic )
 * prepares for a following => #IS_TRUE or => #IS_FALSE,
 * does basically switch off compile-mode for the enclosed
 * code. <br>
 * better use the ANSI style => [IF] => [ELSE] => [THEN] construct.
 */
extern P4_CODE (p4_sh_if);

/** #IS_TRUE ( flag -- )
              ( state-save mfth-if-magic flag -- )
 * checks the condition on the <c>CS-STACK</c>. <br>
 * Pairs with => #IF <br>
 * better use the ANSI style => [IF] => [ELSE] => [THEN] construct.
 */
extern P4_CODE (p4_sh_is_true);

/** #IS_FALSE ( flag -- )
              ( state-save mfth-if-magic flag -- )
 * checks the condition on the <c>CS-STACK</c>. <br>
 * Pairs with => #IF <br>
 * better use the ANSI style => [IF] => [ELSE] => [THEN] construct.
 */
extern P4_CODE (p4_sh_is_false);

/** #IFDEF ( "word" -- )
 * better use <c>[DEFINED] word [IF]</c> - the word => [IF]
 * is ANSI-conform.
 */
extern P4_CODE (p4_sh_ifdef);

/** #IFNOTDEF ( "word" -- )
 * better use <c>[DEFINED] word [NOT] [IF]</c> - the word => [IF]
 * and => [ELSE] are ANSI-conform, while => #IFDEF => #ELSE are not.
 */
extern P4_CODE (p4_sh_ifnotdef);

/** #define ( "name" "value" -- )
 * create an alias, will actually make a =>"DEFER"ed word,
 * and it has the magic to handle number-arguments
 */
extern P4_CODE (p4_sh_define);

/** #pragma ( "word" -- ? )
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
