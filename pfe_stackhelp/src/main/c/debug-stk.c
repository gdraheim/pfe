/**
 * -- Stackhelp for PFE-Debug
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author$)
 *  @version $Revision$
 *     (modified $Date$)
 *
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) =
"@(#) $Id$";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/stackhelp-ext.h>

P4_LISTWORDSET (debug_check) [] =
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
P4_COUNTWORDSET (debug_check, "Check-Debugger words");

/*@}*/
