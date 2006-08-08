/** 
 * -- extra words for external TERMCATCH support
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version:  %
 *    (%date_modified:  %)
 *
 *  @description
 *      We hook into the terminal driver capturing all the
 *      output into a forth level buffer string. The implementation
 *      creates another terminal driver that overlays with the one
 *      being effective when initiating the capturing. 
 *
 *      Capturing the forth output is most useful when implementing
 *      regression tests which currently uses shell utilities from
 *      the host operating system environment. That purpose is also
 *      the reason that this module is generally not active in any
 *      default system configuration - you have to load/activate the
 *      module explicitly (i.e. NEEDS termcatch-ext)
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: termcatch-ext.c,v 1.1.1.1 2006-08-08 09:07:25 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-limits.h>
#include <pfe/term-sub.h>
#include <pfe/logging.h>

#define ___ {
#define ____ }


static int slot = 0;
#define CHK (*(struct termcatch*)(PFE.p[slot]))

struct termcatch {
    p4_term_struct* term;
    struct { char* base; char* fill; char* ends; } capture;
    p4cell controls;
};

/* forward */
extern p4_term_struct p4_term_capture;


/** TERM-CAPTURE-CONTROLS ( -- var-ptr )
 *  enable/disable common visualization of terminal control sequences.
 */
static void FXCode (p4_term_capture_controls)
{
    FX_PUSH (&CHK.controls);
}

/** (TERM-CAPTURE-ON) ( -- )
 *  start capturing any terminal output. Whether the captured output
 *  does include any control commands depends on the setting of
 *  => (TERM-CAPTURE-CONTROLS)
 */
static void FXCode (p4_paren_term_capture_on) 
{
    CHK.term = PFE.term;
    PFE.term = & p4_term_capture;
}

/** (TERM-CAPTURE-OFF) ( -- )
 *  reset terminal driver to the one being effective on the
 *  last => TERM-CAPTURE-TEXT or => TERM-CAPTURE-KEYS call.
 */
static void FXCode (p4_paren_term_capture_off) 
{
    PFE.term = CHK.term;
}

/** (TERM-CAPTURE-BUFFER) ( capturebuffer-ptr capturebuffer-len -- )
 *  start with new capture buffer.
 */
static void FXCode (p4_paren_term_capture_buffer)
{
    CHK.capture.base = (char*) SP[1];
    CHK.capture.fill = CHK.capture.base;
    CHK.capture.ends = CHK.capture.base + SP[0];
    P4_info3 ("BUFFER(%p:%p:%p)\n", 
             CHK.capture.base, CHK.capture.fill, CHK.capture.ends);
            
    FX_2DROP;
}

/** (TERM-CAPTURE-RESULT) ( -- capturebuffer-ptr capturebuffer-len )
 *  get fill state of current capture buffer.
 */
static void FXCode (p4_paren_term_capture_result)
{
    FX_PUSH (CHK.capture.base);
    FX_PUSH (CHK.capture.fill - CHK.capture.base);
}

/** TERM-CAPTURE-ON ( capturebuffer-ptr capturebuffer-len -- )
 *  init/start capturing terminal output
 :  TERM-CAPTURE-ON (TERM-CAPTURE-BUFFER) (TERM-CAPTURE-ON) ;
 */
static void FXCode (p4_term_capture_on) {
    FX (p4_paren_term_capture_buffer);
    FX (p4_paren_term_capture_on);
}

/** TERM-CAPTURE-OFF ( -- capturebuffer-ptr capturebuffer-len )
 *  shutdown capturing terminal output
 :  TERM-CAPTURE-OFF (TERM-CAPTURE-OFF) (TERM-CAPTURE-RESULT) ;
 */
static void FXCode (p4_term_capture_off) {
    FX (p4_paren_term_capture_off);
    FX (p4_paren_term_capture_result);
}
    

/** TERMCATCH ( str-ptr str-len some-xt* -- str-ptr str-len' catch-code# )
 *  create a catch-domain around the token to be executed. This works
 *  the same as => CATCH. Additionally all terminal output of that word
 *  is being captured to the buffer being provided as an argument. The
 *  maximum length input argument is modified flagging the actual length
 *  of captured output stream as the output argument. Note that in most
 *  cases a => POCKET-PAD is just not big enough, atleast many error
 *  condition notifications tend to be quite lengthy for byte counted
 *  forth strings.
 : TERMCATCH TERM-CAPTURE-ON CATCH >R TERM-CAPTURE-OFF R> ;
 */
FCode (p4_termcatch)
{
    if (FX_DEPTH < 3) /* common error in interactive mode: no buffer arg */
        p4_throw (P4_ON_STACK_UNDER);
    ___ p4xt xt = (p4xt) FX_POP;
    FX (p4_term_capture_on);    /* compare with CATCH implementation */
    ___ p4cell catch_code = p4_catch (xt);     /* in exception-ext.c */
    FX (p4_term_capture_off);
    FX_PUSH (catch_code); ____;____;
}

/* ------------------------------------------------------------------- */
/* term capture definitions */

#define P4_TERM_QUERY_WINSIZE    P4_TERM_MAXENUM+2
#define P4_TERM_INTERACTIVE_ON   P4_TERM_MAXENUM+3
#define P4_TERM_INTERACTIVE_OFF  P4_TERM_MAXENUM+4
#define P4_TERM_GOTO_XY          P4_TERM_MAXENUM+5
#define P4_TERM_WHERE_XY         P4_TERM_MAXENUM+6

static char controlstrings[][2] = {
    /** P4_TERM_XY: */              { 'X','Y' },
    /** P4_TERM_HOME: */            { 'F','F' },
    /** P4_TERM_GOLEFT: */          { 'L','E' },
    /** P4_TERM_GORIGHT: */         { 'R','E' },
    /** P4_TERM_GOUP: */            { 'U','P' },
    /** P4_TERM_GODOWN: */          { 'D','N' },
    /** P4_TERM_CLRSCR: */          { 'X','X' },
    /** P4_TERM_CLRDOWN: */         { 'X','D' },
    /** P4_TERM_CLREOL: */          { 'X','R' },
    /** P4_TERM_BELL: */            { 'B','L' },
    /** P4_TERM_DELCH: */           { 'D','C' },
    /** P4_TERM_DELLN: */           { 'D','L' },
    /** P4_TERM_SCROLLDOWN: */      { 'P','D' },
    /** P4_TERM_SCROLLUP: */        { 'P','U' },
    /** P4_TERM_BOLD_ON: */         { '=','B' },
    /** P4_TERM_BOLD_OFF: */        { '/','B' },
    /** P4_TERM_UNDERLINE_ON: */    { '=','U' },
    /** P4_TERM_UNDERLINE_OFF: */   { '/','U' },
    /** P4_TERM_BRIGHT: */          { '=','H' },
    /** P4_TERM_REVERSE: */         { '=','V' },
    /** P4_TERM_BLINKING: */        { '=','K' },
    /** P4_TERM_NORMAL: */          { '/','/' },
    /** P4_TERM_KEYPAD_ON: */       { 'K','P' },
    /** P4_TERM_KEYPAD_OFF: */      { 'K','Q' },
    /**                     */      { '?','?' },
    /**                     */      { '?','?' },
    /** P4_TERM_QUERY_WINSIZE: */   { 'S','Z' },
    /** P4_TERM_INTERACTIVE_ON: */  { '=','=' },
    /** P4_TERM_INTERACTIVE_OFF: */ { '=','/' },
    /** P4_TERM_GOTO_XY: */         { '*','!' },
    /** P4_TERM_WHERE_XY: */        { '*','?' },
    /**                     */      { '?','?' },
    /**                     */      { '?','?' }
};

static void control(int c) {
    return;
    if (CHK.controls && CHK.capture.ends > CHK.capture.fill+4) {
        *CHK.capture.fill++ = '{';
        *CHK.capture.fill++ = controlstrings[c][0];
        *CHK.capture.fill++ = controlstrings[c][1];
        *CHK.capture.fill++ = '}';
    }
}    

static  void c_tput(int c) {
    control(c);
    if (CHK.term->tput)
        CHK.term->tput (c); }

static  int c_tty_interrupt_key (char ch) { 
    return CHK.term->tty_interrupt_key (ch); 
}
static  void c_interactive_terminal (void) {
    control(P4_TERM_INTERACTIVE_ON);
    if (CHK.term->interactive_terminal)
        CHK.term->interactive_terminal (); }
static  void c_system_terminal (void) {
    control(P4_TERM_INTERACTIVE_OFF);
    if (CHK.term->system_terminal)
        CHK.term->system_terminal (); }
static  void c_query_winsize (void) {
    control(P4_TERM_QUERY_WINSIZE);
    if (CHK.term->query_winsize)
        CHK.term->query_winsize (); }

static  int c_keypressed (void) {
    return CHK.term->c_keypressed (); }
static  int c_getkey (void) {
    return CHK.term->c_getkey (); }
  
static  void c_putc_noflush (char c) {

    if (CHK.capture.ends > CHK.capture.fill+4) {
        *CHK.capture.fill++ = c;
    }

    if (CHK.term->c_putc_noflush)
        CHK.term->c_putc_noflush (c); }
static  void c_put_flush (void) {
    if (CHK.term->c_put_flush)
        CHK.term->c_put_flush (); }
static  void c_putc (char c) {

    if (CHK.capture.ends > CHK.capture.fill+4) {
        *CHK.capture.fill++ = c;
    }

    if (CHK.term->c_putc)
        CHK.term->c_putc (c); }
static  void c_puts (const char *s) {
    const char* x = s; while (*x && CHK.capture.ends > CHK.capture.fill+4) {
        *CHK.capture.fill++ = *x++;
    }

    if (CHK.term->c_puts)
        CHK.term->c_puts (s); }

static  void c_gotoxy (int x, int y) {
    control(P4_TERM_GOTO_XY);
    if (CHK.term->c_gotoxy)
        CHK.term->c_gotoxy (x, y); }
static  void c_wherexy (int *x, int *y) {
    control(P4_TERM_WHERE_XY);
    if (CHK.term->c_wherexy)
        CHK.term->c_wherexy (x, y); }
static  int c_getvkey (void) {
    if (CHK.term->c_getvkey)
        return CHK.term->c_getvkey(); 
    else
        return CHK.term->c_getkey();
}


#ifdef __GNUC__
#define INTO(x) .x =
#else
#define INTO(x)
#endif

p4_term_struct p4_term_capture =
{
    "capture",
    0,
    0, /* no rawkeys */
    
    INTO(init)            0,
    INTO(fini)            0,
    INTO(tput)                 c_tput,

    INTO(tty_interrupt_key)    c_tty_interrupt_key,
    INTO(interactive_terminal) c_interactive_terminal,
    INTO(system_terminal)      c_system_terminal,
    INTO(query_winsize)        c_query_winsize,
    
    INTO(c_keypressed)    c_keypressed,
    INTO(c_getkey)        c_getkey,
    INTO(c_putc_noflush)  c_putc_noflush,
    INTO(c_put_flush)     c_put_flush,
    INTO(c_putc)          c_putc,
    INTO(c_puts)          c_puts,
    INTO(c_gotoxy)        c_gotoxy,
    INTO(c_wherexy)       c_wherexy,
    
    INTO(c_getvkey)       c_getvkey
};

P4_LISTWORDS (termcatch) =
{
    P4_SLOT("", &slot),
    P4_SSIZ("", sizeof(struct termcatch)),

    P4_INTO ("FORTH", "[ANS]"),
    P4_FXco ("TERM-CAPTURE-CONTROLS", p4_term_capture_controls),
    P4_FXco ("(TERM-CAPTURE-ON)", p4_paren_term_capture_on),
    P4_FXco ("(TERM-CAPTURE-OFF)", p4_paren_term_capture_off),
    P4_FXco ("(TERM-CAPTURE-BUFFER)", p4_paren_term_capture_buffer),
    P4_FXco ("(TERM-CAPTURE-RESULT)", p4_paren_term_capture_result),
    P4_FXco ("TERM-CAPTURE-ON", p4_term_capture_on),
    P4_FXco ("TERM-CAPTURE-OFF", p4_term_capture_off),
    P4_FXco ("TERMCATCH", p4_termcatch),
};
P4_COUNTWORDS (termcatch, "TERMCATCH support for testing");

/*@}*/
/* 
 * Local variables:
 * c-file-style: "stroustrup"
 * End:
 */
