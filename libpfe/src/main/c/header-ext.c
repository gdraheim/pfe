/**
 *  Implements header creation and navigation and defer/synonym words.
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.16 $
 *     (modified $Date: 2008-10-07 02:35:39 $)
 *
 *  @description
 *    Implements header creation and navigation words including the
 *    ones from the forth-83 "experimental annex" targetting definition
 *    field access. Also it has the defer and synonym words that are
 *    almost standard - It is said that the missing DEFER in the ANS Forth
 *    standard of 1994 was just a mistake.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) =
"@(#) $Id: header-ext.c,v 1.16 2008-10-07 02:35:39 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-limits.h>
#include <pfe/exception-sub.h>
#include <pfe/logging.h>

/** >NAME ( cfa -- nfa )
 * converts a pointer to the code-field (CFA) to point
 * then to the corresponding name-field (NFA)
 implementation-specific simulation:
   : >NAME  >LINK L>NAME ;
 */
void FXCode (p4_to_name)
{
    *SP = (p4cell) p4_to_name ((p4xt) *SP);
}

/** >LINK ( cfa -- lfa )
 * converts a pointer to the code-field (CFA) to point
 * then to the corresponding link-field (LFA) - in some configurations
 * this can be a very slow operation since the system might need to walk
 * through all header-words in the system, looking for a =>">NAME" that
 * has the cfa and *then* returning the "N>LINK" result here - which might
 * be none at all if the word is a =>":NONAME". Use always =>">NAME" and
 * treat this word as non-portable just like any assumption about the
 * contents of the =>">LINK"-field.
 * Only in fig-mode and for traditional fig-mode programs, this word may
 * possibly have enough extra assertions to be somewhat reliable.
 * (and fig-mode did not know about =>"SYNONYM"s - see note at =>"LINK>").
 */
void FXCode (p4_to_link)
{
    *SP = (p4cell) P4_TO_LINK (*SP);
}

/** BODY> ( pfa -- cfa )
 * trying to convert a pointer to the parameter-field (PFA) to point
 * then to the corresponding code-field (CFA) - note that this is not
 * necessarily the inverse of =>">BODY" instead it is a fast implementation
 * assuming a =>"VARIABLE" thing had been used. Every use of "BODY>" is
 * warned in the logfile.
 implementation-specific simulation:
   : BODY> CELL - ;
 *
 */
void FXCode (p4_body_from)
{
    *SP = (p4cell) p4_body_from ((p4cell*) *SP);
}

/** NAME> ( nfa -- cfa )
 * converts a pointer to the name-field (NFA) to point
 * then to the corresponding code-field (CFA)
 *
 * In all cases but a => SYNONYM the pfe will behave not unlike the
 * original fig-forth did - being identical to => N>LINK => LINK> .
 */
void FXCode (p4_name_from)
{
    p4xt xt = p4_name_from ((p4char *) *SP);
    /* p4_check_obsoleted (xt);  // at p4_name_from ? */
    *SP = (p4cell) xt;
}

/** LINK> ( lfa -- cfa )
 * converts a pointer to the link-field (LFA) to point
 * then to the corresponding code-field (CFA)
 *
 * BEWARE: this one does not care about =>"SYNONYM"s and it is the
 * only way to get at the data of a =>"SYNONYM". Therefore, if you have
 * a synonym called A for an old word B then there is a different
 * result using "NAME>" on an A-nfa or using "N>LINK LINK>" since the
 * first "NAME>" will return the xt of B while the latter will return
 * the xt of A - but executing an xt of A is an error and it will => THROW
 *
 * this difference is intentional to allow knowledgable persons to
 * do weird things looking around in the dictionary. The forth standard
 * words will not give you much of a chance to get hold of the nfa of
 * a => SYNONYM word anyway - asking => FIND for a word A will return
 * the execution token of B immediatly and "NAME>" on that one lead to
 * the nfa of B and not that of A.
 */
void FXCode (p4_link_from)
{
    *SP = (p4cell) P4_LINK_FROM (*SP);
}

/** L>NAME ( lfa -- nfa )
 * converts a pointer to the link-field (LFA) to point
 * then to the corresponding name-field (CFA) - this one is one of
 * the slowest operation available. One should always use the inverse
 * operation =>"N>LINK" and cache an older value if that is needed.
 * Some words might be linked but they do not have a name-field (just
 * the other fields) but this word can not detect that and will try to look
 * into the bits of the dictionary anway in the assumption that there is
 * something - and if done in the wrong place it might even segfault.
 * Only in fig-mode and for traditional fig-mode programs, this word may
 * possibly have enough extra assertions to be somewhat reliable.
 * (and fig-mode did not know about =>"SYNONYM"s - see note at =>"LINK>").

 implementation-specific configure-dependent fig-only simulation:
 : L>NAME BEGIN DUP C@ 128 AND 0= WHILE 1- REPEAT ;
 */
void FXCode (p4_l_to_name)
{
    *SP = (p4cell) p4_link_to_name ((p4char **) *SP);
}

/** N>LINK ( nfa -- lfa )
 * converts a pointer to the name-field (NFA) to point
 * then to the corresponding link-field (LFA) - this operation
 * is quicker than the inverse =>"L>NAME". This word is a specific
 * implementation detail and should not be used by normal users - instead
 * use always =>"NAME>" which is much more portable. Many systems may
 * possibly not even have a =>">LINK"-field in the sense that a => @ on
 * this adress will lead to another =>">NAME". Any operation on the
 * resulting =>">LINK"-adress is even dependent on the current configuration
 * of PFE - only in fig-mode you are asserted to have the classic detail.
 * (and fig-mode did not know about =>"SYNONYM"s - see note at =>"LINK>").

 implementation-specific configure-dependent fig-only simulation:
   : N>LINK  C@ + ;
 */
void FXCode (p4_n_to_link)
{
    *SP = (p4cell) p4_name_to_link ((p4char *) *SP);
}


/** NAME>STRING        ( name-token -- str-ptr str-len )
 * convert a name-token into a string-span, used to detect the
 * name for a word and print it. The word =>"ID." can be defined as
 : ID. NAME>STRING TYPE ;
 * the implementation of => NAME>STRING depends on the header layout
 * that is defined during the configuration of the forth system.
 : NAME>STRING COUNT 31 AND ; ( for fig-like names )
 : NAME>STRING COUNT ;        ( default, name is a simple counted string )
 : NAME>STRING @ ZCOUNT ;     ( name-token is a pointer to a C-level string )
 : NAME>STRING COUNT 31 AND   ( hybrid of fig-like and zero-terminated )
      DUP 31 = IF DROP 1+ ZCOUNT THEN
 ;
 : NAME>STRING HEAD:: COUNT CODE:: PAD PLACE PAD ; ( different i86 segments )
*/
void FXCode (p4_name_to_string)
{
    FX_1ROOM;
    SP[0] = NAMELEN(SP[1]);
    SP[1] = (p4cell) NAMEPTR(SP[1]);
}

/** HEADER, ( str-ptr str-len -- )
 * => CREATE a new header in the dictionary from the given string, without CFA
 usage: : VARIABLE  BL WORD COUNT HEADER, DOVAR , ;
 */
void FXCode (p4_header_comma)
{
    p4_header_comma ((p4char*)SP[1], (int)SP[0], CURRENT);
    FX_2DROP;
}

/** $HEADER ( bstring -- )
 * => CREATE a new header in the dictionary from the given string
 * with the variable runtime (see =>"HEADER," and =>"CREATE:")
 usage: : VARIABLE  BL WORD $HEADER ;
 *
 * OLD: this was also called HEADER up to PFE 0.33.x
 */
void FXCode (p4_str_header)
{
    p4_header_comma (1+ *(p4char**)SP,  **(p4char**)SP, CURRENT);
    FX_RUNTIME1 (p4_variable);
    FX_DROP;
}

/** LATEST ( -- nfa )
 * return the NFA of the lateset definition in the
 * => CURRENT vocabulary
 */
void FXCode (p4_latest)
{
    *--SP = (p4cell) p4_latest ();
}

/** SMUGDE ( -- )
 * the FIG definition toggles the => SMUDGE bit, and not all systems have
 * a smudge bit - instead one should use => REVEAL or => HIDE
 : SMUDGE LAST @ >FFA SMUDGE-MASK TOGGLE ;
 : SMUDGE LAST @ NAME-FLAGS@ SMUDGE-MASK XOR LAST @ NAME-FLAGS! ;
 : HIDE   LAST @ NAME-FLAGS@ SMUDGE-MASK  OR LAST @ NAME-FLAGS! ;
 */
void FXCode (p4_smudge)
{
    if (p4_LogMask & P4_LOG_DEBUG)
        P4_warn ("do not use SMUDGE - use REVEAL or HIDE");

    if (LAST)
        P4_NAMEFLAGS(LAST) ^= P4xSMUDGED; /* <-- XOR */
    else
        p4_throw (P4_ON_ARG_TYPE);
}

/** HIDE ( -- )
 * the FIG definition toggles the => SMUDGE bit, and not all systems have
 * a smudge bit - instead one should use => REVEAL or => HIDE
 : HIDE LAST @ FLAGS@ SMUDGE-MASK XOR LAST @ FLAGS! ;
 */
void FXCode (p4_hide)
{
    if (LAST)
        P4_NAMEFLAGS(LAST) |= P4xSMUDGED;   /* <-- OR */
    else
        p4_throw (P4_ON_ARG_TYPE);
}

/** RECURSIVE ( -- )
 * => REVEAL the current definition making it => RECURSIVE by its
 * own name instead of using the ans-forth word to =>"RECURSE".
 ' REVEAL ALIAS RECURSIVE IMMEDIATE
 */

/** REVEAL ( -- )
 * the FIG definition toggles the => SMUDGE bit, and not all systems have
 * a smudge bit - instead one should use => REVEAL or => HIDE
 : REVEAL LAST @ FLAGS@ SMUDGE-MASK INVERT AND LAST @ FLAGS! ;
 : REVEAL LAST @ CHAIN-INTO-CURRENT ;
 *
 * OLD: this was also called UNSMUDGE up to PFE 0.33.x
 */
void FXCode (p4_reveal)
{
    if (LAST)
        P4_NAMEFLAGS(LAST) &= ~P4xSMUDGED;
    else
        p4_throw (P4_ON_ARG_TYPE);
}

/** IMMEDIATE-MASK ( -- bit-mask )
 *  returns the bit-mask to check if a found word is immediate
 *  (use in conjunction with => NAME-FLAGS@ and NAME-FLAGS! )
    " my-word" FIND-NAME IF NAME-FLAGS@ IMMEDIATE-MASK AND
                       IF ." immediate" THEN ELSE DROP THEN
 *
 * OLD: this was called "(IMMEDIATE#)" up to PFE 0.33.x
 */

/** SMUDGE-MASK ( -- bit-mask )
 *  returns the bit-mask to check if a found word is smudge
 *  (use in conjunction with => NAME-FLAGS@ and NAME-FLAGS! )
    " my-word" FIND-NAME IF NAME-FLAGS@ SMUDGE-MASK AND
                       IF ." smudge" THEN ELSE DROP THEN
 *
 * OLD: this was called "(SMUDGE#)" up to PFE 0.33.x
 */

/** NAME-FLAGS@ ( nfa -- nfa-flags )
 * get the nfa-flags that corresponds to the nfa given. Note that
 * in the fig-style would include the nfa-count in the lower bits.
 * (see =>"NAME-FLAGS!")
 */
void FXCode (p4_name_flags_fetch)
{
    *SP = P4_NAMEFLAGS(*SP);
}

/** NAME-FLAGS! ( nfa-flags nfa -- )
 * set the nfa-flags of nfa given. Note that in the fig-style the nfa-flags
 * would include the nfa-count in the lower bits - therefore this should only
 * set bits that had been previously retrieved with => NAME-FLAGS@
 : IMMEDIATE LAST @ NAME-FLAGS@ IMMEDIATE-MASK OR LAST @ NAME-FLAGS! ;
 */
void FXCode (p4_name_flags_store)
{
    P4_NAMEFLAGS(SP[0]) = (p4ucell) SP[1];
}

/* ------------------------------------------------------------------------ */

/** ((DEFER)) ( -- )
 * runtime of => DEFER words
 */
void FXCode (p4_defer_RT)
{   FX_USE_BODY_ADDR {
    register p4xt xt;
    xt = * (p4xt*) P4_TO_DOES_BODY(P4_BODY_FROM(FX_POP_BODY_ADDR)); /* check IS-field */
    if (xt) { PFE.execute (xt); return; }
    else { P4_warn1 ("null execution in DEFER %p", WP_CFA); }
}}

/** DEFER ( 'word' -- )
 * create a new word with ((DEFER))-semantics
 simulate:
   : DEFER  CREATE 0, DOES> ( the ((DEFER)) runtime )
      @ ?DUP IF EXECUTE THEN ;
   : DEFER  DEFER-RT HEADER 0 , ;
 *
 * declare as <c>"DEFER deferword"</c>  <br>
 * and set as <c>"['] executionword IS deferword"</c>
 * (in pfe, you can also use <c>TO deferword</c> to set the execution)
 */
void FXCode (p4_defer)
{
    FX_RUNTIME_HEADER;
    FX_RUNTIME1 (p4_defer);
    FX_XCOMMA (0); /* <-- leave it blank (may become chain-link later) */
    FX_XCOMMA (0); /* <-- put XT here in fig-mode */
}
P4RUNTIME1(p4_defer, p4_defer_RT);

void FXCode_XE (p4_is_execution)
{
    FX_USE_CODE_ADDR;
    * (p4xt*) P4_TO_DOES_BODY(*IP++) = (p4xt) FX_POP;
    FX_USE_CODE_EXIT;
}

/** IS ( xt-value [word] -- )
 * set a => DEFER word
 * (in pfe: set the DOES-field - which is the BODY-field in ans-mode
 *  and therefore the same as => TO /  in fig-mode the DOES-field is
 *  one cell higher up than for a => CREATE: => VARIABLE
 *  Use => IS freely on each DOES-words in both modes).
 : IS '
   STATE @ IF LITERAL, POSTPONE >DOES-BODY POSTPONE !
   ELSE >DOES-BODY ! THEN
 ; IMMEDIATE
 */
void FXCode (p4_is)
{
    p4xt xt = p4_tick_cfa ();
    if (STATE)
    {
        FX_COMPILE (p4_is);
        FX_XCOMMA (xt);
    }else{
        * (p4xt*) P4_TO_DOES_BODY (xt) = (p4xt) FX_POP;
    }
}
P4COMPILES(p4_is, p4_is_execution,
           P4_SKIPS_TO_TOKEN, P4_DEFAULT_STYLE);

void FXCode_XE (p4_action_of_execution)
{
    FX_USE_CODE_ADDR;
    p4xt xt = (p4xt) (*IP++);
    FX_PUSH(* P4_TO_DOES_BODY(xt));
    FX_USE_CODE_EXIT;
}

/** ACTION-OF ( [word] -- xt-value )
 * get the => BEHAVIOR of a => DEFER word when executed. If being
 * compiled then the ACTION-OF will be the value of [word] at the
 * time of execution and not that of compilation time (non-constant).
 *
 * In PFE it does actually pick whatever is stored in the DOES-field
 * of a word and therefore ACTION-OF may applied to all DOES-words.
 */
void FXCode (p4_action_of)
{
    p4xt xt = p4_tick_cfa ();
    if (STATE)
    {
        FX_COMPILE (p4_action_of);
        FX_XCOMMA (xt);
    }else{
        FX_PUSH(* P4_TO_DOES_BODY(xt));
    }
}
P4COMPILES(p4_action_of, p4_action_of_execution,
           P4_SKIPS_TO_TOKEN, P4_DEFAULT_STYLE);

/** DEFER! ( xt-value xt-defer -- )
 * A Forth200x definition that is not very useful.
 */
void FXCode (p4_defer_store)
{
    p4xt xt = (p4xt) FX_POP;
    * (p4xt*) P4_TO_DOES_BODY (xt) = (p4xt) FX_POP;
}

/** BEHAVIOR ( xt1 -- x2 )
 * A defintion from OpenBoot that is identical Forth200x => DEFER@
 */

/** DEFER@ ( xt1 -- xt2 )
 * get the execution token xt2 that would be executed by the => DEFER
 * identified by xt1.
 *
 * This command is used to obtain the execution contents of a deferred
 * word. A typical use would be to retrieve and save the execution
 * behavior of the deferred word, set the deferred word to a new behavior,
 * and then later restore the old behavior.
 *
 * If the deferred word identified by _xt1_ is associated with some
 * other deferred word, _xt2_ is the execution token of that other
 * deferred word. To retrieve the execution token of the word currently
 * associated with that other deferred word, use the phrase DEFER@ DEFER@ .
 *
 * Experience:
 *      BEHAVIOR was used many years in OpenBoot and OpenFirmware systems.
 *
 * In PFE it is the inverse of an => IS operation and it will never fail
 * if applied to a word with atleast a body. That's just like => IS can
 * be applied to almost every =>"DOES>" word where => DEFER@ will get
 * the value back.
 */
void FXCode (p4_defer_fetch)
{
    *SP = (p4cell) *(P4_TO_DOES_BODY( (p4xt)(*SP) ));
}

/** ALIAS ( some-xt* "name" -- ) [EXT]
 * create a defer word that is initialized with the given x-token.
 *                                                           => DO-ALIAS
 */
void FXCode (p4_alias)
{
    FX_HEADER;
    FX_RUNTIME_BODY;
    FX_RUNTIME1 (p4_defer); /* fixme? p4_alias_RT */
    FX_XCOMMA (0); /* DOES-CODE field (later may be used for chain link)*/
    FX_XCOMMA (FX_POP); /* set DOES-BODY here */
}

/** ((SYNONYM))
 * should not actually be called ever.
 */
void FXCode_RT (p4_synonym_RT)
{
    p4_throw (P4_ON_SYNONYM_CALLED);
}

/** ((OBSOLETED))
 * should not actually be called ever.
 */
void FXCode_RT (p4_obsoleted_RT)
{
    p4_throw (P4_ON_SYNONYM_CALLED);
}

/** SYNONYM ( "newname" "oldname" -- )
 * make an name-alias for a word - this is very different from a => DEFER
 * since a => DEFER will resolve at runtime. Changing the target of a
 * => DEFER via => IS will result in changing the => BEHAVIOR of all
 * words defined earlier and containing the name of the => DEFER.
 *
 * A => SYNONYM however does not have any data field (theoretically not
 * even an execution token), instead it gets resolved at compile time.
 * In theory, you can try to => FIND the name of the => SYNONYM but as
 * soon as you apply =>"NAME>" the execution token of the end-point is
 * returned. This has also the effect that using the inverse =>">NAME"
 * operation will result in the name-token of the other name.

   SYNONYM CREATE <BUILDS ( like it is in ANS Forth )
   : FOO CREATE DOES> @ ;
   SEE FOO
   : foo <builds
     does> @ ;
   SYNONYM CREATE CREATE:
   : BAR CREATE 10 ALLOT ;
   SEE BAR
   : bar create: 10 allot ;
 *                      (only =>"LINK>" does not care about =>"SYNONYM"s)
 */
void FXCode (p4_synonym)
{
    FX_RUNTIME_HEADER;
    FX_RUNTIME1 (p4_synonym);
    FX_XCOMMA(p4_body_from((p4cell*) DP));
    register p4char* nfa = p4_tick_nfa ();
    if (P4_NAMExIMMEDIATE(nfa))
        FX_IMMEDIATE;
    ((p4xt*)DP)[-1] = p4_name_from (nfa);
}
P4RUNTIME1(p4_synonym, p4_synonym_RT);

/** "SYNONYM-OBSOLETED ( "newname" "oldname" -- )
 * same as => SYNONYM but on the first use an error message will be
 * displayed on both the screen and the sys-log.
 */
void FXCode (p4_obsoleted)
{
    FX (p4_synonym);
    P4_XT_VALUE(p4_name_from(LAST)) = FX_GET_RT (p4_obsoleted);
}
P4RUNTIME1(p4_obsoleted, p4_obsoleted_RT);

static void show_deprecated(char** body)
{
    if (p4_OUT) FX (p4_cr);
    p4_namebuf_t* name = p4_to_name(P4_BODY_FROM(body));
    p4_outf ("(DEPRECATED: %.*s %s)", NAMELEN(name), NAMEPTR(name), *body);
    FX (p4_cr_show_input);
}

void FXCode_RT (p4_deprecated_RT)
{ FX_USE_BODY_ADDR {
    show_deprecated((char**)( FX_POP_BODY_ADDR));
}}

/** (DEPRECATED: ( "newname" [message<closeparen>] -- )
 * add a message for the following word "newname" that should
 * be shown once upon using the following word. Use it like
   (DEPRECATED: myword is obsoleted in Forth200X)
   : myword ." hello world" ;
 */
void FXCode (p4_deprecated)
{
    FX_RUNTIME_HEADER;
    FX_RUNTIME1(p4_deprecated); FX_IMMEDIATE; FX_SMUDGED;
    FX_COMMA (DP + sizeof(p4cell)); /* pointer to zstring */
    /* compare with FX(p4_paren) */
    switch (SOURCE_ID)
    {
     case -1:
     case 0:
         p4_word_parse (')'); /* PARSE-NOHERE-NOTHROW */
         p4_memcpy (DP, PFE.word.ptr, PFE.word.len);
         DP += PFE.word.len;
         break;
     default:
         while (! p4_word_parse (')')) /* PARSE-NOHERE-NOTH */
         {
             p4_memcpy (DP, PFE.word.ptr, PFE.word.len);
             DP += PFE.word.len; *DP++ = '\n';
             if (! p4_refill ()) return;
         }
         p4_memcpy (DP, PFE.word.ptr, PFE.word.len);
         DP += PFE.word.len;
         break;
    }
}
P4RUNTIME1(p4_deprecated, p4_deprecated_RT);

/** (CHECK-DEPRECATED) ( nfa* -- nfa* )
 * an internal function that will check a word name
 * to have any deprecation attribution - some words have
 * a (one time) message to be shown to the user, while
 * => OBSOLETED-SYNONYM will show a message and rebuild
 * itself as a normal SYNONYM. - Note that most deprecations
 * are only shown once and that they are not emitted when
 * having REDEFINED-MSG OFF.
 */
void FXCode (p4_check_deprecated)
{
    p4_check_deprecated ((p4_namebuf_t*) *SP);
}

/** EXTERN,-DEPRECATED: ( "newname" zstring* -- )
 * compile a pointer to an extern (loader) z-string
 * to the dictionary and on execution show a deprecation
 * message once. Note: the new name is smudged+immediate,
 * so it you can not => FIND it right after compilation.
 *
 * see also =>"(DEPRECATED:" name message) for the real thing
 */
void FXCode (p4_extern_deprecated)
{
    FX_RUNTIME_HEADER;
    FX_RUNTIME1_RT (p4_deprecated);
    FX_COMMA(*SP); /* a zstring pointer */
    FX_IMMEDIATE; FX_SMUDGED;
    FX_DROP;
}
P4RUNTIME1(p4_extern_deprecated, p4_deprecated_RT);

static void show_logmessage(char** body)
{
    if (p4_OUT) FX (p4_cr);
    p4_namebuf_t* name = p4_to_name(P4_BODY_FROM(body));
    p4_outf ("\\ NOTE: %.*s %s", NAMELEN(name), NAMEPTR(name), *body);
    FX (p4_cr);
}

void FXCode_RT (p4_logmessage_RT)
{ FX_USE_BODY_ADDR {
    show_logmessage((char**)( FX_POP_BODY_ADDR));
}}

/** EXTERN,-LOGMESSAGE: ( "newname" zstring* -- )
 * compile a pointer to an extern (loader) z-string
 * to the dictionary and on execution show a logging
 * message once. Note: this name is NOT smudged+immediate.
 *
 * see also =>"(DEPRECATED:" name message) for
 * deprecation messages
 */
void FXCode (p4_logmessage)
{
    FX_RUNTIME_HEADER;
    FX_RUNTIME1_RT (p4_logmessage);
    FX_COMMA(*SP); /* a zstring pointer */
    FX_DROP;
}
P4RUNTIME1(p4_logmessage, p4_logmessage_RT);

P4_LISTWORDSET (header) [] =
{
    P4_INTO ("FORTH", "[ANS]"),

    /* FORTH-83 definition field address conversion operators */
    P4_FXco ("BODY>",		p4_body_from),
    P4_FXco (">LINK",		p4_to_link),
    P4_FXco ("LINK>",		p4_link_from),
    P4_FXco (">NAME",		p4_to_name),
    P4_FXco ("NAME>",		p4_name_from),
    P4_FXco ("L>NAME",		p4_l_to_name),
    P4_FXco ("N>LINK",		p4_n_to_link),

    P4_FXco ("NAME>STRING",	p4_name_to_string),
    P4_DVaR ("LAST",		last),
    P4_FXco ("LATEST",		p4_latest),

    P4_INTO ("EXTENSIONS", "FORTH"),
    P4_FXco ("NAME-FLAGS@",		p4_name_flags_fetch),
    P4_FXco ("NAME-FLAGS!",		p4_name_flags_store),

    P4_FXco ("HEADER,",			p4_header_comma),
    P4_FXco ("$HEADER",		        p4_str_header),
    P4_FXco ("SMUDGE",			p4_smudge),
    P4_FXco ("HIDE",                    p4_hide),
    P4_FXco ("REVEAL",			p4_reveal),
    P4_IXco ("RECURSIVE",		p4_reveal),

    P4_OCoN ("IMMEDIATE-MASK",		P4xIMMEDIATE),
    P4_OCoN ("SMUDGE-MASK",		P4xSMUDGED),

    P4_RTco ("SYNONYM-OBSOLETED",	p4_obsoleted),
    P4_RTco ("(DEPRECATED:",            p4_deprecated),
    P4_RTco ("EXTERN,-DEPRECATED:",     p4_extern_deprecated),
    P4_FXco ("(CHECK-DEPRECATED:)",     p4_check_deprecated),
    P4_RTco ("EXTERN,-LOGMESSAGE:",     p4_logmessage),

    P4_INTO ("FORTH", 0 ),
    /* FACILITY-EXT forth200x/synonym */
    P4_RTco ("SYNONYM",                 p4_synonym),
    P4_FXco ("ALIAS",                     p4_alias),

    /* CORE-EXT forth200x/deferred */
    P4_RTco ("DEFER",                   p4_defer),
    P4_SXco ("IS",                      p4_is),
    P4_FXco ("DEFER!",                  p4_defer_store),
    P4_FXco ("DEFER@",                  p4_defer_fetch),
    P4_SXco ("ACTION-OF",               p4_action_of),

    P4_INTO ("ENVIRONMENT", 0 ),
    P4_OCON ("HEADER-EXT", 1983),
    P4_OCON ("forth200x/synonym",  2006), /* actually FACILITY-EXT */
    P4_SHOW ("X:synonym",  "forth200x/synonym 2006"),
    P4_OCON ("forth200x/deferred", 2005), /* actually CORE-EXT */
    P4_SHOW ("X:deferred", "forth200x/deferred 2005"),

    P4_INTO ("EXTENSIONS", 0),
    P4_EXPT ("SYNONYM was called at runtime" /*2070*/, P4_ON_SYNONYM_CALLED),
};
P4_COUNTWORDSET (header, "Header Navigation");

/*@}*/
