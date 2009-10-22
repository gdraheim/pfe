/**
 * -- Stackhelp for miscellaneous useful words (fig-forth)
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
 *      Compatiblity with former standards, miscellaneous useful words.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) =
"@(#) $Id$";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/stackhelp-ext.h>

P4_LISTWORDSET (misc_check) [] =
{
    P4_STKi ("ok",		"--"),
    P4_STKi ("COLD",		"-- [THROW]"),
    P4_STKi ("LIT",		"-- lit"),
    P4_STKi (".LINE",		"line# block# --"),
    P4_STKi ("OUT",		"-- out#*"),
    P4_STKi ("DP",		"-- dp#*"),
    P4_STKi ("HLD",		"-- hold#*"),
    P4_STKi ("R0",		"-- r0*"),
    P4_STKi ("S0",		"-- s0*"),
    P4_STKi ("UD.R",		"x,x# r# --"),
    P4_STKi ("UD.",		"x,x# --"),
    P4_STKi ("ID.",	        "nfa* --"),
    P4_STKi (".NAME",		"ID."),
    P4_STKi ("-ROLL",		"num# -- | ...[n-1] x n# -- x ...[n-1]"),
    P4_STKi ("RANDOM",		"n# -- random#"),
    P4_STKi ("SRAND",		"n# --"),
    P4_STKi ("(UNDER+)",	"n# m# -- n#' m# | n* m# -- n*' m#"),
    P4_STKi ("+TO",		"num# <varname> --"),
    P4_STKi ("BUILD-ARRAY",	"n...[X] X# -- m"),
    P4_STKi ("ACCESS-ARRAY",	"n...[X] array* -- array*' X#"),
    P4_STKi (".STATUS",		"--"),
    P4_STKi ("UPPER-CASE?",	"-- case?"),
    P4_STKi ("LOWER-CASE",	"-- case?*"),
    P4_STKi ("LOWER-CASE-FN?",	"-- case?"),
    P4_STKi ("LOWER-CASE-FN",	"-- case?*"),
    P4_STKi ("REDEFINED-MSG?",	"-- case?"),
    P4_STKi ("REDEFINED-MSG",	"-- case?*"),
    P4_STKi ("QUOTED-PARSE?",	"-- case?*"),

    P4_STKi ("SOURCE-LINE",	"-- line#"),
    P4_STKi ("SOURCE-NAME",     "-- name-ptr name-len"),
    P4_STKi ("TH'POCKET",	"pocket# -- pocket-ptr pocket-len"),
    P4_STKi ("POCKET-PAD",	"-- pocket-ptr"),
    P4_STKi ("/CELL",		"-- cellmax#"),
    P4_STKi ("W@",		"wchar* -- wchar"),
    P4_STKi ("W!",		"wchars# wchar* --"),
    P4_STKi ("W+!",		"wchars# wchar* --"),
    P4_STKi ("WL-HASH",		"char* len# -- hash#"),
    P4_STKi ("TOPMOST",		"wid# -- nfa*"),

    P4_STKi ("LS.WORDS",	"--"),
    P4_STKi ("LS.PRIMITIVES",	"--"),
    P4_STKi ("LS.COLON-DEFS",	"--"),
    P4_STKi ("LS.DOES-DEFS",	"--"),
    P4_STKi ("LS.CONSTANTS",	"--"),
    P4_STKi ("LS.VARIABLES",	"--"),
    P4_STKi ("LS.VOCABULARIES",	"--"),
    P4_STKi ("LS.MARKERS",	"--"),

    P4_STKi ("TAB",		"n# --"),
    P4_STKi ("BACKSPACE",	"--"),
    P4_STKi ("?STOP",		"-- flag?"),
    P4_STKi ("START?CR",	"--"),
    P4_STKi ("?CR",		"-- flag?"),
    P4_STKi ("CLOSE-ALL-FILES", "--"),
    P4_STKi (".MEMORY",		"--"),

    P4_STKi ("*EMIT*",		"-- handler-xt*"),
    P4_STKi ("*EXPECT*",	"-- handler-xt*"),
    P4_STKi ("*KEY*",		"-- handler-xt*"),
    P4_STKi ("*TYPE*",		"-- handler-xt*"),
    P4_STKi ("(EMIT)",		"val# --"),
    P4_STKi ("(EXPECT)",	"a b --"),
    P4_STKi ("(KEY)",		"-- key#"),
    P4_STKi ("(TYPE)",		"str* len# --"),
    P4_STKi ("STANDARD-I/O",	"--"),

    P4_STKi ("HELP",		"<name> --"),
    P4_STKi ("EDIT-BLOCKFILE",	"<name> --"),
    P4_STKi ("APPLICATION",	"app-xt* --"),
    P4_STKi ("ARGC",		"-- argc#"),
    P4_STKi ("ARGV",		"argn# --- arg-ptr arg-len"),
    P4_STKi ("EXITCODE",	"-- exitcode#*"),
    P4_STKi ("STDIN",		"-- stdin-file*"),
    P4_STKi ("STDOUT",		"-- stdout-file*"),
    P4_STKi ("STDERR",		"-- stderr-file*"),

    P4_STKi ("EXPAND-FN",  "name-ptr name-len buf-ptr -- buf-ptr buf-len"),
    P4_STKi ("LOAD\"",		"[name<dq>] -- ???"),
    P4_STKi ("SYSTEM",		"command-str command-len -- exitcode#"),
    P4_STKi ("SYSTEM\"",	"[command<dq>] -- exitcode#"),
    P4_STKi ("RAISE",		"signal# --"),
    P4_STKi ("SIGNAL",		"new-xt* signal# -- old-xt*"),

    P4_STKi ("CREATE:",		"<name> --"),
    P4_STKi ("BUFFER:",		"size# <name> --"),

    P4_STKi ("R'@",		"R: a b -- a R: a b"),
    P4_STKi ("R'!",		"x R: a b -- R: x b"),
    P4_STKi ("R\"@",		"R: a b c -- a R: a b c"),
    P4_STKi ("R\"!",		"x R: a b c -- R: x b c"),
    P4_STKi ("R!",		"x R: a -- R: x"),
    P4_STKi ("2R!",		"x,x R: a b -- R: x,x"),
    P4_STKi ("DUP>R",		"x -- x R: x"),
    P4_STKi ("R>DROP",		"R: x -- "),
    P4_STKi ("2R>2DROP",	"R: x,x -- "),
    P4_STKi ("CLEARSTACK",	"--"),

    P4_INTO ("EXTENSIONS",      0),
    P4_STKi ("+UNDER",          "n x m -- n' x {AS: n'=n+m}"),

    P4_STKi ("EXECUTES",	"fkey# <name> --"),

    P4_INTO ("ENVIRONMENT", 0 ),
    P4_STKi ("RAND_MAX",        "-- randmax#"),
};
P4_COUNTWORDSET (misc_check, "Check-Compatibility Miscellaneous words");

/*@}*/
