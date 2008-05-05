/** 
 * --    The Optional Exception Word Set
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.5 $
 *     (modified $Date: 2008-05-05 02:04:52 $)
 *
 *  @description
 *      These words implement an exception system in the
 *      widely known => THROW &amp; => CATCH concept.
 *
 *      see the PFE-SIG wordset for catching OS traps.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: exception-ext.c,v 1.5 2008-05-05 02:04:52 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-limits.h>

#include <pfe/os-string.h>
#include <pfe/os-ctype.h>

#include <pfe/_missing.h>

/** CATCH ( catch-xt* -- 0 | throw#! ) [ANS]
 * execute the given execution-token and catch
 * any exception that can be caught therein.
 * software can arbitrarily raise an exception
 * using => THROW - the value 0 means there
 * was no exception, other denote implementation
 * dependent exception-codes.
 */
FCode (p4_catch)
{
    p4cell catch_code = p4_catch ((p4xt) *SP++);
    FX_PUSH (catch_code);
}

/** THROW ( throw#! -- [THROW] | throw# -- ) [ANS]
 * raise an exception - it will adjust the depth
 * of all stacks and start interpreting at the point 
 * of the latest => CATCH <br>
 * if n is null nothing happens, the -1 (ie. => FALSE )
 * is the raise-code of => ABORT - the other codes
 * are implementation dependent and will result in
 * something quite like => ABORT
 */
FCode (p4_throw)
{
    p4cell n = FX_POP;

    switch (n)
    {
     case 0:
         return;
     case -2:
         p4_throws (n, (p4_char_t *) SP[1], SP[0]);
     default:
	 p4_throw (n);
    }
}

/** ABORT ( -- [THROW] ) [ANS]
 * throw - cleanup some things and go back to the QUIT routine
 : ABORT -1 THROW ;
 */
FCode (p4_abort)
{
    p4_throw (P4_ON_ABORT);
}

/** ((ABORT")) ( -- ) [HIDDEN]
 * compiled by => ABORT" what"
 */ 
FCode_XE (p4_abort_quote_execution)
{   FX_USE_CODE_ADDR {
    p4_charbuf_t *p = (p4_char_t *) IP;
    FX_SKIP_STRING;
    if (*SP++ != 0)
        p4_throws (P4_ON_ABORT_QUOTE, P4_CHARBUF_PTR(p), P4_CHARBUF_LEN(p));
    FX_USE_CODE_EXIT;
}}
/** 'ABORT"' ( [string<">] -- [THROW] ) [ANS]
 * throw like => ABORT but print an additional error-message
 * to stdout telling what has happened.
 */
FCode (p4_abort_quote)
{
    FX_COMPILE (p4_abort_quote);
    FX (p4_parse_comma_quote);
}
P4COMPILES (p4_abort_quote, p4_abort_quote_execution,
  P4_SKIPS_STRING, P4_DEFAULT_STYLE);

/** NEXT-EXCEPTION ( -- throw#!* ) [EXT]
 *  used for runtime allocation of a new throw id.
 */

/* ((EXCEPTION-STRING)) ( -- zstring* id )
 */ 
FCode_RT (p4_exception_string_RT)
{ FX_USE_BODY_ADDR {
    p4_Exception* expt = (p4_Exception*) FX_POP_BODY_ADDR;
    FX_PUSH(expt->name);
    FX_PUSH(expt->id);
}}

/** (EXCEPTION-STRING: ( exception# [description<closeparen>] -- )
 * append a node with the given id and a pointer to an 
 * extern zstring to the => NEXT-EXCEPTION chain-list.
 */
FCode (p4_exception_string)
{
    FX_RUNTIME_HEADER;
    FX_RUNTIME1(p4_exception_string);
    p4cell id = FX_POP;
    p4_Exception* expt = (void*) DP; DP += sizeof(*expt);
    if (id < PFE.next_exception) PFE.next_exception = id - 1;
    expt->next = PFE.exception_link; PFE.exception_link = expt;
    expt->name = (char*) DP; expt->id = id;
    p4_word_parse (')'); /* PARSE-NOHERE-NOTHROW */
    p4_memcpy (DP, PFE.word.ptr, PFE.word.len);
    DP += PFE.word.len;
}
P4RUNTIME1(p4_exception_string, p4_exception_string_RT);

P4_LISTWORDS (exception) =
{
    P4_INTO ("[ANS]", 0),
    P4_FXco ("CATCH",			p4_catch),
    P4_FXco ("THROW",			p4_throw),
    P4_FXco ("ABORT",			p4_abort),
    P4_SXco ("ABORT\"",			p4_abort_quote),

    P4_INTO ("ENVIRONMENT", 0 ),
    P4_OCON ("EXCEPTION-EXT",	1994 ),
    P4_OCON ("THROW-IORS:X",    2007 ),

    P4_INTO ("EXTENSIONS", "FORTH"),
    P4_DVAR ("NEXT-EXCEPTION", next_exception),
    P4_EXPT ("no or not matching binary image",      P4_ON_NO_BINARY),
    P4_EXPT ("binary image too big",                 P4_ON_BIN_TOO_BIG),
    P4_EXPT ("out of memory",                        P4_ON_OUT_OF_MEMORY),
    P4_EXPT ("index out of range",                   P4_ON_INDEX_RANGE),
    P4_EXPT ("compile failed (call from bad point)", P4_ON_COMPILE_FAIL),
};
P4_COUNTWORDS (exception, "Exception + extensions");

/*@}*/

