/** 
 * -- user-supplied additional primitives
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 5.13 %
 *    (%date_modified: Mon Mar 12 10:33:12 2001 %)
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
"@(#) $Id: your-ext.c,v 0.31 2001-05-12 18:15:46 guidod Exp $";
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
 *	CO (NAME, c-name)	a primitive, not immediate
 *	CI (NAME, c-name)	an immediate primitive
 *	CS (NAME, c-name)	a primitive with separate execution
 *				and compilation semantics, see examples
 *				in core.c (if, case etc.) and macros in
 *				p4_comp.h and -- good luck :-)
 *	OV (NAME)		a normal variable
 *	OC (NAME, value)	a normal constant
 *	OL (NAME, value)	a normal value
 *	IV (NAME)		an immediate variable
 *	IC (NAME, value)	an immediate constant
 *	IL (NAME, value)	an immediate value
 *
 * Finally, add a description with P4_COUNTWORDS and you can then let
 * it be loaded at startup by modifying the PFE startup loadlist in
 * pfe-words.c, and loaded wordsets get visible in the LOADED vocabulary.
 */
 
#define _P4_SOURCE 1
#include <pfe/pfe-base.h>
#include <pfe/def-xtra.h>

#include <string.h>
#include <ctype.h>
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
FCode (p4_fetch_from_execution)
{
    FX_PUSH ( *P4_POP_(p4cell*,IP));  
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
        p4xt xt;
        p4_tick (&xt);
        FX_PUSH (*TO_BODY(xt));
    }else{
        char* p; p4ucell l;
        char* x; 
        p4_parseword (' ', &p, &l);
        if ((x = p4_find (p, l))) 
        {
            if (*p4_name_from (x) != p4_constant_RT_ ) 
            {
                FX_COMMA (p4_name_from (x));
            }else{
                FX_COMPILE (p4_fetch_from);
                FX_COMMA (TO_BODY (p4_name_from (x)));
            }
            return;
        }
        if (!p4_compile_local (p,l)) 
        {
            p4_hereword (p,l);
            p4_throw (P4_ON_UNDEFINED);
        }
    }
}
P4COMPILES (p4_fetch_from, p4_fetch_from_execution, 
  P4_SKIPS_CELL, P4_DEFAULT_STYLE);
        
/** !> ( value [name] -- )
 * actually a synonym for => TO but very common amongst
 * forth interpreters
 */
FCode (p4_store_to)
{
    /* copied from to, to help debugger */
    extern FCode (p4_to);
    FX (p4_to); /* SEE will show TO */
}

/* these are mostly copied from p4_to in core.c */        
        
/** ((INTO))
 * execution compiled by => INTO
 */
FCode (p4_into_execution)
{
    FX_PUSH (TO_BODY (P4_POP(IP)));
}

/** ((INTO-)) ( -- value ) 
 * execution compiled by => INTO
 */
FCode (p4_into_local_execution)
{
    FX_PUSH (&LP [ P4_POP(IP) ]);
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
    char *p;
    int l, n;
    p4xt xt;
    
    if (STATE)
    {
        p = p4_word (' ');
        l = *(p4char *) p++;
        if (PFE.locals && (n = p4_find_local (p, l)) != 0)
	{
            FX_COMPILE2 (p4_into);
            FX_COMMA (n);
	}else{
            if ((p = p4_find (p, l)) == NULL)
	           p4_throw (P4_ON_UNDEFINED);
            FX_COMPILE1 (p4_into);
            FX_COMMA (p4_name_from (p));
	}
    }else{
        p4_tick (&xt);
        FX_PUSH (TO_BODY (xt));
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
    p4cell val = FX_POP;
    if (val < 0x100)
        p4_outf ("%02x", val);
    else if (val < 0x10000)
        p4_outf ("%04x", val);
    else if (val < 0x1000000)
        p4_outf ("%06x", val);
    else 
        p4_outf ("%08x", val);
}

/* ------------------------------------------------------------- */

p4char*
p4_fig_word (char del)
{
    char *p, *q;
    int n, i;

    p4_skip_delimiter (' '); /* <= the only different to p4_word */
    p4_source (&q, &n);
    q += TO_IN;
    n -= TO_IN;
    p = (char *) DP + 1;
    if (del == ' ')
    {
        for (i = 0; 
             i < n && !(isascii (*q) && isspace ((unsigned char) *q)); i++)
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

/** FIG-WORD ( char "name<char>" -- )
 * a FIG-compatible WORD. Where ANSI says "skip leading delimiters"
 * this one acts as "skip leading whitespace".
 */
FCode (p4_fig_word)
{
    p4_fig_word ((char) FX_POP);
}
    
P4_LISTWORDS (your) =
{
    CS ("@>",             p4_fetch_from),
    CI ("!>",             p4_store_to), 
    CS ("'>",		  p4_tick_from),
    CS ("INTO",		  p4_into),

    CO (".H2",		  p4_dot_h2),
    CO ("FIG-WORD",       p4_fig_word),
};
P4_COUNTWORDS (your, "YOUR kernel extensions");

/*@}*/

