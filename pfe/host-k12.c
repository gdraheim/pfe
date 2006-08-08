/** 
 * -- useful additional primitives for K12xx platforms
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: bln_mpt1!33.21 %
 *    (%date_modified: Mon Feb 24 20:28:15 2003 %)
 *
 *  @description
 *              This wordset adds some additional primitives that
 *		are useful in K12xx environments.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: host-k12.c,v 1.1.1.1 2006-08-08 09:07:50 guidod Exp $";
#endif
 
#define _P4_SOURCE 1
#include <pfe/pfe-base.h>
#include <pfe/os-string.h>
#ifdef VxWorks
#include <fcntl.h>
#include <ioLib.h>
#endif
#include <pfe/term-k12.h>
#include <pfe/logging.h>

/* -------------------------------------------------------------------- */

#ifdef _K12_SOURCE
#include <pfe/main-k12.h>
#endif

/** OPEN-TERMINAL-LOGFILE ( s-buf s-len -- )
 * open terminal logfile named by the string-buffer
 * all further output to the terminal window is also logged into
 * this file. This is especially useful in embedded environments
 * where the terminal connection is often not used or it is
 * directed to a different location that does not easily allow to
 * redirect the forth output to a file for further examination.
 */
FCode (p4_open_terminal_logfile)
{
    char* filename = p4_pocket_filename ((p4char*) SP[1], (p4ucell) SP[0]);
    FX_2DROP;
    if (! filename || ! p4_strlen (filename)) return;
# ifdef _K12_SOURCE
    {
        p4_emu_t* p4 = P4_K12_EMUL(p4TH);
        p4->private.tx_logfile = open (filename, O_WRONLY|O_CREAT, 0640);
        if (p4->private.tx_logfile == -1)
        {
            P4_warn1 ("could not open terminal logfile %s", filename);
            p4->private.tx_logfile = 0;
        }
    }
# endif    
}

/** CLOSE-TERMINAL-LOGFILE ( -- )
 * close terminal logfile opened with => OPEN-TERMINAL-LOGFILE
 */
FCode (p4_close_terminal_logfile)
{
#  ifdef _K12_SOURCE
    {
        p4_emu_t* p4 = P4_K12_EMUL(p4TH);
        if (p4->private.tx_logfile)
        {
            close (p4->private.tx_logfile);
            p4->private.tx_logfile = 0;
        }
    }
#  endif
}

/** TERMINAL-ANSWER-LINK ( -- sap#* )
 * send terminal-output as a data-message to the specified link sap.
 * Unlike => TERMINAL-OUTPUT-LINK the data-messages are in line-mode.
 * The flushed characters are buffered until a non-printable character
 * is seen. This is somewhat more useful when treating pfe as a print
 * service and testing machine, but can not provide for interactivity.
 60 TERMINAL-ANSWER-LINK !
 ...
 TERMINAL-ANSWER-LINK OFF
 */
FCode (p4_terminal_answer_link)
{
# ifdef _K12_SOURCE
    {
        p4_emu_t* p4 = P4_K12_EMUL(p4TH);
        FX_PUSH (&(p4->private.qx_link));
    }
# endif    
}

/** TERMINAL-OUTPUT-LINK ( -- sap#* )
 * send terminal-output as a data-message to the specified link sap.
 * This can be used in an embedded systems for a terminal session simulation.
 * setting zero-sap will disable sending message-frames (the zero sap is
 * therefore not usable for output-to-link). The startup default is zero.
 60 TERMINAL-OUTPUT-LINK !
 ...
 TERMINAL-OUTPUT-LINK OFF
 */
FCode (p4_terminal_output_link)
{
# ifdef _K12_SOURCE
    {
        p4_emu_t* p4 = P4_K12_EMUL(p4TH);
        FX_PUSH (&(p4->private.tx_link));
    }
# endif    
}

/** TERMINAL-INPUT-LINK ( -- sap#* )
 * let the forth stdin-handling look for data-messages on this link too.
 * These will be interpreted like messages that come from the interactive
 * forth terminal. This can be used in an embedded systems for a terminal 
 * session simulation. setting zero-sap will disable interpreting these
 * incoming data-frames as keyboard-strings (so that the zero sap is
 * therefore not usable for an input-link!). The startup default is zero.
 60 TERMINAL-INPUT-LINK !
 ...
 TERMINAL-INPUT-LINK OFF
 */
FCode (p4_terminal_input_link)
{
# ifdef _K12_SOURCE
    {
        p4_emu_t* p4 = P4_K12_EMUL(p4TH);
        FX_PUSH (&(p4->private.rx_link));
    }
# endif    
}

/** TERMINAL-EMULATION-STATE ( -- state* )
 * returns the address of the emulations state variable so it can be
 * read and explicitly changed to another value from forth text. This is
 * a very questionable thing to do as the emulation-state is actually
 * an enumerated value, the ESE will just show question-marks setting 
 * this variable to something not understood.
 */
FCode(p4_terminal_emulation_state)
{
    register k12_priv* k12p = P4_K12_PRIV(p4TH);
    FX_PUSH(&k12p->state);
}

P4_LISTWORDS (host_k12) =
{
    P4_INTO ("FORTH", 0),
# ifdef _K12_SOURCE
    P4_OCoN ("#K1297-G20",     _K12_SOURCE+100),
# endif

    P4_INTO ("EXTENSIONS", "FORTH"),
    P4_FXco ("OPEN-TERMINAL-LOGFILE",    p4_open_terminal_logfile),
    P4_FXco ("CLOSE-TERMINAL-LOGFILE",   p4_close_terminal_logfile),
    P4_FXco ("TERMINAL-EMULATION-STATE", p4_terminal_emulation_state),
    P4_FXco ("TERMINAL-ANSWER-LINK",     p4_terminal_answer_link),
    P4_FXco ("TERMINAL-OUTPUT-LINK",     p4_terminal_output_link),
    P4_FXco ("TERMINAL-INPUT-LINK",      p4_terminal_input_link),
};
P4_COUNTWORDS (host_k12, "HOST-K12 extensions");

/*@}*/

/* 
 * Local variables:
 * c-file-style: "stroustrup"
 * End:
 */
