/** 
 * -- SPY nest threading extension
 *
 *  Copyright (C) Tektronix, Inc. 2000 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 1.5 %
 *    (%date_modified: Mon Mar 12 10:33:10 2001 %)
 *
 *  @description
 *              These words are of old Tek CTE/MPT usage - it is
 *              quite useful for debugging, but in a portable
 *              environment it just means a minor speed penalty
 *              even if => SPY_OFF and a great speed penalty
 *              if SPY_ON. The default should be to not include
 *              it but some special Tek builds.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: with-spy.c,v 0.30.86.1 2001-03-12 09:33:11 guidod Exp $";
#endif

#define _P4_SOURCE 1
#include <pfe/pfe-base.h>

#if defined PFE_WITH_SPY

/** B(SPY)
 * runtime portion callback. This routine is
 * called at the end of a => (NEST) that is
 * compiled by => : - it will print the name
 * of the colon word along with the current
 * param stack.
 */
FCode (p4_spy_RT)
{
    static const char spaces[] = "                           ";
    p4char* nfa = p4_to_name ((*RP)[-1]);
    int len = NFACNT(*nfa);
    if (len > 27) len = 27;

    if (p4_OUT) FX (p4_cr);

    p4_outf ("SPY%2d%s %.*s%.*s ", p4_R0-RP, IP == *RP ? "+" : "-",
      len, nfa+1, 28-len, spaces);

    len = p4_S0-p4SP;
    if (len > 6) len = 6;
    while (--len >= 0)
    {
        p4_outf("%x ", p4SP[len]);
    }
    p4_outf ("[%d]\n", p4_S0 - SP);
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
FCode (p4_spy_on)
{
    PFE.spy_nest = PFX(p4_spy_RT);
}

/** SPY_OFF ( -- )
 * disable => SPY_ON nest-trace.
 */
FCode (p4_spy_off)
{
    PFE.spy_nest = 0;
}

/** "SPY' (NEST)" ( -- ) 
 * compiled by => :
 */ 
FCode (p4_spy_colon_RT)
{
    *--RP = IP;
    if (PFE.spy_nest) PFE.spy_nest();
    IP = (p4xt *) WP_PFA;
}

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
FCode (p4_spy_colon)
{
    FX (p4_Q_exec);
    p4_header (p4_spy_colon_RT_, P4xSMUDGED);
    FX (p4_store_csp);
    STATE = P4_TRUE;
    PFE.locals = NULL;
    PFE.semicolon_code = PFX(p4_colon_EXIT);
}

/** "SPY' ((SPY;))" ( -- ) 
 * compiled by => ; and maybe => ;AND --
 * it will perform an => EXIT
 * this is the SPY-version
 */                                /*"((SPY;))"*/
FCode (p4_spy_semicolon_execution)
{
    if (PFE.spy_nest) PFE.spy_nest(); 
    IP = *RP++;
}

/** "SPY' ;" ( -- )
 * =>";SPY"
 */

/** ";SPY" ( -- ) 
 * compiles => ((;)) which does => EXIT the current
 * colon-definition. It does then end compile-mode
 * and returns to execute-mode. See => : and => :NONAME
 */
FCode (p4_spy_semicolon)
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
FCode (p4_spy_exit)
{
    if (PFE.locals)
        FX_COMPILE2 (p4_spy_exit);
    else
        FX_COMPILE1 (p4_spy_exit);
}
P4COMPILES2 (p4_spy_exit, p4_spy_semicolon_execution, p4_locals_exit_execution,
	   P4_SKIPS_NOTHING, P4_DEFAULT_STYLE);

    
P4_LISTWORDS (with_spy) =
{
    CO ("SPY-EXIT",      p4_spy_exit),
    CO ("SPY:",          p4_spy_colon),
    CO (";SPY",          p4_spy_semicolon),
    CO ("SPY_ON",	 p4_spy_on),
    CO ("SPY_OFF",       p4_spy_off),
    P4_INTO ("SPY'",     0),
    CO (":",             p4_spy_colon),
    CO (";",             p4_spy_semicolon),
    CO ("EXIT",          p4_spy_exit),
};
P4_COUNTWORDS (with_spy, "WITH-SPY kernel extension");

#endif
    /*PFE_WITH_SPY*/

/*@}*/

