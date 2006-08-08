dnl use `aclocal && autoconf` to compile this script correctly into "configure"
dnl and remember to run `libtoolize --force --copy` to get the libtoo files.
dnl btw, the first few lines are mandatory, modify name/version, always!!!

AC_INIT(testmodule2-ext.c,testmodule2,0.2.0)
AC_CONFIG_AUX_DIR([../uses])
AC_PROG_CC
AM_PROG_LIBTOOL
AC_PROG_INSTALL

PKG_CONFIG_PATH="$PKG_CONFIG_PATH:../pfe" ; export PKG_CONFIG_PATH 
PKG_CHECK_MODULES(PFE,[pfe > 0.33.10],,[
PKG_CONFIG="$SHELL ../pfe/pfe-uninstalled.sh"
AC_MSG_RESULT(fallback for pkg-config... $PKG_CONFIG)
PFE_CFLAGS=`$PKG_CONFIG --cflags`
PFE_LIBS=`$PKG_CONFIG --libs` ])

PFE_VERSION=`${PKG_CONFIG} pfe --modversion`
PFE_INCLUDEDIR=`${PKG_CONFIG} pfe --variable=includedir`
PFE_DATADIR=`${PKG_CONFIG} pfe --variable=prefix`/share/pfe
PFE_LIBDIR=`${PKG_CONFIG} pfe --variable=exec_prefix`/lib/pfe
PFE_SUFFIX=`${PKG_CONFIG} pfe --variable=suffix`
AC_SUBST([PFE_VERSION PFE_INCLUDEDIR PFE_DATADIR PFE_LIBDIR PFE_SUFFIX])
AC_MSG_RESULT([installs into... $PFE_LIBDIR$PFE_SUFFIX])

AC_OUTPUT(Makefile)
