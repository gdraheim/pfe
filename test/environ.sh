#! /bin/sh

cnt=`grep "UNKNOWN" $* | grep -v "NONSENSE" | wc -l`
test "0" -eq "$cnt" || echo "$* : $cnt BAD lines"
test "0" -eq "$cnt" 
exit

