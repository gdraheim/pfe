/**
 * an example module implementation (C) 2003 Guido Draheim
 * given away under the terms of the ZLIB License (see COPYING.ZLIB)
 *
 * after install, start up a `pfe` box, and type
 *    needs testmodule-ext
 *    hello
 */

#include <pfe/pfe-base.h>

/* The macro FCode creates the callframe info needed by C to be a forth prim */
static
FCode (p4_hello)
{
    p4_outs ("\nHello World!\n");
}

/* the pfe export table binds FCode primitives (in C) to their forth name.
 * it carries other information as well interpreted at load-time of the module.
 */
P4_LISTWORDS(testmodule2) =
{
    P4_INTO ("EXTENSIONS", 0),
    P4_FXco ("hello", p4_hello),
};
P4_COUNTWORDS(testmodule2, "TESTMODULE2 - example module implementation");

/* emacs...
 * Local variables:
 * c-file-style: "stroustrup"
 * End:
 */
