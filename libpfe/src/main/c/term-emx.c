/**
 * -- terminal driver for emx
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2003.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author$)
 *  @version $Revision$
 *     (modified $Date$)
 *
 *  @description
 *                      Terminal driver for DOS and OS/2 with EMX,
 *                      there is almost nothing to do.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) =
"@(#) $Id$";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-ext.h>
#include <pfe/term-sub.h>

#include <stdlib.h>		/* _read_kbd() */
#include <sys/video.h>		/* all those v_...() functions */

#include <pfe/_missing.h>

char *
term_emx_rawkey_string[P4_NUM_KEYS] =	/* what function keys send */
{
    "\377;", "\377<", "\377=", "\377>", "\377?",
    "\377@", "\377A", "\377B", "\377C", "\377D",
    "\377T", "\377U", "\377V", "\377W", "\377X",
    "\377Y", "\377Z", "\377[", "\377\\", "\377]",
    "\377K", "\377M", "\377H", "\377P",
    "\377G", "\377O", "\377Q", "\377I",
    NULL, "\377S", NULL, "\377R",
    NULL, NULL, NULL, NULL,	/*"\r" */
};

static int c_interrupt_key (char ch)		{ return 0; }
static void c_interactive_terminal (void)	{ v_init (); }
static void c_system_terminal (void)		{}
static void c_query_winsize (void)		{}

static int
c_prepare_terminal (void)
{
    v_init ();
    v_dimen (&PFE.cols, &PFE.rows);
    return 1;
}

static void
c_cleanup_terminal (void)
{
    return; /* nothing to do here */
}

#define NOCH 0x789ABCDE
static int nxch = NOCH;

static int
c_keypressed (void)
{
    int c;

    if (nxch != NOCH)
        return 1;
    c = _read_kbd (0, 0, 0);
    if (c == -1)
        return 0;
    nxch = c;
    return 1;
}

static int
getch0 (void)
{
    if (nxch != NOCH)
    {
        int ch = nxch;

        nxch = NOCH;
        return ch;
    }
    for (;;)
    {
        int c = _read_kbd (0, 1, 0);

        if (c != -1)
            return c;
    }
}

static int				/* return '\377' instead of DOS' '\0' */
c_getkey (void)			/* for function keys. */
{
    int c = getch0 ();

    return c == 0 ? '\377' : c;
}

static void
c_putc_noflush (char c)
{
    int x, y;

    switch (c)			/* v_putc doesn't interpret some */
    {				/* very common control codes */
     case '\r':
         v_getxy (&x, &y);
         v_gotoxy (0, y);
         break;
     case '\b':
         c_goleft ();
         break;
     case '\t':
         do {
             v_putc (' ');
             v_getxy (&x, &y);
         } while (x % 8);
         break;
     default:
         v_putc (c);
    }
}

static void
c_put_flush (void)
{
}

static void
c_putc (char c)
{
    p4_putc_noflush (c);
}

static void
c_puts (const char *s)
{
    while (*s)
        p4_putc_noflush (*s++);
}

static void
c_gotoxy (int x, int y)
{
    v_gotoxy (x, y);
}

static void
c_wherexy (int *x, int *y)
{
    v_getxy (x, y);
}



static void
_addxy (int x, int y)
{
    int col, row;

    v_getxy (&col, &row);
    col += x;
    row += y;
    v_gotoxy (col, row);
}

static void
_clrdown (void)
{
    int i, row, col;

    v_getxy (&col, &row);
    v_clreol ();
    for (i = row + 1; i < PFE.rows; i++)
    {
        v_gotoxy (0, i);
        v_clreol ();
    }
    v_gotoxy (col, row);
}

static void
c_tput (int attr)
{
    switch (attr)
    {
     case P4_TERM_GOLEFT:	_addxy (-1,  0); break;
     case P4_TERM_GORIGHT:	_addxy ( 1,  0); break;
     case P4_TERM_GOUP:		_addxy ( 0, -1); break;
     case P4_TERM_GODOWN:	_addxy ( 0,  1); break;

# if 0
     case P4_TERM_CLRSCR:	v_clear (); break;
# else
     case P4_TERM_CLRSCR:	v_gotoxy (0, 0); _clrdown (); break;
# endif
     case P4_TERM_HOME:		v_gotoxy (0, 0); break;
     case P4_TERM_CLREOL:	v_clreol (); break;
     case P4_TERM_CLRDOWN:	_clrdown (); break;
     case P4_TERM_BELL:		putchar ('\a'); break;

     case P4_TERM_NORMAL:	v_attrib (BW_NORMAL); break;
     case P4_TERM_BOLD_ON:	v_attrib (v_getattr () | INTENSITY); break;
     case P4_TERM_BOLD_OFF:	v_attrib (v_getattr () & ~INTENSITY); break;
     case P4_TERM_BRIGHT:	c_standout_on (); break;
     case P4_TERM_REVERSE:	v_attrib (BW_REVERSE); break;
     case P4_TERM_BLINKING:	v_attrib (v_getattr () | BLINK); break;
     case P4_TERM_UNDERLINE_ON:	v_attrib (BW_UNDERLINE); break;
     case P4_TERM_UNDERLINE_OFF:v_attrib (BW_NORMAL); break;
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
    "term-emx",
    0, term_emx_rawkey_string,
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
    INTO(c_putc)	c_putc,
    INTO(c_puts)	c_puts,
    INTO(c_gotoxy)	c_gotoxy,
    INTO(c_wherexy)	c_wherexy
};

/*@}*/
