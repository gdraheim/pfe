/** 
 * -- C-like declaration primitives
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 1.4 %
 *    (%date_modified: Mon Mar 12 10:32:01 2001 %)
 *
 *  @description
 *       some words that mimic partial behaviour of a C compiler
 *       especially it's preprocessor.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: cdecl-ext.c,v 0.30 2001-03-12 09:32:01 guidod Exp $";
#endif
 
#define _P4_SOURCE 1
#include <pfe/pfe-base.h>
#include <pfe/def-xtra.h>

#include <string.h>
#include <ctype.h>

#include <pfe/forth-usual-ext.h>
#include <pfe/logging.h>

/* ------------------ #IFDEF ------------------------- */
/* 
   old-style precompiler if-else-then construct
   as used by many older forth scripts. These symbols are
   not used in ANSI, but as they were in widespread use
   we do include them here. Older Forth Scripts can hence
   be easier ported, possibly using ifdef'ed code sections
   for either variant.

This implementation of #ELSE is the base for #IF, #IFDEF etc.

variable #if-state
: #ENDIF            postpone [then] ; immediate
: #IF               state @ #if-state ! false state ! ; immediate
: #IFDEF            bl word find 0= if postpone #ELSE then drop ; immediate
: #IFNOTDEF         bl word find if postpone #ELSE then drop ; immediate
: #IS_FALSE         #if-state @ state ! if postpone #ELSE then ; immediate
: #IS_TRUE          #if-state @ state ! 0= if postpone #ELSE then ; immediate
*/

/** #ELSE ( -- )
 * The implementation of #ELSE is done in C for speed and
 * being less error prone. Better use the ANSI-compatible
 * => [IF] => [ELSE] => [THEN] construct.
 */
FCode (p4_sh_else)
{
    char *p;
    int len, level = 1;
    
    do{
        for (;;)
        {
            p = p4_word (' ');
            if ((len = *(unsigned char *) p++) == 0)
                break;
            if (LOWER_CASE)
                p4_upper (p, len);
            
            if ((len == 3 && strncmp (p, "#IF", 3) == 0)
              || (len == 6 && strncmp (p, "#IFDEF", 6) == 0)
              || (len == 6 && strncmp (p, "#IFNDEF", 7) == 0)
              || (len == 9 && strncmp (p, "#IFNOTDEF", 9) == 0))
            {
                ++level;
            }
            else if (len == 5 && strncmp (p, "#ELSE", 5) == 0)
            {
                if (--level == 0) return;  else ++level;
            }
            else if (len == 6 && strncmp (p, "#ENDIF", 6) == 0)
            {
                if (--level == 0) return;
            }
        }
    } while (p4_refill ()) ;
    p4_throw (P4_ON_UNEXPECTED_EOF);
}

/** #ENDIF ( -- )
 * end of => #IF => #IFDEF => #IFNOTDEF and => #ELSE contructs
   (a dummy word that does actually nothing, but #ELSE may look for it)
 */
FCode(p4_sh_endif)
{
    /* just nothing */
}

#define NUMBER_IF_MAGIC MAKE_MAGIC('X','#','I','F')

/** #IF ( -- )
        ( -- state-save mfth-if-magic )
 * prepares for a following => #IS_TRUE or => #IS_FALSE,
 * does basically switch off compile-mode for the enclosed
 * code. <br>
 * better use the ANSI style => [IF] => [ELSE] => [THEN] construct.
 */
FCode (p4_sh_if)
{
    FX_PUSH(PFE.state); PFE.state = 0;
    FX_PUSH(NUMBER_IF_MAGIC);
}

/** #IS_TRUE ( flag -- )
              ( state-save mfth-if-magic flag -- )
 * checks the condition on the <c>CS-STACK</c>. <br>
 * Pairs with => #IF <br>
 * better use the ANSI style => [IF] => [ELSE] => [THEN] construct.
 */
FCode (p4_sh_is_true)
{
    p4cell value = FX_POP;
    p4_Q_pairs(NUMBER_IF_MAGIC);
    PFE.state = FX_POP;

    if (value == 0)
	FX (p4_sh_else);
}

/** #IS_FALSE ( flag -- )
              ( state-save mfth-if-magic flag -- )
 * checks the condition on the <c>CS-STACK</c>. <br>
 * Pairs with => #IF <br>
 * better use the ANSI style => [IF] => [ELSE] => [THEN] construct.
 */
FCode (p4_sh_is_false)
{
    p4cell value = FX_POP;
    p4_Q_pairs(NUMBER_IF_MAGIC);
    PFE.state = FX_POP;

    if (value)
	FX (p4_sh_else);
}

/** #IFDEF ( "word" -- )
 * better use <c>[DEFINED] word [IF]</c> - the word => [IF]
 * is ANSI-conform.
 */
FCode (p4_sh_ifdef)
{
    extern FCode (p4_bracket_defined);
    FX (p4_bracket_defined);
    if (! FX_POP)
        FX (p4_sh_else);
}

/** #IFNOTDEF ( "word" -- )
 * better use <c>[DEFINED] word [NOT] [IF]</c> - the word => [IF]
 * and => [ELSE] are ANSI-conform, while => #IFDEF => #ELSE are not.
 */
FCode (p4_sh_ifnotdef)
{
    extern FCode (p4_bracket_defined);
    FX (p4_bracket_defined);
    if (FX_POP)
        FX (p4_sh_else);
} 

/** // ( [...<cr>] -- )
 * a line-comment
 */
extern FCode(p4_backslash);

/** #define ( "name" "value" -- )
 * create an alias, will actually make a =>"DEFER"ed word,
 * and it has the magic to handle number-arguments
 */
FCode (p4_sh_define)
{
    char* nm;
    p4cell len;
    char* s;
    p4cell* body;

    FX (p4_defer);
    body = TO_BODY(p4_name_from(PFE.last));

    p4_parseword (' ', &nm, &len);
    s = p4_find (nm, len);

    if (s)
    {
        *body = (p4cell) p4_name_from (s);
    }else{
        p4dcell d;
        if (p4_number_question (nm, len, &d))
        {
            *body = (p4cell) DP;
            if (p4_DPL >= 0)
            {
                FX_COMMA (p4_two_constant_RT_);
                FX_COMMA (d.hi);
            }else{
                FX_COMMA (p4_constant_RT_);
            }
            FX_COMMA (d.lo);
        }else{
            p4_throw (P4_ON_INVALID_NAME);
        }
    }
}

/** #pragma ( "word" -- ? )
 * pass the word to => ENVIRONMENT?
 * If the word does *not* exist, the rest of the line is parsed
 * away with => // - therefore, if the executed word does not
 * consume the line itself, the rest of the line is still executed.
 * examples:
 #pragma warnings on // if warnings is a variable, ON can set it
 #pragma stack-cells 50 < [if] .( not enough stackcells ) [then]
 #pragma simply anything else you like to have in environment or not

 * implementation:
 : #pragma ?exec
   bl word count environment? if exit then ( interpret the rest of the line )
   [compile] \               ( parse away the rest of the line as a comment )
 ;
 */
FCode (p4_sh_pragma)
{
    p4cell len;
    char* nm;

    p4_parseword (' ', &nm, &len);

    /* new style */
    if (PFE.atexit_wl) 
    { 
        p4char* nfa = p4_search_wordlist (nm, len, PFE.atexit_wl);
        if (nfa)
        {
#           ifdef PFE_WITH_FFA
            if (*_FFA(nfa) & P4xONxDESTROY)
                FX_PUSH (TO_BODY(p4_name_from(nfa)));
            else
#           endif
                p4_call (p4_name_from(nfa));
            return;
        }
    }

    /* BEWARE: no old-style query like in environ-ext.c */

    FX (p4_backslash);
}

P4_LISTWORDS (cdecl) =
{
    CI ("#ELSE",          p4_sh_else),
    CI ("#ENDIF",         p4_sh_endif),
    CI ("#IF",            p4_sh_if),
    CI ("#IFDEF",         p4_sh_ifdef),
    CI ("#IFNDEF",        p4_sh_ifnotdef),
    CI ("#IFNOTDEF",      p4_sh_ifnotdef),
    CI ("#IS_TRUE",       p4_sh_is_true),
    CI ("#IS_FALSE",      p4_sh_is_false),
    CI ("//",             p4_backslash),
    CO ("#DEFINE",        p4_sh_define),
    CO ("#PRAGMA",        p4_sh_pragma),
};
P4_COUNTWORDS (cdecl, "C-preprocessor declaration syntax");

/*@}*/

