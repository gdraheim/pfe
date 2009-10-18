#ifndef __PFE_OS_GETTIMEOFDAY_H
#define __PFE_OS_GETTIMEOFDAY_H
/*
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2008-04-20 04:46:30 $)
 */

#include <pfe/def-cell.h>

#ifdef __cplusplus
extern "C" {
#endif

_extern  void p4_gettimeofday (p4ucell* sec, p4ucell* usec) ;

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
