#ifndef __PFE_DEF_COMP_H
#define __PFE_DEF_COMP_H "%full_filespec: def-comp.h~bln_mpt1!33.40:incl:bln_12xx!1 %"

/* 
 * -- internal structure definitions 
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2003. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE             @(#) %derived_by: guidod %
 *  @version %version: bln_mpt1!33.40 %
 *    (%date_modified: Wed Mar 19 15:19:34 2003 %)
 *
 *  @description
 * 	lists of declarations reflecting internal structures
 *	of pfe mainly for decompiler (SEE) and debugger.
 */
/*@{*/

#include <pfe/pfe-sub.h>
#include <pfe/def-types.h>

#ifndef P4STRING
# if defined PFE_OLD_STRINGIZE
# define P4STR(A) "A"
# define P4CAT(A,B) A/**/B
# define P4CAT3(A,B,C) A/**/B/**/C
# define P4CAT4(A,B,C,D) A/**/B/**/C/**/D
# else
# define P4STR(A) #A
# define P4CAT(A,B) A##B
# define P4CAT3(A,B,C) A##B##C
# define P4CAT4(A,B,C,D) A##B##C##D
# endif
#define P4STRING(A) P4STR(A)
#endif

#ifdef PFE_SUFFIX
#define P4SUFFIX_(A,B,C) P4CAT3(A,B,C)
#define P4SUFFIX(A,B) P4SUFFIX_(A,B,PFE_SUFFIX)
#else
#define P4SUFFIX(A,B) P4CAT(A,B)
#endif

#define P4CODE(X) P4CAT(X,_)

/* only works in 0.31.x style !! */
#if defined PFE_ENABLE_STACKTHREADED
# define P4_VOID p4_Thread* p4TH
# define P4_CALL p4_Thread* p4TH,
# define FX_VOID p4TH
# define FX_CALL p4TH,
#else
# define P4_VOID void
# define P4_CALL
# define FX_VOID
# define FX_CALL
#endif

/* 0.31.x style */
# define FXCode(X)    P4CODE(X) (P4_VOID)
# define P4_FCODE(X)  P4CODE(X) (P4_VOID)
# define FX_FCODE(X) (P4CODE(X) (FX_VOID))

/* 0.30.x style */
# define P4_CODE(X)  void P4CODE(X) (P4_VOID) /* delcare a primitive */
# define FX_EXEC(X)  (P4CODE(X) (FX_VOID))

/* heritage: declare a prim-function */
# define FCode(X)  void P4CODE(X) (P4_VOID) 

#if defined _P4_SOURCE || defined __PFE_PFE_H
# define PFX(X) P4CODE(X)
# define FX(X)  (P4CODE(X) (FX_VOID))
# define FX_(X) void P4CODE(X) (P4_VOID)
#endif

/* sbr style */

#if   1 || !defined PFE_SBR_CALL_ARG_THREADING
#define FXCode_RT(X)      P4CODE(X) (P4_VOID)
#define  FCode_RT(X) void P4CODE(X) (P4_VOID)
#define FXCode_XE(X)      P4CODE(X) (P4_VOID)
#define  FCode_XE(X) void P4CODE(X) (P4_VOID)
typedef p4code p4code_XE;
typedef p4code p4code_RT;
#else
#define FXCode_RT(X)      P4CODE(X) (P4_USE_BODY_ADDR)
#define  FCode_RT(X) void P4CODE(X) (P4_USE_BODY_ADDR)
#define FXCode_XE(X)      P4CODE(X) (P4_USE_CODE_ADDR)
#define  FCode_XE(X) void P4CODE(X) (P4_USE_CODE_ADDR)
typedef void (*p4code_XE) (P4_ARG_TAKE_CODE_T);
typedef void (*p4code_RT) (P4_ARG_TAKE_BODY_T);
#endif

/* --------------------- Destroyer support ------------------ */

/* P4xONxDESTROY in FFA field */
/* p4_destroyer_RT somewhere else :eek: */
/* btw, watch out not to jump on p4_name_from and p4_obsoleted_RT */
# define PFE_IS_DESTROYER(__nfa) \
   ( (P4xONxDESTROY   &&  P4_NFA_FLAGS(__nfa) & P4xONxDESTROY) \
    || (P4_NFA_FLAGS(__nfa) & P4xIMMEDIATE \
        && P4_XT_VALUE(P4_LINK_FROM(p4_name_to_link(__nfa))) \
	   == FX_GET_RT(p4_destroyer)))

/* ---------------------- Threading support -------------------- */
typedef struct p4_Runtime2 p4_Runtime2; /* and also for the CFA themselves */

/* bitmasks for P4RUNTIMES flags - use 0 for defaults of this flags entry */
#define P4_ONLY_CODE1 1            /* runtime1 fetches no word-pointer */
#define P4_ONLY_CODE2 2            /* runtime2 fetches no word-pointer */
#define P4_RTCODE1_PRIMITIVE 1     /* just an alias under the traditional */
#define P4_RTCODE2_PRIMITIVE 2     /* name - a primitive has no BODY */

/* ---------------------- Decomp support -------------------- */

typedef struct p4_Decomp p4_Decomp; /* informations for the decompiler */
typedef struct p4_Semant p4_Semant; /* pointer set for state smart words */
typedef struct p4_Seman2 p4_Seman2; /* dito for even smarter words like TO */

#define P4_CODE_SEE(func) p4xcode* func (p4xcode* ip, char* p, p4_Semant* s)
#define P4_CODE_RUN(func) p4xcode* func (char* p, p4xt xt, p4char* nfa)

/* encodings for what information follows the compiled word inline */
#define  P4_SKIPS_NOTHING            ((P4_CODE_SEE((*)))(0))
#define  P4_SKIPS_OFFSET             ((P4_CODE_SEE((*)))(1))
#define  P4_SKIPS_CELL               ((P4_CODE_SEE((*)))(2))
#define  P4_SKIPS_DCELL              ((P4_CODE_SEE((*)))(3))
#define  P4_SKIPS_STRING             ((P4_CODE_SEE((*)))(5))
#define  P4_SKIPS_2STRINGS           ((P4_CODE_SEE((*)))(6))
#define  P4_SKIPS_TO_TOKEN           ((P4_CODE_SEE((*)))(7))

/* .... P4_SKIPS_DCELL */
_extern P4_CODE_SEE(p4_lit_dcell_SEE);
/* .... P4_SKIPS_STRING */
_extern P4_CODE_SEE(p4_lit_string_SEE);
/* .... P4_SKIPS_2STRINGS */
_extern P4_CODE_SEE(p4_lit_2strings_SEE);
/* .... P4_SKIPS_TO_TOKEN */
_extern P4_CODE_SEE(p4_lit_to_token_SEE);

struct p4_Decomp		/* informations for the decompiler */
{                               /* (skips is now basically enum'd, see above)*/
    P4_CODE_SEE((*skips));      /* to decompile the data following xt */
    unsigned space:3;		/* additional spaces past the word */
    unsigned cr_bef:2;		/* carriage return before printing */
    signed ind_bef:4;		/* changed indentation before print */
    unsigned cr_aft:2;		/* carriage return after print */
    signed ind_aft:4;		/* changed indentation after print */
    unsigned unused:3;
};

struct p4_Semant		/* for words with different compilation */
{				/* and execution semantics: */
    long magic;			/* mark begin of structure */
    p4_Decomp decomp;		/* decompiler aid */
    p4_namebuf_t const *name;	/* compiled by */
    p4code comp;		/* compilation/interpretation semantics */
    p4code exec[1];		/* execution semantics */
};

struct p4_Seman2		/* for words with different compilation */
{				/* and two different execution semantics: */
    long magic;			/* mark begin of structure */
    p4_Decomp decomp;		/* decompiler aid */
    p4_namebuf_t const *name;	/* compiled by */
    p4code comp;		/* compilation/interpretation semantics */
    p4code_XE exec[2];		/* two different execution semantics */
};				/* for cases like TO (value/local variable) */

struct p4_Runtime2              /* describes characteristics of CFA code */
{
    const p4char* type;         /* in place of p4_Word.name */
    long magic;                 /* mark begin of structure */
    p4cell flag;                /* the call-threading flags for the exec[]s */
    char const *name;           /* the header name for it */
    p4code    comp;             /* the word that will CREATE new headers */
    p4code_RT exec[2];          /* and the values contained in created CFAs */
    struct {
	P4_CODE_RUN((*see));    /* the decompiler routine */
	P4_CODE_RUN((*forget)); /* while running forget destroyers */
	P4_CODE_RUN((*atexit)); /* while running atexit destroyers */
    } run;                      /* we did not make an extra typedef for it */
};

#define P4SEMANTICS(X) P4SUFFIX(X,_Semant)
#define P4RUNTIME_(X) P4SUFFIX(X,_Runtime)

#ifndef HOST_WIN32
#define P4COMPILES(C,E,S,STYLE)			\
p4_Semant P4SEMANTICS(C) =			\
{						\
  P4_SEMANT_MAGIC,				\
  { S, STYLE },					\
  NULL,						\
  P4CODE (C),					\
  { P4CODE (E) }				\
}

#define P4COMPILES2(C,E1,E2,S,STYLE)		\
p4_Seman2 P4SEMANTICS(C) =			\
{						\
  P4_SEMANT_MAGIC,				\
  { S, STYLE },					\
  NULL,						\
  P4CODE (C),					\
  { P4CODE (E1), P4CODE (E2) }			\
}

#define P4RUNTIMES1_(C,E1,FLAGS,SEE)            \
p4_Runtime2 P4RUNTIME_(C) =                     \
{ (const p4char*) "@",                          \
  P4_RUNTIME_MAGIC, FLAGS, 0,                   \
  P4CODE(C), { P4CODE(E1), NULL },              \
  { SEE, NULL, NULL }                           \
}

#define P4RUNTIMES2_(C,E1,E2,FLAGS,SEE)         \
p4_Runtime2 P4RUNTIME_(C) =                     \
{ (const p4char*) "@",                          \
  P4_RUNTIME_MAGIC, FLAGS, 0,                   \
  P4CODE(C), { P4CODE(E1), P4CODE(E2) },        \
  { SEE, NULL, NULL },                          \
}

#else /* HOST_WIN32 */

#define P4COMPILES(C,E,S,STYLE)			\
p4_Semant* P4SEMANTICS(C) (void)		\
{ static p4_Semant semant =			\
 {						\
  P4_SEMANT_MAGIC,				\
  { S, STYLE },					\
  NULL,						\
  P4CODE (C),					\
  { P4CODE (E) }				\
 }; return &semant;                             \
}

#define P4COMPILES2(C,E1,E2,S,STYLE)		\
p4_Seman2* P4SEMANTICS(C) (void)		\
{ static p4_Seman2 semant =			\
 {						\
  P4_SEMANT_MAGIC,				\
  { S, STYLE },					\
  NULL,						\
  P4CODE (C),					\
  { P4CODE (E1), P4CODE (E2) }			\
 }; return &semant;                             \
}

#define P4RUNTIMES1_(C,E,FLAGS,SEE)             \
p4_Runtime2* P4RUNTIME_(C) (void)               \
{ static p4_Runtime2 runtime =                  \
 { (const p4char*) "@",                         \
  P4_RUNTIME_MAGIC, FLAGS, 0,                   \
  P4CODE(C), { P4CODE(E), NULL },               \
  { SEE, NULL, NULL }                           \
 }; return &runtime;                            \
}

#define P4RUNTIMES2_(C,E1,E2,FLAGS,SEE)         \
p4_Runtime2* P4RUNTIME_(C) (void)               \
{ static p4_Runtime2 runtime =                  \
 { (const p4char*) "@",                         \
  P4_RUNTIME_MAGIC, FLAGS, 0,                   \
  P4CODE(C), { P4CODE(E1), P4CODE(E2) },        \
  { SEE, NULL, NULL }                           \
 }; return &runtime;                            \
}

#endif /* HOST_WIN32 */

#define P4RUNTIMES1(C,E,FLAGS) P4RUNTIMES1_(C,E,FLAGS,0)
#define P4RUNTIMES2(C,E1,E2,FLAGS) P4RUNTIMES1_(C,E1,E2,FLAGS,0)

/* compile execution semantics from within C-code: */
#ifndef HOST_WIN32
#define FX_DEF_COMPILES(X) p4_Semant  P4SEMANTICS(X)
#define FX_DEF_COMPILE1(X) p4_Seman2  P4SEMANTICS(X)
#define FX_DEF_COMPILE2(X) p4_Seman2  P4SEMANTICS(X)
#define FX_DEF_RUNTIME1(X) p4_Runtime2 P4RUNTIME_(X)
#define FX_DEF_RUNTIME2(X) p4_Runtime2 P4RUNTIME_(X)
# ifndef PFE_CALL_THREADING
# define FX_GET_COMPILE1(X) (&P4SEMANTICS(X).exec[0])
# define FX_GET_COMPILE2(X) (&P4SEMANTICS(X).exec[1])
# define FX_GET_RUNTIME1(X) (P4RUNTIME_(X).exec[0])
# define FX_GET_RUNTIME2(X) (P4RUNTIME_(X).exec[1])
# else
# define FX_GET_COMPILE1(X) (P4SEMANTICS(X).exec[0])
# define FX_GET_COMPILE2(X) (P4SEMANTICS(X).exec[1])
# define FX_GET_RUNTIME1(X) (&P4RUNTIME_(X))
# define FX_GET_RUNTIME2(X) (&P4RUNTIME_(X))
# endif
#else /* HOST_WIN32: */
#define FX_DEF_COMPILES(X) p4_Semant*  P4SEMANTICS(X) (void)
#define FX_DEF_COMPILE1(X) p4_Seman2*  P4SEMANTICS(X) (void)
#define FX_DEF_COMPILE2(X) p4_Seman2*  P4SEMANTICS(X) (void)
#define FX_DEF_RUNTIME1(X) p4_Runtime2* P4RUNTIME_(X) (void)
#define FX_DEF_RUNTIME2(X) p4_Runtime2* P4RUNTIME_(X) (void)
# ifndef PFE_CALL_THREADING
# define FX_GET_COMPILE1(X) (&P4SEMANTICS(X)()->exec[0])
# define FX_GET_COMPILE2(X) (&P4SEMANTICS(X)()->exec[1])
# define FX_GET_RUNTIME1(X) (P4RUNTIME_(X)()->exec[0])
# define FX_GET_RUNTIME2(X) (P4RUNTIME_(X)()->exec[1])
# else
# define FX_GET_COMPILE1(X) (P4SEMANTICS(X)()->exec[0])
# define FX_GET_COMPILE2(X) (P4SEMANTICS(X)()->exec[1])
# define FX_GET_RUNTIME1(X) (P4RUNTIME_(X)())
# define FX_GET_RUNTIME2(X) (P4RUNTIME_(X)())
# endif
#endif /* HOST_WIN32 */

#ifndef PFE_SBR_CALL_THREADING
#define FX_COMPILE_COMMA_XE(X) FX_ZCOMMA(X)
#define FX_COMPILE_COMMA_RT(X) FX_ZCOMMA(X)
#else
#define FX_COMPILE_COMMA_XE(X) \
             p4_DP = (p4char*) p4_compile_xcode_CODE((p4xcode*)p4_DP,X)
#define FX_COMPILE_COMMA_RT(X,B) \
             p4_DP = (p4char*) p4_compile_xcode_BODY((p4xcode*)p4_DP,X,B)
#endif

#define FX_COMPILE(X)  do { extern     FX_DEF_COMPILES(X);  \
                  FX_COMPILE_COMMA_XE (FX_GET_COMPILE1(X)); } while(0)
#define FX_COMPILE1(X) do { extern     FX_DEF_COMPILE1(X);  \
                  FX_COMPILE_COMMA_XE (FX_GET_COMPILE1(X)); } while(0)
#define FX_COMPILE2(X) do { extern     FX_DEF_COMPILE2(X);  \
                  FX_COMPILE_COMMA_XE (FX_GET_COMPILE2(X)); } while(0)
#define FX_RUNTIME1(X) do { extern     FX_DEF_RUNTIME1(X);  \
                            FX_RCOMMA (FX_GET_RUNTIME1(X)); } while(0)
#define FX_RUNTIME2(X) do { extern     FX_DEF_RUNTIME2(X);  \
                            FX_RCOMMA (FX_GET_RUNTIME2(X)); } while(0)

/* _RT's are optimizations assuming "_RT" suffix for the runtime of words */
#if   ! defined PFE_CALL_THREADING
#define P4_XT_VALUE(__xt) (*(__xt))
#define FX_GET_RT(__rt)   __rt##_RT_ 
#define extern_FX_DEF_RUNTIME1(X) /* nothing */
#define FX_RUNTIME1_RT(X) FX_XCOMMA(X##_RT_) /* a simply comma */
#define P4RUNTIME1_RT(X) /* nothing */
#define P4RUNTIMES1_RT(X,F) /* nothing */
#else
#define P4_XT_VALUE(__xt) ((__xt)->call)
#define FX_GET_RT(__rt)   FX_GET_RUNTIME1(__rt)
#define extern_FX_DEF_RUNTIME1(X) extern FX_DEF_RUNTIME1(X) /* see following */
#define FX_RUNTIME1_RT(X) FX_RUNTIME1(X) /* compiling pointer to def-runtime */
#define P4RUNTIME1_RT(X) static FCode(X) { /* dummy */ } P4RUNTIME1(X, X##_RT)
#define P4RUNTIMES1_RT(X,F) static FCode(X) { /* dummy */ } \
                                                     P4RUNTIMES1(X, X##_RT, F)
#endif

#define FX_IMMEDIATE          P4_NFA_FLAGS(p4_LAST) |= P4xIMMEDIATE
#define FX_SMUDGED            P4_NFA_FLAGS(p4_LAST) |= P4xSMUDGED
#define FX_RUNTIME_BODY       P4_NFA_FLAGS(p4_LAST) |= P4xISxRUNTIME

#define P4_NFA_xIMMEDIATE(X)    (P4_NFA_FLAGS(X) & P4xIMMEDIATE)
#define P4_NFA_xSMUDGED(X)      (P4_NFA_FLAGS(X) & P4xSMUDGED)
#define P4_NFA_xISxRUNTIME(X)   (P4_NFA_FLAGS(X) & P4xISxRUNTIME)
#define P4_NFA_x0x80(X)         (P4_NFA_FLAGS(X) & 0x80)

#if   ! defined PFE_CALL_THREADING
#define FX_POP_BODY_ADDR           (P4_WP_PFA)
#define FX_POP_BODY_ADDR_(VARNAME) register p4cell* VARNAME = FX_POP_BODY_ADDR
#define FX_POP_BODY_ADDR_p4_BODY   register p4cell* p4_BODY = FX_POP_BODY_ADDR
#define FX_POP_BODY_ADDR_UNUSED
#elif ! defined PFE_SBR_CALL_THREADING
#define FX_POP_BODY_ADDR           ((p4cell*)(*p4IP++))
#define FX_POP_BODY_ADDR_(VARNAME) register p4cell* VARNAME = FX_POP_BODY_ADDR
#define FX_POP_BODY_ADDR_p4_BODY   register p4cell* p4_BODY = FX_POP_BODY_ADDR
#define FX_POP_BODY_ADDR_UNUSED    (p4IP++)
#elif ! defined PFE_SBR_CALL_ARG_THREADING
#define FX_POP_BODY_ADDR           ((p4cell*)(*p4IP++))
#define FX_POP_BODY_ADDR_(VARNAME) register p4cell* VARNAME = FX_POP_BODY_ADDR
#define FX_POP_BODY_ADDR_p4_BODY   register p4cell* p4_BODY = FX_POP_BODY_ADDR
#define FX_POP_BODY_ADDR_UNUSED    (++p4IP)
#else
#define FX_POP_BODY_ADDR           (p4_BODY)
#define FX_POP_BODY_ADDR_(VARNAME) register p4cell* VARNAME = FX_POP_BODY_ADDR
#define FX_POP_BODY_ADDR_p4_BODY   /* already declared */
#define FX_POP_BODY_ADDR_UNUSED    /* old trick: */ p4_BODY=p4_BODY
#endif

#if   ! defined PFE_CALL_THREADING
#define FX_COMPILE_COMMA(xt) FX_XCOMMA(xt)
#else
#define FX_COMPILE_COMMA(xt) \
        p4_DP = (p4char*) p4_compile_comma((p4xcode*)p4_DP, xt)
#endif

#ifndef PFE_CALL_THREADING
#define FX_XCODE_CALL(__x) p4_call((__x))
#else
#define FX_XCODE_CALL(__x) { \
		p4Word __w = { "", (__x) }; \
		p4Word* __p == & __w; \
		p4_call((p4xt)(&__p)); }
#endif


/* NONAME is currently empty, but could be a real call later, please use it.
 * the _RUNTIME_HEADER is obsolete, and will be removed *very* soon, just as
 * the _RUNTIME_BODY word - the flag shall be part of the runtime-defblock
 * that will be used in call-threaded and sbr-call-threaded mode
 */
#define FX_NONAME
#define FX_HEADER_(W)       p4_header_in(W) 
#define FX_HEADER           FX_HEADER_(p4_CURRENT)
#define FX_RUNTIME_HEADER   FX_HEADER ; FX_RUNTIME_BODY
/* the P4RUNTIME1 define is obsolete - the default is call-thread WITH BODY */
#define P4RUNTIME1(C,E1)    P4RUNTIMES1(C,E1,0)
#define P4RUNTIME2(C,E1,E2) P4RUNTIMES2(C,E1,E2,0)

/* ------------------------------------------------------------------ */
/* debug-ext: we walk registered wordsets (in atexit_wl) looking for
 * items that can be used for decompiling. No need to register each
 * item in the forth dictionary anymore. Here we define a walker struct
 * that can be referenced in decompiler-routines in the wordsets around
 */

typedef struct p4_Decompile p4_Decompile;
struct p4_Decompile
{
    p4char* next;            /* initially set to PFE.atexit_wl->thread[0] */
    char const* wordset;     /* current wordset reference, the name info */ 
    int left;                /* how many left to check: */
    struct {                 /* compatible with p4Words !! */
	struct{
	    const char type;    /* loader-type */
	    const char lencode; /* forth-name compatibility */
	    const char name[1]; /* zero-terminated... */
	} *loader;
	union {                 /* the value part from the wordset item: */
	    const p4cell cell;             /* as type p4cell */
	    void * ptr;                    /* generic pointer */
	    p4_Runtime2 const * runtime;   /* runtime reference */
	    p4_Seman2   const * semant;    /* semant reference */
	    char* name;                    /* name, zero-terminated */
	} value;
    } *word;
};

/* ------------------------------------------------------------------ */

p4_Semant* p4_to_semant(p4xt xt);
char p4_category (p4code p);
void p4_decompile (p4_char_t* nfa, p4xt xt);

FCode (p4_noop);
FCode (p4_ahead);

FCode (p4_backward_mark);     /* FORTH-83 style system extension words */
FCode (p4_backward_resolve);
FCode (p4_forward_mark);
FCode (p4_forward_resolve);
FCode (p4_bracket_compile);

FCode (p4_smudge);
FCode (p4_unsmudge);

/* Local variables: */

int p4_find_local (const p4_char_t *nm, int l);
int p4_word_compile_local (void);

/* Runtimes to identify words by them: */

FCode_XE (p4_to_execution);
FCode_XE (p4_plus_to_execution);
FCode_XE (p4_locals_bar_execution);
FCode_XE (p4_locals_exit_execution);
FCode_XE (p4_local_execution);
FCode_XE (p4_to_local_execution);
FCode_XE (p4_plus_to_local_execution);

FCode_XE (p4_semicolon_execution);
FCode_XE (p4_literal_execution);
FCode_XE (p4_two_literal_execution);
FCode_XE (p4_f_literal_execution); 

/* some can be subject to the FX_GET_RT optimization (over FX_GET_RUNTIME1) */
FCode_RT (p4_builds_RT);	extern FX_DEF_RUNTIME1(p4_builds);
FCode_RT (p4_variable_RT);	extern FX_DEF_RUNTIME1(p4_variable);
FCode_RT (p4_constant_RT);	extern FX_DEF_RUNTIME1(p4_constant);
FCode_RT (p4_value_RT);		extern FX_DEF_RUNTIME1(p4_value);
FCode_RT (p4_two_constant_RT);  extern FX_DEF_RUNTIME1(p4_two_constant);
FCode_RT (p4_f_constant_RT);    extern FX_DEF_RUNTIME1(p4_f_constant);
FCode_RT (p4_f_variable_RT);    extern FX_DEF_RUNTIME1(p4_f_variable);

FCode_RT (p4_colon_RT);		extern FX_DEF_RUNTIME1(p4_colon);
FCode_RT (p4_debug_colon_RT);	extern FX_DEF_RUNTIME1(p4_debug_colon);
FCode_RT (p4_dictvar_RT);	extern_FX_DEF_RUNTIME1(p4_dictvar);
FCode_RT (p4_dictget_RT);	extern_FX_DEF_RUNTIME1(p4_dictget);
FCode_RT (p4_destroyer_RT);     extern_FX_DEF_RUNTIME1(p4_destroyer);

FCode    (p4_only_RT);		extern_FX_DEF_RUNTIME1(p4_only);
FCode    (p4_forth_RT);		extern_FX_DEF_RUNTIME1(p4_forth);
FCode_RT (p4_vocabulary_RT);	extern FX_DEF_RUNTIME1(p4_vocabulary);
FCode_RT (p4_does_RT);		extern FX_DEF_RUNTIME1(p4_does);
FCode_RT (p4_debug_does_RT);	extern FX_DEF_RUNTIME1(p4_debug_does);
FCode_RT (p4_marker_RT);	extern FX_DEF_RUNTIME1(p4_marker);
FCode_RT (p4_defer_RT);		extern FX_DEF_RUNTIME1(p4_defer);
FCode_RT (p4_offset_RT);	extern FX_DEF_RUNTIME1(p4_offset);
FCode_RT (p4_synonym_RT);	extern FX_DEF_RUNTIME1(p4_synonym);
FCode_RT (p4_obsoleted_RT);	extern FX_DEF_RUNTIME1(p4_obsoleted);

#ifdef _DEBUG_BORLAND_ANSI_COMPILERS_ /* USER-CONFIG */
/* adding SEMANT forward declarations... */

/* 
 # the following is needed to port pfe to a windows environment that has
 # a compiler being strictly standard conformant (usually C89 conformant)
 # - well, on windows we have to use a function declaration to get a handle
 # to the Semant-struct and inside each FX_COMPILE we make actually a forward 
 # declaration of the function usually defined just thereafter in P4COMPILES.
 # However the C compiler is allowed to treat all function declarations as if
 # they were global and Borland-CC-5.5 does just that. But a couple of lines
 # later we do actually define the function (with the P4COMPILES macro) that
 # returns our Semant-struct and when the two declarations are not identical,
 # well then the compiler is allowed to call that an error - and effectivly 
 # stop compilation. Many other compilers do not globalize any declarations
 # being enclosed in the {...}-body of a function and these do not have any
 # problem with it and since we usually use a GCC to develop the PFE, there
 # are many places where the FX_COMPILE's forward-declaration does not match
 # with the actual function-definition later on in P4COMPILES. And the next
 # time that I try to run the PFE through a windows compiler - it feels,
 # aahm, uhmm - broken. The most prominent example is the free Borland CC.
 #
 # but we can try to help ourselves a bit in the process to port pfe to such
 # compilers being not that similar to gcc - but that is not that easy. The
 # problem is that there is absolutly no way to make GCC warn if some 
 # function-enclosed forward-declaration does not match with a global 
 # function-definition later on - when the first function is closed
 # the enclosed forward-function-declaration is forgotten and can not be
 # compared with the function-definition later. The solution here does now
 # try to inverse the order of declaration - we use a script to fetch all
 # function-definitions by looking for their P4COMPILE macros in the source
 # files around and create a header file from the list that contains forward 
 # declarations which match the actual function definitions in the sources.
 #
 # Now the next time the pfe is compiled with gcc, just ensure that this
 # header file is #include'd - and when the gcc jumps at the forward
 # declaration inside the FX_COMPILE macro then it is actually a redeclaration
 # of the global forward declaration from our generated header file. Well,
 # if the declaration types are identical then it's just fine and the gcc
 # will just go on compiling but when the two do *not* match then the GCC will
 # warn us about that. And that's all we want - it will not warn in the place
 # of the P4COMPILES contained function-definition but at each FX_COMPILE that
 # does not match - including file-name and line-number. And then go ahead
 # and swap FX_COMPILE <-> FX_COMPILE1 to match the P4COMPILES later on. When
 # there are no more gcc-warnings then the pfe sources are ready again to be
 # piped through other compilers too that would error out on the differences.
 #
 # 'nuff said, here's the helper (unix) script to build the header file.

 grep '^ *P4COMPILES' *.c                      >def-comp.c-1
 sed -e 's,^.*:,*&/ extern ,'                  >def-comp.c-2 <def-comp.c-1
 sed -e 's,^.*:,/&*,'                          >def-comp.c-3 <def-comp.c-2
 sed -e 's:,.*:);:'                            >def-comp.c-4 <def-comp.c-3
 sed -e 's,P4COMPILES2,p4_Seman2 P4SEMANTICS,' >def-comp.c-5 <def-comp.c-4
 sed -e 's,P4COMPILES,p4_Semant P4SEMANTICS,'  >def-comp.c-6 <def-comp.c-5
 sed -e 's, extern .*(.*(, // &,'              >def-comp.c-7 <def-comp.c-6
 cp def-comp.c-7 def-comp-c89.h 
 rm def-comp.c-?

*/
#include <pfe/def-comp-c89.h>

#endif /* __borland_ansi_compilers__ */

#endif

/*@}*/


