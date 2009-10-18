/**
 * -- Stackhelp for PFE-Debug
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2008-04-20 04:46:29 $)
 *
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: debug-stk.c,v 1.3 2008-04-20 04:46:29 guidod Exp $";
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

