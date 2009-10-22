/**
 * -- Stackhelp for CHAIN words
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author$)
 *  @version $Revision$
 *     (modified $Date$)
 *
 *  @description
 *    Allow for chained words in the interpreter core.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) =
"@(#) $Id$";
#endif

#define _P4_SOURCE 1
#include <pfe/pfe-base.h>
#include <pfe/stackhelp-ext.h>

P4_LISTWORDSET (chain_check) [] =
{
    P4_STKi ("link,",                   "list -- "),
    P4_STKi ("chain-link",              " -- var*"),
    P4_STKi (".chain",                  "<name> --"),
    P4_STKi (".chains",                 "chain* --"),
    P4_STKi ("chain-add",               "chain* <word> --"),
    P4_STKi ("chain-add-before",        "chain* <word> --"),

    P4_STKi ("do-chain",		"chain* --"),
    P4_STKi ("new-chain",               "<name> --"),
    P4_STKi ("xdo-chain",		"chain* --"),
    P4_STKi ("new-sys-chain",           "<name> --"),
/*  P4_STKi ("semicolon-chain",         "-- chain*"), */
};
P4_COUNTWORDSET (chain_check, "Check-chain of executions");

/*@}*/
