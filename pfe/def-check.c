/*
 * Allow checking the def-*.h configuration. There are various extra
 * checks in those headers that are triggered by the following define
 */
#define PFE_DEF_CHECK 1

/*
 * Furthermore we try to include the header files in an overly correct 
 * order with the least amount of nested includes. Between each header
 * file we go to check for some additional specifics.
 */

#include <pfe/def-config.h>
/* "config.h" ifdef HAVE_CONFIG_H */
/* <pfe/_config.h> autoconf generated */
/* <pfe/_target.h> autoconf generated */
#include <pfe/def-const.h>
/* <pfe/def-config.h> */
#include <pfe/def-paths.h>
/* <pfe/def-config.h> */
#include <pfe/def-regs.h>
/* <pfe/def-config.h> */
/* <pfe/def-regmacro.h> */
#include <pfe/def-macro.h>
/* <pfe/def-config.h> */
/* <pfe/def-const.h> */
/* <pfe/def-regmacro.h> */

#include <pfe/def-limits.h>
/* <limits.h> ndef CHAR_BIT */

#include <pfe/def-pth.h>
/* <pfe/def-config.h> */
/* <semLib.h> VxWorks */
/* <taskLib.h> VxWorks */
/* <pthread.h> Posix2 */
/* <pth.h> libpth */
/* <SDL.h> libSDL */

#include <pfe/def-restore.h>
/* <pfe/def-regs.h> */

#include <pfe/pfe-sub.h>
/* <pfe/def-config.h> */
/* <pfe/def-regs.h> */
/* <pfe/def-const.h> */
/* <pfe/def-comp.h> */
/* --------------------------------------------- */

#include <pfe/def-comp.h>
/* <pfe/pfe-sub.h> */
/* <pfe/def-comp-c89.h> BorlandC */

#include <pfe/def-types.h>
/* <pfe/pfe-sub.h> */
/* <stdio.h> */
/* <pfe/os-setjmp.h> <setjmp.h> <pfe/def-config.h> */
/* <sys/types.h> */
/* <pfe/def-macro.h> */
/* <pfe/def-paths.h> */
/* <pfe/lined.h> */

#include <pfe/def-words.h>
/* <pfe/pfe-sub.h> */
/* <pfe/def-macro.h> */
/* <pfe/def-const.h> */

#include <pfe/pfe-ext.h>
/* <pfe/pfe-sub.h> */
/* <pfe/def-types.h> */
/* <pfe/def-macro.h> */
/* <pfe/def-words.h> */
/* --------------------------------------------- */

#include <pfe/_nonansi.h>
/* <pfe/def-config.h> */
/* <sys/ioctl.h> */
/* <fcntl.h> */
/* <sys/stat.h> */
/* <direct.h> */
/* <sys/ndir.h> */
/* <sys/time.h> */
/* <sys/utime.h> */
/* <poll.h> */
/* <sys/poll.h> */
/* <sys/select.h> */
/* <process.h> */
/* <sys/process.h> */
/* <unistd.h> */
/* <io.h> */
/* <libc.h> */
/* <conio.h> */
/* <dos.h> */
/* <pfe/_missing.h> */

#include <pfe/_missing.h>
/* <pfe/_config.h> */
/* <sys/stat.h> */
/* <fcntl.h> */
/* <io.h> */

#if PFE_PFE_CONFIGVERSION < 200000
volatile char PFE_00;
#elif PFE_PFE_CONFIGVERSION < 210000
volatile char PFE_21;
#elif PFE_PFE_CONFIGVERSION < 220000
volatile char PFE_22;
#elif PFE_PFE_CONFIGVERSION < 230000
volatile char PFE_23;
#elif PFE_PFE_CONFIGVERSION < 240000
volatile char PFE_24;
#elif PFE_PFE_CONFIGVERSION < 250000
volatile char PFE_25;
#elif PFE_PFE_CONFIGVERSION < 260000
volatile char PFE_26;
#elif PFE_PFE_CONFIGVERSION < 270000
volatile char PFE_27;
#elif PFE_PFE_CONFIGVERSION < 280000
volatile char PFE_28;
#elif PFE_PFE_CONFIGVERSION < 290000
volatile char PFE_29;
#elif PFE_PFE_CONFIGVERSION < 300000
volatile char PFE_30;
#elif PFE_PFE_CONFIGVERSION < 310000
volatile char PFE_31;
#elif PFE_PFE_CONFIGVERSION < 310000
volatile char PFE_31;
#elif PFE_PFE_CONFIGVERSION < 320000
volatile char PFE_32;
#elif PFE_PFE_CONFIGVERSION < 330000
volatile char PFE_33;
#elif PFE_PFE_CONFIGVERSION < 340000
volatile char PFE_34;
#elif PFE_PFE_CONFIGVERSION < 350000
volatile char PFE_35;
#elif PFE_PFE_CONFIGVERSION < 360000
volatile char PFE_36;
#elif PFE_PFE_CONFIGVERSION < 370000
volatile char PFE_37;
#elif PFE_PFE_CONFIGVERSION < 380000
volatile char PFE_38;
#elif PFE_PFE_CONFIGVERSION < 390000
volatile char PFE_39;
#else
volatile char PFE_99;
#endif
