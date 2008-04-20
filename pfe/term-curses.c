/**
 * -- TERM-CURSES --- curses-terminal low-level routines for term-sub
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2008-04-20 04:46:30 $)
 *
 *  @description
 *              Experimental terminal driver for UNIX-like systems
 *		using the curses library.
 *
 *              new version uses a getch -> getvkey mapping.
 */
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
      "@(#) $Id: term-curses.c,v 1.3 2008-04-20 04:46:30 guidod Exp $";
#endif

#include <pfe/pfe-base.h>
#include <pfe/term-sub.h>

#include <limits.h>
#include <sys/ioctl.h>		/* ioctl(), TIOCGWINSZ */

#ifdef PFE_HAVE_NCURSES_H 
# include <ncurses.h>
#else
# include <curses.h>
#endif

#include <pfe/_missing.h>

static int c_interrupt_key (char ch)		{ return 0; }
static void c_interactive_terminal (void)	{ fixterm (); }
static void c_system_terminal (void)		{ resetterm (); }

static int
c_prepare_terminal (void)
{
    initscr ();
    cbreak ();
    nonl ();
    noecho ();
    idlok (stdscr, TRUE);
    scrollok (stdscr, TRUE);
    meta (stdscr, TRUE);
    keypad (stdscr, TRUE);
    nodelay (stdscr, FALSE);
    saveterm ();
    refresh ();
    return 1;
}

static void
c_cleanup_terminal (void)
{
    return; /* nothing to do here */
}

static void
c_query_winsize (void)
{
#  ifdef TIOCGWINSZ  
    struct winsize size;

    if (ioctl (1, TIOCGWINSZ, (char *) &size) >= 0)
    {
	PFE.rows = size.ws_row;
	PFE.cols = size.ws_col;
	PFE.xmax = size.ws_xpixel;
	PFE.ymax = size.ws_ypixel;
    }
#  endif
}

static int
c_getvkey (void)
{
    int c;
    
    c = getch ();
    if (c == -1)
	return 0;
    if (c < 0x100)
	return c;

    switch (c)
    {
    case KEY_F(1):  return P4_KEY_k1;
    case KEY_F(2):  return P4_KEY_k2;
    case KEY_F(3):  return P4_KEY_k3;
    case KEY_F(4):  return P4_KEY_k4;
    case KEY_F(5):  return P4_KEY_k5;
    case KEY_F(6):  return P4_KEY_k6;
    case KEY_F(7):  return P4_KEY_k7;
    case KEY_F(8):  return P4_KEY_k8;
    case KEY_F(9):  return P4_KEY_k9;
    case KEY_F(10): return P4_KEY_k0;
    case KEY_LEFT:  return P4_KEY_kl;
    case KEY_RIGHT: return P4_KEY_kr;
    case KEY_UP:    return P4_KEY_ku;
    case KEY_DOWN:  return P4_KEY_kd;
    case KEY_HOME:  return P4_KEY_kh;
    case KEY_END:   return P4_KEY_kH;
    case KEY_PPAGE: return P4_KEY_kP;
    case KEY_NPAGE: return P4_KEY_kN;
    case KEY_BACKSPACE: return '\b';
    case KEY_DC:    return P4_KEY_kD;
    case KEY_IC:    return P4_KEY_kI;
    case KEY_IL:    return P4_KEY_kA;
    case KEY_EOL:   return P4_KEY_kE;
    case KEY_DL:    return P4_KEY_kL;
    case KEY_CLEAR: return P4_KEY_kC;
    default:
	return 0;
    }
}

static int
c_keypressed (void)
{
    int k;
    refresh ();
    nodelay (stdscr, TRUE);
    k = getch ();
    nodelay (stdscr, FALSE);
    return k != -1;
}

static int
c_getkey (void)
{
    int vkey;
    refresh ();

    if (PFE.wait_for_stdin)
	PFE.wait_for_stdin ();

    vkey = c_getvkey ();
    if (vkey > 0x100) return 0;
    else return vkey;
}

static void 
c_putc_noflush (char c)
{
    addch (c);
}
static void c_put_flush ()		{ refresh (); }
static void c_putc (char c)		{ addch (c); refresh (); }
static void c_puts (const char *s)	{ addstr ((char *)s); refresh (); }

static void c_gotoxy (int x, int y)	{ move (y, x); }
static void c_wherexy (int *x, int *y)	{ getyx (stdscr, *y, *x); }

static void
addxy (int x, int y)
{
    int col, row;
    getyx (stdscr, row, col);
    move (row + y, col + x);
}

static void 
c_tput (int attr)
{
    switch (attr)
    {
    case P4_TERM_GOLEFT:	addxy (-1,  0); 	break;
    case P4_TERM_GORIGHT:	addxy ( 1,  0); 	break;
    case P4_TERM_GOUP:		addxy ( 0, -1); 	break;
    case P4_TERM_GODOWN:	addxy ( 0,  1); 	break;
	
    case P4_TERM_CLRSCR:	clear (); refresh (); 	break;
    case P4_TERM_HOME:		move (0, 0); 		break;
    case P4_TERM_CLREOL:	clrtoeol (); 		break;
    case P4_TERM_CLRDOWN:	clrtobot (); 		break;
    case P4_TERM_BELL:		beep (); 		break;
	
    case P4_TERM_NORMAL:	attrset (A_NORMAL); 	break;
    case P4_TERM_BOLD_ON:	standout (); 		break;
    case P4_TERM_BOLD_OFF:	standend (); 		break;
    case P4_TERM_BRIGHT:	attron (A_BOLD); 	break;
    case P4_TERM_REVERSE:	attron (A_REVERSE); 	break;
    case P4_TERM_BLINKING:	attron (A_BLINK); 	break;
    case P4_TERM_UNDERLINE_ON:	attron (A_UNDERLINE); 	break;
    case P4_TERM_UNDERLINE_OFF:	attroff (A_UNDERLINE);	break;

    default: break;
   }
}

#ifdef __GNUC__
#define INTO(x) .x =
#else
#define INTO(x)
#endif

p4_term_struct p4_term_ios =
{
  "curses",
  0, 
  0, /* no rawkeys -> use _getvkey */
  INTO(init) 		c_prepare_terminal, 
  INTO(fini) 		c_cleanup_terminal,
  INTO(tput)		c_tput,

  INTO(tty_interrupt_key) c_interrupt_key,
  INTO(interactive_terminal) c_interactive_terminal,
  INTO(system_terminal)   c_system_terminal,
  INTO(query_winsize)     c_query_winsize,

  INTO(c_keypressed)	c_keypressed,
  INTO(c_getkey)	c_getkey,
  INTO(c_putc_noflush)  c_putc_noflush,
  INTO(c_put_flush)	c_put_flush,
  INTO(c_putc)		c_putc,
  INTO(c_puts)		c_puts,
  INTO(c_gotoxy)	c_gotoxy,
  INTO(c_wherexy)	c_wherexy,

  INTO(c_getvkey)       c_getvkey
};

