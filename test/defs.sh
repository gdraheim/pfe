#! /bin/sh
testcase="defs"

echo "
//d
/^\$/d
/^simple colon definitions.*/!bskip
/^simple colon definitions/{g;N;}
/\n0 0 0  0 0 0/{g;N;}
/\nconstants.*/{g;N;}
/\n   0 0 0 0 0 0.*/{g;N;}
:skip
/----------*/d
{p;d;}
" >$testcase.sed

sed -n -f $testcase.sed $* >$testcase.tmp
cnt=`cat $testcase.tmp | wc -w`
test "0" -eq "$cnt" || echo "$* : $cnt BAD lines"
test "0" -eq "$cnt"
exit
