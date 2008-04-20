/** 
 * -- miscellaneous useful words, mostly stemming from fig-forth
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.6 $
 *     (modified $Date: 2008-04-20 04:46:31 $)
 *
 *  @description
 *      Compatiblity with former standards, miscellaneous useful words.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: misc-ext.c,v 1.6 2008-04-20 04:46:31 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>

#include <stdlib.h>
#include <pfe/os-string.h>
#include <errno.h>
#include <signal.h>

#include <pfe/def-comp.h>
#include <pfe/term-sub.h>
#include <pfe/file-sub.h>
#include <pfe/double-sub.h>
#include <pfe/block-ext.h>
#include <pfe/exception-sub.h>
#include <pfe/version-sub.h>
#include <pfe/core-mix.h>
#include <pfe/block-mix.h>

#include <pfe/def-words.h>
#include <pfe/logging.h>
#include <pfe/_missing.h>

/** COLD ( -- ) [FTH]
 * cold abort - reinitialize everything and go to => QUIT routine
 * ... this routine is implemented as a warm-boot in pfe.
 : COLD [ ALSO ENVIRONMENT ] EMPTY SCRIPT-FILE INCLUDED QUIT ;
 */
FCode (p4_cold)
{
    FX (p4_close_all_files);

#if 0
    PFE.atexit_running = 1;
    p4_forget (PFE.dict);
    PFE.atexit_running = 0;

    FX (p4_cold_system); 
    FX (p4_boot_system); 
#else
    PFE.atexit_running = 1;
    {
        p4_namebuf_t* golden = p4_search_wordlist ((p4_char_t*) "EMPTY", 5, 
						   PFE.environ_wl);
        if (golden)
        {
            p4xt xt = p4_name_from(golden);
            if (*P4_TO_CODE(xt) != PFX (p4_marker_RT))
                P4_fail ("COLD found non-MARKER named EMPTY in ENVIRONMENT,"
                         " that is suspicious but still going to execute it");

            /*do we need a CATCH-domain here? that'd be usage errors, right?*/
            golden = PFE.dp;
            p4_call (xt); /* runs => (FORGET) with MARKER-address */
            if (PFE.dp == golden)
            {
                P4_fail ("COLD did run EMPTY but dictionary space was not"
                         " touched, so now do FORGET>FENCE additionally");
                p4_forget (FENCE);
            }
        }else{
            p4_forget (FENCE);
        }
    }
    PFE.atexit_running = 0;
#endif

    FX (p4_paren_abort);
    FX (p4_script_files);

    /* If it's a turnkey-application, start it: */
    if (APPLICATION)
    {
        p4_call_loop (APPLICATION);
        p4_longjmp_exit (); 
    }
    if (P4_opt.verbose)
        FX (p4_dot_memory);
    /* p4_longjmp_abort (); -> paren_abort + yield */
    p4_longjmp_yield ();
}

/** .LINE ( line# block# -- ) [FTH]
 */
FCode (p4_dot_line)			
{
    p4_dot_line (BLOCK_FILE, SP[0], SP[1]);
    SP += 2;
}


/************************************************************************/
/* some well known words without pedigree                               */
/************************************************************************/

/** UD.R ( x,x# r# -- ) [FTH]
 */
FCode (p4_u_d_dot_r)	
{
    p4cell w = *SP++;

    FX (p4_less_sh);
    FX (p4_sh_s);
    FX (p4_sh_greater);
    p4_emits (w - *SP, ' ');
    FX (p4_type);
}

/** UD. ( x,x# -- ) [FTH]
 * see also => UD.R
 */
FCode (p4_u_d_dot)
{
    *--SP = 0;
    FX (p4_u_d_dot_r);
    FX (p4_space);
}

/** .NAME ( some-nfa* -- ) [FTH]
 * use => ID. or better => NAME>STRING => TYPE
 * (the => .NAME word is not very portable across forth systems)
 */

/** ID. ( some-nfa* -- ) [FTH]
 * print the name-field pointed to by the nfa-argument.
 * a synonym for .NAME - but this word is more portable due its
 * heritage from fig-forth. 
 * 
 * in fig-forth the name-field is effectivly a bstring with some flags,
 * so the nfa's count has to be masked out, e.g. 
 : .NAME COUNT 32 AND TYPE ;
 *
 * in other pfe configurations, the name might not contain the flags it
 * it just a counted string - and there may be even more possibilities.
 : .NAME COUNT TYPE ;
 * 
 * you should more and more convert your code to use the sequence
 * => NAME>STRING => TYPE which is widely regarded as the better
 * variant.
 */
FCode(p4_id_dot)
{
    /* Anton Ertl (gforth), Tom Zimmer (win32for), Guido U. Draheim (pfe)
     * agreed to export ID. as a portable function since win32for's
     * .NAME did not match the gforth' and pfe' variant of the same
     * name. The ID. has a heritage dating back to fig-forth, and was
     * present in tek'mforth too.
     */
    p4_dot_name ((p4_char_t *) *SP++);
}

/** -ROLL ( x...[n-1] y n# -- y x...[n-1] |  num# -- ) [FTH]
 * the inverse of => ROLL
 */
FCode (p4_dash_roll)
{
    p4cell n = *SP++;
    p4cell h, i;

    h = SP[0];
    for (i = 0; i < n; i++)
        SP[i] = SP[i + 1];
    SP[i] = h;
}

/* some systems (BSD) have a better random number generator than
   standard unix' rand() 
*/
#if defined PFE_HAVE_RANDOM
# define _rand_ random
#else
# define _rand_ rand
#endif

/** RANDOM ( n# -- random# ) [FTH]
 * returns random number with 0 <= n2 < n1)
 : RANDOM ?DUP IF _random SWAP MOD ELSE _random THEN ;
 */
FCode (p4_random)			
{				
    if (*SP == 0)
        *SP = _rand_ ();
    else
    {
#     if (PFE_SIZEOF_CELL == 2 && RAND_MAX-0 == 32767) \
      || (PFE_SIZEOF_CELL == 4 && RAND_MAX-0 == 2147483647L)
        /* ansi-rand has 15-bit, and most unix-rand have 31-bit */
        *SP = p4_d_ummul (*SP, _rand_ () << 1).hi;
#     elif (PFE_SIZEOF_CELL >= 4) 
        if (*(p4ucell*)SP < 32767)
        { /* many systems are 32-bit or better */
            *(p4ucell*)SP *= (p4ucell) _rand_ () & 32767;
            *(p4ucell*)SP >>= 15;
        }else
            *SP = ((p4ucell) _rand_ ()) % (*(p4ucell*)SP);
#     else
        *SP = ((p4ucell) _rand_ ()) % (*(p4ucell*)SP);
#     endif        
    }
}

#undef rand

/** SRAND ( seed# -- ) [FTH]
 */
FCode (p4_srand)			
{
#  if defined PFE_HAVE_RANDOM
    srandom (*SP++);
#  else
    srand (*SP++);
#  endif
}

/** UNDER+ ( n1 x n2 -- n1+n2 x [??] | n1 n2 -- n1+n2 n2 [??] ) [FTH]
 *     quicker than
 : UNDER+  ROT + SWAP ;
 * Note: the old pfe version of UNDER+ is obsolete as it is in conflict
 * with a comus word of the same name. The behavior of this word will
 * continue to exist under the name of =>"(UNDER+)". Users are encouraged
 * to use the comus behavior of UNDER+ which does already exist under
 * the name of =>"+UNDER". In the future pfe will be changed to pick up
 * the comus behavior making UNDER+ and +UNDER to be synonyms. In the
 * current version there will be load-time warning on usages of "UNDER+".
 */

/** +UNDER ( n1 x n2 -- n1+n2 x ) [EXT]
 *     quicker than
 : UNDER+  ROT + SWAP ;
 * Note: the old pfe version of UNDER+ is obsolete as it is in conflict
 * with a comus word of the same name. The behavior of this word will
 * continue to exist under the name of =>"(UNDER+)". Users are encouraged
 * to use the comus behavior of UNDER+ which does already exist under
 * the name of =>"+UNDER". In the future pfe will be changed to pick up
 * the comus behavior making UNDER+ and +UNDER to be synonyms. In the
 * current version there will be load-time warning on usages of "UNDER+".
 */
FCode (p4_plus_under)
{
    p4cell n = FX_POP;
    SP[1] += n;
}

/** "(UNDER+)" ( n1 n2 -- n1+n2 n2 ) [FTH]
 * quicker than
 : (UNDER+) TUCK + SWAP ; or : (UNDER+) DUP UNDER+ ;
 */
FCode (p4_under_plus)
{		
    SP[1] += SP[0];
}

/************************************************************************/
/* more local variables                                                 */
/************************************************************************/

/** ((+TO)) ( val -- ) [HIDDEN]
 * execution compiled by => +TO
 * adds the stack-val to the lvalue compiled
 */
FCode_XE (p4_plus_to_execution)	
{	
    FX_USE_CODE_ADDR;
    *p4_to_body ((p4xt)(*IP++)) += *SP++;
    FX_USE_CODE_EXIT;
}

/** ((+TO.local)) ( val -- ) [HIDDEN]
 * same as => ((+TO)) when the lvalue is a => LOCALS| value
 * <br> compiled by => +TO
 */
FCode_XE (p4_plus_to_local_execution)
{
    FX_USE_CODE_ADDR;
    LP[(p4cell) *IP++] += *SP++;
    FX_USE_CODE_EXIT;
}

/** +TO ( val [name] -- ) [FTH]
 * add the val to the named => VALUE or => LOCALS| value
 */
FCode (p4_plus_to)
{
    if (STATE)
    {
        p4_char_t *p;
        int l, n;

        p = p4_word (' ');
        l = *p++;
        if (PFE.locals && (n = p4_find_local (p, l)) != 0)
	{
            FX_COMPILE2 (p4_plus_to);
            FX_UCOMMA (n);
	}else{
            if ((p = p4_find (p, l)) == NULL)
                p4_throw (P4_ON_UNDEFINED);
            FX_COMPILE1 (p4_plus_to);
            FX_XCOMMA (p4_name_from (p));
	}
    }else{
        *p4_to_body (p4_tick_cfa (FX_VOID)) += FX_POP;
    }
}
P4COMPILES2 (p4_plus_to, p4_plus_to_execution, p4_plus_to_local_execution,
  P4_SKIPS_TO_TOKEN, P4_DEFAULT_STYLE);

/************************************************************************/
/* data structures                                                      */
/************************************************************************/

/** BUILD-ARRAY ( x#...[dim] dim# -- memsize# ) [FTH]
 * writes X, n1, ... nX into the dictionary - 
 * returns product n1 * n2 * ... * nX 
 */
FCode (p4_build_array)		
{				
    p4cell i = *SP++;		
    p4ucell n = 1;

    FX_UCOMMA (i);
    while (--i >= 0)
    {
        FX_UCOMMA (*SP);
        n *= *SP++;
    }
    *--SP = n;
}

/** ACCESS-ARRAY ( x#...[dim#] array* --- array* value# ) [FTH]
 * see => BUILD-ARRAY
 */
FCode (p4_access_array)	
{
    p4ucell *p = (p4ucell *) *SP++, n = 0;
    p4cell i = *p++;

    for (;;)
    {
        if (*p++ <= (p4ucell) *SP)
            p4_throw (P4_ON_INDEX_RANGE);
        n += *SP++;
        if (--i <= 0)
            break;
        n *= *p;
    }
    *--SP = (p4cell) p;
    *--SP = n;
}


/************************************************************************/
/* implementation                                                       */
/************************************************************************/

/** SOURCE-LINE ( -- source-line# ) [FTH]
 * if => SOURCE is from => EVALUATE (or => QUERY ) then
 * the result is 0 else the line-numbers start from 1 
 */
FCode (p4_source_line)
{
    switch (SOURCE_ID)
    {
    case 0:
	if (BLK)
	{
	    *--SP = TO_IN / 64 + 1;	/* source line from BLOCK */
	    break;
	}
	/* else fallthrough */
    case -1:			/* string from EVALUATE */
	*--SP = 0;		/* or from QUERY (0/BLK==0) */
	break;
    default:			/* source line from text file */
	*--SP = SOURCE_FILE->n + 1;
    }
}

/** SOURCE-NAME ( -- source-name-ptr source-name-len ) [FTH]
 * if => SOURCE is from => INCLUDE then the result is the filename,
 * otherwise a generic name for the SOURCE-ID is given.
 */
FCode (p4_source_name)
{
    switch (SOURCE_ID)
    {
    case 0:
	if (BLK) FX_PUSH("*block#*");
	else FX_PUSH ("*query*"); /*correct?*/
	break;
    case -1:	
	FX_PUSH ("*evaluate*");
	break;
    default:			/* source line from text file */
	FX_PUSH (SOURCE_FILE->name);
    }
    FX_1ROOM; SP[0] = p4_strlen((char*)(SP[1]));
}

/** TH'POCKET ( pocket# -- pocket-ptr pocket-len ) [FTH]
 * returns the specified pocket as a => S" string reference
 */
FCode (p4_th_pocket)			
{			
    p4cell n = FX_POP;
    p4char* str = PFE.pockets_ptr[n].buffer;
    FX_PUSH (str + 1);
    FX_PUSH (*str);
}

/** POCKET-PAD ( -- pocket-ptr ) [FTH]
 * Returns the next pocket.
 * A pocket has usually the size of a maxstring, see =>"ENVIRONMENT /STRING"
 * (but can be configured to be different, mostly when MAXPATH > /STRING )
 * Note that a pocket is a temporary and forth internal functions do
 * sometimes call => POCKET-PAD too, especially when building filenames
 * and getting a literal (but temporary) string from the keyboard.
 * Functions are not expected to hold references to this transient
 * area any longer than building a name and calling another word with it.

 * Usage of a pocket pad is a good way to make local temporary buffers
 * superfluous that are only used to construct a temporary string that 
 * usually gets swallowed by another function.
 depracated code:
   create temp-buffer 255 allot
   : make-temp ( str buf ) 
          temp-buffer place  " .tmp" count temp-buffer append 
          temp-buffer count make-file ;
 replace with this:
   : make-temp ( str buf )
        pocket-pad >r    
        r place  " .tmp" count r append
        r> count make-file
   ;
 */
FCode (p4_pocket_pad)
{
    FX_PUSH (p4_pocket());
}

/** WL-HASH ( buf-ptr buf-len -- buf-hash# ) [FTH]
 * calc hash-code for selection of thread
 * in a threaded-vocabulary
 */
FCode (p4_wl_hash)	
{			
    SP[1] = p4_wl_hash ((p4_char_t *) SP[1], SP[0]);
    SP++;
}

/** TOPMOST ( some-wordlist* -- some-topmost-nfa* ) [FTH]
 * that last valid word in the specified vocabulary
 */
FCode (p4_topmost)
{
    *SP = (p4cell) p4_topmost ((Wordl *) *SP);
}

/* .................. */

static void
ls_words (char const * cat)
{
    Wordl *wl = CONTEXT[0] ? CONTEXT[0] : ONLY;
    char *pattern = (char*) p4_word (' ');

    if (*pattern == 0)
        p4_strcpy (pattern, "\001*");
    p4_outf ("\nWords matching %s:", pattern + 1);
    p4_wild_words (wl, pattern + 1, cat);
}

/** LS.WORDS ( -- ) [FTH]
 * see => WORDS
 */
FCode (p4_ls_words)		{ ls_words (NULL); }

/** LS.PRIMITIVES ( -- ) [FTH]
 * see => WORDS
 */
FCode (p4_ls_primitives)	{ ls_words ("p"); }

/** LS.COLON-DEFS ( -- ) [FTH]
 * see => WORDS
 */
FCode (p4_ls_cdefs)		{ ls_words (":"); }

/** LS.DOES-DEFS ( -- ) [FTH]
 * see => WORDS
 */
FCode (p4_ls_ddefs)		{ ls_words ("D"); }

/** LS.CONSTANTS ( -- ) [FTH]
 * see => WORDS
 */
FCode (p4_ls_constants)		{ ls_words ("cC"); }

/** LS.VARIABLES ( -- ) [FTH]
 * see => WORDS
 */
FCode (p4_ls_variables)		{ ls_words ("vV"); }

/** LS.VOCABULARIES ( -- ) [FTH]
 * see => WORDS
 */
FCode (p4_ls_vocabularies)	{ ls_words ("W"); }

/** LS.MARKERS ( -- ) [FTH]
 * see => WORDS
 */
FCode (p4_ls_markers)		{ ls_words ("M"); }

/* ............... */

/** W@ ( some-wchar* -- some-wchar# | some* -- some# [?] ) [FTH]
 * fetch a 2byte-val from address
 */
FCode (p4_w_fetch)
{
    *SP = *(short *) *SP;
}

/** W! ( value#  some-wchar* -- | value# wchar* -- [?] ) [FTH]
 * store a 2byte-val at addressed 2byte-value
 */
FCode (p4_w_store)			
{
    *(short *) SP[0] = (short) SP[1];
    SP += 2;
}

/** W+! ( value# some-wchar* -- | value# wchar* -- [?] ) [FTH]
 * add a 2byte-val to addressed 2byte-value
 */
FCode (p4_w_plus_store)	
{
    *(short *) SP[0] += (short) SP[1];
    SP += 2;
}

/** TAB ( tab-n# -- ) [FTH]
 * jump to next column divisible by n 
 */
FCode (p4_tab)			
{			
    p4_tab (*SP++);
}

/** BACKSPACE ( -- ) [FTH]
 * reverse of => SPACE
 */
FCode (p4_backspace)
{
    p4_outs ("\b \b");
}

/** ?STOP ( -- stop-flag ) [FTH]
 * check for 'q' pressed
 * - see => ?CR
 */
FCode (p4_Q_stop)
{
    *--SP = P4_FLAG (p4_Q_stop ());
}

/** START?CR ( -- ) [FTH]
 * initialized for more-like effect
 * - see => ?CR
 */
FCode (p4_start_Q_cr)	
{				
    PFE.more = PFE.rows - 2;
    PFE.lines = 0;
}

/** ?CR ( -- cr-flag ) [FTH]
 * like => CR , stop 25 lines past => START?CR
 */
FCode (p4_Q_cr)	
{		
    *--SP = p4_Q_cr ();
}

/** CLOSE-ALL-FILES ( -- ) [FTH]
 */
FCode (p4_close_all_files)
{
    File *f = 0;

    for (f = PFE.files; f < PFE.files_top - 3; f++)
    {
        if (f->f)
	{
            if (f->updated)
	    {
                p4_read_write (f, f->buffer, f->n, P4_FALSE);
	    }
            fclose (f->f);
	}
    }
}

/** .MEMORY ( -- ) [FTH]
 */
FCode (p4_dot_memory)
{
    p4_outf ("\nDictionary space:    %7ld Bytes, in use: %7ld Bytes\n"
	     "Stack space:         %7ld %s\n"
	     "Floating stack space:%7ld %s\n"
	     "Return stack space:  %7ld %s\n",
	     /* the C language returns n as n*sizeof==bytes */
	     (p4celll) (PFE.dictlimit - PFE.dict),
	     (p4celll) (PFE.dp - PFE.dict), 
	     (p4celll) (PFE.s0 - PFE.stack),  /* sizeof (p4cell) */
	     (PFE.dstrings ? "cells, (extra dstrings stack)" : "cells"),
	     (p4celll) (PFE.f0 - PFE.fstack), /* sizeof (double) */
	     (PFE.f0 ? "floats" : "       (not used)"),
	     (p4celll) (PFE.r0 - PFE.rstack),  /* sizeof (p4xt**) */
	     ("cells, (not the C call stack)"));
}


/** .STATUS ( -- ) [FTH]
 * display internal variables 
 : .STATUS .VERSION .CVERSION .MEMORY .SEARCHPATHS .DICTVARS .REGSUSED ;
 */
FCode (p4_dot_status)
{
    extern const char p4_dl_def[]; /* dl-def.c */

    FX (p4_cr);
    FX (p4_dot_version);
    FX (p4_cr);
    FX (p4_dot_date); p4_outs(p4_dl_def);
    FX (p4_cr);
    p4_outf ("\nMemory overview:");
    FX (p4_dot_memory);
    p4_outf ("\nsearch path for binary modules:   %s", *P4_opt.lib_paths);
    p4_outf ("\nsearching for help files in:      %s", PFE_PKGHELPDIR);
    p4_outf ("\nsearch path for block files:      %s", *P4_opt.blk_paths);
    p4_outf ("\nextensions for block files:       %s", *P4_opt.blk_ext);
    p4_outf ("\nsearch path for source files:     %s", *P4_opt.inc_paths);
    p4_outf ("\nextensions for source files:      %s", *P4_opt.inc_ext);
    FX (p4_cr);
/*  p4_outf ("\nmaximum number of open files:     %u", P4_opt.max_files); */
/*  p4_outf ("\nmaximum simultaneous S\" pockets   %u", P4_opt.pockets);  */
    p4_outf ("\ndictionary threads configured     %u", 1<<LD_THREADS);
    p4_outf ("\nmaximum wordlists in search order %u", P4_opt.wordlists);
    FX (p4_cr);
    p4_outf ("\nText screen size:                 %dx%d", PFE.rows, PFE.cols);

#  define flag(X) ((X) ? "ON " : "OFF")
    p4_outf ("\n      CASELESS %s", flag (LOWER_CASE));
    p4_outf ("\nUPPER-CASE-IS  %s", flag (UPPER_CASE));
#  ifdef P4_REGTH
    p4_outs ("           REGTH="P4_REGTH);
#  elif defined PFE_USE_THREAD_BLOCK
    p4_outs ("           (static regVM block)");
#  else
    p4_outs ("           (static regTH pointer)");
#  endif
    p4_outf ("\nLOWER-CASE-FN  %s", flag (LOWER_CASE_FN));
#  ifdef P4_REGSP
    p4_outs ("           REGSP="P4_REGSP);
#  endif
    p4_outf ("\nRESET-ORDER-IS %s", flag (RESET_ORDER));
#  ifdef P4_REGIP
    p4_outs ("           REGIP="P4_REGIP);
#  elif defined PFE_SBR_CALL_THREADING
    p4_outs ("           REGIP (hardware)");
#  endif
    p4_outf ("\nREDEFINED-MSG  %s", flag (REDEFINED_MSG));
#  ifdef P4_REGRP
    p4_outs ("           REGRP="P4_REGRP);
#  elif defined PFE_SBR_CALL_THREADING
    p4_outs ("           REGRP (hardware)");
#  endif
    p4_outf ("\nFLOAT-INPUT    %s", flag (FLOAT_INPUT));
#  ifdef P4_REGLP
    p4_outs ("           REGLP="P4_REGLP);
#  elif defined PFE_REGFP
    p4_outs ("           REGFP="P4_REGFP);
#  endif
#  undef flag
    p4_outf ("\nPRECISION     %3d", (int) PRECISION);
#  ifdef P4_REGW
    p4_outs ("            REGW="P4_REGW" (threading)");
#  elif defined PFE_SBR_CALL_ARG_THREADING
    p4_outs ("           (sbr-call-arg-threading)");
#  elif defined PFE_SBR_CALL_THREADING
    p4_outs ("           (sbr-call-threading)");
#  elif defined PFE_CALL_THREADING
    p4_outs ("           (call-threading)");
#  else
    p4_outs ("           (traditional-threading)");
#  endif
    FX (p4_space);
}

/************************************************************************/
/* vectorized I/O                                                       */
/************************************************************************/

/** *EMIT* ( -- handler-xt* ) [FTH]
 * initialized to =>"(EMIT)"
 */

/** (EMIT) ( char# -- ) [FTH]
 * like => EMIT and always to screen 
 * - the routine to be put into => *EMIT*
 */
FCode (p4_paren_emit)
{
    p4_outc ((char) *SP++);
}

/** *EXPECT* ( -- handler-xt* ) [FTH]
 * initialized to =>"(EXPECT)"
 */

/** (EXPECT) ( a b -- ) [FTH]
 * like => EXPECT and always from screen
 * - the routine to be put into => *EXPECT*
 */
FCode (p4_paren_expect)
{
    p4_expect ((char *) SP[1], SP[0]);
    SP += 2;
}

/** *KEY* ( -- handler-xt* ) [FTH]
 * initialized to =>"(KEY)"
 */

/** (KEY) ( -- key# ) [FTH]
 * like => KEY and always from screen
 * - the routine to be put into => *KEY*
 */
FCode (p4_paren_key)
{
    int c;

    do {
        c = p4_getekey ();
    } while (c >= 0x100);
    *--SP = c;
}

/** (TYPE) ( str* len# -- ) [FTH]
 * like => TYPE and always to screen 
 * - the routine to be put into => *TYPE*
 */
FCode (p4_paren_type)
{
    p4_type ((p4_char_t *) SP[1], SP[0]);
    SP += 2;
}

/** STANDARD-I/O ( -- ) [FTH]
 * initialize => *TYPE* , => *EMIT* , => *EXPECT* and => *KEY*
 * to point directly to the screen I/O routines, <br> 
 * namely => (TYPE) , => (EMIT) , => (EXPECT) , => (KEY) 
 */
FCode (p4_standard_io)
{
#  ifdef PFE_CALL_THREADING
    static const p4Word paren_emit_w =   { "",  PFX (p4_paren_emit) };
    static const p4Word paren_expect_w = { "", PFX (p4_paren_expect) };
    static const p4Word paren_key_w =    { "", PFX (p4_paren_key) };
    static const p4Word paren_type_w =   { "", PFX (p4_paren_type) };
#  ifndef __GNUC__
    static const p4Word* paren_emit_xt =   & paren_emit_w;
    static const p4Word* paren_expect_xt = & paren_expect_w;
    static const p4Word* paren_key_xt =    & paren_key_w;
    static const p4Word* paren_type_xt =   & paren_type_w;
#  else /* the gcc can grok this: */
    static p4_ExecToken paren_emit_xt =   { .word = (p4Word*) & paren_emit_w };
    static p4_ExecToken paren_expect_xt = { .word = (p4Word*) &paren_expect_w};
    static p4_ExecToken paren_key_xt =    { .word = (p4Word*) & paren_key_w };
    static p4_ExecToken paren_type_xt =   { .word = (p4Word*) & paren_type_w };
#  endif

    PFE.emit =   &paren_emit_xt;
    PFE.expect = &paren_expect_xt;
    PFE.key =    &paren_key_xt;
    PFE.type =   &paren_type_xt;
#  else
    static p4code paren_emit_xt =   PFX (p4_paren_emit);
    static p4code paren_expect_xt = PFX (p4_paren_expect);
    static p4code paren_key_xt =    PFX (p4_paren_key);
    static p4code paren_type_xt =   PFX (p4_paren_type);

    PFE.emit =   &paren_emit_xt;
    PFE.expect = &paren_expect_xt;
    PFE.key =    &paren_key_xt;
    PFE.type =   &paren_type_xt;
#  endif
}

/************************************************************************/
/* Function keys on the commandline                                     */
/************************************************************************/

static void
p4_fkey_call_execution (int n)
{
    if (PFE.fkey_xt[n])
        p4_call (PFE.fkey_xt[n]);
}

static void
p4_fkey_store_execution (p4xt xt, int key)
{
    if (key < P4_KEY_k1 || P4_KEY_k0 < key)
        p4_throw (P4_ON_ARG_TYPE);
    PFE.fkey_xt[key - P4_KEY_k1] = xt;
}

/*data*/ void (*p4_fkey_default_executes[10]) (int) =
{
    p4_fkey_call_execution, p4_fkey_call_execution, 
    p4_fkey_call_execution, p4_fkey_call_execution, 
    p4_fkey_call_execution, p4_fkey_call_execution,
    p4_fkey_call_execution, p4_fkey_call_execution, 
    p4_fkey_call_execution, p4_fkey_call_execution,
};

/** ((EXECUTES)) ( fkey# -- ) [HIDDEN]
 * compiled by => EXECUTES
 */
FCode_XE (p4_executes_execution)
{
    FX_USE_CODE_ADDR;
    p4_fkey_store_execution ((p4xt)(*IP++), *SP++);
    FX_USE_CODE_EXIT;
}

/** EXECUTES ( fkey# [word] -- ) [EXT]
 * stores the execution token of following word into
 * the callback pointer for the specified function-key 
 */
FCode (p4_executes)
{
    if (STATE)
    {
        FX_COMPILE (p4_executes);
        FX (p4_bracket_compile);
    }else{
        p4_fkey_store_execution (p4_tick_cfa (FX_VOID), FX_POP);
    }
}
P4COMPILES (p4_executes, p4_executes_execution,
  P4_SKIPS_NOTHING, P4_DEFAULT_STYLE);

/************************************************************************/
/* display help                                                         */
/************************************************************************/

/** HELP ( "name" -- ) [FTH] [EXEC]
 * will load the help module in the background and hand over the 
 * parsed name to => (HELP) to be resolved. If no => (HELP) word
 * can be loaded, nothing will happen.
 */
FCode (p4_help)
{
    p4_char_t* wordpad = p4_pocket ();

    FX (p4_Q_exec);

    p4_word_parseword (' '); *DP=0; /* PARSE-WORD-NOHERE */
    if (! PFE.word.len || PFE.word.len > P4_POCKET_SIZE) { return; }
    
    memcpy (wordpad, PFE.word.ptr, PFE.word.len); 
    if (LOWER_CASE)
        p4_upper (wordpad, PFE.word.len);
    {
	extern void* p4_loadm_once (const p4char* nm, int l);
	register p4_namebuf_t* name;
	register int wordlen = PFE.word.len; /* loadm might parse */

	p4_loadm_once ((const p4_char_t*) "\thelp", 5);
	if ((name = p4_search_wordlist (
		 (const p4_char_t*) "(HELP)", 6, PFE.forth_wl)))
	{
	    FX (p4_cr);
	    FX_PUSH(wordpad); /* arguments for => (HELP) */
	    FX_PUSH(wordlen);
	    p4_call(p4_name_from(name));
	}
    }
}

/** EDIT-BLOCKFILE ( "name" -- ) [FTH] [EXEC]
 * will load the edit module in the background and look for a word
 * called => EDIT-BLOCK that could be used to edit the blockfile.
 * If no => EDIT-BLOCKFILE word can be loaded, nothing will happen.
 * Otherwise, => OPEN-BLOCKFILE is called followed by => 0 => EDIT-BLOCK
 * to start editing the file at the first block.
 */
FCode (p4_edit_blockfile)
{
    p4_char_t* wordpad = p4_pocket ();

    FX (p4_Q_exec);

    p4_word_parseword (' '); *DP=0; /* PARSE-WORD-NOHERE */
    if (! PFE.word.len) { return; }

    wordpad = (p4_char_t*) p4_pocket_expanded_filename (
        PFE.word.ptr, PFE.word.len, *PFE_set.blk_paths, *PFE_set.blk_ext);
    {
	extern void* p4_loadm_once (const p4char* nm, int l);
	register p4char* name;
	register int wordlen = PFE.word.len; /* loadm might parse */

	p4_loadm_once ((const p4_char_t*) "\tedit", 5);
	if ((name = p4_search_wordlist (
		 (const p4_char_t*) "EDIT-BLOCK-START", 16, PFE.forth_wl)))
	{
	    /* see => OPEN-BLOCKFILE */
	    FX (p4_close_blockfile);
	    if (! p4_set_blockfile (p4_open_blockfile (wordpad, wordlen)))
		p4_throws (FX_IOR, wordpad, wordlen);

	    FX_PUSH(0); /* argument for => EDIT-BLOCK */
	    p4_call(p4_name_from(name));
	}
    }
}

/************************************************************************/
/* hooks to editors and os services                                     */
/************************************************************************/

/** ARGC ( -- arg-count ) [FTH]
 */
FCode (p4_argc)                      
{
    FX_PUSH (P4_opt.argc);
}

/** ARGV ( arg-n# -- arg-ptr arg-len ) [FTH]
 */
FCode (p4_argv)	
{
    p4ucell n = *SP++;

    if (n < (p4ucell) P4_opt.argc)
        p4_strpush (P4_opt.argv [n]);
    else
        p4_strpush (NULL);
}

/** EXPAND-FN ( name-ptr name-len buf-ptr -- buf-ptr buf-len ) [FTH]
 : e.g. s" includefile" POCKET-PAD EXPAND-FN ;
 */
FCode (p4_expand_fn)		
{
    p4_char_t *nm = (p4_char_t *) SP[2];
    p4cell len = SP[1];
    char *fn = (char *) SP[0];
    char* buf;

    buf = p4_pocket_expanded_filename (
        nm, len, *P4_opt.inc_paths, *P4_opt.inc_ext);
    p4_strcpy (fn, buf);
    SP++;
    SP[1] = (p4cell) fn;
    SP[0] = p4_strlen (fn);
}

/** ((LOAD")) ( -- ? ) [HIDDEN]
 */
FCode_XE (p4_load_quote_execution)
{   FX_USE_CODE_ADDR {
#  if !defined PFE_SBR_CALL_THREADING
    register p4_char_t *p = (p4_char_t *) IP;
    register int n = *p++;

    FX_SKIP_STRING;
    p4_load_file (p, n, *SP++);
#  else
    register p4_char_t *p;
    FX_NEW_IP_WORK;
    p = FX_NEW_IP_CHAR;
    FX_NEW_IP_SKIP_STRING;
    FX_NEW_IP_DONE;
    p4_load_file (p+1, *p, *SP++);
#  endif
    FX_USE_CODE_EXIT;
}}

/** LOAD"  ( [filename<">] -- ??? ) [FTH] [OLD]
 * load the specified file - this word can be compiled into a word-definition
 * obsolete! use => OPEN-BLOCKFILE name => LOAD
 */
FCode (p4_load_quote)
{
    if (STATE)
    {
        FX_COMPILE (p4_load_quote);
        FX (p4_parse_comma_quote);
    }else{
        p4_skip_delimiter (' ');
        p4_word_parse ('"'); *DP=0; /* PARSE-NOHERE (actually PARSE-WORD) */
        p4_load_file (PFE.word.ptr, PFE.word.len, *SP++); /* uses p4_pocket */
    }
}
P4COMPILES (p4_load_quote, p4_load_quote_execution,
  P4_SKIPS_STRING, P4_DEFAULT_STYLE);

#ifndef NO_SYSTEM
/** SYSTEM ( command-ptr command-len -- command-exitcode# ) [FTH]
 * run a shell command  (note: embedded systems have no shell)
 */
FCode (p4_system)
{
    SP[1] = p4_systemf ("%.*s", (int) SP[0], (char *) SP[1]);
    SP++;
}

/** ((SYSTEM")) ( ... -- exitcode# ) [HIDDEN]
 * compiled by => SYSTEM" commandline"
 */
FCode_XE (p4_system_quote_execution)
{   FX_USE_CODE_ADDR {
#  if !defined PFE_SBR_CALL_THREADING
    char *p = (char *) IP;

    FX_SKIP_STRING;
    *--SP = p4_systemf ("%.*s", *p, p + 1);
#  else
    char *p;
    FX_NEW_IP_WORK;
    p = FX_NEW_IP_CHAR;
    FX_NEW_IP_SKIP_STRING;
    FX_NEW_IP_DONE;
    *--SP = p4_systemf ("%.*s", *p, p + 1);
#  endif
    FX_USE_CODE_EXIT;
}}

/** SYSTEM" ( [command-line<">] -- command-exitcode# ) [FTH] [OLD]
 * run a shell command (note:embedded systems have no shell)
 * obsolete! use => S" string" => SYSTEM
 */
FCode (p4_system_quote)
{
    if (STATE)
    {
        FX_COMPILE (p4_system_quote);
        FX (p4_parse_comma_quote);
    }else{
        p4_word_parse ('"'); *DP=0; /* PARSE-NOHERE */
        *--SP = p4_systemf ("%.*s", PFE.word.len, PFE.word.ptr);
    }
}
P4COMPILES (p4_system_quote, p4_system_quote_execution,
  P4_SKIPS_STRING, P4_DEFAULT_STYLE);
#endif /* NO_SYSTEM */

/** OK ( -- )
 * it usually prints "ok" 
 */
extern FCode (p4_ok); 

/* ------------------------------------------------------------------ */

/** CREATE: ( "name" -- ) [FTH]
 * this creates a name with the => VARIABLE runtime.
 * Note that this is the FIG-implemenation of => CREATE whereas in
 * ANS-Forth mode we have a => CREATE identical to FIG-style =>"<BUILDS"
 : CREATE: BL WORD $HEADER DOVAR A, ;
 */
FCode (p4_create_var)
{
    FX_RUNTIME_HEADER;
    FX_RUNTIME1 (p4_variable);
}

/** BUFFER: ( size# "name" -- ) [FTH]
 * this creates a name with the => VARIABLE runtime and =>"ALLOT"s memory
 : BUFFER: BL WORD $HEADER DOVAR A, ALLOT ;
 */
FCode (p4_buffer_var)
{
    FX_RUNTIME_HEADER;
    FX_RUNTIME1 (p4_variable);
    FX (p4_allot);
}

/** R'@ ( R: a b -- a R: a b ) [FTH]
 * fetch the next-under value from the returnstack.
 * used to interpret the returnstack to hold two => LOCALS| values.
 * ( =>'R@' / =>'2R@' / =>'R>DROP' / =>'R"@')
 */
FCode (p4_r_tick_fetch)
{
    FX_COMPILE (p4_r_tick_fetch);
}
FCode_XE (p4_r_tick_fetch_execution)
{
    FX_USE_CODE_ADDR;
    FX_PUSH (FX_RP[1]);
    FX_USE_CODE_EXIT;
}
P4COMPILES (p4_r_tick_fetch, p4_r_tick_fetch_execution,
	    P4_SKIPS_NOTHING, P4_DEFAULT_STYLE);

/** R'! ( x R: a b -- R: x b ) [FTH]
 * store the value into the next-under value in the returnstack.
 * used to interpret the returnstack to hold two => LOCALS| values.
 * see =>"R'@" for inverse operation
 */
FCode (p4_r_tick_store)
{
    FX_COMPILE (p4_r_tick_store);
}
FCode_XE (p4_r_tick_store_execution)
{
    FX_USE_CODE_ADDR;
    FX_RP[1] = FX_POP;
    FX_USE_CODE_EXIT;
}
P4COMPILES (p4_r_tick_store, p4_r_tick_store_execution,
	    P4_SKIPS_NOTHING, P4_DEFAULT_STYLE);

/** R"@ ( R: a b c -- a R: a b c ) [FTH]
 * fetch the second-under value from the returnstack.
 * used to interpret the returnstack to hold three => LOCALS| values.
 * see =>'R"!' for inverse operation ( =>"R'@" =>"R@" / =>"2R@" / =>"R>DROP" )
 */
FCode (p4_r_quote_fetch)
{
    FX_COMPILE (p4_r_quote_fetch);
}
FCode_XE (p4_r_quote_fetch_execution)
{
    FX_USE_CODE_ADDR;
    FX_PUSH (FX_RP[2]);
    FX_USE_CODE_EXIT;
}
P4COMPILES (p4_r_quote_fetch, p4_r_quote_fetch_execution,
	    P4_SKIPS_NOTHING, P4_DEFAULT_STYLE);

/** R"! ( x R: a b c -- R: x b c ) [FTH]
 * store the value into the second-under value in the returnstack.
 * used to interpret the returnstack to hold three => LOCALS| values.
 * see =>'R"@' for inverse operation
 */
FCode (p4_r_quote_store)
{
    FX_COMPILE (p4_r_quote_store);
}
FCode_XE (p4_r_quote_store_execution)
{
    FX_USE_CODE_ADDR;
    FX_RP[2] = FX_POP;
    FX_USE_CODE_EXIT;
}
P4COMPILES (p4_r_quote_store, p4_r_quote_store_execution,
	    P4_SKIPS_NOTHING, P4_DEFAULT_STYLE);

/** R! ( x R: a -- R: x ) [FTH]
 * store the value as the topmost value in the returnstack.
 * see =>"R@" for inverse operation ( =>"R'@" / =>'R"@' / =>'2R@' / =>'2R!')
 */
FCode (p4_r_store)
{
    FX_COMPILE (p4_r_store);
}
FCode_XE (p4_r_store_execution)
{
    FX_USE_CODE_ADDR;
    FX_RP[0] = FX_POP;
    FX_USE_CODE_EXIT;
}
P4COMPILES (p4_r_store, p4_r_store_execution,
	    P4_SKIPS_NOTHING, P4_DEFAULT_STYLE);

/** 2R! ( x y R: a b -- R: x y ) [FTH]
 * store the value as the topmost value in the returnstack.
 * see =>"2R@" for inverse operation ( =>"R'@" / =>'R"@' / =>'2R@' / =>'2R!')
 */
FCode (p4_two_r_store)
{
    FX_COMPILE (p4_two_r_store);
}
FCode_XE (p4_two_r_store_execution)
{
    FX_USE_CODE_ADDR;
    FX_RP[0] = SP[0];
    FX_RP[1] = SP[1];
    FX_2DROP;
    FX_USE_CODE_EXIT;
}
P4COMPILES (p4_two_r_store, p4_two_r_store_execution,
	    P4_SKIPS_NOTHING, P4_DEFAULT_STYLE);

/** DUP>R ( val -- val R: val ) [FTH]
 * shortcut, see => R>DROP
 * <br> note again that the following will fail:
 : DUP>R DUP >R ;
 */
FCode (p4_dup_to_r)
{
    FX_COMPILE (p4_dup_to_r);
    FX_COMPILE_RP_ROOM (1);
}
FCode_XE (p4_dup_to_r_execution)
{
    FX_USE_CODE_ADDR;
#  ifndef PFE_SBR_CALL_THREADING
    RP_PUSH (*SP);
#  else
    FX_EXECUTE_RP_ROOM (1);
    RP[0] = (p4xcode *)( *SP );
#  endif
    FX_USE_CODE_EXIT;
}
P4COMPILES (p4_dup_to_r, p4_dup_to_r_execution,
	    P4_SKIPS_NOTHING, P4_DEFAULT_STYLE);

/** R>DROP ( R: val -- R: ) [FTH]
 * shortcut (e.g. in CSI-Forth)
 * <br> note that the access to R is configuration dependent - only in
 * a traditional fig-forth each NEST will be one cell wide - in case that
 * there are no => LOCALS| of course. And remember, the word above reads
 * like the sequence => R> and => DROP but that is not quite true.
 : R>DROP R> DROP ; ( is bad - correct might be )  : R>DROP R> R> DROP >R ;
 */
FCode (p4_r_from_drop)
{
    FX_COMPILE (p4_r_from_drop);
    FX_COMPILE_RP_DROP (1);
}
FCode_XE (p4_r_from_drop_execution)
{
    FX_USE_CODE_ADDR;
    FX_EXECUTE_RP_DROP (1);
    FX_USE_CODE_EXIT;
}
P4COMPILES (p4_r_from_drop, p4_r_from_drop_execution,
	    P4_SKIPS_NOTHING, P4_DEFAULT_STYLE);

/** 2R>2DROP ( R: a b -- R: ) [FTH]
 * this is two times => R>DROP but a bit quicker.
 * it is however really quick compared to the sequence => 2R> and => 2DROP
 */
FCode (p4_two_r_from_drop)
{
    FX_COMPILE (p4_two_r_from_drop);
    FX_COMPILE_RP_DROP (2);
}
FCode_XE (p4_two_r_from_drop_execution)
{
    FX_USE_CODE_ADDR;
    FX_EXECUTE_RP_DROP (2);
    FX_USE_CODE_EXIT;
}
P4COMPILES (p4_two_r_from_drop, p4_two_r_from_drop_execution,
	    P4_SKIPS_NOTHING, P4_DEFAULT_STYLE);

/** CLEARSTACK ( -- ) [FTH]
 * reset the parameter stack to be empty
 : CLEARSTACK  S0 SP! ;
 */
FCode (p4_clearstack)
{
    p4SP = PFE.s0;
}

/** LOWER-CASE ( -- uppercase-var* ) [OLD]
 * Call to enable lower-case symbols as input
 * Replace code of "LOWER-CASE OFF" with "NO TO UPPER-CASE?"
 * see => UPPER-CASE?
 */

/** UPPER-CASE? ( -- uppercase-flag )
 * Call to check whether lower-case symbols in input can match 
 * words defined in uppercase. Actually it sets the internal
 * wordl-flag which has some bit-defs used when creating new
 * vocabularies.
 *
 * This flag => VALUE is modifiable with => TO
   YES TO UPPER-CASE?
 */

/** REDEFINED-MSG ( -- redefined-var* ) [OLD]
 * Call to enable warnings if creating symbols being in the => ORDER 
 * Replace code of "REDEFINED-MSG OFF" with "NO TO REDEFINED-MSG?"
 * see => REDEFINED-MSG?
 */

/** REDEFINED-MSG? ( -- redefined-flag ) 
 * Call to check whether the system will emit warnings if creating 
 * symbols being already defined in the => CURRENT vocabulary.
 *
 * This flag => VALUE is modifiable with => TO
   YES TO REDEFINED-MSG?
 */

/** QUOTED-PARSE ( -- quotedparse-var* ) [OLD]
 * Call to enable quoted-parse extension in =>"PARSE" =>"WORD"s 
 * Replace code of "QUOTED-PARSE OFF" with "NO TO QUOTED-PARSE?"
 * see => QUOTED-PARSE?
 */

/** QUOTED-PARSE? ( -- quotedparse-flag ) 
 * Call to check for quoted-parse extension in =>"PARSE" =>"WORD"s 
 *
 * This flag => VALUE is modifiable with => TO
   YES TO QUOTED-PARSE?
 */

/** RAND_MAX ( -- rand-max ) [ENVIRONMENT]
 * maximum value for => RANDOM
 */

P4_LISTWORDS (misc) =
{
    P4_INTO ("FORTH", 0),
    /** just print OK, also fine for copy-and-paste in terminal */
    P4_FXco ("ok",		p4_ok),

    /* more fig-forth */
    P4_FXco ("COLD",		p4_cold),
    P4_FXco ("LIT",		p4_literal_execution),
    P4_FXco (".LINE",		p4_dot_line),

    /** basic system variables ( => OUT => DP => HLD => R0 => S0 ) */
    P4_DVaR ("OUT",		out),
    P4_DVaR ("DP",		dp),
    P4_DVaR ("HLD",		hld),
    P4_DVaR ("R0",		r0),
    P4_DVaR ("S0",		s0),

    /* words without pedigree */
    P4_FXco ("UD.R",		p4_u_d_dot_r),
    P4_FXco ("UD.",		p4_u_d_dot),
    P4_FXco ("ID.",	        p4_id_dot),
    P4_FNYM (".NAME",		"ID."),
    P4_FXco ("-ROLL",		p4_dash_roll),
    P4_FXco ("RANDOM",		p4_random),
    P4_FXco ("SRAND",		p4_srand),
    P4_FXco ("(UNDER+)",	p4_under_plus),
    P4_xOLD ("UNDER+",		"(UNDER+)"),

    /* more local variables */
    P4_SXco ("+TO",		p4_plus_to),

    /* data structures */
    P4_FXco ("BUILD-ARRAY",	p4_build_array),
    P4_FXco ("ACCESS-ARRAY",	p4_access_array),

    P4_FXco (".STATUS",		p4_dot_status),
    P4_xOLD ("SHOW-STATUS",	".STATUS"),

    /* NOTE: the newer "TO"-implementation is able to set DVaLs */
    P4_DVaL ("UPPER-CASE?",	wordl_flag), /* will be bool-VaL */
    P4_DVaR ("LOWER-CASE",	wordl_flag), /* fixme: deleted somewhen */
    P4_DVaL ("LOWER-CASE-FN?",	lower_case_fn), /* will be bool-VaL */
    P4_DVaR ("LOWER-CASE-FN",	lower_case_fn), /* fixme: delete somewhen */
    P4_DVaL ("REDEFINED-MSG?",	redefined_msg), /* will be bool-VaL */
    P4_DVaR ("REDEFINED-MSG",	redefined_msg), /* fixme: delete somewhen */
    P4_DVaL ("QUOTED-PARSE?",	quoted_parse),  /* will be bool-VaL */
  
    P4_FXco ("SOURCE-LINE",	p4_source_line),
    P4_FXco ("SOURCE-NAME",	p4_source_name),
    P4_FXco ("TH'POCKET",	p4_th_pocket),
    P4_FXco ("POCKET-PAD",	p4_pocket_pad),
    P4_OCoN ("/CELL",		sizeof (p4cell)),
    P4_xOLD ("WSIZE",		"/CELL"),
    P4_FXco ("W@",		p4_w_fetch),
    P4_FXco ("W!",		p4_w_store),
    P4_FXco ("W+!",		p4_w_plus_store),
    P4_FXco ("WL-HASH",		p4_wl_hash),
    P4_FXco ("TOPMOST",		p4_topmost),
     /** the variable accessed with => LATEST */

    P4_FXco ("LS.WORDS",	p4_ls_words),
    P4_FXco ("LS.PRIMITIVES",	p4_ls_primitives),
    P4_FXco ("LS.COLON-DEFS",	p4_ls_cdefs),
    P4_FXco ("LS.DOES-DEFS",	p4_ls_ddefs),
    P4_FXco ("LS.CONSTANTS",	p4_ls_constants),
    P4_FXco ("LS.VARIABLES",	p4_ls_variables),
    P4_FXco ("LS.VOCABULARIES",	p4_ls_vocabularies),
    P4_FXco ("LS.MARKERS",	p4_ls_markers),

    P4_FXco ("TAB",		p4_tab),
    P4_FXco ("BACKSPACE",	p4_backspace),
    P4_FXco ("?STOP",		p4_Q_stop),
    P4_FXco ("START?CR",	p4_start_Q_cr),
    P4_FXco ("?CR",		p4_Q_cr),
    P4_FXco ("CLOSE-ALL-FILES", p4_close_all_files),
    P4_FXco (".MEMORY",		p4_dot_memory),
    
    /** vectorized i/o variables, see => STANDARD-I/O */
    P4_DVaR ("*EMIT*",		emit),
    P4_DVaR ("*EXPECT*",	expect),
    P4_DVaR ("*KEY*",		key),
    P4_DVaR ("*TYPE*",		type),
    P4_FXco ("(EMIT)",		p4_paren_emit),
    P4_FXco ("(EXPECT)",	p4_paren_expect),
    P4_FXco ("(KEY)",		p4_paren_key),
    P4_FXco ("(TYPE)",		p4_paren_type),
    P4_FXco ("STANDARD-I/O",	p4_standard_io),

    /* show online help: */
    P4_FXco ("HELP",		p4_help),
    P4_FXco ("EDIT-BLOCKFILE",	p4_edit_blockfile),
  /** the application to be called, options like => ARGC => ARGV */
    P4_DVaR ("APPLICATION",	application),

    /** task system hooks */
    P4_FXco ("ARGC",		p4_argc),
    P4_FXco ("ARGV",		p4_argv),
    P4_DVaR ("EXITCODE",	exitcode),
     /** ( -- fid ) - the standard file-handles of the task */
    P4_DVaL ("STDIN",		stdIn),	
    P4_DVaL ("STDOUT",		stdOut),	
    P4_DVaL ("STDERR",		stdErr),	

    P4_FXco ("EXPAND-FN",	p4_expand_fn),
    P4_SXco ("LOAD\"",		p4_load_quote),
#ifndef NO_SYSTEM
    P4_FXco ("SYSTEM",		p4_system),
    P4_SXco ("SYSTEM\"",	p4_system_quote),
#endif /* NO_SYSTEM */

/* almost usable everywhere */
    P4_FXco ("CREATE:",		p4_create_var),
    P4_FXco ("BUFFER:",		p4_buffer_var),

/* quite useful, esp. for non-locals code */
    P4_SXco ("R'@",		p4_r_tick_fetch),
    P4_SXco ("R'!",		p4_r_tick_store),
    P4_SXco ("R\"@",		p4_r_quote_fetch),
    P4_SXco ("R\"!",		p4_r_quote_store),
    P4_SXco ("R!",		p4_r_store),
    P4_SXco ("2R!",		p4_two_r_store),
    P4_SXco ("DUP>R",		p4_dup_to_r),
    P4_SXco ("R>DROP",		p4_r_from_drop),
    P4_SXco ("2R>2DROP",	p4_two_r_from_drop),
    P4_FXco ("CLEARSTACK",	p4_clearstack),

    P4_INTO ("EXTENSIONS",      "FORTH"),
    P4_FXco ("+UNDER",          p4_plus_under),

/* smart */
    P4_FXco ("EXECUTES",	p4_executes),

    P4_INTO ("ENVIRONMENT", 0 ),
    P4_OCoN ("RAND_MAX",	RAND_MAX),
};
P4_COUNTWORDS (misc, "Compatibility Miscellaneous words");

/*@}*/
/* 
 * Local variables:
 * c-file-style: "stroustrup"
 * End:
 */




