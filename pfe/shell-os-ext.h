#ifndef _PFE_SHELL_OS_EXT_H
#define _PFE_SHELL_OS_EXT_H 1105051253
/* generated 2005-0106-2340 ../../../pfe/../mk/Make-H.pl ../../../pfe/shell-os-ext.c */

#include <pfe/pfe-ext.h>

/** 
 * -- os-like / shell-like commands for pfe
 * 
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE         @(#) %derived_by: guidod %
 *  @version %version: bln_mpt1!5.30 %
 *    (%date_modified: Mon Feb 03 11:57:09 2003 %)
 *
 *  @description
 *        These builtin words are modelled after common shell commands,
 *        so that the Portable Forth Environment can often 
 *        be put in the place of a normal OS shell.
 */

#ifdef __cplusplus
extern "C" {
#endif




/** $PID ( -- pid )
 * calls system's <c> getpid </c>
 */
extern P4_CODE (p4_getpid);

/** $UID ( -- val )
 * calls system's <c> getuid </c>
 */
extern P4_CODE (p4_getuid);

/** $EUID ( -- val )
 * calls system's <c> geteuid </c>
 */
extern P4_CODE (p4_geteuid);

/** $GID ( -- val )
 * calls system's <c> getgid </c>
 */
extern P4_CODE (p4_getgid);

/** UMASK ( val -- ret )
 * calls system's <c> umask </c>
 */
extern P4_CODE (p4_umask);

/** $HOME ( -- str-ptr str-len )
 * calls system's <c> getenv(HOME) </c>
 */
extern P4_CODE (p4_home);

/** $USER ( -- str-ptr str-len )
 * calls system's <c> getenv(USER) </c>
 */
extern P4_CODE (p4_user);

/** $CWD ( -- str-ptr str-len )
 * calls system's <c> getcwd </c>
 */
extern P4_CODE (p4_cwd);

/** PWD ( -- )
 * calls system's <c> getcwd </c> and prints it to the screen
 : PWD  $CWD TYPE ;
 */
extern P4_CODE (p4_pwd);

/** CHDIR ( bstring -- )
 * change the current directory. <br>
 * <small> (under VxWorks it is global! do not use in scripts!!) </small>
 */
extern P4_CODE (p4_chdir);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
