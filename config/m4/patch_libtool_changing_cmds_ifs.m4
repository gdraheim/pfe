dnl /usr/share/aclocal/guidod/patch_libtool_changing_cmds_ifs.m4
dnl @synopsis PATCH_LIBTOOL_CHANGING_CMDS_IFS
dnl
dnl Do a `grep "^[a-z]*_cmds=" libtool` - these are "subroutines"
dnl encoded by libtool.m4 into the generated libtool script. Since
dnl libtool assumes that there is no subroutine-facility in the shell
dnl invoked, these are not actually subroutines, but actually a "list
dnl of commands". This looks correct, but the command separator is not
dnl ";" - it is "~", the tilde character.
dnl
dnl Now, grep again, look for `grep 'IFS="~"' libtool` and see that
dnl libtool scripting uses a for-loop on the command-list, i.e for cmd
dnl in $some_cmds. This works correctly when the IFS was modified,
dnl where IFS stands for "input field separator" which is whitespace
dnl characters by default.
dnl
dnl The problem: I have some real-world filesystems where there are
dnl directories using "~" inside of them, to be more to the point, it
dnl is a change control management software that uses source
dnl repositories of the form "path/master/project~version/src" and
dnl libtool has the tendency to resolve any symlinks so that it will
dnl paste such path into the $_cmds script when it gets evaluated a
dnl number of times.
dnl
dnl This script is a workaround: I do not know why the ";" was not
dnl chosen as the IFS, perhaps it has some weird interactions in some
dnl shells since it is also the default record separator being one time
dnl bigger in context than the argument separator. I have made good
dnl success however with using "?" as the IFS, since there is no
dnl path-name that uses a question mark, and there is no _cmds ever
dnl around that uses "?" for some thing.
dnl
dnl Oh yes, there are some usages of "*" to match shell-wise at the
dnl output file of some tool, so that might have triggered the choice
dnl to not use "?" in the first place - but in real life it never
dnl occured that a _cmds script was created that has gone to use "?".
dnl And so, this ac-macro exchanges the s/~/?/g in configured _cmds
dnl variables and replaces all occurences of s/IFS="~"/IFS="?"/ - and
dnl it all works smooth now.
dnl
dnl @category Misc
dnl @author Guido U. Draheim <guidod@gmx.de>
dnl @version 2003-03-24
dnl @license GPLWithACException

AC_DEFUN([PATCH_LIBTOOL_CHANGING_CMDS_IFS],
[# patch libtool to change $_cmds IFS from ~ to ? character
if grep "^[[_$as_cr_letters]]_cmds=.*[[?]]" libtool >/dev/null; then
  AC_MSG_WARN(dnl
  [patching libtool skipped - _cmds already contain question marks])
else
 AC_MSG_RESULT([patching libtool to change cmds IFS from ~ to ?])
    test -f libtool.old || (mv libtool libtool.old && cp libtool.old libtool)
    sed -e "/^[[_$as_cr_letters]]*_cmds=/s/~/?/g" -e 's/IFS="~"/IFS="?"/g' \
        -e "s/IFS='~'/IFS='?'/g"    libtool > libtool.new
    (test -s libtool.new || rm libtool.new) 2>/dev/null
    test -f libtool.new && mv libtool.new libtool # not 2>/dev/null !!
    test -f libtool     || mv libtool.old libtool
fi
])

