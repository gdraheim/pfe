/** 
 * -- miscellaneous useful extra words for TOOLS-EXT
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version:  1.15 %
 *    (%date_modified:  Tue Apr 16 11:59:23 2002 %)
 *
 *  @description
 *      Compatiblity with former standards, miscellaneous useful words.
 *      ... for TOOLS-EXT
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: tools-mix.c,v 1.1.1.1 2006-08-08 09:09:03 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>

#include <errno.h>

/** VLIST ( -- )
 *  The VLIST command had been present in FIG and other forth
 *  implementations. It has to list all accessible words. In PFE
 *  it list all words in the search order. Well, the point is,
 *  that we do really just look into the search order and are
 *  then calling => WORDS on that Wordl. That way you can see
 *  all accessible words in the order they might be found.
 *  Uses => ?CR
 */
FCode (p4_vlist) 
{
    extern int p4_Q_cr (void);
    Wordl **p, **q;
    
    for (p = CONTEXT; p <= &ONLY; p++)
    {
        if (*p == NULL)
            continue;
        for (q = CONTEXT; *q != *p; q++); /* don't search wordl twice */
        if (q != p)
            continue;
        p4_dot_name ((*p)->nfa);
        p4_outs ("WORDS");
        p4_wild_words ((*p), "*", NULL);
        
        if (p < &ONLY) {
            PFE.more = 0; if (p4_Q_cr ()) return; 
        }
    }
}

/** !CSP ( -- )
 * put => SP into => CSP
 * <br> used in control-words
 */
FCode (p4_store_csp)
{
    CSP = SP;
}

/** ?CSP ( -- )
 * check that => SP == => CSP otherwise => THROW
 * <br> used in control-words
 */
FCode (p4_Q_csp)	
{
    if (SP != CSP)
        p4_throw (P4_ON_CONTROL_MISMATCH);
}

/** ?COMP ( -- )
 * check that the current => STATE is compiling
 * otherwise => THROW
 * <br> often used in control-words
 */
FCode (p4_Q_comp)		
{
    if (!STATE)
        p4_throw (P4_ON_COMPILE_ONLY);
}

/** ?EXEC ( -- )
 * check that the current => STATE is executing
 * otherwise => THROW
 * <br> often used in control-words
 */
FCode (p4_Q_exec)		
{
    if (STATE)
        p4_throw (P4_ON_COMPILER_NESTING);
}

/** ?FILE ( file-id -- )
 * check the file-id otherwise (fixme)
 */
FCode (p4_Q_file)
{
    int ior = *SP++;

    if (ior)
        p4_throw (FX_IOR);
}

/** ?LOADING ( -- )
 * check that the currently interpreted text is 
 * from a file/block, otherwise => THROW
 */
FCode (p4_Q_loading)
{
    if (BLK == 0)
        p4_throw (P4_ON_INVALID_BLOCK);
}

/** ?PAIRS ( a b -- )
 * if compiling, check that the two magics on
 * the => CS-STACK are identical, otherwise throw
 * <br> used in control-words
 */
FCode (p4_Q_pairs)
{
    FX (p4_Q_comp);
    p4_Q_pairs (*SP++);
}

/** ?STACK ( -- )
 * check all stacks for underflow and overflow conditions,
 * and if such an error condition is detected => THROW
 */
FCode (p4_Q_stack)
{
#  ifdef P4_RP_IN_VM
    if (RP > PFE.r0)	    p4_throw (P4_ON_RSTACK_UNDER);
    if (RP < PFE.rstack)    p4_throw (P4_ON_RSTACK_OVER);
#  endif
    if (SP > PFE.s0)	    p4_throw (P4_ON_STACK_UNDER);
    if (SP < PFE.stack)	    p4_throw (P4_ON_STACK_OVER);
#  ifndef P4_NO_FP
    if (FP > PFE.f0)	    p4_throw (P4_ON_FSTACK_UNDER);
    if (FP < PFE.fstack)    p4_throw (P4_ON_FSTACK_OVER);
#  endif
    if (PFE.dictlimit - PFE_MINIMAL_UNUSED < PFE.dp) 
        p4_throw (P4_ON_DICT_OVER);  
}

/* ______________________________________________________________________ */
/* definitions checks */

/** [VOID]                ( -- flag )
 *  Immediate FALSE. Used to comment out sections of code.
 *  IMMEDIATE so it can be inside definitions.
 : [VOID] 0 ; immediate
 */

/** DEFINED             ( "name" -- flag )
 *  Search the dictionary for _name_. If _name_ is found,
 *  return TRUE; otherwise return FALSE. Immediate for use in
 *  definitions.
  
 * This word will actually return what => FIND returns (the NFA). 
 * does check for the word using find (so it does not throw like => ' )
 * and puts it on stack. As it is immediate it does work in compile-mode
 * too, so it places its argument in the cs-stack then. This is most
 * useful with a directly following => [IF] clause, so that sth. like
 * an <c>[IFDEF] word</c> can be simulated through <c>[DEFINED] word [IF]</c>

 : DEFINED BL WORD COUNT (FIND-NFA) ; 
 */

/** [DEFINED]             ( [name] -- flag )
 *  Search the dictionary for _name_. If _name_ is found,
 *  return TRUE; otherwise return FALSE. Immediate for use in
 *  definitions.
  
 * This word will actually return what => FIND returns (the NFA). 
 * does check for the word using find (so it does not throw like => ' )
 * and puts it on stack. As it is immediate it does work in compile-mode
 * too, so it places its argument in the cs-stack then. This is most
 * useful with a directly following => [IF] clause, so that sth. like
 * an <c>[IFDEF] word</c> can be simulated through <c>[DEFINED] word [IF]</c>

 : [DEFINED] DEFINED ; IMMEDIATE
 : [DEFINED] BL WORD COUNT (FIND-NFA) ; IMMEDIATE
 */
FCode (p4_defined)
{
    p4_word_parseword (' '); *DP=0; /* PARSE-WORD-NOHERE */
    FX_PUSH (p4_find (PFE.word.ptr, PFE.word.len));
}
 
/** [UNDEFINED]          ( [name] -- flag )
 *  Search the dictionary for _name_. If _name_ is found,
 *  return FALSE; otherwise return TRUE. Immediate for use in
 *  definitions.
 *
 *  see => [DEFINED]
 : [UNDEFINED] DEFINED 0= ; IMMEDIATE
 */
FCode (p4_undefined)
{
    FX (p4_defined);
    FX (p4_zero_equal);
}

/* ______________________________________________________________________ */
/* dictionary limits */

/** (FORGET) ( addr -- )
 * forget everything above addr
 * - used by => FORGET
 */
FCode (p4_paren_forget)		
{			
    p4_forget ((p4_byte_t *) *SP++);
}

/** (DICTLIMIT)   ( -- constvalue )
 * the upper limit of the forth writeable memory space,
 * the variable => DICTLIMIT must be below this line.
 * stack-space and other space-areas are often allocated
 * above => DICTLIMIT upto this constant.
 *
 * => DICTFENCE is the lower end of the writeable dictionary
 */
FCode(p4_paren_dictlimit)
{
    FX_PUSH ((((p4char*) PFE_MEM) + PFE_set.total_size));
} 

/** (DICTFENCE)   ( -- constvalue )
 * the lower limit of the forth writeable memory space,
 * the variable => DICTFENCE must be above this line.
 * Some code-areas are often moved in between => DICTFENCE and
 * this constant. To guard normal Forth code from deletion
 * the usual practice goes with the => FENCE variable
 *
 * => DICTLIMIT is the upper end of the writeable dictionary
 */
FCode(p4_paren_dictfence)
{
    FX_PUSH (PFE_MEM);
} 

/** FENCE        ( -- var* )
 * a forth system variable - => (FORGET) will not work below
 * this address and any => FORGET on a header below this mark
 * will => THROW
 */

/** DICTFENCE   ( -- var* )
 * the lower end of usable area - the forth memory block minus the 
 * forth-VM backstore. Note that this is a variable by tradition but
 * you should not move it.
 */

/** DICTLIMIT   ( -- var* )
 * the lower end of usable area - the forth memory block minus the 
 * forth-related DICTALLOCS at the upper end (e.g. => POCKET-PAD )
 * Note that this is a variable by tradition but you should not move it.
 */

P4_LISTWORDS (tools_misc) =
{
    P4_INTO ("FORTH", "[ANS]"),
    P4_FXco ("VLIST",		p4_vlist),

    /** see => !CSP and ?CSP */
    P4_DVaR ("CSP",		csp),
    P4_FXco ("!CSP",		p4_store_csp),
    P4_FXco ("?CSP",		p4_Q_csp),
    P4_FXco ("CS-SWAP",		p4_two_swap),
    P4_FXco ("CS-DROP",		p4_two_drop),

    /* state checks */
    P4_FXco ("?COMP",		p4_Q_comp),
    P4_FXco ("?EXEC",		p4_Q_exec),
    P4_FXco ("?FILE",		p4_Q_file),
    P4_FXco ("?LOADING",	p4_Q_loading),
    P4_FXco ("?PAIRS",		p4_Q_pairs),
    P4_FXco ("?STACK",		p4_Q_stack),

    /* definition checks */
    P4_ICoN ("[VOID]",       0),
    P4_FXco ("DEFINED",      p4_defined),
    P4_IXco ("[DEFINED]",    p4_defined),
    P4_IXco ("[UNDEFINED]",  p4_undefined),

    P4_INTO ("EXTENSIONS",	0),

    /** dictionary area dividers */
    P4_FXco ("(FORGET)",	p4_paren_forget),
    P4_DVaR ("FENCE",		fence),       
    P4_DVaR ("DICTLIMIT",	dictlimit),   
    P4_DVaR ("DICTFENCE",	dict),        
    P4_FXco ("(DICTLIMIT)",	p4_paren_dictlimit), 
    P4_FXco ("(DICTFENCE)",	p4_paren_dictfence), 

    /** implementation specific magic - used by control words */
    P4_OCoN ("EXCEPTION_MAGIC", P4_EXCEPTION_MAGIC),
    P4_OCoN ("INPUT_MAGIC",	P4_INPUT_MAGIC),
    P4_OCoN ("DEST_MAGIC",	P4_DEST_MAGIC),
    P4_OCoN ("ORIG_MAGIC",	P4_ORIG_MAGIC),
    P4_OCoN ("LOOP_MAGIC",	P4_LOOP_MAGIC),
    P4_OCoN ("CASE_MAGIC",	P4_CASE_MAGIC),
    P4_OCoN ("OF_MAGIC",	P4_OF_MAGIC),

};
P4_COUNTWORDS (tools_misc, "TOOLS-Misc Compatibility words");

/*@}*/
/* 
 * Local variables:
 * c-file-style: "stroustrup"
 * End:
 */


