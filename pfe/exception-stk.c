/** 
 * -- Stackhelp for The Optional Exception Word Set
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 33.23 %
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
"@(#) $Id: exception-stk.c,v 1.1.1.1 2006-08-08 09:09:36 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/stackhelp-ext.h>

P4_LISTWORDS (exception_check) =
{
    P4_STKi ("CATCH",		"xt* -- exception! | 0"),
    P4_STKi ("THROW",		"exception! -- [THROW] | exception# -- "),
    P4_STKi ("ABORT",		"-- [THROW]"),
    P4_STKi ("ABORT\"",		"[string<dq>] -- [THROW]"),

    P4_INTO ("EXTENSIONS", 0),
    P4_STKi ("NEXT-EXCEPTION",  "-- exception#*"),
};
P4_COUNTWORDS (exception_check, "Check-Exception + extensions");

/*@}*/

