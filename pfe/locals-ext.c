/** 
 * -- The Optional Locals Word Set
 * 
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2008-04-20 04:46:31 $)
 *
 *  @description
 *      The Portable Forth Environment does implement locals
 *      in such an extended form as that additional variable
 *      names can be declared anywhere in the compiled word.
 *
 *      Locals are names for values that live in a locals-frame
 *      on the return-stack - on entry to the procedure that
 *      locals-frame is carved from the return-stack and a 
 *      frame-pointer is setup. Locals are in two forms, one
 *      is inialized by a chunk from the parameter-stack as
 *      it is with => LOCALS| while the others are local variables
 *      declared later. The latter are left unitialized on
 *      setup of the locals-frame. 
 *
 *      For unnamed returnstack locals, see words like 
 *      =>"R@" =>"R!" =>"R'@" =>"R'!" =>'R"@' =>'R"!' =>'2R@' =>'2R!'
 *      but here the setup and cleanup of the return-stack frame
 *      is left to the user, possibly using some words like
 *      =>">R" =>"R>" =>"2>R" =>"2R>" =>"R>DROP" while the locals-ext
 *      will take care to provide a frame-creation token and
 *      some cleanup-code for each => EXIT or => ;
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: locals-ext.c,v 1.3 2008-04-20 04:46:31 guidod Exp $";
#endif

#define _P4_SOURCE 1
#include <pfe/pfe-base.h>
#include <pfe/def-limits.h>

#include <pfe/os-string.h>

#include <pfe/def-comp.h>
#include <pfe/logging.h>

/** 
      Did change the locals-implementation completely.
  Now the return stack looks like this after locals_bar_execution:

  rp --> 'rp' -- contains real return used by locals_exit_execution
  lp --> 'lp' -- contains locals-pointer of surrounding scope
          arg1
           ..
          argN
          localN+1
            ..
          localM
  'rp' --> -- real return address

   you can declare now a local non-arg at any time.
   All local-args must be declared before any local-non-arg.
   Not that the code after locals_bar_execution does not contain 
   _two_ numbers N and M. Classic use of LOCALS| will put them
   at the very same value.
*/   

/* 1. Actions at runtime: */
/* establish local variables on return stack */
FCode_XE (p4_local_enter_execution) 
{   FX_USE_CODE_ADDR {
    p4cell *Rp = FX_RP;

    Rp = FX_RP - P4_POP(IP);
#  ifdef P4_RP_IN_VM
    P4_POSH(Rp) = (p4cell) LP;  LP = Rp;
    P4_POSH(Rp) = (p4cell) RP;  RP = (p4xcode**) Rp;
#  else
    /* FIXME: PFE_SBR_CALL_THREADING mode */
#  endif
    FX_USE_CODE_EXIT;

    /* P4_SKIPS_CELL + x */
}}

FCode_XE (p4_local_args)
{
    FX_USE_CODE_ADDR;
    p4_memcpy (LP+1, SP, (*(p4cell*)IP) * sizeof(p4cell));
    SP += P4_POP(IP);
    FX_USE_CODE_EXIT;
    /* P4_SKIPS_CELL */
} 


FCode_XE (p4_locals_bar_execution)
{
    FX_USE_CODE_ADDR_UNUSED;
    FX (p4_local_enter_execution);  /* FIXME: forward USE_CODE_ADDR */
    FX (p4_local_args);             /* FIXME: forward USE_CODE_ADDR */
    FX_USE_CODE_EXIT_UNUSED;
    /* P4_SKIPS_DCELL + x */
}

/* alternative EXIT,
 * cleans up local variable stack frame 
 */
FCode_XE (p4_locals_exit_execution)       
{
#  ifdef P4_RP_IN_VM
    FX_USE_CODE_ADDR;
    LP = (p4cell *) RP[1];
    RP = (p4xcode **) *RP;
    IP = *RP++;
    FX_USE_CODE_EXIT;
#  else
    P4_fail("LOCALS EXIT not implemented");
    /* FIXME: PFE_SBR_CALL_THREADING mode */
#  endif
}

/* retrieve current value of local variable */
FCode_XE (p4_local_execution)
{
    FX_USE_CODE_ADDR;
    P4_POSH(SP) = LP[P4_POP(IP)];     /*  had been: LP[P4_POP(*IP)]; */
    FX_USE_CODE_EXIT;
}

/* set current value of local variable */
FCode_XE (p4_to_local_execution) 
{
    FX_USE_CODE_ADDR;
    LP[P4_POP(IP)] = P4_POP(SP);
    FX_USE_CODE_EXIT;
}

/* 2. Actions at compile time */

/* returns index i to access local variable,
 * relative to LP [i], 0 if not defined
 */
int
p4_find_local (const p4_char_t* nm, int l)
{
    int i;
    /* TBD: why use zero-terminated strings for locals ? strncmp is heavy!*/

    if (! *PFE.locals) 
        return 0; /* shortcut */

    for (i = 0; i < *PFE.locals; i++)
	if (p4_strncmp ((char*) nm, (char*) PFE.local[i], l) == 0 && 
	    PFE.local[i][l] == 0)
	    return i + 1;
    return 0;
}

/* uses PFE.word.ptr and PFE.word.len as arguments */
int
p4_word_compile_local (void)
{
    static p4code cfa = PFX (p4_local_execution);
    int n;
    
    if (! (n = p4_find_local (PFE.word.ptr, PFE.word.len)))
	return 0;
    FX_XCOMMA (&cfa);
    FX_UCOMMA (n);
    return 1;
}

static void 
enter_locals(void)
{
    PFE.locals = (p4cell *) DP;
    FX_UCOMMA (0);
    FX_UCOMMA (0);
}

/* uses PFE.word.ptr and PFE.word.len as arguments */
static void
p4_word_paren_local (void)
{
    FX (p4_Q_comp);
    if (PFE.word.len == 0) return;
    if (PFE.word.len > NFACNTMAX ) { p4_throw (P4_ON_NAME_TOO_LONG); return; }

    if (PFE.locals == NULL) {
        FX_COMPILE (p4_locals_bar); /* p4_locals_bar_execution */
        enter_locals ();
    }else{
        if (p4_find_local (PFE.word.ptr, PFE.word.len)) { 
            p4_word_to_here (); /* fixme: not needed if throw changed */
            p4_throw (P4_ON_INVALID_NAME);
	    return;
        }
    }
    p4_store_c_string (PFE.word.ptr, PFE.word.len, 
		       PFE.local[(*PFE.locals)++], NFACNTMAX+1);
}

/** (LOCAL)          ( strptr strcnt -- )
 * this word is used to create compiling words that can
 * declare => LOCALS| - it shall not be used directly
 * to declare a local, the pfe provides => LVALUE for
 * that a purpose beyond => LOCALS| 
 */
FCode (p4_paren_local)
{
    /* this function is rarely used - other than our word_paren_local above */
    if ((p4char*) SP[1] == PFE.word.ptr	&& SP[0] == PFE.word.len)
    {
	p4_word_paren_local ();
    }else{
	p4cchar* ptr = PFE.word.ptr; 
	p4ucell  len = PFE.word.len; 
	PFE.word.ptr = (p4char*) SP[1];
	PFE.word.len = SP[0];
	p4_word_paren_local ();
	PFE.word.ptr = ptr;
	PFE.word.len = len;
	SP += 2;
	PFE.locals[1] ++;
    }
}
P4COMPILES (p4_paren_local, p4_locals_bar_execution,
  P4_SKIPS_DCELL, P4_LOCALS_STYLE);

/** LOCALS|   ( xN ... x2 x1 [name1 .. nameN <|>] -- )
 * create local identifiers to be used in the current definition.
 * At runtime, each identifier will be assigned a value from
 * the parameter stack. <br>
 * The identifiers may be treated as if being a => VALUE , it does
 * also implement the ansi => TO extensions for locals. Note that
 * the identifiers are only valid inside the currently compiled
 * word, the => SEE decompiled word will show them as 
 * => <A> => <B> ... => <N> a.s.o.   <br>
 * see also => LVALUE
 */
FCode (p4_locals_bar)
{
    for (;;)
    {
        register int ignore = 0;
        p4_word_parseword (' '); *DP=0; /* PARSE-WORD-NOHERE */
	/* if (! PFE.word.len) ?? REFILL or THROW ?? */
        
        if (PFE.word.len == 1 && *PFE.word.ptr == '|')
            break;
	if (ignore) 
	    continue;

	if (PFE.word.len == 2 
	    && PFE.word.ptr[0] == '-' 
	    && PFE.word.ptr[1] == '-')
	    ignore = 1;
	else
	{
	    p4_word_paren_local ();
	    PFE.locals[1] ++;
	}
    }
}
P4COMPILES (p4_locals_bar, p4_locals_bar_execution,
  P4_SKIPS_DCELL, P4_LOCALS_STYLE);

/** LVALUE ( value [name] -- )
 * declares a single local => VALUE using => (LOCAL) - a 
 * sequence of => LVALUE declarations can replace a 
 * => LOCALS| argument, ie. <c> LOCALS| a b c | </c> 
 * is the same as <c> LVALUE a  LVALUE b  LVALUE c </c>.
 * This should also clarify the runtime stack behaviour of
 * => LOCALS| where the stack parameters seem to be
 * assigned in reverse order as opposed to their textual
 * identifier declarations. <br>
 * compare with => VALUE and the pfe's convenience word
 * =>'VAR'.
 *
 : LVALUE 
   STATE @ IF 
     VALUE 
   ELSE 
     BL WORD COUNT DUP (LOCAL) (TO)
   THEN
 ; IMMEDIATE
 */
FCode (p4_local_value)
{
    if (! STATE) 
    { 
	FX (p4_value);
    }else{
	if (! PFE.locals)    /* need to build a dummy (LOCAL) entry */
	{
	    FX_COMPILE (p4_paren_local); 
	    enter_locals ();
	}
	p4_word_parseword (' '); *DP=0; /* PARSE-WORD-NOHERE */
	p4_word_paren_local ();
	FX_COMPILE2 (p4_local_value);
	FX_UCOMMA (p4_find_local (PFE.word.ptr, PFE.word.len));
    }
}

P4COMPILES2(p4_local_value, p4_to_execution, p4_to_local_execution, 
  P4_SKIPS_TO_TOKEN, P4_LOCALS_STYLE);

FCode (p4_local_buffer_var_TO)
{
#  ifdef P4_RP_IN_VM
    *SP = (p4cell)(( RP -= *SP ));
    FX (p4_to_local_execution);
#  else
    /* FIXME: PFE_SBR_CALL_THREADING mode */
#  endif
}

/** LBUFFER: ( size [name] -- )
 * declares a single local => VALUE using => (LOCAL) - which
 * will hold the address of an area like => BUFFER: but carved
 * from the return-stack (as in C with alloca). This local buffer
 * will be automatically given up at the end of the word. The
 * return-stack-pointer will be increased only at the time of
 * this function (and the address assigned to the =>"LVALUE")
 * so that the provided size gets determined at runtime. Note
 * that in some configurations the forth-return-stack area is
 * quite small - for large string operations you should consider
 * to use a => POCKET-PAD in pfe.
 : LBUFFER:
   STATE @ IF 
     BUFFER:
   ELSE 
     :NONAME ( size -- rp* ) R> RP@ - DUP RP! SWAP >R ;NONAME
     COMPILE, POSTPONE LVALUE
   THEN
 ; IMMEDIATE
 */
FCode (p4_local_buffer_var)
{
    if (! PFE.locals)    /* need to build a dummy (LOCAL) entry */
    {
        FX_COMPILE (p4_paren_local); 
        enter_locals ();
    }
    p4_word_parseword (' '); *DP=0; /* PARSE-WORD-NOHERE */
    p4_word_paren_local ();
    FX_COMPILE2 (p4_local_value);
    FX_UCOMMA (p4_find_local (PFE.word.ptr, PFE.word.len));
}

P4COMPILES2(p4_local_buffer_var, p4_to_execution, p4_local_buffer_var_TO, 
  P4_SKIPS_TO_TOKEN, P4_LOCALS_STYLE);

static p4ucell 
FXCode (p4_interpret_locals) /* hereclean */
{
    if (! STATE || ! PFE.locals) return 0; /* quick path */
    /* WORD-string is at HERE and at PFE.word.ptr / PFE.word.len */
    return p4_word_compile_local ();
}

static FCode(locals_init)
{
    PFE.interpret[5] = PFX (p4_interpret_locals);
}

/** "ENVIRONMENT #LOCALS" ( -- number )
 * the number of local names allowed during compilation.
 * portable programs can check this with => ENVIRONMENT?
 */

P4_LISTWORDS (locals) =
{
    P4_INTO ("[ANS]", 0),
    P4_SXco ("(LOCAL)",		p4_paren_local),
    P4_SXco ("LOCALS|",		p4_locals_bar),

    P4_INTO ("EXTENSIONS", 0),
    P4_SXco ("LVALUE",		p4_local_value),
    P4_SXco ("LBUFFER:",	p4_local_buffer_var),

    P4_INTO ("ENVIRONMENT", 0 ),
    /* enviroment hints (testing for -EXT will mark this wordset as present) */
    P4_OCON ("LOCALS-EXT",	1994 ),
    P4_OCON ("#LOCALS",		MAX_LOCALS ),
    P4_XXco ("LOCALS-LOADED",   locals_init),
};
P4_COUNTWORDS (locals, "Locals + extensions");

/*@}*/

