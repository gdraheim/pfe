#ifndef __PFE_LINED_H
#define __PFE_LINED_H

/** 
 * -- declarations for line editor
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2000.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2008-04-20 04:46:29 $)
 */
/*@{*/
 
#include <pfe/def-macro.h>

/* Simple input of one line only: initialize the members string and max_length
   and all others with 0, then call p4_lined().
   For luxury: before first call to p4_lined() initialize history and
   history_max with a static or calloc()-ed memory area, all others with 0.
   Then don't touch the components between further calls to lined()
*/

struct lined
{
    char *string;		 /* where to place the result */
    int max_length;		 /* maximum chars to input */
    char *history;		 /* where the history is kept */
    int history_max;		 /* size of history area */
    int (*complete)		 /* called on tab key for completion */
         (char *in,		 /* string typed so far */
           char *out,		 /* possible completion */
           int display);	 /* flag: display alternatives */
    void (**executes) (int);	 /* NULL or list of functions bound to keys */
    int length; 		 /* output: how many chars actually typed */
    char overtype, caps, hidden, unused;  /* flags */
         /* these are for internal use only: */
    int cursor;			 /* position in the string */
    int history_length;		 /* how much history is available */
    int history_read;		 /* when using arrow up and down: */
    int history_write;		 /* where you are in that mass of strings. */
         /* some extensions that came in later: */
    int (*intercept) (char* str, int len); /* returns new length of 'str' */
    void *user3, *user2, *user1; /* padded to 16*32bit on 32bit machines */
};

int p4_lined (struct lined *l, char *dflt);

/*@}*/
#endif




