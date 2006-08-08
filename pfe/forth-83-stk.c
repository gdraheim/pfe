/** 
 * --  Stackhelp for Compatiblity with the FORTH-83 standard.
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 33.26 %
 *    (%date_modified: Wed Mar 19 17:31:24 2003 %)
 *
 *  @description
 *     All FORTH-83-Standard words are included here that are not 
 *     in the dpANS already.
 *     Though most of the "uncontrolled reference words" are omitted.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: forth-83-stk.c,v 1.1.1.1 2006-08-08 09:09:54 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/stackhelp-ext.h>

P4_LISTWORDS (forth_83_check) =
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
P4_COUNTWORDS (forth_83_check, "Check-Forth'83 compatibility");

/*@}*/

/*
 * Local variables:
 * c-file-style: "stroustrup"
 * End:
 */

