#ifndef __PFE_MAIN_K12_H
#define __PFE_MAIN_K12_H "%full_filespec: main-k12.h~5.3:incl:bln_12xx!2 %"
/** @name main-k12.h
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2000. All rights reserved.
 *
 *  @author Tektronix CTE               @(#) %derived_by: guidod %
 *  @version %version: 5.3 %
 *    (%date_modified: Mon Mar 12 10:33:39 2001 %)
 */
/*@{ */

#include <pfe/term-k12.h>

typedef struct                  /* FIXME: Later deletion planned! */
{
    int msc_ints[16];
    char msc_strings[81][16];
    int msc_timers[64];
} _p4_msc_var_buf_t;

typedef struct
{
    struct k12_priv private;
    struct p4_Session session;
    struct p4_Thread  thread;
    _p4_msc_var_buf_t msc_vars;
                                /* FIXME: Later deletion planned! */
    char   includes[255];
    char   incpaths[255];
    char   config_blocked;
} p4_emu_t;

extern k12_emu_table_t   p4_emu_table;

#define P4_K12_EMUL(P) ((p4_emu_t*)((P)->priv))

/*@}*/
#endif

