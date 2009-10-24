/**
 *  Implements dictionary and wordlists.
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author$)
 *  @version $Revision$
 *     (modified $Date$)
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) =
"@(#) $Id$";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-limits.h>

#include <pfe/os-string.h>
#include <pfe/os-ctype.h>
#include <pfe/os-delay.h>
#include <stddef.h> /*offsetof*/
#include <stdlib.h>

#include <pfe/def-comp.h>
#include <pfe/term-sub.h>
#include <pfe/lined.h>
#include <pfe/_missing.h>
#include <pfe/logging.h>

#define UPPERMAX 32
#define UPPERCOPY(_upper_, _nm_, _l_) \
        if ((_l_) < UPPERMAX) {       \
          p4_memcpy ((_upper_), (_nm_), (_l_)); \
          p4_upper ((_upper_), (_l_)); \
        } else { \
          *(int*)(_upper_) = 0; \
        }


/**
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
int
p4_wl_hash (const p4_char_t *s, int l)
/* s string, l length of string, returns hash-code for that name */
{
    register p4_char_t c = *s;

    while(--l > 0)
    { c += *s++; c ^= l; }
    return c & (THREADS - 1);
}

/**
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

/**
 * find the thread with the latest word in the given word list
 */
p4char **
p4_topmost (p4_Wordl *w)

{
    int n = THREADS;
    p4char **p, **s = w->thread;

    for (p = s++; --n; s++)
        if (*s > *p)
            p = s;
    return p;
}

/** return the NFA of the latest definition in the CURRENT WORDLIST */
p4char *
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
p4_Wordl *
p4_make_wordlist (p4_namebuf_t* nfa)
{
    p4_Wordl *w = (Wordl *) HERE; /* allocate word list in HERE */
    P4_INC (HERE, Wordl);

    _p4_buf_zero (w->thread);   /* initialize all threads to empty */
    w->nfa = nfa;               /* set name for the wordlist (if any) */
    w->flag = WORDL_FLAG;       /* init flags from global flags */
    w->prev = VOC_LINK;         /* chain word list in VOC-LINK */
    VOC_LINK = w;
    w->id = w->prev ? (w->prev->id << 1) : 1;
    if (w->flag & WORDL_CURRENT)
        w->also = CURRENT;      /* if WORDL_CURRENT, search also this */
    else
        w->also = 0;
    return w;
}

/** FIND-WORDLIST is mostly required by => LOAD-WORDS  */
p4_Wordl *
p4_find_wordlist (const p4_char_t* nm, int nmlen)
{
    auto p4_char_t upper[UPPERMAX];
    UPPERCOPY(upper, nm, nmlen)

    /* a special, since FORTH has no vocabulary_RT anymore */
    if (nmlen == 5 && ! p4_memcmp (nm, "FORTH", 5))
        return PFE.forth_wl;
    if (nmlen == 11 && ! p4_memcmp (nm, "ENVIRONMENT", 11))
        return PFE.environ_wl;

    p4_Wordl* wl;
    for (wl = VOC_LINK; wl ; wl = wl->prev)
    {
        p4_namebuf_t* nfa = wl->nfa;
        if (! nfa) continue;
        if (NAMELEN(nfa) != nmlen) continue;
        if (p4_memequal (NAMEPTR(nfa), nm, nmlen) ||
            p4_memequal (NAMEPTR(nfa), upper, nmlen))
            return wl;
    }
    return 0;
}

/** ((FORGET))
 * remove words from dictionary, free dictionary space, this is the
 * runtime helper of => (FORGET)
 */
void FXCode (p4_forget_dp)
{
    register p4_Wordl *wl;
    register p4char* new_here = PFE.forget_dp;

    /* unchain words in all threads of all word lists: */
    for (wl = VOC_LINK; wl; wl = wl->prev)
    {
        p4_namebuf_t **p = wl->thread;
        int i;

        if (0) if (wl->nfa)
            P4_debug2(4, "\"%.*s\"", NAMELEN(wl->nfa), NAMEPTR(wl->nfa));

        for (i = THREADS; --i >= 0; p++)
        {  /* unchain words in thread: */
            while (*p >= new_here)
            {
                if (PFE_IS_DESTROYER(*p))
                {
                    P4_info2 (" destroy: \"%.*s\"", NAMELEN(*p), NAMEPTR(*p));
                    p4_call (p4_name_from (*p));
                    new_here = PFE.forget_dp; /* forget_dp is volatile */
                    /* and may have changed through recursive forget */
                }
                *p = *p4_name_to_link (*p);
            }
        }
    }

    /* unchain word lists: */
    while (VOC_LINK && VOC_LINK >= (p4_Wordl *) new_here)
    {
        {   /* delete from search-order */
            int i;
            for (i=0; i < PFE_set.wordlists; i++)
            {
                if (CONTEXT[i] == VOC_LINK)
                {
                    CONTEXT[i] = NULL;
                    if (! PFE.atexit_running)
                    {
                        const p4char* nfa = VOC_LINK->nfa ? VOC_LINK->nfa
                            : (const p4char*) "\1?";
                        P4_note3 ("deleted '%.*s' "
                                  "from context search-order [%i]",
                                  NAMELEN(nfa), NAMEPTR(nfa), i);
                    }
                }

                if (PFE.dforder[i] == VOC_LINK)
                {
                    PFE.dforder[i] = NULL;
                    if (! PFE.atexit_running)
                    {
                        const p4char* nfa = VOC_LINK->nfa ? VOC_LINK->nfa
                            : (const p4char*) "\1?";
                        P4_note3 ("deleted '%.*s' "
                                  "from default search-order [%i]",
                                  NAMELEN(nfa), NAMEPTR(nfa), i);
                    }
                }
            }
        }

        VOC_LINK = VOC_LINK->prev;
    }

    /* compact search-order */
    { register int i, j;
      for (i=0, j=0; i < PFE_set.wordlists; i++)
      {
        if (CONTEXT[i]) CONTEXT[j++] = CONTEXT[i];
      }
      while (j < PFE_set.wordlists) CONTEXT[j++] = NULL;

      for (i=0, j=0; i < PFE_set.wordlists; i++)
      {
        if (PFE.dforder[i]) PFE.dforder[j++] = PFE.dforder[i];
      }
      while (j < PFE_set.wordlists) PFE.dforder[j++] = NULL;
    }

    /* free dictionary space: */
    HERE = (p4_byte_t *) new_here;
    LAST = NULL;
    PFE.forget_dp = 0;

    if (CURRENT >= (p4_Wordl *) new_here)
    {
        if (CONTEXT[0]) CURRENT = PFE.forth_wl; /* initial CURRENT */
        if (! PFE.atexit_running)
            p4_throw (P4_ON_CURRENT_DELETED);  /* and still throw */
    }
}

/** (FORGET)
 * forget anything above address
 */
void
p4_forget (p4_byte_t* above)
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

void FXCode_RT (p4_destroyer_RT)
{
    /* this code is a trampoline for ITC code not using an FFA flag.
     * we just expect the a prior p4_call in p4_forget has setup an
     * appropriate BODY pointer - either it goes through a p4WP or
     * indirectly through p4IP. All we have to do is, to fetch the
     * actual ccode from PFA[1] and branch down into the target code.
     */
#   if ! defined PFE_CALL_THREADING
    ((p4code*)(P4_WP_PFA))[1](FX_VOID);
#   elif ! defined PFE_SBR_CALL_THREADING /* not actually ever compiled */
    ((p4code*)(*IP))[1](FX_VOID); /* CTC, (*IP) would have the BODY-pointer */
#   else
    P4_fail (
        "you are not supposed to have a destroyer_RT in this configuration");
#   endif
}
P4RUNTIME1_RT(p4_destroyer);

/**
 * create a destroyer word. Upon =>'FORGET' the code will will be
 * run with the given argument. The structure is code/CFA and what/PFA.
 */
p4_namebuf_t*
p4_forget_word (const char *name, p4cell id, p4code ccode, p4cell what)
{
    p4_char_t nm[255]; /* better as POCKET ? */
    sprintf ((char*) nm, name, id);

#  if ! defined PFE_CALL_THREADING
    {
        p4_header_comma (nm, p4_strlen((char*) nm), PFE.atexit_wl);
        P4_NAMEFLAGS(LAST) |= (P4xIMMEDIATE|P4xONxDESTROY);
        FX_RUNTIME1_RT (p4_destroyer);
        FX_VCOMMA (what); /*pfa*/
        FX_RCOMMA (ccode); /*pfa+1*/
    }
#  else /* PFE_CALL_THREADING and up, p4_call uses INFO-block */
    {
        static const char dtor_type[] = { p4_DTOR, 0 }; /* = "~" */
        p4Word* w = (p4Word*) p4_HERE;
        FX_RCOMMA (dtor_type); /* trampoline with body */
        FX_RCOMMA (ccode);
        p4_header_comma (nm, p4_strlen((char*) nm), PFE.atexit_wl);
        P4_NAMEFLAGS(LAST) |= (P4xIMMEDIATE|P4xONxDESTROY);
        FX_PCOMMA (w);    /* cfa = word-comp-info */
        FX_VCOMMA (what); /* pfa */
    }
#  endif

    return LAST;
}

/** ------------------------------
 * search a header
 */
static p4_namebuf_t*
search_thread (const p4_char_t *nm, int l, p4_namebuf_t *t, const p4_Wordl* wl)
{
    auto p4_char_t upper[UPPERMAX];
    register p4cell wl_flag = wl->flag;

# if P4_LOG /* additional sanity check */
    if (p4_LogMask & P4_LOG_DEBUG) /* if any debug level */
        if (t && !(DICT_BASE <= t && t <= DICT_LIMIT))
        {
            P4_fail3 ("hashlink pointer invalid %p in search for '%.*s'",
              t, l, nm);
        }
# endif

    if( UPPER_CASE && (wl_flag & WORDL_UPPER_CASE) && l < UPPERMAX)
    {   /* note: p4_match/p4_search_incomplete */
        UPPERCOPY (upper, nm, l);

        /* this thread does contain some upper-case defs
           AND lower-case input shall match those definitions */
        while (t)
        {
            if (! P4_NAMExSMUDGED(t) && NAMELEN(t) == l)
            {
                if (p4_memequal (nm, NAMEPTR(t), l))  break;
                if (p4_memequal (upper, NAMEPTR(t), l)) break;
            }
            t = *p4_name_to_link (t);
        }
    }else{
        /* input is case-sensitive OR vocabulary contains mixed-case defs */
        while (t)
        {
            if (! P4_NAMExSMUDGED(t) && NAMELEN(t) == l)
            {
                if (p4_memequal (nm, NAMEPTR(t), l))  break;
            }
            t = *p4_name_to_link (t);
        }
    }

    return t;
}

/** search headers in a wordlist */
p4_namebuf_t*
p4_search_wordlist (const p4_char_t *nm, int l, const p4_Wordl *w)
{
    if( w->flag & WORDL_NOHASH )
    { return search_thread (nm, l, w->thread[0], w ); }
    else
    { return search_thread (nm, l, w->thread[p4_wl_hash (nm, l)], w ); }
}

/** continue search headers in a wordlist */
p4_namebuf_t*
p4_next_search_wordlist (p4_namebuf_t* last, const p4_char_t* nm, int l, const p4_Wordl* w)
{
    if (! last) return last;
    return search_thread (nm, l, *p4_name_to_link(last), w );
}

/** FIND
 *  search all word lists in the search order for name, return NFA
 * (we use the id speedup here - the first WLs have each a unique bitmask
 *  in the wl->id. Especially the FORTH wordlist can be present multiple
 *  time - even in being just search via wl->also. With w->id each is just
 *  searched once - atleast for each of the WLs that have gotten an id-bit
 *  which on a 32bit system are 32 WLs - enough for many system setups.
 *  It might be possible to use the old code even here (that walked the
 *  ORDER to see if the next WL is present in an earlier slot) but in a
 *  system with so many vocs it is quite improbable to find duplicates
 *  other than the basic vocs like FORTH in there anyway - so we use this
 *  one that might search a WL twice theoretically. Tell me of occasions
 *  where that is really a problem - in my setups it happens that the ORDER
 *  overflows much before getting duplicates other than the basic wordlists.
 */
p4char *
p4_find (const p4_char_t *nm, int l)
{
    register Wordl **p;
    register Wordl *wordl;
    register p4_namebuf_t *w = NULL;
    register int n = p4_wl_hash (nm, l);
    register p4ucell searched = 0;

    for (p = CONTEXT; p <= &ONLY; p++)
    {
        for (wordl = *p; wordl ; wordl=wordl->also)
        {
            if (searched&wordl->id)
                continue;
            searched |= wordl->id;

            if( wordl->flag & WORDL_NOHASH )
                w = search_thread (nm, l, wordl->thread[0], wordl );
            else
                w = search_thread (nm, l, wordl->thread[n], wordl );

            if (w) return w;
        }
    }
    return w; /*0*/
}

/**
 * tick next word,  and
 * return count byte pointer of name field (to detect immediacy)
 */
p4char *
p4_tick_nfa (void)
{
    register p4char *p;

    p4_word_parseword (' '); *HERE=0; /* PARSE-WORD-NOHERE */
    p = p4_find (PFE.word.ptr, PFE.word.len);
    if (! p)
        p4_throw (P4_ON_UNDEFINED);
    return p;
}

/**
 * tick next word,  and return xt
 */
p4xt
p4_tick_cfa (void)
{
    return p4_name_from (p4_tick_nfa ());
}

/* ---------------------------
 * create a header
 */

/** writes counted string into dictionary, returns address */
p4_charbuf_t*
p4_string_comma (const p4_char_t* s, int len)
{
    p4_charbuf_t *p = HERE;

    if (len >= (1 << CHAR_BIT))
        p4_throw (P4_ON_ARG_TYPE);
    *HERE++ = len;                /* store count byte */
    while (--len >= 0)          /* store string */
        *HERE++ = *s++;
    FX (p4_align);
    return p;
}

/*
 *  char*                       |* use FX(p4_parse_comma_quote) !!! *|
 *  p4_parse_comma(char del)
 * {
 *    p4_word_parse (del); |* PARSE-WORD-NOTHROW *|
 *    return p4_string_comma (PFE.word.ptr, (int) PFE.word.len);
 * }
 */

/* ----------------------
 * words with wildcards
*/

/**
 * Show words in word list matching pattern, and of one of the
 * categories in string `categories'. NULL pointer or zero length
 * string means all kinds of words.
 */
void
p4_wild_words (const p4_Wordl *wl, const p4char *pattern, const char *categories)
{
    p4char **t;
    /* Wordl wcopy = *wl;          // clobbered while following it */
    Wordl wcopy; p4_memcpy (&wcopy, wl, sizeof(wcopy));

# ifndef WILD_TAB
# define WILD_TAB 26 /* traditional would be 20 (26*4=80), now 26*3=78 */
# endif

    FX (p4_cr);
    FX (p4_start_Q_cr);
    if (categories && *categories == '\0')
        categories = NULL;
    for (t = p4_topmost (&wcopy); *t; t = p4_topmost (&wcopy))
    {
        p4char *w = *t;
        p4char **s = p4_name_to_link (w);
        int l = NAMELEN(w); w = NAMEPTR(w);
        if (p4_match (pattern, w, l, wl->flag & P4_UPPER_CASE_FLAGS))
        {
            char c = p4_category (*P4_TO_CODE(P4_LINK_FROM (s)));
            if (! categories || p4_strchr (categories, c))
            {
                if (p4_OUT+WILD_TAB - p4_OUT%WILD_TAB + 2 + l > p4_COLS ||
                    p4_OUT+WILD_TAB - p4_OUT%WILD_TAB + WILD_TAB*2/3 > p4_COLS)
                {
                    if (p4_Q_cr ())
                        break;
                }else{
                    if (p4_OUT)
                        p4_tab (WILD_TAB);
                }
                p4_outf ("%c %.*s ", c, l, w);
            }
        }
        *t = *s;
    }
}

static p4_namebuf_t *
search_thread_startswith (const p4_char_t *nm, int l, p4_namebuf_t *t, p4_Wordl* wl )
{  /* compare with p4_search_thread */
    auto p4_char_t upper[UPPERMAX];
    p4cell wl_flag = wl->flag;

    if( UPPER_CASE && (wl_flag & WORDL_UPPER_CASE) && l < UPPERMAX)
    {   /* note: p4_match/p4_search_incomplete */
        UPPERCOPY (upper, nm, l);

        /* this thread does contain some upper-case defs
           AND lower-case input shall match those definitions */
        while (t)
        {
            if (! P4_NAMExSMUDGED(t) && NAMELEN(t) >= l)
            {
                if (p4_memequal (nm, NAMEPTR(t), l))  break;
                if (p4_memequal (upper, NAMEPTR(t), l)) break;
            }
            t = *p4_name_to_link (t);
        }
    }else{
        /* input is case-sensitive OR vocabulary contains mixed-case defs */
        while (t)
        {
            if (! P4_NAMExSMUDGED(t) && NAMELEN(t) >= l)
            {
                if (p4_memequal (nm, NAMEPTR(t), l))  break;
            }
            t = *p4_name_to_link (t);
        }
    }
    return t;
}

static p4_namebuf_t *
find_next_incomplete (const p4_char_t *nm, int l, p4_namebuf_t* old)
{   /* compare with p4_find */
    register Wordl **p, **q;
    register Wordl *wordl;
    register p4_namebuf_t* w = NULL;
    register int n;
    register p4ucell searched = 0;

    for (p = CONTEXT; p <= &ONLY; p++)
    {
        for (q = CONTEXT; q < p; q++)
        {
            if (!*q) continue;
            if (*q == *p) goto continue_outer;
        }

        for (wordl = *p; wordl ; wordl=wordl->also)
        {
            /* we _must_ ensure that no world-thread is searched twice unlike
             * p4_find, so just skip any wordl that does not have an id for
             * the search_also case. Generally, we do not miss any word thus.
             */
            if (! wordl->id)
                continue;
            if (searched & wordl->id)
                continue;
            searched |= wordl->id;

            for (n = 0; n < THREADS; n++)
            {
                w = wordl->thread[n];
                while (w)
                {
                    w = search_thread_startswith (nm, l, w, wordl );

                    if (w)
                    {
                        if (! old)              /* search all wordl-threads */
                            return w;           /* until "old" is found     */
                        if (old == w)           /* then return the next nfa */
                            old = 0;            /* found after that one -   */
                        w = *p4_name_to_link(w); /* and if old-arg was null */
                    }                           /* it returns the first w   */
                } /* while (w) */
            }
        }
    continue_outer:
        continue;
    }
    return w; /*0*/
}

/**
 * Try to complete string in/len from dictionary.
 * Store completion in out (asciiz), return number of possible completions.
 * If display is true, display alternatives.
 * (if (display && !len) { don't print 200 words, just the number })
 */
static int
p4_complete_word (const p4_char_t *in, int len, char *out_pocket, int display)
{
    p4_char_t *s = NULL, *t = NULL;
    int n = 0, m = 0, cnt = 0;

    while ((t = find_next_incomplete(in, len, t)))
    {
        cnt ++;
        if (display && len)
        {
            FX (p4_space);
            p4_type_on_line (NAMEPTR(t), NAMELEN(t));
        }
        if (! s)
        {
            s = NAMEPTR(t);
            m = NAMELEN(t);
        }else{
            for (n = 0; n < m; n++)
                if (s[n] != NAMEPTR(t)[n])
                    break;
            m = n;
        }
    }
    if (cnt)
        p4_store_c_string (s, m, out_pocket, POCKET_SIZE);
    if (display && !len)
    { p4_outf (" %i words ", cnt); }
    return cnt;
}


/** used in lined and edit-ext */
int
p4_complete_dictionary (char *in, char *out, int display)
{
    char* buf = p4_pocket();
    int n;
    char* lw;

    lw = strrchr (in, ' ');
    if (lw)
        lw++;
    else
        lw = in;
    p4_memcpy (out, in, lw - in);
    n = p4_complete_word ((p4_char_t*) lw, p4_strlen (lw), buf, display);
    p4_strcpy (&out[lw - in], buf);
    return n;
}

/* ------------------------------------------------------------------- */

/** ONLY ( -- )
 * the only-vocabulary is special. Calling it will erase
 * the search => ORDER of vocabularies and only allows
 * to name some very basic vocabularies. Even => ALSO
 * is not available.
 example:
   ONLY FORTH ALSO EXTENSIONS ALSO DEFINITIONS
 */
void FXCode (p4_only_RT)
{
    /* NO BODY_ADDR */
    p4_memset(CONTEXT, 0, PFE_set.wordlists*sizeof(p4_Wordl*));
    CONTEXT[0] = CURRENT = ONLY;
}
P4RUNTIMES1_RT(p4_only, P4_ONLY_CODE1);

/** FORTH ( -- )
 : FORTH FORTH-WORDLIST CONTEXT ! ;
 */
void FXCode (p4_forth_RT)
{
    /* NO BODY_ADDR */
    CONTEXT[0] = PFE.forth_wl;
}
P4RUNTIMES1_RT(p4_forth, P4_ONLY_CODE1);


static p4_char_t p4_lit_ONLY[] = "ONLY";
static p4_char_t p4_lit_FORTH[] = "FORTH";
static p4_char_t p4_lit_xANSx[] = "[ANS]";
static p4_char_t p4_lit_LOADED[] = "LOADED";
static p4_char_t p4_lit_ENVIRONMENT[] = "ENVIRONMENT";

/** used during boot-phase */
void
p4_preload_only (void)
{
    auto p4_Wordl only;                   /* scratch ONLY word list */

    DICT_HERE = (p4char *) & DICT_BASE[1];

    /* Load the ONLY word list to the scratch ONLY: */
    p4_memset (&only, 0, sizeof only);
    /* # only.flag |= WORDL_NOHASH; */
    p4_header_comma (p4_lit_ONLY, 4, &only ); FX_RUNTIME1_RT(p4_only);
    ONLY = p4_make_wordlist (LAST);
    /* # ONLY->flag |= WORDL_NOHASH; */
    _p4_buf_copy (ONLY->thread, only.thread);
    /* i.e. copy threads from scratch ONLY to real ONLY*/
    CURRENT = ONLY;

    /* FORTH -> [ANS] -> ONLY */
    p4_header_comma (p4_lit_FORTH, 5, ONLY); FX_RUNTIME1_RT(p4_forth);
    PFE.forth_wl = p4_make_wordlist (LAST);
    p4_header_comma (p4_lit_xANSx, 5, ONLY); FX_RUNTIME1(p4_vocabulary);
    FX_IMMEDIATE;
    PFE.forth_wl->also = p4_make_wordlist (LAST);
    PFE.forth_wl->also->also = ONLY;

    /* destroyers :: LOADED */
    p4_header_comma (p4_lit_LOADED, 6, ONLY); FX_RUNTIME1(p4_vocabulary);
    PFE.atexit_wl = p4_make_wordlist (LAST);
    PFE.atexit_wl->flag |= WORDL_NOHASH; /* see environment_dump in core.c */

    /* ENVIRONMENT -> LOADED */
    p4_header_comma (p4_lit_ENVIRONMENT, 11, ONLY); FX_RUNTIME1(p4_vocabulary);
    FX_IMMEDIATE;
    PFE.environ_wl = p4_make_wordlist (LAST);
    PFE.environ_wl->also = PFE.atexit_wl;
    PFE.environ_wl->flag |= WORDL_NOHASH;          /* for option-ext that */
    PFE.environ_wl->thread[0] = PFE.set->opt.dict.last; /* goes here (fixme?)*/
}

/*@}*/
