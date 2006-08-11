#ifndef _PFE_POSIX_EXT_H
#define _PFE_POSIX_EXT_H 1155333836
/* generated 2006-0812-0003 ../../pfe/../mk/Make-H.pl ../../pfe/posix-ext.c */

#include <pfe/pfe-ext.h>

/** 
 * -- Words making sense in POSIX-like systems only.
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2006 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.2 $
 *     (modified $Date: 2006-08-11 22:56:05 $)
 *
 *  @description
 *              This file exports a set of system words for 
 *              a posixish OS environment. So should do
 *              any alternative wordset you might create for your OS.
 *
 *              NOTE: this wordset is going to be removed soon.
 */

#ifdef __cplusplus
extern "C" {
#endif




/** NTOHS ( w -- w' )
 * if current host-encoding is bigendian, this is a NOOP
 * otherwise byteswap the lower 16-bit bits of the topofstack.
 * see =>'W@' and =>'W!'
 * (on some platforms, the upper bits are erased, on others not)
 */
extern P4_CODE (p4_ntohs);

/** NTOHL ( l -- l' )
 * if current host-encoding is bigendian, this is a NOOP
 * otherwise byteswap the lower 32-bit bits of the topofstack.
 * see =>'L@' and =>'L!' (being usually just =>'@' and =>'!' )
 * (on some platforms, the upper bits are erased, on others not)
 */
extern P4_CODE (p4_ntohl);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
