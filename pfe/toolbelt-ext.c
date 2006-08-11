/** 
 *
 *  Copyright (C) 2000 - 2001 Guido U. Draheim <guidod@gmx.de>
 *  Copyright (C) 2005 - 2006 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.2 $
 *     (modified $Date: 2006-08-11 22:56:05 $)
 *
 * @description:
 *       Words as defined by Neil Bawd's toolbelt, quite a few of these 
 *       have been around for a while, invented and implemented independently.
 *       Some of these were also present as PFE's extensions words, and they
 *       are referenced here due to the fact that Neil Bawd's website 
 *       had been given quite some attention, hence these words should be 
 *       assembled in a wordset to clarify their behaviour is compatible. 
 *       Comments taken from toolbelt.txt
 *
 */
 
#define _P4_SOURCE 1
 
#include <pfe/pfe-base.h>
 
#include <pfe/os-ctype.h>
#include <pfe/os-string.h>

#include <pfe/def-words.h>
 
/* --------------------------------------------------------------------- *
 *    Forth Programmer's Handbook, Conklin and Rather
 */

/** [VOID]          ( -- flag )
 *  Immediate FALSE. Used to comment out sections of code.
 *  IMMEDIATE so it can be inside definitions.
 */
/*P4: IC(0) */

/** NOT                 ( x -- flag )
 *  Identical to `0=`, used for program clarity to reverse the
 *  result of a previous test.
 *
 *  WARNING: PFE's NOT uses bitwise complement =>'INVERT'
 *           instead of the logical complement =>'0=', so
 *           that loading TOOLBELT will change semantics.
 *  ... this difference in semantics has caused dpans94 to
 *  depracate the word. Only if TRUE is -1 it would be identical
 *  but not all words return -1 for true.
 */ 
extern FCode(p4_zero_equal);
 
/** [DEFINED]           ( "name" -- flag )
 *  Search the dictionary for _name_. If _name_ is found,
 *  return TRUE; otherwise return FALSE. Immediate for use in
 *  definitions.
  
 * [DEFINED] word       ( -- nfa|0 ) immediate
 * does check for the word using find (so it does not throw like => ' )
 * and puts it on stack. As it is immediate it does work in compile-mode
 * too, so it places its argument in the cs-stack then. This is most
 * useful with a directly following => [IF] clause, so that sth. like
 * an <c>[IFDEF] word</c> can be simulated through <c>[DEFINED] word [IF]</c>

 : [DEFINED] BL WORD FIND NIP ; IMMEDIATE
 */
extern FCode(p4_defined);
 
/** [UNDEFINED]         ( "name" -- flag )
 *  Search the dictionary for _name_. If _name_ is found,
 *  return FALSE; otherwise return TRUE. Immediate for use in
 *  definitions.
 *
 *  see => [DEFINED]
 */
extern FCode (p4_undefined);
 
/** C+!                 ( n addr -- )
 *  Add the low-order byte of _n_ to the byte at _addr_,
 *  removing both from the stack.
 */
extern FCode (p4_c_plus_store);
 
/** EMPTY               ( -- )
 *  Reset the dictionary to a predefined golden state,
 *  discarding all definitions and releasing all allocated
 *  data space beyond that state.
 */
FCode (p4_empty)
{
    p4_forget(FENCE);
}

/*  VOCABULARY          ( "name" -- )
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
extern FCode (p4_bounds);

/** OFF                   ( addr  -- )
 *  Store 0 at _addr_. See `ON`.
  : OFF  ( addr -- )  0 SWAP ! ;
 */
extern FCode (p4_off_store);

/** ON                    ( addr -- )
 *  Store -1 at _addr_. See `OFF`.
  : ON  ( addr -- )  -1 SWAP ! ;
 */
extern FCode (p4_on_store);

/** APPEND                ( str len add2 -- )
 *  Append string _str len_ to the counted string at _addr_.
 *  AKA `+PLACE`.
 : APPEND   2DUP 2>R  COUNT +  SWAP MOVE ( ) 2R> C+! ;
 */
extern FCode (p4_append);
 
/** APPEND-CHAR           ( char addr -- )
 *  Append _char_ to the counted string at _addr_.
 : APPEND-CHAR   DUP >R  COUNT  DUP 1+ R> C!  +  C! ;
 */
extern FCode (p4_append_char);
 
/** PLACE                 ( str len addr -- )
 *  Place the string _str len_ at _addr_, formatting it as a
 *  counted string.
 : PLACE  2DUP 2>R  1+ SWAP  MOVE  2R> C! ;
 : PLACE  2DUP C!   1+ SWAP CMOVE ;
 */
extern FCode (p4_place);
 
/** STRING,               ( str len -- )
 *  Store a string in data space as a counted string.
 : STRING, HERE  OVER 1+  ALLOT  PLACE ;
 */
extern FCode (p4_parse_comma);
 
/** ,"                    ( "<ccc><quote>" -- )
 *  Store a quote-delimited string in data space as a counted
 *  string.
 : ," [CHAR] " PARSE  STRING, ; IMMEDIATE
 */
extern FCode (p4_parse_comma_quote);

/* ------------------------------------------------------- *
 *        Stack Handling
 */

/** THIRD               ( x y z -- x y z x )
 *  Copy third element on the stack onto top of stack.
 : THIRD   2 PICK ;
 */
FCode (p4_third)
{
    /* FX_PUSH(SP[3]) leaves undefined in behaviour in C!! */
    register p4cell cell = SP[3];
    FX_PUSH (cell);
}
 
/** FOURTH              ( w x y z -- w x y z w )
 *  Copy fourth element on the stack onto top of stack.
 : FOURTH  3 PICK ;
 */
FCode (p4_fourth)
{
    /* FX_PUSH(SP[4]) leaves undefined in behaviour in C!! */
    register p4cell cell = SP[4];
    FX_PUSH (cell);
}

/** 3DUP                ( x y z -- x y z x y z )
 *  Copy top three elements on the stack onto top of stack.
 : 3DUP   THIRD THIRD THIRD ;
 *
 * or
 : 3DUP  3 PICK 3 PICK 3 PICK ;
 */
extern FCode (p4_three_dup);
 
/** 3DROP               ( x y z -- )
 *  Drop the top three elements from the stack.
 : 3DROP   DROP 2DROP ;
 */
extern FCode (p4_three_drop);

/** 2NIP                ( w x y z -- y z )
 *  Drop the third and fourth elements from the stack.
 : 2NIP   2SWAP 2DROP ;
 */
FCode (p4_two_nip)
{
    SP[2] = SP[0];
    SP[3] = SP[1];
    SP += 2;
}

/** R'@                 ( -- x )( R: x y -- x y )
 *  The second element on the return stack.
 : R'@   S" 2R@ DROP " EVALUATE ; IMMEDIATE
 */
extern FCode(p4_r_tick_fetch);  /* misc-ext */

/* ------------------------------------------------------- 
 *        Short-Circuit Conditional
 */
 

/** ANDIF               ( p ... -- flag )
 *  Given `p ANDIF q THEN`,  _q_ will not be performed if
 *  _p_ is false.
 : ANDIF  S" DUP IF DROP " EVALUATE ; IMMEDIATE
 */
FCode (p4_andif)
{
    FX_COMPILE (p4_andif);
    FX (p4_ahead);
}
FCode_XE (p4_andif_execution)
{
    FX_USE_CODE_ADDR;
    if (! *SP)
    {
        FX_BRANCH;
    }else{
        IP++;
        FX_DROP;
    }
    FX_USE_CODE_EXIT;
}
P4COMPILES (p4_andif, p4_andif_execution,
    P4_SKIPS_OFFSET, P4_IF_STYLE);
 

/** ORIF                ( p ... -- flag )
 *  Given `p ORIF q THEN`,  _q_ will not be performed if
 *  _p_ is true.
 : ORIF   S" DUP 0= IF DROP " EVALUATE ; IMMEDIATE
 */
FCode (p4_orif)
{
    FX_COMPILE (p4_orif);
    FX (p4_ahead);
}
FCode_XE (p4_orif_execution)
{
    FX_USE_CODE_ADDR
    if (*SP)
    {
        FX_BRANCH;
    }else{
        IP++;
        FX_DROP;
    }
}
P4COMPILES (p4_orif, p4_orif_execution,
    P4_SKIPS_OFFSET, P4_IF_STYLE);

/* ------------------------------------------------------- *
 *        String Handling
 */

/** SCAN            ( str len char -- str+i len-i )
 *  Look for a particular character in the specified string.
 : SCAN     
    >R  BEGIN  DUP WHILE  OVER C@ R@ -
        WHILE  1 /STRING  REPEAT THEN
    R> DROP ;
 *
 * ie.
 * scan for first occurence of c in string 
   : SCAN >R BEGIN DUP OVER C@ R@ = 0= OR WHILE 
                    1- SWAP 1- SWAP REPEAT R> DROP ;
 */
FCode (p4_scan)			
{				
    char *p = (char *) SP[2];
    p4cell n = SP[1];
    char c = (char) *SP++;

    while (n && *p != c)
    {
        n--; p++;
    }
    SP[1] = (p4cell) p;
    SP[0] = n;
}
 
/** SKIP            ( str len char -- str+i len-i )
 *  Advance past leading characters in the specified string.
 : SKIP     
   >R  BEGIN  DUP WHILE  OVER C@ R@ =
        WHILE  1 /STRING  REPEAT THEN
    R> DROP ;
 *
 * ie.
 * skip leading characters c 
   : SKIP  >R BEGIN DUP OVER C@ R@ = OR WHILE 
                    1- SWAP 1- SWAP REPEAT R> DROP ;
 */
FCode (p4_skip)			
{			
    char *p = (char *) SP[2];
    p4cell n = SP[1];
    char c = (char) *SP++;

    while (n && *p == c)
    {    
        n--; p++;
    }
    SP[1] = (p4cell) p;
    SP[0] = n;
}

 
/** BACK            ( str len char -- str len-i )
 *  Look for a particular character in the string from the
 *  back toward the front.
 : BACK     
    >R  BEGIN  DUP WHILE
        1-  2DUP + C@  R@ =
    UNTIL 1+ THEN
    R> DROP ;
 */
FCode (p4_back)
{
    char *p = (char *) SP[2];
    p4cell n = SP[1];
    char c = (char) *SP++;

    p4cell i; 
    for (i = n; i ; i--)
    {
        if (p[i-1] == c)
            break;
    }
    SP[1] = (p4cell) p+i;
    SP[0] = (p4cell) n-i;
}
    
 
/** /SPLIT          ( a m a+i m-i -- a+i m-i a i )
 *  Split a character string _a m_ at place given by _a+i m-i_.
 *  Called "cut-split" because "slash-split" is a tongue
 *  twister.
 : /SPLIT  DUP >R  2SWAP  R> - ;
 */
FCode (p4_div_split)
{
    FX (p4_two_swap);
    SP[0] -= SP[2];
}


/** IS-WHITE        ( char -- flag )
 *  Test char for white space.
 : IS-WHITE   33 - 0< ;
 */
FCode (p4_is_white)
{
    *SP = (! isgraph ((p4char) *SP));
}
 
/** TRIM            ( str len -- str len-i )
 *  Trim white space from end of string.
 : TRIM    
    BEGIN  DUP WHILE
        1-  2DUP + C@ IS-WHITE NOT
    UNTIL 1+ THEN ;
 */
FCode (p4_trim)
{
    p4char* p = (p4char*) SP[1];
    p4cell n = SP[0];
    while (n && ! isgraph (p[n-1]))
    {
        n--;
    }
    SP[0] = n;
}
 
/** BL-SCAN         ( str len -- str+i len-i )
 *  Look for white space from start of string
 : BL-SCAN 
    BEGIN  DUP WHILE  OVER C@ IS-WHITE NOT
    WHILE  1 /STRING  REPEAT THEN ;
 */
FCode(p4_bl_scan)
{
    p4char* p = (p4char*) SP[1];
    p4cell n  = SP[0];
    int i = 0;
    while (i < n && isgraph (p[i]))
        i++;
        
    SP[1] += i;
    SP[0] -= i;
}

/** BL-SKIP         ( str len -- str+i len-i )
 *  Skip over white space at start of string.
 : BL-SKIP 
    BEGIN  DUP WHILE  OVER C@ IS-WHITE
    WHILE  1 /STRING  REPEAT THEN ;

 */
FCode(p4_bl_skip)
{
    p4char* p = (p4char*) SP[1];
    p4cell n  = SP[0];
    int i = 0;
    while (i < n && ! isgraph (p[i]))
        i++;
        
    SP[1] += i;
    SP[0] -= i;
}

/** STARTS?         ( str len pattern len2 -- str len flag )
 *  Check start of string.
 : STARTS?   DUP >R  2OVER  R> MIN  COMPARE 0= ;
 */
FCode (p4_starts_Q)
{
    p4cell n = FX_POP;
    if (SP[1] < n) { *SP = 0; return; }
    *SP = ! p4_memcmp ((char*) SP[2], (char*) SP[0], n);
}
 
/** ENDS?           ( str len pattern len2 -- str len flag )
 *  Check end of string.
 : ENDS?   DUP >R  2OVER  DUP R> - /STRING  COMPARE 0= ;
 */
FCode (p4_ends_Q)
{
    p4cell n = FX_POP;
    if (SP[1] < n) { *SP = 0; return; }
    *SP = ! p4_memcmp ((char*) SP[2] + (SP[1] - n), (char*) SP[0], n);
}

/* ------------------------------------------------------- *
 *       Character Tests
 */

/** IS-DIGIT        ( char -- flag )
 *  Test _char_ for digit [0-9].
 : IS-DIGIT   [CHAR] 0 -  10 U< ;
 */
FCode(p4_is_digit)
{
    *SP = isdigit ((p4char) *SP);
}
 
/** IS-ALPHA        ( char -- flag )
 *  Test _char_ for alphabetic [A-Za-z].
 : IS-ALPHA  32 OR  [CHAR] a -  26 U< ;
 */
FCode(p4_is_alpha)
{
    *SP = isalpha ((p4char) *SP);
}

/** IS-ALNUM        ( char -- flag )
 *  Test _char_ for alphanumeric [A-Za-z0-9].
 : IS-ALNUM  
    DUP IS-ALPHA  ORIF  DUP IS-DIGIT  THEN  NIP ;
 */
FCode (p4_is_alnum)
{
    *SP = p4_isalnum ((p4char) *SP);
}

/* ------------------------------------------------------- *
 *       Common Constants
 */

/** #BACKSPACE-CHAR     ( -- char )
 *  Backspace character.
 8 CONSTANT #BACKSPACE-CHAR
 */
/*P4: OC(8)*/

/** #CHARS/LINE         ( -- n )
 *  Preferred width of line in source files.  Suit yourself.
 62 VALUE    #CHARS/LINE
 */
/*P4: OC(80)*/

/** #EOL-CHAR           ( -- char )
 *  End-of-line character.  13 for Mac and DOS, 10 for Unix.
 13 CONSTANT #EOL-CHAR
 */
/*P4: OC(10)*/
#define EOL_CHAR '\n'
 
/** #TAB-CHAR           ( -- char )
 *  Tab character.
 9 CONSTANT #TAB-CHAR
 */
/*P4: OC(9) */

/** MAX-N               ( -- n )
 *  Largest usable signed integer.
 TRUE 1 RSHIFT        CONSTANT MAX-N
 */
/*P4: OC(xx)*/
 
/** SIGN-BIT            ( -- n )
 *  1-bit mask for the sign bit.
 TRUE 1 RSHIFT INVERT CONSTANT SIGN-BIT
 */
/*P4: OC(xx)*/

/** CELL                ( -- n )
 *  Address units (i.e. bytes) in a cell.
 1 CELLS CONSTANT CELL
 */
/*P4: OC(sizeof cell)*/
 
/** -CELL               ( -- n )
 *  Negative of address units in a cell.
 -1 CELLS CONSTANT -CELL
 */
/*P4: OC(- sizeof cell)*/

/* ------------------------------------------------------- *
 *       Filter Handling
 */

/** SPLIT-NEXT-LINE     ( src . -- src' . str len )
 *  Split the next line from the string.
 : SPLIT-NEXT-LINE 
    2DUP #EOL-CHAR SCAN  
    DUP >R  1 /STRING  2SWAP R> - ;
 * FIXME: inform Neil Bawd that this is probably
 * not what he wanted. replace /STRING with /SPLIT here. 
 */
FCode (p4_split_next_line)
{
    /*hmm, don't understand the code, so... no real optimizations here */
    p4cell R;
    
    FX (p4_two_dup);
    FX_PUSH (EOL_CHAR);
    FX (p4_scan);
    R = *SP;
    FX_PUSH (1);
    FX (p4_div_split);
    FX (p4_two_swap);
    *SP -= R;
}

/** VIEW-NEXT-LINE    ( src . str len -- src . str len str2 len2 )
 *  Copy next line above current line.
 : VIEW-NEXT-LINE 
    2OVER 2DUP #EOL-CHAR SCAN NIP - ;
 */
FCode (p4_view_next_line)
{
    /*hmm, don't understand the code, so... no real optimizations here */
    SP += 5;
    SP[1] = SP[3] = SP[7];
    SP[2] = SP[4] = SP[8];
    SP[0] = EOL_CHAR;
    FX (p4_scan);
    FX_NIP;
}
 
/** OUT                 ( -- addr )
 *   Promiscuous variable.
 VARIABLE OUT
 */
/*P4: DV(out)*/

/** TEMP                ( -- addr )
 *  Promiscuous variable.
 VARIABLE TEMP
 */
/*P4: OV(temp)*/

/* ------------------------------------------------------- *
 *       Input Stream
 */

/** NEXT-WORD             ( -- str len )
 *  Get the next word across line breaks as a character
 *  string. _len_ will be 0 at end of file.
 : NEXT-WORD         
    BEGIN   BL WORD COUNT      ( str len )
        DUP IF EXIT THEN
        REFILL
    WHILE  2DROP ( ) REPEAT ;  
 */
FCode (p4_next_word)
{
    do {
        if (p4_word_parseword (' ')) /* PARSE-WORD-NOHERE */
        {
	    *DP=0;
            FX_PUSH(PFE.word.ptr);
            FX_PUSH(PFE.word.len);
            return;
        }
    } while (p4_refill());
    FX_PUSH (0);
    FX_PUSH (0);
}

/** LEXEME                ( "name" -- str len )
 *  Get the next word on the line as a character string.
 *  If it's a single character, use it as the delimiter to
 *  get a phrase.
 : LEXEME             
    BL WORD ( addr) DUP C@ 1 =
        IF  CHAR+ C@ WORD  THEN
    COUNT ;
 */
FCode (p4_lexeme)
{
    p4_word_parseword (' '); /* PARSE-WORD-NOHERE >>> */
    if (PFE.word.len == 1)
	p4_word_parseword (*PFE.word.ptr); /* >>> PARSEWORD ++ NOHERE */
    *DP=0;
    FX_PUSH (PFE.word.ptr);
    FX_PUSH (PFE.word.len);
}

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
    p4dcell d;
    p4ucell base;
    
    p4_word_parseword (' '); *DP=0; /* PARSE-WORD-NOHERE */
    base = BASE; BASE = 16;
    if (p4_number_question (PFE.word.ptr, PFE.word.len, &d))
    {
        if (STATE)
        {
            FX_COMPILE (p4_literal);
            FX_UCOMMA (d.lo);
        }else{
            FX_PUSH (d.lo);
        }
    }else{
        p4_throwstr (P4_ON_ABORT_QUOTE, " Not A Hex Number");
    }
    BASE = base;
}
 
/** \\                    ( "...<eof>" -- )
 *  Ignore the rest of the input stream.
 : \\   BEGIN  -1 PARSE  2DROP  REFILL 0= UNTIL ;
 */
FCode (p4_backslash_backslash)
{
    /* while (p4_refill()) {} */
    switch (SOURCE_ID)
    {
    case -1:       /* evaluate */
    case 0:        /* QUERY or BLOCK */
	return;
    default:
	while (p4_next_line ()) {}
    }
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
extern FCode (p4_file_check);
 
/** MEMORY-CHECK      ( n -- )
 *  Check for memory allocation error.
 \ : MEMORY-CHECK  ( n -- )  THROW ;
 : MEMORY-CHECK    ( n -- )  ABORT" Memory Allocation Error " ;
 */
extern FCode (p4_memory_check);

/* ------------------------------------------------------- *
 *       Generally Useful
 */

/** ++                  ( addr -- )
 *  Increment the value at _addr_.
 : ++  ( addr -- )  1 SWAP +! ;
 */
extern FCode (p4_plus_plus);

/** @+                  ( addr -- addr' x )
 *  Fetch the value _x_ from _addr_, and increment the address
 *  by one cell.
 : @+  ( addr -- addr' x )  DUP CELL+ SWAP  @ ;
 */
extern FCode (p4_fetch_plus_plus);

/** !+                  ( addr x -- addr' )
 *  Store the value _x_ into _addr_, and increment the address
 *  by one cell.
 : !+  ( addr x -- addr' )  OVER !  CELL+ ;
 */
extern FCode (p4_store_plus_plus);

/* ------------------------------------------------------- *
 *       Miscellaneous
 */
 
/** 'th             ( n "addr" -- &addr[n] )
 *  Address `n CELLS addr +`.
 : 'th     ( n "addr" -- &addr[n] )
    S" 2 LSHIFT " EVALUATE
    BL WORD COUNT EVALUATE
    S" + " EVALUATE
    ; IMMEDIATE
 */
FCode (p4_tick_th)
{
    p4xt xt = p4_tick_cfa (FX_VOID);
    FX_COMPILE (p4_tick_th);
    FX_XCOMMA (xt);
}
FCode_XE (p4_tick_th_execution)
{   FX_USE_CODE_ADDR {
    p4cell cells = sizeof(p4cell) * FX_POP;
    p4xt   xt = (p4xt) P4_POP(IP);
    p4_call (xt);
    *SP += cells;
}}
P4COMPILES(p4_tick_th, p4_tick_th_execution,
    P4_SKIPS_TO_TOKEN, P4_DEFAULT_STYLE);
 
/** (.)             ( n -- addr u )
 *  Convert _n_ to characters, without punctuation, as for `.`
 *  (dot), returning the address and length of the resulting
 *  string.
 : (.)  ( n -- addr u )  DUP ABS 0 <# #S ROT SIGN #> ;
 */
FCode(p4_paren_dot)
{
    int len = sprintf ((char*) PAD, "%li", (p4celll) *SP);
    HLD = PAD + len;
    *SP = (p4cell) PAD;
    FX_PUSH (len);
}
 
/** CELL-               ( addr -- addr' )
 *  Decrement address by one cell
 : CELL-  ( addr -- addr' )  CELL - ;
 */
FCode(p4_cell_minus)
{
    *SP -= sizeof(p4cell);
}

/** EMITS           ( n char -- )
 *  Emit _char_ _n_ times.
 : EMITS             ( n char -- )
    SWAP 0 ?DO  DUP EMIT  LOOP DROP ;
 */
extern FCode(p4_emits);

/** HIWORD          ( xxyy -- xx )
 *  The high half of the value.
 : HIWORD  ( xxyy -- xx )  16 RSHIFT ;
 */
FCode (p4_hiword)
{
    *SP = (p4ucell) P4xW0(*SP);
}

/** LOWORD          ( xxyy -- yy )
 *  The low half of the value.
 : LOWORD  ( xxyy -- yy )  65535 AND ;
 */
FCode (p4_loword)
{
    P4xW0(*SP) = 0;
}

/** REWIND-FILE     ( file-id -- ior )
 *  Rewind the file.
 : REWIND-FILE       ( file-id -- ior )
    0 0 ROT REPOSITION-FILE ;
 */
FCode (p4_rewind_file)
{
    *SP = p4_reposition_file ((void*) *SP, 0);
}

P4_LISTWORDS(toolbelt) =
{
    /* INTO ("EXTENSIONS", 0 ) ? */
    P4_ICoN ("[VOID]",			0),
    P4_IXco ("[DEFINED]",		p4_defined),
    P4_IXco ("[UNDEFINED]",		p4_undefined),
    P4_FXco ("NOT",			p4_zero_equal),
    P4_FXco ("C+!",			p4_c_plus_store),
    P4_FXco ("EMPTY",			p4_empty),
    P4_FXco ("VOCABULARY",		p4_vocabulary),
    P4_FXco ("BOUNDS",			p4_bounds),
    P4_FXco ("OFF",			p4_off_store),
    P4_FXco ("ON",			p4_on_store),
    P4_FXco ("APPEND",			p4_append),
    P4_FXco ("APPEND-CHAR",		p4_append_char),
    P4_FXco ("PLACE",			p4_place),
    P4_FXco ("STRING,",			p4_parse_comma),
    P4_IXco (",\"",			p4_parse_comma_quote),
    P4_FXco ("THIRD",			p4_third),
    P4_FXco ("FOURTH",			p4_fourth),
    P4_FXco ("3DUP",			p4_three_dup),
    P4_FXco ("3DROP",			p4_three_drop),
    P4_FXco ("2NIP",			p4_two_nip),
    P4_FXco ("R'@",			p4_r_tick_fetch),
    P4_SXco ("ANDIF",			p4_andif),
    P4_SXco ("ORIF",			p4_orif),
    P4_FXco ("SCAN",			p4_scan),
    P4_FXco ("SKIP",			p4_skip),
    P4_FXco ("BACK",			p4_back),
    P4_FXco ("/SPLIT",			p4_div_split),
    P4_FXco ("IS-WHITE",		p4_is_white),
    P4_FXco ("TRIM",			p4_trim),
    P4_FXco ("BL-SCAN",			p4_bl_scan),
    P4_FXco ("BL-SKIP",			p4_bl_skip),
    P4_FXco ("STARTS?",			p4_starts_Q),
    P4_FXco ("ENDS?",			p4_ends_Q),
    P4_FXco ("IS-DIGIT",		p4_is_digit),
    P4_FXco ("IS-ALPHA",		p4_is_alpha),
    P4_FXco ("IS-ALNUM",		p4_is_alnum),
    P4_OCoN ("#BACKSPACE-CHAR", 	'\b'),
    P4_OCoN ("#CHARS/LINE",		80),
    P4_OCoN ("#EOL-CHAR", 		EOL_CHAR),
    P4_OCoN ("#TAB-CHAR", 		'\t'),
    P4_OCoN ("MAX-N",			CELL_MAX),
    P4_OCoN ("SIGN-BIT",		(1 << (sizeof(p4cell)-1))),
    P4_OCoN ("CELL",			sizeof(p4cell)),
    P4_OCoN ("-CELL",			- sizeof(p4cell)),
    P4_FXco ("SPLIT-NEXT-LINE", 	p4_split_next_line),
    P4_FXco ("VIEW-NEXT-LINE",		p4_view_next_line),
    P4_DVaR ("OUT",			out),
    P4_FXco ("NEXT-WORD",		p4_next_word),
    P4_FXco ("LEXEME",			p4_lexeme),
    P4_IXco ("H#",			p4_h_sh),
    P4_FXco ("\\\\",			p4_backslash_backslash),
    P4_FXco ("FILE-CHECK",		p4_file_check),
    P4_FXco ("MEMORY-CHECK",		p4_memory_check),
    P4_FXco ("++",			p4_plus_plus),
    P4_FXco ("@+",			p4_fetch_plus_plus),
    P4_FXco ("!+",			p4_store_plus_plus),
    P4_SXco ("'th",			p4_tick_th),
    P4_FXco ("(.)",			p4_paren_dot),
    P4_FXco ("CELL-",			p4_cell_minus),
    P4_FXco ("EMITS",			p4_emits),
    P4_FXco ("HIWORD",			p4_hiword),
    P4_FXco ("LOWORD",			p4_loword),
    P4_FXco ("REWIND-FILE",		p4_rewind_file),

    P4_INTO ("ENVIRONMENT", 0 ),
    P4_OCON ("TOOLBELT-EXT",		1999 ),
};
P4_COUNTWORDS(toolbelt, "TOOLBELT - Neil Bawd's common extensions");

/* 
 * Local variables:
 * c-file-style: "stroustrup"
 * End:
 */















