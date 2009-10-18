/** 
 * -- Process command line, get memory and start up.
 * 
 *  Copyright (C) Tektronix, Inc. 1999 - 2001. 
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2008-04-20 04:46:30 $)
 *
 *  @description
 *  Process command line, get memory and start up the interpret loop of PFE
 */
/*@{*/

#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: main-stdc.c,v 1.3 2008-04-20 04:46:30 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/engine-set.h>
#include <pfe/option-set.h>
#include <stdlib.h>
#include <pfe/os-string.h>
#include <errno.h>

#ifdef PFE_HAVE_SYS_RESOURCE_H
#include <sys/time.h>
#include <sys/resource.h>
#endif

#ifdef STATIC_MAIN
#define main static_main
 static 
#endif
 int
main (int argc, const char** argv)
{
    int i;
    struct {
        p4_Thread  thread;
        p4_Session session;
        char options[500];
    } x;
    
    if ((i=p4_SetOptions (&x.session, sizeof(x.session)+sizeof(x.options), 
                          argc, argv))) 
	return i-1;

# ifndef PFE_WITH_MODULES
    {	extern p4Words P4WORDS(internal);
    if ((i=p4_SetModules (&x.session, &(P4WORDS(internal))))) return i-1;
    }
# endif

# ifdef PFE_HAVE_SYS_RESOURCE_H
    {/* a pfe does not need lots of data on the real cpu return stack */
	struct rlimit rlimits = { 64*1024, 64*1024 };
	setrlimit (RLIMIT_STACK, &rlimits);
    }
# endif

    return p4_FreeOptions (
        p4_Exec (p4_SetThreadOf( &x.thread , &x.session )), 
        &x.session); 
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

/* 
 * Local variables:
 * c-file-style: "stroustrup"
 * End:
 */


