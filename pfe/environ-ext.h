#ifndef _PFE_ENVIRON_EXT_H
#define _PFE_ENVIRON_EXT_H 1158897468
/* generated 2006-0922-0557 ../../pfe/../mk/Make-H.pl ../../pfe/environ-ext.c */

#include <pfe/pfe-ext.h>

/** 
 * -- Extended Environment related definitions
 * 
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.4 $
 *     (modified $Date: 2008-04-20 04:46:31 $)
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

/** ENVIRONMENT ( -- )
 * execute the => VOCABULARY runtime for the => ENVIRONMENT-WORDLIST
 : ENVIRONMENT  ENVIRONMENT-WORDLIST CONTEXT ! ;
 ' ENVIRONMENT  ALIAS [ENV] IMMEDIATE
 * see newstyle =>"ENVIRONMENT?" 
 */
extern P4_CODE (p4_environment);

extern P4_CODE (p4_environment_Q);

/** NEEDS ( name -- )
 *
 * A self-parsing variant of an environment-query check. It is similar
 * to a simulation like

 : NEEDS PARSE-WORD 2DUP ENVIRONMENT? 
   IF DROP ( extra value ) 2DROP ( success - be silent )
   ELSE TYPE ." not available " CR THEN ;

 * however that would only match those worset-envqueries which return a
 * single extra item under the uppermost TRUE flag in the success case.
 * Instead it works more like

 : NEEDS PARSE-WORD 2DUP ENVIRONMENT-WORDLIST SEARCH-WORDLIST
   IF 2DROP ( success - be silent and just drop the parsed word )
   ELSE TYPE ." not available " CR THEN ;

 * however we add the same extension as in => ENVIRONMENT? as that
 * it can automatically load a wordset module to fullfil a query
 * that looks like "[wordsetname]-ext". Therefore, the following
 * two lines are pretty much identical:

 LOADM floating
 NEEDS floating-ext

 * the difference between the two: if somebody did provide a forth
 * level implementation of floating-ext then that implementation might
 * have registered a hint "floating-ext" in the environment-wordlist.
 * This extra-hint will inhibit loading of the binary module even if
 * it exists and not been loaded so far. The => LOADM however will
 * not check the => ENVIRONMENT-WORDLIST and only check its loadlist
 * of binary wordset modules in the system.
 *
 * It is therefore recommended to use => NEEDS instead of => LOADM
 * unless you know you want the binary module, quickly and uncondtionally.
 */
extern P4_CODE (p4_needs_environment);

_extern  void* p4_required (const p4_char_t* name, const p4cell length) ; /*{*/

/** ENVIRONMENT? ( a1 n1 -- false | ?? true )
 * check the environment for a property, usually
 * a condition like questioning the existance of 
 * specified wordset, but it can also return some
 * implementation properties like "WORDLISTS"
 * (the length of the search-order) or "#LOCALS"
 * (the maximum number of locals) 

 * Here it implements the environment queries as a => SEARCH-WORDLIST 
 * in a user-visible vocabulary called => ENVIRONMENT
 : ENVIRONMENT?
   ['] ENVIRONMENT >WORDLIST SEARCH-WORDLIST
   IF  EXECUTE TRUE ELSE  FALSE THEN ;

 * special extension: a search for CORE will also find a definition
 * of CORE-EXT or CORE-EXT-EXT or CORE-EXT-EXT-EXT - it just has to
 * be below the ansi-standard maximum length of 31 chars.
 
 * if a name like "dstrings-ext" is given, and no such entry
 * can be found, then envQ will try to trigger a => (LOADM) of
 * that module, in the hope that this implicit-load does in fact
 * define the answer. Use with care, it's a very new feature.
 */
_extern  p4_char_t* p4_environment_Q(const p4_char_t* str, p4cell l) ; /*{*/

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
