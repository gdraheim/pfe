/** 
 * -- miscellaneous useful words, mostly stemming from fig-forth
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 5.16 %
 *    (%date_modified: Mon Mar 12 10:32:33 2001 %)
 *
 *  @description
 *      Compatiblity with former standards, miscellaneous useful words.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: misc-ext.c,v 0.33 2001-05-12 18:15:46 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-xtra.h>

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#include <pfe/def-comp.h>
#include <pfe/term-sub.h>
#include <pfe/file-sub.h>
#include <pfe/double-sub.h>
#include <pfe/block-ext.h>
#include <pfe/exception-sub.h>
#include <pfe/version-sub.h>

#include <pfe/_missing.h>

/** COLD ( -- )
 * cold abort - reinitialize everything and go to => QUIT routine
 * ... this routine is implemented as a warm-boot in pfe.
 : WARM FENCE @ (FORGET) INCLUDE-FILE ?DUP IF COUNT INCLUDED THEN QUIT ;
 */
FCode (p4_cold)
{
    FX (p4_close_all_files);

#if 0
    PFE.atexit_running = 1;
    p4_forget (PFE.dict);
    PFE.atexit_running = 0;

    p4_boot_system (); 
#else
    PFE.atexit_running = 1;
    p4_forget (FENCE);
    PFE.atexit_running = 0;
#endif

    FX (p4_paren_abort);

    if (PFE_set.include_file)
    {
        p4_included1 (PFE_set.include_file, strlen (PFE_set.include_file), 0);
    }

    /* If it's a turnkey-application, start it: */
    if (APPLICATION)
    {
        p4_run_forth (APPLICATION);
        p4_longjmp_exit (); 
    }
    if (P4_opt.verbose)
        FX (p4_dot_memory);
    /* p4_longjmp_abort (); -> paren_abort + yield */
    p4_longjmp_yield ();
}

/** .LINE ( line# block# -- )
 */
FCode (p4_dot_line)			
{
    p4_dot_line (BLOCK_FILE, SP[0], SP[1]);
    SP += 2;
}

/** !CSP ( -- )
 * put => SP into => CSP
 * <br> used in control-words
 */
FCode (p4_store_csp)
{
    CSP = SP;
}

/** ?CSP ( -- )
 * check that => SP == => CSP otherwise => THROW
 * <br> used in control-words
 */
FCode (p4_Q_csp)	
{
    if (SP != CSP)
        p4_throw (P4_ON_CONTROL_MISMATCH);
}

/** ?COMP ( -- )
 * check that the current => STATE is compiling
 * otherwise => THROW
 * <br> often used in control-words
 */
FCode (p4_Q_comp)		
{
    if (!STATE)
        p4_throw (P4_ON_COMPILE_ONLY);
}

/** ?EXEC ( -- )
 * check that the current => STATE is executing
 * otherwise => THROW
 * <br> often used in control-words
 */
FCode (p4_Q_exec)		
{
    if (STATE)
        p4_throw (P4_ON_COMPILER_NESTING);
}

/** ?FILE ( file-id -- )
 * check the file-id otherwise (fixme)
 */
FCode (p4_Q_file)
{
    int ior = *SP++;

    if (ior)
        p4_throw (FX_IOR);
}

/** ?LOADING ( -- )
 * check that the currently interpreted text is 
 * from a file/block, otherwise => THROW
 */
FCode (p4_Q_loading)
{
    if (BLK == 0)
        p4_throw (P4_ON_INVALID_BLOCK);
}

/** ?PAIRS ( a b -- )
 * if compiling, check that the two magics on
 * the => CS-STACK are identical, otherwise throw
 * <br> used in control-words
 */
FCode (p4_Q_pairs)
{
    FX (p4_Q_comp);
    p4_Q_pairs (*SP++);
}

/** ?STACK ( -- )
 * check all stacks for underflow and overflow conditions,
 * and if such an error condition is detected => THROW
 */
FCode (p4_Q_stack)
{
    if (RP > PFE.r0)	    p4_throw (P4_ON_RSTACK_UNDER);
    if (RP < PFE.rstack)    p4_throw (P4_ON_RSTACK_OVER);
    if (SP > PFE.s0)	    p4_throw (P4_ON_STACK_UNDER);
    if (SP < PFE.stack)	    p4_throw (P4_ON_STACK_OVER);
#ifndef P4_NO_FP
    if (FP > PFE.f0)	    p4_throw (P4_ON_FSTACK_UNDER);
    if (FP < PFE.fstack)    p4_throw (P4_ON_FSTACK_OVER);
#endif
    if (PFE.dictlimit-256 < PFE.dp) p4_throw (P4_ON_DICT_OVER);  
}

/** TOGGLE ( c-addr charmask -- ) 
 * toggle the bits given in charmask, see also => SMUDGE and = UNSMUDGE
 example: the fig-style SMUDGE had been defined such
   : FIG-SMUDGE LATEST >FFA (SMUDGE#) TOGGLE ;
 */
FCode (p4_toggle)			
{
    *(p4char *) SP[1] ^= (p4char) SP[0];
    SP += 2;
}

/** LATEST ( -- nfa )
 * return the NFA of the lateset definition in the
 * => CURRENT vocabulary
 */
FCode (p4_latest)			
{
    *--SP = (p4cell) p4_latest ();
}

/** SMUDGE (modified from FIG definition) 
 *   FIG definition toggles the bit! 
 */
FCode (p4_smudge)
{
    if (LAST)
        *_FFA(LAST) |= P4xSMUDGED;
    else
        p4_throw (P4_ON_ARG_TYPE);
}

/** UNSMUDGE (turn smudge-bit off) 
 *   neccessary because => SMUDGE modified 
 */
FCode (p4_unsmudge)
{
    if (LAST)
        *_FFA(LAST) &= ~P4xSMUDGED;
    else
        p4_throw (P4_ON_ARG_TYPE);
}

/************************************************************************/
/* some well known words without pedigree                               */
/************************************************************************/

/** UD.R ( 2val r -- )
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

/** UD. ( 2val -- )
 * see also => UD.R
 */
FCode (p4_u_d_dot)
{
    *--SP = 0;
    FX (p4_u_d_dot_r);
    FX (p4_space);
}

/** .NAME ( nfa -- )
 * print the name-field which is effectivly a bstring - in
 * some forth implementations the name-field is identical 
 * with the flag-field, so the nfa's count has to be masked
 * the, e.g. 
 : .NAME COUNT 32 AND TYPE ;
 * if the name-field and flag-field are different, then this
 * word does effectivly 
 : .NAME COUNT TYPE ;
 */
FCode (p4_dot_name)
{
    p4_dot_name ((char *) *SP++);
}

/** ID. ( nfa -- )
 * print the name-field pointed to by the nfa-argument.
 * a synonym for .NAME that is more portable due its
 * heritage from fig-forth.
 */
FCode(p4_id_dot)
{
    /* Anton Ertl (gforth), Tom Zimmer (win32for), Guido Draheim (pfe)
     * agreed to export ID. as a portable function since win32for's
     * .NAME did not match the gforth' and pfe' variant of the same
     * name. The ID. has a heritage dating back to fig-forth, and was
     * present in tek'mforth too.
     */
    p4_dot_name ((char *) *SP++);
}

/** -ROLL ( xn ... x2 x1 n -- x1 xn ... x2 )
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

/** R>DROP ( -- )
 * shortcut (e.g. in CSI-Forth)
 * <br> note that
 : R>DROP R> DROP ; 
 * would be wrong - for a non-direct-threaded forth it would be
 : R>DROP R> R> NIP >R ;
 */
FCode (p4_r_from_drop)
{
    RP++;
}

/** DUP>R ( val -- val )
 * shortcut, see => R>DROP
 * <br> note again that the following will fail:
 : DUP>R DUP >R ;
 */
FCode (p4_dup_to_r)
{
    RP_PUSH (*SP);
}

/* some systems (BSD) have a better random number generator than
   standard unix' rand() 
*/
#if defined HAVE_RANDOM
# define _rand_ random
#else
# define _rand_ rand
#endif

/** RANDOM ( n1 -- n2 )
 * returns random number with 0 <= n2 < n1)
 */
FCode (p4_random)			
{				
    if (*SP == 0)
        *SP = _rand_ ();
    else
        *SP = p4_d_ummul (*SP, _rand_ () << 1).hi;
}

#undef rand

/** SRAND ( n -- )
 */
FCode (p4_srand)			
{
#if defined HAVE_RANDOM
    srandom (*SP++);
#else
    srand (*SP++);
#endif
}
/** UNDER+ ( n1 n2 -- n1+n2 n2 )
 * quicker than
 : UNDER+ TUCK + SWAP ;
 */
FCode (p4_under_plus)
{		
    SP[1] += SP[0];
}

/************************************************************************/
/* more local variables                                                 */
/************************************************************************/

/** ((+TO)) ( val -- )
 * execution compiled by => +TO
 * adds the stack-val to the lvalue compiled
 */
FCode (p4_plus_to_execution)	
{			
    *TO_BODY (*IP++) += *SP++;
}

/** ((+TO.local)) ( val -- )
 * same as => ((+TO)) when the lvalue is a => LOCALS| value
 * <br> compiled by => +TO
 */
FCode (p4_plus_to_local_execution)
{
    LP[(p4cell) *IP++] += *SP++;
}

/** +TO ( val [name] -- )
 * add the val to the named => VALUE or => LOCALS| value
 */
FCode (p4_plus_to)
{
    if (STATE)
    {
        char *p;
        int l, n;

        p = p4_word (' ');
        l = *(p4char *) p++;
        if (PFE.locals && (n = p4_find_local (p, l)) != 0)
	{
            FX_COMPILE2 (p4_plus_to);
            FX_COMMA (n);
	}else{
            if ((p = p4_find (p, l)) == NULL)
                p4_throw (P4_ON_UNDEFINED);
            FX_COMPILE1 (p4_plus_to);
            FX_COMMA (p4_name_from (p));
	}
    }else{
        p4xt xt;

        p4_tick (&xt);
        *TO_BODY (xt) += *SP++;
    }
}
P4COMPILES2 (p4_plus_to, p4_plus_to_execution, p4_plus_to_local_execution,
  P4_SKIPS_TO_TOKEN, P4_DEFAULT_STYLE);

/************************************************************************/
/* data structures                                                      */
/************************************************************************/

/** BUILD-ARRAY ( n1 n2 ... nX X --- n )
 * writes X, n1, ... nX into the dictionary - 
 * returns product n1 * n2 * ... * nX 
 */
FCode (p4_build_array)		
{				
    p4cell i = *SP++;		
    p4ucell n = 1;

    FX_COMMA (i);
    while (--i >= 0)
    {
        FX_COMMA (*SP);
        n *= *SP++;
    }
    *--SP = n;
}
/** ACCESS-ARRAY ( i1 i2 ... iX addr1 --- addr2 n )
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
/* more comparision operators                                           */
/************************************************************************/

/** 0<= ( a -- flag )
 simulate    : 0<= 0> 0= ;
 */
FCode (p4_zero_less_equal)
{
    *SP = P4_FLAG (*SP <= 0);
}

/** 0>= ( a -- flag )
 simulate    : 0>= 0< 0= ;
 */
FCode (p4_zero_greater_equal)
{
    *SP = P4_FLAG (*SP >= 0);
}

/** <= ( a b -- flag )
 simulate    : <= > 0= ;
 */
FCode (p4_less_equal)
{
    SP[1] = P4_FLAG (SP[1] <= SP[0]);
    SP++;
}

/** >= ( a b -- flag )
 simulate    : >= < 0= ;
 */
FCode (p4_greater_equal)
{
    SP[1] = P4_FLAG (SP[1] >= SP[0]);
    SP++;
}

/** U<= ( a b -- flag )
 simulate    : U<= U> 0= ;
 */
FCode (p4_u_less_equal)
{
    SP[1] = P4_FLAG ((p4ucell) SP[1] <= (p4ucell) SP[0]);
    SP++;
}

/** U>= ( a b -- flag )
 simulate    : U>= U< 0= ;
 */
FCode (p4_u_greater_equal)
{
    SP[1] = P4_FLAG ((p4ucell) SP[1] >= (p4ucell) SP[0]);
    SP++;
}

/** UMAX ( a b -- max )
 * see => MAX
 */
FCode (p4_u_max)
{
    if ((p4ucell) SP[0] > (p4ucell) SP[1])
        SP[1] = SP[0];
    SP++;
}

/** UMIN ( a b -- min )
 * see => MIN , => MAX and => UMAX
 */
FCode (p4_u_min)
{
    if ((p4ucell) SP[0] < (p4ucell) SP[1])
        SP[1] = SP[0];
    SP++;
}

/************************************************************************/
/* implementation                                                       */
/************************************************************************/

/** SOURCE-LINE ( -- n )
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

/** TH'POCKET ( n -- addr u )
 * returns the specified pocket as a => S" string reference
 */
FCode (p4_th_pocket)			
{			
    int n = *SP;

    SP -= 1;
    SP[1] = (p4cell) PFE.pockets[n] + 1;
    SP[0] = *(p4char *) PFE.pockets[n];
}

/** POCKET-PAD ( -- addr )
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

/** WL-HASH ( c-addr n1 -- n2 )
 * calc hash-code for selection of thread
 * in a threaded-vocabulary
 */
FCode (p4_wl_hash)	
{			
    SP[1] = p4_wl_hash ((char *) SP[1], SP[0]);
    SP++;
}

/** TOPMOST ( wid -- a-addr )
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
    char *pattern = p4_word (' ');

    if (*pattern == 0)
        strcpy (pattern, "\001*");
    p4_outf ("\nWords matching %s:", pattern + 1);
    p4_wild_words (wl, pattern + 1, cat);
}

/** LS.WORDS ( -- )
 * see => WORDS
 */
FCode (p4_ls_words)		{ ls_words (NULL); }

/** LS.PRIMITIVES ( -- )
 * see => WORDS
 */
FCode (p4_ls_primitives)	{ ls_words ("p"); }

/** LS.COLON-DEFS ( -- )
 * see => WORDS
 */
FCode (p4_ls_cdefs)		{ ls_words (":"); }

/** LS.DOES-DEFS ( -- )
 * see => WORDS
 */
FCode (p4_ls_ddefs)		{ ls_words ("D"); }

/** LS.CONSTANTS ( -- )
 * see => WORDS
 */
FCode (p4_ls_constants)		{ ls_words ("cC"); }

/** LS.VARIABLES ( -- )
 * see => WORDS
 */
FCode (p4_ls_variables)		{ ls_words ("vV"); }

/** LS.VOCABULARIES ( -- )
 * see => WORDS
 */
FCode (p4_ls_vocabularies)	{ ls_words ("W"); }

/** LS.MARKERS ( -- )
 * see => WORDS
 */
FCode (p4_ls_markers)		{ ls_words ("M"); }

/* ............... */

/** W@ ( addr -- w-val )
 * fetch a 2byte-val from address
 */
FCode (p4_w_fetch)
{
    *SP = *(short *) *SP;
}

/** W! ( w-val addr -- )
 * store a 2byte-val at addressed 2byte-value
 */
FCode (p4_w_store)			
{
    *(short *) SP[0] = (short) SP[1];
    SP += 2;
}

/** W+! ( w-val addr -- )
 * add a 2byte-val to addressed 2byte-value
 */
FCode (p4_w_plus_store)	
{
    *(short *) SP[0] += (short) SP[1];
    SP += 2;
}

/** (FORGET) ( addr -- )
 * forget everything above addr
 * - used by => FORGET
 */
FCode (p4_paren_forget)		
{			
    p4_forget ((char *) *SP++);
}

/** TAB ( n -- )
 * jump to next column divisible by n 
 */
FCode (p4_tab)			
{			
    p4_tab (*SP++);
}

/** BACKSPACE ( -- )
 * reverse of => SPACE
 */
FCode (p4_backspace)
{
    p4_outs ("\b \b");
}

/** ?STOP ( -- flag )
 * check for 'q' pressed
 * - see => ?CR
 */
FCode (p4_Q_stop)
{
    *--SP = P4_FLAG (p4_Q_stop ());
}

/** START?CR ( -- )
 * initialized for more-like effect
 * - see => ?CR
 */
FCode (p4_start_Q_cr)	
{				
    PFE.more = PFE.rows - 2;
    PFE.lines = 0;
}

/** ?CR ( -- flag )
 * like => CR , stop 25 lines past => START?CR
 */
FCode (p4_Q_cr)	
{		
    *--SP = p4_Q_cr ();
}

/** CLOSE-ALL-FILES ( -- )
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

/** .MEMORY ( -- )
 */
FCode (p4_dot_memory)
{
    p4_outf ("\nDictionary space:    %7ld Bytes, in use: %7ld Bytes\n"
	"Stack space:         %7ld cells\n"
	"Floating stack space:%7ld floats\n"
	"Return stack space:  %7ld cells\n",
           	/* the C language returns n as n*sizeof==bytes */
	(long) (PFE.dictlimit - PFE.dict),
	(long) (PFE.dp - PFE.dict), 
	(long) (PFE.s0 - PFE.stack),  /* sizeof (p4cell) */
	(long) (PFE.f0 - PFE.fstack), /* sizeof (double) */
	(long) (PFE.r0 - PFE.rstack));  /* sizeof (p4xt**) */
}

/** .VERSION ( -- )
 */
FCode (p4_dot_version)
{
    p4_outs (p4_version_string ());
}

/** .PFE-DATE ( -- )
 */
FCode (p4_dot_date)
{
    p4_outf ("PFE compiled %s, %s ",
	p4_compile_date (), p4_compile_time ());
}

/** LICENSE ( -- )
 */
FCode (p4_license)
{
    p4_outs (p4_license_string ());
}

/** WARRANTY ( -- )
 */
FCode (p4_warranty)
{
    p4_outs (p4_warranty_string ());
}

/** SHOW-STATUS ( -- )
 * display internal variables 
 */
FCode (p4_show_status)
{
    FX (p4_cr);
    FX (p4_dot_version);
    FX (p4_cr);
    FX (p4_dot_date);
    FX (p4_cr);
    p4_outf ("\nMemory overview:");
    FX (p4_dot_memory);
    p4_outf ("\nsearch path for source files:         %s", P4_opt.incpaths);
    p4_outf ("\nextensions for source files:          %s", P4_opt.incext);
    p4_outf ("\nsearch path for block files:          %s", P4_opt.blkpaths);
    p4_outf ("\nextensions for block files:           %s", P4_opt.blkext);
    p4_outf ("\nsearching help files in:              %s", PFE_PKGHELPDIR);
    p4_outf ("\neditor called by EDIT-TEXT:           %s", P4_opt.editor);
    FX (p4_cr);
    p4_outf ("\nmaximum number of open files:         %u", P4_opt.max_files);
    p4_outf ("\nmaximum simultaneous interpretive S\"  %u", P4_opt.pockets);
    p4_outf ("\ndictionary threads configured         %u", 1<<LD_THREADS);
    p4_outf ("\nmaximum length of search order        %u", ORDER_LEN);
    FX (p4_cr);
    p4_outf ("\nText screen size:                     %dx%d", PFE.rows, PFE.cols);
    FX (p4_cr);
#define flag(X) ((X) ? "ON" : "OFF")
    p4_outf ("\nLOWER-CASE    %s", flag (LOWER_CASE));
    p4_outf ("\nLOWER-CASE-FN %s", flag (LOWER_CASE_FN));
    p4_outf ("\nRESET-ORDER   %s", flag (RESET_ORDER));
    p4_outf ("\nREDEFINED-MSG %s", flag (REDEFINED_MSG));
    p4_outf ("\nFLOAT-INPUT   %s", flag (FLOAT_INPUT));
#undef flag
    p4_outf ("\nPRECISION     %d", PRECISION);
    FX (p4_space);
}

/************************************************************************/
/* vectorized I/O                                                       */
/************************************************************************/

/** (EMIT) ( val -- )
 * like => EMIT and always to screen 
 * - the routine to be put into => *EMIT*
 */
FCode (p4_paren_emit)
{
    p4_outc ((char) *SP++);
}

/** (EXPECT) ( . -- . )
 * like => EXPECT and always from screen
 * - the routine to be put into => *EXPECT*
 */
FCode (p4_paren_expect)
{
    p4_expect ((char *) SP[1], SP[0]);
    SP += 2;
}

/** (KEY) ( -- ... )
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

/** (TYPE) ( val -- )
 * like => TYPE and always to screen 
 * - the routine to be put into => *TYPE*
 */
FCode (p4_paren_type)
{
    p4_type ((char *) SP[1], SP[0]);
    SP += 2;
}

/** STANDARD-I/O ( -- )
 * initialize => *TYPE* , => *EMIT* , => *EXPECT* and => *KEY*
 * to point directly to the screen I/O routines, <br> 
 * namely => (TYPE) , => (EMIT) , => (EXPECT) , => (KEY) 
 */
FCode (p4_standard_io)
{
    static p4code paren_emit_cfa = PFX (p4_paren_emit);
    static p4code paren_expect_cfa = PFX (p4_paren_expect);
    static p4code paren_key_cfa = PFX (p4_paren_key);
    static p4code paren_type_cfa = PFX (p4_paren_type);

    PFE.emit = &paren_emit_cfa;
    PFE.expect = &paren_expect_cfa;
    PFE.key = &paren_key_cfa;
    PFE.type = &paren_type_cfa;
}

/************************************************************************/
/* Function keys on the commandline                                     */
/************************************************************************/

void
accept_executes_xt (int n)
{
    if (PFE.fkey_xt[n])
        p4_call (PFE.fkey_xt[n]);
}

static void
p4_store_execution (p4xt xt, int key)
{
    if (key < P4_KEY_k1 || P4_KEY_k0 < key)
        p4_throw (P4_ON_ARG_TYPE);
    PFE.fkey_xt[key - P4_KEY_k1] = xt;
}

/** ((EXECUTES)) ( n -- )
 * compiled by => EXECUTES
 */
FCode (p4_executes_execution)
{
    p4_store_execution (*IP++, *SP++);
}

/** EXECUTES ( n [word] -- )
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
        p4xt xt;

        p4_tick (&xt);
        p4_store_execution (xt, *SP++);
    }
}
P4COMPILES (p4_executes, p4_executes_execution,
  P4_SKIPS_NOTHING, P4_DEFAULT_STYLE);

/************************************************************************/
/* display help                                                         */
/************************************************************************/

/** HELP ( -- )
 * non-functional at the moment.
 */
FCode (p4_help)
{
    char *p, buf[80];
    p4ucell n;

    p4_skip_delimiter (' ');
    p4_parse (' ', &p, &n);
    p4_store_c_string (p, n, buf, sizeof buf);
    if (LOWER_CASE)
        p4_upper (buf, n);
    FX (p4_cr);
    /*  print_help (buf); */
}

/************************************************************************/
/* more file manipulation                                               */
/************************************************************************/

/** COPY-FILE ( src-str src-strlen dst-str dst-strlen -- errno|0 )
 * like =>'RENAME-FILE', copies the file from src-name to dst-name
 * and returns an error-code or null
 */
FCode (p4_copy_file)
{
    char* src = p4_pocket_filename ((char *) SP[3], SP[2]);
    char* dst = p4_pocket_filename ((char *) SP[1], SP[0]);
    SP += 3;
    *SP = fn_copy (src, dst, LONG_MAX) ? errno : 0;
}

/** MOVE-FILE ( src-str src-strlen dst-str dst-strlen -- errno|0 )
 * like =>'RENAME-FILE', but also across-volumes <br>
 * moves the file from src-name to dst-name and returns an
 * error-code or null
 */
FCode (p4_move_file)		
{
    char* src = p4_pocket_filename ((char *) SP[3], SP[2]);
    char* dst = p4_pocket_filename ((char *) SP[1], SP[0]);
    SP += 3;
    *SP = fn_move (src, dst) ? errno : 0;
}

/** FILE-R/W ( addr blk f fid -- )
 * like FIG-Forth <c> R/W </c>
 */
FCode (p4_file_rw)			
{	
    p4_read_write (
                   (File *) SP[0],	/* file to read from */
                   (char *) SP[3],	/* buffer address, 1K */
                   (p4ucell) SP[2],	/* block number */
                   SP[0]);		/* readflag */
    SP += 4;
}

/** FILE-BLOCK ( a file-id -- c )
 */
FCode (p4_file_block)
{
    File *fid = (File *) *SP++;

    *SP = (p4cell) p4_block (fid, *SP);
}

/** FILE-BUFFER ( a file-id -- c )
 */
FCode (p4_file_buffer)
{
    File *fid = (File *) *SP++;
    int n;

    *SP = (p4cell) p4_buffer (fid, *SP, &n);
}

/** FILE-EMPTY-BUFFERS ( file-id -- )
 */
FCode (p4_file_empty_buffers)
{
    p4_empty_buffers ((File *) *SP++);
}

/** FILE-FLUSH ( file-id -- )
 simulate      : FILE-FLUSH DUP FILE-SAVE-BUFFERS FILE-EMTPY-BUFFERS ;
 */
FCode (p4_file_flush)
{
    File *fid = (File *) *SP++;

    p4_save_buffers (fid);
    p4_empty_buffers (fid);
}

/** FILE-LIST ( x file-id -- )
 */
FCode (p4_file_list)
{
    File *fid = (File *) *SP++;
    
    p4_list (fid, SCR = *SP++);
}

/** FILE-LOAD ( x file-id -- )
 */
FCode (p4_file_load)
{
    File *fid = (File *) *SP++;

    p4_load (fid, *SP++);
}

/** FILE-SAVE-BUFFERS ( file-id -- )
 */
FCode (p4_file_save_buffers)
{
    File *fid = (File *) *SP++;
    
    p4_save_buffers (fid);
}

/** FILE-THRU ( lo hi file-id -- )
 * see => THRU
 */
FCode (p4_file_thru)
{
    File *fid = (File *) *SP++;
    int hi = *SP++;
    int lo = *SP++;

    p4_thru (fid, lo, hi);
}

/** FILE-UPDATE ( file-id -- )
 */
FCode (p4_file_update)
{
    p4_update ((File *) *SP++);
}

/************************************************************************/
/* hooks to editors and os services                                     */
/************************************************************************/

/** ARGC ( -- n )
 */
FCode (p4_argc)                      
{
    FX_PUSH (P4_opt.argc);
}

/** ARGV ( n -- addr u )
 */
FCode (p4_argv)	
{
    p4ucell n = *SP++;

    if (n < (p4ucell) P4_opt.argc)
        p4_strpush (P4_opt.argv [n]);
    else
        p4_strpush (NULL);
}

/** EXPAND-FN ( addr1 u1 addr2 -- addr2 cnt2 )
 */
FCode (p4_expand_fn)		
{
    char *nm = (char *) SP[2];
    char *fn = (char *) SP[0];
    int len = SP[1];
    char* buf;

    buf = p4_pocket_expanded_filename (nm, len, 
				       P4_opt.incpaths, P4_opt.incext);
    strcpy (fn, buf);
    SP++;
    SP[1] = (p4cell) fn;
    SP[0] = strlen (fn);
}

/** USING ( 'filename' -- )
 * use filename as a block file
 */
FCode (p4_using)
{
    char *fn;
    p4ucell len;

    p4_skip_delimiter (' ');
    p4_parse (' ', &fn, &len);
    if (len == 0)
        p4_throw (P4_ON_INVALID_NAME);
    if (!p4_use_block_file (fn, len))
        p4_throws (FX_IOR, fn, len);
}

/** USING-NEW ( 'filename' -- )
 * like => USING but can create the file
 */
FCode (p4_using_new)
{
    char *fn;
    p4ucell len;
    File *fid;
    
    p4_skip_delimiter (' ');
    p4_parse (' ', &fn, &len);
    if (len == 0)
        p4_throw (P4_ON_INVALID_NAME);
    switch (p4_file_access (fn, len))
    {
     case -1:
     case 0:
         fid = p4_create_file (fn, len, FMODE_RWB);
         if (fid == NULL)
             p4_throws (FX_IOR, fn, len);
         p4_close_file (fid);
    }
    if (! p4_use_block_file (fn, len))
        p4_throws (FX_IOR, fn, len);
}

/** ((LOAD")) ( -- ? )
 */
FCode (p4_load_quote_execution)
{
    char *p = (char *) IP;
    int n = (p4char) *p++;

    FX_SKIP_STRING;
    p4_load_file (p, n, *SP++);
}

/** LOAD"  ( [filename<">] -- ? )
 * load the specified file - this word can be
 * compiled into a word-definition
 */
FCode (p4_load_quote)
{
    if (STATE)
    {
        FX_COMPILE (p4_load_quote);
        p4_parse_comma('"');
    }else{
        char *p;
        p4ucell n;

        p4_skip_delimiter (' ');
        p4_parse ('"', &p, &n);
        p4_load_file (p, n, *SP++);
    }
}
P4COMPILES (p4_load_quote, p4_load_quote_execution,
  P4_SKIPS_STRING, P4_DEFAULT_STYLE);

/** INCLUDE ( 'filename' -- ? )
 * load the specified file, see also => LOAD" filename"
 */
FCode (p4_include)
{
    char *fn = p4_word (' ');

    p4_included (fn + 1, *(p4char *) fn);
}

#ifndef NO_SYSTEM
/** SYSTEM ( addr u -- ret-val )
 * run a shell command
 * <br> (embedded systems have no shell)
 */
FCode (p4_system)
{
    SP[1] = p4_systemf ("%.*s", (int) SP[0], (char *) SP[1]);
    SP++;
}

/** ((SYSTEM")) 
 * compiled by => SYSTEM" commandline"
 */
FCode (p4_system_quote_execution)
{
    char *p = (char *) IP;

    FX_SKIP_STRING;
    *--SP = p4_systemf ("%.*s", *p, p + 1);
}

/** SYSTEM" ( [commandline<">] -- ret-val )
 * run a shell command 
 * <br> (embedded systems have no shell)
 */
FCode (p4_system_quote)
{
    if (STATE)
    {
        FX_COMPILE (p4_system_quote);
        p4_parse_comma('"');
    }else{
        char *p;
        p4ucell l;

        p4_parse ('"', &p, &l);
        *--SP = p4_systemf ("%.*s", l, p);
    }
}
P4COMPILES (p4_system_quote, p4_system_quote_execution,
  P4_SKIPS_STRING, P4_DEFAULT_STYLE);
#endif /* NO_SYSTEM */

/** RAISE ( n -- )
 * send a => SIGNAL to self
 */
FCode (p4_raise)
{
    _pfe_raise (*SP++);
}

/** SIGNAL ( xt1 n -- xt2 )
 * install signal handler
 * - return old signal handler
 */
FCode (p4_signal)		
{			
    SP[1] = (p4cell) p4_forth_signal (SP[0], (p4xt) SP[1]);
    SP++;
}

/** OK ( -- )
 * it usually prints "ok" 
 */
extern FCode (p4_ok); 

/** HEADER ( bstring -- )
 * => CREATE a new header in the dictionary from the given string,
 * the runtime is the same as with => CREATE
 usage: : CREATE  BL WORD HERE HEADER ;
 */
FCode (p4_header)  
{
    extern FCode (p4_create_RT);

    p4char* bname = (p4char*)FX_POP;
    p4_header_from (p4_create_RT_, 0, bname+1, (int)*bname);
}

/** (DICTLIMIT)   ( -- constvalue )
 * the upper limit of the forth writeable memory space,
 * the variable => DICTLIMIT must be below this line.
 * stack-space and other space-areas are often allocated
 * above => DICTLIMIT upto this constant.
 *
 * => DICTFENCE is the lower end of the writeable dictionary
 */
FCode(p4_paren_dictlimit)
{
    FX_PUSH (PFE_MEM + PFE_set.total_size);
} 

/** (DICTFENCE)   ( -- constvalue )
 * the lower limit of the forth writeable memory space,
 * the variable => DICTFENCE must be above this line.
 * Some code-areas are often moved in between => DICTFENCE and
 * this constant. To guard normal Forth code from deletion
 * the usual practice goes with the => FENCE variable
 *
 * => DICTLIMIT is the upper end of the writeable dictionary
 */
FCode(p4_paren_dictfence)
{
    FX_PUSH (PFE_MEM);
} 

/* ------------------------------------------------------------------ */

/** >DOES-BODY ( xt -- cell* )
 * in ans'forth a synonym for => >BODY
 : >DOES-BODY >BODY ;
 * in fig'forth it adds another cell
 : >DOES-BODY >BODY CELL+ ;
 */
FCode (p4_to_does_body)
{
    *SP = (p4cell) P4_TO_DOES_BODY (*SP);
}

/** >DOES-CODE ( xt -- xt* )
 * in fig'forth a synonym for => >BODY
 : >DOES-CODE >BODY ;
 * in ans'forth it returns the field that points to DOES>
 * which is just before the cfa in pfe.
 : >DOES-CODE CELL- ;
 */
FCode (p4_to_does_code)
{
    *SP = (p4cell) P4_TO_DOES_CODE (*SP);
}

/** <BUILDS ( 'name' -- )
 *  => CREATE a name whose runtime will be changed later
 *  using => DOES>  <br />
 *  note that ans'forth does not define => <BUILDS and
 *  it suggests to use => CREATE directly. <br />
 *  in fig'forth however it did also => ALLOT a cell
 *  to hold the address where => DOES> starts.
 : <BUILDS CREATE ; \ ans'forth 
 : <BUILDS CREATE 0 , ; \ fig'forth
 */
FCode (p4_does_create)
{
    FX (p4_create);
# ifdef PFE_WITH_FIG
    FX_COMMA (0);
# endif
}

/** R'@ ( R: a b -- a R: a b )
 * fetch the next-under value from the returnstack.
 * used to interpret the returnstack to hold two => LOCALS| values.
 * ( => R@ / => 2R@ / => R>DROP )
 */
FCode (p4_r_tick_fetch)
{
    FX_PUSH (FX_RP[1]);
}

/** R'! ( x R: a b -- R: x b )
 * store the value into the next-under value in the returnstack.
 * used to interpret the returnstack to hold two => LOCALS| values.
 */
FCode (p4_r_tick_store)
{
    FX_RP[1] = FX_POP;
}

/** CLEARSTACK ( -- )
 * reset the parameter stack to be empty
 : CLEARSTACK  S0 SP! ;
 */
FCode (p4_clearstack)
{
    p4SP = PFE.s0;
}

P4_LISTWORDS (misc) =
{
    /** just print OK, also fine for copy-and-paste in terminal */
    CO ("ok",		p4_ok),
    /** quick constants - implemented as code */
    OC ("0",		0),
    OC ("1",		1),
    OC ("2",		2),
    OC ("3",		3),
    /** see => !CSP and ?CSP */
    DV ("CSP",		csp),
    CO ("!CSP",		p4_store_csp),
    CO ("?CSP",		p4_Q_csp),
    /* more fig-forth */
    CO ("COLD",		p4_cold),
    CO ("LIT",		p4_literal_execution),
    CO (".LINE",	p4_dot_line),
    CO ("?COMP",	p4_Q_comp),
    CO ("?EXEC",	p4_Q_exec),
    CO ("?FILE",	p4_Q_file),
    CO ("?LOADING",	p4_Q_loading),
    CO ("?PAIRS",	p4_Q_pairs),
    CO ("?STACK",	p4_Q_stack),
    CO ("TOGGLE",	p4_toggle),
    CO ("LATEST",	p4_latest),
    CO ("SMUDGE",	p4_smudge),
    CO ("UNSMUDGE",	p4_unsmudge),
    /** basic system variables ( => OUT => DP => HLD => R0 => S0 ) */
    DV ("OUT",		out),
    DV ("DP",		dp),
    DV ("HLD",		hld),
    DV ("R0",		r0),
    DV ("S0",		s0),

    /* words without pedigree */
    CO ("UD.R",		p4_u_d_dot_r),
    CO ("UD.",		p4_u_d_dot),
    CO ("ID.",	        p4_id_dot),
    CO (".NAME",	p4_dot_name),
    CO ("-ROLL",	p4_dash_roll),
    CO ("R>DROP",	p4_r_from_drop),
    CO ("DUP>R",	p4_dup_to_r),
    CO ("RANDOM",	p4_random),
    CO ("SRAND",	p4_srand),
    CO ("UNDER+",	p4_under_plus),

    /* more local variables */
    CS ("+TO",		p4_plus_to),
    /* data structures */
    CO ("BUILD-ARRAY",	p4_build_array),
    CO ("ACCESS-ARRAY",	p4_access_array),

    /* more comparision */
    CO ("0<=",		p4_zero_less_equal),
    CO ("0>=",		p4_zero_greater_equal),
    CO ("<=",		p4_less_equal),
    CO (">=",		p4_greater_equal),
    CO ("U<=",		p4_u_less_equal),
    CO ("U>=",		p4_u_greater_equal),
    CO ("UMIN",		p4_u_min),
    CO ("UMAX",		p4_u_max),
    
  /** implementation specific magic - used by control words */
    OC ("EXCEPTION_MAGIC",EXCEPTION_MAGIC),
    OC ("INPUT_MAGIC",	INPUT_MAGIC),
    OC ("DEST_MAGIC",	DEST_MAGIC),
    OC ("ORIG_MAGIC",	ORIG_MAGIC),
    OC ("LOOP_MAGIC",	LOOP_MAGIC),
    OC ("CASE_MAGIC",	CASE_MAGIC),
    OC ("OF_MAGIC",	OF_MAGIC),

    CO ("SHOW-STATUS",	p4_show_status),
    /** variable to enable lower-case symbols as input, 
        => LOWER-CASE and => LOWER-CASE-FN 
    */
    DV ("LOWER-CASE",	 wordl_flag),    
    DV ("LOWER-CASE-FN", lower_case_fn),
  /** variable to enable warnings if creating symbols being in the => ORDER */
    DV ("REDEFINED-MSG", redefined_msg),
  /** the application to be called, options like => ARGC => ARGV */
    DV ("APPLICATION",	application),
  
    CO ("SOURCE-LINE",	p4_source_line),
    CO ("TH'POCKET",	p4_th_pocket),
    CO ("POCKET-PAD",	p4_pocket_pad),
    OC ("WSIZE",	sizeof (p4cell)),
    CO ("W@",		p4_w_fetch),
    CO ("W!",		p4_w_store),
    CO ("W+!",		p4_w_plus_store),
    CO ("WL-HASH",	p4_wl_hash),
    CO ("TOPMOST",	p4_topmost),
     /** the variable accessed with => LATEST */
    DV ("LAST",		last),

    CO ("LS.WORDS",		p4_ls_words),
    CO ("LS.PRIMITIVES",	p4_ls_primitives),
    CO ("LS.COLON-DEFS",	p4_ls_cdefs),
    CO ("LS.DOES-DEFS",		p4_ls_ddefs),
    CO ("LS.CONSTANTS",		p4_ls_constants),
    CO ("LS.VARIABLES",		p4_ls_variables),
    CO ("LS.VOCABULARIES",	p4_ls_vocabularies),
    CO ("LS.MARKERS",		p4_ls_markers),

    CO ("(FORGET)",	p4_paren_forget),
    CO ("TAB",		p4_tab),
    CO ("BACKSPACE",	p4_backspace),
    CO ("?STOP",	p4_Q_stop),
    CO ("START?CR",	p4_start_Q_cr),
    CO ("?CR",		p4_Q_cr),
    CO ("CLOSE-ALL-FILES",p4_close_all_files),
    CO (".MEMORY",	p4_dot_memory),
    CO (".VERSION",	p4_dot_version),
    CO (".PFE-DATE",	p4_dot_date),
    CO ("LICENSE",	p4_license),
    CO ("WARRANTY",	p4_warranty),
    
    /** vectorized i/o variables, see => STANDARD-I/O */
    DV ("*EMIT*",	emit),
    DV ("*EXPECT*",	expect),
    DV ("*KEY*",	key),
    DV ("*TYPE*",	type),
    CO ("(EMIT)",	p4_paren_emit),
    CO ("(EXPECT)",	p4_paren_expect),
    CO ("(KEY)",	p4_paren_key),
    CO ("(TYPE)",	p4_paren_type),
    CO ("STANDARD-I/O",	p4_standard_io),

    /* show online help: */
    CO ("HELP",		p4_help),

    /* more file-manipulation */
    CO ("COPY-FILE",	p4_copy_file),
    CO ("MOVE-FILE",	p4_move_file),
    CO ("FILE-R/W",	p4_file_rw),
    /** the FILE-operations can can also be => USING blocks from a file */
    DC ("BLOCK-FILE",	input.block_file),
    CO ("FILE-BLOCK",	p4_file_block),
    CO ("FILE-BUFFER",	p4_file_buffer),
    CO ("FILE-EMPTY-BUFFERS", p4_file_empty_buffers),
    CO ("FILE-FLUSH",	p4_file_flush),
    CO ("FILE-LIST",	p4_file_list),
    CO ("FILE-LOAD",	p4_file_load),
    CO ("FILE-SAVE-BUFFERS", p4_file_save_buffers),
    CO ("FILE-THRU",	p4_file_thru),
    CO ("FILE-UPDATE",	p4_file_update),

    /** task system hooks */
    CO ("ARGC",         p4_argc),
    CO ("ARGV",         p4_argv),
    DV ("EXITCODE",	exitcode),
     /** ( -- fid ) - the standard file-handles of the task */
    DC ("STDIN",	stdIn),	
    DC ("STDOUT",	stdOut),	
    DC ("STDERR",	stdErr),	

    CO ("EXPAND-FN",	p4_expand_fn),
    CO ("USING",	p4_using),
    CO ("USING-NEW",	p4_using_new),
    CS ("LOAD\"",	p4_load_quote),
#ifndef NO_SYSTEM
    CO ("SYSTEM",	p4_system),
    CS ("SYSTEM\"",	p4_system_quote),
#endif /* NO_SYSTEM */
    CO ("INCLUDE",	p4_include),
    CO ("RAISE",	p4_raise),
    CO ("SIGNAL",	p4_signal),

/* fig'forth helpers */
    CO (">DOES-BODY",   p4_to_does_body),
    CO (">DOES-CODE",   p4_to_does_code),
    CO ("<BUILDS",      p4_does_create),

/* quite useful, esp. for non-locals code */
    CO ("R'@",		p4_r_tick_fetch),
    CO ("R'!",		p4_r_tick_store),
    CO ("CLEARSTACK",	p4_clearstack),

/* smart */
    CO ("EXECUTES",	p4_executes),
    CO ("HEADER",       p4_header),

    DV ("FENCE",        fence),       /* a variable !! */
    DV ("DICTLIMIT",    dictlimit),   /* a variable !! */
    DV ("DICTFENCE",    dict),        /* a variable !! */
    CO ("(DICTLIMIT)",  p4_paren_dictlimit), /* constant */
    CO ("(DICTFENCE)",  p4_paren_dictfence), /* constant */
};
P4_COUNTWORDS (misc, "Misc.Compatibility words");

/*@}*/

