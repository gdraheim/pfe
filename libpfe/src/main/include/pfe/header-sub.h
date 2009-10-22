#ifndef PFE_HEADER_SUB_H
#define PFE_HEADER_SUB_H 1256212373
/* generated 2009-1022-1352 make-header.py ../../c/header-sub.c */

#include <pfe/pfe-sub.h>

/**
 *  Implements header creation and navigation.
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.10 $
 *     (modified $Date: 2008-05-10 16:34:51 $)
 */

#ifdef __cplusplus
extern "C" {
#endif


/* you must differentiate between VAR-style body and DOES-style body */
# define P4_TO_LINK(C)     ((p4char**)(C) -1 )
# define P4_LINK_FROM(C)   ((p4xt)(C) + 1 )
# define P4_TO_BODY(C)     ((p4cell *)((p4xt)(C) + 1))
# define P4_BODY_FROM(P)   ((p4xt)((p4cell *)(P) - 1))
# define P4_TO_DOES_BODY(C)  ((p4cell *)((p4xt)(C) + 2))
# define P4_TO_DOES_CODE(C)  ((p4xcode **)((p4xt)(C) + 1))
# ifdef PFE_CALL_THREADING
extern const p4xcode* p4_to_code (p4xt xt);
# define P4_TO_CODE(C)     (p4_to_code((p4xt)C))
# else
# define P4_TO_CODE(C)     (C)
# endif


/**
 * (DICTVAR) forth-thread variable runtime, => VARIABLE like
 */
extern void FXCode_RT (p4_dictvar_RT);

/**
 * (DICTGET) forth-thread constget runtime, => VALUE like
 */
extern void FXCode_RT (p4_dictget_RT);

/**
 * make a new dictionary entry in the word list identified by wid
 *                   ( TODO: delete the externs in other code portions)
 * This function is really ifdef'd a lot because every implementation
 * needs to be (a) fast because it is used heavily when loading a forth
 * script and (b) robust to bad names like non-ascii characters and (c)
 * each variant has restrictions on header field alignments.
 *
 */
_extern  p4_namebuf_t* p4_header_comma (const p4_namechar_t *name, int len, p4_Wordl *wid) ; /*{*/

_extern  p4_namebuf_t* p4_make_header (p4code cfa, char flags, const p4_namechar_t* name, int count, p4_Wordl* wid) ; /*{*/

_extern  p4_namebuf_t* p4_header (p4code cfa, char flags) ; /*{*/

_extern  p4_namebuf_t* p4_header_in (p4_Wordl* wid) ; /*{*/

_extern  p4cell * p4_to_body (p4xt xt) ; /*{*/

_extern  p4xt p4_body_from (p4cell* body) ; /*{*/

_extern  p4_namebuf_t** p4_name_to_link (const p4_namebuf_t* p) ; /*{*/

_extern  p4_namebuf_t * p4_link_to_name (p4_namebuf_t **l) ; /*{*/

_extern  p4_Semant * p4_to_semant (p4xt xt) ; /*{*/

_extern  p4_namebuf_t ** p4_to_link (p4xt xt) ; /*{*/

_extern  p4xt p4_name_from (const p4_namebuf_t *p) ; /*{*/

_extern  void p4_check_deprecated (p4_namebuf_t* nfa) ; /*{*/

_extern  p4_namebuf_t * p4_to_name (p4xt c) ; /*{*/

_extern  void p4_dot_name (const p4_namebuf_t *nfa) ; /*{*/

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
