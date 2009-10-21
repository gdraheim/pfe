/**
 *  -- Stackhelp for standard CORE and CORE-EXT wordset
 *
 *  Copyright (C) Tektronix, Inc. 2003 - 2004.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2008-04-20 04:46:29 $)
 *
 *  @description
 *      The Core Wordset contains the most of the essential words
 *      for ANS Forth. Here we add their stackdepth checks.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) =
      "@(#) $Id: core-stk.c,v 1.3 2008-04-20 04:46:29 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/stackhelp-ext.h>

P4_LISTWORDSET (core_check) [] =
{
    P4_STKi ("!",            "val addr* --"),
    P4_STKi ("#",            "n,n -- n,n'"),
    P4_STKi ("#>",           "n,n -- str* str#"), /* want code */
    P4_STKi ("#S",           "n,n -- 0:0"),
    P4_STKi ("(",            "[comment<rp>] --"),
    P4_STKi ("*",            "a# b# -- a#' {AS: =a*b}"),
    P4_STKi ("*/",           "a# b# c# -- n#"),
    P4_STKi ("*/MOD",        "a# b# c# -- m# n#"),
    P4_STKi ("+",            "a b -- a' | b'"),
    P4_STKi ("+!",           "val# addr* --"),
    P4_STKi ("+LOOP",        "increment# --"), /* need code */
    P4_STKi (",",            "val -- "),
    P4_STKi ("-",            "a* b* -- diff# | a* b# -- a' | a b -- a' {AS: a-b}"),
    P4_STKi (".",            "val --"),
    P4_STKi (".\"",          "[string<dq>]--"),
    P4_STKi ("/",            "a# b# -- a#' {AS: =a/b}"),
    P4_STKi ("/MOD",         "a# b# c# -- m# n#"),
    P4_STKi ("0<",           "a -- a?"),
    P4_STKi ("0=",           "0 -- 1! | a! -- 0"),
    P4_STKi ("1+",           "val -- val'"),
    P4_STKi ("1-",           "val -- val'"),
    P4_STKi ("2!",           "a,a addr* --"),
    P4_STKi ("2*",           "val# -- val#'"),
    P4_STKi ("2/",           "val# -- val#'"),
    P4_STKi ("2@",           "addr* -- a,a"),
    P4_STKi ("2DROP",        "a,b --"),
    P4_STKi ("2DUP",         "a,b -- a,b a,b"),
    P4_STKi ("2OVER",        "a,b c,d -- a,b c,d a,b"),
    P4_STKi ("2SWAP",        "a,b c,d -- c,d a,b"),
    P4_STKi (":",            "<name> --"), /* need code */
    P4_STKi (";",            " -- [EXIT] [DONE] "), /* need code */
    P4_STKi ("<",            "a b -- a'? {AS: a<b}"),
    P4_STKi ("<#",           " -- "), /* want code */
    P4_STKi ("=",            "a b -- a'? {AS: a=b}"),
    P4_STKi (">",            "a b -- a'? {AS: a>b}"),
    P4_STKi (">BODY",        "a* -- a*'"),
    P4_STKi (">IN",          "-- in#*"),
    P4_STKi (">NUMBER",      "a,a str* str# -- a,a' str*' str#"),
    P4_STKi (">R",           "a -- R: a"),
    P4_STKi ("?DUP",         "0 -- 0 | val! -- val val"), /* splitstack! */
    P4_STKi ("@",            "addr* -- val"),
    P4_STKi ("ABS",          "val -- val'"),
    P4_STKi ("ACCEPT",       "addr* n -- n'"),
    P4_STKi ("ALIGN",        "--"),
    P4_STKi ("ALIGNED",      "addr -- addr'"),
    P4_STKi ("ALLOT",        "count# --"),
    P4_STKi ("AND",          "a b -- x"),
    P4_STKi ("BASE",         "-- base#*"),
    P4_STKi ("BEGIN",        "--"), /* need code (branch) */
    P4_STKi ("BL",           "bell-char#"),
    P4_STKi ("C!",           "char addr* --"),
    P4_STKi ("C,",           "char --"),
    P4_STKi ("C@",           "addr* -- char"),
    P4_STKi ("CELL+",        "addr -- addr'"),
    P4_STKi ("CELLS",        "count# -- count#'"),
    P4_STKi ("CHAR",         "<char> -- char"),
    P4_STKi ("CHAR+",        "addr -- addr'"),
    P4_STKi ("CHARS",        "count# -- count#'"),
    P4_STKi ("CONSTANT",     "value <name> -- {DOES: -- value}"),
    P4_STKi ("COUNT",        "str* -- str-ptr' str-len"),
    P4_STKi ("CR",           "--"),
    P4_STKi ("DECIMAL",      "--"),
    P4_STKi ("DEPTH",        "-- value#"),
    P4_STKi ("DO",           "end start --"), /* need code (branch) */
    P4_STKi ("DOES>",        " -- addr* [EXIT] [DONE] "), /* need code */
    P4_STKi ("DROP",         "a --"),
    P4_STKi ("DUP",          "a -- a a"),
    P4_STKi ("ELSE",         " -- "), /* need code (branch) */
    P4_STKi ("EMIT",         "char --"),
    P4_STKi ("ENVIRONMENT?", "str* str# -- 0? | ... have?"),
    P4_STKi ("EVALUATE",     "str* str# -- ???"),
    P4_STKi ("EXECUTE",      "... xt* -- ???"),
    P4_STKi ("EXIT",         "-- [EXIT] "), /* need code */
    P4_STKi ("FILL",         "me* mem# char --"),
    P4_STKi ("FIND",         "name* -- xt* 1! | name* 0"),
    P4_STKi ("FM/MOD",       "n1,n2 n2# -- m# n#"),
    P4_STKi ("HERE",         "-- data*"),
    P4_STKi ("HOLD",         ""),
    P4_STKi ("I",            "-- i"), /* want code */
    P4_STKi ("IF",           " flag! | 0 -- "), /* need code */
    P4_STKi ("IMMEDIATE",    "--"),
    P4_STKi ("INVERT",       "a -- a'"),
    P4_STKi ("J",            "-- j"), /* want code */
    P4_STKi ("KEY",          "-- key"),
    P4_STKi ("LEAVE",        "--"), /* need code (branch) */
    P4_STKi ("LITERAL",      "-- value"), /* want code ? */
    P4_STKi ("LOOP",         "--"), /* meed code (branch) */
    P4_STKi ("LSHIFT",       "value shift# -- value'"),
    P4_STKi ("M*",           ""),
    P4_STKi ("MAX",          "a b -- a | b"),
    P4_STKi ("MIN",          "a b -- a | b"),
    P4_STKi ("MOD",          "a# b# -- c#"),
    P4_STKi ("MOVE",         "from-addr* to-addr* length# --"),
    P4_STKi ("NEGATE",       "value -- value'"),
    P4_STKi ("OR",           "a b# -- a' | a b -- c"),
    P4_STKi ("OVER",         "a b -- a b a"),
    P4_STKi ("POSTPONE",     "[name] --"),
    P4_STKi ("QUIT",         "--"), /* need code (exitpoint) */
    P4_STKi ("R>",           "R: a -- a "),
    P4_STKi ("R@",           "R: a -- a R: a"),
    P4_STKi ("RECURSE",      " ... -- ???"), /* need code (exitpoint) */
    P4_STKi ("REPEAT",       "--"), /* need code (branch) */
    P4_STKi ("ROT",          "a b c -- b c a"),
    P4_STKi ("RSHIFT",       "value shift# -- value'"),
    P4_STKi ("S\"",          "[string<dq>] -- str* str#"),
    P4_STKi ("S>D",          "a -- a,a"),
    P4_STKi ("SIGN",         "a --"),
    P4_STKi ("SM/REM",       "a,a b -- c d"),
    P4_STKi ("SOURCE",       "-- inputsource* inputoffset#"),
    P4_STKi ("SPACE",        "--"),
    P4_STKi ("SPACES",       "n# --"),
    P4_STKi ("STATE",        "-- state#*"),
    P4_STKi ("SWAP",         ""),
    P4_STKi ("THEN",         "--"), /* need code (branch) */
    P4_STKi ("TYPE",         "str* str# --"),
    P4_STKi ("U.",           "value --"),
    P4_STKi ("U<",           "a b -- a'? {AS: a<b}"),
    P4_STKi ("UM*",          "a b -- a,a' {AS: a*b}"),
    P4_STKi ("UM/MOD",       "a b -- c,c#"),
    P4_STKi ("UNLOOP",       "--"), /* want code */
    P4_STKi ("UNTIL",        "--"), /* need code (branch) */
    P4_STKi ("VARIABLE",     "<name> -- {DOES: -- name*}"),
    P4_STKi ("WHILE",        "flag? --"), /* need code (branch) */
    P4_STKi ("WORD",         "delim-char <string[delim-char]> -- here*"),
    P4_STKi ("XOR",          "a b# -- a' | a b -- c"),
    P4_STKi ("[",            "--"), /* need code (exitlike) */
    P4_STKi ("[']",          "[name] -- xt*"),
    P4_STKi ("[CHAR]",       "[char] -- char!"),
    P4_STKi ("]",            "--"), /* need code (exitlike) */

    /* core extension words */
    P4_STKi (".(",           "[comment<rp>] --"),
    P4_STKi (".R",           "value precision# --"),
    P4_STKi ("0<>",          "a! -- a! | 0 -- 0"),
    P4_STKi ("0>",           "a -- a?"),
    P4_STKi ("2>R",          "a,b -- R: a,b"),
    P4_STKi ("2R>",          "R: a,b-- a,b"),
    P4_STKi ("2R@",          "R: a,b-- a,b R: a,b"),
    P4_STKi (":NONAME",      "--"), /* need code (exitlike) */
    P4_STKi ("<>",           "a b -- a'? {AS: a<>b}"),
    P4_STKi ("?DO",          "a b --"), /* need code (branch) */
    P4_STKi ("AGAIN",        "--"), /* need code (branch) */
    P4_STKi ("C\"",          "[string<dq>] -- string*"),
    P4_STKi ("CASE",         "--"),
    P4_STKi ("COMPILE,",     "xt* --"),
    P4_STKi ("CONVERT",      "a b -- a b"),
    P4_STKi ("ENDCASE",      "switchvalue --"), /* need code (branch) */
    P4_STKi ("ENDOF",        "--"), /* need code (branch) */
    P4_STKi ("ERASE",        "buffer* buffer# -- "),
    P4_STKi ("EXPECT",       "inputstring* inputstringmax# -- "),
    P4_STKi ("HEX",          "--"),
    P4_STKi ("MARKER",       "<name> --"),
    P4_STKi ("NIP",          "a b -- b"),
    P4_STKi ("OF",           "switchvalue test -- switchvalue"), /* need co*/
    P4_STKi ("PAD",          "-- transientbuffer*"),
    P4_STKi ("PARSE",        " delim-char <string[delim-char]> -- buffer* buffer#"),
    P4_STKi ("PICK",         "value ...[n-1] n -- value ...[n-1] value | n -- value"),
    P4_STKi ("QUERY",        "--"),
    P4_STKi ("REFILL",       " -- flag?"),
    P4_STKi ("RESTORE-INPUT",""),
    P4_STKi ("ROLL",         "value ...[n-1] n -- ...[n-1] value | ... n -- ??? value"),
    P4_STKi ("SAVE-INPUT",   " -- ...[input!]"),
    P4_STKi ("SOURCE-ID",    " -- source-block#"),
    P4_STKi ("SPAN",         " -- span#*"),
    P4_STKi ("TIB",          " -- tib*" ),
    P4_STKi ("TO",           "value [name] --"), /* want code ? */
    P4_STKi ("TUCK",         "a b -- b a b"),
    P4_STKi ("U.R",          "value precision# --"),
    P4_STKi ("U>",           "a b -- a'? {AS: a>b}"),
    P4_STKi ("UNUSED",       "-- dictleft#"),
    P4_STKi ("VALUE",        "value <name> -- {DOES: -- value}"),
    P4_STKi ("WITHIN",       "a b c -- a?"),
    P4_STKi ("[COMPILE]",    "... [word] -- ???"), /* want code */
    P4_STKi ("\\",           "[string<eol>] -- "),

    P4_STKi ("PARSE-WORD",   "<name> -- name* name#"),
    P4_STKi ("<BUILDS",      "<name> -- "),
    P4_STKi ("CFA'",         "[name] -- xt*"),

    P4_INTO ("FORTH", 0),
    P4_STKi ("0",            "-- 0"),
    P4_STKi ("1",            "-- 1#"),
    P4_STKi ("2",            "-- 2#"),
    P4_STKi ("3",            "-- 3#"),

    P4_STKi ("0<=",           "a -- a'? {AS: a<=0}"),
    P4_STKi ("0>=",           "a -- a'? {AS: a>=0}"),
    P4_STKi ("<=",            "a b -- a'? {AS: a<=b}"),
    P4_STKi (">=",            "a b -- a'? {AS: a>=b}"),

    P4_INTO ("ENVIRONMENT",  0),
    P4_STKi ("CORE-EXT",     "-- year# true! | 0"),
    P4_STKi ("/COUNTED-STRING", "-- str-len"),
    P4_STKi ("/HOLD",        "-- hold-len"),
    P4_STKi ("/PAD",         "-- pad-len"),
    P4_STKi ("ADDRESS-UNIT-BITS", "-- octet-len"),
    P4_STKi ("FLOORED",      "-- floored?"),
    P4_STKi ("MAX-CHAR",     "-- charmax#"),
    P4_STKi ("MAX-N",        "-- intmax#"),
    P4_STKi ("MAX-U",        "-- uintmax#"),
    P4_STKi ("STACK-CELLS",  "-- stackcells#"),
};
P4_COUNTWORDSET (core_check, "Check-Core words + extensions");

/*@}*/

