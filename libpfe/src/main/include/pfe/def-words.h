#ifndef __PFE_DEF_WORDS_H
#define __PFE_DEF_WORDS_H
/**
 * -- types, codes and macros for symbol table load
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2003.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.5 $
 *     (modified $Date: 2008-09-11 01:27:20 $)
 *
 *  @description
 *         here are the macros that are used to build the various
 *         symbols tables, including wordsets and loadlists.
 */
/*@{*/

#include <pfe/pfe-sub.h>
#include <pfe/def-macro.h>
#include <pfe/def-const.h>

#define P4WORDS(SET)  P4SUFFIX (SET,_LTX_p4_WORDS)
#define P4WLIST(SET)  P4SUFFIX (SET,_LTX_p4_WLIST)
#define P4LLIST(SET)  P4SUFFIX (SET,_LTX_p4_LLIST)

#define extern_P4WORDS(SET) extern const p4Words P4WORDS(SET)

typedef struct                  /* describe a word for registration */
{                               /* in the dictionary */
    const char *name;           /* name of word */
    p4code ptr;                 /* code it executes or something */
}                               /* according to spc@pineal.math.fau.edu: */
p4Word;                         /* 'void *' doesn't work here on SGI */

typedef struct                  /* describes the set of words */
{                               /* a module loads into the dictionary */
    int n;                      /* how many words? */
    const p4Word *w;            /* pointer to vector of words */
    const char *name;           /* name of word set, or null */
}
p4Words;

#ifdef PFE_CALL_THREADING
typedef union p4_ExecToken
{
    struct { p4char* def; } *type;
    p4Word* word;
    p4_Runtime2* call;
} p4_ExecToken;
#endif

#define P4_LISTWORDSET( SET )  \
    static const p4Word P4WLIST(SET)
#define P4_LISTWORDS( SET )  \
    static const p4Word P4WLIST(SET)[]

# define P4_COUNTWORDSET(SET,NAME) P4_COUNTWORDS(SET,NAME)
# define P4_COUNTWORDS(SET,NAME) \
    const p4Words P4WORDS(SET) = \
    {                           \
        DIM (P4WLIST (SET)),    \
        P4WLIST (SET),          \
        NAME                    \
    }

#ifdef _P4_SOURCE

#define LISTWORDS( SET )  \
    static const p4Word P4WLIST(SET)[]

# define COUNTWORDS(SET,NAME) \
    const p4Words P4WORDS(SET) = \
    {                           \
        DIM (P4WLIST (SET)),    \
        P4WLIST (SET),          \
        NAME                    \
    }

#endif

/* Encoding the kind of definition i.e. which runtime to fill into the cfa.
 *
 * the original pfe had 3 (or 4?) levels of load-tables.
 * the definitions above define a two level approach.
 * the following definitions define a one level approach,
 * and up to now, no heritage-defines are given.
 *
 * each uppercase-name is an immediate word, all others are not.
 */

#define p4_FXCO 'p'     /* CO */ /* ordinary primitive (code) */
#define p4_IXCO 'P'     /* CI */
#define p4_SXCO 'X'     /* CS */ /* smart-word (semant) */
#define p4_XXCO 'x'     /* CX */ /* auto-init */
#define p4_RTCO 'r'     /* RT */ /* creates a word with special runtime */

#define p4_OVAR 'v'     /* OV */ /* ordinary variable */
#define p4_IVAR 'V'     /* IV */
#define p4_OCON 'c'     /* OC */ /* ordinary constant */
#define p4_ICON 'C'     /* IC */
#define p4_OVAL 'l'     /* OL */ /* ordinary value */
#define p4_IVAL 'L'     /* IL */

/* -def p4_DVAH 'h'     // DW */ /* dict chainvar (threaded) [REMOVED] */
#define p4_DVAR 'm'     /* DV */ /* dict variable (threaded) */
#define p4_DCON 'n'     /* DC */ /* dict constget (threaded) */
/* -def p4_DSET 't'     // DS */ /* dict valueset (threaded) [REMOVED] */
#define p4_OFFS 'o'     /* offset word */

#define p4_SNYM 'Y'     /* immediate synonym */
#define p4_FNYM 'y'     /*  ordinary synonym */
#define p4_iOLD 'Z'     /* immediate synonym of obsolete word */
#define p4_xOLD 'z'     /*  ordinary synonym of obsolete word */

#define p4_SLOT 's'     /* open slot */
#define p4_SSIZ 'S'     /* init slot */

#define p4_OVOC 'w'     /* explicitly create a vocabulary (normal) */
#define p4_IVOC 'W'     /* explicitly create a vocabulary (immediate) */

#define p4_NEED 'q'     /* issue a NEEDS check - which may load a module */
#define p4_INTO 'i'     /* make sure the vocabulary exists, and put all */
                        /* of the following words into it, and with nonzero */
                        /* arg, make sure the voc is in the default search */
                        /* order (so they are visible after load) */
#define p4_LOAD 'I'     /* load the referenced wordset, now, recursively */
#define p4_EXPT 'e'     /* set an exception-id and descriptive string */

#define p4_SHOW 'd'     /* show one-time message notes to the user */
#define p4_DEPR 'D'     /* show one-time deprecation notes to the user */
#define p4_STKi 'M'     /* stackhelp info for static tracing/checking */
#define p4_STKx 'N'     /* stackhelp info for static tracing/checking */

#define p4_ITEM '@'     /* Runtime item (e.g. compiled by RTCO word) */
#define p4_DTOR '~'     /* Destroyer item (e.g. compiled by forget_word) */
#define p4_NEST '_'     /* NestedCall item (e.g. compiled by : ... ; ) */

/* macros to build entries in the wordlists:
 * until all sematic-words have a proper name along, we need to help
 * the decompiler here to print the name. Since PFE uses %.*s to print,
 * it is okay to the upper bound of the length as a dummy count byte,
 * which is 0xFF (0377) for immediate words and 0x9F (0237) for the others.
 */

#define P4_FXCO( NM, PCODE)     { "p\237"NM, &P4CODE (PCODE) }
#define P4_IXCO( NM, PCODE)     { "P\377"NM, &P4CODE (PCODE) }
#define P4_SXCO( NM, SEM)       { "X\377"NM, (p4code)&P4SEMANTICS(SEM) }
#define P4_XXCO( NM, PCODE)     { "x\237"NM, &P4CODE (PCODE) } /* AUTO-INIT */
#define P4_RTCO( NM, RUN)       { "r\237"NM, (p4code)&P4RUNTIME_(RUN) }

#define P4_FXco( NM, PCODE)     { "p\237"NM, &P4CODE (PCODE) }
#define P4_IXco( NM, PCODE)     { "P\377"NM, &P4CODE (PCODE) }
#define P4_SXco( NM, SEM)       { "X\377"NM, (p4code)&P4SEMANTICS(SEM) }
#define P4_XXco( NM, PCODE)     { "x\237"NM, &P4CODE (PCODE) } /* AUTO-INIT */
#define P4_RTco( NM, RUN)       { "r\237"NM, (p4code)&P4RUNTIME_(RUN) }

#define P4_OVAR( NM)            { "v\237"NM, ((p4code)0) }
#define P4_OCON( NM, VAL)       { "c\237"NM, (p4code)(VAL) }
#define P4_OVAL( NM, VAL)       { "l\237"NM, (p4code)(VAL) }
#define P4_OFFS( NM, VAL)       { "o\237"NM, (p4code)(VAL) }

#define P4_IVAR( NM)            { "V\377"NM, ((p4code)0) }
#define P4_ICON( NM, VAL)       { "C\377"NM, (p4code)(VAL) }
#define P4_IVAL( NM, VAL)       { "L\377"NM, (p4code)(VAL) }

#define P4_OVaR( NM)            { "v\237"NM, ((p4code)0) }
#define P4_OCoN( NM, VAL)       { "c\237"NM, (p4code)(VAL) }
#define P4_OVaL( NM, VAL)       { "l\237"NM, (p4code)(VAL) }
#define P4_OFFs( NM, VAL)       { "o\237"NM, (p4code)(VAL) }

#define P4_IVaR( NM)            { "V\377"NM, ((p4code)0) }
#define P4_ICoN( NM, VAL)       { "C\377"NM, (p4code)(VAL) }
#define P4_IVaL( NM, VAL)       { "L\377"NM, (p4code)(VAL) }

/* -def      VO( NM, LIST)      { "\112"NM, (p4code)(&P4WORDS(LIST,WList)) } */

#define P4_DVAR( NM, MEMBER)    { "m\237"NM, (p4code)OFFSET_OF(p4_Thread, MEMBER) }
#define P4_DCON( NM, MEMBER)    { "n\237"NM, (p4code)OFFSET_OF(p4_Thread, MEMBER) }
/* -def P4_DSET( NM, MEMBER)    { "t\237"NM, (p4code)OFFSET_OF(p4_Thread, MEMBER) } */

/* -def P4_DVaH( NM, MEMBER)    { "h\237"NM, (p4code)OFFSET_OF(p4_Thread, MEMBER) } */
#define P4_DVaR( NM, MEMBER)    { "m\237"NM, (p4code)OFFSET_OF(p4_Thread, MEMBER) }
#define P4_DVaL( NM, MEMBER)    { "n\237"NM, (p4code)OFFSET_OF(p4_Thread, MEMBER) }
/* -def P4_DSeT( NM, MEMBER)    { "t\237"NM, (p4code)OFFSET_OF(p4_Thread, MEMBER) } */

#define P4_SNYM( NM, OLDNAME)   { "Y\377"NM, (p4code)OLDNAME }
#define P4_FNYM( NM, OLDNAME)   { "y\237"NM, (p4code)OLDNAME }
#define P4_iOLD( NM, OLDNAME)   { "Z\377"NM, (p4code)OLDNAME }
#define P4_xOLD( NM, OLDNAME)   { "z\237"NM, (p4code)OLDNAME }

#define P4_SLOT( NM, SLOTVAR)   { "s\237"NM, (p4code)(SLOTVAR) }
#define P4_SSIZ( NM, SIZE)      { "S\377"NM, (p4code)(SIZE) }

#define P4_OVOC( NM, CHAIN)     { "w\237"NM, (p4code)(CHAIN) }
#define P4_IVOC( NM, CHAIN)     { "W\237"NM, (p4code)(CHAIN) }
#define P4_NEED( NM)            { "q\237"NM }
#ifndef __WATCOMC__
#define P4_INTO( NM, DESCR)     { "i\377"NM, (p4code)(DESCR) } /*search-also*/
#else
#define P4_INTO( NM, DESCR)     { "i\377"NM, 0 } /*search-also*/
#endif
#define P4_LOAD( NM, WORDS)     { "I\377"NM, (p4code)(&P4WORDS(WORDS)) }

#define P4_EXPT( NM, ID)        { "e\237"NM, (p4code)(ID) } /*exception*/
#define P4_SHOW( NM, ID)        { "d\337"NM, (p4code)(ID) } /*message*/
#define P4_DEPR( NM, ID)        { "D\337"NM, (p4code)(ID) } /*deprecated*/

#define P4_STKi( NM, INFO)      { "M\377"NM, (p4code)(INFO) } /*stackhelp*/
#define P4_STKx( NM, PCODE)     { "N\377"NM, &P4CODE(PCODE) } /*stackhelp*/


/* ------------------------
   defining a load list
   ------------------------  */

#define P4_LOAD_END  ((void*)0)
#define P4_LOAD_INTO ((void*)1)
#define P4_LOAD_EXEC ((void*)2)
#define P4_LOAD_ORDER ((void*)3)
#define P4_LOAD_SLOT ((void*)4)
#define P4_LOAD_MAX  ((void*)8)

#define P4_LOADSTRUCT(NAME) P4CAT3(p4_,NAME,_LoadList)
#define P4_LOADLIST(NAME) const void* P4_LOADSTRUCT(NAME) []

#ifdef MODULE
#define P4_MODULE_LIST(name) \
        void* p4_LTX_p4_MODULE(void) \
        {                       \
           return (void*) &P4WORDS(name); \
        }
#else
#define P4_MODULE_LIST(name) \
        void* name##_LTX_p4_MODULE(void) \
        {                       \
           return (void*) &P4WORDS(name); \
        }
#endif

/*@}*/
#endif
