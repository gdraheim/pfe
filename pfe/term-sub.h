#ifndef __PFE_TERM_EXT_H
#define __PFE_TERM_EXT_H "%full_filespec: term-sub.h~5.5:incl:bln_12xx!1 %"

/** 
 * -- Declarations for the terminal drivers of pfe
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2000. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE            @(#) %derived_by: guidod %
 *  @version %version: 5.5 %
 *    (%date_modified: Mon Mar 12 10:33:45 2001 %)
 *
 *  @description
 *		It doesn't matter how you supply these functions,
 *		  --- just do it!
 *  
 *		For an example driver refer to termunix.c.
 */
/*@{*/


enum _p4_key_codes			/* The function keys known to p4th */
{
    P4_KEY_k1 = 0x100,		/* function keys F1 through F10 */
    P4_KEY_k2,
    P4_KEY_k3,
    P4_KEY_k4,
    P4_KEY_k5,
    P4_KEY_k6,
    P4_KEY_k7,
    P4_KEY_k8,
    P4_KEY_k9,
    P4_KEY_k0,

    P4_KEY_F1,			/* function keys S-F1 through S-F10 */
    P4_KEY_F2,
    P4_KEY_F3,
    P4_KEY_F4,
    P4_KEY_F5,
    P4_KEY_F6,
    P4_KEY_F7,
    P4_KEY_F8,
    P4_KEY_F9,
    P4_KEY_FA,
    
    /* "kl", "kr", "ku", "kd" */
    P4_KEY_kl,			/* arrow key left */
    P4_KEY_kr,			/* arrow key right */
    P4_KEY_ku,			/* arrow key up */
    P4_KEY_kd,			/* arrow key down */
    /* "kh", "kH", "kN", "kP" */
    P4_KEY_kh,			/* home key */
    P4_KEY_kH,			/* home down key */
    P4_KEY_kN,			/* next page key */
    P4_KEY_kP,			/* previous page key */
    /* "kb", "kD", "kM", "kI" */
    P4_KEY_kb,			/* backspace key */
    P4_KEY_kD,			/* delete character key */
    P4_KEY_kM,			/* exit insert mode key */
    P4_KEY_kI,			/* insert character key */
    /* "kA", "kE", "kL", "kC" */
    P4_KEY_kA,			/* insert line key */
    P4_KEY_kE,			/* clear to end of line key */
    P4_KEY_kL,			/* delete line key */
    P4_KEY_kC,			/* clear screen key */
    /* count the keys: */
    P4_KEY_LAST,
    P4_NUM_KEYS = P4_KEY_LAST - P4_KEY_k1
};

extern char const * p4_dumbterm_rawkeys[];
extern char const * p4_dumbterm_controls[];

struct _p4_term_struct
{
    char const * name;
    char const ** control_string;
    char const ** rawkey_string;
    int  (*init) (void);
    void (*fini) (void);
    
    void (*tput)(int);
    
    int (*tty_interrupt_key) (char ch);
    void (*interactive_terminal) (void);
    void (*system_terminal) (void);
    void (*query_winsize) (void);

    int (*c_keypressed) (void);	
    int (*c_getkey) (void);
    
    void (*c_putc_noflush) (char c);
    void (*c_put_flush) (void);
    void (*c_putc) (char c);
    void (*c_puts) (const char *s);
    void (*c_gotoxy) (int x, int y);
    void (*c_wherexy) (int *x, int *y);

    /* latest addition in 0.30.81, most driver have default NULL, *FIXME:* */
    int (*c_getvkey) ();
};

extern struct _p4_term_struct p4_term_ios;

int p4_tty_interrupt_key (char ch);
int p4_prepare_terminal (void);
void p4_interactive_terminal (void);
void p4_system_terminal (void);
void p4_query_winsize (void);

int p4_keypressed (void);	
int p4_getkey (void);

void p4_putc_noflush (char c);
void p4_put_flush (void);
void p4_putc (char c);
void p4_puts (const char *s);
void p4_gotoxy (int x, int y);
void p4_wherexy (int *x, int *y);

void p4_goleft (void);
void p4_goright (void);
void p4_goup (void);
void p4_godown (void);

void p4_dot_home (void);
void p4_dot_clrscr (void);
void p4_dot_clreol (void);
void p4_dot_clrdown (void);
void p4_dot_bell (void);

void p4_dot_standout_on (void);
void p4_dot_standout_off (void);
void p4_dot_bright (void);
void p4_dot_reverse (void);
void p4_dot_blinking (void);
void p4_dot_normal (void);
void p4_dot_underline_on (void);
void p4_dot_underline_off (void);

/* These are not part of the driver, but system independent, in term.c */

int  p4_isprint (int c);	/* like isprint() for ISO-characters */
void p4_putc_printable (int c);	/* like p4_putc() but certainly visible */
int  p4_change_case (int key);	/* exchange lower case with upper case char */
int  p4_getekey (void);		/* get a character like EKEY */
int  p4_ekeypressed (void);	/* check for extended key available */
int  p4_getwskey (void);	/* get a character, for block editor */

extern void (*on_stop) (void);
extern void (*on_continue) (void);
extern void (*on_winchg) (void);

extern void (*p4_ptr_wait_for_stdin)();

/* these are modelled after the tc-codes in termunix ... */
enum p4tcap
{
    P4_TERM_XY = 0, /* not used */
    P4_TERM_HOME,
    
    P4_TERM_GOLEFT,
    P4_TERM_GORIGHT,
    P4_TERM_GOUP,
    P4_TERM_GODOWN,
    
    P4_TERM_CLRSCR,
    P4_TERM_CLRDOWN,
    P4_TERM_CLREOL,
    
    P4_TERM_BELL,
    P4_TERM_DELCH, /* not used */
    P4_TERM_DELLN, /* not used */
  
    P4_TERM_SCROLLDOWN, /* not used */
    P4_TERM_SCROLLUP,   /* not used */
  
    P4_TERM_BOLD_ON,
    P4_TERM_BOLD_OFF,
    P4_TERM_UNDERLINE_ON,
    P4_TERM_UNDERLINE_OFF,
    P4_TERM_BRIGHT,
    P4_TERM_REVERSE,
    P4_TERM_BLINKING,
    P4_TERM_NORMAL,
    
    P4_TERM_KEYPAD_ON,  /* not used */
    P4_TERM_KEYPAD_OFF, /* not used */

    P4_TERM_MAXENUM
};

/*@}*/
#endif
