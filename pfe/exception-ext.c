/** 
 * --    The Optional Exception Word Set
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: bln_mpt1!33.15 %
 *    (%date_modified: Mon Apr 08 20:19:38 2002 %)
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
"@(#) $Id: exception-ext.c,v 1.1.1.1 2006-08-08 09:08:33 guidod Exp $";
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

P4_LISTWORDS (exception) =
{
    P4_INTO ("[ANS]", 0),
    P4_FXco ("CATCH",			p4_catch),
    P4_FXco ("THROW",			p4_throw),
    P4_FXco ("ABORT",			p4_abort),
    P4_SXco ("ABORT\"",			p4_abort_quote),

    P4_INTO ("ENVIRONMENT", 0 ),
    P4_OCON ("EXCEPTION-EXT",	1994 ),

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

