/** 
 * -- The Optional String Word Set
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE             @(#) %derived_by: guidod %
 *  @version %version: 5.8 %
 *    (%date_modified: Mon Mar 12 10:32:42 2001 %)
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
"@(#) $Id: string-ext.c,v 0.31 2001-05-12 18:15:46 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-xtra.h>

#include <string.h>
#include <ctype.h>

#include <pfe/def-comp.h>
#include <pfe/_missing.h>

/** -TRAILING ( str-ptr str-len -- str-ptr str-len' )
 * check the given buffer if it contains whitespace at its end.
 * If so, shorten str-len to meet the last non-whitespace
 * character in the buffer.
 */
FCode (p4_dash_trailing)
{
    SP[0] = p4_dash_trailing ((char *)SP[1], SP[0]);
}

/** /STRING ( str-ptr str-len n -- str-ptr' str-len' )
 * shorten the buffer from the beginning by n characters, i.e.
  str-ptr += n ;
  str-len -= n; 
 */
FCode (p4_slash_string)
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
FCode (p4_blank)
{
    memset ((char *)SP[1], ' ', (p4ucell)SP[0]);
    SP+=2;
}

/** CMOVE ( from-ptr to-ptr len -- )
 *  memcpy an area from->to for len bytes, starting at
 *  the lower addresses, see => CMOVE>
 */
FCode (p4_cmove)
{
    char *p = (char *)SP[2];
    char *q = (char *)SP[1];
    p4ucell n = SP[0];
    SP+=3;
  
    while (n--)
        *q++ = *p++;
}

/** CMOVE> ( from-ptr to-ptr len -- )
 *  memcpy an area from->to for len bytes, starting 
 *  with the higher addresses, see => CMOVE
 */
FCode (p4_cmove_up)
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

/** COMPARE ( str-ptr1 str-len1 str-ptr2 str-len2 -- n )
 * compare both str-buffers, return 0 if they are equal,
 * -1 if lower or shorter, and 1 if greater or longer
 */
FCode (p4_compare)
{
    char *p1 = (char *)SP[3];
    p4ucell u1 = SP[2];
    char *p2 = (char *)SP[1];
    p4ucell u2 = SP[0];
    int d;
    SP+=3;

    if (u1 < u2)
        *SP = (d = memcmp (p1, p2, u1)) == 0
            ? -1
            : d < 0 ? -1 : 1;
    else
        *SP = (d = memcmp (p1, p2, u2)) == 0
            ? u1 == u2 ? 0 : 1
            : d < 0 ? -1 : 1;
}

/** SEARCH ( str-ptr1 str-len1 str-ptr2 str-len2 -- str-ptr1' str-len1' flag )
 * search the str-buffer1 for the text of str-buffer2,
 * if it is contained return TRUE and return buffer-values that
 * point to the contained string, otherwise return FALSE and
 * leave the original str-buffer1.
 */
FCode (p4_search)
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

/** SLITERAL ( -- string )
 compile-time: ( CS: str-ptr str-len -- )
 * this word does almost the same as => LITERAL - it takes
 * an => S" string as specified in the CS-STACK at compile
 * time and compiles into the current definition where it is
 * returned as if there were a direct string-literal. This
 * can be used to compute a string-literal at compile-time
 * and hardwire it.
 example:
   : ORIGINAL-HOME  [ $HOME COUNT ] SLITERAL ; ( -- str-ptr str-len )
 */
FCode (p4_sliteral)
{
    FX_COMPILE (p4_sliteral);
    p4_string_comma ((char *)SP[1], SP[0]);
    SP += 2;
}
extern FCode (p4_s_quote_execution);
P4COMPILES (p4_sliteral, p4_s_quote_execution,
  P4_SKIPS_STRING, P4_DEFAULT_STYLE);


P4_LISTWORDS (string) =
{
    CO ("-TRAILING",	p4_dash_trailing),
    CO ("/STRING",	p4_slash_string),
    CO ("BLANK",	p4_blank),
    CO ("CMOVE",	p4_cmove),
    CO ("CMOVE>",	p4_cmove_up),
    CO ("COMPARE",	p4_compare),
    CO ("SEARCH",	p4_search),
    CS ("SLITERAL",	p4_sliteral),
    P4_INTO ("ENVIRONMENT", 0 ),
    P4_OCON ("STRING-EXT",	1994 ),
    
};
P4_COUNTWORDS (string, "String + extensions");

/*@}*/

