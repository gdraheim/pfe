/**
 * --  Exception-oriented Subroutines.
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author$)
 *  @version $Revision$
 *     (modified $Date$)
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) =
"@(#) $Id$";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-limits.h>

#include <stdio.h>
#include <stdarg.h>
#include <pfe/os-string.h>
#include <pfe/os-setjmp.h>

#include <pfe/exception-sub.h>
#include <pfe/block-sub.h>
#include <pfe/file-sub.h>
#include <pfe/_missing.h>

#include <pfe/logging.h>

#ifndef _export
#define p4_longjmp_abort()	(p4_longjmp_loop('A'))
#define p4_longjmp_exit()	(p4_longjmp_loop('X'))
#define p4_longjmp_quit()	(p4_longjmp_loop('Q'))
#define p4_longjmp_yield()	(p4_longjmp_loop('S'))
#endif

/**
 * just call longjmp on PFE.loop
 */
_export void
p4_longjmp_loop(int arg)
{
        PFE.setjmp_fenv_save(& PFE.loop_fenv);
    p4_longjmp (PFE.loop, arg);
}

/*
 * show the error, along with info like the block, filename, line numer.
 */
static void
show_error (const char* str, int len)
{
    PFE.input_err = PFE.input;	/* save input specification of error */

    if (! str) str = "";
    if (! len) len = p4_strlen(str);
    p4_outf ("\nError: %.*s", len, str);
    FX (p4_cr_show_input);
    p4_longjmp_abort ();
}

void FXCode (p4_cr_show_input)
{
    int n;
    const char* str = "";
    int len = 1;
    if (PFE.word.ptr && PFE.word.len)
    {
        str = (char*) PFE.word.ptr;
        len = PFE.word.len;
    }

    switch (SOURCE_ID)
    {
     case 0:
         if (BLK && BLOCK_FILE && ! ferror (BLOCK_FILE->f))
         {
             p4_outf ("\n\\ Block %lu line %ld: \"%.*s\"\n",
               (p4ucelll) BLK, (p4celll)( TO_IN / 64), len, str);
             p4_blockfile_dot_line (BLOCK_FILE, BLK, TO_IN / 64);
             n = TO_IN % 64;
             break;
         } /* fallthrough*/
     case -1:
         p4_outf ("\n\\ Terminal input: \"%.*s\"\n", len, str); /* to Error:-line */
         p4_type (TIB, NUMBER_TIB);
         n = TO_IN;
         break;
     default:
         p4_outf ("\n\\ File %s line %lu: \"%.*s\"\n",
           SOURCE_FILE->name, SOURCE_FILE->blk + 1, len, str);
         p4_type (TIB, NUMBER_TIB);
         n = TO_IN;
    }
    if (PFE.word.len > TO_IN)
        p4_outf ("\n%*s", n, "^"); /* just mark ">IN" */
    else
    {
        p4_outs ("\n");
        if (TO_IN != PFE.word.len)
            p4_emits (TO_IN - PFE.word.len-1, ' ');
        p4_emits (PFE.word.len+1, '^'); /* mark the word */
    }

    p4_outs (" ");
}

static void
throw_msg (int id, char *msg)
{
    static const char *throw_explanation[] =
    {
        /*  -1 */ NULL, /* ABORT */
        /*  -2 */ NULL, /* ABORT" */
        /*  -3 */ "stack overflow",
        /*  -4 */ "stack underflow",
        /*  -5 */ "return-stack overflow",
        /*  -6 */ "return-stack underflow",
        /*  -7 */ "do-loops nested too deeply during execution",
        /*  -8 */ "dictionary overflow",
        /*  -9 */ "invalid memory address",
        /* -10 */ "division by zero",
        /* -11 */ "result out of range",
        /* -12 */ "argument type mismatch",
        /* -13 */ "undefined word",
        /* -14 */ "interpreting a compile-only word",
        /* -15 */ "invalid FORGET (not between FENCE and HERE)",
        /* -16 */ "attempt to use a zero-length string as a name",
        /* -17 */ "pictured numeric output string overflow",
        /* -18 */ "parsed string overflow (input token longer than 255)",
        /* -19 */ "definition name too long",
        /* -20 */ "write to a read-only location",
        /* -21 */ "unsupported operation",
        /* -22 */ "control structure mismatch",
        /* -23 */ "address alignment exception",
        /* -24 */ "invalid numeric argument",
        /* -25 */ "return stack imbalance",
        /* -26 */ "loop parameters unavailable",
        /* -27 */ "invalid recursion",
        /* -28 */ "user interrupt",
        /* -29 */ "compiler nesting (exec/comp state incorrect)",
        /* -30 */ "obsolescent feature",
        /* -31 */ ">BODY used on non-CREATEDd definition",
        /* -32 */ "invalid name argument",
        /* -33 */ "block read exception",
        /* -34 */ "block write exception",
        /* -35 */ "invalid block number",
        /* -36 */ "invalid file position",
        /* -37 */ "file I/O exception",
        /* -38 */ "non-existent file",
        /* -39 */ "unexpected end of file",
        /* -40 */ "invalid BASE for floating-point conversion",
        /* -41 */ "loss of precision",
        /* -42 */ "floating-point divide by zero",
        /* -43 */ "floating-point result out of range",
        /* -44 */ "floating-point stack overflow",
        /* -45 */ "floating-point stack underflow",
        /* -46 */ "floating-point invalid argument",
        /* -47 */ "CURRENT deleted (forget on DEFINITIONS vocabulary)",
        /* -48 */ "invalid POSTPONE",
        /* -49 */ "search-order overflow (ALSO failed)",
        /* -50 */ "search-order underflow (PREVIOUS failed)",
        /* -51 */ "compilation word list changed",
        /* -52 */ "control flow stack overflow",
        /* -53 */ "exception stack overflow",
        /* -54 */ "floating-point underflow",
        /* -55 */ "floating-point unidentified fault",
        /* -56 */ NULL, /* QUIT */
        /* -57 */ "error in sending or receiving a character",
        /* -58 */ "[IF], [ELSE] or [THEN] error",
        /* these Forth200x THROW-IORS:X are not used in PFE */
        /* -59 */ "ALLOCATE error",
        /* -60 */ "FREE error",
        /* -61 */ "RESIZE error",
        /* -62 */ "CLOSE-FILE error",
        /* -63 */ "CREATE-FILE error",
        /* -64 */ "DELETE-FILE error",
        /* -65 */ "FILE-POSITION error",
        /* -66 */ "FILE-SIZE error",
        /* -67 */ "FILE-STATUS error",
        /* -68 */ "FLUSH-FILE error",
        /* -69 */ "OPEN-FILE error",
        /* -70 */ "READ-FILE error",
        /* -71 */ "READ-LINE error",
        /* -72 */ "RENAME-FILE error",
        /* -73 */ "REPOSITION-FILE error",
        /* -74 */ "RESIZE-FILE error",
        /* -75 */ "WRITE-FILE error",
        /* -76 */ "WRITE-LINE error",
    };

    if (-1 - DIM (throw_explanation) < id && id <= -1)
    {
        /* ANS-Forth throw codes, messages are in throw_explanation[] */
        p4_strcpy (msg, throw_explanation[-1 - id]);
    }
    else if (-1024 < id && id <= -256)
    {
        /* Signals, see signal-ext.c,
           those not handled and not fatal lead to THROW */
        sprintf (msg, "Received signal %d", -256 - id);
    }
    else if (-2048 < id && id <= -1024)
    {
        /* File errors, see FX_IOR / P4_IOR(flag) */
        sprintf (msg, "I/O Error %d : %s", -1024-id, strerror (-1024-id));
    }
    else if (-32767 < id && id <= -2048)
    {
        /* search the exception_link for our id */
        p4_Exception* expt = PFE.exception_link;
        p4_strcpy (msg, "module-specific error-condition");
        while (expt)
        {
            if (expt->id == id)
            {
                p4_strcpy (msg, expt->name);
                break;
            }
            expt = expt->next;
        }
    }
    else if (0 < id)
    {
#     ifdef PFE_HAVE_STRERROR_R
        strerror_r (id, msg, 255);
#     else
        p4_strcpy (msg, strerror (id));
#     endif
    }
    else
    {
        sprintf (msg, "%d THROW unassigned", id);
    }
}

/**
 * the CATCH impl
 */
_export int
p4_catch (p4xt xt)
{
    register int returnvalue;
    auto p4_Except frame;

    frame.magic = P4_EXCEPTION_MAGIC;
#  ifndef PFE_SBR_CALL_ARG_THREADING
    frame.ipp = IP;
#  endif
    frame.spp = SP;
    frame.lpp = LP;
#  ifndef P4_NO_FP
    frame.fpp = FP;
#  endif
#  ifdef P4_RP_IN_VM
    frame.rpp = RP;
#  endif
    frame.iframe = PFE.saved_input;
    frame.prev = PFE.catchframe;  PFE.catchframe = &frame;
        p4_setjmp_fenv_save(& frame.jmp_fenv);
    returnvalue = p4_setjmp (frame.jmp);
    if (! returnvalue) {
        p4_call (xt);
    }
        p4_setjmp_fenv_load(& frame.jmp_fenv);
    PFE.catchframe = frame.prev;
#  ifdef P4_RP_IN_VM
    RP = frame.rpp;
#  endif
    return returnvalue;
}

_export void
p4_throw (int id)
{
    p4_throws (id, 0, 0);
}

_export void
p4_throwstr (int id, const char* description)
{
    p4_throws (id, (const p4_char_t*) description,
               (description ? strlen(description) : 0));
}

/**
 * the THROW impl
 */
_export void
p4_throws (int id, const p4_char_t* description, int len)
{
    p4_Except *frame = PFE.catchframe;
    char msg[256];
    char* addr = (char*) description;

    if (PFE.atexit_running)
    {
        if (addr && len)
            show_error (addr, len);
        p4_longjmp_exit ();
    }

    if (PFE.throw_cleanup)
    {
        PFE.throw_cleanup ();
        PFE.throw_cleanup = NULL;
    }

    if (frame && frame->magic == P4_EXCEPTION_MAGIC)
    {
#    ifndef PFE_SBR_CALL_ARG_THREADING
        IP = frame->ipp;
#    endif
        SP = frame->spp;
        LP = frame->lpp;
#    ifndef P4_NO_FP
        FP = frame->fpp;
#     endif /*P4_NO_FP*/
#  ifdef P4_RP_IN_VM
        RP = frame->rpp;
#  endif
        p4_unnest_input (frame->iframe);
        p4_longjmp (frame->jmp, id);
    }

#  ifdef P4_RP_IN_VM
    *--RP = IP;
    CSP = (p4cell*) RP;         /* come_back marker */
#  endif
    switch (id)
    {
     case P4_ON_ABORT_QUOTE:
     {
         show_error (addr, len);
     }
     case P4_ON_ABORT:
         p4_longjmp_abort ();
     case P4_ON_QUIT:
         p4_longjmp_quit ();
     default:
         throw_msg (id, msg);
         if (addr)
         {
             p4_strcat (msg, " : ");
             if (! len)
                 p4_strcat (msg, addr);
             else
             {
                 msg[len+p4_strlen(msg)] = '\0';
                 p4_strncat (msg, addr, len);
             }
         }
         show_error (msg, 0);
    }
}

/*@}*/
