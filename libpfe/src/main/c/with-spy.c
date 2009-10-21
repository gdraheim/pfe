/**
 * -- SPY nest threading extension
 *
 *  Copyright (C) Tektronix, Inc. 2000 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.4 $
 *     (modified $Date: 2008-05-01 00:42:01 $)
 *
 *  @description
 *              These words are of old Tek CTE/MPT usage - it is
 *              quite useful for debugging, but in a portable
 *              environment it just means a minor speed penalty
 *              even if => SPY_OFF and a great speed penalty
 *              if SPY_ON. The default should be to not include
 *              it but some special Tek builds.
 *
 *              To use it: enable the SPY' vocabulary when compiling
 *              colon words with spy-support. Later on, enable the
 *              spy-routine with SPY_ON and disable with SPY_OFF.
 *              The default spy-routine will print each word on enter
 *              and leave along with the stack contents at that point.
 *              You can attach customized entries into VALUEs named
 *              SPY-ENTER and SPY-LEAVE. Within a token-threaded
 *              environment, the CFA of the current entry can be gotten
 *              with
 *                                  R> DUP >R CELL - @
 *              and most entries will give you a good >NAME for it.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) =
"@(#) $Id: with-spy.c,v 1.4 2008-05-01 00:42:01 guidod Exp $";
#endif

#define _P4_SOURCE 1
#include <pfe/pfe-base.h>

#if defined PFE_WITH_SPY
#include <pfe/def-restore.h>

# ifndef PFE_SBR_CALL_THREADING
static void p4_spy_execute (p4xt V)
{
    void (*saved)(int);
    P4_CALLER_MKSAVED;
    saved = PFE.spy_nest; PFE.spy_nest = 0;
    p4_call (V);       PFE.spy_nest = saved;
    P4_CALLER_RESTORE;
}
# endif

/*
 * runtime portion callback. This routine is
 * called at the end of a => (NEST) that is
 * compiled by => : - it will print the name
 * of the colon word along with the current
 * param stack.
 */
static void p4_spy_nest(int enter)
{
#  if !defined PFE_SBR_CALL_THREADING
    if (p4_OUT) FX (p4_cr);
    if (enter) {
        if (PFE.spy_enter) { p4_spy_execute (PFE.spy_enter); return; }
    }else{
        if (PFE.spy_leave) { p4_spy_execute (PFE.spy_leave); return; }
    }

    { /* colon word name lookup */
        static const char spaces[] = "                           ";
#  if defined PFE_CALL_THREADING
        if (enter) {
#  endif
            p4char* nfa = p4_to_name ((*RP)[-1]);
            int len = NAMELEN(nfa);  if (len > 27) len = 27;
            p4_outf ("[%02d%s] %.*s%.*s ", p4_R0-RP, enter ? ">>" : "<<",
              len, NAMEPTR(nfa), 28-len, spaces);
#  if defined PFE_CALL_THREADING
        } else { p4_outf("[%02d<<]----------------------------", p4_R0-RP); }
#  endif
    }
    {  /* parameter stack */
        int len = p4_S0 - SP;
        if (len) p4_outf ("[%02d@] ", len);
        if (len > 6) len = 6;
        while (--len >= 0)
        {
            p4_outf("%x ", p4SP[len]);
        }
        p4_outs ("\n");
    }
#  endif
}

/** SPY_ON ( -- )
 * change the runtime-code of => (NEST)
 * to call a special word that prints info
 * to the screen whenever a colon word is
 * entered. It will print the name and
 * the current stack, and results in a kind
 * of execution trace over =>"SPY' :"-colon nested
 * words.
 */
void FXCode (p4_spy_on)
{
    PFE.spy_nest = p4_spy_nest;
}

/** SPY_OFF ( -- )
 * disable => SPY_ON nest-trace.
 */
void FXCode (p4_spy_off)
{
    PFE.spy_nest = 0;
}

/** "SPY' (NEST)" ( -- )
 * compiled by => :
 */
void FXCode_RT (p4_spy_colon_RT)
{   FX_USE_BODY_ADDR {
#  if ! defined PFE_CALL_THREADING
    FX_PUSH_RP = IP; IP = (p4xt *) FX_POP_BODY_ADDR;
    if (PFE.spy_nest) PFE.spy_nest(1);
#  elif ! defined PFE_SBR_CALL_THREADING
    FX_POP_BODY_ADDR_p4_BODY;
    FX_PUSH_RP = IP; IP = (p4xcode *) p4_BODY;
    if (PFE.spy_nest) PFE.spy_nest(1);
#  else
    p4code c = (p4code) FX_POP_BODY_ADDR;
    if (PFE.spy_nest) PFE.spy_nest(1);
    c ();
#  endif
}}

/** "SPY' :" ( name -- )
 * =>"SPY:"
 */

/** "SPY:" ( name -- )
 * create a header for a nesting word and go to compiling
 * mode then. This word is usually ended with => ; but
 * the execution of the resulting colon-word can also
 * return with => EXIT
 * this is the spy-version => SPY_ON
 */
void FXCode (p4_spy_colon)
{
    FX (p4_Q_exec);
    FX_RUNTIME_HEADER; FX_SMUDGED;
    FX_RUNTIME1 (p4_spy_colon);
    FX (p4_store_csp);
    STATE = P4_TRUE;
    PFE.locals = NULL;
    PFE.semicolon_code = PFX(p4_colon_EXIT);
}
P4RUNTIME1(p4_spy_colon, p4_spy_colon_RT);

/** "SPY' ((SPY;))" ( -- )
 * compiled by => ; and maybe => ;AND --
 * it will perform an => EXIT
 * this is the SPY-version
 */                                /*"((SPY;))"*/
void FXCode_XE (p4_spy_semicolon_execution)
{
    FX_USE_CODE_ADDR;
    if (PFE.spy_nest) PFE.spy_nest(0);
    IP = *RP++;
    FX_USE_CODE_EXIT;
}

/** "SPY' ;" ( -- )
 * =>";SPY"
 */

/** ";SPY" ( -- )
 * compiles => ((;)) which does => EXIT the current
 * colon-definition. It does then end compile-mode
 * and returns to execute-mode. See => : and => :NONAME
 */
void FXCode (p4_spy_semicolon)
{
    if (PFE.locals)
    {
        FX_COMPILE2 (p4_spy_semicolon);
        PFE.locals = NULL;
    }
    else
        FX_COMPILE1 (p4_spy_semicolon);

    if (PFE.semicolon_code)
    {
        PFE.semicolon_code ();
    }else{
        PFE.state = P4_FALSE; /* atleast switch off compiling mode */
    }
}

P4COMPILES2 (p4_spy_semicolon,
  p4_spy_semicolon_execution, p4_locals_exit_execution,
  P4_SKIPS_NOTHING, P4_SEMICOLON_STYLE);

/** "SPY' EXIT" ( -- )
 * =>"SPY-EXIT"
 */

/** "SPY-EXIT" ( -- )
 * will unnest the current colon-word so it will actually
 * return the word calling it. This can be found in the
 * middle of a colon-sequence between => : and => ;
 */
void FXCode (p4_spy_exit)
{
    if (PFE.locals)
        FX_COMPILE2 (p4_spy_exit);
    else
        FX_COMPILE1 (p4_spy_exit);
}
P4COMPILES2 (p4_spy_exit, p4_spy_semicolon_execution, p4_locals_exit_execution,
           P4_SKIPS_NOTHING, P4_DEFAULT_STYLE);


P4_LISTWORDSET (with_spy) [] =
{
    P4_INTO ("EXTENSIONS", 0),
    P4_FXco ("SPY-EXIT",      p4_spy_exit),
    P4_RTco ("SPY:",          p4_spy_colon),
    P4_SXco (";SPY",          p4_spy_semicolon),
    P4_FXco ("SPY_ON",	      p4_spy_on),
    P4_FXco ("SPY_OFF",       p4_spy_off),
    P4_DVaL ("SPY-ENTER",     spy_enter),
    P4_DVaL ("SPY-LEAVE",     spy_leave),

    P4_INTO ("SPY'", 0),
    P4_FNYM (":",             "SPY:"),
    P4_SNYM (";",             ";SPY"),
    P4_SNYM ("EXIT",          "SPY-EXIT"),
};
P4_COUNTWORDSET (with_spy, "WITH-SPY kernel extension");

#endif
    /*PFE_WITH_SPY*/

/*@}*/
