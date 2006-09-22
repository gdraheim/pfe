#ifndef _PFE_OPTION_EXT_H
#define _PFE_OPTION_EXT_H 1158897468
/* generated 2006-0922-0557 ../../pfe/../mk/Make-H.pl ../../pfe/option-ext.c */

#include <pfe/pfe-ext.h>

/** 
 * -- Almost-Non-Volatile Environment Options
 * 
 *  Copyright (C) Tektronix, Inc. 2001 - 2001.
 *  Copyright (C) 2005 - 2006 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2006-09-22 04:43:03 $)
 *
 *  @description
 *       the openfirmware standard specifies some means to add
 *       options to a non-volatile ram-area (nvram) that are used
 *       in the bootup sequence of the forth-based bootsystem.
 *       Here we add an internal API for looking for bootup options,
 *       an internal API to add bootup options through commandline
 *       processing, and an external API to change the bootup options
 *       for a => COLD reboot or an => APPLICATION specific sequence.
 *       The external API will try to follow openfirmware as closely
 *       as possible without having a real non-volatile ram-area.
 *       Instead there is a session-struct that can be changed and
 *       from which multiple forth-threads can be instantiated
 *       later using those options. The forth-instantion processing
 *       is not supposed to scan for commandline options, which is
 *       a mere necessity in embedded enviroments where option
 *       transferal is done in a completly different way and where
 *       the forth thread is never killed but just stopped or
 *       restarted for various reasons. Even that there is no real
 *       nvram we add a cold-options reboot-area in this wordset.
 *       The option-ram is organized just along a normal dictionary
 *       just need to add a wordlist-handlestruct to find the
 *       definitions in this dictlike ram-portion.
 */

#ifdef __cplusplus
extern "C" {
#endif


typedef p4_Session p4_Options;
#define p4_get_option_value  p4_search_option_value
#define p4_get_option_string p4_search_option_string


extern P4_CODE (p4_string_RT);

/** NVRAM,WORDS ( -- )
 *
 * Print a list of => WORDS in the NVRAM buffer. Try to show also the
 * current value, atleast for NVRAM numbers and strings. Words can be
 * added or changed with the help of => NVRAM,SET or => NVRAM,USE
 *
 * Values in the NVRAM buffer will survive a => COLD reboot, in many
 * hosted environments however the NVRAM will be lost on program exit.
 */
extern P4_CODE (p4_nvram_words);

/** NVRAM,AS ( str-ptr str-len "varname" -- )
 * set the NVRAM variable to the specified string.
 *
 * Some NVRAM strings do not take effect until next => COLD reboot.
 */
extern P4_CODE (p4_nvram_as);

/** NVRAM,TO ( number "varname" -- )
 * set the NVRAM variable to the specified number.
 *
 * Most NVRAM numbers do not take effect until next => COLD reboot.
 */
extern P4_CODE (p4_nvram_to);

/** NVRAM,Z@ ( "varname" -- z-str )
 *
 * Return the string pointer of the NVRAM string item, or null if no
 * such item exists.
 */
extern P4_CODE (p4_nvram_z_fetch);

/** NVRAM,S@ ( "varname" -- str-ptr str-len )
 *
 * Return the string span of the NVRAM string item, or double null if no
 * such item exists.
 */
extern P4_CODE (p4_nvram_s_fetch);

/** NVRAM,?@ ( number "varname" -- number' )
 *
 * Return the value of the NVRAM value item, or leave the original
 * number untouched (i.e. the default value for your option).
 */
extern P4_CODE (p4_nvram_Q_fetch);

_extern  p4xt p4_search_option (const p4char* nm, int l, p4_Options* opt) ; /*{*/

_extern  void p4_invalidate_string_options (p4_Options* opt) ; /*{*/

_extern  p4xt p4_create_option (const p4char* name, int len, int size, p4_Options* opt) ; /*{*/

_extern  p4celll p4_search_option_value (const p4char* nm, int l, p4celll defval, p4_Options* opt) ; /*{*/

_extern  p4cell* p4_create_option_value (const p4char* nm, int l, p4celll defval, p4_Options* opt) ; /*{*/

_extern  p4cell* p4_change_option_value (const p4char* nm, int l, p4celll defval, p4_Options* opt) ; /*{*/

_extern  const p4_char_t* p4_search_option_string (const p4char* nm, int l, const char* defval, p4_Options* opt) ; /*{*/

_extern  const char** p4_lookup_option_string (const p4char* nm, int l, const char** defval, p4_Options* opt) ; /*{*/

_extern  const char** p4_create_option_string (const p4char* nm, int l, const char* defval, p4_Options* opt) ; /*{*/

_extern  const char** p4_change_option_string (const p4char* nm, int l, const char* defval, p4_Options* opt) ; /*{*/

_extern  const char** p4_append_option_string (const p4char* nm, int l, char delimiter, const char* defval, p4_Options* opt) ; /*{*/

_extern  p4ucell p4_convsize (const char* s, p4ucell elemsize) ; /*{*/

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
