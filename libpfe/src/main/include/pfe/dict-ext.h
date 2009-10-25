#ifndef PFE_DICT_EXT_H
#define PFE_DICT_EXT_H 20091025
/* generated by make-header.py from ../../c/dict-ext.c */

#include <pfe/pfe-ext.h>

/**
 * -- DICT words - allow for multiple dictionaries
 *
 *  Copyright (C) 2009 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 0 $
 *     (modified $Date: 2009-10-24 15:33:19 +0200 (Sa, 24 Okt 2009) $)
 *
 *  @description
 *       Allows to create new memory banks and activates them as
 *       the standard dictionory for compilation. It is the basis
 *       of the auto-extending dictionary memory feature which
 *       will simply allocate/activate a new bank when required.
  */

#ifdef __cplusplus
extern "C" {
#endif




/** COMPILE-TARGET ( dict-header-addr -- )
 * activates a new dictionary as the compilation target
 */
extern void FXCode(p4_compile_target);

/** OPTION-DICTIONARY ( -- dict-header-addr )
 * this dictionary is used during option parsing.
 */
extern void FXCode(p4_option_dictionary);

/** SYSTEM-DICTIONARY ( -- dict-header-addr )
 * this dictionary is initialized during boot phase
 */
extern void FXCode (p4_system_dictionary);

/** CREATE-DICTIONARY ( size "name" -- )
 * create a new word with a dictionary header and
 * add an initial memory block to the new dictionary.
 * The dictionary is not enabled immediately, you need
 * to do that with the help of => COMPILE-TARGET
 */
extern void FXCode (p4_create_diciontary);

/** EXTEND-DICTIONARY ( size -- )
 * add a new memory block to the current dictionary => COMPILE-TARGET
 *
 * extending a dictionary is most usually done while
 * a dictionary is active as the => COMPILE-TARGET and
 * its headroom has run out. The implementation is like
 * making a new non-named Dictionary header and to move
 * the pointers of the current memory block to the item
 * after which the original header is updated with a new
 * and longer memory block. That allows to have the first
 * header block in the dictionary link and show its name
 * by calling BODY> >NAME on its base pointer.
 */
extern void FXCode (p4_extend_dictionary);

/** _compile_target_ ( dict-header-addr -- )
 * activates a new dictionary as the compilation target
 */
extern void p4_compile_target(p4_Dictionary* dict); /*{*/

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
