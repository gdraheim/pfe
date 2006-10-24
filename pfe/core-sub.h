#ifndef _PFE_CORE_SUB_H
#define _PFE_CORE_SUB_H 1159392645
/* generated 2006-0927-2330 ../../pfe/../mk/Make-H.pl ../../pfe/core-sub.c */

#include <pfe/pfe-sub.h>

/** 
 * --  Subroutines for the Core Forth-System
 * 
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2006 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.5 $
 *     (modified $Date: 2006-10-24 00:54:08 $)
 *
 *  @description
 *         Subroutines for the Forth Core System - especially the
 *         general input/output routines like ACCEPT/QUERY/WORD/PARSE 
 *         and converters like UD.DR and >NUMBER 
 */

#ifdef __cplusplus
extern "C" {
#endif




/** QUERY ( -- )
 * source input:  read from terminal using => _accept_ with the
 * returned string to show up in => TIB of => /TIB size.
 */
extern P4_CODE (p4_query);

/**
 * return cell-aligned address
 */
_extern  P4_GCC_CONST p4cell p4_aligned (p4cell n) ; /*{*/

/** _strpush_ ( zstr* -- S: str* str# )
 * push a C-string onto the SP runtime-stack, as if => S" string" was used
 : _strpush_ s! _strlen_ s! ;
 */
_extern  void p4_strpush (const char *s) ; /*{*/

/** _pocket_ ( -- str* )
 * return the next pocket for interactive string input.
 : _pocket_ _pockets@_ _pocket@_ th  _pocket@_ 1+ _pockets#_ mod to _pocket@_ ;
 */
_extern  P4_GCC_MALLOC void* p4_pocket (void) ; /*{*/

/** _-trailing_ ( str* str# -- str#' )
 * chop off trailing spaces for the stringbuffer. returns the new length,
 * so for an internal counted string, use
   <x> dup count _-trailing_ c!
   : _-trailing_ begin dup while 
      2dup + c@ bl <> if nip exit then 
      1- repeat nip ;
 */
_extern  P4_GCC_WARN_UNUSED_RESULT int p4_dash_trailing (p4_char_t *s, int n) ; /*{*/

/** _lower_ ( str* str# -- )
 * _tolower_ applied to a stringbuffer
 : _lower_ 0 do dup c@ _tolower_ over c! 1+ loop drop ;
 */
_extern  void p4_lower (p4_char_t *p, int n) ; /*{*/

/** _upper_ ( str* str# -- )
 * _toupper_ applied to a stringbuffer
 : _upper_ 0 do dup c@ _toupper_ over c! 1+ loop drop ;
 */
_extern  void p4_upper (p4_char_t *p, int n) ; /*{*/

/** _zplaced_ ( str* str# dst* max# -- dst* ) [alias] _store_c_string_
 * copy stringbuffer into a field as a zero-terminated string.
 : _zsplaced_ rot 2dup > if drop 1- else nip then _zplace_ ;
 */
_extern  char* p4_store_c_string (const p4_char_t *src, int n, char *dst, int max) ; /*{*/

/** _pocket_zplaced ( str* str# -- pocket* ) [alias] _pocket_c_string_
 * store a string-span as a zero-terminated string into another pocket-pad
 : _pocket_zplaced _pocket_ _/pocket_ _zplaced_ ;
*/
_extern  P4_GCC_MALLOC char* p4_pocket_c_string (const p4_char_t* src, int n) ; /*{*/

/** _zplaced_filename_ ( str* str# dst* max# -- dst* ) [alias] _store_filename_
 * copy stringbuffer into a field as a zero-terminated filename-string,
 * a shell-homedir like "~username" will be expanded, and the
 * platform-specific dir-delimiter is converted in on the fly ('/' vs. '\\')
 */
_extern  char* p4_store_filename (const p4_char_t* str, int n, char* dst, int max) ; /*{*/

/** _pocket_fileame_ ( str* str# -- dst* )
 * a new pocket with the given filename as asciiz
 : _pocket_filename_ _pocket_ /pocket _zplaced_filename_
 */
_extern  P4_GCC_MALLOC char* p4_pocket_filename (const p4_char_t* src, int n) ; /*{*/

_extern  char* p4_pocket_expanded_filename (const p4_char_t *nm, int ln, const char *paths, const char *exts) ; /*{*/

/** _search_ ( str* str# key* key# -- 0 | key-in-str* )
 * search for substring p2/u2 in string p1/u1, returns null if not found
 * or a pointer into str*,str# that has lenght of key# 
 */
_extern  char * p4_search (const char *p1, int u1, const char *p2, int u2) ; /*{*/

/** _match_ ( zpattern* zstring* ignorecase? -- yes? )
 * Match string against pattern.
 * Pattern knows wildcards `*' and `?' and `\' to escape a wildcard.
 */
_extern  int p4_match (const char *pattern, const char *string, int ic) ; /*{*/

/** _U/_
 * unsigned divide procedure, single prec 
 */
_extern  P4_GCC_CONST udiv_t p4_udiv (p4ucell num, p4ucell denom) ; /*{*/

/** _/_
 * floored divide procedure, single prec 
 */
_extern  P4_GCC_CONST fdiv_t p4_fdiv (p4cell num, p4cell denom) ; /*{*/

/** _ud/_
 * Divides *ud by denom, leaves result in *ud, returns remainder.
 * For number output conversion: dividing by BASE.
 */
_extern  p4ucell p4_u_d_div (p4udcell *ud, p4ucell denom) ; /*{*/

/** _ud*_
 * Computes *ud * w + c, where w is actually only half of a cell in size.
 * Leaves result in *ud.
 * For number input conversion: multiply by BASE and add digit.
 */
_extern  void p4_u_d_mul (p4udcell *ud, p4ucell w, p4ucell c) ; /*{*/

/** _dig>num_ ( c n* base -- ?ok )
 * Get value of digit c into *n, return flag: valid digit.
 */
_extern  int p4_dig2num (p4_char_t c, p4ucell *n, p4ucell base) ; /*{*/

/** _num2dig_ ( val -- c )
 * make digit 
 */
_extern  P4_GCC_CONST char p4_num2dig (p4ucell n) ; /*{*/

/** _hold_ ( c -- )
 * insert into pictured numeric output string
 */
_extern  void p4_hold (char c) ; /*{*/

/** _>number_
 * try to convert into numer, see => >NUMBER
 */
_extern  const p4_char_t * p4_to_number (const p4_char_t *p, p4ucell *n, p4udcell *d, p4ucell base) ; /*{*/

/** _?number_ ( str* str# dcell* -- ?ok )
 * try to convert into number, see => ?NUMBER
 */
_extern  int p4_number_question (const p4_char_t *p, p4ucell n, p4dcell *d) ; /*{*/

/** _ud.r_ ( d,d str* str# base -- str* )
 * This is for internal use only (SEE and debugger),
 * The real => UD.R etc. words uses => HOLD and the memory area below => PAD
 */
_extern  char * p4_str_ud_dot_r (p4udcell ud, char *p, int w, int base) ; /*{*/

/** _d.r_ ( d,d str* str# base -- str* )
 * This is for internal use only (SEE and debugger),
 * The real => UD.R etc. words use => HOLD and the memory area below => PAD
 */
_extern  char * p4_str_d_dot_r (p4dcell d, char *p, int w, int base) ; /*{*/

/** _._ ( i str* str# base -- str* )
 * This is for internal use only (SEE and debugger),
 * The real => . etc. words use => HOLD and the memory area below => PAD
 */
_extern  char * p4_str_dot (p4cell n, char *p, int base) ; /*{*/

/** _outc_ ( char -- ) [alias] _outc
 * emit single character,   
 * (output adjusting the => OUT variable, see => _putc_ to do without)
 : _emit_ _putc_ _?xy_ drop out ! ;
 */
_extern  void p4_outc (char c) ; /*{*/

/** _ztype_ ( zstr* -- ) [alias] _outs
 * type a string
 * (output adjusting the => OUT variable, see => _puts_ to do without)
 : _ztype_ _puts_ _?xy_ drop out ! ;
 */
_extern  void p4_outs (const char *s) /* type a string */ ; /*{*/

_extern  P4_GCC_PRINTF int p4_outf (const char *s,...); /*;*/

/** _type_ ( str* str# -- )
 * type counted string to terminal
 * (output adjusting the => OUT variable, see => _puts_ and => _outs_ )
 : _type_ 0 do c@++ _putc_ loop drop _flush_ _?xy drop out ! ;
 */
_extern  void p4_type (const p4_char_t *str, p4cell len) ; /*{*/

/** _typeline_ ( str* str# -- )
 * type counted string to terminal, if it does not fit in full on
 * the current line, emit a => CR before
 * (output adjusting the OUT variable, see => _type_ and => _outs_ )
 : _typeline_ out @ over + cols @ > if cr then _type_ ;
 */
_extern  void p4_type_on_line (const p4_char_t *str, p4cell len) ; /*{*/

/** _emits_ ( n# ch -- )
 * type a string of chars by repeating a single character which
 * is usually a space, see => SPACES
 * (output adjusting the OUT variable, see => _type_ and => _outs_ )
 : _emits_ swap 0 do dup _putc_ loop drop _flush_ _?xy_ drop out ! ;
 */
_extern  void p4_emits (int n, const char c) ; /*{*/

/** _tab_ ( n# -- )
 * type a string of space up to the next tabulator column
 * (output adjusting the OUT variable, see => _emits and => _typeonline )
 : _tab_ dup out @ - swap mod bl _emits_ ;
 */
_extern  void p4_tab (int n) ; /*{*/

/** _.line_ ( file* block# line# -- )
 */
_extern  void p4_dot_line (p4_File *fid, p4cell n, p4cell l) ; /*{*/

/** _expect_ ( str* str# -- span# )
 * EXPECT counted string from terminal, with echo, so one can use
 * simple editing facility with backspace, but nothing more.
 * it's very traditional, you want to use a lined-like function instead! 
 */
_extern  int p4_expect (char *p, p4cell n) ; /*{*/

/** _accept_ ( str* str# -- span# )
 * better input facility using lined if possible, otherwise
 * call _expect_noecho when running in a pipe or just _expect_ if no
 * real terminal attached.
 */
_extern  int p4_accept (p4_char_t *tib, int n) ; /*{*/

/**
 * source input: read from text-file 
 */
_extern  p4_bool_t p4_next_line (void) ; /*{*/

/** _source_ ( str*& str#& -- )
 * see => SOURCE - dispatch input source 
 */
_extern  void p4_source (const p4_char_t **p, int *n) ; /*{*/

/** _size_saved_input_ ( -- iframe-size )
 */
_extern  p4ucell p4_size_saved_input (void) ; /*{*/

/** _link_saved_input_ ( iframe* -- )
 * see => SAVE-INPUT
 */
_extern  void p4_link_saved_input (void *p) ; /*{*/

/** _save_input_ ( iframe-stack* -- iframe-stack*' )
 * see => SAVE-INPUT
 */
_extern  void * p4_save_input (void *p) ; /*{*/

/** _unlink_saved_input_ ( iframe* -- )
 * see => RESTORE-INPUT
 */
_extern  void p4_unlink_saved_input (void *p) ; /*{*/

/** _restore_input_ ( iframe-stack* -- iframe-stack*' )
 * see => RESTORE-INPUT
 */
_extern  void * p4_restore_input (void *p) ; /*{*/

/** _refill_ ( -- flag )
 * see => REFILL
 */
_extern  p4_bool_t p4_refill (void) ; /*{*/

/** _skip_delimiter_ ( del -- )
 * => SKIP-DELIMITER
 */
_extern  void p4_skip_delimiter (char del) ; /*{*/

/** _word:parse_ ( delim -- <end?> )
 */
_extern  p4_cell_t p4_word_parse (char del) ; /*{*/

/** _parse_ ( delim -- ptr len )
 : _parse_ _word:parse_ _word*_ s! _word#_ s! ;
 */
_extern  p4_cell_t p4_parse (char del, const p4_char_t **p, p4ucell *l) ; /*{*/

/** _word>here_ ( -- here* )
 * complement => _word:parse_ to  arrive at the normal => WORD implementation
 * will also ensure the string is zero-terminated - this makes a lot of
 * operations easier since most forth function can receive a string-span
 * directly but some need a string-copy and that is usually because it has
 * to be passed down into a C-defined function with zerotermined string. Just
 * use p4_HERE+1 (which is also the returnvalue of this function!) to have 
 * the start of the zero-terminated string. Note that this function may throw
 * with P4_ON_PARSE_OVER if the string is too long (it has set *DP=0 to
 * ensure again that => THROW will report PFE.word. as the offending string)
 */
_extern  p4_char_t* p4_word_to_here (void) ; /*{*/

/** _word_ ( del -- here* )
 : _word_ dup _skip_delimiter_ _word:parse_ _word>here_ ;
 */
_extern  p4_char_t * p4_word (char del) ; /*{*/

_extern  p4_cell_t p4_word_parseword (char del) ; /*{*/

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
