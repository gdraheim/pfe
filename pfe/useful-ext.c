/** 
 * -- useful additional primitives
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.6 $
 *     (modified $Date: 2008-05-01 19:54:37 $)
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
"@(#) $Id: useful-ext.c,v 1.6 2008-05-01 19:54:37 guidod Exp $";
#endif
 
#define _P4_SOURCE 1
#include <pfe/pfe-base.h>
#include <pfe/def-limits.h>

#include <pfe/os-string.h>
#include <pfe/os-ctype.h>

#include <pfe/dict-sub.h>
#include <pfe/logging.h>
#include <pfe/def-restore.h>

/** 
 * see => >COMPILE  and => POSTPONE
 */
void p4_to_compile (p4xt xt)
{
    if (!xt) return;
    if (STATE && ! P4_NFA_xIMMEDIATE(p4_to_name (xt)))
        FX_XCOMMA (xt);
    else
        p4_call (xt);
}

/** "_for:COMPILE," ( xt -- )
 *  does the work of => POSTPONE on the execution token that 
 *  you got from somewhere else - so it checks if the name
 *  (that correspond to the execution-token argument) is
 *  actually immediate, so it has to be executed to compile
 *  something, e.g. => IF or => THEN - see also => POSTPONE ,
 *  => COMPILE , => [COMPILE] , => INTERPRET
 *
 *  warning: do not use this word anymore, it is an error to
 *  compile a token where the immediate-bit must be checked.
 *  The immediate-bit is a pure header information not present
 *  with headerless words as such. Furthermore, this function
 *  is subject to POSTPONE problems as well.
 *
 *  Newer code should use =>"COMPILE," when trying to simulate
 *  a behavior similar to => POSTPONE.
 */
FCode (p4_to_compile)
{
    p4_to_compile ((p4xt) FX_POP);
}

#define P4_PAREN_MAGIC P4_MAGIC_('P','(',')','X')

extern FCode (p4_tick);
/** "($" ( [word] -- cs-token ) compile-only
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
    FX_PUSH (P4_PAREN_MAGIC);
}

/** ")" ( cs-token -- )
 * takes the execution-token from => ($ and compiles
 * it using => >COMPILE
 */
FCode (p4_prefix_end)
{
    p4_Q_pairs (P4_PAREN_MAGIC);
    FX (p4_to_compile);
}

/** "))" ( cs-token cs-token -- )
 * takes two execution-tokens from two of => ($ and compiles
 * them on after another using => >COMPILE
 simulate:
    : )) [COMPILE] ) [COMPILE] ) ; IMMEDIATE
 */
FCode (p4_prefix_end_doubled)
{
    p4_Q_pairs (P4_PAREN_MAGIC);
    FX (p4_to_compile);
    p4_Q_pairs (P4_PAREN_MAGIC);
    FX (p4_to_compile);
}
  
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
    p4_char_t formbuf[255];
    p4_char_t* formed;
    p4_char_t* format;
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
            printed = sprintf (s, (const char*) formbuf, 
              argv[0], argv[1], argv[2], argv[3], 
              argv[4], argv[5], argv[6], argv[7]);
        }else{
#         ifdef P4_UPPER_REGS /* i960 */
            P4_CALLER_MKSAVED
#         endif
            printed = sprintf (s, (const char*) formbuf, 
              argv[0], argv[1], argv[2], argv[3], 
              argv[4], argv[5], argv[6], argv[7],
              argv[8], argv[9], argv[10], argv[11], 
              argv[12], argv[13], argv[14], argv[15]);
            
#         ifdef P4_UPPER_REGS
            P4_CALLER_RESTORE
#         endif
            if (argn > 13)
            { P4_warn1 ("quite many args for a printf (%i)", argn); }
        }
        if (printed > 255)
        { P4_warn1 ("printf long string (%i chars)", printed); }
        return printed;
    }
}

/** PFE-SPRINTF ( args ... format$ dest$ -- len-dest ) 
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
    register void* format = (void*) FX_POP; 
    FX_PUSH (p4sprintf (format));
}

/** PFE-PRINTF ( args ... format$ -- )
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
{   FX_USE_BODY_ADDR {
    p4_byte_t* dp = (p4_byte_t*) FX_POP_BODY_ADDR[0];

    p4_forget (dp);
}}

/** LOADF ( "filename" -- )
 *  loads a file just like => INCLUDE but does also put
 *  a => MARKER in the => LOADED dictionary that you can
 *  do a => FORGET on to kill everything being loaded
 *  from that file.
 */
FCode (p4_loadf)
{
    /* can not use p4_pocket because included file might use it as well */
    char* filename[POCKET_SIZE];
    p4_byte_t*    dp = DP;
    p4_charbuf_t* fn = p4_word(' ');
    
    p4_store_c_string (P4_CHARBUF_PTR(fn), P4_CHARBUF_LEN(fn), 
		       filename, POCKET_SIZE);
    
    if (p4_included1 (P4_CHARBUF_PTR(fn), P4_CHARBUF_LEN(fn), 1))
        p4_forget_word ("%s", (p4cell)filename, p4_forget_loadf, (p4cell)dp);
}

static p4_namebuf_t*
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
        p4_namebuf_t* nfa = wl->thread[i];
        while (nfa)
        {
            p4xt cfa = p4_name_from(nfa);
            if (*P4_TO_CODE(cfa) == p4_forget_loadf
              &&  cfa > xt && xt > *(p4xt*)P4_TO_BODY(cfa)) 
                return nfa;
            
            nfa = *p4_name_to_link(nfa);
        }
    }
    return 0;
}

/** "(LOADF-LOCATE)" ( xt -- nfa )
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
    if ((xt = p4_tick_cfa (FX_VOID)))
    {
        p4_namebuf_t* nfa = p4_loadf_locate(xt);
        if (nfa) p4_outf("%.*s", NAMELEN(nfa), NAMEPTR(nfa));
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

/** "(;AND)" ( -- )
 * compiled by => ;AND
 */
FCode_XE (p4_semicolon_and_execution)
{
    FX_USE_CODE_ADDR_UNUSED;
    FX (p4_semicolon_execution);
    /* cannot use it in P4COMPILES directly, since it would prevent
       decompiler from acting on at that place 
    */
    FX_USE_CODE_EXIT_UNUSED;
}

#if !defined PFE_SBR_CALL_THREADING
#define FX_COMPILE1_p4_semicolon_and FX_COMPILE(p4_semicolon_and)
#define FX_COMPILE2_p4_semicolon_and FX_COMPILE(p4_semicolon_and)
#else
#define FX_COMPILE1_p4_semicolon_and FX_COMPILE1_p4_semicolon
#define FX_COMPILE2_p4_semicolon_and FX_COMPILE2_p4_semicolon
#endif

/* the 3 in MAKE-begin style is invariant, see debug.c */
#define P4_MAKE0_STYLE 3, 1, 0,  0, 4  /* almost like IF */
#define P4_MAKE1_STYLE 1, 0, -4, 1, 0  /* almost like THEN */

/** ";AND" ( -- )
 * For the code piece between => MAKE and => ;AND , this word
 * will do just an => EXIT . For the code outside of
 * the => MAKE construct a branch-around must be resolved then.
 */                
FCode (p4_semicolon_and)
{
    /* almost a copy of FX(p4_semicolon); */
    extern FCode (p4_store);

    p4_Q_pairs (P4_MAKE_MAGIC);
    PFE.state = FX_POP;
    PFE.locals = (void*) FX_POP;
    PFE.semicolon_code = (void*) FX_POP;

    if (PFE.locals)
    {
        FX_COMPILE2_p4_semicolon_and;
        PFE.locals = NULL;
    }else{
        FX_COMPILE1_p4_semicolon_and;
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

/** "((MAKE-))" ( -- )
 * compiled by => MAKE
 */
FCode_XE (p4_make_to_local_execution)
{
    FX_USE_CODE_ADDR;
    FX_PUSH (IP+2);             /* push following colon-RT, ie. CFA */
    FX (p4_to_local_execution); /* let TO put it into local */
    FX_BRANCH;                  /* and branch over */
    FX_USE_CODE_EXIT;
}

/** "((MAKE))" ( -- )
 * compiled by => MAKE
 */
FCode_XE (p4_make_to_execution)
{
    extern FCode(p4_is_execution);
    FX_USE_CODE_ADDR;
    FX_PUSH (IP+2);             /* push following colon-RT, ie. CFA */
    FX (p4_is_execution);       /* let IS put it into defer */
    FX_BRANCH;                  /* and branch over */
    FX_USE_CODE_EXIT;
}

/** DOER ( word -- )
 * In PFE it is a synonym to => DEFER which a semistandard word.
 * Unlike =>"DEFER", the =>"DOER"-vector was set with an a small
 * piece of code between =>"MAKE" and =>";AND". The "DOER"-word
 * should be replaced with =>"DEFER" =>"IS", which is easy since
 * the =>"DEFER" and =>"DOER" point to the same internal runtime.
 */
extern void FXCode (p4_defer);

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
            FX_UCOMMA (n);
        }else{
            FX_COMPILE1(p4_make);
            FX_XCOMMA (xt);
        }
        FX (p4_forward_mark);  /* third token is empty, filled at ";and"  */
    } else {
        xt = p4_tick_cfa (FX_VOID);
        * (p4xt*) P4_TO_DOES_BODY(xt) = (p4xt) PFE.dp; 
        /* so DEFER points to colon_RT now */
    }
    FX_RCOMMA (PFX(p4_colon_RT)); /* the implicit CFA that we need */
    FX_PUSH (PFE.semicolon_code); PFE.semicolon_code = PFX(p4_semicolon_and);
    FX_PUSH (PFE.locals); PFE.locals = NULL;
    FX_PUSH (PFE.state); PFE.state = P4_TRUE;
    FX_PUSH (P4_MAKE_MAGIC);
}
P4COMPILES2(p4_make, p4_make_to_execution, p4_make_to_local_execution,
        	P4_SKIPS_TO_TOKEN, P4_MAKE0_STYLE);

/**@}*/

static P4_CODE_RUN(p4_offset_RT_SEE)
{
    p4_strcat (p, p4_str_dot (*P4_TO_BODY (xt), p+200, BASE));
    p4_strcat (p, "OFFSET: ");
    p4_strncat (p, (char*) NAMEPTR(nfa), NAMELEN(nfa));
    return 0;
}

/** OFFSET-RT ( value -- value+offset )
 *  this runtime will add the body-value to the value at top-of-stack.
 *  used heavily in structure access words, compiled by => /FIELD
 */
FCode_RT (p4_offset_RT)
{
    FX_USE_BODY_ADDR;
    *SP += FX_POP_BODY_ADDR[0];
}

/** +CONSTANT ( offset "name" -- )
 * create a new offsetword. The word is created and upon execution
 * it adds the offset, ie. compiling the => OFFSET-RT runtime:
       ( address -- address+offset )
 * This word is just a convenience word, just use the word => +FIELD 
 * directly and choose a => DROP to flag the end of a current
 * offset-field declaration series. See also => /FIELD series to
 * declare simple structures which end with a final => CONSTANT to
 * memorize the complete size. The => /FIELD style is more traditional.
 */
FCode (p4_offset_constant)
{
    FX_RUNTIME_HEADER;
    FX_RUNTIME1 (p4_offset_constant);
    FX_UCOMMA (*SP); FX_DROP;
}
P4RUNTIMES1_(p4_offset_constant, p4_offset_RT, 0, p4_offset_RT_SEE);

/** +FIELD ( offset "name" -- offset )
 * created a new name with an => OFFSET-RT runtime using the given offset. 
 * Leave the offset-value untouched, so it can be modified with words
 * like => CHAR+ and => CELL+ and => SFLOAT+ ; This word is the simplest way 
 * to declared structure access words in forth - the two => STRUCT modules 
 * contain a more elaborate series of words. Use this one like:
 0                        ( a fresh definition is started )
 +FIELD zapp.a+ CHAR+     ( zero offset from the base of the struct )
 +FIELD zapp.b+ CELL+     ( no alignment, starts off at 1 from base )
 +FIELD zapp+   DROP      ( store size of complete zap structure )

 0 zapp+                  ( extend the zap structure )
 +FIELD zappx.c+ CELL+    ( a new field )
 +FIELD zappx+   DROP     ( and save it again )

 CREATE zapp1  0 zapp+ ALLOT ( a way to allocate a strucutre )

 zapp2 zapp.b+ @         ( read a value from the field )
 16 zapp2 zapp.b+ !      ( store a value in there )

 * this form is not the traditional form used in forth, it is however
 * quite simple. Use the simplefield declaration with => /FIELD to
 * be compatible with traditional styles that build on top of sizeof
 * constants in forth (which are not part of the ANS Forth standard).
 */
FCode (p4_plus_field)
{
    FX_RUNTIME_HEADER;
    FX_RUNTIME1 (p4_offset_constant);
    FX_UCOMMA (SP[0]);
}

/** /FIELD ( offset size "name" -- offset+size )
 * created a new => +FIELD name with an => OFFSET-RT
 * of offset. Then add the size value to the offset so that
 * the next => /FIELD declaration will start at the end of the
 * field currently declared. This word is the simplest way to
 * declared structure access words in forth - the two => STRUCT modules 
 * contain a more elaborate series of words. This one is used like:
 0                        ( a fresh definition is started )
 /CHAR /FIELD ->zapp.a    ( zero offset from the base of the struct )
 /CELL /FIELD ->zapp.b    ( no alignment, starts off at 1 from base )
 CONSTANT /zapp           ( store size of complete zap structure )

 /zapp                    ( extend the zap structure )
 /CELL /FIELD ->zappx.c   ( a new field )
 CONSTANT /zappx          ( and save it again )

 CREATE zapp1 /zapp ALLOT ( a way to allocate a strucutre )
 /zapp BUFFER: zapp2      ( another way to do it, semi-standard )

 zapp2 ->zapp.b @         ( read a value from the field )
 16 zapp2 ->zapp.b !      ( store a value in there )

 * compare also with => /CHAR => /WCHAR => /CELL => /DCELL
 * and use => +FIELD as the lowlevel word, can simulate as
 : /FIELD SWAP +FIELD + ;
 */
FCode (p4_slash_field)
{
    FX_RUNTIME_HEADER;
    FX_RUNTIME1 (p4_offset_constant);
    FX_UCOMMA (SP[1]);
    SP[1] += SP[0]; FX_DROP;
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
    register p4char* nfa = 0;
    register p4_Wordl* wl;

    for (wl = VOC_LINK; wl; wl = wl->prev)
    {
        p4char * n;
        int i;
        for (i = THREADS; --i >= 0; )
        {
            for (n = wl->thread[i]; n; )
            {
                if (nfa < P4_NAMESTART(n) && P4_NAMESTART(n) < (p4char*) adr)
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
    register p4char* nfa = PFE.dp;
    register p4_Wordl* wl;

    for (wl = VOC_LINK; wl; wl = wl->prev)
    {
        p4char * n;
        int i;
        for (i = THREADS; --i >= 0; )
        {
            for (n = wl->thread[i]; n; )
            {
                if (nfa > P4_NAMESTART(n) && P4_NAMESTART(n) > (p4char*) adr)
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

    xt = (p4cell*) p4_tick_cfa (FX_VOID);
    xt = p4_to_body((p4xt)xt); /* body for _colon_RT */
    ex = (p4cell*) p4_nexthigherNFA(xt);

    n  = FX_POP;
    fr = FX_POP;
    to = FX_POP;
    if (! n) return;
    for ( ; xt < ex-1; xt++)
    {
        if (*xt == fr) 
        {
            --n;
            if (! n) { *xt = to; return; }
            if (n < 0) *xt = to; 
        }
    }
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
    register p4_byte_t *ps, *p;
    register const p4_char_t* src;
    register p4ucell len, i, pc;
    register unsigned int val;
    
    if (STATE) { FX_COMPILE (p4_x_quote); p = DP;  } 
    else { p = p4_pocket (); }
    
    p4_word_parse ('"'); *DP=0; /* PARSE-NOHERE */
    src = PFE.word.ptr;
    len = PFE.word.len;
    
    ps = p+1; pc = 0;
    
    i = 0;
    while (i < len)
    {
        while (src[i] == ' ' && i < len)  i++; /* skip whitespace */
        if (i >= len) break;
        
        val = hexval (src[i]); i++;
        if (i < len && src[i] != ' ')
        {
            val <<= 4; val |= hexval (src[i]);
            i++;
        }
        
        *ps++ = val; pc++; /* store on dest, pc is the count stored */
    }
    
    *p = pc; /* set count byte */
    
    if (STATE) { DP += pc + 1;  FX (p4_align); }
    else { FX_PUSH ((p4cell) p); }
}
extern FCode (p4_c_quote_execution);
P4COMPILES (p4_x_quote, p4_c_quote_execution,
            P4_SKIPS_STRING, P4_DEFAULT_STYLE);


/* ------------------------------------------------------------------- */
/** EVALUATE-WITH ( i*x addr len xt[i*x--j*x] -- j*x ) 
 * added to be visible on the forth command line on request by MLG,
 * he has explained the usage before a lot, you can get an idea from:
    : EVALUATE ['] INTERPRET EVALUATE-WITH ;
 * The word is used internally in PFE for the loadlist evaluation of
 * the binary modules: where previously each loadercode had its own
 * CREATE-execution we do now call the original forthish CREATE-word
 * like, so bootstrapping a => VARIABLE will now call VARIABLE itself
 * and of course we need to set up the TIB-area to point to the name
 * of the variable that shall be created in the forth dictionary:
 : LOAD-WORD ( arg-value str-ptr str-len loader-code -- )
      CASE
        #LOAD-VARIABLE OF ['] VARIABLE EVALUATE-WITH ENDOF
        ....
      ENDCASE
      CLEARSTACK
 ;
 */
FCode(p4_evaluate_with)
{
    /* compare with p4_evaluate() in engine-sub !! */
    p4xt xt = (void*) SP[0];
    p4cell n = SP[1];
    p4char* p = (void*) SP[2];
    FX_3DROP;

#  if !defined P4_RP_IN_VM
    Iframe saved;
    p4_link_saved_input (&saved);
#  else
    RP = (p4xcode **) p4_save_input (RP);
#  endif
    SOURCE_ID = -1;
    BLK = 0;
    TIB = p;                /* leave that warning for a while... */
    NUMBER_TIB = n;
    TO_IN = 0;
    p4_call(xt);           /* <-- the only real difference over p4_evaluate */
#  if defined P4_RP_IN_VM
    RP = (p4xcode **) p4_restore_input (RP);
#  else
    p4_unlink_saved_input (&saved);
#  endif
}

/* ------------------------------------------------------------------- */

/** [VOCABULARY] ( "name" -- )
 * create an immediate vocabulary. Provides for basic 
 * modularization.
 : [VOCABULARY] VOCABULARY IMMEDIATE ;
 */
FCode (p4_bracket_vocabulary)
{
    extern FCode (p4_vocabulary);

    FX (p4_vocabulary);
    P4_NFA_FLAGS(LAST) |= P4xIMMEDIATE;
}

/** [POSSIBLY] ( [name] -- ?? )
 * check if the name exists, and execute it immediatly
 * if found. Derived from POSSIBLY as seen in other forth systems.
 : [POSSIBLY] (') ?DUP IF EXECUTE THEN ; IMMEDIATE
 */
FCode (p4_bracket_possibly)
{
    p4xt cfa;
    p4_charbuf_t* p = p4_word (' ');
    if (! p) return;
    p = p4_find (P4_CHARBUF_PTR(p), P4_CHARBUF_LEN(p));
    if (! p) return;
    cfa = p4_name_from (p);
    if (! cfa) return;
    PFE.execute (cfa);
}

/** [DEF] ( -- )
 * immediatly set topmost => CONTEXT voc to => CURRENT compilation voc.
 : DEF' CURRENT @ CONTEXT ! ; IMMEDIATE
 * note that in PFE most basic vocabularies are immediate, so that
 * you can use a sequence of
 FORTH ALSO  DEFINITIONS
 [DEF] : GET-FIND-3  [ANS] ['] FIND  [FIG] ['] FIND  [DEF] ['] FIND ;
 * where the first wordlist to be searched via the search order are
 * [ANS] and [FIG] and FORTH (in this order) and which may or may not 
 * yield different flavours of the FIND routine (i.e. different XTs)
 */
FCode (p4_bracket_def)
{
    CONTEXT[0] = CURRENT;
}

/** CONTEXT? ( -- number )
 * GET-CONTEXT and count how many times it is in the order but
 * the CONTEXT variable itself. The returned number is therefore
 * minus one the occurences in the complete search-order.
 * usage:
   ALSO EXTENSIONS CONTEXT? [IF] PREVIOUS [THEN]
   ALSO DEF' DEFAULT-ORDER
 : CONTEXT? 
   0 LVALUE _count
   GET-ORDER 1- SWAP  LVALUE _context
   0 ?DO _context = IF 1 +TO _count THEN LOOP
   _count
 ;
 */
FCode (p4_context_Q)
{
    Wordl **p, **q;
    p4cell cnt = 0;

    p = CONTEXT; q= p+1;
    for (q = p+1; q <= &ONLY ; q++)
        if (*p == *q) cnt++;

    FX_PUSH(cnt);
}

/** DEFS-ARE-CASE-SENSITIVE ( -- ) 
 * accesses => CURRENT which is generally the last wordlist that the
 * => DEFINITIONS shall go in. sets there a flag in the vocabulary-definition
 * so that words are matched case-sensitive. 
 example: 
    VOCABULARY MY-VOC  MY-VOC DEFINITIONS DEFS-ARE-CASE-SENSITIVE
 */
FCode (p4_defs_are_case_sensitive)
{
    if (! CURRENT) return;
    CURRENT->flag &=~ WORDL_NOCASE ; 
}

/** CASE-SENSITIVE-VOC ( -- ) 
 * accesses => CONTEXT which is generally the last named => VOCABULARY .
 * sets a flag in the vocabulary-definition so that words are matched
 * case-sensitive. 
 example: 
    VOCABULARY MY-VOC  MY-VOC CASE-SENSITIVE-VOC
 * OBSOLETE! use => DEFS-ARE-CASE-SENSITIVE
 */
FCode (p4_case_sensitive_voc)
{
    if (! CONTEXT[0]) return;
    CONTEXT[0]->flag &=~ WORDL_NOCASE ; 
}

/** DEFS-ARE-SEARCHED-ALSO ( -- )
 * binds => CONTEXT with =>'CURRENT'. If the => CURRENT => VOCABULARY is in
 * the search-order (later), then the => CONTEXT vocabulary will 
 * be searched also. If the result of this word could lead into 
 * a recursive lookup with => FIND it will throw <c>CURRENT_DELETED</c>
 * and leave the => CURRENT => VOCABULARY unaltered.
 example:
 * MY-VOC DEFINITIONS  MY-VOC-PRIVATE DEFS-ARE-SEARCHED-ALSO
 */
FCode (p4_defs_are_searched_also)
{
    if (! CONTEXT[0] || ! CURRENT) return;
    { /* sanity check -> CURRENT may not be part of CONTEXT also-chain */
        register Wordl* wl; 
        for (wl = CONTEXT[0]; wl; wl=wl->also) 
            if (wl == CURRENT) p4_throw (P4_ON_CURRENT_DELETED);  
    }
    CURRENT->also = CONTEXT[0] ; 
}

/** SEARCH-ALSO-VOC ( -- )
 * OBSOLETE!! use DEFS-ARE-SEARCHED-ALSO
 */

/** [EXECUTE] ( [word] -- )
 * ticks the following word, and executes it - even in compiling mode.
 : [EXECUTE] ' EXECUTE ;
 */
FCode (p4_bracket_execute)
{
    p4_call (p4_tick_cfa(FX_VOID));
}

/** !NO ( -- false )
 * a synonym for => FALSE
 !NO SMART-WORDS!
 */

/** !USE ( -- true )
 * a synonym for => TRUE
 !USE SMART-WORDS!
 */

P4_LISTWORDS (useful) =
{
    P4_INTO ("EXTENSIONS", 0),
    P4_FXco ("_like:COMPILE,",		p4_to_compile),
    P4_xOLD (">COMPILE",		"_like:COMPILE,"),
    P4_IXco ("($",			p4_prefix_begin),
    P4_IXco (")",			p4_prefix_end),
    P4_FXco ("PFE-PRINTF",		p4_printf),
    P4_FXco ("PFE-SPRINTF",		p4_sprintf),
    P4_xOLD ("PRINTF",			"PFE-PRINTF"),
    P4_xOLD ("SPRINTF",			"PFE-SPRINTF"),
    P4_FXco ("LOADF",			p4_loadf),
    P4_FXco ("DOER",			p4_defer),
    P4_SXco ("MAKE",			p4_make),
    P4_SXco (";AND",			p4_semicolon_and),
    P4_IXco ("[NOT]",			p4_bracket_not),

    P4_RTco ("+CONSTANT",		p4_offset_constant),  
    P4_FNYM ("FIELD-OFFSET",		"+CONSTANT"),  
    P4_FNYM ("OFFSET:",			"+CONSTANT"),  
    P4_FXco ("+FIELD",                  p4_plus_field),
    P4_FXco ("/FIELD",                  p4_slash_field),
    P4_OCoN ("/CHAR",                   sizeof(p4char)),
    P4_OCoN ("/WCHAR",                  sizeof(short)),

    P4_FXco ("REPLACE-IN",		p4_replace_in),
    P4_FXco ("(LOADF-LOCATE)",		p4_paren_loadf_locate),
    P4_FXco ("LOADF-LOCATE",		p4_loadf_locate),
# ifdef PFE_WITH_FIG
    P4_OCoN ("#WITH-FIG",		PFE_WITH_FIG+100),
# endif
# ifdef WITH_NO_FFA
    P4_OCoN ("#WITH-NO-FFA",		WITH_NO_FFA+100),
# endif
    P4_SXco ("X\"",			p4_x_quote),
    P4_FXco ("EVALUATE-WITH",           p4_evaluate_with),
    P4_IXco ("[POSSIBLY]",		p4_bracket_possibly),
    P4_FXco ("[VOCABULARY]",		p4_bracket_vocabulary),
    P4_IXco ("[DEF]",			p4_bracket_def),
    P4_xOLD ("VOCABULARY'",		"[VOCABULARY]"),
    P4_iOLD ("DEF'",			"[DEF]"),
    P4_FXco ("CONTEXT?",		p4_context_Q),
    P4_FXco ("CASE-SENSITIVE-VOC",      p4_case_sensitive_voc),
    P4_FXco ("DEFS-ARE-CASE-SENSITIVE", p4_defs_are_case_sensitive),
    P4_FXco ("DEFS-ARE-SEARCHED-ALSO",  p4_defs_are_searched_also),
    P4_xOLD ("SEARCH-ALSO-VOC",         "DEFS-ARE-SEARCHED-ALSO"),
    P4_FNYM ("!NO",			"FALSE"),
    P4_FNYM ("!USE",                    "TRUE"),
    P4_SNYM ("|(",                      "("),
};
P4_COUNTWORDS (useful, "Useful kernel extensions");

/*@}*/

/* 
 * Local variables:
 * c-file-style: "stroustrup"
 * End:
 */
