#ifndef __PFE_DEF_XTRA_H
#define __PFE_DEF_XTRA_H "%full_filespec: def-xtra.h~1.4:incl:bln_12xx!1 %"

/** 
 * -- declare extras for specific environments
 * 
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE             @(#) %derived_by: guidod %
 *  @version %version: 1.4 %
 *    (%date_modified: Mon Mar 12 10:33:28 2001 %)
 *  @description
 *            
 */
/*@{*/

#include <pfe/incl-sub.h>

#ifndef FILE
#include <stdio.h>
#endif

#ifdef _P4_SOURCE
# if defined VxWorks || defined SYS_vxworks || defined __target_os_vxworks
# ifndef HAVE_STRDUP
  extern char *strdup (const char *s);
# endif
# ifndef HAVE_ACCESS
  extern int access (const char *, int);
# endif
  /* extern int chdir (const char *); */
  extern int close (int);
  /* extern int rmdir (const char *); */

# ifndef HAVE_FCNTL
  extern int fcntl (int, int, ...);
# endif

# include <fcntl.h>
# include <timers.h>
# endif /* VxWorks */
#endif

/*@}*/
#endif
