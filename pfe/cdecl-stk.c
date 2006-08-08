/** 
 * -- Stackhelp for C-like declaration primitives
 *
 *  Copyright (C) Tektronix, Inc. - All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 1.13 %
 *    (%date_modified: Wed Jul 17 13:54:43 2002 %)
 *
 *  @description
 *       some words that mimic partial behaviour of a C compiler
 *       especially it's preprocessor.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: cdecl-stk.c,v 1.1.1.1 2006-08-08 09:07:40 guidod Exp $";
#endif
 
#define _P4_SOURCE 1
#include <pfe/pfe-base.h>
#include <pfe/stackhelp-ext.h>

P4_LISTWORDS (cdecl_check) =
{
    P4_STKi ("#ELSE",          "--"), /* want code (branch)*/
    P4_STKi ("#ENDIF",         "--"), /* want code (branch)*/
    P4_STKi ("#IF",            "val? --"), /* want code (branch)*/
    P4_STKi ("#IFDEF",         "[name] --"), /* want code (branch)*/
    P4_STKi ("#IFNDEF",        "[name] --"), /* want code (branch)*/
    P4_STKi ("#IFNOTDEF",      "[name] --"), /* want code (branch)*/
    P4_STKi ("#IS_TRUE",       "x -- x?"),
    P4_STKi ("#IS_FALSE",      "x -- x?"),
    P4_STKi ("//",             "[string<eol>] --"),
    P4_STKi ("#DEFINE",        "<name> <expression> --"),
    P4_STKi ("#PRAGMA",        "<word> -- ?"),
};
P4_COUNTWORDS (cdecl_check, "Check-C-preprocessor declaration syntax");

/*@}*/

