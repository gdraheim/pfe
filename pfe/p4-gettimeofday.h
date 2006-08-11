#ifndef __PFE_OS_GETTIMEOFDAY_H
#define __PFE_OS_GETTIMEOFDAY_H
/*
 *  Copyright (C) 2005 - 2006 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.2 $
 *     (modified $Date: 2006-08-11 22:56:05 $)
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
