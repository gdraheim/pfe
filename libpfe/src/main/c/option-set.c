/**
 * -- Process command line, init option block, prepare for start.
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
 *  Process command line, init option block, prepare for start.
 *  The init of ofe memory and start up the interpret loop in PFE
 *  is done in the engine-set part.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) =
"@(#) $Id$";
#endif

#define	_P4_SOURCE 1


#include <pfe/pfe-base.h>
#include <pfe/def-limits.h>

#include <stdlib.h>
#include <stdarg.h>
#include <pfe/os-string.h>
#ifndef P4_NO_FP
#include <float.h>
#endif
#include <errno.h>
#ifdef PFE_HAVE_LOCALE_H
#include <locale.h>
#endif
#ifdef PFE_HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <pfe/term-sub.h>
#include <pfe/version-sub.h>

#include <pfe/option-ext.h>
#include <pfe/logging.h>

#ifndef _export
#define _export
# include <pfe/def-types.h>
# include <pfe/def-words.h>
#endif

#include <pfe/def-restore.h>

/************************************************************************/
/* Analyze command line options:                                        */
/************************************************************************/

#ifndef CAPS_ON                 /* USER-CONFIG: */
#define	CAPS_ON		0	/* do you like (faked) caps lock by default? */
#endif
#ifndef UPPER_CASE_ON           /* USER-CONFIG: */
#define	UPPER_CASE_ON	1	/* allow "dup" to find "DUP" by default */
#endif
#ifndef LOWER_CASE_ON           /* USER-CONFIG: */
#define	LOWER_CASE_ON	1	/* allow "Dup" to find "dup" by default */
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

/**
 * fill the session struct with precompiled options
 */
void
p4_SetOptionsDefault(p4_sessionP set, int len)
{
    if (! set) return;

    p4_memset(set, 0, sizeof(*set));

    /* newstyle option-ext support */
    set->opt.dict = set->opt.space;
    set->opt.dp = set->opt.dict;
    set->opt.last = 0;
    if (! len) len = sizeof(*set);
    set->opt.dictlimit = ((p4char*)set) + len;

    set->argv = 0;
    set->argc = 0;
    set->optv = 0;
    set->optc = 0;
    set->boot_name = 0;
    set->isnotatty = 0;
    set->stdio = 0;
    set->caps_on = CAPS_ON;
    set->find_any_case = LOWER_CASE_ON;
    set->lower_case_fn = LWRCASE_FN_ON;
    set->upper_case_on = UPPER_CASE_ON;
#  ifndef P4_NO_FP
    set->float_input = FLOAT_INPUT_ON;
#  else
    set->float_input = 0;
#  endif
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
    set->ret_stack_size = 0;

    /*set->boot_include = 0;*/
    set->inc_paths = p4_append_option_string ((p4_char_t*)
        "INC-PATH",8,PFE_PATH_DELIMITER,PFE_INC_PATH,set);
    set->blk_paths = p4_append_option_string ((p4_char_t*)
        "BLK-PATH",8,PFE_PATH_DELIMITER,PFE_BLK_PATH,set);
    set->lib_paths = p4_append_option_string ((p4_char_t*)
        "LIB-PATH",8,PFE_PATH_DELIMITER,PFE_LIB_PATH,set);
    set->cpus = P4_MP;

    if (! set->inc_ext)
    {
        set->inc_ext = p4_append_option_string ((p4_char_t*)
            "INC-EXT",7,PFE_PATH_DELIMITER,PFE_INC_EXTENSIONS,set);
        set->blk_ext = p4_append_option_string ((p4_char_t*)
            "BLK-EXT",7,PFE_PATH_DELIMITER,PFE_BLK_EXTENSIONS,set);
    }

    /* environment scanning */
    char* t;
    /*
     * get special options from environment variables:
     */
    if ((t = getenv ("FORTHINCLUDE")) != NULL)
    {
        p4_change_option_string ((p4_char_t*) "INC-PATH",8,t,set);
        p4_change_option_string ((p4_char_t*) "BLK-PATH",8,t,set);
    }
    else if ((t = getenv ("PFEINCLUDE")) != NULL)
    {
        p4_change_option_string ((p4_char_t*) "INC-PATH",8,t,set);
        p4_change_option_string ((p4_char_t*) "BLK-PATH",8,t,set);
    }

    if ((t = getenv ("PFEDIR")) != NULL)
    {
        p4_change_option_string ((p4_char_t*) "PREFIX-DIR", 10, t, set);
    }
    if ((t = getenv ("PFELIBDIR")) != NULL)
    {
        p4_change_option_string ((p4_char_t*) "LIB-PATH",8,t,set);
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
    "-B --prefix DIR       \t prefix installation path to be used",
    "-C --case-sensitive   \t turn on case-sensitive matching (no upper)",
    "-c --caps-lock        \t turn on CAPS lock",
    "-e --evaluate         \t bootcommand to evaluate",
    "-f --max-files        \t maximum N simultaneously open files",
    "-F --lowercase-fn     \t convert file names to lower case",
    "-G --float-input      \t allow input of floating point numbers",
    "-i --bootfile FILE    \t use FILE as forth script inside boot",
    "-k --total-size SIZE  \t SIZE of system in KBytes",
    "-l --lower-case       \t base system's wordlists are case insensitive",
    "-L --license          \t display license",
    "-p --max-pockets N    \t number of pockets for S\"",
    "-q --quiet            \t suppress signon message",
    "-r --ret-stack SIZE   \t SIZE of return stack in cells",
    "-s --stack SIZE       \t SIZE of stack in cells",
    "-t --flt-stack SIZE   \t SIZE of floating point stack in items",
    "-T --screen CxR       \t text screen has C/columns and R/rows",
    "-v --verbose          \t verbose",
    "-V --version          \t version string",
    "-W --warranty         \t display warranty. Of course: Absolutely none.",
    "-y --bye              \t non-interactive, exit after running file",
    "-? --help             \t display this message and exit",
    " * Turn option off by appending \"-\" to the letter.",
    " * The given file is loaded initially.",
    "-I  --path <path>     \t add to search path",
    "-P  --pipe            \t run in pipe, just read from stdio",
    "-!  --debug           \t start debugging",
    " * generic options: (transferred into environment-wordlist)",
    " * --OPTION-string=<str>    set string 'OPTION' (without -string suffix)",
    " * --OPTION-value=<val>     set value  'OPTION' (without -value suffix)",
    " * --OPTION-(dir|file|image)=<name> set a string variable of this name",
    " * --OPTION-(init|command)=<string> append/reset an *-init string of pfe",
    " * --OPTION-(ext|extensions)=<string> append/reset ext(ensions) variable",
    " * --OPTION-path=<name>     append to 'OPTION-PATH' with path-delim",
    " * --OPTION-cells=<val>     set value 'OPTION-CELLS' in size elements",
    " * --OPTION-base=<val>      set value 'OPTION-BASE' as if an offset",
    " * --OPTION-<on|off>        set value 'OPTION' to flag as true or false",
    " * --OPTION-name=<str>      set strng '$OPTION' to the name string",
    " * --OPTION-size=<val[K]>   set value '/OPTION', understands K=1024 etc.",
    " * --max-OPTION=<val[K]>    set value '#OPTION', understands K=1024 etc.",
    "   e.g. --map-base --map-file --dump-file --str-buffer-size",
    "        --load-image --make-image --block-file --boot-file",
    "        --max-locals --max-cpus --max-files --inc-path",
    "        --data-stack-size --fp-stack-size --return-stack-size",
    "        --editor-name",
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

    if (! f) f = stderr;

    fprintf (f, "%s\n%s\n", p4_version_string (), p4_copyright_string ());

    for (p = help_options; *p && **p; p++)
    {
        if (**p == '-')  fprintf(f, "  "); /* indent the options */
        switch ((*p)[1])
        {
        default:
            if ((*p)[1] > ' ') fprintf(f, *p);
            else fprintf(f, "  %s", (*p)+2);
            break;
        case '>': fprintf(f, *p,
                          set->boot_name ? *set->boot_name : PFE_PACKAGE);
            break;
        case 'B': fprintf(f, "%s [%s]", *p, p4_search_option_string (
                              (p4_char_t*) "PREFIX-DIR",10, ".", set));
            break;
        case 'C': fprintf(f, "%s [%s]", *p, set->upper_case_on ? "OFF":"ON");
            break;
        case 'c': fprintf(f, "%s [%s]", *p, set->caps_on ? "ON":"OFF");
            break;
        case 'G': fprintf(f, "%s [%s]", *p, set->float_input ? "ON":"OFF");
            break;
        case 'f': fprintf(f, "%s [%d]", *p, (int) p4_search_option_value (
                              (p4_char_t*) "#files",6, MAX_FILES, set));
            break;
        case 'F': fprintf(f, "%s [%s]", *p, set->lower_case_fn ? "ON":"OFF");
            break;
        case 'k': fprintf(f, "%s [%d K]", *p, (int) set->total_size >> 10);
            break;
        case 'l': fprintf(f, "%s [%s]", *p, set->find_any_case ? "ON":"OFF");
             break;
        case 'p': fprintf(f, "%s [%d]", *p, (int) p4_search_option_value (
                              (p4_char_t*) "#pockets",8, POCKETS, set));
            break;
        case 'r': fprintf(f, "%s [%d]", *p, (int) set->ret_stack_size);
            break;
        case 's': fprintf(f, "%s [%d]", *p, (int) set->stack_size);
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

    if (! l) l = p4_strlen(str);
    if (l == 1) return *str;

    for (p=helptab; *p; p++)
    {
        if (**p != '-') continue;
        q = *p;
        q++; while (*q && *q != '-') q++; while (*q == '-') q++;
        if (p4_strlen (q) > l && p4_memequal (q, str, l) && q[l] == ' ')
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
int
p4_AddOptions (p4_sessionP set, int argc, const char** argv)
{
    int i, optc, flag;		/* count of all options */
    char const ** optv;		/* values of these options */
    char const *t, *val;
    char const* boot_include = 0;

    if (! argc) return 0;

    if (argc && argv[0] && set->boot_name)
        set->boot_name = & argv[0];

    /* we may have already scanned some options (setup via set->argc) */
    optv = malloc (sizeof(char*) * (set->optc + argc));
    if (! optv) return 2;
    optc = set->optc + argc -1;

    if (set->optc)
        p4_memcpy (&optv[0], &set->optv[0], (set->optc) * sizeof(char*));
    p4_memcpy (&optv[set->optc], &argv[1], (argc-1) * sizeof(char*));
    optv[optc] = 0;
    /* set->optv = optv; */   /* see later down where we really set it */
    /* set->optc = optc; */

    /*
     * process options:
     */
    for (i = set->optc; i < optc; i++)
    {
        register int l, k, s;
        const char* p;

        t = optv[i]; /* scan options up to first (include-)file argument */
        if (*t == '-') { t++; }
        else { boot_include = optv[i]; i++; break; }
        if (*t == '-') {
            t++; if (*t == '-') { i++; break; } /* triple => no scriptfile */
            if (!*t) { /* double => stopscanning, use next arg as scriptfile */
                i++;  if (i < optc) { boot_include = optv[i]; i++; }
                break; };
        }

        k = l = p4_strlen(t);
        p = p4_strchr(t, '=');
        if (p) { k = p-t; } /* length of key */

        s=0; /* skips i - use if val is consumed */
        flag = 1; /* ON - may be switched to OFF here...*/
        if (k == l && t[k-1] == '-') { k--; flag ^= 1; }
        if (l >= 4 && p4_strequal (t, "no-")) { t+=3; k-=3; flag ^= 1; val=t+k; }
        else if (k != l) { val = t + k + 1; } /* seperator = or postfix - */
        else if (i == optc - 1) { val = NULL; }
        else { val = optv[i+1]; s=1; }

        switch (help_opt(t, k, help_options))
        {
        case 'V': fprintf (stdout, "%s\n", p4_version_string ());
                                                return 1; continue;
            /*
             * Simple flag options can be -x or -x- to turn them off.
             * these can be combined into a single option.
             */
        case 'c': set->caps_on = flag;         continue;
        case 'C': set->upper_case_on = ! flag; continue;
        case 'l': set->find_any_case = flag;   continue; /* depracated */
        case 'F': set->lower_case_fn = flag;   continue;
        case 'G': set->float_input = flag;     continue;
        case 'L': set->license = flag;	       continue;
        case 'W': set->warranty = flag;        continue;
        case 'q': set->quiet = flag;           continue;
        case 'v': set->verbose = flag;         continue;
        case 'P': set->stdio = flag;           continue;
        case 'y': set->bye = flag;             continue;
        case '!': set->debug = flag;           continue;

             /*
              * Other options have values either following
              * immediately after the option letter or as
              * next command line argument:
              */
        case 'B': p4_change_option_string ((p4_char_t*) "PREFIX-DIR",10,
                                           val,set);
            i+=s; continue;
        case 'e': p4_append_option_string ((p4_char_t*) "BOOT-INIT",9,
                                           ' ',val,set);
            i+=s; continue;
        case 'i': p4_change_option_string ((p4_char_t*) "BOOT-FILE",9,
                                           val,set);
            i+=s; continue;
        case 'k': set->total_size = atoi (val) << 10; i+=s; continue;
        case 'r': set->ret_stack_size = atoi (val);   i+=s; continue;
        case 's': set->stack_size = atoi (val);       i+=s; continue;
        case 'p': { int v = atoi (val);
            p4_change_option_value ((p4_char_t*) "#pockets",6,
                                    v, set); }
            i+=s; continue;
        case 'f':  { int v = atoi (val); if (v < 0) v = MAX_FILES;
            p4_change_option_value ((p4_char_t*) "#files",6,
                                    v, set); }
            i+=s; continue;
        case 'T':
            if (sscanf (val, "%dx%d", &set->cols, &set->rows) != 2)
                set->cols = TEXT_COLS, set->rows = TEXT_ROWS;
            i+=s; continue;

        case 'I': /* this adds the specified string to the internal string */
        {
            set->inc_paths = p4_append_option_string ((p4_char_t*)
                "INC-PATH",8,PFE_PATH_DELIMITER, val, set);
            set->blk_paths = p4_append_option_string ((p4_char_t*)
                "BLK-PATH",8,PFE_PATH_DELIMITER, val, set);
            i+=s; continue;
        }
#       ifdef __move_cpus_code_to_forth_vm_init
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
#       endif
        case '?': help_print (set, stdout);  return 1; continue;
        default:
        {
            /* generic option setting via option-ext (into environment-wl) */
            p4char name [32];
            if (k > 6 && p4_memequal (t + k - 6, "-value", 6))
            {
                p4_change_option_value ((p4_char_t*) t, k-6,
                                        p4_convsize (val, 1), /* direct */
                                        set);
                i += s;
            }
            else if (k > 7 && p4_memequal (t + k - 7, "-string", 7))
            {
                p4_change_option_string ((p4_char_t*) t, k - 7,
                                         val, set);
                i += s;
            }
            else if (k > 5 && p4_memequal (t + k - 5, "-path", 5))
            {
                p4_append_option_string ((p4_char_t*) t, k,
                                         PFE_PATH_DELIMITER, val, set);
                i += s;
            }
            else if ((k > 5 && p4_memequal (t + k - 4, "-dir", 4)) ||
                     (k > 5 && p4_memequal (t + k - 5, "-file", 5)) ||
                     (k > 6 && p4_memequal (t + k - 6, "-image", 6)))
            {
                p4_change_option_string ((p4_char_t*) t, k, val, set);
                i += s;
            }
            else if (k > 8 && p4_memequal (t + k - 8, "-command", 8) && k < 39)
            {
                p4_memcpy (name, t, k-8); p4_memcpy (name+k-8, "-init", 5);
                p4_change_option_string ((p4_char_t*) name, k,
                                         val, set);
                i += s;
            }
            else if (k > 8 && p4_memequal (t + k - 5, "-init", 5) && k < 36)
            {
                p4_append_option_string ((p4_char_t*) t, k,
                                         ' ', val, set);
                i += s;
            }
            else if (k > 8 && p4_memequal (t + k - 5, "-ext", 4))
            {
                p4_append_option_string ((p4_char_t*) t, k,
                                         PFE_PATH_DELIMITER, val, set);
                i += s;
            }
            else if (k > 8 && p4_memequal (t + k - 5, "-extensions", 11))
            {
                p4_change_option_string ((p4_char_t*) t, k-7,
                                         val, set);
                i += s;
            }
            else if (k > 6 && p4_memequal (t + k - 6, "-cells", 6))
            {
                p4_change_option_value ((p4_char_t*) t, k,
                                        p4_convsize (val, 1), /* %cells */
                                        set);
                i += s;
            }
            else if (k > 5 && p4_memequal (t + k - 5, "-base", 5))
            {
                p4_change_option_value ((p4_char_t*) t, k,
                                        p4_convsize (val, 1), /* direct */
                                        set);
                i += s;
            }
            else if (k > 5 && p4_memequal (t + k - 5, "-size", 5) && k < 36)
            {
                /* --pad-size becomes "environment /pad" */
                name[0] = '/'; p4_memcpy (name+1, t, k - 5);
                p4_change_option_value ((p4_char_t*) name, k-4,
                                        p4_convsize (val, 1),
                                        set);
                i += s;
            }
            else if (k > 5 && p4_memequal (t + k - 5, "-name", 5) && k < 36)
            {
                /* --editor-name becomes "environment $editor" */
                name[0] = '$'; p4_memcpy (name+1, t, k - 5);
                p4_change_option_string ((p4_char_t*) name, k-4,
                                         val, set);
                i += s;
            }
            else if (k > 4 && p4_memequal (t , "max-", 4) && k < 35)
            {
                /* --max-locals becomes "environment #locals" */
                name[0] = '#'; p4_memcpy (name+1, t + 4, k - 4);
                p4_change_option_value ((p4_char_t*) name, k-3,
                                        p4_convsize (val, 1),
                                        set);
                i += s;
            }
            else if (k > 4 && p4_memequal (t + k - 4, "-off", 4))
            {
                flag ^= 1;
                p4_change_option_value ((p4_char_t*) t, k - 4,
                                        flag, set);
            }
            else if (k > 3 && p4_memequal (t + k - 3, "-on", 3))
            {
                p4_change_option_value ((p4_char_t*) t, k - 3,
                                        flag, set);
            }
            else
            {
                help_print (set, stderr);  return 2;
            }
            continue;
        } /*default*/
        } /*switch*/
    }

    if (boot_include)
        p4_change_option_string ((p4_char_t*) "SCRIPT-FILE", 11,
                                 boot_include, set);

    if (set->optv) free (set->optv);
    set->optv = optv;   /* and here we really set it */
    set->optc = optc;

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
int
p4_SetOptions (p4_sessionP set, int len, int argc, const char** argv)
{
    p4_SetOptionsDefault(set, len);
    return p4_AddOptions (set, argc, argv);
}

/**
 * de-init the session struct
 *
 * => p4_SetOptions , => p4_AddOptions
 */
int
p4_FreeOptions (int returncode, p4_sessionP set)
{
    if (set->optv) free ((void*) set->optv);

    p4_invalidate_string_options (set);
    return returncode;
}

/**
 * set prelinked-modules-table
 */
int
p4_SetModules (p4_sessionP set, p4Words* modules)
{
    set->modules = modules;
    return 0;
}

/************************************************************************/
/* physical instance of the global system variable:                     */
/************************************************************************/

#ifndef P4_REGTH
# ifdef PFE_USE_THREAD_BLOCK
/*export*/ PFE_CC_THREADED struct p4_Thread  p4_reg;
/*export*/ PFE_CC_THREADED struct p4_Session p4_opt;
static  char allocated_p4_reg = 0;
static  char allocated_p4_opt = 0;
# else
/*export*/ PFE_CC_THREADED struct p4_Thread* p4TH;
# endif
#endif

/** new VM-Thread session options */
p4_sessionP
p4_NewSessionOptions (int extra)
{
#  ifdef PFE_USE_THREAD_BLOCK
    if (allocated_p4_opt)
        return 0;
    p4_SetOptionsDefault (&p4_opt, sizeof(p4_opt));
    allocated_p4_opt = 1;
    return &p4_opt;
#  else
    p4_sessionP ptr = malloc (sizeof(*ptr)+extra);
    p4_SetOptionsDefault (ptr, sizeof(*ptr)+extra);
    return ptr;
#  endif
}


/** new VM-Thread seesion options */
p4_threadP
p4_NewThreadOptions (p4_sessionP set)
{
#  ifdef PFE_USE_THREAD_BLOCK
    if (allocated_p4_reg)
        return 0;
    p4_reg.set = set;
    allocated_p4_reg = 1;
    return &p4_reg;
#  else
    p4_threadP ptr = malloc (sizeof(*ptr));
    p4_memset (ptr, 0, sizeof(*ptr));
    ptr->set = set;
    return ptr;
#  endif
}

/** bind VM-Thread with VM-Thread session options */
p4_threadP
p4_SetThreadOf(p4_threadP ptr, p4_sessionP set)
{
    if (! ptr) return ptr;
    p4_memset (ptr, 0, sizeof (*ptr));
    ptr->set = set;
    return ptr;
}

/** new VM-Thread session options */
char
p4_FreeSessionPtr (p4_sessionP ptr)
{
#  ifdef PFE_USE_THREAD_BLOCK
    if (ptr != &p4_opt)
        return 1;
    return ((allocated_p4_opt = 0));
#  else
    if (ptr) free (ptr);
    return 0;
#  endif
}

/** free VM-Thread */
char
p4_FreeThreadPtr (p4_threadP ptr)
{
#  ifdef PFE_USE_THREAD_BLOCK
    if (ptr != &p4_reg)
        return 1;
    return ((allocated_p4_reg = 0));
#  else
    if (ptr) free (ptr);
    return 0;
#  endif
}

/*@}*/
