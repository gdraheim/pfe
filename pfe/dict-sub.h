#ifndef _PFE_DICT_SUB_H
#define _PFE_DICT_SUB_H 985126518
/* generated 2001-0320-2315 ../../pfe/../mk/Make-H.pl ../../pfe/dict-sub.c */

#include <pfe/incl-sub.h>

/** 
 *  Implements dictionary and wordlists.
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 5.16 %
 *    (%date_modified: Mon Mar 12 10:32:05 2001 %)
 */

#ifdef __cplusplus
extern "C" {
#endif


# define TO_BODY(C)	((p4cell *)((p4xt)(C) + 1))
# define BODY_FROM(P)	((p4xt)((p4cell *)(P) - 1))
# ifdef PFE_WITH_FIG
#  define P4_TO_DOES_BODY(C)  ((p4cell *)((p4xt)(C) + 2))
#  define P4_TO_DOES_CODE(C)  ((p4xt **)((p4xt)(C) + 1))
# else  
#  define P4_TO_DOES_BODY(C)  TO_BODY(C)
#  define P4_TO_DOES_CODE(C)  ((p4xt **)((p4xt)(C) - 1))
# endif


/** ((FORGET)) 
 * remove words from dictionary, free dictionary space, this is the
 * runtime helper of => (FORGET)
 */
extern P4_CODE (p4_forget_dp);

extern P4_CODE (p4_forget_wordset_RT);

/** ((ONLY)) ( -- )
 * the only-vocabulary is special. Calling it will erase
 * the search => ORDER of vocabularies and only allows
 * to name some very basic vocabularies. Even => ALSO
 * is not available.
 example:
   ONLY FORTH ALSO EXTENSIONS ALSO DEFINITIONS
 */
extern P4_CODE (p4_only_RT);

/** FORTH ( -- )
 : FORTH FORTH-WORDLIST CONTEXT ! ;
 */
extern P4_CODE (p4_forth_RT);

_extern  int p4_wl_hash (const char *s, int l) /* s string, l length of string, returns hash-code for that name */ ; /*{*/

_extern  char ** p4_topmost (p4_Wordl *w) ; /*{*/

_extern  char * p4_latest (void) ; /*{*/

/** 
 * create a word list in the dictionary 
 */
_extern  p4_Wordl * p4_make_wordlist (void) ; /*{*/

_extern  p4_Wordl * p4_find_wordlist (const char* nm, int nmlen) ; /*{*/

/** (FORGET)
 * forget anything above address
 */
_extern  void p4_forget (char* above) ; /*{*/

/**
 * create a destroyer word. Upon =>'FORGET' the code will will be
 * run with the given argument. The structure is code/CFA and what/PFA.
 */
_extern  char* p4_forget_word (const char *name, p4cell id, p4code ccode, p4cell what) ; /*{*/

_extern  char * p4_search_wordlist (const char *nm, int l, const p4_Wordl *w) ; /*{*/

_extern  char * p4_find (const char *nm, int l) ; /*{*/

/**
 * tick next word,  and
 * return count byte pointer of name field (to detect immediacy)
 */
_extern  char * p4_tick_nfa (void) ; /*{*/

/**
 * tick next word,  and return xt
 */
_extern  p4xt p4_tick_cfa (void) ; /*{*/

/**
 * tick next word, store p4xt in xt, and
 * return count byte pointer of name field (to detect immediacy)
 */
_extern  char * p4_tick (p4xt *xt) ; /*{*/

_extern  char * p4_string_comma (const char *s, int len) ; /*{*/

_extern  char* p4_parse_comma(char del) ; /*{*/

_extern  char * p4_word_comma (char del) ; /*{*/

/**
 * make a new dictionary entry in the word list identified by wid 
 *                   (fixme: delete the externs in other code portions)
 */
_extern  p4_Head * p4_make_head (const char *name, int len, char **nfa, p4_Wordl *wid) ; /*{*/

_extern  char* p4_header_from (p4code cfa, char flags, const char* name, int count) ; /*{*/

_extern  char* p4_header (p4code cfa, char flags) ; /*{*/

_extern  char ** p4_name_to_link (const char *p) ; /*{*/

_extern  char * p4_link_to_name (char **l) ; /*{*/

_extern  p4_Semant * p4_to_semant (p4xt xt) ; /*{*/

_extern  p4xt p4_link_from (char **lnk) ; /*{*/

_extern  char ** p4_to_link (p4xt xt) ; /*{*/

_extern  p4xt p4_name_from (const char *p) ; /*{*/

_extern  char * p4_to_name (p4xt c) ; /*{*/

_extern  void p4_dot_name (const char *nfa) ; /*{*/

_extern  void p4_wild_words (const p4_Wordl *wl, const char *pattern, const char *categories) ; /*{*/

_extern  int p4_complete_dictionary (char *in, char *out, int display) ; /*{*/

_extern  void p4_load_words (const p4Words* ws, p4_Wordl* wid, int unused) ; /*{*/

/**
 * the load_llist should be considered obsolote - the 0.30.x model
 * is essentially a flat-loadmodel via load_words. However it does
 * still work for backward-compatibility with source for binary-modules 
 * of version 0.29.x
 */
_extern  void p4_load_llist (void** list, p4_Wordl* wid, int extending) ; /*{*/

_extern  void p4_preload_only (void) ; /*{*/

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
