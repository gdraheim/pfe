/** 
 * -- The Optional Programming-Tools Word Set
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 5.8 %
 *    (%date_modified: Mon Mar 12 10:33:05 2001 %)
 *
 *  @description
 *      The ANS Forth defines some "Programming Tools", words to
 *      inspect the stack (=>'.S'), memory (=>'DUMP'), 
 *      compiled code (=>'SEE') and what words
 *      are defined (=>'WORDS').
 *
 *      There are also word that provide some precompiler support 
 *      and explicit acces to the =>'CS-STACK'.
 * 
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: tools-ext.c,v 0.30 2001-03-12 09:33:05 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-xtra.h>
#include <pfe/def-comp.h>
#include <pfe/term-sub.h>

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <pfe/_missing.h>


#define DECWIDTH (sizeof (p4cell) * 5 / 2 + 1)
#define HEXWIDTH (sizeof (p4cell) * 2)


static void
p4_prCell (p4cell n)
{
    p4_outf ("%*ld [%0*lX] ",
      DECWIDTH, (long)n,
      HEXWIDTH, (unsigned long)n);
}

/** .S ( -- )
 *     print the stack content in vertical nice format.
 *     tries to show cell-stack and float-stack side-by-side,
 *
 *	 Depending on configuration,
 *	there are two parameter stacks: for integers and for
 *	floating point operations. If both stacks are empty, =>'.S'
 *	will display the message <code>&lt;stacks empty&gt;</code>.
 *
 *	If only the floating point stack is empty, =>'.S' displays
 *	the integer stack items  in one column, one item per line,
 *	both in hex and in decimal like this (the first item is topmost):
 12345 HEX 67890 .S
    	424080 [00067890]
         12345 [00003039] ok
 *
 *      If both stacks ar not empty, => .S displays both stacks, in two
 *	columns, one item per line
 HEX 123456.78E90 ok
 DECIMAL 123456.78E90 .S
    	   291 [00000123]          1.234568E+95
    1164414608 [45678E90] ok
 * 	Confusing example? Remember that floating point input only works
 * 	when the => BASE number is =>'DECIMAL'. The first number looks like
 * 	a floating point but it is a goodhex double integer too - the number
 * 	base is =>'HEX'. Thus it is accepted as a hex number. Second try 
 *      with a decimal base will input the floating point number.
 *
 *      If only the integer stack is empty, => .S shows two columns, but
 *      he first columns is called <tt>&lt;stack empty&gt;</tt>, and the
 *      second column is the floating point stack, topmost item first.
 */
FCode (p4_dot_s)
{
    int i;
    
    int dd = p4_S0 - SP;
#  ifndef P4_NO_FP
    int fd = p4_F0 - FP;
    
    if (fd == 0)
#  endif
    {
        if (dd == 0)
        {   /* !fd !dd */
            /* both stacks empty */
            p4_outf ("<stacks empty> ");
        }else{ /* !fd dd */
            /* only data stack not empty */
            for (i = 0; i < dd; i++)
            {
                FX (p4_cr);
                p4_prCell (SP [i]);
            }
        }
    }
# ifndef P4_NO_FP
    else if (dd == 0) /* fd !dd */
    {
        /* only floating point stack not empty */
        p4_outf ("\n<stack empty>%*.7G ",
          (DECWIDTH + HEXWIDTH + 4 - 13) + 15, FP [0]);
        for (i = 1; i < fd; i++)
            p4_outf ("\n%*.7G ",
              (DECWIDTH + HEXWIDTH + 4) + 15, FP [i]);
    }else{ /* fd dd */
        int bd = dd < fd ? dd : fd;
        for (i = 0; i < bd; i++)
        {
	    FX (p4_cr);
	    p4_prCell (SP [i]);
	    p4_outf ("%15.7G ", FP [i]);
        }
	for (; i < dd; i++)
        {
	    FX (p4_cr);
	    p4_prCell (SP [i]);
        }
	for (; i < fd; i++)
            p4_outf ("\n%*.7G ",
              (DECWIDTH + HEXWIDTH + 4) + 15, FP [i]);
    }
# endif
}

/** ? ( addr -- )
 * Display the (integer) content of at address <tt>addr</tt>.
 * This word is sensitive to =>'BASE'
 simulate:
   : ?  @ . ;
 */
FCode (p4_question)
{
    FX (p4_fetch);
    FX (p4_dot);
}

/** DUMP ( addr len -- )
 * show a hex-dump of the given area, if it's more than a screenful
 * it will ask using => ?CR
 *
 * You can easily cause a segmentation fault of something like that
 * by accessing memory that does not belong to the pfe-process.
 */
FCode (p4_dump)
{
    p4ucell i, j, n = (p4ucell) FX_POP;
    p4char *p;
    
    p = P4_POP_ (p4char*, SP);
    FX (p4_cr);
    FX (p4_start_Q_cr);
    p4_outf ("%*s ", HEXWIDTH, "");
    for (j = 0; j < 16; j++)
        p4_outf ("%02X ", (unsigned)((p4ucell)(p + j) & 0x0F));
    for (j = 0; j < 16; j++)
        p4_outf ("%X", (unsigned)((p4ucell)(p + j) & 0x0F));
    for (i = 0; i < n; i += 16, p += 16)
    {
        if (p4_Q_cr ())
            break;
        p4_outf ("%0*lX ", HEXWIDTH, (unsigned long)(p4ucell)p);
        for (j = 0; j < 16; j++)
            p4_outf ("%02X ", p [j]);
        for (j = 0; j < 16; j++)
            p4_outf ("%c", p4_isprint (p [j]) ? p [j] : '.');
    }
    FX (p4_space);
}

/** SEE ( "word" -- )
 *  decompile word - tries to show it in re-compilable form.
 *
 *  => (SEE) tries to display the word as a reasonable indented
 *  source text. If you defined your own control structures or
 *  use extended control-flow patterns, the indentation may be
 *  suboptimal.
 simulate:
   : SEE  [COMPILE] ' (SEE) ; 
 */

FCode (p4_see)
{
    char *nfa;
    p4xt xt;
    
    nfa = p4_tick (&xt);
    p4_decompile (nfa, xt);
}

/** WORDS ( -- )
 * uses CONTEXT and lists the words defined in that vocabulary.
 * usually the vocabulary to list is named directly in before.
 example:
    FORTH WORDS  or  LOADED WORDS
 */
FCode (p4_words)
{
    Wordl *wl = CONTEXT [0] ? CONTEXT [0] : ONLY;
    p4_wild_words (wl, "*", NULL);
}

/** VLIST ( -- )
 *  The VLIST command had been present in FIG and other forth
 *  implementations. It has to list all accessible words. In PFE
 *  it list all words in the search order. Well, the point is,
 *  that we do really just look into the search order and are
 *  then calling => WORDS on that Wordl. Uses => ?CR
*/

FCode (p4_vlist) 
{
    extern int p4_Q_cr ();
    Wordl **p, **q;
    
    for (p = CONTEXT; p <= &ONLY; p++)
    {
        if (*p == NULL)
            continue;
        for (q = CONTEXT; *q != *p; q++); /* don't search wordl twice */
        if (q != p)
            continue;
        p4_dot_name (p4_to_name (BODY_FROM (*p)));
        p4_outs ("WORDS");
        p4_wild_words ((*p), "*", NULL);
        
        if (p < &ONLY) {
            PFE.more = 0; if (p4_Q_cr ()) return; 
        }
    }
}


/* Programming-Tools Extension words */

/** AHEAD ( -- DP-mark ORIG-magic ) compile-only
 simulate:
   : AHEAD  MARK> (ORIG#) ;
 */
FCode (p4_ahead)
{
    FX (p4_forward_mark);
    FX_PUSH (ORIG_MAGIC);
}

/** BYE ( -- ) no-return
 * should quit the forth environment completly
 */
FCode (p4_bye)
{
    FX (p4_save_buffers);
    FX (p4_close_all_files);
    if (P4_opt.quiet)
        p4_outc ('\n');
    else
        p4_outs ("\nGoodbye!\n");
#  ifndef _K12_SOURCE
    p4_longjmp_exit ();
#  else
    /* BYE doesn't make sense in an embedded system. */
    FX (p4_cold);
#  endif /* _K12_SOURCE */
}

/** CS-PICK ( 2a 2b 2c ... n -- 2a 2b 2c ... 2a )
 * pick a value in the compilation-stack - note that the compilation
 * stack _can_ be seperate in some forth-implemenations. In PFE
 * the parameter-stack is used in a double-cell fashion, so CS-PICK
 * would 2PICK a DP-mark and a COMP-magic, see => PICK
 */
FCode (p4_cs_pick)
{
    p4cell n = (*SP-- + 1) << 1;
    SP [0] = SP [n];
    SP [1] = SP [n + 1];
}

/** CS-ROLL ( 2a 2b 2c ... n -- 2b 2c ... 2a )
 * roll a value in the compilation-stack - note that the compilation
 * stack _can_ be seperate in some forth-implemenations. In PFE
 * the parameter-stack is used in a double-cell fashion, so CS-ROLL
 * would 2ROLL a DP-mark and a COMP-magic, see => ROLL
 */
FCode (p4_cs_roll)
{
    p4cell n = *SP++;
    p4dcell h = ((p4dcell *)SP) [n];
    for (; n > 0; n--)
        ((p4dcell *)SP) [n] = ((p4dcell *)SP) [n - 1];
    ((p4dcell *)SP) [0] = h;
}

/** FORGET ( "word" -- )
 simulate:
   : FORGET  [COMPILE] '  >NAME (FORGET) ; IMMEDIATE
 */
FCode (p4_forget)
{
    p4xt xt;
    if (LAST) FX (p4_unsmudge);
    p4_forget (_FFA(p4_tick (&xt)));
}

/** [ELSE] ( -- )
 * eat up everything upto and including the next [THEN]. count
 * nested [IF] ... [THEN] constructs. see => [IF]
 this word provides a simple pre-compiler mechanism
 */
FCode (p4_bracket_else)
{
    char *p;
    int len, level = 1;

    do {
        for (;;)
	{
            p = p4_word (' ');
            if ((len = *(p4char*)p++) == 0)
                break;
            if (LOWER_CASE)
                p4_upper (p, len);
            if (len == 4 && strncmp (p, "[IF]", 4) == 0)
                ++level;
            else if (len == 6 && strncmp (p, "[ELSE]", 6) == 0)
                if (--level == 0) return; else ++level;
            else  if (len == 6 && strncmp (p, "[THEN]", 6) == 0)
                if (--level == 0) return;
	}
    } while (p4_refill ());
    p4_throw (P4_ON_UNEXPECTED_EOF);
}

/** [IF] ( flag -- )
 * check the condition in the CS-STACK. If true let the following
 * text flow into => INTERPRET , otherwise eat up everything upto
 * and including the next => [ELSE] or => [THEN] . In case of 
 * skipping, count nested [IF] ... [THEN] constructs.
 this word provides a simple pre-compiler mechanism
 */
FCode (p4_bracket_if)
{
    if (FX_POP == 0)
        FX (p4_bracket_else);
}

P4_LISTWORDS (toolkit) =
{
    CO (".S",		p4_dot_s),
    CO ("DUMP",		p4_dump),
    CO ("SEE",		p4_see),
    CO ("WORDS",	p4_words),
    CI ("AHEAD",	p4_ahead),       /*fixme: isn't that incorrect ? */
    CO ("BYE",		p4_bye),
    CO ("CS-PICK",	p4_cs_pick),
    CO ("CS-ROLL",	p4_cs_roll),
    CO ("FORGET",	p4_forget),
    CI ("[ELSE]",	p4_bracket_else),
    CI ("[IF]",		p4_bracket_if),
    CI ("[THEN]",	p4_noop),
    CO ("?",		p4_question),
    /* an extension */
    CO ("VLIST",        p4_vlist),
    P4_INTO ("ENVIRONMENT", 0 ),
    P4_OCON ("TOOLS-EXT",	1994 ),
};
P4_COUNTWORDS (toolkit, "Programming-Tools + parts of extensions");

/*@}*/

