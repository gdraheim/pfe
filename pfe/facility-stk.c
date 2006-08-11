/** 
 *  --- Stackhelp for The Optional Facility Word Set
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2006 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.2 $
 *     (modified $Date: 2006-08-11 22:56:04 $)
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: facility-stk.c,v 1.2 2006-08-11 22:56:04 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/stackhelp-ext.h>

P4_LISTWORDS (facility_check) =
{
    P4_STKi ("AT-XY",		"col# row# --"),
    P4_STKi ("KEY?",		"-- key?"),
    P4_STKi ("PAGE",		"--"),
    P4_STKi ("EKEY",		"-- key-code#"),
    P4_STKi ("EKEY>CHAR",	"key-code# -- key-code# 0 | char# true!"),
    P4_STKi ("EKEY?",		"-- ekey?"),
    P4_STKi ("EMIT?",	        "-- emit?"),
    P4_STKi ("MS",		"milliseconds# --"),
    P4_STKi ("TIME&DATE",	"-- sec# min# hrs# day# month# year#"),

    P4_INTO ("ENVIRONMENT",	0 ),
    P4_STKi ("FACILITY-EXT",	"-- year# true! | 0" ),
};
P4_COUNTWORDS (facility_check, "Check-Facility + extensions");

/*@}*/

