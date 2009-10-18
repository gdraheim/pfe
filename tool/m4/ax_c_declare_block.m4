dnl /usr/share/aclocal/guidod/ax_c_declare_block.m4
dnl @synopsis AX_C_DECLARE_BLOCK
dnl
dnl @summary if C variables must be declared at the beginning of a block
dnl
dnl The macro will compile a test program to see whether the compiler does
dnl allow new variable declarations in the middle of a C statement block,
dnl i.e. after some non-declaration line. New compilers will allow that
dnl which makes the behave a bit more like C++ - the gcc did support it
dnl for quite a time already.
dnl
dnl #define DECLARE_BLOCK_NEEDED says they need to be at the beginning of
dnl of a statement block. Additionlly two defines DECLARE_BLOCK { and
dnl DECLARE_END } are being set. That makes it possible to do the following
dnl in your source code (which this macro is really made up for):
dnl
dnl  #define ___ DECLARE_BLOCK
dnl  #define ____ DECLARE_END
dnl
dnl int f() {
dnl   char buffer[1024];
dnl   fgets(buffer, 1024, stdin);
dnl   ___ int i; int ii = strlen(buffer);
dnl   for (i=0; i < ii; i++) { 
dnl      fputc(buffer[i], stdout); 
dnl   }____;
dnl }
dnl
dnl @category C
dnl @author Guido U. Draheim <guidod@gmx.de>
dnl @version 2006-08-10
dnl @licence GPLWithACException

AC_DEFUN([AX_C_DECLARE_BLOCK],[dnl
AC_CACHE_CHECK(
 [if C variables must be declared at the beginning of a block],
 ax_cv_c_declare_block,[
 AC_TRY_COMPILE([#include <stdio.h>
 int f() {
   char buffer[1024];
   fgets(buffer, 1024, stdin);
   int i; int ii = strlen(buffer);
   for (i=0; i < ii; i++) { 
     fputc(buffer[i], stdout);
   }
 }],
 [],
 ax_cv_c_declare_block=no, ax_cv_c_declare_block=yes)])
 if test "$ax_cv_c_declare_block" = yes; then
   AC_DEFINE([DECLARE_BLOCK_NEEDED],[1],
    [if C variables must be declared at the beginning of a block])
   AC_DEFINE([DECLARE_BLOCK],[{],
    [set to { if variable declarations need a block start before])
   AC_DEFINE([DECLARE_END],[}],
    [set to } if variable declarations need a block start before])
 fi
])

