/**
 * -- Terminal Driver for K12xx-2 Emulation System
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2003.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2008-04-20 04:46:31 $)
 *
 *  @description
 *              The Terminal Driver for K12xx-2 Emulation System,
 *              interface via the global input queue of the task
 *              and write back via an assigned reply queue. The
 *              output is run on a seperated ring-buffer and
 *              send-task so that the normal execution of an
 *              embedded PFE is not blocked by the output system.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) =
"@(#) $Id: term-k12.c,v 1.3 2008-04-20 04:46:31 guidod Exp $ ";
#endif

#define _P4_SOURCE 1
#include <pfe/pfe-sub.h>
#include <pfe/term-sub.h>

#include <pfe/os-ctype.h>
#include <pfe/os-string.h>
#include <stddef.h>
#include <unistd.h>

#include <pfe/option-ext.h>
#include <pfe/pfe-sup.h>

#if HAVE_TQLIB_H
#include <tqlib.h>
#include <pfe/term-k12.h>
#else
#include <pfe/term-k12.h>
/* we only need tQSend */
#ifndef PC
#define FAR    /* */
#define PASCAL /* */
#endif
#include <sysSymTbl.h>
#include <pfe/logging.h>
#include <pfe/def-restore.h>

#include <pfe/term-k12.h>
int tQSend (u32_t qId, void* hdr, u32_t hdrlen,
        	void* msg, u32_t msglen, int timeout);
#endif

#include <time.h>
#include <taskLib.h>
#include <inetLib.h>

#ifdef _TERMTOFRONT_ON_ABORT
# if 0
# include <K12/gemapi.h>
# else
# include <gemapi.h>
# endif
#endif

/* two defines stolen from gemrpc.h which is not included here */
#ifndef FORTH_MAX_MESSAGE_SIZE
#define FORTH_MAX_MESSAGE_SIZE     0x200
#endif
#ifndef FORTH_NBR_MESSAGES
#define FORTH_NBR_MESSAGES     4
#endif

#define OUTBUF_SIZE (FORTH_MAX_MESSAGE_SIZE)
#define OUTBUF_RING (4*80*25) /* four screenful */
static long outring_size = 0; /* four screenful */

#define DATA_SAP K12_FORTH_COMMAND_SAP
         /* has to be in consensus w/ main-k12.c */

#define K12PRIV(P) P4_K12_PRIV(P)


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




static int
logPrintf(void *pEmul, char *fmt, ...)
{
    va_list argptr;
    int ret;

    va_start(argptr, fmt);

    ret = vfprintf(stdout, fmt, argptr);
    fflush(stdout);

    va_end(argptr);
    return(ret);
}

static char hexdigit[] = "0123456789ABCDEF";

#define LOGSTRING_BUFFER 600
static void
k12LogString (int mask, char* hail, char* p, int size)
{
    int  i, j=0;
    char v[LOGSTRING_BUFFER];
    for (i=0; i<size; i++)
    {
        if (p[i] >= 0x20 && p[i] < 0x7F)
            v[j++] = p[i];
        else
            switch (p[i])
            {
             case '\n': v[j++] = '\\'; v[j++] = 'n'; break;
             case '\r': v[j++] = '\\'; v[j++] = 'r'; break;
             case '\v': v[j++] = '\\'; v[j++] = 'v'; break;
             case '\t': v[j++] = '\\'; v[j++] = 't'; break;
             case '\f': v[j++] = '\\'; v[j++] = 'f'; break;
             case '\b': v[j++] = '\\'; v[j++] = 'b'; break;
             case '\a': v[j++] = '\\'; v[j++] = 'a'; break;
             case '\\': v[j++] = '\\'; v[j++] = '\\'; break;
             case '\'': v[j++] = '\\'; v[j++] = '\''; break;
             case '\"': v[j++] = '\\'; v[j++] = '\"'; break;
             case '\33': v[j++] = '\\'; v[j++] = 'e'; break;
             default:
                 if ((unsigned char)(p[i]) <= 9)
                 { v[j++] = '\\'; v[j++] = '0'+p[i]; }
                 else
                 { v[j++] = '\\'; v[j++] = 'x';
                 v[j++] = hexdigit [(p[i]>>4) &15];
                 v[j++] = hexdigit [p[i] &15];
                 }
            }
        if (j > LOGSTRING_BUFFER-3) { v[j++]='\\'; v[j++]='+'; break;}
    }
    v[j] = 0;
    /* K12LogMsg1 (level, hail, "\"%s\"\n", v); */
    { if(K12LogGrpMatch(mask))
        k12LogMsg(mask, "%s \"%s\" (%i chars)\n",
          (u32_t)hail, (u32_t)(v), size, 0, 0, 0);
    }

}

static k12_status_t
write_link (k12_emu_type_t* emu, k12_emu_event_t link, int type,
            s8_t* buf, u32_t n)
{
    /* messages have the format seen on the stdin-link. If you know the
     * the stdin-link id of a pfe-box then you can connect the output of
     * one pfe box with the input of another box if you dare to.
     */
    register k12_status_t e;
    auto k12_emu_msg_subhead_t* msg;
    e = k12EmuLowBufferGet(emu, n + sizeof(*msg), (s8_t**)(&msg));
    if (e) return e;
    msg->type = type; /* K12_EMU_DATA_REQ or K12_EMU_DATA_IND */
    msg->para0 = n; (void*)msg->handle = emu;
    msg->para1 = 0; msg->para2 = 0;
    p4_memcpy (& msg[1], buf, n);
    return k12EmuLowEventSend (emu, link, (s8_t*)(msg), n + sizeof(*msg),
                               K12_EMU_NOOPT);
}

/* ************************************************ */
/* output thread */

#define OUTPUT_TQSEND_TIMEOUT 100 /*ticks, so this is 1..2 seconds */
/* if the terminal is closed while we are blocked in tqsend, we'll
   stop forever. No error will be returned if the receiver task exits!
*/

int
p4_TX_task (k12_priv* p)
{
    char buf[OUTBUF_SIZE];
    int n = 0;

  /* VxWorks Reference says:
   *  semDelete: "any pended task will unblock and return ERROR"
   *  semTake: "ERROR if the semaphore ID is invalid or the task timed out"
   *  taskSafe: "Tasks that attempt to delete a protected task
   *                   will block until the task is made unsafe"
   *  ie. to kill this task, call
   *      semDelete(p->tx_sem); taskDelete(tx_task);
   *  _in_this_order_ !!
   */

    taskSafe (); /* can't delete unless semaphore invalidated */
    while (semTake (p->tx_sem, WAIT_FOREVER) == OK) /* wait for */
    {
        if (p->tx_quit) goto quit;

        if (n && rngNBytes (p->tx_ring) < OUTBUF_SIZE-n) /* append case */
        {
            n += rngBufGet (p->tx_ring, buf+n, OUTBUF_SIZE-n);
        } else {                                      /* replace case */
            n = rngBufGet (p->tx_ring, buf, OUTBUF_SIZE);
        }

        K12LogMsg2 (K12_LOG_DEBUG_L(11), "send", "qid=%08lx (k12p=%08lx)\n",
          (long)p->tx_qid, (long)p);

        if (n && p->tx_qid)
        {
            do {
                if (p->tx_link)
                    write_link (p->emu, p->tx_link, K12_EMU_DATA_REQ, buf, n);

                if (tQSend (p->tx_qid, 0, 0, buf, n, OUTPUT_TQSEND_TIMEOUT)
                  == OK)
                {
                    if (p->tx_logfile)
                        write (p->tx_logfile, buf, n);
                    k12LogString (K12_LOG_DEBUG_L(15),
                                  "sent>", buf, n);
                }
                else
                    k12LogString (K12_LOG_DEBUG_L(15),
                                  "sent> FAIL at ", buf, n);
                n = 0;

                if (p->tx_quit) goto quit;

                if (rngNBytes (p->tx_ring))
                    n = rngBufGet (p->tx_ring, buf, OUTBUF_SIZE);
            } while (n);
        } else { /* offline */
            if (p->tx_link)
                write_link (p->emu, p->tx_link, K12_EMU_DATA_REQ, buf, n);
            if (p->tx_logfile)
                write (p->tx_logfile, buf, n);
            if (n > 32)
                k12LogString (K12_LOG_DEBUG_L(15), "hold> ...", buf+n-32, 32);
            else
                k12LogString (K12_LOG_DEBUG_L(15), "hold>", buf, n);
        }
        if (p->tx_quit) goto quit;
    } /*while semTake*/
 quit:
    P4_debug(13, "exit tx-task complete");
    return taskUnsafe (); /*OK*/
}

/* ************************************************ */

static char const *
term_k12_rawkey_string [P4_NUM_KEYS]  = /* what function keys send */
{
    /* Terminal Application sends VT100 key sequences (hopefully) */
    "\033OP",   "\033OQ",   "\033OR",   "\033OS",
    "\033[15~", "\033[17~", "\033[18~", "\033[19~",
    "\033[20~", "\033[21~", "\033[22~", "\033[23~",
    "\033[24~", "\033[25~", "\033[26~", "\033[27~",
    "\033[28~", "\033[29~", "\033[30~", "\033[31~",

    "\033OD",  "\033OC",    "\033OA",   "\033OB", /* le re up do */

    "\033[1~",  "\033[4~", "\033[6~",   "\033[5~", /* kh kH kN kP */
    "\b",       "\033[3~",  NULL,       "\033[2~", /* kb kD kM kI */
    NULL,       NULL,       NULL,       NULL,
};

static char const *
term_k12_control_string[] = /* vt100 sequences. */
{
    "\033[%d;%dH",                /* cm - cursor move */
    "\033[H",                     /* ho - home position */

    "\033[D",                     /* le - cursor left */
    "\033[C",                     /* nd - right one column */
    "\033[A",                     /* up - up one column */
    "\033[B",                     /* do - down one column */

    "\033[H\033[2J",              /* cl - clear screen and home */
    "\033[J",                     /* cd - clear down */
    "\033[K",                     /* ce - clear to end of line */
    "\a",                         /* bl - bell */

    "\033[P",                     /* dc - delete character in line */
    "\033[M",                     /* dl - delete line from screen */

    "\033D",                      /* sf - scroll screen up */
    "\033M",                      /* sr - scroll screen down */

    "\033[7m",                    /* so - enter standout mode */
    "\033[m",                     /* se - leave standout mode */
    "\033[4m",                    /* us - turn on underline mode */
    "\033[m",                     /* ue - turn off underline mode */

    "\033[1m",                    /* md - enter double bright mode */
    "\033[7m",                    /* mr - enter reverse video mode */
    "\033[5m",                    /* mb - enter blinking mode */
    "\033[m",                     /* me - turn off all appearance modes */

    "\033[?1h\033=",              /* ks - make function keys transmit */
    "\033[?1l\033>"               /* ke - make function keys work locally */
};

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

static char const parmdesc[] =
"cmv\0hom\0cul\0cur\0cuu\0cud\0"
"cls\0cld\0cel\0bel\0"
"dch\0dln\0scu\0scd\0"
"bf1\0bf0\0ul1\0ul0\0"
"bri\0rev\0blk\0nrm\0"
"fk1\0fk0\0" ;

#define T_PARM(cap) (PFE.control_string[cap])

static void
t_puts (int cap, int n)
{
    K12LogMsg2 (K12_LOG_DEBUG_L(11), "cap>", "%s (%i) \n",
      (long)(parmdesc+4*cap), (long)cap);

    c_puts (T_PARM(cap));
}

static int c_interrupt_key (char ch)		{ return 0; }
static void c_interactive_terminal (void)	{}
static void c_system_terminal (void)		{}
static void c_query_winsize (void)		{}

static void
tx_task_spawn (k12_priv* k12p)
{
    int prio=0;
    char* taskname = taskName(taskIdSelf());
#    ifdef P4_UPPER_REGS /* i960 */
    P4_CALLER_MKSAVED
#    endif

    /* avoiding prio inversion by making the output_thread of
       higher-priority. If the output_thread is blocked in
       tx_sem, each output in p4_put_flush will implicitly
        preempt the p4th-thread and result in a tQSend there,
        but if the output_thread is blocked in tQSend we can
        fill the buffer in between. For now, it works. *gd*
        news: no higher-priority anymore, but increased the ringbuf
        size to be able to hold the double of a screenful to
        avoid loosing too much if the TX_task is too slow
    */
    {  /* C99: dynamic-sized auto-fields, no alloca needed, e.g. gcc 2.95.2 */
        char tasknameTX[p4_strlen(taskname)+4];
        sprintf (tasknameTX, "%s:TX", taskname);

        taskPriorityGet (taskIdSelf(), &prio);
        k12p->tx_task = taskSpawn (/*name*/ tasknameTX,
                                   prio /* ? prio-1 : prio */,
                                   0, /*opt*/
                                   OUTBUF_SIZE + 4096, /*stack*/
                                   p4_TX_task, /* thread-entry */
                                   (int) k12p, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    }
#    ifdef P4_UPPER_REGS
    P4_CALLER_RESTORE
#    endif
}

static void
tx_task_check (k12_priv* k12p)
{
    register WIND_TCB* tcb;

    if ((tcb=taskTcb (k12p->tx_task)) == NULL /*invalid task-id*/
      ||   tcb->entry != p4_TX_task)            /*not one of mine*/
    {  /* well, output-task has been killed, need to respawn */
        register int old_task_id = k12p->tx_task;
        tx_task_spawn (k12p);
        if (k12p->tx_task == ERROR) {
            P4_fail1 ("re-spawn:TX: old=0%x new=ERROR", old_task_id);
        }else{
            P4_warn2 ("re-spawn:TX: old 0%x new 0%x", old_task_id, k12p->tx_task);
        }
    }
}

#ifdef _TERMTOFRONT_ON_ABORT
/* this is a copy from the k12termtofront.module */
static void FXCode (p4_k12TermToFront_on_abort)
{
# if 0
    extern status_t k12GemStubTerminalWnd(uint32_t ulGemId,
                               char *pEmulName,      /* emulation name */
                               uint32_t ulAction );  /* action: hide/pop up */
# endif
    extern status_t k12EmuLowEmulNameGet(k12_emu_type_t *EmulId,
                                         char **EmulName);

    register int e;
    register k12_emu_type_t * emul_id;
    auto uint32_t stackid;
    auto char* emulname;

    emul_id = P4_K12_PRIV(p4TH)->emu;

    e = k12EmuLowPipeIdGet (emul_id, &stackid);
    if (e) { P4_warn1 ("k12EmuLowPipeIdGet failed (%x)", e); return; }

    e = k12EmuLowEmulNameGet (emul_id, &emulname);
    if (e) { P4_warn1 ("k12EmuLowNameGet failed (%x)", e); return; }

    e = k12GemStubTerminalWnd (stackid, emulname, K12_EMU_GEM_TERM_WND_POPUP);
    if (e) { P4_warn1 ("k12GemStubTerminalWnd failed (%x)", e); return; }
}
#endif

/*
 * the abort-cleanups are also run with COLD - this function here should
 * actullay hook into a THROW list and then be executed.
 */
static FCode(p4_paintred_on_abort)
{
    k12_priv* k12p = K12PRIV(p4TH);

    if (k12p->validation_req)
        return; /* in setup-phase : QUIT calls ABORT[*] followed by QUERY */

# ifdef _TERMTOFRONT_ON_ABORT
    if (k12p->state == K12_EMU_NOT_LOADED)
    { FX (p4_k12TermToFront_on_abort); }
# endif

    k12p->state = K12_EMU_ERROR_CASE;
    if (PFE.tib && ! k12p->answering)
    {
        p4_strncpy (k12p->answerbuf, PFE.tib, K12_ANSWERBUF_SIZE);
        k12p->answerwarning = p4_strlen (k12p->answerbuf);
    }else{
        k12p->answerwarning = 0;
    }
}

static int
c_prepare_terminal (void)
{
    k12_priv* k12p = K12PRIV(p4TH);

    /* setbuf (stdout, NULL); */

    if (!k12p->tx_ring)
    {
        if (! outring_size)
        {
            outring_size = p4_search_option_value ("/outring", 8, 0, PFE.set);
        }
        if (! outring_size)
        {
            static const char _RINGSIZE[] = "p4_outring_size";
            auto long* val = 0;

            if (symFindByName (sysSymTbl, (char*) _RINGSIZE,
                               (char**) &val, 0) == OK
                && val
                && (80*25) < (*val) && (*val) < (100* 80*25)
                )
                outring_size = (*val);
            if (! outring_size)
                outring_size = OUTBUF_RING;
            P4_info2 ("outring_size = %ld (p4_outring_size = %ld)",
                      outring_size, val ? *val : 0);
        }

        k12p->tx_ring = rngCreate (outring_size);
        if (!k12p->tx_ring) return 0;

        k12p->tx_sem = semBCreate (0,0);
        if (!k12p->tx_sem) {
            rngDelete (k12p->tx_ring); return 0;
        }

        tx_task_spawn (k12p);

        if (k12p->tx_task == ERROR) {
            semDelete (k12p->tx_sem);
            rngDelete (k12p->tx_ring); return 0;
        }
    }

    PFE.abort[2] = PFX(p4_paintred_on_abort); /* abusing abort_float vector */

    k12p->rx_dataSAP = DATA_SAP;
    return 1;
}

static void
c_cleanup_terminal (void)
{
    k12_priv* k12p = K12PRIV(p4TH);

    if (k12p->tx_ring)
    {
        P4_debug (13, "cleanup term TX-task <start>");
        k12p->tx_quit = 1; /* asserts that tx_task will not take tx_sem */
        semFlush (k12p->tx_sem); /* releases tx_task if had already taken it */
        semDelete (k12p->tx_sem);   /* invalidate TX-semaphore */
        taskDelete (k12p->tx_task); /* block on delete-safe TX-thread */
        rngDelete (k12p->tx_ring);  /* dump mutual TX-buffer */
        P4_debug (13, "cleanup term TX-task </done>");
        k12p->tx_ring = 0;
    }
    if (k12p->tx_logfile)
    {
        close (k12p->tx_logfile);
        k12p->tx_logfile = 0;
    }
}

void p4_set_fkey_name (int num, char* name);

#define FORTHTYPE(_TYPE_) (0x4e00 < (_TYPE_) && (_TYPE_) < 0x5000)

/* handle incoming events until a terminal' key-event is received */
static int
p4_k12_wait_for_stdin ()
{
    register k12_priv* k12p = K12PRIV(p4TH);

    if (k12p->answering && k12p->frm_putback == '\n')
    {   /*              && k12p->frm_input == k12p->rx_link */
        k12p->rx_data = "\n";
        k12p->rx_datalen = 1;
        k12p->rx_dataIN = 0;
        k12p->frm_putback = 0;
        return 1;
    }else{
        k12p->answering = FALSE;
        k12p->frm_putback = 0;
        /*fallthrough*/
    }

    if (k12p->state == K12_EMU_NOT_LOADED && ! k12p->validation_req)
        k12p->state = K12_EMU_IDLE;

    while (1)
    {
        if (k12p->frm_putback) { k12p->frm_putback = 0; }
        else{
            k12EmuLowEventGet (k12p->emu,
              &k12p->frm_input,
              &k12p->frm_data,
              &k12p->frm_datalen,
              &k12p->frm_option);
            K12LogMsg2 (K12_LOG_DEBUG_L(0),
              "got>", "event via %i len %i\n",
              k12p->frm_input, k12p->frm_datalen);
        }
        if (k12p->rx_dataSAP == k12p->frm_input
            || (k12p->rx_link && k12p->rx_link == k12p->frm_input))
        {
            register k12_emu_msg_subhead_t* msg = (void*)k12p->frm_data;
            switch (msg->type)
            {
             case K12_EMU_VALIDATION_REQ:
                 if (k12p->validation_req)
                     k12p->validation_req --;
                 if (k12p->state == K12_EMU_NOT_LOADED)
                     k12p->state = K12_EMU_IDLE;
                 break;
             case K12_EMU_DATA_REQ:
                 K12LogMsg2 ( K12_LOG_DEBUG_L(11), "got>",
                              "K12_EMU_DATA_REQ(%i) len=%i\n",
                   msg->type, k12p->frm_datalen - sizeof(*msg));
                 k12p->rx_data = k12p->frm_data + sizeof(*msg);
                 k12p->rx_datalen = k12p->frm_datalen - sizeof(*msg);
                 k12p->rx_dataIN = 0;
                 k12LogString(K12_LOG_DEBUG_L(10),
                   "got>", k12p->rx_data, k12p->rx_datalen);

                 if (! k12p->rx_datalen || ! k12p->rx_data[0])
                 {
                     /* the k12-terminal may send a flush request
                        as being an empty key buffer, so flush now.
                     */
                     if (k12p->bufidx)       /* | quick path: p4_put_flush */
                         p4_put_flush ();    /* | has the same if-check */

                     /* and the c_keypressed routine may send an empty
                        key buffer to detect a queue without a key-event
                        - is will also set ->para2
                     */
                     if (msg->para2)
                         return 0;
                 }else{
                     /* there are new chars to be handled (will go to TIB) */

                     /* see new p4_paintred_on_abort - paint good now */
                     if (k12p->state == K12_EMU_ERROR_CASE ||
                         k12p->state == K12_EMU_WARNING)
                         k12p->state = K12_EMU_IDLE;
                     return 1;
                 }
                 break;
             case K12_EMU_XDAT_REQ: /* command/answer facility */
                 K12LogMsg2 (K12_LOG_DEBUG_L(11),
                   "got>", "K12_EMU_XDAT_REQ(%i) len=%i\n",
                   msg->type, k12p->frm_datalen - sizeof(*msg));
                 k12p->rx_data = k12p->frm_data + sizeof(*msg);
                 k12p->rx_datalen = k12p->frm_datalen - sizeof(*msg);
                 k12p->rx_dataIN = 0;
                 k12LogString (K12_LOG_DEBUG_L(10),
                   "cmd>", k12p->rx_data, k12p->rx_datalen);

                 k12p->answering = TRUE;
                 if (!k12p->rx_datalen || !k12p->rx_data[0])
                 { /* empty input - old fixup to frm_putback problems */
                     k12p->rx_data = "\n";
                     k12p->rx_datalen = 1;
                     k12p->rx_dataIN = 0;
                 }
                 /* -> start command/answer facility */
                 p4_strcpy (k12p->answerbuf, "\\ ");
                 k12p->answeridx = p4_strlen (k12p->answerbuf);

                 /* see new p4_paintred_on_abort - paint partly good now */
                 if (k12p->state == K12_EMU_ERROR_CASE)
                     k12p->state = K12_EMU_WARNING;
                 return 1;
             case K12_EMU_CONN_REQ:
                 K12LogMsg2 (K12_LOG_DEBUG_L(11),
                   "got>", "K12_EMU_CONN_REQ(%i) handle=%i\n",
                   msg->type, msg->handle);
                 k12p->rx_data = 0;
                 k12p->rx_datalen = k12p->frm_datalen;
                 k12p->rx_dataIN = 0;
#              if defined HOST_CPU_I960 || defined __target_cpu_i960
                 /* FIXME: i960 K12xx LEM pre-beta had a byteswap */
                 if (!(msg->handle&1))
                 {
                     msg->handle = ntohl(msg->handle);
                     K12LogMsg1 (K12_LOG_SEVERE,
                       "got>",
                       "PANIC: byte-swapping queue id (now %08x) !!!!!!!!\n",
                       msg->handle);
                 }
#	       endif
                 if (!(msg->handle&1)) /* FIXME: */
                 {
                     K12LogMsg1 (K12_LOG_SEVERE,
                       "got>",
                       "PANIC: can't be a global queue id !!!!!!!! (%08x)\n",
                       msg->handle);
                     break;
                 }
                 k12p->tx_qid = msg->handle;
                 K12LogMsg1 (K12_LOG_INFO, "got:",
                   "terminal opened (tqid %08lx)\n",
                   (long)k12p->tx_qid);
                 {  /* sending function key names as response */
                     int i;
                     for (i=0; i<K12_TERM_FKKEYS_MAX; i++)
                     {
                         p4_set_fkey_name (i, 0);
                     }
                 }
                 break;
             case K12_EMU_DISC_REQ:
                 K12LogMsg2 (K12_LOG_DEBUG_L(11),
                   "got>", "K12_EMU_DISC_REQ(%i) handle=%i\n",
                   msg->type, msg->handle);
                 k12p->rx_data = 0;
                 k12p->rx_datalen = k12p->frm_datalen;
                 k12p->rx_dataIN = 0;
                 k12p->tx_qid = msg->handle;         /* should be null */
                 K12LogMsg1 (K12_LOG_INFO, "got:", "terminal closed (%08lx)\n",
                   (long)k12p->tx_qid);
                 break;
             default:
                 K12LogMsg2 (K12_LOG_DEBUG_L(11),
                   "got>"," K12_EMU_UnKnOwN(%i) handle=%i;\n",
                   msg->type, msg->handle);

                 if (k12p->eventHook && FORTHTYPE(msg->type))
                 { (*k12p->eventHook)(k12p->frm_input,
                                      k12p->frm_data, k12p->frm_datalen);
                 }
            }
        }
        else if (k12p->qx_link && k12p->qx_link == k12p->frm_input)
        {
            register k12_emu_msg_subhead_t* msg = (void*)k12p->frm_data;
            switch (msg->type)
            {
             case K12_EMU_DATA_REQ: /* command/answer facility */
                 K12LogMsg2 (K12_LOG_DEBUG_L(11),
                   "got>", "K12_EMU_DATA_REQ(%i) len=%i to-forth\n",
                   msg->type, k12p->frm_datalen - sizeof(*msg));
                 k12p->rx_data = k12p->frm_data + sizeof(*msg);
                 k12p->rx_datalen = k12p->frm_datalen - sizeof(*msg);
                 k12p->rx_dataIN = 0;
                 k12LogString (K12_LOG_DEBUG_L(10),
                   "cmd>", k12p->rx_data, k12p->rx_datalen);

                 k12p->answering = TRUE;
                 /* -> start command/answer facility */
                 k12p->answeridx = 0;
                 k12p->frm_putback = '\n';
                 return 1;
             default:
                 K12LogMsg2 (K12_LOG_DEBUG_L(11),
                   "got>"," K12_EMU_UnKnOwN(%i) handle=%i;\n",
                   msg->type, msg->handle);

                 if (k12p->eventHook && FORTHTYPE(msg->type))
                 { (*k12p->eventHook)(k12p->frm_input,
                                      k12p->frm_data, k12p->frm_datalen);
                 }
            }
        }else{
            if (K12_EMU_VALIDATION_REQ ==
                ((k12_emu_msg_subhead_t*)k12p->frm_data)->type)
            {
                 if (k12p->validation_req)
                     k12p->validation_req --;
                 if (k12p->state == K12_EMU_NOT_LOADED)
                     k12p->state = K12_EMU_IDLE;
            }
            else if (k12p->eventHook)
            {
                if ( (*k12p->eventHook)(k12p->frm_input,
                                        k12p->frm_data, k12p->frm_datalen))
                { continue; } /* else unhandled message */
            }else{
                K12LogMsg1 (K12_LOG_WARNING,
                            "got>", "unknown input - skipped event via %i\n",
                            k12p->frm_input);
            }
        }
    }
#   undef _sizeof_subhead_t
}

/* #define NOCH ((int)0xdeadc4a3) */
#define NOCH 0

static int
c_keypressed (void)
{
    status_t e;
    int k12_sap;
    k12_emu_msg_subhead_t* buf;
    time_t sec;
    k12_priv* k12p = K12PRIV(p4TH);

    /* part A: check if there is a putback/onhold char */
    if (k12p->nxch != NOCH)
        return 1;

    /* since k12xx eventsys can not scan into the input queue
       for some kind of type (like an X11 eventqueue would do)
       we will just go to handle all request. If the event-queue
       had some events, but no key-events, the getevent routine
       would go blocking. Therefore, we feed an empty KEY-event
       to the end of the event-queue so it will definitly return
       here, either with the actual key-buffer or with that empty
       key-buffer (length zero) that we will generate now.
    */

    /* part B: check if the last key buffer is not yet empty */
    if (k12p->rx_data && k12p->rx_dataIN < k12p->rx_datalen)
    {
        return 1;
    }

    /* part C: get new event-buffer, and add it to our event queue */

    if (! FENCE)
        k12_sap = K12_FORTH_COMMAND_SAP;
    else
        k12_sap = k12p->rx_dataSAP;

    e=k12EmuLowBufferGet (k12p->emu, sizeof(*buf), (char**) &buf);
    if (e) {
        P4_fail ("cannot make term nonblocking "
          "because timeout buffer could not be allocated");
        return 0;
    }
    p4_memset (buf, 0, sizeof(*buf));
    buf->type = K12_EMU_DATA_REQ;
    buf->para2 = (u32_t) buf;

    e=k12EmuLowEventPut (k12p->emu, k12_sap,
      (char*) buf, sizeof(*buf), K12_EMU_NOOPT);
    if (e) {
        P4_fail ("cannot make term nonblocking "
          "because timeout buffer could not be allocated");
        return 0;
    }

    sec = time (0); /*let the loop terminate even if our event was lost*/
    do
    {
        /* part D: go to handle requests on the event-queue */
        if (PFE.wait_for_stdin)
            (*PFE.wait_for_stdin) ();
        else
        p4_k12_wait_for_stdin ();

        /* part E: check again if there is a non-empty key-buffer now */
        if (k12p->rx_data && k12p->rx_dataIN < k12p->rx_datalen)
        {
            return 1;
        }

        /* no new keys in event-buffer, but a key flushrequest received.
           Now check that this flushrequest is the one issued by this
           call to c_keypressed. Then return a "no new keys" flag == false;
        */
        if (((k12_emu_msg_subhead_t*)k12p->frm_data)->para2 == (u32_t) buf)
            return 0;

        /* There was an old flush request, go again to the getevent loop */
    } while (time (0) - sec <= 1);

    /* ouch, no key event received in about one..two seconds, and we did
       not see our flushrequest in that time either. Better return now.
    */
    return 0;
}

static int
c_getkey (void)
{
    k12_priv* k12p = K12PRIV(p4TH);

    if (k12p->nxch != NOCH) {
        int ch = k12p->nxch;

        k12p->nxch = NOCH;
        return ch;
    }

    /* loop until a non-empty key-buffer has been received */
    while (1)
    {
        /* check if there is a non-empty key-buffer */
        if (k12p->rx_data && k12p->rx_dataIN < k12p->rx_datalen)
        {
            register int ch = k12p->rx_data[k12p->rx_dataIN++];
            return ch;
        }else{
            /* handle requests on the event-queue and try again */
            if (PFE.wait_for_stdin)
                (*PFE.wait_for_stdin) ();
            else
                p4_k12_wait_for_stdin ();
        }
    }
}

/************************************************************************/
/* Output to screen, control with termcap:                              */
/************************************************************************/

static void
c_put_flush (void)
{
    k12_priv* k12p = K12PRIV(p4TH);

    if (!k12p->bufidx) return;

    /* fflush (stdout); */
    k12LogString (K12_LOG_DEBUG_L(11), "put>", k12p->buffer, k12p->bufidx);

    rngBufPut (k12p->tx_ring, k12p->buffer, k12p->bufidx);
    if (rngNBytes (k12p->tx_ring) > outring_size-256) {
        if (rngIsFull (k12p->tx_ring)) {
            K12LogMsg (K12_LOG_DEBUG_L(11),
              "put>", "WARN: tx_ring is full (overflow) \n");
        }else{
            K12LogMsg (K12_LOG_DEBUG_L(11),
              "put>", "NOTE: tx_ring highwater \n");
        }
    }

    tx_task_check (k12p);
    semGive (k12p->tx_sem);

    if (! k12p->answering) goto done;
    /* -------------------------------------- command/answer facility ----- */
    if (k12p->qx_link)
    { /* line mode on qx_link for terminal output scanning as for TMS */
        register int i;
        if (!p4_isprint(*k12p->buffer) && !p4_isspace(*k12p->buffer)) /* esc-seq */
        {
            if (k12p->answeridx)
            {
                write_link (k12p->emu, k12p->qx_link, K12_EMU_DATA_IND,
                            k12p->answerbuf, k12p->answeridx);
                k12p->answeridx = 0;
            }

            write_link (k12p->emu, k12p->qx_link, K12_EMU_DATA_IND,
                        k12p->buffer, k12p->bufidx);
            goto done;
        }

        for (i=0; i < k12p->bufidx; i++)
        {
            register unsigned char c;
            if (k12p->answeridx >= K12_ANSWERBUF_SIZE-1)
            {
                write_link (k12p->emu, k12p->qx_link, K12_EMU_DATA_IND,
                            k12p->answerbuf, k12p->answeridx);
                k12p->answeridx = 0;
            }

            c = k12p->buffer[i];
            if (p4_isprint (c))
            {
                k12p->answerbuf[k12p->answeridx++] = c;
            }else
            {
                if (k12p->answeridx)
                {
                    write_link (k12p->emu, k12p->qx_link, K12_EMU_DATA_IND,
                                k12p->answerbuf, k12p->answeridx);
                    k12p->answeridx = 0;
                }
                if (c == '\n')
                    write_link (k12p->emu, k12p->qx_link, K12_EMU_DATA_IND,
                                "\n", 1);
                else
                    k12p->answerbuf[k12p->answeridx++] = c;
            }
        }

        if (k12p->answeridx && !p4_isprint(*k12p->answerbuf))
        { /* send controls always, including newlines at end of line */
            write_link (k12p->emu, k12p->qx_link, K12_EMU_DATA_IND,
                        k12p->answerbuf, k12p->answeridx);
            k12p->answeridx = 0;
        }
        // goto done;
    }
    else  if (k12p->answeridx < K12_ANSWERBUF_SIZE-1)
    { /* answer to emu-config commandline only when no qx_link command mode */
        register int i;
        register unsigned char c;
        for (i=0; i < k12p->bufidx; i++)
        {
            if (k12p->answeridx >= K12_ANSWERBUF_SIZE-1)
                break;

            c = k12p->buffer[i];
            if (p4_isprint (c) && ! p4_iscntrl (c))
                k12p->answerbuf[k12p->answeridx++] = c;
            else
                k12p->answerbuf[k12p->answeridx++] = '\t';
        }
        k12p->answerbuf[k12p->answeridx] = '\0';
        k12p->answerwarning = 0;
    }

 done:
    /* ->bufidx == 0 will mark it as "flushed" -> see wait_for_stdin */
    k12p->bufidx = 0; *k12p->buffer = 0;
}

static void
c_putchar (char c)
{
    k12_priv* k12p = K12PRIV(p4TH);

    k12p->buffer[(int)(k12p->bufidx++)] = c;
    if (k12p->bufidx >= sizeof (k12p->buffer)) { c_put_flush (); }
    k12p->buffer[(int)(k12p->bufidx)] = 0;
}

static void				/* output character and */
c_putc_noflush (char c)		/* trace the cursor position */
{
    k12_priv* k12p = K12PRIV(p4TH);

    c_putchar (c);
    switch (c)
    {
     case '\a':			/* bell, no change of cursor position */
         break;
     case '\b':			/* backspace, move cursor left */
         if (k12p->col > 0)
             k12p->col--;
         break;
     case '\r':			/* carriage return, ->column 0 */
         k12p->col = 0;
         break;
     default:			/* ordinary character: */
         if (k12p->col < P4_opt.cols - 1)	/* at right edge of screen? */
         {
             k12p->col++;		/* no: increment column */
             break;
         }			/* yes: like line feed */
     case '\n':			/* line feed */
         k12p->col = 0;
         if (k12p->row < P4_opt.rows - 1)/* if not at bottom of screen: */
             k12p->row++;		 /* increment row */
    }				/* otherwise terminal is supposed to scroll */
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
    char b[16];
    k12_priv* k12p = K12PRIV(p4TH);

    /* tputs (tparm (cursor_address, y, x), 1, t_putc); */
    sprintf (b, T_PARM(cursor_address), y, x);
    p4_puts (b);
    k12p->col = x;
    k12p->row = y;
}

static void
c_wherexy (int *x, int *y)
{
    k12_priv* k12p = K12PRIV(p4TH);

    *x = k12p->col;
    *y = k12p->row;
}

static void
c_tput (int attr)
{
    k12_priv* k12p = K12PRIV(p4TH);

    switch (attr)
    {
     case P4_TERM_GOLEFT:	t_puts (cursor_left,  0); --k12p->col; break;
     case P4_TERM_GORIGHT:	t_puts (cursor_right, 0); ++k12p->col; break;
     case P4_TERM_GOUP:		t_puts (cursor_up,    0); --k12p->row; break;
     case P4_TERM_GODOWN:	t_puts (cursor_down,  0); ++k12p->row; break;

     case P4_TERM_CLRSCR:	t_puts (clear_screen, PFE.rows); /*->HOME*/
     case P4_TERM_HOME:		t_puts (cursor_home, 1);
         			k12p->row = k12p->col = 0; break;
     case P4_TERM_CLREOL:	t_puts (clr_eol, 1); break;
     case P4_TERM_CLRDOWN:	t_puts (clr_eos, PFE.rows - k12p->row); break;
     case P4_TERM_BELL:		t_puts (bell, 0); break;

     case P4_TERM_NORMAL:	t_puts (exit_attribute_mode, 0); break;
     case P4_TERM_BOLD_ON:	t_puts (enter_standout_mode, 0); break;
     case P4_TERM_BOLD_OFF:	t_puts (exit_standout_mode, 0); break;
     case P4_TERM_UNDERLINE_ON:	t_puts (enter_underline_mode, 0); break;
     case P4_TERM_UNDERLINE_OFF: t_puts (exit_underline_mode, 0); break;
     case P4_TERM_BRIGHT:	t_puts (enter_bold_mode, 0); break;
     case P4_TERM_REVERSE:	t_puts (enter_reverse_mode, 0); break;
     case P4_TERM_BLINKING:	t_puts (enter_blink_mode, 0); break;
     default: break;
    }
}

void
p4_set_fkey_name (int num, const char* name)
{
    char buffer[24];
    k12_priv* k12p = K12PRIV(p4TH);


    if (name)
    {   /* memorize the string - it's gotten send on each term (re-)open */
        p4_strncpy (k12p->f[num], name, K12_TERM_FKNAME_MAX);
        k12p->f[num][K12_TERM_FKNAME_MAX-1] = 0;
    }

    if (!k12p->tx_qid)
        return; /* don't send anything if no terminal open */

    if (!k12p->f[num][0])
        sprintf (k12p->f[num], "F%i", num);

    sprintf (buffer, "\033!f%02i:%s\r", num, k12p->f[num]);

    c_puts (buffer); c_put_flush ();
};

#ifdef USE_TERMCAP
static char const tckeycode[][3] =
{
    "k1", "k2", "k3", "k4", "k5",	/* keys in same order as enumkeycode */
    "k6", "k7", "k8", "k9", "k0",	/* from term.h */
    "F1", "F2", "F3", "F4", "F5",
    "F6", "F7", "F8", "F9", "FA",
    "kl", "kr", "ku", "kd",	/* result is just what has to be exported */
    "kh", "kH", "kN", "kP",	/* via variable rawkey_string */
    "kb", "kD", "kM", "kI",
    "kA", "kE", "kL", "kC"
};
#endif /* USE_TERMCAP */

#if defined USE_TERMCAP || defined USE_TERMINFO
static char const tcctlcode[][3] =
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

#if __STDC_VERSION__ > 199900L || (__GNUC__*100+__GNUC_MINOR__) > 206
#define INTO(x) .x =
#else
#define INTO(x)
#endif

p4_term_struct p4_term_k12 =
{
    "term-k12",
    INTO(control_string) 	term_k12_control_string,
    INTO(rawkey_string)  	term_k12_rawkey_string,
    INTO(init) 			c_prepare_terminal,
    INTO(fini) 			c_cleanup_terminal,
    INTO(tput)			c_tput,

    INTO(tty_interrupt_key) 	c_interrupt_key,
    INTO(interactive_terminal)  c_interactive_terminal,
    INTO(system_terminal)   	c_system_terminal,
    INTO(query_winsize)     	c_query_winsize,

    INTO(c_keypressed)		c_keypressed,
    INTO(c_getkey)		c_getkey,
    INTO(c_putc_noflush)  	c_putc_noflush,
    INTO(c_put_flush)		c_put_flush,
    INTO(c_putc)		c_putc,
    INTO(c_puts)		c_puts,
    INTO(c_gotoxy)		c_gotoxy,
    INTO(c_wherexy)		c_wherexy
};

/*@}*/

/*
 * Local variables:
 * c-file-style: "stroustrup"
 * End:
 */
