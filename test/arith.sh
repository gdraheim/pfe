#! /bin/sh

cnt=`grep "BAD" $* | wc -l`
test "0" -eq "$cnt" || echo "$* : $cnt BAD lines"
test "0" -eq "$cnt" 
exit

