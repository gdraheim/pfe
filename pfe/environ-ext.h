#ifndef _PFE_ENVIRON_EXT_H
#define _PFE_ENVIRON_EXT_H 984413842
/* generated 2001-0312-1717 ../../pfe/../mk/Make-H.pl ../../pfe/environ-ext.c */

#include <pfe/incl-ext.h>

/** 
 * -- Extended Environment related definitions
 * 
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 1.6 %
 *    (%date_modified: Mon Mar 12 10:32:14 2001 %)
 *
 *  @description
 *       gforth and win32for are also using an extra => ENVIRONMENT
 *       wordlist to register hints on the system environment. It
 *       is just a normal => VOCABULARY that can be searched&executed
 *       with => ENVIRONMENT? . In pfe, the environment wordlist does
 *       also register the extension status of the system including
 *       allocated slots, loaded binaries and => REQUIRED source files.
 */

#ifdef __cplusplus
extern "C" {
#endif




/** ENVIRONMENT ( -- )
 * execute the => VOCABULARY runtime for the => ENVIRONMENT-WORDLIST
 : ENVIRONMENT  ENVIRONMENT-WORDLIST CONTEXT ! ;
 * see newstyle =>"ENVIRONMENT?"
 */
extern P4_CODE (p4_environment);

/** ENVIRONMENT-WORDLIST ( -- wid )
 * return the => WORDLIST id of the => ENVIRONMENT so
 * it could be passed to => CURRENT (via =>"SET-CURRENT")
 : ENVIRONMENT-WORDLIST  ['] ENVIRONMENT >WORDLIST ;
 */
extern P4_CODE (p4_environment_wordlist);

/** REQUIRED ( ... str-ptr str-len -- ??? )
 * the filename argument is loaded via => INCLUDED as
 * an extension package to the current system. The filename
 * is registered in the current => ENVIRONMENT so that it is
 * only => INCLUDED once (!!) if called multiple times via
 * => REQUIRED or => REQUIRES
 */
extern P4_CODE (p4_required);

/** REQUIRES ( ... "name" -- ??? )
 * parses the next => WORD and passes it to => REQUIRED
 * this is the self-parsing version of => REQUIRED and
 * it does parrallel => INCLUDE w.r.t. => INCLUDED
 */
extern P4_CODE (p4_requires);

_extern  void* p4_required (const char* name, const p4cell length) ; /*{*/

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
