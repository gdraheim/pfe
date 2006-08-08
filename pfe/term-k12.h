#ifndef __PFE_TERM_K12_H__
#define __PFE_TERM_K12_H__ "%full_filespec: term-k12.h~bln_mpt1!5.12:incl:bln_12xx!3 %"
/**
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2000. All rights reserved.
 *
 *  @(#)                            RCS: %filespec: term-k12.h~bln_mpt1!5.12 %
 *  @author Tektronix CTE                %derived_by: guidod %
 *  @version %version: bln_mpt1!5.12 %
 *    (%date_modified: Tue Mar 18 15:23:08 2003 %)
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

#ifdef __vxworks
#include <rngLib.h>
#include <taskLib.h>
#include <semLib.h>
#endif

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
#define K12_FORTH_COMMAND_SAP 63
#endif

typedef char p4_fkey_caption_t[K12_TERM_FKNAME_MAX];

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
    unsigned char buffer[256];
    int nxch;
    
    unsigned char answering;
    unsigned char answeridx;
    unsigned char answerbuf[K12_ANSWERBUF_SIZE];
    
    int row, col; /* position of cursor as tracked */
    
#ifdef QID
    QID tx_qid;
#else  
    u32_t tx_qid; /* the queue id */
#endif
    
#ifdef __vxworks
    RING_ID tx_ring; /* output ring id */
    int     tx_task; /* writer task for the ring */
    SEM_ID  tx_sem;  /* sync p4_task and tx_task */
    int     tx_quit; /* flag for tx_task to quit */
#endif
    
    p4_fkey_caption_t f[K12_TERM_FKKEYS_MAX]; 
    
    /* Pointer for test manager demux function */
    int (*eventHook) (k12_emu_event_t mEvent,  s8_t* pData,  u32_t mLength);

    /* Terminal output logfile for (embedded) automatic regression testing */
    int     tx_logfile;

    /* wait for validation_req to set emulation state to LOADED */
    int     validation_req; 

    /* show state-description from answerbuf */
    int     answerwarning;

    /* Terminal output to service access point */
    k12_emu_event_t tx_link;
    /* Terminal extra input - compare with rx_dataSAP */
    k12_emu_event_t rx_link;
    /* Terminal answer to service access point - line mode */
    k12_emu_event_t qx_link;
    
    /* used for binary compatibility - use it! */
    int     padding[1];
};

#define P4_K12_PRIV(P) ((struct k12_priv*)(P->priv))

extern struct _p4_term_struct p4_term_k12;

#endif
/*@}*/

