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
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) =
"@(#) $Id: dict-ext.c 0 guidod $";
#endif

#define _P4_SOURCE 1
#include <pfe/pfe-base.h>
#include <pfe/logging.h>

/** _compile_target_ ( dict-header-addr -- )
 * activates a new dictionary as the compilation target
 */
void p4_compile_target(p4_Dictionary* dict)
{
	PFE.dict_current->here  = PFE.dict.here;
	PFE.dict_current->last  = PFE.dict.last;
	PFE.dict.here  = dict->here;
	PFE.dict.last  = dict->last;
	PFE.dict.base  = dict->base;
	PFE.dict.limit = dict->limit;
	PFE.dict.link  = dict->link;
	PFE.dict.next  = dict->next;
	PFE.dict_current = dict;
}

/** COMPILE-TARGET ( dict-header-addr -- )
 * activates a new dictionary as the compilation target
 */
void FXCode(p4_compile_target)
{
	p4_Dictionary* dict = (p4_Dictionary*) FX_POP;
	p4_compile_target(dict);
}

/** OPTION-DICTIONARY ( -- dict-header-addr )
 * this dictionary is used during option parsing.
 */
void FXCode(p4_option_dictionary)
{
	FX_PUSH (& PFE_set.opt);
}

/** SYSTEM-DICTIONARY ( -- dict-header-addr )
 * this dictionary is initialized during boot phase
 */
void FXCode (p4_system_dictionary)
{
	FX_PUSH(& PFE.system_dict);
}

void FXCode_RT(p4_dictionary_RT)
{
	FX_USE_BODY_ADDR;
	FX_PUSH(FX_POP_BODY_ADDR);
}

/** CREATE-DICTIONARY ( size "name" -- )
 * create a new word with a dictionary header and
 * add an initial memory block to the new dictionary.
 * The dictionary is not enabled immediately, you need
 * to do that with the help of => COMPILE-TARGET
 */
void FXCode (p4_create_dictionary)
{
	p4ucell size = FX_POP;
	FX_HEADER;
	FX_RUNTIME1(p4_create_dictionary);
	p4_Dictionary* dict = (p4_Dictionary*) HERE;
	HERE += sizeof(p4_Dictionary);
	dict->link = PFE.dict_link; PFE.dict_link = dict;
	dict->last = NULL;
	dict->base = p4_xalloc (size);
	dict->limit = dict->base + size;
	dict->here = dict->base;
}
P4RUNTIME1(p4_create_dictionary, p4_dictionary_RT);

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
void FXCode (p4_extend_dictionary)
{
	p4ucell size = FX_POP;
	FX_NONAME;
	FX_RUNTIME1(p4_create_dictionary);
	p4_Dictionary* dict = (p4_Dictionary*) HERE;
	HERE += sizeof(p4_Dictionary);
	dict->base = PFE.dict.base;
	dict->limit = PFE.dict.limit;
	dict->last = PFE.dict.last;
	dict->here = PFE.dict.here;
	PFE.dict.link = NULL;
	PFE.dict.base = p4_xalloc (size);
	PFE.dict.limit = PFE.dict.base + size;
	PFE.dict.here = PFE.dict.base;
	PFE.dict.next = dict;
}

P4_LISTWORDSET (dict) [] =
{
    P4_INTO ("EXTENSIONS", 0),
    P4_FXco ("COMPILE-TARGET",          p4_compile_target),
    P4_FXco ("OPTION-DICTIONARY",       p4_option_dictionary),
    P4_FXco ("SYSTEM-DICTIONARY",       p4_system_dictionary),
    P4_RTco ("CREATE-DICTIONARY",       p4_create_dictionary),
    P4_FXco ("EXTEND-DICTIONARY",       p4_extend_dictionary),
    P4_DVaL ("DICTIONARY-LINK",         dict_link),
    P4_DVaL ("DICTIONARY-TARGET",       dict_current),
};
P4_COUNTWORDSET (dict, "DICT-EXT - multiple dictionary space support");

/*@}*/
