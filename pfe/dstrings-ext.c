/**
 * -- Dynamic-Strings words
 *
 * Copyright (C) 2001 David N. Williams
 *
 * @see LGPL 
 * @author David N. Williams              @(#) %derived_by: guidod %
 * @version %version: 0.6.5 %
 *   (%date_modified: Mon Mar 12 10:32:11 2001 %)
 *     starting date: Sat Dec 16 14:00:00 2000
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
 * version of this library part under the GPL, the author would regard it
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
 *
 */

#include <pfe/def-config.h>
#ifdef PFE_WITH_DSTRINGS_EXT

#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: dstrings-ext.c,v 0.30 2001-03-12 09:32:11 guidod Exp $";
#endif

/* ------------------------------------------------------------------- */

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <stddef.h>		/* offsetof() */
#include <pfe/dstrings-ext.h>	/* stdlib.h */

#if 0
#include <pfe/def-comp.h>	/* p4_create_RT_() */
#include <pfe/core-sub.h>	/* p4_parse(), p4_skip_delimiter() */
#include <pfe/dict-sub.h>	/* p4_header() */
#include <pfe/misc-ext.h>	/* p4_Q_comp_() */
#endif

#ifndef MAX_DSCOUNT             /* USER-CONFIG */
#define MAX_DSCOUNT  65535      /* -> the default for TYPEOF_SCOUNT short */
#endif
#ifndef MAX_SCOUNT              /* USER-CONFIG */
#define MAX_SCOUNT  1024        /* reasonable limit for counted data strings */
#endif

#define SIZEOF_SCOUNT (sizeof(PFE_TYPEOF_SCOUNT))
#define SIZEOF_DSTR_HEADER  (PFE_SIZEOF_CELL + SIZEOF_SCOUNT) 

/************************************************************************/
/* functions								*/
/************************************************************************/

/* STORE FORTH STRING AS PACKED STRING
 *
 * These two functions are a "political" statement, an implementation of
 * our advocacy for packed strings over counted strings.
 *
 * Store the Forth string in data space as a packed string, which has an
 * environmental dependence on the size of the count field.  The current
 * version throws an error if the count is larger than MAX_SCOUNT.
 */

static char *
p4_pstring_comma (const char *addr, size_t len)
{
  char *p = (char *) DP;
  char *limit;

  if (len >= MAX_SCOUNT)
    p4_throw (P4_ON_SCOUNT_OVERFLOW);

  *((PCount *) DP)++ = (PCount) len;    /* store count */
  while (len-- != 0)                    /* store string */
    *DP++ = (p4char) *addr++;

  limit = (char *) ALIGNTO_CELL (DP);
  while ( (char *) DP < limit ) 
    *DP++ = 0;
  return p;
}

static char *
p4_parse_pstring_comma (char del)
{
  char *p;
  p4ucell n;

  p4_parse (del, &p, &n);
  return p4_pstring_comma (p, (size_t) n);
}

/* CLEAR A STRING SPACE
 *
 * Clear the string buffer, string stack, and string frame stack.  Any
 * string variables holding strings in the string buffer are left pointing
 * into limbo.  This may be executed with the string space in an invalid
 * state, as long as the size and numframes fields of its StrSpace structure
 * are intact.
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
 * Allocate and initialize a string space with string buffer plus string 
 * stack of prescribed size.  The size is rounded up for cell alignment, and
 * buffer begins and ends with that alignment.  The size of the string frame
 * stack is a compiled system parameter.
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
 * The garbage flag in the current string space structure is tested, and if
 * there is garbage, it is collected, unless garbage collection is locked.
 * Garbage strings are marked by null backward links.  Nongarbage strings
 * are bound by their backward links, pointing either to a string variable 
 * parameter field address or to an entry on the string stack (which may be 
 * the deepest of several).
 *
 * Garbage collection fills the gaps occupied by garbage strings by moving
 * any nongarbage strings to lower memory one at a time.  The backward link
 * of a string that is moved does not change, but the forward links, either 
 * in at most one string variable, or possibly several on the string stack,
 * are updated to point to the new CFA.  This algorithm is "fast" because
 * it does not require a scan of a list of string variables, due to the
 * backward links.  It does, however, require a scan of the string stack.
 *
 * When there is no garbage to collect, this routine returns 0.  If there is 
 * garbage, it throws an error when garbage collection is locked.  Otherwise 
 * it collects the garbage and returns 1.
 */

int
p4_collect_garbage (void)
{
  DStr *next, *target;
  PStr **sstack;
  char *p, *q, *limit;

  if ( !GARBAGE_FLAG ) 
    return 0;		/* no garbage to collect */

  if ( GARBAGE_LOCK ) p4_throw (P4_ON_SGARBAGE_LOCK);

  GARBAGE_FLAG = 0;
  next = SBUFFER;

  /* locate first garbage hole (no need to check off end, because we
     know there is a hole) */
  while ( next->backlink )
    {	    /* not garbage, skip to next */
      next = (DStr*)ALIGNTO_CELL(next + sizeof (next->backlink)
             + sizeof (next->count) + next->count);
    }

  do	    /* not off end, garbage hole found */
    {
      target = next;

      /* skip over garbage */
      while ( !next->backlink && next < SBREAK )
	{	/* garbage and not off end, skip to next */
	  next = (DStr*) ALIGNTO_CELL (next + sizeof (next->backlink)
	         + sizeof (next->count)
	         + next->count);
	}

      /* move and update until next garbage */
      while ( next->backlink && next < SBREAK )
	{	/* not garbage and not off end */
	  target->backlink = next->backlink;
	  target->count = next->count;

	  /* update forward link(s) */
          if (next->backlink <= (PStr**) SBUFFER || next->backlink > SSP0)
            {       /* must be a string variable or cat$ */
	      *(next->backlink) = (PStr*) &(target->count);
	    }
	  else
	    {	    /* must be on the string stack */
 	      for ( sstack = SSP; sstack < SSP0; sstack++ )
		if (*sstack == (PStr*) &(next->count))
		  *sstack = (PStr*) &(target->count);
	    }

	  /* move string, including null fill, to hole */
	  q = (char*) &(target->body);
	  p = (char*) &(next->body);
	  limit = (char*) ALIGNTO_CELL (p + next->count);
	  while (p < limit)
	    *q++ = *p++;
	  next = (DStr*) p;
 	  target = (DStr*) q;
	}
    }
  while ( next < SBREAK );

  SBREAK = target; 
  return 1;
}

/* POP STRING AND MARK GARBAGE
 *
 * Throw an error if the string stack would underflow when popped.
 *
 * Otherwise increment the string stack pointer, thus popping the string
 * stack.
 * 
 * If the old string is in the current string space and bound to the old
 * string stack position, set its back link to NULL and set the garbage
 * flag.
 */

PStr *
p4_pop_str (void)
{
  PStr **strsp = SSP;

  if (strsp == SSP0) p4_throw (P4_ON_SSTACK_UNDERFLOW);
  SSP += 1;

  if ( *strsp >= (PStr *) SBUFFER
       && *strsp < (PStr *) SBREAK
       && *((p4cell **) (*strsp)-1) == (p4cell *) strsp )
    {
      *((p4cell **) (*strsp)-1) = NULL;
      GARBAGE_FLAG = ~0;
    };

  return *strsp;
}

/* DROP ALL STRINGS AND FRAMES
 *
 * Clear the string stack, string frame stack, and any concatenating string
 * in a string space.  Dynamic strings held in string variables remain.
 * This word is called by ABORT with the current string space.  We decided
 * not to do a garbage collection here because it's used when there's an
 * error, and we might want to dump string space for debugging.
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
p4_push_str_copy (char *addr, size_t len)
{
  char *buf;

  if (len > MAX_DSCOUNT)
    p4_throw (P4_ON_DSCOUNT_OVERFLOW);
  Q_CAT;
  Q_ROOM (SBREAK, len + SIZEOF_DSTR_HEADER + PFE_SIZEOF_CELL);

  /* Don't do earlier, maybe garbage was collected: */
  buf = (char*) SBREAK;

  (void*) *((PStr**) buf)++ = --SSP;	/* back link */
  *SSP = (PStr*) buf;			/* forward link */
  *((PCount*) buf)++ = (PCount) len;
 
  while (len-- > 0)			/* copy string body */
    *buf++ = *addr++;
 
  addr = (char*) ALIGNTO_CELL (buf);	/* null fill */
  while (buf < addr) 
    *buf++ = 0;

  SBREAK = (DStr*) buf;
}

/* FIND ARGUMENT IN SFRAME
 *
 * Search the top string frame for a match to a Forth string and return
 * its index if found, else return -1.
 *
 * NOTE:  The index starts with 0 at the top of the string frame.  This
 * is opposite to the natural left to right ordering in stack comments.
 */

int
p4_find_arg (char *nm, int l)
{
  PStr **ssp = SFSP->top;
  PStr *p;
  int i;

  for (i = 0; i < SFSP->num; i++)
    {
      p = *(ssp++);
      if (l == SLEN (p) && memcmp (nm, SADDR (p), l) == 0)  
        return i;
    }
  return -1;
}

/* MAKE STRING FRAME
 *
 * Define the top n items on the string stack as a string frame by
 * pushing n and the SSP onto the string frame stack.  Errors are thrown
 * if there is not enough room on the string frame stack or if there are
 * not at least n items on the top of the string stack above any previous 
 * string frame.
 */

void
p4_make_str_frame (p4ucell n)
{
  if (SFSP == SFBREAK)
    p4_throw (P4_ON_SFRAME_OVERFLOW);
  if (n > (SFSP - (StrFrame *) SSP))
    p4_throw (P4_ON_SFRAME_ITEMS);
  SFSP -= 1 ;
  SFSP->top = SSP;
  SFSP->num = n;
}

/* COMPILE MACRO ARGUMENT 
 *
 * Search for a string in the top string stack frame.  If found, compile
 * run-time code that concatenates the corresponding string in the top
 * run-time string frame onto the current CAT$.
 *
 * This code imitates PFE code for compiling a local.  It is intended for
 * use in a modified INTERPRET.
 */

FCode (p4_marg_execution)
{
  PUSH_STR ((PStr *) (SFSP->top)[(p4cell) *IP++]);
  p4_cat_ ();
}

int
p4_compile_marg (char *name, int len)
{
  static p4code cfa = p4_marg_execution_;
  int n;

  if ((n = p4_find_arg (name, len)) == -1)
    return 0;
  FX_COMMA (&cfa);
  FX_COMMA (n);
  return 1;
}


/************************************************************************/
/* constants								*/
/************************************************************************/

struct p4_PStr p4_empty_str =
{ 
  0,		/* count */
  0		/* superfluous body */
};

struct p4_PStr p4_newline_str =
{ 
  1,		/* count */
  '\n'		/* body */
};

/** EMPTY$	( $: -- empty$ )
 * Push the PSA of a fixed, external representation of the empty string onto
 * the string stack.  <ansref>"empty-string"</ansref>
 */
FCode (p4_empty_str)
{
  PUSH_STR (&p4_empty_str);
}

/** \n$		( $: -- newline$ )
 * Leave the PSA of a string whose body is the Unix newline character on the
 * string stack.  <ansref>"newline-string"</ansref>
*/
FCode (p4_newline_str)
{
  PUSH_STR (&p4_newline_str);
}

/************************************************************************/
/* variables								*/
/************************************************************************/

/** DSTRINGS	( -- dfa )
 * A Forth variable that holds the address of the current string space, where
 * all dynamic string operations take place.  <ansref>"d-strings"</ansref>
*/


/************************************************************************/
/* Forth string extensions						*/
/************************************************************************/

/** S,		( addr len -- addr' len )
 * ALLOT room and store the Forth string into data space as a packed string, 
 * leaving data space aligned; and leave the length and new body address. It
 * is assumed that len is unsigned. An error is thrown if len is larger than
 * the system parameter MAX_SCOUNT. <ansref>"s-comma"</ansref>

 * NOTE: MAX_SCOUNT is returned by 
   S" /SCOPY" ENVIRONMENT?
 * Perhaps this restriction should be removed in favor of the normal data
 * space overflow error. 
 
 * NOTE: S, is the same as STRING, in Wil Baden's Tool Belt, except it
 * stores a packed string instead of a counted string.
 */
FCode (p4_s_comma)
{
  SP[1] = (p4cell) ( p4_pstring_comma ((char*) SP[1], (size_t) *SP)
		     + SIZEOF_SCOUNT );
}


/************************************************************************/
/* string space								*/
/************************************************************************/

/** 0STRINGS	( -- )
 * Set all string variables holding bound string values in string space to
 * the empty string, and clear string space including the string buffer,
 * string stack, and string stack frames.  This word should be executed 
 * only when string space is in a valid state.
 * <ansref>"zero-strings"</ansref>
 */
FCode (p4_zero_strings)
{
  DStr *next = SBUFFER;

  while (next < SBREAK)
    {   
      if (next->backlink)
	*(next->backlink) = &p4_empty_str;
      next = (DStr*) ALIGNTO_CELL ((size_t)next
	     + offsetof (DStr, body) + next->count);
    }
  p4_clear_str_space (DSTRINGS);
} 

/** $GC-OFF		( -- )
 * Unlock string space for garbage collection.  This is the default.  
 * <ansref>"string-g-c-off"</ansref>
*/

FCode (p4_str_gc_off)
{
  GARBAGE_LOCK = ~0;
}

/** $GC-ON		( -- )
 * Lock string space so garbage collection cannot occur.  An error will be 
 * thrown if garbage collection is attempted.
 * <ansref>"string-g-c-on"</ansref>
 */

FCode (p4_str_gc_on)
{
  GARBAGE_LOCK = 0;
}

/** $UNUSED		( -- u )
 * Leave the number of bytes available for dynamic strings and string stack 
 * entries in the string buffer.  <ansref>"string-unused"</ansref>
 */

FCode (p4_str_unused)
{
  *--SP = (p4cell) SSP - (p4cell) SBREAK; 
}


/** COLLECT-$GARBAGE	( -- collected-flag )
 * If string space is not marked as containing garbage, return false.  If 
 * there is garbage, throw an error when garbage collection is locked. 
 * Otherwise remove the garbage and return true.  Garbage collection is 
 * "transparent", so the user would not normally use this word. 
 * <ansref>"collect-string-garbage"</ansref>
 */
FCode (p4_collect_str_garbage)
{
  p4_collect_garbage () ? (*--SP = ~0) : (*--SP = 0);
}

/** MAKE-$SPACE	( size #frames -- addr )
 * Allocate and initialize a string space with size bytes available for
 * the string buffer including the string stack, and with a string frame 
 * stack for frame description entries holding up to #frames.  The size is
 * rounded up to cell alignment, and the buffer begins and ends with cell 
 * alignment.  Return addr, the address of the string space.  The standard
 * word FREE with addr as input can be used to release the space.
 * <ansref>"make-string-space"</ansref>
 */
FCode (p4_make_str_space)
{
  SP[1] = (p4cell) p4_make_str_space (SP[1], SP[0]);
  SP += 1;
}


/************************************************************************/
/* string compilation							*/
/************************************************************************/

/** $"  ( [ccc<">] -- $: str )
 * Parse ccc delimited by " (double-quote) and store it in data space as a
 * packed string.  If interpreting, leave the PSA on the string stack.  If
 * compiling, append to the current definition run-time semantics that
 * leaves the PSA on the string stack.  A program should not alter the
 * stored string.  An error is thrown if the quoted string length is larger
 * than the system parameter MAX_SCOUNT (see =>"S,").
 * <ansref>"string-quote"</ansref>

 * NOTE:  In contrast to S" when interpreting, the string stored by =>'$"' 
 * is not transient.

 * The implementation is based on PFE code for =>'S"'.
 */
FCode (p4_str_quote)
{
  if (STATE)
    {
      FX_COMPILE1 (p4_str_quote);
      p4_parse_pstring_comma ('"');
    }
  else
    {
      PUSH_STR ((PStr *) p4_parse_pstring_comma ('"'));
    }
}
FCode (p4_str_quote_execution)
{
  PUSH_STR ((PStr *) IP);
  FX_SKIP_PSTRING;
}
P4COMPILES (p4_str_quote, p4_str_quote_execution,
	    P4_SKIPS_PSTRING, P4_DEFAULT_STYLE);


/** $CONSTANT	( "name" $: a$ -- )
 * Create a definition for "name" with the execution semantics
 * "name" execution:		($: -- a$ )

 * It is assumed that the input string resides as a packed, unchanging string
 * outside of string space.  <ansref>"string-constant"</ansref>

 * For example:
   $" This is a sample string." $constant sample$
 */
FCode (p4_str_constant)
{
  extern FCode (p4_str_constant_RT);
  p4_header (PFX (p4_str_constant_RT), 0);
  FX_COMMA (p4_pop_str ());
}

FCode (p4_str_constant_RT)
{
  PUSH_STR ((PStr *) WP_PFA[0]);
}

/** $VARIABLE		( "name" -- )
  "name" execution:	( -- dfa )

 * Create an ordinary Forth variable and initialize it to the address of a
 * fixed, external, packed representation of the empty string, such as that 
 * pushed onto the string stack by =>"EMPTY$".  
 * <ansref>"string-variable"</ansref>"
 */
FCode (p4_str_variable)
{
  p4_header (p4_create_RT_, 0);
  FX_COMMA (&p4_empty_str);
}

/** ($:			( "ccc<paren>" -- )
 * A synonym for =>"(".  Immediate.  "paren-string-colon"
 */

/** ARGS{ ( arg1'$ ... argN'$ "arg1 ... argN <}>" --  )
    compilation: ( -- $: arg1$ ... argN$ )

 * Immediate and compilation-only.

 * Copy the argument strings to the string buffer, push them onto 
 * the string stack with "argn" the most accessible, and make them 
 * into the top compile-time string stack frame.  Compile the run-time 
 * code to make an argument frame out of the n most accessible 
 * run-time string stack entries.  Inform the system interpreter that 
 * it should compile run-time code for any white-space delimited 
 * argument encountered in the text of the definition, that 
 * concatenates the corresponding string in the run-time frame.  At 
 * the semicolon terminating the definition, drop the compile-time 
 * argument frame and compile code to drop the run-time argument 
 * frame. <ansref>"args-brace"</ansref>

 * Syntax for defining a string macro GEORGE:

	: george   ($: a$ b$ c$ -- cat$ )
	  args{ arg1 arg2 arg3 }
	  m" This is arg1:  " arg1 m" ." ENDCAT ;

 * The blank following the last argument is required.  For a macro 
 * with no arguments, =>"ARGS{" } does nothing but add useless 
 * overhead and should be omitted.  Words intended only as steps in 
 * building a macro would omit =>"ENDCAT", which terminates concatenation 
 * and leaves the concatenated string on the string stack.

 * Sample syntax using the string macro GEORGE:

    $" bill"  $" sue"  $" marie"  george $.

 * The resulting display is:

    This is arg1:  bill.

 * NOTE: Macro argument labels must be distinct from each other and 
 * from any local labels that appear in the same definition, and there 
 * is no check for that.

 * NOTE: At the moment the semantics of =>"ARGS{" is undefined 
 * before =>"DOES>".
 */
FCode (p4_args_brace)
{
  int i;
  char *p;
  p4ucell n;

  p4_Q_comp_ ();
  for (i = 0;; i++)
    {
      p4_skip_delimiter (' ');
      p4_parse (' ', &p, &n);
      if (n == 1 && *p == '}')
        break;	/* if never taken, string space overflows */
      p4_push_str_copy (p, n);
    }

  if (i)
  {
    p4_make_str_frame (i);
    FX_COMPILE(p4_args_brace);
    FX_COMMA (i);
    MARGS_FLAG = ~0;
  }
}
FCode (p4_make_str_frame_execution)
{
  p4_make_str_frame ((p4ucell) *IP++);
}
P4COMPILES(p4_args_brace, p4_make_str_frame_execution, 
	   P4_SKIPS_CELL, P4_LOCALS_STYLE);


/** M"	( "ccc<quote>" -- )
 * This word has only compile-time semantics, just like =>"M'". 
 * They append run-time semantics to the current definition 
 * that concatenates the quoted, respectively, ticked string, 
 * according to the specification for =>"CAT".  An error is thrown 
 * if the length of the quoted string is longer than the system 
 * parameter MAX_SCOUNT (see =>"S,"). <ansref>"m-quote"</ansref> 

 * NOTE: These words are not just for use in macros.  Perhaps better
 * names would be +" and +', but that suggests a string operand 
 * on the stack.  A choice consistent with the rest of our names
 * would be cat" and cat', which requires more typing.
 */
FCode (p4_m_quote)
{
  FX_COMPILE1 (p4_m_quote);
  p4_parse_pstring_comma ('"');
}
FCode (p4_m_quote_execution)
{
  PUSH_STR ((PStr *) IP);
  FX_SKIP_PSTRING;
  p4_cat_ ();
}
P4COMPILES (p4_m_quote, p4_m_quote_execution,
            P4_SKIPS_PSTRING, P4_DEFAULT_STYLE);

/** M'	( "ccc<tick>" -- )
 * This word has only compile-time semantics, just like =>'M"'. 
 * They append run-time semantics to the current definition 
 * that concatenates the ticked, respectively, quoted string, 
 * according to the specification for =>"CAT".  An error is thrown 
 * if the length of the quoted string is longer than the system 
 * parameter MAX_SCOUNT (see =>"S,"). <ansref>"m-tick"</ansref>

 * NOTE: These words are not just for use in macros.  Perhaps better
 * names would be +" and +', but that suggests a string operand 
 * on the stack.  A choice consistent with the rest of our names
 * would be cat" and cat', which requires more typing.
 */
FCode (p4_m_tick)
{
  FX_COMPILE1 (p4_m_tick);
  p4_parse_pstring_comma ('\'');
}
P4COMPILES (p4_m_tick, p4_m_quote_execution,
	    P4_SKIPS_PSTRING_TICK, P4_DEFAULT_STYLE);


/************************************************************************/
/* string stack operations						*/
/************************************************************************/

/** $DEPTH   ( -- n )
 * Leave the number of items on the string stack.
 * <ansref>"string-depth"</ansref>
 */
FCode (p4_str_depth)
{
  *--SP = (SSP0 - SSP);
}

/** $DROP   ( $:  a$ -- )
 * Drop the topmost string stack entry, marking it as garbage if it is
 * initially bound to the top of the string stack.
 * <ansref>"string-drop"</ansref>
 */
FCode(p4_str_drop)
{
 p4_pop_str ();
}

/** $DUP		( $: a$ -- a$ a$ )
 * Leave a copy of the topmost string stack entry.  The string value is
 * not copied.  <ansref>"string-dupe"</ansref>
 */
FCode (p4_str_dup)
{
  PStr **strsp;

  strsp = SSP;
  if (strsp == SSP0)
   p4_throw (P4_ON_SSTACK_UNDERFLOW);
  PUSH_STR (*strsp);
}

/** $OVER		( $: a$ b$ -- a$ b$ a$ )
 * Leave a copy of the next most accessible string stack entry on top of
 * the string stack.  The string value is not copied.
 * <ansref>"string-over"</ansref>
 */
FCode (p4_str_over)
{
  if (SSP0 - SSP < 2)
    p4_throw (P4_ON_SSTACK_UNDERFLOW);
  PUSH_STR (SSP[2]);
}

/* $SWAP                ( $: a$ b$ -- b$ a$ )
 * Exchange the two most accessible strings on the string stack.  An error
 * occurs if there are less than two strings on the string stack.  Neither
 * string value is copied.  <ansref>"string-swap"</ansref>
*/
FCode (p4_str_swap)
{
  PStr *str1, *str2;
  char **blp;	/* back link pointer */

  if ( (SSP0 - SSP) < 2 )
   p4_throw (P4_ON_SSTACK_UNDERFLOW);
  str1 = SSP[1];
  str2 = SSP[0];

  if ( !(str1 == str2) )
    {
      SSP[0] = str1;
      SSP[1] = str2;

      if ( str1 >= (PStr *) SBUFFER && str1 < (PStr *) SBREAK )
	{
	blp = (char **) ((size_t) str1 - PFE_SIZEOF_CELL);
	if ( *blp == (char *) (&SSP[1]) )
	  *blp = (char *) SSP;
	}

      if ( str2 >= (PStr *) SBUFFER && str2 < (PStr *) SBREAK )
	{
	blp = (char **) ((size_t) str2 - PFE_SIZEOF_CELL);
	if ( *blp == (char *) SSP )
	  *blp = (char *) &SSP[1];
	}
    }
}

/** $S>		( $: a$ -- S: a.str )
 * Drop a$ from the string stack and leave it as a Forth string a.str,
 * without copying.  <ansref>"string-s-from"</ansref>

 * WARNING:  If a$ is a dynamic string, it may  at the next garbage
 * collection, making a.str invalid.
*/
FCode (p4_str_s_from)
{
  PStr *str = p4_pop_str ();

  *--SP = (p4cell) str + SIZEOF_SCOUNT;
  *--SP = str->count;
}

/** $S>-COPY	( $: a$ -- S: a.str )
 * Drop a$ from the string stack, copy it into data space as a packed string,
 * and leave it as a Forth string a.str.  An error is thrown if the string
 * length is larger than the system parameter MAX_SCOUNT (see =>"S,").
 * <ansref>"string-s-from-copy"</ansref>
*/
FCode (p4_str_s_from_copy)
{
  PStr *str = p4_pop_str ();
  PStr *p = (PStr *) p4_pstring_comma (SADDR (str), SLEN (str));

  *--SP = (p4cell) SADDR (p);
  *--SP = SLEN (p);
}

/** $S@ 	 ( $: a$ -- a$ S: a.str )
 *
 * Leave the string stack unchanged, and leave the string body address and
 * length on the data stack.  <ansref>"string-s-fetch"</ansref>

 * NOTE:  In earlier versions this was call $S@S.  The trailing "S" is 
 * superfluous if it is understood that the only string format that
 * usually appears on the data stack is the Forth string format.

 * WARNING:  If a$ is a dynamic string, it may move at the next garbage 
 * collection, making a.str invalid.
 */
FCode (p4_str_s_fetch)
{
  if (SSP == SSP0)
    p4_throw (P4_ON_SSTACK_UNDERFLOW);
  *--SP = (p4cell) SADDR (*SSP);
  *--SP = SLEN (*SSP);
}

/** >$S	( a.str -- $: a$ )
 * Push the external Forth string a.str onto the string stack, without
 * copying the string value into the string buffer.  It is an unchecked
 * error if the Forth string a.str is not stored as an external packed
 * string. <ansref>"to-string-s"</ansref> 

 * WARNING: If the string value of a.str is actually in the string 
 * buffer, the push operation may generate a garbage collection 
 * that invalidates its PSA. 
 */
FCode (p4_to_str_s)
{
  SP += 1;		/* drop length */
  PUSH_STR ((void*) *SP++ - SIZEOF_SCOUNT);
}

/** >$S-COPY	( a.str -- $: a$ )
 * Copy the external string value whose body address and count are on the
 * parameter stack into the string buffer and push it onto the string stack.
 * Errors are thrown if the count is larger than MAX_DSCOUNT, if there is not
 * enough room in string space, even after garbage collection, or if there is
 * an unterminated string concatenation.  The input external string need not
 * exist as a packed string. <ansref>"to-string-s-copy"</ansref>

 * NOTE:  MAX_DSCOUNT is the largest size the count field of a packed string
 * can hold, e.g., 255 or 64K - 1. It is returned by:
 * S" /DYNAMIC-STRING" ENVIRONMENT?

 * WARNING: This word should not be used when the input string is a bound
 * string because the copy operation may generate a garbage collection which
 * invalidates its PSA.
 */
FCode (p4_to_str_s_copy)
{
  p4_push_str_copy ((char *) SP[1], SP[0]);
  SP += 2;
}


/************************************************************************/
/* string manipulation							*/
/************************************************************************/

/* $!	( $var.dfa $: a$ -- )
 * Store the string PSA on the string stack in the variable 
 * whose DFA is on the parameter stack.  <ansref>"string-store"</ansref>

 * NOTES: The only situation in which =>"$!" copies the string 
 * value is when it is a bound string already stored in another
 * variable.  In that case, the new copy is the one that is stored 
 * in the variable. In particular, external strings are not copied.

 * If the string value held by the string variable on entry is a bound string
 * that is also referenced deeper on the string stack, its back link is reset
 * to point to the deepest string stack reference.  If it is a bound string not
 * deeper on the string stack and not identical to the input string, its back
 * link is set to zero, making it garbage.  If it is an external string, its
 * PSA in the variable is simply written over by that popped from the string 
 * stack.
 */
FCode (p4_str_store)
{
  PStr **addr, **strsp, *oldstr, *newstr;
  int oldext, newext;	/* true if old/new strings external */ 
  char **backlink;
  char *next;

  size_t len;
  char *buf;

  addr = (PStr **) *SP++;
  oldstr = *addr;
  oldext = (oldstr < (PStr *) SBUFFER || oldstr >= (PStr *) SBREAK);

  strsp = SSP;	/* not ready to pop if copy causes garbage collection */
  if (strsp == SSP0)
    p4_throw (P4_ON_SSTACK_UNDERFLOW);
  newstr = *strsp;
  newext = (newstr < (PStr *) SBUFFER || newstr>= (PStr *) SBREAK);

  if ( !(oldext && newext) && !(!oldext && newstr == oldstr) )
    { /* We know the new string is not bound to our variable. */ 

      /* Do old string first; it might open a garbage hole, more room
	 in case new string is copied. */
      if (!oldext)
	{
	  backlink = (char **)((size_t) oldstr - PFE_SIZEOF_CELL);
	  next = NULL;		  /* garbage if not on stack */
	  while (++strsp < SSP0)  /* no need to check first stack item */
	    if (*strsp == oldstr) next = (char*) strsp;
	  *backlink = next;
	  if (!next)
	    GARBAGE_FLAG = ~0;
	}

      if (!newext)
	{
	  backlink = (char **) ((size_t) newstr - PFE_SIZEOF_CELL); 
	  if (*backlink < (char*) SSP || *backlink >= (char*) SSP0)
	    { /* New string is bound to a different variable, copy it. */

	      Q_CAT;
	      len = newstr->count;
	      Q_ROOM (SBREAK, len + SIZEOF_DSTR_HEADER);
	      /* garbage possibly collected */

	      backlink = (char**) SBREAK;
	      SBREAK->count = len;

	      buf = &(SBREAK->body);
	      (char*) newstr = (char*) *SSP + SIZEOF_SCOUNT;

	      while (len-- > 0)			/* copy string body */
		*buf++ = *((char*) newstr)++;
 
	      (char*) newstr = (char*) ALIGNTO_CELL (buf);  /* null fill */
	      while (buf < (char*) newstr) 
		*buf++ = 0;

	      SBREAK = (DStr*) buf;
	      newstr = (PStr*) (backlink + offsetof (DStr, count));
	    }
	  *backlink = (char*) addr;
	}
    }
  *addr = newstr;
  SSP += 1;	/* now I tin pop */
}

/** $.		( $: a$ -- )
 * Display the string on the terminal. If the system implementation of TYPE
 * has its output vectored, $. uses the same vector.
 * <ansref>"string-dot"</ansref>
 */
FCode (p4_str_dot)
{
  PStr *str = p4_pop_str ();

  p4_type (SADDR (str), SLEN (str));
}

/** $@		( $var.pfa -- $: a$ )
		($: -- $ )
 * Leave the PSA of the string held by the string variable.  
 * <ansref>"string-fetch"</ansref>
 */
FCode (p4_str_fetch)
{
  PUSH_STR ((PStr*) *(char**) (*SP++));
}

/** $TYPE	($: a$ -- )
 *  Display the string on the terminal.  A =>'$.' synonym.  
 * <ansref>"string-type"</ansref>
 */

/** CAT		($: a$ -- )
 * Append the string body to the end of the string currently 
 * being concatenated as the last string in the string buffer, 
 * and update its count field.  If there is no concatenating 
 * string, start one.  An error is thrown if there is not
 * enough room in string space even after a garbage collection. 
 * When there is a concatenating string, CAT is the only basic 
 * string operation that can copy a string into the string buffer. 
 * Pushes onto the string stack without copy are still allowed. 
 * <ansref>"cat"</ansref>

 * NOTE: It is left to the user to define special concatenating 
 * words like:
    : \n-cat  ( -- )  \n$ cat ;
 */
FCode (p4_cat)
{
  char *p,*q;
  size_t len, delta = *(PCount*) *SSP;

  if (SSP == SSP0)
    p4_throw (P4_ON_SSTACK_UNDERFLOW);

  if (!CAT_STR)		/* copy first string */
    {
      Q_ROOM (SBREAK, delta + SIZEOF_DSTR_HEADER - PFE_SIZEOF_CELL);
      (PStr *) p = p4_pop_str ();	/* pop ok after gc */
      (PCount *) p += 1;
      (DStr *) q = SBREAK;   
      *((PStr **) q)++ = (PStr *) &CAT_STR;	/* back link */
      CAT_STR = (PStr *) q;			/* forward link */
      *((PCount *) q)++ = delta; 
      while (delta-- > 0)
	*q++ = *p++;
    }
  else			/* append next string */
    {
      len = SLEN (CAT_STR);
      Q_ROOM (CAT_STR, SIZEOF_SCOUNT + len + delta - PFE_SIZEOF_CELL);
      (PStr *) p = p4_pop_str ();	/* pop ok after gc */
      (PCount *) p += 1;
      q = (char *) CAT_STR;
      *(PCount *) q = len + delta;
      q += len + SIZEOF_SCOUNT;
      while (delta-- > 0)
	*q++ = *p++;
    }

  /* null fill */
  (char*) p = (char*) ALIGNTO_CELL (q);
  while (q < p)
    *q++ = 0;
  SBREAK = (DStr *) q;
}

/** ENDCAT	( -- $: cat$ )
 * If there is no concatenating string, do nothing.  If there is, 
 * leave it as a string bound to the string stack, and terminate 
 * concatenation, permitting normal copies into the string buffer. 
 * <ansref>"end-cat"</ansref>
*/
FCode (p4_endcat)
{
  PUSH_STR (CAT_STR);
  *(char **) (CAT_STR - 1) = (char *) SSP; 
  CAT_STR = NULL;
}


/************************************************************************/
/* string frames							*/
/************************************************************************/

/** $FRAME		( u -- )
 * Push the description of a string stack frame starting at the 
 * top of the string stack and containing u entries onto the 
 * string frame stack.  Errors are thrown if the frame stack 
 * would overflow or if the depth of the string stack above the
 * top frame, if there is one, is less than u.  The value u = 0
 * is allowed.  <ansref>"string-frame"</ansref>

 * NOTE: The current implementation pushes u and the string stack pointer
 * onto the frame stack.
 */
FCode (p4_str_frame)
{
  p4_make_str_frame (*SP++);
}

/** DROP-$FRAME		( -- )
 * Drop the topmost string frame from the string frame stack 
 * and string stack.  Errors are thrown if either stack would 
 * underflow or if the string frame does not begin at the top 
 * of the string stack.  The case where the frame has zero entries
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

/** FIND-ARG	( s -- i true | false )
 * Leave true and its index in the top string frame if the 
 * Forth string matches an element of the frame, else leave 
 * false.  The index of the top frame element is zero.  
 * <ansref>"find-arg"</ansref>
 */
FCode (p4_find_arg)
{
  p4cell i;

  if ( (i = p4_find_arg ((char *) SP[1], SP[0])) >= 0)
    {
      SP[1] = i;
      SP[0] = ~0;
    }
  else
    {
      *++SP = 0;
    }
}

/** "(DROP-$FRAME)" ( -- )
 * cleanupcode for the end of a definition that used =>"ARGS{".
 * =>";"-semicolon should be overloaded to compile it automatically
 * if dynamic string arguments were in use.
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

/************************************************************************/
/* debugging								*/
/************************************************************************/

FCode (per_str_space)
{
  *--SP = DSTRINGS->size;
}

FCode (per_str_space_header)
{
  *--SP = sizeof (StrSpace);
}
FCode (str_break)
{
  *--SP = (p4cell)SBREAK;
}

FCode (str_buffer)
{
  *--SP = (p4cell)SBUFFER;
}

FCode (str_sp)
{
  *--SP = (p4cell)SSP;
}

FCode (str_sp0)
{
  *--SP = (p4cell)SSP0;
}

FCode (num_frames)
{
  *--SP = DSTRINGS->numframes;
}

FCode (per_frame_stack)
{
  *--SP = sizeof (StrFrame) * DSTRINGS->numframes;
}

FCode (sf_break)
{
  *--SP = (p4cell)SFBREAK;
}

FCode (sf_sp)
{
  *--SP = (p4cell)SFSP;
}

FCode (sf_sp0)
{
  *--SP = (p4cell)SFSP0;
}

FCode (zero_str_space)
{
  p4_clear_str_space (DSTRINGS);
}

/** "ENVIRONMENT DSTRINGS-EXT" ( -- datecoded-version )
 * an => ENVIRONMENT constant to be checked with =>"ENVIRONMENT?"
 * the value is currently encoded as a datecode with a decimal
 * printout of format lik YYMMDD
 */

/** "ENVIRONMENT /SCOPY" ( -- MAX_SCOUNT )
 * an => ENVIRONMENT constant to be checked with =>"ENVIRONMENT?"
 * returns the configuration value of MAX_SCOUNT
 */

/** "ENVIRONMENT /DYNAMIC-STRING" ( -- MAX_DSCOUNT )
 * an => ENVIRONMENT constant to be checked with =>"ENVIRONMENT?"
 * returns the configuration value of MAX_DSCOUNT
 */

P4_LISTWORDS (dstrings) =
{
  /* constants */
  CO ("EMPTY$",		p4_empty_str),
  CO ("\\n$",		p4_newline_str),
  /* variables */
  DV ("DSTRINGS",	dstrings),
  /* forth string extensions */
  CO ("S,",		p4_s_comma),
  /* string space */
  CO ("0STRINGS",	p4_zero_strings),
  CO ("$GC-OFF",	p4_str_gc_off),
  CO ("$GC-ON",		p4_str_gc_on),
  CO ("$UNUSED",	p4_str_unused),
  CO ("COLLECT-$GARBAGE", p4_collect_str_garbage),
  CO ("MAKE-$SPACE",	p4_make_str_space),
  /* string compilation */
  CS ("$\"",		p4_str_quote),
  CO ("$CONSTANT",	p4_str_constant),
  CO ("$VARIABLE",	p4_str_variable),
  CI ("($:",		p4_paren),
  CI ("ARGS{",		p4_args_brace),
  CS ("M\"",		p4_m_quote),
  CS ("M'",		p4_m_tick),
  /* string stack */
  CO ("$DEPTH",		p4_str_depth),
  CO ("$DROP",		p4_str_drop),
  CO ("$DUP",		p4_str_dup),
  CO ("$OVER",		p4_str_over),
  CO ("$SWAP",		p4_str_swap),
  CO ("$S>",		p4_str_s_from),
  CO ("$S>-COPY",	p4_str_s_from_copy),
  CO ("$S@",		p4_str_s_fetch),
  CO (">$S-COPY",	p4_to_str_s_copy),
  CO (">$S",		p4_to_str_s),
  /* string manipulation */
  CO ("$!",		p4_str_store),
  CO ("$.",		p4_str_dot),
  CO ("$@",		p4_str_fetch),
  CO ("$TYPE",		p4_str_dot),
  CO ("CAT",		p4_cat),
  CO ("ENDCAT",		p4_endcat),
  /* string frames */
  CO ("$FRAME",		p4_str_frame),
  CO ("DROP-$FRAME",	p4_drop_str_frame),
  CO ("FIND-ARG",	p4_find_arg),
  CS ("(DROP-$FRAME)",  p4_do_drop_str_frame),
  /* debugging */
  CO ("/$SPACE",	per_str_space),
  CO ("/$SPACE-HEADER",  per_str_space_header),
  CO ("$BREAK",		str_break),
  CO ("$BUFFER",	str_buffer),
  CO ("$SP",		str_sp),
  CO ("$SP0",		str_sp0),
  CO ("#FRAMES",	num_frames),
  CO ("/FRAME-STACK",	per_frame_stack),
  CO ("$FBREAK",	sf_break),
  CO ("$FSP",		sf_sp),
  CO ("$FSP0",		sf_sp0),
  CO ("0$SPACE",	zero_str_space),
  P4_INTO ("ENVIRONMENT", 0),
  P4_OCON ("DSTRINGS-EXT",	10219),
  P4_OCON ("/SCOPY",		MAX_SCOUNT ),
  P4_OCON ("/DYNAMIC-STRING",	MAX_DSCOUNT ),
};
P4_COUNTWORDS (dstrings, "Dynamic-Strings extension");

/* if PFE_WITH_DSTRINGS_EXT */
#endif

/* 
 * Local variables:
 * c-file-style: "gnu"
 * End:
 */
