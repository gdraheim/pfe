dnl Process this file with autoconf to produce a configure script.
AC_INIT(pfe.h)         
   if test -z "$CC" ; then configurecall="$0 $ac_configure_args"
   else configurecall="CC=$CC $0 $ac_configure_args"
   fi AC_SUBST(configurecall)
AC_PREREQ(2.49)
AC_COPYRIGHT([Guido U. Draheim <guidod@gmx.de> for PFE.sourceforge.net])
AC_REVISION($Revision: 1.4 $)
AC_CONFIG_AUX_DIR([../uses])

# a linux fix where available - the newer gcc 3.x release are not too pretty
if test -z "$CC" ; then if test -f /usr/bin/gcc-2.96 ; then CC=gcc-2.96 ; fi fi

AC_CANONICAL_HOST
AC_CANONICAL_SYSTEM
AC_LANG_C
AC_PROG_CC

AC_SET_DEFAULT_PATHS_SYSTEM
AX_SPEC_DEFAULTS(pfe.spec)
AM_INIT_AUTOMAKE($PACKAGE, $VERSION)
AC_CONFIG_HEADER(config.h)
AX_PREFIX_CONFIG_H(_config.h)
AX_SET_VERSION_INFO
AC_DEFINE_VERSIONLEVEL(PFE_CONFIGVERSION)

AC_ARG_WITH(ltdl,
[  --with-ltdl             build and install libtool dlopen convenience kit ])
AX_SUBST_WITH_ARG(ltdl,no)
if test "$with_ltdl" != "no"; then 
   AC_LIBLTDL_INSTALLABLE
fi

AC_LIBTOOL_DLOPEN
AC_LIBTOOL_WIN32_DLL
AM_PROG_LIBTOOL
AC_PROG_INSTALL

AC_PATH_PROGS(PERL, perl5 perl, :)
AC_PATH_PROGS(SED, sed, :)
AC_SUBST(PERL) AC_SUBST(SED)

dnl AC_COMPILE_WARNINGS
AM_MAINTAINER_MODE
# _AM_ENABLE_MULTILIB      # broken in 2.13

dnl Checks for header files.
AC_CHECK_HEADERS( sys/types.h sys/ioctl.h fcntl.h sys/stat.h)
AC_CHECK_HEADERS( utime.h sys/time.h sys/utime.h )
AC_CHECK_HEADERS( process.h sys/process.h poll.h sys/poll.h sys/select.h)
AC_CHECK_HEADERS( sys/resource.h unistd.h pwd.h io.h libc.h dos.h  )
AC_CHECK_HEADERS( mem.h memory.h strings.h)
AC_CHECK_HEADERS( locale.h conio.h term.h termcap.h )
AC_CHECK_HEADERS( termios.h termio.h sgtty.h curses.h ncurses.h)
AC_CHECK_HEADERS( ltdl.h dlfcn.h dl.h vxWorks.h )
AC_CHECK_HEADERS( winbase.h wincon.h )
AC_CHECK_HEADERS( pthread.h pth.h )
AC_CHECK_HEADERS( math.h fenv.h )

AC_HEADER_DIRENT
AC_DECL_SYS_SIGLIST 
AC_FUNC_MKDIR

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_BIGENDIAN_CROSS
dnl ___P4_C_OLDCPP          # obsolete..
AX_C_DECLARE_BLOCK
AC_C_CONST
AC_C_INLINE
AC_C_STRINGIZE
AC_C_LONG_LONG_
ACX_C_RESTRICT
AC_COMPILE_CHECK_SIZEOF(char)
AC_COMPILE_CHECK_SIZEOF(short)
AC_COMPILE_CHECK_SIZEOF(int)
AC_COMPILE_CHECK_SIZEOF(long)
AC_COMPILE_CHECK_SIZEOF(void*)
AC_COMPILE_CHECK_SIZEOF(float)
AC_COMPILE_CHECK_SIZEOF(double)
dnl flat that we have been guessing sizeofs due to cross-compiling 
AC_COMPILE_CHECK_SIZEOF(unsigned)

dnl off_t checking
AC_TYPE_OFF_T
AC_SYS_LARGEFILE_SENSITIVE
dnl Checks for library functions.
AC_TYPE_SIGNAL
AC_FUNC_SNPRINTF
AC_CHECK_FUNC(sys_errlist)
AC_CHECK_FUNCS( random strerror)
AC_CHECK_FUNCS( strncasecmp strnicmp strlcpy strlcat )
AC_CHECK_FUNC( memmove, 
               AC_DEFINE(HAVE_MEMMOVE,, [memmove]),[ AC_CHECK_FUNC(bcopy)])
AC_CHECK_FUNCS( remove rename access getcwd stat fstat fileno )
AC_CHECK_FUNCS( truncate umask link getpid getuid getgid usleep )
AC_CHECK_FUNCS( select delay raise ospeed alarm system)
AC_CHECK_FUNCS( ftello fseeko )

dnl sigsetjmp is not a linkable function in many cases
AC_MSG_CHECKING( sigsetjmp in setjmp.h)
AC_EGREP_HEADER( sigsetjmp,   setjmp.h,[
               AC_MSG_RESULT(yes)
               AC_DEFINE([HAVE_SIGSETJMP],, [sigsetjmp in setjmp.h])],[
               AC_MSG_RESULT(no)])

AC_CHECK_LIB( m, pow )
AC_CHECK_FUNCS(acosh pow10 rint)

AC_CHECK_LIB( dl, dlopen,, 
   AC_CHECK_LIB( dld, shl_load ))

AC_CHECK_DEFINED(__STRICT_ANSI__,
  __strict_ansi__="-D_POSIX_SOURCE -D_XOPEN_SOURCE -D_XOPEN_SOURCE_EXTENDED"
  AC_MSG_RESULT(: adding CPPFLAGS $__strict_ansi__)
  CPPFLAGS="$CPPFLAGS $__strict_ansi__")

dnl libtool x.x ltdl.m4 has integrated the previous pfe specific macro
dnl it does now define config.h NEED_USCORE directly.
AC_LTDL_DLSYM_USCORE
AX_CHECK_GNU_DLADDR

dnl --------------------------------------------------------------------
dnl features list
PFEDEFS=""
AC_SUBST(PFE_DEFS)

GCC_VERSION=NONE
if test "$GCC" = "yes" ; then
 AC_MSG_CHECKING([gcc version])
 GCC_VERSION=`$CC --version 2>/dev/null | sed -e 's/^[[^0-9]]*//' -e q`
 AC_MSG_RESULT([$GCC_VERSION])
fi

AC_MSG_CHECKING([default --with-regs])
regs_def="no"
pfehints=""
if test "$GCC" = "yes" ; then
  case "$host,$GCC_VERSION" in
    i?86-*-*,2.*) regs_def="2" 
    pfehints="gcc2.x is a good choice, may be add --with-cflags=-mpentium" ;; 
    i?86-*-*,*) regs_def="1" 
    pfehints="gcc3.x is a bad choice, may be add --with-cflags=-mpentium" ;; 
    x86_64-*-*,*) regs_def="1" 
    pfehints="let us try with a regs_def of 1, yet unsure about the result" ;; 
    sparc*-*-solaris2.8,*) regs_def="no" 
    pfehints="--with-regs=1 on sparc works often and fast... but not always" ;;
    sparc|sparc32,*) regs_def="1" 
    pfehints="--with-regs=1 on sparc causes warnings but works... and fast";;
    ppc|powerpc,*) regs_def="all" 
    pfehints="powerpc is good enough for maximum --with-regs usage" ;;
    *) regs_def="no" 
    pfehints="untested processor architecture" ;;
  esac
fi
AC_MSG_RESULT([$regs_def ($pfehints)])

AC_ARG_ENABLE(all,
[  --enable-all            create multiple versions of pfe])

AC_ARG_WITH(termcap,
[  --with-termcap[=auto|yes|no|termcap|curses|ncurses|video] ])
AX_SUBST_WITH_ARG(termcap,auto)

AC_ARG_WITH(k12, 
[  --with-k12              use for k12xx environment])
AX_SUBST_WITH_ARG(k12,no)

AC_ARG_WITH(fig, 
[  --with-fig              use FIG-like word-header (and consequences)])
AX_SUBST_WITH_DEFINE_ARG(fig,no)

AC_ARG_WITH(chains, 
[  --with-chains           use chain wordset (default=no - in 33.x always)])
AX_SUBST_WITH_DEFINE_ARG(chains,no)

AC_ARG_WITH(spy, 
[  --with-spy              use spy wordset (default=no, k12xx=yes)])
AX_SUBST_WITH_DEFINE_ARG(spy,no)

AC_ARG_WITH(float,
[  --with-float            use floating wordset (default=yes)])
AX_SUBST_WITH_DEFINE_ARG(float,yes)

AC_ARG_WITH(regs,
[  --with-regs[=many]      use global regs for forth machine (gcc only)])
AC_COND_WITH_LEVEL_DEFINE(regs,$regs_def,many)

AC_ARG_WITH(opt,
[  --with-opt[=many]       set optimize level])
AC_COND_WITH_LEVEL(opt,some,many) 

AC_ARG_WITH(no-completion, 
[  --with-no-completion    do not compile the completion feature]) 
AX_SUBST_WITH_DEFINE(no-completion)
dnl PFE_DEFS="$PFE_DEFS $WITHDEF_NO_COMPLETION" FIXME:

AC_ARG_WITH(modules, 
[  --without-modules       do not compile the external modules (default=yes)]) 
AX_SUBST_WITH_DEFINE_ARG(modules,yes)

AC_ARG_WITH(static-dict, 
[  --with-static-dict      use a static dictionary]) 
AX_SUBST_WITH_DEFINE(static-dict)

AC_ARG_WITH(static-regs, 
[  --with-static-regs      use a static forth machine]) 
AX_SUBST_WITH_DEFINE(static-regs)

AC_ARG_WITH(nosmart, 
[  --with-nosmart          do not make code-adding words statesmart]) 
AX_SUBST_WITH_DEFINE(nosmart)

AC_ARG_WITH(call-threading, 
[  --with-call-threading   use call-threading and optimizations for it]) 
AX_SUBST_WITH_DEFINE(call-threading)

AC_ARG_WITH(sbr-call-threading, 
[  --with-sbr-call-threading try sbr-threading on top of call-threading]) 
AX_SUBST_WITH_DEFINE(sbr-call-threading)

AC_ARG_WITH(sbr-threading, 
[  --with-sbr-threading   or try sbr-threading with argument prefixing]) 
AX_SUBST_WITH_DEFINE(sbr-threading)

AC_ARG_WITH(cflags,
[  --with-cflags           add arbitrary options to CFLAGS])
WITH_CFLAGS=`echo $with_cflags | sed -e 's/&/ /' -e 's/,-/ -/'`
AC_SUBST(WITH_CFLAGS)

AC_ARG_WITH(user-config,
[  --with-user-config      e.g. float_input_on=0,caps_on=1
         user-config defaults:  grep -1 USER-CONFIG pfe/*.c | grep '#define'])
case "$with_user_config" in
  *=*)	USER_CONFIG=`echo ,$with_user_config | sed -e 'y:abcdefghijklmnopqrstuvwxyz:ABCDEFGHIJKLMNOPQRSTUVWXYZ:' -e 's/,/ -D/g'` ;;
  *)    USER_CONFIG=""  ;;
esac 
  AC_SUBST(USER_CONFIG)

AC_MSG_CHECKING(user-config)
case ".$USER_CONFIG" in
 .) AC_MSG_RESULT(none (of use_dollarhex stack_size order_len caps_on ...));;
 *) AC_MSG_RESULT($USER_CONFIG) ;;
esac

dnl --------------------------------------------------------------------
case "$with_k12" in
  offline-msc) RELEASE_INFO="$RELEASE_INFO-offline-msc" 
    AC_MSG_RESULT( changed release_info : $RELEASE_INFO)
#    if test "$with_termcap" = "auto" 
#    then with_termcap="k12-win"
         WITH_CFLAGS="$WITH_CFLAGS -I$srcdir/../MEAS_Include_K12xx"
	 WITH_CFLAGS="$WITH_CFLAGS -D_K12_OFFLINE"
#    fi
  ;;
  offline) RELEASE_INFO="$RELEASE_INFO-offline" 
    AC_MSG_RESULT( changed release_info : $RELEASE_INFO)
  ;;
  yes) RELEASEINFO="$RELEASEINFO-k12" 
    AC_MSG_RESULT( changed release_info : $RELEASE_INFO)
  ;;
esac

dnl --------------------------------------------------------------------
AC_MSG_CHECKING(which term characteristic should be used)
AC_MSG_RESULT([(can be set --with-termcap)])

termcap_tputs="" ; curses_tputs="" ; ncurses_tputs=""
curses_initscr="" ; ncurses_initscr="" ; video_gotoxy=""
termios_header="" ; ncurses_header="" ; curses_header="" ; sgtty_header="" 
AC_CHECK_HEADER(sgtty.h, [sgtty_header="term-lib"])
AC_CHECK_HEADER(termios.h, [termios_header="term-lib"])
AC_CHECK_HEADER(ncurses.h, [ncurses_header="term-curses"])
AC_CHECK_HEADER(curses.h, [curses_header="term-curses"])
AC_CHECK_LIB( termcap, tputs, [termcap_tputs="term-lib:-ltermcap"])
AC_CHECK_LIB( curses, tputs, [curses_tputs="term-lib:-lcurses"])
AC_CHECK_LIB( ncurses, tputs, [ncurses_tputs="term-lib:-lncurses"])
AC_CHECK_LIB( curses, initscr, [curses_initscr="term-curses:-lcurses"])
AC_CHECK_LIB( ncurses, initscr, [ncurses_initscr="term-curses:-lncurses"])
AC_CHECK_LIB( video, v_gotoxy, [video_gotoxy="term-emx:-bsd:-lvideo"])
term_lib="$ncurses_tputs,$curses_tputs,$termcap_tputs"
term_lib="$term_lib,$term_ios_header,$sgtty_header"
term_curses="$curses_initscr,$ncurses_initscr"
term_curses="$term_curses,$ncurses_header,$curses_header"
term_speclib="$video_gotoxy"

host_termdef=""
case $target_os in
      k12*)    host_termdef="term-k12" ;;
      os2*)    host_termdef="term-dj" ;;
      mingw*)  host_termdef="term-wincon:-luser32:-lkernel32" ;;
      cygwin*) host_termdef="term-lib:-lcygwin" ;;
      watcom*) host_termdef="term-wat" ;;
      *)       host_termdef="" ;;
esac

v=`echo $term_lib,none | sed -e 's/^,*//' -e 's/,.*//'`
AC_MSG_RESULT(term-lib: $v)
v=`echo $term_curses,none | sed -e 's/^,*//' -e 's/,.*//'`
AC_MSG_RESULT(term-curses: $v)
v=`echo $term_speclib,none | sed -e 's/^,*//' -e 's/,.*//'`
AC_MSG_RESULT(term-special-lib: $v)
v=`echo $host_termdef,none | sed -e 's/^,*//' -e 's/,.*//'`
AC_MSG_RESULT(term-host-specific: $v)

case "$with_termcap" in
   term-*) term_choose="$with_termcap" ;;
   yes|termcap) term_choose="$term_lib";;
   no) term_choose="$term_curses" ;;
   curses) term_choose="$ncurses_initscr,$curses_initscr,$curses_header," ;;
   ncurses) term_choose="$ncurses_initscr,$ncurses_tputs,$ncurses_header," ;;
   video) term_choose="$video_gotoxy,$ncurses_initscr,$curses_initscr," ;;
   auto)  term_choose="$host_termdef,$video_gotoxy," 
          if test -z "$termios_header" ; then
              term_choose="$term_choose,$term_curses,$term_lib,"
          else
              term_choose="$term_choose,$term_lib,$term_curses,"
          fi ;;
   *) AC_MSG_RESULT(WARNING: unknown option --with-termcap $with_termcap)
      term_choose="$with_termcap" ;;
esac
dnl AC_MSG_RESULT(choosing $with_termcap from $term_choose)
V=`echo $term_choose | sed -e 's/^,*//' -e 's/,.*//'`
DEF_TERM=`echo $V | sed -e 's/:.*//' -e 'y/:/ /'`
LIB_TERM=`echo $V | sed -e 's/^[[^:]]*//' -e 's/^:*//' -e 'y/:/ /'` 
case "$DEF_TERM" in
   term-wincon)  TERM_DEF="1" ;;
   term-dj)      TERM_DEF="2" ;;
   term-k12)     TERM_DEF="3" ;;
   term-emx)     TERM_DEF="4" ;;
   term-curses)  TERM_DEF="5" ;;
   term-lib)     TERM_DEF="6" ;;
   term-wat)     TERM_DEF="9" ;;
   *)            TERM_DEF="111" ;;
esac

case "$with_k12" in
  offline-msc) 
	LIB_TERM="$LIB_TERM -L../k12-emul"
	LIB_TERM="$LIB_TERM `sh ../k12-emul/k12-emul-config --libs`"
  ;;
esac

AC_SUBST(LIB_TERM) 
AC_DEFINE_UNQUOTED([TERM_DEF],[$TERM_DEF],dnl
[which terminal driver to use as default])
AC_MSG_RESULT(term characteristic $TERM_DEF : $DEF_TERM.. $LIB_TERM)

dnl --------------------------------------------------------------------
case "$program_suffix" in
  NONE) VARIANT= ;;
  -*-*) VARIANT=`echo "$program_suffix" | sed -e 's:-[[^-]]*$::'` ;;
  *) VARIANT="$program_suffix" ;;
esac
AC_SUBST([VARIANT])
case "$program_suffix" in
  -*)  RELEASE_INFO="$RELEASE_INFO$program_suffix"
       AC_MSG_RESULT( changed releaseinfo: $RELEASE_INFO )
  ;;
esac
dnl --------------------------------------------------------------------
dnl mostly win32 specific
AC_MSG_CHECKING(lib link options)
case "$host" in
*-*-cygwin* | *-*-mingw*)
 libpfe_la="libpfe.la" 
 DLLFLAGS="-no-undefined -mconsole -export-dynamic" 
 DLLFLAGS="$DLLFLAGS --export-all-symbols --no-inhibit-exec"
 DYNALINK="$DLLFLAGS"
 libpfe_VERSION="$VERSION_INFO"
# CFLAGS="$CFLAGS -DPFE_DLL_EXPORT"
 RESOLVES=' ' ;;
*-*-darwin*)
 libpfe_la=""
 libpfe_VERSION="$RELEASE_INFO $VERSION_INFO"
 DLLFLAGS=""
 DYNALINK=" # "
 RESOLVES=" # " ;;
*)
 libpfe_la=""  # in linux/solaris, don't do add it - a bug in libtool...
 libpfe_VERSION="$RELEASE_INFO $VERSION_INFO"
 DLLFLAGS=""
 DYNALINK=""
 RESOLVES=' # ' ;;
esac
AC_SUBST(libpfe_la)
AC_SUBST(libpfe_VERSION)
AC_SUBST(DLLFLAGS)
AC_SUBST(DYNALINK)
AC_SUBST(RESOLVES)
result=`echo "$DYNALINK $RESOLVES $libpfe_la" | sed -e "s/\\#/\\#(nolink)\\#/"`
AC_MSG_RESULT($result)
dnl --------------------------------------------------------------------
dnl some platforms (especially embedded) do not support dynamic linking
dnl or atleast pfe does not yet support it there. Here we can link the
dnl external sharedlib modules into the main pfe, using an "internal"
dnl preopen table that informs the LOADM-routine about the pre-linked
dnl modules. User-space scripts shall not be able to distinguish the case.

if test "$with_modules" = "no" ; then

  for module_c in \
    dstrings-ext.c \
    toolbelt-ext.c \
    gforth-ext.c \
    zchar-ext.c \
    edit-ext.c \
    struct-ext.c \
    help-ext.c 
  do if test -f "$srcdir/$module_c" ; then
	PFE_DEFS="$PFE_DEFS -D"`echo $PACKAGE-$module_c | $as_tr_cpp`
  fi done

  if test -d "$srcdir/../MEAS_Include_K12xx" ; then
     if test -f "$srcdir/../gensrc/enc_const.c" ; then
	PFE_DEFS="$PFE_DEFS -DPFE_K12_GENSRC_ENC_CONST_C -DPFE_SWIG_DUMMY"
	PFE_DEFS="$PFE_DEFS -I$srcdir/../MEAS_Include_K12xx"
     fi
     if test -f "$srcdir/../mforth/mforth-words.c" ; then
	PFE_DEFS="$PFE_DEFS -DPFE_K12_MFORTH_MFORTH_WORDS_C -DPFE_MFORTH_DUMMY"
     fi	
  fi
fi
dnl --------------------------------------------------------------------
# [LDFLAGS="$LDFLAGS -W,-E"] # export
AC_SUBST(LDFLAGS)
AX_CFLAGS_WARN_ALL
AX_CFLAGS_NO_WRITABLE_STRINGS
AX_CFLAGS_STRICT_PROTOTYPES
AX_CFLAGS_GCC_OPTION(-fno-strict-aliasing)
AX_CFLAGS_GCC_OPTION(-Wno-sign-compare)
dnl _AC_CHECK_CC_OPT(-pipe, cc_opt_pipe)
AX_CHECK_TLS_SUPPORT

EXTRAS=""
dnl -------------------------------------------------------------------
uses_sbr_threading="no"
test "$with_sbr_call_threading" = "yes" && uses_sbr_threading="yes"
test "$with_sbr_threading" = "yes" && uses_sbr_threading="yes"

GCC_WARN="" ; OPTIM="" ; DEBUG=""
AC_MSG_CHECKING(optimize level "$WITHVAL_OPT" is...)
CFLAGS=`echo " $CFLAGS " | sed -e 's/ -O[[0-9]]* / /'`
opt_level="$WITHVAL_OPT"
if test "$GCC" = "yes" ; then
   OPTIM="-O$opt_level"
   if test "$with_sbr_call_threading" != "yes" ; then
     AC_DEFINE(OMIT_FRAME_POINTER,1,
               [whether -fomit-frame-pointer was enabled during setup])
     CFLAGS="$CFLAGS -fomit-frame-pointer -D_OMIT_FRAME_POINTER"
     EXTRAS="$EXTRAS -fomit-frame-pointer -D_OMIT_FRAME_POINTER"
   fi
   DEBUG=" -W -Wshadow"
   LDFLAGS="$LDFLAGS -W,--warn-common" # -Wl,-S
   # GCC_WARN="$GCC_WARN -Wtraditional"
   # GCC_WARN="$GCC_WARN -Wconversion"
elif test ".$am_cv_CC_dependencies_compiler_type" = ".aix" ; then
   OPTIM="-O2 -D_AIX_CC"
   CFLAGS=`echo $CFLAGS | sed -e s:-qinfo=all:-qinfo=all,noext:`
elif test "$CC" = "c89" ; then
   use_regs=0
   if test -z "$opt_level" ; then opt_level="1"; fi
   OPTIM="+O$opt_level -D_HPUX_SOURCE" ; DEBUG="-D_HPUX_SOURCE"
else
   OPTIM="-O$opt_level" 
fi

case `pwd` in      # implicitly add -DNDEBUG if in path with Release
  */Release/*) OPTIM="-DNDEBUG $OPTIM"  ; libpfe_no_debug="yes" ;;
  */Debug/*)   DEBUG="-DDEBUG $DEBUG" ; libpfe_no_strip="yes" ;;
esac

AC_SUBST(OPTIM)
AC_SUBST(DEBUG)
AC_SUBST(GCC_WARN)
AC_MSG_RESULT($WITH_OPT $OPTIM)

test "_$USE_MAINTAINER_MODE" = "_no" && LIBTOOL="$LIBTOOL --silent"
dnl --------------------------------------------------------------------
case "$GCC,$host_cpu,$WITHVAL_REGS" in
  yes,i?86*,[[45678]]) 
      case "$GCC_VERSION" in
	3.0*) AC_MSG_ERROR(:can not use --with-regs along with gcc 3.0.x) ;;
	3.1*) AC_MSG_RESULT(:can not use --with-regs along with gcc 3.1.x) ;;
	3.*)  AC_MSG_RESULT(:can use --with-regs / -fno-builtin os-string.h) ;;
	2.9*) AC_MSG_RESULT(:can use --with-regs with traditional gcc) ;;
        *) AC_MSG_RESULT(:and use --with-regs on unknown gcc version) ;;
  esac ;;
esac
case "$GCC,$host" in
  yes,*darwin*)
     AC_MSG_RESULT(:and use -no-cpp-precomp) 
     CFLAGS="$CFLAGS -no-cpp-precomp" 
     EXTRAS="$EXTRAS -no-cpp-precomp" ;;
esac
dnl --------------------------------------------------------------------
AC_MSG_RESULT(cleaning LIBS from -lm occurrences)
LIBS=`echo " $LIBS " | sed -e "s/ -lm //g"`

PACKAGE_CFLAGS="$EXTRAS" # PACKAGE_LIBS = -lpfe
AX_CREATE_PKGCONFIG_INFO

if test "$GCC" = "yes" ; then if test "_$libpfe_no_debug" = "_yes" ; then
AC_MSG_RESULT(cleaning CFLAGS from -g occurrences)
 CFLAGS=`echo " $CFLAGS " | sed -e "s/ -g //g"`
fi fi
if test "_$libpfe_no_strip" = "_yes" ; then
AC_MSG_RESULT(cleaning LDFLAGS from -Wl,-S occurrences, i.e. --strip)
 LDFLAGS=`echo " $LDFLAGS " | sed -e 's/ -Wl,-S //'` 
fi 
if test "$enable_shared" = "yes" ; then
  AC_DEFINE(ENABLE_SHARED,1,[whether --enable-shared was used during setup])
fi

test ".$WITH_CFLAGS" != "." && CFLAGS="$CFLAGS \$(WITH_CFLAGS)"
test ".$USER_CONFIG" != "." && CFLAGS="$CFLAGS \$(USER_CONFIG)"
dnl --------------------------------------------------------------------

AC_DEFINE_PATH_STYLE
AC_DEFINE_DIR_([EPREFIX], [exec_prefix], [--exec-prefix or its default])
AC_DEFINE_DIR_([LIBDIR], [libdir], [--libdir or its default])
AC_DEFINE_DIR_([INCLUDEDIR],[includedir], [Header Files of package])
AC_DEFINE_UNQUOTED([PACKAGE], "$PACKAGE", [Name of package])
AC_DEFINE_UNQUOTED([VERSION], "$VERSION", [Version of package])
AC_DEFINE_UNQUOTED([VARIANT],"$VARIANT", [Variant suffix string for package])
AX_WARNING_DEFAULT_PKGCONFIGDIR
dnl --------------------------------------------------------------------
cp confdefs.h havedefs.h
AC_CREATE_TARGET_H(_target.h)
AC_CREATE_GENERIC_CONFIG
AC_OUTPUT(Makefile) 
dnl --------------------------------------------------------------------
PATCH_LIBTOOL_ON_DARWIN_ZSH_OVERQUOTING
PATCH_LIBTOOL_SYS_LIB_SEARCH_PATH_SPEC
PATCH_LIBTOOL_TO_ADD_HOST_CC
PATCH_LIBTOOL_ON_DARWIN_PASS_ALL
PATCH_LIBTOOL_CHANGING_CMDS_IFS
dnl --------------------------------------------------------------------
[
if test "$build" != "$host" ; then
   buildhost="(build on $host)"
else
   buildhost="(build local)"
fi

echo " ===== " $PACKAGE $VERSION $prefix $buildhost
sh ./libtool --features
   grep PFE_WITH_ _config.h \
	| grep -v ifndef \
	| sed -e '/^.define PFE_WITH_/s,^.define,config define,' \
	| sed -e '/^.*undef PFE_WITH_/s,[*][/],,' \
	| sed -e '/^.*undef PFE_WITH_/s,^.*undef ,config     no ,'

   echo "using   defs= $PFE_DEFS"
   x="$cc_opt_pipe $cc_opt_write_strings $cc_opt_strict_prototypes"
   echo "using cflags= $x $GCC_WARN $WITH_CFLAGS $USER_CONFIG"
   echo "using CFLAGS= $CFLAGS"
   echo "using LIBS=   $LIBS"
case $WITHVAL_REGS in
0) echo "    NO-regs   (no)   (Forth VM in memory, slowest)" ;;
1) echo " -with-regs   some (1) (Forth Thread in CPU reg / reentrant build)" ;;
2) echo " -with-regs   more (2) (Forth Thread & Stack Pointer in CPU regs)" ;;
3) echo " -with-regs   many (3) (Forth Important VM Pointers in CPU regs)" ;;
4) echo " -with-regs   many (4) (Forth Important VM Pointers in CPU regs)" ;;
5) echo " -with-regs   many (5) (Forth Important VM Pointers in CPU regs)" ;;
6) echo " -with-regs   many (6) (Forth Important VM Pointers in CPU regs)" ;;
7) echo " -with-regs   all  (try to have Forth VM completly in CPU regs)" ;;
?) echo " -with-regs   $WITHVAL_REGS (unknown state, numeric is o.k.)" ;;
*) echo " -with-regs   $WITHVAL_REGS (unknown state)" ;;
esac

test -n "$pfe_hints" && echo "hint: $pfe_hints"

test $with_k12 = "yes" && \
   echo " -with-k12    (yes)  (have special k12xx extras, esp. main & term)"
test $with_fig = "yes" && \
   echo " -with-fig    (yes)  (have special FIG-like headers activated)"

   echo "set DEF_TERM= $DEF_TERM (link with $LIB_TERM)"
]

