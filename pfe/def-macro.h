#ifndef __PFE_DEF_MACRO_H
#define __PFE_DEF_MACRO_H "%full_filespec: def-macro.h~5.7:incl:bln_12xx!1 %"

/** @name macros.h --- macro definitions for the portable forth environment
 * 
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see    GNU LGPL
 *  @author Tektronix CTE             @(#) %derived_by: guidod %
 *  @version %version: 5.7 %
 *    (%date_modified: Mon Mar 12 10:33:21 2001 %)
 */
/*@{*/

#include <pfe/def-config.h>
#include <pfe/def-const.h>

#ifndef CHAR_BIT
#include <limits.h>
#endif

/* determines the dimension of any given vector */
#ifndef DIM
#define DIM(X)		((int)(sizeof (X) / sizeof *(X)))
#endif

/* wipe any given vector X */
#ifndef ZERO
#define ZERO(X)		(memset ((X), 0, sizeof (X)))
#endif

/* copy any given vector Y to X */
#ifndef COPY
#define COPY(X,Y)	(memcpy ((X), (Y), sizeof (X)))
#endif

/* return the byte offset of a given component to beginning of structure: */
#ifndef OFFSET_OF
#define OFFSET_OF(T,C)	((char *)&(((T *)0)->C) - (char *)0)
#endif

#ifndef BITSOF
#define BITSOF(X)	((int)(sizeof (X) * CHAR_BIT))
#endif

/* inc/decrement, push/pop of arbitrary types with arbitrary pointers */
#if defined __GNUC__ && !defined __STRICT_ANSI__ && !defined __cplusplus
/* Use non-ANSI extensions avoiding address-of operator: */
#define P4_ADD(P,N)	((char *)(P) += (N))
#define P4_INC(P,T)	(((T *)(P))++)
#define P4_DEC(P,T)	(--((T *)(P)))
#define P4_INCR(P,T,N)	(((T *)(P)) += (N))
#define P4_DECR(P,T,N)	(((T *)(P)) -= (N))
#elif defined MSDOS
#define P4_ADD(P,N)	((char *huge)(P) += (int)(N))
#define P4_INC(P,T)	(((T *huge)(P))++)
#define P4_DEC(P,T)	(--((T *huge)(P)))
#define P4_INCR(P,T,N)	(((T *huge)(P)) += (N))
#define P4_DECR(P,T,N)	(((T *huge)(P)) -= (N))
#else
/* Force (or fool) ANSI-C to do typecast's it normally refuses by */
/* casting pointers, not objects, then reference the casted pointers: */
#define P4_ADD(P,N)	(*(char **)&(P) += (N))
#define P4_INC(P,T)	((*(T **)&(P))++)
#define P4_DEC(P,T)	(--(*(T **)&(P)))
#define P4_INCR(P,T,N)	((*(T **)&(P)) += (N))
#define P4_DECR(P,T,N)	((*(T **)&(P)) -= (N))
#endif

#define P4_POPTO_(T,P,X)  ((X) = *P4_INC (P, T))
#define P4_POP_(T,P)      (*P4_INC(P,T))
#define P4_PUSH_(T,X,P)	  (*P4_DEC (P, T) = (X))
#define P4_POSH_(T,P)     (*P4_DEC (P, T))

#define P4_POPTO(P,X) 	P4_POPTO_(p4cell, P, X)
#define P4_POP(P)	P4_POP_(p4cell, P)
#define P4_PUSH(X,P)	P4_PUSH_(p4cell, X, P)
#define P4_POSH(P)      P4_POSH_(p4cell, P)

#define	P4_FLAG(X)	((X) ? P4_TRUE : P4_FALSE)

#ifdef _P4_SOURCE
/* useful shortcuts */

# define NFA_P(X)	(p4_to_name (X))	/* CFA -> NFA */
# define CFA_P(X)	(X)			/* CFA -> CFA */
# define PFA_P(X)	(&((p4cell*)(X))[1])	/* CFA -> PFA */

# define WP_NFA		(p4_to_name(p4WP))
# define WP_CFA		(p4WP)		 
# define WP_PFA		((p4cell *)&p4WP [1]) 

# define P4CNT(X) ((unsigned)(*(p4char*)(X))) /* avoid hickups with gcc > 2.7.x */
# ifndef PFE_WITH_FFA
#   define _FFA(X)	(X)		 /* NFA -> FFA w/o FFA-byte */
#   define NFACNT(X)	((X)&31) 	 /* NFA -> count of namefield */
#   define NFACNTMAX     31              /* used for buffer-sizes */
# else
#   define _FFA(X)	(&((char*)X)[-1]) /* NFA -> FFA w/ FFA-byte */
#   define NFACNT(X)	(X)
#   define NFACNTMAX	127
# endif

# define CELL_MAX	((p4cell)((p4ucell)-1 >> 1))
# define UCELL_MAX	((p4ucell)-1)
/* #define RP		((p4cell *)p4RP) */
# define RP_PUSH(X)	P4_PUSH(X, p4RP)
# define RP_POP()	P4_POP(p4RP)
#endif

#define FX_RP           ((p4cell*)p4RP)

#if 1
#define FX_PUSH(X)      (*--p4SP = (p4cell)(X))
#define FX_POP		(*p4SP++)
#define FX_DUP		(*p4SP++ = p4SP[0])
#define FX_OVER		(*p4SP++ = p4SP[1])
#define FX_NIP		(*p4SP = *p4SP++)
#else
#define FX_PUSH(X)      (P4_PUSH((p4cell)(X), p4SP))
#define FX_POP		(P4_POP(p4SP))
#define FX_DUP		(P4_PUSH(p4SP[0], p4SP))
#define FX_OVER		(P4_PUSH(p4SP[1], p4SP))
#define FX_NIP		(*p4SP = FX_POP)
#endif
#define P4_ROOM(P,N)    (P4_DECR(P,p4cell,N))
#define P4_DROP(P,N)    (P4_INCR(P,p4cell,N))
#define FX_DROP		(P4_INC(p4SP,p4cell))
#define FX_DROP2	(P4_INCR(p4SP,p4cell,2));
#define FX_DROP3	(P4_INCR(p4SP,p4cell,3));
#define FX_DROP4	(P4_INCR(p4SP,p4cell,4));
#define FX_DROP5	(P4_INCR(p4SP,p4cell,5));
#define FX_DROP6	(P4_INCR(p4SP,p4cell,6));
#define FX_DROP7	(P4_INCR(p4SP,p4cell,7));
#define FX_2DROP	(P4_DROP(p4SP,2))
#define FX_3DROP	(P4_DROP(p4SP,3))
#define FX_4DROP	(P4_DROP(p4SP,4))
#define FX_5DROP	(P4_DROP(p4SP,5))
#define FX_6DROP	(P4_DROP(p4SP,6))
#define FX_7DROP	(P4_DROP(p4SP,7))
#define FX_1ROOM        (P4_ROOM(p4SP,1))
#define FX_2ROOM        (P4_ROOM(p4SP,2))
#define FX_3ROOM        (P4_ROOM(p4SP,3))
#define FX_4ROOM        (P4_ROOM(p4SP,4))
#define FX_5ROOM        (P4_ROOM(p4SP,5))
#define FX_6ROOM        (P4_ROOM(p4SP,6))
#define FX_7ROOM        (P4_ROOM(p4SP,7))

#if 0
#define	FX_BRANCH	P4_ADD (p4IP, *(p4cell *)p4IP)
#else
#define	FX_BRANCH	(p4IP = (p4xt*)*p4IP)
#endif
#define P4_SKIP_STRING(X)  P4_ADD (X, p4_aligned (*(p4char*)X + 1))
#define FX_SKIP_STRING     P4_SKIP_STRING(p4IP);
#define P4_ALIGNED(P)	(((size_t)(P) & (PFE_ALIGNOF_CELL - 1)) == 0)
#define P4_DFALIGNED(P)	(((size_t)(P) & (PFE_ALIGNOF_DFLOAT - 1)) == 0)
#define P4_SFALIGNED(P)	(((size_t)(P) & (PFE_ALIGNOF_SFLOAT - 1)) == 0)

#define FX_HERE         (FX_PUSH(p4_DP))
#define FX_ALLOT(X)     ((p4_DP) += (X))
#define	FX_DEPTH	(PFE.s0 - p4SP)
#define	P4_COMMA(P,X)	(*(p4cell *)(P) = (p4cell)(X), P4_INC ((P), p4cell))
#define P4_FCOMMA(P,F)	(*(double *)(P) = (F), P4_INC ((P), double))
#define	FX_COMMA(X)	P4_COMMA(p4_DP,X)
#define FX_FCOMMA(X)	P4_FCOMMA(p4_DP,X)

#ifdef _P4_SOURCE /* these will go away */
#define	COMMA(X)	P4_COMMA(DP,X)
#define FCOMMA(X)	P4_FCOMMA(DP,X)
#endif

#if defined PFE_WITH_NOSMART
#define p4_STATESMART 1
#define FX_STATESMART_Q_COMP FX(p4_Q_comp)
#define FX_STATESMART_Q_EXEC FX(p4_Q_exec)
#else  
#define p4_STATESMART p4_STATE
#define FX_STATESMART_Q_COMP
#define FX_STATESMART_Q_EXEC
#endif

#ifdef _P4_SOURCE
#define STATESMART p4_STATESMART
#define _FX_STATESMART_Q_COMP FX_STATESMART_Q_COMP
#define _FX_STATESMART_Q_EXEC FX_STATESMART_Q_EXEC
#endif

/* Given a p4cell or dp4cell these macros access the half cell components of it: */
/* (yes yes this is non ANSI C but it works) */
#define	P4xW0(X)		(((p4ucell_hi_lo *)&(X))->hi)
#define	P4xW1(X)		(((p4ucell_hi_lo *)&(X))->lo)

#define	P4xD0(X)		P4xW0((X).hi)
#define	P4xD1(X)		P4xW1((X).hi)
#define	P4xD2(X)		P4xW0((X).lo)
#define	P4xD3(X)		P4xW1((X).lo)

#define P4xCELL(HI,LO)	((p4cell)(HI) << (sizeof(p4cell)*4) | (LO))

#ifndef P4_NO_FP
#define P4_IF_FP_(X) X
#else
#define P4_IF_FP_(X)
#endif

/*@}*/
#endif 
