/** 
 * --  Subroutines for the Internal Forth-System
 * 
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2006 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.6 $
 *     (modified $Date: 2006-09-26 21:11:50 $)
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: engine-sub.c,v 1.6 2006-09-26 21:11:50 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-limits.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <pfe/os-string.h>
#include <pfe/os-setjmp.h>

#ifdef _K12_SOURCE
#include <pfe/main-k12.h>
#endif

#include <pfe/option-ext.h>
#include <pfe/double-sub.h>
#include <pfe/debug-ext.h>
#include <pfe/block-sub.h>
#include <pfe/file-sub.h>
#include <pfe/term-sub.h>
#include <pfe/_missing.h>
#include <pfe/exception-sub.h>
#include <pfe/chainlist-ext.h>

#include <pfe/logging.h>

FCode(p4_noop)
{
    /* well, nothing... */
}


/* ********************************************************************** 
 * inner and outer interpreter						  
 */

#ifndef PFE_SBR_CALL_THREADING

/**
 * longjmp via (Except->jmp) following inline
 * - purpose: stop the inner interpreter
 */
FCode_XE (p4_call_stop)	
{   FX_USE_CODE_ADDR {
    p4_Except *buf = (p4_Except *) *IP;

# ifdef P4_REGRP		/* save global register variables */
    buf->rpp = RP;
# endif
# ifdef P4_REGSP
    buf->spp = SP;
# endif
# ifdef P4_REGLP
    buf->lpp = LP;
# endif
# ifndef P4_NO_FP
# ifdef P4_REGFP
    buf->fpp = FP;
# endif
# endif
    p4_longjmp (buf->jmp, 1);
    /*FX_USE_CODE_EXIT;*/
}}

#endif /* ! SBR_THREADING */

/**
 * Run a forth word from within C-code
 * - this is the inner interpreter
 */
_export void
p4_call_loop (p4xt xt)
{
#if defined PFE_SBR_CALL_THREADING
    p4_sbr_call (xt);
    return;
#else

    p4_Except stop;

#  if ! defined PFE_CALL_THREADING
    static p4code call_stop = PFX (p4_call_stop);
    p4xcode list[3];
    list[0] = xt;
    list[1] = &call_stop;
    list[2] = (p4xcode) &stop;
#  else
    /* sbr-stub, xt-code, xt-data, sbr-stub, jump-code, jump-data */
    p4xcode list[6] /* = { 0,0,0,0,0,0 } */ ;
    *p4_compile_xcode(
	p4_compile_comma(
	    list,
	    xt),
	PFX(p4_call_stop)) = (p4xcode) &stop;
#  endif

    IP = list;
#  if !defined P4_WP_VIA_IP && !defined PFE_CALL_THREADING
    p4WP = *IP;
#  endif

    if (p4_setjmp (stop.jmp))
    {
#     ifdef P4_REGRP		/* restore global register variables */
        RP = stop.rpp;		/* clobbered by longjmp() */
#     endif
#     ifdef P4_REGSP
        SP = stop.spp;
#     endif
#     ifdef P4_REGLP
        LP = stop.lpp;
#     endif
#     ifndef P4_NO_FP
#     ifdef P4_REGFP
        FP = stop.fpp;
#     endif
#     endif
        return;
    }

    /* next_loop */
    for (;;)
    {
#    ifdef PFE_CALL_THREADING
#      define NVAR register p4code c;
#      define NEXT c = *IP++, (c)()
#    elif defined P4_WP_VIA_IP
#      define NVAR register p4xt w;
#      define NEXT w = *IP++, (*w) ()	/* ip is register but p4WP isn't */
#    else
#      define NVAR
#      define NEXT p4WP = *IP++, (*p4WP) ()	
				/* ip and p4WP are same: register or not */
#    endif

        NVAR;
#     ifdef UNROLL_NEXT		/* USER-CONFIG: if it helps */
        NEXT; NEXT; NEXT; NEXT;	/* do a little loop unrolling for speed */
        NEXT; NEXT; NEXT; NEXT;
#     else
        NEXT;			/* on some machines it doesn't do any good */
#     endif
    }

#endif /* ! PFE_SBR_CALL_THREADING */
}

/**
 */
_export void
p4_call (p4xt xt)
{
# if 0 && defined __target_os_sunos
    void *saved_ip;

    saved_ip = IP;
    printf ("%X/%X\n", IP, saved_ip);
    p4_call_loop (xt);
    printf ("%X/%X\n", IP, saved_ip);
    IP = saved_ip;
    printf ("%X/%X\n\n", IP, saved_ip);
# elif !defined PFE_SBR_CALL_THREADING
    p4xcode *saved_ip = IP;
    p4_call_loop (xt);
    IP = saved_ip;
# else
    p4_sbr_call (xt);
# endif
}

/**
 * the NEXT call. Can be replaced by p4_debug_execute to
 * trace the inner forth interpreter.
 */
_export void
p4_normal_execute (p4xt xt)
{
#  if defined PFE_SBR_CALL_THREADING /*FIXME: BODY / CODE ADDR needed? */
    (*p4_to_code(xt))();
#  else
    p4_call(xt);
#  endif
}

/**
 * quick execute - unsafe and slow and simple
 *
 * use this routine for callbacks that might go through some forth
 * colon-routines - that code shall not THROW or do some other nifty
 * tricks with the return-stack or the inner interpreter. 
 * Just simple things - use only for primitives or colon-routines,
 * nothing curried with a DOES part in SBR-threading or sth. like that.
 */
_export void
p4_simple_execute (p4xt xt)
{
#  if defined PFE_SBR_CALL_THREADING /*FIXME: BODY / CODE ADDR needed? */
    (*p4_to_code(xt))();
#  elif defined PFE_CALL_THREADING 
    P4_REGIP_T ip = IP;
    P4_REGRP_T rp = RP; 
    p4xcode body = (p4xcode) P4_TO_BODY(xt);
    IP = &body; /* fake the body-field, just in case it is needed */
    (*p4_to_code(xt))();
    while (RP < rp) { NVAR; NEXT; }
    IP = ip;
#  else /* ITC: */
    P4_REGIP_T ip = IP;
    P4_REGRP_T rp = RP; 
    IP = & xt;
    do { NVAR; NEXT; }  while (RP < rp);
    IP = ip;
#  endif
}

/* ================= INTERPRET =================== */

static p4ucell
FXCode (p4_interpret_find_word) /* hereclean */
{
    register p4char *nfa;
    register p4xt xt;

    /* WORD-string is at HERE and at PFE.word.ptr / PFE.word.len */
    nfa = p4_find (PFE.word.ptr, PFE.word.len);
    if (! nfa) return (p4cell) nfa; /* quick path, even alias null-return */

    xt = p4_name_from (nfa);
    if (! STATE || P4_NFA_xIMMEDIATE(nfa))
    {
	p4_call (xt);           /* execute it now */
	FX (p4_Q_stack);        /* check stack */
    }else{
	FX_COMPILE_COMMA (xt);  /* comma token */
    }
    return 1;
}
static FCode (p4_interpret_find_execution)
{
    FX_USE_CODE_ADDR;
    if (FX (p4_interpret_find_word)) FX_BRANCH; else FX_SKIP_BRANCH;
    FX_USE_CODE_EXIT;
}
FCode (p4_interpret_find)
{
    p4_Q_pairs (P4_DEST_MAGIC); /* BEGIN ... AGAIN */
    FX_COMPILE (p4_interpret_find);
    FX (p4_dup);
    FX (p4_backward_resolve);
    FX_PUSH (P4_DEST_MAGIC);
}
P4COMPILES (p4_interpret_find, p4_interpret_find_execution,
  P4_SKIPS_OFFSET, P4_NEW1_STYLE);
/** INTERPRET-FIND ( CS: dest* -- dest* ) executes ( -- ) experimental
 *  check the next word from => QUERY and try to look it up
 *  with => FIND - if found then execute the token right away
 *  and branch out of the loop body (usually do it => AGAIN )
 */

static p4ucell
FXCode (p4_interpret_number_word) /* hereclean */
{
    p4dcell d;

    /* WORD-string is at HERE and at PFE.word.ptr / PFE.word.len */
    if (! p4_number_question (PFE.word.ptr, PFE.word.len, &d))
	return 0; /* quick path */

    if (STATE)
    {
	if (p4_DPL >= 0) 
	{   
	    FX_COMPILE (p4_two_literal);
	    FX_COMMA_ (d.hi,'D');
            FX_COMMA_ (d.lo,'d');
	}else{
	    FX_COMPILE (p4_literal);
            FX_SCOMMA (d.lo);
	}
    }else{
	*--SP = d.lo;
	if (p4_DPL >= 0) 
	    *--SP = d.hi;
    }
    return 1;
}
static FCode (p4_interpret_number_execution)
{
    FX_USE_CODE_ADDR;
    if (FX (p4_interpret_find_word)) FX_BRANCH; else FX_SKIP_BRANCH;
    FX_USE_CODE_EXIT;
}
FCode (p4_interpret_number)
{
    p4_Q_pairs (P4_DEST_MAGIC); /* BEGIN ... AGAIN */
    FX_COMPILE (p4_interpret_number);
    FX (p4_dup);
    FX (p4_backward_resolve);
    FX_PUSH (P4_DEST_MAGIC);
}
P4COMPILES (p4_interpret_number, p4_interpret_number_execution,
  P4_SKIPS_OFFSET, P4_NEW1_STYLE);
/** INTERPRET-NUMBER ( CS: dest* -- dest* ) executes ( -- ) experimental
 *  check the next word from => QUERY and try to parse it up
 *  with => ?NUMBER - if parseable then postpone the number for execution
 *  and branch out of the loop body (usually do it => AGAIN )
 */

static FCode (p4_interpret_loop);
static unsigned FXCode (p4_interpret_next_word);

/**
 * the => INTERPRET as called by the outer interpreter
 */
FCode (p4_interpret)
{
    /* HACK: until proper initialization bindings, we do init'
     * the interpret-vectors right in here. This *will* go away.
     */
    if (! PFE.interpret[3])
    {
	/* PFE.interpret[6] = PFX (p4_interpret_dstrings); */
	/* PFE.interpret[5] = PFX (p4_interpret_locals); */
	PFE.interpret[4] = PFX (p4_interpret_find_word);
	PFE.interpret[3] = PFX (p4_interpret_number_word);
	/* PFE.interpret[2] = PFX (p4_interpret_float); */
	/* PFE.interpret[1] = PFX (p4_interpret_smart); */
    }

    PFE.last_here = PFE.dp;
    if (PFE.interpret_compiled)
	p4_simple_execute (PFE.interpret_loop);
    else
	FX (p4_interpret_loop);
}

static FCode (p4_interpret_loop)
{
    register int i;
    for (;;)
    {
    again:
	if (! FX (p4_interpret_next_word)) return;
	i = DIM (PFE.interpret);
	while ( i-- )
	{
	    if (! PFE.interpret[i]) continue;
	    if (PFE.interpret[i] (FX_VOID)) goto again;
	}
        p4_throw (P4_ON_UNDEFINED);
    }
}

static unsigned FXCode (p4_interpret_next_word)
{
    for (;;)
    {
	/* the parsed string is in PFE.word.ptr / PFE.word.len,
	 * and by setting the HERE-string to length null, THROW
	 * will not try to report it but instead it prints PFE.word.
	 */
	p4_word_parseword (' '); *DP = 0; /* PARSE-WORD-NOHERE */
	if (PFE.word.len) return PFE.word.len;
            
	switch (SOURCE_ID)
	{
	default:
	    if (p4_next_line ())
	    {
		PFE.last_here = PFE.dp;
		continue;
	    }
	case 0:
	case -1:
	    return 0;
	}
    }  
}

/**
 * => INTERPRET buffer
 */
_export void
p4_evaluate (const p4_char_t *p, int n)
{
#  if !defined P4_RP_IN_VM
    Iframe saved;
    p4_link_saved_input (&saved);
#  else
    RP = (p4xcode **) p4_save_input (RP);
#  endif
    SOURCE_ID = -1;
    BLK = 0;
    TIB = p;                /* leave that warning for a while... */
    NUMBER_TIB = n;
    TO_IN = 0;
    FX (p4_interpret);
#  if defined P4_RP_IN_VM
    RP = (p4xcode **) p4_restore_input (RP);
#  else
    p4_unlink_saved_input (&saved);
#  endif
}

/**
 */
_export void
p4_include_file (p4_File *fid)
{
    if (fid == NULL || fid->f == NULL)
        p4_throwstr (P4_ON_FILE_NEX, fid->name);
    else
    {
#      if !defined P4_RP_IN_VM
	Iframe saved;
	p4_link_saved_input (&saved);
#      else
	RP = (p4xcode **) p4_save_input (RP);
#      endif
	SOURCE_ID = (p4cell) fid;
	BLK = 0;
	TO_IN = 0;
	FX (p4_interpret);
#      if defined P4_RP_IN_VM
	RP = (p4xcode **) p4_restore_input (RP);
#      else
	p4_unlink_saved_input (&saved);
#      endif
    }
}

/**
 * called by INCLUDED and INCLUDE
 */
_export int
p4_included1 (const p4_char_t *name, int len, int throws)
{
    char* fn = p4_pocket_expanded_filename (
        name, len, *P4_opt.inc_paths, *P4_opt.inc_ext);
    File* f = p4_open_file ((p4_char_t*) fn, p4_strlen (fn), FMODE_RO);
    if (!f)
    {  
        if (throws) 
        {
            p4_throwstr (P4_ON_FILE_NEX, fn); 
        }else{ 
            P4_fail2 ("- could not open '%s' (paths='%s')\n", 
                      fn, *P4_opt.inc_paths); 
            return 0; 
        }
    }
#   ifdef _K12_SOURCE
    {
        register struct k12_priv* k12p = P4_K12_PRIV(p4TH);
        k12p->state = K12_EMU_NOT_LOADED;
        /* before GetEvent, it goes _IDLE in term-k12.c FIXME: generalize!!*/
    }
#   endif

    p4_include_file (f);
    p4_close_file (f);
    return 1;
}

/**
 * INCLUDED
 */
_export void
p4_included (const p4_char_t* name, int len)
{
    p4_included1 (name, len, 1);
}

/*
 */
_export void
p4_unnest_input (p4_Iframe *p)
{
    while (PFE.saved_input && PFE.saved_input != p)
    {
        switch (SOURCE_ID)
	{
        case -1:
        case 0:
            break;
        default:
            p4_close_file (SOURCE_FILE);
	}
#     if defined P4_RP_IN_VM
        RP = (p4xcode **) p4_restore_input (PFE.saved_input);
#     else
	p4_unlink_saved_input (PFE.saved_input);
#     endif
    }
}

/**
 * walk the filedescriptors and close/free the fds. This function
 * is usefully called from => ABORT - otherwise it may rip too
 * many files in use.
 */
FCode (p4_closeall_files) 
{
    /*FIXME: look at p4_close_all_files, is it the same?? */
    File* f;

    /* see => p4_free_file_slot for an example */
    for (f = PFE.files; f < PFE.files_top; f++) 
        if (f->f != NULL)
        {
            if (f->name && f->name[0] == '<')
                continue; /* stdIn, stdOut, stdErr, a.k.a. "<STDIN>" etc. */
            else
                p4_close_file(f);
        }
}

/* ********************************************************************** 
 *  QUIT, ABORT, INTERPRET
 */

/**
 * a little helper that just emits "ok", called in outer interpreter,
 * also useful on the command line to copy lines for re-execution
 */
FCode (p4_ok)
{
    if (!STATE)
    {
        p4_outs ("ok");
        if (PFE.nr) {
            p4_outc ('-');
            p4_outc ('0' + PFE.nr % 10); 
        }
        FX (p4_space);
    }
}

/*
 * things => QUIT has to initialize 
 */
static void
quit_system (P4_VOID)
{
#  ifdef P4_RP_IN_VM
    CSP = (p4cell*) RP;         /* come_back marker */
    RP = p4_R0;			/* return stack to its bottom */
#  endif
    LP = NULL;			/* including all local variables */
    STATE = P4_FALSE;		/* interpreting now */
    PFE.catchframe = NULL;	/* and no exceptions to be caught */
    p4_debug_off ();		/* turn off debugger */
}

/*
 * things => ABORT has to initialize
 */
static void
abort_system (P4_VOID)
{
    SP = p4_S0;				/* stacks */
    if (PFE.abort[2]) { (PFE.abort[2]) (FX_VOID); } /* -> floating */
    if (PFE.abort[3]) { (PFE.abort[3]) (FX_VOID); } /* -> dstrings */
    if (p4_RESET_ORDER)  { FX (p4_reset_order); }   /* reset search order */
    FX (p4_decimal);			/* number i/o base */
    FX (p4_standard_io);		/* disable i/o redirection */
    FX (p4_closeall_files);             /* close open filedescriptors */
    if (PFE.dictlimit - PFE_MINIMAL_UNUSED > PFE.dp)
        return;
    else
    {
        P4_fail2 ("DICT OVER - reset HERE from %+li to %+li",
                  (long)(PFE.dp - PFE.dict), 
		  (long)(PFE.last_here - PFE.dict));

        PFE.dp = PFE.last_here;
    }
}

FCode (p4_paren_abort)
{
    abort_system (FX_VOID);
    quit_system (FX_VOID);
}

/** 
 * the outer interpreter, in PFE the jumppoint for both => ABORT and => QUIT
 */
_export int
p4_interpret_loop (P4_VOID)
{
    register int err;
    switch (err = p4_setjmp(PFE.loop))
    {
     case  0:  /* newloop -> do abort*/
         /* initialize */
     case 'A': /* do abort */
         abort_system (FX_VOID);
         p4_redo_all_words (PFE.abort_wl);
         /* -> do quit */
     case 'Q': /* do quit */
         quit_system (FX_VOID);
     case 'S': /* schedule */
				/* normal interactive QUIT */
                                /* doing the QUERY-INTERPRET loop */
         p4_unnest_input (NULL);
         for (;;)
         {
             p4_do_all_words (PFE.prompt_wl);
             FX (p4_ok);
             FX (p4_cr);	
             FX (p4_query);		
             FX (p4_interpret);		
             FX (p4_Q_stack);	
         }

     case 'X': /* exit / bye */
         /* -> PFE.atexit_cleanup (); */
         return 0;
    }
    return err;
}

/* **************************************************** compiled interpret */

static const p4_char_t p4_lit_interpret[] = "(INTERPRET)";

FCode (p4_interpret_next_execution)
{
    FX_PUSH (FX (p4_interpret_next_word));
}
FCode (p4_interpret_next)
{
    FX_COMPILE (p4_interpret_next);
}
P4COMPILES(p4_interpret_next, p4_interpret_next_execution,
	   P4_SKIPS_NOTHING, P4_DEFAULT_STYLE);

FCode (p4_interpret_undefined_execution)
{
    p4_type ((p4_char_t*) "oops... ", 8); // FIXME: for debugging...
    p4_throw (P4_ON_UNDEFINED);
}
FCode (p4_interpret_undefined)
{
    FX_COMPILE (p4_interpret_undefined);
}
P4COMPILES(p4_interpret_undefined, p4_interpret_undefined_execution,
	   P4_SKIPS_NOTHING, P4_DEFAULT_STYLE);

FCode_XE (p4_interpret_nothing_execution) { 
    FX_USE_CODE_ADDR;
    FX_SKIP_BRANCH;
    FX_USE_CODE_EXIT;
}
FCode (p4_interpret_nothing) {
    FX_COMPILE (p4_interpret_nothing);
    FX_COMMA (0);
}
P4COMPILES(p4_interpret_nothing, p4_interpret_nothing_execution,
	   P4_SKIPS_OFFSET, P4_NEW1_STYLE);

FCode (p4_preload_interpret)
{
    p4_header_comma (p4_lit_interpret, sizeof(p4_lit_interpret)-1, 
		     PFE.forth_wl);
    FX_RUNTIME1 (p4_colon);
    PFE.interpret_loop = P4_BODY_FROM (p4_HERE);
    PFE.state = P4_TRUE;
    FX (p4_begin); // compiling word
    PFE.interpret_compile_resolve = ((p4cell*) p4_HERE) - 1;
    FX (p4_interpret_next); // like NEXT-WORD but returns FALSE for TERMINAL
    FX (p4_while);
    FX (p4_interpret_find);
    PFE.interpret_compile_extra = ((p4cell*) p4_HERE);
    FX (p4_interpret_nothing);
    FX (p4_interpret_number);
    PFE.interpret_compile_float = ((p4cell*) p4_HERE);
    FX (p4_interpret_nothing);
    FX (p4_interpret_undefined);
    FX (p4_repeat); // compiling word
    FX (p4_semicolon);
}

/* ********************************************************************** 
 * Initialize dictionary, and system variables, include files		  
 */

static const p4_char_t p4_lit_precision[] = "precision";
static const p4_char_t p4_lit_source_any_case[] = "source-any-case";
static const p4_char_t p4_lit_source_upper_case[] = "source-upper-case";
static const p4_char_t p4_lit_lower_case_filenames[] = "lower-case-filenames";
/**
 * setup all system variables and initialize the dictionary
 * to reach a very clean status as if right after cold boot.
 */
FCode (p4_cold_system)
{
    SP = p4_S0;
#  ifndef P4_NO_FP
    FP = p4_F0;
#  endif
#  ifdef P4_RP_IN_VM
    RP = p4_R0;
#  endif
    TIB = PFE.tib;
    BASE = 10;
    p4_DPL = -1;
    PRECISION = p4_search_option_value(p4_lit_precision,9, 6, PFE.set);
    WORDL_FLAG = 0; /* implicitly enables HASHing */
    if (p4_search_option_value(p4_lit_source_any_case,15, 
	  PFE_set.find_any_case, PFE.set)) WORDL_FLAG |= WORDL_NOCASE;
    if (p4_search_option_value(p4_lit_source_upper_case,17, 
	  PFE_set.upper_case_on, PFE.set)) WORDL_FLAG |= WORDL_UPPER_CASE;
    LOWER_CASE_FN = p4_search_option_value(p4_lit_lower_case_filenames,20, 
      PFE_set.lower_case_fn, PFE.set);
    FLOAT_INPUT = P4_opt.float_input;
    PFE.local = (p4_char_t (*)[P4_LOCALS]) PFE.stack;
    PFE.pocket = PFE.pockets_ptr;

    p4_memset (PFE.files_top - 3, 0, sizeof (File) * 3);

    PFE.stdIn = PFE.files_top - 3;
    PFE.stdIn->f = stdin;
    p4_strcpy (PFE.stdIn->name, "<STDIN>");
    p4_strcpy (PFE.stdIn->mdstr, "r");
    PFE.stdIn->mode = FMODE_RO;
    
    PFE.stdOut = PFE.files_top - 2;
    PFE.stdOut->f = stdout;
    p4_strcpy (PFE.stdOut->name, "<STDOUT>");
    p4_strcpy (PFE.stdOut->mdstr, "a");
    PFE.stdOut->mode = FMODE_WO;
    
    PFE.stdErr = PFE.files_top - 1;
    PFE.stdErr->f = stderr;
    p4_strcpy (PFE.stdErr->name, "<STDERR>");
    p4_strcpy (PFE.stdErr->mdstr, "a");
    PFE.stdErr->mode = FMODE_WO;
    
    REDEFINED_MSG = P4_FALSE;
    
    /* Wipe the dictionary: */
    p4_memset (PFE.dict, 0, (PFE.dictlimit - PFE.dict));
    p4_preload_only ();
    if (! PFE.abort_wl)     PFE.abort_wl  = p4_new_wordlist (0);
    if (! PFE.prompt_wl)    PFE.prompt_wl = p4_new_wordlist (0);
    if (! PFE.assembler_wl) PFE.assembler_wl = p4_new_wordlist (0);
    FX (p4_preload_interpret);
    FX (p4_only_RT);
    {
        /* Defines the following default search order:
         * FORTH EXTENSIONS ONLY */
        extern const p4Words P4WORDS (forth);
        p4_load_words (&P4WORDS (forth), ONLY, 0);
    }
    /* last step of bootup default search-order is 
       FORTH DEFINITIONS a.k.a.  FORTH-WORDLIST CONTEXT ! DEFINITIONS
    */
    CURRENT = CONTEXT[0] = PFE.forth_wl; /* points to FORTH vocabulary */
    FX (p4_default_order);
    
    REDEFINED_MSG = P4_TRUE;
}    

/**
 * setup all system variables and initialize the dictionary
 */
FCode (p4_boot_system)
{
    if (PFE.nr) { printf (" CPU%i ", PFE.nr); }

    /* Action of COLD ABORT and QUIT, but don't enter the interactive QUIT */
    RESET_ORDER = P4_TRUE;
    /* FX (p4_cold_system); */
    REDEFINED_MSG = P4_FALSE;
    {
#ifndef MODULE0
#define MODULE0 extensions
#endif
        extern const p4Words P4WORDS (MODULE0); 
        
#ifdef MODULE1
        extern const p4Words P4WORDS (MODULE1);
#endif
#ifdef MODULE2
        extern const p4Words P4WORDS (MODULE2);
#endif
#ifdef MODULE3
        extern const p4Words P4WORDS (MODULE3);
#endif

        p4_load_words (&P4WORDS (MODULE0), ONLY, 0);
        
#ifdef MODULE1
        p4_load_words (&P4WORDS (MODULE1), ONLY, 0);
#endif
#ifdef MODULE2
        p4_load_words (&P4WORDS (MODULE2), ONLY, 0);
#endif
#ifdef MODULE3
        p4_load_words (&P4WORDS (MODULE3), ONLY, 0);
#endif
	/* should be replaced by p4_load_words someday... fixme: */
        if (PFE.set->loadlist[0]) 
            p4_load_words (PFE.set->loadlist[0], ONLY, 0);
        if (PFE.set->loadlist[1]) 
            p4_load_words (PFE.set->loadlist[1], ONLY, 0);
        if (PFE.set->loadlist[2]) 
            p4_load_words (PFE.set->loadlist[2], ONLY, 0);
        if (PFE.set->loadlist[3]) 
            p4_load_words (PFE.set->loadlist[3], ONLY, 0);
    }

    abort_system ();
    quit_system ();

    REDEFINED_MSG = P4_FALSE;
    { 
	static const p4_char_t p4_lit_block_file[] = "block-file";
	static const p4_char_t p4_lit_boot_file[] = "boot-file";

	register const char* file;
#       ifndef PFE_BLOCK_FILE /* USER-CONFIG: --block-file=<mapped-file> */
#       define PFE_BLOCK_FILE PFE_DEFAULT_BLKFILE 
#       endif

	if ((file = (char*) p4_search_option_string (
	    p4_lit_block_file, 10, PFE_BLOCK_FILE, PFE.set)))
	{
	    if (! p4_set_blockfile (p4_open_blockfile(
		   (const p4_char_t*) file, p4_strlen (file)))
		&& p4_strcmp (file, PFE_DEFAULT_BLKFILE) != 0)
	    {
		P4_fatal1 ("Can't find block file %s", file);
		PFE.exitcode = 4;
		p4_longjmp_exit ();
	    }
	}

#       ifndef PFE_BOOT_FILE /* USER-CONFIG: --boot-file=<included-file> */
#       define PFE_BOOT_FILE 0
#       endif
	if ((file = (char*) p4_search_option_string (
	    p4_lit_boot_file, 9, PFE_BOOT_FILE, PFE.set)))
	{
	    p4_included1 ((const p4_char_t*) file, p4_strlen (file), 0);
	}
    }

    /*  read_help_index (PFE_PKGHELPDIR, "index"); */

    /* According to the ANS Forth description, the order after BOOT must
     * include the FORTH-WORDLIST, and the CURRENT definition-wordlist
     * must be the FORTH-WORDLIST. Here we assume that the various LOADs
     * before have kept atleast one occurence of FORTH-WORDLIST in the
     * search-order but we explicitly set the CURRENT definition-wordlist
     * Then we do DEFAULT-ORDER so it can pop up in a RESET-ORDER on ABORT
     * BEWARE: a bootscript can arrange the items in the search-order but
     * it can not arrange to set the CURRENT definitions-wordlist as well.
     * Note that ONLY is always searched, so one can always get back at FORTH
     * OTOH, in main-sub, the first include-file is loaded after boot_system
     * so it can arrange for a different the DEFAULT-ORDER incl. CURRENT.
     */
    CURRENT = PFE.forth_wl;
    FX (p4_default_order);

    FENCE = DP;
    LAST  = NULL;

    REDEFINED_MSG = P4_TRUE;
}

/*@}*/

/* 
 * Local variables:
 * c-file-style: "stroustrup"
 * End:
 */
