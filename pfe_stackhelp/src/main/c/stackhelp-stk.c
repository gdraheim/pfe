/**
 * -- Stackhelp for StackHelp TypeCheck Word Set
 *
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

P4_LISTWORDSET (stackhelp_check) [] =
{
    P4_STKi ("|(", " [string<rp>] --"),
    P4_STKi ("STACKHELP", "[name] --"),
    P4_STKi ("STACKHELP-WORDLIST","-- wordlist*"),

    P4_STKi ("NARROW-CHANGER(", "changer# <stackhelp[rp]> --"),
    P4_STKi ("NARROW-INPUTS(",  "changer# <stackhelp[rp]> --"),
    P4_STKi ("NARROW-OUTPUTS(", "changer# <stackhelp[rp]> --"),
    P4_STKi ("NARROW-INPUT-VARIANT(",
             "variant# changer# <stackhelp[rp]> --"),
    P4_STKi ("NARROW-OUTPUT-VARIANT(",
             "variant# changer# <stackhelp[rp]> --"),
    P4_STKi ("NARROW-INPUT-STACK(",
             "stk-char variant# changer# <stackhelp[rp]> --"),
    P4_STKi ("NARROW-OUTPUT-STACK(",
             "stk-char variant# changer# <stackhelp[rp]> --"),
    P4_STKi ("NARROW-INPUT-ARGUMENT(",
             "arg# stk-char variant# changer# <stackhelp[rp]> --"),
    P4_STKi ("NARROW-OUTPUT-ARGUMENT(",
             "arg# stk-char variant# changer# <stackhelp[rp]> --"),
    P4_STKi ("NARROW-INPUT-ARGUMENT-NAME(",
             "arg# stk-char variant# changer# <stackhelp[rp]> --"),
    P4_STKi ("NARROW-OUTPUT-ARGUMENT-NAME(",
             "arg# stk-char variant# changer# <stackhelp[rp]> --"),
    P4_STKi ("NARROW-INPUT-ARGUMENT-TYPE(",
             "arg# stk-char variant# changer# <stackhelp[rp]> --"),
    P4_STKi ("NARROW-OUTPUT-ARGUMENT-TYPE(",
             "arg# stk-char variant# changer# <stackhelp[rp]> --"),
    P4_STKi ("CANONIC-INPUT-TYPE(",
             "arg# stk-char variant# changer# <stackhelp[rp]> --"),
    P4_STKi ("CANONIC-OUTPUT-TYPE(",
             "arg# stk-char variant# changer# <stackhelp[rp]> --"),
    P4_STKi ("REWRITER-TEST(",
             "<from-stack -- into-stack[rp]> --"),
    P4_STKi ("REWRITER-INPUT-ARG(",
             "arg# <from-stack -- into-stack[rp]> --"),

    P4_STKi ("REWRITE-LINE(", " <stack-layout[rp]> --"),
    P4_STKi ("REWRITE-SHOW.", "--"),
    P4_STKi ("REWRITE-STACK-TEST(", " <stackhelp[rp]> --"),
    P4_STKi ("REWRITE-INPUT-ARG(", "arg# <stackhelp[rp]> --"),
    P4_STKi ("REWRITE-STACK-RESULT(", " <stackhelp[rp]> --"),

    P4_STKi ("NARROW-INPUT-NOTATION(",
             "notation# changer# <stackhelp[rp]> --"),
    P4_STKi ("NARROW-OUTPUT-NOTATION(",
             "notation# changer# <stackhelp[rp]> --"),
    P4_STKi ("REWRITE-STK-VARIANT-TEST(", " <stackhelp[rp]> --"),
    P4_STKi ("REWRITE-STK-VARIANT-RESULT("," <stackhelp[rp]> --"),
    P4_STKi ("REWRITE-STK-SELECT(", " <stackhelp[rp]> --"),
    P4_STKi ("REWRITE-STK-EXPAND(", " <stackhelp[rp]> --"),
    P4_STKi ("REWRITE-STK-RESULT(", " <stackhelp[rp]> --"),

    P4_STKi ("REWRITE-TEST(",   " <stackhelp[rp]> --"),
    P4_STKi ("REWRITE-SELECT(", " <stackhelp[rp]> --"),
    P4_STKi ("REWRITE-EXPAND(", " <stackhelp[rp]> --"),
    P4_STKi ("REWRITE-RESULT(", " <stackhelp[rp]> --"),
};
P4_COUNTWORDSET (stackhelp_check, "Check-StackHelp TypeChecking extension");

/*@}*/

/*
 * Local variables:
 * c-file-style: "stroustrup"
 * End:
 */
