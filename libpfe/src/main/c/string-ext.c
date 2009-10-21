/**
 * -- The Optional String Word Set
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2008-04-20 04:46:29 $)
 *
 *  @description
 *      The basic words for handling string buffers. There are
 *      much more buffer words used in most Forth implementations around
 *      but they can usually implemented as colon-word quite effectivly
 *      on the basis of the STRING wordset.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) =
"@(#) $Id: string-ext.c,v 1.3 2008-04-20 04:46:29 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-limits.h>

#include <pfe/os-string.h>
#include <pfe/os-ctype.h>

#include <pfe/def-comp.h>
#include <pfe/_missing.h>

/** -TRAILING ( str-ptr str-len -- str-ptr str-len' )
 * check the given buffer if it contains whitespace at its end.
 * If so, shorten str-len to meet the last non-whitespace
 * character in the buffer.
 */
void FXCode (p4_dash_trailing)
{
    SP[0] = p4_dash_trailing ((p4_char_t *)SP[1], SP[0]);
}

/** /STRING ( str-ptr str-len n -- str-ptr' str-len' )
 * shorten the buffer from the beginning by n characters, i.e.
  str-ptr += n ;
  str-len -= n;
 */
void FXCode (p4_slash_string)
{
    p4ucell a = FX_POP;
    if (a < (p4ucell) SP[0])
    {
        SP[0] -= a;
        SP[1] += a;
    }else{
        SP[1] += SP[0];
        SP[0] = 0;
    }
}

/** BLANK ( str-ptr str-len -- )
 * => FILL a given buffer with => BL blanks
 */
void FXCode (p4_blank)
{
    p4_memset ((char *)SP[1], ' ', (p4ucell)SP[0]);
    SP+=2;
}

/** CMOVE ( from-ptr to-ptr len# -- )
 *  memcpy an area from->to for len bytes, starting at
 *  the lower addresses, see => CMOVE>
 */
void FXCode (p4_cmove)
{
    char *p = (char *)SP[2];
    char *q = (char *)SP[1];
    p4ucell n = SP[0];
    SP+=3;

    while (n--)
        *q++ = *p++;
}

/** CMOVE> ( from-ptr to-ptr len# -- )
 *  memcpy an area from->to for len bytes, starting
 *  with the higher addresses, see => CMOVE
 */
void FXCode (p4_cmove_up)
{
    char *p = (char *)SP[2];
    char *q = (char *)SP[1];
    p4ucell n = SP[0];
    SP+=3;

    p += n;
    q += n;
    while (n--)
        *--q = *--p;
}

/** COMPARE ( str1-ptr str1-len str2-ptr str2-len -- diff# )
 * compare both str-buffers, return 0 if they are equal,
 * -1 if lower or shorter, and 1 if greater or longer
 */
void FXCode (p4_compare)
{
    char *p1 = (char *)SP[3];
    p4ucell u1 = SP[2];
    char *p2 = (char *)SP[1];
    p4ucell u2 = SP[0];
    int d;
    SP+=3;

    if (u1 < u2)
        *SP = (d = p4_memcmp (p1, p2, u1)) == 0
            ? -1
            : d < 0 ? -1 : 1;
    else
        *SP = (d = p4_memcmp (p1, p2, u2)) == 0
            ? u1 == u2 ? 0 : 1
            : d < 0 ? -1 : 1;
}

/** SEARCH ( str1-ptr str1-len str2-ptr str2-len -- str1-ptr' str1-len' flag )
 * search the str-buffer1 for the text of str-buffer2,
 * if it is contained return TRUE and return buffer-values that
 * point to the contained string, otherwise return FALSE and
 * leave the original str-buffer1.
 */
void FXCode (p4_search)
{
    const char *p =
        p4_search ((char *)SP[3], SP[2], (char *)SP[1], SP[0]);
    ++SP;
    if (p == NULL)
        SP[0] = P4_FALSE;
    else
    {
        SP[0] = P4_TRUE;
        SP[1] += (char *)SP[2] - p;
        SP[2] = (p4cell)p;
    }
}

/** SLITERAL ( C: str-ptr str-len -- S: str-ptr str-len )
 * this word does almost the same as => LITERAL - it takes
 * an => S" string as specified in the CS-STACK at compile
 * time and compiles into the current definition where it is
 * returned as if there were a direct string-literal. This
 * can be used to compute a string-literal at compile-time
 * and hardwire it.
 example:
   : ORIGINAL-HOME  [ $HOME COUNT ] SLITERAL ; ( -- str-ptr str-len )
 */
void FXCode (p4_sliteral)
{
    FX_COMPILE (p4_sliteral);
    p4_string_comma ((p4_char_t *)SP[1], SP[0]);
    SP += 2;
}
extern void FXCode (p4_s_quote_execution);
P4COMPILES (p4_sliteral, p4_s_quote_execution,
  P4_SKIPS_STRING, P4_DEFAULT_STYLE);


P4_LISTWORDSET (string) [] =
{
    P4_INTO ("[ANS]", 0),
    P4_FXco ("-TRAILING",	p4_dash_trailing),
    P4_FXco ("/STRING",		p4_slash_string),
    P4_FXco ("BLANK",		p4_blank),
    P4_FXco ("CMOVE",		p4_cmove),
    P4_FXco ("CMOVE>",		p4_cmove_up),
    P4_FXco ("COMPARE",		p4_compare),
    P4_FXco ("SEARCH",		p4_search),
    P4_SXco ("SLITERAL",	p4_sliteral),

    P4_INTO ("ENVIRONMENT", 0 ),
    P4_OCON ("STRING-EXT",	1994 ),

};
P4_COUNTWORDSET (string, "String + extensions");

/*@}*/

