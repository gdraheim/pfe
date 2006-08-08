#! /bin/sh
testcase="wordsets"

echo "
/checking .*/D
/ ^/D
s/ CODE *missing //
s/ ;CODE *missing //
s/ ASSEMBLER *missing //
s/ EDITOR *missing //
s/ missing//
p
" >$testcase.sed

sed -n -f $testcase.sed $* >$testcase.tmp
cnt=`cat $testcase.tmp | wc -w`
test "0" -eq "$cnt" || echo "$* : $cnt BAD lines"
test "0" -eq "$cnt"
exit


