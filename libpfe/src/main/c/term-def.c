#include <pfe/def-config.h>

#ifdef PFE_WITH_X11
#include <pfe/term-x11.c>
#endif

#if PFE_TERM_DEF == 1
#include <pfe/term-wincon.c>
#elif PFE_TERM_DEF == 2
#include <pfe/term-dj.c>
#elif PFE_TERM_DEF == 3
#include <pfe/term-k12.c>
#elif PFE_TERM_DEF == 4
#include <pfe/term-emx.c>
#elif PFE_TERM_DEF == 5
#include <pfe/term-curses.c>
#elif PFE_TERM_DEF == 6
#include <pfe/term-lib.c>
#elif PFE_TERM_DEF == 7
#include <pfe/term-curses.c>
#elif PFE_TERM_DEF == 8
#include <pfe/term-lib.c>
#elif PFE_TERM_DEF == 9
#include <pfe/term-wat.c>
#elif PFE_TERM_DEF == 11
#include <pfe/term-x11.c>
#else
#include <pfe/term-lib.c>
#endif


