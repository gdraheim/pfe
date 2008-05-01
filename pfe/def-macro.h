#ifndef __PFE_DEF_MACRO_H                              /* -*- width: 100 -*- */
#define __PFE_DEF_MACRO_H

/** 
 * -- macro definitions for the portable forth environment
 * 
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.8 $
 *     (modified $Date: 2008-05-01 00:42:01 $)
 */
/*@{*/

#include <pfe/def-config.h>
#include <pfe/def-const.h>
#include <pfe/def-regmacro.h>

/* determines the dimension of any given vector */
#ifndef DIM
#define DIM(X)		((int)(sizeof (X) / sizeof *(X)))
#endif

/* wipe any given vector X */
#ifndef _p4_buf_zero
#define _p4_buf_zero(X)	(p4_memset ((X), 0, sizeof (X)))
#endif
#ifndef _p4_var_zero
#define _p4_var_zero(X)	(p4_memset ((&X), 0, sizeof (X)))
#endif

/* copy any given vector Y to X */
#ifndef _p4_buf_copy
#define _p4_buf_copy(X,Y)	(p4_memcpy ((X), (Y), sizeof (X)))
#endif
#ifndef _p4_var_copy
#define _p4_var_copy(X,Y)	(p4_memcpy ((&X), (Y), sizeof (X)))
#endif

/* return the byte offset of a given component to beginning of structure: */
#ifndef OFFSET_OF
#define OFFSET_OF(T,C)	((char *)&(((T *)0)->C) - (char *)0)
#endif

#if defined __GNUC__ && !defined __cplusplus
# if __GNUC__+3 < 3 && __GNUC_MINOR_+0 < 3 && !defined __STRICT_ANSI__
# define PFE_NO_ALIAS_POINTER 1
# elif defined PFE_USE_REGS
/* in GCC --with-regs we can not take the adress operator even if we want to */
# define PFE_GCC_REGS_POINTER 1
# endif
#endif

/* inc/decrement, push/pop of arbitrary types with arbitrary pointers */
#if PFE_NO_ALIAS_POINTER+0
/* Use non-ANSI lvalue-casts avoiding address-of operator: */
#define P4_ADD(P,N)	((char *)(P) += (N))
#define P4_DEC(P,T)	(--((T *)(P)))
#define P4_INC(P,T)	(((T *)(P))++)
#define P4_DECC(P,T)	(--((T *)(P)))
#define P4_INCC(P,T)	(((T *)(P))++)
#define P4_DECR(P,T,N)	(((T *)(P)) -= (N))
#define P4_INCR(P,T,N)	(((T *)(P)) += (N))
#elif PFE_GCC_REGS_POINTER+0
#define P4_ADD(P,N)	({ char * _p = (void*)(P); _p += (N); (P) =(void*)( _p ); _p; })
#define P4_DEC(P,T)	({ T * _p = (void*)(P); _p -=1; (P) =(void*)( _p ); _p; }) 
#define P4_INC(P,T)	({ T * _p = (void*)(P); (P) =(void*)( _p+1 ); _p; })
#define P4_DECC(P,T)	({ T * _p = (void*)(P); _p -=1; (P) =(p4cell)( _p ); _p; }) 
#define P4_INCC(P,T)	({ T * _p = (void*)(P); (P) =(p4cell)( _p+1 ); _p; })
#define P4_DECR(P,T,N)	({ T * _p = (void*)(P); _p -=(N); (P) =(void*)( _p ); _p; })
#define P4_INCR(P,T,N)	({ T * _p = (void*)(P); (P) =(void*)( _p+(N) ); _p; })
#elif defined MSDOS
#define P4_ADD(P,N)	((char *huge)(P) += (int)(N))
#define P4_DEC(P,T)	(--((T *huge)(P)))
#define P4_INC(P,T)	(((T *huge)(P))++)
#define P4_DECC(P,T)	(--((T *huge)(P)))
#define P4_INCC(P,T)	(((T *huge)(P))++)
#define P4_DECR(P,T,N)	(((T *huge)(P)) -= (N))
#define P4_INCR(P,T,N)	(((T *huge)(P)) += (N))
#else
/* Force (or fool) ANSI-C to do typecast's it normally refuses by */
/* casting pointers, not objects, then reference the casted pointers: */
#define P4_ADD(P,N)	(*(char **)&(P) += (N))
#define P4_DEC(P,T)	(--(*(T **)&(P)))
#define P4_INC(P,T)	((*(T **)&(P))++)
#define P4_DECR(P,T,N)	((*(T **)&(P)) -= (N))
#define P4_INCR(P,T,N)	((*(T **)&(P)) += (N))
#define P4_DECC(P,T)	(--(*(T **)&(P)))
#define P4_INCC(P,T)	((*(T **)&(P))++)
#endif

/* next generation of the macros above - we keep those for a while just
 * for compatibility with old code. At some point we will remove the
 * macros above and only keep the ones below. To change your code:
 *      P4_INC(p4SP,p4char)    is the same as 
 *      P4_INC_(p4char*,p4SP)  and both represent ((p4char*)p4SP)++
 * Note that there are a lot of P4_VAR casts around that need to be
 * replaced with one of the macros below, usually P4_INCR_ (add value)
 */

/* inc/decrement, push/pop of arbitrary types with arbitrary pointers */
#if PFE_NO_ALIAS_POINTER+0
/* Use non-ANSI lvalue-casts avoiding address-of operator: */
#define P4_PTR_(T,P)	((T)(P))
#define P4_DEC_(T,P)	(--((T)(P)))
#define P4_INC_(T,P)	(((T)(P))++)
#define P4_DECC_(T,P)	(--((T)(P)))
#define P4_INCC_(T,P)	(((T)(P))++)
#define P4_DECR_(T,P,N)	(((T)(P)) -= (N))
#define P4_INCR_(T,P,N)	(((T)(P)) += (N))
#define P4_ADD_(P,N,T)	(((T*)(P)) += (N))
#elif PFE_GCC_REGS_POINTER+0
#define P4_PTR_(T,P)	((T)(P))
#define P4_DEC_(T,P)	({ T _p=(void*)(P); _p -=1; (P) =(void*)( _p ); _p;}) 
#define P4_INC_(T,P)	({ T _p=(void*)(P); (P) =(void*)( _p+1 ); _p;})
#define P4_DECC_(T,P)	({ T _p=(void*)(P); _p -=1; (P) =(p4cell)( _p ); _p;})
#define P4_INCC_(T,P)	({ T _p=(void*)(P); (P) =(p4cell)( _p+1 ); _p;})
#define P4_DECR_(T,P,N)	({ T _p=(void*)(P); _p -=(N); (P) =(void*)( _p ); _p;})
#define P4_INCR_(T,P,N)	({ T _p=(void*)(P); (P) =(void*)( _p+(N) ); _p;})
#define P4_ADD_(P,N,T)	({ T*_p=(void*)(P); (P) =(void*)( _p+(N) );})
#elif defined MSDOS
#define P4_PTR_(T,P)	((T huge)(P))
#define P4_DEC_(T,P)	(--((T huge)(P)))
#define P4_INC_(T,P)	(((T huge)(P))++)
#define P4_DECC_(T,P)	(--((T huge)(P)))
#define P4_INCC_(T,P)	(((T huge)(P))++)
#define P4_DECR_(T,P,N)	(((T huge)(P)) -= (N))
#define P4_INCR_(T,P,N)	(((T huge)(P)) += (N))
#define P4_ADD_(P,N,T)	(((T*huge)(P)) += (N))
#else
/* Force (or fool) ANSI-C to do typecast's it normally refuses by */
/* casting pointers, not objects, then reference the casted pointers: */
#define P4_PTR_(T,P)	((T)(P))
#define P4_DEC_(T,P)	(--(*(T*)&(P)))
#define P4_INC_(T,P)	((*(T*)&(P))++)
#define P4_DECR_(T,P,N)	((*(T*)&(P)) -= (N))
#define P4_INCR_(T,P,N)	((*(T*)&(P)) += (N))
#define P4_DECC_(T,P)	(--(*(T*)&(P)))
#define P4_INCC_(T,P)	((*(T*)&(P))++)
#define P4_ADD_(P,N,T)	((P) = (void*)( ((T*)(void*)(P)) + (N) ))
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

# define P4_WP_NFA	(p4_to_name(p4WP))
# define P4_WP_CFA	(p4WP)		 
# define P4_WP_PFA	((p4cell *)&p4WP [1]) 

# ifndef PFE_WITH_FFA
#   define P4_NAME_GETFLAGS(X) (*(p4char*)X)   /* == (*P4_NFA2FLAGS(X)) */
#   define P4_NAME_TO_FLAGS(X)  ((p4char*)X)   /* NFA -> FFA w/o FFA-byte */
#   define P4_NAME_MASK_LEN(X)  ((X)&31) 	   /* NFA -> count of namefield */
#   define P4_NAME_SIZE_MAX     31             /* used for buffer-sizes */
# else
#   define P4_NAME_GETFLAGS(X)   (((p4char*)X)[-1]) /* == (*P4_NFA2FLAGS(X)) */
#   define P4_NAME_TO_FLAGS(X)  (&((p4char*)X)[-1]) /* NFA -> FFA w/ FFA-byte */
#   define P4_NAME_MASK_LEN(X)  (X)
#   define P4_NAME_SIZE_MAX     127             /* C99 defines SIZE_MAX for size_t */
# endif

/* newstyle */
# define P4_NAMEPTR(X)   (((p4_namebuf_t*)(X))+1)
# define P4_NAMELEN(X)   P4_NAME_MASK_LEN(*(p4_namebuf_t*)X)
# define P4_NAME_TO_START(X) P4_NAME_TO_FLAGS(X)

/* oldstyle */
# define P4_NFACNT(X)     P4_NAME_MASK_LEN(X)
# define P4_NFACNTMAX     P4_NAME_SIZE_MAX
# define P4_NFA2FLAGS(X)  P4_NAME_TO_FLAGS(X)
# define P4_NFA_FLAGS(X)  P4_NAME_GETFLAGS(X)

# define P4_NFA_PTR(X)    P4_NAMEPTR(X)
# define P4_NFA_LEN(X)    P4_NAMELEN(X)

# define P4_NFA2START(X)  P4_NFA2FLAGS(X)
# define P4_NFA2COUNT(X)  P4_NFACNT(X)
# define P4_NFA2CHARS(X)  P4_NFACHARS(X)

#define P4_CHARBUF_LEN(X)     (*((p4char*)(X)))
#define P4_CHARBUF_PTR(X)     (((p4char*)(X))+1)

#ifdef _P4_SOURCE
/* useful shortcuts */

# define WP_NFA		(p4_to_name(p4WP))
# define WP_CFA		(p4WP)		 
# define WP_PFA		((p4cell *)&p4WP [1]) 

# define NFA_P(X)	(p4_to_name (X))	/* CFA -> NFA */
# define CFA_P(X)	(X)			/* CFA -> CFA */
# define PFA_P(X)	(&((p4cell*)(X))[1])	/* CFA -> PFA */

 /* P4CNT(X) for counted strings (not names): avoid hickups with gcc > 2.7.x */
# define P4CNT(X) ((unsigned)(*(p4char*)(X)))

# define NAMELEN(X)       P4_NAMELEN(X)
# define NAMEPTR(X)       P4_NAMEPTR(X)
# define NAME_SIZE_MAX    P4_NAME_SIZE_MAX
# define NAME_TO_FLAGS(X) P4_NAME_TO_FLAGS(X)
# define NAME_GETFLAGS(X) P4_NAME_GETFLAGS(X)

/* oldstyle */
# define _FFA(X)	P4_NFA2FLAGS(X)	 /* NFA -> FFA w/o FFA-byte */
# define NFA2FF(X)	P4_NFA2FLAGS(X)	 /* NFA -> FFA w/o FFA-byte */
# define NFACNT(X)	P4_NFACNT(X) 	 /* NFA -> count of namefield */
# define NFACNTMAX      P4_NFACNTMAX     /* used for buffer-sizes */

# define CELL_MAX	((p4cell)((p4ucell)-1 >> 1))
# define UCELL_MAX	((p4ucell)-1)
/* #define RP		((p4cell *)p4RP) */
# if !defined PFE_SBR_CALL_THREADING
# define RP_PUSH(X)	P4_PUSH(X, p4RP)
# define RP_POP()	P4_POP(p4RP)
# endif
#endif

#define FX_RP           ((p4cell*)p4RP)

#define FX_POP_(T)     (*((T*)p4SP)++)

#if 1
#define FX_PUSH(X)      (*--p4SP = (p4cell)(X))
#define FX_PUSH_SP      (*--p4SP)
#define FX_PUSH_RP      (*--p4RP)
#define FX_POP_RP	(*p4RP++)
#define FX_POP		(*p4SP++)
#define FX_DUP___	(*--p4SP = p4SP[0])
#define FX_OVER___	(*--p4SP = p4SP[1])
#define FX_NIP___	(*p4SP = *p4SP++)
#define FX_DUP		(--p4SP, p4SP[0] = p4SP[1], p4SP)
#define FX_OVER		(--p4SP, p4SP[0] = p4SP[2], p4SP)
#define FX_NIP          (p4SP[1] = p4SP[0], p4SP++)
#else
#define FX_PUSH(X)      (P4_PUSH((p4cell)(X), p4SP))
#define FX_PUSH_SP      (P4_POSH(p4SP))
#define FX_PUSH_RP      (P4_POSH(p4RP))
#define FX_POP_RP	(P4_POP(p4RP))
#define FX_POP		(P4_POP(p4SP))
#define FX_DUP		(P4_PUSH(p4SP[0], p4SP))
#define FX_OVER		(P4_PUSH(p4SP[1], p4SP))
#define FX_NIP		(*p4SP = FX_POP)
#endif
#define P4_ROOM(P,N)    (P4_DECR(P,p4cell,N))
#define P4_DROP(P,N)    (P4_INCR(P,p4cell,N))
#define FX_SP_ROOM(N)   P4_ROOM(p4SP,N)
#define FX_SP_DROP(N)   P4_DROP(p4SP,N)
#define FX_DROP		(P4_INC(p4SP,p4cell))
#define FX_2DROP	FX_SP_DROP(2)
#define FX_3DROP	FX_SP_DROP(3)
#define FX_4DROP	FX_SP_DROP(4)
#define FX_5DROP	FX_SP_DROP(5)
#define FX_6DROP	FX_SP_DROP(6)
#define FX_7DROP	FX_SP_DROP(7)
#define FX_1ROOM        FX_SP_ROOM(1)
#define FX_2ROOM        FX_SP_ROOM(2)
#define FX_3ROOM        FX_SP_ROOM(3)
#define FX_4ROOM        FX_SP_ROOM(4)
#define FX_5ROOM        FX_SP_ROOM(5)
#define FX_6ROOM        FX_SP_ROOM(6)
#define FX_7ROOM        FX_SP_ROOM(7)

#if 0 /* FIXME: don't write (*IP) but DATA_IN_CODE (this here is ITC'ish) */
#define	FX_BRANCH	P4_ADD (p4IP, *(p4cell *)p4IP)
#else
#define	FX_BRANCH	(p4IP = (p4xcode*)*p4IP)
#endif
#define FX_SKIP_BRANCH  (p4IP++)

#ifdef  FX_SBR_CALL_THREADING
#define FX_SKIP_STRING  \
    __FX_SKIP_STRING_impossible_while_SBR_threading_USE_the_FX_NEW_IP_words__
#else
#define FX_SKIP_STRING     P4_SKIP_STRING(p4IP);
#endif
#define P4_SKIP_STRING(X)  P4_ADD (X, p4_aligned (*(p4char*)X + 1))
#define P4_ALIGNED(P)	(((size_t)(P) & (PFE_ALIGNOF_CELL - 1)) == 0)
#define P4_DFALIGNED(P)	(((size_t)(P) & (PFE_ALIGNOF_DFLOAT - 1)) == 0)
#define P4_SFALIGNED(P)	(((size_t)(P) & (PFE_ALIGNOF_SFLOAT - 1)) == 0)
#define P4_ALIGNED_VALUE(X)  (( ((p4cell)(X)) + \
                                (PFE_SIZEOF_CELL-1) ) &~ (PFE_SIZEOF_CELL-1) )


/* P:dictpointer X:value Y:hintchar T:typedef */
#define	P4_COMMA_(P,X,Y,T) (*(T *)(P) = (T)(X), P4_INC (P, T))
#define P4_COMMA(P,X,Y)  P4_COMMA_(P,X,Y,p4cell)
#define P4_BCOMMA(P,X)   P4_COMMA_(P,X,0,unsigned char)
#define P4_WCOMMA(P,X)   P4_COMMA_(P,X,0,unsigned short)
#define P4_LCOMMA(P,X)   P4_COMMA_(P,X,0,p4ucell)
#define P4_PCOMMA(P,X)   P4_COMMA_(P,X,0,void*)
#define	FX_COMMA(X)	 P4_COMMA(p4_DP,X,0)
#define	FX_COMMA_(X,Y)	 P4_COMMA(p4_DP,X,Y)
#define FX_FCOMMA(X)	 P4_COMMA_(p4_DP,(X),'F',double)
#define FX_XCOMMA(X)     P4_COMMA_(p4_DP,(X),'X',p4xt)
#define FX_ZCOMMA(X)     P4_COMMA_(p4_DP,(X),'Z',p4xt)
#define FX_RCOMMA(X)     P4_COMMA_(p4_DP,(X),'R',p4code)
#define FX_PCOMMA(X)     P4_COMMA_(p4_DP,(X),'P',void*)
#define FX_QCOMMA(X)     P4_COMMA_(p4_DP,(X),'Q',void*)
#define FX_BCOMMA(X)     P4_COMMA_(p4_DP,(X),'B',unsigned char)
#define FX_WCOMMA(X)     P4_COMMA_(p4_DP,(X),'W',unsigned short)
#define FX_LCOMMA(X)     P4_COMMA_(p4_DP,(X),'L',p4ucell)
#define FX_UCOMMA(X)     P4_COMMA_(p4_DP,(X),'U',p4ucell)
#define FX_VCOMMA(X)     P4_COMMA_(p4_DP,(X),'V',p4ucell)
#define FX_SCOMMA(X)     P4_COMMA_(p4_DP,(X),'S',p4cell)
#define FX_DP_ADD(X)     (p4_DP += X)
#define FX_DP_ADD_(X,Y)  (p4_DP += X)
#define FX_HERE_COMMA    (FX_QCOMMA(p4_HERE))
#define p4_HERE          ((p4char*const)(p4_DP))
#define FX_ALLOT(X)      ((p4_DP) += (X))
#define	FX_DEPTH	 (PFE.s0 - p4SP)

/* typed comma:
   X = exectoken = pointer to pointer to C-routine =~ pointer to code-field
   R = runtime = pointer to C-routine = code-field (needs .so relocation!)
   P = pointer into the dictionary
   Q = pointer into compiled colon word =~ P (i.e. HERE)
   C = character =~ byte-wide number
   S = singlecell-wide (signed) number or item of cell-element stringspan
   U = singlecell-wide (unsigned) number or item of cell-element stringspan
   D = doublecell (signed) number or item of doubecell-element stringspan
   B = byte-wide number or item of byte-element stringspan
   W = word-wide number or item of word-element stringspan
   L = long-wide number or item of long-element stringspan
   F = floating-pointer number
   I = bitfield (e.g. flags-byte of headers)
   Z = pointer to runtime-address in C-datafield =~ X (e.g. semantics)
   V = value, probably a number but can not be sure, must better check
   ... bigcaps for the start of the element ... lowcaps for extensions ...
*/


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

#if defined __WATCOMC__ || defined _AIX_CC
#define P4_VAR(_TYPE,_VAR) (*((_TYPE*)&(_VAR)))
#else
#define P4_VAR(_TYPE,_VAR)    ((_TYPE)(_VAR))
#endif

/*@}*/
#endif 
