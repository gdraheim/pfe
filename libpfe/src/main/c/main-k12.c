/**
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2003.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2008-04-20 04:46:30 $)
 */
/*@{ */
#if defined(__version_control__) && defined(__GNUC__)
static char * id __attribute__((unused)) =
"@(#) $Id: main-k12.c,v 1.3 2008-04-20 04:46:30 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-ext.h>

#include <stddef.h> /*offsetof*/
#include <stdlib.h>
#include <pfe/os-string.h>
#include <pfe/os-ctype.h>

#include <taskLib.h>
#include <sysLib.h>
#include <time.h>
#include <K12/emul.h>
#include <K12/trace.h>

#include <pfe/engine-set.h>
#include <pfe/option-set.h>
#include <pfe/main-k12.h>
#include <pfe/term-k12.h>
#include <pfe/logging.h>
#include <pfe/def-restore.h>

#include <pfe/engine-sub.h>
#include <pfe/version-sub.h>
#include <gemapi.h>

#include <pfe/option-ext.h>

#ifdef  CLOCKS_PER_SEC_BUG
#undef  CLOCKS_PER_SEC
#define CLOCKS_PER_SEC sysClkRateGet()
#endif

#ifndef DIM
#define DIM(X) sizeof(X)/sizeof(*X)
#endif
#ifndef _K12_EAGAIN     /* the LEM does not know what to do about EAGAIN */
#define _K12_EAGAIN	K12_GEN_OK
#endif
#ifndef MSC_MAX_VARS             /* user-config */
#define MSC_MAX_VARS   16
#endif
#ifndef MAX_MAX_TIMERS           /* user-config */
#define MSC_MAX_TIMERS 64
#endif

#define config_blocked has.config.blocked

/* definition of enumeral types, for config/query menu */
static k12_emu_enum_t
StatesEnum[] =
{
    { "COMP", -1 },
    { "EXEC",  0 },
    { "COMP",  1 },
};

k12_emu_enum_t
p4_emu_hmsc_action_enum[] =
{
    { "NONE", 0 },
    { "Start", 1 },
    { "Stop", 2 },
    { "Continue", 3 },
    { "Reload", 4 },
    { "Reset", 5 },
};

k12_emu_enum_t
p4_emu_state__k12_enum[] =
{
    { "NOT_LOADED", K12_EMU_NOT_LOADED },
    { "IDLE"      , K12_EMU_IDLE       },
    { "ACTIVE"    , K12_EMU_ACTIVE     },
    { "WARNING"   , K12_EMU_WARNING    },
    { "ERROR_CASE", K12_EMU_ERROR_CASE },
    { "NO_ANSWER" , K12_EMU_NO_ANSWER  }
};

status_t
p4_conftrace (k12_emu_type_t* emulId,
              u32_t request,
              u32_t* value,
              u32_t* addr);

/* BEWARE: called from LEM context */
k12_status_t
p4_emu_state (k12_emu_type_t *emulId,
              u32_t mode,
              u32_t *value,
              s8_t *addr)
{
    p4_emu_t *p4_emu;
    /* NO P4_CALLER_SAVEALL necessary - no forth VM register used */

    if (k12EmuLowBaseGet (emulId, (s8_t **) &p4_emu) != K12_GEN_OK)
    {
        P4_fatal ("Could not get base!\n");
        return (K12_EMU_LOW_ERROR);
    }
#if 0 /* silently ignore CONFIG */
    if (mode & K12_EMU_CONFIG)
    {
        if (p4_emu->private.state != K12_EMU_NOT_LOADED /* at the begin */
          && *value != K12_EMU_NOT_LOADED)    /* at the end   */
        {
            P4_warn ("config of read only value \"state\"\n");
        }else{
            p4_emu->state = *value;
        }
        return (K12_GEN_OK);
    }
#endif /* 0 */
    if (mode & K12_EMU_QUERY)
    {
        *value = p4_emu->private.state;
    }
    return (K12_GEN_OK);
}

/* BEWARE: called from LEM context */
k12_status_t
p4_emu_state_desc (k12_emu_type_t *emulId,
                   u32_t mode,
                   s8_t *value,
                   s8_t *addr)
{
    p4_emu_t *p4_emu;
    /* NO P4_CALLER_SAVEALL necessary - no forth VM register used */

    if (k12EmuLowBaseGet (emulId, (s8_t **) &p4_emu) != K12_GEN_OK)
    {
        P4_fatal ("Could not get base!\n");
        return (K12_EMU_LOW_ERROR);
    }

    if (mode & K12_EMU_QUERY)
    {
        switch (p4_emu->private.state)
        {
        case K12_EMU_NOT_LOADED:
            /* tell why not ready for action */
            if (! p4_emu->thread.dict)
            {   /* boot_system not through */
                p4_strcpy (value, "INIT: receiving config options");
            } else if (! p4_emu->thread.fence)
            {   /* boot_system not through */
                p4_strcpy (value, "BOOT: setting up base system");
            } else if (p4_emu->thread.input.source_id != 0
                       && p4_emu->thread.input.source_id != -1)
            {   /* loading a file, source_id is a p4_File */
                sprintf (value, "LOAD: %.*s : %u", 240,
                         ((p4_File*)p4_emu->thread.input.source_id)->name,
                         ((p4_File*)p4_emu->thread.input.source_id)->n);
            }else{
                p4_strcpy (value, "LOAD: - : looking for commands");
            } break;
        case K12_EMU_IDLE:
            p4_strcpy (value, "Forth System Ready");
            break;
        case K12_EMU_ACTIVE:
            p4_strcpy (value, "Test Manager Active");
            break;
        case K12_EMU_WARNING:
            p4_strcpy (value, "Warning ");
            if (p4_emu->private.answerwarning)
                p4_strcat (value, p4_emu->private.answerbuf);
            break;
        case K12_EMU_ERROR_CASE:
            p4_strcpy (value, "Error ");
            if (p4_emu->private.answerwarning)
                p4_strcat (value, p4_emu->private.answerbuf);
            break;
        case K12_EMU_NO_ANSWER:
            p4_strcpy (value, "No Answer");
            break;
        default:
            p4_strcpy (value, "(not available)");
            break;
        }
    }
    return (K12_GEN_OK);
}

/* BEWARE: called from LEM context */
/* Used outside of pfe.o */
status_t
p4_emu_not_implemented (k12_emu_type_t* pEmul,
                        u32_t Mode,
                        u8_t* pString,
                        s8_t* pAddr)
{
    /* NO P4_CALLER_SAVEALL necessary - no forth VM register used */

    K12LogWarn1("p4 emu call", " '%s' not implemented\n", pAddr);
    /*  return K12_GEN_NOT_IMPLEMENTED; *FIXME:*/
    return K12_GEN_OK;
}

/* BEWARE: called from LEM context */
status_t
p4_emu_ignore (k12_emu_type_t* pEmul,
               u32_t Mode,
               u8_t* pString,
               s8_t* pAddr)
{
    /* NO P4_CALLER_SAVEALL necessary - no forth VM register used */

    /*  return K12_GEN_NOT_IMPLEMENTED; *FIXME:*/
    return K12_GEN_OK;
}

/* BEWARE: called from LEM context */
status_t
p4_emu_version (k12_emu_type_t* emul_id,
                u32_t mode,
                u8_t* str,
                s8_t* addr)
{
    /* <head> */
    register status_t e;
    p4_emu_t* p4;
    /* only partial P4_CALLER_SAVE necessary - just REGTH of the Forth VM */
    P4_REGTH_MKSAVED;

    e=k12EmuLowBaseGet (emul_id, (void*)&p4);
    if (e) return e;
    p4TH = &p4->thread; /* enough of forth context */
    /* <body> */
    {
        if (mode&K12_EMU_QUERY)
        {
            p4_strncpy (str, p4_version_string (), 80); /* << REGTH usage */
            P4_REGTH_RESTORE; return (K12_GEN_OK);
        }
    }
    P4_REGTH_RESTORE; return (K12_GEN_OK);
}

/** BEWARE: helper function, make sure to save LEM context and setup the
 *          forth's REGTH (if it uses a cpu register for that).
 * this function will create an eventbuffer that will land
 * in the term-k12 getevent loop - it will work as if the
 * the string has been magically typed on the terminal. It
 * can be used to send a string from LEM-context (during a
 * config-request) to the PFE-context (in its getevent loop).
 * The string will be implicitly terminated with a CR to start
 * execution in the engine's interpret_loop.
 */
status_t
p4_emu_sendme_command (k12_emu_type_t* emul_id, const u8_t* str, int len)
{
    register status_t e;
    char* data_buf;
    int k12_sap;
    struct k12_priv* k12p;
    P4_enter2 ("sendme_command: '%.*s'", len, str);

    k12p = P4_K12_PRIV(p4TH);
    if (! FENCE) /* done p4_boot_system ? */
        k12_sap = K12_FORTH_COMMAND_SAP;
    else
        k12_sap = k12p->rx_dataSAP;

    /* send the string as XDAT */

    e=k12EmuLowBufferGet (emul_id,
                          2 + len + sizeof(k12_emu_msg_subhead_t),
                          &data_buf);
    if (e) {  P4_returns1 (e, "status %i", e); }

    p4_memset (data_buf,0,sizeof(k12_emu_msg_subhead_t));
    p4_memcpy (data_buf + sizeof(k12_emu_msg_subhead_t), str, len);
    p4_memcpy (data_buf + sizeof(k12_emu_msg_subhead_t) + len, " \n", 2);
    ((k12_emu_msg_subhead_t*)data_buf) ->type = K12_EMU_XDAT_REQ;

    e=k12EmuLowEventPut (emul_id,
                         k12_sap,
                         data_buf,
                         2 + len + sizeof (k12_emu_msg_subhead_t),
                         K12_EMU_NOOPT);
    if (e != K12_GEN_OK && e != K12_EMU_LOW_HIGH_WATER)
    { P4_warn1 ("k12EmuLowEventPut failed (%x) [data not sent]", e); }

# if 0
    /* NEW: \n now added above */
    /* now send the XDAT-enter command */

    e=k12EmuLowBufferGet (emul_id,
                          sizeof(k12_emu_msg_subhead_t),
                          &data_buf);
    if (e) { P4_returns1 (e, "status %i", e); }

    p4_memset (data_buf, 0, sizeof(k12_emu_msg_subhead_t));
    ((k12_emu_msg_subhead_t*)data_buf) ->type = K12_EMU_XDAT_REQ;

    e=k12EmuLowEventPut (emul_id, k12_sap,
                         data_buf,
                         sizeof (k12_emu_msg_subhead_t),
                         K12_EMU_NOOPT);
    if (e != K12_GEN_OK && e != K12_EMU_LOW_HIGH_WATER)
    { P4_warn1 ("k12EmuLowEventPut failed (%x) [enter not sent]", e); }
#endif

    /* done */
    P4_returns1 (e, "status %i", e);
}

/* BEWARE: called from LEM context */
status_t
p4_emu_command (k12_emu_type_t* emul_id,
                u32_t mode,
                u8_t* str,
                s8_t* addr)
{
    /* <head> */
    register status_t e;
    p4_emu_t* p4;
    /* only partial P4_CALLER_SAVE necessary - just REGTH of the Forth VM */
    P4_REGTH_MKSAVED;

    e=k12EmuLowBaseGet (emul_id, (void*)&p4);
    if (e) return e;

    p4TH = &p4->thread; /* enough of forth context */
    /* <body> */
    {
        struct k12_priv* k12p;
        k12p = P4_K12_PRIV(p4TH);

        if (mode&K12_EMU_CONFIG)
        {
            if (p4->config_blocked)
            { P4_REGTH_RESTORE; return _K12_EAGAIN; }

            e=p4_emu_sendme_command (emul_id, str, p4_strlen(str)); /* REGTH!!*/
            P4_REGTH_RESTORE; P4_returns1 (e, "status %i", e);
        }

        if (mode&K12_EMU_QUERY)
        {
            if (FENCE)
            {
                P4_enter1 ("get '%s'", k12p->answerbuf);
                p4_memcpy (str, k12p->answerbuf, k12p->answeridx+1);
                str[k12p->answeridx+1] = '\0';
                P4_REGTH_RESTORE; P4_returns (K12_GEN_OK, "done ok");
            }else{
                p4_strcpy (str, "\\ not running");
                P4_REGTH_RESTORE; P4_returns (_K12_EAGAIN, "query ignored");
            }
        }
    }
    P4_REGTH_RESTORE; P4_returns (K12_GEN_OK, "nothing to do -> return");
}

/* BEWARE: called from LEM context */
status_t
p4_emu_scriptfile (k12_emu_type_t* emul_id,
                   u32_t mode,
                   u8_t* str,
                   s8_t* addr)
{
    register status_t e;
    p4_emu_t* p4;

    e=k12EmuLowBaseGet (emul_id, (void*)&p4);
    if (e) return e;

    if (mode&K12_EMU_CONFIG)
    {
        if (p4->config_blocked) return _K12_EAGAIN;
        if (!*str || *str == ' ' || ! p4_strcmp (str, "\"\"")) str = "";
        p4->has.config.scriptfile =
            p4_change_option_string ("SCRIPT-FILE", 11, str, &p4->session);
    }
    else if (mode&K12_EMU_QUERY)
    {
        if (! p4->has.config.scriptfile)
            p4->has.config.scriptfile = p4_lookup_option_string (
                "SCRIPT-FILE", 11, 0, &p4->session);
        *str = '\0';
        if (p4->has.config.scriptfile && *p4->has.config.scriptfile)
            p4_strncpy (str, *p4->has.config.scriptfile, 255);
    }
    return K12_GEN_OK;
}

/* BEWARE: called from LEM context */
status_t
p4_emu_scriptpath (k12_emu_type_t* emul_id,
                   u32_t mode,
                   u8_t* str,
                   s8_t* addr)
{
    register status_t e;
    p4_emu_t* p4;

    e=k12EmuLowBaseGet (emul_id, (void*)&p4);
    if (e) return e;

    if (mode&K12_EMU_CONFIG)
    {
        if (p4->config_blocked) return _K12_EAGAIN;
        if (!*str || *str == ' ') str = PFE_PKGDATADIR;
        p4->has.config.scriptpath =
            p4_change_option_string ("INC-PATH", 8, str, &p4->session);
    }
    else if (mode&K12_EMU_QUERY)
    {
        if (! p4->has.config.scriptpath)
            p4->has.config.scriptpath = p4->session.inc_paths;
        /*  p4_lookup_option_string ("INC-PATH", 8, 0, &p4->session);  */
        p4_strcpy (str, PFE_PKGDATADIR);
        if (p4->has.config.scriptpath && *p4->has.config.scriptpath)
            p4_strncpy (str, *p4->has.config.scriptpath, 255);
    }

    return K12_GEN_OK;
}


/* BEWARE: called from LEM context */
status_t
p4_emu_do_config (k12_emu_type_t* emul_id,
                  u32_t mode,
                  u32_t* pval,
                  s8_t* addr)
{
    /* <head> */
    register status_t e;
    p4_emu_t* p4;
    /* only partial P4_CALLER_SAVE necessary - just REGTH of the Forth VM */
    P4_REGTH_MKSAVED;

    e=k12EmuLowBaseGet (emul_id, (void*)&p4);
    if (e) return e;
    if (mode&K12_EMU_CONFIG && p4->config_blocked)
    { P4_REGTH_RESTORE; return _K12_EAGAIN; }

    p4TH = &p4->thread; /* enough forth context */
    /* <body> */

    P4_enter2 ("addr/idx=%x (#'%8s')", addr, 8);

    /**/ if (p4_memequal(addr,"DictLeft",8))
    {
        if (mode&K12_EMU_QUERY) *pval = (u32_t)(PFE.dictlimit - PFE.dp);
    }
    else if (p4_memequal(addr,"sp-depth",8))
    {
#    ifndef P4_REGSP
        if (mode&K12_EMU_QUERY) *pval = (u32_t)(PFE.s0 - PFE.sp);
#    else
        if (mode&K12_EMU_QUERY) *pval = -1;
#    endif
    }
    else if (p4_memequal(addr,"rp-depth",8))
    {
#    ifndef P4_REGRP
        if (mode&K12_EMU_QUERY) *pval = (u32_t)(PFE.r0 - PFE.rp);
#    else
        if (mode&K12_EMU_QUERY) *pval = -1;
#    endif
    }
    else if (p4_memequal(addr,"ttx-base",8))
    {
        if (mode&K12_EMU_CONFIG && *pval) PFE.base = *pval;
        if (mode&K12_EMU_QUERY) *pval = (u32_t)(PFE.base);
    }
    else if (p4_memequal(addr,"p4-state",8))
    {
        if (mode&K12_EMU_CONFIG) PFE.state = *pval;
        if (mode&K12_EMU_QUERY) *pval = (u32_t)(PFE.state);
    }
    else if (p4_memequal(addr, "thr-size",8))
    {
        if (mode&K12_EMU_CONFIG) if(!PFE_MEM) PFE.set->total_size = *pval;
        if (mode&K12_EMU_QUERY) *pval = PFE.set->total_size;
    }
    else if (p4_memequal(addr, "sp-size-",8))
    {
        if (mode&K12_EMU_CONFIG) if(!PFE_MEM) PFE.set->stack_size = *pval;
        if (mode&K12_EMU_QUERY) *pval = PFE.set->stack_size;
    }
    else if (p4_memequal(addr, "rp-size-",8))
    {
        if (mode&K12_EMU_CONFIG) if(!PFE_MEM) PFE.set->ret_stack_size = *pval;
        if (mode&K12_EMU_QUERY) *pval = PFE.set->ret_stack_size;
    }

    P4_REGTH_RESTORE;
    if (e)
    {
        P4_returns1 (e, "config subcall failed (#'%8s')", addr);
    }else{
        P4_returns (K12_GEN_OK, "done");
    }
}

/* ................................................................ */

/* BEWARE: called from LEM context */
status_t
p4_config_msc_int (k12_emu_type_t* emul_id,
                   u32_t mode,
                   u32_t* pval,
                   s8_t* addr)
{
    register status_t e;
    u32_t arg_idx = (u32_t) (void *) (addr);
    p4_emu_t* p4;
    /* NO P4_CALLER_SAVEALL necessary - no forth VM register used */

    if ((e = k12EmuLowBaseGet (emul_id, (void *) &p4)))
        return e;

    if ((mode & K12_EMU_CONFIG) && p4->config_blocked)
        return _K12_EAGAIN;

    if (arg_idx > 0 && arg_idx <= MSC_MAX_VARS)
    {
        if (mode & K12_EMU_CONFIG)
            p4->msc_vars.msc_ints[arg_idx - 1] = *pval;
        if (mode & K12_EMU_QUERY)
            *pval = p4->msc_vars.msc_ints[arg_idx - 1];
    }else{
        P4_warn1 ("Parameter arg_idx out of range (%d)", arg_idx);
    }
    return K12_GEN_OK;
}

/* BEWARE: called from LEM context */
status_t
p4_config_msc_string (k12_emu_type_t* emul_id,
                      u32_t mode,
                      u8_t* pval,
                      s8_t* addr)
{
    register status_t e;
    u32_t arg_idx = (u32_t) (void *) (addr);
    p4_emu_t* p4;
    /* NO P4_CALLER_SAVEALL necessary - no forth VM register used */

    if ((e = k12EmuLowBaseGet (emul_id, (void *) &p4)))
        return e;

    if ((mode & K12_EMU_CONFIG) && p4->config_blocked)
        return _K12_EAGAIN;

    if (arg_idx > 0 && arg_idx <= MSC_MAX_VARS)
    {
        if (mode & K12_EMU_CONFIG)
        {
            int len = pval? p4_strlen (pval): 0;
            p4_memcpy (p4->msc_vars.msc_strings[arg_idx - 1] + 1, pval, len);
            p4->msc_vars.msc_strings[arg_idx - 1][0] = len;
        }
        if (mode & K12_EMU_QUERY)
        {
            int len = p4->msc_vars.msc_strings[arg_idx - 1][0];
            p4_memcpy (pval, p4->msc_vars.msc_strings[arg_idx - 1] + 1, len);
            pval[len] = '\0';
        }
    }else{
        P4_warn1 ("Parameter arg_idx out of range (%d)", arg_idx);
    }
    return K12_GEN_OK;
}

/* BEWARE: called from LEM context */
status_t
p4_config_msc_timer (k12_emu_type_t* emul_id,
                     u32_t mode,
                     u32_t* pval,
                     s8_t* addr)
{
    register status_t e;
    u32_t arg_idx = (u32_t) (void *) (addr);
    p4_emu_t* p4;
    /* NO P4_CALLER_SAVEALL necessary - no forth VM register used */

    if ((e = k12EmuLowBaseGet (emul_id, (void *) &p4)))
        return e;

    if ((mode & K12_EMU_CONFIG) && p4->config_blocked)
        return _K12_EAGAIN;

    if (arg_idx > 0 && arg_idx <= MSC_MAX_TIMERS)
    {
        if (mode & K12_EMU_CONFIG)
            p4->msc_vars.msc_timers[arg_idx - 1] = *pval;
        if (mode & K12_EMU_QUERY)
            *pval = p4->msc_vars.msc_timers[arg_idx - 1];
    }else{
        P4_warn1 ("Parameter arg_idx out of range (%d)", arg_idx);
    }
    return K12_GEN_OK;
}

#define p4_emu_cpuipcsend      p4_emu_ignore
#define p4_emu_do_or_tm_config p4_emu_do_config
#define p4_emu_hmsc_action     p4_emu_ignore
#define p4_emu_tm_action       p4_emu_ignore
#define p4_emu_tm_config       p4_emu_do_config
#define p4_emu_verdict_action  p4_emu_ignore
#include <pfe/emu-menu-table.c>

k12_emu_in_out_t p4_emu_inp_tab[] =
{                             /* name and type of inputs */
    { "Lower0",   K12_EMU_LOWER },
    { "Lower1",   K12_EMU_LOWER },
    { "Lower2",   K12_EMU_LOWER },
    { "Lower3",   K12_EMU_LOWER },
    { "Lower4",   K12_EMU_LOWER },
    { "Lower5",   K12_EMU_LOWER },
    { "Lower6",   K12_EMU_LOWER },
    { "Lower7",   K12_EMU_LOWER },
    { "Lower8",   K12_EMU_LOWER },
    { "Lower9",   K12_EMU_LOWER },
    { "Upper0",   K12_EMU_UPPER },
    { "Upper1",   K12_EMU_UPPER },
    { "Upper2",   K12_EMU_UPPER },
    { "Upper3",   K12_EMU_UPPER },
    { "Upper4",   K12_EMU_UPPER },
    { "Upper5",   K12_EMU_UPPER },
    { "Upper6",   K12_EMU_UPPER },
    { "Upper7",   K12_EMU_UPPER },
    { "Upper8",   K12_EMU_UPPER },
    { "Upper9",   K12_EMU_UPPER },
    { "Lower10",  K12_EMU_LOWER },
    { "Lower11",  K12_EMU_LOWER },
    { "Lower12",  K12_EMU_LOWER },
    { "Lower13",  K12_EMU_LOWER },
    { "Lower14",  K12_EMU_LOWER },
    { "Lower15",  K12_EMU_LOWER },
    { "Lower16",  K12_EMU_LOWER },
    { "Lower17",  K12_EMU_LOWER },
    { "Lower18",  K12_EMU_LOWER },
    { "Lower19",  K12_EMU_LOWER },
    { "Upper10",  K12_EMU_UPPER },
    { "Upper11",  K12_EMU_UPPER },
    { "Upper12",  K12_EMU_UPPER },
    { "Upper13",  K12_EMU_UPPER },
    { "Upper14",  K12_EMU_UPPER },
    { "Upper15",  K12_EMU_UPPER },
    { "Upper16",  K12_EMU_UPPER },
    { "Upper17",  K12_EMU_UPPER },
    { "Upper18",  K12_EMU_UPPER },
    { "Upper19",  K12_EMU_UPPER },
    { "Lower20",  K12_EMU_LOWER },
    { "Lower21",  K12_EMU_LOWER },
    { "Lower22",  K12_EMU_LOWER },
    { "Lower23",  K12_EMU_LOWER },
    { "Lower24",  K12_EMU_LOWER },
    { "Lower25",  K12_EMU_LOWER },
    { "Lower26",  K12_EMU_LOWER },
    { "Lower27",  K12_EMU_LOWER },
    { "Lower28",  K12_EMU_LOWER },
    { "Lower29",  K12_EMU_LOWER },
    { "Upper20",  K12_EMU_UPPER },
    { "Upper21",  K12_EMU_UPPER },
    { "Upper22",  K12_EMU_UPPER },
    { "Upper23",  K12_EMU_UPPER },
    { "Upper24",  K12_EMU_UPPER },
    { "Upper25",  K12_EMU_UPPER },
    { "Upper26",  K12_EMU_UPPER },
    { "Upper27",  K12_EMU_UPPER },
    { "Upper28",  K12_EMU_UPPER },
    { "Upper29",  K12_EMU_UPPER },
    { "Lower30",  K12_EMU_LOWER },
    { "Lower31",  K12_EMU_LOWER },
    { "Lower32",  K12_EMU_LOWER },
    { "stdin",    K12_EMU_LOWER },
};

k12_emu_in_out_t p4_emu_out_tab[] =
{                             /* name and type of outputs */
    { "Lower0",   K12_EMU_LOWER },
    { "Lower1",   K12_EMU_LOWER },
    { "Lower2",   K12_EMU_LOWER },
    { "Lower3",   K12_EMU_LOWER },
    { "Lower4",   K12_EMU_LOWER },
    { "Lower5",   K12_EMU_LOWER },
    { "Lower6",   K12_EMU_LOWER },
    { "Lower7",   K12_EMU_LOWER },
    { "Lower8",   K12_EMU_LOWER },
    { "Lower9",   K12_EMU_LOWER },
    { "Upper0",   K12_EMU_UPPER },
    { "Upper1",   K12_EMU_UPPER },
    { "Upper2",   K12_EMU_UPPER },
    { "Upper3",   K12_EMU_UPPER },
    { "Upper4",   K12_EMU_UPPER },
    { "Upper5",   K12_EMU_UPPER },
    { "Upper6",   K12_EMU_UPPER },
    { "Upper7",   K12_EMU_UPPER },
    { "Upper8",   K12_EMU_UPPER },
    { "Upper9",   K12_EMU_UPPER },
    { "Lower10",  K12_EMU_LOWER },
    { "Lower11",  K12_EMU_LOWER },
    { "Lower12",  K12_EMU_LOWER },
    { "Lower13",  K12_EMU_LOWER },
    { "Lower14",  K12_EMU_LOWER },
    { "Lower15",  K12_EMU_LOWER },
    { "Lower16",  K12_EMU_LOWER },
    { "Lower17",  K12_EMU_LOWER },
    { "Lower18",  K12_EMU_LOWER },
    { "Lower19",  K12_EMU_LOWER },
    { "Upper10",  K12_EMU_UPPER },
    { "Upper11",  K12_EMU_UPPER },
    { "Upper12",  K12_EMU_UPPER },
    { "Upper13",  K12_EMU_UPPER },
    { "Upper14",  K12_EMU_UPPER },
    { "Upper15",  K12_EMU_UPPER },
    { "Upper16",  K12_EMU_UPPER },
    { "Upper17",  K12_EMU_UPPER },
    { "Upper18",  K12_EMU_UPPER },
    { "Upper19",  K12_EMU_UPPER },
    { "Lower20",  K12_EMU_LOWER },
    { "Lower21",  K12_EMU_LOWER },
    { "Lower22",  K12_EMU_LOWER },
    { "Lower23",  K12_EMU_LOWER },
    { "Lower24",  K12_EMU_LOWER },
    { "Lower25",  K12_EMU_LOWER },
    { "Lower26",  K12_EMU_LOWER },
    { "Lower27",  K12_EMU_LOWER },
    { "Lower28",  K12_EMU_LOWER },
    { "Lower29",  K12_EMU_LOWER },
    { "Upper20",  K12_EMU_UPPER },
    { "Upper21",  K12_EMU_UPPER },
    { "Upper22",  K12_EMU_UPPER },
    { "Upper23",  K12_EMU_UPPER },
    { "Upper24",  K12_EMU_UPPER },
    { "Upper25",  K12_EMU_UPPER },
    { "Upper26",  K12_EMU_UPPER },
    { "Upper27",  K12_EMU_UPPER },
    { "Upper28",  K12_EMU_UPPER },
    { "Upper29",  K12_EMU_UPPER },
    { "Lower30",  K12_EMU_LOWER },
    { "Lower31",  K12_EMU_LOWER },
    { "Lower32",  K12_EMU_LOWER },
    { "stdin",    K12_EMU_LOWER },
};

k12_emu_table_t p4_emu_table =
{
    DIM (p4_emu_inp_tab), p4_emu_inp_tab,
    DIM (p4_emu_out_tab), p4_emu_out_tab,
    DIM (p4_emu_menu_tab), p4_emu_menu_tab
};

/* This table is needed to connect PFE with another emulation.
   Useful for MSC! */
char* k12_pfe_sap_table[] = {
    "Lower0",
    "Lower1",
    "Lower2",
    "Lower3",
    "Lower4",
    "Lower5",
    "Lower6",
    "Lower7",
    "Lower8",
    "Lower9",
    "Upper0",
    "Upper1",
    "Upper2",
    "Upper3",
    "Upper4",
    "Upper5",
    "Upper6",
    "Upper7",
    "Upper8",
    "Upper9",
    "Lower10",
    "Lower11",
    "Lower12",
    "Lower13",
    "Lower14",
    "Lower15",
    "Lower16",
    "Lower17",
    "Lower18",
    "Lower19",
    "Upper10",
    "Upper11",
    "Upper12",
    "Upper13",
    "Upper14",
    "Upper15",
    "Upper16",
    "Upper17",
    "Upper18",
    "Upper19",
    "Lower20",
    "Lower21",
    "Lower22",
    "Lower23",
    "Lower24",
    "Lower25",
    "Lower26",
    "Lower27",
    "Lower28",
    "Lower29",
    "Upper20",
    "Upper21",
    "Upper22",
    "Upper23",
    "Upper24",
    "Upper25",
    "Upper26",
    "Upper27",
    "Upper28",
    "Upper29",
    "Lower30",
    "Lower31",
    "Lower32",
    "stdin",
    NULL };                     /* ? */

/* ............................................................ */

status_t
emuInit (k12_emu_type_t* emul_id)
{
    register status_t e;
    p4_emu_t* p4;
    /* NO P4_CALLER_SAVEALL necessary - no forth VM register used */

# ifdef DEBUG
    K12LogInit(0, 64, K12_LOG_DIRECT);
# endif

    e=k12EmuLowMemGet (emul_id, sizeof (p4_emu_t), (s8_t**) &p4);
    if (e) return (K12_GEN_NOMEM);

    p4_memset (p4, 0, sizeof (p4_emu_t));
    /* pEmul->private.state = K12_EMU_IDLE; */ /* -> term-k12.c */

    /* must be called *before* TableInd - 'cause we want to use OPT space! */
    p4_SetOptions (&p4->session, sizeof(p4->session) + sizeof (p4->extra),
                   1, (const char**) &"p4th");

    /* don't forget about prelinked modules, e.g. zchar-ext */
    { extern p4Words P4WORDS(internal);
    p4_SetModules (&p4->session, &(P4WORDS(internal)));
    }

    /* pass table of features - menusystem may write to those vars now ! */
    e=k12EmuLowTableInd (emul_id, &p4_emu_table, (s8_t *)p4);
    if (e) return (e);

    K12LogDebug1(1, "p4.emuInit",
      "privat data size=%08lX\n", (u32_t)sizeof(p4_emu_t));

    p4->thread.term = &p4_term_k12; /* the gui interface */

    return (K12_GEN_OK);
}

status_t
emuDeinit (k12_emu_type_t* emul_id)
{
    register status_t e;
    p4_emu_t* p4_emu;

    e=k12EmuLowBaseGet (emul_id, (void*)&p4_emu);
    if (e) return (e);

    /* p4TH is not enough context IFF calling userspace ATEXIT code */
    {
        P4_CALLER_SAVEALL;
#      ifdef P4_REGTH
        p4TH = &p4_emu->thread;
#      endif
#      ifdef P4_REGRP
        p4RP = PFE.r0;
#      endif
#      ifdef P4_REGSP
        p4SP = PFE.s0;
#      endif
#      ifdef P4_REGFP
        p4FP = PFE.f0;
#      endif
        if (PFE_MEM)
        {
            if (FENCE)
                FX (p4_closeall_files);
            else { P4_warn ("killed before (re)boot complete"); }

            PFE.atexit_cleanup ();
        }else { P4_fail ("killed before boot, bad setup? no ESE connection?");}

        P4_CALLER_RESTORE;
    }
    return K12_GEN_OK;
}

status_t
emuStart (k12_emu_type_t* emul_id)
{
    register status_t e;
    p4_emu_t* p4;
    /* P4_CALLER_SAVEALL not necessary - it has its own thread-context */

    e=k12EmuLowBaseGet (emul_id, (void*)&p4);
    if (e) return (e);

    /* did someone reset this? */
    if (p4->session.inc_paths && *p4->session.inc_paths &&
        !**p4->session.inc_paths) /* an empty strings as INC-PATH */
        p4_change_option_string ("INC-PATH", 8, PFE_PKGDATADIR, &p4->session);

    p4->session.cpus = 1;
    /*  p4->session.cpu[0] = &p4->thread; */
    p4->thread.set = &p4->session;
    p4->thread.priv = &p4->private;
    p4->private.emu = emul_id;

    p4->private.validation_req = 0;
    {
        int i = 0;
        k12_emu_event_t ev; k12_emu_msg_subhead_t* msg; u32_t len;
        P4_debug (13, "K12_EMU_VALIDATION_REQ used - set timeout to 200*HZ");
        k12EmuLowTimerStart (emul_id, 0x4e77 /* K12_EMU_VALIDATION_REQ */,
                             200*CLOCKS_PER_SEC);
        while (1)
        {
            k12EmuLowEventGet(emul_id, &ev, (char**) &msg, &len, 0);
            switch (msg->type)
            {
            case K12_EMU_MSG_TIMER:
            case 0x4e77:
                P4_warn2 ("0x%x : OOPS validation_req timeout :"
                          " CLOCKS_PER_SEC = %lu",
                          msg->type, ((long) CLOCKS_PER_SEC));
                taskDelay(CLOCKS_PER_SEC);
                if (p4->private.state == K12_EMU_WARNING)
                {
                    p4->private.state = K12_EMU_ERROR_CASE;
                    P4_debug (13, "set EMU_ERROR - go creeping startup mode");
                } else{
                    p4->private.state = K12_EMU_WARNING;
                    P4_debug (13, "set EMU_WARNING -  new timeout 1000*HZ");
                    k12EmuLowTimerStart (emul_id, 0x4e77, 1000*CLOCKS_PER_SEC);
                }
                continue; /*NOT fallthrough*/
            case K12_EMU_VALIDATION_REQ:
                k12EmuLowTimerStop(emul_id, 0x4e77 /*K12_EMU_VALIDATION_REQ*/);
                if (k12EmuLowEventPut(emul_id, ev, (char*) msg, len,
                                      K12_EMU_NOOPT) == K12_GEN_OK)
                { p4->private.validation_req ++; }

                return p4_Exec(& p4->thread); /* outer-API (SAVEALL) */

            default:
                P4_warn2 ("UNK START MSG type->0x%x len->%u",
                          msg->type, len);
                /*fallthrough*/
            case K12_EMU_XDAT_REQ:
                /* config-cb to command-msg via xdat-req */
                if (++i > 3)
                    P4_warn ("START DELAYED (and requeue message) (HZ/2)");
                switch (p4->private.state)
                {
                case K12_EMU_ERROR_CASE:
                    taskDelay(CLOCKS_PER_SEC*16); break;
                case K12_EMU_WARNING:
                    taskDelay(CLOCKS_PER_SEC*2); break;
                default:
                    taskDelay(CLOCKS_PER_SEC/2); break;
                }
                if (k12EmuLowEventPut(emul_id, ev, (char*) msg, len,
                                      K12_EMU_NOOPT))
                { P4_fail ("could not requeue message"); }
                continue;
            }
        }
    }
}

status_t
p4_conftrace (k12_emu_type_t* emulId,
              u32_t request,
              u32_t* value,
              u32_t* addr)
{
    if (request == K12_EMU_QUERY)
    {
        *value = (uint32_t) K12EmuTraceLevelGet(emulId);
        return (K12_GEN_OK);
    }
    return (K12EmuTraceLevelSet (emulId, *value));
}

/*@} */
