#ifndef __PFE_LINED_H
#define __PFE_LINED_H "%full_filespec: lined.h~5.2:incl:bln_12xx!5 %"

/** 
 * -- declarations for line editor
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2000. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE            @(#) %derived_by: guidod %
 *  @version %version: 5.2 %
 *    (%date_modified: Mon Mar 12 10:33:36 2001 %)
 */
/*@{*/
 
#include <pfe/def-macro.h>

/* Simple input of one line only: initialize the members string and max_length
   and all others with 0, then call lined().
   For luxury: before first call to lined() initialize history and
   history_max with a static or calloc()-ed memory area, all others with 0.
   Then don't touch the components between further calls to lined()
*/

struct lined
{
    char *string;		/* where to place the result */
    int max_length;		/* maximum chars to input */
    char *history;		/* where the history is kept */
    int history_max;		/* size of history area */
    int (*complete)		/* called on tab key for completion */
         (char *in,		/* string typed so far */
           char *out,		/* possible completion */
           int display);	/* flag: display alternatives */
    void (**executes) (int);	/* NULL or list of functions bound to keys */
    int length;		/* output: how many chars actually typed */
    char overtype, caps, hidden;	/* flags */
         /* these are for internal use only: */
    int cursor,			/* position in the string */
    history_length,		/* how much history is available */
    history_read,			/* when using arrow up and down: */
    history_write;		/* where you are in that mass of strings. */
};

int p4_lined (struct lined *l, char *dflt);

/*@}*/
#endif




