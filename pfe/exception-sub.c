/** 
 * --  Exception-oriented Subroutines.
 * 
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE            @(#) %derived_by: guidod %
 *  @version %version: 1.7 %
 *    (%date_modified: Mon Mar 12 10:32:16 2001 %)
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: exception-sub.c,v 0.30.86.1 2001-03-12 09:32:16 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-xtra.h>

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <setjmp.h>

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
    longjmp (PFE.loop, arg);
}

/*
 * show the error, along with info like the block, filename, line numer.
 */
static void
show_error (const char *fmt,...)
{
    char buf[256];
    va_list p;
    int n;

    PFE.input_err = PFE.input;	/* save input specification of error */
    va_start (p, fmt);
    vsprintf (buf, fmt, p);
    va_end (p);
    p4_outf ("\nError: \"%.*s\": %s\n", *DP, DP + 1, buf);
    switch (SOURCE_ID)
    {
     case 0:
         if (BLK && BLOCK_FILE && !ferror (BLOCK_FILE->f))
         {
             p4_outf ("Block %lu line %d:\n",
               (unsigned long) BLK, (int) TO_IN / 64);
             p4_dot_line (BLOCK_FILE, BLK, TO_IN / 64);
             n = TO_IN % 64;
             break;
         }
     case -1:
         p4_type (TIB, NUMBER_TIB);
         n = TO_IN;
         break;
     default:
         p4_outf ("File %s line %lu:\n",
           SOURCE_FILE->name, (unsigned long) SOURCE_FILE->n);
         p4_type (TIB, NUMBER_TIB);
      n = TO_IN;
    }
    p4_outf ("\n%*s", n, "^");
    p4_longjmp_abort ();
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
        /* -59 */ "dictionary space exhausted"
    };
    static const char* throw_explanation_p4[] =
    {
        /* -2048 */ "no or not matching binary image",
        /* -2049 */ "binary image too big",
        /* -2050 */ "out of memory",
        /* -2051 */ "index out of range",
        /* -2052 */ "compile failed (call from bad point)",
#      ifdef PFE_WITH_DSTRINGS_EXT      /* (dnw 4feb010 */
        /* NOTE:  I'm not sure these assignments are the best solution,
           but they let us use the code below.  I haven't checked whether
           the code requires the trailing comma, which was also in
           pfe-0.9.14.
        */ 
        /* -2053 */ "string count too large",
        /* -2054 */ "string space overflow",
        /* -2055 */ "string garbage locked",
        /* -2056 */ "string stack underflow",
        /* -2057 */ "cat lock preventing string copy",
        /* -2058 */ "dynamic string count too large",
        /* -2059 */ "too many string frames",
        /* -2060 */ "not enough strings for frame",
        /* -2061 */ "string frame stack underflow",
        /* -2062 */ "string frame not at top of string stack",
#      endif
    };
    
    if (-1 - DIM (throw_explanation) < id && id <= -1)
    {
        /* ANS-Forth throw codes, messages are in throw_explanation[] */
        strcpy (msg, throw_explanation[-1 - id]);
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
    else if (-2048 - DIM (throw_explanation_p4) < id && id <= -2048)
    {
        strcpy (msg, throw_explanation_p4[-2048 - id]);
    }
    else if (0 < id)
    {
#     ifdef PFE_HAVE_STRERROR_R
	strerror_r (id, msg, 255);
#     else
	strcpy (msg, strerror (id));
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
    Except *x = P4_DEC (RP, Except);
    int id;

    x->magic = EXCEPTION_MAGIC;
    x->ipp = IP;
    x->spp = SP;
    x->lpp = LP;
#  ifndef P4_NO_FP
    x->fpp = FP;
#  endif
    x->iframe = PFE.saved_input;
    x->prev = PFE.cAtch;
    PFE.cAtch = x;
    id = setjmp (x->jmp);
    if (!id)
        p4_call (xt);
    PFE.cAtch = x->prev;
    RP = (p4xt **) &x[1];
    return id;
}

#ifdef _K12_SOURCE
extern void trcStack(int); /* show stack trace */
extern int taskIdSelf();
extern int taskPriorityGet(int, int*);
extern int taskDelay(int);
extern int taskSpawn(char*, int, int, int, void*, int, ...);
static int spawn_trcStack(int taskprio, int taskid)
{ 
    if (taskprio > 0) taskprio--;
    taskDelay(1); /* 1 x sched_yield */
    taskSpawn(0, taskprio, 0, 8192, (void*)trcStack, taskid); 
    return 0;
}
#endif 

/**
 * the THROW impl
 */
_export void
p4_throws (int id, const char* addr, int len)
{
    Except *x = PFE.cAtch;
    char msg[256];

    if (PFE.atexit_running) 
    {
        if (addr && len)
            show_error(".*s", addr, len);
        p4_longjmp_exit ();
    }
  
#ifdef _K12_SOURCE
    {
        int taskid, taskprio;
        if (p4_LogMask & P4_LOG_DEBUG) 
        { /* if any debug-channel used */
            taskPriorityGet((taskid= taskIdSelf()), &taskprio);
            taskSpawn(0, taskprio, 0, 8192, 
              (void*)spawn_trcStack, taskprio, taskid);
            taskDelay(2); /* 2 x sched_yield */
        }
    }
#endif
  
    if (PFE.throw_cleanup) 
    { 
        PFE.throw_cleanup ();
        PFE.throw_cleanup = NULL;
    }

    if (x && x->magic == EXCEPTION_MAGIC)
    {
        IP = x->ipp;
        SP = x->spp;
        LP = x->lpp;
#    ifndef P4_NO_FP
        FP = x->fpp;
#     endif P4_NO_FP
        p4_unnest_input (x->iframe);
        longjmp (x->jmp, id);
    }
    switch (id)
    {
     case P4_ON_ABORT_QUOTE:
     {
         if (len)
             show_error ("%.*s", len, addr);
         else
             show_error ("%s", addr);
     }
     case P4_ON_ABORT:
         p4_longjmp_abort ();
     case P4_ON_QUIT:
         p4_longjmp_quit ();
     default:
         throw_msg (id, msg);
         if (addr)
         {
             strcat (msg, " : ");
             if (! len)
                 strcat (msg, addr);
             else
             {
                 msg[len+strlen(msg)] = '\0';
                 strncat (msg, addr, len);
             }
         }
         show_error (msg);
    }
}

_export void
p4_throw (int id)
{
    p4_throws (id, 0, 0);
}

/*@}*/

