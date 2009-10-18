#! /bin/sh

cnt=`grep "UNKNOWN" $* | grep -v "NONSENSE" | wc -l`
mod=`grep "UNKNOWN" $* | grep -v "NONSENSE" | grep -v "FLOATING" | wc -l`
mod_floating=`grep "FLOATING-EXT .* UNKNOWN" $* | wc -l`
if test "$mod_floating" = "1"  ; then
     echo "  no floating wordset available"
     test "3" -eq "$cnt" || echo "$* : $cnt BAD lines"
else test "0" -eq "$cnt" || echo "$* : $cnt BAD lines" ; fi
test "0" -eq "$cnt" && exit 0
test "0" -eq "$mod" && exit 11
exit 1
