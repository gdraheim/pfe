#ifndef __PFE_DEF_TYPES_H
#define __PFE_DEF_TYPES_H "%full_filespec: def-types.h~5.13:incl:bln_12xx!1 %"

/** 
 * -- pfe's data structures
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 5.13 %
 *    (%date_modified: Mon Mar 12 10:33:25 2001 %)
 *
 *  @description
 *     p4th's forth data structures definitions, types and sys.names
 *
 */
/*@{*/

#include <pfe/incl-ext.h>

#include <setjmp.h>
#include <stdio.h>

#if defined HAVE_SYS_TYPES_H || defined PFE_HAVE_SYS_TYPES_H
# include <sys/types.h>		/* size_t, time_t and friends */
#endif

#include <pfe/def-paths.h>

#include <pfe/lined.h>

/* options section */

#ifndef P4_ORDER_LEN     /* maximum word lists in search order */
#define P4_ORDER_LEN 16
#endif
#ifndef P4_THREADS_SHIFT /* 2^n number of threads in a word list */
#define P4_THREADS_SHIFT 5
#endif
#ifndef P4_LOCALS_SHIFT  /* 2^n number of local variables allowed */
#define P4_LOCALS_SHIFT 5
#endif

#ifndef P4_MAX_FILES	/* maximum number of open files */
#define P4_MAX_FILES	0x10
#endif
#ifndef P4_POCKETS	/* number of buffers and */
#define P4_POCKETS	8	
#endif
#ifndef P4_POCKET_SIZE	/* size of buffers for interpretation of S" */
#define P4_POCKET_SIZE	0x100	
#endif
#ifndef	P4_MIN_HOLD	/* minimum space for <# # #S HOLD #> etc. */
#define P4_MIN_HOLD	0x100	
#endif
#ifndef P4_MIN_PAD	/* minimum free space in PAD */
#define P4_MIN_PAD	0x400	
#endif
#ifndef P4_HISTORY_SIZE	/* size of command line history buffer */
#define P4_HISTORY_SIZE	0x1000
#endif

#ifndef P4_MP		 /* how many machines by default */
#define P4_MP 		1
#endif
#ifndef P4_MP_MAX	/* how many machines as maximum */
#define P4_MP_MAX 	8 
#endif

/* some constants needed here, they are dependent of options */

#define P4_LOCALS  (1<<P4_LOCALS_SHIFT)
#define P4_THREADS (1<<P4_THREADS_SHIFT)

#define P4_TIB_SIZE 	0x100 /* size of terminal input buffer */
#define P4_BLK_SIZE	0x400 /* bytes per block */

#ifndef PATH_LENGTH		/* suggested by Andrew Houghton */

# if defined _POSIX_PATH_MAX
#   define PATH_LENGTH _POSIX_PATH_MAX
# elif defined MAXPATHLEN
#   define PATH_LENGTH MAXPATHLEN
# elif defined MAXNAMLEN
#   define PATH_LENGTH MAXNAMLEN
# elif defined PATH_MAX
#   define PATH_LENGTH PATH_MAX
# else
#   define PATH_LENGTH PFE_DEF_PATH_MAX
# endif

#endif
  /* ndef PATH_LENGTH */
  
  
#ifdef _P4_SOURCE
/* heritage compatibility, to be removed in next version */

# define ORDER_LEN     	P4_ORDER_LEN
# define LD_THREADS    	P4_THREADS_SHIFT
# define LD_LOCALS     	P4_LOCALS_SHIFT
  
# define MAX_FILES	P4_MAX_FILES
# define POCKETS	P4_POCKETS
# define POCKET_SIZE	P4_POCKET_SIZE

# define MIN_HOLD	P4_MIN_HOLD
# define MIN_PAD	P4_MIN_PAD
# define HISTORY_SIZE	P4_HISTORY_SIZE

# define MAX_LOCALS	P4_LOCALS
# define THREADS	P4_THREADS

# define TIB_SIZE     	P4_TIB_SIZE
# define BPBUF  	P4_BLK_SIZE
  
#endif

typedef p4cell  (*p4cell_p4code) (void); /* very useful sometimes */
typedef p4ucell (*p4ucell_p4code) (void); /* very useful sometimes */

typedef struct p4_Head 	 p4_Head;   /* pointer set comprising a definition */
typedef struct p4_Wordl  p4_Wordl;  /* body of a word list */
typedef struct p4_File 	 p4_File;   /* neccessary information about open files */
typedef struct p4_Dict 	 p4_Dict;   /* a dictionary specification */
typedef struct p4_Input  p4_Input;  /* an input source specification */
typedef struct p4_Iframe p4_Iframe; /* a frame to save an input specification */
typedef struct p4_Except p4_Except; /* an exception frame */

struct p4_Head			/* set of pointers comprising a header */
{				/* when preceeded by a name */
    char *link;			/* link back to previous word in vocabulary */
# ifndef PFE_WITH_FIG
#   if defined CPU_i960 || defined __target_cpu_i960
    p4code i960_padding;        /* the i960 does align to 8-bytes anyway */
#   endif                       /* (yet each pointer is 4-bytes there ...) */
    p4code aux;			/* execution semantics or pointer for DOES> */
# endif
    p4code cfa;			/* compilation/interpretation semantics */
};

struct p4_Wordl			/* a word list */
{				/* a word list identifier is a (Wordl *) */
    char *thread[P4_THREADS];
    p4_Wordl *prev;		/* word lists are chained */
    p4_Wordl *also;		/* search also this dictionary, kinda FIG*/
    p4cell  flag;		/* do to_upper, a copy of sys.world_flag */
#define WORDL_NOCASE (1<<0)
#define WORDL_NOHASH (1<<1)
#define WORDL_CURRENT (1<<2)
};

struct p4_File			/* describes a file */
{
    FILE *f;			/* associated ANSI-C file structure */
    char mdstr[4];		/* mode string for fopen() */
    char mode;			/* mode code for open_file() */
    signed char last_op;	/* -1 write, 0 none, 1 read */
    p4word len;			/* if stream: length of input line */
    p4ucell size;		/* if block file: size of file in blocks */
    long pos;			/* a saved position, e.g. beginning of line */
    p4ucell n;			/* block in buffer or source line */
    p4cell updated;		/* if block file: block updated? */
    char name[PATH_LENGTH];	/* file name */
    char buffer[P4_BLK_SIZE];	/* buffer for block or input line */
};

struct p4_Input			/* an input source specification */
{
    p4cell source_id;		/* SOURCE-ID */
    p4_File *block_file;	/* which file is active? */
    p4ucell blk;		/* currently loaded block */
    long pos;			/* stream file: beginning of line in file */
    char *tib;			/* points to TIB or to EVALUATED string */
    p4ucell number_tib;		/* #TIB, length of string in TIB */
    p4ucell to_in;		/* input parsing position */
};

struct p4_Iframe		/* a frame to save an input specification */
{
    p4cell magic;
    p4_Input input;
    p4_Iframe *prev;
};

struct p4_Except
{
    p4cell magic;
    p4xt *ipp;
    p4cell *spp;
    p4cell *lpp;
    double *fpp;
    p4_Iframe *iframe;
    jmp_buf jmp;
    p4_Except *prev;
};
  
  

/* this is called options structure, since this aggregate communicates
   the command-line. But it has enhanced over time. Just expect it to
   be THE data-structure unique to each thread (started via main).
   You can actually have MP_MAX forth-processors per thread, so if
   your OS isn`t multi-threaded, what the heck, use this feature!
*/

typedef struct p4_Session 	p4_Session;
typedef struct p4_Thread	p4_Thread;

typedef struct p4_Session* 	p4_sessionP;
typedef struct p4_Thread*  	p4_threadP;
typedef struct _p4_term_struct p4_term_struct;
  
struct p4_Session
{				
    int argc;
    char** argv;
    unsigned    canonical:2,	/* running in canonical mode */
	        stdio:1,	/* standard input isn't-tty: work as filter */
	        caps_on:1,	/* exchange lower and upper case chars */
	        lower_case_on:1,/* make lower case words find upper case */
	        lower_case_fn:1,/* convert file names to lower case? */
	        float_input:1,	/* disables floating point input when false */
		license:1,	/* show license string at startup */
		warranty:1,	/* show warranty string at startup */
		quiet:1,	/* no messages */
		verbose:1,	/* more messages */
		debug:1,	/* enable a few more outputs */
                bye:1,          /* don't enter the mainloop (non-interact..) */
                lastbit:1;      /* last bit */
    int	cols, rows;	/* size of screen */
    p4ucell	total_size;
    p4ucell	stack_size;
    p4ucell	flt_stack_size;
    p4ucell	ret_stack_size;
    p4ucell	max_files;
    p4ucell	pockets;
    char *	block_file;	/* block file to use initially */
    char *	boot_file;	/* forth script to use initially */
    char *	include_file;	/* file to include after boot */
    char *	incpaths;
    char *	incext;
    char *	blkpaths;
    char *	blkext;
    char *	editor;		/* preferred ASCII text file editor */
        
    /* for VMs */
    int     cpus;          /* how many cpus do we have in this tread */
    /* for mmap */
    char *      mapfile;
    void *      mapbase;
    char *      dumpfile;
    /* image support */
    char *      load_image;    /* --load-image */
    char *      make_image;    /* --make-image */

    char *      prefix;
    char *      bootname;
    char *	bootcommand;
    char **	optv;
    struct { 
	unsigned block_file	: 1;
	unsigned boot_file	: 1;
	unsigned include_file	: 1;
	unsigned incpaths	: 1;
	unsigned incext		: 1;
	unsigned blkpaths	: 1;
	unsigned blkext		: 1;
	unsigned editor		: 1;
	unsigned mapfile	: 1;
	unsigned dumpfile	: 1;
	unsigned load_image	: 1;
	unsigned make_image	: 1;
	unsigned prefix         : 1;
	unsigned bootcommand    : 1;
	unsigned optv		: 1;
    } heap;

    /* dstrings support (ifdef WITH_DSTRINGS): */
    p4ucell     str_buffer_size; /* -$ --string-space */

    p4ucell     padding[4];      /* padding cells for binary compatibility */
    
    /* additional loadlists for p4_initialize_system */
    void ** loadlist[4];   
};

#define PFE (*p4TH)

#ifndef P4_MOPTRS
#define P4_MOPTRS 128
#endif

#define P4_MEM_SLOT (P4_MOPTRS-1)
#define P4_FIG_SLOT (P4_MOPTRS-3)

   /* there's nothing good in this solution... *FIXME*/
#define PFE_FIG (PFE.p[P4_FIG_SLOT]) 
#define PFE_MEM (PFE.p[P4_MEM_SLOT])

struct p4_Thread
{
    void* p[P4_MOPTRS];
    /* p4_cleanup() will automatically free this field, so the base
       memory *must* be the last pointer in the field.
    */
    p4char *dp;			/* actual top of the dictionary */

    p4char* dict;		/*  dictionary */
    p4char* dictlimit;
    p4cell* stack;		/*  data stack */
    p4cell* s0;
    double *fstack;		/*  floating point stack */
    double* f0;
    p4xt** rstack;		/*  return stack */
    p4xt**  r0;
    char (*pockets)[P4_POCKET_SIZE];	/*  POCKET for interactive S" */
    char *tib;			/*  TIB */
    char* tib_end;
    char* history;		/*  command line history buffer */
    char* history_top;
    p4_File* files;		/*  files */
    p4_File* files_top;

/* VM */
    p4xt* ip; /* the intruction pointer */
    p4xt  wp; /* speed up the inner interpreter */
    p4cell* sp; /* the stack pointer */
    p4xt**  rp; /* the return stack pointer */
    p4cell* lp; /* the pointer to local variables */
    double* fp; /* the floating point stack pointer */

/* jmp_buf */
    jmp_buf loop;          /* QUIT and ABORT do a THROW which longjmp() */
       			   /* here thus C-stack gets cleaned up too */
/*Options*/
    int nr;                 /* this cpu's id (mostly an optional feature)*/
    p4_Session* set;        /* contains cpu-pointers */
#define P4_opt  (*PFE.set)
#define PFE_set (*PFE.set)
    
/*Dict*/
    p4char *fence;		/* can't forget below that address */
    char *last;			/* NFA of most recently CREATEd header */

    p4_Wordl *voc_link;		/* link to chained word lists */
    p4_Wordl *context[P4_ORDER_LEN];	/* dictionary search order */
    p4_Wordl *only;		/* ONLY is always searched */
    p4_Wordl *current;		/* points to vocabulary in extension */
    p4_Wordl *dforder[P4_ORDER_LEN];	/* default dictionary search order */
    p4xt application;		/* word to run initially or 0 */
    char *hld;			/* auxiliary pointer for number output */
    p4cell dpl;			/* position of input decimal point */

    p4_Input input;		/* active input source, full specification */
    p4_Input input_err;		/* input specification when error occurred */
    p4_Iframe *saved_input;	/* links to chain of saved input specs */
    p4_Except *cAtch;		/* links to chain of CATCHed words */
    p4ucell span;		/* char count of last EXPECT */
    p4ucell scr;		/* latest LISTed block number */
    p4cell out;			/* current output column on screen */
    p4cell state;		/* interpreting (0) or compiling (-1) */
    p4cell *locals;		/* number of locals in current def. */
    char (*local)[P4_LOCALS];	/* names of locals in current def. */
    p4cell *csp;		/* compiler security, saves sp here */
    p4ucell base;		/* of number i/o conversion */
    p4cell precision;		/* floating point output precision */

/*Forth*/
#if defined SYS_AIX3 && !defined(__GNUC__)
    /* avoid "internal compiler error" from AIX 3.2 cc compiler */
    void *key;			/* executed by KEY */
    void *emit;			/* executed by EMIT */
    void *expect;		/* executed by EXPECT */
    void *type;			/* executed by TYPE */
#else
    p4xt key;			/* executed by KEY */
    p4xt emit;			/* executed by EMIT */
    p4xt expect;		/* executed by EXPECT */
    p4xt type;			/* executed by TYPE */
#endif
    p4cell wordl_flag;		/* do toupper() before dictionary search */
    p4cell lower_case_fn;	/* do tolower() on file names */
    p4cell redefined_msg;	/* no `"xxx" is redefined' msg if false */
    p4cell float_input;		/* don't try floating pt input when false */
    p4cell reset_order;		/* if true: reset search order on ABORT */
    p4cell pocket;		/* which pocket to use next */

    p4_File *stdIn;		/* C-library standard files */
    p4_File *stdOut;		/* mapped to Forth-files */
    p4_File *stdErr;
    p4ucell more;		/* for a more-like effect */
    p4ucell lines;
    
    struct lined accept_lined;	/* better input-facilities for accep := 0*/
    p4xt  fkey_xt[10];		/* fkey_executes_xt := 0*/
    void (*execute)(p4xt);	/* := normal_execute */

/* core.c */     
    p4code semicolon_code;      /* the code to run at next semicolon */

/* main-sub / dict-sub */
    int exitcode;
    void (*system_terminal)();
    void (*atexit_cleanup)();   /* free this thread */
    int atexit_running;         /* p4_atexit_cleanup called... */
    p4_Wordl *atexit_wl;	/* atexit dictionary holder */
    char* volatile forget_dp;   /* temporary of forget */

/* term.h */ 
    int rows, cols;		/* size of text screen */
    int xmax, ymax;             /* size of graphics window in pixels */
    
    p4char keybuf [8];
    p4char* keyptr;
    
/* term*.c */
    void* priv;         	/* private term area, better be also in p[] */
    p4_term_struct* term; 
    char** rawkey_string; 	/* pointer to terminal escape sequences */
    char** control_string; 	/* pointer to terminal control sequences */
                        	/* as used by termunix.c */
    int (*wait_for_stdin)();
    
    void (*on_stop) (void);     /* = p4_system_terminal; */
    void (*on_continue) (void); /* = p4_interactive_terminal; */
    void (*on_winchg) (void);   /* = p4_query_winsize; */
    void (*on_sigalrm) (void);  /* really from signal.c */
    
/* debug.c */
    int debugging;
    int level;
    int maxlevel;
    long opcounter ;
    
/* yours.c */
    p4xt (*smart_char)(char c);

/* support.c/xception */
    p4code throw_cleanup;

/* mapfile variables */
    int    mapfile_fd;

/* p4_query hook */
    int (*query_hook)(int);
    
/* main-mmap -> main-sub */
    int moptrs;

/* new forth-wordlist mechanism */
    p4_Wordl* forth_wl;

/* spy_on/spy_off callback mechanism */
    p4code spy_nest;

/* dstrings suppport (ifdef WITH_DSTRINGS): */
    char* dstrings;

/* environ-wl support */
    p4_Wordl* environ_wl;

/* vectorized p4_interpret support */
    p4ucell_p4code interpret [8];

/* make updates safer with additional padding space, use it! */
    p4cell padding[16];
};

# define p4_S0 PFE.s0
# define p4_F0 PFE.f0
# define p4_R0 PFE.r0

# define p4_DP          (PFE.dp)
# define p4_HLD		(PFE.hld)
# define p4_DPL		(PFE.dpl)
# define p4_PAD		((char *)p4_DP + P4_MIN_HOLD)
# define p4_FENCE	(PFE.fence)
# define p4_LAST	(PFE.last)
# define p4_VOC_LINK	(PFE.voc_link)
# define p4_CONTEXT	(PFE.context)
# define p4_DFORDER     (PFE.dforder)
# define p4_ONLY	(PFE.only)
# define p4_CURRENT	(PFE.current)
# define p4_APPLICATION	(PFE.application)

#ifdef _P4_SOURCE
# define DP		p4_DP
# define HLD		p4_HLD
# define DPL		p4_DPL
# define PAD		p4_PAD
# define FENCE		p4_FENCE
# define LAST		p4_LAST
# define VOC_LINK	p4_VOC_LINK
# define CONTEXT	p4_CONTEXT
# define DEFAULT_ORDER	p4_DFORDER
# define ONLY		p4_ONLY
# define CURRENT	p4_CURRENT
# define APPLICATION	p4_APPLICATION
#endif

# define p4_SOURCE_ID	(PFE.input.source_id)
# define p4_SOURCE_FILE	((p4_File*) SOURCE_ID)
# define p4_BLOCK_FILE	(PFE.input.block_file)
# define p4_BLK		(PFE.input.blk)
# define p4_TIB		(PFE.input.tib)
# define p4_NUMBER_TIB	(PFE.input.number_tib)
# define p4_TO_IN	(PFE.input.to_in)

#ifdef _P4_SOURCE
# define SOURCE_ID	p4_SOURCE_ID
# define SOURCE_FILE	p4_SOURCE_FILE
# define BLOCK_FILE	p4_BLOCK_FILE
# define BLK		p4_BLK
# define TIB		p4_TIB
# define NUMBER_TIB	p4_NUMBER_TIB
# define TO_IN		p4_TO_IN
#endif

# define p4_SPAN		(PFE.span)
# define p4_SCR			(PFE.scr)
# define p4_OUT			(PFE.out)
# define p4_STATE		(PFE.state)
# define p4_CSP			(PFE.csp)
# define p4_BASE		(PFE.base)
# define p4_PRECISION		(PFE.precision)
# define p4_WORDL_FLAG		(PFE.wordl_flag)
# define p4_LOWER_CASE		(PFE.wordl_flag & WORDL_NOCASE)
# define p4_LOWER_CASE_FN	(PFE.lower_case_fn)
# define p4_REDEFINED_MSG	(PFE.redefined_msg)
# define p4_FLOAT_INPUT		(PFE.float_input)
# define p4_RESET_ORDER		(PFE.reset_order)

#ifdef _P4_SOURCE
# define SPAN		p4_SPAN
# define SCR		p4_SCR
# define OUT		p4_OUT
# define STATE		p4_STATE
# define CSP		p4_CSP
# define BASE		p4_BASE
# define PRECISION	p4_PRECISION
# define WORDL_FLAG	p4_WORDL_FLAG
# define LOWER_CASE	p4_LOWER_CASE
# define LOWER_CASE_FN	p4_LOWER_CASE_FN
# define REDEFINED_MSG	p4_REDEFINED_MSG
# define FLOAT_INPUT	p4_FLOAT_INPUT
# define RESET_ORDER	p4_RESET_ORDER
#endif

#ifdef _P4_SOURCE
typedef p4_Head 	Head;
typedef p4_Wordl 	Wordl;
typedef p4_File 	File;
typedef p4_Dict		Dict;
typedef p4_Input	Input;
typedef p4_Iframe	Iframe;
typedef p4_Except	Except;
#endif

#ifdef _P4_SOURCE
enum
{
    FMODE_RO = 1, FMODE_WO, FMODE_RW,
    FMODE_ROB, FMODE_WOB, FMODE_RWB
};

# define FMODE_BIN (FMODE_ROB - FMODE_RO)
#endif

/*@}*/
#endif
