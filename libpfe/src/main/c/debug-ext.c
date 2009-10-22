/**
 * PFE-DEBUG --- analyze compiled code
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.15 $
 *     (modified $Date: 2008-05-11 12:48:04 $)
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
"@(#) $Id: debug-ext.c,v 1.15 2008-05-11 12:48:04 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-limits.h>
#include <pfe/def-types.h>
#include <pfe/def-comp.h>
#include <pfe/term-sub.h>

#include <pfe/os-ctype.h>
#include <pfe/os-string.h>

#include <pfe/_missing.h>

/* ----------------------------------------------------------------------- */

/* LOADER refers to the compiled WORDLIST tables at the end of C modules.
 * Each loader is registered in the extension wordlist so that we simply
 * walk the wordlist looking for those entries. If a loader table entry has
 * been found then we can simply iterate through the element list. Each
 * element has really just a name and a value. For primitives the value is
 * simply the code pointer but other types may refer to an info block.
 */
static const char*
p4_loader_next_wordset (p4_Decompile* decomp)
{
    p4xt xt;
    do {
        if (! decomp->next) return 0;
        xt = p4_name_from (decomp->next);
        decomp->next = *P4_TO_LINK(xt);
    } while (*P4_TO_CODE(xt) != PFX(p4_forget_wordset_RT));
    /* assert xt is wordset_RT */
    /* FIXME: forget-layout? BODY[0] has the value? */
    p4Words* ws = *(p4Words**) P4_TO_BODY(xt);
    decomp->left = ws->n;
    decomp->word = (void*) ws->w;
    decomp->wordset = ws->name;
    return decomp->wordset;
}

static p4_char_t p4_loader_next (p4_Decompile* decomp)
{

    if (! decomp->word) /* after first initializing a decomp-struct */
        goto nothing_left;
    decomp->word ++;
    if (! -- decomp->left)
        goto nothing_left;
 next_loader:
    if (! *(void**) decomp->word->loader)
        goto nothing_left;
    return decomp->word->loader->type;
 nothing_left:
    if (! p4_loader_next_wordset (decomp))
        return '\0';
    if (! decomp->left)
        goto nothing_left;
    goto next_loader;
}

#ifndef PFE_CALL_THREADING
#define p4_code_to_semant p4_to_semant
#else
p4_Seman2 const * p4_code_to_semant (p4xcode code)
{
    auto p4_Decompile decomp;
    _p4_var_zero(decomp); decomp.next = PFE.atexit_wl->thread[0];
    while (p4_loader_next (&decomp))
    {
        if (decomp.word->loader->type != p4_SXCO) continue;
        if (decomp.word->value.semant->exec[0] == code)
            return decomp.word->value.semant;
        if (decomp.word->value.semant->exec[1] == code)
            return decomp.word->value.semant;
    }
    return 0;
}
#endif


/* ----------------------------------------------------------------------- */

/* in contrast to walking the LOADER tables one could also walk the list
 * of execution tokens. In this case we have a simple list of wordlist
 * (i.e. VOCABULARYs) in the list start at VOC_LINK and in each p4_Wordl
 * we go from namebuf to namebuf. Unlike FIND we do not care about the
 * SEARCH-ORDER and we do also return :NONAME entries. It is up to the
 * call what he prefers to do with the execution token.
 */
typedef struct _p4_name_Walk {
    p4_Wordl* wl;
    int thread;
    p4_namebuf_t* name;
} p4_name_Walk;

_extern void
p4_name_walk_init(p4_name_Walk* walk) {
    walk->wl = VOC_LINK;
    walk->thread = 0;
    walk->name = 0;
}

_extern p4_namebuf_t*
p4_name_walk_next(p4_name_Walk* walk) {
    if (walk->wl == NULL) {
        return NULL;
    }
    if (walk->name == NULL) {
        walk->thread = 0;
        walk->name = walk->wl->thread[walk->thread];
    } else {
        walk->name = *p4_name_to_link(walk->name);
    }
    while (walk->name == NULL) {
        walk->thread ++;
        if (walk->thread >= P4_THREADS) {
            walk->wl = walk->wl->prev;
            if (walk->wl == NULL) return NULL;
            walk->thread = 0;
        }
        walk->name = walk->wl->thread[walk->thread];
    }
    return walk->name;
}


/************************************************************************/
/* decompiler                                                           */
/************************************************************************/

#ifdef WRONG_SPRINTF		/* provision for buggy sprintf (SunOS) */
#define SPRFIX(X) p4_strlen(X)
#else
#define SPRFIX(X) X
#endif

#define UDDOTR(UD,W,BUF) p4_outs (p4_str_ud_dot_r (UD, &(BUF)[sizeof (BUF)], W,BASE))
#define DDOTR(D,W,BUF)	p4_outs (p4_str_d_dot_r (D, &(BUF) [sizeof (BUF)], W, BASE))
#define DOT(N,BUF)	p4_outs (p4_str_dot (N, &(BUF) [sizeof (BUF)], BASE))

/* ----------------------------------------------------------------------- */

typedef p4xcode* (*func_SEE) (p4xcode* , char*, p4_Semant*);

_export p4xcode*
p4_locals_bar_SEE (p4xcode* ip, char* p, p4_Semant* s)
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

_export p4xcode*
p4_local_SEE (p4xcode* ip, char* p, p4_Semant* s)
{
    sprintf (p, "<%c> ", 'A' - 1 +  (int) *(p4cell *) ip);
    return ++ip;
}

_export p4xcode*
p4_literal_SEE (p4xcode* ip, char* p, p4_Semant* s)
{
    char buf[80];
    if (s)
    {
        if (s->name && ! p4_memcmp (s->name+1, "LITERAL", 7)) /* 'bit fuzzy... */
            sprintf (p, "0x%lX ", (unsigned long) *(p4ucell*)ip);
        else
            sprintf (p, "( %.*s) 0x%lX ",
              NAMELEN(s->name), NAMEPTR(s->name), (unsigned long) *(p4ucell*)ip);
    }else{
        p4_strcpy (p, p4_str_dot (*(p4ucell *) ip, buf + sizeof buf, BASE));
    }
    return ++ip;
}

_export p4xcode* /* P4_SKIPS_TO_TOKEN */
p4_lit_to_token_SEE (p4xcode* ip, char* p, p4_Semant* s)
{
    register p4xcode xt = ip[-1];
    if (*xt == s->exec[0])
    {
        register p4char* itemnfa;
        xt = *ip++;
#     ifndef PFE_CALL_THREADING
        itemnfa = p4_to_name (xt);
#     else
        itemnfa = p4_to_name (P4_BODY_FROM(*ip)); ip++;
#     endif
        sprintf (p, "%.*s %.*s ",
          NAMELEN(s->name), NAMEPTR(s->name),
          NAMELEN(itemnfa), NAMEPTR(itemnfa));
        { /* make-recognition, from yours.c */
            if (s->decomp.space > 1) ip++;
            if (s->decomp.space > 2) ip++;
        }
        return ip;
    }else{
        sprintf (p, "%.*s <%c> ",
          NAMELEN(s->name), NAMEPTR(s->name),
          'A' - 1 + (int) *(p4cell *) ip);
        { /* make-recognition, from yours.c */
            if (s->decomp.space > 1) ip++;
            if (s->decomp.space > 2) ip++;
        }
        return ++ip;
    }
}

_export p4xcode* /* P4_SKIPS_STRING */
p4_lit_string_SEE (p4xcode* ip, char* p, p4_Semant* s)
{
    sprintf (p, "%.*s %.*s\" ",
      NAMELEN(s->name), NAMEPTR(s->name),
      (int) *(p4char *) ip, (p4char *) ip + 1);
    P4_SKIP_STRING (ip);
    return ip;
}

_export p4xcode* /* P4_SKIPS_2STRINGS */
p4_lit_2strings_SEE (p4xcode* ip, char* p, p4_Semant* s)
{
    p4char *s1 = (p4char *) ip;

    P4_SKIP_STRING (ip);
    sprintf (p, "%.*s %.*s %.*s ",
      NAMELEN(s->name), NAMEPTR(s->name), (int) *s1, s1 + 1,
      (int) *(p4char *) ip, (p4char *) ip + 1);
    P4_SKIP_STRING (ip);
    return ip;
}

_export p4xcode* /* P4_SKIPS_DCELL */
p4_lit_dcell_SEE (p4xcode* ip, char* p, p4_Semant* s)
{
    char buf[80];
    sprintf (p, "%s. ",
      p4_str_d_dot_r (*(p4dcell *) ip, buf + sizeof buf, 0, BASE));
    P4_INC (ip, p4dcell);

    return ip;
}

static P4_CODE_RUN(p4_code_RT_SEE)
{
#  ifdef PFE_SBR_CALL_THREADING
    sprintf(p, ": %.*s ", NAMELEN(nfa), NAMEPTR(nfa));
#  else
    sprintf(p, "CODE %.*s ", NAMELEN(nfa), NAMEPTR(nfa));
#  endif
    p4xcode* ip = (p4xcode*) P4_TO_BODY(xt);
#  ifdef PFE_SBR_DECOMPILE_PROC
    return PFE_SBR_DECOMPILE_PROC(ip);
#  else
    return ip;
#  endif
}

static p4_bool_t
is_sbr_compile_exit(p4xcode** ip)
{
#  ifdef PFE_SBR_COMPILE_EXIT
    p4char code[40];
    p4char* ref = (p4char*) *ip;
    p4char* end = code;
    PFE_SBR_COMPILE_EXIT(end);
    if (end > code && ! memcmp(ref, code, end-code)) {
        *ip = (p4xcode*)(ref + (end-code));
        return P4_TRUE;
    }
#  endif
    return P4_FALSE;
}

static p4_bool_t
is_sbr_compile_proc(p4xcode** ip)
{
#  ifdef PFE_SBR_COMPILE_PROC
    p4char code[40];
    p4char* ref = (p4char*) *ip;
    p4char* end = code;
    PFE_SBR_COMPILE_PROC(end);
    if (end > code && ! memcmp(ref, code, end-code)) {
        *ip = (p4xcode*)(ref + (end-code));
        return P4_TRUE;
    }
#  endif
    return P4_FALSE;
}

static p4_bool_t
is_sbr_give_code(p4xcode** ip)
{
#  ifdef FX_SBR_GIVE_CODE
    p4char code[40];
    p4char* ref = (p4char*) *ip;
    p4char* end = code;
    FX_SBR_GIVE_CODE(end);
    if (end > code && ! memcmp(ref, code, end-code)) {
        *ip = (p4xcode*)(ref + (end-code));
        return P4_TRUE;
    }
#endif
    return P4_FALSE;
}

static p4_bool_t
is_sbr_give_body(p4xcode** ip, const p4_namebuf_t** name)
{
#  ifdef FX_SBR_GIVE_BODY
    p4char code[40];
    auto p4_name_Walk walk;
    p4_name_walk_init(&walk);
    while (p4_name_walk_next(&walk))
    {
        p4xt xt = P4_LINK_FROM(p4_name_to_link(walk.name));
        p4char* ref = (p4char*) *ip;
        p4char* end = code;
        /* does not work relative addressing */
        p4char* arg = (p4char*) P4_TO_BODY(xt);
        FX_SBR_GIVE_BODY(end, arg);
        if (end > code && ! memcmp(ref, code, end-code)) {
            *name = walk.name;
            *ip = (p4xcode*)(ref + (end-code));
            return P4_TRUE;
        }
        end = code;
        arg += (code-ref);
        FX_SBR_GIVE_BODY(end, arg);
        if (end > code && ! memcmp(ref, code, end-code)) {
            *name = walk.name;
            *ip = (p4xcode*)(ref + (end-code));
            return P4_TRUE;
        }
    }
#  endif
    return P4_FALSE;
}

static p4_bool_t
is_sbr_compile_call_to(p4xcode** ip, p4char* arg)
{
#  ifdef PFE_SBR_COMPILE_CALL
#    undef PFE_SBR_COMPILE_CALL_FAILED
#   define PFE_SBR_COMPILE_CALL_FAILED(X) /* we don't wanna know */
    p4char code[40];
    p4char* ref = (p4char*) *ip;
    p4char* end = code;             /* => absolute address */
    PFE_SBR_COMPILE_CALL(end, arg);
    if (end > code && ! memcmp(ref, code, end-code)) {
        *ip = (p4xcode*)(ref + (end-code));
        return P4_TRUE;
    }
#    undef PFE_SBR_LABEL_
#   define PFE_SBR_LABEL_(label) __pfe_sbr_label_relative_##label
    end = code;  arg += (code-ref); /* => relative address */
    PFE_SBR_COMPILE_CALL(end, arg);
    if (end > code && ! memcmp(ref, code, end-code)) {
        *ip = (p4xcode*)(ref + (end-code));
        return P4_TRUE;
    }
#  endif
    return P4_FALSE;
}

static p4_bool_t
is_sbr_compile_call(p4xcode** ip, const p4_namebuf_t** name)
{
#  ifdef PFE_SBR_COMPILE_CALL
    auto p4_name_Walk walk;
    p4_name_walk_init(&walk);
    while (p4_name_walk_next(&walk))
    {
        p4xt xt = P4_LINK_FROM(p4_name_to_link(walk.name));
        if (is_sbr_compile_call_to(ip, (p4char*) P4_TO_BODY(xt)))
        {
            *name = walk.name;
            return P4_TRUE;
        }
    }

    auto p4_Decompile decomp;
    _p4_var_zero(decomp); decomp.next = PFE.atexit_wl->thread[0];
    while (p4_loader_next (&decomp))
    {
        switch(decomp.word->loader->type)
        {
        case p4_SXCO:
            if (is_sbr_compile_call_to (ip, (p4char*) decomp.word->value.semant->exec[0])) {
                *name = decomp.word->value.semant->name;
                return P4_TRUE;
            }
            if (is_sbr_compile_call_to (ip, (p4char*) decomp.word->value.semant->exec[1])) {
                *name = decomp.word->value.semant->name;
                return P4_TRUE;
            }
        case p4_RTCO:
            if (is_sbr_compile_call_to (ip, (p4char*) decomp.word->value.runtime->exec[0])) {
                *name = decomp.word->value.semant->name;
                return P4_TRUE;
            }
            if (is_sbr_compile_call_to (ip, (p4char*) decomp.word->value.runtime->exec[1])) {
                *name = decomp.word->value.semant->name;
                return P4_TRUE;
            }
        case p4_FXCO:
            if (is_sbr_compile_call_to (ip, (p4char*) decomp.word->value.ptr)) {
                *name = (p4_namebuf_t*) (decomp.word->loader->name - 1); /* NAMEPTR>NAME */
                /* TODO: the NAMELEN of a wordset loader name is wrong... but it is the maximum
                 * value for the count field but current users of this function do always watch
                 * for the zero-byte and the end.... and so it does work okay (so far). */
                return P4_TRUE;
            }
        }
    }
#  endif
    return P4_FALSE;
}

static const p4_Decomp default_style = {P4_SKIPS_NOTHING, 0, 0, 0, 0, 0};

static p4xcode *
p4_decompile_comma (p4xcode* ip, char *p)
{
#  if defined PFE_SBR_DECOMPILE_LCOMMA
    p4cell* x = (p4cell*) ip;
    sprintf (p, "$%08x L, ", *x); ++x;
#  elif defined PFE_SBR_DECOMPILE_WCOMMA
    p4word* x = (p4word*) ip;
    sprintf (p, "$%04x W, ", *x); ++x;
#  else /*  def PFE_SBR_DECOMPILE_BCOMMA */
    p4char* x = (p4char*) ip;
    sprintf (p, "$%02x C, ", *x); ++x;
#  endif
    return (p4xcode*) (x);
}

static p4xcode *
p4_decompile_code (p4xcode* ip, char *p, p4_Decomp *d)
{
    const p4_namebuf_t* name;
    if (is_sbr_compile_exit (& ip))
    {
        static const p4_Decomp end_code_style = {P4_SKIPS_NOTHING, 0, 0, 0, 3, 0};
        p4_memcpy (d, (& end_code_style), sizeof (*d));
        sprintf (p, "] ;");
        return ip;
    }
    if (is_sbr_compile_proc (& ip))
    {
        p4_memcpy (d, (& default_style), sizeof (*d));
        sprintf (p, "( -- ) ");
        return ip;
    }
    if (is_sbr_give_code (& ip))
    {
        if (is_sbr_compile_call(& ip, & name)) {
            p4_memcpy (d, (& default_style), sizeof (*d));
            sprintf (p, "] %.*s ", NAMELEN(name), NAMEPTR(name));
        } else {
            p4_memcpy (d, (& default_style), sizeof (*d));
            sprintf (p, "(  ) ");
        }
        return ip;
    }
    if (is_sbr_give_body (& ip, & name))
    {
        p4_memcpy (d, (& default_style), sizeof (*d));
        sprintf (p, "] %.*s ", NAMELEN(name), NAMEPTR(name));
        is_sbr_compile_call (& ip, & name); /* already printed */
        return ip;
    }
    if (is_sbr_compile_call (& ip, & name))
    {
        p4_memcpy (d, (& default_style), sizeof (*d));
        sprintf (p, "] %.*s ", NAMELEN(name), NAMEPTR(name));
        return ip;
    }
    { /* else */
        p4_memcpy (d, (& default_style), sizeof (*d));
        return p4_decompile_comma (ip, p);
    }
    /* return *ip++; */
}

static p4xcode *
p4_decompile_word (p4xcode* ip, char *p, p4_Decomp *d)
{
    /* assert SKIPS_NOTHING == 0 */
    register p4xcode xt = *ip++;
    register p4_Semant *s;

    s = (p4_Semant*) p4_code_to_semant (xt);
    p4_memcpy (d, ((s) ? (& s->decomp) : (& default_style)), sizeof(*d));

    /* some tokens are (still) compiled without a semant-definition */
    if (*xt == PFX (p4_literal_execution))
        return p4_literal_SEE (ip, p, s);
    if (*xt == PFX (p4_locals_bar_execution))
        return p4_locals_bar_SEE (ip, p, s);
    if (*xt == PFX (p4_local_execution))
        return p4_local_SEE (ip, p, s);

    if (d->skips == P4_SKIPS_CELL
      || d->skips == P4_SKIPS_OFFSET)
    {
        P4_INC (ip, p4cell);
        sprintf (p, "%.*s ", NAMELEN(s->name), NAMEPTR(s->name));
        return ip;
    }

    if (d->skips == P4_SKIPS_DCELL)
        return p4_lit_dcell_SEE (ip, p, s);
    if (d->skips == P4_SKIPS_STRING)
        return p4_lit_string_SEE (ip, p, s);
    if (d->skips == P4_SKIPS_2STRINGS)
        return p4_lit_2strings_SEE (ip, p, s);
    if (d->skips == P4_SKIPS_TO_TOKEN)
        return p4_lit_to_token_SEE (ip, p, s);

    /* per default, just call the skips-decomp routine */
    if (d->skips) /* SKIPS_NOTHING would be NULL */
        return (*d->skips)(ip, p, s);

    if (s != NULL)
    {
        /* use the semant-name (or compiled-by name) */
        sprintf (p, "%.*s ", NAMELEN(s->name), NAMEPTR(s->name));
        return ip;
    }else{
        /* the prim-name (or colon-name) */
#      ifndef PFE_CALL_THREADING
        register p4char* nfa = p4_to_name (xt);
        sprintf (p, P4_NAMExIMMEDIATE(nfa) ? "POSTPONE %.*s " : "%.*s ",
                 NAMELEN(nfa), NAMEPTR(nfa));
        return ip;
#      else
        /* actually, we need to find the item. In other words, we need to
         * find the NFA with the given CODE execution - and if it is not a
         * primitive then it must also match the body code. When it is a
         * non-primitive, then the decompiler must also skip the next
         * cell (the body-reference) in the decompiled sequence.
         */
#      ifdef PFE_AVOID_BUILTIN_MEMCPY
        auto p4_Decompile decomp;
        _p4_var_zero(decomp); decomp.next = PFE.atexit_wl->thread[0];
#      else
        auto p4_Decompile decomp = { PFE.atexit_wl->thread[0] };
#      endif
        while (p4_loader_next (&decomp))
        {
            switch (decomp.word->loader->type)
            {
            case p4_FXCO:
            case p4_IXCO:
                if (decomp.word->value.ptr != xt) continue;
                sprintf (p, decomp.word->loader->lencode & P4xIMMEDIATE
                         ? "POSTPONE %s " : "%s ", decomp.word->loader->name);
                return ip;
            case p4_SXCO:
                if (decomp.word->value.semant->exec[0] != xt &&
                    decomp.word->value.semant->exec[1] != xt) continue;
                /* FIXME: fetch the SEE routine and decompile */
                sprintf (p, "PoSTPoNe %s ",        decomp.word->loader->name);
                return ip;
            case p4_RTCO:
                if (decomp.word->value.runtime->exec[0] != xt) continue;
                if (decomp.word->value.runtime->flag & P4_ONLY_CODE1)
                    goto ouch;
                /* we assume the next cell is the pointer to BODY: */
                p4char* nfa = p4_to_name (P4_BODY_FROM(*ip)); ip++;
                sprintf (p, P4_NAMExIMMEDIATE(nfa)
                         ? "POSTPONE %.*s " : "%.*s ",
                         NAMELEN(nfa), NAMEPTR(nfa));
                return ip;
            ouch:
                /* OUCH! There is no body, so the real name is lost */
                sprintf (p, "POSTPONe %s ",        decomp.word->loader->name);
                return ip;
            }
        }
        p4_strcpy (p, "?""?""?"" ");
        return ip;
#      endif
    }
}

_export void
p4_decompile_rest (p4xcode *ip, int nl, int indent, p4_bool_t iscode)
{
#  if defined PFE_SBR_CALL_THREADING
    int incode = P4_FALSE;
#  endif
    char* buf = p4_pocket ();
    /* p4_Seman2 *seman; // unused ? */
    p4_Decomp decomp;
    *buf = '\0';

    FX (p4_start_Q_cr);
    for (;;)
    {
        if (!*ip) break;
        /* seman = (p4_Seman2 *) p4_code_to_semant (*ip); // unused ? */
        if (iscode)
        {
#         if !defined PFE_SBR_CALL_THREADING
            p4xcode* old_ip = ip;
            ip = p4_decompile_code (ip, buf, &decomp);
            if (! strcmp (buf, "] ;") )
            {
                strcpy(buf, "END-CODE ");
            } else if (! strncmp (buf, "] ", 2)) {
                static const p4_Decomp call_style = {P4_SKIPS_NOTHING, 0, 1, 0, 1, 0};
                /* if not STC then show just as comment - each decompiled code
                 * will then be presented on a seperate line - on x86 ITC just try
                 *   CODE uu $90 c, $e8 c, ' DUP @ HERE cell+ - , END-CODE
                 *   SEE uu \ results in ->
                 *   CODE uu      $90 C,
                 *       ( DUP ) $e8 C, $ec C, $ff C, $ff C, $ff C,
                 *       END-CODE
                 *  and the execution of uu will actually perform a DUP ( $90 is NOP )
                 */
                char* append = strchr(buf, '\0');
                *buf = '('; strcpy (append, ") ");
                memcpy (& decomp, & call_style, sizeof(decomp));
                while (old_ip < ip) {
                    append = strchr(append, '\0');
                    old_ip = p4_decompile_comma(old_ip, append);
                }
            }
#         else
            ip = p4_decompile_code (ip, buf, &decomp);
            if (! strncmp (buf, "] ", 2))
            {
                if (incode)
                    incode = P4_FALSE;
                else
                    memmove(buf, buf+2, strlen(buf)-1);
            } else if (buf[0] == '$') {
                if (! incode) {
                    memmove(buf+2, buf, strlen(buf)+1);
                    buf[0] = '['; buf[1] = ' ';
                    incode = P4_TRUE;
                }
            }
#         endif
        } else
        {
            ip = p4_decompile_word (ip, buf, &decomp);
        }
        indent += decomp.ind_bef;
        if ((!nl && decomp.cr_bef) || p4_OUT + p4_strlen (buf) >= (size_t) p4_COLS)
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
        p4_emits (decomp.space, ' ');
        indent += decomp.ind_aft;
        if (decomp.cr_aft)
        {
            if (p4_Q_cr ())
                break;
            nl = 1;
        }
        if (decomp.cr_aft > 2)  /* instead of exec[0] == PFX(semicolon_execution) */
            break;
    }
}

static P4_CODE_RUN(p4_colon_RT_SEE)
{
    p4_strcat (p, ": ");
    p4_strncat (p, (char*) NAMEPTR(nfa), NAMELEN(nfa));
    p4_strcat (p, "\n");
    return (p4xcode*) p4_to_body (xt);
}

static P4_CODE_RUN(p4_does_RT_SEE)
{
    p4_strcat (p, "<BUILDS ");
    p4_strncat (p, (char*) NAMEPTR(nfa), NAMELEN(nfa));
    p4_strcat (p, " ( ALLOT )");
    return (*P4_TO_DOES_CODE(xt))-1;
}

static void print_comment (const char* prefix, const char* wordset)
{
    char* end = p4_strchr (wordset, ' ');
    p4_outs ("  ( ");
    if (! prefix) prefix = "";

    if (! end) {
        p4_outf ("%s%s", prefix, wordset); }
    else {
        p4_outf ("%s%.*s", prefix, (int)(end - wordset), wordset); }
    p4_outs (" Word ) ");
}

_export void
p4_decompile (p4_namebuf_t* nfa, p4xt xt)
{
    register char* buf = p4_pocket ();
    register p4xcode* rest = 0;
    p4_bool_t iscode = P4_FALSE;
    *buf = '\0';

    FX (p4_cr);
    if (     *P4_TO_CODE(xt) == PFX(p4_colon_RT) ||
             *P4_TO_CODE(xt) == PFX(p4_debug_colon_RT))
    { rest = p4_colon_RT_SEE(buf,xt,nfa); goto decompile; }
    else if (*P4_TO_CODE(xt) == PFX(p4_does_RT)||
             *P4_TO_CODE(xt) == PFX(p4_debug_does_RT))
    { rest = p4_does_RT_SEE(buf,xt,nfa); goto decompile; }

#  if !defined PFE_CALL_THREADING
    if (*xt == (p4code) P4_TO_BODY(xt)) {
        iscode = P4_TRUE;
        rest = p4_code_RT_SEE(buf,xt,nfa); goto decompile;
    }
#  else
    switch (*xt->type->def) {
    case 0:       /* code trampolin */
    case p4_NEST: /* sbr-threading colon start */
        iscode = P4_TRUE;
        rest = p4_code_RT_SEE(buf,xt,nfa); goto decompile;
    }
#  endif

    /* new variant: we walk the atexit-list looking for WORDSET
     * registerations. We walk each entry in the wordset looking for
     * RTco items and comparing their values with what we have as CODE(xt).
     * When there is a SEE decompile-routine registered, then we use it.
     */
# if __GNUC__ < 3
    auto p4_Decompile decomp = { PFE.atexit_wl->thread[0] };
# else
    auto p4_Decompile decomp = {}; decomp.next = PFE.atexit_wl->thread[0];
# endif
    while (p4_loader_next (&decomp))
    {
        switch (decomp.word->loader->type)
        {
        case p4_RTCO:
            if (*P4_TO_CODE(xt) != decomp.word->value.runtime->exec[0])
                continue;
            /* we have it! */
            if (decomp.word->value.runtime->run.see)
            {
                rest = decomp.word->value.runtime->run.see (buf,xt,nfa);
                if (rest) goto decompile;
                p4_outs (buf); p4_outs (" ");
            }else
            {
                p4_outf (buf, "%s %.*s ", decomp.word->loader->name,
                         (int) NAMELEN(nfa), NAMEPTR(nfa));
            }
            p4_outs (P4_NAMExIMMEDIATE (nfa) ? " IMMEDIATE " : "        ");
            print_comment ("From ", decomp.wordset);
            return;
        case p4_FXCO:
        case p4_IXCO:
        case p4_XXCO:
            if (*P4_TO_CODE(xt) != (p4code) decomp.word->value.ptr)
                    continue;
            p4_dot_name (nfa);
            p4_outs (P4_NAMExIMMEDIATE (nfa) ? " IMMEDIATE " : "        ");
            print_comment ("A Prim ", decomp.wordset);
            goto primitive;
        case p4_SXCO:
            if (*P4_TO_CODE(xt) != (p4code) decomp.word->value.semant->comp)
                continue;
            p4_dot_name (nfa);
            p4_outs (" ...");
            if (p4_strchr (decomp.word->loader->name, '"'))
                p4_outc ('"');
            p4_outs (P4_NAMExIMMEDIATE (nfa) ? " IMMEDIATE " : "        ");
            print_comment ("A Smart ", decomp.wordset);
            goto primitive;
        default:
            continue;
        } /* switch */
        p4_outs (buf); p4_outs (" ");
        return;
    }; /* nothing found */
/* else: */
    p4_dot_name (nfa);
    if (P4_NAMExIMMEDIATE(nfa))
            p4_outs ("is IMMEDIATE ");
    else
            p4_outs ("is prim CODE ");
    if (P4xISxRUNTIME)
            if (P4_NAMExISxRUNTIME(nfa))
                p4_outs ("RUNTIME ");
 primitive: {
#  ifdef PFE_HAVE_GNU_DLADDR
    extern char* p4_dladdr (void*, int*);
    register char* name = p4_dladdr (*P4_TO_CODE(xt), 0);
    if (name) p4_outs(name); else p4_outc('.');
    p4_outc(' ');
#  endif
    return;
 }
 decompile: {
    /* assert (*buf) */
    p4_outs (buf); p4_outs (" ");
    if (rest)
        p4_decompile_rest (rest , 1, 4, iscode);
    if (P4_NAMExIMMEDIATE (nfa))
        p4_outs (" IMMEDIATE ");
    return;
 }
}

/************************************************************************/
/* debugger                                                             */
/************************************************************************/

_export char
p4_category (p4code p)
{
    if (p == PFX(p4_colon_RT) || p == PFX(p4_debug_colon_RT))
        return ':';
    if (p == PFX(p4_variable_RT) || p == PFX(p4_value_RT) || p == PFX(p4_builds_RT))
        return 'V';
    if (p == PFX(p4_constant_RT) || p == PFX(p4_two_constant_RT))
        return 'C';
    if (p == PFX(p4_vocabulary_RT))
        return 'W';
    if (p == PFX(p4_does_RT) || p == PFX(p4_debug_does_RT))
        return 'D';
    if (p == PFX(p4_marker_RT))
        return 'M';
    if (p == PFX(p4_defer_RT))
        return 'F';
    if (p == PFX(p4_offset_RT))
        return '+';
    /* must be primitive */ return 'p';
}

static void
prompt_col (void)
{
    p4_emits (24 - p4_OUT, ' ');
}

static void
display (p4xcode *ip)
{
    p4_Decomp style;
    char buf[80];
    int indent = PFE.maxlevel * 2;
    int depth = p4_S0 - SP, i;

    prompt_col ();
    for (i = 0; i < depth; i++)
    {
        p4_outf ("%10ld ", (long) SP[i]);
        if (p4_OUT + 11 >= p4_COLS)
            break;
    }
    FX (p4_cr);
    p4_decompile_word (ip, buf, &style);
# ifndef PFE_CALL_THREADING
    p4_outf ("%*s%c %s", indent, "", p4_category (**ip), buf);
# else
    p4_outf ("%*s%c %s", indent, "", ' ',buf);
# endif
}

static void
interaction (p4xcode *ip)
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
#          ifndef PFE_CALL_THREADING /*FIXME*/
             switch (p4_category (**ip))
             {
              default:
                  p4_decompile (p4_to_name (*ip), *ip);
                  break;
              case ':':
                  FX (p4_cr);
                  p4_decompile_rest ((p4xt *) p4_to_body (*ip), 1, 4, P4_FALSE);
                  break;
              case 'd':
                  p4_outs ("\nDOES>");
#               ifndef PFE_CALL_THREADING /*FIXME*/
                  p4_decompile_rest ((p4xt *) (*ip)[-1], 0, 4, P4_FALSE);
#               endif
                  break;
             }
#          endif
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


#  if !defined PFE_CALL_THREADING
#  define  p__do_adjust_level(xt) do_adjust_level(xt);
#  else
#  define  p__do_adjust_level(xt) do_adjust_level (*P4_TO_CODE(xt));
#  endif

static void
do_adjust_level (const p4xcode xt)
{
    if (*xt == PFX(p4_colon_RT) ||
        *xt == PFX(p4_debug_colon_RT) ||
        *xt == PFX(p4_does_RT) ||
        *xt == PFX(p4_debug_does_RT))
        PFE.level++;
    else if (*xt == PFX (p4_semicolon_execution) ||
             *xt == PFX (p4_locals_exit_execution))
        PFE.level--;
}

static void
p4_debug_execute (p4xt xt)
{
    p__do_adjust_level (xt);
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
#  if ! defined PFE_SBR_CALL_THREADING
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
#         if defined PFE_CALL_THREADING
            p4xcode w = *IP++;
            w ();
#         elif defined P4_WP_VIA_IP
            p4xcode w = *IP++;	/* ip is register but W isn't */

            (*w) ();
#         else
            p4WP = *IP++;	/* ip and W are same: register or not */
            (*p4WP) ();
#         endif
        }
    }
#   else
    /* one can not really single-step in sbr-threading mode */
    interaction (0);
#   endif
}

void FXCode (p4_debug_colon_RT)
{
    FX (p4_colon_RT);
    if (!PFE.debugging)
    {
        p4_debug_on ();
        do_single_step ();
        p4_debug_off ();
    }
}
static void FXCode (p4_debug_colon) { /* dummy */ }
P4RUNTIME1(p4_debug_colon, p4_debug_colon_RT);

void FXCode (p4_debug_does_RT)
{
    FX (p4_does_RT);
    if (!PFE.debugging)
    {
        p4_debug_on ();
        do_single_step ();
        p4_debug_off ();
    }
}
static void FXCode (p4_debug_does) { /* dummy */ }
P4RUNTIME1(p4_debug_does, p4_debug_does_RT);

/** DEBUG ( "word" -- ) [FTH]
 * this word will place an debug-runtime into
 * the => CFA of the following word. If the
 * word gets executed later, the user will
 * be prompted and can decide to single-step
 * the given word. The debug-stepper is
 * interactive and should be self-explanatory.
 * (use => NO-DEBUG to turn it off again)
 */
void FXCode (p4_debug)
{
    p4xt xt;

    xt = p4_tick_cfa (FX_VOID);
    if (P4_XT_VALUE(xt) == FX_GET_RT (p4_debug_colon)
      || P4_XT_VALUE(xt) == FX_GET_RT (p4_debug_does))
        return;
    else if (P4_XT_VALUE(xt) == FX_GET_RT (p4_colon))
        P4_XT_VALUE(xt) = FX_GET_RT (p4_debug_colon);
    else if (P4_XT_VALUE(xt) == FX_GET_RT (p4_does))
        P4_XT_VALUE(xt) = FX_GET_RT (p4_debug_does);
    else
        p4_throw (P4_ON_ARG_TYPE);
}

/** NO-DEBUG ( "word" -- ) [FTH]
 * the inverse of " => DEBUG word "
 */
void FXCode (p4_no_debug)
{
    p4xt xt;

    xt = p4_tick_cfa (FX_VOID);
    if (P4_XT_VALUE(xt) == FX_GET_RT (p4_debug_colon))
        P4_XT_VALUE(xt) = FX_GET_RT (p4_colon);
    else if (P4_XT_VALUE(xt) == FX_GET_RT (p4_debug_does))
        P4_XT_VALUE(xt) = FX_GET_RT (p4_does);
    else
        p4_throw (P4_ON_ARG_TYPE);
}

/** (SEE) ( some-xt* -- ) [FTH]
 * decompile the token-sequence - used
 * by => SEE name
 */
void FXCode (p4_paren_see)
{
    p4_decompile (0, (void*)FX_POP);
}

/** ADDR>NAME ( word-addr* -- word-nfa*!' | 0 ) [FTH]
 * search the next corresponding namefield that address
 * is next too. If it is not in the base-dictionary, then
 * just return 0 as not-found.
 */
_export p4_namebuf_t const *
p4_addr_to_name (const p4_byte_t* addr)
{
    Wordl* wl;
    int t;
    p4_namebuf_t const * nfa;
    p4_namebuf_t const * best = 0;

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

/** ADDR>NAME ( word-addr* -- word-nfa!*' | 0 ) [FTH]
 * search the next corresponding namefield that address
 * is next too. If it is not in the base-dictionary, then
 * just return 0 as not-found.
 */
void FXCode (p4_addr_to_name)
{
    *SP = (p4cell) p4_addr_to_name((p4char*)(*SP));
}

/** COME_BACK ( -- ) [FTH]
 * show the return stack before last exception
 * along with the best names as given by => ADDR>NAME
 */
void FXCode (p4_come_back)
{
# ifdef PFE_SBR_CALL_THREADING
    p4_outs ("come_back not implemented in sbr-threaded mode\n");
# else
    p4_namebuf_t const * nfa;
    p4xcode** rp = (p4xcode**) p4_CSP;

    if (PFE.rstack < rp && rp < PFE.r0)
    {
        if (PFE.dict < (p4char*) *rp && (p4char*) *rp < PFE.dp
          && (nfa = p4_addr_to_name ((void*)((*rp)[-1]))))
        {
            p4_outf ("[at] %8p ' %.*s (%+ld) \n", *rp,
                     NAMELEN(nfa), NAMEPTR(nfa),
                     (long)(((p4xt) *rp) - (p4_name_from(nfa))));
        }else{
            p4_outf ("[at] %8p (?""?""?) \n", *rp);
        }

        while (rp < RP)
        {
            nfa = p4_addr_to_name ((void*)(*rp));
            if (nfa)
            {
                p4_outf ("[%02ld] %8p ' %.*s (%+ld) \n",
                  (long)(RP-rp), *rp, NAMELEN(nfa), NAMEPTR(nfa),
                  (long)(((p4xt) *rp) - (p4_name_from(nfa))));
            }else{
                p4_outf ("[%02ld] %8p   %+ld \n",
                  (long)(RP-rp), *rp, (long) *rp);
            }
            rp++;
        }
    }else{
        p4_outs (" come_back csp trashed, sorry \n");
    }
# endif
}

P4_LISTWORDSET (debug) [] =
{
    P4_INTO ("FORTH", 0),
    P4_FXco ("DEBUG",		p4_debug),
    P4_FXco ("NO-DEBUG",	p4_no_debug),
    P4_FXco ("(SEE)",		p4_paren_see),
    P4_FXco ("ADDR>NAME",	p4_addr_to_name),
    P4_FXco ("COME_BACK",	p4_come_back),

    P4_INTO ("ENVIRONMENT", 0),
    P4_DCON ("PFE-DEBUG",	maxlevel),
};
P4_COUNTWORDSET (debug, "Debugger words");

/*@}*/
