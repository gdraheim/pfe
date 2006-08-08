dnl @synopsis AC_CREATE_TARGET_H [(HEADER-FILE [,PREFIX)]
dnl
dnl create the header-file and let it contain '#defines' for the target
dnl platform. This macro is used for libraries that have
dnl platform-specific quirks. Instead of inventing a target-specific
dnl target.h.in files, just let it create a header file from the
dnl definitions of AC_CANONICAL_SYSTEM and put only ifdef's in the
dnl installed header-files.
dnl
dnl  if the HEADER-FILE is absent, [target.h] is used. 
dnl  if the PREFIX is absent, [TARGET] is used. 
dnl  the prefix can be the packagename. (y:a-z-:A-Z_:)
dnl
dnl the defines look like...
dnl
dnl  #ifndef TARGET_CPU_M68K
dnl  #define TARGET_CPU_M68K "m68k"
dnl  #endif
dnl
dnl  #ifndef TARGET_OS_LINUX
dnl  #define TARGET_OS_LINUX "linux-gnu"
dnl  #endif
dnl
dnl  #ifndef TARGET_OS_TYPE                     /* the string itself */
dnl  #define TARGET_OS_TYPE "linux-gnu"
dnl  #endif
dnl
dnl detail: in the case of hppa1.1, the three idents "hppa1_1" "hppa1"
dnl and "hppa" are derived, for an m68k it just two, "m68k" and "m"
dnl
dnl the CREATE_TARGET_H__ variant is almost the same function, but
dnl everything is lowercased instead of uppercased, and there is a "__"
dnl in front of each prefix, so it looks like...
dnl
dnl  #ifndef __target_os_linux
dnl  #define __target_os_linux "linux-gnulibc2"
dnl  #endif
dnl
dnl  #ifndef __target_os__                     /* the string itself */
dnl  #define __target_os__ "linux-gnulibc2"
dnl  #endif
dnl
dnl  #ifndef __target_cpu_i586
dnl  #define __target_cpu_i586 "i586"
dnl  #endif
dnl
dnl  #ifndef __target_arch_i386
dnl  #define __target_arch_i386 "i386"
dnl  #endif
dnl
dnl  #ifndef __target_arch__                   /* cpu family arch */
dnl  #define __target_arch__ "i386"
dnl  #endif
dnl
dnl other differences: the default string-define is "__" insteadof
dnl "_TYPE"
dnl
dnl personally I prefer the second variant (which had been the first in
dnl the devprocess of this file but I assume people will often fallback
dnl to the primary variant presented herein).
dnl
dnl  NOTE: CREATE_TARGET_H does also fill HOST_OS-defines 
dnl functionality has been split over functions called CREATE_TARGET_H_UPPER
dnl CREATE_TARGET_H_LOWER CREATE_TARGET_HOST_UPPER CREATE_TARGET_HOST_LOWER 
dnl  CREATE_TARGET_H  uses CREATE_TARGET_H_UPPER CREATE_TARGET_HOST_UPPER 
dnl  CREATE_TARGET_H_ uses CREATE_TARGET_H_LOWER CREATE_TARGET_HOST_LOWER
dnl
dnl there is now a CREATE_PREFIX_TARGET_H in this file as a shorthand
dnl for PREFIX_CONFIG_H from a target.h file, however w/o the target.h
dnl ever created (the prefix is a bit different, since we add an extra
dnl -target- and -host-)
dnl
dnl @category Misc
dnl @author Guido U. Draheim <guidod@gmx.de>
dnl @version 2005-12-03
dnl @license GPLWithACException

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
changequote([, ])dnl
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
changequote([, ])dnl
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
ac_need_target_h_prefix=`echo ifelse($2, , host, $2) | sed -e 'y:abcdefghijklmnopqrstuvwxyz-:ABCDEFGHIJKLMNOPQRSTUVWXYZ_:' -e 's:[^A-Z0-9_]::g'`
#
host_os0=`echo "$host_os"  | sed -e 'y:abcdefghijklmnopqrstuvwxyz.-:ABCDEFGHIJKLMNOPQRSTUVWXYZ__:' -e 's:[^A-Z0-9_]::g'`
host_os1=`echo "$host_os0" | sed -e 's:\([^0-9]*\).*:\1:' `
host_os2=`echo "$host_os0" | sed -e 's:\([^_]*\).*:\1:' `
host_os3=`echo "$host_os2" | sed -e 's:\([^0-9]*\).*:\1:' `
#
host_cpu0=`echo "$host_cpu"  | sed -e 'y:abcdefghijklmnopqrstuvwxyz.-:ABCDEFGHIJKLMNOPQRSTUVWXYZ__:' -e 's:[^A-Z0-9]::g'`
host_cpu1=`echo "$host_cpu0" | sed -e 's:\([^0-9]*\).*:\1:' `
host_cpu2=`echo "$host_cpu0" | sed -e 's:\([^_]*\).*:\1:' `
host_cpu3=`echo "$host_cpu2" | sed -e 's:\([^0-9]*\).*:\1:' `
#
host_cpu_arch0=`echo "$host_cpu_arch" | sed -e 'y:abcdefghijklmnopqrstuvwxyz:ABCDEFGHIJKLMNOPQRSTUVWXYZ:'`
#
changequote([, ])dnl
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
changequote([, ])dnl
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
