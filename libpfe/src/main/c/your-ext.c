/**
 * -- user-supplied additional primitives
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) Guido U. Draheim, 2005 - 2008
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.4 $
 *     (modified $Date: 2008-05-02 20:31:29 $)
 *
 *  @description
 *              This wordset is the place to add any additional primitives
 *		you might wish. A set of words do already live here that
 *              must be bound statically into the main pfe-object to
 *              work out smart and nicely.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) =
"@(#) $Id: your-ext.c,v 1.4 2008-05-02 20:31:29 guidod Exp $";
#endif
/**
 *
 * To make a new primitive Forth word, you have to write a C function of
 * type
 *
 *	static void name_(void)
 *
 * Use the preprocessor macro
 *
 *	FCode (p4_name)
 *
 * to provide the prototype with the underscore appended to the
 * name. The underscore helps to avoid name clashes with names and
 * keywords of the C language.
 *
 * Having defined a primitive you must add it to the forth dictionary
 * in order to make it visible to Forth.  At the end of each input
 * file defining forth primitives you'll find a table declared with
 * the macro
 *
 *	P4_LISTWORDS (wordset_name) =
 *	{
 *	  ...
 *	};
 *
 * This declares a vector of some type and shows how to continue with
 * the initialization of the vector.  Each vector element registers a
 * forth word for loading into the dictionary at startup. In PFE the
 * dictionary is not an initialized C structure but it is built from
 * such structures -- like the one described here -- at startup.
 *
 * To enter words into this table there are several macros, all for
 * brevity named with two uppercase letters. Using such a macro adds
 * one more element to the list of initializers following the
 * P4_LISTWORDS declaration. Besides primitives you can enter variables
 * and constants to the dictionary this way.
 *
 *	P4_FXco (NAME, c-name)	a primitive, not immediate
 *	P4_IXco (NAME, c-name)	an immediate primitive
 *	P4_SXco (NAME, c-name)	a primitive with separate execution
 *				and compilation semantics, see examples
 *				in core-ext.c (if, case etc.) and macros
 *				in def-comp.h and -- good luck :-)
 *	P4_OVaR (NAME)		a normal variable
 *	P4_OCoN (NAME, value)	a normal constant
 *	P4_OVaL (NAME, value)	a normal value
 *	P4_IVaR (NAME)		an immediate variable
 *	P4_ICoN (NAME, value)	an immediate constant
 *	P4_IVaL (NAME, value)	an immediate value
 *
 * Finally, add a description with P4_COUNTWORDS and you can then let
 * it be loaded at startup by modifying the PFE startup loadlist in
 * pfe-words.c, and loaded wordsets get visible in the ATEXIT-WORDLIST
 */

#define _P4_SOURCE 1
#include <pfe/pfe-base.h>

#include <pfe/os-string.h>
#include <pfe/os-ctype.h>
#include <pfe/def-limits.h>
#include <pfe/logging.h>

/** "'>" ( [name] -- xt )
 * get the execution-token, ie the CFA, of the word following.
 * This word is fully state-smart while the ANSI standard words
 * namely => ' and => ['] are not.
 */
FCode (p4_tick_from)
{
   extern FCode (p4_tick);
   extern FCode (p4_comma);

   FX (p4_tick);
   if (STATE)
   {
       FX_COMPILE (p4_tick_from);
       FX (p4_comma);
   }
}
P4COMPILES (p4_tick_from, p4_literal_execution,
   	P4_SKIPS_CELL, P4_DEFAULT_STYLE);
/* ->
  the P4COMPILES is for so called "smart words" which compile
  some words into the dictionary. This example compiles a
  literal into the dictionary. The PFE has some magic to decompile
  the name of *this* word instead of the LITERAL that has provided
  us with its execution-semantics. The execution-runtime is
  references in the macro and we instruct the decompiler to skip
  the following cell - literal_execution would then advance the IP
  by one cell which we have been telling to the decompiler in the
  COMPILES-macro. The decompiler output can be further modified,
  like identation and linebreaks but here it is just DEFAULT_STYLE
*/

/* ------------------------------------------------------------- */

/** ((@>)) ( -- value )
 * execution compiled by => @>
 */
static FCode_XE (p4_fetch_from_XT)
{
    FX_USE_CODE_ADDR;
    FX_PUSH ( *P4_POP_(p4cell*,IP));
    FX_USE_CODE_EXIT;
}

extern FCode (p4_constant_RT);

/** @> ( [name] -- value )
 * does fetch the value from the PFA of the named item, which
 * may be about everything, including a => VARIABLE , => VALUE
 * => LVALUE , => LOCALS| , => VAR , => DEFER , => DOER , => DOES>
 * and more.
 */
FCode (p4_fetch_from)
{
    if (!STATE)
    {
        FX_PUSH (*p4_to_body (p4_tick_cfa (FX_VOID)));
    }else{
        register p4_namebuf_t* nfa;
        p4_word_parseword (' ');
        if ((nfa = p4_find (PFE.word.ptr, PFE.word.len)))
        {
            if (*P4_TO_CODE(p4_name_from (nfa)) != PFX(p4_constant_RT) )
            {
                FX_XCOMMA (p4_name_from (nfa));
            }else{
                FX_COMPILE (p4_fetch_from);
                FX_PCOMMA (p4_to_body (p4_name_from (nfa)));
            }
            return;
        }
        if (! p4_word_compile_local ())
        {
            p4_word_to_here (); /* fixme: don't need anymore if throw changed*/
            p4_throw (P4_ON_UNDEFINED);
        }
    }
}
P4COMPILES (p4_fetch_from, p4_fetch_from_XT,
	    P4_SKIPS_CELL, P4_DEFAULT_STYLE);

/** !> ( value [name] -- )
 * actually a synonym for => TO but very common amongst
 * forth interpreters
 */


/* these are mostly copied from p4_to in core.c */

/** ((INTO))
 * execution compiled by => INTO
 */
FCode_XE (p4_into_execution)
{
    FX_USE_CODE_ADDR;
    FX_PUSH (p4_to_body ((p4xt)( P4_POP(IP) )));
    FX_USE_CODE_EXIT;
}

/** ((INTO-)) ( -- value )
 * execution compiled by => INTO
 */
FCode_XE (p4_into_local_execution)
{
    FX_USE_CODE_ADDR;
    FX_PUSH (&LP [ P4_POP(IP) ]);
    FX_USE_CODE_EXIT;
}

/** INTO ( [name] -- pfa )
 * will return the parameter-field address of the following word.
 * Unlike others, this word will also return the address of
 * => LOCALS| and local => LVALUE - so in fact a <c>TO A</c> and
 * <c>INTO A !</c> are the same. This word is most useful when calling
 * C-exported function with a temporary local-VAR as a return-place
 * argument - so the address of a local has to be given as an arg.
 * Beware that you should not try to save the address anywhere else,
 * since a local's address does always depend of the RP-depth -
 * EXIT from a colon-word and the value may soon get overwritten.
 * (see also => TO )
 */
FCode (p4_into)
{
    register p4_charbuf_t *p;
    register int l, n;

    if (STATE)
    {
        p = p4_word (' ');
        l = *p++;
        if (PFE.locals && (n = p4_find_local (p, l)) != 0)
	{
            FX_COMPILE2 (p4_into);
            FX_UCOMMA (n);
	}else{
            if ((p = p4_find (p, l)) == NULL)
	           p4_throw (P4_ON_UNDEFINED);
            FX_COMPILE1 (p4_into);
            FX_XCOMMA (p4_name_from (p));
	}
    }else{
        FX_PUSH (p4_to_body (p4_tick_cfa (FX_VOID)));
    }
}

P4COMPILES2 (p4_into, p4_into_execution, p4_into_local_execution,
  P4_SKIPS_TO_TOKEN, P4_DEFAULT_STYLE);
/* ->
 this "smart word" is really smart. Depending on the situation it
 will compile two different executions. The decompiler will still
 be able to present us with the name of *this* word - in this case
 we have even defined our own execution-runtimes right here. Just
 see how we grab the next cell and advance the IP by one cell. And
 instead of SKIPS_CELL we use the decompiler's special implementation
 for SKIPS_TO_TOKEN, so that low number will not be seen as the
 address of a VARIABLE, instead the decompiler knows it's an offset
 from the RP, and it will present us with names like <A>. Nice eh.
*/

/* ------------------------------------------------------------ */

/** .H2 ( value -- )
 * print hexadecimal, but with per-byte 0-padding
   0x0     -> 00
   0xf     -> 0f
   0x12    -> 12
   0x123   -> 0123
   0x1234  -> 1234
   0x12345 -> 012345
 */
FCode (p4_dot_h2)
{
    int len = sprintf ((char*) (PAD+1), "%lx", (p4celll) *SP);
    HLD = PAD+1 + len; *PAD = '0';
    if (len&1)
	p4_outs ((char*) PAD);
    else
	p4_outs ((char*) PAD+1);
}

/* ------------------------------------------------------------- */

p4char*
p4_here_word (char del)
{
    const char *q;
    char *p;
    int n, i;

    p4_skip_delimiter (' '); /* <= the only different to p4_word */
    p4_source ((const p4_char_t**) &q, &n);
    q += TO_IN;
    n -= TO_IN;
    p = (char *) DP + 1;
    if (del == ' ')
    {
        for (i = 0;
             i < n && !(p4_isascii (*q) && p4_isspace (*q)); i++)
        {
            *p++ = *q++;
        }
    }else{
        for (i = 0; i < n && *q != del; i++)
        {
            *p++ = *q++;
        }
    }
    TO_IN += i + (i < n);
    *p = '\0';
    if (i > 255)
    {
        p4_throw (P4_ON_PARSE_OVER);
    }
    *DP = i;
    return (p4char *) DP;
}

/** HERE-WORD ( char "name<char>" -- )
 * a FIG-compatible WORD. Where ANSI says "skip leading delimiters"
 * this one acts as "skip leading whitespace". And it will not return
 * anything and have the string parsed to => HERE
 *
 * OLD: was called FIG-WORD up to PFE 0.33.x
 */
FCode (p4_here_word)
{
    p4_here_word ((char) FX_POP);
}

P4_LISTWORDS (your) =
{
    P4_INTO ("EXTENSIONS", 0),
    P4_SNYM ("!>",             "TO"),
    P4_SXco ("@>",		p4_fetch_from),
    P4_SXco ("'>",		p4_tick_from),
    P4_SXco ("INTO",		p4_into),
    P4_SNYM ("&OF",            "INTO"),

    P4_FXco (".H2",		p4_dot_h2),
    P4_FXco ("HERE-WORD",	p4_here_word),
};
P4_COUNTWORDS (your, "YOUR kernel extensions");

/*@}*/

