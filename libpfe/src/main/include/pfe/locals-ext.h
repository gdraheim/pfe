#ifndef PFE_LOCALS_EXT_H
#define PFE_LOCALS_EXT_H 1256212373
/* generated 2009-1022-1352 make-header.py ../../c/locals-ext.c */

#include <pfe/pfe-ext.h>

/**
 * -- The Optional Locals Word Set
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.7 $
 *     (modified $Date: 2008-08-31 03:07:54 $)
 *
 *  @description
 *      The Portable Forth Environment does implement locals
 *      in such an extended form as that additional variable
 *      names can be declared anywhere in the compiled word.
 *
 *      Locals are names for values that live in a locals-frame
 *      on the return-stack - on entry to the procedure that
 *      locals-frame is carved from the return-stack and a
 *      frame-pointer is setup. Locals are in two forms, one
 *      is inialized by a chunk from the parameter-stack as
 *      it is with => LOCALS| while the others are local variables
 *      declared later. The latter are left unitialized on
 *      setup of the locals-frame.
 *
 *      For unnamed returnstack locals, see words like
 *      =>"R@" =>"R!" =>"R'@" =>"R'!" =>'R"@' =>'R"!' =>'2R@' =>'2R!'
 *      but here the setup and cleanup of the return-stack frame
 *      is left to the user, possibly using some words like
 *      =>">R" =>"R>" =>"2>R" =>"2R>" =>"R>DROP" while the locals-ext
 *      will take care to provide a frame-creation token and
 *      some cleanup-code for each => EXIT or => ;
 */

#ifdef __cplusplus
extern "C" {
#endif




extern void FXCode_XE (p4_local_enter_execution);

extern void FXCode_XE (p4_local_args);

extern void FXCode_XE (p4_locals_bar_execution);

extern void FXCode_XE (p4_locals_exit_execution);

extern void FXCode_XE (p4_local_execution);

extern void FXCode_XE (p4_to_local_execution);

/** (LOCAL)          ( strptr strcnt -- )
 * this word is used to create compiling words that can
 * declare => LOCALS| - it shall not be used directly
 * to declare a local, the pfe provides => LVALUE for
 * that a purpose beyond => LOCALS|
 */
extern void FXCode (p4_paren_local);

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
extern void FXCode (p4_locals_bar);

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
 *
 : LVALUE
   STATE @ IF
     VALUE
   ELSE
     BL WORD COUNT DUP (LOCAL) (TO)
   THEN
 ; IMMEDIATE
 */
extern void FXCode (p4_local_value);

extern void FXCode (p4_local_buffer_var_TO);

/** LBUFFER: ( size [name] -- )
 * declares a single local => VALUE using => (LOCAL) - which
 * will hold the address of an area like => BUFFER: but carved
 * from the return-stack (as in C with alloca). This local buffer
 * will be automatically given up at the end of the word. The
 * return-stack-pointer will be increased only at the time of
 * this function (and the address assigned to the =>"LVALUE")
 * so that the provided size gets determined at runtime. Note
 * that in some configurations the forth-return-stack area is
 * quite small - for large string operations you should consider
 * to use a => POCKET-PAD in pfe.
 : LBUFFER:
   STATE @ IF
     BUFFER:
   ELSE
     :NONAME ( size -- rp* ) R> RP@ - DUP RP! SWAP >R ;NONAME
     COMPILE, POSTPONE LVALUE
   THEN
 ; IMMEDIATE
 */
extern void FXCode (p4_local_buffer_var);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
