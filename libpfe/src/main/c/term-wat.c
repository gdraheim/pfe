/**
 * -- terminal driver for Watcom C
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
 *                      Terminal driver for Watcom C, uses functions from
 *                      conio.h and ANSI escape sequences for attributes.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) =
"@(#) $Id$";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-sub.h>
#include <pfe/term-sub.h>
#include <conio.h>
#include <stdlib.h>

/* defines from the 10.5 docs:
 M_I86  // 16-bit intel
 M_I386 // 32-bit intel
 __386__ // again, 32-bit intel
 _M_IX86 // 500 means commandline-option of "/5"=pentium-oriented
 __DOS__ // dos-target, including extenders
 MSDOS   // again, dos-target including extenders
 __OS2__ // os2-target, both 16-bit or 32-bit
 __QNX__ // qnx-target, both 16-bit or 32-bit
 __NT__  // just NT
 __WINDOWS__ // any windows
 __WINDOWS_386__ // 32-bit windows
 __FPI__ // fpi or fpi87
 __CHAR_SIGNED_
 __CHEAP_WINDOWS__
 __INLINE_FUNCTIONS__
 __FLAT__ // for 32-bit in --model=flat mode
*/

#ifdef __WATCOMC__ /* put compatible compiler-defs in here */
#if defined __OS2__
#define __host_os_os2_watcom
#define __host_os_os2
#elif defined __DOS__ && __386__
#define __host_os_dos32_watcom
#define __host_os_dos32
#define __host_os_dos
#elif defined __DOS__
#define __host_os_dos16_watcom
#define __host_os_dos16
#define __host_os_dos
#elif defined __WINDOWS_386__
#define __host_os_win32_watcom
#define __host_os_win32
#define __host_os_win
#elif defined __WINDOWS__
#define __host_os_win16_watcom
#define __host_os_win16
#define __host_os_win
#else
#error did not detect target-type
#endif
#endif

#if defined __host_os_os2                /* __OS2__ should do here but doesn't */
# define INCL_KBD 1
# define INCL_VIO 1
# include <pfe/os-string.h>
# include <os2.h>
#elif defined __host_os_dos
# include <dos.h>
# include <i86.h>
#elif defined __host_os_win16
# include <stdio.h>
#endif

#include <pfe/def-types.h>


char *
term_wat_rawkey_string [P4_NUM_KEYS] =  /* what function keys send */
{
    "\377;", "\377<", "\377=", "\377>", "\377?",
    "\377@", "\377A", "\377B", "\377C", "\377D",
    "\377K", "\377M", "\377H", "\377P",
    "\377G", "\377O", "\377Q", "\377I",
    NULL,          "\377S",    NULL,    "\377R",
    NULL,    NULL,    NULL,    NULL,    /*"\r"*/
};

static int c_interrupt_key (char ch)            { return 0; }
static void c_interactive_terminal (void)       {}
static void c_system_terminal (void)            {}
static void c_query_winsize (void)              {}

#if defined __host_os_dos32
static int page;                /* display screen, 0-7 */
#endif

static int
c_prepare_terminal (void)
{
#if defined __host_os_os2

    VIOMODEINFO viomi;
    VioGetMode (&viomi, 0);
    PFE.rows = viomi.row;
    PFE.cols = viomi.col;
    VioSetAnsi (1, 0);

#elif defined __host_os_dos32

    union REGS regs;
    regs.h.ah = 15;             /* int 10 subfunction 15, get video mode */
    int386 (0x10, &regs, &regs);
    switch (regs.h.al)
    {
     default:
         page     = regs.h.bh;
         PFE.rows = 25;
         PFE.cols = regs.h.ah;
    }

#endif

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
#if defined __host_os_os2
    KBDKEYINFO kbci;
    KbdPeek (&kbci, 0);
    return kbci.fbStatus >> 6 & 1;
#else
    return kbhit ();
#endif
}

static int                      /* return '\377' instead of DOS' '\0' */
c_getkey (void)                 /* for function keys. */
{
    int c;
#if defined __host_os_os2
    KBDKEYINFO kbci;
    KbdCharIn (&kbci, 0, 0);
    c = kbci.chChar;
#else
    c = getch ();
#endif
    return (c &0xFF) == 0 ? '\377' : c;
}

#if defined __host_os_os2
  /* the following gives an internal compiler error */
# define CPUTC(C) VioWrtTTY (&(C), 1, 0)
# define CPUTS(S) VioWrtTTY (S, (p4_strlen (S)), 0)
#elif defined host_os_win32
# define CPUTC(C) putchar (C)
# define CPUTS(S) fputs (S, stdout)
#elif defined __host_os_mingw
# define CPUTC(C) _putch (C)
# define CPUTS(S) _cputs (S)
#else
# define CPUTC(C) putch (C)
# define CPUTS(S) cputs (S)
#endif

static void
c_putc_noflush (char c)
{
    switch (c)
    {
     case '\n':
         CPUTS ("\r\n");
         break;
     default:
         CPUTC (c);
         break;
    }
}

static void
c_put_flush (void)
{
#if defined __host_os_win
    fflush (stdout);
#endif
}

static void
c_putc (char c)
{
    c_putc_noflush (c);
    c_put_flush ();
}

static void
c_puts (const char *s)
{
    while (*s)
        c_putc_noflush (*s++);
    c_put_flush ();
}

static void
c_gotoxy (int x, int y)
{
#  if defined __host_os_os2
    VioSetCurPos (y, x, 0);
#  elif defined __host_os_dos32
    union REGS regs;
    regs.h.ah = 2;              /* int 10 subfunction 2, set cursor pos */
    regs.h.bh = page;           /* logical screen */
    regs.h.dl = x;
    regs.h.dh = y;
    int386 (0x10, &regs, &regs);
#   elif defined __host_os_mingw
    _cprintf ("\033[%d;%dH", y, x);
#   else
    cprintf ("\033[%d;%dH", y, x);
#  endif
}

static void
c_wherexy (int *x, int *y)
{
#  if defined __host_os_os2
    USHORT row, col;
    VioGetCurPos (&row, &col, 0);
    *x = col;
    *y = row;
#  elif defined __host_os_dos32
    union REGS regs;
    regs.h.ah = 3;              /* int 10 subfunction 3, get cursor pos */
    regs.h.bh = page;           /* logical screen */
    int386 (0x10, &regs, &regs);
    *x = regs.h.dl;
    *y = regs.h.dh;
# else
    *x = *y = 0;                        /* uargh! */
# endif
}

static void                     /* move cursor in x and y */
addxy (int dx, int dy)
{
    int x, y;
    c_wherexy (&x, &y);
    x += dx;
    y += dy;
    c_gotoxy (x, y);
    p4_OUT = x;
}

static void
c_clreol (void)
{
# if defined __host_os_os2
    static char cell [2] = { ' ', 0x07 };
    USHORT row, col;
    VioGetCurPos (&row, &col, 0);
    VioWrtNCell (cell, PFE.cols - col, row, col, 0);
#else
    CPUTS ("\033[K");
#endif
}

static void
c_clrdown (void)
{
# if defined __host_os_os2
    static char cell [2] = { ' ', 0x07 };
    int r;
    USHORT row, col;

    VioGetCurPos (&row, &col, 0);
    VioWrtNCell (cell, PFE.cols - col, row, col, 0);
    for (r = row + 1; r < PFE.rows; r++)
        VioWrtNCell (cell, PFE.cols, r, 0, 0);
# else /*if defined __host_os_dos32 */
    int x, y, i;

    c_clreol ();
    c_wherexy (&x, &y);
    for (i = y + 1; i < PFE.rows; i++)
    {
        c_gotoxy (i, 0);
        c_clreol ();
    }
    c_gotoxy (x, y);
# endif
}

enum {
    none, bold, faint, italic, blink = 5,
    rapid_blink, reverse_video, concealed
};

static int attrib;

static void
setattr (int attr)
{
    if (attr == none)
    {
        attrib = 0;
        CPUTS ("\033[0m");
    }else{
        attrib |= 1 << attr;
#     ifdef __host_os_mingw
        _cprintf ("\033[%dm", attr);
#     else
        cprintf ("\033[%dm", attr);
#     endif
    }
}

static void
clrattr (int attr)
{
    int i;

    attrib &= ~(1 << attr);
    CPUTS ("\033[0");
    for (i = bold; i <= concealed; i++)
        if (attrib >> i & 1)
#         ifdef __host_os_mingw
            _cprintf (";%d", i);
#         else
            cprintf (";%d", i);
#         endif
    CPUTS ("m");
}

static void
c_tput (int attr)
{
    switch (attr)
    {
     case P4_TERM_GOLEFT:               addxy (-1,  0); break;
     case P4_TERM_GORIGHT:              addxy ( 1,  0); break;
     case P4_TERM_GOUP:                 addxy ( 0, -1); break;
     case P4_TERM_GODOWN:               addxy ( 0,  1); break;

     case P4_TERM_HOME:                 c_gotoxy (0, 0); break;
     case P4_TERM_CLRSCR:               c_gotoxy (0, 0); c_clrdown (); break;
     case P4_TERM_CLRDOWN:              c_clrdown (); break;
     case P4_TERM_CLREOL:               c_clreol (); break;
     case P4_TERM_BELL:                 CPUTS ("\a"); break;

     case P4_TERM_NORMAL:               setattr (none); break;
     case P4_TERM_BOLD_ON:              setattr (bold); break;
     case P4_TERM_BOLD_OFF:             clrattr (bold); break;
     case P4_TERM_REVERSE:              setattr (reverse_video); break;
     case P4_TERM_BRIGHT:               setattr (bold); break;
     case P4_TERM_BLINKING:             setattr (blink); break;
     case P4_TERM_UNDERLINE_ON:         setattr (italic); break;
     case P4_TERM_UNDERLINE_OFF:        clrattr (italic); break;
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
    "term-watcom",
    0, // term_wat_control_string, FIXME: !!!!!!!!!!!!!!!!!!
    term_wat_rawkey_string,
    INTO(init)          c_prepare_terminal,
    INTO(fini)          c_cleanup_terminal,
    INTO(tput)          c_tput,

    INTO(tty_interrupt_key) c_interrupt_key,
    INTO(interactive_terminal) c_interactive_terminal,
    INTO(system_terminal)   c_system_terminal,
    INTO(query_winsize)     c_query_winsize,

    INTO(c_keypressed)  c_keypressed,
    INTO(c_getkey)      c_getkey,
    INTO(c_putc_noflush)  c_putc_noflush,
    INTO(c_put_flush)   c_put_flush,
    INTO(c_putc)                c_putc,
    INTO(c_puts)                c_puts,
    INTO(c_gotoxy)      c_gotoxy,
    INTO(c_wherexy)     c_wherexy
};

/*@}*/
