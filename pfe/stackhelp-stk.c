/** 
 * -- Stackhelp for StackHelp TypeCheck Word Set
 * 
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 33.57 %
 *    (%date_modified: Mon Feb 24 20:02:23 2003 %)
 *
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: stackhelp-stk.c,v 1.1.1.1 2006-08-08 09:08:03 guidod Exp $";
#endif

#define _P4_SOURCE 1
#include <pfe/pfe-base.h>
#include <pfe/stackhelp-ext.h>

P4_LISTWORDS (stackhelp_check) =
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
P4_COUNTWORDS (stackhelp_check, "Check-StackHelp TypeChecking extension");

/*@}*/

/* 
 * Local variables:
 * c-file-style: "stroustrup"
 * End:
 */
