#include <pfe/def-config.h>

/*
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2008-04-20 04:46:31 $)
 */

#ifdef _REENTRANT
# if !defined PFE_USE_WIN32   && !defined PFE_USE_PTH \
  && !defined PFE_USE_PTHREAD && !defined PFE_USE_SDL \
  && !defined PFE_USE_VXWORKS
  /* detect thread/mutex usage */
#  if defined HOST_WIN32
#  define PFE_USE_WIN32
#  elif defined PFE_HAVE_VXWORKS_H
#  define PFE_USE_VXWORKS
#  elif defined PFE_HAVE_PTHREAD_H
 /* POSIX2 means 1003.1b (realtime) or 1003.1c (pthreads) */
#  define PFE_USE_POSIX2 
#  elif defined PFE_HAVE_PTH_H
#  define PFE_USE_PTH
#  else
#  define PFE_NO_THREADS
#  endif
# endif
#else
# define PFE_NO_THREADS
#endif

#ifndef PFE_NO_THREADS
# if defined PFE_USE_WIN32
# define PFE_SEM_WIN32 
# define PFE_SEM_TYPE HANDLE
# define PFE_SEM_CREATE(VAR)  ((VAR) = CreateMutex (0,0,0))
# define PFE_SEM_TAKE(VAR)      (WaitForSingleObject ((VAR),INFINITE))
# define PFE_SEM_GIVE(VAR)      (ReleaseMutex ((VAR)))
# define PFE_SEM_DESTROY(VAR) (CloseHandle((VAR)))
# define PFE_COND_TYPE HANDLE
# define PFE_COND_CREATE(VAR)  ((VAR) = CreateSemaphore (0,0,10000,0))
# define PFE_COND_WAIT(VAR)      (WaitForSingleObject((VAR),INFINITE))
# define PFE_COND_POST(VAR)      (ReleaseSemaphore ((VAR), 1, 0))
# define PFE_COND_DESTROY(VAR) (CloseHandle ((VAR)))
# define PFE_THR_WIN32
# define PFE_THR_TYPE HANDLE
# define PFE_THR_ARG LPVOID
# define PFE_THR_SPAWN(VAR,FUNC,ARG,STACK)  do { DWORD x; \
             ((VAR) = CreateThread \
             (0,(DWORD) (STACK),(LPTHREAD_START_ROUTINE) (FUNC),(ARG),0,&x)); \
             } while (0)
# define PFE_THR_EXIT(EXITCODE) (ExitThread((EXITCODE)))
# define PFE_THR_KILL(VAR, EXITCODE) (TerminateThread((VAR), (EXITCODE)))
# define PFE_THR_SELF()  (GetCurrentThread ())  /* Pseudo Thread Id (NULL) */
# define PFE_THR_YIELD(VAR)  (Yield ())
/* -- */
# elif defined PFE_USE_VXWORKS
# define PFE_SEM_VXWORKS
# include <semLib.h>
# define PFE_SEM_TYPE SEM_ID
# define PFE_SEM_CREATE(VAR)  ((VAR) = semMCreate (0))
# define PFE_SEM_TAKE(VAR)      (semTake ((VAR)))
# define PFE_SEM_GIVE(VAR)      (semGive ((VAR)))
# define PFE_SEM_DESTROY(VAR) (semDelete ((VAR)))
# define PFE_THR_VXWORKS
# include <taskLib.h>
# define PFE_THR_TYPE int
# define PFE_THR_ARG void*
# define PFE_THR_SPAWN(VAR,FUNC,ARG,STACK)  ((VAR) = taskSpawn \
                                                   (0,0,0, \
                                                   (STACK),(FUNC),(ARG), \
                                                   0, 0, 0, 0, 0, 0, 0, 0, 0))
# define PFE_THR_EXIT(EXITCODE) (exit((EXITCODE)))
# define PFE_THR_KILL(VAR, EXITCODE) (taskDelete((VAR)))
# define PFE_THR_SELF()  (taskIdSelf ())
# define PFE_THR_YIELD(VAR)  (taskDelay (0))
/* -- */
# elif defined PFE_USE_POSIX2
# define PFE_SEM_POSIX2
# include <pthread.h>
# define PFE_SEM_TYPE pthread_mutex_t*
# define PFE_SEM_CREATE(VAR)  (pthread_mutex_init(\
       (VAR)=malloc(sizeof(pthread_mutex_t)), 0))
# define PFE_SEM_TAKE(VAR)    (pthread_mutex_lock ((VAR)))
# define PFE_SEM_GIVE(VAR)    (pthread_mutex_unlock ((VAR)))
# define PFE_SEM_DESTROY(VAR) do {pthread_mutex_destroy ((VAR)); \
                                  free ((VAR)); } while (0)
# define PFE_COND_TYPE pthread_cond_t*
# define PFE_COND_CREATE(VAR)  (pthread_cond_init(\
       (VAR)=malloc(sizeof(pthread_cond_t)), 0))
# define PFE_COND_POST(VAR)    (pthread_cond_signal ((VAR)))
# define PFE_COND_WAIT(VAR)    (pthread_cond_wait ((VAR), 0))
# define PFE_COND_DESTROY(VAR) do {pthread_cond_destroy ((VAR)); \
                                  free ((VAR)); } while (0)
# define PFE_THR_POSIX2
# define PFE_THR_TYPE pthread_t
# define PFE_THR_ARG void*
# define PFE_THR_SPAWN(VAR,FUNC,ARG,STACK)  (pthread_create (&(VAR),\
                                                  0,(FUNC),(ARG)))
# define PFE_THR_EXIT(EXITCODE) (pthread_exit((void*)(EXITCODE)))
# define PFE_THR_KILL(VAR, EXITCODE)  (pthread_cancel((VAR)))
# define PFE_THR_SELF()  (pthread_self ())
# define PFE_THR_YIELD(VAR)  (sched_yield ())
/* -- */
# elif defined PFE_USE_PTH
# define PFE_SEM_PTH
# include <pth.h>
# define PFE_SEM_TYPE pth_mutex_t*
# define PFE_SEM_CREATE(VAR) (pth_mutex_init (\
      (VAR)=malloc(sizeof(pth_mutex_t))))
# define PFE_SEM_TAKE(VAR)   (pth_mutex_acquire (&(VAR)))
# define PFE_SEM_GIVE(VAR)   (pth_mutex_release (&(VAR)))
# define PFE_SEM_DESTROY(VAR) (free ((VAR)))
# define PFE_COND_TYPE pth_cond_t*
# define PFE_COND_CREATE(VAR) (pth_cond_init(\
      (VAR)=malloc(sizeof(p4th_cond_t))))
# define PFE_COND_POST(VAR)     (pth_cond_notify ((VAR),0))
# define PFE_COND_WAIT(VAR)     (pth_cond_await((VAR),0,0))
# define PFE_COND_DESTROY(VAR) (free ((VAR)))
# define PFE_THR_PTH
# define PFE_THR_TYPE pth_t
# define PFE_THR_ARG void*
# define PFE_THR_SPAWN(VAR,FUNC,ARG,STACK)  ((VAR) = pth_spawn \
                                                  (0,(FUNC),(ARG)))
# define PFE_THR_EXIT(EXITCODE) (pth_exit((void*)(EXITCODE)))
# define PFE_THR_KILL(VAR, EXITCODE) (pth_abort((VAR)))
# define PFE_THR_SELF()  (pth_self ())
# define PFE_THR_YIELD(VAR)  (pth_yield ((VAR)))
/* -- */
# elif defined PFE_USE_SDL
# define PFE_SEM_SDL
# include <SDL.h>
# define PFE_SEM_TYPE SDL_mutex*
# define PFE_SEM_CREATE(VAR)  ((VAR) = SDL_CreateMutex ())
# define PFE_SEM_TAKE(VAR)    (SDL_LockMutex ((VAR)))
# define PFE_SEM_GIVE(VAR)    (SDL_UnlockMutex ((VAR)))
# define PFE_SEM_DESTROY(VAR) (SDL_DestroyMutex((VAR)))
# define PFE_COND_TYPE SDL_sem*
# define PFE_COND_CREATE(VAR) ((VAR) = SDL_CreateSemaphore(0))
# define PFE_COND_POST(VAR)     (SDL_SemPost((VAR)))
# define PFE_COND_WAIT(VAR)     (SDL_SemWait((VAR)))
# define PFE_COND_DESTROY(VAR) (SDL_DestroySemaphore((VAR)))
# define PFE_THR_PTHREAD
# define PFE_THR_TYPE SDL_Thread*
# define PFE_THR_ARG void*
# define PFE_THR_SPAWN(VAR,FUNC,ARG,STACK)  ((VAR) = SDL_CreateThread\
                                                  ((FUNC),(ARG)))
# define PFE_THR_EXIT(EXITCODE) (exit(EXITCODE))
# define PFE_THR_KILL(VAR, EXITCODE)  (SDL_KillThread((VAR)))
# define PFE_THR_SELF()  (SDL_ThreadID ())
# define PFE_THR_YIELD(VAR)  (SDL_Delay(0))
/* -- */
# else
#  ifdef __GNUC__
#  warning PFE_NO_THREADS ??
#  else
#  error PFE_NO_THREADS ??
#  endif
#  define PFE_NO_THREADS
# endif
#endif

#ifdef PFE_NO_THREADS
# define PFE_SEM_NONE
# define PFE_SEM_TYPE int
# define PFE_SEM_CREATE(VAR) ((VAR) = 0)
# define PFE_SEM_TAKE(VAR) ((VAR)+=1)
# define PFE_SEM_GIVE(VAR) ((VAR)-=1)
# define PFE_SEM_DESTROY(VAR) do {} while(0)
# define PFE_THR_NONE
# define PFE_THR_TYPE int
# define PFE_THR_ARG void*
# define PFE_THR_SPAWN(VAR,FUNC,ARG,STACK)  ((VAR) = 0)
# define PFE_THR_EXIT(EXITCODE) (exit(EXITCODE))
# define PFE_THR_KILL(VAR, EXITCODE) do {} while (0)
# define PFE_THR_SELF()  (0)
# define PFE_THR_YIELD(VAR)  do {} while (0)
#endif
