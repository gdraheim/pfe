/** 
 * -- Extended Environment related definitions
 * 
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 1.6 %
 *    (%date_modified: Mon Mar 12 10:32:14 2001 %)
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
"@(#) $Id: environ-ext.c,v 0.30 2001-03-12 09:32:14 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/engine-sub.h>

#include <string.h>

#include <pfe/version-sub.h>
#include <pfe/logging.h>

#if 0
/** ENVIRONMENT ( -- )
 * execute the => VOCABULARY runtime for the => ENVIRONMENT-WORDLIST
 : ENVIRONMENT  ENVIRONMENT-WORDLIST CONTEXT ! ;
 * see newstyle =>"ENVIRONMENT?"
 */
FCode (p4_environment)
{
    CONTEXT[0] = PFE.environ_wl;
}
#endif

/** ENVIRONMENT-WORDLIST ( -- wid )
 * return the => WORDLIST id of the => ENVIRONMENT so
 * it could be passed to => CURRENT (via =>"SET-CURRENT")
 : ENVIRONMENT-WORDLIST  ['] ENVIRONMENT >WORDLIST ;
 */
FCode (p4_environment_wordlist)
{
    FX_PUSH (PFE.environ_wl);
}

/* ------------------------------------------------------------------
 * required/requires facility
 * (will be in the upcoming forth standard scheduled for 2004)
 */

_export void*
p4_required (const char* name, const p4cell length)
{
    static const char* _file = "file:";
    void* p;
    p4cell len;
    char* path = p4_pocket ();

    strcpy (path, _file); strncat (path, name, length);
    len = strlen (path);
    if (len > 255) p4_throw (P4_ON_NAME_TOO_LONG);

    p = p4_search_wordlist (path, len, PFE.atexit_wl);
    if (! p)
    {
        p4_make_head (path, len, 0, PFE.atexit_wl)
            ->cfa = PFX(p4_constant_RT);
        p = DP; FX_COMMA (0);
        p4_included (name, length);
        *(p4cell*)p = (p4cell) DP;
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
FCode (p4_required)
{
    p4cell len = FX_POP;
    char* name = (char*) FX_POP;
    p4_required (name, len);
}

/** REQUIRES ( ... "name" -- ??? )
 * parses the next => WORD and passes it to => REQUIRED
 * this is the self-parsing version of => REQUIRED and
 * it does parrallel => INCLUDE w.r.t. => INCLUDED
 */
FCode (p4_requires)
{
    p4cell len;
    char* name;
    p4_parseword (' ', &name, &len);
    p4_required (name, len);
}

static FCode (p__host_system)
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

static FCode (p__forth_license)
{
    p4_strpush (p4_license_string ());
}

/*
 * Classifying Forth systems 7
 * by J Thomas on comp.lang.forth, 06.Feb.2001
 * -------------------------------------------
 * Thomas' spec did not reveal whether to return the
 * system-wide LOWER_CASE or the wordlist-local NOCASE-state.
 * For now, it returns the wordlist-local state.
 */
static FCode (p__case_sensitive_Q)
{
    FX_PUSH( (!( CURRENT->flag & WORDL_NOCASE )) );
}

/*
 * Classifying Forth systems 6
 * by J Thomas on comp.lang.forth, 03.Feb.2001
 *
 * FORTH-NAME ( -- caddr len )
 * The string should be a name, like ProForth or SwiftForth
 * -------------------------------------------
 * Thomas' spec did not reveal whether to return a
 * longname or shortname.
 * For now, it returns the short name.
 */
static FCode(p__forth_name)
{
    p4_strpush (PFE_PACKAGE);
}

/*
 * Classifying Forth systems 6
 * by J Thomas on comp.lang.forth, 03.Feb.2001
 *
 * FORTH-VERSION ( -- caddr len )
 * The string will give a version number, whatever the
 * implementor chooses.
 * -------------------------------------------
 * Thomas' spec did not reveal whether to return a
 * compact or talkative version spec.
 * For now, it returns the long version-string.
 */
static FCode(p__forth_version)
{
    p4_strpush (p4_version_string ());
}

/*
 * Classifying Forth systems 6
 * by J Thomas on comp.lang.forth, 03.Feb.2001
 *
 * FORTH-CONTACT ( -- caddr len )
 * The string will give a name-and-address or website etc.
 * -------------------------------------------
 * Thomas' spec did not reveal whether to return a
 * URL-form and a readable free-form. 
 * For now, it returns the URL-form of the sourceforge project.
 */
static FCode(p__forth_contact)
{
    p4_strpush ("http://pfe.sourceforge.net");
}

P4_LISTWORDS (environ) =
{
    CO ("ENVIRONMENT-WORDLIST",	p4_environment_wordlist),
    CO ("REQUIRED",             p4_required),
    CI ("REQUIRES",             p4_requires),
    P4_INTO ("ENVIRONMENT", 0),
    P4_OCON ("ENVIRON-EXT",	2000 ),
    P4_FXCO ("HOST-SYSTEM",	p__host_system ),
    P4_FXCO ("FORTH-LICENSE",	p__forth_license ),
    P4_FXCO ("CASE-SENSITIVE?", p__case_sensitive_Q),
    P4_FXCO ("FORTH-NAME",	p__forth_name),
    P4_FXCO ("FORTH-VERSION",   p__forth_version),
    P4_FXCO ("FORTH-CONTACT",   p__forth_contact),
};
P4_COUNTWORDS (environ, "Environment related definitions");

/*@}*/

