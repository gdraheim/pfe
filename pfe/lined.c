/** 
 * -- small general purpose line editor
 * 
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 5.8 %
 *    (%date_modified: Mon Mar 12 10:32:25 2001 %)
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: lined.c,v 0.31 2001-03-19 21:50:41 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-xtra.h>

#include <string.h>
#include <ctype.h>

#include <pfe/term-sub.h>
#include <pfe/lined.h>
#include <pfe/_missing.h>

static void left (int dist)	{ while (--dist >= 0) p4_goleft (); }
static void right (int dist)	{ while (--dist >= 0) p4_goright (); }

/* Some shortcuts. All functions in this file work on a "struct lined *l" */
#undef MAX

#define P	(l->string)
#define MAX	(l->max_length)
#define H	(l->history)
#define HMAX	(l->history_max)
#define L	(l->length)
#define C	(l->cursor)
#define HL	(l->history_length)
#define HR	(l->history_read)
#define HW	(l->history_write)

static void
redisplay (struct lined *l)
{
    int i;

    for (i = 0; i < L; i++)
        p4_putc_printable (P[i]);
    left (i - C);
}

static void
replace_string (struct lined * l, char const * s)
{
    int i;

    left (C);
    for (i = 0; *s && i < MAX; i++)
        p4_putc_printable (P[i] = *s++);
    C = i;
    if (i < L)
    {
        for (; i < L; i++)
            p4_putc (' ');
        left (i - C);
    }
    L = C;
}

static void
put_history_string (struct lined *l, char *p)
{
    char c;
    
    do {
        if (HL < HMAX)
            HL++;
        H[HW++] = c = *p++;
        HW %= HMAX;
    } while (c != '\0');
    HR = HW;
}

#define NEXT(X)	(X = (X      + 1) % HL)
#define PREV(X)	(X = (X + HL - 1) % HL)

static int
get_history_string (struct lined *l, char *p, int n)
{
    int i, r = HR;
    
    for (i = 0; i < n; i++)
    {
        if (r == HW || (*p++ = H[r]) == '\0')
            break;
        NEXT (r);
    }
    return i;
}

static int
back_history (struct lined *l)
{
    char buf[0x100];
    int n = HR;
    
    if (HL == 0)
        return 0;
    PREV (n);
    do {
        PREV (n);
        if (n == HW)
            return 0;
    } while (H[n] != '\0');
    NEXT (n);
    HR = n;
    get_history_string (l, buf, sizeof buf);
    replace_string (l, buf);
    return 1;
}

static int
fwd_history (struct lined *l)
{
    char buf[0x100];
    int r = HR;
    
    if (HL == 0)
        return 0;
    for (r = HR; H[r] != '\0'; NEXT (r));
    NEXT (r);
    if (HR == HW)
        return 0;
    HR = r;
    get_history_string (l, buf, sizeof buf);
    replace_string (l, buf);
    return 1;
}

static void
insertc (struct lined *l, char c)
{
    int i;
  
    if (l->overtype)
    {
        if (C == L)
            L++;
    }else{
        for (i = L++; i > C; i--)
            P[i] = P[i - 1];
    }
    p4_putc_printable (P[C++] = c);
    if (l->overtype)
        return;
    for (i = C; i < L; i++)
        p4_putc_printable (P[i]);
    left (L - C);
}

#ifndef CTRL
#define CTRL(X) ((X) &0x1F)
#endif

int
p4_lined (struct lined *l, char *dflt)
{
    char *b, buf[0x100];		/* scratchpad to work on */
    int c, i, display = 0;

    b = P, P = buf;		/* switch to scratchpad */
    C = L = 0;
    if (dflt) 				
        replace_string (l, dflt);		
    while (L < MAX)
    {
        c = p4_getekey ();
        if (l->caps)
            c = p4_change_case (c);
        switch (c)
	{
         case 0:
             break; 		/* ignore */
         case CTRL('P'):
             c = p4_getkey ();
             if (l->caps)
                 c = p4_change_case (c);
         default:
             if (c >= 0x100)	/* other function key */
             {
                 if (!l->executes || c < P4_KEY_k1 || P4_KEY_k0 < c)
                 {
                     p4_dot_bell ();
                     break;
                 }
                 right (L - C);
                 /* p4_puts ("\\\n"); * echo for function keys - deleted */
                 l->executes[c - P4_KEY_k1] (c - P4_KEY_k1);
                 for (i = 0; i < L; i++)
                     p4_putc_printable (P[i]);
                 left (L - C);
                 break;
             }
             if (dflt)
                 replace_string (l, "");
             insertc (l, c);
             break;
         case '\t':
#ifndef WITH_NO_COMPLETION   /* AUTOCONF-CONFIGURE: */
	  if (l->complete)
          {
	      char cpl[0x100];

	      p4_store_c_string (P, C, cpl, sizeof cpl);
	      if (display)
              {
		  extern FCode(p4_cr);
                  
		  FX (p4_cr);
		  c = l->complete (cpl, cpl, 1);
		  FX (p4_cr);
		  redisplay (l);
              }else{
		  c = l->complete (cpl, cpl, 0);
		  display = 1;
              }

	      if (c == 0)
              {
		  p4_dot_bell ();
		  continue;
              }
	      for (i = C; i < (int)strlen (cpl); i++)
              { insertc (l, cpl[i]); }

	      if (c == 1)  { insertc (l, ' '); }
	      else  { p4_dot_bell (); }
	      continue;
          }
#endif
	  do
              if (C < L && l->overtype)
                  ++C, p4_goright ();
              else
                  insertc (l, ' ');
	  while (C % 8 != 0);
	  break;
         case CTRL('D'):
         case P4_KEY_kr:
             if (C == L)
             {
                 p4_dot_bell ();
                 break;
             }
             p4_goright ();
             C++;
             break;
         case CTRL('S'):
         case P4_KEY_kl:
             if (C == 0)
             {
                 p4_dot_bell ();
                 break;
             }
             p4_goleft ();
             C--;
             break;
         case CTRL('A'):
             while (C && P[C - 1] == ' ')
                 p4_goleft (), C--;
             while (C && P[C - 1] != ' ')
                 p4_goleft (), C--;
             break;
         case CTRL('F'):
             while (C < L && P[C] != ' ')
                 p4_goright (), C++;
             while (C < L && P[C] == ' ')
                 p4_goright (), C++;
             break;
         case P4_KEY_kb:
         case '\x7F':
         case CTRL('H'):
             if (C == 0)
             {
                 p4_dot_bell ();
                 break;
             }
             C--;
             p4_goleft ();
             if (l->overtype)
             {
                 p4_putc_printable (P[C] = ' ');
                 p4_goleft ();
                 break;
             }
         case P4_KEY_kD:
         case CTRL('G'):
             if (C == L)
             {
                 p4_dot_bell ();
                 break;
             }
             for (i = C; ++i < L;)
                 p4_putc_printable (P[i - 1] = P[i]);
             p4_putc_printable (' ');
             left (i - C);
             L--;
             break;
         case P4_KEY_kI:
         case CTRL('V'):
             l->overtype = !l->overtype;
             continue;
         case CTRL('C'):
             l->caps = !l->caps;
             continue;
         case P4_KEY_ku:
         case CTRL('E'):
             if (!H || !back_history (l))
                 p4_dot_bell ();
             break;
         case P4_KEY_kd:
         case CTRL('X'):
             if (!H || !fwd_history (l))
                 p4_dot_bell ();
             break;
         case P4_KEY_kh:
             left (C);
             C = 0;
             break;
         case P4_KEY_kH:
             right (L - C);
             C = L;
             break;
         case CTRL('Q'):
             switch (toupper (p4_getkey ()) | '@')
             {
              case 'S':
                  left (C);
                  C = 0;
                  break;
              case 'D':
                  right (L - C);
                  C = L;
                  break;
              default:
                  p4_dot_bell ();
             }
             break;
         case CTRL('U'):
             replace_string (l, "");
             return 0;
         case CTRL('J'):
         case CTRL('M'):
             /*      case P4_KEY_enter:
              */ goto end;
	}
        display = 0;
        dflt = NULL;
    }
 end:
    right (L - C);
    P[L] = '\0';
    if (H && L > 0)
        put_history_string (l, P);
    memcpy (b, P, L + 1);		/* copy scratchpad to output string */
    P = b;			/* restore pointer to original area */
    return 1;
}

/*@}*/

