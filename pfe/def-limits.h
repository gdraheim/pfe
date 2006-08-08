#ifndef __PFE_DEF_LIMITS_H
#define __PFE_DEF_LIMITS_H

#ifndef CHAR_BIT
#include <limits.h>
#endif

#ifndef BITSOF
#define BITSOF(X)	((int)(sizeof (X) * CHAR_BIT))
#endif

#endif
