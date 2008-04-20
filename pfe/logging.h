#ifndef __PFE_LOGGING_H
#define __PFE_LOGGING_H

/** 
 * -- PFE Log Message System
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2000.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2008-04-20 04:46:30 $)
 *
 *  @description
 * informational messages, warnings, errors, fatal errors and debug messages.
 *  
 * Only debug messages are intended to be removed from released projects.
 * All others stay in code and wait for their time to appear.
 *
 * These defintions are modelled to be shallow to the macros in K12/logging.h
 *
 */
/*@{*/
/*****************************************************************************/

#ifdef _K12_SOURCE
#  include <K12/logging.h>
#endif

/* this is the log-level at the same time - make 0 to disable all */
#ifndef P4_LOG                    /* USER-CONFIG */
#if defined(DEBUG)
#define P4_LOG 0xFFFFFFFF         /* just everything */
#else
#define P4_LOG 0x0E               /* ie. _warn _fail _fatal */
#endif
#endif

#if !P4_LOG
#define P4_LOG_NO_MSG
#endif

#ifndef __GNUC__
#ifndef __FUNCTION__
#define __FUNCTION__ "..."
#endif
#endif

/*****************************************************************************/
/* 4 resident predefined message types                                       */
/*****************************************************************************/
#ifdef _K12_SOURCE

#define P4_LOG_INFO     K12_LOG_INFO
#define P4_LOG_WARN     K12_LOG_WARNING
#define P4_LOG_FAIL     K12_LOG_SEVERE
#define P4_LOG_FATAL    K12_LOG_FATAL

#define P4_LOG_MESG     K12_LOG_XINFO_L(0)
#define P4_LOG_NOTE     K12_LOG_XINFO_L(1)
#define P4_LOG_ENTER    K12_LOG_XINFO_L(2)
#define P4_LOG_LEAVE    K12_LOG_XINFO_L(3)

#define P4_LOG_ALL      K12_LOG_ALL

#define P4_LOG_DEFAULT  K12_LOG_DEFAULT
#define P4_LOG_XINFO    K12_LOG_XINFO
#define P4_LOG_DEBUG    K12_LOG_DEBUG
#define P4_LOG_USER     K12_LOG_USER

#else

#define P4_LOG_INFO     (1UL<<0)   
#define P4_LOG_WARN     (1UL<<1)
#define P4_LOG_FAIL     (1UL<<2)
#define P4_LOG_FATAL    (1UL<<3)

#define P4_LOG_MESG     (1UL<<4)
#define P4_LOG_NOTE     (1UL<<5)
#define P4_LOG_ENTER    (1UL<<6)
#define P4_LOG_LEAVE    (1UL<<7)

#define P4_LOG_ALL      0xFFFFFFFFL   /* All Messages */

#define P4_LOG_DEFAULT  0x0000000FL   /* 4 standard groups */
#define P4_LOG_XINFO    0x000000F0L   /* 4 XInfo groups */
#define P4_LOG_DEBUG    0x00FFFF00L   /* 16 Debug groups */
#define P4_LOG_USER     0xFF000000L   /* 8 User groups */

#endif 

#ifdef _K12_SOURCE

#define P4_LOG_FA_ "FA."
#define P4_LOG_SE_ "SE."
#define P4_LOG_WA_ "WA."
#define P4_LOG_IN_ "IN."
#define P4_LOG_XI_ "XI."
#define P4_LOG_NT_ " !_"

#else

#define P4_LOG_FA_ "<CRIT "
#define P4_LOG_SE_ "<FAIL "
#define P4_LOG_WA_ "<WARN "
#define P4_LOG_IN_ "<INFO "
#define P4_LOG_XI_ "<HINT "
#define P4_LOG_NT_ "<NOTE "

#endif

/* The following Message groups shall contain informational messages with
** decreasing significance from 1 to 4. The messages shall be used in modules
** where it seems to be sane to tell the user lots of things about what the
** module is actually doing. Like -d option of make or -v option of various
** programs
*/

/*****************************************************************************/
/* Mask makros for dynamically used message types                            */
/*****************************************************************************/

/*  4 XINFO Groups */
#define P4_LOG_XINFO_M(a) (((a)<< 4) & P4_LOG_XINFO)
/* 16 DEBUG Groups */
#define P4_LOG_DEBUG_M(a) (((a)<< 8) & P4_LOG_DEBUG)
/*  8 USER Groups */
#define P4_LOG_USER_M(a)  (((a)<<24) & P4_LOG_USER)

#define P4_LOG_XINFO_L(a) ((0x00000010L << (a)) & P4_LOG_XINFO)
#define P4_LOG_DEBUG_L(a) ((0x00000100L << (a)) & P4_LOG_DEBUG)
#define P4_LOG_USER_L(a)  ((0x01000000L << (a)) & P4_LOG_USER)


/*****************************************************************************/
/* front end makros for generating log messages                              */
/*****************************************************************************/

#ifdef _K12_SOURCE
#  define p4_LogMask k12LogMask
#elif defined P4_LOG_VAR
#  define p4_LogMask P4_LOG_VAR
#else
#  define p4_LogMask P4_LOG
#endif

#define P4_LogGrpMatch(mask) (((mask) & p4_LogMask))

/* Don't wonder about the misuse of "while" in the following makros. We use
** this construct to avoid unpredictable results when using the makro with
** an "if" statement
*/

#ifndef P4_LOG_NO_MSG

#ifdef _K12_SOURCE

#define P4_LogDumpHex(mask, funcId, str, adr, len, mode) \
  { if(P4_LogGrpMatch(mask)) \
    { k12LogDumpHex(mask, funcId"> "str"\n", adr, len, mode); \
    } \
  } 

#define P4_LogMsg(mask, prefix, funcId, str) \
  { if(P4_LogGrpMatch(mask)) \
      k12LogMsg(mask, prefix"%s> "str"\n", (u32_t)(funcId), 0, 0, 0, 0, 0); \
  } 

#define P4_LogMsg1(mask, prefix, funcId, str, x1) \
  { if(P4_LogGrpMatch(mask)) \
      k12LogMsg(mask, prefix"%s> "str"\n", (u32_t)(funcId), (u32_t)(x1), 0, 0, 0, 0); \
  } 

#define P4_LogMsg2(mask, prefix, funcId, str, x1, x2) \
  { if(P4_LogGrpMatch(mask)) \
      k12LogMsg(mask, prefix"%s> "str"\n", (u32_t)(funcId), (u32_t)(x1), (u32_t)(x2), 0, 0, 0); \
  } 

#define P4_LogMsg3(mask, prefix, funcId, str, x1, x2, x3) \
  { if(P4_LogGrpMatch(mask)) \
      k12LogMsg(mask, prefix"%s> "str"\n", (u32_t)(funcId), (u32_t)(x1), (u32_t)(x2), (u32_t)(x3), 0, 0); \
  } 

#define P4_LogMsg4(mask, prefix, funcId, str, x1, x2, x3, x4) \
  { if(P4_LogGrpMatch(mask)) \
      k12LogMsg(mask, prefix"%s> "str"\n", (u32_t)(funcId), (u32_t)(x1), (u32_t)(x2), (u32_t)(x3),\
        (u32_t)(x4), 0); \
  } 

#define P4_LogMsg5(mask, prefix, funcId, str, x1, x2, x3, x4, x5) \
  { if(P4_LogGrpMatch(mask)) \
      k12LogMsg(mask, prefix"%s> "str"\n", (u32_t)(funcId), (u32_t)(x1), (u32_t)(x2), (u32_t)(x3),\
        (u32_t)(x4), (u32_t)(x5)); \
  } 

#else /* !_K12_SOURCE */
  
#define P4_LogDumpHex(mask, funcId, str, adr, len, mode) \
  { if(P4_LogGrpMatch(mask)) \
    { fprintf (stderr, funcId"> "str" %lx... [%i]\n", *(long*)adr, len); \
    } \
  } 

#define P4_LogMsg(mask, prefix, funcId, str) \
  { if(P4_LogGrpMatch(mask)) \
      fprintf (stderr, prefix"%s> "str"\n", funcId); \
  } 

#define P4_LogMsg1(mask, prefix, funcId, str, x1) \
  { if(P4_LogGrpMatch(mask)) \
      fprintf (stderr, prefix"%s> "str"\n", funcId, x1); \
  } 

#define P4_LogMsg2(mask, prefix, funcId, str, x1, x2) \
  { if(P4_LogGrpMatch(mask)) \
      fprintf (stderr, prefix"%s> "str"\n", funcId, x1, x2); \
  } 

#define P4_LogMsg3(mask, prefix, funcId, str, x1, x2, x3) \
  { if(P4_LogGrpMatch(mask)) \
      fprintf (stderr, prefix"%s> "str"\n", funcId, x1, x2, x3); \
  } 

#define P4_LogMsg4(mask, prefix, funcId, str, x1, x2, x3, x4) \
  { if(P4_LogGrpMatch(mask)) \
      fprintf (stderr, prefix"%s> "str"\n", funcId, x1, x2, x3, x4); \
  } 

#define P4_LogMsg5(mask, prefix, funcId, str, x1, x2, x3, x4, x5) \
  { if(P4_LogGrpMatch(mask)) \
      fprintf (stderr, prefix"%s> "str"\n", funcId, x1, x2, x3, x4, x5); \
  } 

#endif  /* !_K12_SOURCE */

#else

#define P4_LogDumpHex(mask, funcId, str, adr, len, mode)

#define P4_LogMsg(mask, prefix, funcId, str)
#define P4_LogMsg1(mask, prefix, funcId, str, x1)
#define P4_LogMsg2(mask, prefix, funcId, str, x1, x2)
#define P4_LogMsg3(mask, prefix, funcId, str, x1, x2, x3)
#define P4_LogMsg4(mask, prefix, funcId, str, x1, x2, x3, x4)
#define P4_LogMsg5(mask, prefix, funcId, str, x1, x2, x3, x4, x5)

#endif /* K12_LOG_NO_MSG */

/*****************************************************************************/
/* Info Message Makros for one to six parameters           */
/*****************************************************************************/

#ifndef P4_LOG_NO_INFO_MSG

#define P4_info_dump( str, adr, len, mode) \
  P4_LogDumpHex(P4_LOG_INFO, __FUNCTION__, str, adr, len, mode)

#define P4_info(str) \
  P4_LogMsg (P4_LOG_INFO, P4_LOG_IN_,__FUNCTION__, str)
#define P4_info1(str, x1) \
  P4_LogMsg1(P4_LOG_INFO, P4_LOG_IN_,__FUNCTION__, str, x1)
#define P4_info2(str, x1, x2) \
  P4_LogMsg2(P4_LOG_INFO, P4_LOG_IN_,__FUNCTION__, str, x1, x2)
#define P4_info3(str, x1, x2, x3) \
  P4_LogMsg3(P4_LOG_INFO, P4_LOG_IN_,__FUNCTION__, str, x1, x2, x3)
#define P4_info4(str, x1, x2, x3, x4) \
  P4_LogMsg4(P4_LOG_INFO, P4_LOG_IN_,__FUNCTION__, str, x1, x2, x3, x4)
#define P4_info5(str, x1, x2, x3, x4, x5) \
  P4_LogMsg5(P4_LOG_INFO, P4_LOG_IN_,__FUNCTION__, str, x1, x2, x3, x4, x5)
  
#else

#define P4_info_dump( str, adr, len, mode)

#define P4_info( str)
#define P4_info1( str, x1)
#define P4_info2( str, x1, x2)
#define P4_info3( str, x1, x2, x3)
#define P4_info4( str, x1, x2, x3, x4)
#define P4_info5( str, x1, x2, x3, x4, x5)

#endif /* P4_LOG_NO_INFO_MSG */

/*****************************************************************************/
/* Warning Message Makros for one to six parameters                          */
/*****************************************************************************/

#ifndef P4_LOG_NO_WARN_MSG

#define P4_warn_dump( str, adr, len, mode) \
  P4_LogDumpHex(P4_LOG_WARNING, __FUNCTION__, str, adr, len, mode)

#define P4_warn( str) \
  P4_LogMsg (P4_LOG_WARN, P4_LOG_WA_,__FUNCTION__, str)
#define P4_warn1( str, x1) \
  P4_LogMsg1(P4_LOG_WARN, P4_LOG_WA_,__FUNCTION__, str, x1)
#define P4_warn2( str, x1, x2) \
  P4_LogMsg2(P4_LOG_WARN, P4_LOG_WA_,__FUNCTION__, str, x1, x2)
#define P4_warn3( str, x1, x2, x3) \
  P4_LogMsg3(P4_LOG_WARN, P4_LOG_WA_,__FUNCTION__, str, x1, x2, x3)
#define P4_warn4( str, x1, x2, x3, x4) \
  P4_LogMsg4(P4_LOG_WARN, P4_LOG_WA_,__FUNCTION__, str, x1, x2, x3, x4)
#define P4_warn5( str, x1, x2, x3, x4, x5) \
  P4_LogMsg5(P4_LOG_WARN, P4_LOG_WA_,__FUNCTION__, str, x1, x2, x3, x4, x5)
  
#else

#define P4_warn_dump( str, adr, len, mode)

#define P4_warn( str)
#define P4_warn1( str, x1)
#define P4_warn2( str, x1, x2)
#define P4_warn3( str, x1, x2, x3)
#define P4_warn4( str, x1, x2, x3, x4)
#define P4_warn5( str, x1, x2, x3, x4, x5)

#endif /* P4_LOG_NO_WARN_MSG */

/*****************************************************************************/
/* Severe Error Message Makros for one to six parameters         */
/*****************************************************************************/

#ifndef P4_LOG_NO_FAIL_MSG

#define P4_fail_dump( str, adr, len, mode) \
  P4_LogDumpHex(P4_LOG_FAIL, __FUNCTION__, str, adr, len, mode)

#define P4_fail( str) \
  P4_LogMsg (P4_LOG_FAIL, P4_LOG_SE_,__FUNCTION__, str)
#define P4_fail1( str, x1) \
  P4_LogMsg1(P4_LOG_FAIL, P4_LOG_SE_,__FUNCTION__, str, x1)
#define P4_fail2( str, x1, x2) \
  P4_LogMsg2(P4_LOG_FAIL, P4_LOG_SE_,__FUNCTION__, str, x1, x2)
#define P4_fail3( str, x1, x2, x3) \
  P4_LogMsg3(P4_LOG_FAIL, P4_LOG_SE_,__FUNCTION__, str, x1, x2, x3)
#define P4_fail4( str, x1, x2, x3, x4) \
  P4_LogMsg4(P4_LOG_FAIL, P4_LOG_SE_,__FUNCTION__, str, x1, x2, x3, x4)
#define P4_fail5( str, x1, x2, x3, x4, x5) \
  P4_LogMsg5(P4_LOG_FAIL, P4_LOG_SE_,__FUNCTION__, str, x1, x2, x3, x4, x5)

#else

#define P4_fail_dump( str, adr, len, mode)

#define P4_fail( str)
#define P4_fail1( str, x1)
#define P4_fail2( str, x1, x2)
#define P4_fail3( str, x1, x2, x3)
#define P4_fail4( str, x1, x2, x3, x4)
#define P4_fail5( str, x1, x2, x3, x4, x5)

#endif /* P4_LOG_NO_SEVERE_MSG */

/*****************************************************************************/
/* Fatal Error Message Makros for one to six parameters          */
/*****************************************************************************/

#ifndef P4_LOG_NO_FATAL_MSG

#define P4_fatal_dump(str, adr, len, mode) \
  P4_LogDumpHex(P4_LOG_FATAL, __FUNCTION__, str, adr, len, mode)

#define P4_fatal(str) \
  P4_LogMsg (P4_LOG_FATAL, P4_LOG_FA_,__FUNCTION__, str)
#define P4_fatal1(str, x1) \
  P4_LogMsg1(P4_LOG_FATAL, P4_LOG_FA_,__FUNCTION__, str, x1)
#define P4_fatal2(str, x1, x2) \
  P4_LogMsg2(P4_LOG_FATAL, P4_LOG_FA_,__FUNCTION__, str, x1, x2)
#define P4_fatal3(str, x1, x2, x3) \
  P4_LogMsg3(P4_LOG_FATAL, P4_LOG_FA_,__FUNCTION__, str, x1, x2, x3)
#define P4_fatal4(str, x1, x2, x3, x4) \
  P4_LogMsg4(P4_LOG_FATAL, P4_LOG_FA_,__FUNCTION__, str, x1, x2, x3, x4)
#define P4_fatal5(str, x1, x2, x3, x4, x5) \
  P4_LogMsg5(P4_LOG_FATAL, P4_LOG_FA_,__FUNCTION__, str, x1, x2, x3, x4, x5)

#else

#define P4_fatal_dump(str, adr, len, mode)

#define P4_fatal(str)
#define P4_fatal1(str, x1)
#define P4_fatal2(str, x1, x2)
#define P4_fatal3(str, x1, x2, x3)
#define P4_fatal4(str, x1, x2, x3, x4)
#define P4_fatal5(str, x1, x2, x3, x4, x5)

#endif /* P4_LOG_NO_FATAL_MSG */

/*****************************************************************************/
/* Function Enter/Leave Message Makros for one to six parameters          */
/*****************************************************************************/

#ifndef P4_LOG_NO_XINFO_MSG

#define P4_mesg_dump( str, adr, len, mode) \
  P4_LogDumpHex(P4_LOG_MESG, __FUNCTION__, str, adr, len, mode)

#define P4_mesg( str) \
  P4_LogMsg (P4_LOG_MESG, P4_LOG_XI_,__FUNCTION__, str)
#define P4_mesg1( str, x1) \
  P4_LogMsg1(P4_LOG_MESG, P4_LOG_XI_,__FUNCTION__, str, x1)
#define P4_mesg2( str, x1, x2) \
  P4_LogMsg2(P4_LOG_MESG, P4_LOG_XI_,__FUNCTION__, str, x1, x2)
#define P4_mesg3( str, x1, x2, x3) \
  P4_LogMsg3(P4_LOG_MESG, P4_LOG_XI_,__FUNCTION__, str, x1, x2, x3)
#define P4_mesg4( str, x1, x2, x3, x4) \
  P4_LogMsg4(P4_LOG_MESG, P4_LOG_XI_,__FUNCTION__, str, x1, x2, x3, x4)
#define P4_mesg5( str, x1, x2, x3, x4, x5) \
  P4_LogMsg5(P4_LOG_MESG, P4_LOG_XI_,__FUNCTION__, str, x1, x2, x3, x4, x5)

#else

#define P4_mesg_dump( str, adr, len, mode)

#define P4_mesg( str)
#define P4_mesg1( str, x1)
#define P4_mesg2( str, x1, x2)
#define P4_mesg3( str, x1, x2, x3)
#define P4_mesg4( str, x1, x2, x3, x4)
#define P4_mesg5( str, x1, x2, x3, x4, x5)

#endif /* P4_LOG_NO_XINFO_MSG - mesg* part */

#ifndef P4_LOG_NO_XINFO_MSG

#define P4_note_dump( str, adr, len, mode) \
  P4_LogDumpHex(P4_LOG_NOTE, __FUNCTION__, str, adr, len, mode)

#define P4_note( str) \
  P4_LogMsg (P4_LOG_NOTE, P4_LOG_NT_,__FUNCTION__, str)
#define P4_note1( str, x1) \
  P4_LogMsg1(P4_LOG_NOTE, P4_LOG_NT_,__FUNCTION__, str, x1)
#define P4_note2( str, x1, x2) \
  P4_LogMsg2(P4_LOG_NOTE, P4_LOG_NT_,__FUNCTION__, str, x1, x2)
#define P4_note3( str, x1, x2, x3) \
  P4_LogMsg3(P4_LOG_NOTE, P4_LOG_NT_,__FUNCTION__, str, x1, x2, x3)
#define P4_note4( str, x1, x2, x3, x4) \
  P4_LogMsg4(P4_LOG_NOTE, P4_LOG_NT_,__FUNCTION__, str, x1, x2, x3, x4)
#define P4_note5( str, x1, x2, x3, x4, x5) \
  P4_LogMsg5(P4_LOG_NOTE, P4_LOG_NT_,__FUNCTION__, str, x1, x2, x3, x4, x5)

#else

#define P4_note_dump( str, adr, len, mode)

#define P4_note( str)
#define P4_note1( str, x1)
#define P4_note2( str, x1, x2)
#define P4_note3( str, x1, x2, x3)
#define P4_note4( str, x1, x2, x3, x4)
#define P4_note5( str, x1, x2, x3, x4, x5)

#endif /* P4_LOG_NO_XINFO_MSG - note* part */

#ifndef P4_LOG_NO_XINFO_MSG

#define P4_enter_dump( str, adr, len, mode) \
  P4_LogDumpHex(P4_LOG_ENTER, __FUNCTION__, str, adr, len, mode)

#define P4_enter( str) \
  P4_LogMsg (P4_LOG_ENTER, ">>:",__FUNCTION__, str)
#define P4_enter1( str, x1) \
  P4_LogMsg1(P4_LOG_ENTER, ">>:",__FUNCTION__, str, x1)
#define P4_enter2( str, x1, x2) \
  P4_LogMsg2(P4_LOG_ENTER, ">>:",__FUNCTION__, str, x1, x2)
#define P4_enter3( str, x1, x2, x3) \
  P4_LogMsg3(P4_LOG_ENTER, ">>:",__FUNCTION__, str, x1, x2, x3)
#define P4_enter4( str, x1, x2, x3, x4) \
  P4_LogMsg4(P4_LOG_ENTER, ">>:",__FUNCTION__, str, x1, x2, x3, x4)
#define P4_enter5( str, x1, x2, x3, x4, x5) \
  P4_LogMsg5(P4_LOG_ENTER, ">>:",__FUNCTION__, str, x1, x2, x3, x4, x5)

#else

#define P4_enter_dump( str, adr, len, mode)

#define P4_enter( str)
#define P4_enter1( str, x1)
#define P4_enter2( str, x1, x2)
#define P4_enter3( str, x1, x2, x3)
#define P4_enter4( str, x1, x2, x3, x4)
#define P4_enter5( str, x1, x2, x3, x4, x5)

#endif /* P4_LOG_NO_XINFO_MSG - enter* part */

#ifndef P4_LOG_NO_XINFO_MSG

#define P4_leave_dump( str, adr, len, mode) \
  P4_LogDumpHex(P4_LOG_LEAVE, __FUNCTION__, str, adr, len, mode)

#define P4_leave( str) \
  P4_LogMsg (P4_LOG_LEAVE, "<<:",__FUNCTION__, str)
#define P4_leave1( str, x1) \
  P4_LogMsg1(P4_LOG_LEAVE, "<<:",__FUNCTION__, str, x1)
#define P4_leave2( str, x1, x2) \
  P4_LogMsg2(P4_LOG_LEAVE, "<<:",__FUNCTION__, str, x1, x2)
#define P4_leave3( str, x1, x2, x3) \
  P4_LogMsg3(P4_LOG_LEAVE, "<<:",__FUNCTION__, str, x1, x2, x3)
#define P4_leave4( str, x1, x2, x3, x4) \
  P4_LogMsg4(P4_LOG_LEAVE, "<<:",__FUNCTION__, str, x1, x2, x3, x4)
#define P4_leave5( str, x1, x2, x3, x4, x5) \
  P4_LogMsg5(P4_LOG_LEAVE, "<<:",__FUNCTION__, str, x1, x2, x3, x4, x5)

#else

#define P4_leave_dump( str, adr, len, mode)

#define P4_leave( str)
#define P4_leave1( str, x1)
#define P4_leave2( str, x1, x2)
#define P4_leave3( str, x1, x2, x3)
#define P4_leave4( str, x1, x2, x3, x4)
#define P4_leave5( str, x1, x2, x3, x4, x5)

#endif /* P4_LOG_NO_XINFO_MSG - leave* part */

#ifndef P4_LOG_NO_XINFO_MSG

#define P4_return_dump( str, adr, len, mode) { \
  P4_LogDumpHex(P4_LOG_LEAVE, __FUNCTION__, str, adr, len, mode); \
  return ; }

#define P4_return( str) { \
  P4_LogMsg (P4_LOG_LEAVE, "<<:",__FUNCTION__, str); return; }
#define P4_return1( str, x1) { \
  P4_LogMsg1(P4_LOG_LEAVE, "<<:",__FUNCTION__, str, x1); return; }
#define P4_return2( str, x1, x2) { \
  P4_LogMsg2(P4_LOG_LEAVE, "<<:",__FUNCTION__, str, x1, x2); return; }
#define P4_return3( str, x1, x2, x3) { \
  P4_LogMsg3(P4_LOG_LEAVE, "<<:",__FUNCTION__, str, x1, x2, x3); return; }
#define P4_return4( str, x1, x2, x3, x4) { \
  P4_LogMsg4(P4_LOG_LEAVE, "<<:",__FUNCTION__, str, x1, x2, x3, x4); return; }
#define P4_return5( str, x1, x2, x3, x4, x5) { \
  P4_LogMsg5(P4_LOG_LEAVE, "<<:",__FUNCTION__, str, x1, x2, x3, x4, x5); return; }

#else

#define P4_return_dump( str, adr, len, mode) return

#define P4_return( str) return
#define P4_return1( str, x1) return
#define P4_return2( str, x1, x2) return
#define P4_return3( str, x1, x2, x3) return
#define P4_return4( str, x1, x2, x3, x4) return
#define P4_return5( str, x1, x2, x3, x4, x5) return

#endif /* P4_LOG_NO_XINFO_MSG - return* part */

#ifndef P4_LOG_NO_XINFO_MSG

#define P4_returns_dump( a, str, adr, len, mode) { \
  P4_LogDumpHex(P4_LOG_LEAVE, __FUNCTION__, str, adr, len, mode); \
  return a ; }

#define P4_returns( a, str) { \
  P4_LogMsg (P4_LOG_LEAVE, "<<:",__FUNCTION__, str); return a; }
#define P4_returns1( a, str, x1) { \
  P4_LogMsg1(P4_LOG_LEAVE, "<<:",__FUNCTION__, str, x1); return a; }
#define P4_returns2( a, str, x1, x2) { \
  P4_LogMsg2(P4_LOG_LEAVE, "<<:",__FUNCTION__, str, x1, x2); return a; }
#define P4_returns3( a, str, x1, x2, x3) { \
  P4_LogMsg3(P4_LOG_LEAVE, "<<:",__FUNCTION__, str, x1, x2, x3); return a; }
#define P4_returns4( a, str, x1, x2, x3, x4) { \
  P4_LogMsg4(P4_LOG_LEAVE, "<<:",__FUNCTION__, str, x1, x2, x3, x4); return a; }
#define P4_returns5( a, str, x1, x2, x3, x4, x5) { \
  P4_LogMsg5(P4_LOG_LEAVE, "<<:",__FUNCTION__, str, x1, x2, x3, x4, x5); return a; }

#else

#define P4_returns_dump( str, adr, len, mode) return

#define P4_returns( a, str) return a
#define P4_returns1( a, str, x1) return a
#define P4_returns2( a, str, x1, x2) return a
#define P4_returns3( a, str, x1, x2, x3) return a
#define P4_returns4( a, str, x1, x2, x3, x4) return a
#define P4_returns5( a, str, x1, x2, x3, x4, x5) return a

#endif /* P4_LOG_NO_XINFO_MSG - returns* part */

/*****************************************************************************/
/* Debug Messages for one to six parameters            */
/*****************************************************************************/

#ifndef P4_LOG_NO_DEBUG_MSG

#define P4_debug_dump(mask, str, adr, len, mode) \
  P4_LogDumpHex(P4_LOG_DEBUG_L(mask), __FUNCTION__, str, adr, len, mode)

#define P4_debug(mask, str) \
  P4_LogMsg (P4_LOG_DEBUG_L(mask), "DB.",__FUNCTION__, str)
#define P4_debug1(mask, str, x1) \
  P4_LogMsg1(P4_LOG_DEBUG_L(mask), "DB.",__FUNCTION__, str, x1)
#define P4_debug2(mask, str, x1, x2) \
  P4_LogMsg2(P4_LOG_DEBUG_L(mask), "DB.",__FUNCTION__, str, x1, x2)
#define P4_debug3(mask, str, x1, x2, x3) \
  P4_LogMsg3(P4_LOG_DEBUG_L(mask), "DB.",__FUNCTION__, str, x1, x2, x3)
#define P4_debug4(mask, str, x1, x2, x3, x4) \
  P4_LogMsg4(P4_LOG_DEBUG_L(mask), "DB.",__FUNCTION__, str, x1, x2, x3, x4)
#define P4_debug5(mask, str, x1, x2, x3, x4, x5) \
  P4_LogMsg5(P4_LOG_DEBUG_L(mask), "DB.",__FUNCTION__, str, x1, x2, x3, x4, x5)

#else

#define P4_debug_dump(mask, str, adr, len, mode)

#define P4_debug(mask, str)
#define P4_debug1(mask, str, x1)
#define P4_debug2(mask, str, x1, x2)
#define P4_debug3(mask, str, x1, x2, x3)
#define P4_debug4(mask, str, x1, x2, x3, x4)
#define P4_debug5(mask, str, x1, x2, x3, x4, x5)

#endif /* P4_LOG_NO_DEBUG_MSG */

/*****************************************************************************/
/* Debug Messages for one to six parameters            */
/*****************************************************************************/

#ifndef P4_LOG_NO_MSG

#define P4_log_dump(mask, str, adr, len, mode) \
  P4_LogDumpHex(P4_LOG_USER_L(mask), __FUNCTION__, str, adr, len, mode)

#define P4_log(mask, str) \
  P4_LogMsg (P4_LOG_USER_L(mask), "US.",__FUNCTION__, str)
#define P4_log1(mask, str, x1) \
  P4_LogMsg1(P4_LOG_USER_L(mask), "US.",__FUNCTION__, str, x1)
#define P4_log2(mask, str, x1, x2) \
  P4_LogMsg2(P4_LOG_USER_L(mask), "US.",__FUNCTION__, str, x1, x2)
#define P4_log3(mask, str, x1, x2, x3) \
  P4_LogMsg3(P4_LOG_USER_L(mask), "US.",__FUNCTION__, str, x1, x2, x3)
#define P4_log4(mask, str, x1, x2, x3, x4) \
  P4_LogMsg4(P4_LOG_USER_L(mask), "US.",__FUNCTION__, str, x1, x2, x3, x4)
#define P4_log5(mask, str, x1, x2, x3, x4, x5) \
  P4_LogMsg5(P4_LOG_USER_L(mask), "US.",__FUNCTION__, str, x1, x2, x3, x4, x5)

#else

#define P4_log_dump(mask, str, adr, len, mode)

#define P4_log(mask, str)
#define P4_log1(mask, str, x1)
#define P4_log2(mask, str, x1, x2)
#define P4_log3(mask, str, x1, x2, x3)
#define P4_log4(mask, str, x1, x2, x3, x4)
#define P4_log5(mask, str, x1, x2, x3, x4, x5)

#endif /* P4_LOG_NO_DEBUG_MSG */

/******************************************************************************
** Makros instead of Real Prototypes to remove the calls with P4_LOG_NO_MSG
******************************************************************************/
#if defined(P4_LOG_NO_MSG) || ! defined(_K12_SOURCE)

#define P4_LogInit(logFile, maxMsg, logMode)
#define P4_LogOptionSet(msgMask, msgFilter, tId, xflg)
#define P4_LogOptionSetN(msgMask, msgFilter, tname, xflg)
#define P4_LogOptionGet(pMsgMask, msgFilter, ptId, pxflg)
#define P4_LogOptionShow()

#else

#define P4_LogInit(logFile, maxMsg, logMode) \
  k12LogInit(logFile, maxMsg, logMode)
#define P4_LogOptionSet(msgMask, msgFilter, tId, xflg) \
  k12LogOptionSet(msgMask, msgFilter, tId, xflg)
#define P4_LogOptionSetN(msgMask, msgFilter, tname, xflg) \
  k12LogOptionSetN(msgMask, msgFilter, tname, xflg)
#define P4_LogOptionGet(pMsgMask, msgFilter, ptId, pxflg) \
  k12LogOptionGet(pMsgMask, msgFilter, ptId, pxflg)
#define P4_LogOptionShow() \
  k12LogOptionShow()
  
#endif /* P4_LOG_NO_MSG */

/*@}*/
#endif
