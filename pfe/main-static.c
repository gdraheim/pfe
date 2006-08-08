/** 
 * -- Process command line, get memory and start up.
 * 
 *  Copyright (C) Tektronix, Inc. 1999 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: bln_mpt1!1.6 %
 *    (%date_modified: Mon Mar 25 16:46:50 2002 %)
 *
 *  @description
 *  Process command line, get memory and start up the interpret loop of PFE
 */
/*@{*/

#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: main-static.c,v 1.1.1.1 2006-08-08 09:09:56 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/option-set.h>
#include <pfe/engine-set.h>
#include <stdlib.h>
#include <errno.h>

static char memory[P4_KB*1024]; /* BSS */

int
main (int argc, char** argv)
{
    p4_Thread* thread;
    p4_Session session;
    int i;
  
    if ((i=p4_SetOptions (&session, 0, argc, argv))) return i-1;
# ifndef PFE_WITH_MODULES
    {	extern p4Words P4WORDS(internal);
    if ((i=p4_SetModules (&session, &(P4WORDS(internal))))) return i-1;
    }
# endif

    thread = (p4_Thread*) memory;
    p4_memset (thread, 0, sizeof(p4_Thread));

    /* how to override the size of the dict if the user did use an option? */
    p4_SetDictMem(thread, memory+sizeof(p4_Thread));
    thread->set = &session;

    return p4_Exec (thread); 
}

