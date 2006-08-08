/**
 * -- Stackhelp for PFE-Debug
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE             @(#) %derived_by: guidod %
 *  @version %version: 33.41 %
 *    (%date_modified: Wed Mar 19 16:52:22 2003 %)
 *
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: debug-stk.c,v 1.1.1.1 2006-08-08 09:07:46 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/stackhelp-ext.h>

P4_LISTWORDS (debug_check) =
{
    P4_INTO ("FORTH", 0),
    P4_STKi ("DEBUG",		"<name> --"),
    P4_STKi ("NO-DEBUG",	"<name> --"),
    P4_STKi ("(SEE)",		"xt* --"),
    P4_STKi ("ADDR>NAME",	"addr* -- nfa* | 0"),
    P4_STKi ("COME_BACK",	"--"),

    P4_INTO ("ENVIRONMENT", 0),
    P4_STKi ("PFE-DEBUG",	"-- level#"),
};
P4_COUNTWORDS (debug_check, "Check-Debugger words");

/*@}*/

