/**
 * -- usually implemented words.
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.4 $
 *     (modified $Date: 2008-04-20 04:46:29 $)
 *
 *  @description
 *              There are lots of useful words that do not appear
 *              in any standard. This wordset defines some of them.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) =
"@(#) $Id: forth-usual-ext.c,v 1.4 2008-04-20 04:46:29 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-limits.h>

#include <pfe/os-string.h>
#include <pfe/os-ctype.h>

#include <pfe/def-comp.h>
#include <pfe/dict-sub.h>
#include <pfe/_missing.h>
#include <pfe/logging.h>

/* ---------------------------------------------------------------------- *
 * quite a few words have their predecessor being described in
 *      Forth Programmer's Handbook, Conklin and Rather
 * comments here are taken from Neil Bawd's toolbelt.txt
 */

/** C+!  ( n addr -- )
 *  Add the low-order byte of _n_ to the byte at _addr_,
 *  removing both from the stack.
 */
FCode (p4_c_plus_store)
{
    * (char*) SP[0] += (char) SP[1];
    SP += 2;
}

/*  VOCABULARY           ( 'name' -- )
 *  Create a word list _name_. Subsequent execution of _name_
 *  replaces the first word list in the search order with
 *  _name_. When _name_ is made the compilation word list, new
 *  definitions will be added to _name_'s list.
 *
 * there are a few specialties about vocabularies in pfe.
 */
extern FCode(p4_vocabulary);

_export p4_Wordl* p4_to_wordlist (p4xt xt)
{
    return (p4_Wordl*) P4_TO_BODY(xt);
}

/** ">WORDLIST" ( xt -- wordl* )
 * convert a =>"VOCABULARY"-xt into its wordlist reference
 * (as in win32forth)
 */
FCode (p4_to_wordlist)
{
    *SP = (p4cell) p4_to_wordlist ((void*)(*SP));
}

/* --------------------------------------------------------- *
 *       Common Use
 */

/** BOUNDS                ( str len -- str+len str )
 *  Convert _str len_ to range for DO-loop.
 : BOUNDS  ( str len -- str+len str )  OVER + SWAP ;
 */
FCode (p4_bounds)
{
    p4cell h = SP[1];

    SP[1] += SP[0];
    SP[0] = h;
}

/** OFF                   ( addr  -- )
 *  Store 0 at _addr_. Defined in f84 as =>"OFF". See antonym =>"ON!".
  : OFF  ( addr -- )  0 SWAP ! ;
 */
FCode (p4_off_store)
{
    *(p4cell*) (*SP++) = P4_FALSE;
}

/** ON!                    ( addr -- )
 *  Store -1 at _addr_. Defined in f83 as =>"ON". See antonym =>"OFF!".
  : ON!  ( addr -- )  -1 SWAP ! ;
 */
FCode (p4_on_store)
{
    *(p4cell*) (*SP++) = P4_TRUE;
}

/** APPEND                ( str len add2 -- )
 *  Append string _str len_ to the counted string at _addr_.
 *  a.k.a. =>"+PLACE" of the => PLACE family
 : APPEND   2DUP 2>R  COUNT +  SWAP MOVE ( ) 2R> C+! ;
 */
/** +PLACE                ( str len add2 -- )
 *  Append string _str len_ to the counted string at _addr_.
 *  a.k.a. =>"APPEND" (being a => SYNONYM now)
 : +PLACE   2DUP 2>R  COUNT +  SWAP MOVE ( ) 2R> C+! ;
 */
FCode (p4_append)
{
    p4_char_t* p = (p4_char_t*) SP[0];
    p4_memcpy (p + 1 + *p, (char*) SP[2], SP[1]);
    *p += (p4_char_t) SP[1];
    SP += 3;
}

/** APPEND-CHAR           ( char addr -- )
 *  Append _char_ to the counted string at _addr_.
 *  a.k.a. =>"C+PLACE" of the => PLACE family
 : APPEND-CHAR   DUP >R  COUNT  DUP 1+ R> C!  +  C! ;
 */
/** C+PLACE           ( char addr -- )
 *  Append _char_ to the counted string at _addr_.
 *  a.k.a. =>"APPEND-CHAR" (being a => SYNONYM now)
 : C+PLACE   DUP >R  COUNT  DUP 1+ R> C!  +  C! ;
 */
FCode (p4_append_char)
{
    p4_char_t* p = (p4_char_t*) SP[0];
    p[ 1 + *p ] = (p4_char_t) SP[1];
    *p += (p4_char_t) 1;
    SP += 2;
}

/** PLACE                 ( str len addr -- )
 *  Place the string _str len_ at _addr_, formatting it as a
 *  counted string.
 : PLACE  2DUP 2>R  1+ SWAP  MOVE  2R> C! ;
 : PLACE  2DUP C!   1+ SWAP CMOVE ;
 */
FCode (p4_place)
{
    p4char *p = (p4char *) SP[0];

    *p = SP[1];
    p4_memcpy (p + 1, (p4char *) SP[2], *p);
    SP += 3;
}


/* ---------------------------------------------------------------------- *
 * F83 like, e.g. F-PC EXTEND
 */

/** PERFORM ( addr -- ? )
 * see => @EXECUTE which reads better
  */

/** ?LEAVE ( cond -- )
 * leave a (innermost) loop if condition is true
 */
FCode (p4_question_leave)
{
    FX_COMPILE (p4_question_leave);
    FX_COMPILE_RP_DROP (3);
}
FCode_XE (p4_question_leave_execution)
{
    FX_USE_CODE_ADDR;
    if (*SP++)
    {
        IP = RP[2] - 1;
        FX_EXECUTE_RP_DROP  (3);
        FX_BRANCH;
    } else {
        FX_EXECUTE_RP_DROP_SKIPS;
    }
    FX_USE_CODE_EXIT;
}
P4COMPILES (p4_question_leave, p4_question_leave_execution,
	    P4_SKIPS_NOTHING, P4_DEFAULT_STYLE);

/** NOOP ( -- )
 * do nothing, used as a place-holder where
 * an execution word is needed
 */
extern FCode (p4_noop);

/** RP@ ( -- addr )
 * returns the return stack pointer
 example:
   : R@ RP@ @ ;
 */
FCode (p4_r_p_fetch)
{
    FX_COMPILE (p4_r_p_fetch);
}
FCode (p4_r_p_fetch_execution)
{
    FX_USE_CODE_ADDR;
    *--SP = (p4cell) RP;
    FX_USE_CODE_EXIT;
}
P4COMPILES (p4_r_p_fetch, p4_r_p_fetch_execution,
	    P4_SKIPS_NOTHING, P4_DEFAULT_STYLE);

/** RP! ( addr -- )
 * sets the return stack pointer, reverse of => RP@
 */
FCode (p4_r_p_store)
{
#  ifdef P4_RP_IN_VM
    RP = (p4xcode **) *SP++;
#  else
    FX_DROP; /* throw in SBR_THREADING mode ? */
#  endif
}

/** SP! ( ... addr -- )
 * sets the stack pointer, reverse of => SP@
 */
FCode (p4_s_p_store)
{
    SP = *(p4cell **) SP;
}

/** -ROT ( a b c -- c a b )
 * inverse of => ROT
 */
FCode (p4_dash_rot)
{
    p4cell h = SP[2];

    SP[2] = SP[0];
    SP[0] = SP[1];
    SP[1] = h;
}

/** CSET ( n addr -- )
 * set bits in byte at given address
 simulate:
   : CSET  TUCK @ SWAP OR SWAP ! ;
 */
FCode (p4_c_set)
{
    *(char *) SP[0] |= (char) SP[1];
    SP += 2;
}

/** CRESET ( n addr -- )
 *  reset bits in byte at given address
 simulate:
   : CRESET  TUCK @ SWAP NOT AND SWAP ! ;
 */
FCode (p4_c_reset)
{
    *(char *) SP[0] &= ~(char) SP[1];
    SP += 2;
}

/** CTOGGLE ( n addr -- )
 * toggle bits in byte at given address
 simulate:
   : CTOGGLE  TUCK @ SWAP XOR SWAP ! ;
 */
FCode (p4_c_toggle)
{
    *(char *) SP[0] ^= (char) SP[1];
    SP += 2;
}

/** TOGGLE ( c-addr charmask -- )
 * toggle the bits given in charmask, see also => SMUDGE and = UNSMUDGE
 example: the fig-style SMUDGE had been defined such
   : FIG-SMUDGE LATEST >FFA (SMUDGE#) TOGGLE ;
 */
FCode (p4_toggle)
{
    *(p4char *) SP[1] ^= (p4char) SP[0];
    SP += 2;
}

/** 3DUP                ( x y z -- x y z x y z )
 *  Copy top three elements on the stack onto top of stack.
 : 3DUP   THIRD THIRD THIRD ;
 *
 * or
 : 3DUP  3 PICK 3 PICK 3 PICK ;
 */
FCode (p4_three_dup)
{
    SP -= 3;
    SP[0] = SP[3];
    SP[1] = SP[4];
    SP[2] = SP[5];
}


/** 3DROP               ( x y z -- )
 *  Drop the top three elements from the stack.
 : 3DROP   DROP 2DROP ;
 */
FCode (p4_three_drop)
{
    FX_3DROP;
}

/** 4DUP ( a b c d -- a b c d a b c d )
 simulate:
  : 4DUP  4 PICK 4 PICK 4 PICK 4 PICK ;
 */
FCode (p4_four_dup)
{
    SP -= 4;
    SP[0] = SP[4];
    SP[1] = SP[5];
    SP[2] = SP[6];
    SP[3] = SP[7];
}


/** 4DROP               ( x y z -- )
 *  Drop the top three elements from the stack.
 : 4DROP   2DROP 2DROP ;
 */
FCode (p4_four_drop)
{
    FX_4DROP;
}

/** TOUPPER ( c1 -- c2 )
 * convert a single character to upper case
   : TOUPPER  >R _toupper ;
 *
 * OLD: this was also called UPC up to PFE 0.33.x
 */
FCode (p4_toupper)
{
    *SP = toupper (*SP);
}

/** UPPER ( addr cnt -- )
 * convert string to upper case
 simulate:
   : UPPER  0 DO  DUP I +  DUP C@ UPC SWAP C!  LOOP  DROP ;
 */
FCode (p4_upper)
{
    p4_upper ((p4_char_t *) SP[1], SP[0]);
    FX_2DROP;
}

/** LOWER ( addr cnt -- )
 * convert string to lower case
 * This is not in L&P's F83 but provided for symmetry
 simulate:
   : LOWER  0 DO  DUP I +  DUP C@ >R _tolower SWAP C!  LOOP  DROP ;
 */
FCode (p4_lower)
{
    p4_lower ((p4_char_t *) SP[1], SP[0]);
    FX_2DROP;
}

/** ASCII ( [word] -- val )
 * state smart version of => CHAR or => [CHAR] resp.
 simulate:
   : ASCII  [COMPILE] [CHAR]
            STATE @ IF [COMPILE] LITERAL THEN ;
 */
FCode (p4_ascii)
{
    p4_word_parseword (' '); *DP=0; /* PARSE-WORD-NOHERE */
    if (! PFE.word.len)
        p4_throw (P4_ON_INVALID_NAME);
    if (STATE)
    {
        FX_COMPILE (p4_ascii);
        FX_UCOMMA( *(p4char*) PFE.word.ptr);
    }else
        FX_PUSH( *(p4char*) PFE.word.ptr);
}
P4COMPILES (p4_ascii, p4_literal_execution,
  P4_SKIPS_CELL, P4_DEFAULT_STYLE);

/** CONTROL ( [word] -- val )
 * see =>'ASCII', but returns char - '@'
 simulate:
   : CONTROL  [COMPILE] [CHAR]  [CHAR] @ -
              STATE @ IF [COMPILE] LITERAL THEN ;
 */
FCode (p4_control)
{
    register p4ucell c;

    p4_word_parseword (' '); *DP=0; /* PARSE-WORD-NOHERE */
    if (! PFE.word.len)
        p4_throw (P4_ON_INVALID_NAME);
    c = *(p4char*) PFE.word.ptr;
    if ('@' <= c && c <= '_')
        c -= '@';
    if (STATE)
    {
        FX_COMPILE (p4_control);
        FX_UCOMMA (c);
    }else
        FX_PUSH (c);
}
P4COMPILES (p4_control, p4_literal_execution,
  P4_SKIPS_CELL, P4_DEFAULT_STYLE);

/** NUMBER? ( addr -- d flag )
 * convert counted string to number - used in inner interpreter
 * ( => INTERPRET ), flags if conversion was successful
 example:
   BL WORD  HERE NUMBER? 0= IF ." not a number " THEN .
 */
FCode (p4_number_question)
{
    p4_charbuf_t *p = (p4_charbuf_t *) *SP;

    SP -= 2;
    SP[0] = p4_number_question (P4_CHARBUF_PTR(p), P4_CHARBUF_LEN(p),
				(p4dcell *) &SP[1]);
}

/* ---------------------------------------------------------------------- */

/** VOCS ( -- )
 * list all vocabularies in the system
 simulate:
   : VOCS VOC-LINK @ BEGIN DUP WHILE
                           DUP ->WORDLIST.NAME @ ID.
                           ->WORDLIST.LINK @
                     REPEAT DROP ;
 */
FCode (p4_vocs)
{
    Wordl *wl = VOC_LINK;

    while (wl != NULL)
    {
        if (wl->nfa)
            p4_dot_name (wl->nfa);
        else
            p4_outs(". "); /* show that there are unnamed wordlists */
        wl = wl->prev;
    }
}

/* ------------------------------------------------------------- */


/** @EXECUTE ( xt -- ? )
 * same as => @ => EXECUTE , but checks for null as xt and
 * silently ignores it. Same as in most forths where defined.
 simulate:
   : @EXECUTE  @ ?DUP IF EXECUTE THEN ;
 *
 * OLD: this was also called PERFORM up to PFE 0.33.x
 */
FCode (p4_fetch_execute)
{
    p4xt xt = *(p4xt*) FX_POP;

    if (xt) { PFE.execute (xt); }
}

/** EMITS           ( n char -- )
 *  Emit _char_ _n_ times.
 : EMITS             ( n char -- )
    SWAP 0 ?DO  DUP EMIT  LOOP DROP ;
 * also compare
 : SPACES BL EMITS ;
 : SPACE BL EMIT ;
 */
FCode(p4_emits)
{
    p4_emits (SP[1], SP[0]);
    SP += 2;
}

/* -------------------------------------------------------
 *        Error Checking
 *                            These words should be tailored for your system.
 */

/** FILE-CHECK        ( n -- )
 *  Check for file access error.
 \ : FILE-CHECK    ( n -- )  THROW ;
 : FILE-CHECK      ( n -- )  ABORT" File Access Error " ;
 */
FCode (p4_file_check)
{
    p4cell d = FX_POP;
    if (d)
    {
        char* s = strerror(d);
        p4_throwstr (P4_ON_ABORT_QUOTE, s);
    }
}

/** MEMORY-CHECK      ( n -- )
 *  Check for memory allocation error.
 \ : MEMORY-CHECK  ( n -- )  THROW ;
 : MEMORY-CHECK    ( n -- )  ABORT" Memory Allocation Error " ;
 */
FCode (p4_memory_check)
{
    p4cell d = FX_POP;
    if (! d)
        p4_throw (P4_ON_OUT_OF_MEMORY);
}

/* ------------------------------------------------------- *
 *       Generally Useful
 */

/** ++                  ( addr -- )
 *  Increment the value at _addr_.
 : ++  ( addr -- )  1 SWAP +! ;
 */
FCode (p4_plus_plus)
{
    p4cell* p = (p4cell*) FX_POP;
    ++ *p;
}

/** @++                  ( addr -- addr' x )
 *  Fetch the value _x_ from _addr_, and increment the address
 *  by one cell.
 : @++  ( addr -- addr' x )  DUP CELL+ SWAP  @ ;
 */
FCode (p4_fetch_plus_plus)
{
    SP -= 1;
    SP[0] = *P4_INCC(SP[1], p4cell);
}

/** !++                  ( addr x -- addr' )
 *  Store the value _x_ into _addr_, and increment the address
 *  by one cell.
 : !++  ( addr x -- addr' )  OVER !  CELL+ ;
 */
FCode (p4_store_plus_plus)
{
    register p4cell d = FX_POP;
    *P4_INCC(SP[0], p4cell) = d;
}

P4_LISTWORDS (forth_usual) =
{
    P4_INTO ("FORTH", 0),

    /** the value for BackSpace to be used with => EMIT - compare with => BL */
    P4_OCoN ("#BACKSPACE-CHAR",      '\b'),

    P4_FXco ("C+!",          p4_c_plus_store),
    P4_FXco ("VOCABULARY",   p4_vocabulary),

    P4_FXco ("BOUNDS",       p4_bounds),
    P4_FXco ("OFF!",         p4_off_store),
    P4_FXco ("ON!",          p4_on_store),
    P4_FNYM ("OFF",          "OFF!"),
    P4_FNYM ("ON",           "ON!"),
    P4_FXco ("PLACE",        p4_place),
    P4_FXco ("+PLACE",       p4_append),
    P4_FXco ("C+PLACE",      p4_append_char),
    P4_FNYM ("APPEND",       "+PLACE"),
    P4_FNYM ("APPEND-CHAR",  "C+PLACE"),

    P4_FXco ("@EXECUTE",     p4_fetch_execute),
    P4_SXco ("?LEAVE",       p4_question_leave),
    P4_FXco ("NOOP",         p4_noop),
    P4_SXco ("RP@",          p4_r_p_fetch),
    P4_FXco ("RP!",          p4_r_p_store),
    P4_FXco ("SP!",          p4_s_p_store),
    P4_FXco ("-ROT",         p4_dash_rot),
    P4_FXco ("CSET",         p4_c_set),
    P4_FXco ("CRESET",       p4_c_reset),
    P4_FXco ("CTOGGLE",      p4_c_toggle),
    P4_FXco ("TOGGLE",       p4_toggle),

    P4_FXco ("3DUP",         p4_three_dup),
    P4_FXco ("3DROP",        p4_three_drop),
    P4_FXco ("4DUP",         p4_four_dup),
    P4_FXco ("4DROP",        p4_four_drop),

    P4_FXco ("TOUPPER",      p4_toupper),
    P4_FXco ("UPPER",        p4_upper),
    P4_FXco ("LOWER",        p4_lower),
    P4_SXco ("ASCII",        p4_ascii),
    P4_SXco ("CONTROL",      p4_control),
    P4_FXco ("NUMBER?",      p4_number_question),
    P4_FXco ("VOCS",         p4_vocs),

    P4_FXco ("EMITS",        p4_emits),
    P4_FXco ("FILE-CHECK",   p4_file_check),
    P4_FXco ("MEMORY-CHECK", p4_memory_check),

    P4_FXco ("++",           p4_plus_plus),
    P4_FXco ("@++",          p4_fetch_plus_plus),
    P4_FXco ("!++",          p4_store_plus_plus),
    P4_FNYM ("@+",	     "@++"),
    P4_FNYM ("!+",           "!++"),
    P4_SNYM("ENDIF",         "THEN"),

    P4_INTO ("EXTENSIONS", 0),
    P4_FXco (">WORDLIST",    p4_to_wordlist),
};
P4_COUNTWORDS (forth_usual, "Usual Forth extensions");

/*@}*/

/*
 * Local variables:
 * c-file-style: "stroustrup"
 * End:
 */
