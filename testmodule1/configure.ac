# use `aclocal && autoconf` to compile this script correctly into "configure"
# the first few lines are mandatory, modify name/version, always!!!
AC_INIT(testmodule1-ext.c)
AC_CONFIG_AUX_DIR([../uses])
AC_PROG_CC
AM_INIT_AUTOMAKE(testmodule1,0.2.0)
AM_PROG_LIBTOOL
AC_PROG_INSTALL

PKG_CONFIG_PATH="$PKG_CONFIG_PATH:../pfe" ; export PKG_CONFIG_PATH 
PKG_CHECK_MODULES(PFE,[pfe > 0.33.10],,[
PKG_CONFIG="$SHELL ../pfe/pfe-uninstalled.sh"
AC_MSG_RESULT(fallback for pkg-config... $PKG_CONFIG)
PFE_CFLAGS=`$PKG_CONFIG --cflags`
PFE_LIBS=`$PKG_CONFIG --libs` ])

AC_MSG_CHECKING(for pfe version) # just an extra status message
v=`$PKG_CONFIG pfe --modversion 2>/dev/null`
if $PKG_CONFIG pfe --uninstalled 2>/dev/null ; then v="$v (uninstalled)" ; fi
test ".$v" != "." || v="(fallback) (no)"
AC_MSG_RESULT($v)

# the following will only allow us to put a dependency in the Makefile
# so that a local module-dll.c can be regenerated from the template.
AC_MSG_CHECKING(pfeincludedir)
pfeincludedir=`pkg-config pfe --variable=includedir 2>/dev/null`
v="" ; case ".$pfeincludedir" in .) v="(fallback$pfeincludedir) "
pfeincludedir='${srcdir}/..' ;; esac
AC_MSG_RESULT($v$pfeincludedir)
AC_SUBST(pfeincludedir)

# the pfelibdir / pfedatadir map to the pkglibdir / pkgdatadir in pfe sources
AC_MSG_CHECKING(pfedatadir)
pfedatadir=`pkg-config pfe --variable=prefix 2>/dev/null`
v="" ; case ".$pfedatadir" in .) v="(fallback$pfedatadir) "
pfedatadir='${prefix}' ;; esac
pfedatadir="$pfedatadir/share/pfe"
AC_MSG_RESULT($v$pfedatadir)
AC_SUBST(pfedatadir)

AC_MSG_CHECKING(pfelibdir)
PFE_LIBDIR=`pkg-config pfe --variable=exec_prefix 2>/dev/null`
PFE_SUFFIX=`pkg-config pfe --variable=suffix 2>/dev/null`
v="" ; case ".$PFE_LIBDIR" in .) v="(fallback$PFE_LIBDIR) "
PFE_LIBDIR='${exec_prefix}' ;; esac
PFE_LIBDIR="$PFE_LIBDIR/lib/pfe"
AC_MSG_RESULT($v$PFE_LIBDIR)
AC_SUBST([PFE_LIBDIR PFE_SUFFIX])

AC_OUTPUT(Makefile)
# optional patches that fix problems in older libtool versions...
# _PATCH_LIBTOOL_ON_DARWIN_ZSH_OVERQUOTING
# _PATCH_LIBTOOL_SYS_LIB_SEARCH_PATH_SPEC
# _PATCH_LIBTOOL_TO_ADD_HOST_CC
# _PATCH_LIBTOOL_ON_DARWIN_PASS_ALL
# _PATCH_LIBTOOL_CHANGING_CMDS_IFS
