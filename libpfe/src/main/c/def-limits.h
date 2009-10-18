#ifndef __PFE_DEF_LIMITS_H
#define __PFE_DEF_LIMITS_H

#ifndef CHAR_BIT
#include <limits.h>
#endif

#ifndef BITSOF
#define BITSOF(X)	((int)(sizeof (X) * CHAR_BIT))
#endif

/* Maximum number of bytes allowed in a terminal input queue.
 * _POSIX_MAX_INPUT minimum value: 255 (also used on Linux)
 */
#ifdef MAX_INPUT
#define P4_MAX_INPUT MAX_INPUT
#else
#define P4_MAX_INPUT 255
#endif

/* Unless otherwise noted, the maximum length, in bytes, of a utility's
 * input line (either standard input or another file), when the utility
 * is described as processing text files. The length includes room for the
 * trailing newline. Minimum Acceptable Value: _POSIX2_LINE_MAX = 2048
 */
#ifdef LINE_MAX
#define P4_LINE_MAX LINE_MAX
#else
#define P4_LINE_MAX 2048
#endif

/* Maximum number of bytes that is guaranteed to be atomic when writing
 * to a pipe. Minimum value is _POSIX_PIPE_BUF = 512
 */
#ifdef PIPE_BUF
#define P4_PIPE_BUF PIPE_BUF
#else
#define P4_PIPE_BUF 512
#endif

/* Maximum length of argument to the exec functions including environment
 * data. Minimum Acceptable Value: _POSIX_ARG_MAX = 4096
 */
#ifdef ARG_MAX
#define P4_ARG_MAX
#else
#define P4_ARG_MAX 4096
#endif

#endif
