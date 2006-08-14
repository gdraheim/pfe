AC_INIT([pfe.spec])
AX_SPEC_DEFAULTS
AC_PREREQ([2.52])
AC_COPYRIGHT([Guido U. Draheim <guidod@gmx.de> for PFE.sourceforge.net])
AC_REVISION($Revision: 1.3 $)
AC_CONFIG_AUX_DIR([uses])
AX_ENABLE_BUILDDIR_UNAME

AC_SET_DEFAULT_PATHS_SYSTEM
# NOTE: gcc 3.0...3.2.2 compiles incorrect code for -fomit-frame-pointer !
if test -z "$CC" ; then for i in 2.96 2.95.3 2.95.2 2.95 ; do
if test -f /usr/bin/gcc-$i ; then CC=gcc-$i ; export CC ; break; fi done fi
# ........................................................................
AC_ARG_WITH(call-threading, 
[  --with-call-threading   use call-threading and optimizations for it]) 
AC_COND_WITH_DEFINE(call-threading)

AC_ARG_WITH(sbr-call-threading, 
[  --with-sbr-call-threading  try sbr-threading on top of call-threading]) 
AC_COND_WITH_DEFINE(sbr-call-threading)

AC_ARG_WITH(sbr-threading, 
[  --with-sbr-threading    or try sbr-threading with argument prefixing]) 
AC_COND_WITH_DEFINE(sbr-threading)

AC_ARG_WITH(sbr-static, 
[  --with-sbr-static       or the fastest variant that is available]) 
AC_COND_WITH_DEFINE(sbr-threading)

AC_ARG_WITH(modules, 
[  --without-modules       do not compile the external modules (default=yes)]) 
AC_COND_WITH_DEFINE(modules,yes)

VARIANT=""

if test ".$with_call_threading" = ".yes" ; then 
VARIANT="-call"
elif test ".$with_sbr_call_threading" = ".yes" ; then 
VARIANT="-calls"
ac_configure_args="$ac_configure_args --disable-shared"
elif test ".$with_sbr_threading" = ".yes" ; then 
VARIANT="-fast"
elif test ".$with_sbr_static" = ".yes" ; then 
VARIANT="-fastest"
ac_configure_args="$ac_configure_args --with-static-regs"
ac_configure_args="$ac_configure_args --with-regs=all"
else
VARIANT="-forth"
fi

AC_ARG_WITH(variant, AC_HELP_STRING(
[--with-variant],[choose a predefined variant - along with a suffix
   (fig|regs|regs-forth|forth|call|calls|fast)]))
case ".$with_variant" in
  .fig)  VARIANT="-$with_variant"
  ac_configure_args="$ac_configure_args --with-fig=1" ;;
  .regs) VARIANT="-$with_variant"
  ac_configure_args="$ac_configure_args --with-regs=all" ;;
  .forth|.regs-forth) VARIANT="-regs-forth"
  ac_configure_args="$ac_configure_args --with-regs=all" 
  ac_configure_args="$ac_configure_args --without-modules" 
  ac_configure_args="$ac_configure_args --disable-shared" ;;
  .call) VARIANT="-$with_variant"
  ac_configure_args="$ac_configure_args --with-call-threading" ;;
  .calls) VARIANT="-$with_variant"
  ac_configure_args="$ac_configure_args --with-sbr-call-threading" ;;
  .calls-forth) VARIANT="-$with_variant"
  ac_configure_args="$ac_configure_args --with-sbr-call-threading"
  ac_configure_args="$ac_configure_args --without-modules" 
  ac_configure_args="$ac_configure_args --disable-shared" ;;
  .fast) VARIANT="-$with_variant"
  ac_configure_args="$ac_configure_args --with-sbr-threading" ;;
  .fast-forth) VARIANT="-fast-forth"
  ac_configure_args="$ac_configure_args --with-sbr-threading"
  ac_configure_args="$ac_configure_args --without-modules" 
  ac_configure_args="$ac_configure_args --disable-shared" ;;
  .sbr|.sbr-static|.fastest) VARIANT="-fastest"
  ac_configure_args="$ac_configure_args --with-sbr-static" ;;
  .sbr-forth|.fastest-forth) VARIANT="-fastest-forth"
  ac_configure_args="$ac_configure_args --with-sbr-static"
  ac_configure_args="$ac_configure_args --without-modules" 
  ac_configure_args="$ac_configure_args --disable-shared" ;;
esac
if test ".$VARIANT" != "." ; then
  ac_configure_args="$ac_configure_args --program-suffix=$VARIANT" 
fi
export VARIANT
# ........................................................................
AM_INIT_AUTOMAKE($PACKAGE, $VERSION)
AM_MAINTAINER_MODE
AC_CONFIG_SUBDIRS(pfe bin)
# ........................................................................
AC_MSG_CHECKING(extra modules)
EXTRAMODULES=""

 AC_ARG_WITH(testmodule,
[  --without-testmodule    wether to compile/install an external test module])
 test "$with_testmodule" != "no" && EXTRAMODULES="$EXTRAMODULES testmodule1"
 test "$with_testmodule" != "no" && EXTRAMODULES="$EXTRAMODULES testmodule2"

AC_MSG_RESULT(EXTRAMODULES = $EXTRAMODULES)
AC_SUBST(EXTRAMODULES)
dnl "$subdirs" is the internal name used by AC_CONFIG_SUBDIRS execution
for module in $EXTRAMODULES ; do
  AC_MSG_CHECKING(existing extra module $module)
  if test -d $srcdir/$module ; then
    AC_MSG_RESULT(ok)
    subdirs="$subdirs $module" 
  else
    AC_MSG_RESULT(NO sources)
  fi
done

# ........................................................................
AC_OUTPUT([Makefile lib/Makefile test/Makefile doc/Makefile]) 
AX_WARNING_DEFAULT_PKGCONFIGDIR
AC_MSG_NOTICE(mkdir dbk)
AS_MKDIR_P(dbk)
echo "# make && make check && make install"
