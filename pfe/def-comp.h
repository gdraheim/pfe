#ifndef __PFE_DEF_COMP_H
#define __PFE_DEF_COMP_H "%full_filespec: def-comp.h~5.6:incl:bln_12xx!1 %"

/* 
 * -- internal structure definitions 
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE             @(#) %derived_by: guidod %
 *  @version %version: 5.6 %
 *    (%date_modified: Mon Mar 12 10:33:17 2001 %)
 *
 *  @description
 * 	lists of declarations reflecting internal structures
 *	of pfe mainly for decompiler (SEE) and debugger.
 */
/*@{*/

#include <pfe/incl-sub.h>

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

#define P4CODE(X) P4CAT(X,_)

#define P4_CODE(X)  void P4CODE(X) (void) /* delcare a primitive */
#define P4_EXEC(X)  (P4CODE(X) ())

/* heritage: declare a primitive */
# define FCode(X)  void P4CODE(X) (void) 

/* 0.31.x style */
# define P4_VOID void
# define P4_CALL
# define FX_VOID
# define FX_CALL
# define FXCode(X)    P4CODE(X) (P4_VOID)
# define P4_FCODE(X)  P4CODE(X) (P4_VOID)
# define FX_FCODE(X) (P4CODE(X) (FX_VOID))

#ifdef _P4_SOURCE
# define PFX(X) P4CODE(X)
# define FX(X)  (P4CODE(X) ())
# define FX_(X) void P4CODE(X) (void)
#endif

typedef struct p4_Decomp p4_Decomp; /* informations for the decompiler */
typedef struct p4_Semant p4_Semant; /* pointer set for state smart words */
typedef struct p4_Seman2 p4_Seman2; /* dito for even smarter words like TO */

enum				/* encodings for what information */
{				/* follows the compiled word inline */
    P4_SKIPS_NOTHING,
    P4_SKIPS_OFFSET,
    P4_SKIPS_CELL,
    P4_SKIPS_DCELL,
    P4_SKIPS_FLOAT,
    P4_SKIPS_STRING,
    P4_SKIPS_2STRINGS,
    P4_SKIPS_TO_TOKEN,          /* TO and +TO compile CFA or LOCAL-number */
# ifdef PFE_WITH_DSTRINGS_EXT
    P4_SKIPS_PSTRING,
    P4_SKIPS_PSTRING_TICK,
# endif
    P4_SKIPS_USERDEF
};

struct p4_Decomp		/* informations for the decompiler */
{                               /* (skips is now basically enum'd, see above)*/
    unsigned skips:5;		/* this xt skips this many cells of data */
    unsigned space:3;		/* additional spaces past the word */
    unsigned cr_bef:2;		/* carriage return before printing */
    signed ind_bef:4;		/* changed indentation before print */
    unsigned cr_aft:2;		/* carriage return after print */
    signed ind_aft:4;		/* changed indentation after print */
    unsigned unused:12;
};

struct p4_Semant		/* for words with different compilation */
{				/* and execution semantics: */
    long magic;			/* mark begin of structure */
    p4_Decomp decomp;		/* decompiler aid */
    char const *name;		/* compiled by */
    p4code comp;		/* compilation/interpretation semantics */
    p4code exec[1];		/* execution semantics */
};

struct p4_Seman2		/* for words with different compilation */
{				/* and two different execution semantics: */
    long magic;			/* mark begin of structure */
    p4_Decomp decomp;		/* decompiler aid */
    char const *name;		/* compiled by */
    p4code comp;		/* compilation/interpretation semantics */
    p4code exec[2];		/* two different execution semantics */
};				/* for cases like TO (value/local variable) */

#define P4SEMANTICS(X) P4CAT(X,_Semant)

#define P4COMPILES(C,E,S,STYLE)			\
p4_Semant P4SEMANTICS(C) =			\
{						\
  SEMANT_MAGIC,					\
  { S, STYLE },					\
  NULL,						\
  P4CODE (C),					\
  { P4CODE (E) }				\
}

#define P4COMPILES2(C,E1,E2,S,STYLE)		\
p4_Seman2 P4SEMANTICS(C) =			\
{						\
  SEMANT_MAGIC,					\
  { S, STYLE },					\
  NULL,						\
  P4CODE (C),					\
  { P4CODE (E1), P4CODE (E2) }			\
}

/* compile execution semantics from within C-code: */
#define FX_COMPILE(X)	do { extern p4_Semant P4SEMANTICS(X); FX_COMMA (&P4SEMANTICS(X).exec [0]); } while(0)
#define FX_COMPILE1(X)  do { extern p4_Seman2 P4SEMANTICS(X); FX_COMMA (&P4SEMANTICS(X).exec [0]); } while(0)
#define FX_COMPILE2(X)	do { extern p4_Seman2 P4SEMANTICS(X); FX_COMMA (&P4SEMANTICS(X).exec [1]); } while(0)

p4_Semant* p4_to_semant(p4xt xt);
char p4_category (p4code p);
void p4_decompile (char *nfa, p4xt xt);

FCode (p4_noop);
FCode (p4_ahead);

FCode (p4_backward_mark);		/* FORTH-83 style system extension words */
FCode (p4_backward_resolve);
FCode (p4_forward_mark);
FCode (p4_forward_resolve);
FCode (p4_bracket_compile);

FCode (p4_smudge);
FCode (p4_unsmudge);

/* Local variables: */

int p4_find_local (char *nm, int l);
int p4_compile_local (char *name, int len);

/* Runtimes to identify words by them: */

FCode (p4_to_execution);
FCode (p4_plus_to_execution);
FCode (p4_locals_bar_execution);
FCode (p4_locals_exit_execution);
FCode (p4_local_execution);
FCode (p4_to_local_execution);
FCode (p4_plus_to_local_execution);

FCode (p4_semicolon_execution);
FCode (p4_literal_execution);
FCode (p4_two_literal_execution);
FCode (p4_f_literal_execution); 

FCode (p4_create_RT);
FCode (p4_constant_RT);
FCode (p4_value_RT);
FCode (p4_two_constant_RT);
FCode (p4_f_constant_RT);
FCode (p4_f_variable_RT);
FCode (p4_colon_RT);
FCode (p4_debug_colon_RT);
FCode (p4_dictvar_RT);
FCode (p4_dictconst_RT);

FCode (p4_only_RT);
FCode (p4_vocabulary_RT);
FCode (p4_does_defined_RT);
FCode (p4_debug_does_defined_RT);
FCode (p4_marker_RT);
FCode (p4_defer_RT);
FCode (p4_offset_RT);

/*@}*/
#endif
