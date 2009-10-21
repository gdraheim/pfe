/**
 * -- Almost-Non-Volatile Environment Options
 *
 *  Copyright (C) Tektronix, Inc. 2001 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.5 $
 *     (modified $Date: 2008-05-01 18:26:24 $)
 *
 *  @description
 *       the openfirmware standard specifies some means to add
 *       options to a non-volatile ram-area (nvram) that are used
 *       in the bootup sequence of the forth-based bootsystem.
 *       Here we add an internal API for looking for bootup options,
 *       an internal API to add bootup options through commandline
 *       processing, and an external API to change the bootup options
 *       for a => COLD reboot or an => APPLICATION specific sequence.
 *       The external API will try to follow openfirmware as closely
 *       as possible without having a real non-volatile ram-area.
 *       Instead there is a session-struct that can be changed and
 *       from which multiple forth-threads can be instantiated
 *       later using those options. The forth-instantion processing
 *       is not supposed to scan for commandline options, which is
 *       a mere necessity in embedded enviroments where option
 *       transferal is done in a completly different way and where
 *       the forth thread is never killed but just stopped or
 *       restarted for various reasons. Even that there is no real
 *       nvram we add a cold-options reboot-area in this wordset.
 *       The option-ram is organized just along a normal dictionary
 *       just need to add a wordlist-handlestruct to find the
 *       definitions in this dictlike ram-portion.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) =
"@(#) $Id: option-ext.c,v 1.5 2008-05-01 18:26:24 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/option-ext.h>

#include <stdlib.h>
#include <pfe/os-string.h>

#include <pfe/logging.h>

#ifdef __vxworks
#include <sysSymTbl.h>
#else
#include <errno.h>
#endif
#include <pfe/os-ctype.h>

#define ___ {
#define ____ }

#define str_cpy(A,B) p4_strcpy((char*)(A),(B))

#ifndef _export
typedef p4_Session p4_Options;
#define p4_get_option_value  p4_search_option_value
#define p4_get_option_string p4_search_option_string
#endif

#define OPT (opt->opt)

#define IS_VALUE_RT(X) (*P4_TO_CODE(X) == PFX(p4_value_RT) \
                     || *P4_TO_CODE(X) == PFX(p4_constant_RT))

#define P4_TO_BODY_00_(_xt) (P4_TO_BODY(xt)[0])
#define P4_TO_BODY_01_(_xt) (P4_TO_BODY(xt)[1])

void FXCode_RT (p4_string_RT)
{   FX_USE_BODY_ADDR {
    char* str = *(char**) FX_POP_BODY_ADDR;
    FX_PUSH (str);
    FX_PUSH (p4_strlen(str));
}}
static void FXCode (p4_string) { /* dummy */ }
P4RUNTIME1(p4_string, p4_string_RT); /* ready for FX_GET_RT optimization */

/*
 * returns the CFA of the option if found
 *
 * Since this routine is sometimes run in problematic conditions,
 * during interrupt service handlers, before the pfe thread is
 * allocated, some other boot routines did nasty things with just
 * no catch frame around yet. We try to guard that with checking
 * the entries to be part of the OPT assigned space, anything
 * else is considered wrong and returns null.
 */
_export p4xt
p4_search_option (const p4char* nm, int l, p4_Options* opt)
{
    auto p4_char_t upper[32];

    if (l < 32)
    { p4_memcpy (upper, nm, l); p4_upper (upper, l); }
    else
    { *(int*)upper = 0; }

    if(0){P4_warn3 (" <!> '%.*s'/%i", l, nm, l);}

    /* compare with dict-sub:search_thread called by p4_search_wordlist */
    ___ p4char* t = OPT.last;
    while (OPT.dict < t+1 && t+1 <= OPT.dictlimit) /* excluding null(s) */
    {
        if(0){P4_warn3 (" <?> '%.*s'/%i", NAMELEN(t), NAMEPTR(t), NAMELEN(t));}

        if (! P4_NFA_xSMUDGED(t) && NAMELEN(t) == l)
        {
            if (! p4_memcmp (nm, NAMEPTR(t), l) || ! p4_memcmp (upper, NAMEPTR(t), l))
                return p4_name_from (t);

            /* omitted extra strncmpi here... and no warning... */
        }
        t = *p4_name_to_link (t);
    }____;
    return 0;
}

/*
 * look for string_RT options, free the malloc'ed string buffer,
 * and put a two_constant_RT in the execution token area. For a
 * limited time the mem space (originally referenced) will keep
 * the string until that area gets reallocated to another part.
 * However, this function is normally called _after_ pfeVM shutdown
 * and it is unlikely that (a) the memspace is realloc'ed and that
 * (b) the string value is used actually in some pfe context therefore.
 */
_export void
p4_invalidate_string_options (p4_Options* opt)
{
    p4char* t = OPT.last;
    while (t)
    {
        p4xt xt = p4_name_from (t);
        if (xt && *P4_TO_CODE(xt) == PFX(p4_string_RT))
        {
            free ((void*)( P4_TO_BODY_00_(xt) ));
            P4_XT_VALUE(xt) = FX_GET_RT (p4_two_constant);
        }
        t = *P4_TO_LINK(xt);
    }
}

/*
 * create a new header in the option-dict and return
 * the xt. Unlike other header-creations, here we never
 * smudge the name, and the caller is responsible to
 * setup the value of the CFA. If no header could be
 * created then the function will return null - check for that!
 */
_export p4xt
p4_create_option (const p4char* name, int len, int size, p4_Options* opt)
{
    /* compare with dict-sub:p4_header_comma */

    if (len == 0 || len > NAME_SIZE_MAX
      || OPT.dictlimit < OPT.dp + len + 2*sizeof(p4char) + 4*sizeof(p4cell) )
        return 0; /* invalid or dict exhausted */

    ___ p4char* link = OPT.last;


# if defined PFE_WITH_FIG
    OPT.dp += 1; OPT.dp += len; while (! P4_ALIGNED(OPT.dp)) OPT.dp++;
    p4_memmove (OPT.dp-len, name, len);
    OPT.last = OPT.dp-len -1;
    *OPT.last = len;
    *OPT.last |= '\x80';
#else
    OPT.last = OPT.dp++;
    if (name != OPT.dp) p4_memcpy (OPT.dp, name, len);
    *OPT.last = len;
    *OPT.last |= '\x80';
    OPT.dp += len; while (! P4_ALIGNED(OPT.dp)) { *OPT.dp++ = 0; };
#endif

    *P4_INC(OPT.dp, pfe_lfa_t) = link;
    link = OPT.dp; OPT.dp += size + sizeof (p4code);
    return (p4xt)( link ); ____;
}

/*
 * search the option value in the option-ram, if nothing
 * is found then return the argument default. The option-ram
 * is not changed.
 * (in vxworks/k12xx: lookup also p4__default_<optionname> datasymbol)
 * (in posixish os: lookup also PFE_<OPTIONNAME> environment variable)
 */
_export p4celll
p4_search_option_value (const p4char* nm, int l,
                        p4celll defval, p4_Options* opt)
{
    p4xt xt = p4_search_option (nm, l, opt);
    if (xt && IS_VALUE_RT(xt)) return P4_TO_BODY_00_(xt);
    /* else return deval; */
    if (l >= 32) return defval;

    { /* generic option passing via vx-start symbols-settings */
#      ifdef __vxworks
        long* symval;
        const p4char prefix[] = "p4__default_";
#      else
        const p4char prefix[] = "pfe_default_";
#      endif
#       define strlen_prefix (sizeof(prefix)-1)
        p4_char_t symbol[strlen_prefix+32+5];
        p4_char_t* s;

        if (*nm == '/') {
            p4_memcpy (&symbol[0], prefix, strlen_prefix);
            p4_memcpy (&symbol[strlen_prefix], nm+1, l-1);
            p4_memcpy (&symbol[strlen_prefix+l-1], "_size", 6);
        }else{
            p4_memcpy (&symbol[0], prefix, sizeof(prefix)-1);
            p4_memcpy (&symbol[strlen_prefix], nm, l);
            /*    */ symbol[strlen_prefix+l] = '\0';
        }
        /* forth-symbols may contain non-alnums, need to sanitize */
        for (s=symbol; *s; s++) if (! p4_isalnum(*(p4char*)s)) *s = '_';

#      ifdef __vxworks
        if (symFindByName (sysSymTbl, symbol, (char**) &symval, 0) == OK)
            if (symval)
            {
                P4_info4 ("seen '%.*s' = %ld (%s)", l, nm, *symval, symbol);
                return *symval;
            }
#      else
#       ifndef _toupper
#       define _toupper(X) toupper(X)
#       endif
        for (s=symbol; *s; s++) if (islower(*s)) *s = _toupper(*s);

        if ((s= (p4_char_t*) getenv((char*) symbol)))
        {
            int newval;  errno = 0; newval = atoi ((char*) s);
            if (! errno)
            {
                P4_info4 ("seen '%.*s' = %d (%s)", l, nm, newval, symbol);
                return newval;
            }
        }
#      endif
        P4_info4 ("keep '%.*s' = %ld (%s not found)",  l, nm, defval,
                  symbol);
    }
    return defval;
}

/*
 * return the value of an option that is already in the
 * option-ram. If no option is found, add the default we
 * give as an argument, and return this parameter as the result.
 */
_export p4cell*
p4_create_option_value (const p4char* nm, int l,
                        p4celll defval, p4_Options* opt)
{
    p4xt xt = p4_search_option (nm, l, opt);
    if (xt && IS_VALUE_RT(xt))
        return P4_TO_BODY(xt);
    else{
        xt = p4_create_option (nm, l, sizeof(p4cell), opt);
        if (! xt) return 0;
	P4_XT_VALUE(xt) = FX_GET_RT (p4_value);
        return (p4cell*)( OPT.dp = (p4char*) defval );
    }
}

/*
 * change the value of an option that is already in the
 * option-ram. If no option-var is found, create that option
 * and set the var to the argument value. Return the address
 * to the value, or null if the option-ram is filled up.
 */
_export p4cell*
p4_change_option_value (const p4char* nm, int l,
                        p4celll defval, p4_Options* opt)
{
    p4xt xt = p4_search_option (nm, l, opt);
    if (! xt || ! IS_VALUE_RT(xt))
    {
        xt = p4_create_option (nm, l, sizeof(p4celll), opt);
        if (! xt) return 0;
	P4_XT_VALUE(xt) = FX_GET_RT (p4_value);
    }
    P4_TO_BODY_00_(xt) = defval;
    return P4_TO_BODY(xt);
}

/*
 * search the option value in the option-ram, if nothing
 * is found then return the argument default. The option-ram
 * is not changed.
 */
_export const p4_char_t*
p4_search_option_string (const p4char* nm, int l,
                         const char* defval, p4_Options* opt)
{
    p4xt xt = p4_search_option (nm, l, opt);
    if (!xt || *P4_TO_CODE(xt) != PFX(p4_string_RT)) return (void*) defval;
    return (void*) P4_TO_BODY_00_(xt);
}

/*
 * search the option value in the option-ram, if nothing
 * is found then return the argument default. The option-ram
 * is not changed. This one uses an indirect reference!
 */
_export const char**
p4_lookup_option_string (const p4char* nm, int l,
                         const char** defval, p4_Options* opt)
{
    p4xt xt = p4_search_option (nm, l, opt);
    if (!xt || *P4_TO_CODE(xt) != PFX(p4_string_RT)) return defval;
    return (const char**) P4_TO_BODY(xt);
}

/*
 * return the address of the value of an option string that is already
 * in the option-ram. If no option is found, add the default we
 * give as an argument. The string is zero-terminated (!!)
 */
_export const char**
p4_create_option_string (const p4char* nm, int l,
                         const char* defval, p4_Options* opt)
{
    p4xt xt = p4_search_option (nm, l, opt);
    if (xt && *P4_TO_CODE(xt) == PFX(p4_string_RT))
        return (const char**) P4_TO_BODY(xt);
    else{
        xt = p4_create_option (nm, l, 2*sizeof(p4cell), opt);
        if (! xt) return 0;
        l = P4_TO_BODY_01_(xt) = p4_strlen (defval)+1;
        if ((nm = malloc (l))) {
            P4_XT_VALUE(xt) = FX_GET_RT (p4_string);
            P4_TO_BODY_00_(xt) = (p4cell) str_cpy (nm, defval);
        }else{
            P4_XT_VALUE(xt) = FX_GET_RT (p4_two_constant);
            P4_TO_BODY_00_(xt) = (p4cell) defval;
        }
        return (const char**) P4_TO_BODY(xt);
    }
}

/*
 * change the value of an option that is already in the
 * option-ram. If no option var is found, create that var
 * and set it to the argument string. The string is
 * zero-terminated. The return is the address of the
 * allocated string or zero if no option-var could be created.
 */
_export const char**
p4_change_option_string (const p4char* nm, int l,
                         const char* defval, p4_Options* opt)
{
    ___ p4xt xt = p4_search_option (nm, l, opt);
    if (! xt || *P4_TO_CODE(xt) != PFX(p4_string_RT))
    {
        xt = p4_create_option (nm, l, 2*(sizeof(p4cell)), opt);
        if (! xt) return 0;
	P4_XT_VALUE(xt) = FX_GET_RT (p4_string);
        /* fallthrough */
    }
    else if (P4_TO_BODY_01_(xt) < p4_strlen (defval)+1)
    {
        free ((void*) P4_TO_BODY_00_(xt));
        /* fallthrough */
    }else{
        str_cpy ((char*) P4_TO_BODY_00_(xt), defval);
        return (const char**) P4_TO_BODY(xt);
        /* keep TO_BODY(xt)[1] [at the allocated length] */
    }

    l = P4_TO_BODY_01_(xt) = p4_strlen (defval)+1;
    if ((nm = malloc (l))) {
        P4_TO_BODY_00_(xt) = (p4cell) str_cpy (nm, defval);
    }else{
        P4_XT_VALUE(xt) = FX_GET_RT (p4_two_constant);
        P4_TO_BODY_00_(xt) = (p4cell) defval;
    };
    return (const char**) P4_TO_BODY(xt); ____;
}

/*
 * change the value of an option that is already in the
 * option-ram. If no option var is found, create that var
 * and set it to the argument string. The string is
 * zero-terminated. The return is the address of the
 * string pointer or zero if no option-var could be created.
 *
 * If there had been an option then append the value
 * given. If that string was not empty then insert a
 * delimiter string in between the old and new value.
 * Nice to build path string options. The mechanism
 * automatically knows how to realloc the option string
 * buffer so that the whole path fits.
 */
_export const char**
p4_append_option_string (const p4char* nm, int l, char delimiter,
                         const char* defval, p4_Options* opt)
{
    p4xt xt = p4_search_option (nm, l, opt);
    if (! xt || *P4_TO_CODE(xt) != PFX(p4_string_RT))
    {
        xt = p4_create_option (nm, l, 2*(sizeof(p4cell)), opt);
        if (! xt) return 0;
	P4_XT_VALUE(xt) = FX_GET_RT (p4_string);
        P4_TO_BODY_01_(xt) = l = p4_strlen (defval) + 1;
        P4_TO_BODY_00_(xt) = (p4cell) malloc (l);
        str_cpy ((char*) P4_TO_BODY_00_(xt), defval);
    } else
    {
        l = p4_strlen ((char*) P4_TO_BODY_00_(xt)) + p4_strlen (defval) + 2;
        nm = realloc ((char*) P4_TO_BODY_00_(xt), l);
        if (! nm) goto _done; /* ENOMEM */
        P4_TO_BODY_00_(xt) = (p4cell) nm;
        P4_TO_BODY_01_(xt) = l;
        l = p4_strlen ((char*) P4_TO_BODY_00_(xt));
        if (*(char*) P4_TO_BODY_00_(xt) && delimiter)
            ((char*) P4_TO_BODY_00_(xt))[l++] = delimiter;
        str_cpy (((char*) P4_TO_BODY_00_(xt)) + l, defval);
    }

 _done:
    return (const char**) P4_TO_BODY(xt);
}

/* ---------------------------------------------------------------- */

/*
 * a string shall be converted into a cell that should flag the
 * size of something like the dictionary or stack. The second
 * argument gives an idea about the default if no size-specifier
 * has been provided. This routine does match the arg-option
 * processing of gforth - had an e-mail exchange with anton ertl
 * about this. Does return null on any errors.
 */
_export p4ucell
p4_convsize (const char* s, p4ucell elemsize)
{/* converts s of the format [0-9]+[bekMGT]? (e.g. 25k) into the number
    of bytes.  the letter at the end indicates the unit, where e stands
    for the element size. default is e */
    char *endp;
    p4ucell n,m;

    m = elemsize;
    n = strtoul(s,&endp,0);
    if (endp != NULL) {
        if (*endp == 'b' || *endp == 'c')
        { m = 1; if (*++endp == 'e') m *= elemsize; }
        else if (*endp == 'k' || *endp == 'K')
        { m = 1024; if (*++endp == 'e') m *= elemsize; }
        else if (*endp == 'M')
        { m = 1024*1024; if (*++endp == 'e') m *= elemsize; }
        else if (*endp == 'G')
        { m = 1024*1024*1024; if (*++endp == 'e') m *= elemsize;  }
        else if (*endp == 'T') {
#         if (PFE_SIZEOF_VOIDP > 4)
            { m = 1024L*1024*1024*1024; endp++; if (*++endp) m *= elemsize; }
#         else
            P4_fail1 ("size specification \"%s\" "
                      "too large for this machine\n", endp);
            return 0;
#         endif
        }
        if (*endp != 'e' && *endp != 'b' && *endp != 'B' && *endp != 0) {
            P4_fail2 ("cannot grok size specification %s: "
                      "invalid unit \"%s\"\n", s, endp);
            return 0;
        }
    }
    return m * n;
}

/** NVRAM,WORDS ( -- )
 *
 * Print a list of => WORDS in the NVRAM buffer. Try to show also the
 * current value, atleast for NVRAM numbers and strings. Words can be
 * added or changed with the help of => NVRAM,SET or => NVRAM,USE
 *
 * Values in the NVRAM buffer will survive a => COLD reboot, in many
 * hosted environments however the NVRAM will be lost on program exit.
 */
void FXCode (p4_nvram_words)
{
    FX (p4_cr);
    FX (p4_start_Q_cr);
    ___ p4char* t = PFE_set.opt.last;
    while (t)
    {
        p4xt xt = p4_name_from (t);
        if (! P4_NFA_xSMUDGED(t))
        {
            ___ static char spaces[] = "                  ";
            int namelen = NAMELEN(t); int x = namelen; if (x > 20) x = 20;
            p4_outf ("%.*s%s", namelen, NAMEPTR(t), spaces+x);
            ____;

            if (IS_VALUE_RT (xt))
            {
                p4_outf (" == %li", (p4celll) P4_TO_BODY_00_(xt));
            }
            else if (*P4_TO_CODE (xt) == PFX (p4_string_RT))
            {
                p4_outf (" =\" %s\"", (p4char*) P4_TO_BODY_00_(xt));
            }
            else {
                p4_outs (" (?)");
            }

            if (p4_Q_cr ())
                break;
            /* omitted extra strncmpi here... and no warning... */
        }
        t = *p4_name_to_link (t);
    }____;
}

/** NVRAM,AS ( str-ptr str-len "varname" -- )
 * set the NVRAM variable to the specified string.
 *
 * Some NVRAM strings do not take effect until next => COLD reboot.
 */
void FXCode (p4_nvram_as)
{
    int len = FX_POP;
    char* str = (char*) FX_POP;
    char* val = str;

    p4_word_parseword (' '); *DP=0; /* PARSE-WORD-NOHERE */
    if (! PFE.word.len || ! str) return;

    if (str[len])
    {
        val = malloc (len+1);
        if (! val) return;
        p4_memcpy (val, str, len);
        val[len] = '\0';
    }
    p4_change_option_string (PFE.word.ptr, PFE.word.len,
                             val, PFE.set);
    if (str != val)
    {
        free (val);
    }
}

/** NVRAM,TO ( number "varname" -- )
 * set the NVRAM variable to the specified number.
 *
 * Most NVRAM numbers do not take effect until next => COLD reboot.
 */
void FXCode (p4_nvram_to)
{
    p4celll val = FX_POP;

    p4_word_parseword (' '); *DP=0; /* PARSE-WORD-NOHERE */
    if (! PFE.word.len) return;

    p4_change_option_value (PFE.word.ptr, PFE.word.len,
                            val, PFE.set);
}

/** NVRAM,Z@ ( "varname" -- z-str )
 *
 * Return the string pointer of the NVRAM string item, or null if no
 * such item exists.
 */
void FXCode (p4_nvram_z_fetch)
{
    p4_word_parseword (' '); *DP=0; /* PARSE-WORD-NOHERE */
    FX_PUSH (p4_search_option_string (PFE.word.ptr, PFE.word.len,
                                      0, PFE.set));
}

/** NVRAM,S@ ( "varname" -- str-ptr str-len )
 *
 * Return the string span of the NVRAM string item, or double null if no
 * such item exists.
 */
void FXCode (p4_nvram_s_fetch)
{
    register const char* s;
    p4_word_parseword (' '); *DP=0; /* PARSE-WORD-NOHERE */
    s = (char*) p4_search_option_string (PFE.word.ptr, PFE.word.len,
					 0, PFE.set);
    FX_PUSH (s);
    FX_PUSH ((s ? p4_strlen (s) : 0));
}

/** NVRAM,?@ ( number "varname" -- number' )
 *
 * Return the value of the NVRAM value item, or leave the original
 * number untouched (i.e. the default value for your option).
 */
void FXCode (p4_nvram_Q_fetch)
{
    p4_word_parseword (' '); *DP=0; /* PARSE-WORD-NOHERE */
    SP[0] = p4_search_option_value (PFE.word.ptr, PFE.word.len,
                                    SP[0], PFE.set);
}

P4_LISTWORDSET (option) [] =
{
    P4_INTO ("EXTENSIONS", 0),
    P4_FXco ("NVRAM,WORDS", p4_nvram_words),
    P4_FXco ("NVRAM,Z@",    p4_nvram_z_fetch),
    P4_FXco ("NVRAM,S@",    p4_nvram_s_fetch),
    P4_FXco ("NVRAM,?@",    p4_nvram_Q_fetch),
    P4_FXco ("NVRAM,AS",    p4_nvram_as),
    P4_FXco ("NVRAM,TO",    p4_nvram_to),
    P4_INTO ("ENVIRONMENT", 0),
    P4_OCON ("OPTION-EXT",	2001 ),
};
P4_COUNTWORDSET (option, "Option Words For Almost-Non-Volatile Environment");

/*@}*/

/*
 * Local variables:
 * c-file-style: "stroustrup"
 * End:
 */
