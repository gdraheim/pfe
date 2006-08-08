/** 
 *  Implements dictionary and wordlists.
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 5.16 %
 *    (%date_modified: Mon Mar 12 10:32:05 2001 %)
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: dict-sub.c,v 0.32 2006-08-08 06:21:35 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-xtra.h>

#include <string.h>
#include <ctype.h>
#include <stddef.h> /*offsetof*/
#include <stdlib.h>

#include <pfe/def-comp.h>
#include <pfe/term-sub.h>
#include <pfe/lined.h>
#include <pfe/_missing.h>
#include <pfe/logging.h>

/*
 * A vocabulary is organized as a mixture between hash-table and
 * linked list. (This is a practice you can observe in several
 * systems.) It works like this: Given a name, first a hash-code is
 * generated. This hash-code selects one of several linked lists
 * called threads. The hooks to these threads are stored in a table.
 *
 * The body of a WORDLIST is essentially such a table of pointers to
 * threads, while in FIG-Forth it was just a pointer to the one and
 * only linked list a VOCABULARY consists of in FIG-Forth.
 */
_export int
p4_wl_hash (const char *s, int l)
/* s string, l length of string, returns hash-code for that name */
{
#  if 0 /*GD* original by DUZ */
    int n = *s++ - '@';

    while (--l > 0)
        n = n * 37 + *s++ - '@';	/* a maybe-stupid hash function :-) */
    return n & (THREADS - 1);	/* i.e. modulo threads */

#  else /*GD* more simpler, avoiding multiply */
    register char c = *s;

    while(--l > 0)
    { c += *s++; c ^= l; }
    return c & (THREADS - 1);
#  endif
}

/*
 * If we want to traverse a WORDLIST in it's entirety, we must follow
 * all threads simultaneously. The following definition eases this by
 * locating the thread with the hook pointing to the highest memory
 * location, assuming that this thread contains the latest definition
 * entered in the given WORDLIST. For usage refer to the definition of
 * WORDS.
 *
 * When following a wordlist using topmost, a copy of the word list
 * must be made. Everytime the topmost item was processed it must be
 * replaced by its successor in the linked list.
 */

/* find the thread with the latest word in the given word list */
_export char **
p4_topmost (p4_Wordl *w)

{
    int n = THREADS;
    char **p, **s = w->thread;

    for (p = s++; --n; s++)
        if (*s > *p)
            p = s;
    return p;
}

/* return the NFA of the latest definition in the CURRENT WORDLIST */
_export char * 
p4_latest (void) 
{
    return *p4_topmost (CURRENT);
}

/* --------------------------------
 * word list and forget 
 */

/** 
 * create a word list in the dictionary 
 */
_export p4_Wordl *
p4_make_wordlist (void)
{
    p4_Wordl *w = (Wordl *) DP;	/* allocate word list in HERE */
    P4_INC (DP, Wordl);
    
    ZERO (w->thread);		/* initialize all threads to empty */
    w->flag = WORDL_FLAG;         /* init flags from global flags */
    w->prev = VOC_LINK;		/* chain word list in VOC-LINK */
    VOC_LINK = w;
    if (w->flag & WORDL_CURRENT)
        w->also = CURRENT;	/* if WORDL_CURRENT, search also this */
    else
        w->also = 0;
    return w;
}

_export p4_Wordl *
p4_find_wordlist (const char* nm, int nmlen)
{
    p4_Wordl* wl;
    char* nfa;

    /* a special, since FORTH has no vocabulary_RT anymore */
    if (nmlen == 5 && ! memcmp (nm, "FORTH", 5))
	return PFE.forth_wl;

    for (wl = VOC_LINK; wl != NULL; wl = wl->prev)
    {
        nfa = p4_to_name (BODY_FROM (wl));
        if (NFACNT(*nfa) == nmlen && !strncmpi (nfa+1, nm, nmlen))
            return wl;
    }
    return 0;
}

/** ((FORGET)) 
 * remove words from dictionary, free dictionary space, this is the
 * runtime helper of => (FORGET)
 */
FCode (p4_forget_dp)
{
    register p4_Wordl *wl;
    register char* new_dp = PFE.forget_dp;

    /* unchain words in all threads of all word lists: */
    for (wl = VOC_LINK; wl; wl = wl->prev)
    {
        char **p = wl->thread;
        int i;
    
        for (i = THREADS; --i >= 0; p++)
	{  /* unchain words in thread: */
	    while (*p >= (char *) new_dp) 
            {
#	       ifdef PFE_WITH_FFA
               if (*_FFA(*p) & P4xONxDESTROY) {
	            P4_info2 (" destroy: \"%.*s\"", **p, *p+1);
		    p4_call (p4_name_from (*p));
                    new_dp = PFE.forget_dp; /* forget_dp is volatile */
                      /* and may have changed through recursive forget */
	       }
#              endif
	       *p = *p4_name_to_link (*p);
            }
        }
    }

    /* unchain word lists: */
    while (VOC_LINK >= (p4_Wordl *) new_dp) 
    {   
        {   /* delete from search-order */   
            int i;
            for (i=0; i < ORDER_LEN; i++) 
            {
                if (CONTEXT[i] == VOC_LINK) 
                {
                    CONTEXT[i] = NULL;
                    if (! PFE.atexit_running)
                    {
                        char* nfa = p4_to_name (BODY_FROM (VOC_LINK));
                        P4_warn3 ("deleted '%.*s' "
                                  "from context search-order [%i]", 
                                  NFACNT(*nfa), nfa+1, i);
                    }
                }
            
                if (PFE.dforder[i] == VOC_LINK) 
                {
                    PFE.dforder[i] = NULL;
                    if (! PFE.atexit_running)
                    {
                        char* nfa = p4_to_name (BODY_FROM (VOC_LINK));
                        P4_warn3 ("deleted '%.*s' "
                                  "from default search-order [%i]", 
                                  NFACNT(*nfa), nfa+1, i);
                    }
                }
            }
        }
        
        VOC_LINK = VOC_LINK->prev;
    }
    
    /* compact search-order */
    { register int i, j;
      for (i=0, j=0; i < ORDER_LEN; i++)
      {
        if (CONTEXT[i]) CONTEXT[j++] = CONTEXT[i];
      }
      while (j < ORDER_LEN) CONTEXT[j++] = NULL;

      for (i=0, j=0; i < ORDER_LEN; i++)
      {
        if (PFE.dforder[i]) PFE.dforder[j++] = PFE.dforder[i];
      }
      while (j < ORDER_LEN) PFE.dforder[j++] = NULL;
    }
    
    /* free dictionary space: */
    DP = (p4char *) new_dp; 
    LAST = NULL;
    PFE.forget_dp = 0;

    if (CURRENT >= (p4_Wordl *) new_dp) 
    {
        if (CONTEXT[0]) CURRENT = CONTEXT[0]; /* implicit DEFINITIONS */
        if (! PFE.atexit_running)
            p4_throw (P4_ON_CURRENT_DELETED);  /* and still throw */
    }
}

/** (FORGET)
 * forget anything above address
 */
_export void
p4_forget (char* above)
{
    if ((p4char*) above < FENCE)
        p4_throw (P4_ON_INVALID_FORGET);

    if (PFE.forget_dp) /* some p4_forget_dp already started */
    {
        /* P4_info1 ("recursive forget %p", above); */
        if (PFE.forget_dp > above) 
        {
            PFE.forget_dp = above; /* update p4_forget_dp argument */
        }
    }else{ 
        /* P4_info1 ("forget start %p", above); */
        PFE.forget_dp = above; /* put new argument for p4_forget_dp */
        FX (p4_forget_dp);     /* forget execution start */
    }
}

/*forward*/ p4_Head *
p4_make_head (const char *name, int len, char **nfa, p4_Wordl *wid);

/**
 * create a destroyer word. Upon =>'FORGET' the code will will be
 * run with the given argument. The structure is code/CFA and what/PFA.
 */
_export char*
p4_forget_word (const char *name, p4cell id, p4code ccode, p4cell what)
{
    char* nfa;
    char nm[255];

    sprintf (nm, name, id);
  
    p4_make_head (nm, strlen(nm), &nfa, PFE.atexit_wl)->cfa  = ccode;
#  ifdef PFE_WITH_FFA
    *_FFA(nfa) |= P4xONxDESTROY;
#  endif
    FX_COMMA(what);
    return nfa;
}

/* ------------------------------ 
 * search a header 
 */

static char *
search_thread (const char *nm, int l, char *t, p4cell wl_flag )
{
    if (l > NFACNTMAX)
        return NULL;

# if P4_LOG /* additional sanity check */
    if (p4_LogMask & P4_LOG_DEBUG) /* if any debug level */
        if (t && !((char*)PFE.dict <= t && t <= (char*)PFE.dictlimit)) 
        { 
            P4_fail3 ("hashlink pointer invalid %p in search for '%.*s'", 
              t, l, nm);
        }
# endif

    if( LOWER_CASE && (wl_flag & WORDL_NOCASE) )
    {
        /* this thread does only contain upper-case defs 
           AND lower-case input shall match definitions */
        while (t)
        {
            if ( !(*_FFA(t) & P4xSMUDGED) 
              && NFACNT(*t) == l && !strncmpi (nm, t+1, l))
                break;
            t = *p4_name_to_link (t);
        }
    }else{
        /* input is case-sensitive OR vocabulary contains mixed-case defs */
        while (t)
        {
            if ( !(*_FFA(t) & P4xSMUDGED) 
              && NFACNT(*t) == l && !strncmp (nm, t+1, l))
                break;
            t = *p4_name_to_link (t);
        }
    }

    return t;
}

_export char *
p4_search_wordlist (const char *nm, int l, const p4_Wordl *w)
{
    if( w->flag & WORDL_NOHASH )
    { return search_thread (nm, l, w->thread[0], w->flag ); }
    else
    { return search_thread (nm, l, w->thread[p4_wl_hash (nm, l)], w->flag ); }
}

/* search all word lists in the search order for name, return NFA */
_export char *
p4_find (const char *nm, int l)
{
    Wordl **p, **q, *wordl;
    char *w = NULL;
    int n = p4_wl_hash (nm, l);
    
    for (p = CONTEXT; !w && p <= &ONLY; p++)
    {
        if (*p == NULL)
            continue;
        for (q = CONTEXT; *q != *p; q++); /* don't search wordl twice */
        if (q != p)
            continue;
        wordl = *p;
        do {
            if( wordl->flag & WORDL_NOHASH )
                w = search_thread (nm, l, wordl->thread[0], wordl->flag );
            else
                w = search_thread (nm, l, wordl->thread[n], wordl->flag );
        } while (!w && (wordl=wordl->also));
    }
    return w;
}

/**
 * tick next word,  and
 * return count byte pointer of name field (to detect immediacy)
 */
_export char *
p4_tick_nfa (void) 
{
    char *p;

    p = p4_word (' ');
    p = p4_find ((char *) p + 1, *(p4char *) p);
    if (!p)
        p4_throw (P4_ON_UNDEFINED);
    return p;
}

/**
 * tick next word,  and return xt
 */
_export p4xt
p4_tick_cfa (void)
{
    return p4_name_from (p4_tick_nfa ());
}

/**
 * tick next word, store p4xt in xt, and
 * return count byte pointer of name field (to detect immediacy)
 */
_export char *
p4_tick (p4xt *xt)
{
    char *p = p4_tick_nfa ();
    *xt = p4_name_from (p);
    return p;
}

/* ---------------------------
 * create a header 
 */

/* writes counted string into dictionary, returns address */
_export char *
p4_string_comma (const char *s, int len)
{
    char *p = (char *) DP;
    
    if (len >= (1 << CHAR_BIT))
        p4_throw (P4_ON_ARG_TYPE);
    *DP++ = len;			/* store count byte */
    while (--len >= 0)		/* store string */
        *DP++ = (p4char) *s++;
    FX (p4_align);
    return p;
}

_export char*
p4_parse_comma(char del)
{
    char *p;
    p4ucell n;
    
    p4_parse (del, &p, &n);
    return p4_string_comma (p, (int) n);
}

_export char *
p4_word_comma (char del)
{
    char *p = p4_word (del);

    DP += *p + 1;
    FX (p4_align);
    return p;
}

static void			/* written to cfa by make_head() */
illegal_xt (void)		/* to give an error msg when calling */
{				/* a word without execution semantics */
    p4_throw (P4_ON_INVALID_NAME);
}

/**
 * make a new dictionary entry in the word list identified by wid 
 *                   (fixme: delete the externs in other code portions)
 */
_export p4_Head *
p4_make_head (const char *name, int len, char **nfa, p4_Wordl *wid)

{
    Head *h;
    int hc;

    if (len == 0)
        p4_throw (P4_ON_ZERO_NAME);
    if (len > NFACNTMAX)
        p4_throw (P4_ON_NAME_TOO_LONG);
    if (REDEFINED_MSG && p4_search_wordlist (name, len, CURRENT))
        p4_outf ("\n\"%.*s\" is redefined ", len, name);
    if (len >= (1 << CHAR_BIT))
        p4_throw (P4_ON_ARG_TYPE);

    /* p4_string_comma .. */
#if defined PFE_WITH_FFA
    /* have to compute the overall size of the namefield and its gaps */ 
    DP += 2; DP += len; FX (p4_align); 
    memmove (DP-len, name, len); /* i.e. #define NFA2LFA(p) (p+1+*p) */
    LAST = DP-len -1;      /* point to count-byte before the name */
    *LAST = len;           /* set the count-byte */
    LAST[-1] = '\x80';     /* set the flag-byte before the count-byte */
#elif defined PFE_WITH_FIG
    DP += 1; DP += len; FX (p4_align);
    memmove (DP-len, name, len);
    LAST = DP-len -1;
    *LAST = len;
    *LAST |= '\x80';
#else
    /* traditional way - avoid copying if using WORD */
    LAST = DP++;
    if (name != DP) memcpy(DP, name, len);
    *LAST = len;
    *LAST |= '\x80'; 
    DP += len; FX (p4_align); 
#endif
 
    h = (Head *) DP;
    P4_INC (DP, Head);

    if (wid->flag & WORDL_NOHASH)
        hc = 0;
    else
        hc = p4_wl_hash (LAST+1, len); 
    h->link = wid->thread[hc];
    wid->thread[hc] = LAST;
    h->cfa = illegal_xt;
#ifndef PFE_WITH_FIG
    h->aux = illegal_xt;
#endif

    if (nfa) *nfa = LAST;
    return h;
}

_export char* 
p4_header_from (p4code cfa, char flags, const char* name, int count)
{
    char* nfa;
    p4_make_head (name, count, &nfa, CURRENT)->cfa = cfa;
    *_FFA(nfa) |= flags;
    return nfa;
}

_export char*
p4_header (p4code cfa, char flags)
{
    char* p; p4ucell len;
    p4_parseword (' ', &p, &len);
    return p4_header_from (cfa, flags, p, (int)len);
}

/* -------------------------
 * navigation in the header 
 */
 
#ifndef _export
# define TO_BODY(C)	((p4cell *)((p4xt)(C) + 1))
# define BODY_FROM(P)	((p4xt)((p4cell *)(P) - 1))
# ifdef PFE_WITH_FIG
#  define P4_TO_DOES_BODY(C)  ((p4cell *)((p4xt)(C) + 2))
#  define P4_TO_DOES_CODE(C)  ((p4xt **)((p4xt)(C) + 1))
# else  
#  define P4_TO_DOES_BODY(C)  TO_BODY(C)
#  define P4_TO_DOES_CODE(C)  ((p4xt **)((p4xt)(C) - 1))
# endif
#endif

_export char **
p4_name_to_link (const char *p)
{
    return (char **) p4_aligned ((p4cell) (p+1 + NFACNT(*p)) );
}

/*
 * scan backward for count byte preceeding name of definition
 * returns pointer to count byte of name field or NULL
 */
_export char *
p4_link_to_name (char **l)
{
    char *p = (char *) l;
    unsigned n;

  /* Skip possible alignment padding: */
    for (n = 0; *--p == '\0'; n++)
        if (n > sizeof (p4cell) - 1)
            return NULL;

  /* Scan for count byte. Note: this is not reliable! */
    for (n = 0; n < NFACNTMAX; n++, p--)
    {
        if (*_FFA(p) & 0x80 && (unsigned)NFACNT(*p) == n)
            return p;
        if (!p4_isprint (*p))
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
    if (s->magic == SEMANT_MAGIC)
        return s;
    s = TO_SEMANT (xt, exec[1]);
    if (s->magic == SEMANT_MAGIC)
        return s;
    return NULL;
# undef TO_SEMANT
}

_export p4xt
p4_link_from (char **lnk)
{
    return &((Head *)(void *) lnk)->cfa;  /*NOTE:* assumes lnk is first field*/
}

_export char **
p4_to_link (p4xt xt)
{
    p4_Semant *s = p4_to_semant (xt);

    return s ? p4_name_to_link (s->name)
        : (char**)( ((void*) xt) - offsetof(Head,cfa) ); 
                                	/*NOTE:* assumes lnk is first field*/
}

_export p4xt
p4_name_from (const char *p)
{
    return p4_link_from (p4_name_to_link (p));
}

_export char *
p4_to_name (p4xt c)
{
    return p4_link_to_name (p4_to_link (c));
}

_export void
p4_dot_name (const char *nfa)
{
    if (!nfa || !(*_FFA(nfa) & 0x80))
    {
        p4_outs ("<\?\?\?> ");	/* avoid trigraph interpretation */
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

    p4_Seman* seman = TO_BODY(P4_WP);
    if (seman->magic != SEMANT_MAGIC)
        P4_throw (P4_ON_COMPILE_FAIL);
        
    FX_COMMA(&seman->exec[0]);
}

/*macroized*/ void
p4_compile2(p4code code)
{
    /* IP -> WP == code ?? */
    if (*P4_WP != code)
        P4_throw (P4_ON_COMPILE_FAIL);

    p4_Seman* seman = TO_BODY(P4_WP);
    if (seman->magic != SEMANT_MAGIC)
        P4_throw (P4_ON_COMPILE_FAIL);
        
    FX_COMMA(&seman->exec[1]);
}
#endif

/* ----------------------
 * words with wildcards 
*/

/*
 * Show words in word list matching pattern, and of one of the
 * categories in string `categories'. NULL pointer or zero length
 * string means all kinds of words.
 */
_export void
p4_wild_words (const p4_Wordl *wl, const char *pattern, const char *categories)
{
    Wordl wcopy = *wl;		/* clobbered while following it */
    char **t;

# ifndef WILD_TAB
# define WILD_TAB 26 /* traditional would be 20 (26*4=80), now 26*3=78 */
# endif

    FX (p4_cr);
    FX (p4_start_Q_cr);
    if (categories && *categories == '\0')
        categories = NULL;
    for (t = p4_topmost (&wcopy); *t; t = p4_topmost (&wcopy))
    {
        char wbuf[NFACNTMAX+1];
        char *w = *t;
        char **s = p4_name_to_link (w);
        int l = NFACNT(*w++);
        
        p4_store_c_string (w, l, wbuf, sizeof wbuf);
        if (p4_match (pattern, wbuf, wl->flag & WORDL_NOCASE))
	{
            char c = p4_category (*p4_link_from (s));
            if (!categories || strchr (categories, c))
	    {
                if (OUT+WILD_TAB - OUT%WILD_TAB + 2 + l > PFE.cols ||
		    OUT+WILD_TAB - OUT%WILD_TAB + WILD_TAB*2/3 > PFE.cols)
		{
                    if (p4_Q_cr ())
                        break;
		}else{
                    if (OUT)
                        p4_tab (WILD_TAB);
		}
                p4_outf ("%c %.*s ", c, l, w);
	    }
	}
        *t = *s;
    }
}

/* completion of word against dictionary */
static char *
search_incomplete (const char *name, int len, Wordl *w)
/*
 * traverses the entire given wordlist to find a matching word
 * caution: clobbers *w. This is needed to be able to continue the search.
 */
{
    char **t, *s;
    
    for (t = p4_topmost (w); *t; t = p4_topmost (w))
    {
        s = *t;
        *t = *p4_name_to_link (*t);
        if (NFACNT(*s) >= len 
          && ( (w->flag & WORDL_NOCASE) 
            ? strncmpi (s + 1, name, len) 
            : strncmp  (s + 1, name, len) 
               ) == 0)
            return s;
    }
    return NULL;
}

/*
 * Try to complete string in/len from dictionary.
 * Store completion in out (asciiz), return number of possible completions.
 * If display is true, display alternatives.
 * (if (display && !len) { don't print 200 words, just the number })
 */
static int
p4_complete_word (const char *in, int len, char *out, int display)
{
    Wordl w, *wl, **p;
    char *s = NULL, *t = NULL;	
    int n = 0, m = 0, cnt = 0, searched_n = 0;
    Wordl* searched[32] = {0}; 
    
    for (p = CONTEXT; p <= &ONLY; p++)
    {
        for (wl = *p; wl; wl = wl->also ) 
        {
            for (n=0; n < searched_n; n++)
                if (wl == searched[n])
                    break;       /* continue at second outer for */
            if (wl == searched[n]) 
                continue;        /* must expressed like that in C*/
        
            if (searched_n < 32) 
                searched[searched_n++] = wl;
             
            for (w = *wl; (t = search_incomplete (in, len, &w)) != NULL; cnt++)
            {
                if (display && len) 
                {
                    FX (p4_space);
                    p4_type_on_line (t + 1, NFACNT(*t));
                }
                if (cnt == 0) 
                {
                    s = t + 1;
                    m = NFACNT(*t);
                }else{
                    ++t;
                    for (n = 0; n < m; n++)
                        if (s[n] != t[n])
                            break;
                    m = n;
                }
            }
        }
    }
    if (cnt)
        p4_store_c_string (s, m, out, NFACNTMAX+1);
    if (display && !len)
    { p4_outf (" %i words ", cnt); }
    return cnt;
}

_export int
p4_complete_dictionary (char *in, char *out, int display)
{
    char *lw, buf[NFACNTMAX+1];
    int n;
    
    lw = strrchr (in, ' ');
    if (lw)
        lw++;
    else
        lw = in;
    memcpy (out, in, lw - in);
    n = p4_complete_word (lw, strlen (lw), buf, display);
    strcpy (&out[lw - in], buf);
    return n;
}

/* ---------------------------------------------------------------------- *
 * initial dictionary setup                                             
 */

FCode (p4_forget_wordset_RT)
{
    /* do nothing so far, forget_wordset_RT_ is just a type-marker */
}

extern int p4_slot_use (int*); /* FIXME: move to header file ? */
extern int p4_slot_unuse (int*); /* FIXME: move to header file ? */

static FCode (p4_forget_slot_RT)
{
    int* slot = (int*)(WP_PFA[0]);
    P4_info1 ("unuse load-slot '%i'", *slot);
   
    if (slot && *slot && PFE.p[*slot]) 
    {
        p4_xfree (PFE.p[*slot]); PFE.p[*slot] = 0;
    }
   
    p4_slot_unuse (slot);
}

static void
p4_load_slot_open (int* slot)
{
    int e;
    if (!slot) return;
                
    if ((e=p4_slot_use (slot))) 
    {
	P4_fail2 ("load-slot %i failed : %s", *slot, strerror(-e));
	return; 
    }
}

static void
p4_load_slot_init (int* slot, p4ucell size)
{
    if (!slot || !*slot || size < 4)
	return;

    if (!(PFE.p)[*slot]) 
    {
	(PFE.p)[*slot] = p4_calloc (1, size);
	P4_info3 ("load-slot %i size %lu alloc (%p)", 
                  *slot, (unsigned long)size, (PFE.p)[*slot]);
    }else{ 
	P4_warn2 ("load-slot %i already allocated (%p)", 
                  *slot, (PFE.p)[*slot]);
    }
    
    p4_forget_word ("(load-slot: %i)", *slot, 
		    p4_forget_slot_RT_, (p4cell) slot);
}

static void
p4_load_into (const char* vocname)
{
    Wordl* voc;
    if (! vocname) return;

    voc = p4_find_wordlist (vocname, strlen(vocname));
    if (voc) 
    {
	{    
	    register int i;
	    for (i=ORDER_LEN; --i > 0; )
		if (CONTEXT[i] == voc) 
		{
		    P4_info1 ("search also '%s' : already there", 
                              vocname);
		    return;
		}
	}
	FX (p4_also);    /* the top-of-order (CONTEXT) isn't changed */
	CONTEXT [1] = voc; /* instead we place it under-the-top */
	P4_info1 ("search also '%s' : done", vocname);
    }else{
	P4_warn2 ("search also failed: no '%s' vocabulary (%lu)", 
                  vocname, (unsigned long) strlen(vocname));
    }
} 

_export void
p4_load_words (const p4Words* ws, p4_Wordl* wid, int unused)
{
    Wordl* save_current = CURRENT;
    int k = ws->n;
    const p4Word* w = ws->w;
    char dictname[NFACNTMAX+1]; char* dn;
    int* slot = 0;

    if (!wid) wid = CURRENT;
    
    if (ws->name) 
    {  
	P4_info1 ("load '%s'", (ws->name));
	strncpy (dictname, ws->name, NFACNTMAX);
	dictname[NFACNTMAX] = '\0';
	if ((dn= strchr (dictname, ' '))
	    ||  (dn= strchr (dictname, '(')))
	    *dn = '\0';
    }else{
	sprintf (dictname, "%p", DP);
    }
    
    p4_forget_word ("wordset:%s", (p4cell) dictname,
		    p4_forget_wordset_RT_, 
		    (p4cell) (ws));
    
    for ( ; --k >= 0; w++)
    {
	wid = CURRENT;
	if (w)
	{
	    /* the C-name is really type-byte + count-byte away */
	    char type = *w->name;
	    const char* name = w->name+2;
	    int len = strlen (w->name+2);
	    void* ptr = w->ptr;
	    Wordl* wid = CURRENT;

	    /* and paste over make_word inherited from pre 0.30.28 times */
	    Head *h;
	    char *nfa;

	    /* part 1: specials... */
  
	    switch (type)
	    {
	    case p4_LOAD:
		if (ptr)
		    p4_load_words ((p4Words*) ptr, 0, 0); /* RECURSION !! */
		continue;
	    case p4_INTO:
	    {
		register void* p;
		p = p4_find_wordlist (name, strlen (name));
		if (p) 
		{   
		    P4_debug1 (13, "load into old '%s'", name);
		    CURRENT = p;
		}else{
		    P4_info1 ("load into new '%s'", name);
		    p4_header_from (p4_vocabulary_RT_ , P4xIMMEDIATE,
				    name, strlen(name));
		    
		    CURRENT = p4_make_wordlist (); 
		}

		if (ptr) 
		{
		    if (! CURRENT->also)
			CURRENT->also = p4_find_wordlist (ptr, strlen(ptr));

		    p4_load_into (name); /* search-also */
		}
	    } continue;
	    case p4_SLOT:
		slot = (int*) ptr;
		p4_load_slot_open (slot);
		continue;
	    case p4_SSIZ:
		p4_load_slot_init (slot, (p4ucell)(ptr));
	    case p4_XXCO: /* constructors are registered in => LOADED */
		wid = PFE.atexit_wl;
		break;
	    } /*switch*/

	    /* part 2: general... CREATE a name and setup its CFA field */

	    h = p4_make_head (name, len, &nfa, wid);
	    if ('A' <= type && type <= 'Z')
		*_FFA(nfa) |= P4xIMMEDIATE;
	    switch (type)
	    {
	    case p4_SXCO:
		h->cfa = ((p4_Semant *) ptr)->comp;
		if (! ((p4_Semant *)ptr)->name)
		    ((p4_Semant *)ptr)->name = name-1; 
		/* discard const */
		/* BEWARE: the arg' name must come from a wordset entry to
		   be both static and have byte in front that could be 
		   a maxlen
		*/
		continue;
	    case p4_IXCO:
	    case p4_FXCO:
		h->cfa = (p4code) ptr;
		continue;
	    case p4_XXCO:
		h->cfa = (p4code) ptr;
		((p4code)ptr) ();     /* runs *now* !! no checks !! */
		continue;
	    case p4_IVOC:
	    case p4_OVOC:
		/* creating a VO before IN will make sure that the */
		/* other words will go in there. Nice stuff, eh ;-) */
		h->cfa = p4_vocabulary_RT_ ;
		/* (((WList*) ptr)->wid = p4_make_wordlist ()); */
		continue;
	    case p4_DVAR:
		h->cfa = p4_dictvar_RT_ ;
		break;
	    case p4_DCON:
		h->cfa = p4_dictconst_RT_ ;
		break;
	    case p4_OVAR:
	    case p4_IVAR:
		h->cfa = p4_create_RT_ ;
		break;
	    case p4_OVAL:
	    case p4_IVAL:
		h->cfa = p4_value_RT_ ;
		break;
	    case p4_OFFS:
		h->cfa = p4_offset_RT_ ;
		break;
	    default:
		P4_fail3 ("unknown typecode for loadlist entry: "
			  "0x%x -> \"%.*s\"", 
			  type, len, name);
	    case p4_OCON:
	    case p4_ICON:
		h->cfa = p4_constant_RT_ ;
		break;
	    }
	    FX_COMMA (ptr);
	    continue;
	} /* if(w) */
    } /* for w in ws->w */

    CURRENT = save_current; /* should save_current moved to the caller? */
}

/**
 * the load_llist should be considered obsolote - the 0.30.x model
 * is essentially a flat-loadmodel via load_words. However it does
 * still work for backward-compatibility with source for binary-modules 
 * of version 0.29.x
 */
_export void 
p4_load_llist (void** list, p4_Wordl* wid, int extending)
{
    int e;
    Wordl* save_current = CURRENT;
    if (!list) return;
   
    while (*list) 
    { 
        /* switch(void*) P4_LOAD_XXXX */     
        if (*list == P4_LOAD_INTO) 
        {    
	    register void* p;
            register char* vocname = *(char**)(++list);
        
            /* 1. lookup Wordl, or create one if required */
        
            if (!extending)
                p = 0;
            else 
            { /* check if there is already a vocabulary of that name,
                 in which case we just want to add another few words to it
              */
                p = p4_search_wordlist (vocname, strlen (vocname), wid);
                if (p) 
                { 
                    p = p4_name_from ((char*) p);
                    if (*((p4xt)p) != &p4_vocabulary_RT_ )
                        p = 0;
                }
            }
  
            if (p) 
            {   
                CURRENT = (Wordl*)TO_BODY ((p4xt) p);
                P4_info1 ("load into old '%s'", vocname);
            }else{
                CURRENT = wid;
                p4_header_from (p4_vocabulary_RT_ , P4xIMMEDIATE,
                  vocname, strlen(vocname));
                 
                CURRENT = p4_make_wordlist (); 
                P4_info1 ("load into new '%s'", vocname);
            }

            /* 2. load Wordsets into Wordl */
            for (++list ; *list < 0 || *list > P4_LOAD_MAX; ++list)
            {
		p4_load_words (*(p4Words**)list, 0, 0);
            }
        }
        else if (*list == P4_LOAD_EXEC) 
        {    
            register void (*fx)(void) = *(void**)(++list);
          
            P4_info1 ("load exec %p", fx);
            if (fx) { fx(); }
        }
        else if (*list == P4_LOAD_ORDER) 
        {
            p4_load_into (*(void**)(++list));
        } 
        else if (*list == P4_LOAD_SLOT) 
        {   
            register int* slot = *(int**)(++list);
            register p4ucell size = (p4ucell) *(void**)(++list);
         
            if (!slot || size < 4)
                continue;
                
            if ((e=p4_slot_use (slot))) 
            {
                P4_fail2 ("load-slot %i failed : %s", *slot, strerror(-e));
                break; /* return !!!! */
            }

            if (!(PFE.p)[*slot]) 
            {
                (PFE.p)[*slot] = p4_calloc (1, size);
                P4_info3 ("load-slot %i size %lu alloc (%p)", 
                  *slot, (unsigned long) size, (PFE.p)[*slot]);
            }else{ 
                P4_warn2 ("load-slot %i already allocated (%p)", 
                  *slot, (PFE.p)[*slot]);
            }

            p4_forget_word ("(load-slot: %i)", *slot, 
              p4_forget_slot_RT_, (p4cell) slot);
        }else{
            list++;
        } /* switch LOAD_COMMAND */
    } /* while not LOAD_END */

    CURRENT = save_current;
}

/* ------------------------------------------------------------------- */

/** ((ONLY)) ( -- )
 * the only-vocabulary is special. Calling it will erase
 * the search => ORDER of vocabularies and only allows
 * to name some very basic vocabularies. Even => ALSO
 * is not available.
 example:
   ONLY FORTH ALSO EXTENSIONS ALSO DEFINITIONS
 */
FCode (p4_only_RT)
{
    ZERO (CONTEXT);
    CONTEXT[0] = CURRENT = ONLY;
}

/** FORTH ( -- )
 : FORTH FORTH-WORDLIST CONTEXT ! ;
 */
FCode (p4_forth_RT)
{
    CONTEXT[0] = PFE.forth_wl;
}

_export void
p4_preload_only (void)
{
    Wordl only;                   /* scratch ONLY word list */
    char* nfa;
    
    DP = (p4char *) &PFE.dict[1];
  
    /* Load the ONLY word list to the scratch ONLY: */
    memset (&only, 0, sizeof only);
    /* # only.flag |= WORDL_NOHASH; */
    p4_make_head ("ONLY", 4, &nfa, &only )->cfa = p4_only_RT_;
    ONLY = p4_make_wordlist ();
    /* # ONLY->flag |= WORDL_NOHASH; */
    COPY (ONLY->thread, only.thread);   /* Copy scratch ONLY to real ONLY */
    CURRENT = ONLY;

    /* FORTH -> ANS' -> ONLY */
    p4_header_from (p4_forth_RT_ , 0, "FORTH", 5);
    PFE.forth_wl = p4_make_wordlist (); 
    p4_header_from (p4_vocabulary_RT_, P4xIMMEDIATE, "ANS'", 4);
    PFE.forth_wl->also = p4_make_wordlist ();
    PFE.forth_wl->also->also = ONLY;

    /* destroyers :: LOADED */
    p4_header_from (p4_vocabulary_RT_ , 0, "LOADED", 6);
    PFE.atexit_wl = p4_make_wordlist (); 
    PFE.atexit_wl->flag |= WORDL_NOHASH; /* see environment_dump in core.c */

    /* ENVIRONMENT -> LOADED */
    p4_header_from (p4_vocabulary_RT_ , P4xIMMEDIATE, "ENVIRONMENT", 11);
    PFE.environ_wl = p4_make_wordlist ();
    PFE.environ_wl->also = PFE.atexit_wl;
}

/*@}*/

/*
 * Local variables:
 * c-file-style: "stroustrup"
 * End:
 */











