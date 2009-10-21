/**
 * -- The Additional StackHelp TypeCheck Word Set
 *
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.5 $
 *     (modified $Date: 2008-05-01 00:42:01 $)
 *
 *  @description
 *    These are routines to add stackchecking capabilities. The
 *    loader routine will hook into the outer interpreter and
 *    _before_ any word is compiled/interpreted it is being
 *    sent through an stackcheck interpreter. The stackcheck
 *    interpreter code is fully independent from the rest of
 *    the code. It will only work on the stackcheck hints given
 *    with each word. In most cases the stackcheck hints are
 *    simple strings which get parsed for information - these
 *    strings may be taken from the usual stack notation of a
 *    a colon word. The parser is detecting the syntax given
 *    in the OpenFirmware recommendations for specifying a compact
 *    stack and parsing behavior of a word. They are expanded for
 *    extra type hints and tracing through splitstack parts.
 *
 *    Other than pure strings, the stackchecking can be done
 *    through code words that work on the checkstack - they are
 *    similar to immediate words in the normal forth interpreter
 *    whereas the stacknotation strings are checkstacked by the
 *    checkstack parser directly instead of blackboxing it
 *    through a call to a routine somewhere. It does however
 *    bring in a lot of flexibility and allows for complex
 *    stackcheck code in extension modules. The core-stk module
 *    is an integral part of the stackcheck behavior for forth
 *    and encompasses stackchecking for IF-ELSE branches and
 *    the various LOOPs and EXIT-points of a ':'-colon word.
 *
 *   implementation notes:
 *     The parsing of a single stack notation ( .... ) follows
 *     these rules:
 *     (a) recognize "changer"s with embedded "--" and seperated
 *            by " |" twochar sequence
 *     (b) cut into input / output effect notation without "--"
 *            with a resulting "notation"
 *     (c) each stack effect may have "variant" notations seperated
 *         by "| " twochar sequence
 *     (d) cut into stack effect parts with the "stack" notation broken
 *          at each "X:" up to the following "Y:" marker. The
 *          resulting notation applies to the specified stack only.
 *     (e) each argument is seperated by whitespace, but an argument
 *         may be broken down further.
 *     (f) an argument may have ellipsis expansion with "..."
 *         for multi-cell notations
 *     (g) an argument may have  a singular "alternative" notation with
 *         a "|" that has no whitespace on either side.
 *     (h) an argument may have  a singular expansion with ","
 *         used for double-cell notations
 *     (i) after seperation as above, each part is broken down
 *         into name-prefix and type-suffix-list where types may be
 *         non-ascii singlechar or ascii-named strings introduced
 *         by a "-", i.e. "a-ptr" is the same as "a*".
 *     (j) a literal may occur ("0"), or self-parsing hint as seen
 *         in traditional forth stack notations ("[text<eol>]")
 *     (x) note the ambiguity of " | ", the changer recognition
 *         is supposed to be greedy, i.e.
 *         0 NARROW-CHANGER( a b -- c d | x y | u v -- m n )
 *                         ..^^^^^^^^^^^^^^^^^..
 *         1 NARROW-CHANGER( a b -- c d | x y | u v -- m n )
 *                         ....................^^^^^^^^^^^^..
 *         1 NARROW-CHANGER( a b -- c d | x y| u v -- m n )
 *                         ..............^^^^^^^^^^^^^^^^^..
 *         1 NARROW-CHANGER( a b -- c d |x y |u v -- m n )
 *                         ..............^^^^^^^^^^^^^^^^..
 *         1 NARROW-CHANGER( a b -- c d| x y| u v -- m n )
 *                [invalid -- no separator for second changer]
 *
 *     (y) a "notation" is a single stack layout definition per changer
 *         which includes the syntaxhint at the start. This scheme allows
 *         to step through user-provided "notation"s in order and "rewrite"
 *         each stack layout without changing the original order. Therefore
 *         we have
 *         0 0 NARROW-INPUT-NOTATION( a b R: x y | u v -- m n )
 *                                    ^^^^
 *         1 0 NARROW-INPUT-NOTATION( a b R: x y | u v -- m n )
 *                                        ^^^^^^^
 *         2 0 NARROW--INPUT-NOTATION( a b R: x y | u v -- m n )
 *                                                ^^^^^^
 *
 * preliminary usage:
 *         start compiling a colon word and add a stack notation with
 *         the help of "|( ..... )". The stackhelp checker will show
 *         which words it did recognize and at the final semicolon (or
 *         any other exitpoint) it will check the resulting stack
 *         layout with the layout given first as the intended stack
 *         notation. Try ": |( a -- a a ) dup dup ;" for a start. And
 *         remember that stackhelp is an external module which you must
 *         load before as "needs stackhelp-ext".
 *
 *         Additionally there are a few helper words, most prominently
 *         you can fetch the stackhelp notation for any registered word
 *         by saying "stackhelp 'wordname'". Note that forth is really
 *         a nontyped language and you are able to add "hint narrowing"
 *         on each portion of a stack item in a postfix notation of
 *         each item. Therefore ": dup |( a -- a a)" will work simply
 *         on anything while ": @ |( a* -- a)" will issue a warning if
 *         you give it "depth @" since the depth word is defined with
 *         a numeric postfix hint by ": depth |( -- value#)".
 *
 *         Furthermore, there are a number of debugging words that can
 *         be used also to interactivly test whether some type narrowing
 *         will work later in practice. Start off with giving a current
 *         stack notation as the input line notation
 *                 REWRITE-LINE( a b 0 | c 1! )
 *         which is a split notation example with differing depths in
 *         each of the possible inputs. You can get back the current
 *         stack notation by saying REWRITE-SHOW. Next one can test a
 *         few changer notations, where changer means anything that
 *         does contain a "--" seperator. One can first test which part
 *         of a multivariant changer would be used by asking with
 *         REWRITE-SELECT( a 0 -- | a 1! -- a ). The same with a
 *         REWRITE-TEST( a 0 -- | a 1! -- a) would say "Ok!" since we
 *         do can apply the changer to the current stack line. Then
 *         we can try with REWRITE-EXPAND( a 0 -- | a 1! -- a) and
 *         REWRITE-RESULT( a 0 -- | a 1! -- a) where the second one
 *         will also try to collapse a splitstack result to a simple
 *         one.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) =
"@(#) $Id: stackhelp-ext.c,v 1.5 2008-05-01 00:42:01 guidod Exp $";
#endif

#define _P4_SOURCE 1
#include <pfe/pfe-base.h>
#include <pfe/def-limits.h>

#include <pfe/os-ctype.h>
#include <pfe/os-string.h>

#include <pfe/def-comp.h>
#include <pfe/logging.h>

#define Q (int)

#define BUFLEN 255

#define STK 'S'
#define NUM2STK(X) ((STK+1)-(X))
#define STKNULL 1 /* allow 'S' to match '' stack part */

#define CHK (*(struct stackhelp*)(PFE.p[slot]))
static int slot = 0;

/* non-int index into array generates gcc warning messages */
#define IS(x) ((int)(x))

typedef struct pair
{
    const char* str;
    const char* end;
} *pair_t;

/* #define pairdef(X,Y) p4_memcpy((X),(Y),sizeof(struct pair)) */
#define pairdef(X,Y) { (X)->str = (Y)->str ; (X)->end = (Y)->end; }
#define pairdef_(X,Y) { (X).str = (Y).str ; (X).end = (Y).end; }

#define pairlen(X) ((X)->end - (X)->str)
#define pairlen_(X) ((X).end - (X).str)

#define INTERPRET_FIND 4
#define INTERPRET_NUMBER 3

struct stackhelp
{
    p4char* last;
    struct { char str[BUFLEN]; char* end; } word, line;
    int depth['Z'-'A']; /* the param stack 'S' is always last/first */
    int shallow;
    char debug[100];
    p4ucell_p4code interpret[8];
    int afters; p4code after[16];
    p4cell notdone;
};

#ifndef SHOWRESULT
#define SHOWRESULT CHK.debug[0]
#endif
#ifndef SHOW
#define SHOW CHK.debug[1]
#endif
#ifndef SHOWTESTS
#define SHOWTESTS CHK.debug[4]
#endif
#ifndef SHOWCOPIES
#define SHOWCOPIES CHK.debug[5]
#endif
#ifndef SHOWSELECT
#define SHOWSELECT CHK.debug[6]
#endif
#ifndef SHOWAFTER
#define SHOWAFTER CHK.debug[7]
#endif

#define X  fprintf (stderr, "%c", *x)
#define XS  fprintf (stderr, "%c.", *x)
#define XC  fprintf (stderr, "%c:", *x)
#define XX fprintf (stderr, "#\n")

#define isSTARTTOKEN(x) ((x) == '"' || (x) == '[' || (x) == '<' || (x) == '{')
#define isENDOFTOKEN(x) ((x) == '"' || (x) == ']' || (x) == '>' || (x) == '}')

static const char* skipback(const char* x, const char* least)
{
    while (x > least && isENDOFTOKEN(*x))
    {
        if (*x == ']' || *x == '}') {
            register int depth = 0;
            do {
                if (*x == ']' || *x == '}') depth++;
                if (*x == '[' || *x == '{') depth--;
                if (*x == '>') {
                    x--;
                    while (x > least && *x != '<' && *x != '>') x--;
                }
                if (*x == '"') {
                    x--;
                    while (x > least && *x != '"') x--;
                }
                x--;
            } while (x > least && depth);
        }
        if (*x == '>') {
            x--;
            while (x > least && *x != '<' && *x != '>') x--;
        }
        if (*x == '"') {
            x--;
            while (x > least && *x != '"') x--;
        }
    }
    return x;
}

static const char* skipnext(const char* x, const char* above)
{
    while (x < above && isSTARTTOKEN(*x))
    {
        if (*x == '[' || *x == '{') {
            register int depth = 0;
            do {
                if (*x == '[' || *x == '{') depth++;
                if (*x == ']' || *x == '}') depth--;
                if (*x == '<') {
                    x++;
                    while (x < above && *x != '<' && *x != '>') x++;
                }
                if (*x == '"') {
                    x++;
                    while (x < above && *x != '"') x++;
                }
                x++;
            } while (x < above && depth);
        }
        if (*x == '<') {
            x++;
            while (x < above && *x != '<' && *x != '>') x++;
        }
        if (*x == '"') {
            x++;
            while (x < above && *x != '"') x++;
        }
    }
    return x;
}

static const char* find_changer (const char* s, const char* x)
{
    /*      : x |( a -- a a ) dup ;         */
    while (s < --x)
    {
        if (x[0] == '-' && x[-1] == '-')
        {
            return x;
        }
    }
    return 0;
}

static int stack_depth (const char* s, const char* x, unsigned char stk)
{
    int depth = 0;
    while (x >= s)
    {
        do { x--; } while (x >= s && p4_isspace(*x));
        if (isENDOFTOKEN(*x)) x = skipback (x, s);
        if (x >= s && *x == '|') break;
        if (x >  s && x[0] == ':') {
            if (x[-1] == stk) return depth; /* found */
            do { x--; } while (x >= s && ! p4_isspace(*x));
            depth = 0; continue;
        }
        if (x >= s && ! p4_isspace(*x)) {
            depth ++;
            do { x--; } while (x >= s && ! p4_isspace(*x));
        }
    }
    if (stk && stk != STK) return 0; /* untouched */
    return depth; /* default stack - the parameters */
}

static int input_depth (const char* s, const char* x, unsigned char stk)
{
    x = find_changer (s, x);
    if (! x) return 0;
    if (! stk) stk = STK;
    return stack_depth (s, x-1, stk);
}

static int output_depth (const char* s, const char* x, unsigned char stk)
{
    s = find_changer (s, x);
    if (! s) return 0;
    if (! stk) stk = STK;
    return stack_depth (s+1, x, stk);
}

/* ------------------------------------------------------------------ */
/*
 * implementation note: this is quite a lot of code but each part is
 * pretty self-contained and works on strings alone. The actual
 * algorithmics involved are not even worth speaking much about. We
 * do export each subroutine as a forth word for debugging reasons,
 * but many pfe check routines will use the lowlevel words themselves.
 */

static inline pair_t use_pair (pair_t pair, const char* str, int len)
{
    pair->str = str;
    pair->end = str+len;
    return pair;
}

static const char* find_lastxor (const char* str, const char* end)
{
    const char* p = end;
    for (p = end; str <= p; p--)
    {
        if (isENDOFTOKEN(*p)) p = skipback (p, str);
	if (*p == '|')
	{
#         if 1 /* CHANGER uses this */
	    if (str < p && ! p4_isspace(p[-1])) /* " |" twochar sequence */
		continue;
#         else
	    if (p < end && ! p4_isspace(p[1])) /* "| " twochar sequence */
		continue;
#         endif
	    return p;
	}
    }
    return 0;
}

static const char* find_nextchanger (const char* s, const char* x)
{
    /*      : x |( a -- a a ) dup ;         */
    while (s < x)
    {
        if (s[0] == '-' && s[1] == '-')
        {
            return s+1;
        }
	s++;
        if (isSTARTTOKEN(*s)) s = skipnext(s, x);
    }
    return 0;
}
static inline char* find_nextchanger_ (char* s, char* x) {
    return (char*) find_nextchanger (s, x);
}


static int narrow_changer (pair_t pair, int later)
{
    const char* s = pair->str;
    const char* p = find_nextchanger (pair->str, pair->end);
    if (! p) return 0;
    /* there are two possible matches - one for last part (or whole part)
     * and the other for an inner part with a following changer
     */
    while (1) {
	const char* x = find_nextchanger (p+1, pair->end);
	if (! later)
	{
	    if (x)
	    {
		const char* e = find_lastxor (p+1, x);
		if (! e) return 0; /*invalid*/
		pair->end = e;
	    }
	    /* else pair->end = pair->end */
	    pair->str = s;
	    return 1;
	}
	if (! x) return 0;
	later--;
	s = find_lastxor (p, x);
	if (! s) return 0;
	s ++;
	p = x;
    }
}

static int narrow_inputlist (pair_t pair)
{
    const char* x = find_nextchanger (pair->str, pair->end);
    if (x) { pair->end = x-1; return 1; }
    return 0;
}

static int narrow_outputlist (pair_t pair)
{
    const char* x = find_nextchanger (pair->str, pair->end);
    if (x) { pair->str = x+1; return 1; }
    return 0;
}

static const char* find_nextxor (const char* str, const char* end)
{
    const char* p = str;
    for (p = str; p < end ; p++)
    {
        if (isSTARTTOKEN(*p)) { if ((p = skipnext(p, end)) == end) break; }
	if (*p == '|')
	{
#         if 0
	    if (str < p && ! p4_isspace(p[-1])) /* " |" twochar sequence */
		continue;
#         else /* VARIANT uses this */
	    if (p+1 < end && ! p4_isspace(p[1])) /* "| " twochar sequence */
		continue;
#         endif
	    return p;
	}
    }
    return 0;
}

static int narrow_variant (pair_t pair, int later)
{
    const char* s = pair->str;
    const char* x = pair->end;
    /* there are two possible matches - one for last part (or whole part)
     * and the other for an inner part with a following changer
     */
    while (1) {
	if (! later)
	{
            x = find_nextxor (s, x);
            if (x) pair->end = x; /* else pair->end = pair->end; */
            pair->str = s;
            return 1;
        }
	later--;
	s = find_nextxor (s, x);
	if (! s) return 0; /* no further variants */
	s ++; /* (x-s) ==> 0 */
    }
    /*   1 1 NARROW-INPUT-VARIANT( a b -- c d | x y| u v -- m n )   */
}

static int narrow_stack (pair_t pair, unsigned char stk)
{
    register const char* p = pair->str, *x = pair->end;
    if (!stk)
	goto initial;
    while (p < x)
    {
	while (p < x && p4_isspace(*p)) p++;
        if (isSTARTTOKEN(*p)) p = skipnext(p, x);
	if (p+1 < x && p[0] == stk && p[1] == ':')
	    goto found;
	while (p < x && !p4_isspace(*p)) p++;
    }
    return 0; /*nothing:*/
 found:
    p += 2;
    pair->str = p;
    for ( ; p < x ; p++)
    {
        while (p < x && p4_isspace(*p)) p++;
        if (isSTARTTOKEN(*p)) {
            if (p4_isspace(*(p-1)))
                break; /* parser information found. */
            p = skipnext(p, x);
        }
	if (p+1 < x && p4_isupper(p[0]) && p[1] == ':')
	{
	    if (! p4_isalnum(*p)) p++;
	    break; /* done - next stack layout found */
	}
    };
 done:
    pair->end = p;
    return 1;
 initial:
    for (;; p++)
    {
	while (p < x && p4_isspace(*p)) p++;
        if (! (p < x)) break; /*exit*/
        if (isSTARTTOKEN(*p)) {
            if (p <= pair->str || p4_isspace(*(p-1)))
                break; /* parser information found. */
            p = skipnext(p, x);
        }
	if (p+1 < x && p4_isupper(p[0]) && p[1] == ':')
	    break; /* done - next stack layout found. */
    }
    goto done; /* possibly a zero-length string! */
}

/**
 * return stack layout start as  "X: ...."
 * return variant notation as    "| ...."
 * return process information as " [..."
 * - one can continue parsing with giving the return value plus 1
 *   to get to the next item in the notation.
 */
static const char*
find_nextxor_or_stackhint_or_proc (const char* str, const char* end)
{
    const char* p = str;
    for (p = str; p < end ; p++)
    {
        if (isSTARTTOKEN(*p)) {
            if ((str < p) && p4_isspace(p[-1]))
                return p-1; /* done - found parser information. */
            if ((p = skipnext(p, end)) == end) break;
        }
	if (*p == '|')
	{
#         if 0
	    if (str < p && ! p4_isspace(p[-1])) /* " |" twochar sequence */
		continue;
#         else /* VARIANT uses this */
	    if (p+1 < end && ! p4_isspace(p[1])) /* "| " twochar sequence */
		continue;
#         endif
	    return p; /* done - found "| " sequence */
	}
        if (p+1 < end && p4_isupper(p[0]) && p[1] == ':')
        {
            return p; /* done - found "X:" sequence */
        }
    }
    return 0;
}

/** return xor-start, stack-start or proc-start */
static int narrow_notation (pair_t pair, int later)
{
    const char* s = pair->str;
    const char* x = pair->end;
    int next = 0;
    while (1) {
	if (! later)
	{
            x = find_nextxor_or_stackhint_or_proc (s+next, x);
            if (x) pair->end = x; /* else pair->end = pair->end; */
            pair->str = s;
            return 1;
        }
	s = find_nextxor_or_stackhint_or_proc (s+next, x); /* (x-s) ==> 0 */
	later--; next = 1;
	if (! s) return 0; /* no further variants */
    }
    /*   1 0 NARROW-INPUT-NOTATION( c d | x y| u v -- m n )     */
    /*   1 0 NARROW-INPUT-NOTATION( | c d | x y| u v -- m n )   */
    /*   0 0 NARROW-INPUT-NOTATION( | c d | x y| u v -- m n )   */
    /*   3 0 NARROW-INPUT-NOTATION( c d | x y| u v |-- m n )    */
}

static int narrow_argument (pair_t pair, int argid)
{
    register const char* p = pair->str, *x = pair->end;
    register const char* q;
    if (argid < 0)
        return 0;
    while (x > p)
    {
        x--;
        while (p4_isspace(*x)) { x--; if (x < p) return 0; }
        if (isENDOFTOKEN(*x)) x = skipback(x, p);
        q = x;
        while (!p4_isspace(*q)) { q--; if (q < p) break; }
        if (q == x) return 0;
        if (! argid) /* found */
        {
            pair->str = q+1; /* q is on the space before the argument */
            pair->end = x+1; /* x is on the last char of the argument */
            return 1;
        }
        x = q; argid--;
    }
    return 0;
}

static const char* find_argument_name_end(const char* p, const char* x)
{
    while (p < x && (p4_isalnum(*p) || p4_strchr("_/", *p))) p++;
    return p;
}

static int narrow_argument_name(pair_t pair)
{
    register const char* p = find_argument_name_end(pair->str, pair->end);
    if (p == pair->str) return 0;
    pair->end = p; return 1; /* never empty */
}

static int narrow_argument_type(pair_t pair)
{
    register const char* p = find_argument_name_end(pair->str, pair->end);
    pair->str = p; return 1; /* possibly empty */
}

static int parse_pair(pair_t pair)
{
    p4_word_parse(')');
    if (PFE.word.len)
    {
	use_pair(pair, (char*) PFE.word.ptr, PFE.word.len);
	return 1;
    }
    return 0;
}

static int show_parse_pair(pair_t pair)
{
# define _CUT_STR(V) p4_outf ("{%li}>", (long)(V));
# define _CUT_END(V) p4_outf ("<{%li}", (long)(V));
    const char* p = (char*) PFE.word.ptr;
    p4_outf("\n( %.*s)\n .", Q PFE.word.len, p);
    if (pair->str > p+250) { _CUT_STR(pair->str-p); p = pair->str; }
    for (; p < pair->str ; p++) p4_outs(".");
    if (p == pair->end) p4_outs("<");
    if (pair->end > p+250) { _CUT_END(pair->end-p); p = pair->end; }
    for (; p < pair->end ; p++) p4_outs("^");
    p4_outf("..\n");
    return 0;
}

/** "NARROW-CHANGER(" ( changer# "stackhelp<rp>" -- ) [EXT]
 */
void FXCode(p4_narrow_changer)
{
    struct pair pair;
    p4cell which = FX_POP;

    if (parse_pair(&pair)) {
	if (narrow_changer(&pair, which)) {
	    show_parse_pair (&pair);
	}else p4_outs("no changer found\n");
    }else p4_outs("empty input");
}

/** "NARROW-INPUTLIST(" ( changer# "stackhelp<rp>" -- ) [EXT]
 */
void FXCode(p4_narrow_inputlist)
{
    struct pair pair;
    int changer = FX_POP;

    if (parse_pair(&pair)) {
	if (narrow_changer(&pair, changer)) {
	    if (narrow_inputlist(&pair)) {
		show_parse_pair (&pair);
	    }else p4_outs ("no inputdefs there\n");
	}else p4_outf("changer %i not found\n", changer);
    }else p4_outs("empty input");
}

/** "NARROW-OUTPUTLIST(" ( changer# "stackhelp<rp>" -- ) [EXT]
 */
void FXCode(p4_narrow_outputlist)
{
    struct pair pair;
    int changer = FX_POP;

    if (parse_pair(&pair)) {
	if (narrow_changer(&pair, changer)) {
	    if (narrow_outputlist(&pair)) {
		show_parse_pair (&pair);
	    }else p4_outs ("no outputdefs there\n");
	}else p4_outf("changer %i not found\n", changer);
    }else p4_outs("empty input");
}

/** "NARROW-INPUT-VARIANT(" ( variant# changer# "stackhelp<rp>" -- ) [EXT]
 * 0 = default, 1 = 'S', 2 = 'R', ... 4 = 'P', ... 7 = 'M', .. 14 = 'F'
 */
void FXCode(p4_narrow_input_variant)
{
    struct pair pair;
    int changer = FX_POP;
    int variant = FX_POP;

    if (parse_pair(&pair)) {
	if (narrow_changer(&pair, changer)) {
	    if (narrow_inputlist(&pair)) {
                if (narrow_variant (&pair, variant)) {
                        show_parse_pair (&pair);
                }else p4_outf ("variant %i not found\n", variant);
	    }else p4_outs ("no inputdefs there\n");
	}else p4_outf("changer %i not found\n", changer);
    }else p4_outs("empty input");
}

/** "NARROW-OUTPUT-VARIANT(" ( variant# changer# "stackhelp<rp>" -- ) [EXT]
 */
void FXCode(p4_narrow_output_variant)
{
    struct pair pair;
    int changer = FX_POP;
    int variant = FX_POP;

    if (parse_pair(&pair)) {
	if (narrow_changer(&pair, changer)) {
	    if (narrow_outputlist(&pair)) {
                if (narrow_variant (&pair, variant)) {
                        show_parse_pair (&pair);
                }else p4_outf ("variant %i not found\n", variant);
	    }else p4_outs ("no outputdefs there\n");
	}else p4_outf ("changer %i not found\n", changer);
    }else p4_outs ("empty input");
}

/** "NARROW-INPUT-STACK(" ( stk-char variant# changer# "stackhelp<rp>" -- ) [EXT]
 * 0 = default, 1 = 'S', 2 = 'R', ... 4 = 'P', ... 7 = 'M', .. 14 = 'F'
 */
void FXCode(p4_narrow_input_stack)
{
    struct pair pair;
    int changer = FX_POP;
    int variant = FX_POP;
    p4char stack = FX_POP;
    if (0 < stack && stack < 20) stack = NUM2STK(stack);

    if (parse_pair(&pair)) {
	if (narrow_changer(&pair, changer)) {
	    if (narrow_inputlist(&pair)) {
                if (narrow_variant (&pair, variant)) {
                    if (narrow_stack(&pair, stack)) {
                        show_parse_pair (&pair);
                    }else p4_outf ("stack %c not mentioned\n", stack);
                }else p4_outf ("variant %i not found\n", variant);
	    }else p4_outs ("no inputdefs there\n");
	}else p4_outf("changer %i not found\n", changer);
    }else p4_outs("empty input");
}

/** "NARROW-OUTPUT-STACK(" ( stk-char variant# changer# "stackhelp<rp>" -- ) [EXT]
 */
void FXCode(p4_narrow_output_stack)
{
    struct pair pair;
    int changer = FX_POP;
    int variant = FX_POP;
    p4char stack = FX_POP;
    if (0 < stack && stack < 20) stack = NUM2STK(stack);

    if (parse_pair(&pair)) {
	if (narrow_changer(&pair, changer)) {
	    if (narrow_outputlist(&pair)) {
                if (narrow_variant (&pair, variant)) {
                    if (narrow_stack(&pair, stack)) {
                        show_parse_pair (&pair);
                    }else p4_outf ("stack %c not mentioned\n", stack);
                }else p4_outf ("variant %i not found\n", variant);
	    }else p4_outs ("no outputdefs there\n");
	}else p4_outf ("changer %i not found\n", changer);
    }else p4_outs ("empty input");
}

/** "NARROW-INPUT-ARGUMENT(" ( arg# stk-char variant# changer# "stackhelp<rp>" -- ) [EXT]
 * 0 = default, 1 = 'S', 2 = 'R', ... 4 = 'P', ... 7 = 'M', .. 14 = 'F'
 * arg# is [0] = TOS and [1] = UNDER, same as the pick values where
 * 3 2 1 0 2 pick . =:= 2
 */
void FXCode(p4_narrow_input_argument)
{
    struct pair pair;
    int changer = FX_POP;
    int variant = FX_POP;
    p4char stack = FX_POP;
    int argid = FX_POP;
    if (0 < stack && stack < 20) stack = NUM2STK(stack);

    if (parse_pair(&pair)) {
	if (narrow_changer(&pair, changer)) {
	    if (narrow_inputlist(&pair)) {
                if (narrow_variant (&pair, variant)) {
                    if (narrow_stack(&pair, stack)) {
                        if (narrow_argument(&pair, argid)) {
                            show_parse_pair (&pair);
                        }else p4_outf ("arg %i not found\n", argid);
                    }else p4_outf ("stack %c not mentioned\n", stack);
                }else p4_outf ("variant %i not found\n", variant);
	    }else p4_outs ("no inputdefs there\n");
	}else p4_outf ("changer %i not found\n", changer);
    }else p4_outs ("empty input");
}

/** "NARROW-OUTPUT-ARGUMENT(" ( arg# stk-char variant# changer# "stackhelp<rp>" -- ) [EXT]
 * arg# is [0] = TOS and [1] = UNDER, same as the pick values where
 * 3 2 1 0 2 pick . =:= 2
 */
void FXCode(p4_narrow_output_argument)
{
    struct pair pair;
    int changer = FX_POP;
    int variant = FX_POP;
    p4char stack = FX_POP;
    int argid = FX_POP;
    if (0 < stack && stack < 20) stack = NUM2STK(stack);

    if (parse_pair(&pair)) {
	if (narrow_changer(&pair, changer)) {
	    if (narrow_outputlist(&pair)) {
                if (narrow_variant (&pair, variant)) {
                    if (narrow_stack(&pair, stack)) {
                        if (narrow_argument(&pair, argid)) {
                            show_parse_pair (&pair);
                        }else p4_outf ("arg %i not found\n", argid);
                    }else p4_outf ("stack %c not mentioned\n", stack);
                }else p4_outf ("variant %i not found\n", variant);
	    }else p4_outs ("no outputdefs there\n");
	}else p4_outf ("changer %i not found\n", changer);
    }else p4_outs("empty input");
}

/** "NARROW-INPUT-ARGUMENT-NAME(" ( arg# stk-char variant# changer# "stackhelp<rp>" -- ) [EXT]
 * 0 = default, 1 = 'S', 2 = 'R', ... 4 = 'P', ... 7 = 'M', .. 14 = 'F'
 * arg# is [0] = TOS and [1] = UNDER, same as the pick values where
 * 3 2 1 0 2 pick . =:= 2
 */
void FXCode(p4_narrow_input_argument_name)
{
    struct pair pair;
    int changer = FX_POP;
    int variant = FX_POP;
    p4char stack = FX_POP;
    int argid = FX_POP;
    if (0 < stack && stack < 20) stack = NUM2STK(stack);

    if (parse_pair(&pair)) {
	if (narrow_changer(&pair, changer)) {
	    if (narrow_inputlist(&pair)) {
                if (narrow_variant (&pair, variant)) {
                    if (narrow_stack(&pair, stack)) {
                        if (narrow_argument(&pair, argid)) {
                            if (narrow_argument_name(&pair)) {
                                show_parse_pair (&pair);
                            }else p4_outs ("oops, no argument name seen\n");
                        }else p4_outf ("arg %i not found\n", argid);
                    }else p4_outf ("stack %c not mentioned\n", stack);
                }else p4_outf ("variant %i not found\n", variant);
	    }else p4_outs ("no inputdefs there\n");
	}else p4_outf ("changer %i not found\n", changer);
    }else p4_outs("empty input");
}

/** "NARROW-OUTPUT-ARGUMENT-NAME(" ( arg# stk-char variant# changer# "stackhelp<rp>" -- ) [EXT]
 * arg# is [0] = TOS and [1] = UNDER, same as the pick values where
 * 3 2 1 0 2 pick . =:= 2
 */
void FXCode(p4_narrow_output_argument_name)
{
    struct pair pair;
    int changer = FX_POP;
    int variant = FX_POP;
    p4char stack = FX_POP;
    int argid = FX_POP;
    if (0 < stack && stack < 20) stack = NUM2STK(stack);

    if (parse_pair(&pair)) {
	if (narrow_changer(&pair, changer)) {
	    if (narrow_outputlist(&pair)) {
                if (narrow_variant (&pair, variant)) {
                    if (narrow_stack(&pair, stack)) {
                        if (narrow_argument(&pair, argid)) {
                            if (narrow_argument_name(&pair)) {
                                show_parse_pair (&pair);
                            }else p4_outs ("oops, no argument name seen\n");
                        }else p4_outf ("arg %i not found\n", argid);
                    }else p4_outf ("stack %c not mentioned\n", stack);
                }else p4_outf ("variant %i not found\n", variant);
	    }else p4_outs ("no outputdefs there\n");
	}else p4_outf ("changer %i not found\n", changer);
    }else p4_outs("empty input");
}

/** "NARROW-INPUT-ARGUMENT-TYPE(" ( arg# stk-char variant# changer# "stackhelp<rp>" -- ) [EXT]
 * 0 = default, 1 = 'S', 2 = 'R', ... 4 = 'P', ... 7 = 'M', .. 14 = 'F'
 * arg# is [0] = TOS and [1] = UNDER, same as the pick values where
 * 3 2 1 0 2 pick . =:= 2
 */
void FXCode(p4_narrow_input_argument_type)
{
    struct pair pair;
    int changer = FX_POP;
    int variant = FX_POP;
    p4char stack = FX_POP;
    int argid = FX_POP;
    if (0 < stack && stack < 20) stack = NUM2STK(stack);

    if (parse_pair(&pair)) {
	if (narrow_changer(&pair, changer)) {
	    if (narrow_inputlist(&pair)) {
                if (narrow_variant (&pair, variant)) {
                    if (narrow_stack(&pair, stack)) {
                        if (narrow_argument(&pair, argid)) {
                            if (narrow_argument_type(&pair)) {
                                show_parse_pair (&pair);
                            }else p4_outs ("oops, no argument type seen\n");
                        }else p4_outf ("arg %i not found\n", argid);
                    }else p4_outf ("stack %c not mentioned\n", stack);
                }else p4_outf ("variant %i not found\n", variant);
	    }else p4_outs ("no inputdefs there\n");
	}else p4_outf ("changer %i not found\n", changer);
    }else p4_outs("empty input");
}

/** "NARROW-OUTPUT-ARGUMENT-TYPE(" ( arg# stk-char which# "stackhelp<rp>" -- ) [EXT]
 * arg# is [0] = TOS and [1] = UNDER, same as the pick values where
 * 3 2 1 0 2 pick . =:= 2
 */
void FXCode(p4_narrow_output_argument_type)
{
    struct pair pair;
    int changer = FX_POP;
    int variant = FX_POP;
    p4char stack = FX_POP;
    int argid = FX_POP;
    if (0 < stack && stack < 20) stack = NUM2STK(stack);

    if (parse_pair(&pair)) {
	if (narrow_changer(&pair, changer)) {
	    if (narrow_outputlist(&pair)) {
                if (narrow_variant (&pair, variant)) {
                    if (narrow_stack(&pair, stack)) {
                        if (narrow_argument(&pair, argid)) {
                            if (narrow_argument_type(&pair)) {
                                show_parse_pair (&pair);
                            }else p4_outs ("oops, no argument type seen\n");
                        }else p4_outf ("arg %i not found\n", argid);
                    }else p4_outf ("stack %c not mentioned\n", stack);
                }else p4_outf ("variant %i not found\n", variant);
	    }else p4_outs ("no outputdefs there\n");
	}else p4_outf ("changer %i not found\n", changer);
    }else p4_outs("empty input");
}

/* ------------------------------------------------------------------- */

struct mapping { const char type; const char len; const char* name; };
typedef const struct mapping mapping_t;
mapping_t mappings[] =
{
    { '*', 4, "-ptr" },
    { '*', 8, "-pointer" },
    { '*', 5, "-addr" },
    { '*', 8, "-address" },
    { '*', 7, "-struct" },
    { '#', 4, "-num" },
    { '#', 7, "-number" },
    { '#', 4, "-len" },
    { '#', 7, "-length" },
    { '#', 5, "-size" },
    { '#', 4, "-cnt" },
    { '#', 6, "-count" },
    { '?', 5, "-flag" },
    { '\'', 5, "-next" },
    { '*', 1, "^" },
    { 0, 0, 0 },
};

static mapping_t* canonic_mapping (const char* str, const char* end)
{
    mapping_t* map;
    for (map = mappings; map->type; map++)
    {
        if (end-str < IS(map->len)) continue;
        if (p4_isalnum(str[IS(map->len)])) continue;
        if (p4_memcmp(str, map->name, IS(map->len))) continue;
        return map; /* found */
    }
    return 0; /* not found */
}

static char* canonic_type(pair_t pair, char* str, const char* end)
{
    register const char* p = pair->str, *x = pair->end;
    while (p < x) /* left to right */
    {
        mapping_t* map = canonic_mapping (p, x);
        if (map)
        {   *str++ = map->type; p += map->len; }
        else do
        {   *str++ = *p++; } while (p < x && p4_isalnum(*p) && str < end);
        if (str >= end) return 0;
    }
    *str = 0;
    return str;
}

static void show_canonic(const char* buffer)
{
    p4_outs(" (");
    p4_outs(buffer);
    p4_outs(") ");
};

/** "CANONIC-INPUT-TYPE(" ( arg# stk-char variant# changer# "stackhelp<rp>" -- ) [EXT]
 * 0 = default, 1 = 'S', 2 = 'R', ... 4 = 'P', ... 7 = 'M', .. 14 = 'F'
 * arg# is [0] = TOS and [1] = UNDER, same as the pick values where
 * 3 2 1 0 2 pick . =:= 2
 */
void FXCode(p4_canonic_input_type)
{
    char buffer[BUFLEN];
    struct pair pair;
    p4cell changer = FX_POP;
    p4cell variant = FX_POP;
    p4char stack = FX_POP;
    p4cell argid = FX_POP;
    if (0 < stack && stack < 20) stack = NUM2STK(stack);

    if (parse_pair(&pair)) {
	if (narrow_changer(&pair, changer)) {
	    if (narrow_inputlist(&pair)) {
                if (narrow_variant (&pair, variant)) {
                    if (narrow_stack(&pair, stack)) {
                        if (narrow_argument(&pair, argid)) {
                            if (narrow_argument_type(&pair)) {
                                canonic_type(&pair, buffer, buffer+BUFLEN);
                                show_canonic(buffer);
                            }else p4_outs ("oops, no argument type seen\n");
                        }else p4_outf ("arg %i not found\n", Q argid);
                    }else p4_outf ("stack %c not mentioned\n", stack);
                }else p4_outf ("variant %i not found\n", Q variant);
	    }else p4_outs ("no inputdefs there\n");
	}else p4_outf ("changer %i not found\n", Q changer);
    }else p4_outs("empty input");
}

/** "CANONIC-OUTPUT-TYPE(" ( arg# stk-char variant# changer# "stackhelp<rp>" -- ) [EXT]
 * arg# is [0] = TOS and [1] = UNDER, same as the pick values where
 * 3 2 1 0 2 pick . =:= 2
 */
void FXCode(p4_canonic_output_type)
{
    char buffer[BUFLEN];
    struct pair pair;
    p4cell changer = FX_POP;
    p4cell variant = FX_POP;
    p4char stack = FX_POP;
    p4cell argid = FX_POP;
    if (0 < stack && stack < 20) stack = NUM2STK(stack);

    if (parse_pair(&pair)) {
	if (narrow_changer(&pair, changer)) {
	    if (narrow_outputlist(&pair)) {
                if (narrow_variant (&pair, variant)) {
                    if (narrow_stack(&pair, stack)) {
                        if (narrow_argument(&pair, argid)) {
                            if (narrow_argument_type(&pair)) {
                                canonic_type(&pair, buffer, buffer+BUFLEN);
                                show_canonic(buffer);
                            }else p4_outs ("oops, no argument type seen\n");
                        }else p4_outf ("arg %i not found\n", Q argid);
                    }else p4_outf ("stack %c not mentioned\n", stack);
                }else p4_outf ("variant %i not found\n", Q variant);
	    }else p4_outs ("no outputdefs there\n");
	}else p4_outf ("changer %i not found\n", Q changer);
    }else p4_outs("empty input");
}

/* ------------------------------------------------------------------- */

/* cut non-type tail chars from a canonic-type buffer */
static void cut_modified(char* buffer)
{
    int x = p4_strlen(buffer);
    while (--x > 0)
    {  if (buffer[x] != '\'') break; }
    buffer[x+1] = '\0';
}

static int good_type_suffix (pair_t larger, pair_t smaller)
{
    char lbuffer[BUFLEN];
    char rbuffer[BUFLEN];
    int diff;

    canonic_type (larger, lbuffer, lbuffer+BUFLEN);
    canonic_type (smaller, rbuffer, rbuffer+BUFLEN);
    cut_modified (lbuffer);
    diff = (int)(p4_strlen (lbuffer)) - (int)(p4_strlen (rbuffer));
    if (diff < 0) return 0; /* right side too long */
    if (p4_strcmp (lbuffer+diff, rbuffer)) return 0; /* they differ */
    return 1;
}

static int equal_type_suffix (pair_t larger, pair_t smaller)
{
    char lbuffer[BUFLEN];
    char rbuffer[BUFLEN];
    int diff;

    canonic_type (larger, lbuffer, lbuffer+BUFLEN);
    canonic_type (smaller, rbuffer, rbuffer+BUFLEN);
    cut_modified (lbuffer);
    diff = (int)(p4_strlen (lbuffer)) - (int)(p4_strlen (rbuffer));
    if (diff) return 0; /* either side too long */
    if (p4_strcmp (lbuffer, rbuffer)) return 0; /* they differ */
    return 1;
}

/** out-reason points into "input" */
static int rewrite_stack_test(pair_t stack, pair_t input, pair_t reason)
{
    struct pair inp; /** arg in "input" */
    struct pair arg; /** arg in "stack" */
    register int n;
    for (n = 0; n < 32; n ++) { /* walk each argument on the right side... */
        pairdef (&arg, stack);
        pairdef (&inp, input);
        if (! narrow_argument (&inp, n)) return 1; /*success*/
        if (! narrow_argument (&arg, n)) { /* not enough in stack */
            if (reason) pairdef (reason, &inp);
            return 0;
        }
        if (! narrow_argument_type(&inp) ||
            ! narrow_argument_type(&arg))
        {
            inp.str = inp.end;
            if (reason) pairdef (reason, &inp);
            return 0;
        }
        if (! good_type_suffix (&arg, &inp))
        {
            if (reason) pairdef (reason, &inp);
            return 0;
        }
    }
    if (reason) pairdef (reason, input);
    return 0;
}

/** "REWRITER-TEST(" ( "tracked-stack -- input-stack<rp>" -- ) [EXT]
 * suppose that the left side is a tracked stack line during compiling
 * and the right side is a candidate changer input stack. Test whethr
 * the candidate does match and the complete changer would be allowed
 * to run a rewrite in the track stack buffer.
 *
 * Possible conditions include:
 *   the left side has not enough arguments or...
 *      any argument on the right side has a type specialization
 *      that does not match as a valid suffix to their counterpart
 *      on the left side.
 */
void FXCode(p4_rewriter_test)
{
    struct pair pair;
    struct pair test;
    struct pair reason;

    if (parse_pair(&pair)) {
	if (narrow_changer(&pair, 0)) {
            pairdef (&test, &pair);
	    if (narrow_inputlist(&pair)) {
                if (narrow_outputlist(&test)) {
                    if (rewrite_stack_test (&pair, &test, &reason)) {
                        p4_outs ("oK ");
                    }else{
                        p4_outs ("No ");
                        show_parse_pair(&reason);
                    }
		}else p4_outs ("no outputdefs changer found\n");
	    }else p4_outs ("no inputdefs stack found\n");
	}else p4_outs("no changer found\n");
    }else p4_outs("empty input");
}

/* similar to the earlier good_type_suffix(orig,from):
 * narrow "orig" with all stuff cut away that is a common suffix
 * with the right "test" specification. We try to preserve as
 * much as possible. As a result, the "test" arg_name matches
 * a sequence in "orig" comprising a name plus (possibly empty)
 * extra type specifications not in the common type suffix
 * beware: the "orig" sequence may have non-type tail chars that
 * are not in the narrowed sequence, when rewriting a term then
 * remember to copy that non-type tail into the target arg spec.
 */
static int narrow_good_type_prefix (pair_t orig, pair_t test)
{
    /* here we duplicate the good_type_suffix code */
    char lbuffer[BUFLEN];
    char rbuffer[BUFLEN];
    int diff;

    canonic_type (orig, lbuffer, lbuffer+BUFLEN);
    canonic_type (test, rbuffer, rbuffer+BUFLEN);
    cut_modified (lbuffer);
    diff = (int)(p4_strlen (lbuffer)) - (int)(p4_strlen (rbuffer));
    if (diff < 0) return 0; /* right side too long */
    if (p4_strcmp (lbuffer+diff, rbuffer)) return 0; /* they differ */

    /* now we have a good suffix - diff is the good _canonic_ prefix */
    if (! diff) { orig->end = orig->str; return 1; /* quick path */ }
    else {
        const char* q = lbuffer;
        const char* p = orig->str, *x = orig->end;
        while (p < x)
        {
            mapping_t* map = canonic_mapping (p, x);
            if (map)
            {
                q += 1; /* assert (*q == map->type); */
                p += map->len;
            }else{
                q += 1;
                p += 1;
            }
            if (q-lbuffer >= diff)
            {
                orig->end = p; /* narrowed to good _orig_ prefix */
                return 1;
            }
        }
        return 0;
    }
}

static int narrow_good_item_prefix (pair_t orig, pair_t test)
{
    struct pair o, t;
    pairdef (&o, orig);
    pairdef (&t, test);
    narrow_argument_type (&o);
    narrow_argument_type (&t);
    if (! narrow_good_type_prefix (&o, &t))
        return 0;
    orig->end = o.end;
    return 1;
}

/** "REWRITER-INPUT-ARG(" ( arg# "tracked-stack -- changer<rp>" -- ) [EXT]
 * suppose that the left side is a tracked stack line during compiling
 * and the right side is a candidate changer input stack. Assume the
 * righthand candidate does match - look at the given argument on the
 * left side and show the prefix being copied to the output trackstack
 * when the rewrite-rule is gettin applied later.
 */
void FXCode(p4_rewriter_input_arg)
{
    struct pair pair;
    struct pair test;
    struct pair reason;
    p4cell argid = FX_POP;

    if (parse_pair(&pair)) {
	if (narrow_changer(&pair, 0)) {
            pairdef (&test, &pair);
	    if (narrow_inputlist(&pair)) {
                if (narrow_outputlist(&test)) {
                    if (rewrite_stack_test (&pair, &test, &reason)) {
                        if (narrow_argument (&pair, argid)) {
                            if (narrow_argument (&test, argid)) {
                                if (narrow_good_item_prefix (&pair, &test))
                                {
                                    show_parse_pair (&pair);
                                }else p4_outs ("no good prefix seen\n");
                            }else p4_outs ("no arg id in changer found\n");
                        }else p4_outs ("no arg id in inputdefs found\n");
                    }else{
                        p4_outs ("[not rewritable]");
                        show_parse_pair(&reason);
                    }
		}else p4_outs ("no outputdefs changer found\n");
	    }else p4_outs ("no inputdefs stack found\n");
	}else p4_outs("no changer found\n");
    }else p4_outs("empty input");
}

/* ------------------------------------------------------------------- */
/** "REWRITE-LINE(" ( "stack-layout<rp>" -- )  [EXT]
 * fill rewrite-buffer with a stack-layout to be processed.
 * see =>"REWRITE-SHOW."
 */
void FXCode (p4_rewrite_line)
{
    struct pair pair;
    if (parse_pair (&pair))
    {
        p4_memcpy (CHK.line.str, pair.str, pairlen_(pair));
        CHK.line.end = CHK.line.str + pairlen_(pair);
    }
}

/** "REWRITE-SHOW." ( -- ) [EXT]
 * show current rewrite-buffer.
 */
void FXCode (p4_rewrite_show)
{
    p4_outf ("( %.*s)", Q pairlen_(CHK.line), CHK.line.str);
}

#define line_pair(_pair_) pairdef(_pair_, &(CHK.line))

static int line_show(pair_t pair)
{
    char* p = CHK.line.str;
    p4_outf("\n( %.*s)\n .", Q pairlen_(CHK.line), p);
    for (; p < pair->str ; p++) p4_outs(".");
    if (p == pair->end) p4_outs("<");
    for (; p < pair->end ; p++) p4_outs("^");
    p4_outf("..\n");
    return 0;
}

static int pair_equal (pair_t left, pair_t right)
{
    if (pairlen(left) != pairlen(right))
        return 0;
    return ! p4_memcmp (left->str, right->str, pairlen(right));
}

/** "REWRITE-STACK-TEST(" ( "stackhelp<rp>" -- ) [EXT]
 *  check whether this stackhelp does match on current rewrite-buffer
 *  and say oK/No respectivly.
 */
void FXCode (p4_rewrite_stack_test)
{
    struct pair line;
    struct pair pair;
    struct pair reason;

    line_pair (&line);
    if (parse_pair(&pair)) {
        narrow_changer(&pair, 0);
        if (narrow_inputlist(&pair)) {
            if (rewrite_stack_test (&line, &pair, &reason)) {
                p4_outs ("oK ");
            }else{
                p4_outs ("No ");
                show_parse_pair(&reason);
            }
        }else p4_outs ("no inputdefs stack found\n");
    }else p4_outs("empty input");
}

/** "REWRITE-INPUT-ARG(" ( arg# "stackhelp<rp>" -- ) [EXT]
 *  check whether this stackhelp does match on current rewrite-buffer
 *  and in the given input match show us the argument but only the
 *  good prefix i.e. the type constraint being cut off already.
 */
void FXCode(p4_rewrite_input_arg)
{
    struct pair line;
    struct pair pair;
    struct pair reason;
    p4cell argid = FX_POP;

    line_pair (&line);
    if (parse_pair(&pair)) {
	if (narrow_changer(&pair, 0)) {
	    if (narrow_inputlist(&pair)) {
                if (rewrite_stack_test (&line, &pair, &reason)) {
                    if (narrow_argument (&pair, argid)) {
                        if (narrow_argument (&line, argid)) {
                            if (narrow_good_item_prefix (&line, &pair))
                            {
                                line_show (&line);
                            }else p4_outs ("no good prefix seen\n");
                        }else p4_outs ("no arg id in changer found\n");
                    }else p4_outs ("no arg id in inputdefs found\n");
                }else{
                    p4_outs ("[not rewritable]");
                    show_parse_pair(&reason);
                }
	    }else p4_outs ("no inputdefs stack found\n");
	}else p4_outs("no changer found\n");
    }else p4_outs("empty input");
}

#ifdef __GNUC__
static int narrow_modified (pair_t pair) __attribute__((unused));
#endif

static int narrow_modified (pair_t pair)
{
    const char* p = pair->end;
    while (--p >= pair->str)
    {
        if (*p != '\'') break;
    }
    p++;
    if (p <= pair->end)
    {
        pair->str = p;
        return 1;
    }else{
        return 0;
    }
}

static int narrow_true_modified (pair_t pair)
{
    const char* p = pair->end;
    while (--p >= pair->str)
    {
        if (*p != '\'') break;
    }
    p++;
    if (p < pair->end) /* <= the only difference */
    {
        pair->str = p;
        return 1;
    }else{
        return 0;
    }
}

int p4_rewrite_stack (pair_t stack, pair_t input, pair_t output,
                      char* sink, int sinklen)
{
    struct pair arg; /** "arg" of "stack" */
    struct pair inp; /** "arg" of "input"  */
    struct pair out; /** "arg" of "output" */
    int i, j;
    *sink = '\0';
    if (sinklen < 32) return 0;

    for (i = 32; i >= 0; i --) /* forall args in "stack" */
    {
        pairdef (&arg, stack);
        if (! narrow_argument(&arg, i))
            continue;
        pairdef (&inp, input);
        if (! narrow_argument(&inp, i))
        {   /* copy unchanged input argument */
            if (CHK.debug[9]) p4_outf ("<unchanged stack arg #%i: '%.*s'>\n",
                                       i, Q pairlen_(arg), arg.str);
            if (pairlen_(arg) >= 32) return 0;
            p4_strncat (sink, arg.str, pairlen_(arg));
            p4_strlcat (sink, " ", sinklen-32);
            continue;
        }
        break; /* only inputs left that are to be changed */
    }

    for (i = 32; i >= 0; i --) /* forall args in "output" */
    {
        pairdef (&out, output);
        if (! narrow_argument(&out, i) || ! narrow_argument_name (&out))
            continue;
        for (j = 32; j >= 0; j --) /* forall args in "input" */
        {
            pairdef (&inp, input);
            if (! narrow_argument(&inp, j) || ! narrow_argument_name(&inp))
                continue;
            /* check if the "input" name is the same as "output" name */
            if (! pair_equal (&inp, &out))
                continue;
            break; /* found */
        }
        if (j >= 0) /* an "input" was found that shall be copied ! */
        {
            pairdef (&inp, input);
            pairdef (&arg, stack);
            if (narrow_argument (&inp, j) &&
                narrow_argument (&arg, j) &&
                narrow_good_item_prefix (&arg, &inp))
            {
                if (CHK.debug[9]) p4_outf ("<copying stack arg #%i as #%i:"
                                           "'%.*s'>\n",
                                           j, i, Q pairlen_(arg), arg.str);
                if (pairlen_(arg) >= 32) return 0;
                p4_strncat (sink, arg.str, pairlen_(arg));
                pairdef (&out, output);
                if (narrow_argument (&out, i) &&
                    narrow_argument_type (&out)) {
                    if (pairlen_(out) >= 32) return 0;
                    p4_strncat (sink, out.str, pairlen_(out));
                }
                pairdef (&arg, stack);
                if (narrow_argument (&arg, j) &&
                    narrow_true_modified (&arg)) {
                    if (pairlen_(arg) >= 32) return 0;
                    p4_strncat (sink, arg.str, pairlen_(arg));
                }
                p4_strlcat (sink, " ", sinklen-32);
                continue; /* at next "output" arg */
            }
        } /*else:*/
        /* if any of the "if"s above did fail: copy the "output" spec */
        pairdef (&out, output);
        if (! narrow_argument(&out, i))
            continue;
        if (CHK.debug[9]) p4_outf ("<copying out arg #%i: '%.*s'>\n",
                                   i, Q pairlen_(out), out.str);
        if (pairlen_(out) >= 32) return 0;
        p4_strncat (sink, out.str, pairlen_(out));
        p4_strlcat (sink, " ", sinklen-32);
    }
    return 1;
}

/** "REWRITE-STACK-RESULT(" ( "stackhelp<rp>" -- ) [EXT]
 * rewrite the current rewrite-buffer and show the result that
 * would occur with this stackhelp being applied.
 */
void FXCode (p4_rewrite_stack_result)
{
    char buffer[BUFLEN];
    struct pair inputdefs;
    struct pair outputdefs;
    struct pair reason;
    struct pair line;

    line_pair (&line);
    if (parse_pair(&inputdefs)) {
	if (narrow_changer(&inputdefs, 0)) {
            pairdef (&outputdefs, &inputdefs);
	    if (narrow_inputlist(&inputdefs)) {
                if (narrow_outputlist(&outputdefs)) {
                    if (rewrite_stack_test (&line, &inputdefs, &reason)) {
                        if (p4_rewrite_stack (&line, &inputdefs, &outputdefs,
                                              buffer, BUFLEN))
                        {
                            p4_outf ("( %s )\n", buffer);
                        }
                    }else{
                        p4_outs ("[not rewritable]");
                        show_parse_pair(&reason);
                    }
		}else p4_outs ("no outputdefs changer found\n");
	    }else p4_outs ("no inputdefs stack found\n");
	}else p4_outs("no changer found\n");
    }else p4_outs("empty input");
}

/* ------------------------------------------------------------------- */

/** "NARROW-INPUT-NOTATION(" ( notation# changer# "stackhelp<rp>" -- ) [EXT]
 */
void FXCode(p4_narrow_input_notation)
{
    struct pair pair;
    p4cell changer = FX_POP;
    p4char stackproc = FX_POP;

    if (parse_pair(&pair)) {
	if (narrow_changer(&pair, changer)) {
	    if (narrow_inputlist(&pair)) {
                if (narrow_notation (&pair, stackproc)) {
                        show_parse_pair (&pair);
                }else p4_outf ("notation %i not found\n", stackproc);
	    }else p4_outs ("no inputdefs there\n");
	}else p4_outf("changer %i not found\n", Q changer);
    }else p4_outs("empty input");
}

/** "NARROW-OUTPUT-NOTATION(" ( notation# changer# "stackhelp<rp>" -- ) [EXT]
 */
void FXCode(p4_narrow_output_notation)
{
    struct pair pair;
    p4cell changer = FX_POP;
    p4char stack = FX_POP;

    if (parse_pair(&pair)) {
	if (narrow_changer(&pair, changer)) {
	    if (narrow_outputlist(&pair)) {
                if (narrow_notation (&pair, stack)) {
                    show_parse_pair (&pair);
                }else p4_outf ("notation %i not found\n", stack);
	    }else p4_outs ("no outputdefs there\n");
	}else p4_outf ("changer %i not found\n", Q changer);
    }else p4_outs ("empty input");
}

static int narrow_stack0 (pair_t pair, unsigned char stk, unsigned char def)
{
    if (narrow_stack (pair, stk)) return 1;
    if (stk != def) return 0;
    return narrow_stack (pair, 0);
}

static int narrow_isempty (pair_t pair)
{
    const char* p;
    for (p = pair->str; p < pair->end; p++)
        if (! p4_isspace(*p)) break;

    pair->str = p;
    return p == pair->end;
}

/** out-reason points into inputdefs.
 *  forall stacks in stackdef: find corresponding stack in inputdefs
 *  and try a rewrite_stack_test */
static int rewrite_stackdef_test(pair_t stackdef, pair_t inputdef,
                                 pair_t reason)
{
    struct pair input;
    struct pair stack;
    register unsigned char stk;
    for (stk = 'A'; stk < 'Z'; stk++)
    {
        pairdef (&stack, stackdef);
        pairdef (&input, inputdef);
        if (! narrow_stack0(&input, stk, STK))
            continue; /* not required to match in changer-input */
        if (! narrow_stack0(&stack, stk, STK)) {
            if (! narrow_isempty (&input)) {
                pairdef (reason, &input);
                return 0; } /* must exist if changer-input not empty */
            continue; }
        if (! rewrite_stack_test (&stack, &input, reason))
            return 0;
    }
    return 1; /* all existing stack layouts did match */
}

/** "REWRITE-STACKDEF-TEST(" ( "stackdef<rp>" )  [EXT]
 *  match a stackdef (single variant of stacks).
 *  assume: single variant in rewrite-buffer and
 *          single variant in stackdef-arg and
 *          only one changer in arg-stackhelp
 */
void FXCode (p4_rewrite_stackdef_test)
{
    struct pair line;
    struct pair pair;
    struct pair reason;

    line_pair (&line);
    if (parse_pair(&pair)) {
        narrow_changer(&pair, 0);
        if (narrow_inputlist(&pair)) {
            if (rewrite_stackdef_test (&line, &pair, &reason)) {
                p4_outs ("oK ");
            }else{
                p4_outs ("No ");
                show_parse_pair(&reason);
            }
        }else p4_outs ("no inputdefs variant found\n");
    }else p4_outs("empty input");
}

unsigned char
narrow_to_stack (pair_t pair) /* a notation */
{
    if (pair->str < pair->end && *pair->str == '|')
    {
        pair->str ++;
        return 0;
    }
    if (pair->str+1 < pair->end &&
        *(pair->str+1) == ':' && p4_isupper (*pair->str))
    {
        unsigned char stk = *pair->str;
        pair->str += 2;
        return stk;
    }
    return 0;
}

unsigned char
narrow_is_proc(pair_t pair) /* a proc hint */
{
    if (pair->str+1 < pair->end &&
        p4_isspace(pair->str[0]) &&
        isSTARTTOKEN(pair->str[1]))
        return pair->str[1];
    else
        return 0;
}

int p4_rewrite_stackdef(pair_t subject, pair_t input, pair_t output,
                         char* sink, int sinklen)
{
    char stack[] = "X: ";
    struct pair arg;
    struct pair inp;
    struct pair out;
    unsigned char stk;
    unsigned char emptystart = 0;
    int n;
    if (sinklen < 32) return 0;
    *sink = '\0';

    for (n=0; n < 123; n ++)
    {
        pairdef (&arg, subject);
        if (! narrow_notation (&arg, n))
            break;
        if (narrow_is_proc(&arg))
            continue;
        stk = narrow_to_stack (&arg);
        if (! stk && narrow_isempty (&arg)) {
            emptystart = 1;
            continue; } /* test later and ... */
        if (stk == STK) emptystart = 0; /* ... do not when STK was seen */
        pairdef (&inp, input);
        if (stk) { *stack = stk; p4_strlcat (sink, stack, sinklen); }
        pairdef (&out, output);
        if (! narrow_stack0(&out, stk, STK))
        {
            if (! narrow_stack0(&inp, stk, STK))
            {   /* passthrough stacks not mentioned in inp/out changer */
                if (pairlen_(arg) >= 32) return 0;
                p4_strncat(sink, arg.str, pairlen_(arg));
                continue;
            } else
            {   out.str = output->str; out.end = out.str; }
        } else {
            if (! narrow_stack0(&inp, stk, STK))
            {   inp.str = input->str; inp.end = inp.str; }
        }
        if (! p4_rewrite_stack (&arg, &inp, &out,
                                sink + p4_strlen(sink),
				sinklen - p4_strlen(sink)))
            return 0;
        /* this is actually not finished: we want the actual names in
         * the input line be copied to the output line according to the
         * reshuffling expressed in the input/output changer. However, the
         * above call can only transfer actual names in the same stack 'stk'
         * and therefore |( a* R: x ) ( b -- R: b ) will not yield the
         * intended |( R: x a* ) but instead it will show |( R: x b )
         * since the output "R: b" can _not_ see "S: b" that could be
         * maps to the actual "S: a*" argument. It's not cross-stack, i.e.
         * here the code makes ( a R: a ) be with two different values. It
         * should be the same name pointing out same value however.
         */
    }
    /* what if (emptystack) { ? } */
    return 1;
}

/** "REWRITE-STACKDEF-RESULT(" ( "stackhelp<rp>" -- ) [EXT]
 *  assume:
 *       only one changer (if more are provided then only the first is used)
 *       only one stackdef variant in inputlist
 */
void FXCode (p4_rewrite_stackdef_result)
{
    char buffer[BUFLEN];
    struct pair inputdefs;
    struct pair outputdefs;
    struct pair reason;
    struct pair line;

    line_pair (&line);
    if (parse_pair(&inputdefs)) {
	if (narrow_changer(&inputdefs, 0)) {
            pairdef (&outputdefs, &inputdefs);
	    if (narrow_inputlist(&inputdefs)) {
                if (narrow_outputlist(&outputdefs)) {
                    if (rewrite_stackdef_test (
                            &line, &inputdefs, &reason)) {
                        if (p4_rewrite_stackdef (
                                &line, &inputdefs, &outputdefs,
                                buffer, BUFLEN))
                        {
                            p4_outf ("( %s )\n", buffer);
                        }else p4_outs ("\\ error during rewriting ");
                    }else{
                        p4_outs ("[not rewritable]");
                        show_parse_pair(&reason);
                    }
		}else p4_outs ("no outputdefs changer found\n");
	    }else p4_outs ("no inputdefs stack found\n");
	}else p4_outs("no changer found\n");
    }else p4_outs("empty input");
}
/* ------------------------------------------------------------------- */
/*
 * in the earlier code we did notice that name-checking is done per stack
 * instead of per stack notation across all stacks in a variant notation.
 * That applies both for _test and _rewrite mechanics as seen in the note
 * above in p4_rewrite_variant: ( a* R: x ) : ( b -- R : b ) will yield
 * ( R: x b ) but we do actually want it to be ( R: x a* ), thereby
 * allowing to track items under the user-given name as long as possible.
 * Likewise we want the _test execution to check identities, i.e. we we
 * want ( a* b* ) ? ( a* a* -- a ) to fail as there is no match here.
 *
 * In the following we start over with an alternative implementation
 * that will walk variant notations in full. In order to simplify and
 * speedup identity checks, we are building up a "catched" buffer. The
 * catched buffer contains per item a pair of match-name<tick>subject-part
 * where subject-part is the name and any overspec type notations from
 * the subject stream. As soon as we fetch another match item from the
 * the match variant notation then we strstr into that buffer whether
 * we did already have a catch for that name. In most cases there is no
 * such thing but in the other case we do some extra checks - the older
 * subject prefix part must be (canonically) identical to the current
 * matched subject prefix part, i.e. the name must be equal and the
 * canonic overspec type must be equal as well. Therefore, the following
 * would fail as well: ( a* a** ) ? ( b* b* -- c ) since the two b's
 * try to match different items "a" and "a*" in the subject buffer.
 */

int p4_equal_item_prefix (pair_t arg, pair_t chk)
{
    struct pair x;
    struct pair y;
    pairdef (&x, arg);
    pairdef (&y, chk);
    if (! narrow_argument_name(&x) ||
        ! narrow_argument_name(&y) ||
        ! pair_equal(&x, &y)) return 0;
    pairdef (&x, arg);
    pairdef (&y, chk);
    if (! narrow_argument_type(&x) ||
        ! narrow_argument_type(&y) ||
        ! equal_type_suffix(&x, &y)) return 0;
    return 1;
}

static int
rewrite_variant_try_test(pair_t stackdef, pair_t inputdef, pair_t reason)
{
    /*
     *
     * implementation: earlier we were simply trying a foreach on all
     * available stack names A..Z and handing it over into a a call to
     * rewrite_stack_test. We can not use the latter call anymore.
     */
# if 0
    struct pair input;
    struct pair stack;
    register unsigned char stk;
    for (stk = 'A'; stk < 'Z'; stk++)
    {
        pairdef (&stack, stackdef);
        pairdef (&input, inputdef);
        if (! narrow_stack0(&input, stk, STK))
            continue; /* not required to match in changer-input */
        if (! narrow_stack0(&stack, stk, STK)) {
            if (! narrow_isempty (&input)) {
                pairdef (reason, &input);
                return 0; } /* must exist if changer-input not empty */
            continue; }
        if (! rewrite_stack_test (&stack, &input, reason))
            return 0;
    }
    return 1; /* all existing stack layouts did match */
# else
    /* instead we step through all stack notations available in the
     * the match variant given to this call. Then we check for its
     * existance in the stackdef variant. For each item in the stack
     * we build the catch buffer and check with it. Right here.
     */
    char catched[BUFLEN];
    int n;
    struct pair stack; /** stack in "stackdef"s */
    struct pair input; /** stack in "inputdef"s */
    unsigned char stk;

    p4_strcpy (catched, " ");
    for (n=0; n < 123 ; n++)
    {
        pairdef (&stack, stackdef);
        pairdef (&input, inputdef);
        if (! narrow_notation (&input, n))
            break; /* no more notations */
        if (narrow_is_proc(&input))
            continue;
        stk = narrow_to_stack (&input);
        if (! narrow_stack0 (&stack, stk, STK)) {
            if (! narrow_isempty (&input)) {
                pairdef (reason, &input);
                return 0; } /* must exist if changer-input not empty */
            continue; }
        if (SHOWTESTS) { p4_outf ("\n(1 %s ))\n", catched); }
        /* rewrite_stack_test(stack, input, reason): */
        {
            char* p;
            struct pair chk;
            struct pair inp; /** arg in "input" */
            struct pair arg; /** arg in "stack" */
            register int m;
            for (m = 0; m < 32; m ++) { /* walk each argument in "input" */
                pairdef (&arg, &stack);
                pairdef (&inp, &input);
                if (! narrow_argument (&inp, m)) break; /*success*/
                if (! narrow_argument (&arg, m)) { /* not enough in stack */
                    if (reason) pairdef (reason, &inp);
                    return 0;
                }
                if (! narrow_good_item_prefix (&arg, &inp) ||
                    ! narrow_argument_name (&inp) || pairlen_(inp) >= 31)
                {
                    if (reason) pairdef (reason, &inp);
                    return 0;
                }
                /* and now for the new checks: */
                {
                    int len = p4_strlen(catched);
                    if (len + pairlen_(inp) > 253) { /* overflow */
                        if (reason) reason->str = reason->end = inp.str;
                        return 0;
                    }
                    p = catched + len; /* := the new "name" start */
                    p4_strcat (p, " ");
                    p4_strncat (p+1, inp.str, pairlen_(inp));
                    p4_strlcat (p, "'", 33);
                    if (SHOWTESTS) { p4_outf ("(2 %s ))\n", catched); }
                    chk.str = p4_strstr (catched, p);
                    /* assert(chk.str) */
                    chk.str += p4_strlen (p);     /* move to stackdef-part */
                    if (*chk.str) { /* check not being the new name */
                        chk.end = p4_strchr (chk.str, ' ');
                        if (! p4_equal_item_prefix (&chk, &arg))
                        {
                            if (reason) pairdef (reason, &inp);
                            return 0;
                        }
                    }
                    /* append item_prefix stackdef part to the new "name" */
                    len = p4_strlen(catched);
                    if (len + pairlen_(arg) > 254) { /* overflow */
                        if (reason) reason->str = reason->end = inp.str;
                        return 0;
                    }
                    p = catched + len;
                    p4_strncat (p, arg.str, pairlen_(arg));
                    p4_strcat (p, " ");
                    if (SHOWTESTS) { p4_outf ("(3 %s ))\n", catched); }
                }
            }
        }
    }
    return 1;
# endif
}

/** "REWRITE-TEST(" ( "stackhelp<rp>" -- ) [EXT]
 * Test whether the given changer would match the current line.
 * assume:
 *      only one changer (if more are provided then only the first is used)
 */
void FXCode (p4_rewrite_test)
{
    struct pair line;
    struct pair pair;
    struct pair reason;

    line_pair (&line);
    if (parse_pair(&pair)) {
        narrow_changer(&pair, 0);
        if (narrow_inputlist(&pair)) {
            if (rewrite_variant_try_test (&line, &pair, &reason)) {
                p4_outs ("oK ");
            }else{
                p4_outs ("No ");
                show_parse_pair(&reason);
            }
        }else p4_outs ("no inputdefs variant found\n");
    }else p4_outs("empty input");
}

/* now, let's go for the real thing:
 *
 * we take the code of _tests again but the "catched" buffer is provided
 * as an argument to allow for proper modularization of algorithms.
 * args:
 *   subject: current stackdef buffer (a single variant)
 *   input: next inputdefs variant to test with
 *   reason: offending notation in the "inputlist" (a stack part)
 */
int p4_rewrite_variant_test(pair_t stacklist, pair_t inputlist,
                            pair_t reason, char* catched, int catchmax)
{
    int n;
    struct pair input;
    struct pair stack;
    unsigned char stk;
    if (! catched)
        return rewrite_stackdef_test (stacklist, inputlist, reason);

    p4_strcpy (catched, " ");
    for (n=0; n < 123 ; n++)
    {
        pairdef (&stack, stacklist);
        pairdef (&input, inputlist);
        if (! narrow_notation (&input, n))
            break; /* no more notations */
        if (narrow_is_proc (&input))
            continue;
        stk = narrow_to_stack (&input); /* get stack char from notation. */
        if (! narrow_stack0 (&stack, stk, STK)) {
            if (! narrow_isempty (&input)) {
                pairdef (reason, &input);
                return 0; } /* must exist if changer-inputlist not empty */
            continue; }
        if (SHOWTESTS) { p4_outf ("\n(1 %s ))\n", catched); }
        /* rewrite_stack_test(stack, input, reason): */
        {
            char* p;
            struct pair chk; /** arg in "catched" */
            struct pair inp; /** arg in "input" */
            struct pair arg; /** arg in "stack" */
            register int m;
            for (m = 0; m < 32; m ++) { /* walk each argument in "input" */
                pairdef (&arg, &stack);
                pairdef (&inp, &input);
                if (! narrow_argument (&inp, m)) break; /*success*/
                if (! narrow_argument (&arg, m)) { /* not enough in stack */
                    if (reason) pairdef (reason, &inp);
                    return 0;
                }
                if (! narrow_good_item_prefix (&arg, &inp) ||
                    ! narrow_argument_name (&inp) || pairlen_(inp) >= 31)
                {
                    if (reason) pairdef (reason, &inp);
                    return 0;
                }
                /* and now for the new checks: */
                {
                    int len = p4_strlen(catched);
                    if (len + pairlen_(inp) > catchmax-2) { /* overflow */
                        if (reason) reason->str = reason->end = inp.str;
                        return 0;
                    }
                    p = catched + len; /* := the new "name" start */
                    p4_strcat (p, " ");
                    p4_strncat (p+1, inp.str, pairlen_(inp));
                    p4_strlcat (p, "'", 33);
                    if (SHOWTESTS) { p4_outf ("(2 %s ))\n", catched); }
                    chk.str = p4_strstr (catched, p);
                    /* assert(chk.str) */
                    chk.str += p4_strlen (p);     /* move to stacklist-part */
                    if (*chk.str) { /* check not being the new name */
                        chk.end = p4_strchr (chk.str, ' ');
                        if (! p4_equal_item_prefix (&chk, &arg))
                        {
                            if (reason) pairdef (reason, &inp);
                            return 0;
                        }
                    }
                    /* append item_prefix stacklist part to the new "name" */
                    len = p4_strlen(catched);
                    if (len + pairlen_(arg) > catchmax-1) { /* overflow */
                        if (reason) reason->str = reason->end = inp.str;
                        return 0;
                    }
                    p = catched + len;
                    p4_strncat (p, arg.str, pairlen_(arg));
                    p4_strcat (p, " ");
                    if (SHOWTESTS) { p4_outf ("(3 %s ))\n", catched); }
                }
            }
        }
    }
    return 1;
}

/* here we feed the "catched" buffer as an additional argument, if
 * it is left empty then it is the same as simple p4_rewrite_stack
 * args:
 *   subject: current stack layout buffer
 *   input: variant to be overlayed with stack layout
 *   output: variant to be rewritten into
 *   sink/sinklen: buffer being rewritten (out)
 * note:
 *   the input and output are expected to be already narrowed to a single
 *   stack item. This routine does walk over each argument and rewrites it.
 * catched:
 *   used to transfer rewrites of names from a different stack.
 */
int p4_rewrite_stack_result (pair_t stack, pair_t input, pair_t output,
                             char* catched, char* sink, int sinklen)
{
    struct pair arg; /** arg in "stack" */
    struct pair inp; /** arg in "input" */
    struct pair out; /** arg in "output" */
    int i, j;
    *sink = '\0';
    if (sinklen < 32) return 0;

    for (i = 32; i >= 0; i --) /* forall args in "stack" */
    {
        pairdef (&arg, stack);
        if (! narrow_argument(&arg, i))
            continue;
        pairdef (&inp, input);
        if (! narrow_argument(&inp, i))
        {   /* copy unchanged input argument */
            if (SHOWCOPIES) p4_outf ("<unchanged stack arg #%i: '%.*s'>\n",
                                     i, Q pairlen_(arg), arg.str);
            if (pairlen_(arg) >= 32) return 0;
            p4_strncat (sink, arg.str, pairlen_(arg));
            if (p4_strlcat (sink, " ", sinklen-32) >= sinklen-32)
                return 0; /* overflow */
            continue;
        }
        break; /* only inputdefs left that are to be changed */
    }

    for (i = 32; i >= 0; i --) /* forall args in "output" */
    {
        pairdef (&out, output);
        if (! narrow_argument(&out, i) || ! narrow_argument_name (&out))
            continue;
        for (j = 32; j >= 0; j --) /* forall args in "input" */
        {
            pairdef (&inp, input);
            if (! narrow_argument(&inp, j) || ! narrow_argument_name(&inp))
                continue;
            /* check if the "input" name is the same as "output" name */
            if (! pair_equal (&inp, &out))
                continue;
            break; /* found */
        }
        if (j >= 0) /* an "input" was found that shall be copied ! */
        {
            pairdef (&inp, input);
            pairdef (&arg, stack);
            if (narrow_argument (&inp, j) &&
                narrow_argument (&arg, j) &&
                narrow_good_item_prefix (&arg, &inp))
            {
                if (SHOWCOPIES) p4_outf("<copying stack %i as %i: '%.*s'>\n"
                                        , j, i, Q pairlen_(arg), arg.str);
                if (pairlen_(arg) >= 32) return 0;
                p4_strncat (sink, arg.str, pairlen_(arg));
                /* -- */
                pairdef (&out, output);
                if (narrow_argument (&out, i) &&
                    narrow_argument_type (&out)) {
                    if (pairlen_(out) >= 32) return 0;
                    p4_strncat (sink, out.str, pairlen_(out));
                }
                pairdef (&arg, stack);
                if (narrow_argument (&arg, j) &&
                    narrow_true_modified (&arg)) {
                    if (pairlen_(arg) >= 32) return 0;
                    p4_strncat (sink, arg.str, pairlen_(arg));
                }
                if (p4_strlcat (sink, " ", sinklen-32) >= sinklen-32)
                    return 0; /* overflow */
                continue; /* at next "output" arg */
            }
        }
        if (catched && pairlen_(out) <= 31)
        {
            char name[33];
            p4_strcpy (name, " ");
            p4_strncat (name, out.str, pairlen_(out));
            p4_strcat (name, "'");
            inp.str = p4_strstr (catched, name);
            if (inp.str) {
                inp.str += p4_strlen(name);
                inp.end = p4_strchr (inp.str, ' ');
                if (SHOWCOPIES) p4_outf ("<copying catched %s%.*s'>\n",
                                         name, Q pairlen_(inp), inp.str);
                if (pairlen_(out) >= 32)
                    return 0;
                p4_strncat (sink, inp.str, pairlen_(inp));
                /* --- */
                pairdef (&out, output);
                if (narrow_argument (&out, i) &&
                    narrow_argument_type (&out)) {
                    if (pairlen_(out) >= 32) return 0;
                    p4_strncat (sink, out.str, pairlen_(out));
                }
                if (p4_strlcat (sink, " ", sinklen-32) >= sinklen-32)
                    return 0; /* overflow */
                continue;
            }
        }/*else:*/
        /* if any of the "if"s above did fail: copy the "output" spec */
        pairdef (&out, output);
        if (! narrow_argument(&out, i))
            continue;
        if (SHOWCOPIES) p4_outf ("<copying out arg %i: '%.*s'>\n",
                                 i, Q pairlen_(out), out.str);
        if (pairlen_(out) >= 32) return 0;
        p4_strncat (sink, out.str, pairlen_(out));
        if (p4_strlcat (sink, " ", sinklen-32) >= sinklen-32)
            return 0;
    }
    return 1;
}


/* here we feed the "catched" buffer as an additional argument, if
 * it is left empty then it is the same as simple p4_rewrite_variant
 * args:
 *   subject: current stack layout buffer
 *   input: variant to be overlayed with stack layout
 *   output: variant to be rewritten into
 *   sink/sinklen: buffer being rewritten (out)
 * note:
 *   it does always narrow to stack items. Therefore the result does
 *   only have stack items again and no proc at all.
 */
int p4_rewrite_variant_result(pair_t stackdef,
                              pair_t inputdef, pair_t outptdef,
                              char* catched, char* sink, int sinklen)
{
    char stackprefix[] = "X: ";
    struct pair stack;
    struct pair input;
    struct pair outpt;
    unsigned char stk;
    unsigned char emptystart = 0;
    int n;
    if (sinklen < 32) return 0;
    *sink = '\0';

    for (n=0; n < 123; n ++)
    {
        pairdef (&stack, stackdef);
        if (! narrow_notation (&stack, n))
            break;
        if (narrow_is_proc (&stack))
            continue;
        stk = narrow_to_stack (&stack);
        if (! stk && narrow_isempty (&stack)) {
            emptystart = 1;
            continue; } /* test later and ... */
        if (stk == STK) emptystart = 0; /* ... do not when STK was seen */
        if (stk) {
	    *stackprefix = stk;
	    p4_strlcat (sink, stackprefix, sinklen); }
        pairdef (&input, inputdef);
        pairdef (&outpt, outptdef);
        if (! narrow_stack0(&outpt, stk, STK))
        {
            if (! narrow_stack0(&input, stk, STK))
            {   /* passthrough stacks not mentioned in inp/out changer */
                if (pairlen_(stack) >= 32) return 0;
                p4_strncat(sink, stack.str, pairlen_(stack));
                continue;
            } else
            {   outpt.str = outptdef->str; outpt.end = outpt.str; }
        } else {
            if (! narrow_stack0(&input, stk, STK))
            {   input.str = inputdef->str; input.end = input.str; }
        }
        if (! p4_rewrite_stack_result (&stack, &input, &outpt, catched,
                                       sink + p4_strlen(sink),
                                       sinklen - p4_strlen(sink)))
            return 0;
    }
    /* if (emptystart):
     * the stackdef variant has only items with an explicit stackhint.
     * however the inputdef variant may have some items without a stackhint
     * thereby expecting to match on defaultstack items in STK (S:).
     * Note some test in between that did reset "emptystart" whenever
     * an explicit S: was seen in the stackdef variant, for which some
     * stackdef()-call will yield either S: or the unnamed area. And
     * therefore this test will only catch on when no S: was present
     * either in the stackdef buffer. - now actually we expect that the
     * inputdef notation does not have any item either about that (it would
     * have not been able to test/match with the inputdef variant). However
     * the outptdef may produce items on a new stack including the default.
     */
    for (stk='A'; stk <= 'Z' ; stk++)
    {
        pairdef (&stack, stackdef);
        if (narrow_stack (&stack, stk))
            continue; /* already gone through rewrite rule above */
        pairdef (&outpt, outptdef);
        if (! narrow_stack (&outpt, stk))
            continue; /* no new stack in outptdef rule */
        if (stk == STK) emptystart = 0;
        if (! narrow_stack (&input, stk))
            input.str = input.end = inputdef->end;
        stack.str = stack.end = stackdef->end;
        *stackprefix = stk; p4_strlcat (sink, stackprefix, sinklen);
        if (! p4_rewrite_stack_result (&stack, &input, &outpt, catched,
                                       sink + p4_strlen(sink),
                                       sinklen - p4_strlen(sink)))
            return 0;
    }
    if (emptystart)
    {
        pairdef (&stack, stackdef);
        pairdef (&input, inputdef);
        pairdef (&outpt, outptdef);
        if (narrow_stack (&stack, 0) &&
            narrow_stack (&input, 0) &&
            narrow_stack (&outpt, 0) &&
            ! narrow_isempty (&outpt))
        {
            /* want to reorder ( R: a S: b ) -> ( b R: a ) ? */
            *stackprefix = STK; p4_strlcat (sink, stackprefix, sinklen);
            if (! p4_rewrite_stack_result (&stack, &input, &outpt, catched,
                                           sink + p4_strlen(sink),
                                           sinklen - p4_strlen(sink)))
                return 0;
        }
    }
    return 1;
}

/* ------------------------------------------------------------------- */
/*
 * so far we have been showing a lot of code to test algorithms that
 * can walk user stack notations and use it to rewrite an existing
 * stack notation with it - mostly just pure string operations. Now
 * we want to combine it all, in that it shall allow us to rewrite
 * things. Here we get a little more complicated on things.
 *
 * First off, the file header paragraphs tell of the parsing and
 * substructuring of a given rewrite notation. The actual rewrite
 * follows this scheme: a matching rewrite IS A single changer
 * in a list of possible changers. If we have a split stack in
 * the input line (and count(stacksplits) == 1 is normal stack)
 * then each of the stack variants needs to have one match in the
 * input variants of the changer. It is allowed that all line
 * variants match with the same input variant of the changer, and
 * we use generally the first match (non the best or longest match).
 * Then all line variants are aligned with their input variant,
 * items below the alignment are copied directly into the output,
 * the rest is dropped first and replaced by the output of the
 * changer. If the changer exhibits multiple variants (i.e. it creates
 * a splitstack) then each single line variant will create that
 * many output variants, possibly items below the alignment must
 * be duplicated. - in short:
 *
 *  SELECT changers(rule) C
 *  WHERE  FORALL variants(line) V
 *         EXIST  variants(input(C)) I
 *         WHERE  V <more-specific-than> I
 *  BEGIN
 *         FORALL variants(output(C)) O
 *           FORALL variants(line) V
 *             RESULT REWRITE V : I(V) => O
 *  END
 *  Therefore,
 *     ( x a* b* 0 | y c# 1 ) : ( c# 1 | x y* 0 -- q 0 | p 1 )
 *  will have a result of
 *     ( x q 0 | x p 1 | y q 0 | y p 1 )
 *  This is unusual though, more realistic is a splistack only in
 *  its two uppermost items, and therefore a rewrite like
 *     ( x a* b* 0 | x c# 1 ) : ( c# 1 | x y* 0 -- q 0 | p 1 )
 *  will have an intermediate result of
 *     ( x q 0 | x p 1 | x q 0 | x p 1 )
 *  which may be subject to a "collapse" operation that looks
 *  for identical variants. Only one instance of each variant
 *  needs to be mentioned, and the real result will be
 *     ( x q 0 | x p 1 )
 *  If you put this output as subject to a 2DROP operation then
 *     ( x q 0 | x p 1 ) : 2DROP ( a b -- )
 *  has an intermediate result of
 *     ( x | x )
 *  that will get collapsed to a simple result of
 *     ( x )
 *
 *  In the following text we try to be consistent in terminology
 *  as to "select" a changer from a ruleset, and a variant subject in
 *  the line buffer is a "split" variant, which has a "match" variant
 *  in the list of variants in the "input" part of the "select"ed changer.
 *  Then each "split" variant of the subject buffer is "aligned" as an
 *  overspecification of its "match" variant, yielding an "overspec"
 *  type identification (which is of interest when the "split" item
 *  is copied into the output). If there are more items in the "split"
 *  variant than in its "match" variant then these are "deep" items that
 *  will be copied unchanged. Each output variant of the "select"ed changer
 *  is called a "template" variant. We try hard to copy names and overspecs
 *  from the subject buffer onto the "result" variant. The walk over all
 *  "templates" and "splits" is called "expansion", and mostly the
 *  intermediate result list is called "expansion" too. Then each "result"
 *  variant in the "expansion" buffer is crosschecked for an identical
 *  earlier "result" which makes it be dropped. This is called "collapse"
 *  operation. Generally we "collapse" right after a single "expansion"
 *  so that the "sink" buffer is the same as "expansion" buffer where
 *  a "collapsed" result may be appended or not (or dropped after append).
 *  in short:
 *     we have a single subject line buffer (list of split variants)
 *     the selected changer has an input / output part (each list of variants)
 *     each split variant as a match variant in the input part.
 *     expansion rewrites each split with all templates in the output part.
 *     collapse checks the result with earlier results
 *     only unique results are appended to the sink buffer
 *                      (or non-unique results dropped from the result buffer)
 *
 *  If you think this is enough then wait for the later routines
 *  which do not only handle "split" stacks but also the case to
 *  memorize some stack variants around controls (IF ELSE ENDIF) where
 *  they are non-active and therefore not subject to rewrites, just until
 *  they are reactived into the subject stack line at the ENDIF. Upon
 *  reactivation we will generally want to "collapse" variants of the
 *  merged list in the result buffer to be the next subject line buffer.
 */

/** return 1 if "inputlist" was narrowed.
 *  return 0 if no good inputdef was found in inputlist variants. */
int
p4_narrow_inputdef_for_stackdef (pair_t inputlist, pair_t stackdef)
{
    struct pair inputdef;
    int n;
    for (n=0 ; n < 123 ; n++)
    {
        pairdef (&inputdef, inputlist);
        if (! narrow_variant (&inputdef, n))
            break;
        if (SHOWSELECT) p4_outf ("<testing inputdef %i '%.*s'>\n", n,
                                 Q pairlen_(inputdef), inputdef.str);
        if (rewrite_stackdef_test (stackdef, &inputdef, 0))
        {
            pairdef (inputlist, &inputdef);
            return 1;
        }
    }
    return 0;
}

int
p4_test_inputlist_with_stacklist (pair_t inputlist, pair_t stacklist)
{
    struct pair stackdef;
    struct pair templist;
    int n; int found = 0;
    for (n=0 ; n < 123 ; n++)
    {
        pairdef (&stackdef, stacklist);
        if (! narrow_variant(&stackdef, n))
            break;
        if (SHOWSELECT) p4_outf ("<testing stackdef %i '%.*s'>\n", n,
                                 Q pairlen_(stackdef), stackdef.str);
        pairdef (&templist, inputlist);
        if (! p4_narrow_inputdef_for_stackdef (&templist, &stackdef))
            return 0;
        else
            found ++;
    }
    return found+1;
}

int
p4_narrow_changer_for_stacklist (pair_t changerlist, pair_t stacklist)
{
    struct pair changer;
    int n;
    for (n=0; n < 123 ; n++)
    {
        pairdef (&changer, changerlist);
        if (! narrow_changer (&changer, n))
            break;
        if (SHOWSELECT) p4_outf ("<testing changer %i '%.*s'>\n", n,
                                 Q pairlen_(changer), changer.str);
        narrow_inputlist (&changer);
        if (p4_test_inputlist_with_stacklist (&changer, stacklist))
        {
            if (SHOWSELECT) p4_outf ("<found at changer %i>\n", n);
            narrow_changer (changerlist, n);
            /* i.e. pairdef (changerlist, &changer); */
            return 1;
        }
    }
    return 0;
}

/* "REWRITE-CHANGER-SELECT(" ( "stackhelp<rp>" )
 * show the matching changer part that matches to current REWRITE-LINE.
 */
void FXCode (p4_rewrite_changer_select)
{
    struct pair pair;
    struct pair line;

    line_pair (&line);
    if (parse_pair(&pair)) {
	if (p4_narrow_changer_for_stacklist(&pair, &line)) {
            show_parse_pair(&pair);
	}else p4_outs("no matching changer found\n");
    }else p4_outs("empty input");
}

int
p4_rewrite_changer_test (pair_t stacklist, pair_t rewriter)
{
    struct pair changerlist = { rewriter->str, rewriter->end };
    return p4_narrow_changer_for_stacklist (&changerlist, stacklist);
}

int
p4_rewrite_changer_expand (pair_t stacklist, pair_t changer,
                           char* sink, int sinklen)
{
    struct pair inputlist;
    struct pair outptlist;
    struct pair stackdef;
    struct pair inputdef;
    struct pair outptdef;
    int i;
    int k;
    if (sinklen < 32) return 0;
    *sink = '\0';

    pairdef (&inputlist, changer);
    pairdef (&outptlist, changer);
    if (! narrow_inputlist (&inputlist))
        return 0; /* oops */
    if (! narrow_outputlist (&outptlist))
        return 0; /* oops */

    for (i=0; i < 123 ; i++)
    {
        pairdef (&stackdef, stacklist);
        if (! narrow_variant (&stackdef, i))
            break;
        pairdef (&inputdef, &inputlist);
        if (! p4_narrow_inputdef_for_stackdef (&inputdef, &stackdef))
            return 0; /* oops */
        for (k=0; k < 123 ; k++)
        {
            pairdef (&outptdef, &outptlist);
            if (! narrow_variant (&outptdef, k))
                break;
            if (*sink) p4_strlcat (sink, "| ", sinklen);
            if (! p4_rewrite_stackdef (&stackdef, &inputdef, &outptdef,
                                       sink + p4_strlen(sink),
                                       sinklen - p4_strlen(sink)))
                return 0;
        }
    }
    return 1;
}

/* "REWRITE-CHANGER-EXPAND(" ( "stackhelp<rp>" -- )
 * show the result after expansion with the changer selected via
 * REWRITE-CHANGER-SELECT from the given changlist. This is not collapsed.
 */
void FXCode (p4_rewrite_changer_expand)
{
    char buffer[BUFLEN];
    struct pair pair;
    struct pair line;

    line_pair (&line);
    if (parse_pair(&pair)) {
	if (p4_narrow_changer_for_stacklist(&pair, &line)) {
            if (p4_rewrite_changer_expand (&line, &pair, buffer, BUFLEN)) {
                p4_outf("\n  ( %s)\n", buffer);
            } else p4_outs ("unable to expand\n");
	}else p4_outs("no matching changer found\n");
    }else p4_outs("empty input");
}

static int
same_variant (pair_t A, pair_t B)
{
    char strA[BUFLEN];
    char strB[BUFLEN];
    int stk, n;
    struct pair stackA;
    struct pair stackB;
    struct pair argA;
    struct pair argB;
    struct pair defA;
    struct pair defB;


    for (stk='A' ; stk <= 'Z'; stk ++)
    {
        pairdef (&stackA, A);
        pairdef (&stackB, B);
        if (! narrow_stack0 (&stackA, stk, STK))
        {
            if (! narrow_stack0 (&stackB, stk, STK))
                continue;
            return 0;
        }else{
            if (! narrow_stack0 (&stackB, stk, STK))
                return 0;
        }

        for (n=0; n < 123 ; n++)
        {
            pairdef (&argA, &stackA);
            pairdef (&argB, &stackB);
            if (! narrow_argument (&argA, n))
            {
                if (! narrow_argument (&argB, n))
                    break;
                return 0;
            }else{
                if (! narrow_argument (&argB, n))
                    return 0;
            }
            pairdef (&defA, &argA);
            pairdef (&defB, &argB);
            if (! narrow_argument_name(&defA) ||
                ! narrow_argument_name(&defB) ||
                ! pair_equal (&defA, &defB))
                return 0;
            pairdef (&defA, &argA);
            pairdef (&defB, &argB);
            if (! narrow_argument_type(&defA) ||
                ! narrow_argument_type(&defB) ||
                ! canonic_type (&defA, strA, strA+BUFLEN) ||
                ! canonic_type (&defB, strB, strB+BUFLEN) ||
                p4_strcmp (strA, strB))
                return 0;
        }
    }
    return 1;
}

/** append all variants in "expand". This routine does collapse
 *  as it does actually narrow_variant on all parts of "result"
 *  and narrow_variant on all parts of "expand" so that none of
 *  the expand-variants is being copied that does already exist
 *  in the result descriptor. The equal-test is done with
 *  "same_variant".
 */
static int
append_new_variants (char* expand, char* result, int resultlen)
{
    int m,n;
    for (m=0; m < 123 ; m++)
    {
        struct pair new1 = { expand, expand+p4_strlen(expand) };
        if (! narrow_variant (&new1, m))
            break;
        for (n=0; n < 123 ; n++)
        {
            struct pair old1 = { result, result+p4_strlen(result) };
            if (! narrow_variant (&old1, n))
            {
                if (*result) p4_strlcat (result, "| ", resultlen);
                if (resultlen-p4_strlen(result) < pairlen_(new1)) return 0;
                p4_strncat (result, new1.str, pairlen_(new1));
                break;
            }
            if (same_variant (&old1, &new1))
                break; /* do not p4_strcat this new1 variant */
        }
    }
    return 1;
}

int
p4_rewrite_changer_result (pair_t subject, pair_t changer,
                           char* result, int result_length)
{
    char expand[BUFLEN];
    struct pair inputdefs;
    struct pair outputdefs;
    struct pair subj;
    struct pair match;
    struct pair templat;
    int i,k;
    if (result_length < 32) return 0;
    *result = '\0';

    pairdef (&inputdefs, changer);
    pairdef (&outputdefs, changer);
    if (! narrow_inputlist (&inputdefs))
        return 0; /* oops */
    if (! narrow_outputlist (&outputdefs))
        return 0; /* oops */

    for (i=0; i < 123 ; i++)
    {
        pairdef (&subj, subject);
        if (! narrow_variant (&subj, i))
            break;
        pairdef (&match, &inputdefs);
        if (! p4_narrow_inputdef_for_stackdef (&match, &subj))
            return 0; /* oops */
        for (k=0; k < 123 ; k++)
        {
            pairdef (&templat, &outputdefs);
            if (! narrow_variant (&templat, k))
                break;
            if (! p4_rewrite_stackdef (&subj, &match, &templat,
                                       expand, BUFLEN))
                return 0;
            if (! append_new_variants(expand, result, result_length))
                return 0;
        }
    }
    return 1;
}

/* "REWRITE-CHANGER-RESULT(" ( "stackhelp<rp>" -- )
 * show the result after expansion with the changer selected via
 * REWRITE-CHANGER-SELECT from the given changlist. The result is collapsed.
 */
void FXCode (p4_rewrite_changer_result)
{
    char buffer[BUFLEN];
    struct pair pair;
    struct pair line;

    line_pair (&line);
    if (parse_pair(&pair)) {
	if (p4_narrow_changer_for_stacklist(&pair, &line)) {
            if (p4_rewrite_changer_result (&line, &pair, buffer, BUFLEN)) {
                p4_outf("\n  ( %s)\n", buffer);
            } else p4_outs ("unable to expand\n");
	}else p4_outs("no matching changer found\n");
    }else p4_outs("empty input");
}

/* ------------------------------------------------------------------- */

int
p4_narrow_match_variant_for (pair_t inputdefs, pair_t subj,
                             char* catched, int catchmax)
{
    struct pair match;
    int n;
    for (n=0 ; n < 123 ; n++)
    {
        pairdef (&match, inputdefs);
        if (! narrow_variant (&match, n))
            break;
        if (SHOWSELECT) p4_outf ("<testing match %i '%.*s'>\n", n,
                                 Q pairlen_(match), match.str);
        if (p4_rewrite_variant_test (subj, &match, 0, catched, catchmax))
        {
            pairdef (inputdefs, &match);
            return 1;
        }
    }
    return 0;
}

int
p4_narrow_variant_for (pair_t inputdefs, pair_t subj)
{
    struct pair match;
    int n;
    for (n=0 ; n < 123 ; n++)
    {
        pairdef (&match, inputdefs);
        if (! narrow_variant (&match, n))
            break;
        if (SHOWSELECT) p4_outf ("<testing match %i '%.*s'>\n", n,
                                 Q pairlen_(match), match.str);
        if (rewrite_variant_try_test (subj, &match, 0))
        {
            pairdef (inputdefs, &match);
            return 1;
        }
    }
    return 0;
}

int
p4_test_enough_variants_for (pair_t inputdefs, pair_t subject)
{
    struct pair subj;
    int n;
    for (n=0 ; n < 123 ; n++)
    {
        pairdef (&subj, subject);
        if (! narrow_variant(&subj, n))
            break;
        if (SHOWSELECT) p4_outf ("<testing subj %i '%.*s'>\n", n,
                                 Q pairlen_(subj), subj.str);
        if (! p4_narrow_variant_for (inputdefs, &subj))
            return 0;
    }
    return 1;
}

int
p4_narrow_changer_for (pair_t rewriter, pair_t subject)
{
    struct pair changer;
    int n;
    for (n=0; n < 123 ; n++)
    {
        pairdef (&changer, rewriter);
        if (! narrow_changer (&changer, n))
            break;
        if (SHOWSELECT) p4_outf ("<testing changer %i '%.*s'>\n", n,
                                 Q pairlen_(changer), changer.str);
        narrow_inputlist (&changer);
        if (p4_test_enough_variants_for (&changer, subject))
        {
            if (SHOWSELECT) p4_outf ("<found at changer %i>\n", n);
            narrow_changer (rewriter, n); /* pairdef (rewriter, &changer); */
            return 1;
        }
    }
    return 0;
}

/* "REWRITE-SELECT(" ( "stackhelp<rp>" -- )
 * show the matching changer part that matches to current REWRITE-LINE.
 */
void FXCode (p4_rewrite_select)
{
    struct pair pair;
    struct pair line;

    line_pair (&line);
    if (parse_pair(&pair)) {
	if (p4_narrow_changer_for(&pair, &line)) {
            show_parse_pair(&pair);
	}else p4_outs("no matching changer found\n");
    }else p4_outs("empty input");
}

int
p4_rewrite_test (pair_t subject, pair_t rewriter)
{
    struct pair test = { rewriter->str, rewriter->end };
    return p4_narrow_changer_for (&test, subject);
}

int
p4_rewrite_expand (pair_t subject, pair_t changer,
                   char* sink, int sinklen)
{
    char catched[BUFLEN]; /** store name-assocs from the match operation */
    struct pair inputdefs;
    struct pair outputdefs;
    struct pair subj;   /** variant of subject */
    struct pair match;   /** first matching variant of inputdefs for "subj" */
    struct pair templat; /** variant of outputdefs */
    int i;
    int k;
    if (sinklen < 32) return 0;
    *sink = '\0';

    pairdef (&inputdefs, changer);
    pairdef (&outputdefs, changer);
    if (! narrow_inputlist (&inputdefs))
        return 0; /* oops */
    if (! narrow_outputlist (&outputdefs))
        return 0; /* oops */

    for (i=0; i < 123 ; i++)
    {
        pairdef (&subj, subject);
        if (! narrow_variant (&subj, i))
            break;
        pairdef (&match, &inputdefs);
        if (! p4_narrow_match_variant_for (&match, &subj, catched, BUFLEN))
            return 0; /* oops */
        for (k=0; k < 123 ; k++)
        {
            pairdef (&templat, &outputdefs);
            if (! narrow_variant (&templat, k))
                break;
            if (*sink) p4_strlcat (sink, "| ", sinklen);
            if (! p4_rewrite_variant_result (&subj, &match, &templat,
                                             catched,
                                             sink + p4_strlen(sink),
                                             sinklen - p4_strlen(sink)))
                return 0;
        }
    }
    return 1;
}

/* "REWRITE-EXPAND(" ( "stackhelp<rp>" -- )
 * show the result after expansion with the changer selected via
 * REWRITE-SELECT from the given changlist. This is not collapsed.
 */
void FXCode (p4_rewrite_expand)
{
    char buffer[BUFLEN];
    struct pair pair;
    struct pair line;

    line_pair (&line);
    if (parse_pair(&pair)) {
	if (p4_narrow_changer_for(&pair, &line)) {
            if (p4_rewrite_expand (&line, &pair, buffer, BUFLEN)) {
                p4_outf("\n  ( %s)\n", buffer);
            } else p4_outs ("unable to expand\n");
	}else p4_outs("no matching changer found\n");
    }else p4_outs("empty input");
}

int
p4_rewrite_result (pair_t subject, pair_t changer,
                   char* result, int result_length)
{
    char catched[BUFLEN]; /** store name-assocs from the match operation */
    char expand[BUFLEN];
    struct pair inputdefs;
    struct pair outputdefs;
    struct pair subj;   /** variant of subject */
    struct pair match;   /** first matching variant of inputdefs for "subj" */
    struct pair templat; /** variant of outputdefs */
    int i,k;
    if (result_length < 32) return 0;
    *result = '\0';

    pairdef (&inputdefs, changer);
    pairdef (&outputdefs, changer);
    if (! narrow_inputlist (&inputdefs))
        return 0; /* oops */
    if (! narrow_outputlist (&outputdefs))
        return 0; /* oops */

    for (i=0; i < 123 ; i++)
    {
        pairdef (&subj, subject);
        if (! narrow_variant (&subj, i))
            break;
        pairdef (&match, &inputdefs);
        if (! p4_narrow_match_variant_for (&match, &subj, catched, BUFLEN))
            return 0; /* oops */
        for (k=0; k < 123 ; k++)
        {
            pairdef (&templat, &outputdefs);
            if (! narrow_variant (&templat, k))
                break;
            if (! p4_rewrite_variant_result (&subj, &match, &templat,
                                             catched, expand, BUFLEN))
                return 0;
            if (! append_new_variants(expand, result, result_length))
                return 0;
        }
    }
    return 1;
}

/* REWRITE-RESULT ( "changerlist<rp>" )
 * show the result after expansion with the changer selected via
 * REWRITE-SELECT from the given changlist. The result is collapsed.
 */
void FXCode (p4_rewrite_result)
{
    char buffer[BUFLEN];
    struct pair pair;
    struct pair line;

    line_pair (&line);
    if (parse_pair(&pair)) {
	if (p4_narrow_changer_for(&pair, &line)) {
            if (p4_rewrite_result (&line, &pair, buffer, BUFLEN)) {
                p4_outf("\n  ( %s)\n", buffer);
            } else p4_outs ("unable to expand\n");
	}else p4_outs("no matching changer found\n");
    }else p4_outs("empty input");
}

/*

     needs stackhelp-ext   : x |( a b -- a a b ) >r dup r> ;
                           : y |( a b -- a a b ) >r dup r> ;

*/

/* ------------------------------------------------------------------- */
/*
 *
 *
 *                        the outer interface
 *
 *
 */
/* ------------------------------------------------------------------- */

typedef struct {  p4cell len; const char* str; p4xt xt; } stackhelp_body;

#define IS_BODY_CODE(xt) (*P4_TO_CODE(xt) == PFX(p4_two_constant_RT))
#define IS_REAL_CODE(xt) (*P4_TO_CODE(xt) == PFX(p4_variable_RT))

p4_char_t* p4_next_search_stackhelp(p4_namebuf_t* nfa, const p4_char_t* word, p4cell len)
{
    int guard = 0;
 again:
    if (++guard > 255) { P4_fail("infinite loop"); return 0; }
    nfa = p4_next_search_wordlist (nfa, word, len, PFE.stackhelp_wl);
    if (! nfa) return 0;
    p4xt xt = p4_name_from (nfa);
    if (IS_BODY_CODE(xt) || IS_REAL_CODE(xt)) return nfa;
    else goto again;
}

p4_char_t* p4_search_stackhelp (const p4_char_t* word, p4cell len)
{
    p4_namebuf_t* nfa = p4_search_wordlist (word, len, PFE.stackhelp_wl);
    if (! nfa) return 0;
    p4xt xt = p4_name_from (nfa);
    if (IS_BODY_CODE(xt) || IS_REAL_CODE(xt)) return nfa;
    else return p4_next_search_stackhelp (nfa, word, len);
}

/** FIND word in the search-order
 *  and find a matching stackhelp entry in the stackhelp-wordlist.
 *  We do check for a matching XT to allow for multiple words with
 *  the same name. If this find-routine is too specific then try
 *  again with a direct search.
 */
stackhelp_body* p4_find_stackhelp_body(const p4_char_t* word, p4cell len)
{
    p4_namebuf_t* nfa = p4_find (word, len);
    p4xt    xt = 0; if (nfa) xt = p4_name_from (nfa);
    if (! xt) return 0;

    int guard = 0;
    p4_char_t* help = p4_search_wordlist (word, len, PFE.stackhelp_wl);
    while (help && ++guard < 255) {
        p4xt found = p4_name_from (help);
        if (IS_BODY_CODE(found))
        {
            stackhelp_body * body = (void*) P4_TO_BODY (found);
            if (body->xt && body->xt == xt) return body;
        }
        help = p4_next_search_wordlist (help, word, len, PFE.stackhelp_wl);
    }
    return 0;
}

static void FXCode (add_last_stackhelp)
{
    int len = pairlen_(CHK.word);
    if (! CHK.last) return;
    p4xt xt = p4_name_from (CHK.last);
    p4_header_comma (NAMEPTR(CHK.last), NAMELEN(CHK.last), PFE.stackhelp_wl);
    FX_RUNTIME1(p4_two_constant);
    FX_COMMA (len);   /* stackhelp_body.len */
    FX_COMMA (0);     /* stackhelp_body.str */
    FX_COMMA (xt);    /* stackhelp_body.xt  */
    ((void**)(PFE.dp))[-2] = PFE.dp;
    p4_memcpy (PFE.dp, CHK.word.str, len);
    PFE.dp += len;
    FX (p4_align);
    /* p4_outf("</xt=%p[%p]>", xt, CHK.last); */
    CHK.last = 0;
}

void FXCode (p4_stackhelp_when_done) {
    if (SHOWAFTER) p4_outs("<registering [DONE]>");
    CHK.after[CHK.afters++] = PFX(add_last_stackhelp);
}

/** "|(" ( [string<rp>] -- ) [EXT]
 *  add a checkstack notation for the LAST word or just try to
 *  match the given notation with the stacklayout traced so
 *  far - possibly casting a few types as needed.
 */
void FXCode (p4_stackhelpcomment)
{
    p4_word_parse(')');
    if (PFE.word.len >= 255) return;

    if (find_changer ((char*) PFE.word.ptr, (char*) PFE.word.ptr + PFE.word.len))
    {  /* there seems to be a stack notation here */
        if (LAST && CHK.last != (p4char*) LAST)
        {
            /* a new word definition is being started */
            p4_memcpy (CHK.word.str, PFE.word.ptr, PFE.word.len);
            CHK.word.end = CHK.word.str + PFE.word.len;
            CHK.last = LAST;
            p4_memset (CHK.depth, 0, sizeof(CHK.depth));
	    /* and init the line test buffer */
            p4_memcpy (CHK.line.str, PFE.word.ptr, PFE.word.len);
            CHK.line.end = CHK.line.str + PFE.word.len;
            {   /* narrow inputdefs: */
                char* x = find_nextchanger_(CHK.line.str, CHK.line.end);
                if (x) CHK.line.end = x-1;
            }
            if (SHOW) p4_outf ("\\ |( %.*s ) \n",
                               Q pairlen_(CHK.line), CHK.line.str);
            return;
        }else{
            /* inside a definition, we need a static cast */
            return;
        }
    } /* no ( ... -- ... ) was found => match stack notation here
       * and send out error message when it does not match :-)=) */


    return;
}

/** implicit PFE.word.ptr / PFE.word.len args */
void p4_stackhelps(void)
{
    p4char* nfa = p4_search_stackhelp (PFE.word.ptr, PFE.word.len);
    if (! nfa)
    {
        p4_outf ("\n: %.*s has no stackhelp, sorry. ",
                 Q PFE.word.len, PFE.word.ptr);
        return;
    } /*else*/
    do {
        p4xt xt = p4_name_from (nfa);
        if (! IS_BODY_CODE(xt))
        {
            p4_outf ("\n: %.*s has complex behavior. ",
                     NAMELEN(nfa), NAMEPTR(nfa));
        }else{
            stackhelp_body * body = (void*) P4_TO_BODY(xt);
            p4_outf ("\n: %.*s ( %.*s ) ",
                     Q NAMELEN(nfa), NAMEPTR(nfa),  Q body->len, body->str);
        }
        nfa = p4_next_search_stackhelp(nfa, PFE.word.ptr, PFE.word.len);
    } while (nfa);
}

/** STACKHELPS ( [name] -- ) [EXT]
 *  show all possible stackhelps for this name.
 */
void FXCode (p4_stackhelps)
{
    p4_word_parseword (' '); *DP = 0; /* PRASE-WORD-NOHERE */
    p4_stackhelps();
}

/** STACKHELP ( [name] -- ) [EXT]
 *  show the stackhelp info registered for this name.
 */
void FXCode (p4_stackhelp)
{
    p4_word_parseword (' '); *DP = 0; /* PRASE-WORD-NOHERE */
    { /** try XT match */
        stackhelp_body* body = p4_find_stackhelp_body(
            PFE.word.ptr, PFE.word.len);
        if (body) {
            p4_outf ("\n   : %.*s ( %.*s ) ", /* with three spaces */
                     Q PFE.word.len, PFE.word.ptr, Q body->len, body->str);
            return;
        }
    }
    p4_stackhelps();
}

/** STACKHELP-WORDLIST ( -- stackhelp-wordlist* ) [EXT]
 */

/* -------------------------------------------------------------- REWRITE */

void
p4_stackhelp_layout (char* str)
{
    p4_strcpy (CHK.line.str, str);
    CHK.line.end = CHK.line.str + p4_strlen(str);
}


/*
 * At the moment we store only one stackline per current definition
 * being underway. We try to match the value/type-names in that
 * stackline with the stackinfo detected by the caller. With the
 * help of the stackinfo the stackline is rewritten to reflect the
 * changes that the word would produce at runtime at that point
 * in the definition.
 *   str/end = changer
 *   name/len = ignored
 *   CHK.line = buffer to be modified
 * return:
 *  true if the changer did match and the CHK.line buffer modified
 */
int p4_stackhelp_rewrite (const char* str, const char* end,
			  const p4_char_t* name, int len)
{
    char buffer[BUFLEN];
    struct pair reason;
    struct pair line;
    struct pair inputdefs = { str, end };

    line_pair (&line);
    if (0)
    {
        struct pair outputdefs = { str, end };
        if (narrow_inputlist(&inputdefs)) {
            if (narrow_outputlist(&outputdefs)) {
                if (rewrite_stack_test (&line, &inputdefs, &reason)) {
                    if (p4_rewrite_stack (&line, &inputdefs, &outputdefs,
                                          buffer, BUFLEN))
                    {
                        if (SHOW) p4_outf ("\\ |( %s) ", buffer);
                        p4_strcpy (CHK.line.str, buffer);
                        CHK.line.end = CHK.line.str + p4_strlen(buffer);
                        return 1;
                    }else p4_outs ("\\ error during rewriting ");
                }else{
                    p4_outs ("\\ |( [not rewritable] ) ");
                }
            }else p4_outs ("\\ |( no outputdefs changer found )");
        }else p4_outs ("\\ |( no inputdefs stack found )");
    } else if (0) {
        if (p4_rewrite_changer_test (&line, &inputdefs)) {
            if (p4_rewrite_changer_result (&line, &inputdefs, buffer, BUFLEN))
            {
                if (SHOW) p4_outf ("\\ |( %s) ", buffer);
                p4_strcpy (CHK.line.str, buffer);
                CHK.line.end = CHK.line.str + p4_strlen(buffer);
                return 1;
            } else p4_outs ("\\ error during rewriting ");
        }else{
            p4_outs ("\\ |( [not rewritable] ) ");
        }
    } else {
        if (p4_rewrite_test (&line, &inputdefs)) {
            if (p4_narrow_changer_for(&inputdefs, &line)) {
                if (p4_rewrite_result (&line, &inputdefs, buffer, BUFLEN))
                {
                    if (SHOW) p4_outf ("\\ |( %s) ", buffer);
                    p4_strcpy (CHK.line.str, buffer);
                    CHK.line.end = CHK.line.str + p4_strlen(buffer);
                    return 1;
                } else p4_outs ("\\ error during rewriting ");
            } else p4_outs("\\ could not narrow good changer ");
        }else{
            p4_outs ("\\ |( [not rewritable] ) ");
        }
    }

    return 0;
}

/** call it after p4_stackhelp_rewrite : it does consume any
 *  stackhelp infos that ought to be executed to be a check.
 *  This applies also to exitpoints!  - no argument as it does
 *  work on the CHK.line internal buffer */
int p4_stackhelp_execute_procs (const char* str, const char* end)
{
    int i;
    for (i=0; i < 123; i++) {
        struct pair proc = { str, end };
        if (narrow_notation(&proc, i)) {
            if (! narrow_is_proc(&proc))
                continue;
            const char* colon = p4_memchr(proc.str, ':', pairlen_(proc));
            if (! colon) colon = proc.end-1;
            while (colon > proc.str && p4_isspace(*colon)) colon--;
            int procclen = (colon+1)-proc.str;
            p4_namebuf_t* nfa = p4_search_wordlist (
                (p4_char_t*) proc.str, procclen, PFE.stackhelp_wl);
            if (! nfa) {
                if (! p4_memchr(proc.str, '[', procclen) ||
                    ! p4_memchr(proc.str, '<', procclen))
                    p4_outf("<no such proc: '%.*s'>", procclen, proc.str);
            } else {
                p4cell* old_sp = SP;
                FX_PUSH(proc.str);
                FX_PUSH(procclen);
                p4xt xt = p4_name_from(nfa);
                p4_call (xt);
                SP = old_sp;
            }
        }
    }
    return 1;
}

/*
 * This one is called (in compile mode) for each word with the help
 * of an interpret-hook. We look for a stackhelp info string and
 * detect the input/output depths in that stackhelp info but only
 * for the given "stk" variant. The return value is the diff of
 * the input/outputdefs and that can be added to a runtime-depth
 * in the caller routine.
 */
int stackdepth_change (const char* str, const char* end, unsigned char stk,
                       const p4_char_t* name, int len)
{
    if (SHOW) { /* debugging */
        int i_depth = input_depth (str, end, stk);
        int o_depth = output_depth (str, end, stk);
        if (i_depth || o_depth) {
            if (name)
                fprintf (stderr, "\\ %.*s (%c: [%i]--[%i])\n", len, name, stk,
                         i_depth, o_depth);
            else
                fprintf (stderr, "\\    (%c: [%i]--[%i])\n", stk,
                         i_depth, o_depth);
        }
    }
    return output_depth (str, end, stk) - input_depth (str, end, stk);
}

void
p4_stackdepth_change(const char* changer, const char* end,
		     const p4_char_t* name, int len)
{
    register unsigned char stk;
    for (stk = 'A'; stk < 'Z'; stk++)
    {
        register int change;
        if (CHK.depth[stk-'A'] > 4444) continue;
        change = stackdepth_change (changer, end, stk, name, len);
        if (change > 4444)
            CHK.depth[stk-'A'] = 8888;
        else
            CHK.depth[stk-'A'] += change;
    }
}

void
p4_stackdepth_invalid (unsigned char stk)
{
    CHK.depth[stk-'A'] = 8888;
}

int
p4_stackhelp_interpret_number (const p4_char_t* word, int len)
{
    /* replicates engine-sub.c:p4_interpret_number */

    p4dcell d;
    p4cell old_dpl = p4_DPL;
    if (p4_number_question (word, len, &d)) {
        if (! p4_isspace(CHK.line.end[-1])) {
            CHK.line.end[0] = ' '; CHK.line.end[1] = 0;
        } else CHK.line.end[0] = 0;

        if (p4_DPL >= 0) {
            if (d.hi) p4_strcat(CHK.line.end, "88,");
            else p4_strcat(CHK.line.end, "0,");
            CHK.line.end = p4_strchr(CHK.line.end, 0);
        }

        if (d.lo) p4_strcat(CHK.line.end, "88# ");
        else p4_strcat(CHK.line.end, "0# ");
        CHK.line.end = p4_strchr(CHK.line.end, 0);

        p4_DPL = old_dpl; return 1;
    }
    p4_DPL = old_dpl; return 0;
}

int
p4_stackhelp_interpret_body (stackhelp_body* body, const p4_char_t* word, int len) {
    if (p4_stackhelp_rewrite (body->str, body->str + body->len, word, len))
        p4_stackhelp_execute_procs (body->str, body->str + body->len);
    p4_stackdepth_change (body->str, body->str + body->len, word, len);
    return 1;
}


/*
 * This one is called (in compile mode) for each word with the help
 * of an interpret-hook. We look for a stackhelp info string and
 * try to interpret it for the local stackhelp buffers
 */
int
p4_stackhelp_interpret_find (const p4_char_t* word, int len)
{
    {   /** use XT specific stackhelp (if any) */
        stackhelp_body* body = p4_find_stackhelp_body (word, len);
        if (body) return p4_stackhelp_interpret_body(body, word, len);
    }   /** or fallback to first stackhelp available */

    p4char* nfa = p4_search_stackhelp (word, len);
    if (! nfa) return 0;

    p4xt xt = p4_name_from (nfa);
    if (IS_BODY_CODE(xt))
    {
        stackhelp_body* body = (stackhelp_body*) P4_TO_BODY(xt);
        return p4_stackhelp_interpret_body (body, word, len);
    }else if (IS_REAL_CODE(xt))
    {
        /* note: the callframe layout is subject to change!! */
        p4cell* info = P4_TO_BODY(xt);
        if (*info &&
            ((int (*)(const p4_char_t*,int))(*info))(word, len)) { return 1; }
    }
    return 0;
}

void
p4_stackhelp_interpret_invalid(void)
{
    register unsigned char stk;

    for (stk = 'A'; stk < 'Z'; stk++) p4_stackdepth_invalid (stk);
    if (CHK.line.end > CHK.line.str) {
        if (p4_isspace(CHK.line.end[-1])) CHK.line.end[-1] = '\'';
        else *CHK.line.end++ = '\'';
        *CHK.line.end++ = '\''; *CHK.line.end = 0;
    }
}

void FXCode (p4_stackhelp_exitpoint)
{
    register unsigned char stk;
    for (stk = 'A'; stk < 'Z'; stk++)
    {
        int i_depth = input_depth (CHK.word.str, CHK.word.end, stk);
        int o_depth = output_depth (CHK.word.str, CHK.word.end, stk);
        if (CHK.depth[stk-'A'] < 4444 &&
            CHK.depth[stk-'A'] != o_depth-i_depth)
        {
            if (SHOWRESULT) {
                p4_outf ("\\ * WARNING: seen stackchange (%c: [%i]--[%i])"
                         " for\n", stk, i_depth, i_depth+CHK.depth[stk-'A']);
                p4_outf ("\\ : %.*s |( %.*s) definition with "
                         "(%c: [%i]--[%i]) but\n",
                         NAMELEN(CHK.last), NAMEPTR(CHK.last),
                         Q pairlen_(CHK.word), CHK.word.str,
                         stk, i_depth, o_depth);
            }
        }else if (i_depth || o_depth) { /* debugging */
            if (SHOWRESULT) {
                p4_outf ("\\ : %.*s |( %.*s) definition i.e. "
                         "(%c: [%i]--[%i])\n",
                         Q NAMELEN(CHK.last), NAMEPTR(CHK.last),
                         Q pairlen_(CHK.word), CHK.word.str,
                         stk, i_depth, o_depth);
            }
        }
    }
    if (SHOWRESULT) {
        struct pair pair = { CHK.word.str, CHK.word.end };
        if (narrow_inputlist (&pair))
        {
            p4_outf ("\\ : %.*s |( %.*s-- %.*s) result stack at '%.*s'\n",
                     Q NAMELEN(CHK.last), NAMEPTR(CHK.last),
                     Q pairlen_(pair), pair.str,
                     Q pairlen_(CHK.line), CHK.line.str,
                     Q PFE.word.len, PFE.word.ptr);
            /* fixme: we may want to have a result message whether
             * the line.str and word.str are actually about the same.
             * and optional that may even yield a throw or something.
             */
        }
    }
}

void FXCode (p4_stackhelp_when_exit) {
    if (SHOWAFTER) p4_outs("<registering [EXIT]>");
    CHK.after[CHK.afters++] = PFX(p4_stackhelp_exitpoint);
}

/*
 * This one is the actual interpret-hook. The EXIT-point detection is
 * currently a bit scary, we just check for a straying ";"-word in
 * the text input stream. That should be changed, ye know, possibly
 * extending P4COMPILES-words with a bitflag for exactly that.
 */
static p4ucell
FXCode (p4_interpret_find_stackhelp) /* hereclean */
{
    if (STATE) {
        CHK.notdone =
            ! p4_stackhelp_interpret_find(PFE.word.ptr, PFE.word.len);
    }

    p4ucell done = CHK.interpret[INTERPRET_FIND]();

    if (CHK.afters) { int i;
        for (i=0; i < CHK.afters; ++i) {
            if (SHOWAFTER) p4_outs("<running after>");
            CHK.after[i] ();
        }
        CHK.afters = 0;
    }

    if (done && CHK.notdone) {
        p4_stackhelp_interpret_invalid();
        CHK.notdone = 0;
    }

    /** if not done then number will be tried next. */
    return done;
}

static p4ucell
FXCode (p4_interpret_number_stackhelp)
{
    if (STATE) {
        CHK.notdone =
            ! p4_stackhelp_interpret_number (PFE.word.ptr, PFE.word.len);
    }

    if (CHK.notdone) {
        p4_stackhelp_interpret_invalid();
        CHK.notdone = 0;
    }

    return CHK.interpret[INTERPRET_NUMBER]();
}



static const p4_char_t stackhelp_wl[] = "[STACKHELP]";

static void FXCode_RT(stackhelp_deinit)
{  FX_USE_BODY_ADDR {
    register struct stackhelp* set = (struct stackhelp*) FX_POP_BODY_ADDR[0];
    P4_note1 ("clean stackhelp area %p", set);
    PFE.stackhelp_wl = 0;
    PFE.interpret[INTERPRET_NUMBER] = CHK.interpret[INTERPRET_NUMBER];
    PFE.interpret[INTERPRET_FIND]   = CHK.interpret[INTERPRET_FIND];
}}

/**
 * we use interpret[7] as the channel hook which is the first channel
 * being executed - in other words, the stackcheck will see the word
 * before it is compiled/immexecuted. There we can hypothesize about
 * the actual result of the word.
 */
static void FXCode(stackhelp_init)
{
    PFE.stackhelp_wl = p4_find_wordlist (stackhelp_wl,sizeof(stackhelp_wl)-1);
    CHK.interpret[INTERPRET_NUMBER] = PFE.interpret[INTERPRET_NUMBER];
    PFE.interpret[INTERPRET_NUMBER] = PFX (p4_interpret_number_stackhelp);
    CHK.interpret[INTERPRET_FIND]   = PFE.interpret[INTERPRET_FIND];
    PFE.interpret[INTERPRET_FIND]   = PFX (p4_interpret_find_stackhelp);
    p4_forget_word ("stackhelp:%i", (p4cell) slot,
                    PFX(stackhelp_deinit), (p4cell) &CHK);
    SHOWRESULT = 1;
}

static void FXCode(stackhelp_debug) {
    FX_PUSH(CHK.debug);
}

extern const p4Words P4WORDS(core_check);
extern const p4Words P4WORDS(block_check);
extern const p4Words P4WORDS(cdecl_check);
extern const p4Words P4WORDS(chain_check);
extern const p4Words P4WORDS(chainlist_check);
extern const p4Words P4WORDS(file_check);
extern const p4Words P4WORDS(memory_check);
extern const p4Words P4WORDS(string_check);
extern const p4Words P4WORDS(stackhelp_check);
extern const p4Words P4WORDS(exception_check);
extern const p4Words P4WORDS(facility_check);
extern const p4Words P4WORDS(misc_check);
extern const p4Words P4WORDS(debug_check);
extern const p4Words P4WORDS(forth_83_check);

P4_LISTWORDSET (stackhelp) [] =
{
    P4_SLOT("", &slot),
    P4_SSIZ("", sizeof(struct stackhelp)),

    P4_INTO ("EXTENSIONS", 0),
    P4_IXco ("|(", p4_stackhelpcomment),
    P4_IXco ("STACKHELP", p4_stackhelp),
    P4_IXco ("STACKHELPS", p4_stackhelps),
    P4_IVOC ("[STACKHELP]", 0),
    P4_DVaL ("STACKHELP-WORDLIST", stackhelp_wl),
    P4_FXco ("STACKHELP-DEBUG", stackhelp_debug),

    P4_FXco ("NARROW-CHANGER(", p4_narrow_changer),
    P4_FXco ("NARROW-INPUTLIST(",  p4_narrow_inputlist),
    P4_FXco ("NARROW-OUTPUTLIST(", p4_narrow_outputlist),
    P4_FXco ("NARROW-INPUT-VARIANT(",  p4_narrow_input_variant),
    P4_FXco ("NARROW-OUTPUT-VARIANT(", p4_narrow_output_variant),
    P4_FXco ("NARROW-INPUT-STACK(",  p4_narrow_input_stack),
    P4_FXco ("NARROW-OUTPUT-STACK(", p4_narrow_output_stack),
    P4_FXco ("NARROW-INPUT-ARGUMENT(",  p4_narrow_input_argument),
    P4_FXco ("NARROW-OUTPUT-ARGUMENT(", p4_narrow_output_argument),
    P4_FXco ("NARROW-INPUT-ARGUMENT-NAME(",  p4_narrow_input_argument_name),
    P4_FXco ("NARROW-OUTPUT-ARGUMENT-NAME(", p4_narrow_output_argument_name),
    P4_FXco ("NARROW-INPUT-ARGUMENT-TYPE(",  p4_narrow_input_argument_type),
    P4_FXco ("NARROW-OUTPUT-ARGUMENT-TYPE(", p4_narrow_output_argument_type),
    P4_FXco ("CANONIC-INPUT-TYPE(",  p4_canonic_input_type),
    P4_FXco ("CANONIC-OUTPUT-TYPE(", p4_canonic_output_type),
    P4_FXco ("REWRITER-TEST(", p4_rewriter_test),
    P4_FXco ("REWRITER-INPUT-ARG(", p4_rewriter_input_arg),

    P4_FXco ("REWRITE-LINE(", p4_rewrite_line),
    P4_FXco ("REWRITE-SHOW.", p4_rewrite_show),
    P4_FXco ("REWRITE-STACK-TEST(", p4_rewrite_stack_test),
    P4_FXco ("REWRITE-INPUT-ARG(", p4_rewrite_input_arg),
    P4_FXco ("REWRITE-STACK-RESULT(", p4_rewrite_stack_result),

    P4_FXco ("NARROW-INPUT-NOTATION(",  p4_narrow_input_notation),
    P4_FXco ("NARROW-OUTPUT-NOTATION(", p4_narrow_output_notation),
    P4_FXco ("REWRITE-STACKDEF-TEST(", p4_rewrite_stackdef_test),
    P4_FXco ("REWRITE-STACKDEF-RESULT(",p4_rewrite_stackdef_result),
    P4_FXco ("REWRITE-CHANGER-SELECT(", p4_rewrite_changer_select),
    P4_FXco ("REWRITE-CHANGER-EXPAND(", p4_rewrite_changer_expand),
    P4_FXco ("REWRITE-CHANGER-RESULT(", p4_rewrite_changer_result),

    P4_FXco ("REWRITE-TEST(",   p4_rewrite_test),
    P4_FXco ("REWRITE-SELECT(", p4_rewrite_select),
    P4_FXco ("REWRITE-EXPAND(", p4_rewrite_expand),
    P4_FXco ("REWRITE-RESULT(", p4_rewrite_result),

    P4_INTO ("ENVIRONMENT", 0 ),
    /* enviroment hints (testing for -EXT will make the wordset present) */
    P4_OCON ("STACKHELP-EXT",	2003 ),
    P4_XXco ("STACKHELP-LOADED",   stackhelp_init),

    P4_LOAD ("", core_check),
    P4_LOAD ("", block_check),
    P4_LOAD ("", cdecl_check),
    P4_LOAD ("", chain_check),
    P4_LOAD ("", chainlist_check),
    P4_LOAD ("", file_check),
    P4_LOAD ("", memory_check),
    P4_LOAD ("", string_check),
    P4_LOAD ("", stackhelp_check),
    P4_LOAD ("", exception_check),
    P4_LOAD ("", facility_check),
    P4_LOAD ("", misc_check),
    P4_LOAD ("", debug_check),
    P4_LOAD ("", forth_83_check),

    P4_STKx (" [EXIT]", p4_stackhelp_when_exit),
    P4_STKx (" [DONE]", p4_stackhelp_when_done),
};
P4_COUNTWORDSET (stackhelp, "StackHelp TypeChecking extension");

/*@}*/

/*
 * Local variables:
 * c-file-style: "stroustrup"
 * End:
 */
