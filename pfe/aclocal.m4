dnl aclocal.m4 generated automatically by aclocal 1.4

dnl Copyright (C) 1994, 1995-8, 1999 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY, to the extent permitted by law; without
dnl even the implied warranty of MERCHANTABILITY or FITNESS FOR A
dnl PARTICULAR PURPOSE.

dnl @synopsis AC_SET_DEFAULT_PATHS_DLLSYSTEM
dnl
dnl this macro diverts all subpaths to either /bin/.. or /share/..
dnl
dnl @version $Id: aclocal.m4,v 0.31 2001-03-19 21:50:41 guidod Exp $
dnl @author Guido Draheim <guidod@gmx.de>       STATUS: one time use
AC_DEFUN([AC_SET_DEFAULT_PATHS_DLLSYSTEM],
[AC_REQUIRE([AC_CANONICAL_HOST])
case ${host_os} in
  *cygwin* | *mingw* | *uwin* | *djgpp | *emx*)
     AC_MSG_RESULT(changing default paths for win/dos target...yes)
     test "$ac_default_prefix" = "/usr/local" && ac_default_prefix="/programs"
     # on win/dos, .exe .dll and .cfg live in the same directory
     bindir=`echo $bindir |sed -e '/^..exec_prefix/s:/bin$:/${PACKAGE}:'`
     libdir=`echo $libdir |sed -e 's:^..exec_prefix./lib$:${bindir}:'`
     sbindir=`echo $sbindir |sed -e 's:^..exec_prefix./sbin$:${bindir}:'`
     sysconfdir=`echo $sysconfdir |sed -e 's:^..prefix./etc$:${bindir}:'`
     libexecdir=`echo $libexecdir |sed -e 's:^..exec_prefix./libexec$:${bindir}/system:'`
     # help-files shall be set with --infodir
     # leave datadir as /share
     infodir=`echo $infodir |sed -e 's:^..prefix./info$:${datadir}/help:'`
     mandir=`echo $mandir |sed -e 's:^..prefix./man$:${datadir}/help:'`
     includedir=`echo $includedir |sed -e 's:..prefix./include$:${datadir}/include:'`
     sharedstatedir=`echo $sharedstatedir |sed -e 's:..prefix./com$:${datadir}/common:'`
     localstatedir=`echo $localstatedir |sed -e 's:..prefix./var$:${datadir}/local:'`
  ;;
esac
])     

# Do all the work for Automake.  This macro actually does too much --
# some checks are only needed if your package does certain things.
# But this isn't really a big deal.

# serial 1

dnl Usage:
dnl AM_INIT_AUTOMAKE(package,version, [no-define])

AC_DEFUN(AM_INIT_AUTOMAKE,
[AC_REQUIRE([AC_PROG_INSTALL])
PACKAGE=[$1]
AC_SUBST(PACKAGE)
VERSION=[$2]
AC_SUBST(VERSION)
dnl test to see if srcdir already configured
if test "`cd $srcdir && pwd`" != "`pwd`" && test -f $srcdir/config.status; then
  AC_MSG_ERROR([source directory already configured; run "make distclean" there first])
fi
ifelse([$3],,
AC_DEFINE_UNQUOTED(PACKAGE, "$PACKAGE", [Name of package])
AC_DEFINE_UNQUOTED(VERSION, "$VERSION", [Version number of package]))
AC_REQUIRE([AM_SANITY_CHECK])
AC_REQUIRE([AC_ARG_PROGRAM])
dnl FIXME This is truly gross.
missing_dir=`cd $ac_aux_dir && pwd`
AM_MISSING_PROG(ACLOCAL, aclocal, $missing_dir)
AM_MISSING_PROG(AUTOCONF, autoconf, $missing_dir)
AM_MISSING_PROG(AUTOMAKE, automake, $missing_dir)
AM_MISSING_PROG(AUTOHEADER, autoheader, $missing_dir)
AM_MISSING_PROG(MAKEINFO, makeinfo, $missing_dir)
AC_REQUIRE([AC_PROG_MAKE_SET])])

#
# Check to make sure that the build environment is sane.
#

AC_DEFUN(AM_SANITY_CHECK,
[AC_MSG_CHECKING([whether build environment is sane])
# Just in case
sleep 1
echo timestamp > conftestfile
# Do `set' in a subshell so we don't clobber the current shell's
# arguments.  Must try -L first in case configure is actually a
# symlink; some systems play weird games with the mod time of symlinks
# (eg FreeBSD returns the mod time of the symlink's containing
# directory).
if (
   set X `ls -Lt $srcdir/configure conftestfile 2> /dev/null`
   if test "[$]*" = "X"; then
      # -L didn't work.
      set X `ls -t $srcdir/configure conftestfile`
   fi
   if test "[$]*" != "X $srcdir/configure conftestfile" \
      && test "[$]*" != "X conftestfile $srcdir/configure"; then

      # If neither matched, then we have a broken ls.  This can happen
      # if, for instance, CONFIG_SHELL is bash and it inherits a
      # broken ls alias from the environment.  This has actually
      # happened.  Such a system could not be considered "sane".
      AC_MSG_ERROR([ls -t appears to fail.  Make sure there is not a broken
alias in your environment])
   fi

   test "[$]2" = conftestfile
   )
then
   # Ok.
   :
else
   AC_MSG_ERROR([newly created file is older than distributed files!
Check your system clock])
fi
rm -f conftest*
AC_MSG_RESULT(yes)])

dnl AM_MISSING_PROG(NAME, PROGRAM, DIRECTORY)
dnl The program must properly implement --version.
AC_DEFUN(AM_MISSING_PROG,
[AC_MSG_CHECKING(for working $2)
# Run test in a subshell; some versions of sh will print an error if
# an executable is not found, even if stderr is redirected.
# Redirect stdin to placate older versions of autoconf.  Sigh.
if ($2 --version) < /dev/null > /dev/null 2>&1; then
   $1=$2
   AC_MSG_RESULT(found)
else
   $1="$3/missing $2"
   AC_MSG_RESULT(missing)
fi
AC_SUBST($1)])

# Like AC_CONFIG_HEADER, but automatically create stamp file.

AC_DEFUN(AM_CONFIG_HEADER,
[AC_PREREQ([2.12])
AC_CONFIG_HEADER([$1])
dnl When config.status generates a header, we must update the stamp-h file.
dnl This file resides in the same directory as the config header
dnl that is generated.  We must strip everything past the first ":",
dnl and everything past the last "/".
AC_OUTPUT_COMMANDS(changequote(<<,>>)dnl
ifelse(patsubst(<<$1>>, <<[^ ]>>, <<>>), <<>>,
<<test -z "<<$>>CONFIG_HEADERS" || echo timestamp > patsubst(<<$1>>, <<^\([^:]*/\)?.*>>, <<\1>>)stamp-h<<>>dnl>>,
<<am_indx=1
for am_file in <<$1>>; do
  case " <<$>>CONFIG_HEADERS " in
  *" <<$>>am_file "*<<)>>
    echo timestamp > `echo <<$>>am_file | sed -e 's%:.*%%' -e 's%[^/]*$%%'`stamp-h$am_indx
    ;;
  esac
  am_indx=`expr "<<$>>am_indx" + 1`
done<<>>dnl>>)
changequote([,]))])

dnl @synopsis AC_DEFINE_VERSIONLEVEL(VARNAME [,VERSION [, DESCRIPTION]])
dnl
dnl if the VERSION is ommitted, shellvar $VERSION is used
dnl as defined by AM_INIT_AUTOMAKE's second argument.
dnl
dnl The versionlevel is the numeric representation of the given
dnl version string, thereby assuming the inputversion is
dnl a string with (maximal) three decimal numbers seperated
dnl by "."-dots. A "-patch" adds a percent.
dnl
dnl typical usage:
dnl AM_INIT_AUTOMAKE(mypkg,4.12.3)
dnl AC_DEFINE_VERSIONLEVEL(MYPKG_VERSION)
dnl
dnl the config.h created from autoheader's config.h.in will contain...
dnl /* mypkg versionlevel */
dnl #define MYPKG_VERSION 4120003
dnl
dnl the MYKG_VERSION will be defined as both a shell-variable and AC_DEFINE
dnl
dnl examples:
dnl        3.0-beta1     3000001
dnl        3.1           3010000
dnl        3.11          3110000
dnl        3.11-dirpatch 3111000
dnl        3.11-patch6   3110006
dnl        2.2.18        2020018
dnl        2.0.112       2000112
dnl        2.4.2         2040002
dnl        2.4.2-pre     2040003 
dnl        2.4.2-pre5    2040003
dnl        5.0-build125  5000125 
dnl        5.0           5000000
dnl        0.30.17       30017
dnl
dnl @version $Id: aclocal.m4,v 0.31 2001-03-19 21:50:41 guidod Exp $
dnl @author Guido Draheim <guidod@gmx.de>
dnl

AC_DEFUN(AC_DEFINE_VERSIONLEVEL,
[changequote(<<, >>)dnl
ac_versionlevel_strdf=`echo ifelse($2, , $VERSION, $2) | sed -e 's:[A-Z-]*:.:' -e 's:[^0-9.]::g' -e 's:^[.]*::'`
AC_MSG_CHECKING(versionlevel $ac_versionlevel_strdf)
case $ac_versionlevel_strdf in
 *.*.*.|*.*.*.*) :
 ac_versionlevel_major=`echo $ac_versionlevel_strdf`
 ac_versionlevel_minor=`echo $ac_versionlevel_major | sed -e 's/[^.]*[.]//'`
 ac_versionlevel_patch=`echo $ac_versionlevel_minor | sed -e 's/[^.]*[.]//'`
 ac_versionlevel_major=`echo $ac_versionlevel_major | sed -e 's/[.].*//'`
 ac_versionlevel_minor=`echo $ac_versionlevel_minor | sed -e 's/[.].*//'`
 ac_versionlevel_patch=`echo $ac_versionlevel_patch | sed -e 's/[.].*//'`
 $1=`expr $ac_versionlevel_major '*' 1000000 \
        + $ac_versionlevel_minor '*'   10000 \
        + $ac_versionlevel_patch \
	+ 1` ;;
 *.*.*) :
 ac_versionlevel_major=`echo $ac_versionlevel_strdf`
 ac_versionlevel_minor=`echo $ac_versionlevel_major | sed -e 's/[^.]*[.]//'`
 ac_versionlevel_patch=`echo $ac_versionlevel_minor | sed -e 's/[^.]*[.]//'`
 ac_versionlevel_major=`echo $ac_versionlevel_major | sed -e 's/[.].*//'`
 ac_versionlevel_minor=`echo $ac_versionlevel_minor | sed -e 's/[.].*//'`
 ac_versionlevel_patch=`echo $ac_versionlevel_patch | sed -e 's/[.].*//'`
 $1=`expr $ac_versionlevel_major '*' 1000000 \
        + $ac_versionlevel_minor '*'   10000 \
        + $ac_versionlevel_patch`               ;;
 *.*.) :
 ac_versionlevel_major=`echo $ac_versionlevel_strdf`
 ac_versionlevel_minor=`echo $ac_versionlevel_major | sed -e 's/[^.]*[.]//'`
 ac_versionlevel_major=`echo $ac_versionlevel_major | sed -e 's/[.].*//'`
 ac_versionlevel_minor=`echo $ac_versionlevel_minor | sed -e 's/[.].*//'`
 ac_versionlevel_patch=0
 $1=`expr $ac_versionlevel_major '*' 1000000 \
        + $ac_versionlevel_minor '*'   10000 \
	+ 1000 \
        + $ac_versionlevel_patch`               ;;
 *.*) :
 ac_versionlevel_major=`echo $ac_versionlevel_strdf`
 ac_versionlevel_minor=`echo $ac_versionlevel_major | sed -e 's/[^.]*[.]//'`
 ac_versionlevel_major=`echo $ac_versionlevel_major | sed -e 's/[.].*//'`
 ac_versionlevel_minor=`echo $ac_versionlevel_minor | sed -e 's/[.].*//'`
 ac_versionlevel_patch=0
 $1=`expr $ac_versionlevel_major '*' 1000000 \
        + $ac_versionlevel_minor '*'   10000 \
        + $ac_versionlevel_patch`               ;;
 *.) :
 ac_versionlevel_major=0
 ac_versionlevel_minor=`echo $ac_versionlevel_strdf`
 ac_versionlevel_minor=`echo $ac_versionlevel_minor | sed -e 's/[.].*//'`
 ac_versionlevel_patch=0
 $1=`expr $ac_versionlevel_major '*' 1000000 \
        + $ac_versionlevel_minor '*'   10000 \
	+ 1000 \
        + $ac_versionlevel_patch`               ;;
 *) :
 ac_versionlevel_major=0
 ac_versionlevel_minor=`echo $ac_versionlevel_strdf`
 ac_versionlevel_minor=`echo $ac_versionlevel_minor | sed -e 's/[.].*//'`
 ac_versionlevel_patch=0
 $1=`expr $ac_versionlevel_major '*' 1000000 \
        + $ac_versionlevel_minor '*'   10000 \
        + $ac_versionlevel_patch`               ;;
esac
changequote([, ])dnl
AC_MSG_RESULT($[$1])
AC_DEFINE_UNQUOTED( $1, $[$1], ifelse( $3, , $PACKAGE versionlevel, $3))
])


dnl @synopsis AC_COND_WITH(PACKAGE [,DEFAULT])
dnl
dnl actually used after an AC_ARG_WITH(PKG,...) option-directive,
dnl where AC_ARG_WITH is a part of the standard autoconf to define a
dnl `configure` --with-PKG option.
dnl
dnl the AC_COND_WITH(PKG) will use the $with_PKG var to define WITH_PKG and 
dnl WITHOUT_PKG substitutions (AC_SUBST), that are either '' or '#' -
dnl depending whether the var was "no" or not (probably 'yes', or a value);
dnl it will also declare WITHVAL_PKG for use when someone wanted to set a val
dnl other than just "yes". And there is a WITHDEF_PKG that expands to a
dnl C-precompiler definition of the form -DWITH_PKG or  -DWITH_PKG=\"value\" 
dnl (n.b.: the PKG *is*  uppercased if in lowercase and "-" translit to "_")
dnl
dnl this macro is most handily in making Makefile.in/Makefile.am that have
dnl a set of if-with declarations that can be defined as follows:
dnl CFLAGS = -Wall @WITHOUT_FLOAT@ -msoft-float # --without-float
dnl @WITH_FLOAT@ LIBS += -lm              # --with-float
dnl DEFS += -DNDEBUG @WITHDEF_MY_PKG@     # --with-my-pkg="/usr/lib"
dnl DEFS += @WITHVAL_DEFS@                # --with-defs="-DLOGLEVEL=6"
dnl
dnl example configure.in:
dnl AC_ARG_WITH(float,
dnl [ --with-float,       with float words support])
dnl AC_COND_WITH(float,no)
dnl
dnl extened notes:
dnl 1. the idea comes from AM_CONDITIONAL but it is much easier to use,
dnl    and unlike automake's ifcond, the Makefile.am will work as a
dnl    normal $(MAKE) -f Makefile.am makefile.
dnl 2. the @VALS@ are parsed over by automake so automake will see all
dnl    the filenames and definitions that follow @WITH_FLOAT@, so that
dnl    the AC_COND_WITH user can see additional message if they apply.
dnl 3. in this m4-part, there's a AC_ARG_COND_WITH with the synopsis of 
dnl      AC_ARG_WITH and an implicit following AC_COND_WITH   =:-)
dnl 4. and there is an AC_ARG_COND_WITH_DEFINE that will emit an 
dnl      implicit AC_DEFINE that is actually seen by autoheader, even
dnl      generated with the correct name and comment, for config.h.in
dnl 
dnl some non-autoconf coders tend to create "editable" Makefile where
dnl they have out-commented lines with an example (additional) definition.
dnl Each of these can be replaced with a three-liner in configure.in as
dnl shown above. Starting to use AC_COND_WITH will soon lead you to 
dnl provide a dozen --with-option rules for the `configure` user. Do it!
dnl
dnl @version $Id: aclocal.m4,v 0.31 2001-03-19 21:50:41 guidod Exp $
dnl @author Guido Draheim <guidod@gmx.de>

AC_DEFUN([AC_COND_WITH],
[dnl the names to be defined...
pushdef([WITH_VAR],    patsubst([with_$1], -, _))dnl
pushdef([VAR_WITH],    patsubst(translit([with_$1], [a-z], [A-Z]), -, _))dnl
pushdef([VAR_WITHOUT], patsubst(translit([without_$1], [a-z], [A-Z]), -, _))dnl
pushdef([VAR_WITHVAL], patsubst(translit([withval_$1], [a-z], [A-Z]), -, _))dnl
pushdef([VAR_WITHDEF], patsubst(translit([withdef_$1], [a-z], [A-Z]), -, _))dnl
AC_SUBST(VAR_WITH)
AC_SUBST(VAR_WITHOUT)
AC_SUBST(VAR_WITHVAL)
AC_SUBST(VAR_WITHDEF)
if test -z "$WITH_VAR" ; then WITH_VAR=`echo ifelse([$2], , no, [$2])` ; fi
if test "$WITH_VAR" != "no"; then
  VAR_WITH=    ; VAR_WITHOUT='#'
  case "$WITH_VAR" in
    [yes)]    VAR_WITHVAL=""
              VAR_WITHDEF="-D""VAR_WITH" ;;
    [*)]      VAR_WITHVAL=WITH_VAR
              VAR_WITHDEF="-D""VAR_WITH="'"'$WITH_VAR'"' ;;
  esac
else
  VAR_WITH='#' ;  VAR_WITHOUT=
  VAR_WITHVAL= ;  VAR_WITHDEF=
fi
popdef([VAR_WITH])dnl
popdef([VAR_WITHOUT])dnl
popdef([VAR_WITHVAL])dnl
popdef([VAR_WITHDEF])dnl
popdef([WITH_VAR])dnl
])

AC_DEFUN([AC_ARG_COND_WITH],
[dnl
AC_ARG_WITH([$1],[$2],[$3],[$4],[$5])
# done with AC_ARG_WITH, now do AC_COND_WITH (rather than AM_CONDITIONAL)
AC_COND_WITH([$1])
])

dnl and the same version as AC_COND_WITH but including the 
dnl AC_DEFINE for WITH_PACKAGE 

AC_DEFUN([AC_COND_WITH_DEFINE],
[dnl the names to be defined...
pushdef([WITH_VAR],    patsubst([with_$1], -, _))dnl
pushdef([VAR_WITH],    patsubst(translit([with_$1], [a-z], [A-Z]), -, _))dnl
pushdef([VAR_WITHOUT], patsubst(translit([without_$1], [a-z], [A-Z]), -, _))dnl
pushdef([VAR_WITHVAL], patsubst(translit([withval_$1], [a-z], [A-Z]), -, _))dnl
pushdef([VAR_WITHDEF], patsubst(translit([withdef_$1], [a-z], [A-Z]), -, _))dnl
AC_SUBST(VAR_WITH)
AC_SUBST(VAR_WITHOUT)
AC_SUBST(VAR_WITHVAL)
AC_SUBST(VAR_WITHDEF)
if test -z "$WITH_VAR" ; then WITH_VAR=`echo ifelse([$2], , no, [$2])` ; fi
if test "$WITH_VAR" != "no"; then
  VAR_WITH=    ; VAR_WITHOUT='#'
  case "$WITH_VAR" in
    [yes)]    VAR_WITHVAL=""
              VAR_WITHDEF="-D""VAR_WITH" ;;
    [*)]      VAR_WITHVAL=WITH_VAR
              VAR_WITHDEF="-D""VAR_WITH="'"'$WITH_VAR'"' ;;
  esac
else
  VAR_WITH='#' ;  VAR_WITHOUT=
  VAR_WITHVAL= ;  VAR_WITHDEF=
fi
if test "_$WITH_VAR" != "_no" ; then
      AC_DEFINE_UNQUOTED(VAR_WITH, "$WITH_VAR", "--with-$1")
fi dnl
popdef([VAR_WITH])dnl
popdef([VAR_WITHOUT])dnl
popdef([VAR_WITHVAL])dnl
popdef([VAR_WITHDEF])dnl
popdef([WITH_VAR])dnl
])


# Define a conditional.

AC_DEFUN(AM_CONDITIONAL,
[AC_SUBST($1_TRUE)
AC_SUBST($1_FALSE)
if $2; then
  $1_TRUE=
  $1_FALSE='#'
else
  $1_TRUE='#'
  $1_FALSE=
fi])


# serial 40 AC_PROG_LIBTOOL
AC_DEFUN(AC_PROG_LIBTOOL,
[AC_REQUIRE([AC_LIBTOOL_SETUP])dnl

# Save cache, so that ltconfig can load it
AC_CACHE_SAVE

# Actually configure libtool.  ac_aux_dir is where install-sh is found.
CC="$CC" CFLAGS="$CFLAGS" CPPFLAGS="$CPPFLAGS" \
LD="$LD" LDFLAGS="$LDFLAGS" LIBS="$LIBS" \
LN_S="$LN_S" NM="$NM" RANLIB="$RANLIB" \
DLLTOOL="$DLLTOOL" AS="$AS" OBJDUMP="$OBJDUMP" \
${CONFIG_SHELL-/bin/sh} $ac_aux_dir/ltconfig --no-reexec \
$libtool_flags --no-verify $ac_aux_dir/ltmain.sh $lt_target \
|| AC_MSG_ERROR([libtool configure failed])

# Reload cache, that may have been modified by ltconfig
AC_CACHE_LOAD

# This can be used to rebuild libtool when needed
LIBTOOL_DEPS="$ac_aux_dir/ltconfig $ac_aux_dir/ltmain.sh"

# Always use our own libtool.
LIBTOOL='$(SHELL) $(top_builddir)/libtool'
AC_SUBST(LIBTOOL)dnl

# Redirect the config.log output again, so that the ltconfig log is not
# clobbered by the next message.
exec 5>>./config.log
])

AC_DEFUN(AC_LIBTOOL_SETUP,
[AC_PREREQ(2.13)dnl
AC_REQUIRE([AC_ENABLE_SHARED])dnl
AC_REQUIRE([AC_ENABLE_STATIC])dnl
AC_REQUIRE([AC_ENABLE_FAST_INSTALL])dnl
AC_REQUIRE([AC_CANONICAL_HOST])dnl
AC_REQUIRE([AC_CANONICAL_BUILD])dnl
AC_REQUIRE([AC_PROG_RANLIB])dnl
AC_REQUIRE([AC_PROG_CC])dnl
AC_REQUIRE([AC_PROG_LD])dnl
AC_REQUIRE([AC_PROG_NM])dnl
AC_REQUIRE([AC_PROG_LN_S])dnl
dnl

case "$target" in
NONE) lt_target="$host" ;;
*) lt_target="$target" ;;
esac

# Check for any special flags to pass to ltconfig.
libtool_flags="--cache-file=$cache_file"
test "$enable_shared" = no && libtool_flags="$libtool_flags --disable-shared"
test "$enable_static" = no && libtool_flags="$libtool_flags --disable-static"
test "$enable_fast_install" = no && libtool_flags="$libtool_flags --disable-fast-install"
test "$ac_cv_prog_gcc" = yes && libtool_flags="$libtool_flags --with-gcc"
test "$ac_cv_prog_gnu_ld" = yes && libtool_flags="$libtool_flags --with-gnu-ld"
ifdef([AC_PROVIDE_AC_LIBTOOL_DLOPEN],
[libtool_flags="$libtool_flags --enable-dlopen"])
ifdef([AC_PROVIDE_AC_LIBTOOL_WIN32_DLL],
[libtool_flags="$libtool_flags --enable-win32-dll"])
AC_ARG_ENABLE(libtool-lock,
  [  --disable-libtool-lock  avoid locking (might break parallel builds)])
test "x$enable_libtool_lock" = xno && libtool_flags="$libtool_flags --disable-lock"
test x"$silent" = xyes && libtool_flags="$libtool_flags --silent"

# Some flags need to be propagated to the compiler or linker for good
# libtool support.
case "$lt_target" in
*-*-irix6*)
  # Find out which ABI we are using.
  echo '[#]line __oline__ "configure"' > conftest.$ac_ext
  if AC_TRY_EVAL(ac_compile); then
    case "`/usr/bin/file conftest.o`" in
    *32-bit*)
      LD="${LD-ld} -32"
      ;;
    *N32*)
      LD="${LD-ld} -n32"
      ;;
    *64-bit*)
      LD="${LD-ld} -64"
      ;;
    esac
  fi
  rm -rf conftest*
  ;;

*-*-sco3.2v5*)
  # On SCO OpenServer 5, we need -belf to get full-featured binaries.
  SAVE_CFLAGS="$CFLAGS"
  CFLAGS="$CFLAGS -belf"
  AC_CACHE_CHECK([whether the C compiler needs -belf], lt_cv_cc_needs_belf,
    [AC_TRY_LINK([],[],[lt_cv_cc_needs_belf=yes],[lt_cv_cc_needs_belf=no])])
  if test x"$lt_cv_cc_needs_belf" != x"yes"; then
    # this is probably gcc 2.8.0, egcs 1.0 or newer; no need for -belf
    CFLAGS="$SAVE_CFLAGS"
  fi
  ;;

ifdef([AC_PROVIDE_AC_LIBTOOL_WIN32_DLL],
[*-*-cygwin* | *-*-mingw*)
  AC_CHECK_TOOL(DLLTOOL, dlltool, false)
  AC_CHECK_TOOL(AS, as, false)
  AC_CHECK_TOOL(OBJDUMP, objdump, false)
  ;;
])
esac
])

# AC_LIBTOOL_DLOPEN - enable checks for dlopen support
AC_DEFUN(AC_LIBTOOL_DLOPEN, [AC_BEFORE([$0],[AC_LIBTOOL_SETUP])])

# AC_LIBTOOL_WIN32_DLL - declare package support for building win32 dll's
AC_DEFUN(AC_LIBTOOL_WIN32_DLL, [AC_BEFORE([$0], [AC_LIBTOOL_SETUP])])

# AC_ENABLE_SHARED - implement the --enable-shared flag
# Usage: AC_ENABLE_SHARED[(DEFAULT)]
#   Where DEFAULT is either `yes' or `no'.  If omitted, it defaults to
#   `yes'.
AC_DEFUN(AC_ENABLE_SHARED, [dnl
define([AC_ENABLE_SHARED_DEFAULT], ifelse($1, no, no, yes))dnl
AC_ARG_ENABLE(shared,
changequote(<<, >>)dnl
<<  --enable-shared[=PKGS]  build shared libraries [default=>>AC_ENABLE_SHARED_DEFAULT],
changequote([, ])dnl
[p=${PACKAGE-default}
case "$enableval" in
yes) enable_shared=yes ;;
no) enable_shared=no ;;
*)
  enable_shared=no
  # Look at the argument we got.  We use all the common list separators.
  IFS="${IFS= 	}"; ac_save_ifs="$IFS"; IFS="${IFS}:,"
  for pkg in $enableval; do
    if test "X$pkg" = "X$p"; then
      enable_shared=yes
    fi
  done
  IFS="$ac_save_ifs"
  ;;
esac],
enable_shared=AC_ENABLE_SHARED_DEFAULT)dnl
])

# AC_DISABLE_SHARED - set the default shared flag to --disable-shared
AC_DEFUN(AC_DISABLE_SHARED, [AC_BEFORE([$0],[AC_LIBTOOL_SETUP])dnl
AC_ENABLE_SHARED(no)])

# AC_ENABLE_STATIC - implement the --enable-static flag
# Usage: AC_ENABLE_STATIC[(DEFAULT)]
#   Where DEFAULT is either `yes' or `no'.  If omitted, it defaults to
#   `yes'.
AC_DEFUN(AC_ENABLE_STATIC, [dnl
define([AC_ENABLE_STATIC_DEFAULT], ifelse($1, no, no, yes))dnl
AC_ARG_ENABLE(static,
changequote(<<, >>)dnl
<<  --enable-static[=PKGS]  build static libraries [default=>>AC_ENABLE_STATIC_DEFAULT],
changequote([, ])dnl
[p=${PACKAGE-default}
case "$enableval" in
yes) enable_static=yes ;;
no) enable_static=no ;;
*)
  enable_static=no
  # Look at the argument we got.  We use all the common list separators.
  IFS="${IFS= 	}"; ac_save_ifs="$IFS"; IFS="${IFS}:,"
  for pkg in $enableval; do
    if test "X$pkg" = "X$p"; then
      enable_static=yes
    fi
  done
  IFS="$ac_save_ifs"
  ;;
esac],
enable_static=AC_ENABLE_STATIC_DEFAULT)dnl
])

# AC_DISABLE_STATIC - set the default static flag to --disable-static
AC_DEFUN(AC_DISABLE_STATIC, [AC_BEFORE([$0],[AC_LIBTOOL_SETUP])dnl
AC_ENABLE_STATIC(no)])


# AC_ENABLE_FAST_INSTALL - implement the --enable-fast-install flag
# Usage: AC_ENABLE_FAST_INSTALL[(DEFAULT)]
#   Where DEFAULT is either `yes' or `no'.  If omitted, it defaults to
#   `yes'.
AC_DEFUN(AC_ENABLE_FAST_INSTALL, [dnl
define([AC_ENABLE_FAST_INSTALL_DEFAULT], ifelse($1, no, no, yes))dnl
AC_ARG_ENABLE(fast-install,
changequote(<<, >>)dnl
<<  --enable-fast-install[=PKGS]  optimize for fast installation [default=>>AC_ENABLE_FAST_INSTALL_DEFAULT],
changequote([, ])dnl
[p=${PACKAGE-default}
case "$enableval" in
yes) enable_fast_install=yes ;;
no) enable_fast_install=no ;;
*)
  enable_fast_install=no
  # Look at the argument we got.  We use all the common list separators.
  IFS="${IFS= 	}"; ac_save_ifs="$IFS"; IFS="${IFS}:,"
  for pkg in $enableval; do
    if test "X$pkg" = "X$p"; then
      enable_fast_install=yes
    fi
  done
  IFS="$ac_save_ifs"
  ;;
esac],
enable_fast_install=AC_ENABLE_FAST_INSTALL_DEFAULT)dnl
])

# AC_ENABLE_FAST_INSTALL - set the default to --disable-fast-install
AC_DEFUN(AC_DISABLE_FAST_INSTALL, [AC_BEFORE([$0],[AC_LIBTOOL_SETUP])dnl
AC_ENABLE_FAST_INSTALL(no)])

# AC_PROG_LD - find the path to the GNU or non-GNU linker
AC_DEFUN(AC_PROG_LD,
[AC_ARG_WITH(gnu-ld,
[  --with-gnu-ld           assume the C compiler uses GNU ld [default=no]],
test "$withval" = no || with_gnu_ld=yes, with_gnu_ld=no)
AC_REQUIRE([AC_PROG_CC])dnl
AC_REQUIRE([AC_CANONICAL_HOST])dnl
AC_REQUIRE([AC_CANONICAL_BUILD])dnl
ac_prog=ld
if test "$ac_cv_prog_gcc" = yes; then
  # Check if gcc -print-prog-name=ld gives a path.
  AC_MSG_CHECKING([for ld used by GCC])
  ac_prog=`($CC -print-prog-name=ld) 2>&5`
  case "$ac_prog" in
    # Accept absolute paths.
changequote(,)dnl
    [\\/]* | [A-Za-z]:[\\/]*)
      re_direlt='/[^/][^/]*/\.\./'
changequote([,])dnl
      # Canonicalize the path of ld
      ac_prog=`echo $ac_prog| sed 's%\\\\%/%g'`
      while echo $ac_prog | grep "$re_direlt" > /dev/null 2>&1; do
	ac_prog=`echo $ac_prog| sed "s%$re_direlt%/%"`
      done
      test -z "$LD" && LD="$ac_prog"
      ;;
  "")
    # If it fails, then pretend we aren't using GCC.
    ac_prog=ld
    ;;
  *)
    # If it is relative, then search for the first ld in PATH.
    with_gnu_ld=unknown
    ;;
  esac
elif test "$with_gnu_ld" = yes; then
  AC_MSG_CHECKING([for GNU ld])
else
  AC_MSG_CHECKING([for non-GNU ld])
fi
AC_CACHE_VAL(ac_cv_path_LD,
[if test -z "$LD"; then
  IFS="${IFS= 	}"; ac_save_ifs="$IFS"; IFS="${IFS}${PATH_SEPARATOR-:}"
  for ac_dir in $PATH; do
    test -z "$ac_dir" && ac_dir=.
    if test -f "$ac_dir/$ac_prog" || test -f "$ac_dir/$ac_prog$ac_exeext"; then
      ac_cv_path_LD="$ac_dir/$ac_prog"
      # Check to see if the program is GNU ld.  I'd rather use --version,
      # but apparently some GNU ld's only accept -v.
      # Break only if it was the GNU/non-GNU ld that we prefer.
      if "$ac_cv_path_LD" -v 2>&1 < /dev/null | egrep '(GNU|with BFD)' > /dev/null; then
	test "$with_gnu_ld" != no && break
      else
	test "$with_gnu_ld" != yes && break
      fi
    fi
  done
  IFS="$ac_save_ifs"
else
  ac_cv_path_LD="$LD" # Let the user override the test with a path.
fi])
LD="$ac_cv_path_LD"
if test -n "$LD"; then
  AC_MSG_RESULT($LD)
else
  AC_MSG_RESULT(no)
fi
test -z "$LD" && AC_MSG_ERROR([no acceptable ld found in \$PATH])
AC_PROG_LD_GNU
])

AC_DEFUN(AC_PROG_LD_GNU,
[AC_CACHE_CHECK([if the linker ($LD) is GNU ld], ac_cv_prog_gnu_ld,
[# I'd rather use --version here, but apparently some GNU ld's only accept -v.
if $LD -v 2>&1 </dev/null | egrep '(GNU|with BFD)' 1>&5; then
  ac_cv_prog_gnu_ld=yes
else
  ac_cv_prog_gnu_ld=no
fi])
])

# AC_PROG_NM - find the path to a BSD-compatible name lister
AC_DEFUN(AC_PROG_NM,
[AC_MSG_CHECKING([for BSD-compatible nm])
AC_CACHE_VAL(ac_cv_path_NM,
[if test -n "$NM"; then
  # Let the user override the test.
  ac_cv_path_NM="$NM"
else
  IFS="${IFS= 	}"; ac_save_ifs="$IFS"; IFS="${IFS}${PATH_SEPARATOR-:}"
  for ac_dir in $PATH /usr/ccs/bin /usr/ucb /bin; do
    test -z "$ac_dir" && ac_dir=.
    if test -f $ac_dir/nm || test -f $ac_dir/nm$ac_exeext ; then
      # Check to see if the nm accepts a BSD-compat flag.
      # Adding the `sed 1q' prevents false positives on HP-UX, which says:
      #   nm: unknown option "B" ignored
      if ($ac_dir/nm -B /dev/null 2>&1 | sed '1q'; exit 0) | egrep /dev/null >/dev/null; then
	ac_cv_path_NM="$ac_dir/nm -B"
	break
      elif ($ac_dir/nm -p /dev/null 2>&1 | sed '1q'; exit 0) | egrep /dev/null >/dev/null; then
	ac_cv_path_NM="$ac_dir/nm -p"
	break
      else
	ac_cv_path_NM=${ac_cv_path_NM="$ac_dir/nm"} # keep the first match, but
	continue # so that we can try to find one that supports BSD flags
      fi
    fi
  done
  IFS="$ac_save_ifs"
  test -z "$ac_cv_path_NM" && ac_cv_path_NM=nm
fi])
NM="$ac_cv_path_NM"
AC_MSG_RESULT([$NM])
])

# AC_CHECK_LIBM - check for math library
AC_DEFUN(AC_CHECK_LIBM,
[AC_REQUIRE([AC_CANONICAL_HOST])dnl
LIBM=
case "$lt_target" in
*-*-beos* | *-*-cygwin*)
  # These system don't have libm
  ;;
*-ncr-sysv4.3*)
  AC_CHECK_LIB(mw, _mwvalidcheckl, LIBM="-lmw")
  AC_CHECK_LIB(m, main, LIBM="$LIBM -lm")
  ;;
*)
  AC_CHECK_LIB(m, main, LIBM="-lm")
  ;;
esac
])

# AC_LIBLTDL_CONVENIENCE[(dir)] - sets LIBLTDL to the link flags for
# the libltdl convenience library and INCLTDL to the include flags for
# the libltdl header and adds --enable-ltdl-convenience to the
# configure arguments.  Note that LIBLTDL and INCLTDL are not
# AC_SUBSTed, nor is AC_CONFIG_SUBDIRS called.  If DIR is not
# provided, it is assumed to be `libltdl'.  LIBLTDL will be prefixed
# with '${top_builddir}/' and INCLTDL will be prefixed with
# '${top_srcdir}/' (note the single quotes!).  If your package is not
# flat and you're not using automake, define top_builddir and
# top_srcdir appropriately in the Makefiles.
AC_DEFUN(AC_LIBLTDL_CONVENIENCE, [AC_BEFORE([$0],[AC_LIBTOOL_SETUP])dnl
  case "$enable_ltdl_convenience" in
  no) AC_MSG_ERROR([this package needs a convenience libltdl]) ;;
  "") enable_ltdl_convenience=yes
      ac_configure_args="$ac_configure_args --enable-ltdl-convenience" ;;
  esac
  LIBLTDL='${top_builddir}/'ifelse($#,1,[$1],['libltdl'])/libltdlc.la
  INCLTDL='-I${top_srcdir}/'ifelse($#,1,[$1],['libltdl'])
])

# AC_LIBLTDL_INSTALLABLE[(dir)] - sets LIBLTDL to the link flags for
# the libltdl installable library and INCLTDL to the include flags for
# the libltdl header and adds --enable-ltdl-install to the configure
# arguments.  Note that LIBLTDL and INCLTDL are not AC_SUBSTed, nor is
# AC_CONFIG_SUBDIRS called.  If DIR is not provided and an installed
# libltdl is not found, it is assumed to be `libltdl'.  LIBLTDL will
# be prefixed with '${top_builddir}/' and INCLTDL will be prefixed
# with '${top_srcdir}/' (note the single quotes!).  If your package is
# not flat and you're not using automake, define top_builddir and
# top_srcdir appropriately in the Makefiles.
# In the future, this macro may have to be called after AC_PROG_LIBTOOL.
AC_DEFUN(AC_LIBLTDL_INSTALLABLE, [AC_BEFORE([$0],[AC_LIBTOOL_SETUP])dnl
  AC_CHECK_LIB(ltdl, main,
  [test x"$enable_ltdl_install" != xyes && enable_ltdl_install=no],
  [if test x"$enable_ltdl_install" = xno; then
     AC_MSG_WARN([libltdl not installed, but installation disabled])
   else
     enable_ltdl_install=yes
   fi
  ])
  if test x"$enable_ltdl_install" = x"yes"; then
    ac_configure_args="$ac_configure_args --enable-ltdl-install"
    LIBLTDL='${top_builddir}/'ifelse($#,1,[$1],['libltdl'])/libltdl.la
    INCLTDL='-I${top_srcdir}/'ifelse($#,1,[$1],['libltdl'])
  else
    ac_configure_args="$ac_configure_args --enable-ltdl-install=no"
    LIBLTDL="-lltdl"
    INCLTDL=
  fi
])

dnl old names
AC_DEFUN(AM_PROG_LIBTOOL, [indir([AC_PROG_LIBTOOL])])dnl
AC_DEFUN(AM_ENABLE_SHARED, [indir([AC_ENABLE_SHARED], $@)])dnl
AC_DEFUN(AM_ENABLE_STATIC, [indir([AC_ENABLE_STATIC], $@)])dnl
AC_DEFUN(AM_DISABLE_SHARED, [indir([AC_DISABLE_SHARED], $@)])dnl
AC_DEFUN(AM_DISABLE_STATIC, [indir([AC_DISABLE_STATIC], $@)])dnl
AC_DEFUN(AM_PROG_LD, [indir([AC_PROG_LD])])dnl
AC_DEFUN(AM_PROG_NM, [indir([AC_PROG_NM])])dnl

dnl This is just to silence aclocal about the macro not being used
ifelse([AC_DISABLE_FAST_INSTALL])dnl

dnl @synopsis AC_COMPILE_WARNINGS
dnl
dnl Set the maximum warning verbosity according to compiler used.
dnl Currently supports g++ and gcc.
dnl This macro must be put after AC_PROG_CC and AC_PROG_CXX in
dnl configure.in
dnl
dnl @version $Id: aclocal.m4,v 0.31 2001-03-19 21:50:41 guidod Exp $
dnl @author Loic Dachary <loic@senga.org>
dnl
AC_DEFUN([AC_COMPILE_WARNINGS],
[AC_MSG_CHECKING(maximum warning verbosity option)
if test -n "$CXX"
then
  if test "$GXX" = "yes"
  then
    ac_compile_warnings_opt='-Wall'
  fi
  CXXFLAGS="$CXXFLAGS $ac_compile_warnings_opt"
  ac_compile_warnings_msg="$ac_compile_warnings_opt for C++"
fi

if test -n "$CC"
then
  if test "$GCC" = "yes"
  then
    ac_compile_warnings_opt='-Wall'
  fi
  CFLAGS="$CFLAGS $ac_compile_warnings_opt"
  ac_compile_warnings_msg="$ac_compile_warnings_msg $ac_compile_warnings_opt for C"
fi
AC_MSG_RESULT($ac_compile_warnings_msg)
unset ac_compile_warnings_msg
unset ac_compile_warnings_opt
])

# Add --enable-maintainer-mode option to configure.
# From Jim Meyering

# serial 1

AC_DEFUN(AM_MAINTAINER_MODE,
[AC_MSG_CHECKING([whether to enable maintainer-specific portions of Makefiles])
  dnl maintainer-mode is disabled by default
  AC_ARG_ENABLE(maintainer-mode,
[  --enable-maintainer-mode enable make rules and dependencies not useful
                          (and sometimes confusing) to the casual installer],
      USE_MAINTAINER_MODE=$enableval,
      USE_MAINTAINER_MODE=no)
  AC_MSG_RESULT($USE_MAINTAINER_MODE)
  AM_CONDITIONAL(MAINTAINER_MODE, test $USE_MAINTAINER_MODE = yes)
  MAINT=$MAINTAINER_MODE_TRUE
  AC_SUBST(MAINT)dnl
]
)

dnl @synopsis AC_FUNC_MKDIR
dnl
dnl Check whether mkdir() is mkdir or _mkdir, and whether it takes one or two
dnl arguments.
dnl
dnl This macro can define HAVE_MKDIR, HAVE__MKDIR, and MKDIR_TAKES_ONE_ARG,
dnl which are expected to be used as follow:
dnl
dnl   #if HAVE_MKDIR
dnl   # if MKDIR_TAKES_ONE_ARG
dnl   	 /* Mingw32 */
dnl   #  define mkdir(a,b) mkdir(a)
dnl   # endif
dnl   #else
dnl   # if HAVE__MKDIR
dnl   	 /* plain Win32 */
dnl   #  define mkdir(a,b) _mkdir(a)
dnl   # else
dnl   #  error "Don't know how to create a directory on this system."
dnl   # endif
dnl   #endif
dnl
dnl @version $Id: aclocal.m4,v 0.31 2001-03-19 21:50:41 guidod Exp $
dnl @author Written by Alexandre Duret-Lutz <duret_g@epita.fr>.

AC_DEFUN([AC_FUNC_MKDIR],
[AC_CHECK_FUNCS([mkdir _mkdir])
AC_CACHE_CHECK([whether mkdir takes one argument],
                [ac_cv_mkdir_takes_one_arg],
[AC_TRY_COMPILE([
#include <sys/stat.h>
#if HAVE_UNISTD_H
# include <unistd.h>
#endif
],[mkdir (".");],
[ac_cv_mkdir_takes_one_arg=yes],[ac_cv_mkdir_takes_one_arg=no])])
if test x"$ac_cv_mkdir_takes_one_arg" = xyes; then
  AC_DEFINE([MKDIR_TAKES_ONE_ARG],1,
            [Define if mkdir takes only one argument.])
fi
])

dnl Note:
dnl =====
dnl I have not implemented the following suggestion because I don't have
dnl access to such a broken environment to test the macro.  So I'm just
dnl appending the comments here in case you have, and want to fix
dnl AC_FUNC_MKDIR that way.
dnl
dnl |Thomas E. Dickey (dickey@herndon4.his.com) said:
dnl |  it doesn't cover the problem areas (compilers that mistreat mkdir
dnl |  may prototype it in dir.h and dirent.h, for instance).
dnl |
dnl |Alexandre:
dnl |  Would it be sufficient to check for these headers and #include
dnl |  them in the AC_TRY_COMPILE block?  (and is AC_HEADER_DIRENT
dnl |  suitable for this?)
dnl |
dnl |Thomas:
dnl |  I think that might be a good starting point (with the set of recommended
dnl |  ifdef's and includes for AC_HEADER_DIRENT, of course).

dnl @synopsis AC_C_BIGENDIAN_CROSS
dnl
dnl check endianess even when crosscompiling
dnl (partially based on the original AC_C_BIGENDIAN)
dnl
dnl the implementation will create a binary, and instead of running
dnl the binary it will be grep'ed for some symbols that will look
dnl different for different endianess of the binary.
dnl
dnl @version $Id: aclocal.m4,v 0.31 2001-03-19 21:50:41 guidod Exp $
dnl @author                            Guido Draheim <guidod@gmx.de>

AC_DEFUN([AC_C_BIGENDIAN_CROSS],
[AC_CACHE_CHECK(whether byte ordering is bigendian, ac_cv_c_bigendian,
[ac_cv_c_bigendian=unknown
# See if sys/param.h defines the BYTE_ORDER macro.
AC_TRY_COMPILE([#include <sys/types.h>
#include <sys/param.h>], [
#if !BYTE_ORDER || !BIG_ENDIAN || !LITTLE_ENDIAN
 bogus endian macros
#endif], [# It does; now see whether it defined to BIG_ENDIAN or not.
AC_TRY_COMPILE([#include <sys/types.h>
#include <sys/param.h>], [
#if BYTE_ORDER != BIG_ENDIAN
 not big endian
#endif], ac_cv_c_bigendian=yes, ac_cv_c_bigendian=no)])
if test $ac_cv_c_bigendian = unknown; then
AC_TRY_RUN([main () {
  /* Are we little or big endian?  From Harbison&Steele.  */
  union
  {
    long l;
    char c[sizeof (long)];
  } u;
  u.l = 1;
  exit (u.c[sizeof (long) - 1] == 1);
}], ac_cv_c_bigendian=no, ac_cv_c_bigendian=yes, 
[ echo $ac_n "cross-compiling... " 2>&AC_FD_MSG ])
fi])
if test $ac_cv_c_bigendian = unknown; then
AC_MSG_CHECKING(to probe for byte ordering)
[
cat >conftest.c <<EOF
short ascii_mm[] = { 0x4249, 0x4765, 0x6E44, 0x6961, 0x6E53, 0x7953, 0 };
short ascii_ii[] = { 0x694C, 0x5454, 0x656C, 0x6E45, 0x6944, 0x6E61, 0 };
void _ascii() { char* s = (char*) ascii_mm; s = (char*) ascii_ii; }
short ebcdic_ii[] = { 0x89D3, 0xE3E3, 0x8593, 0x95C5, 0x89C4, 0x9581, 0 };
short ebcdic_mm[] = { 0xC2C9, 0xC785, 0x95C4, 0x8981, 0x95E2, 0xA8E2, 0 };
void _ebcdic() { char* s = (char*) ebcdic_mm; s = (char*) ebcdic_ii; }
int main() { _ascii (); _ebcdic (); return 0; }
EOF
] if test -f conftest.c ; then 
     if ${CC-cc} conftest.c -o conftest.o && test -f conftest.o ; then
        if test `grep -l BIGenDianSyS conftest.o` ; then
           echo $ac_n ' big endian probe OK, ' 1>&AC_FD_MSG
	   ac_cv_c_bigendian=yes 
        fi
        if test `grep -l LiTTleEnDian conftest.o` ; then
           echo $ac_n ' little endian probe OK, ' 1>&AC_FD_MSG
	   if test $ac_cv_c_bigendian = yes ; then
	    ac_cv_c_bigendian=unknown;
           else	
            ac_cv_c_bigendian=no
           fi
        fi
	echo $ac_n 'guessing bigendian ...  ' >&AC_FD_MSG
     fi
  fi
AC_MSG_RESULT($ac_cv_c_bigendian)
fi
if test $ac_cv_c_bigendian = yes; then
  AC_DEFINE(WORDS_BIGENDIAN, 1, [whether byteorder is bigendian])
  BYTEORDER=4321
else
  BYTEORDER=1234
fi
AC_DEFINE_UNQUOTED(BYTEORDER, $BYTEORDER, [1234 = LIL_ENDIAN, 4321 = BIGENDIAN])
if test $ac_cv_c_bigendian = unknown; then
  AC_MSG_ERROR(unknown endianess - sorry, please pre-set ac_cv_c_bigendian)
fi
])


dnl @synopsis AC_C_LONG_LONG
dnl
dnl Provides a test for the existance of the long long int type and
dnl defines HAVE_LONG_LONG if it is found.
dnl
dnl @version $Id: aclocal.m4,v 0.31 2001-03-19 21:50:41 guidod Exp $
dnl @author Caolan McNamara <caolan@skynet.ie>
dnl
dnl change: add a comment for the ac_define <guidod>

AC_DEFUN([AC_C_LONG_LONG_],
[AC_CACHE_CHECK(for long long int, ac_cv_c_long_long,
[if test "$GCC" = yes; then
  ac_cv_c_long_long=yes
  else
        AC_TRY_COMPILE(,[long long int i;],
   ac_cv_c_long_long=yes,
   ac_cv_c_long_long=no)
   fi])
   if test $ac_cv_c_long_long = yes; then
     AC_DEFINE(HAVE_LONG_LONG, 1, [compiler understands long long])
   fi
])

dnl @synopsis AC_C_LONG_LONG
dnl
dnl Provides a test for the existance of the long long int type and
dnl defines HAVE_LONG_LONG if it is found.
dnl
dnl @version $Id: aclocal.m4,v 0.31 2001-03-19 21:50:41 guidod Exp $
dnl @author Caolan McNamara <caolan@skynet.ie>
dnl
AC_DEFUN([AC_C_LONG_LONG],
[AC_CACHE_CHECK(for long long int, ac_cv_c_long_long,
[if test "$GCC" = yes; then
  ac_cv_c_long_long=yes
  else
        AC_TRY_COMPILE(,[long long int i;],
   ac_cv_c_long_long=yes,
   ac_cv_c_long_long=no)
   fi])
   if test $ac_cv_c_long_long = yes; then
     AC_DEFINE(HAVE_LONG_LONG)
   fi
])

dnl @synopsis ACX_C_RESTRICT
dnl
dnl This macro determines whether the C compiler supports the "restrict"
dnl keyword introduced in ANSI C99, or an equivalent.  Does nothing if
dnl the compiler accepts the keyword.  Otherwise, if the compiler supports
dnl an equivalent (like gcc's __restrict__) defines "restrict" to be that.
dnl Otherwise, defines "restrict" to be empty.
dnl
dnl @version $Id: aclocal.m4,v 0.31 2001-03-19 21:50:41 guidod Exp $
dnl @author Steven G. Johnson <stevenj@alum.mit.edu>

AC_DEFUN([ACX_C_RESTRICT],
[AC_CACHE_CHECK([for C restrict keyword], acx_cv_c_restrict,
[acx_cv_c_restrict=unsupported
 AC_LANG_SAVE
 AC_LANG_C
 # Try the official restrict keyword, then gcc's __restrict__, then
 # SGI's __restrict.  __restrict has slightly different semantics than
 # restrict (it's a bit stronger, in that __restrict pointers can't
 # overlap even with non __restrict pointers), but I think it should be
 # okay under the circumstances where restrict is normally used.
 for acx_kw in restrict __restrict__ __restrict; do
   AC_TRY_COMPILE([], [float * $acx_kw x;], [acx_cv_c_restrict=$acx_kw; break])
 done
 AC_LANG_RESTORE
])
 if test "$acx_cv_c_restrict" != "restrict"; then
   acx_kw="$acx_cv_c_restrict"
   if test "$acx_kw" = unsupported; then acx_kw=""; fi
   AC_DEFINE_UNQUOTED(restrict, $acx_kw, [Define to equivalent of C99 restrict keyword, or to nothing if this is not supported.  Do not define if restrict is supported directly.])
 fi
])

dnl @synopsis AC_COMPILE_CHECK_SIZEOF(TYPE [, HEADERS [, EXTRA_SIZES...]])
dnl
dnl This macro checks for the size of TYPE using compile checks, not
dnl run checks. You can supply extra HEADERS to look into. the check
dnl will cycle through 1 2 4 8 16 and any EXTRA_SIZES the user
dnl supplies. If a match is found, it will #define SIZEOF_`TYPE' to
dnl that value. Otherwise it will emit a configure time error
dnl indicating the size of the type could not be determined.
dnl
dnl The trick is that C will not allow duplicate case labels. While
dnl this is valid C code:
dnl
dnl      switch (0) case 0: case 1:;
dnl
dnl The following is not:
dnl
dnl      switch (0) case 0: case 0:;
dnl
dnl Thus, the AC_TRY_COMPILE will fail if the currently tried size
dnl does not match.
dnl
dnl Here is an example skeleton configure.in script, demonstrating the
dnl macro's usage:
dnl
dnl      AC_PROG_CC
dnl      AC_CHECK_HEADERS(stddef.h unistd.h)
dnl      AC_TYPE_SIZE_T
dnl      AC_CHECK_TYPE(ssize_t, int)
dnl
dnl      headers='#ifdef HAVE_STDDEF_H
dnl      #include <stddef.h>
dnl      #endif
dnl      #ifdef HAVE_UNISTD_H
dnl      #include <unistd.h>
dnl      #endif
dnl      '
dnl
dnl      AC_COMPILE_CHECK_SIZEOF(char)
dnl      AC_COMPILE_CHECK_SIZEOF(short)
dnl      AC_COMPILE_CHECK_SIZEOF(int)
dnl      AC_COMPILE_CHECK_SIZEOF(long)
dnl      AC_COMPILE_CHECK_SIZEOF(unsigned char *)
dnl      AC_COMPILE_CHECK_SIZEOF(void *)
dnl      AC_COMPILE_CHECK_SIZEOF(size_t, $headers)
dnl      AC_COMPILE_CHECK_SIZEOF(ssize_t, $headers)
dnl      AC_COMPILE_CHECK_SIZEOF(ptrdiff_t, $headers)
dnl      AC_COMPILE_CHECK_SIZEOF(off_t, $headers)
dnl
dnl @author Kaveh Ghazi <ghazi@caip.rutgers.edu>
dnl @version $Id: aclocal.m4,v 0.31 2001-03-19 21:50:41 guidod Exp $
dnl
AC_DEFUN([AC_COMPILE_CHECK_SIZEOF],
[changequote(<<, >>)dnl
dnl The name to #define.
define(<<AC_TYPE_NAME>>, translit(sizeof_$1, [a-z *], [A-Z_P]))dnl
dnl The cache variable name.
define(<<AC_CV_NAME>>, translit(ac_cv_sizeof_$1, [ *], [_p]))dnl
changequote([, ])dnl
AC_MSG_CHECKING(size of $1)
AC_CACHE_VAL(AC_CV_NAME,
[for ac_size in 4 8 1 2 16 $2 ; do # List sizes in rough order of prevalence.
  AC_TRY_COMPILE([#include "confdefs.h"
#include <sys/types.h>
$2
], [switch (0) case 0: case (sizeof ($1) == $ac_size):;], AC_CV_NAME=$ac_size)
  if test x$AC_CV_NAME != x ; then break; fi
done
])
if test x$AC_CV_NAME = x ; then
  AC_MSG_ERROR([cannot determine a size for $1])
fi
AC_MSG_RESULT($AC_CV_NAME)
AC_DEFINE_UNQUOTED(AC_TYPE_NAME, $AC_CV_NAME, [The number of bytes in type $1])
undefine([AC_TYPE_NAME])dnl
undefine([AC_CV_NAME])dnl
])

dnl @synopsis AC_FUNC_SNPRINTF
dnl
dnl Provides a test for a fully C9x complient snprintf
dnl function.
Dnl defines HAVE_SNPRINTF if it is found, and
dnl sets ac_cv_func_snprintf to yes, otherwise to no.
dnl
dnl @version $Id: aclocal.m4,v 0.31 2001-03-19 21:50:41 guidod Exp $
dnl @author Caolan McNamara <caolan@skynet.ie>
dnl
AC_DEFUN([AC_FUNC_SNPRINTF],
[AC_CACHE_CHECK(for working snprintf, ac_cv_func_snprintf,
[AC_TRY_RUN([#include <stdio.h>
int main () { exit (!(3 <= snprintf(NULL,0,"%d",100))); }
], ac_cv_func_snprintf=yes, ac_cv_func_snprintf=no,
ac_cv_func_snprintf=no)])
if test $ac_cv_func_snprintf = yes; then
  AC_DEFINE(HAVE_SNPRINTF)
fi
])

dnl @synopsis AC_COND_WITH_LEVEL(PACKAGE [,DEFAULT [,YESLEVEL]])
dnl
dnl actually used after an AC_ARG_WITH(PKG,...) option-directive,
dnl where AC_ARG_WITH is a part of the standard autoconf to define a
dnl `configure` --with-PKG option.
dnl
dnl this macros works almost like AC_COND_WITH(PACKAGE [,DEFAULT]), but
dnl the definition is set to a numeric value. The level of a simple "yes"
dnl is given (or assumed "2" if absent), otherwise the level can be
dnl given in mnemonic names, being
dnl 1 = some
dnl 2 = many
dnl 3 = all
dnl 4 = extra 
dnl 5 = extrasome more
dnl 6 = extramany muchmore much
dnl 7 = super everything
dnl 8 = ultra experimental
dnl 9 = insane
dnl
dnl this macro is most handily it making Makefile.in/Makefile.am that have
dnl a set of with-level declarations, espcially optimization-levels
dnl
dnl the $withval shell-variable is set for further examination
dnl (it carries the numeric value of the various mnemonics above)
dnl
dnl @version $Id: aclocal.m4,v 0.31 2001-03-19 21:50:41 guidod Exp $
dnl @author Guido Draheim <guidod@gmx.de>

AC_DEFUN(AC_COND_WITH_LEVEL,
[dnl the names to be defined...
pushdef([WITH_VAR],    patsubst(with_$1, -, _))dnl
pushdef([VAR_WITH],    patsubst(translit(with_$1, [a-z], [A-Z]), -, _))dnl
pushdef([VAR_WITHOUT], patsubst(translit(without_$1, [a-z], [A-Z]), -, _))dnl
pushdef([VAR_WITHVAL], patsubst(translit(withval_$1, [a-z], [A-Z]), -, _))dnl
pushdef([VAR_WITHDEF], patsubst(translit(withdef_$1, [a-z], [A-Z]), -, _))dnl
AC_SUBST(VAR_WITH)
AC_SUBST(VAR_WITHOUT)
AC_SUBST(VAR_WITHVAL)
AC_SUBST(VAR_WITHDEF)
if test -z "$WITH_VAR" 
   then WITH_VAR=`echo ifelse([$2], , no, [$2])` 
fi
if test "$WITH_VAR" = "yes" 
   then WITH_VAR=`echo ifelse([$3], , some, [$3])` 
fi
if test "$WITH_VAR" != "no"; then
  VAR_WITH=    ; VAR_WITHOUT='#'
  case "$WITH_VAR" in
    9|9,*|ultrasome|ultrasome,*|insane)				withval=9
		VAR_WITHVAL=9 ;  VAR_WITHDEF="-D""VAR_WITH=9" ;;
    8|8,*|ultra|ultra,*|experimental) 				withval=8 
                VAR_WITHVAL=8 ;  VAR_WITHDEF="-D""VAR_WITH=8" ;;
    7|7,*|muchmore|somemanymore|somemanymore,*|all)		withval=7
	        VAR_WITHVAL=7 ;  VAR_WITHDEF="-D""VAR_WITH=7" ;;
    6|6,*|manymore|manymore,*|most) 				withval=6
                VAR_WITHVAL=6 ;  VAR_WITHDEF="-D""VAR_WITH=6" ;;
    5|5,*|somemore|somemore,*|almost) 				withval=5
	        VAR_WITHVAL=5 ;  VAR_WITHDEF="-D""VAR_WITH=5" ;;
    4|4,*|more|more,*) 						withval=4
		VAR_WITHVAL=4 ;  VAR_WITHDEF="-D""VAR_WITH=4" ;;
    3|3,*|much|somemany,*)					withval=3
		VAR_WITHVAL=3 ;  VAR_WITHDEF="-D""VAR_WITH=3" ;;
    2|2,*|many|many,*) 						withval=2
		VAR_WITHVAL=2 ;  VAR_WITHDEF="-D""VAR_WITH=2" ;;
    1|1,*|some|some,*) 						withval=1
		VAR_WITHVAL=1 ;  VAR_WITHDEF="-D""VAR_WITH=1" ;;
      *) 							withval=-
 	       AC_MSG_ERROR([dnl
 bad value of --with-$1 option, choose a number or some|many|more|all... ])
  esac
else								withval=0
  VAR_WITH='#' ;   VAR_WITHOUT=""
  VAR_WITHVAL=0 ;  VAR_WITHDEF=""
fi
popdef([VAR_WITH])dnl
popdef([VAR_WITHOUT])dnl
popdef([VAR_WITHVAL])dnl
popdef([VAR_WITHDEF])dnl
popdef([WITH_VAR])dnl
])

dnl and the same as AC_COND_WITH_LEVEL with an AC_DEFINE in the end...
AC_DEFUN(AC_COND_WITH_LEVEL_DEFINE,
[dnl the names to be defined...
pushdef([WITH_VAR],    patsubst(with_$1, -, _))dnl
pushdef([VAR_WITH],    patsubst(translit(with_$1, [a-z], [A-Z]), -, _))dnl
pushdef([VAR_WITHOUT], patsubst(translit(without_$1, [a-z], [A-Z]), -, _))dnl
pushdef([VAR_WITHVAL], patsubst(translit(withval_$1, [a-z], [A-Z]), -, _))dnl
pushdef([VAR_WITHDEF], patsubst(translit(withdef_$1, [a-z], [A-Z]), -, _))dnl
AC_SUBST(VAR_WITH)
AC_SUBST(VAR_WITHOUT)
AC_SUBST(VAR_WITHVAL)
AC_SUBST(VAR_WITHDEF)
if test -z "$WITH_VAR"  
   then WITH_VAR=`echo ifelse([$2], , no, [$2])` 
fi
if test "$WITH_VAR" = "yes" 
   then WITH_VAR=`echo ifelse([$3], , some, [$3])`  
fi
if test "$WITH_VAR" != "no"; then
  VAR_WITH=    ; VAR_WITHOUT='#'
  case "$WITH_VAR" in
    9|9,*|ultrasome|ultrasome,*|insane)				withval=9
		VAR_WITHVAL=9 ;  VAR_WITHDEF="-D""VAR_WITH=9" ;;
    8|8,*|ultra|ultra,*|experimental) 				withval=8 
                VAR_WITHVAL=8 ;  VAR_WITHDEF="-D""VAR_WITH=8" ;;
    7|7,*|muchmore|somemanymore|somemanymore,*|all)		withval=7
	        VAR_WITHVAL=7 ;  VAR_WITHDEF="-D""VAR_WITH=7" ;;
    6|6,*|manymore|manymore,*|most) 				withval=6
                VAR_WITHVAL=6 ;  VAR_WITHDEF="-D""VAR_WITH=6" ;;
    5|5,*|somemore|somemore,*|almost) 				withval=5
	        VAR_WITHVAL=5 ;  VAR_WITHDEF="-D""VAR_WITH=5" ;;
    4|4,*|more|more,*) 						withval=4
		VAR_WITHVAL=4 ;  VAR_WITHDEF="-D""VAR_WITH=4" ;;
    3|3,*|much|somemany,*)					withval=3
		VAR_WITHVAL=3 ;  VAR_WITHDEF="-D""VAR_WITH=3" ;;
    2|2,*|many|many,*) 						withval=2
		VAR_WITHVAL=2 ;  VAR_WITHDEF="-D""VAR_WITH=2" ;;
    1|1,*|some|some,*) 						withval=1
		VAR_WITHVAL=1 ;  VAR_WITHDEF="-D""VAR_WITH=1" ;;
      *) 							withval=-
 	       AC_MSG_ERROR([dnl
 bad value of --with-$1 option, choose a number or some|many|more|all... ])
  esac
dnl -- the additional line is here --
  AC_DEFINE_UNQUOTED(VAR_WITH,$VAR_WITHVAL,"--with-$1=level")
else								withval=0
  VAR_WITH='#' ;   VAR_WITHOUT=""
  VAR_WITHVAL=0 ;  VAR_WITHDEF=""
fi
popdef([VAR_WITH])dnl
popdef([VAR_WITHOUT])dnl
popdef([VAR_WITHVAL])dnl
popdef([VAR_WITHDEF])dnl
popdef([WITH_VAR])dnl
])




dnl @synopsis AC_CHECK_CC_OPT(flag, cachevarname)
dnl 
dnl AC_CHECK_CC_OPT(-fvomit-frame,vomitframe)
dnl would show a message as like 
dnl "checking wether gcc accepts -fvomit-frame ... no"
dnl and sets the shell-variable $vomitframe to either "-fvomit-frame"
dnl or (in this case) just a simple "". In many cases you would then call 
dnl AC_SUBST(_fvomit_frame_,$vomitframe) to create a substitution that
dnl could be fed as "CFLAGS = @_funsigned_char_@ @_fvomit_frame_@
dnl
dnl in consequence this function is much more general than their 
dnl specific counterparts like ac_cxx_rtti.m4 that will test for
dnl -fno-rtti -fno-exceptions
dnl 
dnl @version $Id: aclocal.m4,v 0.31 2001-03-19 21:50:41 guidod Exp $
dml @author  Guido Draheim <guidod@gmx.de>

AC_DEFUN(AC_CHECK_CC_OPT,
[AC_CACHE_CHECK(whether ${CC-cc} accepts [$1], [$2],
[AC_SUBST($2)
echo 'void f(){}' > conftest.c
if test -z "`${CC-cc} -c $1 conftest.c 2>&1`"; then
  $2="$1"
else
  $2=""
fi
rm -f conftest*
])])


dnl @synopsis AC_DEFINE_PATH_STYLE ([defvar-name])
dnl
dnl _AC_DEFINE(PATH_STYLE) describing the filesys interface. 
dnl The value is numeric, where the basetype is encoded as
dnl 16 = dos/win, 32 = unix, 64 = url/www, 0 = other
dnl
dnl note that there could be a combination of the values
dnl that should lead you to accept multiple forms of
dnl PATH_SEP and DIR_SEP
dnl
dnl @version $Id: aclocal.m4,v 0.31 2001-03-19 21:50:41 guidod Exp $
dnl @author Guido Draheim <guidod@gmx.de>
 
AC_DEFUN([AC_DEFINE_PATH_STYLE], [
AC_CACHE_CHECK([for path style], ac_cv_path_style,
[  
  if test -z "$ac_cv_path_style"; then
    case "$target_os" in
      *djgpp | *mingw32* | *emx*) ac_cv_path_style="dos" ;;
      *) ac_cv_path_style="unix" ;;    # it is just the default ;-)
    esac
    if test "$ac_cv_path_style" = "unix" ; then
      _exec_prefix=`eval "echo $exec_prefix"`
      _exec_prefix=`eval "echo $_exec_prefix"`
      case "$_exec_prefix" in
        *:*) ac_cv_path_style="url" ;;
        *\\) ac_cv_path_style="dos" ;;
      esac
    fi
  fi
])
case "$ac_cv_path_style" in
  *dos*)  ac_define_path_style=16 ;;
  *unix*) ac_define_path_style=32 ;;
  *url*)  ac_define_path_style=64 ;;
  *mac*)  ac_define_path_style=128 ;;
  *) ac_define_path_style=1
esac
AC_DEFINE_UNQUOTED(PATH_STYLE,$ac_define_path_style,dnl
   [path style 16=dos 32=unix 64=url 128=mac])dnl
])


dnl @synopsis AC_DEFINE_DIR_(VARNAME, DIR [, DESCRIPTION])
dnl
dnl This macro _AC_DEFINEs VARNAME to the expansion of the DIR
dnl variable, taking care of fixing up ${prefix} and such.
dnl
dnl Note that the 3 argument form is only supported with autoconf 2.13 and
dnl later (i.e. only where _AC_DEFINE supports 3 arguments).
dnl
dnl Examples:
dnl
dnl    AC_DEFINE_DIR_(DATADIR, datadir)
dnl    AC_DEFINE_DIR_(PROG_PATH, bindir, [Location of installed binaries])
dnl
dnl @version $Id: aclocal.m4,v 0.31 2001-03-19 21:50:41 guidod Exp $
dnl @author Guido Draheim <guidod@gmx.de>, from _AC_DEFINE_DIR by oliva@

AC_DEFUN([AC_DEFINE_DIR_], [
  test "x$prefix" = xNONE && prefix="$ac_default_prefix"
  test "x$exec_prefix" = xNONE && exec_prefix='${prefix}'
  ac_define_dir=`eval "echo [$]$2"`
  ac_define_dir=`eval "echo [$]ac_define_dir"`
  ifelse($3, ,dnl
    AC_DEFINE_UNQUOTED($1, "$ac_define_dir"),dnl
    AC_DEFINE_UNQUOTED($1, "$ac_define_dir", $3))
])

dnl @synopsis AC_DEFINE_DIR(VARNAME, DIR [, DESCRIPTION])
dnl
dnl This macro _AC_DEFINEs VARNAME to the expansion of the DIR
dnl variable, taking care of fixing up ${prefix} and such.
dnl
dnl Note that the 3 argument form is only supported with autoconf 2.13 and
dnl later (i.e. only where _AC_DEFINE supports 3 arguments).
dnl
dnl Examples:
dnl
dnl    AC_DEFINE_DIR(DATADIR, datadir)
dnl    AC_DEFINE_DIR(PROG_PATH, bindir, [Location of installed binaries])
dnl
dnl @version $Id: aclocal.m4,v 0.31 2001-03-19 21:50:41 guidod Exp $
dnl @author Guido Draheim <guidod@gmx.de>, original by Alexandre Oliva

AC_DEFUN([AC_DEFINE_DIR], [
  test "x$prefix" = xNONE && prefix="$ac_default_prefix"
  test "x$exec_prefix" = xNONE && exec_prefix='${prefix}'
  ac_define_dir=`eval echo [$]$2`
  ac_define_dir=`eval echo [$]ac_define_dir`
  ifelse($3, ,
    AC_DEFINE_UNQUOTED($1, "$ac_define_dir"),
    AC_DEFINE_UNQUOTED($1, "$ac_define_dir", $3))
])

dnl @synopsis AC_CREATE_TARGET_H [(HEADER-FILE [,PREFIX)]
dnl
dnl create the header-file and let it contain '#defines'
dnl for the target platform. This macro is used for libraries
dnl that have platform-specific quirks. Instead of inventing a
dnl target-specific target.h.in files, just let it create a
dnl header file from the definitions of AC_CANONICAL_SYSTEM
dnl and put only ifdef's in the installed header-files.
dnl
dnl if the HEADER-FILE is absent, [target.h] is used.
dnl if the PREFIX is absent, [TARGET] is used. 
dnl the prefix can be the packagename. (y:a-z-:A-Z_:)
dnl
dnl the defines look like...
dnl
dnl #ifndef TARGET_CPU_M68K
dnl #define TARGET_CPU_M68K "m68k"
dnl #endif
dnl
dnl #ifndef TARGET_OS_LINUX
dnl #define TARGET_OS_LINUX "linux-gnu"
dnl #endif
dnl
dnl #ifndef TARGET_OS_TYPE                     /* the string itself */
dnl #define TARGET_OS_TYPE "linux-gnu"
dnl #endif
dnl 
dnl detail:  in the case of hppa1.1, the three idents "hppa1_1" "hppa1" 
dnl and "hppa"  are derived, for an m68k it just two, "m68k" and "m"
dnl
dnl the CREATE_TARGET_H__ variant is almost the same function, but everything 
dnl is lowercased instead of uppercased, and there is a "__" in front of
dnl each prefix, so it looks like...
dnl 
dnl #ifndef __target_os_linux
dnl #define __target_os_linux "linux-gnulibc2"
dnl #endif
dnl
dnl #ifndef __target_os__                     /* the string itself */
dnl #define __target_os__ "linux-gnulibc2"
dnl #endif
dnl
dnl #ifndef __target_cpu_i586
dnl #define __target_cpu_i586 "i586"
dnl #endif
dnl
dnl #ifndef __target_arch_i386                
dnl #define __target_arch_i386 "i386"
dnl #endif
dnl
dnl #ifndef __target_arch__                   /* cpu family arch */
dnl #define __target_arch__ "i386"
dnl #endif
dnl
dnl other differences: the default string-define is "__" insteadof "_TYPE" 
dnl
dnl personally I prefer the second variant (which had been the first in
dnl the devprocess of this file but I assume people will often fallback
dnl to the primary variant presented herein).
dnl
dnl NOTE: CREATE_TARGET_H does also fill HOST_OS-defines
dnl functionality has been split over functions called CREATE_TARGET_H_UPPER 
dnl CREATE_TARGET_H_LOWER CREATE_TARGET_HOST_UPPER CREATE_TARGET_HOST_LOWER
dnl CREATE_TARGET_H  uses CREATE_TARGET_H_UPPER    CREATE_TARGET_HOST_UPPER
dnl CREATE_TARGET_H_ uses CREATE_TARGET_H_LOWER    CREATE_TARGET_HOST_LOWER
dnl
dnl there is now a CREATE_PREFIX_TARGET_H in this file as a shorthand for
dnl PREFIX_CONFIG_H from a target.h file, however w/o the target.h ever created
dnl (the prefix is a bit different, since we add an extra -target- and -host-)
dnl 
dnl @version: $Id: aclocal.m4,v 0.31 2001-03-19 21:50:41 guidod Exp $
dnl @author Guido Draheim <guidod@gmx.de>                 STATUS: used often

AC_DEFUN([AC_CREATE_TARGET_H],
[AC_REQUIRE([AC_CANONICAL_CPU_ARCH])
AC_CREATE_TARGET_H_UPPER($1,$2)
AC_CREATE_TARGET_HOST_UPPER($1,$2)
])

AC_DEFUN([AC_CREATE_TARGET_OS_H],
[AC_REQUIRE([AC_CANONICAL_CPU_ARCH])
AC_CREATE_TARGET_H_LOWER($1,$2)
AC_CREATE_TARGET_HOST_LOWER($1,$2)
])

AC_DEFUN([AC_CREATE_TARGET_H__],
[AC_REQUIRE([AC_CANONICAL_CPU_ARCH])
AC_CREATE_TARGET_H_LOWER($1,$2)
AC_CREATE_TARGET_HOST_LOWER($1,$2)
])

dnl [(OUT-FILE [, PREFIX])]  defaults: PREFIX=$PACKAGE OUTFILE=$PREFIX-target.h
AC_DEFUN([AC_CREATE_PREFIX_TARGET_H],[dnl
ac_prefix_conf_PKG=`echo ifelse($2, , $PACKAGE, $2)`
ac_prefix_conf_OUT=`echo ifelse($1, , $ac_prefix_conf_PKG-target.h, $1)`
ac_prefix_conf_PRE=`echo $ac_prefix_conf_PKG-target | sed -e 'y:abcdefghijklmnopqrstuvwxyz-:ABCDEFGHIJKLMNOPQRSTUVWXYZ_:'`
AC_CREATE_TARGET_H_UPPER($ac_prefix_conf_PRE,$ac_perfix_conf_OUT)
ac_prefix_conf_PRE=`echo __$ac_prefix_conf_PKG-host | sed -e 'y:abcdefghijklmnopqrstuvwxyz-:ABCDEFGHIJKLMNOPQRSTUVWXYZ_:'`
AC_CREATE_TARGET_HOST_UPPER($ac_prefix_conf_PRE,$ac_perfix_conf_OUT)
])

dnl [(OUT-FILE[, PREFIX])]  defaults: PREFIX=$PACKAGE OUTFILE=$PREFIX-target.h
AC_DEFUN([AC_CREATE_PREFIX_TARGET_H_],[dnl
ac_prefix_conf_PKG=`echo ifelse($2, , $PACKAGE, $2)`
ac_prefix_conf_OUT=`echo ifelse($1, , $ac_prefix_conf_PKG-target.h, $1)`
ac_prefix_conf_PRE=`echo __$ac_prefix_conf_PKG-target | sed -e 'y:ABCDEFGHIJKLMNOPQRSTUVWXYZ-:abcdefghijklmnopqrstuvwxyz_:'`
AC_CREATE_TARGET_H_LOWER($ac_prefix_conf_PRE,$ac_perfix_conf_OUT)
ac_prefix_conf_PRE=`echo __$ac_prefix_conf_PKG-host | sed -e 'y:ABCDEFGHIJKLMNOPQRSTUVWXYZ-:abcdefghijklmnopqrstuvwxyz_:'`
AC_CREATE_TARGET_HOST_LOWER($ac_prefix_conf_PRE,$ac_perfix_conf_OUT)
])

AC_DEFUN([AC_CREATE_TARGET_H_FILE],[dnl
ac_need_target_h_file_new=true
])

AC_DEFUN([AC_CREATE_TARGET_H_UPPER],
[AC_REQUIRE([AC_CANONICAL_CPU_ARCH])
AC_REQUIRE([AC_CREATE_TARGET_H_FILE])
changequote({, })dnl
ac_need_target_h_file=`echo ifelse($1, , target.h, $1)`
ac_need_target_h_prefix=`echo ifelse($2, , target, $2) | sed -e 'y:abcdefghijklmnopqrstuvwxyz-:ABCDEFGHIJKLMNOPQRSTUVWXYZ_:' -e 's:[^A-Z0-9_]::g'`
#
target_os0=`echo "$target_os"  | sed -e 'y:abcdefghijklmnopqrstuvwxyz.-:ABCDEFGHIJKLMNOPQRSTUVWXYZ__:' -e 's:[^A-Z0-9_]::g'`
target_os1=`echo "$target_os0" | sed -e 's:\([^0-9]*\).*:\1:' `
target_os2=`echo "$target_os0" | sed -e 's:\([^_]*\).*:\1:' `
target_os3=`echo "$target_os2" | sed -e 's:\([^0-9]*\).*:\1:' `
#
target_cpu0=`echo "$target_cpu"  | sed -e 'y:abcdefghijklmnopqrstuvwxyz.-:ABCDEFGHIJKLMNOPQRSTUVWXYZ__:' -e 's:[^A-Z0-9_]::g'`
target_cpu1=`echo "$target_cpu0" | sed -e 's:\([^0-9]*\).*:\1:' `
target_cpu2=`echo "$target_cpu0" | sed -e 's:\([^_]*\).*:\1:' `
target_cpu3=`echo "$target_cpu2" | sed -e 's:\([^0-9]*\).*:\1:' `
#
target_cpu_arch0=`echo "$target_cpu_arch" | sed -e 'y:abcdefghijklmnopqrstuvwxyz:ABCDEFGHIJKLMNOPQRSTUVWXYZ:'`
#
if $ac_need_target_h_file_new ; then
AC_MSG_RESULT(creating $ac_need_target_h_file - canonical system defines)
echo /'*' automatically generated by $PACKAGE configure '*'/ >$ac_need_target_h_file
echo /'*' on `date` '*'/ >>$ac_need_target_h_file
ac_need_target_h_file_new=false
fi
echo /'*' target uppercase defines '*'/ >>$ac_need_target_h_file
dnl
old1=""
old2=""
for i in $target_os0 $target_os1 $target_os2 $target_os3 "TYPE"
do
  if test "$old1" != "$i"; then
  if test "$old2" != "$i"; then
   echo " " >>$ac_need_target_h_file
   echo "#ifndef "$ac_need_target_h_prefix"_OS_"$i >>$ac_need_target_h_file
   echo "#define "$ac_need_target_h_prefix"_OS_"$i '"'"$target_os"'"' >>$ac_need_target_h_file
   echo "#endif" >>$ac_need_target_h_file
  fi
  fi
  old2="$old1"
  old1="$i"
done
#
old1=""
old2=""
for i in $target_cpu0 $target_cpu1 $target_cpu2 $target_cpu3 "TYPE" 
do
  if test "$old1" != "$i"; then
  if test "$old2" != "$i"; then
   echo " " >>$ac_need_target_h_file
   echo "#ifndef "$ac_need_target_h_prefix"_CPU_"$i >>$ac_need_target_h_file
   echo "#define "$ac_need_target_h_prefix"_CPU_"$i '"'"$target_cpu"'"' >>$ac_need_target_h_file
   echo "#endif" >>$ac_need_target_h_file
  fi
  fi
  old2="$old1"
  old1="$i"
done
#
old1=""
old2=""
for i in $target_cpu_arch0 "TYPE" 
do
  if test "$old1" != "$i"; then
  if test "$old2" != "$i"; then
   echo " " >>$ac_need_target_h_file
   echo "#ifndef "$ac_need_target_h_prefix"_ARCH_"$i >>$ac_need_target_h_file
   echo "#define "$ac_need_target_h_prefix"_ARCH_"$i '"'"$target_cpu_arch"'"' >>$ac_need_target_h_file
   echo "#endif" >>$ac_need_target_h_file
  fi
  fi
  old2="$old1"
  old1="$i"
done
#
changequote([, ])dnl
])

dnl
dnl ... the lowercase variant ...
dnl
AC_DEFUN([AC_CREATE_TARGET_H_LOWER],
[AC_REQUIRE([AC_CANONICAL_CPU_ARCH])
AC_REQUIRE([AC_CREATE_TARGET_H_FILE])
changequote({, })dnl
ac_need_target_h_file=`echo ifelse($1, , target-os.h, $1)`
ac_need_target_h_prefix=`echo ifelse($2, , target, $2) | sed -e 'y:ABCDEFGHIJKLMNOPQRSTUVWXYZ-:abcdefghijklmnopqrstuvwxyz_:' -e 's:[^a-z0-9_]::g'`
#
target_os0=`echo "$target_os"  | sed -e 'y:ABCDEFGHIJKLMNOPQRSTUVWXYZ.-:abcdefghijklmnopqrstuvwxyz__:' -e 's:[^a-z0-9_]::g'`
target_os1=`echo "$target_os0" | sed -e 's:\([^0-9]*\).*:\1:' `
target_os2=`echo "$target_os0" | sed -e 's:\([^_]*\).*:\1:' `
target_os3=`echo "$target_os2" | sed -e 's:\([^0-9]*\).*:\1:' `
#
target_cpu0=`echo "$target_cpu"  | sed -e 'y:ABCDEFGHIJKLMNOPQRSTUVWXYZ.-:abcdefghijklmnopqrstuvwxyz__:' -e 's:[^a-z0-9_]::g'`
target_cpu1=`echo "$target_cpu0" | sed -e 's:\([^0-9]*\).*:\1:' `
target_cpu2=`echo "$target_cpu0" | sed -e 's:\([^_]*\).*:\1:' `
target_cpu3=`echo "$target_cpu2" | sed -e 's:\([^0-9]*\).*:\1:' `
#
target_cpu_arch0=`echo "$target_cpu_arch" | sed -e 'y:ABCDEFGHIJKLMNOPQRSTUVWXYZ:abcdefghijklmnopqrstuvwxyz:'`
#
if $ac_need_target_h_file_new ; then
AC_MSG_RESULT(creating $ac_need_target_h_file - canonical system defines)
echo /'*' automatically generated by $PACKAGE configure '*'/ >$ac_need_target_h_file
echo /'*' on `date` '*'/ >>$ac_need_target_h_file
ac_need_target_h_file_new=false
fi
echo /'*' target lowercase defines '*'/ >>$ac_need_target_h_file
dnl
old1=""
old2=""
for i in $target_os0 $target_os1 $target_os2 $target_os3 "_"; 
do
  if test "$old1" != "$i"; then
  if test "$old2" != "$i"; then
   echo " " >>$ac_need_target_h_file
   echo "#ifndef __"$ac_need_target_h_prefix"_os_"$i >>$ac_need_target_h_file
   echo "#define __"$ac_need_target_h_prefix"_os_"$i '"'"$target_os"'"' >>$ac_need_target_h_file
   echo "#endif" >>$ac_need_target_h_file
  fi
  fi
  old2="$old1"
  old1="$i"
done
#
old1=""
old2=""
for i in $target_cpu0 $target_cpu1 $target_cpu2 $target_cpu3 "_" 
do
  if test "$old1" != "$i"; then
  if test "$old2" != "$i"; then
   echo " " >>$ac_need_target_h_file
   echo "#ifndef __"$ac_need_target_h_prefix"_cpu_"$i >>$ac_need_target_h_file
   echo "#define __"$ac_need_target_h_prefix"_cpu_"$i '"'"$target_cpu"'"' >>$ac_need_target_h_file
   echo "#endif" >>$ac_need_target_h_file
  fi
  fi
  old2="$old1"
  old1="$i"
done
#
old1=""
old2=""
for i in $target_cpu_arch0 "_" 
do
  if test "$old1" != "$i"; then
  if test "$old2" != "$i"; then
   echo " " >>$ac_need_target_h_file
   echo "#ifndef __"$ac_need_target_h_prefix"_arch_"$i >>$ac_need_target_h_file
   echo "#define __"$ac_need_target_h_prefix"_arch_"$i '"'"$target_cpu_arch"'"' >>$ac_need_target_h_file
   echo "#endif" >>$ac_need_target_h_file
  fi
  fi
  old2="$old1"
  old1="$i"
done
#
changequote([, ])dnl
])

dnl -------------------------------------------------------------------
dnl
dnl ... the uppercase variant for the host ...
dnl
AC_DEFUN([AC_CREATE_TARGET_HOST_UPPER],
[AC_REQUIRE([AC_CANONICAL_CPU_ARCH])
AC_REQUIRE([AC_CREATE_TARGET_H_FILE])
changequote({, })dnl
ac_need_target_h_file=`echo ifelse($1, , target.h, $1)`
ac_need_target_h_prefix=`echo ifelse($2, , host, $2) | sed -e 'y:abcdefghijklmnopqrstuvwxyz-:ABCDEFGHIJKLMNOPQRSTUVWXYZ_:' -e 'y:[^A-Z0-9_]::g'`
#
host_os0=`echo "$host_os"  | sed -e 'y:abcdefghijklmnopqrstuvwxyz.-:ABCDEFGHIJKLMNOPQRSTUVWXYZ__:' -e 'y:[^A-Z0-9_]::g'`
host_os1=`echo "$host_os0" | sed -e 's:\([^0-9]*\).*:\1:' `
host_os2=`echo "$host_os0" | sed -e 's:\([^_]*\).*:\1:' `
host_os3=`echo "$host_os2" | sed -e 's:\([^0-9]*\).*:\1:' `
#
host_cpu0=`echo "$host_cpu"  | sed -e 'y:abcdefghijklmnopqrstuvwxyz.-:ABCDEFGHIJKLMNOPQRSTUVWXYZ__:' -e 'y:[^A-Z0-9]::g'`
host_cpu1=`echo "$host_cpu0" | sed -e 's:\([^0-9]*\).*:\1:' `
host_cpu2=`echo "$host_cpu0" | sed -e 's:\([^_]*\).*:\1:' `
host_cpu3=`echo "$host_cpu2" | sed -e 's:\([^0-9]*\).*:\1:' `
#
host_cpu_arch0=`echo "$host_cpu_arch" | sed -e 'y:abcdefghijklmnopqrstuvwxyz:ABCDEFGHIJKLMNOPQRSTUVWXYZ:'`
#
if $ac_need_target_h_file_new ; then
AC_MSG_RESULT(creating $ac_need_target_h_file - canonical system defines)
echo /'*' automatically generated by $PACKAGE configure '*'/ >$ac_need_target_h_file
echo /'*' on `date` '*'/ >>$ac_need_target_h_file
ac_need_target_h_file_new=false
fi
echo /'*' host uppercase defines '*'/ >>$ac_need_target_h_file
dnl
old1=""
old2=""
for i in $host_os0 $host_os1 $host_os2 $host_os3 "TYPE"
do
  if test "$old1" != "$i"; then
  if test "$old2" != "$i"; then
   echo " " >>$ac_need_target_h_file
   echo "#ifndef "$ac_need_target_h_prefix"_OS_"$i >>$ac_need_target_h_file
   echo "#define "$ac_need_target_h_prefix"_OS_"$i '"'"$host_os"'"' >>$ac_need_target_h_file
   echo "#endif" >>$ac_need_target_h_file
  fi
  fi
  old2="$old1"
  old1="$i"
done
#
old1=""
old2=""
for i in $host_cpu0 $host_cpu1 $host_cpu2 $host_cpu3 "TYPE" 
do
  if test "$old1" != "$i"; then
  if test "$old2" != "$i"; then
   echo " " >>$ac_need_target_h_file
   echo "#ifndef "$ac_need_target_h_prefix"_CPU_"$i >>$ac_need_target_h_file
   echo "#define "$ac_need_target_h_prefix"_CPU_"$i '"'"$host_cpu"'"' >>$ac_need_target_h_file
   echo "#endif" >>$ac_need_target_h_file
  fi
  fi
  old2="$old1"
  old1="$i"
done
#
old1=""
old2=""
for i in $host_cpu_arch0 "TYPE" 
do
  if test "$old1" != "$i"; then
  if test "$old2" != "$i"; then
   echo " " >>$ac_need_target_h_file
   echo "#ifndef "$ac_need_target_h_prefix"_ARCH_"$i >>$ac_need_target_h_file
   echo "#define "$ac_need_target_h_prefix"_ARCH_"$i '"'"$host_cpu_arch"'"' >>$ac_need_target_h_file
   echo "#endif" >>$ac_need_target_h_file
  fi
  fi
  old2="$old1"
  old1="$i"
done
#
changequote([, ])dnl
])

dnl ---------------------------------------------------------------------
dnl
dnl ... the lowercase variant for the host ...
dnl
AC_DEFUN([AC_CREATE_TARGET_HOST_LOWER],
[AC_REQUIRE([AC_CANONICAL_CPU_ARCH])
AC_REQUIRE([AC_CREATE_TARGET_H_FILE])
changequote({, })dnl
ac_need_target_h_file=`echo ifelse($1, , target.h, $1)`
ac_need_target_h_prefix=`echo ifelse($2, , host, $2) | sed -e 'y:ABCDEFGHIJKLMNOPQRSTUVWXYZ-:abcdefghijklmnopqrstuvwxyz_:' -e 's:[^a-z0-9_]::g'`
#
host_os0=`echo "$host_os"  | sed -e 'y:ABCDEFGHIJKLMNOPQRSTUVWXYZ.-:abcdefghijklmnopqrstuvwxyz__:' -e 's:[^a-z0-9_]::g'`
host_os1=`echo "$host_os0" | sed -e 's:\([^0-9]*\).*:\1:' `
host_os2=`echo "$host_os0" | sed -e 's:\([^_]*\).*:\1:' `
host_os3=`echo "$host_os2" | sed -e 's:\([^0-9]*\).*:\1:' `
#
host_cpu0=`echo "$host_cpu"  | sed -e 'y:ABCDEFGHIJKLMNOPQRSTUVWXYZ.-:abcdefghijklmnopqrstuvwxyz__:' -e 's:[^a-z0-9_]::g'`
host_cpu1=`echo "$host_cpu0" | sed -e 's:\([^0-9]*\).*:\1:' `
host_cpu2=`echo "$host_cpu0" | sed -e 's:\([^_]*\).*:\1:' `
host_cpu3=`echo "$host_cpu2" | sed -e 's:\([^0-9]*\).*:\1:' `
#
host_cpu_arch0=`echo "$host_cpu_arch" | sed -e 'y:ABCDEFGHIJKLMNOPQRSTUVWXYZ:abcdefghijklmnopqrstuvwxyz:'`
#
if $ac_need_target_h_file_new ; then
AC_MSG_RESULT(creating $ac_need_target_h_file - canonical system defines)
echo /'*' automatically generated by $PACKAGE configure '*'/ >$ac_need_target_h_file
echo /'*' on `date` '*'/ >>$ac_need_target_h_file
ac_need_target_h_file_new=false
fi
echo /'*' host lowercase defines '*'/ >>$ac_need_target_h_file
dnl
old1=""
old2=""
for i in $host_os0 $host_os1 $host_os2 $host_os3 "_"; 
do
  if test "$old1" != "$i"; then
  if test "$old2" != "$i"; then
   echo " " >>$ac_need_target_h_file
   echo "#ifndef __"$ac_need_target_h_prefix"_os_"$i >>$ac_need_target_h_file
   echo "#define __"$ac_need_target_h_prefix"_os_"$i '"'"$host_os"'"' >>$ac_need_target_h_file
   echo "#endif" >>$ac_need_target_h_file
  fi
  fi
  old2="$old1"
  old1="$i"
done
#
old1=""
old2=""
for i in $host_cpu0 $host_cpu1 $host_cpu2 $host_cpu3 "_" 
do
  if test "$old1" != "$i"; then
  if test "$old2" != "$i"; then
   echo " " >>$ac_need_target_h_file
   echo "#ifndef __"$ac_need_target_h_prefix"_cpu_"$i >>$ac_need_target_h_file
   echo "#define __"$ac_need_target_h_prefix"_cpu_"$i '"'"$host_cpu"'"' >>$ac_need_target_h_file
   echo "#endif" >>$ac_need_target_h_file
  fi
  fi
  old2="$old1"
  old1="$i"
done
#
old1=""
old2=""
for i in $host_cpu_arch0 "_" 
do
  if test "$old1" != "$i"; then
  if test "$old2" != "$i"; then
   echo " " >>$ac_need_target_h_file
   echo "#ifndef __"$ac_need_target_h_prefix"_arch_"$i >>$ac_need_target_h_file
   echo "#define __"$ac_need_target_h_prefix"_arch_"$i '"'"$host_cpu_arch"'"' >>$ac_need_target_h_file
   echo "#endif" >>$ac_need_target_h_file
  fi
  fi
  old2="$old1"
  old1="$i"
done
#
changequote([, ])dnl
])

dnl -------------------------------------------------------------------

dnl
dnl the instruction set architecture (ISA) has evolved for a small set
dnl of cpu types. So they often have specific names, e.g. sparclite,
dnl yet they share quite a few similarities. This macro will set the
dnl shell-var $target_cpu_arch to the basic type. Note that these
dnl names are often in conflict with their original 32-bit type name
dnl of these processors, just use them for directory-handling or add
dnl a prefix/suffix to distinguish them from $target_cpu
dnl
dnl this macros has been invented since config.guess is sometimes
dnl too specific about the cpu-type. I chose the names along the lines
dnl of linux/arch/ which is modelled after widespread arch-naming, IMHO.
dnl
AC_DEFUN([AC_CANONICAL_CPU_ARCH],
[AC_REQUIRE([AC_CANONICAL_SYSTEM])
target_cpu_arch="unknown"
case $target_cpu in
 i386*|i486*|i586*|i686*|i786*) target_cpu_arch=i386 ;;
 power*)   target_cpu_arch=ppc ;;
 arm*)     target_cpu_arch=arm ;;
 sparc64*) target_cpu_arch=sparc64 ;;
 sparc*)   target_cpu_arch=sparc ;;
 mips64*)  target_cpu_arch=mips64 ;;
 mips*)    target_cpu_arch=mips ;;
 alpha*)   target_cpu_arch=alpha ;;
 hppa1*)   target_cpu_arch=hppa1 ;;
 hppa2*)   target_cpu_arch=hppa2 ;;
 arm*)     target_cpu_arch=arm ;;
 m68???|mcf54??) target_cpu_arch=m68k ;;
 *)        target_cpu_arch="$target_cpu" ;;
esac

host_cpu_arch="unknown"
case $host_cpu in
 i386*|i486*|i586*|i686*|i786*) host_cpu_arch=i386 ;;
 power*)   host_cpu_arch=ppc ;;
 arm*)     host_cpu_arch=arm ;;
 sparc64*) host_cpu_arch=sparc64 ;;
 sparc*)   host_cpu_arch=sparc ;;
 mips64*)  host_cpu_arch=mips64 ;;
 mips*)    host_cpu_arch=mips ;;
 alpha*)   host_cpu_arch=alpha ;;
 hppa1*)   host_cpu_arch=hppa1 ;;
 hppa2*)   host_cpu_arch=hppa2 ;;
 arm*)     host_cpu_arch=arm ;;
 m68???|mcf54??) host_cpu_arch=m68k ;;
 *)        host_cpu_arch="$target_cpu" ;;
esac
])



dnl @synopsis AC_CREATE_GENERIC_CONFIG [(PACKAGEnlibs [, VERSION])]
dnl
dnl Creates a generic PACKAGE-config file that has all the
dnl things that you want, hmm, well, atleast it has
dnl --cflags, --version, --libs. Ahhm, did you see ac_path_generic
dnl in the autoconf-archive? ;-)
dnl
dnl this macros saves you all the typing for a pkg-config.in script,
dnl you don't even need to distribute one along. Place this macro
dnl in your configure.ac, et voila, you got one that you want to install.
dnl
dnl oh, btw, if the first arg looks like "mylib -lwhat' then it
dnl will go to be added to the --libs, and mylib is extracted.
dnl
dnl the defaults: $1 = $PACKAGE $LIBS  $2 = $VERSION
dnl there is also an AC_SUBST(GENERIC_CONFIG) that will be set to 
dnl the name of the file that we did output in this macro. Use as:
dnl
dnl install-exec-local: install-generic-config
dnl install-generic-config:
dnl 	$(mkinstalldirs) $(DESTDIR)$(bindir)
dnl 	$(INSTALL_SCRIPT) @GENERIC_CONFIG@ $(DESTDIR)$(bindir)
dnl
dnl @version $Id: aclocal.m4,v 0.31 2001-03-19 21:50:41 guidod Exp $
dnl @author Guido Draheim <guidod@gmx.de>

AC_DEFUN([AC_CREATE_GENERIC_CONFIG],[# create a generic PACKAGE-config file
L=`echo ifelse($1, , $PACKAGE $LIBS, $1)`
P=`echo $L | sed -e 's/ -.*//'`
P=`echo $P`
V=`echo ifelse($1, , $VERSION, $1)`
F=`echo $P-config`
AC_MSG_RESULT(creating $F - generic $V of $L)
test "x$prefix" = xNONE && prefix="$ac_default_prefix"
test "x$exec_prefix" = xNONE && exec_prefix='${prefix}'
echo '#! /bin/sh' >$F
echo ' ' >>$F
echo 'package="'$P'"' >>$F
echo 'version="'$V'"' >>$F
echo 'libs="'$L'"' >>$F
echo ' ' >>$F
# in the order of occurence a standard automake Makefile
echo 'prefix="'$prefix'"' >>$F
echo 'exec_prefix="'$exec_prefix'"' >>$F
echo 'bindir="'$bindir'"' >>$F
echo 'sbindir="'$sbindir'"' >>$F
echo 'libexecdir="'$libexecdir'"' >>$F
echo 'datadir="'$datadir'"' >>$F
echo 'sysconfdir="'$sysconfdir'"' >>$F
echo 'sharedstatedir="'$sharedstatedir'"' >>$F
echo 'localstatedir="'$localstatedir'"' >>$F
echo 'libdir="'$libdir'"' >>$F
echo 'infodir="'$infodir'"' >>$F
echo 'mandir="'$mandir'"' >>$F
echo 'includedir="'$includedir'"' >>$F
echo 'target="'$target'"' >>$F
echo 'host="'$host'"' >>$F
echo 'build="'$build'"' >>$F
echo ' ' >>$F
echo 'if test "'"\$""#"'" -eq 0; then' >>$F
echo '   cat <<EOF' >>$F
echo 'Usage: $package-config [OPTIONS]' >>$F
echo 'Options:' >>$F
echo '  --prefix[=DIR]) : \$prefix' >>$F
echo '  --package) : \$package' >>$F
echo '  --version) : \$version' >>$F
echo '  --cflags) : -I\$includedir' >>$F
echo '  --libs) : -L\$libdir -l\$package' >>$F
echo '  --help) print all the options (not just these)' >>$F
echo 'EOF' >>$F
echo 'fi' >>$F
echo ' ' >>$F
echo 'o=""' >>$F
echo 'h=""' >>$F
echo 'for i ; do' >>$F
echo '  case $i in' >>$F
echo '  --prefix=*) prefix=`echo $i | sed -e "s/--prefix=//"` ;;' >>$F
echo '  --prefix)    o="$o $prefix" ;;' >>$F
echo '  --package)   o="$o $package" ;;' >>$F
echo '  --version)   o="$o $version" ;;' >>$F
echo '  --cflags) if test "_$includedir" != "_/usr/include"' >>$F
echo '          then o="$o -I$includedir" ; fi' >>$F
echo '  ;;' >>$F
echo '  --libs)      o="$o -L$libdir -l$libs" ;;' >>$F
echo '  --exec_prefix|--eprefix) o="$o $exec_prefix" ;;' >>$F
echo '  --bindir)                o="$o $bindir" ;;' >>$F
echo '  --sbindir)               o="$o $sbindir" ;;' >>$F
echo '  --libexecdir)            o="$o $libexecdir" ;;' >>$F
echo '  --datadir)               o="$o $datadir" ;;' >>$F
echo '  --datainc)               o="$o -I$datadir" ;;' >>$F
echo '  --datalib)               o="$o -L$datadir" ;;' >>$F
echo '  --sysconfdir)            o="$o $sysconfdir" ;;' >>$F
echo '  --sharedstatedir)        o="$o $sharedstatedir" ;;' >>$F
echo '  --localstatedir)         o="$o $localstatedir" ;;' >>$F
echo '  --libdir)                o="$o $libdir" ;;' >>$F
echo '  --libadd)                o="$o -L$libdir" ;;' >>$F
echo '  --infodir)               o="$o $infodir" ;;' >>$F
echo '  --mandir)                o="$o $mandir" ;;' >>$F
echo '  --target)                o="$o $target" ;;' >>$F
echo '  --host)                  o="$o $host" ;;' >>$F
echo '  --build)                 o="$o $build" ;;' >>$F
echo '  --data)                  o="$o -I$datadir/$package" ;;' >>$F
echo '  --pkgdatadir)            o="$o $datadir/$package" ;;' >>$F
echo '  --pkgdatainc)            o="$o -I$datadir/$package" ;;' >>$F
echo '  --pkgdatalib)            o="$o -L$datadir/$package" ;;' >>$F
echo '  --pkglibdir)             o="$o $libdir/$package" ;;' >>$F
echo '  --pkglibinc)             o="$o -I$libinc/$package" ;;' >>$F
echo '  --pkglibadd)             o="$o -L$libadd/$package" ;;' >>$F
echo '  --pkgincludedir)         o="$o $includedir/$package" ;;' >>$F
echo '  --help) h="1" ;;' >>$F
echo '  -?//*|-?/*//*|-?./*//*|//*|/*//*|./*//*) ' >>$F
echo '       v=`echo $i | sed -e s://:\$:g`' >>$F
echo '       v=`eval "echo $v"` ' >>$F
echo '       o="$o $v" ;; ' >>$F
echo '  esac' >>$F
echo 'done' >>$F
echo ' ' >>$F
echo 'o=`eval "echo $o"`' >>$F
echo 'o=`eval "echo $o"`' >>$F
echo 'eval "echo $o"' >>$F
echo ' ' >>$F
echo 'if test ! -z "$h" ; then ' >>$F
echo 'cat <<EOF' >>$F
echo '  --prefix=xxx)      (what is that for anyway?)' >>$F
echo '  --prefix)         \$prefix        $prefix' >>$F
echo '  --package)        \$package       $package' >>$F
echo '  --version)        \$version       $version' >>$F
echo '  --cflags)         -I\$includedir    unless it is /usr/include' >>$F
echo '  --libs)           -L\$libdir -l\$PACKAGE \$LIBS' >>$F
echo '  --exec_prefix) or... ' >>$F
echo '  --eprefix)        \$exec_prefix   $exec_prefix' >>$F
echo '  --bindir)         \$bindir        $bindir' >>$F
echo '  --sbindir)        \$sbindir       $sbindir' >>$F
echo '  --libexecdir)     \$libexecdir    $libexecdir' >>$F
echo '  --datadir)        \$datadir       $datadir' >>$F
echo '  --sysconfdir)     \$sysconfdir    $sysconfdir' >>$F
echo '  --sharedstatedir) \$sharedstatedir$sharedstatedir' >>$F
echo '  --localstatedir)  \$localstatedir $localstatedir' >>$F
echo '  --libdir)         \$libdir        $libdir' >>$F
echo '  --infodir)        \$infodir       $infodir' >>$F
echo '  --mandir)         \$mandir        $mandir' >>$F
echo '  --target)         \$target        $target' >>$F
echo '  --host)           \$host          $host' >>$F
echo '  --build)          \$build         $build' >>$F
echo '  --data)           -I\$datadir/\$package' >>$F
echo '  --pkgdatadir)     \$datadir/\$package' >>$F
echo '  --pkglibdir)      \$libdir/\$package' >>$F
echo '  --pkgincludedir)  \$includedir/\$package' >>$F
echo '  --help)           generated by ac_create_generic_config.m4' >>$F
echo '  -I//varname and other inc-targets like --pkgdatainc supported' >>$F
echo '  -L//varname and other lib-targets, e.g. --pkgdatalib or --libadd' >>$F
echo 'EOF' >>$F
echo 'fi' >>$F 
GENERIC_CONFIG="$F"
AC_SUBST(GENERIC_CONFIG)
])









dnl @synopsis AC_CREATE_PREFIX_CONFIG_H [(OUTPUT-HEADER [,PREFIX [,ORIG-HEADER]])]
dnl
dnl this is a new variant from ac_prefix_config_
dnl   this one will use a lowercase-prefix if
dnl   the config-define was starting with a lowercase-char, e.g. 
dnl   #define const or #define restrict or #define off_t
dnl   (and this one can live in another directory, e.g. testpkg/config.h
dnl    therefore I decided to move the output-header to be the first arg)
dnl
dnl takes the usual config.h generated header file; looks for each of
dnl the generated "#define SOMEDEF" lines, and prefixes the defined name
dnl (ie. makes it "#define PREFIX_SOMEDEF". The result is written to
dnl the output config.header file. The PREFIX is converted to uppercase 
dnl for the conversions. 
dnl
dnl default OUTPUT-HEADER = $PACKAGE-config.h
dnl default PREFIX = $PACKAGE
dnl default ORIG-HEADER, derived from OUTPUT-HEADER
dnl         if OUTPUT-HEADER has a "/", use the basename
dnl         if OUTPUT-HEADER has a "-", use the section after it.
dnl         otherwise, just config.h
dnl
dnl In most cases, the configure.in will contain a line saying
dnl         AC_CONFIG_HEADER(config.h) 
dnl somewhere *before* AC_OUTPUT and a simple line saying
dnl        AC_PREFIX_CONFIG_HEADER
dnl somewhere *after* AC_OUTPUT.
dnl
dnl example:
dnl   AC_INIT(config.h.in)        # config.h.in as created by "autoheader"
dnl   AM_INIT_AUTOMAKE(testpkg, 0.1.1)   # "#undef VERSION" and "PACKAGE"
dnl   AM_CONFIG_HEADER(config.h)         #                in config.h.in
dnl   AC_MEMORY_H                        # "#undef NEED_MEMORY_H"
dnl   AC_C_CONST_H                       # "#undef const"
dnl   AC_OUTPUT(Makefile)                # creates the "config.h" now
dnl   AC_CREATE_PREFIX_CONFIG_H          # creates "testpkg-config.h"
dnl         and the resulting "testpkg-config.h" contains lines like
dnl   #ifndef TESTPKG_VERSION 
dnl   #define TESTPKG_VERSION "0.1.1"
dnl   #endif
dnl   #ifndef TESTPKG_NEED_MEMORY_H 
dnl   #define TESTPKG_NEED_MEMORY_H 1
dnl   #endif
dnl   #ifndef _testpkg_const 
dnl   #define _testpkg_const const
dnl   #endif
dnl
dnl   and this "testpkg-config.h" can be installed along with other
dnl   header-files, which is most convenient when creating a shared
dnl   library (that has some headers) where some functionality is
dnl   dependent on the OS-features detected at compile-time. No
dnl   need to invent some "testpkg-confdefs.h.in" manually. :-)
dnl
dnl @version $Id: aclocal.m4,v 0.31 2001-03-19 21:50:41 guidod Exp $
dnl @author Guido Draheim <guidod@gmx.de>

AC_DEFUN([AC_CREATE_PREFIX_CONFIG_H],
[changequote({, })dnl 
ac_prefix_conf_OUT=`echo ifelse($1, , $PACKAGE-config.h, $1)`
ac_prefix_conf_DEF=`echo _$ac_prefix_conf_OUT | sed -e 'y:abcdefghijklmnopqrstuvwxyz./,-:ABCDEFGHIJKLMNOPQRSTUVWXYZ____:'`
ac_prefix_conf_PKG=`echo ifelse($2, , $PACKAGE, $2)`
ac_prefix_conf_LOW=`echo _$ac_prefix_conf_PKG | sed -e 'y:ABCDEFGHIJKLMNOPQRSTUVWXYZ-:abcdefghijklmnopqrstuvwxyz_:'`
ac_prefix_conf_UPP=`echo $ac_prefix_conf_PKG | sed -e 'y:abcdefghijklmnopqrstuvwxyz-:ABCDEFGHIJKLMNOPQRSTUVWXYZ_:'  -e '/^[0-9]/s/^/_/'`
ac_prefix_conf_INP=`echo ifelse($3, , _, $3)`
if test "$ac_prefix_conf_INP" = "_"; then
   case $ac_prefix_conf_OUT in
      */*) ac_prefix_conf_INP=`basename $ac_prefix_conf_OUT` 
      ;;
      *-*) ac_prefix_conf_INP=`echo $ac_prefix_conf_OUT | sed -e 's/[a-zA-Z0-9_]*-//'`
      ;;
      *) ac_prefix_conf_INP=config.h
      ;;
   esac
fi
changequote([, ])dnl
if test -z "$ac_prefix_conf_PKG" ; then
   AC_MSG_ERROR([no prefix for _PREFIX_PKG_CONFIG_H])
else
  AC_MSG_RESULT(creating $ac_prefix_conf_OUT - prefix $ac_prefix_conf_UPP for $ac_prefix_conf_INP defines)
  if test -f $ac_prefix_conf_INP ; then
    AC_ECHO_MKFILE([/* automatically generated */], $ac_prefix_conf_OUT)
changequote({, })dnl 
    echo '#ifndef '$ac_prefix_conf_DEF >>$ac_prefix_conf_OUT
    echo '#define '$ac_prefix_conf_DEF' 1' >>$ac_prefix_conf_OUT
    echo ' ' >>$ac_prefix_conf_OUT
    echo /'*' $ac_prefix_conf_OUT. Generated automatically at end of configure. '*'/ >>$ac_prefix_conf_OUT

    echo 's/#undef  *\([A-Z_]\)/#undef '$ac_prefix_conf_UPP'_\1/' >conftest.sed
    echo 's/#undef  *\([a-z]\)/#undef '$ac_prefix_conf_LOW'_\1/' >>conftest.sed
    echo 's/#define  *\([A-Z_][A-Za-z0-9_]*\)\(.*\)/#ifndef '$ac_prefix_conf_UPP"_\\1 \\" >>conftest.sed
    echo '#define '$ac_prefix_conf_UPP"_\\1 \\2 \\" >>conftest.sed
    echo '#endif/' >>conftest.sed
    echo 's/#define  *\([a-z][A-Za-z0-9_]*\)\(.*\)/#ifndef '$ac_prefix_conf_LOW"_\\1 \\" >>conftest.sed
    echo '#define '$ac_prefix_conf_LOW"_\\1 \\2 \\" >>conftest.sed
    echo '#endif/' >>conftest.sed
    sed -f conftest.sed $ac_prefix_conf_INP >>$ac_prefix_conf_OUT
    echo ' ' >>$ac_prefix_conf_OUT
    echo '/*' $ac_prefix_conf_DEF '*/' >>$ac_prefix_conf_OUT
    echo '#endif' >>$ac_prefix_conf_OUT
changequote([, ])dnl
  else
    AC_MSG_ERROR([input file $ac_prefix_conf_IN does not exist, dnl
    skip generating $ac_prefix_conf_OUT])
  fi
  rm -f conftest.* 
fi])
           

dnl @synopsis AC_PREFIX_CONFIG_H [(PREFIX [,ORIG-HEADER [,OUTPUT-HEADER]])]
dnl                                superseded by ac_create_prefix_config_h
dnl
dnl takes the usual config.h generated header file; looks for each of
dnl the generated "#define SOMEDEF" lines, and prefixes the defined name
dnl (ie. makes it "#define PREFIX_SOMEDEF". The result is written to
dnl the output config.header file. The PREFIX is converted to uppercase 
dnl for the conversions. If PREFIX is absent, $PACKAGE will be assumed.
dnl If the ORIG-HEADER is absent, "config.h" will be assumed.
dnl If the OUTPUT-HEADER is absent, "PREFIX-config.h" will be assumed. 
dnl
dnl In most cases, the configure.in will contain a line saying
dnl         AC_CONFIG_HEADER(config.h) 
dnl somewhere *before* AC_OUTPUT and a simple line saying
dnl        AC_PREFIX_CONFIG_HEADER
dnl somewhere *after* AC_OUTPUT.
dnl
dnl example:
dnl   AC_INIT(config.h.in)        # config.h.in as created by "autoheader"
dnl   AM_INIT_AUTOMAKE(testpkg, 0.1.1)   # "#undef VERSION" and "PACKAGE"
dnl   AM_CONFIG_HEADER(config.h)         #                in config.h.in
dnl   AC_MEMORY_H                        # "#undef NEED_MEMORY_H"
dnl   AC_C_CONST_H                       # "#undef const"
dnl   AC_OUTPUT(Makefile)                # creates the "config.h" now
dnl   AC_PREFIX_CONFIG_H                 # creates "testpkg-config.h"
dnl         and the resulting "testpkg-config.h" contains lines like
dnl   #define TESTPKG_VERSION "0.1.1"
dnl   #define TESTPKG_NEED_MEMORY_H 1
dnl   #define TESTPKG_const const
dnl
dnl   and this "testpkg-config.h" can be installed along with other
dnl   header-files, which is most convenient when creating a shared
dnl   library (that has some headers) where some functionality is
dnl   dependent on the OS-features detected at compile-time. No
dnl   need to invent some "testpkg-confdefs.h.in" manually. :-)
dnl
dnl @version $Id: aclocal.m4,v 0.31 2001-03-19 21:50:41 guidod Exp $
dnl @author Guido Draheim <guidod@gmx.de>

AC_DEFUN([AC_PREFIX_CONFIG_H],
[changequote(<<, >>)dnl 
ac_prefix_conf_PKG=`echo ifelse($1, , $PACKAGE, $1)`
ac_prefix_conf_PRE=`echo $ac_prefix_conf_PKG | tr 'abcdefghijklmnopqrstuvwxyz-' 'ABCDEFGHIJKLMNOPQRSTUVWXYZ_'`
ac_prefix_conf_PRE=`echo $ac_prefix_conf_PRE | sed -e '/^[0-9]/s/^/_/'`
ac_prefix_conf_INP=`echo ifelse($2, , config.h, $2)`
ac_prefix_conf_OUT=`echo ifelse($3, , $ac_prefix_conf_PKG-$ac_prefix_conf_INP, $3)`
ac_prefix_conf_DEF=`echo _$ac_prefix_conf_OUT | tr 'abcdefghijklmnopqrstuvwxyz./,-' 'ABCDEFGHIJKLMNOPQRSTUVWXYZ____'`
changequote([, ])dnl
if test -z "$ac_prefix_conf_PKG" ; then
   AC_MSG_ERROR([no prefix for _PREFIX_CONFIG_H])
else
  AC_MSG_RESULT(creating $ac_prefix_conf_OUT - prefix $ac_prefix_conf_PRE for $ac_prefix_conf_INP defines)
  if test -f $ac_prefix_conf_INP ; then
    echo '#ifndef '$ac_prefix_conf_DEF >$ac_prefix_conf_OUT
    echo '#define '$ac_prefix_conf_DEF' 1' >>$ac_prefix_conf_OUT
    echo ' ' >>$ac_prefix_conf_OUT
    echo /'*' $ac_prefix_conf_OUT. Generated automatically at end of configure. '*'/ >>$ac_prefix_conf_OUT

    echo 's/#undef  */#undef '$ac_prefix_conf_PRE'_/' >conftest.sed
    echo 's/#define  *\([A-Za-z0-9_]*\)\(.*\)/#ifndef '$ac_prefix_conf_PRE"_\\1 \\" >>conftest.sed
    echo '#define '$ac_prefix_conf_PRE"_\\1 \\2 \\" >>conftest.sed
    echo '#endif/' >>conftest.sed
    sed -f conftest.sed $ac_prefix_conf_INP >>$ac_prefix_conf_OUT
    echo ' ' >>$ac_prefix_conf_OUT
    echo '/*' $ac_prefix_conf_DEF '*/' >>$ac_prefix_conf_OUT
    echo '#endif' >>$ac_prefix_conf_OUT
  else
    AC_MSG_ERROR([input file $ac_prefix_conf_IN does not exist, dnl
    skip generating $ac_prefix_conf_OUT])
  fi
  rm -f conftest.* 
fi])
           

dnl AC_AS_DIRNAME (PATH)
dnl this is the macro AS_DIRNAME from autoconf 2.4x
dnl defined here for use in autoconf 2.1x, remove the AC_ when you use 2.4x 
dnl
dnl @version $Id: aclocal.m4,v 0.31 2001-03-19 21:50:41 guidod Exp $
dnl @author complain to <guidod@gmx.de>

AC_DEFUN([AC_ECHO_MKFILE],
[dnl
  case $2 in
    */*) P=` AC_AS_DIRNAME($2) ` ; AC_AS_MKDIR_P($P) ;;
  esac
  echo "$1" >$2
])

AC_DEFUN([AC_AS_DIRNAME],
[AC_AS_DIRNAME_EXPR([$1]) 2>/dev/null ||
AC_AS_DIRNAME_SED([$1])
])


# _AC_AS_EXPR_PREPARE
# ----------------
# Some expr work properly (i.e. compute and issue the right result),
# but exit with failure.  When a fall back to expr (as in AS_DIRNAME)
# is provided, you get twice the result.  Prevent this.
AC_DEFUN([_AC_AS_EXPR_PREPARE],
[if expr a : '\(a\)' >/dev/null 2>&1; then
  as_expr=expr
else
  as_expr=false
fi
])# _AC_AS_EXPR_PREPARE


# AS_DIRNAME(PATHNAME)
# --------------------
# Simulate running `dirname(1)' on PATHNAME, not all systems have it.
# This macro must be usable from inside ` `.
#
# Prefer expr to echo|sed, since expr is usually faster and it handles
# backslashes and newlines correctly.  However, older expr
# implementations (e.g. SunOS 4 expr and Solaris 8 /usr/ucb/expr) have
# a silly length limit that causes expr to fail if the matched
# substring is longer than 120 bytes.  So fall back on echo|sed if
# expr fails.
#
# FIXME: Please note the following m4_require is quite wrong: if the first
# occurrence of AS_DIRNAME_EXPR is in a backquoted expression, the
# shell will be lost.  We might have to introduce diversions for
# setting up an M4sh script: required macros will then be expanded there.

AC_DEFUN([AC_AS_DIRNAME_EXPR],
[AC_REQUIRE([_AC_AS_EXPR_PREPARE])dnl
$as_expr X[]$1 : 'X\(.*[[^/]]\)//*[[^/][^/]]*/*$' \| \
         X[]$1 : 'X\(//\)[[^/]]' \| \
         X[]$1 : 'X\(//\)$' \| \
         X[]$1 : 'X\(/\)' \| \
         .     : '\(.\)'])

AC_DEFUN([AC_AS_DIRNAME_SED],
[echo X[]$1 |
    sed ['/^X\(.*[^/]\)\/\/*[^/][^/]*\/*$/{ s//\1/; q; }
          /^X\(\/\/\)[^/].*/{ s//\1/; q; }
          /^X\(\/\/\)$/{ s//\1/; q; }
          /^X\(\/\).*/{ s//\1/; q; }
          s/.*/./; q']])




dnl AC_AS_MKDIR_P(PATH)
dnl this is the macro AS_MKDIR_P from autoconf 2.4x
dnl defined here for use in autoconf 2.1x, remove the AC_ when you use 2.4x 
dnl
dnl @version $Id: aclocal.m4,v 0.31 2001-03-19 21:50:41 guidod Exp $
dnl @author complain to <guidod@gmx.de>

AC_DEFUN([AC_AS_MKDIR_P],[dnl
case $1 in
  \\*)   ac_incr_dir=\\;;
  ?:\\*) ac_incr_dir=\\;; # ouch
  /*)    ac_incr_dir=/;;
  ?:/*)  ac_incr_dir=/;; # ouch
  *)     ac_incr_dir=.;;
esac
as_dummy=$1
for as_mkdir_dir in `echo $ac_dummy | sed -e 'y:\\/:  :'`; do
  case $as_mkdir_dir in
    # Skip DOS drivespec
    ?:) as_incr_dir=$as_mkdir_dir ;;
    *)
      as_incr_dir=$as_incr_dir/$as_mkdir_dir
      test -d "$as_incr_dir" || mkdir "$as_incr_dir"
    ;;
  esac
done
])

