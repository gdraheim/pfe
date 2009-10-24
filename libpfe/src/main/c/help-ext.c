/**
 * -- extra words for external HELP system
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author$)
 *  @version $Revision$
 *     (modified $Date$)
 *
 *  @description
 *      we grep the installed header files for comments ;-)
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) =
"@(#) $Id$";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-limits.h>
#include <pfe/def-paths.h>

#ifdef PFE_HAVE_DIRENT_H
#include <stdio.h>
#include <pfe/os-string.h>
#include <dirent.h>
#endif

static int p4_search_help(p4char* nm, p4ucell ln, const char* directory);

/** (HELP) ( str-ptr str-len -- )
 * display help for the specified word
 * (not functional yet)
 */
void FXCode (p4_paren_help)
{
# if !defined PFE_HAVE_DIRENT_H || !defined PFE_INCLUDEDIR
    p4_outs ("(help not implemented, sorry)");
    FX_2DROP;
# else
    p4ucell ln = FX_POP;
    p4char* nm = (void*) FX_POP;

    static const char search_dir[] = PFE_INCLUDEDIR PFE_DIR_DELIMSTR "pfe";
    if (! p4_search_help (nm, ln, search_dir)) {
    	p4_outs (search_dir);
        p4_outs (": no header help files found. ");
    }
}

#ifndef PATH_MAX
#define PATH_MAX PFE_DEF_PATH_MAX
#endif

static int
p4_search_help(p4char* nm, p4ucell ln, const char* directory)
{
    char buf[P4_LINE_MAX];
    char filename[PATH_MAX];
    DIR* helpdir;
    struct dirent* dirent;

    if (! (helpdir = opendir (directory)))
    	return 0;

    int found = 0;
    while ((dirent = readdir (helpdir)))
    {
        p4_strncpy (filename, directory, sizeof filename);
        p4_strncat (filename, PFE_DIR_DELIMSTR, sizeof filename);
        p4_strncat (filename, dirent->d_name, sizeof filename);

        FILE* f;
        if (! (f = fopen (filename, "r")))
            continue;

        int seen = 0;
        while (fgets(buf,sizeof buf, f))
        {
        	/* look for the first line */
            if (!seen && ! p4_memcmp (buf, "/** ", 4))
            {
                if (! p4_memcmp (buf+4, nm, ln)&& buf[4+ln] == ' ')
                    seen = 1;
                if (buf[4] == '"' && ! p4_memcmp (buf+5, nm, ln) && buf[5+ln] == '"')
                    seen = 1;
                if (seen) {
                    p4_outf ("%s:\n", filename);
                    found += 1;
                }
            }
            /* keep printing */
            if (seen)
            {
                p4_outs(buf); /* ?CR ... later... fixme */
                /* look for the end */
                if (! p4_memcmp (buf, "/** ", 4))
                    seen = 0;
                if (! p4_memcmp (buf, " */", 3))
                    seen = 0;
            }
        }
        fclose (f);
    }
    closedir (helpdir);
    return found;
# endif
}

P4_LISTWORDSET (help) [] =
{
    P4_INTO ("FORTH", "[ANS]"),
    P4_FXco ("(HELP)", p4_paren_help),
};
P4_COUNTWORDSET (help, "HELP System with Headers");

/*@}*/
