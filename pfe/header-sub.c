/** 
 *  Implements header creation and navigation.
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2008-04-20 04:46:30 $)
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: header-sub.c,v 1.3 2008-04-20 04:46:30 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-limits.h>
#include <pfe/term-sub.h>
#include <pfe/os-string.h>
#include <pfe/os-delay.h>
#include <pfe/logging.h>
#include <pfe/_missing.h>

/**
 * (DICTVAR) forth-thread variable runtime, => VARIABLE like
 */
FCode_RT (p4_dictvar_RT)
{
    FX_USE_BODY_ADDR;
    *--SP = (p4cell) ((char *) p4TH + FX_POP_BODY_ADDR[0]);
}
P4RUNTIME1_RT(p4_dictvar);

/**
 * (DICTGET) forth-thread constget runtime, => VALUE like
 */
FCode_RT (p4_dictget_RT)
{
    FX_USE_BODY_ADDR;
    *--SP = *(p4cell *) ((char *) p4TH + FX_POP_BODY_ADDR[0]);
}
P4RUNTIME1_RT(p4_dictget);

/**
 * make a new dictionary entry in the word list identified by wid 
 *                   (fixme: delete the externs in other code portions)
 */
_export p4_namebuf_t*
p4_header_comma (const p4_namechar_t *name, int len, p4_Wordl *wid)
{
    int hc;
#  if defined PFE_WTIH_FFA || defined PFE_WITH_FIG
#  define p4_ZNAMES_ALLOWED 0
#  else 
#  define p4_ZNAMES_ALLOWED 0 /*1*/
#  endif

    /* move exception handling to the end of this word - esp. nametoolong */
    if (len == 0)
        p4_throw (P4_ON_ZERO_NAME);
    if (len > NFACNTMAX || len > (1 << CHAR_BIT)-1) 
	if (! p4_ZNAMES_ALLOWED)
	{
	    P4_fail2 ("nametoolong: '%.*s'", len, name);
	    p4_throw (P4_ON_NAME_TOO_LONG);
	}

    if (REDEFINED_MSG && p4_search_wordlist (name, len, wid))
        p4_outf ("\n\"%.*s\" is redefined ", len, name);

    /* and now, do the p4_string_comma ... */
# if defined PFE_WITH_FFA
    /* for the FFA style we have to insert a flag byte before the 
     * string that might be HERE via a WORD call. However that makes
     * the string to move UP usually - so we have to compute the overall 
     * size of the namefield first and its gaps, then move it */ 
    DP += 2; DP += len; FX (p4_align); 
    p4_memmove (DP-len, name, len); /* i.e. #define NFA2LFA(p) (p+1+*p) */
    LAST = DP-len -1;      /* point to count-byte before the name */
    *LAST = len;           /* set the count-byte */
    LAST[-1] = '\x80';     /* set the flag-byte before the count-byte */
# elif defined PFE_WITH_FIG
    /* the FIG style variant is a bit more special - as it is trying to
     * be extra compatible with user code that expects N>LINK to actually
     * turn out to be "DUP C@ 31 AND + 1+" - i.e. the end of the name is
     * exactly the start of the link-field. This is based on the oldest
     * of the implementations that had not to watch out for address
     * alignment exceptions - but even the widespread m68k implementation
     * did need to align at 16-bit adresses. Therefore - we have to move
     * a name up being layed down at HERE via traditional WORD - but that
     * makes the string to move UP usually - an overlap for normal memcpy()
     */
    DP += 1; DP += len; FX (p4_align);
    p4_memmove (DP-len, name, len);
    LAST = DP-len -1;
    *LAST = len;
    *LAST |= '\x80';
# else
    /* traditional way - avoid copying if using WORD. Just look for the
     * only if() in this code which will skip over the memcpy() call if
     * WORD $HEADER, was called. At the same time we do not look for any
     * overlaps - when memcpy runs lower-to-upper address then this is
     * okay with strings shortened at HERE - but there *are* rare cases 
     * that this could fail. That's the responsibility of the user code
     * to avoid this by copying into a scratch pad first. Easy I'd say.
     */
    LAST = DP++;
    if (name != DP) p4_memcpy(DP, name, len);
    *LAST = len;
    if (p4_ZNAMES_ALLOWED && *LAST != len) { *LAST = 0; DP[len] = 0; len++; }
    *LAST |= '\x80'; 
    DP += len; FX (p4_align); 
# endif

    /* and register in LAST and the correct (hashed) WORDLIST thread */
    hc = (wid->flag & WORDL_NOHASH) ? 0 : p4_wl_hash (LAST+1, len); 
    FX_PCOMMA (wid->thread[hc]); /* create the link field... */
    wid->thread[hc] = LAST;
    return LAST;
}

#ifndef PFE_CALL_THREADING
_export p4_namebuf_t* 
p4_make_header (p4code cfa, char flags, 
                const p4_namechar_t* name, int count, p4_Wordl* wid)
{
    p4_namebuf_t* nfa = p4_header_comma (name, count, wid);
    P4_NFA_FLAGS (nfa) |= flags;
    FX_RCOMMA (cfa);

    fprintf(stderr,"<error obsolete word used: make-header word=%.*s>", 
	    count, name);
    fprintf(stderr,"<please use p4_header_comma with FX_RUNTIME1>");
    return nfa;
}

_export p4_namebuf_t*
p4_header (p4code cfa, char flags)
{
    p4_word_parseword (' '); *DP=0; /* PARSE-WORD-NOHERE */
    return p4_make_header (cfa, flags, PFE.word.ptr, PFE.word.len, CURRENT);
}
#endif

_export p4_namebuf_t*
p4_header_in (p4_Wordl* wid)
{
    p4_word_parseword (' '); *DP=0; /* PARSE-WORD-NOHERE */
    return p4_header_comma (PFE.word.ptr, PFE.word.len, wid);
}

/* -------------------------
 * navigation in the header 
 */

/*
  [NAME-COUNT/FLAGS] [NAME-AREA] [LFA] [CFA] [PFA]  -- fig-style
  [FLAGS] [NAME-COUNT] [NAME-AREA] [LFA] [CFA] [PFA] -- flags-style
  -- and for the does-code we change it to
  ... [CFA] [XFA] [PFA]
  where XFA points to the implementation for this word.
*/
  

 
#ifndef _export
/* you must differentiate between VAR-style body and DOES-style body */
# define P4_TO_LINK(C)     ((p4char**)(C) -1 )
# define P4_LINK_FROM(C)   ((p4xt)(C) + 1 )
# define P4_TO_BODY(C)     ((p4cell *)((p4xt)(C) + 1))
# define P4_BODY_FROM(P)   ((p4xt)((p4cell *)(P) - 1))
# define P4_TO_DOES_BODY(C)  ((p4cell *)((p4xt)(C) + 2))
# define P4_TO_DOES_CODE(C)  ((p4xcode **)((p4xt)(C) + 1))
# ifdef PFE_CALL_THREADING
extern const p4xcode* p4_to_code (p4xt xt);
# define P4_TO_CODE(C)     (p4_to_code((p4xt)C))
# else
# define P4_TO_CODE(C)     (C)
# endif
#endif

/* >BODY is known to work on both DOES-style and VAR-style words
 * and it will even return the thread-local address of remote-style words
 * (DOES-style words are <BUILDS CREATE and DEFER in ans-forth-mode)
 */
_export p4cell *
p4_to_body (p4xt xt)
{
    if (! xt) return P4_TO_BODY (xt);

    if (p4_LogMask & P4_LOG_DEBUG)
        if ((p4char*)xt < PFE.dict || PFE.dictlimit < (p4char*)xt)
            p4_abortq ("xt in '>BODY' out of range (not in my dict space)");

    if (P4_XT_VALUE(xt) == FX_GET_RT (p4_dictvar) || 
	P4_XT_VALUE(xt) == FX_GET_RT (p4_dictget)) 
        return ((p4cell*)( (char*)p4TH + P4_TO_BODY(xt)[0] ));
    else if (P4_XT_VALUE(xt) == FX_GET_RT (p4_builds) ||
             P4_XT_VALUE(xt) == FX_GET_RT (p4_does) || 
             P4_XT_VALUE(xt) == FX_GET_RT (p4_defer))
        return P4_TO_DOES_BODY(xt); 
    else /* it's not particularly right to let primitives return a body... */
        /* but otherwise we would have to if-check all known var-RTs ... */
        return P4_TO_BODY(xt);
}            

_export p4xt
p4_body_from (p4cell* body)
{
    if (! body) return P4_BODY_FROM (body);

    if (p4_LogMask & P4_LOG_DEBUG)
        if ((p4char*)body < PFE.dict || PFE.dictlimit < (p4char*)body)
            p4_abortq ("xt in '>BODY' out of range (not in my dict space)");

    { /* this one does not try to look up dictvars, it throws above on those */
        register p4xt xt = P4_BODY_FROM(body);
        if (P4_XT_VALUE(xt-1) == FX_GET_RT (p4_builds) ||
            P4_XT_VALUE(xt-1) == FX_GET_RT (p4_does) ||
            P4_XT_VALUE(xt-1) == FX_GET_RT (p4_defer))
        { 
	    xt--; /* skip extra-cell being not the BODY data but extra CODE */
	}
        return xt;
    }
}

_export p4_namebuf_t**
p4_name_to_link (const p4_namebuf_t* p)
{
    return (p4_namechar_t **) p4_aligned ((p4cell) (p+1 + P4_NFACNT(*p)) );
}

/*
 * scan backward for count byte preceeding name of definition
 * returns pointer to count byte of name field or NULL
 */
_export p4_namebuf_t *
p4_link_to_name (p4_namebuf_t **l)
{
    p4_char_t * p = (p4_char_t *) l;
    unsigned n;

  /* Skip possible alignment padding: */
    for (n = 0; *--p == '\0'; n++)
        if (n > sizeof (p4cell) - 1)
            return NULL;

#   define PRENFAWIDTH sizeof(p4cell) /* one or two byte */
  /* Scan for count byte. Note: this is not reliable! */
    for (n = 0; n < NFACNTMAX+PRENFAWIDTH; n++, p--)
    {
        /* traditional: search for CHAR of name-area with a hi-bit set
         * and assume that it is the flags/count field for the NAME */
        if (P4_NFA_x0x80(p) && (unsigned)P4_NFACNT(*p) == n)
            return p;
        if (! p4_isprintable (*p))
            return NULL;
    }
    return NULL;
}

_export p4_Semant * 
p4_to_semant (p4xt xt)
{
   /* I don't like this either. :-) */
# define TO_SEMANT(XT,ELEMENT) \
    ((p4_Semant *)((char *)XT - OFFSET_OF (p4_Semant, ELEMENT)))
    p4_Semant *s;

    s = TO_SEMANT (xt, exec[0]);
    if (s->magic == P4_SEMANT_MAGIC)
        return s;
    s = TO_SEMANT (xt, exec[1]);
    if (s->magic == P4_SEMANT_MAGIC)
        return s;
    return NULL;
# undef TO_SEMANT
}

_export p4_namebuf_t **
p4_to_link (p4xt xt)
{
    p4_Semant *s = p4_to_semant (xt);

    return s ? p4_name_to_link (s->name) : (p4_namebuf_t**)( xt - 1 ); 
}

#ifndef PFE_USE_OBSOLETED
# ifdef _K12_SOURCE
# define PFE_USE_OBSOLETED 1
# elif P4_LOG && (P4_LOG^P4_LOG_FATAL)
# define PFE_USE_OBSOLETED 1
# else
# define PFE_USE_OBSOLETED 0
# endif
#endif

#if PFE_USE_OBSOLETED
static void make_obsoleted_a_synonym (const p4char* p, p4xt xt)
{
    register p4char* q = p4_to_name ((p4xt)( *P4_TO_BODY(xt)));

#   ifdef __vxworks    
    P4_warn4 ("obsolete word %.*s used - use %.*s (only reported once)",
	      NFACNT(*p), p+1, NFACNT(*q), q+1);
#   endif

    FX (p4_cr);
    p4_outs (" ||  obsolete word  "); p4_dot_name(p); FX (p4_cr);
    p4_outs (" | please change to "); p4_dot_name(q); FX (p4_cr);
    if (q[1] != '_')
        p4_outs (" | (message reported only once per name"
                 " - otherwise treated as SYNONYM)");
    else
        p4_outs (" | (message reported only once per name"
                 " - otherwise still implemented)");
    FX (p4_cr);

    p4_delay (200); /* 200ms to get attention of user */

    P4_XT_VALUE(xt) = FX_GET_RT (p4_synonym); /* FX_GET_RUNTIME1(p4_synonym) */
}
#endif

_export p4xt
p4_name_from (const p4_namebuf_t *p)
{
    extern FCode(p4_synonym_RT);
    register p4xt xt = P4_LINK_FROM (p4_name_to_link (p));

#if PFE_USE_OBSOLETED
    if (P4_XT_VALUE(xt) == FX_GET_RT(p4_obsoleted)) 
	make_obsoleted_a_synonym (p, xt);
#endif

    if (P4_XT_VALUE(xt) == FX_GET_RT (p4_synonym)) 
	return (p4xt)( *P4_TO_BODY(xt) );
    else return xt;
}

_export p4_namebuf_t *
p4_to_name (p4xt c)
{
    return p4_link_to_name (p4_to_link (c));
}

_export void
p4_dot_name (const p4_namebuf_t *nfa)
{
    if (! nfa || ! P4_NFA_x0x80(nfa))
    {
        p4_outs ("<?""?""?> ");  /* avoid trigraph interpretation */
        return;
    }
    p4_type (nfa+1, NFACNT(*nfa));
    FX (p4_space);
}

/* ----------------------
 * compile words
 */
#if 0
/*macroized*/ void
p4_compile1(p4code code)
{
    /* IP -> WP == code ?? */
    if (*P4_WP != code)
        P4_throw (P4_ON_COMPILE_FAIL);

    p4_Seman* seman = P4_TO_BODY(P4_WP);
    if (seman->magic != P4_SEMANT_MAGIC)
        P4_throw (P4_ON_COMPILE_FAIL);
        
    FX_ZCOMMA(&seman->exec[0]);
}

/*macroized*/ void
p4_compile2(p4code code)
{
    /* IP -> WP == code ?? */
    if (*P4_WP != code)
        P4_throw (P4_ON_COMPILE_FAIL);

    p4_Seman* seman = P4_TO_BODY(P4_WP);
    if (seman->magic != P4_SEMANT_MAGIC)
        P4_throw (P4_ON_COMPILE_FAIL);
        
    FX_ZCOMMA(&seman->exec[1]);
}
#endif

/*@}*/

/*
 * Local variables:
 * c-file-style: "stroustrup"
 * End:
 */
