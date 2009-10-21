/**
 * an example module implementation (C) 2003 Guido U. Draheim
 * given away under the terms of the ZLIB License (see COPYING.ZLIB)
 *
 * after install, start up a `pfe` box, and type
 *    needs testmodule-ext
 *    hello
 */

#include <pfe/pfe-base.h>

/* The macro void FXCode creates the callframe info needed by C to be a forth prim */
static
void FXCode (p4_hello)
{
    p4_outs ("\nHello World!\n");
}

/* the pfe export table binds FXCode primitives (in C) to their forth name.
 * it carries other information as well interpreted at load-time of the module.
 */
P4_LISTWORDSET(testmodule1) [] =
{
    P4_INTO ("EXTENSIONS", 0),
    P4_FXco ("hello", p4_hello),
};
P4_COUNTWORDSET (testmodule1, "TESTMODULE1 - example module implementation");
