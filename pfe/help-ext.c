/** 
 * -- extra words for external HELP system
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE              @(#) %derived_by: guidod %
 *  @version %version:  %
 *    (%date_modified:  %)
 *
 *  @description
 *      we grep the installed header files for comments ;-)
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: help-ext.c,v 1.1.1.1 2006-08-08 09:07:29 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-limits.h>

#ifdef PFE_HAVE_DIRENT_H
#include <stdio.h>
#include <pfe/os-string.h>
#include <dirent.h>
#endif

/** (HELP) ( str-ptr str-len -- )
 * display help for the specified word
 * (not functional yet)
 */
FCode (p4_paren_help)
{
# if !defined PFE_HAVE_DIRENT_H || !defined PFE_INCLUDEDIR
    p4_outs ("(help not implemented, sorry)");
    FX_2DROP;
# else
    char buf[80];
    char filename[80];
    DIR* helpdir;
    struct dirent* dirent;
    p4ucell ln = FX_POP;
    p4char* nm = (void*) FX_POP;

    if (! (helpdir = opendir (PFE_INCLUDEDIR"/pfe")))
    {
	p4_outs ("no header help files found");
	return;
    }

    while ((dirent = readdir (helpdir)))
    {
	register FILE* f;
	register int seen;
	p4_strncpy (filename, PFE_INCLUDEDIR"/pfe/", sizeof filename);
	p4_strncat (filename, dirent->d_name, sizeof filename);

	if (! (f = fopen (filename, "r")))
	    continue;

	seen = 0;
	while (fgets(buf,sizeof buf, f))
	{
	    if (!seen && ! p4_memcmp (buf, "/** ", 4))
	    {
		if (! p4_memcmp (buf+4, nm, ln) 
		    && buf[4+ln] == ' ')
		    seen = 1;
		if (buf[4] == '"' && ! p4_memcmp (buf+5, nm, ln) 
		    && buf[5+ln] == '"')
		    seen = 1;
		if (seen)
		    p4_outf ("%s:\n", filename);
	    }
	    if (seen)
	    {
		p4_outs(buf);
		seen++; /* ?CR ... later... fixme */
	    }
	    if (seen > 2)
	    {
		if (! p4_memcmp (buf, "/** ", 4))
		    seen = 0;
		if (! p4_memcmp (buf, " */", 3))
		    seen = 0;
		if (! seen)
		    return;
	    }
	}
	fclose (f);
    }
    closedir (helpdir);
# endif
}



P4_LISTWORDS (help) =
{
    P4_INTO ("FORTH", "[ANS]"),
    P4_FXco ("(HELP)", p4_paren_help),
};
P4_COUNTWORDS (help, "HELP System with Headers");

/*@}*/
/* 
 * Local variables:
 * c-file-style: "stroustrup"
 * End:
 */


