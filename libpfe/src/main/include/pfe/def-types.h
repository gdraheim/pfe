#ifndef __PFE_DEF_TYPES_H
#define __PFE_DEF_TYPES_H
/**
 * -- pfe's data structures
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2003.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author$)
 *  @version $Revision$
 *     (modified $Date$)
 *
 *  @description
 *     p4th's forth data structures definitions, types and sys.names
 *
 */
/*@{*/

#include <pfe/def-config.h>
#include <pfe/def-regs.h>

/* typedef FILE */
#include <stdio.h>
#include <pfe/os-setjmp.h>
#include <pfe/os-fesetenv.h>

#if defined HAVE_SYS_TYPES_H || defined PFE_HAVE_SYS_TYPES_H
# include <sys/types.h>		/* size_t, time_t and friends */
#endif

#include <pfe/def-macro.h>
#include <pfe/def-paths.h>

#include <pfe/lined.h>

/* options section */

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

# define LD_THREADS    	P4_THREADS_SHIFT
# define LD_LOCALS     	P4_LOCALS_SHIFT

# define MAX_FILES	P4_MAX_FILES
# define POCKETS	P4_POCKETS
# define POCKET_SIZE	P4_POCKET_SIZE

# define MIN_HOLD	P4_MIN_HOLD
# define MIN_PAD	P4_MIN_PAD

# define MAX_LOCALS	P4_LOCALS
# define THREADS	P4_THREADS

# define TIB_SIZE     	P4_TIB_SIZE
# define BPBUF  	P4_BLK_SIZE
# define P4_BPBUF  	P4_BLK_SIZE

#endif

typedef p4char p4_namechar_t;      /* word list name char */
typedef p4char p4_namebuf_t;       /* start of counted string with namechars */
typedef p4char p4_charbuf_t;       /* start of counted string with i/o chars */
/*   */        /* char */           /* i/o char of Standard C/C++ (compiler) */

typedef p4cell  (*p4cell_p4code) (void); /* very useful sometimes */
typedef p4ucell (*p4ucell_p4code) (void); /* very useful sometimes */
typedef p4ucell p4_blk_t;          /* for block-ext support */
#define P4_INVALID_BLK UINT_MAX

typedef struct p4_Wordl  p4_Wordl;  /* body of a word list */
typedef struct p4_File 	 p4_File;   /* neccessary information about open files */
typedef struct p4_Dict 	 p4_Dict;   /* a dictionary specification */
typedef struct p4_Input  p4_Input;  /* an input source specification */
typedef struct p4_Iframe p4_Iframe; /* a frame to save an input specification */
typedef struct p4_Except p4_Except; /* an exception frame */

typedef p4char* pfe_lfa_t;
typedef p4code  pfe_cfa_t;
typedef int (*p4_decompile_func_t)(p4_namebuf_t* nfa, p4xt xt);

typedef int (*p4_setjmp_fenv_save_func_t)(p4_fenv_t*); /* uses fegetenv(fenv_t*) */
typedef int (*p4_setjmp_fenv_load_func_t)(p4_fenv_t*); /* uses fesetenv(const fenv_t*) */

typedef struct { p4_byte_t buffer[P4_POCKET_SIZE]; } p4_pocket_t;

struct p4_Wordl			/* a word list */
{				/* a word list identifier is a (Wordl *) */
    p4_namebuf_t* thread[P4_THREADS]; /* field of ptrs to first NFA of chain */
    p4_Wordl *prev;		/* word lists are chained */
    p4_Wordl *also;		/* search also this dictionary, kinda FIG*/
    p4_namebuf_t* nfa;          /* corresponding vocabulary NFA */
    p4cell  flag;		/* do to_upper, a copy of sys.world_flag */
#define WORDL_NOCASE     (1<<0) /* FIND will match any case with any case */
#define WORDL_NOHASH     (1<<1) /* WORDLIST not hashed into multiple buckets */
#define WORDL_CURRENT    (1<<2) /* on new WORDLIST, make it chain to CURRENT */
#define WORDL_UPPER_CASE (1<<3) /* FIND will match uppercased entries too */
#define WORDL_UPPER_DEFS (1<<4) /* new definitions are placed uppercase */
    p4ucell id;                 /* used to speed up order search (p4_find) */
};

/* FIXME: p4_File.n should be p4_File.block.num or p4_File.file.line */
/* and similar prefix all block/file items using a local struct wrapper */

struct p4_File			/* describes a file */
{
    FILE *f;			   /* associated ANSI-C file structure */
    char mdstr[4];		   /* mode string for fopen() */
    char mode;			   /* mode code for open_file() */
    signed char last_op;	   /* -1 write, 0 none, 1 read */
    p4word len;			   /* if stream: length of input line */
    p4_blk_t blkcnt;	   /* if block file: size of file in blocks */
    p4_blk_t blk;		   /* block in buffer or source line */
    p4cell updated;		   /* if block file: block updated? */
    union {
    	_p4_off_t pos;	   /* saved position, e.g. beginning of line */
    	char compat[8];
    } line;
    char name[PATH_LENGTH];	   /* file name */
    p4_byte_t buffer[P4_BLK_SIZE]; /* buffer for block or input line */
};

struct p4_Input			/* an input source specification */
{
    p4cell source_id;		/* SOURCE-ID */
    p4_File *block_file;	/* which file is active? */
    p4ucell blk;		/* currently loaded block */
    union { _p4_off_t pos;	/* stream file: beginning of line in file */
        char compat[8]; } line;
    const p4_char_t *tib;	/* points to TIB or to EVALUATED string */
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
    p4xcode** rpp;              /* P4_REGRP_T */
    p4xcode *ipp;               /* P4_REGIP_T */
    p4cell *spp;                /* P4_REGSP_T */
    p4cell *lpp;                /* P4_REGLP_T */
    double *fpp;                /* P4_REGFP_T */
    p4_Iframe *iframe;
    p4_jmp_buf jmp;
    p4_fenv_t  jmp_fenv;
    p4_Except *prev;
};

typedef struct p4_Exception p4_Exception;
struct p4_Exception
{
    struct p4_Exception* next;
    p4cell id;
    char const * name;
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
typedef struct _p4_term_struct  p4_term_struct;

typedef struct p4_Dictionary    p4_Dictionary;

struct p4_Dictionary
{
    p4_namebuf_t* last;  /* PFE.last -> PFE.dict.last */
    p4_namebuf_t* link;  /* PFE.link -> PFE.dict.link */
    p4_byte_t*    here;  /* PFE.dp   -> PFE.dict.here */
    p4_byte_t*    base;  /* PFE.dict -> PFE.dict.base */
    p4_byte_t*    limit; /* PFE.dictlimit -> PFE.dict.limit */
};


#define P4_TTY_ISPIPE 1 /* filter mode: standard input is not a tty */
#define P4_TTY_NOECHO 2 /* noecho mode: standard output is not a tty */

struct p4_Session
{
    int argc;
    char const ** argv;
    unsigned    isnotatty:2,	/* running in canonical mode */
                stdio:1,	/* standard input isn't-tty: work as filter */
                caps_on:1,	/* exchange lower and upper case chars */
                find_any_case:1,/* make case-insensitive find default */
                lower_case_fn:1,/* convert file names to lower case? */
                float_input:1,	/* disables floating point input when false */
                license:1,	/* show license string at startup */
                warranty:1,	/* show warranty string at startup */
                quiet:1,	/* no messages */
                verbose:1,	/* more messages */
                debug:1,	/* enable a few more outputs */
                bye:1,          /* don't enter the mainloop (non-interact..) */
                upper_case_on:1,/* make lower case words find upper case */
                lastbit:1;      /* last bit */
    int	cols, rows;	/* size of screen */
    p4ucelll	total_size;
    p4ucelll	stack_size;
    p4ucelll	ret_stack_size;
    p4ucelll	unused_max_files; /* use option_value "#files" */
    p4ucelll	unused_pockets;   /* use option_value "#pockets" */
    /* quick access to option-ext variables... (if any) */
    char**      unused_boot_include; /* use option_string "SCRIPT-FILE" */
    /* we keep the following entries only to speed up runtime processing: */
    char const** inc_paths;     /* usually points to "INC-PATH" */
    char const** inc_ext;       /* usually points to "INC-EXT" */
    char const** blk_paths;     /* usually points to "BLK-PATH" */
    char const** blk_ext;       /* usually points to "BLK-EXT" */
    char const** lib_paths;     /* usually points to "LIB-PATH" */
    /*                 _editor; // use option_string("$EDITOR") */

    /* for VMs */
    int     cpus;          /* how many cpus do we have in this tread */

    char const** unused_prefix;      /* use option_string("PREFIX-DIR") */
    char const** boot_name;          /* points to argv[0] usually... */
    char const** unused_bootcommand; /* use option_string("BOOT-INIT") */
    char const** optv;
    p4cell       unused_heap;        /* obsoleted bitfield */

    p4ucell     optc;
    unsigned    wordlists;       /* p4ucell might be 64bit (16bit is okay) */
    void*       modules;         /* p4Words* : dl-internal / dl-ext */
    p4ucell     padding[4];      /* padding cells for binary compatibility */

    /* additional loadlists for p4_initialize_system */
    void * loadlist[4];

    /* newstyle options support via option-ext */
    struct
    {
        p4_namebuf_t* last;
        p4_namebuf_t* dp;
        p4_byte_t* dict;
        p4_byte_t* dictlimit;
        p4_byte_t  space[284]; /* atleast a few headers... */
    } opt; /* must be last in this structure !! */
};

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
	/* VM */
	p4xcode*   ip; /* the intruction pointer */
	p4xt       wp; /* speed up the inner interpreter */
	p4cell*    sp; /* the stack pointer */
	p4xcode**  rp; /* the return stack pointer */
	p4cell*    lp; /* the pointer to local variables */
	double*   fp; /* the floating point stack pointer */
	/* MODULES */
    void* p[P4_MOPTRS];
    /* p4_cleanup() will automatically free this field, so the base
       memory *must* be the last pointer in the field.
    */
    p4_byte_t *dp;		/* actual top of the dictionary */

    p4_byte_t* dict;		/*  dictionary */
    p4_byte_t* dictlimit;
    p4cell* stack;		/*  data stack */
    p4cell* s0;
    double *fstack;		/*  floating point stack */
    double* f0;
    p4xcode** rstack;		/*  return stack */
    p4xcode**  r0;
    p4_pocket_t* pockets_ptr;	/*  POCKET for interactive S" */
    p4_char_t *tib;		/*  TIB terminal input buffer */
    p4_char_t* tib_end;
    char* history;		/*  command line history buffer */
    char* history_top;
    p4_File* files;		/*  files */
    p4_File* files_top;


/* jmp_buf */
    p4_jmp_buf loop;       /* QUIT and ABORT do a THROW which longjmp() */
                                  /* here thus C-stack gets cleaned up too */
/*Options*/
    int nr;                 /* this cpu's id (mostly an optional feature)*/
    p4_Session* set;        /* contains cpu-pointers */
#define P4_opt  (*PFE.set)
#define PFE_set (*PFE.set)

/*Dict*/
    p4_byte_t *fence;		/* can't forget below that address */
    p4_namebuf_t *last;		/* NFA of most recently CREATEd header */

    p4_Wordl *voc_link;		/* link to chained word lists */
    p4_Wordl **context;	        /* dictionary search order */
    p4_Wordl *only__;		/* ONLY is always searched OBSOLETE */
    p4_Wordl *current;		/* points to vocabulary in extension */
    p4_Wordl **dforder;	        /* default dictionary search order */
    p4_Wordl *dfcurrent;        /* default definition wordlist */
    p4_char_t *hld;		/* auxiliary pointer for number output */
    p4cell dpl;			/* position of input decimal point */

    p4_Input input;		/* active input source, full specification */
    p4_Input input_err;		/* input specification when error occurred */
    p4_Iframe *saved_input;	/* links to chain of saved input specs */
    p4_Except *catchframe;	/* links to chain of CATCHed words */
    p4ucell span;		/* char count of last EXPECT */
    p4ucell scr;		/* latest LISTed block number */
    p4cell out;			/* current output column on screen */
    p4cell state;		/* interpreting (0) or compiling (-1) */
    p4cell *locals;		/* number of locals in current def. */
    char (*local)[P4_LOCALS];   /* names of locals in current def. */
    p4cell *csp;		/* compiler security, saves sp here */
    p4ucell base;		/* of number i/o conversion */
    p4cell precision;		/* floating point output precision */

/*Forth*/
#if defined HOST_OS_AIX3 && !defined(__GNUC__)
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
    p4_pocket_t* pocket;	/* which pocket to use next */

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
    void (*system_terminal)(void);
    void (*atexit_cleanup)(void);    /* free this thread */
    int atexit_running;              /* p4_atexit_cleanup called... */
    p4_Wordl *atexit_wl;	     /* atexit dictionary holder */
    p4_byte_t* volatile forget_dp;   /* temporary of forget */

/* term.h */
    int rows, cols;		/* size of text screen */
    int xmax, ymax;             /* size of graphics window in pixels */

    p4_char_t keybuf [8];
    p4_char_t* keyptr;

/* term*.c */
    void* priv;         	/* private term area, better be also in p[] */
    p4_term_struct* term;
    char const ** rawkey_string;  /* pointer to terminal escape sequences */
    char const ** control_string; /* pointer to terminal control sequences */
                                /* as used by termunix.c */
    int (*wait_for_stdin)(void);

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
/*  int (*query_hook)(int); // please use lined.h:lined->intercept */

/* main-mmap -> main-sub */
    int moptrs;

/* new forth-wordlist mechanism */
    p4_Wordl* forth_wl;

/* spy_on/spy_off callback mechanism */
    void (*spy_nest)(int);

/* dstrings suppport (ifdef WITH_DSTRINGS): */
    char* dstrings;

/* environ-wl support */
    p4_Wordl* environ_wl;

/* vectorized p4_interpret support */
    p4ucell_p4code interpret [8];

/* vectorized abort_system support */
    p4code abort [4];

    p4cell next_exception;
    p4_Exception* exception_link;

/* vectorized p4_decompile - just a hack that needs to be replaced later */
    p4_decompile_func_t decompile[3];

/* go at quoting... */
    p4_char_t quoted_parse;
    p4_char_t unused_flag1;
    p4_char_t unused_flag2;
    p4_char_t unused_flag3;

    struct {
        const p4_char_t* ptr;
        unsigned len;          /* p4ucell is 8byte on x86_64 but */
    } word;                    /* parsing is not exceeding 16bit anyway */

    p4xt application;		/* word to run initially or 0 */

    p4_byte_t* last_here;      /* set in interpret and used in abort */

    p4xt spy_enter;            /* see with-spy.c */
    p4xt spy_leave;            /* see with-spy.c */

    void* chain_link;          /* see chain-ext.c */
    p4_Wordl* abort_wl;        /* see engine-sub/chainlist-ext REDO-WL */
    p4_Wordl* prompt_wl;       /* see engine-sub/chainlist-ext DO-WL */

    double asinh_MAX_over_4;          /* see complex-ext.c */
    double sqrt_MAX_over_4;           /* see complex-ext.c */

    p4_pocket_t* pockets_top;  /* fixme: should be moved to above... */

/* stackhelp-ext.c */
    p4_Wordl* stackhelp_wl;
/* tools-ext.c + assembler-ext.c */
    p4_Wordl* assembler_wl;

    p4xt    interpret_loop;       /* compiled interpret loop */
    p4cell  interpret_compiled;   /* use it! */
    p4cell* interpret_compile_resolve;
    p4cell* interpret_compile_extra;
    p4cell* interpret_compile_float;

/* os-fesetenv.h + floating-ext.c */
    p4_setjmp_fenv_save_func_t   setjmp_fenv_save;
    p4_setjmp_fenv_load_func_t   setjmp_fenv_load;
    p4_fenv_t loop_fenv;

/* make updates safer with additional padding space, use it! */
    p4cell padding[3];
};

# define p4_S0 PFE.s0
# define p4_F0 PFE.f0
# define p4_R0 PFE.r0

#define p4_DICT_LAST    (PFE.last)
#define p4_DICT_HERE    (PFE.dp)
#define p4_DICT_BASE    (PFE.dict)
#define p4_DICT_LIMIT   (PFE.dictlimit)

# define p4_DP          (PFE.dp)
# define p4_HERE        (PFE.dp)
# define p4_HLD		(PFE.hld)
# define p4_DPL		(PFE.dpl)
# define p4_PAD		((p4_char_t *)p4_DP + P4_MIN_HOLD)
# define p4_FENCE	(PFE.fence)
# define p4_LAST	(PFE.last)
# define p4_VOC_LINK	(PFE.voc_link)
# define p4_CONTEXT	(PFE.context)
# define p4_DFORDER     (PFE.dforder)
# define p4_DFCURRENT   (PFE.dfcurrent)
# define p4_ONLY	(PFE.context[PFE_set.wordlists])
# define p4_CURRENT	(PFE.current)
# define p4_APPLICATION	(PFE.application)

#ifdef _P4_SOURCE
# define DICT_LAST     p4_DICT_LAST
# define DICT_HERE     p4_DICT_HERE
# define DICT_BASE     p4_DICT_BASE
# define DICT_LIMIT    p4_DICT_LIMIT
# define DP		    p4_DP
# define HERE		    p4_HERE
# define HLD		    p4_HLD
# define DPL		    p4_DPL
# define PAD		    p4_PAD
# define FENCE		    p4_FENCE
# define LAST		    p4_LAST
# define VOC_LINK	    p4_VOC_LINK
# define CONTEXT	    p4_CONTEXT
# define DEFAULT_ORDER	p4_DFORDER
# define ONLY		    p4_ONLY
# define CURRENT	    p4_CURRENT
# define APPLICATION	p4_APPLICATION
#endif

/* use as p4_setjmp_fenv_save(& thread->loop_fenv) */
#if defined P4_NO_FP
# define p4_setjmp_fenv_save(buffer)
# define p4_setjmp_fenv_load(buffer)
#else
# define p4_setjmp_fenv_save(buffer) PFE.setjmp_fenv_save(buffer)
# define p4_setjmp_fenv_load(buffer) PFE.setjmp_fenv_load(buffer)
#endif

# define p4_DP_CHAR     p4_DP
# define p4_DP_CELL     ((p4cell*)(p4_DP))

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

# define P4_UPPER_CASE_FLAGS (WORDL_NOCASE|WORDL_UPPER_CASE|WORDL_UPPER_DEFS)

# define p4_OUT			(PFE.out)
# define p4_COLS                (PFE.cols)
# define p4_SPAN		(PFE.span)

# define p4_SCR			(PFE.scr)
# define p4_STATE		(PFE.state)
# define p4_CSP			(PFE.csp)
# define p4_BASE		(PFE.base)
# define p4_PRECISION		(PFE.precision)
# define p4_WORDL_FLAG		(PFE.wordl_flag)
# define p4_UPPER_CASE		(PFE.wordl_flag & P4_UPPER_CASE_FLAGS)
# define p4_LOWER_CASE		(PFE.wordl_flag & WORDL_NOCASE) /*depracated*/
# define p4_LOWER_CASE_FN	(PFE.lower_case_fn)
# define p4_REDEFINED_MSG	(PFE.redefined_msg)
# define p4_FLOAT_INPUT		(PFE.float_input)
# define p4_RESET_ORDER		(PFE.reset_order)

# if PFE_USE_QUOTED_PARSE
# define p4_QUOTED_PARSE        (PFE.quoted_parse)
# else
# define p4_QUOTED_PARSE        0
#endif

#ifdef _P4_SOURCE
# define SCR		p4_SCR
# define STATE		p4_STATE
# define CSP		p4_CSP
# define BASE		p4_BASE
# define PRECISION	p4_PRECISION
# define WORDL_FLAG	p4_WORDL_FLAG
# define UPPER_CASE	p4_UPPER_CASE
# define LOWER_CASE	p4_LOWER_CASE
# define LOWER_CASE_FN	p4_LOWER_CASE_FN
# define REDEFINED_MSG	p4_REDEFINED_MSG
# define FLOAT_INPUT	p4_FLOAT_INPUT
# define RESET_ORDER	p4_RESET_ORDER
#endif

#ifdef _P4_SOURCE
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

#ifndef PFE_MINIMAL_UNUSED
#define PFE_MINIMAL_UNUSED 256
#endif

/*@}*/
#endif
