#! /bin/sh

if test -z "$srcdir"; then
  echo "srcdir=. (guess$srcdir)"
  srcdir="."
fi

WARNDIFFS="diff -uw"
DIFFS="diff -u"
SKIPS="queryapp"
SED="sed"

SCRIPTDIR="$srcdir/.."
PFE_DIR="../main/c"
PFE_EXE="$PFE_DIR/pfe$EXEEXT"
echo "# $PFE_EXE $PFE_CHECK_ARGS ... ($SCRIPTDIR)"

PFE_LIB_PATH_STRING="--lib-path-string=$PFE_DIR/.libs"
PFE_SCRIPTDIR_STRING="--inc-path-string=$SCRIPTDIR"
PFE_CHECK_ARGS="--bye --quiet $PFE_SCRIPTDIR_STRING $PFE_LIB_PATH_STRING"
# PFE_SCRIPTDIR_STRING="-I $SCRIPTDIR"

{  # either args are give on the commandline
   # or test elements are found in the SCRIPTDIR
   args=""
   for file in "$@"; do
      # echo "! $file" >&2
      echo "$file"
      args="x$args";
   done
   if test -z "$args"; then
      ls $SCRIPTDIR/test/*.4th
   fi
} | {
   OK="" ; BAD="" ; WARN="" ; SKIP=""
   while read f; do
      check=`basename $f .4th`
      testdir=`dirname $f`
      if echo ":$SKIPS:" | grep ":$check:" >/dev/null
      then
          echo "$check skipped (SKIPS=$SKIPS:)"
          SKIP="$SKIP $check"; continue
      fi

      #.....................................................
      # check by compare with "log" of an earlier run
      if test -r "$testdir/$check.test.ok"
      then
         echo -n "L" `basename $testdir`/$check
         if test -f $tests/$check.txt
         then
           cat $tests/$check.txt | {
              $PFE_EXE $PFE_CHECK_ARGS $f
           } >$check.out 2>&1
         else
           echo "" | {
              $PFE_EXE $PFE_CHECK_ARGS $f
           } >$check.out 2>&1
         fi

         ok="BAD" ; count=""
         for logfile in $testdir/$check.*.ok
         do
            if test -f "$logfile"
            then
               log=`basename "$logfile" .ok`
               $DIFFS "$logfile" $check.out > $log.diff

               if test -s $log.diff; then
                  $WARNDIFFS "$logfile" $check.out > $log.warn.diff
                  if test -s $log.warn.diff
                  then :
                  else
                      ok="$log.ok:WARN"
                  fi
               else
                   ok="$log.ok"
                   break # for logfile
               fi
            fi
            count="^$count"
         done
         echo ... "$ok $count"

         if test "$ok" = "BAD"
         then
            BAD="$BAD $check"
         else
            case "$ok" in
            *:WARN)
               WARN="$WARN $check"
            ;; *)
                OK="$OK $check"
            ;; esac
         fi
     #.....................................................
     # check by letting shell-script decide if it is right or wrong
     elif test -r "$testdir/$check.sh"
     then
        echo -n "C" "test/$check ...."
        if test "$SED" = ":"
        then
           SKIP="$SKIP $check"
        elif test -f $testdir/$check.txt
        then
            cat $testdir/$check.txt \
           |cat| $PFE_EXE $PFE_CHECK_ARGS $f >$check.out
        else
                 $PFE_EXE $PFE_CHECK_ARGS $f >$check.out
        fi

        if test `cat $check.out | wc -l ` = "0"
        then
           if grep "no .* wordset" $check.out
           then
              SKIP="$SKIP $check"
           else
              BAD="$BAD $check"
              echo "... nothing"
           fi
        else
           if $SHELL "$testdir/$check.sh" $check.out
           then
              OK="$OK $check"
           elif test "$?" = "11"
           then
              SKIP="$SKIP $check"
           else
              BAD="$BAD $check"
           fi
        fi
     fi
  done
  echo "______________________________________________________"
  if test -n "$SHOWBADTESTOUT"
  then
     for f in $BAD ...
     do
        if test -f "$f.out"
        then
           echo "==== $f"; cat $f.out
        fi
     done
  fi

  test -z "$OK"   || echo "OK   : $OK"
  test -z "$BAD"  || echo "BAD  : $BAD"
  test -z "$WARN" || echo "WARN : $WARN       (result differs in whitespace)"
  test -z "$SKIP" || echo "SKIP : $SKIP"
  test -z "$BAD" # let the check fail
}
