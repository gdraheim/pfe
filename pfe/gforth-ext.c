/** 
 * -- Words designed to mimic gforth behaviour.
 *
 *  Copyright (C) Guido U. Draheim 2001 - 2004. 
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2008-04-20 04:46:31 $)
 *
 *  @description
 *		Usually the following words are defined to mimic
 *		a certain gforth extension that some application
 *		writers need. They are only added on request, and
 *		they may be removed without warning requesting to
 *              use the functionality provided by pfe itself.
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: gforth-ext.c,v 1.3 2008-04-20 04:46:31 guidod Exp $";
#endif

/* ------------------------------------------------------------------- */

#include <pfe/pfe-base.h>
#include <pfe/os-string.h>
#include <errno.h>

#ifdef PFE_HAVE_DIRENT_H
#include <dirent.h>

#define  IOR(flag)       ((flag)? -512-errno : 0)

/** open-dir   ( c_addr u -- wdirid wior )  gforth  open_dir
 * will vanish without warning. see gforth documentation.
 */
FCode(p4_gforth_open_dir)
{
    char * p = p4_pocket_filename ((void*) p4SP[1], p4SP[0]);
    p4SP[1] = (p4cell)opendir (p);
    p4SP[0] =  IOR (p4SP[1] == 0);
}

/** read-dir   ( c_addr u1 wdirid -- u2 flag wior )  gforth  read_dir
 * will vanish without warning. see gforth documentation.
 */
FCode(p4_gforth_read_dir)
{
    struct dirent * dent;
    dent = readdir ((DIR *)p4SP[0]);
    if (dent == NULL) 
    {
	p4_memset (&p4SP[0],0, 3*sizeof(p4cell));
    } else {
	p4cell len = p4_strlen (dent->d_name); 
	if (len > p4SP[1])
	{
	    p4SP[0] = -512-ENAMETOOLONG;
	    len = p4SP[1];
	}else{
	    p4SP[0] = 0;
	}
	p4_memcpy ((void*)(p4SP[2]), dent->d_name, len);
	p4SP[2] = len;
	p4SP[1] = P4_TRUE;
    }
}

/** close-dir       ( wdirid -- wior )      gforth  close_dir
 * will vanish without warning. see gforth documentation.
 */
FCode(p4_gforth_close_dir)
{
    *p4SP = IOR (closedir ((DIR *)*p4SP));
}
#endif
    /* PFE_HAVE_DIRENT_H */

/* -------------------------------------------------------------
 * This defines execution chains.
 * The first application for this is building initialization chains:
 * Think of many modules or program parts, each of it with some specific
 * initialization code. If we hardcode the initialization routines into a
 * "master-init" we will get unflexible and are not able only to load some
 * specific modules...

 * The chain is basicaly a linked-list. Define a Variable for the head of
 * linked-list. Name it "foo8" or "foo-chain" to indicate it is a execution
 * chain.

 * You can add a word to the list with "' my-init foo8 chained". You can
 * execute all the code with "foo8 chainperform".
 */

/** linked ( list -- ) \ gforth
 : linked        here over @ a, swap ! ;
 * (note: win32forth calls it "link," )
 */
FCode (p4_gforth_linked)
{
    register void** link = (void**) FX_POP;
    register void*  here = (void*)  p4_DP;

    FX_PCOMMA (*link);
    *link = here;
}

/** chained       ( xt list -- ) \ gforth
 * generic chains
 : chained  linked , ;
 */
FCode(p4_gforth_chained)
{
    FX_FCODE (p4_gforth_linked);
    FX_XCOMMA (FX_POP);
}

/** chainperform  ( list -- ) \ gforth
 *
 : chainperform  BEGIN @ dup WHILE dup cell+ perform REPEAT drop ;
 */
FCode (p4_gforth_chainperform)
{
    register void** link = (void**) FX_POP;
    while ((link = (void**) *link))
    {
	p4xt* xt = link[1];
	if (xt && *xt) PFE.execute (*xt);
    }
}

static FCode (p__gforth)
{
#  ifndef PFE_PACKAGE
#  define PFE_PACKAGE "PFE"
#  endif
#  ifndef PFE_VERSION
#  define PFE_VERSION "33.x"
#  endif
    p4_strpush (PFE_PACKAGE" "PFE_VERSION" module, "
                "words compatible with gforth 5.0");
}

#include <pfe/def-words.h>

P4_LISTWORDS (gforth) =
{
    P4_INTO ("gforth'",		0),
    P4_FXCO ("linked",		p4_gforth_linked),
    P4_FXCO ("chained",		p4_gforth_chained),
    P4_FXCO ("chainperform",	p4_gforth_chainperform),
# ifdef PFE_HAVE_DIRENT_H
    P4_FXCO ("open-dir",	p4_gforth_open_dir),
    P4_FXCO ("read-dir",	p4_gforth_read_dir),
    P4_FXCO ("close-dir",	p4_gforth_close_dir),
# endif
    P4_INTO ("ENVIRONMENT", 0),
    P4_OCON ("GFORTH-DIR",	500 ),
    P4_FXco ("GFORTH",          p__gforth),
};
P4_COUNTWORDS (gforth, "GForth'Like words for applications");

/*@}*/

/* 
 * Local variables:
 * c-file-style: "stroustrup"
 * End:
 */
