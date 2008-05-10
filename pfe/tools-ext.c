/** 
 * -- The Optional Programming-Tools Word Set
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.14 $
 *     (modified $Date: 2008-05-10 17:04:17 $)
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
"@(#) $Id: tools-ext.c,v 1.14 2008-05-10 17:04:17 guidod Exp $";
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

#define ___ {
#define ____ }

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
    p4_wild_words (wl, (p4char*) "*", NULL);
}

/* ----------------------------------------------------------------------- */
/* Programming-Tools Extension words */

/** AHEAD ( -- DP-mark ORIG-magic ) compile-only
 simulate:
   : AHEAD  BRANCH MARK> (ORIG#) ;
 */
FCode (p4_new_ahead)
{   
    /* FIXME: rename FX(p4_new_ahead) to FX(p4_ahead) in pfe-34 */
    FX_COMPILE (p4_new_ahead);   /* <--- this is the difference */
    FX (p4_forward_mark);
    FX_PUSH (P4_ORIG_MAGIC);
}
P4COMPILES (p4_new_ahead, p4_branch_execution,
  P4_SKIPS_OFFSET, P4_ELSE_STYLE);

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
    p4_forget (P4_NAMESTART (p4_tick_nfa (FX_VOID)));
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

/** ASSEMBLER ( -- )
 * set the => ASSEMBLER-WORDLIST as current => CONTEXT
 */
FCode (p4_assembler) 
{
    CONTEXT[0] = PFE.assembler_wl;
}

/** CODE ( "name" -- )
 * => CREATE a new name and put PFA adress into the CFA place. 
 *
 * NOTE: this description (PFA into CFA) is only correct for traditional
 * indirect threaded code (ITC). The other variants use a block info
 * in the CFA - there we will start a normal colon word which is cut
 * off immediately by a => ;CODE directive to enter the machine-level.
 *
 * BE AWARE:
 * The TOOLS-EXT will not provide an => END-CODE or any other word in the
 * => ASSEMBLER wordlist which is required to start any useful assembler 
 * programming. After requiring ASSEMBLER-EXT you will see a second "CODE"
 * in the => EXTENSIONS wordlist that will also provide an optimized execution
 * than the result of this standard-forth implemenation.
 */
FCode (p4_create_code)
{
#  if !defined PFE_CALL_THREADING
    /* traditional variant for indirect threaded code */
    FX_HEADER; /* FX_SMUDGED; */
    ___ p4xt cfa = (p4xt) p4_HERE;
    FX_COMMA (P4_TO_BODY(cfa)); ____ /* FX_RUNTIME */
#  else
    /* use standard colon and reveal it right away */
    FX (p4_colon);
    FX (p4_colon_EXIT);
#  endif
    FX (p4_also); CONTEXT[0] = PFE.assembler_wl;
}

/** ;CODE ( -- )
 * Does end the latest word (being usually some DOES> part) and enters
 * machine-level (in EXEC-mode). 
 *
 * BE AWARE:
 * The TOOLS-EXT will not provide an => END-CODE or any other word in the
 * => ASSEMBLER wordlist which is required to start any useful assembler 
 * programming. After requiring ASSEMBLER-EXT you will see a second ";CODE"
 * in the => EXTENSIONS wordlist that will also provide an optimized execution
 * than the result of this standard-forth implemenation.
 *
 * The Standard-Forth implementation will actually compile a derivate of
 * => BRANCH into the dictionary followed by =>";". The compiled word
 * will not jump to the target adress (following the execution token)
 * but it will call the target adress via the host C stack. The target
 * machine level word (C domain) will just return here for being
 * returned (Forth domain). Hence => END-CODE may be a simple RET, comma!
 */
FCode_XE (p4_semicolon_code_execution)
{   FX_USE_CODE_ADDR {
    p4code code = (p4code) IP;
    code();
    FX_USE_CODE_EXIT;
}}
FCode (p4_semicolon_code)
{
    FX_COMPILE (p4_semicolon_code);
    ___ p4cell* target = (p4cell*) p4_HERE;
    FX_COMMA (0);
    FX (p4_semicolon);
    *target = (p4cell) p4_HERE; ____;
    /* and finally */
    FX (p4_also); CONTEXT[0] = PFE.assembler_wl;
}
P4COMPILES(p4_semicolon_code, p4_semicolon_code_execution,
	   P4_SKIPS_OFFSET, P4_NEW1_STYLE);

#ifdef PFE_SBR_COMPILE_EXIT
/** END-CODE ( "name" -- )
 * call => PREVIOUS and  add PROC LEAVE assembler snippet as needed
 * for the architecture -  usually includes bits to "return from
 * subroutine". Remember that not all architectures are support and
 * PFE usually does only do variants of call-threading with a separate
 * loop for the inner interpreter that does "call into subroutine".
 * 
 * Some forth implementations do "jump into routine" and the PROC
 * LEAVE part would do "jump to next routine" also known as 
 * next-threading. The sbr-call-threading is usually similar to the
 * native subroutine-coding of the host operating system. See => CODE
 * 
 * On some machine types, this word is NOT DEFINED!
 */
FCode (p4_end_code)
{
    FX (p4_previous); /* kick out ASSEMBLER wordlist */
    PFE_SBR_COMPILE_EXIT (DP);
}
#endif

static FCode (tools_asm_init) 
{
    PFE.assembler_wl = p4_find_wordlist ((p4_char_t*) "ASSEMBLER", 9);
}
/* missing TOOLS-EXT EDITOR */

P4_LISTWORDS (tools) =
{
    P4_INTO ("[ANS]", 0),
    P4_FXco (".S",		   p4_dot_s),
    P4_FXco ("DUMP",		   p4_dump),
    P4_FXco ("SEE",		   p4_see),
    P4_FXco ("WORDS",		   p4_words),
    P4_SXco ("AHEAD",		   p4_new_ahead), 
    P4_FXco ("BYE",		   p4_bye),
    P4_FXco ("CS-PICK",		   p4_cs_pick),
    P4_FXco ("CS-ROLL",		   p4_cs_roll),
    P4_FXco ("FORGET",		   p4_forget),
    P4_IXco ("[ELSE]",		   p4_bracket_else),
    P4_IXco ("[IF]",		   p4_bracket_if),
    P4_IXco ("[THEN]",		   p4_noop),
    P4_FXco ("?",		   p4_question),
    P4_FXco ("CODE",               p4_create_code),
    P4_SXco (";CODE",              p4_semicolon_code),
    P4_INTO ("ASSEMBLER",          0),
# ifdef PFE_SBR_COMPILE_EXIT
    P4_FXco ("END-CODE",           p4_end_code),
# endif
    P4_INTO ("EXTENSIONS", 0 ),

    P4_INTO ("ENVIRONMENT", 0 ),
    P4_OCON ("TOOLS-EXT",   1994 ),
    P4_OCON ("forth200x/defined",   2005 ),
    /* TODO: DEFINED:X should be moved to SEARCH-EXT */
    P4_XXco ("TOOLS-ASM",   tools_asm_init),
};
P4_COUNTWORDS (tools, "TOOLS Programming-Tools (without ASSEMBLER)");

/*@}*/

