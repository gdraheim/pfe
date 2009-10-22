/**
 * -- Handle signals in forth
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.5 $
 *     (modified $Date: 2008-05-11 12:29:19 $)
 *
 *  @description
 *  The signal-callback system divides signals internally
 *  into the following classes:
 *
 *  <dl>
 *  <dt>P4_ON_XXXXX:</dt><dd>
 *      a signal which will be assigned a corresponding THROW
 *      on forth level, e.g. for SIGFPE
 *  </dd>
 *  <dt>Abort:</dt><dd>
 *      a signal that will not kill the current forth process
 *      but which has not forth-level THROW either, e.g. SIGILL.
 *      It will result in an ABORT" Signal-Description"
 *   </dd>
 *  <dt>Fatal:</dt><dd>
 *	the current forth process will die gracefully.
 *  </dd>
 *  <dt>Default:</dt><dd>
 *      A signal with some unknown meaning, exported to allow
 *      a forth-programmer to hook it anyway, e.g. to let
 *      a programmer on AIX to intercept SIGGRANT and run
 *      a forth word, otherwise the signal will be left untouched.
 *  </dd>
 *  <dt>Chandled:</dt><dd>
 *      A signal used internally be PFE and initially hooked
 *      by the runtime system, in general this would be the
 *      Job-Control signals and SIGWINCH that will get send
 *      when an xterm changes its size.
 *  </dd>
 *  </dl>
 *
 * The first three classes will go to the default Forth Signal-Handler.
 * Its execution will look for user-routine being registered (in
 * which cases that forth-routine will be executed) and otherwise
 * do its default action (to throw, abort" or exit).
 *
 * The fourth class is not hooked until some user-code requests that
 * signal in which case the user-defiend forth-routine is executed
 * as its action, otherwise the system-defined default-action will
 * be left untouched.
 *
 * The signal of the fourth type are not handled by the default
 * signal handler defined herein, so can not just call a
 * forth word, if I still get this right ;-)
 *                             <p align=right> Dirk-Uwe Zoller </p>
 *
 * note: forth-level callbacks might not work as expected on
 *       all systems that the rest of PFE runs on. Be careful.
 *                             <p align=right> Guido U. Draheim </p>
 */
/*@{*/

#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) =
"@(#) $Id: signals-ext.c,v 1.5 2008-05-11 12:29:19 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-limits.h>

#include <stdlib.h>
#include <pfe/os-string.h>
#include <signal.h>
#include <errno.h>

#include <pfe/def-comp.h>
#include <pfe/term-sub.h>
#include <pfe/option-ext.h>
#include <pfe/_nonansi.h>
#include <pfe/_missing.h>
#include <pfe/logging.h>
#include <pfe/def-restore.h>

typedef void (*SigHdl) (int);	/* signal handler function type */

enum				/* Classification of signals: The */
{				/* signal class is either a THROW code or: */
    Fatal,			/*   p4th terminates if such a signal arrives */
    Abort,			/*   executes ABORT" */
    Chandled,			/*   handled by C code, e.g. stop/continue */
    Default			/*   left alone by p4th, cannot be caught */
};

typedef struct			/* all we need to know about a signal */
{
    short sig;			/* the signal */
    short cLass;		/* a classification */
    char const * name;		/* the name of the signal */
#ifndef PFE_HAVE_SYS_SIGLIST
    char const * msg;		/* a textual signal description */
#endif
    SigHdl old;			/* state of signal before we took it */
    p4xt hdl;			/* a forth word to handle the signal */
} Siginfo;

/**
 * on a lot of platforms, you will see warning messages like
 * "function declaration is not a prototype". This is due to a change
 * in semantics for C99: <sys/signal.h> contains a declaration like
 *   #define SIG_DFL (void(*)())0
 * but that is wrong in the world of -fstrict-prototype where it should read
 *   #define SIG_DFL (void(*)(void))0
 * so actually, this can not be helped. You have to live with these warning
 * messages.
 */
#ifdef PFE_OLD_STRINGIZE
# ifdef PFE_HAVE_SYS_SIGLIST
# define SIG(NM,CL,MSG)		{ NM, CL, "NM", SIG_DFL }
# else
# define SIG(NM,CL,MSG)		{ NM, CL, "NM", MSG, SIG_DFL }
# endif
#else
# ifdef PFE_HAVE_SYS_SIGLIST
# define SIG(NM,CL,MSG)		{ NM, CL, #NM, SIG_DFL }
# else
# define SIG(NM,CL,MSG)		{ NM, CL, #NM, MSG, SIG_DFL }
# endif
#endif

/*
 * With the means of the above structures and classifications we
 * describe a lot of signals to p4th conditionally. Fine if a system
 * has a signal. If not it's just as well.
 */

static Siginfo siginfo[] =
{

/**
 * signal constants as being defined on the local system.
 * not all signal names must be valid in a specific system,
 * the pfe will only export those being available.
 * <p>
 * The set of signal names being tested for during pfe compilation
 * are described in W.R.Stevens' <br>
 * "Advanced Programming in the UNIX Environment"
 * <p>
 * The signals constants are generated by their
 * signal constructor => <<load_signals>>
 */
    /*"SIGABRT"*/
#ifdef SIGABRT
  SIG (SIGABRT, Fatal, "abnormal termination (abort)"),
#endif
    /*"SIGALRM"*/
#ifdef SIGALRM
  SIG (SIGALRM, Abort, "time out (alarm)"),
#endif
    /*"SIGBUS"*/
#ifdef SIGBUS
  SIG (SIGBUS, P4_ON_ADDRESS_ALIGNMENT, NULL),
#endif
    /*"SIGCHLD"*/
#ifdef SIGCHLD
  SIG (SIGCHLD, Default, "change in status of child"),
#endif
    /*"SIGCONT"*/
#ifdef SIGCONT
  SIG (SIGCONT, Default, "continue stopped process"),
#endif
    /*"SIGEMT"*/
#ifdef SIGEMT
  SIG (SIGEMT, Abort, "hardware fault (EMT)"),
#endif
    /*"SIGPFE"*/
#ifdef SIGFPE
  SIG (SIGFPE, P4_ON_FP_FAULT, NULL),
#endif
    /*"SIGHUP"*/
#ifdef SIGHUP
  SIG (SIGHUP, Fatal, "hangup"),
#endif
    /*"SIGILL"*/
#ifdef SIGILL
  SIG (SIGILL, Abort, "illegal hardware instruction"),
#endif
    /*"SIGINFO"*/
#ifdef SIGINFO
  SIG (SIGINFO, Default, "status request from keyboard"),
#endif
    /*"SIGINT"*/
#ifdef SIGINT
  SIG (SIGINT, P4_ON_USER_INTERRUPT, NULL),
#endif
    /*"SIGIO"*/
#ifdef SIGIO
  SIG (SIGIO, Default, "asynchronous io"),
#endif
    /*"SIGIOT"*/
#ifdef SIGIOT
  SIG (SIGIOT, Abort, "hardware fault (IOT)"),
#endif
    /*"SIGKILL"*/
#ifdef SIGKILL
  SIG (SIGKILL, Default, "kill"),
#endif
    /*"SIGPIPE"*/
#ifdef SIGPIPE
  SIG (SIGPIPE, Fatal, "write to pipe with no readers"),
#endif
    /*"SIGPOLL"*/
#ifdef SIGPOLL
  SIG (SIGPOLL, Abort, "pollable event (poll)"),
#endif
    /*"SIGPROF"*/
#ifdef SIGPROF
  SIG (SIGPROF, Abort, "profiling time alarm (timer)"),
#endif
    /*"SIGPWR"*/
#ifdef SIGPWR
  SIG (SIGPWR, Default, "power fail/restart"),
#endif
    /*"SIGQUIT"*/
#ifdef SIGQUIT
  SIG (SIGQUIT, Fatal, "terminal quit key"),
#endif
    /*"SIGSEGV"*/
#ifdef SIGSEGV
  SIG (SIGSEGV, P4_ON_INVALID_MEMORY, NULL),
#endif
    /*"SIGSTOP"*/
#ifdef SIGSTOP
  SIG (SIGSTOP, Default, "stop"),
#endif
    /*"SIGSYS"*/
#ifdef SIGSYS
  SIG (SIGSYS, Abort, "invalid system call"),
#endif
    /*"SIGTERM"*/
#ifdef SIGTERM
  SIG (SIGTERM, Fatal, "terminated"),
#endif
    /*"SIGTRAP"*/
#ifdef SIGTRAP
  SIG (SIGTRAP, Abort, "hardware fault (trace trap)"),
#endif
    /*"SIGTSTP"*/
#ifdef SIGTSTP
  SIG (SIGTSTP, Chandled, "terminal stop character"),
#endif
    /*"SIGTTIN"*/
#ifdef SIGTTIN
  SIG (SIGTTIN, Chandled, "background read from control tty"),
#endif
    /*"SIGTTOU"*/
#ifdef SIGTTOU
  SIG (SIGTTOU, Chandled, "background write to control tty"),
#endif
    /*"SIGURG"*/
#ifdef SIGURG
  SIG (SIGURG, Abort, "urgent condition"),
#endif
    /*"SIGUSR1"*/
#ifdef SIGUSR1
  SIG (SIGUSR1, Abort, "user defined signal 1"),
#endif
    /*"SIGUSR2"*/
#ifdef SIGUSR2
  SIG (SIGUSR2, Abort, "user defined signal 2"),
#endif
    /*"SIGVTALRM"*/
#ifdef SIGVTALRM
  SIG (SIGVTALRM, Abort, "virtual time alarm (timer)"),
#endif
    /*"SIGWINCH"*/
#ifdef SIGWINCH
  SIG (SIGWINCH, Chandled, "terminal window size change"),
#endif
    /*"SIGXCPU"*/
#ifdef SIGXCPU
  SIG (SIGXCPU, Fatal, "CPU limit exceeded"),
#endif
    /*"SIGXFSZ"*/
#ifdef SIGXFSZ
  SIG (SIGXFSZ, Abort, "file size limit exceeded"),
#endif

/**
 * Some Signals are specific to certain systems. They don't belong
 * to the normal set of UNIX signals like => SIGALRM or => SIGQUIT
 * <p>
 * The signals constants are generated by their
 * signal constructor => <<load_signals>>
 */
    /*"SIGSTKFLT"*/
#ifdef SIGSTKFLT		/* Linux */
  SIG (SIGSTKFLT, Abort, "SIGSTKFLT"),
#endif

    /*"SIGBREAK"*/
#ifdef SIGBREAK			/* EMX, Watcom */
  SIG (SIGBREAK, P4_ON_USER_INTERRUPT, NULL),
#endif

    /*"SIGMSG"*/
#ifdef SIGMSG			/* AIX 3.2 */
  SIG (SIGMSG, Default, "input data is in the HFT ring buffer"),
#endif
    /*"SIGDANGER"*/
#ifdef SIGDANGER
  SIG (SIGDANGER, Default, "system crash imminent; free up some page space"),
#endif
    /*"SIGMIGRATE"*/
#ifdef SIGMIGRATE
  SIG (SIGMIGRATE, Default, "migrate process (see TCF)"),
#endif
    /*"SIGPRE"*/
#ifdef SIGPRE
  SIG (SIGPRE, Default, "programming exception"),
#endif
    /*"SIGVIRT"*/
#ifdef SIGVIRT
  SIG (SIGVIRT, Default, "AIX virtual time alarm"),
#endif
    /*"SIGGRANT"*/
#ifdef SIGGRANT
  SIG (SIGGRANT, Default, "HFT monitor mode granted"),
#endif
    /*"SIGRETRACT"*/
#ifdef SIGRETRACT
  SIG (SIGRETRACT, Default, "HFT monitor mode should be relinguished"),
#endif
    /*"SIGSOUND"*/
#ifdef SIGSOUND
  SIG (SIGSOUND, Default, "HFT sound control has completed"),
#endif
    /*"SIGSAK"*/
#ifdef SIGSAK
  SIG (SIGSAK, Default, "secure attention key"),
#endif
};

static int
getinfo (int sig)
{
    int i;

    for (i = 0; i < DIM (siginfo); i++)
        if (siginfo[i].sig == sig)
            return i;

    p4_throw (P4_ON_ARG_TYPE);
    return i;
}

typedef PFE_RETSIGTYPE (*_sighandler_t)(int);

/* in multithread, signal-handlers might possibly be in another context... */
#if defined P4_REGTH && ! defined unix
#define P4_REGTH_SIGNAL_SAVEALL P4_CALLER_SAVEALL; p4TH = p4_main_threadP;
#define P4_REGTH_SIGNAL_RESTORE P4_CALLER_RESTORE;
#else
#define P4_REGTH_SIGNAL_SAVEALL
#define P4_REGTH_SIGNAL_RESTORE
#endif

static void
sig_handler (int sig)		/* Signal handler for all signals */
{
    Siginfo *s;

# if !KEEPS_SIGNALS
    if (SIG_ERR == signal (sig, (_sighandler_t) sig_handler)) {
        P4_fail ("signal reinstall failed");
    }
# endif
# if defined SYS_EMX || defined SYS_WC_OS2V2
    signal (sig, SIG_ACK);	/* OS/2: acknowledge signal */
# endif
# if defined SYS_EMX
    _control87 (EM_DENORMAL | EM_INEXACT, MCW_EM);
# endif

    {

        s = &siginfo[getinfo (sig)];
        if (s->hdl) {
            P4_info ("forth-signal callback does not work!"); /* FIXME: */
#         if 0
            p4_call (s->hdl);	/* a p4sys.handled signal */
#         elif defined PFE_SBR_CALL_THREADING
            p4_sbr_call (s->hdl);
#         elif defined PFE_CALL_THREADING
            p4_call (s->hdl);  /* fixme */
#         else
            /* assume that s->hdl is a colon word */
            PFE_VM_LOAD(p4TH);
            FX_PUSH_RP = IP;
            IP = (p4xcode *) P4_TO_BODY (s->hdl);
#         endif
        } else {
            P4_REGTH_SIGNAL_SAVEALL;
#          ifdef PFE_HAVE_SYS_SIGLIST
            const char* msg = sys_siglist[sig];
#          else
            const char* msg = s->msg;
#          endif
            /* P4_warn1 ("throw signal '%s'", msg); */
            switch (s->cLass)
            {
             default:		  /* an ANSI-Forth defined condition */
                 P4_info2 ("signal %i throw %i", sig, s->cLass);
                 p4_throw (s->cLass);
             case Abort:		/* another catchable signal */
                 P4_info2 ("signal %i abort %s", sig, msg);
                 p4_throwstr (-256 - sig, msg);
             case Fatal:		/* a signal that kills us */
                 P4_fatal2 ("Received signal %s, %s", s->name, msg);
                 PFE.exitcode = 1;
                 p4_longjmp_exit ();
            }
            P4_REGTH_SIGNAL_RESTORE;
        }
    }
}

/*
 * Actions to take when job control interferes or on window size change:
 */

#ifdef SIGTSTP
static void
stop_hdl (int sig)
{
#  if !KEEPS_SIGNALS
    signal (sig, (_sighandler_t) stop_hdl);
#  endif
    {
        P4_REGTH_SIGNAL_SAVEALL;
        PFE.on_stop ();
        p4_swap_signals ();
#      if _BSD
        _pfe_raise (SIGSTOP);
#      else
        _pfe_raise (SIGTSTP);
#      endif
        p4_swap_signals ();
        PFE.on_continue ();
        P4_REGTH_SIGNAL_RESTORE;
    }
}
#endif

#ifdef SIGWINCH
static void
winchg_hdl (int sig)
{
#  if !KEEPS_SIGNALS
    signal (sig, winchg_hdl);
#  endif
    {
        P4_REGTH_SIGNAL_SAVEALL;
        PFE.on_winchg ();
        P4_REGTH_SIGNAL_RESTORE;
    }
}
#endif

#ifdef _AIX_CC
#define volatile
#endif

#ifdef SIGALRM
static void
handle_sigalrm (int sig)
{
#  if !KEEPS_SIGNALS
    signal (sig, (_sighandler_t) handle_sigalrm);
#  endif
    {
        P4_REGTH_SIGNAL_SAVEALL;
        if (PFE.on_sigalrm)
            (*PFE.on_sigalrm)();
        P4_REGTH_SIGNAL_RESTORE;
    }
}
#endif

/**
 * install all signal handlers:
 */
void
p4_install_signal_handlers (void)
{
    int i, j;
    static const p4_char_t use_signals[] = "signals";

    if (p4_search_option_value (use_signals, sizeof (use_signals) - 1,
                                P4_TRUE, PFE.set))
    for (i = 0; i < DIM (siginfo); i++)
    {
        /* some systems may have more than one name for the same signal,
         * take care not to install it twice: */
        for (j = 0; j < i; j++)
            if (siginfo[i].sig == siginfo[j].sig)
                goto cont;
        switch (siginfo[i].cLass)
        {
         default:
             siginfo[i].old = signal (siginfo[i].sig, sig_handler);
             if (0) { P4_fail3("signal %s @ %i, hooked %p",
                        siginfo[i].name, siginfo[i].sig, siginfo[i].old); }
         case Chandled:
         case Default:;
        }
     cont:;
    }

    if (PFE_set.stdio)
        return;
    /* else */
#ifdef SIGTSTP
    if (signal (SIGTSTP, SIG_IGN) == SIG_DFL)
    {
        signal (SIGTSTP, stop_hdl);
        siginfo[getinfo (SIGTSTP)].old = SIG_DFL;
        siginfo[getinfo (SIGTTIN)].old = signal (SIGTTIN, stop_hdl);
        siginfo[getinfo (SIGTTOU)].old = signal (SIGTTOU, stop_hdl);
    }
#endif
#ifdef SIGWINCH
#ifdef KEEPS_SIGNALS
    signal (SIGWINCH, winchg_hdl);
#endif
    winchg_hdl (SIGWINCH);
#endif

#ifdef SIGALRM
    signal (SIGALRM, (_sighandler_t) handle_sigalrm);
#endif
}

/**
 * switch between p4th setting of signals and state before
 */
void
p4_swap_signals (void)
{
    int i;

    for (i = 0; i < DIM (siginfo); i++)
        if (siginfo[i].cLass != Default || siginfo[i].hdl)
            siginfo[i].old = signal (siginfo[i].sig, siginfo[i].old);
}

/**
 * xt != NULL: install forth word as signal handler for signal
 * xt == NULL: install p4th default signal handler for signal
 */
p4xt
p4_forth_signal (int sig, p4xt xt)
{
    int i = getinfo (sig);
    p4xt old;

    old = siginfo[i].hdl;
    siginfo[i].hdl = xt;

    if (siginfo[i].cLass == Default)
    {
        if (xt == NULL)
        {
            siginfo[i].old = signal (sig, siginfo[i].old);
        }
        else
        {
            siginfo[i].old = signal (sig, sig_handler);
        }
    }

    return old;
}

/**
 * Load constants for each signal found into the dictionary.
 */
void
p4_load_signals (p4_Wordl *wid)
{
    Siginfo *s;

    for (s = siginfo; s < siginfo + DIM (siginfo); s++)
    {
        p4_header_comma ((p4_char_t*) s->name, p4_strlen (s->name), wid);
        FX_RUNTIME1(p4_constant);
        FX_UCOMMA (s->sig);
    }
}

/** RAISE-SIGNAL ( signal# -- ior ) [FTH]
 * send a => SIGNAL to self
 *
 * OLD: this was called RAISE up to PFE 0.33.x
 *      and the old word was not returning any result-code
 */
void FXCode (p4_raise_signal)
{
    *SP = P4_IOR (_pfe_raise (*SP));
}

/** FORTH-SIGNAL ( handler-xt* signal# -- old-signal-xt* ) [FTH]
 * install signal handler
 * - return old signal handler
 *
 * OLD: this was called SIGNAL up to PFE 0.33.x
 */
void FXCode (p4_forth_signal)
{
    SP[1] = (p4cell) p4_forth_signal (SP[0], (p4xt) SP[1]);
    SP++;
}

/**
 * the signals-constructor will declare the available
 * system signals as contants - usually sth. like
 * => SIGALRM or => SIGHUP or => SIGABRT
 * <p>
 * some signals are only valid in specific systems,
 * like => SIGBREAK or => SIGMSG or => SIGVIRT
 */
void FXCode (p4_load_signals)
{
    p4_load_signals (CURRENT);
}

P4_LISTWORDSET (signals) [] =
{
    P4_INTO ("FORTH", 0),
    P4_FXco ("RAISE-SIGNAL",		p4_raise_signal),
    P4_FXco ("FORTH-SIGNAL",		p4_forth_signal),

    P4_INTO ("EXTENSIONS", 0),
    P4_XXco ("<<load_signals>>", p4_load_signals),
};
P4_COUNTWORDSET (signals, "Signals Extension");

/*@}*/
