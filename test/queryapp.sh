#! /bin/sh
testcase="queryapp"

echo "
//d
s/^hello world//
s/^boo//
s/^oops//
s/^!//
{p;d;}
" >$testcase.sed

sed -n -f $testcase.sed $* >$testcase.tmp
cnt=`cat $testcase.tmp | wc -w`
test "0" -eq "$cnt" || echo "$* : $cnt BAD lines"
test "0" -eq "$cnt"
exit
