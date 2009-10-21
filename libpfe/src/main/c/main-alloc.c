/**
 * -- Process command line, get memory and start up.
 *
 *  Copyright (C) Tektronix, Inc. 1999 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2008-04-20 04:46:29 $)
 *
 *  @description
 *  Process command line, get memory and start up the interpret loop of PFE
 */
/*@{*/

#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) =
"@(#) $Id: main-alloc.c,v 1.3 2008-04-20 04:46:29 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/option-set.h>
#include <pfe/engine-set.h>
#include <stdlib.h>
#include <errno.h>

#ifdef PFE_WITH_STATIC_DICT
static char memory[P4_KB*1024]; /* BSS */
#endif

int
main (int argc, const char** argv)
{
    p4_Session* session;
    p4_Thread*  thread;
    int i;

    session = p4_NewSessionOptions (500);
    if (!session) return 1;
    if ((i=p4_AddOptions (session, argc, argv))) return i-1;

# ifndef PFE_WITH_MODULES
    {	extern p4Words P4WORDS(internal);
    if ((i=p4_SetModules (session, &(P4WORDS(internal))))) return i-1;
    }
# endif

#  ifdef PFE_WITH_STATIC_DICT
    session->total_size = P4_KB*1024;
    p4_SetDictMem(thread, memory);
#  endif

    thread = p4_NewThreadOptions(session);
    i = p4_Exec (thread);
    p4_FreeThreadPtr (thread);
    p4_FreeOptions (i, session);
    p4_FreeSessionPtr (session);
    return i;
}
