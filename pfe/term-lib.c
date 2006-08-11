/** 
 * -- terminal driver for unix-like systems
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2006 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2006-08-11 22:56:05 $)
 *
 *  @description
 *                      Terminal driver for UNIX-like systems using
 *			termcap or the termcap emulation in curses.
 *
 * Refer to this file as an example of how to write such a driver, though
 * other drivers for non-portable operating systems should be much simpler.
 * E.g. on an IBM-PC you could do everything here with a few INT-10h calls.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: term-lib.c,v 1.3 2006-08-11 22:56:05 guidod Exp $";
#endif

#define _P4_SOURCE 1
/* #define _P4_NO_REGS_SOURCE 1 */
#include <pfe/pfe-base.h> /* */

#include <pfe/def-cell.h>
#include <pfe/def-limits.h>

#include <stdio.h>		/* putchar() */
#include <stdlib.h>		/* getenv() */
#include <pfe/os-string.h>		/* p4_strlen() */
#include <errno.h>		/* EINTR */

#ifdef PFE_HAVE_UNISTD_H 
#include <unistd.h>
#endif
#ifdef PFE_HAVE_FCNTL_H
#include <fcntl.h>
#endif

#include <pfe/term-sub.h>
#include <pfe/_nonansi.h>
#include <pfe/_missing.h>

/* sun manpages say "int putchar(int)" but their tputs is not quite like it
 * with being int "tputs(char *str, int affcnt, int (*putc)(char));". The
 * linux and hpux manges use "...., int affcnt, int (*putc)(int)));" instead.
 * update: solaris9 has changed its headers to match standard "int" parameter
 */
# if defined HOST_OS_SOLARIS && !defined HOST_OS_SOLARIS2_9
# define t_putchar_ ((int (*)(char)) t_putchar)
# else
# define t_putchar_ t_putchar
# endif

#if ! defined PFE_HAVE_FCNTL && defined F_GETFL && defined F_SETFL
#define PFE_HAVE_FCNTL 1 /* should have been done by configure... */
#endif

#ifdef PFE_HAVE_FCNTL
#define P4_fcntl_F_GETFL(_FD_,_ARG_) (fcntl((_FD_),F_GETFL,(_ARG_)))
#define P4_fcntl_F_SETFL(_FD_,_ARG_) (fcntl((_FD_),F_SETFL,(_ARG_)))
#else
int P4_fcntl_F_GETFL(int fd, int arg)
{
    static seen;
    if (! seen)
    {
        P4_warn2 ("no fcntl - ignored F_GETFL %i on fd %i", arg, fd);
        seen++;
    }
    return 0;
}
int P4_fcntl_F_SETFL(int fd, int arg)
{
    static seen;
    if (! seen)
    {
        P4_warn2 ("no fcntl - ignored F_SETFL %i on fd %i", arg, fd);
        seen++;
    }
}
#endif

/* ...................................................................... */

/*-
 * Table of contents
 *
 * 1      Initialization for usage of screen package
 * 1.1      File system, blocking/nonblocking io
 * 1.2      Setting tty driver flags
 * 1.2.1      Old System-V (termio) and Posix (termios) styles
 * 1.2.2      V7 and BSD style
 * 2      Getting information about the terminal
 * 2.1      Simply assume vt100
 * 2.2      Use termcap
 * 2.3      Use terminfo
 */

/* forward */ 
static int  c_interrupt_key (char ch);
static int  c_prepare_terminal (void);
static void c_cleanup_terminal (void);
static void c_interactive_terminal (void);
static void c_system_terminal (void);
static void c_query_winsize (void);

static int c_keypressed (void);	
static int c_getkey (void);

static void c_putc_noflush (char c);
static void c_put_flush (void);
static void c_putc (char c);
static void c_puts (const char *s);
static void c_gotoxy (int x, int y);
static void c_wherexy (int *x, int *y);
static void c_tput (int);


/*-
 * 1 Initialization for usage of screen package
 *
 * All this stuff isn't neccessary or very easy on non unix-like machines
 * where there are direct console i/o functions available. See term-emx.c.
 * It implements the three required functions:
 *
 *	- prepare_terminal()      called once at startup
 *	- interactive_terminal()  called to set up the terminal for p4th
 *	- system_terminal()       called to reset the terminal on exit
 *
 * One problem is that in Unix-like OS console i/o is done via the
 * file system. It takes a little hacking to get an interactive
 * flavour from this arrangement. The kind of hacking changes while
 * Unix evolves. The hacking is addressed at two instances:
 *
 *    - UNIX file system. fcntl() calls on the standard input are used
 *	to prevent it from waiting for a pressed key when we only want
 *	to detect if a key is pressed.
 *
 *    - terminal device driver. It must be configured to pass keys
 *	immediately instead of assembling it to lines, to pass through
 *	all keys and not interpret some of them or interpret those keys
 *	we need in a way we want.
 *
 * All what's changed must be carefully restored on exit otherwise
 * the terminal won't work any more.
 */

/*
 * 1.1 file system, blocking/nonblocking io:
 */

#ifndef O_NONBLOCK
# ifdef O_NDELAY
#   define O_NONBLOCK O_NDELAY
# else
#   error "neither O_NONBLOCK nor O_NDELAY defined"
# endif
#endif

static int saved_fcntl[3] = {0}; /* fixme: in multithreaded, use only once */

static void			/* set file associated with fd to */
waitchar (int fd)		/* "wait for character" */
{
    int flags = P4_fcntl_F_GETFL (fd, 0); /* fcntl (fd, F_GETFL, 0); */
    P4_fcntl_F_SETFL (fd, flags & ~O_NONBLOCK);
}

static void			/* set file associated with fd to */
no_waitchar (int fd)		/* "don't wait for character" */
{
    int flags = P4_fcntl_F_GETFL (fd, 0);  /* fcntl (fd, F_GETFL, 0); */
    P4_fcntl_F_SETFL (fd, flags | O_NONBLOCK);
}

/*
 * 1.2 tty hacking, two major styles:
 */

#if (defined PFE_HAVE_TERMIO_H || defined PFE_HAVE_TERMIOS_H) \
    && !defined __target_os_nextstep

/*
 * 1.2.1 old System-V (termio) and Posix (termios) styles
 */

#if defined PFE_HAVE_TERMIOS_H

#include <termios.h>

#elif defined PFE_HAVE_TERMIO_H

#include <termio.h>

/* make old SysV style look enough like Posix: */
#define termios termio
#define tcsetattr(F,X,M)	ioctl (F, TCSETAW, M)
#define tcgetattr(F,M)		ioctl (F, TCGETA, M)

#ifndef VINTR
#define VINTR 0
#endif
#ifndef VQUIT
#define VQUIT 1
#endif
#ifndef VERASE
#define VERASE 2
#endif
#ifndef VMIN
#define VMIN 4
#endif
#ifndef VTIME
#define VTIME 5
#endif

#endif

static struct termios tty_system;
#define tty_save()	tcgetattr (STDIN_FILENO, &tty_system)
#define tty_restore()	tcsetattr (STDIN_FILENO, TCSAFLUSH, &tty_system)
#ifdef CBAUD
# define tty_ospeed	(tty_system.c_cflag & CBAUD)
#else
# define tty_ospeed	(tty_system.c_ospeed)
#endif
#define tty_erasechar	(tty_system.c_cc[VERASE])

int
c_interrupt_key (char ch)
/*
 * If ch != 0 enables SIGINT on key ch, else disables SIGINT
 * returns old break key.
 * Porting this function might be hard. It isn't neccessary though.
 * This is needed only by the block editor: What's usually the break key
 * means exit to the block editor. If you have no break key or you have
 * it somewhere else than ^U, just #define this to nothing in term.h.
 */
{
    struct termios tty;
    int old;

    if (!isatty (0))
	return -1;
    if (tcgetattr (STDIN_FILENO, &tty) != 0)
	return -1;
    old = tty.c_cc[VINTR];
    if (ch)
    {
	tty.c_iflag |= BRKINT;
	tty.c_cc[VINTR] = 0xFF;
    }
    else
    {
	tty.c_iflag &= ~BRKINT;
	tty.c_cc[VINTR] = ch;
    }
    tcsetattr (0, TCSAFLUSH, &tty);
    return old;
}

#define	C_IFLAGS_OFF	(ICRNL | IGNBRK | IGNCR | INLCR | \
			 ISTRIP | IXOFF | IXON)
#define C_IFLAGS_ON	(BRKINT)
#define C_OFLAGS_OFF	(0)
#define C_OFLAGS_ON	(0)
#define C_LFLAGS_OFF	(ECHO | ICANON)
#define C_LFLAGS_ON	(ISIG)

/*
 * Special keys for this unix-prone driver...
 */

#ifndef INTR_KEY                /* USER-CONFIG: */
#define	INTR_KEY	'\025'	/* (^U) key to send a SIGINT */
#endif
#ifndef QUIT_KEY                /* USER-CONFIG: */
#define	QUIT_KEY	'\034'	/* (^\) key to send a SIGQUIT */
#endif
#ifndef SUSP_KEY                /* USER-CONFIG: */
#define	SUSP_KEY	'\032'	/* (^Z) key to send a SIGTSTP */
#endif

static void
tty_interactive (void)
{
    /* struct termios tty = tty_system; */
    struct termios tty; p4_memcpy(&tty, &tty_system, sizeof(tty));

    if (! isatty (STDIN_FILENO)) return;

    tty.c_iflag &= ~C_IFLAGS_OFF, tty.c_iflag |= C_IFLAGS_ON;
    tty.c_oflag &= ~C_OFLAGS_OFF, tty.c_oflag |= C_OFLAGS_ON;
    tty.c_lflag &= ~C_LFLAGS_OFF, tty.c_lflag |= C_LFLAGS_ON;
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 0;
    tty.c_cc[VINTR] = INTR_KEY;
    tty.c_cc[VQUIT] = QUIT_KEY;
#  if defined PFE_HAVE_TERMIOS_H
    tty.c_cc[VSUSP] = SUSP_KEY;
    tty.c_cc[VSTART] = '\xFF';
    tty.c_cc[VSTOP] = '\xFF';
#  endif
    tcsetattr (STDIN_FILENO, TCSAFLUSH, &tty);
}

#elif defined PFE_HAVE_SGTTY_H

/*
 * 1.2.2 V7 and BSD style, when struct sgttyb is defined
 */

#include <sgtty.h>

static struct
{
    struct sgttyb sg;
    struct tchars tc;
    int lc;			/* local mode word */
}
tty_system;

#define tty_save()	ioctl (STDIN_FILENO, TIOCGETP, &tty_system.sg), \
			ioctl (STDIN_FILENO, TIOCGETC, &tty_system.tc), \
			ioctl (STDIN_FILENO, TIOCLGET, &tty_system.lc)
#define tty_restore()	ioctl (STDIN_FILENO, TIOCSETP, &tty_system.sg), \
			ioctl (STDIN_FILENO, TIOCSETC, &tty_system.tc), \
			ioctl (STDIN_FILENO, TIOCLSET, &tty_system.lc)
#define tty_ospeed	(tty_system.sg.sg_ospeed)
#define tty_erasechar	(tty_system.sg.sg_erase)

static int
c_interrupt_key (char ch)
/*
 * If ch != 0 enables SIGINT on key ch, else disables SIGINT
 * returns old break key.
 * Porting this function might be hard. It isn't neccessary though.
 * This is needed only by the block editor: What's usually the break key
 * means exit to the block editor. If you have no break key or you have
 * it somewhere else than ^U, just #define this to nothing in term.h.
 */
{
    struct tchars tc;
    int old;

    if (!isatty (0))
	return -1;
    if (ioctl (STDIN_FILENO, TIOCGETC, &tc) != 0)
	return -1;
    old = tc.t_intrc;
    tc.t_intrc = ch == 0 ? -1 : ch;
    ioctl (STDIN_FILENO, TIOCSETC, &tc);
    return old == -1 ? 0 : old;
}

#define	C_FLAGS_OFF	(LCASE | ECHO | RAW)
#define C_FLAGS_ON	(CBREAK)
#define L_FLAGS_ON	(LTOSTOP | LPASS8)

static void
tty_interactive (void)
{
    struct sgttyb sg = tty_system.sg;
    struct tchars tc = tty_system.tc;
    int flags;

    if (! isatty (STDIN_FILENO)) return;
    
    sg.sg_flags &= ~C_FLAGS_OFF;
    sg.sg_flags |=  C_FLAGS_ON;
    tc.t_intrc = INTR_KEY;
    tc.t_quitc = QUIT_KEY;
    tc.t_startc = -1;
    tc.t_stopc = -1;
    ioctl (STDIN_FILENO, TIOCSETP, &sg);
    ioctl (STDIN_FILENO, TIOCSETC, &tc);
    flags = L_FLAGS_ON;
    ioctl (STDIN_FILENO, TIOCLBIS, &flags);
}

#else

#if defined VxWorks 

#include <vxWorks.h>
#include <ioLib.h>

static int tty_system = 0; /* not reentrant */
#define tty_save()    (tty_system = ioctl(0,FIOGETOPTIONS,0))
#define tty_restore() (ioctl(0,FIOSETOPTIONS,tty_system))


static int
c_interrupt_key (char ch)
{
    return 0;
}

static void
tty_interactive (void)
{
    ioctl (0, FIOSETOPTIONS, ioctl (0, FIOGETOPTIONS, 0) & ~OPT_LINE);
    ioctl (0, FIOSETOPTIONS, ioctl (0, FIOGETOPTIONS, 0) & ~OPT_ECHO);
}

#else /* VxWorks */

#error "neither termios.h/termio.h nor sgtty.h available"

#endif /* VxWorks */

#endif /* tty hacking, two major styles */

/*-
 * 2 Getting information about the terminal
 *
 * The next problem is to get information about the connected terminal.
 * UNIX employs a database file called termcap or (in newer versions)
 * terminfo, where informations on the actually used terminal are
 * stored. Before we begin we must retrieve these information from the
 * data base. Three versions of the retrieval:
 *
 *	- don't read databases but assume vt100
 *	- use termcap or the termcap emulation in curses
 *	- use terminfo
 */

/* If nothing is decided yet, make a guess what should work.
   Note that you can override this with -DUSE_TERMCAP even if no
   termcap.h is avavailable in your system. Just in case the terminfo
   calls work worse than the termcap emulation inside curses. */

#if !defined ASSUME_VT100 && !defined USE_TERMCAP && !defined USE_TERMINFO
# if defined PFE_HAVE_TERMCAP_H
#   define USE_TERMCAP
# elif defined VxWorks
#   define ASSUME_DUMBTERM
# else
#   if defined PFE_HAVE_CURSES_H && defined PFE_HAVE_TERM_H
#     define USE_TERMINFO
#   else
#     define ASSUME_VT100
#   endif
# endif
#endif

#if defined ASSUME_VT100 || defined USE_TERMCAP || defined ASSUME_DUMBTERM

/*
 * We make termcap look like terminfo. The following identifiers stand
 * for capabilities and are declared in term.h when using terminfo.
 * Otherwise they are used as indices to vectors containing capability
 * names or escape sequences. In the latter case they must be declared
 * in the same order as the initializers to the vectors of capability
 * names below.
 */
enum
{
    cursor_address,
    cursor_home,
    cursor_left,
    cursor_right,
    cursor_up,
    cursor_down,
    clear_screen,
    clr_eos,
    clr_eol,
    bell,
    delete_character,
    delete_line,
    scroll_forward,
    scroll_reverse,
    enter_standout_mode,
    exit_standout_mode,
    enter_underline_mode,
    exit_underline_mode,
    enter_bold_mode,
    enter_reverse_mode,
    enter_blink_mode,
    exit_attribute_mode,
    keypad_xmit,
    keypad_local
};

#endif

/*
 * Strings used to query the termcap database.
 *
 * (In case terminfo is used, these are only needed to satisfy the
 * show_*_strings functions.)
 */

#ifdef USE_TERMCAP
static char tckeycode[][3] =
{
    "k1", "k2", "k3", "k4", "k5", /* keys in same order as enum keycode */
    "k6", "k7", "k8", "k9", "k0", /* from term.h */
    "F1", "F2", "F3", "F4", "F5",
    "F6", "F7", "F8", "F9", "FA",
    "kl", "kr", "ku", "kd",	/* result is just what has to be exported */
    "kh", "kH", "kN", "kP",	/* via variable rawkey_string */
    "kb", "kD", "kM", "kI",
    "kA", "kE", "kL", "kC"
};
#endif /* USE_TERMCAP */

#if defined USE_TERMCAP || defined USE_TERMINFO
static char tcctlcode[][3] =
{
    "cm", "ho",
    "le", "nd", "up", "do",
    "cl", "cd", "ce", "bl",
    "dc", "dl",
    "sf", "sr",
    "so", "se", "us", "ue",
    "md", "mr", "mb", "me",
    "ks", "ke"
};
#endif /* USE_TERMCAP || USE_TERMINFO */

#if defined ASSUME_DUMBTERM || defined ASSUME_VT100

/*
 * terminal is 
 *   a) dumb/unknown
 *   b) vt100 like
 *   c) hopefully one of them
 */
 
extern char const * p4_vt100_controls[];
extern char const * p4_vt100_rawkeys[];


#define query_database() 1	/* nothing to query */
#define t_putchar(C) putchar (C)
#define tputs(S,P,F) fputs (S, stdout)

static void
c_tputs (int cap, int n)
{
    fputs (PFE.control_string[cap], stdout);
}

static char *
tparm (int cap, int x, int y)	/* call this only with `cm' */
{
  static char buf[12] = {0}; /*not reentrant */
  if (PFE.control_string == p4_vt100_controls) 
     sprintf (buf, "\033[%d;%dH", x + 1, y + 1);
  return buf;
}

#ifdef __GNUC__
#define INTO(x) .x =
#else
#define INTO(x)
#endif

#ifdef ASSUME_DUMBTERM
p4_term_struct p4_term_ios =
#else
p4_term_struct p4_term_dumbterm =
#endif
{
    "dumbterm",
    p4_dumbterm_controls,
    p4_dumbterm_rawkeys,
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
    INTO(c_wherexy)	c_wherexy

};

#ifdef ASSUME_VT100
p4_term_struct p4_term_ios =
#else
p4_term_struct p4_term_vt100 =
#endif
{
    "vt100",
    p4_vt100_controls,
    p4_vt100_rawkeys,
    INTO(init)			c_prepare_terminal, 
    INTO(fini)			c_cleanup_terminal,
    INTO(tput)			c_tput,
    
    INTO(tty_interrupt_key)	c_interrupt_key,
    INTO(interactive_terminal)	c_interactive_terminal,
    INTO(system_terminal)	c_system_terminal,
    INTO(query_winsize)		c_query_winsize,
    
    INTO(c_keypressed)		c_keypressed,
    INTO(c_getkey)		c_getkey,
    INTO(c_putc_noflush)	c_putc_noflush,
    INTO(c_put_flush)		c_put_flush,
    INTO(c_putc)		c_putc,
    INTO(c_puts)		c_puts,
    INTO(c_gotoxy)		c_gotoxy,
    INTO(c_wherexy)		c_wherexy

};

#elif defined USE_TERMCAP

/*
 * 2.2 Use termcap calls to retrieve information about the terminal
 */

#ifdef PFE_HAVE_TERMCAP_H
#include <termcap.h>
#else

/* Systems emulating the termcap functions in the curses library
 * usually haven't declared them in a file <termcap.h>, so here are
 * these declarations: */
#ifdef __cplusplus
extern "C"
{
#endif
    int tgetent (char *, char *);
    int tgetnum (char *);
    int tgetflag (char *);
    char *tgetstr (char *, char **);
    int tputs (char *, int, int (*)(int));
    char *tgoto (char *, int, int);
#ifdef __cplusplus
}
#endif

#endif

/* The termcap function have been invented before "const" came into
 * usage. Almost no system declares them with "const char*" in places
 * where they are apropriate. Since PFE is usually compiled with
 * non-writable strings, this gives us a series of warnings we do
 * not really want to see. Hence, here we re-#define them sending
 * the const string literals as plain "char*" to the real functions.
 */
#define _tgetstr(A,B) tgetstr((char*)(A),(B))
#define _tgetnum(A)   tgetnum((char*)(A))

#define __using_termcap 1

static char const * termcap_control_string[DIM (tcctlcode)];
static char const * termcap_rawkey_string[P4_NUM_KEYS];

/*
 * Get the strings needed out of termcap/terminfo.
 */
static int
query_database (void)
{
    char *ttype = getenv ("TERM");
    char tcent[2048];
    static char tcbuf[2048];
    static char erase[] = "\b";
    char *tctop = tcbuf;
    int i;
    
    if (ttype == NULL || tgetent (tcent, ttype) <= 0)
	return 0;
    
    PFE.term->name = ttype;

#  ifdef PFE_HAVE_TERMCAP_H
    {
	char *pc = _tgetstr ("pc", &tctop);
	if (pc != NULL)
	    PC = *pc;
	else
	    PC = 0;
    }
#  endif

    PFE.rows = _tgetnum ("li");
    PFE.cols = _tgetnum ("co");

    /* Read all termcap strings we need, */
    for (i = 0; i < P4_NUM_KEYS; i++)
	PFE.rawkey_string[i] = _tgetstr (tckeycode[i], &tctop);
    
    /* another chance for F10: */
    if (PFE.rawkey_string [P4_KEY_k0 - P4_KEY_k1] == NULL)
	PFE.rawkey_string [P4_KEY_k0 - P4_KEY_k1] = _tgetstr ("k;", &tctop);
    /* better believe stty which is the erase character: */
    if (tty_erasechar)
    {
	erase[0] = tty_erasechar;
	PFE.rawkey_string [P4_KEY_kb - P4_KEY_k1] = erase;
    }
    for (i = 0; i < DIM (tcctlcode); i++)
	PFE.control_string[i] = _tgetstr (tcctlcode[i], &tctop);
    if (PFE.control_string [cursor_left] == NULL)
	PFE.control_string [cursor_left] = "\b";

#  if defined PFE_HAVE_OSPEED
    {
	char *pc = _tgetstr ("pc", &tctop);
	/* these are defined inside the termcap-library: */
	ospeed = tty_ospeed;
	PC = pc ? *pc : 0;
	BC = PFE.control_string[cursor_left];
	UP = PFE.control_string[cursor_up];
    }
#  endif
    return 1;
}

static int			/* utility function for tputs(): */
t_putchar (int c)		/* putchar() is a macro, we need a function */
{
    return putchar (c);
}

static void
c_tputs (int tcidx, int n)	/* issue termcap string to terminal */
{
    if (!PFE.control_string[tcidx])
	/* no harm if feature not available */
	return;
    tputs (PFE.control_string[tcidx], n, t_putchar_);
    fflush (stdout);
}

#define tparm(CAP,X,Y) tgoto (PFE.control_string [CAP], Y, X)

#ifdef __GNUC__
#define INTO(x) .x =
#else
#define INTO(x)
#endif

p4_term_struct p4_term_ios =
{
    "termcap",
    termcap_control_string,
    termcap_rawkey_string,
    INTO(init)			c_prepare_terminal, 
    INTO(fini)			c_cleanup_terminal,
    INTO(tput)			c_tput,

    INTO(tty_interrupt_key)	c_interrupt_key,
    INTO(interactive_terminal)	c_interactive_terminal,
    INTO(system_terminal)	c_system_terminal,
    INTO(query_winsize)		c_query_winsize,

    INTO(c_keypressed)		c_keypressed,
    INTO(c_getkey)		c_getkey,
    INTO(c_putc_noflush)	c_putc_noflush,
    INTO(c_put_flush)		c_put_flush,
    INTO(c_putc)		c_putc,
    INTO(c_puts)		c_puts,
    INTO(c_gotoxy)		c_gotoxy,
    INTO(c_wherexy)		c_wherexy

};

#elif defined USE_TERMINFO

/*
 * 2.3 Use terminfo calls to retrieve information about the terminal
 */

#if SunOS || defined __target_os_solaris
#undef TRUE			/* These confuse Sun's cc in curses.h */
#undef FALSE
#include <sys/termio.h>		/* Solaris' curses.h seems to need this */
#elif defined HAVE_TERMIO_H
#include <termio.h>             /* AIX 4.1 needs this */
#endif
#include <curses.h>
#include <term.h>

#define __using_terminfo

static char const * terminfo_control_string[DIM (tcctlcode)];
static char const * terminfo_rawkey_string[P4_NUM_KEYS];

static int
query_database (void)
{
    int errret;
    static char erase[] = "\b";

    setupterm (NULL, STDOUT_FILENO, &errret);
    if (errret != 1)
	return 0;

#  define KINIT(key, ti_capability) \
    PFE.rawkey_string [P4CAT (P4_KEY_,key) - P4_KEY_k1] = ti_capability

    KINIT (k1, key_f1);		/* function keys F1 through F10 */
    KINIT (k2, key_f2);
    KINIT (k3, key_f3);
    KINIT (k4, key_f4);
    KINIT (k5, key_f5);
    KINIT (k6, key_f6);
    KINIT (k7, key_f7);
    KINIT (k8, key_f8);
    KINIT (k9, key_f9);
    KINIT (k0, key_f10);

    KINIT (kl, key_left);	/* arrow key left */
    KINIT (kr, key_right);	/* arrow key right */
    KINIT (ku, key_up);		/* arrow key up */
    KINIT (kd, key_down);	/* arrow key down */

    KINIT (kh, key_home);	/* home key */
#  ifdef key_end
    KINIT (kH, key_end);	/* home down key */
#  else /* on HPUX9 */
    KINIT (kH, key_ll);         /* the same, 'lower left' */
#  endif
    KINIT (kN, key_npage);	/* next page key */
    KINIT (kP, key_ppage);	/* previous page key */
    
    KINIT (kb, key_backspace);	/* backspace key */
    KINIT (kD, key_dc);		/* delete character key */
    KINIT (kM, key_eic);	/* exit insert mode key */
    KINIT (kI, key_ic);		/* insert character key */
    
    KINIT (kA, key_il);		/* insert line key */
    KINIT (kE, key_eol);	/* clear to end of line key */
    KINIT (kL, key_dl);		/* delete line key */
    KINIT (kC, key_clear);	/* clear screen key */

#  undef KINIT
    /* better believe stty which is the erase character: */
    if (tty_erasechar)
    {
	erase[0] = tty_erasechar;
	PFE.rawkey_string [P4_KEY_kb - P4_KEY_k1] = erase;
    }

    if (cursor_left == NULL)
	cursor_left = "\b";     /* is it safe? w/o a strdup ? */
    return 1;
}

static int		/* utility function for tputs(): */
t_putchar (int c)	/* putchar() is a macro, we need a function */
{
    return putchar (c);
}

static void		/* output capability with affected lines cnt */
c_tputs (char *s, int n)
{
    tputs (s, n, t_putchar_);
    fflush (stdout);
}

#ifdef __GNUC__
#define INTO(x) .x =
#else
#define INTO(x)
#endif

p4_term_struct p4_term_ios =
{
    "terminfo",
    terminfo_control_string,
    terminfo_rawkey_string,
    INTO(init)			c_prepare_terminal, 
    INTO(fini)			c_cleanup_terminal,
    INTO(tput)			c_tput,

    INTO(tty_interrupt_key)	c_interrupt_key,
    INTO(interactive_terminal)	c_interactive_terminal,
    INTO(system_terminal)	c_system_terminal,
    INTO(query_winsize)		c_query_winsize,

    INTO(c_keypressed)		c_keypressed,
    INTO(c_getkey)		c_getkey,
    INTO(c_putc_noflush)	c_putc_noflush,
    INTO(c_put_flush)		c_put_flush,
    INTO(c_putc)		c_putc,
    INTO(c_puts)		c_puts,
    INTO(c_gotoxy)		c_gotoxy,
    INTO(c_wherexy)		c_wherexy
};

#else

#error "One of ASSUME_VT100, USE_TERMCAP or USE_TERMINFO must be set."

#endif

static int				/* Prepares usage of all other functions. */
c_prepare_terminal (void)		/* Call only once at startup. */
{
    int fd;
  
#  if defined ASSUME_DUMBTERM
    PFE.control_string = p4_dumbterm_controls;
    PFE.rawkey_string  = p4_dumbterm_rawkeys;
#  elif defined ASSUME_VT100
    PFE.control_string = p4_vt100_controls;
    PFE.rawkey_string  = p4_vt100_rawkeys;
#  endif
  

    /* save state before all changes */
    for (fd = 0; fd < 3; fd++)
        saved_fcntl[fd] = P4_fcntl_F_GETFL (fd, 0); /* fcntl(fd, F_GETFL, 0) */
    tty_save ();

    /* initialize termcap-stuff */
    return query_database ();
}

static void
c_cleanup_terminal (void)
{
    return; /* nothing to do here */
}

static void			/* set terminal to the sort of */
c_interactive_terminal (void)	/* `cbreak' mode used by p4th */
{
    tty_interactive ();
    c_tputs (keypad_xmit, 0);
}

static void			/* resets terminal state to */
c_system_terminal (void)		/* as it was before */
{
    int fd;

    if (! isatty (STDIN_FILENO)) return;

    fflush (stdout);
    tty_restore ();
    c_tputs (keypad_local, 0);
    for (fd = 0; fd < 3; fd++)
        P4_fcntl_F_SETFL (fd, saved_fcntl[fd]); /* fcntl (fd, F_SETFL, ..) */
}

/*
 * Handle window size change, see also signal.c:
 */
#ifdef TIOCGWINSZ
static void
c_query_winsize (void)
{
    struct winsize size;

    if (ioctl (1, TIOCGWINSZ, (char *) &size) >= 0)
    {
	PFE.rows = size.ws_row;
	PFE.cols = size.ws_col;
	PFE.xmax = size.ws_xpixel;
	PFE.ymax = size.ws_ypixel;
    }
}
#else
static void
c_query_winsize (void)
{
}
#endif

/************************************************************************/
/* Input from keyboard.                                                 */
/************************************************************************/

/*
 * Having hacked the terminal driver and the standard input in the
 * appropiate modes all we have to do is read one character from
 * standard input to get a key.
 * The only problem is: we can't detect if a key is available without
 * reading it. So we have to store it in this case.
 */

#define NOCH ((unsigned short)0xABCD)
				/* encodes 'no character available' */

static unsigned short		/* the next character when read by */
  nxch = NOCH;			/* keypressed() */

static int
nextch (void)
/*
 * Read a single character from standard input.
 */
{
    unsigned char c;

    if (nxch != NOCH)
    {
	c = (unsigned char) nxch;
	nxch = NOCH;
	return c;
    }
    else
    {
	for (;;)
	{
	    switch (read (0, (void *) &c, 1))
	    {
	    case -1:
		switch (errno)
		{
		default:
		    return -1;
		case EAGAIN:
		case EINTR:
		    continue;
		}
	    case 0:
		return -1;
	    default:
		return c;
	    }
	}
    }
}

static int
c_keypressed (void)
/*
 * Checks if a character is available in the standard input. Saves it in nxch.
 * Returns: 1 if char available, 0 otherwise
 */
{
    unsigned char c;		/* place to read the character */
    int result;

    fflush (stdout);
    if (nxch != NOCH)
	return 1;		/* char from previos keypressed() */
    no_waitchar (STDIN_FILENO);
    result = read (0, (void *) &c, 1);
    waitchar (STDIN_FILENO);
    if (result != 1)
	return 0;
    nxch = c;
    return 1;
}

static int
c_getkey (void)
{
    fflush (stdout);

    if (PFE.wait_for_stdin)
	PFE.wait_for_stdin ();

    return nextch ();
}

/************************************************************************/
/* Output to screen, control with termcap:                              */
/************************************************************************/

static int row, col;		/* position of curser as tracked */

static void			/* output character and */
c_putc_noflush (char c)		/* trace the cursor position */
{
    putchar (c);
    switch (c)
    {
    case '\a':			/* bell, no change of cursor position */
	break;
    case '\b':			/* backspace, move cursor left */
	if (col > 0)
	    col--;
	break;
    case '\r':			/* carriage return, ->column 0 */
	col = 0;
	break;
    default:			/* ordinary character: */
	if (col < PFE.cols - 1)	/* at right edge of screen? */
	{
	    col++;		/* no: increment column */
	    break;
	}			/* yes: like line feed */
    case '\n':			/* line feed */
	col = 0;
	if (row < PFE.rows - 1)	/* if not at bottom of screen: */
	    row++;		/* increment row */
    }				/* otherwise terminal is supposed to scroll */
}

static void
c_put_flush (void)
{
    fflush (stdout);
}

static void
c_putc (char c)
{
    c_putc_noflush (c);
    fflush (stdout);
}

static void
c_puts (const char *s)
{
    while (*s)
	c_putc_noflush (*s++);
    fflush (stdout);
}

static void
c_gotoxy (int x, int y)
{
    tputs (tparm (cursor_address, y, x), 1, t_putchar_);
    fflush (stdout);
    col = x;
    row = y;
}

static void
c_wherexy (int *x, int *y)
{
    *x = col;
    *y = row;
}

static void 
c_tput (int attr)
{
    switch (attr)
    {
    case P4_TERM_GOLEFT:	c_tputs (cursor_left,  	0); --col; break;
    case P4_TERM_GORIGHT:	c_tputs (cursor_right, 	0); ++col; break;
    case P4_TERM_GOUP:		c_tputs (cursor_up,	0); --row; break;
    case P4_TERM_GODOWN:	c_tputs (cursor_down,	0); ++row; break;
	
    case P4_TERM_CLRSCR:	c_tputs (clear_screen, PFE.rows); /*->HOME*/
    case P4_TERM_HOME:		c_tputs (cursor_home, 1); row = col = 0;  break;
    case P4_TERM_CLREOL:	c_tputs (clr_eol, 1); 		   break;
    case P4_TERM_CLRDOWN:	c_tputs (clr_eos, PFE.rows - row);  break;
    case P4_TERM_BELL:		c_tputs (bell, 0); 		   break;
	
    case P4_TERM_NORMAL:	c_tputs (exit_attribute_mode,	0); break;
    case P4_TERM_BOLD_ON:	c_tputs (enter_standout_mode,	0); break;
    case P4_TERM_BOLD_OFF:	c_tputs (exit_standout_mode,	0); break;
    case P4_TERM_UNDERLINE_ON:	c_tputs (enter_underline_mode,	0); break;
    case P4_TERM_UNDERLINE_OFF:	c_tputs (exit_underline_mode,	0); break;
    case P4_TERM_BRIGHT:	c_tputs (enter_bold_mode,	0); break;
    case P4_TERM_REVERSE:	c_tputs (enter_reverse_mode,	0); break;
    case P4_TERM_BLINKING:	c_tputs (enter_blink_mode,	0); break;
    default: 
	break;
    }
}

 /*@}*/
 
