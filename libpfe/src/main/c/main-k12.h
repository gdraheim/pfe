#ifndef __PFE_MAIN_K12_H
#define __PFE_MAIN_K12_H
/** @name main-k12.h
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2000. 
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE            (modified by $Author: guidod $)
 *  @version $Revision: 1.2 $
 *     (modified $Date: 2006-08-11 22:56:04 $)
 */
/*@{ */

#include <pfe/term-k12.h>

typedef char _p4_msc_string_buffer_t[256];

typedef struct                  /* FIXME: Later deletion planned! */
{
    int msc_ints[16];
    _p4_msc_string_buffer_t msc_strings[16];
    int msc_timers[64];
} _p4_msc_var_buf_t;

typedef struct
{
    struct k12_priv private;
    union { 
        p4_Session old;
        struct {
            char blocked; /* #define config_blocked has.config.blocked */
            char const** scriptfile;
            char const** scriptpath; /* session.inc_paths */
        } config;
    } has; 
    struct p4_Thread  thread;
    _p4_msc_var_buf_t msc_vars;
    struct p4_Session session;
    char   extra[1024];
} p4_emu_t;

extern k12_emu_table_t   p4_emu_table;

#define P4_K12_EMUL(P) ((p4_emu_t*)((P)->priv))

extern char* k12_pfe_sap_table[];

/*@}*/
#endif
