/** 
 * --    The Optional Exception Word Set
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 5.5 %
 *    (%date_modified: Mon Mar 12 10:32:15 2001 %)
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
"@(#) $Id: exception-ext.c,v 0.30 2001-03-12 09:32:15 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-xtra.h>

#include <string.h>
#include <ctype.h>

#include <pfe/_missing.h>

/** CATCH ( xt -- 0|n )
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

/** THROW ( n -- )
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
         p4_throws (n, (char *) SP[1], SP[0]);
     default:
	 p4_throw (n);
    }
}

/** ABORT ( -- ) no-return
 * throw - cleanup some things and go back to the QUIT routine
 : ABORT -1 THROW ;
 */
FCode (p4_abort)
{
    p4_throw (P4_ON_ABORT);
}

/** ((ABORT")) ( -- )
 * compiled by => ABORT" what"
 */ 
FCode (p4_abort_quote_execution)
{
    char *p = (char *) IP;

    FX_SKIP_STRING;
    if (*SP++ == 0)
        return;
    p4_throws (P4_ON_ABORT_QUOTE, p + 1, *(p4char *) p);
}
/** 'ABORT"' ( [string<">] -- ) no-return
 * throw like => ABORT but print an additional error-message
 * to stdout telling what has happened.
 */
FCode (p4_abort_quote)
{
    FX_COMPILE1 (p4_abort_quote);
    p4_parse_comma('"');
}
P4COMPILES (p4_abort_quote, p4_abort_quote_execution,
  P4_SKIPS_STRING, P4_DEFAULT_STYLE);


P4_LISTWORDS (exception) =
{
    CO ("CATCH",	p4_catch),
    CO ("THROW",	p4_throw),
    CO ("ABORT",	p4_abort),
    CS ("ABORT\"",	p4_abort_quote),
    P4_INTO ("ENVIRONMENT", 0 ),
    /* enviroment hints (testing for -EXT will mark this wordset as present) */
    P4_OCON ("EXCEPTION-EXT",	1994 ),
};
P4_COUNTWORDS (exception, "Exception + extensions");

/*@}*/

