/**
 * -- Extended Environment related definitions
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
 *       gforth and win32for are also using an extra => ENVIRONMENT
 *       wordlist to register hints on the system environment. It
 *       is just a normal => VOCABULARY that can be searched&executed
 *       with => ENVIRONMENT? . In pfe, the environment wordlist does
 *       also register the extension status of the system including
 *       allocated slots, loaded binaries and => REQUIRED source files.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) =
"@(#) $Id$";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/engine-sub.h>

#include <pfe/os-string.h>
#include <pfe/os-ctype.h>

#include <pfe/version-sub.h>
#include <pfe/logging.h>

/* ------------------------------------------------------------------
 * required/requires facility
 * (will be in the upcoming forth standard scheduled for 2004)
 */

_export void*
p4_include_required (const p4_char_t* name, const p4cell length)
{
    void* p;
    p4cell len;
    void* path = p4_pocket ();

    if (length > 250) p4_throw (P4_ON_NAME_TOO_LONG);
    p4_strcpy (path, "-"); p4_strncat (path, (const char*) name, length);
    if (! p4_strchr (path, '.') && ! p4_strchr (path, '/'))
        p4_strcat (path, ".*");
    len = p4_strlen (path);
    if (len > 255) p4_throw (P4_ON_NAME_TOO_LONG);

    p = p4_search_wordlist (path, len, PFE.atexit_wl);
    if (! p)
    {
        p4_header_comma (path, len, PFE.atexit_wl);
        FX_RUNTIME1(p4_constant);
        p = HERE; FX_UCOMMA (0);
        p4_included (name, length);
        *(p4cell*)p = (p4cell) HERE; /* save to PFA */
        return p;
    }else{
        return 0;
    }
}

/** REQUIRED ( ... str-ptr str-len -- ??? )
 * the filename argument is loaded via => INCLUDED as
 * an extension package to the current system. The filename
 * is registered in the current => ENVIRONMENT so that it is
 * only => INCLUDED once (!!) if called multiple times via
 * => REQUIRED or => REQUIRES
 */
void FXCode (p4_include_required)
{
    p4cell len = FX_POP;
    p4_char_t* name = (p4_char_t*) FX_POP;
    p4_include_required (name, len);
}

/** REQUIRES ( ... "file-name" -- ... )
 * old variant of Forth200x => REQUIRE name
 */

/** REQUIRE ( ... "file-name" -- ... )
 * parses the next => WORD and passes it to => REQUIRED
 * this is the self-parsing version of => REQUIRED and
 * it does parrallel => INCLUDE w.r.t. => INCLUDED
 */
void FXCode (p4_include_require)
{
    p4_word_parseword (' '); *HERE=0; /* PARSE-WORD-NOHERE */
    p4_include_required (PFE.word.ptr, PFE.word.len);
}


#if 0
/** ENVIRONMENT ( -- )
 * execute the => VOCABULARY runtime for the => ENVIRONMENT-WORDLIST
 : ENVIRONMENT  ENVIRONMENT-WORDLIST CONTEXT ! ;
 ' ENVIRONMENT  ALIAS [ENV] IMMEDIATE
 * see newstyle =>"ENVIRONMENT?"
 */
void FXCode (p4_environment)
{
    CONTEXT[0] = PFE.environ_wl;
}
#endif

/** ENVIRONMENT-WORDLIST ( -- wid )
 * return the => WORDLIST id of the => ENVIRONMENT so
 * it could be passed to => CURRENT (via =>"SET-CURRENT")
 WORDLIST VALUE ENVIRONMENT-WORDLIST ;
 */

/** ENVIRONMENT? ( a1 n1 -- false | ?? true )
 * check the environment for a property, usually
 * a condition like questioning the existance of
 * specified wordset, but it can also return some
 * implementation properties like "WORDLISTS"
 * (the length of the search-order) or "#LOCALS"
 * (the maximum number of locals)

 * Here it implements the environment queries as a => SEARCH-WORDLIST
 * in a user-visible vocabulary called => ENVIRONMENT
 : ENVIRONMENT?
   ['] ENVIRONMENT >WORDLIST SEARCH-WORDLIST
   IF  EXECUTE TRUE ELSE  FALSE THEN ;

 * special extension: a search for CORE will also find a definition
 * of CORE-EXT or CORE-EXT-EXT or CORE-EXT-EXT-EXT - it just has to
 * be below the ansi-standard maximum length of 31 chars.

 * if a name like "dstrings-ext" is given, and no such entry
 * can be found, then envQ will try to trigger a => (LOADM) of
 * that module, in the hope that this implicit-load does in fact
 * define the answer. Use with care, it's a very new feature.
 */
_export p4_char_t*
p4_environment_Q(const p4_char_t* str, p4cell l)
{
    auto p4_char_t query[32];
    auto p4_char_t upper[32];
    register p4cell len = l;

    if (len < 31 )
    {   /* this scheme allows you also to submit a zero-terminated string */
        p4_memset (query, 0, sizeof(query));
        p4_memcpy (query, str, len);
        query[len] = '\0';
        len = p4_strlen ((char*) query); /* may be shorter than original */
        memcpy(upper, query, sizeof(upper));
        p4_upper (upper, len);
    }

    /* --- try to find it in environ_wl, possibly "-ext"-extended --- */

    if (0 < len && len < 32 && PFE.environ_wl)
    {
        int i = 3;
        while (--i)
        {
            p4_namebuf_t* nfa;
            nfa = p4_search_wordlist (query, len, PFE.environ_wl);
            if (nfa) return nfa;
            nfa = p4_search_wordlist (upper, len, PFE.environ_wl);
            if (nfa) return nfa;
            /* and now for the '-EXT' part */
            if (len < 25)
            {
                p4_strcat ((char*) query, "-ext");
                p4_strcat ((char*) upper, "-EXT");
                len = p4_strlen ((char*) query);
                continue;
            }else
                break;
        }
    }
#ifndef ENV_EXT_DLOPEN /* USER-CONFIG */
#define ENV_EXT_DLOPEN 1
#endif

#if ! ENV_EXT_DLOPEN
    return 0;
#else
    /* --- not found, try to see if it can be a module that defines it --- */
    if (25 < l || l < 4)
        return 0;

    if (! p4_strncmp ((char*) str + l-4, "-ext", 4))
    {
        extern void* p4_loadm (const p4char*, int);
        extern int p4_loadm_test (const p4char*, int);
        len = l;
        p4_strncpy ((char*) query, (char*) str, len);

        if (p4_loadm_test(query, len-4))
            return 0;
        if (! p4_loadm (query, len-4))
            return 0;
        /* ok: loaded something */
    }
    else if (! p4_strncmp ((char*) str + l-4, "-EXT", 4))
    {
        extern void* p4_loadm (const p4char*, int);
        extern int p4_loadm_test (const p4char*, int);
        len = l;
        p4_strncpy ((char*) query, (char*) str, len);
        if (p4_loadm_test (query, len-4))
            return 0;
        if (! p4_loadm (query, len-4))
        {
            /* check again with lowercased name for the bigcased question */
            p4_lower (query, len);
            if (p4_loadm_test (query, len-4))
                return 0;
            if (! p4_loadm (query, len-4))
                return 0;
        }
        /* ok: loaded something */
    }else if (str[0] == '-' && p4_isalnum(str[1]) &&
              (memchr (str, '.', l) || memchr (str, '/', l)))
    {
        if (str[l-1] == '*' && str[l-2] == '.')
            l -= 2; /* "l" is unused after this alternative */
        if (! p4_include_required (str+1, l-1))
            return 0;
    }else{
        return 0;
    }

    {	/* something new is loaded to memory, try again search_wl */
        register p4char* nfa = p4_search_wordlist (str, len, PFE.environ_wl);
        if (nfa) return nfa;
    }

    /* the new module did not define it explicitly... */
    p4_header_comma (str, len, PFE.environ_wl);
    FX_RUNTIME1(p4_value);
    FX_UCOMMA (P4_FALSE); /* flags its implicit definition */
    return p4_search_wordlist (str, len, PFE.environ_wl);
#endif
}

void FXCode (p4_environment_Q)
{
    p4cell len = SP[0];
    p4_namebuf_t* nfa;

#ifndef ENV_EXT_COUNTED /* USER-CONFIG */
#define ENV_EXT_COUNTED 1    /* may use a counted string for env-query? */
#endif

#if ENV_EXT_COUNTED
    if (len > 256 || -256 > len )
    {  /* this scheme allows you to submit a forth counted string */
        P4_warn ("counted string at query to ENVIRONMENT?");
        FX (p4_count);
        len = SP[0];
    }
#endif

    nfa = p4_environment_Q((p4_char_t*) SP[1], len);
    if (nfa)
    {
        FX_2DROP;
        if (PFE_IS_DESTROYER(nfa))
            FX_PUSH (p4_to_body(p4_name_from(nfa)));
        else
            p4_call (p4_name_from(nfa));
        FX_PUSH(P4_TRUE);
        return;
    }else{
        FX_DROP;
        *SP = 0;
        return;
    }
}

/** NEEDS ( name -- )
 *
 * A self-parsing variant of an environment-query check. It is similar
 * to a simulation like

 : NEEDS PARSE-WORD 2DUP ENVIRONMENT?
   IF DROP ( extra value ) 2DROP ( success - be silent )
   ELSE TYPE ." not available " CR THEN ;

 * however that would only match those worset-envqueries which return a
 * single extra item under the uppermost TRUE flag in the success case.
 * Instead it works more like

 : NEEDS PARSE-WORD 2DUP ENVIRONMENT-WORDLIST SEARCH-WORDLIST
   IF 2DROP ( success - be silent and just drop the parsed word )
   ELSE TYPE ." not available " CR THEN ;

 * however we add the same extension as in => ENVIRONMENT? as that
 * it can automatically load a wordset module to fullfil a query
 * that looks like "[wordsetname]-ext". Therefore, the following
 * two lines are pretty much identical:

 LOADM floating
 NEEDS floating-ext

 * the difference between the two: if somebody did provide a forth
 * level implementation of floating-ext then that implementation might
 * have registered a hint "floating-ext" in the environment-wordlist.
 * This extra-hint will inhibit loading of the binary module even if
 * it exists and not been loaded so far. The => LOADM however will
 * not check the => ENVIRONMENT-WORDLIST and only check its loadlist
 * of binary wordset modules in the system.
 *
 * It is therefore recommended to use => NEEDS instead of => LOADM
 * unless you know you want the binary module, quickly and uncondtionally.
 */
void FXCode (p4_needs_environment)
{
    p4char* nfa;
    p4_word_parseword (' '); *HERE=0; /* PARSE-WORD-NOHERE */
    nfa = p4_environment_Q (PFE.word.ptr, PFE.word.len);
    if (nfa) return;
    P4_note2 ("'%.*s' not found", (int) PFE.word.len, PFE.word.ptr);
    if (p4_OUT) FX (p4_cr); p4_outs (" .... "); /* like 'NEEDS ' */
    p4_type (PFE.word.ptr, PFE.word.len);
    p4_outs (" not available "); FX (p4_cr);
}


/** "ENVIRONMENT HOST-SYSTEM" ( -- str len )
 * returns the HOST-SYTEM type, can be used to distinguish
 * different runtime environments. (see =>".STATUS")
 */
static void FXCode (p__host_system)
{
#  ifndef TARGET_OS
#  if defined __target_os__
#  define TARGET_OS __target_os__
#  elif defined TARGET_OS_TYPE
#  define TARGET_OS TARGET_OS_TYPE
#  else
#  define TARGET_OS "unknown-system"
#  endif
#  endif

    p4_strpush (TARGET_OS);
}

static void FXCode (p__forth_license)
{
    p4_strpush (p4_license_string ());
}

/** "ENVIRONMENT CASE-SENSITIVE?" ( -- flag )
 * Classifying Forth systems 7
 * by J Thomas on comp.lang.forth, 06.Feb.2001
 * -------------------------------------------
 * Thomas' spec did not reveal whether to return the
 * system-wide LOWER_CASE or the wordlist-local NOCASE-state.
 * For now, it returns the wordlist-local state.
 */
static void FXCode (p__case_sensitive_Q)
{
    FX_PUSH( (!( CURRENT->flag & WORDL_NOCASE )) );
}

/** "ENVIRONMENT FORTH-NAME" ( -- caddr len )
 * Classifying Forth systems 6
 * by J Thomas on comp.lang.forth, 03.Feb.2001
 * -------------------------------------------
 *
 * The string should be a name, like ProForth or SwiftForth
 *
 * -------------------------------------------
 * Thomas' spec did not reveal whether to return a
 * longname or shortname.
 * For now, it returns the short name.
 */
static void FXCode(p__forth_name)
{
    p4_strpush (PFE_PACKAGE);
}

/** "ENVIRONMENT FORTH-VERSION" ( -- caddr len )
 * Classifying Forth systems 6
 * by J Thomas on comp.lang.forth, 03.Feb.2001
 * -------------------------------------------
 *
 * The string will give a version number, whatever the
 * implementor chooses.
 *
 * -------------------------------------------
 * Thomas' spec did not reveal whether to return a
 * compact or talkative version spec.
 * For now, it returns the long version-string.
 */
static void FXCode(p__forth_version)
{
    p4_strpush (p4_version_string ());
}

/** "ENVIRONMENT FORTH-CONTACT" ( -- caddr len )
 * Classifying Forth systems 6
 * by J Thomas on comp.lang.forth, 03.Feb.2001
 * -------------------------------------------
 *
 * The string will give a name-and-address or website etc.
 *
 * -------------------------------------------
 * Thomas' spec did not reveal whether to return a
 * URL-form and a readable free-form.
 * For now, it returns the URL-form of the sourceforge project.
 */
static void FXCode(p__forth_contact)
{
    p4_strpush ("http://pfe.sourceforge.net");
}

P4_LISTWORDSET (environ) [] =
{
    P4_INTO ("FORTH", "[ANS]"),
    P4_DVaL ("ENVIRONMENT-WORDLIST", environ_wl),
    P4_FXco ("ENVIRONMENT?",         p4_environment_Q),
    P4_FXco ("REQUIRED",             p4_include_required),
    P4_FXco ("REQUIRE",              p4_include_require),
    P4_FNYM ("REQUIRES",             "REQUIRE"),
    /* TODO: obsolete REQUIRES (in favor of Forth200x REQUIRE) */
    P4_FXco ("NEEDS",                p4_needs_environment),

    P4_INTO ("ENVIRONMENT", 0),
    P4_OCON ("ENVIRON-EXT",	  2000 ),
    P4_OCON ("forth200x/extension-query", 2005 ),
    P4_OCON ("forth200x/required",        2006 ),
    /* TODO: forth200x/required should be moved to FILE-EXT */
    P4_SHOW ("X:extension-query", "forth200x/extension-query 2005" ),
    P4_SHOW ("X:required",        "forth200x/required 2006" ),
    P4_FXCO ("HOST-SYSTEM",	p__host_system ),
    P4_FXCO ("FORTH-LICENSE",	p__forth_license ),
    P4_FXCO ("CASE-SENSITIVE?", p__case_sensitive_Q),
    P4_FXCO ("FORTH-NAME",	p__forth_name),
    P4_FXCO ("FORTH-VERSION",   p__forth_version),
    P4_FXCO ("FORTH-CONTACT",   p__forth_contact),
};
P4_COUNTWORDSET (environ, "Environment related definitions");

/*@}*/
