/** 
 * -- Process command line, get memory and start up.
 * 
 *  Copyright (C) Tektronix, Inc. 1999 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 5.1 %
 *    (%date_modified: Wed Aug 23 11:57:05 2000 %)
 *
 *  @description
 *  Process command line, get memory and start up the interpret loop of PFE
 */
/*@{*/

#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: main-mmap.c,v 0.30 2001-03-12 09:30:13 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/main-sub.h>
#include <stdlib.h>
#include <errno.h>

static char memory[P4_KB*1024];

#ifdef STATIC_MAIN
#define main static_main
 static 
#endif
 int
main (int argc, char** argv)
{
    p4_Thread* thread;
    p4_Session session;
    int i;
  
    if ((i=p4_SetOptions (&session, argc, argv))) return i-1;

    thread = malloc (sizeof(p4_Thread);
    if (!thread) return -ENOMEM;
    memset (thread, 0, sizeof(p4_Thread));
    
    p4_SetDictMem(thread, memory);
  
    return p4_Exec (thread); 
}



#if 0

    PFE_MEM = 0;
#  ifdef USE_MMAP
    if (PFE_set.mapfile)
    {
        PFE.mapfile_fd = open (PFE_set.mapfile, O_RDWR|O_CREAT|O_TRUNC, 0660);
        if (PFE.mapfile_fd == -1) 
        {
            P4_info3("[%p] %s: could not open: %s", 
              p4TH, PFE_set.mapfile, strerror(errno));
            PFE.mapfile_fd = 0;
        }else{
            lseek (PFE.mapfile_fd, PFE_set.total_size-3, SEEK_SET); 
            write (PFE.mapfile_fd, "END", 3); /* mostly a sparse file */
#           ifndef MAP_FAILED
#           define MAP_FAILED      ((void *) -1)
#           endif
          
            PFE_MEM = MAP_FAILED;
            if (PFE_set.mapbase)
            {
                PFE_MEM = mmap(PFE_set.mapbase, PFE_set.total_size,
                  PROT_READ|PROT_WRITE, MAP_SHARED, PFE.mapfile_fd, 0);
                if (PFE_MEM == MAP_FAILED) 
                    P4_fail4 ("[%p] %s: could not mmap to mapbase %08p : %s",
                      p4TH, PFE_set.mapfile, PFE_set.mapbase, strerror(errno));
            }
            if (PFE_MEM == MAP_FAILED)
            {
                PFE_MEM = mmap(0, PFE_set.total_size,
                  PROT_READ|PROT_WRITE, MAP_SHARED, PFE.mapfile_fd, 0);
                if (PFE_MEM == MAP_FAILED)
                    P4_info3("[%p] %s: could not mmap: %s", 
                      p4TH, PFE_set.mapfile, strerror(errno));              
            }
            if (PFE_MEM == MAP_FAILED)
            {
                close(PFE.mapfile_fd); 
                PFE.mapfile_fd = 0;
                PFE_MEM = 0;
            }else{
                P4_info3 ("[%p] mapped at %8p len %d",
                  p4TH, PFE_MEM, PFE_set.total_size);
            }
        }
    }
#  endif /*USE_MMAP*/



#ifdef USE_MMAP
    if (PFE.mapfile_fd)
    {
        munmap (PFE_MEM, PFE_set.total_size); PFE_MEM = 0;
        close (PFE.mapfile_fd);  PFE.mapfile_fd = 0;
        P4_info1 ("[%p] unmapped basemem", p4TH);      
    }
#endif
      
#endif

