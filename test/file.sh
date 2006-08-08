#! /bin/sh
testcase="stack"

echo "
//d
/^\$/d
/^file access word set */!bNX
/^file access word set */{g;N;}
/\n==================== */{g;N;}
/\n0 0 \$/{g;N;}
/\n0 0 0 0 0 \$/{g;N;}
/\n0 0 0 \$/{g;N;}
/\n0 0 0 0 \$/{g;N;}
/\n0 0 0  0 0  0 0 0  0 0 0  0 0 0  0 \$/{g;N;}
/\n0 0 0 0 0 0 \$/d
:NX
/----------*/d
{p;d;}
" >$testcase.sed

sed -n -f $testcase.sed $* >$testcase.tmp
cnt=`cat $testcase.tmp | wc -w`
test "0" -eq "$cnt" || echo "$* : $cnt BAD lines"
test "0" -eq "$cnt"
exit
