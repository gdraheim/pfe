/** 
 * --   terminal i/o, system independent parts
 * 
 *  Copyright (C) Tektronix, Inc, 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 5.9 %
 *    (%date_modified: Mon Mar 12 14:01:58 2001 %)
 *
 *  @description
 *       this wordset exports words to talk to the terminal driver
 *       of forth. Depending on the active terminal driver, not
 *       all of these words are fully functional.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: term-ext.c,v 0.31 2001-03-19 21:50:41 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-xtra.h>
#include <pfe/term-sub.h>
#include <string.h>

static char tckeycode[][3] =
{
    "k1", "k2", "k3", "k4", "k5", /* keys in same order as enum keycode */
    "k6", "k7", "k8", "k9", "k0", /* from term-ext.h */
    "F1", "F2", "F3", "F4", "F5",
    "F6", "F7", "F8", "F9", "FA",
    "kl", "kr", "ku", "kd",	/* result is just what has to be exported */
    "kh", "kH", "kN", "kP",	/* via variable rawkey_string */
    "kb", "kD", "kM", "kI",
    "kA", "kE", "kL", "kC"
};

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

/** SHOW-TERM-CONTROLS ( -- ) for debugging
 * show the current mappings for the terminal output 
 * may give hints about what is wrong if the output
 * seems to miss some functionality
 */
FCode (p4_show_control_strings)
{
    if (PFE.term)
        p4_outf ("\n term control '%s'", PFE.term->name);
    else
        p4_outs ("\n term control unknown");
    
    if (PFE.control_string) 
    {
        /* for your information why screen manipulation doesn't work :-) */
        int i;
        char const * p;
        
        for (i = 0; i < DIM (tcctlcode); i++)
        {
            p4_outf ("\n\"%s\"=", tcctlcode[i]);
            if ((p = PFE.control_string[i]))
                while (*p)
                    p4_putc_printable (*p++);
            else
                p4_puts ("undefined");
        }
    } else {
        p4_outs ("\n no controls set. ");
    }
}

/** SHOW-TERM-ESC-KEYS ( -- ) for debugging
 * show the current mappings for the terminal input
 * may give hints about what is wrong if the input
 * seems to miss some functionality
 */
FCode (p4_show_rawkey_strings)
{
    int cross = 0;

    if (PFE.term)
        p4_outf ("\n term rawkeys '%s'", PFE.term->name);
    else
        p4_outs ("\n term rawkeys unknown");



    if (PFE.rawkey_string == p4_dumbterm_rawkeys)
    {
	p4_outs ("\n term has default escape sequences activated");
	cross = 1;
    }

    if (PFE.rawkey_string) 
    {
        /* for your information why function keys don't work :-) */
        int i;
        char const * p;
        
        for (i = 0; i < DIM (tckeycode); i++)
        {
	    if (!cross || !(i&3)) p4_outs("\n");
            p4_outf ("%s\"=", tckeycode[i]);
            if ((p = PFE.rawkey_string[i]))
            {
                while (*p)
                    p4_putc_printable (*p++);
		if (cross)
		    p4_emits (12 - strlen(PFE.rawkey_string[i]), ' ');
            }else{
                p4_puts (" undefined  "); /* again, 12 chars */
            }
        }
    } else {
        p4_outs ("\n no rawkeys set. ");
    }
}

/** SHOW-TERMCAP ( -- ) for debugging
 * print the termcap strings used for input and output
 * may give hints about what is wrong if the terminal
 * seems to miss some functionality
 */
FCode (p4_show_termcap)
{
    FX (p4_show_control_strings);
    FX (p4_show_rawkey_strings);
}

/* the terminal understands vt100 codes? often the correct assumption*/

char const * p4_vt100_controls[] = /* Some hardcoded vt100 sequences. */
{
    "\033[%i%d;%dH",		/* cm - cursor move */
    "\033[H",			/* ho - home position */
    
    "\b",			/* le - cursor left */
    "\033[C",			/* nd - right one column */
    "\033[A",			/* up - up one column */
    "\n",			/* do - down one column */
    
    "\033[H\033[2J",		/* cl - clear screen and home */
    "\033[J",			/* cd - clear down */
    "\033[K",			/* ce - clear to end of line */
    "\a",				/* bl - bell */
    
    "\033[P",			/* dc - delete character in line */
    "\033[M",			/* dl - delete line from screen */
    
    "\033D",			/* sf - scroll screen up */
    "\033M",			/* sr - scroll screen down */
    
    "\033[7m",			/* so - enter standout mode */
    "\033[m",			/* se - leave standout mode */
    "\033[4m",			/* us - turn on underline mode */
    "\033[m",			/* ue - turn off underline mode */
    
    "\033[1m",			/* md - enter double bright mode */
    "\033[7m",			/* mr - enter reverse video mode */
    "\033[5m",			/* mb - enter blinking mode */
    "\033[m",			/* me - turn off all appearance modes */
    
    "\033[?1h\033=",		/* ks - make function keys transmit */
    "\033[?1l\033>"		/* ke - make function keys work locally */
};

char const * p4_vt100_rawkeys[] = /* Strings sent by function keys */
{
    "\033OP",			/* k1 - function keys 1 - 4 from vt100 */
    "\033OQ",			/* k2 */
    "\033OR",			/* k3 */
    "\033OS",			/* k4 */
    "\033[15~",			/* k5 - function keys 5 - 10 from xterm */
    "\033[17~",			/* k6 */
    "\033[18~",			/* k7 */
    "\033[19~",			/* k8 */
    "\033[20~",			/* k9 */
    "\033[21~",			/* k0 */
    
    "\033[23~",			/* F1 - function keys S-F1 thru S-F10 */
    "\033[24~",			/* F2 - stem from Linux console */
    "\033[25~",			/* F3 - or whoknowswhereelse */
    "\033[26~",			/* F4 */
    "\033[28~",			/* F5 */
    "\033[29~",			/* F6 */
    "\033[31~",			/* F7 */
    "\033[32~",			/* F8 */
    "\033[33~",			/* F9 */
    "\033[34~",			/* FA */

    "\033OD",			/* kl - arrow left */
    "\033OC",			/* kr - arrow right */
    "\033OA",			/* ku - arrow up */
    "\033OB",			/* kd - arrow down */
    
    "\033[1~",			/* kh - home key */
    "\033[4~",			/* kH - home down key (end key) */
    "\033[6~",			/* kN - next page */
    "\033[5~",			/* kP - previous page */
    
    "\b",			/* kb - backspace key */
    "\033[3~",			/* kD - delete character key */
    NULL,			/* kM - exit insert mode key */
    "\033[2~",			/* kI - insert character key */
    
    NULL,			/* kA - insert line key */
    NULL,			/* kE - clear end of line key */
    NULL,			/* kL - delete line key */
    NULL,			/* kC - clear screen key */
};

/** ASSUME_VT100 ( -- )
 * load hardwired VT100-termcap into the terminal-driver
 */
FCode (p4_assume_vt100)
{
    PFE.control_string = p4_vt100_controls;
    PFE.rawkey_string = p4_vt100_rawkeys;
}

/** ASSUME_DUMBTERM ( -- )
 * load hardwired DUMBTERM-termcap into the terminal-driver
 */
FCode (p4_assume_dumbterm)
{
    PFE.control_string = p4_dumbterm_controls;
    PFE.rawkey_string = p4_dumbterm_rawkeys;
}

/* ********************************************************************** */
/* more advanced screen control                                           */
/* ********************************************************************** */

/** GOTOXY ( x y -- )
 * move the cursor to the specified position on the screen -
 * this is usually done by sending a corresponding esc-sequence
 * to the terminal. 
 */
FCode (p4_gotoxy)			
{
    p4_gotoxy (SP[1], SP[0]);
    SP += 2;
}

/** ?XY ( -- x y )
 * returns the cursor position on screen, on a real unix system
 * this includes a special call to the screen driver, in remote
 * systems this can be the expected position as seen on the
 * client side's terminal driver.
 */
FCode (p4_question_xy)		
{				
    int x, y;
    
    p4_wherexy (&x, &y);
    SP -= 2;
    SP[1] = x;
    SP[0] = y;
}

/* these are defined in term-sub.c */
/* They are renamed here to satisfy the macro CO(char *, p4code) */

#define	p4_dot_home_		p4_dot_home
#define	p4_dot_clrscr_		p4_dot_clrscr
#define	p4_dot_clreol_		p4_dot_clreol
#define p4_dot_bell_		p4_dot_bell
#define p4_dot_highlight_	p4_dot_standout_on
#define p4_dot_highlight_off_	p4_dot_standout_off
#define p4_dot_underline_	p4_dot_underline_on
#define p4_dot_underline_off_	p4_dot_underline_off
#define p4_dot_intensity_	p4_dot_bright
#define p4_dot_intensity_off_	p4_dot_normal
#define p4_dot_blink_		p4_dot_blinking
#define p4_dot_blink_off_	p4_dot_normal
#define p4_dot_reverse_		p4_dot_reverse
#define p4_dot_reverse_off_	p4_dot_normal
#define p4_dot_normal_		p4_dot_normal


P4_LISTWORDS (term) =
{
    CO ("SHOW-TERMCAP",		p4_show_termcap),
    CO ("SHOW-TERM-CONTROLS",	p4_show_control_strings),
    CO ("SHOW-TERM-ESC-KEYS",	p4_show_rawkey_strings),
    CO ("ASSUME_VT100", 	p4_assume_vt100),
    CO ("ASSUME_DUMBTERM",	p4_assume_dumbterm),
  
  /** words for more advanced screen control */
    DV ("ROWS",		        rows),
    DV ("COLS",			cols),
    DV ("XMAX",			xmax),
    DV ("YMAX",			ymax),
    CO ("GOTOXY",		p4_gotoxy),
    CO ("?XY",			p4_question_xy),
    CO ("CLS",			p4_dot_clrscr),
    CO (".CLRSCR",		p4_dot_clrscr),
    CO (".CLREOL",		p4_dot_clreol),
    CO (".HOME",		p4_dot_home),
    CO (".HIGHLIGHT",		p4_dot_highlight),
    CO (".HIGHLIGHT.OFF",	p4_dot_highlight_off),
    CO (".UNDERLINE",		p4_dot_underline),
    CO (".UNDERLINE.OFF",	p4_dot_underline_off),
    CO (".INTENSITY",		p4_dot_intensity),
    CO (".INTENSITY.OFF",	p4_dot_intensity_off),
    CO (".BLINKING",		p4_dot_blink),
    CO (".BLINKING.OFF",	p4_dot_blink_off),
    CO (".REVERSE",		p4_dot_reverse),
    CO (".REVERSE.OFF",		p4_dot_reverse_off),
    CO (".NORMAL",		p4_dot_normal),
    
    /** ring the bell on the terminal (output the corresponding control) */
    CO (".BELL",		p4_dot_bell),

    /** EKEY code constants for function keys */
    OC ("K-LEFT",		P4_KEY_kl),
    OC ("K-RIGHT",		P4_KEY_kr),
    OC ("K-UP",			P4_KEY_ku),
    OC ("K-DOWN",		P4_KEY_kd),
    OC ("K-HOME",		P4_KEY_kh),
    OC ("K-END",		P4_KEY_kH),
    OC ("K-PRIOR",		P4_KEY_kP),
    OC ("K-NEXT",		P4_KEY_kN),
    OC ("K1",			P4_KEY_k1),
    OC ("K2",			P4_KEY_k2),
    OC ("K3",			P4_KEY_k3),
    OC ("K4",			P4_KEY_k4),
    OC ("K5",			P4_KEY_k5),
    OC ("K6",			P4_KEY_k6),
    OC ("K7",			P4_KEY_k7),
    OC ("K8",			P4_KEY_k8),
    OC ("K9",			P4_KEY_k9),
    OC ("K10",			P4_KEY_k0),
    OC ("S-K1",			P4_KEY_F1),
    OC ("S-K2",			P4_KEY_F2),
    OC ("S-K3",			P4_KEY_F3),
    OC ("S-K4",			P4_KEY_F4),
    OC ("S-K5",			P4_KEY_F5),
    OC ("S-K6",			P4_KEY_F6),
    OC ("S-K7",			P4_KEY_F7),
    OC ("S-K8",			P4_KEY_F8),
    OC ("S-K9",			P4_KEY_F9),
    OC ("S-K10",		P4_KEY_FA),
};
P4_COUNTWORDS (term, "Terminal Interface extensions");

/*@}*/

