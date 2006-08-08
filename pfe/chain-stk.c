/** 
 * -- Stackhelp for CHAIN words
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 1.13 %
 *    (%date_modified: Tue Jul 09 16:19:55 2002 %)
 *
 *  @description
 *    Allow for chained words in the interpreter core.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: chain-stk.c,v 1.1.1.1 2006-08-08 09:07:48 guidod Exp $";
#endif
 
#define _P4_SOURCE 1
#include <pfe/pfe-base.h>
#include <pfe/stackhelp-ext.h>

P4_LISTWORDS (chain_check) =
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
P4_COUNTWORDS (chain_check, "Check-chain of executions");

/*@}*/

/* 
 * Local variables:
 * c-file-style: "stroustrup"
 * End:
 */
