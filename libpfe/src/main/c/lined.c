/**
 * -- small general purpose line editor
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author$)
 *  @version $Revision$
 *     (modified $Date$)
 *
 *  @description
 *      implements a history buffer. The buffer is a straight
 *      ring buffer or chars - the entries are seperated by '\0'.
 *      The last entry is given at the write-pointer that the next
 *      string gets appended to.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) =
"@(#) $Id$";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-limits.h>

#include <pfe/os-string.h>
#include <pfe/os-ctype.h>

#include <pfe/term-sub.h>
#include <pfe/lined.h>
#include <pfe/_missing.h>

static void left (int dist)	{ while (--dist >= 0) p4_goleft (); }
static void right (int dist)	{ while (--dist >= 0) p4_goright (); }

/* Some shortcuts. All functions in this file work on a "struct lined *l" */
#undef LMAX

#define P	(l->string)
#define LMAX	(l->max_length)
#define H	(l->history)
#define HMAX	(l->history_max)
#define L	(l->length)
#define C	(l->cursor)
#define HL	(l->history_length)
#define HR	(l->history_read)
#define HW	(l->history_write)

#if 0
static void debug(struct lined *l, const char* info)
{
    int x;
    fflush(stdout); fflush(stderr);
    fprintf(stderr, "<%s HR=%i HW=%i H='", info, (HR), (HW));
    for (x=0; x < HL; x++)
    {
        if (H[x] > 0x20) fprintf(stderr, "%c", (H[x]));
        else fprintf(stderr, "\\%c", (0x40+H[x]));
    }
    fprintf(stderr, "'>\n");
    fflush(stderr);
}
#else
#define debug(x,y)
#endif

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
    for (i = 0; *s && i < LMAX; i++)
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

/* add the zero-terminated string at the end of the history ringbuffer
 * and reset the read-point (points also to the end of the history buffer).
 */
static void
put_history_string (struct lined *l, char *p)
{
    char c;
    debug(l,"put");
    if (! HL) { H[0] = '\0'; HR=HW=1; /*HL=2*/}

    do {
        H[HW++] = c = *p++;      /* add each char at the write-point */
        if (HL < HMAX) HL=HW+1;  /* history buffer not used up? */
        HW %= HMAX;              /* wrap at end of history buffer */
    } while (c != '\0');         /* the inserted string is zero terminated */
    HR = HW; H[HW] = '\0';
    debug(l,"!put");
}

#define HL_INCR(X)	(X = (X      + 1) % HL)
#define HL_DECR(X)	(X = (X + HL - 1) % HL)

/* copy the string at the read-point into the target buffer (up to the
 * buffers maximum). Returns the number or chars actually written.
 */
static int
get_history_string (struct lined *l, char *p, int n)
{
    int i, r = HR;

    for (i = 0; i < n; i++)
    {
        if ((*p++ = H[r]) == '\0' || r == HW)
            break;
        HL_INCR (r);
    }
    return i;
}

/* assume the read-point is at the start of a history-string. Move back
 * over the null-char just preceding it to the end of the previous history
 * string - then scan back in the history buffer until the null-char of
 * yet another history-string is found. Adjust the read-pointer to be at
 * the start of the history-string in between. When done, put the history
 * string at the read-point into the editline buffer. When the read-point
 * did move return true, otherwise false if there are no more strings before.
 */
static int
back_history (struct lined *l)
{
    char buf[P4_MAX_INPUT+1];
    int n = HR;
    debug(l,"back");

    if (HL == 0)
        return 0;
    HL_DECR (n);
    do {
        HL_DECR (n);
        if (n == HW)
            return 0;
    } while (H[n] != '\0');
    HL_INCR (n);
    HR = n;
    get_history_string (l, buf, sizeof buf);
    replace_string (l, buf);
    debug(l,"!back");
    return 1;
}

/* assume the read-point is at the start of a history-string. Start scanning
 * up to the null-char that ends it and adjust the read-pointer to the start
 * of the next history string. When done, put the history string at the
 * read-point into the editline buffer. If there is no more history after
 * here then return false, and return true if the read-point did move.
 */
static int
fwd_history (struct lined *l)
{
    char buf[P4_MAX_INPUT+1];
    int r = HR;
    debug(l,"fwd");

    if (HL == 0)
        return 0;
    if (HR == HW)
        return 0;
    for (r = HR; H[r] != '\0'; HL_INCR (r));
    HL_INCR (r);
    HR = r;
    get_history_string (l, buf, sizeof buf);
    replace_string (l, buf);
    debug(l,"!fwd");
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
    char *b, inputbuf[P4_MAX_INPUT+1];
    int c, i, display = 0;

    b = P, P = inputbuf;	/* switch to scratchpad */
    C = L = 0;
    if (dflt)
        replace_string (l, dflt);
    while (L < sizeof(inputbuf))
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
              char cpl[P4_LINE_MAX+1];

              p4_store_c_string ((p4_char_t*) P, C, cpl, sizeof cpl);
              if (display)
              {
                  extern void FXCode(p4_cr);

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
              for (i = C; i < (int)p4_strlen (cpl); i++)
              { insertc (l, cpl[i]); }

              if (c == 1)  { insertc (l, ' '); }
              else  { p4_dot_bell (); }
              continue;
          }
#endif
          do {
              if (C < L && l->overtype)
                  ++C, p4_goright ();
              else
                  insertc (l, ' ');
          } while (C % 8 != 0);
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
    if (l->intercept)
    {   L = l->intercept(P,L);  P[L+1] = '\0';   }
    if (L >= LMAX) L = LMAX-1;
    p4_memcpy (b, P, L + 1);	/* copy scratchpad to output string */
    P = b;			/* restore pointer to original area */
    return 1;
}

/*@}*/
