/** 
 * -- useful additional primitives
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 1.14 %
 *    (%date_modified: Mon Mar 12 10:33:06 2001 %)
 *
 *  @description
 *              This wordset adds some additional primitives that
 *		are useful. The structure of this file follows the
 *              the example in your-ext.c, yet some of the words here
 *              must be bound statically into the main pfe-object to
 *              work out smart and nicely.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: useful-ext.c,v 0.33 2001-05-12 18:15:46 guidod Exp $";
#endif
 
#define _P4_SOURCE 1
#include <pfe/pfe-base.h>
#include <pfe/def-xtra.h>

#include <string.h>
#include <ctype.h>

#include <pfe/logging.h>

/** (IMMEDIATE#) ( -- bit-mask )
 *  returns the bit-mask to check if a found word is immediate
    " my-word" FIND IF >FFA C@ (IMMEDIATE#) AND 
                       IF ." immediate" THEN ELSE DROP THEN
 */
FCode (p4_immediate_bit)
{
    FX_PUSH (P4xIMMEDIATE);
}

/** (SMUDGE#) ( -- bit-mask )
 *  returns the bit-mask to check if a found word is smudge
    " my-word" FIND IF >FFA C@ (SMUDGE#) AND 
                       IF ." smudge" THEN ELSE DROP THEN
 */
FCode (p4_smudge_bit)
{
    FX_PUSH (P4xSMUDGED);
}

/** 
 * see => >COMPILE  and => POSTPONE
 */
void p4_to_compile (p4xt xt)
{
    if (!xt) return;
    if (STATE && !(*_FFA(p4_to_name (xt)) & P4xIMMEDIATE))
        FX_COMMA (xt);
    else
        p4_call (xt);
}

/** >COMPILE ( xt -- )
 *  does the work of => POSTPONE on the execution token that 
 *  you got from somewhere else - so it checks if the name
 *  (that correspond to the execution-token argument) is
 *  actually immediate, so it has to be executed to compile
 *  something, e.g. => IF or => THEN - see also => POSTPONE ,
 *  => COMPILE , => [COMPILE] , => INTERPRET
 */
FCode (p4_to_compile)
{
    p4_to_compile ((p4xt) FX_POP);
}

#define PAREN_MAGIC MAKE_MAGIC('P','(',')','X')

extern FCode (p4_tick);
/** ($ ( [word] -- cs-token ) compile-only
 *  takes the execution token of the following word and
 *  saves it on the compile-stack. The correspondig closing
 *  => ) will then feed it into => >COMPILE - so this pair
 *  of word provides you with a prefix-operation syntax
 *  that you may have been seen in lisp-like languages.
   ($ IF ($ 0= A1 @ )) ($ THEN ." hello " )
 * Note that an opening simple => ( paren is a comment.
 */
FCode (p4_prefix_begin)
{
    FX (p4_Q_comp);
    FX (p4_tick);
    FX_PUSH (PAREN_MAGIC);
}

/** ) ( cs-token -- )
 * takes the execution-token from => ($ and compiles
 * it using => >COMPILE
 */
FCode (p4_prefix_end)
{
    p4_Q_pairs (PAREN_MAGIC);
    FX (p4_to_compile);
}

/** )) ( cs-token cs-token -- )
 * takes two execution-tokens from two of => ($ and compiles
 * them on after another using => >COMPILE
 simulate:
    : )) [COMPILE] ) [COMPILE] ) ; IMMEDIATE
 */
FCode (p4_prefix_end_doubled)
{
    p4_Q_pairs (PAREN_MAGIC);
    FX (p4_to_compile);
    p4_Q_pairs (PAREN_MAGIC);
    FX (p4_to_compile);
}


#ifdef SMART_WORDS

static p4xt
_p4_smart_interpret_char (char c)
{
    auto char wd[] = "interpret- ";
    register char* nfa;
    register p4xt xt;
    
    wd[sizeof(wd)-2] = c;
    nfa = p4_find (wd, sizeof(wd)-1);
    if (!nfa || !(*_FFA(nfa)|P4xIMMEDIATE)) return 0; /* must be immediate */
    xt = p4_name_from (nfa);
    if (*xt != p4_defer_RT_) return 0; /* must be DEFER or DOER */
    return xt;
}

/**
 */
p4xt 
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
char*
_p4_smart_interpret_init (char c, char const * nm, int l)
{
    auto char wd[] = "interpret- ";
    register char* nfa;
    
    wd[sizeof(wd)-2] = c;
    if (!(nfa = p4_find (nm, l))) return 0;
    if (!(*_FFA(nfa)|P4xIMMEDIATE)) return 0;
  
    p4_header_from (p4_defer_RT_, P4xIMMEDIATE, wd, sizeof(wd)-1);
    FX_COMMA (p4_name_from (nfa));
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
FCode (p4_smart_interpret_init)
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

/** SMART-INTERPRET-OFF ( -- )
 * disables the SMART-INTERPRET extension in => INTERPRET ,
 * see => SMART-INTERPRET-INIT
 */
FCode (p4_smart_interpret_off)
{
    PFE.smart_char = NULL;
}

/** SMART-INTERPRET-ON ( -- )
 * enables the SMART-INTERPRET extension in => INTERPRET ,
 * see => SMART-INTERPRET-INIT - the default for smart-interpret 
 * is always off
 */
FCode (p4_smart_interpret_on)
{
    PFE.smart_char = p4_smart_interpret_char;
}

#endif
  /*SMART_WORDS*/
  
/* ----------- output convenience ---------- */
extern FCode (p4_emit);

/** 
 *  printing a forth counted string is done through %#s,
 *  the standard %s is ignored defending against invalid use,
 *  but it may be useful to use %1s to print a real
 *  zeroterminated-string.
 */
static int
p4sprintf (char* s)
{
    p4char formbuf[255];
    p4char* formed;
    p4char* format;
    int format_n;
    int argn = 0;
    p4cell argv[16];
    
    formed = formbuf;
    format = (void*)FX_POP; /* get the argument string */
    format_n = *format++; /* COUNT */
  
    while (format_n > 0)
    {
        if (*format=='%') {
            *formed++ = *format++; format_n--;
            if (*format == '%') {
                *formed++ = *format++; format_n--;
                continue;
            }
            if (*format == 's') {
                /* not sure what a normal string is in this context */
                *formed++ = '%'; /* so it is ignored */
                format++; format_n--;
                FX_DROP;
                continue;
            }
            argv [argn++] = FX_POP; 
            while (format_n > 0)
            {
                if (argn >= 15) goto printnow;
                if (format[0] == '.' && format[1] == '*') {
                    argv [argn++] = FX_POP;
                    *formed++ = *format++; format_n--;
                    *formed++ = *format++; format_n--;
                    continue;
                }
                
                if (format[0] == '#' && format[1] == 's') {
                    p4char* p = (void*) argv [argn-1];
                    argv [argn-1] = *p; argv [argn++] = (p4cell)(p+1); 
                    *formed++ = '.';
                    *formed++ = '*';
                    *formed++ = 's';
                    format+=2; format_n-=2;
                    break;
                }
                
                if (isalpha(*format))
                    break;
                
                *formed++ = *format++; format_n--;
            }
            continue;
        }
        /*else*/
        *formed++ = *format++; format_n--;
    }
 printnow:
    { 
        int printed;
        *formed='\0'; /* should we do it really here ? 
                         or let it do the caller -gud */
        
        if (argn <= 8)
        {
            printed = sprintf (s, formbuf, 
              argv[0], argv[1], argv[2], argv[3], 
              argv[4], argv[5], argv[6], argv[7]);
        }else{
#         ifdef P4_UPPER_REGS /* i960 */
            _p4_thread_save();
#         endif
            printed = sprintf (s, formbuf, 
              argv[0], argv[1], argv[2], argv[3], 
              argv[4], argv[5], argv[6], argv[7],
              argv[8], argv[9], argv[10], argv[11], 
              argv[12], argv[13], argv[14], argv[15]);
            
#         ifdef P4_UPPER_REGS
            _p4_thread_load(); 
#         endif
            if (argn > 13)
            { P4_warn1 ("quite many args for a printf (%i)", argn); }
        }
        if (printed > 255)
        { P4_warn1 ("printf long string (%i chars)", printed); }
        return printed;
    }
}

/** SPRINTF ( args ... format$ dest$ -- len-dest ) 
 * just like the standard sprintf() function in C, but
 * the format is a counted string and accepts %#s to
 * be the format-symbol for a forth-counted string.
 * The result is a zeroterminated string at dest$ having
 * a length being returned. To create a forth-counted
 * string, you could use:
   variable A 256 ALLOT
   15 " example" " the %#s value is %i" A 1+ SPRINTF A C!
   A COUNT TYPE
 */
FCode (p4_sprintf)
{
    FX_PUSH (p4sprintf ((void*)FX_POP));
}

/** PRINTF ( args ... format$ -- )
 * uses => SPRINTF to print to a temporary 256-char buffer
 * and prints it to stdout afterwards. See the example
 * at => SPRINTF of what it does internally.
 */
FCode (p4_printf)
{
    char outbuf[256];
    p4sprintf (outbuf);
    p4_outs (outbuf);
}

/* ---------------- */

void
p4_forget_loadf(void)
{
    char* dp = (char*) WP_PFA[0];

    p4_forget (dp);
}

/** LOADF ( "filename" -- )
 *  loads a file just like => INCLUDE but does also put
 *  a => MARKER in the => LOADED dictionary that you can
 *  do a => FORGET on to kill everything being loaded
 *  from that file.
 */
FCode (p4_loadf)
{
    char filename[NFACNTMAX+1];
    char* dp = DP;
    char* fn = p4_word(' ');
    
    p4_store_c_string (fn+1, *fn, filename, NFACNTMAX+1);
    
    if (p4_included1 (fn + 1, *(p4char *)fn, 1))
        p4_forget_word ("%s", (p4cell)filename, p4_forget_loadf, (p4cell)dp);
}

char*
p4_loadf_locate(p4xt xt)
{
    int i;
    Wordl* wl = PFE.atexit_wl;
    
    /* look for a loadf-marker that is above xt and contains a
       forget address below xt. This should make sure that xt is
       really defined during that LOADF.
    */
    for (i = THREADS; --i >= 0; )
    {
        char* p = wl->thread[i];
        while (p)
        {
            p4xt cfa = p4_name_from(p);
            if (*cfa == p4_forget_loadf
              &&  cfa > xt && xt > *(p4xt*)TO_BODY(cfa)) 
                return p;
            
            p = *p4_name_to_link(p);
        }
    }
    return 0;
}

/** (LOADF-LOCATE) ( xt -- nfa )
 * the implementation of => LOADF-LOCATE
 */
FCode(p4_paren_loadf_locate)
{
    *SP = (p4cell) p4_loadf_locate((p4xt) *SP);
}

/** LOADF-LOCATE ( "name" -- )
 * look for the filename created by => LOADF that had been
 * defining the given name. => LOADF has created a marker
 * that is <em>above</em> the => INCLUDED file and that
 * marker has a body-value just <em>below</em> the 
 * => INCLUDED file. Hence the symbol was defined during
 * => LOADF execution of that file.
 : LOADF-LOCATE ?EXEC POSTPONE ' (LOADF-LOCATE) .NAME ;
 */
FCode(p4_loadf_locate)
{
    p4xt xt;
    FX (p4_Q_exec);
    if(p4_tick(&xt))
    {
        char* nfa = p4_loadf_locate(xt);
        if (nfa) p4_outf("%.*s", *nfa, nfa+1);
        else p4_outs("(unknown)");
    }
}

/* ---------------- */

/**@name MAKE-words
 * this make-implementation is quite different from the usual 
 * doer..make implementation. Actually, doer and defer are the
 * same in pfe, ie. make will store the cfa wherever you want it,
 * even in a locals-variable! For that purpose, the make-execution
 * works on the following compiled layout:
   
   +-----------------+----------------+---------------+-----------+
   | (make-exec)-CFA | TO-data-token  | BRANCH-offset | colon-RT  |
   +-----------------+----------------+---------------+-----------+
   
 * note: I had to hack debug.c to work correctly on this. beware.
*/
/**@{*/

/** (;AND)
 * compiled by => ;AND
 */
FCode (p4_semicolon_and_execution)
{
    FX (p4_semicolon_execution);
    /* cannot use it in P4COMPILES directly, since it would prevent
       decompiler from acting on at that place 
    */
}

/* the 3 in MAKE-begin style is invariant, see debug.c */
#define P4_MAKE0_STYLE 3, 1, 0,  0, 4  /* almost like IF */
#define P4_MAKE1_STYLE 1, 0, -4, 1, 0  /* almost like THEN */

/** ;AND ( -- )
 * For the code piece between => MAKE and => ;AND , this word
 * will do just an => EXIT . For the code outside of
 * the => MAKE construct a branch-around must be resolved then.
 */                
FCode (p4_semicolon_and)
{
    /* almost a copy of FX(p4_semicolon); */
    extern FCode (p4_store);

    P4_fail ("make-check\n");
    p4_Q_pairs (P4_MAKE_MAGIC);
    PFE.state = FX_POP;
    PFE.locals = (void*) FX_POP;
    PFE.semicolon_code = (void*) FX_POP;

    if (PFE.locals)
    {
        FX_COMPILE(p4_semicolon_and); /* FX_COMPILE2 (p4_semicolon_and); */
        PFE.locals = NULL;
    }else{
        FX_COMPILE(p4_semicolon_and); /* FX_COMPILE1 (p4_semicolon_and); */
    }

    /*
    if (PFE.semicolon_code)
    {
        PFE.semicolon_code ();
    }else{
    */
        if (PFE.state)
            FX (p4_forward_resolve); /* atleast resolve the branch */
        /*
    }
        */
}
P4COMPILES (p4_semicolon_and, p4_semicolon_and_execution,
        	P4_SKIPS_NOTHING, P4_MAKE1_STYLE);

/** ((MAKE-))
 * compiled by => MAKE
 */
FCode (p4_make_to_local_execution)
{
    FX_PUSH (IP+2);             /* push following colon-RT, ie. CFA */
    FX (p4_to_local_execution); /* let TO put it into local */
    FX_BRANCH;                  /* and branch over */
}

/** ((MAKE))
 * compiled by => MAKE
 */
FCode (p4_make_to_execution)
{
    FX_PUSH (IP+2);             /* push following colon-RT, ie. CFA */
    FX (p4_to_execution);       /* let TO put it into var/defer */
    FX_BRANCH;                  /* and branch over */
}

/** DOER ( word -- )
 * In PFE it is a synonym to => DEFER which a semistandard word.
 * Unlike =>"DEFER", the =>"DOER"-vector was set with an a small
 * piece of code between =>"MAKE" and =>";AND". The "DOER"-word
 * should be replaced with =>"DEFER" =>"IS", which is easy since
 * the =>"DEFER" and =>"DOER" point to the same internal runtime.
 */

/** MAKE ( [word] -- ) ... ;AND
 * make a seperated piece of code between => MAKE and => ;AND 
 * and on execution of the => MAKE the named word is twisted
 * to point to this piece of code. The word is usually 
 * a => DOER but the current implementation works 
 * on => DEFER just as well, just as it does on other words who
 * expect to find an execution-token in its PFA. You could even
 * create a colon-word that starts with => NOOP and can then make
 * that colon-word be prefixed with the execution of the code piece. 
 * This => MAKE
 * does even work on => LOCALS| and => VAR but it is uncertain
 * what that is good for.
 */
FCode (p4_make)
{
    extern int p4_tick_local (p4xt*);
    p4xt xt;
    int n;

    if (STATE) 
    {
        if ((n = p4_tick_local(&xt)))
        {
            FX_COMPILE2(p4_make);
            FX_COMMA (n);
        }else{
            FX_COMPILE1(p4_make);
            FX_COMMA (xt);
        }
        FX (p4_forward_mark);  /* third token is empty, filled at ";and"  */
    } else {
        p4_tick(&xt);
        *(void**)TO_BODY(xt) = PFE.dp; /* let DOER/DEFER point to colon_RT */
    }
    FX_COMMA (p4_colon_RT_); /* the implicit CFA that we need */
    FX_PUSH (PFE.semicolon_code); PFE.semicolon_code = PFX(p4_semicolon_and);
    FX_PUSH (PFE.locals); PFE.locals = NULL;
    FX_PUSH (PFE.state); PFE.state = P4_TRUE;
    FX_PUSH (P4_MAKE_MAGIC);
}
P4COMPILES2(p4_make, p4_make_to_execution, p4_make_to_local_execution,
        	P4_SKIPS_TO_TOKEN, P4_MAKE0_STYLE);

/**@}*/

/** OFFSET: ( offset "name" -- )
 *  an older word for =>"FIELD-OFFSET", please use =>"FIELD-OFFSET"
 *  or =>"FIELD".
 */

/** FIELD-OFFSET ( offset "name" -- )
 * create a new offsetword. The word is created and upon execution
 * it add the offset, ie. compiling runtime:
       ( address -- address+offset )
 */
FCode (p4_field_offset)
{
    extern FCode(p4_offset_RT);

    p4_header (PFX(p4_offset_RT), 0);
    FX_COMMA (*SP); FX_DROP;
}

FCode (p4_offset_RT)
{
    *SP += WP_PFA[0];
}


/** [NOT] ( a -- a' )
 * executes => 0= but this word is immediate so that it does 
 * affect the cs-stack while compiling rather than compiling
 * anything. This is useful just before words like => [IF] to
 * provide semantics of an <c>[IFNOT]</c>. It is most useful in
 * conjunction with "=> [DEFINED] word" as it the sequence
 * "<c>[DEFINED] word [NOT] [IF]</c>" can simulate "<c>[IFNOTDEF] word</c>"
 */
FCode (p4_bracket_not)
{
    extern FCode(p4_zero_equal);
    FX (p4_zero_equal);
}

/* ------------------------- */
p4char*
p4_nextlowerNFA(void* adr)
{
    char* nfa = 0;
    register Wordl* wl;

    for (wl = VOC_LINK; wl; wl = wl->prev)
    {
        char * n;
        int i;
        for (i = THREADS; --i >= 0; )
        {
            for (n = wl->thread[i]; n; )
            {
                if (_FFA(n) > nfa && _FFA(n) < (char*) adr)
                    nfa = n;
                n = *p4_name_to_link(n);
            }
        }
    }
    return nfa;
}

p4char*
p4_nexthigherNFA(void* adr)
{
    char* nfa = PFE.dp;
    register Wordl* wl;

    for (wl = VOC_LINK; wl; wl = wl->prev)
    {
        char * n;
        int i;
        for (i = THREADS; --i >= 0; )
        {
            for (n = wl->thread[i]; n; )
            {
                if (_FFA(n) < nfa && _FFA(n) > (char*) adr)
                    nfa = n;
                n = *p4_name_to_link(n);
            }
        }
    }
    return nfa;
}


/** REPLACE-IN ( to-xt from-xt n "name" -- )
 * will handle the body of the named word as a sequence of cells (or tokens) 
 * and replaces the n'th occurences of from-xt into to-xt. A negative value
 * will change all occurences. A zero value will not change any.
 */
FCode(p4_replace_in)
{
    int n;
    p4cell fr, to;
    p4cell* ex;
    p4cell* xt;

    p4_tick((p4xt*) &xt);
    xt = TO_BODY(xt);
    ex = (p4cell*) p4_nexthigherNFA(xt);

    n  = FX_POP;
    fr = FX_POP;
    to = FX_POP;
    if (!n) return;
    for ( ; xt < ex-1; xt++)
    {
        if (*xt == fr) 
        {
            --n;
            if (!n) { *xt = to; return; }
            if (n < 0) *xt = to; 
        }
    }
}

/* ----------------------------------------------------------------- */
    
extern FCode(p4_defer); /* -> DOER */

/** ALIAS ( xt "name" -- )
 * create a defer word that is initialized with the given x-token.
 */
FCode (p4_alias)
{
    extern FCode(p4_defer_RT);
    p4_header (PFX(p4_defer_RT), 0);
    FX_COMMA (FX_POP);
}

/** ALIAS-ATEXIT ( xt "name" -- )
 * create a defer word that is initialized with the given x-token.
 */
FCode (p4_alias_atexit)
{
    register Wordl* save = CURRENT;
    CURRENT = PFE.atexit_wl;
    FX (p4_alias);
    CURRENT = save;

#ifdef PFE_WITH_FFA
    *_FFA(LAST) |= P4xONxDESTROY;
#endif
}

/** VOCABULARY' ( "name" -- )
 * create an immediate vocabulary. Provides for basic 
 * modularization.
 : VOCABULARY' VOCABULARY IMMEDIATE ;
 */
FCode (p4_vocabulary_tick)
{
    extern FCode (p4_vocabulary);

    FX (p4_vocabulary);
    *_FFA(LAST) |= P4xIMMEDIATE;
}

/* ------------------------------------------------------------------- 
 * hex string
 */

static int hexval (char c)
{
  if (c >= '0' && c <= '9' ) return c - '0';
  if (c >= 'A' && c <= 'Z' ) return c - 'A' + 10;
  if (c >= 'a' && c <= 'z' ) return c - 'a' + 10;
  if (c == '*') return 0xF;
  if (c == '!') return 0x1;
  return 0;
}

/** 'X"' ( "hex-q" -- bstring ) 
 * places a counted string on stack
 * containing bytes specified by hex-string
 * - the hex string may contain spaces which will delimit the bytes
 example: 
    X" 41 42 4344" COUNT TYPE ( shows ABCD )
 */
FCode (p4_x_quote)
{
  char *ps, *p, *q;
  p4ucell n, i, pc;
  unsigned int v;
  
  if (STATE)
  {
    FX_COMPILE (p4_x_quote);
    p = DP;
  } 
  else
  {
    p = p4_pocket ();
  }
  
  p4_parse ('"', &q, &n);
  
  ps = p+1; pc = 0;
  
  i = 0;
  while (i < n)
  {
    while (q[i] == ' ' && i < n)  i++; /* skip whitespace */
    if (i >= n) break;
    
    v = hexval (q[i]); i++;
    if (i < n && q[i] != ' ')
    {
      v <<= 4; v |= hexval (q[i]);
      i++;
    }
    
    *ps++ = v; pc++; /* store on dest, pc is the count stored */
  }
  
  *p = pc; /* set count byte */
  
  if (STATE)
  {
    DP += pc + 1;
    FX (p4_align);
  }
  else
  {
    FX_PUSH ((p4cell) p);
  }
}
extern FCode (p4_c_quote_execution);
P4COMPILES (p4_x_quote, p4_c_quote_execution,
        	P4_SKIPS_STRING, P4_DEFAULT_STYLE);


/* ------------------------------------------------------------------- */

/*forward*/ FCode (p4_boot_script_fetch);
/*forward*/ FCode (p4_boot_script_colon);

P4_LISTWORDS (useful) =
{
    CO ("(IMMEDIATE#)",   p4_immediate_bit),
    CO ("(SMUDGE#)",      p4_smudge_bit),
    CO (">COMPILE", 	  p4_to_compile),
    CI ("($",  		  p4_prefix_begin),
    CI (")",		  p4_prefix_end),
# ifdef SMART_WORDS
    CO ("SMART-INTERPRET-INIT", p4_smart_interpret_init),
    CO ("SMART-INTERPRET-OFF",  p4_smart_interpret_off),
    CO ("SMART-INTERPRET-ON",   p4_smart_interpret_on),
# endif
    CO ("PRINTF",         p4_printf),
    CO ("SPRINTF",        p4_sprintf),
    CO ("LOADF",	  p4_loadf),
    CO ("DOER",           p4_defer),
    CS ("MAKE",           p4_make),
    CS (";AND",           p4_semicolon_and),
    CI ("[NOT]",          p4_bracket_not),
    CO ("FIELD-OFFSET",   p4_field_offset),  
    CO ("OFFSET:",        p4_field_offset),  
    CO ("REPLACE-IN",     p4_replace_in),
    CO ("(LOADF-LOCATE)", p4_paren_loadf_locate),
    CO ("LOADF-LOCATE",   p4_loadf_locate),
    CO ("BOOT-SCRIPT:",   p4_boot_script_colon),
    CO ("BOOT-SCRIPT@",   p4_boot_script_fetch),
# ifdef _K12_SOURCE
    OC ("#K1297-G20",     _K12_SOURCE+100),
# endif
# ifdef PFE_WITH_FIG
    OC ("#WITH-FIG",      PFE_WITH_FIG+100),
# endif
# ifdef WITH_NO_FFA
    OC ("#WITH-NO-FFA",   WITH_NO_FFA+100),
# endif
    CO ("VOCABULARY'",    p4_vocabulary_tick),
    CO ("ALIAS-ATEXIT",   p4_alias_atexit),
    CO ("ALIAS",          p4_alias),
    CS ("X\"",            p4_x_quote),
};
P4_COUNTWORDS (useful, "Useful kernel extensions");

/* -------------------------------------------------------------------- */

#ifdef _K12_SOURCE
#include <pfe/main-k12.h>
#endif

/** BOOT-SCRIPT@ ( -- s-a s-n )
 * the file that will be include on next => COLD boot
 * DO NOT USE! will vanish w/o warning in the next version!
 */
FCode (p4_boot_script_fetch)
{
    if (PFE.set->include_file)
    {
        FX_PUSH (PFE.set->include_file);
        FX_PUSH (strlen (PFE.set->include_file));
    }else{
        FX_PUSH (0);
        FX_PUSH (0);
    }
}

/** BOOT-SCRIPT: ( "string" -- )
 * DO NOT USE! will vanish w/o warning in the next version!
 * see => BOOT-SCRIPT@
 */
FCode (p4_boot_script_colon)
{
    char* str; p4ucell len;
    p4_parseword (' ', &str, &len);

# ifdef _K12_SOURCE
    { 
        p4_emu_t* p4 = P4_K12_EMUL(p4TH);

        strncpy (p4->includes, str, len);
        PFE.set->include_file = p4->includes;
    }
# else
    {
        p4_throw (P4_ON_OBSOLETED);
    }
#endif
}

/* P4_C*UNTWORDS(useful, " is here... "); */

/*@}*/

