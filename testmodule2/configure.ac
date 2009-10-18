dnl use `aclocal && autoconf` to compile this script correctly into "configure"
dnl and remember to run `libtoolize --force --copy` to get the libtoo files.
dnl btw, the first few lines are mandatory, modify name/version, always!!!

AC_INIT(testmodule2-ext.c,testmodule2,0.2.0)
AC_PREREQ(2.49)
AC_COPYRIGHT([Guido U. Draheim <guidod@gmx.de> for PFE.sourceforge.net])
AC_REVISION($Revision: 1.12 $)
AC_CONFIG_AUX_DIR([../config/aux])
AC_CONFIG_MACRO_DIR([../config/m4])
AC_PROG_CC
AM_PROG_LIBTOOL
AC_PROG_INSTALL

PKG_CONFIG_PATH="$PKG_CONFIG_PATH:../src/lib/pfe" ; export PKG_CONFIG_PATH
PKG_CHECK_MODULES(PFE,[pfe > 0.33.10],,[
PKG_CONFIG="$SHELL ../src/lib/pfe/pfe-uninstalled.sh"
AC_MSG_RESULT(fallback for pkg-config... $PKG_CONFIG)
PFE_CFLAGS=`$PKG_CONFIG --cflags`
PFE_LIBS=`$PKG_CONFIG --libs` ])

PFE_VERSION=`${PKG_CONFIG} pfe --modversion`
PFE_INCLUDEDIR=`${PKG_CONFIG} pfe --variable=includedir`
PFE_DATADIR=`${PKG_CONFIG} pfe --variable=prefix`/share/pfe
PFE_LIBDIR=`${PKG_CONFIG} pfe --variable=libdir`/pfe
PFE_SUFFIX=`${PKG_CONFIG} pfe --variable=suffix`
test -n "$pfemoduleslibdir" && PFE_LIBDIR="$pfemoduleslibdir/pfe"
AC_SUBST([PFE_VERSION])
AC_SUBST([PFE_INCLUDEDIR])
AC_SUBST([PFE_DATADIR])
AC_SUBST([PFE_LIBDIR])
AC_SUBST([PFE_SUFFIX])
AC_MSG_RESULT([installs into... $PFE_LIBDIR$PFE_SUFFIX])

AC_OUTPUT(Makefile)
