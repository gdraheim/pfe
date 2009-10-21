/**
 * -- Stackhelp for The Optional Exception Word Set
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2008-04-20 04:46:30 $)
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
"@(#) $Id: exception-stk.c,v 1.3 2008-04-20 04:46:30 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/stackhelp-ext.h>

P4_LISTWORDSET (exception_check) [] =
{
    P4_STKi ("CATCH",		"xt* -- exception! | 0"),
    P4_STKi ("THROW",		"exception! -- [THROW] | exception# -- "),
    P4_STKi ("ABORT",		"-- [THROW]"),
    P4_STKi ("ABORT\"",		"[string<dq>] -- [THROW]"),

    P4_INTO ("EXTENSIONS", 0),
    P4_STKi ("NEXT-EXCEPTION",  "-- exception#*"),
};
P4_COUNTWORDSET (exception_check, "Check-Exception + extensions");

/*@}*/
