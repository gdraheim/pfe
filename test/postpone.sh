#! /bin/sh
testcase="postpone"

echo "
/is redefined/d
//d
/testing postpone .*/d
/you should see this first./b postponeorder
/----------*/d
{p;d;}
:postponeorder
{N;s/you should see this first.//;}
s/you should see this later.//
{p;d;}
" >$testcase.sed

sed -n -f $testcase.sed $* >$testcase.tmp
cnt=`cat $testcase.tmp | wc -w`
test "0" -eq "$cnt" || echo "$* : $cnt BAD lines"
test "0" -eq "$cnt"
exit
