/**
 *
 *  Copyright (C) 2000 - 2004 Guido U. Draheim <guidod@gmx.de>
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author$)
 *  @version $Revision$
 *     (modified $Date$)
 *
 * @description:
 *       ZCHAR-EXT wordset - defines words for zero-terminated strings,
 *       the datatype prefix is called "Z" which is usually a simple CHAR.
 *       And CHAR can be either BCHAR or WCHAR depending on your platform.
 *       Anyway, the words in this wordset should be largely modelled
 *       after the examples found in other forth implementations - most
 *       prominently MPE's forths.
 */

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>

#include <pfe/os-ctype.h>
#include <pfe/os-string.h>

#include <pfe/def-words.h>

/* --------------------------------------------------------------------- */

/** 'Z"'           ( [chars<">] -- z* )
 * scan the input to the next doublequote and create a buffer
 * that holds the chars - return the address of that zero-terminated
 * string-buffer, either =>"POCKET-PAD" or =>"ALLOT"ed into the dictionary.
 */
void FXCode(p4_z_quote)
{
    register p4_byte_t* p;
    register p4ucell n;

    p4_word_parse ('"'); *HERE=0; /* PARSE-NOHERE */
    n = PFE.word.len;

    if (STATE)
    {
        FX_COMPILE (p4_z_quote);
        HERE += sizeof(short);
        p = HERE;
    }else{
        p = p4_pocket ();
        n = PFE.word.len < P4_POCKET_SIZE ?
            PFE.word.len : P4_POCKET_SIZE;
    }

    p4_memcpy (p, PFE.word.ptr, n);  p[PFE.word.len] = '\0';

    if (STATE)
    {
        HERE += n+1;
        FX (p4_align);
        ((short*)p)[-1] = (HERE - p);
    }else{
        FX_PUSH(p);
    }
}
void FXCode_XE (p4_z_quote_XT)
{   FX_USE_CODE_ADDR;
    short skip = *P4_INC(IP, short);
    FX_PUSH(IP);
    P4_ADD_(IP,skip,char);
    FX_USE_CODE_EXIT;
}
p4xcode* p4_z_quote_SEE(p4xcode* ip, char* p, p4_Semant* s)
{
    int skip = *P4_INC(ip,short);
    sprintf (p, "%.*s %.*s\" ",
             NAMELEN(s->name), NAMEPTR(s->name),
             (int) skip, (char*) ip);
    P4_ADD_(ip,skip,char);
    return ip;
}
P4COMPILES(p4_z_quote, p4_z_quote_XT, p4_z_quote_SEE, 0);

/** ZCOUNT    ( z* -- z* len )
 * push length of z-string, additionally to the string addr itself.
 : ZSTRLEN ZCOUNT NIP ;
 * (see libc strlen(3)) / compare with => COUNT / => ZSTRLEN
 */
void FXCode (p4_zcount)
{
    /* FX_PUSH (p4_strlen ((char*)(*SP))) is wrong,
     * gcc may leave unintended behaviour
     */
    register int i = p4_strlen ((char*)(*SP));
    FX_PUSH(i);
}

/** ZSTRLEN    ( z* -- len )
 * push length of z-string.
 : ZSTRLEN ZCOUNT NIP ;
 * (see libc strlen(3)) / compare with => ZMOVE / => CMOVE
 */
void FXCode (p4_zstrlen)
{
    *SP = p4_strlen ((char*)(*SP));
}

/** ZMOVE      ( zsrc* zdest* -- )
 * copy a zero terminated string
 * (see libc strcpy(3)) / compare with => ZSTRLEN / => COUNT
 */
void FXCode (p4_zmove)
{
    p4_strcpy ((char*)(SP[0]), (char*)(SP[1]));
    FX_2DROP;
}


/** APPENDZ    ( caddr* u zdest* -- )
 * Add the string defined by CADDR LEN to the zero terminated string
 * at ZDEST - actually a => SYNONYM of => +ZPLACE of the => ZPLACE family
 * (see strncat(3)) / compare with => ZPLACE / => +PLACE
 */

/** +ZPLACE    ( caddr* u zdest* -- )
 * Add the string defined by CADDR LEN to the zero terminated string
 * at ZDEST - (for older scripts the => SYNONYM named => APPENDZ exists)
 * (see libc strncat(3)) / compare with => ZPLACE / => +PLACE
 */
void FXCode (p4_appendz)
{
    p4_strncat ((char*)(SP[0]), (char*)(SP[2]), (int)(SP[1]));
    FX_3DROP;
}

/** ZPLACE  ( addr* len zaddr* -- )
 * copy string and place as 0 terminated
 * (see libc strncpy(3)) / see also => +ZPLACE / => Z+PLACE
 */
void FXCode (p4_zplace)
{
    p4_strncpy ((char*)(SP[0]), (char*)(SP[2]), (int)(SP[1]));
    FX_3DROP;
}

/* ------------------------------------------------------------------- */

/*
 * helper function used by all backslash-lit-strings
 * copies a string from input buffer to output buffer
 * thereby interpreting backlash-sequences. Returns
 * the number of chars copied.
 */
p4ucell p4_backslash_parse_into (p4char delim, p4char* dst, int max,
                                 int refills)
{
    register int i, j = 0;
    register const p4char* src; p4ucell len;

 parse:
    p4_word_parse (delim); *HERE=0; /* PARSE-NOHERE */
    src = PFE.word.ptr; len = PFE.word.len;

    if (! len && refills--) { if (p4_refill ()) goto parse; }
    i = 0;
    while (i < len && j < max)
    {
        if (src[i] != '\\')
        {
            dst[j++] = src[i++];
        }else{
            if (++i == len) goto parse;
            switch (src[i])
            {
            case 'z': dst[j++] = '\0'; i++; break;
            case 't': dst[j++] = '\t'; i++; break;
            case 'n': dst[j++] = '\n'; i++; break;
            case 'l': dst[j++] = '\n'; i++; break;
            case 'r': dst[j++] = '\r'; i++; break;
            case 'm': dst[j++] = '\r'; i++;
                      dst[j++] = '\n'; i++; break;
            case 'b': dst[j++] = '\b'; i++; break;
            case 'a': dst[j++] = '\a'; i++; break;
            case 'f': dst[j++] = '\f'; i++; break;
            case 'v': dst[j++] = '\v'; i++; break;
            case 'e': dst[j++] = '\33'; i++; break;
            case 'i': dst[j++] = '\''; i++; break; /* extra feature */
            case 'q': dst[j++] = '\"'; i++; break; /* extra feature */
            case 'x': i++;
                if (i < len && isxdigit(src[i]))
                {
                    register p4char a = src[i++]-'0';
                    if (a > '9') a -= 'A'-'9'+1;
                    if (i < len && isxdigit (src[i]))
                    {
                        a <<= 4;
                        if (src[i] <= '9') a |= src[i] - '0';
                        else a |= src[i] - 'A' + 10;
                    }
                    dst[j++] = a;
                }else{
                    p4_throw (P4_ON_INVALID_NUMBER);
                }
                break;
            default:
                if (! p4_isalnum (src[i]))
                    dst[j++] = src[i++];
                else if (isdigit (src[i]))
                {
                    register p4char a = src[i++]-'0';
                    if (i < len && isdigit (src[i]))
                    { a <<= 3; a |= src[i++]-'0'; }
                    if (i < len && isdigit (src[i]))
                    { a <<= 3; a |= src[i++]-'0'; }
                    dst[j++] = a;
                }
                else if ('A' <= src[i] && src[i] <= 'Z')
                {
                    dst[j++] = src[i++] & 31;
                }else{
                    p4_throw (P4_ON_INVALID_NUMBER);
                }
            }
        }
    }
    dst[j] = '\0'; return j;
}




/** 'C\\\"' ( [backslashed-strings_<">] -- bstr* )
 * scan the following text to create a literal just
 * like =>'C"' does, but backslashes can be used to
 * escape special chars. The rules for the backslashes
 * follow C literals, implemented techniques are
 * \n \r \b \a \f \v \e \777
 * and all non-alnum chars represent themselves, esp.
 * \" \' \ \? \! \% \( \) \[ \] \{ \} etcetera.
 * most importantly the doublequote itself can be escaped.
 * but be also informed that the usage of \' and \" is not
 * portable as some systems preferred to map [\'] into ["].
 * Here I use the experimental addition to map [\q] to ["] and [\i] to [']
 */
void FXCode (p4_c_backslash_quote)
{
    p4char* p;
    p4ucell l;

    if (STATE)
    {
        FX_COMPILE(p4_c_backslash_quote);
        p = HERE;
    }else{
        p = p4_pocket ();
    }
    p[0] = l = p4_backslash_parse_into ('"', p+1, 255, 127);
    if (STATE)
    {
        HERE += l+1;
        FX (p4_align);
    }
    FX_PUSH (p);
}
P4COMPILES (p4_c_backslash_quote, p4_c_quote_execution,
            P4_SKIPS_STRING, P4_DEFAULT_STYLE);

/** 'S\\\"' ( [backslashed-strings_<">] -- str cnt )
 * scan the following text to create a literal just
 * like =>'S"' does, but backslashes can be used to
 * escape special chars. The rules for the backslashes
 * follow C literals, implemented techniques are
 * \n \r \b \a \f \v \e \777
 * and all non-alnum chars represent themselves, esp.
 * \" \' \ \? \! \% \( \) \[ \] \{ \} etcetera.
 * most importantly the doublequote itself can be escaped.
 * but be also informed that the usage of \' and \" is not
 * portable as some systems preferred to map [\'] into ["].
 * Here I use the experimental addition to map [\q] to ["] and [\i] to [']
 */
void FXCode (p4_s_backslash_quote)
{
    p4char* p;
    p4ucell l;

    if (STATE)
    {
        FX_COMPILE(p4_s_backslash_quote);
        p = HERE;
    }else{
        p = p4_pocket ();
    }
    p[0] = l = p4_backslash_parse_into ('"', p+1, 255, 127);
    if (STATE)
    {
        HERE += l+1;
        FX (p4_align);
    }
    FX_PUSH (p+1);
    FX_PUSH (l);
}
P4COMPILES(p4_s_backslash_quote, p4_s_quote_execution,
           P4_SKIPS_STRING, P4_DEFAULT_STYLE);

/** 'Z\\\"' ( [backslashed-strings_<">] -- zstr* )
 * scan the following text to create a literal just
 * like =>'Z"' does, but backslashes can be used to
 * escape special chars. The rules for the backslashes
 * follow C literals, implemented techniques are
 * \n \r \b \a \f \v \e \777
 * and all non-alnum chars represent themselves, esp.
 * \" \' \ \? \! \% \( \) \[ \] \{ \} etcetera.
 * most importantly the doublequote itself can be escaped
 * but be also informed that the usage of \' and \" is not
 * portable as some systems preferred to map [\'] into ["].
 * Here I use the experimental addition to map [\q] to ["] and [\i] to [']
 */
void FXCode (p4_z_backslash_quote)
{
    p4char* p;
    p4ucell l;

    if (STATE)
    {
        FX_COMPILE(p4_z_backslash_quote);
        p = HERE;
        l = p4_backslash_parse_into ('"', p+sizeof(short), 65535, 32767);
    }else{
        p = p4_pocket ();
        l = p4_backslash_parse_into ('"', p+sizeof(short), 254, 126);
    }
    if (STATE)
    {
        HERE += l+sizeof(short);
        FX (p4_align);
        (*(short*)p) = ((p4char*)HERE - p);
    }
    FX_PUSH (p+sizeof(short));
}
P4COMPILES(p4_z_backslash_quote, p4_z_quote_XT,
           p4_z_quote_SEE, P4_DEFAULT_STYLE);


P4_LISTWORDSET (zchar) [] =
{
    P4_INTO ("FORTH", 0 ),
    P4_SXco ("Z\"",              p4_z_quote),
    P4_FXco ("ZCOUNT",           p4_zcount),
    P4_FXco ("ZSTRLEN",          p4_zstrlen),
    P4_FXco ("ZMOVE",            p4_zmove),
    P4_FXco ("ZPLACE",           p4_zplace),
    P4_FXco ("+ZPLACE",          p4_appendz),
    P4_FNYM ("APPENDZ",          "+ZPLACE"),
    P4_SXco ("S\\\"",	         p4_s_backslash_quote),
    P4_SXco ("C\\\"",            p4_c_backslash_quote),
    P4_SXco ("Z\\\"",            p4_z_backslash_quote),
    P4_INTO ("ENVIRONMENT", 0 ),
    P4_OCON ("ZCHAR-EXT",          2000 ),
    P4_OCON ("forth200x/escaped-strings",  2007 ),
    P4_SHOW ("X:escaped-strings", "forth200x/escaped-strings 2007" ),
};
P4_COUNTWORDSET (zchar, "ZCHAR-EXT - zero-terminated C-like charstrings");
