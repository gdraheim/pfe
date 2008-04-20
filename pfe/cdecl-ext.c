/** 
 * -- C-like declaration primitives
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.4 $
 *     (modified $Date: 2008-04-20 04:46:29 $)
 *
 *  @description
 *       some words that mimic partial behaviour of a C compiler
 *       especially it's preprocessor.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: cdecl-ext.c,v 1.4 2008-04-20 04:46:29 guidod Exp $";
#endif
 
#define _P4_SOURCE 1
#include <pfe/pfe-base.h>
#include <pfe/def-limits.h>

#include <pfe/os-string.h>
#include <pfe/os-ctype.h>

#include <pfe/header-ext.h>
#include <pfe/logging.h>

#define ___ PFE_DECLARE_BLOCK
#define ____ PFE_DECLARE_END

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

/** #ELSE ( -- ) [FTH]
 * The implementation of #ELSE is done in C for speed and
 * being less error prone. Better use the ANSI-compatible
 * => [IF] => [ELSE] => [THEN] construct.
 */
FCode (p4_sh_else)
{
    int level = 1;
    
    do{
        for (;;)
        {
            p4_charbuf_t* word = p4_word (' ');
	    int len =      P4_CHARBUF_LEN(word);
	    p4_char_t* p = P4_CHARBUF_PTR(word);
            if (len == 0) break;

            if (UPPER_CASE) p4_upper (p, len);
            
            if ((len == 3 && memcmp (p, "#IF", 3) == 0)
              || (len == 6 && memcmp (p, "#IFDEF", 6) == 0)
              || (len == 6 && memcmp (p, "#IFNDEF", 7) == 0)
              || (len == 9 && memcmp (p, "#IFNOTDEF", 9) == 0))
            {
                ++level;
            }
            else if (len == 5 && memcmp (p, "#ELSE", 5) == 0)
            {
                if (--level == 0) return;  else ++level;
            }
            else if (len == 6 && memcmp (p, "#ENDIF", 6) == 0)
            {
                if (--level == 0) return;
            }
        }
    } while (p4_refill ()) ;
    p4_throw (P4_ON_UNEXPECTED_EOF);
}

/** #ENDIF ( -- ) [FTH] 
 * end of => #IF => #IFDEF => #IFNOTDEF and => #ELSE contructs
   (a dummy word that does actually nothing, but #ELSE may look for it)
 */
FCode(p4_sh_endif)
{
    /* just nothing */
}

#define P4_NUMBER_IF_MAGIC P4_MAGIC_('X','#','I','F')

/** #IF ( -- C: state-save-flag mfth-if-magic S: ) [FTH]
 * prepares for a following => #IS_TRUE or => #IS_FALSE,
 * does basically switch off compile-mode for the enclosed
 * code. <br>
 * better use the ANSI style => [IF] => [ELSE] => [THEN] construct.
 */
FCode (p4_sh_if)
{
    FX_PUSH (PFE.state); PFE.state = 0;
    FX_PUSH (P4_NUMBER_IF_MAGIC);
}

/** #IS_TRUE ( C: state-save-flag mfth-if-magic S: test-flag -- ) [FTH]
 * checks the condition on the <c>CS-STACK</c>. <br>
 * Pairs with => #IF <br>
 * better use the ANSI style => [IF] => [ELSE] => [THEN] construct.
 */
FCode (p4_sh_is_true)
{
    p4cell value = FX_POP;
    p4_Q_pairs (P4_NUMBER_IF_MAGIC);
    PFE.state = FX_POP;

    if (value == 0)
	FX (p4_sh_else);
}

/** #IS_FALSE ( C: state-save-flag mfth-if-magic S: test-flag -- ) [FTH]
 * checks the condition on the <c>CS-STACK</c>. <br>
 * Pairs with => #IF <br>
 * better use the ANSI style => [IF] => [ELSE] => [THEN] construct.
 */
FCode (p4_sh_is_false)
{
    p4cell value = FX_POP;
    p4_Q_pairs (P4_NUMBER_IF_MAGIC);
    PFE.state = FX_POP;

    if (value)
	FX (p4_sh_else);
}

/** #IFDEF ( "word" -- ) [FTH]
 * better use <c>[DEFINED] word [IF]</c> - the word => [IF]
 * is ANSI-conform.
 */
FCode (p4_sh_ifdef)
{
    extern FCode (p4_defined);
    FX (p4_defined);
    if (! FX_POP)
        FX (p4_sh_else);
}

/** #IFNOTDEF ( "word" -- ) [FTH]
 * better use <c>[DEFINED] word [NOT] [IF]</c> - the word => [IF]
 * and => [ELSE] are ANSI-conform, while => #IFDEF => #ELSE are not.
 */
FCode (p4_sh_ifnotdef)
{
    extern FCode (p4_defined);
    FX (p4_defined);
    if (FX_POP)
        FX (p4_sh_else);
} 

/** // ( [...<cr>] -- ) [FTH]
 * a line-comment
 */
extern FCode(p4_backslash);

/** #define ( "name" "value" -- ) [FTH]
 * create an alias, will actually make a =>"DEFER"ed word,
 * and it has the magic to handle number-arguments
 */
FCode (p4_sh_define)
{
    FX (p4_defer); /* FX_HEADER */
    ___ p4cell* body = p4_to_body(p4_name_from(PFE.last));

    p4_word_parseword (' '); *DP=0; /* PARSE-WORD-NOHERE */
    ___ p4_namebuf_t* name = p4_find (PFE.word.ptr, PFE.word.len);

    if (name)
    {
        *body = (p4cell) p4_name_from (name);
    }else{
        auto p4dcell d;
        if (p4_number_question (PFE.word.ptr, PFE.word.len, &d))
        {
            *body = (p4cell) DP;
            if (p4_DPL >= 0)
            {
                FX_RCOMMA (p4_two_constant_RT_);
                FX_COMMA_ (d.hi,'D');
                FX_COMMA_ (d.lo,'d');
            }else{
                FX_RCOMMA (p4_constant_RT_);
                FX_SCOMMA (d.lo);
            }
        }else{
            p4_throw (P4_ON_INVALID_NAME);
        }
    }
    ____;____;
}

/** #pragma ( "word" "evaluate<cr>" -- ) [FTH]
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
    p4_word_parseword (' '); *DP=0; /* PARSE-WORD-NOHERE */

    /* new style */
    if (PFE.atexit_wl) 
    { 
        register p4char* nfa = 
	    p4_search_wordlist (PFE.word.ptr, PFE.word.len, PFE.atexit_wl);
        if (nfa)
        {
            if (PFE_IS_DESTROYER(nfa))
                FX_PUSH (p4_to_body(p4_name_from(nfa)));
            else
                p4_call (p4_name_from(nfa));
            return;
        }
    }

    /* BEWARE: no old-style query like in environ-ext.c */

    FX (p4_backslash);
}

P4_LISTWORDS (cdecl) =
{
    P4_INTO ("FORTH", 0),
    P4_IXco ("#ELSE",          p4_sh_else),
    P4_IXco ("#ENDIF",         p4_sh_endif),
    P4_IXco ("#IF",            p4_sh_if),
    P4_IXco ("#IFDEF",         p4_sh_ifdef),
    P4_IXco ("#IFNDEF",        p4_sh_ifnotdef),
    P4_IXco ("#IFNOTDEF",      p4_sh_ifnotdef),
    P4_IXco ("#IS_TRUE",       p4_sh_is_true),
    P4_IXco ("#IS_FALSE",      p4_sh_is_false),
    P4_SNYM ("//",             "\\"),
    P4_FXco ("#DEFINE",        p4_sh_define),
    P4_FXco ("#PRAGMA",        p4_sh_pragma),
};
P4_COUNTWORDS (cdecl, "C-preprocessor declaration syntax");

/*@}*/

