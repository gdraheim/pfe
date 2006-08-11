/**
 * -- Dynamic-Strings words
 *
 *  Copyright (C) 2001, 2002, 2003, 2004 David N. Williams
 *
 *  @see     GNU LGPL
 *  @author  David N.Williams           (modified by $Author: guidod $)
 *  @version $Revision: 1.2 $
 *     (modified $Date: 2006-08-11 22:56:04 $)
 *      starting date:  Sat Dec 16 14:00:00 2000
 *
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
 * This code is based on the ^Forth Motorola 680x0 strings
 * package as of June, 1999.
 * 
 * Please direct any comments to david.n.williams@umich.edu.
 */

#include <pfe/def-config.h>

#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: dstrings-ext.c,v 1.2 2006-08-11 22:56:04 guidod Exp $";
#endif

/* ------------------------------------------------------------------- */

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <stddef.h>		/* offsetof() */
#include <pfe/dstrings-ext.h>	/* stdlib.h */
#include <pfe/os-string.h>

#include <pfe/option-ext.h>     /* p4_search_option_value */

/* make_str_frame_execution / marg_execution helper global for SEE */
static unsigned int frame_size;  /* fixme: not MT but acceptable */

#ifdef WRONG_SPRINTF		/* provision for buggy sprintf (SunOS) */
#define SPRFIX(X) p4_strlen(X)
#else
#define SPRFIX(X) X
#endif

#define P4_MARGS_MAGIC P4_MAGIC_('M','A','R','G')

/** MEASURED STRINGS
 *
 * This package uses a "measured string" representation for
 * strings in memory.  A measured string is the same as a Forth
 * counted string, except that instead of being restricted to
 * one character, the size of the count field is defined by the
 * implementation.  We default to unsigned long as the count
 * field type.  See dstrings-ext.h to change that.
 *
 * "Mstring" is short for "measured string".  The "MSA" is the
 * measured string address, the same as the count field address.
 *
 * An "ANS Forth string" is represented by (c-addr u), and need
 * not be stored as a counted string or measured string.
 *
 * We often use the suffix ".str" as shorthand for the ANS Forth
 * string pair representation on the data stack.
 *
 * We like the rule that MSA's generally should not appear on
 * the data stack, except for situations like system
 * development.
 */

/************************************************************************/
/* functions								*/
/************************************************************************/

/* STORE AN ANS FORTH STRING AS A MEASURED STRING
 *
 * This function stores an ANS Forth string into memory, assumed
 * to be cell aligned, as a measured string.  It throws an error
 * if the count is larger than MAX_DATA_STR, which is itself not
 * larger than MAX_MCOUNT.  Zero padding, not included in the
 * count, is added to the string body up to the first cell
 * boundary following the string.  Sufficient room is assumed. 
 * It returns the address just after the padding.
 */

static p4_char_t *
p4_mstring_place (const p4_char_t *addr, size_t len, p4_char_t *p)
{
  p4_char_t *limit;

  if (len >= MAX_DATA_STR)
    p4_throw (P4_ON_SCOUNT_OVERFLOW);

  *P4_INC_(MCount*,p) = (MCount) len;  /* store count */
  while (len-- != 0)                    /* store string */
    *p++ = *addr++;

  limit = (p4_char_t *) ALIGNTO_CELL (p);
  while ( p < limit ) 
    *p++ = 0;
  return p;  /* address just after padding */
}

/* STORE AN ANS FORTH STRING AS A MEASURED STRING IN DATA SPACE
 *
 * These two functions store an ANS Forth string into data space
 * as a measured string, using the specification of
 * p4_mstring_place().
 */

static p4_char_t *  /* returns initial aligned data address */ 
p4_mstring_comma (const p4_char_t *addr, size_t len)
{
  p4_char_t *p;

  p = (p4_char_t*) p4_aligned ((p4cell) DP);
  DP = p4_mstring_place (addr, len, p);
  return p;
}

static p4_char_t *
p4_parse_mstring_comma (char del)
{
  /* uses transient PFE.word. parse routines */
  p4_word_parse (del); *DP=0; /* PARSE-NOHERE */
  return p4_mstring_comma (PFE.word.ptr, (size_t) PFE.word.len);
}

/* CLEAR STRING SPACE
 *
 * Clear the string buffer, string stack, and string frame
 * stack.  Any string variables holding strings in the string
 * buffer are left pointing into limbo.  This may be executed
 * with the string space in an invalid state, as long as the
 * size and numframes fields of its StrSpace structure are
 * intact.
 */

void
p4_clear_str_space (StrSpace *space)
{
  size_t fstack_offset = ALIGNTO_CELL (sizeof (*space));
  size_t fstack_size = space->numframes * sizeof (StrFrame);

  space->fbreak = (void *) space + fstack_offset;
  space->fp0 = space->fp = (void *) space->fbreak + fstack_size;
  space->buf = space->sbreak = (void *)space->fp0;
  space->sp0 = space->sp = (void *) space->buf + space->size;
  space->cat_str = NULL;
  space->garbage_flag = space->garbage_lock = 0;
  *space->sp0 = NULL;	/* string stack underflow guard, not used */
}

/* MAKE A STRING SPACE
 *
 * Allocate and initialize a string space with a string buffer
 * including a string stack of prescribed size, and a prescribed
 * number of string stack frames.  The size is rounded up for
 * cell alignment, and the buffer begins and ends with that
 * alignment.
 *   
 * Return the address of the string space.
 */

StrSpace *
p4_make_str_space (size_t size, size_t frames)
{
  StrSpace *space;
  size_t fstack_offset = ALIGNTO_CELL (sizeof (*space));
  size_t fstack_size = frames * sizeof (StrFrame);

  size = ALIGNTO_CELL (size);
  space = (StrSpace *) p4_xalloc (size + PFE_SIZEOF_CELL
          + fstack_offset + fstack_size);
  if (space == NULL)
    p4_throw (P4_ON_OUT_OF_MEMORY);
  space->size = size;
  space->numframes = frames;
  p4_clear_str_space ( space );
  return space;
}

/* COLLECT GARBAGE FROM STRING SPACE
 *
 * The garbage flag in the current string space structure is
 * tested, and if there is garbage, it is collected, unless
 * garbage collection is locked.  Garbage strings are marked by
 * null backward links.  Nongarbage strings are bound by their
 * backward links, pointing either to a string variable data
 * field address or to an entry on the string stack (the deepest
 * if there are several identical references).
 *
 * Garbage collection fills the gaps occupied by garbage strings
 * by moving any nongarbage strings to lower memory one at a
 * time.  The backward link of a string that is moved does not
 * change, but the forward links, in at most one string variable
 * and/or possibly several string stack entries, are updated to
 * point to the new MSA.  This algorithm is "fast" because the
 * backward links make it unnecessary to scan a list of string
 * variables, and because no string is moved more than once.  It
 * does, however, require a scan of the string stack for each
 * string that moves, unless it is the current concatenation
 * string, which is guaranteed not to be on the string stack.
 *
 * When there is no garbage to collect, this routine returns 0.
 * If there is garbage, it throws an error when garbage
 * collection is locked.  Otherwise it collects the garbage and
 * returns 1.
 */

int
p4_collect_garbage (void)
{
  DStr *next, *target;
  MStr **sstack;
  char *p, *q, *limit;

  if ( !GARBAGE_FLAG ) 
    return 0;		/* no garbage to collect */

  if ( GARBAGE_LOCK ) p4_throw (P4_ON_SGARBAGE_LOCK);

  GARBAGE_FLAG = 0;
  next = SBUFFER;


  /* locate first garbage hole (no need to check off end,
     because we know there is a hole) */
  while ( next->backlink )
  {
    /* not garbage, skip to next */
    next = (DStr*)ALIGNTO_CELL(next + sizeof (next->backlink)
           + sizeof (next->count) + next->count);
  }
  target = next;
  
  /* not off end, garbage hole found */
  do
  {
    /* skip over garbage */
    while ( !next->backlink && next < SBREAK )
    {
      /* garbage and not off end, skip to next */
      next = (DStr*) ALIGNTO_CELL (next + sizeof (next->backlink)
	     + sizeof (next->count) + next->count);
    }

    /* BEGIN move and update until next garbage */
    while ( next->backlink && next < SBREAK )
    {
      /* not garbage and not off end */
      target->backlink = next->backlink;
      target->count = next->count;

      /* We always update the forward link pointed to by the
	 backward link.  The backward link points to one of:
	 a string variable, a deepest string stack entry, or
	 CAT$. */
	   
      *(next->backlink) = (MStr*) &(target->count);

      /* Unless the backward link points to CAT$, we scan the
         string stack and update copies of the old forward link.
         Note that if the backward link points into the string
         stack, we've already updated the deepest reference,
         which won't be found in the scan. */

      if ( next->backlink != (MStr**) &CAT_STR )
      {
        for ( sstack = SSP; sstack < SSP0; sstack++ )
	  if ( *sstack == (MStr*) &(next->count) )
	    *sstack = (MStr*) &(target->count);
      }

      /* move string, including null fill, to hole */
      q = (char*) &(target->body);
      p = (char*) &(next->body);
      limit = (char*) ALIGNTO_CELL (p + next->count);
      while ( p < limit )
        *q++ = *p++;
      next = (DStr*) p;
      target = (DStr*) q;
    }
    /* END move and update until next garbage */
  }
  while ( next < SBREAK );

  SBREAK = target; 
  return 1;
}

/* POP STRING AND MARK GARBAGE
 *
 * Throw an error if the string stack would underflow when
 * popped.
 *
 * Otherwise increment the string stack pointer, thus popping the
 * string stack.
 * 
 * If the old string is in the current string space and bound to
 * the old string stack position, set its back link to NULL and
 * set the garbage flag.
 */

MStr *
p4_pop_str (void)
{
  MStr **strsp = SSP;

  if (strsp == SSP0) p4_throw (P4_ON_SSTACK_UNDERFLOW);
  SSP += 1;

  if ( *strsp >= (MStr *) SBUFFER
       && *strsp < (MStr *) SBREAK
       && *((p4cell **) (*strsp)-1) == (p4cell *) strsp )
  {
    *((p4cell **) (*strsp)-1) = NULL;
    GARBAGE_FLAG = ~0;
  };

  return *strsp;
}

/* DROP ALL STRINGS AND FRAMES
 *
 * Clear the string stack, string frame stack, and any
 * concatenating string in a string space.  Dynamic strings held
 * in string variables remain.  This word is called by ABORT with
 * the current string space.  We decided not to do a garbage
 * collection here because it's used when there's an error, and
 * we might want to dump string space for debugging.
 */

void
p4_drop_all_strings (StrSpace *space)
{
  int depth = space->sp0 - space->sp;
  int i;

  space->fp = space->fp0;	/* drop string frames */
  if (space->cat_str)
  {
    *(p4ucell *)((size_t) space->cat_str - PFE_SIZEOF_CELL) = 0;
    space->cat_str = NULL;
  }

  for (i = 0; i < depth; i++)
    p4_pop_str ();
}

/* PUSH STRING WITH COPY INTO STRING SPACE
 *
 * See the word >$S-COPY for the specs.
 */

void
p4_push_str_copy (const p4_char_t *addr, size_t len)
{
  p4_char_t *buf;

  Q_CAT;

/* Required for Forth string copies, superfluous
   for mstring copies, insignificant overhead: */
#if MAX_MCOUNT < UCELL_MAX
  if (len > MAX_MCOUNT)
    p4_throw (P4_ON_DSCOUNT_OVERFLOW);
#endif

  Q_ROOM (SBREAK, len + SIZEOF_DSTR_HEADER + PFE_SIZEOF_CELL);

  /* Don't do earlier, maybe garbage was collected: */
  buf = (p4_char_t*) SBREAK;

  *P4_INC_(MStr**,buf) = (MStr*) --SSP;   /* back link */
  *SSP = (MStr*) buf;			/* forward link */
  *P4_INC_(MCount*,buf) = (MCount) len;
 
  while (len-- > 0)			/* copy string body */
    *buf++ = *addr++;
 
  { /* null fill */
    p4_char_t* bbuf = (p4_char_t*) ALIGNTO_CELL (buf);
    while (buf < bbuf) 
      *buf++ = 0;
  }
  SBREAK = (DStr*) buf;
}

/* FIND ARGUMENT IN STRING FRAME
 *
 * Search the top string frame for a match to an ANS Forth
 * string and return its index if found, else return -1.
 *
 * NOTE:  The index starts with 0 at the top of the string frame.
 * On the other hand, dynamic-strings words maintain the natural
 * left to right ordering for arguments in string stack comments.
 */

int
p4_find_arg (const p4_char_t *nm, int l)
{
  MStr **ssp = SFSP->top;
  MStr *p;
  int i;

  for (i = 0; i < SFSP->num; i++)
  {
    p = *(ssp++);
    if (l == MLEN (p) && p4_memcmp (nm, MADDR (p), l) == 0)  
      return i;
  }
  return -1;
}

/* MAKE STRING FRAME
 *
 * Define the top u items on the string stack as a string frame
 * by pushing u and the SSP onto the string frame stack.  Errors
 * are thrown if there is not enough room on the string frame
 * stack or if there are not at least u items on the top of the
 * string stack above any previous string frame.
 */

void
p4_make_str_frame (p4ucell u)
{
  register MStr **x;

  if (SFSP == SFBREAK)
    p4_throw (P4_ON_SFRAME_OVERFLOW);

  x = SFSP0 != SFSP
      ? SFSP->top : SSP0;
  if (u  > x - SSP)
    p4_throw (P4_ON_SSTACK_UNDERFLOW);

  SFSP -= 1 ;
  SFSP->top = SSP;
  SFSP->num = u;
}

/* COMPILE MACRO ARGUMENT 
 *
 * Search for a string in the top string stack frame.  If found,
 * compile run-time code that concatenates the corresponding
 * string in the top run-time string frame onto the current CAT$.
 *
 * This code imitates PFE code for compiling a local.  It is
 * intended for use in a modified INTERPRET.
 */

FCode_XE (p4_marg_execution)
{
  FX_USE_CODE_ADDR
  PUSH_STR ((MStr *) (SFSP->top)[(p4cell) *IP++]);
  FX (p4_cat);
}

/* Warning: this one P4COMPILES an marg_execution. To decompile
 * please ensure that a proper marg_SEE is defined upwards.
 */
int
p4_compile_marg (const p4_char_t *name, int len)
{
  int n;

  if ( (n = p4_find_arg (name, len)) == -1 )
    return 0;
  FX_COMPILE(p4_marg_execution);
  FX_UCOMMA (n);
  return 1;
}
static p4xcode*
p4_marg_SEE (p4xcode* ip, char* p, p4_Semant* s)
{
  sprintf (p, "<%c> ", 'A' + frame_size - 1 - (int)(p4cell) *ip);
  return ++ip;
}
P4COMPILES (p4_marg_execution, p4_marg_execution,
	    p4_marg_SEE, P4_DEFAULT_STYLE);


/************************************************************************/
/* constants								*/
/************************************************************************/

/** MAX-MCOUNT	( -- u )
 * Leave the largest mstring count provided by the
 * implementation.  Canonical values are 255 and 2^32-1.
 * <ansref>"max-m-count"</ansref>
 */
/*P4: OC(MAX_MCOUNT)*/

/** /MCOUNT	( -- u )
 * Leave the size of the mstring count field provided by the
 * implementation in ANS Forth character units, nominally bytes.
 * Canonical sizes are one character and one cell.
 * <ansref>"slash-m-count"</ansref>
 */
/*P4: OC(SIZEOF_MCOUNT)*/

struct p4_MStr p4_empty_str =
{ 
  0,		/* count */
  0		/* superfluous body */
};

struct p4_MStr p4_newline_str =
{ 
  1,		/* count */
  '\n'		/* body */
};

/** EMPTY$	( $: -- empty$ )
 * Push the MSA of a fixed, external representation of the empty
 * string onto the string stack.  <ansref>"empty-string"</ansref>
 */
FCode (p4_empty_str)
{
  PUSH_STR (&p4_empty_str);
}

/** \n$		( $: -- newline$ )
 * Push the MSA of a fixed, external string whose body is the
 * Unix newline character onto the string stack.
 * <ansref>"newline-string"</ansref>
 */
FCode (p4_newline_str)
{
  PUSH_STR (&p4_newline_str);
}


/************************************************************************/
/* variables								*/
/************************************************************************/

/** DSTRINGS	( -- dfa )
 * A Forth variable that holds the address of the current string
 * space, where all dynamic string operations take place. 
 * <ansref>"d-strings"</ansref>
 */
/*P4: DV(dstrings)*/


/************************************************************************/
/* ANS Forth string extensions						*/
/************************************************************************/

/** MPLACE	( a.str msa -- )
 * MPLACE is the same as Wil Baden's PLACE, except it assumes
 * the buffer address msa to be aligned, and stores the ANS
 * Forth string a.str as a measured string, zero-filled to
 * trailing alignment.  As with PLACE, it is assumed that the
 * mstring copy does not clobber the old string, and there is no
 * check for room starting at msa.  <ansref>"m-place"</ansref>
 */
FCode (p4_m_place)
{
  p4_mstring_place ( (p4_char_t *) SP[2], SP[1], (p4_char_t *) SP[0] );
  SP += 3;
}

/** PARSE>S	( [ccc<char>] char -- addr len )
 * Parse the input stream up to the first occurrence of char,
 * which is parsed away.  If executing in compilation mode,
 * append run-time semantics to the current definition that
 * leaves the ANS Forth string representation on the stack.  In
 * interpretation mode, leave the ANS Forth string
 * representation for a stored copy, which may be transient in

 * the style of =>"S"".  In either mode, the format of the
 * stored string is implementation dependent.
 * <ansref>"parse-to-s"</ansref>

 * NOTE: The interpreted copy is a nontransient in this
 * implementation, and both copies are mstrings.
 */
FCode (p4_parse_to_s)
{
  if (STATE)
  {
    FX_COMPILE (p4_parse_to_s);
    p4_parse_mstring_comma ((p4char) *SP++);
  }
  else
  {
    MStr* mstr = (MStr *) p4_parse_mstring_comma ((p4char) *SP);

    *--SP = (p4cell) (*mstr).count;
    SP[1] = (p4cell) &((*mstr).body);
  }
}
FCode_XE (p4_parse_to_s_execution)
{
  FX_USE_CODE_ADDR
  *--SP = (p4cell) IP + SIZEOF_MCOUNT;
  *--SP = (p4cell) (*(MStr *) IP).count;
  FX_SKIP_MSTRING;
}
P4COMPILES (p4_parse_to_s, p4_parse_to_s_execution,
	    P4_SKIPS_NOTHING, P4_DEFAULT_STYLE);

/** S`		( [ccc<`>] -- addr len )
 * An immediate version of parse>s where the delimiter is `.  In
 * particular, the stored string in interpret mode is not
 * transient. <ansref>"s-back-tick"</ansref>
 */
FCode (p4_s_back_tick)
{
  if (STATE)
  {
    FX_COMPILE (p4_s_back_tick);
    p4_parse_mstring_comma ('`');
  }
  else
  {
    MStr* mstr = (MStr *) p4_parse_mstring_comma ('`');

    *--SP = (p4cell) &((*mstr).body);
    *--SP = (p4cell) (*mstr).count; 
  }
}
FCode_XE (p4_s_back_tick_execution)
{
  FX_USE_CODE_ADDR
  *--SP = (p4cell) IP + SIZEOF_MCOUNT;
  *--SP = (p4cell) (*(MStr *) IP).count;
  FX_SKIP_MSTRING;
}
static p4xcode* /* P4_SKIPS_MSTRING */
p4_back_tick_mstring_SEE (p4xcode* ip, char* p, p4_Semant* s)
{
  sprintf (p, "%.*s %.*s` ",
           NFACNT(*s->name), s->name + 1,
           (int) *(p4_MCount *) ip,
           (p4char *) ip + sizeof(p4_MCount));
  P4_SKIP_MSTRING (ip);
  return ip;
}
P4COMPILES (p4_s_back_tick, p4_s_back_tick_execution,
	    p4_back_tick_mstring_SEE, P4_DEFAULT_STYLE);

/** SM,		( addr len -- addr' len )

 * ALLOT room and store the ANS Forth string into aligned data
 * space as an mstring, leaving data space zero-filled to
 * alignment; and leave the length and new body address.  It is
 * assumed that len is unsigned.  An error is thrown if len is
 * larger than the system parameter MAX_DATA_STR.
 * <ansref>"s-m-comma"</ansref>

 * NOTE: MAX_DATA_STR is returned by 
   S" /SCOPY" ENVIRONMENT?

 * NOTE: SM, differs from STRING, in Wil Baden's Tool Belt in
 * that it stores an aligned, measured string with zero-filled
 * alignment instead of a counted string, and it leaves the
 * ANS Forth string representation of the stored string.
 */
FCode (p4_s_m_comma)
{
  SP[1] = (p4cell) (p4_mstring_comma ((p4_char_t*) SP[1],
                    (size_t) *SP) + SIZEOF_MCOUNT);
}


/************************************************************************/
/* measured strings							*/
/************************************************************************/

/** The words in this section are intended mainly for under the
  * hood development.
  */

/** MCOUNT@	( msa -- count )
 * Fetch the count of the mstring at msa.
 * <ansref>"m-count-fetch"</ansref>
 */
FCode (p4_m_count_fetch)
{
  *SP = (p4cell) (**(MStr **) SP).count ;
}

/** MCOUNT!	( count msa -- )
 * Store the count in the measured string count field at msa,
 * without checking that it fits. 
 * <ansref>"m-count-store"</ansref>
 */
FCode (p4_m_count_store)
{
  (**(MStr **) SP).count = (MCount) SP[1];
  SP += 2;
}

/** MCOUNT	( msa -- body.addr count )
 * Convert the mstring MSA to its ANS Forth string
 * representation.  <ansref>"m-count"</ansref>
 */
FCode (p4_m_count)
{
  MStr *p = *(MStr **) SP;

  *SP = *SP + SIZEOF_MCOUNT;
  *--SP = (p4cell) (*p).count;
}

/** -MCOUNT	( addr len -- msa )
 * Convert the ANS Forth representation of an mstring to its
 * MSA.  <ansref>"minus-m-count"</ansref>
 */
FCode (p4_minus_m_count)
{
  SP[1] = SP[1] - SIZEOF_MCOUNT;
  SP++;
}


/************************************************************************/
/* string space								*/
/************************************************************************/

/** 0STRINGS	( -- )
 * Set all string variables holding bound string values in string
 * space to the empty string, and clear string space, including
 * the string buffer, string stack, and string stack frames. 
 * <ansref>"zero-strings"</ansref>

 * NOTE:  If used for under the hood development, this word must
 * be executed only when string space is in a valid state.
 */
FCode (p4_zero_strings)
{
  DStr *next = SBUFFER;

  while (next < SBREAK)
  {   
    if (next->backlink)
      *(next->backlink) = &p4_empty_str;
      next = (DStr*) ALIGNTO_CELL ((size_t) next
	                           + offsetof (DStr, body)
	                           + next->count);
  }
  p4_clear_str_space (DSTRINGS);
} 

/** $GARBAGE?	( -- flag )
 * Leave true if there is garbage in the current string space. 
 * Not normally used, since garbage collection is transparent.
 * <ansref>"string-garbage-question"</ansref>
 */
FCode (p4_str_garbage_Q)
{
  *--SP = GARBAGE_FLAG;
}

/** $GC-OFF	( -- )
 * Disable garbage collection in the current string space.  An
 * error will be thrown if garbage collection is attempted.
 * <ansref>"string-g-c-off"</ansref>
*/
FCode (p4_str_gc_off)
{
  GARBAGE_LOCK = ~0;
}

/** $GC-ON	( -- )
 * Enable garbage collection in the current string space.  This
 * is the default.  <ansref>"string-g-c-on"</ansref>
 */
FCode (p4_str_gc_on)
{
  GARBAGE_LOCK = 0;
}

/** $UNUSED	( -- u )
 * Leave the number of bytes available for dynamic strings and
 * string stack entries in the string buffer. 
 * <ansref>"string-unused"</ansref>
 */
FCode (p4_str_unused)
{
  *--SP = (p4cell) SSP - (p4cell) SBREAK; 
}

/** COLLECT-$GARBAGE	( -- collected-flag )
 * If string space is not marked as containing garbage, return
 * false.  If there is garbage, throw an error when garbage
 * collection is disabled.  Otherwise remove the garbage and
 * return true.  Garbage collection is "transparent", so the
 * user would not normally use this word.
 * <ansref>"collect-string-garbage"</ansref>
 */
FCode (p4_collect_str_garbage)
{
  p4_collect_garbage () ? (*--SP = ~0) : (*--SP = 0);
}

/** MAKE-$SPACE		( size #frames -- addr )
 * Allocate and initialize a string space with size bytes
 * available for the string buffer including the string stack,
 * and with a string frame stack for frame description entries
 * holding up to #frames.  The size is rounded up to cell
 * alignment, and the buffer begins and ends with cell alignment.
 * Return addr, the address of the string space.  The standard
 * word FREE with addr as input can be used to release the space.
 * <ansref>"make-string-space"</ansref>
 */
FCode (p4_make_str_space)
{
  SP[1] = (p4cell) p4_make_str_space (SP[1], SP[0]);
  SP += 1;
}

/** /$BUF	( -- u )
 * Leave the size in address units allocated for the current
 * string buffer. <ansref>"slash-string-buf"</ansref>
 */
FCode (p4_slash_str_buf)
{
  *--SP = DSTRINGS->size;
}

/** MAX-#$FRAMES	( -- u )
 * Leave the number of string frames allowed on the string frame
 * stack for the current string space.
 * <ansref>"max-number-string-frames"</ansref>
 */
FCode (p4_max_num_str_frames)
{
  *--SP = DSTRINGS->numframes;
}


/************************************************************************/
/* load and store							*/
/************************************************************************/

/** $!		( $var.dfa $: a$ -- )
 * Store the string MSA on the string stack in the variable
 * whose DFA is on the parameter stack. 
 * <ansref>"string-store"</ansref>

 * NOTES: The only situation in which =>"$!" copies the string
 * value is when it is a bound string already stored in another
 * variable.  In that case, the new copy is the one that is
 * stored in the variable.  In particular, external strings are
 * not copied.

 * If the string value held by the string variable on entry is a
 * bound string that is also referenced deeper on the string
 * stack, its back link is reset to point to the deepest string
 * stack reference.  If it is a bound string not deeper on the
 * string stack and not identical to the input string, its back
 * link is set to zero, making it garbage.  If it is an external
 * string, its MSA in the variable is simply written over by
 * that popped from the string stack.
 */
FCode (p4_str_store)
{
  MStr **addr, **strsp, *oldstr, *newstr;
  int oldext, newext;	/* true if old/new strings external */ 
  char **backlink;
  char *next;
  size_t len;
  char *buf;

  addr = (MStr **) *SP++;
  oldstr = *addr;
  oldext = (oldstr < (MStr *) SBUFFER || oldstr >= (MStr *) SBREAK);

  strsp = SSP;	/* not ready to pop if copy causes garbage collection */
  if (strsp == SSP0)
    p4_throw (P4_ON_SSTACK_UNDERFLOW);
  newstr = *strsp;
  newext = (newstr < (MStr *) SBUFFER || newstr >= (MStr *) SBREAK);

  if ( !(oldext && newext) && !(!oldext && newstr == oldstr) )
  {
    /* We know the new string is not bound to our variable.
       Do old string first; it might open a garbage hole, more
       room in case new string is copied. */
    if (!oldext)
    {
      backlink = (char **)((size_t) oldstr - PFE_SIZEOF_CELL);
      next = NULL;		/* garbage if not on stack */
      /* no need to check first stack item */
      while (++strsp < SSP0)
        if (*strsp == oldstr) next = (char*) strsp;
      *backlink = next;
      if (!next)
        GARBAGE_FLAG = ~0;
    }

    if (!newext)
    {
      backlink = (char **) ((size_t) newstr - PFE_SIZEOF_CELL); 
      if (*backlink < (char*) SSP || *backlink >= (char*) SSP0)
      {
	/* New string is bound to a different variable, copy it. */

	Q_CAT;
	len = newstr->count;
	Q_ROOM (SBREAK, len + SIZEOF_DSTR_HEADER);
	/* garbage possibly collected */

	backlink = (char**) SBREAK;
	SBREAK->count = len;

	buf = &(SBREAK->body);
	newstr = (MStr*)( (char*) *SSP + SIZEOF_MCOUNT );

	while (len-- > 0)	/* copy string body */
	*buf++ = *P4_INC_(char*,newstr);
 
	newstr = (MStr*)( ALIGNTO_CELL (buf) ); /* null fill*/
	while (buf < (char*) newstr) 
	*buf++ = 0;

	SBREAK = (DStr*) buf;
	newstr = (MStr*)( (char*) backlink + offsetof (DStr, count) );
      }
      *backlink = (char*) addr;
    }
  }
  *addr = newstr;
  SSP += 1;	/* now I tin pop */
}

/** $@		( $var.pfa -- $: a$ )
 * Leave the MSA of the string held by the string variable.
 * <ansref>"string-fetch"</ansref>
 */
FCode (p4_str_fetch)
{
  PUSH_STR ((MStr*) *(char**) (*SP++));
}

/** $"		( [ccc<">] -- $: str )
 * Parse ccc delimited by " (double quote) and store it in data
 * space as an mstring.  If interpreting, leave the MSA on the
 * string stack.  If compiling, append run-time semantics to the
 * current definition that leaves the MSA on the string stack. 
 * A program should not alter the stored string.  An error is
 * thrown if the quoted string length is larger than the system
 * parameter MAX_DATA_STR (see =>"SM,").
 * <ansref>"string-quote"</ansref>

 * NOTE:  In contrast to S", the string stored by =>'$"' when
 * interpreting is not transient.

 * The implementation is based on PFE code for =>'S"'.
 */
FCode (p4_str_quote)
{
  if (STATE)
  {
    FX_COMPILE (p4_str_quote);
    p4_parse_mstring_comma ('"');
  }
  else
  {
    PUSH_STR ((MStr *) p4_parse_mstring_comma ('"'));
  }
}
FCode_XE (p4_str_quote_execution)
{
  FX_USE_CODE_ADDR
  PUSH_STR ((MStr *) IP);
  FX_SKIP_MSTRING;
}
static p4xcode* /* P4_SKIPS_MSTRING */
p4_quote_mstring_SEE (p4xcode* ip, char* p, p4_Semant* s)
{
  sprintf (p, "%.*s %.*s\" ",
           NFACNT(*s->name),
           s->name + 1,
           (int) *(p4_MCount *) ip,
           (p4char *) ip + sizeof(p4_MCount));
  P4_SKIP_MSTRING (ip);
  return ip;
}
P4COMPILES (p4_str_quote, p4_str_quote_execution,
	    p4_quote_mstring_SEE, P4_DEFAULT_STYLE);

/** $`		( [ccc<`>] -- $: str )
 * Parse ccc delimited by ` (back-tick).  This is => "$"" with
 * back tick instead of double quote as the delimiter.
 * <ansref>"string-quote"</ansref>
 */
FCode (p4_str_back_tick)
{
  if (STATE)
  {
    FX_COMPILE (p4_str_back_tick);
    p4_parse_mstring_comma ('`');
  }
  else
  {
    PUSH_STR ((MStr *) p4_parse_mstring_comma ('`'));
  }
}
P4COMPILES (p4_str_back_tick, p4_str_quote_execution,
	    p4_back_tick_mstring_SEE, P4_DEFAULT_STYLE);

/** $CONSTANT		( "name" $: a$ -- )
 * Create a definition for "name" with the execution semantics
 * "name" execution:	($: -- a$ )

 * It is assumed that the input string resides as a measured,
 * unchanging string outside of string space.
 * <ansref>"string-constant"</ansref>

 * For example:
   $" This is a sample string." $constant sample$
 */
FCode (p4_str_constant)
{
  FX_RUNTIME_HEADER;
  FX_RUNTIME1 (p4_str_constant);
  FX_PCOMMA (p4_pop_str ());
}
FCode_RT (p4_str_constant_RT)
{
  FX_USE_BODY_ADDR
  PUSH_STR ((MStr *) FX_POP_BODY_ADDR[0]);
}
P4RUNTIME1(p4_str_constant, p4_str_constant_RT);

/** $VARIABLE		( "name" -- )
   "name" execution:	( -- dfa )

 * Create an ordinary Forth variable and initialize it to the
 * address of a fixed, external, measured representation of the
 * empty string, such as that pushed onto the string stack by
 * =>"EMPTY$".  <ansref>"string-variable"</ansref>"
 */
FCode (p4_str_variable)
{
  FX_RUNTIME_HEADER;
  FX_RUNTIME1 (p4_variable);
  FX_PCOMMA (&p4_empty_str);
}

/** PARSE>$	( [ccc<char>] char -- $: ccc$ )
 * Parse the input stream up to the first occurrence of char,
 * which is parsed away, and store the string as an external
 * measured string.  If executing in compilation mode, append
 * run-time semantics to the current definition that leaves the
 * MSA on the string stack.  In interpretation mode, leave the
 * MSA on the string stack, where the stored copy, unlike
 * =>"PARSE>S", is required to be nontransient.
 */
FCode (p4_parse_to_str)
{
  if (STATE)
  {
    FX_COMPILE (p4_parse_to_str);
    p4_parse_mstring_comma ((p4char) *SP++);
  }
  else
  {
    PUSH_STR ( (MStr *) p4_parse_mstring_comma ((p4char) *SP++) );
  }
}
FCode_XE (p4_parse_to_str_execution)
{
  FX_USE_CODE_ADDR
  PUSH_STR ((MStr *) IP);
  FX_SKIP_MSTRING;
}
P4COMPILES (p4_parse_to_str, p4_parse_to_str_execution,
	    P4_SKIPS_NOTHING, P4_DEFAULT_STYLE);


/************************************************************************/
/* comments								*/
/************************************************************************/

/** ($:		( "ccc<paren>" -- )
 * A synonym for =>"(".  Immediate. 
 * <ansref>"paren-string-colon"</ansref>
 */

/* EXIT helper called by a semicolon in the sources */
static void p4_margs_EXIT(P4_VOID)
{
  extern FCode (p4_do_drop_str_frame);
  FX (p4_do_drop_str_frame);
  p4_Q_pairs (P4_MARGS_MAGIC);
  
  { 
    register p4code semicolon_code = (p4code) FX_POP; 
    semicolon_code(FX_VOID); /* pushed in p4_args_brace */
  }
}


/************************************************************************/
/* string stack								*/
/************************************************************************/

/** $.		( $: a$ -- )
 * Display the string on the terminal.  If the system
 * implementation of TYPE has its output vectored, $. uses the
 * same vector. <ansref>"string-dot"</ansref>
 */
FCode (p4_str_dot)
{
  MStr *str = p4_pop_str ();

  p4_type ((p4_char_t*) MADDR (str), MLEN (str));
}

/** $2DROP	( $: a$ b$ --  )
 * Drop the two topmost string stack entries, marking them as
 * garbage if appropriate.  <ansref>"string-two-drop"</ansref>
 */
FCode (p4_str_two_drop)
{
  p4_pop_str (); p4_pop_str ();
}

/** $2DUP	( $: a$ b$ -- a$ b$ a$ b$ )
 * Leave copies of the two topmost string stack entries.  The string
 * values are not copied.  <ansref>"string-two-dupe"</ansref>
 */
FCode (p4_str_two_dup)
{
  if (SSP0 - SSP < 2)
    p4_throw (P4_ON_SSTACK_UNDERFLOW);
  P4_Q_ROOM (SBREAK, PFE_SIZEOF_CELL * 2);
  SSP -= 2;
  SSP[0] = SSP[2];  SSP[1] = SSP[3];
}

/** $DEPTH	( -- n )
 * Leave the number of items on the string stack.
 * <ansref>"string-depth"</ansref>
 */
FCode (p4_str_depth)
{
  *--SP = (SSP0 - SSP);
}

/** $DROP	( $:  a$ -- )
 * Drop the topmost string stack entry, marking it as garbage if
 * it is initially bound to the top of the string stack.
 * <ansref>"string-drop"</ansref>
 */
FCode(p4_str_drop)
{
  p4_pop_str ();
}

/** $DUP	( $: a$ -- a$ a$ )
 * Leave a copy of the topmost string stack entry.  The string
 * value is not copied.  <ansref>"string-dupe"</ansref>
 */
FCode (p4_str_dup)
{
  MStr **strsp;

  strsp = SSP;
  if (strsp == SSP0)
    p4_throw (P4_ON_SSTACK_UNDERFLOW);
  PUSH_STR (*strsp);
}

/** $NIP	($: a$ b$ -- b$ )
 * Drop the next to top item from the string stack.
 * <ansref>"string-nip"</ansref>

 * NOTE:  Because of essential string  space bookkeeping, the
 * system level implementation can be little more efficient than
 * the high-level definition:
   : $NIP  $SWAP $DROP ;
 */
FCode (p4_str_nip)
{
  FX (p4_str_swap);  FX (p4_str_drop);
}

/** $OVER	( $: a$ b$ -- a$ b$ a$ )
 * Leave a copy of the next most accessible string stack entry
 * on top of the string stack.  The string value is not copied.
 * <ansref>"string-over"</ansref>
 */
FCode (p4_str_over)
{
  if (SSP0 - SSP < 2)
    p4_throw (P4_ON_SSTACK_UNDERFLOW);
  PUSH_STR (SSP[2]);
}

/** $PICK	( u $: au$ ... a0$ -- au$ ... a0$ au$ )
 * Copy the u-th string stack entry to the top of the string
 * stack.  The string value is not copied.  Throw an error if
 * the input string stack does not have at least u+1 items. 
 * <ansref>"string-pick"</ansref>
 */
FCode (p4_str_pick)
{
  p4ucell u = *SP++;

  if (SSP0 - SSP < u + 1) 
    p4_throw (P4_ON_SSTACK_UNDERFLOW);
  PUSH_STR (SSP[u + 1]);
}

/** $SWAP	( $: a$ b$ -- b$ a$ )
 * Exchange the two most accessible strings on the string stack.
 * Throw an error if there are less than two strings on the
 * stack.  Neither string value is copied.
 * <ansref>"string-swap"</ansref>
*/
FCode (p4_str_swap)
{
  MStr *str1, *str2;
  char **blp;	/* back link pointer */

  if ( (SSP0 - SSP) < 2 )
    p4_throw (P4_ON_SSTACK_UNDERFLOW);
  str1 = SSP[1];
  str2 = SSP[0];

  if ( !(str1 == str2) )
  {
    SSP[0] = str1;
    SSP[1] = str2;

    if ( str1 >= (MStr *) SBUFFER && str1 < (MStr *) SBREAK )
    {
      blp = (char **) ((size_t) str1 - PFE_SIZEOF_CELL);
      if ( *blp == (char *) (&SSP[1]) )
	*blp = (char *) SSP;
    }

    if ( str2 >= (MStr *) SBUFFER && str2 < (MStr *) SBREAK )
    {
      blp = (char **) ( (size_t) str2 - PFE_SIZEOF_CELL );
      if ( *blp == (char *) SSP )
	*blp = (char *) &SSP[1];
    }
  }
}

/** $S>		( $: a$ -- S: a.str )
 * Drop a$ from the string stack and leave it as a ANS Forth
 * string a.str, without copying.
 * <ansref>"string-s-from"</ansref>

 * WARNING:  If a$ is a bound string, it may move or disappear
 * at the next garbage collection, making a.str invalid.  This
 * can be avoided by sandwiching sections of code where this
 * could occur between $GC-OFF and $GC-ON.
 */
FCode (p4_str_s_from)
{
  MStr *str = p4_pop_str ();

  *--SP = (p4cell) str + SIZEOF_MCOUNT;
  *--SP = str->count;
}

/** $S>-COPY	( $: a$ -- S: a.str )
 * Drop a$ from the string stack, copy it into data space as a
 * measured string, and leave it as an ANS Forth string a.str.
 * An error is thrown if the string length is larger than the
 * system parameter MAX_DATA_STR (see =>"S,").
 * <ansref>"string-s-from-copy"</ansref>
 */
FCode (p4_str_s_from_copy)
{
  MStr *str = p4_pop_str ();
  MStr *p = (MStr *) p4_mstring_comma ((p4_char_t*) MADDR (str), MLEN (str));

  *--SP = (p4cell) MADDR (p);
  *--SP = MLEN (p);
}

/** $S@ 	( $: a$ -- a$ S: a.str )
 * Leave the string stack unchanged, and leave the string body
 * address and length on the data stack. 
 * <ansref>"string-s-fetch"</ansref>

 * WARNING:  If a$ is a bound string, it may move at the next
 * garbage collection, making a.str invalid.  This can be
 * avoided by sandwiching sections of code where this could
 * occur between $GC-OFF and $GC-ON.
 */
FCode (p4_str_s_fetch)
{
  if (SSP == SSP0)
    p4_throw (P4_ON_SSTACK_UNDERFLOW);
  *--SP = (p4cell) MADDR (*SSP);
  *--SP = MLEN (*SSP);
}

/** $TUCK	($: a$ b$ -- b$ a$ b$ )
 * Copy the top string stack item just below the second item.  The
 * string value is not copied.  <ansref>"string-tuck"</ansref>

 * NOTE:  Because of essential string  space bookkeeping, the
 * system level implementation can be little more efficient than
 * the high-level definition:
   : $TUCK  $SWAP $OVER ;
 */
FCode (p4_str_tuck)
{
  FX (p4_str_swap);  FX (p4_str_over);
}

/** $TYPE	($: a$ -- )
 * Display the string on the terminal.  A deprecated =>'$.'
 * synonym.  <ansref>"string-type"</ansref>
 */

/** >$S		( a.str -- $: a$ )

 * Push the external ANS Forth string a.str onto the string
 * stack, without copying the string value into the string
 * buffer.  It is an unchecked error if the Forth string a.str
 * is not stored as an external measured string.
 * <ansref>"to-string-s"</ansref>

 * WARNING: If the string value of a.str is actually in the
 * string buffer and not external, the push operation may
 * generate a garbage collection that invalidates its MSA.
 */
FCode (p4_to_str_s)
{
  SP += 1;		/* drop length */
  PUSH_STR ((void*) *SP++ - SIZEOF_MCOUNT);
}

/** >$S-COPY	( a.str -- $: a$ )
 * Copy the external string value whose body address and count
 * are on the parameter stack into the string buffer and push it
 * onto the string stack.  Errors are thrown if the count is
 * larger than MAX_MCOUNT, if there is not enough room in string
 * space, even after garbage collection, or if there is an
 * unterminated string concatenation.  The input external string
 * need not exist as a measured string. 
 * <ansref>"to-string-s-copy"</ansref>

 * NOTE:  MAX_MCOUNT is the largest size the count field of a
 * measured string can hold, e.g., 255, 64K-1, or 4,096M-1.  It
 * is returned by: S" /DYNAMIC-STRING" ENVIRONMENT?

 * WARNING: This word should not be used when the input string
 * is a bound string because the copy operation may generate a
 * garbage collection which invalidates its MSA.
 */
FCode (p4_to_str_s_copy)
{
  p4_push_str_copy ((p4_char_t *) SP[1], SP[0]);
  SP += 2;
}


/************************************************************************/
/* concatenation							*/
/************************************************************************/

/** CAT		($: a$ -- )
 * Append the string body to the end of the string currently
 * being concatenated as the last string in the string buffer,
 * and update its count field.  If there is no concatenating
 * string, start one.  An error is thrown if the size of the
 * combined string would be larger than MAX_MCOUNT or if there
 * is not enough room in string space even after a garbage
 * collection.

 * If garbage collection occurs, a$ remains valid even when
 * it is in the string buffer.
 
 * When there is a concatenating string, concatenation is the
 * only basic string operation that can copy a string into the
 * string buffer.  <ansref>"cat"</ansref>
 */
FCode (p4_cat)
{
  char *p,*q;
  size_t delta = *(MCount*) *SSP;

  if (SSP == SSP0)
    p4_throw (P4_ON_SSTACK_UNDERFLOW);

  if (!CAT_STR)		/* copy first string */
  {
    Q_ROOM (SBREAK, delta + SIZEOF_DSTR_HEADER - PFE_SIZEOF_CELL);
    p = (char*) P4_PTR_(MStr*, p4_pop_str ());	/* pop ok after gc */
    P4_INC_(MCount*,p);
    q = (char*) P4_PTR_(DStr*, SBREAK);
    *P4_INC_(MStr**,q) = (MStr *)( &CAT_STR );	/* back link */
    CAT_STR = (MStr *) q;			/* forward link */
    *P4_INC_(MCount*,q) = delta;
    while (delta-- > 0)
      *q++ = *p++;
  }
  else			/* append next string */
  {
    size_t len = MLEN (CAT_STR);
    size_t newlen = len + delta;

#if MAX_MCOUNT < UCELL_MAX
    if (newlen > MAX_MCOUNT)
      p4_throw (P4_ON_DSCOUNT_OVERFLOW);
#endif
    Q_ROOM (CAT_STR, SIZEOF_MCOUNT + newlen - PFE_SIZEOF_CELL);
    p = (char*) P4_PTR_(MStr*, p4_pop_str ());	/* pop ok after gc */
    P4_INC_(MCount*,p);
    q = (char *) CAT_STR;
    *(MCount *) q = newlen;
    q += len + SIZEOF_MCOUNT;
    while (delta-- > 0)
      *q++ = *p++;
  }

  { /* null fill */
    p = (char*) ALIGNTO_CELL (q);
    while (q < p)
      *q++ = 0;
  }
  SBREAK = (DStr *) q;
}

/** S-CAT	( a.str -- )
 * Append the ANS Forth string body to the end of the string
 * currently being concatenated as the last string in the string
 * buffer, and update its count field.  If there is no
 * concatenating string, start one.  An error is thrown if the
 * size of the combined string would be larger than MAX_MCOUNT
 * or if there is not enough room in string space even after a
 * garbage collection.

 * S-CAT is most commonly used on external strings, not assumed
 * to exist as mstrings.  In contrast to =>"CAT", garbage
 * collection could invalidate a.str if it is a dynamic string
 * in the string buffer.  S-CAT can be used in that situation if
 * garbage collection is turned off with =>"$GC-OFF".
 
 * When there is a concatenating string, concatenation is the
 * only basic string operation that can copy a string into the
 * string buffer.  <ansref>"s-cat"</ansref>
 */
static void
p4_s_cat (const p4_char_t *p, size_t delta )
{
  p4_char_t *q;

  if (!CAT_STR)		/* copy first string */
  {
#if MAX_MCOUNT < UCELL_MAX
    if (delta > MAX_MCOUNT)
      p4_throw (P4_ON_DSCOUNT_OVERFLOW);
#endif
    Q_ROOM (SBREAK, delta + SIZEOF_DSTR_HEADER);
    q = (p4_char_t*)(DStr*) SBREAK;                    /* !!! */
    *P4_INC_(MStr**,q) = (MStr *) &CAT_STR;	/* back link */
    CAT_STR = (MStr *) q;			/* forward link */
    *P4_INC_(MCount*,q) = delta;
    while (delta-- > 0)
      *q++ = *p++;
  }
  else			/* append next string */
  {
    size_t len = MLEN (CAT_STR);
    size_t newlen = len + delta;
      
#if MAX_MCOUNT < UCELL_MAX
    if (newlen > MAX_MCOUNT)
      p4_throw (P4_ON_DSCOUNT_OVERFLOW);
#endif
    Q_ROOM (CAT_STR, SIZEOF_MCOUNT + newlen);
    q = (p4_char_t *) CAT_STR;
    *(MCount *) q = newlen;
    q += len + SIZEOF_MCOUNT;
    while (delta-- > 0)
      *q++ = *p++;
  }

  { /* null fill */
    p4_char_t* qq = (p4_char_t*) ALIGNTO_CELL (q);
    while (q < qq) { *q++ = 0; }
  }
  SBREAK = (DStr *) q;
}

FCode (p4_s_cat)
{
  p4_s_cat ((p4_char_t *) SP[1], SP[0]);
  SP += 2;
}

/** PARSE-CAT	( [ccc<char>] char -- )
 * Parse the input stream up to the first occurrence of char,
 * which is parsed away.  If executing in compilation mode,
 * append run-time semantics to the current definition that
 * concatenates the characters parsed from the string. 
 * Otherwise concatenate the characters. 
 * <ansref>"parse-cat"</ansref>
 */
FCode (p4_parse_cat)
{
  if (STATE)
  {
    FX_COMPILE (p4_parse_cat);
    p4_parse_mstring_comma ((p4char) *SP++);
    }
  else
  {
    p4_word_parse ((char) *SP++);
    p4_s_cat (PFE.word.ptr, PFE.word.len);
  }
}
FCode_XE (p4_parse_cat_execution)
{
  FX_USE_CODE_ADDR
#if 0
  *--SP = (p4cell) IP + SIZEOF_MCOUNT;
  *--SP = (p4cell) (*(MStr *) IP).count;
  FX (p4_s_cat);
#endif
  p4_s_cat ((p4_char_t *) ( (p4cell) IP + SIZEOF_MCOUNT ),
            (size_t) (*(MStr *) IP).count );
  FX_SKIP_MSTRING;
}
P4COMPILES (p4_parse_cat, p4_parse_cat_execution,
	    P4_SKIPS_NOTHING, P4_DEFAULT_STYLE);

/** ENDCAT	( -- $: cat$ | empty$ )
 * If there is no concatenating string, do nothing but leave the
 * empty string.  If there is, leave it as a string bound to the
 * top of the string stack, and terminate concatenation,
 * permitting normal copies into the string buffer. 
 * <ansref>"end-cat"</ansref>
 */
FCode (p4_endcat)
{
  if (CAT_STR != NULL)
  {
    PUSH_STR (CAT_STR);
    *(char **) ((p4cell *)CAT_STR - 1) = (char *) SSP; 
    CAT_STR = NULL;
  }
  else
  {
    PUSH_STR (&p4_empty_str);
  }
}

/** CAT"	( [ccc<quote>] -- )
 * This word is immediate.  In compilation mode it appends
 * run-time semantics to the current definition that
 * concatenates the quoted string according to the specification
 * for =>"CAT".  In interpretation mode it concatenates the
 * string.  An error is thrown if the length of the quoted
 * string is longer than the system parameter MAX_DATA_STR (see
 * =>"S,").  <ansref>"cat-quote"</ansref>
 */
FCode (p4_cat_quote)
{
  if (STATE)
  { 
    FX_COMPILE (p4_cat_quote);
    p4_parse_mstring_comma ('"');
  }
  else
  {
    p4_word_parse ('"');
    p4_s_cat (PFE.word.ptr, PFE.word.len);
  }
}
FCode_XE (p4_cat_quote_execution)
{
  FX_USE_CODE_ADDR
  PUSH_STR ((MStr *) IP);
  FX_SKIP_MSTRING;
  FX (p4_cat);
}
P4COMPILES (p4_cat_quote, p4_cat_quote_execution,
            p4_quote_mstring_SEE, P4_DEFAULT_STYLE);

/** CAT`	( [ccc<backtick>] -- )
 * The same as =>'CAT"' but with back tick instead of double
 * quote as delimiter.  <ansref>"cat-back-tick"</ansref>
 */
FCode (p4_cat_back_tick)
{
  if (STATE)
  { 
    FX_COMPILE (p4_cat_back_tick);
    p4_parse_mstring_comma ('`');
  }
  else
  {
    p4_word_parse ('`');
    p4_s_cat (PFE.word.ptr, PFE.word.len);
  }
}
P4COMPILES (p4_cat_back_tick, p4_cat_quote_execution,
	    p4_back_tick_mstring_SEE, P4_DEFAULT_STYLE);


/************************************************************************/
/* arguments								*/
/************************************************************************/

/** #$ARGS	( -- u )
 * Leave the number of entries in the topmost string frame.
 * Throw an error if the frame stack is empty.
 * <ansref>"number-string-args"</ansref>
 */
FCode (p4_num_str_args)
{
  if (SFSP == SFSP0)
    p4_throw (P4_ON_SFRAME_UNDERFLOW);
  *--SP = (*SFSP).num;
}

/** $ARGS{	 ( arg1'$ ... argN'$ "arg1 ... argN <}>" --  )
    compilation: ( -- $: arg1$ ... argN$ )
 * Immediate and compilation-only.

 * Copy the argument strings across lines to the string buffer,
 * push them onto the string stack with "argN" the most
 * accessible, and make them into the top compile-time string
 * stack frame.  Compile the run-time code to make an argument
 * frame out of the N most accessible run-time string stack
 * entries.  Inform the system text interpreter that it should
 * compile run-time code for any white-space delimited argument
 * encountered in the text of the definition, that concatenates
 * the corresponding string in the run-time frame.  At the
 * semicolon terminating the definition, drop the compile-time
 * argument frame and compile code to drop the run-time argument
 * frame.

 * The code between $ARGS{ ... } and the terminating semicolon
 * is not allowed to make a net change in the string stack
 * depth, because that would interfere with the automatic
 * dropping of the string argument frame at the semicolon.
 * <ansref>"args-brace"</ansref>

 * Syntax for defining a string macro GEORGE:

     : george  ($: a$ b$ c$ -- cat$ )
       $ARGS{ arg1 arg2 arg3 }
       cat" This is arg1:  " arg1 cat" ." ENDCAT $. ;

 * The blank following the last argument is required.  For a
 * macro with no arguments, $ARGS{ } does nothing but add
 * useless overhead and should be omitted.  Two of the
 * arguments in this example are ignored and could have been
 * left out.  Note that =>"ENDCAT" would not be legal in this
 * word without something like $. to remove the concatenated
 * string from the string stack before the terminating
 * semicolon.  It is normal to use an $ARGS{ } word as a step in
 * a concatenation that is terminated elsewhere.
 
 * Sample syntax using the string macro GEORGE:

     $" bill"  $" sue"  $" marie"  george $.

 * The resulting display is:

     This is arg1:  bill.

 * NOTE: Macro argument labels must be distinct from each other
 * and from any local labels that appear in the same definition,
 * and there is no check for that.

 * NOTE: At the moment the semantics of =>"$ARGS{" is undefined
 * before =>"DOES>".
 */
FCode (p4_args_brace)
{
  register int i;

  FX (p4_Q_comp);
  for (i = 0;; i++)
  {
    switch (SOURCE_ID)
    {
      case -1:
      case 0:
        p4_word_parseword (' '); /* PARSE-WORD-NOHERE-NOTHROW */
        break;
      default:
        while ( ! p4_word_parseword (' ') && ! PFE.word.len
                 && p4_refill () ); /* PARSE-WORD-NOHERE */
        break;
    }
    *DP = 0;

    if (PFE.word.len == 1 && *PFE.word.ptr == '}')
      break;	/* if never taken, string space overflows */
    p4_push_str_copy (PFE.word.ptr, PFE.word.len);
  }

  if (i)
  {
    p4_make_str_frame (i);
    FX_COMPILE(p4_args_brace);
    FX_UCOMMA (i);
    MARGS_FLAG = ~0;
    FX_PUSH (PFE.semicolon_code);
    FX_PUSH (P4_MARGS_MAGIC);
    PFE.semicolon_code = p4_margs_EXIT;
  }
}
FCode_XE (p4_make_str_frame_execution)
{
  FX_USE_CODE_ADDR
  p4_make_str_frame ((p4ucell) *IP++);
}
static p4xcode*
p4_make_str_frame_SEE (p4xcode* ip, char* p, p4_Semant* s)
{
  int i;
/* unsigned int frame_size; */
        
  frame_size = (p4cell) *ip;
  p += SPRFIX (sprintf (p, "$ARGS{ "));
  for (i = frame_size; --i >= 0;)
    p += SPRFIX (sprintf (p, "<%c> ", 'A' - 1 + (int)(p4cell) *ip - i));
  p += SPRFIX (sprintf (p, "} "));
  return ++ip;
}

P4COMPILES(p4_args_brace, p4_make_str_frame_execution, 
	   p4_make_str_frame_SEE, P4_LOCALS_STYLE);

/** $FRAME	( u -- )
 * Push the description of a string stack frame starting at the
 * top of the string stack and containing u entries onto the
 * string frame stack.  Errors are thrown if the frame stack
 * would overflow or if the depth of the string stack above the
 * top frame, if there is one, is less than u.  The value u = 0
 * is allowed.  <ansref>"string-frame"</ansref>

 * NOTE: This implementation pushes u and the string stack
 * pointer onto the frame stack.
 */
FCode (p4_str_frame)
{
  p4_make_str_frame (*SP++);
}

/** $FRAME-DEPTH	( -- u )
 * Leave the number of string frames currently on the string
 * frame stack.  <ansref>"string-frame-depth"</ansref>
 */
FCode (p4_str_frame_depth)
{
  *--SP = ((p4ucell) SFSP0 - (p4ucell) SFSP) / sizeof (StrFrame);
}

/** DROP-$FRAME		( -- )
 * Drop the topmost string frame from the string frame stack and
 * string stack.  Errors are thrown if either stack would
 * underflow or if the string frame does not begin at the top of
 * the string stack.  The case where the frame has zero entries
 * on the string stack is handled properly.
 * <ansref>"drop-string-frame"</ansref>
 */
FCode (p4_drop_str_frame)
{
  int i;

  if (SFSP == SFSP0)
    p4_throw (P4_ON_SFRAME_UNDERFLOW);
  if (SFSP->top != SSP)
    p4_throw (P4_ON_SFRAME_MISMATCH);
  for (i = 0; i < (int) SFSP->num; i++)
    p4_pop_str ();
  SFSP += 1;
}

/** FIND-$ARG	( s -- u true | false )
 * Leave true and its index u in the top string frame if the
 * ANS Forth string matches an element of the frame, else leave
 * false.  The index of the top frame element is zero.
 * <ansref>"find-string-arg"</ansref>
 */
FCode (p4_find_str_arg)
{
  p4cell i;

  if ( (i = p4_find_arg ((const p4_char_t *) SP[1], SP[0])) >= 0)
  {
    SP[1] = i;
    SP[0] = ~0;
  }
  else
  {
    *++SP = 0;
  }
}

/** TH-$ARG	( u -- $: arg$ )
 * Leave the u-th string in the topmost string frame, where the
 * index u of the top element is zero. Throw an error if the
 * frame stack is empty or if the top frame contains less than
 * u+1 strings. <ansref>"th-string-arg"</ansref>
 */
FCode (p4_th_str_arg)
{
  if (SFSP == SFSP0)
    p4_throw (P4_ON_SFRAME_UNDERFLOW);
  if ( (p4ucell) *SP >= (*SFSP).num )
    p4_throw (P4_ON_SFRAME_ITEMS);
  PUSH_STR ( (MStr *) (SFSP->top)[(p4cell) *SP++] );
}

/** (DROP-$FRAME)	( -- )
 * Cleanup code for the end of a definition that uses
 * =>"$ARGS{".  =>";"-semicolon should be overloaded to compile
 * it automatically if dynamic string arguments were in use.
 * <ansref>"paren-drop-string-frame-paren"</ansref>
 */
FCode (p4_do_drop_str_frame)
{
  if (MARGS_FLAG)
  {
    MARGS_FLAG = 0;
    FX_COMPILE(p4_do_drop_str_frame);
    FX (p4_drop_str_frame);
  }
}
P4COMPILES(p4_do_drop_str_frame, p4_drop_str_frame, 
	   P4_SKIPS_NOTHING, P4_DEFAULT_STYLE);


/* **********************************************************************/
/* string stack support							*/
/************************************************************************/

/** $POP	( $:  a$ -- s: a$)
 * Abort if the string stack would underflow when popped.

 * Otherwise pop the top of the string stack and push it onto
 * the data stack.

 * If the string is in the current string space and initially
 * bound to the top of the string stack, mark it as garbage by
 * setting its back link to NULL and set the garbage flag.

 * This word violates the rule that only ANS Forth strings
 * should appear on the data stack, and so is under the hood.
 * <ansref>"string-pop"</ansref>
 */
FCode (p4_str_pop)
{
  *--SP = (p4cell) p4_pop_str ();
}

/** $PUSH-EXT  	( a$ -- $: a$ )
 * Pop an external mstring address from the data stack and push
 * it onto the string stack after checking for room, invoking
 * garbage collection if necessary.  Not to be used with a
 * dynamic string because a garbage collection can invalidate
 * its address.

 * This word violates the normal rule that only ANS Forth
 * strings should appear on the data stack, and so is under the
 * hood.
 * <ansref>"string-push-ext"</ansref>
 */
FCode (p4_str_push_ext)
{
  PUSH_STR ((MStr *) *SP++);
}


/* **********************************************************************/
/* more string space							*/
/************************************************************************/

/** $BREAKP@	( -- $stack.break.addr )
 * <ansref>"string-break-p-fetch"</ansref>
 */
FCode (p4_str_breakp_fetch)
{
  *--SP = (p4cell) SBREAK;
}

/** $BUFP@	( -- $buffer.addr )
 * <ansref>"string-buf-p-fetch"</ansref>
 */
FCode (p4_str_bufp_fetch)
{
  *--SP = (p4cell) SBUFFER;
}

/** $FBREAKP@	( -- frame.stack.break.addr )
 * <ansref>"string-f-break-p-fetch"</ansref>
 */
FCode (p4_str_fbreakp_fetch)
{
  *--SP = (p4cell) SFBREAK;
}

/** $FSP0@	( -- initial.frame.stack.top.addr )
 * <ansref>"string-f-s-p-zero-fetch"</ansref>
 */
FCode (p4_str_fsp0_fetch)
{
  *--SP = (p4cell) SFSP0;
}

/** $FSP@	( -- frame.stack.top.addr )
 * <ansref>"string-f-s-p-fetch"</ansref>
 */
FCode (p4_str_fsp_fetch)
{
  *--SP = (p4cell) SFSP;
}

/** $SP0@	( -- initial.string.stack.top.addr )
 * <ansref>"string-s-p-zero-fetch"</ansref>
 */
FCode (p4_str_sp0_fetch)
{
  *--SP = (p4cell) SSP0;
}

/** $SP@	( -- string.stack.top.addr )
 * <ansref>"string-s-p-fetch"</ansref>
 */
FCode (p4_str_sp_fetch)
{
  *--SP = (p4cell) SSP;
}

/** /$SFRAME-ITEM	( -- frame.stack.item.size )
 * <ansref>"slash-string-frame-item"</ansref>
 */
FCode (p4_slash_str_frame_item)
{
  *--SP = sizeof (StrFrame);
}
/** /$SFRAME-STACK	( -- max.frame.stack.size )
 * <ansref>"slash-string-frame-stack"</ansref>
 */
FCode (p4_slash_str_frame_stack)
{
  *--SP = sizeof (StrFrame) * DSTRINGS->numframes;
}

/** /$SPACE-HEADER	( -- $space.header.size )
 * <ansref>"slash-string-space-header"</ansref>
 */
FCode (p4_slash_str_space_header)
{
  *--SP = sizeof (StrSpace);
}

/** 0$SPACE	( $space.addr -- )
 * Clear the string buffer, string stack, and string frame stack
 * in the string space starting at space.addr.  Any string
 * variables holding strings in the string buffer are left
 * pointing into limbo.  This may be executed with the string
 * space in an invalid state, as long as the =>"/$BUF" and
 * =>"MAX-#$FRAMES" fields of its string space structure are
 * intact. <ansref>"zero-string-space"</ansref>

 * NOTE: This word does not zero fill the string buffer.
 */
FCode (p4_zero_str_space)
{
  p4_clear_str_space ((StrSpace *) *SP++);
}

/** CAT$@	( -- cat$.msa | 0 )
 * <ansref>"cat-string-fetch"</ansref>
 */
FCode (p4_cat_str_fetch)
{
  *--SP = (p4cell) CAT_STR;
}

/** IN-$BUFFER?	( msa -- flag )
 * Leave true if the mstring is in the string buffer.
 * <ansref>"in-string-buffer"</ansref>
 */
FCode (p4_in_str_buffer_Q)
{
  *SP = ( SBUFFER <= (DStr *) *SP && (DStr *) *SP < SBREAK ) ? ~0 : 0 ;   
}


/* **********************************************************************/
/* environment								*/
/************************************************************************/

/** "ENVIRONMENT DSTRINGS-EXT" ( -- datecoded-version )
 * an => ENVIRONMENT constant to be checked with =>"ENVIRONMENT?"
 * the value is currently encoded as a datecode with a decimal
 * printout of format like YYMMDD
 */

/** "ENVIRONMENT /SCOPY" ( -- MAX_DATA_STR )
 * an => ENVIRONMENT constant to be checked with =>"ENVIRONMENT?"
 * returns the configuration value of MAX_DATA_STR
 */

/** "ENVIRONMENT /DYNAMIC-STRING" ( -- MAX_MCOUNT )
 * an => ENVIRONMENT constant to be checked with =>"ENVIRONMENT?"
 * returns the configuration value of MAX_MCOUNT
 */


/* **********************************************************************/
/* interpreter								*/
/************************************************************************/

static p4ucell
FXCode (interpret_dstrings) /*hereclean*/
{
  if (! STATE || ! p4_MARGS_FLAG) return 0; /* quick path */
  /* WORD-string is at PFE.word. (and not at HERE anymore) */
  return p4_compile_marg (PFE.word.ptr, PFE.word.len);
}

static FCode (drop_all_strings)
{
  p4_drop_all_strings (p4_DSTRINGS);
}

static FCode(dstrings_deinit)
{
  PFE.interpret[6] = 0;
  PFE.abort[3] = 0;
  if (PFE.dstrings)
  { 
    p4_xfree (PFE.dstrings);
    PFE.dstrings = 0;
  }
}

static FCode(dstrings_init)
{
  /* stdc commandline option: --str-buffer-size VALUE */
  static const p4_char_t __str_buffer_size[] = "/str-buffer"; 
  p4ucell str_buffer_size =
    p4_search_option_value (__str_buffer_size,
                            sizeof(__str_buffer_size)-1,
                            P4_STR_BUFFER_SIZE, PFE.set);

  /* WARNING: make_str_space calls xalloc, _deinit above calls xfree */
  PFE.dstrings = (char *) p4_make_str_space (str_buffer_size,
					     P4_MAX_SFRAMES);

  PFE.interpret[6] = PFX (interpret_dstrings);
  PFE.abort[3] = PFX(drop_all_strings);
  p4_forget_word ("deinit:dstrings:%i", 6, PFX(dstrings_deinit), 6);
}


P4_LISTWORDS (dstrings) =
{
  /* P4_INTO: CURRENT */
  /* constants */
  P4_OCoN ("/MCOUNT",		SIZEOF_MCOUNT),
  P4_OCoN ("MAX-MCOUNT",	MAX_MCOUNT),
  P4_FXco ("EMPTY$",		p4_empty_str),
  P4_FXco ("\\n$",		p4_newline_str),
  /* variables */
  P4_DVaR ("DSTRINGS",	        dstrings),
  /* ANS Forth string extensions */
  P4_FXco ("MPLACE",		p4_m_place),
  P4_FXco ("PARSE>S",		p4_parse_to_s),
  P4_SXco ("S`",		p4_s_back_tick),
  P4_FXco ("SM,",		p4_s_m_comma),
  P4_xOLD ("S,",		"SM,"),
  /* measured strings */
  P4_FXco ("MCOUNT@",		p4_m_count_fetch),
  P4_FXco ("MCOUNT!",		p4_m_count_store),
  P4_FXco ("MCOUNT",		p4_m_count),
  P4_FXco ("-MCOUNT",		p4_minus_m_count),
  /* comments */
  P4_IXco ("($:",		p4_paren),
  /* string space */
  P4_FXco ("0STRINGS",		p4_zero_strings),
  P4_FXco ("$GARBAGE?",		p4_str_garbage_Q),
  P4_FXco ("$GC-OFF",		p4_str_gc_off),
  P4_FXco ("$GC-ON",		p4_str_gc_on),
  P4_FXco ("$UNUSED",		p4_str_unused),
  P4_FXco ("COLLECT-$GARBAGE",  p4_collect_str_garbage),
  P4_FXco ("MAKE-$SPACE",	p4_make_str_space),
  P4_FXco ("/$BUF",		p4_slash_str_buf),
  P4_FXco ("MAX-#$FRAMES",	p4_max_num_str_frames),
 /* load and store */
  P4_FXco ("$!",		p4_str_store),
  P4_FXco ("$@",		p4_str_fetch),
  P4_SXco ("(M$:)",		p4_marg_execution),
  P4_SXco ("$\"",		p4_str_quote),
  P4_SXco ("$`",		p4_str_back_tick),
  P4_RTco ("$CONSTANT",		p4_str_constant),
  P4_FXco ("$VARIABLE",		p4_str_variable),
  P4_FXco ("PARSE>$",		p4_parse_to_str),
  /* string stack */
  P4_FXco ("$.",		p4_str_dot),
  P4_FXco ("$2DROP",		p4_str_two_drop),
  P4_FXco ("$2DUP",		p4_str_two_dup),
  P4_FXco ("$DEPTH",		p4_str_depth),
  P4_FXco ("$DROP",		p4_str_drop),
  P4_FXco ("$DUP",		p4_str_dup),
  P4_FXco ("$NIP",		p4_str_nip),
  P4_FXco ("$OVER",		p4_str_over),
  P4_FXco ("$PICK",		p4_str_pick),
  P4_FXco ("$SWAP",		p4_str_swap),
  P4_FXco ("$S>",		p4_str_s_from),
  P4_FXco ("$S>-COPY",		p4_str_s_from_copy),
  P4_FXco ("$S@",		p4_str_s_fetch),
  P4_FXco ("$TUCK",		p4_str_tuck),
  P4_FXco ("$TYPE",		p4_str_dot),
  P4_FXco (">$S-COPY",		p4_to_str_s_copy),
  P4_FXco (">$S",		p4_to_str_s),
 /* concatenation */
  P4_FXco ("CAT",		p4_cat),
  P4_FXco ("S-CAT",		p4_s_cat),
  P4_FXco ("PARSE-CAT",		p4_parse_cat),
  P4_FXco ("ENDCAT",		p4_endcat),
  P4_SXco ("CAT\"",		p4_cat_quote),
  P4_SXco ("CAT`",		p4_cat_back_tick),
 /* arguments */
  P4_FXco ("#$ARGS",		p4_num_str_args),
  P4_IXco ("$ARGS{",		p4_args_brace),
  P4_iOLD ("ARGS{",		"$ARGS{"),
  P4_FXco ("$FRAME",		p4_str_frame),
  P4_FXco ("$FRAME-DEPTH",	p4_str_frame_depth),
  P4_FXco ("DROP-$FRAME",	p4_drop_str_frame),
  P4_FXco ("FIND-$ARG",		p4_find_str_arg),
  P4_xOLD ("FIND-ARG",		"FIND-$ARG"),
  P4_FXco ("TH-$ARG",		p4_th_str_arg),
  P4_SXco ("(DROP-$FRAME)",	p4_do_drop_str_frame),
  /* string stack support */
  P4_FXco ("$POP",		p4_str_pop),
  P4_FXco ("$PUSH-EXT",		p4_str_push_ext),
  /* more string space */
  P4_FXco ("$BREAKP@",		p4_str_breakp_fetch),
  P4_FXco ("$BUFP@",		p4_str_bufp_fetch),
  P4_FXco ("$FBREAKP@",		p4_str_fbreakp_fetch),
  P4_FXco ("$FSP@",		p4_str_fsp_fetch),
  P4_FXco ("$FSP0@",		p4_str_fsp0_fetch),
  P4_FXco ("$SP@",		p4_str_sp_fetch),
  P4_FXco ("$SP0@",		p4_str_sp0_fetch),
  P4_FXco ("/$FRAME-ITEM",	p4_slash_str_frame_item),
  P4_FXco ("/$FRAME-STACK",	p4_slash_str_frame_stack),
  P4_FXco ("/$SPACE-HEADER",	p4_slash_str_space_header),
  P4_FXco ("0$SPACE",		p4_zero_str_space),
  P4_FXco ("CAT$@",		p4_cat_str_fetch),
  P4_FXco ("IN-$BUFFER?",	p4_in_str_buffer_Q),

  P4_INTO ("ENVIRONMENT", 0),
  P4_OCoN ("DSTRINGS-EXT",	040715),
  P4_OCoN ("/SCOPY",		MAX_DATA_STR ),
  P4_OCoN ("/DYNAMIC-STRING",	MAX_MCOUNT ),
  P4_XXco ("DSTRINGS-LOADED",   dstrings_init),

  P4_INTO ("EXTENSIONS", 0),
  P4_EXPT ("string count too large"       /* -2053 */, P4_ON_SCOUNT_OVERFLOW),
  P4_EXPT ("string space overflow"        /* -2054 */, P4_ON_SSPACE_OVERFLOW),
  P4_EXPT ("string garbage locked"        /* -2055 */, P4_ON_SGARBAGE_LOCK),
  P4_EXPT ("string stack underflow"       /* -2056 */, P4_ON_SSTACK_UNDERFLOW),
  P4_EXPT ("cat lock preventing string copy" /* -2057 */,   P4_ON_SCAT_LOCK),
  P4_EXPT ("dynamic string count too large"  /* .. */, P4_ON_DSCOUNT_OVERFLOW),
  P4_EXPT ("too many string frames"       /* -2059 */, P4_ON_SFRAME_OVERFLOW),
  P4_EXPT ("not enough strings in top frame" /* -2060 */, P4_ON_SFRAME_ITEMS),
  P4_EXPT ("string frame stack underflow" /* -2061 */, P4_ON_SFRAME_UNDERFLOW),
  P4_EXPT ("string frame not at top of string stack"   /* -2062 */,
	   P4_ON_SFRAME_MISMATCH),
};
P4_COUNTWORDS (dstrings, "Dynamic-Strings extension");

/* 
 * Local variables:
 * c-file-style: "gnu"
 * End:
 */
