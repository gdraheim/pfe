#ifndef _PFE_CORE_SUB_H
#define _PFE_CORE_SUB_H 984413845
/* generated 2001-0312-1717 ../../pfe/../mk/Make-H.pl ../../pfe/core-sub.c */

#include <pfe/incl-sub.h>

/** 
 * --  Subroutines for the Core Forth-System
 * 
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE            @(#) %derived_by: guidod %
 *  @version %version: 5.19 %
 *    (%date_modified: Mon Mar 12 10:32:03 2001 %)
 */

#ifdef __cplusplus
extern "C" {
#endif


# ifdef _P4_SOURCE
#  if defined HAVE_STRNCASECMP 
#   define p4_strncmpi strncasecmp
#  elif defined HAVE_STRNICMP
#   define p4_strncmpi strnicmp
#  else
    extern int p4_strncmpi ( const char *s1, const char* s2, int n); 
#  endif
#  ifndef strncmpi       /*cygwin32 has it already def'd*/
#   define strncmpi p4_strncmpi
#  endif
# endif
# ifndef HASNT_SYSTEM
  _extern int p4_systemf (const char* s, ...);
# endif


/**
 * (DICTVAR) forth-thread variable runtime, => VARIABLE like
 */
extern P4_CODE (p4_dictvar_RT);

/**
 * (DICTCONST) forth-thread variable runtime, => CONSTANT like
 */
extern P4_CODE (p4_dictconst_RT);

/**
 * source input:  read from terminal 
 */
extern P4_CODE (p4_query);

/**
 * return cell-aligned address
 */
_extern  p4cell p4_aligned (p4cell n) ; /*{*/

/**
 * return double float-aligned address
 */
_extern  p4cell p4_dfaligned (p4cell n) ; /*{*/

/**
 * push a C-string onto the SP runtime-stack, as if => S" string" was used
 */
_extern  void p4_strpush (const char *s) ; /*{*/

/**
 * return the next pocket for interactive string input.
 */
_extern  char * p4_pocket (void) ; /*{*/

/**
 * chop off trailing spaces for the stringbuffer. returns the new length,
 * so for an internal counted string, use
   *s = p4_dash_trailing (s+1, *s);
 */
_extern  int p4_dash_trailing (char *s, int n) ; /*{*/

/**
 * tolower() applied to a stringbuffer
 */
_extern  void p4_lower (char *p, int n) ; /*{*/

/**
 * toupper() applied to a stringbuffer
 */
_extern  void p4_upper (char *p, int n) ; /*{*/

/**
 * copy stringbuffer into a field as a zero-terminated string.
 */
_extern  char * p4_store_c_string (const char *src, int n, char *dst, int max) ; /*{*/

/**
 * copy stringbuffer into a field as a zero-terminated filename-string,
 * a shell-homdir like "~username" will be expanded, and the
 * platform-specific dir-delimiter is copied in on the fly ('/' vs. '\\')
 */
_extern  char* p4_store_filename (const char *src, int n, char* dst, int max) ; /*{*/

/**
 * a new pocket with the given filename as asciiz
 */
_extern  char* p4_pocket_filename (const char* src, int n) ; /*{*/

_extern  char * p4_pocket_expanded_filename (const char *nm, int ln, const char *paths, const char *exts) ; /*{*/

/**
 * search for substring p2/u2 in string p1/u1 
 */
_extern  char * p4_search (const char *p1, int u1, const char *p2, int u2) ; /*{*/

/**
 * Match string against pattern.
 * Pattern knows wildcards `*' and `?' and `\' to escape a wildcard.
 */
_extern  int p4_match (const char *pattern, const char *string, int ic) ; /*{*/

/**
 * unsigned divide procedure, single prec 
 */
_extern  udiv_t p4_udiv (p4ucell num, p4ucell denom) ; /*{*/

/**
 * floored divide procedure, single prec 
 */
_extern  fdiv_t p4_fdiv (p4cell num, p4cell denom) ; /*{*/

/**
 * Divides *ud by denom, leaves result in *ud, returns remainder.
 * For number output conversion: dividing by BASE.
 */
_extern  p4ucell p4_u_d_div (p4udcell *ud, p4ucell denom) ; /*{*/

/**
 * Computes *ud * w + c, where w is actually only half of a cell in size.
 * Leaves result in *ud.
 * For number input conversion: multiply by BASE and add digit.
 */
_extern  void p4_u_d_mul (p4udcell *ud, p4ucell w, p4ucell c) ; /*{*/

/**
 * Get value of digit c into *n, return flag: valid digit.
 */
_extern  int p4_dig2num (p4char c, p4ucell *n, p4ucell base) ; /*{*/

/**
 * make digit 
 */
_extern  char p4_num2dig (p4ucell n) ; /*{*/

/**
 * insert into pictured numeric output string
 */
_extern  void p4_hold (char c) ; /*{*/

/**
 * try to convert into numer, see => >NUMBER
 */
_extern  const char * p4_to_number (const char *p, p4ucell *n, p4udcell *d, p4ucell base) ; /*{*/

/**
 * try to convert into number, see => ?NUMBER
 */
_extern  int p4_number_question (const char *p, p4ucell n, p4dcell *d) ; /*{*/

/**
 * This is for internal use only (SEE and debugger),
 * The real `UD.R' etc. words use HOLD and the memory area below PAD
 */
_extern  char * p4_str_ud_dot_r (p4udcell ud, char *p, int w, int base) ; /*{*/

/**
 * This is for internal use only (SEE and debugger),
 * The real `UD.R' etc. words use HOLD and the memory area below PAD
 */
_extern  char * p4_str_d_dot_r (p4dcell d, char *p, int w, int base) ; /*{*/

/**
 * This is for internal use only (SEE and debugger),
 * The real `UD.R' etc. words use HOLD and the memory area below PAD
 */
_extern  char * p4_str_dot (p4cell n, char *p, int base) ; /*{*/

/**
 * emit single character
                            (output adjusting the OUT variable)
 */
_extern  void p4_outc (char c) ; /*{*/

/**
 * type a string
                            (output adjusting the OUT variable)
 */
_extern  void p4_outs (const char *s) /* type a string */ ; /*{*/

/**
 * type a string with formatting
                            (output adjusting the OUT variable)
 */
_extern  int p4_outf (const char *s,...) ; /*{*/

/**
 * type counted string to terminal
                            (output adjusting the OUT variable)
 */
_extern  void p4_type (const char *s, p4cell n) ; /*{*/

/**
 * type counted string to terminal, if it does not fit in full on
 * the current line, emit a => CR before
                            (output adjusting the OUT variable)
 */
_extern  void p4_type_on_line (const char *s, p4cell n) ; /*{*/

/**
 * type a string of chars, usually a string of spaces,
 * see => SPACES
                            (output adjusting the OUT variable)
 */
_extern  void p4_emits (int n, const char c) ; /*{*/

/**
 * type a string of space up to the next tabulator column
                            (output adjusting the OUT variable)
 */
_extern  void p4_tab (int n) ; /*{*/

/**
 */
_extern  void p4_dot_line (p4_File *fid, p4cell n, p4cell l) ; /*{*/

/** EXPECT counted string from terminal
 * simple editing facility with backspace,
 * very traditional, use lined-like function instead! 
 */
_extern  int p4_expect (char *p, p4cell n) ; /*{*/

/** 
 * better input facility using lined 
 */
_extern  int p4_accept (char *p, int n) ; /*{*/

/** 
 * check for 'q' pressed 
 */
_extern  int p4_Q_stop (void) ; /*{*/

/**
 * Like CR but stop after one screenful and return flag if 'q' pressed.
 * Improved by aph@oclc.org (Andrew Houghton)
 */
_extern  int p4_Q_cr (void) ; /*{*/

/**
 * source input: read from text-file 
 */
_extern  int p4_next_line (void) ; /*{*/

/** 
 * SOURCE - dispatch input source 
 */
_extern  void p4_source (char **p, int *n) ; /*{*/

/**
 * SAVE-INPUT
 */
_extern  void * p4_save_input (void *p) ; /*{*/

/**
 * RESTORE-INPUT
 */
_extern  void * p4_restore_input (void *p) ; /*{*/

/**
 * REFILL
 */
_extern  int p4_refill (void) ; /*{*/

/**
 * SKIP-DELIMITER
 */
_extern  void p4_skip_delimiter (char del) ; /*{*/

/**
 * PARSE
 */
_extern  int p4_parse (char del, char **p, p4ucell *l) ; /*{*/

/**
 * WORD
 */
_extern  char * p4_word (char del) ; /*{*/

_extern  int p4_parseword (char del, char** p, p4ucell* len) ; /*{*/

/**
 * complement p4_parseword to  arrive at the normal => WORD implementation
 */
_extern  char* p4_hereword (char* p, p4ucell len) ; /*{*/

/**
 * ABORT" string" impl.
 */
_extern  void p4_abortq (const char *fmt,...) ; /*{*/

/**
 * ?PAIRS
 */
_extern  void p4_Q_pairs (p4cell n) ; /*{*/

/**
 * ?OPEN
 */
_extern  void p4_Q_file_open (p4_File *fid) ; /*{*/

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
