/** 
 * -- Words for dynmaic loading of code modules
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE                %derived_by: guidod %
 *  @version %version: bln_mpt1!33.30 %
 *    (%date_modified: Mon Apr 08 20:16:52 2002 %)
 *
 *  @description
 *		This file exports a set of system words for 
 *              dynamic loading of code modules.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: dl-ext.c,v 1.1.1.1 2006-08-08 09:07:35 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-limits.h>

#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <pfe/os-string.h>
#include <time.h>

#include <pfe/option-ext.h>
#include <pfe/_nonansi.h>
#include <pfe/_missing.h>
#include <pfe/logging.h>

#ifdef VxWorks
#include <semLib.h>
#include <sysLib.h>
#include <time.h>
#ifdef  CLOCKS_PER_SEC_BUG
#undef  CLOCKS_PER_SEC
#define CLOCKS_PER_SEC sysClkRateGet()
#endif
#endif

#ifndef PATH_MAX
# ifdef _POSIX_PATH_MAX
# define PATH_MAX _POSIX_PATH_MAX
# else
# define PATH_MAX 255
# endif
#endif

#ifndef VxWorks
static const p4_char_t p4_lit_dlerror[] = "dlerror"; 
#define PFE_WARN_DLERROR (p4_search_option_value (p4_lit_dlerror, 7, 0, PFE.set))
#else
#define PFE_WARN_DLERROR 0  /* let the compiler do some code removal */
#endif

#ifdef PFE_HAVE_USELIB
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
        
        p4_memset (&p4_slot_table[slot], 0, sizeof (struct slot_info));
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
    char name[256];       /* cannot be longer than longest forth-string */
    void* dlptr;          /* the dlopen handle */
    void* (*llist)(void); /* loadlist extraction */
    int use;              /* usecount */
};

typedef void* (*p4_llist_f)(void); /* loadlist extraction typedef */

struct dlslot p4_dlslot_table [P4_DLSLOTS] = {{ "", 0 }};

#define _dlslot_name(_slot) (p4_dlslot_table[_slot].name)

/* copies a forth-module-name into
   name-area being a C-string afterwards.
   A possible shared-object extension is gotten stripped away 
   just as the '\t' prefix to mark systemonly moduleload.
 */
static char* 
module_makename (char* name, const p4_char_t* nameptr, int namelen)
{
    if (nameptr && *nameptr == '\t')
    { nameptr++; namelen--; }
	
    p4_memcpy (name, nameptr, namelen);
    name [namelen] = 0;
    
    if (namelen > 2 
      && ( p4_memequal (&name[namelen-2], ".o", 2)
        || p4_memequal (&name[namelen-2], ".O", 2)
           ))
    {
        name [namelen-2] = 0; /* truncate to basename  */
    }else if (namelen > 3 
      && ( p4_memequal (&name[namelen-3], ".so", 3)
        || p4_memequal (&name[namelen-3], ".sl", 3)
           ))
    {
        name [namelen-3] = 0; /* truncate to basename */
    }
    
    return name;
}

static int
p4_dlslot_find (const p4_char_t* nameptr, int namelen)
{
    int slot;
    int len;
    char name[256];
    
    module_makename (name, nameptr, namelen);
    len = p4_strlen (name);
    
    for (slot = 1; slot < P4_DLSLOTS; slot++)
    {
        if (p4_memequal (_dlslot_name(slot), name, len+1))
            return slot; /* found */
    }
    
    return 0;
}

static int
p4_dlslot_create (const p4_char_t* nameptr, int namelen)
{
    int slot;
    char len;
    char name[PATH_MAX+1];
    
    module_makename (name, nameptr, namelen);
    len = p4_strlen (name);
    
    for (slot = 1; slot < P4_DLSLOTS; slot++)
    {
        if (p4_memequal (_dlslot_name(slot), name, len+1)) {
            return slot; /* found */
        }
    }
    /* none found, need new slot */
    
    for (slot = 1; slot < P4_DLSLOTS; slot++) {
        if (!p4_dlslot_table[slot].use) {
            p4_dlslot_table[slot].use = 1;
            p4_memcpy (p4_dlslot_table[slot].name, name, len+1);
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
  
    p4_memset (&p4_dlslot_table[slot], 0, sizeof (struct dlslot));
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
/* import declarations for sub-routines compiled via dl-def.c */

extern int p4_dlinit(void);
extern const char* p4_dlerror (void);
extern void* p4_dlopenext (const char* name);
extern int p4_dlclose (const void* lib);
extern void* p4_dlsym (const void* lib, const char* symbol);

/* ---------------------------------------------------------------------- */

/* negative return values denote error conditions */
int
p4_dlslot_open (const p4_char_t* nameptr, int namelen)
{
    int slot;
    void* dll = 0;
    char systemonly = 0;

    /* vxworks has only one global symtable where we want to find
     * the loadlist-symbol. This constitutes a race condition that
     * a thread starts to extend the symtable with yet another C object
     * while another thread tries to find the loadlist-sym of the former
     */
#  ifdef VxWorks
    static volatile SEM_ID mutex;
    static int timeout;
    register SEM_ID my_mutex = 0;
    if (! timeout) timeout = 2000 * CLOCKS_PER_SEC; 
    if (! mutex) mutex = my_mutex = semMCreate (0); /* FIXME: memory leak */
    if (! mutex) { 
        P4_fatal ("PANIC: no sem-id from semMCreate!!");
        return -ECONNREFUSED;
    }
    if (my_mutex && my_mutex != mutex) {
        P4_fatal ("PANIC: race detected, doubled smMCreate!!");
        return -EISCONN;
    }
    if (semTake (mutex, timeout)) {
        P4_fatal ("PANIC: semTake failed (timeout? invalid?)");
        return -ETIMEDOUT;
    }
#  define mutexGive() semGive(mutex)
#  else
#  define mutexGive()
#  endif    

    if (*nameptr == '\t') {
	P4_debug (13,"system only...");
	systemonly=1;
	nameptr++; namelen--;
    }

    if ((slot= p4_dlslot_find (nameptr, namelen)))
    {
        p4_dlslot_table[slot].use++;
        P4_info2 ("module already loaded: [%i] '%s'", 
		  slot, _dlslot_name(slot));
        mutexGive ();
        return slot;
    } /*else*/
    
    slot= p4_dlslot_create (nameptr, namelen);   
    
    P4_enter1 ("loading '%s'", _dlslot_name(slot));
    
    if (! p4_strchr (_dlslot_name(slot), '/') && 
	! p4_strchr (_dlslot_name(slot), ':'))
    {
	/* char named[255] = PFE_LIB_DIR;  // system module path */
	char named[P4_POCKET_SIZE]; const char* paths = *P4_opt.lib_paths;
	while (*paths)
	{
	    char* p = named; 
	    while (*paths && *paths == PFE_PATH_DELIMITER)
	    { paths++; }
	    if (!*paths) break;
	    do { *p++ = *paths++; }
	    while (*paths && *paths != PFE_PATH_DELIMITER);

	    if (p[-1] != '/' && p[-1] != '\\' && p[-1] != ':') /* DELIMITERS */
		*p++ = PFE_DIR_DELIMITER;
	    *p = '\0'; p4_strlcat (named, _dlslot_name(slot), P4_POCKET_SIZE);
	    dll = p4_dlopenext (named);
	    if (dll) break; /* goto register in [slot].dlptr */
	    if (PFE_WARN_DLERROR)
	    { P4_warn1 ("%s", p4_dlerror ()); }
	}
	if (! dll && systemonly)
	    goto skipdirectpath;
    }
    if (! dll)
	dll = p4_dlopenext (_dlslot_name(slot)); /* direct path */
 skipdirectpath:
    if (! dll) 
    { 
	if (PFE_WARN_DLERROR)
	{   P4_warn1 ("%s", p4_dlerror ()); }
	else /* whatever the system prints */
	{   p4_dlerror (); }
        p4_dlslot_remove (slot);
        mutexGive ();                          /* <-- important !! */
        return -ENOENT;                        /* before returning */
    }

    p4_dlslot_table[slot].dlptr = dll;  

#  ifdef PFE_SUFFIX
#  define PFE_MODULE_LOADLIST_SYMBOL P4STRING(P4SUFFIX(p4_LTX_,p4_MODULE))
#  else
#  define PFE_MODULE_LOADLIST_SYMBOL                   "p4_LTX_p4_MODULE"
#  endif
    p4_dlslot_table[slot].llist = p4_dlsym (dll, PFE_MODULE_LOADLIST_SYMBOL);
    if (p4_dlslot_isnt_unique_llist (slot)) 
        p4_dlslot_table[slot].llist = NULL;
    
    if (!p4_dlslot_table[slot].llist) 
    {
        P4_fail1 ("'%s': cannot find modules loadlist", _dlslot_name(slot));
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
            p4_dlerror (); 
        else
            P4_note1 ("done dlunmap: '%s'", _dlslot_name(slot)); 
        
        p4_dlslot_remove (slot);
    } else {
        P4_leave1 ("kept dlmap: '%s'", _dlslot_name(slot));
    }
}

void
p4_forget_loadm (void) /* FCode_RT */
{   FX_USE_BODY_ADDR {
    int slot = FX_POP_BODY_ADDR[0];
  
    P4_note1 ("unuse dlslot %i", slot);
    
    p4_dlslot_close (slot);
}}

void
p4_forget_loadm_prelinked (void) /* FCode_RT */
{   FX_USE_BODY_ADDR {
    int slot = FX_POP_BODY_ADDR[0];
    P4_note1 ("unuse prelinked %i", slot);
}}

void*
p4_loadm (const p4_char_t* nm, int l)
{
    int slot; 

    P4_enter2("dlmap of '%.*s'", l, nm); 
  
    slot = p4_dlslot_open (nm, l); 
  
    P4_info1 ( "dlslot: %d", slot);

    if (slot >= 0) 
    {        
        /* register it and load the words exported by that module */
        p4_forget_word ("%s.O", (p4cell) _dlslot_name(slot), 
                        p4_forget_loadm, (p4cell) slot);
    
        if (p4_dlslot_table[slot].llist) 
        {
            void* llist = (*p4_dlslot_table[slot].llist)();
            
            p4_load_words (llist, ONLY, 1); /* load the words */
        
            P4_leave1 ("done loadlist at %p", (void*) *SP);
            return llist;
        } else {
            P4_leave ("nothing loaded");
            return 0;
        }
    }

    /* no external module loaded, try to find a prelinked module */
    if (PFE.set->modules)
    {
	/*  extern const p4Words P4WORDS(internal);
	 *  register int k  = P4WORDS(internal).n;
	 *  const p4Word* w = P4WORDS(internal).w;
	 */
	register int k  = ((p4Words*)PFE.set->modules)->n;
	const p4Word* w = ((p4Words*)PFE.set->modules)->w;
	
        for ( ; --k >= 0; w++ )
        {
            const char* name;
            int len;
            void* ptr;

            if (! w || ! w->name) continue;
            name = w->name +2;
            len = p4_strlen (w->name+2);
            ptr = w->ptr;
            if (*w->name != p4_LOAD) continue;
            if (len != l) continue;
            if (p4_strncmpi (name, (const char*) nm, l)) continue;

            /* found it ! */
            p4_forget_word ("%s.O", (p4cell) name, 
                            p4_forget_loadm_prelinked, (p4cell) -k);
            p4_load_words (ptr, ONLY, 1); /* load the words */
            return ptr;
        }
    }

    return 0;  /* no luck in either case */
}

/** (LOADM) ( module-bstr* -- module-sucess# ) [FTH]
 */   
FCode (p4_paren_loadm)
{
    SP[1] = (p4cell) p4_loadm ((*(p4_char_t**)SP)+1, **(p4_char_t**)SP);
}

int
p4_loadm_test(const p4_char_t* nm, int l)
{
    char name[260]; 
    if (! nm || l<2) return 0;
    module_makename (name, nm, l);
    p4_strcat (name, ".O"); /* ... name *is* long enough ... */
    if (p4_search_wordlist ((p4_char_t*) name, p4_strlen(name), PFE.atexit_wl))
    { 
	P4_info1 ("%s: is loaded (skipped)", name);
	return -1; /* already loaded */
    }else{
	return 0; /* not yet loaded */
    }
}

void*
p4_loadm_once(const p4char* nm, int l)
{
    if (p4_loadm_test (nm, l)) {
	return (void*)(-1); /* already loaded */
    }else{
	return p4_loadm (nm, l);
    }
}

/** LOADM ( "filename" -- ) [FTH]
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
    p4_charbuf_t* fn = p4_word (' ');
    
    if (! p4_loadm_once (fn+1, *fn))
	p4_outf (" -- load failed: '%.*s'\n", *fn, fn+1);
}

/** LOCAL-DLSYM ( [symbol] -- symbol-addr ) [FTH] [EXEC]
 *  lookup the symbol that follows and leave the address (or null)
 */
FCode (p4_local_dlsym)
{
    FX (p4_Q_exec); /* currently ignored while compiling */
    
    p4_word_parseword (' '); /* PARSE-WORD + WORD>HERE */
  
    if (PFE.word.len) {
        FX_PUSH (p4_dlsym (0, p4_word_to_here ()));
        
        if (! *SP) {
            P4_warn1 ("no dlsym '%s'", p4_HERE+1);
        }
    }
}

/** LOCAL_DLCALL ( x...[8] [symbol] -- x...[8] ) [FTH] [EXEC]
 *  does not do any stack changes, the caller
 *  must clean up the stack himself. You don't even
 *  get the return value (sigh)
 */
FCode (p4_local_dlcall)
{
    register 
	void (*f)(p4cell,p4cell,p4cell,p4cell,p4cell,p4cell,p4cell,p4cell);
    
    FX (p4_Q_exec); /* currently ignored while compiling */
    
    p4_word_parseword (' '); /* PARSE-WORD + WORD>HERE */
    
    if (PFE.word.ptr) 
    {
        f = p4_dlsym (0, p4_word_to_here ());
        
        if (! f) { 
            P4_warn1 ("no dlsym '%s'", p4_HERE+1);
	}else{
            (*f)(SP[0], SP[1], SP[2], SP[3], SP[4], SP[5], SP[6], SP[7]);
        }
    }
}

/* ----------------------- direct calls into lower layer ----------------- */

/** lt_dlinit ( -- dlinit-ior# ) [FTH]
 * initialiize library, usually open the program itself so that its
 * handles can be found under "0"
 */
FCode (p4_lt_dlinit)
{
    FX_PUSH (p4_dlinit ());
}

/** lt_dlopenext ( module-ptr module-len -- module-dl-handle*! | 0 ) [FTH]
 * walk the searchpath for dlopen and try to open a binary module
 * under the given name with the usual file extension for the 
 * current system.
 */
FCode (p4_lt_dlopenext)
{
    SP[1] = (p4ucell) 
        p4_dlopenext (p4_pocket_filename ((p4_char_t*)(SP[1]), SP[0]));
    FX_DROP;
}

/** lt_dlsym ( symbol-ptr symbol-len module-dl-handle* -- symbol*! | 0 ) [FTH]
 * try to find the name in the binary module denoted by its handle
 * .. if handle is null, use the main body of the program
 */
FCode (p4_lt_dlsym)
{
    SP[2] = (p4ucell)
        p4_dlsym ((void*)(SP[0]), 
                  p4_pocket_filename ((char*)(SP[2]), SP[1]));
    FX_2DROP;
}

/** lt_dlclose ( module-dl-handle* -- module-ior# ) [FTH]
 * close handle that was returned by => lt_dlopenext
 */
FCode (p4_lt_dlcose)
{
    *SP = p4_dlclose ((void*)(*SP));
}

/** lt_dlerror ( -- dlerror-zstr* )
 * returns string describing the last dlerror as for => lt_dlopenext
 * and => lt_dlsym
 */
FCode (p4_lt_dlerror)
{
    FX_PUSH (p4_dlerror ());
}

P4_LISTWORDS (dlfcn) =
{
    P4_INTO ("FORTH", 0),
    P4_FXco ("(LOADM)",		p4_paren_loadm),
    P4_FXco ("LOADM",    	p4_loadm),
    
    P4_IXco ("LOCAL-DLSYM",	p4_local_dlsym),
    P4_IXco ("LOCAL-DLCALL",	p4_local_dlcall),
    P4_iOLD ("DLSYM",		"LOCAL-DLSYM"),
    P4_iOLD ("DLCALL",		"LOCAL-DLCALL"),

    P4_INTO ("EXTENSIONS", 0),
# if 0 && defined _target_os_linux && !defined __cplusplus
  /** accessing shared libraries - only available for i386-linux */
    P4_FXco ("CALL-C",	  p4_call_c),
# endif

# ifdef PFE_HAVE_USELIB
    P4_FXco ("USELIBRARY",	p4_uselibrary),
# endif

    P4_FXco ("lt_dlinit",          p4_lt_dlinit),
    P4_FXco ("lt_dlopenext",       p4_lt_dlopenext),
    P4_FXco ("lt_dlsym",           p4_lt_dlsym),
    P4_FXco ("lt_dlclose",         p4_lt_dlcose),
    P4_FXco ("lt_dlerror",         p4_lt_dlerror),
};
P4_COUNTWORDS (dlfcn, "Dynamic-Loading of code modules");

/*@}*/

/*
 * Local variables:
 * c-file-style: "stroustrup"
 * End:
 */
