#ifndef _PFE_TOOLS_MIX_H
#define _PFE_TOOLS_MIX_H 1105051254
/* generated 2005-0106-2340 ../../../pfe/../mk/Make-H.pl ../../../pfe/tools-mix.c */

#include <pfe/pfe-mix.h>

/** 
 * -- miscellaneous useful extra words for TOOLS-EXT
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version:  1.15 %
 *    (%date_modified:  Tue Apr 16 11:59:23 2002 %)
 *
 *  @description
 *      Compatiblity with former standards, miscellaneous useful words.
 *      ... for TOOLS-EXT
 */

#ifdef __cplusplus
extern "C" {
#endif




/** VLIST ( -- )
 *  The VLIST command had been present in FIG and other forth
 *  implementations. It has to list all accessible words. In PFE
 *  it list all words in the search order. Well, the point is,
 *  that we do really just look into the search order and are
 *  then calling => WORDS on that Wordl. That way you can see
 *  all accessible words in the order they might be found.
 *  Uses => ?CR
 */
extern P4_CODE (p4_vlist);

/** !CSP ( -- )
 * put => SP into => CSP
 * <br> used in control-words
 */
extern P4_CODE (p4_store_csp);

/** ?CSP ( -- )
 * check that => SP == => CSP otherwise => THROW
 * <br> used in control-words
 */
extern P4_CODE (p4_Q_csp);

/** ?COMP ( -- )
 * check that the current => STATE is compiling
 * otherwise => THROW
 * <br> often used in control-words
 */
extern P4_CODE (p4_Q_comp);

/** ?EXEC ( -- )
 * check that the current => STATE is executing
 * otherwise => THROW
 * <br> often used in control-words
 */
extern P4_CODE (p4_Q_exec);

/** ?FILE ( file-id -- )
 * check the file-id otherwise (fixme)
 */
extern P4_CODE (p4_Q_file);

/** ?LOADING ( -- )
 * check that the currently interpreted text is 
 * from a file/block, otherwise => THROW
 */
extern P4_CODE (p4_Q_loading);

/** ?PAIRS ( a b -- )
 * if compiling, check that the two magics on
 * the => CS-STACK are identical, otherwise throw
 * <br> used in control-words
 */
extern P4_CODE (p4_Q_pairs);

/** ?STACK ( -- )
 * check all stacks for underflow and overflow conditions,
 * and if such an error condition is detected => THROW
 */
extern P4_CODE (p4_Q_stack);

/** [DEFINED]             ( [name] -- flag )
 *  Search the dictionary for _name_. If _name_ is found,
 *  return TRUE; otherwise return FALSE. Immediate for use in
 *  definitions.
  
 * This word will actually return what => FIND returns (the NFA). 
 * does check for the word using find (so it does not throw like => ' )
 * and puts it on stack. As it is immediate it does work in compile-mode
 * too, so it places its argument in the cs-stack then. This is most
 * useful with a directly following => [IF] clause, so that sth. like
 * an <c>[IFDEF] word</c> can be simulated through <c>[DEFINED] word [IF]</c>

 : [DEFINED] DEFINED ; IMMEDIATE
 : [DEFINED] BL WORD COUNT (FIND-NFA) ; IMMEDIATE
 */
extern P4_CODE (p4_defined);

/** [UNDEFINED]          ( [name] -- flag )
 *  Search the dictionary for _name_. If _name_ is found,
 *  return FALSE; otherwise return TRUE. Immediate for use in
 *  definitions.
 *
 *  see => [DEFINED]
 : [UNDEFINED] DEFINED 0= ; IMMEDIATE
 */
extern P4_CODE (p4_undefined);

/** (FORGET) ( addr -- )
 * forget everything above addr
 * - used by => FORGET
 */
extern P4_CODE (p4_paren_forget);

/** (DICTLIMIT)   ( -- constvalue )
 * the upper limit of the forth writeable memory space,
 * the variable => DICTLIMIT must be below this line.
 * stack-space and other space-areas are often allocated
 * above => DICTLIMIT upto this constant.
 *
 * => DICTFENCE is the lower end of the writeable dictionary
 */
extern P4_CODE(p4_paren_dictlimit);

/** (DICTFENCE)   ( -- constvalue )
 * the lower limit of the forth writeable memory space,
 * the variable => DICTFENCE must be above this line.
 * Some code-areas are often moved in between => DICTFENCE and
 * this constant. To guard normal Forth code from deletion
 * the usual practice goes with the => FENCE variable
 *
 * => DICTLIMIT is the upper end of the writeable dictionary
 */
extern P4_CODE(p4_paren_dictfence);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
