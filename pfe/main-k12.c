/** 
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @author Tektronix CTE               @(#) %derived_by: guidod %
 *  @version %version: 5.28 %
 *    (%date_modified: Mon Mar 12 14:51:13 2001 %)
 */
/*@{ */
#if defined(__version_control__) && defined(__GNUC__)
static char * id __attribute__((unused)) =
"@(#) $Id: main-k12.c,v 0.30.86.1 2001-03-12 15:34:19 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/incl-ext.h>

#include <stddef.h> /*offsetof*/
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <taskLib.h>
#include <sysLib.h>
#include <time.h>
#include <K12/emul.h>

#include <pfe/main-sub.h>
#include <pfe/main-k12.h>
#include <pfe/term-k12.h>
#include <pfe/logging.h>

#include <pfe/engine-sub.h>
#include <pfe/version-sub.h>

#ifdef  CLOCKS_PER_SEC_BUG
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

/* BEWARE: called from LEM context */
k12_status_t
p4_emu_state (k12_emu_type_t *emulId,
              u32_t mode,
              u32_t *value,
              s8_t *addr)
{
    p4_emu_t *p4_emu;

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
            if (! p4_emu->thread.fence)
            {   /* boot_system not through */
                strcpy (value, "not booted");
            } else if (p4_emu->thread.input.source_id != 0 
                       && p4_emu->thread.input.source_id != -1)
            {   /* loading a file, source_id is a p4_File */
                sprintf (value, "loading %.*s : %u", 240,
                         ((p4_File*)p4_emu->thread.input.source_id)->name,
                         ((p4_File*)p4_emu->thread.input.source_id)->n);
            }else{
                strcpy (value, "not loaded");
            } break;
        case K12_EMU_IDLE:
            strcpy (value, "Forth System Ready");
            break;
        case K12_EMU_ACTIVE:
            strcpy (value, "Test Manager Active");
            break;
        case K12_EMU_WARNING:
            strcpy (value, "Warning");
            break;
        case K12_EMU_ERROR_CASE:
            strcpy (value, "Error");
            break;
        case K12_EMU_NO_ANSWER:
            strcpy (value, "No Answer");
            break;
        default:
            strcpy (value, "(not available)");
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
    void* save;
    e=k12EmuLowBaseGet (emul_id, (void*)&p4);
    if (e) return e;
    save = p4TH; /* p4TH is a CPU register, restore before return!*/
    p4TH = &p4->thread; /* enough of forth context */
    /* <body> */
    {
	if (mode&K12_EMU_QUERY)
	{
	    strncpy (str, p4_version_string (), 80);
	    p4TH = save; return (K12_GEN_OK);
	}
    }
    p4TH = save; return (K12_GEN_OK);
}

/** BEWARE: helper function, make sure to save LEM context 
 *          (so that this routine runs in p4TH mode atleast)
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

    memset (data_buf,0,sizeof(k12_emu_msg_subhead_t));
    memcpy (data_buf + sizeof(k12_emu_msg_subhead_t), str, len);
    memcpy (data_buf + sizeof(k12_emu_msg_subhead_t) + len, " \n", 2);
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
	    
    memset (data_buf, 0, sizeof(k12_emu_msg_subhead_t));
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
    void* save;
    e=k12EmuLowBaseGet (emul_id, (void*)&p4);
    if (e) return e;
    save = p4TH; /* p4TH is a CPU register, restore before return!*/
    p4TH = &p4->thread; /* enough of forth context */
    /* <body> */
    {
	struct k12_priv* k12p;
	k12p = P4_K12_PRIV(p4TH);

	if (mode&K12_EMU_CONFIG)
	{
	    if (p4->config_blocked) 
	    { p4TH = save; return _K12_EAGAIN; }

            e=p4_emu_sendme_command (emul_id, str, strlen(str));
	    p4TH = save; P4_returns1 (e, "status %i", e);
	}
	
	if (mode&K12_EMU_QUERY)
	{
	    if (FENCE)
	    {
		P4_enter1 ("get '%s'", k12p->answerbuf);
		memcpy (str, k12p->answerbuf, k12p->answeridx+1);
		str[k12p->answeridx+1] = '\0';
		p4TH = save; P4_returns (K12_GEN_OK, "done ok");
	    }else{
		strcpy (str, "\\ not running");
		p4TH = save; P4_returns (_K12_EAGAIN, "query ignored");
	    }
	}
    }
    p4TH = save; P4_returns (K12_GEN_OK, "nothing to do -> return");
}

/* BEWARE: called from LEM context */
status_t
p4_emu_scriptfile (k12_emu_type_t* emul_id, 
		   u32_t mode, 
		   u8_t* str, 
		   s8_t* addr)
{
    char buf [256] = "BOOT-SCRIPT: ";
    
    if (!str || strlen(str) <= 1) return K12_GEN_OK;
    if (mode&K12_EMU_QUERY) return K12_GEN_ILLMODE;

    strncat (buf, str, 256);
    strncat (buf, " BOOT-SCRIPT@ INCLUDED", 256);
    return p4_emu_command (emul_id, mode, buf, addr);
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
    void* save;

    e=k12EmuLowBaseGet (emul_id, (void*)&p4);
    if (e) return e;
    if (mode&K12_EMU_CONFIG && p4->config_blocked)
        return _K12_EAGAIN;
    save = p4TH;  /* p4TH is a CPU register, restore before return!*/
    p4TH = &p4->thread; /* enough forth context */
    /* <body> */

    P4_enter2 ("addr/idx=%x (#'%8s')", addr, 8);

    /**/ if (!memcmp(addr,"DictLeft",8))
    {
        if (mode&K12_EMU_QUERY) *pval = (u32_t)(PFE.dictlimit - PFE.dp);
    } 
    else if (!memcmp(addr,"sp-depth",8))
    {
#    ifndef P4_REGSP
        if (mode&K12_EMU_QUERY) *pval = (u32_t)(PFE.s0 - PFE.sp);
#    else
        if (mode&K12_EMU_QUERY) *pval = -1;
#    endif
    } 
    else if (!memcmp(addr,"rp-depth",8))
    {
#    ifndef P4_REGRP
        if (mode&K12_EMU_QUERY) *pval = (u32_t)(PFE.r0 - PFE.rp);
#    else
        if (mode&K12_EMU_QUERY) *pval = -1;
#    endif
    } 
    else if (!memcmp(addr,"ttx-base",8))
    {
        if (mode&K12_EMU_CONFIG && *pval) PFE.base = *pval;
        if (mode&K12_EMU_QUERY) *pval = (u32_t)(PFE.base);
    } 
    else if (!memcmp(addr,"p4-state",8))
    {
        if (mode&K12_EMU_CONFIG) PFE.state = *pval;
        if (mode&K12_EMU_QUERY) *pval = (u32_t)(PFE.state);
    } 
    else if (!memcmp(addr, "thr-size",8))
    {
        if (mode&K12_EMU_CONFIG) if(!PFE_MEM) PFE.set->total_size = *pval;
        if (mode&K12_EMU_QUERY) *pval = PFE.set->total_size;
    } 
    else if (!memcmp(addr, "sp-size-",8))
    {
        if (mode&K12_EMU_CONFIG) if(!PFE_MEM) PFE.set->stack_size = *pval;
        if (mode&K12_EMU_QUERY) *pval = PFE.set->stack_size;
    } 
    else if (!memcmp(addr, "rp-size-",8))
    {
        if (mode&K12_EMU_CONFIG) if(!PFE_MEM) PFE.set->ret_stack_size = *pval;
        if (mode&K12_EMU_QUERY) *pval = PFE.set->ret_stack_size;
    }

    if (e)
    {
	p4TH = save; P4_returns1 (e, "config subcall failed (#'%8s')", addr);
    }else{
	p4TH = save; P4_returns (K12_GEN_OK, "done");
    }
}

/* ................................................................ */

/* BEWARE: called from LEM context */
status_t
p4_tmp_config_msc_int (k12_emu_type_t* emul_id,
                       u32_t mode,
                       u32_t* pval,
                       s8_t* addr)
{
    register status_t e;
    u32_t arg_idx = (u32_t) (void *) (addr);
    p4_emu_t* p4;

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
p4_tmp_config_msc_string (k12_emu_type_t* emul_id,
                          u32_t mode,
                          u8_t* pval,
                          s8_t* addr)
{
    register status_t e;
    u32_t arg_idx = (u32_t) (void *) (addr);
    p4_emu_t* p4;

    if ((e = k12EmuLowBaseGet (emul_id, (void *) &p4)))
        return e;

    if ((mode & K12_EMU_CONFIG) && p4->config_blocked)
        return _K12_EAGAIN;

    if (arg_idx > 0 && arg_idx <= MSC_MAX_VARS)
    {
        if (mode & K12_EMU_CONFIG)
        {
            strncpy (p4->msc_vars.msc_strings[arg_idx - 1], pval, 81);
            p4->msc_vars.msc_strings[arg_idx - 1][81] = '\0';
        }
        if (mode & K12_EMU_QUERY)
        {
            strncpy (pval, p4->msc_vars.msc_strings[arg_idx - 1], 81);
            pval[81] = '\0';
        }
    }else{
        P4_warn1 ("Parameter arg_idx out of range (%d)", arg_idx);
    }
    return K12_GEN_OK;
}

/* BEWARE: called from LEM context */
status_t
p4_tmp_config_msc_timer (k12_emu_type_t* emul_id,
                         u32_t mode,
                         u32_t* pval,
                         s8_t* addr)
{
    register status_t e;
    u32_t arg_idx = (u32_t) (void *) (addr);
    p4_emu_t* p4;
    
    if ((e = k12EmuLowBaseGet (emul_id, (void *) &p4)))
        return e;

    if ((mode & K12_EMU_CONFIG) && p4->config_blocked)
        return _K12_EAGAIN;

    if (arg_idx > 0 && arg_idx <= MSC_MAX_TIMERS)
    {
        if (mode & K12_EMU_CONFIG)
            p4->msc_vars.msc_timers[arg_idx - 1] = *pval;
        if (mode & K12_EMU_QUERY)
            *pval = p4->msc_vars.msc_ints[arg_idx - 1];
    }else{
        P4_warn1 ("Parameter arg_idx out of range (%d)", arg_idx);
    }
    return K12_GEN_OK;
}

static k12_emu_enum_t tmStateActionEnum[]
= {
   	{ "NONE", 0 },
    	{ "START",  1 },
    	{ "STOP",  2 },
    	{ "CONTINUE",  3 },
    	{ "END",  4 },
};

/* defining config/query menu, used in emulation table below */
k12_emu_confque_t p4_emu_confque_tab []
= {
     { "ScriptFile", K12_EMU_TYPE_STRING, K12_EMU_CONFIG,
        255,
        0,0,0,
        "include scriptfile",0,(FUNCPTR)&p4_emu_scriptfile
     },
     { "General", K12_EMU_TYPE_SUBMENU, 0,
        5, /* entries */
     },
        { "Version", K12_EMU_TYPE_STRING, K12_EMU_QUERY,
           80,
           0,0,0,
           P4_VERSION, 0, (FUNCPTR) p4_emu_version
        },
        {
          "State", K12_EMU_TYPE_ENUM, K12_EMU_QUERY,
          sizeof (u32_t),
          0, 6, (k12_emu_enum_t *) p4_emu_state__k12_enum,
          (s8_t *) 0, 0, (FUNCPTR) p4_emu_state,
          0, 0
        },
        {
          "State Description", K12_EMU_TYPE_STRING, K12_EMU_QUERY,
          255,
          0, 6, (k12_emu_enum_t *) p4_emu_state__k12_enum,
          (s8_t *) 0, 0, (FUNCPTR) p4_emu_state_desc,
          0, 0
        },
     /* "Chart" and "Control" are for MSC support, not for general
        Forth programming or test manager functionality.  Therefore
        these sections are not mentioned in the feaure files pfe.fxt
        or pfe-ap1.fxt. */
        { "Chart", K12_EMU_TYPE_INT, K12_EMU_CONFIGQUERY,
           sizeof (p4cell),
           0, 1024*1024, 0,
          "reqstate", 0, (FUNCPTR) p4_emu_ignore,
        },
	{ "Control", K12_EMU_TYPE_ENUM, K12_EMU_CONFIG,
		sizeof (p4cell), 		/*size of variable */
                0, DIM(tmStateActionEnum), tmStateActionEnum, /* Min,Max,Enm */
          "tmaction", 0, (FUNCPTR) p4_emu_ignore,
	},

     { "Actions", K12_EMU_TYPE_SUBMENU, 0,
       7, /* entries */ 
     }, 
         { "HMSC", K12_EMU_TYPE_INT, K12_EMU_CONFIG,
           sizeof(p4cell),
           0, 0x7FFFFFFF, 0, /* Min,Max,Enum* */
           "set-hmsc", 0, (FUNCPTR) p4_emu_do_config,
           0, 0
         },
         { "Verdict", K12_EMU_TYPE_INT, K12_EMU_CONFIGQUERY,
           sizeof(p4cell),
           0, 0x7FFFFFFF, 0, /* Min,Max,Enum* */
           "verdict-", 0, (FUNCPTR) p4_emu_do_config,
           0, 0
         },
         { "Start", K12_EMU_TYPE_ENUM, K12_EMU_CONFIG,
           sizeof(p4cell),
           0, DIM(p4_emu_hmsc_action_enum), p4_emu_hmsc_action_enum,
           "HMSC.Start", 0, (FUNCPTR) p4_emu_ignore
         },
         { "Stop", K12_EMU_TYPE_ENUM, K12_EMU_CONFIG,
           sizeof(p4cell),
           0, DIM(p4_emu_hmsc_action_enum), p4_emu_hmsc_action_enum,
           "HMSC.Stop", 0, (FUNCPTR) p4_emu_ignore
         },
         { "Continue", K12_EMU_TYPE_ENUM, K12_EMU_CONFIG,
           sizeof(p4cell),
           0, DIM(p4_emu_hmsc_action_enum), p4_emu_hmsc_action_enum,
           "HMSC.Continue", 0, (FUNCPTR) p4_emu_ignore
         },
         { "Reload", K12_EMU_TYPE_ENUM, K12_EMU_CONFIG,
           sizeof(p4cell),
           0, DIM(p4_emu_hmsc_action_enum), p4_emu_hmsc_action_enum,
           "HMSC.Reload", 0, (FUNCPTR) p4_emu_ignore
         },
         { "Reset", K12_EMU_TYPE_ENUM, K12_EMU_CONFIG,
           sizeof(p4cell),
           0, DIM(p4_emu_hmsc_action_enum), p4_emu_hmsc_action_enum,
           "HMSC.Reset", 0, (FUNCPTR) p4_emu_ignore
         },

     { "Forth", K12_EMU_TYPE_SUBMENU, 0,
          11, /* entries */
     },

	{ "state", K12_EMU_TYPE_ENUM, K12_EMU_QUERY,
		sizeof (p4cell), 		/*size of variable */
                0, DIM(StatesEnum), StatesEnum,     /* Min,Max,Enum* */
		"p4-state", 0, (FUNCPTR) p4_emu_do_config,
		0, 			/* sizeof of var-array */
                0			/* default : interpret */
	},
	{ "total_size", K12_EMU_TYPE_INT, K12_EMU_CONFIGQUERY,
		sizeof (p4cell), 		
                8*1024, 1024*1024*1024, 0, /* Min,Max,Enum* */
                "thr-size",0, (FUNCPTR) p4_emu_do_config,
		0, 			/* sizeof of var-array */
                P4_KB*1024		/* default : options.h */

	},
	{ "sp_size", K12_EMU_TYPE_INT, K12_EMU_CONFIGQUERY,
		sizeof (p4cell), 		/*size of variable */
		1*1024, 1024*1024, 0,   /* Min,Max,Enum* */
		"sp-size-", 0, (FUNCPTR) p4_emu_do_config,
		0, 			/* sizeof of var-array */
                (P4_KB*1024 / 8) / sizeof(p4cell), /* default */
	},
	{ "rp_size", K12_EMU_TYPE_INT, K12_EMU_CONFIGQUERY,
		sizeof (p4cell), 		/*size of variable */
		1*1024, 1024*1024, 0,
		"rp-size-", 0, (FUNCPTR) p4_emu_do_config,
		0, 			/* sizeof of var-array */
                (P4_KB*1024 / 16) / sizeof(p4cell), /* default */
	},
        { "dict_left", K12_EMU_TYPE_INT, K12_EMU_QUERY,
                sizeof (p4cell),
                0, 0x7fffffff, 0,
                "DictLeft", 0, (FUNCPTR) p4_emu_do_config
        },
        { "sp_depth", K12_EMU_TYPE_INT, K12_EMU_QUERY,
                sizeof (p4cell),
                0, 0x7fffffff, 0,
                "sp-depth", 0, (FUNCPTR) p4_emu_do_config
        },
        { "rp_depth", K12_EMU_TYPE_INT, K12_EMU_QUERY,
                sizeof (p4cell),
                0, 0x7fffffff, 0,
                "rp-depth", 0, (FUNCPTR) p4_emu_do_config
        },
        { "base", K12_EMU_TYPE_INT, K12_EMU_CONFIGQUERY,
                sizeof (p4cell),
                2,16,0,
                "ttx-base", 0, (FUNCPTR) p4_emu_do_config
        },
	{ "incpath", K12_EMU_TYPE_STRING, K12_EMU_CONFIGQUERY,
                255, 			/* size of variable */
		0, 0, 0,                /* Min,Max,Enum* */
		0, offsetof(p4_emu_t, incpaths), /* Var Addr,Offset */
		(FUNCPTR) 0,
		0,			/* sizeof of var-array */
                (u32_t) PFE_PKGDATADIR
	},
        { "execute", K12_EMU_TYPE_STRING, K12_EMU_CONFIGQUERY,
                255,                    /* Len */
                0, 0, 0,
                "Forth.execute", 0, (FUNCPTR) p4_emu_command,
        },
        { "cpuipcsend", K12_EMU_TYPE_STRING, K12_EMU_CONFIG,
                255,            /* Len */
                0, 0, 0,
          "Forth.cpuipcsend", 0, (FUNCPTR) p4_emu_ignore,
        },     
/* FIXME: The remaining entries have to be deleted later! */
     { "Variables", K12_EMU_TYPE_SUBMENU, 0,
       32, /* entries */
     },
#define MSC_INT_CONSTRUCT(i) \
         { \
           "MSC_Int" #i, K12_EMU_TYPE_INT, K12_EMU_CONFIGQUERY, \
           sizeof (int), \
           0, (1<<30), 0, \
           (void *) i + 1, 0, \
           (FUNCPTR) p4_tmp_config_msc_int, \
         }

       MSC_INT_CONSTRUCT(0),
       MSC_INT_CONSTRUCT(1),
       MSC_INT_CONSTRUCT(2),
       MSC_INT_CONSTRUCT(3),
       MSC_INT_CONSTRUCT(4),
       MSC_INT_CONSTRUCT(5),
       MSC_INT_CONSTRUCT(6),
       MSC_INT_CONSTRUCT(7),
       MSC_INT_CONSTRUCT(8),
       MSC_INT_CONSTRUCT(9),
       MSC_INT_CONSTRUCT(10),
       MSC_INT_CONSTRUCT(11),
       MSC_INT_CONSTRUCT(12),
       MSC_INT_CONSTRUCT(13),
       MSC_INT_CONSTRUCT(14),
       MSC_INT_CONSTRUCT(15),
#undef MSC_INT_CONSTRUCT

#define MSC_STRING_CONSTRUCT(i) \
         { \
           "MSC_String" #i, K12_EMU_TYPE_STRING, K12_EMU_CONFIGQUERY, \
           81, \
           0, 0, 0, \
           (void *) i + 1, 0, \
           (FUNCPTR) p4_tmp_config_msc_string, \
         }

       MSC_STRING_CONSTRUCT(0),
       MSC_STRING_CONSTRUCT(1),
       MSC_STRING_CONSTRUCT(2),
       MSC_STRING_CONSTRUCT(3),
       MSC_STRING_CONSTRUCT(4),
       MSC_STRING_CONSTRUCT(5),
       MSC_STRING_CONSTRUCT(6),
       MSC_STRING_CONSTRUCT(7),
       MSC_STRING_CONSTRUCT(8),
       MSC_STRING_CONSTRUCT(9),
       MSC_STRING_CONSTRUCT(10),
       MSC_STRING_CONSTRUCT(11),
       MSC_STRING_CONSTRUCT(12),
       MSC_STRING_CONSTRUCT(13),
       MSC_STRING_CONSTRUCT(14),
       MSC_STRING_CONSTRUCT(15),
#undef MSC_STRING_CONSTRUCT

     { "Timers", K12_EMU_TYPE_SUBMENU, 0,
          MSC_MAX_TIMERS, /* entries */
     },

#define MSC_TIMER_CONSTRUCT(i) \
         { \
           "MSC_TIMER_" #i, K12_EMU_TYPE_INT, K12_EMU_CONFIGQUERY, \
           sizeof (int), \
           0, (1<<30), 0, \
           (void *) i + 1, 0, \
           (FUNCPTR) p4_tmp_config_msc_timer, \
         }

       MSC_TIMER_CONSTRUCT(0),
       MSC_TIMER_CONSTRUCT(1),
       MSC_TIMER_CONSTRUCT(2),
       MSC_TIMER_CONSTRUCT(3),
       MSC_TIMER_CONSTRUCT(4),
       MSC_TIMER_CONSTRUCT(5),
       MSC_TIMER_CONSTRUCT(6),
       MSC_TIMER_CONSTRUCT(7),
       MSC_TIMER_CONSTRUCT(8),
       MSC_TIMER_CONSTRUCT(9),
       MSC_TIMER_CONSTRUCT(10),
       MSC_TIMER_CONSTRUCT(11),
       MSC_TIMER_CONSTRUCT(12),
       MSC_TIMER_CONSTRUCT(13),
       MSC_TIMER_CONSTRUCT(14),
       MSC_TIMER_CONSTRUCT(15),
       MSC_TIMER_CONSTRUCT(16),
       MSC_TIMER_CONSTRUCT(17),
       MSC_TIMER_CONSTRUCT(18),
       MSC_TIMER_CONSTRUCT(19),
       MSC_TIMER_CONSTRUCT(20),
       MSC_TIMER_CONSTRUCT(21),
       MSC_TIMER_CONSTRUCT(22),
       MSC_TIMER_CONSTRUCT(23),
       MSC_TIMER_CONSTRUCT(24),
       MSC_TIMER_CONSTRUCT(25),
       MSC_TIMER_CONSTRUCT(26),
       MSC_TIMER_CONSTRUCT(27),
       MSC_TIMER_CONSTRUCT(28),
       MSC_TIMER_CONSTRUCT(29),
       MSC_TIMER_CONSTRUCT(30),
       MSC_TIMER_CONSTRUCT(31),
       MSC_TIMER_CONSTRUCT(32),
       MSC_TIMER_CONSTRUCT(33),
       MSC_TIMER_CONSTRUCT(34),
       MSC_TIMER_CONSTRUCT(35),
       MSC_TIMER_CONSTRUCT(36),
       MSC_TIMER_CONSTRUCT(37),
       MSC_TIMER_CONSTRUCT(38),
       MSC_TIMER_CONSTRUCT(39),
       MSC_TIMER_CONSTRUCT(40),
       MSC_TIMER_CONSTRUCT(41),
       MSC_TIMER_CONSTRUCT(42),
       MSC_TIMER_CONSTRUCT(43),
       MSC_TIMER_CONSTRUCT(44),
       MSC_TIMER_CONSTRUCT(45),
       MSC_TIMER_CONSTRUCT(46),
       MSC_TIMER_CONSTRUCT(47),
       MSC_TIMER_CONSTRUCT(48),
       MSC_TIMER_CONSTRUCT(49),
       MSC_TIMER_CONSTRUCT(50),
       MSC_TIMER_CONSTRUCT(51),
       MSC_TIMER_CONSTRUCT(52),
       MSC_TIMER_CONSTRUCT(53),
       MSC_TIMER_CONSTRUCT(54),
       MSC_TIMER_CONSTRUCT(55),
       MSC_TIMER_CONSTRUCT(56),
       MSC_TIMER_CONSTRUCT(57),
       MSC_TIMER_CONSTRUCT(58),
       MSC_TIMER_CONSTRUCT(59),
       MSC_TIMER_CONSTRUCT(60),
       MSC_TIMER_CONSTRUCT(61),
       MSC_TIMER_CONSTRUCT(62),
       MSC_TIMER_CONSTRUCT(MSC_MAX_TIMERS - 1),
#undef MSC_TIMER_CONSTRUCT
};

/* the emu_table definitions */

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
#ifdef COMPILED_FOR_G20
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
    { "Lower33",  K12_EMU_LOWER },
    { "Lower34",  K12_EMU_LOWER },
    { "Lower35",  K12_EMU_LOWER },
    { "Lower36",  K12_EMU_LOWER },
    { "Lower37",  K12_EMU_LOWER },
    { "Lower38",  K12_EMU_LOWER },
#endif /* COMPILED_FOR_G20 */
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
#ifdef COMPILED_FOR_G20
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
    { "Lower33",  K12_EMU_LOWER },
    { "Lower34",  K12_EMU_LOWER },
    { "Lower35",  K12_EMU_LOWER },
    { "Lower36",  K12_EMU_LOWER },
    { "Lower37",  K12_EMU_LOWER },
    { "Lower38",  K12_EMU_LOWER },
#endif /* COMPILED_FOR_G20 */
    { "stdin",    K12_EMU_LOWER },
};

k12_emu_table_t p4_emu_table =
{
    DIM (p4_emu_inp_tab), p4_emu_inp_tab,
    DIM (p4_emu_out_tab), p4_emu_out_tab,
    DIM (p4_emu_confque_tab), p4_emu_confque_tab
};

/* ............................................................ */

status_t
emuInit (k12_emu_type_t* emul_id)
{
    register status_t e;
    p4_emu_t* pEmul;

# ifdef DEBUG
    K12LogInit(0, 64, K12_LOG_DIRECT);
# endif

    e=k12EmuLowMemGet (emul_id, sizeof (p4_emu_t), (s8_t**) &pEmul);
    if (e) return (K12_GEN_NOMEM);

    memset (pEmul, 0, sizeof (p4_emu_t));
    /* pEmul->private.state = K12_EMU_IDLE; */ /* -> term-k12.c */

    /* must be called *before* TableInd - right ?? */
    p4_SetOptions (&pEmul->session, 1, (char**) &"p4th");

    /* pass table of features - menusystem may write to those vars now ! */
    e=k12EmuLowTableInd (emul_id, &p4_emu_table, (s8_t *)pEmul);
    if (e) return (e);

    K12LogDebug1(1, "p4.emuInit",
      "privat data size=%08lX\n", (u32_t)sizeof(p4_emu_t));

    pEmul->thread.term = &p4_term_k12; /* the gui interface */

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
#   ifdef P4_REGTH
    {
        void* save_TH = p4TH;
        p4TH = &p4_emu->thread; 
#   endif
#       ifdef P4_REGRP
        { 
            void* save_RP = p4RP;
            p4RP = PFE.r0;
#       endif
#           ifdef P4_REGSP
            { 
                void* save_SP = p4SP;
                p4SP = PFE.s0;
#           endif
#               ifdef P4_REGFP
                { 
                    void* save_FP = p4FP;
                    p4FP = PFE.f0;
#               endif
                    {
                        if (FENCE) FX (p4_closeall_files);
                        PFE.atexit_cleanup ();
                    }
#               ifdef P4_REGFP
                    p4FP = save_FP;
                }
#               endif
#           ifdef P4_REGSP
                p4SP = save_SP;
            }
#           endif
#       ifdef P4_REGRP
            p4RP = save_RP;
        }
#       endif
#   ifdef P4_REGTH
        p4TH = save_TH;
    }
#   endif
    return K12_GEN_OK;
}

status_t
emuStart (k12_emu_type_t* emul_id)
{
    register status_t e;
    p4_emu_t* p4_emu;
    
    e=k12EmuLowBaseGet (emul_id, (void*)&p4_emu);
    if (e) return (e);

    if (*p4_emu->includes)
        p4_emu->session.include_file = p4_emu->includes;
    if (!*p4_emu->incpaths)
        strncpy (p4_emu->incpaths, PFE_PKGDATADIR, 254);
    p4_emu->session.incpaths = p4_emu->incpaths;

    p4_emu->session.cpus = 1;
    /*  p4_emu->session.cpu[0] = &p4_emu->thread; */
    p4_emu->thread.set = &p4_emu->session;
    p4_emu->thread.priv = &p4_emu->private;
    p4_emu->private.emu = emul_id;

#  ifdef K12_EMU_VALIDATION_REQ
    { 
        int i = 0;
        k12_emu_event_t ev; k12_emu_msg_subhead_t* msg; u32_t len;
        P4_debug (13, "K12_EMU_VALIDATION_REQ used - set timeout to 200*HZ")
        k12EmuLowTimerStart (emul_id, K12_EMU_VALIDATION_REQ, 
                             200*CLOCKS_PER_SEC);
        while (1) 
        { 
            k12EmuLowEventGet(emul_id, &ev, (char**) &msg, &len, 0);
            switch (msg->type)
            {
            case K12_EMU_MSG_TIMER:
                P4_warn1 ("EMU_MSG_TIMER type->0x%x : "
                          "assuming validation_req timeout", 
                          msg->type);
                taskDelay(CLOCKS_PER_SEC);
                /*fallthrough*/
            case K12_EMU_VALIDATION_REQ:
                k12EmuLowTimerStop(emul_id, K12_EMU_VALIDATION_REQ);
                return p4_Exec(&p4_emu->thread);
            default:
                P4_warn2 ("UNK START MSG type->0x%x len->%u", 
                          msg->type, len);
                /*fallthrough*/
            case K12_EMU_XDAT_REQ: 
                /* config-cb to command-msg via xdat-req */
                if (++i > 3)
                    P4_warn ("START DELAYED (and requeue message) (HZ/2)");
                taskDelay(CLOCKS_PER_SEC/2);
                k12EmuLowEventPut(emul_id, ev, (char*) msg, len, 
                                  K12_EMU_NOOPT);
                continue;
            }
        }
    } 
#  endif
}

/*@} */

/* 
 * Local variables:
 * c-file-style: "stroustrup"
 * End:
 */

