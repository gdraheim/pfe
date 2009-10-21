/**
 * --  Stackhelp for Compatiblity with the FORTH-83 standard.
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2008-04-20 04:46:31 $)
 *
 *  @description
 *     All FORTH-83-Standard words are included here that are not
 *     in the dpANS already.
 *     Though most of the "uncontrolled reference words" are omitted.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) =
"@(#) $Id: forth-83-stk.c,v 1.3 2008-04-20 04:46:31 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/stackhelp-ext.h>

P4_LISTWORDSET (forth_83_check) [] =
{
    P4_STKi ("2+",		"a# -- a#' | a* -- a*'"),
    P4_STKi ("2-",		"a# -- a#' | a* -- a*'"),
    P4_STKi ("?TERMINAL",	""),
    P4_STKi ("COMPILE",		"<name> --"),
    P4_STKi ("VOCABULARY",	"<name> --"),

    P4_STKi ("-->",		"--"),
    P4_STKi ("INTERPRET",	"--"),
    P4_STKi ("K",		"-- loop#"),
    P4_STKi ("OCTAL",		"--"),
    P4_STKi ("SP@",		"-- sp-cell*"),

    P4_STKi ("!BITS",		"x-bits# x-addr* mask# --"),
    P4_STKi ("@BITS",		"x-addr* mask# -- x-bits#"),
    P4_STKi ("_like:NTOHS",	"a -- a'"),
    P4_STKi ("_like:NTOHS-MOVE", "from-addr* to-addr* count# --"),
    P4_STKi ("**",		"a# b# -- power#"),
    P4_STKi ("DPL",		"dpl-cell*"),

    P4_STKi ("SEAL",		"--"),
};
P4_COUNTWORDSET (forth_83_check, "Check-Forth'83 compatibility");

/*@}*/

/*
 * Local variables:
 * c-file-style: "stroustrup"
 * End:
 */

