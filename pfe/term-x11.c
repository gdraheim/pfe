/** 
 * -- terminal driver that opens an xterm
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see GNU LGPL
 *  @author Tektronix CTE             @(#) %derived_by: guidod %
 *  @version %version: 5.4 %
 *    (%date_modified: Mon Mar 12 10:33:02 2001 %)
 *
 *  @description
 *                   NOT FINISHED YET !!!!        *guidod*
 *
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: term-x11.c,v 0.30.86.1 2001-03-12 09:33:02 guidod Exp $";
#endif

#define _P4_SOURCE 1

/* extern char* vt100_rawkey_string[P4_NUM_KEYS]; etc... */
extern char** rawkey_string;	/* what all those keys really send */

/*
 * These variables are defined in the term.c and initialized by
 * term-xxx:prepare_terminal().  If window size can change, it would
 * be nice if they were kept up to date.
 */

/* XTerm specific variables */

#include <X11.h>
#include <stdlib.h>

Display xdsp;
Screen xscreen;
Window xwindow;
GC     xgc;
Font   xfont;
int xw, yw; /* heigth and width of a char-cell : depends on xfont */

int tty_interrupt_key (char ch)
{
   return 0;
}

int p4_prepare_terminal (void)
{
  /* a hack for having some meaningful value for xdsp
     actually we can provide a null to XopenDisplay for defaultdisplay
  */
  char* xdsp_name = getenv ("DISPLAY");
  char* xfnt_name = "7x13bold";

  xdsp = XOpenDisplay (xdsp_name);
  if (!xdsp) { 
     fprintf (stderr, "cannot open -display %s - fail", xdsp_name); 
     exit(8); 
  }

  xscreen = XDefaultScreenOfDisplay (xdsp);

  xfont = XLoadQueryFont (xdsp, xfnt_name);

  if (!xfont) {
     fprintf (stderr, "cannot open Font -fn %s - fail", xfnt_name); 
     exit(8); 
  }

  xh = xfont->ascent + xfont->descent;
  xw = xh; /* should be some xfont->max_bounds */

/* ............ */
}

void
p4_cleanup_terminal (void)
{
  return; /* nothing to do here */
}

void p4_interactive_terminal (void);
void p4_system_terminal (void);
void p4_query_winsize (void);

int p4_keypressed (void);	/* I added the "c_"-prefix to avoid */
int p4_getkey (void);		/* name clashes */

void p4_putc_noflush (char c);
void p4_put_flush (void);
void p4_putc (char c);
void p4_puts (const char *s);
void c_gotoxy (int x, int y);
void c_wherexy (int *x, int *y);

void c_goleft (void);
void c_goright (void);
void c_goup (void);
void c_godown (void);

void c_home (void);
void c_clrscr (void);
void c_clreol (void);
void c_clrdown (void);
void c_bell (void);

void p4_attrset (int attr) {}

/* These are not part of the driver, but system independent, in term.c */

int printable (int c);		/* like isprint() for ISO-characters */
void p4_putc_printable (int c);	/* like cputc() but certainly visible */
int change_case (int key);	/* exchange lower case with upper case char */
int p4_getekey (void);		/* get a character like EKEY */
int p4_ekeypressed (void);		/* check for extended key available */
int p4_getwskey (void);		/* get a character, for block editor */

extern void (*on_stop) (void);
extern void (*on_continue) (void);
extern void (*on_winchg) (void);

/*@}*/

