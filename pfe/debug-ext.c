/**
 * PFE-DEBUG --- analyze compiled code
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE             @(#) %derived_by: guidod %
 *  @version %version: 5.11 %
 *    (%date_modified: Tue Apr 24 11:53:00 2001 %)
 *
 *  @description
 *	The Portable Forth Environment provides a decompiler for
 *      colon words and a single stepper for debugging. After 
 *      setting a breakpoint at a word saying => DEBUG <tt>word</tt>.
 *  	The next time the <tt>word</tt> gets executed the single
 * 	stepper takes control.
 *
 * 	When this happens you see the top stack items displayed in one
 *	line. The topmost stack item is the first in line, the second and
 *	following stack items are displayed throughout the end of line.
 *	This line is empty if the stack is empty when the word in question
 *	executes.
 *
 *	On the next line you see the first word to become executed inside
 *	the debugged <tt>word</tt>. There is a prompt <tt>&gt;</tt> to
 *	the right of the displayed word. At this prompt you have several
 *	options. Choose one by typing a key (<tt>[h]</tt> shows helpscreen):
 *
 *	<dl>
 *	<dt> <tt>[enter], [x], [k], [down]</tt> </dt>  <dd>
 *	The displayed word will be executed without single stepping.
 *	Note that the execution of the word is slowed down a little
 *	compared to execution outside the single stepper. This is
 *	because the single stepper has to keep control to detect when
 *	the word has finished.
 *
 *	After the actual word finished execution the resulting stack
 *	is printed on the current line. The next line shows the next
 *	word to become executed.
 *
 *	Having repeated this step several times, you can see to the
 *	the right of every decompiled word what changes to the stack
 *	this word caused by comparing with the stack display just
 *	one line above.
 *      </dd>
 *	<dt> <tt>[d], [l], [right]</tt> </dt><dd>
 *	Begin single step the execution of the actual word. The first
 *	word to become executed inside the definition is displayed on
 *	the next line. The word's display is intended by two spaces
 *	for each nesting level.
 * 
 *   	You can single step through colon-definitions and the children
 *	of defining words. Note that most of the words in PFE are
 *	rewritten in C for speed, and you can not step those kernel
 *	words.
 *      </dd>
 *      <dt> <tt>[s], [j], [left]</tt> </dt><dd>
 *	Leaves the nesting level. The rest of the definition currently
 *	being executed is run with further prompt. If you leave the
 *	outmost level, the single stepper won't get control again.
 *	Otherwise the debugger stops after the current word is
 *	finished and offers the next word in the previous nesting level.
 *	</dd>
 *	<dt> <tt>[space]</tt> </dt><dd>
 *	The next word to be executed is decompiled. This should help 
 *	to decide as if to single step that word.
 *	</dd>
 *	<dt> <tt>[q]</tt> </dt><dd>
 *	Quits from the debugger. The execution of the debugged word is
 *	not continued. The stacks are not cleared or changed.
 *	</dd>
 *	<dt> <tt>[c]</tt> </dt><dd>
 *	Displays the profiling instruction counter.
 *	<dt> <tt>[r]</tt> </dt><dd>
 *	Reset the instruction counter, to profile some code. The
 *	debugger counts how often the inner interpreter i.e. how
 *	many Forth-primitives are executed. Use this option to 
 *      reset the counter to 0 to measure an arbitrary part of code.
 *	</dd>
 *	</dl>
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: debug-ext.c,v 0.31 2001-04-24 22:49:01 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-xtra.h>
#include <pfe/def-types.h>
#include <pfe/def-comp.h>
#include <pfe/term-sub.h>

#ifdef PFE_WITH_DSTRINGS_EXT    /* (dnw 4feb01) */
# include <pfe/dstrings-ext.h>
#endif

#include <ctype.h>
#include <string.h>


#include <pfe/_missing.h>
/************************************************************************/
/* decompiler                                                           */
/************************************************************************/

#ifdef WRONG_SPRINTF		/* provision for buggy sprintf (SunOS) */
#define SPRFIX(X) strlen(X)
#else
#define SPRFIX(X) X
#endif

#define UDDOTR(UD,W,BUF) p4_outs (p4_str_ud_dot_r (UD, &(BUF)[sizeof (BUF)], W,BASE))
#define DDOTR(D,W,BUF)	p4_outs (p4_str_d_dot_r (D, &(BUF) [sizeof (BUF)], W, BASE))
#define DOT(N,BUF)	p4_outs (p4_str_dot (N, &(BUF) [sizeof (BUF)], BASE))

static p4xt *
p4_decompile_word (p4xt* ip, char *p, p4_Decomp *d)
{
    static p4_Decomp default_style = {P4_SKIPS_NOTHING, 0, 0, 0, 0, 0};
    p4xt xt = *ip++;
    p4_Semant *s;
    char const *nfa;
    char buf[80];

    s = p4_to_semant (xt);
    *d = s ? s->decomp : default_style;
    if (*xt == PFX (p4_literal_execution))
    {
        if (s) 
            sprintf (p, "( %.*s) 0x%p ", NFACNT(*s->name), s->name+1, *(void**)ip);
        else
            strcpy (p, p4_str_dot (*(p4cell *) ip, buf + sizeof buf, BASE));
        return ++ip;
    }
#ifdef PFE_WITH_DSTRINGS_EXT	/* (dnw 19feb01) */
    {
      static unsigned int frame_size;

      if (*xt == PFX (p4_make_str_frame_execution))
      {
        int i;
        
	frame_size = (p4cell) *ip;
	p += SPRFIX (sprintf (p, "ARGS{ "));
	for (i = frame_size; --i >= 0;)
	  p += SPRFIX (sprintf (p, "<%c> ", 'A' - 1 + (p4cell) *ip - i));
	p += SPRFIX (sprintf (p, "} "));
	return ++ip;
      }
      if (*xt == PFX (p4_marg_execution))
      {
        sprintf (p, "<%c> ", 'A' + frame_size - 1 - (p4cell ) *ip);
        return ++ip;
      }
    }
#endif
    if (*xt == PFX (p4_locals_bar_execution))
    {
        int i;
        
        /* locals[PFE.level] = *(p4cell *) ip; */
        p += SPRFIX (sprintf (p, "LOCALS| "));
        for (i = ((p4cell*)ip)[1]; --i >= 0;)
            p += SPRFIX (sprintf (p, "<%c> ", 
				  'A'-1 + (unsigned)(((p4ucell*)ip)[1]) - i));
        p += SPRFIX (sprintf (p, "| "));
        return (ip+=2);
    }
    if (*xt == PFX (p4_local_execution))
    {
        sprintf (p, "<%c> ", 'A' - 1 +  (int) *(p4cell *) ip);
        return ++ip;
    }
    if (s == NULL)
    {
        nfa = p4_to_name (xt);
        sprintf (p, *_FFA(nfa) & P4xIMMEDIATE ? "POSTPONE %.*s " : "%.*s ",
          NFACNT(*nfa), nfa + 1);
        return ip;
    }
    else
        nfa = s->name;
    switch (d->skips)
    {
     case P4_SKIPS_CELL:
     case P4_SKIPS_OFFSET:
         P4_INC (ip, p4cell);
         
     default:
         sprintf (p, "%.*s ", NFACNT(*nfa), nfa + 1);
         return ip;
     case P4_SKIPS_DCELL:
         sprintf (p, "%s. ",
           p4_str_d_dot_r (*(p4dcell *) ip, buf + sizeof buf, 0, BASE));
         P4_INC (ip, p4dcell);
         
         return ip;
     case P4_SKIPS_FLOAT:
#if PFE_ALIGNOF_DFLOAT > PFE_ALIGNOF_CELL
         if (!P4_DFALIGNED (ip))
             ip++;
#endif
         sprintf (p, "%e ", *(double *) ip);
         P4_INC (ip, double);

      return ip;
     case P4_SKIPS_STRING:
         sprintf (p, "%.*s %.*s\" ",
           NFACNT(*nfa), nfa + 1,
           (int) *(p4char *) ip, (p4char *) ip + 1);
         P4_SKIP_STRING (ip);
         return ip;
     case P4_SKIPS_2STRINGS:
     {
         p4char *s1 = (p4char *) ip;
         
         P4_SKIP_STRING (ip);
         sprintf (p, "%.*s %.*s %.*s ",
           NFACNT(*nfa), nfa + 1, (int) *s1, s1 + 1,
           (int) *(p4char *) ip, (p4char *) ip + 1);
         P4_SKIP_STRING (ip);
         return ip;
     }
#  ifdef PFE_WITH_DSTRINGS_EXT    /* (dnw 4feb01) */
    case P4_SKIPS_PSTRING:
	sprintf (p, "%.*s %.*s\" ",
		 NFACNT(*nfa), nfa + 1,
		 (int) *(p4_PCount *) ip, (p4char *) ip + sizeof(p4_PCount));
	P4_SKIP_PSTRING (ip);
	return ip;
    case P4_SKIPS_PSTRING_BACK_TICK:
	sprintf (p, "%.*s %.*s` ",
		 NFACNT(*nfa), nfa + 1,
		 (int) *(p4_PCount *) ip, (p4char *) ip + sizeof(p4_PCount));
	P4_SKIP_PSTRING (ip);
	return ip;
#   endif
     case P4_SKIPS_TO_TOKEN:
         if (*xt == s->exec[0])
         {
             xt = *ip++;
             nfa = p4_to_name (xt);
             sprintf (p, "%.*s %.*s ", 
               NFACNT(*s->name), s->name+1,
               NFACNT(*nfa), nfa + 1);
             { /* make-recognition, from yours.c */
                 if (d->space > 1) ip++;
                 if (d->space > 2) ip++;
             }
             return ip;
         }else{
             sprintf (p, "%.*s <%c> ", 
               NFACNT(*nfa), nfa + 1,
               'A' - 1 + (int) *(p4cell *) ip);
             { /* make-recognition, from yours.c */
                 if (d->space > 1) ip++;
                 if (d->space > 2) ip++;
             }
             return ++ip;
         }
    }
}

_export void
p4_decompile_rest (p4xt *ip, int nl, int indent)
{
    char buf[0x80] = "";
    p4_Seman2 *s;
    p4_Decomp d;
    
    FX (p4_start_Q_cr);
    for (;;)
    {
        if (!*ip) break;
        s = (p4_Seman2 *) p4_to_semant (*ip);
        ip = p4_decompile_word (ip, buf, &d);
        indent += d.ind_bef;
        if ((!nl && d.cr_bef) || OUT + strlen (buf) >= (size_t) PFE.cols)
	{
            if (p4_Q_cr ())
                break;
            nl = 1;
	}
        if (nl)
	{
            p4_emits (indent, ' ');
            nl = 0;
	}
        p4_outs (buf);
        p4_emits (d.space, ' ');
        indent += d.ind_aft;
        if (d.cr_aft)
	{
            if (p4_Q_cr ())
                break;
            nl = 1;
	}
        if (d.cr_aft > 2)  /* instead of exec[0] == PFX(semicolon_execution) */
            break;
    }
}

_export void
p4_decompile (char *nfa, p4xt xt)
{
    char buf[80];

    FX (p4_cr);
    if (*xt == p4_create_RT_)
    {
        p4_outs ("VARIABLE ");
        p4_dot_name (nfa);
    }
    else if (*xt == p4_constant_RT_)
    {
        DOT (*TO_BODY (xt), buf);
        p4_outs ("CONSTANT ");
        p4_dot_name (nfa);
    }
    else if (*xt == p4_value_RT_)
    {
        DOT (*TO_BODY (xt), buf);
        p4_outs ("VALUE ");
        p4_dot_name (nfa);
    }
    else if (*xt == p4_two_constant_RT_)
    {
        DDOTR (*(p4dcell *) TO_BODY (xt), 0, buf);
        p4_outs (". 2CONSTANT ");
        p4_dot_name (nfa);
    }
#ifndef P4_NO_FP
    else if (*xt == p4_f_constant_RT_)          
    {
        p4_outf ("%g FCONSTANT ", *(double *) p4_dfaligned ((p4cell) TO_BODY (xt)));
        p4_dot_name (nfa);
    }
    else if (*xt == p4_f_variable_RT_)
    {
        p4_outf ("%g FVARIABLE ", *(double *) p4_dfaligned ((p4cell) TO_BODY (xt)));
        p4_dot_name (nfa);
    } 
#endif
    else if (*xt == p4_marker_RT_)
    {
        p4_outs ("MARKER ");
        p4_dot_name (nfa);
    }
    else if (*xt == p4_defer_RT_ )
    {
        p4_outs ("DEFER ");
        p4_dot_name (nfa);
    }
    else if (*xt == p4_offset_RT_)
    { 
        DOT (*TO_BODY (xt), buf);
        p4_outs ("OFFSET: ");
        p4_dot_name (nfa);
    }
    else if (*xt == p4_vocabulary_RT_)
    {
        p4_outs ("VOCABULARY ");
        p4_dot_name (nfa);
    }
    else if (*xt == p4_colon_RT_ 
      || *xt == p4_debug_colon_RT_)
    {
        p4_outs (": ");
        p4_dot_name (nfa);
        FX (p4_cr);
        p4_decompile_rest ((p4xt *) TO_BODY (xt), 1, 4);
    }
    else if (*xt == p4_does_defined_RT_ 
      || *xt == p4_debug_does_defined_RT_)
    {
        p4_outs ("DOES> ");
        p4_decompile_rest (((p4xt **) xt)[-1], 0, 4);
    }else{
        p4_dot_name (nfa);
        p4_outf ("is primitive ");
    }
    if (*_FFA(nfa) & P4xIMMEDIATE)
        p4_outs ("IMMEDIATE ");
}

/************************************************************************/
/* debugger                                                             */
/************************************************************************/

_export char
p4_category (p4code p)
{
    if (p == p4_colon_RT_ || p == p4_debug_colon_RT_)
        return ':';
    if (p == p4_create_RT_)
        return 'V';
    if (p == p4_constant_RT_ || p == p4_two_constant_RT_)
        return 'C';
    if (p == p4_vocabulary_RT_)
        return 'W';
    if (p == p4_does_defined_RT_ || p == p4_debug_does_defined_RT_)
        return 'D';
    if (p == p4_marker_RT_)
        return 'M';
    if (p == p4_defer_RT_)
        return 'F'; 
    if (p == p4_offset_RT_)
        return '+';
    /* must be primitive */ return 'p';
}

static void
prompt_col (void)
{
    p4_emits (24 - OUT, ' ');
}

static void
display (p4xt *ip)
{
    p4_Decomp style;
    char buf[80];
    int indent = PFE.maxlevel * 2;
    int depth = p4_S0 - SP, i;

    prompt_col ();
    for (i = 0; i < depth; i++)
    {
        p4_outf ("%10ld ", (long) SP[i]);
        if (OUT + 11 >= PFE.cols)
            break;
    }
    FX (p4_cr);
    p4_decompile_word (ip, buf, &style);
    p4_outf ("%*s%c %s", indent, "", p4_category (**ip), buf);
}

static void
interaction (p4xt *ip)
{
    int c;

    for (;;)
    {
        display (ip);
        
        prompt_col ();
        p4_outs ("> ");
        c = p4_getekey ();
        FX (p4_backspace);
        FX (p4_backspace);
        if (isalpha (c))
            c = tolower (c);

        switch (c)
	{
         default:
             p4_dot_bell ();
             continue;
         case P4_KEY_kr:
         case 'd':
         case 'l':
             PFE.maxlevel++;
             return;
         case P4_KEY_kd:
         case '\r':
         case '\n':
         case 'k':
         case 'x':
             return;
         case P4_KEY_kl:
         case 's':
         case 'j':
             PFE.maxlevel--;
             return;
         case 'q':
             p4_outf ("\nQuit!");
             PFE.debugging = 0;
             p4_throw (P4_ON_QUIT);
         case ' ':
             switch (p4_category (**ip))
             {
              default:
                  p4_decompile (p4_to_name (*ip), *ip);
                  break;
              case ':':
                  FX (p4_cr);
                  p4_decompile_rest ((p4xt *) TO_BODY (*ip), 1, 4);
                  break;
              case 'd':
                  p4_outs ("\nDOES>");
                  p4_decompile_rest ((p4xt *) (*ip)[-1], 0, 4);
                  break;
             }
             FX (p4_cr);
             continue;
         case 'r':
             PFE.opcounter = 0;
             p4_outf ("\nOperation counter reset\n");
             continue;
         case 'c':
             p4_outf ("\n%ld Forth operations\n", PFE.opcounter);
             continue;
         case 'h':
         case '?':
             p4_outf ("\nDown,  'x', 'k', CR\t" "execute word"
               "\nRight, 'd', 'l'\t\t" "single step word"
               "\nLeft,  's', 'j'\t\t" "finish word w/o single stepping"
               "\nSpace\t\t\t" "SEE word to be executed"
               "\n'C'\t\t\t" "display operation counter"
               "\n'R'\t\t\t" "reset operation counter"
               "\n'Q'\t\t\t" "QUIT"
		"\n'?', 'H'\t\t" "this message"
               "\n");
             continue;
	}
    }
}

static void
do_adjust_level (p4xt xt)
{
    if (*xt == p4_colon_RT_ 
      || *xt == p4_debug_colon_RT_ 
      || *xt == p4_does_defined_RT_ 
      || *xt == p4_debug_does_defined_RT_)
        PFE.level++;
    else if (*xt == PFX (p4_semicolon_execution) 
      || *xt == PFX (p4_locals_exit_execution))
        PFE.level--;
}

static void
p4_debug_execute (p4xt xt)
{
    do_adjust_level (xt);
    p4_normal_execute (xt);
}

static void
p4_debug_on (void)
{
    PFE.debugging = 1;
    PFE.opcounter = 0;
    PFE.execute = p4_debug_execute;
    PFE.level = PFE.maxlevel = 0;
    p4_outf ("\nSingle stepping, type 'h' or '?' for help\n");
}

_export void
p4_debug_off (void)
{
    PFE.debugging = 0;
    PFE.execute = p4_normal_execute;
}

static void			/* modified inner interpreter for */
do_single_step (void)		/* single stepping */
{
    while (PFE.level >= 0)
    {
        if (PFE.level <= PFE.maxlevel)
	{
            PFE.maxlevel = PFE.level;
            interaction (IP);
	}
        do_adjust_level (*IP);
        PFE.opcounter++;
        {
#ifdef P4_WP_VIA_IP
            p4xt w = *IP++;	/* ip is register but W isn't */
            
            (*w) ();
#else
            p4WP = *IP++;	/* ip and W are same: register or not */
            (*p4WP) ();
#endif
        }
    }
}

FCode (p4_debug_colon_RT)
{
    FX (p4_colon_RT);
    if (!PFE.debugging)
    {
        p4_debug_on ();
        do_single_step ();
        p4_debug_off ();
    }
}

FCode (p4_debug_does_defined_RT)
{
    FX (p4_does_defined_RT);
    if (!PFE.debugging)
    {
        p4_debug_on ();
        do_single_step ();
        p4_debug_off ();
    }
}

/** DEBUG ( 'word' -- )
 * this word will place an debug-runtime into
 * the => CFA of the following word. If the
 * word gets executed later, the user will
 * be prompted and can decide to single-step
 * the given word. The debug-stepper is
 * interactive and should be self-explanatory.
 * (use => NO-DEBUG to turn it off again)
 */
FCode (p4_debug)
{
    p4xt xt;

    p4_tick (&xt);
    if (*xt == p4_debug_colon_RT_ 
      || *xt == p4_debug_does_defined_RT_)
        return;
    else if (*xt == p4_colon_RT_)
        *xt = p4_debug_colon_RT_;
    else if (*xt == p4_does_defined_RT_)
        *xt = p4_debug_does_defined_RT_;
    else
        p4_throw (P4_ON_ARG_TYPE);
}

/** NO-DEBUG ( 'word' -- )
 * the inverse of " => DEBUG word "
 */
FCode (p4_no_debug)
{
    p4xt xt;

    p4_tick (&xt);
    if (*xt == p4_debug_colon_RT_)
        *xt = p4_colon_RT_;
    else if (*xt == p4_debug_does_defined_RT_)
        *xt = p4_does_defined_RT_;
    else
        p4_throw (P4_ON_ARG_TYPE);
}

/** (SEE) ( xt -- )
 * decompile the token-sequence - used
 * by => SEE name
 */
FCode (p4_paren_see)
{
    p4_decompile (0, (void*)FX_POP);
}

/** ADDR>NAME ( addr -- nfa|0 )
 * search the next corresponding namefield that address
 * is next too. If it is not in the base-dictionary, then
 * just return 0 as not-found.
 */
const p4char *
p4_addr_to_name (const p4char* addr)
{
    Wordl* wl;
    int t;
    p4char const * nfa;
    p4char const * best = 0;

    if (addr >  DP) return 0;
    if (addr < PFE.dict) return 0;

    /* foreach vocobulary */
    for (wl = VOC_LINK; wl; wl = wl->prev)
    {
        /* foreach thread */
        for (t=0; t < THREADS; t++)
        {
            nfa = wl->thread[t];
            /* foreach name in linked names */
            while (nfa)
            {
                if (nfa < addr && best < nfa)
                {
                    best = nfa;
                }
                nfa = *p4_name_to_link(nfa);
            }
        }
    }
    return best;
}

/** ADDR>NAME ( addr -- nfa|0 )
 * search the next corresponding namefield that address
 * is next too. If it is not in the base-dictionary, then
 * just return 0 as not-found.
 */
FCode (p4_addr_to_name)
{
    *SP = (p4cell) p4_addr_to_name((p4char*)(*SP));
}

/** COME_BACK ( -- )
 * show the return stack before last exception
 * along with the best names as given by => ADDR>NAME
 */
FCode (p4_come_back)
{
    char const * nfa;
    p4xt** rp = (p4xt**) p4_CSP;

    if (PFE.rstack < rp && rp < PFE.r0)
    {
        if (PFE.dict < (p4char*) *rp && (p4char*) *rp < PFE.dp
          && (nfa = p4_addr_to_name ((void*)((*rp)[-1]))))
        {
            p4_outf (" --> in %.*s \n", NFACNT(*nfa), nfa+1);
        }else{
            p4_outs (" --> (???) \n");
        }

        while (rp < RP)
        {
            nfa = p4_addr_to_name ((void*)(*rp));
            if (nfa)
            {
                p4_outf ("[%d] %08p ' %.*s (%+d) \n", 
                  RP-rp, *rp, NFACNT(*nfa), nfa+1, 
                  ((p4xt) *rp) - (p4_name_from(nfa)));
            }else{
                p4_outf ("[%d] %08p   %+ld \n", 
                  RP-rp, *rp, (long) *rp);
            }
            rp++;
        }
    }else{
        p4_outs (" come_back csp trashed, sorry \n");
    }
}

P4_LISTWORDS (debug) =
{
    CO ("DEBUG",        p4_debug),
    CO ("NO-DEBUG",     p4_no_debug),
    CO ("(SEE)",	p4_paren_see),
    CO ("ADDR>NAME",    p4_addr_to_name),
    CO ("COME_BACK",    p4_come_back),
    P4_INTO ("ENVIRONMENT", 0 ),
    P4_DCON ("PFE-DEBUG", maxlevel ),
};
P4_COUNTWORDS (debug, "Debugger words");

/*@}*/

