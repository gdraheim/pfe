#ifndef __PFE_TERM_K12_H__
#define __PFE_TERM_K12_H__ "%full_filespec: term-k12.h~5.3:incl:bln_12xx!3 %"
/**
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2000. All rights reserved.
 *
 *  @(#)                            RCS: %filespec: term-k12.h~5.3 %
 *  @author Tektronix CTE                %derived_by: guidod %
 *  @version %version: 5.3 %
 *    (%date_modified: Mon Mar 12 10:33:44 2001 %)
 *
 *  @description
 *        The structure of the K12XX Terminal Driver is sometimes
 *        accessed directly from special modules to provide some
 *        of the specialties later.
 */
/*@{*/

#include <K12/emul.h>
#include <K12/logging.h>
/* #include "emuint.h" */

#include <rngLib.h>
#include <taskLib.h>
#include <semLib.h>

typedef struct k12_priv k12_priv;

#ifndef K12_TERM_FKNAME_MAX
#define K12_TERM_FKNAME_MAX 16
#endif
#ifndef K12_TERM_FKKEYS_MAX
#define K12_TERM_FKKEYS_MAX 12
#endif

#ifndef  K12_ANSWERBUF_SIZE
#define  K12_ANSWERBUF_SIZE 160
#endif

#ifndef K12_FORTH_COMMAND_SAP
#define K12_FORTH_COMMAND_SAP 15
#endif

struct k12_priv
{
    k12_emu_type_t* emu;
    u32_t state;
    s8_t* rx_data;
    u32_t rx_datalen;
    u32_t rx_dataIN;
    u32_t rx_dataSAP;

    k12_emu_event_t  frm_input;
    s8_t*            frm_data;
    u32_t            frm_datalen;
    k12_emu_option_t frm_option;
    unsigned char    frm_putback;
    
    unsigned char bufidx;
    char          buffer[256];
    int nxch;
    
    unsigned char answering;
    unsigned char answeridx;
    char          answerbuf[K12_ANSWERBUF_SIZE];
    
    int row, col; /* position of cursor as tracked */
    
#ifdef QID
    QID tx_qid;
#else  
    u32_t tx_qid; /* the queue id */
#endif
    
    RING_ID tx_ring; /* output ring id */
    int     tx_task; /* writer task for the ring */
    SEM_ID  tx_sem;  /* sync p4_task and tx_task */
    int     tx_quit; /* flag for tx_task to quit */
    
    char f[K12_TERM_FKNAME_MAX][K12_TERM_FKKEYS_MAX]; 
    
    /* Pointer for test manager demux function */
    int (*eventHook) (k12_emu_event_t mEvent,  s8_t* pData,  u32_t mLength);
};

#define P4_K12_PRIV(P) ((struct k12_priv*)(P->priv))

extern struct _p4_term_struct p4_term_k12;

#endif
/*@}*/

