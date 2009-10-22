/**
 * -- miscellaneous useful extra words for CORE-EXT
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
 *      ... for CORE-EXT
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) =
"@(#) $Id$";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/version-sub.h>
#include <pfe/logging.h>

/************************************************************************/
/* more comparision operators                                           */
/************************************************************************/

/** 0<= ( a -- flag )
 simulate    : 0<= 0> 0= ;
 */
void FXCode (p4_zero_less_equal)
{
    *SP = P4_FLAG (*SP <= 0);
}

/** 0>= ( a -- flag )
 simulate    : 0>= 0< 0= ;
 */
void FXCode (p4_zero_greater_equal)
{
    *SP = P4_FLAG (*SP >= 0);
}

/** <= ( a b -- flag )
 simulate    : <= > 0= ;
 */
void FXCode (p4_less_equal)
{
    SP[1] = P4_FLAG (SP[1] <= SP[0]);
    SP++;
}

/** >= ( a b -- flag )
 simulate    : >= < 0= ;
 */
void FXCode (p4_greater_equal)
{
    SP[1] = P4_FLAG (SP[1] >= SP[0]);
    SP++;
}

/** U<= ( a b -- flag )
 simulate    : U<= U> 0= ;
 */
void FXCode (p4_u_less_equal)
{
    SP[1] = P4_FLAG ((p4ucell) SP[1] <= (p4ucell) SP[0]);
    SP++;
}

/** U>= ( a b -- flag )
 simulate    : U>= U< 0= ;
 */
void FXCode (p4_u_greater_equal)
{
    SP[1] = P4_FLAG ((p4ucell) SP[1] >= (p4ucell) SP[0]);
    SP++;
}

/** UMAX ( a b -- max )
 * see => MAX
 */
void FXCode (p4_u_max)
{
    if ((p4ucell) SP[0] > (p4ucell) SP[1])
        SP[1] = SP[0];
    SP++;
}

/** UMIN ( a b -- min )
 * see => MIN , => MAX and => UMAX
 */
void FXCode (p4_u_min)
{
    if ((p4ucell) SP[0] < (p4ucell) SP[1])
        SP[1] = SP[0];
    SP++;
}

/** LICENSE ( -- )
 * show a lisence info - the basic PFE system is licensed under the terms
 * of the LGPL (Lesser GNU Public License) - binary modules loaded into
 * the system and hooking into the system may carry another => LICENSE
 : LICENSE [ ENVIRONMENT ] FORTH-LICENSE TYPE ;
 */
void FXCode (p4_license)
{
    p4_outs (p4_license_string ());
}

/** WARRANTY ( -- )
 * show a warranty info - the basic PFE system is licensed under the terms
 * of the LGPL (Lesser GNU Public License) - which exludes almost any
 * liabilities whatsoever - however loadable binary modules may hook into
 * the system and their functionality may have different WARRANTY infos.
 */
void FXCode (p4_warranty)
{
    p4_outs (p4_warranty_string ());
}

/** .VERSION ( -- )
 * show the version of the current PFE system
 : .VERSION [ ENVIRONMENT ] FORTH-NAME TYPE FORTH-VERSION TYPE ;
 */
void FXCode (p4_dot_version)
{
    p4_outs (p4_version_string ());
}

/** .CVERSION ( -- )
 * show the compile date of the current PFE system
 : .CVERSION [ ENVIRONMENT ] FORTH-NAME TYPE FORTH-DATE TYPE ;
 */
void FXCode (p4_dot_date)
{
    p4_outf ("PFE compiled %s, %s ",
        p4_compile_date (), p4_compile_time ());
}

/* ..................................................................... */
/* parse and place at HERE */

/** STRING,               ( str len -- )
 *  Store a string in data space as a counted string.
 : STRING, HERE  OVER 1+  ALLOT  PLACE ;
 */
void FXCode (p4_string_comma)
{
    p4_string_comma ((p4char*) SP[1], SP[0]);
    FX_2DROP;
}

/** PARSE,                    ( "chars<">" -- )
 *  Store a char-delimited string in data space as a counted
 *  string. As seen in Bawd's
 : ," [CHAR] " PARSE  STRING, ; IMMEDIATE
 *
 * this implementation is much different from Bawd's
 : PARSE, PARSE STRING, ;
 */
void FXCode (p4_parse_comma)
{
    p4_word_parse (FX_POP); *DP=0; /* PARSE-NOHERE */
    p4_string_comma (PFE.word.ptr, PFE.word.len);
}

/** "PARSE,\""  ( "chars<">" -- )
 *  Store a quote-delimited string in data space as a counted
 *  string.
 : ," [CHAR] " PARSE  STRING, ; IMMEDIATE
 *
 * implemented here as
 : PARSE," [CHAR] " PARSE, ; IMMEDIATE
 */
void FXCode (p4_parse_comma_quote)
{
    p4_word_parse ('"'); *DP=0; /* PARSE-NOHERE */
    p4_string_comma (PFE.word.ptr, PFE.word.len);
}

/* ------------------------------ */
/* MARKER HINTS                   */

static void p4_create_marker (const p4char* name, p4cell len,
                              p4_Wordl* wordlist);

/** "(MARKER)" ( str-ptr str-len -- )
 * create a named marker that you can use to => FORGET ,
 * running the created word will reset the dict/order variables
 * to the state at the creation of this name.
 : (MARKER) (CREATE) HERE ,
         GET-ORDER DUP , 0 DO ?DUP IF , THEN LOOP 0 ,
         ...
   DOES> DUP @ (FORGET)
         ...
 ;
 */
void FXCode (p4_paren_marker)
{
    register p4cell len = FX_POP;
    register p4char* name = (p4char*) FX_POP;
    p4_create_marker (name, len, CURRENT);
}

/** ANEW ( 'name' -- )
 * creates a => MARKER if it doesn't exist,
 * or forgets everything after it if it does. (it just gets executed).
 *
 * Note: in PFE the => ANEW will always work on the => ENVIRONMENT-WORDLIST
 * which has a reason: it is never quite sure whether the same
 * => DEFINITIONS wordlist is in the search => ORDER that the original
 * => ANEW => MARKER was defined in. Therefore, => ANEW would be only safe
 * on systems that do always stick to => FORTH => DEFINITIONS. Instead
 * we will => CREATE the => ANEW => MARKER in the => ENVIRONMENT and use a
 * simple => SEARCH-WORDLIST on the => ENVIRONMENT-WORDLIST upon re-run.
 \ old
 : ANEW BL WORD   DUP FIND NIP IF EXECUTE THEN   (MARKER) ;
 \ new
 : ANEW
   PARSE-WORD  2DUP ENVIRONMENT-WORDLIST SEARCH-WORDLIST IF  EXECUTE  THEN
   GET-CURRENT >R ENVIRONMENT-WORDLIST SET-CURRENT  (MARKER)  R> SET-CURRENT ;
 */
void FXCode (p4_anew)
{
    register p4_namebuf_t* name;

    if ((p4_word_parseword(' ')) &&
        (name= p4_search_wordlist (PFE.word.ptr, PFE.word.len, PFE.environ_wl) ))
    {
        register p4xt xt = p4_name_from (name);
        if (*P4_TO_CODE(xt) != PFX(p4_marker_RT))
            P4_fail2 ("ANEW did find non-MARKER name"
                      " called '%.*s' as its argument, still executing...",
                      (int) PFE.word.len, PFE.word.ptr);
        p4_call (xt);
    }

    p4_create_marker (PFE.word.ptr, PFE.word.len, PFE.environ_wl);
}

static void p4_create_marker (const p4_char_t* name, p4cell len,
                              p4_Wordl* wordlist)
{
    int i;
    p4char* forget_address = PFE.dp;

    p4_header_comma (name, len, wordlist);
    FX_RUNTIME1 (p4_marker);

    FX_PCOMMA (forget_address); /* PFE.dp restore */
    FX_PCOMMA (p4_FENCE);
    FX_PCOMMA (p4_LAST);
    FX_PCOMMA (p4_ONLY);
    FX_PCOMMA (p4_CURRENT);

    /* we do not need to memorize null-ptrs in the search-order,
     * and we use a nullptr to flag the end of the saved ptrlist
     */

    for (i=0; i < PFE_set.wordlists ; i++)
        if (p4_CONTEXT[i])
            FX_PCOMMA (p4_CONTEXT[i]);
    FX_UCOMMA (0);

    for (i=0; i < PFE_set.wordlists ; i++)
        if (p4_DFORDER[i])
            FX_PCOMMA (p4_DFORDER[i]);
    FX_UCOMMA (0);
}

/** "((MARKER))" ( -- )
 * runtime compiled by => MARKER
 */
void FXCode_RT (p4_marker_RT)
{   FX_USE_BODY_ADDR;
    int i;
    void** RT = (void*) FX_POP_BODY_ADDR;
    void* forget_address;
    /* assert (sizeof(void*) == sizeof(p4cell)) */

    forget_address =       (*RT++);
    p4_FENCE =   (p4char*) (*RT++);
    p4_LAST =    (p4char*) (*RT++);
    p4_ONLY =     (Wordl*) (*RT++);
    p4_CURRENT =  (Wordl*) (*RT++);
    for (i=0; i < PFE_set.wordlists ; i++)
    {
        if (! *RT)
            p4_CONTEXT[i] = 0; /* no RT++ !! */
        else
            p4_CONTEXT[i] = (Wordl*) (*RT++);
    }
    while (*RT) RT++;
    RT++; /* skip null */
    for (i=0; i < PFE_set.wordlists ; i++)
    {
        if (! *RT)
            p4_DFORDER[i] = 0; /* no RT++ !! */
        else
            p4_DFORDER[i] = (Wordl*) (*RT++);
    }
    p4_forget (forget_address); /* will set the PFE.dp */
    /* MARKER RT wants (FORGET) to prune VOC_LINK and run DESTROYERs */
}

P4_LISTWORDSET (core_misc) [] =
{
    P4_INTO ("FORTH", 0),

    /** quick constants - implemented as code */
    P4_OCoN ("0",		0),
    P4_OCoN ("1",		1),
    P4_OCoN ("2",		2),
    P4_OCoN ("3",		3),

    /* more comparision */
    P4_FXco ("0<=",		p4_zero_less_equal),
    P4_FXco ("0>=",		p4_zero_greater_equal),
    P4_FXco ("<=",		p4_less_equal),
    P4_FXco (">=",		p4_greater_equal),
    P4_FXco ("U<=",		p4_u_less_equal),
    P4_FXco ("U>=",		p4_u_greater_equal),
    P4_FXco ("UMIN",		p4_u_min),
    P4_FXco ("UMAX",		p4_u_max),

    /* forth distributor info */
    P4_FXco (".VERSION",	p4_dot_version),
    P4_FXco (".CVERSION",	p4_dot_date),
    P4_FNYM (".PFE-DATE",	".CVERSION"),
    P4_FXco ("LICENSE",		p4_license),
    P4_FXco ("WARRANTY",	p4_warranty),

    /* parse and place HERE */
    P4_FXco ("STRING,",      p4_string_comma),
    P4_FXco ("PARSE,",       p4_parse_comma),
    P4_IXco ("PARSE,\"",     p4_parse_comma_quote),

    /* definition checks */
    P4_ICoN ("[VOID]",       0),
    P4_FXco ("DEFINED",      p4_defined),

    /* SEARCH-EXT forth200x/defined */
    P4_IXco ("[DEFINED]",    p4_defined),
    P4_IXco ("[UNDEFINED]",  p4_undefined),

    P4_INTO ("EXTENSIONS", "FORTH"),
    P4_FXco ("(MARKER)",     p4_paren_marker),
    P4_FXco ("ANEW",         p4_anew),
};
P4_COUNTWORDSET (core_misc, "CORE-Misc Compatibility words");

/*@}*/
