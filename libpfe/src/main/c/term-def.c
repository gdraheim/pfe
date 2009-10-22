#include <pfe/def-config.h>

#ifdef PFE_WITH_X11
#include "term-x11.c"
#endif

#if PFE_TERM_DEF == 1
#include "term-wincon.c"
#elif PFE_TERM_DEF == 2
#include "term-dj.c"
#elif PFE_TERM_DEF == 4
#include "term-emx.c"
#elif PFE_TERM_DEF == 5
#include "term-curses.c"
#elif PFE_TERM_DEF == 6
#include "term-lib.c"
#elif PFE_TERM_DEF == 7
#include "term-curses.c"
#elif PFE_TERM_DEF == 8
#include "term-lib.c"
#elif PFE_TERM_DEF == 9
#include "term-wat.c"
#elif PFE_TERM_DEF == 11
#include "term-x11.c"
#else
#include "term-lib.c"
#endif
