/**
 * -- simple FORTH-screenfile editor
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2000.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author$)
 *  @version $Revision$
 *     (modified $Date$)
 *
 *  @description
 *       This is a simple fullscreen FORTH block editor.
 *
 *       will be missing in most builds
 *
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) =
"@(#) $Id$";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-limits.h>

#include <stdlib.h>
#include <stdarg.h>
#include <pfe/os-string.h>
#include <pfe/os-ctype.h>
#include <time.h>
#include <signal.h>

#include <pfe/option-ext.h>
#include <pfe/def-words.h>
#include <pfe/block-ext.h>
#include <pfe/term-sub.h>
#include <pfe/lined.h>

#include <pfe/logging.h>
#include <pfe/_nonansi.h>
#include <pfe/_missing.h>

#define ED (*(struct edit*)(PFE.p[slot]))
static int slot = 0;

typedef char line[64];
typedef line blck[16];		/* block buffer */
struct position { int row, col, scr; };

struct edit
{
  line			/* block buffers used for editing */
    *buf, *blk,			/* and stacking source */
    *linestk, *linetop, *linesp;
  blck
    *blkstk, *blktop, *blksp;

  int row, col;		/* cursor position */
  struct position mark;

  char			/* mode flags: */
    overtype,		/*   overwrite mode */
    caps,		/*   simulate CAPS (for stupid DIN keyboard) */
    stamp_changed,	/*   shall changed screens be stamped? */
    was_replacing,	/*   action of ^L, repeat search or replace? */
    readonly;		/*   file cannot be written */

  char log_name[16];	/* for stamp to screen */

  char search_str[32];
  char search_history[512];

  struct lined search_lined;

  char replace_str[32];
  char replace_history[512];
  struct lined replace_lined;

/*
 * keyboard input and dispatch
 */

  struct helpline *sub_help;
  int sub_help_len;

   void (*saved_on_stop) (void);
   void (*saved_on_continue) (void);
   void (*saved_on_winchg) (void);
   p4_jmp_buf after_stop;
   p4_fenv_t  after_stop_fenv;

/*
 * options
 */
    char const ** editor;
};

#define setcursor(R,C)	p4_gotoxy ((C) + 16, (R))
/* ----------------------------------------------------------------- */
#ifndef EDITOR                  /* USER-CONFIG: */
#define	EDITOR		"emacs"	/* preferred editor for text files, */
#endif				/* env-variable EDITOR overrides this */
#ifndef LOGNAME                 /* USER-CONFIG: */
#define	LOGNAME		"you"	/* if your machine has no env-variable */
#endif				/* LOGNAME, put your name here */
#ifndef ED_TABW                 /* USER-CONFIG: */
#define	ED_TABW		4	/* width of tab steps in block editor */
#endif

/* ----------------------------------------------------------------- */

static void edit_init (struct edit* set)
{
   set->overtype = 0;
   set->caps = 0;
   set->stamp_changed = 0;
   set->was_replacing = 0;

   set->search_lined.string = ED.search_str;
   set->search_lined.max_length = sizeof (ED.search_str);
   set->search_lined.history = ED.search_history;
   set->search_lined.history_max = sizeof (ED.search_history);
   set->search_lined.complete = p4_complete_dictionary;
   set->search_lined.executes = NULL;

   set->replace_lined.string = ED.replace_str;
   set->replace_lined.max_length = sizeof (ED.replace_str);
   set->replace_lined.history = ED.replace_history;
   set->replace_lined.history_max = sizeof (ED.replace_history);
   set->replace_lined.complete = p4_complete_dictionary;
   set->replace_lined.executes = NULL;

   {   /* environment scanning */
       register const char* t;

       if ((t = getenv ("FORTHEDITOR")) != NULL)
       {
           set->editor = p4_change_option_string ((p4_char_t*) "$EDITOR",7,
                                                  t, PFE.set);
       }
       else if ((t = getenv ("PFEEDITOR")) != NULL)
       {
           set->editor = p4_change_option_string ((p4_char_t*) "$EDITOR",7,
                                                  t, PFE.set);
       }
       else if ((t = getenv ("EDITOR")) != NULL)
       {
           set->editor = p4_change_option_string ((p4_char_t*) "$EDITOR",7,
                                                  t, PFE.set);
       }
       else
       {
           static char const * const editor = EDITOR;
           set->editor = (char const * *) & editor;
       }
   }
}

void FXCode_RT (p4_edit_forget)
{   FX_USE_BODY_ADDR;
    register struct edit* set = (struct edit*) FX_POP_BODY_ADDR[0];
    P4_note1 ("clean edit area %p", set);
}

void FXCode (p4_edit_init)
{
    if (slot)
    {
        edit_init (&ED);
        p4_forget_word ("edit:%s", (p4cell) ED.editor,
                        PFX(p4_edit_forget), (p4cell) &ED);
    }
}

/*
 * file-i/o
 */

#define NOBLK UCELL_MAX

static int
scr_changed (void)
{
  ED.blk = (line *) p4_blockfile_block (BLOCK_FILE, SCR);
  return p4_memcmp (ED.blk, ED.buf, sizeof (blck)) != 0;
}

static int
block_empty (char *p)
{
  int n;

  for (n = 64; n < BPBUF; n++)
    if (p[n] != ' ' && p4_isprintable (p[n]))
      return 0;
  return 1;
}

static int
scr_empty (p4_blk_t blk)
{
  return block_empty (p4_blockfile_block (BLOCK_FILE, blk));
}

static void
scr_copy (p4_blk_t dst, p4_blk_t src)
{
  p4_blockfile_block (BLOCK_FILE, src);
  BLOCK_FILE->blk = dst;
  p4_blockfile_update (BLOCK_FILE);
  p4_blockfile_save_buffers (BLOCK_FILE);
}

static void
truncate_file (void)
{
  p4_blk_t blk;

  for (blk = BLOCK_FILE->blkcnt; blk; blk--)
    if (!scr_empty (blk - 1))
      break;
  p4_resize_file (BLOCK_FILE, (_p4_off_t)blk * BPBUF);
}

static void
stamp_screen (void)
{
  time_t t;
  struct tm *tm;
  char stamp[65];
  int n;

  time (&t);
  tm = localtime (&t);
  n = 64 - 3 - p4_strlen (ED.log_name) - 6 - 9;
  sprintf (stamp, "\\ %.*s %s %02d:%02d %02d/%02d/%02d",
           n, &ED.buf[0][2], ED.log_name, tm->tm_hour, tm->tm_min,
           tm->tm_mon + 1, tm->tm_mday, tm->tm_year);
  _p4_buf_copy (ED.buf[0], stamp);
}

static void
writebuf (void)
{
  if (SCR != NOBLK && scr_changed ())
    {
      if (ED.stamp_changed)
        stamp_screen ();
      ED.blk = (line *)p4_blockfile_buffer (BLOCK_FILE, SCR);
      p4_memcpy (ED.blk, ED.buf, sizeof (blck));
      p4_blockfile_update (BLOCK_FILE);
      p4_blockfile_save_buffers (BLOCK_FILE);
    }
}

static void
readbuf (p4_blk_t blk)
{
  ED.blk = (line *)p4_blockfile_block (BLOCK_FILE, blk);
  p4_memcpy (ED.buf, ED.blk, sizeof (blck));
  SCR = blk;
}

static void
changescr (p4_blk_t blk)
{
  writebuf ();
  readbuf (blk);
}


/*
 * screen-i/o
 */

static int
getckey (void)
{
  char c = p4_getwskey ();
  return c < ' '
    ? c + '@'
    : toupper (c);
}

static void
c_printf (char const * fmt,...)
{
  char buf[P4_PIPE_BUF] = "";
  va_list p;

  va_start (p, fmt);
  vsprintf (buf, fmt, p);
  va_end (p);
  p4_puts (buf);
}

static void
type_line (p4_char_t *p, int n)
{
  int i;

  for (i = 0; i < n; i++)
    if (!p4_isprintable (p[i]))
      break;
  if (i < n)
    while (n--)
      p4_putc_printable (*p++);
  else
    p4_type (p, n);
}


/*
 * show help
 */

struct helpline
{
  char row, col;
  char const * str;
};

static struct helpline *displayed_help = NULL;
static struct helpline primary_help[] =
{
  {0, 0, "CURSOR MOVE"},
  {1, 1, "^E up"},
  {2, 1, "^X down"},
  {3, 1, "^S left"},
  {4, 1, "^D right"},
  {5, 1, "^A word left"},
  {6, 1, "^F word right"},

  {0, 20, "INSERT"},
  {1, 21, "^W char"},
  {2, 21, "^N line"},
  {3, 21, "^P quotes ctl"},
  {4, 21, "^V toggle overtype"},

  {0, 40, "DELETE"},
  {1, 41, "^H char left"},
  {2, 41, "^G char right"},
  {3, 41, "^T word right"},
  {4, 41, "^Y line"},

  {0, 60, "SCREEN CHANGE"},
  {1, 61, "^R previous"},
  {2, 61, "^C next"},

  {3, 60, "OTHER"},
  {4, 61, "^L find string"},
  {5, 61, "^Q... more"},
  {6, 61, "^K... more"},
  {7, 61, "^U exit editor"},
};
static struct helpline ctl_k_help[] =
{
  {0, 0, "LINE STACK"},
  {1, 1, "X push line"},
  {4, 1, "Y push&delete"},
  {2, 1, "E pop line"},
  {3, 1, "W pop&insert"},
  {5, 1, "N push eol"},
  {6, 1, "T pop eol"},

  {0, 20, "EVALUATE"},
  {1, 21, "L this line"},
  {2, 21, "B this block"},
  {3, 21, "F block# 1"},

  {0, 40, "INSERT"},
  {1, 41, "V insert block"},
  {2, 40, "DELETE"},
  {3, 41, "G delete block"},

  {0, 60, "OTHER"},
  {1, 61, "M set mark"},
  {2, 61, "R restore block"},
};
static struct helpline ctl_o_help[] =
{
  {0, 0, "OPTIONS"},
  {1, 1, "C caps lock"},
  {2, 1, "S stamp on/off"},
};
static struct helpline ctl_q_help[] =
{
  {0, 0, "CURSOR MOVE"},
  {1, 1, "E top of block"},
  {2, 1, "X bottom"},
  {3, 1, "S begin line"},
  {4, 1, "D end of line"},

  {0, 20, "FIND&REPLACE"},
  {1, 21, "F find string"},
  {2, 21, "A find and replace"},

  {0, 40, "INSERT"},
  {1, 41, "N split line"},
  {2, 40, "DELETE"},
  {3, 41, "Y end of line"},

  {0, 60, "SCREEN CHANGE"},
  {1, 61, "R first"},
  {2, 61, "C last"},
  {3, 60, "OTHER"},
  {4, 61, "M goto mark"},
  {5, 61, "L redraw screen"},
};

static void
show_help (int row, int col, int n, struct helpline *h)
{
  for (; n > 0; n--, h++)
    {
      setcursor (row + h->row, col + h->col);
      p4_puts (h->str);
    }
}

static void
show_bottom_help (int n, struct helpline *h)
{
  if (displayed_help == h)
    return;
  setcursor (17, -16);
  p4_dot_clrdown ();
  show_help (17, -16, n, h);
  displayed_help = h;
}


/*
 * display blocks and lines on screen
 */

static int
coleol (int row)
{
  char *p = ED.buf[row];
  int col;

  col = 63;
  if (p[col] == ' ')
    while (col > 0 && p[col - 1] == ' ')
      col--;
  return col;
}

static char *
ptreol (int row)
{
  char *p, *q = ED.buf[row];

  for (p = q + 64; p > q && p[-1] == ' '; p--);
  return p;
}

static void
show_line (int i, int col)
{
  char *p;
  int n;

  setcursor (i, col);
  p = &ED.buf[i][col];
  n = ptreol (i) - p;
  if (n > 0)
    type_line ((p4_char_t*) p, n);
  if (col + n < 64)
    p4_dot_clreol ();
}

static void
show_all_lines (int from)
{
  int i;

  for (i = from; i < 16; i++)
    show_line (i, 0);
}

static void
show_screen (void)
{
  setcursor (0, -11);
  c_printf ("%4u", (unsigned) SCR);
  show_all_lines (0);
}

static void
show_status (void)
{
  setcursor (4, -16);
  c_printf ("%3d  %3d", (int) ED.row, (int) ED.col);
  setcursor (5, -12);
  c_printf ("%02X", (unsigned char) ED.buf[ED.row][ED.col]);
  if (ED.readonly)
    {
      if (scr_changed ())
        {
          p4_memcpy (ED.buf, ED.blk, sizeof (blck));
          p4_dot_bell ();
          show_all_lines (0);
        }
    }
  else
    {
      setcursor (0, -4);
      p4_putc (scr_changed () ? '*' : ' ');
    }
}

static void
show_snr (void)
{
  p4_dot_underline_on ();
  setcursor ( 8, -15), c_printf ("%-12.12s", ED.search_str);
  setcursor (10, -15), c_printf ("%-12.12s", ED.replace_str);
  p4_dot_underline_off ();
}

static void
show_options (void)
{
  setcursor (12, -15);
  c_printf ("%c %c %c %c",
            ED.caps ? 'C' : ' ',
            ED.overtype ? 'O' : 'I',
            ED.was_replacing ? 'R' : 'F',
            ED.stamp_changed ? 'S' : ' ');
}

static void
show_line_stack (void)
{
  char buf[65];

  if (ED.linesp == ED.linetop)
    p4_memset (buf, '-', 64);
  else
    p4_memcpy (buf, *ED.linesp, 64);
  buf[64] = '\0';
  setcursor (16, -16);
  p4_dot_reverse ();
  c_printf ("line stack:  %2d %s", ED.linetop - ED.linesp, buf);
  p4_dot_normal ();
}

static void
show_frame (void)
{
  int i;

  setcursor ( 0, -16), p4_puts ("blk #");
  setcursor ( 1, -16), c_printf ("%-10.10s", BLOCK_FILE->name);
  setcursor ( 3, -16), p4_puts ("row  col");
  setcursor ( 5, -16), p4_puts ("hex");
  setcursor ( 7, -16), p4_puts ("find:");
  setcursor ( 9, -16), p4_puts ("replace:");
  setcursor (11, -16), p4_puts ("options:");
  if (ED.readonly)
    {
      setcursor (0, -4);
      p4_putc ('%');
    }
  p4_dot_reverse ();
  for (i = 0; i < 16; i++)
    {
      setcursor (i, -3);
      c_printf ("%2d", i);
    }
  p4_dot_normal ();
}

static void
show_all (void)
{
  p4_dot_normal ();
  p4_dot_clrscr ();
  show_frame ();
  show_snr ();
  show_options ();
  show_screen ();
  show_line_stack ();
  displayed_help = NULL;
  show_bottom_help (DIM (primary_help), primary_help);
}

static void
show_ctl (char c)
{
  setcursor (15, -7);
  if (c)
    c_printf ("^%c", c);
  else
    p4_puts ("  ");
}

static int
prompt_for (char const * prompt, struct lined *l, char *dflt)
{
  setcursor (16, -16);
  p4_dot_reverse ();
  c_printf ("%15s[%*s]%*s", prompt,
           l->max_length, "",
           80 - 17 - l->max_length, "");
  setcursor (16, 0);
  p4_lined (l, dflt);
  p4_dot_normal ();
  show_line_stack ();
  return l->length;
}

static int
yesno (char const * prompt)
{
  int c;

  setcursor (16, -16);
  p4_dot_reverse ();
  c_printf ("%15s?%*s", prompt, 64, "");
  setcursor (16, 0);
  do
    c = toupper (p4_getkey ());
  while (c != 'N' && c != 'Y');
  show_line_stack ();
  return c == 'Y';
}

static void
word_from_cursor (char *p, int n)
{
  char *q = &ED.buf[ED.row][ED.col];
  while (q < ED.buf[16] && q[0] == ' ')
    q++;
  while (q > ED.buf[0] && q[-1] != ' ')
    q--;
  while (q < ED.buf[16] && q[0] != ' ')
    {
      *p++ = *q++;
      if (--n == 0)
        break;
    }
  *p = '\0';
}


/*
 * insert / delete character, word, line
 */

static void
insertc (char c)
{
  char *p = &ED.buf[ED.row][ED.col], *q;

  for (q = &ED.buf[ED.row][coleol (ED.row)] + 1; --q > p;)
    q[0] = q[-1];
  *p = c;
}

static void
deletec (void)
{
  char *p, *q = ptreol (ED.row) - 1;

  for (p = &ED.buf[ED.row][ED.col]; p < q; p++)
    p[0] = p[1];
  *p = ' ';
}

static void
insertl (int row)
{
  int i;

  for (i = 15; i > row; i--)
    _p4_buf_copy (ED.buf[i], ED.buf[i - 1]);
  p4_memset (ED.buf + i, ' ', sizeof (ED.buf[15]));
}

static void
deletel (int row)
{
  int i;

  for (i = row; i < 15; i++)
    _p4_buf_copy (ED.buf[i], ED.buf[i + 1]);
  p4_memset (ED.buf + 15, ' ', sizeof (ED.buf[15]));
}

static void
clear_endl (void)
{
  char *p = &ED.buf[ED.row][ED.col], *q = ptreol (ED.row);

  if (q > p)
    p4_memset (p, ' ', q - p);
}

static void
strip_blanks (char **p, int *n)
{
  while (*n && (*p)[*n - 1] == ' ')
    --*n;
  while (*n && **p == ' ')
    --*n, ++*p;
}

static int
append_line (char *ln)
{
  char *q = ptreol (ED.row);
  int n = 64, j = q - ED.buf[ED.row];

  strip_blanks (&ln, &n);
  if (j)
    q++, j++;
  if (64 - j < n)
    return 0;
  p4_memcpy (q, ln, n);
  return 1;
}

static void
split_line (void)
{
  if (ED.row == 15)
    clear_endl ();
  else
    {
      insertl (ED.row);
      p4_memcpy (ED.buf[ED.row], ED.buf[ED.row + 1], ED.col);
      p4_memset (ED.buf[ED.row + 1], ' ', ED.col);
    }
}

static void
join_line (void)
{
  if (ED.row < 15 && append_line (ED.buf[ED.row + 1]))
    deletel (ED.row + 1);
  else
    p4_dot_bell ();
}

static void
deletew (void)
{
  char *p = &ED.buf[ED.row][ED.col];
  int n = ptreol (ED.row) - p;

  if (n <= 0)
    {
      join_line ();
      show_all_lines (ED.row);
    }
  else
    {
      while (n && *p != ' ')
        deletec (), n--;
      while (n && *p == ' ')
        deletec (), n--;
    }
}

static void
inserts (void)
{
  p4_blk_t blk;

  writebuf ();
  for (blk = BLOCK_FILE->blkcnt; blk > SCR; blk--)
    if (!scr_empty (blk - 1))
      break;
  for (; blk > SCR; blk--)
    scr_copy (blk, blk - 1);
  p4_memset (ED.buf, ' ', sizeof (blck));
  stamp_screen ();
  writebuf ();
  show_screen ();
}

static int
deletes (void)
{
	p4_blk_t blk;

	if ((!scr_empty (SCR) || !block_empty (ED.buf[0]))
			&& !yesno ("delete screen")) {
		return 0;
	}
	writebuf ();
	for (blk = SCR + 1; blk < BLOCK_FILE->blkcnt; blk++)
	{
		scr_copy (blk - 1, blk);
	}
	void* buffer = p4_blockfile_buffer (BLOCK_FILE, BLOCK_FILE->blkcnt - 1);
	p4_memset (buffer, ' ', BPBUF);
	FX (p4_update);
	readbuf (SCR);
	show_screen ();
	return 1;
}


/*
 * move cursor
 */

static void
fwd_word (void)
{
  char *p = &ED.buf[ED.row][ED.col];
  int n;

  while (p < ED.buf[16] - 1)
    if (*p != ' ')
      p++;
    else
      break;
  while (p < ED.buf[16] - 1)
    if (*p == ' ')
      p++;
    else
      break;
  n = p - ED.buf[0];
  ED.row = n / 64;
  ED.col = n % 64;
}

static void
back_word (void)
{
  char *p = &ED.buf[ED.row][ED.col];
  int n;

  while (ED.buf[0] < p)
    if (p[-1] == ' ')
      p--;
    else
      break;
  while (ED.buf[0] < p)
    if (p[-1] != ' ')
      p--;
    else
      break;
  n = p - ED.buf[0];
  ED.row = n / 64;
  ED.col = n % 64;
}


/*
 * line- and block-stack
 */

static int
push_to_linestk (char *p, int n)
{
  if (ED.linesp == ED.linestk)
    {
      p4_dot_bell ();
      return 0;
    }
  ED.linesp--;
  p4_memcpy (*ED.linesp, p, n);
  p4_memset (*ED.linesp + n, ' ', 64 - n);
  show_line_stack ();
  return 1;
}

static int
pushln (int row)
{
  return push_to_linestk (ED.buf[ED.row], 64);
}

static int
popln (char *to)
{
  if (ED.linesp == ED.linetop)
    {
      p4_dot_bell ();
      return 0;
    }
  p4_memcpy (to, *ED.linesp++, 64);
  show_line_stack ();
  return 1;
}

void
push_del_line (void)
{
  if (pushln (ED.row))
    {
      deletel (ED.row);
      show_all_lines (ED.row);
    }
}

void
push_line (void)
{
  if (pushln (ED.row) && ED.row < 15)
    ED.row++;
}

void
pop_spread_line (void)
{
  if (ED.linesp < ED.linetop)
    {
      insertl (ED.row);
      popln (ED.buf[ED.row]);
      show_all_lines (ED.row);
    }
  else
    p4_dot_bell ();
}

void
pop_line (void)
{
  if (popln (ED.buf[ED.row]))
    {
      show_line (ED.row, 0);
      if (ED.row > 0)
        ED.row--;
    }
}

void
push_line_end (void)
{
  if (push_to_linestk (&ED.buf[ED.row][ED.col], 64 - ED.col))
    {
      clear_endl ();
      show_line (ED.row, ED.col);
    }
}

void
pop_line_end (void)
{
  int c = coleol (ED.row);

  if (c >= 63)
    {
      p4_dot_bell ();
      return;
    }
  ED.col = c ? c + 1 : 0;
  if (ED.linesp < ED.linetop && append_line (*ED.linesp))
    {
      ED.linesp++;
      show_line_stack ();
      show_line (ED.row, ED.col);
    }
  else
    p4_dot_bell ();
}


/*
 * find and replace
 */

static int
search_string (int prompt)
{
  p4_blk_t blk;
  int n, l;
  char *b, *p;

  l = p4_strlen (ED.search_str);
  if (prompt || l == 0)
    {
      char buf[65];
      word_from_cursor (buf, sizeof buf);
      ED.search_lined.overtype = ED.overtype;
      l = prompt_for ("Search: ", &ED.search_lined, buf);
      show_snr ();
    }
  if (l == 0)
    return 0;
  b = ED.buf[0];
  n = &ED.buf[ED.row][ED.col] + 1 - b;
  p = p4_search (b + n, BPBUF - n, ED.search_str, l);
  if (!p)
    for (blk = SCR + 1; blk < BLOCK_FILE->blkcnt; blk++)
      {
        b = p4_blockfile_block (BLOCK_FILE, blk);
        p = p4_search (b, BPBUF, ED.search_str, l);
        if (p)
          {
            changescr (blk);
            show_screen ();
            break;
          }
      }
  if (!p)
    return 0;
  n = p - b;
  ED.row = n / 64;
  ED.col = n % 64;
  return 1;
}

static int
replace_string (int prompt)
{
  int i, lr, ls;

  if (!search_string (prompt))
    return 0;
  ls = p4_strlen (ED.search_str);
  lr = p4_strlen (ED.replace_str);
  if (prompt || lr == 0)
    {
      ED.replace_lined.overtype = ED.overtype;
      lr = prompt_for ("Replace: ", &ED.replace_lined, NULL);
      show_snr ();
    }
  if (lr == 0)
    return 0;
  for (i = 0; i < ls; i++)
    deletec ();
  for (i = lr; --i >= 0;)
    insertc (ED.replace_str[i]);
  show_line (ED.row, ED.col);
  return 1;
}


/*
 * keyboard input and dispatch
 */

static void
show_sub_help (int sig)
{
#if defined SYS_EMX || defined SYS_WC_OS2V2
  signal (sig, SIG_ACK);
#elif defined SIGALRM
  signal (SIGALRM, SIG_IGN);
#endif
  show_bottom_help (ED.sub_help_len, ED.sub_help);
  setcursor (ED.row, ED.col);
}

#ifndef PFE_HAVE_ALARM
unsigned int
alarm (unsigned int sec)
{
  return 0;
}
#endif /* PFE_HAVE_ALARM */

static void
submenu (char key, int n, struct helpline *h)
{
  show_ctl (key);
  if (key)
    {
      ED.sub_help_len = n;
      ED.sub_help = h;
#if defined SIGALRM
      signal (SIGALRM, show_sub_help);
      alarm (1);
#else
      show_sub_help (0);
#endif
    }
  else
    {
#if defined SIGALRM
      signal (SIGALRM, SIG_IGN);
#endif
      show_bottom_help (DIM (primary_help), primary_help);
    }
  setcursor (ED.row, ED.col);
}

static void
do_ctlK (void)
{
  int c;

  submenu ('K', DIM (ctl_k_help), ctl_k_help);
  c = getckey ();
  submenu (0, 0, NULL);
  switch (c)
    {
    default:
      p4_dot_bell ();
    case ' ':
    case '\033':
      break;
    case 'Y':
    case 'Z':
      push_del_line ();
      break;
    case 'X':
      push_line ();
      break;
    case 'W':
      pop_spread_line ();
      break;
    case 'E':
      pop_line ();
      break;
    case 'N':
      push_line_end ();
      break;
    case 'T':
      pop_line_end ();
      break;
    case 'D':
      stamp_screen ();
      show_line (0, 0);
      ED.row = 0;
      ED.col = 2;
      ED.overtype = 1;
    case 'L':
      show_bottom_help (0, NULL);
      writebuf ();
      p4_evaluate ((p4char*) ED.buf[ED.row], 64);
      readbuf (SCR);
      show_all ();
      break;
    case 'B':
      show_bottom_help (0, NULL);
      writebuf ();
      p4_blockfile_load (BLOCK_FILE, SCR);
      readbuf (SCR);
      show_all ();
      break;
    case 'F':
      show_bottom_help (0, NULL);
      writebuf ();
      truncate_file ();
      p4_blockfile_load (BLOCK_FILE, 1);
      readbuf (SCR);
      show_all ();
      break;
    case 'M':
      ED.mark.row = ED.row;
      ED.mark.col = ED.col;
      ED.mark.scr = SCR;
      break;
    case 'R':
      p4_memcpy (ED.buf, ED.blk, sizeof (blck));
      show_screen ();
      break;
    case 'V':
      inserts ();
      break;
    case 'G':
      deletes ();
      break;
    }
}

static void
do_ctlO (void)
{
  int c;

  submenu ('O', DIM (ctl_o_help), ctl_o_help);
  c = getckey ();
  submenu (0, 0, NULL);
  switch (c)
    {
    default:
      p4_dot_bell ();
    case ' ':
    case '\033':
      break;
    case 'C':
      ED.caps ^= 1;
      show_options ();
      break;
    case 'S':
      ED.stamp_changed ^= 1;
      show_options ();
      break;
    }
}

static void
do_ctlQ (void)
{
  int c;

  submenu ('Q', DIM (ctl_q_help), ctl_q_help);
  c = getckey ();
  submenu (0, 0, NULL);
  switch (c)
    {
    default:
      p4_dot_bell ();
    case ' ':
    case '[':
      break;
    case 'E':
      ED.row = 0;
      break;
    case 'X':
      ED.row = 15;
      break;
    case 'S':
      ED.col = 0;
      break;
    case 'D':
      ED.col = coleol (ED.row);
      break;
    case 'I':
      ED.col--;
      ED.col -= ED.col % ED_TABW;
      break;
    case 'N':
      split_line ();
      show_all_lines (ED.row);
      break;
    case 'Y':
      clear_endl ();
      show_line (ED.row, ED.col);
      break;
    case 'L':
      show_all ();
      break;
    case 'R':
      changescr (0);
      show_screen ();
      break;
    case 'C':
      changescr (BLOCK_FILE->blkcnt - 1);
      show_screen ();
      break;
    case 'M':
      changescr (ED.mark.scr);
      ED.row = ED.mark.row;
      ED.col = ED.mark.col;
      show_screen ();
      break;
    case 'F':
      if (!search_string (1))
        p4_dot_bell ();
      ED.was_replacing = 0;
      show_options ();
      break;
    case 'A':
      if (!replace_string (1))
        p4_dot_bell ();
      ED.was_replacing = 1;
      show_options ();
      break;
    }
}

static int
do_key (char c)
/* interpretiert Zeichen erster Stufe */
{
  switch (c)
    {
    case '\033':		/* map Esc to ^Q, on Linux: Alt-F == ^Q-F */
    case 'Q' - '@':
      do_ctlQ ();
      break;
    case 'K' - '@':
      do_ctlK ();
      break;
    case 'O' - '@':
      do_ctlO ();
      break;
    case 'P' - '@':
      c = p4_getkey ();
    default:
      if (ED.overtype)
        {
          ED.buf[ED.row][ED.col] = c;
          p4_putc (c);
        }
      else
        {
          insertc (c);
          show_line (ED.row, ED.col);
        }
    case 'D' - '@':
      if (++ED.col >= 64)
        {
          ED.col -= 64;
    case 'X' - '@':
          if (ED.row < 15)
            ED.row++;
        }
      break;
    case 'S' - '@':
      if (--ED.col < 0)
        {
          ED.col += 64;
    case 'E' - '@':
          if (ED.row > 0)
            ED.row--;
        }
      break;
    case '\x7F':
    case 'H' - '@':
      if (ED.col == 0)
        break;
      ED.col--;
    case 'G' - '@':
      deletec ();
      show_line (ED.row, ED.col);
      break;
    case 'I' - '@':
      ED.col += ED_TABW - ED.col % ED_TABW;
      break;
    case 'W' - '@':
      insertc (' ');
      break;
    case 'A' - '@':
      back_word ();
      break;
    case 'F' - '@':
      fwd_word ();
      break;
    case 'T' - '@':
      deletew ();
      show_line (ED.row, ED.col);
      break;
    case 'M' - '@':
      ED.col = 0;
      if (ED.row < 15)
        ED.row++;
      break;
    case 'N' - '@':
      insertl (ED.row);
      show_all_lines (ED.row);
      break;
    case 'Y' - '@':
      deletel (ED.row);
      show_all_lines (ED.row);
      break;
    case 'Z' - '@':
      p4_memset (ED.buf, ' ', sizeof (blck));
      show_screen ();
      break;
    case 'V' - '@':
      ED.overtype ^= 1;
      show_options ();
      break;
    case 'L' - '@':
      if (ED.was_replacing
          ? replace_string (0)
          : search_string (0))
        p4_dot_bell ();
      break;
    case 'R' - '@':
      if (SCR <= 0)
        {
          p4_dot_bell ();
          break;
        }
      changescr (SCR - 1);
      show_screen ();
      break;
    case 'C' - '@':
      if (SCR == BLOCK_FILE->blkcnt && !scr_changed ())
        {
          p4_dot_bell ();
          break;
        }
      changescr (SCR + 1);
      show_screen ();
      break;
    case 'U' - '@':
      writebuf ();
      truncate_file ();
      show_bottom_help (0, NULL);
      return 1;
    }
  return 0;
}

static void
free_bufs (void)
{
  if (ED.buf)
    p4_xfree (ED.buf);
  if (ED.linestk)
    p4_xfree (ED.linestk);
  if (ED.blkstk)
    p4_xfree (ED.blkstk);
}

static int
p4_alloc_bufs (int ls, int bs)
{
  ED.buf = (line *) p4_calloc (1, sizeof (blck));
  ED.linestk = (line *) p4_calloc (1, sizeof (*ED.linestk) * ls);
  ED.blkstk = (blck *) p4_calloc (1, sizeof (*ED.blkstk) * bs);
  if (!ED.buf || !ED.linestk || !ED.blkstk)
    {
      free_bufs ();
      return 0;
    }
  ED.linesp = ED.linetop = ED.linestk + ls;
  ED.blksp = ED.blktop = ED.blkstk + bs;
  return 1;
}


/*
 * register additional actions for certain events:
 */


static void
ed_on_stop (void)
{
  show_bottom_help (0, NULL);
  ED.saved_on_stop ();
}

static void
ed_on_continue (void)
{
  ED.saved_on_continue ();
  p4_tty_interrupt_key (0);
  p4_longjmp (ED.after_stop, 1);
}

static void
ed_on_winchg (void)
{
  ED.saved_on_winchg ();
  show_all ();
}

void
p4_edit (int n, int r, int c)
{
  char *logn;
  volatile char intkey = p4_tty_interrupt_key (0);

  logn = getenv ("LOGNAME");
  p4_strncpy (ED.log_name, logn ? logn : LOGNAME, sizeof (ED.log_name));
  switch (BLOCK_FILE->mode)
    {
    case FMODE_RO:
    case FMODE_ROB:
      ED.readonly = 1;
      break;
    default:
      ED.readonly = 0;
      break;
    }
  if (!p4_alloc_bufs (32, 10))
    p4_throw (P4_ON_OUT_OF_MEMORY);
  readbuf (n);
  ED.row = r;
  ED.col = c;

  ED.saved_on_stop = PFE.on_stop; PFE.on_stop = ed_on_stop;
  ED.saved_on_continue = PFE.on_continue; PFE.on_continue = ed_on_continue;
  ED.saved_on_winchg = PFE.on_winchg; PFE.on_winchg = ed_on_winchg;
  p4_setjmp_fenv_save(& ED.after_stop_fenv);
  if (p4_setjmp (ED.after_stop)) {
          p4_setjmp_fenv_load(& ED.after_stop_fenv);
  }

  displayed_help = NULL;
  show_all ();
  for (;;)
    {
      int key;

      setcursor (ED.row, ED.col);
      key = p4_getwskey ();
      if (ED.caps)
        key = p4_change_case (key);
      if (do_key (key))
        break;
      show_status ();
    }
  free_bufs ();
  p4_tty_interrupt_key (intkey);

  PFE.on_stop = ED.saved_on_stop;
  PFE.on_continue = ED.saved_on_continue;
  PFE.on_winchg = ED.saved_on_winchg;
}

/** EDIT-BLOCK ( blk -- )
 * start the internal block-editor on the assigned block
 */
void FXCode (p4_edit_block)
{
   p4_edit (*SP++, 0, 0);
}

#ifndef NO_SYSTEM

/** EDIT-TEXT name ( -- )
 * start an external => EDITOR with the specified filename
 */
void FXCode (p4_edit_text)
{
  p4_charbuf_t *nm = p4_word (' ');

  if (*nm == '\0')
    p4_throw (P4_ON_FILE_NEX);

  p4_systemf ("%s %s", ED.editor, p4_pocket_expanded_filename (
                  P4_CHARBUF_PTR(nm), P4_CHARBUF_LEN(nm),
                  *P4_opt.inc_paths, *P4_opt.inc_ext));
}

/** EDIT-ERROR ( -- )
 * if an error occured, this routine can be called to invoke
 * an appropriate => EDITOR (see also =>"EDIT-BLOCK")
 */
void FXCode (p4_edit_error)
{
  switch (PFE.input_err.source_id)
    {
    case 0:
      if (PFE.input_err.blk)
        {
          p4_edit (
             PFE.input_err.blk,
             PFE.input_err.to_in / 64,
             PFE.input_err.to_in % 64);
          break;
        }
    case -1:
      p4_dot_bell ();
      break;
    default:
      {
        File *f = (File *) PFE.input_err.source_id;

        p4_systemf ("%s +%u %s", ED.editor, (unsigned) f->blk + 1, f->name);
        break;
      }
    }
}
#endif /* NO_SYSTEM */

P4_LISTWORDSET (edit) [] =
{
    P4_SLOT("", &slot),
    P4_SSIZ("", sizeof(struct edit)),

    P4_INTO("EXTENSIONS", 0),
    P4_XXco ("<<edit-init>>",		p4_edit_init),
    P4_FXco ("EDIT-BLOCK",		p4_edit_block),
#ifndef NO_SYSTEM
    P4_FXco ("EDIT-TEXT",		p4_edit_text),
    P4_FXco ("EDIT-ERROR",		p4_edit_error),
#endif
    P4_INTO ("FORTH", 0),
    P4_FXco ("EDIT-BLOCK-START",	p4_edit_block),
};
P4_COUNTWORDSET (edit, "EDIT - builtin forth editor");

/*@}*/
