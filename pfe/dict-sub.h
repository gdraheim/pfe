#ifndef _PFE_DICT_SUB_H
#define _PFE_DICT_SUB_H 1158897468
/* generated 2006-0922-0557 ../../pfe/../mk/Make-H.pl ../../pfe/dict-sub.c */

#include <pfe/pfe-sub.h>

/** 
 *  Implements dictionary and wordlists.
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.4 $
 *     (modified $Date: 2008-04-20 04:46:30 $)
 */

#ifdef __cplusplus
extern "C" {
#endif




/** ((FORGET)) 
 * remove words from dictionary, free dictionary space, this is the
 * runtime helper of => (FORGET)
 */
extern P4_CODE (p4_forget_dp);

extern P4_CODE (p4_destroyer_RT);

/** ONLY ( -- )
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

_extern  int p4_wl_hash (const p4_char_t *s, int l) /* s string, l length of string, returns hash-code for that name */ ; /*{*/

_extern  p4char ** p4_topmost (p4_Wordl *w) ; /*{*/

_extern  p4char * p4_latest (void) ; /*{*/

/** 
 * create a word list in the dictionary 
 */
_extern  p4_Wordl * p4_make_wordlist (p4char* nfa) ; /*{*/

_extern  p4_Wordl * p4_find_wordlist_str (const char* nm) ; /*{*/

_extern  p4_Wordl * p4_find_wordlist (const p4_char_t* nm, int nmlen) ; /*{*/

/** (FORGET)
 * forget anything above address
 */
_extern  void p4_forget (p4_byte_t* above) ; /*{*/

/**
 * create a destroyer word. Upon =>'FORGET' the code will will be
 * run with the given argument. The structure is code/CFA and what/PFA.
 */
_extern  p4_namebuf_t* p4_forget_word (const char *name, p4cell id, p4code ccode, p4cell what) ; /*{*/

_extern  p4_namebuf_t* p4_search_wordlist (const p4_char_t *nm, int l, const p4_Wordl *w) ; /*{*/

_extern  p4_namebuf_t* p4_next_search_wordlist (p4_namebuf_t* last, const p4_char_t* nm, int l, const p4_Wordl* w) ; /*{*/

_extern  p4char * p4_find (const p4_char_t *nm, int l) ; /*{*/

/**
 * tick next word,  and
 * return count byte pointer of name field (to detect immediacy)
 */
_extern  p4char * p4_tick_nfa (void) ; /*{*/

/**
 * tick next word,  and return xt
 */
_extern  p4xt p4_tick_cfa (void) ; /*{*/

_extern  char * p4_tick (p4xt *xt) ; /*{*/

_extern  p4_charbuf_t* p4_string_comma (const p4_char_t* s, int len) ; /*{*/

_extern  void p4_wild_words (const p4_Wordl *wl, const char *pattern, const char *categories) ; /*{*/

_extern  int p4_complete_dictionary (char *in, char *out, int display) ; /*{*/

_extern  void p4_preload_only (void) ; /*{*/

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
