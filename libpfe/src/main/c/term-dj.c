/**
 * --	terminal driver fo djgpp
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2003.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2008-04-20 04:46:30 $)
 *
 *  @description
 *                      Terminal driver for DOS and OS/2 with DJGPP,
 *			there is almost nothing to do.
 *
 * Adapted from term-emx.c by Antonio Costa (acc@asterix.inescn.pt)
 * Works in 25 and 50 lines text modes (at least).
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) =
"@(#) $Id: term-dj.c,v 1.3 2008-04-20 04:46:30 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-sub.h>
#include <pfe/term-sub.h>

#include <pc.h>
#include <pfe/os-ctype.h>

char *
term_dj_rawkey_string [P4_NUM_KEYS] =	/* what function keys send */
{
    "\377;", "\377<", "\377=", "\377>", "\377?",
    "\377@", "\377A", "\377B", "\377C", "\377D",
    "\377K", "\377M", "\377H", "\377P",
    "\377G", "\377O", "\377Q", "\377I",
    NULL,	   "\377S", NULL,    "\377R",
    NULL,    NULL,    NULL,    NULL,    /*"\r"*/
};

static int c_interrupt_key (char ch)		{ return 0; }
static void c_interactive_terminal (void)	{}
static void c_system_terminal (void)		{}
static void c_query_winsize (void)		{}

static int
c_prepare_terminal (void)
{
    setbuf (stdout, NULL);
    PFE.cols = ScreenCols ();
    PFE.rows = ScreenRows ();
    return 1;
}

static void
c_cleanup_terminal (void)
{
    return; /* nothing to do here */
}


static int
c_keypressed (void)
{
    return kbhit ();
}

#undef getkey

static int nextkey = 0;

static int newgetkey (void)
{
    int key;

    if (nextkey)
    {
        key = nextkey;
        nextkey = 0;
        return key;
    }
    key = getkey ();
    if (key > 255)
    {
        nextkey = key & 255;
        return 255;
    }
    return key;
}

#define BLINK        0x0080
#define INTENSITY    0x0008
#define BW_NORMAL    0x0007
#define BW_UNDERLINE 0x0004
static int attribs = BW_NORMAL;

#if 1
static void
c_putc (char c)
{
    int row, col;

    if (attribs == BW_NORMAL || p4_iscntrl(c))
    {
        putchar (c);
        return;
    }
    ScreenGetCursor (&row, &col);
    ScreenPutChar (c, attribs, col, row);
    ScreenSetCursor (row, col + 1);
}
static void
c_puts (const char *s)
{
    int i;

    if (attribs == BW_NORMAL)
    {
        fputs (s, stdout);
        return;
    }
    for (i = 0; s[i]; i++)
        c_putc(s[i]);
}
#else
static void c_putc (char c)		{ putchar (c); }
static void c_puts (const char *s)	{ fputs (s, stdout); }
#endif

static void c_gotoxy (int x, int y)	{ ScreenSetCursor (y, x); }
static void c_wherexy (int *x, int *y)	{ ScreenGetCursor (y, x); }

static void _addxy (int x, int y)
{
    int col, row;

    ScreenGetCursor (&row, &col);
    ScreenSetCursor (row + y, col + x);
}

static void _clrscr (void)	{ ScreenClear (); ScreenSetCursor (0, 0); }
static void _home (void)	{ ScreenSetCursor (0, 0); }
static void _clreol (void)
{
    int i, col;

    ScreenGetCursor (&i, &col);
    for (i = col; i < PFE.cols; i++)
        putchar (' ');
}

static void _clrdown (void)
{
    int i, row, col;

    ScreenGetCursor (&row, &col);
    clreol ();
    for (i = row + 1; i < PFE.rows; i++)
    {
        gotoxy (0, i);
        clreol ();
    }
    gotoxy (col, row);
}


static int c_tput (int attr)
{
    switch (attr)
    {
     case P4_TERM_GOLEFT:	_addxy (-1,  0); break;
     case P4_TERM_GORIGHT:	_addxy ( 1,  0); break;
     case P4_TERM_GOUP:		_addxy ( 0, -1); break;
     case P4_TERM_GODOWN:	_addxy ( 0,  1); break;

     case P4_TERM_CLRSCR:	_clrscr (); break;
     case P4_TERM_HOME:		_home (); break;
     case P4_TERM_CLREOL:	_clreol (); break;
     case P4_TERM_CLRDOWN:	_clrdown (); break;

     case P4_TERM_BELL:		putchar ('\a'); break;

     case P4_TERM_NORMAL:	attribs = BW_NORMAL; break;
     case P4_TERM_BOLD_ON:	attribs |= INTENSITY; break;
     case P4_TERM_BOLD_OFF:	attribs &= ~INTENSITY; break;
     case P4_TERM_BRIGHT:	standout_on (); break;
     case P4_TERM_REVERSE:	attribs = ((attribs & 0x0007) << 4) |
                                            (attribs & 0x0088); break;
     case P4_TERM_BLINKING:	attribs |= BLINK; break;
     case P4_TERM_UNDERLINE_ON:	attribs = (attribs&0x00F8) | BW_UNDERLINE;
         break;
     case P4_TERM_UNDERLINE_OFF: attribs = BW_NORMAL; break;
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
    "term-dj",
    0, term_dj_rawkey_string,
    INTO(init) 		c_prepare_terminal,
    INTO(fini) 		c_cleanup_terminal,
    INTO(tput)		c_tput,

    INTO(tty_interrupt_key) c_interrupt_key,
    INTO(interactive_terminal) c_interactive_terminal,
    INTO(system_terminal)   c_system_termainl,
    INTO(query_winsize)     c_query_winsize,

    INTO(c_keypressed)	c_keypressed,
    INTO(c_getkey)	c_getkey,
    INTO(c_putc_noflush)  c_putc_noflush,
    INTO(c_put_flush)	c_put_flush,
    INTO(c_putc)		c_putc,
    INTO(c_puts)		c_puts,
    INTO(c_gotoxy)	c_gotoxy,
    INTO(c_wherexy)	c_wherexy
};
/*@}*/
