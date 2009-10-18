#ifndef _PFE_DSTRINGS_H 
#define _PFE_DSTRINGS_H

/**
 * PFWords: Public types, prototypes, and macros for
 *          Dynamic-Strings words.
 *
 * Copyright (C) 2001-2004, 2006, 2008 David N. Williams
 *
 * @see LGPL
 * @author David N. Williams             (modified $Author guidod $)
 * @version %version: 0.7.6 %            ($Revision: 1.4 $)
 *   (modified $Date: 2008-09-11 23:05:06 $)
 *   (%date_modified: Fri Aug 22 16:10:00 2008 %)
 *   (%date_modified: Tue Oct 24 17:13:40 2006 %)
 *   (%date_modified: Mon Jul 19 11:29:00 2004 %)
 *   (%date_modified: Mon Jun 10 09:00:00 2002 %)
 *   (%date_modified: Wed May 29 16:30:00 2002 %)
 *   (%date_modified: Fri Apr 27 18:13:58 2001 %)
 *     starting date: Sat Dec 16 14:00:00 2000 %)
 * @description
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later
 * version.
 *
 * This library is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Library General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA.
 *
 * If you take advantage of the option in the LGPL to put a
 * particular version of this library part under the GPL, the
 * author would regard it as polite if you would put any direct
 * modifications under the LGPL as well, and include a copy of
 * this request near the beginning of the modified library
 * source.  A "direct modification" is one that enhances or
 * extends the library in line with its original concept, as
 * opposed to developing a distinct application or library which
 * might use it.
 * 
 * Please direct any comments to david.n.williams at umich.edu.
 */

#include <stdlib.h>	/* size_t */
#include <limits.h>     /* ULONG_MAX, UINT_MAX */

/* COUNT FIELD SIZE
 *
 * The count field type is defined through PFE_CASEOF_MCOUNT.
 * The cases are 1, 2, or 3:
 * 
 * 	1: unsigned char  <-- deprecated
 * 	2: unsigned short <-- deprecated
 * 	3: unsigned long  <-- default
 * 
 * P4_MAX_DATA_MSTR restricts the size of measured string copies
 * into data space and defaults to the largest unsigned integer
 * that fits into the count field.  Although not a USER-CONFIG,
 * it can in principle be set outside of this file, in which
 * case it is forced here to be no larger than the default.
 */ 

/* #define PFE_CASEOF_MCOUNT 1 */  		/* USER-CONFIG */

/* Changing the default from unsigned long is deprecated. */
#ifndef PFE_CASEOF_MCOUNT
  #define PFE_CASEOF_MCOUNT 3			/* default is long */
#endif

#if   PFE_CASEOF_MCOUNT == 3
  #define P4_TYPEOF_MCOUNT  unsigned long
  #define P4_MAX_MCOUNT     ULONG_MAX
#elif PFE_CASEOF_MCOUNT == 2
  #define P4_TYPEOF_MCOUNT  unsigned short
  #define P4_MAX_MCOUNT     USHRT_MAX
#elif PFE_CASEOF_MCOUNT == 1
  #define P4_TYPEOF_MCOUNT  unsigned char
  #define P4_MAX_MCOUNT     UCHAR_MAX
#else
  #error  PFE_CASEOF_MCOUNT must be 1, 2, or 3
#endif

/* ensure consistent data space copy limit */
#ifndef P4_MAX_DATA_MSTR
  #define P4_MAX_DATA_MSTR  P4_MAX_MCOUNT
#endif
#if P4_MAX_DATA_MSTR > P4_MAX_MCOUNT
  #undef  P4_MAX_DATA_MSTR
  #define P4_MAX_DATA_MSTR  P4_MAX_MCOUNT
#endif

#ifndef P4_STR_BUFFER_SIZE       /* USER-CONFIG */
#define P4_STR_BUFFER_SIZE 4096  /* initial string buffer size in bytes */
#endif
#ifndef P4_MAX_SFRAMES           /* USER-CONFIG */
#define P4_MAX_SFRAMES 4         /* initial max number of string frames */
#endif

typedef P4_TYPEOF_MCOUNT p4_MCount;  /* string count */

#define P4_SIZEOF_MCOUNT       (sizeof(p4_MCount))
#define P4_SIZEOF_DSTR_HEADER  (PFE_SIZEOF_CELL + P4_SIZEOF_MCOUNT) 

#undef UCELL_MAX
#if PFE_SIZEOF_CELL == PFE_SIZEOF_INT
  #define UCELL_MAX  UINT_MAX
#elif PFE_SIZEOF_CELL == PFE_SIZEOF_LONG
  #define UCELL_MAX  ULONG_MAX
#else
  #error  Cell type must be int or long.
#endif

typedef struct p4_MStr p4_MStr;		      /* measured string */
typedef struct p4_DStr p4_DStr;		      /* dynamic string */
typedef struct p4_StrFrame p4_StrFrame;	      /* string stack frame */
typedef struct p4_StrSpace p4_StrSpace;

struct p4_MStr		/* measured string */
{
  p4_MCount count;	/* size of string body without padding */
  char body;
};

struct p4_DStr		/* dynamic string */
{
  p4_MStr **backlink;
  p4_MCount count;
  char body;
};

struct p4_StrFrame     /* frame stack item */
{  
  p4_MStr **top;       /* pointer to frame top */
  p4ucell num;         /* number of strings in frame */
};

struct p4_StrSpace
{
  size_t size;          /* size of string buffer plus stack,
                           excluding guard null */
  size_t numframes;     /* maximum number of string stack frames */
  p4_DStr *buf;         /* pointer to start of string buffer */
  p4_DStr *sbreak;      /* pointer to next available space after
                           strings, normally aligned */
  p4_MStr **sp;         /* string stack pointer */
  p4_MStr **sp0;        /* initial string stack pointer, address of
                           last cell in string space */
  p4_StrFrame *fbreak;  /* top of frame stack limit pointer */
  p4_StrFrame *fp;      /* frame stack pointer */
  p4_StrFrame *fp0;     /* initial frame stack pointer */
  p4_MStr *cat_str;     /* pointer to the last string in the string
                           buffer, if concatenation is in progress,
                           else NULL */
  short garbage_flag;   /* true when there is garbage */
  short garbage_lock;   /* true when collection not permitted */
  short args_flag;      /* true when compiling with macro args */
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
#define MAX_MCOUNT	   P4_MAX_MCOUNT
#define MAX_DATA_STR	   P4_MAX_DATA_MSTR
#define SIZEOF_MCOUNT	   P4_SIZEOF_MCOUNT
#define SIZEOF_DSTR_HEADER P4_SIZEOF_DSTR_HEADER

typedef p4_MCount   MCount;
typedef p4_MStr     MStr;
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
 * THROW an error if there is not enough aligned room in string
 * space for NUMBYTES starting at START, even after garbage
 * collection.  Note that we compare against an aligned address,
 * so no rounding is necessary.
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
 * Note that A must be of type (MStr *).
 */

#define P4_PUSH_STR( A )	\
  P4_Q_ROOM (SBREAK, PFE_SIZEOF_CELL);\
  SSP[-1] = A; SSP--

#define P4_MLEN( MSTR )   (MSTR)->count
#define P4_MADDR( MSTR )  &(MSTR)->body

#define P4_SKIP_MSTRING(X)  P4_ADD (X,\
  P4_ALIGNTO_CELL (*(p4_MCount *)X + sizeof(p4_MCount)))
#define FX_SKIP_MSTRING  P4_SKIP_MSTRING(p4IP)

#ifdef _P4_SOURCE
#define ALIGNTO_CELL(P) P4_ALIGNTO_CELL(P)
#define Q_ROOM( START, NUMBYTES ) P4_Q_ROOM( START, NUMBYTES )
#define Q_CAT P4_Q_CAT
#define PUSH_STR( A ) P4_PUSH_STR( A )
#define MLEN( MSTR ) P4_MLEN( MSTR )
#define MADDR( MSTR ) P4_MADDR( MSTR )
#endif

/************************************************************************/
/* functions and words							*/
/************************************************************************/

void p4_clear_str_space (p4_StrSpace *space);
p4_StrSpace *p4_make_str_space (size_t size, size_t frames);
int p4_collect_garbage (void);
p4_MStr *p4_pop_str (void);
void p4_drop_all_strings (p4_StrSpace *space);
void p4_push_str_copy (const p4_char_t *addr, size_t len);
int p4_find_str_arg (const p4_char_t *nm, int l);
void p4_make_str_frame (p4ucell n);
FCode_XE (p4_marg_execution);
int p4_compile_marg (const p4_char_t *name, int len);

/* constants */
FCode (p4_empty_str);
FCode (p4_newline_str);
/* ANS Forth string extensions */
FCode (p4_parens_m_store);
FCode (p4_parse_to_s);
FCode_XE (p4_parse_to_s_execution);
FCode (p4_s_back_tick);
FCode_XE (p4_s_back_tick_execution);
FCode (p4_m_comma_s);
/* measured strings */
FCode (p4_m_count_fetch);
FCode (p4_m_count_store);
FCode (p4_m_count);
FCode (p4_minus_m_count);
/* string space */
FCode (p4_zero_strings);
FCode (p4_str_garbage_Q);
FCode (p4_str_gc_off);
FCode (p4_str_gc_on);
FCode (p4_str_gc_lock_fetch);
FCode (p4_str_gc_lock_store);
FCode (p4_str_unused);
FCode (p4_collect_str_garbage);
FCode (p4_make_str_space);
FCode (p4_slash_str_buf);
FCode (p4_max_num_str_frames);
/* load and store */
FCode (p4_str_store);
FCode (p4_str_fetch);
FCode (p4_str_quote);
FCode_XE (p4_str_quote_execution);
FCode (p4_str_back_tick);
FCode (p4_str_constant);
FCode_RT (p4_str_constant_RT);
FCode (p4_str_variable);
FCode (p4_parse_to_str);
FCode_XE (p4_parse_to_str_execution);
/* string stack */
FCode (p4_str_dot);
FCode (p4_str_two_drop);
FCode (p4_str_two_dup);
FCode (p4_str_depth);
FCode (p4_str_drop);
FCode (p4_str_dup);
FCode (p4_str_nip);
FCode (p4_str_over);
FCode (p4_str_pick);
FCode (p4_str_swap);
FCode (p4_str_exchange);
FCode (p4_str_s_from);
FCode (p4_str_comma_s);
FCode (p4_str_s_fetch);
FCode (p4_str_tuck);
FCode (p4_to_str_s);
FCode (p4_to_str_s_copy);
/* concatenation */
FCode (p4_str_plus);
FCode (p4_s_plus);
FCode (p4_parse_s_plus);
FCode_XE (p4_parse_s_plus_execution);
FCode (p4_endcat);
FCode (p4_str_plus_quote);
FCode_XE (p4_str_plus_quote_execution);
FCode (p4_str_plus_back_tick);
/* arguments */
FCode (p4_num_str_args);
FCode (p4_str_args_brace);
FCode_XE (p4_make_str_frame_execution);
FCode (p4_str_frame);
FCode (p4_str_frame_depth);
FCode (p4_drop_str_frame);
FCode (p4_find_str_arg);
FCode (p4_th_str_arg);
FCode (p4_do_drop_str_frame);
/* string stack support */
FCode (p4_str_pop);
FCode (p4_str_push_ext);
/* more string space */
FCode (p4_str_breakp_fetch);
FCode (p4_str_bufp_fetch);
FCode (p4_str_fbreakp_fetch);
FCode (p4_str_fsp0_fetch);
FCode (p4_str_fsp_fetch);
FCode (p4_str_sp0_fetch);
FCode (p4_str_sp_fetch);
FCode (p4_slash_str_frame_item);
FCode (p4_slash_str_frame_stack);
FCode (p4_slash_str_space_header);
FCode (p4_zero_str_space);
FCode (p4_cat_str_p_fetch);
FCode (p4_in_str_buffer_Q);

#endif  /* _PFE_DSTRINGS_H */
