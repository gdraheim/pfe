/** 
 * --  Subroutines for the Internal Forth-System
 * 
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE            @(#) %derived_by: guidod %
 *  @version %version: 1.11 %
 *    (%date_modified: Mon Mar 12 14:09:49 2001 %)
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: engine-sub.c,v 0.30 2001-03-12 13:10:02 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-xtra.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <setjmp.h>

#ifdef _K12_SOURCE
#include <pfe/main-k12.h>
#endif

#include <pfe/double-sub.h>
#include <pfe/debug-ext.h>
#include <pfe/block-sub.h>
#include <pfe/file-sub.h>
#include <pfe/term-sub.h>
#include <pfe/_missing.h>
#include <pfe/floating-ext.h>
#include <pfe/exception-sub.h>

#ifdef PFE_WITH_DSTRINGS_EXT
#include <pfe/dstrings-ext.h>
#endif

#include <pfe/logging.h>


/* ********************************************************************** 
 * inner and outer interpreter						  
 */

/* jumpbuf is a jmp_buf enanced by space to preserve variables that
 * may be stored in cpu register, would be the Forth VM
 */
typedef struct	
{
    jmp_buf jmp;
# ifdef P4_REGRP
    p4xt **rp;
# endif
# ifdef P4_REGSP
    p4cell *sp;
# endif
# ifdef P4_REGLP
    p4cell *lp;
# endif
# ifdef P4_REGFP
    double *fp;
# endif
}
jumpbuf;

/**
 * longjmp via (jumpbuf*) following inline
 * - purpose: stop the inner interpreter
 */
FCode (p4_jump)	
{
    jumpbuf *buf = (jumpbuf *) *IP;

# ifdef P4_REGRP		/* save global register variables */
    buf->rp = RP;
# endif
# ifdef P4_REGSP
    buf->sp = SP;
# endif
# ifdef P4_REGLP
    buf->lp = LP;
# endif
# ifndef P4_NO_FP
# ifdef P4_REGFP
    buf->fp = FP;
# endif
# endif
    longjmp (buf->jmp, 1);
}

/**
 * Run a forth word from within C-code
 * - this is the inner interpreter
 */
_export void
p4_run_forth (p4xt xt)
{
    static p4code jump_p = PFX (p4_jump);
    jumpbuf stop;
    p4xt list[3];

    list[0] = xt;
    list[1] = &jump_p;
    list[2] = (p4xt) &stop;
    IP = list;
# ifndef P4_WP_VIA_IP
    p4WP = *IP;
# endif

    if (setjmp (stop.jmp))
    {
#     ifdef P4_REGRP		/* restore global register variables */
        RP = stop.rp;		/* clobbered by longjmp() */
#     endif
#     ifdef P4_REGSP
        SP = stop.sp;
#     endif
#     ifdef P4_REGLP
        LP = stop.lp;
#     endif
#     ifndef P4_NO_FP
#     ifdef P4_REGFP
        FP = stop.fp;
#     endif
#     endif
        return;
    }

    /* next_loop */
    for (;;)
    {
#     ifdef P4_WP_VIA_IP
        p4xt w;
#     define NEXT w = *IP++, (*w) ()	/* ip is register but p4WP isn't */
#     else
#     define NEXT p4WP = *IP++, (*p4WP) ()	
				/* ip and p4WP are same: register or not */
#     endif

#     ifdef UNROLL_NEXT		/* USER-CONFIG: if it helps */
        NEXT; NEXT; NEXT; NEXT;	/* do a little loop unrolling for speed */
        NEXT; NEXT; NEXT; NEXT;
#     else
        NEXT;			/* on some machines it doesn't do any good */
#     endif
    }
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
    p4_run_forth (xt);
    printf ("%X/%X\n", IP, saved_ip);
    IP = saved_ip;
    printf ("%X/%X\n\n", IP, saved_ip);
# else
    p4xt *saved_ip = IP;
    
    p4_run_forth (xt);
    IP = saved_ip;
# endif
}

/**
 * the NEXT call. Can be replaced by p4_debug_execute to
 * trace the inner forth interpreter.
 */
_export void
p4_normal_execute (p4xt xt)
{
# ifdef P4_WP_VIA_IP
    p4_call (xt);
# else
    p4WP = xt;
    (*p4WP) ();
# endif
}

/* ================= INTERPRET =================== */

static p4ucell
FXCode (p4_interpret_dstrings)
{
#  ifdef PFE_WITH_DSTRINGS_EXT       /* (dnw 3feb01) */
    if (! STATE || ! p4_MARGS_FLAG) return 0; /* quick path */
    /* WORD-string is at HERE */
    return p4_compile_marg (DP+1, *DP);
#  else
    return 0;
#  endif
}

static p4ucell 
FXCode (p4_interpret_locals)
{
    if (! STATE || ! PFE.locals) return 0; /* quick path */
    /* WORD-string is at HERE */
    return p4_compile_local (DP+1, *DP);
}

static p4ucell
FXCode (p4_interpret_smart)
{
    register p4xt xt;

    if (! PFE.smart_char) return 0; /* quick path */

    /* WORD-string is at HERE */
    xt = PFE.smart_char (DP[1]);
    if (! xt) return 0; /* quick path */

    TO_IN -= *DP; p4_skip_delimiter (DP[1]);
    p4_call (xt);
    return 1;
}

static p4ucell
FXCode (p4_interpret_find)
{
    register char *s;
    register p4xt xt;

    /* WORD-string is at HERE */
    s = p4_find (DP+1, *DP);
    if (! s) return (p4cell) s; /* quick path, even alias null returncode */

    xt = p4_name_from (s);
    if (! STATE || *_FFA(s) & P4xIMMEDIATE)
    {
	p4_call (xt);         /* execute it now */
	FX (p4_Q_stack);      /* check stack */
    }else{
	FX_COMMA (xt);        /* comma token */
    }
    return 1;
}

static p4ucell
FXCode (p4_interpret_float)
{
# ifndef P4_NO_FP
    if (! BASE == 10 || ! FLOAT_INPUT) return 0; /* quick path */

    {
	double f;
	/* WORD-string is at HERE */
	if (! p4_to_float (DP+1, *DP, &f)) 
	    return 0; /* quick path */
	
	if (STATE)
	{
#          if PFE_ALIGNOF_DFLOAT > PFE_ALIGNOF_CELL
	    if (P4_DFALIGNED (DP))
		FX_COMPILE2 (p4_f_literal);
#          endif
	    FX_COMPILE1 (p4_f_literal);
	    FX_FCOMMA (f);
	}else{ 
	    *--FP = f;
	}
	return 1;
    }
#  else
	return 0;
#  endif
}

static p4ucell
FXCode (p4_interpret_number)
{
    p4dcell d;

    /* WORD-string is at HERE */
    if (! p4_number_question (DP+1, *DP, &d))
	return 0; /* quick path */

    if (STATE)
    {
	if (p4_DPL >= 0) 
	{   
	    FX_COMPILE1 (p4_two_literal);
	    FX_COMMA (d.hi);
	}else{
	    FX_COMPILE1 (p4_literal);
	}
	FX_COMMA (d.lo);
    }else{
	*--SP = d.lo;
	if (p4_DPL >= 0) 
	    *--SP = d.hi;
    }
    return 1;
}

/**
 * the => INTERPRET as called by the outer interpreter
 */
FCode (p4_interpret)
{
    register int i;

    /* HACK: until proper initialization bindings, we do init'
     * the interpret-vectors right in here. This *will* go away.
     */
    if (! PFE.interpret[3])
    {
	PFE.interpret[6] = PFX (p4_interpret_dstrings);
	PFE.interpret[5] = PFX (p4_interpret_locals);
	PFE.interpret[4] = PFX (p4_interpret_find);
	PFE.interpret[3] = PFX (p4_interpret_number);
	PFE.interpret[2] = PFX (p4_interpret_float);
	PFE.interpret[1] = PFX (p4_interpret_smart);
    }

    for (;;)
    {
    again:
        for (;;)
        {
            p4_word (' '); /* copy parsed-string to HERE !! */
            if (*DP) break;
            
            switch (SOURCE_ID)
            {
             default:
                 if (p4_next_line ())
                     continue;
             case 0:
             case -1:
                 return;
            }
        }  

	i = DIM (PFE.interpret);
	while ( i-- )
	{
	    if (! PFE.interpret[i]) continue;
	    if (PFE.interpret[i] (FX_VOID)) goto again;
	}
        p4_throw (P4_ON_UNDEFINED);
    }
}

/**
 * => INTERPRET buffer
 */
_export void
p4_evaluate (char *p, int n)
{
    RP = (p4xt **) p4_save_input (RP);
    SOURCE_ID = -1;
    BLK = 0;
    TIB = p;
    NUMBER_TIB = n;
    TO_IN = 0;
    FX (p4_interpret);
    RP = (p4xt **) p4_restore_input (RP);
}

/**
 */
_export void
p4_include_file (p4_File *fid)
{
    if (fid == NULL || fid->f == NULL)
        p4_throws (P4_ON_FILE_NEX, fid->name, 0);
    RP = (p4xt **) p4_save_input (RP);
    SOURCE_ID = (p4cell) fid;
    BLK = 0;
    TO_IN = 0;
    FX (p4_interpret);
    RP = (p4xt **) p4_restore_input (RP);
}

/**
 * called by INCLUDED and INCLUDE
 */
_export int
p4_included1 (const char *name, int len, int throws)
{
    File* f;
    char* fn;
    
    fn = p4_pocket_expanded_filename (name, len, 
				      P4_opt.incpaths, P4_opt.incext);
    f = p4_open_file (fn, strlen (fn), FMODE_RO);
    if (!f)
    {  
        if (throws) 
        {
            p4_throws (P4_ON_FILE_NEX, fn, 0); 
        }else{ 
            P4_fail2 ("- could not open '%s' (paths='%s')\n", 
              fn, P4_opt.incpaths); 
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
p4_included (const char* name, int len)
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
        RP = (p4xt **) p4_restore_input (PFE.saved_input);
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
quit_system (void)
{
    CSP = (p4cell*) RP;         /* come_back marker */
    RP = p4_R0;			/* return stack */
    LP = NULL;			/* including all local variables */
    STATE = P4_FALSE;		/* interpreting now */
    PFE.cAtch = NULL;		/* and no exceptions caught */
    p4_debug_off ();		/* turn off debugger */
}

/*
 * things => ABORT has to initialize
 */
static void
abort_system (void)
{
    SP = p4_S0;				/* stacks */
#ifndef P4_NO_FP
    FP = p4_F0; 
#endif
#ifdef PFE_WITH_DSTRINGS_EXT         /* (dnw 5feb01) */
    p4_drop_all_strings (p4_DSTRINGS);
#endif
    FX (p4_reset_order);		/* reset search order */
    FX (p4_definitions);                /* definitions to initial voc */
    FX (p4_decimal);			/* number i/o base */
    FX (p4_standard_io);		/* disable i/o redirection */
    FX (p4_closeall_files);             /* close open filedescriptors */
}

FCode (p4_paren_abort)
{
    abort_system ();
    quit_system ();
}

/** 
 * the outer interpreter, in PFE the jumppoint for both => ABORT and => QUIT
 */
_export int
p4_interpret_loop ()
{
    register int err;
    switch (err = setjmp(PFE.loop))
    {
     case  0:  /* newloop -> do abort*/
         /* initialize */
     case 'A': /* do abort */
         abort_system ();
         /* -> do quit */
     case 'Q': /* do quit */
         quit_system ();
     case 'S': /* schedule */
				/* normal interactive QUIT */
                                /* doing the QUERY-INTERPRET loop */
         p4_unnest_input (NULL);
         for (;;)
         {
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

/* ********************************************************************** 
 * Initialize dictionary, and system variables, include files		  
 */

/**
 * setup all system variables and initialize the dictionary
 * to reach a very clean status as if right after cold boot.
 */
static void
cold_system (void)
{
    SP = p4_S0;
#  ifndef P4_NO_FP
    FP = p4_F0;
#  endif
    RP = p4_R0;
    TIB = PFE.tib;
    BASE = 10;
    p4_DPL = -1;
    PRECISION = 6;
    WORDL_FLAG = P4_opt.lower_case_on; /* implicitly enables HASHing */
    LOWER_CASE_FN = P4_opt.lower_case_fn;
    FLOAT_INPUT = P4_opt.float_input;
    PFE.local = (char (*)[32]) PFE.stack;

    memset (PFE.files_top - 3, 0, sizeof (File) * 3);

    PFE.stdIn = PFE.files_top - 3;
    PFE.stdIn->f = stdin;
    strcpy (PFE.stdIn->name, "<STDIN>");
    strcpy (PFE.stdIn->mdstr, "r");
    PFE.stdIn->mode = FMODE_RO;
    
    PFE.stdOut = PFE.files_top - 2;
    PFE.stdOut->f = stdout;
    strcpy (PFE.stdOut->name, "<STDOUT>");
    strcpy (PFE.stdOut->mdstr, "a");
    PFE.stdOut->mode = FMODE_WO;
    
    PFE.stdErr = PFE.files_top - 1;
    PFE.stdErr->f = stderr;
    strcpy (PFE.stdErr->name, "<STDERR>");
    strcpy (PFE.stdErr->mdstr, "a");
    PFE.stdErr->mode = FMODE_WO;
    
    REDEFINED_MSG = P4_FALSE;
    
    /* Wipe the dictionary: */
    memset (PFE.dict, 0, (PFE.dictlimit - PFE.dict));
    p4_preload_only ();
    FX (p4_only_RT);
    {
        /* Defines the following default search order:
         * FORTH EXTENSIONS ONLY */
#ifndef MODULE0
#define MODULE0 extensions
#endif
        extern const p4Words P4WORDS (forth);
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

        p4_load_words (&P4WORDS (forth), ONLY, 0);
        p4_load_words (&P4WORDS (MODULE0), ONLY, 0);
        
#ifdef MODULE1
        p4_load_words (&P4WORDS (MODULE1), ONLY, 0);
#endif
#ifdef MODULE2
        p4_load_llist (&P4WORDS (MODULE2), ONLY, 0);
#endif
#ifdef MODULE3
        p4_load_llist (&P4WORDS (MODULE3), ONLY, 0);
#endif
	/* should be replaced by p4_load_words someday... fixme: */
        if (PFE.set->loadlist[0]) 
            p4_load_llist (PFE.set->loadlist[0], ONLY, 0);
        if (PFE.set->loadlist[1]) 
            p4_load_llist (PFE.set->loadlist[1], ONLY, 0);
        if (PFE.set->loadlist[2]) 
            p4_load_llist (PFE.set->loadlist[2], ONLY, 0);
        if (PFE.set->loadlist[3]) 
            p4_load_llist (PFE.set->loadlist[3], ONLY, 0);
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
_export void
p4_boot_system (void)
{
    if (PFE.nr) { printf (" CPU%i ", PFE.nr); }

    /* Action of COLD ABORT and QUIT, but don't enter the interactive QUIT */
    cold_system ();
    abort_system ();
    quit_system ();

    REDEFINED_MSG = P4_FALSE;

    if (P4_opt.block_file)
    {
        if (!p4_use_block_file (P4_opt.block_file, strlen (P4_opt.block_file))
	    && strcmp (P4_opt.block_file, PFE_DEFAULT_BLKFILE) != 0)
	{
            P4_fatal1 ("Can't find block file %s", P4_opt.block_file);
	    PFE.exitcode = 4;
	    p4_longjmp_exit ();
	}
    }

    /* Include file from command line: */
    if (P4_opt.boot_file)
    {
        p4_included1 (P4_opt.boot_file, strlen (P4_opt.boot_file), 0);
    }

    /*  read_help_index (PFE_PKGHELPDIR, "index"); */

    FENCE = DP;
    LAST  = NULL;

    REDEFINED_MSG = P4_TRUE;
}

/*@}*/








