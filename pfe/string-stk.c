/** 
 * -- StackHelp for String-Ext
 * 
 *  Copyright (C) Tektronix, Inc. 2003 - 2003. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 33.11 %
 *    (%date_modified: %)
 *
 *  @description
 *      see stackhelp-ext for details
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: string-stk.c,v 1.1.1.1 2006-08-08 09:09:51 guidod Exp $";
#endif

#define _P4_SOURCE 1
#include <pfe/pfe-base.h>


P4_LISTWORDS (string_check) =
{
    P4_STKi ("-TRAILING", "str-ptr str-len   -- str-ptr  str-len'"),
    P4_STKi ("/STRING",   "str-ptr str-len n -- str-ptr' str-len'"),
    P4_STKi ("BLANK",     "str-ptr str-len -- "),
    P4_STKi ("CMOVE",     "from-ptr to-ptr len# -- "),
    P4_STKi ("CMOVE>",    "from-ptr to-ptr len# -- "),
    P4_STKi ("COMPARE",   "str1-ptr str1-len str2-ptr str2-len -- n"),
    P4_STKi ("SEARCH",     "str1-ptr str1-len str2-ptr str2-len -- "
      /*................*/ "str1-ptr' str1-len' flag" ),
    P4_STKi ("SLITERAL",   "C: str-ptr str-len -- str-ptr str-len"),
};
P4_COUNTWORDS (string_check, "Check-String words + extension");

/*@}*/

/* 
 * Local variables:
 * c-file-style: "stroustrup"
 * End:
 */
