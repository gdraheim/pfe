/** 
 * -- Process command line, get memory and start up.
 * 
 *  Copyright (C) Tektronix, Inc. 1999 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 1.3 %
 *    (%date_modified: Mon Mar 12 10:32:28 2001 %)
 *
 *  @description
 *  Process command line, get memory and start up the interpret loop of PFE
 */
/*@{*/

#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: main-static.c,v 0.30 2001-03-12 09:32:28 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/main-sub.h>
#include <stdlib.h>
#include <errno.h>

static char memory[P4_KB*1024]; /* BSS */

int
main (int argc, char** argv)
{
    p4_Thread* thread;
    p4_Session session;
    int i;
  
    if ((i=p4_SetOptions (&session, argc, argv))) return i-1;

    thread = (p4_Thread*) memory;
    memset (thread, 0, sizeof(p4_Thread));
    
    p4_SetDictMem(thread, memory+sizeof(p4_Thread));
    thread->set = &session;

    return p4_Exec (thread); 
}

