/** 
 * -- Process command line, get memory and start up.
 * 
 *  Copyright (C) Tektronix, Inc. 1999 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 5.6 %
 *    (%date_modified: Mon Mar 12 10:32:29 2001 %)
 *
 *  @description
 *  Process command line, get memory and start up the interpret loop of PFE
 */
/*@{*/

#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: main-stdc.c,v 0.30.86.1 2001-03-12 09:32:29 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/main-sub.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef STATIC_MAIN
#define main static_main
 static 
#endif
 int
main (int argc, char** argv)
{
    p4_Thread  thread;
    p4_Session session;
    int i;
  
    if ((i=p4_SetOptions (&session, argc, argv))) 
	return i-1;

    memset (&thread, 0, sizeof(thread));
    thread.set = &session;

    return p4_FreeOptions (p4_Exec (&thread), &session); 
}

#ifdef STATIC_MAIN
void
p4 (int arg1, int arg2, int arg3, int arg4, int arg5,
     int arg6, int arg7, int arg8, int arg9, int arg10)
{
    int argc = 0;
    char *argv[12];
    
    argv [0] = "p4th";
    argv [1] = (char *) arg1;
    argv [2] = (char *) arg2;
    argv [3] = (char *) arg3;
    argv [4] = (char *) arg4;
    argv [5] = (char *) arg5;
    argv [6] = (char *) arg6;
    argv [7] = (char *) arg7;
    argv [8] = (char *) arg8;
    argv [9] = (char *) arg9;
    argv[10] = (char *) arg10;
    argv[11] = NULL;
    while (argv[argc]!=NULL)	/* count args */
    {
        argc++;
    }
    static_main (argc, argv);
}
#endif  /* STATIC_MAIN */
/*@}*/

