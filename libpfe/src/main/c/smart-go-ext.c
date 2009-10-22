/**
 * -- smart outer interpreter
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2008-04-20 04:46:30 $)
 *
 *  @description
 *      Smart Outer Interpreter allows to register executions tokens
 *      that get tied to a single char - if the outer interpreter
 *      Compatiblity with former standards, miscellaneous useful words.
 *      ... for TOOLS-EXT
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) =
"@(#) $Id: smart-go-ext.c,v 1.3 2008-04-20 04:46:30 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-limits.h>
#include <pfe/os-ctype.h>

static p4xt
_p4_smart_interpret_char (char c)
{
    auto p4_char_t wd[] = "interpret- ";
    register p4_namebuf_t* nfa;
    register p4xt xt;

    wd[sizeof(wd)-2] = c;
    nfa = p4_find (wd, sizeof(wd)-1);
    if (! nfa || ! P4_NAMExIMMEDIATE(nfa))
        return 0; /* must be immediate */
    xt = p4_name_from (nfa);
    if (P4_XT_VALUE(xt) != FX_GET_RT (p4_defer))
        return 0; /* must be DEFER or DOER */
    return xt;
}

/**
 */
static p4xt
p4_smart_interpret_char (char c)
{
    if (!ispunct ((unsigned char) c))
    {
        return NULL;
    }else{
        return _p4_smart_interpret_char (c);
    }
}

/**
 */
static p4_char_t*
_p4_smart_interpret_init (char c, char const * nm, int l)
{
    auto p4_char_t wd[] = "interpret- ";
    register p4_namebuf_t* nfa;

    wd[sizeof(wd)-2] = c;
    if (! (nfa = p4_find ((p4_char_t*) nm, l))) return 0;
    if (! (P4_NAMExIMMEDIATE(nfa))) return 0;

    p4_header_comma (wd, sizeof(wd)-1, CURRENT); FX_IMMEDIATE;
    FX_RUNTIME1 (p4_defer);
    FX_XCOMMA (p4_name_from (nfa));
    return nfa;
}

/**
 */
void
p4_smart_interpret_init (char c, char const * nm, int l)
{
    if (!_p4_smart_interpret_init (c, nm, l))
        p4_throw (P4_ON_UNDEFINED);
}

/** SMART-INTERPRET-INIT ( -- )
 * creates a set of interpret-words that are used in the inner
 * interpreter, so if a word is unknown to the interpreter-loop
 * it will use the first char of that word, attach it to an
 * "interpret-" prefix, and tries to use that =>'IMMEDIATE'-=>'DEFER'-word
 * on the rest of the word. This => SMART-INTERPRET-INIT will set up
 * words like interpret-" so you can write
 * <c>"hello"</c>  instead of   <c>" hello"</c>
 * and it creates interpret-\ so that words like <c>\if-unix</c> are
 * ignoring the line if the word <c>\if-unknown</c> is unknown in itself.
 * This is usually <i>not</i> activated on startup.
 */
void FXCode (p4_smart_interpret_init)
{
    /* so comments will even get shorter */
    _p4_smart_interpret_init ('(', "(", 1);
    /* will silently ignore any unknown backslash word */
    _p4_smart_interpret_init ('\\', "\\", 1);
    _p4_smart_interpret_init ('@', "@>", 2);
    _p4_smart_interpret_init ('!', "TO", 2);
    _p4_smart_interpret_init ('\'', "ASCII", 5);  /* C like */
    _p4_smart_interpret_init ('^', "CONTROL", 7);
# ifdef P4_C_QUOTE
    _p4_smart_interpret_init ('\"', "C\"", 2); /* C like */
# else
    _p4_smart_interpret_init ('\"', "S\"", 2); /* C like */
# endif
}

/** SMART-INTERPRET! ( -- )
 * enables/disables the SMART-INTERPRET extension in => INTERPRET ,
 * (actually stores an XT in => DEFER inside the mainloop interpreter)
 */
void FXCode (p4_smart_interpret_store)
{
    if (FX_POP)
        PFE.smart_char = p4_smart_interpret_char;
    else
        PFE.smart_char = NULL;
}

static p4ucell
FXCode (interpret_smart) /*hereclean*/
{
    register p4xt xt;
    /* scanned word-span is now at PFE.word. (not HERE) */

    if (! PFE.smart_char) return 0; /* quick path */

    /* WORD-string is at HERE */
    xt = PFE.smart_char (*PFE.word.ptr);
    if (! xt) return 0; /* quick path */

    TO_IN -= PFE.word.len; p4_skip_delimiter (*PFE.word.ptr);
    p4_call (xt);
    return 1;
}

#ifndef SMART_INTERPRET_SLOT       /* USER-CONFIG: */
#define SMART_INTERPRET_SLOT 1     /* 1 == smart-ext / 2 == floating-ext */
#endif

static void FXCode (smart_interpret_deinit)
{
    FX_USE_BODY_ADDR;
    FX_POP_BODY_ADDR_UNUSED;
    PFE.interpret[SMART_INTERPRET_SLOT] = 0;
}

static void FXCode(smart_interpret_init)
{
    PFE.interpret[1] = PFX (interpret_smart);
    p4_forget_word ("deinit:smart-interpret:%i", SMART_INTERPRET_SLOT,
                    PFX (smart_interpret_deinit), SMART_INTERPRET_SLOT);
}

P4_LISTWORDSET (smart_go) [] =
{
    P4_INTO ("EXTENSIONS", 0),
    P4_FXco ("SMART-INTERPRET-INIT",	p4_smart_interpret_init),
    P4_FXco ("SMART-INTERPRET!",	p4_smart_interpret_store),

    P4_INTO ("ENVIRONMENT", 0),
    P4_XXco ("SMART-INTERPRET-LOADED",	smart_interpret_init),
};
P4_COUNTWORDSET (smart_go, "smart-go interpreter");

/*@}*/
