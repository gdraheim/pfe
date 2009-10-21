/**
 * -- terminal driver that opens an xterm
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.5 $
 *     (modified $Date: 2008-04-20 04:46:30 $)
 *
 *  @description
 *                   NOT FINISHED YET !!!!        *guidod*
 *
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) =
"@(#) $Id: term-x11.c,v 1.5 2008-04-20 04:46:30 guidod Exp $";
#endif

#define _P4_SOURCE 1

/* extern char* vt100_rawkey_string[P4_NUM_KEYS]; etc... */
extern char** rawkey_string;	/* what all those keys really send */

/*
 * These variables are defined in the term.c and initialized by
 * term-xxx:prepare_terminal().  If window size can change, it would
 * be nice if they were kept up to date.
 */

/* XTerm specific variables */

#include <pfe/pfe-sub.h>
#include <pfe/term-sub.h>
#include <pfe/tools-ext.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define TERM_PTR_PRIV PFE.priv
#define TERM_PTR ((p4_x11_term*)(TERM_PTR_PRIV))
#define TERM (*TERM_PTR)

#define AttrNormal 0
#define AttrCursor 1
#define AttrInvert 2
#define AttrUnderline  4
#define AttrBold   8

typedef struct p4_x11_term_
{
    const char* display_name;
    const char* font_name;
    Display* display;
    int screen; // Screen screen;
    Window window;
    GC     gc;
    XFontStruct* font;
    XSizeHints size;
    XEvent event;
    Atom wm_close, wm_protocol, wm_selection, text_selection, gtk_selection;
    int xw, yw; /* heigth and width of a char-cell : depends on xfont */
    int colors[16];
    unsigned long fgcolor;
    unsigned long bgcolor;
    char* selection;
    struct {
        int x;
        int y;
        char attr;
    } cursor;
    char* buf; /* character content */
    char* attr; /* bold, underline, etc */
    int interactive;
    char input[32];
    int inputlen;
} p4_x11_term;

// #define TERM_line_width()  TERM_width()
#define TERM_line_width()  256
#define TERM_font_width()  (TERM.size.width_inc)
#define TERM_font_height() (TERM.size.height_inc)
#define TERM_height()      (TERM.size.height / TERM_font_height())
#define TERM_width()       (TERM.size.width / TERM_font_width())
#define TERM_buf(x,y)      (TERM.buf[ (y)*TERM_line_width() + (x) ])
#define TERM_attr(x,y)     (TERM.attr[ (y)*TERM_line_width() + (x) ])

static void x11_clear(void);

static void x11_perror(const char* msg, int code) {
    if (! code) return;
    char buf[256];
    XGetErrorText(TERM.display, code, buf, sizeof(buf));
    fprintf(stderr, "X11:%s: %s\n", msg, buf);
}

int x11_interrupt_key (char ch)
{
   return 0;
}

int x11_prepare_terminal (void)
{
    if (TERM_PTR != NULL) return 0;
    TERM_PTR_PRIV = calloc(1, sizeof (p4_x11_term));

    /* a hack for having some meaningful value for display
     * actually we can provide a null to XopenDisplay for defaultdisplay
     */
    TERM.display_name = getenv ("DISPLAY");
    TERM.font_name = "7x13bold";

    TERM.display = XOpenDisplay (TERM.display_name);
    if (! TERM.display) {
        fprintf (stderr, "cannot open -display %s - fail", TERM.display_name);
        free (TERM_PTR);
        exit(8);
    }

    // TERM.screen = XDefaultScreenOfDisplay (TERM.display);
    TERM.screen = XDefaultScreen (TERM.display);
    TERM.font = XLoadQueryFont (TERM.display, TERM.font_name);

    if (! TERM.font) {
        fprintf (stderr, "cannot open Font -fn %s - fail", TERM.font_name);
        free (TERM_PTR);
        exit(8);
    }
    if (TERM.font->max_bounds.width != TERM.font->min_bounds.width)
        fprintf (stderr, "not a fixed with font: %s", TERM.font_name);

    static int min_width = 80;
    static int min_height = 24;
    TERM.size.flags = PResizeInc | PMinSize | PBaseSize;
    TERM.size.height_inc = TERM.font->ascent + TERM.font->descent;
    TERM.size.width_inc = TERM.font->max_bounds.width;
    TERM.size.min_width = TERM.size.width_inc * min_width;
    TERM.size.min_height = TERM.size.height_inc * min_height;
    TERM.size.base_width = 0;
    TERM.size.base_height = 0;
    TERM.size.x = 0;
    TERM.size.y = 0;
    TERM.size.width = TERM.size.min_width;
    TERM.size.height = TERM.size.min_height;
    int disp_height = (DisplayHeight (TERM.display, TERM.screen) / 4) * 3;
    int term_height = disp_height / TERM_font_height();
    if (term_height > TERM_height())
        TERM.size.height = term_height * TERM_font_height();
    int term_gravity = 0;
    char term_geometry[20];
    int term_border = 4;
    sprintf(term_geometry, "%dx%d", TERM_width(), TERM_height());
    XWMGeometry(TERM.display, TERM.screen, term_geometry, term_geometry,
                term_border, & TERM.size, & TERM.size.x, & TERM.size.y,
                & TERM.size.width, & TERM.size.height, & term_gravity);
    Window root = RootWindow(TERM.display, TERM.screen);
    // Colormap colormap = DefaultColormap(TERM.display, TERM.screen);
    // int depth = DisplayPlanes(TERM.display, TERM.screen);
    // Visual visual = DefaultVisual(TERM.display, TERM.screen);
    // XSetWindowColormap(TERM.display, TERM.window, colormap);
    // XColor color;
    // XParseColor(TERM.display, colormap, name, & color); // rgb.txt
    // XAllocColor(TERM.display, colormap, color);
    TERM.fgcolor = BlackPixel(TERM.display, TERM.screen);
    TERM.bgcolor = WhitePixel(TERM.display, TERM.screen);
    TERM.window = XCreateSimpleWindow(
        TERM.display, root, TERM.size.x, TERM.size.y,
        TERM.size.width, TERM.size.height,
        term_border, TERM.bgcolor, TERM.fgcolor);
    XSelectInput(TERM.display, TERM.window, ExposureMask | PPosition |
                 KeyPressMask | ButtonPressMask | StructureNotifyMask);

    TERM.wm_close = XInternAtom(TERM.display, "WM_DELETE_WINDOW", False);
    TERM.wm_protocol = XInternAtom(TERM.display, "WM_PROTOCOLS", False);
    TERM.wm_selection = XInternAtom(TERM.display, "PRIMARY", False);
    TERM.gtk_selection = XInternAtom(TERM.display, "GTK_SELECTION", False);
    TERM.text_selection = XInternAtom(TERM.display, "STRING", False);
    TERM.selection = NULL;
    Atom* atom_list = NULL;
    int atom_list_len = 0;
    XGetWMProtocols(TERM.display, TERM.window, & atom_list, & atom_list_len);
    Atom* new_list = calloc(atom_list_len + 1, sizeof(Atom));
    memcpy(new_list, atom_list, atom_list_len * sizeof(Atom));
    new_list[atom_list_len] = TERM.wm_close;
    XFree(atom_list);
    XSetWMProtocols(TERM.display, TERM.window, new_list, atom_list_len + 1);
    XGCValues values;
    TERM.gc = XCreateGC(TERM.display, TERM.window, 0, &values);
    XSetFont(TERM.display, TERM.gc, TERM.font->fid);
    XSetForeground(TERM.display, TERM.gc, TERM.fgcolor);
    XSetLineAttributes(TERM.display, TERM.gc, /*width*/ 1,
                       LineSolid, CapRound, JoinRound);
    static int dash_offset = 1;
    static char dash_list[2] = { 12, 24 };
    XSetDashes(TERM.display, TERM.gc, dash_offset,
               dash_list, sizeof(dash_list));
    XMapWindow(TERM.display, TERM.window);
    int len = TERM_line_width() * (TERM_height()+1);
    TERM.buf = malloc(len);
    TERM.attr = malloc(len);
    x11_clear();
    return 1;
}

void
x11_cleanup_terminal (void)
{
    if (TERM_PTR) {
        XFreeFont (TERM.display, TERM.font);
        XCloseDisplay (TERM.display);
        free (TERM.buf);
        free (TERM_PTR);
        TERM_PTR_PRIV = NULL;
    }
    return; /* nothing more to do here */
}

void
x11_redraw(int x, int y)
{
    char* buf = & TERM_buf(x, y);
    char attr = TERM_attr(x, y);
    if (attr & (AttrInvert | AttrCursor)) {
        XSetForeground(TERM.display, TERM.gc, TERM.bgcolor);
        XSetBackground(TERM.display, TERM.gc, TERM.fgcolor);
    } else {
        XSetForeground(TERM.display, TERM.gc, TERM.fgcolor);
        XSetBackground(TERM.display, TERM.gc, TERM.bgcolor);
    }
    XDrawImageString(
        TERM.display, TERM.window, TERM.gc,
        TERM_font_width() * x,
        TERM_font_height() * (y + 1) - TERM.font->max_bounds.descent,
        buf, 1);
    if (attr & (AttrUnderline)) {
        XDrawLine(TERM.display, TERM.window, TERM.gc,
                  TERM_font_width() * (x)    + 0,
                  TERM_font_height() * (y+1) - 1,
                  TERM_font_width() * (x+1)  - 1,
                  TERM_font_height() * (y+1) - 1);
    }
}

void x11_show_cursor(int update)
{
    TERM_attr (TERM.cursor.x, TERM.cursor.y) |= AttrCursor;
    if (update) x11_redraw (TERM.cursor.x, TERM.cursor.y);
}

void x11_hide_cursor(int update)
{
    TERM_attr (TERM.cursor.x, TERM.cursor.y) &=~ AttrCursor;
    if (update) x11_redraw (TERM.cursor.x, TERM.cursor.y);
}

void x11_scrollup(void)
{
    int res;
    int remaining = TERM_line_width() * (TERM_height() - 1);
    memcpy (TERM.buf, TERM.buf + TERM_line_width(), remaining);
    memcpy (TERM.attr, TERM.attr + TERM_line_width(), remaining);
    memset (TERM.buf + remaining, ' ', TERM_line_width());
    memset (TERM.attr + remaining, ' ', TERM_line_width());
    // if (TERM.cursor.y >= 0) { TERM.cursor.y --; x11_show_cursor(0); }
    // x11_refresh(); return;
    res = XCopyArea (
        TERM.display, TERM.window, TERM.window, TERM.gc,
        0, TERM_font_height(), /* src */
        TERM.size.width, TERM.size.height - TERM_font_height(), /* area */
        0, 0); /* dst */
    x11_perror("XCopyArea", res);
    int x; int y = TERM_height() - 1;
    for (x = 0; x < TERM_width(); x++) { x11_redraw (x, y); }
    if (TERM.cursor.y >= 0) TERM.cursor.y --;
}

void x11_putc_noflush (char c)
{
    switch (c)
    {
    case '\n':
        x11_hide_cursor(1);
        TERM.cursor.x = 0; TERM.cursor.y ++;
        if (TERM.cursor.y == TERM_height()) {
            x11_scrollup();
        }
        x11_show_cursor(1);
        break;
    default:
        TERM_buf(TERM.cursor.x, TERM.cursor.y) = c;
        TERM_attr(TERM.cursor.x, TERM.cursor.y) = TERM.cursor.attr;
        x11_redraw(TERM.cursor.x, TERM.cursor.y);
        TERM.cursor.x ++;
        if (TERM.cursor.x == TERM_width()) {
            TERM.cursor.x = 0; TERM.cursor.y ++;
        }
        if (TERM.cursor.y == TERM_height()) {
            x11_scrollup();
        }
    }
}

void x11_put_flush (void)
{
    x11_show_cursor(1);
}
void x11_putc (char c)
{
    x11_putc_noflush(c);
    x11_put_flush();
}

void x11_puts (const char *s)
{
    for(; *s; s++) { x11_putc(*s); }
    x11_put_flush();
}

void x11_wherexy (int *x, int *y)
{
    *x = TERM.cursor.x;
    *y = TERM.cursor.y;
}

static void x11_gotoxy (int new_x, int new_y)
{
    if (new_x < 0) new_x = 0;
    if (new_y < 0) new_y = 0;
    if (new_x >= TERM_width ()) new_x = TERM_width () - 1;
    if (new_y >= TERM_height ()) new_y = TERM_height () - 1;
    if (new_x != TERM.cursor.x ||
        new_y != TERM.cursor.y) {
        x11_hide_cursor(1);
        TERM.cursor.x = new_x;
        TERM.cursor.y = new_y;
        x11_show_cursor(1);
    }
}

static void x11_addxy (int x, int y)
{
    x11_gotoxy (TERM.cursor.x + x, TERM.cursor.y + y);
}

static void x11_clear (void) {
    memset(TERM.buf,  ' ', TERM_line_width() * TERM_height());
    memset(TERM.attr, ' ', TERM_line_width() * TERM_height());
    x11_show_cursor (0);
}

static void x11_clrtoeol (void) {
    int x, y = TERM.cursor.y;
    for (x = TERM.cursor.x + 1; x < TERM_line_width(); x ++) {
        TERM_buf(x, y) = ' ';
        TERM_attr(x, y) = ' ';
        x11_redraw(x, y);
    }
}

static void x11_clrtobot (void) {
    int x, y;
    for (y = TERM.cursor.y + 1; y < TERM_height(); y ++) {
        for (x = 0; x < TERM_line_width(); x ++) {
            TERM_buf(x, y) = ' ';
            TERM_attr(x, y) = ' ';
            x11_redraw(x, y);
        }
    }
}

static void x11_refresh (void) {
    int x, y;
    for (y = 0; y < TERM_height(); y ++) {
        for (x = 0; x < TERM_line_width(); x ++) {
            x11_redraw(x, y);
        }
    }
}

static void x11_bell (void) {
    XBell (TERM.display, 80);
}

void x11_interactive_terminal (void) { TERM.interactive = 1; }
void x11_system_terminal (void) { TERM.interactive = 0; }
void x11_check_winsize (int disp_width, int disp_height) {
    int term_width = disp_width / TERM_font_width();
    int term_height = disp_height / TERM_font_height();
    if (term_width != TERM_width() ||
        term_height != TERM_height()) {
        fprintf(stderr, "Resize: %ix%i -> %ix%i  (Font: %ix%i)\n",
                TERM_width(), TERM_height(),
                term_width, term_height,
                TERM_font_width(), TERM_font_height());
        TERM.size.width = disp_width;
        TERM.size.height = disp_height;
        x11_refresh();
    } else {
        // fprintf(stderr, "Resize: none\n");
    }
}
void x11_query_winsize (void) {
    if (1) return;
    int disp_width = DisplayWidth(TERM.display, TERM.window);
    int disp_height = DisplayHeight(TERM.display, TERM.window);
    x11_check_winsize(disp_width, disp_height);
}

void x11_handle_event (void) {
    switch (TERM.event.type) {
    case Expose:
        x11_refresh();
        break;
    case ConfigureNotify:
        x11_check_winsize(TERM.event.xconfigure.width,
                          TERM.event.xconfigure.height);
        break;
    case ClientMessage:
        if (TERM.event.xclient.message_type == TERM.wm_protocol) {
            if (TERM.event.xclient.format == 8 &&
                TERM.event.xclient.data.b[0] == TERM.wm_close) {
                FX (p4_bye); fprintf(stderr, "bye!\n"); break;
            }
            if (TERM.event.xclient.format == 16 &&
                TERM.event.xclient.data.s[0] == TERM.wm_close) {
                FX (p4_bye); fprintf(stderr, "bye!\n"); break;
            }
            if (TERM.event.xclient.format == 32 &&
                TERM.event.xclient.data.l[0] == TERM.wm_close) {
                FX (p4_bye); fprintf(stderr, "bye!\n"); break;
            }
            fprintf(stderr, "ClientMessage: protocol [%i %lx\n]",
                    TERM.event.xclient.format, TERM.event.xclient.data.l[0]);
        } else {
            fprintf(stderr, "ClientMessage: type %p (protocol %p)\n",
                    (void*) TERM.event.xclient.message_type,
                    (void*) TERM.wm_protocol);
        }
        break;
    case ButtonPress:
        fprintf(stderr, "ButtonPress!\n");
        // Move cursor!
        break;
    case SelectionRequest:
        fprintf(stderr, "SelectionRequest!\n");
        // copy!
        break;
    case SelectionClear:
        fprintf(stderr, "SelectionClear!\n");
        // copy!
        break;
    default:
        break;
    }
}

int x11_getvkey_from_event(void) {
    KeySym keysym = 0;
    TERM.inputlen = XLookupString(
        & TERM.event.xkey, TERM.input, sizeof(TERM.input),
        & keysym, NULL);
    if (keysym == XK_F1)     return P4_KEY_k1;
    if (keysym == XK_F2)     return P4_KEY_k2;
    if (keysym == XK_F3)     return P4_KEY_k3;
    if (keysym == XK_F4)     return P4_KEY_k4;
    if (keysym == XK_F5)     return P4_KEY_k5;
    if (keysym == XK_F6)     return P4_KEY_k6;
    if (keysym == XK_F7)     return P4_KEY_k7;
    if (keysym == XK_F8)     return P4_KEY_k8;
    if (keysym == XK_F9)     return P4_KEY_k9;
    if (keysym == XK_F10)    return P4_KEY_k0;
    if (keysym == XK_Left)   return P4_KEY_kl;
    if (keysym == XK_Right)  return P4_KEY_kr;
    if (keysym == XK_Up)     return P4_KEY_ku;
    if (keysym == XK_Down)   return P4_KEY_kd;
    if (keysym == XK_Home)   return P4_KEY_kh;
    if (keysym == XK_End)    return P4_KEY_kH;
    if (keysym == XK_Prior)  return P4_KEY_kP;
    if (keysym == XK_Next)   return P4_KEY_kN;
    if (keysym == XK_Delete) return P4_KEY_kD;
    if (keysym == XK_Insert) return P4_KEY_kI;
    if (keysym == XK_BackSpace) return '\b';
    if (keysym == XK_Tab) return '\t';
    if (keysym == XK_Return) return '\n';
    if (keysym == XK_Escape) return '\33';
    if (keysym > 256) {
        fprintf(stderr, "Keysym: %lx\n", (long) keysym);
        return 0;
    } else {
        return keysym;
    }
}

int x11_getvkey (void) {
    for (;;) {
        XNextEvent(TERM.display, & TERM.event);
        if (TERM.event.type != KeyPress) {
            x11_handle_event();
            continue;
        } else {
            int key = x11_getvkey_from_event();
            if (key) return key;
            continue;
        }
    }
}

int x11_keypressed (void) {
    while (XPending (TERM.display)) {
        XPeekEvent (TERM.display, & TERM.event);
        if (TERM.event.type == KeyPress) return 1;
        XNextEvent (TERM.display, & TERM.event);
        x11_handle_event();
    }
    return 0;
}

int x11_getkey (void) {
    for (;;) {
        int c = x11_getvkey();
        if (c < 256) return c;
    }
}

static void
x11_tput (int attr)
{
    switch (attr)
    {
    case P4_TERM_GOLEFT:	x11_addxy (-1,  0);		break;
    case P4_TERM_GORIGHT:	x11_addxy ( 1,  0);		break;
    case P4_TERM_GOUP:		x11_addxy ( 0, -1);		break;
    case P4_TERM_GODOWN:	x11_addxy ( 0,  1);		break;

    case P4_TERM_CLRSCR:	x11_clear (); x11_refresh ();	break;
    case P4_TERM_HOME:		x11_gotoxy(0, 0);		break;
    case P4_TERM_CLREOL:	x11_clrtoeol (); 		break;
    case P4_TERM_CLRDOWN:	x11_clrtobot (); 		break;
    case P4_TERM_BELL:		x11_bell ();			break;

    case P4_TERM_NORMAL:	TERM.cursor.attr = AttrNormal; 	break;
    case P4_TERM_BOLD_ON:	TERM.cursor.attr |= AttrBold;	break;
    case P4_TERM_BOLD_OFF:	TERM.cursor.attr &=~AttrBold;	break;
    case P4_TERM_BRIGHT:	TERM.cursor.attr |= AttrBold;	break;
    case P4_TERM_REVERSE:	TERM.cursor.attr |= AttrInvert;	break;
    case P4_TERM_BLINKING:	TERM.cursor.attr |= AttrInvert;	break;
    case P4_TERM_UNDERLINE_ON:	TERM.cursor.attr |= AttrUnderline; break;
    case P4_TERM_UNDERLINE_OFF:	TERM.cursor.attr &=~ AttrUnderline; break;

    default: break;
   }
}

#ifdef __GNUC__
#define INTO(x) .x =
#else
#define INTO(x)
#endif

p4_term_struct p4_term_x11 =
{
  "x11",
  0,
  0, /* no rawkeys -> use _getvkey */
  INTO(init) 		x11_prepare_terminal,
  INTO(fini) 		x11_cleanup_terminal,
  INTO(tput)		x11_tput,

  INTO(tty_interrupt_key) x11_interrupt_key,
  INTO(interactive_terminal) x11_interactive_terminal,
  INTO(system_terminal)   x11_system_terminal,
  INTO(query_winsize)     x11_query_winsize,

  INTO(c_keypressed)	x11_keypressed,
  INTO(c_getkey)	x11_getkey,
  INTO(c_putc_noflush)  x11_putc_noflush,
  INTO(c_put_flush)	x11_put_flush,
  INTO(c_putc)		x11_putc,
  INTO(c_puts)		x11_puts,
  INTO(c_gotoxy)	x11_gotoxy,
  INTO(c_wherexy)	x11_wherexy,

  INTO(c_getvkey)       x11_getvkey
};

/*@}*/
