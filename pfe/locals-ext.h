#ifndef _PFE_LOCALS_EXT_H
#define _PFE_LOCALS_EXT_H 985126517
/* generated 2001-0320-2315 ../../pfe/../mk/Make-H.pl ../../pfe/locals-ext.c */

#include <pfe/incl-ext.h>

/** 
 * -- The Optional Locals Word Set
 * 
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 5.8 %
 *    (%date_modified: Mon Mar 12 10:32:26 2001 %)
 *
 *  @description
 *      The Portable Forth Environment does implement locals
 *      in such an extended form as that additional variable
 *      names can be declared anywhere in the compiled word.
 */

#ifdef __cplusplus
extern "C" {
#endif




extern P4_CODE (p4_local_enter_execution);

extern P4_CODE (p4_local_args);

extern P4_CODE (p4_locals_bar_execution);

extern P4_CODE (p4_locals_exit_execution);

extern P4_CODE (p4_local_execution);

extern P4_CODE (p4_to_local_execution);

/** (LOCAL)          ( strptr strcnt -- )
 * this word is used to create compiling words that can
 * declare => LOCALS| - it shall not be used directly
 * to declare a local, the pfe provides => LVALUE for
 * that a purpose beyond => LOCALS| 
 */
extern P4_CODE (p4_paren_local);

/** LOCALS|   ( xN ... x2 x1 [name1 .. nameN <|>] -- )
 * create local identifiers to be used in the current definition.
 * At runtime, each identifier will be assigned a value from
 * the parameter stack. <br>
 * The identifiers may be treated as if being a => VALUE , it does
 * also implement the ansi => TO extensions for locals. Note that
 * the identifiers are only valid inside the currently compiled
 * word, the => SEE decompiled word will show them as 
 * => <A> => <B> ... => <N> a.s.o.   <br>
 * see also => LVALUE
 */
extern P4_CODE (p4_locals_bar);

/** LVALUE ( value [name] -- )
 * declares a single local => VALUE using => (LOCAL) - a 
 * sequence of => LVALUE declarations can replace a 
 * => LOCALS| argument, ie. <c> LOCALS| a b c | </c> 
 * is the same as <c> LVALUE a  LVALUE b  LVALUE c </c>.
 * This should also clarify the runtime stack behaviour of
 * => LOCALS| where the stack parameters seem to be
 * assigned in reverse order as opposed to their textual
 * identifier declarations. <br>
 * compare with => VALUE and the pfe's convenience word
 * =>'VAR'.
 */
extern P4_CODE (p4_local_value);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
