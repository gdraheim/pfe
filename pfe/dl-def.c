/** 
 * -- Words to open a shared code object
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE                %derived_by: guidod %
 *  @version %version: 1.17 %
 *    (%date_modified: Tue Jul 31 13:27:20 2001 %)
 *
 *  @description
 *		This file exports a set of system words for 
 *              any OS that can dynamically bind object code to
 *		the interpreter. This part will then try to look
 *              up a symbol that can return a loadlist-table to
 *              be fed to the loadlist-loader routine.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: dl-def.c,v 1.1.1.1 2006-08-08 09:09:37 guidod Exp $";
#endif

#define _P4_SOURCE 1

#ifndef _export
#include <pfe/def-config.h>
#endif

#ifndef PFE_DL_DEF
# ifdef PFE_HAVE_WINCON_H
# define PFE_DL_DEF 3
# endif
#endif

#ifndef PFE_DL_DEF
# if defined PFE_HAVE_LTDL_H || defined PFE_WITH_LTDL \
  || defined PFE_HAVE_DLFCN_H
# define PFE_DL_DEF 4
# endif
#endif

#ifndef PFE_DL_DEF
# if defined HOST_OS_HPUX || defined __target_os_hpux
# define PFE_DL_DEF 9
# endif
#endif

#ifndef PFE_DL_DEF
# if defined HOST_OS_VXWORKS || defined VxWorks || defined __target_os_vxworks
# define PFE_DL_DEF 7
# endif
#endif

#ifndef PFE_DL_DEF
# if defined PFE_HAVE_WINBASE_H
# define PFE_DL_DEF 3
# endif
#endif

#if PFE_DL_DEF == 3
#include "dl-win32.c"
const char p4_dl_def[] = "dl-win32";
#elif PFE_DL_DEF == 4
#include "dl-dlfcn.c"
const char p4_dl_def[] = "dl-dlfcn";
#elif PFE_DL_DEF == 7
#include "dl-vxworks.c"
const char p4_dl_def[] = "dl-vxworks";
#elif PFE_DL_DEF == 9
#include "dl-hpux.c"
const char p4_dl_def[] = "dl-hpux";
#else 
#include "dl-none.c"
const char p4_dl_def[] = "dl-none";
#endif 
/* : HAVE_P4_DLFCN layer */

/*@}*/

