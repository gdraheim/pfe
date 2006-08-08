dnl @synopsis AC_COND_WITH_LEVEL(PACKAGE [,DEFAULT [,YESLEVEL]])
dnl
dnl actually used after an AC_ARG_WITH(PKG,...) option-directive, where
dnl AC_ARG_WITH is a part of the standard autoconf to define a
dnl `configure` --with-PKG option.
dnl
dnl this macros works almost like AC_COND_WITH(PACKAGE [,DEFAULT]), but
dnl the definition is set to a numeric value. The level of a simple "yes"
dnl is given (or assumed "2" if absent), otherwise the level can be
dnl given in mnemonic names, being
dnl  1 = some
dnl  2 = many
dnl  3 = all
dnl  4 = extra
dnl  5 = extrasome more
dnl  6 = extramany muchmore much
dnl  7 = super everything
dnl  8 = ultra experimental
dnl  9 = insane
dnl
dnl this macro is most handily it making Makefile.in/Makefile.am that
dnl have a set of with-level declarations, espcially
dnl optimization-levels
dnl
dnl the $withval shell-variable is set for further examination (it
dnl carries the numeric value of the various mnemonics above)
dnl
dnl @category Misc
dnl @author Guido U. Draheim <guidod@gmx.de>
dnl @version 2003-10-29
dnl @license GPLWithACException

AC_DEFUN([AC_COND_WITH_LEVEL],
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
AC_DEFUN([AC_COND_WITH_LEVEL_DEFINE],
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
