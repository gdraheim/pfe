#ifndef _PFE_DSTRINGS_H 
#define _PFE_DSTRINGS_H "%full_filespec: dstrings-ext.h~0.6.5:incl:bln_12xx!1 %"

/**
 * PFWords: Public types, prototypes, and macros for
 *          Dynamic-Strings words.
 *
 * Copyright (C) 2001 David N. Williams
 *
 * @see LGPL
 * @author David N. Williams             @(#) %derived_by: guidod %
 * @version %version: 0.6.5 %
 *   (%date_modified: Mon Mar 12 10:32:11 2001 %)
 *     starting date: Sat Dec 16 14:00:00 2000 %)
 * @description
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA.
 *
 * If you take advantage of the option in the LGPL to put a particular
 * version of this library under the GPL, the author would regard it
 * as polite if you would put any direct modifications under the LGPL as
 * well, and include a copy of this request near the beginning of the
 * modified library source.  A "direct modification" is one that enhances
 * or extends the library in line with its original concept, as opposed to
 * developing a distinct application or library which might use it.
 * 
 * This file is based on the ^Forth Motorola 680x0 strings package
 * of June, 1999.
 * 
 * Please direct any comments to david.n.williams@umich.edu.
 */

#include <stdlib.h>	/* size_t */

#ifndef PFE_TYPEOF_SCOUNT       /* USER-CONFIG */
#define PFE_TYPEOF_SCOUNT short /* the packed string count (unsigned short) */
#endif
#ifndef P4_STR_BUFFER_SIZE      /* USER-CONFIG */
#define P4_STR_BUFFER_SIZE 4096 /* string buffer size in bytes */
#endif
#ifndef P4_MAX_SFRAMES          /* USER-CONFIG */
#define P4_MAX_SFRAMES 4        /* initial max number of string frames */
#endif

typedef unsigned PFE_TYPEOF_SCOUNT p4_PCount; /* packed string count */
typedef struct p4_PStr p4_PStr;		      /* packed string */
typedef struct p4_DStr p4_DStr;		      /* dynamic string */
typedef struct p4_StrFrame p4_StrFrame;	      /* string stack frame */
typedef struct p4_StrSpace p4_StrSpace;

struct p4_PStr
{
  p4_PCount count;	/* size of string body without padding */
  char body;
};

struct p4_DStr
{
  p4_PStr **backlink;
  p4_PCount count;
  char body;
};

struct p4_StrFrame     /* frame stack item */
{  
  p4_PStr **top;       /* pointer to frame top */
  p4ucell num;         /* number of strings in frame */
};

struct p4_StrSpace
{
  size_t size;		/* size of string buffer plus stack,
			   excluding guard null */
  size_t numframes;	/* maximum number of string stack frames */
  p4_DStr *buf;		/* pointer to start of string buffer */
  p4_DStr *sbreak;	/* pointer to next available space after
			   strings, normally aligned */
  p4_PStr **sp;		/* string stack pointer */
  p4_PStr **sp0;	/* initial string stack pointer, address of
			   last cell in string space */
  p4_StrFrame *fbreak;	/* top of frame stack limit pointer */
  p4_StrFrame *fp;	/* frame stack pointer */
  p4_StrFrame *fp0;	/* initial frame stack pointer */
  p4_PStr *cat_str;	/* pointer to the last string in the string
			   buffer, if multiple concatenation is in
			   progress, else NULL */
  short garbage_flag;	/* true when there is garbage */
  short garbage_lock;	/* true when collection not permitted */
  short args_flag;	/* true when compiling with macro args */
};

/* shortcuts */
#define p4_DSTRINGS     ((p4_StrSpace*)(PFE.dstrings))
#define p4_SBUFFER	p4_DSTRINGS->buf
#define p4_SBREAK	p4_DSTRINGS->sbreak
#define p4_SFBREAK	p4_DSTRINGS->fbreak
#define p4_SSP		p4_DSTRINGS->sp
#define p4_SSP0		p4_DSTRINGS->sp0
#define p4_SFSP		p4_DSTRINGS->fp
#define p4_SFSP0	p4_DSTRINGS->fp0
#define p4_CAT_STR	p4_DSTRINGS->cat_str
#define p4_GARBAGE_FLAG	p4_DSTRINGS->garbage_flag
#define p4_GARBAGE_LOCK	p4_DSTRINGS->garbage_lock
#define p4_MARGS_FLAG	p4_DSTRINGS->args_flag

#ifdef _P4_SOURCE
typedef p4_PCount   PCount;
typedef p4_PStr     PStr;
typedef p4_DStr     DStr;
typedef p4_StrSpace StrSpace;
typedef p4_StrFrame StrFrame;

#define DSTRINGS        p4_DSTRINGS
#define SBUFFER		p4_SBUFFER
#define SBREAK		p4_SBREAK
#define SFBREAK		p4_SFBREAK
#define SSP		p4_SSP
#define SSP0		p4_SSP0
#define SFSP		p4_SFSP
#define SFSP0		p4_SFSP0
#define CAT_STR		p4_CAT_STR
#define GARBAGE_FLAG	p4_GARBAGE_FLAG
#define GARBAGE_LOCK	p4_GARBAGE_LOCK
#define MARGS_FLAG	p4_MARGS_FLAG
#endif

/************************************************************************/
/* string space macros							*/
/************************************************************************/

#define P4_ALIGNTO_CELL(P) \
  ((size_t)( ( ((size_t)P) & (PFE_ALIGNOF_CELL - 1) )       \
    ? ( ( ((size_t)P) & ~(PFE_ALIGNOF_CELL - 1) ) + PFE_ALIGNOF_CELL )  \
      : (size_t)P ))

/* ENSURE STRING SPACE ROOM
 *
 * THROW an error if there is not enough aligned room in string space for    
 * NUMBYTES starting at START, even after garbage collection.  Note that we
 * compare against an aligned address, so no rounding is necessary.
 */

#define P4_Q_ROOM( START, NUMBYTES )	\
if ( (void*) ((size_t) START + NUMBYTES) > (void*) p4_SSP )\
  {\
    if (!p4_collect_garbage()\
	|| (void*) ((size_t) START + NUMBYTES) > (void*) p4_SSP)\
      p4_throw (P4_ON_SSPACE_OVERFLOW);\
   }

#define P4_Q_CAT	\
if (p4_CAT_STR) p4_throw (P4_ON_SCAT_LOCK)

/* PUSH EXISTING STRING
 *
 * Note that A must be of type (PStr *).
 */

#define P4_PUSH_STR( A )	\
P4_Q_ROOM (SBREAK, PFE_SIZEOF_CELL);\
*--SSP = A

#define P4_SLEN( PSTR )	(PSTR)->count
#define P4_SADDR( PSTR )	&(PSTR)->body

#define P4_SKIP_PSTRING(X)  P4_ADD (X,\
  P4_ALIGNTO_CELL (*(p4_PCount *)X + sizeof(p4_PCount)))
#define FX_SKIP_PSTRING  P4_SKIP_PSTRING(p4IP)

#ifdef _P4_SOURCE
#define ALIGNTO_CELL(P) P4_ALIGNTO_CELL(P)
#define Q_ROOM( START, NUMBYTES ) P4_Q_ROOM( START, NUMBYTES )
#define Q_CAT P4_Q_CAT
#define PUSH_STR( A ) P4_PUSH_STR( A )
#define SLEN( PSTR ) P4_SLEN( PSTR )
#define SADDR( PSTR ) P4_SADDR( PSTR )
#endif

/************************************************************************/
/* functions and words							*/
/************************************************************************/

void p4_clear_str_space (p4_StrSpace *space);
p4_StrSpace *p4_make_str_space (size_t size, size_t frames);
int p4_collect_garbage (void);
p4_PStr *p4_pop_str (void);
void p4_drop_all_strings (p4_StrSpace *space);
void p4_push_str_copy (char *addr, size_t len);
int p4_find_arg (char *nm, int l);
void p4_make_str_frame (p4ucell n);
FCode (p4_marg_execution);
int p4_compile_marg (char *name, int len);

FCode (p4_empty_str);
FCode (p4_newline_str);
FCode (p4_s_comma);
FCode (p4_zero_strings);
FCode (p4_str_gc_off);
FCode (p4_str_gc_on);
FCode (p4_str_unused);
FCode (p4_collect_str_garbage);
FCode (p4_make_str_space);
FCode (p4_str_quote);
FCode (p4_str_constant);
FCode (p4_str_variable);
FCode (p4_args_brace);
FCode (p4_make_str_frame_execution);
FCode (p4_m_quote);
FCode (p4_m_tick);
FCode (p4_str_depth);
FCode (p4_str_drop);
FCode (p4_str_dup);
FCode (p4_str_over);
FCode (p4_str_swap);
FCode (p4_str_s_from);
FCode (p4_str_s_from_copy);
FCode (p4_str_s_fetch);
FCode (p4_to_str_s);
FCode (p4_to_str_s_copy);
FCode (p4_str_store);
FCode (p4_str_dot);
FCode (p4_str_fetch);
FCode (p4_cat);
FCode (p4_endcat);
FCode (p4_str_frame);
FCode (p4_drop_str_frame);
FCode (p4_find_arg);

  /* ndef __DSTRINGS_H */
#endif
