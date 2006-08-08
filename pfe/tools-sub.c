/** 
 * -- implementation words for TOOLS-EXT / TOOLS-MIX
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version:  %
 *    (%date_modified:  %)
 *
 *  @description
 *      These are tool words used throughout the system implemenation.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: tools-sub.c,v 1.1.1.1 2006-08-08 09:09:36 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/term-sub.h>
#include <pfe/os-ctype.h>
#include <stdarg.h>
#include <stdlib.h>

/**
 * ?PAIRS
 */
_export void
p4_Q_pairs (p4cell n)
{
    if (n != *SP++)
        p4_throw (P4_ON_CONTROL_MISMATCH);
}

/**
 * ?OPEN
 */
_export void
p4_Q_file_open (p4_File *fid)
{
    if (fid == NULL || fid->f == NULL)
        p4_throw (P4_ON_FILE_NEX);
}

/* _________________________________________________________________________ */

/** _?stop_ ( -- ?key )
 * check for a keypress, and if it was 'q' being pressed
 : _?stop_ _key?_ _key_ [char] q = ;
 */
_export int
p4_Q_stop (void) 
{
    if (p4_ekeypressed ())
    {
        register int ch;  	
        ch = p4_getkey ();  
        if (tolower (ch) == 'q') return 1;
    }
    return 0;
}

/** _?cr_ ( -- ?stopped )
 * Like CR but stop after one screenful and return flag if 'q' pressed.
 * Improved by aph@oclc.org (Andrew Houghton)
 */
_export int
p4_Q_cr (void)
{
    static char more[] = "more? ";
    static char help[] = "\r[next line=<return>, next page=<space>, quit=q] ";

    FX (p4_cr);
    if (P4_opt.isnotatty)
        return 0;
    if (PFE.lines < PFE.more)
        return 0;
    PFE.lines = 0;
    for (;;)
    {
        register int ch;
        p4_outs (more);
        ch = p4_getkey (); 	/* tolower(..) may be a macro ! *gud*/
        switch (tolower (ch)) 
	{
         case 'n':		/* no more */
         case 'q':		/* quit    */
             return 1;
         case 'y':		/* more    */
         case ' ':		/* page    */
             while (p4_OUT)
                 FX (p4_backspace);
             PFE.more = PFE.rows - 1;
             return 0;
         case '\r':		/* line    */
         case '\n':		/* line    */
             while (p4_OUT)
                 FX (p4_backspace);
             PFE.more = 1;
             return 0;
         default:		/* unknown */
             p4_dot_bell ();
             /* ... */
         case '?':		/* help    */
         case 'h':		/* help    */
             p4_outs (help);
             break;
	}
    }
}

/* _________________________________________________________________________ */

/**
 * ABORT" string" impl.
 */
_export void
p4_abortq (const char *fmt,...)
{
    char buf[128];
    va_list p;

    va_start (p, fmt);
    vsprintf (buf, fmt, p);
    va_end (p);
    p4_throwstr (P4_ON_ABORT_QUOTE, buf);
}

/* ********************************************************************** */

#ifndef _export
# ifndef HASNT_SYSTEM
  _extern int p4_systemf (const char* s, ...);
# endif
#endif

#ifndef NO_SYSTEM
/**
 * issue a system() call, after formatting
 */
/*export*/ int
p4_systemf (const char *s,...)
{
    char buf[0x100];
    va_list p;
    int r;

    va_start (p, s);
    vsprintf (buf, s, p);
    va_end (p);
    if (! PFE_set.bye) p4_system_terminal ();
    p4_swap_signals ();
    r = system (buf);
    p4_swap_signals ();
    if (! PFE_set.bye) p4_interactive_terminal ();
    p4_dot_normal ();
    return r;
}
#endif /* NO_SYSTEM */


/*@}*/
/* 
 * Local variables:
 * c-file-style: "stroustrup"
 * End:
 */


