/** 
 * -- Process command line, get memory and start up.
 * 
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version: 5.22 %
 *    (%date_modified: Mon Mar 12 10:32:30 2001 %)
 *
 *  @description
 *  Process command line, get memory and start up the interpret loop of PFE
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: main-sub.c,v 0.30 2001-03-15 10:30:44 guidod Exp $";
#endif

#define	_P4_SOURCE 1


#include <pfe/pfe-base.h>
#include <pfe/def-xtra.h>
 
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#ifndef P4_NO_FP
#include <float.h>
#endif
#include <errno.h>
#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif
#include <unistd.h>

#include <pfe/term-sub.h>
#include <pfe/version-sub.h>
#include <pfe/exception-sub.h>
#include <pfe/lined.h>
#include <pfe/_nonansi.h>
#include <pfe/_missing.h>

#ifdef PFE_WITH_DSTRINGS_EXT    /* (dnw 3feb01) */
# include <pfe/dstrings-ext.h>
#endif

#include <pfe/logging.h>

#ifndef _export
#define _export
# include <pfe/incl-sub.h>
# include <pfe/def-types.h>

# define p4_GetOptions     p4_SetOptions
# define p4_DefaultSession p4_SetOptionsDefault
  /* and a few internals */
# define _p4_thread_save() (p4_thread_save(p4_main_threadP))
# define _p4_thread_load() (p4_thread_load(p4_main_threadP))

# ifdef P4_REGTH
# define _p4_thread_local() (p4TH = p4_main_threadP)
# define _p4_thread_ENTER_(arg_threadP) \
   	register p4_threadP _p4_thread_SAVED_ = p4TH; \
                        	p4TH = arg_threadP;
# define _p4_thread_LEAVE_() p4TH = _p4_thread_SAVED_;
# else
# define _p4_thread_local()
# define _p4_thread_ENTER_(ignored)
# define _p4_thread_LEAVE_()
# endif
#endif

/************************************************************************/
/* physical instance of the global system variable:                     */
/************************************************************************/

#ifndef P4_REGTH
p4_threadP p4TH;
#endif

/** two helper functions (thread_save and thead_load)
 * that can be called from non-pfe compiled context
 */
_export void 
p4_thread_save (p4_threadP p)
{
# ifdef P4_REGIP
    p->ip = p4IP;
# endif
  
# ifdef P4_REGW
    p->wp = p4WP;
# endif
  
# ifdef P4_REGSP
    p->sp = p4SP;
# endif
  
# ifdef P4_REGRP
    p->rp = p4RP;
# endif
  
# ifdef P4_REGLP
    p->lp = p4LP;
# endif

# ifndef P4_NO_FP  
# ifdef P4_REGFP
    p->fp = p4FP;
# endif
# endif
}

_export void 
p4_thread_load (p4_threadP p)
{
    p4TH = p; /* either a global variable or a register */

# ifdef P4_REGIP
    p4IP = p->ip;
# endif
  
# ifdef P4_REGW
    p4WP = p->wp;
# endif
  
# ifdef P4_REGSP
    p4SP = p->sp;
# endif
  
# ifdef P4_REGRP
    p4RP = p->rp;
# endif
  
# ifdef P4_REGLP
    p4LP = p->lp;
# endif

# ifndef P4_NO_FP  
# ifdef P4_REGFP
    p4FP = p->fp;
# endif
# endif
}

typedef struct 
{
    p4xt* ip;    /* the instruction pointer */
    p4xt wp;     /* the word pointer */
    p4cell* sp;  /* the parameter stack */
    p4xt** rp;   /* the return stack */
    p4cell* lp;  /* the locals pointer */
    double* fp;  /* the floating stack */
    p4_threadP th; /* the thread pointer */
} p4_thread_VM;

static inline void p4_thread_save_VM (p4_thread_VM* p)
{
# ifdef P4_REGTH
    p->th = p4TH;
# endif
  
# ifdef P4_REGIP
    p->ip = p4IP;
# endif
  
# ifdef P4_REGW
    p->wp = p4WP;
# endif
  
# ifdef P4_REGSP
    p->sp = p4SP;
# endif
  
# ifdef P4_REGRP
    p->rp = p4RP;
# endif
  
# ifdef P4_REGLP
    p->lp = p4LP;
# endif

# ifndef P4_NO_FP  
# ifdef P4_REGFP
    p->fp = p4FP;
# endif
# endif
}

static inline void p4_thread_load_VM (p4_thread_VM* p)
{
# ifdef P4_REGTH
    p4TH = p->th;
# endif

# ifdef P4_REGIP
    p4IP = p->ip;
# endif
  
# ifdef P4_REGW
    p4WP = p->wp;
# endif
  
# ifdef P4_REGSP
    p4SP = p->sp;
# endif
  
# ifdef P4_REGRP
    p4RP = p->rp;
# endif
  
# ifdef P4_REGLP
    p4LP = p->lp;
# endif

# ifndef P4_NO_FP  
# ifdef P4_REGFP
    p4FP = p->fp;
# endif
# endif
}


/************************************************************************/
/* Analyze command line options:                                        */
/************************************************************************/

#ifndef EDITOR                  /* USER-CONFIG: */
#define	EDITOR		"emacs"	/* preferred editor for text files, */
#endif				/* env-variable EDITOR overrides this */

#ifndef CAPS_ON                 /* USER-CONFIG: */
#define	CAPS_ON		0	/* do you like (faked) caps lock by default? */
#endif
#ifndef LOWER_CASE_ON           /* USER-CONFIG: */
#define	LOWER_CASE_ON	1	/* allow "dup" to find "DUP" by default */
#endif
#ifndef LWRCASE_FN_ON           /* USER-CONFIG: */
#define LWRCASE_FN_ON	1	/* convert file names to lower case? */
#endif
#ifndef FLOAT_INPUT_ON          /* USER-CONFIG: */
#define FLOAT_INPUT_ON	1	/* allow input of floating point numbers */
#endif

#ifndef TEXT_COLS               /* USER-CONFIG: */
#define	TEXT_COLS	80	/* used only in case p4th fails determining */
#endif
#ifndef TEXT_ROWS               /* USER-CONFIG: */
#define	TEXT_ROWS	25	/* the screen size */
#endif

#define TOTAL_SIZE (P4_KB*1024) /* the shorthand for default-computations */

#ifndef STACK_SIZE              /* USER-CONFIG: */
#define	STACK_SIZE	0	/* 0 -> P4_KB*1024 / 8 */
#endif
#ifndef FLT_STACK_SIZE          /* USER-CONFIG: */
#define	FLT_STACK_SIZE	0	/* 0 -> P4_KB*1024 / 16 */
#endif
#ifndef RET_STACK_SIZE          /* USER-CONFIG: */
#define	RET_STACK_SIZE	0	/* 0 -> P4_KB*1024 / 16 */
#endif

/**
 * fill the session struct with precompiled options
 */
_export void
p4_SetOptionsDefault(p4_sessionP set)
{
    if (set) {
        memset(set, 0, sizeof(*set));

	set->argv = 0;
	set->argc = 0;
        set->bootname = PFE_PACKAGE;
        set->canonical = 0;
        set->stdio = 0;
        set->caps_on = CAPS_ON;
        set->lower_case_on = LOWER_CASE_ON;
        set->lower_case_fn = LWRCASE_FN_ON;
#       ifndef P4_NO_FP
        set->float_input = FLOAT_INPUT_ON;
#       else
        set->float_input = 0;
#       endif
        set->license = 0;
        set->warranty = 0;
        set->quiet = 0;
        set->verbose = 0;
        set->debug = 0;
        set->bye = 0;
        set->cols = TEXT_COLS;
        set->rows = TEXT_ROWS;
        set->total_size = TOTAL_SIZE;
        /* TOTAL_SIZE dependent defaults are moved to dict_allocate */
        set->stack_size = 0;
        set->flt_stack_size = 0;
        set->ret_stack_size = 0;

	set->prefix = PFE_EPREFIX;
        set->max_files = MAX_FILES;
        set->pockets = POCKETS;
        set->block_file = PFE_DEFAULT_BLKFILE;
        set->bootcommand = 0;
        set->include_file = 0;
        set->incpaths = PFE_INC_PATH;
        set->incext = PFE_INC_EXTENSIONS;
        set->blkpaths = PFE_BLK_PATH;
        set->blkext = PFE_BLK_EXTENSIONS;
        set->editor = EDITOR;
        set->cpus = P4_MP;

#      ifdef PFE_WITH_DSTRINGS_EXT    /* (dnw 1feb01) */
        set->str_buffer_size = P4_STR_BUFFER_SIZE;
#      else
	set->str_buffer_size = -1;
#      endif

#      ifndef _K12_SOURCE
	{   /* environment scanning */
	    char* t;
	    /*
	     * get special options from environment variables:
	     */
	    if ((t = getenv ("FORTHINCLUDE")) != NULL)
	    {
		set->incpaths = strdup (t); set->heap.incpaths = 1;
		set->blkpaths = strdup (t); set->heap.blkpaths = 1;
	    }
	    else if ((t = getenv ("PFEINCLUDE")) != NULL)
	    {
		set->incpaths = strdup (t); set->heap.incpaths = 1;
		set->blkpaths = strdup (t); set->heap.blkpaths = 1;
	    }
	    
	    if ((t = getenv ("FORTHEDITOR")) != NULL)
	    {
		set->editor = strdup (t); set->heap.editor = 1;
	    }
	    else if ((t = getenv ("PFEEDITOR")) != NULL)
	    {
		set->editor = strdup (t); set->heap.editor = 1;
	    }
	    else if ((t = getenv ("EDITOR")) != NULL)
	    {
		set->editor = strdup (t); set->heap.editor = 1;
	    }
	    
	    if ((t = getenv ("PFEDIR")) != NULL)
	    {
		set->prefix = strdup (t); set->heap.prefix = 1;
	    }
	}
#      endif
    }
}

/**
 * the help_options table is scanned by help_opt for lines starting
 * with "-". It then tries to match the directly following longoption,
 * ie. the one that starts with the doubled "--" . If it does match,
 * the single char at offset +1 is returned. The help_print routine
 * will only show the strings upto the first length 0 string, so you
 * can have invisible options. This is good for having synonyms of
 * longoptions - just map them to the same shortoption. Note that this
 * optionsystem does not allow shortoptions to be assembled into a
 * single arg-position. Note also that it is a good thing to instruct
 * the package user to only use longoptions since the shortoption
 * vector may change but any old longoptions can be moved to the 
 * invisible section (and have them mapped to the new vector or some
 * shortopt vector that would not print well on a terminal, e.g. \8).
 */
static const char* help_options[] = {
    ">> Usage: %s [-#bcdefhklrsv] [file] [args..]",
    "-C --cpus x           \t number of cpus in this forth",
    "-B --prefix DIR       \t prefix installation path to be used",  
    "-b --blockfile FILE   \t use FILE as block device before boot",
    "-c --caps-lock        \t turn on CAPS lock",
    "-d --load-image FILE  \t reload dictionary image from FILE",
    "-D --make-image FILE  \t build dictionary image and exit",
    "-e --evaluate         \t bootcommand to evaluate",
    "-E --editor NAME      \t NAME of preferred ASCII text editor",
    "-f --max-files        \t maximum N simultaneously open files",
    "-F --lowercase-fn     \t convert file names to lower case",
    "-G --float-input      \t allow input of floating point numbers",
    "-i --bootfile FILE    \t use FILE as forth script inside boot",
    "-k --total-size SIZE  \t SIZE of system in KBytes",
    "-l --lower-case       \t base system's wordlists are case insensitive",
    "-L --license          \t display license",
    "-p --pockets N        \t number of pockets for S\"",
    "-q --quiet            \t suppress signon message",
    "-r --ret-stack SIZE   \t SIZE of return stack in cells",
    "-s --stack SIZE       \t SIZE of stack in cells",
    "-t --flt-stack SIZE   \t SIZE of floating point stack in items",
    "-$ --str-buffer SIZE  \t SIZE of dynamic string buffer in bytes",
    "-T --screen CxR       \t text screen has C/columns and R/rows",
    "-v --verbose          \t verbose",
    "-V --version          \t version string",
    "-W --warranty         \t display warranty. Of course: Absolutely none.",
    "-y --bye              \t non-interactive, exit after running file",
    "-? --help             \t display this message and exit",
    " * Turn option off by appending \"-\" to the letter.",
    " * The given file is loaded initially.",
    " * extra options: ",
    "-I  --path <path>     \t add to search path",
    "-A  --mapbase <address> \t map the dictionary with this base",
    "-M  --mapfile <file>  \t map a file as dictionary while running",
    "-Q  --dumpfile <file> \t where to dump the dictionary on exit",
    "-P  --pipe            \t run in pipe, just read from stdio",
    "-!  --debug           \t start debugging",
    "", /* and some invisible options (usually aliases) */
    "-d  --image-file         gforth' --load-image",
    "-D  --appl-image         gforth' --make-image",
    "-s  --data-stack-size    gforth' --stack",
    "-r  --return-stack-size  gforth' --ret-stack",
    "-t  --fp-stack-size      gforth' --flt-stack",
    "-k  --dictionary-size    gforth' --total-size",
    "-c  --caps               old' --caps-lock",
    0
};


static void
help_print (p4_sessionP set, FILE* f)
{
    const char** p;
    
    if (!f) f = stderr;
    
    fprintf (f, "%s\n%s\n", p4_version_string (), p4_copyright_string ());
    
    for (p = help_options; *p && **p; p++)
    {
        if (**p == '-')  fprintf(stderr, "  "); /* indent the options */
        switch ((*p)[1])
        {
	default:  
	    if ((*p)[1] > ' ') fprintf(f, *p); 
	    else fprintf(f, "  %s", (*p)+2);
	    break;
	case '>': fprintf(f, *p, set->bootname ? set->bootname : "..." ); 
	    break;
	case 'B': fprintf(f, "%s [%s]", *p, set->prefix ? set->prefix : "." );
	    break;
	case 'C': fprintf(f, "%s [%i]", *p, set->cpus); 
	    break;
	case 'c': fprintf(f, "%s [%s]", *p, set->caps_on ? "ON":"OFF"); 
	    break;
	case 'E': fprintf(f, "%s [%s]", *p, set->editor ? set->editor : "." ); 
             break;
	case 'G': fprintf(f, "%s [%s]", *p, set->float_input ? "ON":"OFF"); 
	    break;
	case 'f': fprintf(f, "%s [%d]", *p, (int) set->max_files);
	    break;
	case 'F': fprintf(f, "%s [%s]", *p, set->lower_case_fn ? "ON":"OFF");
	    break;
	case 'k': fprintf(f, "%s [%d K]", *p, (int) set->total_size >> 10);
	    break;
	case 'l': fprintf(f, "%s [%s]", *p, set->lower_case_on ? "ON":"OFF");
             break;
	case 'p': fprintf(f, "%s [%d]", *p, (int) set->pockets);
	    break;
	case 'r': fprintf(f, "%s [%d]", *p, (int) set->ret_stack_size);
	    break;
	case 's': fprintf(f, "%s [%d]", *p, (int) set->stack_size);
	    break;
	case 't': fprintf(f, "%s [%d]", *p, (int) set->flt_stack_size);
	    break;
        case '$': fprintf(f, "%s [%d]", *p, (int) set->str_buffer_size);
            break;
	case 'T': fprintf(f, "%s [%ix%i]", *p, 
			  (int) set->cols, (int) set->rows); 
	    break;
        }
        fprintf(f, "\n");
    }
}

static char
help_opt(const char* str, int l, const char** helptab)
{
    const char** p;
    const char* q;

    if(! str || ! helptab) return 0;

    if (! l) l = strlen(str);
    if (l == 1) return *str;

    for (p=helptab; *p; p++)
    {
        if (**p != '-') continue;
        q = *p; 
        q++; while (*q && *q != '-') q++; while (*q == '-') q++;
        if (strlen (q) > l && !memcmp (q, str, l) && q[l] == ' ')
            return (*p)[1];
    }
    return 0;
}

/**
 * parse the command-line options and put them into the session-structure
 * that is used in thread->set. 
 * returns status code (0 == ok, 1 == normal, 2 == error)
 *
 * note, that these argc/argv are given as references! 
 */ 
_export int
p4_AddOptions (p4_sessionP set, int argc, char* argv[])
{
    int i, optc, flag;		/* count of all options */
    char ** optv;		/* values of these options */
    char *t, *val;

    if (! argc) return 0;

    if (argc && argv[0]) 
	set->bootname = argv[0];

    if (set->argc)
    {
	/* we have already scanned some options */
	optv = malloc (sizeof(char*) * (set->argc + argc));
	if (!optv) return 2;

	memcpy (&optv[0], set->argv, sizeof(char*) * set->argc);
	memcpy (&optv[set->argc], &argv[1], argc-1);
	optv[set->argc + argc - 1] = 0;
	if (set->heap.optv) free (set->optv);
	set->optv = optv; set->heap.optv = 1;
	optc = set->argc + argc - 1;
    }else{
	optv = argv + 1; optc = argc - 1;
    }

    /*
     * process options:
     */
    for (i = set->argc; i < optc; i++)
    {
        register int l, k, s;
        const char* p;

        t = optv[i]; /* scan options up to first (include-)file argument */
        if (*t == '-') { t++; } else { set->include_file = t; i++; break; } 
        if (*t == '-') {
	    t++; if (*t == '-') { i++; break; } /* triple => no scriptfile */
	    if (!*t) { /* double => stopscanning, use next arg as scriptfile */
		i++;  if (i < optc) { set->include_file = optv[i]; i++; } 
		break; }; 
	}

        k = l = strlen(t);
        p = strchr(t, '='); 
        if (p) { k = p-t; } /* length of key */

        s=0; /* skips i - use if val is consumed */
        flag = 1; /* ON - may be switched to OFF here...*/
        if (k == l && t[k-1] == '-') { k--; flag ^= 1; }
        if (l >= 4 && !strcmp (t, "no-")) { t+=3; k-=3; flag ^= 1; val=t+k; }
        else if (k != l) { val = t + k + 1; } /* seperator = or postfix - */
        else if (i == optc - 1) { val = NULL; }
        else { val = optv[i+1]; s=1; }
        
        switch (help_opt(t, k, help_options))
        {
	default:  help_print (set, stderr);  return 2; continue;
	case '?': help_print (set, stdout);  return 1; continue;
	case 'V': fprintf (stdout, "%s\n", p4_version_string ());  
						return 1; continue;
	    /*
	     * Simple flag options can be -x or -x- to turn them off.
	     * these can be combined into a single option.
	     */
	case 'c': set->caps_on = flag;       continue;
	case 'l': set->lower_case_on = flag; continue;
	case 'F': set->lower_case_fn = flag; continue;
	case 'G': set->float_input = flag;   continue;
	case 'L': set->license = flag;	      continue;
	case 'W': set->warranty = flag;      continue;
	case 'q': set->quiet = flag;         continue;
	case 'v': set->verbose = flag;       continue;
	case 'P': set->stdio = flag;         continue;
	case 'y': set->bye = flag;           continue;
	case '!': set->debug = flag;         continue;

             /*
              * Other options have values either following 
              * immediately after the option letter or as 
              * next command line argument:
              */
#       define set__strvar_(VAR) \
	if (set->heap.VAR) free (set->VAR); \
	set->heap.VAR = 0; set->VAR  
	case 'B': set__strvar_(prefix) = val; 	      i+=s; continue;
	case 'b': set__strvar_(block_file) = val;     i+=s; continue;
	case 'i': set__strvar_(boot_file) = val;      i+=s; continue;
	case 'e': set__strvar_(bootcommand) = val;    i+=s; continue;
	case 'E': set__strvar_(editor) = val;         i+=s; continue;
	case 'k': set->total_size = atoi (val) << 10; i+=s; continue;
	case 'p': set->pockets = atoi (val);	      i+=s; continue;
	case 'r': set->ret_stack_size = atoi (val);   i+=s; continue;
	case 's': set->stack_size = atoi (val);       i+=s; continue;
	case 't': set->flt_stack_size = atoi (val);   i+=s; continue;
	case '$': set->str_buffer_size = atoi (val);  i+=s; continue;
	case 'f': set->max_files = atoi (val);
	    if (set->max_files < 4) set->max_files = 4;
	    i+=s; continue;
	case 'T':
	    if (sscanf (val, "%dx%d", &set->cols, &set->rows) != 2)
		set->cols = TEXT_COLS, set->rows = TEXT_ROWS;
	    i+=s; continue;

	case 'I': /* this adds the specified string to the internal string */
	{
	    char* p;
	    static const char delimstr[2] = { PFE_PATH_DELIMITER, '\0' };

	    p = malloc (strlen(set->incpaths) + 1 + strlen(val));
	    if (p) { 
		strcpy (p, set->incpaths);
		strcat (p, delimstr);
		strcat (p, val);
		if (set->heap.incpaths) free (set->incpaths);
		set->incpaths = p; set->heap.incpaths = 1;
	    }

	    p = malloc (strlen(set->blkpaths) + 1 + strlen(val));
	    if (p) {
		strcpy (p, set->blkpaths);
		strcat (p, delimstr);
		strcat (p, val);
		if (set->heap.blkpaths) free (set->blkpaths);
		set->blkpaths = p; set->heap.blkpaths = 1;
	    }
		
	    i+=s; continue;
	}
	case 'C':
	{  
	    register int cpus = atoi(val);
	    if (0 < cpus && cpus <= P4_MP_MAX) set->cpus = cpus;
	    else { 
		P4_fail2 ("cpus=%d invalid (max %d allowed)", 
			  cpus, P4_MP_MAX); 
	    }
	    i+=s; continue;
	}
	case 'Q': set->dumpfile = val;               i+=s; continue;
	case 'M': set->mapfile = val;                i+=s; continue;
	case 'A': set->mapbase = (void*) atol(val);  i+=s; continue;
	case 'd': set->load_image = val;             i+=s; continue;
	case 'D': set->make_image = val;             i+=s; continue;
        }
    }
    
    /*
     * Register remaining options (without included file name) in app_ argc/v:
     */
    set->argv = &optv[i];
    set->argc = optc - i;
    
    return 0;
}

/**
 * initalize the session struct
 *
 * => p4_SetOptionsDefault , => p4_AddOptions , => FreeOptions
 */
_export int
p4_SetOptions (p4_sessionP set, int argc, char* argv[])
{
    p4_SetOptionsDefault(set);
    return p4_AddOptions (set, argc, argv);
}

/** 
 * de-init the session struct
 *
 * => p4_SetOptions , => p4_AddOptions
 */
_export int
p4_FreeOptions (int returncode, p4_sessionP set)
{
    if (set->heap.block_file)	free (set->block_file);
    if (set->heap.boot_file)	free (set->boot_file);
    if (set->heap.include_file) free (set->include_file);
    if (set->heap.incpaths)	free (set->incpaths);
    if (set->heap.incext)	free (set->incext);
    if (set->heap.blkpaths)	free (set->blkpaths);
    if (set->heap.blkext)	free (set->blkext);
    if (set->heap.editor)	free (set->editor);
    if (set->heap.mapfile)	free (set->mapfile);
    if (set->heap.dumpfile)	free (set->dumpfile);
    if (set->heap.load_image)	free (set->load_image);
    if (set->heap.make_image)	free (set->make_image);
    if (set->heap.prefix)	free (set->prefix);
    if (set->heap.bootcommand)	free (set->bootcommand);
    if (set->heap.optv)		free (set->optv);
    return returncode;
}

/************************************************************************/
/* Initialize memory map:                                               */
/************************************************************************/

void
p4_SetDictMem (p4_threadP thread, void* dictmem, long size)
{
    if (!dictmem) return;
    thread->p[P4_MEM_SLOT] = dictmem;
    thread->moptrs = P4_MEM_SLOT;   /* _cleanup shall not free this one */
    thread->set->total_size = size; /* or any later module mem pointer */
}

static void
init_accept_lined (void)
{
    extern void accept_executes_xt (int);
    static void (*exec[10]) (int) =
    {
	accept_executes_xt, accept_executes_xt, accept_executes_xt,
	accept_executes_xt, accept_executes_xt, accept_executes_xt,
	accept_executes_xt, accept_executes_xt, accept_executes_xt,
	accept_executes_xt,
    };
    
    memset (&PFE.accept_lined, 0, sizeof PFE.accept_lined);
    PFE.accept_lined.history = PFE.history;
    PFE.accept_lined.history_max = PFE.history_top - PFE.history;
    PFE.accept_lined.complete = p4_complete_dictionary ;
    PFE.accept_lined.executes = exec;
    PFE.accept_lined.caps = PFE_set.caps_on != 0;
}

/* 
 * Allocates all memory areas in a continuous buffer at p with given size. 
 */
static void
dict_allocate (void *p, p4ucell size)
{
    void *tmp;
    void *q = (char *) p + size;
    typedef char pock_t[POCKET_SIZE];

    if (! PFE_set.stack_size)
    	PFE_set.stack_size = STACK_SIZE 
            ? STACK_SIZE     : (PFE_set.total_size / 16)  / sizeof(p4cell);
    if (! PFE_set.flt_stack_size)
	PFE_set.flt_stack_size = FLT_STACK_SIZE  
            ? FLT_STACK_SIZE : (PFE_set.total_size / 32) / sizeof(double);
    if (! PFE_set.ret_stack_size)
        PFE_set.ret_stack_size = RET_STACK_SIZE  
            ? RET_STACK_SIZE : (PFE_set.total_size / 32) / sizeof(p4cell);

#define ALLOC(TYPE,ALIGN,MEMBOT,MEMTOP,SIZE)		       \
    (						               \
       MEMTOP = (TYPE *) ((size_t)q & ~((size_t)(ALIGN) - 1)), \
       P4_ADD (q, -(SIZE) * sizeof (TYPE)),		       \
       MEMBOT = (TYPE *)q			               \
    )

    ALLOC (File,	 	PFE_ALIGNOF_CELL,  
	   PFE.files,   	PFE.files_top, 		
	   PFE_set.max_files+3);
    ALLOC (char,	 	1,	      
	   PFE.history, 	PFE.history_top, 	
	   HISTORY_SIZE);
    ALLOC (char,	 	1,	      
	   PFE.tib,     	PFE.tib_end, 		
	   TIB_SIZE);
    ALLOC (p4xt *,		PFE_ALIGNOF_CELL,  
	   PFE.rstack,  	PFE.r0, 		
	   PFE_set.ret_stack_size);
    ALLOC (p4cell,		PFE_ALIGNOF_CELL,  
	   PFE.stack,		PFE.s0, 		
	   PFE_set.stack_size);
#ifndef P4_NO_FP
    ALLOC (double,		PFE_ALIGNOF_DFLOAT, 
	   PFE.fstack,  	PFE.f0, 		
	   PFE_set.flt_stack_size);
#else
    PFE.fstack = 0; PFE.f0 = 0; 
#endif
    ALLOC (pock_t, 	1,           
	   PFE.pockets, 	tmp,    		
	   PFE_set.pockets);
#undef ALLOC

    if ((char *) q < (char *) p + MIN_PAD + MIN_HOLD + 0x4000)
    {
	P4_fatal ("impossible memory map");
	PFE.exitcode = 3;
	p4_longjmp_exit ();
    }
    PFE.dict = (p4char *) p;
    PFE.dictlimit = (p4char *) q;
    init_accept_lined ();
}

/************************************************************************/
/* Here's main()                                                        */
/************************************************************************/

static void p4_atexit_cleanup (void);

/* distinct for each tread ! */
_export p4_threadP p4_main_threadP = NULL; 

/**
 * note the argument 
 */
int
p4_main (p4_threadP th)
{
#  ifdef VXWORKS
    extern int taskVarAdd (int, int*);
    extern int taskIdSelf ();
    taskVarAdd (taskIdSelf (), (int*) &p4_main_threadP);
#  endif
    p4TH = p4_main_threadP = th;  

#  ifdef HAVE_LOCALE_H
    setlocale (LC_ALL, "C");
#  endif
#  if defined SYS_EMX
    _control87 (EM_DENORMAL | EM_INEXACT, MCW_EM);
#  endif

    switch (setjmp (PFE.loop))
    {
    case 'A':
    case 'Q':
	P4_fatal ("Fatal Run Error");
	p4_atexit_cleanup ();
	return -1;
    case 'X':
	P4_info ("Exiting");
	p4_atexit_cleanup ();
	return PFE.exitcode;
    }

    p4TH = p4_main_threadP; 

#  if !defined __WATCOMC__
    if (! isatty (STDIN_FILENO))
        PFE_set.stdio = 1;
#  endif

    if (! PFE_set.stdio)
    {
        if (!p4_prepare_terminal ())
	{
            if (!PFE_set.quiet)
                fputs (
		    "[unknown terminal, "
#                  if defined ASSUME_VT100
		    "assuming vt100"
#                  elif defined ASSUME_DUMBTERM
		    "assuming dumb terminal"
#                  else
		    "running in canonical mode"
#                  endif
		    "]\n", stderr);
#          if !defined ASSUME_VT100 && !defined ASSUME_DUMBTERM
            PFE_set.canonical = 1;
#          endif
	}
	if (! PFE_set.bye)
	{
	    p4_interactive_terminal ();
	    PFE.system_terminal = &p4_system_terminal;
	}
    }

    if (! PFE_set.debug)
        p4_install_signal_handlers ();
    
    if (! PFE_set.quiet)
    {
        p4_outs ("\\ ");
        p4_outs (p4_version_string ());
	if(! PFE_set.include_file) 
	    p4_outs (p4_copyright_string ());
	if (PFE_set.license)
	    p4_outs (p4_license_string ());
	if (PFE_set.warranty)
	    p4_outs (p4_warranty_string ());

	if (! PFE_set.bye)
	{
	    if (! PFE_set.license || ! PFE_set.warranty)
		p4_outs ("\n\nPlease enter LICENSE and WARRANTY. ");
	    else
		p4_outs ("\n\nHi there, enjoy Forth! ");

#         ifndef _K12_SOURCE /* BYE does'nt make sense in an embedded system */
		p4_outs ("- To quit say BYE.\n");
#         else
		p4_outs ("- To restart say COLD.\n");
#         endif /* _K12_SOURCE */
	}
    }
    if (PFE.rows == 0)
        PFE.rows = PFE_set.rows;
    if (PFE.cols == 0)
        PFE.cols = PFE_set.cols;
    
    p4TH->atexit_cleanup = &p4_atexit_cleanup;
    
# ifdef USE_MMAP
    if (PFE_set.mapfile)
    {
	PFE.mapfile_fd = p4_mmap_creat (PFE_set.mapfile,
					PFE_set.mapbase,
					PFE_set.total_size);
	if (! PFE.mapfile_fd)
	{
	    P4_fail1 ("[%p] mapfile failed", p4TH);
	}else{
	    P4_info3 ("[%p] mapped at %8p len %d", 
		      p4TH, PFE_MEM, PFE_set.total_size);
	}
    }
# endif
    if (! PFE_MEM) 
    {
        PFE_MEM = p4_xcalloc (1, (size_t) PFE_set.total_size);
        if (PFE_MEM)
        {
            P4_info3 ("[%p] newmem at %p len %lu",
		      p4TH, PFE_MEM, (unsigned long)PFE_set.total_size);
        }else{
            P4_fail3 ("[%p] FAILED to alloc any base memory (len %lu): %s",
		      p4TH, (unsigned long)PFE_set.total_size, 
		      strerror(errno));
        }
    }
    
    dict_allocate ((char *) PFE_MEM, PFE_set.total_size);
#  ifdef PFE_WITH_DSTRINGS_EXT    /* (dnw 5feb01) */
    PFE.dstrings = (char *) p4_make_str_space (PFE_set.str_buffer_size,
                     P4_MAX_SFRAMES);
#   endif
    p4_boot_system ();
    p4TH = p4_main_threadP; 

    if (PFE_set.load_image)
    {
	if (! p4_load_dict_image (PFE_set.load_image))
	{
	    P4_fail2 ("[%p] load_image failed: %s", p4TH, PFE_set.load_image);
	}
    }

    /* process the boot command: */
    if (PFE_set.bootcommand)
    {
        p4_evaluate (PFE_set.bootcommand, strlen(PFE_set.bootcommand));
    }

    /* Include file from command line: */
    if (PFE_set.include_file)
    {
        p4_included1 (PFE_set.include_file, strlen (PFE_set.include_file), 0);
    }
    
    /* If running in a pipe, process commands from stdin: */
    if (PFE_set.stdio)
    {
        p4_include_file (PFE.stdIn);
        PFE.atexit_cleanup ();
        return 0;
    }
    
    /* If it's a turnkey-application, start it: */
    if (APPLICATION)
    {
        p4_run_forth (APPLICATION);
        PFE.atexit_cleanup ();
        return 0;
    }
    if (PFE_set.verbose)
        FX (p4_dot_memory);
    
    if (PFE_set.dumpfile)
    {  
        int f = p4_creat_image(PFE_set.dumpfile, 0);
        if (-1 != f) { 
            p4_write_image (f, PFE.dict, PFE.dp-PFE.dict, 
			    "dictbase", PFE.nr); 
            p4_write_image (f, &PFE,     sizeof(PFE),      
			    "thread",  PFE.nr); 
            close (f); 
        }else{
            /* perror(PFE_set.dumpfile); */
        }
    }
    
    if (PFE_set.make_image)
    {  
        int f = p4_creat_image(PFE_set.make_image, 0);
        if (-1 != f) { 
            p4_write_image (f, PFE.fence, PFE.dp-PFE.fence, 
			    "dictfence", PFE.nr); 
            p4_write_image (f, &PFE,     sizeof(PFE),     
			    "thread",    PFE.nr); 
            close (f); 
        }else{
            /* perror(PFE_set.make_image); */
        }
    }
    
    if (! PFE_set.bye)
	p4_interpret_loop (); /* will catch QUIT, ABORT, COLD .. and BYE */
    PFE.atexit_cleanup ();
    return 0;
}

/** 
 * init and execute the previously allocated forth-maschine,
 * e.g. pthread_create(&thread_id,0,p4_Exec,threadP);
 */
_export int 
p4_Exec(p4_threadP th)
{
    auto p4_thread_VM vm;
    auto volatile int retval;
    p4_thread_save_VM(&vm);
    retval = p4_main(th);
    p4_thread_load_VM(&vm);
    return retval;
}

static void
p4_atexit_cleanup ()
{
    extern void p4_cleanup_terminal ();
    P4_enter ("atexit cleanup");

    PFE.atexit_running = 1;
    p4_forget ((FENCE = PFE_MEM));
    
    if (PFE.system_terminal)    /* call this once, with the first cpu */
        PFE.system_terminal ();
    p4_cleanup_terminal ();

#  ifdef USE_MMAP
    if (PFE.mapfile_fd)
    {
	p4_mmap_close(PFE.mapfile_fd, PFE_MEM, PFE_set.total_size);
        PFE_MEM = 0; PFE.mapfile_fd = 0;
        P4_info1 ("[%p] unmapped basemem", p4TH);      
    }
#  endif

    { /* see if there's some memory chunk still to be freed */
        register int i;
        register int moptrs = PFE.moptrs ? PFE.moptrs : P4_MOPTRS;
        for ( i=0; i < moptrs; i++) {
            if (PFE.p[i]) { 
                P4_info3 ("[%p] free %d. %p", p4TH, i, PFE.p[i]);
                p4_xfree (PFE.p[i]); PFE.p[i] = 0; 
            }
        }
    }
    
    P4_leave ("atexit cleanup done");
}

/*@}*/

