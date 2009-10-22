#ifndef __PFE_DEF_RESTORE_H
#define __PFE_DEF_RESTORE_H
/**
 * -- Declares save and restore macros for the virtual machine.
 *
 *  Copyright (C) Tektronix, Inc. 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author$)
 *  @version $Revision$
 *     (modified $Date$)
 *
 *  @description
 *         Declares macros to save and restore the cpu registers used
 *         by the Forth Virtual Machine.
 *         Using GCC these can be put into CPU registers that are
 *         valid across function calls.
 *         However when run as a callback from somewhere else or when
 *         being entered through an outside-API call, then these must
 *         be saved, setup and restored explicitly since the gcc in its
 *         later version will not do it on its own and neither would any
 *         of the standard-C and standard-OS callback services (like those
 *         timers and signals).
 */
/*@{*/

#include <pfe/def-regs.h>

#ifndef P4_REGS_SIGBUS_ADDR /* 0xFFFFDEAD */
#define P4_REGS_SIGBUS_ADDR ((void*)(-8531))
#endif

#ifdef P4_REGTH
#define P4_REGTH_MKSAVED  void* pfe__p4TH__saved = p4TH;
#define P4_REGTH_MKTHROW p4TH = P4_REGS_SIGBUS_ADDR;
#define P4_REGTH_RESTORE p4TH = pfe__p4TH__saved;
#else
#define P4_REGTH_MKSAVED
#define P4_REGTH_MKTHROW
#define P4_REGTH_RESTORE
#endif

#ifdef P4_REGIP
#define P4_REGIP_MKSAVED  void* pfe__p4IP__saved = p4IP;
#define P4_REGIP_MKTHROW p4IP = P4_REGS_SIGBUS_ADDR;
#define P4_REGIP_RESTORE p4IP = pfe__p4IP__saved;
#else
#define P4_REGIP_MKSAVED
#define P4_REGIP_MKTHROW
#define P4_REGIP_RESTORE
#endif

#ifdef P4_REGWP
#define P4_REGWP_MKSAVED  void* pfe__p4WP__saved = p4WP;
#define P4_REGWP_MKTHROW p4WP = P4_REGS_SIGBUS_ADDR;
#define P4_REGWP_RESTORE p4WP = pfe__p4WP__saved;
#else
#define P4_REGWP_MKSAVED
#define P4_REGWP_MKTHROW
#define P4_REGWP_RESTORE
#endif

#ifdef P4_REGSP
#define P4_REGSP_MKSAVED  void* pfe__p4SP__saved = p4SP;
#define P4_REGSP_MKTHROW p4SP = P4_REGS_SIGBUS_ADDR;
#define P4_REGSP_RESTORE p4SP = pfe__p4SP__saved;
#else
#define P4_REGSP_MKSAVED
#define P4_REGSP_MKTHROW
#define P4_REGSP_RESTORE
#endif

#ifdef P4_REGRP
#define P4_REGRP_MKSAVED  void* pfe__p4RP__saved = p4RP;
#define P4_REGRP_MKTHROW p4RP = P4_REGS_SIGBUS_ADDR;
#define P4_REGRP_RESTORE p4RP = pfe__p4RP__saved;
#else
#define P4_REGRP_MKSAVED
#define P4_REGRP_MKTHROW
#define P4_REGRP_RESTORE
#endif

#ifdef P4_REGLP
#define P4_REGLP_MKSAVED  void* pfe__p4LP__saved = p4LP;
#define P4_REGLP_MKTHROW p4LP = P4_REGS_SIGBUS_ADDR;
#define P4_REGLP_RESTORE p4LP = pfe__p4LP__saved;
#else
#define P4_REGLP_MKSAVED
#define P4_REGLP_MKTHROW
#define P4_REGLP_RESTORE
#endif

#ifdef P4_REGFP
#define P4_REGFP_MKSAVED  void* pfe__p4FP__saved = p4FP;
#define P4_REGFP_MKTHROW p4FP = P4_REGS_SIGBUS_ADDR;
#define P4_REGFP_RESTORE p4FP = pfe__p4FP__saved;
#else
#define P4_REGFP_MKSAVED
#define P4_REGFP_MKTHROW
#define P4_REGFP_RESTORE
#endif

#define P4_CALLER_MKSAVED \
        P4_REGTH_MKSAVED \
        P4_REGIP_MKSAVED \
        P4_REGWP_MKSAVED \
        P4_REGSP_MKSAVED \
        P4_REGRP_MKSAVED \
        P4_REGLP_MKSAVED \
        P4_REGFP_MKSAVED

#define P4_CALLER_MKTHROW \
        P4_REGTH_MKTHROW \
        P4_REGIP_MKTHROW \
        P4_REGWP_MKTHROW \
        P4_REGSP_MKTHROW \
        P4_REGRP_MKTHROW \
        P4_REGLP_MKTHROW \
        P4_REGFP_MKTHROW

#define P4_CALLER_RESTORE \
        P4_REGTH_RESTORE \
        P4_REGIP_RESTORE \
        P4_REGWP_RESTORE \
        P4_REGSP_RESTORE \
        P4_REGRP_RESTORE \
        P4_REGLP_RESTORE \
        P4_REGFP_RESTORE

#define P4_CALLER_SAVEALL P4_CALLER_MKSAVED P4_CALLER_MKTHROW

/* ............. and the same for the global p4_Thread ............. */
/*              note: we use a reference, not a pointer              */

#ifdef P4_REGTH
#define P4_REGTH_LOAD(th) p4TH = (th);
#else
#define P4_REGTH_LOAD(th)
#endif

#ifdef P4_REGIP
#define P4_REGIP_SAVE(th) (th)->ip = p4IP;
#define P4_REGIP_LOAD(th) p4IP = (th)->ip;
#else
#define P4_REGIP_SAVE(th)
#define P4_REGIP_LOAD(th)
#endif

#ifdef P4_REGWP
#define P4_REGWP_SAVE(th) (th)->wp = p4WP;
#define P4_REGWP_LOAD(th) p4WP = (th)->wp;
#else
#define P4_REGWP_SAVE(th)
#define P4_REGWP_LOAD(th)
#endif

#ifdef P4_REGSP
#define P4_REGSP_SAVE(th) (th)->sp = p4SP;
#define P4_REGSP_LOAD(th) p4SP = (th)->sp;
#else
#define P4_REGSP_SAVE(th)
#define P4_REGSP_LOAD(th)
#endif

#ifdef P4_REGRP
#define P4_REGRP_SAVE(th) (th)->rp = p4RP;
#define P4_REGRP_LOAD(th) p4RP = (th)->rp;
#else
#define P4_REGRP_SAVE(th)
#define P4_REGRP_LOAD(th)
#endif

#ifdef P4_REGLP
#define P4_REGLP_SAVE(th) (th)->lp = p4LP;
#define P4_REGLP_LOAD(th) p4LP = (th)->lp;
#else
#define P4_REGLP_SAVE(th)
#define P4_REGLP_LOAD(th)
#endif

#ifdef P4_REGFP
#define P4_REGFP_SAVE(th) (th)->fp = p4FP;
#define P4_REGFP_LOAD(th) p4FP = (th)->fp;
#else
#define P4_REGFP_SAVE(th)
#define P4_REGFP_LOAD(th)
#endif

#define PFE_VM_SAVE(th) \
        \
        P4_REGIP_SAVE(th) \
        P4_REGWP_SAVE(th) \
        P4_REGSP_SAVE(th) \
        P4_REGRP_SAVE(th) \
        P4_REGLP_SAVE(th) \
        P4_REGFP_SAVE(th)

#define PFE_VM_MKTHROW \
        P4_REGTH_MKTHROW \
        P4_REGIP_MKTHROW \
        P4_REGWP_MKTHROW \
        P4_REGSP_MKTHROW \
        P4_REGRP_MKTHROW \
        P4_REGLP_MKTHROW \
        P4_REGFP_MKTHROW

#define PFE_VM_LOAD(th) \
        P4_REGTH_LOAD(th) \
        P4_REGIP_LOAD(th) \
        P4_REGWP_LOAD(th) \
        P4_REGSP_LOAD(th) \
        P4_REGRP_LOAD(th) \
        P4_REGLP_LOAD(th) \
        P4_REGFP_LOAD(th)

#define PFE_VM_SAFE(th) PFE_VM_SAVE(th) PFE_VM_MKTHROW

/* ................ and FINALLY the combined words ................ */

#define PFE_VM_ENTER(th)  P4_CALLER_MKSAVED PFE_VM_LOAD(th)
#define PFE_VM_LEAVE(th)  PFE_VM_SAVE(th) P4_CALLER_RESTORE

/*@}*/
#endif
