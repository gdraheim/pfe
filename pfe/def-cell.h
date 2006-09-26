#ifndef __PFE_DEF_CELL_H
#define __PFE_DEF_CELL_H

/* 
 * -- The basic types
 *
 *  Copyright (C) 2005 - 2006 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2006-09-26 22:56:10 $)
 *
 *  note that TYPEOF_CELL is either long or int.
 *  It must be atleast as big as a pointer.
 */
#include <pfe/def-config.h>
 
typedef unsigned char  p4char; /* hopefully an 8-bit type */
typedef unsigned short p4word; /* hopefully a 16-bit type */

typedef unsigned char const p4cchar;

typedef PFE_TYPEOF_CELL			p4cell;	 /* a stack item */
typedef unsigned PFE_TYPEOF_CELL	p4ucell; /* dito unsigned */

# if PFE_SIZEOF_CELL+0 == PFE_SIZEOF_LONG+0
typedef long p4celll;                  /* using the C type "long" saves us */
typedef unsigned long p4ucelll;        /* a couple of warnings on LP64 */
# else
typedef p4cell p4celll;       /* FIXME: default p4cell should be "long" */
typedef p4ucell p4celll;      /* instead of the traditional "int" default */
# endif

typedef struct
{ 
    p4cell hi; 
    p4ucell lo; 
} p4dcell;	/* dito, double precision signed */

typedef struct 
{ 
    p4ucell hi;
    p4ucell lo; 
} p4udcell;	/* dito, double precision unsigned */

typedef struct				/* "map" of a cell */
{
#if PFE_BYTEORDER == 4321
    unsigned PFE_TYPEOF_HALFCELL hi;
    unsigned PFE_TYPEOF_HALFCELL lo;
#else
    unsigned PFE_TYPEOF_HALFCELL lo;
    unsigned PFE_TYPEOF_HALFCELL hi;
#endif
} p4ucell_hi_lo;

typedef p4char p4_byte_t;          /* adressing element */
typedef p4char p4_char_t;          /* i/o char element */
typedef p4cell p4_cell_t;          /* computational element */
typedef p4cell p4_bool_t;          /* and as boolean flag */

#endif
