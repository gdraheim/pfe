dnl @synopsis AX_CHECK_GNU_DLADDR
dnl
dnl Check if this system has a dladdr in dlfcn.h which is atleast
dnl the case for gnuish systems. Define HAVE_GNU_DLADDR if yes.
dnl
dnl @author Guiod U. Draheim
dnl @version 2002
dnl @category C

AC_DEFUN([AX_CHECK_GNU_DLADDR],[
AC_MSG_CHECKING(for gnu dladdr)
AC_TRY_COMPILE(
  [
#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif
#include <dlfcn.h>  
  ],[
Dl_info* info = 0;
dladdr(0, info);  
  ],[
AC_DEFINE([HAVE_GNU_DLADDR],[1],[gnuish system might have a dladdr in dlfcn.h])
AC_MSG_RESULT(yes)
  ],[
AC_MSG_RESULT(no)
  ])
])
