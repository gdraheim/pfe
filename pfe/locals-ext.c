/** 
 * -- The Optional Locals Word Set
 * 
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 5.8 %
 *    (%date_modified: Mon Mar 12 10:32:26 2001 %)
 *
 *  @description
 *      The Portable Forth Environment does implement locals
 *      in such an extended form as that additional variable
 *      names can be declared anywhere in the compiled word.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: locals-ext.c,v 0.32 2001-05-12 18:15:46 guidod Exp $";
#endif

#define _P4_SOURCE 1
#include <pfe/pfe-base.h>
#include <pfe/def-xtra.h>

#include <string.h>

#include <pfe/def-comp.h>


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
FCode (p4_local_enter_execution) 
{
    p4cell *Rp = FX_RP;

    Rp = FX_RP - P4_POP(IP);
    P4_POSH(Rp) = (p4cell) LP;  LP = Rp;
    P4_POSH(Rp) = (p4cell) RP;  RP = (p4xt**) Rp;

    /* P4_SKIPS_CELL + x */
}

FCode (p4_local_args)
{
    memcpy (LP+1, SP, (*(p4cell*)IP) * sizeof(p4cell));
    SP += P4_POP(IP);

    /* P4_SKIPS_CELL */
} 


FCode (p4_locals_bar_execution)
{
    FX (p4_local_enter_execution);
    FX (p4_local_args);
    /* P4_SKIPS_DCELL + x */
}

/* alternative EXIT,
 * cleans up local variable stack frame 
 */
FCode (p4_locals_exit_execution)       
{
    LP = (p4cell *) RP[1];
    RP = (p4xt **) *RP;
    IP = *RP++;
}

/* retrieve current value of local variable */
FCode (p4_local_execution)
{
    P4_POSH(SP) = LP[P4_POP(IP)];     /*  had been: LP[P4_POP(*IP)]; */
}

/* set current value of local variable */
FCode (p4_to_local_execution) 
{
    LP[P4_POP(IP)] = P4_POP(SP);
}

/* 2. Actions at compile time */

/* returns index i to access local variable,
 * relative to LP [i], 0 if not defined
 */
int
p4_find_local (char *nm, int l)
{
    int i;
    int (*cmp)(const char*, const char*, int) = 
     	(int (*)(const char*, const char*,int))strncmp;
    
    if (LOWER_CASE)
        cmp = (int (*)(const char*, const char*,int))strncmpi;
    for (i = 0; i < *PFE.locals; i++)
        if ((*cmp) (nm, PFE.local[i], l) == 0 && PFE.local[i][l] == 0)
            return i + 1;
    return 0;
}

int
p4_compile_local (char *name, int len)
{
    static p4code cfa = PFX (p4_local_execution);
    int n;
    
    if ((n = p4_find_local (name, len)) == 0)
    return 0;
    FX_COMMA (&cfa);
    FX_COMMA (n);
    return 1;
}

static void 
enter_locals(void)
{
    PFE.locals = (p4cell *) DP;
    FX_COMMA (0);
    FX_COMMA (0);
}

static void
paren_local (char *nm, int l)
{
    FX (p4_Q_comp);
    if (l == 0)
        return;
    if (l > NFACNTMAX ) 
        p4_throw (P4_ON_NAME_TOO_LONG);
    if (PFE.locals == NULL) {
        FX_COMPILE (p4_locals_bar); /* p4_locals_bar_execution */
        enter_locals ();
    }else{
        if (p4_find_local (nm, l)) { 
            p4_hereword (nm, l);
            p4_throw (P4_ON_INVALID_NAME);
        }
    }
    p4_store_c_string (nm, l, PFE.local[(*PFE.locals)++], NFACNTMAX+1);
}

/** (LOCAL)          ( strptr strcnt -- )
 * this word is used to create compiling words that can
 * declare => LOCALS| - it shall not be used directly
 * to declare a local, the pfe provides => LVALUE for
 * that a purpose beyond => LOCALS| 
 */
FCode (p4_paren_local)
{
    paren_local ((char *) SP[1], SP[0]);
    SP += 2;
    PFE.locals[1] ++;
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
        char *p; p4ucell l;
        p4_parseword (' ', &p, &l);
        
        if (l == 1 && *p == '|')
            break;
        paren_local (p, l);
        PFE.locals[1] ++;
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
 */
FCode (p4_local_value)
{
    char* nm; p4ucell l;
    if (!PFE.locals)    /* need to build a dummy (LOCAL) entry */
    {
        FX_COMPILE (p4_paren_local); 
        enter_locals ();
    }
    p4_parseword (' ', &nm, &l);
    paren_local (nm, l);
    FX_COMPILE2 (p4_local_value);
    FX_COMMA (p4_find_local (nm,l));
}

P4COMPILES2(p4_local_value, p4_to_execution, p4_to_local_execution, 
  P4_SKIPS_TO_TOKEN, P4_LOCALS_STYLE);

/** "ENVIRONMENT #LOCALS" ( -- number )
 * the number of local names allowed during compilation.
 * portable programs can check this with => ENVIRONMENT?
 */

P4_LISTWORDS (locals) =
{
    CS ("(LOCAL)", p4_paren_local),
    CS ("LOCALS|", p4_locals_bar),
    CS ("LVALUE",  p4_local_value),
    P4_INTO ("ENVIRONMENT", 0 ),
    /* enviroment hints (testing for -EXT will mark this wordset as present) */
    P4_OCON ("LOCALS-EXT",	1994 ),
    P4_OCON ("#LOCALS",		MAX_LOCALS ),
};
P4_COUNTWORDS (locals, "Locals + extensions");

/*@}*/

