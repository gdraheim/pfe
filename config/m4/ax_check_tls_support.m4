dnl @synopsis AX_CHECK_TLS_SUPPORT
dnl
dnl Check whether operating system is assumed to support tls registers.
dnl The compiler will allow for the __thread keyword to assemble global
dnl variables into a per-thread stack block. That data block must be
dnl declared specifically in a binary / shared library export table to
dnl be copied on thread creation. - Defiens HAVE_TLS_SUPPORT if yes.
dnl
dnl Implementation note: we do currently just check the compiler version
dnl of a compiler/platform combination that is known to be good. There
dnl is no generic check at the moment. Someone?
dnl
dnl @author Guido U. Draheim
dnl @version 2005
dnl @category C

AC_DEFUN([AX_CHECK_TLS_SUPPORT],[
AC_MSG_CHECKING(tls support)
AC_CACHE_VAL([ac_cv_have_tls_support],[
case "$GCC_VERSION,$host" in
    2.*,*) ac_cv_have_tls_support="no" ;;
    3.0,*) ac_cv_have_tls_support="no" ;;
    3.*,*linux*) ac_cv_have_tls_suport="yes" ;;
    *)  ac_cv_have_tls_support="unkown (no)" ;;
esac])
AC_MSG_RESULT([$ac_cv_tls_support])
if test ".$ac_cv_have_tls_support" = ".yes" ; then
AC_DEFINE([HAVE_TLS_SUPPORT], [1], 
      [whether operating system is assumed to support tls registers])
fi
])
