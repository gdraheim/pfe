/** 
 * -- terminal driver for win32 wincon.h API 
 *
 *  Copyright (C) Guido Draheim, 2001
 *
 *  @see     GNU LGPL
 *  @author  Guido Draheim            @(#) %derived_by: guidod %
 *  @version %version: 1.15 %
 *    (%date_modified: Mon Mar 12 11:03:00 2001 %)
 *
 *  @description
 *              	Terminal driver for win32 wincon.h API as
 *			provided with the mingw32 compilers.
 * 
 *  http://msdn.microsoft.com/library/psdk/winbase/conchar_8wfi.htm
 *  this microsoft doc says that wincon.h API is supported since
 *  NT-3.1 and WIN-95 (and later versions).
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: term-wincon.c,v 0.31 2001-03-19 22:21:06 guidod Exp $";
#endif

/*
 * html-references in this driver section point to
 * http://msdn.microsoft.com/library/...
 * (driver created in March 2001)
 * you should use a highlighting editor to read this text as
 * there are far too many comments around.
 */

#include <pfe/incl-sub.h>
#include <pfe/term-sub.h>
/* include <wincon.h> */
#include <windows.h> 
#include <stdlib.h>
#include <ctype.h>

#include <pfe/def-types.h>
#include <pfe/logging.h>

#define pfeTerm ((p4_wincon_term*)(PFE.priv))

#ifndef WINCON_NOECHO               /* USER-CONFIG */
#define WINCON_NOECHO 1
#endif

#ifndef WINCON_KEYDOWN              /* USER-CONFIG */
#define WINCON_KEYDOWN 1
#endif

typedef struct p4_wincon_term_
{
    /* names like in psdk/winbase/conchar_156b.htm */
    HANDLE hStdout;
    HANDLE hStdin;
    WORD   wColor;
    WORD   wOldColorAttrs;
    DWORD  fdwOldMode;
    CHAR   AsciiChar;
} p4_wincon_term;

static int c_interrupt_key (char ch)		{ return 0; }

static void c_query_winsize (void)		
{
    /* psdk/winbase/conchar_34dr.htm */
    CONSOLE_SCREEN_BUFFER_INFO screenInfo;
    if (! pfeTerm || pfeTerm->hStdout == INVALID_HANDLE_VALUE)
	return;
    
    if (GetConsoleScreenBufferInfo (pfeTerm->hStdout, &screenInfo))
    {
	PFE.cols = screenInfo.dwSize.X;
	PFE.rows = screenInfo.dwSize.Y;
	pfeTerm->wColor = screenInfo.wAttributes;
    }else{
	/* using defaults */
	PFE.cols = PFE.set->cols;
	PFE.rows = PFE.set->rows;
	pfeTerm->wColor = 0x7; /* white on black */
    }
}

static int
c_prepare_terminal (void)
{
    P4_enter ("now");
    if (!pfeTerm)
    {
	pfeTerm = calloc (1, sizeof(*pfeTerm));
	if (!pfeTerm) return 0;
	pfeTerm->hStdout = INVALID_HANDLE_VALUE;
	pfeTerm->hStdin = INVALID_HANDLE_VALUE;
    }

    if (pfeTerm->hStdout == INVALID_HANDLE_VALUE)
    {
	/* should we check for --bye ? is this an implicit check     *checkme*
	 * for a kind of terminal, i.e. isatty on stdio/stdin?       *checkme* 
	 */
	pfeTerm->hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	pfeTerm->hStdin  = GetStdHandle(STD_INPUT_HANDLE);
	if (pfeTerm->hStdout == INVALID_HANDLE_VALUE ||
	    pfeTerm->hStdin  == INVALID_HANDLE_VALUE)
	{
	    P4_warn1 ("no default console window found: %ld", 
		      GetLastError());
	    pfeTerm->hStdout = pfeTerm->hStdin = INVALID_HANDLE_VALUE;
	}
    }
    
    if (pfeTerm->hStdout == INVALID_HANDLE_VALUE)
    {
	/* psdk/winbase/conchar_93n6.htm */
	pfeTerm->hStdout = CreateConsoleScreenBuffer(
	    GENERIC_READ|GENERIC_WRITE, /* access */
	    0, /* buffer share mode */
	    0, /* lpSecurityAttributes */
	    CONSOLE_TEXTMODE_BUFFER, /* buffer type (the only one possible) */
	    0 /* reserved */
	    );
	if (pfeTerm->hStdout == INVALID_HANDLE_VALUE)
	{
	    P4_fail1 ("could not open console window: %ld", 
		      GetLastError());
	    return 0;
	}
	/* psdk/winbase/conchar_9hrm.htm */
	if (! SetConsoleActiveScreenBuffer(pfeTerm->hStdout))
	{
	    P4_fail1 ("could not activate console window: %ld", 
		      GetLastError());
	    return 0;
	}
	pfeTerm->hStdin = pfeTerm->hStdout;
    }

    if (! GetConsoleMode (pfeTerm->hStdin, &pfeTerm->fdwOldMode))
    {
	P4_warn1 ("can not retrieve console window mode, guess default %ld",
		 GetLastError());
	pfeTerm->fdwOldMode = 
	    ENABLE_LINE_INPUT | 
	    ENABLE_ECHO_INPUT |
	    ENABLE_PROCESSED_INPUT ;
    }

    c_query_winsize ();
    pfeTerm->wOldColorAttrs = pfeTerm->wColor;

    P4_leave1 ("hStdout=%p", pfeTerm->hStdout);
    return 1;
}

static void
c_cleanup_terminal (void)
{
    if (! pfeTerm ) return;

    SetConsoleMode (pfeTerm->hStdin, pfeTerm->fdwOldMode);
    SetConsoleTextAttribute (pfeTerm->hStdout, pfeTerm->wOldColorAttrs);

    free (pfeTerm); pfeTerm = 0;
    return;
}

static void c_interactive_terminal (void)	
{
# if WINCON_NOECHO
    P4_enter ("now");

    if (! pfeTerm ) return;
    SetConsoleMode (pfeTerm->hStdin, 
		    pfeTerm->fdwOldMode & 
		    ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT));
    P4_leave ("now");
# endif
}
static void c_system_terminal (void)		
{
# if WINCON_NOECHO
    P4_enter ("now");
    if (! pfeTerm ) return;
    SetConsoleMode (pfeTerm->hStdin, pfeTerm->fdwOldMode);
    P4_leave ("now");
# endif
}

static int KeyEventProc (KEY_EVENT_RECORD* ev)
{
    int k;

    if (! pfeTerm ) 
	return 0;

# if WINCON_KEYDOWN
    if (! ev->bKeyDown)
	return 0;
# else
    if (ev->bKeyDown)
	return 0;
# endif

    if (ev->uChar.AsciiChar)
    {
	pfeTerm->AsciiChar = 0;
	return ev->uChar.AsciiChar;
    }

    k = ev->dwControlKeyState & MOD_SHIFT ? (P4_KEY_k1 - P4_KEY_F1) : 0;

    switch (ev->wVirtualKeyCode)
    {
    case VK_F1:  return P4_KEY_F1 + k;
    case VK_F2:  return P4_KEY_F2 + k;
    case VK_F3:  return P4_KEY_F3 + k;
    case VK_F4:  return P4_KEY_F4 + k;
    case VK_F5:  return P4_KEY_F5 + k;
    case VK_F6:  return P4_KEY_F6 + k;
    case VK_F7:  return P4_KEY_F7 + k;
    case VK_F8:  return P4_KEY_F8 + k;
    case VK_F9:  return P4_KEY_F9 + k;
    case VK_F10: return P4_KEY_FA + k;

    case VK_LEFT:  return P4_KEY_kl;
    case VK_UP:    return P4_KEY_ku;
    case VK_RIGHT: return P4_KEY_kr;
    case VK_DOWN:  return P4_KEY_kd;

    case VK_HOME:  return P4_KEY_kh;
    case VK_END:   return P4_KEY_kH;
    case VK_NEXT:  return P4_KEY_kN;
    case VK_PRIOR: return P4_KEY_kP;

    case VK_BACK:   return '\b';
    case VK_DELETE: return P4_KEY_kD;
    case VK_INSERT: return P4_KEY_kI;
    case VK_EREOF:  return P4_KEY_kE;
    case VK_CLEAR:  
    case VK_CANCEL: return P4_KEY_kC;
    case VK_TAB:    return '\t';
    case VK_RETURN: return '\n';
    default:
	return 0;
    }
}

static int			
c_getvkey (void)
{
    /* psdk/winbase/conchar_2nw3.htm */
    INPUT_RECORD irInBuf[1];
    DWORD cNumRead;

    if (pfeTerm->AsciiChar)
    {
	int c = pfeTerm->AsciiChar;
	pfeTerm->AsciiChar = 0;
	return c;
    }

    while (1)
    {
	if (! ReadConsoleInput(
	    pfeTerm->hStdin,
	    irInBuf,
	    1,
	    &cNumRead))
	{
	    P4_warn ("ReadConsoleInput Failed");
	    return 0;
	}

	if (! cNumRead) continue;

	switch (irInBuf[0].EventType)
	{
	case KEY_EVENT:
	    { 
		return KeyEventProc (&irInBuf[0].Event.KeyEvent);
	    }
	case WINDOW_BUFFER_SIZE_EVENT:
	    PFE.cols = irInBuf[0].Event.WindowBufferSizeEvent.dwSize.X;
	    PFE.rows = irInBuf[0].Event.WindowBufferSizeEvent.dwSize.Y;
	    break;
	case MOUSE_EVENT:
	    /* ignore */
	case FOCUS_EVENT:
		/* ignore */
	default:
	    /* ignore */
	    break;
	}
    }
}

int c_getkey ()
{
    register int vkey = c_getvkey ();
    if (vkey > 0x100) return 0;
    else return vkey;
}

static int c_keypressed (void)
{
    /* psdk/winbase/conchar_2nw3.htm */
    INPUT_RECORD irInBuf[1];
    DWORD cNumRead;

    if (pfeTerm->AsciiChar)
    {
	int c = pfeTerm->AsciiChar;
	pfeTerm->AsciiChar = 0;
	return c;
    }

    while (1)
    {
	/* peek the next event */

	if (! PeekConsoleInput(
	    pfeTerm->hStdin,
	    irInBuf,
	    1,
	    &cNumRead))
	{
	    P4_warn ("PeekConsoleInput Failed");
	    return 0;
	}

	if (! cNumRead) 
	    return 0;

# if WINCON_KEYDOWN
	if (irInBuf[0].EventType == KEY_EVENT && 
	    irInBuf[0].Event.KeyEvent.bKeyDown)
	    return 1;
# else
	if (irInBuf[0].EventType == KEY_EVENT && 
	    ! irInBuf[0].Event.KeyEvent.bKeyDown)
	    return 1;
# endif

	/* process other event types */

	if (! ReadConsoleInput(
	    pfeTerm->hStdin,
	    irInBuf,
	    1,
	    &cNumRead))
	{
	    P4_warn ("ReadConsoleInput Failed");
	    return 0;
	}

	switch (irInBuf[0].EventType)
	{
	case WINDOW_BUFFER_SIZE_EVENT:
	    PFE.cols = irInBuf[0].Event.WindowBufferSizeEvent.dwSize.X;
	    PFE.rows = irInBuf[0].Event.WindowBufferSizeEvent.dwSize.Y;
	    break;
	case KEY_EVENT:
	    /* ignore */
	case MOUSE_EVENT:
	    /* ignore */
	case FOCUS_EVENT:
	    /* ignore */
	default:
	    /* ignore */
	    break;
	}
    }
}

static void
c_putc_noflush (char c)
{
    DWORD ignore;
    if (c != '\n')
    {
	WriteConsole (pfeTerm->hStdout, &c, 1, &ignore, 0);
    }else{
	/* psdk/winbase/conchar_156b.htm */
	CONSOLE_SCREEN_BUFFER_INFO screenInfo;
	if (! GetConsoleScreenBufferInfo(pfeTerm->hStdout, &screenInfo)) 
	    goto failed;
	screenInfo.dwCursorPosition.X = 0; 

	/* If it is the last line in the screen buffer, 
	 * scroll the buffer up. 
	 */

	if ((screenInfo.dwSize.Y-1) == screenInfo.dwCursorPosition.Y) 
	{ 
	    /* ScrollScreenBuffer(pfeTerm->hStdout, 1); */

	    SMALL_RECT srRect;
	    CHAR_INFO fill;
	    COORD coord = { 0 , 0 };
	    fill.Attributes = pfeTerm->wColor;
	    fill.Char.AsciiChar = ' ';
	    memcpy (&srRect, &screenInfo.srWindow, sizeof(srRect));
	    srRect.Top++;

	    ScrollConsoleScreenBuffer(
		pfeTerm->hStdout,
		&srRect,
		0,
		coord,
		&fill);
	} 

	/* Otherwise, advance the cursor to the next line. */

	else screenInfo.dwCursorPosition.Y += 1; 
 
	if (! SetConsoleCursorPosition(
	    pfeTerm->hStdout, 
	    screenInfo.dwCursorPosition))
	    goto failed;
	return;
    failed:
	WriteConsole (pfeTerm->hStdout, &c, 1, &ignore, 0);
    }
}

static void
c_put_flush (void)
{
    return;
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
    /* optimize path */
    DWORD n = 0;
    while (s[n] && isprint(s[n]) && s[n] != '\n')
	n++;

    if (n)
    {
	WriteConsole (pfeTerm->hStdout, s, n, &n, 0);
	s += n;
    }

    /* standard path */
    while (*s)
        c_putc_noflush (*s++);
    c_put_flush ();
}

static void
c_gotoxy (int x, int y)
{
    CONSOLE_SCREEN_BUFFER_INFO screenInfo;
    if (! GetConsoleScreenBufferInfo(pfeTerm->hStdout, &screenInfo)) 
	return;

    screenInfo.dwCursorPosition.X = x; 
    screenInfo.dwCursorPosition.Y = y; 

    if (! SetConsoleCursorPosition(
	pfeTerm->hStdout, 
	screenInfo.dwCursorPosition))
	return;
    return;
}

static void
c_wherexy (int *x, int *y)
{
    CONSOLE_SCREEN_BUFFER_INFO screenInfo;
    if (! GetConsoleScreenBufferInfo(pfeTerm->hStdout, &screenInfo)) 
    {
	*x = *y = 0;			/* uargh! */
	P4_warn ("could not get cursor position");
    }else{
	*x = screenInfo.dwCursorPosition.X; 
	*y = screenInfo.dwCursorPosition.Y; 
    }
}

static void			/* move cursor in x and y */
addxy (int dx, int dy)
{
    CONSOLE_SCREEN_BUFFER_INFO screenInfo;
    if (! GetConsoleScreenBufferInfo(pfeTerm->hStdout, &screenInfo)) 
	return;
    
    screenInfo.dwCursorPosition.X += dx; 
    screenInfo.dwCursorPosition.Y += dy; 
    
    SetConsoleCursorPosition(
	pfeTerm->hStdout, 
	screenInfo.dwCursorPosition);
    
    p4_OUT = screenInfo.dwCursorPosition.X;
}

static void
c_clreol (void)
{
    /*FIXME: does not work */
    DWORD ignore;
    CONSOLE_SCREEN_BUFFER_INFO screenInfo;

    if (! GetConsoleScreenBufferInfo(pfeTerm->hStdout, &screenInfo)) 
	return;
    FillConsoleOutputCharacter (
	pfeTerm->hStdout,
	' ',
	screenInfo.dwSize.X - screenInfo.dwCursorPosition.X, 
	screenInfo.dwCursorPosition,
	&ignore);
    FillConsoleOutputAttribute (
	pfeTerm->hStdout,
	pfeTerm->wColor,
	screenInfo.dwSize.X - screenInfo.dwCursorPosition.X, 
	screenInfo.dwCursorPosition,
	&ignore);
}

static void
c_clrscr (void)
{
    DWORD ignore;
    CONSOLE_SCREEN_BUFFER_INFO screenInfo;
    if (! GetConsoleScreenBufferInfo(pfeTerm->hStdout, &screenInfo)) 
	return;
    c_gotoxy (0, 0);
    FillConsoleOutputCharacter (
	pfeTerm->hStdout,
	' ',
	screenInfo.dwSize.X*screenInfo.dwSize.Y, 
	screenInfo.dwCursorPosition,
	&ignore);
    FillConsoleOutputAttribute (
	pfeTerm->hStdout,
	pfeTerm->wColor,
	screenInfo.dwSize.X*screenInfo.dwSize.Y, 
	screenInfo.dwCursorPosition,
	&ignore);
}

static void
c_clrdown (void)
{
    int x, y, i;
    
    c_clreol ();
    c_wherexy (&x, &y);
    for (i = y + 1; i < PFE.rows; i++)
    {
        c_gotoxy (i, 0);
        c_clreol ();
    }
    c_gotoxy (x, y);
}

enum {
    none, bold, faint, italic, blink = 5,
    rapid_blink, reverse_video, concealed
};

static void
setattr (int attr)
{
    switch (attr)
    {
    case none:
	pfeTerm->wColor &= 0x77;
	break;
    case bold:
	pfeTerm->wColor |= FOREGROUND_INTENSITY;
	break;
    case italic:
	pfeTerm->wColor |= BACKGROUND_INTENSITY;
	break;
    default:
	/* ignore */
	return;
    }
    SetConsoleTextAttribute (pfeTerm->hStdout, pfeTerm->wColor);
}

static void
clrattr (int attr)
{
    switch (attr)
    {
    case bold:
	pfeTerm->wColor &=~ FOREGROUND_INTENSITY;
	break;
    case italic:
	pfeTerm->wColor &=~ BACKGROUND_INTENSITY;
	break;
    default:
	/* ignore */
	return;
    }
    SetConsoleTextAttribute (pfeTerm->hStdout, pfeTerm->wColor);
}

static void 
c_tput (int attr)
{
    switch (attr)
    {
     case P4_TERM_GOLEFT:		addxy (-1,  0); break;
     case P4_TERM_GORIGHT:		addxy ( 1,  0); break;
     case P4_TERM_GOUP:			addxy ( 0, -1); break;
     case P4_TERM_GODOWN:		addxy ( 0,  1); break;
         
     case P4_TERM_HOME:			c_gotoxy (0, 0); break;
     case P4_TERM_CLRSCR:		c_clrscr (); break;
     case P4_TERM_CLRDOWN:		c_clrdown (); break;
     case P4_TERM_CLREOL:		c_clreol (); break;
     case P4_TERM_BELL:			MessageBeep (MB_ICONASTERISK); break;
       
     case P4_TERM_NORMAL:		setattr (none); break;
     case P4_TERM_BOLD_ON:		setattr (bold); break;
     case P4_TERM_BOLD_OFF:		clrattr (bold); break;
     case P4_TERM_REVERSE:		setattr (reverse_video); break;
     case P4_TERM_BRIGHT:		setattr (bold); break;
     case P4_TERM_BLINKING:		setattr (blink); break;
     case P4_TERM_UNDERLINE_ON:		setattr (italic); break;
     case P4_TERM_UNDERLINE_OFF:	clrattr (italic); break;
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
    "term-wincon",
    0,
    0, /* no rawkeys -> use getvkey */
    INTO(init) 	                c_prepare_terminal, 
    INTO(fini) 	                c_cleanup_terminal,
    INTO(tput)	                c_tput,

    INTO(tty_interrupt_key)     c_interrupt_key,
    INTO(interactive_terminal)  c_interactive_terminal,
    INTO(system_terminal)       c_system_terminal,
    INTO(query_winsize)         c_query_winsize,
    
    INTO(c_keypressed)          c_keypressed,
    INTO(c_getkey)              c_getkey,
    INTO(c_putc_noflush)        c_putc_noflush,
    INTO(c_put_flush)           c_put_flush,
    INTO(c_putc)               	c_putc,
    INTO(c_puts)                c_puts,
    INTO(c_gotoxy)              c_gotoxy,
    INTO(c_wherexy)             c_wherexy,
    INTO(c_getvkey)             c_getvkey
};


/*@}*/
    
    
