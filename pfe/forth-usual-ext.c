/** 
 * -- usually implemented words.
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE               @(#) %derived_by: guidod %
 *  @version %version: 5.10 %
 *    (%date_modified: Mon Mar 12 10:32:22 2001 %)
 *
 *  @description
 *              There are lots of useful words that do not appear
 *              in any standard. This wordset defines some of them.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: forth-usual-ext.c,v 0.33 2006-08-08 06:21:35 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-xtra.h>

#include <string.h>
#include <ctype.h>

#include <pfe/def-comp.h>
#include <pfe/_missing.h>
#include <pfe/logging.h>

/* ---------------------------------------------------------------------- *
 * Forth Programmer's Handbook, Conklin and Rather
 *
 * comments taken from Neil Bawd's toolbelt.txt
 */

/** [VOID]                ( -- flag )
 *  Immediate FALSE. Used to comment out sections of code.
 *  IMMEDIATE so it can be inside definitions.
 : [VOID] 0 ; immediate
 */

/** [DEFINED]             ( [name] -- flag )
 *  Search the dictionary for _name_. If _name_ is found,
 *  return TRUE; otherwise return FALSE. Immediate for use in
 *  definitions.
  
 * This word will actually return what => FIND returns (the NFA). 
 * does check for the word using find (so it does not throw like => ' )
 * and puts it on stack. As it is immediate it does work in compile-mode
 * too, so it places its argument in the cs-stack then. This is most
 * useful with a directly following => [IF] clause, so that sth. like
 * an <c>[IFDEF] word</c> can be simulated through <c>[DEFINED] word [IF]</c>

 : [DEFINED] BL WORD FIND NIP ; IMMEDIATE
 */
FCode (p4_bracket_defined)
{
    /* moved from useful-ext.c */
    char* p;
    p4ucell l;
    
    p4_parseword (' ', &p, &l);
    FX_PUSH (p4_find (p, l));
}
 
/** [UNDEFINED]          ( [name] -- flag )
 *  Search the dictionary for _name_. If _name_ is found,
 *  return FALSE; otherwise return TRUE. Immediate for use in
 *  definitions.
 *
 *  see => [DEFINED]
 */
FCode (p4_bracket_undefined)
{
    FX (p4_bracket_defined);
    FX (p4_zero_equal);
}
 
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
 *  Store 0 at _addr_. See `ON`.
  : OFF  ( addr -- )  0 SWAP ! ;
 */
FCode (p4_off)			
{
    *(p4cell*) (*SP++) = P4_FALSE;
}

/** ON                    ( addr -- )
 *  Store -1 at _addr_. See `OFF`.
  : ON  ( addr -- )  -1 SWAP ! ;
 */
FCode (p4_on)			
{
    *(p4cell*) (*SP++) = P4_TRUE;
}

/** APPEND                ( str len add2 -- )
 *  Append string _str len_ to the counted string at _addr_.
 *  AKA `+PLACE`.
 : APPEND   2DUP 2>R  COUNT +  SWAP MOVE ( ) 2R> C+! ;
 */
FCode (p4_append)
{
    unsigned char* p = (char*) SP[0];
    memcpy (p + 1 + *p, (char*) SP[2], SP[1]);
    *p += (unsigned char) SP[1];
    SP += 3;
}
 

/** APPEND-CHAR           ( char addr -- )
 *  Append _char_ to the counted string at _addr_.
 : APPEND-CHAR   DUP >R  COUNT  DUP 1+ R> C!  +  C! ;
 */
FCode (p4_append_char)
{
    unsigned char* p = (char*) SP[0];
    p[ 1 + *p ] = (unsigned char) SP[1];
    *p += (unsigned char) 1;
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
    memcpy (p + 1, (p4char *) SP[2], *p);
    SP += 3;
}

 
/** STRING,               ( str len -- )
 *  Store a string in data space as a counted string.
 : STRING, HERE  OVER 1+  ALLOT  PLACE ;
 */
FCode (p4_string_comma)
{
    p4_string_comma ((char*) SP[1], SP[0]);
    SP += 2;
}
 
/** PARSE,                    ( "chars<">" -- )
 *  Store a char-delimited string in data space as a counted
 *  string. As seen in Bawd's
 : ," [CHAR] " PARSE  STRING, ; IMMEDIATE
 *
 * this implementation is much different from Bawd's
 : PARSE, PARSE STRING, ;
 */
FCode (p4_parse_comma)
{
    p4_parse_comma (FX_POP);
}


/** PARSE,"                   ( "chars<">" -- )
 *  Store a quote-delimited string in data space as a counted
 *  string.
 : ," [CHAR] " PARSE  STRING, ; IMMEDIATE
 *
 * implemented here as
 : PARSE," [CHAR] " PARSE, ; IMMEDIATE
 */
FCode (p4_parse_comma_quote)
{
    p4_parse_comma ('"');
}

/** PARSE-WORD ( "chars" -- c-addr u )
 * the ANS'94 standard describes this word in a comment
 * under =>"PARSE", section A.6.2.2008 - quote:
 * 
 * Skip leading spaces and parse name delimited by a space. c-addr 
 * is the address within the input buffer and u is the length of the
 * selected string. If the parse area is empty, the resulting string 
 * has a zero length. 
 *
 * If both => PARSE and => PARSE-WORD are present, the need for => WORD is 
 * largely eliminated. 
 */
FCode (p4_parse_word)
{
    FX_2ROOM;
    p4_parseword (' ', (char**) SP+1, (p4ucell*) SP+0);
}

/* ---------------------------------------------------------------------- *
 * F83 like, e.g. F-PC EXTEND
 */

/** PERFORM ( addr -- ? ) 
  simulate:
    : PERFORM  @ EXECUTE 
  */
FCode (p4_perform)	
{		
    PFE.execute (*(p4xt *) *SP++);
}

/** ?LEAVE ( cond -- )
 * leave a (innermost) loop if condition is true
 */
FCode (p4_question_leave)
{
    if (*SP++)
    {
        IP = RP[2] - 1;
        RP += 3;
        FX_BRANCH;
    }
}

/** NOOP ( -- )
 * do nothing, used as a place-holder where 
 * an execution word is needed
 */
FCode (p4_noop)
{
}

/** RP@ ( -- addr ) 
 * returns the return stack pointer 
 example:
   : R@ RP@ @ ;
 */
FCode (p4_r_p_fetch)		
{				
    *--SP = (p4cell) RP;
}

/** RP! ( addr -- ) 
 * sets the return stack pointer, reverse of => RP@
 */
FCode (p4_r_p_store)		
{				
    RP = (p4xt **) *SP++;
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

/** UPC ( c1 -- c2 ) 
 * convert a single character to upper case 
   : UPC  >R _toupper ;
 */
FCode (p4_upc)			
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
    p4_upper ((char *) SP[1], SP[0]);
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
    p4_lower ((char *) SP[1], SP[0]);
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
    char *p;
    p4ucell n;

    p4_skip_delimiter (' ');
    p4_parse (' ', &p, &n);
    if (n == 0)
        p4_throw (P4_ON_INVALID_NAME);
    if (STATE)
    {
        FX_COMPILE (p4_ascii);
        FX_COMMA (*(p4char *) p);
    }else
        FX_PUSH ( *(p4char *) p);
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
    char *p;
    p4ucell c;
    p4ucell n;

    p4_skip_delimiter (' ');
    p4_parse (' ', &p, &n);
    if (n == 0)
        p4_throw (P4_ON_INVALID_NAME);
    c = *(p4char *) p;
    if ('@' <= c && c <= '_')
        c -= '@';
    if (STATE)
    {
        FX_COMPILE (p4_control);
        FX_COMMA (c);
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
    char *p = (char *) *SP;

    SP -= 2;
    SP[0] = p4_number_question (p + 1, *(p4char *) p, (p4dcell *) &SP[1]);
}

/* ---------------------------------------------------------------------- */

/** VOCS ( -- )
 * list all vocabularies in the system
 simulate:
   : VOCS VOC-LINK @ BEGIN DUP WHILE
                           DUP BODY> >NAME .NAME
                           ->VOC-LINK @
                     REPEAT DROP ; 
 */
FCode (p4_vocs)	
{
    Wordl *wl = VOC_LINK;

    while (wl != NULL)
    {
        p4_dot_name (p4_to_name (BODY_FROM (wl)));
        wl = wl->prev;
    }
}

/** ((DEFER)) ( -- )
 * runtime of => DEFER words
 */
FCode (p4_defer_RT)
{
    register p4xt xt;
    if ((xt = (p4xt) WP_PFA[0])) { PFE.execute (xt); }
# ifdef PFE_WITH_FIG
    else if ((xt = (p4xt) WP_PFA[1])) { PFE.execute (xt); }
# endif
    else    { P4_warn ("null execution in deferred word"); }
}

/** DEFER ( 'word' -- )
 * create a new word with ((DEFER))-semantics
 simulate:
   : DEFER  CREATE 0, DOES> ( the ((DEFER)) runtime ) 
      @ ?DUP IF EXECUTE THEN ;
   : DEFER  DEFER-RT HEADER 0 , ;
 *
 * declare as <c>"DEFER deferword"</c>  <br>
 * and set as <c>"['] executionword IS deferword"</c>
 * (in pfe, you can also use <c>TO deferword</c> to set the execution)
 */
FCode (p4_defer)
{
    p4_header (PFX(p4_defer_RT), 0);
    FX_COMMA (0);
# ifdef PFE_WITH_FIG
    FX_COMMA (0);
# endif
}
    
FCode (p4_is_execution)
{
    *P4_TO_DOES_BODY((*IP++)) = FX_POP;
}

/** IS ( xt-value [word] -- )
 * set a => DEFER word
 : IS ' 
   STATE @ IF LITERAL, POSTPONE >DOES-BODY POSTPONE ! 
   ELSE >DOES-BODY ! THEN 
 ; IMMEDIATE
 */
FCode (p4_is)
{
    p4xt xt = p4_tick_cfa ();
    if (STATE)
    {
	FX_COMPILE (p4_is);
	FX_COMMA (xt);
    }else{
	*P4_TO_DOES_BODY (xt) = FX_POP;
    }
}
P4COMPILES(p4_is, p4_is_execution, 
	   P4_SKIPS_TO_TOKEN, P4_DEFAULT_STYLE);

/* ------------------------------------------------------------- */


/** @EXECUTE ( xt -- ? )
 * same as => @ => EXECUTE , but checks for null as xt and
 * silently ignores it. Same as in most forths where defined.
 simulate:
   : @EXECUTE  @ ?DUP IF EXECUTE THEN ;
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
        p4_throws (P4_ON_ABORT_QUOTE, s, 0); 
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
 *       Input Stream
 */

/** H#                    ( "hexnumber" -- n )
 *  Get the next word in the input stream as a hex
 *  single-number literal.  (Adopted from Open Firmware.)
 : H#  ( "hexnumber" -- n )  \  Simplified for easy porting.
    0 0 BL WORD COUNT                  
    BASE @ >R  HEX  >NUMBER  R> BASE !
        ABORT" Not Hex " 2DROP          ( n)
    STATE @ IF  POSTPONE LITERAL  THEN
    ; IMMEDIATE
 */
FCode (p4_h_sh)
{
    char* p; p4ucell n;
    p4dcell d;
    p4ucell base;
    
    p4_parseword (' ', &p, &n);
    base = BASE; BASE = 16;
    if (p4_number_question (p, n, &d))
    {
        if (STATE)
        {
            FX_COMPILE (p4_literal);
            FX_COMMA (d.lo);
        }else{
            FX_PUSH (d.lo);
        }
    }else{
        p4_throws (P4_ON_ABORT_QUOTE, " Not A Hex Number", 0);
    }
    BASE = base;
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

/** @+                  ( addr -- addr' x )
 *  Fetch the value _x_ from _addr_, and increment the address
 *  by one cell.
 : @+  ( addr -- addr' x )  DUP CELL+ SWAP  @ ;
 */
FCode (p4_fetch_plus)
{
    FX_1ROOM;
    SP[0] = P4_POP_(p4cell, SP[1]);
}

/** !+                  ( addr x -- addr' )
 *  Store the value _x_ into _addr_, and increment the address
 *  by one cell.
 : !+  ( addr x -- addr' )  OVER !  CELL+ ;
 */
FCode (p4_store_plus)
{
    p4cell d = FX_POP;
    P4_POP_(p4cell, SP[0]) = d;
}

P4_LISTWORDS (forth_usual) =
{
    /** the value for BackSpace to be used with => EMIT - compare with => BL */
    OC ("#BACKSPACE-CHAR", 	'\b'),

    IC ("[VOID]",       0),
    CI ("[DEFINED]",    p4_bracket_defined),
    CI ("[UNDEFINED]",  p4_bracket_undefined),
    CO ("C+!",		p4_c_plus_store),
    CO ("VOCABULARY",   p4_vocabulary),

    CO ("BOUNDS",	p4_bounds),
    CO ("OFF",		p4_off),
    CO ("ON",		p4_on),
    CO ("APPEND",       p4_append),
    CO ("APPEND-CHAR",  p4_append_char),
    CO ("PLACE",	p4_place),
    CO ("STRING,",      p4_string_comma),
    CO ("PARSE,",       p4_parse_comma),
    CI ("PARSE,\"",     p4_parse_comma_quote),
    CO ("PARSE-WORD",   p4_parse_word),

    CO ("PERFORM",	p4_perform),
    CO ("?LEAVE",	p4_question_leave),
    CO ("NOOP",		p4_noop),
    CO ("RP@",		p4_r_p_fetch),
    CO ("RP!",		p4_r_p_store),
    CO ("SP!",		p4_s_p_store),
    CO ("-ROT",		p4_dash_rot),
    CO ("CSET",		p4_c_set),
    CO ("CRESET",	p4_c_reset),
    CO ("CTOGGLE",	p4_c_toggle),

    CO ("3DUP",         p4_three_dup),
    CO ("3DROP",        p4_three_drop),
    CO ("4DUP",		p4_four_dup),
    CO ("4DROP",        p4_four_drop),

    CO ("UPC",		p4_upc),
    CO ("UPPER",	p4_upper),
    CO ("LOWER",	p4_lower),
    CS ("ASCII",	p4_ascii),
    CS ("CONTROL",	p4_control),
    CO ("NUMBER?",	p4_number_question),
    CO ("VOCS",		p4_vocs),
    
    CO ("@EXECUTE",	p4_fetch_execute),
    CO ("DEFER",	p4_defer),
    CS ("IS",           p4_is),

    CO ("EMITS",        p4_emits),
    CO ("FILE-CHECK",   p4_file_check),
    CO ("MEMORY-CHECK", p4_memory_check),

    CO ("H#",           p4_h_sh),
    CO ("++",           p4_plus_plus),
    CO ("@+",           p4_fetch_plus),
    CO ("!+",           p4_store_plus),
};
P4_COUNTWORDS (forth_usual, "Usual Forth extensions");

/*@}*/

/* 
 * Local variables:
 * c-file-style: "stroustrup"
 * End:
 */
