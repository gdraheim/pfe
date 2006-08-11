/** 
 *  Compile definitions, load-time with load-wordl, runtime with compile-comma
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 33.35 %
 *    (%date_modified: Tue Jul 23 18:18:51 2002 %)
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: dict-comp.c,v 1.3 2006-08-11 21:17:18 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>

#include <pfe/os-string.h>

#include <pfe/def-comp.h>
#include <pfe/option-ext.h>
#include <pfe/environ-ext.h>
#include <pfe/_missing.h>
#include <pfe/logging.h>

#define ___ {
#define ____ }

/* ---------------------------------------------------------------------- *
 * initial dictionary setup                                             
 */

FCode_RT (p4_forget_wordset_RT)
{   FX_USE_BODY_ADDR {
    FX_POP_BODY_ADDR_UNUSED;
    /* do nothing so far, forget_wordset_RT_ is just a type-marker */
}}

extern int p4_slot_use (int*); /* FIXME: move to header file ? */
extern int p4_slot_unuse (int*); /* FIXME: move to header file ? */

static FCode_RT (p4_forget_slot_RT)
{   FX_USE_BODY_ADDR {
    int* slot = (int*)(FX_POP_BODY_ADDR[0]);
    P4_info1 ("unuse load-slot '%i'", *slot);
   
    if (slot && *slot && PFE.p[*slot]) 
    {
        p4_xfree (PFE.p[*slot]); PFE.p[*slot] = 0;
    }
   
    p4_slot_unuse (slot);
}}

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
p4_load_slot_init (int* slot, p4ucelll size)
{
    if (!slot || !*slot || size < 4)
        return;

    if (!(PFE.p)[*slot]) 
    {
        (PFE.p)[*slot] = p4_calloc (1, size);
        P4_info3 ("load-slot %i size %lu alloc (%p)", 
                  *slot, size, (PFE.p)[*slot]);
    }else{ 
        P4_warn2 ("load-slot %i already allocated (%p)", 
                  *slot, (PFE.p)[*slot]);
    }
    
    p4_forget_word ("(load-slot: %i)", *slot, 
                    PFX (p4_forget_slot_RT), (p4cell) slot);
}

static void
p4_load_into (const char* vocname)
{
    if (! vocname) return;

    ___ Wordl* voc = p4_find_wordlist_str (vocname);
    if (! voc) goto search_failed;
    ___ register int i;
    for (i=PFE_set.wordlists; --i > 0; )
    {
	if (CONTEXT[i] == voc) 
	{
	    P4_info1 ("search also '%s' : already there", vocname);
	    return;
	}
    }; ____;
    FX (p4_also);    /* the top-of-order (CONTEXT) isn't changed */
    CONTEXT [1] = voc; /* instead we place it under-the-top */
    P4_info1 ("search also '%s' : done", vocname);
    return; ____;
 search_failed:
    P4_warn2 ("search also failed: no '%s' vocabulary (%lu)", 
                  vocname, (p4celll) p4_strlen(vocname));
} 

static void p4_exception_string (const char* name, p4cell id)
{
    /* FIXME: instead of compiling to the forth-dict we should better
       create a way to let functions search the loaded wordset tables
    */
    p4_Exception* expt = (void*) DP; DP += sizeof(*expt);
    if (id < PFE.next_exception) PFE.next_exception = id - 1;
    expt->next = PFE.exception_link; PFE.exception_link = expt;
    expt->name = name; expt->id = id;
}

static void FXCode(illegal_RT)              
{                                  /* written to cfa following header_comma */
    /* NO BODY_ADDR */             /* to give an error msg when calling */
    p4_throw (P4_ON_INVALID_NAME); /* a word without execution semantics */
}
P4RUNTIMES1_RT(illegal, P4_ONLY_CODE1);

/* ................................................................... */
/* for the new_implementation: */

static inline void* p4_save_input_tib (void* stack)
{
    void* R = p4_save_input (stack);
    SOURCE_ID = -1;
    BLK = 0;
#  if 0
    TIB = tib;
    NUMBER_TIB = len;
    TO_IN = 0;
#  endif
    return R;
}

#define p4_uses_input_tib(P) { PFE.word.ptr = (P); PFE.word.len = -1; }


static FCode (p4_exception_string)
{
    p4cell id = FX_POP;
    /* FIXME: instead of compiling to the forth-dict we should better
       create a way to let functions search the loaded wordset tables
    */
    p4_Exception* expt = (void*) DP; DP += sizeof(*expt);
    if (id < PFE.next_exception) PFE.next_exception = id - 1;
    expt->next = PFE.exception_link; PFE.exception_link = expt;
    /* FIXME: this is wrong: we assume it is called from a wordset-loader */
    expt->name = (char*) PFE.word.ptr; expt->id = id;
}

/* we choose compiler code removal here */
#ifndef PFE_CALL_THREADING
#define _ITC_ 1
#else
#define _ITC_ 0
#endif

#ifndef USE_NEW_LOADER            /* USER-CONFIG: */
#define USE_NEW_LOADER 1          /* new FX implemenation of wordset-loader */
#endif                            /* (--walk-on / --walk-off on commandline) */

static FCode (p4_load_into)
{
    register char* vocname = (void*) FX_POP;

    p4_word_parseword (' '); *DP=0; /* PARSE-WORD-NOHERE */
    ___ register void* p = p4_find_wordlist (PFE.word.ptr, PFE.word.len);
    if (p) 
    {   
	P4_debug1 (13, "load into old '%s'", PFE.word.ptr);
	CURRENT = p;
    }else{
	Wordl* current = 0;
	if (vocname) {
	    current = p4_find_wordlist_str (vocname);
	    if (! current) 
		P4_warn1 ("could not find also-voc %s",  vocname);
	}
	if (! current) current = CURRENT;
	P4_info2 ("load into new '%.*s'", (int) PFE.word.len, PFE.word.ptr);
	p4_header_comma (PFE.word.ptr, PFE.word.len, current);
	P4_info1 ("did comma '%p'", LAST);
	FX_RUNTIME1 (p4_vocabulary);  FX_IMMEDIATE;
	P4_info1 ("done runtime '%p'", LAST);
	CURRENT = p4_make_wordlist (LAST);
	P4_info1 ("load into current '%p'", CURRENT);
    }; ____;
    
    if (vocname) 
    {
	if (! CURRENT->also)
	    CURRENT->also = p4_find_wordlist_str (vocname);

	/* FIXME: it does nest for INTO and ALSO ? */
	p4_load_into (PFE.word.ptr); /* search-also */
    }
} 


static FCode (p4_load_words)
{
    extern void /*forward*/
	p4_load_words (const p4Words* ws, p4_Wordl* wid, int unused);

    void* p = (void*) FX_POP;
    if (p) p4_load_words (p, CURRENT, 0);
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
        p4_strncpy (dictname, ws->name, NFACNTMAX);
        dictname[NFACNTMAX] = '\0';
        if ((dn= p4_strchr (dictname, ' '))
            ||  (dn= p4_strchr (dictname, '(')))
            *dn = '\0';
    }else{
        sprintf (dictname, "%p", DP);
    }
    
    p4_forget_word ("wordset:%s", (p4cell) dictname,
                    PFX (p4_forget_wordset_RT), 
                    (p4cell) (ws));

    ___ static p4_char_t p4_lit_walk[] = "walk";
    if (p4_search_option_value (p4_lit_walk, 4, USE_NEW_LOADER, PFE.set)) 
	goto new_implementation;
    ____;
    
    for ( ; --k >= 0; w++)
    {
	wid = CURRENT;
        if (! w) continue;
	/* the C-name is really type-byte + count-byte away */
        ___  char type = *w->name;
	const p4_char_t* name = (p4_char_t*) w->name+2;
	int name_len = p4_strlen (w->name+2);
	void* ptr = w->ptr;
	Wordl* wid = CURRENT;

	/* and paste over make_word inherited from pre 0.30.28 times */
	p4xt  cfa;
	
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
	    p = p4_find_wordlist (name, name_len);
	    if (p) 
	    {   
		P4_debug1 (13, "load into old '%s'", name);
		CURRENT = p;
	    }else{
		Wordl* current = 0;
		if (ptr) {
		    current = p4_find_wordlist_str (ptr);
		    if (! current) 
			P4_warn1 ("could not find also-voc %s", 
				  (char*)(ptr));
		}
		if (! current) current = CURRENT;
		P4_info1 ("load into new '%s'", name);
		p4_header_comma (name, name_len, current);
		P4_info1 ("did comma '%p'", LAST);
		FX_RUNTIME1 (p4_vocabulary);  FX_IMMEDIATE;
		P4_info1 ("done runtime '%p'", LAST);
		CURRENT = p4_make_wordlist (LAST);
		P4_info1 ("load into current '%p'", CURRENT);
	    }
	    
	    if (ptr) 
	    {
		if (! CURRENT->also)
		    CURRENT->also = p4_find_wordlist_str (ptr);
		
		p4_load_into ((const char*) name); /* search-also */
	    }
	} continue;
	case p4_NEED:
	    if (! p4_environment_Q(name, name_len)) {
		P4_info2 ("'%.*s' not found", name_len, name);
		p4_outs (" .... "); p4_type (name, name_len);
		p4_outs (" not available "); FX (p4_cr);
	    }
	case p4_SLOT:
	    slot = (int*) ptr;
	    p4_load_slot_open (slot);
	    continue;
	case p4_SSIZ:
	    p4_load_slot_init (slot, (p4ucell)(ptr));
	    continue;
	case p4_EXPT:
	    p4_exception_string((const char*) name, (p4cell)(ptr));
	    continue;
	case p4_XXCO: /* constructors are registered in => LOADED */
	    wid = PFE.atexit_wl;
	    break;
	} /*switch*/
	
	/* part 2: general... CREATE a name and setup its CFA field */
	
	p4_header_comma (name, name_len, wid); FX_RUNTIME1_RT (illegal);
	if ('A' <= type && type <= 'Z')
	    FX_IMMEDIATE;
	cfa = P4_BODY_FROM(DP);
#      ifndef PFE_CALL_THREADING
	switch (type)
	{
	case p4_SXCO:
#          ifndef HOST_WIN32
	    *cfa = ((p4_Semant *) ptr) ->comp;
	    if (! ((p4_Semant *)ptr) ->name)
		((p4_Semant *)ptr) ->name = (p4_namebuf_t*)( name-1 ); 
	    /* discard const */
	    /* BEWARE: the arg' name must come from a wordset entry to
	       be both static and have byte in front that could be 
	       a maxlen
	    */
#          else
	    /* on WIN32, the ptr is a function that returns a SemantP */
	    *cfa = ((p4_Semant*(*)())ptr) () -> comp;
	    if (! ((p4_Semant *(*)())ptr) () ->name)
		((p4_Semant *(*)())ptr) () ->name = (p4_namebuf_t*)( name-1 ); 
#          endif
	    continue;
	case p4_RTCO:
#          ifndef HOST_WIN32
	    *cfa = ((p4_Runtime2 *) ptr) ->comp;
	    /* and start registering the runtimes centrally FIXME:
	       FX_COMMA(PFE.runtime); PFE.runtime = p4_HERE;
	       FX_COMMA(ptr);
	       but that sys-link should be honoured in p4_forget too
	    */
#          else
	    /* on WIN32, the ptr is a function that returns a RuntimeP */
	    *cfa = ((p4_Runtime2*(*)())ptr) () -> comp;
#          endif
	    continue;
	case p4_IXCO:
	case p4_FXCO:
	    *cfa = (p4code) ptr;
	    continue;
	case p4_XXCO:
	    *cfa = (p4code) ptr;
	    ((p4code)ptr) ();     /* runs *now* !! no checks !! */
	    continue;
	case p4_IVOC:
	case p4_OVOC:
	    /* creating a VO before IN will make sure that the */
	    /* other words will go in there. Nice stuff, eh ;-) */
	    *cfa = p4_vocabulary_RT_ ;
	    /* (((WList*) ptr)->wid = p4_make_wordlist (nfa)); */
	    continue;
	case p4_DVAR:
	    *cfa = p4_dictvar_RT_ ;
	    break;
	case p4_DCON:
	    *cfa = p4_dictget_RT_ ;
	    break;
	case p4_OVAR:
	case p4_IVAR:
	    *cfa = p4_variable_RT_ ;
	    break;
	case p4_OVAL:
	case p4_IVAL:
	    *cfa = p4_value_RT_ ;
                break;
	case p4_OCON:
	case p4_ICON:
	    *cfa = p4_constant_RT_ ;
	    break;
	case p4_OFFS:
	    *cfa = p4_offset_RT_ ;
	    break;
	case p4_iOLD:
	case p4_xOLD:
	    *cfa = p4_obsoleted_RT_;
	    if (p4_LogMask && p4_LogMask^P4_LOG_FATAL) goto synonym;
	case p4_SNYM:
	case p4_FNYM:
	    *cfa = p4_synonym_RT_ ;
	synonym:
	    ptr = p4_find (ptr, p4_strlen(ptr));
	    if (ptr) ptr = p4_name_from (ptr);
	    else P4_fail3 ("could not resolve SYNONYM %.*s %s",
			   P4_NFA_LEN(LAST), P4_NFA_PTR(LAST), (char*)w->ptr);
	    break;
	default:
	    P4_fail3 ("unknown typecode for loadlist entry: "
		      "0x%x -> \"%.*s\"", 
		      type, name_len, name);
	}
#      else
	/* CALL_THREADING */
	switch (type)
	{
	case p4_XXCO:
#          if 1
	    ((p4code)ptr) ();     /* runs *now* !! no checks !! */
	    /* fallthrough */
#          endif
	case p4_IXCO:
	case p4_FXCO:
	case p4_IVOC:
	case p4_OVOC:
	case p4_DVAR:
	case p4_DCON:
	case p4_OVAR:
	case p4_IVAR:
	case p4_OVAL:
	case p4_IVAL:
	case p4_OCON:
	case p4_ICON:
	case p4_OFFS:
	case p4_SXCO:
	case p4_RTCO:
	case p4_iOLD:
	case p4_xOLD:
	    cfa->word = (p4Word*)w; /* discard "const" here */
	    break;
	case p4_SNYM:
	case p4_FNYM:   
	    ptr = p4_find (ptr, p4_strlen(ptr));
	    if (ptr) ptr = p4_name_from (ptr);
	    else P4_fail3 ("could not resolve SYNONYM %.*s %s",
			   NFACNT(*LAST), LAST+1, (char*)w->ptr);
	    if (ptr) cfa->word = ((p4xt)ptr)->word;
	    continue;
	default:
	    P4_fail3 ("unknown typecode for loadlist entry: "
		      "0x%x -> \"%.*s\"", 
		      type, name_len, name);
	}
#      endif /* not CALL_THREADING */
	FX_VCOMMA (ptr);
	continue;
    } /* for w in ws->w */

    CURRENT = save_current; /* should save_current moved to the caller? */

    return;
    ____;
    /* ............................................................. */
 new_implementation: 
    ___ extern FCode (p4_vocabulary); extern FCode (p4_offset_constant);
    ___ void* saved_input = SP = p4_save_input_tib (SP);
    
    for ( ; --k >= 0; w++)
    {
        if (! w) continue;
	/* the C-name is really type-byte + count-byte away */
	___ char type = *w->name;
	p4_uses_input_tib ((p4_char_t*)(w->name+2));
	
	FX_PUSH (w->ptr);
	
	switch (type)
	{
	case p4_LOAD:
	    FX (p4_load_words); /* RECURSION !! */
	    continue;
	case p4_INTO:
	    FX (p4_load_into);
	    continue;
	case p4_NEED:
	    FX (p4_needs_environment);
	    continue;
	case p4_SLOT:
	    slot = (int*) FX_POP;    /* oops, needs a TH value */
	    p4_load_slot_open (slot);
	    continue;
	case p4_SSIZ:
	    p4_load_slot_init (slot, FX_POP);
	    continue;
	case p4_EXPT:
	    FX (p4_exception_string);
	    continue;
	case p4_SXCO:
#         ifndef HOST_WIN32
	    ___ p4_Semant* semant = (p4_Semant*)(void*)(FX_POP);
#          else  /* on WIN32, the ptr is a function that returns a SemantP */
	    ___ p4_Semant* semant = ((p4_Semant*(*)()) (void*)(FX_POP)) ();
#         endif

	    FX_HEADER;
	    FX_COMMA (( _ITC_ ? semant->comp : (p4code) w));
	    if (! (semant ->name))
		semant ->name = (p4_namebuf_t*)( PFE.word.ptr-1 ); 
	    /* discard const */
	    /* BEWARE: the arg' name must come from a wordset entry to
	       be both static and have a byte in front that could be 
	       a maxlen
	    */
	    break; ____;
	case p4_RTCO:
#          ifndef HOST_WIN32
	    ___ p4_Runtime2* runtime  = ((p4_Runtime2 *) (FX_POP));
	    /* and start registering the runtimes centrally FIXME:
	       FX_COMMA(PFE.runtime); PFE.runtime = p4_HERE;
	       FX_COMMA(ptr);
	       but that sys-link should be honoured in p4_forget too
	    */
#          else
	    /* on WIN32, the ptr is a function that returns a RuntimeP */
	    ___ p4_Runtime2* runtime = ((p4_Runtime2*(*)()) (FX_POP)) ();
#          endif

	    FX_HEADER;
	    FX_COMMA (( _ITC_ ? runtime->comp : (p4code) w ));
	    break; ____;
	case p4_IXCO:         /* these are real primitives which do */
	case p4_FXCO:         /* not reference an info-block but just */
	    FX_HEADER;        /* the p4code directly */
	    FX_COMMA (( _ITC_ ? *SP : (p4cell) w )); 
	    FX_DROP;
	    break;
	case p4_XXCO:
	    FX_HEADER_(PFE.atexit_wl);
	    FX_COMMA (( _ITC_ ? *SP : (p4cell) w )); 
	    ((p4code)(FX_POP)) ();
	    break;
	case p4_STKx:
            if (PFE.stackhelp_wl)
            {
                p4cell old = REDEFINED_MSG;   REDEFINED_MSG = P4_FALSE;
                FX_HEADER_(PFE.stackhelp_wl); REDEFINED_MSG = old;
                FX_COMMA (( _ITC_ ? *SP : (p4cell) w )); /* a.k.a. FXCO */
                FX_DROP;
            } break;
        case p4_STKi:
            if (PFE.stackhelp_wl)
            {
                p4_char_t*        help_str = (p4char*) FX_POP;
                p4cell            help_len = p4_strlen((char*) help_str);
                p4_char_t const * word_str = PFE.word.ptr;
                p4ucell           word_len = PFE.word.len;
                p4cell old = REDEFINED_MSG;   REDEFINED_MSG = P4_FALSE;
                if (word_len > 127) /* oops */
		    word_len = p4_strlen((char*) word_str);
                ___ p4char* nfa = p4_find (word_str, word_len);
                FX_HEADER_(PFE.stackhelp_wl); REDEFINED_MSG = old;
                FX_RUNTIME1(p4_two_constant);
                FX_COMMA_ (help_len,  'V');
                FX_COMMA_ (help_str, 'p');
                FX_COMMA_ (nfa ? p4_name_from(nfa) : 0, 'x'); ____;
            } break;
	case p4_IVOC:
	case p4_OVOC:
	    FX (p4_vocabulary);
	    FX_DROP;
	    break;
	case p4_DVAR:
	    FX_RUNTIME_HEADER;
	    FX_RUNTIME1_RT (p4_dictvar);
	    FX_COMMA (FX_POP);
	    break;
	case p4_DCON:
	    FX_RUNTIME_HEADER;
	    FX_RUNTIME1_RT (p4_dictget);
	    FX_COMMA (FX_POP);
	    break;
	case p4_OVAR:
	case p4_IVAR:
	    FX (p4_variable);
	    break;
	case p4_OVAL:
	case p4_IVAL:
	    FX (p4_value);
	    break;
	case p4_OCON:
	case p4_ICON:
	    FX (p4_constant);
	    break;
	case p4_OFFS:
	    FX (p4_offset_constant);
	    break;
	case p4_iOLD:
	case p4_xOLD:
	    FX_RUNTIME_HEADER;
	    FX_RUNTIME1_RT (p4_obsoleted);
	    if (p4_LogMask && p4_LogMask^P4_LOG_FATAL) goto synonyms;
	case p4_SNYM:
	case p4_FNYM:
	    FX_RUNTIME_HEADER;
	    FX_RUNTIME1_RT (p4_synonym);
	synonyms:
	    ___ void* use = (char*) FX_POP;
	    use = p4_find (use, p4_strlen(use));
	    if (use) use = p4_name_from (use);
	    else P4_fail3 ("could not resolve SYNONYM %.*s %s",
			   NFACNT(*LAST), LAST+1, (char*)w->ptr);
	    FX_COMMA (use);
	    break; ____;
	default:
	    P4_fail2 ("unknown typecode for loadlist entry: "
		      "0x%x -> \"%s\"", 
		      type, PFE.word.ptr);
	} /*switch*/
	
	/* implicit IMMEDIATE still around: */
	if ('A' <= type && type <= 'Z')
	    FX_IMMEDIATE;
	____;
    } /* for w in ws->w */

    CURRENT = save_current; /* should save_current moved to the caller? */
    SP = p4_restore_input (saved_input);
    ____;____;
}

/* ------------------------------------------------------------------- */

#ifdef _export
extern p4xcode* p4_compile_comma (p4xcode* at, p4xt);
extern p4xcode* p4_compile_xcode (p4xcode* at, p4xcode);
extern p4xcode* p4_compile_xcode_CODE (p4xcode* at, p4xcode);
extern p4xcode* p4_compile_xcode_BODY (p4xcode* at, p4xcode, p4cell*);
#endif

# ifdef PFE_CALL_THREADING
typedef struct { const char* name; const p4xcode xcode; } const loader_t;
static loader_t * loader (p4char c)
{
    static loader_t trampoline = { "trampoline", 0 };
    static loader_t primitive = { "primitive", 0 };
    static loader_t compiling = { "compiling-prim", 0 };
    static loader_t creating =  { "creating-prim", 0 };
    static loader_t createdW = { "created-word", 0 };
    static loader_t vocabulary = { "vocabulary", PFX (p4_vocabulary_RT) };
    static loader_t dictvar = { "system-variable", PFX(p4_dictvar_RT) };
    static loader_t dictget = { "system-constant", PFX(p4_dictget_RT) };
    static loader_t variable = { "variable", PFX(p4_variable_RT) };
    static loader_t valuevar = { "valuevar", PFX(p4_value_RT) };
    static loader_t constant = { "constant", PFX(p4_constant_RT) };
    static loader_t offsetW = { "offset-word", PFX(p4_offset_RT) };
    static loader_t obsoleted = { "obsolete-word", PFX(p4_obsoleted_RT) };
    static loader_t unknown = { "unknown-typecode", 0 };

    switch (c)
    {
    case 0:	  return & trampoline;
    case p4_FXCO:
    case p4_IXCO: 
    case p4_XXCO: return & primitive;
    case p4_SXCO: return & compiling;
    case p4_RTCO: return & creating;
    case p4_ITEM: return & createdW;
    case p4_IVOC: 
    case p4_OVOC: return & vocabulary;
    case p4_DVAR: return & dictvar;
    case p4_DCON: return & dictget;
    case p4_OVAR: 
    case p4_IVAR: return & variable;
    case p4_OVAL: 
    case p4_IVAL: return & valuevar;
    case p4_OCON: 
    case p4_ICON: return & constant;
    case p4_OFFS: return & offsetW;
    case p4_iOLD: 
    case p4_xOLD: return & obsoleted ;
    default:	  return & unknown;
    }
}

p4xcode* p4_compile_xcode(p4xcode* at, p4code code)
{
    FX_COMPILE1_CALL (at, code);
    return at;
}

p4xcode* p4_compile_xcode_BODY(p4xcode* at, p4code code, p4cell* body)
{
    FX_ARG_BODY_ADDR (at, body);
    FX_COMPILE1_CALL (at, code);
    FX_PUT_BODY_ADDR (at, body);
    return at;
}

p4xcode* p4_compile_xcode_CODE(p4xcode* at, p4code code)
{
    FX_ARG_CODE_ADDR (at);
    FX_COMPILE1_CALL (at, code);
    FX_PUT_CODE_ADDR (at);
    return at;
}

/* the const here will hint where possibly sth. woudl write to code-mem */
const p4xcode* p4_to_code(p4xt xt)
{
    static p4xcode vocabulary = PFX(p4_vocabulary_RT);
    static p4xcode dictvar =    PFX(p4_dictvar_RT);
    static p4xcode dictget =    PFX(p4_dictget_RT);
    static p4xcode variable =   PFX(p4_variable_RT);
    static p4xcode value =      PFX(p4_value_RT);
    static p4xcode constant =   PFX(p4_constant_RT);
    static p4xcode offset =     PFX(p4_offset_RT);
    static p4xcode obsoleted =  PFX(p4_obsoleted_RT);

    switch (*xt->type->def)
    {
    case 0: /* the "" string indicates a trampoline */
    case p4_DTOR:
    case p4_FXCO:
    case p4_IXCO:
    case p4_XXCO:	return & xt->word->ptr;
    case p4_SXCO:	return & ((p4_Semant*)xt->word->ptr)->comp;
    case p4_RTCO:	return & ((p4_Runtime2*)xt->word->ptr)->comp;
    case p4_ITEM:	return & xt->call->exec[0];
    case p4_IVOC:
    case p4_OVOC:	return & vocabulary;
    case p4_DVAR:	return & dictvar ;
    case p4_DCON:	return & dictget ;
    case p4_OVAR:
    case p4_IVAR:	return & variable ;
    case p4_OVAL:
    case p4_IVAL:	return & value ;
    case p4_OCON:
    case p4_ICON:	return & constant ;
    case p4_OFFS:	return & offset ;
    case p4_iOLD:
    case p4_xOLD:	return & obsoleted;
    default:
	P4_fail2 ("<!unknown execution code!(%c:%s)>", 
                  *xt->type->def, loader(*xt->type->def)->name);
	/* not yet supported */
	return 0;
    }
    /* unreachable */
}



/* simplest form of compilation */
p4xcode* p4_compile_comma(p4xcode* at, p4xt xt)
{
    switch (*xt->type->def)
    {
    case 0: /* the "" string indicates a trampoline */
    case p4_FXCO:
    case p4_IXCO:
    case p4_XXCO:
	return p4_compile_xcode (at,xt->word->ptr);
    case p4_SXCO:
	return p4_compile_xcode (at,((p4_Semant*)xt->word->ptr)->comp);
    case p4_RTCO:
	return p4_compile_xcode (at,((p4_Runtime2*)xt->word->ptr)->comp);
    case p4_ITEM:
	if (! xt->call->flag & P4_ONLY_CODE1) 
	    return p4_compile_xcode_BODY (at,xt->call->exec[0],P4_TO_BODY(xt));
	else
	    return p4_compile_xcode (at, xt->call->exec[0]);
    case p4_DTOR: /* a destroyer-trampoline */
	return p4_compile_xcode_BODY (at, xt->word->ptr, P4_TO_BODY(xt));
    case p4_NEST: /* a CODE trampoline */
	return p4_compile_xcode (at, (p4xcode)(xt+1));
    case p4_IVOC:
    case p4_OVOC:	
    case p4_DVAR:      
    case p4_DCON:               /* all these are not primitives */
    case p4_OVAR:               /* their runtimes will fetch the */
    case p4_IVAR:               /* body-ptr being compiled here */
    case p4_OVAL:
    case p4_IVAL:  
    case p4_OCON:
    case p4_ICON:
    case p4_OFFS: 
	/* P4_fail5 ("<!word type=%c:%s xt=%p code=%p body=%p!>",
	 *           *xt->type->def, loader(*xt->type->def)->name, xt, 
	 *           *p4_to_code(xt), P4_TO_BODY(xt));
	 */
	return p4_compile_xcode_BODY (at, *p4_to_code(xt), P4_TO_BODY(xt));
    default:
	P4_fail2 ("<!unknown compile code!(%c:%s)>", 
                  *xt->type->def, loader(*xt->type->def)->name);
	/* not yet supported */
	return at;
    }
    /* unreachable */
}

# if defined PFE_SBR_CALL_THREADING
/* ... and here are the SBR snippets needed to call an XT from C ... */

# if defined PFE_SBR_CALL_ARG_THREADING
/* defeat the compiler which wishes to optimize arg away for being unused */
#  if defined PFE_HOST_ARCH_I386
/*  the i386 architecture is so heavily register-starved that it does
 *  quite always setup a local frame which however breaks the ret-jmp
 *  asm-code presented. So what, we make another subroutine for which
 *  hopefully the compiler will not try to build an extra locals frame */
#   define __call(X,Y) { \
     register void* _v P4_SBR_TAKE_BODY; \
     _v = (X);        asm volatile ("push %0":: "r" (_v)); \
     _v = P4_TO_BODY (Y); asm volatile ("ret":: "r" (_v)); }
#   define _call(X,Y) p4_sbr_call_arg((X),(Y),(Y)) 
    void p4_sbr_call_arg(void* code, void* xt1, void*xt2) { __call(code,xt1); }
#  elif defined PFE_HOST_ARCH_M68K
#   define _call(X,Y) { \
     register p4xcode _x asm ("%a0") = (X); \
     register void* _y asm ("%a1") = P4_TO_BODY (Y); \
     asm volatile ("jsr %0@":: "r" (_x), "r" (_y)); }
#  elif defined PFE_HOST_ARCH_POWERPC
#   define _call(X,Y) { \
     register p4xcode _x asm ("0") = (X); \
     register p4xcode _y P4_SBR_TAKE_BODY = P4_TO_BODY (Y); \
     asm volatile ("mtlr %0" :: "r" (_x)); \
     asm volatile ("blrl" :: "r" (_y)); }

#  else
#   error need to define asm p4_sbr_call for this architecture
#  endif /* PFE_HOST_* */
# endif /* PFE_SBR_CALL_ARG_THREADING */

#if 0
static void _enter() { p4_outf(" {<%p:%p>", p4RP,p4RP[-2]); }
static void _leave() { p4_outf("<%p:%p>}", p4RP,p4RP[-2]); }
void p4_sbr_call (p4xt xt) {
    _enter();
    _p4_sbr_call(xt);
    _leave();
}
#define p4_sbr_call _p4_sbr_call
#else
#define _enter()
#define _leave()
#endif

_export void p4_sbr_call (p4xt xt)
{
# if defined PFE_HOST_ARCH_I386 /* && ! SBR_CALL_ARG */
#   if defined PFE_ASM_USE_EBP || !defined PFE_USE_REGS \
    || defined PFE_HOST_ARCH_I386_64
#   define PFE_i386_backup_ebp
#   define PFE_i386_restore_ebp
#   else
#   define PFE_i386_backup_ebp     asm volatile ("movl %%ebp, %0" : "=r" (ebp))
#   define PFE_i386_restore_ebp    asm volatile ("movl %0, %%ebp" :: "r" (ebp))
    void* ebp;
#   endif

    /* the modern RISC architectures do not quite like it when some memory
     * area is modified and executed right away. It emerges to be a variant
     * of the problems about self-modifiying code. Even a jump via register
     * did not help it. So far, only the i386 processors can stand the
     * following simple code that is based on the compile_comma code
     */

#  if 1
    /* sbr-stub, xt-code, xt-data, sbr-exit */
    p4xcode list[6] /* = { 0,0,0,0,0,0 } */;
#  else
    p4xcode* list = (void*) p4_pocket();
#  endif
    void** p = p4_compile_comma(list, xt);
    PFE_SBR_COMPILE_EXIT(p);    _enter();  PFE_i386_backup_ebp;
    ((p4code) (list))();        PFE_i386_restore_ebp;  _leave();
    return;

    /* note however, that quite some i386-type processors do not honour
     * such conditions lightly - they might not just only flush the
     * the instruction pipeline, they might even flush the L1 cache when
     * there are different L1 caches for data and code. You don't want that.
     */
# elif defined PFE_SBR_CALL_ARG_THREADING
    /* and here, we have to recreate the variants of our compile_xcode
     * routines, in this case however, we just want to have the register effect
     * immediatly that the compiled code would have, and jump to the target
     * xcode - it is almost easy with sbr-arg threading since we just need
     * to setup the arg-register correctly, and then call the actual routine.
     */

    switch (*xt->type->def)
    {
    case 0: /* the "" string indicates a trampoline */
    case p4_FXCO:
    case p4_IXCO:
    case p4_XXCO:
        xt->word->ptr (); return; 
        /* p4_compile_xcode (at,xt->word->ptr); */
    case p4_SXCO:
        ((p4_Semant*)xt->word->ptr)->comp (); return;
	/* p4_compile_xcode (at,((p4_Semant*)xt->word->ptr)->comp); */
    case p4_RTCO:
        ((p4_Runtime2*)xt->word->ptr)->comp (); return;
	/* p4_compile_xcode (at,((p4_Runtime2*)xt->word->ptr)->comp);*/
    case p4_ITEM:
        _call (xt->call->exec[0], xt); return;
	/* if (! xt->call->flag & P4_ONLY_CODE1) 
         *  return p4_compile_xcode_BODY (at,xt->call->exec[0],P4_TO_BODY(xt));
         * else
         *  return p4_compile_xcode (at, xt->call->exec[0]);
         */
    case p4_DTOR: /* a destroyer-trampoline */
        _call (xt->word->ptr, xt); return;
	/* p4_compile_xcode_BODY (at, xt->word->ptr, P4_TO_BODY(xt)); */
    case p4_NEST: /* a CODE trampoline */
        ((p4xcode)(xt+1)) (); return;
	/* p4_compile_xcode (at, (p4xcode)(xt+1)); */
    case p4_IVOC:
    case p4_OVOC:	
    case p4_DVAR:      
    case p4_DCON:               /* all these are not primitives */
    case p4_OVAR:               /* their runtimes will fetch the */
    case p4_IVAR:               /* body-ptr being compiled here */
    case p4_OVAL:
    case p4_IVAL:  
    case p4_OCON:
    case p4_ICON:
    case p4_OFFS: 
	/* P4_note5 ("<!word type=%c:%s xt=%p code=%p body=%p!>",
	 *           *xt->type->def, loader(*xt->type->def)->name, xt, 
	 *           *p4_to_code(xt), P4_TO_BODY(xt));
	 */
        
        _call (*p4_to_code(xt), xt); return; 
	/* p4_compile_xcode_BODY (at, *p4_to_code(xt), P4_TO_BODY(xt)); */
    default:
	P4_fail2 ("<!unknown execute code!(%c:%s)>", 
                  *xt->type->def, loader(*xt->type->def)->name);
	/* not yet supported */
	return;
    }
    /* unreachable */
# else /* other HOST_* && ! SBR_CALL_ARG */
    /* for the case of sbr-call no-arg, we will need to do it quite
     * differently. Here we have to lie about the return-code that is
     * on the stack and which will be used to return later on. Instead
     * of setting an arg-register, we set a memory cell and take the
     * address of it to be pushed on the return-stack as the return
     * address for the items with an call-body, the other items can
     * just be called as is. Then we JUMP into the routine instead of
     * CALL to the routine, which will execute that routine and let
     * it return via the ret-code that follows the data area. However,
     * this is not tested - I'm not sure if it works on superpipelined
     * RISC machines although I guess it should. For the work at the
     * Tek labs, the no-arg sbr-threaded mode is not used anyway. Feel
     * free to add it. Otherwise this area will be left incomplete
     * as its support is only academic - for commercial grade developments
     * all the cpu docs are at hand, so it should be always possible to
     * define the bits to use sbr-call-arg threading.
     */
    P4_fail2 ("<!sbr-call no-arg is not supported on this platform,"
              " can not handle execute code!(%c:%s)>", 
              *xt->type->def, loader(*xt->type->def)->name);
    return;
# endif
}

/* _SBR_CALL_THREADING */
# endif
/* _CALL_THREADING */
#endif

/*@}*/

/*
 * Local variables:
 * c-file-style: "stroustrup"
 * End:
 */
