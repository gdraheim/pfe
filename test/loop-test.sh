#! /bin/sh
testcase="loop-test"

echo "
/is redefined/d
//d
/loop zero-up/b zeroup
/loop two-up/b twoup
{p;d;}
:zeroup
{N;N;s/loop zero-up//;}
s/0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19//
s/0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19//
{p;d;}
:twoup
{N;N;s/loop two-up//;}
s/2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19//
s/2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19//
{p;d;}
" >$testcase.sed

sed -n -f $testcase.sed $* >$testcase.tmp
cnt=`cat $testcase.tmp | wc -w`
test "0" -eq "$cnt" || echo "$* : $cnt BAD lines"
test "0" -eq "$cnt"
exit

