/** 
 * -- The Optional Programming-Tools Word Set
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2006 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.2 $
 *     (modified $Date: 2006-08-11 22:56:05 $)
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
"@(#) $Id: tools-ext.c,v 1.2 2006-08-11 22:56:05 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-limits.h>
#include <pfe/def-comp.h>
#include <pfe/term-sub.h>
#include <pfe/header-ext.h>

#include <stdlib.h>
#include <pfe/os-string.h>
#include <pfe/os-ctype.h>

#include <pfe/_missing.h>


#define DECWIDTH (sizeof (p4cell) * 5 / 2 + 1)
#define HEXWIDTH (sizeof (p4cell) * 2)

#ifndef USE_END_CODE
#if defined PFE_SBR_COMPILE_PROC && defined PFE_SBR_COMPILE_EXIT
#define USE_END_CODE 1
#else
#define USE_END_CODE 0
#endif
#endif

static void
p4_prCell (p4cell n)
{
    p4_outf ("%*ld [%0*lX] ",
      (int) DECWIDTH, (p4celll)n,
      (int) HEXWIDTH, (p4ucelll)n);
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
            p4_outf ("\n%*s",
              (int)(DECWIDTH + HEXWIDTH + 4), "<stacks empty> ");
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
        p4_outf ("\n%*s%15.7G ",
          (int)(DECWIDTH + HEXWIDTH + 4), "<stack empty> ", FP [0]);
        for (i = 1; i < fd; i++)
            p4_outf ("\n%*.7G ",
              (int)(DECWIDTH + HEXWIDTH + 4) + 15, FP [i]);
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
              (int)(DECWIDTH + HEXWIDTH + 4) + 15, FP [i]);
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
    p4_outf ("%*s ", (int)HEXWIDTH, "");
    for (j = 0; j < 16; j++)
        p4_outf ("%02X ", (unsigned)((p4ucell)(p + j) & 0x0F));
    for (j = 0; j < 16; j++)
        p4_outf ("%X", (unsigned)((p4ucell)(p + j) & 0x0F));
    for (i = 0; i < n; i += 16, p += 16)
    {
        if (p4_Q_cr ())
            break;
        p4_outf ("%0*lX ", (int)HEXWIDTH, (p4ucelll)p);
        for (j = 0; j < 16; j++)
            p4_outf ("%02X ", p [j]);
        for (j = 0; j < 16; j++)
            p4_outf ("%c", (p4_isspace (p [j]) ? ' ' : 
			    ! p4_isascii (p [j]) ? '_' :
			    p4_isprintable (p [j]) ? p [j] : '.'));
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
    p4_namebuf_t *nfa = p4_tick_nfa ();
    p4_decompile (nfa, p4_name_from(nfa));
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

/* ----------------------------------------------------------------------- */
/* Programming-Tools Extension words */

/** AHEAD ( -- DP-mark ORIG-magic ) compile-only
 simulate:
   : AHEAD  MARK> (ORIG#) ;
 */
FCode (p4_ahead)
{
    FX (p4_forward_mark);
    FX_PUSH (P4_ORIG_MAGIC);
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
    if (LAST) FX (p4_reveal);
    p4_forget (P4_NFA2START (p4_tick_nfa (FX_VOID)));
}

/* ----------------------------------------------------------------------- */

/** [ELSE] ( -- )
 * eat up everything upto and including the next [THEN]. count
 * nested [IF] ... [THEN] constructs. see => [IF]
 this word provides a simple pre-compiler mechanism
 */
FCode (p4_bracket_else)
{
    p4_char_t *p;
    int len, level = 1;

    do {
        for (;;)
	{
            p = p4_word (' ');
            if ((len = *p++) == 0)
                break;
            if (UPPER_CASE)
                p4_upper (p, len);
            if (len == 4 && p4_strncmp ((char*) p, "[IF]", 4) == 0)
                ++level;
            else if (len == 6 && p4_strncmp ((char*) p, "[ELSE]", 6) == 0)
                if (--level == 0) return; else ++level;
            else  if (len == 6 && p4_strncmp ((char*) p, "[THEN]", 6) == 0)
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

#if USE_END_CODE+0
/** CODE ( "name" -- )
 * call => ALSO and add ASSEMBLER wordlist if available. Add PROC ENTER
 * assembler snippet as needed for the architecture into the PFA. The
 * CFA is setup (a) with the PFA adress in traditional ITC or (b)
 * with an infoblock as for sbr-coded colon words.
 * <p> Remember that not all architectures are support and that the
 * ASSEMBLER wordset is not compiled into pfe by default. Use always
 * the corresponding => END-CODE for each => CODE start. The new
 * word name is not smudged.
 */
FCode (p4_create_code)
{
    FX_HEADER; /* FX_SMUDGED; */
#  ifndef PFE_SBR_CALL_THREADING
    /* indirect threaded */
    { p4xcode* dp = (p4xcode*) DP; FX_COMMA (dp+1); }
#  else
    { 
        /*atic const char* nest_code = "_"; */
        static const char _nest_code[] = { p4_NEST, 0 };
        static const char* nest_code = _nest_code;
        FX_COMMA (&nest_code); /* CODE trampoline */ 
    }
#  endif
    PFE_SBR_COMPILE_PROC (DP); /* a.k.a. FX_COMPILE_PROC */
    FX (p4_also);
    { void* p = p4_find_wordlist_str ("ASSEMBLER"); if (p) CONTEXT[0] = p; }
}

/** END-CODE ( "name" -- )
 * call => PREVIOUS and  add PROC LEAVE assembler snippet as needed
 * for the architecture -  usually includes bits to "return from
 * subroutine". Remember that not all architectures are support and
 * PFE usually does only do variants of call-threading with a separate
 * loop for the inner interpreter that does "call into subroutine".
 * Some forth implementations do "jump into routine" and the PROC
 * LEAVE part would do "jump to next routine" also known as 
 * next-threading. The sbr-call-threading is usually similar to the
 * native subroutine-coding of the host operating system. See => CODE
 */
FCode (p4_semicolon_code)
{
    FX (p4_previous);
    PFE_SBR_COMPILE_EXIT (DP);
}
#endif

/* missing TOOLS-EXT ASSEMBLER */
/* missing TOOLS-EXT CODE */
/* missing TOOLS-EXT ;CODE */
/* missing TOOLS-EXT EDITOR */

P4_LISTWORDS (tools) =
{
    P4_INTO ("[ANS]", 0),
    P4_FXco (".S",		p4_dot_s),
    P4_FXco ("DUMP",		p4_dump),
    P4_FXco ("SEE",		p4_see),
    P4_FXco ("WORDS",		p4_words),
    P4_IXco ("AHEAD",		p4_ahead), /*fixme: isn't that incorrect ? */
    P4_FXco ("BYE",		p4_bye),
    P4_FXco ("CS-PICK",		p4_cs_pick),
    P4_FXco ("CS-ROLL",		p4_cs_roll),
    P4_FXco ("FORGET",		p4_forget),
    P4_IXco ("[ELSE]",		p4_bracket_else),
    P4_IXco ("[IF]",		p4_bracket_if),
    P4_IXco ("[THEN]",		p4_noop),
    P4_FXco ("?",		p4_question),
#  if USE_END_CODE+0
    P4_FXco ("CODE",            p4_create_code),
    P4_FXco (";CODE",           p4_semicolon_code),
#  endif

    P4_INTO ("ENVIRONMENT", 0 ),
    P4_OCON ("TOOLS-EXT",	1994 ),
};
P4_COUNTWORDS (tools, "TOOLS Programming-Tools (without ASSEMBLER)");

/*@}*/

