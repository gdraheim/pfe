/** 
 * -- Words for dynmaic loading of code modules
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE                %derived_by: guidod %
 *  @version %version: 1.13 %
 *    (%date_modified: Mon Mar 12 14:40:03 2001 %)
 *
 *  @description
 *		This file exports a set of system words for 
 *              dynamic loading of code modules.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: dlfcn-ext.c,v 0.30 2001-03-12 13:40:33 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-xtra.h>

#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <time.h>

#include <pfe/dlfcn-sub.h>
#include <pfe/_nonansi.h>
#include <pfe/_missing.h>
#include <pfe/logging.h>

#ifdef VxWorks
#include <semLib.h>
#include <sysLib.h>
#include <time.h>
#ifdef  CLOCKS_PER_SEC_BUG
#define CLOCKS_PER_SEC sysClkRateGet()
#endif
#endif

extern Head *
   p4_make_head (const char *name, int count, char **nfa, Wordl * wid);
   
#ifndef PATH_MAX
# ifdef _POSIX_PATH_MAX
# define PATH_MAX _POSIX_PATH_MAX
# else
# define PATH_MAX 255
# endif
#endif

#ifdef HAVE_USELIB
FCode (p4_uselibrary)
{
    extern int uselib (const char* lib);
    /* uselib is a linux' syscall that was supposed to live in
     * unistd.h but it had never been included there, afaik.
     * Still, it can be called in all linux versions so far.
     */
    *SP = uselib ((char *) *SP);
}
#endif

#if 0 && defined __target_os_linux && !defined (__cplusplus)
/* ********************************************************************** */
/* Linux shared library calls -- KAH 930824				  */
/* ********************************************************************** */

/* direct call to shared library function (without loadlist)   */

typedef struct
{
    unsigned nargs	:4;	/* # of arguments */
    unsigned restype	:2;	/* result type */
    unsigned		:2;	/* reserved -- complex res */
    unsigned argtype	:2;	/* double/long arg flags */
    unsigned		:22;	/* other argtypes shifted */
				/* from here */
} control_word;

#define DYN_INTEGER	0
#define DYN_LONGINT	1
#define DYN_FLOAT	2
#define DYN_LONGFLOAT	3

#define _exec(sub,resulthi,resultlo)	\
	__asm__ __volatile__ ("call *%2;movl %%edx, %0;movl %%eax, %1":	\
	"=g" (resulthi), "=g" (resultlo):"g" (sub): "eax", "edx");
#define _cpush(x)	\
	__asm__ __volatile__ ("pushl %0;"::"g" (x));

/* not sure if this float stuff is right.  Are singles and 
   doubles same length?  Are these stored in a different forth stack?
   How do I copy things back and forth from int/pointer stack to
   float stack? */

#ifndef P4_NO_FP
#define lowfresult(x)	__asm__ __volatile__ ("fstpl %0;":"=g" (x));
#define highfresult(x)	lowfresult(x);
#endif

static void
call_c (p4code * sub)
{
    int i, high, low, result_type;
    p4udcell *tmp;
    control_word x = *(control_word *) SP++;
    
    i = x.nargs;
    result_type = x.restype;
#  ifndef P4_NO_FP
    tmp = (p4udcell *) FP;
#  else
    tmp = (p4udcell *) SP;
#  endif
    while (i--)
    {
        switch (x.argtype)
	{
         case DYN_LONGINT:
             _cpush (*SP++);
         case DYN_INTEGER:
             _cpush (*SP++);
             break;
#       ifndef P4_NO_FP
         case DYN_LONGFLOAT:
             _cpush (tmp->hi);
         case DYN_FLOAT:
             _cpush (tmp++->lo);
#       endif
	}
        *(p4ucell *) & x >>= 2;
    }
    _exec (sub, high, low);
    switch (result_type)
    {
     case DYN_INTEGER:
         *--SP = low;
         break;
     case DYN_LONGINT:
         *--SP = low;
         *--SP = high;
         break;
#  ifndef P4_NO_FP
     case DYN_FLOAT:
         lowfresult (*--tmp);
         break;
     case DYN_LONGFLOAT:
         lowfresult (*--tmp);
         highfresult (*--tmp);
#  endif
    }
}

FCode (p4_call_c)
{
    call_c ((p4code *) * SP++);
}

#endif /* Linux shared library calls */


/* ********************************************************************* */

/*
   The following section defines the funtions necessary to support
   forth's dynamically loading modules. It is split into
   two parts: one is loading shared-objects into the task's
   code-space, the other assigns slots in the threaded forth-structure
   that are used to contain a pointer to an object-local data-space.
   
   We will assume, that a single threaded register-based pointer p4TH
   exists which points to a structure holding pointers to
   dl-local mem-space. The id's for the data-space must 
   be given per code-space, so unlike most other data-spaces, 
   here we have them made static here - and it ain't no error.
   
   The slot-id can be both load-dynamic or compile-static.
   In dynamic mode, the code-space slot-var is null before, and
   after slot_use it contains the slot-index grabbed from the
   pool of free slot-indices.
   In compiled mode, the code-space slot-var contains a slot-index;
   slot_use accepts this as a request to a specific slot, and
   does only check if it had still been free, otherwise fails
   without assigning another number from the pool of free slots.
   
   All forth-machines using the same shared-object which in turn
   needs a threaded data-space, will have the same slot-index
   in use.
   
   slot 0 is valid but not assignable - it's the forth dictionary space.
   It cannot be touched by any of the following calls. The
   slot 0 is only touched by the cleanup-routines that calls
   mem-free for all alloc'ed slots in a thread.
*/

#undef  INBETWEEN
#define INBETWEEN(x,a,e) ((a) < (x) && (x) < (e))

#undef  BETWEEN
#define BETWEEN(x,a,e) ((a) <= (x) && (x) < (e))

struct slot_info
{
    int* var;          /*(self-referential) ptr to global var holding slot id*/
    short use;         /* use count */
    char  got;         /* (*var) had been null before we had written to it */
};

static struct slot_info p4_slot_table[P4_MOPTRS] = {{0}}; 

/** 
 * request a slot index. The index is written to the variable arg-address.
 * if the arg-address contains a value != 0, we check if that specific
 * slot index is free for assignment - or already assigned to this variable.
 * a slot_use can be done for the same slot-variable multiple times,
 * which will increase a use-counter. Call slot_unuse correspondingly.
 */
int
p4_slot_use (int* var)
{
    if (!var || !BETWEEN(*var,0,P4_MOPTRS)) 
        return -EINVAL;

    if (*var)  /* has an id */
    {
        if (var != p4_slot_table[*var].var) 
        {
            if (!p4_slot_table[*var].var)  /* not used so far */
            {
                p4_slot_table[*var] .var = var;
                p4_slot_table[*var] .use = 1;
                return 0;
            } else {
                return -EACCES;
            }
        } else { /* reuse id */
            p4_slot_table[*var].use++;
            return 0;
        }
    } else {
        /* new id */
        register int i;
        for (i = 1; i < P4_MOPTRS; i++) 
        {
            if (!p4_slot_table[i].var) 
            {
                p4_slot_table[i] .var = var;
                p4_slot_table[i] .got = 1; /* we're writing to var now */
                p4_slot_table[i] .use = 1;
                *var = i;
                return 0;
            }
        }
        return -EBUSY;
    }
}

/**
 * => p4_slot_use
 */
int
p4_slot_unuse (int* var)
{
    if (!var || !INBETWEEN(*var,0,P4_MOPTRS)
      || p4_slot_table [*var].var != var)
        return -ENOENT;
    
    if (! --p4_slot_table [*var].use) 
    {   /* real free of slot */
        register int slot = *var;
        
        if (p4_slot_table[slot].got) /* had written to var */
            *var = 0;                 /* so be careful and reset it */
        
        memset (&p4_slot_table[slot], 0, sizeof (struct slot_info));
    } 
    
    return 0;
}  


/* ============= and here's the loading part ================= */

/* here again, dlslot 0 is invalid for dynamic loading. It's not
   actually reserved, but, well handy to write
*/

#define P4_DLSLOTS P4_MOPTRS

/* keep `name` to be the first entry, so we can cast to (const char*) */
struct dlslot
{
    char name[256];   /* cannot be longer than longest forth-string */
    void* dlptr;      /* the dlopen handle */
    void* (*llist)(); /* loadlist extraction */
    int use;          /* usecount */
};

typedef void* (*p4_llist_f)(); /* loadlist extraction typedef */

struct dlslot p4_dlslot_table [P4_DLSLOTS] = {{ "", 0 }};


/* copies a forth-module-name into
   name-area being a C-string afterwards.
   A possible shared-object extension is gotten stripped away 
 */
static char* 
module_makename (char* name, const char* nameptr, int namelen)
{
    memcpy (name, nameptr, namelen);
    name [namelen] = 0;
    
    if (namelen > 2 
      && ( !memcmp (&name[namelen-2], ".o", 2)
        || !memcmp (&name[namelen-2], ".O", 2)
           ))
    {
        name [namelen-2] = 0; /* truncate to basename  */
    }else if (namelen > 3 
      && ( !memcmp (&name[namelen-3], ".so", 3)
        || !memcmp (&name[namelen-3], ".sl", 3)
           ))
    {
        name [namelen-3] = 0; /* truncate to basename */
    }
    
    return name;
}

static int
p4_dlslot_find (const char* nameptr, int namelen)
{
    int slot;
    int len;
    char name[256];
    
    module_makename (name, nameptr, namelen);
    len = strlen (name);
    
    for (slot = 1; slot < P4_DLSLOTS; slot++)
    {
        if (!memcmp (p4_dlslot_table[slot].name, name, len+1))
            return slot; /* found */
    }
    
    return 0;
}

static int
p4_dlslot_create (const char* nameptr, int namelen)
{
    int slot;
    char len;
    char name[PATH_MAX+1];
    
    module_makename (name, nameptr, namelen);
    len = strlen (name);
    
    for (slot = 1; slot < P4_DLSLOTS; slot++)
    {
        if (!memcmp (p4_dlslot_table[slot].name, name, len+1)) {
            return slot; /* found */
        }
    }
    /* none found, need new slot */
    
    for (slot = 1; slot < P4_DLSLOTS; slot++) {
        if (!p4_dlslot_table[slot].use) {
            p4_dlslot_table[slot].use = 1;
            memcpy (p4_dlslot_table[slot].name, name, len+1);
            p4_dlslot_table[slot].dlptr = 0; /* mark just created */
            return slot; /* new */
        }
    }
    
    return -EBUSY; /* none found, table full, we're very sorry, dave */
}

static void
p4_dlslot_remove (int slot)
{
    if (!INBETWEEN(slot, 0, P4_DLSLOTS))
    {
        P4_warn1 ("dlslot %i out of range", slot);
        return;
    }
  
    memset (&p4_dlslot_table[slot], 0, sizeof (struct dlslot));
}  


static int
p4_dlslot_isnt_unique_llist (int slot)
{
    int i;
    
    if (!INBETWEEN(slot, 0, P4_DLSLOTS))
        return -EINVAL;
     
    if (!p4_dlslot_table[slot].use
      ||  !p4_dlslot_table[slot].llist)
        return -ENOENT;

    for (i = 1; i < P4_DLSLOTS; i++)
    {
        if (i == slot)
            continue;
        
        if (p4_dlslot_table[slot].llist && p4_dlslot_table[i].llist 
          &&  p4_dlslot_table[slot].llist == p4_dlslot_table[i].llist)
            return 1; /* isnt unique */
    }
    
    return 0; /* is unique */
}

/* ---------------------------------------------------------------------- */

#ifndef PFE_MODULE_DIR
#define PFE_MODULE_DIR PFE_PKGLIBDIR
#endif

/* negative return values denote error conditions */
int
p4_dlslot_open (const char* nameptr, int namelen)
{
    int slot;
    void* dll;

    /* vxworks has only one global symtable where we want to find
     * the loadlist-symbol. This constitutes a race condition that
     * a thread starts to extend the symtable with yet another C object
     * while another thread tries to find the loadlist-sym of the former
     */
#  ifdef VxWorks
    static volatile SEM_ID mutex;
    static int timeout;
    if (! mutex) mutex = semMCreate (0); /* FIXME: memory leak */
    if (! timeout) timeout = 2000 * CLOCKS_PER_SEC; 
    semTake (mutex, timeout);
#  define mutexGive() semGive(mutex)
#  else
#  define mutexGive()
#  endif    

    if ((slot= p4_dlslot_find (nameptr, namelen)))
    {
        p4_dlslot_table[slot].use++;
        P4_info2 ("module already loaded: [%i] '%s'", 
                  slot, p4_dlslot_table[slot].name);
        
        mutexGive ();
        return slot;
    } /*else*/
    
    slot= p4_dlslot_create (nameptr, namelen);   
    
    P4_enter1 ("loading '%s'", p4_dlslot_table[slot].name);
    
    dll = p4_dlopen ("",p4_dlslot_table[slot].name);
    if (!dll)
        dll = p4_dlopen (PFE_MODULE_DIR, p4_dlslot_table[slot].name);
    
    if (!dll) 
    { 
        p4_dlerror (p4_dlslot_table[slot].name);
        p4_dlslot_remove (slot);
        mutexGive ();
        return -ENOENT; 
    }

    p4_dlslot_table[slot].dlptr = dll;  
    
    p4_dlslot_table[slot].llist = p4_dlsym (dll, "p4_LTX_p4_MODULE");
    if (p4_dlslot_isnt_unique_llist (slot)) 
        p4_dlslot_table[slot].llist = NULL;
    
    if (!p4_dlslot_table[slot].llist) 
    {
        P4_fail1 ("'%s': cannot find modules loadlist", 
          p4_dlslot_table[slot].name);
        p4_dlclose (dll);
        p4_dlslot_remove (slot);
        mutexGive ();
        return -ESRCH;
    }
    
    P4_leave1 (" dlslot = %i", slot);
    mutexGive ();
    return slot;
#  undef mutexGive
}

void
p4_dlslot_close (int slot)
{
    if (!INBETWEEN(slot,0,P4_DLSLOTS)) {
        P4_warn1 ("dlslot %i out of range", slot);
        return;
    }
    
    if (! --p4_dlslot_table[slot].use) 
    {
        if (p4_dlclose (p4_dlslot_table[slot].dlptr)) 
            p4_dlerror ("p4_dlslot_close"); 
        else
            P4_note1 ("done dlunmap: '%s'", p4_dlslot_table[slot].name); 
        
        p4_dlslot_remove (slot);
    } else {
        P4_leave1 ("kept dlmap: '%s'", p4_dlslot_table[slot].name);
    }
}

void
p4_forget_loadm (void)
{
    int slot = WP_PFA[0];
  
    P4_note1 ("unuse dlslot %i", slot);
    
    p4_dlslot_close (slot);
}

/** (LOADM) ( bstring -- sucess )
 */   
FCode (p4_loadm2)
{
    int slot; 
    
    P4_enter2("dlmap of '%.*s'", **(char**)SP, *(char**)SP+1); 
  
    slot = p4_dlslot_open ((*(char**)SP)+1, **(char**)SP); 
  
    P4_info1 ( "dlslot: %d", slot);
    if (slot < 0) { *SP = 0; return; }
    
    p4_forget_word ("%s.O", (p4cell) p4_dlslot_table[slot].name, 
      p4_forget_loadm, (p4cell) slot);
    
    if (p4_dlslot_table[slot].llist) 
    {
        void* llist = (*p4_dlslot_table[slot].llist)();
        
        p4_load_llist (llist, ONLY, 1); /* load the words */
        
        *SP = (p4cell)( llist );
    } else {
        *SP = 0;
    }
    
    P4_leave1 ("done loadlist at %p", (void*) *SP);
}

/** LOADM ( 'filename' -- )
 *  dlmap the shared object (or share an already mapped object)
 *  and run the per-thread initialization code. This is the
 *  user-convenient function, otherwise use => (LOADM)
 simulate:
   : LOADM  BL WORD   
     ((IS_MODULE_LOADED)) IF EXIT THEN 
     HERE (LOADM)  0= IF ." -- load failed: " HERE COUNT TYPE CR THEN ;
 */
FCode (p4_loadm)
{
    char* fn = p4_word (' ');
    
    { 
        char name[260]; module_makename (name, fn+1, *fn);
        strcat (name, ".O"); /* ... name *is* long enough ... */
        if (p4_search_wordlist (name, strlen(name), PFE.atexit_wl))
        { 
            P4_info1 ("%s: is loaded (skipped)", name);
            return; /* already loaded */
        }
    }
    
    FX_PUSH (fn);
    FX (p4_loadm2);
    if (FX_POP) return;
    p4_outf (" -- load failed: '%.*s'\n", *fn, fn+1);
}

/** DLSYM ( [symbolname] -- address ) exec-only
 *  lookup the symbol that follows and leave the address (or null)
 */
FCode (p4_dlsym)
{
    char* p; p4ucell plen;
    char name[256];
    
    FX (p4_Q_exec); /* currently ignored while compiling */
    
    p4_parseword (' ', &p, &plen);
  
    if (plen) {
        memcpy (name, p, plen);
        name[plen] = '\0';
        FX_PUSH (p4_dlsym (0, name));
        
        if (! *SP) {
            P4_warn1 ("no dlsym '%s'", name);
        }
    }
}

/** DLCALL ( x8 ... x2 x1 [symbolname] -- x8 ... x2 x1 ) exec-only
 *  does not do any stack changes, the caller
 *  must clean up the stack himself. You don't even
 *  get the return value (sigh)
 */
FCode (p4_dlcall)
{
    char* p; p4ucell plen;
    char name[256];
    void (*f)();
    
    FX (p4_Q_exec); /* currently ignored while compiling */
    
    p4_parseword (' ', &p, &plen);
    
    if (plen) 
    {
        memcpy (name, p, plen);
        name[plen] = '\0';
        f = p4_dlsym (0, name);
        
        if (f) { 
            (*f)(SP[0], SP[1], SP[2], SP[3], SP[4], SP[5], SP[6], SP[7]);
        }else{
            P4_warn1 ("no dlsym '%s'", name);
        }
    }
}

P4_LISTWORDS (dlfcn) =
{
# if 0 && defined _target_os_linux && !defined __cplusplus
  /** accessing shared libraries - only available for i386-linux */
    CO ("CALL-C",	  p4_call_c),
# endif

# ifdef HAVE_USELIB
    CO ("USELIBRARY",	  p4_uselibrary),
# endif

    CO ("(LOADM)",        p4_loadm2),
    CO ("LOADM",    	  p4_loadm),
    
    CI ("DLSYM",          p4_dlsym),
    CI ("DLCALL",         p4_dlcall),
};
P4_COUNTWORDS (dlfcn, "Dynamic-Loading of code modules");

/*@}*/

/*
 * Local variables:
 * c-file-style: "stroustrup"
 * End:
 */
