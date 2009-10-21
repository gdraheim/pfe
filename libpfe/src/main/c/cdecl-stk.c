/**
 * -- Stackhelp for C-like declaration primitives
 *
 *  Copyright (C) Tektronix, Inc. 2001
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2008-04-20 04:46:29 $)
 *
 *  @description
 *       some words that mimic partial behaviour of a C compiler
 *       especially it's preprocessor.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) =
"@(#) $Id: cdecl-stk.c,v 1.3 2008-04-20 04:46:29 guidod Exp $";
#endif

#define _P4_SOURCE 1
#include <pfe/pfe-base.h>
#include <pfe/stackhelp-ext.h>

P4_LISTWORDSET (cdecl_check) [] =
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
P4_COUNTWORDSET (cdecl_check, "Check-C-preprocessor declaration syntax");

/*@}*/

